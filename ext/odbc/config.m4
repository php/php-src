dnl ODBC_INCDIR
dnl ODBC_INCLUDE
dnl ODBC_LIBDIR
dnl ODBC_LIBS
dnl ODBC_LFLAGS

if test -z "$ODBC_TYPE"; then
AC_MSG_CHECKING(for Adabas support)
AC_ARG_WITH(adabas,
[  --with-adabas[=DIR]     Include Adabas D support.  DIR is the Adabas base
                          install directory, defaults to /usr/local.],
[
  if test "$withval" = "yes"; then
    withval=/usr/local
  fi
  if test "$withval" != "no"; then
    ODBC_INCDIR=$withval/incl
    ODBC_LIBDIR=$withval/lib
    ODBC_LFLAGS=-L$ADA_LIBDIR
    ODBC_INCLUDE=-I$ADA_INCDIR
    ODBC_LIBS="${ADA_LIBDIR}/odbclib.a -lsqlrte -lsqlptc"
    ODBC_TYPE=adabas
    AC_DEFINE(HAVE_ADABAS)
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
fi

if test -z "$ODBC_TYPE"; then
AC_MSG_CHECKING(for Solid support)
AC_ARG_WITH(solid,
[  --with-solid[=DIR]      Include Solid support.  DIR is the Solid base
			  install directory, defaults to /usr/local/solid],
[
  if test "$withval" = "yes"; then
    withval=/usr/local/solid
  fi
  if test "$withval" != "no"; then
    ODBC_INCDIR=$withval/include
    ODBC_LIBDIR=$withval/lib
	  ODBC_INCLUDE=-I$ODBC_INCDIR
	  ODBC_TYPE=solid
	  AC_DEFINE(HAVE_SOLID)
	  AC_MSG_RESULT(yes)
	  AC_FIND_SOLID_LIBS($SOLID_LIBDIR)
  else
	  AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
fi

if test -z "$ODBC_TYPE"; then
AC_MSG_CHECKING(for Empress support)
AC_ARG_WITH(empress,
[  --with-empress[=DIR]    Include Empress support.  DIR is the Empress base
                          install directory, defaults to \$EMPRESSPATH],
[
  if test "$withval" != "no"; then
    if test "$withval" = "yes"; then
      ODBC_INCDIR=$EMPRESSPATH/odbccl/include
      ODBC_LIBDIR=$EMPRESSPATH/odbccl/lib
    else
      ODBC_INCDIR=$withval/include
      ODBC_LIBDIR=$withval/lib
    fi
    ODBC_INCLUDE=-I$ODBC_INCDIR
    ODBC_TYPE=empress
    AC_DEFINE(HAVE_EMPRESS)
    AC_MSG_RESULT(yes)
    AC_FIND_EMPRESS_LIBS($ODBC_LIBDIR)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
fi

if test -z "$ODBC_TYPE"; then
AC_MSG_CHECKING(for Velocis support)
AC_ARG_WITH(velocis,
[  --with-velocis[=DIR]    Include Velocis support.  DIR is the Velocis
                          base install directory, defaults to /usr/local/velocis.],
[
  if test "$withval" != "no"; then
    if test "$withval" = "yes"; then
      ODBC_INCDIR=/usr/local/velocis/include
      ODBC_LIBDIR=/usr/local/velocis
    else
      ODBC_INCDIR=$withval/include
      ODBC_LIBDIR=$withval
    fi
    ODBC_INCLUDE=-I$ODBC_INCDIR
    ODBC_LIBDIR="$ODBC_LIBDIR/bin"
    case `uname` in
      FreeBSD|BSD/OS)
        ODBC_LIBS="$ODBC_LIBDIR/../lib/rdscli.a -lcompat";;
      *)
        ODBC_LIBS="-l_rdbc -l_sql";;
    esac
    ODBC_TYPE=velocis
    AC_DEFINE(HAVE_VELOCIS)
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
fi

if test -z "$ODBC_TYPE"; then
AC_MSG_CHECKING(for a custom ODBC support)
AC_ARG_WITH(custom-odbc,
[  --with-custom-odbc[=DIR]      Include a user defined ODBC support.
                          The DIR is ODBC install base directory, 
                          which defaults to /usr/local.
                          Make sure to define CUSTOM_ODBC_LIBS and
                          have some odbc.h in your include dirs.
                          E.g., you should define following for 
                          Sybase SQL Anywhere 5.5.00 on QNX, prior to
                          run configure script:
                              CFLAGS=\"-DODBC_QNX -DSQLANY_BUG\"
                              LDFLAGS=-lunix
                              CUSTOM_ODBC_LIBS=\"-ldblib -lodbc\".],
[
  if test "$withval" = "yes"; then
    withval=/usr/local
  fi
  if test "$withval" != "no"; then
    ODBC_INCDIR=$withval/include
    ODBC_LIBDIR=$withval/lib
    ODBC_LFLAGS=-L$CODBC_LIBDIR
    ODBC_INCLUDE=-I$CODBC_INCDIR
    ODBC_LIBS=$CUSTOM_ODBC_LIBS
    ODBC_TYPE=custom
    AC_DEFINE(HAVE_CODBC)
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
fi

if test -z "$ODBC_TYPE"; then
AC_MSG_CHECKING(for iODBC support)
AC_ARG_WITH(iodbc,
[  --with-iodbc[=DIR]      Include iODBC support.  DIR is the iODBC base
                          install directory, defaults to /usr/local.],
[
  if test "$withval" = "yes"; then
    withval=/usr/local
  fi
  if test "$withval" != "no"; then
    ODBC_INCDIR=$withval/include
    ODBC_LIBDIR=$withval/lib
    ODBC_LFLAGS=-L$IODBC_LIBDIR
    ODBC_INCLUDE=-I$IODBC_INCDIR
    ODBC_LIBS=-liodbc
    ODBC_TYPE=iodbc
    AC_DEFINE(HAVE_IODBC)
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
fi

if test -z "$ODBC_TYPE"; then
AC_MSG_CHECKING(for OpenLink ODBC support)
AC_ARG_WITH(openlink,
[  --with-openlink[=DIR]   Include OpenLink ODBC support.  DIR is the
                          OpenLink base install directory, defaults to
                          /usr/local/openlink.],
[
  if test "$withval" = "yes"; then
    withval=/usr/local/openlink
  fi
  if test "$withval" != "no"; then
    ODBC_INCDIR=$withval/odbcsdk/include
    ODBC_LIBDIR=$withval/odbcsdk/lib
    ODBC_LFLAGS=-L$OPENLINK_LIBDIR
    ODBC_INCLUDE=-I$OPENLINK_INCDIR
    ODBC_LIBS=-liodbc
    ODBC_TYPE=openlink
    AC_DEFINE(HAVE_OPENLINK)
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
fi

if test -n "$ODBC_TYPE"; then
  INCLUDES="$INCLUDES $ODBC_INCLUDE"
  EXTRA_LIBS="$EXTRA_LIBS $ODBC_LFLAGS $ODBC_LIBS"
  AC_DEFINE(HAVE_UODBC, 1)
  AC_SUBST(ODBC_INCDIR)
  AC_SUBST(ODBC_INCLUDE)
  AC_SUBST(ODBC_LIBDIR)
  AC_SUBST(ODBC_LIBS)
  AC_SUBST(ODBC_LFLAGS)
  AC_SUBST(ODBC_TYPE)
  PHP_EXTENSION(odbc)
fi
