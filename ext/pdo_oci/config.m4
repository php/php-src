dnl $Id$
dnl config.m4 for extension pdo_oci
dnl vim:et:sw=2:ts=2:

if test "$PHP_PDO" != "no"; then
SUPPORTED_VERS="1 8 9 10 11"
AC_DEFUN([AC_PDO_OCI_VERSION],[
  AC_MSG_CHECKING([Oracle version])
  if test -s "$PDO_OCI_DIR/orainst/unix.rgs"; then
    PDO_OCI_VERSION=`grep '"ocommon"' $PDO_OCI_DIR/orainst/unix.rgs | sed 's/  */:/g' | cut -d: -f 6 | cut -c 2-4`
    test -z "$PDO_OCI_VERSION" && PDO_OCI_VERSION=7.3
  elif test -f $PDO_OCI_DIR/lib/libclntsh.a; then
    if test -f $PDO_OCI_DIR/lib/libcore4.a; then
      PDO_OCI_VERSION=8.x
    else
      PDO_OCI_VERSION=8.x
    fi
  else
          for OCI_VER in $SUPPORTED_VERS; do
                  if test -f $PDO_OCI_DIR/lib/libclntsh.$SHLIB_SUFFIX_NAME.$OCI_VER.*; then
                    PDO_OCI_VERSION="$OCI_VER.x"
                    echo $PDO_OCI_VERSION
                  fi
          done
  fi
  if [ -z "$PDO_OCI_VERSION" ] ;then
    { { echo "$as_me:$LINENO: error: Oracle-OCI needed libraries not found under $PDO_OCI_DIR" >&5
        echo "$as_me: error: Oracle-OCI needed libraries not found under $PDO_OCI_DIR" >&2;}
   { (exit 1); exit 1; }; }
  fi

PHP_ARG_WITH(pdo-oci, Oracle OCI support for PDO,
[  --with-pdo-oci[=DIR]      PDO: Oracle-OCI support. Default DIR is ORACLE_HOME.
                            You may also use --with-pdo-oci=instantclient,prefix,version to use
                            the InstantClient SDK. For Linux with 10.1.0.3 rpms (for example) use:
                            --with-pdo-oci=instantclient,/usr,10.1.0.3])

if test "$PHP_PDO_OCI" != "no"; then
  AC_MSG_CHECKING([Oracle Install-Dir])
  if test "$PHP_PDO_OCI" = "yes" -o -z "$PHP_PDO_OCI"; then
    PDO_OCI_DIR=$ORACLE_HOME
  else
    PDO_OCI_DIR=$PHP_PDO_OCI
  fi
  AC_MSG_RESULT($PDO_OCI_DIR :$PHP_PDO_OCI:)

  AC_MSG_CHECKING([if that is sane])
  if test -z "$PDO_OCI_DIR"; then
    AC_MSG_ERROR([
You need to tell me where to find your oracle SDK, or set ORACLE_HOME.
])
  else
    AC_MSG_RESULT([yes])
  fi

  if test "instantclient" = "`echo $PDO_OCI_DIR | cut -d, -f1`" ; then
    PDO_OCI_IC_PREFIX="`echo $PDO_OCI_DIR | cut -d, -f2`"
    PDO_OCI_IC_VERS="`echo $PDO_OCI_DIR | cut -d, -f3`"
    AC_MSG_CHECKING([for oci.h])
    if test -f $PDO_OCI_IC_PREFIX/include/oracle/$PDO_OCI_IC_VERS/client/oci.h ; then
      PHP_ADD_INCLUDE($PDO_OCI_IC_PREFIX/include/oracle/$PDO_OCI_IC_VERS/client)
      AC_MSG_RESULT($PDO_OCI_IC_PREFIX/include/oracle/$PDO_OCI_IC_VERS/client)
    elif test -f $PDO_OCI_IC_PREFIX/lib/oracle/$PDO_OCI_IC_VERS/client/include/oci.h ; then
      PHP_ADD_INCLUDE($PDO_OCI_IC_PREFIX/lib/oracle/$PDO_OCI_IC_VERS/client/include)
      AC_MSG_RESULT($PDO_OCI_IC_PREFIX/lib/oracle/$PDO_OCI_IC_VERS/client/include)
    else
      AC_MSG_ERROR([I'm too dumb to figure out where the include dir is in your instant client install])
    fi
    PDO_OCI_LIB_DIR="$PDO_OCI_IC_PREFIX/lib/oracle/$PDO_OCI_IC_VERS/client/lib"
    PDO_OCI_VERSION="`echo $PDO_OCI_IC_VERS | cut -d. -f1-2`"
  else
    if test -d "$PDO_OCI_DIR/rdbms/public"; then
      PHP_ADD_INCLUDE($PDO_OCI_DIR/rdbms/public)
      PDO_OCI_INCLUDES="$PDO_OCI_INCLUDES -I$PDO_OCI_DIR/rdbms/public"
    fi
    if test -d "$PDO_OCI_DIR/rdbms/demo"; then
      PHP_ADD_INCLUDE($PDO_OCI_DIR/rdbms/demo)
      PDO_OCI_INCLUDES="$PDO_OCI_INCLUDES -I$PDO_OCI_DIR/rdbms/demo"
    fi
    if test -d "$PDO_OCI_DIR/network/public"; then
      PHP_ADD_INCLUDE($PDO_OCI_DIR/network/public)
      PDO_OCI_INCLUDES="$PDO_OCI_INCLUDES -I$PDO_OCI_DIR/network/public"
    fi
    if test -d "$PDO_OCI_DIR/plsql/public"; then
      PHP_ADD_INCLUDE($PDO_OCI_DIR/plsql/public)
      PDO_OCI_INCLUDES="$PDO_OCI_INCLUDES -I$PDO_OCI_DIR/plsql/public"
    fi
    if test -d "$PDO_OCI_DIR/include"; then
      PHP_ADD_INCLUDE($PDO_OCI_DIR/include)
      PDO_OCI_INCLUDES="$PDO_OCI_INCLUDES -I$PDO_OCI_DIR/include"
    fi

    if test -f "$PDO_OCI_DIR/lib/sysliblist"; then
      PHP_EVAL_LIBLINE(`cat $PDO_OCI_DIR/lib/sysliblist`, PDO_OCI_SYSLIB)
    elif test -f "$PDO_OCI_DIR/rdbms/lib/sysliblist"; then
      PHP_EVAL_LIBLINE(`cat $PDO_OCI_DIR/rdbms/lib/sysliblist`, PDO_OCI_SYSLIB)
    fi
    PDO_OCI_LIB_DIR="$PDO_OCI_DIR/lib"
    AC_PDO_OCI_VERSION($PDO_OCI_DIR)
  fi
  case $PDO_OCI_VERSION in
    8.0)
      PHP_ADD_LIBRARY_WITH_PATH(nlsrtl3, "", PDO_OCI_SHARED_LIBADD)
      PHP_ADD_LIBRARY_WITH_PATH(core4, "", PDO_OCI_SHARED_LIBADD)
      PHP_ADD_LIBRARY_WITH_PATH(psa, "", PDO_OCI_SHARED_LIBADD)
      PHP_ADD_LIBRARY_WITH_PATH(clntsh, $PDO_OCI_LIB_DIR, PDO_OCI_SHARED_LIBADD)
      ;;

    9.x|10.x|11.x)
      PHP_ADD_LIBRARY(clntsh, 1, PDO_OCI_SHARED_LIBADD)
      ;;
      
    #11.1)
    #  PHP_ADD_LIBRARY(clntsh, 1, PDO_OCI_SHARED_LIBADD)
    #  ;;
    #10.2)
    #  PHP_ADD_LIBRARY(clntsh, 1, PDO_OCI_SHARED_LIBADD)
    #  ;;
    *)
      AC_MSG_ERROR(Unsupported Oracle version! $PDO_OCI_VERSION)
      ;;
  esac

  PHP_ADD_LIBPATH($PDO_OCI_LIB_DIR, PDO_OCI_SHARED_LIBADD)

  PHP_CHECK_LIBRARY(clntsh, OCIEnvCreate,
  [
    AC_DEFINE(HAVE_OCIENVCREATE,1,[ ])
  ], [], [
    -L$PDO_OCI_LIB_DIR $PDO_OCI_SHARED_LIBADD
  ])

  PHP_CHECK_LIBRARY(clntsh, OCIEnvNlsCreate,
  [
    AC_DEFINE(HAVE_OCIENVNLSCREATE,1,[ ])
  ], [], [
    -L$PDO_OCI_LIB_DIR $PDO_OCI_SHARED_LIBADD
  ])

  dnl
  dnl Check if we need to add -locijdbc8 
  dnl
  PHP_CHECK_LIBRARY(clntsh, OCILobIsTemporary,
  [
    AC_DEFINE(HAVE_OCILOBISTEMPORARY,1,[ ])
  ], [
    PHP_CHECK_LIBRARY(ocijdbc8, OCILobIsTemporary,
    [
      PHP_ADD_LIBRARY(ocijdbc8, 1, PDO_OCI_SHARED_LIBADD)
      AC_DEFINE(HAVE_OCILOBISTEMPORARY,1,[ ])
    ], [], [
      -L$PDO_OCI_LIB_DIR $PDO_OCI_SHARED_LIBADD
    ])
  ], [
    -L$PDO_OCI_LIB_DIR $PDO_OCI_SHARED_LIBADD
  ])

  dnl
  dnl Check if we have collections
  dnl
  PHP_CHECK_LIBRARY(clntsh, OCICollAssign,
  [
    AC_DEFINE(HAVE_OCICOLLASSIGN,1,[ ])
  ], [], [
    -L$PDO_OCI_LIB_DIR $PDO_OCI_SHARED_LIBADD
  ])

  dnl Scrollable cursors?
  PHP_CHECK_LIBRARY(clntsh, OCIStmtFetch2,
  [
     AC_DEFINE(HAVE_OCISTMTFETCH2,1,[ ])
  ], [], [
    -L$PDO_OCI_LIB_DIR $PDO_OCI_SHARED_LIBADD
  ])

  ifdef([PHP_CHECK_PDO_INCLUDES],
  [
    PHP_CHECK_PDO_INCLUDES
  ],[
    AC_MSG_CHECKING([for PDO includes])
    if test -f $abs_srcdir/include/php/ext/pdo/php_pdo_driver.h; then
      pdo_inc_path=$abs_srcdir/ext
    elif test -f $abs_srcdir/ext/pdo/php_pdo_driver.h; then
      pdo_inc_path=$abs_srcdir/ext
    elif test -f $prefix/include/php/ext/pdo/php_pdo_driver.h; then
      pdo_inc_path=$prefix/include/php/ext
    else
      AC_MSG_ERROR([Cannot find php_pdo_driver.h.])
    fi
    AC_MSG_RESULT($pdo_inc_path)
  ])

  PHP_NEW_EXTENSION(pdo_oci, pdo_oci.c oci_driver.c oci_statement.c, $ext_shared,,-I$pdo_inc_path)

  PHP_SUBST_OLD(PDO_OCI_SHARED_LIBADD)
  PHP_SUBST_OLD(PDO_OCI_DIR)
  PHP_SUBST_OLD(PDO_OCI_VERSION)

  ifdef([PHP_ADD_EXTENSION_DEP],
  [
    PHP_ADD_EXTENSION_DEP(pdo_oci, pdo)
  ])
  
fi

fi
