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
