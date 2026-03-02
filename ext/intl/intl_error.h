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
   | Authors: Vadim Savchuk <vsavchuk@productengine.com>                  |
   |          Dmitry Lakhtyuk <dlakhtyuk@productengine.com>               |
   |          Stanislav Malyshev <stas@zend.com>                          |
   +----------------------------------------------------------------------+
 */

#ifndef INTL_ERROR_H
#define INTL_ERROR_H

#include <unicode/utypes.h>
#include <unicode/parseerr.h>
#include <zend_smart_str.h>

#define INTL_ERROR_CODE(e) (e).code

typedef struct {
	zend_string *custom_error_message;
	UErrorCode   code;
} intl_error;

intl_error* intl_error_create( void );
void        intl_error_init( intl_error* err );
void        intl_error_reset( intl_error* err );
void        intl_error_set_code( intl_error* err, UErrorCode err_code );
void        intl_error_set_custom_msg( intl_error* err, const char* msg);
void        intl_error_set( intl_error* err, UErrorCode code, const char* msg);
UErrorCode  intl_error_get_code( intl_error* err );
zend_string* intl_error_get_message( intl_error* err );

// Wrappers to synchonize object's and global error structures.
void        intl_errors_reset( intl_error* err );
void        intl_errors_set_custom_msg( intl_error* err, const char* msg);
void        intl_errors_set_code( intl_error* err, UErrorCode err_code );
void        intl_errors_set( intl_error* err, UErrorCode code, const char* msg);

// Other error helpers
smart_str	intl_parse_error_to_string( UParseError* pe );

#endif // INTL_ERROR_H
