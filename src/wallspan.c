#include "flyingdodgem_types.h"
#include <string.h>
#include "callees.h"

extern "C" MerlinPlane g_4a4498;
extern "C" MerlinPlane g_4a44e0;

extern "C" int32_t g_4a453c;
extern "C" void* g_4a1d68;
extern "C" int32_t g_4a4a34;
extern "C" int32_t g_4a4a4c;
extern "C" int32_t g_4a4534;
extern "C" int32_t g_4a4a50;
extern "C" int32_t g_4a454c;
extern "C" int32_t g_461cfc;
extern "C" int32_t g_4a4a20;
extern "C" int32_t g_4a1d70;
extern "C" int32_t g_4a4a1c;
extern "C" int32_t g_4a4554;
extern "C" int32_t g_4a455c;
extern "C" int32_t g_4a4a18;
extern "C" int32_t g_4a4a14;

extern "C" int32_t g_4a4540;
extern "C" int32_t g_4a4a38;
extern "C" int32_t g_4a4a3c;
extern "C" int32_t g_4a4a40;
extern "C" int32_t g_4a4a44;
#if !defined(_MSC_VER)
/* Native 64-bit: host+0x28 / host+0x6c only hold 4 bytes; the live
 * framebuffer and row-colour table live in g_nativePlanes (same side
 * table DrawEntry's GDI path uses). */
extern "C" struct NativePlanes
{
    void* host;
    void* planeB;
    void* ctx;
    uint8_t* bits;
    uint32_t* rowColours;
} g_nativePlanes;
#endif
typedef void (__cdecl *HostFn0)(void);
typedef void (__cdecl *HostFn1)(void);

struct HostObj
{
    HostFn0 fn0;
    HostFn1 fn1;
    int32_t F403850(int32_t arg);
};



int32_t* __fastcall DivFix(int32_t* p, void* unused, int32_t d)
{
    *p = (int32_t)(((int64_t)*p << 16) / d);
    return p;
}

// FUNCTION: HOVER 0x00406740
extern "C" void __fastcall F406740(void* self, void* unused, int32_t a0, int32_t a1)
{
    uint8_t* host;
    int32_t n;
    int32_t scale;
    int32_t w;
    int32_t span;
    int32_t base;
    int32_t top;
    int32_t width;
    int32_t field;
    int32_t reach;
    int32_t b;
    int32_t c;
    int32_t d;
    int32_t u;
    int32_t six16;
    int32_t count;
    int32_t saved;
    int32_t mask;
    uint8_t* tbl;
#if defined(_MSC_VER)
    int32_t six;
#else
    /* Native 64-bit: the host framebuffer pointer at +0x28 does not fit
     * in int32_t.  The MSVC matching build keeps the retail int32 shape. */
    uintptr_t six;
#endif

    (void)self;
    (void)unused;

    n = g_4a453c;
    host = (uint8_t*)g_4a1d68;
    a1 &= -n;

    if (*(int32_t*)(host + 0x74) == 1)
    {
#if defined(_MSC_VER)
        base = a1 * g_4a4a34 + *(int32_t*)(host + 0x28);
#else
        base = (int32_t)((uintptr_t)a1 * (uintptr_t)g_4a4a34 +
                         (uintptr_t)g_nativePlanes.bits);
#endif
        g_4a4a4c = base + a0;
        if (n > 0)
        {
#if defined(_MSC_VER)
            w = *(uint8_t*)(*(uint8_t**)(host + 0x6c) + a1 * 4);
#else
            w = (uint8_t)g_nativePlanes.rowColours[a1];
#endif
            span = g_4a4534 - a0;
#if defined(_MSC_VER)
            six = base + a0;
#else
            /* Reconstruct the full 64-bit framebuffer address so memset
             * writes the real surface (base/g_4a4a4c stay 32-bit for the
             * retail-shaped globals). */
            six = (uintptr_t)g_nativePlanes.bits
                + (uintptr_t)a1 * (uintptr_t)g_4a4a34
                + (uintptr_t)a0;
#endif
            do
            {
                memset((void*)six, w, span);
                six += g_4a4a34;
            } while (--n != 0);
            return;
        }
    }
    else
    {
        scale = 1 << g_461cfc;
        w = 640 / scale;
        g_4a4a50 = (int32_t)(((uint32_t)w << 16) / (uint32_t)g_4a454c);
        six = w * 6;
        tbl = *(uint8_t**)(host + 0x70);
        tbl = *(uint8_t**)(tbl + 0x28) + g_461cfc * 32;
        g_4a4a20 = *(int32_t*)(tbl + 4);
        top = 240 / scale;
        field = (int32_t)(((uint32_t)top << 16) / (uint32_t)g_4a1d70);
        reach = (field * (g_4a1d70 - a1)) >> 16;
        g_4a4a1c = (reach & *(int32_t*)tbl) * *(int32_t*)(tbl + 0xc) + *(int32_t*)(tbl + 0x18);
        span = g_4a4534 - a0;

        if (*(int32_t*)(host + 0x68) != 0)
        {
            b = a1 * g_4a4a34 + *(int32_t*)(host + 0x28) - g_4a4534;
            c = b + g_4a454c - 1;
            d = g_4a4534 - g_4a454c + 1;
            field = (*(int32_t*)(host + 0x54) + 0x55) & 0x1ff;
            g_4a4a4c = c;
            d *= g_4a4a50;
            u = (a0 - g_4a454c + 1) * g_4a4a50;
            field *= six;
            field <<= 7;
            u += field;
            b = d + field;
            if (b < 0)
                b += six << 16;
            six <<= 16;
            if (six > b)
            {
                g_4a4a18 = b;
                span >>= g_4a4554;
                g_4a4a50 <<= g_4a4554;
                g_4a4a14 = span;
                g_4a4a50 = -g_4a4a50;
                ((struct HostObj*)host)->fn1();
                return;
            }
            b -= six;
            count = b / g_4a4a50;
            span -= count;
            saved = g_4a4a50;
            span >>= g_4a4554;
            g_4a4a50 <<= g_4a4554;
            g_4a4a50 = -g_4a4a50;
            g_4a4a18 = b;
            g_4a4a14 = (count >> g_4a4554) + 1;
            ((struct HostObj*)host)->fn1();
            g_4a4a14 = span;
            mask = (g_4a455c - 1) & count;
            if (mask != 0)
            {
                u = count - mask;
                mask -= g_4a455c;
                mask = mask * saved + six;
                g_4a4a4c += u + g_4a455c;
                g_4a4a18 = mask;
            }
            else
            {
                g_4a4a18 = six;
                g_4a4a4c += count;
            }
            ((struct HostObj*)g_4a1d68)->fn1();
            return;
        }

        base = a1 * g_4a4a34 + *(int32_t*)(host + 0x28);
        g_4a4a4c = base + a0;
        field = *(int32_t*)(host + 0x54) & 0x1ff;
        d = g_4a4534 * g_4a4a50;
        field *= six;
        field <<= 7;
        u = a0 * g_4a4a50;
        b = u + field;
        c = field + d;
        six16 = six << 16;
        if (six16 < b)
        {
            b -= six16;
            c -= six16;
        }
        if (six16 > c)
        {
            g_4a4a18 = b;
            span >>= g_4a4554;
            g_4a4a50 <<= g_4a4554;
            g_4a4a14 = span;
            ((struct HostObj*)host)->fn1();
            return;
        }
        count = (six16 - b) / g_4a4a50;
        span -= count;
        saved = g_4a4a50;
        g_4a4a18 = b;
        span >>= g_4a4554;
        g_4a4a50 <<= g_4a4554;
        g_4a4a14 = (count >> g_4a4554) + 1;
        ((struct HostObj*)host)->fn1();
        g_4a4a14 = span;
        mask = (g_4a455c - 1) & count;
        if (mask != 0)
        {
            u = count - mask + g_4a455c;
            mask = (g_4a455c - mask) * saved;
            g_4a4a4c += u;
            g_4a4a18 = mask;
        }
        else
        {
            g_4a4a18 = 0;
            g_4a4a4c += count;
        }
        ((struct HostObj*)g_4a1d68)->fn1();
    }
}
// FUNCTION: HOVER 0x00405b90
int32_t HoverTc::M405b90(void* a1)
{
    uint8_t* node = (uint8_t*)a1;
    int32_t ax = *(int32_t*)(node + 0x14);
    int32_t ay = *(int32_t*)(node + 0x18);
    int32_t bx = *(int32_t*)(node + 0x1c);
    int32_t by = *(int32_t*)(node + 0x20);
    int32_t s1 = ((g_4a4498.m_p18 - ay) * g_4a4498.m_p38 + (ax - g_4a4498.m_p14) * g_4a4498.m_p3c) >= 0;
    int32_t s2 = ((ax - g_4a44e0.m_p14) * g_4a44e0.m_p3c + (g_4a44e0.m_p18 - ay) * g_4a44e0.m_p38) >= 0;
    int32_t s3;
    int32_t s4;
    int32_t s5;

    (void)this;
    if (!s1 && s2)
        return 1;
    s3 = ((g_4a4498.m_p18 - by) * g_4a4498.m_p38 + (bx - g_4a4498.m_p14) * g_4a4498.m_p3c) >= 0;
    s4 = ((bx - g_4a44e0.m_p14) * g_4a44e0.m_p3c + (g_4a44e0.m_p18 - by) * g_4a44e0.m_p38) >= 0;
    if (!s3 && s4)
        return 1;
    s5 = ((ay - g_4a4498.m_p18) * *(int32_t*)(node + 0x38) + (g_4a4498.m_p14 - ax) * *(int32_t*)(node + 0x3c)) >= 0;
    if (s5)
    {
        if (!s1 && !s2 && s3 && s4)
            return 1;
    }
    else
    {
        if (s1 && s2 && !s3 && !s4)
            return 1;
    }
    return 0;
}

// FUNCTION: HOVER 0x004066b0
extern "C" void __stdcall F4066b0(void* a1, void* a2, int32_t a3, int32_t a4)
{
    uint8_t* o = (uint8_t*)a1;
    uint8_t* v = (uint8_t*)a2;
    int32_t f98;
    int32_t shade;

    g_4a4a14 = g_4a453c - a3 + a4;
    if (g_4a4a14 < 1)
        return;
    g_4a4a4c = g_4a4540 + a3 * g_4a4a34;
    shade = *(int16_t*)(o + 0x8);
    g_4a4a44 = shade;
    g_4a4a38 = *(int32_t*)(o + 0x94);
    f98 = *(int32_t*)(o + 0x98);
    g_4a4a50 = f98;
    g_4a4a1c = *(int32_t*)(v + 0x10);
    g_4a4a40 = (a3 - *(int32_t*)(v + 0x8)) * f98;
    g_4a4a3c = ((struct HostObj*)g_4a1d68)->F403850(shade);
    ((struct HostObj*)g_4a1d68)->fn0();
}
