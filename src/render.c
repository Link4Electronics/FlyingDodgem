#include "flyingdodgem_types.h"
#include <stdlib.h>
#ifdef _MSC_VER
#include <math.h>
#else
extern "C" double sqrt(double);
extern "C" double fabs(double);
#endif
#include <string.h>
#include "callees.h"
#include "scene.h"

/* ---------------------------------------------------------------------------
 * Wall/floor/span renderer (retail .text around 0x401ad0-0x407430).  The
 * span setup path computes viewport scratch, light-table shade and texture
 * shift for one span, then hands off to F403850 (row offset table) and the
 * draw-host dispatch.
 * ------------------------------------------------------------------------- */

extern "C" int32_t g_4a453c;
extern "C" int32_t g_4a4540;
extern "C" void* g_4a4548;
extern "C" int32_t g_4a4a14;
extern "C" int32_t g_4a4a1c;
extern "C" int32_t g_4a4a34;
extern "C" int32_t g_4a4a38;
extern "C" int32_t g_4a4a3c;
extern "C" int32_t g_4a4a40;
extern "C" int32_t g_4a4a44;
extern "C" int32_t g_4a4a4c;
extern "C" int32_t g_4a4a50;
extern "C" void* g_4a1d68;
extern "C" int32_t g_4a1d78;
extern "C" int32_t g_461cfc;
extern "C" int32_t g_4a4534;
extern "C" int32_t g_4a454c;
extern "C" int32_t g_4a4554;
extern "C" int32_t g_4a455c;
extern "C" int32_t g_4a4a18;
extern "C" int32_t g_4a4a20;
extern "C" int32_t g_4a1d70;
extern "C" void* g_4c52dc;
extern "C" char g_4c55f4[];
extern "C" void* g_4a4714;
extern "C" void* g_4a4718;
extern "C" void* g_4a471c;

extern "C" int32_t g_4a4790;
extern "C" int32_t g_4a4794;
extern "C" int32_t g_4a4798;
extern "C" int32_t g_4a479c;
extern "C" int32_t g_4a47b0;
extern "C" int32_t g_4a47b8;
extern "C" int32_t g_4a47bc;
extern "C" int32_t g_4a47d8;
extern "C" int32_t g_4a47e0;
extern "C" int32_t g_4a47e4;
extern "C" void* g_4a4844;
extern "C" int32_t g_4a4848;
extern "C" int32_t g_460750;
extern "C" int32_t g_460740;
extern "C" void* g_460970;
extern "C" int32_t g_4a4524;
extern "C" int32_t g_4a4564;
extern "C" uint8_t g_460644[24];
extern "C" uint8_t g_46065c[24];
extern "C" int32_t g_4a452c;
extern "C" int32_t g_4a4538;
extern "C" int32_t g_4a4544;
extern "C" int32_t g_4a4550;
extern "C" int16_t g_4a4528;
extern "C" int32_t g_4a4530;
extern "C" int32_t g_4a4568;
extern "C" int32_t g_4a456c;
extern "C" int32_t g_4a1d64;
extern "C" int32_t g_4a1d6c;
extern "C" int32_t g_4a4a30;
extern "C" MerlinPlane g_4a4498;
extern "C" MerlinPlane g_4a44e0;
extern "C" int32_t __cdecl F406d30(void* a0, void* a1);

extern "C" void* __cdecl GetSub401140(void);

extern "C" void __fastcall F407160(void* self, void* unused, void* a0);

/* Retail packs adjacent 4-byte address fields (host +0x30/+0x34/+0x38/+0x64,
 * arena +0x04/+0x08/+0x18/+0x1c). An 8-byte void* access straddles the
 * neighbour on 64-bit; the 4-byte form below emits the identical mov on
 * 32-bit retail/matching and zero-extends the MAP_32BIT base on 64-bit
 * native. Matching sees the exact original tokens. */
#if defined(FLYINGDODGEM_GDI_COMPAT)
#define HOVER_LPTR(b, o) ((uint8_t*)(uintptr_t)*(uint32_t*)((uint8_t*)(b) + (o)))
#define HOVER_SPTR(b, o, v) (*(uint32_t*)((uint8_t*)(b) + (o)) = (uint32_t)(uintptr_t)(v))
#else
#define HOVER_LPTR(b, o) (*(uint8_t**)((uint8_t*)(b) + (o)))
#define HOVER_SPTR(b, o, v) (*(void**)((uint8_t*)(b) + (o)) = (void*)(v))
#endif

#if defined(FLYINGDODGEM_GDI_COMPAT)
/* Retail packs t+0xc8/+0xcc/+0xd0 as three 4-byte pointers.  On 64-bit each
 * void* store is 8 bytes and the slots overlap, so the native display driver
 * parks the live objects here.  bits holds the full framebuffer pointer that
 * does not fit in the host's 4-byte +0x28 field.  The matching MSVC build
 * never defines this macro. */
struct NativePlanes
{
    void* host;  /* *fc8: span host handed to M406090 */
    void* planeB;
    void* ctx;   /* render context (t+0xd0 on 32-bit) */
    uint8_t* bits;
    uint32_t* rowColours;
};
extern "C" struct NativePlanes g_nativePlanes;
struct NativePlanes g_nativePlanes;
#endif

#if defined(_MSC_VER)
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "kernel32.lib")
#define HOVER_IMPORT __declspec(dllimport)
#else
#define HOVER_IMPORT
#endif
#define HOVER_SRCCOPY 0x00cc0020u

extern "C" HOVER_IMPORT int32_t __stdcall Rectangle(void* hdc, int32_t left, int32_t top, int32_t right, int32_t bottom);

extern "C" HOVER_IMPORT void* __stdcall SelectObject(void* hdc, void* obj);

extern "C" HOVER_IMPORT int32_t __stdcall PatBlt(void* hdc, int32_t x, int32_t y, int32_t w, int32_t h, uint32_t rop);

extern "C" HOVER_IMPORT void __stdcall GdiFlush(void);

extern "C" HOVER_IMPORT void __stdcall OutputDebugStringA(char* psz);

extern "C" HOVER_IMPORT void __stdcall EnterCriticalSection(void* cs);

extern "C" HOVER_IMPORT void __stdcall LeaveCriticalSection(void* cs);

extern "C" HOVER_IMPORT uint32_t __stdcall GetTickCount(void);

extern "C" HOVER_IMPORT int32_t __stdcall InvalidateRect(void* hwnd, void* rect, int32_t erase);

extern "C" HOVER_IMPORT int32_t __stdcall BitBlt(void* hdcDest, int32_t xDest, int32_t yDest, int32_t w, int32_t h, void* hdcSrc, int32_t xSrc, int32_t ySrc, uint32_t rop);

extern "C" HOVER_IMPORT int32_t __stdcall StretchBlt(void* hdcDest, int32_t xDest, int32_t yDest, int32_t wDest, int32_t hDest, void* hdcSrc, int32_t xSrc, int32_t ySrc, int32_t wSrc, int32_t hSrc, uint32_t rop);

typedef void (__fastcall *CdcVSlot)(void* self);
typedef void (__fastcall *CdcVSlotN)(void* self, void* unused, int32_t n);

/* Layout view of the retail dashboard DC: only the members F401ad0 reaches are
 * named, padded to their retail vtable slots (ResetDC +0x24, Finish +0x28).
 * SelectPen/SelectBrush are non-virtual and thiscall, which is what gives the
 * retail `mov ecx,dc; push pen; call` shape with no edx setup. */
class DashDc {
public:
    virtual void reserved00();
    virtual void reserved01();
    virtual void reserved02();
    virtual void reserved03();
    virtual void reserved04();
    virtual void reserved05();
    virtual void reserved06();
    virtual void reserved07();
    virtual void reserved08();
    virtual void ResetDC();
    virtual void Finish(int32_t n);
    void* SelectPen(void* pen);
    void* SelectBrush(void* brush);
};

static void* __stdcall PrevObject4512ff(void* prev);

typedef void (__cdecl *HostFn0)(void);
typedef void (__cdecl *HostFn1)(void);

struct HostObj
{
    HostFn0 fn0;
    HostFn1 fn1;
    int32_t F403850(int32_t arg);
};

// FUNCTION: HOVER 0x00402990
extern "C" void __fastcall SpanSetup2990(void* self, void* unused, void* a1, void* a2, int32_t a3, int32_t a4)
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
    g_4a4a3c = ((struct HostObj*)g_4a1d68)->F403850(g_4a4a44);
    ((struct HostObj*)g_4a1d68)->fn0();
}

// FUNCTION: HOVER 0x00403850
int32_t HostObj::F403850(int32_t arg)
{
    int32_t v = arg - 0x15e;
    if (v < 1)
        v = 1;
    v &= 0xffffffc0;
    {
        int32_t base = *(int32_t*)((uint8_t*)this + 0x60);
        if (v > 0xfc0)
            return base + 0x3f00;
        return base + v * 4;
    }
}

/* Bump-arena span records (retail 0x403980/0x4065f0): the arena object is
 * reached in ECX without touching EDX, so both are __thiscall-shaped
 * members of a throw-away class (free __fastcall would burn EDX). */
struct SpanArena
{
    void F403980(int32_t a1, int32_t a2, int32_t a3, int32_t a4);
    int32_t F4065f0(int32_t a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5);
};

// FUNCTION: HOVER 0x00403980
void SpanArena::F403980(int32_t a1, int32_t a2, int32_t a3, int32_t a4)
{
    uint8_t* rec = *(uint8_t**)((uint8_t*)this + 0xc);
    int32_t key = (a1 << 18) + a2;

    if (rec != 0)
    {
        do
        {
            if (*(int32_t*)(rec + 0x20) == key)
                break;
            rec = *(uint8_t**)(rec + 0x28);
        } while (rec != 0);
    }
    if (rec == 0)
    {
        uint8_t* last = *(uint8_t**)((uint8_t*)this + 0x10);
        uint8_t* bump;
        if (last != 0)
        {
            bump = *(uint8_t**)((uint8_t*)this + 0x1c);
            *(uint8_t**)(last + 0x28) = bump;
        }
        else
        {
            bump = *(uint8_t**)((uint8_t*)this + 0x1c);
            *(uint8_t**)((uint8_t*)this + 0xc) = bump;
        }
        rec = *(uint8_t**)((uint8_t*)this + 0x1c);
        *(uint8_t**)((uint8_t*)this + 0x10) = rec;
        bump = rec + 0x2c;
        *(uint8_t**)((uint8_t*)this + 0x1c) = bump;
        *(uint8_t**)rec = bump;
        bump += 0xa0;
        {
            int32_t cap = *(int32_t*)this;
            uint8_t* end;
            *(uint8_t**)((uint8_t*)this + 0x1c) = bump;
            *(uint8_t**)(rec + 0x4) = bump;
            end = bump + cap * 2 + 0xa0;
            bump += 0xa0;
            *(uint8_t**)((uint8_t*)this + 0x1c) = bump;
            *(uint8_t**)(rec + 0x10) = bump;
            *(uint8_t**)((uint8_t*)this + 0x1c) = end;
            *(int32_t*)(rec + 0x28) = 0;
            *(int32_t*)(rec + 0x14) = 0;
            *(int32_t*)(rec + 0x18) = 0;
            *(uint8_t**)(rec + 0x8) = rec;
            *(int32_t*)(rec + 0x1c) = a1;
            *(uint8_t**)(rec + 0xc) = rec + 4;
            *(int32_t*)(rec + 0x24) = a2;
            {
                uint32_t lim = *(uint32_t*)((uint8_t*)this + 0x18);
                *(int32_t*)(rec + 0x20) = key;
                lim += 0x16b48;
                (*(int32_t*)((uint8_t*)this + 0x14))++;
                if ((uint8_t*)(intptr_t)lim < end)
                    OutputDebugStringA(g_4c55f4);
            }
        }
    }
    {
        int32_t n = *(int32_t*)(rec + 0x14);
        if (n < 0x28)
        {
            uint8_t* arr = *(uint8_t**)(rec + 0xc);
            n++;
            *(int32_t*)(rec + 0x14) = n;
            {
                uint8_t* base = *(uint8_t**)arr;
                int16_t* slot = (int16_t*)(base + n * 4 - 4);
                slot[0] = (int16_t)a3;
                slot[1] = (int16_t)a4;
            }
        }
    }
}

// FUNCTION: HOVER 0x004065f0
int32_t SpanArena::F4065f0(int32_t a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5)
{
    int32_t lim = g_4a4564;
    uint8_t* node = (uint8_t*)(intptr_t)a2;
    uint8_t* tp = *(uint8_t**)(node + 0x88);
    int32_t si = a5;
    int32_t t0 = *(int32_t*)tp;
    (void)a1;
    (void)a3;
    (void)a4;
    if (si < t0)
    {
        if (si < lim)
        {
            if (si <= g_4a4524)
            {
                ((SpanArena*)(*(uint8_t**)this))->F403980(a5, *(int32_t*)(node + 0x54), g_4a4524, lim);
                return 1;
            }
            else
            {
                ((SpanArena*)(*(uint8_t**)this))->F403980(a5, *(int32_t*)(node + 0x54), si, lim);
                si -= g_4a453c;
                g_4a4564 = si;
            }
        }
    }
    else if (lim > t0)
    {
        if (g_4a4524 >= t0)
        {
            ((SpanArena*)(*(uint8_t**)this))->F403980(a5, *(int32_t*)(node + 0x54), g_4a4524, lim);
            return 1;
        }
        else
        {
            ((SpanArena*)(*(uint8_t**)this))->F403980(a5, *(int32_t*)(node + 0x54), t0, lim);
            t0 -= g_4a453c;
            g_4a4564 = t0;
        }
    }
    return 0;
}

typedef struct
{
    int32_t x;
    int32_t y;
} IntPair;

typedef struct
{
    uint32_t x;
    uint32_t y;
} PtPair;

// Runtime-class side of the retail scene objects: only the entry the binary
// actually dispatches through (vtable slot 0) plus the base-class link at +0x10.
class RtHost
{
public:
    virtual void* GetRuntimeClass() = 0;
};

class RenderNode
{
public:
    int32_t IsKindOf(void* pClass);
    IntPair CopyPos();
    PtPair* GetPair(PtPair* out);
    void F404bb0(void* a0, void* a1, int32_t a2);
    void F4055b0(void* a0, int32_t a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7);
};

// FUNCTION: HOVER 0x00403880
IntPair RenderNode::CopyPos()
{
    IntPair p;
    p.x = *(int16_t*)((uint8_t*)this + 0x46);
    p.y = *(int16_t*)((uint8_t*)this + 0x4a);
    return p;
}

// FUNCTION: HOVER 0x004038a0
PtPair* RenderNode::GetPair(PtPair* out)
{
    int32_t* p = (int32_t*)((uint8_t*)this + 0x44);
    out->x = p[0];
    out->y = p[1];
    return out;
}

// FUNCTION: HOVER 0x0044e710
int32_t RenderNode::IsKindOf(void* pClass)
{
    void* rc = ((RtHost*)this)->GetRuntimeClass();

    while (rc != 0)
    {
        if (rc == pClass)
            return 1;
        rc = *(void**)((uint8_t*)rc + 0x10);
    }
    return 0;
}

// FUNCTION: HOVER 0x004038e0
extern "C" int32_t* __fastcall MulFix38e0(int32_t* self, void* unused, int32_t k)
{
    int32_t* p = self;
    int32_t** pp = &p;
    *p = (int32_t)(((int64_t)*p * k) >> 16);
    return *pp;
}

// FUNCTION: HOVER 0x00403910
extern "C" void __fastcall DrawCell3910(void* self, void* unused, int32_t x, int32_t y, int32_t ignored)
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
        row--;
        buf[(row + 1) * stride + x] = v;
        buf[row * stride + x - 1] = v;
        buf[row * stride + x] = v;
        buf[row * stride + x + 1] = v;
    }
}

/* HUD widget bar gauge: draws the segmented on/off blocks of one dashboard
 * gauge into the dashboard DC (retail calls this once per widget from the
 * per-frame repaint at 0x413c70 and again from the DASH loader). */

// FUNCTION: HOVER 0x00401ad0
extern "C" void __fastcall F401ad0(void* self, void* unused, void* dc, int32_t all)
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
    ((DashDc*)dc)->SelectPen(*(void**)((uint8_t*)self + 0x18));
    ((DashDc*)dc)->SelectBrush(*(void**)((uint8_t*)self + 0x10));

    for (i = 0; i < *(int32_t*)((uint8_t*)self + 0x1c); i++)
    {
        if (all == 0 && i > limit)
            break;
        if (*(int32_t*)((uint8_t*)self + 0x20) != 0)
            r = l + *(int32_t*)((uint8_t*)self + 0x2c);
        else
            t = b - *(int32_t*)((uint8_t*)self + 0x2c);
        ((DashDc*)dc)->SelectBrush(
            (*(int32_t*)((uint8_t*)self + 0x24) > i)
                ? *(void**)((uint8_t*)self + 0x10)
                : *(void**)((uint8_t*)self + 0x14));
        if (*(int32_t*)((uint8_t*)GetSub401140() + 0x544) != 0 &&
            (int32_t)((uint8_t*)GetSub401140() - (uint8_t*)dc) != -0xb4)
            Rectangle(*(void**)((uint8_t*)dc + 4), l * 2, t * 2, r * 2, b * 2);
        else
            Rectangle(*(void**)((uint8_t*)dc + 4), l, t, r, b);
        if (*(int32_t*)((uint8_t*)self + 0x20) == 0)
            b = t;
        else
            l = r;
    }
    ((CdcVSlotN)vt[10])(dc, vt, -1);
}

// FUNCTION: HOVER 0x00450a09
void* DashDc::SelectPen(void* pen)
{
    void* prev;
    if (*(void**)((uint8_t*)this + 8) != *(void**)((uint8_t*)this + 4))
        prev = SelectObject(*(void**)((uint8_t*)this + 4),
                            (pen != NULL) ? *(void**)((uint8_t*)pen + 4) : NULL);
    if (*(void**)((uint8_t*)this + 8) != NULL)
        prev = SelectObject(*(void**)((uint8_t*)this + 8),
                            (pen != NULL) ? *(void**)((uint8_t*)pen + 4) : NULL);
    return PrevObject4512ff(prev);
}

// FUNCTION: HOVER 0x00450a65
void* DashDc::SelectBrush(void* brush)
{
    void* prev;
    if (*(void**)((uint8_t*)this + 8) != *(void**)((uint8_t*)this + 4))
        prev = SelectObject(*(void**)((uint8_t*)this + 4),
                            (brush != NULL) ? *(void**)((uint8_t*)brush + 4) : NULL);
    if (*(void**)((uint8_t*)this + 8) != NULL)
        prev = SelectObject(*(void**)((uint8_t*)this + 8),
                            (brush != NULL) ? *(void**)((uint8_t*)brush + 4) : NULL);
    return PrevObject4512ff(prev);
}

extern "C" void* __cdecl Sub45128b(int flag);
extern "C" void* __fastcall Sub44f9a3(void* self, void* unused, void* prev);
// FUNCTION: HOVER 0x004512ff
static void* __stdcall PrevObject4512ff(void* prev)
{
    void* obj = Sub45128b(1);
    return Sub44f9a3(obj, NULL, prev);
}
/* ---------------------------------------------------------------------------
 * Scene renderer setup helpers (retail 0x406c00-0x406d30).  They run from the
 * per-frame 3D draw entry at 0x402250: plane parameter upload, position
 * copy-out and the object-list teardown before a fresh scene pass.
 * ------------------------------------------------------------------------- */

typedef void (__fastcall *VSlot0)(void* self);

// FUNCTION: HOVER 0x00406c00
extern "C" void __fastcall Set6c00(void* self, void* unused, int32_t a1, int32_t a2, int32_t a3, int32_t a4)
{
    uint8_t* b = (uint8_t*)self;
    int32_t* q = (int32_t*)(b + 0x48);
    *(int32_t*)(b + 0x50) = a3;
    *(int32_t*)(b + 0x54) = a4 & 0x1ff;
    q[0] = a1;
    q[1] = a2;
    if (*(int32_t*)(b + 0xa8) != a3)
        *(int32_t*)(b + 0xa8) = a3;
}

// FUNCTION: HOVER 0x00406c40
extern "C" void __fastcall CopyPos6c40(void* self, void* unused, int32_t* out)
{
    PtPair* p = (PtPair*)((uint8_t*)self + 0x54);
    PtPair* d = (PtPair*)out;
    d->x = p->x;
    d->y = p->y;
}

// FUNCTION: HOVER 0x00406c60
extern "C" void __cdecl Release6c60(void)
{
    void* p = g_4c52dc;
    while (p != NULL)
    {
        void* next = *(void**)((uint8_t*)p + 0x74);
        void** vt = *(void***)p;
        *(void**)((uint8_t*)p + 0x74) = NULL;
        *(int32_t*)((uint8_t*)p + 0x6c) = 0;
        ((VSlot0)vt[16])(p);
        p = next;
    }
    g_4c52dc = NULL;
}

/* Per-frame 3D draw entry (retail 0x402250).  Runs under the two critical
 * sections guarding the render node lists, uploads the plane parameters and
 * the cutout content pass, then dispatches the status-mode key handling. */

typedef struct
{
    int32_t left;
    int32_t top;
    int32_t right;
    int32_t bottom;
} HovRect;

// FUNCTION: HOVER 0x00402250
extern "C" void __fastcall DrawEntry402250(void* self, void* unused)
{
    uint8_t* t = (uint8_t*)self;
    uint8_t* world;
    void* node;
    void** slot;
    void** fc8;
    void* obj;
    int32_t halves;
    int32_t den;
    int32_t q;
    int32_t s;
    int32_t v;
    int32_t f3bc;
    int32_t* pp;
    HovRect rect;
    PtPair pair;
#if defined(FLYINGDODGEM_GDI_COMPAT)
    NativePlanes* planes = &g_nativePlanes;
#endif

#if defined(FLYINGDODGEM_GDI_COMPAT)
    if (planes->ctx == NULL)
        return;
    world = *(uint8_t**)(t + 0x30);
    if (world == NULL)
        return;
#else
    if (*(int32_t*)(t + 0xd0) == 0)
        return;

    world = *(uint8_t**)(t + 0x30);
#endif
    EnterCriticalSection(g_460644);
    Release6c60();

    *(int32_t*)(t + 0xdc) = *(int32_t*)(world + 0x8214);
    pp = (int32_t*)(t + 0xd4);
    CopyPos6c40(world + 0x818c, self, (int32_t*)&pair);
    pp[0] = pair.x;
    pp[1] = pair.y;
    *(int32_t*)(t + 0xe0) = *(int32_t*)(world + 0x8218) + 0x28;
    F407160(t + 0x150, t + 0x150, world);

    halves = 0;
    *(int16_t*)((uint8_t*)&halves + 2) =
        (int16_t)*(int32_t*)(world + 0x8330) - *(int16_t*)(world + 0x83c4);
    den = (int32_t)*(int16_t*)(world + 0x83cc);
    q = halves / den;

    s = q + 0x288;
    f3bc = *(int32_t*)(t + 0x3bc);
    s = (int32_t)(int16_t)f3bc * s;
    v = s >> 16;
    if (f3bc > v)
    {
        if (v > 0)
            *(int32_t*)(t + 0x3c4) = v;
        else
            *(int32_t*)(t + 0x3c4) = 0;
    }
    else
    {
        *(int32_t*)(t + 0x3c4) = f3bc;
    }

    LeaveCriticalSection(g_460644);
    EnterCriticalSection(g_46065c);

#if defined(FLYINGDODGEM_GDI_COMPAT)
    slot = &planes->planeB;
    fc8 = &planes->host;
    obj = *slot;
    if (*(int32_t*)(t + 0x21c) != 0)
    {
        if (obj != NULL)
            *(void**)((uint8_t*)obj + 0x60) = world + 0x418c;
        node = world + 0x418c;
    }
    else
    {
        if (obj != NULL)
            *(void**)((uint8_t*)obj + 0x60) = world + 0x18c;
        node = world + 0x18c;
    }

    /* 4-byte store: an 8-byte void* store here would clobber host+0x64
     * (dir-table base) on 64-bit, faulting M406090's entry calc. The value
     * (view node for a future textured F402b10) is unread by the current
     * real+stub set. Matching build is 32-bit (no overlap) and untouched
     * below in the #else path. */
    *(int32_t*)((uint8_t*)*fc8 + 0x60) = (int32_t)(uintptr_t)node;

    if (*(int32_t*)(t + 0x540) == 0)
    {
        Set6c00(*fc8, self, pp[0], pp[1], *(int32_t*)(t + 0xe0), *(int32_t*)(t + 0xdc));
        ((HoverTc*)planes->ctx)->M406090(*(void**)(*(uint8_t**)(t + 0x30) + 0x188), *fc8);
        if (g_460740 != 0 && *slot != NULL)
        {
            Set6c00(*slot, self, pp[0], pp[1], *(int32_t*)(t + 0xe0), (*(int32_t*)(t + 0xdc) + 0x100) & 0x1ff);
            ((HoverTc*)planes->ctx)->M406090(*(void**)(*(uint8_t**)(t + 0x30) + 0x188), *slot);
        }
        ((HoverTc*)(t + 0xe8))->M405a10(self, world);
    }
#else
    if (*(int32_t*)(t + 0x21c) != 0)
    {
        slot = (void**)(t + 0xcc);
        fc8 = (void**)(t + 0xc8);
        obj = *slot;
        if (obj != NULL)
            *(void**)((uint8_t*)obj + 0x60) = world + 0x418c;
        node = world + 0x418c;
    }
    else
    {
        slot = (void**)(t + 0xcc);
        fc8 = (void**)(t + 0xc8);
        obj = *slot;
        if (obj != NULL)
            *(void**)((uint8_t*)obj + 0x60) = world + 0x18c;
        node = world + 0x18c;
    }

    *(void**)((uint8_t*)*fc8 + 0x60) = node;

    if (*(int32_t*)(t + 0x540) == 0)
    {
        Set6c00(*fc8, self, pp[0], pp[1], *(int32_t*)(t + 0xe0), *(int32_t*)(t + 0xdc));
        ((HoverTc*)*(void**)(t + 0xd0))->M406090(*(void**)(*(uint8_t**)(t + 0x30) + 0x188), *fc8);
        if (g_460740 != 0 && *slot != NULL)
        {
            Set6c00(*slot, self, pp[0], pp[1], *(int32_t*)(t + 0xe0), (*(int32_t*)(t + 0xdc) + 0x100) & 0x1ff);
            ((HoverTc*)*(void**)(t + 0xd0))->M406090(*(void**)(*(uint8_t**)(t + 0x30) + 0x188), *slot);
        }
        ((HoverTc*)(t + 0xe8))->M405a10(self, world);
    }
#endif

    rect = *(HovRect*)(t + 0x1ac);
    PatBlt(*(void**)(t + 0xb8), rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, 0x42);
    GdiFlush();

    ((HoverTc*)(t + 0x150))->M4053a0(self, world);

    switch (*(int32_t*)(t + 0x500))
    {
    case 1:
        Ext40d340(self);
        break;
    case 2:
    {
        CBeaconKey key1;
        CBeaconKey key2;

        key1.M44bdef(0xef3f);
        key2.M44bdef(0xef53);
        ((HoverTc*)self)->M40cce0(key1.m_psz, key2.m_psz, NULL, NULL);
        break;
    }
    case 3:
        Ext4283f0(self);
        break;
    }

    if (*(int32_t*)(t + 0x80) != 0 && *(int32_t*)(t + 0x84) != 0)
    {
        InvalidateRect(*(void**)(t + 0x1c), NULL, 1);
        *(int32_t*)(t + 0x80) = 0;
    }
    else
    {
        Ext407430(self);
    }

    LeaveCriticalSection(g_46065c);
    *(int32_t*)(t + 0x52c) = GetTickCount();
    ++*(int32_t*)(t + 0x530);
}

// FUNCTION: HOVER 0x00406c90
extern "C" void __fastcall Setup6c90(void* self)
{
    /* 32-bit address semantics: the arena packs 4-byte fields every 4 bytes
     * (+0x04/+0x08/+0x0c/+0x10/+0x14/+0x18/+0x1c). An 8-byte void* access
     * would straddle neighbours on 64-bit (e.g. +0x18 scratch ptr high half
     * vs +0x1c store). The uint32_t+(uintptr_t) pattern emits the identical
     * 4-byte mov on 32-bit retail/matching and zero-extends the MAP_32BIT
     * base on 64-bit native. */
    uint8_t* b = (uint8_t*)self;
    uint8_t* p = (uint8_t*)(uintptr_t)*(uint32_t*)(b + 0x18);

    *(uint32_t*)(b + 0x1c) = (uint32_t)(uintptr_t)p;
    *(uint32_t*)(b + 4) = (uint32_t)(uintptr_t)p;
    *(int32_t*)(b + 0x10) = 0;
    uint8_t* t = p + 0xcc;
    p = p + 0xa0;
    *(uint32_t*)(b + 0x1c) = (uint32_t)(uintptr_t)p;
    *(int32_t*)(b + 0xc) = 0;
    uint8_t* r = p + 4;
    *(int32_t*)(b + 0x14) = 0;
    *(uint32_t*)(b + 8) = (uint32_t)(uintptr_t)p;
    *(uint32_t*)(b + 0x1c) = (uint32_t)(uintptr_t)t;
    *(uint32_t*)p = (uint32_t)(uintptr_t)t;
    t = t + 0xa0;
    *(uint32_t*)(b + 0x1c) = (uint32_t)(uintptr_t)t;
    *(uint32_t*)r = (uint32_t)(uintptr_t)t;
    *(int32_t*)(p + 0x28) = 0;
    t = t + 0xa0;
    *(int32_t*)(p + 0x14) = 0;
    *(uint32_t*)(b + 0x1c) = (uint32_t)(uintptr_t)t;
    *(uint32_t*)(p + 0x10) = (uint32_t)(uintptr_t)t;
    int32_t size = *(int32_t*)b;
    t = t + size * 2;
    *(int32_t*)(p + 0x18) = 0;
    *(uint32_t*)(b + 0x1c) = (uint32_t)(uintptr_t)t;
    *(uint32_t*)(p + 8) = (uint32_t)(uintptr_t)p;
    *(uint32_t*)(p + 0xc) = (uint32_t)(uintptr_t)r;
}

// FUNCTION: HOVER 0x00406d00
void SpanChain::AddPair6d00(int32_t a1, int32_t a2)
{
    uint8_t* q = (uint8_t*)(uintptr_t)*(uint32_t*)((uint8_t*)this + 8);
    int32_t n;
#if defined(FLYINGDODGEM_GDI_COMPAT)
    /* Empty-span harness: F403a60's list swap can NULL the bucket head;
     * retail always has live spans here. Safe no-op, matching untouched. */
    if (q == NULL)
        return;
#endif
    n = *(int32_t*)(q + 0x14);
    if (n < 0x28)
    {
        uint32_t* base = (uint32_t*)(uintptr_t)*(uint32_t*)(q + 0xc);
        uint8_t* arr = (uint8_t*)(uintptr_t)*base;
        int16_t* slot;

        n = n + 1;
        *(int32_t*)(q + 0x14) = n;
        slot = (int16_t*)(arr + n * 4 - 4);
        slot[1] = (int16_t)a2;
        slot[0] = (int16_t)a1;
    }
}



/* Segment-intersection projector (retail 0x401260): intersects the line
 * through self (pos + t*dir at +0x14/+0x18, dir at +0x38/+0x3c) with the
 * line through a1, writes the projected point into buf and classifies
 * whether the parameter range [0..den] contains both cross terms:
 * returns 1/-1 when the segments meet, 0 when the projection falls
 * outside either segment.  The division keeps the cross product in 64
 * bits and rounds with +/-den/2 before the quotient. */
// FUNCTION: HOVER 0x00401260
extern "C" int16_t __fastcall F401260(void* self, void* unused, void* a1, void* buf)
{
    uint8_t* s = (uint8_t*)self;
    uint8_t* o = (uint8_t*)a1;
    int32_t dx;
    int32_t dy;
    int32_t cross1;
    int32_t cross2;
    int32_t den;
    int32_t half;
    int64_t prod;
    int64_t t;

    (void)unused;
    dx = *(int32_t*)(s + 0x14) - *(int32_t*)(o + 0x14);
    dy = *(int32_t*)(s + 0x18) - *(int32_t*)(o + 0x18);
    cross1 = dy * *(int32_t*)(o + 0x38) - dx * *(int32_t*)(o + 0x3c);
    cross2 = dy * *(int32_t*)(s + 0x38) - dx * *(int32_t*)(s + 0x3c);
    den = *(int32_t*)(s + 0x38) * *(int32_t*)(o + 0x3c) - *(int32_t*)(o + 0x38) * *(int32_t*)(s + 0x3c);
    half = den / 2;

    if (den > 0)
    {
        prod = (int64_t)cross1 * *(int32_t*)(s + 0x38);
        t = (prod >= 0) ? prod + half : prod - half;
        *(int32_t*)buf = (int32_t)(t / den) + *(int32_t*)(s + 0x14);
        prod = (int64_t)cross1 * *(int32_t*)(s + 0x3c);
        t = (prod >= 0) ? prod + half : prod - half;
        *(int32_t*)((uint8_t*)buf + 4) = (int32_t)(t / den) + *(int32_t*)(s + 0x18);
        if (cross1 < 0)
            return 0;
        if (den < cross1)
            return 0;
        if (cross2 < 0)
            return 0;
        if (den >= cross2)
            return (cross2 < 0) ? -1 : 1;
        return 0;
    }
    if (den == 0)
        return 0;
    prod = (int64_t)cross1 * *(int32_t*)(s + 0x38);
    t = (prod >= 0) ? prod - half : prod + half;
    *(int32_t*)buf = (int32_t)(t / den) + *(int32_t*)(s + 0x14);
    prod = (int64_t)cross1 * *(int32_t*)(s + 0x3c);
    t = (prod >= 0) ? prod - half : prod + half;
    *(int32_t*)((uint8_t*)buf + 4) = (int32_t)(t / den) + *(int32_t*)(s + 0x18);
    if (cross1 > 0)
        return 0;
    if (den > cross1)
        return 0;
    if (cross2 > 0)
        return 0;
    if (den > cross2)
        return 0;
    return (cross2 < 0) ? -1 : 1;
}

// FUNCTION: HOVER 0x004044a0
extern "C" void __cdecl F4044a0(int32_t a1, int32_t a2, int32_t a3, int32_t a4)
{
    uint8_t* node;
    uint8_t* cur;
    uint8_t* next;
    uint8_t* prev;

    node = (uint8_t*)g_4a4718;
    *(int16_t*)(node + 0x1c) = (int16_t)a3;
    *(int32_t*)(node + 0x0c) = a1;
    *(int32_t*)(node + 0x24) = a4;
    *(int32_t*)(node + 0x18) = 1;
    *(int32_t*)(node + 0x10) = a2;
    cur = (uint8_t*)g_4a4714;

    if (a1 == 0)
    {
        if (cur == NULL)
        {
            *(void**)(node + 0x2c) = NULL;
            *(void**)(node + 0x28) = NULL;
            g_4a4714 = node;
        }
        else
        {
            while (*(int16_t*)(cur + 0x1c) >= (int16_t)a3)
            {
                if (*(int32_t*)(cur + 0x0c) != 0)
                    break;
                next = *(uint8_t**)(cur + 0x2c);
                if (next == NULL)
                {
                    *(void**)(cur + 0x2c) = node;
                    *(void**)(node + 0x28) = cur;
                    cur = NULL;
                    *(void**)(node + 0x2c) = cur;
                    break;
                }
                cur = next;
            }
            if (cur != NULL)
            {
                prev = *(uint8_t**)(cur + 0x28);
                *(void**)(cur + 0x28) = node;
                if (prev != NULL)
                {
                    *(void**)(prev + 0x2c) = node;
                    *(void**)(node + 0x28) = prev;
                    *(void**)(node + 0x2c) = cur;
                }
                else
                {
                    g_4a4714 = node;
                    *(void**)(node + 0x2c) = cur;
                    *(void**)(node + 0x28) = prev;
                }
            }
        }
    }
    else
    {
        while (cur != NULL && *(int32_t*)(cur + 0x0c) == 0 && *(void**)(cur + 0x2c) != NULL)
            cur = *(uint8_t**)(cur + 0x2c);
        if (cur != NULL)
        {
            next = *(uint8_t**)(cur + 0x2c);
            *(void**)(cur + 0x2c) = node;
            if (next != NULL)
                *(void**)(next + 0x28) = node;
            *(void**)(node + 0x2c) = next;
            *(void**)(node + 0x28) = cur;
        }
        else
        {
            *(void**)(node + 0x2c) = NULL;
            *(void**)(node + 0x28) = NULL;
            g_4a4714 = node;
        }
    }
    g_4a4718 = node + 0xc0;
}

// FUNCTION: HOVER 0x00404580
extern "C" int32_t __cdecl F404580(void* a0, void* a1)
{
    int32_t result = 0;
    uint8_t* node;
    uint8_t* head = (uint8_t*)g_4a4714;
    uint8_t* prev;
    uint8_t* next;

    node = head;

    while (node != NULL)
    {
        if (*(void**)(node + 0x10) == a0 && *(void**)(node + 0x24) == a1)
            break;
        node = *(uint8_t**)(node + 0x2c);
    }
    if (node != NULL)
    {
        if (*(uint32_t*)(node + 0x18) == 0)
            result = 1;
        prev = *(uint8_t**)(node + 0x28);
        next = *(uint8_t**)(node + 0x2c);
        if (prev != NULL)
            *(void**)(prev + 0x2c) = next;
        if (next != NULL)
            *(void**)(next + 0x28) = prev;
        if (head == node)
            g_4a4714 = next;
    }
    return result;
}

// FUNCTION: HOVER 0x004045e0
extern "C" int32_t __cdecl F4045e0(void* a0, void* a1)
{
    int16_t lo;
    int16_t hi;
    uint8_t* child;
    uint8_t* child2;
    uint8_t* prev;
    uint32_t flag1;
    uint32_t flag2;
    uint8_t* node;
    PtPair out;

    lo = hi = (int16_t)(uintptr_t)a0;
    prev = NULL;
    node = (uint8_t*)g_4a4714;
    g_4a471c = NULL;

    while (node != NULL && *(int32_t*)(node + 0xc) == 0)
    {
        child = *(uint8_t**)(node + 0x10);
        *(int32_t*)(node + 0x20) = 0;
        if (*(int16_t*)(child + 0xa6) <= hi && *(int16_t*)(child + 0xa4) >= lo)
        {
            *(int32_t*)(node + 0x18) = 1;
            *(int32_t*)(node + 0x20) = 1;
        }
        else
        {
            int16_t r = F401260(child, child, a1, &out);

            *(int32_t*)(node + 0x14) = ((uint16_t)(r - 1) == 0);
            if (g_4a471c == NULL)
                g_4a471c = node;
            if ((uint16_t)r != 0)
            {
                int32_t ay = out.y;
                int32_t ax = out.x;
                int32_t by = *(int32_t*)((uint8_t*)a1 + 0x18);
                int32_t bx = *(int32_t*)((uint8_t*)a1 + 0x14);
                int32_t dx = abs(ax - bx);
                int32_t dy = abs(ay - by);
                uint32_t m = (dy < dx) ? dy : dx;
                uint32_t den = dy - m / 2 + dx + 1;
                uint32_t num = (uint32_t)(dx * dx + dy * dy);
                uint32_t dist1 = (den + num / den) / 2;
                int16_t addend;

                *(int16_t*)(node + 8) = (int16_t)dist1;
                if (*(int32_t*)(node + 0x14) != 0)
                {
                    int32_t ay2 = out.y;
                    int32_t ax2 = out.x;
                    int32_t by2 = *(int32_t*)((uint8_t*)child + 0x20);
                    int32_t bx2 = *(int32_t*)((uint8_t*)child + 0x1c);
                    int32_t dx2 = abs(ax2 - bx2);
                    int32_t dy2 = abs(ay2 - by2);
                    uint32_t m2 = (dy2 < dx2) ? dy2 : dx2;
                    uint32_t den2 = dy2 - m2 / 2 + dx2 + 1;
                    uint32_t num2 = (uint32_t)(dx2 * dx2 + dy2 * dy2);
                    uint32_t dist2 = (den2 + num2 / den2) / 2;

                    addend = *(int16_t*)((uint8_t*)child + 0xb8);
                    *(int16_t*)(node + 0xa) = (int16_t)dist2;
                }
                else
                {
                    int32_t ay3 = out.y;
                    int32_t ax3 = out.x;
                    int32_t by3 = *(int32_t*)((uint8_t*)child + 0x18);
                    int32_t bx3 = *(int32_t*)((uint8_t*)child + 0x14);
                    int32_t dx3 = abs(ax3 - bx3);
                    int32_t dy3 = abs(ay3 - by3);
                    uint32_t m3 = (dy3 < dx3) ? dy3 : dx3;
                    uint32_t den3 = dy3 - m3 / 2 + dx3 + 1;
                    uint32_t num3 = (uint32_t)(dx3 * dx3 + dy3 * dy3);
                    uint32_t dist3 = (den3 + num3 / den3) / 2;

                    addend = *(int16_t*)((uint8_t*)child + 0xba);
                    *(int16_t*)(node + 0xa) = (int16_t)dist3;
                }
                *(int16_t*)(node + 0xa) += addend;
                if ((*(int32_t*)(node + 0x14) ? *(uint32_t*)((uint8_t*)child + 0xb0) : *(uint32_t*)((uint8_t*)child + 0xb4)) < 1)
                {
                    if (hi >= *(int16_t*)((uint8_t*)child + 0xa4) && hi < *(int16_t*)((uint8_t*)child + 0xa6))
                        hi = *(int16_t*)((uint8_t*)child + 0xa6);
                    if (*(int16_t*)((uint8_t*)child + 0xa6) >= lo && lo > *(int16_t*)((uint8_t*)child + 0xa4))
                        lo = *(int16_t*)((uint8_t*)child + 0xa4);
                }
                *(int32_t*)(node + 0x18) = 0;
            }
            else
            {
                *(int32_t*)(node + 0x18) = 1;
            }
            if (prev != NULL)
                *(void**)(prev + 0x34) = node;
            *(void**)(node + 0x34) = NULL;
            *(void**)(node + 0x30) = prev;
            prev = node;
        }
        node = *(uint8_t**)(node + 0x2c);
    }

    while (node != NULL)
    {
        uint8_t* cur;
        int16_t r;

        child2 = *(uint8_t**)(node + 0x10);
        r = F401260(child2, a1, a1, &out);
        *(int32_t*)(node + 0x20) = 0;
        flag1 = ((uint16_t)(r - 1) == 0);
        *(int32_t*)(node + 0x14) = flag1;
        flag2 = flag1;
        if ((uint16_t)r != 0)
        {
            int32_t ay4 = out.y;
            int32_t ax4 = out.x;
            int32_t by4 = *(int32_t*)((uint8_t*)a1 + 0x18);
            int32_t bx4 = *(int32_t*)((uint8_t*)a1 + 0x14);
            int32_t dx4 = abs(ax4 - bx4);
            int32_t dy4 = abs(ay4 - by4);
            uint32_t m4 = (dy4 < dx4) ? dy4 : dx4;
            uint32_t den4 = dy4 - m4 / 2 + dx4 + 1;
            uint32_t num4 = (uint32_t)(dx4 * dx4 + dy4 * dy4);
            uint32_t dist4 = (den4 + num4 / den4) / 2;
            uint32_t dist;

            *(int16_t*)(node + 8) = (int16_t)dist4;
            if (*(int32_t*)((uint8_t*)child2 + 0x68) != 0)
                flag2 = !flag1;
            if (flag2 != 0)
            {
                int32_t ay5 = out.y;
                int32_t ax5 = out.x;
                int32_t by5 = *(int32_t*)((uint8_t*)child2 + 0x20);
                int32_t bx5 = *(int32_t*)((uint8_t*)child2 + 0x1c);
                int32_t dx5 = abs(ax5 - bx5);
                int32_t dy5 = abs(ay5 - by5);
                uint32_t m5 = (dy5 < dx5) ? dy5 : dx5;
                uint32_t den5 = dy5 - m5 / 2 + dx5 + 1;
                uint32_t num5 = (uint32_t)(dx5 * dx5 + dy5 * dy5);

                dist = (den5 + num5 / den5) / 2;
            }
            else
            {
                int32_t ay6 = out.y;
                int32_t ax6 = out.x;
                int32_t by6 = *(int32_t*)((uint8_t*)child2 + 0x18);
                int32_t bx6 = *(int32_t*)((uint8_t*)child2 + 0x14);
                int32_t dx6 = abs(ax6 - bx6);
                int32_t dy6 = abs(ay6 - by6);
                uint32_t m6 = (dy6 < dx6) ? dy6 : dx6;
                uint32_t den6 = dy6 - m6 / 2 + dx6 + 1;
                uint32_t num6 = (uint32_t)(dx6 * dx6 + dy6 * dy6);

                dist = (den6 + num6 / den6) / 2;
            }
            *(int16_t*)(node + 0xa) = (int16_t)dist;
            *(int32_t*)(node + 0x18) = 0;
        }
        else
        {
            *(int32_t*)(node + 0x18) = 1;
        }

        cur = (uint8_t*)g_4a471c;
        if (cur == NULL)
        {
            *(void**)(node + 0x34) = NULL;
            *(void**)(node + 0x30) = NULL;
            g_4a471c = node;
        }
        else
        {
            for (;;)
            {
                if ((*(int32_t*)(cur + 0x18) == 0 && *(int16_t*)(cur + 8) > *(int16_t*)(node + 8))
                    || *(int32_t*)(node + 0x18) != 0)
                {
                    uint8_t* old = *(uint8_t**)(cur + 0x30);

                    *(void**)(cur + 0x30) = node;
                    if (old != NULL)
                        *(void**)(old + 0x34) = node;
                    else
                        g_4a471c = node;
                    *(void**)(node + 0x34) = cur;
                    *(void**)(node + 0x30) = old;
                    break;
                }
                if (*(void**)(cur + 0x34) == NULL)
                {
                    *(void**)(cur + 0x34) = node;
                    *(void**)(node + 0x30) = cur;
                    *(void**)(node + 0x34) = NULL;
                    break;
                }
                cur = *(uint8_t**)(cur + 0x34);
            }
        }
        node = *(uint8_t**)(node + 0x2c);
    }

    return 1;
}

extern "C" uint8_t g_data_5570[1];
extern "C" uint8_t g_data_5340[1];
extern "C" uint8_t g_data_5318[1];
extern "C" uint8_t g_data_5418[1];
extern "C" uint8_t g_data_53f8[1];
extern "C" uint8_t g_sineTable[32768];
// FUNCTION: HOVER 0x00404bb0
void RenderNode::F404bb0(void* a0, void* a1, int32_t a2)
{
    int32_t m10a = -10;
    int32_t m10b = -10;
    void* item = a1;
    void* parent;
    int32_t left;
    int32_t top;
    IntPair cp;
    PtPair gp;
    PtPair* qpp;
    int16_t w10;
    int16_t w12;
    int16_t w14;
    int16_t w16;
    int32_t v40;
    int32_t v4c;
    int32_t v48;
    int32_t v30;
    int16_t w44;
    int16_t w46;
    int32_t v18;
    int32_t d14;
    int32_t ang;
    int32_t s3c;
    int32_t v68;
    int32_t s38;
    int32_t v60;
    int32_t ang2;
    int32_t v9c;
    int32_t s34;
    int32_t v58;
    int32_t v90;
    int32_t s2c;
    int32_t v50;
    int32_t v94;
    int32_t cvx;
    int32_t dxy;
    int32_t h9a;
    int32_t h92;
    int32_t h9e;
    int32_t h96;
    int32_t v20;
    int32_t v28;
    int32_t v1c;
    int32_t v24;
    uint8_t* rp;
    uint8_t* dp;
    uint8_t* bp;
    uint8_t* ep;

    if (((RenderNode*)item)->IsKindOf(g_data_5570))
    {
        if (*(int32_t*)((uint8_t*)item + 0x38) != 0)
            return;
        if (a2 != 0 || ((parent = *(void**)((uint8_t*)item + 0x78)) != 0 && (*(uint8_t*)((uint8_t*)parent + 0x24) & 8) != 0))
        {
            top = *(int32_t*)((uint8_t*)item + 8);
            left = *(int32_t*)((uint8_t*)item + 4);
            if (left < g_4a47bc && *(int32_t*)((uint8_t*)item + 0xc) < g_4a47bc)
                return;
            if (left > g_4a4798 && *(int32_t*)((uint8_t*)item + 0xc) > g_4a4798)
                return;
            if (top < g_4a47d8 && *(int32_t*)((uint8_t*)item + 0x10) < g_4a47d8)
                return;
            if (top > g_4a479c && *(int32_t*)((uint8_t*)item + 0x10) > g_4a479c)
                return;
            ((RenderNode*)this)->F4055b0((uint8_t*)item + 0x84, left, top, g_4a4790, g_4a4794, g_4a47e0, g_4a47e4, g_4a47b8);
            ((RenderNode*)this)->F4055b0((uint8_t*)item + 0x8c, *(int32_t*)((uint8_t*)item + 0xc), *(int32_t*)((uint8_t*)item + 0x10), g_4a4790, g_4a4794, g_4a47e0, g_4a47e4, g_4a47b8);
            *(int32_t*)((uint8_t*)item + 0x3c) = 1;
        }
        else
        {
            *(int32_t*)((uint8_t*)item + 0x8c) = m10a;
            *(int32_t*)((uint8_t*)item + 0x90) = m10b;
            *(int32_t*)((uint8_t*)item + 0x84) = m10a;
            *(int32_t*)((uint8_t*)item + 0x88) = m10b;
        }
    }
    else if (((RenderNode*)item)->IsKindOf(g_data_5340) && *(int32_t*)((uint8_t*)item + 0x28) != 0)
    {
        rp = *(uint8_t**)((uint8_t*)item + 0xd4);
        left = *(int32_t*)(rp + 0x14);
        top = *(int32_t*)(rp + 0x18);
        if (left < g_4a47bc && *(int32_t*)(rp + 0x1c) < g_4a47bc)
            return;
        if (left > g_4a4798 && *(int32_t*)(rp + 0x1c) > g_4a4798)
            return;
        if (top < g_4a47d8 && *(int32_t*)(rp + 0x20) < g_4a47d8)
            return;
        if (top > g_4a479c && *(int32_t*)(rp + 0x20) > g_4a479c)
            return;
        ((RenderNode*)this)->F4055b0((uint8_t*)item + 0xf0, left, top, g_4a4790, g_4a4794, g_4a47e0, g_4a47e4, g_4a47b8);
        ((RenderNode*)this)->F4055b0((uint8_t*)item + 0xf8, *(int32_t*)(rp + 0x1c), *(int32_t*)(rp + 0x20), g_4a4790, g_4a4794, g_4a47e0, g_4a47e4, g_4a47b8);
        *(int32_t*)((uint8_t*)item + 0x3c) = 1;
    }
    else if (((RenderNode*)item)->IsKindOf(g_data_5318))
    {
        if (*(int32_t*)((uint8_t*)item + 0x88) != 0)
            return;
        if ((a2 == 0 && ((parent = *(void**)((uint8_t*)item + 0x78)) == 0 || (*(uint8_t*)((uint8_t*)parent + 0x24) & 8) == 0)) || *(int32_t*)((uint8_t*)item + 0x38) != 0)
        {
            *(int32_t*)((uint8_t*)item + 0xc0) = m10a;
            *(int32_t*)((uint8_t*)item + 0xc4) = m10b;
            return;
        }
        cp = ((RenderNode*)item)->CopyPos();
        if (cp.x < g_4a47bc)
            return;
        if (cp.x > g_4a4798)
            return;
        if (cp.y < g_4a47d8)
            return;
        if (cp.y > g_4a479c)
            return;
        ((RenderNode*)this)->F4055b0((uint8_t*)item + 0xc0, cp.x, cp.y, g_4a4790, g_4a4794, g_4a47e0, g_4a47e4, g_4a47b8);
        *(int32_t*)((uint8_t*)item + 0x3c) = 1;
        if (((RenderNode*)item)->IsKindOf(g_data_5418))
        {
            if (*(int32_t*)((uint8_t*)this + 0xac) < 4)
            {
                *(int32_t*)((uint8_t*)item + 0x3c) = 0;
            }
            else if (*(int32_t*)((uint8_t*)item + 0xcc) == 0)
            {
                *(int32_t*)((uint8_t*)item + 0x40) = *(int32_t*)((uint8_t*)this + 0x8c);
            }
            else
            {
                *(int32_t*)((uint8_t*)item + 0x40) = *(int32_t*)((uint8_t*)this + 0x90);
            }
        }
        else
        {
            *(int32_t*)((uint8_t*)item + 0x40) = *(int32_t*)((uint8_t*)this + 0x98);
        }
    }
    else if (((RenderNode*)item)->IsKindOf(g_data_53f8))
    {
        w12 = 2;
        w14 = 0;
        w16 = 200;
        v40 = *(int32_t*)((uint8_t*)this + 0x80);
        v4c = v40;
        v4c = v4c * w12;
        v48 = v4c;
        if (v48 > 0x320000)
        {
            v30 = v48;
        }
        else
        {
            w44 = 0;
            w46 = 0x32;
            v30 = *(int32_t*)&w44;
        }
        v18 = v40;
        w10 = 8;
        v18 = v18 * w10;
        d14 = *(int32_t*)&w14;
        if (d14 < v18)
            d14 = v18;
        d14 /= 2;
        ang = *(int16_t*)((uint8_t*)item + 0x19e);
        s3c = *(int32_t*)(g_sineTable + ((ang + 0x80) & 0x1ff) * 64);
        s3c = (int32_t)(((int64_t)s3c * d14) >> 16);
        g_4a47b8 = ang;
        v68 = s3c;
        v60 = d14;
        s38 = *(int32_t*)(g_sineTable + (ang & 0x1ff) * 64);
        s38 = (int32_t)(((int64_t)s38 * v60) >> 16);
        ang2 = *(int16_t*)((uint8_t*)item + 0x19e);
        v9c = s38;
        ang2 -= 0x80;
        v58 = v30;
        s34 = *(int32_t*)(g_sineTable + ((ang2 + 0x80) & 0x1ff) * 64);
        s34 = (int32_t)(((int64_t)s34 * v58) >> 16);
        g_4a47b8 = ang2;
        v90 = s34;
        v50 = v30;
        s2c = *(int32_t*)(g_sineTable + (ang2 & 0x1ff) * 64);
        s2c = (int32_t)(((int64_t)s2c * v50) >> 16);
        v94 = s2c;
        qpp = ((RenderNode*)item)->GetPair(&gp);
        cvx = ((int16_t*)qpp)[1];
        dxy = ((int16_t*)qpp)[3];
        dp = (uint8_t*)item + 0x164;
        ep = (uint8_t*)item + 0x16c;
        bp = (uint8_t*)item + 0x174;
        *(int32_t*)dp = cvx;
        *(int32_t*)(dp + 4) = dxy;
        *(int32_t*)ep = cvx;
        *(int32_t*)(ep + 4) = dxy;
        *(int32_t*)bp = cvx;
        h9a = ((int16_t*)&v68)[1];
        h92 = ((int16_t*)&v90)[1];
        *(int32_t*)(bp + 4) = dxy;
        *(int32_t*)((uint8_t*)item + 0x164) += h9a;
        h9e = ((int16_t*)&v9c)[1];
        *(int32_t*)((uint8_t*)item + 0x168) += h9e;
        v20 = h92;
        v28 = h92;
        v28 -= h9a;
        v1c = ((int16_t*)&v94)[1];
        v24 = v1c;
        v24 -= h9e;
        *(int32_t*)ep += v28;
        h96 = v1c;
        *(int32_t*)bp -= h9a + h92;
        *(int32_t*)((uint8_t*)item + 0x170) += v24;
        *(int32_t*)((uint8_t*)item + 0x178) -= h9e + h96;
        ((RenderNode*)this)->F4055b0((uint8_t*)item + 0x164, *(int32_t*)((uint8_t*)item + 0x164), *(int32_t*)((uint8_t*)item + 0x168), g_4a4790, g_4a4794, g_4a47e0, g_4a47e4, g_4a47b8);
        ((RenderNode*)this)->F4055b0(ep, *(int32_t*)ep, *(int32_t*)(ep + 4), g_4a4790, g_4a4794, g_4a47e0, g_4a47e4, g_4a47b8);
        ((RenderNode*)this)->F4055b0(bp, *(int32_t*)bp, *(int32_t*)(bp + 4), g_4a4790, g_4a4794, g_4a47e0, g_4a47e4, g_4a47b8);
        *(int32_t*)((uint8_t*)item + 0x17c) = 1;
    }
}

extern "C" int32_t g_460744;
// FUNCTION: HOVER 0x004055b0
#ifdef _MSC_VER
#pragma optimize("", off)
#endif
void RenderNode::F4055b0(void* a0, int32_t a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7)
{
    int32_t x = a1;
    int32_t y = a2;
    int16_t w1c;
    int16_t w1a;
    int16_t w20;
    int16_t w1e;
    int32_t t1;
    int32_t t2;
    int32_t mb0;
    int32_t mb4;
    int32_t r28;
    int32_t mb8;
    int32_t mbc;
    int32_t r30;
    int32_t vc4;
    int32_t vc8;
    int32_t vc0;
    int32_t vc034;
    int32_t tc8;
    int32_t tc0;
    int32_t mc94;
    int32_t mc98;
    int32_t rc3c;
    int32_t mc9c;
    int32_t mca0;
    int32_t rc44;
    int32_t ma8;
    int32_t mac;
    int32_t ma4;
    int32_t va8;
    int64_t qx;
    int64_t qy;
    int16_t w18;
    int16_t w16;
    int16_t w04;
    int16_t w02;
    int32_t vc84;
    int32_t vc80;
    int32_t vc78;
    int32_t vc4c;
    int32_t va1c;
    int32_t v888;
    int32_t v84;
    int32_t v8c;
    int32_t v90;
    int32_t va60;
    int32_t va64;
    int32_t va5c;
    int32_t va54;
    int32_t va6c;
    int32_t va70;
    int32_t va68;
    int32_t va58;
    int32_t va74;

    if (g_460744 != 0)
    {
        w1c = 0;
        w1a = (int16_t)x;
        w20 = 0;
        w1e = (int16_t)y;
        w1a -= (int16_t)a3;
        w1e -= (int16_t)a4;
        t1 = ((int32_t*)g_sineTable)[((a7 << 4) & 0x1fff)];
        t2 = ((int32_t*)g_sineTable)[(((a7 << 4) + 0x800) & 0x1fff)];
        mb0 = *(int32_t*)&w20;
        mb4 = t1;
        MulFix38e0(&mb4, NULL, mb0);
        r28 = mb4;
        mb8 = *(int32_t*)&w1c;
        mbc = t2;
        MulFix38e0(&mbc, NULL, mb8);
        r30 = mbc;
        vc4 = r28;
        vc8 = r30;
        vc0 = vc4;
        vc8 += vc0;
        vc034 = vc8;
        x = ((int16_t*)&vc034)[1];
        tc8 = ((int32_t*)g_sineTable)[((a7 << 4) & 0x1fff)];
        tc0 = ((int32_t*)g_sineTable)[(((a7 << 4) + 0x800) & 0x1fff)];
        mc94 = *(int32_t*)&w1c;
        mc98 = tc8;
        MulFix38e0(&mc98, NULL, mc94);
        rc3c = mc98;
        mc9c = *(int32_t*)&w20;
        mca0 = tc0;
        MulFix38e0(&mca0, NULL, mc9c);
        rc44 = mca0;
        ma8 = rc3c;
        mac = rc44;
        ma4 = ma8;
        mac -= ma4;
        va8 = mac;
        y = ((int16_t*)&va8)[1];
        x += a3;
        y += a4;
    }
    qx = (int64_t)x;
    qx <<= 32;
    qx /= (int64_t)*(int32_t*)((uint8_t*)this + 0x80);
    w18 = (int16_t)qx;
    qx /= 65536;
    w16 = (int16_t)qx;
    qy = (int64_t)y;
    qy <<= 32;
    qy /= (int64_t)*(int32_t*)((uint8_t*)this + 0x80);
    w04 = (int16_t)qy;
    qy /= 65536;
    w02 = (int16_t)qy;
    vc84 = *(int32_t*)&w18;
    vc80 = *(int32_t*)((uint8_t*)this + 0x7c);
    vc78 = vc84;
    vc80 -= vc78;
    vc4c = vc80;
    va1c = a5;
    v888 = vc4c;
    v84 = va1c;
    v888 -= v84;
    v8c = v888;
    v90 = v8c;
    *(int32_t*)&w18 = v90;
    va60 = *(int32_t*)&w04;
    va64 = *(int32_t*)((uint8_t*)this + 0x7c);
    va5c = va60;
    va64 -= va5c;
    va54 = va64;
    va6c = a6;
    va70 = va54;
    va68 = va6c;
    va70 -= va68;
    va58 = va70;
    va74 = va58;
    *(int32_t*)&w04 = va74;
    *(int32_t*)a0 = *(int32_t*)((uint8_t*)this + 0x5c) + w16;
    *(int32_t*)((uint8_t*)a0 + 4) = *(int32_t*)((uint8_t*)this + 0x60) + w02;
}
#ifdef _MSC_VER
#pragma optimize("", on)
#endif

// FUNCTION: HOVER 0x004010c0
void __fastcall F4010c0(void* self)
{
    MerlinPlane* p = (MerlinPlane*)self;
    int32_t y1;
    int32_t y0;
    int32_t x1 = p->m_p1c;
    int32_t x0 = p->m_p14;

    p->m_w24 &= (int16_t)0xfff8;
    if (x0 < x1)
    {
        p->m_p28 = x0;
        p->m_p2c = x1;
    }
    else
    {
        p->m_p28 = x1;
        p->m_p2c = x0;
    }
    y0 = p->m_p18;
    y1 = p->m_p20;
    if (y0 < y1)
    {
        p->m_p30 = y0;
        p->m_p34 = y1;
    }
    else
    {
        p->m_p30 = y1;
        p->m_p34 = y0;
    }
    if (x0 != x1)
        p->m_w24 |= 2;
    if (y0 != y1)
        p->m_w24 |= 4;
    if ((p->m_w24 & 6) == 0)
        p->m_w24 |= 1;
    x1 -= x0;
    y1 -= y0;
    p->m_p38 = x1;
    p->m_p3c = y1;
    p->m_p40 = x1 * x1 + y1 * y1;
}

// FUNCTION: HOVER 0x00406090
int32_t HoverTc::M406090(void* a1, void* a2)
{
    MerlinPlane w;
    int32_t py;
    int32_t idx;
    int32_t first;
    int32_t d2;
    int32_t u;
    int32_t v;
    int32_t px;
    uint8_t* entry;
    int32_t pv;
    double r;
    int32_t c1;
    int32_t c2;
    int32_t n1;
    int32_t n2;
    int32_t mask;
    int32_t hi;
    int32_t lo;

    *(void**)((uint8_t*)this + 0x20) = a1;
    g_4a1d68 = a2;
    g_4a453c = *(int32_t*)((uint8_t*)g_4a1d68 + 0x40);
    g_4a455c = *(int32_t*)((uint8_t*)g_4a1d68 + 0x3c);
    g_4a4548 = *(void**)((uint8_t*)g_4a1d68 + 0x14);
    g_4a1d70 = *(int32_t*)((uint8_t*)g_4a1d68 + 0x2c);
    g_4a452c = *(int32_t*)((uint8_t*)g_4a1d68 + 0x5c);
    g_4a454c = *(int32_t*)((uint8_t*)g_4a1d68 + 0x20);
    g_4a4544 = *(int32_t*)((uint8_t*)g_4a1d68 + 0x50);
    n1 = g_4a453c;
    n1 >>= 1;
    c1 = 0;
    while (n1 != 0)
    {
        n1 >>= 1;
        c1++;
    }
    g_4a455c = *(int32_t*)((uint8_t*)g_4a1d68 + 0x3c);
    c2 = 0;
    n2 = g_4a455c;
    n2 >>= 1;
    g_4a4538 = c1;
    while (n2 != 0)
    {
        n2 >>= 1;
        c2++;
    }
    g_4a4554 = c2;
    g_4a1d78 = *(int32_t*)((uint8_t*)this + 0x24);
    g_4a4a34 = *(int32_t*)((uint8_t*)this + 0x2c);
    g_4a4a30 = *(int32_t*)((uint8_t*)g_4a1d68 + 0x7c);
    Setup6c90(*(void**)this);
    *(int32_t*)((uint8_t*)this + 0x30) = (g_4a454c << 16) / 0x55;
    *(int32_t*)((uint8_t*)this + 0x34) = (*(int32_t*)((uint8_t*)g_4a1d68 + 0x54) << 16) - 0x2aaaab;
    *(int32_t*)((uint8_t*)this + 0x34) &= 0x1ffffff;
    memset(*(void**)((uint8_t*)this + 0x40), 0, g_4a454c * 4);
    entry = HOVER_LPTR(g_4a1d68, 0x64) + ((*(int32_t*)((uint8_t*)g_4a1d68 + 0x88) * (*(int32_t*)((uint8_t*)g_4a1d68 + 0x54) & 0x1ff) * 6) / 512) * 8;
    w.m_p14 = *(int32_t*)((uint8_t*)g_4a1d68 + 0x48);
    w.m_p18 = *(int32_t*)((uint8_t*)g_4a1d68 + 0x4c);
    g_4a4498.m_p14 = *(int32_t*)((uint8_t*)g_4a1d68 + 0x48);
    g_4a4498.m_p18 = *(int32_t*)((uint8_t*)g_4a1d68 + 0x4c);
    g_4a4498.m_p1c = *(int32_t*)entry + *(int32_t*)((uint8_t*)g_4a1d68 + 0x48);
    g_4a4498.m_p20 = *(int32_t*)(entry + 4) + *(int32_t*)((uint8_t*)g_4a1d68 + 0x4c);
    F4010c0(&g_4a4498);
    entry += g_4a454c * 8;
    g_4a44e0.m_p14 = *(int32_t*)((uint8_t*)g_4a1d68 + 0x48);
    g_4a44e0.m_p18 = *(int32_t*)((uint8_t*)g_4a1d68 + 0x4c);
    g_4a44e0.m_p1c = *(int32_t*)entry + *(int32_t*)((uint8_t*)g_4a1d68 + 0x48);
    g_4a44e0.m_p20 = *(int32_t*)(entry + 4) + *(int32_t*)((uint8_t*)g_4a1d68 + 0x4c);
    F4010c0(&g_4a44e0);
    entry -= g_4a454c * 8;
    g_4a4550 = *(int32_t*)((uint8_t*)this + 0x3c);
    for (idx = (*(int32_t*)(*(uint8_t**)((uint8_t*)this + 0x20) + 0x30) - 1) * 4; idx >= 0; idx -= 4)
    {
        uint8_t* node = *(uint8_t**)((uint8_t*)*(void***)((uint8_t*)*(void**)((uint8_t*)this + 0x20) + 0x2c) + idx);
        if (*(int32_t*)(node + 0x64) != 0)
        {
            px = (int32_t)*(int16_t*)(node + 0x5a) - *(int32_t*)((uint8_t*)g_4a1d68 + 0x48);
            py = (int32_t)*(int16_t*)(node + 0x5c) - *(int32_t*)((uint8_t*)g_4a1d68 + 0x4c);
            pv = (int32_t)*(int16_t*)(node + 0x58);
            d2 = py * py + px * px;
            r = sqrt(fabs((double)d2));
            u = px * pv;
            u = (int)((double)u / r);
            v = (int)((double)(py * pv) / r);
            px += *(int32_t*)((uint8_t*)g_4a1d68 + 0x48);
            py += *(int32_t*)((uint8_t*)g_4a1d68 + 0x4c);
            *(int32_t*)(node + 0x14) = px + v;
            *(int32_t*)(node + 0x18) = py - u;
            *(int32_t*)(node + 0x1c) = px - v;
            *(int32_t*)(node + 0x20) = py + u;
            F4010c0(node);
        }
        if (this->M405b90(node) != 0)
            this->M405ce0(node, node, 1, 0);
    }
    g_4a4528 = 0;
    this->M405fa0(0);
    g_4a4718 = *(void**)((uint8_t*)this + 0x44);
    g_4a4714 = 0;
    g_4a471c = 0;
    first = 1;
    *(int32_t*)((uint8_t*)this + 0x28) = 0;
    g_4a4534 = 0;
    mask = g_4a455c - 1;
    if (g_4a454c > 0)
    {
        do
        {
            if ((mask & g_4a4534) == 0)
            {
                w.m_p14 = *(int32_t*)entry + *(int32_t*)((uint8_t*)g_4a1d68 + 0x48);
                w.m_p18 = *(int32_t*)(entry + 4) + *(int32_t*)((uint8_t*)g_4a1d68 + 0x4c);
                entry += g_4a455c * 8;
                F4010c0(&w);
            }
            {
                uint8_t* bucket = *(uint8_t**)((uint8_t*)*(void**)((uint8_t*)this + 0x40) + g_4a4534 * 4);
                if (bucket != 0)
                {
                    while (bucket != 0)
                    {
                        if (*(int32_t*)(bucket + 4) != 0)
                            F4044a0(*(int32_t*)(bucket + 8), *(int32_t*)(bucket + 0xc), *(int16_t*)bucket, *(int32_t*)(bucket + 0x10));
                        else
                            *(int32_t*)((uint8_t*)this + 0x28) |= F404580(*(void**)(bucket + 0xc), *(void**)(bucket + 0x10));
                        bucket = *(uint8_t**)(bucket + 0x14);
                    }
                    if ((mask & g_4a4534) == 0)
                    {
                        first = 0;
                        *(int32_t*)((uint8_t*)this + 0x28) |= F4045e0((void*)g_4a4544, &w);
                    }
                    else
                    {
                        first = 1;
                    }
                }
                else
                {
                    if ((mask & g_4a4534) == 0)
                    {
                        if (first)
                        {
                            *(int32_t*)((uint8_t*)this + 0x28) |= F4045e0((void*)g_4a4544, &w);
                            first = 0;
                        }
                        else
                            *(int32_t*)((uint8_t*)this + 0x28) |= F406d30((void*)g_4a4544, &w);
                    }
                }
            }
            if ((mask & g_4a4534) == 0)
            {
                hi = *(int32_t*)(HOVER_LPTR(g_4a1d68, 0x34) + g_4a4534 * 4) & -g_4a453c;
                lo = *(int32_t*)(HOVER_LPTR(g_4a1d68, 0x38) + g_4a4534 * 4) & -g_4a453c;
                g_4a1d64 = hi;
                g_4a4568 = hi - g_4a453c - 1;
                g_4a4530 = g_4a453c + lo + 1;
                g_4a1d6c = *(int32_t*)(HOVER_LPTR(g_4a1d68, 0x30) + g_4a4534 * 4) << 10;
                if (*(int32_t*)((uint8_t*)g_4a1d68 + 0x68) != 0)
                    g_4a4540 = *(int32_t*)((uint8_t*)g_4a1d68 + 0x28) - g_4a455c - g_4a4534 + g_4a454c - 1;
                else
                    g_4a4540 = *(int32_t*)((uint8_t*)g_4a1d68 + 0x28) + g_4a4534;
                g_4a456c = lo;
                if (g_4a471c != 0)
                    this->M402e00(g_4a471c);
                else
                {
                    g_4a456c = lo;
                    *(int32_t*)((uint8_t*)this + 0x28) |= 1;
                    if (g_4a454c - 1 > g_4a4534)
                        ((SpanChain*)*(void**)this)->AddPair6d00(g_4a1d64, lo);
                    ((SpanChain*)*(void**)this)->F403a60(this);
                }
            }
            g_4a4534++;
        } while (g_4a4534 < g_4a454c);
    }
    ((SpanChain*)*(void**)this)->F403a60(this);
    return 0;
}

// FUNCTION: HOVER 0x00406d30
extern "C" int32_t __cdecl F406d30(void* a0, void* a1)
{
    int32_t result = 0;
    uint8_t* p = (uint8_t*)g_4a471c;

    while (p != NULL)
    {
        uint8_t* next = *(uint8_t**)(p + 0x34);
        uint8_t* child = *(uint8_t**)(p + 0x10);
        PtPair buf;
        int32_t r = F401260(child, &buf, a1, &buf);
        uint32_t f18 = ((uint16_t)(r - 1) == 0);
        uint32_t f20;
        int32_t dv;
        int16_t* slot;

        *(uint32_t*)(p + 0x14) = f18;
        f20 = f18;

        if ((uint16_t)r != 0)
        {
            int32_t ay = buf.y;
            int32_t ax = buf.x;
            int32_t by = *(int32_t*)((uint8_t*)a1 + 0x18);
            int32_t bx = *(int32_t*)((uint8_t*)a1 + 0x14);
            int32_t dx = abs(ax - bx);
            int32_t dy = abs(ay - by);
            uint32_t m = (dy < dx) ? dy : dx;
            uint32_t den = dy - m / 2 + dx + 1;
            uint32_t num = (uint32_t)(dx * dx + dy * dy);
            uint32_t dist1 = (den + num / den) / 2;

            *(int16_t*)(p + 8) = (int16_t)dist1;
            if (*(int32_t*)(p + 0xc) != 0 && *(int32_t*)(child + 0x68) != 0)
                f20 = !f18;

            if (f20 != 0)
            {
                int32_t ay2 = buf.y;
                int32_t ax2 = buf.x;
                int32_t by2 = *(int32_t*)((uint8_t*)child + 0x20);
                int32_t bx2 = *(int32_t*)((uint8_t*)child + 0x1c);
                int32_t dx2 = abs(ax2 - bx2);
                int32_t dy2 = abs(ay2 - by2);
                uint32_t m2 = (dy2 < dx2) ? dy2 : dx2;
                uint32_t den2 = dy2 - m2 / 2 + dx2 + 1;
                uint32_t num2 = (uint32_t)(dx2 * dx2 + dy2 * dy2);
                uint32_t dist2 = (den2 + num2 / den2) / 2;

                slot = (int16_t*)(p + 0xa);
                dv = dist2;
            }
            else
            {
                int32_t ay3 = buf.y;
                int32_t ax3 = buf.x;
                int32_t by3 = *(int32_t*)((uint8_t*)child + 0x18);
                int32_t bx3 = *(int32_t*)((uint8_t*)child + 0x14);
                int32_t dx3 = abs(ax3 - bx3);
                int32_t dy3 = abs(ay3 - by3);
                uint32_t m3 = (dy3 < dx3) ? dy3 : dx3;
                uint32_t den3 = dy3 - m3 / 2 + dx3 + 1;
                uint32_t num3 = (uint32_t)(dx3 * dx3 + dy3 * dy3);
                uint32_t dist3 = (den3 + num3 / den3) / 2;

                slot = (int16_t*)(p + 0xa);
                dv = dist3;
            }

            *(int16_t*)slot = (int16_t)dv;

            if (*(int32_t*)(p + 0xc) == 0)
            {
                uint32_t cont;

                if (*(int32_t*)(p + 0x18) != 0)
                {
                    result = 1;
                    *(int32_t*)(p + 0x18) = 0;
                }

                *slot += (*(int32_t*)(p + 0x14) != 0)
                    ? *(int16_t*)((uint8_t*)child + 0xb8)
                    : *(int16_t*)((uint8_t*)child + 0xba);

                do
                {
                    uint8_t* node;
                    cont = 0;
                    node = *(uint8_t**)(p + 0x30);
                    if (node != NULL && *(int32_t*)(node + 0xc) != 0
                        && *(int32_t*)(node + 0x18) == 0
                        && *(int16_t*)(node + 8) > *(int16_t*)(p + 8))
                    {
                        uint8_t* prev;
                        uint8_t* old;

                        result = 1;
                        prev = *(uint8_t**)(node + 0x30);
                        if (prev != NULL)
                            *(void**)(prev + 0x34) = *(void**)(node + 0x34);
                        if (*(void**)(node + 0x34) != NULL)
                            *(void**)(*(uint8_t**)(node + 0x34) + 0x30) = prev;
                        if (g_4a471c == node)
                            g_4a471c = *(void**)(node + 0x34);
                        old = *(uint8_t**)(p + 0x34);
                        *(void**)(p + 0x34) = node;
                        if (old != NULL)
                            *(void**)(old + 0x30) = node;
                        *(void**)(node + 0x34) = old;
                        *(void**)(node + 0x30) = p;
                        cont = 1;
                    }
                } while (cont);
            }
            else if (*(int32_t*)(p + 0x18) == 0)
            {
                uint8_t* prev = *(uint8_t**)(p + 0x30);
                uint8_t* node = prev;

    while (node != NULL)
                {
                    if (*(int32_t*)(node + 0x18) == 0
                        && *(int16_t*)(node + 8) < *(int16_t*)(p + 8))
                        break;
                    node = *(uint8_t**)(node + 0x30);
                }

                if (node != prev)
                {
                    uint8_t* old;

                    result = 1;
                    if (prev != NULL)
                        *(void**)(prev + 0x34) = *(void**)(p + 0x34);
                    if (*(void**)(p + 0x34) != NULL)
                        *(void**)(*(uint8_t**)(p + 0x34) + 0x30) = prev;
                    if (g_4a471c == p)
                        g_4a471c = *(void**)(p + 0x34);

                    if (node != NULL)
                    {
                        old = *(uint8_t**)(node + 0x34);
                        *(void**)(node + 0x34) = p;
                        if (old != NULL)
                            *(void**)(old + 0x30) = p;
                        *(void**)(p + 0x34) = old;
                        *(void**)(p + 0x30) = node;
                    }
                    else
                    {
                        uint8_t* head;

                        *(void**)(p + 0x30) = NULL;
                        head = (uint8_t*)g_4a471c;
                        *(void**)(p + 0x34) = head;
                        if (head != NULL)
                            *(void**)(head + 0x30) = p;
                        g_4a471c = p;
                    }
                }
            }
            else
            {
                uint8_t* prev;
                uint8_t* node;

                result = 1;
                *(int32_t*)(p + 0x18) = 0;

                prev = *(uint8_t**)(p + 0x30);
                if (prev != NULL)
                    *(void**)(prev + 0x34) = *(void**)(p + 0x34);
                if (*(void**)(p + 0x34) != NULL)
                    *(void**)(*(uint8_t**)(p + 0x34) + 0x30) = prev;
                if (g_4a471c == p)
                    g_4a471c = *(void**)(p + 0x34);

                node = (uint8_t*)g_4a471c;
                if (node == NULL)
                {
                    *(void**)(p + 0x34) = NULL;
                    *(void**)(p + 0x30) = NULL;
                    g_4a471c = p;
                }
                else
                {
                    for (;;)
                    {
                        if (*(int32_t*)(node + 0x18) == 0
                            && *(int16_t*)(node + 8) > *(int16_t*)(p + 8))
                        {
                            uint8_t* old = *(uint8_t**)(node + 0x30);

                            *(void**)(node + 0x30) = p;
                            if (old != NULL)
                            {
                                *(void**)(old + 0x34) = p;
                                *(void**)(p + 0x30) = old;
                                *(void**)(p + 0x34) = node;
                            }
                            else
                            {
                                g_4a471c = p;
                                *(void**)(p + 0x34) = node;
                                *(void**)(p + 0x30) = old;
                            }
                            break;
                        }
                        if (*(void**)(node + 0x34) == NULL)
                        {
                            *(void**)(node + 0x34) = p;
                            *(void**)(p + 0x30) = node;
                            *(void**)(p + 0x34) = NULL;
                            break;
                        }
                        node = *(uint8_t**)(node + 0x34);
                    }
                }
            }
        }
        else
        {
            if (*(int32_t*)(p + 0x18) == 0)
            {
                F4045e0(a0, a1);
                return 1;
            }
        }

        p = next;
    }

    return result;
}

/* ---------------------------------------------------------------------------
 * Dashboard widget present (retail 0x42d3e0).  The centre cutout widget
 * paints its offscreen CDC onto the window; when the active DC reports the
 * half-resolution flag the destination rect is doubled on the way out.
 * ------------------------------------------------------------------------- */

// FUNCTION: HOVER 0x0042d3e0
extern "C" void __fastcall F42d3e0(void* self, void* unused, void* arg)
{
    uint8_t* w = (uint8_t*)self;
    void* obj;

    (void)unused;
    obj = GetSub401140();
    if (obj == NULL)
        return;

    if (*(int32_t*)((uint8_t*)obj + 0x544) != 0)
    {
        int32_t t = *(int32_t*)(w + 0x60);
        int32_t b = *(int32_t*)(w + 0x68);
        int32_t cy = b - t;
        int32_t l = *(int32_t*)(w + 0x5c);
        int32_t r = *(int32_t*)(w + 0x64);
        void* src = *(void**)(w + 0x84);
        int32_t cx = r - l;
        int32_t cy2 = cy * 2;
        int32_t cx2 = cx * 2;
        int32_t t2 = t * 2;
        int32_t l2 = l * 2;
        void* hdc = (src == NULL) ? NULL : *(void**)((uint8_t*)src + 4);

        StretchBlt(*(void**)((uint8_t*)arg + 4), l2, t2, cx2, cy2,
                   hdc, l, t, cx, cy, HOVER_SRCCOPY);
    }
    else
    {
        int32_t t = *(int32_t*)(w + 0x60);
        int32_t l = *(int32_t*)(w + 0x5c);
        void* src = *(void**)(w + 0x84);
        int32_t b = *(int32_t*)(w + 0x68);
        int32_t cy = b - t;
        int32_t r = *(int32_t*)(w + 0x64);
        int32_t cx = r - l;
        void* hdc = (src == NULL) ? NULL : *(void**)((uint8_t*)src + 4);

        BitBlt(*(void**)((uint8_t*)arg + 4), l, t, cx, cy, hdc, l, t, HOVER_SRCCOPY);
    }
}

/* ---------------------------------------------------------------------------
 * Centre-cutout content pass (retail 0x407160).  Advances the 8-frame tick,
 * reprojects the player/sub view origin into the four cutout edges, walks the
 * world node lists handing each payload to F404bb0, then visibility-tags and
 * pushes every live entity through F4055b0.
 * ------------------------------------------------------------------------- */

int32_t* __fastcall DivFix(int32_t* p, void* unused, int32_t d);

// FUNCTION: HOVER 0x00407160
extern "C" void __fastcall F407160(void* self, void* unused, void* a0)
{
    uint8_t* s = (uint8_t*)self;
    void* obj;
    int32_t k;
    int32_t n;
    int32_t c;
    int32_t i;
    int32_t off;
    uint8_t* sub;
    uint8_t* node;
    void* item;
    PtPair p1;
    PtPair p2;
    IntPair q0;
    IntPair q1;
    IntPair q2;
    IntPair q3;
    IntPair tmp;

    n = *(int32_t*)(s + 0xac) + 1;
    *(int32_t*)(s + 0xac) = n;
    if (n >= 8)
        *(int32_t*)(s + 0xac) = 0;

    k = 0;
    obj = GetSub401140();
    if (obj != NULL)
        k = *(int32_t*)((uint8_t*)obj + 0x540);

    sub = (uint8_t*)a0 + 0x818c;
    IntPair p0 = ((RenderNode*)sub)->CopyPos();
    g_4a4790 = p0.x;
    g_4a4794 = p0.y;

    g_4a47b8 = (int32_t)*(int16_t*)((uint8_t*)a0 + 0x832a) - 0x80;
    g_4a47b0 = *(int32_t*)(*(uint8_t**)((uint8_t*)a0 + 0x188) + 0x1c);
    c = (int32_t)*(int16_t*)(s + 0x88);
    g_4a47bc = g_4a4790 - c;
    g_4a47d8 = g_4a4794 - c;
    g_4a4798 = g_4a4790 + c;
    g_4a479c = g_4a4794 + c;

    {
        int32_t d = *(int32_t*)(s + 0x80);
        int32_t v;

        v = ((RenderNode*)sub)->GetPair(&p1)->x;
        v = (int32_t)(((int64_t)v << 16) / d);
        g_4a47e0 = (*(int32_t*)(s + 0x7c) - v) - *(int32_t*)(s + 0x78);
    }
    {
        int32_t d = *(int32_t*)(s + 0x80);
        int32_t v;

        v = ((RenderNode*)sub)->GetPair(&p2)->y;
        v = (int32_t)(((int64_t)v << 16) / d);
        g_4a47e4 = (*(int32_t*)(s + 0x7c) - v) - *(int32_t*)(s + 0x74);
    }

    node = *(uint8_t**)((uint8_t*)g_460970 + 0x8390);
    while (node != NULL)
    {
        item = *(void**)(node + 8);
        *(int32_t*)((uint8_t*)item + 0x3c) = 0;
        ((RenderNode*)self)->F404bb0(a0, item, k);
        node = *(uint8_t**)node;
    }

    node = *(uint8_t**)((uint8_t*)g_460970 + 0x83ac);
    while (node != NULL)
    {
        item = *(void**)(node + 8);
        ((RenderNode*)self)->F404bb0(a0, item, k);
        node = *(uint8_t**)node;
    }

    if (g_460750 != 0)
    {
        for (i = 0, off = 0; i < g_4a4848; i++, off += 4)
        {
            item = *(void**)((uint8_t*)g_4a4844 + off);

            q0 = ((RenderNode*)item)->CopyPos();
            if (q0.x < g_4a47bc)
            {
                *(int32_t*)((uint8_t*)item + 0x3c) = 0;
            }
            else
            {
                q1 = ((RenderNode*)item)->CopyPos();
                if (q1.x > g_4a4798)
                {
                    *(int32_t*)((uint8_t*)item + 0x3c) = 0;
                }
                else
                {
                    q2 = ((RenderNode*)item)->CopyPos();
                    if (q2.y < g_4a47d8)
                    {
                        *(int32_t*)((uint8_t*)item + 0x3c) = 0;
                    }
                    else
                    {
                        q3 = ((RenderNode*)item)->CopyPos();
                        *(int32_t*)((uint8_t*)item + 0x3c) = 1;
                        if (q3.y > g_4a479c)
                            *(int32_t*)((uint8_t*)item + 0x3c) = 0;
                    }
                }
            }

            tmp = ((RenderNode*)item)->CopyPos();
            ((RenderNode*)self)->F4055b0((uint8_t*)item + 0x9c, tmp.x, tmp.y,
                    g_4a4790, g_4a4794, g_4a47e0, g_4a47e4, g_4a47b8);
            *(int32_t*)((uint8_t*)item + 0x40) = *(int32_t*)(s + 0x98);
        }
    }
}
