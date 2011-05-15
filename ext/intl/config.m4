dnl config.m4 for extension intl

dnl ##########################################################################
dnl Initialize the extension
PHP_ARG_ENABLE(intl, whether to enable internationalization support,
[  --enable-intl           Enable internationalization support])

if test "$PHP_INTL" != "no"; then
  PHP_SETUP_ICU(INTL_SHARED_LIBADD)
  PHP_SUBST(INTL_SHARED_LIBADD)
  PHP_REQUIRE_CXX()
  if test "$icu_version" -ge "4002"; then
    icu_spoof_src=" spoofchecker/spoofchecker_class.c \
    spoofchecker/spoofchecker.c\
    spoofchecker/spoofchecker_create.c\
    spoofchecker/spoofchecker_main.c"
  else
    icu_spoof_src=""
  fi
  PHP_NEW_EXTENSION(intl, php_intl.c \
    intl_error.c \
    intl_convert.c \
    collator/collator.c \
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
    formatter/formatter.c \
    formatter/formatter_main.c \
    formatter/formatter_class.c \
    formatter/formatter_attr.c \
    formatter/formatter_data.c \
    formatter/formatter_format.c \
    formatter/formatter_parse.c \
    normalizer/normalizer.c \
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
    msgformat/msgformat_helpers.cpp \
    msgformat/msgformat_parse.c \
    grapheme/grapheme_string.c \
    grapheme/grapheme_util.c \
    resourcebundle/resourcebundle.c \
    resourcebundle/resourcebundle_class.c \
    resourcebundle/resourcebundle_iterator.c \
    transliterator/transliterator.c \
    transliterator/transliterator_class.c \
    transliterator/transliterator_methods.c \
    idn/idn.c \
    $icu_spoof_src, $ext_shared,,$ICU_INCS)
  PHP_ADD_BUILD_DIR($ext_builddir/collator)
  PHP_ADD_BUILD_DIR($ext_builddir/common)
  PHP_ADD_BUILD_DIR($ext_builddir/formatter)
  PHP_ADD_BUILD_DIR($ext_builddir/normalizer)
  PHP_ADD_BUILD_DIR($ext_builddir/dateformat)
  PHP_ADD_BUILD_DIR($ext_builddir/locale)
  PHP_ADD_BUILD_DIR($ext_builddir/msgformat)
  PHP_ADD_BUILD_DIR($ext_builddir/grapheme)
  PHP_ADD_BUILD_DIR($ext_builddir/resourcebundle)
  PHP_ADD_BUILD_DIR($ext_builddir/transliterator)
  PHP_ADD_BUILD_DIR($ext_builddir/idn)
  PHP_ADD_BUILD_DIR($ext_builddir/spoofchecker)
fi
