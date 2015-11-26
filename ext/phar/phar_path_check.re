/*
  +----------------------------------------------------------------------+
  | phar php single-file executable PHP extension                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2007-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Marcus Boerger <helly@php.net>                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "phar_internal.h"

phar_path_check_result phar_path_check(char **s, int *len, const char **error)
{
	const unsigned char *p = (const unsigned char*)*s;
	const unsigned char *m;

	if (*len == 1 && *p == '.') {
		*error = "current directory reference";
		return pcr_err_curr_dir;
	} else if (*len == 2 && p[0] == '.' && p[1] == '.') {
		*error = "upper directory reference";
		return pcr_err_up_dir;
	}

#define YYCTYPE         unsigned char
#define YYCURSOR        p
#define YYLIMIT         p+*len
#define YYMARKER        m
#define YYFILL(n)

loop:
/*!re2c
END = "\x00";
UTF8T   = [\x80-\xBF] ;
UTF8_1  = [\x1A-\x7F] ;
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
EOS = "/" | END;
ANY = .;
"//" 	{
			*error = "double slash";
			return pcr_err_double_slash;
		}
"/.." EOS {
			*error = "upper directory reference";
			return pcr_err_up_dir;
		}
"/." EOS {
			*error = "current directory reference";
			return pcr_err_curr_dir;
		}
"\\" {
			*error = "back-slash";
			return pcr_err_back_slash;
		}
"*" {
			*error = "star";
			return pcr_err_star;
		}
"?"	{
			if (**s == '/') {
				(*s)++;
			}
			*len = (p - (const unsigned char*)*s) -1;
			*error = NULL;
			return pcr_use_query;
		}
UTF8 {
			goto loop;
		}
END {
			if (**s == '/') {
				(*s)++;
				(*len)--;
			}
			if ((p - (const unsigned char*)*s) - 1 != *len)
			{
				*error ="illegal character";
				return pcr_err_illegal_char;
			}
			*error = NULL;
			return pcr_is_ok;
		}
ANY {
			*error ="illegal character";
			return pcr_err_illegal_char;
		}
*/
}
