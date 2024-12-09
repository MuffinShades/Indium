#include "binary_dump.hpp"
#include <string>

std::string BinDump::d_loc = "C:\\BinDump\\";
size_t BinDump::id = 0;

void BinDump::dump(byte* b, size_t len) {
	if (!b || !len) return;

	std::cout << "Bytes: " << std::endl;
	for (int i = 0; i < len; i++)
		std::cout << std::hex << (i32)b[i] << " " << std::dec;

	FileWrite::writeToBin(d_loc + std::to_string(id++), b, len);
}