/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
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

extern const phpdbg_help_text_t phpdbg_help_text[];

extern void phpdbg_do_help_cmd(const char *type);
#endif /* PHPDBG_HELP_H */
