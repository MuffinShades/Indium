#include <pch.h>

#include "png.hpp"
#include "balloon.hpp"

const u64 sig[] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
const size_t sigSz = 8;

enum chunk_type {
	NULL_CHUNK = 0x0,
	IDHR = 0x49484452,
	IDAT = 0x49444152,
	IEND = 0x49454e44
};

struct png_chunk {
	size_t len;
	chunk_type type;
	byte* dat;
	u32 checksum;
};

chunk_type int_to_cType(int ty) {
	switch (ty) {
	case IDHR: {
		return IDHR;
	}
	case IDAT: {
		return IDAT;
	}
	case IEND: {
		return IEND;
	}
	}

	return NULL_CHUNK;
}

//read chunk
png_chunk readNextChunk(ByteStream& stream) {
	png_chunk res;
	res.len = stream.readInt32();
	res.type = int_to_cType(stream.readInt32());

	if (res.type == NULL_CHUNK) {
		std::cout << "Error invalid png chunk!" << std::endl;
		return res;
	}

	res.dat = new byte[res.len];

	for (int i = 0; i < res.len; i++)
		res.dat[i] = stream.readByte();

	res.checksum = stream.readInt32();
	return res;
};

balloon_result DecodeIDHR(png_chunk c) {

	//inflate everything
	return Balloon::Inflate(c.dat, c.len);
}

/*png_file PngParse::Decode(std::string src) {

}

png_file PngParse::DecodeBytes(byte* bytes, size_t sz) {
	ByteStream stream = ByteStream(bytes, sz);
}

bool PngParse::Encode(png_file p) {

}*/