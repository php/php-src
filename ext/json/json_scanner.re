/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Jakub Zelenka <bukka@php.net>                                |
  +----------------------------------------------------------------------+
*/

#include "php.h"
#include "php_json_scanner.h"
#include "php_json_scanner_defs.h"
#include "php_json_parser.h"
#include "json_parser.tab.h"

#define	YYCTYPE     php_json_ctype
#define	YYCURSOR    s->cursor
#define	YYLIMIT     s->limit
#define	YYMARKER    s->marker
#define	YYCTXMARKER s->ctxmarker

#define YYGETCONDITION()        s->state
#define YYSETCONDITION(yystate) s->state = yystate

#define	YYFILL(n)

#define PHP_JSON_CONDITION_SET(condition) YYSETCONDITION(yyc##condition)
#define PHP_JSON_CONDITION_GOTO(condition) goto yyc_##condition
#define PHP_JSON_CONDITION_SET_AND_GOTO(condition) \
	PHP_JSON_CONDITION_SET(condition); \
	PHP_JSON_CONDITION_GOTO(condition)
#define PHP_JSON_CONDITION_GOTO_STR_P2() \
	do { \
		if (s->utf8_invalid) { \
			PHP_JSON_CONDITION_GOTO(STR_P2_BIN); \
		} else { \
			PHP_JSON_CONDITION_GOTO(STR_P2_UTF); \
		} \
	} while(0)


#define PHP_JSON_SCANNER_COPY_ESC() php_json_scanner_copy_string(s, 0)
#define PHP_JSON_SCANNER_COPY_UTF() php_json_scanner_copy_string(s, 5)
#define PHP_JSON_SCANNER_COPY_UTF_SP() php_json_scanner_copy_string(s, 11)

#define PHP_JSON_INT_MAX_LENGTH (MAX_LENGTH_OF_LONG - 1)


static void php_json_scanner_copy_string(php_json_scanner *s, int esc_size)
{
	size_t len = s->cursor - s->str_start - esc_size - 1;
	if (len) {
		memcpy(s->pstr, s->str_start, len);
		s->pstr += len;
	}
}

static int php_json_hex_to_int(char code)
{
	if (code >= '0' && code <= '9') {
		return code - '0';
	} else if (code >= 'A' && code <= 'F') {
		return code - ('A' - 10);
	} else if (code >= 'a' && code <= 'f') {
		return code - ('a' - 10);
	} else {
		/* this should never happened (just to suppress compiler warning) */
		return -1;
	}
}

static int php_json_ucs2_to_int_ex(php_json_scanner *s, int size, int start)
{
	int i, code = 0;
	php_json_ctype *pc = s->cursor - start;
	for (i = 0; i < size; i++) {
		code |= php_json_hex_to_int(*(pc--)) << (i * 4);
	}
	return code;
}

static int php_json_ucs2_to_int(php_json_scanner *s, int size)
{
	return php_json_ucs2_to_int_ex(s, size, 1);
}

void php_json_scanner_init(php_json_scanner *s, const char *str, size_t str_len, int options)
{
	s->cursor = (php_json_ctype *) str;
	s->limit = (php_json_ctype *) str + str_len;
	s->options = options;
	PHP_JSON_CONDITION_SET(JS);
}

int php_json_scan(php_json_scanner *s)
{
	ZVAL_NULL(&s->value);

std:
	s->token = s->cursor;

/*!re2c
	re2c:indent:top = 1;
	re2c:yyfill:enable = 0;

	DIGIT   = [0-9] ;
	DIGITNZ = [1-9] ;
	UINT    = "0" | ( DIGITNZ DIGIT* ) ;
	INT     = "-"? UINT ;
	HEX     = DIGIT | [a-fA-F] ;
	HEXNZ   = DIGITNZ | [a-fA-F] ;
	HEX7    = [0-7] ;
	HEXC    = DIGIT | [a-cA-C] ;
	FLOAT   = INT "." DIGIT+ ;
	EXP     = ( INT | FLOAT ) [eE] [+-]? DIGIT+ ;
	NL      = "\r"? "\n" ;
	WS      = [ \t\r]+ ;
	EOI     = "\000";
	CTRL    = [\x00-\x1F] ;
	UTF8T   = [\x80-\xBF] ;
	UTF8_1  = [\x00-\x7F] ;
	UTF8_2  = [\xC2-\xDF] UTF8T ;
	UTF8_3A = "\xE0" [\xA0-\xBF] UTF8T ;
	UTF8_3B = [\xE1-\xEC] UTF8T{2} ;
	UTF8_3C = "\xED" [\x80-\x9F] UTF8T ;
	UTF8_3D = [\xEE-\xEF] UTF8T{2} ;
	UTF8_3  = UTF8_3A | UTF8_3B | UTF8_3C | UTF8_3D ;
	UTF8_4A = "\xF0"[\x90-\xBF] UTF8T{2} ;
	UTF8_4B = [\xF1-\xF3] UTF8T{3} ;
	UTF8_4C = "\xF4" [\x80-\x8F] UTF8T{2} ;
	UTF8_4  = UTF8_4A | UTF8_4B | UTF8_4C ;
	UTF8    = UTF8_1 | UTF8_2 | UTF8_3 | UTF8_4 ;
	ANY     = [^] ;
	ESCPREF = "\\" ;
	ESCSYM  = ( "\"" | "\\" | "/" | [bfnrt] ) ;
	ESC     = ESCPREF ESCSYM ;
	UTFSYM  = "u" ;
	UTFPREF = ESCPREF UTFSYM ;
	UCS2    = UTFPREF HEX{4} ;
	UTF16_1 = UTFPREF "00" HEX7 HEX ;
	UTF16_2 = UTFPREF "0" HEX7 HEX{2} ;
	UTF16_3 = UTFPREF ( ( ( HEXC | [efEF] ) HEX ) | ( [dD] HEX7 ) ) HEX{2} ;
	UTF16_4 = UTFPREF [dD] [89abAB] HEX{2} UTFPREF [dD] [c-fC-F] HEX{2} ;

	<JS>"{"                  { return '{'; }
	<JS>"}"                  { return '}'; }
	<JS>"["                  { return '['; }
	<JS>"]"                  { return ']'; }
	<JS>":"                  { return ':'; }
	<JS>","                  { return ','; }
	<JS>"null"               {
		ZVAL_NULL(&s->value);
		return PHP_JSON_T_NUL;
	}
	<JS>"true"               {
		ZVAL_TRUE(&s->value);
		return PHP_JSON_T_TRUE;
	}
	<JS>"false"              {
		ZVAL_FALSE(&s->value);
		return PHP_JSON_T_FALSE;
	}
	<JS>INT                  {
		zend_bool bigint = 0, negative = s->token[0] == '-';
		size_t digits = (size_t) (s->cursor - s->token - negative);
		if (digits >= PHP_JSON_INT_MAX_LENGTH) {
			if (digits == PHP_JSON_INT_MAX_LENGTH) {
				int cmp = strncmp((char *) (s->token + negative), LONG_MIN_DIGITS, PHP_JSON_INT_MAX_LENGTH);
				if (!(cmp < 0 || (cmp == 0 && negative))) {
					bigint = 1;
				}
			} else {
				bigint = 1;
			}
		}
		if (!bigint) {
			ZVAL_LONG(&s->value, ZEND_STRTOL((char *) s->token, NULL, 10));
			return PHP_JSON_T_INT;
		} else if (s->options & PHP_JSON_BIGINT_AS_STRING) {
			ZVAL_STRINGL(&s->value, (char *) s->token, s->cursor - s->token);
			return PHP_JSON_T_STRING;
		} else {
			ZVAL_DOUBLE(&s->value, zend_strtod((char *) s->token, NULL));
			return PHP_JSON_T_DOUBLE;
		}
	}
	<JS>FLOAT|EXP            {
		ZVAL_DOUBLE(&s->value, zend_strtod((char *) s->token, NULL));
		return PHP_JSON_T_DOUBLE;
	}
	<JS>NL|WS                { goto std; }
	<JS>EOI                  {
		if (s->limit < s->cursor) {
			return PHP_JSON_T_EOI;
		} else {
			s->errcode = PHP_JSON_ERROR_CTRL_CHAR;
			return PHP_JSON_T_ERROR;
		}
	}
	<JS>["]                  {
		s->str_start = s->cursor;
		s->str_esc = 0;
		s->utf8_invalid = 0;
		s->utf8_invalid_count = 0;
		PHP_JSON_CONDITION_SET_AND_GOTO(STR_P1);
	}
	<JS>CTRL                 {
		s->errcode = PHP_JSON_ERROR_CTRL_CHAR;
		return PHP_JSON_T_ERROR;
	}
	<JS>UTF8                 {
		s->errcode = PHP_JSON_ERROR_SYNTAX;
		return PHP_JSON_T_ERROR;
	}
	<JS>ANY                  {
		s->errcode = PHP_JSON_ERROR_UTF8;
		return PHP_JSON_T_ERROR;
	}

	<STR_P1>CTRL             {
		s->errcode = PHP_JSON_ERROR_CTRL_CHAR;
		return PHP_JSON_T_ERROR;
	}
	<STR_P1>UTF16_1          {
		s->str_esc += 5;
		PHP_JSON_CONDITION_GOTO(STR_P1);
	}
	<STR_P1>UTF16_2          {
		s->str_esc += 4;
		PHP_JSON_CONDITION_GOTO(STR_P1);
	}
	<STR_P1>UTF16_3          {
		s->str_esc += 3;
		PHP_JSON_CONDITION_GOTO(STR_P1);
	}
	<STR_P1>UTF16_4          {
		s->str_esc += 8;
		PHP_JSON_CONDITION_GOTO(STR_P1);
	}
	<STR_P1>UCS2             {
		s->errcode = PHP_JSON_ERROR_UTF16;
		return PHP_JSON_T_ERROR;
	}
	<STR_P1>ESC              {
		s->str_esc++;
		PHP_JSON_CONDITION_GOTO(STR_P1);
	}
	<STR_P1>ESCPREF           {
		s->errcode = PHP_JSON_ERROR_SYNTAX;
		return PHP_JSON_T_ERROR;
	}
	<STR_P1>["]              {
		zend_string *str;
		size_t len = s->cursor - s->str_start - s->str_esc - 1 + s->utf8_invalid_count;
		if (len == 0) {
			PHP_JSON_CONDITION_SET(JS);
			ZVAL_EMPTY_STRING(&s->value);
			return PHP_JSON_T_ESTRING;
		}
		str = zend_string_alloc(len, 0);
		ZSTR_VAL(str)[len] = '\0';
		ZVAL_STR(&s->value, str);
		if (s->str_esc || s->utf8_invalid) {
			s->pstr = (php_json_ctype *) Z_STRVAL(s->value);
			s->cursor = s->str_start;
			PHP_JSON_CONDITION_GOTO_STR_P2();
		} else {
			memcpy(Z_STRVAL(s->value), s->str_start, len);
			PHP_JSON_CONDITION_SET(JS);
			return PHP_JSON_T_STRING;
		}
	}
	<STR_P1>UTF8             { PHP_JSON_CONDITION_GOTO(STR_P1); }
	<STR_P1>ANY              {
		if (s->options & (PHP_JSON_INVALID_UTF8_IGNORE | PHP_JSON_INVALID_UTF8_SUBSTITUTE)) {
			if (s->options & PHP_JSON_INVALID_UTF8_SUBSTITUTE) {
				if (s->utf8_invalid_count > INT_MAX - 2) {
					s->errcode = PHP_JSON_ERROR_UTF8;
					return PHP_JSON_T_ERROR;
				}
				s->utf8_invalid_count += 2;
			} else {
				s->utf8_invalid_count--;
			}
			s->utf8_invalid = 1;
			PHP_JSON_CONDITION_GOTO(STR_P1);
		}
		s->errcode = PHP_JSON_ERROR_UTF8;
		return PHP_JSON_T_ERROR;
	}

	<STR_P2_UTF,STR_P2_BIN>UTF16_1             {
		int utf16 = php_json_ucs2_to_int(s, 2);
		PHP_JSON_SCANNER_COPY_UTF();
		*(s->pstr++) = (char) utf16;
		s->str_start = s->cursor;
		PHP_JSON_CONDITION_GOTO_STR_P2();
	}
	<STR_P2_UTF,STR_P2_BIN>UTF16_2             {
		int utf16 = php_json_ucs2_to_int(s, 3);
		PHP_JSON_SCANNER_COPY_UTF();
		*(s->pstr++) = (char) (0xc0 | (utf16 >> 6));
		*(s->pstr++) = (char) (0x80 | (utf16 & 0x3f));
		s->str_start = s->cursor;
		PHP_JSON_CONDITION_GOTO_STR_P2();
	}
	<STR_P2_UTF,STR_P2_BIN>UTF16_3             {
		int utf16 = php_json_ucs2_to_int(s, 4);
		PHP_JSON_SCANNER_COPY_UTF();
		*(s->pstr++) = (char) (0xe0 | (utf16 >> 12));
		*(s->pstr++) = (char) (0x80 | ((utf16 >> 6) & 0x3f));
		*(s->pstr++) = (char) (0x80 | (utf16 & 0x3f));
		s->str_start = s->cursor;
		PHP_JSON_CONDITION_GOTO_STR_P2();
	}
	<STR_P2_UTF,STR_P2_BIN>UTF16_4             {
		int utf32, utf16_hi, utf16_lo;
		utf16_hi = php_json_ucs2_to_int(s, 4);
		utf16_lo = php_json_ucs2_to_int_ex(s, 4, 7);
		utf32 = ((utf16_lo & 0x3FF) << 10) + (utf16_hi & 0x3FF) + 0x10000;
		PHP_JSON_SCANNER_COPY_UTF_SP();
		*(s->pstr++) = (char) (0xf0 | (utf32 >> 18));
		*(s->pstr++) = (char) (0x80 | ((utf32 >> 12) & 0x3f));
		*(s->pstr++) = (char) (0x80 | ((utf32 >> 6) & 0x3f));
		*(s->pstr++) = (char) (0x80 | (utf32 & 0x3f));
		s->str_start = s->cursor;
		PHP_JSON_CONDITION_GOTO_STR_P2();
	}
	<STR_P2_UTF,STR_P2_BIN>ESCPREF          {
		char esc;
		PHP_JSON_SCANNER_COPY_ESC();
		switch (*s->cursor) {
			case 'b':
				esc = '\b';
				break;
			case 'f':
				esc = '\f';				break;
			case 'n':
				esc = '\n';
				break;
			case 'r':
				esc = '\r';
				break;
			case 't':
				esc = '\t';
				break;
			case '\\':
			case '/':
			case '"':
				esc = *s->cursor;
				break;
			default:
				s->errcode = PHP_JSON_ERROR_SYNTAX;
				return PHP_JSON_T_ERROR;
		}
		*(s->pstr++) = esc;
		++YYCURSOR;
		s->str_start = s->cursor;
		PHP_JSON_CONDITION_GOTO_STR_P2();
	}
	<STR_P2_UTF,STR_P2_BIN>["] => JS        {
		PHP_JSON_SCANNER_COPY_ESC();
		return PHP_JSON_T_STRING;
	}
	<STR_P2_BIN>UTF8         { PHP_JSON_CONDITION_GOTO(STR_P2_BIN); }
	<STR_P2_BIN>ANY          {
		if (s->utf8_invalid) {
			PHP_JSON_SCANNER_COPY_ESC();
			if (s->options & PHP_JSON_INVALID_UTF8_SUBSTITUTE) {
				*(s->pstr++) = (char) (0xe0 | (0xfffd >> 12));
				*(s->pstr++) = (char) (0x80 | ((0xfffd >> 6) & 0x3f));
				*(s->pstr++) = (char) (0x80 | (0xfffd & 0x3f));
			}
			s->str_start = s->cursor;
		}
		PHP_JSON_CONDITION_GOTO(STR_P2_BIN);
	}
	<STR_P2_UTF>ANY          { PHP_JSON_CONDITION_GOTO(STR_P2_UTF); }

	<*>ANY                   {
		s->errcode = PHP_JSON_ERROR_SYNTAX;
		return PHP_JSON_T_ERROR;
	}
*/

}
