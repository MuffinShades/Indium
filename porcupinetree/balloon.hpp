#pragma once
/**
 *
 * BALLOON - C++ lightweight zlib implementation
 *
 * Version 1.0 written May 2024 [Old]
 * Version 2.0 written September - December 2024 [New]
 *
 * Program written by muffinshades
 *
 * Copyright (c) 2024 muffinshades
 *
 * You can do what ever you want with the software but you must
 * credit the author (muffinshades) with the original creation
 * of the software. Idk what else to put here lmao.
 *
 * Balloon Notes:
 *
 * This library is a implementation of the zlib or Inflate / Deflate
 * compression algorithm.
 *
 * Right now compression speeds are around 4-6mb/s and decompression
 * speeds are much faster. This isn't the worlds fastest implementation,
 * but its decently fast and lightweight. One day I will improve the LZ77
 * hash functions, but for now it's gonna stay at around 5mb/s.
 *
 * This program should be able to function with any other inflate / deflate
 * implementation apart from the whole compression level calculations being
 * different since I didn't entirley implement lazy and good matches into
 * the lz77 functions. I also didn't add a whole fast version for everything
 * since this is a relativley light weight library. One day I do plan on adding
 * these functions and making a even better implementation of zlib.
 *
 */

#include <iostream>
#include <cstring>
#include "msutil.hpp"

#ifdef MSFL_DLL
#ifdef MSFL_EXPORTS
#define MSFL_EXP __declspec(dllexport)
#else
#define MSFL_EXP __declspec(dllimport)
#endif
#else
#define MSFL_EXP
#endif

#ifdef MSFL_DLL
#ifdef __cplusplus
extern "C" {
#endif
#endif

 //bit stream class for reading and doing stuff with bits
class BalloonStream {
public:
	byte* bytes = nullptr;
	i32 pos = 0, rPos = 0;
	i32 lBit = 0;
	u32 cByte = 0;
	u32 rBit = 0;
	size_t sz, bsz, asz;
	//bit stream for reading existing bytes / bits
	MSFL_EXP BalloonStream(byte* bytes, size_t len);
	//basic zero allocation bit stream for writing
	MSFL_EXP BalloonStream(size_t len);
	MSFL_EXP i32 readBit();
	MSFL_EXP i32 readByte();
	MSFL_EXP i32 readNBits(i32 nBits);
	MSFL_EXP i32 readValue(size_t size);
	MSFL_EXP static u32* bitsToBytes(u32* bits, size_t len);
	MSFL_EXP static u32* bitsToBytes(u32 bits, size_t len);
	MSFL_EXP void checkWritePosition();
	MSFL_EXP void writeBit(u32 bit);
	//write multiple bits
	MSFL_EXP void writeNBits(u32* bits, size_t len);
	//write short, long, int, uint, etc.
	MSFL_EXP void writeValue(byte val);
	MSFL_EXP void writeValue(short val);
	MSFL_EXP void writeValue(i32 val);
	MSFL_EXP void writeValue(u32 val);
	MSFL_EXP void writeValue(i64 val);
	MSFL_EXP void writeValue(u64 val);
	//allocate new bit chunk
	MSFL_EXP void allocNewChunk();
	MSFL_EXP void seek(i32 pos);
	MSFL_EXP void seekw(i32 wPos);
	MSFL_EXP i32 tell();
	MSFL_EXP i32 tellw();
	//remove unused bytes
	MSFL_EXP void clip();
	//allocation function
	MSFL_EXP void calloc(size_t sz);
	MSFL_EXP void writeBytes(byte* dat, size_t nBytes);
};

struct balloon_result {
	byte* data;
	size_t sz;
	u32 checksum;
	byte compressionMethod;
};

#define BALLOON_MAX_THREADS_AUTO -1

class Balloon {
public:
	MSFL_EXP static balloon_result Deflate(byte* data, size_t sz, u32 compressionLevel = 2, const size_t winBits = 0xf);
	MSFL_EXP static balloon_result Inflate(byte* data, size_t sz);
	static bool DeflateFileToFile(std::string in_src, std::string out_src, u32 compressionLevel = 2, const size_t winBits = 0xf);
	static bool InflateFileToFile(std::string in_src, std::string out_src);
	static balloon_result MultiThreadDeflate(byte* data, size_t sz, i32 maxThreads = BALLOON_MAX_THREADS_AUTO, u32 compressionLevel = 2, const size_t winBits = 0xf);
	static balloon_result MultiThreadDeflate(byte* data, size_t sz, i32 maxThreads = BALLOON_MAX_THREADS_AUTO);
	static bool MultiThreadDeflateFileToFile(std::string in_src, std::string out_src, i32 maxThreads = BALLOON_MAX_THREADS_AUTO, u32 compressionLevel = 2, const size_t winBits = 0xf);
	static bool MultiThreadInflateFileToFile(std::string in_src, std::string out_src, i32 maxThreads = BALLOON_MAX_THREADS_AUTO);
	MSFL_EXP static void Free(balloon_result* res);
};

#ifdef MSFL_DLL
#ifdef __cplusplus
}
#endif
#endif