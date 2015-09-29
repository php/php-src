/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
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
   | Authors: Terry Ellison <terry@ellisons.org.uk>                       |
   +----------------------------------------------------------------------+
*/

#include "phpdbg.h"
#include "phpdbg_help.h"
#include "phpdbg_prompt.h"
#include "phpdbg_eol.h"
#include "zend.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

/* {{{ Commands Table */
#define PHPDBG_COMMAND_HELP_D(name, tip, alias, action) \
	{PHPDBG_STRL(#name), tip, sizeof(tip)-1, alias, action, &phpdbg_prompt_commands[16], 0}

const phpdbg_command_t phpdbg_help_commands[] = {
	PHPDBG_COMMAND_HELP_D(aliases,    "show alias list", 'a', phpdbg_do_help_aliases),
	PHPDBG_COMMAND_HELP_D(options,    "command line options", 0, NULL),
	PHPDBG_COMMAND_HELP_D(overview,   "help overview", 0, NULL),
	PHPDBG_COMMAND_HELP_D(phpdbginit, "phpdbginit file format", 0, NULL),
	PHPDBG_COMMAND_HELP_D(syntax,     "syntax overview", 0, NULL),
	PHPDBG_END_COMMAND
};  /* }}} */

/* {{{ pretty_print.  Formatting escapes and wrapping text in a string before printing it. */
void pretty_print(char *text)
{
	char *new, *p, *q;

	const char  *prompt_escape = phpdbg_get_prompt();
	unsigned int prompt_escape_len = strlen(prompt_escape);
	unsigned int prompt_len = strlen(PHPDBG_G(prompt)[0]);

	const char  *bold_on_escape  = PHPDBG_G(flags) & PHPDBG_IS_COLOURED ? "\033[1m" : "";
	const char  *bold_off_escape = PHPDBG_G(flags) & PHPDBG_IS_COLOURED ? "\033[0m" : "";
	unsigned int bold_escape_len = strlen(bold_on_escape);

	unsigned int term_width = phpdbg_get_terminal_width();
	unsigned int size = 0;

	int in_bold = 0;

	char *last_new_blank = NULL;          /* position in new buffer of last blank char */
	unsigned int last_blank_count = 0;    /* printable char offset of last blank char */
	unsigned int line_count = 0;          /* number printable chars on current line */

	if (PHPDBG_G(flags) & PHPDBG_WRITE_XML) {
		phpdbg_xml("<help %r msg=\"%s\" />", text);
		return;
	}

	/* First pass calculates a safe size for the pretty print version */
	for (p = text; *p; p++) {
		if (UNEXPECTED(p[0] == '*') && p[1] == '*') {
			size += bold_escape_len - 2;
			p++;
		} else if (UNEXPECTED(p[0] == '$') && p[1] == 'P') {
			size += prompt_escape_len - 2;
			p++;
		} else if (UNEXPECTED(p[0] == '\\')) {
			p++;
		}
	}
	size += (p-text)+1;

	new = emalloc(size);
	/*
	 * Second pass substitutes the bold and prompt escape sequences and line wrap
	 *
	 * ** toggles bold on and off if PHPDBG_IS_COLOURED flag is set
	 * $P substitutes the prompt sequence
	 * Lines are wrapped by replacing the last blank with a CR before <term width>
	 * characters.  (This defaults to 100 if the width can't be detected).  In the
	 * pathelogical case where no blanks are found, then the wrap occurs at the
	 * first blank.
	 */
	for (p = text, q = new; *p; p++) {
		if (UNEXPECTED(*p == ' ')) {
			last_new_blank = q;
			last_blank_count = line_count++;
			*q++ = ' ';
		} else if (UNEXPECTED(*p == '\n')) {
			last_new_blank = NULL;
			*q++ = *p;
			last_blank_count = 0;
			line_count = 0;
		} else if (UNEXPECTED(p[0] == '*') && p[1] == '*') {
			if (bold_escape_len) {
				in_bold = !in_bold;
				memcpy (q, in_bold ? bold_on_escape : bold_off_escape, bold_escape_len);
				q += bold_escape_len;
				/* bold on/off has zero print width so line count is unchanged */
			}
			p++;
		} else if (UNEXPECTED(p[0] == '$') && p[1] == 'P') {
			memcpy (q, prompt_escape, prompt_escape_len);
			q += prompt_escape_len;
			line_count += prompt_len;
			p++;
		} else if (UNEXPECTED(p[0] == '\\')) {
			p++;
			*q++ = *p;
			line_count++;
		} else {
			*q++ = *p;
			line_count++;
		}

		if (UNEXPECTED(line_count>=term_width) && last_new_blank) {
			*last_new_blank = '\n';
			last_new_blank = NULL;
			line_count -= last_blank_count;
			last_blank_count = 0;
		}
	}
	*q++ = '\0';

	if ((q-new)>size) {
		phpdbg_error("help", "overrun=\"%lu\"", "Output overrun of %lu bytes", ((q - new) - size));
	}

	phpdbg_out("%s\n", new);
	efree(new);
}  /* }}} */

/* {{{ summary_print.  Print a summary line giving, the command, its alias and tip */
void summary_print(phpdbg_command_t const * const cmd)
{
	char *summary;
	spprintf(&summary, 0, "Command: **%s**  Alias: **%c**  **%s**\n", cmd->name, cmd->alias, cmd->tip);
	pretty_print(summary);
	efree(summary);
}

/* {{{ get_help. Retries and formats text from the phpdbg help text table */
static char *get_help(const char * const key)
{
	phpdbg_help_text_t *p;

	/* Note that phpdbg_help_text is not assumed to be collated in key order.  This is an
	   inconvience that means that help can't be logically grouped Not worth
	   the savings */

	for (p = phpdbg_help_text; p->key; p++) {
		if (!strcmp(p->key, key)) {
			return p->text;
		}
	}
	return "";   /* return empty string to denote no match found */
} /* }}} */

/* {{{ get_command.  Return number of matching commands from a command table.
 * Unlike the command parser, the help search is sloppy that is partial matches can occur
 *   * Any single character key is taken as an alias.
 *   * Other keys are matched again the table on the first len characters.
 *   * This means that non-unique keys can generate multiple matches.
 *   * The first matching command is returned as an OUT parameter. *
 * The rationale here is to assist users in finding help on commands. So unique matches
 * will be used to generate a help message but non-unique one will be used to list alternatives.
 */
static int get_command(
	const char *key, size_t len,      /* pointer and length of key */
	phpdbg_command_t const **command, /* address of first matching command  */
	phpdbg_command_t const * commands /* command table to be scanned */
	)
{
	const phpdbg_command_t *c;
	unsigned int num_matches = 0;

	if (len == 1) {
		for (c=commands; c->name; c++) {
			if (c->alias == key[0]) {
				num_matches++;
				if ( num_matches == 1 && command) {
					*command = c;
				}
			}
		}
	} else {
		for (c=commands; c->name; c++) {
			if (!strncmp(c->name, key, len)) {
				++num_matches;
				if ( num_matches == 1 && command) {
					*command = c;
				}
			}
		}
	}

	return num_matches;

} /* }}} */

PHPDBG_COMMAND(help) /* {{{ */
{
	phpdbg_command_t const *cmd;
	int n;

	if (!param || param->type == EMPTY_PARAM) {
		pretty_print(get_help("overview!"));
		return SUCCESS;
	}

	if (param && param->type == STR_PARAM) {
	    n = get_command(param->str, param->len, &cmd, phpdbg_prompt_commands);

		if (n==1) {
			summary_print(cmd);
			pretty_print(get_help(cmd->name));
			return SUCCESS;

		} else if (n>1) {
			if (param->len > 1) {
				for (cmd=phpdbg_prompt_commands; cmd->name; cmd++) {
					if (!strncmp(cmd->name, param->str, param->len)) {
						summary_print(cmd);
					}
				}
				pretty_print(get_help("duplicate!"));
				return SUCCESS;
			} else {
				phpdbg_error("help", "type=\"ambiguousalias\" alias=\"%s\"", "Internal help error, non-unique alias \"%c\"", param->str[0]);
				return FAILURE;
			}

		} else { /* no prompt command found so try help topic */
		    n = get_command( param->str, param->len, &cmd, phpdbg_help_commands);

			if (n>0) {
				if (cmd->alias == 'a') {   /* help aliases executes a canned routine */
					return cmd->handler(param);
				} else {
					pretty_print(get_help(cmd->name));
					return SUCCESS;
				}
			}
		}
	}

	return FAILURE;

} /* }}} */

PHPDBG_HELP(aliases) /* {{{ */
{
	const phpdbg_command_t *c, *c_sub;
	int len;

	/* Print out aliases for all commands except help as this one comes last */
	phpdbg_writeln("help", "", "Below are the aliased, short versions of all supported commands");
	phpdbg_xml("<helpcommands %r>");
	for(c = phpdbg_prompt_commands; c->name; c++) {
		if (c->alias && c->alias != 'h') {
			phpdbg_writeln("command", "alias=\"%c\" name=\"%s\" tip=\"%s\"", " %c     %-20s  %s", c->alias, c->name, c->tip);
			if (c->subs) {
				len = 20 - 1 - c->name_len;
				for(c_sub = c->subs; c_sub->alias; c_sub++) {
					if (c_sub->alias) {
						phpdbg_writeln("subcommand", "parent_alias=\"%c\" alias=\"%c\" parent=\"%s\" name=\"%-*s\" tip=\"%s\"", " %c %c   %s %-*s  %s",
							c->alias, c_sub->alias, c->name, len, c_sub->name, c_sub->tip);
					}
				}
			}
		}
	}

	phpdbg_xml("</helpcommands>");

	/* Print out aliases for help as this one comes last, with the added text on how aliases are used */
	get_command("h", 1, &c, phpdbg_prompt_commands);
	phpdbg_writeln("aliasinfo", "alias=\"%c\" name=\"%s\" tip=\"%s\"", " %c     %-20s  %s\n", c->alias, c->name, c->tip);

	phpdbg_xml("<helpaliases>");

	len = 20 - 1 - c->name_len;
	for(c_sub = c->subs; c_sub->alias; c_sub++) {
		if (c_sub->alias) {
			phpdbg_writeln("alias", "parent_alias=\"%c\" alias=\"%c\" parent=\"%s\" name=\"%-*s\" tip=\"%s\"", " %c %c   %s %-*s  %s",
				c->alias, c_sub->alias, c->name, len, c_sub->name, c_sub->tip);
		}
	}

	phpdbg_xml("</helpaliases>");

	pretty_print(get_help("aliases!"));
	return SUCCESS;
} /* }}} */


/* {{{ Help Text Table
 * Contains help text entries keyed by a lowercase ascii key.
 * Text is in ascii and enriched by a simple markup:
 *   ** toggles bold font emphasis.
 *   $P insert an bold phpdbg> prompt.
 *   \  escapes the following character. Note that this is itself escaped inside string
 *      constants so \\\\ is required to output a single \ e.g. as in namespace names.
 *
 * Text will be wrapped according to the STDOUT terminal width, so paragraphs are
 * flowed using the C stringizing and the CR definition.  Also note that entries
 * are collated in alphabetic order on key.
 *
 * Also note the convention that help text not directly referenceable as a help param
 * has a key ending in !
 */
#define CR "\n"
phpdbg_help_text_t phpdbg_help_text[] = {

/******************************** General Help Topics ********************************/
{"overview!", CR
"**phpdbg** is a lightweight, powerful and easy to use debugging platform for PHP5.4+" CR
"It supports the following commands:" CR CR

"**Information**" CR
"  **list**     list PHP source" CR
"  **info**     displays information on the debug session" CR
"  **print**    show opcodes" CR
"  **frame**    select a stack frame and print a stack frame summary" CR
"  **back**     shows the current backtrace" CR
"  **help**     provide help on a topic" CR CR

"**Starting and Stopping Execution**" CR
"  **exec**     set execution context" CR
"  **run**      attempt execution" CR
"  **step**     continue execution until other line is reached" CR
"  **continue** continue execution" CR
"  **until**    continue execution up to the given location" CR
"  **next**     continue execution up to the given location and halt on the first line after it" CR
"  **finish**   continue up to end of the current execution frame" CR
"  **leave**    continue up to end of the current execution frame and halt after the calling instruction" CR
"  **break**    set a breakpoint at the specified target" CR
"  **watch**    set a watchpoint on $variable" CR
"  **clear**    clear one or all breakpoints" CR
"  **clean**    clean the execution environment" CR CR

"**Miscellaneous**" CR
"  **set**      set the phpdbg configuration" CR
"  **source**   execute a phpdbginit script" CR
"  **register** register a phpdbginit function as a command alias" CR
"  **sh**       shell a command" CR
"  **ev**       evaluate some code" CR
"  **quit**     exit phpdbg" CR CR

"Type **help <command>** or (**help alias**) to get detailed help on any of the above commands, "
"for example **help list** or **h l**.  Note that help will also match partial commands if unique "
"(and list out options if not unique), so **help clea** will give help on the **clean** command, "
"but **help cl** will list the summary for **clean** and **clear**." CR CR

"Type **help aliases** to show a full alias list, including any registered phpdginit functions" CR
"Type **help syntax** for a general introduction to the command syntax." CR
"Type **help options** for a list of phpdbg command line options." CR
"Type **help phpdbginit** to show how to customise the debugger environment."
},
{"options", CR
"Below are the command line options supported by phpdbg" CR CR
                          /* note the extra 4 space index in because of the extra **** */
"**Command Line Options and Flags**" CR
"  **Option**  **Example Argument**    **Description**" CR
"  **-c**      **-c**/my/php.ini       Set php.ini file to load" CR
"  **-d**      **-d**memory_limit=4G   Set a php.ini directive" CR
"  **-n**                          Disable default php.ini" CR
"  **-q**                          Suppress welcome banner" CR
"  **-v**                          Enable oplog output" CR
"  **-b**                          Disable colour" CR
"  **-i**      **-i**my.init           Set .phpdbginit file" CR
"  **-I**                          Ignore default .phpdbginit" CR
"  **-O**      **-O**my.oplog          Sets oplog output file" CR
"  **-r**                          Run execution context" CR
"  **-rr**                         Run execution context and quit after execution (not respecting breakpoints)" CR
"  **-e**                          Generate extended information for debugger/profiler" CR
"  **-E**                          Enable step through eval, careful!" CR
"  **-S**      **-S**cli               Override SAPI name, careful!" CR
"  **-l**      **-l**4000              Setup remote console ports" CR
"  **-a**      **-a**192.168.0.3       Setup remote console bind address" CR
"  **-x**                          Enable xml output (instead of normal text output)" CR
"  **-p**      **-p**, **-p=func**, **-p* **   Output opcodes and quit" CR
"  **-h**                          Print the help overview" CR
"  **-V**                          Print version number" CR
"  **--**      **--** arg1 arg2        Use to delimit phpdbg arguments and php $argv; append any $argv "
"argument after it" CR CR

"**Remote Console Mode**" CR CR

"This mode is enabled by specifying the **-a** option. Phpdbg will bind only to the loopback "
"interface by default, and this can only be overridden by explicitly setting the remote console "
"bind address using the **-a** option. If **-a** is specied without an argument, then phpdbg "
"will bind to all available interfaces.  You should be aware of the security implications of "
"doing this, so measures should be taken to secure this service if bound to a publicly accessible "
"interface/port." CR CR

"**Opcode output**" CR CR

"Outputting opcodes requires that a file path is passed as last argument. Modes of execution:" CR
"**-p** Outputs the main execution context" CR
"**-p* **Outputs all opcodes in the whole file (including classes and functions)" CR
"**-p=function_name** Outputs opcodes of a given function in the file" CR
"**-p=class_name::** Outputs opcodes of all the methods of a given class" CR
"**-p=class_name::method** Outputs opcodes of a given method"
},

{"phpdbginit", CR
"Phpdgb uses an debugger script file to initialize the debugger context.  By default, phpdbg looks "
"for the file named **.phpdbginit** in the current working directory.  This location can be "
"overridden on the command line using the **-i** switch (see **help options** for a more "
"details)." CR CR

"Debugger scripts can also be executed using the **source** command." CR CR

"A script file can contain a sequence of valid debugger commands, comments and embedded PHP "
"code. " CR CR

"Comment lines are prefixed by the **#** character.  Note that comments are only allowed in script "
"files and not in interactive sessions." CR CR

"PHP code is delimited by the start and end escape tags **<:** and **:>**. PHP code can be used "
"to define application context for a debugging session and also to extend the debugger by defining "
"and **register** PHP functions as new commands." CR CR

"Also note that executing a **clear** command will cause the current **phpdbginit** to be reparsed "
"/ reloaded."
},

{"syntax", CR
"Commands start with a keyword, and some (**break**, "
"**info**, **set**, **print** and **list**) may include a subcommand keyword.  All keywords are "
"lower case but also have a single letter alias that may be used as an alternative to typing in the"
"keyword in full.  Note some aliases are uppercase, and that keywords cannot be abbreviated other "
"than by substitution by the alias." CR CR

"Some commands take an argument.  Arguments are typed according to their format:" CR
"     *  **omitted**" CR
"     *  **address**      **0x** followed by a hex string" CR
"     *  **number**       an optionally signed number" CR
"     *  **method**       a valid **Class::methodName** expression" CR
"     *  **func#op**      a valid **Function name** follow by # and an integer" CR
"     *  **method#op**    a valid **Class::methodName** follow by # and an integer" CR
"     *  **string**       a general string" CR
"     *  **function**     a valid **Function name**" CR
"     *  **file:line**    a valid **filename** follow by : and an integer" CR CR

"In some cases the type of the argument enables the second keyword to be omitted." CR CR

"Type **help** for an overview of all commands and type **help <command>** to get detailed help "
"on any specific command." CR CR

"**Valid Examples**" CR CR

"     $P quit" CR
"     $P q" CR
"     Quit the debugger" CR CR

"     $P ev $total[2]" CR
"     Evaluate and print the variable $total[2] in the current stack frame" CR
"    " CR
"     $P break 200" CR
"     $P b my_source.php:200" CR
"     Break at line 200 in the current source and in file **my_source.php**. " CR CR

"     $P b @ ClassX::get_args if $arg[0] == \"fred\"" CR
"     $P b ~ 3" CR
"     Break at ClassX::get_args() if $arg[0] == \"fred\" and delete breakpoint 3" CR CR

"**Examples of invalid commands**" CR

"     $P #This is a comment" CR
"     Comments introduced by the **#** character are only allowed in **phpdbginit** script files."
},

/******************************** Help Codicils ********************************/
{"aliases!", CR
"Note that aliases can be used for either command or sub-command keywords or both, so **info b** "
"is a synomyn for **info break** and **l func** for **list func**, etc." CR CR

"Note that help will also accept any alias as a parameter and provide help on that command, for example **h p** will provide help on the print command."
},

{"duplicate!", CR
"Parameter is not unique. For detailed help select help on one of the above commands."
},

/******************************** Help on Commands ********************************/
{"back",
"Provide a formatted backtrace using the standard debug_backtrace() functionality.  An optional "
"unsigned integer argument specifying the maximum number of frames to be traced; if omitted then "
"a complete backtrace is given." CR CR

"**Examples**" CR CR
"    $P back 5" CR
"    $P t " CR
" " CR
"A backtrace can be executed at any time during execution."
},

{"break",
"Breakpoints can be set at a range of targets within the execution environment.  Execution will "
"be paused if the program flow hits a breakpoint.  The break target can be one of the following "
"types:" CR CR

"  **Target**   **Alias** **Purpose**" CR
"  **at**       **A**     specify breakpoint by location and condition" CR
"  **del**      **d**     delete breakpoint by breakpoint identifier number" CR CR

"**Break at** takes two arguments. The first is any valid target. The second "
"is a valid PHP expression which will trigger the break in "
"execution, if evaluated as true in a boolean context at the specified target." CR CR

"Note that breakpoints can also be disabled and re-enabled by the **set break** command." CR CR

"**Examples**" CR CR
"    $P break test.php:100" CR
"    $P b test.php:100" CR
"    Break execution at line 100 of test.php" CR CR

"    $P break 200" CR
"    $P b 200" CR
"    Break execution at line 200 of the currently PHP script file" CR CR

"    $P break \\\\mynamespace\\\\my_function" CR
"    $P b \\\\mynamespace\\\\my_function" CR
"    Break execution on entry to \\\\mynamespace\\\\my_function" CR CR

"    $P break classX::method" CR
"    $P b classX::method" CR
"    Break execution on entry to classX::method" CR CR

"    $P break 0x7ff68f570e08" CR
"    $P b 0x7ff68f570e08" CR
"    Break at the opline at the address 0x7ff68f570e08" CR CR

"    $P break my_function#14" CR
"    $P b my_function#14" CR
"    Break at the opline #14 of the function my_function" CR CR

"    $P break \\\\my\\\\class::method#2" CR
"    $P b \\\\my\\\\class::method#2" CR
"    Break at the opline #2 of the method \\\\my\\\\class::method" CR CR

"    $P break test.php:#3" CR
"    $P b test.php:#3" CR
"    Break at opline #3 in test.php" CR CR

"    $P break if $cnt > 10" CR
"    $P b if $cnt > 10" CR
"    Break when the condition ($cnt > 10) evaluates to true" CR CR

"    $P break at phpdbg::isGreat if $opt == 'S'" CR
"    $P break @ phpdbg::isGreat if $opt == 'S'" CR
"    Break at any opcode in phpdbg::isGreat when the condition ($opt == 'S') is true" CR CR

"    $P break at test.php:20 if !isset($x)" CR
"    Break at every opcode on line 20 of test.php when the condition evaluates to true" CR CR

"    $P break ZEND_ADD" CR
"    $P b ZEND_ADD" CR
"    Break on any occurrence of the opcode ZEND_ADD" CR CR

"    $P break del 2" CR
"    $P b ~ 2" CR
"    Remove breakpoint 2" CR CR

"Note: Conditional breaks are costly in terms of runtime overhead. Use them only when required "
"as they significantly slow execution." CR CR

"Note: An address is only valid for the current compilation."
},

{"clean",
"Classes, constants or functions can only be declared once in PHP.  You may experience errors "
"during a debug session if you attempt to recompile a PHP source.  The clean command clears "
"the Zend runtime tables which holds the sets of compiled classes, constants and functions, "
"releasing any associated storage back into the storage pool.  This enables recompilation to "
"take place." CR CR

"Note that you cannot selectively trim any of these resource pools. You can only do a complete "
"clean."
},

{"clear",
"Clearing breakpoints means you can once again run code without interruption." CR CR

"Note: use break delete N to clear a specific breakpoint." CR CR

"Note: if all breakpoints are cleared, then the PHP script will run until normal completion."
},

{"ev",
"The **ev** command takes a string expression which it evaluates and then displays. It "
"evaluates in the context of the lowest (that is the executing) frame, unless this has first "
"been explicitly changed by issuing a **frame** command. " CR CR

"**Examples**" CR CR
"    $P ev $variable" CR
"    Will print_r($variable) on the console, if it is defined" CR CR

"    $P ev $variable = \"Hello phpdbg :)\"" CR
"    Will set $variable in the current scope" CR CR

"Note that **ev** allows any valid PHP expression including assignments, function calls and "
"other write statements.  This enables you to change the environment during execution, so care "
"is needed here.  You can even call PHP functions which have breakpoints defined. " CR CR

"Note: **ev** will always show the result, so do not prefix the code with **return**"
},

{"exec",
"The **exec** command sets the execution context, that is the script to be executed.  The "
"execution context must be defined either by executing the **exec** command or by using the "
"**-e** command line option." CR CR

"Note that the **exec** command also can be used to replace a previously defined execution "
"context." CR CR

"**Examples**" CR CR

"    $P exec /tmp/script.php" CR
"    $P e /tmp/script.php" CR
"    Set the execution context to **/tmp/script.php**"
},

//*********** Does F skip any breakpoints lower stack frames or only the current??
{"finish",
"The **finish** command causes control to be passed back to the vm, continuing execution.  Any "
"breakpoints that are encountered within the current stack frame will be skipped.  Execution "
"will then continue until the next breakpoint after leaving the stack frame or until "
"completion of the script" CR CR

"Note when **step**ping is enabled, any opcode steps within the current stack frame are also "
"skipped. "CR CR

"Note **finish** will trigger a \"not executing\" error if not executing."
},

{"frame",
"The **frame** takes an optional integer argument. If omitted, then the current frame is displayed "
"If specified then the current scope is set to the corresponding frame listed in a **back** trace. "
"This can be used to allowing access to the variables in a higher stack frame than that currently being executed." CR CR

"**Examples**" CR CR
"    $P frame 2" CR
"    $P ev $count" CR
"    Go to frame 2 and print out variable **$count** in that frame" CR CR

"Note that this frame scope is discarded when execution continues, with the execution frame "
"then reset to the lowest executiong frame."
},

{"info",
"**info** commands provide quick access to various types of information about the PHP environment" CR
"By default general information about environment and PHP build is shown." CR
"Specific info commands are show below:" CR CR

"  **Target**   **Alias**  **Purpose**" CR
"  **break**      **b**      show current breakpoints" CR
"  **files**      **F**      show included files" CR
"  **classes**    **c**      show loaded classes" CR
"  **funcs**      **f**      show loaded functions" CR
"  **error**      **e**      show last error" CR
"  **constants**  **d**      show user-defined constants" CR
"  **vars**       **v**      show active variables" CR
"  **globals**    **g**      show superglobal variables" CR
"  **literal**    **l**      show active literal constants" CR
"  **memory**     **m**      show memory manager stats"
},

// ******** same issue about breakpoints in called frames
{"leave",
"The **leave** command causes control to be passed back to the vm, continuing execution.  Any "
"breakpoints that are encountered within the current stack frame will be skipped.  In effect a "
"temporary breakpoint is associated with any return opcode, so that a break in execution occurs "
"before leaving the current stack frame. This allows inspection / modification of any frame "
"variables including the return value before it is returned" CR CR

"**Examples**" CR CR

"    $P leave" CR
"    $P L" CR CR

"Note when **step**ping is enabled, any opcode steps within the current stack frame are also "
"skipped. "CR CR

"Note **leave** will trigger a \"not executing\" error if not executing."
},

{"list",
"The list command displays source code for the given argument.  The target type is specficied by "
"a second subcommand keyword:" CR CR

"  **Type**     **Alias**  **Purpose**" CR
"  **lines**    **l**      List N lines from the current execution point" CR
"  **func**     **f**      List the complete source for a specified function" CR
"  **method**   **m**      List the complete source for a specified class::method" CR
"  **class**    **c**      List the complete source for a specified class" CR CR

"Note that the context of **lines**, **func** and **method** can be determined by parsing the "
"argument, so these subcommands are optional.  However, you must specify the **class** keyword "
"to list off a class." CR CR

"**Examples**" CR CR
"    $P list 2" CR
"    $P l l 2" CR
"    List the next 2 lines from the current file" CR CR

"    $P list my_function" CR
"    $P l f my_function" CR
"    List the source of the function **my_function**" CR CR

//************ ????
"    $P list func .mine" CR
"    $P l f .mine" CR
"    List the source of the method **mine** from the active class in scope" CR CR

"    $P list m my::method" CR
"    $P l my::method" CR
"    List the source of **my::method**" CR CR

"    $P list c myClass" CR
"    $P l c myClass" CR
"    List the source of **myClass**" CR CR

"Note that functions and classes can only be listed if the corresponding classes and functions "
"table in the Zend executor has a corresponding entry.  You can use the compile command to "
"populate these tables for a given execution context."
},

{"continue",
"Continue with execution after hitting a break or watchpoint" CR CR

"**Examples**" CR CR
"    $P continue" CR
"    $P c" CR
"    Continue executing until the next break or watchpoint" CR CR

"Note **continue** will trigger a \"not running\" error if not executing."
},

{"print",
"By default, print will show the opcodes of the current execution context." CR
"Other printing commands give access to instruction information." CR
"Specific printers loaded are show below:" CR CR

"  **Type**    **Alias**  **Purpose**" CR
"  **exec**    **e**      print out the instructions in the execution context" CR
"  **opline**  **o**      print out the instruction in the current opline" CR
"  **class**   **c**      print out the instructions in the specified class" CR
"  **method**  **m**      print out the instructions in the specified method" CR
"  **func**    **f**      print out the instructions in the specified function" CR
"  **stack**   **s**      print out the instructions in the current stack" CR CR

"In case passed argument does not match a specific printing command, it will treat it as function or method name and print its opcodes" CR CR

"**Examples**" CR CR
"    $P print class \\\\my\\\\class" CR
"    $P p c \\\\my\\\\class" CR
"    Print the instructions for the methods in \\\\my\\\\class" CR CR

"    $P print method \\\\my\\\\class::method" CR
"    $P p m \\\\my\\\\class::method" CR
"    Print the instructions for \\\\my\\\\class::method" CR CR

"    $P print func .getSomething" CR
"    $P p f .getSomething" CR
//************* Check this local method scope
"    Print the instructions for ::getSomething in the active scope" CR CR

"    $P print func my_function" CR
"    $P p f my_function" CR
"    Print the instructions for the global function my_function" CR CR

"    $P print opline" CR
"    $P p o" CR
"    Print the instruction for the current opline" CR CR

"    $P print exec" CR
"    $P p e" CR
"    Print the instructions for the execution context" CR CR

"    $P print stack" CR
"    $P p s" CR
"    Print the instructions for the current stack"
},

{"register",
//******* Needs a general explanation of the how registered functions work
"Register any global function for use as a command in phpdbg console" CR CR

"**Examples**" CR CR
"    $P register scandir" CR
"    $P R scandir" CR
"    Will register the scandir function for use in phpdbg" CR CR

"Note: arguments passed as strings, return (if present) print_r'd on console"
},

{"run",
"Enter the vm, startinging execution. Execution will then continue until the next breakpoint "
"or completion of the script. Add parameters you want to use as $argv"
"**Examples**" CR CR
"    $P run" CR
"    $P r" CR
"    Will cause execution of the context, if it is set" CR CR
"    $P r test" CR
"    Will execute with $argv[1] == \"test\"" CR CR

"Note that the execution context must be set. If not previously compiled, then the script will "
"be compiled before execution." CR CR

"Note that attempting to run a script that is already executing will result in an \"execution "
"in progress\" error."
},

{"set",
"The **set** command is used to configure how phpdbg looks and behaves.  Specific set commands "
"are as follows:" CR CR

"   **Type**    **Alias**    **Purpose**" CR
"   **prompt**     **p**     set the prompt" CR
"   **color**      **c**     set color  <element> <color>" CR
"   **colors**     **C**     set colors [<on|off>]" CR
"   **oplog**      **O**     set oplog [output]" CR
"   **break**      **b**     set break **id** <on|off>" CR
"   **breaks**     **B**     set breaks [<on|off>]" CR
"   **quiet**      **q**     set quiet [<on|off>]" CR
"   **stepping**   **s**     set stepping [<opcode|line>]" CR
"   **refcount**   **r**     set refcount [<on|off>] " CR CR

"Valid colors are **none**, **white**, **red**, **green**, **yellow**, **blue**, **purple**, "
"**cyan** and **black**.  All colours except **none** can be followed by an optional "
"**-bold** or **-underline** qualifier." CR CR

"Color elements can be one of **prompt**, **notice**, or **error**." CR CR

"**Examples**" CR CR
"     $P S C on" CR
"     Set colors on" CR CR

"     $P set p >" CR
"     $P set color prompt white-bold" CR
"     Set the prompt to a bold >" CR CR

"     $P S c error red-bold" CR
"     Use red bold for errors" CR CR

"     $P S refcount on" CR
"     Enable refcount display when hitting watchpoints" CR CR

"     $P S b 4 off" CR
"     Temporarily disable breakpoint 4.  This can be subsequently reenabled by a **s b 4 on**." CR
//*********** check oplog syntax
},

{"sh",
"Direct access to shell commands saves having to switch windows/consoles" CR CR

"**Examples**" CR CR
"    $P sh ls /usr/src/php-src" CR
"    Will execute ls /usr/src/php-src, displaying the output in the console"
//*********** what does this mean????Note: read only commands please!
},

{"source",
"Sourcing a **phpdbginit** script during your debugging session might save some time." CR CR

"**Examples**" CR CR

"    $P source /my/init" CR
"    $P < /my/init" CR
"    Will execute the phpdbginit file at /my/init" CR CR
},

{"export",
"Exporting breakpoints allows you to share, and or save your current debugging session" CR CR

"**Examples**" CR CR

"    $P export /my/exports" CR
"    $P > /my/exports" CR
"    Will export all breakpoints to /my/exports" CR CR
},

{"step",
"Execute opcodes until next line" CR CR

"**Examples**" CR CR

"    $P s" CR
"    Will continue and break again in the next encountered line" CR CR
},
{"next",
"The **next** command causes control to be passed back to the vm, continuing execution. Any "
"breakpoints that are encountered before the next source line will be skipped. Execution will"
"be stopped when that line is left." CR CR

"Note when **step**ping is enabled, any opcode steps within the current line are also skipped. "CR CR

"Note that if the next line is **not** executed then **all** subsequent breakpoints will be "
"skipped. " CR CR

"Note **next** will trigger a \"not executing\" error if not executing."

},
{"until",
"The **until** command causes control to be passed back to the vm, continuing execution. Any "
"breakpoints that are encountered before the next source line will be skipped. Execution "
"will then continue until the next breakpoint or completion of the script" CR CR

"Note when **step**ping is enabled, any opcode steps within the current line are also skipped. "CR CR

"Note that if the next line is **not** executed then **all** subsequent breakpoints will be "
"skipped. " CR CR

"Note **until** will trigger a \"not executing\" error if not executing."

},
{"watch",
"Sets watchpoints on variables as long as they are defined" CR
"Passing no parameter to **watch**, lists all actually active watchpoints" CR CR

"**Format for $variable**" CR CR
"   **$var**      Variable $var" CR
"   **$var[]**    All array elements of $var" CR
"   **$var->**    All properties of $var" CR
"   **$var->a**   Property $var->a" CR
"   **$var[b]**   Array element with key b in array $var" CR CR

"Subcommands of **watch**:" CR CR

"   **Type**     **Alias**      **Purpose**" CR
"   **array**       **a**       Sets watchpoint on array/object to observe if an entry is added or removed" CR
"   **recursive**   **r**       Watches variable recursively and automatically adds watchpoints if some entry is added to an array/object" CR
"   **delete**      **d**       Removes watchpoint" CR CR

"Note when **recursive** watchpoints are removed, watchpoints on all the children are removed too" CR CR

"**Examples**" CR CR
"     $P watch" CR
"     List currently active watchpoints" CR CR

"     $P watch $array" CR
"     $P w $array" CR
"     Set watchpoint on $array" CR CR

"     $P watch recursive $obj->" CR
"     $P w r $obj->" CR
"     Set recursive watchpoint on $obj->" CR CR

"     $P watch delete $obj->a" CR
"     $P w d $obj->a" CR
"     Remove watchpoint $obj->a" CR CR

"Technical note: If using this feature with a debugger, you will get many segmentation faults, each time when a memory page containing a watched address is hit." CR
"                You then you can continue, phpdbg will remove the write protection, so that the program can continue." CR
"                If phpdbg could not handle that segfault, the same segfault is triggered again and this time phpdbg will abort."
},
{NULL, NULL /* end of table marker */}
};  /* }}} */
