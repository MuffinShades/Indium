#include "bytestream.hpp"
#include "msutil.hpp"

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

		memcpy(block->dat, dat, blck_sz = block->sz);

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

void ByteStream::WriteBytes(byte *dat, size_t sz) {
	if (!dat || sz <= 0)
		return;
#ifdef BYTESTREAM_ALIGNED_16
	
#else
	size_t blockBytesLeft;
	if (sz < (blockBytesLeft = (this->cur_block->sz - this->blockPos))) {
		memcpy(this->cur_block->dat + this->blockPos, dat, sz);
		this->blockPos += sz;
	} else {
		size_t p;
		this->pos += sz;
		memcpy(this->cur_block->dat + this->blockPos, dat, p=blockBytesLeft);
		sz -= blockBytesLeft;

		while (sz > this->blockAllocSz) {
			this->add_new_block(dat+p, this->blockAllocSz);
			p += this->blockAllocSz;
		}

		//copy left over bytes
		this->block_adv();
		memcpy(this->cur_block->dat, dat+p, sz);
		this->blockPos = sz;
	}
#endif
}

void ByteStream::WriteByte(byte b) {
	this->pos_adv();
	*this->cur = b;
}

#ifdef BYTESTREAM_ALIGNED_16
void ByteStream::setBlockAllocSz(const size_t log16Sz) {
	const size_t sz = 1 << (log16Sz << 2);
	this->blockAllocSz = min(sz, MAX_BLOCK_SIZE);
	//this->blockAllocSzLog16 = log16Sz;
	//this->blockAllocSzLog2 = log16Sz << 2;
	this->blockAllocSzLog16 = fast_log16(sz);
	this->blockAllocSzLog2 = fast_log2(sz);
}
#else
void ByteStream::setBlockAllocSz(const size_t sz) {
	this->blockAllocSz = min(sz, MAX_BLOCK_SIZE);
	this->blockAllocSzLog16 = fast_log16(sz);
	this->blockAllocSzLog2  = fast_log2(sz);
}
#endif