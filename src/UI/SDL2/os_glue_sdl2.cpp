/*
	OSGLUSD2.c

	Copyright (C) 2012 Paul C. Pratt, Manuel Alfayate

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
	Operating System GLUe for SDl 2.0 library

	All operating system dependent code for the
	SDL Library should go here.
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <SDL.h>
#include "CNFGRAPI.h"
#include "sys_dependencies.h"
#include "UTIL/endian.h"
#include "UI/my_os_glue.h"
#include "STRCONST.h"
#include "os_glue_sdl2.h"
#include "LANG/intl_chars.h"
#include "HW/SCREEN/screen.h"
#include "HW/ROM/rom.h"
#include "config_manage.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "imgui.h"

extern bool ReCreateMainWindow();
extern void ZapWinStateVars();
extern bool CreateMainWindow();
extern void CloseMainWindow();
extern bool SDL_InitDisplay();

#if MayFullScreen
extern bool GrabMachine;
extern void GrabTheMachine();
extern void UngrabMachine();
#endif
/* --- some simple utilities --- */

void MoveBytes(anyp srcPtr, anyp destPtr, int32_t byteCount)
{
	(void)memcpy((char *)destPtr, (char *)srcPtr, byteCount);
}

/* --- information about the environment --- */

#define WantColorTransValid 0

#include "UI/common_os_glue.h"
#include "UTIL/param_buffers.h"
#include "UI/control_mode.h"

/* --- basic dialogs --- */

static void CheckSavedMacMsg(void)
{
	/* called only on quit, if error saved but not yet reported */

	if (nullptr != SavedBriefMsg)
	{
		char briefMsg0[ClStrMaxLength + 1];
		char longMsg0[ClStrMaxLength + 1];

		NativeStrFromCStr(briefMsg0, SavedBriefMsg);
		NativeStrFromCStr(longMsg0, SavedLongMsg);

		if (0 != SDL_ShowSimpleMessageBox(
					 SDL_MESSAGEBOX_ERROR,
					 SavedBriefMsg,
					 SavedLongMsg,
					 main_wind))
		{
			fprintf(stderr, "%s\n", briefMsg0);
			fprintf(stderr, "%s\n", longMsg0);
		}

		SavedBriefMsg = nullptr;
	}
}

/* --- event handling for main window --- */

#define UseMotionEvents 1

#if UseMotionEvents
static bool CaughtMouse = false;
#endif

static void HandleTheEvent(SDL_Event *event)
{
	switch (event->type)
	{
	case SDL_QUIT:
		RequestMacOff = true;
		break;
	case SDL_WINDOWEVENT:
		switch (event->window.event)
		{
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			gTrueBackgroundFlag = 0;
			break;
		case SDL_WINDOWEVENT_FOCUS_LOST:
			gTrueBackgroundFlag = 1;
			break;
		case SDL_WINDOWEVENT_ENTER:
			CaughtMouse = 1;
			break;
		case SDL_WINDOWEVENT_LEAVE:
			CaughtMouse = 0;
			break;
		}
		break;
	case SDL_MOUSEMOTION:
#if EnableFSMouseMotion && !HaveWorkingWarp
		if (HaveMouseMotion)
		{
			MousePositionNotifyRelative(
				event->motion.xrel, event->motion.yrel);
		}
		else
#endif
		{
			MousePositionNotify(
				event->motion.x, event->motion.y);
		}
		break;
	case SDL_MOUSEBUTTONDOWN:
		/* any mouse button, we don't care which */
#if EnableFSMouseMotion && !HaveWorkingWarp
		if (HaveMouseMotion)
		{
			/* ignore position */
		}
		else
#endif
		{
			MousePositionNotify(
				event->button.x, event->button.y);
		}
		MouseButtonSet(true);
		break;
	case SDL_MOUSEBUTTONUP:
#if EnableFSMouseMotion && !HaveWorkingWarp
		if (HaveMouseMotion)
		{
			/* ignore position */
		}
		else
#endif
		{
			MousePositionNotify(
				event->button.x, event->button.y);
		}
		MouseButtonSet(false);
		break;
	case SDL_KEYDOWN:
		DoKeyCode(&event->key.keysym, true);
		break;
	case SDL_KEYUP:
		DoKeyCode(&event->key.keysym, false);
		break;
	case SDL_MOUSEWHEEL:
		if (event->wheel.x < 0)
		{
			Keyboard_UpdateKeyMap(MKC_Left, true);
			Keyboard_UpdateKeyMap(MKC_Left, false);
		}
		else if (event->wheel.x > 0)
		{
			Keyboard_UpdateKeyMap(MKC_Right, true);
			Keyboard_UpdateKeyMap(MKC_Right, false);
		}
		if (event->wheel.y < 0)
		{
			Keyboard_UpdateKeyMap(MKC_Down, true);
			Keyboard_UpdateKeyMap(MKC_Down, false);
		}
		else if (event->wheel.y > 0)
		{
			Keyboard_UpdateKeyMap(MKC_Up, true);
			Keyboard_UpdateKeyMap(MKC_Up, false);
		}
		break;
	case SDL_DROPFILE:
	{
		char *s = event->drop.file;

		(void)Sony_Insert1a(s, false);
		SDL_RaiseWindow(main_wind);
		SDL_free(s);
	}
	break;
#if 0
		case Expose: /* SDL doesn't have an expose event */
			int x0 = event->expose.x;
			int y0 = event->expose.y;
			int x1 = x0 + event->expose.width;
			int y1 = y0 + event->expose.height;

			if (x0 < 0) {
				x0 = 0;
			}
			if (x1 > vMacScreenWidth) {
				x1 = vMacScreenWidth;
			}
			if (y0 < 0) {
				y0 = 0;
			}
			if (y1 > vMacScreenHeight) {
				y1 = vMacScreenHeight;
			}
			if ((x0 < x1) && (y0 < y1)) {
				HaveChangedScreenBuff(y0, x0, y1, x1);
			}
			break;
#endif
	}
}

/* --- SavedTasks --- */

static void LeaveBackground(void)
{
	ReconnectKeyCodes3();
	DisableKeyRepeat();
}

static void EnterBackground(void)
{
	RestoreKeyRepeat();
	DisconnectKeyCodes3();

	ForceShowCursor();
}

void LeaveSpeedStopped(void)
{
#if SoundEnabled
	Sound_Start();
#endif

	StartUpTimeAdjust();
}

void EnterSpeedStopped(void)
{
#if SoundEnabled
	Sound_Stop();
#endif
}

static void CheckForSavedTasks(void)
{
	if (EvtQ.NeedRecover)
	{
		EvtQ.NeedRecover = false;

		/* Attempt cleanup, EvtQ.NeedRecover may get set again */
		EvtQ.TryRecoverFromFull();
	}

#if EnableFSMouseMotion && HaveWorkingWarp
	if (HaveMouseMotion)
	{
		MouseConstrain();
	}
#endif

	if (RequestMacOff)
	{
		RequestMacOff = false;
		/*if (AnyDiskInserted()) {
			MacMsgOverride(kStrQuitWarningTitle,
				kStrQuitWarningMessage);
		} else {*/
		ForceMacOff = true;
		//}
	}

	if (ForceMacOff)
	{
		return;
	}

	if (gTrueBackgroundFlag != gBackgroundFlag)
	{
		gBackgroundFlag = gTrueBackgroundFlag;
		if (gTrueBackgroundFlag)
		{
			EnterBackground();
		}
		else
		{
			LeaveBackground();
		}
	}

	// TODO: fix this
	/*if (CurSpeedStopped != (SpeedStopped ||
		(gBackgroundFlag && ! RunInBackground))){
		} else {
			LeaveSpeedStopped();
		}*/

#if EnableRecreateW
	if (0 || (UseMagnify != WantMagnify) || (UseFullScreen != WantFullScreen))
	{
		(void)ReCreateMainWindow();
	}
#endif

#if MayFullScreen
	if (GrabMachine != (UseFullScreen &&
						!(gTrueBackgroundFlag || CurSpeedStopped)))
	{
		GrabMachine = !GrabMachine;
		if (GrabMachine)
		{
			GrabTheMachine();
		}
		else
		{
			UngrabMachine();
		}
	}
#endif

	if (NeedWholeScreenDraw)
	{
		NeedWholeScreenDraw = false;
		ScreenChangedAll();
	}

#if NeedRequestIthDisk
	if (0 != RequestIthDisk)
	{
		Sony_InsertIth(RequestIthDisk);
		RequestIthDisk = 0;
	}
#endif

	if (HaveCursorHidden != (WantCursorHidden && !(gTrueBackgroundFlag || CurSpeedStopped)))
	{
		HaveCursorHidden = !HaveCursorHidden;
		(void)SDL_ShowCursor(
			HaveCursorHidden ? SDL_DISABLE : SDL_ENABLE);
	}
}

/* --- command line parsing --- */

// TODO: reimplement with an actual argument parsing library
static bool ScanCommandLine(void)
{
	return true;
}

/* --- main program flow --- */

bool ExtraTimeNotOver(void)
{
	UpdateTrueEmulatedTime();
	return TrueEmulatedTime == OnTrueTime;
}

static void WaitForTheNextEvent(void)
{
	SDL_Event event;

	if (SDL_WaitEvent(&event))
	{
		HandleTheEvent(&event);
	}
}

static void CheckForSystemEvents(void)
{
	SDL_Event event;
	int i = 10;

	while ((--i >= 0) && SDL_PollEvent(&event))
	{
		HandleTheEvent(&event);
	}
}

void WaitForNextTick(void)
{
label_retry:
	CheckForSystemEvents();
	CheckForSavedTasks();

	if (ForceMacOff)
	{
		return;
	}

	if (CurSpeedStopped)
	{
		DoneWithDrawingForTick();
		WaitForTheNextEvent();
		goto label_retry;
	}

	if (ExtraTimeNotOver())
	{
		(void)SDL_Delay(NextIntTime - LastTime);
		goto label_retry;
	}

	if (CheckDateTime())
	{
#if SoundEnabled
		Sound_SecondNotify();
#endif
	}

	if ((!gBackgroundFlag)
#if UseMotionEvents
		&& (!CaughtMouse)
#endif
	)
	{
		CheckMouseState();
	}

	OnTrueTime = TrueEmulatedTime;

#if dbglog_TimeStuff
	spdlog::debug("WaitForNextTick, OnTrueTime = {}", OnTrueTime);
#endif
}

/* --- platform independent code can be thought of as going here --- */

void ZapOSGLUVars(void)
{
	InitDrives();
	ZapWinStateVars();
}

static void ReserveAllocAll(void)
{
#if dbglog_HAVE
	dbglog_ReserveAlloc();
#endif
	ReserveAllocOneBlock(&ROM, kROM_Size, 5, false);

	ReserveAllocOneBlock(&screencomparebuff,
						 vMacScreenNumBytes, 5, true);
#if UseControlKeys
	ReserveAllocOneBlock(&CntrlDisplayBuff,
						 vMacScreenNumBytes, 5, false);
#endif

	ReserveAllocOneBlock(&CLUT_final, CLUT_finalsz, 5, false);
#if SoundEnabled
	ReserveAllocOneBlock((uint8_t **)&TheSoundBuffer,
						 dbhBufferSize, 5, false);
#endif

	EmulationReserveAlloc();
}

static bool AllocMemory(void)
{
	uimr n;
	bool IsOk = false;

	ReserveAllocOffset = 0;
	ReserveAllocBigBlock = nullptr;
	ReserveAllocAll();
	n = ReserveAllocOffset;
	ReserveAllocBigBlock = (uint8_t *)calloc(1, n);
	if (nullptr == ReserveAllocBigBlock)
	{
		MacMsg(kStrOutOfMemTitle, kStrOutOfMemMessage, true);
	}
	else
	{
		ReserveAllocOffset = 0;
		ReserveAllocAll();
		if (n != ReserveAllocOffset)
		{
			/* oops, program error */
		}
		else
		{
			IsOk = true;
		}
	}

	return IsOk;
}

static void UnallocMemory(void)
{
	if (nullptr != ReserveAllocBigBlock)
	{
		free((char *)ReserveAllocBigBlock);
	}
}

#if CanGetAppPath
static bool InitWhereAmI(void)
{
	app_parent = SDL_GetBasePath();

	pref_dir = SDL_GetPrefPath("gryphel", "minivmac");

	return true; /* keep going regardless */
}
#endif

#if CanGetAppPath
static void UninitWhereAmI(void)
{
	SDL_free(pref_dir);

	SDL_free(app_parent);
}
#endif

bool InitOSGLU(void)
{
	if (Config_TryInit())
		if (AllocMemory())
#if CanGetAppPath
			if (InitWhereAmI())
#endif
#if dbglog_HAVE
				if (dbglog_open())
#endif
					if (ScanCommandLine())
						if (LoadMacRom())
							if (LoadInitialImages())
								if (InitLocationDat())
									if (SDL_InitDisplay()) // Switched before initting sound because SDL is initialized in SDL_InitDisplay. Probably should be initialised earlier.
#if SoundEnabled
										if (Sound_Init())
#endif
											if (CreateMainWindow())
												if (WaitForRom())
												{
													return true;
												}
	return false;
}

void UnInitOSGLU(void)
{
	RestoreKeyRepeat();
#if MayFullScreen
	UngrabMachine();
#endif
#if SoundEnabled
	Sound_Stop();
#endif
#if SoundEnabled
	Sound_UnInit();
#endif
#if IncludePbufs
	UnInitPbufs();
#endif
	UnInitDrives();

	ForceShowCursor();

#if dbglog_HAVE
	dbglog_close();
#endif

#if CanGetAppPath
	UninitWhereAmI();
#endif
	UnallocMemory();

	CheckSavedMacMsg();

	CloseMainWindow();

	SDL_Quit();
}
