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
void free_persistent_script(zend_persistent_script *persistent_script, int destroy_elements);

void zend_accel_move_user_functions(HashTable *str, uint32_t count, zend_script *script);
void zend_accel_move_user_classes(HashTable *str, uint32_t count, zend_script *script);
void zend_accel_build_delayed_early_binding_list(zend_persistent_script *persistent_script);
void zend_accel_finalize_delayed_early_binding_list(zend_persistent_script *persistent_script);
void zend_accel_free_delayed_early_binding_list(zend_persistent_script *persistent_script);

zend_op_array* zend_accel_load_script(zend_persistent_script *persistent_script, int from_shared_memory);

#define ADLER32_INIT 1     /* initial Adler-32 value */

unsigned int zend_adler32(unsigned int checksum, unsigned char *buf, uint32_t len);

unsigned int zend_accel_script_checksum(zend_persistent_script *persistent_script);

END_EXTERN_C()

#endif /* ZEND_ACCELERATOR_UTIL_FUNCS_H */
