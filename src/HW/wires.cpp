
#include <cstdint>
#include "wires.h"

uint8_t Wires[kNumWires];

// The way most of this seems to work is that it has variables for various VIA lines set to 1 or 0, and
// then calls a function any time the line changes. the functions vary depending on the model of Mac.
//
// A better way to go might be having changing the line be handled by one function, and call the appropriate
// function when the line changes.

// Common functions aliased
extern void MemOverlay_ChangeNtfy(void);

extern void VIA1_ShiftInData(uint8_t v);
extern uint8_t VIA1_ShiftOutData(void);
uint16_t VIA1_GetT1InvertTime(void);
extern void VIAorSCCinterruptChngNtfy(void);
// VIA1_iCA1_Sixtieth_PulseNtfy // Not defined?
// VIA1_iCA2_RTC_OneSecond_PulseNtfy // Not defined?

extern void RTCdataLine_ChangeNtfy(void);
extern void RTCclock_ChangeNtfy(void);
extern void RTCunEnabled_ChangeNtfy(void);

// Old mac only functions aliased
#if (CurEmMd == kEmMd_Plus)
extern bool SCC_InterruptsEnabled(void);

extern void Kybd_DataLineChngNtfy(void);
#endif

// New mac only functions aliased
#if (CurEmMd == kEmMd_II)
extern void ADBstate_ChangeNtfy(void);
extern void ADB_DataLineChngNtfy(void);
extern void Addr32_ChangeNtfy(void);
extern void PowerOff_ChangeNtfy(void);
// VIA2_iCA1_Vid_VBLinterrupt_PulseNtfy // Not defined?
// VIA2_iCB1_ASC_interrupt_PulseNtfy // Not defined?
#endif