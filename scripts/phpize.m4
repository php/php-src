dnl This file becomes configure.in for self-contained extensions.

divert(1)

AC_PREREQ(2.13)
AC_INIT(config.m4)

PHP_CONFIG_NICE(config.nice)

AC_DEFUN([PHP_WITH_PHP_CONFIG],[
  AC_ARG_WITH(php-config,
[  --with-php-config=PATH],[
  PHP_CONFIG=$withval
],[
  PHP_CONFIG=php-config
])

  prefix=`$PHP_CONFIG --prefix 2>/dev/null`
  phpincludedir=`$PHP_CONFIG --include-dir 2>/dev/null`
  INCLUDES=`$PHP_CONFIG --includes 2>/dev/null`
  EXTENSION_DIR=`$PHP_CONFIG --extension-dir`
  PHP_EXECUTABLE=`$PHP_CONFIG --php-binary`
 
  if test -z "$prefix"; then
    AC_MSG_ERROR(Cannot find php-config. Please use --with-php-config=PATH)
  fi

  php_shtool=$srcdir/build/shtool
  PHP_INIT_BUILD_SYSTEM

  AC_MSG_CHECKING(for PHP prefix)
  AC_MSG_RESULT($prefix)
  AC_MSG_CHECKING(for PHP includes)
  AC_MSG_RESULT($INCLUDES)
  AC_MSG_CHECKING(for PHP extension directory)
  AC_MSG_RESULT($EXTENSION_DIR)
  AC_MSG_CHECKING(for PHP installed headers prefix)
  AC_MSG_RESULT($phpincludedir)
])
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
abs_srcdir=`(cd $srcdir && pwd)`
abs_builddir=`pwd`

AC_PROG_CC
AC_PROG_CC_C_O

dnl Support systems with system libraries in e.g. /usr/lib64
AC_ARG_WITH(libdir,
[  --with-libdir=NAME      Look for libraries in .../NAME rather than .../lib],
[PHP_LIBDIR=$withval], [PHP_LIBDIR=lib])

PHP_RUNPATH_SWITCH
PHP_SHLIB_SUFFIX_NAMES
PHP_WITH_PHP_CONFIG

PHP_BUILD_SHARED

AC_PREFIX_DEFAULT()

PHP_PROG_RE2C
PHP_PROG_AWK
    
sinclude(config.m4)

enable_static=no
enable_shared=yes

AC_PROG_LIBTOOL

all_targets='$(PHP_MODULES)'
install_targets="install-modules install-headers"
phplibdir="`pwd`/modules"
CPPFLAGS="$CPPFLAGS -DHAVE_CONFIG_H"
CFLAGS_CLEAN='$(CFLAGS)'

test "$prefix" = "NONE" && prefix="/usr/local"
test "$exec_prefix" = "NONE" && exec_prefix='$(prefix)'

PHP_SUBST(PHP_MODULES)
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
