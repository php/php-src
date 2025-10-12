dnl Configure options
dnl

PHP_ARG_WITH([external-uriparser],
  [for external/system liburiparser],
  [AS_HELP_STRING([--with-external-uriparser],
    [Use external/system liburiparser])],
  [no],
  [no])

PHP_INSTALL_HEADERS([ext/uri], m4_normalize([
  php_uri.h
  php_uri_common.h
  uri_parser_rfc3986.h
  uri_parser_whatwg.h
  uri_parser_php_parse_url.h
]))


AC_DEFINE([URI_ENABLE_ANSI], [1], [Define to 1 for enabling ANSI support of uriparser.])
AC_DEFINE([URI_NO_UNICODE], [1], [Define to 1 for disabling unicode support of uriparser.])

if test "$PHP_EXTERNAL_URIPARSER" = "no"; then
  URIPARSER_DIR="uriparser"
  URIPARSER_SOURCES="$URIPARSER_DIR/src/UriCommon.c $URIPARSER_DIR/src/UriCompare.c $URIPARSER_DIR/src/UriCopy.c \
  $URIPARSER_DIR/src/UriEscape.c $URIPARSER_DIR/src/UriFile.c $URIPARSER_DIR/src/UriIp4.c $URIPARSER_DIR/src/UriIp4Base.c \
  $URIPARSER_DIR/src/UriMemory.c $URIPARSER_DIR/src/UriNormalize.c $URIPARSER_DIR/src/UriNormalizeBase.c \
  $URIPARSER_DIR/src/UriParse.c $URIPARSER_DIR/src/UriParseBase.c $URIPARSER_DIR/src/UriQuery.c \
  $URIPARSER_DIR/src/UriRecompose.c $URIPARSER_DIR/src/UriResolve.c $URIPARSER_DIR/src/UriSetFragment.c \
  $URIPARSER_DIR/src/UriSetHostAuto.c $URIPARSER_DIR/src/UriSetHostCommon.c $URIPARSER_DIR/src/UriSetHostIp4.c \
  $URIPARSER_DIR/src/UriSetHostIp6.c $URIPARSER_DIR/src/UriSetHostIpFuture.c $URIPARSER_DIR/src/UriSetHostRegName.c \
  $URIPARSER_DIR/src/UriSetPath.c $URIPARSER_DIR/src/UriSetPort.c $URIPARSER_DIR/src/UriSetQuery.c \
  $URIPARSER_DIR/src/UriSetScheme.c $URIPARSER_DIR/src/UriSetUserInfo.c $URIPARSER_DIR/src/UriShorten.c $URIPARSER_DIR/src/UriVersion.c"
  URI_CFLAGS="-DURI_STATIC_BUILD"
else
  PKG_CHECK_MODULES([LIBURIPARSER], [liburiparser >= 0.9.10])
  PHP_EVAL_LIBLINE([$LIBURIPARSER_LIBS], [URI_SHARED_LIBADD])
  PHP_EVAL_INCLINE([$LIBURIPARSER_CFLAGS])
fi

PHP_NEW_EXTENSION(uri, [php_uri.c php_uri_common.c uri_parser_rfc3986.c uri_parser_whatwg.c uri_parser_php_parse_url.c $URIPARSER_SOURCES], [no],,[$URI_CFLAGS -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
PHP_ADD_EXTENSION_DEP(uri, lexbor)

if test "$PHP_EXTERNAL_URIPARSER" = "no"; then
  PHP_ADD_BUILD_DIR($ext_builddir/$URIPARSER_DIR/src $ext_builddir/$URIPARSER_DIR/include)
  PHP_ADD_INCLUDE([$ext_srcdir/$URIPARSER_DIR/include])
fi
