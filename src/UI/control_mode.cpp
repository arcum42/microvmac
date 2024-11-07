/*
	UI/CONTROLM.c

	Copyright (C) 2007 Paul C. Pratt

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
	CONTROL Mode
*/

#include <SDL.h>

#include "CNFGRAPI.h"
#include "STRCONST.h"
#include "error_codes.h"
#include "sys_dependencies.h"

#include "LANG/intl_chars.h"
#include "UI/my_os_glue.h"
#include "UTIL/endian.h"

#include "UI/control_mode.h"
#include "UI/SDL2/os_glue_sdl2.h"
#include "UI/SDL2/video_sdl2.h"

/* Constants and globals */

uimr SpecialModes = 0;
bool NeedWholeScreenDraw = false;
uint8_t * CntrlDisplayBuff = nullptr;

#define ControlBoxh0 0
#define ControlBoxw 62
#define ControlBoxv0 0

#define hLimit (ControlBoxh0 + ControlBoxw - 1)
#define hStart (ControlBoxh0 + 1)

typedef void (*SpclModeBody) (void);

//#define Keyboard_UpdateKeyMap1 Keyboard_UpdateKeyMap
//#define DisconnectKeyCodes1 DisconnectKeyCodes

void MacMsgOverride(const char *title, const char *msg)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, msg, main_wind);
}

[[maybe_unused]] static void WarnMsgCorruptedROM(void)
{
	MacMsgOverride(kStrCorruptedROMTitle, kStrCorruptedROMMessage);
}

[[maybe_unused]] static void WarnMsgUnsupportedROM(void)
{
	MacMsgOverride(kStrUnsupportedROMTitle,
		kStrUnsupportedROMMessage);
}

