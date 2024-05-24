#pragma once

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
} VIA_Ty;

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

constexpr u_int8_t Ui3rPowOf2(u_int8_t p)
{
	return (1 << p);
}
constexpr bool Ui3rTestBit(u_int8_t i, u_int8_t p)
{
	return ((i & Ui3rPowOf2(p)) != 0);
}
