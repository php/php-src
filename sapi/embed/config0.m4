PHP_ARG_ENABLE([embed],,
  [AS_HELP_STRING([[--enable-embed[=TYPE]]],
    [Enable building of embedded SAPI library TYPE is either
    'shared' or 'static'. [TYPE=shared]])],
  [no],
  [no])
