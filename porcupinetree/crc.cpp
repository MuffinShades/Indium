#include "crc.hpp"
#include "msutil.hpp"

u32 crc32(byte* dat, size_t sz) {
	if (!dat || sz <= 0)
		return 0;

	u32 checksum = 0xffffffff;

	foreach_ptr(byte, b, dat, sz)
		checksum = crc_poly_32[(checksum ^ *b) & 0xff] ^ (checksum >> 8);

	return checksum ^ 0xffffffff;
}