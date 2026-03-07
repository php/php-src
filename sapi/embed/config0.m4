PHP_ARG_ENABLE([embed],,
  [AS_HELP_STRING([[--enable-embed[=TYPE]]],
    [Enable building of embedded SAPI library TYPE is either
    'shared' or 'static'. [TYPE=shared]])],
  [no],
  [no])

dnl Embed SAPI reuses CLI object files, so force CLI on.
if test "$PHP_EMBED" != "no"; then
  enable_cli=yes
fi
