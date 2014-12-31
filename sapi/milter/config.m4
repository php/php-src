dnl
dnl $Id$
dnl

PHP_ARG_WITH(milter, for Milter support,
[  --with-milter[=DIR]       Build PHP as Milter application], no, no)

if test "$PHP_MILTER" != "no"; then
  if test "$PHP_MILTER" = "yes"; then
    if test -f /usr/lib/libmilter.a ; then
      MILTERPATH=/usr/lib
    else
      if test -f /usr/lib/libmilter/libmilter.a ; then
        MILTERPATH=/usr/lib/libmilter
      else
        AC_MSG_ERROR([Unable to find libmilter.a])
      fi
    fi
  else
    MILTERPATH=$PHP_MILTER
  fi
  
  SAPI_MILTER_PATH=sapi/milter/php-milter
  PHP_BUILD_THREAD_SAFE
  PHP_ADD_MAKEFILE_FRAGMENT($abs_srcdir/sapi/milter/Makefile.frag,$abs_srcdir/sapi/milter,sapi/milter)
  PHP_SELECT_SAPI(milter, program, php_milter.c getopt.c,,'$(SAPI_MILTER_PATH)') 
  PHP_ADD_LIBRARY_WITH_PATH(milter, $MILTERPATH,)
  BUILD_MILTER="\$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS) \$(PHP_BINARY_OBJS) \$(PHP_MILTER_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_MILTER_PATH)"
  PHP_SUBST(SAPI_MILTER_PATH)
  PHP_SUBST(BUILD_MILTER)
fi
