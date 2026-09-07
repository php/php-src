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
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_PERSIST_H
#define ZEND_PERSIST_H

BEGIN_EXTERN_C()

uint32_t zend_accel_script_persist_calc(zend_persistent_script *script, bool for_shm);
zend_persistent_script *zend_accel_script_persist(zend_persistent_script *script, bool for_shm);

void zend_persist_class_entry_calc(zend_class_entry *ce);
zend_class_entry *zend_persist_class_entry(zend_class_entry *ce);
void zend_update_parent_ce(zend_class_entry *ce);
void zend_persist_warnings_calc(uint32_t num_warnings, zend_error_info **warnings);
zend_error_info **zend_persist_warnings(uint32_t num_warnings, zend_error_info **warnings);

END_EXTERN_C()

#endif /* ZEND_PERSIST_H */
