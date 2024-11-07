/*
	HW/DISK/sony_floppy.h

	Copyright (C) 2004 Paul C. Pratt

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

#ifdef SONYEMDV_H
#error "header already included"
#else
#define SONYEMDV_H
#endif

#include "sys_dependencies.h"
#include "UTIL/param_buffers.h"
#include "error_codes.h"

extern void ExtnDisk_Access(CPTR p);
extern void ExtnSony_Access(CPTR p);

extern void Sony_SetQuitOnEject(void);

extern void Sony_EjectAllDisks(void);
extern void Sony_Reset(void);

extern void Sony_Update(void);


#define tDrive uint16_t

extern uint32_t vSonyWritableMask;
extern uint32_t vSonyInsertedMask;

#define vSonyIsInserted(Drive_No) \
	((vSonyInsertedMask & ((uint32_t)1 << (Drive_No))) != 0)

extern MacErr_t vSonyTransfer(bool IsWrite, uint8_t * Buffer,
	tDrive Drive_No, uint32_t Sony_Start, uint32_t Sony_Count,
	uint32_t *Sony_ActCount);
extern MacErr_t vSonyEject(tDrive Drive_No);
extern MacErr_t vSonyGetSize(tDrive Drive_No, uint32_t *Sony_Count);

extern bool AnyDiskInserted(void);
extern void DiskRevokeWritable(tDrive Drive_No);

#if IncludeSonyRawMode
extern bool vSonyRawMode;
#endif

#if IncludeSonyNew
extern bool vSonyNewDiskWanted;
extern uint32_t vSonyNewDiskSize;
extern MacErr_t vSonyEjectDelete(tDrive Drive_No);
#endif

#if IncludeSonyNameNew
extern tPbuf vSonyNewDiskName;
#endif

#if IncludeSonyGetName
extern MacErr_t vSonyGetName(tDrive Drive_No, tPbuf *r);
#endif

extern uint32_t vSonyWritableMask;
extern uint32_t vSonyInsertedMask;

#if IncludeSonyRawMode
extern bool vSonyRawMode;
#endif

#if IncludeSonyNew
extern bool vSonyNewDiskWanted;
extern uint32_t vSonyNewDiskSize;
#endif

#if IncludeSonyNameNew
extern tPbuf vSonyNewDiskName;
#endif