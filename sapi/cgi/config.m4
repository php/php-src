dnl
dnl $Id$
dnl

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

  PHP_TEST_WRITE_STDOUT

  PHP_ARG_ENABLE(force-cgi-redirect,whether to force Apache CGI redirect,
[  --enable-force-cgi-redirect
                          Enable the security check for internal server
                          redirects.  You should use this if you are
                          running the CGI version with Apache. ])

  if test "$PHP_FORCE_CGI_REDIRECT" = "yes"; then
    REDIRECT=1
  else
    REDIRECT=0
  fi
  AC_DEFINE_UNQUOTED(FORCE_CGI_REDIRECT,$REDIRECT,[ ])

  PHP_ARG_ENABLE(discard-path,whether to discard path_info + path_translated,
[  --enable-discard-path   If this is enabled, the PHP CGI binary
                          can safely be placed outside of the
                          web tree and people will not be able
                          to circumvent .htaccess security. ])

  if test "$PHP_DISCARD_PATH" = "yes"; then
    DISCARD_PATH=1
  else
    DISCARD_PATH=0
  fi
  AC_DEFINE_UNQUOTED(DISCARD_PATH, $DISCARD_PATH, [ ])

  INSTALL_IT="\$(INSTALL) -m 0755 $SAPI_PROGRAM \$(INSTALL_ROOT)\$(bindir)/php-cgi"
fi

AC_MSG_CHECKING(for fhttpd module support)
AC_ARG_WITH(fhttpd,
[  --with-fhttpd[=DIR]     Build fhttpd module.  DIR is the fhttpd sources
                          directory, defaults to /usr/local/src/fhttpd.],
[
    if test "$withval" = "yes"; then
      # fhttpd source directory
      withval=/usr/local/src/fhttpd
    fi
    if test "$withval" != "no"; then
# For fhttpd 0.3.x
      if test -f $withval/servproc.h; then 
        FHTTPD_INCLUDE=-I$withval/
        FHTTPD_LIB=$withval/servproc.o
        FHTTPD_TARGET=$withval/
        PHP_BUILD_STATIC
		PHP_SAPI=cgi
        AC_DEFINE(FHTTPD,1,[ ])
        AC_MSG_RESULT(yes - fhttpd 0.3.x)
      else
        AC_MSG_RESULT(no)
        AC_MSG_ERROR(Invalid fhttpd directory - unable to find servproc.h under $withval)
      fi
    else
      AC_MSG_RESULT(no)
    fi
],[
    AC_MSG_RESULT(no)
])
INCLUDES="$INCLUDES $FHTTPD_INCLUDE"
dnl## AC_SUBST(FHTTPD_INCLUDE)
PHP_SUBST(FHTTPD_LIB)
PHP_SUBST(FHTTPD_TARGET)

case $host_alias in
  *darwin*)
    BUILD_CGI="\$(CC) \$(COMMON_FLAGS) \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS) \$(LDFLAGS) \$(NATIVE_RPATHS) \$(PHP_GLOBAL_OBJS:.lo=.o) \$(PHP_SAPI_OBJS:.lo=.o) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o php"
    ;;
  *)
    BUILD_CGI="\$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(COMMON_FLAGS) \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS) \$(LDFLAGS) \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS) \$(PHP_SAPI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o php"
    ;;
esac
PHP_SUBST(BUILD_CGI)
