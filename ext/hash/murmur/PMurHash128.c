/*-----------------------------------------------------------------------------
 * MurmurHash3 was written by Austin Appleby, and is placed in the public
 * domain.
 *
 * This is a c++ implementation of MurmurHash3_128 with support for progressive
 * processing based on PMurHash implementation written by Shane Day.
 */

/*-----------------------------------------------------------------------------

If you want to understand the MurmurHash algorithm you would be much better
off reading the original source. Just point your browser at:
http://code.google.com/p/smhasher/source/browse/trunk/MurmurHash3.cpp


What this version provides?

1. Progressive data feeding. Useful when the entire payload to be hashed
does not fit in memory or when the data is streamed through the application.
Also useful when hashing a number of strings with a common prefix. A partial
hash of a prefix string can be generated and reused for each suffix string.

How does it work?

We can only process entire 128 bit chunks of input, except for the very end
that may be shorter. So along with the partial hash we need to give back to
the caller a carry containing up to 15 bytes that we were unable to process.
This carry also needs to record the number of bytes the carry holds. I use
the low 4 bits as a count (0..15) and the carry bytes are shifted into the
high byte in stream order.

To handle endianess I simply use a macro that reads an uint and define
that macro to be a direct read on little endian machines, a read and swap
on big endian machines.

-----------------------------------------------------------------------------*/


#include "PMurHash128.h"

/*-----------------------------------------------------------------------------
 * Endianess, misalignment capabilities and util macros
 *
 * The following 5 macros are defined in this section. The other macros defined
 * are only needed to help derive these 5.
 *
 * READ_UINT32(x,i) Read a little endian unsigned 32-bit int at index
 * READ_UINT64(x,i) Read a little endian unsigned 64-bit int at index
 * UNALIGNED_SAFE   Defined if READ_UINTXX works on non-word boundaries
 * ROTL32(x,r)      Rotate x left by r bits
 * ROTL64(x,r)      Rotate x left by r bits
 * BIG_CONSTANT
 * FORCE_INLINE
 */

/* I386 or AMD64 */
#if defined(_M_I86) || defined(_M_IX86) || defined(_X86_) || defined(__i386__) || defined(__i386) || defined(i386) \
 || defined(_M_X64) || defined(__x86_64__) || defined(__x86_64) || defined(__amd64__) || defined(__amd64)
  #define UNALIGNED_SAFE
#endif

/* Find best way to ROTL */
#if defined(_MSC_VER)
  #define FORCE_INLINE  static __forceinline
  #include <stdlib.h>  /* Microsoft put _rotl declaration in here */
  #define ROTL32(x,y)  _rotl(x,y)
  #define ROTL64(x,y)  _rotl64(x,y)
  #define BIG_CONSTANT(x) (x)
#else
  #define FORCE_INLINE static inline __attribute__((always_inline))
  /* gcc recognises this code and generates a rotate instruction for CPUs with one */
  #define ROTL32(x,r)  (((uint32_t)x << r) | ((uint32_t)x >> (32 - r)))
  #define ROTL64(x,r)  (((uint64_t)x << r) | ((uint64_t)x >> (64 - r)))
  #define BIG_CONSTANT(x) (x##LLU)
#endif

#include "endianness.h"

#define READ_UINT64(ptr,i) getblock64((uint64_t *)ptr,i)
#define READ_UINT32(ptr,i) getblock32((uint32_t *)ptr,i)

//-----------------------------------------------------------------------------
// Finalization mix - force all bits of a hash block to avalanche

FORCE_INLINE uint32_t fmix32 ( uint32_t h )
{
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;

  return h;
}

//----------

FORCE_INLINE uint64_t fmix64 ( uint64_t k )
{
  k ^= k >> 33;
  k *= BIG_CONSTANT(0xff51afd7ed558ccd);
  k ^= k >> 33;
  k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
  k ^= k >> 33;

  return k;
}

/*-----------------------------------------------------------------------------*
                                 PMurHash128x86
 *-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Core murmurhash algorithm macros */

static const uint32_t kC1 = 0x239b961b;
static const uint32_t kC2 = 0xab0e9789;
static const uint32_t kC3 = 0x38b34ae5;
static const uint32_t kC4 = 0xa1e38b93;

/* This is the main processing body of the algorithm. It operates
 * on each full 128-bits of input. */
#define doblock128x86(h1, h2, h3, h4, k1, k2, k3,k4)\
do {\
  k1 *= kC1; k1  = ROTL32(k1,15); k1 *= kC2; h1 ^= k1;\
\
  h1 = ROTL32(h1,19); h1 += h2; h1 = h1*5+0x561ccd1b;\
\
  k2 *= kC2; k2  = ROTL32(k2,16); k2 *= kC3; h2 ^= k2;\
\
  h2 = ROTL32(h2,17); h2 += h3; h2 = h2*5+0x0bcaa747;\
\
  k3 *= kC3; k3  = ROTL32(k3,17); k3 *= kC4; h3 ^= k3;\
\
  h3 = ROTL32(h3,15); h3 += h4; h3 = h3*5+0x96cd1c35;\
\
  k4 *= kC4; k4  = ROTL32(k4,18); k4 *= kC1; h4 ^= k4;\
\
  h4 = ROTL32(h4,13); h4 += h1; h4 = h4*5+0x32ac3b17;\
} while(0)

/* Append unaligned bytes to carry, forcing hash churn if we have 16 bytes */
/* cnt=bytes to process, h1-h4=hash k1-k4=carry, n=bytes in carry, ptr/len=payload */
#define dobytes128x86(cnt, h1, h2, h3, h4, k1, k2, k3, k4, n, ptr, len)\
do {\
  unsigned __cnt = cnt;\
  for(;__cnt--; len--) {\
    switch(n) {\
      case  0: case  1: case  2: case  3:\
        k1 = k1>>8 | (uint32_t)*ptr++<<24;\
        ++n; break;\
\
      case  4: case  5: case  6: case  7:\
        k2 = k2>>8 | (uint32_t)*ptr++<<24;\
        ++n; break;\
\
      case  8: case  9: case 10: case 11:\
        k3 = k3>>8 | (uint32_t)*ptr++<<24;\
        ++n; break;\
\
      case 12: case 13: case 14:\
        k4 = k4>>8 | (uint32_t)*ptr++<<24;\
        ++n; break;\
\
      case 15:\
        k4 = k4>>8 | (uint32_t)*ptr++<<24;\
        doblock128x86(h1, h2, h3, h4, k1, k2, k3, k4);\
        n = 0; break;\
    }\
  }\
} while(0)

/* Finalize a hash. To match the original Murmur3_128x86 the total_length must be provided */
void PMurHash128x86_Result(const uint32_t ph[4], const uint32_t pcarry[4], uint32_t total_length, uint32_t out[4])
{
  uint32_t h1 = ph[0];
  uint32_t h2 = ph[1];
  uint32_t h3 = ph[2];
  uint32_t h4 = ph[3];

  uint32_t k1, k2, k3, k4 = pcarry[3];

  int n = k4 & 15;
  switch(n) {
    case  1: case  2: case  3: case  4:
      k1 = pcarry[0] >> (4-n)*8;
      goto finrot_k1;

    case  5: case  6: case  7: case  8:
      k2 = pcarry[1] >> (8-n)*8;
      goto finrot_k21;

    case  9: case 10: case 11: case 12:
      k3 = pcarry[2] >> (12-n)*8;
      goto finrot_k321;

    case 13: case 14: case 15:
      k4 >>= (16-n)*8;
      goto finrot_k4321;

    default:
      goto skiprot;
  }
finrot_k4321:
  k4 *= kC4; k4  = ROTL32(k4,18); k4 *= kC1; h4 ^= k4;
  k3 = pcarry[2];
finrot_k321:
  k3 *= kC3; k3  = ROTL32(k3,17); k3 *= kC4; h3 ^= k3;
  k2 = pcarry[1];
finrot_k21:
  k2 *= kC2; k2  = ROTL32(k2,16); k2 *= kC3; h2 ^= k2;
  k1 = pcarry[0];
finrot_k1:
  k1 *= kC1; k1  = ROTL32(k1,15); k1 *= kC2; h1 ^= k1;
skiprot:

  //----------
  // finalization

  h1 ^= total_length; h2 ^= total_length;
  h3 ^= total_length; h4 ^= total_length;

  h1 += h2; h1 += h3; h1 += h4;
  h2 += h1; h3 += h1; h4 += h1;

  h1 = fmix32(h1);
  h2 = fmix32(h2);
  h3 = fmix32(h3);
  h4 = fmix32(h4);

  h1 += h2; h1 += h3; h1 += h4;
  h2 += h1; h3 += h1; h4 += h1;

  out[0] = h1;
  out[1] = h2;
  out[2] = h3;
  out[3] = h4;
}

/*---------------------------------------------------------------------------*/

/* Main hashing function. Initialise carry[4] to {0,0,0,0} and h[4] to an initial {seed,seed,seed,seed}
 * if wanted. Both ph and pcarry are required arguments. */
void PMurHash128x86_Process(uint32_t ph[4], uint32_t pcarry[4], const void * const key, int len)
{
  uint32_t h1 = ph[0];
  uint32_t h2 = ph[1];
  uint32_t h3 = ph[2];
  uint32_t h4 = ph[3];

  uint32_t k1 = pcarry[0];
  uint32_t k2 = pcarry[1];
  uint32_t k3 = pcarry[2];
  uint32_t k4 = pcarry[3];

  const uint8_t *ptr = (uint8_t*)key;
  const uint8_t *end;

  /* Extract carry count from low 4 bits of c value */
  int n = k4 & 15;

#if defined(UNALIGNED_SAFE)
  /* This CPU handles unaligned word access */
// #pragma message ( "UNALIGNED_SAFE" )
  /* Consume any carry bytes */
  int i = (16-n) & 15;
  if(i && i <= len) {
    dobytes128x86(i, h1, h2, h3, h4, k1, k2, k3, k4, n, ptr, len);
  }

  /* Process 128-bit chunks */
  end = ptr + (len & ~15);
  for( ; ptr < end ; ptr+=16) {
    k1 = READ_UINT32(ptr, 0);
    k2 = READ_UINT32(ptr, 1);
    k3 = READ_UINT32(ptr, 2);
    k4 = READ_UINT32(ptr, 3);
    doblock128x86(h1, h2, h3, h4, k1, k2, k3, k4);
  }

#else /*UNALIGNED_SAFE*/
  /* This CPU does not handle unaligned word access */
// #pragma message ( "ALIGNED" )
  /* Consume enough so that the next data byte is word aligned */
  int i = -(intptr_t)(void *)ptr & 3;
  if(i && i <= len) {
    dobytes128x86(i, h1, h2, h3, h4, k1, k2, k3, k4, n, ptr, len);
  }
  /* We're now aligned. Process in aligned blocks. Specialise for each possible carry count */
  end = ptr + (len & ~15);

  switch(n) { /* how many bytes in c */
  case 0: /*
  k1=[----] k2=[----] k2=[----] k4=[----] w=[3210 7654 ba98 fedc] b=[3210 7654 ba98 fedc] */
    for( ; ptr < end ; ptr+=16) {
      k1 = READ_UINT32(ptr, 0);
      k2 = READ_UINT32(ptr, 1);
      k3 = READ_UINT32(ptr, 2);
      k4 = READ_UINT32(ptr, 3);
      doblock128x86(h1, h2, h3, h4, k1, k2, k3, k4);
    }
    break;
  case 1: case 2: case 3: /*
  k1=[10--] k2=[----] k3=[----] k4=[----] w=[5432 9876 dcba hgfe] b=[3210 7654 ba98 fedc] k1'=[hg--] */
    {
      const int lshift = n*8, rshift = 32-lshift;
      for( ; ptr < end ; ptr+=16) {
        uint32_t c = k1>>rshift;      // --10
        k2 = READ_UINT32(ptr, 0);     // 5432
        c |= k2<<lshift;              // 3210.
        k1 = READ_UINT32(ptr, 1);     // 9876
        k2 = k1<<lshift | k2>>rshift; // 7654.
        k4 = READ_UINT32(ptr, 2);     // dcba
        k3 = k4<<lshift | k1>>rshift; // ba98.
        k1 = READ_UINT32(ptr, 3);     // hgfe.
        k4 = k1<<lshift | k4>>rshift; // fedc.
        doblock128x86(h1, h2, h3, h4, c, k2, k3, k4);
      }
    }
    break;
  case 4: /*
  k1=[3210] k2=[----] k3=[----] k4=[----] w=[7654 ba98 fedc jihg] b=[3210 7654 ba98 fedc] k1'=[jihg] */
    for( ; ptr < end ; ptr+=16) {
      k2 = READ_UINT32(ptr, 0);
      k3 = READ_UINT32(ptr, 1);
      k4 = READ_UINT32(ptr, 2);
      doblock128x86(h1, h2, h3, h4, k1, k2, k3, k4);
      k1 = READ_UINT32(ptr, 3);
    }
    break;
  case 5: case 6: case 7: /*
  k1=[3210] k2=[54--] k3=[----] k4=[----] w=[9876 dcba hgfe lkji] b=[3210 7654 ba98 fedc] k1'=[jihg] k2'=[lk--] */
    {
      const int lshift = n*8-32, rshift = 32-lshift;
      for( ; ptr < end ; ptr+=16) {
        uint32_t c = k2>>rshift;      // --54
        k3 = READ_UINT32(ptr, 0);     // 9876
        c |= k3<<lshift;              // 7654.
        k4 = READ_UINT32(ptr, 1);     // dcba
        k3 = k4<<lshift | k3>>rshift; // ba98.
        k2 = READ_UINT32(ptr, 2);     // hgfe
        k4 = k2<<lshift | k4>>rshift; // fedc.
        doblock128x86(h1, h2, h3, h4, k1, c, k3, k4);
        k1 = k2>>rshift;              // --hg
        k2 = READ_UINT32(ptr, 3);     // lkji.
        k1 |= k2<<lshift;             // jihg.
      }
    }
  case 8: /*
  k1=[3210] k2=[7654] k3=[----] k4=[----] w=[ba98 fedc jihg nmlk] b=[3210 7654 ba98 fedc] k1'=[jihg] k2'=[nmlk] */
    for( ; ptr < end ; ptr+=16) {
      k3 = READ_UINT32(ptr, 0);
      k4 = READ_UINT32(ptr, 1);
      doblock128x86(h1, h2, h3, h4, k1, k2, k3, k4);
      k1 = READ_UINT32(ptr, 2);
      k2 = READ_UINT32(ptr, 3);
    }
    break;
  case 9: case 10: case 11: /*
  k1=[3210] k2=[7654] k3=[98--] k4=[----] w=[dcba hgfe lkji ponm] b=[3210 7654 ba98 fedc] k1'=[jihg] k2'=[nmlk] k3'=[po--] */
    {
      const int lshift = n*8-64, rshift = 32-lshift;
      for( ; ptr < end ; ptr+=16) {
        uint32_t c = k3>>rshift;      // --98
        k4 = READ_UINT32(ptr, 0);     // dcba
        c |= k4<<lshift;              // ba98.
        k3 = READ_UINT32(ptr, 1);     // hgfe
        k4 = k3<<lshift | k4>>rshift; // fedc.
        doblock128x86(h1, h2, h3, h4, k1, k2, c, k4);
        k2 = READ_UINT32(ptr, 2);     // lkji
        k1 = k2<<lshift | k3>>rshift; // jihg.
        k3 = READ_UINT32(ptr, 3);     // ponm.
        k2 = k3<<lshift | k2>>rshift; // nmlk.
      }
    }
  case 12: /*
  k1=[3210] k2=[7654] k3=[ba98] k4=[----] w=[fedc jihg nmlk rqpo] b=[3210 7654 ba98 fedc] k1'=[jihg] k2'=[nmlk] k3'=[rqpo] */
    for( ; ptr < end ; ptr+=16) {
      k4 = READ_UINT32(ptr, 0);
      doblock128x86(h1, h2, h3, h4, k1, k2, k3, k4);
      k1 = READ_UINT32(ptr, 1);
      k2 = READ_UINT32(ptr, 2);
      k3 = READ_UINT32(ptr, 3);
    }
    break;
  default: /* 12 < n <= 15
  k1=[3210] k2=[7654] k3=[ba98] k4=[dc--] w=[hgfe lkji ponm tsrq] b=[3210 7654 ba98 fedc] k1'=[jihg] k2'=[nmlk] k3'=[rqpo] k3'=[ts--] */
    {
      const int lshift = n*8-96, rshift = 32-lshift;
      for( ; ptr < end ; ptr+=16) {
        uint32_t c = k4>>rshift;      // --dc
        k4 = READ_UINT32(ptr, 0);     // hgfe
        c |= k4<<lshift;              // fedc.
        doblock128x86(h1, h2, h3, h4, k1, k2, k3, c);
        k3 = READ_UINT32(ptr, 1);     // lkji
        k1 = k3<<lshift | k4>>rshift; // jihg.
        c  = READ_UINT32(ptr, 2);     // ponm
        k2 = c<<lshift | k3>>rshift;  // nmlk.
        k4 = READ_UINT32(ptr, 3);     // tsrq.
        k3 = k4<<lshift | c>>rshift;  // rqpo.
      }
    }
  }
#endif /*UNALIGNED_SAFE*/

  /* Advance over whole 128-bit chunks, possibly leaving 1..15 bytes */
  len -= len & ~15;

  /* Append any remaining bytes into carry */
  dobytes128x86(len, h1, h2, h3, h4, k1, k2, k3, k4, n, ptr, len);

  /* Copy out new running hash and carry */
  ph[0] = h1;
  ph[1] = h2;
  ph[2] = h3;
  ph[3] = h4;
  pcarry[0] = k1;
  pcarry[1] = k2;
  pcarry[2] = k3;
  pcarry[3] = (k4 & ~0xff) | n;
}

/*---------------------------------------------------------------------------*/

/* All in one go */

/* MurmurHash3_x86_128 api */
void PMurHash128x86(const void * key, const int len, uint32_t seed, void * out)
{
  uint32_t carry[4] = {0, 0, 0, 0};
  uint32_t h[4] = {seed, seed, seed, seed};
  PMurHash128x86_Process(h, carry, key, len);
  PMurHash128x86_Result(h, carry, (uint32_t) len, (uint32_t *) out);
}

/*-----------------------------------------------------------------------------*
                                 PMurHash128x64
 *-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Core murmurhash algorithm macros */

static const uint64_t kC1L = BIG_CONSTANT(0x87c37b91114253d5);
static const uint64_t kC2L = BIG_CONSTANT(0x4cf5ad432745937f);

/* This is the main processing body of the algorithm. It operates
 * on each full 128-bits of input. */
#define doblock128x64(h1, h2, k1, k2)\
do {\
  k1 *= kC1L; k1  = ROTL64(k1,31); k1 *= kC2L; h1 ^= k1;\
\
  h1 = ROTL64(h1,27); h1 += h2; h1 = h1*5+0x52dce729;\
\
  k2 *= kC2L; k2  = ROTL64(k2,33); k2 *= kC1L; h2 ^= k2;\
\
  h2 = ROTL64(h2,31); h2 += h1; h2 = h2*5+0x38495ab5;\
} while(0)

/* Append unaligned bytes to carry, forcing hash churn if we have 16 bytes */
/* cnt=bytes to process, h1,h2=hash k1,k2=carry, n=bytes in carry, ptr/len=payload */
#define dobytes128x64(cnt, h1, h2, k1, k2, n, ptr, len) \
do {\
  unsigned __cnt = cnt;\
  for(;__cnt--; len--) {\
    switch(n) {\
      case  0: case  1: case  2: case  3:\
      case  4: case  5: case  6: case  7:\
        k1 = k1>>8 | (uint64_t)*ptr++<<56;\
        n++; break;\
\
      case  8: case  9: case 10: case 11:\
      case 12: case 13: case 14:\
        k2 = k2>>8 | (uint64_t)*ptr++<<56;\
        n++; break;\
\
      case 15:\
        k2 = k2>>8 | (uint64_t)*ptr++<<56;\
        doblock128x64(h1, h2, k1, k2);\
        n = 0; break;\
    }\
  }\
} while(0)

/* Finalize a hash. To match the original Murmur3_128x64 the total_length must be provided */
void PMurHash128x64_Result(const uint64_t ph[2], const uint64_t pcarry[2],
                        const uint32_t total_length, uint64_t out[2])
{
  uint64_t h1 = ph[0];
  uint64_t h2 = ph[1];

  uint64_t k1;
  uint64_t k2 = pcarry[1];

  int n = k2 & 15;
  if (n) {
    k1 = pcarry[0];
    if (n > 8) {
      k2 >>= (16-n)*8;
      k2 *= kC2L; k2  = ROTL64(k2,33); k2 *= kC1L; h2 ^= k2;
    } else {
      k1 >>= (8-n)*8;
    }
    k1 *= kC1L; k1  = ROTL64(k1,31); k1 *= kC2L; h1 ^= k1;
  }

  //----------
  // finalization

  h1 ^= total_length; h2 ^= total_length;

  h1 += h2;
  h2 += h1;

  h1 = fmix64(h1);
  h2 = fmix64(h2);

  h1 += h2;
  h2 += h1;

  out[0] = h1;
  out[1] = h2;
}

/*---------------------------------------------------------------------------*/

/* Main hashing function. Initialise carry[2] to {0,0} and h[2] to an initial {seed,seed}
 * if wanted. Both ph and pcarry are required arguments. */
void PMurHash128x64_Process(uint64_t ph[2], uint64_t pcarry[2], const void * const key, int len)
{
  uint64_t h1 = ph[0];
  uint64_t h2 = ph[1];

  uint64_t k1 = pcarry[0];
  uint64_t k2 = pcarry[1];

  const uint8_t *ptr = (uint8_t*)key;
  const uint8_t *end;

  /* Extract carry count from low 4 bits of c value */
  int n = k2 & 15;

#if defined(UNALIGNED_SAFE)
  /* This CPU handles unaligned word access */
// #pragma message ( "UNALIGNED_SAFE" )
  /* Consume any carry bytes */
  int i = (16-n) & 15;
  if(i && i <= len) {
    dobytes128x64(i, h1, h2, k1, k2, n, ptr, len);
  }

  /* Process 128-bit chunks */
  end = ptr + (len & ~15);
  for( ; ptr < end ; ptr+=16) {
    k1 = READ_UINT64(ptr, 0);
    k2 = READ_UINT64(ptr, 1);
    doblock128x64(h1, h2, k1, k2);
  }

#else /*UNALIGNED_SAFE*/
  /* This CPU does not handle unaligned word access */
// #pragma message ( "ALIGNED" )
  /* Consume enough so that the next data byte is word aligned */
  int i = -(intptr_t)(void *)ptr & 7;
  if(i && i <= len) {
    dobytes128x64(i, h1, h2, k1, k2, n, ptr, len);
  }
  /* We're now aligned. Process in aligned blocks. Specialise for each possible carry count */
  end = ptr + (len & ~15);

  switch(n) { /* how many bytes in c */
  case 0: /*
    k1=[--------] k2=[--------] w=[76543210 fedcba98] b=[76543210 fedcba98] */
    for( ; ptr < end ; ptr+=16) {
      k1 = READ_UINT64(ptr, 0);
      k2 = READ_UINT64(ptr, 1);
      doblock128x64(h1, h2, k1, k2);
    }
    break;
  case 1: case 2: case 3: case 4: case 5: case 6: case 7: /*
    k1=[10------] k2=[--------] w=[98765432 hgfedcba] b=[76543210 fedcba98] k1'=[hg------] */
    {
      const int lshift = n*8, rshift = 64-lshift;
      for( ; ptr < end ; ptr+=16) {
        uint64_t c = k1>>rshift;
        k2 = READ_UINT64(ptr, 0);
        c |= k2<<lshift;
        k1 = READ_UINT64(ptr, 1);
        k2 = k2>>rshift | k1<<lshift;
        doblock128x64(h1, h2, c, k2);
      }
    }
    break;
  case 8: /*
  k1=[76543210] k2=[--------] w=[fedcba98 nmlkjihg] b=[76543210 fedcba98] k1`=[nmlkjihg] */
    for( ; ptr < end ; ptr+=16) {
      k2 = READ_UINT64(ptr, 0);
      doblock128x64(h1, h2, k1, k2);
      k1 = READ_UINT64(ptr, 1);
    }
    break;
  default: /* 8 < n <= 15
  k1=[76543210] k2=[98------] w=[hgfedcba ponmlkji] b=[76543210 fedcba98] k1`=[nmlkjihg] k2`=[po------] */
    {
      const int lshift = n*8-64, rshift = 64-lshift;
      for( ; ptr < end ; ptr+=16) {
        uint64_t c = k2 >> rshift;
        k2 = READ_UINT64(ptr, 0);
        c |= k2 << lshift;
        doblock128x64(h1, h2, k1, c);
        k1 = k2 >> rshift;
        k2 = READ_UINT64(ptr, 1);
        k1 |= k2 << lshift;
      }
    }
  }
#endif /*UNALIGNED_SAFE*/

  /* Advance over whole 128-bit chunks, possibly leaving 1..15 bytes */
  len -= len & ~15;

  /* Append any remaining bytes into carry */
  dobytes128x64(len, h1, h2, k1, k2, n, ptr, len);

  /* Copy out new running hash and carry */
  ph[0] = h1;
  ph[1] = h2;
  pcarry[0] = k1;
  pcarry[1] = (k2 & ~0xff) | n;
}

/*---------------------------------------------------------------------------*/

/* All in one go */

/* MurmurHash3_x64_128 api */
void PMurHash128x64(const void * key, const int len, uint32_t seed, void * out)
{
  uint64_t carry[2] = {0, 0};
  uint64_t h[2] = {seed, seed};
  PMurHash128x64_Process(h, carry, key, len);
  PMurHash128x64_Result(h, carry, (uint32_t) len, (uint64_t *) out);
}
