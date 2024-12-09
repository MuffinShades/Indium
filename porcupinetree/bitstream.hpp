#pragma once
#include "bytestream.hpp"

class BitStream : public ByteStream {
private:
	size_t subBit = 0, curBitTotal = 0;
public:
	BitStream(byte* dat, size_t sz) : ByteStream(dat, sz) {

	}

	BitStream() : ByteStream() {

	}

	//read functions
	bit curBit();
	bit readBit();
	u32 readNBits(size_t nBits);
	void skipBits(size_t nBits);

	byte readByte() override;
	byte readByteAligned();

	i16 readInt16() override;
	i16 readInt16Aligned();
	u16 readUInt16() override;
	u16 readUInt16Aligned();

	i32 readInt32() override;
	i32 readInt32Aligned();
	u32 readUInt32() override;
	u32 readUInt32Aligned();

	i64 readInt64() override;
	i64 readInt64Aligned();
	u64 readUInt64() override;
	u64 readUInt64Aligned();

	u64 readBytesAsVal(size_t nBytes);

	//write functions
	void writeBit(bit b);
	void writeVal(u64 val, size_t nBits);

	void writeByte(byte val) override;
	void writeByteAligned(byte val);

	void writeInt16(i16 val) override;
	void writeInt16Aligned(i16 val);
	void writeUInt16(u16 val) override;
	void writeUInt16Aligned(u16 val);

	void writeInt32(i32 val) override;
	void writeInt32Aligned(i32 val);
	void writeUInt32(u32 val) override;
	void writeUInt32Aligned(u32 val);

	void writeInt64(i64 val) override;
	void writeInt64Aligned(i64 val);
	void writeUInt64(u64 val) override;
	void writeUInt64Aligned(u64 val);
	
	//align functions
	void alignToNextByte();
	void alignToPrevByte();

	//

};