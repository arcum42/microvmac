#include "sys_dependencies.h"
#include "HW/VIDCARD/video.h"
#include <SDL.h>

#ifndef GrabKeysFullScreen
#define GrabKeysFullScreen 1
#endif

#define EnableRecreateW 0

// video defines
#ifndef UseSDLscaling
#define UseSDLscaling 0
#endif

#if ! UseSDLscaling
#define MaxScale WindowScale
#else
#define MaxScale 1
#endif

#define CLUT_finalsz (256 * 8 * 4 * MaxScale)

extern uint8_t* CLUT_final;
extern uint8_t* screencomparebuff;

extern bool UseFullScreen;
extern bool UseMagnify;
extern bool UseColorMode;
extern bool ColorModeWorks;
extern bool ColorMappingChanged;

void ScreenClearChanges(void);
void ScreenChangedAll(void);

extern int16_t ScreenChangedTop;
extern int16_t ScreenChangedLeft;
extern int16_t ScreenChangedBottom;
extern int16_t ScreenChangedRight;

void Screen_OutputFrame(uint8_t * screencurrentbuff);

#if MayFullScreen
extern uint16_t ViewHSize;
extern uint16_t ViewVSize;
extern uint16_t ViewHStart;
extern uint16_t ViewVStart;
#endif

/* where emulated machine thinks mouse is */
extern uint16_t CurMouseV;
extern uint16_t CurMouseH;

extern SDL_Window *main_wind;
extern SDL_Renderer *renderer;
extern SDL_Texture *texture;
extern SDL_PixelFormat *format;

extern uint8_t * ScalingBuff;

extern bool HaveCursorHidden;
extern bool WantCursorHidden;

#if MayFullScreen
extern bool GrabMachine;
extern void GrabTheMachine();
extern void UngrabMachine();
#endif

extern bool ReCreateMainWindow();
extern void ZapWinStateVars();
extern bool CreateMainWindow();
extern void CloseMainWindow();
extern bool SDL_InitDisplay();
