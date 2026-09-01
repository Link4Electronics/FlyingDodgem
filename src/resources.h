#ifndef FLYINGDODGEM_RESOURCES_H
#define FLYINGDODGEM_RESOURCES_H

#include "flyingdodgem_types.h"

#if defined(FLYINGDODGEM_GDI_COMPAT)
#include "gdi32_compat.h"
#else
/* Reccmp matching build: keep the resource-API opaque types local (see
 * resources.c). */
typedef void *HINSTANCE;
typedef void *HRSRC;
#endif

/* FindResourceA wrapper matching HOVER.EXE 0x453352: forwards the three
 * arguments to the imported FindResourceA, then rejects handles whose high
 * bit is set while running on Windows builds older than 3.50 (g_os_version).
 * Used by the DASH loader (HOVER.EXE 0x410518) and exposed here so the native
 * pipeline drives exactly the same resource path as the retail binary. */
// GLOBAL: HOVER 0x004b7f64
extern uint32_t g_os_version;

// GLOBAL: HOVER 0x004b7f68
extern uint32_t g_os_win32s;

// FUNCTION: HOVER 0x00453352
HRSRC __stdcall FindResourceCompat(HINSTANCE hModule, const char *lpName, const char *lpType);

#endif