#pragma once

#include <cstdint>
#include "sys_dependencies.h"

/* map of address space */

constexpr uint32_t kRAM_Base = 0x00000000; /* when overlay off */
#if (CurEmMd == kEmMd_PB100)
constexpr uint32_t kRAM_ln2Spc = 23;
#elif (CurEmMd == kEmMd_II) || (CurEmMd == kEmMd_IIx)
constexpr uint32_t kRAM_ln2Spc = 23;
#else
constexpr uint32_t kRAM_ln2Spc = 22;
#endif

#if IncludeVidMem
#if CurEmMd == kEmMd_PB100
constexpr uint32_t kVidMem_Base = 0x00FA0000;
constexpr uint32_t kVidMem_ln2Spc = 16;
#else
constexpr uint32_t kVidMem_Base = 0x00540000;
constexpr uint32_t kVidMem_ln2Spc = 18;
#endif
#endif

#if CurEmMd == kEmMd_PB100
constexpr uint32_t kSCSI_Block_Base = 0x00F90000;
constexpr uint32_t kSCSI_ln2Spc = 16;
#else
constexpr uint32_t kSCSI_Block_Base = 0x00580000;
constexpr uint32_t kSCSI_ln2Spc = 19;
#endif

constexpr uint32_t kRAM_Overlay_Base = 0x00600000; /* when overlay on */
constexpr uint32_t kRAM_Overlay_Top = 0x00800000;

#if CurEmMd == kEmMd_PB100
constexpr uint32_t kSCCRd_Block_Base = 0x00FD0000;
constexpr uint32_t kSCC_ln2Spc = 16;
#else
constexpr uint32_t kSCCRd_Block_Base = 0x00800000;
constexpr uint32_t kSCC_ln2Spc = 22;
#endif

#if CurEmMd != kEmMd_PB100
constexpr uint32_t kSCCWr_Block_Base = 0x00A00000;
constexpr uint32_t kSCCWr_Block_Top  = 0x00C00000;
#endif

#if CurEmMd == kEmMd_PB100
constexpr uint32_t kIWM_Block_Base = 0x00F60000;
constexpr uint32_t kIWM_ln2Spc = 16;
#else
constexpr uint32_t kIWM_Block_Base = 0x00C00000;
constexpr uint32_t kIWM_ln2Spc = 21;
#endif

#if CurEmMd == kEmMd_PB100
constexpr uint32_t kVIA1_Block_Base = 0x00F70000;
constexpr uint32_t kVIA1_ln2Spc = 16;
#else
constexpr uint32_t kVIA1_Block_Base = 0x00E80000;
constexpr uint32_t kVIA1_ln2Spc = 19;
#endif

#if CurEmMd == kEmMd_PB100
constexpr uint32_t kASC_Block_Base = 0x00FB0000;
constexpr uint32_t kASC_ln2Spc = 16;
#endif
constexpr uint32_t kASC_Mask = 0x00000FFF;


/* implementation of read/write for everything but RAM and ROM */

constexpr uint32_t kSCC_Mask = 0x03;

constexpr uint32_t kVIA1_Mask = 0x00000F;
#if EmVIA2
constexpr uint32_t kVIA2_Mask = 0x00000F;
#endif

constexpr uint32_t kIWM_Mask = 0x00000F; /* Allocated Memory Bandwidth for IWM */

#if CurEmMd <= kEmMd_512Ke
constexpr uint32_t ROM_CmpZeroMask = 0;
#elif CurEmMd <= kEmMd_Plus
#if kROM_Size > 0x00020000
constexpr uint32_t ROM_CmpZeroMask = 0; /* For hacks like Mac ROM-inator */
#else
constexpr uint32_t ROM_CmpZeroMask = 0x00020000;
#endif
#elif CurEmMd <= kEmMd_PB100
constexpr uint32_t ROM_CmpZeroMask = 0;
#elif CurEmMd <= kEmMd_IIx
constexpr uint32_t ROM_CmpZeroMask = 0;
#else
#error "ROM_CmpZeroMask not defined"
#endif

constexpr uint32_t kROM_cmpmask = (0x00F00000 | ROM_CmpZeroMask);

#if CurEmMd <= kEmMd_512Ke
constexpr uint32_t Overlay_ROM_CmpZeroMask = 0x00100000;
#elif CurEmMd <= kEmMd_Plus
constexpr uint32_t Overlay_ROM_CmpZeroMask = 0x00020000;
#elif CurEmMd <= kEmMd_Classic
constexpr uint32_t Overlay_ROM_CmpZeroMask = 0x00300000;
#elif CurEmMd <= kEmMd_PB100
constexpr uint32_t Overlay_ROM_CmpZeroMask = 0;
#elif CurEmMd <= kEmMd_IIx
constexpr uint32_t Overlay_ROM_CmpZeroMask = 0;
#else
#error "Overlay_ROM_CmpZeroMask not defined"
#endif


static constexpr uint32_t RAMSafetyMarginFudge = 4;

static constexpr uint32_t kRAM_Size = (kRAMa_Size + kRAMb_Size);

extern uint8_t * RAM;
	/*
		allocated by MYOSGLUE to be at least
			kRAM_Size + RAMSafetyMarginFudge
		bytes. Because of shortcuts taken in GLOBGLUE.c, it is in theory
		possible for the emulator to write up to 3 bytes past kRAM_Size.
	*/

#if EmVidCard
extern uint8_t * VidROM;
#endif

#if IncludeVidMem
extern uint8_t * VidMem;
#endif
#if !IncludeVidMem
static constexpr uint32_t kMain_Offset = 0x5900;
static constexpr uint32_t kAlternate_Offset = 0xD900;
static constexpr uint32_t kMain_Buffer = (kRAM_Size - kMain_Offset);
static constexpr uint32_t kAlternate_Buffer = (kRAM_Size - kAlternate_Offset);

#define get_ram_address(addr) ((addr) + RAM)

#endif