#include "platform/audio.h"

/* Null audio backend for configurations that do not use SDL3 (e.g. the MSVC 5.0
 * matching build). Keeps the platform audio API linkable without a real device. */

bool __cdecl PlatAudio_Init(void) { return true; }
void __cdecl PlatAudio_Quit(void) { }

PlatWaveOut * __cdecl PlatWaveOut_Open(const PlatWaveOutDesc *desc) { return NULL; }
void __cdecl PlatWaveOut_Close(PlatWaveOut *h) { }

bool __cdecl PlatWaveOut_QueueBuffer(PlatWaveOut *h, const void *data, u32 bytes) { return false; }
u32  __cdecl PlatWaveOut_GetQueuedBytes(PlatWaveOut *h) { return 0; }
u32  __cdecl PlatWaveOut_GetPosition(PlatWaveOut *h) { return 0; }

void __cdecl PlatWaveOut_Pause(PlatWaveOut *h) { }
void __cdecl PlatWaveOut_Resume(PlatWaveOut *h) { }
void __cdecl PlatWaveOut_Reset(PlatWaveOut *h) { }

bool __cdecl PlatWaveOut_SetVolume(PlatWaveOut *h, float gain) { return false; }

u8 * __cdecl PlatAudio_LoadWav(const u8 *riff, u32 riff_len, PlatWaveFormat *format, u32 *length) { return NULL; }
bool __cdecl PlatAudio_Alloc(void **ptr, u32 size) { return false; }
void __cdecl PlatAudio_Free(void *ptr) { }
