/********************************************************************************
 * MIT License
 * Copyright (c) 2025 Saki Takamachi <saki@sakiot.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *********************************************************************************/


 #ifndef XSSE_H
 #define XSSE_H

 #define XSSE_VERSION 10000

 #ifdef _MSC_VER
 #  define XSSE_FORCE_INLINE __forceinline
 #elif defined(__GNUC__) || defined(__clang__)
 #  define XSSE_FORCE_INLINE inline __attribute__((always_inline))
 #  define XSSE_HAS_MACRO_EXTENSION
 #else
 #  define XSSE_FORCE_INLINE inline
 #endif


 #if defined(__SSE2__) || defined(_M_X64) || defined(_M_AMD64)
 #include <emmintrin.h>
 #define XSSE2


 #elif defined(__aarch64__) || defined(_M_ARM64)
 #include <arm_neon.h>
 #define XSSE2

 typedef int8x16_t __m128i;


 /*****************************************************************************
  * Load / Store                                                              *
  *****************************************************************************/

 #define _mm_set_epi8(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15) \
	 ((int8x16_t) { \
		 (int8_t) (x15), (int8_t) (x14), (int8_t) (x13), (int8_t) (x12), \
		 (int8_t) (x11), (int8_t) (x10), (int8_t) (x9),  (int8_t) (x8), \
		 (int8_t) (x7),  (int8_t) (x6),  (int8_t) (x5),  (int8_t) (x4), \
		 (int8_t) (x3),  (int8_t) (x2),  (int8_t) (x1),  (int8_t) (x0) })
 #define _mm_set_epi16(x0, x1, x2, x3, x4, x5, x6, x7) \
	 (vreinterpretq_s8_s16((int16x8_t) { \
		 (int16_t) (x7), (int16_t) (x6), (int16_t) (x5), (int16_t) (x4), \
		 (int16_t) (x3), (int16_t) (x2), (int16_t) (x1), (int16_t) (x0) }))
 #define _mm_set_epi32(x0, x1, x2, x3) \
	 (vreinterpretq_s8_s32((int32x4_t) { (int32_t) (x3), (int32_t) (x2), (int32_t) (x1), (int32_t) (x0) }))
 #define _mm_set_epi64x(x0, x1) (vreinterpretq_s8_s64((int64x2_t) { (int64_t) (x1), (int64_t) (x0) }))
 #define _mm_set1_epi8(x) (vdupq_n_s8((int8_t) (x)))
 #define _mm_set1_epi16(x) (vreinterpretq_s8_s16(vdupq_n_s16((int16_t) (x))))
 #define _mm_set1_epi32(x) (vreinterpretq_s8_s32(vdupq_n_s32((int32_t) (x))))
 #define _mm_set1_epi64x(x) (vreinterpretq_s8_s64(vdupq_n_s64((int64_t) (x))))

 #define _mm_setr_epi8(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15) \
	 ((int8x16_t) { \
		 (int8_t) (x0), (int8_t) (x1), (int8_t) (x2), (int8_t) (x3), \
		 (int8_t) (x4), (int8_t) (x5), (int8_t) (x6), (int8_t) (x7), \
		 (int8_t) (x8), (int8_t) (x9), (int8_t) (x10), (int8_t) (x11), \
		 (int8_t) (x12), (int8_t) (x13), (int8_t) (x14), (int8_t) (x15) })
 #define _mm_setr_epi16(x0, x1, x2, x3, x4, x5, x6, x7) \
	 (vreinterpretq_s8_s16((int16x8_t) { \
		 (int16_t) (x0), (int16_t) (x1), (int16_t) (x2), (int16_t) (x3), \
		 (int16_t) (x4), (int16_t) (x5), (int16_t) (x6), (int16_t) (x7) }))
 #define _mm_setr_epi32(x0, x1, x2, x3) \
	 (vreinterpretq_s8_s32((int32x4_t) { (int32_t) (x0), (int32_t) (x1), (int32_t) (x2), (int32_t) (x3) }))

 #define _mm_setzero_si128() (vdupq_n_s8(0))

 #define _mm_load_si128(x) (vld1q_s8((const int8_t *) (x)))
 #define _mm_loadu_si128(x) _mm_load_si128(x)

 #define _mm_store_si128(to, x) (vst1q_s8((int8_t *) (to), x))
 #define _mm_storeu_si128(to, x) _mm_store_si128(to, x)
 #define _mm_stream_si128(to, x) _mm_store_si128(to, x)
 #define _mm_stream_si32(to, x) (*(volatile int32_t *)(to) = (int32_t)(x))


 /*****************************************************************************
  * Bit shift / Bit wise                                                      *
  *****************************************************************************/

 #define _mm_or_si128(a, b) (vorrq_s8((a), (b)))
 #define _mm_xor_si128(a, b) (veorq_s8((a), (b)))
 #define _mm_and_si128(a, b) (vandq_s8((a), (b)))
 #define _mm_andnot_si128(a, b) (vbicq_s8((b), (a)))

 #define _mm_slli_epi16(x, count) (vreinterpretq_s8_u16(vshlq_n_u16(vreinterpretq_u16_s8(x), (count))))
 #define _mm_slli_epi32(x, count) (vreinterpretq_s8_u32(vshlq_n_u32(vreinterpretq_u32_s8(x), (count))))
 #define _mm_slli_epi64(x, count) (vreinterpretq_s8_u64(vshlq_n_u64(vreinterpretq_u64_s8(x), (count))))
 static XSSE_FORCE_INLINE __m128i _mm_sll_epi16(__m128i x, __m128i count)
 {
	 uint16_t shift = (uint16_t) (vgetq_lane_s64(vreinterpretq_s64_s8(count), 0) & 0xFFFF);
	 return vreinterpretq_s8_u16(
		 vshlq_u16(vreinterpretq_u16_s8(x), vdupq_n_s16((int16_t) shift))
	 );
 }
 static XSSE_FORCE_INLINE __m128i _mm_sll_epi32(__m128i x, __m128i count)
 {
	 uint32_t shift = (uint32_t) (vgetq_lane_s64(vreinterpretq_s64_s8(count), 0) & 0xFFFFFFFF);
	 return vreinterpretq_s8_u32(
		 vshlq_u32(vreinterpretq_u32_s8(x), vdupq_n_s32((int32_t) shift))
	 );
 }
 static XSSE_FORCE_INLINE __m128i _mm_sll_epi64(__m128i x, __m128i count)
 {
	 uint64_t shift = (uint64_t) vgetq_lane_s64(vreinterpretq_s64_s8(count), 0);
	 return vreinterpretq_s8_u64(
		 vshlq_u64(vreinterpretq_u64_s8(x), vdupq_n_s64((int64_t) shift))
	 );
 }

 #define _mm_slli_si128(x, imm) \
	 ((imm) >= 16 ? vdupq_n_s8(0) : vreinterpretq_s8_u8(vextq_u8(vdupq_n_u8(0), vreinterpretq_u8_s8(x), 16 - (imm))))

 #define _mm_srai_epi16(x, count) (vreinterpretq_s8_s16(vshrq_n_s16(vreinterpretq_s16_s8(x), (count))))
 #define _mm_srai_epi32(x, count) (vreinterpretq_s8_s32(vshrq_n_s32(vreinterpretq_s32_s8(x), (count))))
 static inline __m128i _mm_sra_epi16(__m128i x, __m128i count)
 {
	 uint16_t shift = (uint16_t) (vgetq_lane_s64(vreinterpretq_s64_s8(count), 0) & 0xFFFF);
	 return vreinterpretq_s8_s16(
		 vshlq_s16(vreinterpretq_s16_s8(x), vdupq_n_s16(-(int16_t) shift))
	 );
 }
 static inline __m128i _mm_sra_epi32(__m128i x, __m128i count)
 {
	 uint32_t shift = (uint32_t) (vgetq_lane_s64(vreinterpretq_s64_s8(count), 0) & 0xFFFFFFFF);
	 return vreinterpretq_s8_s32(
		 vshlq_s32(vreinterpretq_s32_s8(x), vdupq_n_s32(-(int32_t) shift))
	 );
 }

 #define _mm_srli_epi16(x, count) (vreinterpretq_s8_u16(vshrq_n_u16(vreinterpretq_u16_s8(x), (count))))
 #define _mm_srli_epi32(x, count) (vreinterpretq_s8_u32(vshrq_n_u32(vreinterpretq_u32_s8(x), (count))))
 #define _mm_srli_epi64(x, count) (vreinterpretq_s8_u64(vshrq_n_u64(vreinterpretq_u64_s8(x), (count))))
 static XSSE_FORCE_INLINE __m128i _mm_srl_epi16(__m128i x, __m128i count)
 {
	 uint16_t shift = (uint16_t) (vgetq_lane_s64(vreinterpretq_s64_s8(count), 0) & 0xFFFF);
	 return vreinterpretq_s8_u16(
		 vshlq_u16(vreinterpretq_u16_s8(x), vdupq_n_s16(-(int16_t) shift))
	 );
 }
 static XSSE_FORCE_INLINE __m128i _mm_srl_epi32(__m128i x, __m128i count)
 {
	 uint32_t shift = (uint32_t) (vgetq_lane_s64(vreinterpretq_s64_s8(count), 0) & 0xFFFFFFFF);
	 return vreinterpretq_s8_u32(
		 vshlq_u32(vreinterpretq_u32_s8(x), vdupq_n_s32(-(int32_t) shift))
	 );
 }
 static XSSE_FORCE_INLINE __m128i _mm_srl_epi64(__m128i x, __m128i count)
 {
	 uint64_t shift = (uint64_t) vgetq_lane_s64(vreinterpretq_s64_s8(count), 0);
	 return vreinterpretq_s8_u64(
		 vshlq_u64(vreinterpretq_u64_s8(x), vdupq_n_s64(-(int64_t) shift))
	 );
 }

 #define _mm_srli_si128(x, imm) \
	 ((imm) >= 16 ? vdupq_n_s8(0) : vreinterpretq_s8_u8(vextq_u8(vreinterpretq_u8_s8(x), vdupq_n_u8(0), (imm))))


 /*****************************************************************************
  * Integer Arithmetic Operations                                             *
  *****************************************************************************/

 /**
  * In practice, there is no problem, but a runtime error for signed integer overflow is triggered by UBSAN,
  * so perform the calculation as unsigned. Since it is optimized at compile time, there are no unnecessary casts at runtime.
  */
 #define _mm_add_epi8(a, b) (vreinterpretq_s8_u8(vaddq_u8(vreinterpretq_u8_s8(a), vreinterpretq_u8_s8(b))))
 #define _mm_add_epi16(a, b) (vreinterpretq_s8_u16(vaddq_u16(vreinterpretq_u16_s8(a), vreinterpretq_u16_s8(b))))
 #define _mm_add_epi32(a, b) (vreinterpretq_s8_u32(vaddq_u32(vreinterpretq_u32_s8(a), vreinterpretq_u32_s8(b))))
 #define _mm_add_epi64(a, b) (vreinterpretq_s8_u64(vaddq_u64(vreinterpretq_u64_s8(a), vreinterpretq_u64_s8(b))))

 #define _mm_adds_epi8(a, b) (vqaddq_s8((a), (b)))
 #define _mm_adds_epi16(a, b) (vreinterpretq_s8_s16(vqaddq_s16(vreinterpretq_s16_s8(a), vreinterpretq_s16_s8(b))))
 #define _mm_adds_epu8(a, b) (vreinterpretq_s8_u8(vqaddq_u8(vreinterpretq_u8_s8(a), vreinterpretq_u8_s8(b))))
 #define _mm_adds_epu16(a, b) (vreinterpretq_s8_u16(vqaddq_u16(vreinterpretq_u16_s8(a), vreinterpretq_u16_s8(b))))

 #define _mm_avg_epu8(a, b) (vreinterpretq_s8_u8(vrhaddq_u8(vreinterpretq_u8_s8(a), vreinterpretq_u8_s8(b))))
 #define _mm_avg_epu16(a, b) (vreinterpretq_s8_u16(vrhaddq_u16(vreinterpretq_u16_s8(a), vreinterpretq_u16_s8(b))))

 static XSSE_FORCE_INLINE __m128i _mm_madd_epi16(__m128i a, __m128i b)
 {
	 int32x4_t mul_lo = vmull_s16(vget_low_s16(vreinterpretq_s16_s8(a)), vget_low_s16(vreinterpretq_s16_s8(b)));
	 int32x4_t mul_hi = vmull_s16(vget_high_s16(vreinterpretq_s16_s8(a)), vget_high_s16(vreinterpretq_s16_s8(b)));

	 return vreinterpretq_s8_s32(vcombine_s32(
		 vpadd_s32(vget_low_s32(mul_lo), vget_high_s32(mul_lo)),
		 vpadd_s32(vget_low_s32(mul_hi), vget_high_s32(mul_hi))
	 ));
 }

 #define _mm_max_epu8(a, b) (vreinterpretq_s8_u8(vmaxq_u8(vreinterpretq_u8_s8(a), vreinterpretq_u8_s8(b))))
 #define _mm_max_epi16(a, b) (vreinterpretq_s8_s16(vmaxq_s16(vreinterpretq_s16_s8(a), vreinterpretq_s16_s8(b))))
 #define _mm_min_epu8(a, b) (vreinterpretq_s8_u8(vminq_u8(vreinterpretq_u8_s8(a), vreinterpretq_u8_s8(b))))
 #define _mm_min_epi16(a, b) (vreinterpretq_s8_s16(vminq_s16(vreinterpretq_s16_s8(a), vreinterpretq_s16_s8(b))))

 static XSSE_FORCE_INLINE __m128i _mm_mulhi_epi16(__m128i a, __m128i b)
 {
	 int32x4_t lo = vmull_s16(vget_low_s16(vreinterpretq_s16_s8(a)), vget_low_s16(vreinterpretq_s16_s8(b)));
	 int32x4_t hi = vmull_s16(vget_high_s16(vreinterpretq_s16_s8(a)), vget_high_s16(vreinterpretq_s16_s8(b)));
	 return vreinterpretq_s8_s16(vcombine_s16(vshrn_n_s32(lo, 16), vshrn_n_s32(hi, 16)));
 }
 static XSSE_FORCE_INLINE __m128i _mm_mulhi_epu16(__m128i a, __m128i b)
 {
	 uint32x4_t lo = vmull_u16(vget_low_u16(vreinterpretq_u16_s8(a)), vget_low_u16(vreinterpretq_u16_s8(b)));
	 uint32x4_t hi = vmull_u16(vget_high_u16(vreinterpretq_u16_s8(a)), vget_high_u16(vreinterpretq_u16_s8(b)));
	 return vreinterpretq_s8_u16(vcombine_u16(vshrn_n_u32(lo, 16), vshrn_n_u32(hi, 16)));
 }
 static XSSE_FORCE_INLINE __m128i _mm_mullo_epi16(__m128i a, __m128i b)
 {
	 int32x4_t lo = vmull_s16(vget_low_s16(vreinterpretq_s16_s8(a)), vget_low_s16(vreinterpretq_s16_s8(b)));
	 int32x4_t hi = vmull_s16(vget_high_s16(vreinterpretq_s16_s8(a)), vget_high_s16(vreinterpretq_s16_s8(b)));
	 return vreinterpretq_s8_s16(vcombine_s16(vmovn_s32(lo), vmovn_s32(hi)));
 }
 static XSSE_FORCE_INLINE __m128i _mm_mul_epu32(__m128i a, __m128i b)
 {
	 uint32x4_t evens = vuzpq_u32(vreinterpretq_u32_s8(a), vreinterpretq_u32_s8(b)).val[0];
	 return vreinterpretq_s8_u64(vmull_u32(vget_low_u32(evens), vget_high_u32(evens)));
 }
 static XSSE_FORCE_INLINE __m128i _mm_sad_epu8(__m128i a, __m128i b)
 {
	 uint16x8_t abs_diffs_16 = vpaddlq_u8(vabdq_u8(vreinterpretq_u8_s8(a), vreinterpretq_u8_s8(b)));
	 uint32x4_t abs_diffs_32 = vpaddlq_u16(abs_diffs_16);
	 uint64x2_t abs_diffs_64 = vpaddlq_u32(abs_diffs_32);

	 return vreinterpretq_s8_u16((uint16x8_t) {
		 (int16_t) vgetq_lane_u64(abs_diffs_64, 0), 0, 0, 0,
		 (int16_t) vgetq_lane_u64(abs_diffs_64, 1), 0, 0, 0
	 });
 }

 #define _mm_sub_epi8(a, b) (vreinterpretq_s8_u8(vsubq_u8(vreinterpretq_u8_s8(a), vreinterpretq_u8_s8(b))))
 #define _mm_sub_epi16(a, b) (vreinterpretq_s8_u16(vsubq_u16(vreinterpretq_u16_s8(a), vreinterpretq_u16_s8(b))))
 #define _mm_sub_epi32(a, b) (vreinterpretq_s8_u32(vsubq_u32(vreinterpretq_u32_s8(a), vreinterpretq_u32_s8(b))))
 #define _mm_sub_epi64(a, b) (vreinterpretq_s8_u64(vsubq_u64(vreinterpretq_u64_s8(a), vreinterpretq_u64_s8(b))))

 #define _mm_subs_epi8(a, b) (vqsubq_s8((a), (b)))
 #define _mm_subs_epi16(a, b) (vreinterpretq_s8_s16(vqsubq_s16(vreinterpretq_s16_s8(a), vreinterpretq_s16_s8(b))))
 #define _mm_subs_epu8(a, b) (vreinterpretq_s8_u8(vqsubq_u8(vreinterpretq_u8_s8(a), vreinterpretq_u8_s8(b))))
 #define _mm_subs_epu16(a, b) (vreinterpretq_s8_u16(vqsubq_u16(vreinterpretq_u16_s8(a), vreinterpretq_u16_s8(b))))


 /*****************************************************************************
  * Comparison                                                                *
  *****************************************************************************/

 #define _mm_cmpeq_epi8(a, b) (vreinterpretq_s8_u8(vceqq_s8((a), (b))))
 #define _mm_cmpeq_epi16(a, b) (vreinterpretq_s8_u16(vceqq_s16(vreinterpretq_s16_s8(a), vreinterpretq_s16_s8(b))))
 #define _mm_cmpeq_epi32(a, b) (vreinterpretq_s8_u32(vceqq_s32(vreinterpretq_s32_s8(a), vreinterpretq_s32_s8(b))))

 #define _mm_cmplt_epi8(a, b) (vreinterpretq_s8_u8(vcltq_s8((a), (b))))
 #define _mm_cmplt_epi16(a, b) (vreinterpretq_s8_u16(vcltq_s16(vreinterpretq_s16_s8(a), vreinterpretq_s16_s8(b))))
 #define _mm_cmplt_epi32(a, b) (vreinterpretq_s8_u32(vcltq_s32(vreinterpretq_s32_s8(a), vreinterpretq_s32_s8(b))))

 #define _mm_cmpgt_epi8(a, b) (vreinterpretq_s8_u8(vcgtq_s8((a), (b))))
 #define _mm_cmpgt_epi16(a, b) (vreinterpretq_s8_u16(vcgtq_s16(vreinterpretq_s16_s8(a), vreinterpretq_s16_s8(b))))
 #define _mm_cmpgt_epi32(a, b) (vreinterpretq_s8_u32(vcgtq_s32(vreinterpretq_s32_s8(a), vreinterpretq_s32_s8(b))))


 /*****************************************************************************
  * Convert                                                                   *
  *****************************************************************************/

 #define _mm_cvtsi32_si128(x) (vreinterpretq_s8_s32((int32x4_t) { (int32_t) (x), 0, 0, 0 }))
 #define _mm_cvtsi64_si128(x) (vreinterpretq_s8_s64((int64x2_t) { (int64_t) (x), 0 }))
 #define _mm_cvtsi128_si32(x) (vgetq_lane_s32(vreinterpretq_s32_s8(x), 0))
 #define _mm_cvtsi128_si64(x) (vgetq_lane_s64(vreinterpretq_s64_s8(x), 0))


 /*****************************************************************************
  * Others                                                                    *
  *****************************************************************************/

 #define _mm_packs_epi16(a, b) (vcombine_s8(vqmovn_s16(vreinterpretq_s16_s8(a)), vqmovn_s16(vreinterpretq_s16_s8(b))))
 #define _mm_packs_epi32(a, b) \
	 (vreinterpretq_s8_s16(vcombine_s16(vqmovn_s32(vreinterpretq_s32_s8(a)), vqmovn_s32(vreinterpretq_s32_s8(b)))))
 #define _mm_packus_epi16(a, b) \
	 (vreinterpretq_s8_u8(vcombine_u8(vqmovun_s16(vreinterpretq_s16_s8(a)), vqmovun_s16(vreinterpretq_s16_s8(b)))))

 #define _mm_extract_epi16(x, imm) (vgetq_lane_s16(vreinterpretq_s16_s8(x), (imm)))
 #define _mm_insert_epi16(x, val, imm) (vreinterpretq_s8_s16(vsetq_lane_s16((int16_t) (val), vreinterpretq_s16_s8(x), (imm))))

 static XSSE_FORCE_INLINE int _mm_movemask_epi8(__m128i x)
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

 #define _MM_SHUFFLE(a, b, c, d) (((a) << 6) | ((b) << 4) | ((c) << 2) | (d))
 #ifdef XSSE_HAS_MACRO_EXTENSION
 #define _mm_shuffle_epi32(x, imm) __extension__({ \
		 int32x4_t __xsse_tmp = vreinterpretq_s32_s8(x); \
		 vreinterpretq_s8_s32((int32x4_t) { \
			 (int32_t) vgetq_lane_s32(__xsse_tmp, ((imm) >> 0) & 0x3), \
			 (int32_t) vgetq_lane_s32(__xsse_tmp, ((imm) >> 2) & 0x3), \
			 (int32_t) vgetq_lane_s32(__xsse_tmp, ((imm) >> 4) & 0x3), \
			 (int32_t) vgetq_lane_s32(__xsse_tmp, ((imm) >> 6) & 0x3) \
		 }); \
	 })
 #define _mm_shufflehi_epi16(x, imm) __extension__({ \
		 int16x8_t __xsse_tmp = vreinterpretq_s16_s8(x); \
		 vreinterpretq_s8_s16(vcombine_s16( \
			 vget_low_s16(__xsse_tmp), \
			 (int16x4_t) { \
				 (int16_t) vgetq_lane_s16(__xsse_tmp, (((imm) >> 0) & 0x3) + 4), \
				 (int16_t) vgetq_lane_s16(__xsse_tmp, (((imm) >> 2) & 0x3) + 4), \
				 (int16_t) vgetq_lane_s16(__xsse_tmp, (((imm) >> 4) & 0x3) + 4), \
				 (int16_t) vgetq_lane_s16(__xsse_tmp, (((imm) >> 6) & 0x3) + 4) \
			 } \
		 )); \
	 })
 #define _mm_shufflelo_epi16(x, imm) __extension__({ \
		 int16x8_t __xsse_tmp = vreinterpretq_s16_s8(x); \
		 vreinterpretq_s8_s16(vcombine_s16( \
			 (int16x4_t) { \
				 (int16_t) vgetq_lane_s16(__xsse_tmp, (((imm) >> 0) & 0x3)), \
				 (int16_t) vgetq_lane_s16(__xsse_tmp, (((imm) >> 2) & 0x3)), \
				 (int16_t) vgetq_lane_s16(__xsse_tmp, (((imm) >> 4) & 0x3)), \
				 (int16_t) vgetq_lane_s16(__xsse_tmp, (((imm) >> 6) & 0x3)) \
			 }, \
			 vget_high_s16(__xsse_tmp) \
		 )); \
	 })
 #else
 static XSSE_FORCE_INLINE __m128i _mm_shuffle_epi32(__m128i x, int imm)
 {
	 int32x4_t vec = vreinterpretq_s32_s8(x);
	 int32_t arr[4];
	 vst1q_s32(arr, vec);

	 return vreinterpretq_s8_s32((int32x4_t) {
		 arr[(imm >> 0) & 0x3],
		 arr[(imm >> 2) & 0x3],
		 arr[(imm >> 4) & 0x3],
		 arr[(imm >> 6) & 0x3]
	 });
 }
 static XSSE_FORCE_INLINE __m128i _mm_shufflehi_epi16(__m128i x, int imm)
 {
	 int16x8_t vec = vreinterpretq_s16_s8(x);
	 int16_t arr[8];
	 vst1q_s16(arr, vec);

	 return vreinterpretq_s8_s16((int16x8_t) {
		 arr[0], arr[1], arr[2], arr[3],
		 arr[((imm >> 0) & 0x3) + 4],
		 arr[((imm >> 2) & 0x3) + 4],
		 arr[((imm >> 4) & 0x3) + 4],
		 arr[((imm >> 6) & 0x3) + 4]
	 });
 }
 static XSSE_FORCE_INLINE __m128i _mm_shufflelo_epi16(__m128i x, int imm)
 {
	 int16x8_t vec = vreinterpretq_s16_s8(x);
	 int16_t arr[8];
	 vst1q_s16(arr, vec);

	 return vreinterpretq_s8_s16((int16x8_t) {
		 arr[((imm >> 0) & 0x3)],
		 arr[((imm >> 2) & 0x3)],
		 arr[((imm >> 4) & 0x3)],
		 arr[((imm >> 6) & 0x3)],
		 arr[4], arr[5], arr[6], arr[7]
	 });
 }
 #endif

 #define _mm_unpackhi_epi8(a, b) (vzip2q_s8((a), (b)))
 #define _mm_unpackhi_epi16(a, b) (vreinterpretq_s8_s16(vzip2q_s16(vreinterpretq_s16_s8(a), vreinterpretq_s16_s8(b))))
 #define _mm_unpackhi_epi32(a, b) (vreinterpretq_s8_s32(vzip2q_s32(vreinterpretq_s32_s8(a), vreinterpretq_s32_s8(b))))
 #define _mm_unpackhi_epi64(a, b) (vreinterpretq_s8_s64(vzip2q_s64(vreinterpretq_s64_s8(a), vreinterpretq_s64_s8(b))))

 #define _mm_unpacklo_epi8(a, b) (vzip1q_s8((a), (b)))
 #define _mm_unpacklo_epi16(a, b) (vreinterpretq_s8_s16(vzip1q_s16(vreinterpretq_s16_s8(a), vreinterpretq_s16_s8(b))))
 #define _mm_unpacklo_epi32(a, b) (vreinterpretq_s8_s32(vzip1q_s32(vreinterpretq_s32_s8(a), vreinterpretq_s32_s8(b))))
 #define _mm_unpacklo_epi64(a, b) (vreinterpretq_s8_s64(vzip1q_s64(vreinterpretq_s64_s8(a), vreinterpretq_s64_s8(b))))

 #define _mm_move_epi64(x) (vreinterpretq_s8_s64((int64x2_t) { vgetq_lane_s64(vreinterpretq_s64_s8(x), 0), 0 }))

 #endif

 #endif /* XSSE_H */
