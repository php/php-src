dnl
dnl $Id$
dnl

dnl
dnl Figure out which library file to link with for the Solid support.
dnl
AC_DEFUN(AC_FIND_SOLID_LIBS,[
  AC_MSG_CHECKING([Solid library file])  
  ac_solid_uname_r=`uname -r 2>/dev/null`
  ac_solid_uname_s=`uname -s 2>/dev/null`
  case $ac_solid_uname_s in
    AIX) ac_solid_os=a3x;;   # a4x for AIX4/ Solid 2.3/3.0 only
    HP-UX) ac_solid_os=h9x;; # h1x for hpux11, h0x for hpux10
    IRIX) ac_solid_os=irx;;	 # Solid 2.3(?)/ 3.0 only
    Linux) if ldd -v /bin/sh | grep GLIBC > /dev/null; then
		AC_DEFINE(SS_LINUX,1,[Needed in sqlunix.h ])
		ac_solid_os=l2x
	else
		AC_DEFINE(SS_LINUX,1,[Needed in sqlunix.h ])
		ac_solid_os=lux
	fi;; 
    SunOS) ac_solid_os=ssx;; # should we deal with SunOS 4?
    FreeBSD) if test `expr $ac_solid_uname_r : '\(.\)'` -gt "2"; then
		AC_DEFINE(SS_FBX,1,[Needed in sqlunix.h for wchar defs ])
		ac_solid_os=fex
   else 
		AC_DEFINE(SS_FBX,1,[Needed in sqlunix.h for wchar defs ])
		ac_solid_os=fbx
   fi	
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

#
# Check for the library files, and setup the ODBC_LIBS path...
#
if test ! -f $1/lib${ac_solid_prefix}${ac_solid_os}${ac_solid_version}.so -a \
	! -f $1/lib${ac_solid_prefix}${ac_solid_os}${ac_solid_version}.a; then
	#
	# we have an error and should bail out, as we can't find the libs!
	#
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
AC_DEFUN(AC_FIND_EMPRESS_LIBS,[
  AC_MSG_CHECKING([Empress library file])
  ODBC_LIBS=`echo $1/libempodbccl.so | cut -d' ' -f1`
  if test ! -f $ODBC_LIBS; then
    ODBC_LIBS=`echo $1/libempodbccl.so | cut -d' ' -f1`
  fi
  AC_MSG_RESULT(`echo $ODBC_LIBS | sed -e 's!.*/!!'`)
])

AC_DEFUN(AC_FIND_EMPRESS_BCS_LIBS,[
  AC_MSG_CHECKING([Empress local access library file])
  ODBCBCS_LIBS=`echo $1/libempodbcbcs.a | cut -d' ' -f1`
  if test ! -f $ODBCBCS_LIBS; then
    ODBCBCS_LIBS=`echo $1/libempodbcbcs.a | cut -d' ' -f1`
  fi
  AC_MSG_RESULT(`echo $ODBCBCS_LIBS | sed -e 's!.*/!!'`)
])

if test -z "$ODBC_TYPE"; then
AC_MSG_CHECKING(for Adabas support)
AC_ARG_WITH(adabas,
[  --with-adabas[=DIR]     Include Adabas D support.  DIR is the Adabas base
                          install directory, defaults to /usr/local.],
[
  PHP_WITH_SHARED
  if test "$withval" = "yes"; then
    withval=/usr/local
  fi
  if test "$withval" != "no"; then
    PHP_ADD_INCLUDE($withval/incl)
    PHP_ADD_LIBPATH($withval/lib)
    ODBC_OBJS="$withval/lib/odbclib.a"
    ODBC_LIB="$abs_builddir/ext/odbc/libodbc_adabas.a"
    $srcdir/build/shtool mkdir -f -p ext/odbc
    rm -f "$ODBC_LIB"
    cp "$ODBC_OBJS" "$ODBC_LIB"
	PHP_ADD_LIBRARY(sqlptc)
	PHP_ADD_LIBRARY(sqlrte)
    PHP_ADD_LIBRARY_WITH_PATH(odbc_adabas, $abs_builddir/ext/odbc)
    ODBC_TYPE=adabas
    AC_DEFINE(HAVE_ADABAS,1,[ ])
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
fi

if test -z "$ODBC_TYPE"; then
AC_MSG_CHECKING(for SAP DB support)
AC_ARG_WITH(sapdb,
[  --with-sapdb[=DIR]      Include SAP DB support.  DIR is SAP DB base
                          install directory, defaults to /usr/local.],
[
  PHP_WITH_SHARED
  if test "$withval" = "yes"; then
    withval=/usr/local
  fi
  if test "$withval" != "no"; then
    PHP_ADD_INCLUDE($withval/incl)
    PHP_ADD_LIBPATH($withval/lib)
    PHP_ADD_LIBRARY(sqlod)
    ODBC_TYPE=sapdb
    AC_DEFINE(HAVE_SAPDB,1,[ ])
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
  PHP_WITH_SHARED
  if test "$withval" = "yes"; then
    withval=/usr/local/solid
  fi
  if test "$withval" != "no"; then
    ODBC_INCDIR=$withval/include
    ODBC_LIBDIR=$withval/lib
    ODBC_INCLUDE=-I$ODBC_INCDIR
    ODBC_TYPE=solid
    if test -f $ODBC_LIBDIR/soc*35.a; then
      AC_DEFINE(HAVE_SOLID_35,1,[ ])
	elif test -f $ODBC_LIBDIR/scl*30.a; then
	  AC_DEFINE(HAVE_SOLID_30,1,[ ])
	elif test -f $ODBC_LIBDIR/scl*23.a; then
      AC_DEFINE(HAVE_SOLID,1,[ ])
    fi
    AC_MSG_RESULT(yes)
    AC_FIND_SOLID_LIBS($ODBC_LIBDIR)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
fi

if test -z "$ODBC_TYPE"; then
AC_MSG_CHECKING(for IBM DB2 support)
AC_ARG_WITH(ibm-db2,
[  --with-ibm-db2[=DIR]    Include IBM DB2 support.  DIR is the DB2 base
                          install directory, defaults to /home/db2inst1/sqllib],
[
  PHP_WITH_SHARED
  if test "$withval" != "no"; then
    if test "$withval" = "yes"; then
        ODBC_INCDIR=/home/db2inst1/sqllib/include
        ODBC_LIBDIR=/home/db2inst1/sqllib/lib
    else
        ODBC_INCDIR=$withval/include
        ODBC_LIBDIR=$withval/lib
    fi
    ODBC_INCLUDE=-I$ODBC_INCDIR
    ODBC_LFLAGS=-L$ODBC_LIBDIR
    ODBC_TYPE=db2
    ODBC_LIBS="-ldb2"
    AC_DEFINE(HAVE_IBMDB2,1,[ ])

    AC_MSG_RESULT(yes)
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
                          install directory, defaults to \$EMPRESSPATH.
                          From PHP4, this option only supports Empress Version
                          8.60 and above],
[
  PHP_WITH_SHARED
  if test "$withval" != "no"; then
    if test "$withval" = "yes"; then
      ODBC_INCDIR=$EMPRESSPATH/include/odbc
      ODBC_LIBDIR=$EMPRESSPATH/shlib
    else
      ODBC_INCDIR=$withval/include/odbc
      ODBC_LIBDIR=$withval/shlib
    fi
    ODBC_INCLUDE=-I$ODBC_INCDIR
    ODBC_LFLAGS=-L$ODBC_LIBDIR
    ODBC_TYPE=empress
    AC_DEFINE(HAVE_EMPRESS,1,[ ])
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
AC_MSG_CHECKING(for Empress local access support)
AC_ARG_WITH(empress-bcs,
[  --with-empress-bcs[=DIR]
                          Include Empress Local Access support.  DIR is the 
                          Empress base install directory, defaults to 
                          \$EMPRESSPATH.  From PHP4, this option only supports
                          Empress Version 8.60 and above.],
[
  PHP_WITH_SHARED
  if test "$withval" != "no"; then
    if test "$withval" = "yes"; then
      ODBC_INCDIR=$EMPRESSPATH/include/odbc
      ODBC_LIBDIR=$EMPRESSPATH/shlib
    else
      ODBC_INCDIR=$withval/include/odbc
      ODBC_LIBDIR=$withval/shlib
    fi
    CC="empocc -bcs";export CC;
    LD="empocc -bcs";export LD;
    ODBC_INCLUDE=-I$ODBC_INCDIR
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
    ODBC_TYPE=empress
    AC_DEFINE(HAVE_EMPRESS,1,[ ])
    AC_MSG_RESULT(yes)
    AC_FIND_EMPRESS_BCS_LIBS($ODBC_LIBDIR)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
fi

if test -z "$ODBC_TYPE"; then
AC_MSG_CHECKING(for Birdstep support)
AC_ARG_WITH(birdstep,
[  --with-birdstep[=DIR]   Include Birdstep support.  DIR is the Birdstep base
                          install directory, defaults to /usr/local/birdstep.],
[
  PHP_WITH_SHARED

  if test "$withval" != "no"; then
    if test "$withval" = "yes"; then
        ODBC_INCDIR=/usr/local/birdstep/include
        ODBC_LIBDIR=/usr/local/birdstep/lib
    else
        ODBC_INCDIR=$withval/include
        ODBC_LIBDIR=$withval/lib
    fi
    ODBC_INCLUDE=-I$ODBC_INCDIR
    ODBC_TYPE=birdstep
    ODBC_LFLAGS=-L$ODBC_LIBDIR
    ODBC_LIBS="-lCadm -lCdict -lCenc -lCrdm -lCrpc -lCrdbc -lCrm -lCuapi -lutil"
    AC_DEFINE(HAVE_BIRDSTEP,1,[ ])

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
[  --with-custom-odbc[=DIR]      
                          Include a user defined ODBC support.
                          The DIR is ODBC install base directory, 
                          which defaults to /usr/local.
                          Make sure to define CUSTOM_ODBC_LIBS and
                          have some odbc.h in your include dirs.
                          E.g., you should define following for 
                          Sybase SQL Anywhere 5.5.00 on QNX, prior to
                          run configure script:
                              CPPFLAGS=\"-DODBC_QNX -DSQLANY_BUG\"
                              LDFLAGS=-lunix
                              CUSTOM_ODBC_LIBS=\"-ldblib -lodbc\".],
[
  if test "$withval" = "yes"; then
    withval=/usr/local
  fi
  if test "$withval" != "no"; then
    ODBC_INCDIR=$withval/include
    ODBC_LIBDIR=$withval/lib
    ODBC_LFLAGS=-L$ODBC_LIBDIR
    ODBC_INCLUDE=-I$ODBC_INCDIR
    ODBC_LIBS=$CUSTOM_ODBC_LIBS
    ODBC_TYPE=custom
    AC_DEFINE(HAVE_CODBC,1,[ ])
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
  PHP_WITH_SHARED
  if test "$withval" = "yes"; then
    withval=/usr/local
  fi
  if test "$withval" != "no"; then
    PHP_ADD_LIBRARY_WITH_PATH(iodbc, $withval/lib)
    PHP_ADD_INCLUDE($withval/include, 1)
    ODBC_TYPE=iodbc
    ODBC_INCLUDE=-I$withval/include
    ODBC_LFLAGS=-L$withval/lib
    ODBC_LIBS=-liodbc
    AC_DEFINE(HAVE_IODBC,1,[ ])
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
fi

if test -z "$ODBC_TYPE"; then
AC_MSG_CHECKING(for Easysoft ODBC-ODBC Bridge support)
AC_ARG_WITH(esoob,
[  --with-esoob[=DIR]      Include Easysoft OOB support. DIR is the OOB base
                          install directory,
                          defaults to /usr/local/easysoft/oob/client.],
[
  PHP_WITH_SHARED
  if test "$withval" = "yes"; then
    withval=/usr/local/easysoft/oob/client
  fi
  if test "$withval" != "no"; then
    ODBC_INCDIR=$withval/include
    ODBC_LIBDIR=$withval/lib
    ODBC_LFLAGS=-L$ODBC_LIBDIR
    ODBC_INCLUDE=-I$ODBC_INCDIR
    ODBC_LIBS=-lesoobclient
    ODBC_TYPE=ESOOB
    AC_DEFINE(HAVE_ESOOB,1,[ ])
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
fi

if test -z "$ODBC_TYPE"; then
AC_MSG_CHECKING(for unixODBC support)
AC_ARG_WITH(unixODBC,
[  --with-unixODBC[=DIR]   Include unixODBC support.  DIR is the unixODBC base
                          install directory, defaults to /usr/local.],
[
  PHP_WITH_SHARED
  if test "$withval" = "yes"; then
    withval=/usr/local
  fi
  if test "$withval" != "no"; then
    ODBC_INCDIR=$withval/include
    ODBC_LIBDIR=$withval/lib
    ODBC_LFLAGS=-L$ODBC_LIBDIR
    ODBC_INCLUDE=-I$ODBC_INCDIR
    ODBC_LIBS=-lodbc
    ODBC_TYPE=unixODBC
    AC_DEFINE(HAVE_UNIXODBC,1,[ ])
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
                          /usr/local.  This is the same as iODBC.],
[
  PHP_WITH_SHARED
  if test "$withval" = "yes"; then
    withval=/usr/local
  fi
  if test "$withval" != "no"; then
	PHP_ADD_LIBRARY_WITH_PATH(iodbc, $withval/lib)
	PHP_ADD_INCLUDE($withval/include, 1)
    ODBC_TYPE=iodbc
    ODBC_INCLUDE=-I$withval/include
    ODBC_LFLAGS=-L$withval/lib
    ODBC_LIBS=-liodbc
    AC_DEFINE(HAVE_IODBC,1,[ ])
    AC_MSG_RESULT(yes)
	echo "****************************************************************"
	echo "  --with-openlink will not be valid in future releases.  Please "
	echo " update your configure script to use --with-iodbc instead.      "
	echo "****************************************************************"
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
fi

if test -z "$ODBC_TYPE"; then
AC_MSG_CHECKING(for DBMaker support)
AC_ARG_WITH(dbmaker,
[  --with-dbmaker[=DIR]    Include DBMaker support.  DIR is the DBMaker base
                          install directory, defaults to where the latest 
                          version of DBMaker is installed (such as
                          /home/dbmaker/3.6).],
[
  PHP_WITH_SHARED
  if test "$withval" = "yes"; then
    # find dbmaker's home directory
    DBMAKER_HOME=`grep "^dbmaker:" /etc/passwd | awk -F: '{print $6}'`

    # check DBMaker version (from 5.0 to 2.0)
    DBMAKER_VERSION=5.0

    while test test ! -d $DBMAKER_HOME/$DBMAKER_VERSION -a \
                 "$DBMAKER_VERSION" != "2.9"; do
        DM_VER=`echo $DBMAKER_VERSION | sed -e 's/\.//' | awk '{ print $1-1;}'`
        MAJOR_V=`echo $DM_VER | awk '{ print $1/10; }' \
                 | awk  -F. '{ print $1; }'`
        MINOR_V=`echo $DM_VER | awk '{ print $1%10; }'`
        DBMAKER_VERSION=$MAJOR_V.$MINOR_V
    done

    if test "$DBMAKER_VERSION" = "2.9"; then
        withval=$DBMAKER_HOME
    else
        DBMAKER_PATH=$DBMAKER_HOME/$DBMAKER_VERSION
    fi

    withval=$DBMAKER_PATH
  fi

  if test "$withval" != "no"; then
    ODBC_INCDIR=$withval/include
    ODBC_LIBDIR=$withval/lib
    ODBC_INCLUDE=-I$ODBC_INCDIR
    ODBC_LFLAGS=-L$ODBC_LIBDIR
    ODBC_INCLUDE=-I$ODBC_INCDIR
    ODBC_LIBS="-ldmapic -lc"
    ODBC_TYPE=dbmaker

    AC_DEFINE(HAVE_DBMAKER,1,[Whether you want DBMaker])

    if test "$shared" = "yes"; then
        AC_MSG_RESULT(yes (shared))
        ODBC_LIBS="-ldmapic -lc -lm"
        ODBC_SHARED="odbc.la"
    else
        AC_MSG_RESULT(yes (static))
        PHP_ADD_LIBRARY_WITH_PATH(dmapic, $ODBC_LIBDIR)
        PHP_ADD_INCLUDE($ODBC_INCDIR)
        ODBC_STATIC="libphpext_odbc.la"
    fi

    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
fi

if test -n "$ODBC_TYPE"; then
  if test "$ODBC_TYPE" != "dbmaker"; then
    if test "$shared" != "yes"; then
      EXTRA_LDFLAGS="$EXTRA_LDFLAGS $ODBC_LFLAGS"
      EXTRA_LIBS="$EXTRA_LIBS $ODBC_LIBS"
    fi
  fi
  AC_DEFINE(HAVE_UODBC,1,[ ])
  PHP_SUBST(ODBC_INCDIR)
  PHP_SUBST(ODBC_LIBDIR)
  PHP_SUBST_OLD(ODBC_INCLUDE)
  PHP_SUBST_OLD(ODBC_LIBS)
  PHP_SUBST_OLD(ODBC_LFLAGS)
  PHP_SUBST_OLD(ODBC_TYPE)

  PHP_NEW_EXTENSION(odbc, php_odbc.c, $shared,, $ODBC_INCLUDE)
fi
