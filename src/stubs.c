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

// FUNCTION: HOVER 0x0040bc70
void* __fastcall ClearSub40bc70(void* self)
{
    uint32_t* s = *(uint32_t**)((uint8_t*)self + 0x88);
    s[0x81] = 0;
    s[0x86] = 0;
    s[0x85] = 0;
    return s;
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