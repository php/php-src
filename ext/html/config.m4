dnl Always enabled and never shared: the native markup syntax in the Zend
dnl scanner/parser is unconditional and lowers into the Html\ symbols this
dnl extension registers, so a build without it would accept the syntax and then
dnl fail at runtime (compare ext/random). See config.w32 for the Windows side.
PHP_NEW_EXTENSION([html], [html.c], [no])
PHP_INSTALL_HEADERS([ext/html], [php_html.h])
PHP_ADD_EXTENSION_DEP(html, standard)
