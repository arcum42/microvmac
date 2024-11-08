#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <SDL.h>

#include "sound_sdl2.h"
#include "UI/my_os_glue.h"
#include "UTIL/param_buffers.h"

/* --- globals --- */

extern uint32_t TrueEmulatedTime;
extern uint32_t LastTime;
extern uint32_t NextIntTime;
extern uint32_t NextFracTime;

extern bool gBackgroundFlag;
extern bool gTrueBackgroundFlag;
extern bool CurSpeedStopped;

// Functions

void DoKeyCode(SDL_Keysym *r, bool down);
