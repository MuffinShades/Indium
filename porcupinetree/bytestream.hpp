#pragma once
#include <iostream>
#include <cstring>
#include <cmath>
#include "msutil.hpp"

enum ByteStreamMode {
	bmode_LittleEndian,
	bmode_BigEndian
};

class ByteStream {
private:
	byte* bytes = nullptr;
	size_t len = 0;
	size_t allocSz = 0;
	size_t readPos = 0;
	size_t chunkSz = 0xfff;
public:
	int mode = bmode_BigEndian;
	void allocNewChunk();
	void allocBytes(size_t sz);
	ByteStream(byte* dat, size_t sz);
	ByteStream();
	size_t seek(size_t pos);

	//read functions
	virtual byte readByte();
	virtual unsigned long long readBytesAsVal(size_t nBytes);
	virtual short readInt16();
	virtual unsigned short readUInt16();
	virtual int readInt32();
	virtual unsigned int readUInt32();
	virtual int64_t readInt64();
	virtual uint64_t readUInt64();

	//write functions
	virtual void writeByte(byte b);
	virtual void writeNBytesAsVal(unsigned long long v, size_t nBytes);
	virtual void writeInt16(short v);
	virtual void writeUInt16(unsigned short v);
	virtual void writeInt32(int v);
	virtual void writeUInt32(unsigned int v);
	virtual void writeInt64(int64_t v);
	virtual void writeUInt64(uint64_t v);
	virtual void writeBytes(byte* dat, size_t sz);


	void clip();
	
	void free() {
		if (this->bytes != nullptr) delete[] this->bytes;
		this->bytes = nullptr;
	}
	~ByteStream();
	size_t getSize() {
		return this->len;
	};
	size_t getAllocSize() {
		return this->allocSz;
	};
	byte* getBytePtr() {
		return this->bytes;
	};
	size_t tell() {
		return this->readPos;
	};
	size_t skipBytes(size_t nBytes);
	char* readCStr(size_t len);
	std::string readStr(size_t len);
};