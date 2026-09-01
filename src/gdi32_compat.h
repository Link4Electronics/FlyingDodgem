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

/* DIB creation flag. */
#define DIB_RGB_COLORS 0

/* --- Device context / surface management --- */

/* Create a memory DC. In this model the DC *is* the 8-bit indexed surface. */
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
