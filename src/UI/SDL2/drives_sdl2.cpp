#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <SDL.h>

#include "CNFGRAPI.h"
#include "sys_dependencies.h"
#include "UTIL/endian.h"
#include "UI/my_os_glue.h"
#include "UI/common_os_glue.h"
#include "STRCONST.h"
#include "HW/ROM/rom.h"
#include "HW/DISK/sony_floppy.h"
#include "UI/control_mode.h"
#include "UI/SDL2/os_glue_sdl2.h"

/* --- drives --- */

SDL_RWops* Drives[NumDrives]; /* open disk image files */

bool FirstFreeDisk(tDrive *Drive_No)
{
	for (tDrive i = 0; i < NumDrives; ++i)
	{
		if (!vSonyIsInserted(i))
		{
			if (nullptr != Drive_No)
			{
				*Drive_No = i;
			}
			return true;
		}
	}
	return false;
}

bool AnyDiskInserted(void)
{
	return 0 != vSonyInsertedMask;
}

void DiskRevokeWritable(tDrive Drive_No)
{
	vSonyWritableMask &= ~((uint32_t)1 << Drive_No);
}

void DiskInsertNotify(tDrive Drive_No, bool locked)
{
	vSonyInsertedMask |= ((uint32_t)1 << Drive_No);
	if (!locked)
	{
		vSonyWritableMask |= ((uint32_t)1 << Drive_No);
	}

	QuietEnds();
}

void DiskEjectedNotify(tDrive Drive_No)
{
	vSonyWritableMask &= ~((uint32_t)1 << Drive_No);
	vSonyInsertedMask &= ~((uint32_t)1 << Drive_No);
}

void InitDrives(void)
{
	/*
		This isn't really needed, Drives[i] and DriveNames[i]
		need not have valid values when not vSonyIsInserted[i].
	*/
	for (tDrive i = 0; i < NumDrives; ++i)
	{
		Drives[i] = nullptr;
	}
}

MacErr_t vSonyTransfer(bool IsWrite, uint8_t *Buffer,
					   tDrive Drive_No, uint32_t Sony_Start, uint32_t Sony_Count,
					   uint32_t *Sony_ActCount)
{
	MacErr_t err = mnvm_miscErr;
	SDL_RWops *refnum = Drives[Drive_No];
	uint32_t NewSony_Count = 0;

	if (SDL_RWseek(refnum, Sony_Start, RW_SEEK_SET) >= 0)
	{
		if (IsWrite)
		{
			NewSony_Count = SDL_RWwrite(refnum, Buffer, 1, Sony_Count);
		}
		else
		{
			NewSony_Count = SDL_RWread(refnum, Buffer, 1, Sony_Count);
		}

		if (NewSony_Count == Sony_Count)
		{
			err = mnvm_noErr;
		}
	}

	if (nullptr != Sony_ActCount)
	{
		*Sony_ActCount = NewSony_Count;
	}

	return err; /*& figure out what really to return &*/
}

MacErr_t vSonyGetSize(tDrive Drive_No, uint32_t *Sony_Count)
{
	MacErr_t err = mnvm_miscErr;
	SDL_RWops *refnum = Drives[Drive_No];
	long v;

	if (SDL_RWseek(refnum, 0, RW_SEEK_END) >= 0)
	{
		v = SDL_RWtell(refnum);
		if (v >= 0)
		{
			*Sony_Count = v;
			err = mnvm_noErr;
		}
	}

	return err; /*& figure out what really to return &*/
}

MacErr_t vSonyEject0(tDrive Drive_No, bool deleteit)
{
	SDL_RWops *refnum = Drives[Drive_No];

	DiskEjectedNotify(Drive_No);

	SDL_RWclose(refnum);
	Drives[Drive_No] = nullptr; /* not really needed */

	return mnvm_noErr;
}

MacErr_t vSonyEject(tDrive Drive_No)
{
	return vSonyEject0(Drive_No, false);
}

MacErr_t vSonyEjectDelete(tDrive Drive_No)
{
	return vSonyEject0(Drive_No, true);
}

// TODO: complete the stub
MacErr_t vSonyGetName(uint16_t Drive_No, uint16_t *r)
{
	memcpy((void *)"TEST\0", r, 5);
	return mnvm_noErr;
}

void UnInitDrives(void)
{
	for (tDrive i = 0; i < NumDrives; ++i)
	{
		if (vSonyIsInserted(i))
		{
			(void)vSonyEject(i);
		}
	}
}

bool Sony_Insert0(SDL_RWops *refnum, bool locked, char *drivepath)
{
	tDrive Drive_No;
	bool IsOk = false;

	if (!FirstFreeDisk(&Drive_No))
	{
		MacMsg(kStrTooManyImagesTitle, kStrTooManyImagesMessage,
			   false);
	}
	else
	{
		printf("Sony_Insert0 %d\n", (int)Drive_No);

		{
			Drives[Drive_No] = refnum;
			DiskInsertNotify(Drive_No, locked);

			IsOk = true;
		}
	}

	if (!IsOk)
	{
		SDL_RWclose(refnum);
	}

	return IsOk;
}

bool Sony_Insert1(char *drivepath, bool silentfail)
{
	bool locked = false;
	printf("Sony_Insert1 %s\n", drivepath);
	SDL_RWops *refnum = SDL_RWFromFile(drivepath, "rb+");
	if (nullptr == refnum)
	{
		locked = true;
		refnum = SDL_RWFromFile(drivepath, "rb");
	}
	if (nullptr == refnum)
	{
		if (!silentfail)
		{
			MacMsg(kStrOpenFailTitle, kStrOpenFailMessage, false);
		}
	}
	else
	{
		return Sony_Insert0(refnum, locked, drivepath);
	}
	return false;
}

bool Sony_Insert1a(char *drivepath, bool silentfail)
{
	bool v;

	if (!ROM_loaded)
	{
		v = (mnvm_noErr == LoadMacRomFrom(drivepath));
	}
	else
	{
		v = Sony_Insert1(drivepath, silentfail);
	}

	return v;
}

// TODO: implement stub function (or don't, lol)
bool Sony_Insert2(char *s)
{
	return Sony_Insert1(s, true);
	// return false;
}

bool Sony_InsertIth(int i)
{
	bool v;

	if ((i > 9) || !FirstFreeDisk(nullptr))
	{
		v = false;
	}
	else
	{
		char s[] = "disk?.dsk";

		s[4] = '0' + i;

		v = Sony_Insert2(s);
	}

	return v;
}

bool LoadInitialImages(void)
{
	if (!AnyDiskInserted())
	{
		for (int i = 1; Sony_InsertIth(i); ++i)
		{
			/* stop on first error (including file not found) */
		}
	}

	return true;
}
