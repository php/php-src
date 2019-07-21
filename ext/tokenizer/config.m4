PHP_ARG_ENABLE([tokenizer],
  [whether to enable tokenizer support],
  [AS_HELP_STRING([--disable-tokenizer],
    [Disable tokenizer support])],
  [yes])

if test "$PHP_TOKENIZER" != "no"; then
  AC_DEFINE(HAVE_TOKENIZER, 1, [Define to 1 if you have the tokenizer extension enabled.])
  PHP_NEW_EXTENSION(tokenizer, tokenizer.c tokenizer_data.c, $ext_shared)
  PHP_ADD_MAKEFILE_FRAGMENT
fi
