dnl
dnl $Id$
dnl

PHP_ARG_WITH(mime-magic, whether to include mime_magic support,
[  --with-mime-magic[=FILE]  Include mime_magic support. FILE is the optional
                            pathname to the magic.mime file.])

if test "$PHP_MIME_MAGIC" != "no"; then

  PHP_NEW_EXTENSION(mime_magic, mime_magic.c, $ext_shared)

  PHP_MIME_MAGIC_FILE_PATH=

  dnl
  dnl Try to see if we can find the path of the magic file in its default locations.
  dnl
  if test "$PHP_MIME_MAGIC" != "yes"; then
    if test -f $PHP_MIME_MAGIC; then
      PHP_MIME_MAGIC_FILE_PATH=$PHP_MIME_MAGIC
    else
      AC_MSG_ERROR([File '$PHP_MIME_MAGIC' not found!])
    fi
  else
    MAGIC_MIME_LOCATIONS="/usr/local/apache/conf/magic /usr/local/etc/apache/conf/magic /etc/apache/magic /etc/httpd/conf/magic /usr/share/magic.mime /usr/share/misc/magic.mime /etc/magic.mime"

    for i in $MAGIC_MIME_LOCATIONS; do
      if test -f $i; then
        PHP_MIME_MAGIC_FILE_PATH=$i
        break
      fi
    done
  fi
  
  AC_DEFINE_UNQUOTED(PHP_MIME_MAGIC_FILE_PATH,"$PHP_MIME_MAGIC_FILE_PATH",[magic file path])

fi
