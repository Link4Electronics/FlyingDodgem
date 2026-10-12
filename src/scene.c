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
                void* grp = *(void**)((uint8_t*)*(void**)(disp + 0x18) + *(int16_t*)(wall + 0x44) * 4);
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
                void* grp = *(void**)((uint8_t*)*(void**)(disp + 0x18) + *(int16_t*)(wall + 0x44) * 4);
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

// STUB: HOVER 0x00401c00
void HoverTc::F401c00(void* a1, void* a2)
{
    (void)a1;
    (void)a2;
}

// STUB: HOVER 0x00401980
void HoverTc::F401980(int32_t x0, int32_t y0, int32_t x1, int32_t y1)
{
    (void)x0;
    (void)y0;
    (void)x1;
    (void)y1;
}

// STUB: HOVER 0x00402630
void HoverTc::F402630(void* a1, void* a2, void* a3)
{
    (void)a1;
    (void)a2;
    (void)a3;
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
