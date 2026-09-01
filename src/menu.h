#ifndef FLYINGDODGEM_MENU_H
#define FLYINGDODGEM_MENU_H

#include "flyingdodgem_types.h"

/* Win32 menu API layer hosted by the native SDL3 driver (FLYINGDODGEM_GDI_COMPAT).
 *
 * The retail HOVER.EXE builds its main menu bar (Game/Options/Help and the
 * dropdown contents) with the real USER32 menu API: CreatePopupMenu +
 * AppendMenuA to assemble each dropdown, SetMenu to attach the bar to the main
 * window, GetSubMenu/GetMenuItemCount/GetMenuItemID to walk it, and
 * TrackPopupMenu to show a dropdown. This module reproduces that same API so
 * HOVER-style menu code can drive an interactive menu bar exactly the way the
 * original did, rendering it as chrome over the 640x480 presentation.
 *
 * The matching (MSVC 5.0 reccmp) build never compiles this unit.
 */

/* Opaque menu / window handles. A menu handle is a GdiMenu*; an HWND is the
 * single "main window" the menu bar is attached to. */
typedef struct GdiMenu GdiMenu;
typedef void *HWND;

/* AppendMenuA flags (the subset the reconstructed menu uses). */
#define MENU_MF_POPUP     0x00000010u
#define MENU_MF_STRING    0x00000000u
#define MENU_MF_SEPARATOR 0x00000800u
#define MENU_MF_GRAYED    0x00000001u
#define MENU_MF_DISABLED  0x00000002u
#define MENU_MF_CHECKED   0x00000008u
#define MENU_MF_DEFAULT   0x00001000u
#define MENU_MF_ENABLED   0x00000000u

/* TrackPopupMenu flags. */
#define MENU_TPM_LEFTALIGN 0x0000u

/* Top-level popup item id for a popup child (an "owner-drawn" bar menu item). */
#define MENU_POPUP_ID 0xFFFFu

/* --- Menu construction --- */

/* Create an empty popup menu (a dropdown). Returns a handle or NULL. */
GdiMenu * __cdecl CreatePopupMenu(void);

/* Create an empty top-level menu bar. Returns a handle or NULL. */
GdiMenu * __cdecl CreateMenu(void);

/* Append an item to a menu. When flags has MF_POPUP, uIDNewItem is a GdiMenu*
 * handle of the submenu; otherwise it is the numeric command id. Text is copied.
 * Returns true (nonzero) on success. */
u8 __cdecl AppendMenuA(GdiMenu *menu, u32 flags, uintptr_t uIDNewItem, const char *text);

/* Free a menu tree that is no longer attached to the window. */
u8 __cdecl DestroyMenu(GdiMenu *menu);

/* --- Window / bar attachment --- */

/* Fetch the main window handle (created lazily). */
HWND __cdecl GdiMenu_GetMainWindow(void);

/* Attach a top-level menu bar to the main window (SetMenu). Passing NULL
 * detaches it. */
u8 __cdecl SetMenu(HWND hwnd, GdiMenu *menu);

/* Return the menu bar currently attached to the window, or NULL. */
GdiMenu * __cdecl GetMenu(HWND hwnd);

/* Return the submenu (GdiMenu*) of the top-level bar item at popupIndex. */
GdiMenu * __cdecl GetSubMenu(GdiMenu *menu, s32 popupIndex);

/* --- Menu inspection --- */

s32 __cdecl GetMenuItemCount(GdiMenu *menu);
u32 __cdecl GetMenuItemID(GdiMenu *menu, s32 index);
u8 __cdecl SetMenuDefaultItem(GdiMenu *menu, u32 item, u32 bypos);
u8 __cdecl EnableMenuItem(GdiMenu *menu, u32 item, u32 flags);
u8 __cdecl CheckMenuItem(GdiMenu *menu, u32 item, u32 flags);

/* --- Interaction / rendering --- */

/* Show a dropdown below a top-level bar index; returns the selected command
 * id (MF_POPUP children return 0 when their submenu is shown). */
u32 __cdecl TrackPopupMenu(GdiMenu *menu, u32 flags, s32 x, s32 y, HWND hwnd);

/* Pointer (x,y) input: hit-test the bar and dropdowns, drive the open menus.
 * mx,my are in viewport pixels. Returns a command id that was "clicked", or 0
 * if only menu state changed. */
u32 __cdecl GdiMenu_PointerInput(s32 mx, s32 my, u8 pressed, u8 released);

/* Draw the menu bar (and any open dropdown) into the given RGBA32 back
 * buffer, mapping through the shared palette for the 3D frame (already drawn
 * there) which this overlays. */
void __cdecl GdiMenu_DrawOverlay(u8 *dst, u32 dstPitch, u32 dstWidth, u32 dstHeight);

/* Reset any open dropdown (e.g. when a new frame cycle starts). */
void __cdecl GdiMenu_EndModal(void);

#endif
