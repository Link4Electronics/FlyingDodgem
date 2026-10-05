#include "flyingdodgem_types.h"

/* Message-pump loop (HOVER 0x44aef6), reached from Run. Compiled as C++ so
 * the retail member dispatch compiles to the original thiscall shape. */

#if defined(_MSC_VER)
#pragma comment(lib, "user32.lib")
#endif

extern "C" {
int __stdcall PeekMessageA(void* pMsg, void* hWnd, uint32_t nMin, uint32_t nMax, uint32_t nRemove);
}

/* Layout view of the retail app object: only the virtuals the pump reaches
 * are named, padded to their retail vtable slots (OnIdle +0x3c,
 * ExitInstance +0x40, PumpMessage +0x54, PreTranslate +0x58). */
class PumpApp {
public:
    virtual void reserved00();
    virtual void reserved01();
    virtual void reserved02();
    virtual void reserved03();
    virtual void reserved04();
    virtual void reserved05();
    virtual void reserved06();
    virtual void reserved07();
    virtual void reserved08();
    virtual void reserved09();
    virtual void reserved10();
    virtual void reserved11();
    virtual void reserved12();
    virtual void reserved13();
    virtual void reserved14();
    virtual int OnIdle(int lIdle);
    virtual int ExitInstance();
    virtual void reserved17();
    virtual void reserved18();
    virtual void reserved19();
    virtual void reserved20();
    virtual int PumpMessage();
    virtual int PreTranslate(void* pMsg);
};

typedef int (__fastcall *PumpFn)(void* self);

// FUNCTION: HOVER 0x0044aef6
extern "C" int __fastcall PumpMessageLoop(void* self)
{
    int lIdle;
    int bIdle;
    PumpFn pumpfn;

    pumpfn = *(PumpFn*)((uint8_t*)*(void**)self + 0x54);
    bIdle = 1;
    lIdle = 0;

    for (;;)
    {
        if (bIdle)
        {
            do
            {
                if (PeekMessageA((uint8_t*)self + 0x30, 0, 0, 0, 0))
                    break;
                if (!((PumpApp*)self)->OnIdle(lIdle++))
                    bIdle = 0;
            } while (bIdle);
        }

        do
        {
            if (!pumpfn(self))
                return ((PumpApp*)self)->ExitInstance();
            if (((PumpApp*)self)->PreTranslate((uint8_t*)self + 0x30))
            {
                bIdle = 1;
                lIdle = 0;
            }
        } while (PeekMessageA((uint8_t*)self + 0x30, 0, 0, 0, 0));
    }
}
