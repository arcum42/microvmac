#pragma once

#include <cstdint>

#if (CurEmMd == kEmMd_Plus)
/* the Wire variables are 1/0, not true/false */

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


#define SoundVolb0 (Wires[Wire_VIA1_iA0_SoundVolb0])
#define VIA1_iA0 (Wires[Wire_VIA1_iA0_SoundVolb0])
#define SoundVolb1 (Wires[Wire_VIA1_iA1_SoundVolb1])
#define VIA1_iA1 (Wires[Wire_VIA1_iA1_SoundVolb1])
#define SoundVolb2 (Wires[Wire_VIA1_iA2_SoundVolb2])
#define VIA1_iA2 (Wires[Wire_VIA1_iA2_SoundVolb2])
#define MemOverlay (Wires[Wire_VIA1_iA4_MemOverlay])
#define VIA1_iA4 (Wires[Wire_VIA1_iA4_MemOverlay])
#define VIA1_iA4_ChangeNtfy MemOverlay_ChangeNtfy
#define SCRNvPage2 (Wires[Wire_VIA1_iA6_SCRNvPage2])
#define VIA1_iA6 (Wires[Wire_VIA1_iA6_SCRNvPage2])
#define IWMvSel (Wires[Wire_VIA1_iA5_IWMvSel])
#define VIA1_iA5 (Wires[Wire_VIA1_iA5_IWMvSel])
#define SCCwaitrq (Wires[Wire_VIA1_iA7_SCCwaitrq])
#define VIA1_iA7 (Wires[Wire_VIA1_iA7_SCCwaitrq])
#define RTCdataLine (Wires[Wire_VIA1_iB0_RTCdataLine])
#define VIA1_iB0 (Wires[Wire_VIA1_iB0_RTCdataLine])
#define VIA1_iB0_ChangeNtfy RTCdataLine_ChangeNtfy
#define RTCclock (Wires[Wire_VIA1_iB1_RTCclock])
#define VIA1_iB1 (Wires[Wire_VIA1_iB1_RTCclock])
#define VIA1_iB1_ChangeNtfy RTCclock_ChangeNtfy
#define RTCunEnabled (Wires[Wire_VIA1_iB2_RTCunEnabled])
#define VIA1_iB2 (Wires[Wire_VIA1_iB2_RTCunEnabled])
#define VIA1_iB2_ChangeNtfy RTCunEnabled_ChangeNtfy
#define SoundBuffer (Wires[Wire_VIA1_iA3_SoundBuffer])
#define VIA1_iA3 (Wires[Wire_VIA1_iA3_SoundBuffer])
#define MouseBtnUp (Wires[Wire_VIA1_iB3_MouseBtnUp])
#define VIA1_iB3 (Wires[Wire_VIA1_iB3_MouseBtnUp])
#define MouseX2 (Wires[Wire_VIA1_iB4_MouseX2])
#define VIA1_iB4 (Wires[Wire_VIA1_iB4_MouseX2])
#define MouseY2 (Wires[Wire_VIA1_iB5_MouseY2])
#define VIA1_iB5 (Wires[Wire_VIA1_iB5_MouseY2])
#define VIA1_iCB2 (Wires[Wire_VIA1_iCB2_KybdDat])
#define VIA1_iCB2_ChangeNtfy Kybd_DataLineChngNtfy
#define SCRNbeamInVid (Wires[Wire_VIA1_iB6_SCRNbeamInVid])
#define VIA1_iB6 (Wires[Wire_VIA1_iB6_SCRNbeamInVid])
#define SoundDisable (Wires[Wire_VIA1_iB7_SoundDisable])
#define VIA1_iB7 (Wires[Wire_VIA1_iB7_SoundDisable])
#define VIA1_InterruptRequest (Wires[Wire_VIA1_InterruptRequest])
#define VIA1_interruptChngNtfy VIAorSCCinterruptChngNtfy
#define SCCInterruptRequest (Wires[Wire_SCCInterruptRequest])
#define SCCinterruptChngNtfy VIAorSCCinterruptChngNtfy
#endif

#if (CurEmMd == kEmMd_II)
/* the Wire variables are 1/0, not true/false */

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


#define SoundDisable (Wires[Wire_unknown_SoundDisable])
#define SoundVolb0 (Wires[Wire_unknown_SoundVolb0])
#define SoundVolb1 (Wires[Wire_unknown_SoundVolb1])
#define SoundVolb2 (Wires[Wire_unknown_SoundVolb2])
#define VIA1_iA0 (Wires[Wire_VIA1_iA0_unknown])
#define VIA1_iA1 (Wires[Wire_VIA1_iA1_unknown])
#define VIA1_iA2 (Wires[Wire_VIA1_iA2_unknown])
#define VIA1_iB7 (Wires[Wire_VIA1_iB7_unknown])
#define VIA2_InterruptRequest (Wires[Wire_VIA2_InterruptRequest])
#define VIA2_interruptChngNtfy VIAorSCCinterruptChngNtfy
#define VIA2_iA7 (Wires[Wire_VIA2_iA7_unknown])
#define VIA2_iA7_ChangeNtfy Addr32_ChangeNtfy
#define VIA2_iA6 (Wires[Wire_VIA2_iA6_unknown])
#define VIA2_iA6_ChangeNtfy Addr32_ChangeNtfy
#define VIA2_iB7 (Wires[Wire_VIA2_iB7_unknown])
#define VIA2_iCB2 (Wires[Wire_VIA2_iCB2_unknown])
#define VIA2_iB2 (Wires[Wire_VIA2_iB2_PowerOff])
#define VIA2_iB2_ChangeNtfy PowerOff_ChangeNtfy
#define VIA2_iB3 (Wires[Wire_VIA2_iB3_Addr32])
#define Addr32 (Wires[Wire_VIA2_iB3_Addr32])
#define VIA2_iB3_ChangeNtfy Addr32_ChangeNtfy
#define MemOverlay (Wires[Wire_VIA1_iA4_MemOverlay])
#define VIA1_iA4 (Wires[Wire_VIA1_iA4_MemOverlay])
#define VIA1_iA4_ChangeNtfy MemOverlay_ChangeNtfy
#define IWMvSel (Wires[Wire_VIA1_iA5_IWMvSel])
#define VIA1_iA5 (Wires[Wire_VIA1_iA5_IWMvSel])
#define SCCwaitrq (Wires[Wire_VIA1_iA7_SCCwaitrq])
#define VIA1_iA7 (Wires[Wire_VIA1_iA7_SCCwaitrq])
#define RTCdataLine (Wires[Wire_VIA1_iB0_RTCdataLine])
#define VIA1_iB0 (Wires[Wire_VIA1_iB0_RTCdataLine])
#define VIA1_iB0_ChangeNtfy RTCdataLine_ChangeNtfy
#define RTCclock (Wires[Wire_VIA1_iB1_RTCclock])
#define VIA1_iB1 (Wires[Wire_VIA1_iB1_RTCclock])
#define VIA1_iB1_ChangeNtfy RTCclock_ChangeNtfy
#define RTCunEnabled (Wires[Wire_VIA1_iB2_RTCunEnabled])
#define VIA1_iB2 (Wires[Wire_VIA1_iB2_RTCunEnabled])
#define VIA1_iB2_ChangeNtfy RTCunEnabled_ChangeNtfy
#define VIA1_iA3 (Wires[Wire_VIA1_iA3_SCCvSync])
#define ADB_Int (Wires[Wire_VIA1_iB3_ADB_Int])
#define VIA1_iB3 (Wires[Wire_VIA1_iB3_ADB_Int])
#define ADB_st0 (Wires[Wire_VIA1_iB4_ADB_st0])
#define VIA1_iB4 (Wires[Wire_VIA1_iB4_ADB_st0])
#define VIA1_iB4_ChangeNtfy ADBstate_ChangeNtfy
#define ADB_st1 (Wires[Wire_VIA1_iB5_ADB_st1])
#define VIA1_iB5 (Wires[Wire_VIA1_iB5_ADB_st1])
#define VIA1_iB5_ChangeNtfy ADBstate_ChangeNtfy
#define ADB_Data (Wires[Wire_VIA1_iCB2_ADB_Data])
#define VIA1_iCB2 (Wires[Wire_VIA1_iCB2_ADB_Data])
#define VIA1_iCB2_ChangeNtfy ADB_DataLineChngNtfy
#define VIA1_InterruptRequest (Wires[Wire_VIA1_InterruptRequest])
#define VIA1_interruptChngNtfy VIAorSCCinterruptChngNtfy
#define SCCInterruptRequest (Wires[Wire_SCCInterruptRequest])
#define SCCinterruptChngNtfy VIAorSCCinterruptChngNtfy
#define ADBMouseDisabled (Wires[Wire_ADBMouseDisabled])
#define Vid_VBLinterrupt (Wires[Wire_VBLinterrupt])
#define VIA2_iA0 (Wires[Wire_VBLinterrupt])
#define Vid_VBLintunenbl (Wires[Wire_VBLintunenbl])

#endif

extern uint8_t Wires[kNumWires];
