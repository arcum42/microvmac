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

 uint32_t vSonyWritableMask = 0;
 uint32_t vSonyInsertedMask = 0;

#if IncludeSonyRawMode
 bool vSonyRawMode = false;
#endif

#if IncludeSonyNew
 bool vSonyNewDiskWanted = false;
 uint32_t vSonyNewDiskSize;
#endif

#if IncludeSonyNameNew
 tPbuf vSonyNewDiskName = NotAPbuf;
#endif

 uint32_t CurMacDateInSeconds = 0;
#if AutoLocation
 uint32_t CurMacLatitude = 0;
 uint32_t CurMacLongitude = 0;
#endif
#if AutoTimeZone
 uint32_t CurMacDelta = 0;
#endif

 bool UseColorMode = false;
 bool ColorModeWorks = false;

 bool ColorMappingChanged = false;

 uint16_t CLUT_reds[CLUT_size];
 uint16_t CLUT_greens[CLUT_size];
 uint16_t CLUT_blues[CLUT_size];

 bool RequestMacOff = false;
 bool ForceMacOff = false;
 bool WantMacInterrupt = false;
 bool WantMacReset = false;

 uint8_t SpeedValue = WantInitSpeedValue;
 uint16_t CurMouseV = 0;
 uint16_t CurMouseH = 0;

#if IncludePbufs
 uint32_t PbufAllocatedMask;
 uint32_t PbufSize[NumPbufs];
 bool FirstFreePbuf(tPbuf *r)
{
	tPbuf i;

	for (i = 0; i < NumPbufs; ++i) {
		if (! PbufIsAllocated(i)) {
			*r = i;
			return true;
		}
	}
	return false;
}

void PbufNewNotify(tPbuf Pbuf_No, uint32_t count)
{
	PbufSize[Pbuf_No] = count;
	PbufAllocatedMask |= ((uint32_t)1 << Pbuf_No);
}

void PbufDisposeNotify(tPbuf Pbuf_No)
{
	PbufAllocatedMask &= ~ ((uint32_t)1 << Pbuf_No);
}

MacErr_t CheckPbuf(tPbuf Pbuf_No)
{
	MacErr_t result;

	if (Pbuf_No >= NumPbufs) {
		result = mnvm_nsDrvErr;
	} else if (! PbufIsAllocated(Pbuf_No)) {
		result = mnvm_offLinErr;
	} else {
		result = mnvm_noErr;
	}

	return result;
}

MacErr_t PbufGetSize(tPbuf Pbuf_No, uint32_t *Count)
{
	MacErr_t result = CheckPbuf(Pbuf_No);

	if (mnvm_noErr == result) {
		*Count = PbufSize[Pbuf_No];
	}

	return result;
}
#endif

 bool FirstFreeDisk(tDrive *Drive_No)
{
	tDrive i;

	for (i = 0; i < NumDrives; ++i) {
		if (! vSonyIsInserted(i)) {
			if (nullptr != Drive_No) {
				*Drive_No = i;
			}
			return true;
		}
	}
	return false;
}

bool AnyDiskInserted(void)
{
	return 0 != vSonyInsertedMask;
}

void DiskRevokeWritable(tDrive Drive_No)
{
	vSonyWritableMask &= ~ ((uint32_t)1 << Drive_No);
}

void DiskInsertNotify(tDrive Drive_No, bool locked)
{
	vSonyInsertedMask |= ((uint32_t)1 << Drive_No);
	if (! locked) {
		vSonyWritableMask |= ((uint32_t)1 << Drive_No);
	}

	QuietEnds();
}

void DiskEjectedNotify(tDrive Drive_No)
{
	vSonyWritableMask &= ~ ((uint32_t)1 << Drive_No);
	vSonyInsertedMask &= ~ ((uint32_t)1 << Drive_No);
}

 uint8_t * screencomparebuff = nullptr;

#if WantColorTransValid
static bool ColorTransValid = false;
#endif

 bool EmVideoDisable = false;
 uint32_t OnTrueTime = 0;
 
 int16_t ScreenChangedTop, ScreenChangedLeft, ScreenChangedBottom, ScreenChangedRight;

void ScreenClearChanges(void)
{
	ScreenChangedTop = vMacScreenHeight;
	ScreenChangedBottom = 0;
	ScreenChangedLeft = vMacScreenWidth;
	ScreenChangedRight = 0;
}

void ScreenChangedAll(void)
{
	ScreenChangedTop = 0;
	ScreenChangedBottom = vMacScreenHeight;
	ScreenChangedLeft = 0;
	ScreenChangedRight = vMacScreenWidth;
}

#if MayFullScreen
 uint16_t ViewHSize, ViewVSize, ViewHStart = 0, ViewVStart = 0;
#endif

static void SetLongs(uint32_t *p, long n)
{
	long i;

	for (i = n; --i >= 0; ) {
		*p++ = (uint32_t) -1;
	}
}

 uimr ReserveAllocOffset;
 uint8_t * ReserveAllocBigBlock = nullptr;

void ReserveAllocOneBlock(uint8_t * *p, uimr n,
	uint8_t align, bool FillOnes)
{
	ReserveAllocOffset = CeilPow2Mult(ReserveAllocOffset, align);
	if (nullptr == ReserveAllocBigBlock) {
		*p = nullptr;
	} else {
		*p = ReserveAllocBigBlock + ReserveAllocOffset;
		if (FillOnes) {
			SetLongs((uint32_t *)*p, n / 4);
		}
	}
	ReserveAllocOffset += n;
}

void EmulationReserveAlloc(void)
{
	ReserveAllocOneBlock(&RAM, kRAM_Size + RAMSafetyMarginFudge, 5, false);
#if EmVidCard
	ReserveAllocOneBlock(&VidROM, kVidROM_Size, 5, false);
#endif
#if IncludeVidMem
	ReserveAllocOneBlock(&VidMem, kVidMemRAM_Size + RAMSafetyMarginFudge, 5, true);
#endif
#if SmallGlobals
	MINEM68K_ReserveAlloc();
#endif
}

/* --- sending debugging info to file --- */

#if dbglog_HAVE

static uimr dbglog_bufpos = 0;
static char *dbglog_bufp = nullptr;

static void dbglog_ReserveAlloc(void)
{
	ReserveAllocOneBlock((uint8_t * *)&dbglog_bufp, dbglog_bufsz,
		5, false);
}

static void dbglog_close(void)
{
	uimr n = ModPow2(dbglog_bufpos, dbglog_buflnsz);
	if (n != 0) {
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
	if (newbufdiv != curbufdiv) {
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

	while (*p++ != 0) {
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

	do {
		v = x & 0x0F;
		if (v < 10) {
			*--p = '0' + v;
		} else {
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

	do {
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

	if ((x > 32) && (x < 127)) {
		s = x;
	} else {
		s = '?';
	}

	dbglog_write(&s, 1);
}

static void dbglog_writeSpace(void)
{
	dbglog_writeCStr(" ");
}

#endif

/* MacMsg */

 char *SavedBriefMsg = nullptr;
 char *SavedLongMsg = nullptr;

#if WantAbnormalReports
 uint16_t SavedIDMsg = 0;
#endif

 bool SavedFatalMsg = false;

void MacMsg(char *briefMsg, char *longMsg, bool fatal)
{
	if (nullptr != SavedBriefMsg) {
		/*
			ignore the new message, only display the
			first error.
		*/
	} else {
		SavedBriefMsg = briefMsg;
		SavedLongMsg = longMsg;
		SavedFatalMsg = fatal;
	}
}

#if WantAbnormalReports
void WarnMsgAbnormalID(uint16_t id)
{
	MacMsg(kStrReportAbnormalTitle,
		kStrReportAbnormalMessage, false);

	if (0 != SavedIDMsg) {
		/*
			ignore the new message, only display the
			first error.
		*/
	} else {
		SavedIDMsg = id;
	}
}
#endif
