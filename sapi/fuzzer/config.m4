PHP_ARG_ENABLE([fuzzer],
  [for Clang fuzzer SAPI],
  [AS_HELP_STRING([--enable-fuzzer],
    [Build PHP as Clang fuzzing test module (for developers)])],
  [no],
  [no])

dnl For newer Clang versions see https://llvm.org/docs/LibFuzzer.html#fuzzer-usage
dnl for relevant flags.

dnl
dnl PHP_FUZZER_TARGET(name, target-var)
dnl
dnl Macro to define the fuzzing target.
dnl
AC_DEFUN([PHP_FUZZER_TARGET], [
  PHP_FUZZER_BINARIES="$PHP_FUZZER_BINARIES $SAPI_FUZZER_PATH/php-fuzz-$1"
  PHP_SUBST([$2])
  PHP_ADD_SOURCES_X([sapi/fuzzer], [fuzzer-$1.c], [], [$2])
  $2="[$]$2 $FUZZER_COMMON_OBJS"
])

if test "$PHP_FUZZER" != "no"; then
  AS_VAR_IF([enable_zts], [yes], [AC_MSG_ERROR(m4_text_wrap([
    Thread safety (--enable-zts) is not supported when building fuzzing module
    (--enable-fuzzer). Please build fuzzer without ZTS.
  ]))])

  dnl Don't use PHP_REQUIRE_CXX() to avoid unnecessarily pulling in -lstdc++
  AC_PROG_CXX
  AC_PROG_CXXCPP
  PHP_ADD_MAKEFILE_FRAGMENT([$abs_srcdir/sapi/fuzzer/Makefile.frag])
  SAPI_FUZZER_PATH=sapi/fuzzer
  PHP_SUBST([SAPI_FUZZER_PATH])
  AS_VAR_IF([LIB_FUZZING_ENGINE],, [
    FUZZING_LIB="-fsanitize=fuzzer"
    FUZZING_CC=$CC
    AX_CHECK_COMPILE_FLAG([-fsanitize=fuzzer-no-link], [
      CFLAGS="$CFLAGS -fsanitize=fuzzer-no-link"
      CXXFLAGS="$CXXFLAGS -fsanitize=fuzzer-no-link"
    ],
    [AC_MSG_ERROR([Compiler doesn't support -fsanitize=fuzzer-no-link])])
  ], [
    FUZZING_LIB=$LIB_FUZZING_ENGINE
    FUZZING_CC="$CXX -stdlib=libc++"
  ])
  PHP_SUBST([FUZZING_LIB])
  PHP_SUBST([FUZZING_CC])

  dnl PHP_SELECT_SAPI([fuzzer-parser], [program], [$FUZZER_SOURCES])

  PHP_ADD_BUILD_DIR([sapi/fuzzer])
  PHP_FUZZER_BINARIES=""
  PHP_BINARIES="$PHP_BINARIES fuzzer"
  PHP_INSTALLED_SAPIS="$PHP_INSTALLED_SAPIS fuzzer"

  PHP_ADD_SOURCES_X([sapi/fuzzer], [fuzzer-sapi.c], [], [FUZZER_COMMON_OBJS])

  PHP_FUZZER_TARGET([parser], [PHP_FUZZER_PARSER_OBJS])
  PHP_FUZZER_TARGET([execute], [PHP_FUZZER_EXECUTE_OBJS])
  PHP_FUZZER_TARGET([function-jit], [PHP_FUZZER_FUNCTION_JIT_OBJS])
  PHP_FUZZER_TARGET([tracing-jit], [PHP_FUZZER_TRACING_JIT_OBJS])
  PHP_FUZZER_TARGET([unserialize], [PHP_FUZZER_UNSERIALIZE_OBJS])
  PHP_FUZZER_TARGET([unserializehash], [PHP_FUZZER_UNSERIALIZEHASH_OBJS])
  PHP_FUZZER_TARGET([json], [PHP_FUZZER_JSON_OBJS])

  if test -n "$enable_exif" && test "$enable_exif" != "no"; then
    PHP_FUZZER_TARGET([exif], [PHP_FUZZER_EXIF_OBJS])
  fi
  if test -n "$enable_mbstring" && test "$enable_mbstring" != "no"; then
    PHP_FUZZER_TARGET([mbstring], [PHP_FUZZER_MBSTRING_OBJS])
    if test -n "$enable_mbregex" && test "$enable_mbregex" != "no"; then
      PHP_FUZZER_TARGET([mbregex], [PHP_FUZZER_MBREGEX_OBJS])
    fi
  fi

  PHP_SUBST([PHP_FUZZER_BINARIES])
fi
