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
   | Authors: Stanislav Malyshev <stas@zend.com>                          |
   +----------------------------------------------------------------------+
 */

#ifndef FORMATTER_DATA_H
#define FORMATTER_DATA_H

#include <php.h>

#include <unicode/unum.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "intl_error.h"
#ifdef __cplusplus
}
#endif

typedef struct {
	// error hangling
	intl_error      error;

	// formatter handling
	UNumberFormat*  unum;
} formatter_data;

#ifdef __cplusplus
extern "C" {
#endif
formatter_data* formatter_data_create( void );
void formatter_data_init( formatter_data* nf_data );
void formatter_data_free( formatter_data* nf_data );
#ifdef __cplusplus
}
#endif

#endif // FORMATTER_DATA_H
