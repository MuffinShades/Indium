#pragma once
#include "msutil.hpp"
#include "ID3.hpp"
#include <iostream>
#include "filewrite.hpp"

struct mp3_frame_header {
	byte version;
	byte layer_dec;
};

struct mp3_frame {
	const u16 frame_sync; //11 bits
	mp3_frame_header header;
};

struct mp3_audio {
	ID3_tag id3;
	mp3_frame* frames;
	size_t nFrames;
};

class mp3 {
public:
	static mp3_audio fileDecode(std::string src);
};