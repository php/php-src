dnl $Id$
dnl config.m4 for extension phar

PHP_ARG_ENABLE(phar, for phar support/phar zlib support,
[  --enable-phar           Enable phar support])

PHP_ARG_WITH(phar-zip, for zip-based phar support,
[  --without-phar-zip        PHAR: Disable zip-based phar archive support], no, no)

if test "$PHP_PHAR" != "no"; then
  PHP_NEW_EXTENSION(phar, tar.c zip.c stream.c func_interceptors.c dirstream.c phar.c phar_object.c phar_path_check.c, $ext_shared)
	AC_MSG_CHECKING([for zip-based phar support])
	if test "$PHP_PHAR_ZIP" != "yes"; then
		AC_MSG_RESULT([yes])
		PHP_PHAR_SOURCES="$PHP_PHAR_SOURCES lib/zip_add.c lib/zip_error.c lib/zip_fclose.c \
                         lib/zip_fread.c lib/zip_open.c lib/zip_source_filep.c  \
                         lib/zip_strerror.c lib/zip_close.c lib/zip_error_get.c \
                         lib/zip_file_error_get.c lib/zip_free.c lib/zip_rename.c \
                         lib/zip_source_free.c lib/zip_unchange_all.c lib/zip_delete.c \
                         lib/zip_error_get_sys_type.c lib/zip_file_get_offset.c \
                         lib/zip_get_name.c lib/zip_replace.c lib/zip_source_function.c \
                         lib/zip_unchange.c lib/zip_dirent.c lib/zip_error_strerror.c \
                         lib/zip_file_strerror.c lib/zip_get_num_files.c \
                         lib/zip_set_name.c lib/zip_source_zip.c lib/zip_unchange_data.c \
                         lib/zip_entry_free.c lib/zip_error_to_str.c lib/zip_fopen.c \
                         lib/zip_name_locate.c lib/zip_source_buffer.c lib/zip_stat.c \
                         lib/zip_entry_new.c lib/zip_err_str.c lib/zip_fopen_index.c \
                         lib/zip_new.c lib/zip_source_file.c lib/zip_stat_index.c lib/zip_get_archive_comment.c \
                         lib/zip_get_file_comment.c lib/zip_set_archive_comment.c lib/zip_set_file_comment.c \
                         lib/zip_unchange_archive.c lib/zip_memdup.c lib/zip_stat_init.c lib/zip_add_dir.c \
                         lib/zip_error_clear.c lib/zip_file_error_clear.c"
		AC_DEFINE(HAVE_PHAR_ZIP,1,[ ])
		PHP_ADD_EXTENSION_DEP(phar, zip, false)
	else
		AC_MSG_RESULT([no])
	fi
  PHP_ADD_BUILD_DIR($ext_builddir/lib, 1)
  PHP_SUBST(PHAR_SHARED_LIBADD)
  PHP_ADD_EXTENSION_DEP(phar, zlib, true)
  PHP_ADD_EXTENSION_DEP(phar, bz2, true)
  PHP_ADD_EXTENSION_DEP(phar, spl, true)
  PHP_ADD_EXTENSION_DEP(phar, gnupg, true)
  PHP_ADD_MAKEFILE_FRAGMENT
fi
