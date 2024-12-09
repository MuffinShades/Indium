#pragma once
#include <iostream>
#include <fstream>
#include "msutil.hpp"

struct file {
	size_t len;
	byte* dat;
};

class FileWrite {
public:
	static bool writeToBin(std::string src, byte* dat, size_t sz);
	static file readFromBin(std::string src);
};