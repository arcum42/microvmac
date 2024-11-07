/*
	HW/SCC/serial_comm.h

	Copyright (C) 2004 Philip Cummins, Paul C. Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with this file; see the file COPYING.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	license for more details.
*/

#pragma once

#include "sys_dependencies.h"

#ifndef SCCEMDEV_H
#define SCCEMDEV_H

#if EmLocalTalk

#define LT_TxBfMxSz 1024
extern uint8_t * LT_TxBuffer;
extern uint16_t LT_TxBuffSz;

extern void LT_TransmitPacket(void);

extern uint8_t * LT_RxBuffer;
extern uint32_t LT_RxBuffSz;

extern void LT_ReceivePacket(void);

#endif

extern void SCC_Reset(void);

extern uint32_t SCC_Access(uint32_t Data, bool WriteMem, CPTR addr);

extern bool SCC_InterruptsEnabled(void);

#if EmLocalTalk
extern void LocalTalkTick(void);
extern int InitLocalTalk(void);
#endif

#endif
