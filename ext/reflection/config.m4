dnl config.m4 for extension reflection

PHP_NEW_EXTENSION(reflection, php_reflection.c, no,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
