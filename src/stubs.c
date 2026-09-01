#include "flyingdodgem_types.h"

/* Small leaf functions from the retail binary that compile down to a handful of
 * instructions. The original shipped as a C++ build; the C recompilation uses
 * __fastcall to reproduce the __thiscall "this" argument in ECX where the body
 * only touches a member of the implicit object. Each stub here compiles to the
 * exact same byte sequence as the retail function it replaces. */

// FUNCTION: HOVER 0x00425370
void __cdecl Dummy437570(void)
{
}

// FUNCTION: HOVER 0x004294f0
void __cdecl Dummy4294f0(void)
{
}

// FUNCTION: HOVER 0x0042b800
void __cdecl Dummy42b800(void)
{
}

// FUNCTION: HOVER 0x0042ecd0
void __cdecl Dummy42ecd0(void)
{
}

// FUNCTION: HOVER 0x0042ece0
void __cdecl Dummy42ece0(void)
{
}

// FUNCTION: HOVER 0x0042ecf0
void __cdecl Dummy42ecf0(void)
{
}

// FUNCTION: HOVER 0x0042ef30
void __cdecl Dummy42ef30(void)
{
}

// FUNCTION: HOVER 0x00425360
void __stdcall Dummy425360(void* arg)
{
}

// FUNCTION: HOVER 0x00425380
void __stdcall Dummy425380(void* arg)
{
}

// FUNCTION: HOVER 0x00426f10
void __stdcall Dummy426f10(void* arg)
{
}

// FUNCTION: HOVER 0x0042ad50
void __stdcall Dummy42ad50(void* arg)
{
}

// FUNCTION: HOVER 0x0042be40
void __stdcall Dummy42be40(void* arg)
{
}

// FUNCTION: HOVER 0x00433840
void __stdcall Dummy433840(void* arg)
{
}

// FUNCTION: HOVER 0x004342c0
void __stdcall Dummy4342c0(void* arg)
{
}

// FUNCTION: HOVER 0x0042ad40
void __stdcall Dummy42ad40(void* a, void* b)
{
}

// FUNCTION: HOVER 0x0042ad60
int32_t __stdcall Ret0_42ad60(void* arg)
{
    return 0;
}

// FUNCTION: HOVER 0x0042ad70
int32_t __stdcall Ret0_42ad70(void* a, void* b, void* c)
{
    return 0;
}

// FUNCTION: HOVER 0x00428d80
int32_t __stdcall Ret1_428d80(void* a, void* b)
{
    return 1;
}

// FUNCTION: HOVER 0x0042ad80
int32_t __stdcall Ret1_42ad80(void* arg)
{
    return 1;
}

// FUNCTION: HOVER 0x0042ad90
int32_t __stdcall Ret1_42ad90(void* arg)
{
    return 1;
}

// FUNCTION: HOVER 0x0042c3c0
int32_t __stdcall Ret1_42c3c0(void* arg)
{
    return 1;
}

// FUNCTION: HOVER 0x00419520
void* __fastcall This419520(void* self)
{
    return self;
}

// FUNCTION: HOVER 0x00419b20
void* __fastcall This419b20(void* self)
{
    return self;
}

// FUNCTION: HOVER 0x0041c010
void __fastcall SetC841c010(void* self)
{
    *(uint32_t*)((uint8_t*)self + 0x8c) = 0xc8;
}

// FUNCTION: HOVER 0x0041d6a0
void __fastcall ZeroWords41d6a0(void* self)
{
    uint16_t* w = (uint16_t*)self;
    w[1] = 0;
    w[0] = 0;
}

// FUNCTION: HOVER 0x00425ec0
void __fastcall Toggle425ec0(void* self)
{
    uint32_t* m = (uint32_t*)((uint8_t*)self + 0x3bc);
    *m = (uint32_t)(*m < 1u);
}

// FUNCTION: HOVER 0x00425f00
void __fastcall Toggle425f00(void* self)
{
    uint32_t* m = (uint32_t*)((uint8_t*)self + 0x1e0);
    *m = (uint32_t)(*m < 1u);
}

// FUNCTION: HOVER 0x004089e0
void* __fastcall Sub89e0(void* self)
{
    return (uint8_t*)self + 0x180;
}

// FUNCTION: HOVER 0x00411570
int32_t __stdcall ZeroOut411570(void* a, void* b, void* c, uint32_t* out)
{
    *out = 0;
    return 0;
}

extern uint32_t g_data_bb670;

// FUNCTION: HOVER 0x00401000
void __fastcall SetTable401000(void* self)
{
    *(uint32_t*)self = (uint32_t)(uintptr_t)&g_data_bb670;
}

extern uint32_t g_data_4ca8;

// FUNCTION: HOVER 0x00429340
void __cdecl Toggle429340(void)
{
    g_data_4ca8 = (uint32_t)(g_data_4ca8 < 1u);
}