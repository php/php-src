AC_MSG_CHECKING(for clang fuzzer SAPI)

PHP_ARG_ENABLE([fuzzer],,
  [AS_HELP_STRING([--enable-fuzzer],
    [Build PHP as clang fuzzing test module (for developers)])],
  [no],
  [no])

PHP_ARG_ENABLE([fuzzer-msan],,
  [AS_HELP_STRING([--enable-fuzzer-msan],
    [Enable msan instead of asan/ubsan when fuzzing])],
  [no],
  [no])

dnl For newer clang versions see https://llvm.org/docs/LibFuzzer.html#fuzzer-usage
dnl for relevant flags.

dnl Macro to define fuzzing target
dnl PHP_FUZZER_TARGET(name, target-var)
dnl
AC_DEFUN([PHP_FUZZER_TARGET], [
  PHP_FUZZER_BINARIES="$PHP_FUZZER_BINARIES $SAPI_FUZZER_PATH/php-fuzz-$1"
  PHP_SUBST($2)
  PHP_ADD_SOURCES_X([sapi/fuzzer],[fuzzer-$1.c],[],$2)
  $2="[$]$2 $FUZZER_COMMON_OBJS"
])

if test "$PHP_FUZZER" != "no"; then
  AC_MSG_RESULT([yes])
  dnl Don't use PHP_REQUIRE_CXX() to avoid unnecessarily pulling in -lstdc++
  AC_PROG_CXX
  AC_PROG_CXXCPP
  PHP_ADD_MAKEFILE_FRAGMENT($abs_srcdir/sapi/fuzzer/Makefile.frag)
  SAPI_FUZZER_PATH=sapi/fuzzer
  PHP_SUBST(SAPI_FUZZER_PATH)
  if test -z "$LIB_FUZZING_ENGINE"; then
    FUZZING_LIB="-fsanitize=fuzzer"
    FUZZING_CC="$CC"
    AX_CHECK_COMPILE_FLAG([-fsanitize=fuzzer-no-link], [
      CFLAGS="$CFLAGS -fsanitize=fuzzer-no-link"
      CXXFLAGS="$CXXFLAGS -fsanitize=fuzzer-no-link"

      if test "$PHP_FUZZER_MSAN" = "yes"; then
        CFLAGS="$CFLAGS -fsanitize=memory -fsanitize-memory-track-origins"
        CXXFLAGS="$CXXFLAGS -fsanitize=memory -fsanitize-memory-track-origins"
      else
        CFLAGS="$CFLAGS -fsanitize=address"
        CXXFLAGS="$CXXFLAGS -fsanitize=address"

        dnl Don't include -fundefined in CXXFLAGS, because that would also require linking
        dnl with a C++ compiler.
        dnl Disable object-size sanitizer, because it is incompatible with our zend_function
        dnl union, and this can't be easily fixed.
        dnl We need to specify -fno-sanitize-recover=undefined here, otherwise ubsan warnings
        dnl will not be considered failures by the fuzzer.
        CFLAGS="$CFLAGS -fsanitize=undefined -fno-sanitize=object-size -fno-sanitize-recover=undefined"
      fi
    ],[
      AC_MSG_ERROR(Compiler doesn't support -fsanitize=fuzzer-no-link)
    ])
  else
    FUZZING_LIB="$LIB_FUZZING_ENGINE"
    FUZZING_CC="$CXX -stdlib=libc++"
  fi
  PHP_SUBST(FUZZING_LIB)
  PHP_SUBST(FUZZING_CC)

  dnl PHP_SELECT_SAPI(fuzzer-parser, program, $FUZZER_SOURCES, , '$(SAPI_FUZZER_PATH)')

  PHP_ADD_BUILD_DIR([sapi/fuzzer])
  PHP_FUZZER_BINARIES=""
  PHP_BINARIES="$PHP_BINARIES fuzzer"
  PHP_INSTALLED_SAPIS="$PHP_INSTALLED_SAPIS fuzzer"

  PHP_ADD_SOURCES_X([sapi/fuzzer], [fuzzer-sapi.c], [], FUZZER_COMMON_OBJS)

  PHP_FUZZER_TARGET([parser], PHP_FUZZER_PARSER_OBJS)
  PHP_FUZZER_TARGET([execute], PHP_FUZZER_EXECUTE_OBJS)
  PHP_FUZZER_TARGET([unserialize], PHP_FUZZER_UNSERIALIZE_OBJS)
  PHP_FUZZER_TARGET([unserializehash], PHP_FUZZER_UNSERIALIZEHASH_OBJS)
  PHP_FUZZER_TARGET([json], PHP_FUZZER_JSON_OBJS)

  if test -n "$enable_exif" && test "$enable_exif" != "no"; then
    PHP_FUZZER_TARGET([exif], PHP_FUZZER_EXIF_OBJS)
  fi
  if test -n "$enable_mbstring" && test "$enable_mbstring" != "no"; then
    PHP_FUZZER_TARGET([mbstring], PHP_FUZZER_MBSTRING_OBJS)
  fi

  PHP_SUBST(PHP_FUZZER_BINARIES)
fi

AC_MSG_RESULT($PHP_FUZZER)
