dnl
dnl $Id$
dnl

dnl By default we'll compile and link against the bundled PCRE library
dnl if DIR is supplied, we'll use that for linking

PHP_ARG_WITH(pcre-regex,,
[  --with-pcre-regex=DIR   Include Perl Compatible Regular Expressions support.
                          DIR is the PCRE install prefix [BUNDLED]], yes, no)

PHP_ARG_WITH(pcre-jit,,[  --with-pcre-jit         Enable PCRE JIT functionality (BUNDLED only)], yes, no)

  if test "$PHP_PCRE_REGEX" != "yes" && test "$PHP_PCRE_REGEX" != "no"; then
    AC_MSG_CHECKING([for PCRE headers location])
    for i in $PHP_PCRE_REGEX $PHP_PCRE_REGEX/include $PHP_PCRE_REGEX/include/pcre $PHP_PCRE_REGEX/local/include; do
      test -f $i/pcre2.h && PCRE_INCDIR=$i
    done

    if test -z "$PCRE_INCDIR"; then
      AC_MSG_ERROR([Could not find pcre2.h in $PHP_PCRE_REGEX])
    fi
    AC_MSG_RESULT([$PCRE_INCDIR])

    AC_MSG_CHECKING([for PCRE library location])
    for j in $PHP_PCRE_REGEX $PHP_PCRE_REGEX/$PHP_LIBDIR; do
      test -f $j/libpcre2.a || test -f $j/libpcre2.$SHLIB_SUFFIX_NAME && PCRE_LIBDIR=$j
    done
    
    if test -z "$PCRE_LIBDIR" ; then
      AC_MSG_ERROR([Could not find libpcre2.(a|$SHLIB_SUFFIX_NAME) in $PHP_PCRE_REGEX])
    fi
    AC_MSG_RESULT([$PCRE_LIBDIR])

    changequote({,})
    pcre_major=`grep PCRE2_MAJOR $PCRE_INCDIR/pcre2.h | sed -e 's/[^0-9]//g'`
    pcre_minor=`grep PCRE2_MINOR $PCRE_INCDIR/pcre2.h | sed -e 's/[^0-9]//g'`
    changequote([,])
    pcre_minor_length=`echo "$pcre_minor" | wc -c | sed -e 's/[^0-9]//g'`
    if test "$pcre_minor_length" -eq 2 ; then
      pcre_minor="$pcre_minor"0
    fi
    pcre_version=$pcre_major$pcre_minor
    if test "$pcre_version" -lt 1030; then
      AC_MSG_ERROR([The PCRE extension requires PCRE library version >= 10.30])
    fi

    PHP_CHECK_LIBRARY(pcre2, pcre2_jit_exec,
    [
      AC_DEFINE(HAVE_PCRE_JIT_SUPPORT, 1, [ ])
    ],[
    ],[
      -L$PCRE_LIBDIR
    ])
    PHP_ADD_LIBRARY_WITH_PATH(pcre2, $PCRE_LIBDIR)
    
    AC_DEFINE(HAVE_PCRE, 1, [ ])
    AC_DEFINE(PCRE2_CODE_UNIT_WIDTH, 8, [ ])
    PHP_ADD_INCLUDE($PCRE_INCDIR)
    PHP_NEW_EXTENSION(pcre, php_pcre.c, no,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
    PHP_INSTALL_HEADERS([ext/pcre], [php_pcre.h])
  else
    AC_MSG_CHECKING([for PCRE library to use])
    AC_MSG_RESULT([bundled])
    pcrelib_sources="pcre2lib/pcre2_auto_possess.c pcre2lib/pcre2_chartables.c pcre2lib/pcre2_compile.c \
    	pcre2lib/pcre2_config.c pcre2lib/pcre2_context.c pcre2lib/pcre2_dfa_match.c pcre2lib/pcre2_error.c \
	pcre2lib/pcre2_jit_compile.c pcre2lib/pcre2_maketables.c pcre2lib/pcre2_match.c pcre2lib/pcre2_match_data.c \
	pcre2lib/pcre2_newline.c pcre2lib/pcre2_ord2utf.c pcre2lib/pcre2_pattern_info.c pcre2lib/pcre2_serialize.c \
	pcre2lib/pcre2_string_utils.c pcre2lib/pcre2_study.c pcre2lib/pcre2_substitute.c  pcre2lib/pcre2_substring.c \
	pcre2lib/pcre2_tables.c pcre2lib/pcre2_ucd.c pcre2lib/pcre2_valid_utf.c pcre2lib/pcre2_xclass.c \
	pcre2lib/pcre2_find_bracket.c pcre2lib/pcre2_convert.c pcre2lib/pcre2_extuni.c"
    PHP_PCRE_CFLAGS="-DHAVE_CONFIG_H -I@ext_srcdir@/pcre2lib -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1"
    PHP_NEW_EXTENSION(pcre, $pcrelib_sources php_pcre.c, no,,$PHP_PCRE_CFLAGS)
    PHP_ADD_BUILD_DIR($ext_builddir/pcre2lib)
    PHP_INSTALL_HEADERS([ext/pcre], [php_pcre.h pcre2lib/])
    AC_DEFINE(HAVE_BUNDLED_PCRE, 1, [ ])
    AC_DEFINE(PCRE2_CODE_UNIT_WIDTH, 8, [ ])

    if test "$PHP_PCRE_REGEX" != "no"; then
      AC_MSG_CHECKING([whether to enable PCRE JIT functionality])
      if test "$PHP_PCRE_JIT" != "no"; then
        AC_DEFINE(HAVE_PCRE_JIT_SUPPORT, 1, [ ])
        AC_MSG_RESULT([yes])
      else
      AC_MSG_RESULT([no])
      fi
    fi
  fi

PHP_ARG_WITH(pcre-valgrind,,[  --with-pcre-valgrind=DIR
                          Enable PCRE valgrind support. Developers only!], no, no)
  if test "$PHP_PCRE_REGEX" != "yes" && test "$PHP_PCRE_REGEX" != "no"; then
    AC_MSG_WARN([PHP is going to be linked with an external PCRE, --with-pcre-valgrind has no effect])
  else
    if test "$PHP_PCRE_VALGRIND" != "no"; then
      PHP_PCRE_VALGRIND_INCDIR=
      AC_MSG_CHECKING([for Valgrind headers location])
      for i in $PHP_PCRE_VALGRIND $PHP_PCRE_VALGRIND/include $PHP_PCRE_VALGRIND/local/include /usr/include /usr/local/include; do
        if test -f $i/valgrind/memcheck.h
        then
          PHP_PCRE_VALGRIND_INCDIR=$i
          break
        fi
      done
      if test -z "$PHP_PCRE_VALGRIND_INCDIR"
      then
        AC_MSG_ERROR([Could not find valgrind/memcheck.h])
      else
        AC_DEFINE(HAVE_PCRE_VALGRIND_SUPPORT, 1, [ ])
        PHP_ADD_INCLUDE($PHP_PCRE_VALGRIND_INCDIR)
        AC_MSG_RESULT([$PHP_PCRE_VALGRIND_INCDIR])
      fi
    fi
  fi

