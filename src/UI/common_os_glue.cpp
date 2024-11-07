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

#include <stdlib.h>
#include "sys_dependencies.h"
#include "global_glue.h"
#include "my_os_glue.h"
#include "CNFGRAPI.h"
#include "common_os_glue.h"
#include "HW/SCREEN/screen.h"

bool RequestMacOff = false;
bool ForceMacOff = false;
bool WantMacInterrupt = false;
bool WantMacReset = false;

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
