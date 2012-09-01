/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Axel Etcheverry <axel@etcheverry.biz>                       |
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

#include "php_spl.h"
#include "spl_functions.h"
#include "spl_engine.h"
#include "spl_filters.h"

PHPAPI zend_class_entry *spl_ce_Filter;

ZEND_BEGIN_ARG_INFO(arginfo_filter_filter, 0) 
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

static const zend_function_entry spl_funcs_Filter[] = {
	SPL_ABSTRACT_ME(Filter, filter,   arginfo_filter_filter)
	PHP_FE_END
};

/* {{{ PHP_MINIT_FUNCTION(spl_filters)
 */
PHP_MINIT_FUNCTION(spl_filters)
{

    REGISTER_SPL_INTERFACE(Filter);
    
    return SUCCESS;
}
/* }}} */

