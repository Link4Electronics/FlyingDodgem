#include "flyingdodgem_types.h"
#include "display_native.h"
#include "gameframe.h"

#include <SDL3/SDL.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#if !defined(_MSC_VER)
#include <sys/mman.h>
#endif

/* ---------------------------------------------------------------------------
 * Native display driver.  Retail runs DisplayEntry41ece0 on a second thread
 * and posts case 3 (DrawEntry402250) into disp+0xac.  The compat event
 * handshake is a no-op, so a free-running worker would race the dashboard
 * composite; instead DisplayDriver_Sync runs one synchronous DrawEntry pass
 * per present.  M406090 is still an empty reccmp stub, so the wall nodes
 * built here are not rasterized yet; the shared INDEX8 bits carry the
 * dashboard composite only.  All pipeline objects (hover, view, host, wall
 * nodes, buckets, bump arenas, per-column tables) live in MAP_32BIT memory
 * so the matched int32_t pointer stores (F4044a0, F405ce0, host+0x28) stay
 * correct on 64-bit once the pass is matched.
 * ------------------------------------------------------------------------- */

extern "C" void* g_460970;

/* Span-host globals written by M406090 / read by F406740. */
extern "C" int32_t g_4a4a34;   /* pitch */
extern "C" int32_t g_4a4534;   /* right edge (span width limit) */
extern "C" int32_t g_4a453c;   /* rows per solid-fill call */
extern "C" void* g_4a1d68;     /* host object */

/* Live plane pointers for DrawEntry's native path (see render.c). */
extern "C" struct NativePlanes
{
    void* host;
    void* planeB;
    void* ctx;
    uint8_t* bits;
    uint32_t* rowColours;
} g_nativePlanes;

#define DISP_SIZE 0x600
#define SUB_SIZE  0x100
#define HOST_SIZE 0xc0 /* Set6c00 writes +0xa8 */
#define HOVER_SIZE 0x50
#define VIEW_SIZE 0x60
#define WALL_NODE_SIZE 0xc0
#define WALL_MAX 128

static uint8_t* g_disp;
static uint8_t* g_host;
static uint8_t* g_ctx;    /* render context */
static uint8_t* g_sube8;  /* t+0xe8 M405a10 this */
static uint8_t* g_hover;
static uint8_t* g_view;
static uint8_t* g_wallPtrs; /* view->walls / view->wallByIndex array */
static uint8_t* g_groupPtrs; /* view->groups edge-desc pointer array */
static uint8_t* g_wallNodes;
static uint8_t* g_wallEdges;   /* group descs, one per wall */
static uint8_t* g_buckets;     /* width int32s */
static uint8_t* g_recordArena; /* F405ce0 records */
static uint8_t* g_nodeArena;   /* F4044a0 nodes */
static uint8_t* g_spanArena;   /* M406090 SpanChain arena (*(ctx+0)) */
static uint8_t* g_spanScratch; /* Setup6c90 working store (*(arena+0x18)) */
static uint8_t* g_dirTable;    /* per-column direction (width*4 int16 pairs) */
static uint8_t* g_scaleTable;  /* width int32s: per-column perspective scale */
static uint8_t* g_topTable;    /* width int32s: row top bound */
static uint8_t* g_botTable;    /* width int32s: row bottom bound */
static uint8_t* g_rowHeight;   /* n entries: row height */
static uint8_t* g_rowColour;   /* 480 uint32s: per-row fill colour */
static int32_t g_wallCount;

extern "C" void __fastcall DrawEntry402250(void* self, void* unused);

static void* alloc_low(size_t n)
{
#if defined(_MSC_VER)
    return calloc(1, n);
#else
    void* p = mmap(NULL, n, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS | MAP_32BIT, -1, 0);
    if (p == MAP_FAILED)
        return NULL;
    memset(p, 0, n);
    return p;
#endif
}

static void free_low(void* p, size_t n)
{
#if defined(_MSC_VER)
    (void)n;
    free(p);
#else
    if (p != NULL)
        munmap(p, n);
#endif
}

/* Retained menu-maze assets (owned by main.c; valid after
 * DisplayDriver_SetMenuMaze). */
static const MazDoc* g_menuMaz = NULL;
static const TexArchive* g_menuTex = NULL;
/* Harness camera in maze units (bbox centre of the active wall set). */
static int32_t g_viewX = 0x400000;
static int32_t g_viewY = 0x400000;

/* Nearest entry of the live screen palette to an RGB triple. */
static uint8_t nearest_screen_index(uint8_t r, uint8_t g, uint8_t b)
{
    RGBQUAD pal[256];
    uint32_t best = 0;
    uint32_t bestD = 0xffffffffu;
    uint32_t i;
    if (GetDIBColorTable(NULL, 0, 256, pal) == 0)
        return 1;
    for (i = 0; i < 256; i++)
    {
        int32_t dr = (int32_t)r - (int32_t)pal[i].red;
        int32_t dg = (int32_t)g - (int32_t)pal[i].green;
        int32_t db = (int32_t)b - (int32_t)pal[i].blue;
        uint32_t d = (uint32_t)(dr * dr + dg * dg + db * db);
        if (d < bestD)
        {
            bestD = d;
            best = i;
        }
    }
    return (uint8_t)best;
}

/* Average colour of a wall-face texture mapped into the screen palette.
 * Solid spans are flat until the textured F402b10 path lands; sampling
 * the real texture keeps the flat colour data-driven. */
static uint8_t tex_face_colour(const char* name)
{
    const TexTexture* t;
    const TexFrame* f;
    uint64_t rs = 0, gs = 0, bs = 0;
    uint32_t n, k;
    if (g_menuTex == NULL || name == NULL || name[0] == '\0')
        return nearest_screen_index(40, 40, 48);
    t = Tex_Find(g_menuTex, name);
    if (t == NULL || t->frame_count == 0)
        return nearest_screen_index(40, 40, 48);
    f = &t->frames[0];
    n = f->pixel_len;
    if (n == 0 || f->pixels == NULL)
        return nearest_screen_index(40, 40, 48);
    for (k = 0; k < n; k++)
    {
        const uint8_t* p = &g_menuTex->palette[(uint32_t)f->pixels[k] * 4];
        rs += p[0];
        gs += p[1];
        bs += p[2];
    }
    return nearest_screen_index((uint8_t)(rs / n), (uint8_t)(gs / n),
                                (uint8_t)(bs / n));
}

/* Emit one wall node + edge desc.  Height (world units) rides in edge+0x54
 * for F402e00; colour is the face-texture average mapped to the screen
 * palette (flat until textured spans land). */
static void emit_wall(int i, int32_t x0, int32_t y0, int32_t x1, int32_t y1,
                      int32_t h, uint8_t col)
{
    uint8_t* node = g_wallNodes + i * WALL_NODE_SIZE;
    uint8_t* edge = g_wallEdges + i * WALL_NODE_SIZE;
    int32_t dx = x1 - x0, dy = y1 - y0;

    /* Endpoints (F4010c0 reads +0x14..+0x20). */
    *(int32_t*)(node + 0x14) = x0;
    *(int32_t*)(node + 0x18) = y0;
    *(int32_t*)(node + 0x1c) = x1;
    *(int32_t*)(node + 0x20) = y1;
    *(uint16_t*)(node + 0x24) = 0;
    /* Centre / radius (int16) for the node loop. */
    *(int16_t*)(node + 0x58) = (int16_t)((x0 + x1) / 2);
    *(int16_t*)(node + 0x5a) = (int16_t)((y0 + y1) / 2);
    {
        int32_t hx = dx < 0 ? -dx : dx;
        int32_t hy = dy < 0 ? -dy : dy;
        int32_t r = (hx > hy ? hx : hy) / 2 + 1;
        *(int16_t*)(node + 0x5c) = (int16_t)r;
    }
    *(int16_t*)(node + 0x44) = (int16_t)i;   /* group id → edge i */
    *(int16_t*)(node + 0x48) = -1;           /* neighbour A */
    *(int16_t*)(node + 0x4a) = -1;           /* neighbour B */
    *(int32_t*)(node + 0x64) = 1;            /* recompute-on-scan flag */

    /* Edge desc mirrors geometry. */
    *(int32_t*)(edge + 0x14) = x0;
    *(int32_t*)(edge + 0x18) = y0;
    *(int32_t*)(edge + 0x1c) = x1;
    *(int32_t*)(edge + 0x20) = y1;
    *(int32_t*)(edge + 0x54) = h;
    edge[0x5c] = col;
}

/* Point the span host at the harness camera (maze units). */
static void apply_camera(void)
{
    if (g_host == NULL)
        return;
    *(int32_t*)(g_host + 0x48) = g_viewX;        /* view X */
    *(int32_t*)(g_host + 0x4c) = g_viewY;        /* view Y */
}

/* (Re)fill the wall/edge pointer arrays after (re)building walls. */
static void fill_view_tables(void)
{
    void** wallPtrs = (void**)g_wallPtrs;
    void** groupPtrs = (void**)g_groupPtrs;
    int i;
    for (i = 0; i < g_wallCount; i++)
    {
        wallPtrs[i] = g_wallNodes + i * WALL_NODE_SIZE;
        groupPtrs[i] = g_wallEdges + i * WALL_NODE_SIZE;
    }
    *(void***)(g_view + 0x00) = groupPtrs;     /* groups */
    *(void***)(g_view + 0x08) = wallPtrs;      /* walls */
    *(int32_t*)(g_view + 0x10) = g_wallCount;
    *(void***)(g_view + 0x18) = wallPtrs;      /* wallByIndex */
    *(void**)(g_view + 0x54) = wallPtrs;       /* M405fa0 wall table (idx) */
}

/* Build the wall list.  With a menu maze retained, each MAZ static is one
 * wall segment ((field_14,field_18)→(field_1c,field_20), height a6, face
 * texture tex[2]); otherwise a placeholder grid.  The camera goes to the
 * bbox centre of the active set. */
static void build_walls(void)
{
    int32_t minX = 0x7fffffff, minY = 0x7fffffff;
    int32_t maxX = (int32_t)0x80000000, maxY = (int32_t)0x80000000;
    int i;

    if (g_menuMaz != NULL && g_menuMaz->arrays[0].count > 0)
    {
        uint32_t n = g_menuMaz->arrays[0].count;
        uint32_t k = 0;
        if (n > WALL_MAX)
            n = WALL_MAX;
        for (i = 0; i < (int)n; i++)
        {
            const MazObject* o = &g_menuMaz->arrays[0].items[i];
            int32_t x0, y0, x1, y1;
            if (o->cls != MAZ_CLASS_STATIC)
                continue;
            x0 = (int32_t)o->u.st.field_14;
            y0 = (int32_t)o->u.st.field_18;
            x1 = (int32_t)o->u.st.field_1c;
            y1 = (int32_t)o->u.st.field_20;
            if (x0 == x1 && y0 == y1)
                continue;
            emit_wall((int)k, x0, y0, x1, y1,
                      (int32_t)o->u.st.u16_a6,
                      tex_face_colour(o->u.st.tex_name[2]));
            if (x0 < minX) minX = x0;
            if (x1 < minX) minX = x1;
            if (x0 > maxX) maxX = x0;
            if (x1 > maxX) maxX = x1;
            if (y0 < minY) minY = y0;
            if (y1 < minY) minY = y1;
            if (y0 > maxY) maxY = y0;
            if (y1 > maxY) maxY = y1;
            k++;
        }
        g_wallCount = (int32_t)k;
    }
    else
    {
        /* Outer ring: 64x64 world units, cell size 4. */
        const int32_t S = 4;
        const int32_t D = 64 * S;
        struct { int32_t x0, y0, x1, y1; } segs[] = {
            { 0, 0, D, 0 },           /* N */
            { D, 0, D, D },           /* E */
            { D, D, 0, D },           /* S */
            { 0, D, 0, 0 },           /* W */
            /* Interior block A (rows 3..6, cols 3..6) */
            { 3 * S, 3 * S, 6 * S, 3 * S },
            { 6 * S, 3 * S, 6 * S, 6 * S },
            { 6 * S, 6 * S, 3 * S, 6 * S },
            { 3 * S, 6 * S, 3 * S, 3 * S },
            /* Interior block B (rows 3..6, cols 10..13) */
            { 10 * S, 3 * S, 13 * S, 3 * S },
            { 13 * S, 3 * S, 13 * S, 6 * S },
            { 13 * S, 6 * S, 10 * S, 6 * S },
            { 10 * S, 6 * S, 10 * S, 3 * S },
            /* Mid pillars (rows 7..9, cols 7..9) */
            { 7 * S, 7 * S, 9 * S, 7 * S },
            { 9 * S, 7 * S, 9 * S, 9 * S },
            { 9 * S, 9 * S, 7 * S, 9 * S },
            { 7 * S, 9 * S, 7 * S, 7 * S },
        };
        const int32_t nSegs = (int32_t)(sizeof(segs) / sizeof(segs[0]));

        g_wallCount = nSegs;
        for (i = 0; i < nSegs; i++)
        {
            int32_t x0 = segs[i].x0, y0 = segs[i].y0;
            int32_t x1 = segs[i].x1, y1 = segs[i].y1;
            int32_t dx = x1 - x0;
            emit_wall(i, x0, y0, x1, y1, 160,
                      (dx == 0) ? 113 : 80);
            if (x0 < minX) minX = x0;
            if (x1 < minX) minX = x1;
            if (x0 > maxX) maxX = x0;
            if (x1 > maxX) maxX = x1;
            if (y0 < minY) minY = y0;
            if (y1 < minY) minY = y1;
            if (y0 > maxY) maxY = y0;
            if (y1 > maxY) maxY = y1;
        }
        /* Link ring neighbours (N→E→S→W) as an open chain — retail wall
         * neighbours form a tree (portal splits), not a closed cycle; a
         * cycle makes F405fa0 recurse forever. */
        if (nSegs >= 4)
        {
            int16_t* n0 = (int16_t*)(g_wallNodes + 0 * WALL_NODE_SIZE + 0x48);
            int16_t* n1 = (int16_t*)(g_wallNodes + 1 * WALL_NODE_SIZE + 0x48);
            int16_t* n2 = (int16_t*)(g_wallNodes + 2 * WALL_NODE_SIZE + 0x48);
            *n0 = 1; *n1 = 2; *n2 = 3;
        }
    }
    if (g_wallCount > 0 && minX <= maxX && minY <= maxY)
    {
        g_viewX = (minX + maxX) / 2;
        g_viewY = (minY + maxY) / 2;
    }
}

/* Rebuild the per-column direction table (host+0x64) from the current yaw.
 * Retail precomputes sin/cos of the FOV sweep; the harness recomputes from
 * atan2-free fixed angles.  host+0x54 is the 9-bit view angle. */
static void rebuild_dir_table(int32_t angle9)
{
    int32_t width = VIEWPORT_WIDTH;
    int32_t* tbl = (int32_t*)g_dirTable;
    /* FOV half-angle in 25-bit turns ≈ 0x2aaaab (~85/512 of a circle). */
    int32_t halfFov = 0x2aaaab;
    int32_t center = ((angle9 & 0x1ff) << 16);
    int32_t i;
    for (i = 0; i < width; i++)
    {
        /* Relative angle for column i: sweep from -halfFov..+halfFov. */
        int32_t rel = center - halfFov +
            (int32_t)(((int64_t)(2 * halfFov) * i) / (width - 1));
        /* Direction (cos, sin) of this column ray.  Use a small angle
         * table via atan2-free sin/cos approx: 25-bit turn → radians. */
        double a = (double)(rel & 0x1ffffff) * (2.0 * 3.14159265358979 /
                                                (double)0x2000000);
        tbl[i * 2]     = (int32_t)(cos(a) * 4096.0);
        tbl[i * 2 + 1] = (int32_t)(sin(a) * 4096.0);
    }
}

void __cdecl DisplayDriver_Init(HDC screen)
{
    u32 pitch = 0;
    u8* bits;
    int32_t width = VIEWPORT_WIDTH;
    int32_t i;

    if (g_disp != NULL)
        return;

    bits = GdiCompat_SurfaceBits(screen, &pitch);
    if (bits == NULL)
        return;

    g_disp = (uint8_t*)calloc(1, DISP_SIZE);
    g_host = (uint8_t*)alloc_low(HOST_SIZE);
    g_ctx = (uint8_t*)calloc(1, SUB_SIZE);
    g_sube8 = (uint8_t*)calloc(1, SUB_SIZE);
    g_hover = (uint8_t*)alloc_low(HOVER_SIZE);
    g_view = (uint8_t*)alloc_low(VIEW_SIZE);
    g_wallNodes = (uint8_t*)alloc_low(WALL_NODE_SIZE * WALL_MAX);
    g_wallEdges = (uint8_t*)alloc_low(WALL_NODE_SIZE * WALL_MAX);
    g_buckets = (uint8_t*)alloc_low((size_t)width * sizeof(int32_t));
    g_recordArena = (uint8_t*)alloc_low(0x10000);
    g_nodeArena = (uint8_t*)alloc_low(0x10000);
    /* MAP_32BIT: Setup6c90/AddPair use 32-bit address fields (retail
     * layout); truncated values must zero-extend to the real mapping. */
    g_spanArena = (uint8_t*)alloc_low(0x40);
    g_spanScratch = (uint8_t*)alloc_low(0x2000);
    g_dirTable = (uint8_t*)alloc_low((size_t)width * 2 * sizeof(int32_t));
    g_scaleTable = (uint8_t*)alloc_low((size_t)width * sizeof(int32_t));
    g_topTable = (uint8_t*)alloc_low((size_t)width * sizeof(int32_t));
    g_botTable = (uint8_t*)alloc_low((size_t)width * sizeof(int32_t));
    g_rowHeight = (uint8_t*)alloc_low(256 * sizeof(int32_t));
    g_rowColour = (uint8_t*)alloc_low(480 * sizeof(uint32_t));
    if (g_disp == NULL || g_host == NULL || g_ctx == NULL || g_sube8 == NULL ||
        g_hover == NULL || g_view == NULL || g_wallNodes == NULL ||
        g_wallEdges == NULL || g_buckets == NULL || g_recordArena == NULL ||
        g_nodeArena == NULL || g_spanArena == NULL || g_spanScratch == NULL ||
        g_dirTable == NULL || g_scaleTable == NULL ||
        g_topTable == NULL || g_botTable == NULL || g_rowHeight == NULL ||
        g_rowColour == NULL)
        return;

    /* Span host: retail stores bits as a 4-byte pointer at +0x28.  On 64-bit
     * that field only holds the low half; the full pointer lives in
     * g_nativePlanes.bits.  host+0x08 carries the native hover object. */
    *(int32_t*)(g_host + 0x20) = width;
    *(int32_t*)(g_host + 0x28) = 0;              /* bits low half */
    *(int32_t*)(g_host + 0x2c) = 240;            /* horizon */
    /* n2 (plane rebuild stride): must be a power of two so entry's
     * n2*8 advance stays inside the width*8 dir table
     * (rebuilds = width/n2, total = width*8). width itself is not a power
     * of two (640 → mask 639 → 4 sparse rebuilds × 5120 stride = OOB and
     * entry walks off the table by col 256). 1 rebuilds every column. */
    *(int32_t*)(g_host + 0x3c) = 1;
    *(int32_t*)(g_host + 0x40) = 1;              /* n: rows per solid call */
    *(void**)(g_host + 0x08) = g_hover;          /* native hover back-ptr */
    /* Per-column tables (int32_t slots; MAP_32BIT so the pointer fits). */
    *(int32_t*)(g_host + 0x30) = (int32_t)(uintptr_t)g_scaleTable;
    *(int32_t*)(g_host + 0x34) = (int32_t)(uintptr_t)g_topTable;
    *(int32_t*)(g_host + 0x38) = (int32_t)(uintptr_t)g_botTable;
    *(int32_t*)(g_host + 0x5c) = (int32_t)(uintptr_t)g_rowHeight;
    *(int32_t*)(g_host + 0x64) = (int32_t)(uintptr_t)g_dirTable;
    *(int32_t*)(g_host + 0x68) = 0;              /* mirror off */
    *(int32_t*)(g_host + 0x6c) = (int32_t)(uintptr_t)g_rowColour;
    *(int32_t*)(g_host + 0x74) = 1;              /* solid fill mode */
    *(int32_t*)(g_host + 0x50) = width / 2;      /* centre column → g_4a4544 */
    *(int32_t*)(g_host + 0x54) = 0x0a4;          /* angle9 */
    *(int32_t*)(g_host + 0x7c) = 0;              /* shade */
    /* plane-table scale: 0 keeps M406090's entry calc (scale*angle*6/512*8)
     * at the dir-table base so the first F4010c0 reads stay in bounds. The
     * old 0x10000 pushed entry ~1MB past the table and faulted. */
    *(int32_t*)(g_host + 0x88) = 0;
    /* Per-column focal length in pixels: (width/2)/tan(halfFov) with the
     * harness half-FOV 0x2aaaab/0x2000000 of a turn (30°) → 320/tan30°.
     * Retail derives this per column from its tan tables (init site
     * still unknown); the constant stands in until that is matched. */
    for (i = 0; i < width; i++)
    {
        ((int32_t*)g_scaleTable)[i] = 554;
        ((int32_t*)g_topTable)[i] = 0;
        ((int32_t*)g_botTable)[i] = 479;
        ((int32_t*)g_rowHeight)[i < 256 ? i : 255] = 1;
    }
    /* Sky/floor row gradient. */
    for (i = 0; i < 240; i++)
        ((uint32_t*)g_rowColour)[i] = 1;          /* sky: dark */
    for (; i < 480; i++)
        ((uint32_t*)g_rowColour)[i] = 40;         /* floor: sand */

    build_walls();
    apply_camera();
    rebuild_dir_table(0x0a4);

    /* Hover object: 64-bit-safe layout matching render_pipeline.c HoverObj. */
    *(void**)(g_hover + 0x00) = g_view;          /* view */
    *(int32_t*)(g_hover + 0x08) = 0;             /* lightFlag */
    *(int32_t*)(g_hover + 0x0c) = 0;             /* pitch */
    *(void**)(g_hover + 0x20) = g_recordArena;   /* bumpBase */
    *(int32_t**)(g_hover + 0x28) = (int32_t*)g_buckets;
    *(void**)(g_hover + 0x30) = g_nodeArena;     /* nodeBump */

    /* M406090 render context (this=ctx): wire the SpanChain arena, the
     * column-bucket table and the pitch so the real wall pass runs without
     * faulting (leaves F4045e0/F406d30/M402e00/M405b90/M405ce0/M405fa0 are
     * still stubs, buckets stay NULL, F403a60 is a no-op). Setup6c90 carves
     * 0xa0/0xcc chunks out of the scratch; 0x2000 covers it with margin.
     * 32-bit stores: arena fields are 4-byte packed (see Setup6c90). */
    *(int32_t*)g_spanArena = 0;                  /* size words (Setup6c90) */
    *(int32_t*)(g_spanArena + 0x18) = (int32_t)(uintptr_t)g_spanScratch;
    *(void**)(g_ctx + 0x00) = g_spanArena;       /* SpanChain arena */
    *(void**)(g_ctx + 0x40) = g_buckets;         /* column buckets (zeroed) */
    *(int32_t*)(g_ctx + 0x2c) = (int32_t)pitch;  /* pitch snapshot */

    /* View object: 64-bit-safe layout matching render_pipeline.c ViewObj.
     * groups is an array of edge-descriptor pointers indexed by the
     * wall+0x44 group id (retail a1+0x18[gid]); walls/wallByIndex share
     * the wall-node pointer array. */
    g_wallPtrs = (uint8_t*)alloc_low((size_t)WALL_MAX * sizeof(void*));
    g_groupPtrs = (uint8_t*)alloc_low((size_t)WALL_MAX * sizeof(void*));
    if (g_wallPtrs == NULL || g_groupPtrs == NULL)
        return;
    fill_view_tables();

    /* Arm DisplayEntry case 3 so DrawEntry402250 runs. */
    *(int32_t*)(g_disp + 0xac) = 3;

    g_4a4a34 = (int32_t)pitch;
    g_4a4534 = width;
    g_4a453c = 1;
    g_4a1d68 = g_host;

    /* --- display object fields DrawEntry / DisplayEntry read --- */
    *(void**)(g_disp + 0x1c) = NULL;
    *(void**)(g_disp + 0x30) = g_460970;
    *(void**)(g_disp + 0x48) = (void*)1;
    *(void**)(g_disp + 0x4c) = (void*)1;
    *(void**)(g_disp + 0x50) = (void*)1;
    *(void**)(g_disp + 0x54) = (void*)1;
    *(int32_t*)(g_disp + 0x80) = 0;
    *(int32_t*)(g_disp + 0x84) = 0;
    *(int32_t*)(g_disp + 0x88) = 128;
    *(void**)(g_disp + 0xd0) = g_ctx;
    *(int32_t*)(g_disp + 0x21c) = 0;
    *(void**)(g_disp + 0xb8) = screen;
    *(int32_t*)(g_disp + 0x3bc) = 0x10000;
    *(int32_t*)(g_disp + 0x500) = 0;
    *(int32_t*)(g_disp + 0x540) = 0;

    /* Zero-size PatBlt rect: the harness clear owns the background. */
    *(int32_t*)(g_disp + 0x1ac) = 0;
    *(int32_t*)(g_disp + 0x1b0) = 0;
    *(int32_t*)(g_disp + 0x1b4) = 0;
    *(int32_t*)(g_disp + 0x1b8) = 0;

    g_nativePlanes.host = g_host;
    g_nativePlanes.planeB = NULL;
    g_nativePlanes.ctx = g_ctx;
    g_nativePlanes.bits = bits;
    g_nativePlanes.rowColours = (uint32_t*)g_rowColour;

    /* Sub-objects: t+0x150 is F407160's view state (divisor at +0x80). */
    *(int32_t*)(g_disp + 0x150 + 0x74) = 0;
    *(int32_t*)(g_disp + 0x150 + 0x78) = 0;
    *(int32_t*)(g_disp + 0x150 + 0x7c) = 0;
    *(int32_t*)(g_disp + 0x150 + 0x80) = 0x10000;
    *(int32_t*)(g_disp + 0x150 + 0x88) = 0;
    *(int32_t*)(g_disp + 0x150 + 0xac) = 0;
    *(void**)(g_disp + 0xe8) = g_sube8;

    /* World plane subject for M406090. */
    if (g_460970 != NULL)
    {
        uint8_t* world = (uint8_t*)g_460970;
        *(void**)(world + 0x188) = g_view;       /* DrawEntry passes this as a1 (view) */
        *(void**)(world + 0x8390) = NULL;
        *(void**)(world + 0x83ac) = NULL;
        *(int32_t*)(world + 0x83cc) = 512;
        *(int16_t*)(world + 0x83c4) = 0;
        *(int16_t*)(world + 0x8330) = 0;
        *(int32_t*)(world + 0x8214) = 0x0a4;
        *(int32_t*)(world + 0x8218) = 0;
        *(int32_t*)(world + 0x81e0) = g_viewX;
        *(int32_t*)(world + 0x81e4) = g_viewY;
    }

    g_4a1d68 = g_host;
}

void __cdecl DisplayDriver_Sync(HDC screen)
{
    u32 pitch = 0;
    u8* bits;

    if (g_disp == NULL || g_host == NULL || g_ctx == NULL || g_view == NULL)
        return;
    /* Re-wire per frame (idempotent): Setup6c90/AddPair scribble the span
     * arena's packed 4-byte fields, and a stale planes->ctx (e.g. from an
     * earlier faulted frame) must never reach the real M406090. */
    if (g_spanArena != NULL && g_spanScratch != NULL)
    {
        *(int32_t*)g_spanArena = 0;
        *(int32_t*)(g_spanArena + 0x18) = (int32_t)(uintptr_t)g_spanScratch;
        *(void**)(g_ctx + 0x00) = g_spanArena;
    }
    if (g_buckets != NULL)
        *(void**)(g_ctx + 0x40) = g_buckets;
    if (g_wallPtrs != NULL)
        *(void**)(g_view + 0x54) = g_wallPtrs;
    if (g_nativePlanes.ctx == NULL)
        g_nativePlanes.ctx = g_ctx;
    if (g_nativePlanes.host == NULL)
        g_nativePlanes.host = g_host;
    bits = GdiCompat_SurfaceBits(screen, &pitch);
    if (bits == NULL)
        return;
    g_nativePlanes.bits = bits;
    g_4a4a34 = (int32_t)pitch;
    /* M406090 reloads the pitch from hover+0x0c mid-frame, so keep the
     * hover object in sync with the live surface (retail carries the
     * back-buffer pitch through the hover object the same way). */
    *(int32_t*)(g_hover + 0x0c) = (int32_t)pitch;
    if (g_ctx != NULL)
        *(int32_t*)(g_ctx + 0x2c) = (int32_t)pitch;
    *(void**)(g_disp + 0xb8) = screen;
    *(void**)(g_disp + 0x30) = g_460970;

    /* Menu showcase camera: retail opens on a static maze view.  The world
     * angle, player node and pipeline angle all stay at the fixed menu
     * yaw (no per-frame sweep). */
    if (g_460970 != NULL)
    {
        uint8_t* world = (uint8_t*)g_460970;
        *(int32_t*)(world + 0x8214) = 0x0a4;
        /* Menu showcase camera: park the world player node (CopyPos6c40
         * reads world+0x818c+0x54) at the maze centre so DrawEntry's
         * CopyPos→Set6c00 chain feeds the pipeline the real position. */
        *(int32_t*)(world + 0x81e0) = g_viewX;
        *(int32_t*)(world + 0x81e4) = g_viewY;
        /* Keep the pipeline's projection angle coherent with the
         * direction table (M406090 derives hover->angle from host+0x54). */
        *(int32_t*)(g_host + 0x54) = 0x0a4;
        rebuild_dir_table(0x0a4);
    }

    DrawEntry402250(g_disp, NULL);
}

void __cdecl DisplayDriver_SetCase(s32 caseSel)
{
    if (g_disp != NULL)
        *(int32_t*)(g_disp + 0xac) = caseSel;
}

void __cdecl DisplayDriver_SetMenuMaze(const MazDoc* maz, const TexArchive* tex)
{
    if (g_disp == NULL || maz == NULL)
        return;
    g_menuMaz = maz;
    g_menuTex = tex;
    build_walls();
    apply_camera();
    fill_view_tables();
}

void __cdecl DisplayDriver_Shutdown(void)
{
    if (g_disp == NULL)
        return;
    free_low(g_host, HOST_SIZE);
    free_low(g_hover, HOVER_SIZE);
    free_low(g_view, VIEW_SIZE);
    if (g_wallPtrs != NULL)
        free_low(g_wallPtrs, WALL_MAX * sizeof(void*));
    if (g_groupPtrs != NULL)
        free_low(g_groupPtrs, WALL_MAX * sizeof(void*));
    free_low(g_wallNodes, WALL_NODE_SIZE * WALL_MAX);
    free_low(g_wallEdges, WALL_NODE_SIZE * WALL_MAX);
    free_low(g_buckets, (size_t)VIEWPORT_WIDTH * sizeof(int32_t));
    free_low(g_recordArena, 0x10000);
    free_low(g_nodeArena, 0x10000);
    free_low(g_spanArena, 0x40);
    free_low(g_spanScratch, 0x2000);
    free_low(g_dirTable, (size_t)VIEWPORT_WIDTH * 2 * sizeof(int32_t));
    free_low(g_scaleTable, (size_t)VIEWPORT_WIDTH * sizeof(int32_t));
    free_low(g_topTable, (size_t)VIEWPORT_WIDTH * sizeof(int32_t));
    free_low(g_botTable, (size_t)VIEWPORT_WIDTH * sizeof(int32_t));
    free_low(g_rowHeight, 256 * sizeof(int32_t));
    free_low(g_rowColour, 480 * sizeof(uint32_t));
    free(g_ctx);
    free(g_sube8);
    free(g_disp);
    g_disp = NULL;
    g_host = NULL;
    g_hover = NULL;
    g_view = NULL;
    g_spanArena = NULL;
    g_spanScratch = NULL;
    g_4a1d68 = NULL;
    g_nativePlanes.host = NULL;
    g_nativePlanes.planeB = NULL;
    g_nativePlanes.ctx = NULL;
    g_nativePlanes.bits = NULL;
    g_nativePlanes.rowColours = NULL;
}
