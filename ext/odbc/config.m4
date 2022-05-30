AC_DEFUN([PHP_ODBC_CHECK_HEADER],[
if ! test -f "$ODBC_INCDIR/$1"; then
  AC_MSG_ERROR([ODBC header file '$ODBC_INCDIR/$1' not found!])
fi
])

dnl
dnl Figure out which library file to link with for the Solid support.
dnl
AC_DEFUN([PHP_ODBC_FIND_SOLID_LIBS],[
  AC_MSG_CHECKING([Solid library file])
  ac_solid_uname_r=`uname -r 2>/dev/null`
  ac_solid_uname_s=`uname -s 2>/dev/null`
  case $ac_solid_uname_s in
    AIX) ac_solid_os=a3x;;   # a4x for AIX4/ Solid 2.3/3.0 only
    HP-UX) ac_solid_os=h9x;; # h1x for hpux11, h0x for hpux10
    IRIX) ac_solid_os=irx;;  # Solid 2.3(?)/ 3.0 only
    Linux)
      if ldd -v /bin/sh | grep GLIBC > /dev/null; then
        AC_DEFINE(SS_LINUX,1,[Needed in sqlunix.h ])
        ac_solid_os=l2x
      else
        AC_DEFINE(SS_LINUX,1,[Needed in sqlunix.h ])
        ac_solid_os=lux
      fi;;
    SunOS)
      ac_solid_os=ssx;; # should we deal with SunOS 4?
    FreeBSD)
      if test `expr $ac_solid_uname_r : '\(.\)'` -gt "2"; then
        AC_DEFINE(SS_FBX,1,[Needed in sqlunix.h for wchar defs ])
        ac_solid_os=fex
      else
        AC_DEFINE(SS_FBX,1,[Needed in sqlunix.h for wchar defs ])
        ac_solid_os=fbx
      fi;;
  esac

  if test -f $1/soc${ac_solid_os}35.a; then
    ac_solid_version=35
    ac_solid_prefix=soc
  elif test -f $1/scl${ac_solid_os}30.a; then
    ac_solid_version=30
    ac_solid_prefix=scl
  elif test -f $1/scl${ac_solid_os}23.a; then
    ac_solid_version=23
    ac_solid_prefix=scl
  fi

dnl Check for the library files, and setup the ODBC_LIBS path.
if test ! -f $1/lib${ac_solid_prefix}${ac_solid_os}${ac_solid_version}.so -a \
  ! -f $1/lib${ac_solid_prefix}${ac_solid_os}${ac_solid_version}.a; then
  dnl we have an error and should bail out, as we can't find the libs!
  echo ""
  echo "*********************************************************************"
  echo "* Unable to locate $1/lib${ac_solid_prefix}${ac_solid_os}${ac_solid_version}.so or $1/lib${ac_solid_prefix}${ac_solid_os}${ac_solid_version}.a"
  echo "* Please correct this by creating the following links and reconfiguring:"
  echo "* $1/lib${ac_solid_prefix}${ac_solid_os}${ac_solid_version}.a -> $1/lib${ac_solid_prefix}${ac_solid_os}${ac_solid_version}.a"
  echo "* $1/${ac_solid_prefix}${ac_solid_os}${ac_solid_version}.so -> $1/lib${ac_solid_prefix}${ac_solid_os}${ac_solid_version}.so"
  echo "*********************************************************************"
else
  ODBC_LFLAGS=-L$1
  ODBC_LIBS=-l${ac_solid_prefix}${ac_solid_os}${ac_solid_version}
fi

  AC_MSG_RESULT(`echo $ODBC_LIBS | sed -e 's!.*/!!'`)
])


dnl
dnl Figure out which library file to link with for the Empress support.
dnl

AC_DEFUN([PHP_ODBC_FIND_EMPRESS_LIBS],[
  AC_MSG_CHECKING([Empress library file])
  ODBC_LIBS=`echo $1/libempodbccl.so | cut -d' ' -f1`
  if test ! -f $ODBC_LIBS; then
    ODBC_LIBS=`echo $1/libempodbccl.so | cut -d' ' -f1`
  fi
  AC_MSG_RESULT(`echo $ODBC_LIBS | sed -e 's!.*/!!'`)
])

AC_DEFUN([PHP_ODBC_FIND_EMPRESS_BCS_LIBS],[
  AC_MSG_CHECKING([Empress local access library file])
  ODBCBCS_LIBS=`echo $1/libempodbcbcs.a | cut -d' ' -f1`
  if test ! -f $ODBCBCS_LIBS; then
    ODBCBCS_LIBS=`echo $1/libempodbcbcs.a | cut -d' ' -f1`
  fi
  AC_MSG_RESULT(`echo $ODBCBCS_LIBS | sed -e 's!.*/!!'`)
])

dnl
dnl configure options
dnl

PHP_ARG_WITH([odbcver],,
  [AS_HELP_STRING([[--with-odbcver[=HEX]]],
    [Force support for the passed ODBC version. A hex number is expected,
    default 0x0350. Use the special value of 0 to prevent an explicit ODBCVER to
    be defined.])],
  [0x0350])

if test -z "$ODBC_TYPE"; then
PHP_ARG_WITH([adabas],,
  [AS_HELP_STRING([[--with-adabas[=DIR]]],
    [Include Adabas D support [/usr/local]])])

  AC_MSG_CHECKING([for Adabas support])
  if test "$PHP_ADABAS" != "no"; then
    if test "$PHP_ADABAS" = "yes"; then
      PHP_ADABAS=/usr/local
    fi
    PHP_ADD_INCLUDE($PHP_ADABAS/incl)
    PHP_ADD_LIBPATH($PHP_ADABAS/$PHP_LIBDIR)
    ODBC_OBJS="$PHP_ADABAS/$PHP_LIBDIR/odbclib.a"
    ODBC_LIB="$abs_builddir/ext/odbc/libodbc_adabas.a"
    $srcdir/build/shtool mkdir -f -p ext/odbc
    rm -f "$ODBC_LIB"
    cp "$ODBC_OBJS" "$ODBC_LIB"
    PHP_ADD_LIBRARY(sqlptc)
    PHP_ADD_LIBRARY(sqlrte)
    PHP_ADD_LIBRARY_WITH_PATH(odbc_adabas, $abs_builddir/ext/odbc)
    ODBC_TYPE=adabas
    ODBC_INCDIR=$PHP_ADABAS/incl
    PHP_ODBC_CHECK_HEADER(sqlext.h)
    AC_DEFINE(HAVE_ADABAS,1,[ ])
    AC_MSG_RESULT([$ext_output])
  else
    AC_MSG_RESULT(no)
  fi
fi

if test -z "$ODBC_TYPE"; then
PHP_ARG_WITH([sapdb],,
  [AS_HELP_STRING([[--with-sapdb[=DIR]]],
    [Include SAP DB support [/usr/local]])])

  AC_MSG_CHECKING([for SAP DB support])
  if test "$PHP_SAPDB" != "no"; then
    if test "$PHP_SAPDB" = "yes"; then
      PHP_SAPDB=/usr/local
    fi
    PHP_ADD_INCLUDE($PHP_SAPDB/incl)
    PHP_ADD_LIBPATH($PHP_SAPDB/$PHP_LIBDIR)
    PHP_ADD_LIBRARY(sqlod)
    ODBC_TYPE=sapdb
    AC_DEFINE(HAVE_SAPDB,1,[ ])
    AC_MSG_RESULT([$ext_output])
  else
    AC_MSG_RESULT(no)
  fi
fi

if test -z "$ODBC_TYPE"; then
PHP_ARG_WITH([solid],,
  [AS_HELP_STRING([[--with-solid[=DIR]]],
    [Include Solid support [/usr/local/solid]])])

  AC_MSG_CHECKING(for Solid support)
  if test "$PHP_SOLID" != "no"; then
    if test "$PHP_SOLID" = "yes"; then
      PHP_SOLID=/usr/local/solid
    fi
    ODBC_INCDIR=$PHP_SOLID/include
    ODBC_LIBDIR=$PHP_SOLID/$PHP_LIBDIR
    ODBC_CFLAGS=-I$ODBC_INCDIR
    ODBC_TYPE=solid
    if test -f $ODBC_LIBDIR/soc*35.a; then
      AC_DEFINE(HAVE_SOLID_35,1,[ ])
    elif test -f $ODBC_LIBDIR/scl*30.a; then
      AC_DEFINE(HAVE_SOLID_30,1,[ ])
    elif test -f $ODBC_LIBDIR/scl*23.a; then
      AC_DEFINE(HAVE_SOLID,1,[ ])
    fi
    AC_MSG_RESULT([$ext_output])
    PHP_ODBC_FIND_SOLID_LIBS($ODBC_LIBDIR)
  else
    AC_MSG_RESULT(no)
  fi
fi

if test -z "$ODBC_TYPE"; then
PHP_ARG_WITH([ibm-db2],,
  [AS_HELP_STRING([[--with-ibm-db2[=DIR]]],
    [Include IBM DB2 support [/home/db2inst1/sqllib]])])

  AC_MSG_CHECKING(for IBM DB2 support)
  if test "$PHP_IBM_DB2" != "no"; then
    if test "$PHP_IBM_DB2" = "yes"; then
      ODBC_INCDIR=/home/db2inst1/sqllib/include
      ODBC_LIBDIR=/home/db2inst1/sqllib/lib
    else
      ODBC_INCDIR=$PHP_IBM_DB2/include
      ODBC_LIBDIR=$PHP_IBM_DB2/$PHP_LIBDIR
    fi

    PHP_ODBC_CHECK_HEADER(sqlcli1.h)

    ODBC_CFLAGS=-I$ODBC_INCDIR
    ODBC_LFLAGS=-L$ODBC_LIBDIR
    ODBC_TYPE=ibm-db2
    ODBC_LIBS=-ldb2

    PHP_TEST_BUILD(SQLExecute, [
      AC_DEFINE(HAVE_IBMDB2,1,[ ])
      AC_MSG_RESULT([$ext_output])
    ], [
      AC_MSG_RESULT(no)
      AC_MSG_ERROR([
build test failed. Please check the config.log for details.
You need to source your DB2 environment before running PHP configure:
# . \$IBM_DB2/db2profile
])
    ], [
      $ODBC_LFLAGS $ODBC_LIBS
    ])
  else
    AC_MSG_RESULT(no)
  fi
fi

if test -z "$ODBC_TYPE"; then
PHP_ARG_WITH([empress],,
  [AS_HELP_STRING([[--with-empress[=DIR]]],
    [Include Empress support $EMPRESSPATH (Empress Version >= 8.60
    required)])])

  AC_MSG_CHECKING(for Empress support)
  if test "$PHP_EMPRESS" != "no"; then
    if test "$PHP_EMPRESS" = "yes"; then
      ODBC_INCDIR=$EMPRESSPATH/include/odbc
      ODBC_LIBDIR=$EMPRESSPATH/shlib
    else
      ODBC_INCDIR=$PHP_EMPRESS/include/odbc
      ODBC_LIBDIR=$PHP_EMPRESS/shlib
    fi
    ODBC_CFLAGS=-I$ODBC_INCDIR
    ODBC_LFLAGS=-L$ODBC_LIBDIR
    ODBC_TYPE=empress
    AC_DEFINE(HAVE_EMPRESS,1,[ ])
    AC_MSG_RESULT([$ext_output])
    PHP_ODBC_FIND_EMPRESS_LIBS($ODBC_LIBDIR)
  else
    AC_MSG_RESULT(no)
  fi
fi

if test -z "$ODBC_TYPE"; then
PHP_ARG_WITH([empress-bcs],,
  [AS_HELP_STRING([[--with-empress-bcs[=DIR]]],
    [Include Empress Local Access support $EMPRESSPATH (Empress Version >=
    8.60 required)])])

  AC_MSG_CHECKING(for Empress local access support)
  if test "$PHP_EMPRESS_BCS" != "no"; then
    if test "$PHP_EMPRESS_BCS" = "yes"; then
      ODBC_INCDIR=$EMPRESSPATH/include/odbc
      ODBC_LIBDIR=$EMPRESSPATH/shlib
    else
      ODBC_INCDIR=$PHP_EMPRESS_BCS/include/odbc
      ODBC_LIBDIR=$PHP_EMPRESS_BCS/shlib
    fi
    CC="empocc -bcs";export CC;
    LD="empocc -bcs";export LD;
    ODBC_CFLAGS=-I$ODBC_INCDIR
    ODBC_LFLAGS=-L$ODBC_LIBDIR
    LIST=`empocc -listlines -bcs -o a a.c`

    NEWLIST=
    for I in $LIST
    do
      case $I in
        $EMPRESSPATH/odbccl/lib/* | \
        $EMPRESSPATH/rdbms/lib/* | \
        $EMPRESSPATH/common/lib/*)
              NEWLIST="$NEWLIST $I"
              ;;
      esac
    done
    ODBC_LIBS="-lempphpbcs -lms -lmscfg -lbasic -lbasic_os -lnlscstab -lnlsmsgtab -lm -ldl -lcrypt"
    ODBC_TYPE=empress-bcs
    AC_DEFINE(HAVE_EMPRESS,1,[ ])
    AC_MSG_RESULT([$ext_output])
    PHP_ODBC_FIND_EMPRESS_BCS_LIBS($ODBC_LIBDIR)
  else
    AC_MSG_RESULT(no)
  fi
fi

if test -z "$ODBC_TYPE"; then
PHP_ARG_WITH([custom-odbc],,
  [AS_HELP_STRING([[--with-custom-odbc[=DIR]]],
    [Include user defined ODBC support. DIR is ODBC install base directory
    [/usr/local]. Make sure to define CUSTOM_ODBC_LIBS and have some odbc.h in
    your include dirs. For example, you should define following for Sybase SQL
    Anywhere 5.5.00 on QNX, prior to running this configure script:
    CPPFLAGS="-DODBC_QNX -DSQLANY_BUG" LDFLAGS=-lunix
    CUSTOM_ODBC_LIBS="-ldblib -lodbc"])])

  AC_MSG_CHECKING(for a custom ODBC support)
  if test "$PHP_CUSTOM_ODBC" != "no"; then
    if test "$PHP_CUSTOM_ODBC" = "yes"; then
      PHP_CUSTOM_ODBC=/usr/local
    fi
    ODBC_INCDIR=$PHP_CUSTOM_ODBC/include
    ODBC_LIBDIR=$PHP_CUSTOM_ODBC/$PHP_LIBDIR
    ODBC_LFLAGS=-L$ODBC_LIBDIR
    ODBC_CFLAGS=-I$ODBC_INCDIR
    ODBC_LIBS=$CUSTOM_ODBC_LIBS
    ODBC_TYPE=custom-odbc
    AC_DEFINE(HAVE_CODBC,1,[ ])
    AC_MSG_RESULT([$ext_output])
  else
    AC_MSG_RESULT(no)
  fi
fi

if test -z "$ODBC_TYPE"; then
PHP_ARG_WITH([iodbc],,
  [AS_HELP_STRING([--with-iodbc],
    [Include iODBC support])])

  AC_MSG_CHECKING(whether to build with iODBC support)
  if test "$PHP_IODBC" != "no"; then
    AC_MSG_RESULT(yes)
    PKG_CHECK_MODULES([ODBC], [libiodbc])
    PHP_EVAL_INCLINE($ODBC_CFLAGS)
    ODBC_TYPE=iodbc
    AC_DEFINE(HAVE_IODBC,1,[ ])
    AC_DEFINE(HAVE_ODBC2,1,[ ])
  else
    AC_MSG_RESULT(no)
  fi
fi

if test -z "$ODBC_TYPE"; then
PHP_ARG_WITH([esoob],,
  [AS_HELP_STRING([[--with-esoob[=DIR]]],
    [Include Easysoft OOB support [/usr/local/easysoft/oob/client]])])

  AC_MSG_CHECKING(for Easysoft ODBC-ODBC Bridge support)
  if test "$PHP_ESOOB" != "no"; then
    if test "$PHP_ESOOB" = "yes"; then
      PHP_ESOOB=/usr/local/easysoft/oob/client
    fi
    ODBC_INCDIR=$PHP_ESOOB/include
    ODBC_LIBDIR=$PHP_ESOOB/$PHP_LIBDIR
    ODBC_LFLAGS=-L$ODBC_LIBDIR
    ODBC_CFLAGS=-I$ODBC_INCDIR
    ODBC_LIBS=-lesoobclient
    ODBC_TYPE=esoob
    AC_DEFINE(HAVE_ESOOB,1,[ ])
    AC_MSG_RESULT([$ext_output])
  else
    AC_MSG_RESULT(no)
  fi
fi

if test -z "$ODBC_TYPE"; then
PHP_ARG_WITH([unixODBC],,
  [AS_HELP_STRING([--with-unixODBC],
    [Include unixODBC support])])

  AC_MSG_CHECKING(whether to build with unixODBC support)
  if test "$PHP_UNIXODBC" != "no"; then
    if test "$PHP_UNIXODBC" = "yes"; then
      AC_MSG_RESULT(yes from pkgconfig)
      PKG_CHECK_MODULES([ODBC], [odbc])
      PHP_EVAL_INCLINE($ODBC_CFLAGS)
    else
      dnl keep old DIR way for old version without libodbc.pc
      ODBC_INCDIR=$PHP_UNIXODBC/include
      ODBC_LIBDIR=$PHP_UNIXODBC/$PHP_LIBDIR
      ODBC_LFLAGS=-L$ODBC_LIBDIR
      ODBC_CFLAGS=-I$ODBC_INCDIR
      ODBC_LIBS=-lodbc
      PHP_ODBC_CHECK_HEADER(sqlext.h)
      AC_MSG_RESULT(yes in $PHP_UNIXODBC)
    fi
    ODBC_TYPE=unixODBC
    AC_DEFINE(HAVE_UNIXODBC,1,[ ])
  else
    AC_MSG_RESULT(no)
  fi
fi

if test -z "$ODBC_TYPE"; then
PHP_ARG_WITH([dbmaker],,
  [AS_HELP_STRING([[--with-dbmaker[=DIR]]],
    [Include DBMaker support])])

  AC_MSG_CHECKING(for DBMaker support)
  if test "$PHP_DBMAKER" != "no"; then
    if test "$PHP_DBMAKER" = "yes"; then
      dnl Find dbmaker's home directory
      DBMAKER_HOME=`grep "^dbmaker:" /etc/passwd | $AWK -F: '{print $6}'`

      dnl check DBMaker version (from 5.0 to 2.0)
      DBMAKER_VERSION=5.0

      while test ! -d $DBMAKER_HOME/$DBMAKER_VERSION -a "$DBMAKER_VERSION" != "2.9"; do
        DM_VER=`echo $DBMAKER_VERSION | sed -e 's/\.//' | $AWK '{ print $1-1;}'`
        MAJOR_V=`echo $DM_VER | $AWK '{ print $1/10; }'  | $AWK -F. '{ print $1; }'`
        MINOR_V=`echo $DM_VER | $AWK '{ print $1%10; }'`
        DBMAKER_VERSION=$MAJOR_V.$MINOR_V
      done

      if test "$DBMAKER_VERSION" = "2.9"; then
        PHP_DBMAKER=$DBMAKER_HOME
      else
        PHP_DBMAKER=$DBMAKER_HOME/$DBMAKER_VERSION
      fi
    fi

    ODBC_INCDIR=$PHP_DBMAKER/include
    ODBC_LIBDIR=$PHP_DBMAKER/$PHP_LIBDIR
    ODBC_CFLAGS=-I$ODBC_INCDIR
    ODBC_LFLAGS=-L$ODBC_LIBDIR
    ODBC_LIBS="-ldmapic -lc"
    ODBC_TYPE=dbmaker

    AC_DEFINE(HAVE_DBMAKER,1,[Whether you want DBMaker])

    if test "$ext_shared" = "yes"; then
      AC_MSG_RESULT([yes (shared)])
      ODBC_LIBS="-ldmapic -lc -lm"
      ODBC_SHARED="odbc.la"
    else
      AC_MSG_RESULT([yes (static)])
      PHP_ADD_LIBRARY_WITH_PATH(dmapic, $ODBC_LIBDIR)
      PHP_ADD_INCLUDE($ODBC_INCDIR)
      ODBC_STATIC="libphpext_odbc.la"
    fi
  else
    AC_MSG_RESULT(no)
  fi
fi

if test "no" != "$PHP_ODBCVER"; then
  if test "$PHP_ODBCVER" != "0"; then
    AC_DEFINE_UNQUOTED(ODBCVER, $PHP_ODBCVER, [ The highest supported ODBC version ])
  fi
else
  AC_DEFINE(ODBCVER, 0x0300, [ The highest supported ODBC version ])
fi

dnl Extension setup
if test -n "$ODBC_TYPE"; then
  if test "$ODBC_TYPE" != "dbmaker"; then
    PHP_EVAL_LIBLINE([$ODBC_LFLAGS $ODBC_LIBS], ODBC_SHARED_LIBADD)
    if test "$ODBC_TYPE" != "solid"; then
      AC_DEFINE(HAVE_SQLDATASOURCES,1,[ ])
    fi
  fi

  AC_DEFINE(HAVE_UODBC,1,[ ])
  PHP_SUBST(ODBC_SHARED_LIBADD)
  PHP_SUBST(ODBC_INCDIR)
  PHP_SUBST(ODBC_LIBDIR)
  PHP_SUBST_OLD(ODBC_CFLAGS)
  PHP_SUBST_OLD(ODBC_LIBS)
  PHP_SUBST_OLD(ODBC_LFLAGS)
  PHP_SUBST_OLD(ODBC_TYPE)

  PHP_NEW_EXTENSION(odbc, php_odbc.c odbc_utils.c, $ext_shared,, [$ODBC_CFLAGS -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
else
  AC_MSG_CHECKING([for any ODBC driver support])
  AC_MSG_RESULT(no)
fi
