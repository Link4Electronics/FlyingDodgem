#include "flyingdodgem_types.h"
#include "callees.h"
#include "scene.h"

extern "C" MerlinPlane g_4a4498;
extern "C" MerlinPlane g_4a44e0;
extern "C" int16_t g_4a4528;
extern "C" void* g_4a1d60;
extern "C" int32_t g_4a453c;
extern "C" int32_t g_4a1d70;
extern "C" void* g_4a4548;
extern "C" int32_t g_4a4568;
extern "C" int32_t g_4a4530;
extern "C" int32_t g_4a1d64;
extern "C" int32_t g_4a456c;
extern "C" void* g_4a4720;
extern "C" int32_t g_461cfc;
extern "C" int32_t g_4a1d78;
extern "C" void* g_4a471c;
extern "C" int32_t g_4a4544;
extern "C" int32_t g_4a1d6c;

/* Stubs for the functions the four unimplemented scene/display entries call.
 * They are compiled out of line (separate TU from their callers) so the
 * optimizer can never inline them away.  Bodies are empty: reccmp excludes
 * // STUB: entities from both the implemented and the accuracy statistics, the
 * markers only exist so the call operands on both sides resolve to one name.
 *
 * Each stub reproduces the retail stack frame, i.e. the return value of
 * `ret N` after `call`:
 *
 *   - the constructors/destructors belong to the classes declared in scene.h,
 *     so the callers reach them through normal C++ construction and get
 *     __thiscall for free;
 *   - the members of HoverTc take stack arguments on top of `this`;
 *   - Ext* taking a lone `self` are __thiscall with `this` in ECX only, which
 *     __fastcall reproduces without touching EDX;
 *   - Ext* with no `self` are __cdecl, the caller pops the arguments.
 */

// STUB: HOVER 0x00401160
MerlinPlane::MerlinPlane()
{
}
// STUB: HOVER 0x00401210
MerlinPlane::~MerlinPlane()
{
}
// STUB: HOVER 0x004030f0
void __fastcall F4030f0(void* self)
{
    (void)self;
}
// STUB: HOVER 0x00404030
int32_t HoverTc::F404030(void* a1, void* a2, void* a3)
{
    (void)a1;
    (void)a2;
    (void)a3;
    return 0;
}
// FUNCTION: HOVER 0x00402e00
int32_t HoverTc::M402e00(void* a1)
{
    uint8_t* node;
    int32_t scale;
    int32_t negN;
    int32_t cnt;

    (void)a1;
    g_4a1d60 = (void*)0x4a1d80;
    if (*(int32_t*)((uint8_t*)this + 0x28) != 0)
        F4030f0(this);
    node = (uint8_t*)g_4a471c;
    while (node != NULL)
    {
        int32_t depth = *(int16_t*)(node + 8);
        if (*(int32_t*)(node + 0x18) != 0)
        {
            if (depth != 0)
            {
                *(int32_t*)(node + 0x58) = 0;
                node = *(uint8_t**)(node + 0x34);
                continue;
            }
        }
        else if (depth <= 0)
        {
            if (depth != 0)
            {
                *(int32_t*)(node + 0x58) = 0;
                node = *(uint8_t**)(node + 0x34);
                continue;
            }
        }
        else
        {
            uint8_t* st = node + 0xac;
            int32_t row;
            int32_t col;
            scale = *(int32_t*)g_4a1d6c / depth;
            *(int32_t*)(node + 0x58) = 1;
            row = (*(int32_t*)(node + 0x6c) * scale >> 10) + g_4a1d70;
            col = (*(int32_t*)(node + 0x70) * scale >> 10) + g_4a1d70;
            negN = -g_4a453c;
            row &= negN;
            col &= negN;
            *(int32_t*)(st + 8) = row;
            *(int32_t*)(st + 0xc) = col;
            if (*(int32_t*)(node + 0xa0) != 0)
            {
                int32_t shade;
                uint8_t* child;
                uint8_t* edge;
                int32_t tex;
                int32_t sh;
                int32_t rowbase;
                int32_t* rp;
                int32_t mask;
                int32_t tbl;
                int32_t rec;
                int32_t span;
                int32_t den;
                if (g_4a1d78 == 0)
                    shade = g_461cfc;
                else
                {
                    int32_t lim;
                    shade = ((uint8_t*)g_4a4548)[(depth >> (*(int32_t*)(node + 0x9c) + 4)) & 0x7ff];
                    child = *(uint8_t**)(node + 0x5c);
                    lim = *(int32_t*)(child + 0x2c) - 1;
                    if (lim < shade)
                        shade = lim;
                    if (g_461cfc > shade)
                        shade = g_461cfc;
                }
                edge = *(uint8_t**)(node + 0x10);
                edge[0x24] |= 8;
                child = *(uint8_t**)(node + 0x5c);
                tex = *(int16_t*)(node + 0xa);
                sh = (uint8_t)(*(int32_t*)(node + 0x9c) + shade);
                rowbase = *(int32_t*)(child + 0x28) + shade * 32;
                rp = (int32_t*)rowbase;
                mask = (int16_t)(tex >> sh) & *rp;
                tbl = *(int32_t*)((uint8_t*)rp + 0x1c);
                rec = *(int32_t*)(tbl + mask * 8 + 4);
                span = *(int32_t*)(rec + 4);
                den = span - row;
                if (den != 0)
                    *(int32_t*)(node + 0x98) = *(int32_t*)(node + 0x68) / den;
                else
                    *(int32_t*)(node + 0x98) = 0;
                {
                    int16_t n = *(int16_t*)(rec + 0);
                    uint8_t* bump = (uint8_t*)g_4a1d60;
                    g_4a1d60 = (void*)(bump + n * 20);
                    if (n != 0)
                    {
                        int32_t* arr = *(int32_t**)(rec + 4);
                        cnt = n;
                        while (cnt > 0)
                        {
                            int32_t vx = *(int16_t*)(arr + 0) << sh;
                            int32_t vy = *(int16_t*)(arr + 1) << sh;
                            vx -= *(int32_t*)(node + 0x60);
                            vy -= *(int32_t*)(node + 0x60);
                            *(int32_t*)bump = vx;
                            *(int32_t*)(bump + 4) = vy;
                            if (vx > *(int32_t*)(node + 0x64))
                                *(int32_t*)(node + 0x64) = vx;
                            vx = *(int32_t*)(arr + 1);
                            arr = (int32_t*)((uint8_t*)arr + 8);
                            *(int32_t*)(bump + 0x10) = vx;
                            bump += 0x14;
                            cnt--;
                        }
                    }
                }
                node = *(uint8_t**)(node + 0x34);
                continue;
            }
            *(int32_t*)(node + 0xa4) = 0;
            if (*(int32_t*)(node + 0x8c) != 0)
            {
                int32_t a = *(int32_t*)(*(uint8_t**)(node + 0x84));
                int32_t b = *(int32_t*)(*(uint8_t**)(node + 0x88));
                int32_t mn = a;
                int32_t mx;
                int32_t r0 = row;
                int32_t r1 = col;
                if (b < a)
                    mn = b;
                mx = mn;
                if (r0 <= mx)
                    mx = r0;
                else
                    mn = r0;
                *(int32_t*)(node + 0x74) = mx;
                if (b != 0xfff0000 && b > r1)
                    mn = b;
                else
                    mn = r1;
                if (a == 0xfff0000 || r1 <= a)
                    mx = r1;
                else
                    mx = a;
                *(int32_t*)(node + 0x78) = mx;
                mn = mn;
            }
            node = *(uint8_t**)(node + 0x34);
            continue;
        }
        {
            uint8_t* st = node + 0xac;
            *(int32_t*)(st + 8) = *(int32_t*)(node + 0x6c) > 0 ? g_4a4530 : g_4a4568;
            *(int32_t*)(st + 0xc) = *(int32_t*)(node + 0x70) > 0 ? g_4a4530 : g_4a4568;
            *(int32_t*)(node + 0x58) = 0;
        }
        node = *(uint8_t**)(node + 0x34);
    }
    this->F404030(g_4a4720, (void*)g_4a1d64, (void*)g_4a456c);
    ((SpanChain*)*(void**)this)->F403a60(this);
    return 0;
}
// STUB: HOVER 0x004053a0
int32_t HoverTc::M4053a0(void* a1, void* a2)
{
    /* Retail: 2D overlay pass (world node lists → F401c00 line/cell draws).
     * The 3D walls already ran in M406090; this only stamps sprites/markers
     * and is left empty until F401980/DrawCell3910 are reconstructed. */
    (void)a1;
    (void)a2;
    return 0;
}
// STUB: HOVER 0x00405a10
int32_t HoverTc::M405a10(void* a1, void* a2)
{
    return 0;
}

// STUB: HOVER 0x00405ce0
int32_t HoverTc::M405ce0(void* a1, void* a2, int32_t a3, int16_t a4)
{
    (void)a1;
    (void)a2;
    (void)a3;
    (void)a4;
    return 0;
}

// STUB: HOVER 0x00407430
void __fastcall Ext407430(void* self)
{
}
// STUB: HOVER 0x0040cce0
int32_t HoverTc::M40cce0(void* a1, void* a2, void* a3, void* a4)
{
    return 0;
}
// STUB: HOVER 0x0040d150
CBeaconBase::CBeaconBase()
{
}
// STUB: HOVER 0x0040d220
CBeaconBase::~CBeaconBase()
{
}
// STUB: HOVER 0x0040d340
void __fastcall Ext40d340(void* self)
{
}
// STUB: HOVER 0x00410510
void* __fastcall Ext410510(void* self)
{
    return 0;
}
// STUB: HOVER 0x00413c10
int32_t HoverTc::M413c10(void* a1)
{
    return 0;
}
// STUB: HOVER 0x004184b0
int32_t HoverTc::M4184b0(void* a1, void* a2)
{
    return 0;
}
// STUB: HOVER 0x0041e1e0
CThingDerived::~CThingDerived()
{
}
// STUB: HOVER 0x00421fa0
CWorldSpan::CWorldSpan()
{
}
// STUB: HOVER 0x00422050
CWorldSpan::~CWorldSpan()
{
}
// STUB: HOVER 0x00423cd0
void* __cdecl Ext423cd0(void)
{
    return 0;
}
// STUB: HOVER 0x00424210
CBeaconSub::~CBeaconSub()
{
}
// STUB: HOVER 0x00425050
CWorldSide::CWorldSide()
{
}
// STUB: HOVER 0x004250c0
CWorldSide::~CWorldSide()
{
}
// STUB: HOVER 0x00427a80
int32_t HoverTc::M427a80(void* a1)
{
    return 0;
}
// STUB: HOVER 0x00427bb0
void* __fastcall Ext427bb0(void* self)
{
    return 0;
}
// STUB: HOVER 0x00427cc0
void* __fastcall Ext427cc0(void* self)
{
    return 0;
}
// STUB: HOVER 0x004283f0
void* __fastcall Ext4283f0(void* self)
{
    return 0;
}
// STUB: HOVER 0x0043e2cf
void* __cdecl Ext43e2cf(uint32_t cb)
{
    return 0;
}
// STUB: HOVER 0x0043f978
void* __cdecl Ext43f978(const char* psz, const char* hits, int32_t cch)
{
    return 0;
}
// STUB: HOVER 0x004491e4
CThing::CThing(int32_t nBlockSize)
{
}
// STUB: HOVER 0x00449277
CThing::~CThing()
{
}
// STUB: HOVER 0x004494cc
CBeaconSubBase::CBeaconSubBase()
{
}
// STUB: HOVER 0x00449534
CBeaconSubBase::~CBeaconSubBase()
{
}
// STUB: HOVER 0x00449598
int32_t HoverTc::M449598(int32_t a1, int32_t a2)
{
    return 0;
}
// STUB: HOVER 0x0044994f
int32_t HoverTc::M44994f(void* a1)
{
    return 0;
}
// STUB: HOVER 0x0044a003
int32_t HoverTc::M44a003(void* a1, void* a2, void* a3)
{
    return 0;
}
// STUB: HOVER 0x0044b986
CBeaconKey::CBeaconKey()
{
}
// STUB: HOVER 0x0044b9f4
CBeaconKey::~CBeaconKey()
{
}
// STUB: HOVER 0x0044bdef
int32_t CBeaconKey::M44bdef(uint32_t nID)
{
    return 0;
}
// STUB: HOVER 0x00451590
CBaseWorld::CBaseWorld()
{
}
// STUB: HOVER 0x0045164d
CBaseWorld::~CBaseWorld()
{
}
