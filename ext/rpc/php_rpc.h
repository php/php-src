/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Harald Radi <h.radi@nme.at>                                  |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_RPC_H
#define PHP_RPC_H

#include "zend.h"

extern zend_module_entry rpc_module_entry;
#define phpext_rpc_ptr &rpc_module_entry

#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_MINIT_FUNCTION(rpc);
ZEND_MSHUTDOWN_FUNCTION(rpc);
ZEND_RINIT_FUNCTION(rpc);
ZEND_RSHUTDOWN_FUNCTION(rpc);
ZEND_MINFO_FUNCTION(rpc);

ZEND_FUNCTION(rpc_load);
ZEND_FUNCTION(rpc_call);
ZEND_FUNCTION(rpc_set);
ZEND_FUNCTION(rpc_get);
ZEND_FUNCTION(rpc_singleton);
ZEND_FUNCTION(rpc_poolable);

ZEND_API void rpc_error(int type, const char *format, ...);
ZEND_API zend_object_value rpc_objects_new(zend_class_entry * TSRMLS_DC);

#define phpext_rpc_ptr &rpc_module_entry

#endif	/* PHP_RPC_H */