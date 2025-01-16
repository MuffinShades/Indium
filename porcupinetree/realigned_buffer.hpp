#pragma once

/**
 *
 * Realigned Buffer
 *
 * This class is used to take buffers that store
 * values in strange sizes (ie 3 bytes) in normal
 * sizes like 4 bytes and allow you to extract the
 * odd values easily
 *
 * Smaller version of ByteStream that works on
 * buffers which store values of sizes different
 * than the C buffer type
 *
 * IE u32 buffer holding 3 byte values
 *
 */

#include <bit>
#include <cassert>

template<class _Ty, const size_t it_sz> class realigned_buffer {
private:
	_Ty* int_buf = nullptr;
	size_t int_sz = 0;
	size_t cur = 0;
	bool lil = false;
public:
	size_t length = 0;
	realigned_buffer(_Ty* buf, size_t len) {
		this->int_buf = buf;
		this->int_sz = len;
		this->cur = 0;

		static_assert(sizeof(_Ty) > 0);

		this->computeLength();

		lil = std::endian::native == std::endian::little;
	}

	void computeLength() {
		this->length = (size_t)(this->int_sz * sizeof(_Ty)) / it_sz;
	}

	u64 operator[] (size_t idx) {
		this->computeLength();
		assert(idx < this->length);

		const size_t s = idx * it_sz;
		u64 BeV = 0;
		memcpy((void*)&BeV, (void*)(((byte*)this->int_buf) + s), it_sz);
		return BeV;
	}
};