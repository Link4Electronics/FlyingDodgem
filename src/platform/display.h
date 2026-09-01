#ifndef FLYINGDODGEM_PLATFORM_DISPLAY_H
#define FLYINGDODGEM_PLATFORM_DISPLAY_H

#include "flyingdodgem_types.h"

/* Framebuffer blit model mirroring the DIB/bitmap rendering the original
 * HOVER.EXE does with GDI (CreateDIBSection + BitBlt). The game renders into a
 * software back buffer, then presents it to the window. The SDL3 implementation
 * uploads the back buffer to a texture and draws it scaled to the window; a
 * null/stub implementation is used for the MSVC matching build. */

typedef struct PlatDisplay PlatDisplay;

typedef struct PlatDisplayDesc
{
    u32 width;           /* back-buffer / viewport width in pixels */
    u32 height;          /* back-buffer / viewport height in pixels */
    u32 window_width;    /* requested window size in pixels (scaled view) */
    u32 window_height;
    const char *title;
} PlatDisplayDesc;

/* Back buffer pixel format: 32-bit little-endian RGBA (byte order R,G,B,A). */

PlatDisplay * __cdecl PlatDisplay_Open(const PlatDisplayDesc *desc);
void __cdecl PlatDisplay_Close(PlatDisplay *d);

/* Returns a pointer to the software back buffer and stores its row pitch
 * (bytes per row) in *pitch. The pointer remains valid until Close. */
u8 * __cdecl PlatDisplay_GetBackBuffer(PlatDisplay *d, u32 *pitch);

/* Upload the back buffer to the window and flip. */
void __cdecl PlatDisplay_Present(PlatDisplay *d);

/* Polls window/input events. Returns false when the user requested quit. */
bool __cdecl PlatDisplay_PollEvents(PlatDisplay *d);

#endif
