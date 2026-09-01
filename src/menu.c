#include "menu.h"

#include <SDL3/SDL.h>

#include <string.h>
#include <stdio.h>

/* ---------------------------------------------------------------------------
 * Win32 menu API layer over SDL3 (native-only, FLYINGDODGEM_GDI_COMPAT).
 *
 * The retail HOVER.EXE builds its menu bar ("Game / Options / Help" and their
 * dropdown menus) with the real USER32 menu API: CreatePopupMenu + AppendMenuA
 * to assemble each dropdown, SetMenu to attach the bar, and TrackPopupMenu to
 * pop a dropdown open. This module reproduces exactly that API so HOVER-style
 * code can drive an interactive menu bar the same way, rendering it as chrome
 * composited over the 640x480 presentation in GdiMenu_DrawOverlay.
 *
 * Menu handles are GdiMenu*. The single "main window" HWND owns the bar. The
 * bar height is MENUBAR_H pixels; each top-level popup is one bar button.
 * ------------------------------------------------------------------------- */

#define MENU_MAGIC 0x4d454e55u /* "UMEM" */

#define MENUBAR_H  20
#define PAD_X      6
#define ITEM_H     20
#define DROPDOWN_W 190

#define COL_BAR_BG     { 0x3c, 0x3c, 0x3c, 255 }
#define COL_BAR_TOP    { 0x00, 0x00, 0x00, 255 }
#define COL_HOT_BG     { 0x5a, 0x5a, 0x5a, 255 }
#define COL_TEXT       { 0xe0, 0xe0, 0xe0, 255 }
#define COL_MENU_BG    { 0xf0, 0xf0, 0xf0, 255 }
#define COL_MENU_TEXT  { 0x20, 0x20, 0x20, 255 }
#define COL_HILITE     { 0x31, 0x63, 0xc5, 255 }
#define COL_HILITE_TXT { 0xff, 0xff, 0xff, 255 }
#define COL_BORDER     { 0x20, 0x20, 0x20, 255 }
#define COL_DISABLED   { 0xa0, 0xa0, 0xa0, 255 }

typedef struct MenuItem
{
    char *text;
    u32 flags;
    uintptr_t id;    /* command id, unless MF_POPUP */
    GdiMenu *submenu; /* non-NULL when MF_POPUP */
} MenuItem;

struct GdiMenu
{
    u32 magic;
    u8 isPopup;        /* dropdown vs top-level bar */
    u32 count;
    MenuItem *items;   /* dynamic array */
    /* popup state */
    u32 hotTop;        /* bar button index being hovered (0xFFFFFFFF none) */
    u8 open;           /* a dropdown is open */
    u32 openIndex;     /* bar index whose dropdown is open */
    s32 openX;
    s32 openY;
    s32 openTopIndex;  /* hovered item within open dropdown */
};

typedef struct
{
    u8 r, g, b, a;
} Pixel;

static HWND g_mainWindow = (HWND)1;
static GdiMenu *g_bar = NULL;

/* --- colour helpers ------------------------------------------------------ */

#define COLOR(name) static const Pixel g_##name = COL_##name
COLOR(BAR_BG);
COLOR(BAR_TOP);
COLOR(HOT_BG);
COLOR(TEXT);
COLOR(MENU_BG);
COLOR(MENU_TEXT);
COLOR(HILITE);
COLOR(HILITE_TXT);
COLOR(BORDER);
COLOR(DISABLED);

/* --- tiny 8x8 font (indexed by ASCII) ------------------------------------ */

#define F_SCAN(ch) ((ch) - 32)
static const u8 g_font[][8] = {
    /* 32 space */ {0,0,0,0,0,0,0,0},
    /* 33 ! */ {0x18,0x18,0x18,0x18,0x18,0,0x18,0},
    /* 34 " */ {0x66,0x66,0x66,0,0,0,0,0},
    /* 35 # */ {0x6c,0x6c,0xfe,0x6c,0xfe,0x6c,0x6c,0},
    /* 36 $ */ {0x18,0x3c,0x62,0x30,0x0c,0x46,0x3c,0x18},
    /* 37 % */ {0x66,0x66,0x0c,0x18,0x30,0x66,0x66,0},
    /* 38 & */ {0x38,0x6c,0x6c,0x38,0x6d,0x66,0x3b,0},
    /* 39 ' */ {0x18,0x18,0x30,0,0,0,0,0},
    /* 40 ( */ {0x0c,0x18,0x30,0x30,0x30,0x18,0x0c,0},
    /* 41 ) */ {0x30,0x18,0x0c,0x0c,0x0c,0x18,0x30,0},
    /* 42 * */ {0,0x66,0x3c,0xff,0x3c,0x66,0,0},
    /* 43 + */ {0,0,0x18,0x7e,0x18,0,0,0},
    /* 44 , */ {0,0,0,0,0,0x18,0x18,0x30},
    /* 45 - */ {0,0,0,0x7e,0,0,0,0},
    /* 46 . */ {0,0,0,0,0,0x18,0x18,0},
    /* 47 / */ {0x06,0x0c,0x0c,0x18,0x30,0x30,0x60,0},
    /* 48 0 */ {0x7e,0xc3,0xc3,0xc3,0xc3,0xc3,0x7e,0},
    /* 49 1 */ {0x18,0x38,0x18,0x18,0x18,0x18,0x7e,0},
    /* 50 2 */ {0x3c,0x66,0x06,0x0c,0x18,0x30,0x7e,0},
    /* 51 3 */ {0x3c,0x66,0x06,0x1c,0x06,0x66,0x3c,0},
    /* 52 4 */ {0x0c,0x1c,0x3c,0x6c,0x7e,0x0c,0x0c,0},
    /* 53 5 */ {0x7e,0x60,0x60,0x3c,0x06,0x06,0x7c,0},
    /* 54 6 */ {0x3c,0x60,0x60,0x7c,0x66,0x66,0x3c,0},
    /* 55 7 */ {0x7e,0x06,0x06,0x0c,0x18,0x30,0x30,0},
    /* 56 8 */ {0x3c,0x66,0x66,0x3c,0x66,0x66,0x3c,0},
    /* 57 9 */ {0x3c,0x66,0x66,0x3e,0x06,0x06,0x3c,0},
    /* 58 : */ {0,0,0x18,0x18,0,0x18,0x18,0},
    /* 59 ; */ {0,0,0x18,0x18,0,0x18,0x18,0x30},
    /* 60 < */ {0x0c,0x18,0x30,0x60,0x30,0x18,0x0c,0},
    /* 61 = */ {0,0,0x7e,0,0,0x7e,0,0},
    /* 62 > */ {0x30,0x18,0x0c,0x06,0x0c,0x18,0x30,0},
    /* 63 ? */ {0x3c,0x66,0x06,0x0c,0x18,0,0x18,0},
    /* 64 @ */ {0x7c,0xc6,0xce,0xde,0xcc,0xc0,0x78,0},
    /* 65 A */ {0x38,0x6c,0xc6,0xc6,0xfe,0xc6,0xc6,0},
    /* 66 B */ {0xfc,0x66,0x66,0x7c,0x66,0x66,0xfc,0},
    /* 67 C */ {0x3c,0x66,0xc0,0xc0,0xc0,0x66,0x3c,0},
    /* 68 D */ {0xf8,0x6c,0x66,0x66,0x66,0x6c,0xf8,0},
    /* 69 E */ {0xfe,0x62,0x68,0x78,0x68,0x62,0xfe,0},
    /* 70 F */ {0xfe,0x62,0x68,0x78,0x68,0x60,0xf0,0},
    /* 71 G */ {0x3c,0x66,0xc0,0xc0,0xce,0x66,0x3a,0},
    /* 72 H */ {0xc6,0xc6,0xc6,0xfe,0xc6,0xc6,0xc6,0},
    /* 73 I */ {0x7e,0x18,0x18,0x18,0x18,0x18,0x7e,0},
    /* 74 J */ {0x3e,0x0c,0x0c,0x0c,0x0c,0x6c,0x38,0},
    /* 75 K */ {0xc6,0xcc,0xd8,0xf0,0xd8,0xcc,0xc6,0},
    /* 76 L */ {0xf0,0x60,0x60,0x60,0x62,0x66,0xfe,0},
    /* 77 M */ {0xc6,0xee,0xfe,0xfe,0xd6,0xc6,0xc6,0},
    /* 78 N */ {0xc6,0xe6,0xf6,0xde,0xce,0xc6,0xc6,0},
    /* 79 O */ {0x38,0x6c,0xc6,0xc6,0xc6,0x6c,0x38,0},
    /* 80 P */ {0xfc,0x66,0x66,0x7c,0x60,0x60,0xf0,0},
    /* 81 Q */ {0x38,0x6c,0xc6,0xc6,0xd6,0x6c,0x3a,0},
    /* 82 R */ {0xfc,0x66,0x66,0x7c,0x6c,0x66,0xe6,0},
    /* 83 S */ {0x3c,0x66,0x30,0x18,0x0c,0x66,0x3c,0},
    /* 84 T */ {0x7e,0x5a,0x18,0x18,0x18,0x18,0x3c,0},
    /* 85 U */ {0xc6,0xc6,0xc6,0xc6,0xc6,0xc6,0x7c,0},
    /* 86 V */ {0xc6,0xc6,0xc6,0xc6,0xc6,0x6c,0x38,0},
    /* 87 W */ {0xc6,0xc6,0xd6,0xd6,0xd6,0xd6,0x7c,0},
    /* 88 X */ {0xc6,0xc6,0x6c,0x38,0x6c,0xc6,0xc6,0},
    /* 89 Y */ {0xc6,0xc6,0x6c,0x38,0x18,0x18,0x3c,0},
    /* 90 Z */ {0xfe,0xc6,0x0c,0x18,0x30,0x62,0xfe,0},
    /* 91 [ */ {0x3c,0x30,0x30,0x30,0x30,0x30,0x3c,0},
    /* 92 \ */ {0x60,0x30,0x30,0x18,0x0c,0x0c,0x06,0},
    /* 93 ] */ {0x3c,0x0c,0x0c,0x0c,0x0c,0x0c,0x3c,0},
    /* 94 ^ */ {0x18,0x3c,0x66,0,0,0,0,0},
    /* 95 _ */ {0,0,0,0,0,0,0xff,0},
    /* 96 ` */ {0x30,0x18,0x0c,0,0,0,0,0},
    /* 97 a */ {0,0,0x3c,0x06,0x3e,0x66,0x3e,0},
    /* 98 b */ {0xe0,0x60,0x7c,0x66,0x66,0x66,0x7c,0},
    /* 99 c */ {0,0,0x3c,0x60,0x60,0x60,0x3c,0},
    /* 100 d */ {0x0e,0x06,0x3e,0x66,0x66,0x66,0x3e,0},
    /* 101 e */ {0,0,0x3c,0x66,0x7e,0x60,0x3c,0},
    /* 102 f */ {0x1c,0x36,0x30,0x78,0x30,0x30,0x78,0},
    /* 103 g */ {0,0,0x3e,0x66,0x66,0x3e,0x06,0x3c},
    /* 104 h */ {0xe0,0x60,0x6c,0x76,0x66,0x66,0xe6,0},
    /* 105 i */ {0x18,0,0x38,0x18,0x18,0x18,0x3c,0},
    /* 106 j */ {0x0c,0,0x1c,0x0c,0x0c,0x0c,0x6c,0x38},
    /* 107 k */ {0xe0,0x60,0x66,0x6c,0x78,0x6c,0xe6,0},
    /* 108 l */ {0x38,0x18,0x18,0x18,0x18,0x18,0x3c,0},
    /* 109 m */ {0,0,0x6c,0xfe,0xd6,0xc6,0xc6,0},
    /* 110 n */ {0,0,0x7c,0x66,0x66,0x66,0x66,0},
    /* 111 o */ {0,0,0x3c,0x66,0x66,0x66,0x3c,0},
    /* 112 p */ {0,0,0x7c,0x66,0x66,0x7c,0x60,0xf0},
    /* 113 q */ {0,0,0x3e,0x66,0x66,0x3e,0x06,0x0f},
    /* 114 r */ {0,0,0x6c,0x76,0x60,0x60,0xf0,0},
    /* 115 s */ {0,0,0x3e,0x60,0x3c,0x06,0x7c,0},
    /* 116 t */ {0x30,0x30,0x78,0x30,0x30,0x36,0x1c,0},
    /* 117 u */ {0,0,0x66,0x66,0x66,0x66,0x3e,0},
    /* 118 v */ {0,0,0xc6,0xc6,0xc6,0x6c,0x38,0},
    /* 119 w */ {0,0,0xc6,0xc6,0xd6,0xfe,0x6c,0},
    /* 120 x */ {0,0,0xc6,0x6c,0x38,0x6c,0xc6,0},
    /* 121 y */ {0,0,0xc6,0xc6,0xc6,0x7e,0x06,0x3c},
    /* 122 z */ {0,0,0x7e,0x0c,0x18,0x30,0x7e,0},
    /* 123 { */ {0x0e,0x18,0x18,0x70,0x18,0x18,0x0e,0},
    /* 124 | */ {0x18,0x18,0x18,0x18,0x18,0x18,0x18,0},
    /* 125 } */ {0x70,0x18,0x18,0x0e,0x18,0x18,0x70,0},
    /* 126 ~ */ {0x6c,0x92,0x24,0,0,0,0,0},
};

static int font_index(char c)
{
    int i = (int)(u8)c - 32;
    if (i < 0 || i > 94)
        return 0;
    return i;
}

static u32 text_width(const char *s)
{
    const char *p = s;
    u32 w = 0;
    if (p == NULL)
        return 0;
    while (*p)
    {
        w += (u32)(font_index(*p) == F_SCAN('i') || font_index(*p) == F_SCAN('l') ||
                   font_index(*p) == F_SCAN('j') || font_index(*p) == F_SCAN('t') ?
               5 : 7);
        p++;
    }
    return w > 0 ? w : 1;
}

/* Draw a single glyph of an 8x8 font, clipped to the surface. */
static void draw_text(u8 *dst, u32 pitch, u32 w, u32 h,
                      s32 x, s32 y, const char *str, Pixel col)
{
    if (dst == NULL || str == NULL)
        return;
    const char *p = str;
    s32 cx = x;
    while (*p)
    {
        int gi = font_index(*p);
        int gw = (gi == F_SCAN('i') || gi == F_SCAN('l') ||
                  gi == F_SCAN('j') || gi == F_SCAN('t')) ? 5 : 7;
        int r, c;
        for (r = 0; r < 8; r++)
        {
            u8 rowbits = g_font[gi][r];
            for (c = 0; c < 8; c++)
            {
                if (rowbits & (0x80 >> c))
                {
                    s32 px = cx + c;
                    s32 py = y + r;
                    if (px >= 0 && px < (s32)w && py >= 0 && py < (s32)h)
                    {
                        u8 *out = dst + (u64)py * pitch + (u64)px * 4u;
                        out[0] = col.r;
                        out[1] = col.g;
                        out[2] = col.b;
                        out[3] = col.a;
                    }
                }
            }
        }
        cx += gw;
        p++;
    }
}

static void fill_rect(u8 *dst, u32 pitch, u32 w, u32 h,
                      s32 x, s32 y, s32 rw, s32 rh, Pixel col)
{
    s32 r, c;
    if (dst == NULL)
        return;
    for (r = 0; r < rh; r++)
    {
        s32 py = y + r;
        if (py < 0 || py >= (s32)h)
            continue;
        for (c = 0; c < rw; c++)
        {
            s32 px = x + c;
            if (px < 0 || px >= (s32)w)
                continue;
            u8 *out = dst + (u64)py * pitch + (u64)px * 4u;
            out[0] = col.r;
            out[1] = col.g;
            out[2] = col.b;
            out[3] = col.a;
        }
    }
}

/* --- lifecycle ----------------------------------------------------------- */

static GdiMenu *menu_alloc(u8 isPopup)
{
    GdiMenu *m = (GdiMenu *)SDL_malloc(sizeof(GdiMenu));
    if (m == NULL)
        return NULL;
    m->magic = MENU_MAGIC;
    m->isPopup = isPopup;
    m->count = 0;
    m->items = NULL;
    m->hotTop = 0xFFFFFFFFu;
    m->open = 0;
    m->openIndex = 0;
    m->openX = 0;
    m->openY = 0;
    m->openTopIndex = 0xFFFFFFFFu;
    return m;
}

GdiMenu * __cdecl CreatePopupMenu(void)
{
    return menu_alloc(1);
}

GdiMenu * __cdecl CreateMenu(void)
{
    return menu_alloc(0);
}

u8 __cdecl AppendMenuA(GdiMenu *menu, u32 flags, uintptr_t uIDNewItem, const char *text)
{
    if (menu == NULL || menu->magic != MENU_MAGIC)
        return 0;
    MenuItem *ni = (MenuItem *)SDL_realloc(menu->items,
                                           (menu->count + 1) * sizeof(MenuItem));
    if (ni == NULL)
        return 0;
    menu->items = ni;
    MenuItem *it = &menu->items[menu->count];
    memset(it, 0, sizeof(*it));
    it->flags = flags;
    it->id = uIDNewItem;
    it->submenu = NULL;
    if (flags & MENU_MF_POPUP)
        it->submenu = (GdiMenu *)uIDNewItem;
    if (text != NULL)
    {
        const char *amp = strrchr(text, '&');
        size_t n = strlen(text);
        if (amp != NULL)
            n = n - 1; /* drop the '&' accelerator marker */
        it->text = (char *)SDL_malloc(n + 1);
        if (it->text == NULL)
            return 0;
        {
            size_t i, k = 0;
            const char *p = text;
            for (i = 0; i < strlen(text); i++)
            {
                if (p[i] == '&')
                    continue;
                it->text[k++] = p[i];
            }
            it->text[k] = 0;
        }
    }
    menu->count++;
    return 1;
}

static void menu_free_inner(GdiMenu *m)
{
    if (m == NULL)
        return;
    u32 i;
    for (i = 0; i < m->count; i++)
    {
        if (m->items[i].submenu != NULL)
            menu_free_inner(m->items[i].submenu);
        SDL_free(m->items[i].text);
    }
    SDL_free(m->items);
    m->magic = 0;
    SDL_free(m);
}

u8 __cdecl DestroyMenu(GdiMenu *menu)
{
    if (menu != NULL && menu->magic == MENU_MAGIC)
    {
        menu_free_inner(menu);
        return 1;
    }
    return 0;
}

/* --- window / bar attachment --------------------------------------------- */

HWND __cdecl GdiMenu_GetMainWindow(void)
{
    return g_mainWindow;
}

u8 __cdecl SetMenu(HWND hwnd, GdiMenu *menu)
{
    (void)hwnd;
    g_bar = menu;
    return 1;
}

GdiMenu * __cdecl GetMenu(HWND hwnd)
{
    (void)hwnd;
    return g_bar;
}

GdiMenu * __cdecl GetSubMenu(GdiMenu *menu, s32 popupIndex)
{
    if (menu == NULL || menu->magic != MENU_MAGIC)
        return NULL;
    if (popupIndex < 0 || (u32)popupIndex >= menu->count)
        return NULL;
    return menu->items[popupIndex].submenu;
}

s32 __cdecl GetMenuItemCount(GdiMenu *menu)
{
    if (menu == NULL || menu->magic != MENU_MAGIC)
        return -1;
    return (s32)menu->count;
}

u32 __cdecl GetMenuItemID(GdiMenu *menu, s32 index)
{
    if (menu == NULL || menu->magic != MENU_MAGIC)
        return 0;
    if (index < 0 || (u32)index >= menu->count)
        return 0;
    return (u32)menu->items[index].id;
}

static MenuItem *find_by_id(GdiMenu *menu, u32 item, u32 bypos, s32 *outIndex)
{
    u32 i;
    if (menu == NULL || menu->magic != MENU_MAGIC)
        return NULL;
    for (i = 0; i < menu->count; i++)
    {
        if (menu->items[i].submenu != NULL)
            continue;
        if (bypos ? (i == (u32)item) : ((u32)menu->items[i].id == item))
        {
            if (outIndex != NULL)
                *outIndex = (s32)i;
            return &menu->items[i];
        }
    }
    return NULL;
}

u8 __cdecl SetMenuDefaultItem(GdiMenu *menu, u32 item, u32 bypos)
{
    MenuItem *it = find_by_id(menu, item, bypos, NULL);
    if (it == NULL)
        return 0;
    it->flags |= MENU_MF_DEFAULT;
    return 1;
}

u8 __cdecl EnableMenuItem(GdiMenu *menu, u32 item, u32 flags)
{
    MenuItem *it = find_by_id(menu, item, 0, NULL);
    if (it == NULL)
        return 0;
    if (flags & MENU_MF_GRAYED)
        it->flags |= MENU_MF_GRAYED;
    else
        it->flags &= ~MENU_MF_GRAYED;
    return 1;
}

u8 __cdecl CheckMenuItem(GdiMenu *menu, u32 item, u32 flags)
{
    MenuItem *it = find_by_id(menu, item, 0, NULL);
    if (it == NULL)
        return 0;
    if (flags & MENU_MF_CHECKED)
        it->flags |= MENU_MF_CHECKED;
    else
        it->flags &= ~MENU_MF_CHECKED;
    return 1;
}

/* --- bar geometry --------------------------------------------------------- */

/* Working area: the bar is drawn over the top MENUBAR_H rows of the frame. */

static s32 bar_x_for(GdiMenu *bar, u32 index)
{
    u32 i;
    s32 x = 0;
    for (i = 0; i < index && i < bar->count; i++)
    {
        const MenuItem *it = &bar->items[i];
        x += (s32)text_width(it->text != NULL ? it->text : "") + PAD_X * 2;
    }
    return x;
}

static s32 bar_item_width(const MenuItem *it, s32 dropdownWidth)
{
    (void)dropdownWidth;
    return (s32)text_width(it->text != NULL ? it->text : "") + PAD_X * 2;
}

/* Return the top-level bar index under viewport x, or 0xFFFFFFFF if none. */
static u32 bar_hit(GdiMenu *bar, s32 x, s32 y)
{
    u32 i;
    if (bar == NULL || bar->magic != MENU_MAGIC)
        return 0xFFFFFFFFu;
    if (y < 0 || y >= MENUBAR_H)
        return 0xFFFFFFFFu;
    for (i = 0; i < bar->count; i++)
    {
        s32 bx = bar_x_for(bar, i);
        s32 bw = bar_item_width(&bar->items[i], 0);
        if (x >= bx && x < bx + bw)
            return i;
    }
    return 0xFFFFFFFFu;
}

/* Hit-test an item row inside an open dropdown. Returns index or 0xFFFFFFFF. */
static s32 drop_hit(GdiMenu *drop, s32 x, s32 y)
{
    s32 row = (s32)(y - drop->openY) / ITEM_H;
    if (row < 0 || (u32)row >= drop->count)
        return -1;
    if (x < drop->openX || x >= drop->openX + DROPDOWN_W)
        return -1;
    return row;
}

/* --- interaction ---------------------------------------------------------- */

u32 __cdecl TrackPopupMenu(GdiMenu *menu, u32 flags, s32 x, s32 y, HWND hwnd)
{
    (void)flags;
    (void)x;
    (void)y;
    (void)hwnd;
    /* In our model the dropdown stays open and is driven by GdiMenu_PointerInput
     * and the mouse click that happens over it; return 0 (no immediate command).
     * The caller opens the dropdown by setting open state on its bar via the
     * pointer-input path. */
    (void)menu;
    return 0;
}

void __cdecl GdiMenu_EndModal(void)
{
    if (g_bar != NULL)
    {
        g_bar->open = 0;
        g_bar->openTopIndex = 0xFFFFFFFFu;
    }
}

/* --- pointer input -------------------------------------------------------- */

u32 __cdecl GdiMenu_PointerInput(s32 mx, s32 my, u8 pressed, u8 released)
{
    u32 result = 0;
    if (g_bar == NULL || g_bar->magic != MENU_MAGIC)
        return 0;

    u32 top = bar_hit(g_bar, mx, my);

    if (g_bar->open)
    {
        GdiMenu *drop = NULL;
        if (g_bar->openIndex < g_bar->count)
            drop = g_bar->items[g_bar->openIndex].submenu;
        if (drop != NULL)
        {
            s32 row = drop_hit(drop, mx, my);
            /* hovering a different top-level bar button while a dropdown is open
             * switches the open dropdown. */
            if (top != 0xFFFFFFFFu && top != g_bar->openIndex)
            {
                g_bar->openIndex = top;
                g_bar->openX = bar_x_for(g_bar, top);
                g_bar->openY = MENUBAR_H;
                g_bar->openTopIndex = 0xFFFFFFFFu;
                return 0;
            }
            if (row >= 0 && (u32)row < drop->count)
            {
                g_bar->openTopIndex = (u32)row;
                if (pressed && released)
                {
                    MenuItem *it = &drop->items[row];
                    if ((it->flags & MENU_MF_GRAYED) == 0 &&
                        it->submenu == NULL)
                    {
                        if (it->flags & MENU_MF_CHECKED)
                            it->flags &= ~MENU_MF_CHECKED; /* toggle */
                        else
                            it->flags |= MENU_MF_CHECKED;
                        result = (u32)it->id;
                        g_bar->open = 0;
                        g_bar->openTopIndex = 0xFFFFFFFFu;
                    }
                }
            }
            else
            {
                g_bar->openTopIndex = 0xFFFFFFFFu;
                if (pressed && released && top == 0xFFFFFFFFu)
                {
                    /* click outside closes the dropdown */
                    g_bar->open = 0;
                    g_bar->openTopIndex = 0xFFFFFFFFu;
                }
            }
        }
        return result;
    }

    /* bar closed */
    if (pressed && released)
    {
        if (top != 0xFFFFFFFFu && top < g_bar->count &&
            g_bar->items[top].submenu != NULL)
        {
            g_bar->open = 1;
            g_bar->openIndex = top;
            g_bar->openX = bar_x_for(g_bar, top);
            g_bar->openY = MENUBAR_H;
            g_bar->openTopIndex = 0xFFFFFFFFu;
        }
    }
    else
    {
        g_bar->hotTop = top;
    }
    return 0;
}

/* --- rendering ------------------------------------------------------------ */

void __cdecl GdiMenu_DrawOverlay(u8 *dst, u32 dstPitch, u32 dstWidth, u32 dstHeight)
{
    if (g_bar == NULL || g_bar->magic != MENU_MAGIC || dst == NULL)
        return;

    /* Menu bar background spanning the top MENUBAR_H rows. */
    fill_rect(dst, dstPitch, dstWidth, dstHeight, 0, 0,
              (s32)dstWidth, MENUBAR_H, g_BAR_BG);
    fill_rect(dst, dstPitch, dstWidth, dstHeight, 0, MENUBAR_H - 1,
              (s32)dstWidth, 1, g_BAR_TOP);

    u32 i;
    for (i = 0; i < g_bar->count; i++)
    {
        MenuItem *it = &g_bar->items[i];
        s32 bx = bar_x_for(g_bar, i);
        s32 bw = bar_item_width(it, 0);
        Pixel tc = g_TEXT;
        if (g_bar->open && g_bar->openIndex == i)
        {
            fill_rect(dst, dstPitch, dstWidth, dstHeight, bx, 0, bw, MENUBAR_H, g_HOT_BG);
        }
        else if (!g_bar->open && g_bar->hotTop == i)
        {
            fill_rect(dst, dstPitch, dstWidth, dstHeight, bx, 0, bw, MENUBAR_H, g_HOT_BG);
        }
        draw_text(dst, dstPitch, dstWidth, dstHeight,
                  bx + PAD_X, (MENUBAR_H - 8) / 2,
                  it->text != NULL ? it->text : "", tc);
    }

    /* Open dropdown. */
    if (g_bar->open && g_bar->openIndex < g_bar->count)
    {
        GdiMenu *drop = g_bar->items[g_bar->openIndex].submenu;
        if (drop != NULL && drop->magic == MENU_MAGIC)
        {
            s32 ddw = DROPDOWN_W;
            s32 ddh = (s32)drop->count * ITEM_H + 2;
            s32 dx = g_bar->openX;
            s32 dy = g_bar->openY;
            fill_rect(dst, dstPitch, dstWidth, dstHeight,
                      dx, dy, ddw, ddh, g_MENU_BG);
            fill_rect(dst, dstPitch, dstWidth, dstHeight,
                      dx, dy, 1, ddh, g_BORDER);
            fill_rect(dst, dstPitch, dstWidth, dstHeight,
                      dx + ddw - 1, dy, 1, ddh, g_BORDER);
            fill_rect(dst, dstPitch, dstWidth, dstHeight,
                      dx, dy + ddh - 1, ddw, 1, g_BORDER);
            u32 r;
            for (r = 0; r < drop->count; r++)
            {
                MenuItem *mit = &drop->items[r];
                s32 iy = dy + (s32)r * ITEM_H;
                Pixel tc = g_MENU_TEXT;
                if (mit->submenu != NULL || (mit->flags & MENU_MF_SEPARATOR))
                {
                    tc = g_DISABLED;
                }
                bool hilite = ((u32)g_bar->openTopIndex == r) &&
                              !(mit->flags & MENU_MF_GRAYED);
                if (hilite)
                {
                    fill_rect(dst, dstPitch, dstWidth, dstHeight,
                              dx + 1, iy, ddw - 2, ITEM_H, g_HILITE);
                    tc = g_HILITE_TXT;
                }
                if (mit->flags & MENU_MF_GRAYED)
                    tc = g_DISABLED;
                if (mit->flags & MENU_MF_SEPARATOR)
                {
                    fill_rect(dst, dstPitch, dstWidth, dstHeight,
                              dx + 6, iy + ITEM_H / 2, ddw - 12, 1, g_BORDER);
                    continue;
                }
                s32 tx = dx + PAD_X;
                if (mit->flags & MENU_MF_CHECKED)
                {
                    /* draw a small check mark */
                    static const Pixel ck = { 0, 0, 0, 255 };
                    draw_text(dst, dstPitch, dstWidth, dstHeight,
                              tx, iy + (ITEM_H - 8) / 2, "\xE2\x9C\x93", hilite ? g_HILITE_TXT : ck);
                    tx += 14;
                }
                if (mit->submenu != NULL)
                {
                    /* right-aligned chevron */
                    draw_text(dst, dstPitch, dstWidth, dstHeight,
                              dx + ddw - PAD_X - 7, iy + (ITEM_H - 8) / 2, ">", tc);
                }
                draw_text(dst, dstPitch, dstWidth, dstHeight,
                          tx, iy + (ITEM_H - 8) / 2,
                          mit->text != NULL ? mit->text : "", tc);
            }
        }
    }
}
