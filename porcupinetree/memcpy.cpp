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
	__mini_cpy_group(130, avx_copy_128u, simple_copy_2); break; //2
	__mini_cpy_group(131, avx_copy_128u, simple_copy_2); simple_copy_2(dc - 2, sc - 2); break; //3
	__mini_cpy_group(132, avx_copy_128u, simple_copy_4); break; //4
	__mini_cpy_group(133, avx_copy_128u, simple_copy_4); simple_copy_1(dc - 1, sc - 1); break; //5
	__mini_cpy_group(134, avx_copy_128u, simple_copy_4); simple_copy_2(dc - 2, sc - 2); break; //6
	__mini_cpy_group(135, avx_copy_128u, simple_copy_4); simple_copy_4(dc - 4, sc - 4); break; //7
	__mini_cpy_group(136, avx_copy_128u, simple_copy_8); break; //8
	__mini_cpy_group(137, avx_copy_128u, simple_copy_8); simple_copy_1(dc - 1, sc - 1); break; //9
	__mini_cpy_group(138, avx_copy_128u, simple_copy_8); simple_copy_2(dc - 2, sc - 2); break; //10
	__mini_cpy_group(139, avx_copy_128u, simple_copy_8); simple_copy_4(dc - 4, sc - 4); break; //11
	__mini_cpy_group(140, avx_copy_128u, simple_copy_8); simple_copy_4(dc - 4, sc - 4); break; //12
	__mini_cpy_group(141, avx_copy_128u, avx_copy_16);   break; //13 uhh i think you need to shift the 16s down since 13-16 need to be 8-8
	__mini_cpy_group(142, avx_copy_128u, avx_copy_16);
	__mini_cpy_group(143, avx_copy_128u, avx_copy_16);
	__mini_cpy_group(144, avx_copy_128u, avx_copy_16);
	__mini_cpy_group(145, avx_copy_128u, avx_copy_16);
	__mini_cpy_group(146, avx_copy_128u, avx_copy_16);
	__mini_cpy_group(147, avx_copy_128u, avx_copy_16);
	__mini_cpy_group(148, avx_copy_128u, avx_copy_16);
	__mini_cpy_group(149, avx_copy_128u, avx_copy_16);
	__mini_cpy_group(150, avx_copy_128u, avx_copy_16);
	__mini_cpy_group(151, avx_copy_128u, avx_copy_16);
	__mini_cpy_group(152, avx_copy_128u, avx_copy_16);
	__mini_cpy_group(153, avx_copy_128u, avx_copy_16);
	__mini_cpy_group(154, avx_copy_128u, avx_copy_16);
	__mini_cpy_group(155, avx_copy_128u, avx_copy_16);
	__mini_cpy_group(156, avx_copy_128u, avx_copy_16);
	__mini_cpy_group(157, avx_copy_128u, avx_copy_16);
	__mini_cpy_group(158, avx_copy_128u, avx_copy_32);
	__mini_cpy_group(159, avx_copy_128u, avx_copy_32);
	__mini_cpy_group(160, avx_copy_128u, avx_copy_32);
	__mini_cpy_group(161, avx_copy_128u, avx_copy_32);
	__mini_cpy_group(162, avx_copy_128u, avx_copy_32);
	__mini_cpy_group(163, avx_copy_128u, avx_copy_32);
	__mini_cpy_group(164, avx_copy_128u, avx_copy_32);
	__mini_cpy_group(165, avx_copy_128u, avx_copy_32);
	__mini_cpy_group(166, avx_copy_128u, avx_copy_32);
	__mini_cpy_group(167, avx_copy_128u, avx_copy_32);
	__mini_cpy_group(168, avx_copy_128u, avx_copy_32);
	__mini_cpy_group(169, avx_copy_128u, avx_copy_32);
	__mini_cpy_group(170, avx_copy_128u, avx_copy_32);
	__mini_cpy_group(140, avx_copy_128u, avx_copy_32);
	__mini_cpy_group(171, avx_copy_128u, avx_copy_32);
	__mini_cpy_group(172, avx_copy_128u, avx_copy_32);
	__mini_cpy_group(173, avx_copy_128u, avx_copy_32);
	__mini_cpy_group(174, avx_copy_128u, avx_copy_32);
	__mini_cpy_group(175, avx_copy_128u, avx_copy_32);
	__mini_cpy_group(176, avx_copy_128u, avx_copy_32);
	__mini_cpy_group(177, avx_copy_128u, avx_copy_32);
	__mini_cpy_group(178, avx_copy_128u, avx_copy_32);
	__mini_cpy_group(179, avx_copy_128u, avx_copy_32);
	__mini_cpy_group(180, avx_copy_128u, avx_copy_32);
	__mini_cpy_group(181, avx_copy_128u, avx_copy_32); //55 32
	__mini_cpy_group(182, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(183, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(184, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(185, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(186, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(187, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(189, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(190, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(191, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(192, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(193, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(194, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(195, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(196, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(197, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(198, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(199, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(200, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(201, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(202, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(203, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(204, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(205, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(206, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(207, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(208, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(209, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(210, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(211, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(212, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(213, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(214, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(215, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(216, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(217, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(218, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(219, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(220, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(221, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(222, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(223, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(224, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(225, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(226, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(227, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(228, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(229, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(230, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(231, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(232, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(233, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(234, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(235, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(236, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(237, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(238, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(239, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(240, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(241, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(242, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(243, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(244, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(245, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(246, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(247, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(248, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(249, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(250, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(251, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(252, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(253, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(254, avx_copy_128u, simple_copy_8);
	__mini_cpy_group(255, avx_copy_128u, simple_copy_8);
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