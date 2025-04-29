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


#elif defined(__aarch64__) || defined(_M_ARM64)
#include <arm_neon.h>
#define ZEND_HAVE_VECTOR_128

typedef int8x16_t __m128i;

#define _mm_setzero_si128() vdupq_n_s8(0)
#define _mm_set1_epi8(x) vdupq_n_s8(x)
#define _mm_set_epi16(x0, x1, x2, x3, x4, x5, x6, x7) \
	vreinterpretq_s8_s16((int16x8_t) { \
		(int16_t) (x7), (int16_t) (x6), (int16_t) (x5), (int16_t) (x4), \
		(int16_t) (x3), (int16_t) (x2), (int16_t) (x1), (int16_t) (x0) })
#define _mm_set_epi32(x0, x1, x2, x3) \
	vreinterpretq_s8_s32((int32x4_t) { (int32_t) (x3), (int32_t) (x2), (int32_t) (x1), (int32_t) (x0) })
#define _mm_set_epi64x(x0, x1) vreinterpretq_s8_s64((int64x2_t) { (int64_t) (x1), (int64_t) (x0) })
#define _mm_load_si128(x) vld1q_s8((const int8_t *) (x))
#define _mm_loadu_si128(x) _mm_load_si128(x)
#define _mm_store_si128(to, x) vst1q_s8((int8_t *) (to), x)
#define _mm_storeu_si128(to, x) _mm_store_si128(to, x)

#define _mm_or_si128(a, b) vorrq_s8(a, b)
#define _mm_xor_si128(a, b) veorq_s8(a, b)
#define _mm_and_si128(a, b) vandq_s8(a, b)

#define _mm_slli_si128(x, imm) \
    ((imm) >= 16 ? vdupq_n_s8(0) : \
     vreinterpretq_s8_u8(vextq_u8(vdupq_n_u8(0), vreinterpretq_u8_s8(x), 16 - (imm))))
#define _mm_srli_si128(x, imm) \
    ((imm) >= 16 ? vdupq_n_s8(0) : \
     vreinterpretq_s8_u8(vextq_u8(vreinterpretq_u8_s8(x), vdupq_n_u8(0), (imm))))

#define _mm_add_epi8(a, b) vaddq_s8(a, b)

#define _mm_cmpeq_epi8(a, b) (vreinterpretq_s8_u8(vceqq_s8(a, b)))
#define _mm_cmplt_epi8(a, b) (vreinterpretq_s8_u8(vcltq_s8(a, b)))
#define _mm_cmpgt_epi8(a, b) (vreinterpretq_s8_u8(vcgtq_s8(a, b)))

static zend_always_inline int _mm_movemask_epi8(__m128i x)
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
