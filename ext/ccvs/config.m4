dnl $Id$
dnl config.m4 for PHP4 CCVS Extension

AC_MSG_CHECKING(CCVS Support)
AC_ARG_WITH(ccvs,
[  --with-ccvs[=DIR]  Compile CCVS support into PHP4. 
					  Please specify your CCVS base install directory as DIR.
],
[
  if test "$withval" != "no"; then
      if test -n "$CCVS_DIR"; then
	AC_MSG_RESULT(yes)
	PHP_EXTENSION(ccvs)
	LIBS="$LIBS -LCCVS_DIR/lib"
	LIBS="$old_LIBS"
	AC_ADD_LIBRARY_WITH_PATH(ccvs, $CCVS_DIR)
	AC_ADD_INCLUDE($withval/include)
      else
        AC_MSG_RESULT(no)
      fi
fi
],[
  AC_MSG_RESULT(no)
])
