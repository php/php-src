/*
  +----------------------------------------------------------------------+
  | phar:// stream wrapper support                                       |
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

#define PHAR_STREAM 1
#include "phar_internal.h"
#include "stream.h"
#include "dirstream.h"

const php_stream_ops phar_ops = {
	phar_stream_write, /* write */
	phar_stream_read,  /* read */
	phar_stream_close, /* close */
	phar_stream_flush, /* flush */
	"phar stream",
	phar_stream_seek,  /* seek */
	NULL,              /* cast */
	phar_stream_stat,  /* stat */
	NULL, /* set option */
};

const php_stream_wrapper_ops phar_stream_wops = {
	phar_wrapper_open_url,
	NULL,                  /* phar_wrapper_close */
	NULL,                  /* phar_wrapper_stat, */
	phar_wrapper_stat,     /* stat_url */
	phar_wrapper_open_dir, /* opendir */
	"phar",
	phar_wrapper_unlink,   /* unlink */
	phar_wrapper_rename,   /* rename */
	phar_wrapper_mkdir,    /* create directory */
	phar_wrapper_rmdir,    /* remove directory */
	NULL
};

const php_stream_wrapper php_stream_phar_wrapper = {
	&phar_stream_wops,
	NULL,
	0 /* is_url */
};

/**
 * Open a phar file for streams API
 */
php_url* phar_parse_url(php_stream_wrapper *wrapper, const char *filename, const char *mode, int options) /* {{{ */
{
	php_url *resource;
	char *arch = NULL, *entry = NULL, *error;
	size_t arch_len, entry_len;

	if (strlen(filename) < 7 || strncasecmp(filename, "phar://", 7)) {
		return NULL;
	}
	if (mode[0] == 'a') {
		if (!(options & PHP_STREAM_URL_STAT_QUIET)) {
			php_stream_wrapper_log_error(wrapper, options, "phar error: open mode append not supported");
		}
		return NULL;
	}
	if (phar_split_fname(filename, strlen(filename), &arch, &arch_len, &entry, &entry_len, 2, (mode[0] == 'w' ? 2 : 0)) == FAILURE) {
		if (!(options & PHP_STREAM_URL_STAT_QUIET)) {
			if (arch && !entry) {
				php_stream_wrapper_log_error(wrapper, options, "phar error: no directory in \"%s\", must have at least phar://%s/ for root directory (always use full path to a new phar)", filename, arch);
				arch = NULL;
			} else {
				php_stream_wrapper_log_error(wrapper, options, "phar error: invalid url or non-existent phar \"%s\"", filename);
			}
		}
		return NULL;
	}
	resource = ecalloc(1, sizeof(php_url));
	resource->scheme = ZSTR_INIT_LITERAL("phar", 0);
	resource->host = zend_string_init(arch, arch_len, 0);
	efree(arch);
	resource->path = zend_string_init(entry, entry_len, 0);
	efree(entry);

#ifdef MBO_0
		if (resource) {
			fprintf(stderr, "Alias:     %s\n", alias);
			fprintf(stderr, "Scheme:    %s\n", ZSTR_VAL(resource->scheme));
/*			fprintf(stderr, "User:      %s\n", resource->user);*/
/*			fprintf(stderr, "Pass:      %s\n", resource->pass ? "***" : NULL);*/
			fprintf(stderr, "Host:      %s\n", ZSTR_VAL(resource->host));
/*			fprintf(stderr, "Port:      %d\n", resource->port);*/
			fprintf(stderr, "Path:      %s\n", ZSTR_VAL(resource->path));
/*			fprintf(stderr, "Query:     %s\n", resource->query);*/
/*			fprintf(stderr, "Fragment:  %s\n", resource->fragment);*/
		}
#endif
	if (mode[0] == 'w' || (mode[0] == 'r' && mode[1] == '+')) {
		phar_archive_data *pphar = NULL, *phar;

		if (PHAR_G(request_init) && HT_IS_INITIALIZED(&PHAR_G(phar_fname_map)) && NULL == (pphar = zend_hash_find_ptr(&(PHAR_G(phar_fname_map)), resource->host))) {
			pphar = NULL;
		}
		if (PHAR_G(readonly) && (!pphar || !pphar->is_data)) {
			if (!(options & PHP_STREAM_URL_STAT_QUIET)) {
				php_stream_wrapper_log_error(wrapper, options, "phar error: write operations disabled by the php.ini setting phar.readonly");
			}
			php_url_free(resource);
			return NULL;
		}
		if (phar_open_or_create_filename(ZSTR_VAL(resource->host), ZSTR_LEN(resource->host), NULL, 0, 0, options, &phar, &error) == FAILURE)
		{
			if (error) {
				if (!(options & PHP_STREAM_URL_STAT_QUIET)) {
					php_stream_wrapper_log_error(wrapper, options, "%s", error);
				}
				efree(error);
			}
			php_url_free(resource);
			return NULL;
		}
		if (phar->is_persistent && FAILURE == phar_copy_on_write(&phar)) {
			if (error) {
				spprintf(&error, 0, "Cannot open cached phar '%s' as writeable, copy on write failed", ZSTR_VAL(resource->host));
				if (!(options & PHP_STREAM_URL_STAT_QUIET)) {
					php_stream_wrapper_log_error(wrapper, options, "%s", error);
				}
				efree(error);
			}
			php_url_free(resource);
			return NULL;
		}
	} else {
		if (phar_open_from_filename(ZSTR_VAL(resource->host), ZSTR_LEN(resource->host), NULL, 0, options, NULL, &error) == FAILURE)
		{
			if (error) {
				if (!(options & PHP_STREAM_URL_STAT_QUIET)) {
					php_stream_wrapper_log_error(wrapper, options, "%s", error);
				}
				efree(error);
			}
			php_url_free(resource);
			return NULL;
		}
	}
	return resource;
}
/* }}} */

/**
 * used for fopen('phar://...') and company
 */
static php_stream * phar_wrapper_open_url(php_stream_wrapper *wrapper, const char *path, const char *mode, int options, zend_string **opened_path, php_stream_context *context STREAMS_DC) /* {{{ */
{
	phar_archive_data *phar;
	phar_entry_data *idata;
	char *internal_file;
	char *error;
	HashTable *pharcontext;
	php_url *resource = NULL;
	php_stream *fpf;
	zval *pzoption, *metadata;
	uint32_t host_len;

	if ((resource = phar_parse_url(wrapper, path, mode, options)) == NULL) {
		return NULL;
	}

	/* we must have at the very least phar://alias.phar/internalfile.php */
	if (!resource->scheme || !resource->host || !resource->path) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options, "phar error: invalid url \"%s\"", path);
		return NULL;
	}

	if (!zend_string_equals_literal_ci(resource->scheme, "phar")) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options, "phar error: not a phar stream url \"%s\"", path);
		return NULL;
	}

	host_len = ZSTR_LEN(resource->host);
	phar_request_initialize();

	/* strip leading "/" */
	internal_file = estrndup(ZSTR_VAL(resource->path) + 1, ZSTR_LEN(resource->path) - 1);
	if (mode[0] == 'w' || (mode[0] == 'r' && mode[1] == '+')) {
		if (NULL == (idata = phar_get_or_create_entry_data(ZSTR_VAL(resource->host), host_len, internal_file, strlen(internal_file), mode, 0, &error, 1))) {
			if (error) {
				php_stream_wrapper_log_error(wrapper, options, "%s", error);
				efree(error);
			} else {
				php_stream_wrapper_log_error(wrapper, options, "phar error: file \"%s\" could not be created in phar \"%s\"", internal_file, ZSTR_VAL(resource->host));
			}
			efree(internal_file);
			php_url_free(resource);
			return NULL;
		}
		if (error) {
			efree(error);
		}
		fpf = php_stream_alloc(&phar_ops, idata, NULL, mode);
		php_url_free(resource);
		efree(internal_file);

		if (context && Z_TYPE(context->options) != IS_UNDEF && (pzoption = zend_hash_str_find(HASH_OF(&context->options), "phar", sizeof("phar")-1)) != NULL) {
			pharcontext = HASH_OF(pzoption);
			if (idata->internal_file->uncompressed_filesize == 0
				&& idata->internal_file->compressed_filesize == 0
				&& (pzoption = zend_hash_str_find(pharcontext, "compress", sizeof("compress")-1)) != NULL
				&& Z_TYPE_P(pzoption) == IS_LONG
				&& (Z_LVAL_P(pzoption) & ~PHAR_ENT_COMPRESSION_MASK) == 0
			) {
				idata->internal_file->flags &= ~PHAR_ENT_COMPRESSION_MASK;
				idata->internal_file->flags |= Z_LVAL_P(pzoption);
			}
			if ((pzoption = zend_hash_str_find(pharcontext, "metadata", sizeof("metadata")-1)) != NULL) {
				phar_metadata_tracker_free(&idata->internal_file->metadata_tracker, idata->internal_file->is_persistent);

				metadata = pzoption;
				ZVAL_COPY_DEREF(&idata->internal_file->metadata_tracker.val, metadata);
				idata->phar->is_modified = 1;
			}
		}
		if (opened_path) {
			*opened_path = strpprintf(MAXPATHLEN, "phar://%s/%s", idata->phar->fname, idata->internal_file->filename);
		}
		return fpf;
	} else {
		if (!*internal_file && (options & STREAM_OPEN_FOR_INCLUDE)) {
			/* retrieve the stub */
			if (FAILURE == phar_get_archive(&phar, ZSTR_VAL(resource->host), host_len, NULL, 0, NULL)) {
				php_stream_wrapper_log_error(wrapper, options, "file %s is not a valid phar archive", ZSTR_VAL(resource->host));
				efree(internal_file);
				php_url_free(resource);
				return NULL;
			}
			if (phar->is_tar || phar->is_zip) {
				if ((FAILURE == phar_get_entry_data(&idata, ZSTR_VAL(resource->host), host_len, ".phar/stub.php", sizeof(".phar/stub.php")-1, "r", 0, &error, 0)) || !idata) {
					goto idata_error;
				}
				efree(internal_file);
				if (opened_path) {
					*opened_path = strpprintf(MAXPATHLEN, "%s", phar->fname);
				}
				php_url_free(resource);
				goto phar_stub;
			} else {
				php_stream *stream = phar_get_pharfp(phar);
				if (stream == NULL) {
					if (UNEXPECTED(FAILURE == phar_open_archive_fp(phar))) {
						php_stream_wrapper_log_error(wrapper, options, "phar error: could not reopen phar \"%s\"", ZSTR_VAL(resource->host));
						efree(internal_file);
						php_url_free(resource);
						return NULL;
					}
					stream = phar_get_pharfp(phar);
				}

				phar_entry_info *entry;

				entry = (phar_entry_info *) ecalloc(1, sizeof(phar_entry_info));
				entry->is_temp_dir = 1;
				entry->filename = estrndup("", 0);
				entry->filename_len = 0;
				entry->phar = phar;
				entry->offset = entry->offset_abs = 0;
				entry->compressed_filesize = entry->uncompressed_filesize = phar->halt_offset;
				entry->is_crc_checked = 1;

				idata = (phar_entry_data *) ecalloc(1, sizeof(phar_entry_data));
				idata->fp = stream;
				idata->phar = phar;
				idata->internal_file = entry;
				if (!phar->is_persistent) {
					++(entry->phar->refcount);
				}
				++(entry->fp_refcount);
				php_url_free(resource);
				if (opened_path) {
					*opened_path = strpprintf(MAXPATHLEN, "%s", phar->fname);
				}
				efree(internal_file);
				goto phar_stub;
			}
		}
		/* read-only access is allowed to magic files in .phar directory */
		if ((FAILURE == phar_get_entry_data(&idata, ZSTR_VAL(resource->host), host_len, internal_file, strlen(internal_file), "r", 0, &error, 0)) || !idata) {
idata_error:
			if (error) {
				php_stream_wrapper_log_error(wrapper, options, "%s", error);
				efree(error);
			} else {
				php_stream_wrapper_log_error(wrapper, options, "phar error: \"%s\" is not a file in phar \"%s\"", internal_file, ZSTR_VAL(resource->host));
			}
			efree(internal_file);
			php_url_free(resource);
			return NULL;
		}
	}
	php_url_free(resource);
#ifdef MBO_0
		fprintf(stderr, "Pharname:   %s\n", idata->phar->filename);
		fprintf(stderr, "Filename:   %s\n", internal_file);
		fprintf(stderr, "Entry:      %s\n", idata->internal_file->filename);
		fprintf(stderr, "Size:       %u\n", idata->internal_file->uncompressed_filesize);
		fprintf(stderr, "Compressed: %u\n", idata->internal_file->flags);
		fprintf(stderr, "Offset:     %u\n", idata->internal_file->offset_within_phar);
		fprintf(stderr, "Cached:     %s\n", idata->internal_file->filedata ? "yes" : "no");
#endif

	/* check length, crc32 */
	if (!idata->internal_file->is_crc_checked && phar_postprocess_file(idata, idata->internal_file->crc32, &error, 2) != SUCCESS) {
		php_stream_wrapper_log_error(wrapper, options, "%s", error);
		efree(error);
		phar_entry_delref(idata);
		efree(internal_file);
		return NULL;
	}

	if (!PHAR_G(cwd_init) && (options & STREAM_OPEN_FOR_INCLUDE)) {
		char *entry = idata->internal_file->filename, *cwd;

		PHAR_G(cwd_init) = 1;
		if ((idata->phar->is_tar || idata->phar->is_zip) && idata->internal_file->filename_len == sizeof(".phar/stub.php")-1 && !strncmp(idata->internal_file->filename, ".phar/stub.php", sizeof(".phar/stub.php")-1)) {
			/* we're executing the stub, which doesn't count as a file */
			PHAR_G(cwd_init) = 0;
		} else if ((cwd = strrchr(entry, '/'))) {
			PHAR_G(cwd_len) = cwd - entry;
			PHAR_G(cwd) = estrndup(entry, PHAR_G(cwd_len));
		} else {
			/* root directory */
			PHAR_G(cwd_len) = 0;
			PHAR_G(cwd) = NULL;
		}
	}
	if (opened_path) {
		*opened_path = strpprintf(MAXPATHLEN, "phar://%s/%s", idata->phar->fname, idata->internal_file->filename);
	}
	efree(internal_file);
phar_stub:
	fpf = php_stream_alloc(&phar_ops, idata, NULL, mode);
	return fpf;
}
/* }}} */

/**
 * Used for fclose($fp) where $fp is a phar archive
 */
static int phar_stream_close(php_stream *stream, int close_handle) /* {{{ */
{
	/* for some reasons phar needs to be flushed even if there is no write going on */
	phar_stream_flush(stream);

	phar_entry_delref((phar_entry_data *)stream->abstract);

	return 0;
}
/* }}} */

/**
 * used for fread($fp) and company on a fopen()ed phar file handle
 */
static ssize_t phar_stream_read(php_stream *stream, char *buf, size_t count) /* {{{ */
{
	phar_entry_data *data = (phar_entry_data *)stream->abstract;
	size_t got;
	phar_entry_info *entry;

	if (data->internal_file->link) {
		entry = phar_get_link_source(data->internal_file);
	} else {
		entry = data->internal_file;
	}

	if (entry->is_deleted) {
		stream->eof = 1;
		return -1;
	}

	/* use our proxy position */
	php_stream_seek(data->fp, data->position + data->zero, SEEK_SET);

	got = php_stream_read(data->fp, buf, MIN(count, (size_t)(entry->uncompressed_filesize - data->position)));
	data->position = php_stream_tell(data->fp) - data->zero;
	stream->eof = (data->position == (zend_off_t) entry->uncompressed_filesize);

	return got;
}
/* }}} */

/**
 * Used for fseek($fp) on a phar file handle
 */
static int phar_stream_seek(php_stream *stream, zend_off_t offset, int whence, zend_off_t *newoffset) /* {{{ */
{
	phar_entry_data *data = (phar_entry_data *)stream->abstract;
	phar_entry_info *entry;
	int res;
	zend_off_t temp;

	if (data->internal_file->link) {
		entry = phar_get_link_source(data->internal_file);
	} else {
		entry = data->internal_file;
	}

	switch (whence) {
		case SEEK_END :
			temp = data->zero + entry->uncompressed_filesize + offset;
			break;
		case SEEK_CUR :
			temp = data->zero + data->position + offset;
			break;
		case SEEK_SET :
			temp = data->zero + offset;
			break;
		default:
			temp = 0;
	}
	if (temp > data->zero + (zend_off_t) entry->uncompressed_filesize) {
		*newoffset = -1;
		return -1;
	}
	if (temp < data->zero) {
		*newoffset = -1;
		return -1;
	}
	res = php_stream_seek(data->fp, temp, SEEK_SET);
	*newoffset = php_stream_tell(data->fp) - data->zero;
	data->position = *newoffset;
	return res;
}
/* }}} */

/**
 * Used for writing to a phar file
 */
static ssize_t phar_stream_write(php_stream *stream, const char *buf, size_t count) /* {{{ */
{
	phar_entry_data *data = (phar_entry_data *) stream->abstract;

	php_stream_seek(data->fp, data->position, SEEK_SET);
	if (count != php_stream_write(data->fp, buf, count)) {
		php_stream_wrapper_log_error(stream->wrapper, stream->flags, "phar error: Could not write %d characters to \"%s\" in phar \"%s\"", (int) count, data->internal_file->filename, data->phar->fname);
		return -1;
	}
	data->position = php_stream_tell(data->fp);
	if (data->position > (zend_off_t)data->internal_file->uncompressed_filesize) {
		data->internal_file->uncompressed_filesize = data->position;
	}
	data->internal_file->compressed_filesize = data->internal_file->uncompressed_filesize;
	data->internal_file->old_flags = data->internal_file->flags;
	data->internal_file->is_modified = 1;
	return count;
}
/* }}} */

/**
 * Used to save work done on a writeable phar
 */
static int phar_stream_flush(php_stream *stream) /* {{{ */
{
	char *error;
	int ret;
	phar_entry_data *data = (phar_entry_data *) stream->abstract;

	if (data->internal_file->is_modified) {
		data->internal_file->timestamp = time(0);
		ret = phar_flush(data->phar, 0, 0, 0, &error);
		if (error) {
			php_stream_wrapper_log_error(stream->wrapper, REPORT_ERRORS, "%s", error);
			efree(error);
		}
		return ret;
	} else {
		return EOF;
	}
}
/* }}} */

 /* {{{ phar_dostat */
/**
 * stat an opened phar file handle stream, used by phar_stat()
 */
void phar_dostat(phar_archive_data *phar, phar_entry_info *data, php_stream_statbuf *ssb, bool is_temp_dir)
{
	memset(ssb, 0, sizeof(php_stream_statbuf));

	if (!is_temp_dir && !data->is_dir) {
		ssb->sb.st_size = data->uncompressed_filesize;
		ssb->sb.st_mode = data->flags & PHAR_ENT_PERM_MASK;
		ssb->sb.st_mode |= S_IFREG; /* regular file */
		/* timestamp is just the timestamp when this was added to the phar */
		ssb->sb.st_mtime = data->timestamp;
		ssb->sb.st_atime = data->timestamp;
		ssb->sb.st_ctime = data->timestamp;
	} else if (!is_temp_dir && data->is_dir) {
		ssb->sb.st_size = 0;
		ssb->sb.st_mode = data->flags & PHAR_ENT_PERM_MASK;
		ssb->sb.st_mode |= S_IFDIR; /* regular directory */
		/* timestamp is just the timestamp when this was added to the phar */
		ssb->sb.st_mtime = data->timestamp;
		ssb->sb.st_atime = data->timestamp;
		ssb->sb.st_ctime = data->timestamp;
	} else {
		ssb->sb.st_size = 0;
		ssb->sb.st_mode = 0777;
		ssb->sb.st_mode |= S_IFDIR; /* regular directory */
		ssb->sb.st_mtime = phar->max_timestamp;
		ssb->sb.st_atime = phar->max_timestamp;
		ssb->sb.st_ctime = phar->max_timestamp;
	}
	if (!phar->is_writeable) {
		ssb->sb.st_mode = (ssb->sb.st_mode & 0555) | (ssb->sb.st_mode & ~0777);
	}

	ssb->sb.st_nlink = 1;
	ssb->sb.st_rdev = -1;
	/* this is only for APC, so use /dev/null device - no chance of conflict there! */
	ssb->sb.st_dev = 0xc;
	/* generate unique inode number for alias/filename, so no phars will conflict */
	if (!is_temp_dir) {
		ssb->sb.st_ino = data->inode;
	}
#ifndef PHP_WIN32
	ssb->sb.st_blksize = -1;
	ssb->sb.st_blocks = -1;
#endif
}
/* }}}*/

/**
 * Stat an opened phar file handle
 */
static int phar_stream_stat(php_stream *stream, php_stream_statbuf *ssb) /* {{{ */
{
	phar_entry_data *data = (phar_entry_data *)stream->abstract;

	/* If ssb is NULL then someone is misbehaving */
	if (!ssb) {
		return -1;
	}

	phar_dostat(data->phar, data->internal_file, ssb, 0);
	return 0;
}
/* }}} */

/**
 * Stream wrapper stat implementation of stat()
 */
static int phar_wrapper_stat(php_stream_wrapper *wrapper, const char *url, int flags,
				  php_stream_statbuf *ssb, php_stream_context *context) /* {{{ */
{
	php_url *resource = NULL;
	char *internal_file, *error;
	phar_archive_data *phar;
	phar_entry_info *entry;
	uint32_t host_len;
	size_t internal_file_len;

	if ((resource = phar_parse_url(wrapper, url, "r", flags|PHP_STREAM_URL_STAT_QUIET)) == NULL) {
		return FAILURE;
	}

	/* we must have at the very least phar://alias.phar/internalfile.php */
	if (!resource->scheme || !resource->host || !resource->path) {
		php_url_free(resource);
		return FAILURE;
	}

	if (!zend_string_equals_literal_ci(resource->scheme, "phar")) {
		php_url_free(resource);
		return FAILURE;
	}

	host_len = ZSTR_LEN(resource->host);
	phar_request_initialize();

	internal_file = ZSTR_VAL(resource->path) + 1; /* strip leading "/" */
	/* find the phar in our trusty global hash indexed by alias (host of phar://blah.phar/file.whatever) */
	if (FAILURE == phar_get_archive(&phar, ZSTR_VAL(resource->host), host_len, NULL, 0, &error)) {
		php_url_free(resource);
		if (error) {
			efree(error);
		}
		return FAILURE;
	}
	if (error) {
		efree(error);
	}
	if (*internal_file == '\0') {
		/* root directory requested */
		phar_dostat(phar, NULL, ssb, 1);
		php_url_free(resource);
		return SUCCESS;
	}
	if (!HT_IS_INITIALIZED(&phar->manifest)) {
		php_url_free(resource);
		return FAILURE;
	}
	internal_file_len = strlen(internal_file);
	/* search through the manifest of files, and if we have an exact match, it's a file */
	if (NULL != (entry = zend_hash_str_find_ptr(&phar->manifest, internal_file, internal_file_len))) {
		phar_dostat(phar, entry, ssb, 0);
		php_url_free(resource);
		return SUCCESS;
	}
	if (zend_hash_str_exists(&(phar->virtual_dirs), internal_file, internal_file_len)) {
		phar_dostat(phar, NULL, ssb, 1);
		php_url_free(resource);
		return SUCCESS;
	}
	/* check for mounted directories */
	if (HT_IS_INITIALIZED(&phar->mounted_dirs) && zend_hash_num_elements(&phar->mounted_dirs)) {
		zend_string *str_key;

		ZEND_HASH_MAP_FOREACH_STR_KEY(&phar->mounted_dirs, str_key) {
			if (ZSTR_LEN(str_key) >= internal_file_len || strncmp(ZSTR_VAL(str_key), internal_file, ZSTR_LEN(str_key))) {
				continue;
			} else {
				char *test;
				size_t test_len;
				php_stream_statbuf ssbi;

				if (NULL == (entry = zend_hash_find_ptr(&phar->manifest, str_key))) {
					goto free_resource;
				}
				if (!entry->tmp || !entry->is_mounted) {
					goto free_resource;
				}
				test_len = spprintf(&test, MAXPATHLEN, "%s%s", entry->tmp, internal_file + ZSTR_LEN(str_key));
				if (SUCCESS != php_stream_stat_path(test, &ssbi)) {
					efree(test);
					continue;
				}
				/* mount the file/directory just in time */
				if (SUCCESS != phar_mount_entry(phar, test, test_len, internal_file, internal_file_len)) {
					efree(test);
					goto free_resource;
				}
				efree(test);
				if (NULL == (entry = zend_hash_str_find_ptr(&phar->manifest, internal_file, internal_file_len))) {
					goto free_resource;
				}
				phar_dostat(phar, entry, ssb, 0);
				php_url_free(resource);
				return SUCCESS;
			}
		} ZEND_HASH_FOREACH_END();
	}
free_resource:
	php_url_free(resource);
	return FAILURE;
}
/* }}} */

/**
 * Unlink a file within a phar archive
 */
static int phar_wrapper_unlink(php_stream_wrapper *wrapper, const char *url, int options, php_stream_context *context) /* {{{ */
{
	php_url *resource;
	char *internal_file, *error;
	int internal_file_len;
	phar_entry_data *idata;
	phar_archive_data *pphar;
	uint32_t host_len;

	if ((resource = phar_parse_url(wrapper, url, "rb", options)) == NULL) {
		php_stream_wrapper_log_error(wrapper, options, "phar error: unlink failed");
		return 0;
	}

	/* we must have at the very least phar://alias.phar/internalfile.php */
	if (!resource->scheme || !resource->host || !resource->path) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options, "phar error: invalid url \"%s\"", url);
		return 0;
	}

	if (!zend_string_equals_literal_ci(resource->scheme, "phar")) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options, "phar error: not a phar stream url \"%s\"", url);
		return 0;
	}

	host_len = ZSTR_LEN(resource->host);
	phar_request_initialize();

	pphar = zend_hash_find_ptr(&(PHAR_G(phar_fname_map)), resource->host);
	if (PHAR_G(readonly) && (!pphar || !pphar->is_data)) {
		php_url_free(resource);
		php_stream_wrapper_log_error(wrapper, options, "phar error: write operations disabled by the php.ini setting phar.readonly");
		return 0;
	}

	/* need to copy to strip leading "/", will get touched again */
	internal_file = estrndup(ZSTR_VAL(resource->path) + 1, ZSTR_LEN(resource->path) - 1);
	internal_file_len = ZSTR_LEN(resource->path) - 1;
	if (FAILURE == phar_get_entry_data(&idata, ZSTR_VAL(resource->host), host_len, internal_file, internal_file_len, "r", 0, &error, 1)) {
		/* constraints of fp refcount were not met */
		if (error) {
			php_stream_wrapper_log_error(wrapper, options, "unlink of \"%s\" failed: %s", url, error);
			efree(error);
		} else {
			php_stream_wrapper_log_error(wrapper, options, "unlink of \"%s\" failed, file does not exist", url);
		}
		efree(internal_file);
		php_url_free(resource);
		return 0;
	}
	if (error) {
		efree(error);
	}
	if (idata->internal_file->fp_refcount > 1) {
		/* more than just our fp resource is open for this file */
		php_stream_wrapper_log_error(wrapper, options, "phar error: \"%s\" in phar \"%s\", has open file pointers, cannot unlink", internal_file, ZSTR_VAL(resource->host));
		efree(internal_file);
		php_url_free(resource);
		phar_entry_delref(idata);
		return 0;
	}
	php_url_free(resource);
	efree(internal_file);
	phar_entry_remove(idata, &error);
	if (error) {
		php_stream_wrapper_log_error(wrapper, options, "%s", error);
		efree(error);
	}
	return 1;
}
/* }}} */

static int phar_wrapper_rename(php_stream_wrapper *wrapper, const char *url_from, const char *url_to, int options, php_stream_context *context) /* {{{ */
{
	php_url *resource_from, *resource_to;
	char *error;
	phar_archive_data *phar, *pfrom, *pto;
	phar_entry_info *entry;
	uint32_t host_len;
	int is_dir = 0;
	int is_modified = 0;

	error = NULL;

	if ((resource_from = phar_parse_url(wrapper, url_from, "wb", options|PHP_STREAM_URL_STAT_QUIET)) == NULL) {
		php_error_docref(NULL, E_WARNING, "phar error: cannot rename \"%s\" to \"%s\": invalid or non-writable url \"%s\"", url_from, url_to, url_from);
		return 0;
	}
	if (SUCCESS != phar_get_archive(&pfrom, ZSTR_VAL(resource_from->host), ZSTR_LEN(resource_from->host), NULL, 0, &error)) {
		pfrom = NULL;
		if (error) {
			efree(error);
		}
	}
	if (PHAR_G(readonly) && (!pfrom || !pfrom->is_data)) {
		php_url_free(resource_from);
		php_error_docref(NULL, E_WARNING, "phar error: Write operations disabled by the php.ini setting phar.readonly");
		return 0;
	}

	if ((resource_to = phar_parse_url(wrapper, url_to, "wb", options|PHP_STREAM_URL_STAT_QUIET)) == NULL) {
		php_url_free(resource_from);
		php_error_docref(NULL, E_WARNING, "phar error: cannot rename \"%s\" to \"%s\": invalid or non-writable url \"%s\"", url_from, url_to, url_to);
		return 0;
	}
	if (SUCCESS != phar_get_archive(&pto, ZSTR_VAL(resource_to->host), ZSTR_LEN(resource_to->host), NULL, 0, &error)) {
		if (error) {
			efree(error);
		}
		pto = NULL;
	}
	if (PHAR_G(readonly) && (!pto || !pto->is_data)) {
		php_url_free(resource_from);
		php_error_docref(NULL, E_WARNING, "phar error: Write operations disabled by the php.ini setting phar.readonly");
		return 0;
	}

	if (!zend_string_equals(resource_from->host, resource_to->host)) {
		php_url_free(resource_from);
		php_url_free(resource_to);
		php_error_docref(NULL, E_WARNING, "phar error: cannot rename \"%s\" to \"%s\", not within the same phar archive", url_from, url_to);
		return 0;
	}

	/* we must have at the very least phar://alias.phar/internalfile.php */
	if (!resource_from->scheme || !resource_from->host || !resource_from->path) {
		php_url_free(resource_from);
		php_url_free(resource_to);
		php_error_docref(NULL, E_WARNING, "phar error: cannot rename \"%s\" to \"%s\": invalid url \"%s\"", url_from, url_to, url_from);
		return 0;
	}

	if (!resource_to->scheme || !resource_to->host || !resource_to->path) {
		php_url_free(resource_from);
		php_url_free(resource_to);
		php_error_docref(NULL, E_WARNING, "phar error: cannot rename \"%s\" to \"%s\": invalid url \"%s\"", url_from, url_to, url_to);
		return 0;
	}

	if (!zend_string_equals_literal_ci(resource_from->scheme, "phar")) {
		php_url_free(resource_from);
		php_url_free(resource_to);
		php_error_docref(NULL, E_WARNING, "phar error: cannot rename \"%s\" to \"%s\": not a phar stream url \"%s\"", url_from, url_to, url_from);
		return 0;
	}

	if (!zend_string_equals_literal_ci(resource_to->scheme, "phar")) {
		php_url_free(resource_from);
		php_url_free(resource_to);
		php_error_docref(NULL, E_WARNING, "phar error: cannot rename \"%s\" to \"%s\": not a phar stream url \"%s\"", url_from, url_to, url_to);
		return 0;
	}

	host_len = ZSTR_LEN(resource_from->host);

	if (SUCCESS != phar_get_archive(&phar, ZSTR_VAL(resource_from->host), host_len, NULL, 0, &error)) {
		php_url_free(resource_from);
		php_url_free(resource_to);
		php_error_docref(NULL, E_WARNING, "phar error: cannot rename \"%s\" to \"%s\": %s", url_from, url_to, error);
		efree(error);
		return 0;
	}

	if (phar->is_persistent && FAILURE == phar_copy_on_write(&phar)) {
		php_url_free(resource_from);
		php_url_free(resource_to);
		php_error_docref(NULL, E_WARNING, "phar error: cannot rename \"%s\" to \"%s\": could not make cached phar writeable", url_from, url_to);
		return 0;
	}

	if (NULL != (entry = zend_hash_str_find_ptr(&(phar->manifest), ZSTR_VAL(resource_from->path)+1, ZSTR_LEN(resource_from->path)-1))) {
		phar_entry_info new, *source;

		/* perform rename magic */
		if (entry->is_deleted) {
			php_url_free(resource_from);
			php_url_free(resource_to);
			php_error_docref(NULL, E_WARNING, "phar error: cannot rename \"%s\" to \"%s\" from extracted phar archive, source has been deleted", url_from, url_to);
			return 0;
		}
		/* transfer all data over to the new entry */
		memcpy((void *) &new, (void *) entry, sizeof(phar_entry_info));
		/* mark the old one for deletion */
		entry->is_deleted = 1;
		entry->fp = NULL;
		ZVAL_UNDEF(&entry->metadata_tracker.val);
		entry->link = entry->tmp = NULL;
		source = entry;

		/* add to the manifest, and then store the pointer to the new guy in entry */
		entry = zend_hash_str_add_mem(&(phar->manifest), ZSTR_VAL(resource_to->path)+1, ZSTR_LEN(resource_to->path)-1, (void **)&new, sizeof(phar_entry_info));

		entry->filename = estrndup(ZSTR_VAL(resource_to->path)+1, ZSTR_LEN(resource_to->path)-1);
		if (FAILURE == phar_copy_entry_fp(source, entry, &error)) {
			php_url_free(resource_from);
			php_url_free(resource_to);
			php_error_docref(NULL, E_WARNING, "phar error: cannot rename \"%s\" to \"%s\": %s", url_from, url_to, error);
			efree(error);
			zend_hash_str_del(&(phar->manifest), entry->filename, strlen(entry->filename));
			return 0;
		}
		is_modified = 1;
		entry->is_modified = 1;
		entry->filename_len = strlen(entry->filename);
		is_dir = entry->is_dir;
	} else {
		is_dir = zend_hash_str_exists(&(phar->virtual_dirs), ZSTR_VAL(resource_from->path)+1, ZSTR_LEN(resource_from->path)-1);
		if (!is_dir) {
			/* file does not exist */
			php_url_free(resource_from);
			php_url_free(resource_to);
			php_error_docref(NULL, E_WARNING, "phar error: cannot rename \"%s\" to \"%s\" from extracted phar archive, source does not exist", url_from, url_to);
			return 0;

		}
	}

	/* Rename directory. Update all nested paths */
	if (is_dir) {
		Bucket *b;
		zend_string *str_key;
		zend_string *new_str_key;
		uint32_t from_len = ZSTR_LEN(resource_from->path) - 1;
		uint32_t to_len = ZSTR_LEN(resource_to->path) - 1;

		ZEND_HASH_MAP_FOREACH_BUCKET(&phar->manifest, b) {
			str_key = b->key;
			entry = Z_PTR(b->val);
			if (!entry->is_deleted &&
				ZSTR_LEN(str_key) > from_len &&
				memcmp(ZSTR_VAL(str_key), ZSTR_VAL(resource_from->path)+1, from_len) == 0 &&
				IS_SLASH(ZSTR_VAL(str_key)[from_len])) {

				new_str_key = zend_string_alloc(ZSTR_LEN(str_key) + to_len - from_len, 0);
				memcpy(ZSTR_VAL(new_str_key), ZSTR_VAL(resource_to->path) + 1, to_len);
				memcpy(ZSTR_VAL(new_str_key) + to_len, ZSTR_VAL(str_key) + from_len, ZSTR_LEN(str_key) - from_len);
				ZSTR_VAL(new_str_key)[ZSTR_LEN(new_str_key)] = 0;

				is_modified = 1;
				entry->is_modified = 1;
				efree(entry->filename);
				// TODO: avoid reallocation (make entry->filename zend_string*)
				entry->filename = estrndup(ZSTR_VAL(new_str_key), ZSTR_LEN(new_str_key));
				entry->filename_len = ZSTR_LEN(new_str_key);

				zend_string_release_ex(str_key, 0);
				b->h = zend_string_hash_val(new_str_key);
				b->key = new_str_key;
			}
		} ZEND_HASH_FOREACH_END();
		zend_hash_rehash(&phar->manifest);

		ZEND_HASH_MAP_FOREACH_BUCKET(&phar->virtual_dirs, b) {
			str_key = b->key;
			if (zend_string_starts_with_cstr(str_key, ZSTR_VAL(resource_from->path)+1, from_len) &&
				(ZSTR_LEN(str_key) == from_len || IS_SLASH(ZSTR_VAL(str_key)[from_len]))) {

				new_str_key = zend_string_alloc(ZSTR_LEN(str_key) + to_len - from_len, 0);
				memcpy(ZSTR_VAL(new_str_key), ZSTR_VAL(resource_to->path) + 1, to_len);
				memcpy(ZSTR_VAL(new_str_key) + to_len, ZSTR_VAL(str_key) + from_len, ZSTR_LEN(str_key) - from_len);
				ZSTR_VAL(new_str_key)[ZSTR_LEN(new_str_key)] = 0;

				zend_string_release_ex(str_key, 0);
				b->h = zend_string_hash_val(new_str_key);
				b->key = new_str_key;
			}
		} ZEND_HASH_FOREACH_END();
		zend_hash_rehash(&phar->virtual_dirs);

		ZEND_HASH_MAP_FOREACH_BUCKET(&phar->mounted_dirs, b) {
			str_key = b->key;
			if (zend_string_starts_with_cstr(str_key, ZSTR_VAL(resource_from->path)+1, from_len) &&
				(ZSTR_LEN(str_key) == from_len || IS_SLASH(ZSTR_VAL(str_key)[from_len]))) {

				new_str_key = zend_string_alloc(ZSTR_LEN(str_key) + to_len - from_len, 0);
				memcpy(ZSTR_VAL(new_str_key), ZSTR_VAL(resource_to->path) + 1, to_len);
				memcpy(ZSTR_VAL(new_str_key) + to_len, ZSTR_VAL(str_key) + from_len, ZSTR_LEN(str_key) - from_len);
				ZSTR_VAL(new_str_key)[ZSTR_LEN(new_str_key)] = 0;

				zend_string_release_ex(str_key, 0);
				b->h = zend_string_hash_val(new_str_key);
				b->key = new_str_key;
			}
		} ZEND_HASH_FOREACH_END();
		zend_hash_rehash(&phar->mounted_dirs);
	}

	if (is_modified) {
		phar_flush(phar, 0, 0, 0, &error);
		if (error) {
			php_url_free(resource_from);
			php_url_free(resource_to);
			php_error_docref(NULL, E_WARNING, "phar error: cannot rename \"%s\" to \"%s\": %s", url_from, url_to, error);
			efree(error);
			return 0;
		}
	}

	php_url_free(resource_from);
	php_url_free(resource_to);

	return 1;
}
/* }}} */
