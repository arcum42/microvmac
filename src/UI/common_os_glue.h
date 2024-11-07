/*
	common_os_glue.h

	Copyright (C) 2009 Paul C. Pratt

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
	COMmon code for Operating System GLUe
*/

#pragma once

#ifndef COMOSGLU_H
#define COMOSGLU_H

#include "sys_dependencies.h"
#include "CNFGRAPI.h"
#include "global_glue.h"
#include "my_os_glue.h"
#include "event_queue.h"
#include "memory.h"
#include "UI/SDL2/video_sdl2.h"
#include "math.h"

extern uint32_t CurMacDateInSeconds;
#if AutoLocation
extern uint32_t CurMacLatitude;
extern uint32_t CurMacLongitude;
#endif
#if AutoTimeZone
extern uint32_t CurMacDelta;
#endif

extern bool RequestMacOff;
extern bool ForceMacOff;
extern bool WantMacInterrupt;
extern bool WantMacReset;

/* --- sending debugging info to file --- */

#if dbglog_HAVE
#define dbglog_bufsz PowOf2(dbglog_buflnsz)
#define dbglog_open dbglog_open0

void dbglog_writeCStr(char *s);
void dbglog_writeReturn(void);
void dbglog_writeHex(uimr x);
void dbglog_writeNum(uimr x);
void dbglog_writeMacChar(uint8_t x);
#endif

#if WantAbnormalReports
extern void WarnMsgAbnormalID(uint16_t id);
#endif

#endif
