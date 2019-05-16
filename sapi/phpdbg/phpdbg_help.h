/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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

#ifndef PHPDBG_HELP_H
#define PHPDBG_HELP_H

#include "TSRM.h"
#include "phpdbg.h"
#include "phpdbg_cmd.h"

#define PHPDBG_HELP(name) PHPDBG_COMMAND(help_##name)

/**
 * Helper Forward Declarations
 */
PHPDBG_HELP(aliases);

extern const phpdbg_command_t phpdbg_help_commands[];

#define phpdbg_help_header() \
	phpdbg_notice("version", "version=\"%s\"", "Welcome to phpdbg, the interactive PHP debugger, v%s", PHPDBG_VERSION);
#define phpdbg_help_footer() \
	phpdbg_notice("issues", "url=\"%s\"", "Please report bugs to <%s>", PHPDBG_ISSUES);

typedef struct _phpdbg_help_text_t {
	char *key;
	char *text;
} phpdbg_help_text_t;

extern phpdbg_help_text_t phpdbg_help_text[];

extern void phpdbg_do_help_cmd(char *type);
#endif /* PHPDBG_HELP_H */
