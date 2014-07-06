dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(embed,,
[  --enable-embed[=TYPE]     EXPERIMENTAL: Enable building of embedded SAPI library
                          TYPE is either 'shared' or 'static'. [TYPE=shared]], no, no)

AC_MSG_CHECKING([for embedded SAPI library support])

if test "$PHP_EMBED" != "no"; then
  case "$PHP_EMBED" in
    yes|shared)
      PHP_EMBED_TYPE=shared
      INSTALL_IT="\$(mkinstalldirs) \$(INSTALL_ROOT)\$(prefix)/lib; \$(INSTALL) -m 0755 $SAPI_SHARED \$(INSTALL_ROOT)\$(prefix)/lib"
      ;;
    static)
      PHP_EMBED_TYPE=static
      INSTALL_IT="\$(mkinstalldirs) \$(INSTALL_ROOT)\$(prefix)/lib; \$(INSTALL) -m 0644 $SAPI_STATIC \$(INSTALL_ROOT)\$(prefix)/lib"
      ;;
    *)
      PHP_EMBED_TYPE=no
      ;;
  esac
  if test "$PHP_EMBED_TYPE" != "no"; then
    PHP_SELECT_SAPI(embed, $PHP_EMBED_TYPE, php_embed.c)
    PHP_INSTALL_HEADERS([sapi/embed/php_embed.h])
  fi
  AC_MSG_RESULT([$PHP_EMBED_TYPE])
else
  AC_MSG_RESULT(no)
fi

