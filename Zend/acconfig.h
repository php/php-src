#undef HAVE_CLASS_ISTDIOSTREAM

#define ZEND_API

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

/* Define if you want to enable bc style precision math support */
#define WITH_BCMATH 0

/* Define if you want to enable memory limit support */
#define MEMORY_LIMIT 0

/* Define if you have broken sprintf function like SunOS 4 */
#define BROKEN_SPRINTF 0

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

#ifdef HAVE_DLFCN_H
# include <dlfcn.h>
#endif

#if defined(HAVE_LIBDL) && defined(RTLD_NOW)
# define DL_LOAD(libname)	dlopen(libname, RTLD_NOW)
# define DL_UNLOAD		dlclose
# define DL_FETCH_SYMBOL	dlsym
# define DL_HANDLE		void *
# define ZEND_EXTENSIONS_SUPPORT 1
#else
# define DL_HANDLE		void *
# define ZEND_EXTENSIONS_SUPPORT 0
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
