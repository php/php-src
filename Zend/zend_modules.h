/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#ifndef MODULES_H
#define MODULES_H

#include "zend.h"
#include "zend_compile.h"
#include "zend_build.h"

#define INIT_FUNC_ARGS		int type, int module_number
#define INIT_FUNC_ARGS_PASSTHRU	type, module_number
#define SHUTDOWN_FUNC_ARGS	int type, int module_number
#define SHUTDOWN_FUNC_ARGS_PASSTHRU type, module_number
#define ZEND_MODULE_INFO_FUNC_ARGS zend_module_entry *zend_module
#define ZEND_MODULE_INFO_FUNC_ARGS_PASSTHRU zend_module

#define ZEND_MODULE_API_NO 20220829
#ifdef ZTS
#define USING_ZTS 1
#else
#define USING_ZTS 0
#endif

#define STANDARD_MODULE_HEADER_EX sizeof(zend_module_entry), ZEND_MODULE_API_NO, ZEND_DEBUG, USING_ZTS
#define STANDARD_MODULE_HEADER \
	STANDARD_MODULE_HEADER_EX, NULL, NULL
#define ZE2_STANDARD_MODULE_HEADER \
	STANDARD_MODULE_HEADER_EX, ini_entries, NULL

#define ZEND_MODULE_BUILD_ID "API" ZEND_TOSTR(ZEND_MODULE_API_NO) ZEND_BUILD_TS ZEND_BUILD_DEBUG ZEND_BUILD_SYSTEM ZEND_BUILD_EXTRA

#define STANDARD_MODULE_PROPERTIES_EX 0, 0, NULL, 0, ZEND_MODULE_BUILD_ID

#define NO_MODULE_GLOBALS 0, NULL, NULL, NULL

#ifdef ZTS
# define ZEND_MODULE_GLOBALS(module_name) sizeof(zend_##module_name##_globals), &module_name##_globals_id
#else
# define ZEND_MODULE_GLOBALS(module_name) sizeof(zend_##module_name##_globals), &module_name##_globals
#endif

#define STANDARD_MODULE_PROPERTIES \
	NO_MODULE_GLOBALS, NULL, STANDARD_MODULE_PROPERTIES_EX

#define NO_VERSION_YET NULL

#define MODULE_PERSISTENT 1
#define MODULE_TEMPORARY 2

struct _zend_ini_entry;
typedef struct _zend_module_entry zend_module_entry;
typedef struct _zend_module_dep zend_module_dep;

struct _zend_module_entry {
	unsigned short size;
	unsigned int zend_api;
	unsigned char zend_debug;
	unsigned char zts;
	const struct _zend_ini_entry *ini_entry;
	const struct _zend_module_dep *deps;
	const char *name;
	const struct _zend_function_entry *functions;
	zend_result (*module_startup_func)(INIT_FUNC_ARGS);
	zend_result (*module_shutdown_func)(SHUTDOWN_FUNC_ARGS);
	zend_result (*request_startup_func)(INIT_FUNC_ARGS);
	zend_result (*request_shutdown_func)(SHUTDOWN_FUNC_ARGS);
	void (*info_func)(ZEND_MODULE_INFO_FUNC_ARGS);
	const char *version;
	size_t globals_size;
#ifdef ZTS
	ts_rsrc_id* globals_id_ptr;
#else
	void* globals_ptr;
#endif
	void (*globals_ctor)(void *global);
	void (*globals_dtor)(void *global);
	zend_result (*post_deactivate_func)(void);
	int module_started;
	unsigned char type;
	void *handle;
	int module_number;
	const char *build_id;
};

#define MODULE_DEP_REQUIRED		1
#define MODULE_DEP_CONFLICTS	2
#define MODULE_DEP_OPTIONAL		3

#define ZEND_MOD_REQUIRED_EX(name, rel, ver)	{ name, rel, ver, MODULE_DEP_REQUIRED  },
#define ZEND_MOD_CONFLICTS_EX(name, rel, ver)	{ name, rel, ver, MODULE_DEP_CONFLICTS },
#define ZEND_MOD_OPTIONAL_EX(name, rel, ver)	{ name, rel, ver, MODULE_DEP_OPTIONAL  },

#define ZEND_MOD_REQUIRED(name)		ZEND_MOD_REQUIRED_EX(name, NULL, NULL)
#define ZEND_MOD_CONFLICTS(name)	ZEND_MOD_CONFLICTS_EX(name, NULL, NULL)
#define ZEND_MOD_OPTIONAL(name)		ZEND_MOD_OPTIONAL_EX(name, NULL, NULL)

#define ZEND_MOD_END { NULL, NULL, NULL, 0 }

struct _zend_module_dep {
	const char *name;		/* module name */
	const char *rel;		/* version relationship: NULL (exists), lt|le|eq|ge|gt (to given version) */
	const char *version;	/* version */
	unsigned char type;		/* dependency type */
};

BEGIN_EXTERN_C()
extern ZEND_API HashTable module_registry;

void module_destructor(zend_module_entry *module);
int module_registry_request_startup(zend_module_entry *module);
int module_registry_unload_temp(const zend_module_entry *module);
END_EXTERN_C()

#endif
