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

#include "phpdbg.h"
#include "phpdbg_cmd.h"
#include "phpdbg_set.h"
#include "phpdbg_utils.h"
#include "phpdbg_bp.h"
#include "phpdbg_prompt.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

#define PHPDBG_SET_COMMAND_D(f, h, a, m, l, s) \
	PHPDBG_COMMAND_D_EXP(f, h, a, m, l, s, &phpdbg_prompt_commands[18])

const phpdbg_command_t phpdbg_set_commands[] = {
	PHPDBG_SET_COMMAND_D(prompt,       "usage: set prompt [<string>]",            'p', set_prompt,       NULL, "|s"),
#ifndef _WIN32
	PHPDBG_SET_COMMAND_D(color,        "usage: set color  <element> <color>",     'c', set_color,        NULL, "ss"),
	PHPDBG_SET_COMMAND_D(colors,       "usage: set colors [<on|off>]",            'C', set_colors,       NULL, "|b"),
#endif
	PHPDBG_SET_COMMAND_D(oplog,        "usage: set oplog  [<output>]",            'O', set_oplog,        NULL, "|s"),
	PHPDBG_SET_COMMAND_D(break,        "usage: set break id [<on|off>]",          'b', set_break,        NULL, "l|b"),
	PHPDBG_SET_COMMAND_D(breaks,       "usage: set breaks [<on|off>]",            'B', set_breaks,       NULL, "|b"),
	PHPDBG_SET_COMMAND_D(quiet,        "usage: set quiet [<on|off>]",             'q', set_quiet,        NULL, "|b"),
	PHPDBG_SET_COMMAND_D(stepping,     "usage: set stepping [<line|op>]",         's', set_stepping,     NULL, "|s"),
	PHPDBG_SET_COMMAND_D(refcount,     "usage: set refcount [<on|off>]",          'r', set_refcount,     NULL, "|b"),
	PHPDBG_END_COMMAND
};

PHPDBG_SET(prompt) /* {{{ */
{
	if (!param || param->type == EMPTY_PARAM) {
		phpdbg_writeln("%s", phpdbg_get_prompt(TSRMLS_C));
	} else phpdbg_set_prompt(param->str TSRMLS_CC);
	
	return SUCCESS;
} /* }}} */

PHPDBG_SET(break) /* {{{ */
{
	switch (param->type) {
		case NUMERIC_PARAM: {
			if (param->next) {
				if (param->next->num) {
					phpdbg_enable_breakpoint(param->num TSRMLS_CC);
				} else phpdbg_disable_breakpoint(param->num TSRMLS_CC);
			} else {
				phpdbg_breakbase_t *brake = phpdbg_find_breakbase(param->num TSRMLS_CC);
				if (brake) {
					phpdbg_writeln(
						"%s", brake->disabled ? "off" : "on");
				} else {
					phpdbg_error("Failed to find breakpoint #%ld", param->num);
				}
			}
		} break;

		default:
			phpdbg_error(
				"set break used incorrectly: set break [id] <on|off>");
	}

	return SUCCESS;
} /* }}} */

PHPDBG_SET(breaks) /* {{{ */
{
	if (!param || param->type == EMPTY_PARAM) {
		phpdbg_writeln("%s",
			PHPDBG_G(flags) & PHPDBG_IS_BP_ENABLED ? "on" : "off");
	} else switch (param->type) {	
		case NUMERIC_PARAM: {
			if (param->num) {
				phpdbg_enable_breakpoints(TSRMLS_C);
			} else phpdbg_disable_breakpoints(TSRMLS_C);
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
	const phpdbg_color_t *color = phpdbg_get_color(
			param->next->str, param->next->len TSRMLS_CC);
	
	if (!color) {
		phpdbg_error(
			"Failed to find the requested color (%s)", param->next->str);
		return SUCCESS;
	}
	
	switch (phpdbg_get_element(param->str, param->len TSRMLS_CC)) {
		case PHPDBG_COLOR_PROMPT:
			phpdbg_notice(
				"setting prompt color to %s (%s)", color->name, color->code);
			if (PHPDBG_G(prompt)[1]) {
				free(PHPDBG_G(prompt)[1]);
				PHPDBG_G(prompt)[1]=NULL;
			}
			phpdbg_set_color(PHPDBG_COLOR_PROMPT, color TSRMLS_CC);
		break;
		
		case PHPDBG_COLOR_ERROR:
			phpdbg_notice(
				"setting error color to %s (%s)", color->name, color->code);
			phpdbg_set_color(PHPDBG_COLOR_ERROR, color TSRMLS_CC);
		break;
		
		case PHPDBG_COLOR_NOTICE:
			phpdbg_notice(
				"setting notice color to %s (%s)", color->name, color->code);
			phpdbg_set_color(PHPDBG_COLOR_NOTICE, color TSRMLS_CC);
		break;
		
		default:
			phpdbg_error(
				"Failed to find the requested element (%s)", param->str);
	}

	return SUCCESS;
} /* }}} */

PHPDBG_SET(colors) /* {{{ */
{
	if (!param || param->type == EMPTY_PARAM) {
		phpdbg_writeln("%s", PHPDBG_G(flags) & PHPDBG_IS_COLOURED ? "on" : "off");
	} else switch (param->type) {
		case NUMERIC_PARAM: {
			if (param->num) {
				PHPDBG_G(flags) |= PHPDBG_IS_COLOURED;
			} else {
				PHPDBG_G(flags) &= ~PHPDBG_IS_COLOURED;
			}
		} break;
		
		default:
			phpdbg_error(
				"set colors used incorrectly: set colors <on|off>");
	}

	return SUCCESS;
} /* }}} */
#endif

PHPDBG_SET(oplog) /* {{{ */
{
	if (!param || param->type == EMPTY_PARAM) {
		phpdbg_notice("Oplog %s", PHPDBG_G(oplog) ? "enabled" : "disabled");
	} else switch (param->type) {
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

PHPDBG_SET(quiet) /* {{{ */
{
	if (!param || param->type == EMPTY_PARAM) {
		phpdbg_writeln("Quietness %s", 
			PHPDBG_G(flags) & PHPDBG_IS_QUIET ? "on" : "off");
	} else switch (param->type) {
		case NUMERIC_PARAM: {
			if (param->num) {
				PHPDBG_G(flags) |= PHPDBG_IS_QUIET;
			} else {
				PHPDBG_G(flags) &= ~PHPDBG_IS_QUIET;
			}
		} break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

PHPDBG_SET(stepping) /* {{{ */
{
	if (!param || param->type == EMPTY_PARAM) {
		phpdbg_writeln("Stepping %s",
			PHPDBG_G(flags) & PHPDBG_STEP_OPCODE ? "opcode" : "line");
	} else switch (param->type) {
		case STR_PARAM: {	
			if ((param->len == sizeof("opcode")-1) &&
				(memcmp(param->str, "opcode", sizeof("opcode")) == SUCCESS)) {
				PHPDBG_G(flags) |= PHPDBG_STEP_OPCODE;
			} else if ((param->len == sizeof("line")-1) &&
				(memcmp(param->str, "line", sizeof("line")) == SUCCESS)) {
				PHPDBG_G(flags) &= ~PHPDBG_STEP_OPCODE;
			} else {
				phpdbg_error("usage set stepping [<opcode|line>]");
			}
		} break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

PHPDBG_SET(refcount) /* {{{ */
{
	if (!param || param->type == EMPTY_PARAM) {
		phpdbg_writeln("Refcount %s", PHPDBG_G(flags) & PHPDBG_IS_QUIET ? "on" : "off");
	} else switch (param->type) {
		case NUMERIC_PARAM: {
			if (param->num) {
				PHPDBG_G(flags) |= PHPDBG_SHOW_REFCOUNTS;
			} else {
				PHPDBG_G(flags) &= ~PHPDBG_SHOW_REFCOUNTS;
			}
		} break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */
