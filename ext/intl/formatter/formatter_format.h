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
   | Authors: Stanislav Malyshev <stas@zend.com>                          |
   +----------------------------------------------------------------------+
 */

#ifndef FORMATTER_FORMAT_H
#define FORMATTER_FORMAT_H

#include <php.h>

PHP_FUNCTION( numfmt_format );
PHP_FUNCTION( numfmt_format_currency );

#define FORMAT_TYPE_DEFAULT	0
#define FORMAT_TYPE_INT32	1
#define FORMAT_TYPE_INT64	2 
#define FORMAT_TYPE_DOUBLE	3
#define FORMAT_TYPE_CURRENCY	4

#endif // FORMATTER_FORMAT_H
