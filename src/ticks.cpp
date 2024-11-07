/* task management */

#include "CNFGRAPI.h"
#include "sys_dependencies.h"
#include "UTIL/dbglog.h"
#include "ticks.h"
#include "HW/M68K/cpu_68k.h"
#include "HW/KBRD/keyboard.h"
#include "HW/VIA/via1.h"
#include "HW/hardware.h"

#ifdef _VIA_Debug
#include <stdio.h>
#endif

// common os glue
extern bool ForceMacOff;
extern uint32_t OnTrueTime;
extern bool EmVideoDisable;
extern uint8_t SpeedValue;
extern void DoneWithDrawingForTick(void);

// my os glue
extern bool ExtraTimeNotOver(void);
extern void WaitForNextTick(void);

// mac main
extern void InterruptReset_Update(void);

uint32_t QuietTime = 0;
uint32_t QuietSubTicks = 0;
//int8_t EmLagTime = 0;
static uint16_t SubTickCounter;
static uint32_t ExtraSubTicksToDo = 0;

void SubTickTaskStart(void)
{
	SubTickCounter = 0;
	ICT::add(kICT_SubTick, CyclesScaledPerSubTick);
}

void SixtiethSecondNotify(void)
{
	/*
	// Begin new frame
	InterruptReset_Update();
	devices_starttick();*/
	
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

void ExtraTimeBeginNotify(void)
{
	devices_timebegin();
}

void ExtraTimeEndNotify(void)
{
	devices_timeend();
}

void SubTickNotify(int value)
{
	devices_subtick(value);
}

void SubTickTaskDo(void)
{
	SubTickNotify(SubTickCounter);
	++SubTickCounter;
	if (SubTickCounter < (kNumSubTicks - 1)) {
		/*
			final SubTick handled by SubTickTaskEnd,
			since CyclesScaledPerSubTick * kNumSubTicks
			might not equal CyclesScaledPerTick.
		*/

		ICT::add(kICT_SubTick, CyclesScaledPerSubTick);
	}
}

void SubTickTaskEnd(void)
{
	SubTickNotify(kNumSubTicks - 1);
}

void SixtiethEndNotify(void)
{
	SubTickTaskEnd();

	devices_endtick();
	// End frame
}

// Run m68k_go_nCycles n times?
void m68k_go_nCycles_1(uint32_t n)
{
	uint32_t n2;
	uint32_t StopiCount = ICT::NextiCount + n;
	do {
		ICT::DoCurrentTasks();
		n2 = ICT::DoGetNext(n);
		ICT::NextiCount += n2;
		m68k_go_nCycles(n2);
		n = StopiCount - ICT::NextiCount;
	} while (n != 0);
}

void DoEmulateOneTick(void)
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

bool MoreSubTicksToDo(void)
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
void DoEmulateExtraTime(void)
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
void RunEmulatedTicksToTrueTime(void)
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

		//EmLagTime = lag;
	}
}

void MainEventLoop(void)
{
	while (true) {
		WaitForNextTick();
		if (ForceMacOff) { return; }
		RunEmulatedTicksToTrueTime();
		DoEmulateExtraTime();
	}
}

namespace ICT
{
	uimr active;
	iCountt when[kNumICTs];
	iCountt NextiCount = 0;

	void zap(void)
	{
		active = 0;
	}
	static void insert(int taskid, iCountt n)
	{
		when[taskid] = n;
		active |= (1 << taskid);
	}

	iCountt GetCuriCount(void)
	{
		return NextiCount - GetCyclesRemaining();
	}

	void add(int taskid, uint32_t n)
	{
		/* n must be > 0 */
		int32_t x = GetCyclesRemaining();
		uint32_t when = NextiCount - x + n;

#ifdef _VIA_Debug
		fprintf(stderr, "ICT add: %d, %d, %d\n", when, taskid, n);
#endif
		insert(taskid, when);

		if (x > (int32_t)n)
		{
			SetCyclesRemaining((int32_t)n);
			NextiCount = when;
		}
	}

	void DoTask(int taskid)
	{
		switch (taskid)
		{
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

	void DoCurrentTasks(void)
	{
		int i;
		uimr m;

		// For each ICT...
		for (m = active, i = 0; m != 0; m >>= 1, i += 1)
		{
			// If no ICT here, continue
			if (0 == (m & 1))
			{
				continue;
			}
			if (when[i] != NextiCount)
			{
				continue;
			}

			/*
				A Task may set the time of any task, including
				itself. But it cannot set any task to execute
				immediately, so one pass is sufficient.
			*/
			active &= ~(1 << i); // Clear active bit
			DoTask(i);
		}
	}

	uint32_t DoGetNext(uint32_t maxn)
	{
		int i;
		uimr m;
		uint32_t v = maxn;

		for (m = active, i = 0; m != 0; m >>= 1, i += 1)
		{
			uint32_t d = when[i] - NextiCount;
			if (0 == (m & 1))
			{
				continue;
			}
			if (d < v)
			{
				v = d;
			}
		}

		return v;
	}
};