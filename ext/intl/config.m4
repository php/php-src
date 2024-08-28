PHP_ARG_ENABLE([intl],
  [whether to enable internationalization support],
  [AS_HELP_STRING([--enable-intl],
    [Enable internationalization support])])

if test "$PHP_INTL" != "no"; then
  PHP_SETUP_ICU([INTL_SHARED_LIBADD])
  PHP_SUBST([INTL_SHARED_LIBADD])
  INTL_COMMON_FLAGS="$ICU_CFLAGS -Wno-write-strings -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1"
  PHP_NEW_EXTENSION([intl], m4_normalize([
      collator/collator_attr.c
      collator/collator_class.c
      collator/collator_compare.c
      collator/collator_convert.c
      collator/collator_create.c
      collator/collator_error.c
      collator/collator_is_numeric.c
      collator/collator_locale.c
      collator/collator_sort.c
      common/common_error.c
      converter/converter.c
      dateformat/dateformat_attr.c
      dateformat/dateformat_class.c
      dateformat/dateformat_data.c
      dateformat/dateformat_format.c
      dateformat/dateformat_parse.c
      dateformat/dateformat.c
      formatter/formatter_attr.c
      formatter/formatter_class.c
      formatter/formatter_data.c
      formatter/formatter_format.c
      formatter/formatter_main.c
      formatter/formatter_parse.c
      grapheme/grapheme_string.c
      grapheme/grapheme_util.c
      idn/idn.c
      intl_convert.c
      intl_error.c
      locale/locale_class.c
      locale/locale_methods.c
      locale/locale.c
      msgformat/msgformat_attr.c
      msgformat/msgformat_class.c
      msgformat/msgformat_data.c
      msgformat/msgformat_format.c
      msgformat/msgformat_parse.c
      msgformat/msgformat.c
      normalizer/normalizer_class.c
      normalizer/normalizer_normalize.c
      php_intl.c
      resourcebundle/resourcebundle_class.c
      resourcebundle/resourcebundle_iterator.c
      resourcebundle/resourcebundle.c
      spoofchecker/spoofchecker_class.c
      spoofchecker/spoofchecker_create.c
      spoofchecker/spoofchecker_main.c
      transliterator/transliterator_class.c
      transliterator/transliterator_methods.c
      uchar/uchar.c
    ]),
    [$ext_shared],,
    [$INTL_COMMON_FLAGS],
    [cxx])

  PHP_INTL_CXX_SOURCES="intl_convertcpp.cpp \
    common/common_enum.cpp \
    common/common_date.cpp \
    dateformat/dateformat_format_object.cpp \
    dateformat/dateformat_create.cpp \
    dateformat/dateformat_attrcpp.cpp \
    dateformat/dateformat_helpers.cpp \
    dateformat/datepatterngenerator_class.cpp \
    dateformat/datepatterngenerator_methods.cpp \
    msgformat/msgformat_helpers.cpp \
    timezone/timezone_class.cpp \
    timezone/timezone_methods.cpp \
    calendar/calendar_class.cpp \
    calendar/calendar_methods.cpp \
    calendar/gregoriancalendar_methods.cpp \
    breakiterator/breakiterator_class.cpp \
    breakiterator/breakiterator_iterators.cpp \
    breakiterator/breakiterator_methods.cpp \
    breakiterator/rulebasedbreakiterator_methods.cpp \
    breakiterator/codepointiterator_internal.cpp \
    breakiterator/codepointiterator_methods.cpp"

  PHP_REQUIRE_CXX()

  AC_MSG_CHECKING([if intl requires -std=gnu++17])
  AS_IF([$PKG_CONFIG icu-uc --atleast-version=74],[
    AC_MSG_RESULT([yes])
    PHP_CXX_COMPILE_STDCXX([17], [mandatory], [PHP_INTL_STDCXX])
  ],[
    AC_MSG_RESULT([no])
    PHP_CXX_COMPILE_STDCXX([11], [mandatory], [PHP_INTL_STDCXX])
  ])

  PHP_INTL_CXX_FLAGS="$INTL_COMMON_FLAGS $PHP_INTL_STDCXX $ICU_CXXFLAGS"
  AS_CASE([$host_alias], [*cygwin*],
    [PHP_INTL_CXX_FLAGS="$PHP_INTL_CXX_FLAGS -D_POSIX_C_SOURCE=200809L"])
  AS_VAR_IF([ext_shared], [no],
    [PHP_ADD_SOURCES([$ext_dir],
      [$PHP_INTL_CXX_SOURCES],
      [$PHP_INTL_CXX_FLAGS])],
    [PHP_ADD_SOURCES_X([$ext_dir],
      [$PHP_INTL_CXX_SOURCES],
      [$PHP_INTL_CXX_FLAGS],
      [shared_objects_intl],
      [yes])])

  PHP_ADD_BUILD_DIR([
    $ext_builddir/breakiterator
    $ext_builddir/calendar
    $ext_builddir/collator
    $ext_builddir/common
    $ext_builddir/converter
    $ext_builddir/dateformat
    $ext_builddir/formatter
    $ext_builddir/grapheme
    $ext_builddir/idn
    $ext_builddir/locale
    $ext_builddir/msgformat
    $ext_builddir/normalizer
    $ext_builddir/resourcebundle
    $ext_builddir/spoofchecker
    $ext_builddir/timezone
    $ext_builddir/transliterator
    $ext_builddir/uchar
  ])

  PHP_ADD_EXTENSION_DEP(intl, date)
fi
