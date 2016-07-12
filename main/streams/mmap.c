/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@thebrainroom.com>                           |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

/* Memory Mapping interface for streams */
#include "php.h"
#include "php_streams_int.h"

PHPAPI char *_php_stream_mmap_range(php_stream *stream, size_t offset, size_t length, php_stream_mmap_access_t mode, size_t *mapped_len)
{
	php_stream_mmap_range range;

	range.offset = offset;
	range.length = length;
	range.mode = mode;
	range.mapped = NULL;

	/* For now, we impose an arbitrary limit to avoid
	 * runaway swapping when large files are passed thru. */
	if (length > 4 * 1024 * 1024) {
		return NULL;
	}

	if (PHP_STREAM_OPTION_RETURN_OK == php_stream_set_option(stream, PHP_STREAM_OPTION_MMAP_API, PHP_STREAM_MMAP_MAP_RANGE, &range)) {
		if (mapped_len) {
			*mapped_len = range.length;
		}
		return range.mapped;
	}
	return NULL;
}

PHPAPI int _php_stream_mmap_unmap(php_stream *stream)
{
	return php_stream_set_option(stream, PHP_STREAM_OPTION_MMAP_API, PHP_STREAM_MMAP_UNMAP, NULL) == PHP_STREAM_OPTION_RETURN_OK ? 1 : 0;
}

PHPAPI int _php_stream_mmap_unmap_ex(php_stream *stream, zend_off_t readden)
{
	int ret = 1;

	if (php_stream_seek(stream, readden, SEEK_CUR) != 0) {
		ret = 0;
	}
	if (php_stream_mmap_unmap(stream) == 0) {
		ret = 0;
	}

	return ret;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
