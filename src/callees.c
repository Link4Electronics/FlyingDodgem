#include "flyingdodgem_types.h"
#include "callees.h"
#include "scene.h"
#if defined(FLYINGDODGEM_GDI_COMPAT)
#include <stdio.h>
#endif

#if defined(_MSC_VER)
#define HOVER_IMPORT __declspec(dllimport)
#else
#define HOVER_IMPORT
#endif
extern "C" HOVER_IMPORT void __stdcall OutputDebugStringA(char* psz);

/* 4-byte packed address fields (see render.c HOVER_LPTR): matching sees the
 * exact original tokens. */
#if defined(FLYINGDODGEM_GDI_COMPAT)
#define HOVER_LPTR(b, o) ((uint8_t*)(uintptr_t)*(uint32_t*)((uint8_t*)(b) + (o)))
#define HOVER_SPTR(b, o, v) (*(uint32_t*)((uint8_t*)(b) + (o)) = (uint32_t)(uintptr_t)(v))
#else
#define HOVER_LPTR(b, o) (*(uint8_t**)((uint8_t*)(b) + (o)))
#define HOVER_SPTR(b, o, v) (*(void**)((uint8_t*)(b) + (o)) = (void*)(v))
#endif

extern "C" MerlinPlane g_4a4498;
extern "C" MerlinPlane g_4a44e0;
extern "C" int16_t g_4a4528;
extern "C" void* g_4a1d60;
extern "C" int32_t g_4a453c;
extern "C" int32_t g_4a1d70;
extern "C" void* g_4a4548;
extern "C" int32_t g_4a4568;
extern "C" int32_t g_4a4530;
extern "C" int32_t g_4a1d64;
extern "C" int32_t g_4a456c;
extern "C" void* g_4a4720;
extern "C" int32_t g_461cfc;
extern "C" int32_t g_4a1d78;
extern "C" void* g_4a471c;
extern "C" int32_t g_4a4544;
extern "C" int32_t g_4a1d6c;
extern "C" char g_4c520c[];
extern "C" int32_t g_4a4534;
extern "C" void* g_4a4490;
extern "C" uint8_t g_4c5208[];
extern "C" void* g_4a4714;
extern "C" void* g_4a1d68;
extern "C" void* g_460970;
extern "C" int32_t g_460750;
extern "C" void** g_4a4844;
extern "C" int32_t g_4a4848;
extern "C" uint8_t g_sineTable[];
extern "C" void __fastcall DrawCell3910(void* self, void* unused, int32_t x, int32_t y, int32_t ignored);
extern "C" void __fastcall SpanSetup2990(void* self, void* unused, void* a1, void* a2, int32_t a3, int32_t a4);
extern "C" void __stdcall F4066b0(void* a1, void* a2, int32_t a3, int32_t a4);

/* Stubs for the functions the four unimplemented scene/display entries call.
 * They are compiled out of line (separate TU from their callers) so the
 * optimizer can never inline them away.  Bodies are empty: reccmp excludes
 * // STUB: entities from both the implemented and the accuracy statistics, the
 * markers only exist so the call operands on both sides resolve to one name.
 *
 * Each stub reproduces the retail stack frame, i.e. the return value of
 * `ret N` after `call`:
 *
 *   - the constructors/destructors belong to the classes declared in scene.h,
 *     so the callers reach them through normal C++ construction and get
 *     __thiscall for free;
 *   - the members of HoverTc take stack arguments on top of `this`;
 *   - Ext* taking a lone `self` are __thiscall with `this` in ECX only, which
 *     __fastcall reproduces without touching EDX;
 *   - Ext* with no `self` are __cdecl, the caller pops the arguments.
 */

// STUB: HOVER 0x00401160
MerlinPlane::MerlinPlane()
{
}
// STUB: HOVER 0x00401210
MerlinPlane::~MerlinPlane()
{
}
// FUNCTION: HOVER 0x004030f0
void __fastcall F4030f0(void* self)
{
    uint8_t* node;
    uint8_t* rec;
    uint8_t* edge;
    uint8_t* other;
    uint8_t* bump;
    int32_t left;
    int32_t right;
    int32_t tmp;
    int32_t width;
    int32_t kk;
    int32_t zero;

#if defined(FLYINGDODGEM_GDI_COMPAT)
    if (g_4a4714 == 0 || g_4a471c == 0)
        return;
    {
        static uint8_t s_bump[0x10000];
        bump = s_bump;
    }
#endif
    g_4a4490 = 0;
    node = (uint8_t*)g_4a4714;
    while (*(uint8_t**)(node + 0x2c) != 0)
        node = *(uint8_t**)(node + 0x2c);
#if !defined(FLYINGDODGEM_GDI_COMPAT)
    bump = (uint8_t*)0x4a1d80;
#endif
    g_4a1d60 = (void*)bump;
    if (node != 0)
    {
        zero = 0;
        do
        {
            *(int32_t*)(node + 0x40) = zero;
            *(int32_t*)(node + 0x4c) = zero;
            *(int32_t*)(node + 0x8c) = zero;
            *(void**)(node + 0x84) = (void*)g_4c5208;
            *(void**)(node + 0x88) = (void*)g_4c5208;
            if (*(int32_t*)(node + 0x0c) != 0)
            {
                uint8_t* e = *(uint8_t**)(node + 0x10);
                int32_t r = *(int16_t*)(e + 0x56);
                int32_t l = *(int16_t*)(e + 0x54);
                int32_t w = r - l;
                int32_t sc;
                int32_t lg;
                int32_t v;
                uint8_t* sp;
                *(int32_t*)(node + 0x60) = r;
                *(int32_t*)(node + 0x64) = l;
                *(int32_t*)(node + 0x68) = w << 16;
                *(int32_t*)(node + 0x6c) = g_4a4544 - r;
                *(int32_t*)(node + 0xa0) = 1;
                sp = *(uint8_t**)(e + 0x50);
                *(int32_t*)(node + 0x70) = g_4a4544 - l;
                *(void**)(node + 0x5c) = (void*)sp;
                if (sp != 0)
                {
                    sc = w / *(int16_t*)(sp + 0x18);
                    lg = 0;
                    v = sc;
                    *(int32_t*)(node + 0x9c) = sc;
                    if (v > 1)
                    {
                        do
                        {
                            lg++;
                            v >>= 1;
                            *(int32_t*)(node + 0x9c) = v;
                        } while (v > 1);
                    }
                    *(int32_t*)(node + 0x9c) = lg;
                }
                else
                {
                    *(int32_t*)(node + 0x18) = 1;
                }
            }
            else
            {
                uint8_t* vw;
                int32_t sc;
                int32_t lg;
                int32_t v;
                uint8_t* sp;
                edge = *(uint8_t**)(node + 0x10);
                rec = (uint8_t*)g_4a4490;
                right = *(int16_t*)(edge + 0xa6);
                *(int32_t*)(node + 0x60) = right;
                left = *(int16_t*)(edge + 0xa4);
                *(int32_t*)(node + 0x64) = left;
                width = right - left;
                *(int32_t*)(node + 0x68) = width << 16;
                *(int32_t*)(node + 0x6c) = g_4a4544 - right;
                *(int32_t*)(node + 0x70) = g_4a4544 - left;
                if (*(int32_t*)(node + 0x18) != 0)
                {
                    vw = (uint8_t*)g_4a1d68;
                    kk = *(int32_t*)(edge + 0x18) - *(int32_t*)(vw + 0x4c);
                    kk *= *(int32_t*)(edge + 0x38);
                    tmp = *(int32_t*)(vw + 0x48) - *(int32_t*)(edge + 0x14);
                    *(int32_t*)(node + 0x14) = zero;
                    tmp *= *(int32_t*)(edge + 0x3c);
                    if (kk + tmp >= 0)
                        *(int32_t*)(node + 0x14) = 1;
                }
                if (*(int32_t*)(node + 0x14) != 0)
                {
                    *(void**)(node + 0x5c) = *(void**)(edge + 0x94);
                    sp = *(uint8_t**)(edge + 0xb0);
                }
                else
                {
                    *(void**)(node + 0x5c) = *(void**)(edge + 0x98);
                    sp = *(uint8_t**)(edge + 0xb0);
                }
                *(void**)(node + 0xa0) = (void*)sp;
                if (*(void**)(node + 0x5c) != 0)
                {
                    sc = width / *(int16_t*)(sp + 0x18);
                    lg = 0;
                    v = sc;
                    *(int32_t*)(node + 0x9c) = sc;
                    if (v > 1)
                    {
                        do
                        {
                            lg++;
                            v >>= 1;
                            *(int32_t*)(node + 0x9c) = v;
                        } while (v > 1);
                    }
                    *(int32_t*)(node + 0x9c) = lg;
                }
                else
                {
                    *(int32_t*)(node + 0x18) = 1;
                }
                if (*(int32_t*)(node + 0x14) != 0)
                {
                    if (g_4a4490 != 0)
                    {
                        do
                        {
                            uint8_t* prev;
                            uint8_t* next;
                            kk = *(int32_t*)(rec + 0x14);
                            if (kk == right && *(int32_t*)(rec + 0x04) == *(int32_t*)(edge + 0x90))
                            {
                                other = *(uint8_t**)(rec + 0x00);
                                if (*(int32_t*)(node + 0x20) != 0)
                                {
                                    prev = *(uint8_t**)(rec + 0x0c);
                                    rec = *(uint8_t**)(rec + 0x08);
                                    if (prev == 0)
                                        g_4a4490 = (void*)rec;
                                    else
                                        *(void**)(prev + 0x08) = (void*)rec;
                                    if (rec == 0)
                                        break;
                                    *(void**)(rec + 0x0c) = (void*)prev;
                                    continue;
                                }
                                if (*(int32_t*)(other + 0x18) == 0 && *(int32_t*)(node + 0x18) == 0)
                                {
                                    if (*(int32_t*)(rec + 0x10) != 0)
                                    {
                                        *(void**)(other + 0x40) = (void*)node;
                                        *(int32_t*)(other + 0x48) = *(int32_t*)(rec + 0x04);
                                        *(int32_t*)(other + 0x44) = 1;
                                        *(int32_t*)(other + 0x8c) = 1;
                                        *(void**)(other + 0x84) = (void*)(node + 0xb4);
                                    }
                                    else
                                    {
                                        *(void**)(other + 0x4c) = (void*)node;
                                        *(int32_t*)(other + 0x54) = *(int32_t*)(rec + 0x04);
                                        *(int32_t*)(other + 0x50) = 1;
                                        *(int32_t*)(other + 0x8c) = 1;
                                        *(void**)(other + 0x88) = (void*)(node + 0xb4);
                                    }
                                    prev = *(uint8_t**)(rec + 0x0c);
                                    rec = *(uint8_t**)(rec + 0x08);
                                    if (prev == 0)
                                        g_4a4490 = (void*)rec;
                                    else
                                        *(void**)(prev + 0x08) = (void*)rec;
                                    if (rec == 0)
                                        break;
                                    *(void**)(rec + 0x0c) = (void*)prev;
                                    continue;
                                }
                            }
                            if (kk == left && *(int32_t*)(rec + 0x04) == *(int32_t*)(edge + 0xa0))
                            {
                                other = *(uint8_t**)(rec + 0x00);
                                if (*(int32_t*)(node + 0x20) != 0)
                                {
                                    prev = *(uint8_t**)(rec + 0x0c);
                                    rec = *(uint8_t**)(rec + 0x08);
                                    if (prev == 0)
                                        g_4a4490 = (void*)rec;
                                    else
                                        *(void**)(prev + 0x08) = (void*)rec;
                                    if (rec == 0)
                                        break;
                                    *(void**)(rec + 0x0c) = (void*)prev;
                                    continue;
                                }
                                if (*(int32_t*)(other + 0x18) == 0 && *(int32_t*)(node + 0x18) == 0)
                                {
                                    if (*(int32_t*)(rec + 0x10) != 0)
                                    {
                                        *(int32_t*)(other + 0x44) = zero;
                                        *(void**)(other + 0x40) = (void*)node;
                                        *(int32_t*)(other + 0x8c) = 1;
                                        *(int32_t*)(other + 0x48) = *(int32_t*)(rec + 0x04);
                                        *(void**)(other + 0x84) = (void*)(node + 0xb8);
                                    }
                                    else
                                    {
                                        *(int32_t*)(other + 0x50) = zero;
                                        *(void**)(other + 0x4c) = (void*)node;
                                        *(int32_t*)(other + 0x8c) = 1;
                                        *(int32_t*)(other + 0x54) = *(int32_t*)(rec + 0x04);
                                        *(void**)(other + 0x88) = (void*)(node + 0xb8);
                                    }
                                    prev = *(uint8_t**)(rec + 0x0c);
                                    rec = *(uint8_t**)(rec + 0x08);
                                    if (prev == 0)
                                        g_4a4490 = (void*)rec;
                                    else
                                        *(void**)(prev + 0x08) = (void*)rec;
                                    if (rec == 0)
                                        break;
                                    *(void**)(rec + 0x0c) = (void*)prev;
                                    continue;
                                }
                                rec = *(uint8_t**)(rec + 0x08);
                            }
                            else
                            {
                                rec = *(uint8_t**)(rec + 0x08);
                            }
                        } while (rec != 0);
                    }
                    if (*(int32_t*)(node + 0x20) == 0)
                    {
                        kk = *(int32_t*)(edge + 0x8c);
                        if (kk != 0)
                        {
                            other = (uint8_t*)g_4a4490;
                            *(void**)(bump + 0x0c) = 0;
                            g_4a4490 = (void*)bump;
                            *(void**)(bump + 0x08) = (void*)other;
                            if (other != 0)
                                *(void**)(other + 0x0c) = (void*)bump;
                            *(int32_t*)(bump + 0x04) = kk;
                            *(void**)(bump + 0x00) = (void*)node;
                            *(int32_t*)(bump + 0x10) = 1;
                            *(int32_t*)(bump + 0x14) = right;
                            bump += 0x18;
                        }
                        other = *(uint8_t**)(edge + 0x9c);
                        if (other != 0 && width > 0)
                        {
                            uint8_t* hd = (uint8_t*)g_4a4490;
                            *(void**)(bump + 0x0c) = 0;
                            g_4a4490 = (void*)bump;
                            *(void**)(bump + 0x08) = (void*)hd;
                            if (hd != 0)
                                *(void**)(hd + 0x0c) = (void*)bump;
                            *(void**)(bump + 0x04) = (void*)other;
                            *(int32_t*)(bump + 0x14) = left;
                            *(void**)(bump + 0x00) = (void*)node;
                            *(int32_t*)(bump + 0x10) = zero;
                            bump += 0x18;
                        }
                    }
                }
                else
                {
                    if (rec != 0)
                    {
                        do
                        {
                            uint8_t* prev;
                            uint8_t* next;
                            kk = *(int32_t*)(rec + 0x14);
                            if (kk == right && *(int32_t*)(rec + 0x04) == *(int32_t*)(edge + 0x8c))
                            {
                                other = *(uint8_t**)(rec + 0x00);
                                if (*(int32_t*)(node + 0x20) != 0)
                                {
                                    prev = *(uint8_t**)(rec + 0x0c);
                                    rec = *(uint8_t**)(rec + 0x08);
                                    if (prev == 0)
                                        g_4a4490 = (void*)rec;
                                    else
                                        *(void**)(prev + 0x08) = (void*)rec;
                                    if (rec == 0)
                                        break;
                                    *(void**)(rec + 0x0c) = (void*)prev;
                                    continue;
                                }
                                if (*(int32_t*)(other + 0x18) == 0 && *(int32_t*)(node + 0x18) == 0)
                                {
                                    if (*(int32_t*)(rec + 0x10) != 0)
                                    {
                                        *(void**)(other + 0x40) = (void*)node;
                                        *(int32_t*)(other + 0x48) = *(int32_t*)(rec + 0x04);
                                        *(int32_t*)(other + 0x44) = 1;
                                        *(int32_t*)(other + 0x8c) = 1;
                                        *(void**)(other + 0x84) = (void*)(node + 0xb4);
                                    }
                                    else
                                    {
                                        *(void**)(other + 0x4c) = (void*)node;
                                        *(int32_t*)(other + 0x54) = *(int32_t*)(rec + 0x04);
                                        *(int32_t*)(other + 0x50) = 1;
                                        *(int32_t*)(other + 0x8c) = 1;
                                        *(void**)(other + 0x88) = (void*)(node + 0xb4);
                                    }
                                    prev = *(uint8_t**)(rec + 0x0c);
                                    rec = *(uint8_t**)(rec + 0x08);
                                    if (prev == 0)
                                        g_4a4490 = (void*)rec;
                                    else
                                        *(void**)(prev + 0x08) = (void*)rec;
                                    if (rec == 0)
                                        break;
                                    *(void**)(rec + 0x0c) = (void*)prev;
                                    continue;
                                }
                            }
                            if (kk == left && *(int32_t*)(rec + 0x04) == *(int32_t*)(edge + 0x9c))
                            {
                                other = *(uint8_t**)(rec + 0x00);
                                if (*(int32_t*)(node + 0x20) != 0)
                                {
                                    prev = *(uint8_t**)(rec + 0x0c);
                                    rec = *(uint8_t**)(rec + 0x08);
                                    if (prev == 0)
                                        g_4a4490 = (void*)rec;
                                    else
                                        *(void**)(prev + 0x08) = (void*)rec;
                                    if (rec == 0)
                                        break;
                                    *(void**)(rec + 0x0c) = (void*)prev;
                                    continue;
                                }
                                if (*(int32_t*)(other + 0x18) == 0 && *(int32_t*)(node + 0x18) == 0)
                                {
                                    if (*(int32_t*)(rec + 0x10) != 0)
                                    {
                                        *(int32_t*)(other + 0x44) = zero;
                                        *(void**)(other + 0x40) = (void*)node;
                                        *(int32_t*)(other + 0x8c) = 1;
                                        *(int32_t*)(other + 0x48) = *(int32_t*)(rec + 0x04);
                                        *(void**)(other + 0x84) = (void*)(node + 0xb8);
                                    }
                                    else
                                    {
                                        *(int32_t*)(other + 0x50) = zero;
                                        *(void**)(other + 0x4c) = (void*)node;
                                        *(int32_t*)(other + 0x8c) = 1;
                                        *(int32_t*)(other + 0x54) = *(int32_t*)(rec + 0x04);
                                        *(void**)(other + 0x88) = (void*)(node + 0xb8);
                                    }
                                    prev = *(uint8_t**)(rec + 0x0c);
                                    rec = *(uint8_t**)(rec + 0x08);
                                    if (prev == 0)
                                        g_4a4490 = (void*)rec;
                                    else
                                        *(void**)(prev + 0x08) = (void*)rec;
                                    if (rec == 0)
                                        break;
                                    *(void**)(rec + 0x0c) = (void*)prev;
                                    continue;
                                }
                                rec = *(uint8_t**)(rec + 0x08);
                            }
                            else
                            {
                                rec = *(uint8_t**)(rec + 0x08);
                            }
                        } while (rec != 0);
                    }
                    if (*(int32_t*)(node + 0x20) == 0)
                    {
                        kk = *(int32_t*)(edge + 0x90);
                        if (kk != 0)
                        {
                            other = (uint8_t*)g_4a4490;
                            *(void**)(bump + 0x0c) = 0;
                            g_4a4490 = (void*)bump;
                            *(void**)(bump + 0x08) = (void*)other;
                            if (other != 0)
                                *(void**)(other + 0x0c) = (void*)bump;
                            *(int32_t*)(bump + 0x04) = kk;
                            *(void**)(bump + 0x00) = (void*)node;
                            *(int32_t*)(bump + 0x10) = 1;
                            *(int32_t*)(bump + 0x14) = right;
                            bump += 0x18;
                        }
                        other = *(uint8_t**)(edge + 0xa0);
                        if (other != 0 && width > 0)
                        {
                            uint8_t* hd = (uint8_t*)g_4a4490;
                            *(void**)(bump + 0x0c) = 0;
                            g_4a4490 = (void*)bump;
                            *(void**)(bump + 0x08) = (void*)hd;
                            if (hd != 0)
                                *(void**)(hd + 0x0c) = (void*)bump;
                            *(void**)(bump + 0x04) = (void*)other;
                            *(int32_t*)(bump + 0x14) = left;
                            *(void**)(bump + 0x00) = (void*)node;
                            *(int32_t*)(bump + 0x10) = zero;
                            bump += 0x18;
                        }
                    }
                }
            }
            node = *(uint8_t**)(node + 0x28);
            g_4a1d60 = (void*)bump;
        } while (node != 0);
    }
    rec = (uint8_t*)g_4a4490;
    if (rec != 0)
    {
        int32_t z = 0;
        int32_t o = 1;
        int32_t m = -1;
        uint8_t* a = (uint8_t*)&g_4a4568;
        do
        {
            other = *(uint8_t**)(rec + 0x00);
            if (*(int32_t*)(other + 0x18) != 0)
            {
                uint8_t* prev = *(uint8_t**)(rec + 0x0c);
                rec = *(uint8_t**)(rec + 0x08);
                if (prev == 0)
                    g_4a4490 = (void*)rec;
                else
                    *(void**)(prev + 0x08) = (void*)rec;
                if (rec == 0)
                    break;
                *(void**)(rec + 0x0c) = (void*)prev;
                continue;
            }
            if (*(int32_t*)(rec + 0x10) != 0)
            {
                *(int32_t*)(other + 0x40) = m;
                *(int32_t*)(other + 0x48) = *(int32_t*)(rec + 0x04);
                *(int32_t*)(other + 0x8c) = o;
                if (*(int32_t*)(rec + 0x14) > g_4a4544)
                {
                    *(int32_t*)(other + 0x44) = o;
                    *(void**)(other + 0x84) = (void*)a;
                }
                else
                {
                    *(void**)(other + 0x84) = (void*)&g_4a4530;
                    *(int32_t*)(other + 0x44) = z;
                }
            }
            else
            {
                *(int32_t*)(other + 0x4c) = m;
                *(int32_t*)(other + 0x54) = *(int32_t*)(rec + 0x04);
                *(int32_t*)(other + 0x8c) = o;
                if (*(int32_t*)(rec + 0x14) > g_4a4544)
                {
                    *(int32_t*)(other + 0x50) = o;
                    *(void**)(other + 0x88) = (void*)a;
                }
                else
                {
                    *(void**)(other + 0x88) = (void*)&g_4a4530;
                    *(int32_t*)(other + 0x50) = z;
                }
            }
            rec = *(uint8_t**)(rec + 0x08);
        } while (rec != 0);
    }
    node = (uint8_t*)g_4a471c;
    if (*(int32_t*)(node + 0x34) != 0)
    {
        uint8_t* ch;
        do
        {
            uint8_t* sib = *(uint8_t**)(node + 0x30);
            ch = *(uint8_t**)(node + 0x34);
            *(void**)(node + 0x38) = (void*)ch;
            *(void**)(node + 0x3c) = (void*)sib;
            node = ch;
        } while (*(int32_t*)(ch + 0x34) != 0);
    }
    {
        uint8_t* s30 = *(uint8_t**)(node + 0x30);
        *(void**)(node + 0x3c) = (void*)s30;
        *(void**)(node + 0x38) = 0;
        if (s30 != 0)
        {
            do
            {
                if (*(int32_t*)(node + 0x8c) != 0)
                    F406a80((void*)node);
                if (*(void**)(node + 0x3c) != 0)
                    node = *(uint8_t**)(node + 0x3c);
            } while (*(void**)(node + 0x3c) != 0);
        }
        *(void**)(node + 0x3c) = 0;
    }
    g_4a4720 = (void*)node;
    *(int32_t*)((uint8_t*)self + 0x28) = 0;
}

// FUNCTION: HOVER 0x00404030
int32_t HoverTc::F404030(void* a1, void* a2, void* a3)
{
    uint8_t* node = (uint8_t*)a1;
    int32_t hi = (int32_t)(intptr_t)a2;

#if defined(FLYINGDODGEM_GDI_COMPAT)
    /* Native: our M402e00 reconstruction can reach here with an empty
     * node list; retail always passes a live node. */
    if (a1 == 0)
        return 0;
#endif
    for (;;)
    {
        if (*(int32_t*)(node + 0x58) == 0)
        {
            node = *(uint8_t**)(node + 0x38);
            if (node == 0)
            {
                ((SpanChain*)(*(uint8_t**)this))->AddPair6d00((int32_t)(intptr_t)a3, hi);
                return hi;
            }
            continue;
        }
        if (*(int32_t*)(node + 0x8c) != 0)
        {
            int32_t c74 = *(int32_t*)(node + 0x74);
            int32_t c78 = *(int32_t*)(node + 0x78);
            int32_t n1 = g_4a453c;
            if (c74 > (int32_t)(intptr_t)a3)
            {
                node = *(uint8_t**)(node + 0x38);
                if (node == 0)
                {
                    ((SpanChain*)(*(uint8_t**)this))->AddPair6d00((int32_t)(intptr_t)a3, hi);
                    return hi;
                }
                continue;
            }
            if (c78 < hi)
            {
                node = *(uint8_t**)(node + 0x38);
                if (node == 0)
                {
                    ((SpanChain*)(*(uint8_t**)this))->AddPair6d00((int32_t)(intptr_t)a3, hi);
                    return hi;
                }
                continue;
            }
            int32_t kx;
            if (c74 > hi)
            {
                uint8_t* ch = *(uint8_t**)(node + 0x38);
                if (ch != 0)
                {
                    int32_t t = c74 - n1;
                    this->F404030(ch, (void*)(intptr_t)hi, (void*)(intptr_t)t);
                }
                else
                {
                    ((SpanChain*)(*(uint8_t**)this))->AddPair6d00(c74 - n1, hi);
                }
                if (c78 >= (int32_t)(intptr_t)a3)
                {
                    this->F402630(node, (void*)(intptr_t)c74, a3);
                    return hi;
                }
                kx = c74;
            }
            else
            {
                if (c78 < hi)
                {
                    node = *(uint8_t**)(node + 0x38);
                    if (node == 0)
                    {
                        ((SpanChain*)(*(uint8_t**)this))->AddPair6d00((int32_t)(intptr_t)a3, hi);
                        return hi;
                    }
                    continue;
                }
                if (c78 > (int32_t)(intptr_t)a3)
                {
                    this->F402630(node, (void*)(intptr_t)hi, a3);
                    return hi;
                }
                kx = hi;
            }
            this->F402630(node, (void*)(intptr_t)kx, (void*)(intptr_t)c78);
            {
                int32_t bb = g_4a453c + c78;
                if (c78 >= bb)
                {
                    ((SpanChain*)(*(uint8_t**)this))->AddPair6d00((int32_t)(intptr_t)a3, bb);
                    return hi;
                }
                node = *(uint8_t**)(node + 0x38);
                if (node == 0)
                {
                    ((SpanChain*)(*(uint8_t**)this))->AddPair6d00((int32_t)(intptr_t)a3, hi);
                    return hi;
                }
            }
            continue;
        }
        else if (*(int32_t*)(node + 0xa0) == 0)
        {
            int32_t bnd = *(int32_t*)(node + 0xb4);
            int32_t bhi = *(int32_t*)(node + 0xb8);
            uint8_t* st = node + 0xac;
            int32_t n3 = g_4a453c;
            int32_t kx;
            if (bnd > (int32_t)(intptr_t)a3)
            {
                node = *(uint8_t**)(node + 0x38);
                if (node == 0)
                {
                    ((SpanChain*)(*(uint8_t**)this))->AddPair6d00((int32_t)(intptr_t)a3, hi);
                    return hi;
                }
                continue;
            }
            if (bhi < hi)
            {
                node = *(uint8_t**)(node + 0x38);
                if (node == 0)
                {
                    ((SpanChain*)(*(uint8_t**)this))->AddPair6d00((int32_t)(intptr_t)a3, hi);
                    return hi;
                }
                continue;
            }
            if (bnd > hi)
            {
                uint8_t* ch = *(uint8_t**)(node + 0x38);
                if (ch != 0)
                {
                    int32_t t = bnd - n3;
                    this->F404030(ch, (void*)(intptr_t)hi, (void*)(intptr_t)t);
                }
                else
                {
                    ((SpanChain*)(*(uint8_t**)this))->AddPair6d00(bnd - n3, hi);
                }
                if (bhi >= (int32_t)(intptr_t)a3)
                {
                    SpanSetup2990(this, NULL, node, st, bnd, (int32_t)(intptr_t)a3);
                    return hi;
                }
                kx = bnd;
            }
            else
            {
                if (bhi < hi)
                {
                    node = *(uint8_t**)(node + 0x38);
                    if (node == 0)
                    {
                        ((SpanChain*)(*(uint8_t**)this))->AddPair6d00((int32_t)(intptr_t)a3, hi);
                        return hi;
                    }
                    continue;
                }
                if (bhi > (int32_t)(intptr_t)a3)
                {
                    SpanSetup2990(this, NULL, node, st, hi, (int32_t)(intptr_t)a3);
                    return hi;
                }
                kx = hi;
            }
            SpanSetup2990(this, NULL, node, st, kx, bhi);
            {
                int32_t bb = g_4a453c + bhi;
                if (bhi >= bb)
                {
                    ((SpanChain*)(*(uint8_t**)this))->AddPair6d00((int32_t)(intptr_t)a3, bb);
                    return hi;
                }
                node = *(uint8_t**)(node + 0x38);
                if (node == 0)
                {
                    ((SpanChain*)(*(uint8_t**)this))->AddPair6d00((int32_t)(intptr_t)a3, hi);
                    return hi;
                }
            }
            continue;
        }
        else
        {
            int32_t c74 = *(int32_t*)(node + 0x74);
            int32_t c78 = *(int32_t*)(node + 0x78);
            int32_t idx;
            int32_t w;
            uint8_t* el;
            int32_t eb;
            int32_t ep;
            int32_t kx;
            int32_t n2;
            if (c74 > (int32_t)(intptr_t)a3)
            {
                node = *(uint8_t**)(node + 0x38);
                if (node == 0)
                {
                    ((SpanChain*)(*(uint8_t**)this))->AddPair6d00((int32_t)(intptr_t)a3, hi);
                    return hi;
                }
                continue;
            }
            if (c78 < hi)
            {
                node = *(uint8_t**)(node + 0x38);
                if (node == 0)
                {
                    ((SpanChain*)(*(uint8_t**)this))->AddPair6d00((int32_t)(intptr_t)a3, hi);
                    return hi;
                }
                continue;
            }
            idx = 0;
            if (*(int32_t*)(node + 0x90) > 0)
            {
                el = *(uint8_t**)(node + 0xa8);
                do
                {
                    n2 = g_4a453c;
                    eb = *(int32_t*)(el + 8);
                    ep = *(int32_t*)(el + 0xc);
                    if (eb > hi)
                    {
                        if (eb > (int32_t)(intptr_t)a3)
                        {
                            node = *(uint8_t**)(node + 0x38);
                            if (node == 0)
                            {
                                ((SpanChain*)(*(uint8_t**)this))->AddPair6d00((int32_t)(intptr_t)a3, hi);
                                return hi;
                            }
                            w = 1;
                            break;
                        }
                        {
                            uint8_t* ch = *(uint8_t**)(node + 0x38);
                            if (ch != 0)
                            {
                                int32_t t = eb - n2;
                                this->F404030(ch, (void*)(intptr_t)hi, (void*)(intptr_t)t);
                            }
                            else
                            {
                                ((SpanChain*)(*(uint8_t**)this))->AddPair6d00(eb - n2, hi);
                            }
                        }
                        if (ep >= (int32_t)(intptr_t)a3)
                        {
                            F4066b0(node, el, eb, (int32_t)(intptr_t)a3);
                            return hi;
                        }
                        kx = eb;
                    }
                    else
                    {
                        if (ep < hi)
                        {
                            node = *(uint8_t**)(node + 0x38);
                            if (node == 0)
                            {
                                ((SpanChain*)(*(uint8_t**)this))->AddPair6d00((int32_t)(intptr_t)a3, hi);
                                return hi;
                            }
                            w = 1;
                            break;
                        }
                        if (ep > (int32_t)(intptr_t)a3)
                        {
                            F4066b0(node, el, hi, (int32_t)(intptr_t)a3);
                            return hi;
                        }
                        kx = hi;
                    }
                    F4066b0(node, el, kx, ep);
                    el += 0x14;
                    idx++;
                } while (idx < *(int32_t*)(node + 0x90));
                if (w)
                    continue;
                {
                    int32_t bb = g_4a453c + ep;
                    if (ep >= bb)
                    {
                        ((SpanChain*)(*(uint8_t**)this))->AddPair6d00((int32_t)(intptr_t)a3, bb);
                        return hi;
                    }
                    node = *(uint8_t**)(node + 0x38);
                    if (node == 0)
                    {
                        ((SpanChain*)(*(uint8_t**)this))->AddPair6d00((int32_t)(intptr_t)a3, hi);
                        return hi;
                    }
                    continue;
                }
            }
            {
                int32_t bb = g_4a453c + ep;
                if (ep >= bb)
                {
                    ((SpanChain*)(*(uint8_t**)this))->AddPair6d00((int32_t)(intptr_t)a3, bb);
                    return hi;
                }
                node = *(uint8_t**)(node + 0x38);
                if (node == 0)
                {
                    ((SpanChain*)(*(uint8_t**)this))->AddPair6d00((int32_t)(intptr_t)a3, hi);
                    return hi;
                }
                continue;
            }
        }
    }
}

// FUNCTION: HOVER 0x00402e00
int32_t HoverTc::M402e00(void* a1)
{
    uint8_t* node;
    int32_t scale;
    int32_t negN;
    int32_t cnt;
#if defined(FLYINGDODGEM_GDI_COMPAT)
    static int s_hits = 0;
    if ((++s_hits == 1 || s_hits % 600 == 0))
        fprintf(stderr, "M402e00 hit #%d\n", s_hits);
#endif

    (void)a1;
    g_4a1d60 = (void*)0x4a1d80;
    if (*(int32_t*)((uint8_t*)this + 0x28) != 0)
        F4030f0(this);
    node = (uint8_t*)g_4a471c;
    while (node != NULL)
    {
        int32_t depth = *(int16_t*)(node + 8);
        if (*(int32_t*)(node + 0x18) != 0)
        {
            if (depth != 0)
            {
                *(int32_t*)(node + 0x58) = 0;
                node = *(uint8_t**)(node + 0x34);
                continue;
            }
        }
        else if (depth <= 0)
        {
            if (depth != 0)
            {
                *(int32_t*)(node + 0x58) = 0;
                node = *(uint8_t**)(node + 0x34);
                continue;
            }
        }
        else
        {
            uint8_t* st = node + 0xac;
            int32_t row;
            int32_t col;
            scale = *(int32_t*)(uintptr_t)(uint32_t)g_4a1d6c / depth;
            *(int32_t*)(node + 0x58) = 1;
            row = (*(int32_t*)(node + 0x6c) * scale >> 10) + g_4a1d70;
            col = (*(int32_t*)(node + 0x70) * scale >> 10) + g_4a1d70;
            negN = -g_4a453c;
            row &= negN;
            col &= negN;
            *(int32_t*)(st + 8) = row;
            *(int32_t*)(st + 0xc) = col;
            if (*(int32_t*)(node + 0xa0) != 0)
            {
                int32_t shade;
                uint8_t* child;
                uint8_t* edge;
                int32_t tex;
                int32_t sh;
                int32_t rowbase;
                int32_t* rp;
                int32_t mask;
                int32_t tbl;
                int32_t rec;
                int32_t span;
                int32_t den;
                if (g_4a1d78 == 0)
                    shade = g_461cfc;
                else
                {
                    int32_t lim;
                    shade = ((uint8_t*)g_4a4548)[(depth >> (*(int32_t*)(node + 0x9c) + 4)) & 0x7ff];
                    child = *(uint8_t**)(node + 0x5c);
                    lim = *(int32_t*)(child + 0x2c) - 1;
                    if (lim < shade)
                        shade = lim;
                    if (g_461cfc > shade)
                        shade = g_461cfc;
                }
                edge = *(uint8_t**)(node + 0x10);
                edge[0x24] |= 8;
                child = *(uint8_t**)(node + 0x5c);
                tex = *(int16_t*)(node + 0xa);
                sh = (uint8_t)(*(int32_t*)(node + 0x9c) + shade);
                rowbase = *(int32_t*)(child + 0x28) + shade * 32;
                rp = (int32_t*)(uintptr_t)(uint32_t)rowbase;
                mask = (int16_t)(tex >> sh) & *rp;
                tbl = *(int32_t*)((uint8_t*)rp + 0x1c);
                rec = *(int32_t*)(uintptr_t)(uint32_t)(tbl + mask * 8 + 4);
                span = *(int32_t*)(uintptr_t)(uint32_t)(rec + 4);
                den = span - row;
                if (den != 0)
                    *(int32_t*)(node + 0x98) = *(int32_t*)(node + 0x68) / den;
                else
                    *(int32_t*)(node + 0x98) = 0;
                {
                    int16_t n = *(int16_t*)(uintptr_t)(uint32_t)(rec + 0);
                    uint8_t* bump = (uint8_t*)g_4a1d60;
                    g_4a1d60 = (void*)(bump + n * 20);
                    if (n != 0)
                    {
                        int32_t* arr = (int32_t*)(uintptr_t)(uint32_t)*(int32_t*)(uintptr_t)(uint32_t)(rec + 4);
                        cnt = n;
                        while (cnt > 0)
                        {
                            int32_t vx = *(int16_t*)(arr + 0) << sh;
                            int32_t vy = *(int16_t*)(arr + 1) << sh;
                            vx -= *(int32_t*)(node + 0x60);
                            vy -= *(int32_t*)(node + 0x60);
                            *(int32_t*)bump = vx;
                            *(int32_t*)(bump + 4) = vy;
                            if (vx > *(int32_t*)(node + 0x64))
                                *(int32_t*)(node + 0x64) = vx;
                            vx = *(int32_t*)(arr + 1);
                            arr = (int32_t*)((uint8_t*)arr + 8);
                            *(int32_t*)(bump + 0x10) = vx;
                            bump += 0x14;
                            cnt--;
                        }
                    }
                }
                node = *(uint8_t**)(node + 0x34);
                continue;
            }
            *(int32_t*)(node + 0xa4) = 0;
            if (*(int32_t*)(node + 0x8c) != 0)
            {
                int32_t a = *(int32_t*)(*(uint8_t**)(node + 0x84));
                int32_t b = *(int32_t*)(*(uint8_t**)(node + 0x88));
                int32_t mn = a;
                int32_t mx;
                int32_t r0 = row;
                int32_t r1 = col;
                if (b < a)
                    mn = b;
                mx = mn;
                if (r0 <= mx)
                    mx = r0;
                else
                    mn = r0;
                *(int32_t*)(node + 0x74) = mx;
                if (b != 0xfff0000 && b > r1)
                    mn = b;
                else
                    mn = r1;
                if (a == 0xfff0000 || r1 <= a)
                    mx = r1;
                else
                    mx = a;
                *(int32_t*)(node + 0x78) = mx;
                mn = mn;
            }
            node = *(uint8_t**)(node + 0x34);
            continue;
        }
        {
            uint8_t* st = node + 0xac;
            *(int32_t*)(st + 8) = *(int32_t*)(node + 0x6c) > 0 ? g_4a4530 : g_4a4568;
            *(int32_t*)(st + 0xc) = *(int32_t*)(node + 0x70) > 0 ? g_4a4530 : g_4a4568;
            *(int32_t*)(node + 0x58) = 0;
        }
        node = *(uint8_t**)(node + 0x34);
    }
    this->F404030(g_4a4720, (void*)(uintptr_t)(uint32_t)g_4a1d64, (void*)(uintptr_t)(uint32_t)g_4a456c);
    ((SpanChain*)*(void**)this)->F403a60(this);
    if (g_4a1d60 > (void*)0x4a40a8)
        OutputDebugStringA(g_4c520c);
    return 0;
}

// FUNCTION: HOVER 0x00403a60
void SpanChain::F403a60(void* a1)
{
    uint8_t* ctx = (uint8_t*)this;
    uint8_t* hover = (uint8_t*)a1;
    uint8_t* src;
    uint8_t* dst;
    int32_t r0;
    int32_t r1;
    int32_t c0;
    int32_t c1;
    int32_t n;

    (void)hover;
    src = HOVER_LPTR(ctx, 8);
    dst = HOVER_LPTR(ctx, 4);
#if defined(FLYINGDODGEM_GDI_COMPAT)
    if (src == NULL || dst == NULL)
        return;
#endif
    n = *(int32_t*)(src + 0x14);
#if defined(FLYINGDODGEM_GDI_COMPAT)
    if (n < 0 || n > 0x28)
        return;
    /* Native scratch-bounds TBD (dst underflows base-4 on first frame);
     * skip pair copies natively (X-tables stay zeroed → fills park at x=0,
     * wrong-but-safe) until base tracking lands. Matching executes fully. */
    n = 0;
#endif
    while (n > 0)
    {
#if defined(FLYINGDODGEM_GDI_COMPAT)
        uint8_t* tp = HOVER_LPTR(src, 0);
        int16_t v0;
        int16_t v1;
        if (tp == NULL)
            break;
        v0 = *(int16_t*)tp;
        v1 = *(int16_t*)(tp + 2);
#else
        int16_t v0 = *(int16_t*)(*(uint8_t**)src + 0);
        int16_t v1 = *(int16_t*)(*(uint8_t**)src + 2);
#endif
        dst -= 4;
        *(int16_t*)(dst + 0) = v0;
        *(int16_t*)(dst + 2) = v1;
        src += 4;
        n--;
    }
    r0 = 0;
    c0 = 0;
    src = HOVER_LPTR(HOVER_LPTR(ctx, 8), 0);
    while (r0 < *(int32_t*)(src + 0x18))
    {
        if (c0 >= *(int32_t*)(src + 0x14))
            break;
        {
            int16_t x0 = *(int16_t*)dst;
            int16_t x1 = *(int16_t*)(*(uint8_t**)src);
            if (x0 > x1)
            {
                int16_t y0 = *(int16_t*)(*(uint8_t**)src + 2);
                if (x0 <= y0)
                {
                    if (x0 >= y0)
                        F406740(this, NULL, *(int16_t*)(*(uint8_t**)(src + 0x10) + x0 * 2), x0);
                    else
                        F406740(this, NULL, *(int16_t*)(*(uint8_t**)(src + 0x10) + x0 * 2), x0);
                }
                else
                    F406740(this, NULL, *(int16_t*)(*(uint8_t**)(src + 0x10) + x0 * 2), x0);
            }
        }
        c0++;
    }
    {
        uint8_t* nd = HOVER_LPTR(ctx, 0xc);
        while (nd != NULL)
        {
            int32_t y = *(int32_t*)(nd + 0x1c);
            int32_t z = *(int32_t*)(nd + 0x24);
            ((SpanChain*)ctx)->F402b10(nd, y, 0, z);
            nd = *(uint8_t**)(nd + 0x28);
        }
    }
    {
        uint8_t* q0 = HOVER_LPTR(ctx, 0xc);
        uint8_t* q1 = HOVER_LPTR(ctx, 8);
        n = *(int32_t*)(ctx + 0x14);
        HOVER_SPTR(ctx, 0xc, q1);
        HOVER_SPTR(ctx, 8, q0);
        *(int32_t*)(ctx + 0x18) = n;
        *(int32_t*)(ctx + 0x14) = 0;
    }
}
// FUNCTION: HOVER 0x004053a0
int32_t HoverTc::M4053a0(void* a1, void* a2)
{
    uint8_t* display = (uint8_t*)a1;
    uint8_t* node;
    uint8_t* next;
    uint8_t* zero;
    (void)a2;

    node = *(uint8_t**)((uint8_t*)g_460970 + 0x8390);
    if (node != 0)
    {
        zero = 0;
        do
        {
            next = *(uint8_t**)node;
            this->F401c00(*(void**)(node + 8), *(void**)(display + 0x540));
            node = next;
        } while (node != zero);
    }
    node = *(uint8_t**)((uint8_t*)g_460970 + 0x83ac);
    if (node != 0)
    {
        zero = 0;
        do
        {
            next = *(uint8_t**)node;
            this->F401c00(*(void**)(node + 8), *(void**)(display + 0x540));
            node = next;
        } while (node != zero);
    }
    if (g_460750 != 0 && g_4a4848 > 0)
    {
        int32_t off = 0;
        int32_t n = g_4a4848;
        do
        {
            uint8_t* obj = *(uint8_t**)((uint8_t*)g_4a4844 + off);
            if (*(int32_t*)(obj + 0x3c) != 0)
            {
                uint8_t* cell = obj + 0x9c;
                uint8_t color = *(uint8_t*)(obj + 0x40);
                uint16_t w = color;
                uint16_t v = (w << 8) | w;
                uint32_t dd = v;
                uint32_t ee = (dd << 16) | dd;
                *(uint8_t*)((uint8_t*)this + 0x1c) = color;
                *(int32_t*)((uint8_t*)this + 0x20) = (int32_t)ee;
                DrawCell3910(this, NULL, *(int32_t*)cell, *(int32_t*)(cell + 4), 5);
                if (*(int32_t*)(obj + 0xa8) != 0)
                {
                    int32_t xa = *(int32_t*)cell - 0xa;
                    int32_t xb = *(int32_t*)cell + 0xa;
                    int32_t ya = *(int32_t*)(cell + 4) - 0xa;
                    int32_t yb = *(int32_t*)(cell + 4) + 0xa;
                    this->F401980(xa, ya, xb, ya);
                    this->F401980(xa, yb, xb, yb);
                    yb += 0x14;
                    xa -= 0x14;
                    this->F401980(xa, ya, xb, yb);
                    this->F401980(xa, yb, xb, yb);
                }
                {
                    int32_t nn = *(int32_t*)(obj + 0x8c);
                    if (nn > 0)
                    {
                        int32_t kk = 0;
                        uint8_t* base = *(uint8_t**)(obj + 0x88);
                        do
                        {
                            uint8_t* child;
                            kk += 4;
                            child = *(uint8_t**)(base + kk - 4);
                            this->F401980(*(int32_t*)cell, *(int32_t*)(cell + 4), *(int32_t*)(child + 0x9c), *(int32_t*)(child + 0x9c + 4));
                            nn--;
                        } while (nn != 0);
                    }
                }
            }
            off += 4;
            n--;
        } while (n != 0);
    }
    /* Retail falls off here (garbage EAX, ignored by the caller); MSVC 4
     * errors out on a missing return (C2561), so return explicitly. */
    return 0;
}
// FUNCTION: HOVER 0x00405a10
int32_t HoverTc::M405a10(void* a1, void* a2)
{
    int32_t c;
    int32_t w24;
    int32_t w20;
    int32_t idx1;
    int32_t idx2;
    uint8_t* pb;
    uint8_t* pd;
    int32_t v30;
    int32_t v10;
    int32_t k64;
    int32_t k58;
    int32_t k50;
    int32_t v28;
    int32_t v1c;
    int32_t v0c;
    int32_t k48;
    int32_t k3c;
    int32_t v14;
    int32_t k34;

    c = -0x80 - *(int32_t*)((uint8_t*)a1 + 0xdc);
    w24 = *(int32_t*)((uint8_t*)a2 + 0x8338);
    w20 = *(int32_t*)((uint8_t*)a2 + 0x833c);
    idx1 = (c + 0x80) & 0x1ff;
    idx2 = c & 0x1ff;
    pb = g_sineTable + idx1 * 64;
    pd = g_sineTable + idx2 * 64;
    v30 = *(int32_t*)pd;
    v10 = *(int32_t*)pb;
    k64 = w20;
    k58 = w24;
    k50 = *(int32_t*)((uint8_t*)this + 0x44);
    v30 = (int32_t)(((int64_t)v30 * k64) >> 16);
    v10 = (int32_t)(((int64_t)v10 * k58) >> 16);
    v28 = v10 - v30;
    v28 = (int32_t)(((int64_t)v28 * k50) >> 16);
    *(int32_t*)((uint8_t*)this + 0x5c) = ((int16_t*)&v28)[1];
    v1c = *(int32_t*)pd;
    v0c = *(int32_t*)pb;
    k48 = w24;
    k3c = w20;
    k34 = *(int32_t*)((uint8_t*)this + 0x48);
    v1c = (int32_t)(((int64_t)v1c * k48) >> 16);
    v0c = (int32_t)(((int64_t)v0c * k3c) >> 16);
    v14 = v0c + v1c;
    v14 = (int32_t)(((int64_t)v14 * k34) >> 16);
    *(int32_t*)((uint8_t*)this + 0x60) = ((int16_t*)&v14)[1];
    return ((int16_t*)&v14)[1];
}



// STUB: HOVER 0x00407430
void __fastcall Ext407430(void* self)
{
}
// STUB: HOVER 0x0040cce0
int32_t HoverTc::M40cce0(void* a1, void* a2, void* a3, void* a4)
{
    return 0;
}
// STUB: HOVER 0x0040d150
CBeaconBase::CBeaconBase()
{
}
// STUB: HOVER 0x0040d220
CBeaconBase::~CBeaconBase()
{
}
// STUB: HOVER 0x0040d340
void __fastcall Ext40d340(void* self)
{
}
// STUB: HOVER 0x00410510
void* __fastcall Ext410510(void* self)
{
    return 0;
}
// STUB: HOVER 0x00413c10
int32_t HoverTc::M413c10(void* a1)
{
    return 0;
}
// STUB: HOVER 0x004184b0
int32_t HoverTc::M4184b0(void* a1, void* a2)
{
    return 0;
}
// STUB: HOVER 0x0041e1e0
CThingDerived::~CThingDerived()
{
}
// STUB: HOVER 0x00421fa0
CWorldSpan::CWorldSpan()
{
}
// STUB: HOVER 0x00422050
CWorldSpan::~CWorldSpan()
{
}
// STUB: HOVER 0x00423cd0
void* __cdecl Ext423cd0(void)
{
    return 0;
}
// STUB: HOVER 0x00424210
CBeaconSub::~CBeaconSub()
{
}
// STUB: HOVER 0x00425050
CWorldSide::CWorldSide()
{
}
// STUB: HOVER 0x004250c0
CWorldSide::~CWorldSide()
{
}
// STUB: HOVER 0x00427a80
int32_t HoverTc::M427a80(void* a1)
{
    return 0;
}
// STUB: HOVER 0x00427bb0
void* __fastcall Ext427bb0(void* self)
{
    return 0;
}
// STUB: HOVER 0x00427cc0
void* __fastcall Ext427cc0(void* self)
{
    return 0;
}
// STUB: HOVER 0x004283f0
void* __fastcall Ext4283f0(void* self)
{
    return 0;
}
// STUB: HOVER 0x0043e2cf
void* __cdecl Ext43e2cf(uint32_t cb)
{
    return 0;
}
// STUB: HOVER 0x0043f978
void* __cdecl Ext43f978(const char* psz, const char* hits, int32_t cch)
{
    return 0;
}
// STUB: HOVER 0x004491e4
CThing::CThing(int32_t nBlockSize)
{
}
// STUB: HOVER 0x00449277
CThing::~CThing()
{
}
// STUB: HOVER 0x004494cc
CBeaconSubBase::CBeaconSubBase()
{
}
// STUB: HOVER 0x00449534
CBeaconSubBase::~CBeaconSubBase()
{
}
// STUB: HOVER 0x00449598
int32_t HoverTc::M449598(int32_t a1, int32_t a2)
{
    return 0;
}
// STUB: HOVER 0x0044994f
int32_t HoverTc::M44994f(void* a1)
{
    return 0;
}
// STUB: HOVER 0x0044a003
int32_t HoverTc::M44a003(void* a1, void* a2, void* a3)
{
    return 0;
}
// STUB: HOVER 0x0044b986
CBeaconKey::CBeaconKey()
{
}
// STUB: HOVER 0x0044b9f4
CBeaconKey::~CBeaconKey()
{
}
// STUB: HOVER 0x0044bdef
int32_t CBeaconKey::M44bdef(uint32_t nID)
{
    return 0;
}
// STUB: HOVER 0x00451590
CBaseWorld::CBaseWorld()
{
}
// STUB: HOVER 0x0045164d
CBaseWorld::~CBaseWorld()
{
}
