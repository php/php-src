dnl
dnl $Id$
dnl

AC_MSG_CHECKING(for FastCGI support)
AC_ARG_WITH(fastcgi,
[  --with-fastcgi=SRCDIR   Build PHP as FastCGI application],[
  if test "$withval" = "yes"; then
        FASTCGIPATH=/usr/local
  else
        FASTCGIPATH=$withval
  fi
  test -f "$FASTCGIPATH/lib/libfcgi.a" || AC_MSG_ERROR(Unable to find libfcgi.a in $FASTCGIPATH/lib)
  test -f "$FASTCGIPATH/include/fastcgi.h" || AC_MSG_ERROR(Unable to find fastcgi.h in $FASTCGIPATH/include)
  PHP_SAPI=fastcgi
  PHP_LIBS=$FASTCGIPATH/lib/libfcgi.a
  PHP_ADD_INCLUDE($FASTCGIPATH/include)
  EXT_PROGRAM_LDADD="$EXT_PROGRAM_LDADD $FASTCGIPATH/lib/libfcgi.a"
  INSTALL_IT="\$(INSTALL) -m 0755 $SAPI_PROGRAM \$(bindir)/$SAPI_FASTCGI"
  RESULT=yes
  PHP_SUBST(FASTCGI_LIBADD)
  PHP_SUBST(EXT_PROGRAM_LDADD)
],[
  RESULT=no
])
AC_MSG_RESULT($RESULT)
