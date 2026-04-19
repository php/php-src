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
   | Authors: Christian Stocker <chregu@php.net>                          |
   |          Rob Richards <rrichards@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifndef DOM_EXCEPTION_H
#define DOM_EXCEPTION_H

/* domexception errors */
typedef enum {
/* PHP_ERR is non-spec code for PHP errors: */
	PHP_ERR                        = 0,
	INDEX_SIZE_ERR                 = 1,
	DOMSTRING_SIZE_ERR             = 2,
	HIERARCHY_REQUEST_ERR          = 3,
	WRONG_DOCUMENT_ERR             = 4,
	INVALID_CHARACTER_ERR          = 5,
	NO_DATA_ALLOWED_ERR            = 6,
	NO_MODIFICATION_ALLOWED_ERR    = 7,
	NOT_FOUND_ERR                  = 8,
	NOT_SUPPORTED_ERR              = 9,
	INUSE_ATTRIBUTE_ERR            = 10,
/* Introduced in DOM Level 2: */
	INVALID_STATE_ERR              = 11,
/* Introduced in DOM Level 2: */
	SYNTAX_ERR                     = 12,
/* Introduced in DOM Level 2: */
	INVALID_MODIFICATION_ERR       = 13,
/* Introduced in DOM Level 2: */
	NAMESPACE_ERR                  = 14,
/* Introduced in DOM Level 2: */
	INVALID_ACCESS_ERR             = 15,
/* Introduced in DOM Level 3: */
	VALIDATION_ERR                 = 16
} dom_exception_code;

void php_dom_throw_error(dom_exception_code error_code, bool strict_error);
void php_dom_throw_error_with_message(dom_exception_code error_code, const char *error_message, bool strict_error);

#endif /* DOM_EXCEPTION_H */
