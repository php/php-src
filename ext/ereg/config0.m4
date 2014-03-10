dnl $Id$
dnl config.m4 for extension ereg

dnl
dnl Check for regex library type
dnl
PHP_ARG_WITH(regex,,
[  --with-regex=TYPE       Regex library type: system, php. [TYPE=php]
                          WARNING: Do NOT use unless you know what you are doing!], php, no)

case $PHP_REGEX in
  system)
    if test "$PHP_SAPI" = "apache" || test "$PHP_SAPI" = "apache2filter" || test "$PHP_SAPI" = "apache2handler"; then
      REGEX_TYPE=php
    else
      REGEX_TYPE=system
    fi
    ;;
  yes | php)
    REGEX_TYPE=php
    ;;
  *)
    REGEX_TYPE=php
    AC_MSG_WARN([Invalid regex library type selected. Using default value: php])
    ;;
esac

AC_MSG_CHECKING([which regex library to use])
AC_MSG_RESULT([$REGEX_TYPE])

if test "$REGEX_TYPE" = "php"; then
  ereg_regex_sources="regex/regcomp.c regex/regexec.c regex/regerror.c regex/regfree.c"
  ereg_regex_headers="regex/"
  PHP_EREG_CFLAGS="-Dregexec=php_regexec -Dregerror=php_regerror -Dregfree=php_regfree -Dregcomp=php_regcomp"
fi

PHP_NEW_EXTENSION(ereg, ereg.c $ereg_regex_sources, no,,$PHP_EREG_CFLAGS)
PHP_INSTALL_HEADERS([ext/ereg], [php_ereg.h php_regex.h $ereg_regex_headers])

if test "$REGEX_TYPE" = "php"; then
  AC_DEFINE(HAVE_REGEX_T_RE_MAGIC, 1, [ ])
  AC_DEFINE(HSREGEX,1,[ ])
  AC_DEFINE(REGEX,1,[ ])  
  PHP_ADD_BUILD_DIR([$ext_builddir/regex], 1)
  PHP_ADD_INCLUDE([$ext_srcdir/regex])
elif test "$REGEX_TYPE" = "system"; then
  AC_DEFINE(REGEX,0,[ ])
  dnl Check if field re_magic exists in struct regex_t
  AC_CACHE_CHECK([whether field re_magic exists in struct regex_t], ac_cv_regex_t_re_magic, [
  AC_TRY_COMPILE([#include <sys/types.h>
#include <regex.h>], [regex_t rt; rt.re_magic;],
  [ac_cv_regex_t_re_magic=yes], [ac_cv_regex_t_re_magic=no])])
  if test "$ac_cv_regex_t_re_magic" = "yes"; then
    AC_DEFINE([HAVE_REGEX_T_RE_MAGIC], [ ], 1)   
  fi 
fi   
