/*
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Gustavo Lopes <cataphract@php.net>                          |
   +----------------------------------------------------------------------+
*/

#ifndef DATEFORMAT_HELPERS_H
#define	DATEFORMAT_HELPERS_H

#ifndef __cplusplus
#error For C++ only
#endif

#include <unicode/calendar.h>
#include <unicode/datefmt.h>

extern "C" {
#include "../php_intl.h"
}

using icu::Locale;
using icu::Calendar;
using icu::DateFormat;

int datefmt_process_calendar_arg(
	zend_object *calendar_obj, zend_long calendar_long, bool calendar_is_null, Locale const& locale,
	const char *func_name, intl_error *err, Calendar*& cal, zend_long& cal_int_type, bool& calendar_owned
);

#endif	/* DATEFORMAT_HELPERS_H */
