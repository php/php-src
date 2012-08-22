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
   | Authors: Stanislav Malyshev <stas@zend.com>                          |
   +----------------------------------------------------------------------+
 */

#ifndef FORMATTER_CLASS_H
#define FORMATTER_CLASS_H

#include <php.h>

#include "intl_common.h"
#include "intl_error.h"
#include "intl_data.h"
#include "formatter_data.h"

typedef struct {
	zend_object     zo;
	formatter_data  nf_data;
} NumberFormatter_object;

void formatter_register_class( TSRMLS_D );
extern zend_class_entry *NumberFormatter_ce_ptr;

/* Auxiliary macros */

#define FORMATTER_METHOD_INIT_VARS				INTL_METHOD_INIT_VARS(NumberFormatter, nfo)
#define FORMATTER_OBJECT(nfo)					(nfo)->nf_data.unum
#define FORMATTER_METHOD_FETCH_OBJECT_NO_CHECK	INTL_METHOD_FETCH_OBJECT(NumberFormatter, nfo)
#define FORMATTER_METHOD_FETCH_OBJECT \
	FORMATTER_METHOD_FETCH_OBJECT_NO_CHECK; \
	if (FORMATTER_OBJECT(nfo) == NULL) \
	{ \
		intl_errors_set(&nfo->nf_data.error, U_ILLEGAL_ARGUMENT_ERROR, \
				"Found unconstructed NumberFormatter", 0 TSRMLS_CC); \
		RETURN_FALSE; \
	}


#endif // #ifndef FORMATTER_CLASS_H
