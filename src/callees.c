#include "flyingdodgem_types.h"
#include "callees.h"
#include "scene.h"

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

// STUB: HOVER 0x004053a0
int32_t HoverTc::M4053a0(void* a1, void* a2)
{
    return 0;
}
// STUB: HOVER 0x00405a10
int32_t HoverTc::M405a10(void* a1, void* a2)
{
    return 0;
}
// STUB: HOVER 0x00406090
int32_t HoverTc::M406090(void* a1, void* a2)
{
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
