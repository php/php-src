static const union {
  uint8_t u8[2];
  uint16_t u16;
} EndianMix = {{ 1, 0 }};
FORCE_INLINE int IsBigEndian(void)
{
  // Constant-folded by the compiler.
  return EndianMix.u16 != 1;
}

#if defined(_MSC_VER)
#  include <stdlib.h>
#  define BSWAP32(u) _byteswap_ulong(u)
#  define BSWAP64(u) _byteswap_uint64(u)
#else
#  ifdef __has_builtin
#    if __has_builtin(__builtin_bswap32)
#      define BSWAP32(u) __builtin_bswap32(u)
#    endif // __has_builtin(__builtin_bswap32)
#    if __has_builtin(__builtin_bswap64)
#      define BSWAP64(u) __builtin_bswap64(u)
#    endif // __has_builtin(__builtin_bswap64)
#  elif defined(__GNUC__) && ( \
                    __GNUC__ > 4 || ( \
                      __GNUC__ == 4 && ( \
                        __GNUC_MINOR__ >= 3 \
                      ) \
                    ) \
                  )
#    define BSWAP32(u) __builtin_bswap32(u)
#    define BSWAP64(u) __builtin_bswap64(u)
#  endif // __has_builtin
#endif // defined(_MSC_VER)

#ifndef BSWAP32
FORCE_INLINE uint32_t BSWAP32(uint32_t u)
{
  return (((u & 0xff000000) >> 24)
          | ((u & 0x00ff0000) >>  8)
          | ((u & 0x0000ff00) <<  8)
          | ((u & 0x000000ff) << 24));
}
#endif
#ifndef BSWAP64
FORCE_INLINE uint64_t BSWAP64(uint64_t u)
{
   return (((u & 0xff00000000000000ULL) >> 56)
          | ((u & 0x00ff000000000000ULL) >> 40)
          | ((u & 0x0000ff0000000000ULL) >> 24)
          | ((u & 0x000000ff00000000ULL) >>  8)
          | ((u & 0x00000000ff000000ULL) <<  8)
          | ((u & 0x0000000000ff0000ULL) << 24)
          | ((u & 0x000000000000ff00ULL) << 40)
          | ((u & 0x00000000000000ffULL) << 56));
}
#endif

#if defined(__clang__) || defined(__GNUC__) &&  __GNUC__ >= 8
# define NO_SANITIZE_ALIGNMENT __attribute__((no_sanitize("alignment")))
#else
# define NO_SANITIZE_ALIGNMENT
#endif

NO_SANITIZE_ALIGNMENT
FORCE_INLINE uint32_t getblock32 ( const uint32_t * const p, const int i)
{
  if (IsBigEndian()) {
    return BSWAP32(p[i]);
  } else {
    return p[i];
  }
}

NO_SANITIZE_ALIGNMENT
FORCE_INLINE uint64_t getblock64 ( const uint64_t * const p, const int i)
{
  if (IsBigEndian()) {
    return BSWAP64(p[i]);
  } else {
    return p[i];
  }
}

#undef NO_SANITIZE_ALIGNMENT
