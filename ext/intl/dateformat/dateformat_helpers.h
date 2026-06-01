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

zend_result datefmt_process_calendar_arg(
	zend_object *calendar_obj, zend_long calendar_long, bool calendar_is_null, Locale const& locale,
	intl_error *err, Calendar*& cal, zend_long& cal_int_type, bool& calendar_owned
);

#endif	/* DATEFORMAT_HELPERS_H */
