dnl Check if embed SAPI is enable before running CLI SAPI feature checks, they are required for embed's do_php_cli too
PHP_ARG_ENABLE([embed],,
  [AS_HELP_STRING([[--enable-embed[=TYPE]]],
    [Enable building of embedded SAPI library TYPE is either
    'shared' or 'static'. [TYPE=shared]])],
  [no],
  [no])
