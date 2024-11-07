#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <SDL.h>
#include "UI/my_os_glue.h"

/* --- defines and macros and such --- */

#include "sound_sdl2.h"
#include "UTIL/param_buffers.h"

/* --- globals --- */

#if MayFullScreen
extern int hOffset;
extern int vOffset;
#endif
extern bool UseFullScreen;
extern bool UseMagnify;

extern uint32_t TrueEmulatedTime;
extern uint32_t LastTime;
extern uint32_t NextIntTime;
extern uint32_t NextFracTime;

extern bool gBackgroundFlag;
extern bool gTrueBackgroundFlag;
extern bool CurSpeedStopped;

extern SDL_Window *main_wind;
extern SDL_Renderer *renderer;
extern SDL_Texture *texture;
extern SDL_PixelFormat *format;

extern uint8_t * ScalingBuff;

extern bool HaveCursorHidden;
extern bool WantCursorHidden;

// Functions

void DoKeyCode(SDL_Keysym *r, bool down);
