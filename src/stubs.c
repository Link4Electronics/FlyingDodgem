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

// FUNCTION: HOVER 0x0041157A
s32 __fastcall Zero1157A(void* self, void* unused, s32 a, s32 b, s32 c, s32 d)
{
    return 0;
}

extern uint32_t g_data_bb670;

// FUNCTION: HOVER 0x00401000
void __fastcall SetTable401000(void* self)
{
    *(uint32_t*)self = (uint32_t)(uintptr_t)&g_data_bb670;
}

// FUNCTION: HOVER 0x0040bc70
void* __fastcall ClearSub40bc70(void* self)
{
    uint32_t* s = *(uint32_t**)((uint8_t*)self + 0x88);
    s[0x81] = 0;
    s[0x86] = 0;
    s[0x85] = 0;
    return s;
}

/* __thiscall leaf methods whose only stack argument follows the implicit
 * "this" (ECX). The extra void* second fastcall argument occupies EDX and goes
 * unused, so the real stack argument lands at [esp+4] exactly like retail. */

typedef struct { uint32_t x, y; } PtPair;

typedef struct { uint8_t pad[0x44]; PtPair p; } Self38a0;

// FUNCTION: HOVER 0x00459d94
void* __fastcall GetField459d94(void* self)
{
    return *(void**)((uint8_t*)self + 0x94);
}

// FUNCTION: HOVER 0x00401140
extern void* g_46058c;
void* __cdecl GetSub401140(void)
{
    void* obj;
    if (g_46058c == NULL)
        return NULL;
    obj = g_46058c;
    return GetField459d94(obj);
}

// FUNCTION: HOVER 0x004038a0
void __fastcall GetPair38a0(Self38a0* self, void* unused, PtPair* out)
{
    *out = self->p;
}

// FUNCTION: HOVER 0x00408530
extern uint32_t g_4c52d8;
#pragma optimize("s", on)
void __fastcall Register8530(void* self)
{
    if (*(uint32_t*)((uint8_t*)self + 0x68) == 0)
    {
        *(uint32_t*)((uint8_t*)self + 0x68) = 1;
        *(uint32_t*)((uint8_t*)self + 0x70) = g_4c52d8;
        g_4c52d8 = (uint32_t)(uintptr_t)self;
    }
}
#pragma optimize("", on)

// FUNCTION: HOVER 0x00413ed0
void __fastcall Set13ed0(void* self, void* unused, void* arg)
{
    uint8_t* s;
    uint32_t tail;
    if (arg != NULL)
        *(void**)((uint8_t*)self + 0x84) = arg;
    s = *(uint8_t**)((uint8_t*)self + 0x84);
    tail = *(uint32_t*)(s + 8);
    *(uint32_t*)((uint8_t*)self + 0x1d0) = *(uint32_t*)(s + 0);
    *(uint32_t*)((uint8_t*)self + 0x1d4) = tail;
    *(uint32_t*)((uint8_t*)self + 0x1d8) = tail;
}

// FUNCTION: HOVER 0x0040bc50
void __fastcall Touch40bc50(void* self)
{
    uint32_t* a = *(uint32_t**)((uint8_t*)self + 0x88);
    *(uint32_t*)((uint8_t*)self + 0x8c) = 0xc8;
    *(uint32_t*)((uint8_t*)a + 0x204) = 1;
}

// FUNCTION: HOVER 0x00411d60
void __fastcall SetFour11d60(void* self, void* unused, s32 a, s32 b, s32 c, s32 d)
{
    *(s32*)((uint8_t*)self + 0x10) = b;
    *(s32*)((uint8_t*)self + 0x18) = c;
    *(s32*)((uint8_t*)self + 0x14) = d;
    *(s32*)((uint8_t*)self + 0x1c) = a;
}

// FUNCTION: HOVER 0x00414a50
s32 __fastcall SetThree14a50(void* self, void* unused, s32 v)
{
    *(s32*)((uint8_t*)self + 0x78) = v;
    *(s32*)((uint8_t*)self + 0x80) = 0;
    *(s32*)((uint8_t*)self + 0x7c) = 0;
    return 1;
}

// FUNCTION: HOVER 0x00414A62
s32 __fastcall Imm14A62(void* self, void* unused, s32 a)
{
    return (s32)0x1;
}

// FUNCTION: HOVER 0x00419940
void __fastcall SetTwo19940(void* self, void* unused, u32 a, u32 b)
{
    *(u32*)((uint8_t*)self + 0x74) = a;
    *(u32*)((uint8_t*)self + 0x70) = b;
}

// FUNCTION: HOVER 0x00422c80
void* __fastcall Init22c80(void* self)
{
    *(uint32_t*)((uint8_t*)self + 0x00) = 0;
    *(uint32_t*)((uint8_t*)self + 0x04) = 0;
    *(uint32_t*)((uint8_t*)self + 0x08) = 10;
    *(uint32_t*)((uint8_t*)self + 0x0c) = 10;
    *(uint32_t*)((uint8_t*)self + 0x10) = 0;
    *(uint32_t*)((uint8_t*)self + 0x18) = 0;
    *(uint32_t*)((uint8_t*)self + 0x14) = 0;
    *(uint32_t*)((uint8_t*)self + 0x2c) = 0;
    *(uint32_t*)((uint8_t*)self + 0x1c) = 1;
    *(uint32_t*)((uint8_t*)self + 0x24) = 0;
    return self;
}

// FUNCTION: HOVER 0x00422CA8
void* __fastcall Self22CA8(void* self)
{
    return self;
}

extern uint32_t g_data_4ca8;

// FUNCTION: HOVER 0x00429340
void __cdecl Toggle429340(void)
{
    g_data_4ca8 = (uint32_t)(g_data_4ca8 < 1u);
}

#if !defined(FLYINGDODGEM_GDI_COMPAT)
/* WinHelpA / joy.cpl thunks from the retail binary. The 0xc variants forward
 * the dereferenced member at arg2+0x0c as the window handle and return the
 * WinHelpA result; the 0xa variants pass arg1 as the handle and return 1. */
#if defined(_MSC_VER)
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "kernel32.lib")
#endif

typedef void* HWND;
typedef int32_t BOOL;
typedef uint32_t DWORD;
typedef uint32_t UINT;
typedef void* HMODULE;
typedef void* FARPROC;

#if defined(_MSC_VER)
__declspec(dllimport)
#endif
BOOL __stdcall WinHelpA(HWND hWndMain, const char* lpszHelp, UINT uCommand, DWORD dwData);

#if defined(_MSC_VER)
__declspec(dllimport)
#endif
HMODULE __stdcall LoadLibraryA(const char* lpFileName);

#if defined(_MSC_VER)
__declspec(dllimport)
#endif
FARPROC __stdcall GetProcAddress(HMODULE hModule, const char* lpProcName);

extern uint8_t g_data_4880[1];
extern uint8_t g_data_d5e8[1];
extern uint8_t g_data_dbb8[1];
extern uint8_t g_data_dfe0[1];
extern uint8_t g_data_ecd0[1];
extern uint8_t g_data_f1e0[1];
extern uint8_t g_data_f8b8[1];
extern uint8_t g_data_5764[1];
extern uint8_t g_data_5758[1];

// FUNCTION: HOVER 0x0042d140
int32_t __stdcall WinHelp42d140(uint32_t arg1, uint32_t arg2)
{
    return WinHelpA((HWND)*(uint32_t*)((uint8_t*)arg2 + 0xc), (const char*)&g_data_4880, 0xc, (DWORD)(uintptr_t)&g_data_d5e8);
}

// FUNCTION: HOVER 0x0042d160
int32_t __stdcall WinHelp42d160(uint32_t arg1, uint32_t arg2)
{
    WinHelpA((HWND)(uintptr_t)arg1, (const char*)&g_data_4880, 0xa, (DWORD)(uintptr_t)&g_data_d5e8);
    return 1;
}

// FUNCTION: HOVER 0x0042D177
s32 __fastcall Imm2D177(void* self, void* unused, s32 a, s32 b)
{
    return (s32)0x1;
}

// FUNCTION: HOVER 0x0042e4f0
int32_t __stdcall WinHelp42e4f0(uint32_t arg1, uint32_t arg2)
{
    return WinHelpA((HWND)*(uint32_t*)((uint8_t*)arg2 + 0xc), (const char*)&g_data_4880, 0xc, (DWORD)(uintptr_t)&g_data_dbb8);
}

// FUNCTION: HOVER 0x0042e510
int32_t __stdcall WinHelp42e510(uint32_t arg1, uint32_t arg2)
{
    WinHelpA((HWND)(uintptr_t)arg1, (const char*)&g_data_4880, 0xa, (DWORD)(uintptr_t)&g_data_dbb8);
    return 1;
}

// FUNCTION: HOVER 0x0042E527
s32 __fastcall Imm2E527(void* self, void* unused, s32 a, s32 b)
{
    return (s32)0x1;
}

// FUNCTION: HOVER 0x0042eaa0
int32_t __stdcall WinHelp42eaa0(uint32_t arg1, uint32_t arg2)
{
    return WinHelpA((HWND)*(uint32_t*)((uint8_t*)arg2 + 0xc), (const char*)&g_data_4880, 0xc, (DWORD)(uintptr_t)&g_data_dfe0);
}

// FUNCTION: HOVER 0x0042eac0
int32_t __stdcall WinHelp42eac0(uint32_t arg1, uint32_t arg2)
{
    WinHelpA((HWND)(uintptr_t)arg1, (const char*)&g_data_4880, 0xa, (DWORD)(uintptr_t)&g_data_dfe0);
    return 1;
}

// FUNCTION: HOVER 0x0042EAD7
s32 __fastcall Imm2EAD7(void* self, void* unused, s32 a, s32 b)
{
    return (s32)0x1;
}

// FUNCTION: HOVER 0x0042f850
int32_t __stdcall WinHelp42f850(uint32_t arg1, uint32_t arg2)
{
    return WinHelpA((HWND)*(uint32_t*)((uint8_t*)arg2 + 0xc), (const char*)&g_data_4880, 0xc, (DWORD)(uintptr_t)&g_data_ecd0);
}

// FUNCTION: HOVER 0x0042f870
int32_t __stdcall WinHelp42f870(uint32_t arg1, uint32_t arg2)
{
    WinHelpA((HWND)(uintptr_t)arg1, (const char*)&g_data_4880, 0xa, (DWORD)(uintptr_t)&g_data_ecd0);
    return 1;
}

// FUNCTION: HOVER 0x0042F887
s32 __fastcall Imm2F887(void* self, void* unused, s32 a, s32 b)
{
    return (s32)0x1;
}

// FUNCTION: HOVER 0x00430610
int32_t __stdcall WinHelp430610(uint32_t arg1, uint32_t arg2)
{
    return WinHelpA((HWND)*(uint32_t*)((uint8_t*)arg2 + 0xc), (const char*)&g_data_4880, 0xc, (DWORD)(uintptr_t)&g_data_f1e0);
}

// FUNCTION: HOVER 0x00430630
int32_t __stdcall WinHelp430630(uint32_t arg1, uint32_t arg2)
{
    WinHelpA((HWND)(uintptr_t)arg1, (const char*)&g_data_4880, 0xa, (DWORD)(uintptr_t)&g_data_f1e0);
    return 1;
}

// FUNCTION: HOVER 0x00430647
s32 __fastcall Imm30647(void* self, void* unused, s32 a, s32 b)
{
    return (s32)0x1;
}

// FUNCTION: HOVER 0x00434010
int32_t __stdcall WinHelp434010(uint32_t arg1, uint32_t arg2)
{
    return WinHelpA((HWND)*(uint32_t*)((uint8_t*)arg2 + 0xc), (const char*)&g_data_4880, 0xc, (DWORD)(uintptr_t)&g_data_f8b8);
}

// FUNCTION: HOVER 0x00434030
int32_t __stdcall WinHelp434030(uint32_t arg1, uint32_t arg2)
{
    WinHelpA((HWND)(uintptr_t)arg1, (const char*)&g_data_4880, 0xa, (DWORD)(uintptr_t)&g_data_f8b8);
    return 1;
}

// FUNCTION: HOVER 0x00434047
s32 __fastcall Imm34047(void* self, void* unused, s32 a, s32 b)
{
    return (s32)0x1;
}

// FUNCTION: HOVER 0x0042e3f0
void __cdecl ShowJoyCpl42e3f0(void)
{
    HMODULE module = LoadLibraryA((const char*)&g_data_5764);
    if (module) {
        FARPROC proc = GetProcAddress(module, (const char*)&g_data_5758);
        if (proc)
            ((void(__stdcall*)(uint32_t))proc)(0);
    }
}
#endif
// FUNCTION: HOVER 0x0044afe6
uint32_t __fastcall Get44afe6(void* self)
{
    return *(uint32_t*)((uint8_t*)self + 0x38);
}

// FUNCTION: HOVER 0x0044b3b8
uint32_t __fastcall Get44b3b8(void* self)
{
    return *(uint32_t*)((uint8_t*)self + 0x1c);
}

// FUNCTION: HOVER 0x00452554
uint32_t __fastcall Get452554(void* self)
{
    return *(uint32_t*)((uint8_t*)self + 0x3c);
}

// FUNCTION: HOVER 0x004508f7
void __fastcall Set4508f7(void* self, void* unused, uint32_t v)
{
    *(uint32_t*)((uint8_t*)self + 0x8) = v;
}

// FUNCTION: HOVER 0x00450901
void __fastcall Set450901(void* self, void* unused, uint32_t v)
{
    *(uint32_t*)((uint8_t*)self + 0x4) = v;
}

// FUNCTION: HOVER 0x0045090b
void __fastcall Clear45090b(void* self)
{
    *(uint32_t*)((uint8_t*)self + 0x8) = 0;
}

// FUNCTION: HOVER 0x00450913
void __fastcall Clear450913(void* self)
{
    *(uint32_t*)((uint8_t*)self + 0x4) = 0;
}

// FUNCTION: HOVER 0x00453abd
void __fastcall Write453abd(void* self, void* unused, void* out)
{
    *(void**)((uint8_t*)out + 0x34) = self;
}

// FUNCTION: HOVER 0x00453ac7
void __fastcall Zap453ac7(void* self, void* unused, void* out)
{
    *(void**)((uint8_t*)out + 0x34) = NULL;
}

// FUNCTION: HOVER 0x004551ad
int32_t __fastcall Set4551ad(void* self, void* unused, uint32_t v, void* unused2)
{
    *(uint32_t*)((uint8_t*)self + 0x68) = v;
    return 0;
}

// FUNCTION: HOVER 0x004551B4
s32 __fastcall Zero551B4(void* self, void* unused, s32 a, s32 b)
{
    return 0;
}

// FUNCTION: HOVER 0x004551b9
uint32_t __fastcall Get4551b9(void* self, void* unused, void* a, void* b)
{
    return *(uint32_t*)((uint8_t*)self + 0x68);
}

// FUNCTION: HOVER 0x0045a9c7
void* __fastcall Id45a9c7(void* self)
{
    return self;
}

// FUNCTION: HOVER 0x0044d6be
void __fastcall Set44d6be(void* self, void* unused, uint32_t v)
{
    *(uint32_t*)((uint8_t*)self + 0x18) = 1;
    *(uint32_t*)((uint8_t*)self + 0x28) = v;
}

// FUNCTION: HOVER 0x004438bc
PtPair __fastcall GetPair4438bc(void* self)
{
    return *(PtPair*)((uint8_t*)self + 0);
}

// FUNCTION: HOVER 0x0042af89
extern void* g_4c5330;
void __fastcall Set42af89(void* v)
{
    g_4c5330 = v;
}

// FUNCTION: HOVER 0x0042b7d6
extern void* g_4c53e0;
void __fastcall Set42b7d6(void* v)
{
    g_4c53e0 = v;
}

// FUNCTION: HOVER 0x0042c3a2
extern void* g_4c54d0;
void __fastcall Set42c3a2(void* v)
{
    g_4c54d0 = v;
}

// FUNCTION: HOVER 0x00432f26
extern void* g_4a48c8;
void __fastcall Set432f26(void* v)
{
    g_4a48c8 = v;
}

// FUNCTION: HOVER 0x0045b1fa
extern void* g_4b6ff0;
void __stdcall Set45b1fa(void* v)
{
    g_4b6ff0 = v;
}

// FUNCTION: HOVER 0x0045b206
extern void* g_4b6ff0;
void* __cdecl Get45b206(void)
{
    return g_4b6ff0;
}

// FUNCTION: HOVER 0x00440221
extern void* g_4c79b0;
void* __cdecl Xchg440221(void* v)
{
    void* old;
    old = g_4c79b0;
    g_4c79b0 = v;
    return old;
}

// FUNCTION: HOVER 0x00442ada
extern uint32_t g_4c844c;
extern uint32_t g_4c8450;
void __cdecl Init442ada(void)
{
    g_4c844c = 0x4000;
    g_4c8450 = 0x1000000;
}

// FUNCTION: HOVER 0x00456e9a
int32_t __cdecl One456e9a(void)
{
    return 1;
}

// FUNCTION: HOVER 0x00457c9d
int32_t __cdecl One457c9d(void)
{
    return 1;
}

// FUNCTION: HOVER 0x0043b836
uint32_t __cdecl Const43b836(void)
{
    return 0x200;
}

// FUNCTION: HOVER 0x0043b843
uint32_t __cdecl Const43b843(void)
{
    return 0x2000;
}

// FUNCTION: HOVER 0x0043b858
uint32_t __cdecl Const43b858(void)
{
    return 0x200;
}

// FUNCTION: HOVER 0x0043b865
uint32_t __cdecl Const43b865(void)
{
    return 0x2000;
}
// FUNCTION: HOVER 0x0041B866
int32_t __fastcall Zac1B866(void* self)
{
    return 0;
}
// FUNCTION: HOVER 0x00446D04
int32_t __fastcall Zac46D04(void* self)
{
    return 0;
}
// FUNCTION: HOVER 0x00449006
int32_t __fastcall Zac49006(void* self, void* unused, void* a0)
{
    return 0;
}
// FUNCTION: HOVER 0x0044900B
int32_t __fastcall Zac4900B(void* self)
{
    return 0;
}
// FUNCTION: HOVER 0x0044AEF3
int32_t __fastcall Zac4AEF3(void* self)
{
    return 0;
}
// FUNCTION: HOVER 0x0044B727
int32_t __fastcall Zac4B727(void* self, void* unused, void* a0)
{
    return 0;
}
// FUNCTION: HOVER 0x0044CC74
int32_t __fastcall Zac4CC74(void* self, void* unused, void* a0)
{
    return 0;
}
// FUNCTION: HOVER 0x0044DA6C
int32_t __fastcall Zac4DA6C(void* self, void* unused, void* a0)
{
    return 0;
}
// FUNCTION: HOVER 0x0044DE19
int32_t __fastcall Zac4DE19(void* self, void* unused, void* a0, void* a1, void* a2, void* a3)
{
    return 0;
}
// FUNCTION: HOVER 0x0044E594
int32_t __fastcall Zac4E594(void* self)
{
    return 0;
}
// FUNCTION: HOVER 0x0044F42B
int32_t __fastcall Zac4F42B(void* self, void* unused, void* a0, void* a1, void* a2, void* a3)
{
    return 0;
}
// FUNCTION: HOVER 0x00451D70
int32_t __fastcall Zac51D70(void* self)
{
    return 0;
}
// FUNCTION: HOVER 0x00452FEB
int32_t __fastcall Zac52FEB(void* self, void* unused, void* a0)
{
    return 0;
}
// FUNCTION: HOVER 0x0045308A
int32_t __fastcall Zac5308A(void* self, void* unused, void* a0, void* a1, void* a2)
{
    return 0;
}
// FUNCTION: HOVER 0x0045308F
int32_t __fastcall Zac5308F(void* self, void* unused, void* a0, void* a1, void* a2)
{
    return 0;
}
// FUNCTION: HOVER 0x00453094
int32_t __fastcall Zac53094(void* self, void* unused, void* a0, void* a1, void* a2, void* a3)
{
    return 0;
}
// FUNCTION: HOVER 0x00453099
int32_t __fastcall Zac53099(void* self, void* unused, void* a0, void* a1, void* a2, void* a3)
{
    return 0;
}
// FUNCTION: HOVER 0x0045309E
int32_t __fastcall Zac5309E(void* self, void* unused, void* a0, void* a1, void* a2, void* a3)
{
    return 0;
}
// FUNCTION: HOVER 0x00456774
int32_t __fastcall Zac56774(void* self)
{
    return 0;
}
// FUNCTION: HOVER 0x004589CD
int32_t __fastcall Zac589CD(void* self, void* unused, void* a0)
{
    return 0;
}
// FUNCTION: HOVER 0x00420F50
void* __fastcall Idt20F50(void* self)
{
    return self;
}
// FUNCTION: HOVER 0x0040C20E
int32_t __fastcall Con0C20E(void* self, void* unused, void* a0)
{
    return 0x1;
}
// FUNCTION: HOVER 0x0044E0C7
int32_t __fastcall Con4E0C7(void* self, void* unused, void* a0, void* a1)
{
    return 0xffff;
}
// FUNCTION: HOVER 0x0045326E
int32_t __fastcall Con5326E(void* self, void* unused, void* a0)
{
    return 0x1;
}
// FUNCTION: HOVER 0x0040BC40
extern uint8_t g_data_5790[1];
void* __fastcall Get0BC40(void* self)
{
    return g_data_5790;
}
// FUNCTION: HOVER 0x004298F0
extern uint8_t g_data_51e0[1];
void* __fastcall Get298F0(void* self)
{
    return g_data_51e0;
}
// FUNCTION: HOVER 0x0042AD00
extern uint8_t g_data_52c0[1];
void* __fastcall Get2AD00(void* self)
{
    return g_data_52c0;
}
// FUNCTION: HOVER 0x0042AEC0
extern uint8_t g_data_5318[1];
void* __fastcall Get2AEC0(void* self)
{
    return g_data_5318;
}
// FUNCTION: HOVER 0x0044A64E
extern uint8_t g_data_fee0[1];
void* __fastcall Get4A64E(void* self)
{
    return g_data_fee0;
}
// FUNCTION: HOVER 0x0044AAA5
extern uint8_t g_data_0320[1];
void* __fastcall Get4AAA5(void* self)
{
    return g_data_0320;
}
// FUNCTION: HOVER 0x0044B703
extern uint8_t g_data_05e0[1];
void* __fastcall Get4B703(void* self)
{
    return g_data_05e0;
}
// FUNCTION: HOVER 0x0044B709
extern uint8_t g_data_0600[1];
void* __fastcall Get4B709(void* self)
{
    return g_data_0600;
}
// FUNCTION: HOVER 0x0044B70F
extern uint8_t g_data_0630[1];
void* __fastcall Get4B70F(void* self)
{
    return g_data_0630;
}
// FUNCTION: HOVER 0x0044D041
extern uint8_t g_data_08b8[1];
void* __fastcall Get4D041(void* self)
{
    return g_data_08b8;
}
// FUNCTION: HOVER 0x0044E70A
extern uint8_t g_data_6a20[1];
void* __fastcall Get4E70A(void* self)
{
    return g_data_6a20;
}
// FUNCTION: HOVER 0x0045158A
extern uint8_t g_data_1d60[1];
void* __fastcall Get5158A(void* self)
{
    return g_data_1d60;
}
// FUNCTION: HOVER 0x00452CF3
extern uint8_t g_data_2380[1];
void* __fastcall Get52CF3(void* self)
{
    return g_data_2380;
}
// FUNCTION: HOVER 0x00453D3C
extern uint8_t g_data_00c0[1];
void* __fastcall Get53D3C(void* self)
{
    return g_data_00c0;
}
// FUNCTION: HOVER 0x004550AF
extern uint8_t g_data_1b00[1];
void* __fastcall Get550AF(void* self)
{
    return g_data_1b00;
}
// FUNCTION: HOVER 0x00455A66
extern uint8_t g_data_2c48[1];
void* __fastcall Get55A66(void* self)
{
    return g_data_2c48;
}
// FUNCTION: HOVER 0x00456670
extern uint8_t g_data_2cb0[1];
void* __fastcall Get56670(void* self)
{
    return g_data_2cb0;
}
// FUNCTION: HOVER 0x00458631
extern uint8_t g_data_31b0[1];
void* __fastcall Get58631(void* self)
{
    return g_data_31b0;
}
// FUNCTION: HOVER 0x00458AE0
extern uint8_t g_data_15a8[1];
void* __fastcall Get58AE0(void* self)
{
    return g_data_15a8;
}
// FUNCTION: HOVER 0x0045B43E
extern uint8_t g_data_34d0[1];
void* __fastcall Get5B43E(void* self)
{
    return g_data_34d0;
}
// FUNCTION: HOVER 0x0045C8E4
extern uint8_t g_data_68b8[1];
void* __fastcall Get5C8E4(void* self)
{
    return g_data_68b8;
}
// FUNCTION: HOVER 0x004179d2
extern uint8_t g_data_79d8[1];
void* __cdecl Get4179d2(void)
{
    return g_data_79d8;
}
// FUNCTION: HOVER 0x0045CB8C
extern uint8_t g_data_68e0[1];
void* __fastcall Get5CB8C(void* self)
{
    return g_data_68e0;
}

// FUNCTION: HOVER 0x0045CCE9
extern uint8_t g_data_6908[1];
void* __fastcall Get5CCE9(void* self)
{
    return g_data_6908;
}

// FUNCTION: HOVER 0x0045CDFD
extern uint8_t g_data_6938[1];
void* __fastcall Get5CDFD(void* self)
{
    return g_data_6938;
}

// FUNCTION: HOVER 0x0045CE36
extern uint8_t g_data_6960[1];
void* __fastcall Get5CE36(void* self)
{
    return g_data_6960;
}

// FUNCTION: HOVER 0x0045D32E
extern uint8_t g_data_6998[1];
void* __fastcall Get5D32E(void* self)
{
    return g_data_6998;
}

// FUNCTION: HOVER 0x0045D455
extern uint8_t g_data_6a00[1];
void* __fastcall Get5D455(void* self)
{
    return g_data_6a00;
}

// FUNCTION: HOVER 0x0045D470
extern uint8_t g_data_6a40[1];
void* __fastcall Get5D470(void* self)
{
    return g_data_6a40;
}

// FUNCTION: HOVER 0x0045D565
extern uint8_t g_data_6a70[1];
void* __fastcall Get5D565(void* self)
{
    return g_data_6a70;
}

// FUNCTION: HOVER 0x0045D57B
extern uint8_t g_data_6a88[1];
void* __fastcall Get5D57B(void* self)
{
    return g_data_6a88;
}

// FUNCTION: HOVER 0x0045D591
extern uint8_t g_data_6aa0[1];
void* __fastcall Get5D591(void* self)
{
    return g_data_6aa0;
}

// FUNCTION: HOVER 0x0045D5A7
extern uint8_t g_data_6ab8[1];
void* __fastcall Get5D5A7(void* self)
{
    return g_data_6ab8;
}

// FUNCTION: HOVER 0x0045D621
extern uint8_t g_data_6ad0[1];
void* __fastcall Get5D621(void* self)
{
    return g_data_6ad0;
}

// FUNCTION: HOVER 0x0045D637
extern uint8_t g_data_6ae8[1];
void* __fastcall Get5D637(void* self)
{
    return g_data_6ae8;
}

// FUNCTION: HOVER 0x0045D64D
extern uint8_t g_data_6b00[1];
void* __fastcall Get5D64D(void* self)
{
    return g_data_6b00;
}

// FUNCTION: HOVER 0x0045D663
extern uint8_t g_data_6b18[1];
void* __fastcall Get5D663(void* self)
{
    return g_data_6b18;
}

// FUNCTION: HOVER 0x0045D679
extern uint8_t g_data_6b30[1];
void* __fastcall Get5D679(void* self)
{
    return g_data_6b30;
}

// FUNCTION: HOVER 0x0045D68F
extern uint8_t g_data_6b48[1];
void* __fastcall Get5D68F(void* self)
{
    return g_data_6b48;
}

// FUNCTION: HOVER 0x0045D6A5
extern uint8_t g_data_6b60[1];
void* __fastcall Get5D6A5(void* self)
{
    return g_data_6b60;
}

// FUNCTION: HOVER 0x0045D92E
extern uint8_t g_data_6d48[1];
void* __fastcall Get5D92E(void* self)
{
    return g_data_6d48;
}

// FUNCTION: HOVER 0x0045D9AF
extern uint8_t g_data_6de0[1];
void* __fastcall Get5D9AF(void* self)
{
    return g_data_6de0;
}

// FUNCTION: HOVER 0x0045D9C5
extern uint8_t g_data_6df8[1];
void* __fastcall Get5D9C5(void* self)
{
    return g_data_6df8;
}

// FUNCTION: HOVER 0x0045DC12
extern uint8_t g_data_6f38[1];
void* __fastcall Get5DC12(void* self)
{
    return g_data_6f38;
}

// FUNCTION: HOVER 0x0045DC93
extern uint8_t g_data_6fc8[1];
void* __fastcall Get5DC93(void* self)
{
    return g_data_6fc8;
}

// FUNCTION: HOVER 0x0045DCA9
extern uint8_t g_data_6fe0[1];
void* __fastcall Get5DCA9(void* self)
{
    return g_data_6fe0;
}

// FUNCTION: HOVER 0x0045DCCF
extern uint8_t g_data_7010[1];
void* __fastcall Get5DCCF(void* self)
{
    return g_data_7010;
}

// FUNCTION: HOVER 0x0045DCE5
extern uint8_t g_data_7028[1];
void* __fastcall Get5DCE5(void* self)
{
    return g_data_7028;
}

// FUNCTION: HOVER 0x0045DD39
extern uint8_t g_data_7098[1];
void* __fastcall Get5DD39(void* self)
{
    return g_data_7098;
}

// FUNCTION: HOVER 0x0045DD4F
extern uint8_t g_data_70b0[1];
void* __fastcall Get5DD4F(void* self)
{
    return g_data_70b0;
}

// FUNCTION: HOVER 0x0045DD65
extern uint8_t g_data_70c8[1];
void* __fastcall Get5DD65(void* self)
{
    return g_data_70c8;
}

// FUNCTION: HOVER 0x0045E1AE
extern uint8_t g_data_7118[1];
void* __fastcall Get5E1AE(void* self)
{
    return g_data_7118;
}

// FUNCTION: HOVER 0x0045E1E7
extern uint8_t g_data_7150[1];
void* __fastcall Get5E1E7(void* self)
{
    return g_data_7150;
}

// FUNCTION: HOVER 0x0045E232
extern uint8_t g_data_7180[1];
void* __fastcall Get5E232(void* self)
{
    return g_data_7180;
}

// FUNCTION: HOVER 0x0045E27D
extern uint8_t g_data_71a0[1];
void* __fastcall Get5E27D(void* self)
{
    return g_data_71a0;
}

// FUNCTION: HOVER 0x0045E2B6
extern uint8_t g_data_71c8[1];
void* __fastcall Get5E2B6(void* self)
{
    return g_data_71c8;
}

// FUNCTION: HOVER 0x0045E2FF
extern uint8_t g_data_71e8[1];
void* __fastcall Get5E2FF(void* self)
{
    return g_data_71e8;
}

// FUNCTION: HOVER 0x0045E36B
extern uint8_t g_data_7238[1];
void* __fastcall Get5E36B(void* self)
{
    return g_data_7238;
}

// FUNCTION: HOVER 0x0045E3A4
extern uint8_t g_data_7268[1];
void* __fastcall Get5E3A4(void* self)
{
    return g_data_7268;
}

// FUNCTION: HOVER 0x0045E63E
extern uint8_t g_data_7298[1];
void* __fastcall Get5E63E(void* self)
{
    return g_data_7298;
}

// FUNCTION: HOVER 0x0045E91F
extern uint8_t g_data_72f0[1];
void* __fastcall Get5E91F(void* self)
{
    return g_data_72f0;
}

// FUNCTION: HOVER 0x0045EACA
extern uint8_t g_data_7330[1];
void* __fastcall Get5EACA(void* self)
{
    return g_data_7330;
}

// FUNCTION: HOVER 0x0045EB18
extern uint8_t g_data_73f8[1];
void* __fastcall Get5EB18(void* self)
{
    return g_data_73f8;
}

// FUNCTION: HOVER 0x0045EB3E
extern uint8_t g_data_7410[1];
void* __fastcall Get5EB3E(void* self)
{
    return g_data_7410;
}

// FUNCTION: HOVER 0x0045EC07
extern uint8_t g_data_7448[1];
void* __fastcall Get5EC07(void* self)
{
    return g_data_7448;
}

// FUNCTION: HOVER 0x0045EC6D
extern uint8_t g_data_97d8[1];
void* __fastcall Get5EC6D(void* self)
{
    return g_data_97d8;
}

// FUNCTION: HOVER 0x0045EC83
extern uint8_t g_data_97f0[1];
void* __fastcall Get5EC83(void* self)
{
    return g_data_97f0;
}

// FUNCTION: HOVER 0x0045ED49
extern uint8_t g_data_98f8[1];
void* __fastcall Get5ED49(void* self)
{
    return g_data_98f8;
}


// FUNCTION: HOVER 0x0040E719
s32 __fastcall Imm0E719(void* self, void* unused, s32 a)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x0040EE81
s32 __fastcall Imm0EE81(void* self)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x0040F90A
s32 __fastcall Imm0F90A(void* self, void* unused, s32 a)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x00412230
s32 __fastcall Imm12230(void* self)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x00413723
s32 __fastcall Imm13723(void* self)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x00413FD5
s32 __fastcall Imm13FD5(void* self)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x004177C3
void* __fastcall Imm177C3(void* self)
{
    extern uint8_t g_data_77c9[1];
    return g_data_77c9;
}
// FUNCTION: HOVER 0x004179F1
void* __fastcall Imm179F1(void* self)
{
    extern uint8_t g_data_79f7[1];
    return g_data_79f7;
}
// FUNCTION: HOVER 0x0041B860
s32 __fastcall Imm1B860(void* self)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x0041D3D1
void* __fastcall Imm1D3D1(void* self)
{
    extern uint8_t g_data_d3d7[1];
    return g_data_d3d7;
}
// FUNCTION: HOVER 0x0041F0AA
void* __fastcall Imm1F0AA(void* self)
{
    extern uint8_t g_data_f0b0[1];
    return g_data_f0b0;
}
// FUNCTION: HOVER 0x0041F315
void* __fastcall Imm1F315(void* self)
{
    extern uint8_t g_data_f31b[1];
    return g_data_f31b;
}
// FUNCTION: HOVER 0x00425EB3
s32 __fastcall Imm25EB3(void* self)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x0042AD28
s32 __fastcall Imm2AD28(void* self, void* unused, s32 a, s32 b)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x0042CB60
void* __fastcall Imm2CB60(void* self)
{
    extern uint8_t g_data_5608[1];
    return g_data_5608;
}
// FUNCTION: HOVER 0x0042E9F0
void* __fastcall Imm2E9F0(void* self)
{
    extern uint8_t g_data_df78[1];
    return g_data_df78;
}
// FUNCTION: HOVER 0x00439C13
s32 __fastcall Imm39C13(void* self)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x0043C414
s32 __fastcall Imm3C414(void* self, void* unused, s32 a, s32 b, s32 c, s32 d)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x0043DBCB
s32 __fastcall Imm3DBCB(void* self)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x0043E24C
s32 __fastcall Imm3E24C(void* self)
{
    return (s32)0x3;
}
// FUNCTION: HOVER 0x0043E2C9
s32 __fastcall Imm3E2C9(void* self)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x0043E601
void* __fastcall Imm3E601(void* self)
{
    extern uint8_t g_data_e607[1];
    return g_data_e607;
}
// FUNCTION: HOVER 0x0043E61C
void* __fastcall Imm3E61C(void* self)
{
    extern uint8_t g_data_e622[1];
    return g_data_e622;
}
// FUNCTION: HOVER 0x0043FB2C
void* __fastcall Imm3FB2C(void* self)
{
    extern uint8_t g_data_fb32[1];
    return g_data_fb32;
}
// FUNCTION: HOVER 0x0043FB47
void* __fastcall Imm3FB47(void* self)
{
    extern uint8_t g_data_fb4d[1];
    return g_data_fb4d;
}
// FUNCTION: HOVER 0x0044124D
s32 __fastcall Imm4124D(void* self)
{
    return (s32)0x404;
}
// FUNCTION: HOVER 0x0044196B
s32 __fastcall Imm4196B(void* self)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x00441D3D
s32 __fastcall Imm41D3D(void* self)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x00442986
s32 __fastcall Imm42986(void* self)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x004429F8
s32 __fastcall Imm429F8(void* self)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x00442B3F
s32 __fastcall Imm42B3F(void* self)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x00445A5A
s32 __fastcall Imm45A5A(void* self)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x0044A60E
void* __fastcall Imm4A60E(void* self)
{
    extern uint8_t g_data_a614[1];
    return g_data_a614;
}
// FUNCTION: HOVER 0x0044B721
s32 __fastcall Imm4B721(void* self)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x0044C333
void* __fastcall Imm4C333(void* self)
{
    extern uint8_t g_data_c339[1];
    return g_data_c339;
}
// FUNCTION: HOVER 0x0044C708
void* __fastcall Imm4C708(void* self)
{
    extern uint8_t g_data_c70e[1];
    return g_data_c70e;
}
// FUNCTION: HOVER 0x0044CA8C
s32 __fastcall Imm4CA8C(void* self, void* unused, s32 a)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x0044E150
void* __fastcall Imm4E150(void* self)
{
    extern uint8_t g_data_e172[1];
    return g_data_e172;
}
// FUNCTION: HOVER 0x0044E16C
void* __fastcall Imm4E16C(void* self)
{
    extern uint8_t g_data_e172[1];
    return g_data_e172;
}
// FUNCTION: HOVER 0x0044E597
s32 __fastcall Imm4E597(void* self)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x0044E7AC
void* __fastcall Imm4E7AC(void* self)
{
    extern uint8_t g_data_e7b2[1];
    return g_data_e7b2;
}
// FUNCTION: HOVER 0x0044EC87
s32 __fastcall Imm4EC87(void* self, void* unused, s32 a)
{
    return (s32)0x5;
}
// FUNCTION: HOVER 0x00451B78
void* __fastcall Imm51B78(void* self)
{
    extern uint8_t g_data_1b7e[1];
    return g_data_1b7e;
}
// FUNCTION: HOVER 0x00452166
void* __fastcall Imm52166(void* self)
{
    extern uint8_t g_data_216c[1];
    return g_data_216c;
}
// FUNCTION: HOVER 0x00452182
void* __fastcall Imm52182(void* self)
{
    extern uint8_t g_data_2188[1];
    return g_data_2188;
}
// FUNCTION: HOVER 0x004521A5
void* __fastcall Imm521A5(void* self)
{
    extern uint8_t g_data_21ab[1];
    return g_data_21ab;
}
// FUNCTION: HOVER 0x004523F5
void* __fastcall Imm523F5(void* self)
{
    extern uint8_t g_data_23fb[1];
    return g_data_23fb;
}
// FUNCTION: HOVER 0x00452411
void* __fastcall Imm52411(void* self)
{
    extern uint8_t g_data_2417[1];
    return g_data_2417;
}
// FUNCTION: HOVER 0x00452434
void* __fastcall Imm52434(void* self)
{
    extern uint8_t g_data_243a[1];
    return g_data_243a;
}
// FUNCTION: HOVER 0x00452DE1
s32 __fastcall Imm52DE1(void* self, void* unused, s32 a)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x004531C2
s32 __fastcall Imm531C2(void* self, void* unused, s32 a)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x0045323A
s32 __fastcall Imm5323A(void* self, void* unused, s32 a)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x004548C3
s32 __fastcall Imm548C3(void* self, void* unused, s32 a)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x00455DCB
s32 __fastcall Imm55DCB(void* self, void* unused, s32 a, s32 b)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x00457B04
void* __fastcall Imm57B04(void* self)
{
    extern uint8_t g_data_3118[1];
    return g_data_3118;
}
// FUNCTION: HOVER 0x00458961
s32 __fastcall Imm58961(void* self, void* unused, s32 a, s32 b, s32 c, s32 d)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x00459323
s32 __fastcall Imm59323(void* self, void* unused, s32 a)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x0045C7AF
void* __fastcall Imm5C7AF(void* self)
{
    extern uint8_t g_data_c7b5[1];
    return g_data_c7b5;
}
// FUNCTION: HOVER 0x0045C7BD
void* __fastcall Imm5C7BD(void* self)
{
    extern uint8_t g_data_c7c3[1];
    return g_data_c7c3;
}
// FUNCTION: HOVER 0x0045CA44
s32 __fastcall Imm5CA44(void* self)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x0045CA4A
s32 __fastcall Imm5CA4A(void* self)
{
    return (s32)0x1;
}
// FUNCTION: HOVER 0x0045D4E2
void* __fastcall Imm5D4E2(void* self)
{
    extern uint8_t g_data_6a58[1];
    return g_data_6a58;
}
// FUNCTION: HOVER 0x0045DA37
void* __fastcall Imm5DA37(void* self)
{
    extern uint8_t g_data_6e10[1];
    return g_data_6e10;
}
// FUNCTION: HOVER 0x0045E948
s32 __fastcall Imm5E948(void* self)
{
    return (s32)0x310;
}
// FUNCTION: HOVER 0x00401149
s32 __fastcall Zero01149(void* self)
{
    return 0;
}
// FUNCTION: HOVER 0x0040C209
s32 __fastcall Zero0C209(void* self, void* unused, s32 a)
{
    return 0;
}
// FUNCTION: HOVER 0x0041ECD1
s32 __fastcall Zero1ECD1(void* self)
{
    return 0;
}
// FUNCTION: HOVER 0x00425EB0
s32 __fastcall Zero25EB0(void* self)
{
    return 0;
}
// FUNCTION: HOVER 0x00426B8D
s32 __fastcall Zero26B8D(void* self, void* unused, s32 a, s32 b)
{
    return 0;
}
// FUNCTION: HOVER 0x00427D2A
s32 __fastcall Zero27D2A(void* self, void* unused, s32 a, s32 b)
{
    return 0;
}
// FUNCTION: HOVER 0x00427D3A
s32 __fastcall Zero27D3A(void* self, void* unused, s32 a, s32 b)
{
    return 0;
}
// FUNCTION: HOVER 0x00438CE8
s32 __fastcall Zero38CE8(void* self)
{
    return 0;
}
// FUNCTION: HOVER 0x00439BC2
s32 __fastcall Zero39BC2(void* self, void* unused, s32 a, s32 b, s32 c, s32 d)
{
    return 0;
}
// FUNCTION: HOVER 0x00439BE3
s32 __fastcall Zero39BE3(void* self, void* unused, s32 a, s32 b, s32 c, s32 d)
{
    return 0;
}
// FUNCTION: HOVER 0x0043C3D2
s32 __fastcall Zero3C3D2(void* self, void* unused, s32 a, s32 b, s32 c, s32 d)
{
    return 0;
}
// FUNCTION: HOVER 0x0043DBA9
s32 __fastcall Zero3DBA9(void* self)
{
    return 0;
}
// FUNCTION: HOVER 0x00441B70
s32 __fastcall Zero41B70(void* self)
{
    return 0;
}
// FUNCTION: HOVER 0x00442156
s32 __fastcall Zero42156(void* self)
{
    return 0;
}
// FUNCTION: HOVER 0x0044289C
s32 __fastcall Zero4289C(void* self)
{
    return 0;
}
// FUNCTION: HOVER 0x0044455C
s32 __fastcall Zero4455C(void* self)
{
    return 0;
}
// FUNCTION: HOVER 0x00449420
s32 __fastcall Zero49420(void* self, void* unused, s32 a, s32 b)
{
    return 0;
}
// FUNCTION: HOVER 0x004499F8
s32 __fastcall Zero499F8(void* self, void* unused, s32 a, s32 b)
{
    return 0;
}
// FUNCTION: HOVER 0x0044AA8D
s32 __fastcall Zero4AA8D(void* self, void* unused, s32 a, s32 b)
{
    return 0;
}
// FUNCTION: HOVER 0x0044AAA0
s32 __fastcall Zero4AAA0(void* self, void* unused, s32 a, s32 b)
{
    return 0;
}
// FUNCTION: HOVER 0x0044B194
s32 __fastcall Zero4B194(void* self, void* unused, s32 a, s32 b)
{
    return 0;
}
// FUNCTION: HOVER 0x0044E58F
s32 __fastcall Zero4E58F(void* self, void* unused, s32 a)
{
    return 0;
}
// FUNCTION: HOVER 0x0044E5BA
s32 __fastcall Zero4E5BA(void* self)
{
    return 0;
}
// FUNCTION: HOVER 0x0044E727
s32 __fastcall Zero4E727(void* self, void* unused, s32 a)
{
    return 0;
}
// FUNCTION: HOVER 0x0044E807
s32 __fastcall Zero4E807(void* self, void* unused, s32 a)
{
    return 0;
}
// FUNCTION: HOVER 0x0044E83B
s32 __fastcall Zero4E83B(void* self)
{
    return 0;
}
// FUNCTION: HOVER 0x00451D6D
s32 __fastcall Zero51D6D(void* self)
{
    return 0;
}
// FUNCTION: HOVER 0x00453380
s32 __fastcall Zero53380(void* self, void* unused, s32 a, s32 b, s32 c)
{
    return 0;
}
// FUNCTION: HOVER 0x00458E9B
s32 __fastcall Zero58E9B(void* self, void* unused, s32 a, s32 b)
{
    return 0;
}
// FUNCTION: HOVER 0x00459D8F
s32 __fastcall Zero59D8F(void* self, void* unused, s32 a, s32 b)
{
    return 0;
}
// FUNCTION: HOVER 0x0041503D
void* __fastcall Self1503D(void* self)
{
    return self;
}
// FUNCTION: HOVER 0x0044B74F
void* __fastcall Self4B74F(void* self)
{
    return self;
}
// FUNCTION: HOVER 0x0044CC70
void* __fastcall Ptr4CC70(void* self)
{
    return (uint8_t*)self + 0x24;
}
