/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Andrey Hristov <andrey@php.net>                             |
  |          Ulf Wendel <uw@php.net>                                     |
  +----------------------------------------------------------------------+
*/

#include "php.h"
#include "mysqlnd.h"
#include "mysqlnd_debug.h"
#include "mysqlnd_read_buffer.h"


/* {{{ mysqlnd_read_buffer_is_empty */
static zend_bool
mysqlnd_read_buffer_is_empty(const MYSQLND_READ_BUFFER * const buffer)
{
	return buffer->len? FALSE:TRUE;
}
/* }}} */


/* {{{ mysqlnd_read_buffer_read */
static void
mysqlnd_read_buffer_read(MYSQLND_READ_BUFFER * buffer, const size_t count, zend_uchar * dest)
{
	if (buffer->len >= count) {
		memcpy(dest, buffer->data + buffer->offset, count);
		buffer->offset += count;
		buffer->len -= count;
	}
}
/* }}} */


/* {{{ mysqlnd_read_buffer_bytes_left */
static size_t
mysqlnd_read_buffer_bytes_left(const MYSQLND_READ_BUFFER * const buffer)
{
	return buffer->len;
}
/* }}} */


/* {{{ mysqlnd_read_buffer_free */
static void
mysqlnd_read_buffer_free(MYSQLND_READ_BUFFER ** buffer)
{
	DBG_ENTER("mysqlnd_read_buffer_free");
	if (*buffer) {
		mnd_efree((*buffer)->data);
		mnd_efree(*buffer);
		*buffer = NULL;
	}
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_create_read_buffer */
PHPAPI MYSQLND_READ_BUFFER *
mysqlnd_create_read_buffer(const size_t count)
{
	MYSQLND_READ_BUFFER * ret = mnd_emalloc(sizeof(MYSQLND_READ_BUFFER));
	DBG_ENTER("mysqlnd_create_read_buffer");
	ret->is_empty = mysqlnd_read_buffer_is_empty;
	ret->read = mysqlnd_read_buffer_read;
	ret->bytes_left = mysqlnd_read_buffer_bytes_left;
	ret->free_buffer = mysqlnd_read_buffer_free;
	ret->data = mnd_emalloc(count);
	ret->size = ret->len = count;
	ret->offset = 0;
	DBG_RETURN(ret);
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
