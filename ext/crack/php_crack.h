#ifndef ZEND_CRACK_H
#define ZEND_CRACK_H

#if HAVE_CRACK

extern zend_module_entry crack_module_entry;
#define phpext_crack_ptr &crack_module_entry

#ifdef ZEND_WIN32
#define ZEND_CRACK_API __declspec(dllexport)
#else
#define ZEND_CRACK_API
#endif

ZEND_MINIT_FUNCTION(crack);
ZEND_MSHUTDOWN_FUNCTION(crack);
ZEND_RINIT_FUNCTION(crack);
ZEND_RSHUTDOWN_FUNCTION(crack);
PHP_MINFO_FUNCTION(crack);

ZEND_FUNCTION(crack_opendict);
ZEND_FUNCTION(crack_closedict);
ZEND_FUNCTION(crack_check);
ZEND_FUNCTION(crack_getlastmessage);

ZEND_BEGIN_MODULE_GLOBALS(crack)
	char *default_dictionary;
	char *last_message;
	long current_id;
ZEND_END_MODULE_GLOBALS(crack)

#ifdef ZTS
#define CRACKG(v) (crack_globals->v)
#define CRACKLS_FETCH() zend_crack_globals *crack_globals = ts_resource(crack_globals_id)
#else
#define CRACKG(v) (crack_globals.v)
#define CRACKLS_FETCH()
#endif

#else

#define phpext_crack_ptr NULL

#endif

#endif	/* ZEND_CRACK_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
