/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Saki Takamachi <saki@php.net>                               |
   +----------------------------------------------------------------------+
*/


#ifndef _BCMATH_SIMD_H_
#define _BCMATH_SIMD_H_

#ifdef __SSE2__
# include <emmintrin.h>
  typedef __m128i bc_simd_128_t;
# define HAVE_BC_SIMD_128
# define bc_simd_set_8x16(x) _mm_set1_epi8(x)
# define bc_simd_load_8x16(ptr) _mm_loadu_si128((const __m128i *) (ptr))
# define bc_simd_xor_8x16(a, b) _mm_xor_si128(a, b)
# define bc_simd_store_8x16(ptr, val) _mm_storeu_si128((__m128i *) (ptr), val)
# define bc_simd_add_8x16(a, b) _mm_add_epi8(a, b)
# define bc_simd_cmpeq_8x16(a, b) _mm_cmpeq_epi8(a, b)
# define bc_simd_cmplt_8x16(a, b) _mm_cmplt_epi8(a, b)
# define bc_simd_movemask_8x16(a) _mm_movemask_epi8(a)

#elif defined(__aarch64__) || defined(_M_ARM64)
# include <arm_neon.h>
  typedef int8x16_t bc_simd_128_t;
# define HAVE_BC_SIMD_128
# define bc_simd_set_8x16(x) vdupq_n_s8(x)
# define bc_simd_load_8x16(ptr) vld1q_s8((const int8_t *) (ptr))
# define bc_simd_xor_8x16(a, b) veorq_s8(a, b)
# define bc_simd_store_8x16(ptr, val) vst1q_s8((int8_t *) (ptr), val)
# define bc_simd_add_8x16(a, b) vaddq_s8(a, b)
# define bc_simd_cmpeq_8x16(a, b) (vreinterpretq_s8_u8(vceqq_s8(a, b)))
# define bc_simd_cmplt_8x16(a, b) (vreinterpretq_s8_u8(vcltq_s8(a, b)))
  static inline int bc_simd_movemask_8x16(int8x16_t vec)
  {
      /**
       * based on code from
       * https://community.arm.com/arm-community-blogs/b/servers-and-cloud-computing-blog/posts/porting-x86-vector-bitmask-optimizations-to-arm-neon
       */
      uint16x8_t high_bits = vreinterpretq_u16_u8(vshrq_n_u8(vreinterpretq_u8_s8(vec), 7));
      uint32x4_t paired16 = vreinterpretq_u32_u16(vsraq_n_u16(high_bits, high_bits, 7));
      uint64x2_t paired32 = vreinterpretq_u64_u32(vsraq_n_u32(paired16, paired16, 14));
      uint8x16_t paired64 = vreinterpretq_u8_u64(vsraq_n_u64(paired32, paired32, 28));
      return vgetq_lane_u8(paired64, 0) | ((int) vgetq_lane_u8(paired64, 8) << 8);
  }
#endif

#endif
