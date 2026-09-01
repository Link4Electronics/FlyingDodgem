#include "platform/audio.h"
#include "platform/display.h"

#include "gdi32_compat.h"
#include "resources.h"

#include <SDL3/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define VIEWPORT_WIDTH  640
#define VIEWPORT_HEIGHT 480

/* The DASHRES dashboard set: mip variants of the cockpit dashboard, loaded by
 * the retail DASH loader (HOVER.EXE 0x410518) via
 * FindResourceCompat(0x410534-ish)/FindResourceA. Sizes ascend 320x149 ..
 * 700x325; the 640-wide 0x85 mip fills the 640x480 viewport width exactly. */
#define DASHRES_FIRST_ID 0x81
#define DASHRES_COUNT    6

#define DASH_BLACK_INDEX 255u /* every DASH palette maps index 255 to pure black */

/* The DASH assets live in DASHRES.DLL (a resource-only PE DLL of RT_BITMAP
 * 8-bpp DIBs). The asset location is resolved in order: the first CLI
 * argument, the FLYINGDODGEM_DASHRES environment variable, the executable's
 * directory, a "dashres" subdirectory of it, then the current directory.
 * Each candidate may be the DLL file itself or a directory containing it
 * (or a directory of legacy dash_XX.dib files); the compat LoadLibraryA
 * handles all three shapes. Returns the first existing candidate, or NULL. */
static bool path_exists(const char *p)
{
    struct stat st;
    return stat(p, &st) == 0;
}

static const char *find_dashres(int argc, char **argv)
{
    const char *exe_base = NULL;

    if (argc > 1 && path_exists(argv[1]))
        return argv[1];
    {
        const char *env = getenv("FLYINGDODGEM_DASHRES");
        if (env != NULL && path_exists(env))
            return env;
    }

#if defined(SDL_PLATFORM_LINUX) || defined(SDL_PLATFORM_WINDOWS) || defined(SDL_PLATFORM_MACOS)
    exe_base = SDL_GetBasePath();
#endif
    if (exe_base != NULL)
    {
        static char exe_dir[520];
        static char exe_dll[520];
        static char exe_sub_dll[520];
        static char exe_sub[520];
        (void)SDL_snprintf(exe_dir, sizeof(exe_dir), "%s", exe_base);
        (void)SDL_snprintf(exe_dll, sizeof(exe_dll), "%sDASHRES.DLL", exe_dir);
        (void)SDL_snprintf(exe_sub_dll, sizeof(exe_sub_dll), "%sdashres/DASHRES.DLL",
                           exe_dir);
        (void)SDL_snprintf(exe_sub, sizeof(exe_sub), "%sdashres", exe_dir);
        if (path_exists(exe_dll))
            return exe_dll;
        if (path_exists(exe_sub_dll))
            return exe_sub;
        if (path_exists(exe_sub))
            return exe_sub;
    }

    if (path_exists("DASHRES.DLL"))
        return "DASHRES.DLL";
    if (path_exists("dashres"))
        return "dashres";
    return NULL;
}

typedef struct DashMip
{
    HDC surface;
    const void *dib;
    u32 width;
    u32 height;
} DashMip;

int main(int argc, char **argv)
{
    HDC screen = NULL;
    HINSTANCE dashmod = NULL;
    DashMip dash[DASHRES_COUNT];
    u32 dashCount = 0;
    u32 selected = DASHRES_COUNT; /* mip index (0x81..0x86); set below */

    SDL_zero(dash);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
        return 1;

    PlatAudio_Init();

    PlatDisplayDesc desc;
    SDL_zero(desc);
    desc.width = VIEWPORT_WIDTH;
    desc.height = VIEWPORT_HEIGHT;
    desc.window_width = VIEWPORT_WIDTH;
    desc.window_height = VIEWPORT_HEIGHT;
    desc.title = "FlyingDodgem";

    PlatDisplay *display = PlatDisplay_Open(&desc);
    if (display == NULL)
    {
        SDL_Log("FlyingDodgem: failed to open display");
        PlatAudio_Quit();
        SDL_Quit();
        return 1;
    }

    u32 pitch = 0;
    u8 *back = PlatDisplay_GetBackBuffer(display, &pitch);
    if (back == NULL)
    {
        SDL_Log("FlyingDodgem: failed to lock back buffer");
        PlatDisplay_Close(display);
        PlatAudio_Quit();
        SDL_Quit();
        return 1;
    }

#if defined(FLYINGDODGEM_GDI_COMPAT)
    {
        const char *dashres = find_dashres(argc, argv);
        /* Retail HOVER sets these from GetVersion; on our host pretend to be
         * a post-3.50 (Win95-class) system so the Win32s high-bit gate inside
         * the matched FindResourceCompat (0x453352) never rejects valid
         * resource handles on 64-bit heap pointers. */
        g_os_version = 0x035fU;
        g_os_win32s = 0;
        if (dashres == NULL)
            SDL_Log("FlyingDodgem: no DASHRES.DLL/dashres located");
        else
            printf("FlyingDodgem: loading dashboard assets from '%s'\n",
                   dashres);

        screen = GdiCompat_GetScreenDC(VIEWPORT_WIDTH, VIEWPORT_HEIGHT, NULL, 0);
        dashmod = (dashres != NULL) ? LoadLibraryA(dashres) : NULL;
        if (dashmod != NULL)
        {
            u32 i;
            u32 id;
            /* Load the whole dashboard mip set like the retail DASH loader
             * (0x410518 loops 'size' over ids 0x81..0x86). FindResourceCompat
             * is the matched 0x453352 wrapper: it forwards the lookup to
             * FindResourceA and applies the Win32s version gate. */
            for (id = 0; id < DASHRES_COUNT; id++)
            {
                HRSRC res = FindResourceCompat(
                    dashmod,
                    (const char *)MAKEINTRESOURCEA(DASHRES_FIRST_ID + id),
                    (const char *)RT_BITMAP);
                if (res == NULL)
                {
                    SDL_Log("FlyingDodgem: could not find dashboard resource 0x%x",
                            DASHRES_FIRST_ID + id);
                    continue;
                }
                u32 size = SizeofResource(dashmod, res);
                HGLOBAL glb = LoadResource(dashmod, res);
                const void *dib = (glb != NULL) ? LockResource(glb) : NULL;
                if (dib == NULL || size < 40)
                {
                    SDL_Log("FlyingDodgem: dashboard resource 0x%x unusable (size %u)",
                            DASHRES_FIRST_ID + id, size);
                    continue;
                }
                HDC s = GdiCompat_CreateDibSurface(dib);
                if (s == NULL)
                {
                    SDL_Log("FlyingDodgem: failed to build dashboard surface 0x%x",
                            DASHRES_FIRST_ID + id);
                    continue;
                }
                dash[dashCount].surface = s;
                dash[dashCount].dib = dib;

                {
                    const u8 *hdr = (const u8 *)dib;
                    s32 w = (s32)(hdr[4] | (hdr[5] << 8) | (hdr[6] << 16) | (hdr[7] << 24));
                    s32 h = (s32)(hdr[8] | (hdr[9] << 8) | (hdr[10] << 16) | (hdr[11] << 24));
                    dash[dashCount].width = (w < 0) ? (u32)(-w) : (u32)w;
                    dash[dashCount].height = (h < 0) ? (u32)(-h) : (u32)h;
                }
                printf("FlyingDodgem: dashboard 0x%x loaded (%ux%u)\n",
                       DASHRES_FIRST_ID + id,
                       dash[dashCount].width, dash[dashCount].height);
                dashCount++;
            }

            /* Default view: the 640-wide mip (0x85), the only one that fills
             * the 640x480 viewport width at native size; bottom-anchored like
             * a cockpit panel. Apply its own (quantized) palette. */
            for (i = 0; i < dashCount; i++)
                if (dash[i].width == VIEWPORT_WIDTH)
                    selected = i;
            if (selected < dashCount)
                GdiCompat_SetDibPalette(dash[selected].dib);
        }
        else
        {
            SDL_Log("FlyingDodgem: could not load DASHRES from '%s'", dashres);
        }
    }
#endif

bool running = true;
HBRUSH black = CreateSolidBrush(DASH_BLACK_INDEX);
bool captured = false;
while (running)
{
        running = PlatDisplay_PollEvents(display);
        if (!running)
            break;

#if defined(FLYINGDODGEM_GDI_COMPAT)
        if (selected < dashCount)
        {
            /* Clear to the DASH palette's pure black, then blit the selected
             * mip at native size, centred horizontally and anchored to the
             * bottom edge. No stretching: the raw paint fills the surrounding
             * frame instead of smearing the bitmap like a full-screen blit. */
            GDI_RECT full = { 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT };
            FillRect(screen, &full, black);

            s32 dx = ((s32)VIEWPORT_WIDTH - (s32)dash[selected].width) / 2;
            s32 dy = (s32)VIEWPORT_HEIGHT - (s32)dash[selected].height;
            BitBlt(screen, dx, dy, (s32)dash[selected].width,
                   (s32)dash[selected].height, dash[selected].surface, 0, 0,
                   GDI_SRCCOPY);
        }
        GdiCompat_Present(screen, back, pitch, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
        if (getenv("FLYINGDODGEM_CAPTURE") != NULL && !captured)
        {
            FILE *f = fopen("flyingdodgem.ppm", "wb");
            if (f != NULL)
            {
                fprintf(f, "P6\n%u %u\n255\n", VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
                u32 y;
                for (y = 0; y < VIEWPORT_HEIGHT; y++)
                {
                    const u8 *row = back + (u64)y * pitch;
                    u32 x;
                    for (x = 0; x < VIEWPORT_WIDTH; x++)
                    {
                        fputc(row[x * 4 + 0], f);
                        fputc(row[x * 4 + 1], f);
                        fputc(row[x * 4 + 2], f);
                    }
                }
                fclose(f);
                printf("FlyingDodgem: captured flyingdodgem.ppm\n");
            }
            captured = true;
        }
#endif

        PlatDisplay_Present(display);
        SDL_Delay(16);
    }

#if defined(FLYINGDODGEM_GDI_COMPAT)
    if (black != NULL)
        DeleteObject((HGDIOBJ)black);
    {
        u32 i;
        for (i = 0; i < dashCount; i++)
        {
            if (dash[i].surface != NULL)
                DeleteObject((HGDIOBJ)dash[i].surface);
        }
    }
    if (dashmod != NULL)
        FreeLibrary(dashmod);
#endif

    PlatDisplay_Close(display);
    PlatAudio_Quit();
    SDL_Quit();
    return 0;
}