dnl config.m4 for PHP4 MCVE Extension

PHP_ARG_WITH(mcve, for MCVE support,
[  --with-mcve[=DIR]       Include MCVE support. libmcve >= 3.2.2 required])

PHP_ARG_WITH(openssl-dir,OpenSSL dir for MCVE,
[  --with-openssl-dir[=DIR]  MCVE: openssl install prefix.], no, no)

if test "$PHP_MCVE" != "no"; then

  if test "$PHP_OPENSSL_DIR" != "no"; then
    PHP_OPENSSL=$PHP_OPENSSL_DIR
    PHP_SETUP_OPENSSL(SNMP_SHARED_LIBADD, [], [
      AC_MSG_ERROR([MCVE: OpenSSL check failed. Please check config.log for more information.])
    ])
  else
    AC_MSG_RESULT([If configure fails, try adding --with-openssl-dir=<DIR>])
  fi

  case "$PHP_MCVE" in
    yes [)]
      for i in /usr/local/mcve /usr/local /usr; do
        if test -r $i/include/mcve.h; then
          MCVE_DIR=$i
          break
        fi
      done
      ;;
    * [)]
      test -r "$PHP_MCVE/include/mcve.h" && MCVE_DIR=$PHP_MCVE
      ;;
  esac

  if test -z "$MCVE_DIR"; then
    AC_MSG_ERROR([mcve.h NOT FOUND. Please check your MCVE installation.])
  fi

  dnl Check version (M_EXPORT exists in >= 3.2.2)
  saved_CPPFLAGS=$CPPFLAGS
  AC_MSG_CHECKING([for correct libmcve version])
  AC_EGREP_CPP(yes,[
#include "$MCVE_DIR/include/mcve.h"
#ifdef M_EXPORT
yes
#endif
  ],[
    AC_MSG_RESULT([ok])
  ],[
    AC_MSG_ERROR([libmcve 3.2.2 or greater required.])
  ])
  CPPFLAGS=$saved_CPPFLAGS

  PHP_ADD_INCLUDE($MCVE_DIR/include)
  PHP_ADD_LIBRARY_WITH_PATH(mcve, $MCVE_DIR/lib, MCVE_SHARED_LIBADD)

  dnl Build test
  PHP_CHECK_LIBRARY(mcve, MCVE_DestroyEngine,
  [
    AC_DEFINE(HAVE_MCVE, 1, [ ])
  ], [
    AC_MSG_ERROR([MCVE: Sanity check failed. Please check config.log for more information.])
  ], [
    $MCVE_SHARED_LIBADD
  ])

  PHP_NEW_EXTENSION(mcve, mcve.c, $ext_shared)
  PHP_SUBST(MCVE_SHARED_LIBADD)
fi
