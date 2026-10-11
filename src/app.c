#include "flyingdodgem_types.h"
#include "app.h"
#include "scene.h"

#include <stdlib.h>
#include <string.h>

/* CBumperApp bootstrap for the HOVER.EXE reconstruction.
 *
 * Retail runs a static initializer (0x426546) that does
 *   mov ecx, 0x460570; call CBumperApp_ctor (0x41c100)
 * which publishes the instance into afxCurrentWinApp (0x4b7cd8 / g_pApp).
 * WinMain (matched) then dispatches InitInstance through vtable+0x30.
 *
 * Full InitInstance (0x41c9e0) loads resources, probes the display and
 * builds the main frame; those paths are not required for the display draw
 * dispatch to see a valid CWorld.  This TU implements the document-template
 * subset of that bootstrap: construct theApp, AddDocTemplate, then create
 * CBumperDoc through its CRuntimeClass so CWorld::CWorld runs and publishes
 * g_460970.  The frame/view create functions are stubs: OpenDocumentFile's
 * frame step is skipped in the native harness. */

extern "C" void* g_pApp;
extern "C" void* g_460970;
extern "C" uint8_t g_460570[];

/* Matched Run (winmain.c, HOVER 0x44aaab). */
extern "C" int __fastcall Run(void* self);

/* ---------------------------------------------------------------------------
 * CRuntimeClass (MFC).  Layout transcribed from the retail blob at 0x4c48d8:
 *   +0x00 lpszClassName, +0x04 nObjectSize, +0x08 wSchema (0xffff),
 *   +0x0c m_pfnCreateObject, +0x10 m_pBaseClass, +0x14 m_pNextClass.
 * The factory is __stdcall and receives the raw allocation (0x44e7f3 calls
 * m_pfnCreateObject(p) after operator new).  0x41e1d0 is:
 *   mov ecx,[esp+4]; test; call CWorld::CWorld; ret 4
 * ------------------------------------------------------------------------- */
struct CRuntimeClass
{
    const char* lpszClassName;
    int32_t     nObjectSize;
    uint32_t    wSchema;
    void (__stdcall* m_pfnCreateObject)(void* p);
    const CRuntimeClass* m_pBaseClass;
    const CRuntimeClass* m_pNextClass;
};

/* Heap CRuntimeClass images for the bootstrap.  The matching globals
 * (g_data_48d8 / 4808 / 4cb0) stay 1-byte stubs so GetTable_* address loads
 * keep rebasing cleanly; the native path needs real size/factory fields. */
static CRuntimeClass* g_pDocClass;
static CRuntimeClass* g_pFrameClass;
static CRuntimeClass* g_pViewClass;

/* CBumperDoc create (retail 0x41e1d0): construct CWorld in place. */
static void __stdcall CreateCBumperDoc(void* p)
{
    if (p != NULL)
        new (p) CWorld;
}

/* CMainFrame / CBumperView factories.  Retail builds full frame/view objects;
 * the native bootstrap only needs the pointers stored on the template so the
 * doc-template list matches InitInstance's shape. */
static void __stdcall CreateCMainFrame(void* p)
{
    (void)p;
}

static void __stdcall CreateCBumperView(void* p)
{
    (void)p;
}

/* ---------------------------------------------------------------------------
 * CObList.  Retail layout (ctor 0x4497e8 / AddTail 0x44994f):
 *   +0x00 vtable, +0x04 pHead, +0x08 pTail, +0x0c nCount,
 *   +0x10 pFree, +0x14 pBlocks, +0x18 nBlockSize.
 * CNode is 12 bytes: { pNext, pPrev, data }.
 * ------------------------------------------------------------------------- */
struct CNode
{
    CNode* pNext;
    CNode* pPrev;
    void*  data;
};

struct CObList
{
    void*    vtable;
    CNode*   pHead;
    CNode*   pTail;
    int32_t  nCount;
    CNode*   pFree;
    void*    pBlocks;
    int32_t  nBlockSize;
};

static void CObList_AddTail(CObList* list, void* data)
{
    CNode* n = (CNode*)calloc(1, sizeof(CNode));
    if (n == NULL)
        return;
    n->data = data;
    n->pNext = NULL;
    n->pPrev = list->pTail;
    if (list->pTail != NULL)
        list->pTail->pNext = n;
    else
        list->pHead = n;
    list->pTail = n;
    list->nCount++;
}

/* CMultiDocTemplate is 100 bytes (malloc(100) in InitInstance).  Only the
 * CDocTemplate fields CreateNewDocument / OpenDocumentFile read are filled. */
enum { kMultiDocTemplateSize = 100 };

static void* CreateMultiDocTemplate(void* pDocClass, void* pFrameClass, void* pViewClass)
{
    uint8_t* t = (uint8_t*)calloc(1, kMultiDocTemplateSize);
    if (t == NULL)
        return NULL;
    /* CDocTemplate ctor (0x453916) + CMultiDocTemplate (0x4503cd):
     *   +0x38 nIDResource, +0x40 pDocClass, +0x44 pFrameClass,
     *   +0x48 pViewClass, +0x60 m_pOnlyDoc = 0. */
    *(uint32_t*)(t + 0x38) = 0x80;
    *(void**)(t + 0x40) = pDocClass;
    *(void**)(t + 0x44) = pFrameClass;
    *(void**)(t + 0x48) = pViewClass;
    *(void**)(t + 0x60) = NULL;
    return t;
}

/* CRuntimeClass::CreateObject subset (0x44e733 + 0x41e1d0): allocate
 * nObjectSize, run the factory (CWorld ctor for CBumperDoc). */
static void* CreateFromRuntimeClass(const CRuntimeClass* cls)
{
    void* p;
    if (cls == NULL || cls->m_pfnCreateObject == NULL)
        return NULL;
    p = calloc(1, (size_t)cls->nObjectSize);
    if (p == NULL)
        return NULL;
    cls->m_pfnCreateObject(p);
    return p;
}

/* ---------------------------------------------------------------------------
 * CBumperApp vtable.  Only the slots WinMain / this bootstrap touch are
 * meaningful; the rest are no-ops so accidental dispatches do not crash.
 * Slot offsets from retail 0x4ba370: +0x30 InitInstance, +0x34 Run,
 * +0x40 ExitInstance, +0x64 InitApplication.
 * ------------------------------------------------------------------------- */
typedef int32_t (__fastcall* AppVFn)(void* self);

static int32_t __fastcall App_InitApplication(void* self)
{
    (void)self;
    return 1; /* retail 0x45ca44: mov eax,1; ret */
}

static int32_t __fastcall App_ExitInstance(void* self)
{
    (void)self;
    return 0;
}

static int32_t __fastcall App_InitInstance(void* self);

/* Padded single-slot views: same pattern as winmain.c so a slot load
 * compiles to `call dword ptr [reg+off]` on MSVC. */
typedef struct { uint8_t pad[0x30]; AppVFn InitInstance; } AppVftInitInstance;
typedef struct { uint8_t pad[0x34]; AppVFn Run; } AppVftRun;
typedef struct { uint8_t pad[0x40]; AppVFn ExitInstance; } AppVftExitInstance;
typedef struct { uint8_t pad[0x64]; AppVFn InitApplication; } AppVftInitApplication;

/* Full vtable image: 0x70 bytes / 28 slots covers +0x64 (InitApplication). */
static void* g_appVtable[0x70 / sizeof(void*)];

static void App_InitVtable(void)
{
    size_t i;
    for (i = 0; i < sizeof(g_appVtable) / sizeof(g_appVtable[0]); i++)
        g_appVtable[i] = (void*)(uintptr_t)App_InitApplication;
    ((AppVftInitInstance*)g_appVtable)->InitInstance = App_InitInstance;
    ((AppVftRun*)g_appVtable)->Run = Run;
    ((AppVftExitInstance*)g_appVtable)->ExitInstance = App_ExitInstance;
    ((AppVftInitApplication*)g_appVtable)->InitApplication = App_InitApplication;
}

static CRuntimeClass* AllocRuntimeClass(const char* name, int32_t size,
    void (__stdcall* factory)(void* p))
{
    CRuntimeClass* cls = (CRuntimeClass*)calloc(1, sizeof(CRuntimeClass));
    if (cls == NULL)
        return NULL;
    cls->lpszClassName = name;
    cls->nObjectSize = size;
    cls->wSchema = 0xffff;
    cls->m_pfnCreateObject = factory;
    return cls;
}

/* InitInstance subset (retail 0x41c9e0, document-template half):
 *   CMultiDocTemplate(0x80, &CBumperDoc, &CMainFrame, &CBumperView)
 *   -> AddDocTemplate (CObList::AddTail at app+0xa0)
 *   -> OpenDocumentFile(NULL, TRUE) -> CreateNewDocument -> CreateObject
 *   -> CWorld::CWorld -> g_460970.
 * Frame creation is skipped; only the document (CWorld) is built. */
static int32_t __fastcall App_InitInstance(void* self)
{
    uint8_t* app = (uint8_t*)self;
    CObList* templates;
    void* pTemplate;

    if (g_pDocClass == NULL)
    {
        static const char kDocName[] = "CBumperDoc";
        static const char kFrameName[] = "CMainFrame";
        static const char kViewName[] = "CBumperView";

        g_pDocClass = AllocRuntimeClass(kDocName, (int32_t)sizeof(CWorld),
            CreateCBumperDoc);
        g_pFrameClass = AllocRuntimeClass(kFrameName, 0x158, CreateCMainFrame);
        g_pViewClass = AllocRuntimeClass(kViewName, 0x55c, CreateCBumperView);
        if (g_pDocClass == NULL || g_pFrameClass == NULL || g_pViewClass == NULL)
            return 0;
    }

    pTemplate = CreateMultiDocTemplate(g_pDocClass, g_pFrameClass, g_pViewClass);
    if (pTemplate == NULL)
        return 0;

    /* AddDocTemplate: CObList at app+0xa0 (head +0xa4, tail +0xa8, count +0xac). */
    templates = (CObList*)(app + 0xa0);
    CObList_AddTail(templates, pTemplate);

    /* OpenDocumentFile(NULL, TRUE) / CreateNewDocument subset. */
    if (CreateFromRuntimeClass(g_pDocClass) == NULL)
        return 0;

    return 1;
}

/* ---------------------------------------------------------------------------
 * Bootstrap entry.  Idempotent: safe from the static initializer and again
 * from main.c before the display loop.
 * ------------------------------------------------------------------------- */
static int s_bootstrapped = 0;

void App_Bootstrap(void)
{
    uint8_t* app;

    if (s_bootstrapped != 0)
        return;
    s_bootstrapped = 1;

    App_InitVtable();

    app = g_460570;
    memset(app, 0, 0x418);
    *(void***)app = g_appVtable;

    /* CObList ctor subset (0x4497e8): block size 10, empty list. */
    {
        CObList* list = (CObList*)(app + 0xa0);
        list->vtable = NULL;
        list->pHead = NULL;
        list->pTail = NULL;
        list->nCount = 0;
        list->pFree = NULL;
        list->pBlocks = NULL;
        list->nBlockSize = 10;
    }

    g_pApp = app;

    /* Drive InitInstance's document half so CWorld exists before the
     * display draw dispatch (DisplayEntry41ece0 case 3 / DrawEntry402250). */
    ((AppVftInitInstance*)g_appVtable)->InitInstance(app);
}

/* Static initializer: retail constructs theApp before WinMain (0x426546). */
#if defined(_MSC_VER) || defined(__GNUC__) || defined(__clang__)
namespace {
struct AppStaticInit
{
    AppStaticInit() { App_Bootstrap(); }
};
static AppStaticInit s_appStaticInit;
}
#endif
