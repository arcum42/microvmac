/*
	PROGMAIN.c

	Copyright (C) 2009 Bernd Schmidt, Philip Cummins, Paul C. Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with this file; see the file COPYING.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	license for more details.
*/

/*
	PROGram MAIN.
*/

#include <string.h>
#include <assert.h>
#include "sys_dependencies.h"

#include "UI/my_os_glue.h"
#include "global_glue.h"
#include "HW/hardware.h"

#include "mac_main.h"

static void EmulatedHardwareZap(void)
{
	devices_reset();
}

static void DoMacReset(void)
{
	Sony_EjectAllDisks();
	EmulatedHardwareZap();
}

void InterruptReset_Update(void)
{
	SetInterruptButton(false); // don't keep held over 1/60 sec

	if (WantMacInterrupt) {
		SetInterruptButton(true);
		WantMacInterrupt = false;
	}
	if (WantMacReset) {
		DoMacReset();
		WantMacReset = false;
	}
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

static bool InitEmulation(void)
{
	devices_init();

	EmulatedHardwareZap();
	return true;
}

extern void MainEventLoop(void);

void ProgramMain(void)
{
	/* Let's get ready to start */
	devices_setup();
	spdlog::debug("Welcome to spdlog!");
	
	if (InitEmulation() == false) {return;}
	MainEventLoop();
}
