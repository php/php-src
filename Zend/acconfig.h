#define ZEND_API
#define ZEND_DLEXPORT

@TOP@

/* This is the default configuration file to read */
#define USE_CONFIG_FILE 1

/* these are defined by automake */
#undef PACKAGE
#undef VERSION

/* define uint by configure if it is missed (QNX and BSD derived) */
#undef uint

/* define ulong by configure if it is missed (most probably is) */
#undef ulong

/* Undefine if you want stricter XML/SGML compliance by default */
/* (this disables "<?expression?>" by default) */
#define T_DEFAULT_SHORT_OPEN_TAG 1

/* Define both of these if you want the bundled REGEX library */
#define REGEX 0
#define HSREGEX 0

#undef ZEND_DEBUG

/* Define if you want to enable memory limit support */
#define MEMORY_LIMIT 0

/* Define to compile Zend thread safe */
#undef ZTS

#undef HAVE_LIBDL

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

/* Define if you have stdiostream.h */
#undef HAVE_STDIOSTREAM_H

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
