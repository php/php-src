/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_FILE_CACHE_H
#define ZEND_FILE_CACHE_H

int zend_file_cache_script_store(zend_persistent_script *script, int in_shm);
zend_persistent_script *zend_file_cache_script_load(zend_file_handle *file_handle);
void zend_file_cache_invalidate(zend_string *full_path);

#endif /* ZEND_FILE_CACHE_H */
