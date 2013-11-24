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
#include "phpdbg_set.h"
#include "phpdbg_utils.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

void phpdbg_set_prompt(const char *prompt, const char *color TSRMLS_DC) /* {{{ */
{
	char *old_prompt_raw = PHPDBG_G(prompt_raw);

	if (PHPDBG_G(prompt)) {
		efree(PHPDBG_G(prompt));
		PHPDBG_G(prompt) = NULL;
	}

	if (color) {
		if (PHPDBG_G(prompt_color)) {
			efree(PHPDBG_G(prompt_color));
		}
		PHPDBG_G(prompt_color) = estrdup(color);
	}

	if (PHPDBG_G(flags) & PHPDBG_IS_COLOURED) {
		spprintf(&PHPDBG_G(prompt), 0, "\033[%sm%s\033[0m ",
			PHPDBG_G(prompt_color) ? PHPDBG_G(prompt_color) : "1;64", prompt);
	} else {
		spprintf(&PHPDBG_G(prompt), 0, "%s ", prompt);
	}

	PHPDBG_G(prompt_raw) = estrdup(prompt);

	if (old_prompt_raw) {
		efree(old_prompt_raw);
	}
} /* }}} */

const char *phpdbg_get_prompt(TSRMLS_D) /* {{{ */
{
	return PHPDBG_G(prompt);
} /* }}} */

void phpdbg_set_prompt_color(const char *color TSRMLS_DC) /* {{{ */
{
	if (memcmp(color, PHPDBG_STRL("blue")) == 0) {
		PHPDBG_G(prompt_color) = estrndup("blue", sizeof("blue")-1);
		phpdbg_set_prompt(PHPDBG_G(prompt_raw), "0;34" TSRMLS_CC);
	} else if (memcmp(color, PHPDBG_STRL("green")) == 0) {
		PHPDBG_G(prompt_color) = estrndup("green", sizeof("green")-1);
		phpdbg_set_prompt(PHPDBG_G(prompt_raw), "0;32" TSRMLS_CC);
	}

} /* }}} */

const char* phpdbg_get_prompt_color(TSRMLS_D) /* {{{ */
{
	return PHPDBG_G(prompt_color);
} /* }}} */

PHPDBG_SET(prompt) /* {{{ */
{
	switch (param->type) {
		case EMPTY_PARAM:
			phpdbg_writeln(phpdbg_get_prompt(TSRMLS_C));
			break;

		case STR_PARAM:
			phpdbg_set_prompt(param->str, NULL TSRMLS_CC);
			break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

PHPDBG_SET(prompt_color) /* {{{ */
{
	switch (param->type) {
		case EMPTY_PARAM:
			phpdbg_writeln(phpdbg_get_prompt_color(TSRMLS_C));
			break;

		case STR_PARAM:
			phpdbg_set_prompt_color(param->str TSRMLS_CC);
			break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

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
					phpdbg_error("Oplog is not enabled !");
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

