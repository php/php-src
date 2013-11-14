/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
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

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

static inline void i_phpdbg_list_func(const char *str TSRMLS_DC)
{
    HashTable *func_table = EG(function_table);
    zend_function* fbc;
    const char *func_name = str;
    size_t func_name_len = strlen(str);

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

    if (zend_hash_find(func_table, func_name, func_name_len+1,
        (void**)&fbc) == SUCCESS) {
        phpdbg_list_function(fbc TSRMLS_CC);
    } else {
        phpdbg_error("Function %s not found", func_name);
    }
}

PHPDBG_LIST(lines) /* {{{ */
{
    phpdbg_param_t param;
    int type = phpdbg_parse_param(
        expr, expr_len, &param TSRMLS_CC);
    
    switch (type) {
        case NUMERIC_PARAM:
	    case EMPTY_PARAM: {
	        if (PHPDBG_G(exec) || zend_is_executing(TSRMLS_C)) {
	            if (type == EMPTY_PARAM) {
	                phpdbg_list_file(phpdbg_current_file(TSRMLS_C), 0, 0 TSRMLS_CC);
	            } else phpdbg_list_file(phpdbg_current_file(TSRMLS_C), param.num, 0 TSRMLS_CC);
	        } else phpdbg_error("Not executing, and execution context not set");
	    } break;
	    
	    default:
	        phpdbg_error("Unsupported parameter type (%d) for command", type);
    }
    
    phpdbg_clear_param(type, &param TSRMLS_CC);
    
    return SUCCESS;
} /* }}} */

PHPDBG_LIST(func) /* {{{ */
{
    phpdbg_param_t param;
    int type = phpdbg_parse_param(
        expr, expr_len, &param TSRMLS_CC);
    
    if (type == STR_PARAM) {
        i_phpdbg_list_func(
            param.str TSRMLS_CC);
    }
    
    phpdbg_clear_param(type, &param TSRMLS_CC);
    
    return SUCCESS;
} /* }}} */

void phpdbg_list_dispatch(int type, phpdbg_param_t *param TSRMLS_DC) /* {{{ */
{
    switch (type) {
        case NUMERIC_PARAM:
	    case EMPTY_PARAM: {
	        if (PHPDBG_G(exec) || zend_is_executing(TSRMLS_C)) {
	            if (type == EMPTY_PARAM) {
	                phpdbg_list_file(phpdbg_current_file(TSRMLS_C), 0, 0 TSRMLS_CC);
	            } else phpdbg_list_file(phpdbg_current_file(TSRMLS_C), param->num, 0 TSRMLS_CC);
	        } else phpdbg_error("Not executing, and execution context not set");
	    } break;
	    
		case FILE_PARAM:
			phpdbg_list_file(param->file.name, param->file.line, 0 TSRMLS_CC);
			break;
			
		case STR_PARAM: {
		    i_phpdbg_list_func(param->str TSRMLS_CC);
		} break;
		
		default:
		    phpdbg_error("Unsupported input type (%d) for command", type);
			break;
    }
} /* }}} */

void phpdbg_list_file(const char *filename, long count, long offset TSRMLS_DC) /* {{{ */
{
	unsigned char *mem, *pos, *last_pos, *end_pos;
	struct stat st;
#ifndef _WIN32
	int fd;
#else
	HANDLE fd, map;
#endif
	int all_content = (count == 0);
	unsigned int line = 0, displayed = 0;

    if (VCWD_STAT(filename, &st) == -1) {
		phpdbg_error("Failed to stat file %s", filename);
		return;
	}
#ifndef _WIN32
	if ((fd = VCWD_OPEN(filename, O_RDONLY)) == -1) {
		phpdbg_error("Failed to open file %s to list", filename);
		return;
	}

	last_pos = mem = mmap(0, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
	end_pos = mem + st.st_size;
#else
	fd = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (fd == INVALID_HANDLE_VALUE) {
		phpdbg_error("Failed to open file!");
		return;
	}

	map = CreateFileMapping(fd, NULL, PAGE_EXECUTE_READ, 0, 0, 0);
	if (map == NULL) {
		phpdbg_error("Failed to map file!");
		CloseHandle(fd);
		return;
	}

	last_pos = mem = (char*) MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0);
	if (mem == NULL) {
		phpdbg_error("Failed to map file in memory");
		CloseHandle(map);
		CloseHandle(fd);
		return;
	}
	end_pos = mem + st.st_size;
#endif
	while (1) {
		pos = memchr(last_pos, '\n', end_pos - last_pos);

		if (!pos) {
			/* No more line breaks */
			break;
		}

		++line;

		if (!offset || offset <= line) {
			/* Without offset, or offset reached */
			phpdbg_writeln("%05u: %.*s", line, (int)(pos - last_pos), last_pos);
			++displayed;
		}

		last_pos = pos + 1;

		if (!all_content && displayed == count) {
			/* Reached max line to display */
			break;
		}
	}

#ifndef _WIN32
	munmap(mem, st.st_size);
	close(fd);
#else
	UnmapViewOfFile(mem);
	CloseHandle(map);
	CloseHandle(fd);
#endif
} /* }}} */

void phpdbg_list_function(const zend_function *fbc TSRMLS_DC) /* {{{ */
{
	const zend_op_array *ops;

	if (fbc->type != ZEND_USER_FUNCTION) {
		return;
	}

	ops = (zend_op_array*)fbc;

	phpdbg_list_file(ops->filename,
		ops->line_end - ops->line_start + 1, ops->line_start TSRMLS_CC);
} /* }}} */

