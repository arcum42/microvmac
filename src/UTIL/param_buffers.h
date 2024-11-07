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

#include "sys_dependencies.h"
#include "error_codes.h"

#if IncludePbufs

typedef uint16_t tPbuf;

const tPbuf NotAPbuf = ((tPbuf)0xFFFF);
extern uint32_t PbufAllocatedMask;
extern uint32_t PbufSize[NumPbufs];

/* extension mechanism */
enum kCmndPbuf
{
	kCmndPbufFeatures = 1,
	kCmndPbufNew = 2,
	kCmndPbufDispose = 3,
	kCmndPbufGetSize = 4,
	kCmndPbufTransfer = 5
};

extern bool FirstFreePbuf(tPbuf *r);
extern void PbufNewNotify(tPbuf Pbuf_No, uint32_t count);
extern void PbufDisposeNotify(tPbuf Pbuf_No);
extern MacErr_t CheckPbuf(tPbuf Pbuf_No);
extern MacErr_t PbufGetSize(tPbuf Pbuf_No, uint32_t *Count);
extern MacErr_t PbufNewFromPtr(void *p, uint32_t count, tPbuf *r);
extern void PbufKillToPtr(void **p, uint32_t *count, tPbuf r);
extern MacErr_t PbufNew(uint32_t count, tPbuf *r);
extern void PbufDispose(tPbuf i);
extern void UnInitPbufs(void);
extern uint8_t *PbufLock(tPbuf i);
extern void PbufTransfer(uint8_t *Buffer, tPbuf i, uint32_t offset, uint32_t count, bool IsWrite);
extern void ExtnParamBuffers_Access(CPTR p);
#endif
