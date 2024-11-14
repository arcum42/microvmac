#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "sys_dependencies.h"
#include "error_codes.h"

#include "UI/os_glue.h"
#include "HW/ROM/rom.h"

/* --- ROM --- */

static char *rom_path = nullptr;

[[maybe_unused]] static char *d_arg = nullptr;
[[maybe_unused]] static char *n_arg = nullptr;

#if CanGetAppPath
static MacErr_t LoadMacRomFromPrefDir(void)
{
	MacErr_t err;
	char *t = nullptr;
	char *t2 = nullptr;

	if (nullptr == pref_dir)
	{
		err = mnvm_fnfErr;
	}
	else if (mnvm_noErr != (err =
								ChildPath(pref_dir, "mnvm_rom", &t)))
	{
		/* fail */
	}
	else if (mnvm_noErr != (err =
								ChildPath(t, RomFileName, &t2)))
	{
		/* fail */
	}
	else
	{
		err = LoadMacRomFrom(t2);
	}

	MayFree(t2);
	MayFree(t);

	return err;
}
#endif

#if CanGetAppPath
static MacErr_t LoadMacRomFromAppPar(void)
{
	MacErr_t err;
	char *d = (nullptr == d_arg) ? app_parent : d_arg;
	char *t = nullptr;

	if (nullptr == d)
	{
		err = mnvm_fnfErr;
	}
	else if (mnvm_noErr != (err =
								ChildPath(d, RomFileName, &t)))
	{
		/* fail */
	}
	else
	{
		err = LoadMacRomFrom(t);
	}

	MayFree(t);

	return err;
}
#endif

bool LoadMacRom(void)
{
	MacErr_t err;

	if ((nullptr == rom_path) || (mnvm_fnfErr == (err = LoadMacRomFrom(rom_path))))
#if CanGetAppPath
		if (mnvm_fnfErr == (err = LoadMacRomFromAppPar()))
			if (mnvm_fnfErr == (err = LoadMacRomFromPrefDir()))
#endif
				if (mnvm_fnfErr == (err = LoadMacRomFrom(RomFileName)))
				{
					printf("Cannot load ROM '%s'\n", RomFileName);
				}

	return true; /* keep launching Mini vMac, regardless */
}

MacErr_t LoadMacRomFrom(const char *path)
{
	MacErr_t err;
	SDL_RWops *ROM_File;
	int File_Size;

	ROM_File = SDL_RWFromFile(path, "rb");
	if (nullptr == ROM_File)
	{
		err = mnvm_fnfErr;
	}
	else
	{
		File_Size = SDL_RWread(ROM_File, ROM, 1, kROM_Size);
		if (File_Size != kROM_Size)
		{
#ifdef FileEof
			if (FileEof(ROM_File))
#else
			if (File_Size > 0)
#endif
			{
				MacMsgOverride(kStrShortROMTitle,
							   kStrShortROMMessage);
				err = mnvm_eofErr;
			}
			else
			{
				MacMsgOverride(kStrNoReadROMTitle,
							   kStrNoReadROMMessage);
				err = mnvm_miscErr;
			}
		}
		else
		{
			bool valid = ROM_IsValid();
			if (valid)
				err = mnvm_noErr;
			else
				// Throw a misc error. Why not? FIXME.
				err = mnvm_miscErr;
		}
		SDL_RWclose(ROM_File);
	}

	return err;
}
