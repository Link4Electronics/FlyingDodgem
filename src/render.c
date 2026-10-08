#include "flyingdodgem_types.h"
#include <stdlib.h>

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
extern "C" void* g_4a4714;
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
extern "C" void* g_460970;

extern "C" void* __cdecl GetSub401140(void);

#if defined(_MSC_VER)
#pragma comment(lib, "gdi32.lib")
#define HOVER_IMPORT __declspec(dllimport)
#else
#define HOVER_IMPORT
#endif
#define HOVER_SRCCOPY 0x00cc0020u

extern "C" HOVER_IMPORT int32_t __stdcall Rectangle(void* hdc, int32_t left, int32_t top, int32_t right, int32_t bottom);

extern "C" HOVER_IMPORT void* __stdcall SelectObject(void* hdc, void* obj);

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

class RenderNode
{
public:
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

// STUB: HOVER 0x004512ff
static void* __stdcall PrevObject4512ff(void* prev)
{
    return prev;
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

// FUNCTION: HOVER 0x00406c90
extern "C" void __fastcall Setup6c90(void* self, void* unused)
{
    uint8_t* b = (uint8_t*)self;
    uint8_t* p = *(uint8_t**)(b + 0x18);

    *(void**)(b + 0x1c) = p;
    *(void**)(b + 4) = p;
    *(int32_t*)(b + 0x10) = 0;
    uint8_t* t = p + 0xcc;
    p = p + 0xa0;
    *(void**)(b + 0x1c) = p;
    *(int32_t*)(b + 0xc) = 0;
    uint8_t* r = p + 4;
    *(int32_t*)(b + 0x14) = 0;
    *(void**)(b + 8) = p;
    *(void**)(b + 0x1c) = t;
    *(void**)p = t;
    t = t + 0xa0;
    *(void**)(b + 0x1c) = t;
    *(void**)r = t;
    *(int32_t*)(p + 0x28) = 0;
    t = t + 0xa0;
    *(int32_t*)(p + 0x14) = 0;
    *(void**)(b + 0x1c) = t;
    *(void**)(p + 0x10) = t;
    int32_t size = *(int32_t*)b;
    t = t + size * 2;
    *(int32_t*)(p + 0x18) = 0;
    *(void**)(b + 0x1c) = t;
    *(void**)(p + 8) = p;
    *(void**)(p + 0xc) = r;
}

// FUNCTION: HOVER 0x00406d00
extern "C" void __fastcall AddPair6d00(void* self, void* unused, int32_t a1, int32_t a2)
{
    uint8_t* q = *(uint8_t**)((uint8_t*)self + 8);
    int32_t n = *(int32_t*)(q + 0x14);
    if (n < 0x28)
    {
        int32_t** base = *(int32_t***)(q + 0xc);
        int32_t* arr;
        int16_t* slot;

        n = n + 1;
        *(int32_t*)(q + 0x14) = n;
        arr = *base;
        slot = (int16_t*)((uint8_t*)arr + n * 4 - 4);
        slot[1] = (int16_t)a2;
        slot[0] = (int16_t)a1;
    }
}

// STUB: HOVER 0x00401260
extern "C" int32_t __fastcall F401260(void* self, void* unused, void* a1, void* buf)
{
    return 0;
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

// STUB: HOVER 0x004045e0
extern "C" int32_t __cdecl F4045e0(void* a0, void* a1)
{
    return 0;
}

// STUB: HOVER 0x00404bb0
void RenderNode::F404bb0(void* a0, void* a1, int32_t a2)
{
}

// STUB: HOVER 0x004055b0
void RenderNode::F4055b0(void* a0, int32_t a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7)
{
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
        DivFix(&v, unused, d);
        g_4a47e0 = (*(int32_t*)(s + 0x7c) - v) - *(int32_t*)(s + 0x78);
    }
    {
        int32_t d = *(int32_t*)(s + 0x80);
        int32_t v;

        v = ((RenderNode*)sub)->GetPair(&p2)->y;
        DivFix(&v, unused, d);
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
