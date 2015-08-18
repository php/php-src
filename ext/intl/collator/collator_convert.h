/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Vadim Savchuk <vsavchuk@productengine.com>                  |
   |          Dmitry Lakhtyuk <dlakhtyuk@productengine.com>               |
   +----------------------------------------------------------------------+
 */

#ifndef COLLATOR_CONVERT_H
#define COLLATOR_CONVERT_H

#include <php.h>
#include <unicode/utypes.h>

void collator_convert_hash_from_utf8_to_utf16( HashTable* hash, UErrorCode* status );
void collator_convert_hash_from_utf16_to_utf8( HashTable* hash, UErrorCode* status );

zval* collator_convert_zstr_utf16_to_utf8( zval* utf16_zval, zval *rv );
zval* collator_convert_zstr_utf8_to_utf16( zval* utf8_zval, zval *rv );

zval* collator_normalize_sort_argument( zval* arg, zval *rv );
zval* collator_convert_object_to_string( zval* obj, zval *rv );
zval* collator_convert_string_to_number( zval* arg, zval *rv );
zval* collator_convert_string_to_number_if_possible( zval* str, zval *rv );
zval* collator_convert_string_to_double( zval* str, zval *rv );

zval* collator_make_printable_zval( zval* arg, zval *rv );

#endif // COLLATOR_CONVERT_H
