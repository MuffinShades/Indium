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
#define loadu_128(src, i) __m128i __m##i = _mm_loadu_si128(((const __m128i*) src) + i)
#define storeu_128(dest, i) _mm_storeu_si128(((__m128i*)dest) + i, __m##i)

#define load_128(src, i) __m128i __m##i = _mm_load_si128(((const __m128i*) src) + i)
#define store_128(dest, i) _mm_store_si128(((__m128i*)dest) + i, __m##i)

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

//copy for 1-16 byte align
#define simple_copy_1(dest, src) *(dest) = *(src)

static void simple_copy_2(void *dest, const void *src) {
	*((u16*)dest) = *((u16*)src);
}

static void simple_copy_4(void *dest, const void *src) {
	*((u32*)dest) = *((u32*)src);
}

static void simple_copy_8(void *dest, const void *src) {
	*((u64*)dest) = *((u64*)src);
}

//avx versions
#define avx_copy_1 simple_copy_1
#define avx_copy_1u simple_copy_1
#define avx_copy_2u simple_copy_2
#define avx_copy_2 simple_copy_2
#define avx_copy_4u simple_copy_4
#define avx_copy_4 simple_copy_4
#define avx_copy_8u simple_copy_8
#define avx_copy_8 simple_copy_8

//true avx copies 16 bytes
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

#define __mini_cpy_group_128(a,cfa,cfb) case a: cfa(dc - a, sc - a); \
	case (a - 128): cfb(dc - (a - 128), sc - (a - 128))

#define __mini_cpy_group_256(a,cfa,cfb) case a: cfa(dc - a, sc - a); \
	case (a - 256): cfb(dc - (a - 256), sc - (a - 256))

#define __cpy_pair_sim(fn, b) simple_copy_##fn(dc - b, sc - b)
#define __cpy_pair_avx(fn, b) avx_copy_##fn(dc - b, sc - b)

#define __mini_line_128(a, b, c, d) __mini_cpy_group_128(a, avx_copy_128u, avx_copy_##b); avx_copy_##c##u(dc - d, sc - b); break;
#define __mini_lin0_128(a, b) __mini_cpy_group_128(a, avx_copy_128u, avx_copy_##b##u); break;

#define __mini_line_256(a, b, c, d) __mini_cpy_group_256(a, avx_copy_256u, avx_copy_##b); avx_copy_##c##u(dc - d, sc - b); break;
#define __mini_lin0_256(a, b) __mini_cpy_group_256(a, avx_copy_256u, avx_copy_##b##u); break;

static void* in_minicpy256(void* dest, const void* src, size_t len) {
	if (!dest || !src || len == 0) return nullptr;
	char* dc = (char*) dest + len;
	const char* sc = (const char*) src + len;

	switch (len) {
	case 128: avx_copy_128u(dc - 128, sc - 128); break;
	case 129: avx_copy_128u(dc - 129, sc - 129);
	case 1:   *dc = *sc; break;
	__mini_lin0_128(130,2)
	__mini_line_128(131,2,1,1)
	__mini_lin0_128(132,4)
	__mini_line_128(133,4,1,1)
	__mini_line_128(134,4,2,2)
	__mini_line_128(135,4,4,3)
	__mini_lin0_128(136,8)
	__mini_line_128(137,8,1,1)
	__mini_line_128(138,8,2,2)
	__mini_line_128(139,8,4,3)
	__mini_line_128(140,8,4,4)
	__mini_line_128(141,8,8,5)
	__mini_line_128(142,8,8,6)
	__mini_line_128(143,8,8,7)
	__mini_lin0_128(144,16)
	__mini_line_128(145,16,1,1)
	__mini_line_128(146,16,2,2)
	__mini_line_128(147,16,4,3)
	__mini_line_128(148,16,4,4)
	__mini_line_128(149,16,8,5)
	__mini_line_128(150,16,8,6)
	__mini_line_128(151,16,8,7)
	__mini_line_128(152,16,8,8)
	__mini_line_128(153,16,16,9)
	__mini_line_128(154,16,16,10)
	__mini_line_128(155,16,16,11)
	__mini_line_128(156,16,16,12)
	__mini_line_128(157,16,16,13)
	__mini_line_128(158,16,16,14)
	__mini_line_128(159,16,16,15)
	__mini_lin0_128(160,32)
	__mini_line_128(161,32,1,1)
	__mini_line_128(162,32,2,2)
	__mini_line_128(163,32,4,3)
	__mini_line_128(164,32,4,4)
	__mini_line_128(165,32,8,5)
	__mini_line_128(166,32,8,6)
	__mini_line_128(167,32,8,7)
	__mini_line_128(168,32,8,8)
	__mini_line_128(169,32,16,9)
	__mini_line_128(170,32,16,10)
	__mini_line_128(171,32,16,11)
	__mini_line_128(172,32,16,12)
	__mini_line_128(173,32,16,13)
	__mini_line_128(174,32,16,14)
	__mini_line_128(175,32,16,15)
	__mini_line_128(176,32,16,16)
	__mini_line_128(177,32,32,17)
	__mini_line_128(178,32,32,18)
	__mini_line_128(179,32,32,19)
	__mini_line_128(180,32,32,20)
	__mini_line_128(181,32,32,21)
	__mini_line_128(182,32,32,22)
	__mini_line_128(183,32,32,23)
	__mini_line_128(184,32,32,24)
	__mini_line_128(185,32,32,25)
	__mini_line_128(186,32,32,26)
	__mini_line_128(187,32,32,27)
	__mini_line_128(188,32,32,28)
	__mini_line_128(189,32,32,29)
	__mini_line_128(190,32,32,30)
	__mini_line_128(191,32,32,31)
	__mini_lin0_128(192,64)
	__mini_line_128(193,64,1,1)
	__mini_line_128(194,64,2,2)
	__mini_line_128(195,64,4,3)
	__mini_line_128(196,64,4,4)
	__mini_line_128(197,64,8,5)
	__mini_line_128(198,64,8,6)
	__mini_line_128(199,64,8,7)
	__mini_line_128(200,64,8,8)
	__mini_line_128(201,64,16,9)
	__mini_line_128(202,64,16,10)
	__mini_line_128(203,64,16,11)
	__mini_line_128(204,64,16,12)
	__mini_line_128(205,64,16,13)
	__mini_line_128(206,64,16,14)
	__mini_line_128(207,64,16,15)
	__mini_line_128(208,64,16,16)
	__mini_line_128(209,64,32,17)
	__mini_line_128(210,64,32,18)
	__mini_line_128(211,64,32,19)
	__mini_line_128(212,64,32,20)
	__mini_line_128(213,64,32,21)
	__mini_line_128(214,64,32,22)
	__mini_line_128(215,64,32,23)
	__mini_line_128(216,64,32,24)
	__mini_line_128(217,64,32,25)
	__mini_line_128(218,64,32,26)
	__mini_line_128(219,64,32,27)
	__mini_line_128(220,64,32,28)
	__mini_line_128(221,64,32,29)
	__mini_line_128(222,64,32,30)
	__mini_line_128(223,64,32,31)
	__mini_line_128(224,64,32,32)
	__mini_line_128(225,64,64,33)
	__mini_line_128(226,64,64,34)
	__mini_line_128(227,64,64,35)
	__mini_line_128(228,64,64,36)
	__mini_line_128(229,64,64,37)
	__mini_line_128(230,64,64,38)
	__mini_line_128(231,64,64,39)
	__mini_line_128(232,64,64,40)
	__mini_line_128(233,64,64,41)
	__mini_line_128(234,64,64,42)
	__mini_line_128(235,64,64,43)
	__mini_line_128(236,64,64,44)
	__mini_line_128(237,64,64,45)
	__mini_line_128(238,64,64,46)
	__mini_line_128(239,64,64,47)
	__mini_line_128(240,64,64,48)
	__mini_line_128(241,64,64,49)
	__mini_line_128(242,64,64,50)
	__mini_line_128(243,64,64,51)
	__mini_line_128(244,64,64,52)
	__mini_line_128(245,64,64,53)
	__mini_line_128(246,64,64,54)
	__mini_line_128(247,64,64,55)
	__mini_line_128(248,64,64,56)
	__mini_line_128(249,64,64,57)
	__mini_line_128(250,64,64,58)
	__mini_line_128(251,64,64,59)
	__mini_line_128(252,64,64,60)
	__mini_line_128(253,64,64,61)
	__mini_line_128(254,64,64,62)
	__mini_line_128(255,64,64,63)
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
	case 256: avx_copy_256u(dc - 256, sc - 256); break;
	case 257: avx_copy_256u(dc - 257, sc - 257);
	case 1: *dc = *sc; break;
	__mini_lin0_256(258,2)
	__mini_line_256(259,2,1,1)
	__mini_lin0_256(260,4)
	__mini_line_256(261,4,1,1)
	__mini_line_256(262,4,2,2)
	__mini_line_256(263,4,4,3)
	__mini_lin0_256(264,8)
	__mini_line_256(265,8,1,1)
	__mini_line_256(266,8,2,2)
	__mini_line_256(267,8,4,3)
	__mini_line_256(268,8,4,4)
	__mini_line_256(269,8,8,5)
	__mini_line_256(270,8,8,6)
	__mini_line_256(271,8,8,7)
	__mini_lin0_256(272,16)
	__mini_line_256(273,16,1,1)
	__mini_line_256(274,16,2,2)
	__mini_line_256(275,16,4,3)
	__mini_line_256(276,16,4,4)
	__mini_line_256(277,16,8,5)
	__mini_line_256(278,16,8,6)
	__mini_line_256(279,16,8,7)
	__mini_line_256(280,16,8,8)
	__mini_line_256(281,16,16,9)
	__mini_line_256(282,16,16,10)
	__mini_line_256(283,16,16,11)
	__mini_line_256(284,16,16,12)
	__mini_line_256(285,16,16,13)
	__mini_line_256(286,16,16,14)
	__mini_line_256(287,16,16,15)
	__mini_lin0_256(288,32)
	__mini_line_256(289,32,1,1)
	__mini_line_256(290,32,2,2)
	__mini_line_256(291,32,4,3)
	__mini_line_256(292,32,4,4)
	__mini_line_256(293,32,8,5)
	__mini_line_256(294,32,8,6)
	__mini_line_256(295,32,8,7)
	__mini_line_256(296,32,8,8)
	__mini_line_256(297,32,16,9)
	__mini_line_256(298,32,16,10)
	__mini_line_256(299,32,16,11)
	__mini_line_256(300,32,16,12)
	__mini_line_256(301,32,16,13)
	__mini_line_256(302,32,16,14)
	__mini_line_256(303,32,16,15)
	__mini_line_256(304,32,16,16)
	__mini_line_256(305,32,32,17)
	__mini_line_256(306,32,32,18)
	__mini_line_256(307,32,32,19)
	__mini_line_256(308,32,32,20)
	__mini_line_256(309,32,32,21)
	__mini_line_256(310,32,32,22)
	__mini_line_256(311,32,32,23)
	__mini_line_256(312,32,32,24)
	__mini_line_256(313,32,32,25)
	__mini_line_256(314,32,32,26)
	__mini_line_256(315,32,32,27)
	__mini_line_256(316,32,32,28)
	__mini_line_256(317,32,32,29)
	__mini_line_256(318,32,32,30)
	__mini_line_256(319,32,32,31)
	__mini_lin0_256(320,64)
	__mini_line_256(321,64,1,1)
	__mini_line_256(322,64,2,2)
	__mini_line_256(323,64,4,3)
	__mini_line_256(324,64,4,4)
	__mini_line_256(325,64,8,5)
	__mini_line_256(326,64,8,6)
	__mini_line_256(327,64,8,7)
	__mini_line_256(328,64,8,8)
	__mini_line_256(329,64,16,9)
	__mini_line_256(330,64,16,10)
	__mini_line_256(331,64,16,11)
	__mini_line_256(332,64,16,12)
	__mini_line_256(333,64,16,13)
	__mini_line_256(334,64,16,14)
	__mini_line_256(335,64,16,15)
	__mini_line_256(336,64,16,16)
	__mini_line_256(337,64,32,17)
	__mini_line_256(338,64,32,18)
	__mini_line_256(339,64,32,19)
	__mini_line_256(340,64,32,20)
	__mini_line_256(341,64,32,21)
	__mini_line_256(342,64,32,22)
	__mini_line_256(343,64,32,23)
	__mini_line_256(344,64,32,24)
	__mini_line_256(345,64,32,25)
	__mini_line_256(346,64,32,26)
	__mini_line_256(347,64,32,27)
	__mini_line_256(348,64,32,28)
	__mini_line_256(349,64,32,29)
	__mini_line_256(350,64,32,30)
	__mini_line_256(351,64,32,31)
	__mini_line_256(352,64,32,32)
	__mini_line_256(353,64,64,33)
	__mini_line_256(354,64,64,34)
	__mini_line_256(355,64,64,35)
	__mini_line_256(356,64,64,36)
	__mini_line_256(357,64,64,37)
	__mini_line_256(358,64,64,38)
	__mini_line_256(359,64,64,39)
	__mini_line_256(360,64,64,40)
	__mini_line_256(361,64,64,41)
	__mini_line_256(362,64,64,42)
	__mini_line_256(363,64,64,43)
	__mini_line_256(364,64,64,44)
	__mini_line_256(365,64,64,45)
	__mini_line_256(366,64,64,46)
	__mini_line_256(367,64,64,47)
	__mini_line_256(368,64,64,48)
	__mini_line_256(369,64,64,49)
	__mini_line_256(370,64,64,50)
	__mini_line_256(371,64,64,51)
	__mini_line_256(372,64,64,52)
	__mini_line_256(373,64,64,53)
	__mini_line_256(374,64,64,54)
	__mini_line_256(375,64,64,55)
	__mini_line_256(376,64,64,56)
	__mini_line_256(377,64,64,57)
	__mini_line_256(378,64,64,58)
	__mini_line_256(379,64,64,59)
	__mini_line_256(380,64,64,60)
	__mini_line_256(381,64,64,61)
	__mini_line_256(382,64,64,62)
	__mini_line_256(383,64,64,63)
	__mini_lin0_256(384,128)
	__mini_line_256(385,128,1,1)
	__mini_line_256(386,128,2,2)
	__mini_line_256(387,128,4,3)
	__mini_line_256(388,128,4,4)
	__mini_line_256(389,128,8,5)
	__mini_line_256(390,128,8,6)
	__mini_line_256(391,128,8,7)
	__mini_line_256(392,128,8,8)
	__mini_line_256(393,128,16,9)
	__mini_line_256(394,128,16,10)
	__mini_line_256(395,128,16,11)
	__mini_line_256(396,128,16,12)
	__mini_line_256(397,128,16,13)
	__mini_line_256(398,128,16,14)
	__mini_line_256(399,128,16,15)
	__mini_line_256(400,128,16,16)
	__mini_line_256(401,128,32,17)
	__mini_line_256(402,128,32,18)
	__mini_line_256(403,128,32,19)
	__mini_line_256(404,128,32,20)
	__mini_line_256(405,128,32,21)
	__mini_line_256(406,128,32,22)
	__mini_line_256(407,128,32,23)
	__mini_line_256(408,128,32,24)
	__mini_line_256(409,128,32,25)
	__mini_line_256(410,128,32,26)
	__mini_line_256(411,128,32,27)
	__mini_line_256(412,128,32,28)
	__mini_line_256(413,128,32,29)
	__mini_line_256(414,128,32,30)
	__mini_line_256(415,128,32,31)
	__mini_line_256(416,128,32,32)
	__mini_line_256(417,128,64,33)
	__mini_line_256(418,128,64,34)
	__mini_line_256(419,128,64,35)
	__mini_line_256(420,128,64,36)
	__mini_line_256(421,128,64,37)
	__mini_line_256(422,128,64,38)
	__mini_line_256(423,128,64,39)
	__mini_line_256(424,128,64,40)
	__mini_line_256(425,128,64,41)
	__mini_line_256(426,128,64,42)
	__mini_line_256(427,128,64,43)
	__mini_line_256(428,128,64,44)
	__mini_line_256(429,128,64,45)
	__mini_line_256(430,128,64,46)
	__mini_line_256(431,128,64,47)
	__mini_line_256(432,128,64,48)
	__mini_line_256(433,128,64,49)
	__mini_line_256(434,128,64,50)
	__mini_line_256(435,128,64,51)
	__mini_line_256(436,128,64,52)
	__mini_line_256(437,128,64,53)
	__mini_line_256(438,128,64,54)
	__mini_line_256(439,128,64,55)
	__mini_line_256(440,128,64,56)
	__mini_line_256(441,128,64,57)
	__mini_line_256(442,128,64,58)
	__mini_line_256(443,128,64,59)
	__mini_line_256(444,128,64,60)
	__mini_line_256(445,128,64,61)
	__mini_line_256(446,128,64,62)
	__mini_line_256(447,128,64,63)
	__mini_line_256(448,128,64,64)
	__mini_line_256(449,128,128,65)
	__mini_line_256(450,128,128,66)
	__mini_line_256(451,128,128,67)
	__mini_line_256(452,128,128,68)
	__mini_line_256(453,128,128,69)
	__mini_line_256(454,128,128,70)
	__mini_line_256(455,128,128,71)
	__mini_line_256(456,128,128,72)
	__mini_line_256(457,128,128,73)
	__mini_line_256(458,128,128,74)
	__mini_line_256(459,128,128,75)
	__mini_line_256(460,128,128,76)
	__mini_line_256(461,128,128,77)
	__mini_line_256(462,128,128,78)
	__mini_line_256(463,128,128,79)
	__mini_line_256(464,128,128,80)
	__mini_line_256(465,128,128,81)
	__mini_line_256(466,128,128,82)
	__mini_line_256(467,128,128,83)
	__mini_line_256(468,128,128,84)
	__mini_line_256(469,128,128,85)
	__mini_line_256(470,128,128,86)
	__mini_line_256(471,128,128,87)
	__mini_line_256(472,128,128,88)
	__mini_line_256(473,128,128,89)
	__mini_line_256(474,128,128,90)
	__mini_line_256(475,128,128,91)
	__mini_line_256(476,128,128,92)
	__mini_line_256(477,128,128,93)
	__mini_line_256(478,128,128,94)
	__mini_line_256(479,128,128,95)
	__mini_line_256(480,128,128,96)
	__mini_line_256(481,128,128,97)
	__mini_line_256(482,128,128,98)
	__mini_line_256(483,128,128,99)
	__mini_line_256(484,128,128,100)
	__mini_line_256(485,128,128,101)
	__mini_line_256(486,128,128,102)
	__mini_line_256(487,128,128,103)
	__mini_line_256(488,128,128,104)
	__mini_line_256(489,128,128,105)
	__mini_line_256(490,128,128,106)
	__mini_line_256(491,128,128,107)
	__mini_line_256(492,128,128,108)
	__mini_line_256(493,128,128,109)
	__mini_line_256(494,128,128,110)
	__mini_line_256(495,128,128,111)
	__mini_line_256(496,128,128,112)
	__mini_line_256(497,128,128,113)
	__mini_line_256(498,128,128,114)
	__mini_line_256(499,128,128,115)
	__mini_line_256(500,128,128,116)
	__mini_line_256(501,128,128,117)
	__mini_line_256(502,128,128,118)
	__mini_line_256(503,128,128,119)
	__mini_line_256(504,128,128,120)
	__mini_line_256(505,128,128,121)
	__mini_line_256(506,128,128,122)
	__mini_line_256(507,128,128,123)
	__mini_line_256(508,128,128,124)
	__mini_line_256(509,128,128,125)
	__mini_line_256(510,128,128,126)
	__mini_line_256(511,128,128,127)
	case 512: avx_copy_512u(dest, src); break;
	default:
		return in_memcpy(dest, src, len);
	}
}

static void* in_memcpy512(void* dest, const void* src, size_t len) {
	if (!dest || !src)
		return nullptr;

	if (len <= 512)
		return in_minicpy512(dest, src, len);

	const size_t toAlign = fast_mod512(len);

	//64 byte align
	in_minicpy256(dest, src, toAlign);
	char *dst_c = (char*) dest + toAlign;
	const char* src_c = (const char*) src + toAlign;
	size_t lCopy = toAlign;

	//bulk copy
	if (((size_t)src) & 511)
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