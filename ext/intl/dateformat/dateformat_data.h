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
   | Authors: Kirti Velankar <kirtig@yahoo-inc.com>                       |
   +----------------------------------------------------------------------+
*/
#ifndef DATE_FORMAT_DATA_H
#define DATE_FORMAT_DATA_H

#include <php.h>

#include <unicode/udat.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "intl_error.h"
#ifdef __cplusplus
}
#endif

typedef struct {
	// error handling
	intl_error      error;

	// formatter handling
	UDateFormat *  udatf;
} dateformat_data;

#ifdef __cplusplus
extern "C" {
#endif
dateformat_data* dateformat_data_create( void );
void dateformat_data_init( dateformat_data* datef_data );
void dateformat_data_free( dateformat_data* datef_data );
#ifdef __cplusplus
}
#endif

#endif // DATE_FORMAT_DATA_H
