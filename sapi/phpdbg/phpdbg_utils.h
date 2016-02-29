/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Felipe Pena <felipe@php.net>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#ifndef PHPDBG_UTILS_H
#define PHPDBG_UTILS_H

/**
 * Input scan functions
 */
PHPDBG_API int phpdbg_is_numeric(const char*);
PHPDBG_API int phpdbg_is_empty(const char*);
PHPDBG_API int phpdbg_is_addr(const char*);
PHPDBG_API int phpdbg_is_class_method(const char*, size_t, char**, char**);
PHPDBG_API const char *phpdbg_current_file(void);
PHPDBG_API char *phpdbg_resolve_path(const char*);
PHPDBG_API char *phpdbg_trim(const char*, size_t, size_t*);
PHPDBG_API const zend_function *phpdbg_get_function(const char*, const char*);

/* {{{ Color Management */
#define PHPDBG_COLOR_LEN 12
#define PHPDBG_COLOR_D(color, code) \
	{color, sizeof(color)-1, code}
#define PHPDBG_COLOR_END \
	{NULL, 0L, {0}}
#define PHPDBG_ELEMENT_LEN 3
#define PHPDBG_ELEMENT_D(name, id) \
	{name, sizeof(name)-1, id}
#define PHPDBG_ELEMENT_END \
	{NULL, 0L, 0}

#define PHPDBG_COLOR_INVALID	-1
#define PHPDBG_COLOR_PROMPT 	 0
#define PHPDBG_COLOR_ERROR		 1
#define PHPDBG_COLOR_NOTICE		 2
#define PHPDBG_COLORS			 3

typedef struct _phpdbg_color_t {
	char       *name;
	size_t      name_length;
	const char  code[PHPDBG_COLOR_LEN];
} phpdbg_color_t;

typedef struct _phpdbg_element_t {
	char		*name;
	size_t		name_length;
	int			id;
} phpdbg_element_t;

PHPDBG_API const phpdbg_color_t *phpdbg_get_color(const char *name, size_t name_length);
PHPDBG_API void phpdbg_set_color(int element, const phpdbg_color_t *color);
PHPDBG_API void phpdbg_set_color_ex(int element, const char *name, size_t name_length);
PHPDBG_API const phpdbg_color_t *phpdbg_get_colors(void);
PHPDBG_API int phpdbg_get_element(const char *name, size_t len); /* }}} */

/* {{{ Prompt Management */
PHPDBG_API void phpdbg_set_prompt(const char*);
PHPDBG_API const char *phpdbg_get_prompt(void); /* }}} */

/* {{{ Console size */
PHPDBG_API int phpdbg_get_terminal_width(void);
PHPDBG_API int phpdbg_get_terminal_height(void); /* }}} */

PHPDBG_API void phpdbg_set_async_io(int fd);

int phpdbg_rebuild_symtable(void);

int phpdbg_safe_class_lookup(const char *name, int name_length, zend_class_entry **ce);

char *phpdbg_get_property_key(char *key);

typedef int (*phpdbg_parse_var_func)(char *name, size_t len, char *keyname, size_t keylen, HashTable *parent, zval *zv);
typedef int (*phpdbg_parse_var_with_arg_func)(char *name, size_t len, char *keyname, size_t keylen, HashTable *parent, zval *zv, void *arg);

PHPDBG_API int phpdbg_parse_variable(char *input, size_t len, HashTable *parent, size_t i, phpdbg_parse_var_func callback, zend_bool silent);
PHPDBG_API int phpdbg_parse_variable_with_arg(char *input, size_t len, HashTable *parent, size_t i, phpdbg_parse_var_with_arg_func callback, phpdbg_parse_var_with_arg_func step_cb, zend_bool silent, void *arg);

int phpdbg_is_auto_global(char *name, int len);

PHPDBG_API void phpdbg_xml_var_dump(zval *zv);

char *phpdbg_short_zval_print(zval *zv, int maxlen);

PHPDBG_API zend_bool phpdbg_check_caught_ex(zend_execute_data *ex, zend_object *exception);

static zend_always_inline zend_execute_data *phpdbg_user_execute_data(zend_execute_data *ex) {
	while (!ex->func || !ZEND_USER_CODE(ex->func->common.type)) {
		ex = ex->prev_execute_data;
		ZEND_ASSERT(ex);
	}
	return ex;
}

#ifdef ZTS
#define PHPDBG_OUTPUT_BACKUP_DEFINES() \
	zend_output_globals *output_globals_ptr; \
	zend_output_globals original_output_globals; \
	output_globals_ptr = (zend_output_globals *) (*((void ***) tsrm_get_ls_cache()))[TSRM_UNSHUFFLE_RSRC_ID(output_globals_id)];
#else
#define PHPDBG_OUTPUT_BACKUP_DEFINES() \
	zend_output_globals *output_globals_ptr; \
	zend_output_globals original_output_globals; \
	output_globals_ptr = &output_globals;
#endif

#define PHPDBG_OUTPUT_BACKUP_SWAP() \
	original_output_globals = *output_globals_ptr; \
	memset(output_globals_ptr, 0, sizeof(zend_output_globals)); \
	php_output_activate();

#define PHPDBG_OUTPUT_BACKUP() \
	PHPDBG_OUTPUT_BACKUP_DEFINES() \
	PHPDBG_OUTPUT_BACKUP_SWAP()

#define PHPDBG_OUTPUT_BACKUP_RESTORE() \
	php_output_deactivate(); \
	*output_globals_ptr = original_output_globals;

#endif /* PHPDBG_UTILS_H */
