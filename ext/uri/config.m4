dnl Configure options
dnl
PHP_ARG_ENABLE([uri],
  [for URI support],
  [AS_HELP_STRING([--enable-uri],
    [Enable URI support])])

if test "$PHP_URI" != "no"; then
    PHP_LEXBOR_CFLAGS="-I$abs_srcdir/ext/dom/lexbor -DLEXBOR_STATIC -I$abs_srcdir/ext/uri/uriparser/include"

    AC_DEFINE(HAVE_URI, 1, [Whether the uri extension is enabled])
    PHP_INSTALL_HEADERS([ext/uri], m4_normalize([
      php_uri.h
      php_uri_common.h
    ]))

    URIPARSER_DIR="uriparser"
    URIPARSER_SOURCES="$URIPARSER_DIR/src/UriCommon.c $URIPARSER_DIR/src/UriCompare.c $URIPARSER_DIR/src/UriEscape.c \
    $URIPARSER_DIR/src/UriFile.c $URIPARSER_DIR/src/UriIp4.c $URIPARSER_DIR/src/UriIp4Base.c \
    $URIPARSER_DIR/src/UriMemory.c $URIPARSER_DIR/src/UriNormalize.c $URIPARSER_DIR/src/UriNormalizeBase.c \
    $URIPARSER_DIR/src/UriParse.c $URIPARSER_DIR/src/UriParseBase.c $URIPARSER_DIR/src/UriQuery.c \
    $URIPARSER_DIR/src/UriRecompose.c $URIPARSER_DIR/src/UriResolve.c $URIPARSER_DIR/src/UriShorten.c"

    PHP_ADD_EXTENSION_DEP(uri, dom)
    PHP_NEW_EXTENSION(uri, [php_uri.c php_uri_common.c php_lexbor.c php_uriparser.c $URIPARSER_SOURCES], $ext_shared,,$PHP_LEXBOR_CFLAGS)
    PHP_ADD_BUILD_DIR($ext_builddir/$URIPARSER_DIR/include)
fi
