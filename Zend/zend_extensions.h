/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2002 Zend Technologies Ltd. (http://www.zend.com) |
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


#ifndef ZEND_EXTENSIONS_H
#define ZEND_EXTENSIONS_H

#include "zend_compile.h"

#define ZEND_EXTENSION_API_NO	90021012

typedef struct _zend_extension_version_info {
	int zend_extension_api_no;
	char *required_zend_version;
	unsigned char thread_safe;
	unsigned char debug;
} zend_extension_version_info;


typedef struct _zend_extension zend_extension;

/* Typedef's for zend_extension function pointers */
typedef int (*startup_func_t)(zend_extension *extension);
typedef void (*shutdown_func_t)(zend_extension *extension);
typedef void (*activate_func_t)(void);
typedef void (*deactivate_func_t)(void);

typedef void (*message_handler_func_t)(int message, void *arg);

typedef void (*op_array_handler_func_t)(zend_op_array *op_array);

typedef void (*statement_handler_func_t)(zend_op_array *op_array);
typedef void (*fcall_begin_handler_func_t)(zend_op_array *op_array);
typedef void (*fcall_end_handler_func_t)(zend_op_array *op_array);

typedef void (*op_array_ctor_func_t)(zend_op_array *op_array);
typedef void (*op_array_dtor_func_t)(zend_op_array *op_array);

struct _zend_extension {
	char *name;
	char *version;
	char *author;
	char *URL;
	char *copyright;

	startup_func_t startup;
	shutdown_func_t shutdown;
	activate_func_t activate;
	deactivate_func_t deactivate;

	message_handler_func_t message_handler;

	op_array_handler_func_t op_array_handler;

	statement_handler_func_t statement_handler;	
	fcall_begin_handler_func_t fcall_begin_handler;
	fcall_end_handler_func_t fcall_end_handler;

	op_array_ctor_func_t op_array_ctor;
	op_array_dtor_func_t op_array_dtor;

	int (*api_no_check)(int api_no);
	void *reserved2;
	void *reserved3;
	void *reserved4;
	void *reserved5;
	void *reserved6;
	void *reserved7;
	void *reserved8;

	DL_HANDLE handle;
	int resource_number;
};


ZEND_API int zend_get_resource_handle(zend_extension *extension);
ZEND_API void zend_extension_dispatch_message(int message, void *arg);

#define ZEND_EXTMSG_NEW_EXTENSION		1


#define ZEND_EXTENSION()	\
	ZEND_EXT_API zend_extension_version_info extension_version_info = { ZEND_EXTENSION_API_NO, ZEND_VERSION, ZTS_V, ZEND_DEBUG }

#define STANDARD_ZEND_EXTENSION_PROPERTIES NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, -1
#define COMPAT_ZEND_EXTENSION_PROPERTIES   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, -1


ZEND_API extern zend_llist zend_extensions;

void zend_extension_dtor(zend_extension *extension);
ZEND_API int zend_load_extension(char *path);
ZEND_API int zend_register_extension(zend_extension *new_extension, DL_HANDLE handle);
void zend_append_version_info(zend_extension *extension);
int zend_startup_extensions_mechanism(void);
int zend_startup_extensions(void);
void zend_shutdown_extensions(TSRMLS_D);
ZEND_API zend_extension *zend_get_extension(char *extension_name);

#endif /* ZEND_EXTENSIONS_H */
