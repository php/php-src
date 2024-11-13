PHP_NEW_EXTENSION([json], m4_normalize([
    json_encoder.c
    json_parser.tab.c
    json_scanner.c
    json.c
  ]),
  [no],,
  [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
PHP_INSTALL_HEADERS([ext/json], m4_normalize([
  php_json_parser.h
  php_json_scanner.h
  php_json.h
]))
PHP_ADD_MAKEFILE_FRAGMENT
