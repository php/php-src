dnl $Id$
dnl config.m4 for PHP4 CCVS Extension

AC_MSG_CHECKING(CCVS Support)
AC_ARG_WITH(ccvs,
[  --with-ccvs[=DIR]  Compile CCVS support into PHP4. 
					  Please specify your CCVS base install directory as DIR.
],
[
  if test "$withval" != "no"; then
     CCVS_DIR="$withval"
     CCVS_LIB_DIR="$CCVS_DIR/lib"
     CCVS_INCLUDE_DIR="$CCVS_DIR/include"
     	if test -n "$CCVS_DIR"; then
		AC_MSG_RESULT(yes)
		PHP_EXTENSION(ccvs)
		LIBS="$LIBS -LCCVS_DIR/lib"
		AC_ADD_LIBRARY_WITH_PATH(ccvs, $CCVS_LIB_DIR)
		AC_ADD_INCLUDE($CCVS_INCLUDE_DIR)
    	  else
    	    AC_MSG_RESULT(no)
    	  fi
	fi
],[
  AC_MSG_RESULT(no)
])
