/*
	PROGMAIN.c

	Copyright (C) 2009 Bernd Schmidt, Philip Cummins, Paul C. Pratt

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
	The main program.
*/

#include "sys_dependencies.h"
#include "HW/hardware.h"

// os glue sdl2
extern void MainEventLoop(void);
extern void ZapOSGLUVars(void);
extern bool InitOSGLU(void);
extern void UnInitOSGLU(void);

int main(int argc, char **argv)
{
	ZapOSGLUVars();
	if (InitOSGLU())
	{
		/* Let's get ready to start */
		devices_setup();
		spdlog::debug("Spdlog initialized.");

		devices_init();
		MainEventLoop();
	}
	UnInitOSGLU();

	return 0;
}