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

#include "phpdbg.h"
#include "phpdbg_cmd.h"
#include "phpdbg_utils.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

const char *phpdbg_get_param_type(const phpdbg_param_t *param TSRMLS_DC) /* {{{ */
{
	switch (param->type) {
		case EMPTY_PARAM:
			return "empty";
		case ADDR_PARAM:
			return "address";
		case NUMERIC_PARAM:
			return "numeric";
		case METHOD_PARAM:
			return "method";
		case FILE_PARAM:
			return "file";
		case STR_PARAM:
			return "string";
		default: /* this is bad */
			return "unknown";
    }
}

phpdbg_param_type phpdbg_parse_param(const char *str, size_t len, phpdbg_param_t *param TSRMLS_DC) /* {{{ */
{
	char *class_name, *func_name;

	if (len == 0) {
		param->type = EMPTY_PARAM;
		goto parsed;
	}

	if (phpdbg_is_addr(str)) {

		param->addr = strtoul(str, 0, 16);
		param->type = ADDR_PARAM;
		goto parsed;

	} else if (phpdbg_is_numeric(str)) {

		param->num = strtol(str, NULL, 0);
		param->type = NUMERIC_PARAM;
        goto parsed;

	} else if (phpdbg_is_class_method(str, len+1, &class_name, &func_name)) {

		param->method.class = class_name;
		param->method.name = func_name;
		param->type = METHOD_PARAM;
		goto parsed;

	} else {
		const char *line_pos = strchr(str, ':');

		if (line_pos && phpdbg_is_numeric(line_pos+1)) {
			char path[MAXPATHLEN];

			memcpy(path, str, line_pos - str);
			path[line_pos - str] = 0;

			param->file.name = phpdbg_resolve_path(path TSRMLS_CC);
			param->file.line = strtol(line_pos+1, NULL, 0);
			param->type = FILE_PARAM;
			goto parsed;
		}
	}

	param->str = estrndup(str, len);
	param->len = len;
	param->type = STR_PARAM;

parsed:
    phpdbg_debug("phpdbg_parse_param(\"%s\", %lu): %s", str, len, phpdbg_get_param_type(param TSRMLS_CC));
	return param->type;
} /* }}} */

void phpdbg_clear_param(phpdbg_param_t *param TSRMLS_DC) /* {{{ */
{
	switch (param->type) {
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

int phpdbg_do_cmd(	const phpdbg_command_t *command,
					phpdbg_command_t **selected,
					char *cmd_line, size_t cmd_len TSRMLS_DC) /* {{{ */
{
	int rc = FAILURE;

	char *expr = NULL;
#ifndef _WIN32
	const char *cmd = strtok_r(cmd_line, " ", &expr);
#else
	const char *cmd = strtok_s(cmd_line, " ", &expr);
#endif
	size_t expr_len = (cmd != NULL) ? strlen(cmd) : 0;

	phpdbg_param_t *param = NULL;

	while (command && command->name && command->handler) {
		if ((command->name_len == expr_len && memcmp(cmd, command->name, expr_len) == 0)
			|| (expr_len == 1 && command->alias && command->alias == cmd_line[0])) {

			param = emalloc(sizeof(phpdbg_param_t));

			PHPDBG_G(last) = (phpdbg_command_t*) command;

		    /* urm ... */
			if (PHPDBG_G(lparam)) {
		        //phpdbg_clear_param(
		        //    PHPDBG_G(lparam) TSRMLS_CC);
		        //efree(PHPDBG_G(lparam));
			}

			phpdbg_parse_param(
				expr,
				(cmd_len - expr_len) ? (((cmd_len - expr_len) - sizeof(" "))+1) : 0,
				param TSRMLS_CC);

			PHPDBG_G(lparam) = param;

			if (command->subs && param->type == STR_PARAM) {
				if (phpdbg_do_cmd(command->subs, selected, param->str, param->len TSRMLS_CC) == SUCCESS) {
					rc = SUCCESS;
					/* because we can */
					phpdbg_clear_param(param TSRMLS_CC);
					efree(param);
					goto done;
				}
			}

			*selected = (phpdbg_command_t*) command;

			if (command->has_args == REQUIRED_ARG && param->type == EMPTY_PARAM) {
				phpdbg_error("This command requires argument!");
				rc = FAILURE;
			} else if (command->has_args == NO_ARG && param->type != EMPTY_PARAM) {
				phpdbg_error("This command does not expect argument!");
				rc = FAILURE;
			} else {
				rc = command->handler(param TSRMLS_CC);
			}
			break;
		}
		++command;
	}

done:
	if (selected && param) {
		phpdbg_debug(
			"phpdbg_do_cmd(%s, \"%s\"): %d",
			command->name, phpdbg_get_param_type(param TSRMLS_CC), (rc==SUCCESS));
	}

	return rc;
} /* }}} */
