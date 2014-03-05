/*
  +----------------------------------------------------------------------+
  | phar php single-file executable PHP extension                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2005-2014 The PHP Group                                |
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
#include "func_interceptors.h"

static zend_class_entry *phar_ce_archive;
static zend_class_entry *phar_ce_data;
static zend_class_entry *phar_ce_PharException;

#if HAVE_SPL
static zend_class_entry *phar_ce_entry;
#endif

#if PHP_MAJOR_VERSION > 5 || ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION >= 3))
# define PHAR_ARG_INFO
#else
# define PHAR_ARG_INFO static
#endif

static int phar_file_type(HashTable *mimes, char *file, char **mime_type TSRMLS_DC) /* {{{ */
{
	char *ext;
	phar_mime_type *mime;
	ext = strrchr(file, '.');
	if (!ext) {
		*mime_type = "text/plain";
		/* no file extension = assume text/plain */
		return PHAR_MIME_OTHER;
	}
	++ext;
	if (SUCCESS != zend_hash_find(mimes, ext, strlen(ext), (void **) &mime)) {
		*mime_type = "application/octet-stream";
		return PHAR_MIME_OTHER;
	}
	*mime_type = mime->mime;
	return mime->type;
}
/* }}} */

static void phar_mung_server_vars(char *fname, char *entry, int entry_len, char *basename, int request_uri_len TSRMLS_DC) /* {{{ */
{
	HashTable *_SERVER;
	zval **stuff;
	char *path_info;
	int basename_len = strlen(basename);
	int code;
	zval *temp;

	/* "tweak" $_SERVER variables requested in earlier call to Phar::mungServer() */
	if (!PG(http_globals)[TRACK_VARS_SERVER]) {
		return;
	}

	_SERVER = Z_ARRVAL_P(PG(http_globals)[TRACK_VARS_SERVER]);

	/* PATH_INFO and PATH_TRANSLATED should always be munged */
	if (SUCCESS == zend_hash_find(_SERVER, "PATH_INFO", sizeof("PATH_INFO"), (void **) &stuff)) {
		path_info = Z_STRVAL_PP(stuff);
		code = Z_STRLEN_PP(stuff);

		if (Z_STRLEN_PP(stuff) > entry_len && !memcmp(Z_STRVAL_PP(stuff), entry, entry_len)) {
			ZVAL_STRINGL(*stuff, Z_STRVAL_PP(stuff) + entry_len, request_uri_len, 1);

			MAKE_STD_ZVAL(temp);
			ZVAL_STRINGL(temp, path_info, code, 0);

			zend_hash_update(_SERVER, "PHAR_PATH_INFO", sizeof("PHAR_PATH_INFO"), &temp, sizeof(zval **), NULL);
		}
	}

	if (SUCCESS == zend_hash_find(_SERVER, "PATH_TRANSLATED", sizeof("PATH_TRANSLATED"), (void **) &stuff)) {
		path_info = Z_STRVAL_PP(stuff);
		code = Z_STRLEN_PP(stuff);
		Z_STRLEN_PP(stuff) = spprintf(&(Z_STRVAL_PP(stuff)), 4096, "phar://%s%s", fname, entry);

		MAKE_STD_ZVAL(temp);
		ZVAL_STRINGL(temp, path_info, code, 0);

		zend_hash_update(_SERVER, "PHAR_PATH_TRANSLATED", sizeof("PHAR_PATH_TRANSLATED"), (void *) &temp, sizeof(zval **), NULL);
	}

	if (!PHAR_GLOBALS->phar_SERVER_mung_list) {
		return;
	}

	if (PHAR_GLOBALS->phar_SERVER_mung_list & PHAR_MUNG_REQUEST_URI) {
		if (SUCCESS == zend_hash_find(_SERVER, "REQUEST_URI", sizeof("REQUEST_URI"), (void **) &stuff)) {
			path_info = Z_STRVAL_PP(stuff);
			code = Z_STRLEN_PP(stuff);

			if (Z_STRLEN_PP(stuff) > basename_len && !memcmp(Z_STRVAL_PP(stuff), basename, basename_len)) {
				ZVAL_STRINGL(*stuff, Z_STRVAL_PP(stuff) + basename_len, Z_STRLEN_PP(stuff) - basename_len, 1);

				MAKE_STD_ZVAL(temp);
				ZVAL_STRINGL(temp, path_info, code, 0);

				zend_hash_update(_SERVER, "PHAR_REQUEST_URI", sizeof("PHAR_REQUEST_URI"), (void *) &temp, sizeof(zval **), NULL);
			}
		}
	}

	if (PHAR_GLOBALS->phar_SERVER_mung_list & PHAR_MUNG_PHP_SELF) {
		if (SUCCESS == zend_hash_find(_SERVER, "PHP_SELF", sizeof("PHP_SELF"), (void **) &stuff)) {
			path_info = Z_STRVAL_PP(stuff);
			code = Z_STRLEN_PP(stuff);

			if (Z_STRLEN_PP(stuff) > basename_len && !memcmp(Z_STRVAL_PP(stuff), basename, basename_len)) {
				ZVAL_STRINGL(*stuff, Z_STRVAL_PP(stuff) + basename_len, Z_STRLEN_PP(stuff) - basename_len, 1);

				MAKE_STD_ZVAL(temp);
				ZVAL_STRINGL(temp, path_info, code, 0);

				zend_hash_update(_SERVER, "PHAR_PHP_SELF", sizeof("PHAR_PHP_SELF"), (void *) &temp, sizeof(zval **), NULL);
			}
		}
	}

	if (PHAR_GLOBALS->phar_SERVER_mung_list & PHAR_MUNG_SCRIPT_NAME) {
		if (SUCCESS == zend_hash_find(_SERVER, "SCRIPT_NAME", sizeof("SCRIPT_NAME"), (void **) &stuff)) {
			path_info = Z_STRVAL_PP(stuff);
			code = Z_STRLEN_PP(stuff);
			ZVAL_STRINGL(*stuff, entry, entry_len, 1);

			MAKE_STD_ZVAL(temp);
			ZVAL_STRINGL(temp, path_info, code, 0);

			zend_hash_update(_SERVER, "PHAR_SCRIPT_NAME", sizeof("PHAR_SCRIPT_NAME"), (void *) &temp, sizeof(zval **), NULL);
		}
	}

	if (PHAR_GLOBALS->phar_SERVER_mung_list & PHAR_MUNG_SCRIPT_FILENAME) {
		if (SUCCESS == zend_hash_find(_SERVER, "SCRIPT_FILENAME", sizeof("SCRIPT_FILENAME"), (void **) &stuff)) {
			path_info = Z_STRVAL_PP(stuff);
			code = Z_STRLEN_PP(stuff);
			Z_STRLEN_PP(stuff) = spprintf(&(Z_STRVAL_PP(stuff)), 4096, "phar://%s%s", fname, entry);

			MAKE_STD_ZVAL(temp);
			ZVAL_STRINGL(temp, path_info, code, 0);

			zend_hash_update(_SERVER, "PHAR_SCRIPT_FILENAME", sizeof("PHAR_SCRIPT_FILENAME"), (void *) &temp, sizeof(zval **), NULL);
		}
	}
}
/* }}} */

static int phar_file_action(phar_archive_data *phar, phar_entry_info *info, char *mime_type, int code, char *entry, int entry_len, char *arch, char *basename, char *ru, int ru_len TSRMLS_DC) /* {{{ */
{
	char *name = NULL, buf[8192];
	const char *cwd;
	zend_syntax_highlighter_ini syntax_highlighter_ini;
	sapi_header_line ctr = {0};
	size_t got;
	int dummy = 1, name_len;
	zend_file_handle file_handle;
	zend_op_array *new_op_array;
	zval *result = NULL;
	php_stream *fp;
	off_t position;

	switch (code) {
		case PHAR_MIME_PHPS:
			efree(basename);
			/* highlight source */
			if (entry[0] == '/') {
				name_len = spprintf(&name, 4096, "phar://%s%s", arch, entry);
			} else {
				name_len = spprintf(&name, 4096, "phar://%s/%s", arch, entry);
			}
			php_get_highlight_struct(&syntax_highlighter_ini);

			highlight_file(name, &syntax_highlighter_ini TSRMLS_CC);

			efree(name);
#ifdef PHP_WIN32
			efree(arch);
#endif
			zend_bailout();
		case PHAR_MIME_OTHER:
			/* send headers, output file contents */
			efree(basename);
			ctr.line_len = spprintf(&(ctr.line), 0, "Content-type: %s", mime_type);
			sapi_header_op(SAPI_HEADER_REPLACE, &ctr TSRMLS_CC);
			efree(ctr.line);
			ctr.line_len = spprintf(&(ctr.line), 0, "Content-length: %u", info->uncompressed_filesize);
			sapi_header_op(SAPI_HEADER_REPLACE, &ctr TSRMLS_CC);
			efree(ctr.line);

			if (FAILURE == sapi_send_headers(TSRMLS_C)) {
				zend_bailout();
			}

			/* prepare to output  */
			fp = phar_get_efp(info, 1 TSRMLS_CC);

			if (!fp) {
				char *error;
				if (!phar_open_jit(phar, info, &error TSRMLS_CC)) {
					if (error) {
						zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
						efree(error);
					}
					return -1;
				}
				fp = phar_get_efp(info, 1 TSRMLS_CC);
			}
			position = 0;
			phar_seek_efp(info, 0, SEEK_SET, 0, 1 TSRMLS_CC);

			do {
				got = php_stream_read(fp, buf, MIN(8192, info->uncompressed_filesize - position));
				if (got > 0) {
					PHPWRITE(buf, got);
					position += got;
					if (position == (off_t) info->uncompressed_filesize) {
						break;
					}
				}
			} while (1);

			zend_bailout();
		case PHAR_MIME_PHP:
			if (basename) {
				phar_mung_server_vars(arch, entry, entry_len, basename, ru_len TSRMLS_CC);
				efree(basename);
			}

			if (entry[0] == '/') {
				name_len = spprintf(&name, 4096, "phar://%s%s", arch, entry);
			} else {
				name_len = spprintf(&name, 4096, "phar://%s/%s", arch, entry);
			}

			file_handle.type = ZEND_HANDLE_FILENAME;
			file_handle.handle.fd = 0;
			file_handle.filename = name;
			file_handle.opened_path = NULL;
			file_handle.free_filename = 0;

			PHAR_G(cwd) = NULL;
			PHAR_G(cwd_len) = 0;

			if (zend_hash_add(&EG(included_files), name, name_len+1, (void *)&dummy, sizeof(int), NULL) == SUCCESS) {
				if ((cwd = zend_memrchr(entry, '/', entry_len))) {
					PHAR_G(cwd_init) = 1;
					if (entry == cwd) {
						/* root directory */
						PHAR_G(cwd_len) = 0;
						PHAR_G(cwd) = NULL;
					} else if (entry[0] == '/') {
						PHAR_G(cwd_len) = cwd - (entry + 1);
						PHAR_G(cwd) = estrndup(entry + 1, PHAR_G(cwd_len));
					} else {
						PHAR_G(cwd_len) = cwd - entry;
						PHAR_G(cwd) = estrndup(entry, PHAR_G(cwd_len));
					}
				}

				new_op_array = zend_compile_file(&file_handle, ZEND_REQUIRE TSRMLS_CC);

				if (!new_op_array) {
					zend_hash_del(&EG(included_files), name, name_len+1);
				}

				zend_destroy_file_handle(&file_handle TSRMLS_CC);

			} else {
				efree(name);
				new_op_array = NULL;
			}
#ifdef PHP_WIN32
			efree(arch);
#endif
			if (new_op_array) {
				EG(return_value_ptr_ptr) = &result;
				EG(active_op_array) = new_op_array;

				zend_try {
					zend_execute(new_op_array TSRMLS_CC);
					if (PHAR_G(cwd)) {
						efree(PHAR_G(cwd));
						PHAR_G(cwd) = NULL;
						PHAR_G(cwd_len) = 0;
					}

					PHAR_G(cwd_init) = 0;
					efree(name);
					destroy_op_array(new_op_array TSRMLS_CC);
					efree(new_op_array);


					if (EG(return_value_ptr_ptr) && *EG(return_value_ptr_ptr)) {
						zval_ptr_dtor(EG(return_value_ptr_ptr));
					}
				} zend_catch {
					if (PHAR_G(cwd)) {
						efree(PHAR_G(cwd));
						PHAR_G(cwd) = NULL;
						PHAR_G(cwd_len) = 0;
					}

					PHAR_G(cwd_init) = 0;
					efree(name);
				} zend_end_try();

				zend_bailout();
			}

			return PHAR_MIME_PHP;
	}
	return -1;
}
/* }}} */

static void phar_do_403(char *entry, int entry_len TSRMLS_DC) /* {{{ */
{
	sapi_header_line ctr = {0};

	ctr.response_code = 403;
	ctr.line_len = sizeof("HTTP/1.0 403 Access Denied")-1;
	ctr.line = "HTTP/1.0 403 Access Denied";
	sapi_header_op(SAPI_HEADER_REPLACE, &ctr TSRMLS_CC);
	sapi_send_headers(TSRMLS_C);
	PHPWRITE("<html>\n <head>\n  <title>Access Denied</title>\n </head>\n <body>\n  <h1>403 - File ", sizeof("<html>\n <head>\n  <title>Access Denied</title>\n </head>\n <body>\n  <h1>403 - File ") - 1);
	PHPWRITE(entry, entry_len);
	PHPWRITE(" Access Denied</h1>\n </body>\n</html>", sizeof(" Access Denied</h1>\n </body>\n</html>") - 1);
}
/* }}} */

static void phar_do_404(phar_archive_data *phar, char *fname, int fname_len, char *f404, int f404_len, char *entry, int entry_len TSRMLS_DC) /* {{{ */
{
	sapi_header_line ctr = {0};
	phar_entry_info	*info;

	if (phar && f404_len) {
		info = phar_get_entry_info(phar, f404, f404_len, NULL, 1 TSRMLS_CC);

		if (info) {
			phar_file_action(phar, info, "text/html", PHAR_MIME_PHP, f404, f404_len, fname, NULL, NULL, 0 TSRMLS_CC);
			return;
		}
	}

	ctr.response_code = 404;
	ctr.line_len = sizeof("HTTP/1.0 404 Not Found")-1;
	ctr.line = "HTTP/1.0 404 Not Found";
	sapi_header_op(SAPI_HEADER_REPLACE, &ctr TSRMLS_CC);
	sapi_send_headers(TSRMLS_C);
	PHPWRITE("<html>\n <head>\n  <title>File Not Found</title>\n </head>\n <body>\n  <h1>404 - File ", sizeof("<html>\n <head>\n  <title>File Not Found</title>\n </head>\n <body>\n  <h1>404 - File ") - 1);
	PHPWRITE(entry, entry_len);
	PHPWRITE(" Not Found</h1>\n </body>\n</html>",  sizeof(" Not Found</h1>\n </body>\n</html>") - 1);
}
/* }}} */

/* post-process REQUEST_URI and retrieve the actual request URI.  This is for
   cases like http://localhost/blah.phar/path/to/file.php/extra/stuff
   which calls "blah.phar" file "path/to/file.php" with PATH_INFO "/extra/stuff" */
static void phar_postprocess_ru_web(char *fname, int fname_len, char **entry, int *entry_len, char **ru, int *ru_len TSRMLS_DC) /* {{{ */
{
	char *e = *entry + 1, *u = NULL, *u1 = NULL, *saveu = NULL;
	int e_len = *entry_len - 1, u_len = 0;
	phar_archive_data **pphar = NULL;

	/* we already know we can retrieve the phar if we reach here */
	zend_hash_find(&(PHAR_GLOBALS->phar_fname_map), fname, fname_len, (void **) &pphar);

	if (!pphar && PHAR_G(manifest_cached)) {
		zend_hash_find(&cached_phars, fname, fname_len, (void **) &pphar);
	}

	do {
		if (zend_hash_exists(&((*pphar)->manifest), e, e_len)) {
			if (u) {
				u[0] = '/';
				*ru = estrndup(u, u_len+1);
				++u_len;
				u[0] = '\0';
			} else {
				*ru = NULL;
			}
			*ru_len = u_len;
			*entry_len = e_len + 1;
			return;
		}

		if (u) {
			u1 = strrchr(e, '/');
			u[0] = '/';
			saveu = u;
			e_len += u_len + 1;
			u = u1;
			if (!u) {
				return;
			}
		} else {
			u = strrchr(e, '/');
			if (!u) {
				if (saveu) {
					saveu[0] = '/';
				}
				return;
			}
		}

		u[0] = '\0';
		u_len = strlen(u + 1);
		e_len -= u_len + 1;

		if (e_len < 0) {
			if (saveu) {
				saveu[0] = '/';
			}
			return;
		}
	} while (1);
}
/* }}} */

/* {{{ proto void Phar::running([bool retphar = true])
 * return the name of the currently running phar archive.  If the optional parameter
 * is set to true, return the phar:// URL to the currently running phar
 */
PHP_METHOD(Phar, running)
{
	char *fname, *arch, *entry;
	int fname_len, arch_len, entry_len;
	zend_bool retphar = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &retphar) == FAILURE) {
		return;
	}

	fname = (char*)zend_get_executed_filename(TSRMLS_C);
	fname_len = strlen(fname);

	if (fname_len > 7 && !memcmp(fname, "phar://", 7) && SUCCESS == phar_split_fname(fname, fname_len, &arch, &arch_len, &entry, &entry_len, 2, 0 TSRMLS_CC)) {
		efree(entry);
		if (retphar) {
			RETVAL_STRINGL(fname, arch_len + 7, 1);
			efree(arch);
			return;
		} else {
			RETURN_STRINGL(arch, arch_len, 0);
		}
	}

	RETURN_STRINGL("", 0, 1);
}
/* }}} */

/* {{{ proto void Phar::mount(string pharpath, string externalfile)
 * mount an external file or path to a location within the phar.  This maps
 * an external file or directory to a location within the phar archive, allowing
 * reference to an external location as if it were within the phar archive.  This
 * is useful for writable temp files like databases
 */
PHP_METHOD(Phar, mount)
{
	char *fname, *arch = NULL, *entry = NULL, *path, *actual;
	int fname_len, arch_len, entry_len, path_len, actual_len;
	phar_archive_data **pphar;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &path, &path_len, &actual, &actual_len) == FAILURE) {
		return;
	}

	fname = (char*)zend_get_executed_filename(TSRMLS_C);
	fname_len = strlen(fname);

#ifdef PHP_WIN32
	phar_unixify_path_separators(fname, fname_len);
#endif

	if (fname_len > 7 && !memcmp(fname, "phar://", 7) && SUCCESS == phar_split_fname(fname, fname_len, &arch, &arch_len, &entry, &entry_len, 2, 0 TSRMLS_CC)) {
		efree(entry);
		entry = NULL;

		if (path_len > 7 && !memcmp(path, "phar://", 7)) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "Can only mount internal paths within a phar archive, use a relative path instead of \"%s\"", path);
			efree(arch);
			return;
		}
carry_on2:
		if (SUCCESS != zend_hash_find(&(PHAR_GLOBALS->phar_fname_map), arch, arch_len, (void **)&pphar)) {
			if (PHAR_G(manifest_cached) && SUCCESS == zend_hash_find(&cached_phars, arch, arch_len, (void **)&pphar)) {
				if (SUCCESS == phar_copy_on_write(pphar TSRMLS_CC)) {
					goto carry_on;
				}
			}

			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s is not a phar archive, cannot mount", arch);

			if (arch) {
				efree(arch);
			}
			return;
		}
carry_on:
		if (SUCCESS != phar_mount_entry(*pphar, actual, actual_len, path, path_len TSRMLS_CC)) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "Mounting of %s to %s within phar %s failed", path, actual, arch);
			if (path && path == entry) {
				efree(entry);
			}

			if (arch) {
				efree(arch);
			}

			return;
		}

		if (entry && path && path == entry) {
			efree(entry);
		}

		if (arch) {
			efree(arch);
		}

		return;
	} else if (PHAR_GLOBALS->phar_fname_map.arBuckets && SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_fname_map), fname, fname_len, (void **)&pphar)) {
		goto carry_on;
	} else if (PHAR_G(manifest_cached) && SUCCESS == zend_hash_find(&cached_phars, fname, fname_len, (void **)&pphar)) {
		if (SUCCESS == phar_copy_on_write(pphar TSRMLS_CC)) {
			goto carry_on;
		}

		goto carry_on;
	} else if (SUCCESS == phar_split_fname(path, path_len, &arch, &arch_len, &entry, &entry_len, 2, 0 TSRMLS_CC)) {
		path = entry;
		path_len = entry_len;
		goto carry_on2;
	}

	zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "Mounting of %s to %s failed", path, actual);
}
/* }}} */

/* {{{ proto void Phar::webPhar([string alias, [string index, [string f404, [array mimetypes, [callback rewrites]]]]])
 * mapPhar for web-based phars. Reads the currently executed file (a phar)
 * and registers its manifest. When executed in the CLI or CGI command-line sapi,
 * this works exactly like mapPhar().  When executed by a web-based sapi, this
 * reads $_SERVER['REQUEST_URI'] (the actual original value) and parses out the
 * intended internal file.
 */
PHP_METHOD(Phar, webPhar)
{
	zval *mimeoverride = NULL, *rewrite = NULL;
	char *alias = NULL, *error, *index_php = NULL, *f404 = NULL, *ru = NULL;
	int alias_len = 0, ret, f404_len = 0, free_pathinfo = 0, ru_len = 0;
	char *fname, *path_info, *mime_type = NULL, *entry, *pt;
	const char *basename;
	int fname_len, entry_len, code, index_php_len = 0, not_cgi;
	phar_archive_data *phar = NULL;
	phar_entry_info *info = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!s!saz", &alias, &alias_len, &index_php, &index_php_len, &f404, &f404_len, &mimeoverride, &rewrite) == FAILURE) {
		return;
	}

	phar_request_initialize(TSRMLS_C);
	fname = (char*)zend_get_executed_filename(TSRMLS_C);
	fname_len = strlen(fname);

	if (phar_open_executed_filename(alias, alias_len, &error TSRMLS_CC) != SUCCESS) {
		if (error) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
			efree(error);
		}
		return;
	}

	/* retrieve requested file within phar */
	if (!(SG(request_info).request_method && SG(request_info).request_uri && (!strcmp(SG(request_info).request_method, "GET") || !strcmp(SG(request_info).request_method, "POST")))) {
		return;
	}

#ifdef PHP_WIN32
	fname = estrndup(fname, fname_len);
	phar_unixify_path_separators(fname, fname_len);
#endif
	basename = zend_memrchr(fname, '/', fname_len);

	if (!basename) {
		basename = fname;
	} else {
		++basename;
	}

	if ((strlen(sapi_module.name) == sizeof("cgi-fcgi")-1 && !strncmp(sapi_module.name, "cgi-fcgi", sizeof("cgi-fcgi")-1))
		|| (strlen(sapi_module.name) == sizeof("cgi")-1 && !strncmp(sapi_module.name, "cgi", sizeof("cgi")-1))) {

		if (PG(http_globals)[TRACK_VARS_SERVER]) {
			HashTable *_server = Z_ARRVAL_P(PG(http_globals)[TRACK_VARS_SERVER]);
			zval **z_script_name, **z_path_info;

			if (SUCCESS != zend_hash_find(_server, "SCRIPT_NAME", sizeof("SCRIPT_NAME"), (void**)&z_script_name) ||
				IS_STRING != Z_TYPE_PP(z_script_name) ||
				!strstr(Z_STRVAL_PP(z_script_name), basename)) {
				return;
			}

			if (SUCCESS == zend_hash_find(_server, "PATH_INFO", sizeof("PATH_INFO"), (void**)&z_path_info) &&
				IS_STRING == Z_TYPE_PP(z_path_info)) {
				entry_len = Z_STRLEN_PP(z_path_info);
				entry = estrndup(Z_STRVAL_PP(z_path_info), entry_len);
				path_info = emalloc(Z_STRLEN_PP(z_script_name) + entry_len + 1);
				memcpy(path_info, Z_STRVAL_PP(z_script_name), Z_STRLEN_PP(z_script_name));
				memcpy(path_info + Z_STRLEN_PP(z_script_name), entry, entry_len + 1);
				free_pathinfo = 1;
			} else {
				entry_len = 0;
				entry = estrndup("", 0);
				path_info = Z_STRVAL_PP(z_script_name);
			}

			pt = estrndup(Z_STRVAL_PP(z_script_name), Z_STRLEN_PP(z_script_name));

		} else {
			char *testit;

			testit = sapi_getenv("SCRIPT_NAME", sizeof("SCRIPT_NAME")-1 TSRMLS_CC);
			if (!(pt = strstr(testit, basename))) {
				efree(testit);
				return;
			}

			path_info = sapi_getenv("PATH_INFO", sizeof("PATH_INFO")-1 TSRMLS_CC);

			if (path_info) {
				entry = path_info;
				entry_len = strlen(entry);
				spprintf(&path_info, 0, "%s%s", testit, path_info);
				free_pathinfo = 1;
			} else {
				path_info = testit;
				free_pathinfo = 1;
				entry = estrndup("", 0);
				entry_len = 0;
			}

			pt = estrndup(testit, (pt - testit) + (fname_len - (basename - fname)));
		}
		not_cgi = 0;
	} else {
		path_info = SG(request_info).request_uri;

		if (!(pt = strstr(path_info, basename))) {
			/* this can happen with rewrite rules - and we have no idea what to do then, so return */
			return;
		}

		entry_len = strlen(path_info);
		entry_len -= (pt - path_info) + (fname_len - (basename - fname));
		entry = estrndup(pt + (fname_len - (basename - fname)), entry_len);

		pt = estrndup(path_info, (pt - path_info) + (fname_len - (basename - fname)));
		not_cgi = 1;
	}

	if (rewrite) {
		zend_fcall_info fci;
		zend_fcall_info_cache fcc;
		zval *params, *retval_ptr, **zp[1];

		MAKE_STD_ZVAL(params);
		ZVAL_STRINGL(params, entry, entry_len, 1);
		zp[0] = &params;

		if (FAILURE == zend_fcall_info_init(rewrite, 0, &fci, &fcc, NULL, NULL TSRMLS_CC)) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar error: invalid rewrite callback");

			if (free_pathinfo) {
				efree(path_info);
			}

			return;
		}

		fci.param_count = 1;
		fci.params = zp;
		Z_ADDREF_P(params);
		fci.retval_ptr_ptr = &retval_ptr;

		if (FAILURE == zend_call_function(&fci, &fcc TSRMLS_CC)) {
			if (!EG(exception)) {
				zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar error: failed to call rewrite callback");
			}

			if (free_pathinfo) {
				efree(path_info);
			}

			return;
		}

		if (!fci.retval_ptr_ptr || !retval_ptr) {
			if (free_pathinfo) {
				efree(path_info);
			}
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar error: rewrite callback must return a string or false");
			return;
		}

		switch (Z_TYPE_P(retval_ptr)) {
			case IS_STRING:
				efree(entry);

				if (fci.retval_ptr_ptr != &retval_ptr) {
					entry = estrndup(Z_STRVAL_PP(fci.retval_ptr_ptr), Z_STRLEN_PP(fci.retval_ptr_ptr));
					entry_len = Z_STRLEN_PP(fci.retval_ptr_ptr);
				} else {
					entry = Z_STRVAL_P(retval_ptr);
					entry_len = Z_STRLEN_P(retval_ptr);
				}

				break;
			case IS_BOOL:
				phar_do_403(entry, entry_len TSRMLS_CC);

				if (free_pathinfo) {
					efree(path_info);
				}

				zend_bailout();
				return;
			default:
				efree(retval_ptr);

				if (free_pathinfo) {
					efree(path_info);
				}

				zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar error: rewrite callback must return a string or false");
				return;
		}
	}

	if (entry_len) {
		phar_postprocess_ru_web(fname, fname_len, &entry, &entry_len, &ru, &ru_len TSRMLS_CC);
	}

	if (!entry_len || (entry_len == 1 && entry[0] == '/')) {
		efree(entry);
		/* direct request */
		if (index_php_len) {
			entry = index_php;
			entry_len = index_php_len;
			if (entry[0] != '/') {
				spprintf(&entry, 0, "/%s", index_php);
				++entry_len;
			}
		} else {
			/* assume "index.php" is starting point */
			entry = estrndup("/index.php", sizeof("/index.php"));
			entry_len = sizeof("/index.php")-1;
		}

		if (FAILURE == phar_get_archive(&phar, fname, fname_len, NULL, 0, NULL TSRMLS_CC) ||
			(info = phar_get_entry_info(phar, entry, entry_len, NULL, 0 TSRMLS_CC)) == NULL) {
			phar_do_404(phar, fname, fname_len, f404, f404_len, entry, entry_len TSRMLS_CC);

			if (free_pathinfo) {
				efree(path_info);
			}

			zend_bailout();
		} else {
			char *tmp = NULL, sa = '\0';
			sapi_header_line ctr = {0};
			ctr.response_code = 301;
			ctr.line_len = sizeof("HTTP/1.1 301 Moved Permanently")-1;
			ctr.line = "HTTP/1.1 301 Moved Permanently";
			sapi_header_op(SAPI_HEADER_REPLACE, &ctr TSRMLS_CC);

			if (not_cgi) {
				tmp = strstr(path_info, basename) + fname_len;
				sa = *tmp;
				*tmp = '\0';
			}

			ctr.response_code = 0;

			if (path_info[strlen(path_info)-1] == '/') {
				ctr.line_len = spprintf(&(ctr.line), 4096, "Location: %s%s", path_info, entry + 1);
			} else {
				ctr.line_len = spprintf(&(ctr.line), 4096, "Location: %s%s", path_info, entry);
			}

			if (not_cgi) {
				*tmp = sa;
			}

			if (free_pathinfo) {
				efree(path_info);
			}

			sapi_header_op(SAPI_HEADER_REPLACE, &ctr TSRMLS_CC);
			sapi_send_headers(TSRMLS_C);
			efree(ctr.line);
			zend_bailout();
		}
	}

	if (FAILURE == phar_get_archive(&phar, fname, fname_len, NULL, 0, NULL TSRMLS_CC) ||
		(info = phar_get_entry_info(phar, entry, entry_len, NULL, 0 TSRMLS_CC)) == NULL) {
		phar_do_404(phar, fname, fname_len, f404, f404_len, entry, entry_len TSRMLS_CC);
#ifdef PHP_WIN32
		efree(fname);
#endif
		zend_bailout();
	}

	if (mimeoverride && zend_hash_num_elements(Z_ARRVAL_P(mimeoverride))) {
		const char *ext = zend_memrchr(entry, '.', entry_len);
		zval **val;

		if (ext) {
			++ext;

			if (SUCCESS == zend_hash_find(Z_ARRVAL_P(mimeoverride), ext, strlen(ext)+1, (void **) &val)) {
				switch (Z_TYPE_PP(val)) {
					case IS_LONG:
						if (Z_LVAL_PP(val) == PHAR_MIME_PHP || Z_LVAL_PP(val) == PHAR_MIME_PHPS) {
							mime_type = "";
							code = Z_LVAL_PP(val);
						} else {
							zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "Unknown mime type specifier used, only Phar::PHP, Phar::PHPS and a mime type string are allowed");
#ifdef PHP_WIN32
							efree(fname);
#endif
							RETURN_FALSE;
						}
						break;
					case IS_STRING:
						mime_type = Z_STRVAL_PP(val);
						code = PHAR_MIME_OTHER;
						break;
					default:
						zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "Unknown mime type specifier used (not a string or int), only Phar::PHP, Phar::PHPS and a mime type string are allowed");
#ifdef PHP_WIN32
						efree(fname);
#endif
						RETURN_FALSE;
				}
			}
		}
	}

	if (!mime_type) {
		code = phar_file_type(&PHAR_G(mime_types), entry, &mime_type TSRMLS_CC);
	}
	ret = phar_file_action(phar, info, mime_type, code, entry, entry_len, fname, pt, ru, ru_len TSRMLS_CC);
}
/* }}} */

/* {{{ proto void Phar::mungServer(array munglist)
 * Defines a list of up to 4 $_SERVER variables that should be modified for execution
 * to mask the presence of the phar archive.  This should be used in conjunction with
 * Phar::webPhar(), and has no effect otherwise
 * SCRIPT_NAME, PHP_SELF, REQUEST_URI and SCRIPT_FILENAME
 */
PHP_METHOD(Phar, mungServer)
{
	zval *mungvalues;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &mungvalues) == FAILURE) {
		return;
	}

	if (!zend_hash_num_elements(Z_ARRVAL_P(mungvalues))) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "No values passed to Phar::mungServer(), expecting an array of any of these strings: PHP_SELF, REQUEST_URI, SCRIPT_FILENAME, SCRIPT_NAME");
		return;
	}

	if (zend_hash_num_elements(Z_ARRVAL_P(mungvalues)) > 4) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "Too many values passed to Phar::mungServer(), expecting an array of any of these strings: PHP_SELF, REQUEST_URI, SCRIPT_FILENAME, SCRIPT_NAME");
		return;
	}

	phar_request_initialize(TSRMLS_C);

	for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(mungvalues)); SUCCESS == zend_hash_has_more_elements(Z_ARRVAL_P(mungvalues)); zend_hash_move_forward(Z_ARRVAL_P(mungvalues))) {
		zval **data = NULL;

		if (SUCCESS != zend_hash_get_current_data(Z_ARRVAL_P(mungvalues), (void **) &data)) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "unable to retrieve array value in Phar::mungServer()");
			return;
		}

		if (Z_TYPE_PP(data) != IS_STRING) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "Non-string value passed to Phar::mungServer(), expecting an array of any of these strings: PHP_SELF, REQUEST_URI, SCRIPT_FILENAME, SCRIPT_NAME");
			return;
		}

		if (Z_STRLEN_PP(data) == sizeof("PHP_SELF")-1 && !strncmp(Z_STRVAL_PP(data), "PHP_SELF", sizeof("PHP_SELF")-1)) {
			PHAR_GLOBALS->phar_SERVER_mung_list |= PHAR_MUNG_PHP_SELF;
		}

		if (Z_STRLEN_PP(data) == sizeof("REQUEST_URI")-1) {
			if (!strncmp(Z_STRVAL_PP(data), "REQUEST_URI", sizeof("REQUEST_URI")-1)) {
				PHAR_GLOBALS->phar_SERVER_mung_list |= PHAR_MUNG_REQUEST_URI;
			}
			if (!strncmp(Z_STRVAL_PP(data), "SCRIPT_NAME", sizeof("SCRIPT_NAME")-1)) {
				PHAR_GLOBALS->phar_SERVER_mung_list |= PHAR_MUNG_SCRIPT_NAME;
			}
		}

		if (Z_STRLEN_PP(data) == sizeof("SCRIPT_FILENAME")-1 && !strncmp(Z_STRVAL_PP(data), "SCRIPT_FILENAME", sizeof("SCRIPT_FILENAME")-1)) {
			PHAR_GLOBALS->phar_SERVER_mung_list |= PHAR_MUNG_SCRIPT_FILENAME;
		}
	}
}
/* }}} */

/* {{{ proto void Phar::interceptFileFuncs()
 * instructs phar to intercept fopen, file_get_contents, opendir, and all of the stat-related functions
 * and return stat on files within the phar for relative paths
 *
 * Once called, this cannot be reversed, and continue until the end of the request.
 *
 * This allows legacy scripts to be pharred unmodified
 */
PHP_METHOD(Phar, interceptFileFuncs)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	phar_intercept_functions(TSRMLS_C);
}
/* }}} */

/* {{{ proto array Phar::createDefaultStub([string indexfile[, string webindexfile]])
 * Return a stub that can be used to run a phar-based archive without the phar extension
 * indexfile is the CLI startup filename, which defaults to "index.php", webindexfile
 * is the web startup filename, and also defaults to "index.php"
 */
PHP_METHOD(Phar, createDefaultStub)
{
	char *index = NULL, *webindex = NULL, *stub, *error;
	int index_len = 0, webindex_len = 0;
	size_t stub_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|ss", &index, &index_len, &webindex, &webindex_len) == FAILURE) {
		return;
	}

	stub = phar_create_default_stub(index, webindex, &stub_len, &error TSRMLS_CC);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
		efree(error);
		return;
	}
	RETURN_STRINGL(stub, stub_len, 0);
}
/* }}} */

/* {{{ proto mixed Phar::mapPhar([string alias, [int dataoffset]])
 * Reads the currently executed file (a phar) and registers its manifest */
PHP_METHOD(Phar, mapPhar)
{
	char *alias = NULL, *error;
	int alias_len = 0;
	long dataoffset = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!l", &alias, &alias_len, &dataoffset) == FAILURE) {
		return;
	}

	phar_request_initialize(TSRMLS_C);

	RETVAL_BOOL(phar_open_executed_filename(alias, alias_len, &error TSRMLS_CC) == SUCCESS);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
		efree(error);
	}
} /* }}} */

/* {{{ proto mixed Phar::loadPhar(string filename [, string alias])
 * Loads any phar archive with an alias */
PHP_METHOD(Phar, loadPhar)
{
	char *fname, *alias = NULL, *error;
	int fname_len, alias_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s!", &fname, &fname_len, &alias, &alias_len) == FAILURE) {
		return;
	}

	phar_request_initialize(TSRMLS_C);

	RETVAL_BOOL(phar_open_from_filename(fname, fname_len, alias, alias_len, REPORT_ERRORS, NULL, &error TSRMLS_CC) == SUCCESS);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
		efree(error);
	}
} /* }}} */

/* {{{ proto string Phar::apiVersion()
 * Returns the api version */
PHP_METHOD(Phar, apiVersion)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	RETURN_STRINGL(PHP_PHAR_API_VERSION, sizeof(PHP_PHAR_API_VERSION)-1, 1);
}
/* }}}*/

/* {{{ proto bool Phar::canCompress([int method])
 * Returns whether phar extension supports compression using zlib/bzip2 */
PHP_METHOD(Phar, canCompress)
{
	long method = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &method) == FAILURE) {
		return;
	}

	phar_request_initialize(TSRMLS_C);
	switch (method) {
	case PHAR_ENT_COMPRESSED_GZ:
		if (PHAR_G(has_zlib)) {
			RETURN_TRUE;
		} else {
			RETURN_FALSE;
		}
	case PHAR_ENT_COMPRESSED_BZ2:
		if (PHAR_G(has_bz2)) {
			RETURN_TRUE;
		} else {
			RETURN_FALSE;
		}
	default:
		if (PHAR_G(has_zlib) || PHAR_G(has_bz2)) {
			RETURN_TRUE;
		} else {
			RETURN_FALSE;
		}
	}
}
/* }}} */

/* {{{ proto bool Phar::canWrite()
 * Returns whether phar extension supports writing and creating phars */
PHP_METHOD(Phar, canWrite)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	RETURN_BOOL(!PHAR_G(readonly));
}
/* }}} */

/* {{{ proto bool Phar::isValidPharFilename(string filename[, bool executable = true])
 * Returns whether the given filename is a valid phar filename */
PHP_METHOD(Phar, isValidPharFilename)
{
	char *fname;
	const char *ext_str;
	int fname_len, ext_len, is_executable;
	zend_bool executable = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|b", &fname, &fname_len, &executable) == FAILURE) {
		return;
	}

	is_executable = executable;
	RETVAL_BOOL(phar_detect_phar_fname_ext(fname, fname_len, &ext_str, &ext_len, is_executable, 2, 1 TSRMLS_CC) == SUCCESS);
}
/* }}} */

#if HAVE_SPL
/**
 * from spl_directory
 */
static void phar_spl_foreign_dtor(spl_filesystem_object *object TSRMLS_DC) /* {{{ */
{
	phar_archive_data *phar = (phar_archive_data *) object->oth;

	if (!phar->is_persistent) {
		phar_archive_delref(phar TSRMLS_CC);
	}

	object->oth = NULL;
}
/* }}} */

/**
 * from spl_directory
 */
static void phar_spl_foreign_clone(spl_filesystem_object *src, spl_filesystem_object *dst TSRMLS_DC) /* {{{ */
{
	phar_archive_data *phar_data = (phar_archive_data *) dst->oth;

	if (!phar_data->is_persistent) {
		++(phar_data->refcount);
	}
}
/* }}} */

static spl_other_handler phar_spl_foreign_handler = {
	phar_spl_foreign_dtor,
	phar_spl_foreign_clone
};
#endif /* HAVE_SPL */

/* {{{ proto void Phar::__construct(string fname [, int flags [, string alias]])
 * Construct a Phar archive object
 *
 * proto void PharData::__construct(string fname [[, int flags [, string alias]], int file format = Phar::TAR])
 * Construct a PharData archive object
 *
 * This function is used as the constructor for both the Phar and PharData
 * classes, hence the two prototypes above.
 */
PHP_METHOD(Phar, __construct)
{
#if !HAVE_SPL
	zend_throw_exception_ex(zend_exception_get_default(TSRMLS_C), 0 TSRMLS_CC, "Cannot instantiate Phar object without SPL extension");
#else
	char *fname, *alias = NULL, *error, *arch = NULL, *entry = NULL, *save_fname;
	int fname_len, alias_len = 0, arch_len, entry_len, is_data;
	long flags = SPL_FILE_DIR_SKIPDOTS|SPL_FILE_DIR_UNIXPATHS;
	long format = 0;
	phar_archive_object *phar_obj;
	phar_archive_data   *phar_data;
	zval *zobj = getThis(), arg1, arg2;

	phar_obj = (phar_archive_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	is_data = instanceof_function(Z_OBJCE_P(zobj), phar_ce_data TSRMLS_CC);

	if (is_data) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ls!l", &fname, &fname_len, &flags, &alias, &alias_len, &format) == FAILURE) {
			return;
		}
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ls!", &fname, &fname_len, &flags, &alias, &alias_len) == FAILURE) {
			return;
		}
	}

	if (phar_obj->arc.archive) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Cannot call constructor twice");
		return;
	}

	save_fname = fname;
	if (SUCCESS == phar_split_fname(fname, fname_len, &arch, &arch_len, &entry, &entry_len, !is_data, 2 TSRMLS_CC)) {
		/* use arch (the basename for the archive) for fname instead of fname */
		/* this allows support for RecursiveDirectoryIterator of subdirectories */
#ifdef PHP_WIN32
		phar_unixify_path_separators(arch, arch_len);
#endif
		fname = arch;
		fname_len = arch_len;
#ifdef PHP_WIN32
	} else {
		arch = estrndup(fname, fname_len);
		arch_len = fname_len;
		fname = arch;
		phar_unixify_path_separators(arch, arch_len);
#endif
	}

	if (phar_open_or_create_filename(fname, fname_len, alias, alias_len, is_data, REPORT_ERRORS, &phar_data, &error TSRMLS_CC) == FAILURE) {

		if (fname == arch && fname != save_fname) {
			efree(arch);
			fname = save_fname;
		}

		if (entry) {
			efree(entry);
		}

		if (error) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"%s", error);
			efree(error);
		} else {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"Phar creation or opening failed");
		}

		return;
	}

	if (is_data && phar_data->is_tar && phar_data->is_brandnew && format == PHAR_FORMAT_ZIP) {
		phar_data->is_zip = 1;
		phar_data->is_tar = 0;
	}

	if (fname == arch) {
		efree(arch);
		fname = save_fname;
	}

	if ((is_data && !phar_data->is_data) || (!is_data && phar_data->is_data)) {
		if (is_data) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"PharData class can only be used for non-executable tar and zip archives");
		} else {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"Phar class can only be used for executable tar and zip archives");
		}
		efree(entry);
		return;
	}

	is_data = phar_data->is_data;

	if (!phar_data->is_persistent) {
		++(phar_data->refcount);
	}

	phar_obj->arc.archive = phar_data;
	phar_obj->spl.oth_handler = &phar_spl_foreign_handler;

	if (entry) {
		fname_len = spprintf(&fname, 0, "phar://%s%s", phar_data->fname, entry);
		efree(entry);
	} else {
		fname_len = spprintf(&fname, 0, "phar://%s", phar_data->fname);
	}

	INIT_PZVAL(&arg1);
	ZVAL_STRINGL(&arg1, fname, fname_len, 0);
	INIT_PZVAL(&arg2);
	ZVAL_LONG(&arg2, flags);

	zend_call_method_with_2_params(&zobj, Z_OBJCE_P(zobj), 
		&spl_ce_RecursiveDirectoryIterator->constructor, "__construct", NULL, &arg1, &arg2);

	if (!phar_data->is_persistent) {
		phar_obj->arc.archive->is_data = is_data;
	} else if (!EG(exception)) {
		/* register this guy so we can modify if necessary */
		zend_hash_add(&PHAR_GLOBALS->phar_persist_map, (const char *) phar_obj->arc.archive, sizeof(phar_obj->arc.archive), (void *) &phar_obj, sizeof(phar_archive_object **), NULL);
	}

	phar_obj->spl.info_class = phar_ce_entry;
	efree(fname);
#endif /* HAVE_SPL */
}
/* }}} */

/* {{{ proto array Phar::getSupportedSignatures()
 * Return array of supported signature types
 */
PHP_METHOD(Phar, getSupportedSignatures)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	
	array_init(return_value);

	add_next_index_stringl(return_value, "MD5", 3, 1);
	add_next_index_stringl(return_value, "SHA-1", 5, 1);
#ifdef PHAR_HASH_OK
	add_next_index_stringl(return_value, "SHA-256", 7, 1);
	add_next_index_stringl(return_value, "SHA-512", 7, 1);
#endif
#if PHAR_HAVE_OPENSSL
	add_next_index_stringl(return_value, "OpenSSL", 7, 1);
#else
	if (zend_hash_exists(&module_registry, "openssl", sizeof("openssl"))) {
		add_next_index_stringl(return_value, "OpenSSL", 7, 1);
	}
#endif
}
/* }}} */

/* {{{ proto array Phar::getSupportedCompression()
 * Return array of supported comparession algorithms
 */
PHP_METHOD(Phar, getSupportedCompression)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	
	array_init(return_value);
	phar_request_initialize(TSRMLS_C);

	if (PHAR_G(has_zlib)) {
		add_next_index_stringl(return_value, "GZ", 2, 1);
	}

	if (PHAR_G(has_bz2)) {
		add_next_index_stringl(return_value, "BZIP2", 5, 1);
	}
}
/* }}} */

/* {{{ proto array Phar::unlinkArchive(string archive)
 * Completely remove a phar archive from memory and disk
 */
PHP_METHOD(Phar, unlinkArchive)
{
	char *fname, *error, *zname, *arch, *entry;
	int fname_len, zname_len, arch_len, entry_len;
	phar_archive_data *phar;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &fname, &fname_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (!fname_len) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "Unknown phar archive \"\"");
		return;
	}

	if (FAILURE == phar_open_from_filename(fname, fname_len, NULL, 0, REPORT_ERRORS, &phar, &error TSRMLS_CC)) {
		if (error) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "Unknown phar archive \"%s\": %s", fname, error);
			efree(error);
		} else {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "Unknown phar archive \"%s\"", fname);
		}
		return;
	}

	zname = (char*)zend_get_executed_filename(TSRMLS_C);
	zname_len = strlen(zname);

	if (zname_len > 7 && !memcmp(zname, "phar://", 7) && SUCCESS == phar_split_fname(zname, zname_len, &arch, &arch_len, &entry, &entry_len, 2, 0 TSRMLS_CC)) {
		if (arch_len == fname_len && !memcmp(arch, fname, arch_len)) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar archive \"%s\" cannot be unlinked from within itself", fname);
			efree(arch);
			efree(entry);
			return;
		}
		efree(arch);
		efree(entry);
	}

	if (phar->is_persistent) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar archive \"%s\" is in phar.cache_list, cannot unlinkArchive()", fname);
		return;
	}

	if (phar->refcount) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar archive \"%s\" has open file handles or objects.  fclose() all file handles, and unset() all objects prior to calling unlinkArchive()", fname);
		return;
	}

	fname = estrndup(phar->fname, phar->fname_len);

	/* invalidate phar cache */
	PHAR_G(last_phar) = NULL;
	PHAR_G(last_phar_name) = PHAR_G(last_alias) = NULL;

	phar_archive_delref(phar TSRMLS_CC);
	unlink(fname);
	efree(fname);
	RETURN_TRUE;
}
/* }}} */

#if HAVE_SPL

#define PHAR_ARCHIVE_OBJECT() \
	phar_archive_object *phar_obj = (phar_archive_object*)zend_object_store_get_object(getThis() TSRMLS_CC); \
	if (!phar_obj->arc.archive) { \
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, \
			"Cannot call method on an uninitialized Phar object"); \
		return; \
	}

/* {{{ proto void Phar::__destruct()
 * if persistent, remove from the cache
 */
PHP_METHOD(Phar, __destruct)
{
	phar_archive_object *phar_obj = (phar_archive_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (phar_obj->arc.archive && phar_obj->arc.archive->is_persistent) {
		zend_hash_del(&PHAR_GLOBALS->phar_persist_map, (const char *) phar_obj->arc.archive, sizeof(phar_obj->arc.archive));
	}
}
/* }}} */

struct _phar_t {
	phar_archive_object *p;
	zend_class_entry *c;
	char *b;
	uint l;
	zval *ret;
	int count;
	php_stream *fp;
};

static int phar_build(zend_object_iterator *iter, void *puser TSRMLS_DC) /* {{{ */
{
	zval **value;
	zend_bool close_fp = 1;
	struct _phar_t *p_obj = (struct _phar_t*) puser;
	uint str_key_len, base_len = p_obj->l, fname_len;
	phar_entry_data *data;
	php_stream *fp;
	size_t contents_len;
	char *fname, *error = NULL, *base = p_obj->b, *opened, *save = NULL, *temp = NULL;
	char *str_key;
	zend_class_entry *ce = p_obj->c;
	phar_archive_object *phar_obj = p_obj->p;
	char *str = "[stream]";

	iter->funcs->get_current_data(iter, &value TSRMLS_CC);

	if (EG(exception)) {
		return ZEND_HASH_APPLY_STOP;
	}

	if (!value) {
		/* failure in get_current_data */
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Iterator %v returned no value", ce->name);
		return ZEND_HASH_APPLY_STOP;
	}

	switch (Z_TYPE_PP(value)) {
		case IS_STRING:
			break;
		case IS_RESOURCE:
			php_stream_from_zval_no_verify(fp, value);

			if (!fp) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Iterator %v returned an invalid stream handle", ce->name);
				return ZEND_HASH_APPLY_STOP;
			}

			if (iter->funcs->get_current_key) {
				zval key;
				iter->funcs->get_current_key(iter, &key TSRMLS_CC);

				if (EG(exception)) {
					return ZEND_HASH_APPLY_STOP;
				}

				if (Z_TYPE(key) != IS_STRING) {
					zval_dtor(&key);
					zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Iterator %v returned an invalid key (must return a string)", ce->name);
					return ZEND_HASH_APPLY_STOP;
				}

				str_key_len = Z_STRLEN(key);
				str_key = estrndup(Z_STRVAL(key), str_key_len);

				save = str_key;
				zval_dtor(&key);
			} else {
				zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Iterator %v returned an invalid key (must return a string)", ce->name);
				return ZEND_HASH_APPLY_STOP;
			}

			close_fp = 0;
			opened = (char *) estrndup(str, sizeof("[stream]") + 1);
			goto after_open_fp;
		case IS_OBJECT:
			if (instanceof_function(Z_OBJCE_PP(value), spl_ce_SplFileInfo TSRMLS_CC)) {
				char *test = NULL;
				zval dummy;
				spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(*value TSRMLS_CC);

				if (!base_len) {
					zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Iterator %v returns an SplFileInfo object, so base directory must be specified", ce->name);
					return ZEND_HASH_APPLY_STOP;
				}

				switch (intern->type) {
					case SPL_FS_DIR:
						test = spl_filesystem_object_get_path(intern, NULL TSRMLS_CC);
						fname_len = spprintf(&fname, 0, "%s%c%s", test, DEFAULT_SLASH, intern->u.dir.entry.d_name);
						php_stat(fname, fname_len, FS_IS_DIR, &dummy TSRMLS_CC);

						if (Z_BVAL(dummy)) {
							/* ignore directories */
							efree(fname);
							return ZEND_HASH_APPLY_KEEP;
						}

						test = expand_filepath(fname, NULL TSRMLS_CC);
						efree(fname);

						if (test) {
							fname = test;
							fname_len = strlen(fname);
						} else {
							zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Could not resolve file path");
							return ZEND_HASH_APPLY_STOP;
						}

						save = fname;
						goto phar_spl_fileinfo;
					case SPL_FS_INFO:
					case SPL_FS_FILE:
						fname = expand_filepath(intern->file_name, NULL TSRMLS_CC);
						if (!fname) {
							zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Could not resolve file path");
							return ZEND_HASH_APPLY_STOP;
						}

						fname_len = strlen(fname);
						save = fname;
						goto phar_spl_fileinfo;
				}
			}
			/* fall-through */
		default:
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Iterator %v returned an invalid value (must return a string)", ce->name);
			return ZEND_HASH_APPLY_STOP;
	}

	fname = Z_STRVAL_PP(value);
	fname_len = Z_STRLEN_PP(value);

phar_spl_fileinfo:
	if (base_len) {
		temp = expand_filepath(base, NULL TSRMLS_CC);
		if (!temp) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Could not resolve file path");
			if (save) {
				efree(save);
			}
			return ZEND_HASH_APPLY_STOP;
		}
		
		base = temp;
		base_len = strlen(base);

		if (strstr(fname, base)) {
			str_key_len = fname_len - base_len;

			if (str_key_len <= 0) {
				if (save) {
					efree(save);
					efree(temp);
				}
				return ZEND_HASH_APPLY_KEEP;
			}

			str_key = fname + base_len;

			if (*str_key == '/' || *str_key == '\\') {
				str_key++;
				str_key_len--;
			}

		} else {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Iterator %v returned a path \"%s\" that is not in the base directory \"%s\"", ce->name, fname, base);

			if (save) {
				efree(save);
				efree(temp);
			}

			return ZEND_HASH_APPLY_STOP;
		}
	} else {
		if (iter->funcs->get_current_key) {
			zval key;
			iter->funcs->get_current_key(iter, &key TSRMLS_CC);

			if (EG(exception)) {
				return ZEND_HASH_APPLY_STOP;
			}

			if (Z_TYPE(key) != IS_STRING) {
				zval_dtor(&key);
				zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Iterator %v returned an invalid key (must return a string)", ce->name);
				return ZEND_HASH_APPLY_STOP;
			}

			str_key_len = Z_STRLEN(key);
			str_key = estrndup(Z_STRVAL(key), str_key_len);

			save = str_key;
			zval_dtor(&key);
		} else {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Iterator %v returned an invalid key (must return a string)", ce->name);
			return ZEND_HASH_APPLY_STOP;
		}
	}
#if PHP_API_VERSION < 20100412
	if (PG(safe_mode) && (!php_checkuid(fname, NULL, CHECKUID_ALLOW_ONLY_FILE))) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Iterator %v returned a path \"%s\" that safe mode prevents opening", ce->name, fname);

		if (save) {
			efree(save);
		}

		if (temp) {
			efree(temp);
		}

		return ZEND_HASH_APPLY_STOP;
	}
#endif

	if (php_check_open_basedir(fname TSRMLS_CC)) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Iterator %v returned a path \"%s\" that open_basedir prevents opening", ce->name, fname);

		if (save) {
			efree(save);
		}

		if (temp) {
			efree(temp);
		}

		return ZEND_HASH_APPLY_STOP;
	}

	/* try to open source file, then create internal phar file and copy contents */
	fp = php_stream_open_wrapper(fname, "rb", STREAM_MUST_SEEK|0, &opened);

	if (!fp) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Iterator %v returned a file that could not be opened \"%s\"", ce->name, fname);

		if (save) {
			efree(save);
		}

		if (temp) {
			efree(temp);
		}

		return ZEND_HASH_APPLY_STOP;
	}
after_open_fp:
	if (str_key_len >= sizeof(".phar")-1 && !memcmp(str_key, ".phar", sizeof(".phar")-1)) {
		/* silently skip any files that would be added to the magic .phar directory */
		if (save) {
			efree(save);
		}

		if (temp) {
			efree(temp);
		}

		if (opened) {
			efree(opened);
		}

		if (close_fp) {
			php_stream_close(fp);
		}

		return ZEND_HASH_APPLY_KEEP;
	}

	if (!(data = phar_get_or_create_entry_data(phar_obj->arc.archive->fname, phar_obj->arc.archive->fname_len, str_key, str_key_len, "w+b", 0, &error, 1 TSRMLS_CC))) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Entry %s cannot be created: %s", str_key, error);
		efree(error);

		if (save) {
			efree(save);
		}

		if (opened) {
			efree(opened);
		}

		if (temp) {
			efree(temp);
		}

		if (close_fp) {
			php_stream_close(fp);
		}

		return ZEND_HASH_APPLY_STOP;

	} else {
		if (error) {
			efree(error);
		}
		/* convert to PHAR_UFP */
		if (data->internal_file->fp_type == PHAR_MOD) {
			php_stream_close(data->internal_file->fp);
		}

		data->internal_file->fp = NULL;
		data->internal_file->fp_type = PHAR_UFP;
		data->internal_file->offset_abs = data->internal_file->offset = php_stream_tell(p_obj->fp);
		data->fp = NULL;
		phar_stream_copy_to_stream(fp, p_obj->fp, PHP_STREAM_COPY_ALL, &contents_len);
		data->internal_file->uncompressed_filesize = data->internal_file->compressed_filesize =
			php_stream_tell(p_obj->fp) - data->internal_file->offset;
	}

	if (close_fp) {
		php_stream_close(fp);
	}

	add_assoc_string(p_obj->ret, str_key, opened, 0);

	if (save) {
		efree(save);
	}

	if (temp) {
		efree(temp);
	}

	data->internal_file->compressed_filesize = data->internal_file->uncompressed_filesize = contents_len;
	phar_entry_delref(data TSRMLS_CC);

	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

/* {{{ proto array Phar::buildFromDirectory(string base_dir[, string regex])
 * Construct a phar archive from an existing directory, recursively.
 * Optional second parameter is a regular expression for filtering directory contents.
 * 
 * Return value is an array mapping phar index to actual files added.
 */
PHP_METHOD(Phar, buildFromDirectory)
{
	char *dir, *error, *regex = NULL;
	int dir_len, regex_len = 0;
	zend_bool apply_reg = 0;
	zval arg, arg2, *iter, *iteriter, *regexiter = NULL;
	struct _phar_t pass;

	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot write to archive - write operations restricted by INI setting");
		return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &dir, &dir_len, &regex, &regex_len) == FAILURE) {
		RETURN_FALSE;
	}

	MAKE_STD_ZVAL(iter);

	if (SUCCESS != object_init_ex(iter, spl_ce_RecursiveDirectoryIterator)) {
		zval_ptr_dtor(&iter);
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Unable to instantiate directory iterator for %s", phar_obj->arc.archive->fname);
		RETURN_FALSE;
	}

	INIT_PZVAL(&arg);
	ZVAL_STRINGL(&arg, dir, dir_len, 0);
	INIT_PZVAL(&arg2);
	ZVAL_LONG(&arg2, SPL_FILE_DIR_SKIPDOTS|SPL_FILE_DIR_UNIXPATHS);

	zend_call_method_with_2_params(&iter, spl_ce_RecursiveDirectoryIterator, 
			&spl_ce_RecursiveDirectoryIterator->constructor, "__construct", NULL, &arg, &arg2);

	if (EG(exception)) {
		zval_ptr_dtor(&iter);
		RETURN_FALSE;
	}

	MAKE_STD_ZVAL(iteriter);

	if (SUCCESS != object_init_ex(iteriter, spl_ce_RecursiveIteratorIterator)) {
		zval_ptr_dtor(&iter);
		zval_ptr_dtor(&iteriter);
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Unable to instantiate directory iterator for %s", phar_obj->arc.archive->fname);
		RETURN_FALSE;
	}

	zend_call_method_with_1_params(&iteriter, spl_ce_RecursiveIteratorIterator, 
			&spl_ce_RecursiveIteratorIterator->constructor, "__construct", NULL, iter);

	if (EG(exception)) {
		zval_ptr_dtor(&iter);
		zval_ptr_dtor(&iteriter);
		RETURN_FALSE;
	}

	zval_ptr_dtor(&iter);

	if (regex_len > 0) {
		apply_reg = 1;
		MAKE_STD_ZVAL(regexiter);

		if (SUCCESS != object_init_ex(regexiter, spl_ce_RegexIterator)) {
			zval_ptr_dtor(&iteriter);
			zval_dtor(regexiter);
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Unable to instantiate regex iterator for %s", phar_obj->arc.archive->fname);
			RETURN_FALSE;
		}

		INIT_PZVAL(&arg2);
		ZVAL_STRINGL(&arg2, regex, regex_len, 0);

		zend_call_method_with_2_params(&regexiter, spl_ce_RegexIterator, 
			&spl_ce_RegexIterator->constructor, "__construct", NULL, iteriter, &arg2);
	}

	array_init(return_value);

	pass.c = apply_reg ? Z_OBJCE_P(regexiter) : Z_OBJCE_P(iteriter);
	pass.p = phar_obj;
	pass.b = dir;
	pass.l = dir_len;
	pass.count = 0;
	pass.ret = return_value;
	pass.fp = php_stream_fopen_tmpfile();
	if (pass.fp == NULL) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" unable to create temporary file", phar_obj->arc.archive->fname);
		return;
	}

	if (phar_obj->arc.archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->arc.archive) TSRMLS_CC)) {
		zval_ptr_dtor(&iteriter);
		if (apply_reg) {
			zval_ptr_dtor(&regexiter);
		}
		php_stream_close(pass.fp);
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar_obj->arc.archive->fname);
		return;
	}

	if (SUCCESS == spl_iterator_apply((apply_reg ? regexiter : iteriter), (spl_iterator_apply_func_t) phar_build, (void *) &pass TSRMLS_CC)) {
		zval_ptr_dtor(&iteriter);

		if (apply_reg) {
			zval_ptr_dtor(&regexiter);
		}

		phar_obj->arc.archive->ufp = pass.fp;
		phar_flush(phar_obj->arc.archive, 0, 0, 0, &error TSRMLS_CC);

		if (error) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
			efree(error);
		}

	} else {
		zval_ptr_dtor(&iteriter);
		if (apply_reg) {
			zval_ptr_dtor(&regexiter);
		}
		php_stream_close(pass.fp);
	}
}
/* }}} */

/* {{{ proto array Phar::buildFromIterator(Iterator iter[, string base_directory])
 * Construct a phar archive from an iterator.  The iterator must return a series of strings
 * that are full paths to files that should be added to the phar.  The iterator key should
 * be the path that the file will have within the phar archive.
 *
 * If base directory is specified, then the key will be ignored, and instead the portion of
 * the current value minus the base directory will be used
 *
 * Returned is an array mapping phar index to actual file added
 */
PHP_METHOD(Phar, buildFromIterator)
{
	zval *obj;
	char *error;
	uint base_len = 0;
	char *base = NULL;
	struct _phar_t pass;

	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot write out phar archive, phar is read-only");
		return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O|s", &obj, zend_ce_traversable, &base, &base_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (phar_obj->arc.archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->arc.archive) TSRMLS_CC)) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar_obj->arc.archive->fname);
		return;
	}

	array_init(return_value);

	pass.c = Z_OBJCE_P(obj);
	pass.p = phar_obj;
	pass.b = base;
	pass.l = base_len;
	pass.ret = return_value;
	pass.count = 0;
	pass.fp = php_stream_fopen_tmpfile();
	if (pass.fp == NULL) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\": unable to create temporary file", phar_obj->arc.archive->fname);
		return;
	}

	if (SUCCESS == spl_iterator_apply(obj, (spl_iterator_apply_func_t) phar_build, (void *) &pass TSRMLS_CC)) {
		phar_obj->arc.archive->ufp = pass.fp;
		phar_flush(phar_obj->arc.archive, 0, 0, 0, &error TSRMLS_CC);
		if (error) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
			efree(error);
		}
	} else {
		php_stream_close(pass.fp);
	}
}
/* }}} */

/* {{{ proto int Phar::count()
 * Returns the number of entries in the Phar archive
 */
PHP_METHOD(Phar, count)
{
	PHAR_ARCHIVE_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(zend_hash_num_elements(&phar_obj->arc.archive->manifest));
}
/* }}} */

/* {{{ proto bool Phar::isFileFormat(int format)
 * Returns true if the phar archive is based on the tar/zip/phar file format depending
 * on whether Phar::TAR, Phar::ZIP or Phar::PHAR was passed in
 */
PHP_METHOD(Phar, isFileFormat)
{
	long type;
	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &type) == FAILURE) {
		RETURN_FALSE;
	}

	switch (type) {
		case PHAR_FORMAT_TAR:
			RETURN_BOOL(phar_obj->arc.archive->is_tar);
		case PHAR_FORMAT_ZIP:
			RETURN_BOOL(phar_obj->arc.archive->is_zip);
		case PHAR_FORMAT_PHAR:
			RETURN_BOOL(!phar_obj->arc.archive->is_tar && !phar_obj->arc.archive->is_zip);
		default:
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "Unknown file format specified");
	}
}
/* }}} */

static int phar_copy_file_contents(phar_entry_info *entry, php_stream *fp TSRMLS_DC) /* {{{ */
{
	char *error;
	off_t offset;
	phar_entry_info *link;

	if (FAILURE == phar_open_entry_fp(entry, &error, 1 TSRMLS_CC)) {
		if (error) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"Cannot convert phar archive \"%s\", unable to open entry \"%s\" contents: %s", entry->phar->fname, entry->filename, error);
			efree(error);
		} else {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"Cannot convert phar archive \"%s\", unable to open entry \"%s\" contents", entry->phar->fname, entry->filename);
		}
		return FAILURE;
	}

	/* copy old contents in entirety */
	phar_seek_efp(entry, 0, SEEK_SET, 0, 1 TSRMLS_CC);
	offset = php_stream_tell(fp);
	link = phar_get_link_source(entry TSRMLS_CC);

	if (!link) {
		link = entry;
	}

	if (SUCCESS != phar_stream_copy_to_stream(phar_get_efp(link, 0 TSRMLS_CC), fp, link->uncompressed_filesize, NULL)) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot convert phar archive \"%s\", unable to copy entry \"%s\" contents", entry->phar->fname, entry->filename);
		return FAILURE;
	}

	if (entry->fp_type == PHAR_MOD) {
		/* save for potential restore on error */
		entry->cfp = entry->fp;
		entry->fp = NULL;
	}

	/* set new location of file contents */
	entry->fp_type = PHAR_FP;
	entry->offset = offset;
	return SUCCESS;
}
/* }}} */

static zval *phar_rename_archive(phar_archive_data *phar, char *ext, zend_bool compress TSRMLS_DC) /* {{{ */
{
	const char *oldname = NULL;
	char *oldpath = NULL;
	char *basename = NULL, *basepath = NULL;
	char *newname = NULL, *newpath = NULL;
	zval *ret, arg1;
	zend_class_entry *ce;
	char *error;
	const char *pcr_error;
	int ext_len = ext ? strlen(ext) : 0;
	int oldname_len;
	phar_archive_data **pphar = NULL;
	php_stream_statbuf ssb;

	if (!ext) {
		if (phar->is_zip) {

			if (phar->is_data) {
				ext = "zip";
			} else {
				ext = "phar.zip";
			}

		} else if (phar->is_tar) {

			switch (phar->flags) {
				case PHAR_FILE_COMPRESSED_GZ:
					if (phar->is_data) {
						ext = "tar.gz";
					} else {
						ext = "phar.tar.gz";
					}
					break;
				case PHAR_FILE_COMPRESSED_BZ2:
					if (phar->is_data) {
						ext = "tar.bz2";
					} else {
						ext = "phar.tar.bz2";
					}
					break;
				default:
					if (phar->is_data) {
						ext = "tar";
					} else {
						ext = "phar.tar";
					}
			}
		} else {

			switch (phar->flags) {
				case PHAR_FILE_COMPRESSED_GZ:
					ext = "phar.gz";
					break;
				case PHAR_FILE_COMPRESSED_BZ2:
					ext = "phar.bz2";
					break;
				default:
					ext = "phar";
			}
		}
	} else if (phar_path_check(&ext, &ext_len, &pcr_error) > pcr_is_ok) {

		if (phar->is_data) {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "data phar converted from \"%s\" has invalid extension %s", phar->fname, ext);
		} else {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "phar converted from \"%s\" has invalid extension %s", phar->fname, ext);
		}
		return NULL;
	}

	if (ext[0] == '.') {
		++ext;
	}

	oldpath = estrndup(phar->fname, phar->fname_len);
	oldname = zend_memrchr(phar->fname, '/', phar->fname_len);
	++oldname;
	oldname_len = strlen(oldname);

	basename = estrndup(oldname, oldname_len);
	spprintf(&newname, 0, "%s.%s", strtok(basename, "."), ext);
	efree(basename);

	

	basepath = estrndup(oldpath, (strlen(oldpath) - oldname_len));
	phar->fname_len = spprintf(&newpath, 0, "%s%s", basepath, newname);
	phar->fname = newpath;
	phar->ext = newpath + phar->fname_len - strlen(ext) - 1;
	efree(basepath);
	efree(newname);

	if (PHAR_G(manifest_cached) && SUCCESS == zend_hash_find(&cached_phars, newpath, phar->fname_len, (void **) &pphar)) {
		efree(oldpath);
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Unable to add newly converted phar \"%s\" to the list of phars, new phar name is in phar.cache_list", phar->fname);
		return NULL;
	}

	if (SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_fname_map), newpath, phar->fname_len, (void **) &pphar)) {
		if ((*pphar)->fname_len == phar->fname_len && !memcmp((*pphar)->fname, phar->fname, phar->fname_len)) {
			if (!zend_hash_num_elements(&phar->manifest)) {
				(*pphar)->is_tar = phar->is_tar;
				(*pphar)->is_zip = phar->is_zip;
				(*pphar)->is_data = phar->is_data;
				(*pphar)->flags = phar->flags;
				(*pphar)->fp = phar->fp;
				phar->fp = NULL;
				phar_destroy_phar_data(phar TSRMLS_CC);
				phar = *pphar;
				phar->refcount++;
				newpath = oldpath;
				goto its_ok;
			}
		}

		efree(oldpath);
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Unable to add newly converted phar \"%s\" to the list of phars, a phar with that name already exists", phar->fname);
		return NULL;
	}
its_ok:
	if (SUCCESS == php_stream_stat_path(newpath, &ssb)) {
		efree(oldpath);
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "phar \"%s\" exists and must be unlinked prior to conversion", newpath);
		return NULL;
	}
	if (!phar->is_data) {
		if (SUCCESS != phar_detect_phar_fname_ext(newpath, phar->fname_len, (const char **) &(phar->ext), &(phar->ext_len), 1, 1, 1 TSRMLS_CC)) {
			efree(oldpath);
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "phar \"%s\" has invalid extension %s", phar->fname, ext);
			return NULL;
		}

		if (phar->alias) {
			if (phar->is_temporary_alias) {
				phar->alias = NULL;
				phar->alias_len = 0;
			} else {
				phar->alias = estrndup(newpath, strlen(newpath));
				phar->alias_len = strlen(newpath);
				phar->is_temporary_alias = 1;
				zend_hash_update(&(PHAR_GLOBALS->phar_alias_map), newpath, phar->fname_len, (void*)&phar, sizeof(phar_archive_data*), NULL);
			}
		}

	} else {

		if (SUCCESS != phar_detect_phar_fname_ext(newpath, phar->fname_len, (const char **) &(phar->ext), &(phar->ext_len), 0, 1, 1 TSRMLS_CC)) {
			efree(oldpath);
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "data phar \"%s\" has invalid extension %s", phar->fname, ext);
			return NULL;
		}

		phar->alias = NULL;
		phar->alias_len = 0;
	}

	if ((!pphar || phar == *pphar) && SUCCESS != zend_hash_update(&(PHAR_GLOBALS->phar_fname_map), newpath, phar->fname_len, (void*)&phar, sizeof(phar_archive_data*), NULL)) {
		efree(oldpath);
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Unable to add newly converted phar \"%s\" to the list of phars", phar->fname);
		return NULL;
	}

	phar_flush(phar, 0, 0, 1, &error TSRMLS_CC);

	if (error) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "%s", error);
		efree(error);
		efree(oldpath);
		return NULL;
	}

	efree(oldpath);

	if (phar->is_data) {
		ce = phar_ce_data;
	} else {
		ce = phar_ce_archive;
	}

	MAKE_STD_ZVAL(ret);

	if (SUCCESS != object_init_ex(ret, ce)) {
		zval_dtor(ret);
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Unable to instantiate phar object when converting archive \"%s\"", phar->fname);
		return NULL;
	}

	INIT_PZVAL(&arg1);
	ZVAL_STRINGL(&arg1, phar->fname, phar->fname_len, 0);

	zend_call_method_with_1_params(&ret, ce, &ce->constructor, "__construct", NULL, &arg1);
	return ret;
}
/* }}} */

static zval *phar_convert_to_other(phar_archive_data *source, int convert, char *ext, php_uint32 flags TSRMLS_DC) /* {{{ */
{
	phar_archive_data *phar;
	phar_entry_info *entry, newentry;
	zval *ret;

	/* invalidate phar cache */
	PHAR_G(last_phar) = NULL;
	PHAR_G(last_phar_name) = PHAR_G(last_alias) = NULL;

	phar = (phar_archive_data *) ecalloc(1, sizeof(phar_archive_data));
	/* set whole-archive compression and type from parameter */
	phar->flags = flags;
	phar->is_data = source->is_data;

	switch (convert) {
		case PHAR_FORMAT_TAR:
			phar->is_tar = 1;
			break;
		case PHAR_FORMAT_ZIP:
			phar->is_zip = 1;
			break;
		default:
			phar->is_data = 0;
			break;
	}

	zend_hash_init(&(phar->manifest), sizeof(phar_entry_info),
		zend_get_hash_value, destroy_phar_manifest_entry, 0);
	zend_hash_init(&phar->mounted_dirs, sizeof(char *),
		zend_get_hash_value, NULL, 0);
	zend_hash_init(&phar->virtual_dirs, sizeof(char *),
		zend_get_hash_value, NULL, 0);

	phar->fp = php_stream_fopen_tmpfile();
	if (phar->fp == NULL) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "unable to create temporary file");
		return NULL;
	}
	phar->fname = source->fname;
	phar->fname_len = source->fname_len;
	phar->is_temporary_alias = source->is_temporary_alias;
	phar->alias = source->alias;

	if (source->metadata) {
		zval *t;

		t = source->metadata;
		ALLOC_ZVAL(phar->metadata);
		*phar->metadata = *t;
		zval_copy_ctor(phar->metadata);
		Z_SET_REFCOUNT_P(phar->metadata, 1);

		phar->metadata_len = 0;
	}

	/* first copy each file's uncompressed contents to a temporary file and set per-file flags */
	for (zend_hash_internal_pointer_reset(&source->manifest); SUCCESS == zend_hash_has_more_elements(&source->manifest); zend_hash_move_forward(&source->manifest)) {

		if (FAILURE == zend_hash_get_current_data(&source->manifest, (void **) &entry)) {
			zend_hash_destroy(&(phar->manifest));
			php_stream_close(phar->fp);
			efree(phar);
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"Cannot convert phar archive \"%s\"", source->fname);
			return NULL;
		}

		newentry = *entry;

		if (newentry.link) {
			newentry.link = estrdup(newentry.link);
			goto no_copy;
		}

		if (newentry.tmp) {
			newentry.tmp = estrdup(newentry.tmp);
			goto no_copy;
		}

		newentry.metadata_str.c = 0;

		if (FAILURE == phar_copy_file_contents(&newentry, phar->fp TSRMLS_CC)) {
			zend_hash_destroy(&(phar->manifest));
			php_stream_close(phar->fp);
			efree(phar);
			/* exception already thrown */
			return NULL;
		}
no_copy:
		newentry.filename = estrndup(newentry.filename, newentry.filename_len);

		if (newentry.metadata) {
			zval *t;

			t = newentry.metadata;
			ALLOC_ZVAL(newentry.metadata);
			*newentry.metadata = *t;
			zval_copy_ctor(newentry.metadata);
			Z_SET_REFCOUNT_P(newentry.metadata, 1);

			newentry.metadata_str.c = NULL;
			newentry.metadata_str.len = 0;
		}

		newentry.is_zip = phar->is_zip;
		newentry.is_tar = phar->is_tar;

		if (newentry.is_tar) {
			newentry.tar_type = (entry->is_dir ? TAR_DIR : TAR_FILE);
		}

		newentry.is_modified = 1;
		newentry.phar = phar;
		newentry.old_flags = newentry.flags & ~PHAR_ENT_COMPRESSION_MASK; /* remove compression from old_flags */
		phar_set_inode(&newentry TSRMLS_CC);
		zend_hash_add(&(phar->manifest), newentry.filename, newentry.filename_len, (void*)&newentry, sizeof(phar_entry_info), NULL);
		phar_add_virtual_dirs(phar, newentry.filename, newentry.filename_len TSRMLS_CC);
	}

	if ((ret = phar_rename_archive(phar, ext, 0 TSRMLS_CC))) {
		return ret;
	} else {
		zend_hash_destroy(&(phar->manifest));
		zend_hash_destroy(&(phar->mounted_dirs));
		zend_hash_destroy(&(phar->virtual_dirs));
		php_stream_close(phar->fp);
		efree(phar->fname);
		efree(phar);
		return NULL;
	}
}
/* }}} */

/* {{{ proto object Phar::convertToExecutable([int format[, int compression [, string file_ext]]])
 * Convert a phar.tar or phar.zip archive to the phar file format. The
 * optional parameter allows the user to determine the new
 * filename extension (default is phar).
 */
PHP_METHOD(Phar, convertToExecutable)
{
	char *ext = NULL;
	int is_data, ext_len = 0;
	php_uint32 flags;
	zval *ret;
	/* a number that is not 0, 1 or 2 (Which is also Greg's birthday, so there) */
	long format = 9021976, method = 9021976;
	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|lls", &format, &method, &ext, &ext_len) == FAILURE) {
		return;
	}

	if (PHAR_G(readonly)) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot write out executable phar archive, phar is read-only");
		return;
	}

	switch (format) {
		case 9021976:
		case PHAR_FORMAT_SAME: /* null is converted to 0 */
			/* by default, use the existing format */
			if (phar_obj->arc.archive->is_tar) {
				format = PHAR_FORMAT_TAR;
			} else if (phar_obj->arc.archive->is_zip) {
				format = PHAR_FORMAT_ZIP;
			} else {
				format = PHAR_FORMAT_PHAR;
			}
			break;
		case PHAR_FORMAT_PHAR:
		case PHAR_FORMAT_TAR:
		case PHAR_FORMAT_ZIP:
			break;
		default:
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
				"Unknown file format specified, please pass one of Phar::PHAR, Phar::TAR or Phar::ZIP");
			return;
	}

	switch (method) {
		case 9021976:
			flags = phar_obj->arc.archive->flags & PHAR_FILE_COMPRESSION_MASK;
			break;
		case 0:
			flags = PHAR_FILE_COMPRESSED_NONE;
			break;
		case PHAR_ENT_COMPRESSED_GZ:
			if (format == PHAR_FORMAT_ZIP) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
					"Cannot compress entire archive with gzip, zip archives do not support whole-archive compression");
				return;
			}

			if (!PHAR_G(has_zlib)) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
					"Cannot compress entire archive with gzip, enable ext/zlib in php.ini");
				return;
			}

			flags = PHAR_FILE_COMPRESSED_GZ;
			break;
		case PHAR_ENT_COMPRESSED_BZ2:
			if (format == PHAR_FORMAT_ZIP) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
					"Cannot compress entire archive with bz2, zip archives do not support whole-archive compression");
				return;
			}

			if (!PHAR_G(has_bz2)) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
					"Cannot compress entire archive with bz2, enable ext/bz2 in php.ini");
				return;
			}

			flags = PHAR_FILE_COMPRESSED_BZ2;
			break;
		default:
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
				"Unknown compression specified, please pass one of Phar::GZ or Phar::BZ2");
			return;
	}

	is_data = phar_obj->arc.archive->is_data;
	phar_obj->arc.archive->is_data = 0;
	ret = phar_convert_to_other(phar_obj->arc.archive, format, ext, flags TSRMLS_CC);
	phar_obj->arc.archive->is_data = is_data;

	if (ret) {
		RETURN_ZVAL(ret, 1, 1);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto object Phar::convertToData([int format[, int compression [, string file_ext]]])
 * Convert an archive to a non-executable .tar or .zip.
 * The optional parameter allows the user to determine the new
 * filename extension (default is .zip or .tar).
 */
PHP_METHOD(Phar, convertToData)
{
	char *ext = NULL;
	int is_data, ext_len = 0;
	php_uint32 flags;
	zval *ret;
	/* a number that is not 0, 1 or 2 (Which is also Greg's birthday so there) */
	long format = 9021976, method = 9021976;
	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|lls", &format, &method, &ext, &ext_len) == FAILURE) {
		return;
	}

	switch (format) {
		case 9021976:
		case PHAR_FORMAT_SAME: /* null is converted to 0 */
			/* by default, use the existing format */
			if (phar_obj->arc.archive->is_tar) {
				format = PHAR_FORMAT_TAR;
			} else if (phar_obj->arc.archive->is_zip) {
				format = PHAR_FORMAT_ZIP;
			} else {
				zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
					"Cannot write out data phar archive, use Phar::TAR or Phar::ZIP");
				return;
			}
			break;
		case PHAR_FORMAT_PHAR:
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"Cannot write out data phar archive, use Phar::TAR or Phar::ZIP");
			return;
		case PHAR_FORMAT_TAR:
		case PHAR_FORMAT_ZIP:
			break;
		default:
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
				"Unknown file format specified, please pass one of Phar::TAR or Phar::ZIP");
			return;
	}

	switch (method) {
		case 9021976:
			flags = phar_obj->arc.archive->flags & PHAR_FILE_COMPRESSION_MASK;
			break;
		case 0:
			flags = PHAR_FILE_COMPRESSED_NONE;
			break;
		case PHAR_ENT_COMPRESSED_GZ:
			if (format == PHAR_FORMAT_ZIP) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
					"Cannot compress entire archive with gzip, zip archives do not support whole-archive compression");
				return;
			}

			if (!PHAR_G(has_zlib)) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
					"Cannot compress entire archive with gzip, enable ext/zlib in php.ini");
				return;
			}

			flags = PHAR_FILE_COMPRESSED_GZ;
			break;
		case PHAR_ENT_COMPRESSED_BZ2:
			if (format == PHAR_FORMAT_ZIP) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
					"Cannot compress entire archive with bz2, zip archives do not support whole-archive compression");
				return;
			}

			if (!PHAR_G(has_bz2)) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
					"Cannot compress entire archive with bz2, enable ext/bz2 in php.ini");
				return;
			}

			flags = PHAR_FILE_COMPRESSED_BZ2;
			break;
		default:
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
				"Unknown compression specified, please pass one of Phar::GZ or Phar::BZ2");
			return;
	}

	is_data = phar_obj->arc.archive->is_data;
	phar_obj->arc.archive->is_data = 1;
	ret = phar_convert_to_other(phar_obj->arc.archive, format, ext, flags TSRMLS_CC);
	phar_obj->arc.archive->is_data = is_data;

	if (ret) {
		RETURN_ZVAL(ret, 1, 1);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto int|false Phar::isCompressed()
 * Returns Phar::GZ or PHAR::BZ2 if the entire archive is compressed
 * (.tar.gz/tar.bz2 and so on), or FALSE otherwise.
 */
PHP_METHOD(Phar, isCompressed)
{
	PHAR_ARCHIVE_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (phar_obj->arc.archive->flags & PHAR_FILE_COMPRESSED_GZ) {
		RETURN_LONG(PHAR_ENT_COMPRESSED_GZ);
	}

	if (phar_obj->arc.archive->flags & PHAR_FILE_COMPRESSED_BZ2) {
		RETURN_LONG(PHAR_ENT_COMPRESSED_BZ2);
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool Phar::isWritable()
 * Returns true if phar.readonly=0 or phar is a PharData AND the actual file is writable.
 */
PHP_METHOD(Phar, isWritable)
{
	php_stream_statbuf ssb;
	PHAR_ARCHIVE_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (!phar_obj->arc.archive->is_writeable) {
		RETURN_FALSE;
	}

	if (SUCCESS != php_stream_stat_path(phar_obj->arc.archive->fname, &ssb)) {
		if (phar_obj->arc.archive->is_brandnew) {
			/* assume it works if the file doesn't exist yet */
			RETURN_TRUE;
		}
		RETURN_FALSE;
	}

	RETURN_BOOL((ssb.sb.st_mode & (S_IWOTH | S_IWGRP | S_IWUSR)) != 0);
}
/* }}} */

/* {{{ proto bool Phar::delete(string entry)
 * Deletes a named file within the archive.
 */
PHP_METHOD(Phar, delete)
{
	char *fname;
	int fname_len;
	char *error;
	phar_entry_info *entry;
	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot write out phar archive, phar is read-only");
		return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &fname, &fname_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (phar_obj->arc.archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->arc.archive) TSRMLS_CC)) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar_obj->arc.archive->fname);
		return;
	}
	if (zend_hash_exists(&phar_obj->arc.archive->manifest, fname, (uint) fname_len)) {
		if (SUCCESS == zend_hash_find(&phar_obj->arc.archive->manifest, fname, (uint) fname_len, (void**)&entry)) {
			if (entry->is_deleted) {
				/* entry is deleted, but has not been flushed to disk yet */
				RETURN_TRUE;
			} else {
				entry->is_deleted = 1;
				entry->is_modified = 1;
				phar_obj->arc.archive->is_modified = 1;
			}
		}
	} else {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Entry %s does not exist and cannot be deleted", fname);
		RETURN_FALSE;
	}

	phar_flush(phar_obj->arc.archive, NULL, 0, 0, &error TSRMLS_CC);
	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
		efree(error);
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int Phar::getAlias()
 * Returns the alias for the Phar or NULL.
 */
PHP_METHOD(Phar, getAlias)
{
	PHAR_ARCHIVE_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (phar_obj->arc.archive->alias && phar_obj->arc.archive->alias != phar_obj->arc.archive->fname) {
		RETURN_STRINGL(phar_obj->arc.archive->alias, phar_obj->arc.archive->alias_len, 1);
	}
}
/* }}} */

/* {{{ proto int Phar::getPath()
 * Returns the real path to the phar archive on disk
 */
PHP_METHOD(Phar, getPath)
{
	PHAR_ARCHIVE_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_STRINGL(phar_obj->arc.archive->fname, phar_obj->arc.archive->fname_len, 1);
}
/* }}} */

/* {{{ proto bool Phar::setAlias(string alias)
 * Sets the alias for a Phar archive. The default value is the full path
 * to the archive.
 */
PHP_METHOD(Phar, setAlias)
{
	char *alias, *error, *oldalias;
	phar_archive_data **fd_ptr;
	int alias_len, oldalias_len, old_temp, readd = 0;

	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot write out phar archive, phar is read-only");
		RETURN_FALSE;
	}

	/* invalidate phar cache */
	PHAR_G(last_phar) = NULL;
	PHAR_G(last_phar_name) = PHAR_G(last_alias) = NULL;

	if (phar_obj->arc.archive->is_data) {
		if (phar_obj->arc.archive->is_tar) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"A Phar alias cannot be set in a plain tar archive");
		} else {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"A Phar alias cannot be set in a plain zip archive");
		}
		RETURN_FALSE;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &alias, &alias_len) == SUCCESS) {
		if (alias_len == phar_obj->arc.archive->alias_len && memcmp(phar_obj->arc.archive->alias, alias, alias_len) == 0) {
			RETURN_TRUE;
		}
		if (alias_len && SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len, (void**)&fd_ptr)) {
			spprintf(&error, 0, "alias \"%s\" is already used for archive \"%s\" and cannot be used for other archives", alias, (*fd_ptr)->fname);
			if (SUCCESS == phar_free_alias(*fd_ptr, alias, alias_len TSRMLS_CC)) {
				efree(error);
				goto valid_alias;
			}
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
			efree(error);
			RETURN_FALSE;
		}
		if (!phar_validate_alias(alias, alias_len)) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"Invalid alias \"%s\" specified for phar \"%s\"", alias, phar_obj->arc.archive->fname);
			RETURN_FALSE;
		}
valid_alias:
		if (phar_obj->arc.archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->arc.archive) TSRMLS_CC)) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar_obj->arc.archive->fname);
			return;
		}
		if (phar_obj->arc.archive->alias_len && SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_alias_map), phar_obj->arc.archive->alias, phar_obj->arc.archive->alias_len, (void**)&fd_ptr)) {
			zend_hash_del(&(PHAR_GLOBALS->phar_alias_map), phar_obj->arc.archive->alias, phar_obj->arc.archive->alias_len);
			readd = 1;
		}

		oldalias = phar_obj->arc.archive->alias;
		oldalias_len = phar_obj->arc.archive->alias_len;
		old_temp = phar_obj->arc.archive->is_temporary_alias;

		if (alias_len) {
			phar_obj->arc.archive->alias = estrndup(alias, alias_len);
		} else {
			phar_obj->arc.archive->alias = NULL;
		}

		phar_obj->arc.archive->alias_len = alias_len;
		phar_obj->arc.archive->is_temporary_alias = 0;
		phar_flush(phar_obj->arc.archive, NULL, 0, 0, &error TSRMLS_CC);

		if (error) {
			phar_obj->arc.archive->alias = oldalias;
			phar_obj->arc.archive->alias_len = oldalias_len;
			phar_obj->arc.archive->is_temporary_alias = old_temp;
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
			if (readd) {
				zend_hash_add(&(PHAR_GLOBALS->phar_alias_map), oldalias, oldalias_len, (void*)&(phar_obj->arc.archive), sizeof(phar_archive_data*), NULL);
			}
			efree(error);
			RETURN_FALSE;
		}

		zend_hash_add(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len, (void*)&(phar_obj->arc.archive), sizeof(phar_archive_data*), NULL);

		if (oldalias) {
			efree(oldalias);
		}

		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string Phar::getVersion()
 * Return version info of Phar archive
 */
PHP_METHOD(Phar, getVersion)
{
	PHAR_ARCHIVE_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_STRING(phar_obj->arc.archive->version, 1);
}
/* }}} */

/* {{{ proto void Phar::startBuffering()
 * Do not flush a writeable phar (save its contents) until explicitly requested
 */
PHP_METHOD(Phar, startBuffering)
{
	PHAR_ARCHIVE_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	phar_obj->arc.archive->donotflush = 1;
}
/* }}} */

/* {{{ proto bool Phar::isBuffering()
 * Returns whether write operations are flushing to disk immediately.
 */
PHP_METHOD(Phar, isBuffering)
{
	PHAR_ARCHIVE_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(phar_obj->arc.archive->donotflush);
}
/* }}} */

/* {{{ proto bool Phar::stopBuffering()
 * Saves the contents of a modified archive to disk.
 */
PHP_METHOD(Phar, stopBuffering)
{
	char *error;

	PHAR_ARCHIVE_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot write out phar archive, phar is read-only");
		return;
	}

	phar_obj->arc.archive->donotflush = 0;
	phar_flush(phar_obj->arc.archive, 0, 0, 0, &error TSRMLS_CC);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
		efree(error);
	}
}
/* }}} */

/* {{{ proto bool Phar::setStub(string|stream stub [, int len])
 * Change the stub in a phar, phar.tar or phar.zip archive to something other
 * than the default. The stub *must* end with a call to __HALT_COMPILER().
 */
PHP_METHOD(Phar, setStub)
{
	zval *zstub;
	char *stub, *error;
	int stub_len;
	long len = -1;
	php_stream *stream;
	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot change stub, phar is read-only");
		return;
	}

	if (phar_obj->arc.archive->is_data) {
		if (phar_obj->arc.archive->is_tar) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"A Phar stub cannot be set in a plain tar archive");
		} else {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"A Phar stub cannot be set in a plain zip archive");
		}
		return;
	}

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "r|l", &zstub, &len) == SUCCESS) {
		if ((php_stream_from_zval_no_verify(stream, &zstub)) != NULL) {
			if (len > 0) {
				len = -len;
			} else {
				len = -1;
			}
			if (phar_obj->arc.archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->arc.archive) TSRMLS_CC)) {
				zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar_obj->arc.archive->fname);
				return;
			}
			phar_flush(phar_obj->arc.archive, (char *) &zstub, len, 0, &error TSRMLS_CC);
			if (error) {
				zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
				efree(error);
			}
			RETURN_TRUE;
		} else {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"Cannot change stub, unable to read from input stream");
		}
	} else if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &stub, &stub_len) == SUCCESS) {
		if (phar_obj->arc.archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->arc.archive) TSRMLS_CC)) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar_obj->arc.archive->fname);
			return;
		}
		phar_flush(phar_obj->arc.archive, stub, stub_len, 0, &error TSRMLS_CC);

		if (error) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
			efree(error);
		}

		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool Phar::setDefaultStub([string index[, string webindex]])
 * In a pure phar archive, sets a stub that can be used to run the archive
 * regardless of whether the phar extension is available. The first parameter
 * is the CLI startup filename, which defaults to "index.php". The second
 * parameter is the web startup filename and also defaults to "index.php"
 * (falling back to CLI behaviour).
 * Both parameters are optional.
 * In a phar.zip or phar.tar archive, the default stub is used only to
 * identify the archive to the extension as a Phar object. This allows the
 * extension to treat phar.zip and phar.tar types as honorary phars. Since
 * files cannot be loaded via this kind of stub, no parameters are accepted
 * when the Phar object is zip- or tar-based.
 */
PHP_METHOD(Phar, setDefaultStub)
{
	char *index = NULL, *webindex = NULL, *error = NULL, *stub = NULL;
	int index_len = 0, webindex_len = 0, created_stub = 0;
	size_t stub_len = 0;
	PHAR_ARCHIVE_OBJECT();

	if (phar_obj->arc.archive->is_data) {
		if (phar_obj->arc.archive->is_tar) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"A Phar stub cannot be set in a plain tar archive");
		} else {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"A Phar stub cannot be set in a plain zip archive");
		}
		return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!s", &index, &index_len, &webindex, &webindex_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() > 0 && (phar_obj->arc.archive->is_tar || phar_obj->arc.archive->is_zip)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "method accepts no arguments for a tar- or zip-based phar stub, %d given", ZEND_NUM_ARGS());
		RETURN_FALSE;
	}

	if (PHAR_G(readonly)) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot change stub: phar.readonly=1");
		RETURN_FALSE;
	}

	if (!phar_obj->arc.archive->is_tar && !phar_obj->arc.archive->is_zip) {
		stub = phar_create_default_stub(index, webindex, &stub_len, &error TSRMLS_CC);

		if (error) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "%s", error);
			efree(error);
			if (stub) {
				efree(stub);
			}
			RETURN_FALSE;
		}

		created_stub = 1;
	}

	if (phar_obj->arc.archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->arc.archive) TSRMLS_CC)) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar_obj->arc.archive->fname);
		return;
	}
	phar_flush(phar_obj->arc.archive, stub, stub_len, 1, &error TSRMLS_CC);

	if (created_stub) {
		efree(stub);
	}

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
		efree(error);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto array Phar::setSignatureAlgorithm(int sigtype[, string privatekey])
 * Sets the signature algorithm for a phar and applies it. The signature
 * algorithm must be one of Phar::MD5, Phar::SHA1, Phar::SHA256,
 * Phar::SHA512, or Phar::OPENSSL. Note that zip- based phar archives
 * cannot support signatures.
 */
PHP_METHOD(Phar, setSignatureAlgorithm)
{
	long algo;
	char *error, *key = NULL;
	int key_len = 0;

	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot set signature algorithm, phar is read-only");
		return;
	}

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "l|s", &algo, &key, &key_len) != SUCCESS) {
		return;
	}

	switch (algo) {
		case PHAR_SIG_SHA256:
		case PHAR_SIG_SHA512:
#ifndef PHAR_HASH_OK
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"SHA-256 and SHA-512 signatures are only supported if the hash extension is enabled and built non-shared");
			return;
#endif
		case PHAR_SIG_MD5:
		case PHAR_SIG_SHA1:
		case PHAR_SIG_OPENSSL:
			if (phar_obj->arc.archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->arc.archive) TSRMLS_CC)) {
				zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar_obj->arc.archive->fname);
				return;
			}
			phar_obj->arc.archive->sig_flags = algo;
			phar_obj->arc.archive->is_modified = 1;
			PHAR_G(openssl_privatekey) = key;
			PHAR_G(openssl_privatekey_len) = key_len;

			phar_flush(phar_obj->arc.archive, 0, 0, 0, &error TSRMLS_CC);
			if (error) {
				zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
				efree(error);
			}
			break;
		default:
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"Unknown signature algorithm specified");
	}
}
/* }}} */

/* {{{ proto array|false Phar::getSignature()
 * Returns a hash signature, or FALSE if the archive is unsigned.
 */
PHP_METHOD(Phar, getSignature)
{
	PHAR_ARCHIVE_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (phar_obj->arc.archive->signature) {
		char *unknown;
		int unknown_len;

		array_init(return_value);
		add_assoc_stringl(return_value, "hash", phar_obj->arc.archive->signature, phar_obj->arc.archive->sig_len, 1);
		switch(phar_obj->arc.archive->sig_flags) {
			case PHAR_SIG_MD5:
				add_assoc_stringl(return_value, "hash_type", "MD5", 3, 1);
				break;
			case PHAR_SIG_SHA1:
				add_assoc_stringl(return_value, "hash_type", "SHA-1", 5, 1);
				break;
			case PHAR_SIG_SHA256:
				add_assoc_stringl(return_value, "hash_type", "SHA-256", 7, 1);
				break;
			case PHAR_SIG_SHA512:
				add_assoc_stringl(return_value, "hash_type", "SHA-512", 7, 1);
				break;
			case PHAR_SIG_OPENSSL:
				add_assoc_stringl(return_value, "hash_type", "OpenSSL", 7, 1);
				break;
			default:
				unknown_len = spprintf(&unknown, 0, "Unknown (%u)", phar_obj->arc.archive->sig_flags);
				add_assoc_stringl(return_value, "hash_type", unknown, unknown_len, 0);
				break;
		}
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool Phar::getModified()
 * Return whether phar was modified
 */
PHP_METHOD(Phar, getModified)
{
	PHAR_ARCHIVE_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(phar_obj->arc.archive->is_modified);
}
/* }}} */

static int phar_set_compression(void *pDest, void *argument TSRMLS_DC) /* {{{ */
{
	phar_entry_info *entry = (phar_entry_info *)pDest;
	php_uint32 compress = *(php_uint32 *)argument;

	if (entry->is_deleted) {
		return ZEND_HASH_APPLY_KEEP;
	}

	entry->old_flags = entry->flags;
	entry->flags &= ~PHAR_ENT_COMPRESSION_MASK;
	entry->flags |= compress;
	entry->is_modified = 1;
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

static int phar_test_compression(void *pDest, void *argument TSRMLS_DC) /* {{{ */
{
	phar_entry_info *entry = (phar_entry_info *)pDest;

	if (entry->is_deleted) {
		return ZEND_HASH_APPLY_KEEP;
	}

	if (!PHAR_G(has_bz2)) {
		if (entry->flags & PHAR_ENT_COMPRESSED_BZ2) {
			*(int *) argument = 0;
		}
	}

	if (!PHAR_G(has_zlib)) {
		if (entry->flags & PHAR_ENT_COMPRESSED_GZ) {
			*(int *) argument = 0;
		}
	}

	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

static void pharobj_set_compression(HashTable *manifest, php_uint32 compress TSRMLS_DC) /* {{{ */
{
	zend_hash_apply_with_argument(manifest, phar_set_compression, &compress TSRMLS_CC);
}
/* }}} */

static int pharobj_cancompress(HashTable *manifest TSRMLS_DC) /* {{{ */
{
	int test;

	test = 1;
	zend_hash_apply_with_argument(manifest, phar_test_compression, &test TSRMLS_CC);
	return test;
}
/* }}} */

/* {{{ proto object Phar::compress(int method[, string extension])
 * Compress a .tar, or .phar.tar with whole-file compression
 * The parameter can be one of Phar::GZ or Phar::BZ2 to specify
 * the kind of compression desired
 */
PHP_METHOD(Phar, compress)
{
	long method;
	char *ext = NULL;
	int ext_len = 0;
	php_uint32 flags;
	zval *ret;
	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|s", &method, &ext, &ext_len) == FAILURE) {
		return;
	}

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot compress phar archive, phar is read-only");
		return;
	}

	if (phar_obj->arc.archive->is_zip) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot compress zip-based archives with whole-archive compression");
		return;
	}

	switch (method) {
		case 0:
			flags = PHAR_FILE_COMPRESSED_NONE;
			break;
		case PHAR_ENT_COMPRESSED_GZ:
			if (!PHAR_G(has_zlib)) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
					"Cannot compress entire archive with gzip, enable ext/zlib in php.ini");
				return;
			}
			flags = PHAR_FILE_COMPRESSED_GZ;
			break;

		case PHAR_ENT_COMPRESSED_BZ2:
			if (!PHAR_G(has_bz2)) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
					"Cannot compress entire archive with bz2, enable ext/bz2 in php.ini");
				return;
			}
			flags = PHAR_FILE_COMPRESSED_BZ2;
			break;
		default:
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
				"Unknown compression specified, please pass one of Phar::GZ or Phar::BZ2");
			return;
	}

	if (phar_obj->arc.archive->is_tar) {
		ret = phar_convert_to_other(phar_obj->arc.archive, PHAR_FORMAT_TAR, ext, flags TSRMLS_CC);
	} else {
		ret = phar_convert_to_other(phar_obj->arc.archive, PHAR_FORMAT_PHAR, ext, flags TSRMLS_CC);
	}

	if (ret) {
		RETURN_ZVAL(ret, 1, 1);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto object Phar::decompress([string extension])
 * Decompress a .tar, or .phar.tar with whole-file compression
 */
PHP_METHOD(Phar, decompress)
{
	char *ext = NULL;
	int ext_len = 0;
	zval *ret;
	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &ext, &ext_len) == FAILURE) {
		return;
	}

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot decompress phar archive, phar is read-only");
		return;
	}

	if (phar_obj->arc.archive->is_zip) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot decompress zip-based archives with whole-archive compression");
		return;
	}

	if (phar_obj->arc.archive->is_tar) {
		ret = phar_convert_to_other(phar_obj->arc.archive, PHAR_FORMAT_TAR, ext, PHAR_FILE_COMPRESSED_NONE TSRMLS_CC);
	} else {
		ret = phar_convert_to_other(phar_obj->arc.archive, PHAR_FORMAT_PHAR, ext, PHAR_FILE_COMPRESSED_NONE TSRMLS_CC);
	}

	if (ret) {
		RETURN_ZVAL(ret, 1, 1);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto object Phar::compressFiles(int method)
 * Compress all files within a phar or zip archive using the specified compression
 * The parameter can be one of Phar::GZ or Phar::BZ2 to specify
 * the kind of compression desired
 */
PHP_METHOD(Phar, compressFiles)
{
	char *error;
	php_uint32 flags;
	long method;
	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &method) == FAILURE) {
		return;
	}

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Phar is readonly, cannot change compression");
		return;
	}

	switch (method) {
		case PHAR_ENT_COMPRESSED_GZ:
			if (!PHAR_G(has_zlib)) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
					"Cannot compress files within archive with gzip, enable ext/zlib in php.ini");
				return;
			}
			flags = PHAR_ENT_COMPRESSED_GZ;
			break;

		case PHAR_ENT_COMPRESSED_BZ2:
			if (!PHAR_G(has_bz2)) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
					"Cannot compress files within archive with bz2, enable ext/bz2 in php.ini");
				return;
			}
			flags = PHAR_ENT_COMPRESSED_BZ2;
			break;
		default:
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
				"Unknown compression specified, please pass one of Phar::GZ or Phar::BZ2");
			return;
	}

	if (phar_obj->arc.archive->is_tar) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Cannot compress with Gzip compression, tar archives cannot compress individual files, use compress() to compress the whole archive");
		return;
	}

	if (!pharobj_cancompress(&phar_obj->arc.archive->manifest TSRMLS_CC)) {
		if (flags == PHAR_FILE_COMPRESSED_GZ) {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
				"Cannot compress all files as Gzip, some are compressed as bzip2 and cannot be decompressed");
		} else {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
				"Cannot compress all files as Bzip2, some are compressed as gzip and cannot be decompressed");
		}
		return;
	}

	if (phar_obj->arc.archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->arc.archive) TSRMLS_CC)) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar_obj->arc.archive->fname);
		return;
	}
	pharobj_set_compression(&phar_obj->arc.archive->manifest, flags TSRMLS_CC);
	phar_obj->arc.archive->is_modified = 1;
	phar_flush(phar_obj->arc.archive, 0, 0, 0, &error TSRMLS_CC);

	if (error) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "%s", error);
		efree(error);
	}
}
/* }}} */

/* {{{ proto bool Phar::decompressFiles()
 * decompress every file
 */
PHP_METHOD(Phar, decompressFiles)
{
	char *error;
	PHAR_ARCHIVE_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Phar is readonly, cannot change compression");
		return;
	}

	if (!pharobj_cancompress(&phar_obj->arc.archive->manifest TSRMLS_CC)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Cannot decompress all files, some are compressed as bzip2 or gzip and cannot be decompressed");
		return;
	}

	if (phar_obj->arc.archive->is_tar) {
		RETURN_TRUE;
	} else {
		if (phar_obj->arc.archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->arc.archive) TSRMLS_CC)) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar_obj->arc.archive->fname);
			return;
		}
		pharobj_set_compression(&phar_obj->arc.archive->manifest, PHAR_ENT_COMPRESSED_NONE TSRMLS_CC);
	}

	phar_obj->arc.archive->is_modified = 1;
	phar_flush(phar_obj->arc.archive, 0, 0, 0, &error TSRMLS_CC);

	if (error) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "%s", error);
		efree(error);
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool Phar::copy(string oldfile, string newfile)
 * copy a file internal to the phar archive to another new file within the phar
 */
PHP_METHOD(Phar, copy)
{
	char *oldfile, *newfile, *error;
	const char *pcr_error;
	int oldfile_len, newfile_len;
	phar_entry_info *oldentry, newentry = {0}, *temp;

	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &oldfile, &oldfile_len, &newfile, &newfile_len) == FAILURE) {
		return;
	}

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot copy \"%s\" to \"%s\", phar is read-only", oldfile, newfile);
		RETURN_FALSE;
	}

	if (oldfile_len >= sizeof(".phar")-1 && !memcmp(oldfile, ".phar", sizeof(".phar")-1)) {
		/* can't copy a meta file */
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"file \"%s\" cannot be copied to file \"%s\", cannot copy Phar meta-file in %s", oldfile, newfile, phar_obj->arc.archive->fname);
		RETURN_FALSE;
	}

	if (newfile_len >= sizeof(".phar")-1 && !memcmp(newfile, ".phar", sizeof(".phar")-1)) {
		/* can't copy a meta file */
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"file \"%s\" cannot be copied to file \"%s\", cannot copy to Phar meta-file in %s", oldfile, newfile, phar_obj->arc.archive->fname);
		RETURN_FALSE;
	}

	if (!zend_hash_exists(&phar_obj->arc.archive->manifest, oldfile, (uint) oldfile_len) || SUCCESS != zend_hash_find(&phar_obj->arc.archive->manifest, oldfile, (uint) oldfile_len, (void**)&oldentry) || oldentry->is_deleted) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"file \"%s\" cannot be copied to file \"%s\", file does not exist in %s", oldfile, newfile, phar_obj->arc.archive->fname);
		RETURN_FALSE;
	}

	if (zend_hash_exists(&phar_obj->arc.archive->manifest, newfile, (uint) newfile_len)) {
		if (SUCCESS == zend_hash_find(&phar_obj->arc.archive->manifest, newfile, (uint) newfile_len, (void**)&temp) || !temp->is_deleted) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"file \"%s\" cannot be copied to file \"%s\", file must not already exist in phar %s", oldfile, newfile, phar_obj->arc.archive->fname);
			RETURN_FALSE;
		}
	}

	if (phar_path_check(&newfile, &newfile_len, &pcr_error) > pcr_is_ok) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"file \"%s\" contains invalid characters %s, cannot be copied from \"%s\" in phar %s", newfile, pcr_error, oldfile, phar_obj->arc.archive->fname);
		RETURN_FALSE;
	}

	if (phar_obj->arc.archive->is_persistent) {
		if (FAILURE == phar_copy_on_write(&(phar_obj->arc.archive) TSRMLS_CC)) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar_obj->arc.archive->fname);
			return;
		}
		/* re-populate with copied-on-write entry */
		zend_hash_find(&phar_obj->arc.archive->manifest, oldfile, (uint) oldfile_len, (void**)&oldentry);
	}

	memcpy((void *) &newentry, oldentry, sizeof(phar_entry_info));

	if (newentry.metadata) {
		zval *t;

		t = newentry.metadata;
		ALLOC_ZVAL(newentry.metadata);
		*newentry.metadata = *t;
		zval_copy_ctor(newentry.metadata);
		Z_SET_REFCOUNT_P(newentry.metadata, 1);

		newentry.metadata_str.c = NULL;
		newentry.metadata_str.len = 0;
	}

	newentry.filename = estrndup(newfile, newfile_len);
	newentry.filename_len = newfile_len;
	newentry.fp_refcount = 0;

	if (oldentry->fp_type != PHAR_FP) {
		if (FAILURE == phar_copy_entry_fp(oldentry, &newentry, &error TSRMLS_CC)) {
			efree(newentry.filename);
			php_stream_close(newentry.fp);
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
			efree(error);
			return;
		}
	}

	zend_hash_add(&oldentry->phar->manifest, newfile, newfile_len, (void*)&newentry, sizeof(phar_entry_info), NULL);
	phar_obj->arc.archive->is_modified = 1;
	phar_flush(phar_obj->arc.archive, 0, 0, 0, &error TSRMLS_CC);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
		efree(error);
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int Phar::offsetExists(string entry)
 * determines whether a file exists in the phar
 */
PHP_METHOD(Phar, offsetExists)
{
	char *fname;
	int fname_len;
	phar_entry_info *entry;

	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &fname, &fname_len) == FAILURE) {
		return;
	}

	if (zend_hash_exists(&phar_obj->arc.archive->manifest, fname, (uint) fname_len)) {
		if (SUCCESS == zend_hash_find(&phar_obj->arc.archive->manifest, fname, (uint) fname_len, (void**)&entry)) {
			if (entry->is_deleted) {
				/* entry is deleted, but has not been flushed to disk yet */
				RETURN_FALSE;
			}
		}

		if (fname_len >= sizeof(".phar")-1 && !memcmp(fname, ".phar", sizeof(".phar")-1)) {
			/* none of these are real files, so they don't exist */
			RETURN_FALSE;
		}
		RETURN_TRUE;
	} else {
		if (zend_hash_exists(&phar_obj->arc.archive->virtual_dirs, fname, (uint) fname_len)) {
			RETURN_TRUE;
		}
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int Phar::offsetGet(string entry)
 * get a PharFileInfo object for a specific file
 */
PHP_METHOD(Phar, offsetGet)
{
	char *fname, *error;
	int fname_len;
	zval *zfname;
	phar_entry_info *entry;
	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &fname, &fname_len) == FAILURE) {
		return;
	}

	/* security is 0 here so that we can get a better error message than "entry doesn't exist" */
	if (!(entry = phar_get_entry_info_dir(phar_obj->arc.archive, fname, fname_len, 1, &error, 0 TSRMLS_CC))) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Entry %s does not exist%s%s", fname, error?", ":"", error?error:"");
	} else {
		if (fname_len == sizeof(".phar/stub.php")-1 && !memcmp(fname, ".phar/stub.php", sizeof(".phar/stub.php")-1)) {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Cannot get stub \".phar/stub.php\" directly in phar \"%s\", use getStub", phar_obj->arc.archive->fname);
			return;
		}

		if (fname_len == sizeof(".phar/alias.txt")-1 && !memcmp(fname, ".phar/alias.txt", sizeof(".phar/alias.txt")-1)) {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Cannot get alias \".phar/alias.txt\" directly in phar \"%s\", use getAlias", phar_obj->arc.archive->fname);
			return;
		}

		if (fname_len >= sizeof(".phar")-1 && !memcmp(fname, ".phar", sizeof(".phar")-1)) {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Cannot directly get any files or directories in magic \".phar\" directory", phar_obj->arc.archive->fname);
			return;
		}

		if (entry->is_temp_dir) {
			efree(entry->filename);
			efree(entry);
		}

		fname_len = spprintf(&fname, 0, "phar://%s/%s", phar_obj->arc.archive->fname, fname);
		MAKE_STD_ZVAL(zfname);
		ZVAL_STRINGL(zfname, fname, fname_len, 0);
		spl_instantiate_arg_ex1(phar_obj->spl.info_class, &return_value, 0, zfname TSRMLS_CC);
		zval_ptr_dtor(&zfname);
	}
}
/* }}} */

/* {{{ add a file within the phar archive from a string or resource
 */
static void phar_add_file(phar_archive_data **pphar, char *filename, int filename_len, char *cont_str, int cont_len, zval *zresource TSRMLS_DC)
{
	char *error;
	size_t contents_len;
	phar_entry_data *data;
	php_stream *contents_file;

	if (filename_len >= sizeof(".phar")-1 && !memcmp(filename, ".phar", sizeof(".phar")-1)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Cannot create any files in magic \".phar\" directory", (*pphar)->fname);
		return;
	}

	if (!(data = phar_get_or_create_entry_data((*pphar)->fname, (*pphar)->fname_len, filename, filename_len, "w+b", 0, &error, 1 TSRMLS_CC))) {
		if (error) {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Entry %s does not exist and cannot be created: %s", filename, error);
			efree(error);
		} else {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Entry %s does not exist and cannot be created", filename);
		}
		return;
	} else {
		if (error) {
			efree(error);
		}

		if (!data->internal_file->is_dir) {
			if (cont_str) {
				contents_len = php_stream_write(data->fp, cont_str, cont_len);
				if (contents_len != cont_len) {
					zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Entry %s could not be written to", filename);
					return;
				}
			} else {
				if (!(php_stream_from_zval_no_verify(contents_file, &zresource))) {
					zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Entry %s could not be written to", filename);
					return;
				}
				phar_stream_copy_to_stream(contents_file, data->fp, PHP_STREAM_COPY_ALL, &contents_len);
			}

			data->internal_file->compressed_filesize = data->internal_file->uncompressed_filesize = contents_len;
		}

		/* check for copy-on-write */
		if (pphar[0] != data->phar) {
			*pphar = data->phar;
		}
		phar_entry_delref(data TSRMLS_CC);
		phar_flush(*pphar, 0, 0, 0, &error TSRMLS_CC);

		if (error) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
			efree(error);
		}
	}
}
/* }}} */

/* {{{ create a directory within the phar archive
 */
static void phar_mkdir(phar_archive_data **pphar, char *dirname, int dirname_len TSRMLS_DC)
{
	char *error;
	phar_entry_data *data;

	if (!(data = phar_get_or_create_entry_data((*pphar)->fname, (*pphar)->fname_len, dirname, dirname_len, "w+b", 2, &error, 1 TSRMLS_CC))) {
		if (error) {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Directory %s does not exist and cannot be created: %s", dirname, error);
			efree(error);
		} else {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Directory %s does not exist and cannot be created", dirname);
		}

		return;
	} else {
		if (error) {
			efree(error);
		}

		/* check for copy on write */
		if (data->phar != *pphar) {
			*pphar = data->phar;
		}
		phar_entry_delref(data TSRMLS_CC);
		phar_flush(*pphar, 0, 0, 0, &error TSRMLS_CC);

		if (error) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
			efree(error);
		}
	}
}
/* }}} */

/* {{{ proto int Phar::offsetSet(string entry, string value)
 * set the contents of an internal file to those of an external file
 */
PHP_METHOD(Phar, offsetSet)
{
	char *fname, *cont_str = NULL;
	int fname_len, cont_len;
	zval *zresource;
	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Write operations disabled by the php.ini setting phar.readonly");
		return;
	}

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "sr", &fname, &fname_len, &zresource) == FAILURE
	&& zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &fname, &fname_len, &cont_str, &cont_len) == FAILURE) {
		return;
	}

	if (fname_len == sizeof(".phar/stub.php")-1 && !memcmp(fname, ".phar/stub.php", sizeof(".phar/stub.php")-1)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Cannot set stub \".phar/stub.php\" directly in phar \"%s\", use setStub", phar_obj->arc.archive->fname);
		return;
	}

	if (fname_len == sizeof(".phar/alias.txt")-1 && !memcmp(fname, ".phar/alias.txt", sizeof(".phar/alias.txt")-1)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Cannot set alias \".phar/alias.txt\" directly in phar \"%s\", use setAlias", phar_obj->arc.archive->fname);
		return;
	}

	if (fname_len >= sizeof(".phar")-1 && !memcmp(fname, ".phar", sizeof(".phar")-1)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Cannot set any files or directories in magic \".phar\" directory", phar_obj->arc.archive->fname);
		return;
	}

	phar_add_file(&(phar_obj->arc.archive), fname, fname_len, cont_str, cont_len, zresource TSRMLS_CC);
}
/* }}} */

/* {{{ proto int Phar::offsetUnset(string entry)
 * remove a file from a phar
 */
PHP_METHOD(Phar, offsetUnset)
{
	char *fname, *error;
	int fname_len;
	phar_entry_info *entry;
	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Write operations disabled by the php.ini setting phar.readonly");
		return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &fname, &fname_len) == FAILURE) {
		return;
	}

	if (zend_hash_exists(&phar_obj->arc.archive->manifest, fname, (uint) fname_len)) {
		if (SUCCESS == zend_hash_find(&phar_obj->arc.archive->manifest, fname, (uint) fname_len, (void**)&entry)) {
			if (entry->is_deleted) {
				/* entry is deleted, but has not been flushed to disk yet */
				return;
			}

			if (phar_obj->arc.archive->is_persistent) {
				if (FAILURE == phar_copy_on_write(&(phar_obj->arc.archive) TSRMLS_CC)) {
					zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar_obj->arc.archive->fname);
					return;
				}
				/* re-populate entry after copy on write */
				zend_hash_find(&phar_obj->arc.archive->manifest, fname, (uint) fname_len, (void **)&entry);
			}
			entry->is_modified = 0;
			entry->is_deleted = 1;
			/* we need to "flush" the stream to save the newly deleted file on disk */
			phar_flush(phar_obj->arc.archive, 0, 0, 0, &error TSRMLS_CC);

			if (error) {
				zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
				efree(error);
			}

			RETURN_TRUE;
		}
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string Phar::addEmptyDir(string dirname)
 * Adds an empty directory to the phar archive
 */
PHP_METHOD(Phar, addEmptyDir)
{
	char *dirname;
	int dirname_len;

	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &dirname, &dirname_len) == FAILURE) {
		return;
	}

	if (dirname_len >= sizeof(".phar")-1 && !memcmp(dirname, ".phar", sizeof(".phar")-1)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Cannot create a directory in magic \".phar\" directory");
		return;
	}

	phar_mkdir(&phar_obj->arc.archive, dirname, dirname_len TSRMLS_CC);
}
/* }}} */

/* {{{ proto string Phar::addFile(string filename[, string localname])
 * Adds a file to the archive using the filename, or the second parameter as the name within the archive
 */
PHP_METHOD(Phar, addFile)
{
	char *fname, *localname = NULL;
	int fname_len, localname_len = 0;
	php_stream *resource;
	zval *zresource;

	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &fname, &fname_len, &localname, &localname_len) == FAILURE) {
		return;
	}

#if PHP_API_VERSION < 20100412
	if (PG(safe_mode) && (!php_checkuid(fname, NULL, CHECKUID_ALLOW_ONLY_FILE))) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "phar error: unable to open file \"%s\" to add to phar archive, safe_mode restrictions prevent this", fname);
		return;
	}
#endif

	if (!strstr(fname, "://") && php_check_open_basedir(fname TSRMLS_CC)) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "phar error: unable to open file \"%s\" to add to phar archive, open_basedir restrictions prevent this", fname);
		return;
	}

	if (!(resource = php_stream_open_wrapper(fname, "rb", 0, NULL))) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "phar error: unable to open file \"%s\" to add to phar archive", fname);
		return;
	}

	if (localname) {
		fname = localname;
		fname_len = localname_len;
	}

	MAKE_STD_ZVAL(zresource);
	php_stream_to_zval(resource, zresource);
	phar_add_file(&(phar_obj->arc.archive), fname, fname_len, NULL, 0, zresource TSRMLS_CC);
	efree(zresource);
	php_stream_close(resource);
}
/* }}} */

/* {{{ proto string Phar::addFromString(string localname, string contents)
 * Adds a file to the archive using its contents as a string
 */
PHP_METHOD(Phar, addFromString)
{
	char *localname, *cont_str;
	int localname_len, cont_len;

	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &localname, &localname_len, &cont_str, &cont_len) == FAILURE) {
		return;
	}

	phar_add_file(&(phar_obj->arc.archive), localname, localname_len, cont_str, cont_len, NULL TSRMLS_CC);
}
/* }}} */

/* {{{ proto string Phar::getStub()
 * Returns the stub at the head of a phar archive as a string.
 */
PHP_METHOD(Phar, getStub)
{
	size_t len;
	char *buf;
	php_stream *fp;
	php_stream_filter *filter = NULL;
	phar_entry_info *stub;

	PHAR_ARCHIVE_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (phar_obj->arc.archive->is_tar || phar_obj->arc.archive->is_zip) {

		if (SUCCESS == zend_hash_find(&(phar_obj->arc.archive->manifest), ".phar/stub.php", sizeof(".phar/stub.php")-1, (void **)&stub)) {
			if (phar_obj->arc.archive->fp && !phar_obj->arc.archive->is_brandnew && !(stub->flags & PHAR_ENT_COMPRESSION_MASK)) {
				fp = phar_obj->arc.archive->fp;
			} else {
				if (!(fp = php_stream_open_wrapper(phar_obj->arc.archive->fname, "rb", 0, NULL))) {
					zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "phar error: unable to open phar \"%s\"", phar_obj->arc.archive->fname);
					return;
				}
				if (stub->flags & PHAR_ENT_COMPRESSION_MASK) {
					char *filter_name;

					if ((filter_name = phar_decompress_filter(stub, 0)) != NULL) {
						filter = php_stream_filter_create(filter_name, NULL, php_stream_is_persistent(fp) TSRMLS_CC);
					} else {
						filter = NULL;
					}
					if (!filter) {
						zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "phar error: unable to read stub of phar \"%s\" (cannot create %s filter)", phar_obj->arc.archive->fname, phar_decompress_filter(stub, 1));
						return;
					}
					php_stream_filter_append(&fp->readfilters, filter);
				}
			}

			if (!fp)  {
				zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC,
					"Unable to read stub");
				return;
			}

			php_stream_seek(fp, stub->offset_abs, SEEK_SET);
			len = stub->uncompressed_filesize;
			goto carry_on;
		} else {
			RETURN_STRINGL("", 0, 1);
		}
	}
	len = phar_obj->arc.archive->halt_offset;

	if (phar_obj->arc.archive->fp && !phar_obj->arc.archive->is_brandnew) {
		fp = phar_obj->arc.archive->fp;
	} else {
		fp = php_stream_open_wrapper(phar_obj->arc.archive->fname, "rb", 0, NULL);
	}

	if (!fp)  {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC,
			"Unable to read stub");
		return;
	}

	php_stream_rewind(fp);
carry_on:
	buf = safe_emalloc(len, 1, 1);

	if (len != php_stream_read(fp, buf, len)) {
		if (fp != phar_obj->arc.archive->fp) {
			php_stream_close(fp);
		}
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC,
			"Unable to read stub");
		efree(buf);
		return;
	}

	if (filter) {
		php_stream_filter_flush(filter, 1);
		php_stream_filter_remove(filter, 1 TSRMLS_CC);
	}

	if (fp != phar_obj->arc.archive->fp) {
		php_stream_close(fp);
	}

	buf[len] = '\0';
	RETURN_STRINGL(buf, len, 0);
}
/* }}}*/

/* {{{ proto int Phar::hasMetaData()
 * Returns TRUE if the phar has global metadata, FALSE otherwise.
 */
PHP_METHOD(Phar, hasMetadata)
{
	PHAR_ARCHIVE_OBJECT();

	RETURN_BOOL(phar_obj->arc.archive->metadata != NULL);
}
/* }}} */

/* {{{ proto int Phar::getMetaData()
 * Returns the global metadata of the phar
 */
PHP_METHOD(Phar, getMetadata)
{
	PHAR_ARCHIVE_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (phar_obj->arc.archive->metadata) {
		if (phar_obj->arc.archive->is_persistent) {
			zval *ret;
			char *buf = estrndup((char *) phar_obj->arc.archive->metadata, phar_obj->arc.archive->metadata_len);
			/* assume success, we would have failed before */
			phar_parse_metadata(&buf, &ret, phar_obj->arc.archive->metadata_len TSRMLS_CC);
			efree(buf);
			RETURN_ZVAL(ret, 0, 1);
		}
		RETURN_ZVAL(phar_obj->arc.archive->metadata, 1, 0);
	}
}
/* }}} */

/* {{{ proto int Phar::setMetaData(mixed $metadata)
 * Sets the global metadata of the phar
 */
PHP_METHOD(Phar, setMetadata)
{
	char *error;
	zval *metadata;

	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Write operations disabled by the php.ini setting phar.readonly");
		return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &metadata) == FAILURE) {
		return;
	}

	if (phar_obj->arc.archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->arc.archive) TSRMLS_CC)) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar_obj->arc.archive->fname);
		return;
	}
	if (phar_obj->arc.archive->metadata) {
		zval_ptr_dtor(&phar_obj->arc.archive->metadata);
		phar_obj->arc.archive->metadata = NULL;
	}

	MAKE_STD_ZVAL(phar_obj->arc.archive->metadata);
	ZVAL_ZVAL(phar_obj->arc.archive->metadata, metadata, 1, 0);
	phar_obj->arc.archive->is_modified = 1;
	phar_flush(phar_obj->arc.archive, 0, 0, 0, &error TSRMLS_CC);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
		efree(error);
	}
}
/* }}} */

/* {{{ proto int Phar::delMetadata()
 * Deletes the global metadata of the phar
 */
PHP_METHOD(Phar, delMetadata)
{
	char *error;

	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Write operations disabled by the php.ini setting phar.readonly");
		return;
	}

	if (phar_obj->arc.archive->metadata) {
		zval_ptr_dtor(&phar_obj->arc.archive->metadata);
		phar_obj->arc.archive->metadata = NULL;
		phar_obj->arc.archive->is_modified = 1;
		phar_flush(phar_obj->arc.archive, 0, 0, 0, &error TSRMLS_CC);

		if (error) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
			efree(error);
			RETURN_FALSE;
		} else {
			RETURN_TRUE;
		}

	} else {
		RETURN_TRUE;
	}
}
/* }}} */
#if PHP_API_VERSION < 20100412
#define PHAR_OPENBASEDIR_CHECKPATH(filename) \
	(PG(safe_mode) && (!php_checkuid(filename, NULL, CHECKUID_CHECK_FILE_AND_DIR))) || php_check_open_basedir(filename TSRMLS_CC)
#else
#define PHAR_OPENBASEDIR_CHECKPATH(filename) \
	php_check_open_basedir(filename TSRMLS_CC)
#endif

static int phar_extract_file(zend_bool overwrite, phar_entry_info *entry, char *dest, int dest_len, char **error TSRMLS_DC) /* {{{ */
{
	php_stream_statbuf ssb;
	int len;
	php_stream *fp;
	char *fullpath;
	const char *slash;
	mode_t mode;

	if (entry->is_mounted) {
		/* silently ignore mounted entries */
		return SUCCESS;
	}

	if (entry->filename_len >= sizeof(".phar")-1 && !memcmp(entry->filename, ".phar", sizeof(".phar")-1)) {
		return SUCCESS;
	}

	len = spprintf(&fullpath, 0, "%s/%s", dest, entry->filename);

	if (len >= MAXPATHLEN) {
		char *tmp;
		/* truncate for error message */
		fullpath[50] = '\0';
		if (entry->filename_len > 50) {
			tmp = estrndup(entry->filename, 50);
			spprintf(error, 4096, "Cannot extract \"%s...\" to \"%s...\", extracted filename is too long for filesystem", tmp, fullpath);
			efree(tmp);
		} else {
			spprintf(error, 4096, "Cannot extract \"%s\" to \"%s...\", extracted filename is too long for filesystem", entry->filename, fullpath);
		}
		efree(fullpath);
		return FAILURE;
	}

	if (!len) {
		spprintf(error, 4096, "Cannot extract \"%s\", internal error", entry->filename);
		efree(fullpath);
		return FAILURE;
	}

	if (PHAR_OPENBASEDIR_CHECKPATH(fullpath)) {
		spprintf(error, 4096, "Cannot extract \"%s\" to \"%s\", openbasedir/safe mode restrictions in effect", entry->filename, fullpath);
		efree(fullpath);
		return FAILURE;
	}

	/* let see if the path already exists */
	if (!overwrite && SUCCESS == php_stream_stat_path(fullpath, &ssb)) {
		spprintf(error, 4096, "Cannot extract \"%s\" to \"%s\", path already exists", entry->filename, fullpath);
		efree(fullpath);
		return FAILURE;
	}

	/* perform dirname */
	slash = zend_memrchr(entry->filename, '/', entry->filename_len);

	if (slash) {
		fullpath[dest_len + (slash - entry->filename) + 1] = '\0';
	} else {
		fullpath[dest_len] = '\0';
	}

	if (FAILURE == php_stream_stat_path(fullpath, &ssb)) {
		if (entry->is_dir) {
			if (!php_stream_mkdir(fullpath, entry->flags & PHAR_ENT_PERM_MASK,  PHP_STREAM_MKDIR_RECURSIVE, NULL)) {
				spprintf(error, 4096, "Cannot extract \"%s\", could not create directory \"%s\"", entry->filename, fullpath);
				efree(fullpath);
				return FAILURE;
			}
		} else {
			if (!php_stream_mkdir(fullpath, 0777,  PHP_STREAM_MKDIR_RECURSIVE, NULL)) {
				spprintf(error, 4096, "Cannot extract \"%s\", could not create directory \"%s\"", entry->filename, fullpath);
				efree(fullpath);
				return FAILURE;
			}
		}
	}

	if (slash) {
		fullpath[dest_len + (slash - entry->filename) + 1] = '/';
	} else {
		fullpath[dest_len] = '/';
	}

	/* it is a standalone directory, job done */
	if (entry->is_dir) {
		efree(fullpath);
		return SUCCESS;
	}

#if PHP_API_VERSION < 20100412
	fp = php_stream_open_wrapper(fullpath, "w+b", REPORT_ERRORS|ENFORCE_SAFE_MODE, NULL);
#else
	fp = php_stream_open_wrapper(fullpath, "w+b", REPORT_ERRORS, NULL);
#endif

	if (!fp) {
		spprintf(error, 4096, "Cannot extract \"%s\", could not open for writing \"%s\"", entry->filename, fullpath);
		efree(fullpath);
		return FAILURE;
	}

	if (!phar_get_efp(entry, 0 TSRMLS_CC)) {
		if (FAILURE == phar_open_entry_fp(entry, error, 1 TSRMLS_CC)) {
			if (error) {
				spprintf(error, 4096, "Cannot extract \"%s\" to \"%s\", unable to open internal file pointer: %s", entry->filename, fullpath, *error);
			} else {
				spprintf(error, 4096, "Cannot extract \"%s\" to \"%s\", unable to open internal file pointer", entry->filename, fullpath);
			}
			efree(fullpath);
			php_stream_close(fp);
			return FAILURE;
		}
	}

	if (FAILURE == phar_seek_efp(entry, 0, SEEK_SET, 0, 0 TSRMLS_CC)) {
		spprintf(error, 4096, "Cannot extract \"%s\" to \"%s\", unable to seek internal file pointer", entry->filename, fullpath);
		efree(fullpath);
		php_stream_close(fp);
		return FAILURE;
	}

	if (SUCCESS != phar_stream_copy_to_stream(phar_get_efp(entry, 0 TSRMLS_CC), fp, entry->uncompressed_filesize, NULL)) {
		spprintf(error, 4096, "Cannot extract \"%s\" to \"%s\", copying contents failed", entry->filename, fullpath);
		efree(fullpath);
		php_stream_close(fp);
		return FAILURE;
	}

	php_stream_close(fp);
	mode = (mode_t) entry->flags & PHAR_ENT_PERM_MASK;

	if (FAILURE == VCWD_CHMOD(fullpath, mode)) {
		spprintf(error, 4096, "Cannot extract \"%s\" to \"%s\", setting file permissions failed", entry->filename, fullpath);
		efree(fullpath);
		return FAILURE;
	}

	efree(fullpath);
	return SUCCESS;
}
/* }}} */

/* {{{ proto bool Phar::extractTo(string pathto[[, mixed files], bool overwrite])
 * Extract one or more file from a phar archive, optionally overwriting existing files
 */
PHP_METHOD(Phar, extractTo)
{
	char *error = NULL;
	php_stream *fp;
	php_stream_statbuf ssb;
	phar_entry_info *entry;
	char *pathto, *filename, *actual;
	int pathto_len, filename_len;
	int ret, i;
	int nelems;
	zval *zval_files = NULL;
	zend_bool overwrite = 0;

	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|z!b", &pathto, &pathto_len, &zval_files, &overwrite) == FAILURE) {
		return;
	}

	fp = php_stream_open_wrapper(phar_obj->arc.archive->fname, "rb", IGNORE_URL|STREAM_MUST_SEEK, &actual);

	if (!fp) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC,
			"Invalid argument, %s cannot be found", phar_obj->arc.archive->fname);
		return;
	}

	efree(actual);
	php_stream_close(fp);

	if (pathto_len < 1) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC,
			"Invalid argument, extraction path must be non-zero length");
		return;
	}

	if (pathto_len >= MAXPATHLEN) {
		char *tmp = estrndup(pathto, 50);
		/* truncate for error message */
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "Cannot extract to \"%s...\", destination directory is too long for filesystem", tmp);
		efree(tmp);
		return;
	}

	if (php_stream_stat_path(pathto, &ssb) < 0) {
		ret = php_stream_mkdir(pathto, 0777,  PHP_STREAM_MKDIR_RECURSIVE, NULL);
		if (!ret) {
			zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC,
				"Unable to create path \"%s\" for extraction", pathto);
			return;
		}
	} else if (!(ssb.sb.st_mode & S_IFDIR)) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC,
			"Unable to use path \"%s\" for extraction, it is a file, must be a directory", pathto);
		return;
	}

	if (zval_files) {
		switch (Z_TYPE_P(zval_files)) {
			case IS_NULL:
				goto all_files;
			case IS_STRING:
				filename = Z_STRVAL_P(zval_files);
				filename_len = Z_STRLEN_P(zval_files);
				break;
			case IS_ARRAY:
				nelems = zend_hash_num_elements(Z_ARRVAL_P(zval_files));
				if (nelems == 0 ) {
					RETURN_FALSE;
				}
				for (i = 0; i < nelems; i++) {
					zval **zval_file;
					if (zend_hash_index_find(Z_ARRVAL_P(zval_files), i, (void **) &zval_file) == SUCCESS) {
						switch (Z_TYPE_PP(zval_file)) {
							case IS_STRING:
								break;
							default:
								zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC,
									"Invalid argument, array of filenames to extract contains non-string value");
								return;
						}
						if (FAILURE == zend_hash_find(&phar_obj->arc.archive->manifest, Z_STRVAL_PP(zval_file), Z_STRLEN_PP(zval_file), (void **)&entry)) {
							zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC,
								"Phar Error: attempted to extract non-existent file \"%s\" from phar \"%s\"", Z_STRVAL_PP(zval_file), phar_obj->arc.archive->fname);
						}
						if (FAILURE == phar_extract_file(overwrite, entry, pathto, pathto_len, &error TSRMLS_CC)) {
							zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC,
								"Extraction from phar \"%s\" failed: %s", phar_obj->arc.archive->fname, error);
							efree(error);
							return;
						}
					}
				}
				RETURN_TRUE;
			default:
				zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC,
					"Invalid argument, expected a filename (string) or array of filenames");
				return;
		}

		if (FAILURE == zend_hash_find(&phar_obj->arc.archive->manifest, filename, filename_len, (void **)&entry)) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC,
				"Phar Error: attempted to extract non-existent file \"%s\" from phar \"%s\"", filename, phar_obj->arc.archive->fname);
			return;
		}

		if (FAILURE == phar_extract_file(overwrite, entry, pathto, pathto_len, &error TSRMLS_CC)) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC,
				"Extraction from phar \"%s\" failed: %s", phar_obj->arc.archive->fname, error);
			efree(error);
			return;
		}
	} else {
		phar_archive_data *phar;
all_files:
		phar = phar_obj->arc.archive;
		/* Extract all files */
		if (!zend_hash_num_elements(&(phar->manifest))) {
			RETURN_TRUE;
		}

		for (zend_hash_internal_pointer_reset(&phar->manifest);
		zend_hash_has_more_elements(&phar->manifest) == SUCCESS;
		zend_hash_move_forward(&phar->manifest)) {

			if (zend_hash_get_current_data(&phar->manifest, (void **)&entry) == FAILURE) {
				continue;
			}

			if (FAILURE == phar_extract_file(overwrite, entry, pathto, pathto_len, &error TSRMLS_CC)) {
				zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC,
					"Extraction from phar \"%s\" failed: %s", phar->fname, error);
				efree(error);
				return;
			}
		}
	}
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto void PharFileInfo::__construct(string entry)
 * Construct a Phar entry object
 */
PHP_METHOD(PharFileInfo, __construct)
{
	char *fname, *arch, *entry, *error;
	int fname_len, arch_len, entry_len;
	phar_entry_object *entry_obj;
	phar_entry_info *entry_info;
	phar_archive_data *phar_data;
	zval *zobj = getThis(), arg1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &fname, &fname_len) == FAILURE) {
		return;
	}

	entry_obj = (phar_entry_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (entry_obj->ent.entry) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Cannot call constructor twice");
		return;
	}

	if (fname_len < 7 || memcmp(fname, "phar://", 7) || phar_split_fname(fname, fname_len, &arch, &arch_len, &entry, &entry_len, 2, 0 TSRMLS_CC) == FAILURE) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC,
			"'%s' is not a valid phar archive URL (must have at least phar://filename.phar)", fname);
		return;
	}

	if (phar_open_from_filename(arch, arch_len, NULL, 0, REPORT_ERRORS, &phar_data, &error TSRMLS_CC) == FAILURE) {
		efree(arch);
		efree(entry);
		if (error) {
			zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC,
				"Cannot open phar file '%s': %s", fname, error);
			efree(error);
		} else {
			zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC,
				"Cannot open phar file '%s'", fname);
		}
		return;
	}

	if ((entry_info = phar_get_entry_info_dir(phar_data, entry, entry_len, 1, &error, 1 TSRMLS_CC)) == NULL) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC,
			"Cannot access phar file entry '%s' in archive '%s'%s%s", entry, arch, error ? ", " : "", error ? error : "");
		efree(arch);
		efree(entry);
		return;
	}

	efree(arch);
	efree(entry);

	entry_obj->ent.entry = entry_info;

	INIT_PZVAL(&arg1);
	ZVAL_STRINGL(&arg1, fname, fname_len, 0);

	zend_call_method_with_1_params(&zobj, Z_OBJCE_P(zobj), 
		&spl_ce_SplFileInfo->constructor, "__construct", NULL, &arg1);
}
/* }}} */

#define PHAR_ENTRY_OBJECT() \
	phar_entry_object *entry_obj = (phar_entry_object*)zend_object_store_get_object(getThis() TSRMLS_CC); \
	if (!entry_obj->ent.entry) { \
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, \
			"Cannot call method on an uninitialized PharFileInfo object"); \
		return; \
	}

/* {{{ proto void PharFileInfo::__destruct()
 * clean up directory-based entry objects
 */
PHP_METHOD(PharFileInfo, __destruct)
{
	phar_entry_object *entry_obj = (phar_entry_object*)zend_object_store_get_object(getThis() TSRMLS_CC); \

	if (entry_obj->ent.entry && entry_obj->ent.entry->is_temp_dir) {
		if (entry_obj->ent.entry->filename) {
			efree(entry_obj->ent.entry->filename);
			entry_obj->ent.entry->filename = NULL;
		}

		efree(entry_obj->ent.entry);
		entry_obj->ent.entry = NULL;
	}
}
/* }}} */

/* {{{ proto int PharFileInfo::getCompressedSize()
 * Returns the compressed size
 */
PHP_METHOD(PharFileInfo, getCompressedSize)
{
	PHAR_ENTRY_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(entry_obj->ent.entry->compressed_filesize);
}
/* }}} */

/* {{{ proto bool PharFileInfo::isCompressed([int compression_type])
 * Returns whether the entry is compressed, and whether it is compressed with Phar::GZ or Phar::BZ2 if specified
 */
PHP_METHOD(PharFileInfo, isCompressed)
{
	/* a number that is not Phar::GZ or Phar::BZ2 */
	long method = 9021976;
	PHAR_ENTRY_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &method) == FAILURE) {
		return;
	}

	switch (method) {
		case 9021976:
			RETURN_BOOL(entry_obj->ent.entry->flags & PHAR_ENT_COMPRESSION_MASK);
		case PHAR_ENT_COMPRESSED_GZ:
			RETURN_BOOL(entry_obj->ent.entry->flags & PHAR_ENT_COMPRESSED_GZ);
		case PHAR_ENT_COMPRESSED_BZ2:
			RETURN_BOOL(entry_obj->ent.entry->flags & PHAR_ENT_COMPRESSED_BZ2);
		default:
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, \
				"Unknown compression type specified"); \
	}
}
/* }}} */

/* {{{ proto int PharFileInfo::getCRC32()
 * Returns CRC32 code or throws an exception if not CRC checked
 */
PHP_METHOD(PharFileInfo, getCRC32)
{
	PHAR_ENTRY_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (entry_obj->ent.entry->is_dir) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, \
			"Phar entry is a directory, does not have a CRC"); \
		return;
	}

	if (entry_obj->ent.entry->is_crc_checked) {
		RETURN_LONG(entry_obj->ent.entry->crc32);
	} else {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, \
			"Phar entry was not CRC checked"); \
	}
}
/* }}} */

/* {{{ proto int PharFileInfo::isCRCChecked()
 * Returns whether file entry is CRC checked
 */
PHP_METHOD(PharFileInfo, isCRCChecked)
{
	PHAR_ENTRY_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(entry_obj->ent.entry->is_crc_checked);
}
/* }}} */

/* {{{ proto int PharFileInfo::getPharFlags()
 * Returns the Phar file entry flags
 */
PHP_METHOD(PharFileInfo, getPharFlags)
{
	PHAR_ENTRY_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(entry_obj->ent.entry->flags & ~(PHAR_ENT_PERM_MASK|PHAR_ENT_COMPRESSION_MASK));
}
/* }}} */

/* {{{ proto int PharFileInfo::chmod()
 * set the file permissions for the Phar.  This only allows setting execution bit, read/write
 */
PHP_METHOD(PharFileInfo, chmod)
{
	char *error;
	long perms;
	PHAR_ENTRY_OBJECT();

	if (entry_obj->ent.entry->is_temp_dir) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, \
			"Phar entry \"%s\" is a temporary directory (not an actual entry in the archive), cannot chmod", entry_obj->ent.entry->filename); \
		return;
	}

	if (PHAR_G(readonly) && !entry_obj->ent.entry->phar->is_data) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "Cannot modify permissions for file \"%s\" in phar \"%s\", write operations are prohibited", entry_obj->ent.entry->filename, entry_obj->ent.entry->phar->fname);
		return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &perms) == FAILURE) {
		return;
	}

	if (entry_obj->ent.entry->is_persistent) {
		phar_archive_data *phar = entry_obj->ent.entry->phar;

		if (FAILURE == phar_copy_on_write(&phar TSRMLS_CC)) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar->fname);
			return;
		}
		/* re-populate after copy-on-write */
		zend_hash_find(&phar->manifest, entry_obj->ent.entry->filename, entry_obj->ent.entry->filename_len, (void **)&entry_obj->ent.entry);
	}
	/* clear permissions */
	entry_obj->ent.entry->flags &= ~PHAR_ENT_PERM_MASK;
	perms &= 0777;
	entry_obj->ent.entry->flags |= perms;
	entry_obj->ent.entry->old_flags = entry_obj->ent.entry->flags;
	entry_obj->ent.entry->phar->is_modified = 1;
	entry_obj->ent.entry->is_modified = 1;

	/* hackish cache in php_stat needs to be cleared */
	/* if this code fails to work, check main/streams/streams.c, _php_stream_stat_path */
	if (BG(CurrentLStatFile)) {
		efree(BG(CurrentLStatFile));
	}

	if (BG(CurrentStatFile)) {
		efree(BG(CurrentStatFile));
	}

	BG(CurrentLStatFile) = NULL;
	BG(CurrentStatFile) = NULL;
	phar_flush(entry_obj->ent.entry->phar, 0, 0, 0, &error TSRMLS_CC);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
		efree(error);
	}
}
/* }}} */

/* {{{ proto int PharFileInfo::hasMetaData()
 * Returns the metadata of the entry
 */
PHP_METHOD(PharFileInfo, hasMetadata)
{
	PHAR_ENTRY_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(entry_obj->ent.entry->metadata != NULL);
}
/* }}} */

/* {{{ proto int PharFileInfo::getMetaData()
 * Returns the metadata of the entry
 */
PHP_METHOD(PharFileInfo, getMetadata)
{
	PHAR_ENTRY_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (entry_obj->ent.entry->metadata) {
		if (entry_obj->ent.entry->is_persistent) {
			zval *ret;
			char *buf = estrndup((char *) entry_obj->ent.entry->metadata, entry_obj->ent.entry->metadata_len);
			/* assume success, we would have failed before */
			phar_parse_metadata(&buf, &ret, entry_obj->ent.entry->metadata_len TSRMLS_CC);
			efree(buf);
			RETURN_ZVAL(ret, 0, 1);
		}
		RETURN_ZVAL(entry_obj->ent.entry->metadata, 1, 0);
	}
}
/* }}} */

/* {{{ proto int PharFileInfo::setMetaData(mixed $metadata)
 * Sets the metadata of the entry
 */
PHP_METHOD(PharFileInfo, setMetadata)
{
	char *error;
	zval *metadata;

	PHAR_ENTRY_OBJECT();

	if (PHAR_G(readonly) && !entry_obj->ent.entry->phar->is_data) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Write operations disabled by the php.ini setting phar.readonly");
		return;
	}

	if (entry_obj->ent.entry->is_temp_dir) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, \
			"Phar entry is a temporary directory (not an actual entry in the archive), cannot set metadata"); \
		return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &metadata) == FAILURE) {
		return;
	}

	if (entry_obj->ent.entry->is_persistent) {
		phar_archive_data *phar = entry_obj->ent.entry->phar;

		if (FAILURE == phar_copy_on_write(&phar TSRMLS_CC)) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar->fname);
			return;
		}
		/* re-populate after copy-on-write */
		zend_hash_find(&phar->manifest, entry_obj->ent.entry->filename, entry_obj->ent.entry->filename_len, (void **)&entry_obj->ent.entry);
	}
	if (entry_obj->ent.entry->metadata) {
		zval_ptr_dtor(&entry_obj->ent.entry->metadata);
		entry_obj->ent.entry->metadata = NULL;
	}

	MAKE_STD_ZVAL(entry_obj->ent.entry->metadata);
	ZVAL_ZVAL(entry_obj->ent.entry->metadata, metadata, 1, 0);

	entry_obj->ent.entry->is_modified = 1;
	entry_obj->ent.entry->phar->is_modified = 1;
	phar_flush(entry_obj->ent.entry->phar, 0, 0, 0, &error TSRMLS_CC);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
		efree(error);
	}
}
/* }}} */

/* {{{ proto bool PharFileInfo::delMetaData()
 * Deletes the metadata of the entry
 */
PHP_METHOD(PharFileInfo, delMetadata)
{
	char *error;

	PHAR_ENTRY_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (PHAR_G(readonly) && !entry_obj->ent.entry->phar->is_data) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Write operations disabled by the php.ini setting phar.readonly");
		return;
	}

	if (entry_obj->ent.entry->is_temp_dir) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, \
			"Phar entry is a temporary directory (not an actual entry in the archive), cannot delete metadata"); \
		return;
	}

	if (entry_obj->ent.entry->metadata) {
		if (entry_obj->ent.entry->is_persistent) {
			phar_archive_data *phar = entry_obj->ent.entry->phar;

			if (FAILURE == phar_copy_on_write(&phar TSRMLS_CC)) {
				zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar->fname);
				return;
			}
			/* re-populate after copy-on-write */
			zend_hash_find(&phar->manifest, entry_obj->ent.entry->filename, entry_obj->ent.entry->filename_len, (void **)&entry_obj->ent.entry);
		}
		zval_ptr_dtor(&entry_obj->ent.entry->metadata);
		entry_obj->ent.entry->metadata = NULL;
		entry_obj->ent.entry->is_modified = 1;
		entry_obj->ent.entry->phar->is_modified = 1;

		phar_flush(entry_obj->ent.entry->phar, 0, 0, 0, &error TSRMLS_CC);

		if (error) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
			efree(error);
			RETURN_FALSE;
		} else {
			RETURN_TRUE;
		}

	} else {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto string PharFileInfo::getContent()
 * return the complete file contents of the entry (like file_get_contents)
 */
PHP_METHOD(PharFileInfo, getContent)
{
	char *error;
	php_stream *fp;
	phar_entry_info *link;

	PHAR_ENTRY_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (entry_obj->ent.entry->is_dir) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Phar error: Cannot retrieve contents, \"%s\" in phar \"%s\" is a directory", entry_obj->ent.entry->filename, entry_obj->ent.entry->phar->fname);
		return;
	}

	link = phar_get_link_source(entry_obj->ent.entry TSRMLS_CC);

	if (!link) {
		link = entry_obj->ent.entry;
	}

	if (SUCCESS != phar_open_entry_fp(link, &error, 0 TSRMLS_CC)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Phar error: Cannot retrieve contents, \"%s\" in phar \"%s\": %s", entry_obj->ent.entry->filename, entry_obj->ent.entry->phar->fname, error);
		efree(error);
		return;
	}

	if (!(fp = phar_get_efp(link, 0 TSRMLS_CC))) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Phar error: Cannot retrieve contents of \"%s\" in phar \"%s\"", entry_obj->ent.entry->filename, entry_obj->ent.entry->phar->fname);
		return;
	}

	phar_seek_efp(link, 0, SEEK_SET, 0, 0 TSRMLS_CC);
	Z_TYPE_P(return_value) = IS_STRING;
	Z_STRLEN_P(return_value) = php_stream_copy_to_mem(fp, &(Z_STRVAL_P(return_value)), link->uncompressed_filesize, 0);

	if (!Z_STRVAL_P(return_value)) {
		Z_STRVAL_P(return_value) = estrndup("", 0);
	}
}
/* }}} */

/* {{{ proto int PharFileInfo::compress(int compression_type)
 * Instructs the Phar class to compress the current file using zlib or bzip2 compression
 */
PHP_METHOD(PharFileInfo, compress)
{
	long method;
	char *error;
	PHAR_ENTRY_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &method) == FAILURE) {
		return;
	}

	if (entry_obj->ent.entry->is_tar) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Cannot compress with Gzip compression, not possible with tar-based phar archives");
		return;
	}

	if (entry_obj->ent.entry->is_dir) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, \
			"Phar entry is a directory, cannot set compression"); \
		return;
	}

	if (PHAR_G(readonly) && !entry_obj->ent.entry->phar->is_data) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Phar is readonly, cannot change compression");
		return;
	}

	if (entry_obj->ent.entry->is_deleted) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Cannot compress deleted file");
		return;
	}

	if (entry_obj->ent.entry->is_persistent) {
		phar_archive_data *phar = entry_obj->ent.entry->phar;

		if (FAILURE == phar_copy_on_write(&phar TSRMLS_CC)) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar->fname);
			return;
		}
		/* re-populate after copy-on-write */
		zend_hash_find(&phar->manifest, entry_obj->ent.entry->filename, entry_obj->ent.entry->filename_len, (void **)&entry_obj->ent.entry);
	}
	switch (method) {
		case PHAR_ENT_COMPRESSED_GZ:
			if (entry_obj->ent.entry->flags & PHAR_ENT_COMPRESSED_GZ) {
				RETURN_TRUE;
			}

			if ((entry_obj->ent.entry->flags & PHAR_ENT_COMPRESSED_BZ2) != 0) {
				if (!PHAR_G(has_bz2)) {
					zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
						"Cannot compress with gzip compression, file is already compressed with bzip2 compression and bz2 extension is not enabled, cannot decompress");
					return;
				}

				/* decompress this file indirectly */
				if (SUCCESS != phar_open_entry_fp(entry_obj->ent.entry, &error, 1 TSRMLS_CC)) {
					zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
						"Phar error: Cannot decompress bzip2-compressed file \"%s\" in phar \"%s\" in order to compress with gzip: %s", entry_obj->ent.entry->filename, entry_obj->ent.entry->phar->fname, error);
					efree(error);
					return;
				}
			}

			if (!PHAR_G(has_zlib)) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
					"Cannot compress with gzip compression, zlib extension is not enabled");
				return;
			}

			entry_obj->ent.entry->old_flags = entry_obj->ent.entry->flags;
			entry_obj->ent.entry->flags &= ~PHAR_ENT_COMPRESSION_MASK;
			entry_obj->ent.entry->flags |= PHAR_ENT_COMPRESSED_GZ;
			break;
		case PHAR_ENT_COMPRESSED_BZ2:
			if (entry_obj->ent.entry->flags & PHAR_ENT_COMPRESSED_BZ2) {
				RETURN_TRUE;
			}

			if ((entry_obj->ent.entry->flags & PHAR_ENT_COMPRESSED_GZ) != 0) {
				if (!PHAR_G(has_zlib)) {
					zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
						"Cannot compress with bzip2 compression, file is already compressed with gzip compression and zlib extension is not enabled, cannot decompress");
					return;
				}

				/* decompress this file indirectly */
				if (SUCCESS != phar_open_entry_fp(entry_obj->ent.entry, &error, 1 TSRMLS_CC)) {
					zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
						"Phar error: Cannot decompress gzip-compressed file \"%s\" in phar \"%s\" in order to compress with bzip2: %s", entry_obj->ent.entry->filename, entry_obj->ent.entry->phar->fname, error);
					efree(error);
					return;
				}
			}

			if (!PHAR_G(has_bz2)) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
					"Cannot compress with bzip2 compression, bz2 extension is not enabled");
				return;
			}
			entry_obj->ent.entry->old_flags = entry_obj->ent.entry->flags;
			entry_obj->ent.entry->flags &= ~PHAR_ENT_COMPRESSION_MASK;
			entry_obj->ent.entry->flags |= PHAR_ENT_COMPRESSED_BZ2;
			break;
		default:
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, \
				"Unknown compression type specified"); \
	}

	entry_obj->ent.entry->phar->is_modified = 1;
	entry_obj->ent.entry->is_modified = 1;
	phar_flush(entry_obj->ent.entry->phar, 0, 0, 0, &error TSRMLS_CC);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
		efree(error);
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int PharFileInfo::decompress()
 * Instructs the Phar class to decompress the current file
 */
PHP_METHOD(PharFileInfo, decompress)
{
	char *error;
	PHAR_ENTRY_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (entry_obj->ent.entry->is_dir) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, \
			"Phar entry is a directory, cannot set compression"); \
		return;
	}

	if ((entry_obj->ent.entry->flags & PHAR_ENT_COMPRESSION_MASK) == 0) {
		RETURN_TRUE;
	}

	if (PHAR_G(readonly) && !entry_obj->ent.entry->phar->is_data) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Phar is readonly, cannot decompress");
		return;
	}

	if (entry_obj->ent.entry->is_deleted) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Cannot compress deleted file");
		return;
	}

	if ((entry_obj->ent.entry->flags & PHAR_ENT_COMPRESSED_GZ) != 0 && !PHAR_G(has_zlib)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Cannot decompress Gzip-compressed file, zlib extension is not enabled");
		return;
	}

	if ((entry_obj->ent.entry->flags & PHAR_ENT_COMPRESSED_BZ2) != 0 && !PHAR_G(has_bz2)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Cannot decompress Bzip2-compressed file, bz2 extension is not enabled");
		return;
	}

	if (entry_obj->ent.entry->is_persistent) {
		phar_archive_data *phar = entry_obj->ent.entry->phar;

		if (FAILURE == phar_copy_on_write(&phar TSRMLS_CC)) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar->fname);
			return;
		}
		/* re-populate after copy-on-write */
		zend_hash_find(&phar->manifest, entry_obj->ent.entry->filename, entry_obj->ent.entry->filename_len, (void **)&entry_obj->ent.entry);
	}
	if (!entry_obj->ent.entry->fp) {
		if (FAILURE == phar_open_archive_fp(entry_obj->ent.entry->phar TSRMLS_CC)) {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Cannot decompress entry \"%s\", phar error: Cannot open phar archive \"%s\" for reading", entry_obj->ent.entry->filename, entry_obj->ent.entry->phar->fname);
			return;
		}
		entry_obj->ent.entry->fp_type = PHAR_FP;
	}

	entry_obj->ent.entry->old_flags = entry_obj->ent.entry->flags;
	entry_obj->ent.entry->flags &= ~PHAR_ENT_COMPRESSION_MASK;
	entry_obj->ent.entry->phar->is_modified = 1;
	entry_obj->ent.entry->is_modified = 1;
	phar_flush(entry_obj->ent.entry->phar, 0, 0, 0, &error TSRMLS_CC);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
		efree(error);
	}
	RETURN_TRUE;
}
/* }}} */

#endif /* HAVE_SPL */

/* {{{ phar methods */
PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, alias)
	ZEND_ARG_INFO(0, fileformat)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_createDS, 0, 0, 0)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, webindex)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_cancompress, 0, 0, 0)
	ZEND_ARG_INFO(0, method)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_isvalidpharfilename, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, executable)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_loadPhar, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, alias)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_mapPhar, 0, 0, 0)
	ZEND_ARG_INFO(0, alias)
	ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_mount, 0, 0, 2)
	ZEND_ARG_INFO(0, inphar)
	ZEND_ARG_INFO(0, externalfile)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_mungServer, 0, 0, 1)
	ZEND_ARG_INFO(0, munglist)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_webPhar, 0, 0, 0)
	ZEND_ARG_INFO(0, alias)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, f404)
	ZEND_ARG_INFO(0, mimetypes)
	ZEND_ARG_INFO(0, rewrites)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_running, 0, 0, 1)
	ZEND_ARG_INFO(0, retphar)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_ua, 0, 0, 1)
	ZEND_ARG_INFO(0, archive)
ZEND_END_ARG_INFO()

#if HAVE_SPL
PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_build, 0, 0, 1)
	ZEND_ARG_INFO(0, iterator)
	ZEND_ARG_INFO(0, base_directory)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_conv, 0, 0, 0)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, compression_type)
	ZEND_ARG_INFO(0, file_ext)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_comps, 0, 0, 1)
	ZEND_ARG_INFO(0, compression_type)
	ZEND_ARG_INFO(0, file_ext)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_decomp, 0, 0, 0)
	ZEND_ARG_INFO(0, file_ext)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_comp, 0, 0, 1)
	ZEND_ARG_INFO(0, compression_type)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_compo, 0, 0, 0)
	ZEND_ARG_INFO(0, compression_type)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_copy, 0, 0, 2)
	ZEND_ARG_INFO(0, newfile)
	ZEND_ARG_INFO(0, oldfile)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_delete, 0, 0, 1)
	ZEND_ARG_INFO(0, entry)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_fromdir, 0, 0, 1)
	ZEND_ARG_INFO(0, base_dir)
	ZEND_ARG_INFO(0, regex)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_offsetExists, 0, 0, 1)
	ZEND_ARG_INFO(0, entry)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_offsetSet, 0, 0, 2)
	ZEND_ARG_INFO(0, entry)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_setAlias, 0, 0, 1)
	ZEND_ARG_INFO(0, alias)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_setMetadata, 0, 0, 1)
	ZEND_ARG_INFO(0, metadata)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_setSigAlgo, 0, 0, 1)
	ZEND_ARG_INFO(0, algorithm)
	ZEND_ARG_INFO(0, privatekey)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_setStub, 0, 0, 1)
	ZEND_ARG_INFO(0, newstub)
	ZEND_ARG_INFO(0, maxlen)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_emptydir, 0, 0, 0)
	ZEND_ARG_INFO(0, dirname)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_extract, 0, 0, 1)
	ZEND_ARG_INFO(0, pathto)
	ZEND_ARG_INFO(0, files)
	ZEND_ARG_INFO(0, overwrite)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_addfile, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, localname)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_fromstring, 0, 0, 1)
	ZEND_ARG_INFO(0, localname)
	ZEND_ARG_INFO(0, contents)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_phar_isff, 0, 0, 1)
	ZEND_ARG_INFO(0, fileformat)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO(arginfo_phar__void, 0)
ZEND_END_ARG_INFO()


#endif /* HAVE_SPL */

zend_function_entry php_archive_methods[] = {
#if !HAVE_SPL
	PHP_ME(Phar, __construct,           arginfo_phar___construct,  ZEND_ACC_PRIVATE)
#else
	PHP_ME(Phar, __construct,           arginfo_phar___construct,  ZEND_ACC_PUBLIC)
	PHP_ME(Phar, __destruct,            arginfo_phar__void,        ZEND_ACC_PUBLIC)
	PHP_ME(Phar, addEmptyDir,           arginfo_phar_emptydir,     ZEND_ACC_PUBLIC)
	PHP_ME(Phar, addFile,               arginfo_phar_addfile,      ZEND_ACC_PUBLIC)
	PHP_ME(Phar, addFromString,         arginfo_phar_fromstring,   ZEND_ACC_PUBLIC)
	PHP_ME(Phar, buildFromDirectory,    arginfo_phar_fromdir,      ZEND_ACC_PUBLIC)
	PHP_ME(Phar, buildFromIterator,     arginfo_phar_build,        ZEND_ACC_PUBLIC)
	PHP_ME(Phar, compressFiles,         arginfo_phar_comp,         ZEND_ACC_PUBLIC)
	PHP_ME(Phar, decompressFiles,       arginfo_phar__void,        ZEND_ACC_PUBLIC)
	PHP_ME(Phar, compress,              arginfo_phar_comps,        ZEND_ACC_PUBLIC)
	PHP_ME(Phar, decompress,            arginfo_phar_decomp,       ZEND_ACC_PUBLIC)
	PHP_ME(Phar, convertToExecutable,   arginfo_phar_conv,         ZEND_ACC_PUBLIC)
	PHP_ME(Phar, convertToData,         arginfo_phar_conv,         ZEND_ACC_PUBLIC)
	PHP_ME(Phar, copy,                  arginfo_phar_copy,         ZEND_ACC_PUBLIC)
	PHP_ME(Phar, count,                 arginfo_phar__void,        ZEND_ACC_PUBLIC)
	PHP_ME(Phar, delete,                arginfo_phar_delete,       ZEND_ACC_PUBLIC)
	PHP_ME(Phar, delMetadata,           arginfo_phar__void,        ZEND_ACC_PUBLIC)
	PHP_ME(Phar, extractTo,             arginfo_phar_extract,      ZEND_ACC_PUBLIC)
	PHP_ME(Phar, getAlias,              arginfo_phar__void,        ZEND_ACC_PUBLIC)
	PHP_ME(Phar, getPath,               arginfo_phar__void,        ZEND_ACC_PUBLIC)
	PHP_ME(Phar, getMetadata,           arginfo_phar__void,        ZEND_ACC_PUBLIC)
	PHP_ME(Phar, getModified,           arginfo_phar__void,        ZEND_ACC_PUBLIC)
	PHP_ME(Phar, getSignature,          arginfo_phar__void,        ZEND_ACC_PUBLIC)
	PHP_ME(Phar, getStub,               arginfo_phar__void,        ZEND_ACC_PUBLIC)
	PHP_ME(Phar, getVersion,            arginfo_phar__void,        ZEND_ACC_PUBLIC)
	PHP_ME(Phar, hasMetadata,           arginfo_phar__void,        ZEND_ACC_PUBLIC)
	PHP_ME(Phar, isBuffering,           arginfo_phar__void,        ZEND_ACC_PUBLIC)
	PHP_ME(Phar, isCompressed,          arginfo_phar__void,        ZEND_ACC_PUBLIC)
	PHP_ME(Phar, isFileFormat,          arginfo_phar_isff,         ZEND_ACC_PUBLIC)
	PHP_ME(Phar, isWritable,            arginfo_phar__void,        ZEND_ACC_PUBLIC)
	PHP_ME(Phar, offsetExists,          arginfo_phar_offsetExists, ZEND_ACC_PUBLIC)
	PHP_ME(Phar, offsetGet,             arginfo_phar_offsetExists, ZEND_ACC_PUBLIC)
	PHP_ME(Phar, offsetSet,             arginfo_phar_offsetSet,    ZEND_ACC_PUBLIC)
	PHP_ME(Phar, offsetUnset,           arginfo_phar_offsetExists, ZEND_ACC_PUBLIC)
	PHP_ME(Phar, setAlias,              arginfo_phar_setAlias,     ZEND_ACC_PUBLIC)
	PHP_ME(Phar, setDefaultStub,        arginfo_phar_createDS,     ZEND_ACC_PUBLIC)
	PHP_ME(Phar, setMetadata,           arginfo_phar_setMetadata,  ZEND_ACC_PUBLIC)
	PHP_ME(Phar, setSignatureAlgorithm, arginfo_phar_setSigAlgo,   ZEND_ACC_PUBLIC)
	PHP_ME(Phar, setStub,               arginfo_phar_setStub,      ZEND_ACC_PUBLIC)
	PHP_ME(Phar, startBuffering,        arginfo_phar__void,        ZEND_ACC_PUBLIC)
	PHP_ME(Phar, stopBuffering,         arginfo_phar__void,        ZEND_ACC_PUBLIC)
#endif
	/* static member functions */
	PHP_ME(Phar, apiVersion,            arginfo_phar__void,        ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(Phar, canCompress,           arginfo_phar_cancompress,  ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(Phar, canWrite,              arginfo_phar__void,        ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(Phar, createDefaultStub,     arginfo_phar_createDS,     ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(Phar, getSupportedCompression,arginfo_phar__void,       ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(Phar, getSupportedSignatures,arginfo_phar__void,        ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(Phar, interceptFileFuncs,    arginfo_phar__void,        ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(Phar, isValidPharFilename,   arginfo_phar_isvalidpharfilename, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(Phar, loadPhar,              arginfo_phar_loadPhar,     ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(Phar, mapPhar,               arginfo_phar_mapPhar,      ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(Phar, running,               arginfo_phar_running,      ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(Phar, mount,                 arginfo_phar_mount,        ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(Phar, mungServer,            arginfo_phar_mungServer,   ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(Phar, unlinkArchive,         arginfo_phar_ua,           ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_ME(Phar, webPhar,               arginfo_phar_webPhar,      ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
	PHP_FE_END
};

#if HAVE_SPL
PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_entry___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

PHAR_ARG_INFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_entry_chmod, 0, 0, 1)
	ZEND_ARG_INFO(0, perms)
ZEND_END_ARG_INFO()

zend_function_entry php_entry_methods[] = {
	PHP_ME(PharFileInfo, __construct,        arginfo_entry___construct,  ZEND_ACC_PUBLIC)
	PHP_ME(PharFileInfo, __destruct,         arginfo_phar__void,         ZEND_ACC_PUBLIC)
	PHP_ME(PharFileInfo, chmod,              arginfo_entry_chmod,        ZEND_ACC_PUBLIC)
	PHP_ME(PharFileInfo, compress,           arginfo_phar_comp,          ZEND_ACC_PUBLIC)
	PHP_ME(PharFileInfo, decompress,         arginfo_phar__void,         ZEND_ACC_PUBLIC)
	PHP_ME(PharFileInfo, delMetadata,        arginfo_phar__void,         ZEND_ACC_PUBLIC)
	PHP_ME(PharFileInfo, getCompressedSize,  arginfo_phar__void,         ZEND_ACC_PUBLIC)
	PHP_ME(PharFileInfo, getCRC32,           arginfo_phar__void,         ZEND_ACC_PUBLIC)
	PHP_ME(PharFileInfo, getContent,         arginfo_phar__void,         ZEND_ACC_PUBLIC)
	PHP_ME(PharFileInfo, getMetadata,        arginfo_phar__void,         ZEND_ACC_PUBLIC)
	PHP_ME(PharFileInfo, getPharFlags,       arginfo_phar__void,         ZEND_ACC_PUBLIC)
	PHP_ME(PharFileInfo, hasMetadata,        arginfo_phar__void,         ZEND_ACC_PUBLIC)
	PHP_ME(PharFileInfo, isCompressed,       arginfo_phar_compo,         ZEND_ACC_PUBLIC)
	PHP_ME(PharFileInfo, isCRCChecked,       arginfo_phar__void,         ZEND_ACC_PUBLIC)
	PHP_ME(PharFileInfo, setMetadata,        arginfo_phar_setMetadata,   ZEND_ACC_PUBLIC)
	PHP_FE_END
};
#endif /* HAVE_SPL */

zend_function_entry phar_exception_methods[] = {
	PHP_FE_END
};
/* }}} */

#define REGISTER_PHAR_CLASS_CONST_LONG(class_name, const_name, value) \
	zend_declare_class_constant_long(class_name, const_name, sizeof(const_name)-1, (long)value TSRMLS_CC);

#define phar_exception_get_default() zend_exception_get_default(TSRMLS_C)

void phar_object_init(TSRMLS_D) /* {{{ */
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "PharException", phar_exception_methods);
	phar_ce_PharException = zend_register_internal_class_ex(&ce, phar_exception_get_default(), NULL  TSRMLS_CC);

#if HAVE_SPL
	INIT_CLASS_ENTRY(ce, "Phar", php_archive_methods);
	phar_ce_archive = zend_register_internal_class_ex(&ce, spl_ce_RecursiveDirectoryIterator, NULL  TSRMLS_CC);

	zend_class_implements(phar_ce_archive TSRMLS_CC, 2, spl_ce_Countable, zend_ce_arrayaccess);

	INIT_CLASS_ENTRY(ce, "PharData", php_archive_methods);
	phar_ce_data = zend_register_internal_class_ex(&ce, spl_ce_RecursiveDirectoryIterator, NULL  TSRMLS_CC);

	zend_class_implements(phar_ce_data TSRMLS_CC, 2, spl_ce_Countable, zend_ce_arrayaccess);

	INIT_CLASS_ENTRY(ce, "PharFileInfo", php_entry_methods);
	phar_ce_entry = zend_register_internal_class_ex(&ce, spl_ce_SplFileInfo, NULL  TSRMLS_CC);
#else
	INIT_CLASS_ENTRY(ce, "Phar", php_archive_methods);
	phar_ce_archive = zend_register_internal_class(&ce TSRMLS_CC);
	phar_ce_archive->ce_flags |= ZEND_ACC_FINAL_CLASS;

	INIT_CLASS_ENTRY(ce, "PharData", php_archive_methods);
	phar_ce_data = zend_register_internal_class(&ce TSRMLS_CC);
	phar_ce_data->ce_flags |= ZEND_ACC_FINAL_CLASS;
#endif

	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "BZ2", PHAR_ENT_COMPRESSED_BZ2)
	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "GZ", PHAR_ENT_COMPRESSED_GZ)
	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "NONE", PHAR_ENT_COMPRESSED_NONE)
	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "PHAR", PHAR_FORMAT_PHAR)
	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "TAR", PHAR_FORMAT_TAR)
	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "ZIP", PHAR_FORMAT_ZIP)
	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "COMPRESSED", PHAR_ENT_COMPRESSION_MASK)
	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "PHP", PHAR_MIME_PHP)
	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "PHPS", PHAR_MIME_PHPS)
	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "MD5", PHAR_SIG_MD5)
	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "OPENSSL", PHAR_SIG_OPENSSL)
	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "SHA1", PHAR_SIG_SHA1)
	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "SHA256", PHAR_SIG_SHA256)
	REGISTER_PHAR_CLASS_CONST_LONG(phar_ce_archive, "SHA512", PHAR_SIG_SHA512)
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
