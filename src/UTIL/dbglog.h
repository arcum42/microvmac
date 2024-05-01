/*
	Based on GLOBGLUE.h

	Copyright (C) 2003 Bernd Schmidt, Philip Cummins, Paul C. Pratt

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


#if WantDisasm
extern void dbglog_StartLine(void);
#else
#define dbglog_StartLine()
#endif

#if dbglog_HAVE
extern void dbglog_WriteMemArrow(bool WriteMem);

extern void dbglog_WriteNote(char *s);
extern void dbglog_WriteSetBool(char *s, bool v);
extern void dbglog_AddrAccess(char *s,
	uint32_t Data, bool WriteMem, uint32_t addr);
extern void dbglog_Access(char *s, uint32_t Data, bool WriteMem);
#endif

#if ! WantAbnormalReports
#define ReportAbnormalID(id, s)
#else
#if dbglog_HAVE
#define ReportAbnormalID DoReportAbnormalID
#else
#define ReportAbnormalID(id, s) DoReportAbnormalID(id)
#endif
extern void DoReportAbnormalID(uint16_t id
#if dbglog_HAVE
	, char *s
#endif
	);
#endif /* WantAbnormalReports */
