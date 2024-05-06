/*
	Adapted from code in vMac.

	Copyright (C) 2009 Bernd Schmidt, Philip Cummins, Paul C. Pratt

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
	Hardware.
*/

#include "sys_dependencies.h"

#include "UI/my_os_glue.h"
#include "EMCONFIG.h"
#include "global_glue.h"

#include "HW/hardware.h"

// Sets up commands that can be called for all the various devices, and iterators over them.
// Should probably be converted to two classes, one for an individual device, and one for all devices, with a way of enabling and disabling them.

const DevMethods_t DEVICES[] = {
	// RTC
	{
	.init = EmRTC ? RTC_Init : nullptr,
	.reset = nullptr,
	.starttick = nullptr,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},
	// ROM
	{
	.init = ROM_Init,
	.reset = nullptr,
	.starttick = nullptr,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},
	// Memory
	{
	.init = AddrSpac_Init,
	.reset = Memory_Reset,
	.starttick = nullptr,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},
	// ICT
	{
	.init = nullptr,
	.reset = ICT_Zap,
	.starttick = nullptr,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},
	// IWM
	{
	.init = nullptr,
	.reset = IWM_Reset,
	.starttick = nullptr,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},
	// SCC
	{
	.init = nullptr,
	.reset = SCC_Reset,
	.starttick = nullptr,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},
	// SCSI
	{
	.init = nullptr,
	.reset = SCSI_Reset,
	.starttick = nullptr,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},
	// VIA1
	{
	.init = VIA1_Zap,
	.reset = VIA1_Reset,
	.starttick = nullptr,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = VIA1_ExtraTimeBegin,
	.timeend = VIA1_ExtraTimeEnd,
	},
	// VIA2
	{
	.init = nullptr,
	.reset = EmVIA2 ? VIA2_Zap : nullptr,
	.starttick = nullptr,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = EmVIA2 ? VIA2_ExtraTimeBegin : nullptr,
	.timeend = EmVIA2 ? VIA2_ExtraTimeEnd : nullptr,
	},
	// Sony disk drive
	{
	.init = nullptr,
	.reset = Sony_Reset,
	.starttick = Sony_Update,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},
	// Extn
	{
	.init = nullptr,
	.reset = Extn_Reset,
	.starttick = nullptr,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},
	// m68k
	{
	.init = nullptr,
	.reset = m68k_reset,
	.starttick = nullptr,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},
	// Mouse
	{
	.init = nullptr,
	.reset = nullptr,
	.starttick = Mouse_Update,
	.endtick = Mouse_EndTickNotify,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},
	// Classic Keyboard
	{
	.init = nullptr,
	.reset = nullptr,
	.starttick = EmClassicKbrd ? KeyBoard_Update : nullptr,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},
    // ADB
	{
	.init = nullptr,
	.reset = nullptr,
	.starttick = EmADB ? ADB_Update : nullptr,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},
	// LocalTalk
	{
	.init = nullptr,
	.reset = nullptr,
	.starttick = /*EmLocalTalk ? LocalTalkTick :*/ nullptr,
	.endtick = nullptr,
    .subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},
	// Video card
	{
	.init = EmVidCard ? Vid_Init : nullptr,
	.reset = nullptr,
	.starttick = EmVidCard ? Vid_Update : nullptr,
	.endtick = nullptr,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	},
	// ASC
	{
	.init = nullptr,
	.reset = nullptr,
	.starttick = nullptr,
	.endtick = nullptr,
	.subtick = EmASC ? ASC_SubTick : nullptr,
	.timebegin = nullptr,
	.timeend = nullptr
	},
	// Sound (non-ASC)
	{
	.init = nullptr,
	.reset = nullptr,
	.starttick = nullptr,
	.endtick = nullptr,
	.subtick = (SoundEnabled && (CurEmMd != kEmMd_PB100)) ? MacSound_SubTick : nullptr,
	.timebegin = nullptr,
	.timeend = nullptr
	},
	// Screen
	{
	.init = Screen_Init,
	.reset = nullptr,
	.starttick = Sixtieth_PulseNtfy, // VBlank interrupt
	.endtick = Screen_EndTickNotify,
	.subtick = nullptr,
	.timebegin = nullptr,
	.timeend = nullptr,
	}
};

void devices_init(void) 
{
	for(auto &device : DEVICES) {
		if (device.init != nullptr) { device.init(); }
	} 
}

void devices_reset(void) 
{
	for(auto &device : DEVICES) {
		if (device.reset != nullptr) { device.reset(); }
	} 
}

void devices_starttick(void) 
{
	for(auto &device : DEVICES) {
		if (device.starttick != nullptr) { device.starttick(); }
	} 
}
void devices_endtick(void) 
{
	for(auto &device : DEVICES) {
		if (device.endtick != nullptr) { device.endtick(); }
	} 
}

void devices_subtick(int value) 
{
	for(auto &device : DEVICES) {
		if (device.subtick != nullptr) { device.subtick(value); }
	} 
}

void devices_timebegin() 
{
	for(auto &device : DEVICES) {
		if (device.timebegin != nullptr) { device.timebegin(); }
	} 
}

void devices_timeend() 
{
	for(auto &device : DEVICES) {
		if (device.timeend != nullptr) { device.timeend(); }
	} 
}