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
