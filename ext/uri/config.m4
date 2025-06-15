dnl Configure options
dnl

PHP_INSTALL_HEADERS([ext/uri], m4_normalize([
  php_lexbor.h
  php_uri.h
  php_uri_common.h
  php_uriparser.h
]))

AC_DEFINE([URI_ENABLE_ANSI], [1], [Define to 1 for enabling ANSI support of uriparser.])
AC_DEFINE([URI_NO_UNICODE], [1], [Define to 1 for disabling unicode support of uriparser.])

URIPARSER_DIR="uriparser"
URIPARSER_SOURCES="$URIPARSER_DIR/src/UriCommon.c $URIPARSER_DIR/src/UriCompare.c $URIPARSER_DIR/src/UriEscape.c \
$URIPARSER_DIR/src/UriFile.c $URIPARSER_DIR/src/UriIp4.c $URIPARSER_DIR/src/UriIp4Base.c \
$URIPARSER_DIR/src/UriMemory.c $URIPARSER_DIR/src/UriNormalize.c $URIPARSER_DIR/src/UriNormalizeBase.c \
$URIPARSER_DIR/src/UriParse.c $URIPARSER_DIR/src/UriParseBase.c $URIPARSER_DIR/src/UriQuery.c \
$URIPARSER_DIR/src/UriRecompose.c $URIPARSER_DIR/src/UriResolve.c $URIPARSER_DIR/src/UriShorten.c"

PHP_NEW_EXTENSION(uri, [php_lexbor.c php_uri.c php_uri_common.c php_uriparser.c $URIPARSER_SOURCES], [no],,[-I$ext_srcdir/$URIPARSER_DIR/include -DURI_STATIC_BUILD -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
PHP_ADD_EXTENSION_DEP(uri, lexbor)
PHP_ADD_BUILD_DIR($ext_builddir/$URIPARSER_DIR/src $ext_builddir/$URIPARSER_DIR/include)
