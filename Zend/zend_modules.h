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


#ifndef _MODULES_H
#define _MODULES_H

#define INIT_FUNC_ARGS		int type, int module_number
#define INIT_FUNC_ARGS_PASSTHRU	type, module_number
#define SHUTDOWN_FUNC_ARGS	int type, int module_number
#define SHUTDOWN_FUNC_ARGS_PASSTHRU type, module_number
#define ZEND_MODULE_INFO_FUNC_ARGS zend_module_entry *zend_module

#define STANDARD_MODULE_PROPERTIES_EX 0, 0, 0, NULL, 0

#define STANDARD_MODULE_PROPERTIES \
	NULL, NULL, STANDARD_MODULE_PROPERTIES_EX

#define MODULE_PERSISTENT 1
#define MODULE_TEMPORARY 2

typedef struct _zend_module_entry zend_module_entry;

struct _zend_module_entry {
	char *name;
	zend_function_entry *functions;
	int (*module_startup_func)(INIT_FUNC_ARGS);
	int (*module_shutdown_func)(SHUTDOWN_FUNC_ARGS);
	int (*request_startup_func)(INIT_FUNC_ARGS);
	int (*request_shutdown_func)(SHUTDOWN_FUNC_ARGS);
	void (*info_func)(ZEND_MODULE_INFO_FUNC_ARGS);
	int (*global_startup_func)(void);
	int (*global_shutdown_func)(void);
	int request_started, module_started;
	unsigned char type;
	void *handle;
	int module_number;
};


extern HashTable module_registry;

extern void module_destructor(zend_module_entry *module);
extern int module_registry_cleanup(zend_module_entry *module);
extern int module_registry_request_startup(zend_module_entry *module);

#define ZEND_MODULE_DTOR (int (*)(void *)) module_destructor
#endif
