#include <iostream>
#include "h264.hpp"
#include "bitstream.hpp"
#include "binary_dump.hpp"
#include "mp3.hpp"

i32 main(void) {
	std::cout << "Muffin Media!" << std::endl;

	//BitStream testStream = BitStream();

	//testStream.writeByte(0x5d);
	//testStream.writeBit(1);
	//testStream.writeBit(0);
	//testStream.writeBit(1);
	//testStream.writeByte(0xff);
	//testStream.writeByte(0xff); //uh overflows somehow ;-;
	//testStream.writeByte(0xff);

	//std::cout << "Stream Info: " << testStream.getBytePtr() << " " << testStream.getSize() << std::endl;

	//BinDump::dump(testStream.getBytePtr(), testStream.getSize());

	mp3_audio test_mp3 = mp3::fileDecode("C:\\mpegtest\\audio2.mp3");

	return 0;
}