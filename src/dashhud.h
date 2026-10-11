#ifndef FLYINGDODGEM_DASHHUD_H
#define FLYINGDODGEM_DASHHUD_H

#include "flyingdodgem_types.h"

/* Repaint the dashboard's seven segmented gauges onto the screen DC.
 * The GDI objects (pen + lit/unlit brushes) are created on first use and kept
 * for the whole session. Safe to call with a NULL DC. */
void __cdecl DashHud_Paint(void *screen);

/* Set the lit-segment count for one gauge (0..6). Values are clamped to the
 * gauge's segment count. Index 0 is the tall vertical speed bar (retail
 * 0x2e0); indices 4..6 are the three 9-segment powerup bars under the left
 * panel (0x3a0 / 0x400 / 0x3d0). Retail feeds them from live game state, so
 * the seam only honours explicit sets and never invents steering input. */
void __cdecl DashHud_SetLevel(u32 index, s32 lit);

/* Set the two score-window values (retail dialog ids 30100/30101). Retail
 * formats them with "%4d" (HOVER.EXE 0x4183E0, format at 0x4c4d74) and draws
 * them centred into the nameplate windows through the text-slot cache
 * (0x408a70/0x454bc5) with an ExtTextOut ETO_CLIPPED repaint (0x45c425).
 * A window paints only after an explicit set: at boot the retail text cache
 * is empty and the windows stay black, so unset scores are never invented. */
void __cdecl DashHud_SetScores(s32 left, s32 right);

/* Set one flag-slot count (retail dialog ids 1000..1002, the three left-panel
 * boxes). Values are clamped to a single green digit cell (0..9), matching
 * the baked "0" placeholders in the art. Retail also checks exactly one
 * slot via CheckRadioButton (HOVER.EXE 0x40e4c4), but the checked visual is
 * unresolved, so no highlight is painted. */
void __cdecl DashHud_SetFlagCount(u32 slot, s32 count);

/* Paint the score windows and flag slots onto the screen DC, over the
 * dashboard art Game_RenderFrame already blitted. `dashSrc` is the loaded
 * DASHRES mip surface (the digit-cell source), `dashW`/`dashH` its size;
 * art coordinates below are measured on the 640x298 reference mip (id 0x85)
 * and scaled to the loaded surface for source reads. `dx`/`split`/`bodyDy`
 * are Game_RenderFrame's frame layout: the nameplate strip (art rows
 * 0..split) sits at screen (dx, 0) and everything below at (dx, bodyDy),
 * both blitted 1:1, so destinations are art coordinates plus the strip
 * offset. Safe to call with NULL surfaces (renders nothing). */
void __cdecl DashHud_PaintScores(void *screen, void *dashSrc, u32 dashW,
                                 u32 dashH, s32 dx, s32 split, s32 bodyDy);

#endif
