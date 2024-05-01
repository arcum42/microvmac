/*
	CONTROL Mode
*/

#pragma once

#ifndef CONTROLM_H
#define CONTROLM_H

#include "sys_dependencies.h"
#include "error_codes.h"

/* Globals */

extern uimr SpecialModes;
extern bool NeedWholeScreenDraw;
extern uint8_t * CntrlDisplayBuff;

/* Public Functions */
void MacMsgOverride(const char *title, const char *msg);

#endif // CONTROLM_H
