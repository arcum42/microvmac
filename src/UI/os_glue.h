/*
	my_os_glue.h

	Copyright (C) 2006 Philip Cummins, Richard F. Bannister,
		Paul C. Pratt

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
	MY Operating System GLUE.

	header file for operating system dependent code.
	the same header is used for all platforms.

	This code is descended from Richard F. Bannister's Macintosh
	port of vMac, by Philip Cummins.
*/

#pragma once

#ifndef MYOSGLUE_H
#define MYOSGLUE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include <SDL.h>

#include "sys_dependencies.h"

#include "UTIL/endian.h"
#include "UTIL/date_to_seconds.h"
#include "error_codes.h"
#include "HW/KBRD/keycodes.h"

#include "UI/sound_sdl2.h"

#ifndef CanGetAppPath
#define CanGetAppPath 1
#endif

#if CanGetAppPath
extern char *app_parent;
extern char *pref_dir;
#endif

#ifdef _WIN32
#define PathSep '\\'
#else
#define PathSep '/'
#endif

extern MacErr_t ChildPath(char *x, char *y, char **r);
extern void MayFree(char *p);

#if WantAbnormalReports
extern void WarnMsgAbnormalID(uint16_t id);
#endif

#if dbglog_HAVE
extern void dbglog_writeCStr(char *s);
extern void dbglog_writeReturn(void);
extern void dbglog_writeHex(uint32_t x);
extern void dbglog_writeNum(uint32_t x);
extern void dbglog_writeMacChar(uint8_t x);
#endif

extern void ReserveAllocOneBlock(
	uint8_t **p, uimr n, uint8_t align, bool FillOnes);
extern void MoveBytes(anyp srcPtr, anyp destPtr, int32_t byteCount);

extern uint8_t *ROM;

#if IncludeHostTextClipExchange
extern MacErr_t HTCEexport(tPbuf i);
extern MacErr_t HTCEimport(tPbuf *r);
#endif

extern uint32_t OnTrueTime;
extern uint32_t CurMacDateInSeconds;

extern bool UseColorMode;
extern bool ColorModeWorks;

extern bool ColorMappingChanged;

extern void Screen_OutputFrame(uint8_t *screencurrentbuff);
extern void DoneWithDrawingForTick(void);

extern bool ForceMacOff;
extern bool WantMacInterrupt;
extern bool WantMacReset;
extern bool ExtraTimeNotOver(void);

extern uint32_t TrueEmulatedTime;
extern uint32_t LastTime;
extern uint32_t NextIntTime;
extern uint32_t NextFracTime;

extern bool gBackgroundFlag;
extern bool gTrueBackgroundFlag;
extern bool CurSpeedStopped;

// Functions
extern void MacMsgOverride(const char *title, const char *msg);
extern void DoKeyCode(SDL_Keysym *r, bool down);

// DRIVES.c
extern void InitDrives();
extern bool Sony_Insert1a(char *drivepath, bool silentfail);
extern bool LoadInitialImages();
extern void UnInitDrives();

// MOUSE.c
extern void ForceShowCursor();
extern void CheckMouseState();

// KEYBOARD.c
extern void DisconnectKeyCodes3();
extern void ReconnectKeyCodes3();
extern void DisableKeyRepeat();
extern void RestoreKeyRepeat();

// SOUND.c
extern void Sound_Start();
extern void Sound_Stop();
extern void Sound_SecondNotify();
extern bool Sound_Init();
extern void Sound_UnInit();

// TIMEDATE.c
extern void StartUpTimeAdjust();
extern bool UpdateTrueEmulatedTime();
extern bool CheckDateTime();
extern bool InitLocationDat();
extern void IncrNextTime(void);

// ROM.c
extern bool LoadMacRom();
extern MacErr_t LoadMacRomFrom(const char *path);

// OSGLUSD2.c
extern void EnterSpeedStopped();
extern void LeaveSpeedStopped();

extern void MacMsg(char *briefMsg, char *longMsg, bool fatal);

#endif
