PHP_NEW_EXTENSION(json,
	  json.c \
	  json_encoder.c \
	  json_parser.tab.c \
	  json_scanner.c,
	  no,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
PHP_INSTALL_HEADERS([ext/json], [php_json.h php_json_parser.h php_json_scanner.h])
PHP_ADD_MAKEFILE_FRAGMENT()
