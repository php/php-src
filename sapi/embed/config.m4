dnl
dnl $Id$
dnl

AC_MSG_CHECKING(for embedded SAPI library support)

AC_ARG_ENABLE(embed,
[  --enable-embed[=TYPE]   EXPERIMENTAL: Enable building of embedded SAPI library
                          TYPE is either 'shared' or 'static'. [TYPE=shared]],
[ 
  case $enableval in
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
],[
  PHP_EMBED_TYPE=no
])

AC_MSG_RESULT($PHP_EMBED_TYPE)

if test "$PHP_EMBED_TYPE" != "no"; then
  PHP_SELECT_SAPI(embed, $PHP_EMBED_TYPE, php_embed.c)
fi
