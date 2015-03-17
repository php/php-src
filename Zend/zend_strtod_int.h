
#ifdef ZTS
#include <TSRM.h>
#endif

#include <stddef.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <math.h>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

/* TODO check to undef this option, this might
	make more perf. destroy_freelist() 
	should be adapted then. */
#define Omit_Private_Memory 1

#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#elif defined(HAVE_STDINT_H)
#include <stdint.h>
#endif

#ifndef HAVE_INT32_T
# if SIZEOF_INT == 4
typedef int int32_t;
# elif SIZEOF_LONG == 4
typedef long int int32_t;
# endif
#endif

#ifndef HAVE_UINT32_T
# if SIZEOF_INT == 4
typedef unsigned int uint32_t;
# elif SIZEOF_LONG == 4
typedef unsigned long int uint32_t;
# endif
#endif

#ifdef HAVE_LOCALE_H
#define USE_LOCALE 1
#endif

#ifdef WORDS_BIGENDIAN
#define IEEE_BIG_ENDIAN 1
#else
#define IEEE_LITTLE_ENDIAN 1
#endif

#if (defined(__APPLE__) || defined(__APPLE_CC__)) && (defined(__BIG_ENDIAN__) || defined(__LITTLE_ENDIAN__))
# if defined(__LITTLE_ENDIAN__)
#  undef WORDS_BIGENDIAN
# else
#  if defined(__BIG_ENDIAN__)
#   define WORDS_BIGENDIAN
#  endif
# endif
#endif

#if defined(__arm__) && !defined(__VFP_FP__)
/*
 *  * Although the CPU is little endian the FP has different
 *   * byte and word endianness. The byte order is still little endian
 *    * but the word order is big endian.
 *     */
#define IEEE_BIG_ENDIAN
#undef IEEE_LITTLE_ENDIAN
#endif

#ifdef __vax__
#define VAX
#undef IEEE_LITTLE_ENDIAN
#endif

#ifdef IEEE_LITTLE_ENDIAN
#define IEEE_8087 1
#endif

#ifdef IEEE_BIG_ENDIAN
#define IEEE_MC68k 1
#endif

#if defined(_MSC_VER)
#define int32_t __int32
#define uint32_t unsigned __int32
#endif

#ifdef ZTS
#define MULTIPLE_THREADS 1

#define  ACQUIRE_DTOA_LOCK(x) \
	if (0 == x) { \
		tsrm_mutex_lock(dtoa_mutex); \
	} else if (1 == x) { \
		tsrm_mutex_lock(pow5mult_mutex); \
	}

#define FREE_DTOA_LOCK(x) \
	if (0 == x) { \
		tsrm_mutex_unlock(dtoa_mutex); \
	} else if (1 == x) { \
		tsrm_mutex_unlock(pow5mult_mutex); \
	}
	

#endif


