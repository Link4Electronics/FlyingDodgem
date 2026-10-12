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

// GLOBAL: HOVER 0x0046058c
void* g_46058c;

// GLOBAL: HOVER 0x004c52d8
uint32_t g_4c52d8;

// GLOBAL: HOVER 0x004c52dc
void* g_4c52dc;

// GLOBAL: HOVER 0x004c79b0
void* g_4c79b0;

// GLOBAL: HOVER 0x004b6ff0
void* g_4b6ff0;

// GLOBAL: HOVER 0x004c5330
void* g_4c5330;

// GLOBAL: HOVER 0x004c53e0
void* g_4c53e0;

// GLOBAL: HOVER 0x004c54d0
void* g_4c54d0;

// GLOBAL: HOVER 0x004a48c8
void* g_4a48c8;

// GLOBAL: HOVER 0x004c844c
uint32_t g_4c844c;

// GLOBAL: HOVER 0x004c8450
uint32_t g_4c8450;

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

// GLOBAL: HOVER 0x004c4880
uint8_t g_data_4880[1];

// GLOBAL: HOVER 0x004bd5e8
uint8_t g_data_d5e8[1];

// GLOBAL: HOVER 0x004bdbb8
uint8_t g_data_dbb8[1];

// GLOBAL: HOVER 0x004bdfe0
uint8_t g_data_dfe0[1];

// GLOBAL: HOVER 0x004becd0
uint8_t g_data_ecd0[1];

// GLOBAL: HOVER 0x004bf1e0
uint8_t g_data_f1e0[1];

// GLOBAL: HOVER 0x004bf8b8
uint8_t g_data_f8b8[1];

// GLOBAL: HOVER 0x004c5764
uint8_t g_data_5764[1];

// GLOBAL: HOVER 0x004c5758
uint8_t g_data_5758[1];
// GLOBAL: HOVER 0x004C5790
uint8_t g_data_5790[1];

// GLOBAL: HOVER 0x004C51E0
uint8_t g_data_51e0[1];

// GLOBAL: HOVER 0x004C52C0
uint8_t g_data_52c0[1];

// GLOBAL: HOVER 0x004C5318
uint8_t g_data_5318[1];

// GLOBAL: HOVER 0x004BFEE0
uint8_t g_data_fee0[1];

// GLOBAL: HOVER 0x004C0320
uint8_t g_data_0320[1];

// GLOBAL: HOVER 0x004C05E0
uint8_t g_data_05e0[1];

// GLOBAL: HOVER 0x004C0600
uint8_t g_data_0600[1];

// GLOBAL: HOVER 0x004C0630
uint8_t g_data_0630[1];

// GLOBAL: HOVER 0x004C08B8
uint8_t g_data_08b8[1];

// GLOBAL: HOVER 0x004C6A20
uint8_t g_data_6a20[1];

// GLOBAL: HOVER 0x004C1D60
uint8_t g_data_1d60[1];

// GLOBAL: HOVER 0x004C2380
uint8_t g_data_2380[1];

// GLOBAL: HOVER 0x004C00C0
uint8_t g_data_00c0[1];

// GLOBAL: HOVER 0x004C1B00
uint8_t g_data_1b00[1];

// GLOBAL: HOVER 0x004C2C48
uint8_t g_data_2c48[1];

// GLOBAL: HOVER 0x004C2CB0
uint8_t g_data_2cb0[1];

// GLOBAL: HOVER 0x004C31B0
uint8_t g_data_31b0[1];

// GLOBAL: HOVER 0x004C15A8
uint8_t g_data_15a8[1];

// GLOBAL: HOVER 0x004C34D0
uint8_t g_data_34d0[1];

// GLOBAL: HOVER 0x004C68B8
uint8_t g_data_68b8[1];

// GLOBAL: HOVER 0x004179d8
uint8_t g_data_79d8[1];
// GLOBAL: HOVER 0x004C68E0
uint8_t g_data_68e0[1];

// GLOBAL: HOVER 0x004C6908
uint8_t g_data_6908[1];

// GLOBAL: HOVER 0x004C6938
uint8_t g_data_6938[1];

// GLOBAL: HOVER 0x004C6960
uint8_t g_data_6960[1];

// GLOBAL: HOVER 0x004C6998
uint8_t g_data_6998[1];

// GLOBAL: HOVER 0x004C6A00
uint8_t g_data_6a00[1];

// GLOBAL: HOVER 0x004C6A40
uint8_t g_data_6a40[1];

// GLOBAL: HOVER 0x004C6A70
uint8_t g_data_6a70[1];

// GLOBAL: HOVER 0x004C6A88
uint8_t g_data_6a88[1];

// GLOBAL: HOVER 0x004C6AA0
uint8_t g_data_6aa0[1];

// GLOBAL: HOVER 0x004C6AB8
uint8_t g_data_6ab8[1];

// GLOBAL: HOVER 0x004C6AD0
uint8_t g_data_6ad0[1];

// GLOBAL: HOVER 0x004C6AE8
uint8_t g_data_6ae8[1];

// GLOBAL: HOVER 0x004C6B00
uint8_t g_data_6b00[1];

// GLOBAL: HOVER 0x004C6B18
uint8_t g_data_6b18[1];

// GLOBAL: HOVER 0x004C6B30
uint8_t g_data_6b30[1];

// GLOBAL: HOVER 0x004C6B48
uint8_t g_data_6b48[1];

// GLOBAL: HOVER 0x004C6B60
uint8_t g_data_6b60[1];

// GLOBAL: HOVER 0x004C6D48
uint8_t g_data_6d48[1];

// GLOBAL: HOVER 0x004C6DE0
uint8_t g_data_6de0[1];

// GLOBAL: HOVER 0x004C6DF8
uint8_t g_data_6df8[1];

// GLOBAL: HOVER 0x004C6F38
uint8_t g_data_6f38[1];

// GLOBAL: HOVER 0x004C6FC8
uint8_t g_data_6fc8[1];

// GLOBAL: HOVER 0x004C6FE0
uint8_t g_data_6fe0[1];

// GLOBAL: HOVER 0x004C7010
uint8_t g_data_7010[1];

// GLOBAL: HOVER 0x004C7028
uint8_t g_data_7028[1];

// GLOBAL: HOVER 0x004C7098
uint8_t g_data_7098[1];

// GLOBAL: HOVER 0x004C70B0
uint8_t g_data_70b0[1];

// GLOBAL: HOVER 0x004C70C8
uint8_t g_data_70c8[1];

// GLOBAL: HOVER 0x004C7118
uint8_t g_data_7118[1];

// GLOBAL: HOVER 0x004C7150
uint8_t g_data_7150[1];

// GLOBAL: HOVER 0x004C7180
uint8_t g_data_7180[1];

// GLOBAL: HOVER 0x004C71A0
uint8_t g_data_71a0[1];

// GLOBAL: HOVER 0x004C71C8
uint8_t g_data_71c8[1];

// GLOBAL: HOVER 0x004C71E8
uint8_t g_data_71e8[1];

// GLOBAL: HOVER 0x004C7238
uint8_t g_data_7238[1];

// GLOBAL: HOVER 0x004C7268
uint8_t g_data_7268[1];

// GLOBAL: HOVER 0x004C7298
uint8_t g_data_7298[1];

// GLOBAL: HOVER 0x004C72F0
uint8_t g_data_72f0[1];

// GLOBAL: HOVER 0x004C7330
uint8_t g_data_7330[1];

// GLOBAL: HOVER 0x004C73F8
uint8_t g_data_73f8[1];

// GLOBAL: HOVER 0x004C7410
uint8_t g_data_7410[1];

// GLOBAL: HOVER 0x004C7448
uint8_t g_data_7448[1];

// GLOBAL: HOVER 0x004C97D8
uint8_t g_data_97d8[1];

// GLOBAL: HOVER 0x004C97F0
uint8_t g_data_97f0[1];

// GLOBAL: HOVER 0x004C98F8
uint8_t g_data_98f8[1];

// GLOBAL: HOVER 0x004177C9
uint8_t g_data_77c9[1];
// GLOBAL: HOVER 0x004179F7
uint8_t g_data_79f7[1];
// GLOBAL: HOVER 0x0041D3D7
uint8_t g_data_d3d7[1];
// GLOBAL: HOVER 0x0041F0B0
uint8_t g_data_f0b0[1];
// GLOBAL: HOVER 0x0041F31B
uint8_t g_data_f31b[1];
// GLOBAL: HOVER 0x004C5608
uint8_t g_data_5608[1];
// GLOBAL: HOVER 0x004BDF78
uint8_t g_data_df78[1];
// GLOBAL: HOVER 0x0043E607
uint8_t g_data_e607[1];
// GLOBAL: HOVER 0x0043E622
uint8_t g_data_e622[1];
// GLOBAL: HOVER 0x0043FB32
uint8_t g_data_fb32[1];
// GLOBAL: HOVER 0x0043FB4D
uint8_t g_data_fb4d[1];
// GLOBAL: HOVER 0x0044A614
uint8_t g_data_a614[1];
// GLOBAL: HOVER 0x0044C339
uint8_t g_data_c339[1];
// GLOBAL: HOVER 0x0044C70E
uint8_t g_data_c70e[1];
// GLOBAL: HOVER 0x0044E172
uint8_t g_data_e172[1];
// GLOBAL: HOVER 0x0044E7B2
uint8_t g_data_e7b2[1];
// GLOBAL: HOVER 0x00451B7E
uint8_t g_data_1b7e[1];
// GLOBAL: HOVER 0x0045216C
uint8_t g_data_216c[1];
// GLOBAL: HOVER 0x00452188
uint8_t g_data_2188[1];
// GLOBAL: HOVER 0x004521AB
uint8_t g_data_21ab[1];
// GLOBAL: HOVER 0x004523FB
uint8_t g_data_23fb[1];
// GLOBAL: HOVER 0x00452417
uint8_t g_data_2417[1];
// GLOBAL: HOVER 0x0045243A
uint8_t g_data_243a[1];
// GLOBAL: HOVER 0x004C3118
uint8_t g_data_3118[1];
// GLOBAL: HOVER 0x0045C7B5
uint8_t g_data_c7b5[1];
// GLOBAL: HOVER 0x0045C7C3
uint8_t g_data_c7c3[1];
// GLOBAL: HOVER 0x004C6A58
uint8_t g_data_6a58[1];
// GLOBAL: HOVER 0x004C6E10
uint8_t g_data_6e10[1];




// GLOBAL: HOVER 0x004b7cd8
void* g_pApp;

/* Wall/floor/span renderer scratch state (0x402990 and friends). */
// GLOBAL: HOVER 0x004a4524
int32_t g_4a4524;
// GLOBAL: HOVER 0x004a4528
int16_t g_4a4528;
// GLOBAL: HOVER 0x004a452c
int32_t g_4a452c;
// GLOBAL: HOVER 0x004a4534
int32_t g_4a4534;
// GLOBAL: HOVER 0x004a4538
int32_t g_4a4538;
// GLOBAL: HOVER 0x004a453c
int32_t g_4a453c;
// GLOBAL: HOVER 0x004a4540
int32_t g_4a4540;
// GLOBAL: HOVER 0x004a4544
int32_t g_4a4544;
// GLOBAL: HOVER 0x004a4548
void* g_4a4548;
// GLOBAL: HOVER 0x004a454c
int32_t g_4a454c;
// GLOBAL: HOVER 0x004a4550
int32_t g_4a4550;
// GLOBAL: HOVER 0x004a4554
int32_t g_4a4554;
// GLOBAL: HOVER 0x004a455c
int32_t g_4a455c;
// GLOBAL: HOVER 0x004a4564
int32_t g_4a4564;
// GLOBAL: HOVER 0x004a4714
void* g_4a4714;
// GLOBAL: HOVER 0x004a4718
void* g_4a4718;
// GLOBAL: HOVER 0x004a471c
void* g_4a471c;
// GLOBAL: HOVER 0x004a44b0
int32_t g_4a44b0;
// GLOBAL: HOVER 0x004a44ac
int32_t g_4a44ac;
// GLOBAL: HOVER 0x004a44d0
int32_t g_4a44d0;
// GLOBAL: HOVER 0x004a44d4
int32_t g_4a44d4;
// GLOBAL: HOVER 0x004a44f8
int32_t g_4a44f8;
// GLOBAL: HOVER 0x004a44f4
int32_t g_4a44f4;
// GLOBAL: HOVER 0x004a451c
int32_t g_4a451c;
// GLOBAL: HOVER 0x004a4518
int32_t g_4a4518;
/* Centre-cutout viewport box / minimap visibility state (0x407160). */
// GLOBAL: HOVER 0x004a4790
int32_t g_4a4790;
// GLOBAL: HOVER 0x004a4794
int32_t g_4a4794;
// GLOBAL: HOVER 0x004a4798
int32_t g_4a4798;
// GLOBAL: HOVER 0x004a479c
int32_t g_4a479c;
// GLOBAL: HOVER 0x004a47b0
int32_t g_4a47b0;
// GLOBAL: HOVER 0x004a47b8
int32_t g_4a47b8;
// GLOBAL: HOVER 0x004a47bc
int32_t g_4a47bc;
// GLOBAL: HOVER 0x004a47d8
int32_t g_4a47d8;
// GLOBAL: HOVER 0x004a47e0
int32_t g_4a47e0;
// GLOBAL: HOVER 0x004a47e4
int32_t g_4a47e4;
// GLOBAL: HOVER 0x004a4844
void** g_4a4844;
// GLOBAL: HOVER 0x004a4848
int32_t g_4a4848;
// GLOBAL: HOVER 0x004a4a14
int32_t g_4a4a14;
// GLOBAL: HOVER 0x004a4a18
int32_t g_4a4a18;
// GLOBAL: HOVER 0x004a4a1c
int32_t g_4a4a1c;
// GLOBAL: HOVER 0x004a4a20
int32_t g_4a4a20;
// GLOBAL: HOVER 0x004a4a24
int32_t g_4a4a24;
// GLOBAL: HOVER 0x004a4a28
int32_t g_4a4a28;
// GLOBAL: HOVER 0x004a4a2c
int32_t g_4a4a2c;
// GLOBAL: HOVER 0x004a4a30
int32_t g_4a4a30;
// GLOBAL: HOVER 0x004a4a34
int32_t g_4a4a34;
// GLOBAL: HOVER 0x004a4a38
int32_t g_4a4a38;
// GLOBAL: HOVER 0x004a4a3c
int32_t g_4a4a3c;
// GLOBAL: HOVER 0x004a4a40
int32_t g_4a4a40;
// GLOBAL: HOVER 0x004a4a44
int32_t g_4a4a44;
// GLOBAL: HOVER 0x004a4a48
int32_t g_4a4a48;
// GLOBAL: HOVER 0x004a4a4c
int32_t g_4a4a4c;
// GLOBAL: HOVER 0x004a4a50
int32_t g_4a4a50;
// GLOBAL: HOVER 0x004a1d68
void* g_4a1d68;
// GLOBAL: HOVER 0x004a1d70
int32_t g_4a1d70;
// GLOBAL: HOVER 0x004a1d74
int32_t g_4a1d74;
// GLOBAL: HOVER 0x004a1d78
int32_t g_4a1d78;
// GLOBAL: HOVER 0x00461cfc
int32_t g_461cfc;
// GLOBAL: HOVER 0x00460644
uint8_t g_460644[24];
// GLOBAL: HOVER 0x0046065c
uint8_t g_46065c[24];
// GLOBAL: HOVER 0x00460740
int32_t g_460740;
// GLOBAL: HOVER 0x00460750
int32_t g_460750;
// GLOBAL: HOVER 0x00460970
void* g_460970;
// GLOBAL: HOVER 0x004aca58
uint8_t g_sineTable[32768];
// GLOBAL: HOVER 0x00460744
int32_t g_460744;
/* CBumperApp instance (theApp). Retail constructs it at 0x460570 from the
 * static-init block (0x426546). Size 0x418 covers every offset InitInstance /
 * ExitInstance / DisplayEntry reference (+0xd4..+0x410). The CWorld* the
 * scene publishes is a separate global (g_460970) even though retail stores it
 * at app+0x400; reccmp only cares about the instruction shape. */
// GLOBAL: HOVER 0x00460570
uint8_t g_460570[0x418];
// GLOBAL: HOVER 0x004605d0
int32_t g_4605d0;
// GLOBAL: HOVER 0x00460734
int32_t g_460734;
// GLOBAL: HOVER 0x00460960
int32_t g_460960;
// GLOBAL: HOVER 0x00460958
int32_t g_460958;
// GLOBAL: HOVER 0x004a4768
int32_t g_4a4768;
// GLOBAL: HOVER 0x004a4774
int32_t g_4a4774;
// GLOBAL: HOVER 0x004a4840
int32_t g_4a4840;
// GLOBAL: HOVER 0x004a4858
int32_t g_4a4858;
// GLOBAL: HOVER 0x004c57a8
void* g_4c57a8;
// GLOBAL: HOVER 0x004c57ac
int32_t g_4c57ac;
// GLOBAL: HOVER 0x004a4498
uint8_t g_4a4498[0x48];
// GLOBAL: HOVER 0x004a44e0
uint8_t g_4a44e0[0x48];
// GLOBAL: HOVER 0x004a4530
int32_t g_4a4530;
// GLOBAL: HOVER 0x004a4568
int32_t g_4a4568;
// GLOBAL: HOVER 0x004a456c
int32_t g_4a456c;
// GLOBAL: HOVER 0x004a1d64
int32_t g_4a1d64;
// GLOBAL: HOVER 0x004a1d6c
int32_t g_4a1d6c;
// GLOBAL: HOVER 0x004a1d60
void* g_4a1d60;
// GLOBAL: HOVER 0x004a4720
void* g_4a4720;
// GLOBAL: HOVER 0x004c520c
char g_4c520c[] = "Trashing Heap";
// GLOBAL: HOVER 0x004c55f4
char g_4c55f4[] = "Blowing floor heap";
// GLOBAL: HOVER 0x004a4a58
uint16_t g_4a4a58[16384];
// GLOBAL: HOVER 0x004a4490
void* g_4a4490;
// GLOBAL: HOVER 0x004c5208
uint8_t g_4c5208[4];
