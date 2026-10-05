/* FlyingDodgem: TEX (CMerlinTexture archive) and MAZ (maze document) loaders.
 *
 * Reconstructed from HOVER.EXE's MFC CArchive serialization:
 *   CArchive ctor            0x452902   ReadObject        0x453750
 *   array Serialize          0x449724   ReadCount         0x4526af
 *   CMerlinTexture Serialize 0x412980   CMerlinStatic     0x419e00
 *   CMerlinLocation          0x41b940   CMerlinBSP        0x41bc40
 *   maze doc Serialize       0x418dc0
 *
 * Framing (verified byte-exact against all retail .TEX/.MAZ files, see
 * AGENTS.md): every element is either a 0xFFFF class header (schema u16 +
 * name u16-length) or a 0x80xx new-object tag whose class lives in the
 * MFC load array; plain back-reference tags never occur.
 *
 * Strict C89: this file is part of the base source list and also compiles
 * under the MSVC 5.0 matching build. */
#include "texmaze.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Class ids for the MFC load-array slot table. */
#define CLS_TEXTURE   1
#define CLS_STATIC    2
#define CLS_LOCATION  3
#define CLS_BSP       4

#define SLOT_FREE  0x0000
#define SLOT_OBJ   0x0100          /* an object entry, not a class */
#define SLOT_COUNT 4096

/* ------------------------------------------------------------------ cursors */

typedef struct Cur {
    const u8 *p;
    const u8 *end;
    int ok;
} Cur;

static void cur_init(Cur *c, const u8 *p, const u8 *end)
{
    c->p = p;
    c->end = end;
    c->ok = (p <= end);
}

static u8 cur_u8(Cur *c)
{
    u8 v;
    if (!c->ok)
        return 0;
    if (c->end - c->p < 1) {
        c->ok = 0;
        return 0;
    }
    v = c->p[0];
    c->p += 1;
    return v;
}

static u16 cur_u16(Cur *c)
{
    u16 v;
    if (!c->ok)
        return 0;
    if (c->end - c->p < 2) {
        c->ok = 0;
        return 0;
    }
    v = (u16)(c->p[0] | ((u16)c->p[1] << 8));
    c->p += 2;
    return v;
}

static u32 cur_u32(Cur *c)
{
    u32 v;
    if (!c->ok)
        return 0;
    if (c->end - c->p < 4) {
        c->ok = 0;
        return 0;
    }
    v = (u32)c->p[0] | ((u32)c->p[1] << 8) | ((u32)c->p[2] << 16) |
        ((u32)c->p[3] << 24);
    c->p += 4;
    return v;
}

static const u8 *cur_take(Cur *c, u32 n)
{
    const u8 *p;
    if (!c->ok)
        return NULL;
    if ((u32)(c->end - c->p) < n) {
        c->ok = 0;
        return NULL;
    }
    p = c->p;
    c->p += n;
    return p;
}

static int cur_skip(Cur *c, u32 n)
{
    if (!c->ok)
        return 0;
    if (n == 0)
        return 1;
    return cur_take(c, n) != NULL;
}

/* MFC narrow string: ReadCount (byte count < 0xFF, 0xFF escape) + bytes.
 * Retail strings are all < 0xFF; escapes are handled for completeness.
 * Returns 0 (and fails the cursor) if the string does not fit in cap. */
static int cur_str(Cur *c, char *dst, u32 cap)
{
    u32 n;
    u8 b;
    const u8 *src;

    b = cur_u8(c);
    if (!c->ok)
        return 0;
    if (b < 0xff) {
        n = b;
    } else {
        u16 w = cur_u16(c);
        if (!c->ok)
            return 0;
        if (w == 0xffff) {
            n = cur_u32(c);
        } else if (w == 0xfffe) {
            c->ok = 0;              /* wide string: never occurs */
            return 0;
        } else {
            n = w;
        }
        if (!c->ok)
            return 0;
    }
    if (n >= cap) {
        c->ok = 0;
        return 0;
    }
    src = cur_take(c, n);
    if (!src)
        return 0;
    memcpy(dst, src, n);
    dst[n] = '\0';
    return 1;
}

/* --------------------------------------------------------- load array */

static u8 class_by_name(const char *name)
{
    if (strcmp(name, "CMerlinTexture") == 0)
        return CLS_TEXTURE;
    if (strcmp(name, "CMerlinStatic") == 0)
        return CLS_STATIC;
    if (strcmp(name, "CMerlinLocation") == 0)
        return CLS_LOCATION;
    if (strcmp(name, "CMerlinBSP") == 0)
        return CLS_BSP;
    return 0;
}

/* ReadObject tag handling: resolve the class for the element and register
 * it in the load array exactly like 0x453750 does (class header registers
 * class + object, nextidx += 2; 0x80xx registers just the object). */
static int elem_begin(Cur *c, u16 *slots, u16 *nextp, u8 *cls_out)
{
    u16 tag = cur_u16(c);

    if (!c->ok)
        return 0;
    if (tag == 0xffff) {
        u16 schema = cur_u16(c);
        u16 nlen = cur_u16(c);
        const u8 *src;
        char name[64];
        u8 cls;

        (void)schema;
        if (!c->ok || nlen >= sizeof(name)) {
            c->ok = 0;
            return 0;
        }
        src = cur_take(c, nlen);
        if (!src) {
            c->ok = 0;
            return 0;
        }
        memcpy(name, src, nlen);
        name[nlen] = '\0';
        cls = class_by_name(name);
        if (cls == 0) {
            c->ok = 0;
            return 0;
        }
        if ((u32)*nextp + 1 >= SLOT_COUNT) {
            c->ok = 0;
            return 0;
        }
        slots[*nextp] = cls;
        (*nextp)++;
        slots[*nextp] = SLOT_OBJ;
        (*nextp)++;
        *cls_out = cls;
        return 1;
    }
    if (tag & 0x8000) {
        u16 idx = (u16)(tag & 0x7fff);
        u16 slotv;

        if (idx == 0 || idx >= SLOT_COUNT) {
            c->ok = 0;
            return 0;
        }
        slotv = slots[idx];
        if (slotv < CLS_TEXTURE || slotv > CLS_BSP) {
            c->ok = 0;
            return 0;
        }
        if (*nextp >= SLOT_COUNT) {
            c->ok = 0;
            return 0;
        }
        slots[*nextp] = SLOT_OBJ;
        (*nextp)++;
        *cls_out = (u8)slotv;
        return 1;
    }
    /* Plain back-reference: never occurs in retail TEX/MAZ. */
    c->ok = 0;
    return 0;
}

/* ------------------------------------------------------------------ file IO */

static u8 *read_file(const char *path, u32 *out_size)
{
    FILE *f;
    long sz;
    u8 *buf;

    f = fopen(path, "rb");
    if (f == NULL)
        return NULL;
    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return NULL;
    }
    sz = ftell(f);
    if (sz <= 0) {
        fclose(f);
        return NULL;
    }
    if (fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        return NULL;
    }
    buf = (u8 *)malloc((size_t)sz);
    if (buf == NULL) {
        fclose(f);
        return NULL;
    }
    if (fread(buf, 1, (size_t)sz, f) != (size_t)sz) {
        free(buf);
        fclose(f);
        return NULL;
    }
    fclose(f);
    *out_size = (u32)sz;
    return buf;
}

/* ------------------------------------------------------------------- TEX -- */

static int tex_frame(Cur *c, TexFrame *f, u16 frame_idx)
{
    u16 a, b, hh, d, e, lg, x, m;
    u32 L, K, sum, k, t, blob;
    const u8 *payload;
    TexColumn *cols;
    TexSpan *sp;

    a = cur_u16(c);
    b = cur_u16(c);
    hh = cur_u16(c);
    d = cur_u16(c);
    e = cur_u16(c);
    if (!c->ok)
        return 0;
    if (a == 0 || b != (u16)(a - 1))
        return 0;
    if (hh == 0 || d != (u16)(hh - 1))
        return 0;
    lg = 0;
    t = hh;
    while (t > 1) {
        if (t & 1)
            return 0;               /* height must be a power of two */
        t >>= 1;
        lg++;
    }
    if (lg != e)
        return 0;

    f->width = a;
    f->height = hh;
    f->log_height = e;
    f->mip = frame_idx;

    L = cur_u32(c);
    if (!c->ok)
        return 0;
    payload = cur_take(c, L);
    if (!payload)
        return 0;
    f->pixel_len = L;
    f->pixels = payload;

    K = cur_u32(c);
    if (!c->ok)
        return 0;
    cols = (TexColumn *)calloc(a, sizeof(TexColumn));
    if (cols == NULL)
        return 0;
    sp = (TexSpan *)calloc(K ? (size_t)K : 1, sizeof(TexSpan));
    if (sp == NULL) {
        free(cols);
        return 0;
    }
    f->columns = cols;
    cols[0].spans = sp;

    sum = 0;
    k = 0;
    for (x = 0; x < a; x++) {
        u16 mm = cur_u16(c);

        if (!c->ok)
            return 0;
        if (k + mm > K)
            return 0;
        cols[x].span_count = mm;
        cols[x].spans = sp + k;
        for (m = 0; m < mm; m++) {
            u16 y0 = cur_u16(c);
            u16 y1 = cur_u16(c);
            u32 sy0, sy1;

            if (!c->ok)
                return 0;
            if (y0 > y1)
                return 0;
            sy0 = (u32)y0 >> frame_idx;
            sy1 = (u32)y1 >> frame_idx;
            if (sy1 >= (u32)hh)
                return 0;
            sum += sy1 - sy0 + 1;
            sp[k].y0 = y0;
            sp[k].y1 = y1;
            k++;
        }
    }
    if (k != K || sum != L)
        return 0;

    blob = cur_u16(c);              /* per-frame extra blob (t bytes) */
    if (!c->ok || !cur_skip(c, blob))
        return 0;
    return c->ok;
}

static int tex_body(Cur *c, TexTexture *t)
{
    u16 blob, nframes, i;

    if (!cur_str(c, t->name, sizeof(t->name)))
        return 0;
    blob = cur_u16(c);
    if (!cur_skip(c, blob))
        return 0;
    t->field_1c = cur_u16(c);
    nframes = cur_u16(c);
    if (!c->ok)
        return 0;
    t->frame_count = nframes;
    t->frames = (TexFrame *)calloc(nframes ? nframes : 1, sizeof(TexFrame));
    if (t->frames == NULL) {
        c->ok = 0;
        return 0;
    }
    for (i = 0; i < nframes; i++) {
        if (!tex_frame(c, &t->frames[i], i))
            return 0;
    }
    return c->ok;
}

int Tex_Load(const char *path, TexArchive *out)
{
    Cur c;
    u8 *buf;
    u32 size;
    u16 count, slots[SLOT_COUNT], nextidx, i;
    u16 trailer;

    memset(out, 0, sizeof(*out));
    buf = read_file(path, &size);
    if (buf == NULL)
        return 0;
    out->data = buf;
    out->size = size;
    if (size < 1026) {
        Tex_Free(out);
        return 0;
    }
    memcpy(out->palette, buf, 1024);

    cur_init(&c, buf + 1024, buf + size);
    count = cur_u16(&c);
    if (!c.ok) {
        Tex_Free(out);
        return 0;
    }
    out->count = count;
    out->textures = (TexTexture *)calloc(count ? count : 1, sizeof(TexTexture));
    if (out->textures == NULL) {
        Tex_Free(out);
        return 0;
    }

    memset(slots, 0, sizeof(slots));
    nextidx = 1;
    for (i = 0; i < count; i++) {
        u8 cls;

        if (!elem_begin(&c, slots, &nextidx, &cls) || cls != CLS_TEXTURE ||
            !tex_body(&c, &out->textures[i])) {
            Tex_Free(out);
            return 0;
        }
    }
    trailer = cur_u16(&c);
    if (!c.ok || trailer != 0 || c.p != c.end) {
        Tex_Free(out);
        return 0;
    }
    return 1;
}

void Tex_Free(TexArchive *tex)
{
    u16 i;

    if (tex == NULL)
        return;
    if (tex->textures != NULL) {
        for (i = 0; i < tex->count; i++) {
            TexTexture *t = &tex->textures[i];

            if (t->frames != NULL) {
                u16 j;

                for (j = 0; j < t->frame_count; j++) {
                    TexFrame *f = &t->frames[j];

                    if (f->columns != NULL) {
                        free((void *)f->columns[0].spans);
                        free(f->columns);
                    }
                }
                free(t->frames);
            }
        }
        free(tex->textures);
    }
    free(tex->data);
    memset(tex, 0, sizeof(*tex));
}

const TexTexture *Tex_Find(const TexArchive *tex, const char *name)
{
    u16 i;

    if (tex == NULL || name == NULL)
        return NULL;
    for (i = 0; i < tex->count; i++) {
        if (strcmp(tex->textures[i].name, name) == 0)
            return &tex->textures[i];
    }
    return NULL;
}

void Tex_DecodeFrame(const TexFrame *frame, u8 *dst)
{
    u32 x, s, y, o;

    if (frame == NULL || dst == NULL || frame->columns == NULL)
        return;
    memset(dst, 0, (size_t)frame->width * (size_t)frame->height);
    o = 0;
    for (x = 0; x < (u32)frame->width; x++) {
        const TexColumn *col = &frame->columns[x];

        for (s = 0; s < (u32)col->span_count; s++) {
            u32 y0 = (u32)col->spans[s].y0 >> frame->mip;
            u32 y1 = (u32)col->spans[s].y1 >> frame->mip;

            for (y = y0; y <= y1 && o < frame->pixel_len; y++)
                dst[y * (u32)frame->width + x] = frame->pixels[o++];
        }
    }
}

/* ------------------------------------------------------------------- MAZ -- */

static int maz_static(Cur *c, MazStatic *s)
{
    u16 blob, count, ver;
    int j;

    if (!cur_str(c, s->name, sizeof(s->name)))
        return 0;
    blob = cur_u16(c);
    if (!cur_skip(c, blob))
        return 0;
    s->field_14 = cur_u16(c);
    s->field_18 = cur_u16(c);
    s->field_1c = cur_u16(c);
    s->field_20 = cur_u16(c);
    count = cur_u16(c);
    if (!c->ok || !cur_skip(c, count))
        return 0;
    for (j = 0; j < 6; j++) {
        if (!cur_str(c, s->tex_name[j], sizeof(s->tex_name[j])))
            return 0;
    }
    s->u16_a4 = cur_u16(c);
    s->u16_a6 = cur_u16(c);
    s->u16_a8 = cur_u16(c);
    s->u16_aa = cur_u16(c);
    s->u8_b0 = cur_u8(c);
    s->u8_b4 = cur_u8(c);
    s->u8_bc = cur_u8(c);
    ver = cur_u16(c);
    if (!c->ok)
        return 0;
    s->version = ver;
    if (ver >= 5) {
        s->u8_c0 = cur_u8(c);
        s->u16_b8 = cur_u16(c);
        s->u16_ba = cur_u16(c);
        if (!c->ok || !cur_skip(c, (u32)ver - 5))
            return 0;
    } else {
        if (!cur_skip(c, ver))
            return 0;
    }
    return c->ok;
}

static int maz_location(Cur *c, MazLocation *l)
{
    u16 blob, count;

    if (!cur_str(c, l->name, sizeof(l->name)))
        return 0;
    blob = cur_u16(c);
    if (!cur_skip(c, blob))
        return 0;
    l->pos[0] = cur_u16(c);
    l->pos[1] = cur_u16(c);
    l->pos[2] = cur_u16(c);
    l->pos[3] = cur_u16(c);
    count = cur_u16(c);
    if (!c->ok || !cur_skip(c, count))
        return 0;
    return c->ok;
}

static int maz_bsp(Cur *c, MazBSP *b)
{
    u16 blob, count;

    if (!cur_str(c, b->name, sizeof(b->name)))
        return 0;
    blob = cur_u16(c);
    if (!cur_skip(c, blob))
        return 0;
    b->field_14 = cur_u16(c);
    b->field_18 = cur_u16(c);
    b->field_1c = cur_u16(c);
    b->field_20 = cur_u16(c);
    count = cur_u16(c);
    if (!c->ok || !cur_skip(c, count))
        return 0;
    b->field_44 = cur_u16(c);
    b->field_46 = cur_u16(c);
    b->field_48 = cur_u16(c);
    b->field_4a = cur_u16(c);
    b->field_60 = cur_u16(c);
    b->field_50 = cur_u32(c);       /* 0x41bee0 reads (8 bytes each)  */
    b->field_54 = cur_u32(c);
    b->field_58 = cur_u32(c);
    b->field_5c = cur_u32(c);
    b->leaf_count = cur_u16(c);
    if (!c->ok)
        return 0;
    b->leaf_bytes = cur_take(c, b->leaf_count);
    if (!b->leaf_bytes)
        return 0;
    return c->ok;
}

static int maz_body(Cur *c, u8 cls, MazObject *o)
{
    switch (cls) {
    case CLS_STATIC:
        o->cls = MAZ_CLASS_STATIC;
        return maz_static(c, &o->u.st);
    case CLS_LOCATION:
        o->cls = MAZ_CLASS_LOCATION;
        return maz_location(c, &o->u.loc);
    case CLS_BSP:
        o->cls = MAZ_CLASS_BSP;
        return maz_bsp(c, &o->u.bsp);
    default:
        return 0;
    }
}

int Maz_Load(const char *path, MazDoc *out)
{
    Cur c;
    u8 *buf;
    u32 size;
    u16 slots[SLOT_COUNT], nextidx;
    int a;

    memset(out, 0, sizeof(*out));
    buf = read_file(path, &size);
    if (buf == NULL)
        return 0;
    out->data = buf;
    out->size = size;

    cur_init(&c, buf, buf + size);
    out->hdr[0] = cur_u16(&c);
    out->hdr[1] = cur_u16(&c);
    out->hdr[2] = cur_u16(&c);
    out->hdr[3] = cur_u16(&c);
    if (!c.ok) {
        Maz_Free(out);
        return 0;
    }

    memset(slots, 0, sizeof(slots));
    nextidx = 1;
    for (a = 0; a < 4; a++) {
        u16 count = cur_u16(&c);
        u16 j;

        if (!c.ok) {
            Maz_Free(out);
            return 0;
        }
        out->arrays[a].count = count;
        out->arrays[a].items = (MazObject *)calloc(count ? count : 1,
                                                    sizeof(MazObject));
        if (out->arrays[a].items == NULL) {
            Maz_Free(out);
            return 0;
        }
        for (j = 0; j < count; j++) {
            u8 cls;

            if (!elem_begin(&c, slots, &nextidx, &cls) ||
                !maz_body(&c, cls, &out->arrays[a].items[j])) {
                Maz_Free(out);
                return 0;
            }
        }
    }
    if (!c.ok || c.p != c.end) {   /* MAZ files end exactly after array 4 */
        Maz_Free(out);
        return 0;
    }
    return 1;
}

void Maz_Free(MazDoc *maz)
{
    int a;

    if (maz == NULL)
        return;
    for (a = 0; a < 4; a++)
        free(maz->arrays[a].items);
    free(maz->data);
    memset(maz, 0, sizeof(*maz));
}
