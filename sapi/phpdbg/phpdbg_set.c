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
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#include "phpdbg.h"
#include "phpdbg_cmd.h"
#include "phpdbg_set.h"
#include "phpdbg_utils.h"
#include "phpdbg_bp.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

PHPDBG_SET(prompt) /* {{{ */
{
	switch (param->type) {
		case EMPTY_PARAM:
			phpdbg_writeln("%s", phpdbg_get_prompt(TSRMLS_C));
			break;

		case STR_PARAM:
			phpdbg_set_prompt(param->str TSRMLS_CC);
			break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

PHPDBG_SET(break) /* {{{ */
{
	switch (param->type) {
		case EMPTY_PARAM:
			phpdbg_writeln("%s",
				PHPDBG_G(flags) & PHPDBG_IS_BP_ENABLED ? "on" : "off");
			break;

		case STR_PARAM:
			if (strncasecmp(param->str, PHPDBG_STRL("on")) == 0) {
				phpdbg_enable_breakpoints(TSRMLS_C);
			} else if (strncasecmp(param->str, PHPDBG_STRL("off")) == 0) {
				phpdbg_disable_breakpoints(TSRMLS_C);
			}
			break;
			
		case NUMERIC_PARAM: {
			if (input->argc > 2) {
					if (phpdbg_argv_is(2, "on")) {
						phpdbg_enable_breakpoint(param->num TSRMLS_CC);
					} else if (phpdbg_argv_is(2, "off")) {
						phpdbg_disable_breakpoint(param->num TSRMLS_CC);
					}
			} else {
				phpdbg_breakbase_t *brake = phpdbg_find_breakbase(param->num TSRMLS_CC);
				if (brake) {
					phpdbg_writeln(
						"%s", brake->disabled ? "off" : "on");
				} else {
					phpdbg_error("Failed to find breakpoint #%lx", param->num);
				}
			}
		} break;

		default:
			phpdbg_error(
				"set break used incorrectly: set break [id] <on|off>");
	}

	return SUCCESS;
} /* }}} */

#ifndef _WIN32
PHPDBG_SET(color) /* {{{ */
{
	if ((param->type == STR_PARAM) && (input->argc == 3)) {
		const phpdbg_color_t *color = phpdbg_get_color(
			input->argv[2]->string, input->argv[2]->length TSRMLS_CC);
		int element = PHPDBG_COLOR_INVALID;

		/* @TODO(anyone) make this consistent with other set commands */
		if (color) {
			if (phpdbg_argv_is(1, "prompt")) {
				phpdbg_notice(
					"setting prompt color to %s (%s)", color->name, color->code);
				element = PHPDBG_COLOR_PROMPT;
				if (PHPDBG_G(prompt)[1]) {
					free(PHPDBG_G(prompt)[1]);
					PHPDBG_G(prompt)[1]=NULL;
				}
			} else if (phpdbg_argv_is(1, "error")) {
				phpdbg_notice(
					"setting error color to %s (%s)", color->name, color->code);
				element = PHPDBG_COLOR_ERROR;

			} else if (phpdbg_argv_is(1, "notice")) {
				phpdbg_notice(
					"setting notice color to %s (%s)", color->name, color->code);
				element = PHPDBG_COLOR_NOTICE;

			} else goto usage;

			/* set color for element */
			phpdbg_set_color(element, color TSRMLS_CC);
		} else {
			phpdbg_error(
				"Failed to find the requested color (%s)", input->argv[2]->string);
		}
	} else {
usage:
		phpdbg_error(
			"set color used incorrectly: set color <prompt|error|notice> <color>");
	}
	return SUCCESS;
} /* }}} */

PHPDBG_SET(colors) /* {{{ */
{
	switch (param->type) {
		case EMPTY_PARAM: {
			phpdbg_writeln(
				"%s", PHPDBG_G(flags) & PHPDBG_IS_COLOURED ? "on" : "off");
			goto done;
		}
		
		case STR_PARAM: {
			if (strncasecmp(param->str, PHPDBG_STRL("on")) == 0) {
				PHPDBG_G(flags) |= PHPDBG_IS_COLOURED;
				goto done;
			} else if (strncasecmp(param->str, PHPDBG_STRL("off")) == 0) {
				PHPDBG_G(flags) &= ~PHPDBG_IS_COLOURED;
				goto done;
			}
		}
		
		default:
			phpdbg_error(
				"set colors used incorrectly: set colors <on|off>");
	}

done:
	return SUCCESS;
} /* }}} */
#endif

PHPDBG_SET(oplog) /* {{{ */
{
	switch (param->type) {
		case EMPTY_PARAM:
			phpdbg_notice(
				"Oplog %s", PHPDBG_G(oplog) ? "enabled" : "disabled");
		break;

		case NUMERIC_PARAM: switch (param->num) {
			case 1:
				phpdbg_error(
					"An output file must be provided to enable oplog");
			break;

			case 0: {
				if (PHPDBG_G(oplog)) {
					phpdbg_notice("Disabling oplog");
					fclose(
						PHPDBG_G(oplog));
				} else {
					phpdbg_error("Oplog is not enabled!");
				}
			} break;
		} break;

		case STR_PARAM: {
			/* open oplog */
			FILE *old = PHPDBG_G(oplog);

			PHPDBG_G(oplog) = fopen(param->str, "w+");
			if (!PHPDBG_G(oplog)) {
				phpdbg_error("Failed to open %s for oplog", param->str);
				PHPDBG_G(oplog) = old;
			} else {
				if (old) {
					phpdbg_notice("Closing previously open oplog");
					fclose(old);
				}
				phpdbg_notice("Successfully opened oplog %s", param->str);
			}
		} break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

