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
    void F401980(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
    void F401c00(void* a1, void* a2);
    void F402630(void* a1, void* a2, void* a3);
    int32_t F401520(void* a1, void* a2);
    int32_t F404030(void* a1, void* a2, void* a3);
    int32_t M402e00(void* a1);
    int32_t M4053a0(void* a1, void* a2);
    int32_t M405a10(void* a1, void* a2);
    int32_t M405b90(void* a1);
    int32_t M405ce0(void* a1, void* a2, int32_t a3, int16_t a4);
    void M405fa0(int16_t a1);
    int32_t M406090(void* a1, void* a2);
    int32_t M40cce0(void* a1, void* a2, void* a3, void* a4);
    int32_t M413c10(void* a1);
    int32_t M4184b0(void* a1, void* a2);
    int32_t M427a80(void* a1);
    int32_t M449598(int32_t a1, int32_t a2);
    int32_t M44994f(void* a1);
    int32_t M44a003(void* a1, void* a2, void* a3);
};

/* Render-node discriminated by RTTI IsKindOf (retail 0x44e710). Shared by
 * render.c (F404bb0/F4055b0/CopyPos/GetPair) and scene.c (F401c00
 * dispatcher) so both TUs resolve the same `RenderNode::IsKindOf` symbol
 * and reccmp sees one call target. Signatures match render.c exactly. */
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
    int32_t IsKindOf(void* pClass);
    IntPair CopyPos();
    PtPair* GetPair(PtPair* out);
    void F404bb0(void* a0, void* a1, int32_t a2);
    void F4055b0(void* a0, int32_t a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7);
};

/* Column-plane object (retail 0x401160/0x401210, normalized by F4010c0).
 * Size is 0x48: field spacing between the two globals g_4a4498/g_4a44e0.
 * Offsets +0x14/+0x18/+0x1c/+0x20 are the endpoint pair F4010c0 reads. */
class MerlinPlane
{
public:
    MerlinPlane();
    ~MerlinPlane();
    uint8_t m_pad00[0x14];
    int32_t m_p14;
    int32_t m_p18;
    int32_t m_p1c;
    int32_t m_p20;
    int16_t m_w24;
    int16_t m_pad26;
    int32_t m_p28;
    int32_t m_p2c;
    int32_t m_p30;
    int32_t m_p34;
    int32_t m_p38;
    int32_t m_p3c;
    int32_t m_p40;
    int32_t m_p44;
};

/* Span-bucket arena at [hover+0] (retail 0x406d00/0x403a60 take it in ECX).
 * Both are __thiscall with stack args, so they are members here. */
class SpanChain
{
public:
    void AddPair6d00(int32_t a1, int32_t a2);
    void F402b10(void* a1, int32_t a2, int32_t a3, int32_t a4);
    void F403a60(void* a1);
};

/* __thiscall, `this` in ECX only (spelled __fastcall on a free function). */
extern "C" void __fastcall F4010c0(void* self);
extern "C" void __fastcall F4030f0(void* self);

/* Span-tree insert (retail 0x406a80, __cdecl, single node argument). */
extern "C" void __cdecl F406a80(void* arg);

/* Integer atan2(y,x) -> 25-bit angle (retail 0x4385f0, __cdecl, caller pops). */
extern "C" int32_t __cdecl Atan4385f0(int32_t y, int32_t x);

/* Solid span-row filler (retail 0x406740, __fastcall self+unused+2 stack). */
extern "C" void __fastcall F406740(void* self, void* unused, int32_t a0, int32_t a1);

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
