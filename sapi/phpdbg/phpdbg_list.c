/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
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

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#ifndef _WIN32
#	include <sys/mman.h>
#	include <unistd.h>
#endif
#include <fcntl.h>
#include "phpdbg.h"
#include "phpdbg_list.h"
#include "phpdbg_utils.h"
#include "phpdbg_prompt.h"
#include "php_streams.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

#define PHPDBG_LIST_COMMAND_D(f, h, a, m, l, s) \
	PHPDBG_COMMAND_D_EXP(f, h, a, m, l, s, &phpdbg_prompt_commands[13])

const phpdbg_command_t phpdbg_list_commands[] = {
	PHPDBG_LIST_COMMAND_D(lines,     "lists the specified lines",    'l', list_lines,  NULL, "l"),
	PHPDBG_LIST_COMMAND_D(class,     "lists the specified class",    'c', list_class,  NULL, "s"),
	PHPDBG_LIST_COMMAND_D(method,    "lists the specified method",   'm', list_method, NULL, "m"),
	PHPDBG_LIST_COMMAND_D(func,      "lists the specified function", 'f', list_func,   NULL, "s"),
	PHPDBG_END_COMMAND
};

PHPDBG_LIST(lines) /* {{{ */
{
	if (!PHPDBG_G(exec) && !zend_is_executing(TSRMLS_C)) {
		phpdbg_error("Not executing, and execution context not set");
		return SUCCESS;
	}

	switch (param->type) {
		case NUMERIC_PARAM:
			phpdbg_list_file(phpdbg_current_file(TSRMLS_C),
				(param->num < 0 ? 1 - param->num : param->num),
				(param->num < 0 ? param->num : 0) + zend_get_executed_lineno(TSRMLS_C),
				0 TSRMLS_CC);
			break;
			
		case FILE_PARAM:
			phpdbg_list_file(param->file.name, param->file.line, 0, 0 TSRMLS_CC);
			break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

PHPDBG_LIST(func) /* {{{ */
{
	phpdbg_list_function_byname(param->str, param->len TSRMLS_CC);

	return SUCCESS;
} /* }}} */

PHPDBG_LIST(method) /* {{{ */
{
	zend_class_entry **ce;

	if (zend_lookup_class(param->method.class, strlen(param->method.class), &ce TSRMLS_CC) == SUCCESS) {
		zend_function *function;
		char *lcname = zend_str_tolower_dup(param->method.name, strlen(param->method.name));

		if (zend_hash_find(&(*ce)->function_table, lcname, strlen(lcname)+1, (void**) &function) == SUCCESS) {
			phpdbg_list_function(function TSRMLS_CC);
		} else {
			phpdbg_error("Could not find %s::%s", param->method.class, param->method.name);
		}

		efree(lcname);
	} else {
		phpdbg_error("Could not find the class %s", param->method.class);
	}

	return SUCCESS;
} /* }}} */

PHPDBG_LIST(class) /* {{{ */
{
	zend_class_entry **ce;

	if (zend_lookup_class(param->str, param->len, &ce TSRMLS_CC) == SUCCESS) {
		if ((*ce)->type == ZEND_USER_CLASS) {
			if ((*ce)->info.user.filename) {
				phpdbg_list_file(
					(*ce)->info.user.filename,
					(*ce)->info.user.line_end - (*ce)->info.user.line_start + 1,
					(*ce)->info.user.line_start, 0 TSRMLS_CC
				);
			} else {
				phpdbg_error("The source of the requested class (%s) cannot be found", (*ce)->name);
			}
		} else {
			phpdbg_error("The class requested (%s) is not user defined", (*ce)->name);
		}
	} else {
		phpdbg_error("The requested class (%s) could not be found", param->str);
	}

	return SUCCESS;
} /* }}} */

void phpdbg_list_file(const char *filename, long count, long offset, int highlight TSRMLS_DC) /* {{{ */
{
	struct stat st;
	char *opened = NULL;
	char buffer[8096] = {0,};
	long line = 0;

	php_stream *stream = NULL;
	
	if (VCWD_STAT(filename, &st) == FAILURE) {
		phpdbg_error("Failed to stat file %s", filename);
		return;
	}

	stream = php_stream_open_wrapper(filename, "rb", USE_PATH, &opened);
	
	if (!stream) {
		phpdbg_error("Failed to open file %s to list", filename);
		return;
	}
	
	if (offset < 0) {
		count += offset;
		offset = 0;
	}
	
	while (php_stream_gets(stream, buffer, sizeof(buffer)) != NULL) {
		long linelen = strlen(buffer);

		++line;
		
		if (offset <= line) {
			if (!highlight) {
				phpdbg_write("%05ld: %s", line, buffer);
			} else {
				if (highlight != line) {
					phpdbg_write(" %05ld: %s", line, buffer);
				} else {
					phpdbg_write(">%05ld: %s", line, buffer);
				}
			}

			if (buffer[linelen - 1] != '\n') {
				phpdbg_write("\n");
			}
		}

		if (count > 0 && count + offset - 1 < line) {
			break;
		}
	}
	
	php_stream_close(stream);
} /* }}} */

void phpdbg_list_function(const zend_function *fbc TSRMLS_DC) /* {{{ */
{
	const zend_op_array *ops;

	if (fbc->type != ZEND_USER_FUNCTION) {
		phpdbg_error("The function requested (%s) is not user defined", fbc->common.function_name);
		return;
	}

	ops = (zend_op_array*)fbc;

	phpdbg_list_file(ops->filename,
		ops->line_end - ops->line_start + 1, ops->line_start, 0 TSRMLS_CC);
} /* }}} */

void phpdbg_list_function_byname(const char *str, size_t len TSRMLS_DC) /* {{{ */
{
	HashTable *func_table = EG(function_table);
	zend_function* fbc;
	char *func_name = (char*) str;
	size_t func_name_len = len;

	/* search active scope if begins with period */
	if (func_name[0] == '.') {
		if (EG(scope)) {
			func_name++;
			func_name_len--;

			func_table = &EG(scope)->function_table;
		} else {
			phpdbg_error("No active class");
			return;
		}
	} else if (!EG(function_table)) {
		phpdbg_error("No function table loaded");
		return;
	} else {
		func_table = EG(function_table);
	}

	/* use lowercase names, case insensitive */
	func_name = zend_str_tolower_dup(func_name, func_name_len);

	if (zend_hash_find(func_table, func_name, func_name_len+1, (void**)&fbc) == SUCCESS) {
		phpdbg_list_function(fbc TSRMLS_CC);
	} else {
		phpdbg_error("Function %s not found", func_name);
	}

	efree(func_name);
} /* }}} */

