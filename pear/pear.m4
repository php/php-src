
AC_INIT(Makefile.in)

AC_DEFUN(PHP_WITH_PHP_CONFIG,[
  AC_ARG_WITH(php-config,
[  --with-php-config=PATH],[
  PHP_CONFIG=$withval
],[
  PHP_CONFIG=php-config
])

  prefix=`$PHP_CONFIG --prefix 2>/dev/null`
  INCLUDES=`$PHP_CONFIG --includes 2>/dev/null`
  EXTENSION_DIR=`$PHP_CONFIG --extension-dir`
 
  if test -z "$prefix"; then
    AC_MSG_ERROR(Cannot find php-config. Please use --with-php-config=PATH)
  fi
  AC_MSG_CHECKING(for PHP prefix)
  AC_MSG_RESULT($prefix)
  AC_MSG_CHECKING(for PHP includes)
  AC_MSG_RESULT($INCLUDES)
  AC_MSG_CHECKING(for PHP extension directory)
  AC_MSG_RESULT($EXTENSION_DIR)
])

abs_srcdir=`(cd $srcdir && pwd)`

php_always_shared=yes

PHP_CONFIG_NICE(config.nice)

AC_PROG_CC
AC_PROG_CC_C_O

PHP_WITH_PHP_CONFIG

AC_PREFIX_DEFAULT()

sinclude(config.m4)

enable_static=no
enable_shared=yes

AC_PROG_LIBTOOL

SHARED_LIBTOOL='$(LIBTOOL)'
PHP_COMPILE='$(LIBTOOL) --mode=compile $(COMPILE) -c $<'
phplibdir="`pwd`/modules"
CPPFLAGS="$CPPFLAGS -DHAVE_CONFIG_H"

test "$prefix" = "NONE" && prefix="/usr/local"
test "$exec_prefix" = "NONE" && exec_prefix='$(prefix)'

PHP_SUBST(prefix)
PHP_SUBST(exec_prefix)
PHP_SUBST(libdir)
PHP_SUBST(prefix)
PHP_SUBST(phplibdir)

PHP_SUBST(PHP_COMPILE)
PHP_SUBST(CC)
PHP_SUBST(CFLAGS)
PHP_SUBST(CPP)
PHP_SUBST(CPPFLAGS)
PHP_SUBST(CXX)
PHP_SUBST(DEFS)
PHP_SUBST(EXTENSION_DIR)
PHP_SUBST(EXTRA_LDFLAGS)
PHP_SUBST(EXTRA_LIBS)
PHP_SUBST(INCLUDES)
PHP_SUBST(LEX)
PHP_SUBST(LEX_OUTPUT_ROOT)
PHP_SUBST(LFLAGS)
PHP_SUBST(SHARED_LIBTOOL)
PHP_SUBST(LIBTOOL)
PHP_SUBST(SHELL)

PHP_FAST_OUTPUT(Makefile)

PHP_GEN_CONFIG_VARS
PHP_GEN_MAKEFILES($PHP_FAST_OUTPUT_FILES)

test -d modules || mkdir modules
touch .deps

AC_CONFIG_HEADER(config.h)

AC_OUTPUT()
