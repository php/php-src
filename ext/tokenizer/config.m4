PHP_ARG_ENABLE([tokenizer],
  [whether to enable tokenizer support],
  [AS_HELP_STRING([--disable-tokenizer],
    [Disable tokenizer support])],
  [yes])

if test "$PHP_TOKENIZER" != "no"; then
  PHP_NEW_EXTENSION([tokenizer], [tokenizer.c tokenizer_data.c], [$ext_shared],,
    [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
  PHP_ADD_MAKEFILE_FRAGMENT
fi
