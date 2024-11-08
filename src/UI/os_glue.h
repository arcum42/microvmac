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

#include "UTIL/date_to_seconds.h"
#include "HW/KBRD/keycodes.h"
#include "error_codes.h"
#include <SDL.h>
#include "UI/sound_sdl2.h"

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
	uint8_t * *p, uimr n, uint8_t align, bool FillOnes
);
extern void MoveBytes(anyp srcPtr, anyp destPtr, int32_t byteCount);

extern uint8_t * ROM;

#if IncludeHostTextClipExchange
extern MacErr_t HTCEexport(tPbuf i);
extern MacErr_t HTCEimport(tPbuf *r);
#endif

extern uint32_t OnTrueTime;
extern uint32_t CurMacDateInSeconds;

extern bool UseColorMode;
extern bool ColorModeWorks;

extern bool ColorMappingChanged;

extern void Screen_OutputFrame(uint8_t * screencurrentbuff);
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

extern void DoKeyCode(SDL_Keysym *r, bool down);

/*** Might be SDL2-specific? ***/
// INTL.c
void NativeStrFromCStr(char *r, char *s);
// DRIVES.c
void InitDrives();
bool Sony_Insert1a(char *drivepath, bool silentfail);
bool LoadInitialImages();
void UnInitDrives();
// MOUSE.c
void ForceShowCursor();
void CheckMouseState();
// KEYBOARD.c
void DisconnectKeyCodes3();
void ReconnectKeyCodes3();
void DisableKeyRepeat();
void RestoreKeyRepeat();
// SOUND.c
void Sound_Start();
void Sound_Stop();
void Sound_SecondNotify();
bool Sound_Init();
void Sound_UnInit();
// TIMEDATE.c
void StartUpTimeAdjust();
bool UpdateTrueEmulatedTime();
bool CheckDateTime();
bool InitLocationDat();
void IncrNextTime(void);
// ROM.c
bool LoadMacRom();
MacErr_t LoadMacRomFrom(const char *path);
// OSGLUSD2.c
void EnterSpeedStopped();
void LeaveSpeedStopped();

#endif
