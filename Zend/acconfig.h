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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
