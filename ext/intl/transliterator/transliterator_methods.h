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
   | Authors: Gustavo Lopes <cataphract@php.net>                          |
   +----------------------------------------------------------------------+
 */

#ifndef TRANSLITERATOR_METHODS_H
#define TRANSLITERATOR_METHODS_H

#include <php.h>

PHP_FUNCTION( transliterator_create );

PHP_FUNCTION( transliterator_create_from_rules );

PHP_FUNCTION( transliterator_list_ids );

PHP_FUNCTION( transliterator_create_inverse );

PHP_FUNCTION( transliterator_transliterate );

PHP_METHOD( Transliterator, __construct );

PHP_FUNCTION( transliterator_get_error_code );

PHP_FUNCTION( transliterator_get_error_message );

#endif /* #ifndef TRANSLITERATOR_METHODS_H */
