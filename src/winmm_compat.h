#ifndef FLYINGDODGEM_WINMM_COMPAT_H
#define FLYINGDODGEM_WINMM_COMPAT_H

#include "flyingdodgem_types.h"

/* WINMM shims over SDL3 (native-only, FLYINGDODGEM_GDI_COMPAT; the MSVC 4
 * reccmp build never compiles this).
 *
 * HOVER.EXE drives sound through waveOut + mmio (WAV assets), midiOut
 * (music), sndPlaySound and multimedia timers, and reads the joystick
 * through joyGetPos/joyGetDevCapsA.  Audio output goes through the
 * PlatAudio backend (SDL3); MIDI has no SDL3 synthesizer so the midiOut
 * device reports absent and retail disables music cleanly.
 */

#ifdef __cplusplus
extern "C" {
#endif

/* --- errors --- */
#define MMSYSERR_NOERROR   0u
#define MMSYSERR_ERROR     1u
#define MMSYSERR_BADDEVICEID 2u
#define MMSYSERR_NODRIVER  6u
#define MMSYSERR_INVALPARAM 11u
#define JOYERR_NOERROR     0u
#define JOYERR_PARMS       165u
#define JOYERR_UNPLUGGED   167u
#define TIMERR_NOERROR     0u
#define TIMERR_NOCANDO     97u

#define JOYSTICKID1        0u
#define JOYCAPS_HASPOV     0x0002u

/* Win32 JOYCAPS layout (fixed-size fields only: safe at any width). */
typedef struct WinmmJoyCaps
{
    u16 mid;
    u16 pid;
    char pname[32];
    u32 xMin;
    u32 xMax;
    u32 yMin;
    u32 yMax;
    u32 zMin;
    u32 zMax;
    u32 numButtons;
    u32 periodMin;
    u32 periodMax;
    u32 rMin;
    u32 rMax;
    u32 uMin;
    u32 uMax;
    u32 caps;
    u32 maxAxes;
    u32 numAxes;
    u32 maxButtons;
    char regKey[32];
    char oemVxD[260];
} WinmmJoyCaps;

/* Win32 JOYINFO layout (four u32s). */
typedef struct WinmmJoyInfo
{
    u32 xPos;
    u32 yPos;
    u32 zPos;
    u32 buttons;
} WinmmJoyInfo;

u32 __stdcall joyGetDevCapsA(u32 devId, WinmmJoyCaps *caps, u32 capsSize);
u32 __stdcall joyGetPos(u32 devId, WinmmJoyInfo *info);

/* --- multimedia timers --- */
#define TIME_ONESHOT    0x0000u
#define TIME_PERIODIC   0x0001u
#define TIME_CALLBACK_FUNCTION 0x0000u
#define TIME_CALLBACK_EVENT_SET 0x0010u
#define TIME_CALLBACK_EVENT_PULSE 0x0020u
#define TIME_CALLBACK_POST_MESSAGE 0x0030u

typedef void (__stdcall *WinmmTimeProc)(u32 id, u32 msg, u32 user, u32 dw1,
                                        u32 dw2);

u32 __stdcall timeGetTime(void);
u32 __stdcall timeBeginPeriod(u32 period);
u32 __stdcall timeEndPeriod(u32 period);
/* FUNCTION callbacks cannot execute (retail 32-bit code), so they fail
 * with 0; EVENT_SET/PULSE/POST_MESSAGE callbacks are honoured. */
u32 __stdcall timeSetEvent(u32 delayMs, u32 resolution, void *callback,
                           u32 user, u32 eventType);
u32 __stdcall timeKillEvent(u32 timerId);

/* Due-check, driven from the USER32 message pump (single-threaded). */
void __cdecl WinmmPumpTimers(u32 nowMs);

/* --- waveOut PCM playback over PlatAudio ---
 * Handles are 32-bit tokens (retail keeps HWAVEOUT in 4-byte slots, so a
 * 64-bit pointer would not survive the round trip).  WAVEHDR uses the exact
 * x86 layout (lpData is a 32-bit pointer: buffers handed to waveOutWrite
 * must live below 4GB, the same MAP_32BIT discipline as the 3D pipeline).
 * CALLBACK_FUNCTION/THREAD cannot execute (retail 32-bit code); CALLBACK
 * NULL/EVENT/WINDOW are honoured (DONE arrives as playback consumes). */

#define WAVE_MAPPER 0xFFFFFFFFu
#define WAVE_FORMAT_PCM 1u
#define WAVE_FORMAT_1M08 0x00000001u
#define WAVE_FORMAT_1S08 0x00000002u
#define WAVE_FORMAT_1M16 0x00000004u
#define WAVE_FORMAT_1S16 0x00000008u
#define WAVE_FORMAT_2M08 0x00000010u
#define WAVE_FORMAT_2S08 0x00000020u
#define WAVE_FORMAT_2M16 0x00000040u
#define WAVE_FORMAT_2S16 0x00000080u

#define CALLBACK_NULL    0x00000000u
#define CALLBACK_WINDOW  0x00010000u
#define CALLBACK_TASK    0x00020000u
#define CALLBACK_FUNCTION 0x00030000u
#define CALLBACK_THREAD  0x00020000u
#define CALLBACK_EVENT   0x00050000u
#define CALLBACK_TYPEMASK 0x00070000u

#define MM_WOM_OPEN  0x03BBu
#define MM_WOM_CLOSE 0x03BCu
#define MM_WOM_DONE  0x03BDu

#define WHDR_DONE      0x00000001u
#define WHDR_PREPARED  0x00000002u
#define WHDR_INQUEUE   0x00000010u

#define TIME_MS      1u
#define TIME_SAMPLES 2u
#define TIME_BYTES   4u

typedef struct WinmmWaveFormat
{
    u16 formatTag;
    u16 channels;
    u32 samplesPerSec;
    u32 avgBytesPerSec;
    u16 blockAlign;
    u16 bitsPerSample;
    u16 extraSize;
} WinmmWaveFormat;

typedef struct WinmmWaveHdr
{
    u32 data;
    u32 bufferLength;
    u32 bytesRecorded;
    u32 user;
    u32 flags;
    u32 loops;
    u32 next;
    u32 reserved;
} WinmmWaveHdr;

typedef struct WinmmWaveOutCaps
{
    u16 mid;
    u16 pid;
    u16 driverVersion;
    char name[32];
    u32 formats;
    u16 channels;
    u16 reserved1;
    u32 support;
} WinmmWaveOutCaps;

typedef struct WinmmTime
{
    u32 type;
    u32 ms;
    u32 sample;
    u32 bytes;
    u32 ticks;
    u8 smpte[8];
    u32 midi;
} WinmmTime;

u32 __stdcall waveOutGetNumDevs(void);
u32 __stdcall waveOutGetDevCapsA(u32 devId, WinmmWaveOutCaps *caps,
                                 u32 capsSize);
u32 __stdcall waveOutOpen(u32 *handleOut, u32 devId,
                           const WinmmWaveFormat *format, u32 callback,
                           u32 instance, u32 openFlags);
u32 __stdcall waveOutPrepareHeader(u32 handle, WinmmWaveHdr *hdr, u32 hdrSize);
u32 __stdcall waveOutUnprepareHeader(u32 handle, WinmmWaveHdr *hdr,
                                     u32 hdrSize);
u32 __stdcall waveOutWrite(u32 handle, WinmmWaveHdr *hdr, u32 hdrSize);
u32 __stdcall waveOutPause(u32 handle);
u32 __stdcall waveOutRestart(u32 handle);
u32 __stdcall waveOutReset(u32 handle);
u32 __stdcall waveOutClose(u32 handle);
u32 __stdcall waveOutGetPosition(u32 handle, WinmmTime *time, u32 timeSize);

/* --- mmio RIFF file/chunk I/O --- */
#define MMIO_READ      0x00000000u
#define MMIO_FINDCHUNK 0x00000010u
#define MMIO_FINDRIFF  0x00000020u
#define FOURCC_MEM     0x4D454D20u /* "MEM " */

typedef struct WinmmChunkInfo
{
    u32 ckid;
    u32 cksize;
    u32 fccType;
    u32 dataOffset;
    u32 flags;
} WinmmChunkInfo;

typedef struct WinmmFileInfo
{
    u32 flags;
    u32 fccIOProc;
    u32 ioProc;
    u32 errorRet;
    u32 task;
    u32 bufCharCount;
    u32 bufData;
    u32 bufNext;
    u32 bufEndRead;
    u32 bufEndWrite;
    u32 bufOffset;
    u32 diskOffset;
    u32 info[4];
    u32 reserved1;
    u32 reserved2;
    u32 hmmio;
} WinmmFileInfo;

u32 __stdcall mmioOpenA(const char *path, WinmmFileInfo *info, u32 openFlags);
u32 __stdcall mmioDescend(u32 hmmio, WinmmChunkInfo *chunk,
                          const WinmmChunkInfo *parent, u32 findFlags);
u32 __stdcall mmioAscend(u32 hmmio, WinmmChunkInfo *chunk, u32 reserved);
u32 __stdcall mmioRead(u32 hmmio, void *buf, u32 bytes);
u32 __stdcall mmioClose(u32 hmmio, u32 reserved);

/* --- MIDI: no SDL3 synthesizer exists, so zero devices are reported and
 * music disables itself cleanly on the retail side. */
u32 __stdcall midiOutGetNumDevs(void);
u32 __stdcall midiOutGetDevCapsA(u32 devId, void *caps, u32 capsSize);
u32 __stdcall midiOutPrepareHeader(u32 handle, void *hdr, u32 hdrSize);
u32 __stdcall midiOutUnprepareHeader(u32 handle, void *hdr, u32 hdrSize);
u32 __stdcall midiOutReset(u32 handle);

/* --- one-shot sounds --- */
#define SND_SYNC     0x00000000u
#define SND_ASYNC    0x00000001u
#define SND_NODEFAULT 0x00000002u
#define SND_MEMORY   0x00000004u
#define SND_LOOP     0x00000008u
#define SND_NOSTOP   0x00000010u
#define SND_FILENAME 0x00020000u

u8 __stdcall sndPlaySoundA(const char *sound, u32 flags);

/* Retail engine band (CSound slots eng_idle/low/norm/high, throttle 0..3).
 * `mixedDir` is the SOUNDS/MIXED directory; the first call may pass NULL
 * after a prior call has cached it.  Band 0 = idle loop (ENG_IDLE.WAV). */
u32 __stdcall WinmmPlayEngineBand(u32 band, const char *mixedDir);
void __cdecl WinmmStopEngine(void);

#ifdef __cplusplus
}
#endif

#endif /* FLYINGDODGEM_WINMM_COMPAT_H */
