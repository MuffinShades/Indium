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

#if (defined(_WIN32) || defined(WIN32) || defined(__unix))

//16 byte copy functins
#define loadu_256(src, i) __m256i __m##i = _mm256_loadu_si256(((const __m256i*) src) + i)
#define storeu_256(dest, i) _mm256_storeu_si256(((__m256i*)dest) + i, __m##i)

#define load_256(src, i) __m256i __m##i = _mm256_load_si256(((const __m256i*) src) + i)
#define store_256(dest, i) _mm256_store_si256(((__m256i*)dest) + i, __m##i)

//32 bytes copy functions
#define loadu_512(src, i) __m512i __m##i = _mm512_loadu_si512(((const __m512i*) src) + i)
#define storeu_512(dest, i) _mm512_storeu_si512(((__m512i*)dest) + i, __m##i)

#define load_512(src, i) __m512i __m##i = _mm512_loadu_si512(((const __m512i*) src) + i)
#define store_512(dest, i) _mm512_storeu_si512(((__m512i*)dest) + i, __m##i)

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
#ifdef __AVX512F__
	load_512(src, 0);

	store_512(dest, 0);
#else
	load_256(src, 0);
	store_256(dest, 0);
#endif
}

//copies 64 bytes
static void avx_copy_64u(void* dest, const void* src) {
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

static void avx_copy_64(void* dest, const void* src) {
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

//copies 128 bytes
static void avx_copy_128u(void* dest, const void* src) {
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
	storeu_256(dest, 0);
	storeu_256(dest, 1);
	storeu_256(dest, 2);
	storeu_256(dest, 3);
	storeu_256(dest, 4);
	storeu_256(dest, 5);
#endif
}

static void avx_copy_128(void* dest, const void* src) {
#ifdef __AVX512F__
	load_512(src, 0);
	load_512(src, 1);
	load_512(src, 2);
	load_512(src, 3);
	store_512(dest, 0);
	store_512(dest, 1);
	store_512(dest, 2);
	store_512(dest, 3);
#else
	load_256(src, 0);
	load_256(src, 1);
	load_256(src, 2);
	load_256(src, 3);
	load_256(src, 4);
	load_256(src, 5);
	store_256(dest, 0);
	store_256(dest, 1);
	store_256(dest, 2);
	store_256(dest, 3);
	store_256(dest, 4);
	store_256(dest, 5);
#endif
}

//copies 256 bytes
static void avx_copy_256u(void* dest, const void* src) {
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

static void avx_copy_256(void* dest, const void* src) {
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
#ifdef __AVX512F__
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
#endif

static void* in_minicpy(void* dest, const void* src, size_t len) {
	if (!dest || !src || len == 0) return nullptr;
	char* dc = (char*) dest;
	const char* sc = (const char*) src;

	switch (len) {
	case 1:
		*dc = *sc;
		break;
	case 32:
		avx_copy_32u(dest, src);
		break;
	case 64:
		avx_copy_64u(dest, src);
		break;
	case 128:
		avx_copy_128u(dest, src);
		break;
	case 256:
		avx_copy_256u(dest, src);
		break;
	default:
		return in_memcpy(dest, src, len);
	}
}

static void* in_memcpy(void* dest, const void* src, size_t len) {
	if (!dest || !src)
		return nullptr;

	if (len <= 0x100)
		return in_minicpy(dest, src, len);
}

#else

#endif