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
   | Authors: Kirti Velankar <kirtig@yahoo-inc.com>                       |
   +----------------------------------------------------------------------+
*/
#ifndef DATE_FORMAT_ATTR_H
#define DATE_FORMAT_ATTR_H

#include <php.h>

//PHP_FUNCTION( datefmt_get_timezone );
PHP_FUNCTION( datefmt_get_datetype );
PHP_FUNCTION( datefmt_get_timetype );
PHP_FUNCTION( datefmt_get_calendar );
PHP_FUNCTION( datefmt_set_calendar );
PHP_FUNCTION( datefmt_get_locale );
PHP_FUNCTION( datefmt_get_timezone_id );
PHP_FUNCTION( datefmt_set_timezone_id );
PHP_FUNCTION( datefmt_get_pattern );
PHP_FUNCTION( datefmt_set_pattern );
PHP_FUNCTION( datefmt_is_lenient );
PHP_FUNCTION( datefmt_set_lenient );

#endif // DATE_FORMAT_ATTR_H
