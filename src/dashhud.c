#include "dashhud.h"
#include "gdi32_compat.h"

/* --------------------------------------------------------------------------
 * Dashboard gauge HUD.
 *
 * Retail repaint path: OnPaint (HOVER.EXE 0x413c70) walks seven widget
 * records inside the app object and, for each one, calls the matched gauge
 * drawer (0x401ad0 / F401ad0 in render.c) with `all = 1`:
 *
 *     SaveDC();
 *     SelectObject(pen); SelectObject(lit);
 *     for (i = 0; i < count; i++) {
 *         if (horizontal) r = l + extent; else t = b - extent;
 *         SelectObject(hi > i ? lit : dark);
 *         Rectangle(l, t, r, b);
 *         if (horizontal) l = r; else b = t;
 *     }
 *     RestoreDC(-1);
 *
 * SaveDC/RestoreDC are the DashDc vtable slots 9/10 (0x45091b / 0x45095e ->
 * GDI32!SaveDC and GDI32!RestoreDC(hdc, -1)); OnPaint itself makes no GDI
 * calls beyond those seven gauges, so the bracket lives inside the drawer.
 *
 * Everything below is lifted from the 640x480 configuration record (retail
 * 0x461680):
 *   - widget rects            0x41fa26..0x41fac1  (SetRect = 0x4caaa8)
 *   - segment counts          0x41fb02..0x41fb3b
 *   - count/brush wiring      0x41060d..0x410704  (SetFour11d60 = 0x411d60)
 *   - paint order             0x413ca1..0x413cfe
 *   - GDI colours created by  0x41f1c0 (CreatePen black/white,
 *     CreateSolidBrush yellow 0x0000ffff, black 0, green 0x0072ff00, red
 *     0x000000ff)
 *
 * F401ad0 itself is *not* driven from here: it packs brushOn/brushOff/pen into
 * three consecutive 4-byte fields (+0x10/+0x14/+0x18) that render.c reads as
 * `void *`. On a 64-bit host those 8-byte reads overlap, and no three distinct
 * pointers can satisfy the overlap (the middle pointer must be both the high
 * half of the first and the low half of the third), so retail code cannot
 * execute correctly here. The native seam therefore reproduces the same
 * sequence instead.
 * ------------------------------------------------------------------------- */

typedef struct DashRect
{
    s32 left;
    s32 top;
    s32 right;
    s32 bottom;
} DashRect;

typedef struct DashGauge
{
    DashRect rect;
    s32 count;
    u32 litRgb;   /* COLORREF passed to CreateSolidBrush at 0x41f1c0 */
} DashGauge;

/* Retail paint order (0x413ca1..0x413cfe): 0x2e0, 0x370, 0x340, 0x310,
 * 0x3a0, 0x3d0, 0x400. 0x370 (green) and 0x340 (red) share a rect, so the red
 * gauge painted second always covers the green one - retail does the same. */
static const DashGauge kGauges[7] = {
    { { 429, 384, 437, 459 }, 25, 0x0000ffffUL }, /* 0x2e0 yellow, vertical   */
    { { 548, 441, 602, 450 }, 18, 0x0072ff00UL }, /* 0x370 green, horizontal  */
    { { 548, 441, 602, 450 }, 18, 0x000000ffUL }, /* 0x340 red,   horizontal  */
    { { 548, 412, 602, 421 }, 18, 0x0000ffffUL }, /* 0x310 yellow, horizontal */
    { {  84, 401,  90, 438 },  9, 0x0000ffffUL }, /* 0x3a0 yellow, vertical   */
    { { 201, 401, 207, 438 },  9, 0x0000ffffUL }, /* 0x3d0 yellow, vertical   */
    { { 142, 401, 148, 438 },  9, 0x0000ffffUL }, /* 0x400 yellow, vertical   */
};

/* Segment counts are `count`; lit levels are `hi`. Retail reads `hi` from live
 * game state (thrust/energy/shields) which the frame seam does not have yet,
 * so these are stand-ins sized to look like a dashboard at rest. */
static const s32 kHi[7] = { 18, 18, 12, 14, 7, 5, 9 };

static HGDIOBJ g_pen = NULL;
static HBRUSH g_dark = NULL;
static HGDIOBJ g_lit[7] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };
static int g_ready = 0;

static void dash_hud_init(void)
{
    HPALETTE pal = (HPALETTE)GetStockObject(GDI_DEFAULT_PALETTE);
    u32 i;

    g_pen = GetStockObject(GDI_BLACK_PEN);          /* CreatePen(0, 1, 0) */
    g_dark = CreateSolidBrush(GetNearestPaletteIndex(pal, 0x00000000UL));
    for (i = 0; i < 7; i++)
        g_lit[i] = CreateSolidBrush(
            GetNearestPaletteIndex(pal, kGauges[i].litRgb));
    g_ready = 1;
}

static void paint_gauge(HDC dc, u32 index)
{
    const DashGauge *g = &kGauges[index];
    HGDIOBJ lit = g_lit[index];
    s32 l = g->rect.left;
    s32 t = g->rect.top;
    s32 r = g->rect.right;
    s32 b = g->rect.bottom;
    s32 horiz = ((b - t) <= (r - l));
    s32 extent = horiz ? (r - l) / g->count : (b - t) / g->count;
    s32 i;

    SaveDC(dc);
    SelectObject(dc, g_pen);
    SelectObject(dc, lit);
    for (i = 0; i < g->count; i++)
    {
        if (horiz)
            r = l + extent;
        else
            t = b - extent;
        SelectObject(dc, (kHi[index] > i) ? lit : g_dark);
        Rectangle(dc, l, t, r, b);
        if (horiz)
            l = r;
        else
            b = t;
    }
    RestoreDC(dc, -1);
}

void __cdecl DashHud_Paint(void *screen)
{
    HDC dc = (HDC)screen;
    u32 i;

    if (dc == NULL)
        return;
    if (!g_ready)
        dash_hud_init();

    for (i = 0; i < 7; i++)
        paint_gauge(dc, i);
}
