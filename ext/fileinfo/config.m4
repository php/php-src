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

  AC_DEFINE_UNQUOTED(PHP_DEFAULT_MAGIC_FILE,"$PHP_DEFAULT_MAGIC_FILE",[magic file path])

  libmagic_sources=" \
    libmagic/apprentice.c libmagic/apptype.c libmagic/ascmagic.c \
    libmagic/compress.c libmagic/fsmagic.c libmagic/funcs.c \
    libmagic/getopt_long.c libmagic/is_tar.c libmagic/magic.c libmagic/print.c \
    libmagic/readelf.c libmagic/softmagic.c"

  PHP_NEW_EXTENSION(fileinfo, fileinfo.c $libmagic_sources, $ext_shared,,-I@ext_srcdir@/libmagic)
  PHP_SUBST(FILEINFO_SHARED_LIBADD)
  PHP_ADD_BUILD_DIR($ext_builddir/libmagic)

  AC_CHECK_FUNCS([utimes])
fi
