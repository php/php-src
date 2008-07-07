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

#ifndef MSG_FORMAT_CLASS_H
#define MSG_FORMAT_CLASS_H

#include <php.h>

#include "intl_common.h"
#include "intl_error.h"
#include "intl_data.h"
#include "msgformat_data.h"

typedef struct {
	zend_object     zo;
	msgformat_data  mf_data;
} MessageFormatter_object;

void msgformat_register_class( TSRMLS_D );
extern zend_class_entry *MessageFormatter_ce_ptr;

/* Auxiliary macros */

#define MSG_FORMAT_METHOD_INIT_VARS		INTL_METHOD_INIT_VARS(MessageFormatter, mfo)
#define MSG_FORMAT_METHOD_FETCH_OBJECT	INTL_METHOD_FETCH_OBJECT(MessageFormatter, mfo)
#define MSG_FORMAT_OBJECT(mfo)			(mfo)->mf_data.umsgf

#endif // #ifndef MSG_FORMAT_CLASS_H
