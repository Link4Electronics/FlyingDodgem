#include "gdi32_compat.h"

#include <SDL3/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/* ---------------------------------------------------------------------------
 * GDI32 -> SDL3 translation layer.
 *
 * The retail HOVER.EXE composites the 3D viewport + cockpit HUD into an 8-bit
 * indexed DIB surface (CreateDIBSection + SetDIBColorTable) and blits text and
 * sprites with GDI (BitBlt/StretchBlt/TextOutA/...). This wrapper translates
 * those same GDI entry points onto SDL3 primitives:
 *
 *   CreateDIBSection  -> SDL_CreateSurface(w,h,SDL_PIXELFORMAT_INDEX8) + palette
 *   SetDIBColorTable   -> SDL_SetPaletteColors
 *   BitBlt             -> SDL_BlitSurface
 *   StretchBlt         -> SDL_BlitSurfaceScaled (nearest)
 *   FillRect           -> SDL_FillSurfaceRect
 *   TextOutA/...       -> glyph blit into the INDEX8 pixel array
 *   GdiCompat_Present  -> map INDEX8 frame through palette into RGBA32 back buffer
 *
 * The HUD code is therefore written against the *same* GDI entry points the
 * retail binary imports, and the rendering happens on real SDL3 surfaces.
 * This unit is only compiled by the SDL3 native driver (FLYINGDODGEM_GDI_COMPAT);
 * the MSVC 5.0 reccmp build never compiles it.
 * ------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------
 * 8x8 bitmap font for TextOutA/ExtTextOutA/DrawTextA.
 * Each glyph is 8 rows of 8 pixels (1 bit per pixel, msb = leftmost column).
 * Covers printable ASCII 0x20..0x7E. The original HUD text was drawn with a
 * small system font; this embedded font reproduces the same indexed blitting
 * model (colour table + bitmap copy), which is what the reconstruction cares
 * about matching at the pipeline level.
 * ------------------------------------------------------------------------- */

typedef struct
{
    u8 rows[8];
} Glyph8;

#define G_FIRST 0x20
#define G_LAST  0x7E
#define G_COUNT (G_LAST - G_FIRST + 1)

static const Glyph8 g_font[G_COUNT] = {
    /* 0x20 space */ {{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
    /* 0x21 ! */     {{0x18,0x18,0x18,0x18,0x18,0x00,0x18,0x00}},
    /* 0x22 " */     {{0x6c,0x6c,0x6c,0x00,0x00,0x00,0x00,0x00}},
    /* 0x23 # */     {{0x6c,0x6c,0xfe,0x6c,0xfe,0x6c,0x6c,0x00}},
    /* 0x24 $ */     {{0x18,0x3c,0x58,0x3c,0x1a,0x3c,0x18,0x00}},
    /* 0x25 % */     {{0x46,0xa6,0x4c,0x18,0x32,0x65,0x62,0x00}},
    /* 0x26 & */     {{0x38,0x6c,0x38,0x78,0x4c,0x6c,0x3a,0x00}},
    /* 0x27 ' */     {{0x18,0x18,0x30,0x00,0x00,0x00,0x00,0x00}},
    /* 0x28 ( */     {{0x0c,0x18,0x30,0x30,0x30,0x18,0x0c,0x00}},
    /* 0x29 ) */     {{0x30,0x18,0x0c,0x0c,0x0c,0x18,0x30,0x00}},
    /* 0x2a * */     {{0x00,0x6c,0x38,0xfe,0x38,0x6c,0x00,0x00}},
    /* 0x2b + */     {{0x00,0x18,0x18,0x7e,0x18,0x18,0x00,0x00}},
    /* 0x2c , */     {{0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x30}},
    /* 0x2d - */     {{0x00,0x00,0x00,0x7e,0x00,0x00,0x00,0x00}},
    /* 0x2e . */     {{0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00}},
    /* 0x2f / */     {{0x06,0x0c,0x18,0x30,0x60,0xc0,0x80,0x00}},
    /* 0x30 0 */     {{0x3c,0x66,0x6e,0x7a,0x72,0x66,0x3c,0x00}},
    /* 0x31 1 */     {{0x18,0x38,0x18,0x18,0x18,0x18,0x7e,0x00}},
    /* 0x32 2 */     {{0x3c,0x66,0x06,0x0c,0x18,0x30,0x7e,0x00}},
    /* 0x33 3 */     {{0x3c,0x66,0x06,0x1c,0x06,0x66,0x3c,0x00}},
    /* 0x34 4 */     {{0x0c,0x1c,0x3c,0x6c,0x7e,0x0c,0x0c,0x00}},
    /* 0x35 5 */     {{0x7e,0x60,0x7c,0x06,0x06,0x66,0x3c,0x00}},
    /* 0x36 6 */     {{0x1c,0x30,0x60,0x7c,0x66,0x66,0x3c,0x00}},
    /* 0x37 7 */     {{0x7e,0x06,0x0c,0x18,0x30,0x30,0x30,0x00}},
    /* 0x38 8 */     {{0x3c,0x66,0x66,0x3c,0x66,0x66,0x3c,0x00}},
    /* 0x39 9 */     {{0x3c,0x66,0x66,0x3e,0x06,0x0c,0x38,0x00}},
    /* 0x3a : */     {{0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x00}},
    /* 0x3b ; */     {{0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x30}},
    /* 0x3c < */     {{0x0c,0x18,0x30,0x60,0x30,0x18,0x0c,0x00}},
    /* 0x3d = */     {{0x00,0x00,0x7e,0x00,0x7e,0x00,0x00,0x00}},
    /* 0x3e > */     {{0x30,0x18,0x0c,0x06,0x0c,0x18,0x30,0x00}},
    /* 0x3f ? */     {{0x3c,0x66,0x06,0x0c,0x18,0x00,0x18,0x00}},
    /* 0x40 @ */     {{0x3c,0x66,0x6e,0x7e,0x6c,0x60,0x3c,0x00}},
    /* 0x41 A */     {{0x3c,0x66,0x66,0x7e,0x66,0x66,0x66,0x00}},
    /* 0x42 B */     {{0x7c,0x66,0x66,0x7c,0x66,0x66,0x7c,0x00}},
    /* 0x43 C */     {{0x3c,0x66,0x60,0x60,0x60,0x66,0x3c,0x00}},
    /* 0x44 D */     {{0x78,0x6c,0x66,0x66,0x66,0x6c,0x78,0x00}},
    /* 0x45 E */     {{0x7e,0x60,0x60,0x78,0x60,0x60,0x7e,0x00}},
    /* 0x46 F */     {{0x7e,0x60,0x60,0x78,0x60,0x60,0x60,0x00}},
    /* 0x47 G */     {{0x3c,0x66,0x60,0x6e,0x66,0x66,0x3c,0x00}},
    /* 0x48 H */     {{0x66,0x66,0x66,0x7e,0x66,0x66,0x66,0x00}},
    /* 0x49 I */     {{0x7e,0x18,0x18,0x18,0x18,0x18,0x7e,0x00}},
    /* 0x4a J */     {{0x1e,0x0c,0x0c,0x0c,0x0c,0x6c,0x38,0x00}},
    /* 0x4b K */     {{0x66,0x6c,0x78,0x70,0x78,0x6c,0x66,0x00}},
    /* 0x4c L */     {{0x60,0x60,0x60,0x60,0x60,0x60,0x7e,0x00}},
    /* 0x4d M */     {{0x66,0x66,0x7e,0x7e,0x66,0x66,0x66,0x00}},
    /* 0x4e N */     {{0x66,0x76,0x7e,0x6e,0x66,0x66,0x66,0x00}},
    /* 0x4f O */     {{0x3c,0x66,0x66,0x66,0x66,0x66,0x3c,0x00}},
    /* 0x50 P */     {{0x7c,0x66,0x66,0x7c,0x60,0x60,0x60,0x00}},
    /* 0x51 Q */     {{0x3c,0x66,0x66,0x66,0x6a,0x6c,0x36,0x00}},
    /* 0x52 R */     {{0x7c,0x66,0x66,0x7c,0x6c,0x66,0x66,0x00}},
    /* 0x53 S */     {{0x3c,0x66,0x60,0x3c,0x06,0x66,0x3c,0x00}},
    /* 0x54 T */     {{0x7e,0x18,0x18,0x18,0x18,0x18,0x18,0x00}},
    /* 0x55 U */     {{0x66,0x66,0x66,0x66,0x66,0x66,0x3c,0x00}},
    /* 0x56 V */     {{0x66,0x66,0x66,0x66,0x66,0x3c,0x18,0x00}},
    /* 0x57 W */     {{0x66,0x66,0x66,0x7e,0x7e,0x66,0x66,0x00}},
    /* 0x58 X */     {{0x66,0x66,0x3c,0x18,0x3c,0x66,0x66,0x00}},
    /* 0x59 Y */     {{0x66,0x66,0x66,0x3c,0x18,0x18,0x18,0x00}},
    /* 0x5a Z */     {{0x7e,0x06,0x0c,0x18,0x30,0x60,0x7e,0x00}},
    /* 0x5b [ */     {{0x3c,0x30,0x30,0x30,0x30,0x30,0x3c,0x00}},
    /* 0x5c \ */     {{0x80,0xc0,0x60,0x30,0x18,0x0c,0x06,0x00}},
    /* 0x5d ] */     {{0x3c,0x0c,0x0c,0x0c,0x0c,0x0c,0x3c,0x00}},
    /* 0x5e ^ */     {{0x18,0x3c,0x66,0x00,0x00,0x00,0x00,0x00}},
    /* 0x5f _ */     {{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7e}},
    /* 0x60 ` */     {{0x30,0x18,0x0c,0x00,0x00,0x00,0x00,0x00}},
    /* 0x61 a */     {{0x00,0x00,0x3c,0x06,0x3e,0x66,0x3e,0x00}},
    /* 0x62 b */     {{0x60,0x60,0x7c,0x66,0x66,0x66,0x7c,0x00}},
    /* 0x63 c */     {{0x00,0x00,0x3c,0x60,0x60,0x60,0x3c,0x00}},
    /* 0x64 d */     {{0x06,0x06,0x3e,0x66,0x66,0x66,0x3e,0x00}},
    /* 0x65 e */     {{0x00,0x00,0x3c,0x66,0x7e,0x60,0x3c,0x00}},
    /* 0x66 f */     {{0x1c,0x36,0x30,0x78,0x30,0x30,0x30,0x00}},
    /* 0x67 g */     {{0x00,0x00,0x3e,0x66,0x66,0x3e,0x06,0x3c}},
    /* 0x68 h */     {{0x60,0x60,0x7c,0x66,0x66,0x66,0x66,0x00}},
    /* 0x69 i */     {{0x18,0x00,0x38,0x18,0x18,0x18,0x7e,0x00}},
    /* 0x6a j */     {{0x0c,0x00,0x1c,0x0c,0x0c,0x0c,0x6c,0x38}},
    /* 0x6b k */     {{0x60,0x60,0x6c,0x78,0x78,0x6c,0x66,0x00}},
    /* 0x6c l */     {{0x38,0x18,0x18,0x18,0x18,0x18,0x7e,0x00}},
    /* 0x6d m */     {{0x00,0x00,0x6c,0x7e,0x7e,0x66,0x66,0x00}},
    /* 0x6e n */     {{0x00,0x00,0x7c,0x66,0x66,0x66,0x66,0x00}},
    /* 0x6f o */     {{0x00,0x00,0x3c,0x66,0x66,0x66,0x3c,0x00}},
    /* 0x70 p */     {{0x00,0x00,0x7c,0x66,0x66,0x7c,0x60,0x60}},
    /* 0x71 q */     {{0x00,0x00,0x3e,0x66,0x66,0x3e,0x06,0x06}},
    /* 0x72 r */     {{0x00,0x00,0x6c,0x76,0x60,0x60,0x60,0x00}},
    /* 0x73 s */     {{0x00,0x00,0x3e,0x60,0x3c,0x06,0x7c,0x00}},
    /* 0x74 t */     {{0x30,0x30,0x7c,0x30,0x30,0x36,0x1c,0x00}},
    /* 0x75 u */     {{0x00,0x00,0x66,0x66,0x66,0x66,0x3e,0x00}},
    /* 0x76 v */     {{0x00,0x00,0x66,0x66,0x66,0x3c,0x18,0x00}},
    /* 0x77 w */     {{0x00,0x00,0x66,0x66,0x7e,0x7e,0x6c,0x00}},
    /* 0x78 x */     {{0x00,0x00,0x66,0x3c,0x18,0x3c,0x66,0x00}},
    /* 0x79 y */     {{0x00,0x00,0x66,0x66,0x66,0x3e,0x06,0x3c}},
    /* 0x7a z */     {{0x00,0x00,0x7e,0x0c,0x18,0x30,0x7e,0x00}},
    /* 0x7b { */     {{0x0c,0x18,0x18,0x70,0x18,0x18,0x0c,0x00}},
    /* 0x7c | */     {{0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x00}},
    /* 0x7d } */     {{0x30,0x18,0x18,0x0e,0x18,0x18,0x30,0x00}},
    /* 0x7e ~ */     {{0x34,0x58,0x00,0x00,0x00,0x00,0x00,0x00}},
};

/* ---------------------------------------------------------------------------
 * Surface / DC model.
 *
 * An HDC is a GdiSurface that wraps an SDL3 8-bit indexed surface
 * (SDL_PIXELFORMAT_INDEX8). All GDI surfaces created by this layer share one
 * 256-entry palette (the indexed colour table), mirroring how the original
 * game sets the DIB colour table once and composites everything in that space.
 * ------------------------------------------------------------------------- */

#define SURFACE_MAGIC 0x47445253u  /* "SDRG" */
#define BRUSH_MAGIC   0x48524253u  /* "SBRH" */

typedef struct GdcState
{
    u32 textColor;      /* palette index used by text drawing */
    u32 bkColor;        /* palette index used for opaque background */
    s32 bkMode;         /* GDI_OPAQUE or GDI_TRANSPARENT */
} GdcState;

struct GdiSurface
{
    u32 magic;
    SDL_Surface *sdl;   /* INDEX8 backing store */
    GdcState state;     /* per-DC drawing state */
};

struct GdiBrush
{
    u32 magic;
    u32 color;          /* palette index */
};

/* A resource blob read from the resource-only DLL asset (.dib). Holds the raw
 * bytes (BITMAPINFOHEADER + palette + pixels) plus opaque id/labels so the
 * resource API can be driven by integer ids (DASHRES) or names. */
struct GdiResource
{
    u32 magic;
    u32 id;             /* numeric id when loaded by MAKEINTRESOURCE */
    char *name;         /* owned label when loaded by name (else NULL) */
    char *type;         /* owned type label when loaded by name (else NULL) */
    u8 *data;           /* owned raw resource bytes */
    u32 size;           /* byte size of data */
};

/* A loaded resource-only DLL: owns a list of resources read from disk. */
struct GdiModule
{
    u32 magic;
    u32 resource_count;
    GdiResource *resources;
};

#define MODULE_MAGIC 0x4c444f4du   /* "MODL" */
#define RESOURCE_MAGIC 0x4553524du /* "MRSE" */

static GdiSurface *g_screen = NULL;

static SDL_Palette *g_palette = NULL;
static RGBQUAD g_paletteStore[256];

static GdiSurface *surface_new(SDL_Surface *sdl)
{
    GdiSurface *s = (GdiSurface *)SDL_malloc(sizeof(GdiSurface));
    if (s == NULL)
        return NULL;
    s->magic = SURFACE_MAGIC;
    s->sdl = sdl;
    s->state.textColor = 255;
    s->state.bkColor = 0;
    s->state.bkMode = GDI_TRANSPARENT;
    return s;
}

static void palette_init_default(SDL_Palette *pal, RGBQUAD *store)
{
    u32 i;
    for (i = 0; i < 256; i++)
    {
        u8 v = (u8)i;
        store[i].blue = v;
        store[i].green = v;
        store[i].red = v;
        store[i].reserved = 0;
    }
    /* Sync into the SDL palette so indexed blits resolve correctly. */
    SDL_Color colors[256];
    for (i = 0; i < 256; i++)
    {
        colors[i].r = store[i].red;
        colors[i].g = store[i].green;
        colors[i].b = store[i].blue;
        colors[i].a = 255;
    }
    SDL_SetPaletteColors(pal, colors, 0, 256);
}

/* Ensure the shared palette exists, sized from the biggest request so far. */
static SDL_Palette *shared_palette(void)
{
    if (g_palette == NULL)
    {
        g_palette = SDL_CreatePalette(256);
        if (g_palette == NULL)
            return NULL;
        palette_init_default(g_palette, g_paletteStore);
    }
    return g_palette;
}

HDC __cdecl CreateCompatibleDC(HDC hdc)
{
    (void)hdc;
    return NULL;
}

HBITMAP __cdecl CreateDIBSection(HDC hdc, const BITMAPINFO *pbmi,
                                 u32 usage, void **ppBits, void *section, u32 offset)
{
    (void)hdc;
    (void)usage;
    (void)section;
    (void)offset;
    if (pbmi == NULL || ppBits == NULL)
        return NULL;

    s32 w = pbmi->bmiHeader.biWidth;
    s32 h = pbmi->bmiHeader.biHeight;
    if (w <= 0 || h <= 0)
        return NULL;
    if (pbmi->bmiHeader.biBitCount != 0 && pbmi->bmiHeader.biBitCount != 8)
        return NULL;

    SDL_Palette *pal = shared_palette();
    if (pal == NULL)
        return NULL;

    SDL_Surface *sdl = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_INDEX8);
    if (sdl == NULL)
        return NULL;
    SDL_SetSurfacePalette(sdl, pal);
    SDL_SetSurfaceColorspace(sdl, SDL_COLORSPACE_SRGB);

    if (pbmi->bmiHeader.biClrUsed != 0 ||
        (pbmi->bmiHeader.biSize >= sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)))
    {
        /* Seed the palette store from the DIB's colour table. */
        u32 i;
        SDL_Color colors[256];
        u32 n = pbmi->bmiHeader.biClrUsed;
        if (n == 0)
            n = 256;
        if (n > 256)
            n = 256;
        memcpy(g_paletteStore, pbmi->bmiColors, n * sizeof(RGBQUAD));
        for (i = 0; i < 256; i++)
        {
            colors[i].r = g_paletteStore[i].red;
            colors[i].g = g_paletteStore[i].green;
            colors[i].b = g_paletteStore[i].blue;
            colors[i].a = 255;
        }
        SDL_SetPaletteColors(pal, colors, 0, 256);
    }

    GdiSurface *s = surface_new(sdl);
    if (s == NULL)
    {
        SDL_DestroySurface(sdl);
        return NULL;
    }
    *ppBits = sdl->pixels;
    return s;
}

u8 __cdecl DeleteDC(HDC hdc)
{
    (void)hdc;
    return 1;
}

u8 __cdecl DeleteObject(HGDIOBJ hgdiobj)
{
    if (hgdiobj == NULL)
        return 0;
    /* Do not allow deleting the shared screen surface on accident. */
    if (hgdiobj == (HGDIOBJ)g_screen)
        return 0;

    GdiSurface *s = hgdiobj;
    if (s->magic == SURFACE_MAGIC)
    {
        SDL_DestroySurface(s->sdl);
        SDL_free(s);
        return 1;
    }
    if (s->magic == BRUSH_MAGIC)
    {
        SDL_free(s);
        return 1;
    }
    return 0;
}

u32 __cdecl SetDIBColorTable(HDC hdc, u32 iStart, u32 cEntries, const RGBQUAD *pColors)
{
    (void)hdc;
    SDL_Palette *pal = shared_palette();
    if (pal == NULL || pColors == NULL)
        return 0;
    if (iStart > 256)
        return 0;
    if (cEntries > 256 - iStart)
        cEntries = 256 - iStart;
    if (cEntries == 0)
        return 0;

    u32 i;
    SDL_Color colors[256];
    memcpy(&g_paletteStore[iStart], pColors, cEntries * sizeof(RGBQUAD));
    for (i = 0; i < cEntries; i++)
    {
        colors[i].r = g_paletteStore[iStart + i].red;
        colors[i].g = g_paletteStore[iStart + i].green;
        colors[i].b = g_paletteStore[iStart + i].blue;
        colors[i].a = 255;
    }
    if (!SDL_SetPaletteColors(pal, colors, (int)iStart, (int)cEntries))
        return 0;
    return cEntries;
}

u32 __cdecl GetDIBColorTable(HDC hdc, u32 iStart, u32 cEntries, RGBQUAD *pColors)
{
    (void)hdc;
    if (pColors == NULL)
        return 0;
    if (iStart > 256)
        return 0;
    if (cEntries > 256 - iStart)
        cEntries = 256 - iStart;
    memcpy(pColors, &g_paletteStore[iStart], cEntries * sizeof(RGBQUAD));
    return cEntries;
}

u8 __cdecl BitBlt(HDC dst, s32 dx, s32 dy, s32 w, s32 h,
                  HDC src, s32 sx, s32 sy, u32 rop)
{
    GdiSurface *d = dst;
    GdiSurface *s = src;
    if (d == NULL || s == NULL)
        return 0;
    if (rop != GDI_SRCCOPY)
        return 0;

    SDL_Rect srcrect = { sx, sy, w, h };
    SDL_Rect dstrect = { dx, dy, w, h };
    /* src and dst share the same palette, so this is a straight index copy. */
    if (!SDL_BlitSurface(s->sdl, &srcrect, d->sdl, &dstrect))
        return 0;
    return 1;
}

u8 __cdecl StretchBlt(HDC dst, s32 dx, s32 dy, s32 dw, s32 dh,
                      HDC src, s32 sx, s32 sy, s32 sw, s32 sh, u32 rop)
{
    GdiSurface *d = dst;
    GdiSurface *s = src;
    if (d == NULL || s == NULL || dw <= 0 || dh <= 0 || sw == 0 || sh == 0)
        return 0;
    if (rop != GDI_SRCCOPY)
        return 0;

    SDL_Rect srcrect = { sx, sy, sw, sh };
    SDL_Rect dstrect = { dx, dy, dw, dh };
    if (!SDL_BlitSurfaceScaled(s->sdl, &srcrect, d->sdl, &dstrect, SDL_SCALEMODE_NEAREST))
        return 0;
    return 1;
}

static int glyph_index(char c)
{
    int idx = (int)(u8)c - G_FIRST;
    if (idx < 0 || idx >= G_COUNT)
        return -1;
    return idx;
}

/* Draw one glyph with its top-left at (x, y) into the INDEX8 pixel array. */
static void draw_glyph(GdiSurface *s, s32 x, s32 y, int gi,
                       u32 color, s32 bkMode, u32 bkColor)
{
    int r, c;
    if (gi < 0 || s->sdl == NULL || s->sdl->pixels == NULL)
        return;
    const Glyph8 *g = &g_font[gi];
    int w = s->sdl->w;
    int h = s->sdl->h;
    int pitch = s->sdl->pitch;
    u8 *base = (u8 *)s->sdl->pixels;

    for (r = 0; r < 8; r++)
    {
        s32 py = y + r;
        if (py < 0 || py >= h)
            continue;
        u8 *row = base + (u32)py * (u32)pitch;
        u8 bits = g->rows[r];
        for (c = 0; c < 8; c++)
        {
            s32 px = x + c;
            if (px < 0 || px >= w)
                continue;
            if (bits & (0x80u >> c))
                row[px] = (u8)color;
            else if (bkMode == GDI_OPAQUE)
                row[px] = (u8)bkColor;
        }
    }
}

u8 __cdecl TextOutA(HDC hdc, s32 x, s32 y, const char *str, s32 count)
{
    GdiSurface *s = hdc;
    if (s == NULL || str == NULL)
        return 0;
    if (count < 0)
        count = (s32)strlen(str);
    s32 cx = x;
    s32 i;
    for (i = 0; i < count; i++)
    {
        char ch = str[i];
        if (ch == '\n')
        {
            cx = x;
            continue;
        }
        draw_glyph(s, cx, y, glyph_index(ch), s->state.textColor, s->state.bkMode, s->state.bkColor);
        cx += 8;
    }
    return 1;
}

u8 __cdecl ExtTextOutA(HDC hdc, s32 x, s32 y, u32 options, const GDI_RECT *clip,
                       const char *str, u32 count, const s32 *dx)
{
    GdiSurface *s = hdc;
    if (s == NULL || str == NULL)
        return 0;
    (void)options;
    (void)clip;
    s32 cx = x;
    u32 i;
    for (i = 0; i < count; i++)
    {
        draw_glyph(s, cx, y, glyph_index(str[i]), s->state.textColor, s->state.bkMode, s->state.bkColor);
        cx += (dx != NULL) ? dx[i] : 8;
    }
    return 1;
}

s32 __cdecl DrawTextA(HDC hdc, const char *str, s32 count, GDI_RECT *rect, u32 format)
{
    GdiSurface *s = hdc;
    if (s == NULL || str == NULL || rect == NULL)
        return 0;
    if (count < 0)
        count = (s32)strlen(str);

    s32 left = rect->left;
    s32 top = rect->top;
    s32 right = rect->right;
    s32 bottom = rect->bottom;
    s32 maxWidth = right - left;
    s32 usableWidth = (maxWidth <= 0) ? 0x7fffffff : maxWidth;

    s32 cx = left;
    s32 cy = top;
    s32 i;
    s32 lineHeight = 9;
    s32 lineCount = 1;

    for (i = 0; i < count; i++)
    {
        char ch = str[i];
        if (ch == '\n')
        {
            cy += lineHeight;
            cx = left;
            lineCount++;
            continue;
        }
        if (usableWidth != 0x7fffffff && (cx + 8 > left + usableWidth) && cx > left)
        {
            cy += lineHeight;
            cx = left;
            lineCount++;
        }
        draw_glyph(s, cx, cy, glyph_index(ch), s->state.textColor, s->state.bkMode, s->state.bkColor);
        cx += 8;
    }

    if (format & GDI_DT_SINGLELINE)
    {
        s32 th = cy + 8 - top;
        if (bottom - top >= th)
            top += ((bottom - top - th) / 2);
    }
    (void)top;

    if (format & GDI_DT_NOCLIP)
        return (s32)strlen(str);

    if (bottom - top <= 0)
        lineCount = 0;
    return lineCount * lineHeight;
}

u8 __cdecl FillRect(HDC hdc, const GDI_RECT *rect, HBRUSH brush)
{
    GdiSurface *s = hdc;
    if (s == NULL || rect == NULL || s->sdl == NULL)
        return 0;
    u32 color = 0;
    if (brush != NULL)
        color = brush->color;

    SDL_Rect r = { rect->left, rect->top,
                   rect->right - rect->left, rect->bottom - rect->top };
    /* On an INDEX8 surface "a pixel of the format" is simply the palette index. */
    if (!SDL_FillSurfaceRect(s->sdl, &r, color))
        return 0;
    return 1;
}

u32 __cdecl SetTextColor(HDC hdc, u32 color)
{
    GdiSurface *s = hdc;
    if (s == NULL)
        return 0;
    u32 old = s->state.textColor;
    s->state.textColor = color;
    return old;
}

u32 __cdecl SetBkColor(HDC hdc, u32 color)
{
    GdiSurface *s = hdc;
    if (s == NULL)
        return 0;
    u32 old = s->state.bkColor;
    s->state.bkColor = color;
    return old;
}

s32 __cdecl SetBkMode(HDC hdc, s32 mode)
{
    GdiSurface *s = hdc;
    if (s == NULL)
        return 0;
    s32 old = s->state.bkMode;
    s->state.bkMode = mode;
    return old;
}

HBRUSH __cdecl CreateSolidBrush(u32 color)
{
    GdiBrush *b = (GdiBrush *)SDL_malloc(sizeof(GdiBrush));
    if (b == NULL)
        return NULL;
    b->magic = BRUSH_MAGIC;
    b->color = color;
    return b;
}

/* ---------------------------------------------------------------------------
 * Win32 resource API (resource-only DLL hosting).
 *
 * DASHRES.DLL contains no code — it is purely a bundle of 8-bpp DIB bitmaps
 * (resources 0x81..0x86) that HOVER.EXE loads at runtime with
 * LoadLibraryA/LoadResource/LockResource, then copies into a DIB section and
 * renders through GDI. This layer provides the same resource API surface, so
 * the reconstructed dashboard loads the *real* DASHRES.DLL, parses its PE
 * resource directory, and drives the same FindResourceA/CreateDIBSection/
 * BitBlt pipeline. As a fallback it can also serve a directory of the same
 * bitmaps split into dash_XX.dib files (BITMAPINFOHEADER + palette + pixels).
 * ------------------------------------------------------------------------- */

/* Number of DIB resources in the DASHRES set (ids 0x81..0x86). */
#define DASHRES_FIRST_ID 0x81
#define DASHRES_COUNT 6

#define PE_MAX_SECTIONS 16
#define PE_MAX_RESOURCES 4096
#define IMAGE_DIRECTORY_ENTRY_RESOURCE 2

static void resource_free(GdiResource *r)
{
    SDL_free(r->name);
    SDL_free(r->type);
    SDL_free(r->data);
}

static void module_free(GdiModule *m)
{
    u32 i;
    if (m == NULL)
        return;
    if (m->magic == MODULE_MAGIC)
    {
        for (i = 0; i < m->resource_count; i++)
            resource_free(&m->resources[i]);
        SDL_free(m->resources);
    }
    SDL_free(m);
}

typedef struct PeSection
{
    u32 va;
    u32 vsz;
    u32 raw_off;
    u32 raw_sz;
} PeSection;

typedef struct PeMap
{
    const u8 *base;
    u32 size;
    u32 nsect;
    u32 first_va; /* VA of the first (lowest) section: anything below is
                     mapped at file offset == RVA (DOS/PE headers) */
    PeSection sect[PE_MAX_SECTIONS];
} PeMap;

static u16 pe_rd_u16(const u8 *p)
{
    u16 v;
    SDL_memcpy(&v, p, 2);
    return v;
}

static u32 pe_rd_u32(const u8 *p)
{
    u32 v;
    SDL_memcpy(&v, p, 4);
    return v;
}

/* Map a PE relative virtual address to a validated pointer into the flat
 * on-disk image (sections are mapped by their raw data). */
static const u8 *pe_ptr(const PeMap *pm, u32 rva, u32 len)
{
    u32 i;

    /* DOS/PE headers are mapped at file offset == RVA (below the first
     * section). Everything else must be translated through the raw data of
     * the section that contains the RVA; the resource directory lives in
     * .rsrc whose file offset differs from its virtual address. */
    if (rva < pm->first_va && (u64)rva + len <= (u64)pm->size)
        return pm->base + (u64)rva;
    for (i = 0; i < pm->nsect; i++)
    {
        const PeSection *s = &pm->sect[i];
        if ((u64)rva >= s->va && (u64)rva + len <= (u64)s->va + s->vsz)
        {
            u64 off = (u64)s->raw_off + ((u64)rva - s->va);
            if (off + len <= pm->size)
                return pm->base + off;
            return NULL;
        }
    }
    return NULL;
}

/* Copy a UTF-16 resource-name string into an ASCII buffer (resources here are
 * plain ANSI ids; named entries are best-effort ported to bytes). */
static void pe_name_string(const PeMap *pm, u32 base_rva, u32 off, char *out,
                           u32 outsz)
{
    const u8 *p = pe_ptr(pm, base_rva + (off & 0x7fffffffu), 2);
    u16 len;
    u32 i;

    if (p == NULL)
    {
        out[0] = '\0';
        return;
    }
    len = pe_rd_u16(p);
    for (i = 0; i < outsz - 1 && i < len; i++)
    {
        const u8 *c = pe_ptr(pm, base_rva + (off & 0x7fffffffu) + 2 + i * 2, 2);
        if (c == NULL)
            break;
        out[i] = (char)(pe_rd_u16(c) & 0x7f);
    }
    out[i] = '\0';
}

static void pe_add_resource(GdiModule *m, u32 id, const char *name,
                            const char *type, const u8 *data, u32 size)
{
    GdiResource *r;
    GdiResource *nr;

    nr = (GdiResource *)SDL_realloc(m->resources,
                                    (m->resource_count + 1) *
                                        sizeof(GdiResource));
    if (nr == NULL)
        return;
    m->resources = nr;
    r = &m->resources[m->resource_count];
    SDL_memset(r, 0, sizeof(*r));
    r->magic = RESOURCE_MAGIC;
    r->id = id;
    if (name != NULL)
        r->name = SDL_strdup(name);
    if (type != NULL)
        r->type = SDL_strdup(type);
    r->size = size;
    r->data = (u8 *)SDL_malloc(size == 0 ? 1 : size);
    if (r->data == NULL)
    {
        resource_free(r);
        return;
    }
    SDL_memcpy(r->data, data, size);
    m->resource_count++;
}

/* Walk the PE resource directory (type/name/language/data) and collect every
 * embedded resource blob into the module. Offsets below the root are relative
 * to base_rva (the resource directory's RVA); data-entry RVAs are absolute. */
static void pe_walk(const PeMap *pm, GdiModule *m, u32 base_rva, u32 dir_rva,
                    u32 depth, u32 type_id, char *type_label, u32 name_id,
                    char *name_label)
{
    const u8 *dir;
    u16 named;
    u16 idc;
    u16 n;
    u16 k;

    if (depth >= 3)
    {
        /* Leaf: dir_rva is the address of a data entry (rva/size/codepage). */
        const u8 *de = pe_ptr(pm, dir_rva, 16);
        u32 rva;
        u32 sz;
        const u8 *data;
        if (de == NULL)
            return;
        rva = pe_rd_u32(de);
        sz = pe_rd_u32(de + 4);
        if (sz == 0)
            return;
        data = pe_ptr(pm, rva, sz);
        if (data == NULL)
            return;
        pe_add_resource(m, name_id, name_label, type_label, data, sz);
        return;
    }

    dir = pe_ptr(pm, dir_rva, 16);
    if (dir == NULL)
        return;
    if (m->resource_count >= PE_MAX_RESOURCES)
        return;
    named = pe_rd_u16(dir + 12);
    idc = pe_rd_u16(dir + 14);
    n = (u16)(named + idc);
    if (n > 1024)
        return;
    for (k = 0; k < n; k++)
    {
        const u8 *e = pe_ptr(pm, dir_rva + 16 + (u32)k * 8, 8);
        u32 nm;
        u32 o;
        u32 sub;
        bool is_dir;

        if (e == NULL)
            continue;
        nm = pe_rd_u32(e);
        o = pe_rd_u32(e + 4);
        is_dir = (o & 0x80000000u) != 0;
        sub = o & 0x7fffffffu;

        if (depth == 0)
        {
            char tlabel[64];
            if (nm & 0x80000000u)
            {
                pe_name_string(pm, base_rva, nm, tlabel, sizeof(tlabel));
                pe_walk(pm, m, base_rva, base_rva + sub, 1, 0, tlabel, 0,
                        NULL);
            }
            else
            {
                if (nm == 2)
                    SDL_strlcpy(tlabel, "BITMAP", sizeof(tlabel));
                else
                    (void)SDL_snprintf(tlabel, sizeof(tlabel), "RT_%u", nm);
                pe_walk(pm, m, base_rva, base_rva + sub, 1, nm, tlabel, 0,
                        NULL);
            }
        }
        else if (depth == 1)
        {
            if (nm & 0x80000000u)
            {
                char nlabel[64];
                pe_name_string(pm, base_rva, nm, nlabel, sizeof(nlabel));
                pe_walk(pm, m, base_rva, base_rva + sub, 2, type_id,
                        type_label, 0, nlabel);
            }
            else
            {
                pe_walk(pm, m, base_rva, base_rva + sub, 2, type_id,
                        type_label, nm, NULL);
            }
        }
        else if (depth == 2)
        {
            /* Language level: the entry points at either a nested directory
             * (another resource level) or, as in DASHRES, directly at a data
             * entry (16-byte rva/size record). */
            pe_walk(pm, m, base_rva, base_rva + sub, 3, type_id, type_label,
                    name_id, name_label);
        }
    }
}

/* Parse a loaded PE image's resource directory into a new GdiModule. Resource
 * payloads are copied out, so the caller may free the image buffer. */
static bool pe_build_module(const u8 *base, u32 size, GdiModule **out)
{
    GdiModule *m = NULL;
    PeMap pm;
    u32 e_lfanew;
    u32 nsect;
    u32 opt_off;
    u16 optmagic;
    u32 dd_off;
    u32 rsrc_rva;
    u16 ndd;
    u32 i;

    *out = NULL;
    SDL_memset(&pm, 0, sizeof(pm));
    pm.base = base;
    pm.size = size;

    if (size < 0x40 || pe_rd_u16(base + 0) != 0x5a4du) /* "MZ" */
        return false;
    e_lfanew = pe_rd_u32(base + 0x3c);
    if (e_lfanew == 0 || (u64)e_lfanew + 24 > size)
        return false;
    if (pe_rd_u32(base + e_lfanew) != 0x00004550u) /* "PE\0\0" */
        return false;

    nsect = pe_rd_u16(base + e_lfanew + 6);
    if (nsect == 0 || nsect > PE_MAX_SECTIONS)
        return false;
    opt_off = e_lfanew + 24;
    if ((u64)opt_off + 110 > size)
        return false;
    optmagic = pe_rd_u16(base + opt_off);
    if (optmagic != 0x10bu && optmagic != 0x20bu)
        return false;

    ndd = pe_rd_u16(base + opt_off + (optmagic == 0x10b ? 92 : 108));
    if (ndd <= IMAGE_DIRECTORY_ENTRY_RESOURCE)
        return false;

    dd_off = opt_off + (optmagic == 0x10b ? 96 : 112) +
             IMAGE_DIRECTORY_ENTRY_RESOURCE * 8;
    rsrc_rva = pe_rd_u32(base + dd_off);

    {
        u32 sect_off = opt_off + (optmagic == 0x10b ? 224 : 240);
        for (i = 0; i < nsect; i++)
        {
            const u8 *sh = base + sect_off + i * 40;
            PeSection *s;
            if ((u64)sect_off + (u64)i * 40 + 40 > size)
                return false;
            s = &pm.sect[pm.nsect];
            s->va = pe_rd_u32(sh + 12);
            s->vsz = pe_rd_u32(sh + 8);
            s->raw_off = pe_rd_u32(sh + 20);
            s->raw_sz = pe_rd_u32(sh + 16);
            if (pm.nsect == 0)
                pm.first_va = s->va;
            pm.nsect++;
        }
    }

    m = (GdiModule *)SDL_calloc(1, sizeof(GdiModule));
    if (m == NULL)
        return false;

    pe_walk(&pm, m, rsrc_rva, rsrc_rva, 0, 0, NULL, 0, NULL);

    if (m->resource_count == 0)
    {
        module_free(m);
        return false;
    }
    m->magic = MODULE_MAGIC;
    *out = m;
    return true;
}

static bool path_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

/* Load a set of raw dash_XX.dib files from a directory. */
static HINSTANCE load_dib_directory(const char *path)
{
    GdiModule *m = NULL;
    char fname[520];
    u32 n = 0;
    u32 id;

    for (id = DASHRES_FIRST_ID; id < DASHRES_FIRST_ID + DASHRES_COUNT; id++)
    {
        FILE *f;
        long sz;
        u8 *buf;

        (void)SDL_snprintf(fname, sizeof(fname), "%s/dash_%02x.dib", path, id);
        f = fopen(fname, "rb");
        if (f == NULL)
            continue;

        if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
            fseek(f, 0, SEEK_SET) != 0)
        {
            fclose(f);
            continue;
        }
        buf = (u8 *)SDL_malloc((size_t)sz);
        if (buf == NULL)
        {
            fclose(f);
            continue;
        }
        if (fread(buf, 1, (size_t)sz, f) != (size_t)sz)
        {
            SDL_free(buf);
            fclose(f);
            continue;
        }
        fclose(f);

        if (m == NULL)
            m = (GdiModule *)SDL_calloc(1, sizeof(GdiModule));
        if (m == NULL)
        {
            SDL_free(buf);
            break;
        }

        m->resources = (GdiResource *)SDL_realloc(
            m->resources, (n + 1) * sizeof(GdiResource));
        if (m->resources == NULL)
        {
            SDL_free(buf);
            break;
        }
        m->resources[n].magic = RESOURCE_MAGIC;
        m->resources[n].id = id;
        m->resources[n].name = NULL;
        m->resources[n].type = NULL;
        m->resources[n].data = buf;
        m->resources[n].size = (u32)sz;
        n++;
    }

    if (m == NULL)
        return NULL;
    if (n == 0)
    {
        SDL_free(m);
        return NULL;
    }
    m->magic = MODULE_MAGIC;
    m->resource_count = n;
    return m;
}

/* Load a Win32 module by path, like the retail HOVER.EXE's LoadLibraryA. The
 * compat loader accepts:
 *   - a real PE DLL (DASHRES.DLL) whose resource directory is parsed in place,
 *   - a directory containing DASHRES.DLL,
 *   - a directory of raw dash_XX.dib files (legacy layout). */
HINSTANCE __cdecl LoadLibraryA(const char *path)
{
    GdiModule *m = NULL;
    FILE *f = NULL;
    long size = 0;
    u8 *buf = NULL;

    if (path == NULL)
        return NULL;

    if (path_is_regular(path))
    {
        f = fopen(path, "rb");
        if (f == NULL)
            return NULL;
        if (fseek(f, 0, SEEK_END) != 0 || (size = ftell(f)) < 0 ||
            fseek(f, 0, SEEK_SET) != 0 || size < 0x40)
        {
            fclose(f);
            return NULL;
        }
        buf = (u8 *)SDL_malloc((size_t)size);
        if (buf == NULL)
        {
            fclose(f);
            return NULL;
        }
        if (fread(buf, 1, (size_t)size, f) != (size_t)size)
        {
            SDL_free(buf);
            fclose(f);
            return NULL;
        }
        fclose(f);

        if (!pe_build_module(buf, (u32)size, &m))
        {
            SDL_free(buf);
            return NULL;
        }
        SDL_free(buf);
        return m;
    }

    /* Directory: a real DASHRES.DLL wins over the legacy split .dib set. */
    {
        char dllpath[520];
        (void)SDL_snprintf(dllpath, sizeof(dllpath), "%s/DASHRES.DLL", path);
        if (path_is_regular(dllpath))
            return LoadLibraryA(dllpath);
        return load_dib_directory(path);
    }
}

void __cdecl FreeLibrary(HINSTANCE hmodule)
{
    module_free((GdiModule *)hmodule);
}

static GdiResource *find_resource_by_id(GdiModule *m, u32 id)
{
    u32 i;
    for (i = 0; i < m->resource_count; i++)
        if (m->resources[i].id == id)
            return &m->resources[i];
    return NULL;
}

static GdiResource *find_resource_by_name(GdiModule *m, const char *name,
                                          const char *type)
{
    u32 i;
    for (i = 0; i < m->resource_count; i++)
    {
        GdiResource *r = &m->resources[i];
        if (r->name != NULL && name != NULL &&
            SDL_strcmp(r->name, name) == 0 &&
            ((type == NULL) || (r->type != NULL && SDL_strcmp(r->type, type) == 0)))
            return r;
    }
    return NULL;
}

static bool is_int_resource(const void *ptr)
{
    return ((uintptr_t)ptr >> 16) == 0;
}

HRSRC __cdecl FindResourceA(HINSTANCE hmodule, const char *name, const char *type)
{
    GdiModule *m = (GdiModule *)hmodule;
    if (m == NULL || m->magic != MODULE_MAGIC || name == NULL)
        return NULL;
    if (is_int_resource(name))
        return find_resource_by_id(m, (u32)(uintptr_t)name);
    return find_resource_by_name(m, name, type);
}

u32 __cdecl SizeofResource(HINSTANCE hmodule, HRSRC hres)
{
    GdiResource *r = (GdiResource *)hres;
    (void)hmodule;
    if (r == NULL || r->magic != RESOURCE_MAGIC)
        return 0;
    return r->size;
}

HGLOBAL __cdecl LoadResource(HINSTANCE hmodule, HRSRC hres)
{
    GdiResource *r = (GdiResource *)hres;
    (void)hmodule;
    if (r == NULL || r->magic != RESOURCE_MAGIC)
        return NULL;
    return r;
}

const void * __cdecl LockResource(HGLOBAL hres)
{
    GdiResource *r = (GdiResource *)hres;
    if (r == NULL || r->magic != RESOURCE_MAGIC)
        return NULL;
    return r->data;
}

u32 __cdecl GdiCompat_SetDibPalette(const void *dib)
{
    const u8 *src = (const u8 *)dib;
    BITMAPINFOHEADER hdr;
    SDL_Palette *pal;
    SDL_Color colors[256];
    u32 i;

    if (src == NULL)
        return 0;
    SDL_memcpy(&hdr, src, sizeof(BITMAPINFOHEADER));
    if (hdr.biSize < sizeof(BITMAPINFOHEADER) || hdr.biBitCount != 8 ||
        hdr.biPlanes != 1)
        return 0;

    pal = shared_palette();
    if (pal == NULL)
        return 0;

    /* Copy the DIB's own colour table into the shared palette store. */
    memcpy(g_paletteStore, src + hdr.biSize, 256 * sizeof(RGBQUAD));
    for (i = 0; i < 256; i++)
    {
        colors[i].r = g_paletteStore[i].red;
        colors[i].g = g_paletteStore[i].green;
        colors[i].b = g_paletteStore[i].blue;
        colors[i].a = 255;
    }
    SDL_SetPaletteColors(pal, colors, 0, 256);
    return 1;
}

HDC __cdecl GdiCompat_CreateDibSurface(const void *dib)
{
    const u8 *src = (const u8 *)dib;
    BITMAPINFOHEADER hdr;
    s32 w, h;
    u32 pal_off, pix_off, row_stride;
    SDL_Surface *sdl;
    GdiSurface *s;
    SDL_Palette *pal;

    if (src == NULL)
        return NULL;

    /* Parse the BITMAPINFOHEADER (biSize=40 header first). */
    SDL_memcpy(&hdr, src, sizeof(BITMAPINFOHEADER));
    if (hdr.biSize < sizeof(BITMAPINFOHEADER) || hdr.biBitCount != 8 ||
        hdr.biPlanes != 1)
        return NULL;
    w = hdr.biWidth;
    h = hdr.biHeight;
    if (w <= 0 || h == 0)
        return NULL;
    if (w > 2048 || h > 2048 || h < -2048)
        return NULL;

    pal_off = hdr.biSize;
    pix_off = pal_off + 256 * 4;
    row_stride = (u32)(((w * 8 + 31) / 32) * 4);   /* DWORD-align rows (8bpp) */

    if (GdiCompat_SetDibPalette(src) == 0)
        return NULL;

    pal = shared_palette();
    if (pal == NULL)
        return NULL;

    sdl = SDL_CreateSurface(w, h < 0 ? -h : h, SDL_PIXELFORMAT_INDEX8);
    if (sdl == NULL)
        return NULL;
    SDL_SetSurfacePalette(sdl, pal);
    SDL_SetSurfaceColorspace(sdl, SDL_COLORSPACE_SRGB);

    {
        const u8 *pixels = src + pix_off;
        u8 *dst = (u8 *)sdl->pixels;
        u32 dst_pitch = (u32)sdl->pitch;
        u32 rows = (u32)(h < 0 ? -h : h);
        u32 r;
        for (r = 0; r < rows; r++)
        {
            u32 src_y = (h > 0) ? (rows - 1 - r) : r;  /* DIBs bottom-up */
            memcpy(dst + (u64)r * dst_pitch, pixels + (u64)src_y * row_stride,
                   (u32)w);
        }
    }

    s = surface_new(sdl);
    if (s == NULL)
    {
        SDL_DestroySurface(sdl);
        return NULL;
    }
    return s;
}

HDC __cdecl GdiCompat_GetScreenDC(u32 width, u32 height,
                                  const RGBQUAD *palette, u32 paletteSize)
{
    if (g_screen != NULL)
        return g_screen;

    SDL_Palette *pal = shared_palette();
    if (pal == NULL)
        return NULL;

    SDL_Surface *sdl = SDL_CreateSurface((int)width, (int)height, SDL_PIXELFORMAT_INDEX8);
    if (sdl == NULL)
        return NULL;
    SDL_SetSurfacePalette(sdl, pal);
    SDL_SetSurfaceColorspace(sdl, SDL_COLORSPACE_SRGB);

    if (palette != NULL)
    {
        u32 i;
        SDL_Color colors[256];
        if (paletteSize > 256)
            paletteSize = 256;
        memcpy(g_paletteStore, palette, paletteSize * sizeof(RGBQUAD));
        for (i = 0; i < 256; i++)
        {
            colors[i].r = g_paletteStore[i].red;
            colors[i].g = g_paletteStore[i].green;
            colors[i].b = g_paletteStore[i].blue;
            colors[i].a = 255;
        }
        SDL_SetPaletteColors(pal, colors, 0, 256);
    }

    GdiSurface *s = surface_new(sdl);
    if (s == NULL)
    {
        SDL_DestroySurface(sdl);
        return NULL;
    }
    g_screen = s;
    return s;
}

void __cdecl GdiCompat_Present(HDC screen, u8 *dst, u32 dstPitch,
                               u32 dstWidth, u32 dstHeight)
{
    GdiSurface *s = screen;
    if (s == NULL || s->sdl == NULL || dst == NULL)
        return;

    u32 y, x;
    u32 sw = (u32)s->sdl->w;
    u32 sh = (u32)s->sdl->h;
    u32 spitch = (u32)s->sdl->pitch;
    const u8 *pixels = (const u8 *)s->sdl->pixels;

    /* Map each palette index through the colour table into the RGBA32 back
     * buffer. This is the analogue of GDI presenting the DIB to the window. */
    for (y = 0; y < sh && y < dstHeight; y++)
    {
        const u8 *sr = pixels + (u64)y * spitch;
        u8 *dr = dst + (u64)y * dstPitch;
        for (x = 0; x < sw && x < dstWidth; x++)
        {
            const RGBQUAD *q = &g_paletteStore[sr[x]];
            u8 *out = dr + (u64)x * 4u;
            out[0] = q->red;
            out[1] = q->green;
            out[2] = q->blue;
            out[3] = 255;
        }
    }
}