dnl
dnl $Id$
dnl

dnl This file is more or less a straight copy from oci8; it won't work with pdo_oci yet!

AC_DEFUN(PHP_OCI_IF_DEFINED,[
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

AC_DEFUN(AC_OCI_VERSION,[
  AC_MSG_CHECKING([Oracle version])
  if test -s "$OCI_DIR/orainst/unix.rgs"; then
    OCI_VERSION=`grep '"ocommon"' $OCI_DIR/orainst/unix.rgs | sed 's/[ ][ ]*/:/g' | cut -d: -f 6 | cut -c 2-4`
    test -z "$OCI_VERSION" && OCI_VERSION=7.3
  elif test -f $OCI_DIR/lib/libclntsh.$SHLIB_SUFFIX_NAME.10.1; then
    OCI_VERSION=10.1    
  elif test -f $OCI_DIR/lib/libclntsh.$SHLIB_SUFFIX_NAME.9.0; then
    OCI_VERSION=9.0
  elif test -f $OCI_DIR/lib/libclntsh.$SHLIB_SUFFIX_NAME.8.0; then
    OCI_VERSION=8.1
  elif test -f $OCI_DIR/lib/libclntsh.$SHLIB_SUFFIX_NAME.1.0; then
    OCI_VERSION=8.0
  elif test -f $OCI_DIR/lib/libclntsh.a; then 
    if test -f $OCI_DIR/lib/libcore4.a; then 
      OCI_VERSION=8.0
    else
      OCI_VERSION=8.1
    fi
  else
    AC_MSG_ERROR(Oracle-OCI needed libraries not found)
  fi
  AC_MSG_RESULT($OCI_VERSION)
])                                                                                                                                                                

PHP_ARG_WITH(oci, for Oracle-OCI support,
[  --with-oci[=DIR]       Include Oracle-oci support. Default DIR is ORACLE_HOME.])

if test "$PHP_OCI" != "no"; then
  AC_MSG_CHECKING([Oracle Install-Dir])
  if test "$PHP_OCI" = "yes"; then
    OCI_DIR=$ORACLE_HOME
  else
    OCI_DIR=$PHP_OCI
  fi
  AC_MSG_RESULT($OCI_DIR)

  if test -d "$OCI_DIR/rdbms/public"; then
    PHP_ADD_INCLUDE($OCI_DIR/rdbms/public)
    OCI_INCLUDES="$OCI_INCLUDES -I$OCI_DIR/rdbms/public"
  fi
  if test -d "$OCI_DIR/rdbms/demo"; then
    PHP_ADD_INCLUDE($OCI_DIR/rdbms/demo)
    OCI_INCLUDES="$OCI_INCLUDES -I$OCI_DIR/rdbms/demo"
  fi
  if test -d "$OCI_DIR/network/public"; then
    PHP_ADD_INCLUDE($OCI_DIR/network/public)
    OCI_INCLUDES="$OCI_INCLUDES -I$OCI_DIR/network/public"
  fi
  if test -d "$OCI_DIR/plsql/public"; then
    PHP_ADD_INCLUDE($OCI_DIR/plsql/public)
    OCI_INCLUDES="$OCI_INCLUDES -I$OCI_DIR/plsql/public"
  fi

  if test -f "$OCI_DIR/lib/sysliblist"; then
    PHP_EVAL_LIBLINE(`cat $OCI_DIR/lib/sysliblist`, OCI_SYSLIB)
  elif test -f "$OCI_DIR/rdbms/lib/sysliblist"; then
    PHP_EVAL_LIBLINE(`cat $OCI_DIR/rdbms/lib/sysliblist`, OCI_SYSLIB)
  fi

  AC_OCI_VERSION($OCI_DIR)
  case $OCI_VERSION in
    8.0)
      PHP_ADD_LIBRARY_WITH_PATH(nlsrtl3, "", OCI_SHARED_LIBADD)
      PHP_ADD_LIBRARY_WITH_PATH(core4, "", OCI_SHARED_LIBADD)
      PHP_ADD_LIBRARY_WITH_PATH(psa, "", OCI_SHARED_LIBADD)
      PHP_ADD_LIBRARY_WITH_PATH(clntsh, $OCI_DIR/lib, OCI_SHARED_LIBADD)
      ;;

    8.1)
      PHP_ADD_LIBRARY(clntsh, 1, OCI_SHARED_LIBADD)
      PHP_ADD_LIBPATH($OCI_DIR/lib, OCI_SHARED_LIBADD)

      dnl 
      dnl OCI_ATTR_STATEMENT is not available in all 8.1.x versions
      dnl 
      PHP_OCI_IF_DEFINED(OCI_ATTR_STATEMENT, [AC_DEFINE(HAVE_OCI_ATTR_STATEMENT,1,[ ])], $OCI_INCLUDES)
      ;;

    9.0)
      PHP_ADD_LIBRARY(clntsh, 1, OCI_SHARED_LIBADD)
      PHP_ADD_LIBPATH($OCI_DIR/lib, OCI_SHARED_LIBADD)
      AC_DEFINE(HAVE_OCI_ATTR_STATEMENT,1,[ ])

      dnl These functions are only available in version >= 9.2
      PHP_CHECK_LIBRARY(clntsh, OCIEnvNlsCreate,
      [
        PHP_CHECK_LIBRARY(clntsh, OCINlsCharSetNameToId,
        [
          AC_DEFINE(HAVE_OCI_9_2,1,[ ])
          OCI_VERSION=9.2
        ], [], [
          -L$OCI_DIR/lib $OCI_SHARED_LIBADD
        ])
      ], [], [
        -L$OCI_DIR/lib $OCI_SHARED_LIBADD
      ])
      ;;
      
    10.1)
      PHP_ADD_LIBRARY(clntsh, 1, OCI_SHARED_LIBADD)
      PHP_ADD_LIBPATH($OCI_DIR/lib, OCI_SHARED_LIBADD)
      AC_DEFINE(HAVE_OCI_ATTR_STATEMENT,1,[ ])
      AC_DEFINE(HAVE_OCI_9_2,1,[ ])
      ;;
    *)
      AC_MSG_ERROR(Unsupported Oracle version!)
      ;;
  esac

  dnl
  dnl Check if we need to add -locijdbc8 
  dnl
  PHP_CHECK_LIBRARY(clntsh, OCILobIsTemporary,
  [
    AC_DEFINE(HAVE_OCI_TEMP_LOB,1,[ ])
  ], [
    PHP_CHECK_LIBRARY(ocijdbc8, OCILobIsTemporary,
    [
      PHP_ADD_LIBRARY(ocijdbc8, 1, OCI_SHARED_LIBADD)
      AC_DEFINE(HAVE_OCI_TEMP_LOB,1,[ ])
    ], [], [
      -L$OCI_DIR/lib $OCI_SHARED_LIBADD
    ])
  ], [
    -L$OCI_DIR/lib $OCI_SHARED_LIBADD
  ])

  dnl
  dnl Check if we have collections
  dnl
  PHP_CHECK_LIBRARY(clntsh, OCICollAssign,
  [
    AC_DEFINE(PHP_OCI_HAVE_COLLECTIONS,1,[ ])
  ], [], [
    -L$OCI_DIR/lib $OCI_SHARED_LIBADD
  ])


  PHP_NEW_EXTENSION(pdo_oci, pdo_oci.c oci_driver.c oci_statement.c, $ext_shared)
  AC_DEFINE(HAVE_OCI,1,[ ])

  PHP_SUBST_OLD(OCI_SHARED_LIBADD)
  PHP_SUBST_OLD(OCI_DIR)
  PHP_SUBST_OLD(OCI_VERSION)
  
fi
