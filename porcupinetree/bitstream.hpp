#pragma once
#include "bytestream.hpp"

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

class BitStream : public ByteStream {
private:
	size_t subBit = 0, curBitTotal = 0;
public:
	BitStream(byte* dat, size_t sz) : ByteStream(dat, sz) {

	}

	BitStream() : ByteStream() {

	}

	//read functions
	MSFL_EXP bit curBit();
	MSFL_EXP bit readBit();
	MSFL_EXP u32 readNBits(size_t nBits);
	void skipBits(size_t nBits);

	MSFL_EXP byte readByte() override;
	MSFL_EXP byte readByteAligned();

	MSFL_EXP i16 readInt16() override;
	i16 readInt16Aligned();
	MSFL_EXP u16 readUInt16() override;
	u16 readUInt16Aligned();

	MSFL_EXP i32 readInt32() override;
	i32 readInt32Aligned();
	MSFL_EXP u32 readUInt32() override;
	u32 readUInt32Aligned();

	MSFL_EXP i64 readInt64() override;
	i64 readInt64Aligned();
	MSFL_EXP u64 readUInt64() override;
	u64 readUInt64Aligned();

	MSFL_EXP u64 readBytesAsVal(size_t nBytes);
	MSFL_EXP byte* readBytes(size_t nBytes) override;

	//write functions
	MSFL_EXP void writeBit(bit b);
	MSFL_EXP void writeVal(u64 val, size_t nBits);

	MSFL_EXP void writeByte(byte val) override;
	MSFL_EXP void writeByteAligned(byte val);

	MSFL_EXP void writeInt16(i16 val) override;
	void writeInt16Aligned(i16 val);
	MSFL_EXP void writeUInt16(u16 val) override;
	void writeUInt16Aligned(u16 val);

	MSFL_EXP void writeInt32(i32 val) override;
	void writeInt32Aligned(i32 val);
	MSFL_EXP void writeUInt32(u32 val) override;
	void writeUInt32Aligned(u32 val);

	MSFL_EXP void writeInt64(i64 val) override;
	void writeInt64Aligned(i64 val);
	MSFL_EXP void writeUInt64(u64 val) override;
	void writeUInt64Aligned(u64 val);
	
	//align functions
	MSFL_EXP void alignToNextByte();
	MSFL_EXP void alignToPrevByte();

	//

};

#ifdef MSFL_DLL
#ifdef __cplusplus
}
#endif
#endif