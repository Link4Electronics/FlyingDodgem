#include "dashhud.h"
#include "gdi32_compat.h"

#include <stdio.h>

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

/* Lit levels per gauge. Retail reads these from live game state each OnPaint
 * (thrust/energy/shields/speed/force). The native seam owns the values; the
 * frame update writes them via DashHud_SetLevel before DashHud_Paint. */
static s32 kHi[7] = { 0, 0, 0, 0, 0, 0, 0 };

static HGDIOBJ g_pen = NULL;
static HBRUSH g_dark = NULL;
static HGDIOBJ g_lit[7] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };
static int g_ready = 0;

void __cdecl DashHud_SetLevel(u32 index, s32 lit)
{
    if (index >= 7)
        return;
    if (lit < 0)
        lit = 0;
    if (lit > kGauges[index].count)
        lit = kGauges[index].count;
    kHi[index] = lit;
}

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

/* --------------------------------------------------------------------------
 * Score windows + flag slots.
 *
 * Retail layout (measured on the 640x298 DASHRES mip, id 0x85; every mip
 * scales uniformly, so coordinates below scale with the loaded surface):
 *
 *   - score windows (dialog ids 30100/30101): black art rects x52..171 and
 *     x452..611 at art y8..46, flanking the HOVER 950 plate. Retail formats
 *     the values with "%4d" (0x4183E0) and repaints them centred with
 *     ExtTextOut ETO_CLIPPED (0x45c425) through the 0x408a70/0x454bc5 text
 *     cache (InvalidateRect per slot).
 *   - flag slots (dialog ids 1000..1002): the three left-panel count boxes.
 *     Each box is ~50px wide with a black digit inset on its right holding
 *     one green 7-segment digit (baked "0" placeholders). The art's bottom
 *     strip carries the matching digit-cell sheet: 10 small cells, 12px
 *     pitch from x3, art y260..277 (rows 261..277 hold the green rows).
 *     Retail checks exactly one slot via CheckRadioButton(hwnd, 1000, 1002,
 *     base+1000) (0x40e4c4); the checked visual is not recoverable from the
 *     binary, so the seam tracks no highlight until it is reversed.
 *
 * Colours: the windows are pure black in every mip palette, so the score
 * text uses the nearest white index with a transparent background; the
 * count digits come straight from the art cells (SRCCOPY over the already
 * black insets).
 * ------------------------------------------------------------------------- */

#define DASH_REF_W 640u
#define DASH_REF_H 298u

/* Score window art rects (right/bottom exclusive). */
static const DashRect kScoreWin[2] = {
    { 52, 8, 172, 47 },
    { 452, 8, 612, 46 },
};

/* Small digit-cell sheet geometry (bottom strip of the art). */
#define DASH_DIGIT_X0 3
#define DASH_DIGIT_Y0 260
#define DASH_DIGIT_W  12
#define DASH_DIGIT_H  18
#define DASH_DIGIT_PITCH 12

/* Count-box digit insets: blit top-left per slot (12x18 cell). The three
 * boxes sit on a 58px pitch starting at x77, art y219. */
#define DASH_COUNT_X0 77
#define DASH_COUNT_Y0 219
#define DASH_COUNT_PITCH 58

static s32 kScore[2] = { 0, 0 };
static u32 kScoreValid[2] = { 0, 0 };
static s32 kFlagCount[3] = { 0, 0, 0 };

void __cdecl DashHud_SetScores(s32 left, s32 right)
{
    kScore[0] = left;
    kScore[1] = right;
    /* Retail only paints a score window after a real score update reaches
     * the text cache (0x408a70/0x454bc5 + InvalidateRect); at boot the
     * cache slots are empty and the windows stay black. Never invent
     * points: a window paints only after an explicit set. */
    kScoreValid[0] = 1;
    kScoreValid[1] = 1;
}

void __cdecl DashHud_SetFlagCount(u32 slot, s32 count)
{
    if (slot >= 3)
        return;
    if (count < 0)
        count = 0;
    if (count > 9)
        count = 9;
    kFlagCount[slot] = count;
}

static s32 dash_scale_x(s32 v, u32 dashW)
{
    return (s32)(((u32)v * dashW) / DASH_REF_W);
}

static s32 dash_scale_y(s32 v, u32 dashH)
{
    return (s32)(((u32)v * dashH) / DASH_REF_H);
}

static void paint_score(HDC dc, u32 index, u32 textIdx, s32 dx)
{
    /* Retail "%4d" (0x4c4d74), centred like the 0x45c425 paint: measure
     * with GetTextExtentPointA, centre in the window, ExtTextOut with
     * ETO_CLIPPED (4) and the window as the clip rect. Both windows live
     * in the nameplate strip, which the frame draws at (dx, 0) 1:1. */
    char buf[16];
    GDI_SIZE ext;
    GDI_RECT clip;
    s32 left;
    s32 top;
    s32 w;
    s32 h;
    s32 x;
    s32 y;
    u32 n;

    /* No invented points: without a real update the window stays black,
     * exactly like the retail boot state (empty text cache). */
    if (!kScoreValid[index])
        return;
    (void)snprintf(buf, sizeof(buf), "%4d", kScore[index]);
    buf[sizeof(buf) - 1] = '\0';
    for (n = 0; buf[n] != '\0'; n++)
    {
    }
    if (n == 0)
        return;
    GetTextExtentPointA(dc, buf, (s32)n, &ext);
    left = kScoreWin[index].left + dx;
    top = kScoreWin[index].top;
    w = kScoreWin[index].right - kScoreWin[index].left;
    h = kScoreWin[index].bottom - kScoreWin[index].top;
    x = left + ((w - ext.cx) / 2);
    y = top + ((h - ext.cy) / 2);
    clip.left = left;
    clip.top = top;
    clip.right = left + w;
    clip.bottom = top + h;
    SetTextColor(dc, textIdx);
    SetBkMode(dc, GDI_TRANSPARENT);
    ExtTextOutA(dc, x, y, 4u, &clip, buf, n, NULL);
}

static void paint_count(HDC dc, HDC dashSrc, u32 slot, u32 dashW, u32 dashH,
                        s32 dx, s32 bodyDy)
{
    /* One green digit cell blitted from the art's own sheet onto the box
     * inset (retail BitBlt path, GDI_SRCCOPY). The cells carry their black
     * background, which lands on the already-black inset. Source reads
     * scale with the loaded mip; the destination is the frame's 1:1 body
     * placement (art + (dx, bodyDy)). */
    s32 sx = dash_scale_x(DASH_DIGIT_X0 + (s32)kFlagCount[slot] * DASH_DIGIT_PITCH, dashW);
    s32 sy = dash_scale_y(DASH_DIGIT_Y0, dashH);
    s32 sw = dash_scale_x(DASH_DIGIT_W, dashW);
    s32 sh = dash_scale_y(DASH_DIGIT_H, dashH);
    s32 ddx = DASH_COUNT_X0 + (s32)slot * DASH_COUNT_PITCH + dx;
    s32 ddy = DASH_COUNT_Y0 + bodyDy;

    BitBlt(dc, ddx, ddy, sw, sh, dashSrc, sx, sy, GDI_SRCCOPY);
}

void __cdecl DashHud_PaintScores(void *screen, void *dashSrc, u32 dashW, u32 dashH, s32 dx, s32 split, s32 bodyDy)
{
    HDC dc = (HDC)screen;
    HDC src = (HDC)dashSrc;
    HPALETTE pal;
    u32 textIdx;
    u32 i;

    if (dc == NULL || src == NULL || dashW == 0 || dashH == 0)
        return;
    /* The score windows must be inside the nameplate strip the frame draws
     * at the top; anything else means the layout changed underneath us. */
    if (kScoreWin[0].bottom > split || kScoreWin[1].bottom > split)
        return;
    if (DASH_COUNT_Y0 < split)
        return;

    SaveDC(dc);

    pal = (HPALETTE)GetStockObject(GDI_DEFAULT_PALETTE);
    textIdx = GetNearestPaletteIndex(pal, 0x00ffffffUL);
    for (i = 0; i < 2; i++)
        paint_score(dc, i, textIdx, dx);
    for (i = 0; i < 3; i++)
        paint_count(dc, src, i, dashW, dashH, dx, bodyDy);

    RestoreDC(dc, -1);
}
