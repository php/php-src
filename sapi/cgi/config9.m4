dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(cgi,,
[  --disable-cgi           Disable building CGI version of PHP], yes, no)

PHP_ARG_ENABLE(fastcgi,,
[  --enable-fastcgi          CGI: Enable FastCGI support in the CGI binary], no, no)

PHP_ARG_ENABLE(force-cgi-redirect,,
[  --enable-force-cgi-redirect
                            CGI: Enable security check for internal server
                            redirects. Use this if you run the PHP CGI with Apache], no, no)

PHP_ARG_ENABLE(discard-path,,
[  --enable-discard-path     CGI: When this is enabled the PHP CGI binary can 
                            safely be placed outside of the web tree and people
                            will not be able to circumvent .htaccess security], no, no)

PHP_ARG_ENABLE(path-info-check,,
[  --disable-path-info-check CGI: If this is disabled, paths such as
                            /info.php/test?a=b will fail to work], yes, no)

dnl
dnl CGI setup
dnl
if test "$PHP_SAPI" = "default"; then
  AC_MSG_CHECKING(whether to build CGI binary)
  if test "$PHP_CGI" != "no"; then
    AC_MSG_RESULT(yes)
    PHP_ADD_MAKEFILE_FRAGMENT($abs_srcdir/sapi/cgi/Makefile.frag)

    dnl Set filename
    case $host_alias in
      *cygwin* )
        SAPI_CGI_PATH=sapi/cgi/php-cgi.exe
        ;;
      * )
        SAPI_CGI_PATH=sapi/cgi/php-cgi
        ;;
    esac
    PHP_SUBST(SAPI_CGI_PATH)

    dnl --enable-fastcgi
    AC_MSG_CHECKING(whether to enable fastcgi support)
    if test "$PHP_FASTCGI" = "yes"; then
      PHP_ENABLE_FASTCGI=1
      PHP_FCGI_FILES="fastcgi.c"
    else
      PHP_ENABLE_FASTCGI=0
      PHP_FCGI_FILES=
    fi
    AC_DEFINE_UNQUOTED(PHP_FASTCGI, $PHP_ENABLE_FASTCGI, [ ])
    AC_MSG_RESULT($PHP_FASTCGI)

    dnl --enable-force-cgi-redirect
    AC_MSG_CHECKING(whether to force Apache CGI redirect)
    if test "$PHP_FORCE_CGI_REDIRECT" = "yes"; then
      CGI_REDIRECT=1
    else
      CGI_REDIRECT=0
    fi
    AC_DEFINE_UNQUOTED(FORCE_CGI_REDIRECT, $CGI_REDIRECT, [ ])
    AC_MSG_RESULT($PHP_FORCE_CGI_REDIRECT)

    dnl --enable-discard-path
    AC_MSG_CHECKING(whether to discard path_info + path_translated)
    if test "$PHP_DISCARD_PATH" = "yes"; then
      DISCARD_PATH=1
    else
      DISCARD_PATH=0
    fi
    AC_DEFINE_UNQUOTED(DISCARD_PATH, $DISCARD_PATH, [ ])
    AC_MSG_RESULT($PHP_DISCARD_PATH)

    dnl --enable-path-info-check
    AC_MSG_CHECKING(whether to enable path info checking)
    if test "$PHP_PATH_INFO_CHECK" = "yes"; then
      ENABLE_PATHINFO_CHECK=1
    else
      ENABLE_PATHINFO_CHECK=0
    fi
    AC_DEFINE_UNQUOTED(ENABLE_PATHINFO_CHECK, $ENABLE_PATHINFO_CHECK, [ ])
    AC_MSG_RESULT($PHP_PATH_INFO_CHECK)

    dnl Set install target and select SAPI
    INSTALL_IT="@echo \"Installing PHP CGI binary: \$(INSTALL_ROOT)\$(bindir)/\"; \$(INSTALL) -m 0755 \$(SAPI_CGI_PATH) \$(INSTALL_ROOT)\$(bindir)/\$(program_prefix)php-cgi\$(program_suffix)\$(EXEEXT)"
    PHP_SELECT_SAPI(cgi, program, $PHP_FCGI_FILES cgi_main.c getopt.c,, '$(SAPI_CGI_PATH)')

    case $host_alias in
      *aix*)
        BUILD_CGI="echo '\#! .' > php.sym && echo >>php.sym && nm -BCpg \`echo \$(PHP_GLOBAL_OBJS) \$(PHP_SAPI_OBJS) | sed 's/\([A-Za-z0-9_]*\)\.lo/\1.o/g'\` | \$(AWK) '{ if (((\$\$2 == \"T\") || (\$\$2 == \"D\") || (\$\$2 == \"B\")) && (substr(\$\$3,1,1) != \".\")) { print \$\$3 } }' | sort -u >> php.sym && \$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) -Wl,-brtl -Wl,-bE:php.sym \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS) \$(PHP_SAPI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CGI_PATH)"
        ;;
      *darwin*)
        BUILD_CGI="\$(CC) \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(NATIVE_RPATHS) \$(PHP_GLOBAL_OBJS:.lo=.o) \$(PHP_SAPI_OBJS:.lo=.o) \$(PHP_FRAMEWORKS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CGI_PATH)"
      ;;
      *)
        BUILD_CGI="\$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS) \$(PHP_SAPI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CGI_PATH)"
      ;;
    esac

    PHP_SUBST(BUILD_CGI)

  elif test "$PHP_CLI" != "no"; then
    AC_MSG_RESULT(no)
    OVERALL_TARGET=
    PHP_SAPI=cli   
  else
    AC_MSG_ERROR([No SAPIs selected.])  
  fi
fi
