/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2015 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Wez Furlong <wez@thebrainroom.com>                          |
   |          Scott MacVicar <scottmac@php.net>                           |
   |          Nuno Lopes <nlopess@php.net>                                |
   |          Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_STREAM_H
#define ZEND_STREAM_H

#include <sys/types.h>
#include <sys/stat.h>

/* Lightweight stream implementation for the ZE scanners.
 * These functions are private to the engine.
 * */
typedef size_t (*zend_stream_fsizer_t)(void* handle);
typedef size_t (*zend_stream_reader_t)(void* handle, char *buf, size_t len);
typedef void   (*zend_stream_closer_t)(void* handle);

#define ZEND_MMAP_AHEAD 32

typedef enum {
	ZEND_HANDLE_FILENAME,
	ZEND_HANDLE_FD,
	ZEND_HANDLE_FP,
	ZEND_HANDLE_STREAM,
	ZEND_HANDLE_MAPPED
} zend_stream_type;

typedef struct _zend_mmap {
	size_t      len;
	size_t      pos;
	void        *map;
	char        *buf;
	void                  *old_handle;
	zend_stream_closer_t   old_closer;
} zend_mmap;

typedef struct _zend_stream {
	void        *handle;
	int         isatty;
	zend_mmap   mmap;
	zend_stream_reader_t   reader;
	zend_stream_fsizer_t   fsizer;
	zend_stream_closer_t   closer;
} zend_stream;

typedef struct _zend_file_handle {
	union {
		int           fd;
		FILE          *fp;
		zend_stream   stream;
	} handle;
	const char        *filename;
	zend_string       *opened_path;
	zend_stream_type  type;
	zend_bool free_filename;
} zend_file_handle;

BEGIN_EXTERN_C()
ZEND_API int zend_stream_open(const char *filename, zend_file_handle *handle);
ZEND_API int zend_stream_fixup(zend_file_handle *file_handle, char **buf, size_t *len);
ZEND_API void zend_file_handle_dtor(zend_file_handle *fh);
ZEND_API int zend_compare_file_handles(zend_file_handle *fh1, zend_file_handle *fh2);
END_EXTERN_C()

#ifdef _WIN64
# define zend_fseek _fseeki64
# define zend_ftell _ftelli64
# define zend_lseek _lseeki64
# define zend_fstat _fstat64
# define zend_stat  _stat64
typedef struct __stat64 zend_stat_t;
#else
# define zend_fseek fseek
# define zend_ftell ftell
# define zend_lseek lseek
# define zend_fstat fstat
# define zend_stat stat
typedef struct stat zend_stat_t;
#endif

#endif
