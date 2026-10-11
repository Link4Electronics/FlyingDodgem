#ifndef FLYINGDODGEM_DISPLAY_NATIVE_H
#define FLYINGDODGEM_DISPLAY_NATIVE_H

#include "flyingdodgem_types.h"
#include "gdi32_compat.h"
#include "texmaze.h"

/* Native display driver (FLYINGDODGEM_GDI_COMPAT only).  Builds the retail
 * display object, starts DisplayEntry41ece0 on a worker thread with case 3
 * (DrawEntry402250), and points the span host at the shared INDEX8 screen
 * so F406740 rasterizes into the back buffer the SDL3 harness presents. */

#ifdef __cplusplus
extern "C" {
#endif

/* Bind the display object to `screen` and arm the span host.  Geometry is
 * drawn synchronously by DisplayDriver_Sync via M406090's wall pass. */
void __cdecl DisplayDriver_Init(HDC screen);

/* Present-side hook: re-sync pitch/framebuffer, run one DrawEntry pass. */
void __cdecl DisplayDriver_Sync(HDC screen);

/* Set the DisplayEntry case selector (disp+0xac).  Case 2 exits the loop. */
void __cdecl DisplayDriver_SetCase(s32 caseSel);

/* Hand the retained menu maze (+textures, may be NULL) to the wall pass.
 * Rebuilds the wall list from the MAZ statics; with maz == NULL the
 * placeholder grid stays.  Must be called after DisplayDriver_Init. */
void __cdecl DisplayDriver_SetMenuMaze(const MazDoc *maz, const TexArchive *tex);

/* Free the display object and host. */
void __cdecl DisplayDriver_Shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* FLYINGDODGEM_DISPLAY_NATIVE_H */
