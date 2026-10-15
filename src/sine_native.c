#include "sine_native.h"
#include "flyingdodgem_types.h"

#include <math.h>

/* Retail table: 8192 int32 entries covering one full turn, 16.16 fixed
 * (entry j = sin(2*pi*j/8192) * 65536). The game-side readers sample every
 * 16th dword for the 512-step angles ((ang & 0x1ff) * 16 and ((a7 << 4) &
 * 0x1fff) hit the same subset; +0x800 dwords is the +90-degree cosine), while
 * scene code walks the full range ((ph & 0x7ffc) byte offsets). Clean-room
 * math: sine values are facts, not game data. Native builds only; the
 * matching build never compiles this unit. */

#define SINE_NATIVE_COUNT 8192u
#define SINE_NATIVE_SCALE 65536.0
#define SINE_NATIVE_PI 3.14159265358979323846

extern uint8_t g_sineTable[32768];

void SineNative_Init(void)
{
    int32_t *t = (int32_t *)(void *)g_sineTable;
    u32 j;

    for (j = 0; j < SINE_NATIVE_COUNT; j++)
        t[j] = (int32_t)(sin((f64)j * (2.0 * SINE_NATIVE_PI) / (f64)SINE_NATIVE_COUNT) * SINE_NATIVE_SCALE);
}
