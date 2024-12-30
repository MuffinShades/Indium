#pragma once
#include "types.hpp"

struct RawVideoData {

};

class h264 {
public:
	RawVideoData ExtractRawVideoDataFromFile(std::string src);
};