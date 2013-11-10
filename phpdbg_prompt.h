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
   +----------------------------------------------------------------------+
*/

#ifndef PHPDBG_PROMPT_H
#define PHPDBG_PROMPT_H

/**
 * Maximum command length
 */
#define PHPDBG_MAX_CMD 500

#define PHPDBG_STRL(s) s, sizeof(s)-1

/**
 * Command handler
 */
typedef void (*phpdbg_command_handler)(const char*);

/**
 * Command representation
 */
typedef struct _phpdbg_command {
	const char *name;                   /* Command name */
	size_t name_len;                    /* Command name length */
	phpdbg_command_handler handler;     /* Command handler */
} phpdbg_command;

#endif
