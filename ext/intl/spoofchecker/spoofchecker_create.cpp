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
   | Authors: Scott MacVicar <scottmac@php.net>                           |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if __cplusplus >= 201703L
#include <string_view>
#include <unicode/unistr.h>
#endif

extern "C" {
#include "php_intl.h"
#include "intl_data.h"
}
#include "spoofchecker_class.h"

/* {{{ Spoofchecker object constructor. */
U_CFUNC PHP_METHOD(Spoofchecker, __construct)
{
	SPOOFCHECKER_METHOD_INIT_VARS;

	ZEND_PARSE_PARAMETERS_NONE();

	SPOOFCHECKER_METHOD_FETCH_OBJECT_NO_CHECK;
	if (co->uspoof) {
		zend_throw_error(NULL, "Spoofchecker object is already constructed");
		RETURN_THROWS();
	}

	co->uspoof = uspoof_open(SPOOFCHECKER_ERROR_CODE_P(co));
	if (U_FAILURE(INTL_DATA_ERROR_CODE(co))) {
		zend_throw_exception(IntlException_ce_ptr,
			"Spoofchecker::__construct(): unable to open ICU Spoof Checker", 0);
	}

	intl_icu_compat_uspoof_init_checker(co->uspoof, &co->uspoofres, SPOOFCHECKER_ERROR_CODE_P(co));
}
/* }}} */
