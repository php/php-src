dnl By default we'll compile and link against the bundled PCRE library. If
dnl --with-external-pcre is supplied, we'll use that for linking.
PHP_ARG_WITH([external-pcre],,
  [AS_HELP_STRING([--with-external-pcre],
    [Use external library for PCRE support])],
  [no],
  [no])

PHP_ARG_WITH([pcre-jit],
  [whether to enable PCRE JIT functionality],
  [AS_HELP_STRING([--without-pcre-jit],
    [Disable PCRE JIT functionality])],
  [yes],
  [no])

AH_TEMPLATE([PCRE2_CODE_UNIT_WIDTH],
  [Number of bits in non-UTF mode for PCRE library.])
AH_TEMPLATE([HAVE_PCRE_JIT_SUPPORT],
  [Define to 1 if PCRE JIT is enabled and supported.])

if test "$PHP_EXTERNAL_PCRE" != "no"; then
  PKG_CHECK_MODULES([PCRE2], [libpcre2-8 >= 10.30])

  PHP_EVAL_INCLINE([$PCRE2_CFLAGS])
  PHP_EVAL_LIBLINE([$PCRE2_LIBS])
  AC_DEFINE([PCRE2_CODE_UNIT_WIDTH], [8])

  AS_VAR_IF([PHP_PCRE_JIT], [no],,
    [AC_CACHE_CHECK([whether external PCRE2 library has JIT supported],
      [php_cv_have_pcre2_jit],
      [
        CFLAGS_SAVE=$CFLAGS
        LIBS_SAVE=$LIBS
        CFLAGS="$CFLAGS $PCRE2_CFLAGS"
        LIBS="$LIBS $PCRE2_LIBS"
        AC_RUN_IFELSE([AC_LANG_SOURCE([
          #include <pcre2.h>
          #include <stdlib.h>
          int main(void) {
            uint32_t have_jit;
            pcre2_config_8(PCRE2_CONFIG_JIT, &have_jit);
            return !have_jit;
          }
        ])],
        [php_cv_have_pcre2_jit=yes],
        [php_cv_have_pcre2_jit=no],
        [AS_CASE([$host_cpu],
          [arm*|i[[34567]]86|x86_64|mips*|powerpc*|sparc],
            [php_cv_have_pcre2_jit=yes],
          [php_cv_have_pcre2_jit=no])])
        CFLAGS=$CFLAGS_SAVE
        LIBS=$LIBS_SAVE
      ])
    AS_VAR_IF([php_cv_have_pcre2_jit], [yes],
      [AC_DEFINE([HAVE_PCRE_JIT_SUPPORT], [1])])
  ])

  PHP_NEW_EXTENSION([pcre],
    [php_pcre.c],
    [no],,
    [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
  PHP_INSTALL_HEADERS([ext/pcre], [php_pcre.h])
else
  AC_MSG_CHECKING([for PCRE library to use])
  AC_MSG_RESULT([bundled])
  pcrelib_sources=m4_normalize(["
    pcre2lib/pcre2_auto_possess.c
    pcre2lib/pcre2_chartables.c
    pcre2lib/pcre2_chkdint.c
    pcre2lib/pcre2_compile.c
    pcre2lib/pcre2_config.c
    pcre2lib/pcre2_context.c
    pcre2lib/pcre2_convert.c
    pcre2lib/pcre2_dfa_match.c
    pcre2lib/pcre2_error.c
    pcre2lib/pcre2_extuni.c
    pcre2lib/pcre2_find_bracket.c
    pcre2lib/pcre2_jit_compile.c
    pcre2lib/pcre2_maketables.c
    pcre2lib/pcre2_match_data.c
    pcre2lib/pcre2_match.c
    pcre2lib/pcre2_newline.c
    pcre2lib/pcre2_ord2utf.c
    pcre2lib/pcre2_pattern_info.c
    pcre2lib/pcre2_script_run.c
    pcre2lib/pcre2_serialize.c
    pcre2lib/pcre2_string_utils.c
    pcre2lib/pcre2_study.c
    pcre2lib/pcre2_substitute.c
    pcre2lib/pcre2_substring.c
    pcre2lib/pcre2_tables.c
    pcre2lib/pcre2_ucd.c
    pcre2lib/pcre2_valid_utf.c
    pcre2lib/pcre2_xclass.c
  "])

  AX_CHECK_COMPILE_FLAG([-Wno-implicit-fallthrough],
    [PHP_PCRE_CFLAGS="$PHP_PCRE_CFLAGS -Wno-implicit-fallthrough"],,
    [-Werror])

  PHP_PCRE_CFLAGS="$PHP_PCRE_CFLAGS -DHAVE_CONFIG_H -I@ext_srcdir@/pcre2lib -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1"
  AC_DEFINE([HAVE_BUNDLED_PCRE], [1],
    [Define to 1 if PHP uses the bundled PCRE library.])
  AC_DEFINE([PCRE2_CODE_UNIT_WIDTH], [8])

  AS_VAR_IF([PHP_PCRE_JIT], [no],,
    [AC_DEFINE([HAVE_PCRE_JIT_SUPPORT], [1])
    AC_CACHE_CHECK([whether Intel CET is enabled],
      [php_cv_have_pcre2_intel_cet],
      [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([
          #ifndef __CET__
          # error CET is not enabled
          #endif
        ])],
        [php_cv_have_pcre2_intel_cet=yes],
        [php_cv_have_pcre2_intel_cet=no])
    ])
    AS_VAR_IF([php_cv_have_pcre2_intel_cet], [yes],
      [PHP_PCRE_CFLAGS="-mshstk $PHP_PCRE_CFLAGS"])
  ])

  dnl Enable pcre Valgrind support only in DEBUG build (it affects performance).
  AS_VAR_IF([PHP_VALGRIND], [no],,
    [AS_VAR_IF([ZEND_DEBUG], [yes],
      [AC_DEFINE([HAVE_PCRE_VALGRIND_SUPPORT], [1],
        [Define to 1 if pcre has Valgrind support enabled.])])])

  PHP_NEW_EXTENSION([pcre],
    [$pcrelib_sources php_pcre.c],
    [no],,
    [$PHP_PCRE_CFLAGS])
  PHP_ADD_BUILD_DIR([$ext_builddir/pcre2lib])
  PHP_INSTALL_HEADERS([ext/pcre], [php_pcre.h pcre2lib/])
fi
