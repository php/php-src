/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_ACCELERATOR_UTIL_FUNCS_H
#define ZEND_ACCELERATOR_UTIL_FUNCS_H

#include "zend.h"
#include "ZendAccelerator.h"

void zend_accel_copy_internal_functions(TSRMLS_D);

zend_persistent_script* create_persistent_script(void);
int compact_persistent_script(zend_persistent_script *script);
void free_persistent_script(zend_persistent_script *persistent_script, int destroy_elements);

void zend_accel_free_user_functions(HashTable *ht TSRMLS_DC);
void zend_accel_move_user_functions(HashTable *str, HashTable *dst TSRMLS_DC);

zend_op_array* zend_accel_load_script(zend_persistent_script *persistent_script, int from_shared_memory TSRMLS_DC);

#define ADLER32_INIT 1     /* initial Adler-32 value */

unsigned int zend_adler32(unsigned int checksum, signed char *buf, uint len);

#endif /* ZEND_ACCELERATOR_UTIL_FUNCS_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
