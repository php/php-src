/*
  +----------------------------------------------------------------------+
  | phar php single-file executable PHP extension                        |
  | utility functions                                                    |
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Gregory Beaver <cellog@php.net>                             |
  |          Marcus Boerger <helly@php.net>                              |
  +----------------------------------------------------------------------+
*/

#include "phar_internal.h"
#include "ext/hash/php_hash_sha.h"

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
static int phar_call_openssl_signverify(int is_sign, php_stream *fp, zend_off_t end, char *key, size_t key_len, char **signature, size_t *signature_len, php_uint32 sig_type);
#endif

/* for links to relative location, prepend cwd of the entry */
static char *phar_get_link_location(phar_entry_info *entry) /* {{{ */
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

phar_entry_info *phar_get_link_source(phar_entry_info *entry) /* {{{ */
{
	phar_entry_info *link_entry;
	char *link;

	if (!entry->link) {
		return entry;
	}

	link = phar_get_link_location(entry);
	if (NULL != (link_entry = zend_hash_str_find_ptr(&(entry->phar->manifest), entry->link, strlen(entry->link))) ||
		NULL != (link_entry = zend_hash_str_find_ptr(&(entry->phar->manifest), link, strlen(link)))) {
		if (link != entry->link) {
			efree(link);
		}
		return phar_get_link_source(link_entry);
	} else {
		if (link != entry->link) {
			efree(link);
		}
		return NULL;
	}
}
/* }}} */

/* retrieve a phar_entry_info's current file pointer for reading contents */
php_stream *phar_get_efp(phar_entry_info *entry, int follow_links) /* {{{ */
{
	if (follow_links && entry->link) {
		phar_entry_info *link_entry = phar_get_link_source(entry);

		if (link_entry && link_entry != entry) {
			return phar_get_efp(link_entry, 1);
		}
	}

	if (phar_get_fp_type(entry) == PHAR_FP) {
		if (!phar_get_entrypfp(entry)) {
			/* re-open just in time for cases where our refcount reached 0 on the phar archive */
			phar_open_archive_fp(entry->phar);
		}
		return phar_get_entrypfp(entry);
	} else if (phar_get_fp_type(entry) == PHAR_UFP) {
		return phar_get_entrypufp(entry);
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

int phar_seek_efp(phar_entry_info *entry, zend_off_t offset, int whence, zend_off_t position, int follow_links) /* {{{ */
{
	php_stream *fp = phar_get_efp(entry, follow_links);
	zend_off_t temp, eoffset;

	if (!fp) {
		return -1;
	}

	if (follow_links) {
		phar_entry_info *t;
		t = phar_get_link_source(entry);
		if (t) {
			entry = t;
		}
	}

	if (entry->is_dir) {
		return 0;
	}

	eoffset = phar_get_fp_offset(entry);

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

	if (temp > eoffset + (zend_off_t) entry->uncompressed_filesize) {
		return -1;
	}

	if (temp < eoffset) {
		return -1;
	}

	return php_stream_seek(fp, temp, SEEK_SET);
}
/* }}} */

/* mount an absolute path or uri to a path internal to the phar archive */
int phar_mount_entry(phar_archive_data *phar, char *filename, size_t filename_len, char *path, size_t path_len) /* {{{ */
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
		entry.tmp = expand_filepath(filename, NULL);
		if (!entry.tmp) {
			entry.tmp = estrndup(filename, filename_len);
		}
	}
	filename = entry.tmp;

	/* only check openbasedir for files, not for phar streams */
	if (!is_phar && php_check_open_basedir(filename)) {
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
		if (NULL == zend_hash_str_add_ptr(&phar->mounted_dirs, entry.filename, path_len, entry.filename)) {
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

	if (NULL != zend_hash_str_add_mem(&phar->manifest, entry.filename, path_len, (void*)&entry, sizeof(phar_entry_info))) {
		return SUCCESS;
	}

	efree(entry.tmp);
	efree(entry.filename);
	return FAILURE;
}
/* }}} */

zend_string *phar_find_in_include_path(zend_string *filename, phar_archive_data **pphar) /* {{{ */
{
	zend_string *ret;
	char *path, *arch, *entry, *test;
	size_t arch_len, entry_len;
	phar_archive_data *phar;

	if (pphar) {
		*pphar = NULL;
	} else {
		pphar = &phar;
	}

	if (!zend_is_executing() || !PHAR_G(cwd)) {
		return NULL;
	}

	zend_string *fname = zend_get_executed_filename_ex();
	if (!fname) {
		return NULL;
	}

	bool is_file_a_phar_wrapper = zend_string_starts_with_literal_ci(fname, "phar://");
	size_t length_phar_protocol = strlen("phar://");

	if (
		PHAR_G(last_phar)
		&& is_file_a_phar_wrapper
		&& ZSTR_LEN(fname) - length_phar_protocol >= PHAR_G(last_phar_name_len)
		&& !memcmp(ZSTR_VAL(fname) + length_phar_protocol, PHAR_G(last_phar_name), PHAR_G(last_phar_name_len))
	) {
		arch = estrndup(PHAR_G(last_phar_name), PHAR_G(last_phar_name_len));
		arch_len = PHAR_G(last_phar_name_len);
		phar = PHAR_G(last_phar);
		goto splitted;
	}

	if (!is_file_a_phar_wrapper || SUCCESS != phar_split_fname(ZSTR_VAL(fname), ZSTR_LEN(fname), &arch, &arch_len, &entry, &entry_len, 1, 0)) {
		return NULL;
	}

	efree(entry);

	if (*ZSTR_VAL(filename) == '.') {
		size_t try_len;

		if (FAILURE == phar_get_archive(&phar, arch, arch_len, NULL, 0, NULL)) {
			efree(arch);
			return NULL;
		}
splitted:
		if (pphar) {
			*pphar = phar;
		}

		try_len = ZSTR_LEN(filename);
		test = phar_fix_filepath(estrndup(ZSTR_VAL(filename), ZSTR_LEN(filename)), &try_len, 1);

		if (*test == '/') {
			if (zend_hash_str_exists(&(phar->manifest), test + 1, try_len - 1)) {
				ret = strpprintf(0, "phar://%s%s", arch, test);
				efree(arch);
				efree(test);
				return ret;
			}
		} else {
			if (zend_hash_str_exists(&(phar->manifest), test, try_len)) {
				ret = strpprintf(0, "phar://%s/%s", arch, test);
				efree(arch);
				efree(test);
				return ret;
			}
		}
		efree(test);
	}

	spprintf(&path, MAXPATHLEN + 1 + strlen(PG(include_path)), "phar://%s/%s%c%s", arch, PHAR_G(cwd), DEFAULT_DIR_SEPARATOR, PG(include_path));
	efree(arch);
	ret = php_resolve_path(ZSTR_VAL(filename), ZSTR_LEN(filename), path);
	efree(path);

	if (ret && zend_string_starts_with_literal_ci(ret, "phar://")) {
		/* found phar:// */
		if (SUCCESS != phar_split_fname(ZSTR_VAL(ret), ZSTR_LEN(ret), &arch, &arch_len, &entry, &entry_len, 1, 0)) {
			return ret;
		}

		*pphar = zend_hash_str_find_ptr(&(PHAR_G(phar_fname_map)), arch, arch_len);

		if (!*pphar && PHAR_G(manifest_cached)) {
			*pphar = zend_hash_str_find_ptr(&cached_phars, arch, arch_len);
		}

		efree(arch);
		efree(entry);
	}

	return ret;
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
int phar_get_entry_data(phar_entry_data **ret, char *fname, size_t fname_len, char *path, size_t path_len, const char *mode, char allow_dir, char **error, int security) /* {{{ */
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

	if (FAILURE == phar_get_archive(&phar, fname, fname_len, NULL, 0, error)) {
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
		if ((entry = phar_get_entry_info_dir(phar, path, path_len, allow_dir, for_create && !PHAR_G(readonly) && !phar->is_data ? NULL : error, security)) == NULL) {
			if (for_create && (!PHAR_G(readonly) || phar->is_data)) {
				return SUCCESS;
			}
			return FAILURE;
		}
	} else {
		if ((entry = phar_get_entry_info(phar, path, path_len, for_create && !PHAR_G(readonly) && !phar->is_data ? NULL : error, security)) == NULL) {
			if (for_create && (!PHAR_G(readonly) || phar->is_data)) {
				return SUCCESS;
			}
			return FAILURE;
		}
	}

	if (for_write && phar->is_persistent) {
		if (FAILURE == phar_copy_on_write(&phar)) {
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
			if (FAILURE == phar_create_writeable_entry(phar, entry, error)) {
				return FAILURE;
			}
		} else if (for_append) {
			phar_seek_efp(entry, 0, SEEK_END, 0, 0);
		}
	} else {
		if (for_write) {
			if (entry->link) {
				efree(entry->link);
				entry->link = NULL;
				entry->tar_type = (entry->is_tar ? TAR_FILE : '\0');
			}

			if (for_trunc) {
				if (FAILURE == phar_create_writeable_entry(phar, entry, error)) {
					return FAILURE;
				}
			} else {
				if (FAILURE == phar_separate_entry_fp(entry, error)) {
					return FAILURE;
				}
			}
		} else {
			if (FAILURE == phar_open_entry_fp(entry, error, 1)) {
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
	(*ret)->fp = phar_get_efp(entry, 1);
	if (entry->link) {
		phar_entry_info *link = phar_get_link_source(entry);
		if(!link) {
			efree(*ret);
			return FAILURE;
		}
		(*ret)->zero = phar_get_fp_offset(link);
	} else {
		(*ret)->zero = phar_get_fp_offset(entry);
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
phar_entry_data *phar_get_or_create_entry_data(char *fname, size_t fname_len, char *path, size_t path_len, const char *mode, char allow_dir, char **error, int security) /* {{{ */
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

	if (FAILURE == phar_get_archive(&phar, fname, fname_len, NULL, 0, error)) {
		return NULL;
	}

	if (FAILURE == phar_get_entry_data(&ret, fname, fname_len, path, path_len, mode, allow_dir, error, security)) {
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

	if (phar->is_persistent && FAILURE == phar_copy_on_write(&phar)) {
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
	if (is_dir && path_len) {
		etemp.filename_len--; /* strip trailing / */
		path_len--;
	}

	phar_add_virtual_dirs(phar, path, path_len);
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

	if (NULL == (entry = zend_hash_str_add_mem(&phar->manifest, etemp.filename, path_len, (void*)&etemp, sizeof(phar_entry_info)))) {
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
int phar_open_archive_fp(phar_archive_data *phar) /* {{{ */
{
	if (phar_get_pharfp(phar)) {
		return SUCCESS;
	}

	if (php_check_open_basedir(phar->fname)) {
		return FAILURE;
	}

	phar_set_pharfp(phar, php_stream_open_wrapper(phar->fname, "rb", IGNORE_URL|STREAM_MUST_SEEK|0, NULL));

	if (!phar_get_pharfp(phar)) {
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

/* copy file data from an existing to a new phar_entry_info that is not in the manifest */
int phar_copy_entry_fp(phar_entry_info *source, phar_entry_info *dest, char **error) /* {{{ */
{
	phar_entry_info *link;

	if (FAILURE == phar_open_entry_fp(source, error, 1)) {
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
	if (dest->fp == NULL) {
		spprintf(error, 0, "phar error: unable to create temporary file");
		return EOF;
	}
	phar_seek_efp(source, 0, SEEK_SET, 0, 1);
	link = phar_get_link_source(source);

	if (!link) {
		link = source;
	}

	if (SUCCESS != php_stream_copy_to_stream_ex(phar_get_efp(link, 0), dest->fp, link->uncompressed_filesize, NULL)) {
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
int phar_open_entry_fp(phar_entry_info *entry, char **error, int follow_links) /* {{{ */
{
	php_stream_filter *filter;
	phar_archive_data *phar = entry->phar;
	char *filtername;
	zend_off_t loc;
	php_stream *ufp;
	phar_entry_data dummy;

	if (follow_links && entry->link) {
		phar_entry_info *link_entry = phar_get_link_source(entry);
		if (link_entry && link_entry != entry) {
			return phar_open_entry_fp(link_entry, error, 1);
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

	if (!phar_get_pharfp(phar)) {
		if (FAILURE == phar_open_archive_fp(phar)) {
			spprintf(error, 4096, "phar error: Cannot open phar archive \"%s\" for reading", phar->fname);
			return FAILURE;
		}
	}

	if ((entry->old_flags && !(entry->old_flags & PHAR_ENT_COMPRESSION_MASK)) || !(entry->flags & PHAR_ENT_COMPRESSION_MASK)) {
		dummy.internal_file = entry;
		dummy.phar = phar;
		dummy.zero = entry->offset;
		dummy.fp = phar_get_pharfp(phar);
		if (FAILURE == phar_postprocess_file(&dummy, entry->crc32, error, 1)) {
			return FAILURE;
		}
		return SUCCESS;
	}

	if (!phar_get_entrypufp(entry)) {
		phar_set_entrypufp(entry, php_stream_fopen_tmpfile());
		if (!phar_get_entrypufp(entry)) {
			spprintf(error, 4096, "phar error: Cannot open temporary file for decompressing phar archive \"%s\" file \"%s\"", phar->fname, entry->filename);
			return FAILURE;
		}
	}

	dummy.internal_file = entry;
	dummy.phar = phar;
	dummy.zero = entry->offset;
	dummy.fp = phar_get_pharfp(phar);
	if (FAILURE == phar_postprocess_file(&dummy, entry->crc32, error, 1)) {
		return FAILURE;
	}

	ufp = phar_get_entrypufp(entry);

	if ((filtername = phar_decompress_filter(entry, 0)) != NULL) {
		filter = php_stream_filter_create(filtername, NULL, 0);
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
	php_stream_seek(phar_get_entrypfp(entry), phar_get_fp_offset(entry), SEEK_SET);

	if (entry->uncompressed_filesize) {
		if (SUCCESS != php_stream_copy_to_stream_ex(phar_get_entrypfp(entry), ufp, entry->compressed_filesize, NULL)) {
			spprintf(error, 4096, "phar error: internal corruption of phar \"%s\" (actual filesize mismatch on file \"%s\")", phar->fname, entry->filename);
			php_stream_filter_remove(filter, 1);
			return FAILURE;
		}
	}

	php_stream_filter_flush(filter, 1);
	php_stream_flush(ufp);
	php_stream_filter_remove(filter, 1);

	if (php_stream_tell(ufp) - loc != (zend_off_t) entry->uncompressed_filesize) {
		spprintf(error, 4096, "phar error: internal corruption of phar \"%s\" (actual filesize mismatch on file \"%s\")", phar->fname, entry->filename);
		return FAILURE;
	}

	entry->old_flags = entry->flags;

	/* this is now the new location of the file contents within this fp */
	phar_set_fp_type(entry, PHAR_UFP, loc);
	dummy.zero = entry->offset;
	dummy.fp = ufp;
	if (FAILURE == phar_postprocess_file(&dummy, entry->crc32, error, 0)) {
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

int phar_create_writeable_entry(phar_archive_data *phar, phar_entry_info *entry, char **error) /* {{{ */
{
	if (entry->fp_type == PHAR_MOD) {
		/* already newly created, truncate */
		php_stream_truncate_set_size(entry->fp, 0);

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

int phar_separate_entry_fp(phar_entry_info *entry, char **error) /* {{{ */
{
	php_stream *fp;
	phar_entry_info *link;

	if (FAILURE == phar_open_entry_fp(entry, error, 1)) {
		return FAILURE;
	}

	if (entry->fp_type == PHAR_MOD) {
		return SUCCESS;
	}

	fp = php_stream_fopen_tmpfile();
	if (fp == NULL) {
		spprintf(error, 0, "phar error: unable to create temporary file");
		return FAILURE;
	}
	phar_seek_efp(entry, 0, SEEK_SET, 0, 1);
	link = phar_get_link_source(entry);

	if (!link) {
		link = entry;
	}

	if (SUCCESS != php_stream_copy_to_stream_ex(phar_get_efp(link, 0), fp, link->uncompressed_filesize, NULL)) {
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
phar_entry_info * phar_open_jit(phar_archive_data *phar, phar_entry_info *entry, char **error) /* {{{ */
{
	if (error) {
		*error = NULL;
	}
	/* seek to start of internal file and read it */
	if (FAILURE == phar_open_entry_fp(entry, error, 1)) {
		return NULL;
	}
	if (-1 == phar_seek_efp(entry, 0, SEEK_SET, 0, 1)) {
		spprintf(error, 4096, "phar error: cannot seek to start of file \"%s\" in phar \"%s\"", entry->filename, phar->fname);
		return NULL;
	}
	return entry;
}
/* }}} */

PHP_PHAR_API int phar_resolve_alias(char *alias, size_t alias_len, char **filename, size_t *filename_len) /* {{{ */ {
	phar_archive_data *fd_ptr;
	if (HT_IS_INITIALIZED(&PHAR_G(phar_alias_map))
			&& NULL != (fd_ptr = zend_hash_str_find_ptr(&(PHAR_G(phar_alias_map)), alias, alias_len))) {
		*filename = fd_ptr->fname;
		*filename_len = fd_ptr->fname_len;
		return SUCCESS;
	}
	return FAILURE;
}
/* }}} */

int phar_free_alias(phar_archive_data *phar, char *alias, size_t alias_len) /* {{{ */
{
	if (phar->refcount || phar->is_persistent) {
		return FAILURE;
	}

	/* this archive has no open references, so emit a notice and remove it */
	if (zend_hash_str_del(&(PHAR_G(phar_fname_map)), phar->fname, phar->fname_len) != SUCCESS) {
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
int phar_get_archive(phar_archive_data **archive, char *fname, size_t fname_len, char *alias, size_t alias_len, char **error) /* {{{ */
{
	phar_archive_data *fd, *fd_ptr;
	char *my_realpath, *save;
	size_t save_len;

	phar_request_initialize();

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

			if (PHAR_G(last_phar)->alias_len && NULL != (fd_ptr = zend_hash_str_find_ptr(&(PHAR_G(phar_alias_map)), PHAR_G(last_phar)->alias, PHAR_G(last_phar)->alias_len))) {
				zend_hash_str_del(&(PHAR_G(phar_alias_map)), PHAR_G(last_phar)->alias, PHAR_G(last_phar)->alias_len);
			}

			zend_hash_str_add_ptr(&(PHAR_G(phar_alias_map)), alias, alias_len, *archive);
			PHAR_G(last_alias) = alias;
			PHAR_G(last_alias_len) = alias_len;
		}

		return SUCCESS;
	}

	if (alias && alias_len && PHAR_G(last_phar) && alias_len == PHAR_G(last_alias_len) && !memcmp(alias, PHAR_G(last_alias), alias_len)) {
		fd = PHAR_G(last_phar);
		fd_ptr = fd;
		goto alias_success;
	}

	if (alias && alias_len) {
		if (NULL != (fd_ptr = zend_hash_str_find_ptr(&(PHAR_G(phar_alias_map)), alias, alias_len))) {
alias_success:
			if (fname && (fname_len != fd_ptr->fname_len || strncmp(fname, fd_ptr->fname, fname_len))) {
				if (error) {
					spprintf(error, 0, "alias \"%s\" is already used for archive \"%s\" cannot be overloaded with \"%s\"", alias, fd_ptr->fname, fname);
				}
				if (SUCCESS == phar_free_alias(fd_ptr, alias, alias_len)) {
					if (error) {
						efree(*error);
						*error = NULL;
					}
				}
				return FAILURE;
			}

			*archive = fd_ptr;
			fd = fd_ptr;
			PHAR_G(last_phar) = fd;
			PHAR_G(last_phar_name) = fd->fname;
			PHAR_G(last_phar_name_len) = fd->fname_len;
			PHAR_G(last_alias) = alias;
			PHAR_G(last_alias_len) = alias_len;

			return SUCCESS;
		}

		if (PHAR_G(manifest_cached) && NULL != (fd_ptr = zend_hash_str_find_ptr(&cached_alias, alias, alias_len))) {
			goto alias_success;
		}
	}

	my_realpath = NULL;
	save = fname;
	save_len = fname_len;

	if (fname && fname_len) {
		if (NULL != (fd_ptr = zend_hash_str_find_ptr(&(PHAR_G(phar_fname_map)), fname, fname_len))) {
			*archive = fd_ptr;
			fd = fd_ptr;

			if (alias && alias_len) {
				if (!fd->is_temporary_alias && (alias_len != fd->alias_len || memcmp(fd->alias, alias, alias_len))) {
					if (error) {
						spprintf(error, 0, "alias \"%s\" is already used for archive \"%s\" cannot be overloaded with \"%s\"", alias, fd_ptr->fname, fname);
					}
					return FAILURE;
				}

				if (fd->alias_len && NULL != (fd_ptr = zend_hash_str_find_ptr(&(PHAR_G(phar_alias_map)), fd->alias, fd->alias_len))) {
					zend_hash_str_del(&(PHAR_G(phar_alias_map)), fd->alias, fd->alias_len);
				}

				zend_hash_str_add_ptr(&(PHAR_G(phar_alias_map)), alias, alias_len, fd);
			}

			PHAR_G(last_phar) = fd;
			PHAR_G(last_phar_name) = fd->fname;
			PHAR_G(last_phar_name_len) = fd->fname_len;
			PHAR_G(last_alias) = fd->alias;
			PHAR_G(last_alias_len) = fd->alias_len;

			return SUCCESS;
		}

		if (PHAR_G(manifest_cached) && NULL != (fd_ptr = zend_hash_str_find_ptr(&cached_phars, fname, fname_len))) {
			*archive = fd_ptr;
			fd = fd_ptr;

			/* this could be problematic - alias should never be different from manifest alias
			   for cached phars */
			if (!fd->is_temporary_alias && alias && alias_len) {
				if (alias_len != fd->alias_len || memcmp(fd->alias, alias, alias_len)) {
					if (error) {
						spprintf(error, 0, "alias \"%s\" is already used for archive \"%s\" cannot be overloaded with \"%s\"", alias, fd_ptr->fname, fname);
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

		if (NULL != (fd_ptr = zend_hash_str_find_ptr(&(PHAR_G(phar_alias_map)), save, save_len))) {
			fd = *archive = fd_ptr;

			PHAR_G(last_phar) = fd;
			PHAR_G(last_phar_name) = fd->fname;
			PHAR_G(last_phar_name_len) = fd->fname_len;
			PHAR_G(last_alias) = fd->alias;
			PHAR_G(last_alias_len) = fd->alias_len;

			return SUCCESS;
		}

		if (PHAR_G(manifest_cached) && NULL != (fd_ptr = zend_hash_str_find_ptr(&cached_alias, save, save_len))) {
			fd = *archive = fd_ptr;

			PHAR_G(last_phar) = fd;
			PHAR_G(last_phar_name) = fd->fname;
			PHAR_G(last_phar_name_len) = fd->fname_len;
			PHAR_G(last_alias) = fd->alias;
			PHAR_G(last_alias_len) = fd->alias_len;

			return SUCCESS;
		}

		/* not found, try converting \ to / */
		my_realpath = expand_filepath(fname, my_realpath);

		if (my_realpath) {
			fname_len = strlen(my_realpath);
			fname = my_realpath;
		} else {
			return FAILURE;
		}
#ifdef PHP_WIN32
		phar_unixify_path_separators(fname, fname_len);
#endif

		if (NULL != (fd_ptr = zend_hash_str_find_ptr(&(PHAR_G(phar_fname_map)), fname, fname_len))) {
realpath_success:
			*archive = fd_ptr;
			fd = fd_ptr;

			if (alias && alias_len) {
				zend_hash_str_add_ptr(&(PHAR_G(phar_alias_map)), alias, alias_len, fd);
			}

			efree(my_realpath);

			PHAR_G(last_phar) = fd;
			PHAR_G(last_phar_name) = fd->fname;
			PHAR_G(last_phar_name_len) = fd->fname_len;
			PHAR_G(last_alias) = fd->alias;
			PHAR_G(last_alias_len) = fd->alias_len;

			return SUCCESS;
		}

		if (PHAR_G(manifest_cached) && NULL != (fd_ptr = zend_hash_str_find_ptr(&cached_phars, fname, fname_len))) {
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
	uint32_t flags;

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
phar_entry_info *phar_get_entry_info(phar_archive_data *phar, char *path, size_t path_len, char **error, int security) /* {{{ */
{
	return phar_get_entry_info_dir(phar, path, path_len, 0, error, security);
}
/* }}} */
/**
 * retrieve information on a file or directory contained within a phar, or null if none found
 * allow_dir is 0 for none, 1 for both empty directories in the phar and temp directories, and 2 for only
 * valid pre-existing empty directory entries
 */
phar_entry_info *phar_get_entry_info_dir(phar_archive_data *phar, char *path, size_t path_len, char dir, char **error, int security) /* {{{ */
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

	if (!HT_IS_INITIALIZED(&phar->manifest)) {
		return NULL;
	}

	if (is_dir) {
		if (path_len <= 1) {
			return NULL;
		}
		path_len--;
	}

	if (NULL != (entry = zend_hash_str_find_ptr(&phar->manifest, path, path_len))) {
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
		if (zend_hash_str_exists(&phar->virtual_dirs, path, path_len)) {
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

	if (HT_IS_INITIALIZED(&phar->mounted_dirs) && zend_hash_num_elements(&phar->mounted_dirs)) {
		zend_string *str_key;

		ZEND_HASH_MAP_FOREACH_STR_KEY(&phar->mounted_dirs, str_key) {
			if (ZSTR_LEN(str_key) >= path_len || strncmp(ZSTR_VAL(str_key), path, ZSTR_LEN(str_key))) {
				continue;
			} else {
				char *test;
				size_t test_len;
				php_stream_statbuf ssb;

				if (NULL == (entry = zend_hash_find_ptr(&phar->manifest, str_key))) {
					if (error) {
						spprintf(error, 4096, "phar internal error: mounted path \"%s\" could not be retrieved from manifest", ZSTR_VAL(str_key));
					}
					return NULL;
				}

				if (!entry->tmp || !entry->is_mounted) {
					if (error) {
						spprintf(error, 4096, "phar internal error: mounted path \"%s\" is not properly initialized as a mounted path", ZSTR_VAL(str_key));
					}
					return NULL;
				}

				test_len = spprintf(&test, MAXPATHLEN, "%s%s", entry->tmp, path + ZSTR_LEN(str_key));

				if (SUCCESS != php_stream_stat_path(test, &ssb)) {
					efree(test);
					return NULL;
				}

				if ((ssb.sb.st_mode & S_IFDIR) && !dir) {
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
				if (SUCCESS != phar_mount_entry(phar, test, test_len, path, path_len)) {
					efree(test);
					if (error) {
						spprintf(error, 4096, "phar error: path \"%s\" exists as file \"%s\" and could not be mounted", path, test);
					}
					return NULL;
				}

				efree(test);

				if (NULL == (entry = zend_hash_str_find_ptr(&phar->manifest, path, path_len))) {
					if (error) {
						spprintf(error, 4096, "phar error: path \"%s\" exists as file \"%s\" and could not be retrieved after being mounted", path, test);
					}
					return NULL;
				}
				return entry;
			}
		} ZEND_HASH_FOREACH_END();
	}

	return NULL;
}
/* }}} */

static const char hexChars[] = "0123456789ABCDEF";

static int phar_hex_str(const char *digest, size_t digest_len, char **signature) /* {{{ */
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
static int phar_call_openssl_signverify(int is_sign, php_stream *fp, zend_off_t end, char *key, size_t key_len, char **signature, size_t *signature_len, php_uint32 sig_type) /* {{{ */
{
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zval retval, zp[4], openssl;
	zend_string *str;

	ZVAL_STRINGL(&openssl, is_sign ? "openssl_sign" : "openssl_verify", is_sign ? sizeof("openssl_sign")-1 : sizeof("openssl_verify")-1);
	if (*signature_len) {
		ZVAL_STRINGL(&zp[1], *signature, *signature_len);
	} else {
		ZVAL_EMPTY_STRING(&zp[1]);
	}
	ZVAL_STRINGL(&zp[2], key, key_len);
	php_stream_rewind(fp);
	str = php_stream_copy_to_mem(fp, (size_t) end, 0);
	if (str) {
		ZVAL_STR(&zp[0], str);
	} else {
		ZVAL_EMPTY_STRING(&zp[0]);
	}
	if (sig_type == PHAR_SIG_OPENSSL_SHA512) {
		ZVAL_LONG(&zp[3], 9); /* value from openssl.c #define OPENSSL_ALGO_SHA512 9 */
	} else if (sig_type == PHAR_SIG_OPENSSL_SHA256) {
		ZVAL_LONG(&zp[3], 7); /* value from openssl.c #define OPENSSL_ALGO_SHA256 7 */
	} else {
		/* don't rely on default value which may change in the future */
		ZVAL_LONG(&zp[3], 1); /* value from openssl.c #define OPENSSL_ALGO_SHA1   1 */
	}

	if ((size_t)end != Z_STRLEN(zp[0])) {
		zval_ptr_dtor_str(&zp[0]);
		zval_ptr_dtor_str(&zp[1]);
		zval_ptr_dtor_str(&zp[2]);
		zval_ptr_dtor_str(&openssl);
		return FAILURE;
	}

	if (FAILURE == zend_fcall_info_init(&openssl, 0, &fci, &fcc, NULL, NULL)) {
		zval_ptr_dtor_str(&zp[0]);
		zval_ptr_dtor_str(&zp[1]);
		zval_ptr_dtor_str(&zp[2]);
		zval_ptr_dtor_str(&openssl);
		return FAILURE;
	}

	fci.param_count = 4;
	fci.params = zp;
	Z_ADDREF(zp[0]);
	if (is_sign) {
		ZVAL_NEW_REF(&zp[1], &zp[1]);
	} else {
		Z_ADDREF(zp[1]);
	}
	Z_ADDREF(zp[2]);

	fci.retval = &retval;

	if (FAILURE == zend_call_function(&fci, &fcc)) {
		zval_ptr_dtor_str(&zp[0]);
		zval_ptr_dtor(&zp[1]);
		zval_ptr_dtor_str(&zp[2]);
		zval_ptr_dtor_str(&openssl);
		return FAILURE;
	}

	zval_ptr_dtor_str(&openssl);
	Z_DELREF(zp[0]);

	if (is_sign) {
		ZVAL_UNREF(&zp[1]);
	} else {
		Z_DELREF(zp[1]);
	}
	Z_DELREF(zp[2]);

	zval_ptr_dtor_str(&zp[0]);
	zval_ptr_dtor_str(&zp[2]);

	switch (Z_TYPE(retval)) {
		default:
		case IS_LONG:
			zval_ptr_dtor(&zp[1]);
			if (1 == Z_LVAL(retval)) {
				return SUCCESS;
			}
			return FAILURE;
		case IS_TRUE:
			*signature = estrndup(Z_STRVAL(zp[1]), Z_STRLEN(zp[1]));
			*signature_len = Z_STRLEN(zp[1]);
			zval_ptr_dtor(&zp[1]);
			return SUCCESS;
		case IS_FALSE:
			zval_ptr_dtor(&zp[1]);
			return FAILURE;
	}
}
/* }}} */
#endif /* #ifndef PHAR_HAVE_OPENSSL */

int phar_verify_signature(php_stream *fp, size_t end_of_phar, uint32_t sig_type, char *sig, size_t sig_len, char *fname, char **signature, size_t *signature_len, char **error) /* {{{ */
{
	size_t read_size, len;
	zend_off_t read_len;
	unsigned char buf[1024];

	php_stream_rewind(fp);

	switch (sig_type) {
		case PHAR_SIG_OPENSSL_SHA512:
		case PHAR_SIG_OPENSSL_SHA256:
		case PHAR_SIG_OPENSSL: {
#ifdef PHAR_HAVE_OPENSSL
			BIO *in;
			EVP_PKEY *key;
			const EVP_MD *mdtype;
			EVP_MD_CTX *md_ctx;

			if (sig_type == PHAR_SIG_OPENSSL_SHA512) {
				mdtype = EVP_sha512();
			} else if (sig_type == PHAR_SIG_OPENSSL_SHA256) {
				mdtype = EVP_sha256();
			} else {
				mdtype = EVP_sha1();
			}
#else
			size_t tempsig;
#endif
			zend_string *pubkey = NULL;
			char *pfile;
			php_stream *pfp;
#ifndef PHAR_HAVE_OPENSSL
			if (!zend_hash_str_exists(&module_registry, "openssl", sizeof("openssl")-1)) {
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

			if (!pfp || !(pubkey = php_stream_copy_to_mem(pfp, PHP_STREAM_COPY_ALL, 0)) || !ZSTR_LEN(pubkey)) {
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

			if (FAILURE == phar_call_openssl_signverify(0, fp, end_of_phar, ZSTR_VAL(pubkey), ZSTR_LEN(pubkey), &sig, &tempsig, sig_type)) {
				zend_string_release_ex(pubkey, 0);

				if (error) {
					spprintf(error, 0, "openssl signature could not be verified");
				}

				return FAILURE;
			}

			zend_string_release_ex(pubkey, 0);

			sig_len = tempsig;
#else
			in = BIO_new_mem_buf(ZSTR_VAL(pubkey), ZSTR_LEN(pubkey));

			if (NULL == in) {
				zend_string_release_ex(pubkey, 0);
				if (error) {
					spprintf(error, 0, "openssl signature could not be processed");
				}
				return FAILURE;
			}

			key = PEM_read_bio_PUBKEY(in, NULL, NULL, NULL);
			BIO_free(in);
			zend_string_release_ex(pubkey, 0);

			if (NULL == key) {
				if (error) {
					spprintf(error, 0, "openssl signature could not be processed");
				}
				return FAILURE;
			}

			md_ctx = EVP_MD_CTX_create();
			if (!md_ctx || !EVP_VerifyInit(md_ctx, mdtype)) {
				if (md_ctx) {
					EVP_MD_CTX_destroy(md_ctx);
				}
				if (error) {
					spprintf(error, 0, "openssl signature could not be verified");
				}
				return FAILURE;
			}
			read_len = end_of_phar;

			if ((size_t)read_len > sizeof(buf)) {
				read_size = sizeof(buf);
			} else {
				read_size = (size_t)read_len;
			}

			php_stream_seek(fp, 0, SEEK_SET);

			while (read_size && (len = php_stream_read(fp, (char*)buf, read_size)) > 0) {
				if (UNEXPECTED(EVP_VerifyUpdate (md_ctx, buf, len) == 0)) {
					goto failure;
				}
				read_len -= (zend_off_t)len;

				if (read_len < read_size) {
					read_size = (size_t)read_len;
				}
			}

			if (EVP_VerifyFinal(md_ctx, (unsigned char *)sig, sig_len, key) != 1) {
				failure:
				/* 1: signature verified, 0: signature does not match, -1: failed signature operation */
				EVP_PKEY_free(key);
				EVP_MD_CTX_destroy(md_ctx);

				if (error) {
					spprintf(error, 0, "broken openssl signature");
				}

				return FAILURE;
			}

			EVP_PKEY_free(key);
			EVP_MD_CTX_destroy(md_ctx);
#endif

			*signature_len = phar_hex_str((const char*)sig, sig_len, signature);
		}
		break;
		case PHAR_SIG_SHA512: {
			unsigned char digest[64];
			PHP_SHA512_CTX context;

			if (sig_len < sizeof(digest)) {
				if (error) {
					spprintf(error, 0, "broken signature");
				}
				return FAILURE;
			}

			PHP_SHA512Init(&context);
			read_len = end_of_phar;

			if ((size_t)read_len > sizeof(buf)) {
				read_size = sizeof(buf);
			} else {
				read_size = (size_t)read_len;
			}

			while ((len = php_stream_read(fp, (char*)buf, read_size)) > 0) {
				PHP_SHA512Update(&context, buf, len);
				read_len -= (zend_off_t)len;
				if ((size_t)read_len < read_size) {
					read_size = (size_t)read_len;
				}
			}

			PHP_SHA512Final(digest, &context);

			if (memcmp(digest, sig, sizeof(digest))) {
				if (error) {
					spprintf(error, 0, "broken signature");
				}
				return FAILURE;
			}

			*signature_len = phar_hex_str((const char*)digest, sizeof(digest), signature);
			break;
		}
		case PHAR_SIG_SHA256: {
			unsigned char digest[32];
			PHP_SHA256_CTX context;

			if (sig_len < sizeof(digest)) {
				if (error) {
					spprintf(error, 0, "broken signature");
				}
				return FAILURE;
			}

			PHP_SHA256Init(&context);
			read_len = end_of_phar;

			if ((size_t)read_len > sizeof(buf)) {
				read_size = sizeof(buf);
			} else {
				read_size = (size_t)read_len;
			}

			while ((len = php_stream_read(fp, (char*)buf, read_size)) > 0) {
				PHP_SHA256Update(&context, buf, len);
				read_len -= (zend_off_t)len;
				if ((size_t)read_len < read_size) {
					read_size = (size_t)read_len;
				}
			}

			PHP_SHA256Final(digest, &context);

			if (memcmp(digest, sig, sizeof(digest))) {
				if (error) {
					spprintf(error, 0, "broken signature");
				}
				return FAILURE;
			}

			*signature_len = phar_hex_str((const char*)digest, sizeof(digest), signature);
			break;
		}
		case PHAR_SIG_SHA1: {
			unsigned char digest[20];
			PHP_SHA1_CTX  context;

			if (sig_len < sizeof(digest)) {
				if (error) {
					spprintf(error, 0, "broken signature");
				}
				return FAILURE;
			}

			PHP_SHA1Init(&context);
			read_len = end_of_phar;

			if ((size_t)read_len > sizeof(buf)) {
				read_size = sizeof(buf);
			} else {
				read_size = (size_t)read_len;
			}

			while ((len = php_stream_read(fp, (char*)buf, read_size)) > 0) {
				PHP_SHA1Update(&context, buf, len);
				read_len -= (zend_off_t)len;
				if ((size_t)read_len < read_size) {
					read_size = (size_t)read_len;
				}
			}

			PHP_SHA1Final(digest, &context);

			if (memcmp(digest, sig, sizeof(digest))) {
				if (error) {
					spprintf(error, 0, "broken signature");
				}
				return FAILURE;
			}

			*signature_len = phar_hex_str((const char*)digest, sizeof(digest), signature);
			break;
		}
		case PHAR_SIG_MD5: {
			unsigned char digest[16];
			PHP_MD5_CTX   context;

			if (sig_len < sizeof(digest)) {
				if (error) {
					spprintf(error, 0, "broken signature");
				}
				return FAILURE;
			}

			PHP_MD5Init(&context);
			read_len = end_of_phar;

			if ((size_t)read_len > sizeof(buf)) {
				read_size = sizeof(buf);
			} else {
				read_size = (size_t)read_len;
			}

			while ((len = php_stream_read(fp, (char*)buf, read_size)) > 0) {
				PHP_MD5Update(&context, buf, len);
				read_len -= (zend_off_t)len;
				if ((size_t)read_len < read_size) {
					read_size = (size_t)read_len;
				}
			}

			PHP_MD5Final(digest, &context);

			if (memcmp(digest, sig, sizeof(digest))) {
				if (error) {
					spprintf(error, 0, "broken signature");
				}
				return FAILURE;
			}

			*signature_len = phar_hex_str((const char*)digest, sizeof(digest), signature);
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

int phar_create_signature(phar_archive_data *phar, php_stream *fp, char **signature, size_t *signature_length, char **error) /* {{{ */
{
	unsigned char buf[1024];
	size_t sig_len;

	php_stream_rewind(fp);

	if (phar->signature) {
		efree(phar->signature);
		phar->signature = NULL;
	}

	switch(phar->sig_flags) {
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
		default:
			phar->sig_flags = PHAR_SIG_SHA256;
			ZEND_FALLTHROUGH;
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
		case PHAR_SIG_OPENSSL_SHA512:
		case PHAR_SIG_OPENSSL_SHA256:
		case PHAR_SIG_OPENSSL: {
			unsigned char *sigbuf;
#ifdef PHAR_HAVE_OPENSSL
			unsigned int siglen;
			BIO *in;
			EVP_PKEY *key;
			EVP_MD_CTX *md_ctx;
			const EVP_MD *mdtype;

			if (phar->sig_flags == PHAR_SIG_OPENSSL_SHA512) {
				mdtype = EVP_sha512();
			} else if (phar->sig_flags == PHAR_SIG_OPENSSL_SHA256) {
				mdtype = EVP_sha256();
			} else {
				mdtype = EVP_sha1();
			}

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
			if (md_ctx == NULL) {
				EVP_PKEY_free(key);
				if (error) {
					spprintf(error, 0, "unable to initialize openssl signature for phar \"%s\"", phar->fname);
				}
				return FAILURE;
			}

			siglen = EVP_PKEY_size(key);
			sigbuf = emalloc(siglen + 1);

			if (!EVP_SignInit(md_ctx, mdtype)) {
				EVP_PKEY_free(key);
				efree(sigbuf);
				if (error) {
					spprintf(error, 0, "unable to initialize openssl signature for phar \"%s\"", phar->fname);
				}
				return FAILURE;
			}

			while ((sig_len = php_stream_read(fp, (char*)buf, sizeof(buf))) > 0) {
				if (!EVP_SignUpdate(md_ctx, buf, sig_len)) {
					EVP_PKEY_free(key);
					efree(sigbuf);
					if (error) {
						spprintf(error, 0, "unable to update the openssl signature for phar \"%s\"", phar->fname);
					}
					return FAILURE;
				}
			}

			if (!EVP_SignFinal (md_ctx, sigbuf, &siglen, key)) {
				EVP_PKEY_free(key);
				efree(sigbuf);
				if (error) {
					spprintf(error, 0, "unable to write phar \"%s\" with requested openssl signature", phar->fname);
				}
				return FAILURE;
			}

			sigbuf[siglen] = '\0';
			EVP_PKEY_free(key);
			EVP_MD_CTX_destroy(md_ctx);
#else
			size_t siglen;
			sigbuf = NULL;
			siglen = 0;
			php_stream_seek(fp, 0, SEEK_END);

			if (FAILURE == phar_call_openssl_signverify(1, fp, php_stream_tell(fp), PHAR_G(openssl_privatekey), PHAR_G(openssl_privatekey_len), (char **)&sigbuf, &siglen, phar->sig_flags)) {
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

	phar->sig_len = phar_hex_str((const char *)*signature, *signature_length, &phar->signature);
	return SUCCESS;
}
/* }}} */

void phar_add_virtual_dirs(phar_archive_data *phar, char *filename, size_t filename_len) /* {{{ */
{
	const char *s;
	zend_string *str;
	zval *ret;

	while ((s = zend_memrchr(filename, '/', filename_len))) {
		filename_len = s - filename;
		if (!filename_len) {
			break;
		}
		if (GC_FLAGS(&phar->virtual_dirs) & GC_PERSISTENT) {
			str = zend_string_init_interned(filename, filename_len, 1);
		} else {
			str = zend_string_init(filename, filename_len, 0);
		}
		ret = zend_hash_add_empty_element(&phar->virtual_dirs, str);
		zend_string_release(str);
		if (ret == NULL) {
			break;
		}
	}
}
/* }}} */

static int phar_update_cached_entry(zval *data, void *argument) /* {{{ */
{
	phar_entry_info *entry = (phar_entry_info *)Z_PTR_P(data);

	entry->phar = (phar_archive_data *)argument;

	if (entry->link) {
		entry->link = estrdup(entry->link);
	}

	if (entry->tmp) {
		entry->tmp = estrdup(entry->tmp);
	}

	entry->filename = estrndup(entry->filename, entry->filename_len);
	entry->is_persistent = 0;

	/* Replace metadata with non-persistent clones of the metadata. */
	phar_metadata_tracker_clone(&entry->metadata_tracker);
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

static void phar_manifest_copy_ctor(zval *zv) /* {{{ */
{
	phar_entry_info *info = emalloc(sizeof(phar_entry_info));
	memcpy(info, Z_PTR_P(zv), sizeof(phar_entry_info));
	Z_PTR_P(zv) = info;
}
/* }}} */

static void phar_copy_cached_phar(phar_archive_data **pphar) /* {{{ */
{
	phar_archive_data *phar;
	HashTable newmanifest;
	char *fname;
	phar_archive_object *objphar;

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

	phar_metadata_tracker_clone(&phar->metadata_tracker);

	zend_hash_init(&newmanifest, sizeof(phar_entry_info),
		zend_get_hash_value, destroy_phar_manifest_entry, 0);
	zend_hash_copy(&newmanifest, &(*pphar)->manifest, phar_manifest_copy_ctor);
	zend_hash_apply_with_argument(&newmanifest, phar_update_cached_entry, (void *)phar);
	phar->manifest = newmanifest;
	zend_hash_init(&phar->mounted_dirs, sizeof(char *),
		zend_get_hash_value, NULL, 0);
	zend_hash_init(&phar->virtual_dirs, sizeof(char *),
		zend_get_hash_value, NULL, 0);
	zend_hash_copy(&phar->virtual_dirs, &(*pphar)->virtual_dirs, NULL);
	*pphar = phar;

	/* now, scan the list of persistent Phar objects referencing this phar and update the pointers */
	ZEND_HASH_MAP_FOREACH_PTR(&PHAR_G(phar_persist_map), objphar) {
		if (objphar->archive->fname_len == phar->fname_len && !memcmp(objphar->archive->fname, phar->fname, phar->fname_len)) {
			objphar->archive = phar;
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

int phar_copy_on_write(phar_archive_data **pphar) /* {{{ */
{
	zval zv, *pzv;
	phar_archive_data *newpphar;

	ZVAL_PTR(&zv, *pphar);
	if (NULL == (pzv = zend_hash_str_add(&(PHAR_G(phar_fname_map)), (*pphar)->fname, (*pphar)->fname_len, &zv))) {
		return FAILURE;
	}

	phar_copy_cached_phar((phar_archive_data **)&Z_PTR_P(pzv));
	newpphar = Z_PTR_P(pzv);
	/* invalidate phar cache */
	PHAR_G(last_phar) = NULL;
	PHAR_G(last_phar_name) = PHAR_G(last_alias) = NULL;

	if (newpphar->alias_len && NULL == zend_hash_str_add_ptr(&(PHAR_G(phar_alias_map)), newpphar->alias, newpphar->alias_len, newpphar)) {
		zend_hash_str_del(&(PHAR_G(phar_fname_map)), (*pphar)->fname, (*pphar)->fname_len);
		return FAILURE;
	}

	*pphar = newpphar;
	return SUCCESS;
}
/* }}} */
