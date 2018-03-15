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
   | Authors: Anatol Belski <ab@php.net>                                  |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include "phpdbg.h"
#include "phpdbg_eol.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg)

#define EOL_LIST_LEN 4
struct phpdbg_eol_rep phpdbg_eol_list[EOL_LIST_LEN] = {
	{"CRLF", "\r\n", PHPDBG_EOL_CRLF},
/*	{"LFCR", "\n\r", PHPDBG_EOL_LFCR},*/
	{"LF", "\n", PHPDBG_EOL_LF},
	{"CR", "\r", PHPDBG_EOL_CR},
};

int phpdbg_eol_global_update(char *name)
{

	if (0 == memcmp(name, "CRLF", 4) || 0 == memcmp(name, "crlf", 4) || 0 == memcmp(name, "DOS", 3) || 0 == memcmp(name, "dos", 3)) {
		PHPDBG_G(eol) = PHPDBG_EOL_CRLF;
	} else if (0 == memcmp(name, "LF", 2) || 0 == memcmp(name, "lf", 2) || 0 == memcmp(name, "UNIX", 4) || 0 == memcmp(name, "unix", 4)) {
		PHPDBG_G(eol) = PHPDBG_EOL_LF;
	} else if (0 == memcmp(name, "CR", 2) || 0 == memcmp(name, "cr", 2) || 0 == memcmp(name, "MAC", 3) || 0 == memcmp(name, "mac", 3)) {
		PHPDBG_G(eol) = PHPDBG_EOL_CR;
	} else {
		return FAILURE;
	}

	return SUCCESS;
}

char *phpdbg_eol_name(int id)
{
	size_t i = 0;

	while (i < EOL_LIST_LEN) {

		if (id == phpdbg_eol_list[i].id) {
			return phpdbg_eol_list[i].name;
		}

		i++;
	}

	return NULL;
}

char *phpdbg_eol_rep(int id)
{
	size_t i = 0;

	while (i < EOL_LIST_LEN) {

		if (id == phpdbg_eol_list[i].id) {
			return phpdbg_eol_list[i].rep;
		}

		i++;
	}

	return NULL;
}


/* Inspired by https://ccrma.stanford.edu/~craig/utility/flip/flip.cpp */
void phpdbg_eol_convert(char **str, int *len)
{
	char *in = *str, *out ;
	int in_len = *len, out_len, cursor, i;
	char last, cur;

	if ((PHPDBG_G(flags) & PHPDBG_IS_REMOTE) != PHPDBG_IS_REMOTE) {
		return;
	}

	out_len = *len;
	if (PHPDBG_EOL_CRLF == PHPDBG_G(eol)) { /* XXX add LFCR case if it's gonna be needed */
		/* depending on the source EOL the out str will have all CR/LF duplicated */
		for (i = 0; i < in_len; i++) {
			if (0x0a == in[i] || 0x0d == in[i]) {
				out_len++;
			}
		}
		out = (char *)emalloc(out_len);

		last = cur = in[0];
		i = cursor = 0;
		for (; i < in_len;) {
			if (0x0a == cur && last != 0x0d) {
				out[cursor] = 0x0d;
				cursor++;
				out[cursor] = cur;
			} else if(0x0d == cur) {
				if (i + 1 < in_len && 0x0a != in[i+1]) {
					out[cursor] = cur;
					cursor++;
					out[cursor] = 0x0a;
					last = 0x0a;
				} else {
					out[cursor] = 0x0d;
					last = 0x0d;
				}
			} else {
				out[cursor] = cur;
				last = cur;
			}

			i++;
			cursor++;
			cur = in[i];
		}

	} else if (PHPDBG_EOL_LF == PHPDBG_G(eol) || PHPDBG_EOL_CR == PHPDBG_G(eol)) {
		char want, kick;

		if (PHPDBG_EOL_LF == PHPDBG_G(eol)) {
			want = 0x0a;
			kick = 0x0d;
		} else {
			want = 0x0d;
			kick = 0x0a;
		}

		/* We gonna have a smaller or equally long string, estimation is almost neglecting */
		out = (char *)emalloc(out_len);

		last = cur = in[0];
		i = cursor = 0;
		for (; cursor < in_len;) {
			if (kick == cur) {
				out[cursor] = want;
			} else if (want == cur) {
				if (kick != last) {
					out[cursor] = want;
				}
			} else {
				out[cursor] = cur;
			}

			last = cur;
			cursor++;
			cur = in[cursor];
		}
	} else {
		return;
	}

	efree(*str);
	*str = erealloc(out, cursor);
	*len = cursor;
	in = NULL;
}
