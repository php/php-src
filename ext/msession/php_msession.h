/*
   +----------------------------------------------------------------------+
   | msession 1.0                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Mark Woodward <markw@mohawksoft.com>                        |
   |    Portions copyright the PHP group.                                 |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_MSESSION_H
#define PHP_MSESSION_H

/* You should tweak config.m4 so this symbol (or some else suitable)
   gets defined.
*/
#if HAVE_MSESSION

extern zend_module_entry msession_module_entry;
#define phpext_msession_ptr &msession_module_entry

#ifdef PHP_WIN32
#define PHP_MSESSION_API __declspec(dllexport)
#else
#define PHP_MSESSION_API
#endif

PHP_MINIT_FUNCTION(msession);
PHP_MSHUTDOWN_FUNCTION(msession);
PHP_RINIT_FUNCTION(msession);
PHP_RSHUTDOWN_FUNCTION(msession);
PHP_MINFO_FUNCTION(msession);

PHP_FUNCTION(msession_connect);
PHP_FUNCTION(msession_disconnect);
PHP_FUNCTION(msession_lock);
PHP_FUNCTION(msession_unlock);
PHP_FUNCTION(msession_ctl);
PHP_FUNCTION(msession_count);
PHP_FUNCTION(msession_create);
PHP_FUNCTION(msession_destroy);
PHP_FUNCTION(msession_exec);
PHP_FUNCTION(msession_set);
PHP_FUNCTION(msession_get);
PHP_FUNCTION(msession_find);
PHP_FUNCTION(msession_get_array);
PHP_FUNCTION(msession_set_array);
PHP_FUNCTION(msession_timeout);
PHP_FUNCTION(msession_inc);
PHP_FUNCTION(msession_set_data);
PHP_FUNCTION(msession_get_data);
PHP_FUNCTION(msession_listvar);
PHP_FUNCTION(msession_list);
PHP_FUNCTION(msession_uniq);
PHP_FUNCTION(msession_randstr);
PHP_FUNCTION(msession_plugin);
PHP_FUNCTION(msession_call);
PHP_FUNCTION(msession_ping);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(msession)
	int global_variable;
	void *conn;
	char *host;
ZEND_END_MODULE_GLOBALS(msession)

*/
/* In every function that needs to use variables in php_msession_globals,
   do call MSESSIONLS_FETCH(); after declaring other variables used by
   that function, and always refer to them as MSESSIONG(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define MSESSIONG(v) (msession_globals->v)
#define MSESSIONLS_FETCH() php_msession_globals *msession_globals = ts_resource(msession_globals_id)
#else
#define MSESSIONG(v) (msession_globals.v)
#define MSESSIONLS_FETCH()
#endif

#else

#define phpext_msession_ptr NULL

#endif

#endif	/* PHP_MSESSION_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
