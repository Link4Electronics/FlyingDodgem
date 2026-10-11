#include "flyingdodgem_types.h"
#include "callees.h"
#include "scene.h"

/* ---------------------------------------------------------------------------
 * Display entry (retail .text 0x41ece0-0x41f17c).  __thiscall on the display
 * object under SEH.  Three parts:
 *
 *   1. Tick init: stash GetTickCount() into this+0x528 / +0x52c, zero +0x530.
 *   2. Display bootstrap: LoadModule("DISPDIB.DLL"), resolve "DisplayDibWindow",
 *      GetWindowLong(GWL_HINSTANCE) fallback, GetSystemMetrics(SM_CYSCREEN /
 *      SM_CXSCREEN) sizing, CreateWindowExA into global g_460570+0x3f0.
 *   3. Frame body, guarded by this+0x530: a critical-section handshake over
 *      g_460570+0x11c and this+{0x48,0x4c,0x50,0x54} events, then a two-level
 *      switch on this+0xac.  Level one reads a scratch into per-case locals;
 *      level two calls the per-member.  Case 0 carries two dwords
 *      (this+0xa4/+0xa8); case 3 calls DrawEntry402250.
 *
 * On exit it resets the +0x3f0/+0x3e0 slots and re-signals the frame event.
 * ------------------------------------------------------------------------- */

#if defined(_MSC_VER)
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "kernel32.lib")
#define HOVER_IMPORT __declspec(dllimport)
#else
#define HOVER_IMPORT
#endif

extern "C" HOVER_IMPORT uint32_t __stdcall GetTickCount(void);
extern "C" HOVER_IMPORT int32_t __stdcall LoadModule(const char* name, void* param);
extern "C" HOVER_IMPORT int32_t __stdcall GetWindowLongA(void* hwnd, int32_t index);
extern "C" HOVER_IMPORT int32_t __stdcall GetSystemMetrics(int32_t index);
extern "C" HOVER_IMPORT void* __stdcall CreateWindowExA(uint32_t exStyle, const char* cls,
    const char* title, uint32_t style, int32_t x, int32_t y, int32_t w, int32_t h,
    void* parent, void* menu, void* instance, void* param);
extern "C" HOVER_IMPORT void __stdcall EnterCriticalSection(void* cs);
extern "C" HOVER_IMPORT void __stdcall LeaveCriticalSection(void* cs);
extern "C" HOVER_IMPORT int32_t __stdcall SetEvent(void* ev);
extern "C" HOVER_IMPORT int32_t __stdcall ResetEvent(void* ev);
extern "C" HOVER_IMPORT uint32_t __stdcall WaitForSingleObject(void* ev, uint32_t ms);
extern "C" HOVER_IMPORT int32_t __stdcall SendMessageA(void* hwnd, uint32_t msg,
    uintptr_t wParam, intptr_t lParam);

extern "C" uint8_t g_460570[];
extern "C" int32_t g_4605d0;
extern "C" int32_t g_460734;
extern "C" int32_t g_460958;
extern "C" int32_t g_460960;
extern "C" void __fastcall DrawEntry402250(void* self, void* unused);

#define FIELD32(p, off) (*(int32_t*)((uint8_t*)(p) + (off)))

// FUNCTION: HOVER 0x0041ece0
extern "C" void __fastcall DisplayEntry41ece0(void* self, void* unused)
{
    uint8_t* disp = (uint8_t*)self;
    uint8_t* gdisp;
    int32_t caseSel2;
    int32_t running;
    void* hwnd;
    int32_t hinst;

    __try
    {
        FIELD32(disp, 0x528) = (int32_t)GetTickCount();
        FIELD32(disp, 0x52c) = FIELD32(disp, 0x528);
        FIELD32(disp, 0x530) = 0;

        gdisp = g_460570;
        FIELD32(gdisp, 0x3e4) = 0;

        if (LoadModule("DISPDIB.DLL", NULL) < 0x20)
        {
            gdisp = g_460570;
            FIELD32(gdisp, 0x3f0) = 0;
        }
        else
        {
            hinst = g_4605d0;
            if (hinst == 0)
                hinst = GetWindowLongA((void*)(uintptr_t)FIELD32(disp, 0x1c), -6);
            hwnd = CreateWindowExA(0, "DisplayDibWindow", NULL, 0x40000000u,
                0, 0, GetSystemMetrics(0), GetSystemMetrics(1),
                NULL, NULL, (void*)(uintptr_t)hinst, NULL);
            gdisp = g_460570;
            FIELD32(gdisp, 0x3f0) = (int32_t)(uintptr_t)hwnd;
        }

        gdisp = g_460570;
        FIELD32(gdisp, 0x3e0) = (g_460960 != 0) ? 1 : 0;

        running = 1;
        while (running != 0)
        {
            EnterCriticalSection(g_460570 + 0x11c);
            SetEvent((void*)(uintptr_t)(uint32_t)FIELD32(disp, 0x50));
            ResetEvent((void*)(uintptr_t)(uint32_t)FIELD32(disp, 0x4c));
            LeaveCriticalSection(g_460570 + 0x11c);
            WaitForSingleObject((void*)(uintptr_t)(uint32_t)FIELD32(disp, 0x48), 0xffffffffu);
            EnterCriticalSection(g_460570 + 0x11c);

            ResetEvent((void*)(uintptr_t)(uint32_t)FIELD32(disp, 0x50));
            SetEvent((void*)(uintptr_t)(uint32_t)FIELD32(disp, 0x4c));
            ResetEvent((void*)(uintptr_t)(uint32_t)FIELD32(disp, 0x48));
            SetEvent((void*)(uintptr_t)(uint32_t)FIELD32(disp, 0x54));
            LeaveCriticalSection(g_460570 + 0x11c);

            caseSel2 = FIELD32(disp, 0xac);
            switch (caseSel2)
            {
            case 0:
                ((HoverTc*)(uintptr_t)disp)->M4184b0(
                    (void*)(uintptr_t)(uint32_t)FIELD32(disp, 0xa4),
                    (void*)(uintptr_t)(uint32_t)FIELD32(disp, 0xa8));
                break;
            case 1:
                ((HoverTc*)(uintptr_t)disp)->M413c10((void*)(uintptr_t)g_460734);
                break;
            case 2:
                running = 0;
                break;
            case 3:
                DrawEntry402250(self, NULL);
                break;
            case 4:
                Ext410510(self);
                break;
            case 5:
                ((HoverTc*)(uintptr_t)disp)->M427a80(
                    (void*)(uintptr_t)(uint32_t)FIELD32(disp, 0xb0));
                break;
            case 6:
                Ext427bb0(self);
                break;
            case 7:
                Ext427cc0(self);
                break;
            }
        }

        if (g_460960 != 0)
        {
            if (g_460958 != 0)
                SendMessageA((void*)(uintptr_t)g_460960, 0x404, 0, 0);
            SendMessageA((void*)(uintptr_t)g_460960, 0x402, 0, 0);
            FIELD32(g_460570, 0x3f0) = 0;
        }

        gdisp = g_460570;
        FIELD32(gdisp, 0x3e0) = 0;

        ResetEvent((void*)(uintptr_t)(uint32_t)FIELD32(disp, 0x4c));
        SetEvent((void*)(uintptr_t)(uint32_t)FIELD32(disp, 0x50));
    }
    __except (1)
    {
    }
}
