#include "flyingdodgem_types.h"

/* Getters that return the address of a module-global table. The original compiler
 * emitted these as two-instruction accessors (`mov eax, <addr>; ret`); each buyer is
 * free to pass the returned pointer to the game code. Declared globals are mapped
 * through GLOBAL annotations so reccmp can rebase the address displacements. */

// GLOBAL: HOVER 0x004bb0c0
uint8_t g_data_b0c0[1];

// FUNCTION: HOVER 0x00401010
uint32_t __cdecl GetTable_1010(void)
{
    return (uint32_t)(uintptr_t)g_data_b0c0;
}

// GLOBAL: HOVER 0x004ba8d0
uint8_t g_data_a8d0[1];

// FUNCTION: HOVER 0x00401020
uint32_t __cdecl GetTable_1020(void)
{
    return (uint32_t)(uintptr_t)g_data_a8d0;
}

// GLOBAL: HOVER 0x004c5570
uint8_t g_data_5570[1];

// FUNCTION: HOVER 0x004011d0
uint32_t __cdecl GetTable_11d0(void)
{
    return (uint32_t)(uintptr_t)g_data_5570;
}

// GLOBAL: HOVER 0x004c53a0
uint8_t g_data_53a0[1];

// FUNCTION: HOVER 0x004011e0
uint32_t __cdecl GetTable_11e0(void)
{
    return (uint32_t)(uintptr_t)g_data_53a0;
}

// GLOBAL: HOVER 0x004c53c8
uint8_t g_data_53c8[1];

// FUNCTION: HOVER 0x004011f0
uint32_t __cdecl GetTable_11f0(void)
{
    return (uint32_t)(uintptr_t)g_data_53c8;
}

// GLOBAL: HOVER 0x004c5460
uint8_t g_data_5460[1];

// FUNCTION: HOVER 0x00401200
uint32_t __cdecl GetTable_1200(void)
{
    return (uint32_t)(uintptr_t)g_data_5460;
}

// GLOBAL: HOVER 0x004c5270
uint8_t g_data_5270[1];

// FUNCTION: HOVER 0x00401480
uint32_t __cdecl GetTable_1480(void)
{
    return (uint32_t)(uintptr_t)g_data_5270;
}

// GLOBAL: HOVER 0x004c5298
uint8_t g_data_5298[1];

// FUNCTION: HOVER 0x00401490
uint32_t __cdecl GetTable_1490(void)
{
    return (uint32_t)(uintptr_t)g_data_5298;
}

// GLOBAL: HOVER 0x004c52f0
uint8_t g_data_52f0[1];

// FUNCTION: HOVER 0x004014a0
uint32_t __cdecl GetTable_14a0(void)
{
    return (uint32_t)(uintptr_t)g_data_52f0;
}

// GLOBAL: HOVER 0x004c5340
uint8_t g_data_5340[1];

// FUNCTION: HOVER 0x004014b0
uint32_t __cdecl GetTable_14b0(void)
{
    return (uint32_t)(uintptr_t)g_data_5340;
}

// GLOBAL: HOVER 0x004c5378
uint8_t g_data_5378[1];

// FUNCTION: HOVER 0x004014c0
uint32_t __cdecl GetTable_14c0(void)
{
    return (uint32_t)(uintptr_t)g_data_5378;
}

// GLOBAL: HOVER 0x004c5418
uint8_t g_data_5418[1];

// FUNCTION: HOVER 0x004014d0
uint32_t __cdecl GetTable_14d0(void)
{
    return (uint32_t)(uintptr_t)g_data_5418;
}

// GLOBAL: HOVER 0x004c5488
uint8_t g_data_5488[1];

// FUNCTION: HOVER 0x004014e0
uint32_t __cdecl GetTable_14e0(void)
{
    return (uint32_t)(uintptr_t)g_data_5488;
}

// GLOBAL: HOVER 0x004c54f8
uint8_t g_data_54f8[1];

// FUNCTION: HOVER 0x004014f0
uint32_t __cdecl GetTable_14f0(void)
{
    return (uint32_t)(uintptr_t)g_data_54f8;
}

// GLOBAL: HOVER 0x004c5520
uint8_t g_data_5520[1];

// FUNCTION: HOVER 0x00401500
uint32_t __cdecl GetTable_1500(void)
{
    return (uint32_t)(uintptr_t)g_data_5520;
}

// GLOBAL: HOVER 0x004c5548
uint8_t g_data_5548[1];

// FUNCTION: HOVER 0x00401510
uint32_t __cdecl GetTable_1510(void)
{
    return (uint32_t)(uintptr_t)g_data_5548;
}

// GLOBAL: HOVER 0x004c5438
uint8_t g_data_5438[1];

// FUNCTION: HOVER 0x004038c0
uint32_t __cdecl GetTable_38c0(void)
{
    return (uint32_t)(uintptr_t)g_data_5438;
}

// GLOBAL: HOVER 0x004c54b8
uint8_t g_data_54b8[1];

// FUNCTION: HOVER 0x004038d0
uint32_t __cdecl GetTable_38d0(void)
{
    return (uint32_t)(uintptr_t)g_data_54b8;
}

// GLOBAL: HOVER 0x004ba0a8
uint8_t g_data_a0a8[1];

// FUNCTION: HOVER 0x0040e2a0
uint32_t __cdecl GetTable_e2a0(void)
{
    return (uint32_t)(uintptr_t)g_data_a0a8;
}

// GLOBAL: HOVER 0x004bac90
uint8_t g_data_ac90[1];

// FUNCTION: HOVER 0x0040e300
uint32_t __cdecl GetTable_e300(void)
{
    return (uint32_t)(uintptr_t)g_data_ac90;
}

// GLOBAL: HOVER 0x004bd598
uint8_t g_data_d598[1];

// FUNCTION: HOVER 0x0040e550
uint32_t __cdecl GetTable_e550(void)
{
    return (uint32_t)(uintptr_t)g_data_d598;
}

// GLOBAL: HOVER 0x004be9a8
uint8_t g_data_e9a8[1];

// FUNCTION: HOVER 0x00414730
uint32_t __cdecl GetTable_4730(void)
{
    return (uint32_t)(uintptr_t)g_data_e9a8;
}

// GLOBAL: HOVER 0x004bf038
uint8_t g_data_f038[1];

// FUNCTION: HOVER 0x0041c020
uint32_t __cdecl GetTable_c020(void)
{
    return (uint32_t)(uintptr_t)g_data_f038;
}

// GLOBAL: HOVER 0x004c4cb0
uint8_t g_data_4cb0[1];

// FUNCTION: HOVER 0x0041e700
uint32_t __cdecl GetTable_e700(void)
{
    return (uint32_t)(uintptr_t)g_data_4cb0;
}

// GLOBAL: HOVER 0x004c4808
uint8_t g_data_4808[1];

// FUNCTION: HOVER 0x00426a50
uint32_t __cdecl GetTable_6a50(void)
{
    return (uint32_t)(uintptr_t)g_data_4808;
}

// GLOBAL: HOVER 0x004c48d8
uint8_t g_data_48d8[1];

// FUNCTION: HOVER 0x00426e80
uint32_t __cdecl GetTable_6e80(void)
{
    return (uint32_t)(uintptr_t)g_data_48d8;
}

// GLOBAL: HOVER 0x004c51b0
uint8_t g_data_51b0[1];

// FUNCTION: HOVER 0x00429730
uint32_t __cdecl GetTable_9730(void)
{
    return (uint32_t)(uintptr_t)g_data_51b0;
}

// GLOBAL: HOVER 0x004c5220
uint8_t g_data_5220[1];

// FUNCTION: HOVER 0x0042a500
uint32_t __cdecl GetTable_a500(void)
{
    return (uint32_t)(uintptr_t)g_data_5220;
}

// GLOBAL: HOVER 0x004c5248
uint8_t g_data_5248[1];

// FUNCTION: HOVER 0x0042a5d0
uint32_t __cdecl GetTable_a5d0(void)
{
    return (uint32_t)(uintptr_t)g_data_5248;
}

// GLOBAL: HOVER 0x004c53f8
uint8_t g_data_53f8[1];

// FUNCTION: HOVER 0x0042b8d0
uint32_t __cdecl GetTable_b8d0(void)
{
    return (uint32_t)(uintptr_t)g_data_53f8;
}

// GLOBAL: HOVER 0x004c55a0
uint8_t g_data_55a0[1];

// FUNCTION: HOVER 0x0042c6e0
uint32_t __cdecl GetTable_c6e0(void)
{
    return (uint32_t)(uintptr_t)g_data_55a0;
}

// GLOBAL: HOVER 0x004c55b8
uint8_t g_data_55b8[1];

// FUNCTION: HOVER 0x0042c6f0
uint32_t __cdecl GetTable_c6f0(void)
{
    return (uint32_t)(uintptr_t)g_data_55b8;
}

// GLOBAL: HOVER 0x004bd208
uint8_t g_data_d208[1];

// FUNCTION: HOVER 0x0042ca30
uint32_t __cdecl GetTable_ca30(void)
{
    return (uint32_t)(uintptr_t)g_data_d208;
}

// GLOBAL: HOVER 0x004c5638
uint8_t g_data_5638[1];

// FUNCTION: HOVER 0x0042cf70
uint32_t __cdecl GetTable_cf70(void)
{
    return (uint32_t)(uintptr_t)g_data_5638;
}

// GLOBAL: HOVER 0x004c5660
uint8_t g_data_5660[1];

// FUNCTION: HOVER 0x0042cfc0
uint32_t __cdecl GetTable_cfc0(void)
{
    return (uint32_t)(uintptr_t)g_data_5660;
}

// GLOBAL: HOVER 0x004bd4a0
uint8_t g_data_d4a0[1];

// FUNCTION: HOVER 0x0042d0c0
uint32_t __cdecl GetTable_d0c0(void)
{
    return (uint32_t)(uintptr_t)g_data_d4a0;
}

// GLOBAL: HOVER 0x004c56a8
uint8_t g_data_56a8[1];

// FUNCTION: HOVER 0x0042d600
uint32_t __cdecl GetTable_d600(void)
{
    return (uint32_t)(uintptr_t)g_data_56a8;
}

// GLOBAL: HOVER 0x004bda18
uint8_t g_data_da18[1];

// FUNCTION: HOVER 0x0042db50
uint32_t __cdecl GetTable_db50(void)
{
    return (uint32_t)(uintptr_t)g_data_da18;
}

// GLOBAL: HOVER 0x004bec50
uint8_t g_data_ec50[1];

// FUNCTION: HOVER 0x0042f840
uint32_t __cdecl GetTable_f840(void)
{
    return (uint32_t)(uintptr_t)g_data_ec50;
}

// GLOBAL: HOVER 0x004bf178
uint8_t g_data_f178[1];

// FUNCTION: HOVER 0x00430180
uint32_t __cdecl GetTable_0180(void)
{
    return (uint32_t)(uintptr_t)g_data_f178;
}

// GLOBAL: HOVER 0x004bf400
uint8_t g_data_f400[1];

// FUNCTION: HOVER 0x00430b70
uint32_t __cdecl GetTable_0b70(void)
{
    return (uint32_t)(uintptr_t)g_data_f400;
}

// GLOBAL: HOVER 0x004bf450
uint8_t g_data_f450[1];

// FUNCTION: HOVER 0x00431d00
uint32_t __cdecl GetTable_1d00(void)
{
    return (uint32_t)(uintptr_t)g_data_f450;
}

// GLOBAL: HOVER 0x004bf4a0
uint8_t g_data_f4a0[1];

// FUNCTION: HOVER 0x00432da0
uint32_t __cdecl GetTable_2da0(void)
{
    return (uint32_t)(uintptr_t)g_data_f4a0;
}

// GLOBAL: HOVER 0x004bf850
uint8_t g_data_f850[1];

// FUNCTION: HOVER 0x00433850
uint32_t __cdecl GetTable_3850(void)
{
    return (uint32_t)(uintptr_t)g_data_f850;
}

// GLOBAL: HOVER 0x004bfb78
uint8_t g_data_fb78[1];

// FUNCTION: HOVER 0x00434230
uint32_t __cdecl GetTable_4230(void)
{
    return (uint32_t)(uintptr_t)g_data_fb78;
}

// GLOBAL: HOVER 0x004bfc80
uint8_t g_data_fc80[1];

// FUNCTION: HOVER 0x004342d0
uint32_t __cdecl GetTable_42d0(void)
{
    return (uint32_t)(uintptr_t)g_data_fc80;
}

// GLOBAL: HOVER 0x004bb670
uint32_t g_data_bb670;

// GLOBAL: HOVER 0x004c4ca8
uint32_t g_data_4ca8;
