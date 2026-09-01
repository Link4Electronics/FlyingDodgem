#include "platform/display.h"

PlatDisplay * __cdecl PlatDisplay_Open(const PlatDisplayDesc *desc)
{
    (void)desc;
    return NULL;
}

void __cdecl PlatDisplay_Close(PlatDisplay *d)
{
    (void)d;
}

u8 * __cdecl PlatDisplay_GetBackBuffer(PlatDisplay *d, u32 *pitch)
{
    (void)d;
    if (pitch != NULL)
        *pitch = 0;
    return NULL;
}

void __cdecl PlatDisplay_Present(PlatDisplay *d)
{
    (void)d;
}

bool __cdecl PlatDisplay_PollEvents(PlatDisplay *d)
{
    (void)d;
    return true;
}
