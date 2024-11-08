/*
	dbglog.cpp

    Based on GLOBGLUE.c.

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

#include <stdlib.h>

#include "sys_dependencies.h"

#include "global_glue.h"
#include "UI/os_glue.h"
#include "UTIL/endian.h"

#include "HW/SCREEN/screen.h"
#include "dbglog.h"

extern void SetInterruptButton(bool v);

#ifndef ReportAbnormalInterrupt
#define ReportAbnormalInterrupt 0
#endif

#if WantDisasm
extern void m68k_WantDisasmContext(void);

void dbglog_StartLine(void)
{
	m68k_WantDisasmContext();
	dbglog_writeCStr(" ");
}
#endif

#if dbglog_HAVE
void dbglog_WriteMemArrow(bool WriteMem)
{
	if (WriteMem) {
		dbglog_writeCStr(" <- ");
	} else {
		dbglog_writeCStr(" -> ");
	}
}

void dbglog_AddrAccess(char *s, uint32_t Data,
	bool WriteMem, uint32_t addr)
{
	dbglog_StartLine();
	dbglog_writeCStr(s);
	dbglog_writeCStr("[");
	dbglog_writeHex(addr);
	dbglog_writeCStr("]");
	dbglog_WriteMemArrow(WriteMem);
	dbglog_writeHex(Data);
	dbglog_writeReturn();
}
void dbglog_Access(char *s, uint32_t Data, bool WriteMem)
{
	dbglog_StartLine();
	dbglog_writeCStr(s);
	dbglog_WriteMemArrow(WriteMem);
	dbglog_writeHex(Data);
	dbglog_writeReturn();
}

void dbglog_WriteSetBool(char *s, bool v)
{
	dbglog_StartLine();
	dbglog_writeCStr(s);
	dbglog_writeCStr(" <- ");
	if (v) {
		dbglog_writeCStr("1");
	} else {
		dbglog_writeCStr("0");
	}
	dbglog_writeReturn();
}
#endif

#if WantAbnormalReports
static bool GotOneAbnormal = false;
#endif

#if WantAbnormalReports
void DoReportAbnormalID(uint16_t id
#if dbglog_HAVE
	, char *s
#endif
	)
{
#if dbglog_HAVE
	dbglog_StartLine();
	dbglog_writeCStr("*** abnormal : ");
	dbglog_writeCStr(s);
	dbglog_writeReturn();
#endif

	if (! GotOneAbnormal) {
		WarnMsgAbnormalID(id);
#if ReportAbnormalInterrupt
		SetInterruptButton(true);
#endif
		GotOneAbnormal = true;
	}
}
#endif

/* --- sending debugging info to file --- */

#if dbglog_HAVE

static uimr dbglog_bufpos = 0;
static char *dbglog_bufp = nullptr;

static void dbglog_ReserveAlloc(void)
{
	ReserveAllocOneBlock((uint8_t **)&dbglog_bufp, dbglog_bufsz,
						 5, false);
}

static void dbglog_close(void)
{
	uimr n = ModPow2(dbglog_bufpos, dbglog_buflnsz);
	if (n != 0)
	{
		dbglog_write0(dbglog_bufp, n);
	}

	dbglog_close0();
}

static void dbglog_write(char *p, uimr L)
{
	uimr r;
	uimr bufposmod;
	uimr curbufdiv;
	uimr newbufpos = dbglog_bufpos + L;
	uimr newbufdiv = FloorDivPow2(newbufpos, dbglog_buflnsz);

label_retry:
	curbufdiv = FloorDivPow2(dbglog_bufpos, dbglog_buflnsz);
	bufposmod = ModPow2(dbglog_bufpos, dbglog_buflnsz);
	if (newbufdiv != curbufdiv)
	{
		r = dbglog_bufsz - bufposmod;
		MoveBytes((anyp)p, (anyp)(dbglog_bufp + bufposmod), r);
		dbglog_write0(dbglog_bufp, dbglog_bufsz);
		L -= r;
		p += r;
		dbglog_bufpos += r;
		goto label_retry;
	}
	MoveBytes((anyp)p, (anyp)dbglog_bufp + bufposmod, L);
	dbglog_bufpos = newbufpos;
}

static uimr CStrLength(char *s)
{
	char *p = s;

	while (*p++ != 0)
	{
	}
	return p - s - 1;
}

void dbglog_writeCStr(char *s)
{
	/* fprintf(DumpFile, "%s", s); */
	dbglog_write(s, CStrLength(s));
}

void dbglog_writeReturn(void)
{
	dbglog_writeCStr("\n");
	/* fprintf(DumpFile, "\n"); */
}

void dbglog_writeHex(uimr x)
{
	uint8_t v;
	char s[16];
	char *p = s + 16;
	uimr n = 0;

	do
	{
		v = x & 0x0F;
		if (v < 10)
		{
			*--p = '0' + v;
		}
		else
		{
			*--p = 'A' + v - 10;
		}
		x >>= 4;
		++n;
	} while (x != 0);

	dbglog_write(p, n);
	/* fprintf(DumpFile, "%d", (int)x); */
}

void dbglog_writeNum(uimr x)
{
	uimr newx;
	char s[16];
	char *p = s + 16;
	uimr n = 0;

	do
	{
		newx = x / (uimr)10;
		*--p = '0' + (x - newx * 10);
		x = newx;
		++n;
	} while (x != 0);

	dbglog_write(p, n);
	/* fprintf(DumpFile, "%d", (int)x); */
}

void dbglog_writeMacChar(uint8_t x)
{
	char s;

	if ((x > 32) && (x < 127))
	{
		s = x;
	}
	else
	{
		s = '?';
	}

	dbglog_write(&s, 1);
}

static void dbglog_writeSpace(void)
{
	dbglog_writeCStr(" ");
}

#endif
