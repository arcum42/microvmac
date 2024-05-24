
#include <stdint.h>
#include "via.h"

void VIA_Ty::Clear()
{
    ORA = 0;
    DDR_A = 0;
    ORB = 0;
    DDR_B = 0;
    T1L_L = T1L_H = 0x00;
    T2L_L = 0x00;
    T1C_F = 0;
    T2C_F = 0;
    SR = ACR = 0x00;
    PCR = IFR = IER = 0x00;
}

void VIA::Clear()
{
    Ty.Clear();
	T1_Active = T2_Active = 0x00;
	T1IntReady = false;
}