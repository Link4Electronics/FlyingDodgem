#include "flyingdodgem_types.h"

/* 96-bit fixed-point arithmetic helpers used by the game's math library.
 * A 96-bit value is stored as three little-endian 32-bit limbs. */

// FUNCTION: HOVER 0x004473f2
void __cdecl Shl96(uint32_t v[3])
{
    uint32_t lo = v[0];
    uint32_t mid = v[1];
    uint32_t carry0 = (lo & 0x80000000U) >> 31;
    uint32_t carry1 = (mid & 0x80000000U) >> 31;
    v[0] = lo << 1;
    v[1] = (mid << 1) | carry0;
    v[2] = (v[2] << 1) | carry1;
}

// FUNCTION: HOVER 0x00447430
void __cdecl Shr96(uint32_t v[3])
{
    uint32_t hi = v[2];
    uint32_t mc = (v[1] & 1u) << 31;
    v[2] = hi >> 1;
    v[1] = (v[1] >> 1) | ((hi & 1u) << 31);
    v[0] = (v[0] >> 1) | mc;
}
