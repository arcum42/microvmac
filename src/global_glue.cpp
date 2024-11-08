/*
	GLOBGLUE.c

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

/*
	GLOBal GLUE (or GLOB of GLUE)

	Holds the program together.

	Some code here adapted from "custom.c" in vMac by Philip Cummins,
	in turn descended from code in the Un*x Amiga Emulator by
	Bernd Schmidt.

	TODO: Try to yank as much out of this as we possibly can.
*/

#include "sys_dependencies.h"

#include "UI/my_os_glue.h"
#include "UTIL/endian.h"

#include "global_glue.h"
#include "UTIL/param_buffers.h"

#include "HW/hardware.h"

/*
	ReportAbnormalID unused 0x111D - 0x11FF
*/

/*
	ReportAbnormalID ranges unused 0x12xx - 0xFFxx
*/

// Global variables (temporary location?)

extern uint8_t get_vm_byte(CPTR addr);
extern uint16_t get_vm_word(CPTR addr);
extern uint32_t get_vm_long(CPTR addr);

extern void put_vm_byte(CPTR addr, uint8_t b);
extern void put_vm_word(CPTR addr, uint16_t w);
extern void put_vm_long(CPTR addr, uint32_t l);

#if IncludeExtnHostTextClipExchange
enum kCmndHTCE
{
	kCmndHTCEFeatures = 1,
	kCmndHTCEExport = 2,
	kCmndHTCEImport = 3
};
#endif

#if IncludeExtnHostTextClipExchange
static void ExtnHostTextClipExchange_Access(CPTR p)
{
	MacErr_t result = mnvm_controlErr;

	switch (get_vm_word(p + ExtnDat_commnd))
	{
	case kCmndVersion:
		put_vm_word(p + ExtnDat_version, 1);
		result = mnvm_noErr;
		break;
	case kCmndHTCEFeatures:
		put_vm_long(p + ExtnDat_params + 0, 0);
		result = mnvm_noErr;
		break;
	case kCmndHTCEExport:
	{
		tPbuf Pbuf_No = get_vm_word(p + ExtnDat_params + 0);

		result = CheckPbuf(Pbuf_No);
		if (mnvm_noErr == result)
		{
			result = HTCEexport(Pbuf_No);
		}
	}
	break;
	case kCmndHTCEImport:
	{
		tPbuf Pbuf_No;
		result = HTCEimport(&Pbuf_No);
		put_vm_word(p + ExtnDat_params + 0, Pbuf_No);
	}
	break;
	}

	put_vm_word(p + ExtnDat_result, result);
}
#endif

constexpr uint32_t kFindExtnExtension = 0x64E1F58A;
constexpr uint32_t kDiskDriverExtension = 0x4C9219E6;
#if IncludeExtnPbufs
constexpr uint32_t kHostParamBuffersExtension = 0x314C87BF;
#endif
#if IncludeExtnHostTextClipExchange
constexpr uint32_t kHostClipExchangeExtension = 0x27B130CA;
#endif

enum kCmndFindExtn
{
	kCmndFindExtnFind = 1,
	kCmndFindExtnId2Code = 2,
	kCmndFindExtnCount = 3
};

constexpr uint32_t kParamFindExtnTheExtn = 8;
constexpr uint32_t kParamFindExtnTheId = 12;

static void ExtnFind_Access(CPTR p)
{
	MacErr_t result = mnvm_controlErr;

	switch (get_vm_word(p + ExtnDat_commnd))
	{
	case kCmndVersion:
		put_vm_word(p + ExtnDat_version, 1);
		result = mnvm_noErr;
		break;
	case kCmndFindExtnFind:
	{
		uint32_t extn = get_vm_long(p + kParamFindExtnTheExtn);

		if (extn == kDiskDriverExtension)
		{
			put_vm_word(p + kParamFindExtnTheId, kExtnDisk);
			result = mnvm_noErr;
		}
		else
#if IncludeExtnPbufs
			if (extn == kHostParamBuffersExtension)
		{
			put_vm_word(p + kParamFindExtnTheId,
						kExtnParamBuffers);
			result = mnvm_noErr;
		}
		else
#endif
#if IncludeExtnHostTextClipExchange
			if (extn == kHostClipExchangeExtension)
		{
			put_vm_word(p + kParamFindExtnTheId,
						kExtnHostTextClipExchange);
			result = mnvm_noErr;
		}
		else
#endif
			if (extn == kFindExtnExtension)
		{
			put_vm_word(p + kParamFindExtnTheId,
						kExtnFindExtn);
			result = mnvm_noErr;
		}
		else
		{
			/* not found */
		}
	}
	break;
	case kCmndFindExtnId2Code:
	{
		uint16_t extn = get_vm_word(p + kParamFindExtnTheId);

		if (extn == kExtnDisk)
		{
			put_vm_long(p + kParamFindExtnTheExtn,
						kDiskDriverExtension);
			result = mnvm_noErr;
		}
		else
#if IncludeExtnPbufs
			if (extn == kExtnParamBuffers)
		{
			put_vm_long(p + kParamFindExtnTheExtn,
						kHostParamBuffersExtension);
			result = mnvm_noErr;
		}
		else
#endif
#if IncludeExtnHostTextClipExchange
			if (extn == kExtnHostTextClipExchange)
		{
			put_vm_long(p + kParamFindExtnTheExtn,
						kHostClipExchangeExtension);
			result = mnvm_noErr;
		}
		else
#endif
			if (extn == kExtnFindExtn)
		{
			put_vm_long(p + kParamFindExtnTheExtn,
						kFindExtnExtension);
			result = mnvm_noErr;
		}
		else
		{
			/* not found */
		}
	}
	break;
	case kCmndFindExtnCount:
		put_vm_word(p + kParamFindExtnTheId, kNumExtns);
		result = mnvm_noErr;
		break;
	}

	put_vm_word(p + ExtnDat_result, result);
}

enum kDSK
{
	kDSK_Params_Hi = 0,
	kDSK_Params_Lo = 1,
	kDSK_QuitOnEject = 3 /* obsolete */
};

static uint16_t ParamAddrHi;

void Extn_Access(uint32_t Data, CPTR addr)
{
	switch (addr)
	{
	case kDSK_Params_Hi:
		ParamAddrHi = Data;
		break;
	case kDSK_Params_Lo:
	{
		CPTR p = ParamAddrHi << 16 | Data;

		ParamAddrHi = (uint16_t)-1;
		if (kcom_callcheck == get_vm_word(p + ExtnDat_checkval))
		{
			put_vm_word(p + ExtnDat_checkval, 0);

			switch (get_vm_word(p + ExtnDat_extension))
			{
			case kExtnFindExtn:
				ExtnFind_Access(p);
				break;
#if EmVidCard
			case kExtnVideo:
				ExtnVideo_Access(p);
				break;
#endif
#if IncludeExtnPbufs
			case kExtnParamBuffers:
				ExtnParamBuffers_Access(p);
				break;
#endif
#if IncludeExtnHostTextClipExchange
			case kExtnHostTextClipExchange:
				ExtnHostTextClipExchange_Access(p);
				break;
#endif
			case kExtnDisk:
				ExtnDisk_Access(p);
				break;
			case kExtnSony:
				ExtnSony_Access(p);
				break;
			default:
				put_vm_word(p + ExtnDat_result,
							mnvm_controlErr);
				break;
			}
		}
	}
	break;
	case kDSK_QuitOnEject:
		/* obsolete, kept for compatibility */
		Sony_SetQuitOnEject();
		break;
	}
}

void Extn_Reset(void)
{
	ParamAddrHi = (uint16_t)-1;
}
