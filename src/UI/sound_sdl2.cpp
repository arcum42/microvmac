#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <SDL.h>

#include "sys_dependencies.h"

#include "UI/os_glue.h"

#include "UTIL/endian.h"

/* --- sound --- */

typedef uint16_t trSoundTemp;

const trSoundTemp kCenterTempSound = 0x8000;
const trSoundTemp AudioStepVal = 0x0040;
const uint32_t SOUND_SAMPLERATE = 22255; /* = round(7833600 * 2 / 704) */

struct SoundR
{
	tpSoundSamp fTheSoundBuffer;
	volatile uint16_t *fPlayOffset;
	volatile uint16_t *fFillOffset;
	volatile uint16_t *fMinFilledSoundBuffs;

	volatile trSoundTemp lastv;

	bool wantplaying;
	bool HaveStartedPlaying;
};
typedef struct SoundR SoundR;

SDL_AudioDeviceID audio_device = 0;
tpSoundSamp TheSoundBuffer = nullptr;
volatile static uint16_t ThePlayOffset;
volatile static uint16_t TheFillOffset;
volatile static uint16_t MinFilledSoundBuffs;

#if dbglog_SoundBuffStats
static uint16_t MaxFilledSoundBuffs;
#endif

static uint16_t TheWriteOffset;

void Sound_Init0(void)
{
	ThePlayOffset = 0;
	TheFillOffset = 0;
	TheWriteOffset = 0;
}

void Sound_Start0(void)
{
	/* Reset variables */
	MinFilledSoundBuffs = kSoundBuffers + 1;
#if dbglog_SoundBuffStats
	MaxFilledSoundBuffs = 0;
#endif
}

tpSoundSamp Sound_BeginWrite(uint16_t n, uint16_t *actL)
{
	uint16_t ToFillLen = kAllBuffLen - (TheWriteOffset - ThePlayOffset);
	uint16_t WriteBuffContig =
		kOneBuffLen - (TheWriteOffset & kOneBuffMask);

	if (WriteBuffContig < n)
	{
		n = WriteBuffContig;
	}
	if (ToFillLen < n)
	{
		/* overwrite previous buffer */
		spdlog::debug("Sound buffer overflow.");
		TheWriteOffset -= kOneBuffLen;
	}

	*actL = n;
	return TheSoundBuffer + (TheWriteOffset & kAllBuffMask);
}

#if 4 == kLn2SoundSampSz
void ConvertSoundBlockToNative(tpSoundSamp p)
{
	for (int i = kOneBuffLen; --i >= 0;)
	{
		*p++ -= 0x8000;
	}
}
#else
#define ConvertSoundBlockToNative(p)
#endif

void Sound_WroteABlock(void)
{
#if (4 == kLn2SoundSampSz)
	uint16_t PrevWriteOffset = TheWriteOffset - kOneBuffLen;
	tpSoundSamp p = TheSoundBuffer + (PrevWriteOffset & kAllBuffMask);
#endif

	spdlog::debug("Enter Sound_WroteABlock");

	ConvertSoundBlockToNative(p);

	TheFillOffset = TheWriteOffset;

#if dbglog_SoundBuffStats
	{
		uint16_t ToPlayLen = TheFillOffset - ThePlayOffset;
		uint16_t ToPlayBuffs = ToPlayLen >> kLnOneBuffLen;

		if (ToPlayBuffs > MaxFilledSoundBuffs)
		{
			MaxFilledSoundBuffs = ToPlayBuffs;
		}
	}
#endif
}

bool Sound_EndWrite0(uint16_t actL)
{
	bool v;

	TheWriteOffset += actL;

	if (0 != (TheWriteOffset & kOneBuffMask))
	{
		v = false;
	}
	else
	{
		/* just finished a block */

		Sound_WroteABlock();

		v = true;
	}

	return v;
}

void Sound_SecondNotify0(void)
{
	if (MinFilledSoundBuffs <= kSoundBuffers)
	{
		if (MinFilledSoundBuffs > DesiredMinFilledSoundBuffs)
		{
			spdlog::debug("MinFilledSoundBuffs too high.");
			IncrNextTime();
		}
		else if (MinFilledSoundBuffs < DesiredMinFilledSoundBuffs)
		{
			spdlog::debug("MinFilledSoundBuffs too low");
			++TrueEmulatedTime;
		}
#if dbglog_SoundBuffStats
		spdlog::debug("MinFilledSoundBuffs = {}",
					  MinFilledSoundBuffs);
		spdlog::debug("MaxFilledSoundBuffs = {}",
					  MaxFilledSoundBuffs);
		MaxFilledSoundBuffs = 0;
#endif
		MinFilledSoundBuffs = kSoundBuffers + 1;
	}
}

#if 3 == kLn2SoundSampSz
#define ConvertTempSoundSampleFromNative(v) ((v) << 8)
#elif 4 == kLn2SoundSampSz
#define ConvertTempSoundSampleFromNative(v) ((v) + kCenterSound)
#else
#error "unsupported kLn2SoundSampSz"
#endif

#if 3 == kLn2SoundSampSz
#define ConvertTempSoundSampleToNative(v) ((v) >> 8)
#elif 4 == kLn2SoundSampSz
#define ConvertTempSoundSampleToNative(v) ((v) - kCenterSound)
#else
#error "unsupported kLn2SoundSampSz"
#endif

void SoundRampTo(trSoundTemp *last_val, trSoundTemp dst_val,
				 tpSoundSamp *stream, int *len)
{
	trSoundTemp diff;
	tpSoundSamp p = *stream;
	int n = *len;
	trSoundTemp v1 = *last_val;

	while ((v1 != dst_val) && (0 != n))
	{
		if (v1 > dst_val)
		{
			diff = v1 - dst_val;
			if (diff > AudioStepVal)
			{
				v1 -= AudioStepVal;
			}
			else
			{
				v1 = dst_val;
			}
		}
		else
		{
			diff = dst_val - v1;
			if (diff > AudioStepVal)
			{
				v1 += AudioStepVal;
			}
			else
			{
				v1 = dst_val;
			}
		}

		--n;
		*p++ = ConvertTempSoundSampleToNative(v1);
	}

	*stream = p;
	*len = n;
	*last_val = v1;
}

static void audio_callback(void *udata, Uint8 *stream, int len)
{
	uint16_t ToPlayLen;
	uint16_t FilledSoundBuffs;
	SoundR *datp = (SoundR *)udata;
	tpSoundSamp CurSoundBuffer = datp->fTheSoundBuffer;
	uint16_t CurPlayOffset = *datp->fPlayOffset;
	trSoundTemp v0 = datp->lastv;
	trSoundTemp v1 = v0;
	tpSoundSamp dst = (tpSoundSamp)stream;

#if kLn2SoundSampSz > 3
	len >>= (kLn2SoundSampSz - 3);
#endif

	spdlog::debug("Enter audio_callback: len = {}", len);

label_retry:
	ToPlayLen = *datp->fFillOffset - CurPlayOffset;
	FilledSoundBuffs = ToPlayLen >> kLnOneBuffLen;

	if (!datp->wantplaying)
	{
		spdlog::debug("playing end transistion");
		SoundRampTo(&v1, kCenterTempSound, &dst, &len);

		ToPlayLen = 0;
	}
	else if (!datp->HaveStartedPlaying)
	{
		spdlog::debug("playing start block");
		if ((ToPlayLen >> kLnOneBuffLen) < 8)
		{
			ToPlayLen = 0;
		}
		else
		{
			tpSoundSamp p = datp->fTheSoundBuffer + (CurPlayOffset & kAllBuffMask);
			trSoundTemp v2 = ConvertTempSoundSampleFromNative(*p);

			spdlog::debug("have enough samples to start");
			SoundRampTo(&v1, v2, &dst, &len);

			if (v1 == v2)
			{
				spdlog::debug("finished start transition");
				datp->HaveStartedPlaying = true;
			}
		}
	}

	if (0 == len) /* done */
	{
		if (FilledSoundBuffs < *datp->fMinFilledSoundBuffs)
		{
			*datp->fMinFilledSoundBuffs = FilledSoundBuffs;
		}
	}
	else if (0 == ToPlayLen)
	{
		spdlog::debug("under run");
		for (int i = 0; i < len; ++i)
		{
			*dst++ = ConvertTempSoundSampleToNative(v1);
		}
		*datp->fMinFilledSoundBuffs = 0;
	}
	else
	{
		uint16_t PlayBuffContig = kAllBuffLen - (CurPlayOffset & kAllBuffMask);
		tpSoundSamp p = CurSoundBuffer + (CurPlayOffset & kAllBuffMask);

		if (ToPlayLen > PlayBuffContig)
		{
			ToPlayLen = PlayBuffContig;
		}
		if (ToPlayLen > len)
		{
			ToPlayLen = len;
		}

		for (int i = 0; i < ToPlayLen; ++i)
		{
			*dst++ = *p++;
		}
		v1 = ConvertTempSoundSampleFromNative(p[-1]);

		CurPlayOffset += ToPlayLen;
		len -= ToPlayLen;

		*datp->fPlayOffset = CurPlayOffset;

		goto label_retry;
	}

	datp->lastv = v1;
}

static SoundR cur_audio;
static bool HaveSoundOut = false;

void Sound_Stop(void)
{
	spdlog::debug("enter Sound_Stop");

	if (cur_audio.wantplaying && HaveSoundOut)
	{
		uint16_t retry_limit = 50; /* half of a second */

		cur_audio.wantplaying = false;

	label_retry:
		if (kCenterTempSound == cur_audio.lastv)
		{
			spdlog::debug("reached kCenterTempSound");

			/* done */
		}
		else if (0 == --retry_limit)
		{
			spdlog::debug("retry limit reached");
			/* done */
		}
		else
		{
			/*
				give time back, particularly important
				if got here on a suspend event.
			*/

			spdlog::debug("busy, so sleep");

			(void)SDL_Delay(10);

			goto label_retry;
		}

		// In SDL 3, becomes SDL_PauseAudioDevice(audio_device);
		SDL_PauseAudioDevice(audio_device, 1); // Pause
	}

	spdlog::debug("leave Sound_Stop");
}

void Sound_Start(void)
{
	if ((!cur_audio.wantplaying) && HaveSoundOut)
	{
		Sound_Start0();
		cur_audio.lastv = kCenterTempSound;
		cur_audio.HaveStartedPlaying = false;
		cur_audio.wantplaying = true;

		// In SDL 3, becomes SDL_ResumeAudioDevice(audio_device);
		SDL_PauseAudioDevice(audio_device, 0); // Unpause
	}
}

void Sound_UnInit(void)
{
	if (HaveSoundOut)
	{
		SDL_CloseAudioDevice(audio_device);
	}
}

bool Sound_Init(void)
{
	SDL_AudioSpec desired;

	Sound_Init0();

	cur_audio.fTheSoundBuffer = TheSoundBuffer;
	cur_audio.fPlayOffset = &ThePlayOffset;
	cur_audio.fFillOffset = &TheFillOffset;
	cur_audio.fMinFilledSoundBuffs = &MinFilledSoundBuffs;
	cur_audio.wantplaying = false;

	desired.freq = SOUND_SAMPLERATE;

#if 3 == kLn2SoundSampSz
	desired.format = AUDIO_U8;
#elif 4 == kLn2SoundSampSz
	desired.format = AUDIO_S16SYS;
#else
#error "unsupported audio format"
#endif

	desired.channels = 1;
	desired.samples = 1024;
	desired.callback = audio_callback;
	desired.userdata = (void *)&cur_audio;

	/* Open the audio device */
	// SDL 3: audio_device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &desired);
	audio_device = SDL_OpenAudioDevice(nullptr, 0, &desired, nullptr, SDL_AUDIO_ALLOW_ANY_CHANGE);

	if (audio_device == 0)
	{
		spdlog::error("Couldn't open audio: {}\n", SDL_GetError());
	}
	else
	{
		HaveSoundOut = true;

		Sound_Start();
		/*
			This should be taken care of by LeaveSpeedStopped,
			but since takes a while to get going properly,
			start early.
		*/
	}

	return true; /* keep going, even if no sound */
}

void Sound_EndWrite(uint16_t actL)
{
	if (Sound_EndWrite0(actL))
	{
	}
}

void Sound_SecondNotify(void)
{
	if (HaveSoundOut)
	{
		Sound_SecondNotify0();
	}
}
