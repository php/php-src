dnl $Id$

AC_DEFUN(AC_ORACLE_VERSION,[
  AC_MSG_CHECKING([Oracle version])
  if test -f "$ORACLEINST_TOP/orainst/unix.rgs"
  then
	changequote({,})
	ORACLE_VERSION=`grep '"ocommon"' $ORACLEINST_TOP/orainst/unix.rgs | sed 's/[ ][ ]*/:/g' | cut -d: -f 6 | cut -c 2-4`
	changequote([,])
    test -z "$ORACLE_VERSION" && ORACLE_VERSION=7.3
  else
    if test -f "$ORACLEINST_TOP/lib/libclntsh.so.8.0"
    then
	ORACLE_VERSION=8.1
    else
	ORACLE_VERSION=8.0
    fi
  fi
  AC_MSG_RESULT($ORACLE_VERSION)
])

AC_MSG_CHECKING(for Oracle-OCI8 support)
AC_ARG_WITH(oci8,
[  --with-oci8[=DIR]       Include Oracle database support.  DIR is Oracle's
                          home directory, defaults to \$ORACLE_HOME.],
[
 case "$withval" in
    yes)
      ORACLEINST_TOP=$ORACLE_HOME
      AC_MSG_RESULT(yes)
      PHP_EXTENSION(oci8)
      ;;
    no)
      ORACLEINST_TOP=
      AC_MSG_RESULT(no)
      ;;
    *)
      AC_EXPAND_PATH($withval, ORACLEINST_TOP)
      AC_MSG_RESULT(yes)
      PHP_EXTENSION(oci8)
      ;;
  esac

  if test "$ORACLEINST_TOP" != ""
  then

    # Oracle include files

    if test -f "$ORACLEINST_TOP/rdbms/public/ocidfn.h"
    then
      # V8.0.5
      ORACLE_INCLUDE="$ORACLE_INCLUDE -I$ORACLEINST_TOP/rdbms/public"
	elif test -f "$ORACLEINST_TOP/rdbms/demo/ocidfn.h"
    then
      # V7.[0123]
      ORACLE_INCLUDE=-I$ORACLEINST_TOP/rdbms/demo
    fi

    if test -d "$ORACLEINST_TOP/rdbms/public"
    then
      ORACLE_INCLUDE="$ORACLE_INCLUDE -I$ORACLEINST_TOP/rdbms/public"
    fi

    if test -d "$ORACLEINST_TOP/network/public"
    then
      ORACLE_INCLUDE="$ORACLE_INCLUDE -I$ORACLEINST_TOP/network/public"
    fi

    if test -d "$ORACLEINST_TOP/plsql/public"
    then
      # V8
      ORACLE_INCLUDE="$ORACLE_INCLUDE -I$ORACLEINST_TOP/plsql/public"
    fi

    # Need to know the version, otherwhise we will mixup nlsrtl
    AC_ORACLE_VERSION($ORACLEINST_TOP)

    # Oracle libs - nightmare :-)

    ORACLE_LIBDIR=lib
    ORACLE_LFLAGS="-L$ORACLEINST_TOP/$ORACLE_LIBDIR ${ld_runpath_switch}$ORACLEINST_TOP/$ORACLE_LIBDIR"
    if test -f "$ORACLEINST_TOP/rdbms/lib/sysliblist"
    then
      ORA_SYSLIB="`cat $ORACLEINST_TOP/rdbms/lib/sysliblist`"
    else
      ORA_SYSLIB="-lm"
    fi

    # Oracle Static libs
    case $ORACLE_VERSION in
      7.0|7.1)
  	ORACLE_STLIBS="-locic $ORACLEINST_TOP/$ORACLE_LIBDIR/osntab.o \
            -lsqlnet -lora -lsqlnet -lnlsrtl -lcv6 -lcore -lnlsrtl -lcv6 \
            -lcore $ORA_SYSLIB -lcore $ORA_SYSLIB"
    if test "`uname -s 2>/dev/null`" = "AIX"; then
      ORACLE_STLIBS="$ORACLE_STLIBS -bI:$ORACLE_HOME/lib/mili.exp"
    fi
  	;;
      7.2)
  	ORACLE_STLIBS="-locic $ORACLEINST_TOP/$ORACLE_LIBDIR/osntab.o \
            -lsqlnet -lora -lsqlnet -lora -lnlsrtl3 -lc3v6 -lcore3 -lnlsrtl3 \
            -lcore3 $ORA_SYSLIB -lcore3 $ORA_SYSLIB"
  	;;
      7.3)
  	ORACLE_STLIBS="-lclient -lsqlnet -lncr -lsqlnet -lclient -lcommon \
  	    -lgeneric -lsqlnet -lncr -lsqlnet -lclient -lcommon -lgeneric \
            -lepc -lnlsrtl3 -lc3v6 -lcore3 -lnlsrtl3 -lcore3 -lnlsrtl3 \
            $ORA_SYSLIB -lcore3 $ORA_SYSLIB"
  	;;
      8.0)
  	ORACLE_STLIBS="-lclient -lsqlnet -lncr -lsqlnet -lclient -lcommon \
  	    -lgeneric -lsqlnet -lncr -lsqlnet -lclient -lcommon -lgeneric \
            -lepc -lnlsrtl3 -lc3v6 -lcore4 -lnlsrtl3 -lcore4 -lnlsrtl3 \
            $ORA_SYSLIB -lcore3 $ORA_SYSLIB"
	;;
      *)
  	ORACLE_STLIBS=
  	;;
    esac
  
    # Oracle shared libs
    case $ORACLE_VERSION in
      7.0)
  	# shared libs not supported
  	ORACLE_SHLIBS="$ORACLE_STLIBS"
  	;;
      7.1)
  	if test -f $ORACLEINST_TOP/$ORACLE_LIBDIR/liboracle.s?
  	then
  	  ORACLE_SHLIBS="-loracle $ORA_SYSLIB"
  	else
  	  ORACLE_SHLIBS="$ORACLE_STLIBS"
  	fi
  	;;
      7.2|7.3)
    	if test -f $ORACLEINST_TOP/$ORACLE_LIBDIR/libclntsh.s?
    	then
    	  ORACLE_SHLIBS="-lclntsh $ORA_SYSLIB"
    	else
    	  ORACLE_SHLIBS="$ORACLE_STLIBS"
    	fi
    	;;
      8.0)
	if test -f $ORACLEINST_TOP/$ORACLE_LIBDIR/libclntsh.s? -o \
	        -f $ORACLEINST_TOP/$ORACLE_LIBDIR/libclntsh.a # AIX
    	then
	  if test "$CC" = "gcc" -a "`uname -sv`" = "AIX 4"; then
	    # for Oracle 8 on AIX 4
	    ORA_SYSLIB="$ORA_SYSLIB -nostdlib /lib/crt0_r.o /usr/lib/libpthreads.a /usr/lib/libc_r.a -lgcc"
	  fi
    	  ORACLE_SHLIBS="-lclntsh -lpsa -lcore4 -lnlsrtl3 -lclntsh $ORA_SYSLIB"
    	else
    	  ORACLE_SHLIBS="$ORACLE_STLIBS"
    	fi
        AC_DEFINE(HAVE_OCI8,1,[ ])
    	;;
      8.1)
	ORACLE_SHLIBS="-lclntsh $ORA_SYSLIB"
	AC_DEFINE(HAVE_OCI8,1,[ ])
	;;
      *)
  	ORACLE_SHLIBS=
  	;;
    esac
  
    # only using shared libs right now
    ORACLE_LIBS=$ORACLE_SHLIBS
  fi

],[AC_MSG_RESULT(no)])
#EXTRA_LIBS="$EXTRA_LIBS $ORACLE_SHLIBS $ORACLE_STLIBS $ORACLE_LIBS $ORACLE_LFLAGS"
EXTRA_LIBS="$EXTRA_LIBS $ORACLE_LFLAGS $ORACLE_LIBS"
INCLUDES="$INCLUDES $ORACLE_INCLUDE"
PHP_SUBST(ORACLE_HOME)
PHP_SUBST(ORACLE_VERSION)
