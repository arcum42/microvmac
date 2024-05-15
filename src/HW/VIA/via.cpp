/*
	VIAEMDEV.c

	Copyright (C) 2008 Philip Cummins, Paul C. Pratt

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
	Versatile Interface Adapter EMulated DEVice

	Emulates the Synertek SY6522 VIA found up until the Mac Plus.

	This code adapted from vMac by Philip Cummins.
*/

// Note: See Inside Macintosh III, page 39.

// VIA data register A is at vBase+vBufA. The corresponding data direction register is at
// vBase+vDirA.

// Bit(s) Name Description
// 7 vSCCWReq sec wait/request
// 6 vPage2 Alternate screen buffer
// 5 vHeadSel Disk SEL line
// 4 vOverlay ROM low-memory overlay
// 3 vSndPg2 Alternate sound buffer
// 0-2 vSound (mask) Sound volume

// The vSCCWReq bit can signal that the SCC has received a character (used to maintain serial
// communications during disk accesses, when the CPU's interrupts from the SCC are disabled).
// The vPage2 bit controls which screen buffer is being displayed, and the vHeadSel bit is the SEL
// control line used by the disk interface. The vOverlay bit (used only during system startup) can be
// used to place another image of ROM at the bottom of memory, where RAM usually is (RAM
// moves to $600000). The sound buffer is selected by the vSndPg2 bit. Finally, the vSound bits
// control the sound volume.

// VIA data register Bis at vBase+vBufB. The corresponding data direction register is at
// vBase+vDirB.

// Bit Name Description
// 7 vSndEnb Sound enable/disable
// 6 vH4 Horizontal blanking
// 5 vY2 MouseY2
// 4 vX2 MouseX2
// 3 vSW Mouse switch
// 2 trCEnb Real-time clock serial enable
// 1 trCClk Real-time clock data-clock line
// 0 trCData Real-time clock serial data

// The vSndEnb bit turns the sound generator on or off, and the vH4 bit is set when the video beam
// is in its horizontal blanking period. The vY2 and vX2 bits read the quadrature signals from the Y
// (vertical) and X (horizontal) directions, respectively, of the mouse's motion lines. The vSW bit
// reads the mouse switch. The trCEnb, trCClk , and trCData bits control and read the real-time
// clock.

// The VIA's peripheral control register, at vBase+vPCR, allows you to set some very low-level
// parameters (such as positive-edge or negative-edge triggering) dealing with the keyboard data and
// clock interrupts, the one-second real-time clock interrupt line, and the vertical blanking interrupt.

// Bit(s)
// 5-7  Keyboard data interrupt control
// 4    Keyboard clock interrupt control
// 1-3  One-second interrupt control
// 0    Vertical blanking interrupt control


// There's also information on the VIA's in "Guide to Macintosh Family Hardware 2nd Edition", Chapter 4, page 147.

#include "sys_dependencies.h"

#include "UI/my_os_glue.h"
#include "EMCONFIG.h"
#include "global_glue.h"

#include "via.h"

/*
	ReportAbnormalID unused 0x0410 - 0x04FF
*/

#ifdef VIA1_iA0_ChangeNtfy
extern void VIA1_iA0_ChangeNtfy(void);
#endif

#ifdef VIA1_iA1_ChangeNtfy
extern void VIA1_iA1_ChangeNtfy(void);
#endif

#ifdef VIA1_iA2_ChangeNtfy
extern void VIA1_iA2_ChangeNtfy(void);
#endif

#ifdef VIA1_iA3_ChangeNtfy
extern void VIA1_iA3_ChangeNtfy(void);
#endif

#ifdef VIA1_iA4_ChangeNtfy
extern void VIA1_iA4_ChangeNtfy(void);
#endif

#ifdef VIA1_iA5_ChangeNtfy
extern void VIA1_iA5_ChangeNtfy(void);
#endif

#ifdef VIA1_iA6_ChangeNtfy
extern void VIA1_iA6_ChangeNtfy(void);
#endif

#ifdef VIA1_iA7_ChangeNtfy
extern void VIA1_iA7_ChangeNtfy(void);
#endif

#ifdef VIA1_iB0_ChangeNtfy
extern void VIA1_iB0_ChangeNtfy(void);
#endif

#ifdef VIA1_iB1_ChangeNtfy
extern void VIA1_iB1_ChangeNtfy(void);
#endif

#ifdef VIA1_iB2_ChangeNtfy
extern void VIA1_iB2_ChangeNtfy(void);
#endif

#ifdef VIA1_iB3_ChangeNtfy
extern void VIA1_iB3_ChangeNtfy(void);
#endif

#ifdef VIA1_iB4_ChangeNtfy
extern void VIA1_iB4_ChangeNtfy(void);
#endif

#ifdef VIA1_iB5_ChangeNtfy
extern void VIA1_iB5_ChangeNtfy(void);
#endif

#ifdef VIA1_iB6_ChangeNtfy
extern void VIA1_iB6_ChangeNtfy(void);
#endif

#ifdef VIA1_iB7_ChangeNtfy
extern void VIA1_iB7_ChangeNtfy(void);
#endif

#ifdef VIA1_iCB2_ChangeNtfy
extern void VIA1_iCB2_ChangeNtfy(void);
#endif

constexpr u_int8_t Ui3rPowOf2(u_int8_t p)
{
	return (1 << p);
}
constexpr bool Ui3rTestBit(u_int8_t i, u_int8_t p)
{
	return ((i & Ui3rPowOf2(p)) != 0);
}

constexpr u_int8_t VIA1_ORA_CanInOrOut = (VIA1_ORA_CanIn | VIA1_ORA_CanOut);
constexpr u_int8_t VIA1_ORB_CanInOrOut = (VIA1_ORB_CanIn | VIA1_ORB_CanOut);

typedef struct
{
	uint32_t T1C_F; /* Timer 1 Counter Fixed Point */
	uint32_t T2C_F; /* Timer 2 Counter Fixed Point */
	uint8_t ORB;	/* Buffer B */
	/* uint8_t ORA_H;     Buffer A with Handshake */
	uint8_t DDR_B; /* Data Direction Register B */
	uint8_t DDR_A; /* Data Direction Register A */
	uint8_t T1L_L; /* Timer 1 Latch Low */
	uint8_t T1L_H; /* Timer 1 Latch High */
	uint8_t T2L_L; /* Timer 2 Latch Low */
	uint8_t SR;	   /* Shift Register */
	uint8_t ACR;   /* Auxiliary Control Register */
	uint8_t PCR;   /* Peripheral Control Register */
	uint8_t IFR;   /* Interrupt Flag Register */
	uint8_t IER;   /* Interrupt Enable Register */
	uint8_t ORA;   /* Buffer A */
} VIA1_Ty;

static VIA1_Ty VIA1_D;

enum kInt
{
	kIntCA2 = 0, /* One_Second */
	kIntCA1,	 /* Vertical_Blanking */
	kIntSR,		 /* Keyboard_Data_Ready */
	kIntCB2,	 /* Keyboard_Data */
	kIntCB1,	 /* Keyboard_Clock */
	kIntT2,		 /* Timer_2 */
	kIntT1		 /* Timer_1 */
};

#define VIA1_dolog (dbglog_HAVE && 0)

/* VIA1_Get_ORA : VIA Get Port A Data */
/*
	This function queries VIA Port A interfaced hardware
	about their status
*/

static uint8_t VIA1_Get_ORA(uint8_t Selection)
{
	uint8_t Value = (~VIA1_ORA_CanIn) & Selection & VIA1_ORA_FloatVal;

	if (Ui3rTestBit(VIA1_ORA_CanIn, 7) && Ui3rTestBit(Selection, 7))
	{
		Value |= (VIA1_iA7 << 7);
	}

	if (Ui3rTestBit(VIA1_ORA_CanIn, 6) && Ui3rTestBit(Selection, 6))
	{
		Value |= (VIA1_iA6 << 6);
	}

	if (Ui3rTestBit(VIA1_ORA_CanIn, 5) && Ui3rTestBit(Selection, 5))
	{
		Value |= (VIA1_iA5 << 5);
	}

	if (Ui3rTestBit(VIA1_ORA_CanIn, 4) && Ui3rTestBit(Selection, 4))
	{
		Value |= (VIA1_iA4 << 4);
	}

	if (Ui3rTestBit(VIA1_ORA_CanIn, 3) && Ui3rTestBit(Selection, 3))
	{
		Value |= (VIA1_iA3 << 3);
	}

	if (Ui3rTestBit(VIA1_ORA_CanIn, 2) && Ui3rTestBit(Selection, 2))
	{
		Value |= (VIA1_iA2 << 2);
	}

	if (Ui3rTestBit(VIA1_ORA_CanIn, 1) && Ui3rTestBit(Selection, 1))
	{
		Value |= (VIA1_iA1 << 1);
	}

	if (Ui3rTestBit(VIA1_ORA_CanIn, 0) && Ui3rTestBit(Selection, 0))
	{
		Value |= (VIA1_iA0 << 0);
	}

	return Value;
}

/* VIA1_Get_ORB : VIA Get Port B Data */
/*
	This function queries VIA Port B interfaced hardware
	about their status
*/

static uint8_t VIA1_Get_ORB(uint8_t Selection)
{
	uint8_t Value = (~VIA1_ORB_CanIn) & Selection & VIA1_ORB_FloatVal;

	if (Ui3rTestBit(VIA1_ORB_CanIn, 7) && Ui3rTestBit(Selection, 7))
	{
		Value |= (VIA1_iB7 << 7);
	}

	if (Ui3rTestBit(VIA1_ORB_CanIn, 6) && Ui3rTestBit(Selection, 6))
	{
		Value |= (VIA1_iB6 << 6);
	}

	if (Ui3rTestBit(VIA1_ORB_CanIn, 5) && Ui3rTestBit(Selection, 5))
	{
		Value |= (VIA1_iB5 << 5);
	}

	if (Ui3rTestBit(VIA1_ORB_CanIn, 4) && Ui3rTestBit(Selection, 4))
	{
		Value |= (VIA1_iB4 << 4);
	}

	if (Ui3rTestBit(VIA1_ORB_CanIn, 3) && Ui3rTestBit(Selection, 3))
	{
		Value |= (VIA1_iB3 << 3);
	}

	if (Ui3rTestBit(VIA1_ORB_CanIn, 2) && Ui3rTestBit(Selection, 2))
	{
		Value |= (VIA1_iB2 << 2);
	}

	if (Ui3rTestBit(VIA1_ORB_CanIn, 1) && Ui3rTestBit(Selection, 1))
	{
		Value |= (VIA1_iB1 << 1);
	}

	if (Ui3rTestBit(VIA1_ORB_CanIn, 0) && Ui3rTestBit(Selection, 0))
	{
		Value |= (VIA1_iB0 << 0);
	}

	return Value;
}

static void VIA1_Put_ORA(uint8_t Selection, uint8_t Data)
{
	uint8_t v = 0;

	if (Ui3rTestBit(VIA1_ORA_CanOut, 7) && (Ui3rTestBit(Selection, 7)))
	{
		v = (Data >> 7) & 1;
		if (v != VIA1_iA7)
		{
			VIA1_iA7 = v;
#ifdef VIA1_iA7_ChangeNtfy
			VIA1_iA7_ChangeNtfy();
#endif
		}
	}

	if (Ui3rTestBit(VIA1_ORA_CanOut, 6) && (Ui3rTestBit(Selection, 6)))
	{
		v = (Data >> 6) & 1;
		if (v != VIA1_iA6)
		{
			VIA1_iA6 = v;
#ifdef VIA1_iA6_ChangeNtfy
			VIA1_iA6_ChangeNtfy();
#endif
		}
	}

	if (Ui3rTestBit(VIA1_ORA_CanOut, 5) && (Ui3rTestBit(Selection, 5)))
	{
		v = (Data >> 5) & 1;
		if (v != VIA1_iA5)
		{
			VIA1_iA5 = v;
#ifdef VIA1_iA5_ChangeNtfy
			VIA1_iA5_ChangeNtfy();
#endif
		}
	}

	if (Ui3rTestBit(VIA1_ORA_CanOut, 4) && (Ui3rTestBit(Selection, 4)))
	{
		v = (Data >> 4) & 1;
		if (v != VIA1_iA4)
		{
			VIA1_iA4 = v;
#ifdef VIA1_iA4_ChangeNtfy
			VIA1_iA4_ChangeNtfy();
#endif
		}
	}

	if (Ui3rTestBit(VIA1_ORA_CanOut, 3) && (Ui3rTestBit(Selection, 3)))
	{
		v = (Data >> 3) & 1;
		if (v != VIA1_iA3)
		{
			VIA1_iA3 = v;
#ifdef VIA1_iA3_ChangeNtfy
			VIA1_iA3_ChangeNtfy();
#endif
		}
	}

	if (Ui3rTestBit(VIA1_ORA_CanOut, 2) && (Ui3rTestBit(Selection, 2)))
	{
		v = (Data >> 2) & 1;
		if (v != VIA1_iA2)
		{
			VIA1_iA2 = v;
#ifdef VIA1_iA2_ChangeNtfy
			VIA1_iA2_ChangeNtfy();
#endif
		}
	}

	if (Ui3rTestBit(VIA1_ORA_CanOut, 1) && (Ui3rTestBit(Selection, 1)))
	{
		v = (Data >> 1) & 1;
		if (v != VIA1_iA1)
		{
			VIA1_iA1 = v;
#ifdef VIA1_iA1_ChangeNtfy
			VIA1_iA1_ChangeNtfy();
#endif
		}
	}

	if (Ui3rTestBit(VIA1_ORA_CanOut, 0) && (Ui3rTestBit(Selection, 0)))
	{
		v = (Data >> 0) & 1;
		if (v != VIA1_iA0)
		{
			VIA1_iA0 = v;
#ifdef VIA1_iA0_ChangeNtfy
			VIA1_iA0_ChangeNtfy();
#endif
		}
	}
}

static void VIA1_Put_ORB(uint8_t Selection, uint8_t Data)
{
	uint8_t v = 0;

	if (Ui3rTestBit(VIA1_ORB_CanOut, 7) && (Ui3rTestBit(Selection, 7)))
	{
		v = (Data >> 7) & 1;
		if (v != VIA1_iB7)
		{
			VIA1_iB7 = v;
#ifdef VIA1_iB7_ChangeNtfy
			VIA1_iB7_ChangeNtfy();
#endif
		}
	}

	if (Ui3rTestBit(VIA1_ORB_CanOut, 6) && (Ui3rTestBit(Selection, 6)))
	{
		v = (Data >> 6) & 1;
		if (v != VIA1_iB6)
		{
			VIA1_iB6 = v;
#ifdef VIA1_iB6_ChangeNtfy
			VIA1_iB6_ChangeNtfy();
#endif
		}
	}

	if (Ui3rTestBit(VIA1_ORB_CanOut, 5) && (Ui3rTestBit(Selection, 5)))
	{
		v = (Data >> 5) & 1;
		{
			if (v != VIA1_iB5)
				VIA1_iB5 = v;
#ifdef VIA1_iB5_ChangeNtfy
			VIA1_iB5_ChangeNtfy();
#endif
		}
	}

	if (Ui3rTestBit(VIA1_ORB_CanOut, 4) && (Ui3rTestBit(Selection, 4)))
	{
		v = (Data >> 4) & 1;
		if (v != VIA1_iB4)
		{
			VIA1_iB4 = v;
#ifdef VIA1_iB4_ChangeNtfy
			VIA1_iB4_ChangeNtfy();
#endif
		}
	}

	if (Ui3rTestBit(VIA1_ORB_CanOut, 3) && (Ui3rTestBit(Selection, 3)))
	{
		v = (Data >> 3) & 1;
		if (v != VIA1_iB3)
		{
			VIA1_iB3 = v;
#ifdef VIA1_iB3_ChangeNtfy
			VIA1_iB3_ChangeNtfy();
#endif
		}
	}

	if (Ui3rTestBit(VIA1_ORB_CanOut, 2) && (Ui3rTestBit(Selection, 2)))
	{
		v = (Data >> 2) & 1;
		{
			if (v != VIA1_iB2)
				VIA1_iB2 = v;
#ifdef VIA1_iB2_ChangeNtfy
			VIA1_iB2_ChangeNtfy();
#endif
		}
	}

	if (Ui3rTestBit(VIA1_ORB_CanOut, 1) && (Ui3rTestBit(Selection, 1)))
	{
		v = (Data >> 1) & 1;
		{
			if (v != VIA1_iB1)
				VIA1_iB1 = v;
#ifdef VIA1_iB1_ChangeNtfy
			VIA1_iB1_ChangeNtfy();
#endif
		}
	}

	if (Ui3rTestBit(VIA1_ORB_CanOut, 0) && (Ui3rTestBit(Selection, 0)))
	{
		v = (Data >> 0) & 1;
		{
			if (v != VIA1_iB0)
				VIA1_iB0 = v;
#ifdef VIA1_iB0_ChangeNtfy
			VIA1_iB0_ChangeNtfy();
#endif
		}
	}
}

static void VIA1_SetDDR_A(uint8_t Data)
{
	uint8_t floatbits = VIA1_D.DDR_A & ~Data;
	uint8_t unfloatbits = Data & ~VIA1_D.DDR_A;

	if (floatbits != 0)
	{
		VIA1_Put_ORA(floatbits, VIA1_ORA_FloatVal);
	}
	VIA1_D.DDR_A = Data;
	if (unfloatbits != 0)
	{
		VIA1_Put_ORA(unfloatbits, VIA1_D.ORA);
	}
	if ((Data & ~VIA1_ORA_CanOut) != 0)
	{
		ReportAbnormalID(0x0401,
						 "Set VIA1_D.DDR_A unexpected direction");
	}
}

static void VIA1_SetDDR_B(uint8_t Data)
{
	uint8_t floatbits = VIA1_D.DDR_B & ~Data;
	uint8_t unfloatbits = Data & ~VIA1_D.DDR_B;

	if (floatbits != 0)
	{
		VIA1_Put_ORB(floatbits, VIA1_ORB_FloatVal);
	}
	VIA1_D.DDR_B = Data;
	if (unfloatbits != 0)
	{
		VIA1_Put_ORB(unfloatbits, VIA1_D.ORB);
	}
	if ((Data & ~VIA1_ORB_CanOut) != 0)
	{
		ReportAbnormalID(0x0402,
						 "Set VIA1_D.DDR_B unexpected direction");
	}
}

static void VIA1_CheckInterruptFlag(void)
{
	uint8_t NewInterruptRequest =
		((VIA1_D.IFR & VIA1_D.IER) != 0) ? 1 : 0;

	if (NewInterruptRequest != VIA1_InterruptRequest)
	{
		VIA1_InterruptRequest = NewInterruptRequest;
#ifdef VIA1_interruptChngNtfy
		VIA1_interruptChngNtfy();
#endif
	}
}

static uint8_t VIA1_T1_Active = 0;
static uint8_t VIA1_T2_Active = 0;

static bool VIA1_T1IntReady = false;

static void VIA1_Clear(void)
{
	VIA1_D.ORA = 0;
	VIA1_D.DDR_A = 0;
	VIA1_D.ORB = 0;
	VIA1_D.DDR_B = 0;
	VIA1_D.T1L_L = VIA1_D.T1L_H = 0x00;
	VIA1_D.T2L_L = 0x00;
	VIA1_D.T1C_F = 0;
	VIA1_D.T2C_F = 0;
	VIA1_D.SR = VIA1_D.ACR = 0x00;
	VIA1_D.PCR = VIA1_D.IFR = VIA1_D.IER = 0x00;
	VIA1_T1_Active = VIA1_T2_Active = 0x00;
	VIA1_T1IntReady = false;
}

bool VIA1_Zap(void)
{
	VIA1_Clear();
	VIA1_InterruptRequest = 0;
	return true;
}

void VIA1_Reset(void)
{
	VIA1_SetDDR_A(0);
	VIA1_SetDDR_B(0);

	VIA1_Clear();

	VIA1_CheckInterruptFlag();
}

static void VIA1_SetInterruptFlag(uint8_t VIA_Int)
{
	VIA1_D.IFR |= ((uint8_t)1 << VIA_Int);
	VIA1_CheckInterruptFlag();
}

static void VIA1_ClrInterruptFlag(uint8_t VIA_Int)
{
	VIA1_D.IFR &= ~((uint8_t)1 << VIA_Int);
	VIA1_CheckInterruptFlag();
}

#ifdef _VIA_Debug
#include <stdio.h>
#endif

void VIA1_ShiftInData(uint8_t v)
{
	/*
		external hardware generates 8 pulses on CB1,
		writes 8 bits to CB2
	*/
	uint8_t ShiftMode = (VIA1_D.ACR & 0x1C) >> 2;

	if (ShiftMode != 3)
	{
#if ExtraAbnormalReports
		if (ShiftMode == 0)
		{
			/* happens on reset */
		}
		else
		{
			ReportAbnormalID(0x0403, "VIA Not ready to shift in");
			/*
				Observed (rarely) in Crystal Quest played
				at 1x speed in "-t mc64".
			*/
		}
#endif
	}
	else
	{
		VIA1_D.SR = v;
		VIA1_SetInterruptFlag(kIntSR);
		VIA1_SetInterruptFlag(kIntCB1);
	}
}

uint8_t VIA1_ShiftOutData(void)
{
	/*
		external hardware generates 8 pulses on CB1,
		reads 8 bits from CB2
	*/
	if (((VIA1_D.ACR & 0x1C) >> 2) != 7)
	{
		ReportAbnormalID(0x0404, "VIA Not ready to shift out");
		return 0;
	}
	else
	{
		VIA1_SetInterruptFlag(kIntSR);
		VIA1_SetInterruptFlag(kIntCB1);
		VIA1_iCB2 = (VIA1_D.SR & 1);
		return VIA1_D.SR;
	}
}

constexpr uint32_t CyclesPerViaTime = (10 * ClockMult);
constexpr uint32_t CyclesScaledPerViaTime = (kCycleScale * CyclesPerViaTime);

static bool VIA1_T1Running = true;
static iCountt VIA1_T1LastTime = 0;

void VIA1_DoTimer1Check(void)
{
	if (VIA1_T1Running)
	{
		iCountt NewTime = GetCuriCount();
		iCountt deltaTime = (NewTime - VIA1_T1LastTime);
		if (deltaTime != 0)
		{
			uint32_t Temp = VIA1_D.T1C_F; /* Get Timer 1 Counter */
			uint32_t deltaTemp =
				(deltaTime / CyclesPerViaTime) << (16 - kLn2CycleScale);
			/* may overflow */
			uint32_t NewTemp = Temp - deltaTemp;
			if ((deltaTime > (0x00010000UL * CyclesScaledPerViaTime)) || ((Temp <= deltaTemp) && (Temp != 0)))
			{
				if ((VIA1_D.ACR & 0x40) != 0)
				{ /* Free Running? */
					/* Reload Counter from Latches */
					uint16_t v = (VIA1_D.T1L_H << 8) + VIA1_D.T1L_L;
					uint16_t ntrans = 1 + ((v == 0) ? 0 : (((deltaTemp - Temp) / v) >> 16));
					NewTemp += (((uint32_t)v * ntrans) << 16);

					if (Ui3rTestBit(VIA1_ORB_CanOut, 7))
					{
						if ((VIA1_D.ACR & 0x80) != 0)
						{ /* invert ? */
							if ((ntrans & 1) != 0)
							{
								VIA1_iB7 ^= 1;
#ifdef VIA1_iB7_ChangeNtfy
								VIA1_iB7_ChangeNtfy();
#endif
							}
						}
					}
					VIA1_SetInterruptFlag(kIntT1);
#if VIA1_dolog && 1
					spdlog::debug("VIA1 Timer 1 Interrupt");
#endif
				}
				else
				{
					if (VIA1_T1_Active == 1)
					{
						VIA1_T1_Active = 0;
						VIA1_SetInterruptFlag(kIntT1);
#if VIA1_dolog && 1
						spdlog::debug("VIA1 Timer 1 Interrupt");
#endif
					}
				}
			}

			VIA1_D.T1C_F = NewTemp;
			VIA1_T1LastTime = NewTime;
		}

		VIA1_T1IntReady = false;
		if ((VIA1_D.IFR & (1 << kIntT1)) == 0)
		{
			if (((VIA1_D.ACR & 0x40) != 0) || (VIA1_T1_Active == 1))
			{
				uint32_t NewTemp = VIA1_D.T1C_F; /* Get Timer 1 Counter */
				uint32_t NewTimer;
#ifdef _VIA_Debug
				fprintf(stderr, "posting Timer1Check, %d, %d\n",
						Temp, GetCuriCount());
#endif
				if (NewTemp == 0)
				{
					NewTimer = (0x00010000UL * CyclesScaledPerViaTime);
				}
				else
				{
					NewTimer =
						(1 + (NewTemp >> (16 - kLn2CycleScale))) * CyclesPerViaTime;
				}
				ICT_add(kICT_VIA1_Timer1Check, NewTimer);
				VIA1_T1IntReady = true;
			}
		}
	}
}

static void CheckT1IntReady(void)
{
	if (VIA1_T1Running)
	{
		bool NewT1IntReady = false;

		if ((VIA1_D.IFR & (1 << kIntT1)) == 0)
		{
			if (((VIA1_D.ACR & 0x40) != 0) || (VIA1_T1_Active == 1))
			{
				NewT1IntReady = true;
			}
		}

		if (VIA1_T1IntReady != NewT1IntReady)
		{
			VIA1_T1IntReady = NewT1IntReady;
			if (NewT1IntReady)
			{
				VIA1_DoTimer1Check();
			}
		}
	}
}

uint16_t VIA1_GetT1InvertTime(void)
{
	uint16_t v;

	if ((VIA1_D.ACR & 0xC0) == 0xC0)
	{
		v = (VIA1_D.T1L_H << 8) + VIA1_D.T1L_L;
	}
	else
	{
		v = 0;
	}
	return v;
}

static bool VIA1_T2Running = true;
static bool VIA1_T2C_ShortTime = false;
static iCountt VIA1_T2LastTime = 0;

void VIA1_DoTimer2Check(void)
{
	if (VIA1_T2Running)
	{
		iCountt NewTime = GetCuriCount();
		uint32_t Temp = VIA1_D.T2C_F; /* Get Timer 2 Counter */
		iCountt deltaTime = (NewTime - VIA1_T2LastTime);
		uint32_t deltaTemp = (deltaTime / CyclesPerViaTime)
							 << (16 - kLn2CycleScale); /* may overflow */
		uint32_t NewTemp = Temp - deltaTemp;
		if (VIA1_T2_Active == 1)
		{
			if ((deltaTime > (0x00010000UL * CyclesScaledPerViaTime)) || ((Temp <= deltaTemp) && (Temp != 0)))
			{
				VIA1_T2C_ShortTime = false;
				VIA1_T2_Active = 0;
				VIA1_SetInterruptFlag(kIntT2);
#if VIA1_dolog && 1
				spdlog::debug("VIA1 Timer 2 Interrupt");
#endif
			}
			else
			{
				uint32_t NewTimer;
#ifdef _VIA_Debug
				fprintf(stderr, "posting Timer2Check, %d, %d\n",
						Temp, GetCuriCount());
#endif
				if (NewTemp == 0)
				{
					NewTimer = (0x00010000UL * CyclesScaledPerViaTime);
				}
				else
				{
					NewTimer = (1 + (NewTemp >> (16 - kLn2CycleScale))) * CyclesPerViaTime;
				}
				ICT_add(kICT_VIA1_Timer2Check, NewTimer);
			}
		}
		VIA1_D.T2C_F = NewTemp;
		VIA1_T2LastTime = NewTime;
	}
}

enum {
kORB = 0x00,
kORA_H = 0x01,
kDDR_B = 0x02,
kDDR_A = 0x03,
kT1C_L = 0x04,
kT1C_H = 0x05,
kT1L_L = 0x06,
kT1L_H = 0x07,
kT2_L = 0x08,
kT2_H = 0x09,
kSR = 0x0A,
kACR = 0x0B,
kPCR = 0x0C,
kIFR = 0x0D,
kIER = 0x0E,
kORA = 0x0F
};

uint32_t VIA1_Access(uint32_t Data, bool WriteMem, CPTR addr)
{
	switch (addr)
	{
	case kORB:
#if VIA1_CB2modesAllowed != 0x01
		if ((VIA1_D.PCR & 0xE0) == 0)
#endif
		{
			VIA1_ClrInterruptFlag(kIntCB2);
		}
		VIA1_ClrInterruptFlag(kIntCB1);
		if (WriteMem)
		{
			VIA1_D.ORB = Data;
			VIA1_Put_ORB(VIA1_D.DDR_B, VIA1_D.ORB);
		}
		else
		{
			Data = (VIA1_D.ORB & VIA1_D.DDR_B) | VIA1_Get_ORB(~VIA1_D.DDR_B);
		}
#if VIA1_dolog && 1
		dbglog_Access("VIA1_Access kORB", Data, WriteMem);
#endif
		break;
	case kDDR_B:
		if (WriteMem)
		{
			VIA1_SetDDR_B(Data);
		}
		else
		{
			Data = VIA1_D.DDR_B;
		}
#if VIA1_dolog && 1
		dbglog_Access("VIA1_Access kDDR_B", Data, WriteMem);
#endif
		break;
	case kDDR_A:
		if (WriteMem)
		{
			VIA1_SetDDR_A(Data);
		}
		else
		{
			Data = VIA1_D.DDR_A;
		}
#if VIA1_dolog && 1
		dbglog_Access("VIA1_Access kDDR_A", Data, WriteMem);
#endif
		break;
	case kT1C_L:
		if (WriteMem)
		{
			VIA1_D.T1L_L = Data;
		}
		else
		{
			VIA1_ClrInterruptFlag(kIntT1);
			VIA1_DoTimer1Check();
			Data = (VIA1_D.T1C_F & 0x00FF0000) >> 16;
		}
#if VIA1_dolog && 1
		dbglog_Access("VIA1_Access kT1C_L", Data, WriteMem);
#endif
		break;
	case kT1C_H:
		if (WriteMem)
		{
			VIA1_D.T1L_H = Data;
			VIA1_ClrInterruptFlag(kIntT1);
			VIA1_D.T1C_F = (Data << 24) + (VIA1_D.T1L_L << 16);
			if ((VIA1_D.ACR & 0x40) == 0)
			{
				VIA1_T1_Active = 1;
			}
			VIA1_T1LastTime = GetCuriCount();
			VIA1_DoTimer1Check();
		}
		else
		{
			VIA1_DoTimer1Check();
			Data = (VIA1_D.T1C_F & 0xFF000000) >> 24;
		}
#if VIA1_dolog && 1
		dbglog_Access("VIA1_Access kT1C_H", Data, WriteMem);
#endif
		break;
	case kT1L_L:
		if (WriteMem)
		{
			VIA1_D.T1L_L = Data;
		}
		else
		{
			Data = VIA1_D.T1L_L;
		}
#if VIA1_dolog && 1
		dbglog_Access("VIA1_Access kT1L_L", Data, WriteMem);
#endif
		break;
	case kT1L_H:
		if (WriteMem)
		{
			VIA1_D.T1L_H = Data;
		}
		else
		{
			Data = VIA1_D.T1L_H;
		}
#if VIA1_dolog && 1
		dbglog_Access("VIA1_Access kT1L_H", Data, WriteMem);
#endif
		break;
	case kT2_L:
		if (WriteMem)
		{
			VIA1_D.T2L_L = Data;
		}
		else
		{
			VIA1_ClrInterruptFlag(kIntT2);
			VIA1_DoTimer2Check();
			Data = (VIA1_D.T2C_F & 0x00FF0000) >> 16;
		}
#if VIA1_dolog && 1
		dbglog_Access("VIA1_Access kT2_L", Data, WriteMem);
#endif
		break;
	case kT2_H:
		if (WriteMem)
		{
			VIA1_D.T2C_F = (Data << 24) + (VIA1_D.T2L_L << 16);
			VIA1_ClrInterruptFlag(kIntT2);
			VIA1_T2_Active = 1;

			if ((VIA1_D.T2C_F < (128UL << 16)) && (VIA1_D.T2C_F != 0))
			{
				VIA1_T2C_ShortTime = true;
				VIA1_T2Running = true;
				/*
					Running too many instructions during
					a short timer interval can crash when
					playing sounds in System 7. So
					in this case don't let timer pause.
				*/
			}
			VIA1_T2LastTime = GetCuriCount();
			VIA1_DoTimer2Check();
		}
		else
		{
			VIA1_DoTimer2Check();
			Data = (VIA1_D.T2C_F & 0xFF000000) >> 24;
		}
#if VIA1_dolog && 1
		dbglog_Access("VIA1_Access kT2_H", Data, WriteMem);
#endif
		break;
	case kSR:
#ifdef _VIA_Debug
		fprintf(stderr, "VIA1_D.SR: %d, %d, %d\n",
				WriteMem, ((VIA1_D.ACR & 0x1C) >> 2), Data);
#endif
		if (WriteMem)
		{
			VIA1_D.SR = Data;
		}
		VIA1_ClrInterruptFlag(kIntSR);
		switch ((VIA1_D.ACR & 0x1C) >> 2)
		{
		case 3: /* Shifting In */
			break;
		case 6: /* shift out under o2 clock */
			if ((!WriteMem) || (VIA1_D.SR != 0))
			{
				ReportAbnormalID(0x0405,
								 "VIA shift mode 6, non zero");
			}
			else
			{
#ifdef _VIA_Debug
				fprintf(stderr, "posting Foo2Task\n");
#endif
				if (VIA1_iCB2 != 0)
				{
					VIA1_iCB2 = 0;
#ifdef VIA1_iCB2_ChangeNtfy
					VIA1_iCB2_ChangeNtfy();
#endif
				}
			}
#if 0 /* possibly should do this. seems not to affect anything. */
					VIA1_SetInterruptFlag(kIntSR); /* don't wait */
#endif
			break;
		case 7: /* Shifting Out */
			break;
		}
		if (!WriteMem)
		{
			Data = VIA1_D.SR;
		}
#if VIA1_dolog && 1
		dbglog_Access("VIA1_Access kSR", Data, WriteMem);
#endif
		break;
	case kACR:
		if (WriteMem)
		{
			if ((VIA1_D.ACR & 0x10) != ((uint8_t)Data & 0x10))
			{
				/* shift direction has changed */
				if ((Data & 0x10) == 0)
				{
					/*
						no longer an output,
						set data to float value
					*/
					if (VIA1_iCB2 == 0)
					{
						VIA1_iCB2 = 1;
#ifdef VIA1_iCB2_ChangeNtfy
						VIA1_iCB2_ChangeNtfy();
#endif
					}
				}
			}
			VIA1_D.ACR = Data;
			if ((VIA1_D.ACR & 0x20) != 0)
			{
				/* Not pulse counting? */
				ReportAbnormalID(0x0406,
								 "Set VIA1_D.ACR T2 Timer pulse counting");
			}
			switch ((VIA1_D.ACR & 0xC0) >> 6)
			{
			/* case 1: happens in early System 6 */
			case 2:
				ReportAbnormalID(0x0407,
								 "Set VIA1_D.ACR T1 Timer mode 2");
				break;
			}
			CheckT1IntReady();
			switch ((VIA1_D.ACR & 0x1C) >> 2)
			{
			case 0: /* this isn't sufficient */
				VIA1_ClrInterruptFlag(kIntSR);
				break;
			case 1:
			case 2:
			case 4:
			case 5:
				ReportAbnormalID(0x0408,
								 "Set VIA1_D.ACR shift mode 1,2,4,5");
				break;
			default:
				break;
			}
			if ((VIA1_D.ACR & 0x03) != 0)
			{
				ReportAbnormalID(0x0409,
								 "Set VIA1_D.ACR T2 Timer latching enabled");
			}
		}
		else
		{
			Data = VIA1_D.ACR;
		}
#if VIA1_dolog && 1
		dbglog_Access("VIA1_Access kACR", Data, WriteMem);
#endif
		break;
	case kPCR:
		if (WriteMem)
		{
			VIA1_D.PCR = Data;
#define Ui3rSetContains(s, i) (((s) & (1 << (i))) != 0)
			if (!Ui3rSetContains(VIA1_CB2modesAllowed,
								 (VIA1_D.PCR >> 5) & 0x07))
			{
				ReportAbnormalID(0x040A,
								 "Set VIA1_D.PCR CB2 Control mode?");
			}
			if ((VIA1_D.PCR & 0x10) != 0)
			{
				ReportAbnormalID(0x040B,
								 "Set VIA1_D.PCR CB1 INTERRUPT CONTROL?");
			}
			if (!Ui3rSetContains(VIA1_CA2modesAllowed,
								 (VIA1_D.PCR >> 1) & 0x07))
			{
				ReportAbnormalID(0x040C,
								 "Set VIA1_D.PCR CA2 INTERRUPT CONTROL?");
			}
			if ((VIA1_D.PCR & 0x01) != 0)
			{
				ReportAbnormalID(0x040D,
								 "Set VIA1_D.PCR CA1 INTERRUPT CONTROL?");
			}
		}
		else
		{
			Data = VIA1_D.PCR;
		}
#if VIA1_dolog && 1
		dbglog_Access("VIA1_Access kPCR", Data, WriteMem);
#endif
		break;
	case kIFR:
		if (WriteMem)
		{
			VIA1_D.IFR = VIA1_D.IFR & ((~Data) & 0x7F);
			/* Clear Flag Bits */
			VIA1_CheckInterruptFlag();
			CheckT1IntReady();
		}
		else
		{
			Data = VIA1_D.IFR;
			if ((VIA1_D.IFR & VIA1_D.IER) != 0)
			{
				Data |= 0x80;
			}
		}
#if VIA1_dolog && 1
		dbglog_Access("VIA1_Access kIFR", Data, WriteMem);
#endif
		break;
	case kIER:
		if (WriteMem)
		{
			if ((Data & 0x80) == 0)
			{
				VIA1_D.IER = VIA1_D.IER & ((~Data) & 0x7F);
				/* Clear Enable Bits */
#if 0 != VIA1_IER_Never0
					/*
						of course, will be 0 initially,
						this just checks not cleared later.
					*/
					if ((Data & VIA1_IER_Never0) != 0) {
						ReportAbnormalID(0x040E, "IER Never0 clr");
					}
#endif
			}
			else
			{
				VIA1_D.IER = VIA1_D.IER | (Data & 0x7F);
				/* Set Enable Bits */
#if 0 != VIA1_IER_Never1
					if ((VIA1_D.IER & VIA1_IER_Never1) != 0) {
						ReportAbnormalID(0x040F, "IER Never1 set");
					}
#endif
			}
			VIA1_CheckInterruptFlag();
		}
		else
		{
			Data = VIA1_D.IER | 0x80;
		}
#if VIA1_dolog && 1
		dbglog_Access("VIA1_Access kIER", Data, WriteMem);
#endif
		break;
	case kORA:
	case kORA_H:
		if ((VIA1_D.PCR & 0xE) == 0)
		{
			VIA1_ClrInterruptFlag(kIntCA2);
		}
		VIA1_ClrInterruptFlag(kIntCA1);
		if (WriteMem)
		{
			VIA1_D.ORA = Data;
			VIA1_Put_ORA(VIA1_D.DDR_A, VIA1_D.ORA);
		}
		else
		{
			Data = (VIA1_D.ORA & VIA1_D.DDR_A) | VIA1_Get_ORA(~VIA1_D.DDR_A);
		}
#if VIA1_dolog && 1
		dbglog_Access("VIA1_Access kORA", Data, WriteMem);
#endif
		break;
	}
	return Data;
}

void VIA1_ExtraTimeBegin(void)
{
	if (VIA1_T1Running)
	{
		VIA1_DoTimer1Check(); /* run up to this moment */
		VIA1_T1Running = false;
	}
	if (VIA1_T2Running & (!VIA1_T2C_ShortTime))
	{
		VIA1_DoTimer2Check(); /* run up to this moment */
		VIA1_T2Running = false;
	}
}

void VIA1_ExtraTimeEnd(void)
{
	if (!VIA1_T1Running)
	{
		VIA1_T1Running = true;
		VIA1_T1LastTime = GetCuriCount();
		VIA1_DoTimer1Check();
	}
	if (!VIA1_T2Running)
	{
		VIA1_T2Running = true;
		VIA1_T2LastTime = GetCuriCount();
		VIA1_DoTimer2Check();
	}
}

/* VIA Interrupt Interface */

#ifdef VIA1_iCA1_PulseNtfy
void VIA1_iCA1_PulseNtfy(void)
{
	VIA1_SetInterruptFlag(kIntCA1);
}
#endif

#ifdef VIA1_iCA2_PulseNtfy
void VIA1_iCA2_PulseNtfy(void)
{
	VIA1_SetInterruptFlag(kIntCA2);
}
#endif

#ifdef VIA1_iCB1_PulseNtfy
void VIA1_iCB1_PulseNtfy(void)
{
	VIA1_SetInterruptFlag(kIntCB1);
}
#endif

#ifdef VIA1_iCB2_PulseNtfy
void VIA1_iCB2_PulseNtfy(void)
{
	VIA1_SetInterruptFlag(kIntCB2);
}
#endif
