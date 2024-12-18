#include "mp3.hpp"


// [mpeg_version][mpeg_layer][bitrate]
// bitrates are in kb/s
const u32 bitrate_convert[2][3][16] = {
	{
		{0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,0},
		{0,32,48,56,64,80,96,112,128,160,192,224,256,320,384,0},
		{0,32,40,48,56,64,80,96,112,128,160,192,224,256,0}
	},
	{
		{0,32,48,56,64,80,91,112,128,144,160,176,192,224,256,0},
		{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0},
		{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0}
	}
};

const u32 sample_freq_table[3][3] = {
	{44100, 48000, 32000}, //mpeg 1
	{22050, 24000, 16000}, //mpeg 2
	{11025, 12000, 8000}   //mpeg 2.5
};

const size_t layer_to_idx[3] = {2,1,0};

mp3_frame_header read_frame_header(BitStream* stream) {
	if (!stream) {
		std::cout << "Invalid mp3 stream!";
		return { 0 };
	}

	mp3_frame_header res = {
		.frame_sync = stream->readNBits(12)
	};

	if (res.frame_sync != 0b111111111111) {
		std::cout << "Warning invalid frame sync!" << std::endl;
	}

	//read in mpeg version
	res.mp_version = stream->readBit()+1;

	//layer
	res.mp_layer = (mp3_layer)stream->readNBits(2);

	if (res.mp_layer == 0) {
		std::cout << "Error invalid mp3 layer!" << std::endl;
		return { 0 };
	}

	//
	res.crc_protect = (bool)stream->readBit();

	//get bit rate
	//TODO: error check to make suer ranges are valid
	const size_t br_idx = stream->readNBits(4);
	res.bitRate = bitrate_convert[res.mp_version - 1][layer_to_idx[res.mp_layer]][br_idx];

	//get freq
	//TODO: same range error chcecking
	const size_t freq_idx = stream->readNBits(2);
	res.freq = sample_freq_table[res.mp_version - 1][freq_idx];

	bool padding = stream->readBit();

	const bit PRIVATE_BIT = stream->readBit(); //ignore this bit

	//TODO: add rest of header reading code
	//mp3 decoder paper thingy page 21-22
}

mp3_audio mp3::fileDecode(std::string src) {
	if (src.length() <= 0) return {};

	//construct bitstream
	file f = FileWrite::readFromBin(src);

	if (f.len <= 0 || !f.dat) {
		std::cout << "Error could not find mp3 file: " << src << std::endl;
		return {};
	}

	std::cout << "mp3 len: " << f.len << std::endl;

	BitStream stream = BitStream(f.dat, f.len);

	//read id3 tag
	const ID3_tag _id3_tag = extract_id3_data(&stream);

	std::cout << "--ID3 Read--" << std::endl;
	std::cout << _id3_tag.version << std::endl;
	std::cout << _id3_tag.tag_len << std::endl;
	std::cout << _id3_tag.flags << std::endl;
	std::cout << "------------" << std::endl;

	//now start frame extraction

	return {};
}