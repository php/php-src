/* __header_here__ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_extname.h"

/* If you declare any globals in php_extname.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(extname)
*/

/* True global resources - no need for thread safety here */
static int le_extname;

/* Every user visible function must have an entry in extname_functions[].
*/
function_entry extname_functions[] = {
	PHP_FE(confirm_extname_compiled,	NULL)		/* For testing, remove later. */
	/* __function_entries_here__ */
	{NULL, NULL, NULL}	/* Must be the last line in extname_functions[] */
};

zend_module_entry extname_module_entry = {
	"extname",
	extname_functions,
	PHP_MINIT(extname),
	PHP_MSHUTDOWN(extname),
	PHP_RINIT(extname),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(extname),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(extname),
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_EXTNAME
ZEND_GET_MODULE(extname)
#endif

/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("extname.value",      "42", PHP_INI_ALL, OnUpdateInt, global_value, zend_extname_globals, extname_globals)
    STD_PHP_INI_ENTRY("extname.string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_extname_globals, extname_globals)
PHP_INI_END()
*/

PHP_MINIT_FUNCTION(extname)
{
/* Remove comments if you have entries in php.ini
	REGISTER_INI_ENTRIES();
*/
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(extname)
{
/* Remove comments if you have entries in php.ini
	UNREGISTER_INI_ENTRIES();
*/
	return SUCCESS;
}

/* Remove if there's nothing to do at request start */
PHP_RINIT_FUNCTION(extname)
{
	return SUCCESS;
}

/* Remove if there's nothing to do at request end */
PHP_RSHUTDOWN_FUNCTION(extname)
{
	return SUCCESS;
}

PHP_MINFO_FUNCTION(extname)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "extname support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}

/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_extname_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_extname_compiled)
{
	zval **arg;
	int len;
	char string[256];

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg);

	len = sprintf(string, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "extname", Z_STRVAL_PP(arg));
	RETURN_STRINGL(string, len, 1);
}
/* }}} */
/* The previous line is meant for emacs, so it can correctly fold and unfold
   functions in source code. See the corresponding marks just before function
   definition, where the functions purpose is also documented. Please follow
   this convention for the convenience of others editing your code.
*/

/* __function_stubs_here__ */

/* __footer_here__ */
