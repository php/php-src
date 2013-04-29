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
   | Authors: Vadim Savchuk <vsavchuk@productengine.com>                  |
   |          Dmitry Lakhtyuk <dlakhtyuk@productengine.com>               |
   +----------------------------------------------------------------------+
 */

#ifndef INTL_COMMON_ERROR_H
#define INTL_COMMON_ERROR_H

#include <php.h>

PHP_FUNCTION( intl_get_error_code );
PHP_FUNCTION( intl_get_error_message );
PHP_FUNCTION( intl_is_failure );
PHP_FUNCTION( intl_error_name );

void intl_expose_icu_error_codes( INIT_FUNC_ARGS );

#endif // INTL_COMMON_ERROR_H
