/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                        |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_MAIL_H
#define PHP_MAIL_H

PHP_FUNCTION(mail);
PHP_FUNCTION(ezmlm_hash);

PHP_MINFO_FUNCTION(mail);

PHPAPI zend_string *php_mail_build_headers(zval *headers);
PHPAPI extern int php_mail(char *to, char *subject, char *message, char *headers, char *extra_cmd);

#define PHP_MAIL_BUILD_HEADER_CHECK(target, s, key, val) \
do { \
	if (Z_TYPE_P(val) == IS_STRING) { \
		php_mail_build_headers_elem(&s, key, val); \
	} else if (Z_TYPE_P(val) == IS_ARRAY) { \
		if (!strncasecmp(target, ZSTR_VAL(key), ZSTR_LEN(key))) { \
			php_error_docref(NULL, E_WARNING, "'%s' header must be at most one header. Array is passed for '%s'", target, target); \
			continue; \
		} \
		php_mail_build_headers_elems(&s, key, val); \
	} else { \
		php_error_docref(NULL, E_WARNING, "Extra header element '%s' cannot be other than string or array.", ZSTR_VAL(key)); \
	} \
} while(0)


#define PHP_MAIL_BUILD_HEADER_DEFAULT(s, key, val) \
do { \
	if (Z_TYPE_P(val) == IS_STRING) { \
		php_mail_build_headers_elem(&s, key, val); \
	} else if (Z_TYPE_P(val) == IS_ARRAY) { \
		php_mail_build_headers_elems(&s, key, val); \
	} else { \
		php_error_docref(NULL, E_WARNING, "Extra header element '%s' cannot be other than string or array.", ZSTR_VAL(key)); \
	} \
} while(0)


#endif /* PHP_MAIL_H */
