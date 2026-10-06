#include "flyingdodgem_types.h"

/* ---------------------------------------------------------------------------
 * Wall/floor/span renderer (retail .text around 0x401ad0-0x407430).  The
 * span setup path computes viewport scratch, light-table shade and texture
 * shift for one span, then hands off to F403850 (row offset table) and the
 * draw-host dispatch.
 * ------------------------------------------------------------------------- */

extern int32_t g_4a453c;
extern int32_t g_4a4540;
extern void* g_4a4548;
extern int32_t g_4a4a14;
extern int32_t g_4a4a1c;
extern int32_t g_4a4a34;
extern int32_t g_4a4a38;
extern int32_t g_4a4a3c;
extern int32_t g_4a4a40;
extern int32_t g_4a4a44;
extern int32_t g_4a4a4c;
extern int32_t g_4a4a50;
extern void* g_4a1d68;
extern int32_t g_4a1d78;
extern int32_t g_461cfc;

extern int32_t __fastcall F403850(void* self, void* unused, int32_t arg);
extern void* __cdecl GetSub401140(void);

#if defined(_MSC_VER)
#pragma comment(lib, "gdi32.lib")
__declspec(dllimport)
#endif
int32_t __stdcall Rectangle(void* hdc, int32_t left, int32_t top, int32_t right, int32_t bottom);

#if defined(_MSC_VER)
__declspec(dllimport)
#endif
void* __stdcall SelectObject(void* hdc, void* obj);

typedef void (__fastcall *CdcVSlot)(void* self);
typedef void (__fastcall *CdcVSlotN)(void* self, void* unused, int32_t n);

static void __fastcall SelectPen450a09(void* self, void* unused, void* pen);
static void __fastcall SelectBrush450a65(void* self, void* unused, void* brush);
static void* __cdecl PrevObject4512ff(void* prev);

typedef void (__cdecl *HostFn0)(void);

struct HostObj
{
    HostFn0 fn0;
};

// FUNCTION: HOVER 0x00402990
void __fastcall SpanSetup2990(void* self, void* unused, void* a1, void* a2, int32_t a3, int32_t a4)
{
    uint8_t* o = (uint8_t*)a1;
    uint8_t* v = (uint8_t*)a2;
    int32_t total;
    int32_t f98;

    (void)self;
    (void)unused;
    total = g_4a453c - a3;
    total += a4;
    g_4a4a14 = total;
    if (total <= 0)
        return;
    g_4a4a4c = g_4a4540 + a3 * g_4a4a34;
    g_4a4a44 = *(int16_t*)(o + 0x8);
    if (*(int32_t*)(o + 0xa4) == 0)
    {
        uint8_t* child = *(uint8_t**)(o + 0x5c);
        uint8_t* hdr = *(uint8_t**)(o + 0x10);
        int32_t shade;
        int32_t rowbase;
        int32_t* row;
        int32_t f9c;
        int16_t tex;
        int32_t sh;
        int32_t mask;
        int32_t tbl;
        int32_t entry;
        int32_t x;
        int32_t span;
        int32_t packed;

        hdr[0x24] |= 8;
        if (g_4a1d78 != 0)
        {
            f9c = *(int32_t*)(o + 0x9c);
            shade = ((uint8_t*)g_4a4548)[(g_4a4a44 >> (uint8_t)(f9c + 4)) & 0x7ff];
            {
                int32_t lim = *(int32_t*)(child + 0x2c) - 1;
                if (lim < shade)
                    shade = lim;
                if (g_461cfc > shade)
                    shade = g_461cfc;
            }
        }
        else
        {
            shade = g_461cfc;
        }
        rowbase = *(int32_t*)(child + 0x28);
        row = (int32_t*)(rowbase + shade * 32);
        f9c = *(int32_t*)(o + 0x9c);
        tex = *(int16_t*)(o + 0xa);
        sh = (uint8_t)(f9c + shade);
        mask = (int16_t)(tex >> sh) & *row;
        tbl = *(int32_t*)((uint8_t*)row + 0x1c);
        entry = *(int32_t*)(tbl + mask * 8 + 4);
        x = *(int32_t*)(entry + 4);
        span = *(int32_t*)(v + 0xc) - *(int32_t*)(v + 0x8);
        *(int32_t*)(v + 0x10) = x;
        g_4a4a1c = x;
        *(int32_t*)(o + 0x98) = span != 0 ? (*(int32_t*)(o + 0x68) / span) >> sh : 0;
        *(int32_t*)(o + 0xa4) = 1;
        packed = (*(int32_t*)((uint8_t*)row + 4) << 16) + 0xffff;
        *(int32_t*)(o + 0x94) = packed;
        g_4a4a38 = packed;
    }
    else
    {
        g_4a4a38 = *(int32_t*)(o + 0x94);
        g_4a4a1c = *(int32_t*)(v + 0x10);
    }
    f98 = *(int32_t*)(o + 0x98);
    g_4a4a50 = f98;
    g_4a4a40 = (a3 - *(int32_t*)(v + 0x8)) * f98;
    g_4a4a3c = F403850(g_4a1d68, NULL, g_4a4a44);
    ((struct HostObj*)g_4a1d68)->fn0();
}

// FUNCTION: HOVER 0x00403850
int32_t __fastcall F403850(void* self, void* unused, int32_t arg)
{
    int32_t v = arg - 0x15e;
    if (v < 1)
        v = 1;
    v &= 0xffffffc0;
    {
        int32_t base = *(int32_t*)((uint8_t*)self + 0x60);
        if (v > 0xfc0)
            return base + 0x3f00;
        return base + v * 4;
    }
}

// FUNCTION: HOVER 0x00403880
void __fastcall CopyPos33880(void* self, void* unused, int32_t* out)
{
    struct
    {
        int32_t x;
        int32_t y;
    } p;
    p.x = *(int16_t*)((uint8_t*)self + 0x46);
    p.y = *(int16_t*)((uint8_t*)self + 0x4a);
    out[0] = p.x;
    out[1] = p.y;
}

typedef struct
{
    uint32_t x;
    uint32_t y;
} PtPair;

// FUNCTION: HOVER 0x004038a0
void __fastcall GetPair38a0(void* self, void* unused, PtPair* out)
{
    int32_t* p = (int32_t*)((uint8_t*)self + 0x44);
    out->x = p[0];
    out->y = p[1];
}

// FUNCTION: HOVER 0x00403910
void __fastcall DrawCell3910(void* self, void* unused, int32_t x, int32_t y, int32_t ignored)
{
    uint8_t* selfb = (uint8_t*)self;
    if (*(int32_t*)(selfb + 4) + 1 > y)
        return;
    if (*(int32_t*)selfb + 1 > x)
        return;
    if (*(int32_t*)(selfb + 0xc) - 1 < y)
        return;
    if (*(int32_t*)(selfb + 8) - 1 < x)
        return;
    {
        int32_t row = *(int32_t*)(selfb + 0x18) - y;
        uint8_t* buf = *(uint8_t**)(selfb + 0x10);
        int32_t stride = *(int32_t*)(selfb + 0x24);
        uint8_t v = *(uint8_t*)(selfb + 0x1c);
        buf[(row - 2) * stride + x] = v;
        buf[row * stride + x] = v;
        buf[(row - 1) * stride + x - 1] = v;
        buf[(row - 1) * stride + x] = v;
        buf[(row - 1) * stride + x + 1] = v;
    }
}

/* HUD widget bar gauge: draws the segmented on/off blocks of one dashboard
 * gauge into the dashboard DC (retail calls this once per widget from the
 * per-frame repaint at 0x413c70 and again from the DASH loader). */

// FUNCTION: HOVER 0x00401ad0
void __fastcall F401ad0(void* self, void* unused, void* dc, int32_t all)
{
    int32_t hi;
    int32_t limit;
    int32_t l;
    int32_t b;
    int32_t i;
    int32_t t;
    int32_t r;
    void** vt;

    hi = *(int32_t*)((uint8_t*)self + 0x24);
    limit = *(int32_t*)((uint8_t*)self + 0x28);
    limit = (limit <= hi) ? hi : limit;
    t = *(int32_t*)((uint8_t*)self + 0x04);
    l = *(int32_t*)((uint8_t*)self + 0x00);
    *(int32_t*)((uint8_t*)self + 0x28) = hi;
    r = *(int32_t*)((uint8_t*)self + 0x08);
    b = *(int32_t*)((uint8_t*)self + 0x0c);
    vt = *(void***)dc;
    ((CdcVSlot)vt[9])(dc);
    SelectPen450a09(dc, NULL, *(void**)((uint8_t*)self + 0x18));
    SelectBrush450a65(dc, NULL, *(void**)((uint8_t*)self + 0x10));

    for (i = 0; i < *(int32_t*)((uint8_t*)self + 0x1c); i++)
    {
        if (all == 0 && i > limit)
            break;
        if (*(int32_t*)((uint8_t*)self + 0x20) != 0)
            r = l + *(int32_t*)((uint8_t*)self + 0x2c);
        else
            t = b - *(int32_t*)((uint8_t*)self + 0x2c);
        SelectBrush450a65(dc, NULL,
            (*(int32_t*)((uint8_t*)self + 0x24) > i)
                ? *(void**)((uint8_t*)self + 0x10)
                : *(void**)((uint8_t*)self + 0x14));
        if (*(int32_t*)((uint8_t*)GetSub401140() + 0x544) != 0 &&
            (int32_t)((uint8_t*)GetSub401140() - (uint8_t*)dc) != -0xb4)
            Rectangle(*(void**)((uint8_t*)dc + 4), l * 2, t * 2, r * 2, b * 2);
        else
            Rectangle(*(void**)((uint8_t*)dc + 4), l, t, r, b);
        if (*(int32_t*)((uint8_t*)self + 0x20) != 0)
            l = r;
        else
            b = t;
    }
    ((CdcVSlotN)vt[10])(dc, NULL, -1);
}

static void __fastcall SelectPen450a09(void* self, void* unused, void* pen)
{
    void* prev;
    (void)unused;
    if (*(void**)((uint8_t*)self + 8) != *(void**)((uint8_t*)self + 4))
        prev = SelectObject(*(void**)((uint8_t*)self + 4),
                            (pen != NULL) ? *(void**)((uint8_t*)pen + 4) : NULL);
    if (*(void**)((uint8_t*)self + 8) != NULL)
        prev = SelectObject(*(void**)((uint8_t*)self + 8),
                            (pen != NULL) ? *(void**)((uint8_t*)pen + 4) : NULL);
    (void)prev;
}

static void __fastcall SelectBrush450a65(void* self, void* unused, void* brush)
{
    void* prev;
    (void)unused;
    if (*(void**)((uint8_t*)self + 8) != *(void**)((uint8_t*)self + 4))
        prev = SelectObject(*(void**)((uint8_t*)self + 4),
                            (brush != NULL) ? *(void**)((uint8_t*)brush + 4) : NULL);
    if (*(void**)((uint8_t*)self + 8) != NULL)
        prev = SelectObject(*(void**)((uint8_t*)self + 8),
                            (brush != NULL) ? *(void**)((uint8_t*)brush + 4) : NULL);
    (void)prev;
}