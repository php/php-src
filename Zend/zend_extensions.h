/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2000 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 0.92 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/0_92.txt.                                |
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

#define ZEND_EXTENSION_API_NO		20000707

typedef struct _zend_extension_version_info {
	int zend_extension_api_no;
	char *required_zend_version;
	unsigned char thread_safe;
	unsigned char debug;
} zend_extension_version_info;


typedef struct _zend_extension zend_extension;

struct _zend_extension {
	char *name;
	char *version;
	char *author;
	char *URL;
	char *copyright;

	int (*startup)(zend_extension *extension);
	void (*shutdown)(zend_extension *extension);
	void (*activate)();
	void (*deactivate)();

	void (*message_handler)(int message, void *arg);

	void (*op_array_handler)(zend_op_array *op_array);
	
	void (*statement_handler)(zend_op_array *op_array);
	void (*fcall_begin_handler)(zend_op_array *op_array);
	void (*fcall_end_handler)(zend_op_array *op_array);

	void (*op_array_ctor)(zend_op_array *op_array);
	void (*op_array_dtor)(zend_op_array *op_array);

	void *reserved1;
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


ZEND_API extern zend_llist zend_extensions;

void zend_extension_dtor(zend_extension *extension);
ZEND_API int zend_load_extension(char *path);
ZEND_API int zend_load_extensions(char **extension_paths);
ZEND_API int zend_register_extension(zend_extension *new_extension, DL_HANDLE handle);
void zend_append_version_info(zend_extension *extension);
int zend_startup_extensions(void);
void zend_shutdown_extensions(void);

#endif /* ZEND_EXTENSIONS_H */
