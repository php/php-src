dnl
dnl $Id$
dnl

PHP_ARG_WITH(cyrus, for cyrus imap support,
[  --with-cyrus[=dir]      Include Cyrus IMAP support])

if test "$PHP_CYRUS" != "no"; then
  found_cyrus=no
  found_sasl=no
  found_openssl=no

  dnl
  dnl Cyrus
  dnl
  for i in $PHP_CYRUS /usr/local /usr; do
    if test -r $i/include/cyrus/imclient.h && test "$found_cyrus" = "no"; then
      PHP_ADD_INCLUDE($i/include)
      PHP_ADD_LIBRARY_WITH_PATH(cyrus, $i/lib, CYRUS_SHARED_LIBADD)
      found_cyrus=yes
      CYRUS_INCPATH=$i/include
      break 2
    fi
  done

  dnl Check that imclient_connect() accepts 4 args
  old_CPPFLAGS=$CPPFLAGS
  CPPFLAGS=-I$CYRUS_INCPATH
  AC_TRY_COMPILE([#include <stdio.h>
#include <cyrus/imclient.h>], [imclient_connect(0,0,0,0)], [], [
    AC_MSG_ERROR(cyrus-imap version 2.0.8 or greater required)
  ])
  CPPFLAGS=$old_CPPFLAGS
  
  if test "$found_cyrus" = "no"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please Re-install the cyrus distribution)
  fi

  dnl
  dnl sasl/sasl2 
  dnl
  for i in $PHP_CYRUS /usr/local /usr; do
    if test -r $i/include/sasl.h && test "$found_sasl" = "no"; then
      PHP_ADD_INCLUDE($i/include)
      found_sasl=yes
    elif test -r $i/include/sasl/sasl.h && test "$found_sasl" = "no"; then
      PHP_ADD_INCLUDE($i/include/sasl)
      found_sasl=yes
    fi

    if test "$found_sasl" = "yes"; then
      if test -f $i/lib/libsasl2.a || test -f $i/lib/libsasl2.$SHLIB_SUFFIX_NAME; then
        PHP_ADD_LIBRARY_WITH_PATH(sasl2, $i/lib, CYRUS_SHARED_LIBADD)
      else
        PHP_ADD_LIBRARY_WITH_PATH(sasl, $i/lib, CYRUS_SHARED_LIBADD)
      fi
      break 2
    fi
  done
  
  if test "$found_sasl" = "no"; then
    AC_MSG_RESULT(sasl not found)
    AC_MSG_ERROR(Please Re-install the cyrus distribution)
  fi

  dnl
  dnl OpenSSL
  dnl
  if test "$PHP_OPENSSL" = "no"; then
    for i in $PHP_CYRUS /usr/local /usr; do
      if test -r $i/include/openssl/ssl.h; then
        PHP_ADD_LIBRARY_WITH_PATH(ssl, $i/lib, CYRUS_SHARED_LIBADD)
        PHP_ADD_LIBRARY_WITH_PATH(crypto, $i/lib, CYRUS_SHARED_LIBADD)
        break 2
      fi
    done
  fi
  
  PHP_NEW_EXTENSION(cyrus, cyrus.c, $ext_shared)
  PHP_SUBST(CYRUS_SHARED_LIBADD)
  AC_DEFINE(HAVE_CYRUS,1,[ ])
fi
