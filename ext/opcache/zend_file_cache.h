/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
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
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_FILE_CACHE_H
#define ZEND_FILE_CACHE_H

typedef struct _zend_file_cache_handle {
	zend_string *full_path;
	zend_file_cache_metainfo info;
	void *mem, *checkpoint;
} zend_file_cache_handle;

int zend_file_cache_script_store(zend_persistent_script *script, bool in_shm);
zend_always_inline zend_result zend_file_cache_open(zend_file_handle *file_handle, zend_file_cache_handle *cache_handle);
zend_always_inline void zend_file_cache_close(zend_file_cache_handle *cache_handle);
zend_result zend_file_cache_validate(zend_file_handle *file_handle);
zend_persistent_script *zend_file_cache_script_load(zend_file_handle *file_handle);
void zend_file_cache_invalidate(zend_string *full_path);

#endif /* ZEND_FILE_CACHE_H */
