/************************************************

  defines.h -

  $Author$
  created at: Wed May 18 00:21:44 JST 1994

************************************************/

#ifndef RUBY_DEFINES_H
#define RUBY_DEFINES_H 1

#if defined(__cplusplus)
extern "C" {
#if 0
} /* satisfy cc-mode */
#endif
#endif

#include "ruby/config.h"
#ifdef RUBY_EXTCONF_H
#include RUBY_EXTCONF_H
#endif

#define RUBY

# include <stddef.h>
#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif
#ifdef __cplusplus
# ifndef  HAVE_PROTOTYPES
#  define HAVE_PROTOTYPES 1
# endif
# ifndef  HAVE_STDARG_PROTOTYPES
#  define HAVE_STDARG_PROTOTYPES 1
# endif
#endif

#undef _
#ifdef HAVE_PROTOTYPES
# define _(args) args
#else
# define _(args) ()
#endif

#undef __
#ifdef HAVE_STDARG_PROTOTYPES
# define __(args) args
#else
# define __(args) ()
#endif

#ifdef __cplusplus
#define ANYARGS ...
#else
#define ANYARGS
#endif

#if defined __GNUC__ && __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

#define xmalloc ruby_xmalloc
#define xmalloc2 ruby_xmalloc2
#define xcalloc ruby_xcalloc
#define xrealloc ruby_xrealloc
#define xrealloc2 ruby_xrealloc2
#define xfree ruby_xfree

void *xmalloc(size_t);
void *xmalloc2(size_t,size_t);
void *xcalloc(size_t,size_t);
void *xrealloc(void*,size_t);
void *xrealloc2(void*,size_t,size_t);
void xfree(void*);

#define STRINGIZE(expr) STRINGIZE0(expr)
#ifndef STRINGIZE0
#define STRINGIZE0(expr) #expr
#endif

#if SIZEOF_LONG_LONG > 0
# define LONG_LONG long long
#elif SIZEOF___INT64 > 0
# define HAVE_LONG_LONG 1
# define LONG_LONG __int64
# undef SIZEOF_LONG_LONG
# define SIZEOF_LONG_LONG SIZEOF___INT64
#endif

#if SIZEOF_INT*2 <= SIZEOF_LONG_LONG
# define BDIGIT unsigned int
# define SIZEOF_BDIGITS SIZEOF_INT
# define BDIGIT_DBL unsigned LONG_LONG
# define BDIGIT_DBL_SIGNED LONG_LONG
# define PRI_BDIGIT_PREFIX ""
# define PRI_BDIGIT_DBL_PREFIX PRI_LL_PREFIX
#elif SIZEOF_INT*2 <= SIZEOF_LONG
# define BDIGIT unsigned int
# define SIZEOF_BDIGITS SIZEOF_INT
# define BDIGIT_DBL unsigned long
# define BDIGIT_DBL_SIGNED long
# define PRI_BDIGIT_PREFIX ""
# define PRI_BDIGIT_DBL_PREFIX "l"
#elif SIZEOF_SHORT*2 <= SIZEOF_LONG
# define BDIGIT unsigned short
# define SIZEOF_BDIGITS SIZEOF_SHORT
# define BDIGIT_DBL unsigned long
# define BDIGIT_DBL_SIGNED long
# define PRI_BDIGIT_PREFIX "h"
# define PRI_BDIGIT_DBL_PREFIX "l"
#else
# define BDIGIT unsigned short
# define SIZEOF_BDIGITS (SIZEOF_LONG/2)
# define BDIGIT_DBL unsigned long
# define BDIGIT_DBL_SIGNED long
# define PRI_BDIGIT_PREFIX "h"
# define PRI_BDIGIT_DBL_PREFIX "l"
#endif

#define PRIdBDIGIT PRI_BDIGIT_PREFIX"d"
#define PRIiBDIGIT PRI_BDIGIT_PREFIX"i"
#define PRIoBDIGIT PRI_BDIGIT_PREFIX"o"
#define PRIuBDIGIT PRI_BDIGIT_PREFIX"u"
#define PRIxBDIGIT PRI_BDIGIT_PREFIX"x"
#define PRIXBDIGIT PRI_BDIGIT_PREFIX"X"

#define PRIdBDIGIT_DBL PRI_BDIGIT_DBL_PREFIX"d"
#define PRIiBDIGIT_DBL PRI_BDIGIT_DBL_PREFIX"i"
#define PRIoBDIGIT_DBL PRI_BDIGIT_DBL_PREFIX"o"
#define PRIuBDIGIT_DBL PRI_BDIGIT_DBL_PREFIX"u"
#define PRIxBDIGIT_DBL PRI_BDIGIT_DBL_PREFIX"x"
#define PRIXBDIGIT_DBL PRI_BDIGIT_DBL_PREFIX"X"

#ifdef __CYGWIN__
#undef _WIN32
#endif

#if defined(_WIN32) || defined(__EMX__)
#define DOSISH 1
# define DOSISH_DRIVE_LETTER
#endif

#ifdef AC_APPLE_UNIVERSAL_BUILD
#undef WORDS_BIGENDIAN
#ifdef __BIG_ENDIAN__
#define WORDS_BIGENDIAN
#endif
#endif

#ifdef _WIN32
#include "ruby/win32.h"
#endif

#if defined(__BEOS__) && !defined(__HAIKU__) && !defined(BONE)
#include <net/socket.h> /* intern.h needs fd_set definition */
#elif defined (__SYMBIAN32__) && defined (HAVE_SYS_SELECT_H)
# include <sys/select.h>
#endif

#ifdef __SYMBIAN32__
# define FALSE 0
# define TRUE 1
#endif

#ifdef RUBY_EXPORT
#undef RUBY_EXTERN

#ifndef FALSE
# define FALSE 0
#elif FALSE
# error FALSE must be false
#endif
#ifndef TRUE
# define TRUE 1
#elif !TRUE
# error TRUE must be true
#endif

#endif

#ifndef RUBY_FUNC_EXPORTED
#define RUBY_FUNC_EXPORTED
#endif

#ifndef RUBY_EXTERN
#define RUBY_EXTERN extern
#endif

#ifndef EXTERN
#define EXTERN RUBY_EXTERN	/* deprecated */
#endif

#ifndef RUBY_MBCHAR_MAXSIZE
#define RUBY_MBCHAR_MAXSIZE INT_MAX
        /* MB_CUR_MAX will not work well in C locale */
#endif

#if defined(__sparc)
void rb_sparc_flush_register_windows(void);
#  define FLUSH_REGISTER_WINDOWS rb_sparc_flush_register_windows()
#elif defined(__ia64)
void *rb_ia64_bsp(void);
void rb_ia64_flushrs(void);
#  define FLUSH_REGISTER_WINDOWS rb_ia64_flushrs()
#else
#  define FLUSH_REGISTER_WINDOWS ((void)0)
#endif

#if defined(DOSISH)
#define PATH_SEP ";"
#else
#define PATH_SEP ":"
#endif
#define PATH_SEP_CHAR PATH_SEP[0]

#define PATH_ENV "PATH"

#if defined(DOSISH) && !defined(__EMX__)
#define ENV_IGNORECASE
#endif

#ifndef CASEFOLD_FILESYSTEM
# if defined DOSISH
#   define CASEFOLD_FILESYSTEM 1
# else
#   define CASEFOLD_FILESYSTEM 0
# endif
#endif

#ifndef DLEXT_MAXLEN
#define DLEXT_MAXLEN 4
#endif

#ifndef RUBY_PLATFORM
#define RUBY_PLATFORM "unknown-unknown"
#endif

#ifndef RUBY_ALIAS_FUNCTION_TYPE
#define RUBY_ALIAS_FUNCTION_TYPE(type, prot, name, args) \
    type prot {return name args;}
#endif
#ifndef RUBY_ALIAS_FUNCTION_VOID
#define RUBY_ALIAS_FUNCTION_VOID(prot, name, args) \
    void prot {name args;}
#endif
#ifndef RUBY_ALIAS_FUNCTION
#define RUBY_ALIAS_FUNCTION(prot, name, args) \
    RUBY_ALIAS_FUNCTION_TYPE(VALUE, prot, name, args)
#endif

#if defined __GNUC__ && __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#if defined(__cplusplus)
#if 0
{ /* satisfy cc-mode */
#endif
}  /* extern "C" { */
#endif

#endif /* RUBY_DEFINES_H */
