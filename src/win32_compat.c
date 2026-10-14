#include "win32_compat.h"
#include "winmm_compat.h"

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

u8 __cdecl InflateRect(GDI_RECT *rect, s32 dx, s32 dy)
{
    if (rect == NULL)
        return 0;
    rect->left -= dx;
    rect->top -= dy;
    rect->right += dx;
    rect->bottom += dy;
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

static void win32_pump_timers(void);

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

/* --- Keyboard ------------------------------------------------------------
 * Retail reads menus through key messages (F1/F2/F3/F8) and maneuvers
 * through GetAsyncKeyState (arrows).  SDL3 keycodes map onto Win95 VKs;
 * the lParam keeps the retail shape: repeat count, scan code,
 * extended-key, context/previous/transition bits. */

/* SDL keycode -> Win32 virtual key, 0 when unmapped. */
static s32 vk_from_sdl_key(s32 key)
{
    if (key >= 'a' && key <= 'z')
        return (s32)(key - ('a' - 'A'));
    if (key >= 'A' && key <= 'Z')
        return key;
    if (key >= '0' && key <= '9')
        return key;
    switch (key)
    {
    case SDLK_BACKSPACE: return VK_BACK;
    case SDLK_TAB: return VK_TAB;
    case SDLK_RETURN: return VK_RETURN;
    case SDLK_ESCAPE: return VK_ESCAPE;
    case SDLK_SPACE: return VK_SPACE;
    case SDLK_PAGEUP: return VK_PRIOR;
    case SDLK_PAGEDOWN: return VK_NEXT;
    case SDLK_END: return VK_END;
    case SDLK_HOME: return VK_HOME;
    case SDLK_LEFT: return VK_LEFT;
    case SDLK_UP: return VK_UP;
    case SDLK_RIGHT: return VK_RIGHT;
    case SDLK_DOWN: return VK_DOWN;
    case SDLK_INSERT: return VK_INSERT;
    case SDLK_DELETE: return VK_DELETE;
    case SDLK_F1: return VK_F1;
    case SDLK_F2: return VK_F2;
    case SDLK_F3: return VK_F3;
    case SDLK_F4: return VK_F4;
    case SDLK_F5: return VK_F5;
    case SDLK_F6: return VK_F6;
    case SDLK_F7: return VK_F7;
    case SDLK_F8: return VK_F8;
    case SDLK_F9: return VK_F9;
    case SDLK_F10: return VK_F10;
    case SDLK_F11: return VK_F11;
    case SDLK_F12: return VK_F12;
    case SDLK_KP_0: return VK_NUMPAD0;
    case SDLK_KP_1: return VK_NUMPAD1;
    case SDLK_KP_2: return VK_NUMPAD2;
    case SDLK_KP_3: return VK_NUMPAD3;
    case SDLK_KP_4: return VK_NUMPAD4;
    case SDLK_KP_5: return VK_NUMPAD5;
    case SDLK_KP_6: return VK_NUMPAD6;
    case SDLK_KP_7: return VK_NUMPAD7;
    case SDLK_KP_8: return VK_NUMPAD8;
    case SDLK_KP_9: return VK_NUMPAD9;
    case SDLK_KP_MULTIPLY: return VK_MULTIPLY;
    case SDLK_KP_PLUS: return VK_ADD;
    case SDLK_KP_MINUS: return VK_SUBTRACT;
    case SDLK_KP_PERIOD:
    case SDLK_KP_DECIMAL: return VK_DECIMAL;
    case SDLK_KP_DIVIDE: return VK_DIVIDE;
    case SDLK_LSHIFT: return VK_LSHIFT;
    case SDLK_RSHIFT: return VK_RSHIFT;
    case SDLK_LCTRL: return VK_LCONTROL;
    case SDLK_RCTRL: return VK_RCONTROL;
    case SDLK_LALT: return VK_LMENU;
    case SDLK_RALT: return VK_RMENU;
    case SDLK_CAPSLOCK: return VK_CAPITAL;
    case SDLK_NUMLOCKCLEAR: return VK_NUMLOCK;
    case SDLK_SCROLLLOCK: return VK_SCROLL;
    case SDLK_PAUSE: return VK_PAUSE;
    default: return 0;
    }
}

/* Virtual key -> SDL scancode for the async state poll, SDL_SCANCODE_UNKNOWN
 * when the key has no positional scan code. */
static s32 scancode_from_vk(s32 vk)
{
    if (vk >= 'A' && vk <= 'Z')
        return (s32)SDL_SCANCODE_A + (vk - 'A');
    if (vk >= '0' && vk <= '9')
        return (s32)SDL_SCANCODE_0 + (vk - '0');
    switch (vk)
    {
    case VK_BACK: return SDL_SCANCODE_BACKSPACE;
    case VK_TAB: return SDL_SCANCODE_TAB;
    case VK_RETURN: return SDL_SCANCODE_RETURN;
    case VK_ESCAPE: return SDL_SCANCODE_ESCAPE;
    case VK_SPACE: return SDL_SCANCODE_SPACE;
    case VK_PRIOR: return SDL_SCANCODE_PAGEUP;
    case VK_NEXT: return SDL_SCANCODE_PAGEDOWN;
    case VK_END: return SDL_SCANCODE_END;
    case VK_HOME: return SDL_SCANCODE_HOME;
    case VK_LEFT: return SDL_SCANCODE_LEFT;
    case VK_UP: return SDL_SCANCODE_UP;
    case VK_RIGHT: return SDL_SCANCODE_RIGHT;
    case VK_DOWN: return SDL_SCANCODE_DOWN;
    case VK_INSERT: return SDL_SCANCODE_INSERT;
    case VK_DELETE: return SDL_SCANCODE_DELETE;
    case VK_F1: return SDL_SCANCODE_F1;
    case VK_F2: return SDL_SCANCODE_F2;
    case VK_F3: return SDL_SCANCODE_F3;
    case VK_F4: return SDL_SCANCODE_F4;
    case VK_F5: return SDL_SCANCODE_F5;
    case VK_F6: return SDL_SCANCODE_F6;
    case VK_F7: return SDL_SCANCODE_F7;
    case VK_F8: return SDL_SCANCODE_F8;
    case VK_F9: return SDL_SCANCODE_F9;
    case VK_F10: return SDL_SCANCODE_F10;
    case VK_F11: return SDL_SCANCODE_F11;
    case VK_F12: return SDL_SCANCODE_F12;
    case VK_NUMPAD0: return SDL_SCANCODE_KP_0;
    case VK_NUMPAD1: return SDL_SCANCODE_KP_1;
    case VK_NUMPAD2: return SDL_SCANCODE_KP_2;
    case VK_NUMPAD3: return SDL_SCANCODE_KP_3;
    case VK_NUMPAD4: return SDL_SCANCODE_KP_4;
    case VK_NUMPAD5: return SDL_SCANCODE_KP_5;
    case VK_NUMPAD6: return SDL_SCANCODE_KP_6;
    case VK_NUMPAD7: return SDL_SCANCODE_KP_7;
    case VK_NUMPAD8: return SDL_SCANCODE_KP_8;
    case VK_NUMPAD9: return SDL_SCANCODE_KP_9;
    case VK_MULTIPLY: return SDL_SCANCODE_KP_MULTIPLY;
    case VK_ADD: return SDL_SCANCODE_KP_PLUS;
    case VK_SUBTRACT: return SDL_SCANCODE_KP_MINUS;
    case VK_DECIMAL: return SDL_SCANCODE_KP_PERIOD;
    case VK_DIVIDE: return SDL_SCANCODE_KP_DIVIDE;
    case VK_LSHIFT: return SDL_SCANCODE_LSHIFT;
    case VK_RSHIFT: return SDL_SCANCODE_RSHIFT;
    case VK_LCONTROL: return SDL_SCANCODE_LCTRL;
    case VK_RCONTROL: return SDL_SCANCODE_RCTRL;
    case VK_LMENU: return SDL_SCANCODE_LALT;
    case VK_RMENU: return SDL_SCANCODE_RALT;
    case VK_CAPITAL: return SDL_SCANCODE_CAPSLOCK;
    case VK_NUMLOCK: return SDL_SCANCODE_NUMLOCKCLEAR;
    case VK_SCROLL: return SDL_SCANCODE_SCROLLLOCK;
    case VK_PAUSE: return SDL_SCANCODE_PAUSE;
    case VK_SHIFT: return SDL_SCANCODE_LSHIFT;
    case VK_CONTROL: return SDL_SCANCODE_LCTRL;
    case VK_MENU: return SDL_SCANCODE_LALT;
    default: return SDL_SCANCODE_UNKNOWN;
    }
}

/* Keys whose lParam carries the extended-key bit on real hardware. */
static int vk_is_extended(s32 vk)
{
    switch (vk)
    {
    case VK_PRIOR:
    case VK_NEXT:
    case VK_END:
    case VK_HOME:
    case VK_LEFT:
    case VK_UP:
    case VK_RIGHT:
    case VK_DOWN:
    case VK_INSERT:
    case VK_DELETE:
    case VK_NUMLOCK:
    case VK_RSHIFT:
    case VK_RCONTROL:
    case VK_RMENU:
    case VK_DIVIDE:
        return 1;
    default:
        return 0;
    }
}

s16 __stdcall GetAsyncKeyState(s32 vk)
{
    s32 sc = scancode_from_vk(vk);
    int nkeys = 0;
    const u8 *state;
    static u8 prev[SDL_SCANCODE_COUNT];
    u8 down = 0;
    u8 pressed = 0;

    if (sc <= SDL_SCANCODE_UNKNOWN || sc >= SDL_SCANCODE_COUNT)
        return 0;
    SDL_PumpEvents();
    state = (const u8 *)SDL_GetKeyboardState(&nkeys);
    if (state == NULL || sc >= nkeys)
        return 0;
    down = state[sc] != 0;
    pressed = (u8)(down && !prev[sc]);
    /* Refresh the edge latch from the live state. */
    {
        int i;
        for (i = 0; i < nkeys && i < SDL_SCANCODE_COUNT; i++)
            prev[i] = state[i] != 0;
    }
    return (s16)((down ? 0x8000 : 0) | (pressed ? 1 : 0));
}

s16 __stdcall GetKeyState(s32 vk)
{
    s32 sc = scancode_from_vk(vk);
    int nkeys = 0;
    const u8 *state;
    u16 toggled = 0;

    if (sc <= SDL_SCANCODE_UNKNOWN || sc >= SDL_SCANCODE_COUNT)
        return 0;
    SDL_PumpEvents();
    state = (const u8 *)SDL_GetKeyboardState(&nkeys);
    if (state == NULL || sc >= nkeys)
        return 0;
    /* Toggle low bit for the locking keys, from the live modifier state. */
    {
        SDL_Keymod mods = SDL_GetModState();
        if (vk == VK_CAPITAL && (mods & SDL_KMOD_CAPS))
            toggled = 1;
        if (vk == VK_NUMLOCK && (mods & SDL_KMOD_NUM))
            toggled = 1;
        if (vk == VK_SCROLL && (mods & SDL_KMOD_SCROLL))
            toggled = 1;
    }
    return (s16)((state[sc] ? 0x8000 : 0) | toggled);
}

s16 __stdcall VkKeyScanA(u8 ch)
{
    if (ch >= 'a' && ch <= 'z')
        return (s16)(0x100 | (ch - ('a' - 'A')));
    if (ch >= 'A' && ch <= 'Z')
        return (s16)ch;
    if (ch >= '0' && ch <= '9')
        return (s16)ch;
    return -1;
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

        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
        {
            s32 vk = vk_from_sdl_key((s32)ev.key.key);
            int down = (ev.type == SDL_EVENT_KEY_DOWN);
            u32 repeat = 1;
            u32 scan = (u32)ev.key.scancode & 0xffu;
            u32 lp = repeat | (scan << 16);
            if (vk == 0)
                continue;
            if ((ev.key.mod & SDL_KMOD_ALT) != 0)
            {
                m.message = down ? WM_SYSKEYDOWN : WM_SYSKEYUP;
                lp |= 1u << 29;
            }
            else
            {
                m.message = down ? WM_KEYDOWN : WM_KEYUP;
            }
            if (vk_is_extended(vk))
                lp |= 1u << 24;
            if (!down)
                lp |= (1u << 30) | (1u << 31);
            m.wParam = (uintptr_t)(u32)vk;
            m.lParam = (intptr_t)lp;
            break;
        }

        default:
            /* Text/input-method events have no retail consumer yet. */
            continue;
        }

        msg_queue_push(&m);
    }

    /* Timer due-checks (USER32 + multimedia) queue WM_TIMER / callbacks. */
    win32_pump_timers();
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

u8 __stdcall PostMessageA(GdiWindow *hwnd, u32 msg, uintptr_t wParam,
                          intptr_t lParam)
{
    GdiMsg m;

    m.hwnd = window_from_hwnd(hwnd);
    if (hwnd != NULL && m.hwnd == NULL)
        return 0;
    m.message = msg;
    m.wParam = wParam;
    m.lParam = lParam;
    m.time = (u32)SDL_GetTicks();
    m.x = 0;
    m.y = 0;
    msg_queue_push(&m);
    return 1;
}

/* --- USER32 timers -------------------------------------------------------
 * WM_TIMER delivery is due-checked in the message pump (single-threaded).
 * A non-NULL TIMERPROC cannot execute (retail 32-bit code); the message is
 * still posted so message-based users work. */

#define WIN32_TIMER_SLOTS 16

typedef struct Win32Timer
{
    u8 used;
    GdiWindow *hwnd;
    u32 id;
    u32 intervalMs;
    u32 dueMs;
} Win32Timer;

static Win32Timer g_winTimers[WIN32_TIMER_SLOTS];
static u32 g_winTimerNext = 1;

u32 __stdcall SetTimer(GdiWindow *hwnd, u32 id, u32 timeoutMs, void *proc)
{
    int i;

    (void)proc;
    if (timeoutMs == 0)
        return 0;
    if (id == 0)
    {
        id = g_winTimerNext++;
        if (g_winTimerNext == 0)
            g_winTimerNext = 1;
    }
    else
    {
        KillTimer(hwnd, id);
    }
    for (i = 0; i < WIN32_TIMER_SLOTS; i++)
    {
        if (!g_winTimers[i].used)
        {
            g_winTimers[i].used = 1;
            g_winTimers[i].hwnd = window_from_hwnd(hwnd);
            g_winTimers[i].id = id;
            g_winTimers[i].intervalMs = timeoutMs;
            g_winTimers[i].dueMs = (u32)SDL_GetTicks() + timeoutMs;
            return id;
        }
    }
    return 0;
}

u8 __stdcall KillTimer(GdiWindow *hwnd, u32 id)
{
    int i;

    for (i = 0; i < WIN32_TIMER_SLOTS; i++)
    {
        if (g_winTimers[i].used && g_winTimers[i].id == id &&
            g_winTimers[i].hwnd == window_from_hwnd(hwnd))
        {
            g_winTimers[i].used = 0;
            return 1;
        }
    }
    return 0;
}

static void win32_pump_timers(void)
{
    u32 now = (u32)SDL_GetTicks();
    int i;

    for (i = 0; i < WIN32_TIMER_SLOTS; i++)
    {
        if (g_winTimers[i].used && (s32)(now - g_winTimers[i].dueMs) >= 0)
        {
            GdiMsg m;

            g_winTimers[i].dueMs = now + g_winTimers[i].intervalMs;
            m.hwnd = g_winTimers[i].hwnd;
            m.message = WM_TIMER;
            m.wParam = (uintptr_t)g_winTimers[i].id;
            m.lParam = 0;
            m.time = now;
            m.x = 0;
            m.y = 0;
            msg_queue_push(&m);
        }
    }
    WinmmPumpTimers(now);
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

s32 __cdecl lstrcmpA(const char *a, const char *b)
{
    if (a == NULL || b == NULL)
        return (a == b) ? 0 : ((a == NULL) ? -1 : 1);
    return (s32)strcmp(a, b);
}

void __cdecl EnterCriticalSection(void *cs)
{
    (void)cs;
}

void __cdecl LeaveCriticalSection(void *cs)
{
    (void)cs;
}

void __cdecl InitializeCriticalSection(void *cs)
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
 * reconstruction runs the display entry inline, so the events are inert.
 * They still carry real signalled state for multimedia-timer callbacks. */
#define EVENT_MAGIC 0x45564e54u /* "TnVE" */

struct GdiEvent
{
    u32 magic;
    u8 manualReset;
    u8 signaled;
};

int __cdecl SetEvent(HANDLE ev)
{
    struct GdiEvent *e = (struct GdiEvent *)ev;
    if (e == NULL || e->magic != EVENT_MAGIC)
        return 0;
    e->signaled = 1;
    return 1;
}

int __cdecl ResetEvent(HANDLE ev)
{
    struct GdiEvent *e = (struct GdiEvent *)ev;
    if (e == NULL || e->magic != EVENT_MAGIC)
        return 0;
    e->signaled = 0;
    return 1;
}

u32 __cdecl WaitForSingleObject(HANDLE ev, u32 ms)
{
    struct GdiEvent *e = (struct GdiEvent *)ev;
    (void)ms;
    if (e == NULL || e->magic != EVENT_MAGIC)
        return 0xffffffffu; /* WAIT_FAILED */
    if (e->signaled)
    {
        if (!e->manualReset)
            e->signaled = 0;
        return 0; /* WAIT_OBJECT_0 */
    }
    return 0x00000102u; /* WAIT_TIMEOUT (single-threaded: no blocking) */
}

HANDLE __cdecl CreateEventA(void *sa, int manualReset, int initialState,
                            const char *name)
{
    struct GdiEvent *e;

    (void)sa;
    (void)name;
    e = (struct GdiEvent *)SDL_malloc(sizeof(*e));
    if (e == NULL)
        return NULL;
    e->magic = EVENT_MAGIC;
    e->manualReset = manualReset != 0;
    e->signaled = initialState != 0;
    return (HANDLE)e;
}

void *__cdecl GetProcAddress(HINSTANCE inst, const char *procName)
{
    /* No retail exports exist in this process (all 32-bit code); every
     * lookup misses so callers take their fallback path. */
    (void)inst;
    (void)procName;
    return NULL;
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

u8 __stdcall TranslateMessage(const void *pMsg)
{
    const GdiMsg *m = (const GdiMsg *)pMsg;
    GdiMsg ch;
    u32 vk;
    u32 c = 0;

    if (m == NULL || (m->message != WM_KEYDOWN && m->message != WM_SYSKEYDOWN))
        return 0;
    vk = (u32)m->wParam;
    if (vk >= (u32)'A' && vk <= (u32)'Z')
    {
        SDL_Keymod mods;
        int shift;
        int caps;
        SDL_PumpEvents();
        mods = SDL_GetModState();
        shift = (mods & (SDL_KMOD_SHIFT)) != 0;
        caps = (mods & SDL_KMOD_CAPS) != 0;
        c = (u32)'a' + (vk - (u32)'A');
        if (shift ^ caps)
            c -= (u32)('a' - 'A');
    }
    else if (vk >= (u32)'0' && vk <= (u32)'9')
    {
        static const char shifted[] = ")!@#$%^&*(";
        SDL_Keymod mods;
        SDL_PumpEvents();
        mods = SDL_GetModState();
        if ((mods & (SDL_KMOD_SHIFT)) != 0)
            c = (u32)shifted[vk - (u32)'0'];
        else
            c = vk;
    }
    else if (vk == (u32)VK_SPACE)
    {
        c = (u32)' ';
    }
    if (c == 0)
        return 0;
    ch.hwnd = m->hwnd;
    ch.message = WM_CHAR;
    ch.wParam = (uintptr_t)c;
    ch.lParam = m->lParam;
    ch.time = (u32)SDL_GetTicks();
    ch.x = m->x;
    ch.y = m->y;
    msg_queue_push(&ch);
    return 1;
}

s32 __cdecl DispatchMessageA(GdiWindow *hwnd, u32 msg, uintptr_t wParam,
                             intptr_t lParam)
{
    /* Retail window procedures are 32-bit code and cannot execute in this
     * 64-bit process until matched, so everything routes to the default
     * procedure (exactly what Win32 does with unhandled messages). */
    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

void __stdcall OutputDebugStringA(char* psz)
{
    (void)psz;
}
