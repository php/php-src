
AC_DEFUN(LIBZEND_BISON_CHECK,[

if test "$YACC" != "bison -y"; then
    AC_MSG_WARN(You will need bison if you want to regenerate the Zend parser.)
else
    AC_MSG_CHECKING(bison version)
    set `bison --version| sed -e 's/^GNU Bison version //' -e 's/\./ /'`
    if test "${1}" = "1" -a "${2}" -lt "25"; then
        AC_MSG_WARN(You will need bison 1.25 if you want to regenerate the Zend parser (found ${1}.${2}).)
    fi
    AC_MSG_RESULT(${1}.${2} (ok))
fi

])

AC_DEFUN(LIBZEND_BASIC_CHECKS,[

AC_REQUIRE([AC_PROG_YACC])
AC_REQUIRE([AC_PROG_CC])
AC_REQUIRE([AC_PROG_CC_C_O])
AC_REQUIRE([AM_PROG_LEX])
AC_REQUIRE([AC_HEADER_STDC])

LIBZEND_BISON_CHECK

dnl Ugly hack to get around a problem with gcc on AIX.
if test "$CC" = "gcc" -a "$ac_cv_prog_cc_g" = "yes" -a \
   "`uname -sv`" = "AIX 4"; then
	CFLAGS=`echo $CFLAGS | sed -e 's/-g//'`
fi

dnl Hack to work around a Mac OS X cpp problem
dnl Known versions needing this workaround are 5.3 and 5.4
if test "$ac_cv_prog_gcc" = "yes" -a "`uname -s`" = "Rhapsody"; then
        CPPFLAGS="$CPPFLAGS -traditional-cpp"
fi

AC_CHECK_HEADERS(
limits.h \
malloc.h \
string.h \
unistd.h \
stdarg.h \
sys/types.h \
signal.h \
unix.h \
dlfcn.h)

AC_TYPE_SIZE_T
AC_TYPE_SIGNAL

AC_CHECK_LIB(c, dlopen, [
 # fake it
 AC_DEFINE(HAVE_LIBDL) ], [
 AC_CHECK_LIB(dl, dlopen, [
  LIBS="-ldl $LIBS"
  AC_DEFINE(HAVE_LIBDL) ], []) ])


dnl This is required for QNX and may be some BSD derived systems
AC_CHECK_TYPE( uint, unsigned int )
AC_CHECK_TYPE( ulong, unsigned long )


dnl Checks for library functions.
AC_FUNC_VPRINTF
AC_FUNC_MEMCMP
AC_FUNC_ALLOCA
AC_CHECK_FUNCS(memcpy strdup getpid kill strtod strtol)
AC_ZEND_BROKEN_SPRINTF
	
AC_SUBST(ZEND_SCANNER)

])





AC_DEFUN(LIBZEND_ENABLE_DEBUG,[

AC_ARG_ENABLE(debug,
[  --disable-debug         Compile without debugging symbols],[
  ZEND_DEBUG=$enableval
],[
  ZEND_DEBUG=yes
])  

])















AC_DEFUN(LIBZEND_OTHER_CHECKS,[

AC_ARG_ENABLE(experimental-zts,
[  --enable-experimental-zts   This will most likely break your build],[
  ZEND_EXPERIMENTAL_ZTS=$enableval
],[
  ZEND_EXPERIMENTAL_ZTS=no
])  
		
AC_ARG_ENABLE(inline,
[  --disable-inline        Disable the inline specifier],[
  ZEND_INLINE=$enableval
],[
  ZEND_INLINE=yes
])

AC_ARG_ENABLE(memory-limit,
[  --enable-memory-limit   Compile with memory limit support. ], [
  ZEND_MEMORY_LIMIT=$enableval
],[
  ZEND_MEMORY_LIMIT=no
])

AC_MSG_CHECKING(whether to enable experimental ZTS)
AC_MSG_RESULT($ZEND_EXPERIMENTAL_ZTS)

AC_MSG_CHECKING(whether to enable inline)
AC_MSG_RESULT($ZEND_INLINE)

AC_MSG_CHECKING(whether to enable a memory limit)
AC_MSG_RESULT($ZEND_MEMORY_LIMIT)

AC_MSG_CHECKING(whether to enable Zend debugging)
AC_MSG_RESULT($ZEND_DEBUG)
	
if test "$ZEND_DEBUG" = "yes"; then
  AC_DEFINE(ZEND_DEBUG,1)
  DEBUG_CFLAGS="-g"
  test -n "$GCC" && DEBUG_CFLAGS="$DEBUG_CFLAGS -Wall"
  test -n "$GCC" && test "$USE_MAINTAINER_MODE" = "yes" && \
    DEBUG_CFLAGS="$DEBUG_CFLAGS -Wmissing-prototypes -Wstrict-prototypes -Wmissing-declarations"
else
  AC_DEFINE(ZEND_DEBUG,0)
fi

test -n "$DEBUG_CFLAGS" && CFLAGS="$CFLAGS $DEBUG_CFLAGS"

if test "$ZEND_EXPERIMENTAL_ZTS" = "yes"; then
  AC_DEFINE(ZTS)
  ZEND_SCANNER_TYPE=cc
  CPPFLAGS="$CPPFLAGS -I../TSRM"
  LIBZEND_CPLUSPLUS_CHECKS
else
  ZEND_SCANNER_TYPE=c
fi  

ZEND_SCANNER="libzend_${ZEND_SCANNER_TYPE}.la"

if test "$ZEND_MEMORY_LIMIT" = "yes"; then
  AC_DEFINE(MEMORY_LIMIT, 1, [Memory limit])
else
  AC_DEFINE(MEMORY_LIMIT, 0, [Memory limit])
fi

if test "$ZEND_INLINE" = "yes"; then
  AC_C_INLINE
else
  AC_DEFINE(inline, [])
fi


])


AC_DEFUN(LIBZEND_CPLUSPLUS_CHECKS,[

dnl extra check to avoid C++ preprocessor testing if in non-ZTS mode
		
if test "$ZEND_EXPERIMENTAL_ZTS" = "yes"; then
AC_PROG_CXX		
AC_LANG_CPLUSPLUS
AC_CHECK_HEADER(stdiostream.h, [ AC_DEFINE(HAVE_STDIOSTREAM_H, [], Whether you have stdiostream.h) ])

AC_CHECK_LIB(C, cin)
AC_CHECK_LIB(g++, cin)
AC_CHECK_LIB(stdc++, cin)
dnl Digital Unix 4.0
AC_CHECK_LIB(cxx, cin)
AC_CHECK_LIB(cxxstd, __array_delete)

AC_MSG_CHECKING(for class istdiostream)
AC_TRY_COMPILE([
#include <sys/types.h>
#include <unistd.h>
#include <fstream.h>
#include <stdiostream.h>
],[
istdiostream *foo = new istdiostream((FILE *) 0);
],[
  AC_DEFINE(HAVE_CLASS_ISTDIOSTREAM, 1, [Whether you have class istdiostream])
  AC_MSG_RESULT(yes)
],[
  AC_MSG_RESULT(no)
])
AC_LANG_C
fi
])

