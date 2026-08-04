/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_FILE_CACHE_H
#define ZEND_FILE_CACHE_H

int zend_file_cache_script_store(zend_persistent_script *script, bool in_shm);
zend_persistent_script *zend_file_cache_script_load(zend_file_handle *file_handle);
zend_persistent_script *zend_file_cache_script_load_ex(zend_file_handle *file_handle, bool validate_only);
void zend_file_cache_invalidate(zend_string *full_path);

#endif /* ZEND_FILE_CACHE_H */
