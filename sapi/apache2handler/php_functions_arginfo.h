/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 2e2e63b5c845bb74309b2b3e52ca5a3d76f2c80b */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_apache_lookup_uri, 0, 1, MAY_BE_OBJECT|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_virtual, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_apache_request_headers, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_getallheaders arginfo_apache_request_headers

#define arginfo_apache_response_headers arginfo_apache_request_headers

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_apache_note, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, note_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, note_value, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_apache_setenv, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, variable, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, walk_to_top, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_apache_getenv, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, variable, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, walk_to_top, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_apache_get_version, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

#define arginfo_apache_get_modules arginfo_apache_request_headers


ZEND_FUNCTION(apache_lookup_uri);
ZEND_FUNCTION(virtual);
ZEND_FUNCTION(apache_request_headers);
ZEND_FUNCTION(apache_response_headers);
ZEND_FUNCTION(apache_note);
ZEND_FUNCTION(apache_setenv);
ZEND_FUNCTION(apache_getenv);
ZEND_FUNCTION(apache_get_version);
ZEND_FUNCTION(apache_get_modules);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(apache_lookup_uri, arginfo_apache_lookup_uri)
	ZEND_FE(virtual, arginfo_virtual)
	ZEND_FE(apache_request_headers, arginfo_apache_request_headers)
	ZEND_FALIAS(getallheaders, apache_request_headers, arginfo_getallheaders)
	ZEND_FE(apache_response_headers, arginfo_apache_response_headers)
	ZEND_FE(apache_note, arginfo_apache_note)
	ZEND_FE(apache_setenv, arginfo_apache_setenv)
	ZEND_FE(apache_getenv, arginfo_apache_getenv)
	ZEND_FE(apache_get_version, arginfo_apache_get_version)
	ZEND_FE(apache_get_modules, arginfo_apache_get_modules)
	ZEND_FE_END
};
