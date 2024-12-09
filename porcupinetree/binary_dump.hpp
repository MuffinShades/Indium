#pragma once

#include "filewrite.hpp"

class BinDump {
private:
	static std::string d_loc;
	static size_t id;
public:
	static void SetDumpLocation(std::string loc);
	static void dump(byte* b, size_t len);
};