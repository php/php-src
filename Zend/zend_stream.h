/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2003 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Wez Furlong <wez@thebrainroom.com>                          |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_STREAM_H
#define ZEND_STREAM_H

/* Lightweight stream implementation for the ZE scanners.
 * These functions are private to the engine.
 * */

typedef size_t (*zend_stream_reader_t)(void *handle, char *buf, size_t len TSRMLS_DC);
typedef void (*zend_stream_closer_t)(void *handle TSRMLS_DC);

typedef struct _zend_stream {
	void *handle;
	zend_stream_reader_t reader;
	zend_stream_closer_t closer;
	int interactive;
} zend_stream;

typedef struct _zend_file_handle {
	zend_uchar type;
	char *filename;
	char *opened_path;
	union {
		int fd;
		FILE *fp;
		zend_stream stream;
	} handle;
	zend_bool free_filename;
} zend_file_handle;

int zend_stream_open(const char *filename, zend_file_handle *handle TSRMLS_DC);
int zend_stream_ferror(zend_file_handle *file_handle TSRMLS_DC);
int zend_stream_getc(zend_file_handle *file_handle TSRMLS_DC);
size_t zend_stream_read(zend_file_handle *file_handle, char *buf, size_t len TSRMLS_DC);
int zend_stream_fixup(zend_file_handle *file_handle TSRMLS_DC);

#define zend_stream_close(handle)	zend_file_handle_dtor((handle))

#endif

