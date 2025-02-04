#include "memcpy.hpp"

//reference https://github.com/skywind3000/FastMemcpy/blob/master/FastMemcpy_Avx.h

#include <stddef.h>
#include <stdint.h>
#include <immintrin.h>
#include <intrin.h>

#include "msutil.hpp"
#include "types.hpp"

#if (defined(_WIN32) || defined(WIN32))
#include <mmsystem.h>
#elif defined(__unix)
#include <sys/time.h>
#include <unistd.h>
#endif

#if (defined(_WIN32) || defined(WIN32) || defined(__unix) || defined(__APPLE__))

//16 byte copy functions
#define loadu_128(src, i) __m128i __m##i = _mm256_loadu_si128(((const __m128i*) src) + i)
#define storeu_128(dest, i) _mm128_storeu_si256(((__m128i*)dest) + i, __m##i)

#define load_128(src, i) __m128i __m##i = _mm128_load_si128(((const __m128i*) src) + i)
#define store_128(dest, i) _mm128_store_si128(((__m128i*)dest) + i, __m##i)

//32 byte copy functins
#define loadu_256(src, i) __m256i __m##i = _mm256_loadu_si256(((const __m256i*) src) + i)
#define storeu_256(dest, i) _mm256_storeu_si256(((__m256i*)dest) + i, __m##i)

#define load_256(src, i) __m256i __m##i = _mm256_load_si256(((const __m256i*) src) + i)
#define store_256(dest, i) _mm256_store_si256(((__m256i*)dest) + i, __m##i)

//64 bytes copy functions
#define loadu_512(src, i) __m512i __m##i = _mm512_loadu_si512(((const __m512i*) src) + i)
#define storeu_512(dest, i) _mm512_storeu_si512(((__m512i*)dest) + i, __m##i)

#define load_512(src, i) __m512i __m##i = _mm512_loadu_si512(((const __m512i*) src) + i)
#define store_512(dest, i) _mm512_storeu_si512(((__m512i*)dest) + i, __m##i)

//copy for 2-16 byte align
static void simple_copy_2(void *dest, const void *src) {
	*((u16*)dest) = *((u16*)src);
}

static void simple_copy_4(void *dest, const void *src) {
	*((u32*)dest) = *((u32*)src);
}

static void simple_copy_8(void *dest, const void *src) {
	*((u64*)dest) = *((u64*)src);
}

//copies 16 bytes
static void avx_copy_16u(void *dest, const void *src) {
	loadu_128(src, 0);
	storeu_128(dest, 0);
}

static void avx_copy_16(void *dest, const void *src) {
	load_128(src, 0);
	store_128(dest, 0);
}

//copies 32 bytes
static void avx_copy_32u(void* dest, const void* src) {
#ifdef __AVX512F__
	loadu_512(src, 0);

	storeu_512(dest, 0);
#else
	loadu_256(src, 0);
	loadu_256(src, 1);

	storeu_256(dest, 0);
	storeu_256(dest, 1);
#endif
}


//TODO: 256 -> 16 bytes, 512 -> 32bytes
static void avx_copy_32(void* dest, const void* src) {
	load_256(src, 0);
	store_256(dest, 0);
}

//copies 64 bytes
static void avx_copy_64u(void* dest, const void* src) {
#ifdef __AVX512F__
	loadu_512(src, 0);
	storeu_512(dest, 0);
#else
	loadu_256(src, 0);
	loadu_256(src, 1);
	storeu_256(dest, 0);
	storeu_256(dest, 1);
#endif
}

static void avx_copy_64(void* dest, const void* src) {
#ifdef __AVX512F__
	load_512(src, 0);
	store_512(dest, 0);
#else
	load_256(src, 0);
	load_256(src, 1);
	store_256(dest, 0);
	store_256(dest, 1);
#endif
}

//copies 128 bytes
static void avx_copy_128u(void* dest, const void* src) {
#ifdef __AVX512F__
	loadu_512(src, 0);
	loadu_512(src, 1);
	storeu_512(dest, 0);
	storeu_512(dest, 1);
#else
	loadu_256(src, 0);
	loadu_256(src, 1);
	loadu_256(src, 2);
	loadu_256(src, 3);
	storeu_256(dest, 0);
	storeu_256(dest, 1);
	storeu_256(dest, 2);
	storeu_256(dest, 3);
#endif
}

static void avx_copy_128(void* dest, const void* src) {
#ifdef __AVX512F__
	load_512(src, 0);
	load_512(src, 1);
	store_512(dest, 0);
	store_512(dest, 1);
#else
	load_256(src, 0);
	load_256(src, 1);
	load_256(src, 2);
	load_256(src, 3);
	store_256(dest, 0);
	store_256(dest, 1);
	store_256(dest, 2);
	store_256(dest, 3);
#endif
}

//copies 256 bytes
static void avx_copy_256u(void* dest, const void* src) {
#ifdef __AVX512F__
	loadu_512(src, 0);
	loadu_512(src, 1);
	loadu_512(src, 2);
	loadu_512(src, 3);
	storeu_512(dest, 0);
	storeu_512(dest, 1);
	storeu_512(dest, 2);
	storeu_512(dest, 3);
#else
	loadu_256(src, 0);
	loadu_256(src, 1);
	loadu_256(src, 2);
	loadu_256(src, 3);
	loadu_256(src, 4);
	loadu_256(src, 5);
	loadu_256(src, 6);
	loadu_256(src, 7);
	storeu_256(dest, 0);
	storeu_256(dest, 1);
	storeu_256(dest, 2);
	storeu_256(dest, 3);
	storeu_256(dest, 4);
	storeu_256(dest, 5);
	storeu_256(dest, 6);
	storeu_256(dest, 7);
#endif
}

static void avx_copy_256(void* dest, const void* src) {
#ifdef __AVX512F__
	load_512(src, 0);
	load_512(src, 1);
	load_512(src, 2);
	load_512(src, 3);
	store_512(dest, 0);
	store_512(dest, 1);
	store_512(src, 2);
	store_512(src, 3);
#else
	load_256(src, 0);
	load_256(src, 1);
	load_256(src, 2);
	load_256(src, 3);
	load_256(src, 4);
	load_256(src, 5);
	load_256(src, 6);
	load_256(src, 7);
	store_256(dest, 0);
	store_256(dest, 1);
	store_256(dest, 2);
	store_256(dest, 3);
	store_256(dest, 4);
	store_256(dest, 5);
	store_256(dest, 6);
	store_256(dest, 7);
#endif
}

//copies 512 bytes
static void avx_copy_512u(void* dest, const void* src) {
	loadu_512(src, 0);
	loadu_512(src, 1);
	loadu_512(src, 2);
	loadu_512(src, 3);
	loadu_512(src, 4);
	loadu_512(src, 5);
	loadu_512(src, 6);
	loadu_512(src, 7);
	storeu_512(dest, 0);
	storeu_512(dest, 1);
	storeu_512(dest, 2);
	storeu_512(dest, 3);
	storeu_512(dest, 4);
	storeu_512(dest, 5);
	storeu_512(dest, 6);
	storeu_512(dest, 7);
}

static void avx_copy_512(void* dest, const void* src) {
	load_512(src, 0);
	load_512(src, 1);
	load_512(src, 2);
	load_512(src, 3);
	load_512(src, 4);
	load_512(src, 5);
	load_512(src, 6);
	load_512(src, 7);
	store_512(dest, 0);
	store_512(dest, 1);
	store_512(dest, 2);
	store_512(dest, 3);
	store_512(dest, 4);
	store_512(dest, 5);
	store_512(dest, 6);
	store_512(dest, 7);
}

#define __mini_cpy_group(a,cfa,cfb) case a: cfa(dc - a, sc - a); \
									case (a - 128): cfb(dc - (a - 128), sc - (a - 128))

#define simple_copy_1(dest, src) *(dest) = *(src)

static void* in_minicpy256(void* dest, const void* src, size_t len) {
	if (!dest || !src || len == 0) return nullptr;
	char* dc = (char*) dest + len;
	const char* sc = (const char*) src + len;

	switch (len) {
	case 128: avx_copy_128u(dc - 128, sc - 128); break;
	case 129: avx_copy_128u(dc - 129, sc - 129);
	case 1:   *dc = *sc; break;
	__mini_cpy_group(130, avx_copy_128u, simple_copy_2); break;
	__mini_cpy_group(131, avx_copy_128u, simple_copy_2); simple_copy_1(dc - 1, sc - 1); break;
	__mini_cpy_group(132, avx_copy_128u, simple_copy_4); break;
	__mini_cpy_group(133, avx_copy_128u, simple_copy_4); simple_copy_1(dc - 1, sc - 1); break;
	__mini_cpy_group(134, avx_copy_128u, simple_copy_4); simple_copy_2(dc - 2, sc - 2); break;
	__mini_cpy_group(135, avx_copy_128u, simple_copy_4); break;
	__mini_cpy_group(136, avx_copy_128u, simple_copy_8); break;
	__mini_cpy_group(137, avx_copy_128u, simple_copy_8); break;
	__mini_cpy_group(138, avx_copy_128u, simple_copy_8); break;
	__mini_cpy_group(139, avx_copy_128u, simple_copy_8); break;
	__mini_cpy_group(140, avx_copy_128u, simple_copy_8); break;
	case 256: avx_copy_256u(dest, src); break;
	default:
		return in_memcpy(dest, src, len);
	}
}

static void* in_minicpy512(void* dest, const void* src, size_t len) {
	if (!dest || !src || len == 0) return nullptr;
	char* dc = (char*) dest;
	const char* sc = (const char*) src;

	switch (len) {
	case 1: *dc = *sc; break;
	case 2: simple_copy_2(dest, src); break;
	case 32: avx_copy_32u(dest, src); break;
	case 64: avx_copy_64u(dest, src); break;
	case 128: avx_copy_128u(dest, src); break;
	case 256: avx_copy_256u(dest, src); break;
	default:
		return in_memcpy(dest, src, len);
	}
}

static void* in_memcpy(void* dest, const void* src, size_t len) {
	if (!dest || !src)
		return nullptr;

	if (len <= 0x100)
		return in_minicpy(dest, src, len);

	const size_t toAlign = fast_mod512(len);

	//64 byte align
	in_minicpy(dest, src, toAlign);
	char *dst_c = (char*) dest + toAlign;
	const char* src_c = (const char*) src + toAlign;
	size_t lCopy = toAlign;

	//bulk copy
	if (src & 511)
		while (lCopy > 64) {
			avx_copy_512u(dst_c, src_c);
			dst_c += 64;
			src_c += 64;
			lCopy -= 64;
		}
	else
		while (lCopy > 64) {
			avx_copy_512(dst_c, src_c);
			dst_c += 64;
			src_c += 64;
			lCopy -= 64;
		}

	//trailing bytes
	in_minicpy512(dst_c, src_c, lCopy);
	return dest;
}

#else

#endif