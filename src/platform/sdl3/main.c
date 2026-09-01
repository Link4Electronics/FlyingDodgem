#include "platform/audio.h"
#include "platform/display.h"

#include "gdi32_compat.h"

#include <SDL3/SDL.h>

#include <stdio.h>
#include <stdlib.h>

#define VIEWPORT_WIDTH  640
#define VIEWPORT_HEIGHT 480

/* The DASHRES dashboard art ships as raw .dib files (resources 0x81..0x86,
 * see AGENTS.md). The asset directory is resolved in order: the first CLI
 * argument, the FLYINGDODGEM_DASHRES environment variable, then a "dashres"
 * subdirectory of the executable's directory, then the current directory. */
static const char *find_dashres_dir(int argc, char **argv)
{
    const char *env = getenv("FLYINGDODGEM_DASHRES");
    if (argc > 1)
        return argv[1];
    if (env != NULL)
        return env;

    {
        static char exe_dir[520];
        static char exe_sub[520];
        const char *base = NULL;
        size_t n = 0;

#if defined(SDL_PLATFORM_LINUX) || defined(SDL_PLATFORM_WINDOWS) || defined(SDL_PLATFORM_MACOS)
        if (SDL_GetBasePath() != NULL)
            base = SDL_GetBasePath();
#endif
        if (base != NULL)
        {
            (void)SDL_snprintf(exe_dir, sizeof(exe_dir), "%s", base);
            n = SDL_strlen(exe_dir);
            while (n > 0 && (exe_dir[n - 1] == '/' || exe_dir[n - 1] == '\\'))
                exe_dir[--n] = '\0';
            (void)SDL_snprintf(exe_sub, sizeof(exe_sub), "%s%sdashres",
                               exe_dir, "/");
            return exe_sub;
        }
    }
    return "dashres";
}

int main(int argc, char **argv)
{
    HDC screen = NULL;
    HDC dash = NULL;
    HINSTANCE dashmod = NULL;

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
        const char *dashdir = find_dashres_dir(argc, argv);
        printf("FlyingDodgem: loading dashboard assets from '%s'\n", dashdir);

        screen = GdiCompat_GetScreenDC(VIEWPORT_WIDTH, VIEWPORT_HEIGHT, NULL, 0);
        dashmod = LoadLibraryA(dashdir);
        if (dashmod != NULL)
        {
            HRSRC res = FindResourceA(dashmod, (const char *)MAKEINTRESOURCEA(0x86),
                                      (const char *)RT_BITMAP);
            if (res != NULL)
            {
                u32 size = SizeofResource(dashmod, res);
                HGLOBAL glb = LoadResource(dashmod, res);
                const void *dib = (glb != NULL) ? LockResource(glb) : NULL;
                if (dib != NULL && size >= 40)
                {
                    dash = GdiCompat_CreateDibSurface(dib);
                    if (dash == NULL)
                        SDL_Log("FlyingDodgem: failed to build dashboard surface");
                }
                else
                {
                    SDL_Log("FlyingDodgem: dashboard resource unusable (size %u)", size);
                }
            }
            else
            {
                SDL_Log("FlyingDodgem: could not find dashboard resource 0x86");
            }
        }
        else
        {
            SDL_Log("FlyingDodgem: could not load DASHRES from '%s'", dashdir);
        }
    }
#endif

    bool running = true;
    while (running)
    {
        running = PlatDisplay_PollEvents(display);
        if (!running)
            break;

#if defined(FLYINGDODGEM_GDI_COMPAT)
        if (dash != NULL)
        {
            /* Compositing step replicating the original: blit the loaded DIB
             * section (INDEX8) into the screen DC, then present it. */
            BitBlt(screen, 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT,
                   dash, 0, 0, GDI_SRCCOPY);
        }
        GdiCompat_Present(screen, back, pitch, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
#endif

        PlatDisplay_Present(display);
        SDL_Delay(16);
    }

#if defined(FLYINGDODGEM_GDI_COMPAT)
    if (dash != NULL)
        DeleteObject((HGDIOBJ)dash);
    if (dashmod != NULL)
        FreeLibrary(dashmod);
#endif

    PlatDisplay_Close(display);
    PlatAudio_Quit();
    SDL_Quit();
    return 0;
}
