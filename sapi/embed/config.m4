dnl
dnl $Id$
dnl

AC_MSG_CHECKING(for embedded SAPI library support)

AC_ARG_ENABLE(embed,
[  --enable-embed[=TYPE]   Enable building embedded SAPI library of PHP
                          TYPE is either 'shared' or 'static'. Defaults to 'static' library.],
[ 
  case $enableval in
    yes|shared)
      PHP_EMBED_TYPE=shared
      ;;
    static)
      PHP_EMBED_TYPE=static
      ;;
    *)
      PHP_EMBED_TYPE=no
      ;;
  esac
],[
  PHP_EMBED=no
])

AC_MSG_RESULT($PHP_EMBED_TYPE)

if test "$PHP_EMBED_TYPE" != "no"; then
  PHP_SELECT_SAPI(embed, $PHP_EMBED_TYPE, php_embed.c)
fi
