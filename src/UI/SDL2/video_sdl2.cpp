#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <SDL.h>
#include <SDL_pixels.h>

#include "sys_dependencies.h"

#include "UTIL/endian.h"
#include "HW/SCREEN/screen.h"

#include "video_sdl2.h"
#include "os_glue_sdl2.h"
#include "UI/my_os_glue.h"
#include "UI/event_queue.h"

/* --- video out --- */

#if MayFullScreen
int hOffset;
int vOffset;
#endif

// Eh?
bool UseFullScreen = (WantInitFullScreen != 0);
bool UseMagnify = (WantInitMagnify != 0);
bool WantFullScreen = (WantInitFullScreen != 0);
bool WantMagnify = (WantInitMagnify != 0);

bool gBackgroundFlag = false;
bool gTrueBackgroundFlag = false;
bool CurSpeedStopped = false;
SDL_Window *main_wind = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Texture *texture = nullptr;
SDL_PixelFormat *format = nullptr;

uint8_t *ScalingBuff = nullptr;

SDL_Color bwpalette[2];
bool bwpalette_loaded = false;
bool EmVideoDisable = false;

uint8_t* CLUT_final;
uint8_t* screencomparebuff = nullptr;

/* --- main window creation and disposal --- */

bool SDL_InitDisplay()
{
	bool v = false;

	InitKeyCodes();

	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
	{
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
	}
	else
	{
		v = true;
	}

	return v;
}

#if MayFullScreen
bool GrabMachine = false;

void GrabTheMachine()
{
#if GrabKeysFullScreen
	SDL_SetWindowGrab(main_wind, SDL_TRUE);
#endif

#if EnableFSMouseMotion

#if HaveWorkingWarp
	/*
		if magnification changes, need to reset,
		even if HaveMouseMotion already true
	*/
	if (MoveMouse(ViewHStart + (ViewHSize / 2),
				  ViewVStart + (ViewVSize / 2)))
	{
		SavedMouseH = ViewHStart + (ViewHSize / 2);
		SavedMouseV = ViewVStart + (ViewVSize / 2);
		HaveMouseMotion = true;
	}
#else
	if (0 == SDL_SetRelativeMouseMode(SDL_ENABLE))
	{
		HaveMouseMotion = true;
	}
#endif

#endif /* EnableFSMouseMotion */
}

void UngrabMachine()
{
#if EnableFSMouseMotion

	if (HaveMouseMotion)
	{
#if HaveWorkingWarp
		(void)MoveMouse(CurMouseH, CurMouseV);
#else
		SDL_SetRelativeMouseMode(SDL_DISABLE);
#endif

		HaveMouseMotion = false;
	}

#endif /* EnableFSMouseMotion */

#if GrabKeysFullScreen
	SDL_SetWindowGrab(main_wind, SDL_FALSE);
#endif
}
#endif

#if EnableFSMouseMotion && HaveWorkingWarp
static void MouseConstrain(void)
{
	int16_t shiftdh;
	int16_t shiftdv;

	if (SavedMouseH < ViewHStart + (ViewHSize / 4))
	{
		shiftdh = ViewHSize / 2;
	}
	else if (SavedMouseH > ViewHStart + ViewHSize - (ViewHSize / 4))
	{
		shiftdh = -ViewHSize / 2;
	}
	else
	{
		shiftdh = 0;
	}
	if (SavedMouseV < ViewVStart + (ViewVSize / 4))
	{
		shiftdv = ViewVSize / 2;
	}
	else if (SavedMouseV > ViewVStart + ViewVSize - (ViewVSize / 4))
	{
		shiftdv = -ViewVSize / 2;
	}
	else
	{
		shiftdv = 0;
	}
	if ((shiftdh != 0) || (shiftdv != 0))
	{
		SavedMouseH += shiftdh;
		SavedMouseV += shiftdv;
		if (!MoveMouse(SavedMouseH, SavedMouseV))
		{
			HaveMouseMotion = false;
		}
	}
}
#endif

// Set the display palette from the Macintosh's memory or w/e
static int SetPalette(SDL_Palette *palette, const SDL_Color *macColors, int ncolors)
{
	return SDL_SetPaletteColors(palette, macColors, 0, ncolors);
}

static SDL_Color HexToColor(const char *hexIn, SDL_Color fallback)
{
	unsigned char r, g, b;
	assert(hexIn != nullptr);
	int numRead = sscanf(hexIn, "#%02hhx%02hhx%02hhx", &r, &g, &b);
	if (numRead != 3)
	{
		return fallback;
	}
	SDL_Color result = {.r = r, .g = g, .b = b, .a = 255};
	return result;
}

void LoadCustomPalette()
{
	if (bwpalette_loaded)
	{
		return;
	}
	SDL_Color fallbacks[] = {{.r = 255, .g = 255, .b = 255, .a = 255}, {.r = 0, .g = 0, .b = 0, .a = 255}};
	bwpalette[0] = HexToColor(ScreenColorWhite, fallbacks[0]);
	bwpalette[1] = HexToColor(ScreenColorBlack, fallbacks[1]);
	bwpalette_loaded = true;
}

// Get pixel format for a given screen depth
// Note: this is complete and total guesswork right now. Lol.
uint32_t GetPixFormatFromDepth(int depth)
{
	switch (depth)
	{
	case 1:
		return SDL_PIXELFORMAT_INDEX1MSB;
	case 4:
		return SDL_PIXELFORMAT_INDEX4MSB;
	case 8:
		return SDL_PIXELFORMAT_INDEX8;
	case 16:
		return SDL_PIXELFORMAT_RGB332;
	case 24:
		return SDL_PIXELFORMAT_RGB24;
	case 32:
		return SDL_PIXELFORMAT_RGBA8888;
	default:
		return SDL_PIXELFORMAT_UNKNOWN;
	}
}

// We aren't being smart about *what* to update, since if we use SDL2 properly
// the update operating is stunningly cheap.
void Screen_OutputFrame(uint8_t *src_ptr)
{
	if (EmVideoDisable)
	{
		return;
	}

	uint32_t src_format = GetPixFormatFromDepth(vMacScreenDepth + 1);
	void *pixels;
	int pitch;

	// Setup source surface
	SDL_Surface *src = SDL_CreateRGBSurfaceWithFormatFrom(
		src_ptr,
		vMacScreenWidth,
		vMacScreenHeight,
		vMacScreenDepth + 1,
		vMacScreenByteWidth,
		src_format);
	LoadCustomPalette();
	SetPalette(src->format->palette, bwpalette, 2);

	// Setup dst surface
	SDL_LockTexture(texture, nullptr, &pixels, &pitch);
	SDL_Surface *dst = SDL_CreateRGBSurfaceWithFormatFrom(
		pixels,
		vMacScreenWidth,
		vMacScreenHeight,
		32, vMacScreenWidth * 4,
		SDL_PIXELFORMAT_RGBX8888);

	// Blit src to dst
	SDL_BlitSurface(src, nullptr, dst, nullptr);

	// Free surfaces
	SDL_FreeSurface(src);
	SDL_FreeSurface(dst);

	// Render the texture
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	SDL_UnlockTexture(texture);

	SDL_RenderPresent(renderer);
}

void DoneWithDrawingForTick(void)
{
	return;
}

enum
{
	kMagStateNormal,
	kMagStateMagnifgy,
	kNumMagStates
};

#define kMagStateAuto kNumMagStates

#if MayNotFullScreen
static int CurWinIndx;
static bool HavePositionWins[kNumMagStates];
static int WinPositionsX[kNumMagStates];
static int WinPositionsY[kNumMagStates];
#endif

bool CreateMainWindow()
{
	int NewWindowX;
	int NewWindowY;
	int NewWindowHeight = vMacScreenHeight;
	int NewWindowWidth = vMacScreenWidth;
	Uint32 flags = 0 /* SDL_WINDOW_HIDDEN */;
	bool v = false;

	if (UseFullScreen)
#if MayFullScreen
	{
		/*
			We don't want physical screen mode to be changed in modern
			displays, so we pass this _DESKTOP flag.
		*/
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

		NewWindowX = SDL_WINDOWPOS_UNDEFINED;
		NewWindowY = SDL_WINDOWPOS_UNDEFINED;
	}
#endif
#if MayNotFullScreen
	{
		int WinIndx;

		if (UseMagnify)
		{
			WinIndx = kMagStateMagnifgy;
		}
		else
		{
			WinIndx = kMagStateNormal;
		}

		if (!HavePositionWins[WinIndx])
		{
			NewWindowX = SDL_WINDOWPOS_CENTERED;
			NewWindowY = SDL_WINDOWPOS_CENTERED;
		}
		else
		{
			NewWindowX = WinPositionsX[WinIndx];
			NewWindowY = WinPositionsY[WinIndx];
		}

		CurWinIndx = WinIndx;
	}
#endif

	if (nullptr == (main_wind = SDL_CreateWindow(
						kStrAppName,
						NewWindowX, NewWindowY,
						NewWindowWidth, NewWindowHeight,
						flags)))
	{
		fprintf(stderr, "SDL_CreateWindow fails: %s\n", SDL_GetError());
	}
	else if (nullptr == (renderer = SDL_CreateRenderer(
							 main_wind, -1,
							 0 /* SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC */
							   /*
								   SDL_RENDERER_ACCELERATED not needed
								   "no flags gives priority to available
								   SDL_RENDERER_ACCELERATED renderers"
							   */
							   /* would rather not require vsync */
							 )))
	{
		fprintf(stderr, "SDL_CreateRenderer fails: %s\n", SDL_GetError());
	}
	else if (nullptr == (texture = SDL_CreateTexture(
							 renderer,
							 SDL_PIXELFORMAT_RGBX8888,
							 SDL_TEXTUREACCESS_STREAMING,
							 vMacScreenWidth, vMacScreenHeight)))
	{
		fprintf(stderr, "SDL_CreateTexture fails: %s\n", SDL_GetError());
	}
	else if (nullptr == (format = SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888)))
	{
		fprintf(stderr, "SDL_AllocFormat fails: %s\n", SDL_GetError());
	}
	else
	{
		SDL_RenderClear(renderer);

		SDL_DisplayMode info;

		if (0 != SDL_GetCurrentDisplayMode(0, &info))
		{
			fprintf(stderr, "SDL_GetCurrentDisplayMode fails: %s\n",
					SDL_GetError());

			return false;
		}

		if (UseFullScreen)
#if MayFullScreen
		{
			int wr;
			int hr;

			SDL_GL_GetDrawableSize(main_wind, &wr, &hr);

			ViewHSize = wr;
			ViewVSize = hr;
			if (UseMagnify)
			{
				ViewHSize /= WindowScale;
				ViewVSize /= WindowScale;
			}
			if (ViewHSize >= vMacScreenWidth)
			{
				ViewHStart = 0;
				ViewHSize = vMacScreenWidth;
			}
			else
			{
				ViewHSize &= ~1;
			}
			if (ViewVSize >= vMacScreenHeight)
			{
				ViewVStart = 0;
				ViewVSize = vMacScreenHeight;
			}
			else
			{
				ViewVSize &= ~1;
			}

			if (wr > NewWindowWidth)
			{
				hOffset = (wr - NewWindowWidth) / 2;
			}
			else
			{
				hOffset = 0;
			}
			if (hr > NewWindowHeight)
			{
				vOffset = (hr - NewWindowHeight) / 2;
			}
			else
			{
				vOffset = 0;
			}
		}
#endif

#if 0 != vMacScreenDepth
		ColorModeWorks = true;
#endif

		v = true;
	}

	return v;
}

void CloseMainWindow()
{
	if (nullptr != format)
	{
		SDL_FreeFormat(format);
		format = nullptr;
	}

	if (nullptr != texture)
	{
		SDL_DestroyTexture(texture);
		texture = nullptr;
	}

	if (nullptr != renderer)
	{
		SDL_DestroyRenderer(renderer);
		renderer = nullptr;
	}

	if (nullptr != main_wind)
	{
		SDL_DestroyWindow(main_wind);
		main_wind = nullptr;
	}
}

#if EnableRecreateW
static void ZapWState(void)
{
	main_wind = nullptr;
	renderer = nullptr;
	texture = nullptr;
	format = nullptr;
}
#endif

#if EnableRecreateW
struct WState
{
#if MayFullScreen
	uint16_t f_ViewHSize;
	uint16_t f_ViewVSize;
	uint16_t f_ViewHStart;
	uint16_t f_ViewVStart;
	int f_hOffset;
	int f_vOffset;
#endif
	bool f_UseFullScreen;
	bool f_UseMagnify;
#if MayNotFullScreen
	int f_CurWinIndx;
#endif
	SDL_Window *f_main_wind;
	SDL_Renderer *f_renderer;
	SDL_Texture *f_texture;
	SDL_PixelFormat *f_format;
};
typedef struct WState WState;
#endif

#if EnableRecreateW
static void GetWState(WState *r)
{
#if MayFullScreen
	r->f_ViewHSize = ViewHSize;
	r->f_ViewVSize = ViewVSize;
	r->f_ViewHStart = ViewHStart;
	r->f_ViewVStart = ViewVStart;
	r->f_hOffset = hOffset;
	r->f_vOffset = vOffset;
#endif
	r->f_UseFullScreen = UseFullScreen;
	r->f_UseMagnify = UseMagnify;
#if MayNotFullScreen
	r->f_CurWinIndx = CurWinIndx;
#endif
	r->f_main_wind = main_wind;
	r->f_renderer = renderer;
	r->f_texture = texture;
	r->f_format = format;
}
#endif

#if EnableRecreateW
static void SetWState(WState *r)
{
#if MayFullScreen
	ViewHSize = r->f_ViewHSize;
	ViewVSize = r->f_ViewVSize;
	ViewHStart = r->f_ViewHStart;
	ViewVStart = r->f_ViewVStart;
	hOffset = r->f_hOffset;
	vOffset = r->f_vOffset;
#endif
	UseFullScreen = r->f_UseFullScreen;
	UseMagnify = r->f_UseMagnify;
#if MayNotFullScreen
	CurWinIndx = r->f_CurWinIndx;
#endif
	main_wind = r->f_main_wind;
	renderer = r->f_renderer;
	texture = r->f_texture;
	format = r->f_format;
}
#endif

enum
{
	kWinStateWindowed,
	kWinStateFullScreen,
	kNumWinStates
};

static int WinMagStates[kNumWinStates];

#if EnableRecreateW
bool ReCreateMainWindow()
{
	WState old_state;
	WState new_state;
#if HaveWorkingWarp
	bool HadCursorHidden = HaveCursorHidden;
#endif
	int OldWinState =
		UseFullScreen ? kWinStateFullScreen : kWinStateWindowed;
	int OldMagState =
		UseMagnify ? kMagStateMagnifgy : kMagStateNormal;

	WinMagStates[OldWinState] =
		OldMagState;

	if (!UseFullScreen)
#if MayNotFullScreen
	{
		SDL_GetWindowPosition(main_wind,
							  &WinPositionsX[CurWinIndx],
							  &WinPositionsY[CurWinIndx]);
		HavePositionWins[CurWinIndx] = true;
	}
#endif

	ForceShowCursor(); /* hide/show cursor api is per window */

#if MayFullScreen
	if (GrabMachine)
	{
		GrabMachine = false;
		UngrabMachine();
	}
#endif

	GetWState(&old_state);

	ZapWState();

	UseMagnify = WantMagnify;
	UseFullScreen = WantFullScreen;

	if (!CreateMainWindow())
	{
		CloseMainWindow();
		SetWState(&old_state);

		/* avoid retry */
		WantFullScreen = UseFullScreen;
		WantMagnify = UseMagnify;
	}
	else
	{
		GetWState(&new_state);
		SetWState(&old_state);
		CloseMainWindow();
		SetWState(&new_state);

#if HaveWorkingWarp
		if (HadCursorHidden)
		{
			(void)MoveMouse(CurMouseH, CurMouseV);
		}
#endif
	}

	return true;
}
#endif

void ZapWinStateVars(void)
{
#if MayNotFullScreen
	{
		for (int i = 0; i < kNumMagStates; ++i)
		{
			HavePositionWins[i] = false;
		}
	}
#endif
	{
		for (int i = 0; i < kNumWinStates; ++i)
		{
			WinMagStates[i] = kMagStateAuto;
		}
	}
}

void ToggleWantFullScreen(void)
{
	WantFullScreen = !WantFullScreen;

	int OldWinState =
		UseFullScreen ? kWinStateFullScreen : kWinStateWindowed;
	int OldMagState =
		UseMagnify ? kMagStateMagnifgy : kMagStateNormal;
	int NewWinState =
		WantFullScreen ? kWinStateFullScreen : kWinStateWindowed;
	int NewMagState = WinMagStates[NewWinState];

	WinMagStates[OldWinState] = OldMagState;
	if (kMagStateAuto != NewMagState)
	{
		WantMagnify = (kMagStateMagnifgy == NewMagState);
	}
	else
	{
		WantMagnify = false;
		if (WantFullScreen)
		{
			SDL_Rect r;

			if (0 == SDL_GetDisplayBounds(0, &r))
			{
				if ((r.w >= vMacScreenWidth * WindowScale) && (r.h >= vMacScreenHeight * WindowScale))
				{
					WantMagnify = true;
				}
			}
		}
	}
}
