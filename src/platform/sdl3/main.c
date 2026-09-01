#include "platform/audio.h"
#include "platform/display.h"

#include <SDL3/SDL.h>

/* The original HOVER.EXE renders into a 640x480 software back buffer and
 * presents it to a window (WinG/DIB surface, confirmed from the disassembly:
 * 0x41f916 stores 640 and 0x41f91c stores 480 as the render surface size). */

#define VIEWPORT_WIDTH  640
#define VIEWPORT_HEIGHT 480

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

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
    if (back != NULL)
    {
        u32 y, x;
        for (y = 0; y < VIEWPORT_HEIGHT; y++)
            for (x = 0; x < VIEWPORT_WIDTH; x++)
            {
                u8 *p = back + (u64)y * pitch + (u64)x * 4;
                p[0] = 0;
                p[1] = 0;
                p[2] = 0;
                p[3] = 255;
            }
    }

    bool running = true;
    while (running)
    {
        running = PlatDisplay_PollEvents(display);
        if (!running)
            break;
        PlatDisplay_Present(display);
        SDL_Delay(16);
    }

    PlatDisplay_Close(display);
    PlatAudio_Quit();
    SDL_Quit();
    return 0;
}
