dnl $Id$

AC_MSG_CHECKING(for InterBase support)
AC_ARG_WITH(interbase,
[  --with-interbase[=DIR]  Include InterBase support.  DIR is the InterBase base
                          install directory, defaults to /usr/interbase],
[
  if test "$withval" != "no"; then
    if test "$withval" = "yes"; then
      IBASE_INCDIR=/usr/interbase/include
      IBASE_LIBDIR=/usr/interbase/lib
    else
      IBASE_INCDIR=$withval/include
      IBASE_LIBDIR=$withval/lib
    fi
    IBASE_INCLUDE=-I$IBASE_INCDIR
    IBASE_LFLAGS=-L$IBASE_LIBDIR
    IBASE_LIBS="-lgds"
    AC_DEFINE(HAVE_IBASE,1,[ ])
    AC_MSG_RESULT(yes)
    EXTRA_LIBS="$EXTRA_LIBS $IBASE_LFLAGS $IBASE_LIBS"
    INCLUDES="$INCLUDES $IBASE_INCLUDE"
    PHP_EXTENSION(interbase)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
