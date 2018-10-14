dnl config.m4 for extension pdo_oci
dnl vim:et:sw=2:ts=2:

if test -z "$SED"; then
  PHP_PDO_OCI_SED="sed";
else
  PHP_PDO_OCI_SED="$SED";
fi

PHP_PDO_OCI_TAIL1=`echo a | tail -n1 2>/dev/null`
if test "$PHP_PDO_OCI_TAIL1" = "a"; then
    PHP_PDO_OCI_TAIL1="tail -n1"
else
    PHP_PDO_OCI_TAIL1="tail -1"
fi

AC_DEFUN([AC_PDO_OCI_VERSION],[
  AC_MSG_CHECKING([Oracle version])
  PDO_OCI_LCS_BASE=$PDO_OCI_LIB_DIR/libclntsh.$SHLIB_SUFFIX_NAME
  PDO_OCI_LCS=`ls $PDO_OCI_LCS_BASE.*.1 2> /dev/null | $PHP_PDO_OCI_TAIL1`  # Oracle 10g, 11g, 12c etc
  if test -f "$PDO_OCI_LCS"; then
    dnl Oracle 10g, 11g 12c etc.  The x.2 version libraries are named x.1 for drop in compatibility
    PDO_OCI_VERSION=`echo $PDO_OCI_LCS | $PHP_PDO_OCI_SED -e 's/.*\.\(.*\)\.1$/\1.1/'`
  elif test -f $PDO_OCI_LCS_BASE.9.0; then
    dnl There is no case for Oracle 9.2. Oracle 9.2 libraries have a 9.0 suffix for drop-in compatibility with Oracle 9.0
    PDO_OCI_VERSION=9.0
  else
    AC_MSG_ERROR(Oracle libclntsh.$SHLIB_SUFFIX_NAME client library not found or its version is lower than 9)
  fi
  AC_MSG_RESULT($PDO_OCI_VERSION)
])

AC_DEFUN([AC_PDO_OCI_CHECK_LIB_DIR],[
  AC_CHECK_SIZEOF(long int, 4)
  AC_MSG_CHECKING([if we're on a 64-bit platform])
  if test "$ac_cv_sizeof_long_int" = "4" ; then
    AC_MSG_RESULT([no])
    TMP_PDO_OCI_LIB_DIR="$PDO_OCI_DIR/lib32"
  else
    AC_MSG_RESULT([yes])
    TMP_PDO_OCI_LIB_DIR="$PDO_OCI_DIR/lib"
  fi

  AC_MSG_CHECKING([OCI8 libraries dir])
  if test -d "$PDO_OCI_DIR/lib" && test ! -d "$PDO_OCI_DIR/lib32"; then
    PDO_OCI_LIB_DIR="$PDO_OCI_DIR/lib"
  elif test ! -d "$PDO_OCI_DIR/lib" && test -d "$PDO_OCI_DIR/lib32"; then
    PDO_OCI_LIB_DIR="$PDO_OCI_DIR/lib32"
  elif test -d "$PDO_OCI_DIR/lib" && test -d "$PDO_OCI_DIR/lib32"; then
    PDO_OCI_LIB_DIR=$TMP_PDO_OCI_LIB_DIR
  else
    AC_MSG_ERROR([Oracle required OCI8 libraries not found])
  fi
  AC_MSG_RESULT($PDO_OCI_LIB_DIR)
])

PHP_ARG_WITH(pdo-oci, Oracle OCI support for PDO,
[  --with-pdo-oci[=DIR]      PDO: Oracle OCI support. DIR defaults to [$]ORACLE_HOME.
                          Use --with-pdo-oci=instantclient,/path/to/instant/client/lib
                          for an Oracle Instant Client installation.])

if test "$PHP_PDO_OCI" != "no"; then

  if test "$PHP_PDO" = "no" && test "$ext_shared" = "no"; then
    AC_MSG_ERROR([PDO is not enabled! Add --enable-pdo to your configure line.])
  fi

  AC_MSG_CHECKING([Oracle Install-Dir])
  if test "$PHP_PDO_OCI" = "yes" || test -z "$PHP_PDO_OCI"; then
    PDO_OCI_DIR=$ORACLE_HOME
  else
    PDO_OCI_DIR=$PHP_PDO_OCI
  fi
  AC_MSG_RESULT($PHP_PDO_OCI)

  AC_MSG_CHECKING([if that is sane])
  if test -z "$PDO_OCI_DIR"; then
    AC_MSG_ERROR([You need to tell me where to find your Oracle Instant Client SDK, or set ORACLE_HOME.])
  else
    AC_MSG_RESULT([yes])
  fi

  if test "instantclient" = "`echo $PDO_OCI_DIR | cut -d, -f1`" ; then
    AC_CHECK_SIZEOF(long int, 4)
    if test "$ac_cv_sizeof_long_int" = "4" ; then
      PDO_OCI_CLIENT_DIR="client"
    else
      PDO_OCI_CLIENT_DIR="client64"
    fi
    PDO_OCI_LIB_DIR="`echo $PDO_OCI_DIR | cut -d, -f2`"
    AC_PDO_OCI_VERSION($PDO_OCI_LIB_DIR)

    AC_MSG_CHECKING([for oci.h])
    dnl Header directory for Instant Client SDK RPM install
    OCISDKRPMINC=`echo "$PDO_OCI_LIB_DIR" | $PHP_PDO_OCI_SED -e 's!^\(.*\)/lib/oracle/\(.*\)/\('${PDO_OCI_CLIENT_DIR}'\)/lib[/]*$!\1/include/oracle/\2/\3!'`

    dnl Header directory for manual installation
    OCISDKMANINC=`echo "$PDO_OCI_LIB_DIR" | $PHP_PDO_OCI_SED -e 's!^\(.*\)/lib[/]*$!\1/include!'`

    dnl Header directory for Instant Client SDK zip file install
    OCISDKZIPINC=$PDO_OCI_LIB_DIR/sdk/include


    if test -f "$OCISDKRPMINC/oci.h" ; then
      PHP_ADD_INCLUDE($OCISDKRPMINC)
      AC_MSG_RESULT($OCISDKRPMINC)
    elif test -f "$OCISDKMANINC/oci.h" ; then
      PHP_ADD_INCLUDE($OCISDKMANINC)
      AC_MSG_RESULT($OCISDKMANINC)
    elif test -f "$OCISDKZIPINC/oci.h" ; then
      PHP_ADD_INCLUDE($OCISDKZIPINC)
      AC_MSG_RESULT($OCISDKZIPINC)
    else
      AC_MSG_ERROR([I'm too dumb to figure out where the include dir is in your Instant Client install])
    fi
  else
    AC_PDO_OCI_CHECK_LIB_DIR($PDO_OCI_DIR)

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

    if test -f "$PDO_OCI_LIB_DIR/sysliblist"; then
      PHP_EVAL_LIBLINE(`cat $PDO_OCI_LIB_DIR/sysliblist`, PDO_OCI_SYSLIB)
    elif test -f "$PDO_OCI_DIR/rdbms/lib/sysliblist"; then
      PHP_EVAL_LIBLINE(`cat $PDO_OCI_DIR/rdbms/lib/sysliblist`, PDO_OCI_SYSLIB)
    fi
    AC_PDO_OCI_VERSION($PDO_OCI_LIB_DIR)
  fi

  case $PDO_OCI_VERSION in
    7.3|8.0|8.1)
      AC_MSG_ERROR([Oracle client libraries < 9 are not supported])
      ;;
  esac

  PHP_ADD_LIBRARY(clntsh, 1, PDO_OCI_SHARED_LIBADD)
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
      pdo_cv_inc_path=$abs_srcdir/ext
    elif test -f $abs_srcdir/ext/pdo/php_pdo_driver.h; then
      pdo_cv_inc_path=$abs_srcdir/ext
    elif test -f $phpincludedir/ext/pdo/php_pdo_driver.h; then
      pdo_cv_inc_path=$phpincludedir/ext
    else
      AC_MSG_ERROR([Cannot find php_pdo_driver.h.])
    fi
    AC_MSG_RESULT($pdo_cv_inc_path)
  ])

  PHP_NEW_EXTENSION(pdo_oci, pdo_oci.c oci_driver.c oci_statement.c, $ext_shared,,-I$pdo_cv_inc_path)

  PHP_SUBST_OLD(PDO_OCI_SHARED_LIBADD)
  PHP_SUBST_OLD(PDO_OCI_DIR)
  PHP_SUBST_OLD(PDO_OCI_VERSION)

  ifdef([PHP_ADD_EXTENSION_DEP],
  [
    PHP_ADD_EXTENSION_DEP(pdo_oci, pdo)
  ])

  AC_DEFINE_UNQUOTED(PHP_PDO_OCI_CLIENT_VERSION, "$PDO_OCI_VERSION", [ ])
fi
