#include "platform/audio.h"

#include <SDL3/SDL.h>

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
        return 1;

    PlatAudio_Init();

    SDL_Delay(100);

    PlatAudio_Quit();
    SDL_Quit();
    return 0;
}
