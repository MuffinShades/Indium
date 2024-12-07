#pragma once
#include "types.hpp"


struct mp3_frame {
	u16 frame_sync; //11 bits
	byte version;
	byte layer_desc;

};