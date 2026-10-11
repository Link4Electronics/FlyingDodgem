#ifndef FLYINGDODGEM_APP_H
#define FLYINGDODGEM_APP_H

#include "flyingdodgem_types.h"

/* CBumperApp bootstrap (retail constructs theApp at 0x460570 from the static
 * init block, then WinMain dispatches InitInstance / Run / ExitInstance
 * through its vtable).  App_Bootstrap is idempotent and is also invoked from
 * a C++ static initializer so g_pApp is valid before main; the native SDL3
 * harness may call it again before the display loop.  On success g_460970
 * holds the CWorld built through the CBumperDoc CRuntimeClass. */
#ifdef __cplusplus
extern "C" {
#endif
void App_Bootstrap(void);
#ifdef __cplusplus
}
#endif

#endif /* FLYINGDODGEM_APP_H */
