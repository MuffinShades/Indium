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
	memset(buf, 0, sz * sizeof(_Ty));
}
#endif

#ifndef MAKE_MASK
#define MAKE_MASK(sz) ((1 << (sz)) - 1)
#endif

template<class _Ty> static bool _bufCmp(_Ty *buf1, _Ty* buf2, size_t bSz) {
	if (!buf1 || !buf2) return false;

	for (size_t i = 0; i < bSz; i++)
		if (*buf1++ != *buf2++)
			return false;

	return true;
}