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

/* Memory Mapping interface for streams.
 * The intention is to provide a uniform interface over the most common
 * operations that are used within PHP itself, rather than a complete
 * API for all memory mapping needs.
 *
 * ATM, we support only mmap(), but win32 memory mapping support will
 * follow soon.
 * */

typedef enum {
	/* Does the stream support mmap ? */
	PHP_STREAM_MMAP_SUPPORTED,
	/* Request a range and offset to be mapped;
	 * while mapped, you MUST NOT use any read/write functions
	 * on the stream (win9x compatibility) */
	PHP_STREAM_MMAP_MAP_RANGE,
	/* Unmap the last range that was mapped for the stream */
	PHP_STREAM_MMAP_UNMAP
} php_stream_mmap_operation_t;

typedef enum {
	PHP_STREAM_MAP_MODE_READONLY,
	PHP_STREAM_MAP_MODE_READWRITE,
	PHP_STREAM_MAP_MODE_SHARED_READONLY,
	PHP_STREAM_MAP_MODE_SHARED_READWRITE
} php_stream_mmap_access_t;

typedef struct {
	/* requested offset and length.
	 * If length is 0, the whole file is mapped */
	size_t offset;
	size_t length;

	php_stream_mmap_access_t mode;

	/* returned mapped address */
	char *mapped;

} php_stream_mmap_range;

#define PHP_STREAM_MMAP_ALL 0

#define php_stream_mmap_supported(stream)	(_php_stream_set_option((stream), PHP_STREAM_OPTION_MMAP_API, PHP_STREAM_MMAP_SUPPORTED, NULL) == 0 ? 1 : 0)

/* Returns 1 if the stream in its current state can be memory mapped,
 * 0 otherwise */
#define php_stream_mmap_possible(stream)			(!php_stream_is_filtered((stream)) && php_stream_mmap_supported((stream)))

BEGIN_EXTERN_C()
PHPAPI char *_php_stream_mmap_range(php_stream *stream, size_t offset, size_t length, php_stream_mmap_access_t mode, size_t *mapped_len);
#define php_stream_mmap_range(stream, offset, length, mode, mapped_len)	_php_stream_mmap_range((stream), (offset), (length), (mode), (mapped_len))

/* un-maps the last mapped range */
PHPAPI int _php_stream_mmap_unmap(php_stream *stream);
#define php_stream_mmap_unmap(stream)				_php_stream_mmap_unmap((stream))

PHPAPI int _php_stream_mmap_unmap_ex(php_stream *stream, zend_off_t readden);
#define php_stream_mmap_unmap_ex(stream, readden)			_php_stream_mmap_unmap_ex((stream), (readden))
END_EXTERN_C()

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
