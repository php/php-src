/*-----------------------------------------------------------------------------
 * MurmurHash3 was written by Austin Appleby, and is placed in the public
 * domain.
 *
 * This implementation was written by Shane Day, and is also public domain.
 *
 * This is a portable ANSI C implementation of MurmurHash3_x86_32 (Murmur3A)
 * with support for progressive processing.
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

We can only process entire 32 bit chunks of input, except for the very end
that may be shorter. So along with the partial hash we need to give back to
the caller a carry containing up to 3 bytes that we were unable to process.
This carry also needs to record the number of bytes the carry holds. I use
the low 2 bits as a count (0..3) and the carry bytes are shifted into the
high byte in stream order.

To handle endianess I simply use a macro that reads a uint32_t and define
that macro to be a direct read on little endian machines, a read and swap
on big endian machines, or a byte-by-byte read if the endianess is unknown.

-----------------------------------------------------------------------------*/


#include "PMurHash.h"

// /* MSVC warnings we choose to ignore */
// #if defined(_MSC_VER)
//   #pragma warning(disable: 4127) /* conditional expression is constant */
// #endif

/*-----------------------------------------------------------------------------
 * Endianess, misalignment capabilities and util macros
 *
 * The following 3 macros are defined in this section. The other macros defined
 * are only needed to help derive these 3.
 *
 * READ_UINT32(x)   Read a little endian unsigned 32-bit int
 * UNALIGNED_SAFE   Defined if READ_UINT32 works on non-word boundaries
 * ROTL32(x,r)      Rotate x left by r bits
 */

/* I386 or AMD64 */
#if defined(_M_I86) || defined(_M_IX86) || defined(_X86_) || defined(__i386__) || defined(__i386) || defined(i386) \
 || defined(_M_X64) || defined(__x86_64__) || defined(__x86_64) || defined(__amd64__) || defined(__amd64)
  #define UNALIGNED_SAFE
#endif
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
#else
  #define FORCE_INLINE static inline __attribute__((always_inline))
  /* gcc recognises this code and generates a rotate instruction for CPUs with one */
  #define ROTL32(x,r)  (((uint32_t)x << r) | ((uint32_t)x >> (32 - r)))
#endif

#include "endianness.h"

#define READ_UINT32(ptr) getblock32((uint32_t *)ptr, 0)

/*-----------------------------------------------------------------------------
 * Core murmurhash algorithm macros */

static const uint32_t kC1 = 0xcc9e2d51;
static const uint32_t kC2 = 0x1b873593;

/* This is the main processing body of the algorithm. It operates
 * on each full 32-bits of input. */
#define doblock(h1, k1) \
do {\
  k1 *= kC1;\
  k1 = ROTL32(k1,15);\
  k1 *= kC2;\
\
  h1 ^= k1;\
  h1 = ROTL32(h1,13);\
  h1 = h1*5+0xe6546b64;\
} while(0)

/* Append unaligned bytes to carry, forcing hash churn if we have 4 bytes */
/* cnt=bytes to process, h1=name of h1 var, c=carry, n=bytes in c, ptr/len=payload */
#define dobytes(cnt, h1, c, n, ptr, len) \
do {\
  unsigned __cnt = cnt;\
  while(__cnt--) {\
    c = c>>8 | (uint32_t)*ptr++<<24;\
    n++; len--;\
    if(n==4) {\
        doblock(h1, c);\
        n = 0;\
    }\
  }\
} while(0)

/*---------------------------------------------------------------------------*/

/* Main hashing function. Initialise carry to 0 and h1 to 0 or an initial seed
 * if wanted. Both ph1 and pcarry are required arguments. */
void PMurHash32_Process(uint32_t *ph1, uint32_t *pcarry, const void *key, int len)
{
  uint32_t h1 = *ph1;
  uint32_t c = *pcarry;

  const uint8_t *ptr = (uint8_t*)key;
  const uint8_t *end;

  /* Extract carry count from low 2 bits of c value */
  int n = c & 3;

#if defined(UNALIGNED_SAFE)
  /* This CPU handles unaligned word access */
// #pragma message ( "UNALIGNED_SAFE" )
  /* Consume any carry bytes */
  int i = (4-n) & 3;
  if(i && i <= len) {
    dobytes(i, h1, c, n, ptr, len);
  }

  /* Process 32-bit chunks */
  end = ptr + (len & ~3);
  for( ; ptr < end ; ptr+=4) {
    uint32_t k1 = READ_UINT32(ptr);
    doblock(h1, k1);
  }

#else /*UNALIGNED_SAFE*/
  /* This CPU does not handle unaligned word access */
// #pragma message ( "ALIGNED" )
  /* Consume enough so that the next data byte is word aligned */
  int i = -(intptr_t)(void *)ptr & 3;
  if(i && i <= len) {
      dobytes(i, h1, c, n, ptr, len);
  }

  /* We're now aligned. Process in aligned blocks. Specialise for each possible carry count */
  end = ptr + (len & ~3);
  switch(n) { /* how many bytes in c */
  case 0: /* c=[----]  w=[3210]  b=[3210]=w            c'=[----] */
    for( ; ptr < end ; ptr+=4) {
      uint32_t k1 = READ_UINT32(ptr);
      doblock(h1, k1);
    }
    break;
  case 1: /* c=[0---]  w=[4321]  b=[3210]=c>>24|w<<8   c'=[4---] */
    for( ; ptr < end ; ptr+=4) {
      uint32_t k1 = c>>24;
      c = READ_UINT32(ptr);
      k1 |= c<<8;
      doblock(h1, k1);
    }
    break;
  case 2: /* c=[10--]  w=[5432]  b=[3210]=c>>16|w<<16  c'=[54--] */
    for( ; ptr < end ; ptr+=4) {
      uint32_t k1 = c>>16;
      c = READ_UINT32(ptr);
      k1 |= c<<16;
      doblock(h1, k1);
    }
    break;
  case 3: /* c=[210-]  w=[6543]  b=[3210]=c>>8|w<<24   c'=[654-] */
    for( ; ptr < end ; ptr+=4) {
      uint32_t k1 = c>>8;
      c = READ_UINT32(ptr);
      k1 |= c<<24;
      doblock(h1, k1);
    }
  }
#endif /*UNALIGNED_SAFE*/

  /* Advance over whole 32-bit chunks, possibly leaving 1..3 bytes */
  len -= len & ~3;

  /* Append any remaining bytes into carry */
  dobytes(len, h1, c, n, ptr, len);

  /* Copy out new running hash and carry */
  *ph1 = h1;
  *pcarry = (c & ~0xff) | n;
}

/*---------------------------------------------------------------------------*/

/* Finalize a hash. To match the original Murmur3A the total_length must be provided */
uint32_t PMurHash32_Result(uint32_t h, uint32_t carry, uint32_t total_length)
{
  uint32_t k1;
  int n = carry & 3;
  if(n) {
    k1 = carry >> (4-n)*8;
    k1 *= kC1; k1 = ROTL32(k1,15); k1 *= kC2; h ^= k1;
  }
  h ^= total_length;

  /* fmix */
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;

  return h;
}
