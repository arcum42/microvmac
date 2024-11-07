#include "sys_dependencies.h"

extern uimr ReserveAllocOffset;
extern uint8_t * ReserveAllocBigBlock;
extern void ReserveAllocOneBlock(uint8_t * *p, uimr n, uint8_t align, bool FillOnes);
extern void EmulationReserveAlloc(void);