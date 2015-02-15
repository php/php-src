dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(json,
  whether to enable JavaScript Object Serialization support,
  [AS_HELP_STRING([--disable-json], [Disable JavaScript Object Serialization support])],
  yes)

AC_ARG_ENABLE(json-filegen,
  [AS_HELP_STRING([--enable-json-filegen], [Enable new Json file generation])],
  [PHP_JSON_FILEGEN=yes], [PHP_JSON_FILEGEN=no])

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

  if test "$PHP_JSON_FILEGEN" != "no"; then
    if test -z "$re2c_vernum" || test "$re2c_vernum" -lt "1306"; then
      AC_MSG_WARN([You will need re2c 0.13.6 or later if you want to regenerate JSON scanner.])
      RE2C_FOR_JSON="exit 0;"
    else
      RE2C_FOR_JSON="$RE2C"
    fi
    PHP_SUBST(RE2C_FOR_JSON)
    PHP_ADD_MAKEFILE_FRAGMENT()
  fi
  PHP_SUBST(JSON_SHARED_LIBADD)
fi
