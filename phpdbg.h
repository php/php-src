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
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   +----------------------------------------------------------------------+
*/

#ifndef PHPDBG_H
#define PHPDBG_H

#include "php.h"
#include "php_globals.h"
#include "php_variables.h"
#include "php_getopt.h"
#include "zend_builtin_functions.h"
#include "zend_extensions.h"
#include "zend_modules.h"
#include "zend_globals.h"
#include "zend_ini_scanner.h"
#include "zend_stream.h"
#include "SAPI.h"
#include <fcntl.h>
#include <sys/types.h>
#if defined(_WIN32) && !defined(__MINGW32__)
# include <windows.h>
# include "config.w32.h"
#else
# include "php_config.h"
#endif
#ifndef O_BINARY
#	define O_BINARY 0
#endif
#include "php_main.h"

#ifdef ZTS
# include "TSRM.h"
#endif

#ifdef HAVE_LIBREADLINE
#   include <readline/readline.h>
#   include <readline/history.h>
#endif

#ifdef ZTS
# define PHPDBG_G(v) TSRMG(phpdbg_globals_id, zend_phpdbg_globals *, v)
#else
# define PHPDBG_G(v) (phpdbg_globals.v)
#endif

#define PHPDBG_NEXT 2

/* {{{ tables */
#define PHPDBG_BREAK_FILE       0
#define PHPDBG_BREAK_SYM        1
#define PHPDBG_BREAK_OPLINE     2
#define PHPDBG_BREAK_METHOD     3
#define PHPDBG_BREAK_TABLES     4 /* }}} */

/* {{{ flags */
#define PHPDBG_HAS_FILE_BP      0x00000001
#define PHPDBG_HAS_SYM_BP       0x00000010
#define PHPDBG_HAS_OPLINE_BP    0x00000100
#define PHPDBG_HAS_METHOD_BP    0x00001000
#define PHPDBG_BP_MASK          (PHPDBG_HAS_FILE_BP|PHPDBG_HAS_SYM_BP|PHPDBG_HAS_METHOD_BP|PHPDBG_HAS_OPLINE_BP)

#define PHPDBG_IS_STEPPING      0x00010000
#define PHPDBG_IS_QUIET         0x00100000
#define PHPDBG_IS_QUITTING      0x01000000
#define PHPDBG_IS_COLOURED      0x10000000

#define PHPDBG_DEFAULT_FLAGS    (PHPDBG_IS_QUIET|PHPDBG_IS_COLOURED) /* }}} */

/* {{{ strings */
#define PHPDBG_ISSUES "http://github.com/krakjoe/phpdbg/issues"
#define PHPDBG_VERSION "0.0.0" /* }}} */

typedef struct _phpdbg_command_t phpdbg_command_t;

ZEND_BEGIN_MODULE_GLOBALS(phpdbg)
    HashTable bp[PHPDBG_BREAK_TABLES];  /* break points */
	char *exec;                         /* file to execute */
	size_t exec_len;                    /* size of exec */
	zend_op_array *ops;                 /* op_array */
	zval *retval;                       /* return value */
	int bp_count;                       /* breakpoint count */
	int vmret;                          /* return from last opcode handler execution */
	phpdbg_command_t *last;             /* last command */
	const char *last_params;            /* last expression */
	size_t last_params_len;             /* last expression length */
	zend_ulong flags;                   /* phpdbg flags */
ZEND_END_MODULE_GLOBALS(phpdbg)

#endif /* PHPDBG_H */
