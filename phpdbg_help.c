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
#include "phpdbg_break.h"
#include "phpdbg_list.h"
#include "phpdbg_info.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

PHPDBG_HELP(exec) /* {{{ */
{
    phpdbg_help_header();
	phpdbg_writeln("Will attempt execution, if compilation has not yet taken place, it occurs now");
	phpdbg_writeln("The execution context must be set before execution can take place");
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(step) /* {{{ */
{
    phpdbg_help_header();
    phpdbg_writeln("You can enable and disable stepping at any phpdbg prompt during execution");
    phpdbg_writeln(EMPTY);
    phpdbg_writeln("Examples:");
    phpdbg_writeln("\t%sstepping 1", PROMPT);
    phpdbg_writeln("Will enable stepping");
    phpdbg_writeln(EMPTY);
    phpdbg_writeln("While stepping is enabled you are presented with a prompt after the execution of each opcode");
    phpdbg_help_footer();
    return SUCCESS;
} /* }}} */

PHPDBG_HELP(next) /* {{{ */
{
    phpdbg_help_header();
	phpdbg_write("While stepping through execution, or after a breakpoint, ");
	phpdbg_writeln("use the next command to step back into the vm and execute the next opcode");
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(until) /* {{{ */
{
    phpdbg_help_header();
	phpdbg_write("While stepping through execution, or after a breakpoint, ");
	phpdbg_writeln("use the until command to step back into the vm and reaches the next source line");
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(finish) /* {{{ */
{
    phpdbg_help_header();
	phpdbg_write("While stepping through execution, or after a breakpoint, ");
	phpdbg_writeln("use the finish command to step back into the vm and continue until the current scope has returned");
	phpdbg_writeln("Note: this allows all breakpoints that would otherwise break execution in the current scope to be skipped");
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(leave) /* {{{ */
{
    phpdbg_help_header();
	phpdbg_write("While stepping through execution, or after a breakpoint, ");
	phpdbg_writeln("use the finish command to step back into the vm and continue until the current scope is returning");
	phpdbg_writeln("Note: this allows inspection of the return value from any scope before it is returned");
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(compile) /* {{{ */
{
    phpdbg_help_header();
	phpdbg_writeln("Pre-compilation of the execution context provides the opportunity to inspect the opcodes before they are executed");
	phpdbg_writeln("The execution context must be set for compilation to succeed");
	phpdbg_writeln("If errors occur during compilation they must be resolved before execution can take place");
	phpdbg_writeln("It is a good idea to clean the environment between each compilation with the clean command");
	phpdbg_writeln("You do not need to exit phpdbg to retry compilation");
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(print) /* {{{ */
{
    phpdbg_help_header();
	phpdbg_writeln("By default, print will show information about the current execution context");
	phpdbg_writeln("Other printing commands give access to instruction information");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Examples:");
	phpdbg_writeln("\t%sprint class \\my\\class", PROMPT);
	phpdbg_writeln("Will print the instructions for the methods in \\my\\class");
	phpdbg_writeln("\t%sprint method \\my\\class::method", PROMPT);
	phpdbg_writeln("Will print the instructions for \\my\\class::method");
	phpdbg_writeln("\t%sprint func .getSomething", PROMPT);
	phpdbg_writeln("Will print the instructions for ::getSomething in the currently active scope");
	phpdbg_writeln("\t%sprint func my_function", PROMPT);
	phpdbg_writeln("Will print the instructions for the global function my_function");
	phpdbg_writeln("\t%sprint opline", PROMPT);
	phpdbg_writeln("Will print the instruction for the current opline");
	phpdbg_writeln("\t%sprint exec", PROMPT);
	phpdbg_writeln("Will print the instructions for the execution context");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Specific printers loaded are show below:");
	phpdbg_notice("Commands");
	{
	    const phpdbg_command_t *print_command = phpdbg_print_commands;

        phpdbg_writeln("\tAlias\tCommand\t\tPurpose");
	    while (print_command && print_command->name) {
			if (print_command->alias) {
			    phpdbg_writeln("\t[%c]\t%s\t\t%s", print_command->alias, print_command->name, print_command->tip);
			} else phpdbg_writeln("\t[ ]\t%s\t\t%s", print_command->name, print_command->tip);
			++print_command;
		}
	}
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(run) /* {{{ */
{
    phpdbg_help_header();
	phpdbg_writeln("Run the code inside the debug vm, you should have break points and variables set before running");
	phpdbg_writeln("The execution context must not be set, but not necessarily compiled before execution occurs");
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(eval) /* {{{ */
{
    phpdbg_help_header();
	phpdbg_writeln("Access to eval() allows you to change the environment during execution, careful though !!");
	phpdbg_writeln("Note: When using eval in phpdbg do not prefix the code with return");
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(break) /* {{{ */
{
    phpdbg_help_header();
	phpdbg_writeln("Setting a breakpoint stops execution at a specific stage");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Examples:");
	phpdbg_writeln("\t%sbreak [file] test.php:1", PROMPT);
	phpdbg_writeln("Will break execution on line 1 of test.php");
	phpdbg_writeln("\t%sbreak [func] my_function", PROMPT);
	phpdbg_writeln("Will break execution on entry to my_function");
	phpdbg_writeln("\t%sbreak [method] \\my\\class::method", PROMPT);
	phpdbg_writeln("Will break execution on entry to \\my\\class::method");
	phpdbg_writeln("\t%sbreak [address] 0x7ff68f570e08", PROMPT);
	phpdbg_writeln("Will break at the opline with the address provided (addresses are shown during execution)");
	phpdbg_writeln("\t%sbreak [lineno] 200", PROMPT);
	phpdbg_writeln("Will break at line 200 of the currently executing file");
	phpdbg_writeln("\t%sbreak on ($expression == true)", PROMPT);
	phpdbg_writeln("Will break when the condition evaluates to true");
	phpdbg_notice("The parameters enclosed by [] are usually optional, but help avoid ambigious commands");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("It is important to note, an address is only valid for the current compiled representation of the script");
	phpdbg_writeln("If you have to clean the environment and recompile then your opline break points will be invalid");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Specific breakers loaded are show below:");
	phpdbg_notice("Commands");
	{
	    const phpdbg_command_t *break_command = phpdbg_break_commands;

        phpdbg_writeln("\tAlias\tCommand\t\tPurpose");
	    while (break_command && break_command->name) {
			if (break_command->alias) {
			    phpdbg_writeln("\t[%c]\t%s\t\t%s", break_command->alias, break_command->name, break_command->tip);
			} else phpdbg_writeln("\t[ ]\t%s\t\t%s", break_command->name, break_command->tip);
			++break_command;
		}
	}
	phpdbg_writeln("Conditional breaks are costly, use them sparingly !!");
    phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(clean) /* {{{ */
{
    phpdbg_help_header();
    phpdbg_writeln("While debugging you may experience errors because of attempts to redeclare classes, constants or functions");
    phpdbg_writeln("Cleaning the environment cleans these tables, so that files can be recompiled without exiting phpdbg");
    phpdbg_help_footer();
    return SUCCESS;
} /* }}} */

PHPDBG_HELP(clear) /* {{{ */
{
    phpdbg_help_header();
    phpdbg_writeln("Clearing breakpoints means you can once again run code without interruption");
    phpdbg_writeln("Careful though, all breakpoints are lost; be sure debugging is complete before clearing");
    phpdbg_help_footer();
    return SUCCESS;
} /* }}} */

PHPDBG_HELP(info) /* {{{ */
{
    phpdbg_help_header();
    phpdbg_writeln("info commands provide quick access to various types of information about the PHP environment");
    phpdbg_writeln("Specific info commands are show below:");
	phpdbg_notice("Commands");
	{
	    const phpdbg_command_t *info_command = phpdbg_info_commands;

        phpdbg_writeln("\tAlias\tCommand\t\tPurpose");
	    while (info_command && info_command->name) {
			if (info_command->alias) {
			    phpdbg_writeln("\t[%c]\t%s\t\t%s", info_command->alias, info_command->name, info_command->tip);
			} else phpdbg_writeln("\t[ ]\t%s\t\t%s", info_command->name, info_command->tip);
			++info_command;
		}
	}
	
    phpdbg_help_footer();
    return SUCCESS;
} /* }}} */

PHPDBG_HELP(quiet) /* {{{ */
{
    phpdbg_help_header();
    phpdbg_writeln("Setting quietness on will stop the OPLINE output during execution");
    phpdbg_writeln(EMPTY);
    phpdbg_writeln("Examples:");
    phpdbg_writeln("\t%squiet 1", PROMPT);
    phpdbg_writeln("Will silence OPLINE output, while");
    phpdbg_writeln("\t%squiet 0", PROMPT);
    phpdbg_writeln("Will enable OPLINE output again");
    phpdbg_writeln(EMPTY);
    phpdbg_writeln("Quietness is disabled while stepping through execution automatically");
    phpdbg_help_footer();
    return SUCCESS;
} /* }}} */

PHPDBG_HELP(back) /* {{{ */
{
    phpdbg_help_header();
	phpdbg_writeln("The backtrace is gathered with the default debug_backtrace functionality");
    phpdbg_writeln(EMPTY);
    phpdbg_writeln("Examples:");
	phpdbg_writeln("You can set the limit on the trace");
	phpdbg_writeln("\t%sback 5", PROMPT);
	phpdbg_writeln("Will limit the number of frames to 5, the default is no limit");
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(list) /* {{{ */
{
    phpdbg_help_header();
	phpdbg_writeln("The list command displays source code for the given argument");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Examples:");
	phpdbg_writeln("\t%slist [lines] 2", PROMPT);
	phpdbg_writeln("Will print next 2 lines from the current file");
	phpdbg_writeln("\t%slist [func] my_function", PROMPT);
	phpdbg_writeln("Will print the source of the global function \"my_function\"");
	phpdbg_writeln("\t%slist [func] .mine", PROMPT);
	phpdbg_writeln("Will print the source of the method \"mine\" from the currently active scope");
	phpdbg_writeln("\t%slist [method] my::method", PROMPT);
	phpdbg_writeln("Will print the source of \"my::method\"");
	phpdbg_writeln("\t%slist [class] myClass", PROMPT);
	phpdbg_writeln("Will print the source of \"myClass\"");
	phpdbg_notice("The parameters enclosed by [] are usually optional, but help avoid ambigious commands");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Note: before listing functions you must have a populated function table, try compile !!");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Specific listers loaded are show below:");
	phpdbg_notice("Commands");
	{
	    const phpdbg_command_t *list_command = phpdbg_list_commands;

        phpdbg_writeln("\tAlias\tCommand\t\tPurpose");
	    while (list_command && list_command->name) {
			if (list_command->alias) {
			    phpdbg_writeln("\t[%c]\t%s\t\t%s", list_command->alias, list_command->name, list_command->tip);
			} else phpdbg_writeln("\t[ ]\t%s\t\t%s", list_command->name, list_command->tip);
			++list_command;
		}
	}
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(oplog) /* {{{ */
{
    phpdbg_help_header();
    phpdbg_writeln("Even when quietness is enabled you may wish to save opline logs to a file");
    phpdbg_writeln("Setting a new oplog closes the previously open log");
    phpdbg_writeln("The log includes a high resolution timestamp on each entry");
	phpdbg_writeln(EMPTY);
    phpdbg_writeln("Example:");
    phpdbg_writeln("\t%soplog /path/to/my.oplog", PROMPT);
    phpdbg_writeln("Will open the file /path/to/my.oplog for writing, creating it if it does not exist");
    phpdbg_writeln("\t%soplog 0", PROMPT);
    phpdbg_writeln("Will close the currently open log file, disabling oplog");
	phpdbg_writeln(EMPTY);
    phpdbg_writeln("Note: upon failure to open a new oplog, the last oplog is held open");
    phpdbg_help_footer();
    return SUCCESS;
} /* }}} */

PHPDBG_HELP(shell) /* {{{ */
{
	phpdbg_help_header();
    phpdbg_writeln("Direct access to shell commands saves having to switch windows/consoles");
	phpdbg_writeln(EMPTY);
    phpdbg_writeln("Example:");
    phpdbg_writeln("\t%s- ls /usr/src/php-src", PROMPT);
    phpdbg_writeln("Will execute ls /usr/src/php-src, displaying the output in the console");
	phpdbg_writeln(EMPTY);
    phpdbg_writeln("Note: read only commands please !");
    phpdbg_help_footer();
    return SUCCESS;
} /* }}} */
