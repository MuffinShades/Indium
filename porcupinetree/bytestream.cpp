#include "bytestream.hpp"
#include "msutil.hpp"

constexpr size_t ALIGN_COMPUTE_THRESH = 0xff;

void free_block(mem_block* block) {
	if (!block) return;

	if (block->dat)
		_safe_free_a(block->dat);
	_safe_free_b(block);
}

void ByteStream::free() {

	//free memory
	if (this->head_block) {
		mem_block* c_block = this->head_block;

		while (c_block) {
			mem_block* next = c_block;
			free_block(c_block);
			c_block = next;
		}
	}

	//reset everything else
	this->pos = this->allocSz = this->len = 0;
	this->cur_block = this->head_block = nullptr;
}

void ByteStream::block_append(mem_block *block) {
	if (!block) return;

	this->allocSz += block->sz;

	if (!this->head_block) {
		this->head_block = block;
		this->cur_block = block;
		return;
	}
	

	//make blocks point to eachother and append
	this->cur_block->next = block;
	block->prev = this->cur_block;
	block->pos = this->allocSz;
	this->cur_block = block;
}

mem_block* ByteStream::alloc_new_block(size_t sz) {
#ifdef BYTESTREAM_ALIGNED_16
	//aligned size to 0xf if needed
	sz = 1 << min(((fast_log16(sz)+1) << 2), MAX_BLOCK_SIZE_LOG16 << 2);
#endif
	if (sz <= 0) return nullptr;

	mem_block* blck = new mem_block{
		.sz = sz,
		.dat = new byte[sz]
	};

	if (!blck->dat) {
		_safe_free_b(blck);
		return nullptr;
	}

	ZeroMem(blck->dat, blck->sz);

	return blck;
}

void ByteStream::add_new_block(const size_t sz) {
	mem_block* block = this->alloc_new_block(sz);

	if (!block) return;

	this->block_append(block);
}

//returns mod block size, useful for getting sub-block position
u32 ByteStream::mod_block_sz(const u64 val) {
#ifdef BYTESTREAM_ALIGNED_16
	return val - ((val >> blockAllocSzLog2) << blockAllocSzLog2);
#else
	u64 b = val;
	do {}  while ((b -= this->allocSz) > this->allocSz);
	return (u32) b;
#endif
}

//jumps to last block from cur block
void ByteStream::block_end() {
	while (this->cur_block->next)
		this->cur_block = this->cur_block->next;
}

void ByteStream::add_new_block(byte* dat, const size_t sz) {
	if (!dat) return;

	size_t bytesLeft = sz, blck_sz = 0;

	do {
		mem_block* block = this->alloc_new_block(sz);

		if (!block) return;

#ifndef BYTESTREAM_ALIGNED_16
		memcpy(block->dat, dat, blck_sz = block->sz);
#else
#ifdef COMPILER_MODE_64_BIT
		a_memcpy64(block->dat, dat, blck_sz=sz);
#else
		a_memcpy32(block->dat, dat, blck_sz = block->sz);
#endif
#endif

		this->block_append(block);

		this->allocSz += block->sz;
	} while ((i64)(bytesLeft -= blck_sz) > 0);

	this->len = sz; //set length to allocation size since were appending a full block of data
	this->pos = this->len - 1; //set pos to last byte we read
	this->block_end(); //jump to last block
	this->blockPos = this->mod_block_sz(this->pos); //compute block pos
}

ByteStream::ByteStream(byte* dat, size_t len) {
	if (!dat || len <= 0) return;

	if (this->head_block)
		this->free();

	//add first block and we done
	this->add_new_block(dat, len);
}

void ByteStream::block_adv() {
	if (this->cur_block && this->cur_block->next) {
		this->cur_block = this->cur_block->next;
		this->blockPos = 0;
		this->cur = this->cur_block->dat;
	}
}

void ByteStream::pos_adv() {
	if (++this->pos >= this->len) {
		this->len++;
		if (this->len > this->allocSz)
			this->add_new_block(this->blockAllocSz);
		this->block_adv();
	}
	else if (++this->blockPos >= this->cur_block->sz)
		this->block_adv();
	else
		this->cur++;
}

void ByteStream::writeBytes(byte *dat, size_t sz) {
	if (!dat || sz <= 0)
		return;
	this->pos += sz;
	size_t blockBytesLeft;
	size_t rCopy = min(sz, (blockBytesLeft = (this->cur_block->sz - this->blockPos)));

	if (sz > ALIGN_COMPUTE_THRESH)
		a_memcpy(this->cur_block->dat, dat, rCopy);
	else
		memcpy(this->cur_block->dat, dat, rCopy);
	sz -= rCopy;

	byte *dp = dat + rCopy;

	//TODO: when writing blocks try computing the alignment for a block and use a
	//manual version of dy_memcpy to optimally copy over for a block of data
	//still have an if between dynamic copy and just normal memcpy since for
	//small byte writes memcpy is likely faster

	while (sz > this->blockAllocSz) {
#ifdef BYTESTREAM_ALIGNED_16
		dy_memcpy_manual_fast(this->cur_block->dat, dp, sz, this->blockAllocSzAlignment);
#else
		dy_memcpy_manual(this->cur_block->dat, dp, sz, this->blockAllocSzAlignment);
#endif
		sz -= this->blockAllocSz;
		dp += this->blockAllocSz;
	}

	if (sz > 0) {
		this->block_adv();
		if (sz > ALIGN_COMPUTE_THRESH)
			a_memcpy(this->cur_block->dat, dp, sz);
		else
			memcpy(this->cur_block->dat, dp, sz);
	}
}

void ByteStream::writeByte(byte b) {
	this->pos_adv();
	*this->cur = b;
}

void ByteStream::setMode(ByteStream_Mode mode) {
	this->int_mode = mode;
}

void ByteStream::writeInt(i64 val, size_t nBytes) {
	//clamp
	if (nBytes <= 0) nBytes = 1;
	if (nBytes > 8) nBytes = 8;

	if (this->int_mode == ByteStream_LittleEndian)
		while (nBytes--) {
			this->writeByte(val & 0xff);
			val >>= 8;
		}
	else
		while (nBytes--)
			this->writeByte((val >> (nBytes << 3)) & 0xff);
}

void ByteStream::writeUInt(u64 val, size_t nBytes) {
	//clamp
	if (nBytes <= 0) nBytes = 1;
	if (nBytes > 8) nBytes = 8;

	if (this->int_mode == ByteStream_LittleEndian)
		while (nBytes--) {
			this->writeByte(val & 0xff);
			val >>= 8;
		}
	else
		while (nBytes--)
			this->writeByte((val >> (nBytes << 3)) & 0xff);
}

//int writes
void ByteStream::writeInt16(i16 val) {
	this->writeInt(val, sizeof(i16));
}

void ByteStream::writeUInt16(u16 val) {
	this->writeUInt(val, sizeof(u16));
}

void ByteStream::writeInt24(i24 val) {
	this->writeInt(val, 3);
}

void ByteStream::writeUInt24(u24 val) {
	this->writeUInt(val, 3);
}

void ByteStream::writeInt32(i32 val) {
	this->writeInt(val, sizeof(i32));
}

void ByteStream::writeUInt32(u32 val) {
	this->writeUInt(val, sizeof(u32));
}

void ByteStream::writeInt48(i48 val) {
	this->writeInt(val, 5);
}

void ByteStream::writeUInt48(u48 val) {
	this->writeUInt(val, 5);
}

void ByteStream::writeInt64(i64 val) {
	this->writeInt(val, sizeof(i64));
}

void ByteStream::writeUInt64(u64 val) {
	this->writeUInt(val, sizeof(u64));
}

//TODO: int reads and other reads along with multi write






//block alloc size stuff
#ifdef BYTESTREAM_ALIGNED_16
void ByteStream::setBlockAllocSz(const size_t log16Sz) {
	const size_t sz = 1 << (log16Sz << 2);
	this->blockAllocSz = min(sz, MAX_BLOCK_SIZE);
	//this->blockAllocSzLog16 = log16Sz;
	//this->blockAllocSzLog2 = log16Sz << 2;
	this->blockAllocSzLog16 = fast_log16(sz);
	this->blockAllocSzLog2 = fast_log2(sz);
	this->blockAllocSzAlignment = computeMaxMod(sz);
	this->sz_aligned = true;
}
#else
void ByteStream::setBlockAllocSz(const size_t sz) {
	this->blockAllocSz = min(sz, MAX_BLOCK_SIZE);
	this->blockAllocSzLog16 = fast_log16(sz);
	this->blockAllocSzLog2  = fast_log2(sz);
	this->blockAllocSzAlignment = computeMaxMod(sz);
}
#endif