/*
  +----------------------------------------------------------------------+
  | Copyright © The PHP Group and Contributors.                          |
  +----------------------------------------------------------------------+
  | This source file is subject to the Modified BSD License that is      |
  | bundled with this package in the file LICENSE, and is available      |
  | through the World Wide Web at <https://www.php.net/license/>.        |
  |                                                                      |
  | SPDX-License-Identifier: BSD-3-Clause                                |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@thebrainroom.com>                           |
  +----------------------------------------------------------------------+
*/

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
	return php_stream_set_option(stream, PHP_STREAM_OPTION_MMAP_API, PHP_STREAM_MMAP_UNMAP, NULL) == PHP_STREAM_OPTION_RETURN_OK;
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
