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

#ifndef ZEND_ACCELERATOR_UTIL_FUNCS_H
#define ZEND_ACCELERATOR_UTIL_FUNCS_H

#include "zend.h"
#include "ZendAccelerator.h"

BEGIN_EXTERN_C()

zend_persistent_script* create_persistent_script(void);
void free_persistent_script(zend_persistent_script *persistent_script, bool destroy_elements);

void zend_accel_move_user_functions(HashTable *str, uint32_t count, zend_script *script);
void zend_accel_move_user_classes(HashTable *str, uint32_t count, zend_script *script);
void zend_accel_build_delayed_early_binding_list(zend_persistent_script *persistent_script);
void zend_accel_finalize_delayed_early_binding_list(const zend_persistent_script *persistent_script);
void zend_accel_free_delayed_early_binding_list(zend_persistent_script *persistent_script);

zend_op_array* zend_accel_load_script(zend_persistent_script *persistent_script, bool from_shared_memory);

#define ADLER32_INIT 1     /* initial Adler-32 value */

unsigned int zend_adler32(unsigned int checksum, unsigned char *buf, uint32_t len);

unsigned int zend_accel_script_checksum(zend_persistent_script *persistent_script);

END_EXTERN_C()

#endif /* ZEND_ACCELERATOR_UTIL_FUNCS_H */
