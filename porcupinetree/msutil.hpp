#pragma once
#include "types.hpp"

/**
 * 
 * msutil.hpp
 * 
 * various generic utility things for le stuff
 * 
 * 
 */

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef clamp
#define clamp(v, n, x) max((n), min((v), (x)))
#endif

#ifndef inRange
#define inRange(v, n, x) ((v) > (n)) && ((v) < (x))
#endif

#ifndef ZeroMem
template<class _Ty> void ZeroMem(_Ty* buf, const size_t sz) {
	if (!buf) return;
	memset(buf, sz * sizeof(_Ty), 0);
}
#endif

#ifndef MAKE_MASK
#define MAKE_MASK(sz) ((1 << (sz)) - 1)
#endif