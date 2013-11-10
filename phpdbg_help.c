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

#include <stdio.h>
#include <string.h>
#include "zend.h"
#include "phpdbg.h"
#include "phpdbg_help.h"

PHPDBG_HELP(exec) /* {{{ */
{
  printf("Will attempt execution, if compilation has not yet taken place, it occurs now.\n");
  printf("The execution context must be set before execution can take place\n");
  return SUCCESS;
} /* }}} */

PHPDBG_HELP(compile) /* {{{ */
{
  printf("Pre-compilation of the execution context provides the opportunity to inspect the opcodes before they are executed\n");
  printf("The execution context must be set for compilation to succeed\n");
  printf("If errors occur during compilation they must be resolved before execution can take place.\n");
  printf("You do not need to exit phpdbg to retry compilation\n");
  return SUCCESS;
} /* }}} */

PHPDBG_HELP(print) /* {{{ */
{
  printf("By default, print will show information about the current execution environment\n");
  printf("To show specific information pass an expression to print, for example:\n");
  printf("\tprint opcodes[0]\n");
  printf("Will show the opline @ 0\n");
  printf("Available print commands:\n");
  printf("\tNone\n");

  return SUCCESS;
} /* }}} */

PHPDBG_HELP(break) /* {{{ */
{
  printf("doing break help: %s\n", expr);

  return SUCCESS;
} /* }}} */
