/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Anthony Ferrara <ircmaxell@php.net>                         |
   +----------------------------------------------------------------------+
*/

/* $Id$ */
#include "zend.h"
#include "zend_API.h"
   
ZEND_FUNCTION(autoload_register);
ZEND_FUNCTION(autoload_unregister);

typedef struct {
   zend_fcall_info fci;
   zend_fcall_info_cache fcc;
   zval *callable;
   long type;
} zend_autoload_func;

int zend_autoload_call(zval* name, long type TSRMLS_DC);
int zend_autoload_register(zend_autoload_func* func, zend_bool prepend TSRMLS_DC);
int zend_autoload_unregister(zval *callable TSRMLS_DC);

#define ZEND_AUTOLOAD_CLASS    1
#define ZEND_AUTOLOAD_FUNCTION 2


