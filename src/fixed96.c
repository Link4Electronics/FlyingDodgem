#include "flyingdodgem_types.h"

/* 96-bit fixed-point arithmetic helpers used by the game's math library.
 * A 96-bit value is stored as three little-endian 32-bit limbs. */

// FUNCTION: HOVER 0x004473f2
void __cdecl Shl96(uint32_t v[3])
{
    uint32_t m0 = v[0] & 0x80000000U;
    uint32_t m1 = v[1] & 0x80000000U;
    v[0] = v[0] << 1;
    v[2] = (v[2] << 1) + (m1 >> 31);
    v[1] = (v[1] << 1) + (m0 >> 31);
}

// FUNCTION: HOVER 0x00447430
void __cdecl Shr96(uint32_t v[3])
{
    uint32_t c1 = v[2] & 1u;
    uint32_t c0 = v[1] & 1u;
    v[2] >>= 1;
    v[1] = (v[1] >> 1) | (c1 << 31);
    v[0] = (v[0] >> 1) | (c0 << 31);
}

// FUNCTION: HOVER 0x00447371
uint32_t __cdecl AddCarry96(uint32_t a, uint32_t b, uint32_t *out)
{
    uint32_t sum = a + b;
    uint32_t r = 0;
    if ((sum < a) || (sum < b))
        r = 1;
    *out = sum;
    return r;
}

// FUNCTION: HOVER 0x00447394
#pragma optimize("s", on)
void __cdecl Add96(uint32_t dst[3], const uint32_t src[3])
{
    if (AddCarry96(dst[0], src[0], dst)) {
        if (AddCarry96(dst[1], 1u, dst + 1))
            dst[2]++;
    }
    if (AddCarry96(dst[1], src[1], dst + 1))
        dst[2]++;
    dst += 2;
    src += 2;
    AddCarry96(dst[0], src[0], dst);
}
#pragma optimize("", on)

extern uint16_t g_4a4a58[16384];

// FUNCTION: HOVER 0x004385f0
int32_t __cdecl Atan4385f0(int32_t y, int32_t x)
{
    int32_t ay = y;
    int32_t ax = x;
    int32_t flags = 0;
    int32_t r;
    int32_t lo;
    int32_t hi;
    int32_t mid;

    if (ay < 0)
    {
        flags = 1;
        ay = -ay;
    }
    if (ax < 0)
    {
        ax = -ax;
        flags |= 2;
    }
    mid = 0x4000;
    if (ax != ay)
    {
        if (ax >= ay)
            r = (ay << 16) / ax;
        else
            r = (ax << 16) / ay;
        lo = 0;
        hi = 0x3fff;
        for (;;)
        {
            mid = (lo + hi) / 2;
            if (g_4a4a58[mid] < r)
            {
                lo = mid + 1;
                if (lo >= hi)
                    break;
            }
            else if (g_4a4a58[mid] > r)
            {
                hi = mid - 1;
                if (lo >= hi)
                    break;
            }
            else
                break;
        }
        if (ax < ay)
            mid = 0x8000 - mid;
    }
    if (flags == 1)
        mid = 0x10000 - mid;
    else if (flags == 2)
        mid = 0x20000 - mid;
    else if (flags == 3)
        mid = mid + 0x10000;
    return mid << 8;
}
