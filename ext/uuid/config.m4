dnl Configure options
dnl

PHP_INSTALL_HEADERS([ext/uuid], m4_normalize([
  php_uuid.h
  uuidv7-h/php_uuid.h
]))

PHP_NEW_EXTENSION(uuid, [php_uuid.c], [no],,[-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
PHP_ADD_EXTENSION_DEP(uuid, date)
PHP_ADD_EXTENSION_DEP(uuid, random)
