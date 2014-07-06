dnl
dnl $Id$
dnl

AC_DEFUN([IMAP_INC_CHK],[if test -r "$i$1/c-client.h"; then
    AC_DEFINE(HAVE_IMAP2000, 1, [ ])
    IMAP_DIR=$i
    IMAP_INC_DIR=$i$1
    break
  elif test -r "$i$1/rfc822.h"; then 
    IMAP_DIR=$i; 
    IMAP_INC_DIR=$i$1
	break
])

AC_DEFUN([IMAP_LIB_CHK],[
  str="$IMAP_DIR/$1/lib$lib.*"
  for i in `echo $str`; do
    test -r $i && IMAP_LIBDIR=$IMAP_DIR/$1 && break 2
  done
])

dnl PHP_IMAP_TEST_BUILD(function, action-if-ok, action-if-not-ok, extra-libs)
AC_DEFUN([PHP_IMAP_TEST_BUILD], [
  PHP_TEST_BUILD([$1], [$2], [$3], [$4],
  [
#if defined(__GNUC__) && __GNUC__ >= 4
# define PHP_IMAP_EXPORT __attribute__ ((visibility("default")))
#else
# define PHP_IMAP_EXPORT
#endif

    PHP_IMAP_EXPORT void mm_log(void){}
    PHP_IMAP_EXPORT void mm_dlog(void){}
    PHP_IMAP_EXPORT void mm_flags(void){}
    PHP_IMAP_EXPORT void mm_fatal(void){}
    PHP_IMAP_EXPORT void mm_critical(void){}
    PHP_IMAP_EXPORT void mm_nocritical(void){}
    PHP_IMAP_EXPORT void mm_notify(void){}
    PHP_IMAP_EXPORT void mm_login(void){}
    PHP_IMAP_EXPORT void mm_diskerror(void){}
    PHP_IMAP_EXPORT void mm_status(void){}
    PHP_IMAP_EXPORT void mm_lsub(void){}
    PHP_IMAP_EXPORT void mm_list(void){}
    PHP_IMAP_EXPORT void mm_exists(void){}
    PHP_IMAP_EXPORT void mm_searched(void){}
    PHP_IMAP_EXPORT void mm_expunged(void){}
  ])
])

AC_DEFUN([PHP_IMAP_KRB_CHK], [
  if test "$PHP_KERBEROS" != "no"; then
    PHP_SETUP_KERBEROS(IMAP_SHARED_LIBADD,
    [
      AC_DEFINE(HAVE_IMAP_KRB,1,[ ])
    ], [
      AC_MSG_ERROR([Kerberos libraries not found. 
      
      Check the path given to --with-kerberos (if no path is given, searches in /usr/kerberos, /usr/local and /usr )
      ])
    ])
  else
    AC_EGREP_HEADER(auth_gss, $IMAP_INC_DIR/linkage.h, [
      AC_MSG_ERROR([This c-client library is built with Kerberos support. 

      Add --with-kerberos to your configure line. Check config.log for details.
      ])
    ])
  fi
])

AC_DEFUN([PHP_IMAP_SSL_CHK], [
  if test "$PHP_IMAP_SSL" != "no"; then
    if test "$PHP_OPENSSL" = ""; then
      PHP_OPENSSL='no'
    fi
    PHP_SETUP_OPENSSL(IMAP_SHARED_LIBADD,
    [
      AC_DEFINE(HAVE_IMAP_SSL,1,[ ])
    ], [
      AC_MSG_ERROR([OpenSSL libraries not found. 
      
      Check the path given to --with-openssl-dir and output in config.log)
      ])
    ])
  elif test -f "$IMAP_INC_DIR/linkage.c"; then
    AC_EGREP_HEADER(ssl_onceonlyinit, $IMAP_INC_DIR/linkage.c, [
      AC_MSG_ERROR([This c-client library is built with SSL support. 

      Add --with-imap-ssl to your configure line. Check config.log for details.
      ])
    ])
  fi
])


PHP_ARG_WITH(imap,for IMAP support,
[  --with-imap[=DIR]         Include IMAP support. DIR is the c-client install prefix])

PHP_ARG_WITH(kerberos,for IMAP Kerberos support,
[  --with-kerberos[=DIR]     IMAP: Include Kerberos support. DIR is the Kerberos install prefix], no, no)

PHP_ARG_WITH(imap-ssl,for IMAP SSL support,
[  --with-imap-ssl[=DIR]     IMAP: Include SSL support. DIR is the OpenSSL install prefix], no, no)


if test "$PHP_IMAP" != "no"; then  
    PHP_SUBST(IMAP_SHARED_LIBADD)
    PHP_NEW_EXTENSION(imap, php_imap.c, $ext_shared)
    AC_DEFINE(HAVE_IMAP,1,[ ])

    for i in $PHP_IMAP /usr/local /usr; do
      IMAP_INC_CHK()
      el[]IMAP_INC_CHK(/include/c-client)
      el[]IMAP_INC_CHK(/include/imap)
      el[]IMAP_INC_CHK(/include)
      el[]IMAP_INC_CHK(/imap)
      el[]IMAP_INC_CHK(/c-client)
      fi
    done

    dnl Check for c-client version 2004
    AC_EGREP_HEADER(mail_fetch_overview_sequence, $IMAP_INC_DIR/mail.h, [
      AC_DEFINE(HAVE_IMAP2004,1,[ ])
    ])

    dnl Check for new version of the utf8_mime2text() function
    old_CFLAGS=$CFLAGS
    CFLAGS="-I$IMAP_INC_DIR"
    AC_CACHE_CHECK(for utf8_mime2text signature, ac_cv_utf8_mime2text,
      AC_TRY_COMPILE([
#include <stdio.h>
#include <c-client.h>
      ],[
        SIZEDTEXT *src, *dst;
        utf8_mime2text(src, dst);
      ],[
        ac_cv_utf8_mime2text=old
      ],[
        ac_cv_utf8_mime2text=new
      ])
    )
    if test "$ac_cv_utf8_mime2text" = "new"; then
      AC_DEFINE(HAVE_NEW_MIME2TEXT, 1, [Whether utf8_mime2text() has new signature])
    fi
    CFLAGS=$old_CFLAGS

    old_CFLAGS=$CFLAGS
    CFLAGS="-I$IMAP_INC_DIR"
    AC_CACHE_CHECK(for U8T_DECOMPOSE, ac_cv_u8t_canonical,
      AC_TRY_COMPILE([
#include <c-client.h>
      ],[
         int i = U8T_CANONICAL;
      ],[
         ac_cv_u8t_decompose=yes
      ],[
         ac_cv_u8t_decompose=no
      ])
    )
    CFLAGS=$old_CFLAGS

    if test "$ac_cv_u8t_decompose" = "no" && test "$ac_cv_utf8_mime2text" = "new"; then
		AC_MSG_ERROR([utf8_mime2text() has new signature, but U8T_CANONICAL is missing. This should not happen. Check config.log for additional information.])
    fi
    if test "$ac_cv_u8t_decompose" = "yes" && test "$ac_cv_utf8_mime2text" = "old"; then
		AC_MSG_ERROR([utf8_mime2text() has old signature, but U8T_CANONICAL is present. This should not happen. Check config.log for additional information.])
    fi

    dnl Check for c-client version 2001
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

    PHP_CHECK_LIBRARY(pam, pam_start, 
    [
      PHP_ADD_LIBRARY(pam,, IMAP_SHARED_LIBADD)
      AC_DEFINE(HAVE_LIBPAM,1,[ ])
    ])

    PHP_CHECK_LIBRARY(crypt, crypt, 
    [
      PHP_ADD_LIBRARY(crypt,, IMAP_SHARED_LIBADD)
      AC_DEFINE(HAVE_LIBCRYPT,1,[ ])
    ])
	    
    PHP_EXPAND_PATH($IMAP_DIR, IMAP_DIR)

    if test -z "$IMAP_DIR"; then
      AC_MSG_ERROR(Cannot find rfc822.h. Please check your c-client installation.)
    fi

    if test ! -r "$IMAP_DIR/c-client/libc-client.a" && test -r "$IMAP_DIR/c-client/c-client.a" ; then
      ln -s "$IMAP_DIR/c-client/c-client.a" "$IMAP_DIR/c-client/libc-client.a" >/dev/null 2>&1
    elif test ! -r "$IMAP_DIR/$PHP_LIBDIR/libc-client.a" && test -r "$IMAP_DIR/$PHP_LIBDIR/c-client.a"; then
      ln -s "$IMAP_DIR/$PHP_LIBDIR/c-client.a" "$IMAP_DIR/$PHP_LIBDIR/libc-client.a" >/dev/null 2>&1
    fi

    for lib in c-client4 c-client imap; do
      IMAP_LIB=$lib
      IMAP_LIB_CHK($PHP_LIBDIR)
      IMAP_LIB_CHK(c-client)
    done

    if test -z "$IMAP_LIBDIR"; then
      AC_MSG_ERROR(Cannot find imap library (libc-client.a). Please check your c-client installation.)
    fi

    PHP_ADD_INCLUDE($IMAP_INC_DIR)
    PHP_ADD_LIBRARY_DEFER($IMAP_LIB,, IMAP_SHARED_LIBADD)
    PHP_ADD_LIBPATH($IMAP_LIBDIR, IMAP_SHARED_LIBADD)
    PHP_IMAP_KRB_CHK
    PHP_IMAP_SSL_CHK

    dnl Test the build in the end
    TST_LIBS="$DLIBS $IMAP_SHARED_LIBADD"

    dnl Check if auth_gss exists
    PHP_IMAP_TEST_BUILD(auth_gssapi_valid, [
      AC_DEFINE(HAVE_IMAP_AUTH_GSS, 1, [ ])
    ], [], $TST_LIBS)

    dnl Check if utf8_to_mutf7 exists
    PHP_IMAP_TEST_BUILD(utf8_to_mutf7, [
      AC_DEFINE(HAVE_IMAP_MUTF7, 1, [ ])
    ], [], $TST_LIBS)

    AC_MSG_CHECKING(whether rfc822_output_address_list function present)
    PHP_TEST_BUILD(foobar, [
      AC_MSG_RESULT(yes)
      AC_DEFINE(HAVE_RFC822_OUTPUT_ADDRESS_LIST, 1, [ ])
    ], [
      AC_MSG_RESULT(no)
	], [
      $TST_LIBS
    ], [
#if defined(__GNUC__) && __GNUC__ >= 4
# define PHP_IMAP_EXPORT __attribute__ ((visibility("default")))
#else
# define PHP_IMAP_EXPORT
#endif

      PHP_IMAP_EXPORT void mm_log(void){}
      PHP_IMAP_EXPORT void mm_dlog(void){}
      PHP_IMAP_EXPORT void mm_flags(void){}
      PHP_IMAP_EXPORT void mm_fatal(void){}
      PHP_IMAP_EXPORT void mm_critical(void){}
      PHP_IMAP_EXPORT void mm_nocritical(void){}
      PHP_IMAP_EXPORT void mm_notify(void){}
      PHP_IMAP_EXPORT void mm_login(void){}
      PHP_IMAP_EXPORT void mm_diskerror(void){}
      PHP_IMAP_EXPORT void mm_status(void){}
      PHP_IMAP_EXPORT void mm_lsub(void){}
      PHP_IMAP_EXPORT void mm_list(void){}
      PHP_IMAP_EXPORT void mm_exists(void){}
      PHP_IMAP_EXPORT void mm_searched(void){}
      PHP_IMAP_EXPORT void mm_expunged(void){}
      void rfc822_output_address_list(void);
      void (*f)(void);
      char foobar () {f = rfc822_output_address_list;}
    ])

    AC_MSG_CHECKING(whether build with IMAP works)
    PHP_IMAP_TEST_BUILD(mail_newbody, [
      AC_MSG_RESULT(yes)
    ], [
      AC_MSG_RESULT(no)
      AC_MSG_ERROR([build test failed. Please check the config.log for details.])
    ], $TST_LIBS)
fi
