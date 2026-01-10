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
  | Author: Matteo Beccati <mbeccati@php.net>                            |
  +----------------------------------------------------------------------+
*/


#include "php.h"
#include "ext/pdo/php_pdo_driver.h"
#include "ext/pdo/pdo_sql_parser.h"

int pdo_pgsql_scanner(pdo_scanner_t *s)
{
	const char *cursor = s->cur;

	s->tok = cursor;
	/*!re2c
	BINDCHR		= [:][a-zA-Z0-9_]+;
	QUESTION	= [?];
	ESCQUESTION	= [?][?];
	COMMENTS	= ("/*"([^*]+|[*]+[^/*])*[*]*"*/"|"--".*);
	DOLQ_START	= [A-Za-z\200-\377_];
	DOLQ_CONT	= [A-Za-z\200-\377_0-9];
	SPECIALS	= [$eE:?"'/-];
	MULTICHAR	= [:]{2,};
	ANYNOEOF	= [\001-\377];
	*/

	/*!re2c
		[eE](['](([']['])|([\\]ANYNOEOF)|ANYNOEOF\['\\])*[']) { RET(PDO_PARSER_TEXT); }
		(["]((["]["])|ANYNOEOF\["])*["])		{ RET(PDO_PARSER_TEXT); }
		(['](([']['])|ANYNOEOF\['])*['])		{ RET(PDO_PARSER_TEXT); }
		[$](DOLQ_START DOLQ_CONT*)?[$]			{ RET(PDO_PARSER_CUSTOM_QUOTE); }
		MULTICHAR								{ RET(PDO_PARSER_TEXT); }
		ESCQUESTION								{ RET(PDO_PARSER_ESCAPED_QUESTION); }
		BINDCHR									{ RET(PDO_PARSER_BIND); }
		QUESTION								{ RET(PDO_PARSER_BIND_POS); }
		SPECIALS								{ SKIP_ONE(PDO_PARSER_TEXT); }
		COMMENTS								{ RET(PDO_PARSER_TEXT); }
		(ANYNOEOF\SPECIALS)+ 					{ RET(PDO_PARSER_TEXT); }
	*/
}
