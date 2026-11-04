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

#ifndef COMMON_DATE_H
#define	COMMON_DATE_H

#include <unicode/umachine.h>

#include <php.h>
#include "../intl_error.h"

#ifdef __cplusplus

// TODO once C++ migration done we can drop this workaround
#undef U_SHOW_CPLUSPLUS_API
#define U_SHOW_CPLUSPLUS_API 1
#include <unicode/timezone.h>

using icu::TimeZone;

TimeZone *timezone_convert_datetimezone(int type, void *object, bool is_datetime, intl_error *outside_error);
zend_result intl_datetime_decompose(zend_object *obj, double *millis, TimeZone **tz, intl_error *err);

#endif

double intl_zval_to_millis(zval *z, intl_error *err);

#endif	/* COMMON_DATE_H */
