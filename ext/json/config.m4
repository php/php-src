dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(json, whether to enable JavaScript Object Serialization support,
[  --disable-json          Disable JavaScript Object Serialization support], yes)

if test "$PHP_JSON" != "no"; then
  AC_DEFINE([HAVE_JSON],1 ,[whether to enable JavaScript Object Serialization support])
  AC_HEADER_STDC

PHP_NEW_EXTENSION(json,
	  json.c \
	  json_encoder.c \
	  json_parser.tab.c \
	  json_scanner.c,
	  $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  PHP_INSTALL_HEADERS([ext/json], [php_json.h])
  PHP_ADD_MAKEFILE_FRAGMENT()
  PHP_SUBST(JSON_SHARED_LIBADD)
fi
