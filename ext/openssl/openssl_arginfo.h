/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_x509_export_to_file, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, x509)
	ZEND_ARG_TYPE_INFO(0, outfilename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, notext, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_x509_export, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, x509)
	ZEND_ARG_INFO(1, out)
	ZEND_ARG_TYPE_INFO(0, notext, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_x509_fingerprint, 0, 0, 1)
	ZEND_ARG_INFO(0, x509)
	ZEND_ARG_TYPE_INFO(0, method, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, raw_output, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_x509_check_private_key, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, cert)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_x509_verify, 0, 2, IS_LONG, 0)
	ZEND_ARG_INFO(0, cert)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_x509_parse, 0, 0, 1)
	ZEND_ARG_INFO(0, x509)
	ZEND_ARG_TYPE_INFO(0, shortname, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_x509_checkpurpose, 0, 0, 2)
	ZEND_ARG_INFO(0, x509cert)
	ZEND_ARG_TYPE_INFO(0, purpose, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, cainfo, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(0, untrustedfile, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_x509_read, 0, 0, 1)
	ZEND_ARG_INFO(0, cert)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_x509_free, 0, 0, 1)
	ZEND_ARG_INFO(0, x509)
ZEND_END_ARG_INFO()
