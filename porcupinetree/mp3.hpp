#pragma once
#include "msutil.hpp"
#include "ID3.hpp"
#include <iostream>
#include "filewrite.hpp"

enum mp3_audio_mode {
	mp3_audio_single_channel,
	mp3_audio_dual_channel,
	mp3_audio_joint_stereo,
	mp3_audio_stereo
};

enum mp3_layer {
	mp3_layer_reserved,
	mp3_layer3,
	mp3_layer2,
	mp3_layer1
};

struct mp3_frame_header {
	const u16 frame_sync; //11 bits
	byte mp_version;
	mp3_layer mp_layer;
	bool crc_protect = 0;
	u32 bitRate = 0, freq = 0;
	mp3_audio_mode audit_mode;
	bool copy;
	bool copyright;
	byte emphasis;
	size_t frameLength;
};

struct mp3_si_gran {
	size_t part2_3_len;
	u32 big_vals;
	u32 global_gain;
	u32 scale_fac_compress;
	bool win_switch;
};

struct mp3_si_gran_dual {
	mp3_si_gran l, r;
};

struct mp3_side_info {
	size_t main_data_beg;
	u32 private_bits;
	u32 scfsi;
};

struct mp3_frame {
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