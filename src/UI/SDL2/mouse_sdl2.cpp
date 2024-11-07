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
#include "UI/event_queue.h"
#include "UI/SDL2/video_sdl2.h"

/* --- mouse --- */

#if MayFullScreen
extern int hOffset;
extern int vOffset;
#endif

/* cursor hiding */

bool HaveCursorHidden = false;
bool WantCursorHidden = false;

void ForceShowCursor(void)
{
	if (HaveCursorHidden) {
		HaveCursorHidden = false;
		(void) SDL_ShowCursor(SDL_ENABLE);
	}
}

/* cursor moving */

#ifndef HaveWorkingWarp
#define HaveWorkingWarp 1
#endif

#if EnableMoveMouse && HaveWorkingWarp
bool MoveMouse(int16_t h, int16_t v)
{
	if (UseFullScreen)
#if MayFullScreen
	{
		h -= ViewHStart;
		v -= ViewVStart;
	}
#endif

	if (UseMagnify) {
		h *= WindowScale;
		v *= WindowScale;
	}

	if (UseFullScreen)
#if MayFullScreen
	{
		h += hOffset;
		v += vOffset;
	}
#endif

	SDL_WarpMouseInWindow(main_wind, h, v);

	return true;
}
#endif

/* cursor state */

void MousePositionNotify(int NewMousePosh, int NewMousePosv)
{
	bool ShouldHaveCursorHidden = true;

	if (UseFullScreen)
#if MayFullScreen
	{
		NewMousePosh -= hOffset;
		NewMousePosv -= vOffset;
	}
#endif

	if (UseMagnify) {
		NewMousePosh /= WindowScale;
		NewMousePosv /= WindowScale;
	}

	if (UseFullScreen)
#if MayFullScreen
	{
		NewMousePosh += ViewHStart;
		NewMousePosv += ViewVStart;
	}
#endif

#if EnableFSMouseMotion
	if (HaveMouseMotion) {
		MousePositionSetDelta(NewMousePosh - SavedMouseH,
			NewMousePosv - SavedMouseV);
		SavedMouseH = NewMousePosh;
		SavedMouseV = NewMousePosv;
	} else
#endif
	{
		if (NewMousePosh < 0) {
			NewMousePosh = 0;
			ShouldHaveCursorHidden = false;
		} else if (NewMousePosh >= vMacScreenWidth) {
			NewMousePosh = vMacScreenWidth - 1;
			ShouldHaveCursorHidden = false;
		}
		if (NewMousePosv < 0) {
			NewMousePosv = 0;
			ShouldHaveCursorHidden = false;
		} else if (NewMousePosv >= vMacScreenHeight) {
			NewMousePosv = vMacScreenHeight - 1;
			ShouldHaveCursorHidden = false;
		}

		if (UseFullScreen)
#if MayFullScreen
		{
			ShouldHaveCursorHidden = true;
		}
#endif

		/* if (ShouldHaveCursorHidden || CurMouseButton) */
		/*
			for a game like arkanoid, would like mouse to still
			move even when outside window in one direction
		*/
		MousePositionSet(NewMousePosh, NewMousePosv);
	}

	WantCursorHidden = ShouldHaveCursorHidden;
}

#if EnableFSMouseMotion && ! HaveWorkingWarp
void MousePositionNotifyRelative(int deltah, int deltav)
{
	bool ShouldHaveCursorHidden = true;

	if (UseMagnify) {
		/*
			This is not really right. If only move one pixel
			each time, emulated mouse doesn't move at all.
		*/
		deltah /= WindowScale;
		deltav /= WindowScale;
	}

	MousePositionSetDelta(deltah,
		deltav);

	WantCursorHidden = ShouldHaveCursorHidden;
}
#endif

void CheckMouseState(void)
{
	/*
		this doesn't work as desired, doesn't get mouse movements
		when outside of our window.
	*/
	int x;
	int y;

	(void) SDL_GetMouseState(&x, &y);
	MousePositionNotify(x, y);
}
