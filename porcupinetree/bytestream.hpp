#pragma once
#include <iostream>
#include <cstring>
#include <cmath>
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

enum ByteStreamMode {
	bmode_LittleEndian,
	bmode_BigEndian
};

class ByteStream {
protected:
	byte* bytes = nullptr;
	size_t len = 0;
	size_t allocSz = 0;
	size_t readPos = 0, writePos = 0;
	size_t chunkSz = 0xfff;
public:
	int mode = bmode_BigEndian;
	MSFL_EXP void allocNewChunk();
	MSFL_EXP void allocBytes(size_t sz);
	MSFL_EXP ByteStream(byte* dat, size_t sz);
	MSFL_EXP ByteStream();
	MSFL_EXP size_t seek(size_t pos);

	//read functions
	MSFL_EXP virtual byte readByte();
	MSFL_EXP virtual unsigned long long readBytesAsVal(size_t nBytes);
	MSFL_EXP virtual short readInt16();
	MSFL_EXP virtual unsigned short readUInt16();
	MSFL_EXP virtual int readInt32();
	MSFL_EXP virtual unsigned int readUInt32();
	MSFL_EXP virtual int64_t readInt64();
	MSFL_EXP virtual uint64_t readUInt64();
	MSFL_EXP virtual byte* readBytes(size_t nBytes);

	MSFL_EXP byte curByte();
	MSFL_EXP byte _readByte();
	MSFL_EXP void _writeByte(byte b);

	//write functions
	MSFL_EXP virtual void writeByte(byte b);
	MSFL_EXP virtual void writeNBytesAsVal(unsigned long long v, size_t nBytes);
	MSFL_EXP virtual void writeInt16(short v);
	MSFL_EXP virtual void writeUInt16(unsigned short v);
	MSFL_EXP virtual void writeInt32(int v);
	MSFL_EXP virtual void writeUInt32(unsigned int v);
	MSFL_EXP virtual void writeInt64(int64_t v);
	MSFL_EXP virtual void writeUInt64(uint64_t v);
	MSFL_EXP virtual void writeBytes(byte* dat, size_t sz);
	
	//
	MSFL_EXP void catchUp();
	MSFL_EXP void byteWriteAdv();


	MSFL_EXP void clip();
	
	MSFL_EXP void free() {
		if (this->bytes != nullptr) delete[] this->bytes;
		this->bytes = nullptr;
	}
	MSFL_EXP ~ByteStream();
	MSFL_EXP size_t getSize() {
		return this->len;
	};
	MSFL_EXP size_t getAllocSize() {
		return this->allocSz;
	};
	MSFL_EXP byte* getBytePtr() {
		return this->bytes;
	};
	MSFL_EXP size_t tell() {
		return this->readPos;
	};
	MSFL_EXP size_t skipBytes(size_t nBytes);
	MSFL_EXP char* readCStr(size_t len);
	MSFL_EXP std::string readStr(size_t len);
};

#ifdef MSFL_DLL
#ifdef __cplusplus
}
#endif
#endif