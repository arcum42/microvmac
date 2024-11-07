#include "sys_dependencies.h"
#include "HW/SCREEN/screen.h"

extern void MacMsg(char *briefMsg, char *longMsg, bool fatal);
#include "STRCONST.h"

// Duplicate of os glue, for the moment.
#define PowOf2(p) ((uimr)1 << (p))
#define Pow2Mask(p) (PowOf2(p) - 1)
#define ModPow2(i, p) ((i) & Pow2Mask(p))
#define FloorDivPow2(i, p) ((i) >> (p))
#define FloorPow2Mult(i, p) ((i) & (~ Pow2Mask(p)))
#define CeilPow2Mult(i, p) FloorPow2Mult((i) + Pow2Mask(p), (p))
	/* warning - CeilPow2Mult evaluates p twice */

 uimr ReserveAllocOffset;
 uint8_t * ReserveAllocBigBlock = nullptr;

static void SetLongs(uint32_t *p, long n)
{
	long i;

	for (i = n; --i >= 0; ) {
		*p++ = (uint32_t) -1;
	}
}

void ReserveAllocOneBlock(uint8_t * *p, uimr n,
	uint8_t align, bool FillOnes)
{
	ReserveAllocOffset = CeilPow2Mult(ReserveAllocOffset, align);
	if (nullptr == ReserveAllocBigBlock) {
		*p = nullptr;
	} else {
		*p = ReserveAllocBigBlock + ReserveAllocOffset;
		if (FillOnes) {
			SetLongs((uint32_t *)*p, n / 4);
		}
	}
	ReserveAllocOffset += n;
}

void EmulationReserveAlloc(void)
{
	ReserveAllocOneBlock(&RAM, kRAM_Size + RAMSafetyMarginFudge, 5, false);
#if EmVidCard
	ReserveAllocOneBlock(&VidROM, kVidROM_Size, 5, false);
#endif
#if IncludeVidMem
	ReserveAllocOneBlock(&VidMem, kVidMemRAM_Size + RAMSafetyMarginFudge, 5, true);
#endif
#if SmallGlobals
	MINEM68K_ReserveAlloc();
#endif
}

extern uint8_t * ROM;
uint8_t * screencomparebuff = nullptr;
uint8_t * CLUT_final;

// video defines
#ifndef UseSDLscaling
#define UseSDLscaling 0
#endif

#if ! UseSDLscaling
#define MaxScale WindowScale
#else
#define MaxScale 1
#endif

#define CLUT_finalsz (256 * 8 * 4 * MaxScale)

#include "UI/SDL2/sound_sdl2.h"

extern tpSoundSamp TheSoundBuffer;

void ReserveAllocAll(void)
{
#if dbglog_HAVE
	dbglog_ReserveAlloc();
#endif
	ReserveAllocOneBlock(&ROM, kROM_Size, 5, false);

	ReserveAllocOneBlock(&screencomparebuff,
						 vMacScreenNumBytes, 5, true);
#if UseControlKeys
	ReserveAllocOneBlock(&CntrlDisplayBuff,
						 vMacScreenNumBytes, 5, false);
#endif

	ReserveAllocOneBlock(&CLUT_final, CLUT_finalsz, 5, false);
#if SoundEnabled
	ReserveAllocOneBlock((uint8_t **)&TheSoundBuffer,
						 dbhBufferSize, 5, false);
#endif

	EmulationReserveAlloc();
}

bool AllocMemory(void)
{
	uimr n;
	bool IsOk = false;

	ReserveAllocOffset = 0;
	ReserveAllocBigBlock = nullptr;
	ReserveAllocAll();
	n = ReserveAllocOffset;
	ReserveAllocBigBlock = (uint8_t *)calloc(1, n);
	if (nullptr == ReserveAllocBigBlock)
	{
		MacMsg(kStrOutOfMemTitle, kStrOutOfMemMessage, true);
	}
	else
	{
		ReserveAllocOffset = 0;
		ReserveAllocAll();
		if (n != ReserveAllocOffset)
		{
			/* oops, program error */
		}
		else
		{
			IsOk = true;
		}
	}

	return IsOk;
}

void UnallocMemory(void)
{
	if (nullptr != ReserveAllocBigBlock)
	{
		free((char *)ReserveAllocBigBlock);
	}
}