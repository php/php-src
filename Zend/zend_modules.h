/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2003 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#ifndef MODULES_H
#define MODULES_H

#include "zend.h"
#include "zend_ini_entry.h"

#define INIT_FUNC_ARGS		int type, int module_number TSRMLS_DC
#define INIT_FUNC_ARGS_PASSTHRU	type, module_number TSRMLS_CC
#define SHUTDOWN_FUNC_ARGS	int type, int module_number TSRMLS_DC
#define SHUTDOWN_FUNC_ARGS_PASSTHRU type, module_number TSRMLS_CC
#define ZEND_MODULE_INFO_FUNC_ARGS zend_module_entry *zend_module TSRMLS_DC
#define ZEND_MODULE_INFO_FUNC_ARGS_PASSTHRU zend_module TSRMLS_CC

ZEND_API extern unsigned char first_arg_force_ref[];
ZEND_API extern unsigned char second_arg_force_ref[];
ZEND_API extern unsigned char third_arg_force_ref[];

#define ZEND_MODULE_API_NO 20020429
#ifdef ZTS
#define USING_ZTS 1
#else
#define USING_ZTS 0
#endif

#define STANDARD_MODULE_HEADER sizeof(zend_module_entry), ZEND_MODULE_API_NO, ZEND_DEBUG, USING_ZTS, NULL
#define ZE2_STANDARD_MODULE_HEADER sizeof(zend_module_entry), ZEND_MODULE_API_NO, ZEND_DEBUG, USING_ZTS, &ini_entries

#define STANDARD_MODULE_PROPERTIES_EX 0, 0, 0, NULL, 0

#define STANDARD_MODULE_PROPERTIES \
	NULL, NULL, STANDARD_MODULE_PROPERTIES_EX

#define NO_VERSION_YET NULL

#define MODULE_PERSISTENT 1
#define MODULE_TEMPORARY 2

typedef struct _zend_module_entry zend_module_entry;

struct _zend_module_entry {
    unsigned short size;
	unsigned int zend_api;
	unsigned char zend_debug;
	unsigned char zts;
	zend_ini_entry *ini_entry;
	char *name;
	zend_function_entry *functions;
	int (*module_startup_func)(INIT_FUNC_ARGS);
	int (*module_shutdown_func)(SHUTDOWN_FUNC_ARGS);
	int (*request_startup_func)(INIT_FUNC_ARGS);
	int (*request_shutdown_func)(SHUTDOWN_FUNC_ARGS);
	void (*info_func)(ZEND_MODULE_INFO_FUNC_ARGS);
	char *version;
	int (*global_startup_func)(void);
	int (*global_shutdown_func)(void);
	int globals_id;
	int module_started;
	unsigned char type;
	void *handle;
	int module_number;
};


extern ZEND_API HashTable module_registry;

void module_destructor(zend_module_entry *module);
int module_registry_cleanup(zend_module_entry *module TSRMLS_DC);
int module_registry_request_startup(zend_module_entry *module TSRMLS_DC);

#define ZEND_MODULE_DTOR (void (*)(void *)) module_destructor
#endif
