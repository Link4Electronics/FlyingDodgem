#include "platform/audio.h"
#include "platform/display.h"

#include "app.h"
#include "gameframe.h"
#include "gdi32_compat.h"
#include "resources.h"
#include "sine_native.h"
#include "texmaze.h"

#if defined(FLYINGDODGEM_GDI_COMPAT)
#include "dashhud.h"
#include "display_native.h"
#include "win32_compat.h"
#include "winmm_compat.h"
#endif

#include <SDL3/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

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

/* The 48-byte game object initialized by Init22c80 (HOVER.EXE 0x422c80).
 * Retail constructs a whole run of these (base + 0x2e0, +0x310, +0x340, ...)
 * and never reads the constructor's return value. The matched recompilation
 * is __fastcall void Init22c80(void* self); the trailing `mov eax,ecx; ret`
 * (return self) lives in the separate Self22CA8 symbol. */
typedef struct Init22Object
{
    u32 f00;
    u32 f04;
    u32 f08;
    u32 f0c;
    u32 f10;
    u32 f14;
    u32 f18;
    u32 f1c;
    u32 f20;
    u32 f24;
    u32 f28;
    u32 f2c;
} Init22Object;

/* FUNCTION: HOVER 0x00422c80
 * __fastcall void Init22c80(void* self). Defined in stubs.c. */
extern void __fastcall Init22c80(void *self);

/* Locate and parse the retail maze/texture assets to exercise the
 * reconstructed TEX/MAZ loaders (texmaze.c). Search order: the
 * FLYINGDODGEM_MAZES environment variable, then build/MAZES and the
 * usual in-tree layouts. The menu maze (SMALL.MAZ) and its textures
 * (SMALL.TEX) are retained for the 3D pipeline; the rest are loaded,
 * validated and released again. */
#if defined(FLYINGDODGEM_GDI_COMPAT)
static TexArchive g_menuTex;
static int g_menuTexOk = 0;
static MazDoc g_menuMaz;
static int g_menuMazOk = 0;
/* First playable maze (MAZE1), retained for IDM_STARTGAME. */
static MazDoc g_level1Maz;
static int g_level1MazOk = 0;
#endif

static void load_maze_assets(void)
{
    const char *dirs[5];
    static const char *files[] = { "SMALL.TEX", "TEXT1.TEX",
                                   "SMALL.MAZ", "MAZE1.MAZ" };
    int ndirs = 0;
    int fi;

    dirs[ndirs++] = getenv("FLYINGDODGEM_MAZES");
    dirs[ndirs++] = "build/MAZES";
    dirs[ndirs++] = "../build/MAZES";
    dirs[ndirs++] = "MAZES";
    dirs[ndirs++] = "mazes";

    for (fi = 0; fi < 4; fi++)
    {
        const char *ext = strrchr(files[fi], '.');
        int is_tex = (ext != NULL && strcmp(ext, ".TEX") == 0);
        int di;
        int done = 0;

        for (di = 0; di < ndirs && !done; di++)
        {
            char path[512];

            if (dirs[di] == NULL)
                continue;
            SDL_snprintf(path, sizeof(path), "%s/%s", dirs[di], files[fi]);
            if (is_tex)
            {
#if defined(FLYINGDODGEM_GDI_COMPAT)
                /* Retain the menu maze's textures for wall colours. */
                if (strcmp(files[fi], "SMALL.TEX") == 0 && !g_menuTexOk)
                {
                    if (Tex_Load(path, &g_menuTex))
                    {
                        g_menuTexOk = 1;
                        printf("FlyingDodgem: %s: %u textures (retained)\n",
                               path, g_menuTex.count);
                        done = 1;
                    }
                }
#endif
                TexArchive tex;
                if (Tex_Load(path, &tex))
                {
                    u16 i;
                    u32 frames = 0;
                    for (i = 0; i < tex.count; i++)
                        frames += tex.textures[i].frame_count;
                    printf("FlyingDodgem: %s: %u textures, %u frames\n",
                           path, tex.count, frames);
                    Tex_Free(&tex);
                    done = 1;
                }
            }
            else
            {
#if defined(FLYINGDODGEM_GDI_COMPAT)
                /* Retain the menu maze for the 3D wall pass, and MAZE1
                 * for the F2 Start Game path (IDM_STARTGAME). */
                if (strcmp(files[fi], "SMALL.MAZ") == 0 && !g_menuMazOk)
                {
                    if (Maz_Load(path, &g_menuMaz))
                    {
                        g_menuMazOk = 1;
                        printf("FlyingDodgem: %s: objs %u/%u/%u/%u (retained)\n",
                               path, g_menuMaz.arrays[0].count,
                               g_menuMaz.arrays[1].count,
                               g_menuMaz.arrays[2].count,
                               g_menuMaz.arrays[3].count);
                        done = 1;
                    }
                }
                else if (strcmp(files[fi], "MAZE1.MAZ") == 0 && !g_level1MazOk)
                {
                    if (Maz_Load(path, &g_level1Maz))
                    {
                        g_level1MazOk = 1;
                        printf("FlyingDodgem: %s: objs %u/%u/%u/%u (retained)\n",
                               path, g_level1Maz.arrays[0].count,
                               g_level1Maz.arrays[1].count,
                               g_level1Maz.arrays[2].count,
                               g_level1Maz.arrays[3].count);
                        done = 1;
                    }
                }
                else
                {
                    MazDoc maz;
                    if (Maz_Load(path, &maz))
                    {
                        printf("FlyingDodgem: %s: objs %u/%u/%u/%u\n", path,
                               maz.arrays[0].count, maz.arrays[1].count,
                               maz.arrays[2].count, maz.arrays[3].count);
                        Maz_Free(&maz);
                        done = 1;
                    }
                }
#else
                MazDoc maz;
                if (Maz_Load(path, &maz))
                {
                    printf("FlyingDodgem: %s: objs %u/%u/%u/%u\n", path,
                           maz.arrays[0].count, maz.arrays[1].count,
                           maz.arrays[2].count, maz.arrays[3].count);
                    Maz_Free(&maz);
                    done = 1;
                }
#endif
            }
        }
        if (!done)
            SDL_Log("FlyingDodgem: could not locate %s", files[fi]);
    }
#if defined(FLYINGDODGEM_GDI_COMPAT)
    if (g_menuMazOk)
        DisplayDriver_SetMenuMaze(&g_menuMaz, g_menuTexOk ? &g_menuTex : NULL);
#endif
}

#if defined(FLYINGDODGEM_GDI_COMPAT)
/* Locate SOUNDS/MIXED (retail builds "%sSounds\\Mixed\\%s.WAV" for every
 * CSound name). Search mirrors the maze-asset order.  Returns 1 and fills
 * `out` with the MIXED directory. */
static int find_sounds_mixed(char *out, size_t outSize)
{
    static const char *dirs[] = {
        NULL, /* FLYINGDODGEM_SOUNDS, filled below */
        "build/SOUNDS/MIXED",
        "../build/SOUNDS/MIXED",
        "SOUNDS/MIXED",
        "sounds/mixed",
        "build/SOUNDS",
        "SOUNDS",
    };
    size_t i;

    dirs[0] = getenv("FLYINGDODGEM_SOUNDS");
    for (i = 0; i < sizeof(dirs) / sizeof(dirs[0]); i++)
    {
        char path[512];
        if (dirs[i] == NULL)
            continue;
        SDL_snprintf(path, sizeof(path), "%s/LEVLSTRT.WAV", dirs[i]);
        if (path_exists(path))
        {
            SDL_snprintf(out, outSize, "%s", dirs[i]);
            return 1;
        }
        /* Also accept a SOUNDS root that contains MIXED/. */
        SDL_snprintf(path, sizeof(path), "%s/MIXED/LEVLSTRT.WAV", dirs[i]);
        if (path_exists(path))
        {
            SDL_snprintf(out, outSize, "%s/MIXED", dirs[i]);
            return 1;
        }
    }
    return 0;
}

/* Set once IDM_STARTGAME has put the harness into retail game state 3. */
static int g_inMaze;
static int g_engineBand = -1;

/* Retail engine-band selector (0x409180 / play sites 0x40927e..0x4092d4):
 * throttle held picks eng_low/norm/high (bands 1..3) from the car's speed
 * fields; throttle released is eng_idle (band 0).  The keyboard path is
 * GetAsyncKeyState (0x408bf5) on the reboundable throttle keys; the default
 * accelerate binding is the Up arrow.  Band 2 = eng_norm (sounds[0x1c]).
 *
 * The dashboard gauges are deliberately NOT driven here. Retail feeds each
 * widget's lit count from live game-state fields every frame (HOVER.EXE
 * 0x417f60 clamps obj+0x2fc/+0x32c/+0x35c/+0x38c/+0x3bc/+0x3ec/+0x41c into
 * widget+0x24 for 0x2e0/0x310/0x340/0x370/0x3a0/0x3d0/0x400), and the three
 * small bars (0x3a0/0x400/0x3d0) are powerup gauges, not steering meters:
 * wiring them to the left/right keys invents behaviour retail does not
 * have. Until the car/powerup state is matched, every gauge keeps its
 * retail boot value (0, all segments dark). */
static void update_game_state(void)
{
    u32 band;
    s16 up;

    if (!g_inMaze)
        return;

    up = GetAsyncKeyState(VK_UP);

    if ((up & 0x8000) != 0)
        band = 2; /* eng_norm */
    else
        band = 0; /* eng_idle */
    if ((int)band != g_engineBand && WinmmPlayEngineBand(band, NULL))
    {
        static const char *name[4] = {
            "ENG_IDLE", "ENG_LOW", "ENG_NORM", "ENG_HIGH"
        };
        printf("FlyingDodgem: engine band %u -> %s\n", band, name[band]);
        g_engineBand = (int)band;
    }
    /* Native seam: thrust gauge (0x2e0, 25 segs, gauge 0) follows the
     * throttle like the engine band above. Retail feeds it from car state
     * (0x417f60 obj+0x2fc); without matched car physics this binary
     * approximation keeps the widget live. Other gauges stay at boot 0. */
    DashHud_SetLevel(0, ((up & 0x8000) != 0) ? 25 : 0);
}

/* Retail IDM_STARTGAME (menu id 0x8009, F2 accelerator): enter the first
 * maze and play the levlstrt sting (sound index 1; retail tick at
 * 0x408d26 calls CSound on sounds[1] when the level-start counter hits
 * 0x28).  Once the car is live (retail game state 3) the engine band
 * selector at 0x40927e keeps CSound on eng_idle (index 0x1a) while the
 * throttle is at 0; we start that loop here so a fresh maze begins with
 * the idle engine hum under the sting. */
static void start_game(void)
{
    char mixed[512];
    char wav[512];
    int haveMixed;

    if (g_level1MazOk)
    {
        DisplayDriver_SetMenuMaze(&g_level1Maz,
                                  g_menuTexOk ? &g_menuTex : NULL);
        printf("FlyingDodgem: Start Game -> MAZE1\n");
    }
    else
    {
        SDL_Log("FlyingDodgem: Start Game but MAZE1.MAZ was not retained");
    }

    haveMixed = find_sounds_mixed(mixed, sizeof(mixed));
    if (haveMixed)
    {
        SDL_snprintf(wav, sizeof(wav), "%s/LEVLSTRT.WAV", mixed);
        if (sndPlaySoundA(wav, SND_ASYNC))
            printf("FlyingDodgem: playing %s\n", wav);
        else
            SDL_Log("FlyingDodgem: sndPlaySoundA failed for %s", wav);

        /* Throttle band 0 = eng_idle loop (retail CSound play args
         * (5, 5, 0xffff) on sounds[0x1a] while state==3). */
        if (WinmmPlayEngineBand(0, mixed))
        {
            printf("FlyingDodgem: engine idle loop (%s/ENG_IDLE.WAV)\n",
                   mixed);
            g_engineBand = 0;
        }
        else
            SDL_Log("FlyingDodgem: failed to start eng_idle");
        g_inMaze = 1;
    }
    else
    {
        SDL_Log("FlyingDodgem: SOUNDS/MIXED not found");
    }
}
#endif

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

    /* Construct CBumperApp (g_pApp) and run the document-template half of
     * InitInstance so CWorld is published into g_460970 before the display
     * draw dispatch.  A C++ static initializer in app.c also does this; the
     * explicit call is idempotent and keeps the ordering obvious for the
     * native harness (retail does the same work from 0x426546 before
     * WinMain). */
    App_Bootstrap();

    /* Retail generates the fixed-point sine table at startup into BSS
     * (0x4aca58 carries no file bytes); without it every sine/cosine read
     * is 0 and the minimap rotation collapses. Must run before any frame,
     * display, or audio work that can touch trig. */
    SineNative_Init();

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
    u8 *back = NULL;

    /* Drive the matched Init22c80 constructor (0x422c80) through the native
     * pipeline: retail builds a run of these 48-byte sub-objects (base + 0x2e0,
     * +0x310, ...) and ignores the return value. */
    {
        Init22Object widget;
        Init22c80(&widget);
        if (widget.f08 != 10 || widget.f0c != 10 || widget.f1c != 1 ||
            widget.f00 != 0 || widget.f24 != 0 || widget.f2c != 0)
            SDL_Log("FlyingDodgem: Init22c80 initialized unexpected fields");
        else
            printf("FlyingDodgem: Init22c80 built 10x10 widget\n");
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
        /* Start DisplayEntry41ece0 on a worker thread with case 3 so the
         * DrawEntry/F406740 rasterizer paints into the shared INDEX8 bits
         * while the main loop composites the dashboard and presents. */
        if (screen != NULL)
            DisplayDriver_Init(screen);
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

    load_maze_assets();

    bool running = true;
    while (running)
    {
        running = PlatDisplay_PollEvents(display);
        if (!running)
            break;

#if defined(FLYINGDODGEM_GDI_COMPAT)
        /* Win32 main-loop shape: drain queued input through Translate +
         * Dispatch so key/mouse messages reach any retail consumer
         * (pump.c).  Wndprocs land in DefWindowProcA until matched.
         *
         * Retail turns F2 into WM_COMMAND IDM_STARTGAME via the accelerator
         * table (menu id 128) before Dispatch; we have no matched wndproc
         * yet, so synthesise the same command here and handle it. */
        {
            GdiMsg msg;
            int drain = 256;
            while (drain-- > 0 &&
                   PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_KEYDOWN && msg.wParam == VK_F2)
                {
                    msg.message = WM_COMMAND;
                    msg.wParam = IDM_STARTGAME;
                    msg.lParam = 0;
                }
                if (msg.message == WM_COMMAND &&
                    (msg.wParam & 0xffffU) == IDM_STARTGAME)
                {
                    start_game();
                    continue;
                }
                TranslateMessage(&msg);
                DispatchMessageA(msg.hwnd, msg.message, msg.wParam,
                                 msg.lParam);
                if (msg.message == WM_QUIT)
                {
                    running = false;
                    break;
                }
            }
            if (!running)
                break;
        }

        /* Throttle/steer -> engine band + dashboard (retail 0x409180 / OnPaint). */
        update_game_state();
#endif
#if defined(FLYINGDODGEM_GDI_COMPAT)
        /* The streaming texture has to be locked again every frame:
         * PlatDisplay_Present unlocks it (and clears its cached pointer) at
         * the end of each frame, so locking once before the loop would leave
         * frames 2..N writing through a stale, already-unlocked pointer. */
        back = PlatDisplay_GetBackBuffer(display, &pitch);
        if (back == NULL)
        {
            SDL_Log("FlyingDodgem: failed to lock back buffer");
            break;
        }

        Game_RenderFrame(screen, dash, dashCount, selected);
        GdiCompat_Present(screen, back, pitch, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
#endif

        PlatDisplay_Present(display);
        SDL_Delay(16);
    }

#if defined(FLYINGDODGEM_GDI_COMPAT)
    DisplayDriver_Shutdown();
    WinmmStopEngine();
    if (g_menuMazOk)
        Maz_Free(&g_menuMaz);
    if (g_level1MazOk)
        Maz_Free(&g_level1Maz);
    if (g_menuTexOk)
        Tex_Free(&g_menuTex);
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