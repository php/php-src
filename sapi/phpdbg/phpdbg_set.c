/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
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

ZEND_EXTERN_MODULE_GLOBALS(phpdbg)

#define PHPDBG_SET_COMMAND_D(f, h, a, m, l, s, flags) \
	PHPDBG_COMMAND_D_EXP(f, h, a, m, l, s, &phpdbg_prompt_commands[17], flags)

const phpdbg_command_t phpdbg_set_commands[] = {
	PHPDBG_SET_COMMAND_D(prompt,       "usage: set prompt [<string>]",            'p', set_prompt,       NULL, "|s", 0),
	PHPDBG_SET_COMMAND_D(pagination,   "usage: set pagination [<on|off>]",        'P', set_pagination,   NULL, "|b", PHPDBG_ASYNC_SAFE),
#ifndef _WIN32
	PHPDBG_SET_COMMAND_D(color,        "usage: set color  <element> <color>",     'c', set_color,        NULL, "ss", PHPDBG_ASYNC_SAFE),
	PHPDBG_SET_COMMAND_D(colors,       "usage: set colors [<on|off>]",            'C', set_colors,       NULL, "|b", PHPDBG_ASYNC_SAFE),
#endif
	PHPDBG_SET_COMMAND_D(break,        "usage: set break id [<on|off>]",          'b', set_break,        NULL, "l|b", PHPDBG_ASYNC_SAFE),
	PHPDBG_SET_COMMAND_D(breaks,       "usage: set breaks [<on|off>]",            'B', set_breaks,       NULL, "|b", PHPDBG_ASYNC_SAFE),
	PHPDBG_SET_COMMAND_D(quiet,        "usage: set quiet [<on|off>]",             'q', set_quiet,        NULL, "|b", PHPDBG_ASYNC_SAFE),
	PHPDBG_SET_COMMAND_D(stepping,     "usage: set stepping [<line|op>]",         's', set_stepping,     NULL, "|s", PHPDBG_ASYNC_SAFE),
	PHPDBG_SET_COMMAND_D(refcount,     "usage: set refcount [<on|off>]",          'r', set_refcount,     NULL, "|b", PHPDBG_ASYNC_SAFE),
	PHPDBG_SET_COMMAND_D(lines,        "usage: set lines [<number>]",             'l', set_lines,        NULL, "|l", PHPDBG_ASYNC_SAFE),
	PHPDBG_END_COMMAND
};

PHPDBG_SET(prompt) /* {{{ */
{
	if (!param || param->type == EMPTY_PARAM) {
		phpdbg_writeln("Current prompt: %s", phpdbg_get_prompt());
	} else {
		phpdbg_set_prompt(param->str);
	}

	return SUCCESS;
} /* }}} */

PHPDBG_SET(pagination) /* {{{ */
{
	if (!param || param->type == EMPTY_PARAM) {
		phpdbg_writeln("Pagination %s", PHPDBG_G(flags) & PHPDBG_HAS_PAGINATION ? "on" : "off");
	} else switch (param->type) {
		case NUMERIC_PARAM: {
			if (param->num) {
				PHPDBG_G(flags) |= PHPDBG_HAS_PAGINATION;
			} else {
				PHPDBG_G(flags) &= ~PHPDBG_HAS_PAGINATION;
			}
		} break;

		default:
			phpdbg_error("set pagination used incorrectly: set pagination <on|off>");
	}

	return SUCCESS;
} /* }}} */

PHPDBG_SET(lines) /* {{{ */
{
	if (!param || param->type == EMPTY_PARAM) {
		phpdbg_writeln("Lines "ZEND_ULONG_FMT, PHPDBG_G(lines));
	} else switch (param->type) {
		case NUMERIC_PARAM: {
			PHPDBG_G(lines) = param->num;
		} break;

		default:
			phpdbg_error("set lines used incorrectly: set lines <number>");
	}

	return SUCCESS;
} /* }}} */

PHPDBG_SET(break) /* {{{ */
{
	switch (param->type) {
		case NUMERIC_PARAM: {
			if (param->next) {
				if (param->next->num) {
					phpdbg_enable_breakpoint(param->num);
				} else {
					phpdbg_disable_breakpoint(param->num);
				}
			} else {
				phpdbg_breakbase_t *brake = phpdbg_find_breakbase(param->num);
				if (brake) {
					phpdbg_writeln("Breakpoint #"ZEND_LONG_FMT" %s", param->num, brake->disabled ? "off" : "on");
				} else {
					phpdbg_error("Failed to find breakpoint #"ZEND_LONG_FMT, param->num);
				}
			}
		} break;

		default:
			phpdbg_error("set break used incorrectly: set break [id] <on|off>");
	}

	return SUCCESS;
} /* }}} */

PHPDBG_SET(breaks) /* {{{ */
{
	if (!param || param->type == EMPTY_PARAM) {
		phpdbg_writeln("Breakpoints %s",PHPDBG_G(flags) & PHPDBG_IS_BP_ENABLED ? "on" : "off");
	} else switch (param->type) {
		case NUMERIC_PARAM: {
			if (param->num) {
				phpdbg_enable_breakpoints();
			} else {
				phpdbg_disable_breakpoints();
			}
		} break;

		default:
			phpdbg_error("set breaks used incorrectly: set breaks <on|off>");
	}

	return SUCCESS;
} /* }}} */

#ifndef _WIN32
PHPDBG_SET(color) /* {{{ */
{
	const phpdbg_color_t *color = phpdbg_get_color(param->next->str, param->next->len);

	if (!color) {
		phpdbg_error("Failed to find the requested color (%s)", param->next->str);
		return SUCCESS;
	}

	switch (phpdbg_get_element(param->str, param->len)) {
		case PHPDBG_COLOR_PROMPT:
			phpdbg_notice("setting prompt color to %s (%s)", color->name, color->code);
			if (PHPDBG_G(prompt)[1]) {
				free(PHPDBG_G(prompt)[1]);
				PHPDBG_G(prompt)[1]=NULL;
			}
			phpdbg_set_color(PHPDBG_COLOR_PROMPT, color);
		break;

		case PHPDBG_COLOR_ERROR:
			phpdbg_notice("setting error color to %s (%s)", color->name, color->code);
			phpdbg_set_color(PHPDBG_COLOR_ERROR, color);
		break;

		case PHPDBG_COLOR_NOTICE:
			phpdbg_notice("setting notice color to %s (%s)", color->name, color->code);
			phpdbg_set_color(PHPDBG_COLOR_NOTICE, color);
		break;

		default:
			phpdbg_error("Failed to find the requested element (%s)", param->str);
	}

	return SUCCESS;
} /* }}} */

PHPDBG_SET(colors) /* {{{ */
{
	if (!param || param->type == EMPTY_PARAM) {
		phpdbg_writeln("Colors %s", PHPDBG_G(flags) & PHPDBG_IS_COLOURED ? "on" : "off");
	} else switch (param->type) {
		case NUMERIC_PARAM: {
			if (param->num) {
				PHPDBG_G(flags) |= PHPDBG_IS_COLOURED;
			} else {
				PHPDBG_G(flags) &= ~PHPDBG_IS_COLOURED;
			}
		} break;

		default:
			phpdbg_error("set colors used incorrectly: set colors <on|off>");
	}

	return SUCCESS;
} /* }}} */
#endif

PHPDBG_SET(quiet) /* {{{ */
{
	if (!param || param->type == EMPTY_PARAM) {
		phpdbg_writeln("Quietness %s", PHPDBG_G(flags) & PHPDBG_IS_QUIET ? "on" : "off");
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
		phpdbg_writeln("Stepping %s", PHPDBG_G(flags) & PHPDBG_STEP_OPCODE ? "opcode" : "line");
	} else switch (param->type) {
		case STR_PARAM: {
			if (param->len == sizeof("opcode") - 1 && !memcmp(param->str, "opcode", sizeof("opcode"))) {
				PHPDBG_G(flags) |= PHPDBG_STEP_OPCODE;
			} else if (param->len == sizeof("line") - 1 && !memcmp(param->str, "line", sizeof("line"))) {
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
		phpdbg_writeln("Showing refcounts %s", PHPDBG_G(flags) & PHPDBG_IS_QUIET ? "on" : "off");
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
