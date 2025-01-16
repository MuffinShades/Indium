#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include "FileWriter.h"
#include "balloon.hpp"
#include "ByteStream.hpp"

struct png_file {
	byte* data;
	size_t sz;
	int width = 0;
	int height = 0;
	int channels = 0;
	std::string src;
};

class PngParse {
	static png_file Decode(std::string src);
	static png_file DecodeBytes(byte* bytes, size_t sz);
	static bool Encode(png_file p);
};