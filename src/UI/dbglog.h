
#include "sys_dependencies.h"
#include "UI/math.h"

/* --- sending debugging info to file --- */

#if dbglog_HAVE
#define dbglog_bufsz PowOf2(dbglog_buflnsz)
#define dbglog_open dbglog_open0

void dbglog_writeCStr(char *s);
void dbglog_writeReturn(void);
void dbglog_writeHex(uimr x);
void dbglog_writeNum(uimr x);
void dbglog_writeMacChar(uint8_t x);
#endif