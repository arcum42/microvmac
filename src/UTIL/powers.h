#pragma once

#include "sys_dependencies.h"

static inline uimr PowOf2(uimr n)
{
	return ((uimr)1 << (n));
}

static inline uimr Pow2Mask(uimr n)
{
	return (PowOf2(n) - 1);
}

static inline uimr ModPow2(uimr n, uint8_t align)
{
	return ((n)&Pow2Mask(align));
}

static inline uimr FloorDivPow2(uimr n, uint8_t align)
{
	return ((n) >> (align));
}

static inline uimr FloorPow2Mult(uimr n, uint8_t align)
{
	return ((n) & (~Pow2Mask(align)));
}

static inline uimr CeilPow2Mult(uimr n, uint8_t align)
{
	return FloorPow2Mult((n) + Pow2Mask(align), (align));
}