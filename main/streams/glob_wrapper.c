/*
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
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "php_streams_int.h"

#ifdef HAVE_GLOB
# ifndef PHP_WIN32
#  include <glob.h>
# else
#  include "win32/glob.h"
# endif
#endif

#ifdef HAVE_GLOB
#ifndef GLOB_ONLYDIR
#define GLOB_ONLYDIR (1<<30)
#define GLOB_FLAGMASK (~GLOB_ONLYDIR)
#else
#define GLOB_FLAGMASK (~0)
#endif

typedef struct {
	glob_t   glob;
	size_t   index;
	int      flags;
	char     *path;
	size_t   path_len;
	char     *pattern;
	size_t   pattern_len;
	size_t   *open_basedir_indexmap;
	size_t   open_basedir_indexmap_size;
	bool     open_basedir_used;
} glob_s_t;

PHPAPI char* _php_glob_stream_get_path(php_stream *stream, size_t *plen STREAMS_DC) /* {{{ */
{
	glob_s_t *pglob = (glob_s_t *)stream->abstract;

	if (pglob && pglob->path) {
		if (plen) {
			*plen = pglob->path_len;
		}
		return pglob->path;
	} else {
		if (plen) {
			*plen = 0;
		}
		return NULL;
	}
}
/* }}} */

PHPAPI char* _php_glob_stream_get_pattern(php_stream *stream, size_t *plen STREAMS_DC) /* {{{ */
{
	glob_s_t *pglob = (glob_s_t *)stream->abstract;

	if (pglob && pglob->pattern) {
		if (plen) {
			*plen = pglob->pattern_len;
		}
		return pglob->pattern;
	} else {
		if (plen) {
			*plen = 0;
		}
		return NULL;
	}
}
/* }}} */

static inline int php_glob_stream_get_result_count(glob_s_t *pglob)
{
	return pglob->open_basedir_used ? (int) pglob->open_basedir_indexmap_size : pglob->glob.gl_pathc;
}

PHPAPI int _php_glob_stream_get_count(php_stream *stream, int *pflags STREAMS_DC) /* {{{ */
{
	glob_s_t *pglob = (glob_s_t *)stream->abstract;

	if (pglob) {
		if (pflags) {
			*pflags = pglob->flags;
		}
		return php_glob_stream_get_result_count(pglob);
	} else {
		if (pflags) {
			*pflags = 0;
		}
		return 0;
	}
}
/* }}} */

static void php_glob_stream_path_split(glob_s_t *pglob, const char *path, int get_path, const char **p_file) /* {{{ */
{
	const char *pos, *gpath = path;

	if ((pos = strrchr(path, '/')) != NULL) {
		path = pos+1;
	}
#ifdef PHP_WIN32
	if ((pos = strrchr(path, '\\')) != NULL) {
		path = pos+1;
	}
#endif

	*p_file = path;

	if (get_path) {
		if (pglob->path) {
			efree(pglob->path);
		}
		if ((path - gpath) > 1) {
			path--;
		}
		pglob->path_len = path - gpath;
		pglob->path = estrndup(gpath, pglob->path_len);
	}
}
/* }}} */

static ssize_t php_glob_stream_read(php_stream *stream, char *buf, size_t count) /* {{{ */
{
	glob_s_t *pglob = (glob_s_t *)stream->abstract;
	php_stream_dirent *ent = (php_stream_dirent*)buf;
	const char *path;
	int glob_result_count;
	size_t index;

	/* avoid problems if someone mis-uses the stream */
	if (count == sizeof(php_stream_dirent) && pglob) {
		glob_result_count = php_glob_stream_get_result_count(pglob);
		if (pglob->index < (size_t) glob_result_count) {
			index = pglob->open_basedir_used && pglob->open_basedir_indexmap ?
					pglob->open_basedir_indexmap[pglob->index] : pglob->index;
			php_glob_stream_path_split(pglob, pglob->glob.gl_pathv[index], pglob->flags & GLOB_APPEND, &path);
			++pglob->index;
			PHP_STRLCPY(ent->d_name, path, sizeof(ent->d_name), strlen(path));
			ent->d_type = DT_UNKNOWN;
			return sizeof(php_stream_dirent);
		}
		pglob->index = glob_result_count;
		if (pglob->path) {
			efree(pglob->path);
			pglob->path = NULL;
		}
	}

	return -1;
}
/* }}} */

static int php_glob_stream_close(php_stream *stream, int close_handle)  /* {{{ */
{
	glob_s_t *pglob = (glob_s_t *)stream->abstract;

	if (pglob) {
		pglob->index = 0;
		globfree(&pglob->glob);
		if (pglob->path) {
			efree(pglob->path);
		}
		if (pglob->pattern) {
			efree(pglob->pattern);
		}
		if (pglob->open_basedir_indexmap) {
			efree(pglob->open_basedir_indexmap);
		}
	}
	efree(stream->abstract);
	return 0;
}
/* {{{ */

static int php_glob_stream_rewind(php_stream *stream, zend_off_t offset, int whence, zend_off_t *newoffs) /* {{{ */
{
	glob_s_t *pglob = (glob_s_t *)stream->abstract;

	if (pglob) {
		pglob->index = 0;
		if (pglob->path) {
			efree(pglob->path);
			pglob->path = NULL;
		}
	}
	return 0;
}
/* }}} */

const php_stream_ops  php_glob_stream_ops = {
	NULL, php_glob_stream_read,
	php_glob_stream_close, NULL,
	"glob",
	php_glob_stream_rewind,
	NULL, /* cast */
	NULL, /* stat */
	NULL  /* set_option */
};

 /* {{{ php_glob_stream_opener */
static php_stream *php_glob_stream_opener(php_stream_wrapper *wrapper, const char *path, const char *mode,
		int options, zend_string **opened_path, php_stream_context *context STREAMS_DC)
{
	glob_s_t *pglob;
	int ret, i;
	const char *tmp, *pos;

	if (!strncmp(path, "glob://", sizeof("glob://")-1)) {
		path += sizeof("glob://")-1;
		if (opened_path) {
			*opened_path = zend_string_init(path, strlen(path), 0);
		}
	}

	pglob = ecalloc(1, sizeof(*pglob));

	if (0 != (ret = glob(path, pglob->flags & GLOB_FLAGMASK, NULL, &pglob->glob))) {
#ifdef GLOB_NOMATCH
		if (GLOB_NOMATCH != ret)
#endif
		{
			efree(pglob);
			return NULL;
		}
	}

	/* if open_basedir in use, check and filter restricted paths */
	if ((options & STREAM_DISABLE_OPEN_BASEDIR) == 0) {
		pglob->open_basedir_used = true;
		for (i = 0; i < pglob->glob.gl_pathc; i++) {
			if (!php_check_open_basedir_ex(pglob->glob.gl_pathv[i], 0)) {
				if (!pglob->open_basedir_indexmap) {
					pglob->open_basedir_indexmap = (size_t *) safe_emalloc(
							pglob->glob.gl_pathc, sizeof(size_t), 0);
				}
				pglob->open_basedir_indexmap[pglob->open_basedir_indexmap_size++] = i;
			}
		}
	}

	pos = path;
	if ((tmp = strrchr(pos, '/')) != NULL) {
		pos = tmp+1;
	}
#ifdef PHP_WIN32
	if ((tmp = strrchr(pos, '\\')) != NULL) {
		pos = tmp+1;
	}
#endif

	pglob->pattern_len = strlen(pos);
	pglob->pattern = estrndup(pos, pglob->pattern_len);

	pglob->flags |= GLOB_APPEND;

	if (pglob->glob.gl_pathc) {
		php_glob_stream_path_split(pglob, pglob->glob.gl_pathv[0], 1, &tmp);
	} else {
		php_glob_stream_path_split(pglob, path, 1, &tmp);
	}

	return php_stream_alloc(&php_glob_stream_ops, pglob, 0, mode);
}
/* }}} */

static const php_stream_wrapper_ops  php_glob_stream_wrapper_ops = {
	NULL,
	NULL,
	NULL,
	NULL,
	php_glob_stream_opener,
	"glob",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

const php_stream_wrapper  php_glob_stream_wrapper = {
	&php_glob_stream_wrapper_ops,
	NULL,
	0
};
#endif /* HAVE_GLOB */
