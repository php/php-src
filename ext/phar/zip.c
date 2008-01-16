/*
  +----------------------------------------------------------------------+
  | ZIP archive support for Phar                                         |
  +----------------------------------------------------------------------+
  | Copyright (c) 2007-2008 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Gregory Beaver <cellog@php.net>                             |
  +----------------------------------------------------------------------+
*/

#include "phar_internal.h"
#include "php_stream_unlink.h"

#ifdef PHP_WIN32
static inline void phar_unixify_path_separators(char *path, int path_len) /* {{{ */
{
	char *s;

	/* unixify win paths */
	for (s = path; s - path < path_len; s++) {
		if (*s == '\\') {
			*s = '/';
		}
	}
}
/* }}} */
#endif

/**
 * Does not check for a previously opened phar in the cache.
 *
 * Parse a new one and add it to the cache, returning either SUCCESS or 
 * FAILURE, and setting pphar to the pointer to the manifest entry
 * 
 * This is used by phar_open_fp to process a zip-based phar, but can be called
 * directly.
 */
int phar_open_zipfile(char *fname, int fname_len, char *alias, int alias_len, phar_archive_data** pphar, char **error TSRMLS_DC) /* {{{ */
{
#if HAVE_PHAR_ZIP
	struct zip *zip;
	int ziperror, i, phar_alias_index, register_alias, metadata_len;
	phar_entry_info entry = {0};
	struct zip_stat zs;
	char tmp_buf[1024], *metadata;
	phar_archive_data *mydata = NULL;

	if (error) {
		*error = NULL;
	}

	if (!phar_has_zip) {
		spprintf(error, 4096, "phar zip error: cannot open zip-based phar \"%s\", ext/zip is not enabled", fname);
		return FAILURE;
	}

	zip = zip_open(fname, 0, &ziperror);
	if (!zip) {
		if (error) {
			/* now for the stupid hoops libzip forces... */
			char *tmp;
			int tmp_len;
			tmp_len = zip_error_to_str(NULL, 0, ziperror, ziperror);
			if (!(tmp = emalloc((tmp_len + 1) * sizeof(char)))) {
				spprintf(error, 4096, "phar zip error: cannot open zip-based phar \"%s\"", fname);
			} else {
				if (!zip_error_to_str(tmp, tmp_len + 1, ziperror, ziperror)) {
					efree(tmp);
					spprintf(error, 4096, "phar zip error: cannot open zip-based phar \"%s\"", fname);
				} else {
					spprintf(error, 4096, "phar zip error: cannot open zip-based phar \"%s\": %s", fname, tmp);
					efree(tmp);
				}
			}
		}
		return FAILURE;
	}
	mydata = ecalloc(sizeof(phar_archive_data), 1);
	mydata->fname = estrndup(fname, fname_len);
#ifdef PHP_WIN32
	phar_unixify_path_separators(mydata->fname, fname_len);
#endif
	mydata->fname_len = fname_len;
	if (-1 != (phar_alias_index = zip_name_locate(zip, ".phar/alias.txt", 0))) {
		struct zip_file *zf = zip_fopen_index(zip, phar_alias_index, 0);
		int tmp_len;

		tmp_len = zip_fread(zf, tmp_buf, 1024);
		zip_fclose(zf);
		/* if the alias is stored we enforce it (implicit overrides explicit) */
		if (tmp_len != -1 && alias && alias_len && (alias_len != tmp_len || strncmp(alias, tmp_buf, tmp_len)))
		{
			if (error) {
				spprintf(error, 0, "cannot load phar \"%s\" with implicit alias \"%s\" under different alias \"%s\"", fname, tmp_buf, alias);
			}
			return FAILURE;
		}
		if (tmp_len != -1) {
			/* use implicit alias */
			alias = tmp_buf;
			alias_len = tmp_len;
			register_alias = 0;
		}
	} else {
		register_alias = alias ? 1 : 0;
	}
	mydata->alias = alias ? estrndup(alias, alias_len) : estrndup(mydata->fname, alias_len);
	mydata->alias_len = alias ? alias_len : fname_len;
	mydata->is_zip = 1;
	mydata->zip = zip;

	phar_request_initialize(TSRMLS_C);

	/* read in phar metadata (zip file comment) */
	metadata = (char *) zip_get_archive_comment(zip, &metadata_len, 0);
	if (metadata) {
		if (phar_parse_metadata(&metadata, &mydata->metadata, metadata_len TSRMLS_CC) == FAILURE) {
			/* if not valid serialized data, it is a regular string */
			ALLOC_INIT_ZVAL(mydata->metadata);
			Z_STRVAL_P(mydata->metadata) = estrndup(metadata, metadata_len);
			Z_STRLEN_P(mydata->metadata) = metadata_len;
			Z_TYPE_P(mydata->metadata) = IS_STRING;
		}
	} else {
		mydata->metadata = NULL;
	}
	/* set up our manifest */
	zend_hash_init(&mydata->manifest, sizeof(phar_entry_info),
		zend_get_hash_value, destroy_phar_manifest_entry, 0);
	entry.phar = mydata;
	entry.is_zip = 1;
	/* prevent CRC checking */
	entry.is_crc_checked = 1;
	for (i = 0; i < zip_get_num_files(zip); i++) {
		char *name;
		name = (char *) zip_get_name(zip, i, 0);
		if (name) {
			/* get file stat */
			if (-1 != zip_stat_index(zip, i, 0, &zs)) {
				entry.compressed_filesize = zs.comp_size;
				entry.uncompressed_filesize = zs.size;
				entry.crc32 = zs.crc;
				entry.timestamp = (php_uint32) zs.mtime;
				entry.flags = PHAR_ENT_PERM_DEF_FILE;
				switch (zs.comp_method) {
					case ZIP_CM_DEFLATE :
						/* if we have zip, we have zlib decompression */
						entry.flags |= PHAR_ENT_COMPRESSED_GZ;
						break;
					case ZIP_CM_BZIP2 :
#if !HAVE_BZ2
						if (mydata->metadata) {
							zval_dtor(mydata->metadata);
						}
						efree(mydata->fname);
						if (mydata->alias) {
							efree(mydata->alias);
						}
						zip_close(zip);
						zend_hash_destroy(&(mydata->manifest));
						mydata->manifest.arBuckets = NULL;
						efree(mydata);
						if (error) {
							spprintf(error, 0, "bz2 extension is required for Bzip2 compressed zip-based .phar file \"%s\"", fname);
						}
						return FAILURE;
#else
						if (!phar_has_bz2) {
							if (mydata->metadata) {
								zval_dtor(mydata->metadata);
							}
							efree(mydata->fname);
							if (mydata->alias) {
								efree(mydata->alias);
							}
							zip_close(zip);
							zend_hash_destroy(&(mydata->manifest));
							mydata->manifest.arBuckets = NULL;
							efree(mydata);
							if (error) {
								spprintf(error, 0, "bz2 extension is required for Bzip2 compressed zip-based .phar file \"%s\"", fname);
							}
							return FAILURE;
						}
#endif
						entry.flags |= PHAR_ENT_COMPRESSED_BZ2;
						break;
				}
			}
			entry.index = i;
			entry.filename_len = strlen(name);
			if (name[entry.filename_len - 1] == '/') {
				entry.is_dir = 1;
				entry.filename_len--;
				entry.flags |= PHAR_ENT_PERM_DEF_DIR;
			} else {
				entry.is_dir = 0;
			}
			entry.filename = estrndup(name, entry.filename_len);
			/* get file metadata */
			metadata = (char *) zip_get_file_comment(zip, i, &metadata_len, 0);
			if (metadata) {
				if (phar_parse_metadata(&metadata, &(entry.metadata), metadata_len TSRMLS_CC) == FAILURE) {
					/* if not valid serialized data, it is a regular string */
					ALLOC_INIT_ZVAL(entry.metadata);
					Z_STRVAL_P(entry.metadata) = estrndup(metadata, metadata_len);
					Z_STRLEN_P(entry.metadata) = metadata_len;
					Z_TYPE_P(entry.metadata) = IS_STRING;
				}
			} else {
				entry.metadata = NULL;
			}
			zend_hash_add(&mydata->manifest, name, entry.filename_len, (void *)&entry,sizeof(phar_entry_info), NULL);
		} 
	}
	/* ignore all errors in loading up manifest */
	zip_error_clear(zip);

	zend_hash_add(&(PHAR_GLOBALS->phar_fname_map), mydata->fname, fname_len, (void*)&mydata, sizeof(phar_archive_data*), NULL);
	if (register_alias) {
		mydata->is_explicit_alias = 1;
		zend_hash_add(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len, (void*)&mydata, sizeof(phar_archive_data*), NULL);
	} else {
		mydata->is_explicit_alias = 0;
	}
	if (pphar) {
		*pphar = mydata;
	}
	return SUCCESS;
#else
	spprintf(error, 4096, "phar zip error: Cannot open zip-based phar \"%s\", phar not compiled with zip enabled", fname);
	return FAILURE;
#endif
}
/* }}} */

/**
 * Create or open a zip-based phar for writing
 */
int phar_open_or_create_zip(char *fname, int fname_len, char *alias, int alias_len, int options, phar_archive_data** pphar, char **error TSRMLS_DC) /* {{{ */
{
#if HAVE_PHAR_ZIP
	phar_archive_data *phar;
	int ret = phar_create_or_parse_filename(fname, fname_len, alias, alias_len, options, &phar, error TSRMLS_CC);

	if (pphar) {
		*pphar = phar;
	}
	if (FAILURE == ret) {
		return FAILURE;
	}
	if (phar->is_zip) {
		return ret;
	}

	if (!phar_has_zip) {
		if (error) {
			spprintf(error, 4096, "phar zip error: phar \"%s\" cannot be created as zip-based phar, zip-based phars are disabled (enable ext/zip)", fname);
		}
		return FAILURE;
	}
	if (phar->is_brandnew) {
		int *errorp = NULL;
		phar->is_zip = 1;
		if (phar->fp) {
			php_stream_close(phar->fp);
			phar->fp = NULL;
			php_stream_unlink(phar->fname, 0, NULL);
		}
		phar->zip = zip_open(fname, 0 | ZIP_CREATE | ZIP_EXCL, errorp);
		if (NULL != phar->zip) {
			return SUCCESS;
		}
		/* fail - free newly created manifest entry */
		zend_hash_del(&(PHAR_GLOBALS->phar_alias_map), phar->alias, phar->alias_len);
		zend_hash_del(&(PHAR_GLOBALS->phar_fname_map), fname, fname_len);
		efree(phar->fname);
		efree(phar->alias);
		efree(phar);
		*pphar = NULL;

		if (error) {
			/* now for the stupid hoops libzip forces... */
			char *tmp;
			int tmp_len;
			tmp_len = zip_error_to_str(NULL, 0, *errorp, *errorp);
			if (!(tmp = emalloc((tmp_len + 1) * sizeof(char)))) {
				spprintf(error, 4096, "phar zip error: cannot create zip-based phar \"%s\"", fname);
			} else {
				if (!zip_error_to_str(tmp, tmp_len + 1, *errorp, *errorp)) {
					spprintf(error, 4096, "phar zip error: cannot create zip-based phar \"%s\"", fname);
				} else {
					spprintf(error, 4096, "phar zip error: cannot create zip-based phar \"%s\": %s", fname, tmp);
					efree(tmp);
				}
			}
		}
		return FAILURE;
	}

	/* we've reached here - the phar exists and is a regular phar */
	if (error) {
		spprintf(error, 4096, "phar zip error: phar \"%s\" already exists as a regular phar and must be deleted from disk prior to creating as a zip-based phar", fname);
	}
	return FAILURE;
#else
	if (error) {
		spprintf(error, 4096, "phar zip error: phar \"%s\" cannot be created as zip-based phar, zip-based phars are disabled and cannot be enabled", fname);
	}
	return FAILURE;
#endif /* #if HAVE_PHAR_ZIP */
}
/* }}} */

#if HAVE_PHAR_ZIP
static ssize_t phar_zip_source(void *state, void *data, size_t len, enum zip_source_cmd cmd)
{
	char *buf = (char *) data;
	phar_entry_info *entry = (phar_entry_info *) state;
	size_t read;
	struct zip_stat *sb = (struct zip_stat *) data;
	TSRMLS_FETCH();

	switch (cmd) {
		case ZIP_SOURCE_OPEN :
			/* offset_within_phar is only non-zero when converting from tar/phar-based to zip-based */
			php_stream_seek(entry->fp, entry->offset_within_phar, SEEK_SET);
			return 0;
		case ZIP_SOURCE_READ :
			read = php_stream_read(entry->fp, buf, len);
			if (read < 0) return 0;
			return read;
		case ZIP_SOURCE_STAT :
			zip_stat_init(sb);
			sb->mtime = time(NULL);
			sb->size = entry->uncompressed_filesize;
			return sizeof(struct zip_stat);
		case ZIP_SOURCE_FREE:
			entry->is_modified = 0;
			/* phar->fp is set only if we're converting from a tar/phar-based archive */
			if (entry->fp && entry->fp_refcount == 0 && entry->fp != entry->phar->fp) {
				php_stream_close(entry->fp);
				entry->fp = NULL;
			}
		default:
			return len;
	}
}

/* reconstruct the zip index of each manifest entry */
static int phar_zip_reconstruct_apply(void *data TSRMLS_DC) /* {{{ */
{
	phar_entry_info *entry = (phar_entry_info *)data;
	if (entry->is_deleted) {
		entry->index = -1;
		if (entry->fp_refcount <= 0) {
			return ZEND_HASH_APPLY_REMOVE;
		} else {
			/* we can't delete this in-memory until it is closed */
			return ZEND_HASH_APPLY_KEEP;
		}
	}
	if (entry->is_dir) {
		char *myname = estrndup(entry->filename, entry->filename_len+2);
		myname[entry->filename_len] = '/';
		myname[entry->filename_len+1] = '\0';
		entry->index = zip_name_locate(entry->phar->zip, entry->filename, ZIP_FL_UNCHANGED);
		efree(myname);
	} else {
		entry->index = zip_name_locate(entry->phar->zip, entry->filename, ZIP_FL_UNCHANGED);
	}
	return ZEND_HASH_APPLY_KEEP;
}

/* perform final modification of zip contents for each file in the manifest before saving */
static int phar_zip_changed_apply(void *data TSRMLS_DC) /* {{{ */
{
	phar_entry_info *entry = (phar_entry_info *)data;

	if (entry->is_deleted) {
		if (entry->fp_refcount <= 0) {
			return ZEND_HASH_APPLY_REMOVE;
		} else {
			/* we can't delete this in-memory until it is closed */
			return ZEND_HASH_APPLY_KEEP;
		}
	}
	if (entry->is_modified) {
		ssize_t (*cb)(void *state, void *data, size_t len, enum zip_source_cmd cmd) = phar_zip_source;
		if (entry->fp) {
			php_stream *fp = entry->fp;
			struct zip_source *s = zip_source_function(entry->phar->zip, cb, entry);

			/* we have to prevent free of this fp by mistake */
			entry->fp = NULL;
			if (-1 == (entry->index = _zip_replace(entry->phar->zip, entry->index, (const char *) entry->filename, s))) {
				zip_error_clear(entry->phar->zip);
				return ZEND_HASH_APPLY_REMOVE;
			}
			/* now restore fp and is_modified */
			entry->fp = fp;
			entry->is_modified = 1;
		}

		/* set file metadata */
		if (entry->metadata) {
			php_serialize_data_t metadata_hash;
			if (entry->metadata_str.c) {
				smart_str_free(&entry->metadata_str);
			}
			entry->metadata_str.c = 0;
			entry->metadata_str.len = 0;
			PHP_VAR_SERIALIZE_INIT(metadata_hash);
			php_var_serialize(&entry->metadata_str, &entry->metadata, &metadata_hash TSRMLS_CC);
			PHP_VAR_SERIALIZE_DESTROY(metadata_hash);
			if (-1 == zip_set_file_comment(entry->phar->zip, entry->index, entry->metadata_str.c, entry->metadata_str.len)) {
				return ZEND_HASH_APPLY_STOP;
			}
		} else {
			zip_set_file_comment(entry->phar->zip, entry->index, NULL, 0);
		}
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

int phar_zip_flush(phar_archive_data *archive, char *user_stub, long len, char **error TSRMLS_DC) /* {{{ */
{
	char *pos;
	smart_str main_metadata_str = {0};
	int ziperrint;
	static const char newstub[] = "<?php // zip-based phar archive stub file\n__HALT_COMPILER();";
	php_stream *stubfile;
	php_serialize_data_t metadata_hash;
	int free_user_stub;
	int phar_stub_index, phar_alias_index;
	phar_entry_info entry = {0};

	entry.flags = PHAR_ENT_PERM_DEF_FILE;
	entry.timestamp = time(NULL);
	entry.is_modified = 1;
	entry.is_crc_checked = 1;
	entry.is_zip = 1;
	entry.phar = archive;

	/* set phar metadata */
	main_metadata_str.c = 0;
	if (archive->metadata) {
		PHP_VAR_SERIALIZE_INIT(metadata_hash);
		php_var_serialize(&main_metadata_str, &archive->metadata, &metadata_hash TSRMLS_CC);
		PHP_VAR_SERIALIZE_DESTROY(metadata_hash);
		zip_set_archive_comment(archive->zip, main_metadata_str.c, main_metadata_str.len);
		smart_str_free(&main_metadata_str);
	} else {
		zip_set_archive_comment(archive->zip, NULL, 0);
	}

	/* set alias */
	if (archive->is_explicit_alias) {
		phar_alias_index = zip_name_locate(archive->zip, ".phar/alias.txt", 0);
		entry.fp = php_stream_fopen_tmpfile();
		if (archive->alias_len != php_stream_write(entry.fp, archive->alias, archive->alias_len)) {
			if (error) {
				spprintf(error, 0, "unable to set alias in new zip-based phar \"%s\"", archive->fname);
			}
			return EOF;
		}
		entry.uncompressed_filesize = sizeof(newstub) - 1;
		entry.filename = estrndup(".phar/alias.txt", sizeof(".phar/alias.txt")-1);
		entry.filename_len = sizeof(".phar/alias.txt")-1;
		entry.is_modified = 1;
		entry.index = phar_alias_index;
		if (SUCCESS != zend_hash_update(&archive->manifest, entry.filename, entry.filename_len, (void*)&entry, sizeof(phar_entry_info), NULL)) {
			if (error) {
				spprintf(error, 0, "unable to set alias in new zip-based phar \"%s\"", archive->fname);
			}
			return EOF;
		}
	}
	/* register alias */
	if (archive->alias_len) {
		phar_get_archive(&archive, archive->fname, archive->fname_len, archive->alias, archive->alias_len, NULL TSRMLS_CC);
	}

	/* set stub */
	phar_stub_index = zip_name_locate(archive->zip, ".phar/stub.php", 0);
	zip_error_clear(archive->zip);
	if (user_stub) {
		if (len < 0) {
			/* resource passed in */
			if (!(php_stream_from_zval_no_verify(stubfile, (zval **)user_stub))) {
				if (error) {
					spprintf(error, 0, "unable to access resource to copy stub to new zip-based phar \"%s\"", archive->fname);
				}
				return EOF;
			}
			if (len == -1) {
				len = PHP_STREAM_COPY_ALL;
			} else {
				len = -len;
			}
			user_stub = 0;
			if (!(len = php_stream_copy_to_mem(stubfile, &user_stub, len, 0)) || !user_stub) {
				if (error) {
					spprintf(error, 0, "unable to read resource to copy stub to new zip-based phar \"%s\"", archive->fname);
				}
				return EOF;
			}
			free_user_stub = 1;
		} else {
			free_user_stub = 0;
		}
		if ((pos = strstr(user_stub, "__HALT_COMPILER();")) == NULL)
		{
			if (error) {
				spprintf(error, 0, "illegal stub for zip-based phar \"%s\"", archive->fname);
			}
			if (free_user_stub) {
				efree(user_stub);
			}
			return EOF;
		}
		len = pos - user_stub + 18;
		entry.fp = php_stream_fopen_tmpfile();
		entry.uncompressed_filesize = len + 5;

		if ((size_t)len != php_stream_write(entry.fp, user_stub, len)
		||            5 != php_stream_write(entry.fp, " ?>\r\n", 5)) {
			if (error) {
				spprintf(error, 0, "unable to create stub from string in new zip-based phar \"%s\"", archive->fname);
			}
			if (free_user_stub) {
				efree(user_stub);
			}
			php_stream_close(entry.fp);
			return EOF;
		}
		entry.filename = estrndup(".phar/stub.php", sizeof(".phar/stub.php")-1);
		entry.filename_len = sizeof(".phar/stub.php")-1;
		entry.is_modified = 1;
		entry.index = phar_stub_index;
		if (SUCCESS != zend_hash_update(&archive->manifest, entry.filename, entry.filename_len, (void*)&entry, sizeof(phar_entry_info), NULL)) {
			if (free_user_stub) {
				efree(user_stub);
			}
			if (error) {
				spprintf(error, 0, "unable to set stub in zip-based phar \"%s\"", archive->fname);
			}
			return EOF;
		}
		if (free_user_stub) {
			efree(user_stub);
		}
	} else {
		if (-1 == phar_stub_index) {
			/* this is a brand new phar, add the stub */
			entry.fp = php_stream_fopen_tmpfile();
			if (sizeof(newstub)-1 != php_stream_write(entry.fp, newstub, sizeof(newstub)-1)) {
				if (error) {
					spprintf(error, 0, "unable to create stub in new zip-based phar \"%s\"", archive->fname);
				}
				return EOF;
			}
			entry.uncompressed_filesize = sizeof(newstub) - 1;
			entry.filename = estrndup(".phar/stub.php", sizeof(".phar/stub.php")-1);
			entry.filename_len = sizeof(".phar/stub.php")-1;
			entry.is_modified = 1;
			entry.index = -1;
			if (SUCCESS != zend_hash_add(&archive->manifest, entry.filename, entry.filename_len, (void*)&entry, sizeof(phar_entry_info), NULL)) {
				php_stream_close(entry.fp);
				efree(entry.filename);
				if (error) {
					spprintf(error, 0, "unable to create stub in new zip-based phar \"%s\": %s", archive->fname, zip_strerror(archive->zip));
				}
				return EOF;
			}
		}
	}

	/* save modified files to the zip */
	zend_hash_apply(&archive->manifest, phar_zip_changed_apply TSRMLS_CC);
	if (archive->zip->error.str) {
		if (error) {
			spprintf(error, 4096, "phar zip flush of \"%s\" failed: %s", archive->fname, zip_strerror(archive->zip));
		}
		return EOF;
	}

	/* save zip */
	if (-1 == zip_close(archive->zip)) {
		if (error) {
			spprintf(error, 4096, "saving of zip-based phar \"%s\" failed: %s", archive->fname, zip_strerror(archive->zip));
		}
		return EOF;
	}

	/* re-open */
	archive->zip = zip_open(archive->fname, 0, &ziperrint);
	if (!archive->zip) {
		if (error) {
			/* now for the stupid hoops libzip forces... */
			char *tmp;
			int tmp_len;
			tmp_len = zip_error_to_str(NULL, 0, ziperrint, ziperrint);
			if (!(tmp = emalloc((tmp_len + 1) * sizeof(char)))) {
				spprintf(error, 4096, "phar zip error: cannot re-open zip-based phar \"%s\"", archive->fname);
			} else {
				if (!zip_error_to_str(tmp, tmp_len + 1, ziperrint, ziperrint)) {
					spprintf(error, 4096, "phar zip error: cannot re-open zip-based phar \"%s\"", archive->fname);
				} else {
					spprintf(error, 4096, "phar zip error: cannot re-open zip-based phar \"%s\": %s", archive->fname, tmp);
					efree(tmp);
				}
			}
		}
		return EOF;
	}

	/* reconstruct manifest zip index map */
	zend_hash_apply(&archive->manifest, phar_zip_reconstruct_apply TSRMLS_CC);
	return EOF;
}
/* }}} */
#endif /* if HAVE_PHAR_ZIP */
