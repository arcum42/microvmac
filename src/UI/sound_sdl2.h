#pragma once
#include "sys_dependencies.h"

// sound defines
#define kLn2SoundBuffers 4 /* kSoundBuffers must be a power of two */
#define kSoundBuffers (1 << kLn2SoundBuffers)
#define kSoundBuffMask (kSoundBuffers - 1)

#define DesiredMinFilledSoundBuffs 3
/*
	if too big then sound lags behind emulation.
	if too small then sound will have pauses.
*/

#define kLnOneBuffLen 9
#define kLnAllBuffLen (kLn2SoundBuffers + kLnOneBuffLen)
#define kOneBuffLen (1UL << kLnOneBuffLen)
#define kAllBuffLen (1UL << kLnAllBuffLen)
#define kLnOneBuffSz (kLnOneBuffLen + kLn2SoundSampSz - 3)
#define kLnAllBuffSz (kLnAllBuffLen + kLn2SoundSampSz - 3)
#define kOneBuffSz (1UL << kLnOneBuffSz)
#define kAllBuffSz (1UL << kLnAllBuffSz)
#define kOneBuffMask (kOneBuffLen - 1)
#define kAllBuffMask (kAllBuffLen - 1)
#define dbhBufferSize (kAllBuffSz + kOneBuffSz)

#define dbglog_SoundStuff (0 && dbglog_HAVE)
#define dbglog_SoundBuffStats (0 && dbglog_HAVE)

#if 3 == kLn2SoundSampSz
typedef uint8_t trSoundSamp;
typedef uint8_t tbSoundSamp;
typedef uint8_t *tpSoundSamp;
constexpr uint32_t kCenterSound = 0x80;
#elif 4 == kLn2SoundSampSz
typedef uint16_t trSoundSamp;
typedef uint16_t tbSoundSamp;
typedef uint16_t *tpSoundSamp;
constexpr uint32_t kCenterSound = 0x8000;
#else
#error "unsupported kLn2SoundSampSz"
#endif

extern tpSoundSamp Sound_BeginWrite(uint16_t n, uint16_t *actL);
extern void Sound_EndWrite(uint16_t actL);

/* 370 samples per tick = 22,254.54 per second */
