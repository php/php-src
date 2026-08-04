/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Author: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                        |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_MAIL_H
#define PHP_MAIL_H

PHP_MINFO_FUNCTION(mail);

PHPAPI zend_string *php_mail_build_headers(const HashTable *headers);
PHPAPI extern bool php_mail(const char *to, const char *subject, const char *message, const char *headers, const zend_string *extra_cmd);

#endif /* PHP_MAIL_H */
