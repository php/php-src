dnl $Id$

AC_DEFUN(AC_OCI8_VERSION,[
  AC_MSG_CHECKING([Oracle version])
  if test -f "$OCI8_HOME/lib/libclntsh.so.8.0"; then
    OCI8_VERSION=8.1
  elif test -f "$OCI8_HOME/lib/libclntsh.so.1.0"; then
    OCI8_VERSION=8.0
  else
  	AC_MSG_ERROR(Oracle-OCI8 needed libraries not found)
  fi
  AC_MSG_RESULT($OCI8_VERSION)
])

AC_MSG_CHECKING(for Oracle-OCI8 support)
AC_ARG_WITH(oci8,
[  --with-oci8[=DIR]      Include Oracle-OCI8 database support. DIR is Oracle's
                          home directory, defaults to \$ORACLE_HOME.],
[
  case $withval in
  shared)
 	shared=yes
    withval=yes
    OCI8_HOME=$ORACLE_HOME
    AC_MSG_RESULT(yes)
    PHP_EXTENSION(oci8,yes)
    ;;
  shared,*)
   	shared=yes
   	withval=`echo $withval | sed -e 's/^shared,//'`
   	AC_EXPAND_PATH($withval, OCI8_HOME)
   	AC_MSG_RESULT(yes)
    PHP_EXTENSION(oci8,yes)
  	;;
  *)
  	shared=no
   	OCI8_HOME=$ORACLE_HOME
   	AC_MSG_RESULT(yes)
    PHP_EXTENSION(oci8,no)
   	;;
  esac

  if test "$OCI8_HOME" != ""; then
    if test -d "$OCI8_HOME/rdbms/public"; then
      OCI8_INCLUDE="$OCI8_INCLUDE -I$OCI8_HOME/rdbms/public"
    fi
	if test -d "$OCI8_HOME/rdbms/demo"; then
      OCI8_INCLUDE="$OCI8_INCLUDE -I$OCI8_HOME/rdbms/demo"
    fi
    if test -d "$OCI8_HOME/network/public"; then
      OCI8_INCLUDE="$OCI8_INCLUDE -I$OCI8_HOME/network/public"
    fi
    if test -d "$OCI8_HOME/plsql/public"; then
      OCI8_INCLUDE="$OCI8_INCLUDE -I$OCI8_HOME/plsql/public"
    fi

    # Need to know the version, otherwhise we will mixup nlsrtl
    AC_OCI8_VERSION($OCI8_HOME)

    OCI8_LIBDIR=lib
    OCI8_LFLAGS="-L$OCI8_HOME/$OCI8_LIBDIR ${ld_runpath_switch}$OCI8INST_TOP/$OCI8_LIBDIR"
    if test -f "$OCI8_HOME/rdbms/lib/sysliblist"; then
      ORA_SYSLIB="`cat $OCI8_HOME/rdbms/lib/sysliblist`"
    else
      ORA_SYSLIB="-lm"
    fi

    # Oracle shared libs
    case $OCI8_VERSION in
      8.0)
		if test -f $OCI8_HOME/$OCI8_LIBDIR/libclntsh.s? -o \
	        	-f $OCI8_HOME/$OCI8_LIBDIR/libclntsh.a # AIX
    	then
	  	  if test "$CC" = "gcc" -a "`uname -sv`" = "AIX 4"; then
	        # for Oracle 8 on AIX 4
	        ORA_SYSLIB="$ORA_SYSLIB -nostdlib /lib/crt0_r.o /usr/lib/libpthreads.a /usr/lib/libc_r.a -lgcc"
	      fi

    	  OCI8_SHLIBS="-lclntsh -lpsa -lcore4 -lnlsrtl3 -lclntsh $ORA_SYSLIB"
    	else
    	  OCI8_SHLIBS="$OCI8_STLIBS"
    	fi
        AC_DEFINE(HAVE_OCI8,1,[ ])
    	;;

      8.1)
	    OCI8_SHLIBS="-lclntsh $ORA_SYSLIB"
		AC_DEFINE(HAVE_OCI8,1,[ ])
		;;
      *)
  		OCI8_SHLIBS=
  		;;
    esac
  
    # only using shared libs right now
    OCI8_LIBS=$OCI8_SHLIBS
  fi
],[AC_MSG_RESULT(no)])

if test "$shared" = yes; then
	OCI8_LIBS="$OCI8_LFLAGS $OCI8_LIBS"
else
	EXTRA_LIBS="$EXTRA_LIBS $OCI8_LFLAGS $OCI8_LIBS"
	INCLUDES="$INCLUDES $OCI8_INCLUDE"
fi

INCLUDES="$INCLUDES $OCI8_INCLUDE"

PHP_SUBST(OCI8_HOME) 
PHP_SUBST(OCI8_INCLUDE) 
PHP_SUBST(OCI8_LFLAGS) 
PHP_SUBST(OCI8_LIBS)

