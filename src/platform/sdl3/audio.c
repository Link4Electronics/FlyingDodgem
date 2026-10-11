#include "platform/audio.h"

#include <SDL3/SDL.h>

struct PlatWaveOut
{
    SDL_AudioStream *stream;
    SDL_AudioSpec spec;
    bool paused;
    float gain;
};

bool __cdecl PlatAudio_Init(void)
{
    return SDL_InitSubSystem(SDL_INIT_AUDIO) == 0;
}

void __cdecl PlatAudio_Quit(void)
{
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

bool __cdecl PlatAudio_Alloc(void **ptr, u32 size)
{
    if (size == 0)
        return false;
    void *p = SDL_malloc(size);
    if (p == NULL)
        return false;
    SDL_memset(p, 0, size);
    *ptr = p;
    return true;
}

void __cdecl PlatAudio_Free(void *ptr)
{
    SDL_free(ptr);
}

static SDL_AudioFormat SdlFormat(const PlatWaveFormat *fmt)
{
    /* Retail SFX are 8-bit unsigned mono (LEVLSTRT.WAV: PCM U8 @ 11025).
     * Claiming S16LE while queueing raw U8 bytes makes SDL pair adjacent
     * samples into garbage 16-bit words (audible corruption). */
    if (fmt->bits_per_sample == 8)
        return SDL_AUDIO_U8;
    if (fmt->bits_per_sample == 32)
        return SDL_AUDIO_S32LE;
    return SDL_AUDIO_S16LE;
}

PlatWaveOut * __cdecl PlatWaveOut_Open(const PlatWaveOutDesc *desc)
{
    if (desc == NULL)
        return NULL;

    SDL_AudioSpec spec;
    SDL_zero(spec);
    spec.format = SdlFormat(&desc->format);
    spec.channels = (int)desc->format.channels;
    spec.freq = (int)desc->format.sample_rate;

    SDL_AudioStream *stream = SDL_OpenAudioDeviceStream(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    if (stream == NULL)
        return NULL;

    PlatWaveOut *h = (PlatWaveOut *)SDL_malloc(sizeof(PlatWaveOut));
    if (h == NULL)
    {
        SDL_DestroyAudioStream(stream);
        return NULL;
    }
    h->stream = stream;
    h->spec = spec;
    h->paused = false;  /* OpenAudioDeviceStream starts the device */
    h->gain = 1.0f;

    SDL_ResumeAudioStreamDevice(stream);
    return h;
}

void __cdecl PlatWaveOut_Close(PlatWaveOut *h)
{
    if (h == NULL)
        return;
    if (h->stream != NULL)
        SDL_DestroyAudioStream(h->stream);
    SDL_free(h);
}

bool __cdecl PlatWaveOut_QueueBuffer(PlatWaveOut *h, const void *data, u32 bytes)
{
    if (h == NULL || h->stream == NULL || data == NULL)
        return false;
    return SDL_PutAudioStreamData(h->stream, data, (int)bytes);
}

u32 __cdecl PlatWaveOut_GetQueuedBytes(PlatWaveOut *h)
{
    if (h == NULL || h->stream == NULL)
        return 0;
    int queued = SDL_GetAudioStreamQueued(h->stream);
    /* SDL returns bytes in the *output* format; convert back to our spec */
    int frames = SDL_AUDIO_FRAMESIZE(h->spec) ? queued / SDL_AUDIO_FRAMESIZE(h->spec) : 0;
    return (u32)(frames * (h->spec.channels * SDL_AUDIO_BITSIZE(h->spec.format) / 8));
}

u32 __cdecl PlatWaveOut_GetPosition(PlatWaveOut *h)
{
    if (h == NULL || h->stream == NULL)
        return 0;
    int frames = SDL_GetAudioStreamQueued(h->stream) / SDL_AUDIO_FRAMESIZE(h->spec);
    u32 bytes_per_frame = (u32)(h->spec.channels * SDL_AUDIO_BITSIZE(h->spec.format) / 8);
    return (u32)frames * bytes_per_frame;
}

void __cdecl PlatWaveOut_Pause(PlatWaveOut *h)
{
    if (h == NULL || h->stream == NULL || h->paused)
        return;
    SDL_PauseAudioStreamDevice(h->stream);
    h->paused = true;
}

void __cdecl PlatWaveOut_Resume(PlatWaveOut *h)
{
    if (h == NULL || h->stream == NULL || !h->paused)
        return;
    SDL_ResumeAudioStreamDevice(h->stream);
    h->paused = false;
}

void __cdecl PlatWaveOut_Reset(PlatWaveOut *h)
{
    if (h == NULL || h->stream == NULL)
        return;
    SDL_ClearAudioStream(h->stream);
    h->paused = true;
}

bool __cdecl PlatWaveOut_SetVolume(PlatWaveOut *h, float gain)
{
    if (h == NULL || h->stream == NULL)
        return false;
    float clamped = gain < 0.0f ? 0.0f : (gain > 1.0f ? 1.0f : gain);
    h->gain = clamped;
    return SDL_SetAudioStreamGain(h->stream, clamped);
}

/* --- WAV (RIFF) parsing --- */

typedef struct RiffChunkHeader
{
    u8 id[4];
    u8 size[4];
} RiffChunkHeader;

typedef struct WavFormatChunk
{
    u16 audio_format;
    u16 channels;
    u32 sample_rate;
    u32 byte_rate;
    u16 block_align;
    u16 bits_per_sample;
} WavFormatChunk;

static u32 ReadU32LE(const u8 *p)
{
    return (u32)p[0] | ((u32)p[1] << 8) | ((u32)p[2] << 16) | ((u32)p[3] << 24);
}

static u16 ReadU16LE(const u8 *p)
{
    return (u16)(p[0] | (p[1] << 8));
}

static bool ChunkIs(const u8 *id, const char *name)
{
    return id[0] == (u8)name[0] && id[1] == (u8)name[1] &&
           id[2] == (u8)name[2] && id[3] == (u8)name[3];
}

u8 * __cdecl PlatAudio_LoadWav(const u8 *riff, u32 riff_len, PlatWaveFormat *format, u32 *length)
{
    if (riff == NULL || riff_len < 12 || format == NULL || length == NULL)
        return NULL;

    if (riff[0] != 'R' || riff[1] != 'I' || riff[2] != 'F' || riff[3] != 'F')
        return NULL;
    if (riff[8] != 'W' || riff[9] != 'A' || riff[10] != 'V' || riff[11] != 'E')
        return NULL;

    const u8 *p = riff + 12;
    const u8 *end = riff + riff_len;

    const u8 *fmt_data = NULL;
    u32 fmt_size = 0;
    const u8 *data_data = NULL;
    u32 data_size = 0;

    while (p + 8 <= end)
    {
        const RiffChunkHeader *chunk = (const RiffChunkHeader *)p;
        u32 chunk_size = ReadU32LE(chunk->size);
        const u8 *body = p + 8;

        if (ChunkIs(chunk->id, "fmt "))
        {
            fmt_data = body;
            fmt_size = chunk_size < 16 ? chunk_size : 16;
        }
        else if (ChunkIs(chunk->id, "data"))
        {
            data_data = body;
            data_size = chunk_size;
            break;
        }

        /* chunk bodies are word-aligned */
        u32 advance = 8 + chunk_size + (chunk_size & 1);
        if (advance == 0)
            break;
        p += advance;
    }

    if (fmt_data == NULL || data_data == NULL)
        return NULL;

    const WavFormatChunk *wfmt = (const WavFormatChunk *)fmt_data;
    if (ReadU16LE(fmt_data) != 1)   /* PCM only */
        return NULL;

    format->channels = ReadU16LE(fmt_data + 2);
    format->sample_rate = ReadU32LE(fmt_data + 4);
    format->bits_per_sample = ReadU16LE(fmt_data + 14);

    if (format->channels == 0 || format->sample_rate == 0 || format->bits_per_sample == 0)
        return NULL;

    u8 *out = (u8 *)SDL_malloc(data_size);
    if (out == NULL)
        return NULL;
    SDL_memcpy(out, data_data, data_size);

    *length = data_size;
    return out;
}
