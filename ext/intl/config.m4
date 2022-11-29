PHP_ARG_ENABLE([intl],
  [whether to enable internationalization support],
  [AS_HELP_STRING([--enable-intl],
    [Enable internationalization support])])

if test "$PHP_INTL" != "no"; then
  PHP_SETUP_ICU(INTL_SHARED_LIBADD)
  PHP_SUBST(INTL_SHARED_LIBADD)
  INTL_COMMON_FLAGS="$ICU_CFLAGS -Wno-write-strings -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1"
  PHP_NEW_EXTENSION(intl, php_intl.c \
    intl_error.c \
    intl_convert.c \
    collator/collator_class.c \
    collator/collator_sort.c \
    collator/collator_convert.c \
    collator/collator_locale.c \
    collator/collator_compare.c \
    collator/collator_attr.c \
    collator/collator_create.c \
    collator/collator_is_numeric.c \
    collator/collator_error.c \
    common/common_error.c \
    converter/converter.c \
    formatter/formatter_main.c \
    formatter/formatter_class.c \
    formatter/formatter_attr.c \
    formatter/formatter_data.c \
    formatter/formatter_format.c \
    formatter/formatter_parse.c \
    normalizer/normalizer_class.c \
    normalizer/normalizer_normalize.c \
    locale/locale.c \
    locale/locale_class.c \
    locale/locale_methods.c \
    dateformat/dateformat.c \
    dateformat/dateformat_class.c \
    dateformat/dateformat_attr.c \
    dateformat/dateformat_data.c \
    dateformat/dateformat_format.c \
    dateformat/dateformat_parse.c \
    msgformat/msgformat.c \
    msgformat/msgformat_attr.c \
    msgformat/msgformat_class.c \
    msgformat/msgformat_data.c  \
    msgformat/msgformat_format.c \
    msgformat/msgformat_parse.c \
    grapheme/grapheme_string.c \
    grapheme/grapheme_util.c \
    resourcebundle/resourcebundle.c \
    resourcebundle/resourcebundle_class.c \
    resourcebundle/resourcebundle_iterator.c \
    transliterator/transliterator_class.c \
    transliterator/transliterator_methods.c \
    uchar/uchar.c \
    idn/idn.c \
    spoofchecker/spoofchecker_class.c \
    spoofchecker/spoofchecker_create.c\
    spoofchecker/spoofchecker_main.c, $ext_shared,,$INTL_COMMON_FLAGS,cxx)

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
  PHP_CXX_COMPILE_STDCXX(11, mandatory, PHP_INTL_STDCXX)
  PHP_INTL_CXX_FLAGS="$INTL_COMMON_FLAGS $PHP_INTL_STDCXX $ICU_CXXFLAGS"
  case $host_alias in
  *cygwin*) PHP_INTL_CXX_FLAGS="$PHP_INTL_CXX_FLAGS -D_POSIX_C_SOURCE=200809L"
  esac
  if test "$ext_shared" = "no"; then
    PHP_ADD_SOURCES(PHP_EXT_DIR(intl), $PHP_INTL_CXX_SOURCES, $PHP_INTL_CXX_FLAGS)
  else
    PHP_ADD_SOURCES_X(PHP_EXT_DIR(intl), $PHP_INTL_CXX_SOURCES, $PHP_INTL_CXX_FLAGS, shared_objects_intl, yes)
  fi

  PHP_ADD_BUILD_DIR($ext_builddir/collator)
  PHP_ADD_BUILD_DIR($ext_builddir/converter)
  PHP_ADD_BUILD_DIR($ext_builddir/common)
  PHP_ADD_BUILD_DIR($ext_builddir/formatter)
  PHP_ADD_BUILD_DIR($ext_builddir/normalizer)
  PHP_ADD_BUILD_DIR($ext_builddir/dateformat)
  PHP_ADD_BUILD_DIR($ext_builddir/locale)
  PHP_ADD_BUILD_DIR($ext_builddir/msgformat)
  PHP_ADD_BUILD_DIR($ext_builddir/grapheme)
  PHP_ADD_BUILD_DIR($ext_builddir/resourcebundle)
  PHP_ADD_BUILD_DIR($ext_builddir/transliterator)
  PHP_ADD_BUILD_DIR($ext_builddir/timezone)
  PHP_ADD_BUILD_DIR($ext_builddir/calendar)
  PHP_ADD_BUILD_DIR($ext_builddir/idn)
  PHP_ADD_BUILD_DIR($ext_builddir/spoofchecker)
  PHP_ADD_BUILD_DIR($ext_builddir/breakiterator)
  PHP_ADD_BUILD_DIR($ext_builddir/uchar)
fi
