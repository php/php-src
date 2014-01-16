/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
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

#ifndef ZEND_SIGNALS
# include <signal.h>
#endif
#include "phpdbg.h"
#include "phpdbg_prompt.h"
#include "phpdbg_bp.h"
#include "phpdbg_break.h"
#include "phpdbg_list.h"
#include "phpdbg_utils.h"
#include "phpdbg_set.h"

/* {{{ remote console headers */
#ifndef _WIN32
#	include <sys/socket.h>
#	include <sys/select.h>
#	include <sys/time.h>
#	include <sys/types.h>
#	include <unistd.h>
#	include <arpa/inet.h>
#endif /* }}} */

ZEND_DECLARE_MODULE_GLOBALS(phpdbg);

static zend_bool phpdbg_booted = 0;

#if PHP_VERSION_ID >= 50500
void (*zend_execute_old)(zend_execute_data *execute_data TSRMLS_DC);
#else
void (*zend_execute_old)(zend_op_array *op_array TSRMLS_DC);
#endif

static inline void php_phpdbg_globals_ctor(zend_phpdbg_globals *pg) /* {{{ */
{
	pg->prompt[0] = NULL;
	pg->prompt[1] = NULL;

	pg->colors[0] = NULL;
	pg->colors[1] = NULL;
	pg->colors[2] = NULL;

	pg->exec = NULL;
	pg->exec_len = 0;
	pg->ops = NULL;
	pg->vmret = 0;
	pg->bp_count = 0;
	pg->lcmd = NULL;
	pg->flags = PHPDBG_DEFAULT_FLAGS;
	pg->oplog = NULL;
	pg->io[PHPDBG_STDIN] = NULL;
	pg->io[PHPDBG_STDOUT] = NULL;
	pg->io[PHPDBG_STDERR] = NULL;
	memset(&pg->lparam, 0, sizeof(phpdbg_param_t));
	pg->frame.num = 0;
} /* }}} */

static PHP_MINIT_FUNCTION(phpdbg) /* {{{ */
{
	ZEND_INIT_MODULE_GLOBALS(phpdbg, php_phpdbg_globals_ctor, NULL);
#if PHP_VERSION_ID >= 50500
	zend_execute_old = zend_execute_ex;
	zend_execute_ex = phpdbg_execute_ex;
#else
	zend_execute_old = zend_execute;
	zend_execute = phpdbg_execute_ex;
#endif

	REGISTER_STRINGL_CONSTANT("PHPDBG_VERSION", PHPDBG_VERSION, sizeof(PHPDBG_VERSION)-1, CONST_CS|CONST_PERSISTENT);
	
	REGISTER_LONG_CONSTANT("PHPDBG_FILE",   FILE_PARAM, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHPDBG_METHOD", METHOD_PARAM, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHPDBG_LINENO", NUMERIC_PARAM, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHPDBG_FUNC",   STR_PARAM, CONST_CS|CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("PHPDBG_COLOR_PROMPT", PHPDBG_COLOR_PROMPT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHPDBG_COLOR_NOTICE", PHPDBG_COLOR_NOTICE, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHPDBG_COLOR_ERROR",  PHPDBG_COLOR_ERROR, CONST_CS|CONST_PERSISTENT);

	return SUCCESS;
} /* }}} */

static void php_phpdbg_destroy_bp_file(void *brake) /* {{{ */
{
	zend_hash_destroy((HashTable*)brake);
} /* }}} */

static void php_phpdbg_destroy_bp_symbol(void *brake) /* {{{ */
{
	efree((char*)((phpdbg_breaksymbol_t*)brake)->symbol);
} /* }}} */

static void php_phpdbg_destroy_bp_opcode(void *brake) /* {{{ */
{
	efree((char*)((phpdbg_breakop_t*)brake)->name);
} /* }}} */


static void php_phpdbg_destroy_bp_methods(void *brake) /* {{{ */
{
	zend_hash_destroy((HashTable*)brake);
} /* }}} */

static void php_phpdbg_destroy_bp_condition(void *data) /* {{{ */
{
	phpdbg_breakcond_t *brake = (phpdbg_breakcond_t*) data;

	if (brake) {
		if (brake->ops) {
			TSRMLS_FETCH();

			destroy_op_array(
					brake->ops TSRMLS_CC);
			efree(brake->ops);
		}
		efree((char*)brake->code);
	}
} /* }}} */

static void php_phpdbg_destroy_registered(void *data) /* {{{ */
{
	zend_function *function = (zend_function*) data;
	TSRMLS_FETCH();

	destroy_zend_function(
		function TSRMLS_CC);
} /* }}} */

static PHP_RINIT_FUNCTION(phpdbg) /* {{{ */
{
	zend_hash_init(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE],   8, NULL, php_phpdbg_destroy_bp_file, 0);
	zend_hash_init(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM], 8, NULL, php_phpdbg_destroy_bp_symbol, 0);
	zend_hash_init(&PHPDBG_G(bp)[PHPDBG_BREAK_FUNCTION_OPLINE], 8, NULL, php_phpdbg_destroy_bp_methods, 0);
	zend_hash_init(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD_OPLINE], 8, NULL, php_phpdbg_destroy_bp_methods, 0);
	zend_hash_init(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE_OPLINE], 8, NULL, php_phpdbg_destroy_bp_methods, 0);
	zend_hash_init(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], 8, NULL, NULL, 0);
	zend_hash_init(&PHPDBG_G(bp)[PHPDBG_BREAK_OPCODE], 8, NULL, php_phpdbg_destroy_bp_opcode, 0);
	zend_hash_init(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD], 8, NULL, php_phpdbg_destroy_bp_methods, 0);
	zend_hash_init(&PHPDBG_G(bp)[PHPDBG_BREAK_COND], 8, NULL, php_phpdbg_destroy_bp_condition, 0);
	zend_hash_init(&PHPDBG_G(bp)[PHPDBG_BREAK_MAP], 8, NULL, NULL, 0);
	
	zend_hash_init(&PHPDBG_G(seek), 8, NULL, NULL, 0);
	zend_hash_init(&PHPDBG_G(registered), 8, NULL, php_phpdbg_destroy_registered, 0);

	return SUCCESS;
} /* }}} */

static PHP_RSHUTDOWN_FUNCTION(phpdbg) /* {{{ */
{
	zend_hash_destroy(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE]);
	zend_hash_destroy(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM]);
	zend_hash_destroy(&PHPDBG_G(bp)[PHPDBG_BREAK_FUNCTION_OPLINE]);
	zend_hash_destroy(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD_OPLINE]);
	zend_hash_destroy(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE_OPLINE]);
	zend_hash_destroy(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE]);
	zend_hash_destroy(&PHPDBG_G(bp)[PHPDBG_BREAK_OPCODE]);
	zend_hash_destroy(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD]);
	zend_hash_destroy(&PHPDBG_G(bp)[PHPDBG_BREAK_COND]);
	zend_hash_destroy(&PHPDBG_G(bp)[PHPDBG_BREAK_MAP]);
	zend_hash_destroy(&PHPDBG_G(seek));
	zend_hash_destroy(&PHPDBG_G(registered));

	if (PHPDBG_G(exec)) {
		efree(PHPDBG_G(exec));
		PHPDBG_G(exec) = NULL;
	}

	if (PHPDBG_G(prompt)[0]) {
		free(PHPDBG_G(prompt)[0]);
	}
	if (PHPDBG_G(prompt)[1]) {
		free(PHPDBG_G(prompt)[1]);
	}

	PHPDBG_G(prompt)[0] = NULL;
	PHPDBG_G(prompt)[1] = NULL;

	if (PHPDBG_G(oplog)) {
		fclose(
				PHPDBG_G(oplog));
		PHPDBG_G(oplog) = NULL;
	}

	if (PHPDBG_G(ops)) {
		destroy_op_array(PHPDBG_G(ops) TSRMLS_CC);
		efree(PHPDBG_G(ops));
		PHPDBG_G(ops) = NULL;
	}

	return SUCCESS;
} /* }}} */

/* {{{ proto mixed phpdbg_exec(string context) 
	Attempt to set the execution context for phpdbg
	If the execution context was set previously it is returned
	If the execution context was not set previously boolean true is returned 
	If the request to set the context fails, boolean false is returned, and an E_WARNING raised */
static PHP_FUNCTION(phpdbg_exec) 
{
	char *exec = NULL;
	zend_ulong exec_len = 0L;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &exec, &exec_len) == FAILURE) {
		return;
	}
	
	{
		struct stat sb;
		zend_bool result = 1;
		
		if (VCWD_STAT(exec, &sb) != FAILURE) {
			if (sb.st_mode & (S_IFREG|S_IFLNK)) {
				if (PHPDBG_G(exec)) {
					ZVAL_STRINGL(return_value, PHPDBG_G(exec), PHPDBG_G(exec_len), 1);
					efree(PHPDBG_G(exec));
					result = 0;
				}
			
				PHPDBG_G(exec) = estrndup(exec, exec_len);
				PHPDBG_G(exec_len) = exec_len;
			
				if (result) 
					ZVAL_BOOL(return_value, 1);
			} else {
				zend_error(
					E_WARNING, "Failed to set execution context (%s), not a regular file or symlink", exec);
				ZVAL_BOOL(return_value, 0);
			}
		} else {
			zend_error(
				E_WARNING, "Failed to set execution context (%s) the file does not exist", exec);

			ZVAL_BOOL(return_value, 0);
		}
	}
} /* }}} */

/* {{{ proto void phpdbg_break([integer type, string expression])
    instructs phpdbg to insert a breakpoint at the next opcode */
static PHP_FUNCTION(phpdbg_break)
{
	if (ZEND_NUM_ARGS() > 0) {
		long type;
		char *expr = NULL;
		zend_uint expr_len = 0;
		phpdbg_param_t param;

		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &type, &expr, &expr_len) == FAILURE) {
			return;
		}

		phpdbg_parse_param(expr, expr_len, &param TSRMLS_CC);

		switch (type) {
			case METHOD_PARAM:
				phpdbg_do_break_method(&param, NULL TSRMLS_CC);
				break;

			case FILE_PARAM:
				phpdbg_do_break_file(&param, NULL TSRMLS_CC);
				break;

			case NUMERIC_PARAM:
				phpdbg_do_break_lineno(&param, NULL TSRMLS_CC);
				break;

			case STR_PARAM:
				phpdbg_do_break_func(&param, NULL TSRMLS_CC);
				break;

			default: zend_error(
							 E_WARNING, "unrecognized parameter type %ld", type);
		}

		phpdbg_clear_param(&param TSRMLS_CC);

	} else if (EG(current_execute_data) && EG(active_op_array)) {
		zend_ulong opline_num = (EG(current_execute_data)->opline -
				EG(active_op_array)->opcodes);

		phpdbg_set_breakpoint_opline_ex(
				&EG(active_op_array)->opcodes[opline_num+1] TSRMLS_CC);
	}
} /* }}} */

/* {{{ proto void phpdbg_clear(void)
   instructs phpdbg to clear breakpoints */
static PHP_FUNCTION(phpdbg_clear)
{
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_FUNCTION_OPLINE]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD_OPLINE]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE_OPLINE]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_COND]);
} /* }}} */

/* {{{ proto void phpdbg_color(integer element, string color) */
static PHP_FUNCTION(phpdbg_color)
{
	long element;
	char *color;
	zend_uint color_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &element, &color, &color_len) == FAILURE) {
		return;
	}

	switch (element) {
		case PHPDBG_COLOR_NOTICE:
		case PHPDBG_COLOR_ERROR:
		case PHPDBG_COLOR_PROMPT:
			phpdbg_set_color_ex(element, color, color_len TSRMLS_CC);
		break;

		default: zend_error(E_ERROR, "phpdbg detected an incorrect color constant");
	}
} /* }}} */

/* {{{ proto void phpdbg_prompt(string prompt) */
static PHP_FUNCTION(phpdbg_prompt)
{
	char *prompt;
	zend_uint prompt_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &prompt, &prompt_len) == FAILURE) {
		return;
	}

	phpdbg_set_prompt(prompt TSRMLS_CC);
} /* }}} */

ZEND_BEGIN_ARG_INFO_EX(phpdbg_break_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, type)
	ZEND_ARG_INFO(0, expression)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(phpdbg_color_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, element)
	ZEND_ARG_INFO(0, color)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(phpdbg_prompt_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, string)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(phpdbg_exec_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(phpdbg_clear_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

zend_function_entry phpdbg_user_functions[] = {
	PHP_FE(phpdbg_clear, phpdbg_clear_arginfo)
	PHP_FE(phpdbg_break, phpdbg_break_arginfo)
	PHP_FE(phpdbg_exec,  phpdbg_exec_arginfo)
	PHP_FE(phpdbg_color, phpdbg_color_arginfo)
	PHP_FE(phpdbg_prompt, phpdbg_prompt_arginfo)
#ifdef  PHP_FE_END
	PHP_FE_END
#else
	{NULL,NULL,NULL}
#endif
};

static zend_module_entry sapi_phpdbg_module_entry = {
	STANDARD_MODULE_HEADER,
	PHPDBG_NAME,
	phpdbg_user_functions,
	PHP_MINIT(phpdbg),
	NULL,
	PHP_RINIT(phpdbg),
	PHP_RSHUTDOWN(phpdbg),
	NULL,
	PHPDBG_VERSION,
	STANDARD_MODULE_PROPERTIES
};

static inline int php_sapi_phpdbg_module_startup(sapi_module_struct *module) /* {{{ */
{
	if (php_module_startup(module, &sapi_phpdbg_module_entry, 1) == FAILURE) {
		return FAILURE;
	}
	
	phpdbg_booted=1;
	
	return SUCCESS;
} /* }}} */

static char* php_sapi_phpdbg_read_cookies(TSRMLS_D) /* {{{ */
{
	return NULL;
} /* }}} */

static int php_sapi_phpdbg_header_handler(sapi_header_struct *h, sapi_header_op_enum op, sapi_headers_struct *s TSRMLS_DC) /* {{{ */
{
	return 0;
}
/* }}} */

static int php_sapi_phpdbg_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC) /* {{{ */
{
	/* We do nothing here, this function is needed to prevent that the fallback
	 * header handling is called. */
	return SAPI_HEADER_SENT_SUCCESSFULLY;
}
/* }}} */

static void php_sapi_phpdbg_send_header(sapi_header_struct *sapi_header, void *server_context TSRMLS_DC) /* {{{ */
{
}
/* }}} */

static void php_sapi_phpdbg_log_message(char *message TSRMLS_DC) /* {{{ */
{
	/*
	* We must not request TSRM before being boot
	*/
	if (phpdbg_booted) {
		phpdbg_error("%s", message);

		switch (PG(last_error_type)) {
			case E_ERROR:
			case E_CORE_ERROR:
			case E_COMPILE_ERROR:
			case E_USER_ERROR:
			case E_PARSE:
			case E_RECOVERABLE_ERROR:
				if (!(PHPDBG_G(flags) & PHPDBG_IN_EVAL)) {
					phpdbg_list_file(
						zend_get_executed_filename(TSRMLS_C),
						3,
						zend_get_executed_lineno(TSRMLS_C)-1,
						zend_get_executed_lineno(TSRMLS_C)
						TSRMLS_CC
					);
				}

				do {
					switch (phpdbg_interactive(TSRMLS_C)) {
						case PHPDBG_LEAVE:
						case PHPDBG_FINISH:
						case PHPDBG_UNTIL:
						case PHPDBG_NEXT:
							return;
					}
				} while (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING));

		}
	} else fprintf(stdout, "%s\n", message);
}
/* }}} */

static int php_sapi_phpdbg_deactivate(TSRMLS_D) /* {{{ */
{
	fflush(stdout);
	if(SG(request_info).argv0) {
		free(SG(request_info).argv0);
		SG(request_info).argv0 = NULL;
	}
	return SUCCESS;
}
/* }}} */

static void php_sapi_phpdbg_register_vars(zval *track_vars_array TSRMLS_DC) /* {{{ */
{
	unsigned int len;
	char   *docroot = "";

	/* In phpdbg mode, we consider the environment to be a part of the server variables
	*/
	php_import_environment_variables(track_vars_array TSRMLS_CC);

	if (PHPDBG_G(exec)) {
		len = PHPDBG_G(exec_len);
		if (sapi_module.input_filter(PARSE_SERVER, "PHP_SELF",
					&PHPDBG_G(exec), PHPDBG_G(exec_len), &len TSRMLS_CC)) {
			php_register_variable("PHP_SELF", PHPDBG_G(exec),
					track_vars_array TSRMLS_CC);
		}
		if (sapi_module.input_filter(PARSE_SERVER, "SCRIPT_NAME",
					&PHPDBG_G(exec), PHPDBG_G(exec_len), &len TSRMLS_CC)) {
			php_register_variable("SCRIPT_NAME", PHPDBG_G(exec),
					track_vars_array TSRMLS_CC);
		}

		if (sapi_module.input_filter(PARSE_SERVER, "SCRIPT_FILENAME",
					&PHPDBG_G(exec), PHPDBG_G(exec_len), &len TSRMLS_CC)) {
			php_register_variable("SCRIPT_FILENAME", PHPDBG_G(exec),
					track_vars_array TSRMLS_CC);
		}
		if (sapi_module.input_filter(PARSE_SERVER, "PATH_TRANSLATED",
					&PHPDBG_G(exec), PHPDBG_G(exec_len), &len TSRMLS_CC)) {
			php_register_variable("PATH_TRANSLATED", PHPDBG_G(exec),
					track_vars_array TSRMLS_CC);
		}
	}

	/* any old docroot will doo */
	len = 0U;
	if (sapi_module.input_filter(PARSE_SERVER, "DOCUMENT_ROOT",
				&docroot, len, &len TSRMLS_CC)) {
		php_register_variable("DOCUMENT_ROOT", docroot, track_vars_array TSRMLS_CC);
	}
}
/* }}} */

static inline int php_sapi_phpdbg_ub_write(const char *message, unsigned int length TSRMLS_DC) /* {{{ */
{
	return phpdbg_write("%s", message);
} /* }}} */

#if PHP_VERSION_ID >= 50700
static inline void php_sapi_phpdbg_flush(void *context TSRMLS_DC)  /* {{{ */
{
#else
static inline void php_sapi_phpdbg_flush(void *context)  /* {{{ */
{
	TSRMLS_FETCH();
#endif

	fflush(PHPDBG_G(io)[PHPDBG_STDOUT]);
} /* }}} */

/* {{{ sapi_module_struct phpdbg_sapi_module
*/
static sapi_module_struct phpdbg_sapi_module = {
	"phpdbg",                       /* name */
	"phpdbg",                       /* pretty name */

	php_sapi_phpdbg_module_startup, /* startup */
	php_module_shutdown_wrapper,    /* shutdown */

	NULL,                           /* activate */
	php_sapi_phpdbg_deactivate,     /* deactivate */

	php_sapi_phpdbg_ub_write,       /* unbuffered write */
	php_sapi_phpdbg_flush,          /* flush */
	NULL,                           /* get uid */
	NULL,                           /* getenv */

	php_error,                      /* error handler */

	php_sapi_phpdbg_header_handler, /* header handler */
	php_sapi_phpdbg_send_headers,   /* send headers handler */
	php_sapi_phpdbg_send_header,    /* send header handler */

	NULL,                           /* read POST data */
	php_sapi_phpdbg_read_cookies,   /* read Cookies */

	php_sapi_phpdbg_register_vars,  /* register server variables */
	php_sapi_phpdbg_log_message,    /* Log message */
	NULL,                           /* Get request time */
	NULL,                           /* Child terminate */
	STANDARD_SAPI_MODULE_PROPERTIES
};
/* }}} */

const opt_struct OPTIONS[] = { /* {{{ */
	{'c', 1, "ini path override"},
	{'d', 1, "define ini entry on command line"},
	{'n', 0, "no php.ini"},
	{'z', 1, "load zend_extension"},
	/* phpdbg options */
	{'q', 0, "no banner"},
	{'e', 1, "exec"},
	{'v', 0, "disable quietness"},
	{'s', 0, "enable stepping"},
	{'b', 0, "boring colours"},
	{'i', 1, "specify init"},
	{'I', 0, "ignore init"},
	{'O', 1, "opline log"},
	{'r', 0, "run"},
	{'E', 0, "step-through-eval"},
	{'S', 1, "sapi-name"},
#ifndef _WIN32
	{'l', 1, "listen"},
	{'a', 1, "address-or-any"},
#endif
	{'V', 0, "version"},
	{'-', 0, NULL}
}; /* }}} */

const char phpdbg_ini_hardcoded[] =
"html_errors=Off\n"
"register_argc_argv=On\n"
"implicit_flush=On\n"
"display_errors=Off\n"
"log_errors=On\n"
"max_execution_time=0\n"
"max_input_time=-1\n"
"error_log=\n\0";

/* overwriteable ini defaults must be set in phpdbg_ini_defaults() */
#define INI_DEFAULT(name, value) \
        Z_SET_REFCOUNT(tmp, 0); \
        Z_UNSET_ISREF(tmp); \
        ZVAL_STRINGL(&tmp, zend_strndup(value, sizeof(value)-1), sizeof(value)-1, 0); \
        zend_hash_update(configuration_hash, name, sizeof(name), &tmp, sizeof(zval), NULL);

void phpdbg_ini_defaults(HashTable *configuration_hash) /* {{{ */
{
	zval tmp;
	INI_DEFAULT("report_zend_debug", "0");
} /* }}} */

static void phpdbg_welcome(zend_bool cleaning TSRMLS_DC) /* {{{ */
{
	/* print blurb */
	if (!cleaning) {
		phpdbg_notice("Welcome to phpdbg, the interactive PHP debugger, v%s",
				PHPDBG_VERSION);
		phpdbg_writeln("To get help using phpdbg type \"help\" and press enter");
		phpdbg_notice("Please report bugs to <%s>", PHPDBG_ISSUES);
	} else {
		phpdbg_notice("Clean Execution Environment");

		phpdbg_writeln("Classes\t\t\t%d", zend_hash_num_elements(EG(class_table)));
		phpdbg_writeln("Functions\t\t%d", zend_hash_num_elements(EG(function_table)));
		phpdbg_writeln("Constants\t\t%d", zend_hash_num_elements(EG(zend_constants)));
		phpdbg_writeln("Includes\t\t%d",  zend_hash_num_elements(&EG(included_files)));
	}
} /* }}} */

static inline void phpdbg_sigint_handler(int signo) /* {{{ */
{
	TSRMLS_FETCH();

	if (EG(in_execution)) {
		/* set signalled only when not interactive */
		if (!(PHPDBG_G(flags) & PHPDBG_IS_INTERACTIVE)) {
			PHPDBG_G(flags) |= PHPDBG_IS_SIGNALED;
		}
	} else {
		PHPDBG_G(flags) |= PHPDBG_IS_QUITTING;
		zend_bailout();
	}
} /* }}} */

#ifndef _WIN32
int phpdbg_open_socket(const char *interface, short port) /* {{{ */
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	
	switch (fd) {
		case -1:
			return -1;
			
		default: {
			int reuse = 1;

			switch (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*) &reuse, sizeof(reuse))) {
				case -1:
					close(fd);
					return -2;
				
				default: {
					struct sockaddr_in address;
					
					memset(&address, 0, sizeof(address));
					
					address.sin_port = htons(port);
					address.sin_family = AF_INET;
					
					if ((*interface == '*')) {
						address.sin_addr.s_addr = htonl(INADDR_ANY);						
					} else if (!inet_pton(AF_INET, interface, &address.sin_addr)) {
						close(fd);
						return -3;
					}
					
					switch (bind(fd, (struct sockaddr *)&address, sizeof(address))) {
						case -1:
							close(fd);
							return -4;
							
						default: {
							listen(fd, 5);
						}
					}
				}
			}
		}
	}
	
	return fd;
} /* }}} */

static inline void phpdbg_close_sockets(int (*socket)[2], FILE *streams[2]) /* {{{ */
{	
	if ((*socket)[0] >= 0) {
		shutdown(
			(*socket)[0], SHUT_RDWR);
		close((*socket)[0]);
	}
	
	if (streams[0]) {
		fclose(streams[0]);
	}
	
	if ((*socket)[1] >= 0) {
		shutdown(
			(*socket)[1], SHUT_RDWR);
		close((*socket)[1]);
	}
	
	if (streams[1]) {
		fclose(streams[1]);
	}
} /* }}} */

/* don't inline this, want to debug it easily, will inline when done */

int phpdbg_open_sockets(char *address, int port[2], int (*listen)[2], int (*socket)[2], FILE* streams[2]) /* {{{ */
{
	if (((*listen)[0]) < 0 && ((*listen)[1]) < 0) {
		((*listen)[0]) = phpdbg_open_socket(address, (short)port[0]);
		((*listen)[1]) = phpdbg_open_socket(address, (short)port[1]);
	}

	streams[0] = NULL;
	streams[1] = NULL;

	if ((*listen)[0] < 0 || (*listen)[1] < 0) {
		if ((*listen)[0] < 0) {
			phpdbg_rlog(stderr,
				"console failed to initialize (stdin) on %s:%d", address, port[0]);
		}

		if ((*listen)[1] < 0) {
			phpdbg_rlog(stderr,
				"console failed to initialize (stdout) on %s:%d", address, port[1]);
		}

		if ((*listen)[0] >= 0) {
			close((*listen)[0]);
		}

		if ((*listen)[1] >= 0) {
			close((*listen)[1]);
		}

		return FAILURE;
	}

	phpdbg_close_sockets(socket, streams);

	phpdbg_rlog(stderr,
		"accepting connections on %s:%d/%d", address, port[0], port[1]);
	{
		struct sockaddr_in address;
		socklen_t size = sizeof(address);
		char buffer[20] = {0};

		{
			memset(&address, 0, size);
			(*socket)[0] = accept(
				(*listen)[0], (struct sockaddr *) &address, &size);
			inet_ntop(AF_INET, &address.sin_addr, buffer, sizeof(buffer));

			phpdbg_rlog(stderr, "connection (stdin) from %s", buffer);
		}

		{
			memset(&address, 0, size);
			(*socket)[1] = accept(
				(*listen)[1], (struct sockaddr *) &address, &size);
		    inet_ntop(AF_INET, &address.sin_addr, buffer, sizeof(buffer));

			phpdbg_rlog(stderr, "connection (stdout) from %s", buffer);
		}
	}

	dup2((*socket)[0], fileno(stdin));
	dup2((*socket)[1], fileno(stdout));
	
	setbuf(stdout, NULL);

	streams[0] = fdopen((*socket)[0], "r");
	streams[1] = fdopen((*socket)[1], "w");
	
	return SUCCESS;
} /* }}} */
#endif

int main(int argc, char **argv) /* {{{ */
{
	sapi_module_struct *phpdbg = &phpdbg_sapi_module;
	char *sapi_name;
	char *ini_entries;
	int   ini_entries_len;
	char **zend_extensions = NULL;
	zend_ulong zend_extensions_len = 0L;
	zend_bool ini_ignore;
	char *ini_override;
	char *exec;
	size_t exec_len;
	char *init_file;
	size_t init_file_len;
	zend_bool init_file_default;
	char *oplog_file;
	size_t oplog_file_len;
	zend_ulong flags;
	char *php_optarg;
	int php_optind, opt, show_banner = 1;
	long cleaning = 0;
	zend_bool remote = 0;
	int run = 0;
	int step = 0;
	char *bp_tmp_file;
#ifndef _WIN32
	char *address;
	int listen[2];
	int server[2];
	int socket[2];
	FILE* streams[2] = {NULL, NULL};
#endif

#ifdef ZTS
	void ***tsrm_ls;
#endif

#ifndef _WIN32
	address = strdup("127.0.0.1");
	socket[0] = -1;
	socket[1] = -1;
	listen[0] = -1;
	listen[1] = -1;
	server[0] = -1;
	server[1] = -1;
	streams[0] = NULL;
	streams[1] = NULL;
#endif

#ifdef PHP_WIN32
	_fmode = _O_BINARY;                 /* sets default for file streams to binary */
	setmode(_fileno(stdin), O_BINARY);  /* make the stdio mode be binary */
	setmode(_fileno(stdout), O_BINARY); /* make the stdio mode be binary */
	setmode(_fileno(stderr), O_BINARY); /* make the stdio mode be binary */
#endif

#ifdef ZTS
	tsrm_startup(1, 1, 0, NULL);

	tsrm_ls = ts_resource(0);
#endif

phpdbg_main:
	if (!cleaning) {
		bp_tmp_file = malloc(L_tmpnam);
		tmpnam(bp_tmp_file);
		if (bp_tmp_file == NULL) {
			phpdbg_error("Unable to create temporary file");
		}
	}
	ini_entries = NULL;
	ini_entries_len = 0;
	ini_ignore = 0;
	ini_override = NULL;
	zend_extensions = NULL;
	zend_extensions_len = 0L;
	exec = NULL;
	exec_len = 0;
	init_file = NULL;
	init_file_len = 0;
	init_file_default = 1;
	oplog_file = NULL;
	oplog_file_len = 0;
	flags = PHPDBG_DEFAULT_FLAGS;
	php_optarg = NULL;
	php_optind = 1;
	opt = 0;
	run = 0;
	step = 0;
	sapi_name = NULL;
	
	
	while ((opt = php_getopt(argc, argv, OPTIONS, &php_optarg, &php_optind, 0, 2)) != -1) {
		switch (opt) {
			case 'r':
				run++;
				break;
			case 'n':
				ini_ignore = 1;
				break;
			case 'c':
				if (ini_override) {
					free(ini_override);
				}
				ini_override = strdup(php_optarg);
				break;
			case 'd': {
				int len = strlen(php_optarg);
				char *val;

				if ((val = strchr(php_optarg, '='))) {
				  val++;
				  if (!isalnum(*val) && *val != '"' && *val != '\'' && *val != '\0') {
					  ini_entries = realloc(ini_entries, ini_entries_len + len + sizeof("\"\"\n\0"));
					  memcpy(ini_entries + ini_entries_len, php_optarg, (val - php_optarg));
					  ini_entries_len += (val - php_optarg);
					  memcpy(ini_entries + ini_entries_len, "\"", 1);
					  ini_entries_len++;
					  memcpy(ini_entries + ini_entries_len, val, len - (val - php_optarg));
					  ini_entries_len += len - (val - php_optarg);
					  memcpy(ini_entries + ini_entries_len, "\"\n\0", sizeof("\"\n\0"));
					  ini_entries_len += sizeof("\n\0\"") - 2;
				  } else {
					  ini_entries = realloc(ini_entries, ini_entries_len + len + sizeof("\n\0"));
					  memcpy(ini_entries + ini_entries_len, php_optarg, len);
					  memcpy(ini_entries + ini_entries_len + len, "\n\0", sizeof("\n\0"));
					  ini_entries_len += len + sizeof("\n\0") - 2;
				  }
				} else {
				  ini_entries = realloc(ini_entries, ini_entries_len + len + sizeof("=1\n\0"));
				  memcpy(ini_entries + ini_entries_len, php_optarg, len);
				  memcpy(ini_entries + ini_entries_len + len, "=1\n\0", sizeof("=1\n\0"));
				  ini_entries_len += len + sizeof("=1\n\0") - 2;
				}
			} break;
			
			case 'z':
				zend_extensions_len++;
				if (zend_extensions) {
					zend_extensions = realloc(zend_extensions, sizeof(char*) * zend_extensions_len);
				} else zend_extensions = malloc(sizeof(char*) * zend_extensions_len);
				zend_extensions[zend_extensions_len-1] = strdup(php_optarg);
			break;

			/* begin phpdbg options */

			case 'e': { /* set execution context */
				exec_len = strlen(php_optarg);
				if (exec_len) {
					if (exec) {
						free(exec);
					}
					exec = strdup(php_optarg);
				}
			} break;
			
			case 'S': { /* set SAPI name */
				if (sapi_name) {
					free(sapi_name);
				}
				sapi_name = strdup(php_optarg);
			} break;

			case 'I': { /* ignore .phpdbginit */
				init_file_default = 0;
			} break;

			case 'i': { /* set init file */
				if (init_file) {
					free(init_file);
				}
				
				init_file_len = strlen(php_optarg);
				if (init_file_len) {
					init_file = strdup(php_optarg);
				}
			} break;

			case 'O': { /* set oplog output */
				oplog_file_len = strlen(php_optarg);
				if (oplog_file_len) {
					oplog_file = strdup(php_optarg);
				}
			} break;

			case 'v': /* set quietness off */
				flags &= ~PHPDBG_IS_QUIET;
			break;

			case 's': /* set stepping on */
				step = 1;
			break;

			case 'E': /* stepping through eval on */
				flags |= PHPDBG_IS_STEPONEVAL;
			break;

			case 'b': /* set colours off */
				flags &= ~PHPDBG_IS_COLOURED;
			break;

			case 'q': /* hide banner */
				show_banner = 0;
			break;

#ifndef _WIN32
			/* if you pass a listen port, we will accept input on listen port */
			/* and write output to listen port * 2 */
			
			case 'l': { /* set listen ports */
				if (sscanf(php_optarg, "%d/%d", &listen[0], &listen[1]) != 2) {
					if (sscanf(php_optarg, "%d", &listen[0]) != 1) {
						/* default to hardcoded ports */
						listen[0] = 4000;
						listen[1] = 8000;
					} else {
						listen[1] = (listen[0] * 2);
					}
				}
			} break;
			
			case 'a': { /* set bind address */
				free(address);
				if (!php_optarg) {
					address = strdup("*");
				} else address = strdup(php_optarg);
			} break;
#endif

			case 'V': {
				sapi_startup(phpdbg);
				phpdbg->startup(phpdbg);
				printf(
					"phpdbg %s (built: %s %s)\nPHP %s, Copyright (c) 1997-2014 The PHP Group\n%s",
					PHPDBG_VERSION,
					__DATE__,
					__TIME__,
					PHP_VERSION,
					get_zend_version()
				);
				sapi_deactivate(TSRMLS_C);
				sapi_shutdown();
				return 0;
			} break;
		}
	}

#ifndef _WIN32
	/* setup remote server if necessary */
	if (!cleaning &&
		(listen[0] > 0 && listen[1] > 0)) {
		if (phpdbg_open_sockets(address, listen, &server, &socket, streams) == FAILURE) {
			remote = 0;
			exit(0);
		}
		/* set remote flag to stop service shutting down upon quit */
		remote = 1;
	}
#endif

	if (sapi_name) {
		phpdbg->name = sapi_name;
	}
	
	phpdbg->ini_defaults = phpdbg_ini_defaults;
	phpdbg->phpinfo_as_text = 1;
	phpdbg->php_ini_ignore_cwd = 1;

	sapi_startup(phpdbg);

	phpdbg->executable_location = argv[0];
	phpdbg->phpinfo_as_text = 1;
	phpdbg->php_ini_ignore = ini_ignore;
	phpdbg->php_ini_path_override = ini_override;

	if (ini_entries) {
		ini_entries = realloc(ini_entries, ini_entries_len + sizeof(phpdbg_ini_hardcoded));
		memmove(ini_entries + sizeof(phpdbg_ini_hardcoded) - 2, ini_entries, ini_entries_len + 1);
		memcpy(ini_entries, phpdbg_ini_hardcoded, sizeof(phpdbg_ini_hardcoded) - 2);
	} else {
		ini_entries = malloc(sizeof(phpdbg_ini_hardcoded));
		memcpy(ini_entries, phpdbg_ini_hardcoded, sizeof(phpdbg_ini_hardcoded));
	}
	ini_entries_len += sizeof(phpdbg_ini_hardcoded) - 2;
	
	if (zend_extensions_len) {
		zend_ulong zend_extension = 0L;
		
		while (zend_extension < zend_extensions_len) {
			const char *ze = zend_extensions[zend_extension];
			size_t ze_len = strlen(ze);
			
			ini_entries = realloc(
				ini_entries, ini_entries_len + (ze_len + (sizeof("zend_extension=\n"))));
			memcpy(&ini_entries[ini_entries_len], "zend_extension=", (sizeof("zend_extension=\n")-1));
			ini_entries_len += (sizeof("zend_extension=")-1);
			memcpy(&ini_entries[ini_entries_len], ze, ze_len);
			ini_entries_len += ze_len;
			memcpy(&ini_entries[ini_entries_len], "\n", (sizeof("\n") - 1));

			free(zend_extensions[zend_extension]);
			zend_extension++;
		}
		
		free(zend_extensions);
	}

	phpdbg->ini_entries = ini_entries;
		
	if (phpdbg->startup(phpdbg) == SUCCESS) {
		
		zend_activate(TSRMLS_C);
		
		/* do not install sigint handlers for remote consoles */
		/* sending SIGINT then provides a decent way of shutting down the server */
#ifdef ZEND_SIGNALS
# ifndef _WIN32
		if (listen[0] < 0) {
# endif
			zend_try {
				zend_signal_activate(TSRMLS_C);
				zend_signal(SIGINT, phpdbg_sigint_handler TSRMLS_CC);
			} zend_end_try();
# ifndef _WIN32
		}
# endif
#else
# ifndef _WIN32
		if (listen[0] < 0) {
# endif
			signal(SIGINT, phpdbg_sigint_handler);
#ifndef _WIN32
		}
#endif
#endif

		PG(modules_activated) = 0;
		
		/* set flags from command line */
		PHPDBG_G(flags) = flags;

#ifndef _WIN32
		/* setup io here */
		if (streams[0] && streams[1]) {
			PHPDBG_G(flags) |= PHPDBG_IS_REMOTE;

			signal(SIGPIPE, SIG_IGN);
		}
#endif

		PHPDBG_G(io)[PHPDBG_STDIN] = stdin;
		PHPDBG_G(io)[PHPDBG_STDOUT] = stdout;
		PHPDBG_G(io)[PHPDBG_STDERR] = stderr;
		
		if (exec) { /* set execution context */
			PHPDBG_G(exec) = phpdbg_resolve_path(
					exec TSRMLS_CC);
			PHPDBG_G(exec_len) = strlen(PHPDBG_G(exec));

			free(exec);
		}

		if (oplog_file) { /* open oplog */
			PHPDBG_G(oplog) = fopen(oplog_file, "w+");
			if (!PHPDBG_G(oplog)) {
				phpdbg_error(
						"Failed to open oplog %s", oplog_file);
			}
			free(oplog_file);
		}

		/* set default colors */
		phpdbg_set_color_ex(PHPDBG_COLOR_PROMPT,  PHPDBG_STRL("white-bold") TSRMLS_CC);
		phpdbg_set_color_ex(PHPDBG_COLOR_ERROR,   PHPDBG_STRL("red-bold") TSRMLS_CC);
		phpdbg_set_color_ex(PHPDBG_COLOR_NOTICE,  PHPDBG_STRL("green") TSRMLS_CC);

		/* set default prompt */
		phpdbg_set_prompt(PROMPT TSRMLS_CC);

		zend_try {
			zend_activate_modules(TSRMLS_C);
		} zend_end_try();

		if (show_banner) {
			/* print blurb */
			phpdbg_welcome((cleaning > 0) TSRMLS_CC);
		}

		zend_try {
			/* activate globals, they can be overwritten */
			zend_activate_auto_globals(TSRMLS_C);
		} zend_end_try();

		/* initialize from file */
		zend_try {
			PHPDBG_G(flags) |= PHPDBG_IS_INITIALIZING;
			phpdbg_init(init_file, init_file_len, init_file_default TSRMLS_CC);
			phpdbg_try_file_init(bp_tmp_file, strlen(bp_tmp_file), 0 TSRMLS_CC);
			PHPDBG_G(flags) &= ~PHPDBG_IS_INITIALIZING;
		} zend_catch {
			PHPDBG_G(flags) &= ~PHPDBG_IS_INITIALIZING;
			if (PHPDBG_G(flags) & PHPDBG_IS_QUITTING) {
				goto phpdbg_out;
			}
		} zend_end_try();

		/* step from here, not through init */
		if (step) {
			PHPDBG_G(flags) |= PHPDBG_IS_STEPPING;
		}

		if (run) {
			/* no need to try{}, run does it ... */
			PHPDBG_COMMAND_HANDLER(run)(NULL, NULL TSRMLS_CC);
			if (run > 1) {
				/* if -r is on the command line more than once just quit */
				goto phpdbg_out;
			}
		}

phpdbg_interact:
		/* phpdbg main() */
		do {
			zend_try {
				phpdbg_interactive(TSRMLS_C);
			} zend_catch {
				if ((PHPDBG_G(flags) & PHPDBG_IS_CLEANING)) {
					FILE *bp_tmp_fp = fopen(bp_tmp_file, "w");
					phpdbg_export_breakpoints(bp_tmp_fp TSRMLS_CC);
					fclose(bp_tmp_fp);
					cleaning = 1;
					goto phpdbg_out;
				} else {
					cleaning = 0;
				}
#ifndef _WIN32
				/* remote client disconnected */
				if ((PHPDBG_G(flags) & PHPDBG_IS_DISCONNECTED)) {

					/* renegociate connections */
					phpdbg_open_sockets(
						address, listen, &server, &socket, streams);
					
					/* set streams */
					if (streams[0] && streams[1]) {
						PHPDBG_G(flags) &= ~PHPDBG_IS_QUITTING;
					}
					
					/* this must be forced */
					CG(unclean_shutdown) = 0;
				}
#endif
				if (PHPDBG_G(flags) & PHPDBG_IS_QUITTING) {
					goto phpdbg_out;
				}
			} zend_end_try();
		} while(!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING));
		
		/* this must be forced */
		CG(unclean_shutdown) = 0;
		
phpdbg_out:
#ifndef _WIN32
		if (PHPDBG_G(flags) & PHPDBG_IS_DISCONNECTED) {
			PHPDBG_G(flags) &= ~PHPDBG_IS_DISCONNECTED;
			goto phpdbg_interact;
		}
#endif

#ifndef ZTS
		/* force cleanup of auto and core globals */
		zend_hash_clean(CG(auto_globals));
		memset(
			&core_globals, 0, sizeof(php_core_globals));
#endif
		
		if (ini_entries) {
			free(ini_entries);
		}

		if (ini_override) {
			free(ini_override);
		}

		if (PG(modules_activated)) {
			zend_try {
				zend_deactivate_modules(TSRMLS_C);
			} zend_end_try();
		}

		zend_deactivate(TSRMLS_C);

		zend_try {
			zend_post_deactivate_modules(TSRMLS_C);
		} zend_end_try();

#ifdef ZEND_SIGNALS
		zend_try {
			zend_signal_deactivate(TSRMLS_C);
		} zend_end_try();
#endif

		zend_try {
			php_module_shutdown(TSRMLS_C);
		} zend_end_try();

		sapi_shutdown();
	}

	if (cleaning || remote) {
		goto phpdbg_main;
	}

#ifdef ZTS
	/* bugggy */
	/* tsrm_shutdown(); */
#endif

#ifndef _WIN32
	if (address) {
		free(address);	
	}
#endif

	if (sapi_name) {
		free(sapi_name);
	}
	
	free(bp_tmp_file);

	return 0;
} /* }}} */
