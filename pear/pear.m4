
AC_INIT(Makefile.in)

AC_DEFUN(PHP_WITH_PHP_CONFIG,[
  AC_ARG_WITH(php-config,
[  --with-php-config=[PATH]],[
  PHP_CONFIG=$withval
],[
  PHP_CONFIG=php-config
])

  prefix=`$PHP_CONFIG --prefix 2>/dev/null`
  INCLUDES=`$PHP_CONFIG --includes 2>/dev/null`
 
  if test -z "$prefix"; then
    AC_MSG_ERROR(Cannot find php-config. Please use --with-php-config=[PATH])
  fi
  AC_MSG_CHECKING(PHP prefix)
  AC_MSG_RESULT($prefix)
  AC_MSG_CHECKING(PHP includes)
  AC_MSG_RESULT($INCLUDES)
])

php_always_shared=yes

AC_PROG_CC
AC_PROG_CC_C_O

PHP_WITH_PHP_CONFIG

AC_PREFIX_DEFAULT()

sinclude(config.m4)

enable_static=no
enable_shared=yes

AC_PROG_LIBTOOL

SHLIBTOOL='$(LIBTOOL)'
PHP_COMPILE='$(LIBTOOL) --mode=compile $(COMPILE) -c $<'
phplibdir="`pwd`/modules"

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
PHP_SUBST(EXTRA_LDFLAGS)
PHP_SUBST(EXTRA_LIBS)
PHP_SUBST(INCLUDES)
PHP_SUBST(LEX)
PHP_SUBST(LEX_OUTPUT_ROOT)
PHP_SUBST(LFLAGS)
PHP_SUBST(SHLIBTOOL)
PHP_SUBST(LIBTOOL)
PHP_SUBST(SHELL)

PHP_FAST_OUTPUT(Makefile)

PHP_FAST_GENERATE

AC_CONFIG_HEADER(php_config.h)

AC_OUTPUT()
