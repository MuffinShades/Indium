#include <pch.h>

#include "binary_dump.hpp"
#include <string>

std::string BinDump::d_loc = "C:\\BinDump\\";
size_t BinDump::id = 0;

void BinDump::dump(byte* b, size_t len) {
	if (!b || !len) return;

	FileWrite::writeToBin(d_loc + std::to_string(id++), b, len);
}