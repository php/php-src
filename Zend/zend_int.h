

#ifndef ZEND_INT_H
#define ZEND_INT_H

/* Integer types or the old good long. This part will have to be
   reworked, obviously if we wanna some platform dependent stuff. */
#ifdef ZEND_ENABLE_INT64
# ifdef ZEND_WIN32
#  ifdef _WIN64
typedef __int64 zend_int_t;
typedef unsigned __int64 zend_uint_t;
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
# else
#  define ZEND_ITOA(i, s, len) \
	do { \
		int st = snprintf((s), (len), "%lld", (i)); \
		(s)[st] = '\0'; \
 	} while (0)
#  define ZEND_ATOI(i, s) (i) = atoll((s))
# endif
#else
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
#endif


#endif /* ZEND_INT_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
