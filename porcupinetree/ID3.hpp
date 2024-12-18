#pragma once
#include "bitstream.hpp"
#include <iostream>
#include "msutil.hpp"

/**
 *
 * ID3.hpp
 * 
 * parses ID3 tags in mp3 files
 * right now just kinda extracts the lengths and
 * discard all the other info so not really a 
 * parser more like a remover
 * 
 */

struct ID3_tag {
	size_t tag_len;
	u32 version;
	byte flags;
};

const static byte ID_TAG_BUF[] = { 'I', 'D', '3' };

static ID3_tag extract_id3_data(BitStream* stream) {
	if (!stream) return {0};

	//make sure alignment is good
	stream->alignToPrevByte();

	const size_t re = stream->tell();

	byte* id3_str = stream->readBytes(3);

	//make sure ID3 tag is there
	if (!_bufCmp<byte>(id3_str, const_cast<byte*>(ID_TAG_BUF), 3)) {
		std::cout << "Error invalid ID3 sig! [" << id3_str << "]" << std::endl;
		stream->seek(re);
		delete[] id3_str;
		return { 0 };
	}

	//extract some of the data and skip the rest
	const u32 version = stream->readUInt16();

	if ((version & 0xff00) == 0xff00 || (version & 0x00ff) == 0xff) {
		std::cout << "Warning invalid ID3 tag version!" << std::endl;
	}

	const byte flags = stream->readByte();
	const size_t tag_sz = stream->readUInt32();

	stream->skipBytes(tag_sz);

	return {
		.tag_len = tag_sz,
		.version = version,
		.flags = flags
	};
}