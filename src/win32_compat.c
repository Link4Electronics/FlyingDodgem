#include "win32_compat.h"

#include <SDL3/SDL.h>

#include <stdlib.h>
#include <string.h>

/* ---------------------------------------------------------------------------
 * USER32 + small KERNEL32 shims over SDL3 (native-only, FLYINGDODGEM_GDI_COMPAT).
 *
 * See win32_compat.h for the scope. Windows are bookkeeping records (SDL3
 * owns the real window); the screen DC is the shared 640x480 presentation
 * surface hosted by gdi32_compat.c.
 * ------------------------------------------------------------------------- */

#define WINDOW_MAGIC 0x47574e44u /* "DnWG" */

struct GdiWindow
{
    u32 magic;
    u32 style;
    u32 exStyle;
    s32 x, y;
    s32 w, h;             /* client size passed to CreateWindowExA */
    GdiWindow *parent;
    HINSTANCE instance;
    void *wndproc;
    intptr_t userData;
    s32 id;
    u8 visible;
};

static GdiWindow *window_from_hwnd(GdiWindow *hwnd)
{
    if (hwnd == NULL || hwnd->magic != WINDOW_MAGIC)
        return NULL;
    return hwnd;
}

u16 __cdecl RegisterClassA(const void *lpWndClass)
{
    (void)lpWndClass;
    /* Class records matter only for message dispatch, which the harness does
     * not do yet; a nonzero atom is enough for callers to proceed. */
    return 0xC000;
}

GdiWindow * __cdecl CreateWindowExA(u32 exStyle, const char *className,
                                    const char *windowName, u32 style,
                                    s32 x, s32 y, s32 w, s32 h,
                                    GdiWindow *parent, void *menu,
                                    HINSTANCE instance, void *param)
{
    (void)className;
    (void)windowName;
    (void)menu;
    (void)param;

    GdiWindow *hwnd = (GdiWindow *)SDL_malloc(sizeof(GdiWindow));
    if (hwnd == NULL)
        return NULL;
    hwnd->magic = WINDOW_MAGIC;
    hwnd->style = style;
    hwnd->exStyle = exStyle;
    hwnd->x = x;
    hwnd->y = y;
    hwnd->w = w;
    hwnd->h = h;
    hwnd->parent = parent;
    hwnd->instance = instance;
    hwnd->wndproc = NULL;
    hwnd->userData = 0;
    hwnd->id = 0;
    hwnd->visible = (style & 0x10000000u) != 0; /* WS_VISIBLE */
    return hwnd;
}

u8 __cdecl DestroyWindow(GdiWindow *hwnd)
{
    if (hwnd == NULL || hwnd->magic != WINDOW_MAGIC)
        return 0;
    hwnd->magic = 0;
    SDL_free(hwnd);
    return 1;
}

s32 __cdecl DefWindowProcA(GdiWindow *hwnd, u32 msg, uintptr_t wParam, intptr_t lParam)
{
    (void)hwnd;
    (void)msg;
    (void)wParam;
    (void)lParam;
    return 0;
}

s32 __cdecl GetWindowLongA(GdiWindow *hwnd, s32 index)
{
    GdiWindow *w = window_from_hwnd(hwnd);
    if (w == NULL)
        return 0;
    switch (index)
    {
    case GWL_WNDPROC:
        return (s32)(intptr_t)w->wndproc;
    case GWL_HINSTANCE:
        return (s32)(intptr_t)w->instance;
    case GWL_HWNDPARENT:
        return (s32)(intptr_t)w->parent;
    case GWL_ID:
        return w->id;
    case GWL_STYLE:
        return (s32)w->style;
    case GWL_EXSTYLE:
        return (s32)w->exStyle;
    case GWL_USERDATA:
        return (s32)w->userData;
    default:
        return 0;
    }
}

s32 __cdecl SetWindowLongA(GdiWindow *hwnd, s32 index, s32 value)
{
    GdiWindow *w = window_from_hwnd(hwnd);
    if (w == NULL)
        return 0;
    s32 prev = 0;
    switch (index)
    {
    case GWL_WNDPROC:
        prev = (s32)(intptr_t)w->wndproc;
        w->wndproc = (void *)(intptr_t)value;
        break;
    case GWL_HINSTANCE:
        prev = (s32)(intptr_t)w->instance;
        w->instance = (HINSTANCE)(intptr_t)value;
        break;
    case GWL_HWNDPARENT:
        prev = (s32)(intptr_t)w->parent;
        w->parent = (GdiWindow *)(intptr_t)value;
        break;
    case GWL_ID:
        prev = w->id;
        w->id = value;
        break;
    case GWL_STYLE:
        prev = (s32)w->style;
        w->style = (u32)value;
        break;
    case GWL_EXSTYLE:
        prev = (s32)w->exStyle;
        w->exStyle = (u32)value;
        break;
    case GWL_USERDATA:
        prev = (s32)w->userData;
        w->userData = value;
        break;
    default:
        break;
    }
    return prev;
}

u8 __cdecl ShowWindow(GdiWindow *hwnd, s32 cmdShow)
{
    GdiWindow *w = window_from_hwnd(hwnd);
    if (w == NULL)
        return 0;
    u8 prev = w->visible;
    w->visible = (cmdShow != 0); /* SW_HIDE == 0, every show command != 0 */
    return prev;
}

u8 __cdecl SetWindowPos(GdiWindow *hwnd, GdiWindow *insertAfter,
                        s32 x, s32 y, s32 cx, s32 cy, u32 flags)
{
    (void)insertAfter;
    (void)flags;
    GdiWindow *w = window_from_hwnd(hwnd);
    if (w == NULL)
        return 0;
    w->x = x;
    w->y = y;
    if (cx > 0)
        w->w = cx;
    if (cy > 0)
        w->h = cy;
    return 1;
}

u8 __cdecl MoveWindow(GdiWindow *hwnd, s32 x, s32 y, s32 w, s32 h, u8 repaint)
{
    (void)repaint;
    GdiWindow *win = window_from_hwnd(hwnd);
    if (win == NULL)
        return 0;
    win->x = x;
    win->y = y;
    win->w = w;
    win->h = h;
    return 1;
}

u8 __cdecl GetClientRect(GdiWindow *hwnd, GDI_RECT *rect)
{
    GdiWindow *w = window_from_hwnd(hwnd);
    if (w == NULL || rect == NULL)
        return 0;
    rect->left = 0;
    rect->top = 0;
    rect->right = w->w;
    rect->bottom = w->h;
    return 1;
}

u8 __cdecl GetWindowRect(GdiWindow *hwnd, GDI_RECT *rect)
{
    GdiWindow *w = window_from_hwnd(hwnd);
    if (w == NULL || rect == NULL)
        return 0;
    /* Window rect approximated by the client rect at the window origin (SDL3
     * owns the real chrome; retail only uses this for layout math). */
    rect->left = w->x;
    rect->top = w->y;
    rect->right = w->x + w->w;
    rect->bottom = w->y + w->h;
    return 1;
}

HDC __cdecl GetDC(GdiWindow *hwnd)
{
    (void)hwnd;
    /* Every window draws into the single shared presentation surface. */
    return GdiCompat_GetScreenDC(640, 480, NULL, 0);
}

s32 __cdecl ReleaseDC(GdiWindow *hwnd, HDC hdc)
{
    (void)hwnd;
    (void)hdc;
    return 1;
}

u8 __cdecl InvalidateRect(GdiWindow *hwnd, const GDI_RECT *rect, u8 erase)
{
    (void)hwnd;
    (void)rect;
    (void)erase;
    return 1;
}

u8 __cdecl ValidateRect(GdiWindow *hwnd, const GDI_RECT *rect)
{
    (void)hwnd;
    (void)rect;
    return 1;
}

u8 __cdecl UpdateWindow(GdiWindow *hwnd)
{
    (void)hwnd;
    return 1;
}

u8 __cdecl RedrawWindow(GdiWindow *hwnd, const GDI_RECT *rect,
                        void *updateRgn, u32 flags)
{
    (void)hwnd;
    (void)rect;
    (void)updateRgn;
    (void)flags;
    return 1;
}

/* --- Message queue -------------------------------------------------------
 * The retail pump (pump.c PumpMessageLoop) tests for work through USER32
 * PeekMessageA; here the SDL3 event queue is the source of messages. SDL3
 * owns the real window, so queued messages carry no GdiWindow handle.
 * ------------------------------------------------------------------------ */

#define MSG_QUEUE_CAPACITY 64

static GdiMsg g_msgQueue[MSG_QUEUE_CAPACITY];
static int g_msgHead;   /* index of the oldest queued message */
static int g_msgCount;

static void msg_queue_push(const GdiMsg *m)
{
    if (g_msgCount == MSG_QUEUE_CAPACITY)
    {
        /* Full queue: drop the oldest entry so the newest input survives. */
        g_msgHead = (g_msgHead + 1) % MSG_QUEUE_CAPACITY;
        g_msgCount--;
    }
    g_msgQueue[(g_msgHead + g_msgCount) % MSG_QUEUE_CAPACITY] = *m;
    g_msgCount++;
}

/* Win32 MK_* flags for the mouse messages, from the live SDL3 state. */
static uintptr_t msg_mouse_flags(void)
{
    SDL_MouseButtonFlags state = SDL_GetMouseState(NULL, NULL);
    SDL_Keymod mods = SDL_GetModState();
    uintptr_t flags = 0;

    if (state & SDL_BUTTON_LMASK)
        flags |= MK_LBUTTON;
    if (state & SDL_BUTTON_RMASK)
        flags |= MK_RBUTTON;
    if (state & SDL_BUTTON_MMASK)
        flags |= MK_MBUTTON;
    if (mods & SDL_KMOD_SHIFT)
        flags |= MK_SHIFT;
    if (mods & SDL_KMOD_CTRL)
        flags |= MK_CONTROL;
    return flags;
}

/* lParam of the mouse messages: x in the low word, y in the high word. */
static intptr_t msg_lparam_xy(s32 x, s32 y)
{
    return (intptr_t)(((u32)(u16)x) | ((u32)y << 16));
}

static void msg_queue_pump(void)
{
    SDL_Event ev;

    SDL_PumpEvents();
    while (SDL_PeepEvents(&ev, 1, SDL_GETEVENT,
                          SDL_EVENT_FIRST, SDL_EVENT_LAST) == 1)
    {
        GdiMsg m;

        m.hwnd = NULL;
        m.time = (u32)SDL_GetTicks();
        m.x = 0;
        m.y = 0;

        switch (ev.type)
        {
        case SDL_EVENT_QUIT:
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            /* The compat DefWindowProcA never turns WM_CLOSE into a quit,
             * so closing the window is reported straight as WM_QUIT. */
            m.message = WM_QUIT;
            m.wParam = 0;
            m.lParam = 0;
            break;

        case SDL_EVENT_MOUSE_MOTION:
            m.message = WM_MOUSEMOVE;
            m.wParam = msg_mouse_flags();
            m.lParam = msg_lparam_xy((s32)ev.motion.x, (s32)ev.motion.y);
            m.x = (s32)ev.motion.x;
            m.y = (s32)ev.motion.y;
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            int pressed = (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN);
            switch (ev.button.button)
            {
            case SDL_BUTTON_LEFT:
                m.message = pressed ? WM_LBUTTONDOWN : WM_LBUTTONUP;
                break;
            case SDL_BUTTON_MIDDLE:
                m.message = pressed ? WM_MBUTTONDOWN : WM_MBUTTONUP;
                break;
            case SDL_BUTTON_RIGHT:
                m.message = pressed ? WM_RBUTTONDOWN : WM_RBUTTONUP;
                break;
            default:
                continue;
            }
            m.wParam = msg_mouse_flags();
            m.lParam = msg_lparam_xy((s32)ev.button.x, (s32)ev.button.y);
            m.x = (s32)ev.button.x;
            m.y = (s32)ev.button.y;
            break;
        }

        default:
            /* Keyboard/text events wait for the retail virtual-key mapping. */
            continue;
        }

        msg_queue_push(&m);
    }
}

int __stdcall PeekMessageA(void *pMsg, void *hWnd, u32 nMin, u32 nMax, u32 nRemove)
{
    int i;

    msg_queue_pump();

    for (i = 0; i < g_msgCount; i++)
    {
        const GdiMsg *m = &g_msgQueue[(g_msgHead + i) % MSG_QUEUE_CAPACITY];

        if ((nMin != 0 || nMax != 0) && (m->message < nMin || m->message > nMax))
            continue;
        if (hWnd != NULL && m->hwnd != (GdiWindow *)hWnd)
            continue;

        if (pMsg != NULL)
            *(GdiMsg *)pMsg = *m;
        if ((nRemove & PM_REMOVE) != 0)
        {
            g_msgHead = (g_msgHead + 1) % MSG_QUEUE_CAPACITY;
            g_msgCount--;
        }
        return 1;
    }
    return 0;
}

s32 __cdecl GetSystemMetrics(s32 index)
{
    switch (index)
    {
    case SM_CXSCREEN:
    case SM_CYSCREEN:
    {
        /* Report the presentation surface size (640x480 fallback before the
         * harness has created it). */
        HDC hdc = GetDC(NULL);
        if (hdc != NULL)
            return GetDeviceCaps(hdc, index == SM_CXSCREEN ? GDI_HORZRES
                                                           : GDI_VERTRES);
        return index == SM_CXSCREEN ? 640 : 480;
    }
    default:
        return 0;
    }
}

u32 __cdecl GetTickCount(void)
{
    return (u32)SDL_GetTicks();
}

char * __stdcall CharUpperA(char *psz)
{
    char *p = psz;
    if (p != NULL)
    {
        while (*p != '\0')
        {
            if (*p >= 'a' && *p <= 'z')
                *p = (char)(*p - 'a' + 'A');
            p++;
        }
    }
    return psz;
}

void __cdecl EnterCriticalSection(void *cs)
{
    (void)cs;
}

void __cdecl LeaveCriticalSection(void *cs)
{
    (void)cs;
}

HINSTANCE __cdecl LoadModule(const char *moduleName, void *paramBlock)
{
    (void)paramBlock;
    HINSTANCE h = LoadLibraryA(moduleName);
    /* Win16-style LoadModule reports failures as error codes < 32 instead of
     * NULL; retail checks `cmp eax, 0x20; jae success`. */
    if (h == NULL)
        return (HINSTANCE)(uintptr_t)2; /* ERROR_FILE_NOT_FOUND */
    return h;
}

/* Display-path synchronisation. The retail dispatcher hands work to a render
 * thread through a critical section plus a four-event handshake; the native
 * reconstruction runs the display entry inline, so the events are inert. */
int __cdecl SetEvent(HANDLE ev)
{
    (void)ev;
    return 1;
}

int __cdecl ResetEvent(HANDLE ev)
{
    (void)ev;
    return 1;
}

u32 __cdecl WaitForSingleObject(HANDLE ev, u32 ms)
{
    (void)ev;
    (void)ms;
    return 0; /* WAIT_OBJECT_0 */
}

s32 __cdecl SendMessageA(GdiWindow *hwnd, u32 msg, uintptr_t wParam,
                         intptr_t lParam)
{
    (void)hwnd;
    (void)msg;
    (void)wParam;
    (void)lParam;
    return 0;
}
