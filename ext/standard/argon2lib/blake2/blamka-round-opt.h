#ifndef BLAKE_ROUND_MKA_OPT_H
#define BLAKE_ROUND_MKA_OPT_H

#include "blake2-impl.h"

#include <emmintrin.h>
#if defined(__SSSE3__)
#include <tmmintrin.h> /* for _mm_shuffle_epi8 and _mm_alignr_epi8 */
#endif

#if defined(__XOP__) && (defined(__GNUC__) || defined(__clang__))
#include <x86intrin.h>
#endif

#if !defined(__XOP__)
#if defined(__SSSE3__)
#define r16                                                                    \
    (_mm_setr_epi8(2, 3, 4, 5, 6, 7, 0, 1, 10, 11, 12, 13, 14, 15, 8, 9))
#define r24                                                                    \
    (_mm_setr_epi8(3, 4, 5, 6, 7, 0, 1, 2, 11, 12, 13, 14, 15, 8, 9, 10))
#define _mm_roti_epi64(x, c)                                                   \
    (-(c) == 32)                                                               \
        ? _mm_shuffle_epi32((x), _MM_SHUFFLE(2, 3, 0, 1))                      \
        : (-(c) == 24)                                                         \
              ? _mm_shuffle_epi8((x), r24)                                     \
              : (-(c) == 16)                                                   \
                    ? _mm_shuffle_epi8((x), r16)                               \
                    : (-(c) == 63)                                             \
                          ? _mm_xor_si128(_mm_srli_epi64((x), -(c)),           \
                                          _mm_add_epi64((x), (x)))             \
                          : _mm_xor_si128(_mm_srli_epi64((x), -(c)),           \
                                          _mm_slli_epi64((x), 64 - (-(c))))
#else /* defined(__SSE2__) */
#define _mm_roti_epi64(r, c)                                                   \
    _mm_xor_si128(_mm_srli_epi64((r), -(c)), _mm_slli_epi64((r), 64 - (-(c))))
#endif
#else
#endif

static BLAKE2_INLINE __m128i fBlaMka(__m128i x, __m128i y) {
    const __m128i z = _mm_mul_epu32(x, y);
    return _mm_add_epi64(_mm_add_epi64(x, y), _mm_add_epi64(z, z));
}

#define G1(A0, B0, C0, D0, A1, B1, C1, D1)                                     \
    do {                                                                       \
        A0 = fBlaMka(A0, B0);                                                  \
        A1 = fBlaMka(A1, B1);                                                  \
                                                                               \
        D0 = _mm_xor_si128(D0, A0);                                            \
        D1 = _mm_xor_si128(D1, A1);                                            \
                                                                               \
        D0 = _mm_roti_epi64(D0, -32);                                          \
        D1 = _mm_roti_epi64(D1, -32);                                          \
                                                                               \
        C0 = fBlaMka(C0, D0);                                                  \
        C1 = fBlaMka(C1, D1);                                                  \
                                                                               \
        B0 = _mm_xor_si128(B0, C0);                                            \
        B1 = _mm_xor_si128(B1, C1);                                            \
                                                                               \
        B0 = _mm_roti_epi64(B0, -24);                                          \
        B1 = _mm_roti_epi64(B1, -24);                                          \
    } while ((void)0, 0)

#define G2(A0, B0, C0, D0, A1, B1, C1, D1)                                     \
    do {                                                                       \
        A0 = fBlaMka(A0, B0);                                                  \
        A1 = fBlaMka(A1, B1);                                                  \
                                                                               \
        D0 = _mm_xor_si128(D0, A0);                                            \
        D1 = _mm_xor_si128(D1, A1);                                            \
                                                                               \
        D0 = _mm_roti_epi64(D0, -16);                                          \
        D1 = _mm_roti_epi64(D1, -16);                                          \
                                                                               \
        C0 = fBlaMka(C0, D0);                                                  \
        C1 = fBlaMka(C1, D1);                                                  \
                                                                               \
        B0 = _mm_xor_si128(B0, C0);                                            \
        B1 = _mm_xor_si128(B1, C1);                                            \
                                                                               \
        B0 = _mm_roti_epi64(B0, -63);                                          \
        B1 = _mm_roti_epi64(B1, -63);                                          \
    } while ((void)0, 0)

#if defined(__SSSE3__)
#define DIAGONALIZE(A0, B0, C0, D0, A1, B1, C1, D1)                            \
    do {                                                                       \
        __m128i t0 = _mm_alignr_epi8(B1, B0, 8);                               \
        __m128i t1 = _mm_alignr_epi8(B0, B1, 8);                               \
        B0 = t0;                                                               \
        B1 = t1;                                                               \
                                                                               \
        t0 = C0;                                                               \
        C0 = C1;                                                               \
        C1 = t0;                                                               \
                                                                               \
        t0 = _mm_alignr_epi8(D1, D0, 8);                                       \
        t1 = _mm_alignr_epi8(D0, D1, 8);                                       \
        D0 = t1;                                                               \
        D1 = t0;                                                               \
    } while ((void)0, 0)

#define UNDIAGONALIZE(A0, B0, C0, D0, A1, B1, C1, D1)                          \
    do {                                                                       \
        __m128i t0 = _mm_alignr_epi8(B0, B1, 8);                               \
        __m128i t1 = _mm_alignr_epi8(B1, B0, 8);                               \
        B0 = t0;                                                               \
        B1 = t1;                                                               \
                                                                               \
        t0 = C0;                                                               \
        C0 = C1;                                                               \
        C1 = t0;                                                               \
                                                                               \
        t0 = _mm_alignr_epi8(D0, D1, 8);                                       \
        t1 = _mm_alignr_epi8(D1, D0, 8);                                       \
        D0 = t1;                                                               \
        D1 = t0;                                                               \
    } while ((void)0, 0)
#else /* SSE2 */
#define DIAGONALIZE(A0, B0, C0, D0, A1, B1, C1, D1)                            \
    do {                                                                       \
        __m128i t0 = D0;                                                       \
        __m128i t1 = B0;                                                       \
        D0 = C0;                                                               \
        C0 = C1;                                                               \
        C1 = D0;                                                               \
        D0 = _mm_unpackhi_epi64(D1, _mm_unpacklo_epi64(t0, t0));               \
        D1 = _mm_unpackhi_epi64(t0, _mm_unpacklo_epi64(D1, D1));               \
        B0 = _mm_unpackhi_epi64(B0, _mm_unpacklo_epi64(B1, B1));               \
        B1 = _mm_unpackhi_epi64(B1, _mm_unpacklo_epi64(t1, t1));               \
    } while ((void)0, 0)

#define UNDIAGONALIZE(A0, B0, C0, D0, A1, B1, C1, D1)                          \
    do {                                                                       \
        __m128i t0, t1;                                                        \
        t0 = C0;                                                               \
        C0 = C1;                                                               \
        C1 = t0;                                                               \
        t0 = B0;                                                               \
        t1 = D0;                                                               \
        B0 = _mm_unpackhi_epi64(B1, _mm_unpacklo_epi64(B0, B0));               \
        B1 = _mm_unpackhi_epi64(t0, _mm_unpacklo_epi64(B1, B1));               \
        D0 = _mm_unpackhi_epi64(D0, _mm_unpacklo_epi64(D1, D1));               \
        D1 = _mm_unpackhi_epi64(D1, _mm_unpacklo_epi64(t1, t1));               \
    } while ((void)0, 0)
#endif

#define BLAKE2_ROUND(A0, A1, B0, B1, C0, C1, D0, D1)                           \
    do {                                                                       \
        G1(A0, B0, C0, D0, A1, B1, C1, D1);                                    \
        G2(A0, B0, C0, D0, A1, B1, C1, D1);                                    \
                                                                               \
        DIAGONALIZE(A0, B0, C0, D0, A1, B1, C1, D1);                           \
                                                                               \
        G1(A0, B0, C0, D0, A1, B1, C1, D1);                                    \
        G2(A0, B0, C0, D0, A1, B1, C1, D1);                                    \
                                                                               \
        UNDIAGONALIZE(A0, B0, C0, D0, A1, B1, C1, D1);                         \
    } while ((void)0, 0)

#endif
