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
#include "phpdbg_set.h"
#include "phpdbg_utils.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

void phpdbg_set_prompt(const char *prompt TSRMLS_DC) /* {{{ */
{
	if (PHPDBG_G(prompt)) {
		efree(PHPDBG_G(prompt));
		PHPDBG_G(prompt) = NULL;
	}

	spprintf(&PHPDBG_G(prompt), 0, "%s ", prompt);
} /* }}} */

const char *phpdbg_get_prompt(TSRMLS_D) /* {{{ */
{
	return PHPDBG_G(prompt);
} /* }}} */

PHPDBG_SET(prompt) /* {{{ */
{
	switch (param->type) {
		case EMPTY_PARAM:
			phpdbg_writeln(phpdbg_get_prompt(TSRMLS_C));
			break;

		case STR_PARAM:
			phpdbg_set_prompt(param->str TSRMLS_CC);
			break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

