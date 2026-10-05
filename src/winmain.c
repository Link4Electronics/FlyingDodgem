#include "flyingdodgem_types.h"

/* Retail startup entry (HOVER 0x45021e): MFC-style AfxWinMain, compiled from
 * the C++ side of the binary. The application object hangs off afxCurrentWinApp
 * (global 0x4b7cd8); the startup virtuals are dispatched through its vtable at
 * the slot offsets the original binary uses (InitApplication +0x64,
 * InitInstance +0x30, Run +0x34, ExitInstance +0x40). The frame pointer stays
 * on and size-opt keeps the argument pushes in the original push-memory form. */

typedef int (__fastcall *AppVFn)(void* self);
typedef void (__fastcall *WndVFn)(void* self);

/* One single-slot struct per vtable view: fixed-byte padding puts the slot at
 * the retail offset on any pointer width and lets the calls compile to the
 * original memory-indirect `call dword ptr [reg + off]` form. */
typedef struct { uint8_t pad[0x64]; AppVFn InitApplication; } AppVftInitApplication;
typedef struct { uint8_t pad[0x30]; AppVFn InitInstance; } AppVftInitInstance;
typedef struct { uint8_t pad[0x34]; AppVFn Run; } AppVftRun;
typedef struct { uint8_t pad[0x40]; AppVFn ExitInstance; } AppVftExitInstance;
typedef struct { uint8_t pad[0x34]; WndVFn WindowMethod; } WndVft;

extern "C" void* g_pApp;

extern "C" int __stdcall AfxWinInit(void* hInstance, void* hPrevInstance, char* lpCmdLine, int nCmdShow);
extern "C" void __stdcall AfxWinTerm(void);
extern "C" void* __cdecl Get45b206(void);
extern "C" void __stdcall AfxPostQuitMessage(int nExitCode);
extern "C" int __fastcall PumpMessageLoop(void* self);

#pragma optimize("s", on)
#pragma optimize("y", off)

// FUNCTION: HOVER 0x0045021e
extern "C" int __stdcall WinMain(void* hInstance, void* hPrevInstance, char* lpCmdLine, int nCmdShow)
{
    int nReturnCode = -1;
    void* pApp = g_pApp;
    void** vft;

    if (AfxWinInit(hInstance, hPrevInstance, lpCmdLine, nCmdShow))
    {
        vft = *(void***)pApp;
        if (((AppVftInitApplication*)vft)->InitApplication(pApp))
        {
            if (((AppVftInitInstance*)vft)->InitInstance(pApp) == 0)
            {
                if (*(void**)((uint8_t*)pApp + 0x1c) != NULL)
                    ((WndVft*)*(void**)*(void**)((uint8_t*)pApp + 0x1c))->WindowMethod(*(void**)((uint8_t*)pApp + 0x1c));
                nReturnCode = ((AppVftExitInstance*)vft)->ExitInstance(pApp);
            }
            else
            {
                nReturnCode = ((AppVftRun*)vft)->Run(pApp);
            }
        }
    }

    AfxWinTerm();
    return nReturnCode;
}

// FUNCTION: HOVER 0x0044aaab
extern "C" int __fastcall Run(void* self)
{
    if (*(uint32_t*)((uint8_t*)self + 0x1c) == 0 && Get45b206() != 0)
        AfxPostQuitMessage(0);
    return PumpMessageLoop(self);
}

#pragma optimize("", on)
