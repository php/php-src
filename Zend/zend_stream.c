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


#include "zend.h"
#include "zend_compile.h"

ZEND_DLIMPORT int isatty(int fd);

static size_t zend_stream_stdio_reader(void *handle, char *buf, size_t len TSRMLS_DC)
{
	return fread(buf, 1, len, (FILE*)handle);
}

static void zend_stream_stdio_closer(void *handle TSRMLS_DC)
{
	if ((FILE*)handle != stdin)
		fclose((FILE*)handle);
}

int zend_stream_open(const char *filename, zend_file_handle *handle TSRMLS_DC)
{
	if (zend_stream_open_function) {
		return zend_stream_open_function(filename, handle TSRMLS_CC);
	}
	handle->type = ZEND_HANDLE_FP;
	handle->opened_path = NULL;
	handle->handle.fp = zend_fopen(filename, &handle->opened_path);
	handle->filename = (char *)filename;
	handle->free_filename = 0;
	
	return (handle->handle.fp) ? SUCCESS : FAILURE;
}

int zend_stream_fixup(zend_file_handle *file_handle TSRMLS_DC)
{
	switch (file_handle->type) {
		case ZEND_HANDLE_FILENAME:
			if (FAILURE == zend_stream_open(file_handle->filename, file_handle TSRMLS_CC)) {
				return FAILURE;
			}
			break;
			
		case ZEND_HANDLE_FD:
			file_handle->handle.fp = fdopen(file_handle->handle.fd, "rb");
			file_handle->type = ZEND_HANDLE_FP;
			break;
			
		case ZEND_HANDLE_FP:
			file_handle->handle.fp = file_handle->handle.fp;
			break;
			
		case ZEND_HANDLE_STREAM:
			/* nothing to do */
			return SUCCESS;
			
		default:
			return FAILURE;
	}
	if (file_handle->type == ZEND_HANDLE_FP) {
		if (!file_handle->handle.fp) {
			return FAILURE;
		}

		/* promote to stream */
		file_handle->handle.stream.handle = file_handle->handle.fp;
		file_handle->handle.stream.reader = zend_stream_stdio_reader;
		file_handle->handle.stream.closer = zend_stream_stdio_closer;
		file_handle->type = ZEND_HANDLE_STREAM;

		file_handle->handle.stream.interactive = isatty(fileno((FILE *)file_handle->handle.stream.handle));
	}
	return SUCCESS;
}

size_t zend_stream_read(zend_file_handle *file_handle, char *buf, size_t len TSRMLS_DC)
{
	if (file_handle->handle.stream.interactive) {
		int c = '*', n; 

		for ( n = 0; n < len && (c = zend_stream_getc( file_handle TSRMLS_CC)) != EOF && c != '\n'; ++n ) 
			buf[n] = (char) c; 
		if ( c == '\n' )
			buf[n++] = (char) c; 

		return n;
	}
	return file_handle->handle.stream.reader(file_handle->handle.stream.handle, buf, len TSRMLS_CC);
}

int zend_stream_getc(zend_file_handle *file_handle TSRMLS_DC)
{
	char buf;

	if (file_handle->handle.stream.reader(file_handle->handle.stream.handle, &buf, sizeof(buf) TSRMLS_CC)) {
		return (int)buf;
	}
	return EOF;
}

int zend_stream_ferror(zend_file_handle *file_handle TSRMLS_DC)
{
	return 0;
}


