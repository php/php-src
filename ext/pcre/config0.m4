dnl
dnl $Id$
dnl

dnl By default we'll compile and link against the bundled PCRE library
dnl if DIR is supplied, we'll use that for linking

PHP_ARG_WITH(pcre-regex,for PCRE support,
[  --without-pcre-regex    Do not include Perl Compatible Regular Expressions 
                          support. Use --with-pcre-regex=DIR to specify DIR
                          where PCRE's include and library files are located,
                          if not using bundled library.],yes)

if test "$PHP_PCRE_REGEX" != "no"; then
  if test "$PHP_PCRE_REGEX" = "yes"; then
    PHP_NEW_EXTENSION(pcre, pcrelib/maketables.c pcrelib/get.c pcrelib/study.c pcrelib/pcre.c php_pcre.c, $ext_shared,,-DSUPPORT_UTF8 -DLINK_SIZE=2 -I@ext_srcdir@/pcrelib)
    PHP_ADD_BUILD_DIR($ext_builddir/pcrelib)
    AC_DEFINE(HAVE_BUNDLED_PCRE, 1, [ ])
  else
    for i in $PHP_PCRE_REGEX $PHP_PCRE_REGEX/include $PHP_PCRE_REGEX/include/pcre; do
      test -f $i/pcre.h && PCRE_INCDIR=$i
    done

    if test -z "$PCRE_INCDIR"; then
      AC_MSG_ERROR([Could not find pcre.h in $PHP_PCRE_REGEX])
    fi

    for j in $PHP_PCRE_REGEX $PHP_PCRE_REGEX/lib; do
      test -f $j/libpcre.a -o -f $j/libpcre.$SHLIB_SUFFIX_NAME && PCRE_LIBDIR=$j
    done
    
    if test -z "$PCRE_LIBDIR" ; then
      AC_MSG_ERROR([Could not find libpcre.(a|$SHLIB_SUFFIX_NAME) in $PHP_PCRE_REGEX])
    fi

    changequote({,})
    pcre_major=`grep PCRE_MAJOR $PCRE_INCDIR/pcre.h | sed -e 's/[^0-9]//g'`
    pcre_minor=`grep PCRE_MINOR $PCRE_INCDIR/pcre.h | sed -e 's/[^0-9]//g'`
    changequote([,])
    pcre_minor_length=`echo "$pcre_minor" | wc -c | sed -e 's/[^0-9]//g'`
    if test "$pcre_minor_length" -eq 2 ; then
      pcre_minor="$pcre_minor"0
    fi
    pcre_version=$pcre_major$pcre_minor
    if test "$pcre_version" -lt 208; then
      AC_MSG_ERROR([The PCRE extension requires PCRE library version >= 2.08])
    fi

    PHP_ADD_LIBRARY_WITH_PATH(pcre, $PCRE_LIBDIR, PCRE_SHARED_LIBADD)
    
    AC_DEFINE(HAVE_PCRE, 1, [ ])
    PHP_ADD_INCLUDE($PCRE_INCDIR)
    PHP_NEW_EXTENSION(pcre, php_pcre.c, $ext_shared,,-DSUPPORT_UTF8 -DLINK_SIZE=2)
  fi
  PHP_SUBST(PCRE_SHARED_LIBADD)
fi
