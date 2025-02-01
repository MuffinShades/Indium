#include "msutil.hpp"
#include "png.hpp"
#include "filewrite.hpp"
#include "bitmap.hpp"
#include "bitstream.hpp"

constexpr u64 sig[] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
constexpr size_t sigSz = 8;

enum chunk_type {
	NULL_CHUNK = 0x0,
	IHDR = 0x49484452,
	IDAT = 0x49444154,
	IEND = 0x49454e44,
	sRGB = 0x73524742,
	gAMA = 0x67414d41,
	PLTE = 0x504c5445
};

struct png_chunk {
	size_t len;
	chunk_type type;
	byte* dat;
	u32 checksum;
	bool good = false;
};

struct _IDAT {
	byte* dat;
	size_t sz;
};

struct _IHDR {
	size_t w, h;
	byte bitDepth;
	Png_ColorSpace colorSpace;
	byte compressionMethod = 0;
	byte filterType = 0;
	bool interlaced;
	size_t bytesPerPixel, nChannels, bpp;
};

chunk_type int_to_cType(int ty) {
	switch (ty) {
	case IHDR:
		return IHDR;
	case IDAT:
		return IDAT;
	case IEND: 
		return IEND;
	case sRGB: 
		return sRGB;
	case PLTE:
		return PLTE;
	case gAMA:
		return gAMA;
	}

	return NULL_CHUNK;
}

//read chunk
png_chunk readNextChunk(ByteStream* stream) {
	png_chunk res = {
		.len = stream->readUInt32()
	};
	i32 iTy = stream->readUInt32();
	res.type = int_to_cType(iTy);

	//std::cout << "Chunk Read: " << res.type << " " << res.len << std::endl;

	if (res.len > 0)
		res.dat = new byte[res.len];
	else {
		res.dat = nullptr;
		return res;
	}

	ZeroMem(res.dat, res.len);
	
	for (size_t i = 0; i < res.len; i++)
		res.dat[i] = stream->readByte();

	res.checksum = stream->readUInt32();
	res.good = true;
	return res;
};

#include "binary_dump.hpp"

_IDAT ProcessIDAT(png_chunk c) {
	//inflate everything
	BinDump::dump(c.dat, c.len);
	balloon_result decompressed_dat = Balloon::Inflate(c.dat, c.len);
	
	return {
		.dat = decompressed_dat.data,
		.sz = decompressed_dat.sz
	};
}

size_t GetNChannelsFromColorSpace(Png_ColorSpace color) {
	switch (color) {
	case Png_Color_GrayScale:
		return 1;
	case Png_Color_GrayScale_Alpha:
		return 2;
	case Png_Color_RGB:
		return 3;
	case Png_Color_RGBA:
		return 4;
	case Png_Color_Indexed:
		return 1;
	default:
		return 0;
	}
}

_IHDR ProcessIDHR(png_chunk hChunk) {
	if (!hChunk.good || !hChunk.dat || hChunk.len <= 0)
		return {0};

	ByteStream h_stream = ByteStream(hChunk.dat, hChunk.len);

	_IHDR header = {
		.w = h_stream.readUInt32(),
		.h = h_stream.readUInt32(),
		.bitDepth = h_stream.readByte(),
		.colorSpace = (Png_ColorSpace) h_stream.readByte(),
		.compressionMethod = h_stream.readByte(),
		.filterType	= h_stream.readByte(),
		.interlaced = (bool) (h_stream.readByte() & 1)
	};

	if (header.compressionMethod != 0 || header.filterType != 0)
		return { 0 };

	//set values of bytesPerPixel, bpp, and nChannels
	header.bytesPerPixel = 
		max(1,(header.bpp=(header.bitDepth * 
		(header.nChannels=GetNChannelsFromColorSpace(header.colorSpace)))) >> 3);

	return header;
}

//defilter instance used for defiltering step
struct defilter_inst {
	size_t scanY = 0, scanX = 0, p = 0;
	const size_t bytesPerScan;
	const _IHDR *hdr = nullptr;
	byte* stream = nullptr, *oStream = nullptr, *pStream = nullptr, *end = nullptr, *o_end = nullptr;
};

//helper function(s) for defilter inst
byte defilter_inst_next(defilter_inst* inst) {
	if (inst->stream + inst->p >= inst->end)
		return 0;
	return inst->stream[inst->p++];
}

byte defilter_inst_cur(defilter_inst* inst) {
	if (inst->stream + inst->p >= inst->end)
		return 0;
	return inst->stream[inst->p];
}

//gets a or value to left
byte defilter_getLeft(defilter_inst* inst) {
	if (!inst || inst->scanX < inst->hdr->bytesPerPixel || !inst->pStream) return 0;
	return *(inst->pStream + (inst->p - inst->hdr->bytesPerPixel - (inst->scanY+1))); //subtract scanY+1 to account for the 1 byte offset the output buffer length is from the raw data buffer each line (from filter type)
}

//gets b or value top
byte defilter_getTop(defilter_inst* inst) {
	if (!inst || inst->scanY <= 0 || !inst->pStream) return 0;
	return *(inst->pStream + (inst->p - inst->bytesPerScan - (inst->scanY+1)));
}

//gets c or value top left
byte defilter_getTopLeft(defilter_inst* inst) {
	if (!inst || inst->scanX < inst->hdr->bytesPerPixel || inst->scanY <= 0  || !inst->pStream) return 0;
	return *(inst->pStream + (inst->p - inst->bytesPerScan - inst->hdr->bytesPerPixel - (inst->scanY+1)));
}

#include <cmath>

//paeth preditionct
byte paeth_predict(defilter_inst* inst) {
	if (!inst) return 0;
	const u32 a = defilter_getLeft(inst), b = defilter_getTop(inst), c = defilter_getTopLeft(inst);
	i32
		p = a + b - c,
		pa = (i32)abs((i64)p - a),
		pb = (i32)abs((i64)p - b),
		pc = (i32)abs((i64)p - c);

	return min(a, min(b, c));
}

//defilter methods
class DefilterMethod {
public: 
	//sub defilter
	static void sub(defilter_inst* inst) {
		if (!inst)
			return;
		//std::cout << (i32)defilter_inst_cur(inst) << " " << (i32)defilter_getLeft(inst) << " -> " << (i32)((i32)defilter_inst_cur(inst) + (i32)defilter_getLeft(inst)) << " | " << inst->p << std::endl;
		*inst->oStream++ = (defilter_inst_cur(inst) + defilter_getLeft(inst)) & 0xff;
	};
	//up defilter
	static void up(defilter_inst* inst) {
		if (!inst)
			return;
		*inst->oStream++ = (defilter_inst_cur(inst) + defilter_getTop(inst)) & 0xff;
	};
	//avg defilter
	static void avg(defilter_inst* inst) {
		if (!inst)
			return;
		const i32 avg = (defilter_getLeft(inst) + defilter_getTop(inst)) >> 1;
		*inst->oStream++ = (defilter_inst_cur(inst) + avg) & 0xff;
	};
	//paeth defilter, he
	static void paeth(defilter_inst* inst) {
		if (!inst)
			return;
		*inst->oStream++ = (defilter_inst_cur(inst) + paeth_predict(inst)) & 0xff;
	};
};

//converts the bgr colors to rgb since png encodes them as bgr for some reason
void bgr_fix(byte* dat, size_t sz, size_t nChannels) {
	byte* pix = dat, *end = dat + sz;

	do {
		std::swap(*pix, *(pix + 2)); //swap b and r
	} while ((pix += nChannels) < end);
}

//fixes flip and bgr issue
byte* defilterCleanUp(byte *dat, size_t sz, _IHDR* hdr) {
	if (!dat || sz <= 0 || !hdr)
		return nullptr;

	//first fix bgr -> rgb
	if (hdr->colorSpace == Png_Color_RGB || hdr->colorSpace == Png_Color_RGBA)
		switch (hdr->colorSpace) {
		case Png_Color_RGB:
			bgr_fix(dat, sz, 3);
			break;
		case Png_Color_RGBA:
			bgr_fix(dat, sz, 4);
			break;
		}

	//next flip image
	byte* flipped = new byte[sz];
	ZeroMem(flipped, sz);

	const size_t scanSz = hdr->w * hdr->bytesPerPixel;
	for (size_t scan = 0, p = 0; scan < hdr->h; scan++, p += scanSz)
		memcpy(flipped + p, dat + (sz - (p + scanSz)), scanSz);

	_safe_free_a(dat);

	//
	return flipped;
}

//defilter the png data
byte* defilterDat(byte* i_dat, const size_t datSz, _IHDR *hdr) {
	BinDump::dump(i_dat, datSz);
	if (!hdr || !i_dat || datSz <= 0) return nullptr;

	if (hdr->colorSpace == Png_Color_Indexed) {
		std::cout << "Error, png color indexing not supported!" << std::endl;
		return nullptr;
	}

	if (hdr->bytesPerPixel <= 0) {
		std::cout << "Error, too little bytes per pixel!" << std::endl;
		return nullptr;
	}

	//do nothing for now
	const size_t _byPerPix = hdr->bytesPerPixel;
	const size_t scanline = hdr->w * _byPerPix;

	size_t osz = datSz - hdr->h; // - hdr->h for each filter type bytes
	byte* out = new byte[osz];
	ZeroMem(out, osz);

	//create inst
	defilter_inst df_inst = {
		.bytesPerScan = scanline,
		.hdr = hdr,
		.stream = i_dat,
		.oStream = out,
		.pStream = out,
		.end = i_dat + datSz,
		.o_end = out + osz
	};

	//y scan
	for (; df_inst.scanY < hdr->h; df_inst.scanY++) {
		byte method = defilter_inst_next(&df_inst); //get defilter method
		std::cout << "Scan: " << df_inst.scanY << " / " << hdr->h << " " << (i32)method << std::endl;
		//x scan
		for (df_inst.scanX = 0; df_inst.scanX < hdr->w * hdr->bytesPerPixel; df_inst.scanX++) {
			switch (method) {

			//none
			case 0:
				*df_inst.oStream++ = defilter_inst_cur(&df_inst);
				break;
			//sub
			case 1: 
				DefilterMethod::sub(&df_inst);
				break;
			//add
			case 2:
				DefilterMethod::up(&df_inst);
				break;
			//avg
			case 3:
				DefilterMethod::avg(&df_inst);
				break;
			//paeth
			case 4:
				DefilterMethod::paeth(&df_inst);
				break;
			default:
				df_inst.p += df_inst.bytesPerScan;
				df_inst.scanY++;
				df_inst.scanX = 0;
				method = defilter_inst_next(&df_inst);
				continue;
			}
			df_inst.p++;
		}
	}

	if (i_dat)
		delete[] i_dat;
	//return defilterCleanUp(out, osz, hdr);
	return out;
};

//decode a file
png_file PngParse::Decode(std::string src) {
	png_file rs;
	if (src == "" || src.length() <= 0)
		return rs;
	file fDat = FileWrite::readFromBin(src);

	//error check
	if (!fDat.dat)
		return rs;
	rs = PngParse::DecodeBytes(fDat.dat, fDat.len);
	delete[] fDat.dat;
	return rs;
}

//to free png chunk
void free_png_chunk(png_chunk* p) {
	if (p && p->dat) {
		delete[] p->dat;
		p->dat = nullptr;
		p->len = 0;
		p->checksum = 0;
		p->type = NULL_CHUNK;
	}
};

#define MSFL_PNG_DEBUG

png_file PngParse::DecodeBytes(byte* bytes, size_t sz) {

	ByteStream stream = ByteStream(bytes, sz);
	stream.mode = bmode_BigEndian; //set stream mode to big endian since that's what pngs use

	png_file rpng;

	//get for signature
	for (auto sig_byte : sig)
		if (stream.readByte() != sig_byte) {
			std::cout << "Error invalid png sig!" << std::endl;
			return rpng;
		}

	//read in header chunk
	png_chunk headChunk = readNextChunk(&stream);

	if (headChunk.type != IHDR) {
		std::cout << "[png error] first chunk was not a header chunk!" << std::endl;
		return rpng;
	}

	//parse header chunk
	_IHDR png_header = ProcessIDHR(headChunk);

#ifdef MSFL_PNG_DEBUG
	std::cout << "Png Header: \n\tWidth: " << png_header.w << "\n\tHeight: " << png_header.h << "\n\tBit Depth: " << png_header.bitDepth << "\n\tColor Space" << png_header.colorSpace << "\n\tFilter Method: " << png_header.filterType << "\n\tInterlacing: " << png_header.interlaced << std::endl;
#endif

	free_png_chunk(&headChunk);

	//parse other chunks till first IDat chunk
	bool idat_found = false;
	png_chunk Idat1;
	for (;;) {
		png_chunk dat = readNextChunk(&stream);

		switch (dat.type) {
		case IHDR: break;
		case sRGB: { //eh idk whats in this chunk anyway
			break;
		}
		case IDAT: {
			idat_found = true;
			Idat1 = dat;
			break;
		}
		default: {
			if (dat.len > 0)
				free_png_chunk(&dat);
			break;
		}
		}

		if (idat_found)
			break;

		if (dat.type != NULL_CHUNK)
			free_png_chunk(&dat);

		if (stream.tell() >= stream.getSize() || dat.type == IEND) break;
	}

	//collect all the idat chunks
	byte* compressedIdata = nullptr; size_t compressedIdataSz = Idat1.len;

	std::vector<png_chunk> iData = { Idat1 };

	png_chunk curIdata = iData[0];

	bool extraChunks = true;

	for (;;) {
		curIdata = readNextChunk(&stream);

		switch (curIdata.type) {
		case IDAT:
			iData.push_back(curIdata);
			compressedIdataSz += curIdata.len;
			std::cout << "I data sz: " << curIdata.len << std::endl;
			break;
		case IEND:
			extraChunks = false;
			break;
		}

		if (curIdata.type == IEND || curIdata.type != IDAT) {
			free_png_chunk(&curIdata);
			break;
		}
	}

	//allocate and stitch togethera all i chunks
	//TODO: fix the concatination between blocks :3 also pix paeth
	compressedIdata = new byte[compressedIdataSz];
	ZeroMem(compressedIdata, compressedIdataSz);
	size_t curCopy = 0;

	for (const png_chunk& idatChunk : iData) {
		size_t chunkLen;
		std::cout << "IDAT Copy: " << curCopy << std::endl;
		memcpy(compressedIdata + curCopy, idatChunk.dat, chunkLen = idatChunk.len);
		free_png_chunk(const_cast<png_chunk*>(&idatChunk));

		curCopy += chunkLen;

		if (curCopy >= compressedIdataSz)
			break;
	}
	
	//decompress all the idata chunks
	size_t decodeDatSz = png_header.w * png_header.h * png_header.bytesPerPixel + png_header.h; // add png_header.h for the defilter addition stuff
	std::cout << "Expected Size: " << decodeDatSz << std::endl;
	byte* imgDat;
	size_t iDPos = 0;
	
	balloon_result rawImgData = Balloon::Inflate(compressedIdata, compressedIdataSz);
	_safe_free_a(compressedIdata);
	imgDat = rawImgData.data;
	decodeDatSz = rawImgData.sz;
	std::cout << "Actual Size: " << decodeDatSz << std::endl;

	//TODO: decode additional (non required) chunks
	if (extraChunks) {

	}

	//defilter the data
	std::cout << "Defiltering..." << std::endl;
	imgDat = defilterDat(imgDat, decodeDatSz, &png_header);

	//for testing just write data to a bitmap
	Bitmap testOut;
	testOut.header.w = png_header.w;
	testOut.header.h = png_header.h;
	testOut.header.fSz = png_header.w * png_header.h * png_header.bytesPerPixel;
	testOut.header.bitsPerPixel = png_header.bpp;
	testOut.data = imgDat;
	BitmapParse::WriteToFile("C:\\mpegtest\\testpngread.bmp", &testOut);

	return {
		.data = imgDat,
		.sz = png_header.w * png_header.h * png_header.bytesPerPixel,
		.width = png_header.w,
		.height = png_header.h,
		.channels = (i32) png_header.nChannels,
		.colorMode = png_header.colorSpace,
		.bitDepth = png_header.bitDepth
	};
}

bool PngParse::Encode(std::string src, png_file p) {
	if (src.length() <= 0 || src == "" || p.sz <= 0 || !p.data)
		return false;

	if (p.colorMode == Png_Color_Indexed) {
		std::cout << "Error! Png color indexing not supported yet!" << std::endl;
		return false;
	}

	/*_IHDR hdr = {
		.w = p.width,
		.h = p.height,
		.bitDepth = p.bitDepth,
		.colorSpace = p.colorMode,
		.compressionMethod = h_stream.readByte(),
		.filterType = h_stream.readByte(),
		.interlaced = (bool)(h_stream.readByte() & 1)
	};*/
}