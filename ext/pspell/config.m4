dnl $Id$

AC_MSG_CHECKING(for PSPELL support)
AC_ARG_WITH(pspell,
[  --with-pspell[=DIR]     Include PSPELL support.],
[
  if test "$withval" != "no"; then
    if test "$withval" = "yes"; then
      PSPELL_DIR=/usr/local
    else
      PSPELL_DIR=$withval
    fi

    AC_ADD_INCLUDE($PSPELL_DIR/include)
    AC_ADD_LIBRARY_WITH_PATH(pspell, $PSPELL_DIR/lib)

    if test ! -f "$PSPELL_DIR/include/pspell/pspell.h"; then
      AC_MSG_ERROR(Could not find pspell.h in $PSPELL_DIR/include/pspell - please copy it manually from the pspell sources to $PSPELL_DIR/include/pspell)
    fi
    AC_DEFINE(HAVE_PSPELL,1,[Whether you have pspell])
    AC_MSG_RESULT(yes)
	PHP_EXTENSION(pspell)
  else
    AC_MSG_ERROR(no)
  fi
],[
  AC_MSG_RESULT(no)
])
