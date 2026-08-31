#include "flyingdodgem_types.h"

/* 96-bit fixed-point arithmetic helpers used by the game's math library.
 * A 96-bit value is stored as three little-endian 32-bit limbs. */

// FUNCTION: HOVER 0x004473f2
void __cdecl Shl96(uint32_t v[3])
{
    uint32_t lo = v[0];
    uint32_t mid = v[1];
    uint32_t b0 = lo & 0x80000000U;
    uint32_t b1 = mid & 0x80000000U;
    v[0] = lo << 1;
    v[1] = (mid << 1) | (b0 >> 31);
    v[2] = (v[2] << 1) | (b1 >> 31);
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
