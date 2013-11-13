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
#include "phpdbg_help.h"
#include "phpdbg_print.h"
#include "phpdbg_utils.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

PHPDBG_HELP(exec) /* {{{ */
{
	phpdbg_writeln("Will attempt execution, if compilation has not yet taken place, it occurs now.");
	phpdbg_writeln("The execution context must be set before execution can take place");
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(step) /* {{{ */
{
    phpdbg_writeln("You can enable and disable stepping at any phpdbg prompt during execution");
    phpdbg_writeln(EMPTY);
    phpdbg_writeln("Examples:");
    phpdbg_writeln("\t%sstepping 1", PROMPT);
    phpdbg_writeln("Will enable stepping");
    phpdbg_writeln(EMPTY);
    phpdbg_writeln("While stepping is enabled you are presented with a prompt after the execution of each opcode");
    return SUCCESS;
} /* }}} */

PHPDBG_HELP(next) /* {{{ */
{
	phpdbg_writeln("While stepping through execution, or after a breakpoint, use the next command to step back into the vm and execute the next opcode");
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(compile) /* {{{ */
{
	phpdbg_writeln("Pre-compilation of the execution context provides the opportunity to inspect the opcodes before they are executed");
	phpdbg_writeln("The execution context must be set for compilation to succeed");
	phpdbg_writeln("If errors occur during compilation they must be resolved before execution can take place.");
	phpdbg_writeln("It is a good idea to clean the environment between each compilation with the clean command");
	phpdbg_writeln("You do not need to exit phpdbg to retry compilation");
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(print) /* {{{ */
{
	phpdbg_writeln("By default, print will show information about the current execution environment");
	phpdbg_writeln("Other printing commands give access to address, file and line information");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Examples:");
	phpdbg_writeln("\t%sprint class \\my\\class", PROMPT);
	phpdbg_writeln("Will print information about \\my\\class, including the instructions for every method and their address");
	phpdbg_writeln("\t%sprint method \\my\\class::method", PROMPT);
	phpdbg_writeln("Will print the instructions for \\my\\class::method");
	phpdbg_writeln("\t%sprint func .getSomething", PROMPT);
	phpdbg_writeln("Will print the instructions for the method getSomething in the currently active scope");
	phpdbg_writeln("\t%sprint func my_function", PROMPT);
	phpdbg_writeln("Will print the instructions for the global function my_function");
	phpdbg_writeln("\t%sprint opline", PROMPT);
	phpdbg_writeln("Will print the instruction for the current opline");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Specific printers loaded are show below:");
	phpdbg_notice("Commands");
	{
	    const phpdbg_command_t *print_command = phpdbg_print_commands;
	    
	    while (print_command && print_command->name) {
			phpdbg_writeln("\t%s\t%s", print_command->name, print_command->tip);
			++print_command;
		}
	}
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(run) /* {{{ */
{
	phpdbg_writeln("Run the code inside the debug vm, you should have break points and variables set before running");
	phpdbg_writeln("The execution context must not be set, but not necessarily compiled before execution occurs");
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(eval) /* {{{ */
{
	phpdbg_writeln("Access to eval() allows you to change the environment during execution, careful though !!");
	phpdbg_writeln("Note: When using eval in phpdbg do not prefix the code with return.");
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(break) /* {{{ */
{
	phpdbg_writeln("Setting a breakpoint stops execution at a specific stage.");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Examples:");
	phpdbg_writeln("\t%sbreak test.php:1", PROMPT);
	phpdbg_writeln("Will break execution on line 1 of test.php");
	phpdbg_writeln("\t%sbreak my_function", PROMPT);
	phpdbg_writeln("Will break execution on entry to my_function");
	phpdbg_writeln("\t%sbreak \\my\\class::method", PROMPT);
	phpdbg_writeln("Will break execution on entry to \\my\\class::method");
	phpdbg_writeln("\t%sbreak 0x7ff68f570e08", PROMPT);
	phpdbg_writeln("Will break at the opline with the address provided (addresses are shown during execution)");
	phpdbg_writeln("\t%sbreak 200", PROMPT);
	phpdbg_writeln("Will break at line 200 of the currently executing file");
	phpdbg_writeln("\t%sbreak on ($expression == true)", PROMPT);
	phpdbg_writeln("Will break when the condition evaluates to true");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("It is important to note, an address is only valid for the current compiled representation of the script");
	phpdbg_writeln("If you have to clean the environment and recompile then your opline break points will be invalid");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Conditional breaks are costly, use them sparingly !!");
	
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(clean) /* {{{ */
{
    phpdbg_writeln("While debugging you may experience errors because of attempts to redeclare classes, constants or functions.");
    phpdbg_writeln("Cleaning the environment cleans these tables, so that files can be recompiled without exiting phpdbg.");
    return SUCCESS;
} /* }}} */

PHPDBG_HELP(clear) /* {{{ */
{
    phpdbg_writeln("Clearing breakpoints means you can once again run code without interruption");
    phpdbg_writeln("Careful though, all breakpoints are lost; be sure debugging is complete before clearing");
    return SUCCESS;
} /* }}} */

PHPDBG_HELP(quiet) /* {{{ */
{
    phpdbg_writeln("Setting quietness on will stop the OPLINE output during execution");
    phpdbg_writeln(EMPTY);
    phpdbg_writeln("Examples:");
    phpdbg_writeln("\t%squiet 1", PROMPT);
    phpdbg_writeln("Will silence OPLINE output, while");
    phpdbg_writeln("\t%squiet 0", PROMPT);
    phpdbg_writeln("Will enable OPLINE output again");
    phpdbg_writeln(EMPTY);
    phpdbg_writeln("Quietness is disabled while stepping through execution automatically");
    return SUCCESS;
} /* }}} */

PHPDBG_HELP(back) /* {{{ */
{
	phpdbg_writeln("The backtrace is gathered with the default debug_backtrace functionality.");
    phpdbg_writeln(EMPTY);
    phpdbg_writeln("Examples:");
	phpdbg_writeln("You can set the limit on the trace");
	phpdbg_writeln("\t%sback 5", PROMPT);
	phpdbg_writeln("Will limit the number of frames to 5, the default is no limit");
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(list) /* {{{ */
{
	phpdbg_writeln("The list command displays N line from current context file.");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Examples:");
	phpdbg_writeln("\t%slist 2", PROMPT);
	phpdbg_writeln("Will print next 2 lines from the current file");
	phpdbg_writeln("\t%slist func", PROMPT);
	phpdbg_writeln("Will print the source of the global function \"func\"");
	phpdbg_writeln("\t%slist .mine", PROMPT);
	phpdbg_writeln("Will print the source of the class method \"mine\"");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Note: before listing functions you must have a populated function table, try compile !!");
	return SUCCESS;
} /* }}} */
