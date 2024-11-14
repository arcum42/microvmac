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

#include <stdio.h>

#include "sys_dependencies.h"
#include "config.h"

#include "UTIL/endian.h"
#include "UTIL/param_buffers.h"

#include "LANG/intl_chars.h"
#include "HW/SCREEN/screen.h"
#include "HW/ROM/rom.h"

#include "UI/os_glue.h"
#include "UI/event_queue.h"
#include "UI/memory.h"
#include "UI/video_sdl2.h"

#if CanGetAppPath
[[maybe_unused]] char *app_parent = nullptr;
[[maybe_unused]] char *pref_dir = nullptr;
#endif

bool NeedWholeScreenDraw = false;
bool RequestMacOff = false;
bool ForceMacOff = false;
bool WantMacInterrupt = false;
bool WantMacReset = false;
bool RunInBackground = (WantInitRunInBackground != 0);

bool RequestInsertDisk = false;
uint8_t RequestIthDisk = 0;

/* --- some simple utilities --- */

void MoveBytes(anyp srcPtr, anyp destPtr, int32_t byteCount)
{
	(void)memcpy((char *)destPtr, (char *)srcPtr, byteCount);
}

/* --- basic dialogs --- */

/* MacMsg */

char *SavedBriefMsg = nullptr;
char *SavedLongMsg = nullptr;

#if WantAbnormalReports
uint16_t SavedIDMsg = 0;
#endif

bool SavedFatalMsg = false;

void MacMsg(char *briefMsg, char *longMsg, bool fatal)
{
	if (nullptr != SavedBriefMsg)
	{
		/*
			ignore the new message, only display the
			first error.
		*/
	}
	else
	{
		SavedBriefMsg = briefMsg;
		SavedLongMsg = longMsg;
		SavedFatalMsg = fatal;
	}
}

#if WantAbnormalReports
void WarnMsgAbnormalID(uint16_t id)
{
	MacMsg(kStrReportAbnormalTitle,
		   kStrReportAbnormalMessage, false);

	if (0 != SavedIDMsg)
	{
		/*
			ignore the new message, only display the
			first error.
		*/
	}
	else
	{
		SavedIDMsg = id;
	}
}
#endif

void MayFree(char *p)
{
	if (nullptr != p)
	{
		free(p);
	}
}

MacErr_t ChildPath(char *x, char *y, char **r)
{
	MacErr_t err = mnvm_miscErr;
	int nx = strlen(x);
	int ny = strlen(y);
	{
		if ((nx > 0) && (PathSep == x[nx - 1]))
		{
			--nx;
		}
		{
			int nr = nx + 1 + ny;
			char *p = (char *)malloc(nr + 1);
			if (p != nullptr)
			{
				char *p2 = p;
				(void)memcpy(p2, x, nx);
				p2 += nx;
				*p2++ = PathSep;
				(void)memcpy(p2, y, ny);
				p2 += ny;
				*p2 = 0;
				*r = p;
				err = mnvm_noErr;
			}
		}
	}

	return err;
}

void NativeStrFromCStr(char *r, char *s)
{
	uint8_t ps[ClStrMaxLength];
	int i;
	int L;

	ClStrFromSubstCStr(&L, ps, s);

	for (i = 0; i < L; ++i)
	{
		r[i] = Cell2PlainAsciiMap[ps[i]];
	}

	r[L] = 0;
}


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

/* --- event handling for main window --- */

static bool CaughtMouse = false;

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
	Sound_Start();

	StartUpTimeAdjust();
}

void EnterSpeedStopped(void)
{
	Sound_Stop();
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
	bool waiting = true;

	do
	{
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
			continue;
		}

		if (ExtraTimeNotOver())
		{
			(void)SDL_Delay(NextIntTime - LastTime);
			continue;
		}

		if (CheckDateTime())
		{
			Sound_SecondNotify();
		}

		if ((!gBackgroundFlag) && (!CaughtMouse))
		{
			CheckMouseState();
		}

		OnTrueTime = TrueEmulatedTime;
		waiting = false;

#if dbglog_TimeStuff
		spdlog::debug("WaitForNextTick, OnTrueTime = {}", OnTrueTime);
#endif
	} while (waiting);
}

void ZapOSGLUVars(void)
{
	InitDrives();
	ZapWinStateVars();
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
	config_init();
	if (!AllocMemory())
		return false;
#if CanGetAppPath
	if (!InitWhereAmI())
		return false;
#endif
#if dbglog_HAVE
	if (!dbglog_open())
		return false;
#endif
	if (!ScanCommandLine())
		return false;
	if (!LoadMacRom())
		return false;
	if (!LoadInitialImages())
		return false;
	if (!InitLocationDat())
		return false;
	if (!SDL_InitDisplay())
		return false; // Switched before initting sound because SDL is initialized in SDL_InitDisplay. Probably should be initialised earlier.
	if (!Sound_Init())
		return false;
	if (!CreateMainWindow())
		return false;
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
	Sound_Stop();
	Sound_UnInit();
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
