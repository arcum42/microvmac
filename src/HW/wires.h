#pragma once

#include <cstdint>
#include "hw_config.h"

/* the Wire variables are 1/0, not true/false */

// Note: The only enum value used outside of this file is kNumWires. 
// The others are referenced in defines in this file that are used elsewhere.

// As such, Wire could be replaced by separate variables for each wire, if needed.

#ifndef WIRES_H
#define WIRES_H

#if (CurEmMd == kEmMd_Plus)
enum {
	Wire_VIA1_iA0_SoundVolb0,
	Wire_VIA1_iA1_SoundVolb1,
	Wire_VIA1_iA2_SoundVolb2,
	Wire_VIA1_iA4_MemOverlay,
	Wire_VIA1_iA6_SCRNvPage2,
	Wire_VIA1_iA5_IWMvSel,
	Wire_VIA1_iA7_SCCwaitrq,
	Wire_VIA1_iB0_RTCdataLine,
	Wire_VIA1_iB1_RTCclock,
	Wire_VIA1_iB2_RTCunEnabled,
	Wire_VIA1_iA3_SoundBuffer,
	Wire_VIA1_iB3_MouseBtnUp,
	Wire_VIA1_iB4_MouseX2,
	Wire_VIA1_iB5_MouseY2,
	Wire_VIA1_iCB2_KybdDat,
	Wire_VIA1_iB6_SCRNbeamInVid,
	Wire_VIA1_iB7_SoundDisable,
	Wire_VIA1_InterruptRequest,
	Wire_SCCInterruptRequest,
	kNumWires
};
#endif


#if (CurEmMd == kEmMd_II)
enum {
	Wire_unknown_SoundDisable,
	Wire_unknown_SoundVolb0,
	Wire_unknown_SoundVolb1,
	Wire_unknown_SoundVolb2,
	Wire_VIA1_iA0_unknown,
	Wire_VIA1_iA1_unknown,
	Wire_VIA1_iA2_unknown,
	Wire_VIA1_iB7_unknown, /* for compatibility with SoundDisable */
	Wire_VIA2_InterruptRequest,
	Wire_VIA2_iA7_unknown,
	Wire_VIA2_iA6_unknown,
	Wire_VIA2_iB7_unknown,
	Wire_VIA2_iCB2_unknown,
	Wire_VIA2_iB2_PowerOff,
	Wire_VIA2_iB3_Addr32,
	Wire_VIA1_iA4_MemOverlay,
	Wire_VIA1_iA5_IWMvSel,
	Wire_VIA1_iA7_SCCwaitrq,
	Wire_VIA1_iB0_RTCdataLine,
	Wire_VIA1_iB1_RTCclock,
	Wire_VIA1_iB2_RTCunEnabled,
	Wire_VIA1_iA3_SCCvSync,
	Wire_VIA1_iB3_ADB_Int,
	Wire_VIA1_iB4_ADB_st0,
	Wire_VIA1_iB5_ADB_st1,
	Wire_VIA1_iCB2_ADB_Data,
	Wire_VIA1_InterruptRequest,
	Wire_SCCInterruptRequest,
	Wire_ADBMouseDisabled,
	Wire_VBLinterrupt,
	Wire_VBLintunenbl,
	kNumWires
};
#endif

#if (CurEmMd == kEmMd_Plus)

// Direct connections to wires.
#define VIA1_iA0 (Wires[Wire_VIA1_iA0_SoundVolb0])
#define VIA1_iA1 (Wires[Wire_VIA1_iA1_SoundVolb1])
#define VIA1_iA2 (Wires[Wire_VIA1_iA2_SoundVolb2])
#define VIA1_iA3 (Wires[Wire_VIA1_iA3_SoundBuffer])
#define VIA1_iA4 (Wires[Wire_VIA1_iA4_MemOverlay])
#define VIA1_iA5 (Wires[Wire_VIA1_iA5_IWMvSel])
#define VIA1_iA6 (Wires[Wire_VIA1_iA6_SCRNvPage2])
#define VIA1_iA7 (Wires[Wire_VIA1_iA7_SCCwaitrq])

#define VIA1_iB0 (Wires[Wire_VIA1_iB0_RTCdataLine])
#define VIA1_iB1 (Wires[Wire_VIA1_iB1_RTCclock])
#define VIA1_iB2 (Wires[Wire_VIA1_iB2_RTCunEnabled])
#define VIA1_iB3 (Wires[Wire_VIA1_iB3_MouseBtnUp])
#define VIA1_iB4 (Wires[Wire_VIA1_iB4_MouseX2])
#define VIA1_iB5 (Wires[Wire_VIA1_iB5_MouseY2])
#define VIA1_iB6 (Wires[Wire_VIA1_iB6_SCRNbeamInVid])
#define VIA1_iB7 (Wires[Wire_VIA1_iB7_SoundDisable])

#define VIA1_iCB2 (Wires[Wire_VIA1_iCB2_KybdDat])

#define VIA1_InterruptRequest (Wires[Wire_VIA1_InterruptRequest])
#define SCCInterruptRequest (Wires[Wire_SCCInterruptRequest])

// Aliases for direct connections.
#define SoundVolb0 VIA1_iA0
#define SoundVolb1 VIA1_iA1
#define SoundVolb2 VIA1_iA2
#define SoundBuffer VIA1_iA3
#define MemOverlay VIA1_iA4
#define IWMvSel VIA1_iA5
#define SCRNvPage2 VIA1_iA6
#define SCCwaitrq VIA1_iA7

#define RTCdataLine VIA1_iB0
#define RTCclock VIA1_iB1
#define RTCunEnabled VIA1_iB2
#define MouseBtnUp VIA1_iB3
#define MouseX2 VIA1_iB4
#define MouseY2 VIA1_iB5
#define SCRNbeamInVid VIA1_iB6
#define SoundDisable VIA1_iB7

// Aliases for functions.
#define Mouse_Enabled SCC_InterruptsEnabled

#define KYBD_ShiftInData VIA1_ShiftOutData
#define KYBD_ShiftOutData VIA1_ShiftInData

#define VIA1_iA4_ChangeNtfy MemOverlay_ChangeNtfy
#define VIA1_iCB2_ChangeNtfy Kybd_DataLineChngNtfy
#endif

#if (CurEmMd == kEmMd_II)
// Direct connections to wires.
#define VIA1_iA0 (Wires[Wire_VIA1_iA0_unknown])
#define VIA1_iA1 (Wires[Wire_VIA1_iA1_unknown])
#define VIA1_iA2 (Wires[Wire_VIA1_iA2_unknown])
#define VIA1_iA3 (Wires[Wire_VIA1_iA3_SCCvSync])
#define VIA1_iA4 (Wires[Wire_VIA1_iA4_MemOverlay])
#define VIA1_iA5 (Wires[Wire_VIA1_iA5_IWMvSel])
#define VIA1_iA7 (Wires[Wire_VIA1_iA7_SCCwaitrq])

#define VIA1_iB0 (Wires[Wire_VIA1_iB0_RTCdataLine])
#define VIA1_iB1 (Wires[Wire_VIA1_iB1_RTCclock])
#define VIA1_iB2 (Wires[Wire_VIA1_iB2_RTCunEnabled])
#define VIA1_iB3 (Wires[Wire_VIA1_iB3_ADB_Int])
#define VIA1_iB4 (Wires[Wire_VIA1_iB4_ADB_st0])
#define VIA1_iB5 (Wires[Wire_VIA1_iB5_ADB_st1])
#define VIA1_iB7 (Wires[Wire_VIA1_iB7_unknown])

#define VIA1_iCB2 (Wires[Wire_VIA1_iCB2_ADB_Data])

#define VIA2_InterruptRequest (Wires[Wire_VIA2_InterruptRequest])
#define VIA2_iA0 (Wires[Wire_VBLinterrupt])
#define VIA2_iA6 (Wires[Wire_VIA2_iA6_unknown])
#define VIA2_iA7 (Wires[Wire_VIA2_iA7_unknown])

#define VIA2_iB2 (Wires[Wire_VIA2_iB2_PowerOff])
#define VIA2_iB3 (Wires[Wire_VIA2_iB3_Addr32])
#define VIA2_iB7 (Wires[Wire_VIA2_iB7_unknown])

#define VIA2_iCB2 (Wires[Wire_VIA2_iCB2_unknown])

#define SoundDisable (Wires[Wire_unknown_SoundDisable])
#define SoundVolb0 (Wires[Wire_unknown_SoundVolb0])
#define SoundVolb1 (Wires[Wire_unknown_SoundVolb1])
#define SoundVolb2 (Wires[Wire_unknown_SoundVolb2])
#define VIA1_InterruptRequest (Wires[Wire_VIA1_InterruptRequest])
#define SCCInterruptRequest (Wires[Wire_SCCInterruptRequest])
#define ADBMouseDisabled (Wires[Wire_ADBMouseDisabled])
#define Vid_VBLintunenbl (Wires[Wire_VBLintunenbl])

// Aliases for direct connections.
#define Addr32 VIA2_iB3
#define MemOverlay VIA1_iA4
#define IWMvSel VIA1_iA5
#define SCCwaitrq VIA1_iA7
#define RTCdataLine VIA1_iB0
#define RTCclock VIA1_iB1
#define RTCunEnabled VIA1_iB2
#define ADB_Int VIA1_iB3
#define ADB_st0 VIA1_iB4
#define ADB_st1 VIA1_iB5
#define ADB_Data VIA1_iCB2
#define Vid_VBLinterrupt VIA2_iA0

// Aliases for functions.
#define VIA1_iA4_ChangeNtfy MemOverlay_ChangeNtfy
#define VIA1_iB4_ChangeNtfy ADBstate_ChangeNtfy
#define VIA1_iB5_ChangeNtfy ADBstate_ChangeNtfy
#define VIA1_iCB2_ChangeNtfy ADB_DataLineChngNtfy

#define VIA2_interruptChngNtfy VIAorSCCinterruptChngNtfy
#define VIA2_iA7_ChangeNtfy Addr32_ChangeNtfy
#define VIA2_iA6_ChangeNtfy Addr32_ChangeNtfy
#define VIA2_iB2_ChangeNtfy PowerOff_ChangeNtfy
#define VIA2_iB3_ChangeNtfy Addr32_ChangeNtfy

#define VIA2_iCA1_PulseNtfy VIA2_iCA1_Vid_VBLinterrupt_PulseNtfy
#define Vid_VBLinterrupt_PulseNotify VIA2_iCA1_Vid_VBLinterrupt_PulseNtfy

#define VIA2_iCB1_PulseNtfy VIA2_iCB1_ASC_interrupt_PulseNtfy
#define ASC_interrupt_PulseNtfy VIA2_iCB1_ASC_interrupt_PulseNtfy

#define ADB_ShiftInData VIA1_ShiftOutData
#define ADB_ShiftOutData VIA1_ShiftInData

#define Mouse_Enabled() (! ADBMouseDisabled)

#endif

extern uint8_t Wires[kNumWires];

// Aliases for functions that are used in multiple models.
#define VIA1_iB0_ChangeNtfy RTCdataLine_ChangeNtfy
#define VIA1_iB1_ChangeNtfy RTCclock_ChangeNtfy
#define VIA1_iB2_ChangeNtfy RTCunEnabled_ChangeNtfy
#define VIA1_interruptChngNtfy VIAorSCCinterruptChngNtfy
#define SCCinterruptChngNtfy VIAorSCCinterruptChngNtfy

#define VIA1_iCA1_PulseNtfy VIA1_iCA1_Sixtieth_PulseNtfy
#define Sixtieth_PulseNtfy VIA1_iCA1_Sixtieth_PulseNtfy

#define VIA1_iCA2_PulseNtfy VIA1_iCA2_RTC_OneSecond_PulseNtfy
#define RTC_OneSecond_PulseNtfy VIA1_iCA2_RTC_OneSecond_PulseNtfy

#define GetSoundInvertTime VIA1_GetT1InvertTime

#endif