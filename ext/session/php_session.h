/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 4.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <ss@2ns.de>                                 |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_SESSION_H
#define PHP_SESSION_H

typedef enum {
	LOCK,
	UNLOCK
} ps_lock;

#define PS_OPEN_ARGS void **mod_data, const char *save_path, const char *session_name
#define PS_CLOSE_ARGS void **mod_data
#define PS_READ_ARGS void **mod_data, const char *key, char **val, int *vallen
#define PS_WRITE_ARGS void **mod_data, const char *key, const char *val, const int vallen
#define PS_DELETE_ARGS void **mod_data, const char *key
#define PS_GC_ARGS void **mod_data

typedef struct ps_module_struct {
	char *name;
	int (*open)(PS_OPEN_ARGS);
	int (*close)(PS_CLOSE_ARGS);
	int (*read)(PS_READ_ARGS);
	int (*write)(PS_WRITE_ARGS);
	int (*delete)(PS_DELETE_ARGS);
	int (*gc)(PS_GC_ARGS);
} ps_module;


#define PS_OPEN_FUNC(x) 	int _ps_open_##x(PS_OPEN_ARGS)
#define PS_CLOSE_FUNC(x) 	int _ps_close_##x(PS_CLOSE_ARGS)
#define PS_READ_FUNC(x) 	int _ps_read_##x(PS_READ_ARGS)
#define PS_WRITE_FUNC(x) 	int _ps_write_##x(PS_WRITE_ARGS)
#define PS_DELETE_FUNC(x) 	int _ps_delete_##x(PS_DELETE_ARGS)
#define PS_GC_FUNC(x) 		int _ps_gc_##x(PS_GC_ARGS)

#define PS_FUNCS(x) \
	PS_OPEN_FUNC(x); \
	PS_CLOSE_FUNC(x); \
	PS_READ_FUNC(x); \
	PS_WRITE_FUNC(x); \
	PS_DELETE_FUNC(x); \
	PS_GC_FUNC(x)


#define PS_MOD(x) \
	#x, _ps_open_##x, _ps_close_##x, _ps_read_##x, _ps_write_##x, \
	 _ps_delete_##x, _ps_gc_##x 

	
typedef struct {
	char *save_path;
	char *session_name;
	char *id;
	ps_module *mod;
	void *mod_data;
	HashTable vars;
	int nr_open_sessions;
} php_ps_globals;

extern zend_module_entry session_module_entry;
#define phpext_session_ptr &session_module_entry

PHP_FUNCTION(session_name);
PHP_FUNCTION(session_module_name);
PHP_FUNCTION(session_save_path);
PHP_FUNCTION(session_id);
PHP_FUNCTION(session_decode);
PHP_FUNCTION(session_register);
PHP_FUNCTION(session_unregister);
PHP_FUNCTION(session_encoded);
PHP_FUNCTION(session_start);

PS_FUNCS(files);

#ifdef ZTS
#define PSLS_D php_ps_globals *ps_globals
#define PSLS_DC , PSLS_D
#define PSLS_C ps_globals
#define PSLS_CC , PSLS_C
#define PS(v) (ps_globals->v)
#define PSLS_FETCH() php_ps_globals *ps_globals = ts_resource(ps_globals_id)
#else
#define PSLS_D
#define PSLS_DC
#define PSLS_C
#define PSLS_CC
#define PS(v) (ps_globals.v)
#define PSLS_FETCH()
#endif


#endif
