#ifndef FLYINGDODGEM_WIN32_COMPAT_H
#define FLYINGDODGEM_WIN32_COMPAT_H

#include "gdi32_compat.h"
#include "menu.h" /* HWND: the single main window type shared with the menu layer */

/* USER32 + small KERNEL32 shims for the retail present path (native-only,
 * FLYINGDODGEM_GDI_COMPAT; the MSVC 5.0 reccmp build never compiles this).
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

/* Message ids the native queue synthesises from SDL3 events. Only the quit
 * and mouse paths are produced: keyboard events need the retail virtual-key
 * mapping, which comes with the key handling they belong to. */
#define WM_QUIT          0x0012
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

s32 __cdecl GetSystemMetrics(s32 index);

/* --- KERNEL32 bits used by the display init --- */

u32 __cdecl GetTickCount(void);
HINSTANCE __cdecl LoadModule(const char *moduleName, void *paramBlock);

#endif
