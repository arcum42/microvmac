
/* --- control mode and internationalization --- */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <SDL.h>

#include "sys_dependencies.h"
#include "UTIL/endian.h"
#include "UI/os_glue.h"
#include "LANG/intl_chars.h"

#ifndef CanGetAppPath
#define CanGetAppPath 1
#endif

[[maybe_unused]] static char *d_arg = nullptr;
[[maybe_unused]] static char *n_arg = nullptr;

#if CanGetAppPath
[[maybe_unused]] static char *app_parent = nullptr;
[[maybe_unused]] static char *pref_dir = nullptr;
#endif

#ifdef _WIN32
#define PathSep '\\'
#else
#define PathSep '/'
#endif

MacErr_t ChildPath(char *x, char *y, char **r)
{
	MacErr_t err = mnvm_miscErr;
	int nx = strlen(x);
	int ny = strlen(y);
	{
		if ((nx > 0) && (PathSep == x[nx - 1])) {
			--nx;
		}
		{
			int nr = nx + 1 + ny;
			char *p = (char*)malloc(nr + 1);
			if (p != nullptr) {
				char *p2 = p;
				(void) memcpy(p2, x, nx);
				p2 += nx;
				*p2++ = PathSep;
				(void) memcpy(p2, y, ny);
				p2 += ny;
				*p2 = 0;
				*r = p;
				err = mnvm_noErr;
			}
		}
	}

	return err;
}

void MayFree(char *p)
{
	if (nullptr != p) {
		free(p);
	}
}
 

/* --- text translation --- */

void NativeStrFromCStr(char *r, char *s)
{
	uint8_t ps[ClStrMaxLength];
	int i;
	int L;

	ClStrFromSubstCStr(&L, ps, s);

	for (i = 0; i < L; ++i) {
		r[i] = Cell2PlainAsciiMap[ps[i]];
	}

	r[L] = 0;
}
