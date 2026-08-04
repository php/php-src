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
  | Author: Matteo Beccati <mbeccati@php.net>                            |
  +----------------------------------------------------------------------+
*/


#include "php.h"
#include "ext/pdo/php_pdo_driver.h"
#include "ext/pdo/pdo_sql_parser.h"

int pdo_sqlite_scanner(pdo_scanner_t *s)
{
	const char *cursor = s->cur;

	s->tok = cursor;
	/*!re2c
	BINDCHR		= [:][a-zA-Z0-9_]+;
	QUESTION	= [?];
	COMMENTS	= ("/*"([^*]+|[*]+[^/*])*[*]*"*/"|"--".*);
	SPECIALS	= [:?"'`/[-];
	MULTICHAR	= ([:]{2,}|[?]{2,});
	ANYNOEOF	= [\001-\377];
	*/

	/*!re2c
		(["]((["]["])|ANYNOEOF)*["])			{ RET(PDO_PARSER_TEXT); }
		(['](([']['])|ANYNOEOF)*['])			{ RET(PDO_PARSER_TEXT); }
		([`](([`][`])|ANYNOEOF)*[`])			{ RET(PDO_PARSER_TEXT); }
		("["ANYNOEOF*"]")						{ RET(PDO_PARSER_TEXT); }
		MULTICHAR								{ RET(PDO_PARSER_TEXT); }
		BINDCHR									{ RET(PDO_PARSER_BIND); }
		QUESTION								{ RET(PDO_PARSER_BIND_POS); }
		SPECIALS								{ SKIP_ONE(PDO_PARSER_TEXT); }
		COMMENTS								{ RET(PDO_PARSER_TEXT); }
		(ANYNOEOF\SPECIALS)+ 					{ RET(PDO_PARSER_TEXT); }
	*/
}
