#include "platform/display.h"

#include <SDL3/SDL.h>

struct PlatDisplay
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    u32 width;
    u32 height;
    u32 pitch;
    u8 *backbuffer;
};

PlatDisplay * __cdecl PlatDisplay_Open(const PlatDisplayDesc *desc)
{
    if (desc == NULL || desc->width == 0 || desc->height == 0)
        return NULL;

    const char *title = desc->title != NULL ? desc->title : "FlyingDodgem";
    u32 win_w = desc->window_width > 0 ? desc->window_width : desc->width;
    u32 win_h = desc->window_height > 0 ? desc->window_height : desc->height;

    SDL_Window *window = SDL_CreateWindow(title, (int)win_w, (int)win_h,
                                          SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
    if (window == NULL)
        return NULL;

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == NULL)
    {
        SDL_DestroyWindow(window);
        return NULL;
    }

    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             (int)desc->width, (int)desc->height);
    if (texture == NULL)
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        return NULL;
    }
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    PlatDisplay *d = (PlatDisplay *)SDL_malloc(sizeof(PlatDisplay));
    if (d == NULL)
    {
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        return NULL;
    }

    d->window = window;
    d->renderer = renderer;
    d->texture = texture;
    d->width = desc->width;
    d->height = desc->height;
    d->pitch = desc->width * 4;
    d->backbuffer = NULL;

    SDL_ShowWindow(window);
    return d;
}

void __cdecl PlatDisplay_Close(PlatDisplay *d)
{
    if (d == NULL)
        return;
    SDL_DestroyTexture(d->texture);
    SDL_DestroyRenderer(d->renderer);
    SDL_DestroyWindow(d->window);
    SDL_free(d);
}

u8 * __cdecl PlatDisplay_GetBackBuffer(PlatDisplay *d, u32 *pitch)
{
    if (d == NULL)
    {
        if (pitch != NULL)
            *pitch = 0;
        return NULL;
    }
    if (pitch != NULL)
        *pitch = d->pitch;

    /* Lock the streaming texture; this is our software back buffer back. */
    void *pixels = NULL;
    int p = 0;
    if (SDL_LockTexture(d->texture, NULL, &pixels, &p))
    {
        d->backbuffer = (u8 *)pixels;
        d->pitch = (u32)p;
    }
    return d->backbuffer;
}

void __cdecl PlatDisplay_Present(PlatDisplay *d)
{
    if (d == NULL)
        return;

    if (d->backbuffer != NULL)
    {
        SDL_UnlockTexture(d->texture);
        d->backbuffer = NULL;
    }

    int win_w = 0, win_h = 0;
    SDL_GetWindowSizeInPixels(d->window, &win_w, &win_h);

    /* Aspect-corrected letterboxed blit of the viewport into the window. */
    float scale = 1.0f;
    float sx = (float)win_w / (float)d->width;
    float sy = (float)win_h / (float)d->height;
    scale = sx < sy ? sx : sy;

    int dst_w = (int)((float)d->width * scale);
    int dst_h = (int)((float)d->height * scale);
    int dst_x = (win_w - dst_w) / 2;
    int dst_y = (win_h - dst_h) / 2;

    SDL_SetRenderDrawColor(d->renderer, 0, 0, 0, 255);
    SDL_RenderClear(d->renderer);

    SDL_FRect dst = { (float)dst_x, (float)dst_y, (float)dst_w, (float)dst_h };
    SDL_RenderTexture(d->renderer, d->texture, NULL, &dst);

    SDL_RenderPresent(d->renderer);
}

bool __cdecl PlatDisplay_PollEvents(PlatDisplay *d)
{
    SDL_Event event;

    (void)d;
    /* Peek, don't drain: every other event belongs to the USER32 message
     * pump (PeekMessageA), which consumes the SDL queue itself.  Only quit
     * is taken here so the main loop can exit. */
    SDL_PumpEvents();
    while (SDL_PeepEvents(&event, 1, SDL_GETEVENT,
                           SDL_EVENT_QUIT, SDL_EVENT_QUIT) == 1)
        return false;
    while (SDL_PeepEvents(&event, 1, SDL_GETEVENT,
                           SDL_EVENT_WINDOW_CLOSE_REQUESTED,
                           SDL_EVENT_WINDOW_CLOSE_REQUESTED) == 1)
        return false;
    return true;
}
