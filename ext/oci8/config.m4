dnl
dnl $Id$
dnl

if test -z "$SED"; then
  PHP_OCI8_SED="sed";
else
  PHP_OCI8_SED="$SED";
fi

PHP_OCI8_TAIL1=`echo a | tail -n1 2>/dev/null`
if test "$PHP_OCI8_TAIL1" = "a"; then
    PHP_OCI8_TAIL1="tail -n1"
else
    PHP_OCI8_TAIL1="tail -1"
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
  AC_MSG_CHECKING([ORACLE_HOME library validity])
  if test ! -d "$OCI8_DIR"; then
    AC_MSG_ERROR([${OCI8_DIR} is not a directory])
  fi
  if test -d "$OCI8_DIR/lib" && test ! -d "$OCI8_DIR/lib32"; then
    OCI8_LIB_DIR=lib
  elif test ! -d "$OCI8_DIR/lib" && test -d "$OCI8_DIR/lib32"; then
    OCI8_LIB_DIR=lib32
  elif test -d "$OCI8_DIR/lib" && test -d "$OCI8_DIR/lib32"; then
    OCI8_LIB_DIR=$PHP_OCI8_OH_LIBDIR
  else
    dnl This isn't an ORACLE_HOME.  Try heuristic examination of the dir to help the user
    if test -f "$OCI8_DIR/libociei.$SHLIB_SUFFIX_NAME"; then
      AC_MSG_ERROR([Expected an ORACLE_HOME top level directory but ${OCI8_DIR} appears to be an Instant Client directory. Try --with-oci8=instantclient,${OCI8_DIR}])
    else
      AC_MSG_ERROR([Oracle library directory not found in ${OCI8_DIR}])
    fi
  fi
  AC_MSG_RESULT($OCI8_LIB_DIR)
])

AC_DEFUN([AC_OCI8IC_VERSION],[
  AC_MSG_CHECKING([Oracle Instant Client library version compatibility])
  OCI8_LCS_BASE=$PHP_OCI8_INSTANT_CLIENT/libclntsh.$SHLIB_SUFFIX_NAME
  OCI8_LCS=`ls $OCI8_LCS_BASE.*.1 2> /dev/null | $PHP_OCI8_TAIL1`  # Oracle 10g, 11g etc
  OCI8_NNZ=`ls $PHP_OCI8_INSTANT_CLIENT/libnnz*.$SHLIB_SUFFIX_NAME 2> /dev/null | $PHP_OCI8_TAIL1`
  if test -f "$OCI8_NNZ" && test -f "$OCI8_LCS"; then
    if test ! -f "$OCI8_LCS_BASE"; then
      AC_MSG_ERROR([Link from $OCI8_LCS_BASE to $OCI8_LCS_BASE.*.1 not found])
    fi
    OCI8_ORACLE_VERSION=`echo $OCI8_LCS | $PHP_OCI8_SED -e 's/.*\.\(.*\)\.1$/\1.1/'`
  else
    AC_MSG_ERROR([Oracle Instant Client libraries libnnz.$SHLIB_SUFFIX_NAME and libclntsh.$SHLIB_SUFFIX_NAME not found])
  fi
  AC_MSG_RESULT([$OCI8_ORACLE_VERSION])
])

AC_DEFUN([AC_OCI8_ORACLE_VERSION],[
  AC_MSG_CHECKING([Oracle library version compatibility])
  OCI8_LCS_BASE=$OCI8_DIR/$OCI8_LIB_DIR/libclntsh.$SHLIB_SUFFIX_NAME
  OCI8_LCS=`ls $OCI8_LCS_BASE.*.1 2> /dev/null | $PHP_OCI8_TAIL1`  # Oracle 10g, 11g etc
  if test -s "$OCI8_DIR/orainst/unix.rgs"; then
    OCI8_ORACLE_VERSION=`grep '"ocommon"' $OCI8_DIR/orainst/unix.rgs | $PHP_OCI8_SED 's/[ ][ ]*/:/g' | cut -d: -f 6 | cut -c 2-4`
    test -z "$OCI8_ORACLE_VERSION" && OCI8_ORACLE_VERSION=7.3
  elif test -f "$OCI8_LCS"; then
    dnl Oracle 10g, 11g etc.  The x.2 version libraries are named x.1 for drop in compatibility
    OCI8_ORACLE_VERSION=`echo $OCI8_LCS | $PHP_OCI8_SED -e 's/.*\.\(.*\)\.1$/\1.1/'`
  elif test -f $OCI8_LCS_BASE.9.0; then
    dnl There is no case for Oracle 9.2. Oracle 9.2 libraries have a 9.0 suffix for drop-in compatibility with Oracle 9.0
    OCI8_ORACLE_VERSION=9.0
  elif test -f $OCI8_LCS_BASE.8.0; then
    OCI8_ORACLE_VERSION=8.1
  elif test -f $OCI8_LCS_BASE.1.0; then
    OCI8_ORACLE_VERSION=8.0
  elif test -f $OCI8_DIR/$OCI8_LIB_DIR/libclntsh.a; then 
    if test -f $OCI8_DIR/$OCI8_LIB_DIR/libcore4.a; then 
      OCI8_ORACLE_VERSION=8.0
    else
      OCI8_ORACLE_VERSION=8.1
    fi
  else
    AC_MSG_ERROR(Oracle libclntsh.$SHLIB_SUFFIX_NAME client library not found)
  fi
  AC_MSG_RESULT($OCI8_ORACLE_VERSION)
])


dnl --with-oci8=shared,instantclient,/path/to/client/dir/lib
dnl or
dnl --with-oci8=shared,/path/to/oracle/home
PHP_ARG_WITH(oci8, for Oracle Database OCI8 support,
[  --with-oci8[=DIR]       Include Oracle Database OCI8 support. DIR defaults to \$ORACLE_HOME.
                          Use --with-oci8=instantclient,/path/to/instant/client/lib 
                          to use an Oracle Instant Client installation])

if test "$PHP_OCI8" != "no"; then 

  if test -z "$PHP_OCI8"; then
    dnl --with-oci8=$ORACLE_HOME where ORACLE_HOME isn't set (or is mistyped) will match this case
    AC_MSG_ERROR([Empty parameter value passed to --with-oci8])
  fi

  dnl Check PHP version is compatible with this extension

  AC_MSG_CHECKING([PHP version])

  tmp_version=$PHP_VERSION
  if test -z "$tmp_version"; then
    if test -z "$PHP_CONFIG"; then
      AC_MSG_ERROR([php-config not found])
    fi
    php_version=`$PHP_CONFIG --version 2>/dev/null|head -n 1|$PHP_OCI8_SED -e 's#\([0-9]\.[0-9]*\.[0-9]*\)\(.*\)#\1#'`
  else
    php_version=`echo "$tmp_version"|$PHP_OCI8_SED -e 's#\([0-9]\.[0-9]*\.[0-9]*\)\(.*\)#\1#'`
  fi

  if test -z "$php_version"; then
    AC_MSG_ERROR([failed to detect PHP version, please report])
  fi

  ac_IFS=$IFS
  IFS="."
  set $php_version
  IFS=$ac_IFS
  oci8_php_version=`expr [$]1 \* 1000000 + [$]2 \* 1000 + [$]3`

  if test "$oci8_php_version" -lt "4003009"; then
    AC_MSG_ERROR([You need at least PHP 4.3.9 to be able to use this version of OCI8. PHP $php_version found])
  elif test "$oci8_php_version" -ge "6000000"; then
    AC_MSG_ERROR([This version of OCI8 is not compatible with PHP 6 or higher])
  else
    AC_MSG_RESULT([$php_version, ok])
  fi

  dnl Set some port specific directory components for use later

  AC_CHECK_SIZEOF(long int, 4)
  AC_MSG_CHECKING([checking if we're on a 64-bit platform])
  if test "$ac_cv_sizeof_long_int" = "4"; then
    AC_MSG_RESULT([no])
    PHP_OCI8_OH_LIBDIR=lib32 
    PHP_OCI8_IC_LIBDIR_SUFFIX=""
  else
    AC_MSG_RESULT([yes])
    PHP_OCI8_OH_LIBDIR=lib
    PHP_OCI8_IC_LIBDIR_SUFFIX=64
  fi

  dnl Determine if the user wants to use Oracle Instant Client libraries

  PHP_OCI8_INSTANT_CLIENT="no"

  dnl PECL Bug 14268 (Allow "pecl install oci8" command to "autodetect"
  dnl an Instant Client RPM install).  This also has a benefit for non
  dnl "pecl install" builds: if the user does --with-oci8 or
  dnl --with-oci8=shared but the ORACLE_HOME environment variable is not
  dnl actually set, the install tries to locate the Instant Client RPM
  if test "$PHP_OCI8" = "yes" && test -z "$ORACLE_HOME"; then
    AC_MSG_WARN([OCI8 extension: ORACLE_HOME is not set, looking for default Oracle Instant Client instead])
    PHP_OCI8=instantclient
  fi

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

  if test "$PHP_OCI8_INSTANT_CLIENT" = "no"; then
    dnl ***************************************
    dnl Branch for using an ORACLE_HOME install
    dnl ***************************************

    AC_MSG_CHECKING([Oracle ORACLE_HOME install directory])

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

    AC_OCI8_ORACLE_VERSION($OCI8_DIR)

    case $OCI8_ORACLE_VERSION in
      7.3|8.0|8.1)
	AC_MSG_ERROR([Oracle client libraries < 9.2 are not supported])
	;;

      9.0)
	PHP_CHECK_LIBRARY(clntsh, OCIEnvNlsCreate,
	[
	  OCI8_ORACLE_VERSION=9.2
	],
	[
	  AC_MSG_ERROR([Oracle client libraries < 9.2 are not supported])
	], [
	  -L$OCI8_DIR/$OCI8_LIB_DIR $OCI8_SHARED_LIBADD
	])
	;;

      *)
	AC_DEFINE(HAVE_OCI_LOB_READ2,1,[ ])
	;;
    esac

    PHP_ADD_LIBRARY(clntsh, 1, OCI8_SHARED_LIBADD)
    PHP_ADD_LIBPATH($OCI8_DIR/$OCI8_LIB_DIR, OCI8_SHARED_LIBADD)
    PHP_NEW_EXTENSION(oci8, oci8.c oci8_lob.c oci8_statement.c oci8_collection.c oci8_interface.c, $ext_shared)
    AC_DEFINE(HAVE_OCI8,1,[ ])

    PHP_SUBST_OLD(OCI8_SHARED_LIBADD)
    PHP_SUBST_OLD(OCI8_DIR)
    PHP_SUBST_OLD(OCI8_ORACLE_VERSION)

    dnl Fix Bug #46623
    AC_DEFINE_UNQUOTED(PHP_OCI8_DEF_DIR, "$OCI8_DIR", [ ])
    AC_DEFINE_UNQUOTED(PHP_OCI8_DEF_SHARED_LIBADD, "$OCI8_SHARED_LIBADD", [ ])

  else
    dnl ***************************************************
    dnl Branch for using an Oracle Instant Client directory
    dnl ***************************************************

    AC_MSG_CHECKING([Oracle Instant Client directory])

    if test "$PHP_OCI8_INSTANT_CLIENT" = "yes"; then
      dnl Find the directory if user specified "instantclient" but did not
      dnl give a dir. Generally the Instant Client can be anywhere so the
      dnl user must pass in the library directory.  But on Linux we default
      dnl to the most recent version in /usr/lib which is where the Oracle
      dnl Instant Client RPM gets installed.
      PHP_OCI8_INSTANT_CLIENT=`ls -d /usr/lib/oracle/*/client${PHP_OCI8_IC_LIBDIR_SUFFIX}/lib/libclntsh.* 2> /dev/null | $PHP_OCI8_TAIL1 | $PHP_OCI8_SED -e 's#/libclntsh[^/]*##'`
      if test -z "$PHP_OCI8_INSTANT_CLIENT"; then
	AC_MSG_ERROR([Oracle Instant Client directory /usr/lib/oracle/.../client${PHP_OCI8_IC_LIBDIR_SUFFIX}/lib libraries not found. Try --with-oci8=instantclient,DIR])
      fi
    fi
    AC_MSG_RESULT($PHP_OCI8_INSTANT_CLIENT)

    OCI8_DIR=$PHP_OCI8_INSTANT_CLIENT

    AC_MSG_CHECKING([Oracle Instant Client SDK header directory])

    dnl Header directory for Instant Client SDK RPM install
    OCISDKRPMINC=`echo "$PHP_OCI8_INSTANT_CLIENT" | $PHP_OCI8_SED -e 's!^/usr/lib/oracle/\(.*\)/client\('${PHP_OCI8_IC_LIBDIR_SUFFIX}'\)*/lib[/]*$!/usr/include/oracle/\1/client\2!'`

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
    PHP_ADD_LIBRARY(clntsh, 1, OCI8_SHARED_LIBADD)
    PHP_ADD_LIBPATH($PHP_OCI8_INSTANT_CLIENT, OCI8_SHARED_LIBADD)

    AC_DEFINE(HAVE_OCI_INSTANT_CLIENT,1,[ ])
    AC_DEFINE(HAVE_OCI_LOB_READ2,1,[ ])

    PHP_NEW_EXTENSION(oci8, oci8.c oci8_lob.c oci8_statement.c oci8_collection.c oci8_interface.c, $ext_shared)
    AC_DEFINE(HAVE_OCI8,1,[ ])

    PHP_SUBST_OLD(OCI8_SHARED_LIBADD)
    PHP_SUBST_OLD(OCI8_DIR)
    PHP_SUBST_OLD(OCI8_ORACLE_VERSION)

  fi
fi
