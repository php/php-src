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
   | Authors: Stanislav Malyshev <stas@zend.com>                          |
   +----------------------------------------------------------------------+
 */

#ifndef FORMATTER_ATTR_H
#define FORMATTER_ATTR_H

#include <php.h>

PHP_FUNCTION( numfmt_set_attribute );
PHP_FUNCTION( numfmt_get_attribute );
PHP_FUNCTION( numfmt_set_text_attribute );
PHP_FUNCTION( numfmt_get_text_attribute );
PHP_FUNCTION( numfmt_set_symbol );
PHP_FUNCTION( numfmt_get_symbol );
PHP_FUNCTION( numfmt_set_pattern );
PHP_FUNCTION( numfmt_get_pattern );
PHP_FUNCTION( numfmt_get_locale );

#endif // FORMATTER_ATTR_H
