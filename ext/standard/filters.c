/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors:                                                             |
   | Wez Furlong (wez@thebrainroom.com)                                   |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "php_globals.h"
#include "ext/standard/basic_functions.h"
#include "ext/standard/file.h"

/* {{{ rot13 stream filter implementation */
static char rot13_from[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
static char rot13_to[] = "nopqrstuvwxyzabcdefghijklmNOPQRSTUVWXYZABCDEFGHIJKLM";

static size_t strfilter_rot13_write(php_stream *stream, php_stream_filter *thisfilter,
			const char *buf, size_t count TSRMLS_DC)
{
	char rotbuf[1024];
	size_t chunk;
	size_t wrote = 0;

	while (count > 0) {
		chunk = count;
		if (chunk > sizeof(rotbuf))
			chunk = sizeof(rotbuf);

		PHP_STRLCPY(rotbuf, buf, sizeof(rotbuf), chunk);
		buf += chunk;
		count -= chunk;

		php_strtr(rotbuf, chunk, rot13_from, rot13_to, 52);
		wrote += php_stream_filter_write_next(stream, thisfilter, rotbuf, chunk);
	}

	return wrote;
}

static size_t strfilter_rot13_read(php_stream *stream, php_stream_filter *thisfilter,
			char *buf, size_t count TSRMLS_DC)
{
	size_t read;

	read = php_stream_filter_read_next(stream, thisfilter, buf, count);
	php_strtr(buf, read, rot13_from, rot13_to, 52);

	return read;
}

static int strfilter_rot13_flush(php_stream *stream, php_stream_filter *thisfilter, int closing TSRMLS_DC)
{
	return php_stream_filter_flush_next(stream, thisfilter, closing);
}

static int strfilter_rot13_eof(php_stream *stream, php_stream_filter *thisfilter TSRMLS_DC)
{
	return php_stream_filter_eof_next(stream, thisfilter);
}

static php_stream_filter_ops strfilter_rot13_ops = {
	strfilter_rot13_write,
	strfilter_rot13_read,
	strfilter_rot13_flush,
	strfilter_rot13_eof,
	NULL,
	"string.rot13"
};

static php_stream_filter *strfilter_rot13_create(const char *filtername, const char *filterparams,
		int filterparamslen, int persistent TSRMLS_DC)
{
	return php_stream_filter_alloc(&strfilter_rot13_ops, NULL, persistent);
}

static php_stream_filter_factory strfilter_rot13_factory = {
	strfilter_rot13_create
};
/* }}} */

static const struct {
	php_stream_filter_ops *ops;
	php_stream_filter_factory *factory;
} standard_filters[] = {
	{ &strfilter_rot13_ops, &strfilter_rot13_factory },
	{ NULL, NULL }
};

PHP_MINIT_FUNCTION(standard_filters)
{
	int i;

	for (i = 0; standard_filters[i].ops; i++) {
		if (FAILURE == php_stream_filter_register_factory(
					standard_filters[i].ops->label,
					standard_filters[i].factory
					TSRMLS_CC)) {
			return FAILURE;
		}
	}

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(standard_filters)
{
	int i;

	for (i = 0; standard_filters[i].ops; i++) {
		php_stream_filter_unregister_factory(standard_filters[i].ops->label TSRMLS_CC);
	}
	return SUCCESS;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
