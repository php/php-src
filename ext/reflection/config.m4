dnl config.m4 for extension reflection

AC_DEFINE(HAVE_REFLECTION, 1, [Whether Reflection is enabled])
PHP_NEW_EXTENSION(reflection, php_reflection.c, no,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
