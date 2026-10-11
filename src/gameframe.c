#include "gameframe.h"
#include "dashhud.h"

#if defined(FLYINGDODGEM_GDI_COMPAT)
#include "display_native.h"
#endif

/* Frame composer for the native SDL3 driver. Everything here is compiled only
 * when FLYINGDODGEM_GDI_COMPAT is set; the reccmp matching build never links
 * this unit, so it cannot perturb assembly matching. */

/* The dashboard backdrop brush is created on first use and lives for the whole
 * session (the frame runs thousands of times). */
static HBRUSH g_black_brush = NULL;

static HBRUSH black_brush(void)
{
    if (g_black_brush == NULL)
        g_black_brush = CreateSolidBrush(DASH_BLACK_INDEX);
    return g_black_brush;
}

void __cdecl Game_RenderFrame(void *screen, const DashMip *mip, u32 count,
                              u32 selected)
{
    /* The DisplayEntry thread (case 3) has already rasterized the floor
     * through F406740 into the shared INDEX8 bits; re-sync the host in case
     * the surface moved, then composite the dashboard on top.  The dashboard
     * art's pure-blue mask leaves the windshield band open, so the 3D pass
     * stays visible without a full-screen clear wiping it. */
#if defined(FLYINGDODGEM_GDI_COMPAT)
    DisplayDriver_Sync(screen);
#endif

    if (mip != NULL && selected < count && mip[selected].surface != NULL)
    {
        /* The dashboard art is split into two pieces.  The top strip - the
         * HOVER 950 nameplate with its two score windows - is drawn at the
         * very top of the screen (y = 0).  Everything below it keeps the
         * retail placement dy = 226 (art row = screen row - 226), which is
         * what lines the art's painted gauge slots up with the retail widget
         * rects: the 8x75 striped bar of the 0x2e0 gauge sits at art
         * x429..436 (exactly the retail rect 429,384,437,459) and the two flat
         * grey bands of the horizontal gauges sit at art y186..199 / y215..228,
         * landing on 548,412,602,421 and 548,441,602,450 only with dy = 226.
         * The split row 105 is where the nameplate band ends: art row 105 is
         * the first row with no marble pixels anywhere across the width.  Rows
         * below the split therefore start at screen 105 + 226 = 331, and rows
         * 254..297 fall below the viewport and are clipped, as in retail;
         * nothing is scaled.  The art's transparent background is pure blue
         * (DASH_MASK_RGB) and is armed as a blit colour key first, so the
         * windshield band and the readout windows stay open for the 3D scene,
         * the mirror and the score instead of painting the mask. */
        s32 dx = ((s32)VIEWPORT_WIDTH - (s32)mip[selected].width) / 2;
        s32 dy = 226;
        s32 split = 105;
        GdiCompat_SetMaskColor(mip[selected].surface, DASH_MASK_RGB);
        BitBlt(screen, dx, 0, (s32)mip[selected].width, split,
               mip[selected].surface, 0, 0, GDI_SRCCOPY);
        BitBlt(screen, dx, dy + split, (s32)mip[selected].width,
               (s32)mip[selected].height - split, mip[selected].surface, 0,
               split, GDI_SRCCOPY);

        /* Score windows + flag slots: retail's 0x413c70 OnPaint tail updates
         * the 30100/30101 score controls and the 1000..1002 flag-slot
         * controls after the panel is drawn. The strip geometry is shared
         * so the paint lands on the same 1:1 placement as the art. */
        DashHud_PaintScores(screen, mip[selected].surface,
                            mip[selected].width, mip[selected].height, dx,
                            split, dy);
    }

    /* The gauges sit on top of the dashboard art, exactly like retail's
     * OnPaint running after the panel has been drawn. */
    DashHud_Paint(screen);
}
