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

#pragma once

#include "PATCHES/rom.h"

#include "M68K/instruct_table_68k.h"
#include "M68K/cpu_68k.h"
#include "VIA/via.h"
#include "VIA/via2.h"
#include "DISK/int_woz_machine.h"
#include "SCC/serial_comm.h"
#include "RTC/rtc.h"
#include "SCSI/scsi.h"
#include "DISK/sony_floppy.h"
#include "SCREEN/screen.h"
#include "VIDCARD/video.h"
#include "KBRD/keyboard.h"
#include "POWERMAN/power.h"
#include "ADB/adb.h"
#include "SOUND/apple_sound_chip.h"
#include "SOUND/sound.h"
#include "MOUSE/mouse.h"

enum {
    DEV_RTC,
    DEV_ROM,
    DEV_MEMORY,
    DEV_ICT,
    DEV_IWM,
    DEV_SCC,
    DEV_SCSI,
    DEV_VIA1,
    DEV_VIA2,
    DEV_SONY,
    DEV_EXTN,
    DEV_68K,
    DEV_MOUSE,
    DEV_KEYBOARD,
    DEV_ADB,
    DEV_LOCALTALK,
    DEV_VIDEO,
    DEV_ASC,
    DEV_SOUND,
    DEV_SCREEN,
    DEV_MAX
};

typedef struct DevMethods {
	bool (*init)(void);
	void (*reset)(void);
	void (*starttick)(void);
	void (*endtick)(void);
	void (*subtick)(int);
	void (*timebegin)(void);
	void (*timeend)(void);
} DevMethods_t;

extern DevMethods_t DEVICES[DEV_MAX];

extern void devices_setup(void);

extern void devices_init(void);
extern void devices_reset(void);
extern void devices_starttick(void);
extern void devices_endtick(void);
extern void devices_subtick(int value);
extern void devices_timebegin();
extern void devices_timeend();

typedef struct device_config {
    bool classic_keyboard;
    bool adb;
    bool rtc;
    bool pmu;
    bool via2;
    bool use68020;
    bool fpu;
    bool mmu;
    bool asc;
    bool video_card;
    bool video_memory;
} device_config_t;

extern device_config_t hw_config;

#define EmClassicKbrd 1
#define EmADB 0
#define EmRTC 1
#define EmPMU 0
#define EmVIA2 0
#define Use68020 0
#define EmFPU 0
#define EmMMU 0
#define EmASC 0

#define IncludeVidMem 0
#define EmVidCard 0