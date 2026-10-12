#ifndef FLYINGDODGEM_TEXMAZE_H
#define FLYINGDODGEM_TEXMAZE_H

#include "flyingdodgem_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* TEX / MAZ asset loaders reconstructed from HOVER.EXE's CMerlinTexture
 * (Serialize 0x412980, vtable 0x4bbd20) and the maze document serializer
 * (doc 0x418dc0, CMerlinStatic 0x419e00, CMerlinLocation 0x41b940,
 * CMerlinBSP 0x41bc40). See AGENTS.md for the reverse-engineered layout. */

/* ------------------------------------------------------------------ TEX -- */

typedef struct TexSpan {
    u16 y0;                 /* full-resolution start row (inclusive) */
    u16 y1;                 /* full-resolution end row (inclusive)   */
} TexSpan;

typedef struct TexColumn {
    u16 span_count;
    const TexSpan *spans;   /* span_count entries, NULL if 0         */
} TexColumn;

typedef struct TexFrame {
    u16 width;              /* a: column count of this mip          */
    u16 height;             /* c: row count of this mip (d + 1)     */
    u16 log_height;         /* e: log2(c)                           */
    u16 mip;                /* frame index: span rows shift by this */
    u32 pixel_len;          /* L: payload bytes (== decoded pixels) */
    const u8 *pixels;       /* column-major palette indices         */
    TexColumn *columns;     /* width entries                        */
} TexFrame;

typedef struct TexTexture {
    char name[32];          /* texture name (e.g. "BRYAN")              */
    u16 isdecal;            /* 1 if decal (transparency), 0 otherwise */
    u16 frame_count;        /* N (mip chain: full size down to 4)       */
    TexFrame *frames;
} TexTexture;

typedef struct TexArchive {
    u8 palette[1024];       /* 256 x RGBA (A is 0x00 in retail)     */
    u16 count;
    TexTexture *textures;
    u8 *data;               /* backing buffer (file contents)       */
    u32 size;
} TexArchive;

/* Load a .TEX archive. Returns 1 on success, 0 on failure. */
int Tex_Load(const char *path, TexArchive *out);
void Tex_Free(TexArchive *tex);

/* Find a texture by name (case-sensitive as stored); NULL if absent. */
const TexTexture *Tex_Find(const TexArchive *tex, const char *name);

/* Decode a frame into dst (width * height palette indices). dst is fully
 * cleared first; rows/columns not covered by spans stay 0 (transparent). */
void Tex_DecodeFrame(const TexFrame *frame, u8 *dst);

/* --------------------------------------------------------------- MAZ -- */

typedef enum MazClass {
    MAZ_CLASS_NONE = 0,
    MAZ_CLASS_STATIC,
    MAZ_CLASS_LOCATION,
    MAZ_CLASS_BSP
} MazClass;

/* CMerlinStatic: a textured surface definition. */
typedef struct MazStatic {
    char name[32];          /* grandbase string                        */
    u16 field_14, field_18, field_1c, field_20; /* doc-relative u16s   */
    char tex_name[6][64];   /* six texture references (+0x44..+0x80)   */
    u16 u16_a4, u16_a6, u16_a8, u16_aa;
    u8 u8_b0, u8_b4, u8_bc;
    u16 version;
    u8 u8_c0;               /* version >= 5 fields                    */
    u16 u16_b8, u16_ba;
} MazStatic;

/* CMerlinLocation: a placed point / spawn. */
typedef struct MazLocation {
    char name[32];
    u16 pos[4];             /* +0x14, +0x16, +0x18, +0x1a             */
} MazLocation;

/* CMerlinBSP: BSP partition data for the maze. */
typedef struct MazBSP {
    char name[32];
    u16 field_14, field_18, field_1c, field_20; /* shared prologue u16s  */
    u16 field_44, field_46, field_48, field_4a, field_60;
    u32 field_50, field_54; /* first  0x41bee0 read (8 bytes)         */
    u32 field_58, field_5c; /* second 0x41bee0 read (8 bytes)         */
    u16 leaf_count;
    const u8 *leaf_bytes;   /* leaf_count bytes (one per leaf)        */
} MazBSP;

typedef struct MazObject {
    MazClass cls;
    union {
        MazStatic st;
        MazLocation loc;
        MazBSP bsp;
    } u;
} MazObject;

typedef struct MazArray {
    u16 count;
    MazObject *items;
} MazArray;

typedef struct MazDoc {
    u16 hdr[4];             /* bytes 0..7                             */
    MazArray arrays[4];     /* statics, locations, BSP, extra         */
    u8 *data;
    u32 size;
} MazDoc;

int Maz_Load(const char *path, MazDoc *out);
void Maz_Free(MazDoc *maz);

#ifdef __cplusplus
}
#endif

#endif
