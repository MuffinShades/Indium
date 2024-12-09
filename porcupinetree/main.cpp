#include <iostream>
#include "h264.hpp"
#include "bitstream.hpp"
#include "binary_dump.hpp"

i32 main(void) {
	std::cout << "Muffin Media!" << std::endl;

	BitStream testStream = BitStream();

	testStream.writeByte(0x5d);
	testStream.writeBit(1);

	std::cout << "Stream Info: " << testStream.getBytePtr() << " " << testStream.getSize() << std::endl;

	BinDump::dump(testStream.getBytePtr(), testStream.getSize());

	return 0;
}