dnl This file becomes configure.in for self-contained extensions.

divert(1)

AC_PREREQ(2.13)
AC_INIT(config.m4)

PHP_CONFIG_NICE(config.nice)

dnl 
AC_DEFUN([PHP_EXT_BUILDDIR],[.])dnl
AC_DEFUN([PHP_EXT_DIR],[""])dnl
AC_DEFUN([PHP_EXT_SRCDIR],[$abs_srcdir])dnl
AC_DEFUN([PHP_ALWAYS_SHARED],[
  ext_output="yes, shared"
  ext_shared=yes
  test "[$]$1" = "no" && $1=yes
])dnl
dnl

test -z "$CFLAGS" && auto_cflags=1

abs_srcdir=`(cd $srcdir && pwd)`
abs_builddir=`pwd`

AC_PROG_CC([cc gcc])
PHP_DETECT_ICC
PHP_DETECT_SUNCC
AC_PROG_CC_C_O

dnl Support systems with system libraries in e.g. /usr/lib64
PHP_ARG_WITH(libdir, for system library directory,
[  --with-libdir=NAME      Look for libraries in .../NAME rather than .../lib], lib, no)

PHP_RUNPATH_SWITCH
PHP_SHLIB_SUFFIX_NAMES

dnl Find php-config script
PHP_ARG_WITH(php-config,,
[  --with-php-config=PATH  Path to php-config [php-config]], php-config, no)

dnl For BC
PHP_CONFIG=$PHP_PHP_CONFIG
prefix=`$PHP_CONFIG --prefix 2>/dev/null`
phpincludedir=`$PHP_CONFIG --include-dir 2>/dev/null`
INCLUDES=`$PHP_CONFIG --includes 2>/dev/null`
EXTENSION_DIR=`$PHP_CONFIG --extension-dir 2>/dev/null`
PHP_EXECUTABLE=`$PHP_CONFIG --php-binary 2>/dev/null`
 
if test -z "$prefix"; then
  AC_MSG_ERROR([Cannot find php-config. Please use --with-php-config=PATH])
fi

php_shtool=$srcdir/build/shtool
PHP_INIT_BUILD_SYSTEM

AC_MSG_CHECKING([for PHP prefix])
AC_MSG_RESULT([$prefix])
AC_MSG_CHECKING([for PHP includes])
AC_MSG_RESULT([$INCLUDES])
AC_MSG_CHECKING([for PHP extension directory])
AC_MSG_RESULT([$EXTENSION_DIR])
AC_MSG_CHECKING([for PHP installed headers prefix])
AC_MSG_RESULT([$phpincludedir])

dnl Checks for PHP_DEBUG / ZEND_DEBUG / ZTS
AC_MSG_CHECKING([if debug is enabled])
old_CPPFLAGS=$CPPFLAGS
CPPFLAGS="-I$phpincludedir"
AC_EGREP_CPP(php_debug_is_enabled,[
#include <main/php_config.h>
#if ZEND_DEBUG
php_debug_is_enabled
#endif
],[
  PHP_DEBUG=yes
],[
  PHP_DEBUG=no
])
CPPFLAGS=$old_CPPFLAGS
AC_MSG_RESULT([$PHP_DEBUG])

AC_MSG_CHECKING([if zts is enabled])
old_CPPFLAGS=$CPPFLAGS
CPPFLAGS="-I$phpincludedir"
AC_EGREP_CPP(php_zts_is_enabled,[
#include <main/php_config.h>
#if ZTS
php_zts_is_enabled
#endif
],[
  PHP_THREAD_SAFETY=yes
],[
  PHP_THREAD_SAFETY=no
])
CPPFLAGS=$old_CPPFLAGS
AC_MSG_RESULT([$PHP_DEBUG])

dnl Support for building and testing Zend extensions
ZEND_EXT_TYPE="zend_extension"
PHP_SUBST(ZEND_EXT_TYPE)

dnl Discard optimization flags when debugging is enabled
if test "$PHP_DEBUG" = "yes"; then
  PHP_DEBUG=1
  ZEND_DEBUG=yes
  changequote({,})
  CFLAGS=`echo "$CFLAGS" | $SED -e 's/-O[0-9s]*//g'`
  CXXFLAGS=`echo "$CXXFLAGS" | $SED -e 's/-O[0-9s]*//g'`
  changequote([,])
  dnl add -O0 only if GCC or ICC is used
  if test "$GCC" = "yes" || test "$ICC" = "yes"; then
    CFLAGS="$CFLAGS -O0"
    CXXFLAGS="$CXXFLAGS -g -O0"
  fi
  if test "$SUNCC" = "yes"; then
    if test -n "$auto_cflags"; then
      CFLAGS="-g"
      CXXFLAGS="-g"
    else
      CFLAGS="$CFLAGS -g"
      CXXFLAGS="$CFLAGS -g"
    fi
  fi
else
  PHP_DEBUG=0
  ZEND_DEBUG=no
fi

dnl Always shared
PHP_BUILD_SHARED

dnl Required programs
PHP_PROG_RE2C
PHP_PROG_AWK

sinclude(config.m4)

enable_static=no
enable_shared=yes

dnl Only allow AC_PROG_CXX and AC_PROG_CXXCPP if they are explicitly called (by PHP_REQUIRE_CXX).
dnl Otherwise AC_PROG_LIBTOOL fails if there is no working C++ compiler.
AC_PROVIDE_IFELSE([PHP_REQUIRE_CXX], [], [
  undefine([AC_PROG_CXX])
  AC_DEFUN([AC_PROG_CXX], [])
  undefine([AC_PROG_CXXCPP])
  AC_DEFUN([AC_PROG_CXXCPP], [php_prog_cxxcpp=disabled])
])
AC_PROG_LIBTOOL

all_targets='$(PHP_MODULES) $(PHP_ZEND_EX)'
install_targets="install-modules install-headers"
phplibdir="`pwd`/modules"
CPPFLAGS="$CPPFLAGS -DHAVE_CONFIG_H"
CFLAGS_CLEAN='$(CFLAGS)'
CXXFLAGS_CLEAN='$(CXXFLAGS)'

test "$prefix" = "NONE" && prefix="/usr/local"
test "$exec_prefix" = "NONE" && exec_prefix='$(prefix)'

PHP_SUBST(PHP_MODULES)
PHP_SUBST(PHP_ZEND_EX)

PHP_SUBST(all_targets)
PHP_SUBST(install_targets)

PHP_SUBST(prefix)
PHP_SUBST(exec_prefix)
PHP_SUBST(libdir)
PHP_SUBST(prefix)
PHP_SUBST(phplibdir)
PHP_SUBST(phpincludedir)

PHP_SUBST(CC)
PHP_SUBST(CFLAGS)
PHP_SUBST(CFLAGS_CLEAN)
PHP_SUBST(CPP)
PHP_SUBST(CPPFLAGS)
PHP_SUBST(CXX)
PHP_SUBST(CXXFLAGS)
PHP_SUBST(CXXFLAGS_CLEAN)
PHP_SUBST(EXTENSION_DIR)
PHP_SUBST(PHP_EXECUTABLE)
PHP_SUBST(EXTRA_LDFLAGS)
PHP_SUBST(EXTRA_LIBS)
PHP_SUBST(INCLUDES)
PHP_SUBST(LFLAGS)
PHP_SUBST(LDFLAGS)
PHP_SUBST(SHARED_LIBTOOL)
PHP_SUBST(LIBTOOL)
PHP_SUBST(SHELL)
PHP_SUBST(INSTALL_HEADERS)

PHP_GEN_BUILD_DIRS
PHP_GEN_GLOBAL_MAKEFILE

test -d modules || $php_shtool mkdir modules
touch .deps

AC_CONFIG_HEADER(config.h)

AC_OUTPUT()
