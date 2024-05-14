/*
	UTILS/param_buffers.h

	Copyright (C) 2018 Paul C. Pratt

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
	Parameter BUFfers implemented with STanDard C library
*/

#pragma once

#if IncludePbufs
static void *PbufDat[NumPbufs];

static MacErr_t PbufNewFromPtr(void *p, uint32_t count, tPbuf *r)
{
	tPbuf i;
	MacErr_t err;

	if (! FirstFreePbuf(&i)) {
		free(p);
		err = mnvm_miscErr;
	} else {
		*r = i;
		PbufDat[i] = p;
		PbufNewNotify(i, count);
		err = mnvm_noErr;
	}

	return err;
}
static void PbufKillToPtr(void **p, uint32_t *count, tPbuf r)
{
	*p = PbufDat[r];
	*count = PbufSize[r];

	PbufDisposeNotify(r);
}
MacErr_t PbufNew(uint32_t count, tPbuf *r)
{
	MacErr_t err = mnvm_miscErr;

	void *p = calloc(1, count);
	if (nullptr != p) {
		err = PbufNewFromPtr(p, count, r);
	}

	return err;
}
void PbufDispose(tPbuf i)
{
	void *p;
	uint32_t count;

	PbufKillToPtr(&p, &count, i);

	free(p);
}
static void UnInitPbufs(void)
{
	tPbuf i;

	for (i = 0; i < NumPbufs; ++i) {
		if (PbufIsAllocated(i)) {
			PbufDispose(i);
		}
	}
}
#define PbufHaveLock 1

static uint8_t * PbufLock(tPbuf i)
{
	return (uint8_t *)PbufDat[i];
}

#define PbufUnlock(i)

void PbufTransfer(uint8_t * Buffer,
	tPbuf i, uint32_t offset, uint32_t count, bool IsWrite)
{
	void *p = ((uint8_t *)PbufDat[i]) + offset;
	if (IsWrite) {
		(void) memcpy(p, Buffer, count);
	} else {
		(void) memcpy(Buffer, p, count);
	}
}
#endif
