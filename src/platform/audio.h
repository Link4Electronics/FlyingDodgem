#ifndef FLYINGDODGEM_PLATFORM_AUDIO_H
#define FLYINGDODGEM_PLATFORM_AUDIO_H

#include "flyingdodgem_types.h"

/* Queue-based waveform playback backend, mirroring the WinMM waveOut model that
 * the original HOVER.EXE uses. The underlying implementation is SDL3 audio for
 * the native build; a null/stub implementation is used for other configurations. */

typedef struct PlatWaveFormat
{
    u32 sample_rate;     /* samples per second */
    u16 channels;        /* 1 = mono, 2 = stereo */
    u16 bits_per_sample; /* 8 or 16 */
} PlatWaveFormat;

typedef struct PlatWaveOut PlatWaveOut;

typedef struct PlatWaveOutDesc
{
    PlatWaveFormat format;
    u32 buffer_count;  /* number of queued buffers to allow */
} PlatWaveOutDesc;

bool __cdecl PlatAudio_Init(void);
void __cdecl PlatAudio_Quit(void);

PlatWaveOut * __cdecl PlatWaveOut_Open(const PlatWaveOutDesc *desc);
void __cdecl PlatWaveOut_Close(PlatWaveOut *h);

bool __cdecl PlatWaveOut_QueueBuffer(PlatWaveOut *h, const void *data, u32 bytes);
u32  __cdecl PlatWaveOut_GetQueuedBytes(PlatWaveOut *h);
u32  __cdecl PlatWaveOut_GetPosition(PlatWaveOut *h);

void __cdecl PlatWaveOut_Pause(PlatWaveOut *h);
void __cdecl PlatWaveOut_Resume(PlatWaveOut *h);
void __cdecl PlatWaveOut_Reset(PlatWaveOut *h);

bool __cdecl PlatWaveOut_SetVolume(PlatWaveOut *h, float gain);

/* WAV (RIFF) loading. Returns the decoded sample buffer (caller frees with
 * PlatAudio_Free), fills *format, and sets *length to the byte length. */
u8 * __cdecl PlatAudio_LoadWav(const u8 *riff, u32 riff_len, PlatWaveFormat *format, u32 *length);
bool __cdecl PlatAudio_Alloc(void **ptr, u32 size);
void __cdecl PlatAudio_Free(void *ptr);

#endif
