#define ZEND_API
#define ZEND_DLEXPORT

@TOP@

/* these are defined by automake */
#undef PACKAGE
#undef VERSION

#undef uint
#undef ulong

/* Define if you want to enable memory limit support */
#define MEMORY_LIMIT 0

@BOTTOM@

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#ifdef HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif

#if ZEND_BROKEN_SPRINTF
int zend_sprintf(char *buffer, const char *format, ...);
#else
# define zend_sprintf sprintf
#endif

#ifdef HAVE_FINITE
#define zend_finite(a) finite(a)
#elif defined(HAVE_ISFINITE)
#define zend_finite(a) isfinite(a)
#elif defined(HAVE_ISNAN) && defined(HAVE_ISINF)
#define zend_finite(a) (isnan(a) ? 0 : isinf(a) ? 0 : 1)
#elif defined(HAVE_ISNAN)
#define zend_finite(a) (isnan(a) ? 0 : 1)
#elif defined(HAVE_ISINF)
#define zend_finite(a) (isinf(a) ? 0 : 1)
#else
#define zend_finite(a) (1)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
