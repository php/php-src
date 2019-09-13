AC_MSG_CHECKING(for clang fuzzer SAPI)

PHP_ARG_ENABLE([fuzzer],,
  [AS_HELP_STRING([--enable-fuzzer],
    [Build PHP as clang fuzzing test module (for developers)])],
  [no])

dnl For newer clang versions see https://llvm.org/docs/LibFuzzer.html#fuzzer-usage
dnl for relevant flags.

dnl Macro to define fuzzing target
dnl PHP_FUZZER_TARGET(name, target-var)
dnl
AC_DEFUN([PHP_FUZZER_TARGET], [
  PHP_FUZZER_BINARIES="$PHP_FUZZER_BINARIES $SAPI_FUZZER_PATH/php-fuzz-$1"
  PHP_SUBST($2)
  PHP_ADD_SOURCES_X([sapi/fuzzer],[fuzzer-$1.c fuzzer-sapi.c],[],$2)
])

if test "$PHP_FUZZER" != "no"; then
  AC_MSG_RESULT([yes])
  PHP_REQUIRE_CXX()
  PHP_ADD_MAKEFILE_FRAGMENT($abs_srcdir/sapi/fuzzer/Makefile.frag)
  SAPI_FUZZER_PATH=sapi/fuzzer
  PHP_SUBST(SAPI_FUZZER_PATH)
  if test -z "$LIB_FUZZING_ENGINE"; then
    FUZZING_LIB="-fsanitize=fuzzer"
    FUZZING_CC="$CC"
    AX_CHECK_COMPILE_FLAG([-fsanitize=fuzzer-no-link,address], [
      CFLAGS="$CFLAGS -fsanitize=fuzzer-no-link,address"
      CXXFLAGS="$CXXFLAGS -fsanitize=fuzzer-no-link,address"
      LDFLAGS="$LDFLAGS -fsanitize=fuzzer-no-link,address"
    ],[
      AC_MSG_ERROR(compiler doesn't support -fsanitize flags)
    ])
  else
    FUZZING_LIB="-lFuzzingEngine"
    FUZZING_CC="$CXX -stdlib=libc++"
  fi
  PHP_SUBST(FUZZING_LIB)
  PHP_SUBST(FUZZING_CC)

  dnl PHP_SELECT_SAPI(fuzzer-parser, program, $FUZZER_SOURCES, , '$(SAPI_FUZZER_PATH)')

  PHP_ADD_BUILD_DIR([sapi/fuzzer])
  PHP_FUZZER_BINARIES=""
  PHP_INSTALLED_SAPIS="$PHP_INSTALLED_SAPIS fuzzer"

  PHP_FUZZER_TARGET([parser], PHP_FUZZER_PARSER_OBJS)
  PHP_FUZZER_TARGET([unserialize], PHP_FUZZER_UNSERIALIZE_OBJS)
  PHP_FUZZER_TARGET([exif], PHP_FUZZER_EXIF_OBJS)

  if test -n "$enable_json" && test "$enable_json" != "no"; then
    PHP_FUZZER_TARGET([json], PHP_FUZZER_JSON_OBJS)
  fi
  if test -n "$enable_mbstring" && test "$enable_mbstring" != "no"; then
    PHP_FUZZER_TARGET([mbstring], PHP_FUZZER_MBSTRING_OBJS)
  fi

  PHP_SUBST(PHP_FUZZER_BINARIES)
fi

AC_MSG_RESULT($PHP_FUZZER)
