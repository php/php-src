dnl $Id$
dnl config.m4 for extension fileinfo

PHP_ARG_WITH(fileinfo, for fileinfo support,
[  --with-fileinfo=DIR   Include fileinfo support])

if test "$PHP_FILEINFO" != "no"; then
  MAGIC_MIME_DIRS="/usr/local/share/file /usr/share/file /usr/share/misc/file /etc /usr/share/misc"
  MAGIC_MIME_FILENAMES="magic magic.mime"
  
  for i in $MAGIC_MIME_DIRS; do
    for j in $MAGIC_MIME_FILENAMES; do
      if test -f "$i/$j"; then
        PHP_DEFAULT_MAGIC_FILE="$i/$j"
        break
      fi
    done
  done

  AC_CHECK_FUNCS([utimes])
  
  AC_DEFINE_UNQUOTED(PHP_DEFAULT_MAGIC_FILE,"$PHP_DEFAULT_MAGIC_FILE",[magic file path])

  PHP_SUBST(FILEINFO_SHARED_LIBADD)

  PHP_NEW_EXTENSION(fileinfo, fileinfo.c lib/apprentice.c lib/apptype.c lib/ascmagic.c lib/compress.c lib/file.c lib/fsmagic.c lib/funcs.c lib/getopt_long.c lib/is_tar.c lib/magic.c lib/print.c lib/readelf.c lib/softmagic.c, $ext_shared)

if test "$ext_shared" = "no" || test "$ext_srcdir" != "$abs_srcdir"; then
  echo '#include <php_config.h>' > $ext_builddir/libsqlite/src/config.h
else
  echo "#include \"$abs_builddir/config.h\"" > $ext_builddir/libsqlite/src/config.h
fi

fi
