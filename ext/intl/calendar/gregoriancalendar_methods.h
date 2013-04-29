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
   | Authors: Gustavo Lopes <cataphract@php.net>                          |
   +----------------------------------------------------------------------+
 */

#ifndef GREORIANCALENDAR_METHODS_H
#define GREORIANCALENDAR_METHODS_H

#include <php.h>

PHP_FUNCTION(intlgregcal_create_instance);

PHP_METHOD(IntlGregorianCalendar, __construct);

PHP_FUNCTION(intlgregcal_set_gregorian_change);

PHP_FUNCTION(intlgregcal_get_gregorian_change);

PHP_FUNCTION(intlgregcal_is_leap_year);

#endif
