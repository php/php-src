dnl $Id$

AC_DEFUN(AC_OCI8_VERSION,[
  AC_MSG_CHECKING([Oracle version])
  if test -f "$OCI8_DIR/orainst/unix.rgs"; then
	OCI8_VERSION=`grep '"ocommon"' $OCI8_DIR/orainst/unix.rgs | sed 's/[ ][ ]*/:/g' | cut -d: -f 6 | cut -c 2-4`
	test -z "$OCI8_VERSION" && OCI8_VERSION=7.3
  elif test -f $OCI8_DIR/lib/libclntsh.s?.8.0; then
	OCI8_VERSION=8.1
  elif test -f $OCI8_DIR/lib/libclntsh.s?.1.0; then
	OCI8_VERSION=8.0
  elif test -f $OCI8_DIR/lib/libclntsh.a; then # AIX - XXX is this check still right for 8.1?
	OCI8_VERSION=8.0
  else
    AC_MSG_ERROR(Oracle-OCI8 needed libraries not found)
  fi
  AC_MSG_RESULT($OCI8_VERSION)
])                                                                                                                                                                

PHP_ARG_WITH(oci8, for Oracle-OCI8 support,
[  --with-oci8[=DIR]     Include oci8 support. DIR is the ORACLE_HOME.])

if test "$PHP_OCI8" != "no"; then
  AC_MSG_CHECKING([Oracle Install-Dir])
  if test "$PHP_OCI8" = "yes"; then
  	OCI8_DIR="$ORACLE_HOME"
  else
  	OCI8_DIR="$PHP_OCI8"
  fi
  AC_MSG_RESULT($OCI8_DIR)

  if test "$PHP_SIGCHILD" != "yes"; then
	echo "+--------------------------------------------------------------------+"
	echo "| Notice:                                                            |"
	echo "| If you encounter <defunc> processes when using a local Oracle-DB   |"
	echo "| please recompile PHP and specify --enable-sigchild when configuring|"
	echo "| (This problem has been reported un Linux using Oracle >= 8.1.5)    |"
	echo "+--------------------------------------------------------------------+"
  fi

  if test -d "$OCI8_DIR/rdbms/public"; then
  	AC_ADD_INCLUDE($OCI8_DIR/rdbms/public)
  fi
  if test -d "$OCI8_DIR/rdbms/demo"; then
  	AC_ADD_INCLUDE($OCI8_DIR/rdbms/demo)
  fi
  if test -d "$OCI8_DIR/network/public"; then
  	AC_ADD_INCLUDE($OCI8_DIR/network/public)
  fi
  if test -d "$OCI8_DIR/plsql/public"; then
  	AC_ADD_INCLUDE($OCI8_DIR/plsql/public)
  fi

  if test -f "$OCI8_DIR/lib/sysliblist"; then
  	PHP_EVAL_LIBLINE(`$OCI8_DIR/lib/sysliblist`, OCI8_SYSLIB)
  elif test -f "$OCI8_DIR/rdbms/lib/sysliblist"; then
  	PHP_EVAL_LIBLINE(`$OCI8_DIR/rdbms/lib/sysliblist`, OCI8_SYSLIB)
  fi

  AC_OCI8_VERSION($OCI8_DIR)
  case $OCI8_VERSION in
	8.0)
  	  AC_ADD_LIBRARY_WITH_PATH(nlsrtl3, "", OCI8_SHARED_LIBADD)
  	  AC_ADD_LIBRARY_WITH_PATH(core4, "", OCI8_SHARED_LIBADD)
  	  AC_ADD_LIBRARY_WITH_PATH(psa, "", OCI8_SHARED_LIBADD)
  	  AC_ADD_LIBRARY_WITH_PATH(clntsh, $OCI8_DIR/lib, OCI8_SHARED_LIBADD)
	  ;;

	8.1)
  	  AC_ADD_LIBRARY_WITH_PATH(clntsh, $OCI8_DIR/lib, OCI8_SHARED_LIBADD)
	  ;;
	*)
      AC_MSG_ERROR(Unsupported Oracle version!)
	  ;;
  esac

  PHP_EXTENSION(oci8, $ext_shared)
  AC_DEFINE(HAVE_OCI8,1,[ ])

  PHP_SUBST(OCI8_SHARED_LIBADD)
  PHP_SUBST(OCI8_DIR)
  PHP_SUBST(OCI8_VERSION)
fi
