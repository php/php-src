dnl $Id$

AC_MSG_CHECKING(for XML support)
AC_ARG_WITH(xml,
[  --with-xml              Include XML support],[
  if test "$withval" != "no"; then
    if test "$withval" = "yes"; then
      XML_LIBS="-lexpat"
      XML_INCLUDE=""
    else
      XML_LIBS="-L$withval/lib -lexpat"
      XML_INCLUDE="-I$withval/include"
    fi
    AC_DEFINE(HAVE_LIBEXPAT, 1)
    AC_MSG_RESULT(yes)
    PHP_EXTENSION(xml)
    EXTRA_LIBS="$EXTRA_LIBS $XML_LIBS"
    INCLUDES="$INCLUDES $XML_INCLUDE"
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
]) 
