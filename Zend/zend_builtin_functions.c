/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to version 0.91 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/0_91.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#include "zend.h"
#include "zend_API.h"
#include "zend_builtin_functions.h"

static ZEND_FUNCTION(zend_version);

static zend_function_entry builtin_functions[] = {
	ZEND_FE(zend_version,		NULL)
	{ NULL, NULL, NULL }
};


int zend_startup_builtin_functions()
{
	return zend_register_functions(builtin_functions, NULL);
}


ZEND_FUNCTION(zend_version)
{
	RETURN_STRINGL(ZEND_VERSION, sizeof(ZEND_VERSION)-1, 1);
}
