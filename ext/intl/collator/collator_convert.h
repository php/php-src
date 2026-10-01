/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Vadim Savchuk <vsavchuk@productengine.com>                  |
   |          Dmitry Lakhtyuk <dlakhtyuk@productengine.com>               |
   +----------------------------------------------------------------------+
 */

#ifndef COLLATOR_CONVERT_H
#define COLLATOR_CONVERT_H

#include <php.h>
#include <unicode/utypes.h>

#ifdef __cplusplus
extern "C" {
#endif
void collator_convert_hash_from_utf8_to_utf16( HashTable* hash, UErrorCode* status );
void collator_convert_hash_from_utf16_to_utf8( HashTable* hash, UErrorCode* status );

zval* collator_convert_zstr_utf16_to_utf8( zval* utf16_zval, zval *rv );
zend_string *collator_convert_zstr_utf8_to_utf16(zend_string *utf8_str);

zval* collator_normalize_sort_argument( zval* arg, zval *rv );
zval* collator_convert_object_to_string( zval* obj, zval *rv );
zval* collator_convert_string_to_number( zval* arg, zval *rv );
zval* collator_convert_string_to_number_if_possible( zval* str, zval *rv );
zval* collator_convert_string_to_double( zval* str, zval *rv );

zend_string *collator_zval_to_string(zval *arg);
#ifdef __cplusplus
}
#endif

#endif // COLLATOR_CONVERT_H
