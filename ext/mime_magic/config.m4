dnl $Id$
dnl config.m4 for extension mime_magic

PHP_ARG_ENABLE(mime_magic, whether to enable mime_magic support,
[  --enable-mime_magic           Enable mime_magic support])

if test "$PHP_MIME_MAGIC" != "no"; then
  dnl PHP_SUBST(MIME_MAGIC_SHARED_LIBADD)

  PHP_NEW_EXTENSION(mime_magic, mime_magic.c, $ext_shared)
fi
