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
#include "EMCONFIG.h"
#include "global_glue.h"
#include "HW/M68K/instruct_table_68k.h"
#include "HW/M68K/cpu_68k.h"
#include "HW/VIA/via.h"
#include "HW/VIA/via2.h"
#include "HW/DISK/int_woz_machine.h"
#include "HW/SCC/serial_comm.h"
#include "HW/RTC/rtc.h"
#include "PATCHES/rom.h"
#include "HW/SCSI/scsi.h"
#include "HW/DISK/sony_floppy.h"
#include "HW/SCREEN/screen.h"
#include "HW/VIDCARD/video.h"
#include "HW/KBRD/keyboard.h"
#include "HW/POWERMAN/power.h"
#include "HW/ADB/adb.h"
#include "HW/SOUND/apple_sound_chip.h"
#include "HW/SOUND/sound.h"
#include "HW/MOUSE/mouse.h"

#include "spdlog/spdlog.h"

#include "mac_main.h"

// Let's define a bunch of function structure thingies

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(*array))

typedef struct DevMethods {
	bool (*init)(void);
	void (*reset)(void);
	void (*starttick)(void);
	void (*endtick)(void);
	void (*subtick)(int);
	void (*timebegin)(void);
	void (*timeend)(void);
} DevMethods_t;

const DevMethods_t DEVICES[] = {
	// RTC
	{
	.init = EmRTC ? RTC_Init : nullptr,
	.reset = nullptr,
	.starttick = nullptr,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},
	// ROM
	{
	.init = ROM_Init,
	.reset = nullptr,
	.starttick = nullptr,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},
	// Memory
	{
	.init = AddrSpac_Init,
	.reset = Memory_Reset,
	.starttick = nullptr,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},
	// ICT
	{
	.init = nullptr,
	.reset = ICT_Zap,
	.starttick = nullptr,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},
	// IWM
	{
	.init = nullptr,
	.reset = IWM_Reset,
	.starttick = nullptr,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},
	// SCC
	{
	.init = nullptr,
	.reset = SCC_Reset,
	.starttick = nullptr,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},
	// SCSI
	{
	.init = nullptr,
	.reset = SCSI_Reset,
	.starttick = nullptr,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},
	// VIA1
	{
	.init = VIA1_Zap,
	.reset = VIA1_Reset,
	.starttick = nullptr,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = VIA1_ExtraTimeBegin,
	.timeend = VIA1_ExtraTimeEnd,
	},
	// VIA2
	{
	.init = nullptr,
	.reset = EmVIA2 ? VIA2_Zap : nullptr,
	.starttick = nullptr,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = EmVIA2 ? VIA2_ExtraTimeBegin : nullptr,
	.timeend = EmVIA2 ? VIA2_ExtraTimeEnd : nullptr,
	},
	// Sony disk drive
	{
	.init = nullptr,
	.reset = Sony_Reset,
	.starttick = Sony_Update,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},
	// Extn
	{
	.init = nullptr,
	.reset = Extn_Reset,
	.starttick = nullptr,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},
	// m68k
	{
	.init = nullptr,
	.reset = m68k_reset,
	.starttick = nullptr,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},
	// Mouse
	{
	.init = nullptr,
	.reset = nullptr,
	.starttick = Mouse_Update,
	.endtick = Mouse_EndTickNotify,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},
	// Classic Keyboard
	{
	.init = nullptr,
	.reset = nullptr,
	.starttick = EmClassicKbrd ? KeyBoard_Update : nullptr,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},
	// ADB
	/*{
	.init = nullptr,
	.reset = nullptr,
	.starttick = EmADB ? ADB_Update : nullptr,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},*/
	// LocalTalk
	/*{
	.init = nullptr,
	.reset = nullptr,
	.starttick = EmLocalTalk ? LocalTalkTick : nullptr,
	.endtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},*/
	// Video card
	/*{
	.init = EmVidCard ? Vid_Init : nullptr,
	.reset = nullptr,
	.starttick = EmVidCard ? Vid_Update : nullptr,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},*/
	// ASC
	{
	.init = nullptr,
	.reset = nullptr,
	.starttick = nullptr,
	.endtick = nullptr,
	.subtick = EmASC ? ASC_SubTick : nullptr,
	.timebegin = nullptr,
	.timeend = nullptr
	},
	// Sound (non-ASC)
	{
	.init = nullptr,
	.reset = nullptr,
	.starttick = nullptr,
	.endtick = nullptr,
	.subtick = (SoundEnabled && (CurEmMd != kEmMd_PB100)) ? MacSound_SubTick : nullptr,
	.timebegin = nullptr,
	.timeend = nullptr
	},
	// Screen
	{
	.init = Screen_Init,
	.reset = nullptr,
	.starttick = Sixtieth_PulseNtfy, // VBlank interrupt
	.endtick = Screen_EndTickNotify,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},
};

static void EmulatedHardwareZap(void)
{
	uint32_t i;
	for ( i = 0; i < ARRAY_SIZE(DEVICES); i++ ) {
		if (DEVICES[i].reset != nullptr) { DEVICES[i].reset(); }
	}
}

static void DoMacReset(void)
{
	Sony_EjectAllDisks();
	EmulatedHardwareZap();
}

static void InterruptReset_Update(void)
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

static void SubTickNotify(int SubTick)
{
	uint32_t i;
	for ( i = 0; i < ARRAY_SIZE(DEVICES); i++ ) {
		if (DEVICES[i].subtick != nullptr) { DEVICES[i].subtick(SubTick); }
	}
}

#define CyclesScaledPerTick (130240UL * ClockMult * kCycleScale)
#define CyclesScaledPerSubTick (CyclesScaledPerTick / kNumSubTicks)

static uint16_t SubTickCounter;

static void SubTickTaskDo(void)
{
	SubTickNotify(SubTickCounter);
	++SubTickCounter;
	if (SubTickCounter < (kNumSubTicks - 1)) {
		/*
			final SubTick handled by SubTickTaskEnd,
			since CyclesScaledPerSubTick * kNumSubTicks
			might not equal CyclesScaledPerTick.
		*/

		ICT_add(kICT_SubTick, CyclesScaledPerSubTick);
	}
}

static void SubTickTaskStart(void)
{
	SubTickCounter = 0;
	ICT_add(kICT_SubTick, CyclesScaledPerSubTick);
}

static void SubTickTaskEnd(void)
{
	SubTickNotify(kNumSubTicks - 1);
}

static void SixtiethSecondNotify(void)
{
	/*uint32_t i;
	// Begin new frame
	InterruptReset_Update();
	for ( i = 0; i < ARRAY_SIZE(DEVICES); i++ ) {
		if (DEVICES[i].starttick != nullptr) { DEVICES[i].starttick(); }
	}*/
	
	// Temporary fix to non-functional RTC
	
	Mouse_Update();
	InterruptReset_Update();
	if (EmClassicKbrd) { KeyBoard_Update(); }
	//if (EmADB) { ADB_Update(); }

	Sixtieth_PulseNtfy(); /* Vertical Blanking Interrupt */
	Sony_Update();

#if EmLocalTalk
	LocalTalkTick();
#endif
	if (EmRTC) { RTC_Interrupt(); }
	//if (EmVidCard) { Vid_Update(); }

	SubTickTaskStart();
}

static void SixtiethEndNotify(void)
{
	uint32_t i;
	SubTickTaskEnd();
	for ( i = 0; i < ARRAY_SIZE(DEVICES); i++ ) {
		if (DEVICES[i].endtick != nullptr) { DEVICES[i].endtick(); }
	}
	// End frame
}

static void ExtraTimeBeginNotify(void)
{
	uint32_t i;
	for ( i = 0; i < ARRAY_SIZE(DEVICES); i++ ) {
		if (DEVICES[i].timebegin != nullptr) { DEVICES[i].timebegin(); }
	}
}

static void ExtraTimeEndNotify(void)
{
	uint32_t i;
	for ( i = 0; i < ARRAY_SIZE(DEVICES); i++ ) {
		if (DEVICES[i].timeend != nullptr) { DEVICES[i].timeend(); }
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
	uint32_t i;
	for ( i = 0; i < ARRAY_SIZE(DEVICES); i++ ) {
		if (DEVICES[i].init != nullptr) {
			assert(DEVICES[i].init());
		}
	}

	EmulatedHardwareZap();
	return true;
}

static void ICT_DoTask(int taskid)
{
	switch (taskid) {
		case kICT_SubTick:
			SubTickTaskDo();
			break;
#if EmClassicKbrd
		case kICT_Kybd_ReceiveEndCommand:
			DoKybd_ReceiveEndCommand();
			break;
		case kICT_Kybd_ReceiveCommand:
			DoKybd_ReceiveCommand();
			break;
#endif
#if EmADB
		case kICT_ADB_NewState:
			ADB_DoNewState();
			break;
#endif
#if EmPMU
		case kICT_PMU_Task:
			PMU_DoTask();
			break;
#endif
		case kICT_VIA1_Timer1Check:
			VIA1_DoTimer1Check();
			break;
		case kICT_VIA1_Timer2Check:
			VIA1_DoTimer2Check();
			break;
#if EmVIA2
		case kICT_VIA2_Timer1Check:
			VIA2_DoTimer1Check();
			break;
		case kICT_VIA2_Timer2Check:
			VIA2_DoTimer2Check();
			break;
#endif
		default:
			ReportAbnormalID(0x1001, "unknown taskid in ICT_DoTask");
			break;
	}
}

static void ICT_DoCurrentTasks(void)
{
	int i;
	uimr m;

	// For each ICT...
	for (m = ICTactive, i = 0; m != 0; m >>=1, i += 1) {
		// If no ICT here, continue
		if (0 == (m & 1)) { continue; }
		if (ICTwhen[i] != NextiCount) { continue; }

		/*
			A Task may set the time of any task, including
			itself. But it cannot set any task to execute
			immediately, so one pass is sufficient.
		*/
		ICTactive &= ~(1 << i); // Clear active bit
		ICT_DoTask(i);
	}
}

static uint32_t ICT_DoGetNext(uint32_t maxn)
{
	int i;
	uimr m;
	uint32_t v = maxn;

	for (m = ICTactive, i = 0; m != 0; m >>=1, i += 1) {
		uint32_t d = ICTwhen[i] - NextiCount;
		if (0 == (m & 1)) { continue; }
		if (d < v) { v = d; }
	}

	return v;
}

static void m68k_go_nCycles_1(uint32_t n)
{
	uint32_t n2;
	uint32_t StopiCount = NextiCount + n;
	do {
		ICT_DoCurrentTasks();
		n2 = ICT_DoGetNext(n);
		NextiCount += n2;
		m68k_go_nCycles(n2);
		n = StopiCount - NextiCount;
	} while (n != 0);
}

static uint32_t ExtraSubTicksToDo = 0;

static void DoEmulateOneTick(void)
{
	// AutoSlow
	uint32_t NewQuietTime = QuietTime + 1;
	uint32_t NewQuietSubTicks = QuietSubTicks + kNumSubTicks;

	if (NewQuietTime > QuietTime) {
		/* if not overflow */
		QuietTime = NewQuietTime;
	}
	if (NewQuietSubTicks > QuietSubTicks) {
		/* if not overflow */
		QuietSubTicks = NewQuietSubTicks;
	}

	SixtiethSecondNotify();
	m68k_go_nCycles_1(CyclesScaledPerTick);
	SixtiethEndNotify();

	if ((uint8_t) -1 == SpeedValue) {
		ExtraSubTicksToDo = (uint32_t) -1;
	} else {
		uint32_t ExtraAdd = (kNumSubTicks << SpeedValue) - kNumSubTicks;
		uint32_t ExtraLimit = ExtraAdd << 3;

		ExtraSubTicksToDo += ExtraAdd;
		if (ExtraSubTicksToDo > ExtraLimit) {
			ExtraSubTicksToDo = ExtraLimit;
		}
	}
}

static bool MoreSubTicksToDo(void)
{
	bool v = false;

	if (ExtraTimeNotOver() && (ExtraSubTicksToDo > 0)) {
		if ( (QuietSubTicks >= 16384) && (QuietTime >= 34)) {
			ExtraSubTicksToDo = 0;
		} else {
			v = true;
		}
	}

	return v;
}

/*
	DoEmulateExtraTime is used for anything over emulation speed of 1x. It
	periodically calls ExtraTimeNotOver and stops when this returns false
	(or it is finished with emulating the extra time).
*/
static void DoEmulateExtraTime(void)
{
	if (MoreSubTicksToDo()) {
		ExtraTimeBeginNotify();
		do {
			uint32_t NewQuietSubTicks = QuietSubTicks + 1;

			if (NewQuietSubTicks > QuietSubTicks) {
				/* if not overflow */
				QuietSubTicks = NewQuietSubTicks;
			}
			m68k_go_nCycles_1(CyclesScaledPerSubTick);
			--ExtraSubTicksToDo;
		} while (MoreSubTicksToDo());
		ExtraTimeEndNotify();
	}
}

/*
	The number of ticks that have been emulated so far.
	That is, the number of times "DoEmulateOneTick" has been called.
*/
static uint32_t CurEmulatedTime = 0;

/*
	The general idea is to call DoEmulateOneTick once per tick.

	But if emulation is lagging, we'll try to catch up by calling
	DoEmulateOneTick multiple times, unless we're too far behind, in
	which case we forget it.

	If emulating one tick takes longer than a tick we don't want to sit
	here forever. So the maximum number of calls to DoEmulateOneTick is
	determined at the beginning, rather than just calling DoEmulateOneTick
	until CurEmulatedTime >= TrueEmulatedTime.
*/
static void RunEmulatedTicksToTrueTime(void)
{
	int8_t lag = OnTrueTime - CurEmulatedTime;

	if (lag > 0) {
		DoEmulateOneTick();
		CurEmulatedTime += 1;
		DoneWithDrawingForTick();

		if (lag > 8) {
			/* emulation not fast enough */
			lag = 8;
			CurEmulatedTime = OnTrueTime - lag;
		} else {
			lag -= 1;
		}

		while (ExtraTimeNotOver() && (lag > 0)) {
			/* Missed vblank due to lag; catch up */
			EmVideoDisable = true;
			DoEmulateOneTick();
			CurEmulatedTime += 1;
			lag -= 1;
			EmVideoDisable = false;
		}

		EmLagTime = lag;
	}
}

static void MainEventLoop(void)
{
	while (true) {
		WaitForNextTick();
		if (ForceMacOff) { return; }
		RunEmulatedTicksToTrueTime();
		DoEmulateExtraTime();
	}
}

void ProgramMain(void)
{
	spdlog::info("Welcome to spdlog!");
	
	if (InitEmulation() == false) {return;}
	MainEventLoop();
}
