/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stig Sæther Bakken <ssb@guardian.no>                        |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#define IS_EXT_MODULE
#ifdef COMPILE_DL_DAV
# if PHP_31
#  include "../phpdl.h"
# else
#  include "dl/phpdl.h"
# endif
#endif
#include "php.h"
#include "php_dav.h"

#if defined(THREAD_SAFE) && !PHP_31
# undef THREAD_SAFE
#endif

#if HAVE_MOD_DAV

# include "mod_dav.h"
#include "ext/standard/info.h"

/* {{{ thread safety stuff */

# ifdef THREAD_SAFE
#  define DAV_GLOBAL(a) phpdav_globals->a
#  define DAV_TLS_VARS phpdav_global_struct *phpdav_globals = TlsGetValue(PHPDAVTls); 

void *phpdav_mutex;
DWORD PHPDAVTls;
static int numthreads=0;

typedef struct phpdav_global_struct {
	phpdav_module php_dav_module;
} phpdav_global_struct;

# else /* !defined(THREAD_SAFE) */
#  define DAV_GLOBAL(a) a
#  define DAV_TLS_VARS

phpdav_module php_dav_module;

# endif /* defined(THREAD_SAFE) */

# define DAV_HANDLER(a) DAV_GLOBAL(php_dav_module).a##_handler
# define DAV_SET_HANDLER(a,b) \
	dav_set_handler(&DAV_GLOBAL(php_dav_module).a##_handler,(b))


/* }}} */
/* {{{ dynamically loadable module stuff */

#ifdef COMPILE_DL_DAV
ZEND_GET_MODULE(phpdav)
# endif /* COMPILE_DL */

/* }}} */
/* {{{ function prototypes */

PHP_MINIT_FUNCTION(phpdav);
PHP_MSHUTDOWN_FUNCTION(phpdav);
PHP_RSHUTDOWN_FUNCTION(phpdav);
PHP_MINFO_FUNCTION(phpdav);

/* }}} */
/* {{{ extension definition structures */

function_entry phpdav_functions[] = {
    PHP_FE(dav_set_mkcol_handlers, NULL)
    {NULL, NULL, NULL}
};

zend_module_entry phpdav_module_entry = {
    "dav",                   /* extension name */
    phpdav_functions,        /* extension function list */
    PHP_MINIT(phpdav),       /* extension-wide startup function */
    PHP_MSHUTDOWN(phpdav),   /* extension-wide shutdown function */
    NULL,       /* per-request startup function */
    PHP_RSHUTDOWN(phpdav),   /* per-request shutdown function */
    PHP_MINFO(phpdav),        /* information function */
    STANDARD_MODULE_PROPERTIES
};

/* }}} */
/* {{{ startup, shutdown and info functions */

PHP_MINIT_FUNCTION(phpdav)
{
#if defined(THREAD_SAFE)
    phpdav_global_struct *phpdav_globals;
    PHP_MUTEX_ALLOC(phpdav_mutex);
    PHP_MUTEX_LOCK(phpdav_mutex);
    numthreads++;
    if (numthreads==1){
		if (!PHP3_TLS_PROC_STARTUP(PHPDAVTls)){
			PHP_MUTEX_UNLOCK(phpdav_mutex);
			PHP_MUTEX_FREE(phpdav_mutex);
			return FAILURE;
		}
    }
    PHP_MUTEX_UNLOCK(phdpav_mutex);
    if(!PHP3_TLS_THREAD_INIT(PHPDAVTls,phpdav_globals,phpdav_global_struct)){
		PHP_MUTEX_FREE(phpdav_mutex);
		return FAILURE;
    }
#endif
    return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(phpdav)
{
    DAV_TLS_VARS;
#ifdef THREAD_SAFE
    PHP3_TLS_THREAD_FREE(phpdav_globals);
    PHP_MUTEX_LOCK(phpdav_mutex);
    numthreads--;
    if (numthreads < 1) {
		PHP3_TLS_PROC_SHUTDOWN(PHPDAVTls);
		PHP_MUTEX_UNLOCK(phpdav_mutex);
		PHP_MUTEX_FREE(phpdav_mutex);
		return SUCCESS;
    }
    PHP_MUTEX_UNLOCK(phpdav_mutex);
#endif
    return SUCCESS;
}


PHP_RSHUTDOWN_FUNCTION(phpdav)
{
    if (DAV_HANDLER(mkcol_test)) {
		efree(DAV_HANDLER(mkcol_test));
    }
    if (DAV_HANDLER(mkcol_create)) {
		efree(DAV_HANDLER(mkcol_create));
    }
    return SUCCESS;
}


PHP_MINFO_FUNCTION(phpdav);
{
	php_info_print_table_start();
	php_info_print_table_row(2, "DAV Support", "enabled");
	php_info_print_table_end();
}

/* {{{ extension-internal functions */

    /* {{{ dav_set_handler() */

static void
dav_set_handler(char **nameBufp, pval *data)
{
    if (data->value.str.len > 0) {
		if (*nameBufp != NULL) {
			efree(*nameBufp);
		}
		*nameBufp = php3i_pval_strdup(data);
    } else {
		if (*nameBufp != NULL) {
			efree(*nameBufp);
		}
		*nameBufp = NULL;
    }
}

/* }}} */
    /* {{{ dav_call_handler() */

static int
dav_call_handler(char *funcName, int argc, pval **argv)
{
    if (funcName) {
		pval *retval, *func;
		int i, ret;
		HashTable *function_table;

		func = php3i_string_pval(funcName);
		ALLOC_ZVAL(retval);
		function_table = php3i_get_function_table();
		if (call_user_function(function_table, NULL, func, retval, argc, argv) == FAILURE) {
			php3tls_pval_destructor(retval);
			efree(retval);
			return HTTP_INTERNAL_SERVER_ERROR;
		}
		php3tls_pval_destructor(func);
		efree(func);
		for (i = 0; i < argc; i++) {
			php3tls_pval_destructor(argv[i]);
			efree(argv[i]);
		}
		convert_to_long(retval);
		ret = retval->value.lval;
		efree(retval);
		return ret;
    }
    return DECLINED;
}

/* }}} */

int phpdav_mkcol_test_handler(request_rec *r)
{
    pval *arg;

    if (DAV_HANDLER(mkcol_test) == NULL) {
		return DECLINED;
    }
    arg = php3i_string_pval(r->filename);
	return dav_call_handler(DAV_HANDLER(mkcol_test), 1, &arg);
}

int phpdav_mkcol_create_handler(request_rec *r)
{
    pval *arg;

    if (DAV_HANDLER(mkcol_create) == NULL) {
		return DECLINED;
    }
    arg = php3i_string_pval(r->filename);
	return dav_call_handler(DAV_HANDLER(mkcol_create), 1, &arg);
}

/* }}} */

/************************* EXTENSION FUNCTIONS *************************/

/* {{{ proto void dav_set_mkcol_handlers(string test, string create)
   Sets the function to test whether a DAV collection exists for MKCOL */
PHP_FUNCTION(dav_set_mkcol_handlers)
{
    pval *test, *create;
    DAV_TLS_VARS;

    if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &test, &create) == FAILURE) {
		WRONG_PARAM_COUNT;
    }
    DAV_SET_HANDLER(mkcol_test, test);
    DAV_SET_HANDLER(mkcol_create, create);
    RETVAL_TRUE;
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
