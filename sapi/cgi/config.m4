
if test "$PHP_SAPI" = "cgi"; then
AC_MSG_CHECKING(whether to force Apache CGI redirect)
AC_ARG_ENABLE(force-cgi-redirect,
[  --enable-force-cgi-redirect
                          Enable the security check for internal server
                          redirects.  You should use this if you are
                          running the CGI version with Apache. ],
[
  if test "$enableval" = "yes"; then
    AC_DEFINE(FORCE_CGI_REDIRECT, 1)
    AC_MSG_RESULT(yes)
    REDIRECT=1
  else
    AC_DEFINE(FORCE_CGI_REDIRECT, 0)
    AC_MSG_RESULT(no)
    REDIRECT=0
  fi
],[
  AC_DEFINE(FORCE_CGI_REDIRECT, 0)
  AC_MSG_RESULT(no)
  REDIRECT=0
]) 

AC_MSG_CHECKING(whether to discard path_info + path_translated)
AC_ARG_ENABLE(discard_path,
[  --enable-discard-path   If this is enabled, the PHP CGI binary
                          can safely be placed outside of the
                          web tree and people will not be able
                          to circumvent .htaccess security. ],
[
  if test "$enableval" = "yes"; then
    AC_DEFINE(DISCARD_PATH, 1)
    AC_MSG_RESULT(yes)
    DISCARD_PATH=1
  else
    AC_DEFINE(DISCARD_PATH, 0)
    AC_MSG_RESULT(no)
    DISCARD_PATH=0
  fi
],[
  AC_DEFINE(DISCARD_PATH, 0)
  AC_MSG_RESULT(no)
  DISCARD_PATH=0
]) 
fi

if test "$PHP_SAPI" = "cgi" ; then
  INSTALL_IT="\$(SHELL) \$(srcdir)/install-sh -m 0755 $SAPI_PROGRAM \$(bindir)/$SAPI_CGI"
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
        AC_DEFINE(FHTTPD)
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
AC_SUBST(FHTTPD_LIB)
AC_SUBST(FHTTPD_TARGET)
