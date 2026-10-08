#ifndef FLYINGDODGEM_GAMEFRAME_H
#define FLYINGDODGEM_GAMEFRAME_H

#include "flyingdodgem_types.h"
#include "gdi32_compat.h"

/* The shared 640x480 software viewport the GDI-compat pipeline renders into. */
#define VIEWPORT_WIDTH  640
#define VIEWPORT_HEIGHT 480

/* The DASHRES dashboard set: mip variants of the cockpit dashboard, loaded by
 * the retail DASH loader (HOVER.EXE 0x410518) via
 * FindResourceCompat(0x410534-ish)/FindResourceA. Sizes ascend 320x149 ..
 * 700x325; the 640-wide 0x85 mip fills the 640x480 viewport width exactly. */
#define DASHRES_FIRST_ID 0x81
#define DASHRES_COUNT    6

#define DASH_BLACK_INDEX 255u /* every DASH palette maps index 255 to pure black */

/* Sprite alpha mask: every DASHRES mip paints its transparent background with
 * pure blue (0,0,255), at a different palette index in each variant. */
#define DASH_MASK_RGB 0x0000ffu

/* One DASHRES.DLL mip (RT_BITMAP 8-bpp DIB) already turned into a surface by
 * the native loader in main.c. */
typedef struct DashMip
{
    HDC surface;
    const void *dib;
    u32 width;
    u32 height;
} DashMip;

/* Compose one 640x480 frame into the shared INDEX8 screen DC. This is the seam
 * between the platform driver (main.c) and everything the game draws: it owns
 * the clear, the dashboard backdrop and the HUD, so main.c only has to present
 * afterwards. `selected` is DASHRES_COUNT when no mip is available. */
void __cdecl Game_RenderFrame(void *screen, const DashMip *mip, u32 count,
                              u32 selected);

#endif
