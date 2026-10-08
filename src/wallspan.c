#include "flyingdodgem_types.h"
#include <string.h>

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

typedef void (__cdecl *HostFn0)(void);
typedef void (__cdecl *HostFn1)(void);

struct HostObj
{
    HostFn0 fn0;
    HostFn1 fn1;
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
    int32_t six;
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

    (void)self;
    (void)unused;

    n = g_4a453c;
    host = (uint8_t*)g_4a1d68;
    a1 &= -n;

    if (*(int32_t*)(host + 0x74) == 1)
    {
        base = a1 * g_4a4a34 + *(int32_t*)(host + 0x28);
        g_4a4a4c = base + a0;
        if (n > 0)
        {
            w = *(uint8_t*)(*(uint8_t**)(host + 0x6c) + a1 * 4);
            span = g_4a4534 - a0;
            six = base + a0;
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
