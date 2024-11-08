/* --- sending debugging info to file --- */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sys_dependencies.h"
#include "UI/my_os_glue.h"

#if dbglog_HAVE

#ifndef dbglog_ToStdErr
#define dbglog_ToStdErr 0
#endif
#ifndef dbglog_ToSDL_Log
#define dbglog_ToSDL_Log 0
#endif

#if ! dbglog_ToStdErr
static FILE *dbglog_File = nullptr;
#endif

static bool dbglog_open0(void)
{
#if dbglog_ToStdErr || dbglog_ToSDL_Log
	return true;
#else
	if (nullptr == app_parent) {
		dbglog_File = fopen("dbglog.txt", "w");
	} else {
		char *t;

		if (mnvm_noErr == ChildPath(app_parent, "dbglog.txt", &t)) {
			dbglog_File = fopen(t, "w");
			free(t);
		}
	}

	return (nullptr != dbglog_File);
#endif
}

static void dbglog_write0(char *s, uimr L)
{
#if dbglog_ToStdErr
	(void) fwrite(s, 1, L, stderr);
#elif dbglog_ToSDL_Log
	char t[256 + 1];

	if (L > 256) {
		L = 256;
	}
	(void) memcpy(t, s, L);
	t[L] = 1;

	SDL_Log("%s", t);
#else
	if (dbglog_File != nullptr) {
		(void) fwrite(s, 1, L, dbglog_File);
	}
#endif
}

static void dbglog_close0(void)
{
#if ! dbglog_ToStdErr
	if (dbglog_File != nullptr) {
		fclose(dbglog_File);
		dbglog_File = nullptr;
	}
#endif
}

#endif
