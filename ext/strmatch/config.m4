PHP_ARG_ENABLE([strmatch],
  [whether to enable strmatch support],
  [AS_HELP_STRING([--enable-strmatch],
    [Enable multi-pattern string matching (Str\Matcher / Str\Replacer)])],
  [no])

AS_VAR_IF([PHP_STRMATCH], [no],, [
  AC_DEFINE([HAVE_STRMATCH], [1],
    [Define to 1 if the PHP extension strmatch is available.])
  PHP_NEW_EXTENSION([strmatch], [strmatch.c], [$ext_shared])
])
