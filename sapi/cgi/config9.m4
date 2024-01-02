PHP_ARG_ENABLE([cgi],,
  [AS_HELP_STRING([--disable-cgi],
    [Disable building CGI version of PHP])],
  [yes],
  [no])

dnl CGI setup.
AC_MSG_CHECKING(for CGI build)
if test "$PHP_CGI" != "no"; then
    AC_MSG_RESULT(yes)

    dnl BSD systems.
    AC_CHECK_MEMBERS([struct sockaddr_un.sun_len],,,[#include <sys/un.h>])

    AC_MSG_CHECKING([whether cross-process locking is required by accept()])
    case "`uname -sr`" in
      SunOS\ 5.*)
        AC_MSG_RESULT([yes])
        AC_DEFINE([USE_LOCKING], [1],
          [Define if cross-process locking is required by accept()])
      ;;
      *)
        AC_MSG_RESULT([no])
      ;;
    esac

    PHP_ADD_MAKEFILE_FRAGMENT($abs_srcdir/sapi/cgi/Makefile.frag)

    dnl Set filename.
    case $host_alias in
      *cygwin* )
        SAPI_CGI_PATH=sapi/cgi/php-cgi.exe
        ;;
      * )
        SAPI_CGI_PATH=sapi/cgi/php-cgi
        ;;
    esac

    dnl Select SAPI.
    PHP_SELECT_SAPI(cgi, program, cgi_main.c, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1, '$(SAPI_CGI_PATH)')

    case $host_alias in
      *aix*)
        if test "$php_sapi_module" = "shared"; then
          BUILD_CGI="echo '\#! .' > php.sym && echo >>php.sym && nm -BCpg \`echo \$(PHP_GLOBAL_OBJS) \$(PHP_BINARY_OBJS) \$(PHP_CGI_OBJS) | sed 's/\([A-Za-z0-9_]*\)\.lo/.libs\/\1.o/g'\` | \$(AWK) '{ if (((\$\$2 == \"T\") || (\$\$2 == \"D\") || (\$\$2 == \"B\")) && (substr(\$\$3,1,1) != \".\")) { print \$\$3 } }' | sort -u >> php.sym && \$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) -Wl,-brtl -Wl,-bE:php.sym \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS) \$(PHP_BINARY_OBJS) \$(PHP_FASTCGI_OBJS) \$(PHP_CGI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CGI_PATH)"
        else
          BUILD_CGI="echo '\#! .' > php.sym && echo >>php.sym && nm -BCpg \`echo \$(PHP_GLOBAL_OBJS) \$(PHP_BINARY_OBJS) \$(PHP_CGI_OBJS) | sed 's/\([A-Za-z0-9_]*\)\.lo/\1.o/g'\` | \$(AWK) '{ if (((\$\$2 == \"T\") || (\$\$2 == \"D\") || (\$\$2 == \"B\")) && (substr(\$\$3,1,1) != \".\")) { print \$\$3 } }' | sort -u >> php.sym && \$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) -Wl,-brtl -Wl,-bE:php.sym \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS) \$(PHP_BINARY_OBJS) \$(PHP_FASTCGI_OBJS) \$(PHP_CGI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CGI_PATH)"
        fi
        ;;
      *darwin*)
        BUILD_CGI="\$(CC) \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(NATIVE_RPATHS) \$(PHP_GLOBAL_OBJS:.lo=.o) \$(PHP_BINARY_OBJS:.lo=.o) \$(PHP_FASTCGI_OBJS:.lo=.o) \$(PHP_CGI_OBJS:.lo=.o) \$(PHP_FRAMEWORKS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CGI_PATH)"
      ;;
      *)
        BUILD_CGI="\$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS:.lo=.o) \$(PHP_BINARY_OBJS:.lo=.o) \$(PHP_FASTCGI_OBJS:.lo=.o) \$(PHP_CGI_OBJS:.lo=.o) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CGI_PATH)"
      ;;
    esac

    dnl Expose to Makefile.
    PHP_SUBST(SAPI_CGI_PATH)
    PHP_SUBST(BUILD_CGI)

    PHP_OUTPUT(sapi/cgi/php-cgi.1)
else
  AC_MSG_RESULT(no)
fi
