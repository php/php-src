

#ifndef ZEND_INT_H
#define ZEND_INT_H

#ifndef ZEND_WIN32
/* this probably needs more on non windows, like for C++ and C99*/
# include <stdint.h>
#endif

/* Integer types or the old good long. This part will have to be
   reworked, obviously if we wanna some platform dependent stuff. */
#ifdef ZEND_ENABLE_INT64
# ifdef ZEND_WIN32
#  ifdef _WIN64
typedef __int64 zend_int_t;
typedef unsigned __int64 zend_uint_t;
typedef __int64 zend_off_t;
#   define ZEND_INT_MAX _I64_MAX
#   define ZEND_INT_MIN _I64_MIN
#   define ZEND_UINT_MAX _UI64_MAX
#  else
#   error Cant enable 64 bit integers on non 64 bit platform
#  endif
# else
#  if defined(__LP64__) || defined(_LP64)
typedef int64_t zend_int_t;
typedef uint64_t zend_uint_t;
typedef off_t zend_off_t;
#   define ZEND_INT_MAX INT64_MAX
#   define ZEND_INT_MIN INT64_MIN
#   define ZEND_UINT_MAX UINT64_MAX
#  else
#   error Cant enable 64 bit integers on non 64 bit platform
#  endif
# endif
# define SIZEOF_ZEND_INT 8
#else
typedef long zend_int_t;
typedef unsigned long zend_uint_t;
typedef long zend_off_t;
# define ZEND_INT_MAX LONG_MAX
# define ZEND_INT_MIN LONG_MIN
# define ZEND_UINT_MAX ULONG_MAX
# define SIZEOF_ZEND_INT SIZEOF_LONG
#endif


/* conversion macros */
#define ZEND_ITOA_BUF_LEN 65

#ifdef ZEND_ENABLE_INT64
# ifdef PHP_WIN32
#  define ZEND_ITOA(i, s, len) _i64toa_s((i), (s), (len), 10)
#  define ZEND_ATOI(i, s) i = _atoi64((s))
#  define ZEND_STRTOL(s0, s1, base) _strtoi64((s0), (s1), (base))
#  define ZEND_STRTOUL(s0, s1, base) _strtoui64((s0), (s1), (base))
#  define ZEND_INT_FMT "%I64d"
#  define ZEND_UINT_FMT "%I64u"
#  define ZEND_INT_FMT_SPEC "I64d"
#  define ZEND_UINT_FMT_SPEC "I64u"
#  define ZEND_STRTOL_PTR _strtoi64
#  define ZEND_STRTOUL_PTR _strtoui64
# else
#  define ZEND_ITOA(i, s, len) \
	do { \
		int st = snprintf((s), (len), "%lld", (i)); \
		(s)[st] = '\0'; \
 	} while (0)
#  define ZEND_ATOI(i, s) (i) = atoll((s))
#  define ZEND_STRTOL(s0, s1, base) strtoll((s0), (s1), (base))
#  define ZEND_STRTOUL(s0, s1, base) strtoull((s0), (s1), (base))
#  define ZEND_INT_FMT "%lld"
#  define ZEND_UINT_FMT "%llu"
#  define ZEND_INT_FMT_SPEC "lld"
#  define ZEND_UINT_FMT_SPEC "llu"
#  define ZEND_STRTOL_PTR strtoll
#  define ZEND_STRTOUL_PTR strtoull
# endif
#else
# define ZEND_STRTOL(s0, s1, base) strtol((s0), (s1), (base))
# define ZEND_STRTOUL(s0, s1, base) strtoul((s0), (s1), (base))
# ifdef PHP_WIN32
#  define ZEND_ITOA(i, s, len) _ltoa_s((i), (s), (len), 10)
#  define ZEND_ATOI(i, s) i = atol((s))
# else
#  define ZEND_ITOA(i, s, len) \
	do { \
		int st = snprintf((s), (len), "%ld", (i)); \
		(s)[st] = '\0'; \
 	} while (0)
#  define ZEND_ATOI(i, s) (i) = atol((s))
# endif
# define ZEND_INT_FMT "%ld"
# define ZEND_UINT_FMT "%lu"
# define ZEND_INT_FMT_SPEC "ld"
# define ZEND_UINT_FMT_SPEC "lu"
# define ZEND_STRTOL_PTR strtol
# define ZEND_STRTOUL_PTR strtoul
#endif


#endif /* ZEND_INT_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
