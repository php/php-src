dnl
dnl $Id$
dnl

AC_MSG_CHECKING(for CyberCash support)
AC_ARG_WITH(cybercash,
[  --with-cybercash[=DIR]  Include CyberCash support.  DIR is the CyberCash MCK 
                          install directory.],
[
  if test "$withval" != "no"; then
      test -f $withval/mckcrypt.h && MCK_DIR=$withval
      test -f $withval/c-api/mckcrypt.h && MCK_DIR=$withval/c-api
      if test -n "$MCK_DIR"; then
	AC_MSG_RESULT(yes)
	PHP_EXTENSION(cybercash, cybercash.c)
	old_LIBS=$LIBS
	LIBS="$LIBS -L$MCK_DIR/lib"
	AC_CHECK_LIB(mckcrypto,base64_encode,[AC_DEFINE(HAVE_MCK,1,[ ])],
		[AC_MSG_ERROR(Please reinstall the CyberCash MCK - cannot find mckcrypto lib)])
	LIBS=$old_LIBS
	PHP_ADD_LIBRARY_WITH_PATH(mckcrypto, $MCK_DIR/lib)
	PHP_ADD_INCLUDE($MCK_DIR)
      else
        AC_MSG_RESULT(no)
      fi
fi
],[
  AC_MSG_RESULT(no)
])
