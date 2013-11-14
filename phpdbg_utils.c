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
#include <ctype.h>
#include "zend.h"
#include "php.h"
#include "spprintf.h"
#include "phpdbg.h"
#include "phpdbg_opcode.h"
#include "phpdbg_utils.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

int phpdbg_is_numeric(const char *str) /* {{{ */
{
    if (!str)
        return 0;

	for (; *str; str++) {
		if (isspace(*str)) {
			continue;
		}
		return isdigit(*str);
	}
	return 0;
} /* }}} */

int phpdbg_is_empty(const char *str) /* {{{ */
{
    if (!str)
        return 1;
    
	for (; *str; str++) {
		if (isspace(*str)) {
			continue;
		}
		return 0;
	}
	return 1;
} /* }}} */

int phpdbg_is_addr(const char *str) /* {{{ */
{
	return str[0] && str[1] && memcmp(str, "0x", 2) == 0;
} /* }}} */

int phpdbg_is_class_method(const char *str, size_t len, char **class, char **method) /* {{{ */
{
	const char *sep = strstr(str, "::");

	if (!sep || sep == str || sep+2 == str+len-1) {
		return 0;
	}

	if (class != NULL) {
	    *class = estrndup(str, sep - str);
	    (*class)[sep - str] = 0;
	}

	if (method != NULL) {
	    *method = estrndup(
	        sep+2, str + len - (sep + 2));
	}

	return 1;
} /* }}} */

char *phpdbg_resolve_path(const char *path TSRMLS_DC) /* {{{ */
{
	char resolved_name[MAXPATHLEN];

	if (expand_filepath(path, resolved_name TSRMLS_CC) == NULL) {
		return NULL;
	}

	return estrdup(resolved_name);
} /* }}} */

const char *phpdbg_current_file(TSRMLS_D) /* {{{ */
{
	const char *file = zend_get_executed_filename(TSRMLS_C);

	if (memcmp(file, "[no active file]", sizeof("[no active file]")) == 0) {
		return PHPDBG_G(exec);
	}

	return file;
} /* }}} */

int phpdbg_parse_param(const char *str, size_t len, phpdbg_param_t *param TSRMLS_DC) /* {{{ */
{
	char *class_name, *func_name;

	if (len == 0) {
		return EMPTY_PARAM;
	}

	if (phpdbg_is_addr(str)) {
		param->addr = strtoul(str, 0, 16);
		return ADDR_PARAM;
	} else if (phpdbg_is_numeric(str)) {
		param->num = strtol(str, NULL, 0);
		return NUMERIC_PARAM;
	} else if (phpdbg_is_class_method(str, len+1, &class_name, &func_name)) {
		param->method.class = class_name;
		param->method.name = func_name;
		return METHOD_PARAM;
	} else {
		const char *line_pos = strchr(str, ':');

		if (line_pos && phpdbg_is_numeric(line_pos+1)) {
			char path[MAXPATHLEN];

			memcpy(path, str, line_pos - str);
			path[line_pos - str] = 0;

			param->file.name = phpdbg_resolve_path(path TSRMLS_CC);
			param->file.line = strtol(line_pos+1, NULL, 0);
			return FILE_PARAM;
		}
	}

	param->str = estrndup(str, len);
	return STR_PARAM;
} /* }}} */

void phpdbg_clear_param(int type, phpdbg_param_t *param TSRMLS_DC) /* {{{ */
{
	switch (type) {
		case FILE_PARAM:
			efree(param->file.name);
			break;
		case METHOD_PARAM:
			efree(param->method.class);
			efree(param->method.name);
			break;
		case STR_PARAM:
			efree(param->str);
			break;
		default:
			break;
	}
} /* }}} */

int phpdbg_print(int type TSRMLS_DC, FILE *fp, const char *format, ...) /* {{{ */
{
    int rc = 0;
	char *buffer = NULL;
	va_list args;

	if (format != NULL && strlen(format) > 0L) {
	    va_start(args, format);
	    vspprintf(&buffer, 0, format, args);
	    va_end(args);
	}

    /* TODO(anyone) colours */

	switch (type) {
		case P_ERROR:
			rc = fprintf(fp, "%s%s%s\n",
				        ((PHPDBG_G(flags) & PHPDBG_IS_COLOURED) ? "\033[1;31m[" : "["),
				        buffer,
				        ((PHPDBG_G(flags) & PHPDBG_IS_COLOURED) ? "]\033[0m" : "]"));
		break;

		case P_NOTICE:
			rc = fprintf(fp, "%s%s%s\n",
				        ((PHPDBG_G(flags) & PHPDBG_IS_COLOURED) ? "\033[1;64m[" : "["),
				        buffer,
				        ((PHPDBG_G(flags) & PHPDBG_IS_COLOURED) ? "]\033[0m" : "]"));
		break;

		case P_WRITELN: {
		    if (buffer) {
			    rc = fprintf(fp, "%s%s%s\n",
				            ((PHPDBG_G(flags) & PHPDBG_IS_COLOURED) ? "\033[37m" : ""),
				            buffer,
				            ((PHPDBG_G(flags) & PHPDBG_IS_COLOURED) ? "\033[0m" : ""));
			} else {
			    rc = fprintf(fp, "\n");
			}
		} break;

		case P_WRITE: if (buffer) {
		    rc = fprintf(fp, "%s%s%s",
		                ((PHPDBG_G(flags) & PHPDBG_IS_COLOURED) ? "\033[37m" : ""),
		                buffer,
		                ((PHPDBG_G(flags) & PHPDBG_IS_COLOURED) ? "\033[0m" : ""));
		} break;
		
		/* no formatting on logging output */
	    case P_LOG: if (buffer) {
	        struct timeval tp;
	        if (gettimeofday(&tp, NULL) == SUCCESS) {
	            rc = fprintf(
	                fp, "[%ld %.8F]: %s\n", tp.tv_sec, tp.tv_usec / 1000000.00, buffer);
	        } else rc = FAILURE;
	    } break;
	}

	if (buffer) {
		efree(buffer);
	}

	return rc;
} /* }}} */
