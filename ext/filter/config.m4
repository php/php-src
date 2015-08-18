dnl $Id$
dnl config.m4 for input filtering extension

PHP_ARG_ENABLE(filter, whether to enable input filter support,
[  --disable-filter        Disable input filter support], yes)

PHP_ARG_WITH(pcre-dir, pcre install prefix,
[  --with-pcre-dir         FILTER: pcre install prefix], no, no)

if test "$PHP_FILTER" != "no"; then

  dnl Check if configure is the PHP core configure
  if test -n "$PHP_VERSION"; then
    dnl This extension can not be build as shared when in PHP core
    ext_shared=no
  else
    dnl This is PECL build, check if bundled PCRE library is used
    old_CPPFLAGS=$CPPFLAGS
    CPPFLAGS=$INCLUDES
    AC_EGREP_CPP(yes,[
#include <main/php_config.h>
#if defined(HAVE_BUNDLED_PCRE) && !defined(COMPILE_DL_PCRE)
yes
#endif
    ],[
      PHP_PCRE_REGEX=yes
    ],[
      AC_EGREP_CPP(yes,[
#include <main/php_config.h>
#if defined(HAVE_PCRE) && !defined(COMPILE_DL_PCRE)
yes
#endif
      ],[
        PHP_PCRE_REGEX=pecl
      ],[
        PHP_PCRE_REGEX=no
      ])
    ])
    CPPFLAGS=$old_CPPFLAGS
  fi

  PHP_NEW_EXTENSION(filter, filter.c sanitizing_filters.c logical_filters.c callback_filter.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  PHP_SUBST(FILTER_SHARED_LIBADD)

  PHP_INSTALL_HEADERS([ext/filter/php_filter.h])
  PHP_ADD_EXTENSION_DEP(filter, pcre)
fi
