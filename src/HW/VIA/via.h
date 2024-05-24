#pragma once

#include <stdint.h>
#include "global_glue.h" // kCycleScale and ClockMult
class VIA_Ty
{
    public:
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

    void Clear();
};

class VIA
{
    public:
        VIA_Ty Ty;
        uint8_t T1_Active = 0;
        uint8_t T2_Active = 0;
        bool T1IntReady = false;
    
    void Clear();
};

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

enum kTy 
{
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

constexpr uint8_t Ui3rPowOf2(uint8_t p)
{
	return (1 << p);
}
constexpr bool Ui3rTestBit(uint8_t i, uint8_t p)
{
	return ((i & Ui3rPowOf2(p)) != 0);
}

constexpr uint32_t CyclesPerViaTime = (10 * ClockMult);
constexpr uint32_t CyclesScaledPerViaTime = (kCycleScale * CyclesPerViaTime);