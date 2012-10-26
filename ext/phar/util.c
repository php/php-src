/*
  +----------------------------------------------------------------------+
  | phar php single-file executable PHP extension                        |
  | utility functions                                                    |
  +----------------------------------------------------------------------+
  | Copyright (c) 2005-2012 The PHP Group                                |
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
  |          Marcus Boerger <helly@php.net>                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "phar_internal.h"
#ifdef PHAR_HASH_OK
#include "ext/hash/php_hash_sha.h"
#endif

#ifdef PHAR_HAVE_OPENSSL
/* OpenSSL includes */
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/crypto.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/conf.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/pkcs12.h>
#else
static int phar_call_openssl_signverify(int is_sign, php_stream *fp, off_t end, char *key, int key_len, char **signature, int *signature_len TSRMLS_DC);
#endif

#if !defined(PHP_VERSION_ID) || PHP_VERSION_ID < 50300
extern php_stream_wrapper php_stream_phar_wrapper;
#endif

/* for links to relative location, prepend cwd of the entry */
static char *phar_get_link_location(phar_entry_info *entry TSRMLS_DC) /* {{{ */
{
	char *p, *ret = NULL;
	if (!entry->link) {
		return NULL;
	}
	if (entry->link[0] == '/') {
		return estrdup(entry->link + 1);
	}
	p = strrchr(entry->filename, '/');
	if (p) {
		*p = '\0';
		spprintf(&ret, 0, "%s/%s", entry->filename, entry->link);
		return ret;
	}
	return entry->link;
}
/* }}} */

phar_entry_info *phar_get_link_source(phar_entry_info *entry TSRMLS_DC) /* {{{ */
{
	phar_entry_info *link_entry;
	char *link;

	if (!entry->link) {
		return entry;
	}

	link = phar_get_link_location(entry TSRMLS_CC);
	if (SUCCESS == zend_hash_find(&(entry->phar->manifest), entry->link, strlen(entry->link), (void **)&link_entry) ||
		SUCCESS == zend_hash_find(&(entry->phar->manifest), link, strlen(link), (void **)&link_entry)) {
		if (link != entry->link) {
			efree(link);
		}
		return phar_get_link_source(link_entry TSRMLS_CC);
	} else {
		if (link != entry->link) {
			efree(link);
		}
		return NULL;
	}
}
/* }}} */

/* retrieve a phar_entry_info's current file pointer for reading contents */
php_stream *phar_get_efp(phar_entry_info *entry, int follow_links TSRMLS_DC) /* {{{ */
{
	if (follow_links && entry->link) {
		phar_entry_info *link_entry = phar_get_link_source(entry TSRMLS_CC);

		if (link_entry && link_entry != entry) {
			return phar_get_efp(link_entry, 1 TSRMLS_CC);
		}
	}

	if (phar_get_fp_type(entry TSRMLS_CC) == PHAR_FP) {
		if (!phar_get_entrypfp(entry TSRMLS_CC)) {
			/* re-open just in time for cases where our refcount reached 0 on the phar archive */
			phar_open_archive_fp(entry->phar TSRMLS_CC);
		}
		return phar_get_entrypfp(entry TSRMLS_CC);
	} else if (phar_get_fp_type(entry TSRMLS_CC) == PHAR_UFP) {
		return phar_get_entrypufp(entry TSRMLS_CC);
	} else if (entry->fp_type == PHAR_MOD) {
		return entry->fp;
	} else {
		/* temporary manifest entry */
		if (!entry->fp) {
			entry->fp = php_stream_open_wrapper(entry->tmp, "rb", STREAM_MUST_SEEK|0, NULL);
		}
		return entry->fp;
	}
}
/* }}} */

int phar_seek_efp(phar_entry_info *entry, off_t offset, int whence, off_t position, int follow_links TSRMLS_DC) /* {{{ */
{
	php_stream *fp = phar_get_efp(entry, follow_links TSRMLS_CC);
	off_t temp, eoffset;

	if (!fp) {
		return -1;
	}

	if (follow_links) {
		phar_entry_info *t;
		t = phar_get_link_source(entry TSRMLS_CC);
		if (t) {
			entry = t;
		}
	}

	if (entry->is_dir) {
		return 0;
	}

	eoffset = phar_get_fp_offset(entry TSRMLS_CC);

	switch (whence) {
		case SEEK_END:
			temp = eoffset + entry->uncompressed_filesize + offset;
			break;
		case SEEK_CUR:
			temp = eoffset + position + offset;
			break;
		case SEEK_SET:
			temp = eoffset + offset;
			break;
		default:
			temp = 0;
	}

	if (temp > eoffset + (off_t) entry->uncompressed_filesize) {
		return -1;
	}

	if (temp < eoffset) {
		return -1;
	}

	return php_stream_seek(fp, temp, SEEK_SET);
}
/* }}} */

/* mount an absolute path or uri to a path internal to the phar archive */
int phar_mount_entry(phar_archive_data *phar, char *filename, int filename_len, char *path, int path_len TSRMLS_DC) /* {{{ */
{
	phar_entry_info entry = {0};
	php_stream_statbuf ssb;
	int is_phar;
	const char *err;

	if (phar_path_check(&path, &path_len, &err) > pcr_is_ok) {
		return FAILURE;
	}

	if (path_len >= sizeof(".phar")-1 && !memcmp(path, ".phar", sizeof(".phar")-1)) {
		/* no creating magic phar files by mounting them */
		return FAILURE;
	}

	is_phar = (filename_len > 7 && !memcmp(filename, "phar://", 7));

	entry.phar = phar;
	entry.filename = estrndup(path, path_len);
#ifdef PHP_WIN32
	phar_unixify_path_separators(entry.filename, path_len);
#endif
	entry.filename_len = path_len;
	if (is_phar) {
		entry.tmp = estrndup(filename, filename_len);
	} else {
		entry.tmp = expand_filepath(filename, NULL TSRMLS_CC);
		if (!entry.tmp) {
			entry.tmp = estrndup(filename, filename_len);
		}
	}
#if PHP_API_VERSION < 20100412
	if (PG(safe_mode) && !is_phar && (!php_checkuid(entry.tmp, NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
		efree(entry.tmp);
		efree(entry.filename);
		return FAILURE;
	}
#endif
	filename = entry.tmp;

	/* only check openbasedir for files, not for phar streams */
	if (!is_phar && php_check_open_basedir(filename TSRMLS_CC)) {
		efree(entry.tmp);
		efree(entry.filename);
		return FAILURE;
	}

	entry.is_mounted = 1;
	entry.is_crc_checked = 1;
	entry.fp_type = PHAR_TMP;

	if (SUCCESS != php_stream_stat_path(filename, &ssb)) {
		efree(entry.tmp);
		efree(entry.filename);
		return FAILURE;
	}

	if (ssb.sb.st_mode & S_IFDIR) {
		entry.is_dir = 1;
		if (SUCCESS != zend_hash_add(&phar->mounted_dirs, entry.filename, path_len, (void *)&(entry.filename), sizeof(char *), NULL)) {
			/* directory already mounted */
			efree(entry.tmp);
			efree(entry.filename);
			return FAILURE;
		}
	} else {
		entry.is_dir = 0;
		entry.uncompressed_filesize = entry.compressed_filesize = ssb.sb.st_size;
	}

	entry.flags = ssb.sb.st_mode;

	if (SUCCESS == zend_hash_add(&phar->manifest, entry.filename, path_len, (void*)&entry, sizeof(phar_entry_info), NULL)) {
		return SUCCESS;
	}

	efree(entry.tmp);
	efree(entry.filename);
	return FAILURE;
}
/* }}} */

char *phar_find_in_include_path(char *filename, int filename_len, phar_archive_data **pphar TSRMLS_DC) /* {{{ */
{
#if PHP_VERSION_ID >= 50300
	char *path, *fname, *arch, *entry, *ret, *test;
	int arch_len, entry_len, fname_len, ret_len;
	phar_archive_data *phar;

	if (pphar) {
		*pphar = NULL;
	} else {
		pphar = &phar;
	}

	if (!zend_is_executing(TSRMLS_C) || !PHAR_G(cwd)) {
		return phar_save_resolve_path(filename, filename_len TSRMLS_CC);
	}

	fname = (char*)zend_get_executed_filename(TSRMLS_C);
	fname_len = strlen(fname);

	if (PHAR_G(last_phar) && !memcmp(fname, "phar://", 7) && fname_len - 7 >= PHAR_G(last_phar_name_len) && !memcmp(fname + 7, PHAR_G(last_phar_name), PHAR_G(last_phar_name_len))) {
		arch = estrndup(PHAR_G(last_phar_name), PHAR_G(last_phar_name_len));
		arch_len = PHAR_G(last_phar_name_len);
		phar = PHAR_G(last_phar);
		goto splitted;
	}

	if (fname_len < 7 || memcmp(fname, "phar://", 7) || SUCCESS != phar_split_fname(fname, strlen(fname), &arch, &arch_len, &entry, &entry_len, 1, 0 TSRMLS_CC)) {
		return phar_save_resolve_path(filename, filename_len TSRMLS_CC);
	}

	efree(entry);

	if (*filename == '.') {
		int try_len;

		if (FAILURE == phar_get_archive(&phar, arch, arch_len, NULL, 0, NULL TSRMLS_CC)) {
			efree(arch);
			return phar_save_resolve_path(filename, filename_len TSRMLS_CC);
		}
splitted:
		if (pphar) {
			*pphar = phar;
		}

		try_len = filename_len;
		test = phar_fix_filepath(estrndup(filename, filename_len), &try_len, 1 TSRMLS_CC);

		if (*test == '/') {
			if (zend_hash_exists(&(phar->manifest), test + 1, try_len - 1)) {
				spprintf(&ret, 0, "phar://%s%s", arch, test);
				efree(arch);
				efree(test);
				return ret;
			}
		} else {
			if (zend_hash_exists(&(phar->manifest), test, try_len)) {
				spprintf(&ret, 0, "phar://%s/%s", arch, test);
				efree(arch);
				efree(test);
				return ret;
			}
		}
		efree(test);
	}

	spprintf(&path, MAXPATHLEN, "phar://%s/%s%c%s", arch, PHAR_G(cwd), DEFAULT_DIR_SEPARATOR, PG(include_path));
	efree(arch);
	ret = php_resolve_path(filename, filename_len, path TSRMLS_CC);
	efree(path);

	if (ret && strlen(ret) > 8 && !strncmp(ret, "phar://", 7)) {
		ret_len = strlen(ret);
		/* found phar:// */

		if (SUCCESS != phar_split_fname(ret, ret_len, &arch, &arch_len, &entry, &entry_len, 1, 0 TSRMLS_CC)) {
			return ret;
		}

		zend_hash_find(&(PHAR_GLOBALS->phar_fname_map), arch, arch_len, (void **) &pphar);

		if (!pphar && PHAR_G(manifest_cached)) {
			zend_hash_find(&cached_phars, arch, arch_len, (void **) &pphar);
		}

		efree(arch);
		efree(entry);
	}

	return ret;
#else /* PHP 5.2 */
	char resolved_path[MAXPATHLEN];
	char trypath[MAXPATHLEN];
	char *ptr, *end, *path = PG(include_path);
	php_stream_wrapper *wrapper;
	const char *p;
	int n = 0;
	char *fname, *arch, *entry, *ret, *test;
	int arch_len, entry_len;
	phar_archive_data *phar = NULL;

	if (!filename) {
		return NULL;
	}

	if (!zend_is_executing(TSRMLS_C) || !PHAR_G(cwd)) {
		goto doit;
	}

	fname = (char*)zend_get_executed_filename(TSRMLS_C);

	if (SUCCESS != phar_split_fname(fname, strlen(fname), &arch, &arch_len, &entry, &entry_len, 1, 0 TSRMLS_CC)) {
		goto doit;
	}

	efree(entry);

	if (*filename == '.') {
		int try_len;

		if (FAILURE == phar_get_archive(&phar, arch, arch_len, NULL, 0, NULL TSRMLS_CC)) {
			efree(arch);
			goto doit;
		}

		try_len = filename_len;
		test = phar_fix_filepath(estrndup(filename, filename_len), &try_len, 1 TSRMLS_CC);

		if (*test == '/') {
			if (zend_hash_exists(&(phar->manifest), test + 1, try_len - 1)) {
				spprintf(&ret, 0, "phar://%s%s", arch, test);
				efree(arch);
				efree(test);
				return ret;
			}
		} else {
			if (zend_hash_exists(&(phar->manifest), test, try_len)) {
				spprintf(&ret, 0, "phar://%s/%s", arch, test);
				efree(arch);
				efree(test);
				return ret;
			}
		}

		efree(test);
	}

	efree(arch);
doit:
	if (*filename == '.' || IS_ABSOLUTE_PATH(filename, filename_len) || !path || !*path) {
		if (tsrm_realpath(filename, resolved_path TSRMLS_CC)) {
			return estrdup(resolved_path);
		} else {
			return NULL;
		}
	}

	/* test for stream wrappers and return */
	for (p = filename; p - filename < filename_len && (isalnum((int)*p) || *p == '+' || *p == '-' || *p == '.'); ++p, ++n);

	if (n < filename_len - 3 && (*p == ':') && (!strncmp("//", p+1, 2) || ( filename_len > 4 && !memcmp("data", filename, 4)))) {
		/* found stream wrapper, this is an absolute path until stream wrappers implement realpath */
		return estrndup(filename, filename_len);
	}

	ptr = (char *) path;
	while (ptr && *ptr) {
		int len, is_stream_wrapper = 0, maybe_stream = 1;

		end = strchr(ptr, DEFAULT_DIR_SEPARATOR);
#ifndef PHP_WIN32
		/* search for stream wrapper */
		if (end - ptr  <= 1) {
			maybe_stream = 0;
			goto not_stream;
		}

		for (p = ptr, n = 0; p < end && (isalnum((int)*p) || *p == '+' || *p == '-' || *p == '.'); ++p, ++n);

		if (n == end - ptr && *p && !strncmp("//", p+1, 2)) {
			is_stream_wrapper = 1;
			/* seek to real end of include_path portion */
			end = strchr(end + 1, DEFAULT_DIR_SEPARATOR);
		} else {
			maybe_stream = 0;
		}
not_stream:
#endif
		if (end) {
			if ((end-ptr) + 1 + filename_len + 1 >= MAXPATHLEN) {
				ptr = end + 1;
				continue;
			}

			memcpy(trypath, ptr, end-ptr);
			len = end-ptr;
			trypath[end-ptr] = '/';
			memcpy(trypath+(end-ptr)+1, filename, filename_len+1);
			ptr = end+1;
		} else {
			len = strlen(ptr);

			if (len + 1 + filename_len + 1 >= MAXPATHLEN) {
				break;
			}

			memcpy(trypath, ptr, len);
			trypath[len] = '/';
			memcpy(trypath+len+1, filename, filename_len+1);
			ptr = NULL;
		}

		if (!is_stream_wrapper && maybe_stream) {
			/* search for stream wrapper */
			for (p = trypath, n = 0; isalnum((int)*p) || *p == '+' || *p == '-' || *p == '.'; ++p, ++n);
		}

		if (is_stream_wrapper || (n < len - 3 && (*p == ':') && (n > 1) && (!strncmp("//", p+1, 2) || !memcmp("data", trypath, 4)))) {
			char *actual;

			wrapper = php_stream_locate_url_wrapper(trypath, &actual, STREAM_OPEN_FOR_INCLUDE TSRMLS_CC);
			if (wrapper == &php_plain_files_wrapper) {
				strlcpy(trypath, actual, sizeof(trypath));
			} else if (!wrapper) {
				/* if wrapper is NULL, there was a mal-formed include_path stream wrapper, so skip this ptr */
				continue;
			} else {
				if (wrapper->wops->url_stat) {
					php_stream_statbuf ssb;

					if (SUCCESS == wrapper->wops->url_stat(wrapper, trypath, 0, &ssb, NULL TSRMLS_CC)) {
						if (wrapper == &php_stream_phar_wrapper) {
							char *arch, *entry;
							int arch_len, entry_len, ret_len;

							ret_len = strlen(trypath);
							/* found phar:// */

							if (SUCCESS != phar_split_fname(trypath, ret_len, &arch, &arch_len, &entry, &entry_len, 1, 0 TSRMLS_CC)) {
								return estrndup(trypath, ret_len);
							}

							zend_hash_find(&(PHAR_GLOBALS->phar_fname_map), arch, arch_len, (void **) &pphar);

							if (!pphar && PHAR_G(manifest_cached)) {
								zend_hash_find(&cached_phars, arch, arch_len, (void **) &pphar);
							}

							efree(arch);
							efree(entry);

							return estrndup(trypath, ret_len);
						}
						return estrdup(trypath);
					}
				}
				continue;
			}
		}

		if (tsrm_realpath(trypath, resolved_path TSRMLS_CC)) {
			return estrdup(resolved_path);
		}
	} /* end provided path */

	/* check in calling scripts' current working directory as a fall back case */
	if (zend_is_executing(TSRMLS_C)) {
		char *exec_fname = (char*)zend_get_executed_filename(TSRMLS_C);
		int exec_fname_length = strlen(exec_fname);
		const char *p;
		int n = 0;

		while ((--exec_fname_length >= 0) && !IS_SLASH(exec_fname[exec_fname_length]));
		if (exec_fname && exec_fname[0] != '[' && 
			exec_fname_length > 0 && 
			exec_fname_length + 1 + filename_len + 1 < MAXPATHLEN) {
			memcpy(trypath, exec_fname, exec_fname_length + 1);
			memcpy(trypath+exec_fname_length + 1, filename, filename_len+1);

			/* search for stream wrapper */
			for (p = trypath; isalnum((int)*p) || *p == '+' || *p == '-' || *p == '.'; ++p, ++n);

			if (n < exec_fname_length - 3 && (*p == ':') && (n > 1) && (!strncmp("//", p+1, 2) || !memcmp("data", trypath, 4))) {
				char *actual;

				wrapper = php_stream_locate_url_wrapper(trypath, &actual, STREAM_OPEN_FOR_INCLUDE TSRMLS_CC);

				if (wrapper == &php_plain_files_wrapper) {
					/* this should never technically happen, but we'll leave it here for completeness */
					strlcpy(trypath, actual, sizeof(trypath));
				} else if (!wrapper) {
					/* if wrapper is NULL, there was a malformed include_path stream wrapper
					   this also should be impossible */
					return NULL;
				} else {
					return estrdup(trypath);
				}
			}

			if (tsrm_realpath(trypath, resolved_path TSRMLS_CC)) {
				return estrdup(resolved_path);
			}
		}
	}

	return NULL;
#endif /* PHP 5.2 */
}
/* }}} */

/**
 * Retrieve a copy of the file information on a single file within a phar, or null.
 * This also transfers the open file pointer, if any, to the entry.
 *
 * If the file does not already exist, this will fail.  Pre-existing files can be
 * appended, truncated, or read.  For read, if the entry is marked unmodified, it is
 * assumed that the file pointer, if present, is opened for reading
 */
int phar_get_entry_data(phar_entry_data **ret, char *fname, int fname_len, char *path, int path_len, char *mode, char allow_dir, char **error, int security TSRMLS_DC) /* {{{ */
{
	phar_archive_data *phar;
	phar_entry_info *entry;
	int for_write  = mode[0] != 'r' || mode[1] == '+';
	int for_append = mode[0] == 'a';
	int for_create = mode[0] != 'r';
	int for_trunc  = mode[0] == 'w';

	if (!ret) {
		return FAILURE;
	}

	*ret = NULL;

	if (error) {
		*error = NULL;
	}

	if (FAILURE == phar_get_archive(&phar, fname, fname_len, NULL, 0, error TSRMLS_CC)) {
		return FAILURE;
	}

	if (for_write && PHAR_G(readonly) && !phar->is_data) {
		if (error) {
			spprintf(error, 4096, "phar error: file \"%s\" in phar \"%s\" cannot be opened for writing, disabled by ini setting", path, fname);
		}
		return FAILURE;
	}

	if (!path_len) {
		if (error) {
			spprintf(error, 4096, "phar error: file \"\" in phar \"%s\" cannot be empty", fname);
		}
		return FAILURE;
	}
really_get_entry:
	if (allow_dir) {
		if ((entry = phar_get_entry_info_dir(phar, path, path_len, allow_dir, for_create && !PHAR_G(readonly) && !phar->is_data ? NULL : error, security TSRMLS_CC)) == NULL) {
			if (for_create && (!PHAR_G(readonly) || phar->is_data)) {
				return SUCCESS;
			}
			return FAILURE;
		}
	} else {
		if ((entry = phar_get_entry_info(phar, path, path_len, for_create && !PHAR_G(readonly) && !phar->is_data ? NULL : error, security TSRMLS_CC)) == NULL) {
			if (for_create && (!PHAR_G(readonly) || phar->is_data)) {
				return SUCCESS;
			}
			return FAILURE;
		}
	}

	if (for_write && phar->is_persistent) {
		if (FAILURE == phar_copy_on_write(&phar TSRMLS_CC)) {
			if (error) {
				spprintf(error, 4096, "phar error: file \"%s\" in phar \"%s\" cannot be opened for writing, could not make cached phar writeable", path, fname);
			}
			return FAILURE;
		} else {
			goto really_get_entry;
		}
	}

	if (entry->is_modified && !for_write) {
		if (error) {
			spprintf(error, 4096, "phar error: file \"%s\" in phar \"%s\" cannot be opened for reading, writable file pointers are open", path, fname);
		}
		return FAILURE;
	}

	if (entry->fp_refcount && for_write) {
		if (error) {
			spprintf(error, 4096, "phar error: file \"%s\" in phar \"%s\" cannot be opened for writing, readable file pointers are open", path, fname);
		}
		return FAILURE;
	}

	if (entry->is_deleted) {
		if (!for_create) {
			return FAILURE;
		}
		entry->is_deleted = 0;
	}

	if (entry->is_dir) {
		*ret = (phar_entry_data *) emalloc(sizeof(phar_entry_data));
		(*ret)->position = 0;
		(*ret)->fp = NULL;
		(*ret)->phar = phar;
		(*ret)->for_write = for_write;
		(*ret)->internal_file = entry;
		(*ret)->is_zip = entry->is_zip;
		(*ret)->is_tar = entry->is_tar;

		if (!phar->is_persistent) {
			++(entry->phar->refcount);
			++(entry->fp_refcount);
		}

		return SUCCESS;
	}

	if (entry->fp_type == PHAR_MOD) {
		if (for_trunc) {
			if (FAILURE == phar_create_writeable_entry(phar, entry, error TSRMLS_CC)) {
				return FAILURE;
			}
		} else if (for_append) {
			phar_seek_efp(entry, 0, SEEK_END, 0, 0 TSRMLS_CC);
		}
	} else {
		if (for_write) {
			if (entry->link) {
				efree(entry->link);
				entry->link = NULL;
				entry->tar_type = (entry->is_tar ? TAR_FILE : '\0');
			}

			if (for_trunc) {
				if (FAILURE == phar_create_writeable_entry(phar, entry, error TSRMLS_CC)) {
					return FAILURE;
				}
			} else {
				if (FAILURE == phar_separate_entry_fp(entry, error TSRMLS_CC)) {
					return FAILURE;
				}
			}
		} else {
			if (FAILURE == phar_open_entry_fp(entry, error, 1 TSRMLS_CC)) {
				return FAILURE;
			}
		}
	}

	*ret = (phar_entry_data *) emalloc(sizeof(phar_entry_data));
	(*ret)->position = 0;
	(*ret)->phar = phar;
	(*ret)->for_write = for_write;
	(*ret)->internal_file = entry;
	(*ret)->is_zip = entry->is_zip;
	(*ret)->is_tar = entry->is_tar;
	(*ret)->fp = phar_get_efp(entry, 1 TSRMLS_CC);
	if (entry->link) {
		(*ret)->zero = phar_get_fp_offset(phar_get_link_source(entry TSRMLS_CC) TSRMLS_CC);
	} else {
		(*ret)->zero = phar_get_fp_offset(entry TSRMLS_CC);
	}

	if (!phar->is_persistent) {
		++(entry->fp_refcount);
		++(entry->phar->refcount);
	}

	return SUCCESS;
}
/* }}} */

/**
 * Create a new dummy file slot within a writeable phar for a newly created file
 */
phar_entry_data *phar_get_or_create_entry_data(char *fname, int fname_len, char *path, int path_len, char *mode, char allow_dir, char **error, int security TSRMLS_DC) /* {{{ */
{
	phar_archive_data *phar;
	phar_entry_info *entry, etemp;
	phar_entry_data *ret;
	const char *pcr_error;
	char is_dir;

#ifdef PHP_WIN32
	phar_unixify_path_separators(path, path_len);
#endif

	is_dir = (path_len && path[path_len - 1] == '/') ? 1 : 0;

	if (FAILURE == phar_get_archive(&phar, fname, fname_len, NULL, 0, error TSRMLS_CC)) {
		return NULL;
	}

	if (FAILURE == phar_get_entry_data(&ret, fname, fname_len, path, path_len, mode, allow_dir, error, security TSRMLS_CC)) {
		return NULL;
	} else if (ret) {
		return ret;
	}

	if (phar_path_check(&path, &path_len, &pcr_error) > pcr_is_ok) {
		if (error) {
			spprintf(error, 0, "phar error: invalid path \"%s\" contains %s", path, pcr_error);
		}
		return NULL;
	}

	if (phar->is_persistent && FAILURE == phar_copy_on_write(&phar TSRMLS_CC)) {
		if (error) {
			spprintf(error, 4096, "phar error: file \"%s\" in phar \"%s\" cannot be created, could not make cached phar writeable", path, fname);
		}
		return NULL;
	}

	/* create a new phar data holder */
	ret = (phar_entry_data *) emalloc(sizeof(phar_entry_data));

	/* create an entry, this is a new file */
	memset(&etemp, 0, sizeof(phar_entry_info));
	etemp.filename_len = path_len;
	etemp.fp_type = PHAR_MOD;
	etemp.fp = php_stream_fopen_tmpfile();

	if (!etemp.fp) {
		if (error) {
			spprintf(error, 0, "phar error: unable to create temporary file");
		}
		efree(ret);
		return NULL;
	}

	etemp.fp_refcount = 1;

	if (allow_dir == 2) {
		etemp.is_dir = 1;
		etemp.flags = etemp.old_flags = PHAR_ENT_PERM_DEF_DIR;
	} else {
		etemp.flags = etemp.old_flags = PHAR_ENT_PERM_DEF_FILE;
	}
	if (is_dir) {
		etemp.filename_len--; /* strip trailing / */
		path_len--;
	}

	phar_add_virtual_dirs(phar, path, path_len TSRMLS_CC);
	etemp.is_modified = 1;
	etemp.timestamp = time(0);
	etemp.is_crc_checked = 1;
	etemp.phar = phar;
	etemp.filename = estrndup(path, path_len);
	etemp.is_zip = phar->is_zip;

	if (phar->is_tar) {
		etemp.is_tar = phar->is_tar;
		etemp.tar_type = etemp.is_dir ? TAR_DIR : TAR_FILE;
	}

	if (FAILURE == zend_hash_add(&phar->manifest, etemp.filename, path_len, (void*)&etemp, sizeof(phar_entry_info), (void **) &entry)) {
		php_stream_close(etemp.fp);
		if (error) {
			spprintf(error, 0, "phar error: unable to add new entry \"%s\" to phar \"%s\"", etemp.filename, phar->fname);
		}
		efree(ret);
		efree(etemp.filename);
		return NULL;
	}

	if (!entry) {
		php_stream_close(etemp.fp);
		efree(etemp.filename);
		efree(ret);
		return NULL;
	}

	++(phar->refcount);
	ret->phar = phar;
	ret->fp = entry->fp;
	ret->position = ret->zero = 0;
	ret->for_write = 1;
	ret->is_zip = entry->is_zip;
	ret->is_tar = entry->is_tar;
	ret->internal_file = entry;

	return ret;
}
/* }}} */

/* initialize a phar_archive_data's read-only fp for existing phar data */
int phar_open_archive_fp(phar_archive_data *phar TSRMLS_DC) /* {{{ */
{
	if (phar_get_pharfp(phar TSRMLS_CC)) {
		return SUCCESS;
	}
#if PHP_API_VERSION < 20100412
	if (PG(safe_mode) && (!php_checkuid(phar->fname, NULL, CHECKUID_ALLOW_ONLY_FILE))) {
		return FAILURE;
	}
#endif

	if (php_check_open_basedir(phar->fname TSRMLS_CC)) {
		return FAILURE;
	}

	phar_set_pharfp(phar, php_stream_open_wrapper(phar->fname, "rb", IGNORE_URL|STREAM_MUST_SEEK|0, NULL) TSRMLS_CC);

	if (!phar_get_pharfp(phar TSRMLS_CC)) {
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

/* copy file data from an existing to a new phar_entry_info that is not in the manifest */
int phar_copy_entry_fp(phar_entry_info *source, phar_entry_info *dest, char **error TSRMLS_DC) /* {{{ */
{
	phar_entry_info *link;

	if (FAILURE == phar_open_entry_fp(source, error, 1 TSRMLS_CC)) {
		return FAILURE;
	}

	if (dest->link) {
		efree(dest->link);
		dest->link = NULL;
		dest->tar_type = (dest->is_tar ? TAR_FILE : '\0');
	}

	dest->fp_type = PHAR_MOD;
	dest->offset = 0;
	dest->is_modified = 1;
	dest->fp = php_stream_fopen_tmpfile();
	phar_seek_efp(source, 0, SEEK_SET, 0, 1 TSRMLS_CC);
	link = phar_get_link_source(source TSRMLS_CC);

	if (!link) {
		link = source;
	}

	if (SUCCESS != phar_stream_copy_to_stream(phar_get_efp(link, 0 TSRMLS_CC), dest->fp, link->uncompressed_filesize, NULL)) {
		php_stream_close(dest->fp);
		dest->fp_type = PHAR_FP;
		if (error) {
			spprintf(error, 4096, "phar error: unable to copy contents of file \"%s\" to \"%s\" in phar archive \"%s\"", source->filename, dest->filename, source->phar->fname);
		}
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

/* open and decompress a compressed phar entry
 */
int phar_open_entry_fp(phar_entry_info *entry, char **error, int follow_links TSRMLS_DC) /* {{{ */
{
	php_stream_filter *filter;
	phar_archive_data *phar = entry->phar;
	char *filtername;
	off_t loc;
	php_stream *ufp;
	phar_entry_data dummy;

	if (follow_links && entry->link) {
		phar_entry_info *link_entry = phar_get_link_source(entry TSRMLS_CC);
		if (link_entry && link_entry != entry) {
			return phar_open_entry_fp(link_entry, error, 1 TSRMLS_CC);
		}
	}

	if (entry->is_modified) {
		return SUCCESS;
	}

	if (entry->fp_type == PHAR_TMP) {
		if (!entry->fp) {
			entry->fp = php_stream_open_wrapper(entry->tmp, "rb", STREAM_MUST_SEEK|0, NULL);
		}
		return SUCCESS;
	}

	if (entry->fp_type != PHAR_FP) {
		/* either newly created or already modified */
		return SUCCESS;
	}

	if (!phar_get_pharfp(phar TSRMLS_CC)) {
		if (FAILURE == phar_open_archive_fp(phar TSRMLS_CC)) {
			spprintf(error, 4096, "phar error: Cannot open phar archive \"%s\" for reading", phar->fname);
			return FAILURE;
		}
	}

	if ((entry->old_flags && !(entry->old_flags & PHAR_ENT_COMPRESSION_MASK)) || !(entry->flags & PHAR_ENT_COMPRESSION_MASK)) {
		dummy.internal_file = entry;
		dummy.phar = phar;
		dummy.zero = entry->offset;
		dummy.fp = phar_get_pharfp(phar TSRMLS_CC);
		if (FAILURE == phar_postprocess_file(&dummy, entry->crc32, error, 1 TSRMLS_CC)) {
			return FAILURE;
		}
		return SUCCESS;
	}

	if (!phar_get_entrypufp(entry TSRMLS_CC)) {
		phar_set_entrypufp(entry, php_stream_fopen_tmpfile() TSRMLS_CC);
		if (!phar_get_entrypufp(entry TSRMLS_CC)) {
			spprintf(error, 4096, "phar error: Cannot open temporary file for decompressing phar archive \"%s\" file \"%s\"", phar->fname, entry->filename);
			return FAILURE;
		}
	}

	dummy.internal_file = entry;
	dummy.phar = phar;
	dummy.zero = entry->offset;
	dummy.fp = phar_get_pharfp(phar TSRMLS_CC);
	if (FAILURE == phar_postprocess_file(&dummy, entry->crc32, error, 1 TSRMLS_CC)) {
		return FAILURE;
	}

	ufp = phar_get_entrypufp(entry TSRMLS_CC);

	if ((filtername = phar_decompress_filter(entry, 0)) != NULL) {
		filter = php_stream_filter_create(filtername, NULL, 0 TSRMLS_CC);
	} else {
		filter = NULL;
	}

	if (!filter) {
		spprintf(error, 4096, "phar error: unable to read phar \"%s\" (cannot create %s filter while decompressing file \"%s\")", phar->fname, phar_decompress_filter(entry, 1), entry->filename);
		return FAILURE;
	}

	/* now we can safely use proper decompression */
	/* save the new offset location within ufp */
	php_stream_seek(ufp, 0, SEEK_END);
	loc = php_stream_tell(ufp);
	php_stream_filter_append(&ufp->writefilters, filter);
	php_stream_seek(phar_get_entrypfp(entry TSRMLS_CC), phar_get_fp_offset(entry TSRMLS_CC), SEEK_SET);

	if (entry->uncompressed_filesize) {
		if (SUCCESS != phar_stream_copy_to_stream(phar_get_entrypfp(entry TSRMLS_CC), ufp, entry->compressed_filesize, NULL)) {
			spprintf(error, 4096, "phar error: internal corruption of phar \"%s\" (actual filesize mismatch on file \"%s\")", phar->fname, entry->filename);
			php_stream_filter_remove(filter, 1 TSRMLS_CC);
			return FAILURE;
		}
	}

	php_stream_filter_flush(filter, 1);
	php_stream_flush(ufp);
	php_stream_filter_remove(filter, 1 TSRMLS_CC);

	if (php_stream_tell(ufp) - loc != (off_t) entry->uncompressed_filesize) {
		spprintf(error, 4096, "phar error: internal corruption of phar \"%s\" (actual filesize mismatch on file \"%s\")", phar->fname, entry->filename);
		return FAILURE;
	}

	entry->old_flags = entry->flags;

	/* this is now the new location of the file contents within this fp */
	phar_set_fp_type(entry, PHAR_UFP, loc TSRMLS_CC);
	dummy.zero = entry->offset;
	dummy.fp = ufp;
	if (FAILURE == phar_postprocess_file(&dummy, entry->crc32, error, 0 TSRMLS_CC)) {
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

#if defined(PHP_VERSION_ID) && PHP_VERSION_ID < 50202
typedef struct {
	char        *data;
	size_t      fpos;
	size_t      fsize;
	size_t      smax;
	int         mode;
	php_stream  **owner_ptr;
} php_stream_memory_data;
#endif

int phar_create_writeable_entry(phar_archive_data *phar, phar_entry_info *entry, char **error TSRMLS_DC) /* {{{ */
{
	if (entry->fp_type == PHAR_MOD) {
		/* already newly created, truncate */
#if PHP_VERSION_ID >= 50202
		php_stream_truncate_set_size(entry->fp, 0);
#else
		if (php_stream_is(entry->fp, PHP_STREAM_IS_TEMP)) {
			if (php_stream_is(*(php_stream**)entry->fp->abstract, PHP_STREAM_IS_MEMORY)) {
				php_stream *inner = *(php_stream**)entry->fp->abstract;
				php_stream_memory_data *memfp = (php_stream_memory_data*)inner->abstract;
				memfp->fpos = 0;
				memfp->fsize = 0;
			} else if (php_stream_is(*(php_stream**)entry->fp->abstract, PHP_STREAM_IS_STDIO)) {
				php_stream_truncate_set_size(*(php_stream**)entry->fp->abstract, 0);
			} else {
				if (error) {
					spprintf(error, 0, "phar error: file \"%s\" cannot be opened for writing, no truncate support", phar->fname);
				}
				return FAILURE;
			}
		} else if (php_stream_is(entry->fp, PHP_STREAM_IS_STDIO)) {
			php_stream_truncate_set_size(entry->fp, 0);
		} else {
			if (error) {
				spprintf(error, 0, "phar error: file \"%s\" cannot be opened for writing, no truncate support", phar->fname);
			}
			return FAILURE;
		}
#endif
		entry->old_flags = entry->flags;
		entry->is_modified = 1;
		phar->is_modified = 1;
		/* reset file size */
		entry->uncompressed_filesize = 0;
		entry->compressed_filesize = 0;
		entry->crc32 = 0;
		entry->flags = PHAR_ENT_PERM_DEF_FILE;
		entry->fp_type = PHAR_MOD;
		entry->offset = 0;
		return SUCCESS;
	}

	if (error) {
		*error = NULL;
	}

	/* open a new temp file for writing */
	if (entry->link) {
		efree(entry->link);
		entry->link = NULL;
		entry->tar_type = (entry->is_tar ? TAR_FILE : '\0');
	}

	entry->fp = php_stream_fopen_tmpfile();

	if (!entry->fp) {
		if (error) {
			spprintf(error, 0, "phar error: unable to create temporary file");
		}
		return FAILURE;
	}

	entry->old_flags = entry->flags;
	entry->is_modified = 1;
	phar->is_modified = 1;
	/* reset file size */
	entry->uncompressed_filesize = 0;
	entry->compressed_filesize = 0;
	entry->crc32 = 0;
	entry->flags = PHAR_ENT_PERM_DEF_FILE;
	entry->fp_type = PHAR_MOD;
	entry->offset = 0;
	return SUCCESS;
}
/* }}} */

int phar_separate_entry_fp(phar_entry_info *entry, char **error TSRMLS_DC) /* {{{ */
{
	php_stream *fp;
	phar_entry_info *link;

	if (FAILURE == phar_open_entry_fp(entry, error, 1 TSRMLS_CC)) {
		return FAILURE;
	}

	if (entry->fp_type == PHAR_MOD) {
		return SUCCESS;
	}

	fp = php_stream_fopen_tmpfile();
	phar_seek_efp(entry, 0, SEEK_SET, 0, 1 TSRMLS_CC);
	link = phar_get_link_source(entry TSRMLS_CC);

	if (!link) {
		link = entry;
	}

	if (SUCCESS != phar_stream_copy_to_stream(phar_get_efp(link, 0 TSRMLS_CC), fp, link->uncompressed_filesize, NULL)) {
		if (error) {
			spprintf(error, 4096, "phar error: cannot separate entry file \"%s\" contents in phar archive \"%s\" for write access", entry->filename, entry->phar->fname);
		}
		return FAILURE;
	}

	if (entry->link) {
		efree(entry->link);
		entry->link = NULL;
		entry->tar_type = (entry->is_tar ? TAR_FILE : '\0');
	}

	entry->offset = 0;
	entry->fp = fp;
	entry->fp_type = PHAR_MOD;
	entry->is_modified = 1;
	return SUCCESS;
}
/* }}} */

/**
 * helper function to open an internal file's fp just-in-time
 */
phar_entry_info * phar_open_jit(phar_archive_data *phar, phar_entry_info *entry, char **error TSRMLS_DC) /* {{{ */
{
	if (error) {
		*error = NULL;
	}
	/* seek to start of internal file and read it */
	if (FAILURE == phar_open_entry_fp(entry, error, 1 TSRMLS_CC)) {
		return NULL;
	}
	if (-1 == phar_seek_efp(entry, 0, SEEK_SET, 0, 1 TSRMLS_CC)) {
		spprintf(error, 4096, "phar error: cannot seek to start of file \"%s\" in phar \"%s\"", entry->filename, phar->fname);
		return NULL;
	}
	return entry;
}
/* }}} */

int phar_free_alias(phar_archive_data *phar, char *alias, int alias_len TSRMLS_DC) /* {{{ */
{
	if (phar->refcount || phar->is_persistent) {
		return FAILURE;
	}

	/* this archive has no open references, so emit an E_STRICT and remove it */
	if (zend_hash_del(&(PHAR_GLOBALS->phar_fname_map), phar->fname, phar->fname_len) != SUCCESS) {
		return FAILURE;
	}

	/* invalidate phar cache */
	PHAR_G(last_phar) = NULL;
	PHAR_G(last_phar_name) = PHAR_G(last_alias) = NULL;

	return SUCCESS;
}
/* }}} */

/**
 * Looks up a phar archive in the filename map, connecting it to the alias
 * (if any) or returns null
 */
int phar_get_archive(phar_archive_data **archive, char *fname, int fname_len, char *alias, int alias_len, char **error TSRMLS_DC) /* {{{ */
{
	phar_archive_data *fd, **fd_ptr;
	char *my_realpath, *save;
	int save_len;
	ulong fhash, ahash = 0;

	phar_request_initialize(TSRMLS_C);

	if (error) {
		*error = NULL;
	}

	*archive = NULL;

	if (PHAR_G(last_phar) && fname_len == PHAR_G(last_phar_name_len) && !memcmp(fname, PHAR_G(last_phar_name), fname_len)) {
		*archive = PHAR_G(last_phar);
		if (alias && alias_len) {

			if (!PHAR_G(last_phar)->is_temporary_alias && (alias_len != PHAR_G(last_phar)->alias_len || memcmp(PHAR_G(last_phar)->alias, alias, alias_len))) {
				if (error) {
					spprintf(error, 0, "alias \"%s\" is already used for archive \"%s\" cannot be overloaded with \"%s\"", alias, PHAR_G(last_phar)->fname, fname);
				}
				*archive = NULL;
				return FAILURE;
			}

			if (PHAR_G(last_phar)->alias_len && SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_alias_map), PHAR_G(last_phar)->alias, PHAR_G(last_phar)->alias_len, (void**)&fd_ptr)) {
				zend_hash_del(&(PHAR_GLOBALS->phar_alias_map), PHAR_G(last_phar)->alias, PHAR_G(last_phar)->alias_len);
			}

			zend_hash_add(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len, (void*)&(*archive), sizeof(phar_archive_data*), NULL);
			PHAR_G(last_alias) = alias;
			PHAR_G(last_alias_len) = alias_len;
		}

		return SUCCESS;
	}

	if (alias && alias_len && PHAR_G(last_phar) && alias_len == PHAR_G(last_alias_len) && !memcmp(alias, PHAR_G(last_alias), alias_len)) {
		fd = PHAR_G(last_phar);
		fd_ptr = &fd;
		goto alias_success;
	}

	if (alias && alias_len) {
		ahash = zend_inline_hash_func(alias, alias_len);
		if (SUCCESS == zend_hash_quick_find(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len, ahash, (void**)&fd_ptr)) {
alias_success:
			if (fname && (fname_len != (*fd_ptr)->fname_len || strncmp(fname, (*fd_ptr)->fname, fname_len))) {
				if (error) {
					spprintf(error, 0, "alias \"%s\" is already used for archive \"%s\" cannot be overloaded with \"%s\"", alias, (*fd_ptr)->fname, fname);
				}
				if (SUCCESS == phar_free_alias(*fd_ptr, alias, alias_len TSRMLS_CC)) {
					efree(*error);
					*error = NULL;
				}
				return FAILURE;
			}

			*archive = *fd_ptr;
			fd = *fd_ptr;
			PHAR_G(last_phar) = fd;
			PHAR_G(last_phar_name) = fd->fname;
			PHAR_G(last_phar_name_len) = fd->fname_len;
			PHAR_G(last_alias) = alias;
			PHAR_G(last_alias_len) = alias_len;

			return SUCCESS;
		}

		if (PHAR_G(manifest_cached) && SUCCESS == zend_hash_quick_find(&cached_alias, alias, alias_len, ahash, (void **)&fd_ptr)) {
			goto alias_success;
		}
	}

	fhash = zend_inline_hash_func(fname, fname_len);
	my_realpath = NULL;
	save = fname;
	save_len = fname_len;

	if (fname && fname_len) {
		if (SUCCESS == zend_hash_quick_find(&(PHAR_GLOBALS->phar_fname_map), fname, fname_len, fhash, (void**)&fd_ptr)) {
			*archive = *fd_ptr;
			fd = *fd_ptr;

			if (alias && alias_len) {
				if (!fd->is_temporary_alias && (alias_len != fd->alias_len || memcmp(fd->alias, alias, alias_len))) {
					if (error) {
						spprintf(error, 0, "alias \"%s\" is already used for archive \"%s\" cannot be overloaded with \"%s\"", alias, (*fd_ptr)->fname, fname);
					}
					return FAILURE;
				}

				if (fd->alias_len && SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_alias_map), fd->alias, fd->alias_len, (void**)&fd_ptr)) {
					zend_hash_del(&(PHAR_GLOBALS->phar_alias_map), fd->alias, fd->alias_len);
				}

				zend_hash_quick_add(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len, ahash, (void*)&fd, sizeof(phar_archive_data*), NULL);
			}

			PHAR_G(last_phar) = fd;
			PHAR_G(last_phar_name) = fd->fname;
			PHAR_G(last_phar_name_len) = fd->fname_len;
			PHAR_G(last_alias) = fd->alias;
			PHAR_G(last_alias_len) = fd->alias_len;

			return SUCCESS;
		}

		if (PHAR_G(manifest_cached) && SUCCESS == zend_hash_quick_find(&cached_phars, fname, fname_len, fhash, (void**)&fd_ptr)) {
			*archive = *fd_ptr;
			fd = *fd_ptr;

			/* this could be problematic - alias should never be different from manifest alias
			   for cached phars */
			if (!fd->is_temporary_alias && alias && alias_len) {
				if (alias_len != fd->alias_len || memcmp(fd->alias, alias, alias_len)) {
					if (error) {
						spprintf(error, 0, "alias \"%s\" is already used for archive \"%s\" cannot be overloaded with \"%s\"", alias, (*fd_ptr)->fname, fname);
					}
					return FAILURE;
				}
			}

			PHAR_G(last_phar) = fd;
			PHAR_G(last_phar_name) = fd->fname;
			PHAR_G(last_phar_name_len) = fd->fname_len;
			PHAR_G(last_alias) = fd->alias;
			PHAR_G(last_alias_len) = fd->alias_len;

			return SUCCESS;
		}

		if (SUCCESS == zend_hash_quick_find(&(PHAR_GLOBALS->phar_alias_map), save, save_len, fhash, (void**)&fd_ptr)) {
			fd = *archive = *fd_ptr;

			PHAR_G(last_phar) = fd;
			PHAR_G(last_phar_name) = fd->fname;
			PHAR_G(last_phar_name_len) = fd->fname_len;
			PHAR_G(last_alias) = fd->alias;
			PHAR_G(last_alias_len) = fd->alias_len;

			return SUCCESS;
		}

		if (PHAR_G(manifest_cached) && SUCCESS == zend_hash_quick_find(&cached_alias, save, save_len, fhash, (void**)&fd_ptr)) {
			fd = *archive = *fd_ptr;

			PHAR_G(last_phar) = fd;
			PHAR_G(last_phar_name) = fd->fname;
			PHAR_G(last_phar_name_len) = fd->fname_len;
			PHAR_G(last_alias) = fd->alias;
			PHAR_G(last_alias_len) = fd->alias_len;

			return SUCCESS;
		}

		/* not found, try converting \ to / */
		my_realpath = expand_filepath(fname, my_realpath TSRMLS_CC);

		if (my_realpath) {
			fname_len = strlen(my_realpath);
			fname = my_realpath;
		} else {
			return FAILURE;
		}
#ifdef PHP_WIN32
		phar_unixify_path_separators(fname, fname_len);
#endif
		fhash = zend_inline_hash_func(fname, fname_len);

		if (SUCCESS == zend_hash_quick_find(&(PHAR_GLOBALS->phar_fname_map), fname, fname_len, fhash, (void**)&fd_ptr)) {
realpath_success:
			*archive = *fd_ptr;
			fd = *fd_ptr;

			if (alias && alias_len) {
				zend_hash_quick_add(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len, ahash, (void*)&fd, sizeof(phar_archive_data*), NULL);
			}

			efree(my_realpath);

			PHAR_G(last_phar) = fd;
			PHAR_G(last_phar_name) = fd->fname;
			PHAR_G(last_phar_name_len) = fd->fname_len;
			PHAR_G(last_alias) = fd->alias;
			PHAR_G(last_alias_len) = fd->alias_len;

			return SUCCESS;
		}

		if (PHAR_G(manifest_cached) && SUCCESS == zend_hash_quick_find(&cached_phars, fname, fname_len, fhash, (void**)&fd_ptr)) {
			goto realpath_success;
		}

		efree(my_realpath);
	}

	return FAILURE;
}
/* }}} */

/**
 * Determine which stream compression filter (if any) we need to read this file
 */
char * phar_compress_filter(phar_entry_info * entry, int return_unknown) /* {{{ */
{
	switch (entry->flags & PHAR_ENT_COMPRESSION_MASK) {
	case PHAR_ENT_COMPRESSED_GZ:
		return "zlib.deflate";
	case PHAR_ENT_COMPRESSED_BZ2:
		return "bzip2.compress";
	default:
		return return_unknown ? "unknown" : NULL;
	}
}
/* }}} */

/**
 * Determine which stream decompression filter (if any) we need to read this file
 */
char * phar_decompress_filter(phar_entry_info * entry, int return_unknown) /* {{{ */
{
	php_uint32 flags;

	if (entry->is_modified) {
		flags = entry->old_flags;
	} else {
		flags = entry->flags;
	}

	switch (flags & PHAR_ENT_COMPRESSION_MASK) {
		case PHAR_ENT_COMPRESSED_GZ:
			return "zlib.inflate";
		case PHAR_ENT_COMPRESSED_BZ2:
			return "bzip2.decompress";
		default:
			return return_unknown ? "unknown" : NULL;
	}
}
/* }}} */

/**
 * retrieve information on a file contained within a phar, or null if it ain't there
 */
phar_entry_info *phar_get_entry_info(phar_archive_data *phar, char *path, int path_len, char **error, int security TSRMLS_DC) /* {{{ */
{
	return phar_get_entry_info_dir(phar, path, path_len, 0, error, security TSRMLS_CC);
}
/* }}} */
/**
 * retrieve information on a file or directory contained within a phar, or null if none found
 * allow_dir is 0 for none, 1 for both empty directories in the phar and temp directories, and 2 for only
 * valid pre-existing empty directory entries
 */
phar_entry_info *phar_get_entry_info_dir(phar_archive_data *phar, char *path, int path_len, char dir, char **error, int security TSRMLS_DC) /* {{{ */
{
	const char *pcr_error;
	phar_entry_info *entry;
	int is_dir;

#ifdef PHP_WIN32
	phar_unixify_path_separators(path, path_len);
#endif

	is_dir = (path_len && (path[path_len - 1] == '/')) ? 1 : 0;

	if (error) {
		*error = NULL;
	}

	if (security && path_len >= sizeof(".phar")-1 && !memcmp(path, ".phar", sizeof(".phar")-1)) {
		if (error) {
			spprintf(error, 4096, "phar error: cannot directly access magic \".phar\" directory or files within it");
		}
		return NULL;
	}

	if (!path_len && !dir) {
		if (error) {
			spprintf(error, 4096, "phar error: invalid path \"%s\" must not be empty", path);
		}
		return NULL;
	}

	if (phar_path_check(&path, &path_len, &pcr_error) > pcr_is_ok) {
		if (error) {
			spprintf(error, 4096, "phar error: invalid path \"%s\" contains %s", path, pcr_error);
		}
		return NULL;
	}

	if (!phar->manifest.arBuckets) {
		return NULL;
	}

	if (is_dir) {
		if (!path_len || path_len == 1) {
			return NULL;
		}
		path_len--;
	}

	if (SUCCESS == zend_hash_find(&phar->manifest, path, path_len, (void**)&entry)) {
		if (entry->is_deleted) {
			/* entry is deleted, but has not been flushed to disk yet */
			return NULL;
		}
		if (entry->is_dir && !dir) {
			if (error) {
				spprintf(error, 4096, "phar error: path \"%s\" is a directory", path);
			}
			return NULL;
		}
		if (!entry->is_dir && dir == 2) {
			/* user requested a directory, we must return one */
			if (error) {
				spprintf(error, 4096, "phar error: path \"%s\" exists and is a not a directory", path);
			}
			return NULL;
		}
		return entry;
	}

	if (dir) {
		if (zend_hash_exists(&phar->virtual_dirs, path, path_len)) {
			/* a file or directory exists in a sub-directory of this path */
			entry = (phar_entry_info *) ecalloc(1, sizeof(phar_entry_info));
			/* this next line tells PharFileInfo->__destruct() to efree the filename */
			entry->is_temp_dir = entry->is_dir = 1;
			entry->filename = (char *) estrndup(path, path_len + 1);
			entry->filename_len = path_len;
			entry->phar = phar;
			return entry;
		}
	}

	if (phar->mounted_dirs.arBuckets && zend_hash_num_elements(&phar->mounted_dirs)) {
		phar_zstr key;
		char *str_key;
		ulong unused;
		uint keylen;

		zend_hash_internal_pointer_reset(&phar->mounted_dirs);
		while (FAILURE != zend_hash_has_more_elements(&phar->mounted_dirs)) {
			if (HASH_KEY_NON_EXISTANT == zend_hash_get_current_key_ex(&phar->mounted_dirs, &key, &keylen, &unused, 0, NULL)) {
				break;
			}

			PHAR_STR(key, str_key);

			if ((int)keylen >= path_len || strncmp(str_key, path, keylen)) {
				PHAR_STR_FREE(str_key);
				continue;
			} else {
				char *test;
				int test_len;
				php_stream_statbuf ssb;

				if (SUCCESS != zend_hash_find(&phar->manifest, str_key, keylen, (void **) &entry)) {
					if (error) {
						spprintf(error, 4096, "phar internal error: mounted path \"%s\" could not be retrieved from manifest", str_key);
					}
					PHAR_STR_FREE(str_key);
					return NULL;
				}

				if (!entry->tmp || !entry->is_mounted) {
					if (error) {
						spprintf(error, 4096, "phar internal error: mounted path \"%s\" is not properly initialized as a mounted path", str_key);
					}
					PHAR_STR_FREE(str_key);
					return NULL;
				}
				PHAR_STR_FREE(str_key);

				test_len = spprintf(&test, MAXPATHLEN, "%s%s", entry->tmp, path + keylen);

				if (SUCCESS != php_stream_stat_path(test, &ssb)) {
					efree(test);
					return NULL;
				}

				if (ssb.sb.st_mode & S_IFDIR && !dir) {
					efree(test);
					if (error) {
						spprintf(error, 4096, "phar error: path \"%s\" is a directory", path);
					}
					return NULL;
				}

				if ((ssb.sb.st_mode & S_IFDIR) == 0 && dir) {
					efree(test);
					/* user requested a directory, we must return one */
					if (error) {
						spprintf(error, 4096, "phar error: path \"%s\" exists and is a not a directory", path);
					}
					return NULL;
				}

				/* mount the file just in time */
				if (SUCCESS != phar_mount_entry(phar, test, test_len, path, path_len TSRMLS_CC)) {
					efree(test);
					if (error) {
						spprintf(error, 4096, "phar error: path \"%s\" exists as file \"%s\" and could not be mounted", path, test);
					}
					return NULL;
				}

				efree(test);

				if (SUCCESS != zend_hash_find(&phar->manifest, path, path_len, (void**)&entry)) {
					if (error) {
						spprintf(error, 4096, "phar error: path \"%s\" exists as file \"%s\" and could not be retrieved after being mounted", path, test);
					}
					return NULL;
				}
				return entry;
			}
		}
	}

	return NULL;
}
/* }}} */

static const char hexChars[] = "0123456789ABCDEF";

static int phar_hex_str(const char *digest, size_t digest_len, char **signature TSRMLS_DC) /* {{{ */
{
	int pos = -1;
	size_t len = 0;

	*signature = (char*)safe_pemalloc(digest_len, 2, 1, PHAR_G(persist));

	for (; len < digest_len; ++len) {
		(*signature)[++pos] = hexChars[((const unsigned char *)digest)[len] >> 4];
		(*signature)[++pos] = hexChars[((const unsigned char *)digest)[len] & 0x0F];
	}
	(*signature)[++pos] = '\0';
	return pos;
}
/* }}} */

#ifndef PHAR_HAVE_OPENSSL
static int phar_call_openssl_signverify(int is_sign, php_stream *fp, off_t end, char *key, int key_len, char **signature, int *signature_len TSRMLS_DC) /* {{{ */
{
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zval *zdata, *zsig, *zkey, *retval_ptr, **zp[3], *openssl;

	MAKE_STD_ZVAL(zdata);
	MAKE_STD_ZVAL(openssl);
	ZVAL_STRINGL(openssl, is_sign ? "openssl_sign" : "openssl_verify", is_sign ? sizeof("openssl_sign")-1 : sizeof("openssl_verify")-1, 1);
	MAKE_STD_ZVAL(zsig);
	ZVAL_STRINGL(zsig, *signature, *signature_len, 1);
	MAKE_STD_ZVAL(zkey);
	ZVAL_STRINGL(zkey, key, key_len, 1);
	zp[0] = &zdata;
	zp[1] = &zsig;
	zp[2] = &zkey;

	php_stream_rewind(fp);
	Z_TYPE_P(zdata) = IS_STRING;
	Z_STRLEN_P(zdata) = end;

#if PHP_MAJOR_VERSION > 5
	if (end != (off_t) php_stream_copy_to_mem(fp, (void **) &(Z_STRVAL_P(zdata)), (size_t) end, 0)) {
#else
	if (end != (off_t) php_stream_copy_to_mem(fp, &(Z_STRVAL_P(zdata)), (size_t) end, 0)) {
#endif
		zval_dtor(zdata);
		zval_dtor(zsig);
		zval_dtor(zkey);
		zval_dtor(openssl);
		efree(openssl);
		efree(zdata);
		efree(zkey);
		efree(zsig);
		return FAILURE;
	}

#if PHP_VERSION_ID < 50300
	if (FAILURE == zend_fcall_info_init(openssl, &fci, &fcc TSRMLS_CC)) {
#else
	if (FAILURE == zend_fcall_info_init(openssl, 0, &fci, &fcc, NULL, NULL TSRMLS_CC)) {
#endif
		zval_dtor(zdata);
		zval_dtor(zsig);
		zval_dtor(zkey);
		zval_dtor(openssl);
		efree(openssl);
		efree(zdata);
		efree(zkey);
		efree(zsig);
		return FAILURE;
	}

	fci.param_count = 3;
	fci.params = zp;
#if PHP_VERSION_ID < 50300
	++(zdata->refcount);
	if (!is_sign) {
		++(zsig->refcount);
	}
	++(zkey->refcount);
#else
	Z_ADDREF_P(zdata);
	if (is_sign) {
		Z_SET_ISREF_P(zsig);
	} else {
		Z_ADDREF_P(zsig);
	}
	Z_ADDREF_P(zkey);
#endif
	fci.retval_ptr_ptr = &retval_ptr;

	if (FAILURE == zend_call_function(&fci, &fcc TSRMLS_CC)) {
		zval_dtor(zdata);
		zval_dtor(zsig);
		zval_dtor(zkey);
		zval_dtor(openssl);
		efree(openssl);
		efree(zdata);
		efree(zkey);
		efree(zsig);
		return FAILURE;
	}

	zval_dtor(openssl);
	efree(openssl);
#if PHP_VERSION_ID < 50300
	--(zdata->refcount);
	if (!is_sign) {
		--(zsig->refcount);
	}
	--(zkey->refcount);
#else
	Z_DELREF_P(zdata);
	if (is_sign) {
		Z_UNSET_ISREF_P(zsig);
	} else {
		Z_DELREF_P(zsig);
	}
	Z_DELREF_P(zkey);
#endif
	zval_dtor(zdata);
	efree(zdata);
	zval_dtor(zkey);
	efree(zkey);

	switch (Z_TYPE_P(retval_ptr)) {
		default:
		case IS_LONG:
			zval_dtor(zsig);
			efree(zsig);
			if (1 == Z_LVAL_P(retval_ptr)) {
				efree(retval_ptr);
				return SUCCESS;
			}
			efree(retval_ptr);
			return FAILURE;
		case IS_BOOL:
			efree(retval_ptr);
			if (Z_BVAL_P(retval_ptr)) {
				*signature = estrndup(Z_STRVAL_P(zsig), Z_STRLEN_P(zsig));
				*signature_len = Z_STRLEN_P(zsig);
				zval_dtor(zsig);
				efree(zsig);
				return SUCCESS;
			}
			zval_dtor(zsig);
			efree(zsig);
			return FAILURE;
	}
}
/* }}} */
#endif /* #ifndef PHAR_HAVE_OPENSSL */

int phar_verify_signature(php_stream *fp, size_t end_of_phar, php_uint32 sig_type, char *sig, int sig_len, char *fname, char **signature, int *signature_len, char **error TSRMLS_DC) /* {{{ */
{
	int read_size, len;
	off_t read_len;
	unsigned char buf[1024];

	php_stream_rewind(fp);

	switch (sig_type) {
		case PHAR_SIG_OPENSSL: {
#ifdef PHAR_HAVE_OPENSSL
			BIO *in;
			EVP_PKEY *key;
			EVP_MD *mdtype = (EVP_MD *) EVP_sha1();
			EVP_MD_CTX md_ctx;
#else
			int tempsig;
#endif
			php_uint32 pubkey_len;
			char *pubkey = NULL, *pfile;
			php_stream *pfp;
#ifndef PHAR_HAVE_OPENSSL
			if (!zend_hash_exists(&module_registry, "openssl", sizeof("openssl"))) {
				if (error) {
					spprintf(error, 0, "openssl not loaded");
				}
				return FAILURE;
			}
#endif
			/* use __FILE__ . '.pubkey' for public key file */
			spprintf(&pfile, 0, "%s.pubkey", fname);
			pfp = php_stream_open_wrapper(pfile, "rb", 0, NULL);
			efree(pfile);

#if PHP_MAJOR_VERSION > 5
			if (!pfp || !(pubkey_len = php_stream_copy_to_mem(pfp, (void **) &pubkey, PHP_STREAM_COPY_ALL, 0)) || !pubkey) {
#else
			if (!pfp || !(pubkey_len = php_stream_copy_to_mem(pfp, &pubkey, PHP_STREAM_COPY_ALL, 0)) || !pubkey) {
#endif
				if (pfp) {
					php_stream_close(pfp);
				}
				if (error) {
					spprintf(error, 0, "openssl public key could not be read");
				}
				return FAILURE;
			}

			php_stream_close(pfp);
#ifndef PHAR_HAVE_OPENSSL
			tempsig = sig_len;

			if (FAILURE == phar_call_openssl_signverify(0, fp, end_of_phar, pubkey, pubkey_len, &sig, &tempsig TSRMLS_CC)) {
				if (pubkey) {
					efree(pubkey);
				}

				if (error) {
					spprintf(error, 0, "openssl signature could not be verified");
				}

				return FAILURE;
			}

			if (pubkey) {
				efree(pubkey);
			}

			sig_len = tempsig;
#else
			in = BIO_new_mem_buf(pubkey, pubkey_len);

			if (NULL == in) {
				efree(pubkey);
				if (error) {
					spprintf(error, 0, "openssl signature could not be processed");
				}
				return FAILURE;
			}

			key = PEM_read_bio_PUBKEY(in, NULL,NULL, NULL);
			BIO_free(in);
			efree(pubkey);

			if (NULL == key) {
				if (error) {
					spprintf(error, 0, "openssl signature could not be processed");
				}
				return FAILURE;
			}

			EVP_VerifyInit(&md_ctx, mdtype);
			read_len = end_of_phar;

			if (read_len > sizeof(buf)) {
				read_size = sizeof(buf);
			} else {
				read_size = (int)read_len;
			}

			php_stream_seek(fp, 0, SEEK_SET);

			while (read_size && (len = php_stream_read(fp, (char*)buf, read_size)) > 0) {
				EVP_VerifyUpdate (&md_ctx, buf, len);
				read_len -= (off_t)len;

				if (read_len < read_size) {
					read_size = (int)read_len;
				}
			}

			if (EVP_VerifyFinal(&md_ctx, (unsigned char *)sig, sig_len, key) != 1) {
				/* 1: signature verified, 0: signature does not match, -1: failed signature operation */
				EVP_MD_CTX_cleanup(&md_ctx);

				if (error) {
					spprintf(error, 0, "broken openssl signature");
				}

				return FAILURE;
			}

			EVP_MD_CTX_cleanup(&md_ctx);
#endif

			*signature_len = phar_hex_str((const char*)sig, sig_len, signature TSRMLS_CC);
		}
		break;
#ifdef PHAR_HASH_OK
		case PHAR_SIG_SHA512: {
			unsigned char digest[64];
			PHP_SHA512_CTX context;

			PHP_SHA512Init(&context);
			read_len = end_of_phar;

			if (read_len > sizeof(buf)) {
				read_size = sizeof(buf);
			} else {
				read_size = (int)read_len;
			}

			while ((len = php_stream_read(fp, (char*)buf, read_size)) > 0) {
				PHP_SHA512Update(&context, buf, len);
				read_len -= (off_t)len;
				if (read_len < read_size) {
					read_size = (int)read_len;
				}
			}

			PHP_SHA512Final(digest, &context);

			if (memcmp(digest, sig, sizeof(digest))) {
				if (error) {
					spprintf(error, 0, "broken signature");
				}
				return FAILURE;
			}

			*signature_len = phar_hex_str((const char*)digest, sizeof(digest), signature TSRMLS_CC);
			break;
		}
		case PHAR_SIG_SHA256: {
			unsigned char digest[32];
			PHP_SHA256_CTX context;

			PHP_SHA256Init(&context);
			read_len = end_of_phar;

			if (read_len > sizeof(buf)) {
				read_size = sizeof(buf);
			} else {
				read_size = (int)read_len;
			}

			while ((len = php_stream_read(fp, (char*)buf, read_size)) > 0) {
				PHP_SHA256Update(&context, buf, len);
				read_len -= (off_t)len;
				if (read_len < read_size) {
					read_size = (int)read_len;
				}
			}

			PHP_SHA256Final(digest, &context);

			if (memcmp(digest, sig, sizeof(digest))) {
				if (error) {
					spprintf(error, 0, "broken signature");
				}
				return FAILURE;
			}

			*signature_len = phar_hex_str((const char*)digest, sizeof(digest), signature TSRMLS_CC);
			break;
		}
#else
		case PHAR_SIG_SHA512:
		case PHAR_SIG_SHA256:
			if (error) {
				spprintf(error, 0, "unsupported signature");
			}
			return FAILURE;
#endif
		case PHAR_SIG_SHA1: {
			unsigned char digest[20];
			PHP_SHA1_CTX  context;

			PHP_SHA1Init(&context);
			read_len = end_of_phar;

			if (read_len > sizeof(buf)) {
				read_size = sizeof(buf);
			} else {
				read_size = (int)read_len;
			}

			while ((len = php_stream_read(fp, (char*)buf, read_size)) > 0) {
				PHP_SHA1Update(&context, buf, len);
				read_len -= (off_t)len;
				if (read_len < read_size) {
					read_size = (int)read_len;
				}
			}

			PHP_SHA1Final(digest, &context);

			if (memcmp(digest, sig, sizeof(digest))) {
				if (error) {
					spprintf(error, 0, "broken signature");
				}
				return FAILURE;
			}

			*signature_len = phar_hex_str((const char*)digest, sizeof(digest), signature TSRMLS_CC);
			break;
		}
		case PHAR_SIG_MD5: {
			unsigned char digest[16];
			PHP_MD5_CTX   context;

			PHP_MD5Init(&context);
			read_len = end_of_phar;

			if (read_len > sizeof(buf)) {
				read_size = sizeof(buf);
			} else {
				read_size = (int)read_len;
			}

			while ((len = php_stream_read(fp, (char*)buf, read_size)) > 0) {
				PHP_MD5Update(&context, buf, len);
				read_len -= (off_t)len;
				if (read_len < read_size) {
					read_size = (int)read_len;
				}
			}

			PHP_MD5Final(digest, &context);

			if (memcmp(digest, sig, sizeof(digest))) {
				if (error) {
					spprintf(error, 0, "broken signature");
				}
				return FAILURE;
			}

			*signature_len = phar_hex_str((const char*)digest, sizeof(digest), signature TSRMLS_CC);
			break;
		}
		default:
			if (error) {
				spprintf(error, 0, "broken or unsupported signature");
			}
			return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

int phar_create_signature(phar_archive_data *phar, php_stream *fp, char **signature, int *signature_length, char **error TSRMLS_DC) /* {{{ */
{
	unsigned char buf[1024];
	int sig_len;

	php_stream_rewind(fp);

	if (phar->signature) {
		efree(phar->signature);
		phar->signature = NULL;
	}

	switch(phar->sig_flags) {
#ifdef PHAR_HASH_OK
		case PHAR_SIG_SHA512: {
			unsigned char digest[64];
			PHP_SHA512_CTX context;

			PHP_SHA512Init(&context);

			while ((sig_len = php_stream_read(fp, (char*)buf, sizeof(buf))) > 0) {
				PHP_SHA512Update(&context, buf, sig_len);
			}

			PHP_SHA512Final(digest, &context);
			*signature = estrndup((char *) digest, 64);
			*signature_length = 64;
			break;
		}
		case PHAR_SIG_SHA256: {
			unsigned char digest[32];
			PHP_SHA256_CTX  context;

			PHP_SHA256Init(&context);

			while ((sig_len = php_stream_read(fp, (char*)buf, sizeof(buf))) > 0) {
				PHP_SHA256Update(&context, buf, sig_len);
			}

			PHP_SHA256Final(digest, &context);
			*signature = estrndup((char *) digest, 32);
			*signature_length = 32;
			break;
		}
#else
		case PHAR_SIG_SHA512:
		case PHAR_SIG_SHA256:
			if (error) {
				spprintf(error, 0, "unable to write to phar \"%s\" with requested hash type", phar->fname);
			}

			return FAILURE;
#endif
		case PHAR_SIG_OPENSSL: {
			int siglen;
			unsigned char *sigbuf;
#ifdef PHAR_HAVE_OPENSSL
			BIO *in;
			EVP_PKEY *key;
			EVP_MD_CTX *md_ctx;

			in = BIO_new_mem_buf(PHAR_G(openssl_privatekey), PHAR_G(openssl_privatekey_len));

			if (in == NULL) {
				if (error) {
					spprintf(error, 0, "unable to write to phar \"%s\" with requested openssl signature", phar->fname);
				}
				return FAILURE;
			}

			key = PEM_read_bio_PrivateKey(in, NULL,NULL, "");
			BIO_free(in);

			if (!key) {
				if (error) {
					spprintf(error, 0, "unable to process private key");
				}
				return FAILURE;
			}

			md_ctx = EVP_MD_CTX_create();

			siglen = EVP_PKEY_size(key);
			sigbuf = emalloc(siglen + 1);

			if (!EVP_SignInit(md_ctx, EVP_sha1())) {
				efree(sigbuf);
				if (error) {
					spprintf(error, 0, "unable to initialize openssl signature for phar \"%s\"", phar->fname);
				}
				return FAILURE;
			}

			while ((sig_len = php_stream_read(fp, (char*)buf, sizeof(buf))) > 0) {
				if (!EVP_SignUpdate(md_ctx, buf, sig_len)) {
					efree(sigbuf);
					if (error) {
						spprintf(error, 0, "unable to update the openssl signature for phar \"%s\"", phar->fname);
					}
					return FAILURE;
				}
			}

			if (!EVP_SignFinal (md_ctx, sigbuf,(unsigned int *)&siglen, key)) {
				efree(sigbuf);
				if (error) {
					spprintf(error, 0, "unable to write phar \"%s\" with requested openssl signature", phar->fname);
				}
				return FAILURE;
			}

			sigbuf[siglen] = '\0';
			EVP_MD_CTX_destroy(md_ctx);
#else
			sigbuf = NULL;
			siglen = 0;
			php_stream_seek(fp, 0, SEEK_END);

			if (FAILURE == phar_call_openssl_signverify(1, fp, php_stream_tell(fp), PHAR_G(openssl_privatekey), PHAR_G(openssl_privatekey_len), (char **)&sigbuf, &siglen TSRMLS_CC)) {
				if (error) {
					spprintf(error, 0, "unable to write phar \"%s\" with requested openssl signature", phar->fname);
				}
				return FAILURE;
			}
#endif
			*signature = (char *) sigbuf;
			*signature_length = siglen;
		}
		break;
		default:
			phar->sig_flags = PHAR_SIG_SHA1;
		case PHAR_SIG_SHA1: {
			unsigned char digest[20];
			PHP_SHA1_CTX  context;

			PHP_SHA1Init(&context);

			while ((sig_len = php_stream_read(fp, (char*)buf, sizeof(buf))) > 0) {
				PHP_SHA1Update(&context, buf, sig_len);
			}

			PHP_SHA1Final(digest, &context);
			*signature = estrndup((char *) digest, 20);
			*signature_length = 20;
			break;
		}
		case PHAR_SIG_MD5: {
			unsigned char digest[16];
			PHP_MD5_CTX   context;

			PHP_MD5Init(&context);

			while ((sig_len = php_stream_read(fp, (char*)buf, sizeof(buf))) > 0) {
				PHP_MD5Update(&context, buf, sig_len);
			}

			PHP_MD5Final(digest, &context);
			*signature = estrndup((char *) digest, 16);
			*signature_length = 16;
			break;
		}
	}

	phar->sig_len = phar_hex_str((const char *)*signature, *signature_length, &phar->signature TSRMLS_CC);
	return SUCCESS;
}
/* }}} */

void phar_add_virtual_dirs(phar_archive_data *phar, char *filename, int filename_len TSRMLS_DC) /* {{{ */
{
	const char *s;

	while ((s = zend_memrchr(filename, '/', filename_len))) {
		filename_len = s - filename;
		if (FAILURE == zend_hash_add_empty_element(&phar->virtual_dirs, filename, filename_len)) {
			break;
		}
	}
}
/* }}} */

static int phar_update_cached_entry(void *data, void *argument) /* {{{ */
{
	phar_entry_info *entry = (phar_entry_info *)data;
	TSRMLS_FETCH();

	entry->phar = (phar_archive_data *)argument;

	if (entry->link) {
		entry->link = estrdup(entry->link);
	}

	if (entry->tmp) {
		entry->tmp = estrdup(entry->tmp);
	}

	entry->metadata_str.c = 0;
	entry->filename = estrndup(entry->filename, entry->filename_len);
	entry->is_persistent = 0;

	if (entry->metadata) {
		if (entry->metadata_len) {
			char *buf = estrndup((char *) entry->metadata, entry->metadata_len);
			/* assume success, we would have failed before */
			phar_parse_metadata((char **) &buf, &entry->metadata, entry->metadata_len TSRMLS_CC);
			efree(buf);
		} else {
			zval *t;

			t = entry->metadata;
			ALLOC_ZVAL(entry->metadata);
			*entry->metadata = *t;
			zval_copy_ctor(entry->metadata);
#if PHP_VERSION_ID < 50300
			entry->metadata->refcount = 1;
#else
			Z_SET_REFCOUNT_P(entry->metadata, 1);
#endif
			entry->metadata_str.c = NULL;
			entry->metadata_str.len = 0;
		}
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

static void phar_copy_cached_phar(phar_archive_data **pphar TSRMLS_DC) /* {{{ */
{
	phar_archive_data *phar;
	HashTable newmanifest;
	char *fname;
	phar_archive_object **objphar;

	phar = (phar_archive_data *) emalloc(sizeof(phar_archive_data));
	*phar = **pphar;
	phar->is_persistent = 0;
	fname = phar->fname;
	phar->fname = estrndup(phar->fname, phar->fname_len);
	phar->ext = phar->fname + (phar->ext - fname);

	if (phar->alias) {
		phar->alias = estrndup(phar->alias, phar->alias_len);
	}

	if (phar->signature) {
		phar->signature = estrdup(phar->signature);
	}

	if (phar->metadata) {
		/* assume success, we would have failed before */
		if (phar->metadata_len) {
			char *buf = estrndup((char *) phar->metadata, phar->metadata_len);
			phar_parse_metadata(&buf, &phar->metadata, phar->metadata_len TSRMLS_CC);
			efree(buf);
		} else {
			zval *t;

			t = phar->metadata;
			ALLOC_ZVAL(phar->metadata);
			*phar->metadata = *t;
			zval_copy_ctor(phar->metadata);
#if PHP_VERSION_ID < 50300
			phar->metadata->refcount = 1;
#else
			Z_SET_REFCOUNT_P(phar->metadata, 1);
#endif
		}
	}

	zend_hash_init(&newmanifest, sizeof(phar_entry_info),
		zend_get_hash_value, destroy_phar_manifest_entry, 0);
	zend_hash_copy(&newmanifest, &(*pphar)->manifest, NULL, NULL, sizeof(phar_entry_info));
	zend_hash_apply_with_argument(&newmanifest, (apply_func_arg_t) phar_update_cached_entry, (void *)phar TSRMLS_CC);
	phar->manifest = newmanifest;
	zend_hash_init(&phar->mounted_dirs, sizeof(char *),
		zend_get_hash_value, NULL, 0);
	zend_hash_init(&phar->virtual_dirs, sizeof(char *),
		zend_get_hash_value, NULL, 0);
	zend_hash_copy(&phar->virtual_dirs, &(*pphar)->virtual_dirs, NULL, NULL, sizeof(void *));
	*pphar = phar;

	/* now, scan the list of persistent Phar objects referencing this phar and update the pointers */
	for (zend_hash_internal_pointer_reset(&PHAR_GLOBALS->phar_persist_map);
	SUCCESS == zend_hash_get_current_data(&PHAR_GLOBALS->phar_persist_map, (void **) &objphar);
	zend_hash_move_forward(&PHAR_GLOBALS->phar_persist_map)) {
		if (objphar[0]->arc.archive->fname_len == phar->fname_len && !memcmp(objphar[0]->arc.archive->fname, phar->fname, phar->fname_len)) {
			objphar[0]->arc.archive = phar;
		}
	}
}
/* }}} */

int phar_copy_on_write(phar_archive_data **pphar TSRMLS_DC) /* {{{ */
{
	phar_archive_data **newpphar, *newphar = NULL;

	if (SUCCESS != zend_hash_add(&(PHAR_GLOBALS->phar_fname_map), (*pphar)->fname, (*pphar)->fname_len, (void *)&newphar, sizeof(phar_archive_data *), (void **)&newpphar)) {
		return FAILURE;
	}

	*newpphar = *pphar;
	phar_copy_cached_phar(newpphar TSRMLS_CC);
	/* invalidate phar cache */
	PHAR_G(last_phar) = NULL;
	PHAR_G(last_phar_name) = PHAR_G(last_alias) = NULL;

	if (newpphar[0]->alias_len && FAILURE == zend_hash_add(&(PHAR_GLOBALS->phar_alias_map), newpphar[0]->alias, newpphar[0]->alias_len, (void*)newpphar, sizeof(phar_archive_data*), NULL)) {
		zend_hash_del(&(PHAR_GLOBALS->phar_fname_map), (*pphar)->fname, (*pphar)->fname_len);
		return FAILURE;
	}

	*pphar = *newpphar;
	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
