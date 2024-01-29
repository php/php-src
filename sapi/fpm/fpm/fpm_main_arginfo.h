/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: b4ac4c0f1d91c354293e21185a2e6d9f99cc9fcc */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_fastcgi_finish_request, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_apache_request_headers, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_getallheaders arginfo_apache_request_headers

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_fpm_get_status, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_FUNCTION(fastcgi_finish_request);
ZEND_FUNCTION(apache_request_headers);
ZEND_FUNCTION(fpm_get_status);

static const zend_function_entry ext_functions[] = {
	ZEND_RAW_FENTRY("fastcgi_finish_request", zif_fastcgi_finish_request, arginfo_fastcgi_finish_request, 0, NULL)
	ZEND_RAW_FENTRY("apache_request_headers", zif_apache_request_headers, arginfo_apache_request_headers, 0, NULL)
	ZEND_RAW_FENTRY("getallheaders", zif_apache_request_headers, arginfo_getallheaders, 0, NULL)
	ZEND_RAW_FENTRY("fpm_get_status", zif_fpm_get_status, arginfo_fpm_get_status, 0, NULL)
	ZEND_FE_END
};
