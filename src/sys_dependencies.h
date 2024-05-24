/*
	sys_dependencies.h

	Copyright (C) 2006 Bernd Schmidt, Philip Cummins, Paul C. Pratt

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
	SYStem DEPeNdencies.
*/

#pragma once

#ifndef SYSDEPNS_H
#define SYSDEPNS_H

#include <stdint.h>
#include <stdbool.h>

/*
	Configuration options used by both platform specific
	and platform independent code.
*/

#define SmallGlobals 0
#define cIncludeUnused 0
#define UnusedParam(p) (void) p

/* capabilities provided by platform specific code */

#define SoundEnabled 1
#define kLn2SoundSampSz 3

#define dbglog_HAVE 0
#define WantAbnormalReports 0

#define NumDrives 6
#define IncludeSonyRawMode 1
#define IncludeSonyGetName 1
#define IncludeSonyNew 1
#define IncludeSonyNameNew 1

#define IncludePbufs 1
#define NumPbufs 4

#define EnableMouseMotion 0

#define IncludeHostTextClipExchange 0
#define AutoLocation 1
#define AutoTimeZone 1

// Variable versions of configuration flags
extern uint16_t vMacScreenHeight;
extern uint16_t vMacScreenWidth;
extern uint16_t vMacScreenDepth;

/*
	Largest efficiently supported
	representation types. uimr should be
	large enough to hold number of elements
	of any array we will deal with.
*/
typedef uint32_t uimr;
typedef int32_t simr;

#define anyp uint8_t *

/* pascal string, single byte characters */
#define ps3p uint8_t *

#ifndef reg_call
#define reg_call
#endif

	/*
		For functions in operating system glue that
		are called by rest of program.
	*/

/*
	best type for uint16_t that is probably in register
	(when compiler messes up otherwise)
*/

#ifndef BigEndianUnaligned
#define BigEndianUnaligned 0
#endif

#ifndef LittleEndianUnaligned
#define LittleEndianUnaligned 0
#endif

#ifndef align_8
#define align_8
#endif

#ifndef cond_rare
#define cond_rare(x) (x)
#endif

#ifndef Have_ASR
#define Have_ASR 0
#endif

#ifndef HaveSwapUi5r
#define HaveSwapUi5r 0
#endif

#define MaxATTListN 32
#define IncludeExtnPbufs 1
// temporary
#define IncludeExtnHostTextClipExchange 0

#define Sony_SupportDC42 1
#define Sony_SupportTags 0
#define Sony_WantChecksumsUpdated 0
#define Sony_VerifyChecksums 0
#define CaretBlinkTime 0x03
#define SpeakerVol 0x07
#define DoubleClickTime 0x05
#define MenuBlink 0x03
#define AutoKeyThresh 0x06
#define AutoKeyRate 0x03

#define WantDisasm 0
#define ExtraAbnormalReports 0

#include "hw_config.h"
#include "HW/wires.h"
#include "global_glue.h"

#endif