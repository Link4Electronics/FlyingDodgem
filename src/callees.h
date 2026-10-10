#ifndef FLYINGDODGEM_CALLEES_H
#define FLYINGDODGEM_CALLEES_H

#include "flyingdodgem_types.h"

/* External callees of the per-frame 3D draw entry (HOVER 0x402250) and of the
 * scene/display entries (0x41e230, 0x423ea0, 0x41ece0).  They are declared but
 * not implemented yet: the markers here only give reccmp the original address
 * so that the `call`/`jmp` operands inside those four functions resolve to the
 * same name on both sides.  Class members of the beacon/world hierarchies
 * (CBeaconBase, CBeaconSubBase, CBeaconSub, CBeaconKey, ...) are stubbed in
 * callees.c as members, so they carry __thiscall for free.
 *
 * Retail passes `this` in ECX and the remaining arguments on the stack
 * (__thiscall).  MSVC rejects __thiscall on a free function, so callees that
 * take stack arguments are members of HoverTc and are reached through a cast
 * of the object pointer.  Callees that only take `this` are __fastcall with a
 * single parameter, which puts it in ECX without touching EDX. */

struct HoverTc
{
    int32_t M4053a0(void* a1, void* a2);
    int32_t M405a10(void* a1, void* a2);
    int32_t M406090(void* a1, void* a2);
    int32_t M40cce0(void* a1, void* a2, void* a3, void* a4);
    int32_t M413c10(void* a1);
    int32_t M4184b0(void* a1, void* a2);
    int32_t M427a80(void* a1);
    int32_t M449598(int32_t a1, int32_t a2);
    int32_t M44994f(void* a1);
    int32_t M44a003(void* a1, void* a2, void* a3);
};

/* __thiscall, `this` in ECX only (spelled __fastcall on a free function). */
extern "C" void __fastcall Ext407430(void* self);
extern "C" void __fastcall Ext40d340(void* self);
extern "C" void* __fastcall Ext410510(void* self);
extern "C" void* __fastcall Ext427bb0(void* self);
extern "C" void* __fastcall Ext427cc0(void* self);
extern "C" void* __fastcall Ext4283f0(void* self);

/* __cdecl. */
extern "C" void* __cdecl Ext423cd0(void);
extern "C" void* __cdecl Ext43e2cf(uint32_t cb);
extern "C" void* __cdecl Ext43f978(const char* psz, const char* hits, int32_t cch);

#endif /* FLYINGDODGEM_CALLEES_H */
