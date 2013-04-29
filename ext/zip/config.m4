dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(zip, for zip archive read/writesupport,
[  --enable-zip            Include Zip read/write support])

if test -z "$PHP_ZLIB_DIR"; then
  PHP_ARG_WITH(zlib-dir, for the location of libz,
  [  --with-zlib-dir[=DIR]     ZIP: Set the path to libz install prefix], no, no)
fi

PHP_ARG_WITH(pcre-dir, pcre install prefix,
[  --with-pcre-dir           ZIP: pcre install prefix], no, no)

if test "$PHP_ZIP" != "no"; then

  if test "$PHP_ZLIB_DIR" != "no" && test "$PHP_ZLIB_DIR" != "yes"; then
    if test -f "$PHP_ZLIB_DIR/include/zlib/zlib.h"; then
      PHP_ZLIB_DIR="$PHP_ZLIB_DIR"
      PHP_ZLIB_INCDIR="$PHP_ZLIB_DIR/include/zlib"
    elif test -f "$PHP_ZLIB_DIR/include/zlib.h"; then
      PHP_ZLIB_DIR="$PHP_ZLIB_DIR"
      PHP_ZLIB_INCDIR="$PHP_ZLIB_DIR/include"
    else
      AC_MSG_ERROR([Can not find zlib headers under "$PHP_ZLIB_DIR"])
    fi
  else
    for i in /usr/local /usr; do
      if test -f "$i/include/zlib/zlib.h"; then
        PHP_ZLIB_DIR="$i"
        PHP_ZLIB_INCDIR="$i/include/zlib"
      elif test -f "$i/include/zlib.h"; then
        PHP_ZLIB_DIR="$i"
        PHP_ZLIB_INCDIR="$i/include"
      fi
    done
  fi

  dnl # zlib
  AC_MSG_CHECKING([for the location of zlib])
  if test "$PHP_ZLIB_DIR" = "no"; then
    AC_MSG_ERROR([zip support requires ZLIB. Use --with-zlib-dir=<DIR> to specify prefix where ZLIB include and library are located])
  else
    AC_MSG_RESULT([$PHP_ZLIB_DIR])
    PHP_ADD_LIBRARY_WITH_PATH(z, $PHP_ZLIB_DIR/$PHP_LIBDIR, ZIP_SHARED_LIBADD)
    PHP_ADD_INCLUDE($PHP_ZLIB_INCDIR)
  fi

  dnl This is PECL build, check if bundled PCRE library is used
  old_CPPFLAGS=$CPPFLAGS
  CPPFLAGS=$INCLUDES
  AC_EGREP_CPP(yes,[
#include <main/php_config.h>
#if defined(HAVE_BUNDLED_PCRE) && !defined(COMPILE_DL_PCRE)
yes
#endif
  ],[
    PHP_PCRE_REGEX=yes
  ],[
    AC_EGREP_CPP(yes,[
#include <main/php_config.h>
#if defined(HAVE_PCRE) && !defined(COMPILE_DL_PCRE)
yes
#endif
    ],[
      PHP_PCRE_REGEX=pecl
    ],[
      PHP_PCRE_REGEX=no
    ])
  ])
  CPPFLAGS=$old_CPPFLAGS

  PHP_ZIP_SOURCES="$PHP_ZIP_SOURCES lib/zip_add.c lib/zip_error.c lib/zip_fclose.c \
                         lib/zip_fread.c lib/zip_open.c lib/zip_source_filep.c  \
                         lib/zip_strerror.c lib/zip_close.c lib/zip_error_get.c \
                         lib/zip_file_error_get.c lib/zip_free.c lib/zip_rename.c \
                         lib/zip_source_free.c lib/zip_unchange_all.c lib/zip_delete.c \
                         lib/zip_error_get_sys_type.c lib/zip_file_get_offset.c \
                         lib/zip_get_name.c lib/zip_replace.c lib/zip_source_function.c \
                         lib/zip_unchange.c lib/zip_dirent.c lib/zip_error_strerror.c \
                         lib/zip_filerange_crc.c lib/zip_file_strerror.c lib/zip_get_num_files.c \
                         lib/zip_get_archive_flag.c lib/zip_set_archive_flag.c \
                         lib/zip_set_name.c lib/zip_source_zip.c lib/zip_unchange_data.c \
                         lib/zip_entry_free.c lib/zip_error_to_str.c lib/zip_fopen.c \
                         lib/zip_name_locate.c lib/zip_source_buffer.c lib/zip_stat.c \
                         lib/zip_entry_new.c lib/zip_err_str.c lib/zip_fopen_index.c \
                         lib/zip_get_archive_comment.c lib/zip_get_file_comment.c \
                         lib/zip_new.c lib/zip_source_file.c lib/zip_stat_index.c \
                         lib/zip_set_archive_comment.c lib/zip_set_file_comment.c \
                         lib/zip_unchange_archive.c lib/zip_memdup.c lib/zip_stat_init.c lib/zip_add_dir.c \
                         lib/zip_error_clear.c lib/zip_file_error_clear.c \
						 lib/zip_fdopen.c lib/zip_fopen_encrypted.c lib/zip_fopen_index_encrypted.c \
						 lib/zip_get_compression_implementation.c lib/zip_get_encryption_implementation.c \
						 lib/zip_get_file_extra.c lib/zip_get_num_entries.c lib/zip_set_default_password.c \
						 lib/zip_set_file_extra.c lib/zip_source_close.c lib/zip_source_crc.c \
						 lib/zip_source_deflate.c lib/zip_source_error.c lib/zip_source_layered.c \
						 lib/zip_source_open.c lib/zip_source_pkware.c lib/zip_source_pop.c \
						 lib/zip_source_read.c lib/zip_source_stat.c"

  AC_DEFINE(HAVE_ZIP,1,[ ])
  PHP_NEW_EXTENSION(zip, php_zip.c zip_stream.c $PHP_ZIP_SOURCES, $ext_shared)
  PHP_ADD_BUILD_DIR($ext_builddir/lib, 1)
  PHP_SUBST(ZIP_SHARED_LIBADD)

  dnl so we always include the known-good working hack.
  PHP_ADD_MAKEFILE_FRAGMENT
fi
