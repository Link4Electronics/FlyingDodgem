#include "winmm_compat.h"
#include "win32_compat.h"
#include "platform/audio.h"

#include <SDL3/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------------------------------------------------------------------
 * WINMM over SDL3: joystick, multimedia timers.  (waveOut/mmio/midi land
 * here next, over the PlatAudio backend.)
 * ------------------------------------------------------------------------- */

/* --- Joystick ------------------------------------------------------------ */

static SDL_Joystick *g_joy;   /* first physical joystick, opened on demand */
static int g_joyProbed;       /* nonzero once the device list was queried */

static SDL_Joystick *joy_handle(void)
{
    if (!g_joyProbed)
    {
        int count = 0;
        SDL_JoystickID *ids;
        g_joyProbed = 1;
        if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) != 0)
            return NULL;
        ids = SDL_GetJoysticks(&count);
        if (ids != NULL)
        {
            if (count > 0)
                g_joy = SDL_OpenJoystick(ids[0]);
            SDL_free(ids);
        }
    }
    return g_joy;
}

u32 __stdcall joyGetDevCapsA(u32 devId, WinmmJoyCaps *caps, u32 capsSize)
{
    SDL_Joystick *joy;
    const char *name;
    WinmmJoyCaps tmp;

    if (devId != JOYSTICKID1 || caps == NULL || capsSize < 4)
        return JOYERR_PARMS;
    joy = joy_handle();
    if (joy == NULL)
        return JOYERR_UNPLUGGED;
    memset(&tmp, 0, sizeof(tmp));
    tmp.mid = 0;
    tmp.pid = 0;
    name = SDL_GetJoystickName(joy);
    if (name != NULL)
    {
        size_t i;
        for (i = 0; i < 31 && name[i] != '\0'; i++)
            tmp.pname[i] = name[i];
        tmp.pname[i] = '\0';
    }
    tmp.xMin = 0;
    tmp.xMax = 65535u;
    tmp.yMin = 0;
    tmp.yMax = 65535u;
    tmp.zMin = 0;
    tmp.zMax = 65535u;
    {
        int nb = SDL_GetNumJoystickButtons(joy);
        int na = SDL_GetNumJoystickAxes(joy);
        if (nb < 0)
            nb = 0;
        if (na < 0)
            na = 0;
        if (nb > 32)
            nb = 32;
        tmp.numButtons = (u32)nb;
        tmp.maxButtons = (u32)nb;
        tmp.numAxes = (u32)(na > 3 ? 3 : na);
        tmp.maxAxes = tmp.numAxes;
    }
    tmp.periodMin = 10u;
    tmp.periodMax = 1000u;
    /* Never write past the caller's buffer (retail passes 396). */
    {
        u32 copy = capsSize < (u32)sizeof(tmp) ? capsSize : (u32)sizeof(tmp);
        memcpy(caps, &tmp, copy);
    }
    return JOYERR_NOERROR;
}

u32 __stdcall joyGetPos(u32 devId, WinmmJoyInfo *info)
{
    SDL_Joystick *joy;
    int i;
    u32 buttons = 0;

    if (devId != JOYSTICKID1 || info == NULL)
        return JOYERR_PARMS;
    joy = joy_handle();
    if (joy == NULL)
        return JOYERR_UNPLUGGED;
    SDL_UpdateJoysticks();
    info->xPos = (u32)((s32)SDL_GetJoystickAxis(joy, 0) + 32768);
    info->yPos = (u32)((s32)SDL_GetJoystickAxis(joy, 1) + 32768);
    info->zPos = (u32)((s32)SDL_GetJoystickAxis(joy, 2) + 32768);
    {
        int nb = SDL_GetNumJoystickButtons(joy);
        if (nb > 32)
            nb = 32;
        for (i = 0; i < nb; i++)
        {
            if (SDL_GetJoystickButton(joy, i))
                buttons |= 1u << i;
        }
    }
    info->buttons = buttons;
    return JOYERR_NOERROR;
}

/* --- Multimedia timers ---------------------------------------------------
 * Single-threaded: WinmmPumpTimers runs from the USER32 message pump, so
 * callbacks that can be honoured without executing retail code (event
 * set/pulse, message post) fire there.  TIME_CALLBACK_FUNCTION targets
 * 32-bit retail code and is refused. */

#define WINMM_TIMER_SLOTS 16

typedef struct WinmmTimer
{
    u8 used;
    u8 periodic;
    u8 kind;      /* TIME_CALLBACK_* high nibble */
    u32 id;
    u32 delayMs;
    u32 dueMs;
    u32 user;
    u32 msg;      /* posted message id for TIME_CALLBACK_POST_MESSAGE */
    void *hwnd;   /* its target window */
    void *event;  /* HANDLE for TIME_CALLBACK_EVENT_* */
} WinmmTimer;

static WinmmTimer g_timers[WINMM_TIMER_SLOTS];
static u32 g_timerNext = 1;

/* Forward: polled DONE completion + one-shot reaping (defined below). */
static void WinmmPumpSound(u32 nowMs);

u32 __stdcall timeGetTime(void)
{
    return (u32)SDL_GetTicks();
}

u32 __stdcall timeBeginPeriod(u32 period)
{
    (void)period;
    return TIMERR_NOERROR;
}

u32 __stdcall timeEndPeriod(u32 period)
{
    (void)period;
    return TIMERR_NOERROR;
}

u32 __stdcall timeSetEvent(u32 delayMs, u32 resolution, void *callback,
                           u32 user, u32 eventType)
{
    u32 kind = eventType & 0x30u;
    int i;

    (void)resolution;
    if (delayMs == 0)
        delayMs = 1;
    if (kind == TIME_CALLBACK_FUNCTION)
    {
        /* Retail 32-bit callbacks cannot execute in this process. */
        (void)callback;
        return 0;
    }
    for (i = 0; i < WINMM_TIMER_SLOTS; i++)
    {
        if (!g_timers[i].used)
        {
            g_timers[i].used = 1;
            g_timers[i].periodic = (eventType & TIME_PERIODIC) != 0;
            g_timers[i].kind = (u8)kind;
            g_timers[i].id = g_timerNext++;
            if (g_timerNext == 0)
                g_timerNext = 1;
            g_timers[i].delayMs = delayMs;
            g_timers[i].dueMs = (u32)SDL_GetTicks() + delayMs;
            /* POST_MESSAGE packs (hwnd, msg) as (callback, user), the
             * shape retail passes to timeSetEvent. */
            if (kind == TIME_CALLBACK_POST_MESSAGE)
            {
                g_timers[i].user = 0;
                g_timers[i].msg = user;
                g_timers[i].hwnd = callback;
                g_timers[i].event = NULL;
            }
            else
            {
                g_timers[i].user = user;
                g_timers[i].msg = 0;
                g_timers[i].hwnd = NULL;
                g_timers[i].event = callback;
            }
            return g_timers[i].id;
        }
    }
    return 0;
}

u32 __stdcall timeKillEvent(u32 timerId)
{
    int i;

    if (timerId == 0)
        return TIMERR_NOCANDO;
    for (i = 0; i < WINMM_TIMER_SLOTS; i++)
    {
        if (g_timers[i].used && g_timers[i].id == timerId)
        {
            g_timers[i].used = 0;
            return TIMERR_NOERROR;
        }
    }
    return TIMERR_NOCANDO;
}

void __cdecl WinmmPumpTimers(u32 nowMs)
{
    int i;

    for (i = 0; i < WINMM_TIMER_SLOTS; i++)
    {
        WinmmTimer *t;

        if (!g_timers[i].used)
            continue;
        t = &g_timers[i];
        if ((s32)(nowMs - t->dueMs) < 0)
            continue;
        if (t->kind == TIME_CALLBACK_EVENT_SET)
            SetEvent(t->event);
        else if (t->kind == TIME_CALLBACK_EVENT_PULSE)
        {
            SetEvent(t->event);
            ResetEvent(t->event);
        }
        else if (t->kind == TIME_CALLBACK_POST_MESSAGE)
        {
            PostMessageA(t->hwnd, t->msg, (uintptr_t)t->user, 0);
        }
        if (t->periodic)
            t->dueMs = nowMs + t->delayMs;
        else
            t->used = 0;
    }

    WinmmPumpSound(nowMs);
}

/* --- waveOut PCM playback over PlatAudio -------------------------------
 * Handles are 32-bit tokens (see header).  DONE completion is tracked per
 * handle: played bytes = total queued minus backend-queued, converted back
 * through the format ratio (the backend counts output S16LE bytes). */

#define WINMM_WAVE_SLOTS 8
#define WINMM_MAX_PENDING 16
typedef struct WinmmPending
{
    WinmmWaveHdr *hdr;
    u32 length;
} WinmmPending;

typedef struct WinmmWave
{
    u8 used;
    PlatWaveOut *stream;
    u32 inBlockAlign;
    u32 outBlockAlign;
    u32 avgBytesPerSec;
    u32 totalQueued;
    u32 callbackKind;
    void *callbackTarget;
    u32 instance;
    WinmmPending pending[WINMM_MAX_PENDING];
    u32 pendHead;
    u32 pendCount;
} WinmmWave;

static WinmmWave g_waves[WINMM_WAVE_SLOTS];

static WinmmWave *wave_from_id(u32 id)
{
    if (id == 0 || id > WINMM_WAVE_SLOTS)
        return NULL;
    if (!g_waves[id - 1].used)
        return NULL;
    return &g_waves[id - 1];
}

u32 __stdcall waveOutGetNumDevs(void)
{
    return 1;
}

u32 __stdcall waveOutGetDevCapsA(u32 devId, WinmmWaveOutCaps *caps,
                                 u32 capsSize)
{
    WinmmWaveOutCaps tmp;

    if ((devId != 0 && devId != WAVE_MAPPER) || caps == NULL || capsSize < 4)
        return MMSYSERR_BADDEVICEID;
    memset(&tmp, 0, sizeof(tmp));
    tmp.mid = 0;
    tmp.pid = 0;
    tmp.driverVersion = 0x0100;
    {
        const char *name = "SDL3 waveOut";
        size_t i;
        for (i = 0; i < 31 && name[i] != '\0'; i++)
            tmp.name[i] = name[i];
        tmp.name[i] = '\0';
    }
    tmp.formats = WAVE_FORMAT_1M08 | WAVE_FORMAT_1S08 | WAVE_FORMAT_1M16 |
                  WAVE_FORMAT_1S16 | WAVE_FORMAT_2M08 | WAVE_FORMAT_2S08 |
                  WAVE_FORMAT_2M16 | WAVE_FORMAT_2S16;
    tmp.channels = 2;
    tmp.reserved1 = 0;
    tmp.support = 0;
    memcpy(caps, &tmp, capsSize < (u32)sizeof(tmp) ? capsSize : (u32)sizeof(tmp));
    return MMSYSERR_NOERROR;
}

u32 __stdcall waveOutOpen(u32 *handleOut, u32 devId,
                           const WinmmWaveFormat *format, u32 callback,
                           u32 instance, u32 openFlags)
{
    u32 kind = openFlags & CALLBACK_TYPEMASK;
    PlatWaveFormat pf;
    PlatWaveOutDesc desc;
    PlatWaveOut *stream;
    int i;

    if (handleOut == NULL || format == NULL)
        return MMSYSERR_INVALPARAM;
    if (devId != 0 && devId != WAVE_MAPPER)
        return MMSYSERR_BADDEVICEID;
    if (format->formatTag != WAVE_FORMAT_PCM)
        return MMSYSERR_INVALPARAM;
    if (format->channels < 1 || format->channels > 2)
        return MMSYSERR_INVALPARAM;
    if (format->bitsPerSample != 8 && format->bitsPerSample != 16)
        return MMSYSERR_INVALPARAM;
    if (kind != CALLBACK_NULL && kind != CALLBACK_EVENT &&
        kind != CALLBACK_WINDOW)
        return MMSYSERR_INVALPARAM;
    pf.sample_rate = format->samplesPerSec;
    pf.channels = format->channels;
    pf.bits_per_sample = format->bitsPerSample;
    desc.format = pf;
    desc.buffer_count = 4;
    stream = PlatWaveOut_Open(&desc);
    if (stream == NULL)
        return MMSYSERR_ERROR;
    for (i = 0; i < WINMM_WAVE_SLOTS; i++)
    {
        if (!g_waves[i].used)
        {
            g_waves[i].used = 1;
            g_waves[i].stream = stream;
            g_waves[i].inBlockAlign = format->blockAlign != 0
                ? format->blockAlign
                : (u32)(format->channels * format->bitsPerSample / 8);
            g_waves[i].outBlockAlign = (u32)format->channels * 2u;
            g_waves[i].avgBytesPerSec = format->avgBytesPerSec;
            g_waves[i].totalQueued = 0;
            g_waves[i].callbackKind = kind;
            g_waves[i].callbackTarget =
                (kind == CALLBACK_NULL) ? NULL : (void *)(uintptr_t)callback;
            g_waves[i].instance = instance;
            g_waves[i].pendHead = 0;
            g_waves[i].pendCount = 0;
            *handleOut = (u32)(i + 1);
            return MMSYSERR_NOERROR;
        }
    }
    PlatWaveOut_Close(stream);
    return MMSYSERR_ERROR;
}

static void wave_notify(WinmmWave *w, u32 msg, uintptr_t p1, intptr_t p2)
{
    if (w->callbackKind == CALLBACK_EVENT)
        SetEvent((void *)w->callbackTarget);
    else if (w->callbackKind == CALLBACK_WINDOW)
        PostMessageA((void *)w->callbackTarget, msg, p1, p2);
}

u32 __stdcall waveOutPrepareHeader(u32 handle, WinmmWaveHdr *hdr, u32 hdrSize)
{
    (void)hdrSize;
    if (wave_from_id(handle) == NULL || hdr == NULL)
        return MMSYSERR_INVALPARAM;
    hdr->flags |= WHDR_PREPARED;
    hdr->flags &= ~WHDR_DONE;
    return MMSYSERR_NOERROR;
}

u32 __stdcall waveOutUnprepareHeader(u32 handle, WinmmWaveHdr *hdr,
                                     u32 hdrSize)
{
    (void)hdrSize;
    if (wave_from_id(handle) == NULL || hdr == NULL)
        return MMSYSERR_INVALPARAM;
    hdr->flags &= ~(WHDR_PREPARED | WHDR_DONE);
    return MMSYSERR_NOERROR;
}

u32 __stdcall waveOutWrite(u32 handle, WinmmWaveHdr *hdr, u32 hdrSize)
{
    WinmmWave *w = wave_from_id(handle);
    const u8 *data;

    (void)hdrSize;
    if (w == NULL || hdr == NULL)
        return MMSYSERR_INVALPARAM;
    if ((hdr->flags & WHDR_PREPARED) == 0 || hdr->bufferLength == 0)
        return MMSYSERR_INVALPARAM;
    if (w->pendCount == WINMM_MAX_PENDING)
        return MMSYSERR_ERROR;
    /* x86-layout 32-bit data pointer: buffers must live below 4GB. */
    data = (const u8 *)(uintptr_t)hdr->data;
    if (!PlatWaveOut_QueueBuffer(w->stream, data, hdr->bufferLength))
        return MMSYSERR_ERROR;
    hdr->flags |= WHDR_INQUEUE;
    hdr->flags &= ~WHDR_DONE;
    w->pending[(w->pendHead + w->pendCount) % WINMM_MAX_PENDING].hdr = hdr;
    w->pending[(w->pendHead + w->pendCount) % WINMM_MAX_PENDING].length =
        hdr->bufferLength;
    w->pendCount++;
    w->totalQueued += hdr->bufferLength;
    wave_notify(w, MM_WOM_OPEN, (uintptr_t)handle, (intptr_t)w->instance);
    return MMSYSERR_NOERROR;
}

/* Mark fully-played buffers DONE (polled from the pump, like retail's
 * driver thread would). */
static void wave_poll_done(WinmmWave *w, u32 handle)
{
    u32 queuedOut = PlatWaveOut_GetQueuedBytes(w->stream);
    u32 queuedIn;
    u32 played;

    if (w->outBlockAlign == 0)
        return;
    queuedIn = queuedOut / w->outBlockAlign * w->inBlockAlign;
    played = w->totalQueued > queuedIn ? w->totalQueued - queuedIn : 0;
    while (w->pendCount > 0)
    {
        WinmmPending *p = &w->pending[w->pendHead % WINMM_MAX_PENDING];
        if (p->length > played)
            break;
        played -= p->length;
        w->totalQueued -= p->length;
        p->hdr->flags &= ~WHDR_INQUEUE;
        p->hdr->flags |= WHDR_DONE;
        p->hdr->bytesRecorded = p->length;
        w->pendHead = (w->pendHead + 1) % WINMM_MAX_PENDING;
        w->pendCount--;
        wave_notify(w, MM_WOM_DONE, (uintptr_t)handle,
                    (intptr_t)(uintptr_t)p->hdr);
    }
}

u32 __stdcall waveOutPause(u32 handle)
{
    WinmmWave *w = wave_from_id(handle);
    if (w == NULL)
        return MMSYSERR_INVALPARAM;
    PlatWaveOut_Pause(w->stream);
    return MMSYSERR_NOERROR;
}

u32 __stdcall waveOutRestart(u32 handle)
{
    WinmmWave *w = wave_from_id(handle);
    if (w == NULL)
        return MMSYSERR_INVALPARAM;
    PlatWaveOut_Resume(w->stream);
    return MMSYSERR_NOERROR;
}

u32 __stdcall waveOutReset(u32 handle)
{
    WinmmWave *w = wave_from_id(handle);
    if (w == NULL)
        return MMSYSERR_INVALPARAM;
    PlatWaveOut_Reset(w->stream);
    /* Return every queued header DONE, like the driver does. */
    while (w->pendCount > 0)
    {
        WinmmPending *p = &w->pending[w->pendHead % WINMM_MAX_PENDING];
        w->totalQueued -= p->length;
        p->hdr->flags &= ~WHDR_INQUEUE;
        p->hdr->flags |= WHDR_DONE;
        p->hdr->bytesRecorded = p->length;
        w->pendHead = (w->pendHead + 1) % WINMM_MAX_PENDING;
        w->pendCount--;
        wave_notify(w, MM_WOM_DONE, (uintptr_t)handle,
                    (intptr_t)(uintptr_t)p->hdr);
    }
    w->totalQueued = 0;
    return MMSYSERR_NOERROR;
}

u32 __stdcall waveOutClose(u32 handle)
{
    WinmmWave *w = wave_from_id(handle);
    if (w == NULL)
        return MMSYSERR_INVALPARAM;
    wave_notify(w, MM_WOM_CLOSE, (uintptr_t)handle, (intptr_t)w->instance);
    PlatWaveOut_Close(w->stream);
    w->stream = NULL;
    w->used = 0;
    return MMSYSERR_NOERROR;
}

u32 __stdcall waveOutGetPosition(u32 handle, WinmmTime *time, u32 timeSize)
{
    WinmmWave *w = wave_from_id(handle);
    u32 queuedOut, queuedIn, played;

    (void)timeSize;
    if (w == NULL || time == NULL)
        return MMSYSERR_INVALPARAM;
    if (w->outBlockAlign == 0 || w->inBlockAlign == 0)
        return MMSYSERR_ERROR;
    queuedOut = PlatWaveOut_GetQueuedBytes(w->stream);
    queuedIn = queuedOut / w->outBlockAlign * w->inBlockAlign;
    played = w->totalQueued > queuedIn ? w->totalQueued - queuedIn : 0;
    switch (time->type)
    {
    case TIME_BYTES:
        time->bytes = played;
        break;
    case TIME_SAMPLES:
        time->sample = played / w->inBlockAlign;
        break;
    case TIME_MS:
        if (w->avgBytesPerSec == 0)
            return MMSYSERR_ERROR;
        time->ms = (u32)(((u64)played * 1000u) / w->avgBytesPerSec);
        break;
    default:
        return MMSYSERR_INVALPARAM;
    }
    return MMSYSERR_NOERROR;
}

/* --- mmio RIFF I/O -------------------------------------------------------
 * File-backed reads plus FOURCC_MEM buffers.  Retail uses these to walk
 * WAV assets (RIFF/WAVE -> fmt/data chunks). */

#define WINMM_MMIO_SLOTS 8

typedef struct WinmmMmio
{
    u8 used;
    u8 memBacked;
    FILE *f;
    const u8 *mem;
    u32 memLen;
    u32 memPos;
} WinmmMmio;

static WinmmMmio g_mmio[WINMM_MMIO_SLOTS];

static WinmmMmio *mmio_from_id(u32 id)
{
    if (id == 0 || id > WINMM_MMIO_SLOTS)
        return NULL;
    if (!g_mmio[id - 1].used)
        return NULL;
    return &g_mmio[id - 1];
}

static u32 mmio_alloc(void)
{
    int i;
    for (i = 0; i < WINMM_MMIO_SLOTS; i++)
    {
        if (!g_mmio[i].used)
        {
            g_mmio[i].used = 1;
            g_mmio[i].memBacked = 0;
            g_mmio[i].f = NULL;
            g_mmio[i].mem = NULL;
            g_mmio[i].memLen = 0;
            g_mmio[i].memPos = 0;
            return (u32)(i + 1);
        }
    }
    return 0;
}

static int mmio_seek(WinmmMmio *h, u32 pos)
{
    if (h->memBacked)
    {
        if (pos > h->memLen)
            return -1;
        h->memPos = pos;
        return 0;
    }
    return fseek(h->f, (long)pos, SEEK_SET);
}

static u32 mmio_tell(WinmmMmio *h)
{
    long p;
    if (h->memBacked)
        return h->memPos;
    p = ftell(h->f);
    return p < 0 ? 0 : (u32)p;
}

static u32 mmio_read_raw(WinmmMmio *h, void *buf, u32 bytes)
{
    size_t n;
    if (bytes == 0)
        return 0;
    if (h->memBacked)
    {
        if (h->memPos >= h->memLen)
            return 0;
        n = bytes;
        if (n > h->memLen - h->memPos)
            n = h->memLen - h->memPos;
        memcpy(buf, h->mem + h->memPos, n);
        h->memPos += (u32)n;
        return (u32)n;
    }
    n = fread(buf, 1, bytes, h->f);
    return (u32)n;
}

u32 __stdcall mmioOpenA(const char *path, WinmmFileInfo *info, u32 openFlags)
{
    u32 id;
    WinmmMmio *h;

    (void)openFlags;
    id = mmio_alloc();
    if (id == 0)
        return 0;
    h = &g_mmio[id - 1];
    if (info != NULL && info->bufData != 0)
    {
        /* Memory-backed open (FOURCC_MEM shape): base + length. */
        h->memBacked = 1;
        h->mem = (const u8 *)(uintptr_t)info->bufData;
        h->memLen = info->info[0] != 0 ? info->info[0] : info->bufCharCount;
        h->memPos = 0;
        if (h->mem == NULL || h->memLen == 0)
        {
            h->used = 0;
            return 0;
        }
        return id;
    }
    if (path == NULL)
    {
        h->used = 0;
        return 0;
    }
    h->f = fopen(path, "rb");
    if (h->f == NULL)
    {
        h->used = 0;
        return 0;
    }
    return id;
}

static u32 mmio_read_u32(WinmmMmio *h, u32 pos, u32 *out)
{
    u8 b[4];
    if (mmio_seek(h, pos) != 0)
        return 0;
    if (mmio_read_raw(h, b, 4) != 4)
        return 0;
    *out = (u32)b[0] | ((u32)b[1] << 8) | ((u32)b[2] << 16) |
           ((u32)b[3] << 24);
    return 1;
}

u32 __stdcall mmioDescend(u32 hmmio, WinmmChunkInfo *chunk,
                          const WinmmChunkInfo *parent, u32 findFlags)
{
    WinmmMmio *h = mmio_from_id(hmmio);
    u32 want;
    u32 pos;
    u32 end;

    if (h == NULL || chunk == NULL)
        return MMSYSERR_INVALPARAM;
    want = chunk->ckid;
    if (parent == NULL)
    {
        u32 tag, size, fcc;
        /* Top level: must be RIFF/WAVE. */
        if (!mmio_read_u32(h, 0, &tag) || tag != 0x46464952u)
            return MMSYSERR_ERROR;
        if (!mmio_read_u32(h, 4, &size))
            return MMSYSERR_ERROR;
        if (!mmio_read_u32(h, 8, &fcc) || fcc != 0x45564157u)
            return MMSYSERR_ERROR;
        chunk->ckid = tag;
        chunk->cksize = size;
        chunk->fccType = fcc;
        chunk->dataOffset = 12;
        chunk->flags = 0;
        return MMSYSERR_NOERROR;
    }
    if ((findFlags & (MMIO_FINDCHUNK | MMIO_FINDRIFF)) == 0)
        return MMSYSERR_INVALPARAM;
    pos = parent->dataOffset;
    end = parent->dataOffset + (parent->cksize & ~1u);
    for (;;)
    {
        u32 tag, size;
        if (pos + 8 > end)
            return MMSYSERR_ERROR;
        if (!mmio_read_u32(h, pos, &tag))
            return MMSYSERR_ERROR;
        if (!mmio_read_u32(h, pos + 4, &size))
            return MMSYSERR_ERROR;
        if (tag == want)
        {
            chunk->ckid = tag;
            chunk->cksize = size;
            chunk->fccType = 0;
            chunk->dataOffset = pos + 8;
            chunk->flags = 0;
            return MMSYSERR_NOERROR;
        }
        pos += 8 + (size + (size & 1u));
    }
}

u32 __stdcall mmioAscend(u32 hmmio, WinmmChunkInfo *chunk, u32 reserved)
{
    WinmmMmio *h = mmio_from_id(hmmio);

    (void)reserved;
    if (h == NULL || chunk == NULL)
        return MMSYSERR_INVALPARAM;
    if (mmio_seek(h, chunk->dataOffset + chunk->cksize +
                  (chunk->cksize & 1u)) != 0)
        return MMSYSERR_ERROR;
    return MMSYSERR_NOERROR;
}

u32 __stdcall mmioRead(u32 hmmio, void *buf, u32 bytes)
{
    WinmmMmio *h = mmio_from_id(hmmio);
    if (h == NULL || buf == NULL)
        return (u32)-1;
    return mmio_read_raw(h, buf, bytes);
}

u32 __stdcall mmioClose(u32 hmmio, u32 reserved)
{
    WinmmMmio *h = mmio_from_id(hmmio);

    (void)reserved;
    if (h == NULL)
        return MMSYSERR_INVALPARAM;
    if (!h->memBacked && h->f != NULL)
        fclose(h->f);
    h->f = NULL;
    h->used = 0;
    return MMSYSERR_NOERROR;
}

/* --- MIDI: no SDL3 synthesizer; zero devices, clean disable ------------- */

u32 __stdcall midiOutGetNumDevs(void)
{
    return 0;
}

u32 __stdcall midiOutGetDevCapsA(u32 devId, void *caps, u32 capsSize)
{
    (void)devId;
    (void)caps;
    (void)capsSize;
    return MMSYSERR_NODRIVER;
}

u32 __stdcall midiOutPrepareHeader(u32 handle, void *hdr, u32 hdrSize)
{
    (void)handle;
    (void)hdr;
    (void)hdrSize;
    return MMSYSERR_BADDEVICEID;
}

u32 __stdcall midiOutUnprepareHeader(u32 handle, void *hdr, u32 hdrSize)
{
    (void)handle;
    (void)hdr;
    (void)hdrSize;
    return MMSYSERR_BADDEVICEID;
}

u32 __stdcall midiOutReset(u32 handle)
{
    (void)handle;
    return MMSYSERR_BADDEVICEID;
}

/* --- one-shot sounds ----------------------------------------------------- */

#define WINMM_ONESHOT_SLOTS 8

/* How many copies of a looping sample to keep queued.  A single buffer
 * underruns the SDL stream to empty; once empty some devices stop and a
 * later re-put does not restart cleanly, so the loop dies after one pass.
 * Keep a short runway and top up from the message-pump poll. */
#define WINMM_LOOP_PRIME 3

typedef struct WinmmOneShot
{
    u8 used;
    u8 loop;       /* re-queue when drained (engine idle etc.) */
    u32 dataLen;
    PlatWaveOut *stream;
    u8 *data;
} WinmmOneShot;

static WinmmOneShot g_oneshots[WINMM_ONESHOT_SLOTS];
/* Dedicated engine channel (retail CSound slots 0x1a..0x1d: eng_idle/low/
 * norm/high). Kept separate so a looping engine sample never collides with
 * one-shot SFX and can be swapped when the throttle band changes. */
static WinmmOneShot g_engine;

static void oneshot_release(WinmmOneShot *s)
{
    if (s->stream != NULL)
        PlatWaveOut_Close(s->stream);
    s->stream = NULL;
    if (s->data != NULL)
        PlatAudio_Free(s->data);
    s->data = NULL;
    s->dataLen = 0;
    s->loop = 0;
    s->used = 0;
}

static void oneshot_reap(WinmmOneShot *s)
{
    u32 queued;

    if (!s->used || s->stream == NULL)
        return;
    queued = PlatWaveOut_GetQueuedBytes(s->stream);
    if (s->loop)
    {
        /* Top up before underrun; if we already hit empty, restart the
         * device as well (underran streams stay silent otherwise). */
        if (queued == 0)
        {
            PlatWaveOut_Resume(s->stream);
            PlatWaveOut_QueueBuffer(s->stream, s->data, s->dataLen);
        }
        else if (queued < s->dataLen)
        {
            PlatWaveOut_QueueBuffer(s->stream, s->data, s->dataLen);
        }
        return;
    }
    if (queued != 0)
        return;
    oneshot_release(s);
}

static void oneshot_poll(void)
{
    int i;
    for (i = 0; i < WINMM_ONESHOT_SLOTS; i++)
        oneshot_reap(&g_oneshots[i]);
    oneshot_reap(&g_engine);
}

/* Load a PCM WAV and open a one-shot (or looping) playback slot.
 * Returns 1 on success.  `slot` must be a free or replaceable slot. */
static u8 oneshot_start(WinmmOneShot *slot, const char *path, u32 flags)
{
    FILE *f;
    long len;
    u8 *raw = NULL;
    u8 *data = NULL;
    PlatWaveFormat fmt;
    u32 dataLen = 0;
    PlatWaveOutDesc desc;
    PlatWaveOut *stream;

    if (slot->used)
        oneshot_release(slot);
    f = fopen(path, "rb");
    if (f == NULL)
        return 0;
    if (fseek(f, 0, SEEK_END) != 0)
    {
        fclose(f);
        return 0;
    }
    len = ftell(f);
    if (len <= 0 || len > 0x1000000)
    {
        fclose(f);
        return 0;
    }
    raw = (u8 *)malloc((size_t)len);
    if (raw == NULL)
    {
        fclose(f);
        return 0;
    }
    rewind(f);
    if (fread(raw, 1, (size_t)len, f) != (size_t)len)
    {
        free(raw);
        fclose(f);
        return 0;
    }
    fclose(f);
    data = PlatAudio_LoadWav(raw, (u32)len, &fmt, &dataLen);
    free(raw);
    if (data == NULL || dataLen == 0)
        return 0;
    desc.format = fmt;
    desc.buffer_count = 2;
    stream = PlatWaveOut_Open(&desc);
    if (stream == NULL)
    {
        PlatAudio_Free(data);
        return 0;
    }
    {
        u32 copies = (flags & SND_LOOP) != 0 ? WINMM_LOOP_PRIME : 1;
        u32 n;
        for (n = 0; n < copies; n++)
        {
            if (!PlatWaveOut_QueueBuffer(stream, data, dataLen))
            {
                PlatWaveOut_Close(stream);
                PlatAudio_Free(data);
                return 0;
            }
        }
    }
    slot->used = 1;
    slot->loop = (flags & SND_LOOP) != 0 ? 1 : 0;
    slot->dataLen = dataLen;
    slot->stream = stream;
    slot->data = data;
    return 1;
}

/* Retail engine band (0x40927e): throttle 0..3 picks eng_idle/low/norm/high
 * and CSound::Play is invoked with (5, 5, 0xffff).  The native path swaps
 * the dedicated looping engine channel to the matching MIXED WAV. */
static const char *kEngineWav[4] = {
    "ENG_IDLE.WAV", "ENG_LOW.WAV", "ENG_NORM.WAV", "ENG_HIGH.WAV"
};

static int g_engineBand = -1;
static char g_engineDir[256];

void __cdecl WinmmStopEngine(void)
{
    oneshot_release(&g_engine);
    g_engineBand = -1;
}

u32 __stdcall WinmmPlayEngineBand(u32 band, const char *mixedDir)
{
    char path[512];

    if (band > 3)
        return 0;
    if (g_engineBand == (int)band && g_engine.used)
        return 1;
    if (mixedDir != NULL && mixedDir[0] != '\0')
        SDL_snprintf(g_engineDir, sizeof(g_engineDir), "%s", mixedDir);
    if (g_engineDir[0] == '\0')
        return 0;
    SDL_snprintf(path, sizeof(path), "%s/%s", g_engineDir, kEngineWav[band]);
    if (!oneshot_start(&g_engine, path, SND_ASYNC | SND_LOOP))
    {
        g_engineBand = -1;
        return 0;
    }
    g_engineBand = (int)band;
    return 1;
}

u8 __stdcall sndPlaySoundA(const char *sound, u32 flags)
{
    int i;

    if (sound == NULL)
    {
        /* Stop all one-shots (not the engine channel). */
        for (i = 0; i < WINMM_ONESHOT_SLOTS; i++)
            oneshot_release(&g_oneshots[i]);
        return 1;
    }
    if ((flags & SND_MEMORY) != 0)
        return 0;
    /* SND_LOOP without SND_ASYNC has no meaning for a file-backed one-shot;
     * retail never uses that combo for SFX. */
    if ((flags & SND_LOOP) != 0 && (flags & SND_ASYNC) == 0)
        return 0;
    for (i = 0; i < WINMM_ONESHOT_SLOTS; i++)
    {
        if (!g_oneshots[i].used)
            return oneshot_start(&g_oneshots[i], sound, flags);
    }
    /* All slots busy: recycle the oldest non-looping slot. */
    for (i = 0; i < WINMM_ONESHOT_SLOTS; i++)
    {
        if (!g_oneshots[i].loop)
            return oneshot_start(&g_oneshots[i], sound, flags);
    }
    return 0;
}

/* Poll every open wave handle for DONE completion and reap spent
 * one-shots.  Runs from WinmmPumpTimers (i.e. the message pump). */
static void WinmmPumpSound(u32 nowMs)
{
    int i;

    (void)nowMs;
    for (i = 0; i < WINMM_WAVE_SLOTS; i++)
    {
        if (g_waves[i].used)
            wave_poll_done(&g_waves[i], (u32)(i + 1));
    }
    oneshot_poll();
}
