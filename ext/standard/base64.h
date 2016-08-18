/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Jim Winstead <jimw@php.net>                                  |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef BASE64_H
#define BASE64_H

PHP_FUNCTION(base64_decode);
PHP_FUNCTION(base64_encode);

PHPAPI extern zend_string *php_base64_encode(const unsigned char *, size_t);
PHPAPI extern zend_string *php_base64_decode_ex(const unsigned char *, size_t, zend_bool);
PHPAPI extern zend_string *php_base64_decode(const unsigned char *, size_t);

#endif /* BASE64_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
