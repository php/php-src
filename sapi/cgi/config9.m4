dnl
dnl $Id$
dnl

AC_ARG_ENABLE(cgi,
[  --disable-cgi           Disable building CGI version of PHP],
[
  PHP_SAPI_CGI=$enableval
],[
  PHP_SAPI_CGI=yes
])

AC_ARG_ENABLE(force-cgi-redirect,
[  --enable-force-cgi-redirect
                           Enable the security check for internal server
                           redirects.  You should use this if you are
                           running the CGI version with Apache.],
[
  PHP_FORCE_CGI_REDIRECT=$enableval
],[
  PHP_FORCE_CGI_REDIRECT=no
])

AC_ARG_ENABLE(discard-path,
[  --enable-discard-path   If this is enabled, the PHP CGI binary
                          can safely be placed outside of the
                          web tree and people will not be able
                          to circumvent .htaccess security.],
[
  PHP_DISCARD_PATH=$enableval
],[
  PHP_DISCARD_PATH=no
])


AC_DEFUN(PHP_TEST_WRITE_STDOUT,[
  AC_CACHE_CHECK(whether writing to stdout works,ac_cv_write_stdout,[
    AC_TRY_RUN([
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#define TEXT "This is the test message -- "
        
main()
{
  int n;

  n = write(1, TEXT, sizeof(TEXT)-1);
  return (!(n == sizeof(TEXT)-1));
}
    ],[
      ac_cv_write_stdout=yes
    ],[
      ac_cv_write_stdout=no
    ],[
      ac_cv_write_stdout=no
    ])
  ])
  if test "$ac_cv_write_stdout" = "yes"; then
    AC_DEFINE(PHP_WRITE_STDOUT, 1, [whether write(2) works])
  fi
])


if test "$PHP_SAPI" = "default"; then
  AC_MSG_CHECKING(for CGI build)
  if test "$PHP_SAPI_CGI" != "no"; then
    AC_MSG_RESULT(yes)

    PHP_ADD_MAKEFILE_FRAGMENT($abs_srcdir/sapi/cgi/Makefile.frag)
    SAPI_CGI_PATH=sapi/cgi/php-cgi
    PHP_SUBST(SAPI_CGI_PATH)

    PHP_TEST_WRITE_STDOUT

    AC_MSG_CHECKING(whether to force Apache CGI redirect)
    if test "$PHP_FORCE_CGI_REDIRECT" = "yes"; then
      REDIRECT=1
    else
      REDIRECT=0
    fi
    AC_DEFINE_UNQUOTED(FORCE_CGI_REDIRECT,$REDIRECT,[ ])
    AC_MSG_RESULT($PHP_FORCE_CGI_REDIRECT)


    AC_MSG_CHECKING(whether to discard path_info + path_translated)
    if test "$PHP_DISCARD_PATH" = "yes"; then
      DISCARD_PATH=1
    else
      DISCARD_PATH=0
    fi
    AC_DEFINE_UNQUOTED(DISCARD_PATH, $DISCARD_PATH, [ ])
    AC_MSG_RESULT($PHP_DISCARD_PATH)


    INSTALL_IT="\$(INSTALL) -m 0755 \$(SAPI_CGI_PATH) \$(INSTALL_ROOT)\$(bindir)/php-cgi"
    PHP_SELECT_SAPI(cgi, program, cgi_main.c getopt.c,,'$(SAPI_CGI_PATH)')

    case $host_alias in
      *darwin*)
        BUILD_CGI="\$(CC) \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS) \$(LDFLAGS) \$(NATIVE_RPATHS) \$(PHP_GLOBAL_OBJS:.lo=.o) \$(PHP_SAPI_OBJS:.lo=.o) \$(PHP_FRAMEWORKS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CGI_PATH)"
      ;;
      *)
        BUILD_CGI="\$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS) \$(LDFLAGS) \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS) \$(PHP_SAPI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CGI_PATH)"
      ;;
    esac

    PHP_SUBST(BUILD_CGI)

  elif test "$PHP_SAPI_CLI" != "no"; then
    AC_MSG_RESULT(no)
    OVERALL_TARGET=
    PHP_SAPI=cli   
  else
    AC_MSG_ERROR([No SAPIs selected.])  
  fi
fi
