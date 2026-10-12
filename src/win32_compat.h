#ifndef FLYINGDODGEM_WIN32_COMPAT_H
#define FLYINGDODGEM_WIN32_COMPAT_H

#include "gdi32_compat.h"
#include "menu.h" /* HWND: the single main window type shared with the menu layer */

/* USER32 + small KERNEL32 shims for the retail present path (native-only,
 * FLYINGDODGEM_GDI_COMPAT; the MSVC 4 reccmp build never compiles this).
 *
 * The retail display path around 0x41ece0 does GetTickCount, LoadModule of
 * DISPDIB.DLL (the Win95 fullscreen-DIB helper), GetWindowLong(GWL_HINSTANCE),
 * GetSystemMetrics(SM_CXSCREEN/CYSCREEN) and CreateWindowExA of the
 * "DisplayDibWindow" child class; the backbuffer setup around 0x424300 then
 * does GetDC(NULL)/ReleaseDC, GetClientRect, MoveWindow and the periodic
 * RedrawWindow/InvalidateRect/UpdateWindow invalidations scattered through
 * the binary. SDL3 owns the real window, so these are behavioural shims:
 * windows are bookkeeping records, paint invalidation is a no-op (the harness
 * repaints every frame anyway), and the screen DC is the shared 640x480
 * presentation surface.
 */

typedef struct GdiWindow GdiWindow;

/* Window long indices used by the retail code (32-bit Win95 values). */
#define GWL_WNDPROC    (-4)
#define GWL_HINSTANCE  (-6)
#define GWL_HWNDPARENT (-8)
#define GWL_ID         (-12)
#define GWL_STYLE      (-16)
#define GWL_EXSTYLE    (-20)
#define GWL_USERDATA   (-21)

/* GetSystemMetrics indices the retail present path queries. */
#define SM_CXSCREEN 0
#define SM_CYSCREEN 1

/* --- Window bookkeeping --- */

/* Returns a nonzero class atom. Class records only matter once message
 * dispatch exists; CreateWindowExA does not validate class names. */
u16 __cdecl RegisterClassA(const void *lpWndClass);

GdiWindow * __cdecl CreateWindowExA(u32 exStyle, const char *className,
                                    const char *windowName, u32 style,
                                    s32 x, s32 y, s32 w, s32 h,
                                    GdiWindow *parent, void *menu,
                                    HINSTANCE instance, void *param);

u8 __cdecl DestroyWindow(GdiWindow *hwnd);

s32 __cdecl DefWindowProcA(GdiWindow *hwnd, u32 msg, uintptr_t wParam, intptr_t lParam);

s32 __cdecl GetWindowLongA(GdiWindow *hwnd, s32 index);
s32 __cdecl SetWindowLongA(GdiWindow *hwnd, s32 index, s32 value);

u8 __cdecl ShowWindow(GdiWindow *hwnd, s32 cmdShow);
u8 __cdecl SetWindowPos(GdiWindow *hwnd, GdiWindow *insertAfter,
                        s32 x, s32 y, s32 cx, s32 cy, u32 flags);
u8 __cdecl MoveWindow(GdiWindow *hwnd, s32 x, s32 y, s32 w, s32 h, u8 repaint);
u8 __cdecl GetClientRect(GdiWindow *hwnd, GDI_RECT *rect);
u8 __cdecl GetWindowRect(GdiWindow *hwnd, GDI_RECT *rect);

/* --- Screen DC access (present path clears/reads the screen surface) --- */

HDC __cdecl GetDC(GdiWindow *hwnd);
s32 __cdecl ReleaseDC(GdiWindow *hwnd, HDC hdc);

/* --- Invalidation / repaint (no-op: the harness repaints every frame) --- */

u8 __cdecl InvalidateRect(GdiWindow *hwnd, const GDI_RECT *rect, u8 erase);
u8 __cdecl ValidateRect(GdiWindow *hwnd, const GDI_RECT *rect);
u8 __cdecl UpdateWindow(GdiWindow *hwnd);
u8 __cdecl RedrawWindow(GdiWindow *hwnd, const GDI_RECT *rect,
                        void *updateRgn, u32 flags);

/* --- Message queue (the retail pump peeks through USER32) --- */

/* Message ids the native queue synthesises from SDL3 events: quit, mouse
 * and keyboard (Win95 VKs in wParam, retail-shaped lParam). */
#define WM_QUIT          0x0012
#define WM_COMMAND       0x0111
#define WM_KEYDOWN       0x0100
#define WM_KEYUP         0x0101
#define WM_CHAR          0x0102
#define WM_SYSKEYDOWN    0x0104
#define WM_SYSKEYUP      0x0105
#define WM_TIMER         0x0113
#define WM_MOUSEMOVE     0x0200
#define WM_LBUTTONDOWN   0x0201
#define WM_LBUTTONUP     0x0202
#define WM_RBUTTONDOWN   0x0204
#define WM_RBUTTONUP     0x0205
#define WM_MBUTTONDOWN   0x0207
#define WM_MBUTTONUP     0x0208

/* wParam mouse key flags (Win32 MK_* values, not the SDL3 button mask). */
#define MK_LBUTTON 0x0001
#define MK_RBUTTON 0x0002
#define MK_SHIFT   0x0004
#define MK_CONTROL 0x0008
#define MK_MBUTTON 0x0010

#define PM_NOREMOVE 0x0000
#define PM_REMOVE   0x0001

/* Virtual-key codes (Win95 values) for the keys HOVER uses: function keys
 * for menus (F1 help, F2 start, F3 pause, F8 key config), arrows for
 * maneuvering, plus the surrounding editing/navigation keys. */
#define VK_BACK      0x08
#define VK_TAB       0x09
#define VK_RETURN    0x0D
#define VK_SHIFT     0x10
#define VK_CONTROL   0x11
#define VK_MENU      0x12
#define VK_PAUSE     0x13
#define VK_CAPITAL   0x14
#define VK_ESCAPE    0x1B
#define VK_SPACE     0x20
#define VK_PRIOR     0x21
#define VK_NEXT      0x22
#define VK_END       0x23
#define VK_HOME      0x24
#define VK_LEFT      0x25
#define VK_UP        0x26
#define VK_RIGHT     0x27
#define VK_DOWN      0x28
#define VK_INSERT    0x2D
#define VK_DELETE    0x2E
#define VK_NUMPAD0   0x60
#define VK_NUMPAD1   0x61
#define VK_NUMPAD2   0x62
#define VK_NUMPAD3   0x63
#define VK_NUMPAD4   0x64
#define VK_NUMPAD5   0x65
#define VK_NUMPAD6   0x66
#define VK_NUMPAD7   0x67
#define VK_NUMPAD8   0x68
#define VK_NUMPAD9   0x69
#define VK_MULTIPLY  0x6A
#define VK_ADD       0x6B
#define VK_SEPARATOR 0x6C
#define VK_SUBTRACT  0x6D
#define VK_DECIMAL   0x6E
#define VK_DIVIDE    0x6F
#define VK_F1        0x70
#define VK_F2        0x71

/* Menu command ids from the retail MENU/ACCEL resources (menu id 128).
 * F2 is the accelerator for IDM_STARTGAME; the retail pump turns the key
 * into WM_COMMAND 0x8009 before DispatchMessage. */
#define IDM_STARTGAME   0x8009
#define IDM_STARTAT     0x800A
#define IDM_PAUSE       0x800B
#define IDM_FULLSCREEN  0x800C
#define VK_F3        0x72
#define VK_F4        0x73
#define VK_F5        0x74
#define VK_F6        0x75
#define VK_F7        0x76
#define VK_F8        0x77
#define VK_F9        0x78
#define VK_F10       0x79
#define VK_F11       0x7A
#define VK_F12       0x7B
#define VK_NUMLOCK   0x90
#define VK_SCROLL    0x91
#define VK_LSHIFT    0xA0
#define VK_RSHIFT    0xA1
#define VK_LCONTROL  0xA2
#define VK_RCONTROL  0xA3
#define VK_LMENU     0xA4
#define VK_RMENU     0xA5

/* The MSG record the pump peeks into, in native pointer width: same field
 * order as the retail 28-byte x86 MSG that lands at app object +0x30. */
typedef struct GdiMsg
{
    GdiWindow *hwnd;
    u32 message;
    uintptr_t wParam;
    intptr_t lParam;
    u32 time;
    s32 x;
    s32 y;
} GdiMsg;

int __stdcall PeekMessageA(void *pMsg, void *hWnd, u32 nMin, u32 nMax, u32 nRemove);
u8 __stdcall PostMessageA(GdiWindow *hwnd, u32 msg, uintptr_t wParam,
                          intptr_t lParam);

/* USER32 timers: due-checked in the message pump, delivered as WM_TIMER.
 * A non-NULL TIMERPROC cannot execute (retail 32-bit code); the message
 * is still posted. */
u32 __stdcall SetTimer(GdiWindow *hwnd, u32 id, u32 timeoutMs, void *proc);
u8 __stdcall KillTimer(GdiWindow *hwnd, u32 id);

/* Main-loop pieces the retail pump uses (pump.c): TranslateMessage turns
 * key downs into WM_CHAR, DispatchMessageA routes to DefWindowProcA (retail
 * window procs are 32-bit code and cannot run here until matched). */
u8 __stdcall TranslateMessage(const void *pMsg);
s32 __cdecl DispatchMessageA(GdiWindow *hwnd, u32 msg, uintptr_t wParam,
                             intptr_t lParam);

/* Async + message-time key state over the live SDL3 keyboard. */
s16 __stdcall GetAsyncKeyState(s32 vk);
s16 __stdcall GetKeyState(s32 vk);
/* Minimal virtual-key lookup for the key-config UI (letters/digits). */
s16 __stdcall VkKeyScanA(u8 ch);

/* In-place upper-caser used by the beacon build (0x423ea0); returns psz. */
char * __stdcall CharUpperA(char *psz);

s32 __cdecl GetSystemMetrics(s32 index);

/* --- KERNEL32 bits used by the display init --- */

u32 __cdecl GetTickCount(void);
HINSTANCE __cdecl LoadModule(const char *moduleName, void *paramBlock);

/* Crit sections used by the render entry (0x402250). The harness drives the
 * render path from one thread, so these only exist to satisfy the retail call
 * shape: enter/leave are no-ops. */
void __cdecl EnterCriticalSection(void *cs);
void __cdecl LeaveCriticalSection(void *cs);

/* Win32 event primitives used by the display entry (0x41ece0).  The reccmp
 * build links the retail-shaped calls; the native harness drives the display
 * loop from a single thread so the waits are behavioural shims. */
#ifndef INFINITE
#define INFINITE 0xffffffffu
#endif

typedef void *HANDLE;

HANDLE __cdecl CreateEventA(void *sa, int manualReset, int initialState,
                            const char *name);
int __cdecl SetEvent(HANDLE ev);
int __cdecl ResetEvent(HANDLE ev);
u32 __cdecl WaitForSingleObject(HANDLE ev, u32 ms);

/* USER32 message + module lookup used by the display entry (0x41ece0). */
s32 __cdecl SendMessageA(GdiWindow *hwnd, u32 msg, uintptr_t wParam,
                         intptr_t lParam);
void *__cdecl GetProcAddress(HINSTANCE inst, const char *procName);

/* KERNEL32 debug log for the span-bump overflow in M402e00 (0x4030d9 pushes
 * "Trashing Heap"). Native no-op: no debugger sink, must only not crash. */
void __stdcall OutputDebugStringA(char* psz);

#endif
