dnl
dnl $Id$
dnl

AC_DEFUN(IMAP_INC_CHK,[if test -r "$i$1/c-client.h"; then
    AC_DEFINE(HAVE_IMAP2000, 1, [ ])
    IMAP_DIR=$i
    IMAP_INC_DIR=$i$1
  elif test -r "$i$1/rfc822.h"; then 
    IMAP_DIR=$i; 
    IMAP_INC_DIR=$i$1
])

AC_DEFUN(IMAP_LIB_CHK,[
  str="$IMAP_DIR/$1/lib$lib.*"
  for i in `echo $str`; do
    if test -r $i; then
      IMAP_LIBDIR=$IMAP_DIR/$1
      break 2
    fi
  done
])

dnl PHP_IMAP_TEST_BUILD(action-if-ok, action-if-not-ok [, extra-libs])
AC_DEFUN(PHP_IMAP_TEST_BUILD, [
  old_LIBS=$LIBS
  LIBS="$3 $LIBS"
  AC_TRY_RUN([
    void mm_log(void){}
    void mm_dlog(void){}
    void mm_flags(void){}
    void mm_fatal(void){}
    void mm_critical(void){}
    void mm_nocritical(void){}
    void mm_notify(void){}
    void mm_login(void){}
    void mm_diskerror(void){}
    void mm_status(void){}
    void mm_lsub(void){}
    void mm_list(void){}
    void mm_exists(void){}
    void mm_searched(void){}
    void mm_expunged(void){}
    char mail_open();
    int main() {
      mail_open(0,"",0);
      return 0;
    }
  ], [
    LIBS=$old_LIBS
    $1
  ],[
    LIBS=$old_LIBS
    $2
  ])
])

AC_DEFUN(PHP_IMAP_KRB_CHK, [
  AC_ARG_WITH(kerberos,
  [  --with-kerberos[=DIR]     IMAP: Include Kerberos support. DIR is the Kerberos install dir.],[
    PHP_KERBEROS=$withval
  ],[
    PHP_KERBEROS=no
  ])

  if test "$PHP_KERBEROS" != "no"; then

    if test "$PHP_KERBEROS" = "yes"; then
      SEARCH_PATHS="/usr/kerberos /usr/local /usr"
    else
      SEARCH_PATHS=$PHP_KERBEROS
    fi

    for i in $SEARCH_PATHS; do
      if test -f $i/lib/libkrb5.a || test -f $i/lib/libkrb5.$SHLIB_SUFFIX_NAME; then
        PHP_KERBEROS_DIR=$i
        break
      fi
    done

    if test -z "$PHP_KERBEROS_DIR"; then
      AC_MSG_ERROR([Kerberos libraries not found. 
      
      Check the path given to --with-kerberos (if no path is given, searches in /usr/kerberos, /usr/local and /usr )
      ])
    fi
    AC_DEFINE(HAVE_IMAP_KRB,1,[ ])
    PHP_ADD_LIBPATH($PHP_KERBEROS_DIR/lib, IMAP_SHARED_LIBADD)
    PHP_ADD_LIBRARY(gssapi_krb5, 1, IMAP_SHARED_LIBADD)
    PHP_ADD_LIBRARY(krb5, 1, IMAP_SHARED_LIBADD)
    PHP_ADD_LIBRARY(k5crypto, 1, IMAP_SHARED_LIBADD)
    PHP_ADD_LIBRARY(com_err,  1, IMAP_SHARED_LIBADD)
  else
    AC_EGREP_HEADER(auth_gss, $IMAP_INC_DIR/linkage.h, [
      AC_MSG_ERROR([This c-client library is built with Kerberos support. 

      Add --with-kerberos<=DIR> to your configure line. Check config.log for details.
      ])
    ])
  fi

])

AC_DEFUN(PHP_IMAP_SSL_CHK, [
  AC_ARG_WITH(imap-ssl,
  [  --with-imap-ssl[=DIR]     IMAP: Include SSL support. DIR is the OpenSSL install dir.],[
    PHP_IMAP_SSL=$withval
  ],[
    PHP_IMAP_SSL=no
  ])

  if test "$PHP_IMAP_SSL" = "yes"; then
    PHP_IMAP_SSL=/usr
  fi

  AC_MSG_CHECKING([whether SSL libraries are needed for c-client])

  if test "$PHP_IMAP_SSL" != "no"; then
    AC_MSG_RESULT([$PHP_IMAP_SSL/lib])
    AC_DEFINE(HAVE_IMAP_SSL,1,[ ])
    PHP_ADD_LIBRARY_DEFER(crypto,, IMAP_SHARED_LIBADD)
    PHP_ADD_LIBRARY_DEFER(ssl,, IMAP_SHARED_LIBADD)
    PHP_ADD_LIBPATH($PHP_IMAP_SSL/lib, IMAP_SHARED_LIBADD)
  else
    TST_LIBS="-L$IMAP_LIBDIR -l$IMAP_LIB"
    if test $PHP_KERBEROS != "no"; then
      TST_LIBS="$TST_LIBS -L$PHP_KERBEROS/lib -lgssapi_krb5 -lkrb5 -lk5crypto -lcom_err"
    fi

    PHP_IMAP_TEST_BUILD([
      AC_MSG_RESULT(no)
    ], [
      AC_MSG_RESULT(yes)
      AC_MSG_ERROR([This c-client library is built with SSL support. 
     
      Add --with-imap-ssl<=DIR> to your configure line. Check config.log for details.
      ])
    ], $TST_LIBS)
  fi
])


PHP_ARG_WITH(imap,for IMAP support,
[  --with-imap[=DIR]       Include IMAP support. DIR is the c-client install prefix.])

if test "$PHP_IMAP" != "no"; then  

    PHP_SUBST(IMAP_SHARED_LIBADD)
    PHP_NEW_EXTENSION(imap, php_imap.c, $ext_shared)
    AC_DEFINE(HAVE_IMAP,1,[ ])

    for i in /usr/local /usr $PHP_IMAP; do
      IMAP_INC_CHK()
      el[]IMAP_INC_CHK(/include/c-client)
      el[]IMAP_INC_CHK(/include/imap)
      el[]IMAP_INC_CHK(/include)
      el[]IMAP_INC_CHK(/imap)
      el[]IMAP_INC_CHK(/c-client)
      fi
    done

    old_CPPFLAGS=$CPPFLAGS
    CPPFLAGS=-I$IMAP_INC_DIR
    AC_EGREP_CPP(this_is_true, [
#include "imap4r1.h"
#if defined(IMAPSSLPORT)
      this_is_true
#endif
    ],[
      AC_DEFINE(HAVE_IMAP2001, 1, [ ])
    ],[])
    CPPFLAGS=$old_CPPFLAGS

    AC_CHECK_LIB(pam, pam_start) 
    AC_CHECK_LIB(crypt, crypt)
	    
    PHP_EXPAND_PATH($IMAP_DIR, IMAP_DIR)

    if test -z "$IMAP_DIR"; then
      AC_MSG_ERROR(Cannot find rfc822.h. Please check your IMAP installation.)
    fi

    if test -r "$IMAP_DIR/c-client/c-client.a"; then
      ln -s "$IMAP_DIR/c-client/c-client.a" "$IMAP_DIR/c-client/libc-client.a" >/dev/null 2>&1
    elif test -r "$IMAP_DIR/lib/c-client.a"; then
      ln -s "$IMAP_DIR/lib/c-client.a" "$IMAP_DIR/lib/libc-client.a" >/dev/null 2>&1
    fi

    for lib in c-client4 c-client imap; do
      IMAP_LIB=$lib
      IMAP_LIB_CHK(lib)
      IMAP_LIB_CHK(c-client)
    done

    if test -z "$IMAP_LIBDIR"; then
      AC_MSG_ERROR(Cannot find imap library (libc-client.a). Please check your IMAP installation.)
    fi

    PHP_ADD_INCLUDE($IMAP_INC_DIR)
    PHP_IMAP_KRB_CHK
    PHP_IMAP_SSL_CHK
    PHP_ADD_LIBRARY_DEFER($IMAP_LIB,, IMAP_SHARED_LIBADD)
    PHP_ADD_LIBPATH($IMAP_LIBDIR, IMAP_SHARED_LIBADD)

    dnl Test the build in the end
    TST_LIBS="$DLIBS $IMAP_SHARED_LIBADD"
    AC_MSG_CHECKING(whether IMAP works)
    PHP_IMAP_TEST_BUILD([
      AC_MSG_RESULT(yes)
    ], [
      AC_MSG_RESULT(no)
      AC_MSG_ERROR([build test failed. Please check the config.log for details.])
    ], $TST_LIBS)
fi
