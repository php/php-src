dnl
dnl $Id$
dnl

if test -z "$SED"; then
  PHP_OCI8_SED="sed";
else
  PHP_OCI8_SED="$SED";
fi

AC_DEFUN([PHP_OCI_IF_DEFINED],[
  old_CPPFLAGS=$CPPFLAGS
  CPPFLAGS=$3
  AC_EGREP_CPP(yes,[
#include <oci.h>
#if defined($1)
    yes
#endif
  ],[
    CPPFLAGS=$old_CPPFLAGS
    $2
  ],[
    CPPFLAGS=$old_CPPFLAGS
  ])
])

AC_DEFUN([AC_OCI8_CHECK_LIB_DIR],[
  AC_CHECK_SIZEOF(long int, 4)
  AC_MSG_CHECKING([if we're on a 64-bit platform])
  if test "$ac_cv_sizeof_long_int" = "4" ; then
    AC_MSG_RESULT([no])
    TMP_OCI8_LIB_DIR=lib32 
  else
    AC_MSG_RESULT([yes])
    TMP_OCI8_LIB_DIR=lib
  fi

  AC_MSG_CHECKING([OCI8 libraries dir])
  if test -d "$OCI8_DIR/lib" && test ! -d "$OCI8_DIR/lib32"; then
    OCI8_LIB_DIR=lib
  elif test ! -d "$OCI8_DIR/lib" && test -d "$OCI8_DIR/lib32"; then
    OCI8_LIB_DIR=lib32
  elif test -d "$OCI8_DIR/lib" && test -d "$OCI8_DIR/lib32"; then
    OCI8_LIB_DIR=$TMP_OCI8_LIB_DIR
  else
    AC_MSG_ERROR([Oracle (OCI8) required libraries not found])
  fi
  AC_MSG_RESULT($OCI8_LIB_DIR)
])

AC_DEFUN([AC_OCI8IC_VERSION],[
  AC_MSG_CHECKING([Oracle Instant Client version])
  if test -f $PHP_OCI8_INSTANT_CLIENT/libnnz11.$SHLIB_SUFFIX_NAME; then
    if test -f $PHP_OCI8_INSTANT_CLIENT/libclntsh.$SHLIB_SUFFIX_NAME.11.1; then
      if test ! -f $PHP_OCI8_INSTANT_CLIENT/libclntsh.$SHLIB_SUFFIX_NAME; then
        AC_MSG_ERROR([Link from $PHP_OCI8_INSTANT_CLIENT/libclntsh.$SHLIB_SUFFIX_NAME to libclntsh.$SHLIB_SUFFIX_NAME.11.1 not found])
      fi
      OCI8_VERSION=11.1
    else
      AC_MSG_ERROR([Oracle Instant Client library version not supported])
    fi
  elif test -f $PHP_OCI8_INSTANT_CLIENT/libnnz10.$SHLIB_SUFFIX_NAME; then
    if test -f $PHP_OCI8_INSTANT_CLIENT/libclntsh.$SHLIB_SUFFIX_NAME.10.1; then
      if test ! -f $PHP_OCI8_INSTANT_CLIENT/libclntsh.$SHLIB_SUFFIX_NAME; then
        AC_MSG_ERROR([Link from $PHP_OCI8_INSTANT_CLIENT/libclntsh.$SHLIB_SUFFIX_NAME to libclntsh.$SHLIB_SUFFIX_NAME.10.1 not found])
      fi
      OCI8_VERSION=10.1
    else
      AC_MSG_ERROR([Oracle Instant Client library version not supported])
    fi
  else
    AC_MSG_ERROR([Oracle Instant Client libraries not found])
  fi
  AC_MSG_RESULT([$OCI8_VERSION])
])


AC_DEFUN([AC_OCI8_VERSION],[
  AC_MSG_CHECKING([Oracle version])
  if test -s "$OCI8_DIR/orainst/unix.rgs"; then
    OCI8_VERSION=`grep '"ocommon"' $OCI8_DIR/orainst/unix.rgs | $PHP_OCI8_SED 's/[ ][ ]*/:/g' | cut -d: -f 6 | cut -c 2-4`
    test -z "$OCI8_VERSION" && OCI8_VERSION=7.3
  elif test -f $OCI8_DIR/$OCI8_LIB_DIR/libclntsh.$SHLIB_SUFFIX_NAME.11.1; then
    OCI8_VERSION=11.1
  elif test -f $OCI8_DIR/$OCI8_LIB_DIR/libclntsh.$SHLIB_SUFFIX_NAME.10.1; then
    OCI8_VERSION=10.1    
  elif test -f $OCI8_DIR/$OCI8_LIB_DIR/libclntsh.$SHLIB_SUFFIX_NAME.9.0; then
    OCI8_VERSION=9.0
  elif test -f $OCI8_DIR/$OCI8_LIB_DIR/libclntsh.$SHLIB_SUFFIX_NAME.8.0; then
    OCI8_VERSION=8.1
  elif test -f $OCI8_DIR/$OCI8_LIB_DIR/libclntsh.$SHLIB_SUFFIX_NAME.1.0; then
    OCI8_VERSION=8.0
  elif test -f $OCI8_DIR/$OCI8_LIB_DIR/libclntsh.a; then 
    if test -f $OCI8_DIR/$OCI8_LIB_DIR/libcore4.a; then 
      OCI8_VERSION=8.0
    else
      OCI8_VERSION=8.1
    fi
  else
    AC_MSG_ERROR(Oracle-OCI8 needed libraries not found)
  fi
  AC_MSG_RESULT($OCI8_VERSION)
])


dnl --with-oci8=shared,instantclient,/path/to/client/dir/lib
dnl or
dnl --with-oci8=shared,/path/to/oracle/home
PHP_ARG_WITH(oci8, for Oracle (OCI8) support,
[  --with-oci8[=DIR]       Include Oracle (OCI8) support. DIR defaults to \$ORACLE_HOME.
                          Use --with-oci8=instantclient,/path/to/oic/lib 
                          for an Oracle Instant Client installation])

if test "$PHP_OCI8" != "no"; then 
  AC_MSG_CHECKING([PHP version])

  tmp_version=$PHP_VERSION
  if test -z "$tmp_version"; then
    if test -z "$PHP_CONFIG"; then
      AC_MSG_ERROR([php-config not found])
    fi
    php_version=`$PHP_CONFIG --version 2>/dev/null|head -n 1|sed -e 's#\([0-9]\.[0-9]*\.[0-9]*\)\(.*\)#\1#'`
  else
    php_version=`echo "$tmp_version"|sed -e 's#\([0-9]\.[0-9]*\.[0-9]*\)\(.*\)#\1#'`
  fi

  if test -z "$php_version"; then
    AC_MSG_ERROR([failed to detect PHP version, please report])
  fi

  ac_IFS=$IFS
  IFS="."
  set $php_version
  IFS=$ac_IFS
  oci8_php_version=`expr [$]1 \* 1000000 + [$]2 \* 1000 + [$]3`

  if test "$oci8_php_version" -le "4003010"; then
    AC_MSG_ERROR([You need at least PHP 4.3.10 to be able to use this version of OCI8. PHP $php_version found])
  else
    AC_MSG_RESULT([$php_version, ok])
  fi
fi

PHP_OCI8_INSTANT_CLIENT="no"

if test "`echo $PHP_OCI8`" = "instantclient"; then
    PHP_OCI8_INSTANT_CLIENT="yes"
elif test "`echo $PHP_OCI8 | cut -d, -f2`" = "instantclient"; then
    PHP_OCI8_INSTANT_CLIENT="`echo $PHP_OCI8 | cut -d, -f3`"
    PHP_OCI8="`echo $PHP_OCI8 | cut -d, -f1,4`"
    if test "$PHP_OCI8_INSTANT_CLIENT" = ""; then
        PHP_OCI8_INSTANT_CLIENT="yes"
    fi
    if test -z "$PHP_OCI8"; then
        PHP_OCI8=yes
    fi
elif test "`echo $PHP_OCI8 | cut -d, -f1`" = "instantclient"; then
    PHP_OCI8_INSTANT_CLIENT="`echo $PHP_OCI8 | cut -d, -f2`"
    PHP_OCI8="`echo $PHP_OCI8 | cut -d, -f3,4`"
    if test "$PHP_OCI8_INSTANT_CLIENT" = ""; then
        PHP_OCI8_INSTANT_CLIENT="yes"
    fi
    if test -z "$PHP_OCI8"; then
        PHP_OCI8=yes
    fi
fi

if test "$PHP_OCI8" != "no" && test "$PHP_OCI8_INSTANT_CLIENT" = "no"; then

  AC_MSG_CHECKING([Oracle Install Directory])
  if test "$PHP_OCI8" = "yes"; then
    OCI8_DIR=$ORACLE_HOME
  else
    OCI8_DIR=$PHP_OCI8
  fi
  AC_MSG_RESULT($OCI8_DIR)

  AC_OCI8_CHECK_LIB_DIR($OCI8_DIR)

  if test -d "$OCI8_DIR/rdbms/public"; then
    PHP_ADD_INCLUDE($OCI8_DIR/rdbms/public)
    OCI8_INCLUDES="$OCI8_INCLUDES -I$OCI8_DIR/rdbms/public"
  fi
  if test -d "$OCI8_DIR/rdbms/demo"; then
    PHP_ADD_INCLUDE($OCI8_DIR/rdbms/demo)
    OCI8_INCLUDES="$OCI8_INCLUDES -I$OCI8_DIR/rdbms/demo"
  fi
  if test -d "$OCI8_DIR/network/public"; then
    PHP_ADD_INCLUDE($OCI8_DIR/network/public)
    OCI8_INCLUDES="$OCI8_INCLUDES -I$OCI8_DIR/network/public"
  fi
  if test -d "$OCI8_DIR/plsql/public"; then
    PHP_ADD_INCLUDE($OCI8_DIR/plsql/public)
    OCI8_INCLUDES="$OCI8_INCLUDES -I$OCI8_DIR/plsql/public"
  fi

  if test -f "$OCI8_DIR/$OCI8_LIB_DIR/sysliblist"; then
    PHP_EVAL_LIBLINE(`cat $OCI8_DIR/$OCI8_LIB_DIR/sysliblist`, OCI8_SYSLIB)
  elif test -f "$OCI8_DIR/rdbms/$OCI8_LIB_DIR/sysliblist"; then
    PHP_EVAL_LIBLINE(`cat $OCI8_DIR/rdbms/$OCI8_LIB_DIR/sysliblist`, OCI8_SYSLIB)
  fi

  AC_OCI8_VERSION($OCI8_DIR)

  case $OCI8_VERSION in
    8.0)
      AC_MSG_ERROR([Oracle client libraries < 9.0 are not supported any more. Please consider upgrading.])
      ;;

    8.1)
      AC_MSG_ERROR([Oracle client libraries < 9.0 are not supported any more. Please consider upgrading.])
      ;;

    9.0)
      PHP_ADD_LIBRARY(clntsh, 1, OCI8_SHARED_LIBADD)
      PHP_ADD_LIBPATH($OCI8_DIR/$OCI8_LIB_DIR, OCI8_SHARED_LIBADD)

      dnl These functions are only available in version >= 9.2
      PHP_CHECK_LIBRARY(clntsh, OCIEnvNlsCreate,
      [
        PHP_CHECK_LIBRARY(clntsh, OCINlsCharSetNameToId,
        [
          AC_DEFINE(HAVE_OCI_ENV_NLS_CREATE,1,[ ])
          OCI8_VERSION=9.2
        ], [], [
          -L$OCI8_DIR/$OCI8_LIB_DIR $OCI8_SHARED_LIBADD
        ])
      ], [], [
        -L$OCI8_DIR/$OCI8_LIB_DIR $OCI8_SHARED_LIBADD
      ])

      PHP_CHECK_LIBRARY(clntsh, OCIEnvCreate,
      [
        AC_DEFINE(HAVE_OCI_ENV_CREATE,1,[ ])
      ], [], [
        -L$OCI8_DIR/$OCI8_LIB_DIR $OCI8_SHARED_LIBADD
      ])
 
      PHP_CHECK_LIBRARY(clntsh, OCIStmtPrepare2,
      [
        AC_DEFINE(HAVE_OCI_STMT_PREPARE2,1,[ ])
      ], [], [
        -L$OCI8_DIR/$OCI8_LIB_DIR $OCI8_SHARED_LIBADD
      ])

      PHP_CHECK_LIBRARY(clntsh, OCILobRead2,
      [
        AC_DEFINE(HAVE_OCI_LOB_READ2,1,[ ])
      ], [], [
        -L$OCI8_DIR/$OCI8_LIB_DIR $OCI8_SHARED_LIBADD
      ])
 
      ;;
      
    11.1|10.1)
      PHP_ADD_LIBRARY(clntsh, 1, OCI8_SHARED_LIBADD)
      PHP_ADD_LIBPATH($OCI8_DIR/$OCI8_LIB_DIR, OCI8_SHARED_LIBADD)
      AC_DEFINE(HAVE_OCI_ENV_NLS_CREATE,1,[ ])
      AC_DEFINE(HAVE_OCI_ENV_CREATE,1,[ ])
      AC_DEFINE(HAVE_OCI_STMT_PREPARE2,1,[ ])
      AC_DEFINE(HAVE_OCI_LOB_READ2,1,[ ])
      AC_DEFINE(HAVE_OCI8_TEMP_LOB,1,[ ])
      AC_DEFINE(PHP_OCI8_HAVE_COLLECTIONS,1,[ ])
      ;;
    *)
      AC_MSG_ERROR([Oracle version $OCI8_VERSION is not supported])
      ;;
  esac

  dnl
  dnl Check if we need to add -locijdbc8 
  dnl
  PHP_CHECK_LIBRARY(clntsh, OCILobIsTemporary,
  [
    AC_DEFINE(HAVE_OCI8_TEMP_LOB,1,[ ])
  ], [
    PHP_CHECK_LIBRARY(ocijdbc8, OCILobIsTemporary,
    [
      PHP_ADD_LIBRARY(ocijdbc8, 1, OCI8_SHARED_LIBADD)
      AC_DEFINE(HAVE_OCI8_TEMP_LOB,1,[ ])
    ], [], [
      -L$OCI8_DIR/$OCI8_LIB_DIR $OCI8_SHARED_LIBADD
    ])
  ], [
    -L$OCI8_DIR/$OCI8_LIB_DIR $OCI8_SHARED_LIBADD
  ])

  dnl
  dnl Check if we have collections
  dnl
  PHP_CHECK_LIBRARY(clntsh, OCICollAssign,
  [
    AC_DEFINE(PHP_OCI8_HAVE_COLLECTIONS,1,[ ])
    PHP_NEW_EXTENSION(oci8, oci8.c oci8_lob.c oci8_statement.c oci8_collection.c oci8_interface.c, $ext_shared)
  ], 
  [
    PHP_NEW_EXTENSION(oci8, oci8.c oci8_lob.c oci8_statement.c oci8_interface.c, $ext_shared)
  ], 
  [
    -L$OCI8_DIR/$OCI8_LIB_DIR $OCI8_SHARED_LIBADD
  ])

  AC_DEFINE(HAVE_OCI8,1,[ ])

  PHP_SUBST_OLD(OCI8_SHARED_LIBADD)
  PHP_SUBST_OLD(OCI8_DIR)
  PHP_SUBST_OLD(OCI8_VERSION)
  
elif test "$PHP_OCI8" != "no" && test "$PHP_OCI8_INSTANT_CLIENT" != "no"; then

  AC_CHECK_SIZEOF(long int, 4)
  LIBDIR_SUFFIX=""
  if test "$ac_cv_sizeof_long_int" = "8" ; then
    LIBDIR_SUFFIX=64
  fi

  AC_MSG_CHECKING([Oracle Instant Client directory])
  if test "$PHP_OCI8_INSTANT_CLIENT" = "yes"; then
dnl Find the directory if user specified "instantclient" but did not give a dir.
dnl Generally the Instant Client can be anywhere so the user must pass in the
dnl directory to the libraries.  But on Linux we default to the most recent
dnl version in /usr/lib
    PHP_OCI8_INSTANT_CLIENT=`ls -d /usr/lib/oracle/*/client${LIBDIR_SUFFIX}/lib 2> /dev/null | tail -1`
    if test -z "$PHP_OCI8_INSTANT_CLIENT"; then
      AC_MSG_ERROR([Oracle Instant Client directory /usr/lib/oracle/.../client${LIBDIR_SUFFIX}/lib not found. Try --with-oci8=instantclient,DIR])
    fi
  fi
  AC_MSG_RESULT($PHP_OCI8_INSTANT_CLIENT)

  OCI8_DIR=$PHP_OCI8_INSTANT_CLIENT

  AC_MSG_CHECKING([Oracle Instant Client SDK header directory])

dnl Header directory for Instant Client SDK RPM install
  OCISDKRPMINC=`echo "$PHP_OCI8_INSTANT_CLIENT" | $PHP_OCI8_SED -e 's!^/usr/lib/oracle/\(.*\)/client/lib[/]*$!/usr/include/oracle/\1/client!'`

dnl Header directory for Instant Client SDK zip file install
  OCISDKZIPINC=$PHP_OCI8_INSTANT_CLIENT/sdk/include

dnl Header directory for manual installation
  OCISDKMANINC=`echo "$PHP_OCI8_INSTANT_CLIENT" | $PHP_OCI8_SED -e 's!\(.*\)/lib[/]*$!\1/include!'`
  
  if test -f "$OCISDKRPMINC/oci.h"; then
    AC_MSG_RESULT($OCISDKRPMINC)
    PHP_ADD_INCLUDE($OCISDKRPMINC)
    OCI8INCDIR=$OCISDKRPMINC
  elif test -f "$OCISDKZIPINC/oci.h"; then
    AC_MSG_RESULT($OCISDKZIPINC)
    PHP_ADD_INCLUDE($OCISDKZIPINC)
    OCI8INCDIR=$OCISDKZIPINC
  elif test -f "$OCISDKMANINC/oci.h"; then
    AC_MSG_RESULT($OCISDKMANINC)
    PHP_ADD_INCLUDE($OCISDKMANINC)
    OCI8INCDIR=$OCISDKMANINC
  else
    AC_MSG_ERROR([Oracle Instant Client SDK header files not found])
  fi

  OCISYSLIBLIST=`echo "$OCI8INCDIR" | $PHP_OCI8_SED -e 's!\(.*\)/include$!\1/demo/sysliblist!'`
  if test -f "$OCISYSLIBLIST"; then
    PHP_EVAL_LIBLINE(`cat $OCISYSLIBLIST`, OCI8_SYSLIB)
  fi

  AC_OCI8IC_VERSION($PHP_OCI8_INSTANT_CLIENT)
  case $OCI8_VERSION in
    11.1|10.1)
      PHP_ADD_LIBRARY(clntsh, 1, OCI8_SHARED_LIBADD)
      PHP_ADD_LIBPATH($PHP_OCI8_INSTANT_CLIENT, OCI8_SHARED_LIBADD)
      ;;

    *)
      AC_MSG_ERROR([Oracle Instant Client version $PHP_OCI8_INSTANT_CLIENT is not supported])
      ;;
  esac

  AC_DEFINE(HAVE_OCI_INSTANT_CLIENT,1,[ ])
  AC_DEFINE(HAVE_OCI_ENV_NLS_CREATE,1,[ ])
  AC_DEFINE(HAVE_OCI_ENV_CREATE,1,[ ])
  AC_DEFINE(HAVE_OCI_STMT_PREPARE2,1,[ ])
  AC_DEFINE(HAVE_OCI_LOB_READ2,1,[ ])
  AC_DEFINE(HAVE_OCI8_TEMP_LOB,1,[ ])
  AC_DEFINE(PHP_OCI8_HAVE_COLLECTIONS,1,[ ])

  PHP_NEW_EXTENSION(oci8, oci8.c oci8_lob.c oci8_statement.c oci8_collection.c oci8_interface.c, $ext_shared)
  AC_DEFINE(HAVE_OCI8,1,[ ])

  PHP_SUBST_OLD(OCI8_SHARED_LIBADD)
  PHP_SUBST_OLD(OCI8_DIR)
  PHP_SUBST_OLD(OCI8_VERSION)
 
fi
