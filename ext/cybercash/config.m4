dnl config.m4 for extension CyberCash
dnl don't forget to call PHP_EXTENSION(cybercash)

AC_MSG_CHECKING(for CyberCash support)
AC_ARG_WITH(cybercash,
[  --with-cybercash[=DIR]  Include CyberCash support.  DIR is the CyberCash MCK 
			  install directory.],
[
  if test "$withval" != "no"; then
      test -f $withval/mckcrypt.h && MCK_DIR="$withval"
      test -f $withval/c-api/mckcrypt.h && MCK_DIR="$withval/c-api"
      if test -n "$MCK_DIR"; then
	AC_MSG_RESULT(yes)
	PHP_EXTENSION(cybercash)
	LIBS="$LIBS -L$MCK_DIR/lib"
	AC_ADD_LIBRARY_WITH_PATH(mckcrypto, $MCK_DIR/lib)
	AC_ADD_INCLUDE($MCK_DIR)
	AC_DEFINE(HAVE_MCK)
      else
        AC_MSG_ERROR(Please reinstall the CyberCash MCK - I cannot find mckcrypt.h)
        AC_MSG_RESULT(no)
      fi
fi
],[
  AC_MSG_RESULT(no)
])
