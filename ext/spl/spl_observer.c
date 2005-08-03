/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2005 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"

#include "php_spl.h"
#include "spl_functions.h"
#include "spl_engine.h"
#include "spl_observer.h"

SPL_METHOD(Observer, update);
SPL_METHOD(Subject, attach);
SPL_METHOD(Subject, detach);
SPL_METHOD(Subject, notify);

static
ZEND_BEGIN_ARG_INFO(arginfo_Observer_update, 0)
	ZEND_ARG_OBJ_INFO(0, subject, Subject, 0)
ZEND_END_ARG_INFO();

static zend_function_entry spl_funcs_Observer[] = {
	SPL_ABSTRACT_ME(Observer, update,   arginfo_Observer_update)
	{NULL, NULL, NULL}
};

static
ZEND_BEGIN_ARG_INFO(arginfo_Subject_attach, 0)
	ZEND_ARG_OBJ_INFO(0, observer, Observer, 0)
ZEND_END_ARG_INFO();

/*static
ZEND_BEGIN_ARG_INFO_EX(arginfo_Subject_notify, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, ignore, Observer, 1)
ZEND_END_ARG_INFO();*/

static zend_function_entry spl_funcs_Subject[] = {
	SPL_ABSTRACT_ME(Subject,  attach,   arginfo_Subject_attach)
	SPL_ABSTRACT_ME(Subject,  detach,   arginfo_Subject_attach)
	SPL_ABSTRACT_ME(Subject,  notify,   NULL)
	{NULL, NULL, NULL}
};

PHPAPI zend_class_entry  *spl_ce_Observer;
PHPAPI zend_class_entry  *spl_ce_Subject;

/* {{{ PHP_MINIT_FUNCTION(spl_observer) */
PHP_MINIT_FUNCTION(spl_observer)
{
	REGISTER_SPL_INTERFACE(Observer);
	REGISTER_SPL_INTERFACE(Subject);
	
	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
