PHP_ARG_ENABLE([intl],
  [whether to enable internationalization support],
  [AS_HELP_STRING([--enable-intl],
    [Enable internationalization support])])

if test "$PHP_INTL" != "no"; then
  PHP_SETUP_ICU([INTL_SHARED_LIBADD])
  PHP_SUBST([INTL_SHARED_LIBADD])
  INTL_COMMON_FLAGS="$ICU_CFLAGS -Wno-write-strings -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1"
  PHP_NEW_EXTENSION([intl], m4_normalize([
      intl_convert.cpp
      intl_error.cpp
      php_intl.cpp
    ]),
    [$ext_shared],,
    [$INTL_COMMON_FLAGS],
    [cxx])

  PHP_INTL_CXX_SOURCES="collator/collator_attr.cpp \
    collator/collator_class.cpp \
    collator/collator_compare.cpp \
    collator/collator_convert.cpp \
    collator/collator_create.cpp \
    collator/collator_error.cpp \
    collator/collator_is_numeric.cpp \
    collator/collator_locale.cpp \
    collator/collator_sort.cpp \
    common/common_enum.cpp \
    common/common_date.cpp \
    common/common_error.cpp \
    dateformat/dateformat_class.cpp \
    converter/converter.cpp \
    dateformat/dateformat.cpp \
    dateformat/dateformat_attr.cpp \
    dateformat/dateformat_attrcpp.cpp \
    dateformat/dateformat_create.cpp \
    dateformat/dateformat_data.cpp \
    dateformat/dateformat_format.cpp \
    dateformat/dateformat_format_object.cpp \
    dateformat/dateformat_helpers.cpp \
    dateformat/dateformat_parse.cpp \
    dateformat/datepatterngenerator_class.cpp \
    dateformat/datepatterngenerator_methods.cpp \
    formatter/formatter_class.cpp \
    grapheme/grapheme_string.cpp \
    grapheme/grapheme_util.cpp \
    msgformat/msgformat_helpers.cpp \
    rangeformatter/rangeformatter_class.cpp \
    timezone/timezone_class.cpp \
    timezone/timezone_methods.cpp \
    calendar/calendar_class.cpp \
    calendar/calendar_methods.cpp \
    calendar/gregoriancalendar_methods.cpp \
    formatter/formatter_attr.cpp \
    formatter/formatter_data.cpp \
    formatter/formatter_format.cpp \
    formatter/formatter_main.cpp \
    formatter/formatter_parse.cpp \
    msgformat/msgformat_attr.cpp \
    msgformat/msgformat_class.cpp \
    msgformat/msgformat_data.cpp \
    msgformat/msgformat_format.cpp \
    msgformat/msgformat_parse.cpp \
    msgformat/msgformat.cpp \
    normalizer/normalizer_class.cpp \
    normalizer/normalizer_normalize.cpp \
    breakiterator/breakiterator_class.cpp \
    breakiterator/breakiterator_iterators.cpp \
    breakiterator/breakiterator_methods.cpp \
    breakiterator/rulebasedbreakiterator_methods.cpp \
    breakiterator/codepointiterator_internal.cpp \
    breakiterator/codepointiterator_methods.cpp \
    listformatter/listformatter_class.cpp \
    transliterator/transliterator_class.cpp \
    transliterator/transliterator_methods.cpp \
    idn/idn.cpp \
    locale/locale_class.cpp \
    locale/locale_methods.cpp \
    locale/locale.cpp \
    resourcebundle/resourcebundle_class.cpp \
    resourcebundle/resourcebundle_iterator.cpp \
    resourcebundle/resourcebundle.cpp \
    spoofchecker/spoofchecker_class.cpp \
    spoofchecker/spoofchecker_create.cpp \
    spoofchecker/spoofchecker_main.cpp \
    uchar/uchar.cpp"

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
    $ext_builddir/listformatter
    $ext_builddir/msgformat
    $ext_builddir/normalizer
    $ext_builddir/rangeformatter
    $ext_builddir/resourcebundle
    $ext_builddir/spoofchecker
    $ext_builddir/timezone
    $ext_builddir/transliterator
    $ext_builddir/uchar
  ])

  PHP_ADD_EXTENSION_DEP(intl, date)
fi
