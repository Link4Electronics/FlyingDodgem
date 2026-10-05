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
