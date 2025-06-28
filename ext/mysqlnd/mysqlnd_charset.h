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
  | Authors: Andrey Hristov <andrey@php.net>                             |
  |          Ulf Wendel <uw@php.net>                                     |
  |          Georg Richter <georg@php.net>                               |
  +----------------------------------------------------------------------+
*/

#ifndef MYSQLND_CHARSET_H
#define MYSQLND_CHARSET_H

#define MYSQLND_UTF8_MB3_DEFAULT_ID 33
#define MYSQLND_UTF8_MB4_DEFAULT_ID 45

PHPAPI zend_ulong mysqlnd_cset_escape_quotes(const MYSQLND_CHARSET * const charset, char * newstr,
											 const char * escapestr, const size_t escapestr_len);

PHPAPI zend_ulong mysqlnd_cset_escape_slashes(const MYSQLND_CHARSET * const cset, char * newstr,
										 	  const char * escapestr, const size_t escapestr_len);

struct st_mysqlnd_plugin_charsets
{
	const struct st_mysqlnd_plugin_header plugin_header;
	struct
	{
		const MYSQLND_CHARSET * (*const find_charset_by_nr)(unsigned int charsetnr);
		const MYSQLND_CHARSET * (*const find_charset_by_name)(const char * const name);
		zend_ulong 			(*const escape_quotes)(const MYSQLND_CHARSET * const cset, char * newstr, const char * escapestr, const size_t escapestr_len);
		zend_ulong			(*const escape_slashes)(const MYSQLND_CHARSET * const cset, char * newstr, const char * escapestr, const size_t escapestr_len);
	} methods;
};

void mysqlnd_charsets_plugin_register(void);

#endif /* MYSQLND_CHARSET_H */
