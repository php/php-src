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
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#include "phpdbg.h"
#include "phpdbg_help.h"
#include "phpdbg_print.h"
#include "phpdbg_utils.h"
#include "phpdbg_break.h"
#include "phpdbg_list.h"
#include "phpdbg_info.h"
#include "phpdbg_set.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

PHPDBG_HELP(exec) /* {{{ */
{
	phpdbg_help_header();
	phpdbg_writeln("\tWill attempt execution, if compilation has not yet taken place, it occurs now");
	phpdbg_writeln("The execution context must be set before execution can take place");
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(step) /* {{{ */
{
	phpdbg_help_header();
	phpdbg_writeln("You can enable and disable stepping at any phpdbg prompt during execution");
	phpdbg_writeln(EMPTY);
	phpdbg_notice("Examples");
	phpdbg_writeln("\t%sstepping 1", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%ss 1", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill enable stepping");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("While stepping is enabled you are presented with a prompt after the execution of each opcode");
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(next) /* {{{ */
{
	phpdbg_help_header();
	phpdbg_write("Step back into the vm and execute the next opcode");
	phpdbg_writeln(EMPTY);
	phpdbg_notice("Examples");
	phpdbg_writeln("\t%snext", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sn", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill cause control to be passed back to the vm, continuing execution");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Note: is only useful while executing");
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(until) /* {{{ */
{
	phpdbg_help_header();
	phpdbg_writeln("Step back into the vm, skipping breakpoints until the next source line");
	phpdbg_writeln(EMPTY);
	phpdbg_notice("Examples");
	phpdbg_writeln("\t%suntil", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%su", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill cause control to be passed back to the vm, continuing execution");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Note: is only useful while executing");
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(finish) /* {{{ */
{
	phpdbg_help_header();
	phpdbg_writeln("Step back into the vm, skipping breakpoints until past the end of the current stack");
	phpdbg_writeln(EMPTY);
	phpdbg_notice("Examples");
	phpdbg_writeln("\t%sfinish", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sF", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill cause control to be passed back to the vm, continuing execution");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Note: this allows all breakpoints that would otherwise break execution in the current scope to be skipped");
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(leave) /* {{{ */
{
	phpdbg_help_header();
	phpdbg_writeln("Step back into the vm, skipping breakpoints until the current stack is returning");
	phpdbg_writeln(EMPTY);
	phpdbg_notice("Examples");
	phpdbg_writeln("\t%sleave", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sL", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill cause a break when instructed to leave the current context");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Note: this allows inspection of the return value before it is returned");
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(compile) /* {{{ */
{
	phpdbg_help_header();
	phpdbg_writeln("Pre-compilation of the execution context provides the opportunity to inspect opcodes before execution");
	phpdbg_writeln("The execution context must be set for compilation to succeed");
	phpdbg_writeln(EMPTY);
	phpdbg_notice("Examples");
	phpdbg_writeln("\t%scompile", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sc", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill compile the current execution context, populating class/function/constant/etc tables");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Note: It is a good idea to clean the environment between each compilation");
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(print) /* {{{ */
{
	phpdbg_help_header();
	phpdbg_writeln("By default, print will show information about the current execution context");
	phpdbg_writeln("Other printing commands give access to instruction information");
	phpdbg_writeln(EMPTY);
	phpdbg_notice("Examples");
	phpdbg_writeln("\t%sprint class \\my\\class", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sp c \\my\\class", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill print the instructions for the methods in \\my\\class");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("\t%sprint method \\my\\class::method", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sp m \\my\\class::method", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill print the instructions for \\my\\class::method");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("\t%sprint func .getSomething", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sp f .getSomething", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill print the instructions for ::getSomething in the active scope");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("\t%sprint func my_function", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sp f my_function", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill print the instructions for the global function my_function");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("\t%sprint opline", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sp o", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill print the instruction for the current opline");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("\t%sprint exec", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sp e", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill print the instructions for the execution context");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("\t%sprint stack", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sp s", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill print the instructions for the current stack");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Specific printers loaded are show below:");
	phpdbg_notice("Commands");
	{
		const phpdbg_command_t *print_command = phpdbg_print_commands;

		phpdbg_writeln("\tAlias\tCommand\t\tPurpose");
		while (print_command && print_command->name) {
			if (print_command->alias) {
				phpdbg_writeln("\t[%c]\t%s\t\t%s", print_command->alias, print_command->name, print_command->tip);
			} else {
				phpdbg_writeln("\t[ ]\t%s\t\t%s", print_command->name, print_command->tip);
			}
			++print_command;
		}
	}
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(run) /* {{{ */
{
	phpdbg_help_header();
	phpdbg_writeln("Execute the current context inside the phpdbg vm");
	phpdbg_writeln(EMPTY);
	phpdbg_notice("Examples");
	phpdbg_writeln("\t%srun", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sr", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill cause execution of the context, if it is set.");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Note: The execution context must be set, but not necessarily compiled before execution occurs");
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(eval) /* {{{ */
{
	phpdbg_help_header();
	phpdbg_writeln("Access to eval() allows you to change the environment during execution, careful though!!");
	phpdbg_writeln(EMPTY);
	phpdbg_notice("Examples");
	phpdbg_writeln("\t%seval $variable", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sE $variable", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill print_r($variable) on the console, if it is defined");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("\t%seval $variable = \"Hello phpdbg :)\"", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sE $variable = \"Hello phpdbg :)\"", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill set $variable in the current scope");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Note: eval() will always show the result; do not prefix the code with \"return\"");
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(break) /* {{{ */
{
	phpdbg_help_header();
	phpdbg_writeln("Setting a breakpoint stops execution at a specific stage");
	phpdbg_writeln(EMPTY);
	phpdbg_notice("Examples");
	phpdbg_writeln("\t%sbreak [file] test.php:1", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sb [F] test.php:1", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill break execution on line 1 of test.php");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("\t%sbreak [func] my_function", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sb [f] my_function", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill break execution on entry to my_function");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("\t%sbreak [method] \\my\\class::method", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sb [m] \\my\\class::method", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill break execution on entry to \\my\\class::method");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("\t%sbreak [address] 0x7ff68f570e08", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sb [a] 0x7ff68f570e08", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill break at the opline with the address provided");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("\t%sbreak [address] my_function#1", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sb [a] my_function#1", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill break at the opline number 1 of the function my_function");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("\t%sbreak [address] \\my\\class::method#2", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sb [a] \\my\\class::method#2", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill break at the opline number 2 of the method \\my\\class::method");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("\t%sbreak address test.php:3", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sb a test.php:3", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill break at the opline number 3 of test.php");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("\t%sbreak [lineno] 200", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sb [l] 200", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill break at line 200 of the currently executing file");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("\t%sbreak on ($expression == true)", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sb on ($expression == true)", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill break when the condition evaluates to true");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("\t%sbreak at phpdbg::isGreat if ($expression == true)", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill break at every opcode in phpdbg::isGreat when the condition evaluates to true");
	phpdbg_writeln("\t%sbreak at test.php:20 if ($expression == true)", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill break at every opcode on line 20 of test.php when the condition evaluates to true");
	phpdbg_write("\t");
	phpdbg_notice("The location can be anything accepted by file, func, method, or address break commands");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("\t%sbreak op ZEND_ADD", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sb O ZEND_ADD", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill break on every occurence of the opcode provided");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("\t%sbreak del 1", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sb d 1", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill remove the breakpoint with the given identifier");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Note: An address is only valid for the current compilation");
	phpdbg_writeln(EMPTY);
	phpdbg_notice("The parameters enclosed by [] are usually optional, but help avoid ambigious commands");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Specific breakers loaded are show below:");
	phpdbg_notice("Commands");
	{
		const phpdbg_command_t *break_command = phpdbg_break_commands;

		phpdbg_writeln("\tAlias\tCommand\t\tPurpose");
		while (break_command && break_command->name) {
			if (break_command->alias) {
				phpdbg_writeln("\t[%c]\t%s\t\t%s", break_command->alias, break_command->name, break_command->tip);
			} else {
				phpdbg_writeln("\t[ ]\t%s\t\t%s", break_command->name, break_command->tip);
			}
			++break_command;
		}
	}
	phpdbg_writeln("Note: Conditional breaks are costly, use them sparingly!");
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
	phpdbg_writeln("Note: all breakpoints are lost; be sure debugging is complete before clearing");
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
			} else {
				phpdbg_writeln("\t[ ]\t%s\t\t%s", info_command->name, info_command->tip);
			}
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
	phpdbg_notice("Examples");
	phpdbg_writeln("\t%squiet 1", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sQ 1", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill silence OPLINE output, while");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("\t%squiet 0", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sQ 0", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill enable OPLINE output again");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Note: Quietness is disabled automatically while stepping");
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(back) /* {{{ */
{
	phpdbg_help_header();
	phpdbg_writeln("The backtrace is built with the default debug backtrace functionality");
	phpdbg_writeln(EMPTY);
	phpdbg_notice("Examples");
	phpdbg_writeln("\t%sback 5", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%st 5", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill limit the number of frames to 5, the default is no limit");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Note: it is not necessary for an exception to be thrown to show a backtrace");
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(frame) /* {{{ */
{
	phpdbg_help_header();
	phpdbg_writeln("When viewing a backtrace, it is sometimes useful to jump to a frame in that trace");
	phpdbg_writeln(EMPTY);
	phpdbg_notice("Examples");
	phpdbg_writeln("\t%sframe 2", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sf 2", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill go to frame 2, temporarily affecting scope and allowing access to the variables in that frame");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Note: the current frame is restored when execution continues");
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(list) /* {{{ */
{
	phpdbg_help_header();
	phpdbg_writeln("The list command displays source code for the given argument");
	phpdbg_writeln(EMPTY);
	phpdbg_notice("Examples");
	phpdbg_writeln("\t%slist [lines] 2", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sl [l] 2", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill print next 2 lines from the current file");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("\t%slist [func] my_function", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sl [f] my_function", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill print the source of the global function \"my_function\"");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("\t%slist [func] .mine", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sl [f] .mine", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill print the source of the method \"mine\" from the active scope");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("\t%slist [method] my::method", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sl [m] my::method", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill print the source of \"my::method\"");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("\t%slist c myClass", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sl c myClass", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill print the source of \"myClass\"");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Note: before listing functions you must have a populated function table, try compile!!");
	phpdbg_writeln(EMPTY);
	phpdbg_notice("The parameters enclosed by [] are usually optional, but help avoid ambigious commands");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Specific listers loaded are show below:");
	phpdbg_notice("Commands");
	{
		const phpdbg_command_t *list_command = phpdbg_list_commands;

		phpdbg_writeln("\tAlias\tCommand\t\tPurpose");
		while (list_command && list_command->name) {
			if (list_command->alias) {
				phpdbg_writeln("\t[%c]\t%s\t\t%s", list_command->alias, list_command->name, list_command->tip);
			} else {
				phpdbg_writeln("\t[ ]\t%s\t\t%s", list_command->name, list_command->tip);
			}
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
	phpdbg_notice("Examples");
	phpdbg_writeln("\t%soplog /path/to/my.oplog", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sO /path/to/my.oplog", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill open the file /path/to/my.oplog for writing, creating it if it does not exist");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("\t%soplog 0", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sO 0", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill close the currently open log file, disabling oplog");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Note: upon failure to open a new oplog, the last oplog is held open");
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(set) /* {{{ */
{
	phpdbg_help_header();
	phpdbg_writeln("Configure how phpdbg looks and behaves with the set command");
	phpdbg_writeln("Specific set commands are show below:");
	phpdbg_notice("Commands");
	{
		const phpdbg_command_t *set_command = phpdbg_set_commands;

		phpdbg_writeln("\tAlias\tCommand\t\tPurpose");
		while (set_command && set_command->name) {
			if (set_command->alias) {
				phpdbg_writeln("\t[%c]\t%s\t\t%s", set_command->alias, set_command->name, set_command->tip);
			} else {
				phpdbg_writeln("\t[ ]\t%s\t\t%s", set_command->name, set_command->tip);
			}
			++set_command;
		}
	}
#ifndef _WIN32
	phpdbg_notice("Colors");
	{
		const phpdbg_color_t *color = phpdbg_get_colors(TSRMLS_C);
		
		if (PHPDBG_G(flags) & PHPDBG_IS_COLOURED) {
			phpdbg_writeln("\t%-20s\t\tExample", "Name");
		} else {
			phpdbg_writeln("\tName");
		}
		
		while (color && color->name) {
			if (PHPDBG_G(flags) & PHPDBG_IS_COLOURED) {
				phpdbg_writeln(
					"\t%-20s\t\t\033[%smphpdbg rocks :)\033[0m", color->name, color->code);
			} else {
				phpdbg_writeln("\t%s", color->name);
			}
			++color;
		}
	}
	phpdbg_writeln("The <element> for set color can be \"prompt\", \"notice\", or \"error\"");
#endif
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(register) /* {{{ */
{
	phpdbg_help_header();
	phpdbg_writeln("Register any global function for use as a command in phpdbg console");
	phpdbg_writeln(EMPTY);
	phpdbg_notice("Examples");
	phpdbg_writeln("\t%sregister scandir", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%sR scandir", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill register the scandir function for use in phpdbg");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Note: arguments passed as strings, return (if present) print_r'd on console");
	if (zend_hash_num_elements(&PHPDBG_G(registered))) {
   		HashPosition position;
   		char *name = NULL;
   		zend_uint name_len = 0;

   		phpdbg_notice("Registered Functions (%d)", zend_hash_num_elements(&PHPDBG_G(registered)));
   		for (zend_hash_internal_pointer_reset_ex(&PHPDBG_G(registered), &position);
   			zend_hash_get_current_key_ex(&PHPDBG_G(registered), &name, &name_len, NULL, 1, &position) == HASH_KEY_IS_STRING;
   			zend_hash_move_forward_ex(&PHPDBG_G(registered), &position)) {
   			phpdbg_writeln("|-------> %s", name);
   			efree(name);
   		}
	}

	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(source) /* {{{ */
{
	phpdbg_help_header();
	phpdbg_writeln("Sourcing a phpdbginit during your debugging session might save some time");
	phpdbg_writeln("The source command can also be used to export breakpoints to a phpdbginit file");
	phpdbg_writeln(EMPTY);
	phpdbg_notice("Examples");
	phpdbg_writeln("\t%ssource /my/init", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%s. /my/init", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill execute the phpdbginit file at /my/init");
	phpdbg_writeln("\t%ssource export /my/init", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%s. export /my/init", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill export breakpoints to /my/init in phpdbginit file format");
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(shell) /* {{{ */
{
	phpdbg_help_header();
	phpdbg_writeln("Direct access to shell commands saves having to switch windows/consoles");
	phpdbg_writeln(EMPTY);
	phpdbg_notice("Examples");
	phpdbg_writeln("\t%sshell ls /usr/src/php-src", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\t%s- ls /usr/src/php-src", phpdbg_get_prompt(TSRMLS_C));
	phpdbg_writeln("\tWill execute ls /usr/src/php-src, displaying the output in the console");
	phpdbg_writeln(EMPTY);
	phpdbg_writeln("Note: read only commands please!");
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */

PHPDBG_HELP(options) /* {{{ */
{
	phpdbg_help_header();
	phpdbg_writeln("Below are the command line options supported by phpdbg");
	phpdbg_notice("Command Line Options and Flags");
	phpdbg_writeln(" -c\t-c/my/php.ini\t\tSet php.ini file to load");
	phpdbg_writeln(" -d\t-dmemory_limit=4G\tSet a php.ini directive");
	phpdbg_writeln(" -n\tN/A\t\t\tDisable default php.ini");
	phpdbg_writeln(" -q\tN/A\t\t\tSupress welcome banner");
	phpdbg_writeln(" -e\t-emytest.php\t\tSet execution context");
	phpdbg_writeln(" -v\tN/A\t\t\tEnable oplog output");
	phpdbg_writeln(" -s\tN/A\t\t\tEnable stepping");
	phpdbg_writeln(" -b\tN/A\t\t\tDisable colour");
	phpdbg_writeln(" -i\t-imy.init\t\tSet .phpdbginit file");
	phpdbg_writeln(" -I\tN/A\t\t\tIgnore default .phpdbginit");
	phpdbg_writeln(" -O\t-Omy.oplog\t\tSets oplog output file");
	phpdbg_writeln(" -r\tN/A\t\t\tRun execution context");
	phpdbg_writeln(" -E\tN/A\t\t\tEnable step through eval, careful!");
	phpdbg_writeln(" -S\t-Scli\t\t\tOverride SAPI name, careful!");
#ifndef _WIN32
	phpdbg_writeln(" -l\t-l4000\t\t\tSetup remote console ports");
	phpdbg_writeln(" -a\t-a192.168.0.3\t\tSetup remote console bind address");
#endif
	phpdbg_writeln(" -V\tN/A\t\t\tVersion number");
	phpdbg_notice("Passing -rr will quit automatically after execution");
#ifndef _WIN32
	phpdbg_writeln("Remote Console Mode");
	phpdbg_notice("For security, phpdbg will bind only to the loopback interface by default");
	phpdbg_writeln("-a without an argument implies all; phpdbg will bind to all available interfaces.");
	phpdbg_writeln("specify both stdin and stdout with -lstdin/stdout; by default stdout is stdin * 2.");
	phpdbg_notice("Steps should be taken to secure this service if bound to a public interface/port");
#endif
	phpdbg_help_footer();
	return SUCCESS;
} /* }}} */
