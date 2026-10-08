#ifndef FLYINGDODGEM_GDI32_COMPAT_H
#define FLYINGDODGEM_GDI32_COMPAT_H

#include "flyingdodgem_types.h"

/* Minimal GDI32 re-implementation used by the reconstructed HOVER HUD/render
 * code so it can call the *same* GDI entry points the retail binary imports
 * (TextOutA, ExtTextOutA, StretchBlt, BitBlt, CreateDIBSection, SetDIBColorTable,
 * SelectObject, CreateSolidBrush, ...). The retail pipeline renders the 3D
 * viewport + cockpit HUD into an 8-bit indexed DIB surface and composites it
 * with the colour table, exactly as confirmed from the disassembly.
 *
 * This build unit is only compiled by the native SDL3 driver
 * (FLYINGDODGEM_GDI_COMPAT). The MSVC 5.0 reccmp matching build never compiles
 * these files, so they cannot perturb assembly matching.
 *
 * The wrapper keeps the same 8-bit-indexed surface model as GDI so the HUD code
 * is a faithful mirror of the original. The final GdiCompat_Present step maps
 * the indexed frame through its palette into the platform's RGBA32 back buffer.
 */

#define GDI_ERROR ((s32)-1)

typedef struct RGBQUAD
{
    u8 blue;
    u8 green;
    u8 red;
    u8 reserved;
} RGBQUAD;

typedef struct GDI_RECT
{
    s32 left;
    s32 top;
    s32 right;
    s32 bottom;
} GDI_RECT;

/* Minimal BITMAPINFOHEADER/BITMAPINFO compatible with the 8-bit DIB sections
 * the original game creates. */
typedef struct BITMAPINFOHEADER
{
    u32 biSize;
    s32 biWidth;
    s32 biHeight;
    u16 biPlanes;
    u16 biBitCount;
    u32 biCompression;
    u32 biSizeImage;
    s32 biXPelsPerMeter;
    s32 biYPelsPerMeter;
    u32 biClrUsed;
    u32 biClrImportant;
} BITMAPINFOHEADER;

typedef struct BITMAPINFO
{
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD bmiColors[256];
} BITMAPINFO;

/* Raster operation codes used by BitBlt/StretchBlt in the reconstructed code. */
#define GDI_SRCCOPY  0x00CC0020UL

/* Background modes and DrawText flags. */
#define GDI_OPAQUE   2
#define GDI_TRANSPARENT 1
#define GDI_DT_LEFT      0
#define GDI_DT_TOP       0
#define GDI_DT_SINGLELINE 0x20
#define GDI_DT_NOCLIP    0x100

/* Opaque GDI handles. */
typedef struct GdiSurface GdiSurface;
typedef struct GdiDC GdiDC;
typedef struct GdiBrush GdiBrush;
typedef struct GdiPen GdiPen;

typedef GdiSurface *HDC;
typedef GdiSurface *HBITMAP;
typedef GdiBrush *HBRUSH;
typedef void *HGDIOBJ;

/* Module / resource handles (resource-only DLL hosting, see below). */
typedef void *HINSTANCE;
typedef void *HMODULE;

/* --- Win32 resource API (resource-only DLL hosting) --- */

/* A loaded immutable resource blob + the raw image bytes it wraps. In the
 * retail binary the "module" is a DLL (e.g. DASHRES.DLL); here a HINSTANCE is
 * a GdiModule that owns a set of named DIB files loaded from disk. */
typedef struct GdiModule GdiModule;
typedef struct GdiResource GdiResource;

typedef GdiResource *HRSRC;
typedef GdiResource *HGLOBAL;

/* MAKEINTRESOURCE: resource ids 1..0xFFFF are integers, 0..0xFFFFF are strings.
 * The DASHRES bitmap ids (0x81..0x86) are integers. */
#define MAKEINTRESOURCEA(id) ((void *)((uintptr_t)(id)))
#define RT_BITMAP ((void *)(uintptr_t)2)

/* Load a resource-only DLL from a path on disk. Returns a module handle that
 * owns the DIBs it contains, or NULL on failure. */
HINSTANCE __cdecl LoadLibraryA(const char *path);

/* Release a module loaded with LoadLibraryA. */
void __cdecl FreeLibrary(HINSTANCE hmodule);

/* Look up a named resource within a module. lpName / lpType may be
 * MAKEINTRESOURCE integers (as for the DASHRES bitmap ids) or "Name"/"Type"
 * strings. Returns a resource handle, or NULL if not found. */
HRSRC __cdecl FindResourceA(HINSTANCE hmodule, const char *name, const char *type);

/* Fetch the byte size of a resource found by FindResourceA. */
u32 __cdecl SizeofResource(HINSTANCE hmodule, HRSRC hres);

/* Load the resource into memory (idempotent, returns the HGLOBAL on success). */
HGLOBAL __cdecl LoadResource(HINSTANCE hmodule, HRSRC hres);

/* Return a pointer to the loaded resource's raw bytes. */
const void * __cdecl LockResource(HGLOBAL hres);

/* Construct an 8-bpp indexed SDL surface/gdi surface directly from a raw DIB
 * (BITMAPINFOHEADER + palette + pixels), the format used by DASHRES.DLL. This
 * mirrors the original game reading a loaded resource into a DIB section and
 * selecting its colour table. Returns the new HDC (caller owns it). */
HDC __cdecl GdiCompat_CreateDibSurface(const void *dib);

/* Apply a raw DIB's 256-entry colour table to the shared palette store (the
 * analogue of selecting the resource's palette into the screen DC, as the
 * original does before drawing a loaded dashboard surface). The DASH mip
 * variants carry different, quantized palettes, so the displayed surface's own
 * palette must be applied before presenting it. */
u32 __cdecl GdiCompat_SetDibPalette(const void *dib);

/* Arm a sprite surface with a mask colour: the shared colour table is scanned
 * for `rgb` and that palette index becomes the blit colour key, so BitBlt
 * skips the transparent background instead of painting it. The DASHRES mip
 * variants each carry the pure-blue mask at a different palette index
 * (235/241/236/238/245/236), so the lookup must happen at draw time against
 * the palette that is currently selected. Returns the mask index, or
 * 0xffffffffu when no colour table entry matches. */
u32 __cdecl GdiCompat_SetMaskColor(HDC hdc, u32 rgb);

/* DIB creation flag. */
#define DIB_RGB_COLORS 0

/* --- Device context / surface management --- */

/* Create a memory DC. It starts out with no bitmap attached; a DIB created by
 * CreateDIBSection is attached with SelectObject, which is exactly the retail
 * backbuffer sequence (CreateCompatibleDC -> CreateDIBSection -> SelectObject
 * -> BitBlt/StretchBlt into it). */
HDC __cdecl CreateCompatibleDC(HDC hdc);

/* Create an 8-bit indexed DIB section. On success sets *ppBits to the pixel
 * array and returns an HDC for it. */
HBITMAP __cdecl CreateDIBSection(HDC hdc, const BITMAPINFO *pbmi,
                                 u32 usage, void **ppBits, void *section, u32 offset);

u8 __cdecl DeleteDC(HDC hdc);
u8 __cdecl DeleteObject(HGDIOBJ hgdiobj);

/* Select the palette for the DC's surface (8-bit indexed colour table). */
u32 __cdecl SetDIBColorTable(HDC hdc, u32 iStart, u32 cEntries, const RGBQUAD *pColors);
u32 __cdecl GetDIBColorTable(HDC hdc, u32 iStart, u32 cEntries, RGBQUAD *pColors);

/* Bit block transfers between indexed surfaces. */
u8 __cdecl BitBlt(HDC dst, s32 dx, s32 dy, s32 w, s32 h,
                  HDC src, s32 sx, s32 sy, u32 rop);
u8 __cdecl StretchBlt(HDC dst, s32 dx, s32 dy, s32 dw, s32 dh,
                      HDC src, s32 sx, s32 sy, s32 sw, s32 sh, u32 rop);

/* --- Object selection / device caps (retail present path) --- */

/* Select a GDI object into a DC. Bitmaps attach to a memory DC (the retail
 * backbuffer setup does CreateCompatibleDC + SelectObject(memDC, dibDib));
 * brushes, pens and palettes are stashed in the DC state. Returns the
 * previously selected object of the same kind, or NULL. */
HGDIOBJ __cdecl SelectObject(HDC hdc, HGDIOBJ obj);

/* Stock objects (GDI_STOCK_* indices). Brushes/pens resolve their RGB colour
 * to the nearest palette index at fill time; DEFAULT_PALETTE is the shared
 * default palette handle. Fonts are not provided. */
#define GDI_WHITE_BRUSH    0
#define GDI_LTGRAY_BRUSH   1
#define GDI_GRAY_BRUSH     2
#define GDI_DKGRAY_BRUSH   3
#define GDI_BLACK_BRUSH    4
#define GDI_NULL_BRUSH     5
#define GDI_WHITE_PEN      6
#define GDI_BLACK_PEN      7
#define GDI_NULL_PEN       8
#define GDI_DEFAULT_PALETTE 15
HGDIOBJ __cdecl GetStockObject(s32 i);

/* Save/restore the selected objects (plus text state) of a DC. The retail
 * dashboard DC brackets every gauge repaint with SaveDC (0x45091b) and
 * RestoreDC(..., -1) (0x45095e) so the HUD's pen/brush selections do not leak
 * into the rest of the frame. `saved` is the id returned by SaveDC, or -1 for
 * "undo the most recent SaveDC". Returns non-zero on success. */
#define GDI_MAX_SAVEDC 8
s32 __cdecl SaveDC(HDC hdc);
s32 __cdecl RestoreDC(HDC hdc, s32 saved);

/* PatBlt raster ops the present path uses: retail clears regions with
 * BLACKNESS (0x42) and fills with PATCOPY of the selected brush. */
#define GDI_PATCOPY    0x00F00021UL
#define GDI_BLACKNESS  0x00000042UL
#define GDI_WHITENESS  0x00FF0062UL
u8 __cdecl PatBlt(HDC hdc, s32 x, s32 y, s32 w, s32 h, u32 rop);

/* Filled/outlined rectangle using the selected brush + pen. */
u8 __cdecl Rectangle(HDC hdc, s32 left, s32 top, s32 right, s32 bottom);

/* GetDeviceCaps indices the retail setup queries (screen size / surface
 * format); everything else reports 0. */
#define GDI_DRIVERVERSION 0
#define GDI_HORZRES       8
#define GDI_VERTRES       10
#define GDI_PLANES        12
#define GDI_BITSPIXEL     14
s32 __cdecl GetDeviceCaps(HDC hdc, s32 index);

/* GDI batch/flush controls are no-ops in this layer (they only existed to
 * batch Win95 GDI calls); they must still link because the retail display
 * init calls them. */
u8 __cdecl GdiFlush(void);
u32 __cdecl GdiSetBatchLimit(u32 limit);

/* Text extent for the layer's fixed 8x8 font. */
typedef struct GDI_SIZE
{
    s32 cx;
    s32 cy;
} GDI_SIZE;
u8 __cdecl GetTextExtentPointA(HDC hdc, const char *str, s32 count, GDI_SIZE *size);

/* Single-pixel access, returned as a COLORREF (0x00bbggrr). */
u32 __cdecl GetPixel(HDC hdc, s32 x, s32 y);

/* --- Palettes (retail builds a LOGPALETTE from each maze's TEX palette) --- */

typedef u32 COLORREF;
#define RGB(r, g, b) ((COLORREF)(((u8)(r)) | ((u32)((u8)(g))) << 8 | \
                                 ((u32)((u8)(b))) << 16))

typedef struct PALETTEENTRY
{
    u8 peRed;
    u8 peGreen;
    u8 peBlue;
    u8 peFlags;
} PALETTEENTRY;

typedef struct LOGPALETTE
{
    u16 palVersion;
    u16 palNumEntries;
    PALETTEENTRY palPalEntry[256];
} LOGPALETTE;

typedef struct GdiPalette GdiPalette;
typedef GdiPalette *HPALETTE;

HPALETTE __cdecl CreatePalette(const LOGPALETTE *lplpal);
HPALETTE __cdecl SelectPalette(HDC hdc, HPALETTE pal, u8 forceBackground);
u32 __cdecl RealizePalette(HDC hdc);
u32 __cdecl SetPaletteEntries(HPALETTE pal, u32 start, u32 count,
                              const PALETTEENTRY *entries);
u32 __cdecl GetPaletteEntries(HPALETTE pal, u32 start, u32 count,
                              PALETTEENTRY *entries);
u32 __cdecl GetNearestPaletteIndex(HPALETTE pal, COLORREF color);
u32 __cdecl GetSystemPaletteEntries(HDC hdc, u32 start, u32 count,
                                    PALETTEENTRY *entries);

/* --- Drawing primitives (indexed surface) --- */

/* Text: draw 8x8 bitmap-font glyphs into the DC using the current text colour
 * (a palette index). */
u8 __cdecl TextOutA(HDC hdc, s32 x, s32 y, const char *str, s32 count);
u8 __cdecl ExtTextOutA(HDC hdc, s32 x, s32 y, u32 options, const GDI_RECT *clip,
                       const char *str, u32 count, const s32 *dx);
s32 __cdecl DrawTextA(HDC hdc, const char *str, s32 count, GDI_RECT *rect, u32 format);

/* Solid-fill rectangle using the current brush (a palette index). */
u8 __cdecl FillRect(HDC hdc, const GDI_RECT *rect, HBRUSH brush);

/* Text colour is a palette index; brush colour likewise. */
u32 __cdecl SetTextColor(HDC hdc, u32 color);
u32 __cdecl SetBkColor(HDC hdc, u32 color);
s32 __cdecl SetBkMode(HDC hdc, s32 mode);

/* A solid brush carries a single palette index. */
HBRUSH __cdecl CreateSolidBrush(u32 color);

/* Fetch the fixed "screen" DC used as the compositing target / presentation
 * surface. Its pixel array is 8-bit indexed, width x height. */
HDC __cdecl GdiCompat_GetScreenDC(u32 width, u32 height,
                                  const RGBQUAD *palette, u32 paletteSize);

/* Present the screen DC: map the indexed frame through its palette into an
 * RGBA32 back buffer (byte order R,G,B,A). */
void __cdecl GdiCompat_Present(HDC screen, u8 *dst, u32 dstPitch,
                               u32 dstWidth, u32 dstHeight);

#endif
