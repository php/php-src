# $Id$
# config.m4 for servlet sapi

AC_MSG_CHECKING(for Servlet support)
AC_ARG_WITH(servlet,
[  --with-servlet[=DIR]	  Include servlet support. DIR is the base install
			  directory for the JSDK.  This SAPI prereqs the
			  java extension must be built as a shared dl.],
[
  if test "$withval" != "no"; then

    if test "$withval" = "yes"; then
      SERVLET_CLASSPATH=.
    else
      if test -f $withval/lib/jsdk.jar; then
	SERVLET_CLASSPATH=$withval/lib/jsdk.jar
      else
	if test -d $withval/javax; then
	  SERVLET_CLASSPATH=$withval
	else
	  AC_MSG_RESULT(no)
	  AC_MSG_ERROR(unable to find JSDK libraries)
	fi
      fi
    fi

    AC_DEFINE(SAPI_SERVLET, 1, [Whether you use Servlet])

    INSTALL_IT="\$(INSTALL) -m 0755 \$(srcdir)/sapi/servlet/phpsrvlt.jar \$(libdir)"
    INSTALL_IT="$INSTALL_IT; \$(INSTALL) -m 0755 $SAPI_SHARED \$(libdir)"
    PHP_EXTENSION(servlet, "shared")
    PHP_SAPI=servlet
    PHP_BUILD_SHARED
    PHP_BUILD_THREAD_SAFE
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])

PHP_SUBST(SERVLET_CLASSPATH)
