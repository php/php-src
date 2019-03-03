dnl config.m4 for extension pcre

dnl By default we'll compile and link against the bundled PCRE library
dnl if DIR is supplied, we'll use that for linking

PHP_ARG_WITH([external-pcre],,
  [AS_HELP_STRING([--with-external-pcre],
    [Use external library for PCRE support])],
  [no],
  [no])

PHP_ARG_WITH([pcre-jit],,
  [AS_HELP_STRING([--with-pcre-jit],
    [Enable PCRE JIT functionality])],
  [yes],
  [no])

if test "$PHP_EXTERNAL_PCRE" != "no"; then

  PKG_CHECK_MODULES([PCRE2], [libpcre2-8 >= 10.30])

  PHP_EVAL_INCLINE($PCRE2_CFLAGS)
  PHP_EVAL_LIBLINE($PCRE2_LIBS)
  AC_DEFINE(PCRE2_CODE_UNIT_WIDTH, 8, [ ])
  AC_DEFINE(HAVE_PCRE, 1, [ ])

  if test "$PHP_PCRE_JIT" != "no"; then
    AC_MSG_CHECKING([for JIT support in PCRE2])
    AC_RUN_IFELSE([
      AC_LANG_SOURCE([[
          #include <pcre2.h>
          #include <stdlib.h>
          int main(void) {
            uint32_t have_jit;
            pcre2_config_8(PCRE2_CONFIG_JIT, &have_jit);
            return !have_jit;
          }
      ]])], [
      AC_MSG_RESULT([yes])
      AC_DEFINE(HAVE_PCRE_JIT_SUPPORT, 1, [])
    ],
    [
      AC_MSG_RESULT([no])
    ],
    [
      AC_CANONICAL_HOST
      case $host_cpu in
      arm*|i[34567]86|x86_64|mips*|powerpc*|sparc)
        AC_MSG_RESULT([yes])
        AC_DEFINE(HAVE_PCRE_JIT_SUPPORT, 1, [])
        ;;
      *)
        AC_MSG_RESULT([no])
        ;;
      esac
    ])
  fi

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

  AC_MSG_CHECKING([whether to enable PCRE JIT functionality])
  if test "$PHP_PCRE_JIT" != "no"; then
    AC_DEFINE(HAVE_PCRE_JIT_SUPPORT, 1, [ ])
    AC_MSG_RESULT([yes])
  else
    AC_MSG_RESULT([no])
  fi
fi

PHP_ARG_WITH([pcre-valgrind],,
  [AS_HELP_STRING([--with-pcre-valgrind=DIR],
    [Enable PCRE valgrind support. Developers only!])],
  [no],
  [no])

  if test "$PHP_EXTERNAL_PCRE" != "no"; then
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
