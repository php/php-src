/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2010 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Georg Richter <georg@mysql.com>                             |
  |          Andrey Hristov <andrey@mysql.com>                           |
  |          Ulf Wendel <uwendel@mysql.com>                              |
  +----------------------------------------------------------------------+
*/

#ifndef MYSQLND_CHARSET_H
#define MYSQLND_CHARSET_H

PHPAPI ulong mysqlnd_cset_escape_quotes(const MYSQLND_CHARSET * const charset, char *newstr,
										const char *escapestr, size_t escapestr_len TSRMLS_DC);

PHPAPI ulong mysqlnd_cset_escape_slashes(const MYSQLND_CHARSET * const cset, char *newstr,
										 const char *escapestr, size_t escapestr_len TSRMLS_DC);

#endif /* MYSQLND_CHARSET_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
