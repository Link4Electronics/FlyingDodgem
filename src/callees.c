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
extern "C" HOVER_IMPORT int32_t __stdcall GetClientRect(void* hwnd, void* rect);

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
extern "C" uint8_t g_4a1d80[];
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
extern "C" int32_t g_4c4cc8[8];
extern "C" int32_t g_460998;
extern "C" uint8_t g_4609a0[];
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
    while (HOVER_LPTR(node, 0x2c) != 0)
        node = HOVER_LPTR(node, 0x2c);
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
            HOVER_SPTR(node, 0x84, g_4c5208);
            HOVER_SPTR(node, 0x88, g_4c5208);
            if (*(int32_t*)(node + 0x0c) != 0)
            {
                uint8_t* e = HOVER_LPTR(node, 0x10);
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
                sp = HOVER_LPTR(e, 0x50);
                *(int32_t*)(node + 0x70) = g_4a4544 - l;
                HOVER_SPTR(node, 0x5c, sp);
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
                edge = HOVER_LPTR(node, 0x10);
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
                    HOVER_SPTR(node, 0x5c, HOVER_LPTR(edge, 0x94));
                    sp = HOVER_LPTR(edge, 0xb0);
                }
                else
                {
                    HOVER_SPTR(node, 0x5c, HOVER_LPTR(edge, 0x98));
                    sp = HOVER_LPTR(edge, 0xb0);
                }
                HOVER_SPTR(node, 0xa0, sp);
                if (HOVER_LPTR(node, 0x5c) != 0)
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
                                other = HOVER_LPTR(rec, 0x00);
                                if (*(int32_t*)(node + 0x20) != 0)
                                {
                                    prev = HOVER_LPTR(rec, 0x0c);
                                    rec = HOVER_LPTR(rec, 0x08);
                                    if (prev == 0)
                                        g_4a4490 = (void*)rec;
                                    else
                                        HOVER_SPTR(prev, 0x08, rec);
                                    if (rec == 0)
                                        break;
                                    HOVER_SPTR(rec, 0x0c, prev);
                                    continue;
                                }
                                if (*(int32_t*)(other + 0x18) == 0 && *(int32_t*)(node + 0x18) == 0)
                                {
                                    if (*(int32_t*)(rec + 0x10) != 0)
                                    {
                                        HOVER_SPTR(other, 0x40, node);
                                        *(int32_t*)(other + 0x48) = *(int32_t*)(rec + 0x04);
                                        *(int32_t*)(other + 0x44) = 1;
                                        *(int32_t*)(other + 0x8c) = 1;
                                        HOVER_SPTR(other, 0x84, node + 0xb4);
                                    }
                                    else
                                    {
                                        HOVER_SPTR(other, 0x4c, node);
                                        *(int32_t*)(other + 0x54) = *(int32_t*)(rec + 0x04);
                                        *(int32_t*)(other + 0x50) = 1;
                                        *(int32_t*)(other + 0x8c) = 1;
                                        HOVER_SPTR(other, 0x88, node + 0xb4);
                                    }
                                    prev = HOVER_LPTR(rec, 0x0c);
                                    rec = HOVER_LPTR(rec, 0x08);
                                    if (prev == 0)
                                        g_4a4490 = (void*)rec;
                                    else
                                        HOVER_SPTR(prev, 0x08, rec);
                                    if (rec == 0)
                                        break;
                                    HOVER_SPTR(rec, 0x0c, prev);
                                    continue;
                                }
                            }
                            if (kk == left && *(int32_t*)(rec + 0x04) == *(int32_t*)(edge + 0xa0))
                            {
                                other = HOVER_LPTR(rec, 0x00);
                                if (*(int32_t*)(node + 0x20) != 0)
                                {
                                    prev = HOVER_LPTR(rec, 0x0c);
                                    rec = HOVER_LPTR(rec, 0x08);
                                    if (prev == 0)
                                        g_4a4490 = (void*)rec;
                                    else
                                        HOVER_SPTR(prev, 0x08, rec);
                                    if (rec == 0)
                                        break;
                                    HOVER_SPTR(rec, 0x0c, prev);
                                    continue;
                                }
                                if (*(int32_t*)(other + 0x18) == 0 && *(int32_t*)(node + 0x18) == 0)
                                {
                                    if (*(int32_t*)(rec + 0x10) != 0)
                                    {
                                        *(int32_t*)(other + 0x44) = zero;
                                        HOVER_SPTR(other, 0x40, node);
                                        *(int32_t*)(other + 0x8c) = 1;
                                        *(int32_t*)(other + 0x48) = *(int32_t*)(rec + 0x04);
                                        HOVER_SPTR(other, 0x84, node + 0xb8);
                                    }
                                    else
                                    {
                                        *(int32_t*)(other + 0x50) = zero;
                                        HOVER_SPTR(other, 0x4c, node);
                                        *(int32_t*)(other + 0x8c) = 1;
                                        *(int32_t*)(other + 0x54) = *(int32_t*)(rec + 0x04);
                                        HOVER_SPTR(other, 0x88, node + 0xb8);
                                    }
                                    prev = HOVER_LPTR(rec, 0x0c);
                                    rec = HOVER_LPTR(rec, 0x08);
                                    if (prev == 0)
                                        g_4a4490 = (void*)rec;
                                    else
                                        HOVER_SPTR(prev, 0x08, rec);
                                    if (rec == 0)
                                        break;
                                    HOVER_SPTR(rec, 0x0c, prev);
                                    continue;
                                }
                                rec = HOVER_LPTR(rec, 0x08);
                            }
                            else
                            {
                                rec = HOVER_LPTR(rec, 0x08);
                            }
                        } while (rec != 0);
                    }
                    if (*(int32_t*)(node + 0x20) == 0)
                    {
                        kk = *(int32_t*)(edge + 0x8c);
                        if (kk != 0)
                        {
                            other = (uint8_t*)g_4a4490;
                            HOVER_SPTR(bump, 0x0c, NULL);
                            g_4a4490 = (void*)bump;
                            HOVER_SPTR(bump, 0x08, other);
                            if (other != 0)
                                HOVER_SPTR(other, 0x0c, bump);
                            *(int32_t*)(bump + 0x04) = kk;
                            HOVER_SPTR(bump, 0x00, node);
                            *(int32_t*)(bump + 0x10) = 1;
                            *(int32_t*)(bump + 0x14) = right;
                            bump += 0x18;
                        }
                        other = HOVER_LPTR(edge, 0x9c);
                        if (other != 0 && width > 0)
                        {
                            uint8_t* hd = (uint8_t*)g_4a4490;
                            HOVER_SPTR(bump, 0x0c, NULL);
                            g_4a4490 = (void*)bump;
                            HOVER_SPTR(bump, 0x08, hd);
                            if (hd != 0)
                                HOVER_SPTR(hd, 0x0c, bump);
                            HOVER_SPTR(bump, 0x04, other);
                            *(int32_t*)(bump + 0x14) = left;
                            HOVER_SPTR(bump, 0x00, node);
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
                                other = HOVER_LPTR(rec, 0x00);
                                if (*(int32_t*)(node + 0x20) != 0)
                                {
                                    prev = HOVER_LPTR(rec, 0x0c);
                                    rec = HOVER_LPTR(rec, 0x08);
                                    if (prev == 0)
                                        g_4a4490 = (void*)rec;
                                    else
                                        HOVER_SPTR(prev, 0x08, rec);
                                    if (rec == 0)
                                        break;
                                    HOVER_SPTR(rec, 0x0c, prev);
                                    continue;
                                }
                                if (*(int32_t*)(other + 0x18) == 0 && *(int32_t*)(node + 0x18) == 0)
                                {
                                    if (*(int32_t*)(rec + 0x10) != 0)
                                    {
                                        HOVER_SPTR(other, 0x40, node);
                                        *(int32_t*)(other + 0x48) = *(int32_t*)(rec + 0x04);
                                        *(int32_t*)(other + 0x44) = 1;
                                        *(int32_t*)(other + 0x8c) = 1;
                                        HOVER_SPTR(other, 0x84, node + 0xb4);
                                    }
                                    else
                                    {
                                        HOVER_SPTR(other, 0x4c, node);
                                        *(int32_t*)(other + 0x54) = *(int32_t*)(rec + 0x04);
                                        *(int32_t*)(other + 0x50) = 1;
                                        *(int32_t*)(other + 0x8c) = 1;
                                        HOVER_SPTR(other, 0x88, node + 0xb4);
                                    }
                                    prev = HOVER_LPTR(rec, 0x0c);
                                    rec = HOVER_LPTR(rec, 0x08);
                                    if (prev == 0)
                                        g_4a4490 = (void*)rec;
                                    else
                                        HOVER_SPTR(prev, 0x08, rec);
                                    if (rec == 0)
                                        break;
                                    HOVER_SPTR(rec, 0x0c, prev);
                                    continue;
                                }
                            }
                            if (kk == left && *(int32_t*)(rec + 0x04) == *(int32_t*)(edge + 0x9c))
                            {
                                other = HOVER_LPTR(rec, 0x00);
                                if (*(int32_t*)(node + 0x20) != 0)
                                {
                                    prev = HOVER_LPTR(rec, 0x0c);
                                    rec = HOVER_LPTR(rec, 0x08);
                                    if (prev == 0)
                                        g_4a4490 = (void*)rec;
                                    else
                                        HOVER_SPTR(prev, 0x08, rec);
                                    if (rec == 0)
                                        break;
                                    HOVER_SPTR(rec, 0x0c, prev);
                                    continue;
                                }
                                if (*(int32_t*)(other + 0x18) == 0 && *(int32_t*)(node + 0x18) == 0)
                                {
                                    if (*(int32_t*)(rec + 0x10) != 0)
                                    {
                                        *(int32_t*)(other + 0x44) = zero;
                                        HOVER_SPTR(other, 0x40, node);
                                        *(int32_t*)(other + 0x8c) = 1;
                                        *(int32_t*)(other + 0x48) = *(int32_t*)(rec + 0x04);
                                        HOVER_SPTR(other, 0x84, node + 0xb8);
                                    }
                                    else
                                    {
                                        *(int32_t*)(other + 0x50) = zero;
                                        HOVER_SPTR(other, 0x4c, node);
                                        *(int32_t*)(other + 0x8c) = 1;
                                        *(int32_t*)(other + 0x54) = *(int32_t*)(rec + 0x04);
                                        HOVER_SPTR(other, 0x88, node + 0xb8);
                                    }
                                    prev = HOVER_LPTR(rec, 0x0c);
                                    rec = HOVER_LPTR(rec, 0x08);
                                    if (prev == 0)
                                        g_4a4490 = (void*)rec;
                                    else
                                        HOVER_SPTR(prev, 0x08, rec);
                                    if (rec == 0)
                                        break;
                                    HOVER_SPTR(rec, 0x0c, prev);
                                    continue;
                                }
                                rec = HOVER_LPTR(rec, 0x08);
                            }
                            else
                            {
                                rec = HOVER_LPTR(rec, 0x08);
                            }
                        } while (rec != 0);
                    }
                    if (*(int32_t*)(node + 0x20) == 0)
                    {
                        kk = *(int32_t*)(edge + 0x90);
                        if (kk != 0)
                        {
                            other = (uint8_t*)g_4a4490;
                            HOVER_SPTR(bump, 0x0c, NULL);
                            g_4a4490 = (void*)bump;
                            HOVER_SPTR(bump, 0x08, other);
                            if (other != 0)
                                HOVER_SPTR(other, 0x0c, bump);
                            *(int32_t*)(bump + 0x04) = kk;
                            HOVER_SPTR(bump, 0x00, node);
                            *(int32_t*)(bump + 0x10) = 1;
                            *(int32_t*)(bump + 0x14) = right;
                            bump += 0x18;
                        }
                        other = HOVER_LPTR(edge, 0xa0);
                        if (other != 0 && width > 0)
                        {
                            uint8_t* hd = (uint8_t*)g_4a4490;
                            HOVER_SPTR(bump, 0x0c, NULL);
                            g_4a4490 = (void*)bump;
                            HOVER_SPTR(bump, 0x08, hd);
                            if (hd != 0)
                                HOVER_SPTR(hd, 0x0c, bump);
                            HOVER_SPTR(bump, 0x04, other);
                            *(int32_t*)(bump + 0x14) = left;
                            HOVER_SPTR(bump, 0x00, node);
                            *(int32_t*)(bump + 0x10) = zero;
                            bump += 0x18;
                        }
                    }
                }
            }
            node = HOVER_LPTR(node, 0x28);
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
            other = HOVER_LPTR(rec, 0x00);
            if (*(int32_t*)(other + 0x18) != 0)
            {
                uint8_t* prev = HOVER_LPTR(rec, 0x0c);
                rec = HOVER_LPTR(rec, 0x08);
                if (prev == 0)
                    g_4a4490 = (void*)rec;
                else
                    HOVER_SPTR(prev, 0x08, rec);
                if (rec == 0)
                    break;
                HOVER_SPTR(rec, 0x0c, prev);
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
                    HOVER_SPTR(other, 0x84, a);
                }
                else
                {
                    HOVER_SPTR(other, 0x84, &g_4a4530);
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
                    HOVER_SPTR(other, 0x88, a);
                }
                else
                {
                    HOVER_SPTR(other, 0x88, &g_4a4530);
                    *(int32_t*)(other + 0x50) = z;
                }
            }
            rec = HOVER_LPTR(rec, 0x08);
        } while (rec != 0);
    }
    node = (uint8_t*)g_4a471c;
    if (*(int32_t*)(node + 0x34) != 0)
    {
        uint8_t* ch;
        do
        {
                uint8_t* sib = HOVER_LPTR(node, 0x30);
                ch = HOVER_LPTR(node, 0x34);
                HOVER_SPTR(node, 0x38, ch);
                HOVER_SPTR(node, 0x3c, sib);
            node = ch;
        } while (*(int32_t*)(ch + 0x34) != 0);
    }
    {
                uint8_t* s30 = HOVER_LPTR(node, 0x30);
                HOVER_SPTR(node, 0x3c, s30);
                HOVER_SPTR(node, 0x38, NULL);
        if (s30 != 0)
        {
            do
            {
                if (*(int32_t*)(node + 0x8c) != 0)
                    F406a80((void*)node);
                if (HOVER_LPTR(node, 0x3c) != 0)
                    node = HOVER_LPTR(node, 0x3c);
            } while (HOVER_LPTR(node, 0x3c) != 0);
        }
        HOVER_SPTR(node, 0x3c, NULL);
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
            node = HOVER_LPTR(node, 0x38);
            if (node == 0)
            {
                break;
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
                node = HOVER_LPTR(node, 0x38);
                if (node == 0)
                {
                    break;
                }
                continue;
            }
            if (c78 < hi)
            {
                node = HOVER_LPTR(node, 0x38);
                if (node == 0)
                {
                    break;
                }
                continue;
            }
            int32_t kx;
            if (c74 > hi)
            {
                uint8_t* ch = HOVER_LPTR(node, 0x38);
                if (ch != 0)
                {
                    int32_t t = c74 - n1;
                    this->F404030(ch, (void*)(intptr_t)hi, (void*)(intptr_t)t);
                }
                else
                {
                    ((SpanChain*)HOVER_LPTR(this, 0))->AddPair6d00(c74 - n1, hi);
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
                    node = HOVER_LPTR(node, 0x38);
                    if (node == 0)
                    {
                        break;
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
                    ((SpanChain*)HOVER_LPTR(this, 0))->AddPair6d00((int32_t)(intptr_t)a3, bb);
                    return hi;
                }
                node = HOVER_LPTR(node, 0x38);
                if (node == 0)
                {
                    break;
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
                node = HOVER_LPTR(node, 0x38);
                if (node == 0)
                {
                    break;
                }
                continue;
            }
            if (bhi < hi)
            {
                node = HOVER_LPTR(node, 0x38);
                if (node == 0)
                {
                    break;
                }
                continue;
            }
            if (bnd > hi)
            {
                uint8_t* ch = HOVER_LPTR(node, 0x38);
                if (ch != 0)
                {
                    int32_t t = bnd - n3;
                    this->F404030(ch, (void*)(intptr_t)hi, (void*)(intptr_t)t);
                }
                else
                {
                    ((SpanChain*)HOVER_LPTR(this, 0))->AddPair6d00(bnd - n3, hi);
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
                    node = HOVER_LPTR(node, 0x38);
                    if (node == 0)
                    {
                        break;
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
                    ((SpanChain*)HOVER_LPTR(this, 0))->AddPair6d00((int32_t)(intptr_t)a3, bb);
                    return hi;
                }
                node = HOVER_LPTR(node, 0x38);
                if (node == 0)
                {
                    break;
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
                node = HOVER_LPTR(node, 0x38);
                if (node == 0)
                {
                    break;
                }
                continue;
            }
            if (c78 < hi)
            {
                node = HOVER_LPTR(node, 0x38);
                if (node == 0)
                {
                    break;
                }
                continue;
            }
            idx = 0;
            if (*(int32_t*)(node + 0x90) > 0)
            {
                el = HOVER_LPTR(node, 0xa8);
                do
                {
                    n2 = g_4a453c;
                    eb = *(int32_t*)(el + 8);
                    ep = *(int32_t*)(el + 0xc);
                    if (eb > hi)
                    {
                        if (eb > (int32_t)(intptr_t)a3)
                        {
                            node = HOVER_LPTR(node, 0x38);
                            if (node == 0)
                            {
                                ((SpanChain*)HOVER_LPTR(this, 0))->AddPair6d00((int32_t)(intptr_t)a3, hi);
                                return hi;
                            }
                            w = 1;
                            break;
                        }
                        {
                            uint8_t* ch = HOVER_LPTR(node, 0x38);
                            if (ch != 0)
                            {
                                int32_t t = eb - n2;
                                this->F404030(ch, (void*)(intptr_t)hi, (void*)(intptr_t)t);
                            }
                            else
                            {
                                ((SpanChain*)HOVER_LPTR(this, 0))->AddPair6d00(eb - n2, hi);
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
                            node = HOVER_LPTR(node, 0x38);
                            if (node == 0)
                            {
                                ((SpanChain*)HOVER_LPTR(this, 0))->AddPair6d00((int32_t)(intptr_t)a3, hi);
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
                        ((SpanChain*)HOVER_LPTR(this, 0))->AddPair6d00((int32_t)(intptr_t)a3, bb);
                        return hi;
                    }
                    node = HOVER_LPTR(node, 0x38);
                    if (node == 0)
                    {
                        ((SpanChain*)HOVER_LPTR(this, 0))->AddPair6d00((int32_t)(intptr_t)a3, hi);
                        return hi;
                    }
                    continue;
                }
            }
            {
                int32_t bb = g_4a453c + ep;
                if (ep >= bb)
                {
                    ((SpanChain*)HOVER_LPTR(this, 0))->AddPair6d00((int32_t)(intptr_t)a3, bb);
                    return hi;
                }
                node = HOVER_LPTR(node, 0x38);
                if (node == 0)
                {
                    break;
                }
                continue;
            }
        }
    }
    ((SpanChain*)HOVER_LPTR(this, 0))->AddPair6d00((int32_t)(intptr_t)a3, hi);
    return hi;
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
    g_4a1d60 = (void*)g_4a1d80;
    if (*(int32_t*)((uint8_t*)this + 0x28) != 0)
        F4030f0(this);
    node = (uint8_t*)g_4a471c;
    while (node != NULL)
    {
        int32_t depth = *(int16_t*)(node + 8);
        if (*(int32_t*)(node + 0x18) != 0 && depth != 0)
        {
            *(int32_t*)(node + 0x58) = 0;
        }
        else if (depth < 0)
        {
            *(int32_t*)(node + 0x58) = 0;
        }
        else if (depth > 0)
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
                    child = HOVER_LPTR(node, 0x5c);
                    lim = *(int32_t*)(child + 0x2c) - 1;
                    if (lim < shade)
                        shade = lim;
                    if (g_461cfc > shade)
                        shade = g_461cfc;
                }
                edge = HOVER_LPTR(node, 0x10);
                edge[0x24] |= 8;
                child = HOVER_LPTR(node, 0x5c);
                tex = *(int16_t*)(node + 0xa);
                sh = (uint8_t)(*(int32_t*)(node + 0x9c) + shade);
                rowbase = *(int32_t*)(child + 0x28) + shade * 32;
                rp = (int32_t*)(uintptr_t)(uint32_t)rowbase;
                mask = (int16_t)(tex >> sh) & *rp;
                tbl = *(int32_t*)((uint8_t*)rp + 0x1c);
                rec = *(int32_t*)(uintptr_t)(uint32_t)(tbl + mask * 8 + 4);
                den = col - row;
                if (den != 0)
                {
                    den <<= sh;
                    *(int32_t*)(node + 0x98) = *(int32_t*)(node + 0x68) / den;
                }
                else
                    *(int32_t*)(node + 0x98) = 0;
                {
                    int16_t n = *(int16_t*)(uintptr_t)(uint32_t)(rec + 0);
                    uint8_t* bump = (uint8_t*)g_4a1d60;
                    span = *(int32_t*)((uint8_t*)rp + 4) << 16;
                    HOVER_SPTR(node, 0xa8, bump);
                    span += 0xffff;
                    *(int32_t*)(node + 0x90) = n;
                    *(int32_t*)(node + 0x94) = span;
                    g_4a1d60 = (void*)(bump + n * 20);
                    if (n != 0)
                    {
                        int32_t* arr = (int32_t*)(uintptr_t)(uint32_t)*(int32_t*)(uintptr_t)(uint32_t)(rec + 4);
                        cnt = n;
                        while (cnt > 0)
                        {
                            int32_t vx = *(int16_t*)(arr + 0) << sh;
                            int32_t vy = *(int16_t*)((uint8_t*)arr + 2) << sh;
                            vx -= *(int32_t*)(node + 0x60);
                            vy -= *(int32_t*)(node + 0x60);
                            *(int32_t*)bump = vx;
                            *(int32_t*)(bump + 4) = vy;
                            if (*(int32_t*)(node + 0x64) > vy)
                                *(int32_t*)(bump + 4) = *(int32_t*)(node + 0x64);
                            col = *(int32_t*)(arr + 1);
                            arr = (int32_t*)((uint8_t*)arr + 8);
                            *(int32_t*)(bump + 0x10) = col;
                            row = ((g_4a4544 - vx) * scale >> 10) + g_4a1d70;
                            row &= negN;
                            *(int32_t*)(bump + 8) = row;
                            col = ((g_4a4544 - *(int32_t*)(bump + 4)) * scale >> 10) + g_4a1d70;
                            col &= negN;
                            *(int32_t*)(bump + 12) = col;
                            bump += 0x14;
                            cnt--;
                        }
                    }
                }
            }
            else
            {
                *(int32_t*)(node + 0xa4) = 0;
                if (*(int32_t*)(node + 0x8c) != 0)
                {
                int32_t a = *(int32_t*)HOVER_LPTR(node, 0x84);
                int32_t b = *(int32_t*)HOVER_LPTR(node, 0x88);
                int32_t mn = a;
                int32_t mx;
                int32_t r0;
                int32_t r1;
                if (b < a)
                    mn = b;
                mx = mn;
                if (row <= mx)
                    mx = row;
                else
                    mn = row;
                *(int32_t*)(node + 0x74) = mx;
                if (b != 0xfff0000 && b > col)
                    mn = b;
                else
                    mn = col;
                if (a == 0xfff0000 || col <= a)
                    mx = col;
                else
                    mx = a;
                *(int32_t*)(node + 0x78) = mx;
                mn = mn;
                }
            }
        }
        else
        {
            uint8_t* st = node + 0xac;
            *(int32_t*)(st + 8) = *(int32_t*)(node + 0x6c) > 0 ? g_4a4530 : g_4a4568;
            *(int32_t*)(st + 0xc) = *(int32_t*)(node + 0x70) > 0 ? g_4a4530 : g_4a4568;
            *(int32_t*)(node + 0x58) = 0;
        }
        node = HOVER_LPTR(node, 0x34);
    }
    this->F404030(g_4a4720, (void*)(uintptr_t)(uint32_t)g_4a1d64, (void*)(uintptr_t)(uint32_t)g_4a456c);
    ((SpanChain*)HOVER_LPTR(this, 0))->F403a60(this);
    if (g_4a1d60 > (void*)0x4a40a8)
        OutputDebugStringA(g_4c520c);
    return 0;
}

// FUNCTION: HOVER 0x00403a60
void SpanChain::F403a60(void* a1)
{
    uint8_t* bucket;
    uint8_t* ap;
    uint8_t* dst;
    uint8_t* src;
    int32_t r0;
    int32_t r1;
    int32_t x0;
    int32_t a;
    int32_t b;
    int32_t y0;
    int32_t n;
    int32_t stride;

    bucket = HOVER_LPTR(this, 8);
    dst = HOVER_LPTR(this, 4);
#if defined(FLYINGDODGEM_GDI_COMPAT)
    if (bucket == NULL || dst == NULL)
        return;
    n = 0;
    src = NULL;
#else
    n = *(int32_t*)(bucket + 0x14);
    src = HOVER_LPTR(HOVER_LPTR(bucket, 0xc), 0);
#endif
    if (n > 0)
    {
        uint8_t* p = dst;
        do
        {
#if defined(FLYINGDODGEM_GDI_COMPAT)
            if (src == NULL)
                break;
            *(int16_t*)(p + 0) = *(int16_t*)src;
            *(int16_t*)(p + 2) = *(int16_t*)(src + 2);
#else
            *(int16_t*)(p + 0) = *(int16_t*)(src + 0);
            *(int16_t*)(p + 2) = *(int16_t*)(src + 2);
#endif
            p += 4;
            src += 4;
            n--;
        } while (n > 0);
    }
    ap = HOVER_LPTR(HOVER_LPTR(bucket, 8), 0);
    r0 = 0;
    r1 = 0;
    stride = g_4a453c;
    if (r1 < *(int32_t*)(bucket + 0x18))
    {
        for (;;)
        {
            if (r0 >= *(int32_t*)(bucket + 0x14))
                break;
            x0 = *(int16_t*)(dst + 0);
            a = *(int16_t*)(ap + 0);
            if (x0 > a)
            {
                b = *(int16_t*)(ap + 2);
                if (x0 > b)
                {
                    if (b >= a)
                    {
                        do
                        {
                            F406740(a1, (void*)(intptr_t)b, *(int16_t*)(HOVER_LPTR(bucket, 0x10) + a * 2), a);
                            a += stride;
                        } while (a <= b);
                    }
                    ap += 4;
                    r1++;
                    if (r1 < *(int32_t*)(bucket + 0x18))
                        continue;
                    break;
                }
                y0 = *(int16_t*)(dst + 2);
                if (y0 >= b)
                {
                    if (x0 > a)
                    {
                        do
                        {
                            F406740(a1, (void*)(intptr_t)b, *(int16_t*)(HOVER_LPTR(bucket, 0x10) + a * 2), a);
                            a += stride;
                        } while (a < x0);
                    }
                    *(int16_t*)(dst + 0) = (int16_t)(b + stride);
                    ap += 4;
                    r1++;
                    if (r1 < *(int32_t*)(bucket + 0x18))
                        continue;
                    break;
                }
                if (x0 > a)
                {
                    do
                    {
                        F406740(a1, (void*)(intptr_t)b, *(int16_t*)(HOVER_LPTR(bucket, 0x10) + a * 2), a);
                        a += stride;
                    } while (a < x0);
                }
                *(int16_t*)(ap + 0) = (int16_t)(y0 + stride);
                dst += 4;
                r0++;
                if (r1 < *(int32_t*)(bucket + 0x18))
                    continue;
                break;
            }
            y0 = *(int16_t*)(dst + 2);
            if (y0 < a)
            {
                if (y0 >= x0)
                {
                    do
                    {
                        *(int16_t*)(HOVER_LPTR(bucket, 0x10) + x0 * 2) = (int16_t)g_4a4534;
                        x0 += stride;
                    } while (x0 <= y0);
                }
                dst += 4;
                r0++;
                if (r1 < *(int32_t*)(bucket + 0x18))
                    continue;
                break;
            }
            b = *(int16_t*)(ap + 2);
            if (y0 <= b)
            {
                if (x0 < a)
                {
                    do
                    {
                        *(int16_t*)(HOVER_LPTR(bucket, 0x10) + x0 * 2) = (int16_t)g_4a4534;
                        x0 += stride;
                    } while (x0 < a);
                }
                *(int16_t*)(ap + 0) = (int16_t)(y0 + stride);
                dst += 4;
                r0++;
                if (r1 < *(int32_t*)(bucket + 0x18))
                    continue;
                break;
            }
            if (x0 < a)
            {
                do
                {
                    *(int16_t*)(HOVER_LPTR(bucket, 0x10) + x0 * 2) = (int16_t)g_4a4534;
                    x0 += stride;
                } while (x0 < a);
            }
            *(int16_t*)(dst + 0) = (int16_t)(b + stride);
            ap += 4;
            r1++;
            if (r1 >= *(int32_t*)(bucket + 0x18))
                break;
        }
    }
    if (r1 < *(int32_t*)(bucket + 0x18))
    {
        for (a = r1; a < *(int32_t*)(bucket + 0x18); a++)
        {
            src = HOVER_LPTR(HOVER_LPTR(bucket, 8), 0) + a * 4;
            x0 = *(int16_t*)(src + 0);
            b = *(int16_t*)(src + 2);
            if (b >= x0)
            {
                for (n = x0; n <= b; n += stride)
                    F406740(a1, (void*)(intptr_t)b, *(int16_t*)(HOVER_LPTR(bucket, 0x10) + n * 2), n);
            }
        }
    }
    if (r0 < *(int32_t*)(bucket + 0x14))
    {
        n = *(int32_t*)(bucket + 0x14) - r0;
        ap = (uint8_t*)HOVER_LPTR(this, 4) + r0 * 4 + 2;
        while (n != 0)
        {
            x0 = *(int16_t*)(ap - 2);
            b = *(int16_t*)(ap);
            if (x0 <= b)
            {
                src = HOVER_LPTR(bucket, 0x10) + x0 * 2;
                a = stride * 2;
                do
                {
                    *(int16_t*)src = (int16_t)g_4a4534;
                    src += a;
                    x0 += stride;
                } while (x0 <= b);
            }
            ap += 4;
            n--;
        }
    }
    src = HOVER_LPTR(bucket, 0xc);
    ap = HOVER_LPTR(bucket, 8);
    n = *(int32_t*)(bucket + 0x14);
    HOVER_SPTR(bucket, 0xc, ap);
    HOVER_SPTR(bucket, 8, src);
    *(int32_t*)(bucket + 0x18) = n;
    *(int32_t*)(bucket + 0x14) = 0;
    bucket = HOVER_LPTR(this, 0xc);
    while (bucket != NULL)
    {
#if defined(FLYINGDODGEM_GDI_COMPAT)
        n = 0;
        src = NULL;
#else
        n = *(int32_t*)(bucket + 0x14);
        src = HOVER_LPTR(HOVER_LPTR(bucket, 0xc), 0);
#endif
        dst = HOVER_LPTR(this, 4);
        if (n > 0)
        {
            uint8_t* p = dst;
            do
            {
#if defined(FLYINGDODGEM_GDI_COMPAT)
                if (src == NULL)
                    break;
                *(int16_t*)(p + 0) = *(int16_t*)src;
                *(int16_t*)(p + 2) = *(int16_t*)(src + 2);
#else
                *(int16_t*)(p + 0) = *(int16_t*)(src + 0);
                *(int16_t*)(p + 2) = *(int16_t*)(src + 2);
#endif
                p += 4;
                src += 4;
                n--;
            } while (n > 0);
        }
        ap = HOVER_LPTR(HOVER_LPTR(bucket, 8), 0);
        r0 = 0;
        r1 = 0;
        stride = g_4a453c;
        if (r1 < *(int32_t*)(bucket + 0x18))
        {
            for (;;)
            {
                if (r0 >= *(int32_t*)(bucket + 0x14))
                    break;
                x0 = *(int16_t*)(dst + 0);
                a = *(int16_t*)(ap + 0);
                if (x0 > a)
                {
                    b = *(int16_t*)(ap + 2);
                    if (x0 > b)
                    {
                        if (b >= a)
                        {
                            do
                            {
                                ((SpanChain*)a1)->F402b10((void*)(intptr_t)*(int16_t*)(HOVER_LPTR(bucket, 0x10) + a * 2), a,
                                                          *(int32_t*)(bucket + 0x24), *(int32_t*)(bucket + 0x1c));
                                a += stride;
                            } while (a <= b);
                        }
                        ap += 4;
                        r1++;
                        if (r1 < *(int32_t*)(bucket + 0x18))
                            continue;
                        break;
                    }
                    y0 = *(int16_t*)(dst + 2);
                    if (y0 >= b)
                    {
                        if (x0 > a)
                        {
                            do
                            {
                                ((SpanChain*)a1)->F402b10((void*)(intptr_t)*(int16_t*)(HOVER_LPTR(bucket, 0x10) + a * 2), a,
                                                          *(int32_t*)(bucket + 0x24), *(int32_t*)(bucket + 0x1c));
                                a += stride;
                            } while (a < x0);
                        }
                        *(int16_t*)(dst + 0) = (int16_t)(b + stride);
                        ap += 4;
                        r1++;
                        if (r1 < *(int32_t*)(bucket + 0x18))
                            continue;
                        break;
                    }
                    if (x0 > a)
                    {
                        do
                        {
                            ((SpanChain*)a1)->F402b10((void*)(intptr_t)*(int16_t*)(HOVER_LPTR(bucket, 0x10) + a * 2), a,
                                                      *(int32_t*)(bucket + 0x24), *(int32_t*)(bucket + 0x1c));
                            a += stride;
                        } while (a < x0);
                    }
                    *(int16_t*)(ap + 0) = (int16_t)(y0 + stride);
                    dst += 4;
                    r0++;
                    if (r1 < *(int32_t*)(bucket + 0x18))
                        continue;
                    break;
                }
                y0 = *(int16_t*)(dst + 2);
                if (y0 < a)
                {
                    if (y0 >= x0)
                    {
                        do
                        {
                            *(int16_t*)(HOVER_LPTR(bucket, 0x10) + x0 * 2) = (int16_t)g_4a4534;
                            x0 += stride;
                        } while (x0 <= y0);
                    }
                    dst += 4;
                    r0++;
                    if (r1 < *(int32_t*)(bucket + 0x18))
                        continue;
                    break;
                }
                b = *(int16_t*)(ap + 2);
                if (y0 <= b)
                {
                    if (x0 < a)
                    {
                        do
                        {
                            *(int16_t*)(HOVER_LPTR(bucket, 0x10) + x0 * 2) = (int16_t)g_4a4534;
                            x0 += stride;
                        } while (x0 < a);
                    }
                    *(int16_t*)(ap + 0) = (int16_t)(y0 + stride);
                    dst += 4;
                    r0++;
                    if (r1 < *(int32_t*)(bucket + 0x18))
                        continue;
                    break;
                }
                if (x0 < a)
                {
                    do
                    {
                        *(int16_t*)(HOVER_LPTR(bucket, 0x10) + x0 * 2) = (int16_t)g_4a4534;
                        x0 += stride;
                    } while (x0 < a);
                }
                *(int16_t*)(dst + 0) = (int16_t)(b + stride);
                ap += 4;
                r1++;
                if (r1 >= *(int32_t*)(bucket + 0x18))
                    break;
            }
        }
        if (r1 < *(int32_t*)(bucket + 0x18))
        {
            for (a = r1; a < *(int32_t*)(bucket + 0x18); a++)
            {
                src = HOVER_LPTR(HOVER_LPTR(bucket, 8), 0) + a * 4;
                x0 = *(int16_t*)(src + 0);
                b = *(int16_t*)(src + 2);
                if (b >= x0)
                {
                    for (n = x0; n <= b; n += stride)
                        ((SpanChain*)a1)->F402b10((void*)(intptr_t)*(int16_t*)(HOVER_LPTR(bucket, 0x10) + n * 2), n,
                                                  *(int32_t*)(bucket + 0x24), *(int32_t*)(bucket + 0x1c));
                }
            }
        }
        if (r0 < *(int32_t*)(bucket + 0x14))
        {
            n = *(int32_t*)(bucket + 0x14) - r0;
            ap = (uint8_t*)HOVER_LPTR(this, 4) + r0 * 4 + 2;
            while (n != 0)
            {
                x0 = *(int16_t*)(ap - 2);
                b = *(int16_t*)(ap);
                if (x0 <= b)
                {
                    src = HOVER_LPTR(bucket, 0x10) + x0 * 2;
                    a = stride * 2;
                    do
                    {
                        *(int16_t*)src = (int16_t)g_4a4534;
                        src += a;
                        x0 += stride;
                    } while (x0 <= b);
                }
                ap += 4;
                n--;
            }
        }
        src = HOVER_LPTR(bucket, 0xc);
        ap = HOVER_LPTR(bucket, 8);
        n = *(int32_t*)(bucket + 0x14);
        HOVER_SPTR(bucket, 0xc, ap);
        HOVER_SPTR(bucket, 8, src);
        *(int32_t*)(bucket + 0x18) = n;
        *(int32_t*)(bucket + 0x14) = 0;
        bucket = HOVER_LPTR(bucket, 0x28);
    }
}
// FUNCTION: HOVER 0x004053a0
int32_t HoverTc::M4053a0(void* a1, void* a2)
{
    uint8_t* volatile selfpad = (uint8_t*)this;
    uint8_t* volatile node;
    uint8_t* zero;
    uint8_t* display = (uint8_t*)a1;
    (void)a2;

    node = *(uint8_t**)((uint8_t*)g_460970 + 0x8390);
    if (node != 0)
    {
        zero = 0;
        do
        {
            void* a1;
            a1 = *(void**)(node + 8);
            node = *(uint8_t**)node;
            ((HoverTc*)selfpad)->F401c00(a1, *(void**)(display + 0x540));
        } while (node != zero);
    }
    node = *(uint8_t**)((uint8_t*)g_460970 + 0x83ac);
    if (node != 0)
    {
        zero = 0;
        do
        {
            void* a1;
            a1 = *(void**)(node + 8);
            node = *(uint8_t**)node;
            ((HoverTc*)selfpad)->F401c00(a1, *(void**)(display + 0x540));
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
                uint8_t color = *(uint8_t*)(obj + 0x40);
                *(uint8_t*)((uint8_t*)selfpad + 0x1c) = color;
                uint16_t w = color;
                uint16_t v = (uint16_t)((w << 8) | w);
                uint8_t* cell = obj + 0x9c;
                uint32_t dd = v;
                uint32_t ee = (dd << 16) | dd;
                *(int32_t*)((uint8_t*)selfpad + 0x20) = (int32_t)ee;
                DrawCell3910((void*)selfpad, (void*)(intptr_t)*(int32_t*)(cell + 4), *(int32_t*)cell, *(int32_t*)(cell + 4), 5);
                if (*(int32_t*)(obj + 0xa8) != 0)
                {
                    volatile int32_t xa = *(int32_t*)cell - 0xa;
                    volatile int32_t xb = *(int32_t*)cell + 0xa;
                    volatile int32_t ya = *(int32_t*)(cell + 4) - 0xa;
                    volatile int32_t yb = *(int32_t*)(cell + 4) + 0xa;
                    ((HoverTc*)selfpad)->F401980(xa, ya, xb, ya);
                    ((HoverTc*)selfpad)->F401980(xa, yb, xb, yb);
                    yb += 0x14;
                    xa -= 0x14;
                    ((HoverTc*)selfpad)->F401980(xa, ya, xb, yb);
                    ((HoverTc*)selfpad)->F401980(xa, yb, xb, yb);
                }
                {
                    int32_t nn = *(int32_t*)(obj + 0x8c);
                    if (nn > 0)
                    {
                        int32_t kk = 0;
                        do
                        {
                            uint8_t* child;
                            uint8_t* base = *(uint8_t**)(obj + 0x88);
                            kk += 4;
                            child = *(uint8_t**)(base + kk - 4);
                            ((HoverTc*)selfpad)->F401980(*(int32_t*)(obj + 0x9c), *(int32_t*)(obj + 0xa0), *(int32_t*)(child + 0x9c), *(int32_t*)(child + 0x9c + 4));
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
#if defined(_MSC_VER) && !defined(_M_X64)
__inline int32_t* MulShift16(int32_t* p, int32_t d)
{
    __asm {
        mov ecx, p
        mov eax, [ecx]
        imul d
        shrd eax, edx, 16
        mov [ecx], eax
    }
    return p;
}
#else
static inline int32_t* MulShift16(int32_t* p, int32_t d)
{
    *p = (int32_t)(((int64_t)*p * (int64_t)d) >> 16);
    return p;
}
#endif
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
    MulShift16(&v30, k64);
    MulShift16(&v10, k58);
    v28 = v10 - v30;
    MulShift16(&v28, k50);
    *(int32_t*)((uint8_t*)this + 0x5c) = ((int16_t*)&v28)[1];
    v1c = *(int32_t*)pd;
    v0c = *(int32_t*)pb;
    k48 = w24;
    k3c = w20;
    k34 = *(int32_t*)((uint8_t*)this + 0x48);
    MulShift16(&v1c, k48);
    MulShift16(&v0c, k3c);
    v14 = v0c + v1c;
    MulShift16(&v14, k34);
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
// STUB: HOVER 0x00410ec0
void HoverTc::M410ec0()
{
}
// probe
// probe
// FUNCTION: HOVER 0x0040fbb0
void HoverTc::M40fbb0(Four rc)
{
    uint8_t* p = (uint8_t*)this;
    int32_t h;
    int32_t lf;
    int32_t base;
    *(Four*)(p + 0x00) = rc;
    (*(int32_t*)(p + 0x08))--;
    (*(int32_t*)(p + 0x0c))--;
    h = *(int32_t*)(p + 0x18) - rc.b - 1;
    *(int32_t*)(p + 0x48) = h;
    *(int32_t*)(p + 0x44) = rc.b - rc.d + h;
    lf = (rc.a % 4) == 0;
    *(int32_t*)(p + 0x34) = lf;
    *(int32_t*)(p + 0x38) = ((rc.c + 1) % 4) == 0;
    base = *(int32_t*)(p + 0x10);
    if (lf)
        *(int32_t*)(p + 0x54) = (rc.a & ~3) + base;
    else
        *(int32_t*)(p + 0x54) = (rc.a & ~3) + base + 4;
    rc.a += base;
    *(int32_t*)(p + 0x4c) = rc.a;
    *(int32_t*)(p + 0x58) = (rc.c & ~3) + base;
    *(int32_t*)(p + 0x50) = rc.c + base - 4;
    *(int32_t*)(p + 0x40) = *(int32_t*)(p + 0x24) * h;
    *(int32_t*)(p + 0x3c) = *(int32_t*)(p + 0x28) * h;
}
// STUB: HOVER 0x00410f20
void HoverTc::M410f20(int32_t a1, int32_t a2)
{
}
extern "C" HOVER_IMPORT uint32_t __stdcall GetNearestPaletteIndex(void* hpal, uint32_t color);
extern "C" void* g_4a1d54;
#if defined(_MSC_VER) && !defined(_M_X64)
__inline int32_t* DivShift16(int32_t* p, int32_t d)
{
    __asm {
        mov ecx, p
        mov eax, [ecx]
        mov edx, eax
        sar edx, 16
        shl eax, 16
        idiv d
        mov [ecx], eax
    }
    return p;
}
#else
static inline int32_t* DivShift16(int32_t* p, int32_t d)
{
    *p = (int32_t)(((int64_t)*p << 16) / d);
    return p;
}
#endif
struct Two
{
    int32_t a;
    int32_t b;
};

union I32
{
    int32_t d;
};

union IP
{
    uint8_t* p;
};

union V16
{
    int32_t d;
    int16_t w[2];
};

// FUNCTION: HOVER 0x0040fb00
void HoverTc::Sub40fb00(int32_t a, int32_t b, int32_t c)
{
    uint8_t* p = (uint8_t*)this;
    int32_t q;
    *(int32_t*)(p + 0x10) = a;
#if defined(_MSC_VER) && !defined(_M_X64)
    *(Two*)(p + 0x14) = *(Two*)&b;
#else
    *(int32_t*)(p + 0x14) = b;
    *(int32_t*)(p + 0x18) = c;
#endif
    q = ((Two*)(p + 0x14))->a;
    *(int32_t*)(p + 0x24) = q;
    if (q % 4)
        q += q % 4;
    *(int32_t*)(p + 0x28) = q >> 2;
}

// FUNCTION: HOVER 0x0040fee0
void HoverTc::M40fee0(void* a1, void* a2, void* a3, Four rc)
{
    volatile int32_t pads[5];
    IP node;
    V16 v;
    v.w[0] = 0;
    v.w[1] = 0x5a00;
    uint8_t* p = (uint8_t*)this;
    void* ctx;
    I32 w;
    I32 k;
    int32_t t1;
    int32_t t2;
    int32_t t3;
    int32_t t4;
    int32_t a;
    int32_t b;

    *(Four*)(p + 0x5c) = rc;

    *(int16_t*)(p + 0x70) = 0;
    *(int16_t*)(p + 0x72) = (int16_t)(rc.c - rc.a);
    w.d = *(int32_t*)(p + 0x70);
    *(int32_t*)(p + 0x78) = w.d;
    *(int32_t*)(p + 0x78) = w.d / 2;

    *(int16_t*)(p + 0x6c) = 0;
    *(int16_t*)(p + 0x6e) = (int16_t)(rc.d - rc.b);
    *(int32_t*)(p + 0x74) = *(int32_t*)(p + 0x6c);
    *(int32_t*)(p + 0x74) /= 2;

    *(int32_t*)(p + 0x7c) = w.d;
    MulShift16((int32_t*)(p + 0x7c), *(int32_t*)(p + 0xc8));

    *(int32_t*)(p + 0x80) = v.d;
    DivShift16((int32_t*)(p + 0x80), *(int32_t*)(p + 0x7c));

    *(int32_t*)(p + 0x7c) = v.d;
    DivShift16((int32_t*)(p + 0x7c), *(int32_t*)(p + 0x80));

    k.d = *(int32_t*)(p + 0x80);
    t1 = *(int32_t*)(p + 0x78);
    MulShift16(&t1, k.d);
    t2 = t1;
    MulShift16(&t2, 0x16b85);
    a = (int32_t)*(int16_t*)((uint8_t*)&t2 + 2);
    t3 = *(int32_t*)(p + 0x74);
    MulShift16(&t3, k.d);
    t4 = t3;
    MulShift16(&t4, 0x16b85);
    b = (int32_t)*(int16_t*)((uint8_t*)&t4 + 2);
    *(int16_t*)(p + 0x88) = (int16_t)((b > a) ? b : a);

    *(int32_t*)(p + 0x84) = (int32_t)(intptr_t)a1;

    ctx = *(void**)((uint8_t*)a2 + 0xd0);
    ((HoverTc*)this)->Sub40fb00(*(int32_t*)((uint8_t*)ctx + 0x4), *(int32_t*)((uint8_t*)ctx + 0x14), *(int32_t*)((uint8_t*)ctx + 0x18));
    ((HoverTc*)this)->M40fbb0(rc);

    node.p = *(uint8_t**)((uint8_t*)g_460970 + 0x8390);
    while (node.p != 0)
    {
        *(int32_t*)(*(uint8_t**)(node.p + 0x8) + 0x3c) = 0;
        node.p = *(uint8_t**)(node.p + 0x0);
    }

    *(int32_t*)(p + 0x8c) = GetNearestPaletteIndex(g_4a1d54, 0xff);
    *(int32_t*)(p + 0x90) = GetNearestPaletteIndex(g_4a1d54, 0xff0000);
    *(int32_t*)(p + 0x94) = GetNearestPaletteIndex(g_4a1d54, 0x9200);
    *(int32_t*)(p + 0x98) = GetNearestPaletteIndex(g_4a1d54, 0xffff);
    *(int32_t*)(p + 0x9c) = GetNearestPaletteIndex(g_4a1d54, 0xffffff);
    *(int32_t*)(p + 0xa0) = GetNearestPaletteIndex(g_4a1d54, 0);
    *(int32_t*)(p + 0xa4) = GetNearestPaletteIndex(g_4a1d54, 0xa4a4a4);
    *(int32_t*)(p + 0xa8) = GetNearestPaletteIndex(g_4a1d54, 0x808080);
    pads[0] = 0;
    pads[1] = 0;
    pads[2] = 0;
    pads[3] = 0;
    pads[4] = 0;
}
// FUNCTION: HOVER 0x00413c10
void HoverTc::M413c10(void* a1)
{
    int32_t v = g_4c4cc8[(int32_t)(intptr_t)a1];
    *(int32_t*)((uint8_t*)this + 0xe4) = v;
    ((HoverTc*)*(void**)((uint8_t*)this + 0xc8))->M410f20(3, v);
    ((HoverTc*)*(void**)((uint8_t*)this + 0xc8))->M410ec0();
    if (*(void**)((uint8_t*)this + 0xcc) != 0)
    {
        ((HoverTc*)*(void**)((uint8_t*)this + 0xcc))->M410f20(3, *(int32_t*)((uint8_t*)this + 0xe4));
        ((HoverTc*)*(void**)((uint8_t*)this + 0xcc))->M410ec0();
    }
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
// FUNCTION: HOVER 0x00427a80
void HoverTc::M427a80(void* a1)
{
    uint8_t* tbl = g_4609a0;
    volatile int32_t scale;
    volatile void* la1 = a1;
    if (*(int32_t*)((uint8_t*)this + 0x544) == 0)
    {
        int32_t idx = g_460998;
        int32_t t1 = idx + idx * 2;
        int32_t t2 = idx + t1 * 4;
        int32_t t3 = t2 * 8 - idx;
        tbl = g_4609a0 + t3 * 8;
    }
    if (la1 == 0)
        goto L_a1zero;
    if (*(int32_t*)((uint8_t*)this + 0x218) < 0xa0000)
        goto L_scale;
    if (la1 != 0)
        return;
L_a1zero:
    if (*(int32_t*)((uint8_t*)this + 0x218) < 0xcccc)
        return;
    if (*(int32_t*)((uint8_t*)this + 0x218) == 0xcccc)
        return;
L_scale:
    {
        volatile int32_t s1;
        volatile int32_t s2;
        if (la1 != 0)
        {
            s1 = 0x1547a;
            scale = s1;
        }
        else
        {
            s2 = 0xc000;
            scale = s2;
        }
    }
    MulShift16((int32_t*)((uint8_t*)this + 0x218), scale);
    if (*(int32_t*)((uint8_t*)this + 0x540) == 0)
    {
        tbl += 0x258;
        ((HoverTc*)((uint8_t*)this + 0x150))->M40fee0((void*)((uint8_t*)this + 0xb4), this, g_460970, *(Four*)tbl);
    }
    else
    {
        int32_t rc[4];
        GetClientRect(*(void**)((uint8_t*)this + 0x1c), rc);
        ((HoverTc*)((uint8_t*)this + 0x150))->M40fee0((void*)((uint8_t*)this + 0xb4), this, g_460970, *(Four*)rc);
    }
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
