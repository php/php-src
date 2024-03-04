/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                        |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_MAIL_H
#define PHP_MAIL_H

PHP_MINFO_FUNCTION(mail);

PHPAPI zend_string *php_mail_build_headers(HashTable *headers);
PHPAPI extern int php_mail(const char *to, const char *subject, const char *message, const char *headers, const char *extra_cmd);

#define PHP_MAIL_BUILD_HEADER_CHECK(target, s, key, val) \
do { \
	if (Z_TYPE_P(val) == IS_STRING) { \
		php_mail_build_headers_elem(&s, key, val); \
	} else if (Z_TYPE_P(val) == IS_ARRAY) { \
		if (zend_string_equals_literal_ci(key, target)) { \
			zend_type_error("Header \"%s\" must be of type string, array given", target); \
			break; \
		} \
		php_mail_build_headers_elems(&s, key, val); \
	} else { \
		zend_type_error("Header \"%s\" must be of type array|string, %s given", ZSTR_VAL(key), zend_zval_type_name(val)); \
	} \
} while(0)


#define PHP_MAIL_BUILD_HEADER_DEFAULT(s, key, val) \
do { \
	if (Z_TYPE_P(val) == IS_STRING) { \
		php_mail_build_headers_elem(&s, key, val); \
	} else if (Z_TYPE_P(val) == IS_ARRAY) { \
		php_mail_build_headers_elems(&s, key, val); \
	} else { \
		zend_type_error("Header \"%s\" must be of type array|string, %s given", ZSTR_VAL(key), zend_zval_type_name(val)); \
	} \
} while(0)

typedef enum {
	NO_HEADER_ERROR,
	CONTAINS_LF_ONLY,
	CONTAINS_CR_ONLY,
	CONTAINS_CRLF,
	CONTAINS_NULL
} php_mail_header_value_error_type;

#endif /* PHP_MAIL_H */
