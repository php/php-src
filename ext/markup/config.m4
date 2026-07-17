dnl Always enabled and never shared: the native markup syntax in the Zend
dnl scanner/parser is unconditional and lowers into the Markup\ symbols this
dnl extension registers, so a build without it would accept the syntax and then
dnl fail at runtime (compare ext/random). See config.w32 for the Windows side.
PHP_NEW_EXTENSION([markup], [markup.c], [no])
PHP_INSTALL_HEADERS([ext/markup], [php_markup.h])
PHP_ADD_EXTENSION_DEP(markup, standard)
