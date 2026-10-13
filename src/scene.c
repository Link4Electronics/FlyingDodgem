#include <string.h>
#include "flyingdodgem_types.h"
#include "callees.h"
#include "scene.h"

#if defined(_MSC_VER)
#pragma comment(lib, "user32.lib")
extern "C" __declspec(dllimport) char* __stdcall CharUpperA(char* psz);
#else
extern "C" char* __stdcall CharUpperA(char* psz);
#endif

extern "C" void* g_460970;
extern "C" MerlinPlane g_4a4498;
extern "C" MerlinPlane g_4a44e0;
extern "C" int32_t g_4a453c;
extern "C" int32_t g_4a4544;
extern "C" int32_t g_4a1d70;
extern "C" void* g_4a452c;
extern "C" void* g_4a4548;
extern "C" int32_t g_4a61cfc;
extern "C" void* g_4a1d68;
extern "C" int32_t g_4a4a44;
extern "C" int32_t g_4a1d74;
extern "C" int32_t g_4a4a4c;
extern "C" int32_t g_4a4a14;
extern "C" int32_t g_4a4a50;
extern "C" int32_t g_4a4a24;
extern "C" int32_t g_4a4a28;
extern "C" int32_t g_4a4a2c;
extern "C" int32_t g_4a4a3c;
extern "C" int32_t g_4a4a20;
extern "C" int32_t g_4a4a38;
extern "C" int32_t g_4a4a48;
extern "C" int32_t g_4a4534;
extern "C" int32_t g_4a454c;
extern "C" int32_t g_461cfc;
extern "C" int32_t g_4a4a1c;
extern "C" int32_t g_4a4a40;
extern "C" int32_t g_4a4a34;
extern "C" uint8_t g_sineTable[];
extern "C" int16_t g_4a4528;
extern "C" int32_t g_4a4768;
extern "C" int32_t g_4a4774;
extern "C" void** g_4a4844;
extern "C" int32_t g_4a4840;
extern "C" int32_t g_4a4858;
extern "C" void* g_4c57a8;
extern "C" int32_t g_4c57ac;

/* 4-byte packed address fields (see render.c HOVER_LPTR): matching sees the
 * exact original tokens. */
#if defined(FLYINGDODGEM_GDI_COMPAT)
#define HOVER_LPTR(b, o) ((uint8_t*)(uintptr_t)*(uint32_t*)((uint8_t*)(b) + (o)))
#else
#define HOVER_LPTR(b, o) (*(uint8_t**)((uint8_t*)(b) + (o)))
#endif

// FUNCTION: HOVER 0x0041e230
CWorld::CWorld() : CBaseWorld(), m_side(), m_span(), m_thing1(10), m_thing2(10)
{
    m_nFrame = 0;
    m_nFlags = 0;
    m_nRows = 0;
    memset(m_grid, 0, sizeof(m_grid));
    m_dInertia = 0.985;
    m_dAccel1 = 1.2;
    m_dAccel2 = 1.2;
    m_dAccel3 = 1.2;
    m_nTick = 0;
    m_nColumns = 512;
    m_nActive = 1;
    g_460970 = this;
}

void CWorld::Update()
{
}

CBeacon::~CBeacon()
{
}

// FUNCTION: HOVER 0x00423ea0
void __cdecl BuildBeacons(void)
{
    int32_t nCount = 0;
    int32_t nState;
    int32_t nIndex;

    nState = g_4a4774 ? -1 : 0;
    while (nState != 0)
    {
        CBeaconKey key;
        CBeaconValue* pValue;
        ((HoverTc*)&g_4a4768)->M44a003(&nState, &key, &pValue);
        CharUpperA(key.m_psz);
        if (Ext43f978(key.m_psz, "BEACON_", 7) == 0)
            nCount++;
    }

    ((HoverTc*)&g_4a4840)->M449598(nCount, -1);

    nState = g_4a4774 ? -1 : 0;
    nIndex = 0;
    while (nState != 0)
    {
        CBeaconKey key;
        CBeaconValue* pValue;
        ((HoverTc*)&g_4a4768)->M44a003(&nState, &key, &pValue);
        CharUpperA(key.m_psz);
        if (Ext43f978(key.m_psz, "BEACON_", 7) != 0)
            continue;
        CBeacon* p = (CBeacon*)g_4c57a8;
        int32_t nBlocks = g_4c57ac;
        void** slot = &g_4a4844[nIndex++];
        if (p == 0)
        {
            CBeacon* block = (CBeacon*)Ext43e2cf(sizeof(CBeacon) * nBlocks);
            int32_t i;
            CBeacon* q;
            g_4c57a8 = block;
            ((HoverTc*)&g_4a4858)->M44994f(block);
            p = (CBeacon*)g_4c57a8;
            for (i = nBlocks - 1, q = p; i > 0; i--, q++)
                q->m_pNext = q + 1;
            p[nBlocks - 1].m_pNext = NULL;
        }
        g_4c57a8 = p->m_pNext;
        if (p)
            *slot = new (p) CBeacon(pValue);
        else
            *slot = NULL;
    }

    Ext423cd0();
}

// FUNCTION: HOVER 0x00405fa0
void HoverTc::M405fa0(int16_t a1)
{
    int16_t idx = a1;

    while (idx != -1)
    {
        uint8_t* wall = *(uint8_t**)((uint8_t*)*(void**)((uint8_t*)this + 0x20) + 0x54);
        int32_t back;

        wall = *(uint8_t**)(wall + idx * 4);
        back = (g_4a4498.m_p14 - *(int32_t*)(wall + 0x14)) * *(int32_t*)(wall + 0x3c)
             + (*(int32_t*)(wall + 0x18) - g_4a44e0.m_p18) * *(int32_t*)(wall + 0x38);
        if (back < 0)
        {
            this->M405fa0(*(int16_t*)(wall + 0x48));
            if (this->M405b90(wall) != 0)
            {
                uint8_t* disp = *(uint8_t**)((uint8_t*)this + 0x20);
                void* grp = HOVER_LPTR(*(void**)(disp + 0x18), *(int16_t*)(wall + 0x44) * 4);
                if (this->M405ce0(grp, wall, 0, g_4a4528) != 0)
                    g_4a4528++;
            }
            idx = *(int16_t*)(wall + 0x4a);
        }
        else
        {
            this->M405fa0(*(int16_t*)(wall + 0x4a));
            if (this->M405b90(wall) != 0)
            {
                uint8_t* disp = *(uint8_t**)((uint8_t*)this + 0x20);
                void* grp = HOVER_LPTR(*(void**)(disp + 0x18), *(int16_t*)(wall + 0x44) * 4);
                if (this->M405ce0(grp, wall, 0, g_4a4528) != 0)
                    g_4a4528++;
            }
            idx = *(int16_t*)(wall + 0x48);
        }
    }
}

/* Span-host dispatch (retail 0x402b10): the palette-ramp method lives on
 * the same HostObj as render.c/wallspan.c (identical layout, so the
 * thiscall mangles alike and links to render.c's definition); the two
 * draw slots are bare thiscall-shaped targets reached indirectly. */
typedef void (__cdecl *HostFn0)(void);
typedef void (__cdecl *HostFn1)(void);
struct HostObj
{
    HostFn0 fn0;
    HostFn1 fn1;
    int32_t F403850(int32_t arg);
};
typedef void (__fastcall *HostDrawFn)(void* self);

extern "C" uint8_t g_data_5570[1];
extern "C" uint8_t g_data_5340[1];
extern "C" uint8_t g_data_5318[1];
extern "C" uint8_t g_data_53f8[1];
extern "C" uint8_t g_data_5488[1];
extern "C" uint8_t g_data_54b8[1];
extern "C" void __fastcall DrawCell3910(void* self, void* unused, int32_t x, int32_t y, int32_t ignored);
extern "C" int32_t g_4a4524;
extern "C" int32_t g_4a4564;
extern "C" int32_t g_4a453c;
extern "C" void __fastcall SpanSetup2990(void* self, void* unused, void* a1, void* a2, int32_t a3, int32_t a4);

// FUNCTION: HOVER 0x00401520
int32_t HoverTc::F401520(void* a1, void* a2)
{
    uint8_t* self = (uint8_t*)this;
    int32_t* pt = (int32_t*)a1;
    int32_t* org = (int32_t*)a2;
    int32_t x0 = *(int32_t*)(self + 0);
    int32_t y0 = *(int32_t*)(self + 4);
    int32_t x1 = *(int32_t*)(self + 8);
    int32_t y1 = *(int32_t*)(self + 0xc);
    int32_t px = pt[0];
    int32_t py = pt[1];
    int32_t ox = org[0];
    int32_t oy = org[1];
    int32_t dx = px - ox;
    int32_t dy = py - oy;
    int32_t i;
    /* Axis-aligned short-circuit (retail checks +0x2c/+0x30): no slope to
     * divide by, so only test containment and clamp the moving axis. */
    if (dx == 0 && dy == 0)
    {
        if (px < x0 || px > x1 || py < y0 || py > y1)
            return 0;
        return 1;
    }
    if (dx == 0)
    {
        if (px < x0 || px > x1)
            return 0;
        if (py < y0)
            py = y0;
        else if (py > y1)
            py = y1;
        pt[1] = py;
        return 1;
    }
    if (dy == 0)
    {
        if (py < y0 || py > y1)
            return 0;
        if (px < x0)
            px = x0;
        else if (px > x1)
            px = x1;
        pt[0] = px;
        return 1;
    }
    /* Cohen-Sutherland style: walk the point towards the origin until it is
     * inside, recomputing the crossing with 16.16 direct (retail `shl 16 /
     * idiv / imul / sar 16` shape, no FP helpers). At most 4 iterations. */
    for (i = 0; i < 4; i++)
    {
        int32_t slope;
        if (px >= x0 && px <= x1 && py >= y0 && py <= y1)
        {
            pt[0] = px;
            pt[1] = py;
            return 1;
        }
        if (px < x0)
        {
            slope = (int32_t)(((int32_t)(py - oy) << 16) / dx);
            py = oy + (int32_t)((int32_t)(slope * (x0 - ox)) >> 16);
            px = x0;
        }
        else if (px > x1)
        {
            slope = (int32_t)(((int32_t)(py - oy) << 16) / dx);
            py = oy + (int32_t)((int32_t)(slope * (x1 - ox)) >> 16);
            px = x1;
        }
        else if (py < y0)
        {
            slope = (int32_t)(((int32_t)(px - ox) << 16) / dy);
            px = ox + (int32_t)((int32_t)(slope * (y0 - oy)) >> 16);
            py = y0;
        }
        else if (py > y1)
        {
            slope = (int32_t)(((int32_t)(px - ox) << 16) / dy);
            px = ox + (int32_t)((int32_t)(slope * (y1 - oy)) >> 16);
            py = y1;
        }
        dx = px - ox;
        dy = py - oy;
        if (dx == 0 && dy == 0)
        {
            if (px < x0 || px > x1 || py < y0 || py > y1)
                return 0;
            pt[0] = px;
            pt[1] = py;
            return 1;
        }
    }
    if (px < x0 || px > x1 || py < y0 || py > y1)
        return 0;
    pt[0] = px;
    pt[1] = py;
    return 1;
}

// FUNCTION: HOVER 0x00401c00
void HoverTc::F401c00(void* a1, void* a2)
{
    uint8_t* obj = (uint8_t*)a1;
    uint8_t* rend;
    uint8_t color;
    uint16_t w;
    uint32_t dw;
    int32_t hasColor;

    if (((RenderNode*)obj)->IsKindOf(g_data_5570) == 0)
    {
        if (((RenderNode*)obj)->IsKindOf(g_data_5340) == 0)
        {
            if (((RenderNode*)obj)->IsKindOf(g_data_5318) == 0)
            {
                if (((RenderNode*)obj)->IsKindOf(g_data_53f8) == 0)
                    return;
                hasColor = 0;
                if (((RenderNode*)obj)->IsKindOf(g_data_5488) != 0)
                {
                    color = *(uint8_t*)((uint8_t*)this + 0x8c);
                    hasColor = 1;
                }
                else if (((RenderNode*)obj)->IsKindOf(g_data_54b8) != 0)
                {
                    if (*(int32_t*)(obj + 0xc0) != 0)
                        color = *(uint8_t*)((uint8_t*)this + 0x98);
                    else if (*(int32_t*)(obj + 0x21c) != 0)
                        color = *(uint8_t*)((uint8_t*)this + 0x94);
                    else
                        color = *(uint8_t*)((uint8_t*)this + 0x90);
                    hasColor = 1;
                }
                if (hasColor != 0)
                {
                    *(uint8_t*)((uint8_t*)this + 0x1c) = color;
                    w = color;
                    w = (uint16_t)((w << 8) | w);
                    dw = w;
                    dw = (dw << 16) | dw;
                    *(uint32_t*)((uint8_t*)this + 0x20) = dw;
                }
                this->F401980(*(int32_t*)(obj + 0x164), *(int32_t*)(obj + 0x168), *(int32_t*)(obj + 0x16c), *(int32_t*)(obj + 0x170));
                this->F401980(*(int32_t*)(obj + 0x16c), *(int32_t*)(obj + 0x170), *(int32_t*)(obj + 0x174), *(int32_t*)(obj + 0x178));
                this->F401980(*(int32_t*)(obj + 0x174), *(int32_t*)(obj + 0x178), *(int32_t*)(obj + 0x164), *(int32_t*)(obj + 0x168));
                return;
            }
            if (a2 == 0)
            {
                uint8_t* parent = *(uint8_t**)(obj + 0x78);
                if (parent == 0)
                    return;
                if ((*(uint8_t*)(parent + 0x24) & 8) == 0)
                    return;
            }
            if (*(int32_t*)(obj + 0x3c) == 0)
                return;
            color = *(uint8_t*)(obj + 0x40);
            *(uint8_t*)((uint8_t*)this + 0x1c) = color;
            w = color;
            w = (uint16_t)((w << 8) | w);
            dw = w;
            dw = (dw << 16) | dw;
            *(uint32_t*)((uint8_t*)this + 0x20) = dw;
            DrawCell3910(this, NULL, *(int32_t*)(obj + 0xc0), *(int32_t*)(obj + 0xc4), 5);
            return;
        }
        if (*(int32_t*)(obj + 0x28) == 0)
            return;
        color = *(uint8_t*)((uint8_t*)this + 0x9c);
        *(uint8_t*)((uint8_t*)this + 0x1c) = color;
        w = color;
        w = (uint16_t)((w << 8) | w);
        dw = w;
        dw = (dw << 16) | dw;
        *(uint32_t*)((uint8_t*)this + 0x20) = dw;
        this->F401980(*(int32_t*)(obj + 0xf0), *(int32_t*)(obj + 0xf4), *(int32_t*)(obj + 0xf8), *(int32_t*)(obj + 0xfc));
        return;
    }

    if (*(int32_t*)(obj + 0x38) != 0)
        return;

    if (a2 == 0)
    {
        uint8_t* parent = *(uint8_t**)(obj + 0x78);
        if (parent == 0)
            return;
        if ((*(uint8_t*)(parent + 0x24) & 8) == 0)
            return;
    }

    if (*(int32_t*)(obj + 0x3c) == 0)
        return;

    rend = *(uint8_t**)(obj + 0x78);
    if (*(int32_t*)(rend + 0xbc) == 0)
        color = *(uint8_t*)((uint8_t*)this + 0x98);
    else if (*(int16_t*)(rend + 0xa4) > 0)
        color = *(uint8_t*)((uint8_t*)this + 0xa8);
    else
        color = *(uint8_t*)((uint8_t*)this + 0x9c);

    *(uint8_t*)((uint8_t*)this + 0x1c) = color;
    w = color;
    w = (uint16_t)((w << 8) | w);
    dw = w;
    dw = (dw << 16) | dw;
    *(uint32_t*)((uint8_t*)this + 0x20) = dw;

    this->F401980(*(int32_t*)(obj + 0x84), *(int32_t*)(obj + 0x88), *(int32_t*)(obj + 0x8c), *(int32_t*)(obj + 0x90));
}

// FUNCTION: HOVER 0x00401980
void HoverTc::F401980(int32_t x0, int32_t y0, int32_t x1, int32_t y1)
{
    int32_t dx;
    int32_t dy;
    int32_t pt0[2];
    int32_t pt1[2];
    int32_t fx0;
    int32_t fy0;
    int32_t fx1;
    int32_t fy1;
    int32_t adx;
    int32_t ady;
    uint8_t* bits;
    int32_t pitch;
    uint8_t color;
    uint8_t* base;
    int32_t xstep;
    int32_t ystep;
    int32_t slope;
    int32_t acc;
    int32_t n;

    dx = x0 - x1;
    if (dx < 0)
        dx = -dx;
    dy = y0 - y1;
    if (dy < 0)
        dy = -dy;
    *(int32_t*)((uint8_t*)this + 0x2c) = dx;
    *(int32_t*)((uint8_t*)this + 0x30) = dy;

    pt0[0] = x0;
    pt0[1] = y0;
    pt1[0] = x1;
    pt1[1] = y1;
    if (this->F401520(pt0, pt1) == 0)
        return;
    if (this->F401520(pt1, pt0) == 0)
        return;

    fx0 = pt0[0];
    fy0 = *(int32_t*)((uint8_t*)this + 0x18) - pt0[1] - 1;
    fx1 = pt1[0];
    fy1 = *(int32_t*)((uint8_t*)this + 0x18) - pt1[1] - 1;

    adx = fx0 - fx1;
    if (adx < 0)
        adx = -adx;
    ady = fy0 - fy1;
    if (ady < 0)
        ady = -ady;
    *(int32_t*)((uint8_t*)this + 0x2c) = adx;
    *(int32_t*)((uint8_t*)this + 0x30) = ady;
    if (adx == 0 && ady == 0)
        return;

    bits = *(uint8_t**)((uint8_t*)this + 0x10);
    pitch = *(int32_t*)((uint8_t*)this + 0x24);
    color = *(uint8_t*)((uint8_t*)this + 0x1c);
    if (bits == 0 || pitch == 0)
        return;
    base = bits + fy0 * pitch + fx0;

    if (adx >= ady)
    {
        slope = (int32_t)(((int32_t)ady << 16) / adx);
        if (fy1 < fy0)
            pitch = -pitch;
        if (fx1 >= fx0)
            xstep = 1;
        else
            xstep = -1;
        ystep = pitch;
        acc = 0x8000;
        for (n = adx; n != 0; n--)
        {
            *base = color;
            acc += slope;
            if ((acc & 0xffff0000) != 0)
            {
                base += ystep;
                acc &= 0xffff;
            }
            base += xstep;
        }
    }
    else
    {
        slope = (int32_t)(((int32_t)adx << 16) / ady);
        if (fx1 < fx0)
            slope = -slope;
        if (fy1 < fy0)
            pitch = -pitch;
        ystep = pitch;
        acc = 0x8000;
        for (n = ady; n != 0; n--)
        {
            int32_t off = acc >> 16;
            base[off] = color;
            acc += slope;
            base += ystep;
        }
    }
}

extern "C" int32_t g_4a4524;
extern "C" int32_t g_4a4564;
extern "C" int32_t g_4a453c;
extern "C" void __fastcall SpanSetup2990(void* self, void* unused, void* a1, void* a2, int32_t a3, int32_t a4);

struct SpanArena
{
    void F403980(int32_t a1, int32_t a2, int32_t a3, int32_t a4);
    int32_t F4065f0(int32_t a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5);
};

// FUNCTION: HOVER 0x00402630
void HoverTc::F402630(void* a1, void* a2, void* a3)
{
    uint8_t* node = (uint8_t*)a1;
    int32_t a2i = (int32_t)(intptr_t)a2;
    int32_t a3i = (int32_t)(intptr_t)a3;
    int32_t local1 = *(int32_t*)(node + 0xb4);
    uint8_t* local2 = node + 0xac;
    int32_t result = 0;
    int32_t ebx, ecx, edx, ebp;

    g_4a4564 = a3i;
    g_4a4524 = a2i;
    *(uint8_t*)(uintptr_t)(uint32_t)(*(int32_t*)(node + 0x10) + 0x24) |= 8;

    if (*(int32_t*)(node + 0x40) == 0)
        goto R3;

    if (*(int32_t*)(node + 0x6c) > 0)
    {
        edx = *(int32_t*)(node + 0x60);
        ebx = *(int32_t*)(uintptr_t)(uint32_t)*(int32_t*)(node + 0x84);
        if (ebx > local1)
        {
            if (ebx > a2i)
            {
                if (ebx >= a3i)
                {
                    ((SpanArena*)HOVER_LPTR(this, 0))->F403980(edx, a2i, a3i, *(int32_t*)(node + 0x48));
                    result = 1;
                    goto R1_flag;
                }
                ((SpanArena*)HOVER_LPTR(this, 0))->F403980(edx, a2i, ebx, *(int32_t*)(node + 0x48));
                g_4a4524 = g_4a453c + ebx;
                goto R1_flag;
            }
            if (a2i >= local1)
            {
                if (a3i <= local1)
                {
                    ((SpanArena*)HOVER_LPTR(this, 0))->F403980(edx, a2i, a3i, *(int32_t*)(node + 0x48));
                    result = 1;
                    goto R1_flag;
                }
                ((SpanArena*)HOVER_LPTR(this, 0))->F403980(edx, a2i, local1, *(int32_t*)(node + 0x48));
                g_4a4524 = g_4a453c + local1;
                goto R1_flag;
            }
        }
        else
        {
            if (a2i < local1 && a3i > local1)
            {
                ((SpanArena*)HOVER_LPTR(this, 0))->F403980(edx, a2i, local1, *(int32_t*)(node + 0x48));
                g_4a4524 = g_4a453c + local1;
                goto R1_flag;
            }
        }
        goto R1_flag;
    }

R2:
    if (*(int32_t*)(node + 0x4c) == 0)
        goto R3;
    edx = *(int32_t*)(node + 0x64);
    ebx = *(int32_t*)(uintptr_t)(uint32_t)*(int32_t*)(node + 0x88);
    ebp = *(int32_t*)(node + 0xb8);
    if (ebx > ebp)
    {
        if (a3i > ebp)
        {
            if (a2i >= ebp)
            {
                ((SpanArena*)HOVER_LPTR(this, 0))->F403980(edx, a2i, a3i, *(int32_t*)(node + 0x48));
                result = 1;
                goto R2_flag;
            }
            ((SpanArena*)HOVER_LPTR(this, 0))->F403980(edx, ebp, a3i, *(int32_t*)(node + 0x48));
            g_4a4564 = ebp - g_4a453c;
            goto R2_flag;
        }
        if (a3i > ebx)
        {
            if (a2i >= ebx)
            {
                ((SpanArena*)HOVER_LPTR(this, 0))->F403980(edx, a2i, a3i, *(int32_t*)(node + 0x48));
                result = 1;
                goto R2_flag;
            }
            ((SpanArena*)HOVER_LPTR(this, 0))->F403980(edx, ebx, a3i, *(int32_t*)(node + 0x48));
            g_4a4564 = ebx - g_4a453c;
            goto R2_flag;
        }
        goto R2_flag;
    }
    goto R2_flag;

R1_flag:
    if (result)
        return;
    goto R2;

R2_flag:
    if (result)
        return;

R3:
    if (((SpanArena*)this)->F4065f0((int32_t)(intptr_t)node, (int32_t)(intptr_t)local2, local1, ebp, *(int32_t*)(node + 0x64)))
        return;
    if (g_4a4564 < g_4a4524)
        return;
    ((SpanArena*)this)->F4065f0((int32_t)(intptr_t)node, (int32_t)(intptr_t)local2, local1, ebp, *(int32_t*)(node + 0x64));
    SpanSetup2990(this, local2, node, local2, g_4a4524, g_4a4564);
}

// FUNCTION: HOVER 0x00402b10
void SpanChain::F402b10(void* a1, int32_t a2, int32_t a3, int32_t a4)
{
    /* Retail passes the column x-value in a1 and the node struct in a3
     * (verified against the 0x403dc9 call site, which pushes an int16
     * x-value, the column index, [ctx+0x24] and [ctx+0x1c]); both are
     * 4-byte stack slots, so the declared types are codegen-neutral and
     * the casts below just name that reality. */
    uint8_t* host;
    uint8_t* prow;
    int32_t row;
    int32_t den;
    int32_t shade;
    int32_t tmp;
    int32_t w1;
    int32_t w2;
    int32_t u0;
    int32_t u1;
    int32_t vb;
    int32_t vb2;

#if defined(FLYINGDODGEM_GDI_COMPAT)
    /* Native: our F403a60 reconstruction passes a null node; retail
     * always passes a live node here. */
    if ((uint8_t*)(intptr_t)a3 == 0)
        return;
#endif
    row = (a2 & -g_4a453c) - g_4a1d70;
    den = row < 0 ? -row : row;
    den++;
    /* Straight into the global (no persp var): retail keeps no home slot
     * here, and every later use reads [g_4a4a44] from memory. */
    g_4a4a44 = *(int32_t*)((uint8_t*)g_4a452c + (g_4a4544 - a4) * 4 + 0x1000) / den;
    if (*(int32_t*)((uint8_t*)this + 0x24) != 0)
    {
        /* NB: read back the just-stored global (not the persp var) so that
         * persp dies here: no home slot, and the later imuls keep their
         * [g_4a4a44] memory operands as in retail. */
        tmp = g_4a4a44 & 0x7ff0;
        shade = 0;
        shade = ((uint8_t*)g_4a4548)[(uint32_t)tmp >> 4];
        shade++;
        g_4a1d74 = shade;
        if (g_4a4a44 < 0xfa)
            shade = g_461cfc;
        g_4a1d74 = shade;
        tmp = *(int32_t*)((uint8_t*)(intptr_t)a3 + 0x2c);
        if (shade >= tmp)
            shade = tmp - 1;
        g_4a1d74 = shade;
        if (g_461cfc > shade)
            shade = g_461cfc;
    }
    else
    {
        shade = g_461cfc;
    }
    host = (uint8_t*)g_4a1d68;
    prow = (uint8_t*)(intptr_t)(*(int32_t*)((uint8_t*)(intptr_t)a3 + 0x28) + (shade << 5));
    g_4a4a1c = *(int32_t*)(prow + 0x18);
    if (*(int32_t*)(host + 0x68) != 0)
    {
        int32_t ph = *(int32_t*)(host + 0x54);
        int32_t v0;
        w1 = g_4a454c - g_4a4534;
        ph <<= 6;
        w1--;
        vb = *(int32_t*)(host + 0x48);
        vb <<= 16;
        w2 = g_4a454c - (int32_t)(intptr_t)a1;
        w2--;
        tmp = (ph + 0x1554) & 0x7ffc;
        vb2 = *(int32_t*)(host + 0x4c);
        vb2 <<= 16;
        tmp = *(int32_t*)(g_sineTable + tmp);
        tmp *= g_4a4a44;
        u0 = tmp + vb;
        tmp = (ph + 0x2aa8) & 0x7ffc;
        v0 = *(int32_t*)(g_sineTable + tmp);
        tmp = (ph - 0xaac) & 0x7ffc;
        ph += 0xaa8;
        v0 *= g_4a4a44;
        tmp = *(int32_t*)(g_sineTable + tmp);
        tmp *= g_4a4a44;
        u1 = tmp + vb2;
        vb += v0;
        tmp = *(int32_t*)(g_sineTable + (ph & 0x7ffc));
        tmp *= g_4a4a44;
        vb2 += tmp;
    }
    else
    {
        int32_t ph;
        int32_t bA;
        int32_t e48;
        tmp = (int32_t)(intptr_t)a1;
        w2 = g_4a4534;
        bA = *(int32_t*)(host + 0x48);
        w1 = tmp;
        bA <<= 16;
        ph = *(int32_t*)(host + 0x54);
        ph <<= 6;
        tmp = (ph + 0x1554) & 0x7ffc;
        tmp = *(int32_t*)(g_sineTable + tmp);
        tmp *= g_4a4a44;
        vb = tmp + bA;
        tmp = (ph + 0x2aa8) & 0x7ffc;
        e48 = *(int32_t*)(host + 0x4c);
        e48 <<= 16;
        tmp = *(int32_t*)(g_sineTable + tmp);
        tmp *= g_4a4a44;
        u0 = tmp + bA;
        ph += 0xaa8;
        tmp = (ph - 0x1554) & 0x7ffc;
        ph &= 0x7ffc;
        tmp = *(int32_t*)(g_sineTable + tmp);
        tmp *= g_4a4a44;
        vb2 = tmp + e48;
        tmp = *(int32_t*)(g_sineTable + ph);
        tmp *= g_4a4a44;
        u1 = tmp + e48;
    }
    {
        /* Short-lived segment temps (no mega-ranges): each dies before
         * the next is born, so nothing here needs a home slot. */
        int32_t yscal = a2;
        int32_t ydiv;
        tmp = w2 - w1;
        g_4a1d74 = shade;
        yscal *= g_4a4a34;
        yscal += *(int32_t*)(host + 0x28);
        g_4a4a14 = tmp;
        tmp = w1;
        u1 -= vb2;
        tmp += yscal;
        ydiv = g_4a454c;
        ydiv <<= shade;
        g_4a4a4c = tmp;
        u1 /= ydiv;
        g_4a4a50 = u1;
        tmp = u0;
        vb2 >>= shade;
        tmp -= vb;
        tmp /= ydiv;
        g_4a4a24 = tmp;
        tmp = w1;
        vb >>= shade;
        tmp *= g_4a4a50;
        tmp += vb2;
        g_4a4a28 = tmp;
        tmp = g_4a4a24 * a2 + vb;
        g_4a4a2c = tmp;
        g_4a4a3c = ((struct HostObj*)host)->F403850(g_4a4a44);
    }
    g_4a4a20 = (*(int32_t*)(prow + 4) << 16) + 0xffff;
    g_4a4a38 = *(int32_t*)prow << 16;
    g_4a4a48 = 0x10 - *(int32_t*)(prow + 0x10);
    if (*(int32_t*)(host + 0x80) != 0)
    {
        uint8_t* tbl = *(uint8_t**)(host + 0x8);
        ((HostDrawFn)*(void**)(tbl + g_4a4a48 * 4))((void*)tbl);
    }
    else
    {
        ((HostDrawFn)*(void**)(host + 0xc))((void*)host);
    }
}
