dnl $Id$

AC_MSG_CHECKING(for Sybase support)
AC_ARG_WITH(sybase,
[  --with-sybase[=DIR]     Include Sybase-DB support.  DIR is the Sybase home
                          directory, defaults to /home/sybase.],
[
  if test "$withval" != "no"; then
    if test "$withval" = "yes"; then
      SYBASE_INCDIR=/home/sybase/include
      SYBASE_LIBDIR=/home/sybase/lib
    else
      SYBASE_INCDIR=$withval/include
      SYBASE_LIBDIR=$withval/lib
    fi
    SYBASE_INCLUDE=-I$SYBASE_INCDIR
    SYBASE_LFLAGS="-L$SYBASE_LIBDIR -L$SYBASE_LIBDIR"
    SYBASE_LIBS=-lsybdb
    AC_MSG_RESULT(yes)
    PHP_EXTENSION(sybase)
    AC_CHECK_LIB(dnet_stub, dnet_addr,
     [ SYBASE_LIBS="$SYBASE_LIBS -ldnet_stub"
        AC_DEFINE(HAVE_LIBDNET_STUB)
     ])
    AC_DEFINE(HAVE_SYBASE)
    EXTRA_LIBS="$EXTRA_LIBS $SYBASE_LFLAGS $SYBASE_LIBS"
    INCLUDES="$INCLUDES $SYBASE_INCLUDE"
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])


AC_MSG_CHECKING(for Sybase-CT support)
AC_ARG_WITH(sybase-ct,
[  --with-sybase-ct[=DIR]  Include Sybase-CT support.  DIR is the Sybase home
                          directory, defaults to /home/sybase.],
[
  if test "$withval" != "no"; then
    AC_MSG_RESULT(yes)
    if test -z "$SYBASE_LIBS"; then
      PHP_EXTENSION(sybase)
    fi
    AC_DEFINE(HAVE_SYBASE_CT)
    if test "$withval" = "yes"; then
      SYBASE_CT_INCDIR=/home/sybase/include
      SYBASE_CT_LIBDIR=/home/sybase/lib
    else
      SYBASE_CT_INCDIR=$withval/include
      SYBASE_CT_LIBDIR=$withval/lib
    fi
    SYBASE_CT_INCLUDE=-I$SYBASE_CT_INCDIR
    SYBASE_CT_LFLAGS="-L$SYBASE_CT_LIBDIR"
    SYBASE_CT_LIBS="-lcs -lct -lcomn -lintl"
    old_LDFLAGS=$LDFLAGS
    LDFLAGS="$LDFLAGS -L$SYBASE_CT_LIBDIR"
    AC_CHECK_LIB(tcl, netg_errstr,
              [ SYBASE_CT_LIBS="$SYBASE_CT_LIBS -ltcl" ],
              [ SYBASE_CT_LIBS="$SYBASE_CT_LIBS -lsybtcl" ],
              [ $SYBASE_CT_LIBS ])
    AC_CHECK_LIB(insck, insck__getVdate,
              [ SYBASE_CT_LIBS="$SYBASE_CT_LIBS -linsck" ])
      LDFLAGS=$old_LDFLAGS
    EXTRA_LIBS="$EXTRA_LIBS $SYBASE_CT_LFLAGS $SYBASE_CT_LIBS"
    INCLUDES="$INCLUDES $SYBASE_CT_INCLUDE"
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
