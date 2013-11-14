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

#include "phpdbg.h"
#include "phpdbg_prompt.h"
#include "phpdbg_bp.h"
#include "phpdbg_utils.h"

ZEND_DECLARE_MODULE_GLOBALS(phpdbg);

void (*zend_execute_old)(zend_execute_data *execute_data TSRMLS_DC);
void (*zend_execute_internal_old)(zend_execute_data *execute_data_ptr, zend_fcall_info *fci, int return_value_used TSRMLS_DC);

static inline void php_phpdbg_globals_ctor(zend_phpdbg_globals *pg) /* {{{ */
{
    pg->exec = NULL;
    pg->exec_len = 0;
    pg->ops = NULL;
    pg->vmret = 0;
    pg->bp_count = 0;
    pg->last = NULL;
    pg->last_params = NULL;
    pg->last_params_len = 0;
    pg->flags = PHPDBG_DEFAULT_FLAGS;
} /* }}} */

static PHP_MINIT_FUNCTION(phpdbg) /* {{{ */
{
    ZEND_INIT_MODULE_GLOBALS(phpdbg, php_phpdbg_globals_ctor, NULL);

    zend_execute_old = zend_execute_ex;
    zend_execute_ex = phpdbg_execute_ex;

    return SUCCESS;
} /* }}} */

static void php_phpdbg_destroy_bp_file(void *brake) /* {{{ */
{
	zend_llist_destroy((zend_llist*)brake);
} /* }}} */

static void php_phpdbg_destroy_bp_symbol(void *brake) /* {{{ */
{
	efree((char*)((phpdbg_breaksymbol_t*)brake)->symbol);
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
        zval_dtor(&brake->code);
    }
} /* }}} */

static PHP_RINIT_FUNCTION(phpdbg) /* {{{ */
{
	zend_hash_init(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE],   8, NULL, php_phpdbg_destroy_bp_file, 0);
	zend_hash_init(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM], 8, NULL, php_phpdbg_destroy_bp_symbol, 0);
    zend_hash_init(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], 8, NULL, NULL, 0);
    zend_hash_init(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD], 8, NULL, php_phpdbg_destroy_bp_methods, 0);
    zend_hash_init(&PHPDBG_G(bp)[PHPDBG_BREAK_COND], 8, NULL, php_phpdbg_destroy_bp_condition, 0);

	return SUCCESS;
} /* }}} */

static PHP_RSHUTDOWN_FUNCTION(phpdbg) /* {{{ */
{
    zend_hash_destroy(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE]);
    zend_hash_destroy(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM]);
    zend_hash_destroy(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE]);
    zend_hash_destroy(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD]);
    zend_hash_destroy(&PHPDBG_G(bp)[PHPDBG_BREAK_COND]);

    if (PHPDBG_G(exec)) {
        efree(PHPDBG_G(exec));
        PHPDBG_G(exec) = NULL;
    }

    if (PHPDBG_G(ops)) {
        destroy_op_array(PHPDBG_G(ops) TSRMLS_CC);
        efree(PHPDBG_G(ops));
        PHPDBG_G(ops) = NULL;
    }
    
    return SUCCESS;
} /* }}} */

/* {{{ proto void phpdbg_break(void)
    instructs phpdbg to insert a breakpoint at the next opcode */
static PHP_FUNCTION(phpdbg_break)
{
    if (EG(current_execute_data) && EG(active_op_array)) {
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
    zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE]);
    zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD]);
    zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_COND]);
} /* }}} */

zend_function_entry phpdbg_user_functions[] = {
    PHP_FE(phpdbg_clear, NULL)
    PHP_FE(phpdbg_break, NULL)
#ifdef  PHP_FE_END
	PHP_FE_END
#else
	{NULL,NULL,NULL}
#endif
};

static zend_module_entry sapi_phpdbg_module_entry = {
	STANDARD_MODULE_HEADER,
	"phpdbg",
	phpdbg_user_functions,
	PHP_MINIT(phpdbg),
	NULL,
	PHP_RINIT(phpdbg),
	PHP_RSHUTDOWN(phpdbg),
	NULL,
	"0.1",
	STANDARD_MODULE_PROPERTIES
};

static inline int php_sapi_phpdbg_module_startup(sapi_module_struct *module) /* {{{ */
{
    if (php_module_startup(module, &sapi_phpdbg_module_entry, 1) == FAILURE) {
		return FAILURE;
	}
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
	fprintf(stderr, "%s\n", message);
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

/* {{{ sapi_module_struct phpdbg_sapi_module
 */
static sapi_module_struct phpdbg_sapi_module = {
	"phpdbg",						/* name */
	"phpdbg",					    /* pretty name */

	php_sapi_phpdbg_module_startup,	/* startup */
	php_module_shutdown_wrapper,    /* shutdown */

	NULL,		                    /* activate */
	php_sapi_phpdbg_deactivate,		/* deactivate */

	NULL,			                /* unbuffered write */
	NULL,				            /* flush */
	NULL,							/* get uid */
	NULL,				            /* getenv */

	php_error,						/* error handler */

	php_sapi_phpdbg_header_handler,	/* header handler */
	php_sapi_phpdbg_send_headers,	/* send headers handler */
	php_sapi_phpdbg_send_header,	/* send header handler */

	NULL,				            /* read POST data */
	php_sapi_phpdbg_read_cookies,   /* read Cookies */

	php_sapi_phpdbg_register_vars,	/* register server variables */
	php_sapi_phpdbg_log_message,	/* Log message */
	NULL,							/* Get request time */
	NULL,							/* Child terminate */
	STANDARD_SAPI_MODULE_PROPERTIES
};
/* }}} */

const opt_struct OPTIONS[] = { /* {{{ */
	{'c', 1, "ini path override"},
	{'d', 1, "define ini entry on command line"},
	{'n', 0, "no php.ini"},
	{'z', 1, "load zend_extension"},
	/* phpdbg options */
	{'e', 1, "exec"},
	{'v', 0, "verbose"},
	{'s', 0, "step"},
	{'b', 0, "boring colours"},
	{'-', 0, NULL}
}; /* }}} */

const char phpdbg_ini_hardcoded[] =
	"html_errors=0\n"
	"register_argc_argv=1\n"
	"implicit_flush=1\n"
	"output_buffering=0\n"
	"max_execution_time=0\n"
	"max_input_time=-1\n\0";

/* overwriteable ini defaults must be set in phpdbg_ini_defaults() */
#define INI_DEFAULT(name,value)\
	Z_SET_REFCOUNT(tmp, 0);\
	Z_UNSET_ISREF(tmp);	\
	ZVAL_STRINGL(&tmp, zend_strndup(value, sizeof(value)-1), sizeof(value)-1, 0);\
	zend_hash_update(configuration_hash, name, sizeof(name), &tmp, sizeof(zval), NULL);\

void phpdbg_ini_defaults(HashTable *configuration_hash) /* {{{ */
{
    zval tmp;
	INI_DEFAULT("report_zend_debug", "0");
	INI_DEFAULT("display_errors", "1");
} /* }}} */

static jmp_buf phpdbg_main;

int main(int argc, char **argv) /* {{{ */
{
	sapi_module_struct *phpdbg = &phpdbg_sapi_module;
	char *ini_entries;
	int   ini_entries_len;
	char *exec;
	size_t exec_len;
	zend_ulong flags;
	char *php_optarg;
    int php_optind;
    int opt;
    long cleaning = 0;
    
#ifdef ZTS
	void ***tsrm_ls;
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
    ini_entries = NULL;
    ini_entries_len = 0;
    exec = NULL;
    exec_len = 0;
    flags = PHPDBG_DEFAULT_FLAGS;
    php_optarg = NULL;
    php_optind = 1;
    opt = 0;

    while ((opt = php_getopt(argc, argv, OPTIONS, &php_optarg, &php_optind, 0, 2)) != -1) {
        switch (opt) {
            case 'n':
                phpdbg->php_ini_ignore = 1;
            break;
            case 'c':
                if (phpdbg->php_ini_path_override) {
                    free(phpdbg->php_ini_path_override);
                }
                phpdbg->php_ini_path_override = strdup(php_optarg);
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
                zend_load_extension(php_optarg);
            break;

            case 'e': /* set execution context */
                exec_len = strlen(php_optarg);
                if (exec_len) {
                    exec = strdup(php_optarg);
                }
            break;

            case 'v': /* set quietness off */
                flags &= ~PHPDBG_IS_QUIET;
            break;

            case 's': /* set stepping on */
                flags |= PHPDBG_IS_STEPPING;
            break;

            case 'b': /* set colours off */
                flags &= ~PHPDBG_IS_COLOURED;
            break;
        }
    }

    phpdbg->ini_defaults = phpdbg_ini_defaults;
	phpdbg->phpinfo_as_text = 1;
	phpdbg->php_ini_ignore_cwd = 1;

	sapi_startup(phpdbg);

    phpdbg->executable_location = argv[0];
    phpdbg->phpinfo_as_text = 1;
    phpdbg->php_ini_ignore = 0;

    if (ini_entries) {
		ini_entries = realloc(ini_entries, ini_entries_len + sizeof(phpdbg_ini_hardcoded));
		memmove(ini_entries + sizeof(phpdbg_ini_hardcoded) - 2, ini_entries, ini_entries_len + 1);
		memcpy(ini_entries, phpdbg_ini_hardcoded, sizeof(phpdbg_ini_hardcoded) - 2);
	} else {
		ini_entries = malloc(sizeof(phpdbg_ini_hardcoded));
		memcpy(ini_entries, phpdbg_ini_hardcoded, sizeof(phpdbg_ini_hardcoded));
	}
	ini_entries_len += sizeof(phpdbg_ini_hardcoded) - 2;

    phpdbg->ini_entries = ini_entries;

	if (phpdbg->startup(phpdbg) == SUCCESS) {
		zend_activate(TSRMLS_C);

#ifdef ZEND_SIGNALS
		zend_try {
			zend_signals_activate(TSRMLS_C);
		} zend_end_try();
#endif

		PG(modules_activated) = 0;

        if (exec) { /* set execution context */
            PHPDBG_G(exec) = phpdbg_resolve_path(
                exec TSRMLS_CC);
            PHPDBG_G(exec_len) = strlen(PHPDBG_G(exec));

            free(exec);
        }

        /* set flags from command line */
        PHPDBG_G(flags) = flags;

		zend_try {
			zend_activate_modules(TSRMLS_C);
		} zend_end_try();

		zend_try {
		    zend_activate_auto_globals(TSRMLS_C);
		} zend_end_try();

        /* print blurb */
		phpdbg_welcome(cleaning TSRMLS_CC);

        /* phpdbg main() */
        do {
		    zend_try {
		        phpdbg_interactive(TSRMLS_C);
		    } zend_catch {
                if ((PHPDBG_G(flags) & PHPDBG_IS_CLEANING)) {
                    cleaning = 1;
                    break;
                } else cleaning = 0;
		    } zend_end_try();
		} while(!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING));

		if (ini_entries) {
		    free(ini_entries);
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

		php_module_shutdown(TSRMLS_C);

		sapi_shutdown();
	}
	
	if (cleaning) {
        goto phpdbg_main;
    }

#ifdef ZTS
    /* bugggy */
	//tsrm_shutdown();
#endif

	return 0;
} /* }}} */
