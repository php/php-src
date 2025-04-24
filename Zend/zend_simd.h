/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Saki Takamachi <saki@php.net>                               |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_SIMD_H
#define ZEND_SIMD_H

#ifdef __SSE2__
#include <emmintrin.h>
#define ZEND_HAVE_VECTOR_128

typedef __m128i zend_vec_8x16_t;
typedef __m128i zend_vec_16x8_t;
typedef __m128i zend_vec_32x4_t;
typedef __m128i zend_vec_64x2_t;

#define zend_vec_setzero_8x16() _mm_setzero_si128()
#define zend_vec_set_8x16(x) _mm_set1_epi8(x)
#define zend_vec_set_8x16_from_16x8(x0, x1, x2, x3, x4, x5, x6, x7) _mm_set_epi16(x0, x1, x2, x3, x4, x5, x6, x7)
#define zend_vec_set_8x16_from_32x4(x0, x1, x2, x3) _mm_set_epi32(x0, x1, x2, x3)
#define zend_vec_set_8x16_from_64x2(x0, x1) _mm_set_epi64(x0, x1)
#define zend_vec_load_8x16(x) _mm_load_si128((const __m128i *) (x))
#define zend_vec_loadu_8x16(x) _mm_loadu_si128((const __m128i *) (x))
#define zend_vec_store_8x16(to, x) _mm_store_si128((__m128i *) (to), x)
#define zend_vec_storeu_8x16(to, x) _mm_storeu_si128((__m128i *) (to), x)

#define zend_vec_or_8x16(a, b) _mm_or_si128(a, b)
#define zend_vec_xor_8x16(a, b) _mm_xor_si128(a, b)
#define zend_vec_and_8x16(a, b) _mm_and_si128(a, b)
#define zend_vec_rshift_128_from_8x16(x, bytes) _mm_srli_si128(x, bytes)
#define zend_vec_lshift_128_from_8x16(x, bytes) _mm_slli_si128(x, bytes)

#define zend_vec_add_8x16(a, b) _mm_add_epi8(a, b)

#define zend_vec_cmpeq_8x16(a, b) _mm_cmpeq_epi8(a, b)
#define zend_vec_cmplt_8x16(a, b) _mm_cmplt_epi8(a, b)
#define zend_vec_cmpgt_8x16(a, b) _mm_cmpgt_epi8(a, b)

#define zend_vec_movemask_8x16(x) _mm_movemask_epi8(x)


#elif defined(__aarch64__) || defined(_M_ARM64)
#include <arm_neon.h>
#define ZEND_HAVE_VECTOR_128

typedef int8x16_t zend_vec_8x16_t;
typedef int16x8_t zend_vec_16x8_t;
typedef int32x4_t zend_vec_32x4_t;
typedef int64x2_t zend_vec_64x2_t;

#define zend_vec_setzero_8x16() vdupq_n_s8(0)
#define zend_vec_set_8x16(x) vdupq_n_s8(x)
#define zend_vec_set_8x16_from_16x8(x0, x1, x2, x3, x4, x5, x6, x7) \
	vreinterpretq_s8_s16((int16x8_t) { \
		(int16_t) (x7), (int16_t) (x6), (int16_t) (x5), (int16_t) (x4), \
		(int16_t) (x3), (int16_t) (x2), (int16_t) (x1), (int16_t) (x0) })
#define zend_vec_set_8x16_from_32x4(x0, x1, x2, x3) \
	vreinterpretq_s8_s32((int32x4_t) { (int32_t) (x3), (int32_t) (x2), (int32_t) (x1), (int32_t) (x0) })
#define zend_vec_set_8x16_from_64x2(x0, x1) vreinterpretq_s8_s64((int64x2_t) { (int64_t) (x1), (int64_t) (x0) })
#define zend_vec_load_8x16(x) vld1q_s8((const int8_t *) (x))
#define zend_vec_loadu_8x16(x) zend_vec_load_8x16(x)
#define zend_vec_store_8x16(to, x) vst1q_s8((int8_t *) (to), x)
#define zend_vec_storeu_8x16(to, x) zend_vec_store_8x16(to, x)

#define zend_vec_or_8x16(a, b) vorrq_s8(a, b)
#define zend_vec_xor_8x16(a, b) veorq_s8(a, b)
#define zend_vec_and_8x16(a, b) vandq_s8(a, b)
#define zend_vec_rshift_128_from_8x16(x, bytes) vreinterpretq_s8_u8(vextq_u8(vdupq_n_u8(0), vreinterpretq_u8_s8(x), bytes))
#define zend_vec_lshift_128_from_8x16(x, bytes) vreinterpretq_s8_u8(vextq_u8(vreinterpretq_u8_s8(x), vdupq_n_u8(0), 16 - bytes))

#define zend_vec_add_8x16(a, b) vaddq_s8(a, b)

#define zend_vec_cmpeq_8x16(a, b) (vreinterpretq_s8_u8(vceqq_s8(a, b)))
#define zend_vec_cmplt_8x16(a, b) (vreinterpretq_s8_u8(vcltq_s8(a, b)))
#define zend_vec_cmpgt_8x16(a, b) (vreinterpretq_s8_u8(vcgtq_s8(a, b)))

static zend_always_inline int zend_vec_movemask_8x16(int8x16_t x)
{
    /**
     * based on code from
     * https://community.arm.com/arm-community-blogs/b/servers-and-cloud-computing-blog/posts/porting-x86-vector-bitmask-optimizations-to-arm-neon
     */
    uint16x8_t high_bits = vreinterpretq_u16_u8(vshrq_n_u8(vreinterpretq_u8_s8(x), 7));
    uint32x4_t paired16 = vreinterpretq_u32_u16(vsraq_n_u16(high_bits, high_bits, 7));
    uint64x2_t paired32 = vreinterpretq_u64_u32(vsraq_n_u32(paired16, paired16, 14));
    uint8x16_t paired64 = vreinterpretq_u8_u64(vsraq_n_u64(paired32, paired32, 28));
    return vgetq_lane_u8(paired64, 0) | ((int) vgetq_lane_u8(paired64, 8) << 8);
}

#endif

#endif       /* ZEND_SIMD_H */
