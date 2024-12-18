#include "mp3.hpp"

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

	return {};
}