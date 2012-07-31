/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Ed Batutis <ed@batutis.com>                                 |
   +----------------------------------------------------------------------+
 */

#ifndef GRAPHEME_GRAPHEME_H
#define GRAPHEME_GRAPHEME_H

#include <php.h>
#include <unicode/utypes.h>
#include <unicode/ubrk.h>

PHP_FUNCTION(grapheme_strlen);
PHP_FUNCTION(grapheme_strpos);
PHP_FUNCTION(grapheme_stripos);
PHP_FUNCTION(grapheme_strrpos);
PHP_FUNCTION(grapheme_strripos);
PHP_FUNCTION(grapheme_substr);
PHP_FUNCTION(grapheme_strstr);
PHP_FUNCTION(grapheme_stristr);
PHP_FUNCTION(grapheme_extract);

void grapheme_register_constants( INIT_FUNC_ARGS );
void grapheme_close_global_iterator( TSRMLS_D );

#endif // GRAPHEME_GRAPHEME_H
