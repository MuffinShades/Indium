#pragma once
#include "types.hpp"

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

constexpr size_t MAX_BLOCK_SIZE = 0xffffffff, MAX_BLOCK_SIZE_LOG16 = 8;

enum ByteStream_Mode {
	ByteStream_LittleEndian,
	ByteStream_BigEndian
};

struct mem_block {
	size_t sz, pos;
	mem_block* next, *prev;
	byte* dat;
};

#define BYTESTREAM_ALIGNED_16

class ByteStream {
private:
	mem_block* head_block = nullptr, *cur_block = nullptr;
	size_t allocSz = 0, len = 0, pos = 0, nBlocks = 0, blockPos = 0;
	size_t blockAllocSz = 0xffff, 
		   blockAllocSzLog16 = 4, 
		   blockAllocSzLog2 = 16;
	byte* cur = nullptr;
	mem_block* alloc_new_block(size_t blockSz);
	void block_append(mem_block* block);
	void add_new_block(const size_t blockSz);
	void add_new_block(byte* dat, const size_t blockSz);
	void block_adv();
	void pos_adv();
	void block_end();
	u32 mod_block_sz(const u64 val);
public:
	MSFL_EXP ByteStream(byte* dat, size_t len);
	MSFL_EXP ByteStream(size_t allocSz);
	MSFL_EXP ByteStream();

	//write functions
	MSFL_EXP virtual void WriteBytes(byte* dat, size_t sz);
	MSFL_EXP virtual void WriteInt(i64 val, size_t nBytes = 4);
	MSFL_EXP virtual void WriteUInt(i64 val, size_t nBytes = 4);
	MSFL_EXP virtual void WriteByte(byte b);
	MSFL_EXP virtual void WriteInt16(i16 b);
	MSFL_EXP virtual void WriteUInt16(u16 b);
	MSFL_EXP virtual void WriteInt24(i24 b);
	MSFL_EXP virtual void WriteUInt24(u24 b);
	MSFL_EXP virtual void WriteInt32(i32 b);
	MSFL_EXP virtual void WriteUInt32(u32 b);
	MSFL_EXP virtual void WriteInt48(i48 b);
	MSFL_EXP virtual void WriteUInt48(u48 b);
	MSFL_EXP virtual void WriteInt64(i64 b);
	MSFL_EXP virtual void WriteUInt64(u64 b);
	MSFL_EXP virtual void MultiWrite(u64 val, size_t valSz, size_t nCopy);

	//read functions
	MSFL_EXP virtual byte* ReadBytes(size_t sz);
	MSFL_EXP virtual i64 ReadInt(size_t nBytes);
	MSFL_EXP virtual u64 ReadUInt(size_t nBytes);
	MSFL_EXP virtual byte ReadByte();
	MSFL_EXP virtual i16 ReadInt16();
	MSFL_EXP virtual u16 ReadUInt16();
	MSFL_EXP virtual i24 ReadInt24();
	MSFL_EXP virtual u24 ReadUInt24();
	MSFL_EXP virtual i32 ReadInt32();
	MSFL_EXP virtual u32 ReadUInt32();
	MSFL_EXP virtual i48 ReadInt48();
	MSFL_EXP virtual u48 ReadUInt48();
	MSFL_EXP virtual i64 ReadInt64();
	MSFL_EXP virtual u64 ReadUInt64();

	//helper functions
	MSFL_EXP virtual void copyTo(byte* buffer, size_t copyStart, size_t copySz);
	MSFL_EXP virtual void pack(); //packs all data into 1 chunk
	MSFL_EXP virtual byte* getBytePtr(); //calls pack then returns pointer to base block
	MSFL_EXP virtual void seek(size_t pos);
	MSFL_EXP virtual size_t size();
	MSFL_EXP virtual size_t tell();
	MSFL_EXP virtual void clear();
	MSFL_EXP void free();
	MSFL_EXP void home();
	MSFL_EXP void resize(size_t sz);
	MSFL_EXP void clip();
	MSFL_EXP void skip(size_t nBytes);

	//operators
	MSFL_EXP byte operator[](size_t i);

	//settings
#ifdef BYTESTREAM_ALIGNED_16
	MSFL_EXP void setBlockAllocSz(const size_t log16Sz);
#else
	MSFL_EXP void setBlockAllocSz(const size_t sz);
#endif
};

#ifdef MSFL_DLL
#ifdef __cplusplus
}
#endif
#endif