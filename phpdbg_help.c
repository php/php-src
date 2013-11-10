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

PHPDBG_HELP(step) { /* {{{ */
    printf("You can enable and disable stepping at any phpdbg prompt during execution\n");
    printf("For example:\n");
    printf("phpdbg> stepping 1\n");
    printf("Will enable stepping\n");
    printf("While stepping is enabled you are presented with a prompt after the execution of each opcode\n");
    return SUCCESS;
} /* }}} */

PHPDBG_HELP(next) { /* {{{ */
    printf("While stepping through execution, use the next command to step back into the vm and execute the next opcode");
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
  printf("\tphpdbg> print opcodes[0]\n");
  printf("Will show the opline @ 0\n");
  printf("Available print commands:\n");
  printf("\tNone\n");

  return SUCCESS;
} /* }}} */

PHPDBG_HELP(run) /* {{{ */ 
{
    printf("Run the code inside the debug vm, you should have break points and variables set before running\n");
    printf("The execution context must not be set, but not necessarily compiled before execution occurs\n");
    return SUCCESS;
} /* }}} */

PHPDBG_HELP(break) /* {{{ */
{
  printf("doing break help: %s\n", expr);

  return SUCCESS;
} /* }}} */
