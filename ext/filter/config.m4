dnl $Id$
dnl config.m4 for input filtering extension

PHP_ARG_ENABLE(filter, whether to enable input filter support,
[  --disable-filter        Disable input filter support], yes)

PHP_ARG_WITH(pcre-dir, pcre install prefix,
[  --with-pcre-dir           FILTER: pcre install prefix], no, no)

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

  if test "$PHP_PCRE_REGEX" != "yes"; then
    dnl
    dnl If PCRE extension is enabled we can use the already found paths,
    dnl otherwise we have to detect them here:
    dnl
    if test "$PHP_PCRE_REGEX" = "no" || test "$PHP_PCRE_REGEX" = "pecl"; then
      dnl Set the PCRE search dirs correctly
      case "$PHP_PCRE_DIR" in
        yes|no)
          PCRE_SEARCH_DIR="/usr/local /usr"
          ;;
        *)
          PCRE_SEARCH_DIR="$PHP_PCRE_DIR"
          ;;
      esac

      for i in $PCRE_SEARCH_DIR; do
        if test -f $i/include/pcre/pcre.h; then
          PCRE_INCDIR=$i/include/pcre
          break
        elif test -f $i/include/pcre.h; then
          PCRE_INCDIR=$i/include
          break
        elif test -f $i/pcre.h; then
          PCRE_INCDIR=$i
          break
        fi
      done

      if test -z "$PCRE_INCDIR"; then
        AC_MSG_ERROR([Could not find pcre.h anywhere under $PCRE_SEARCH_DIR])
      fi

      for j in $PCRE_SEARCH_DIR/$PHP_LIBDIR $PCRE_SEARCH_DIR; do
        if test -f $j/libpcre.a || test -f $j/libpcre.$SHLIB_SUFFIX_NAME; then
          PCRE_LIBDIR=$j
          break
        fi
      done
    
      if test -z "$PCRE_LIBDIR" ; then
        AC_MSG_ERROR([Could not find libpcre.(a|$SHLIB_SUFFIX_NAME) anywhere under $PCRE_SEARCH_DIR])
      fi
    fi

    PHP_ADD_LIBRARY_WITH_PATH(pcre, $PCRE_LIBDIR, FILTER_SHARED_LIBADD)
    PHP_ADD_INCLUDE($PCRE_INCDIR)
  fi

  PHP_NEW_EXTENSION(filter, filter.c sanitizing_filters.c logical_filters.c callback_filter.c, $ext_shared)
  PHP_SUBST(FILTER_SHARED_LIBADD)

  PHP_INSTALL_HEADERS([$ext_srcdir/php_filter.h])
fi
