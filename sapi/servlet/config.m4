dnl
dnl $Id$
dnl

AC_MSG_CHECKING(for Servlet support)
AC_ARG_WITH(servlet,
[  --with-servlet[=DIR]    Include servlet support. DIR is the base install
                          directory for the JSDK.  This SAPI prereqs the
                          java extension must be built as a shared dl.],
[
  if test "$withval" != "no"; then

    if test "$withval" = "yes"; then
      SERVLET_CLASSPATH=.
    else
      if test -f $withval/lib/servlet.jar; then
        SERVLET_CLASSPATH=$withval/lib/servlet.jar
      fi

      if test -f $withval/lib/jsdk.jar; then
        SERVLET_CLASSPATH=$withval/lib/jsdk.jar
      fi

      if test -d $withval/javax; then
        SERVLET_CLASSPATH=$withval
      fi

      if test -z "$SERVLET_CLASSPATH"; then
        AC_MSG_RESULT(no)
        AC_MSG_ERROR(unable to find servlet libraries)
      fi
    fi

    AC_DEFINE(SAPI_SERVLET, 1, [Whether you use Servlet])

    INSTALL_IT="\$(mkinstalldirs) \$(libdir)"
    INSTALL_IT="$INSTALL_IT; \$(INSTALL) -m 0755 \$(top_srcdir)/sapi/servlet/phpsrvlt.jar \$(libdir)"
    INSTALL_IT="$INSTALL_IT; \$(INSTALL) -m 0755 $SAPI_SHARED \$(libdir)"
    PHP_SAPI=servlet
    PHP_BUILD_THREAD_SAFE
    EXTRA_INCLUDES="$EXTRA_INCLUDES \$(JAVA_INCLUDE)"
    PHP_SELECT_SAPI(servlet, shared, servlet.c)
    PHP_ADD_SOURCES(/sapi/servlet, java.c,, sapi)
    PHP_ADD_MAKEFILE_FRAGMENT(sapi/servlet/Makefile.frag)

    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])

PHP_SUBST(SERVLET_CLASSPATH)
