#include "flyingdodgem_types.h"

#if defined(FLYINGDODGEM_GDI_COMPAT)
#include "gdi32_compat.h"
#else
/* Reccmp matching build. Keep the resource-API types and the KERNEL32 import
 * declaration local so the wrapper below resolves to __imp__FindResourceA@12
 * (HOVER.EXE IAT slot 0x4ca854) exactly like the retail binary. */
typedef void *HINSTANCE;
typedef void *HRSRC;

#if defined(_MSC_VER)
#pragma comment(lib, "kernel32.lib")
#endif

#if defined(_MSC_VER)
__declspec(dllimport)
#endif
HRSRC __stdcall FindResourceA(HINSTANCE hModule, const char *lpName, const char *lpType);
#endif

// GLOBAL: HOVER 0x004b7f64
uint32_t g_os_version;

// GLOBAL: HOVER 0x004b7f68
uint32_t g_os_win32s;

// FUNCTION: HOVER 0x00453352
HRSRC __stdcall FindResourceCompat(HINSTANCE hModule, const char *lpName, const char *lpType)
{
    HRSRC hres = FindResourceA(hModule, lpName, lpType);
    if (g_os_win32s == 0 && g_os_version < 0x332U && ((uintptr_t)hres & 0x80000000U) != 0)
        return NULL;
    return hres;
}