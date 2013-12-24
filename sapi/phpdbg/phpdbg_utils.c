/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Felipe Pena <felipe@php.net>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "zend.h"
#include "php.h"
#include "spprintf.h"
#include "phpdbg.h"
#include "phpdbg_opcode.h"
#include "phpdbg_utils.h"

#ifdef _WIN32
#	include "win32/time.h"
#endif

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

/* {{{ color structures */
const static phpdbg_color_t colors[] = {
	PHPDBG_COLOR_D("none",             "0;0"),

	PHPDBG_COLOR_D("white",            "0;64"),
	PHPDBG_COLOR_D("white-bold",       "1;64"),
	PHPDBG_COLOR_D("white-underline",  "4;64"),
	PHPDBG_COLOR_D("red",              "0;31"),
	PHPDBG_COLOR_D("red-bold",         "1;31"),
	PHPDBG_COLOR_D("red-underline",    "4;31"),
	PHPDBG_COLOR_D("green",            "0;32"),
	PHPDBG_COLOR_D("green-bold",       "1;32"),
	PHPDBG_COLOR_D("green-underline",  "4;32"),
	PHPDBG_COLOR_D("yellow",           "0;33"),
	PHPDBG_COLOR_D("yellow-bold",      "1;33"),
	PHPDBG_COLOR_D("yellow-underline", "4;33"),
	PHPDBG_COLOR_D("blue",             "0;34"),
	PHPDBG_COLOR_D("blue-bold",        "1;34"),
	PHPDBG_COLOR_D("blue-underline",   "4;34"),
	PHPDBG_COLOR_D("purple",           "0;35"),
	PHPDBG_COLOR_D("purple-bold",      "1;35"),
	PHPDBG_COLOR_D("purple-underline", "4;35"),
	PHPDBG_COLOR_D("cyan",             "0;36"),
	PHPDBG_COLOR_D("cyan-bold",        "1;36"),
	PHPDBG_COLOR_D("cyan-underline",   "4;36"),
	PHPDBG_COLOR_D("black",            "0;30"),
	PHPDBG_COLOR_D("black-bold",       "1;30"),
	PHPDBG_COLOR_D("black-underline",  "4;30"),
	PHPDBG_COLOR_END
}; /* }}} */

PHPDBG_API int phpdbg_is_numeric(const char *str) /* {{{ */
{
	if (!str)
		return 0;

	for (; *str; str++) {
		if (isspace(*str) || *str == '-') {
			continue;
		}
		return isdigit(*str);
	}
	return 0;
} /* }}} */

PHPDBG_API int phpdbg_is_empty(const char *str) /* {{{ */
{
	if (!str)
		return 1;

	for (; *str; str++) {
		if (isspace(*str)) {
			continue;
		}
		return 0;
	}
	return 1;
} /* }}} */

PHPDBG_API int phpdbg_is_addr(const char *str) /* {{{ */
{
	return str[0] && str[1] && memcmp(str, "0x", 2) == 0;
} /* }}} */

PHPDBG_API int phpdbg_is_class_method(const char *str, size_t len, char **class, char **method) /* {{{ */
{
	char *sep = NULL;

	if (strstr(str, "#") != NULL)
		return 0;

	if (strstr(str, " ") != NULL)
		return 0;

	sep = strstr(str, "::");

	if (!sep || sep == str || sep+2 == str+len-1) {
		return 0;
	}

	if (class != NULL) {
	
		if (str[0] == '\\') {
			str++;
			len--;
		}
		
		*class = estrndup(str, sep - str);
		(*class)[sep - str] = 0;
	}

	if (method != NULL) {
		*method = estrndup(sep+2, str + len - (sep + 2));
	}

	return 1;
} /* }}} */

PHPDBG_API char *phpdbg_resolve_path(const char *path TSRMLS_DC) /* {{{ */
{
	char resolved_name[MAXPATHLEN];

	if (expand_filepath(path, resolved_name TSRMLS_CC) == NULL) {
		return NULL;
	}

	return estrdup(resolved_name);
} /* }}} */

PHPDBG_API const char *phpdbg_current_file(TSRMLS_D) /* {{{ */
{
	const char *file = zend_get_executed_filename(TSRMLS_C);

	if (memcmp(file, "[no active file]", sizeof("[no active file]")) == 0) {
		return PHPDBG_G(exec);
	}

	return file;
} /* }}} */

PHPDBG_API const zend_function *phpdbg_get_function(const char *fname, const char *cname TSRMLS_DC) /* {{{ */
{
	zend_function *func = NULL;
	size_t fname_len = strlen(fname);
	char *lcname = zend_str_tolower_dup(fname, fname_len);

	if (cname) {
		zend_class_entry **ce;
		size_t cname_len = strlen(cname);
		char *lc_cname = zend_str_tolower_dup(cname, cname_len);
		int ret = zend_lookup_class(lc_cname, cname_len, &ce TSRMLS_CC);

		efree(lc_cname);

		if (ret == SUCCESS) {
			zend_hash_find(&(*ce)->function_table, lcname, fname_len+1,
				(void**)&func);
		}
	} else {
		zend_hash_find(EG(function_table), lcname, fname_len+1,
			(void**)&func);
	}

	efree(lcname);
	return func;
} /* }}} */

PHPDBG_API char *phpdbg_trim(const char *str, size_t len, size_t *new_len) /* {{{ */
{
	const char *p = str;
	char *new = NULL;

	while (p && isspace(*p)) {
		++p;
		--len;
	}

	while (*p && isspace(*(p + len -1))) {
		--len;
	}

	if (len == 0) {
		new = estrndup("", sizeof(""));
		*new_len = 0;
	} else {
		new = estrndup(p, len);
		*(new + len) = '\0';

		if (new_len) {
			*new_len = len;
		}
	}

	return new;

} /* }}} */

PHPDBG_API int phpdbg_print(int type TSRMLS_DC, FILE *fp, const char *format, ...) /* {{{ */
{
	int rc = 0;
	char *buffer = NULL;
	va_list args;

	if (format != NULL && strlen(format) > 0L) {
		va_start(args, format);
		vspprintf(&buffer, 0, format, args);
		va_end(args);
	}

	/* TODO(anyone) colours */

	switch (type) {
		case P_ERROR:
			if (PHPDBG_G(flags) & PHPDBG_IS_COLOURED) {
				rc = fprintf(fp,
						"\033[%sm[%s]\033[0m\n",
						PHPDBG_G(colors)[PHPDBG_COLOR_ERROR]->code, buffer);
			} else {
				rc = fprintf(fp, "[%s]\n", buffer);
			}
		break;

		case P_NOTICE:
			if (PHPDBG_G(flags) & PHPDBG_IS_COLOURED) {
				rc = fprintf(fp,
						"\033[%sm[%s]\033[0m\n",
						PHPDBG_G(colors)[PHPDBG_COLOR_NOTICE]->code, buffer);
			} else {
				rc = fprintf(fp, "[%s]\n", buffer);
			}
		break;

		case P_WRITELN: {
			if (buffer) {
				rc = fprintf(fp, "%s\n", buffer);
			} else {
				rc = fprintf(fp, "\n");
			}
		} break;

		case P_WRITE:
			if (buffer) {
				rc = fprintf(fp, "%s", buffer);
			}
		break;

		/* no formatting on logging output */
		case P_LOG:
			if (buffer) {
				struct timeval tp;
				if (gettimeofday(&tp, NULL) == SUCCESS) {
					rc = fprintf(fp, "[%ld %.8F]: %s\n", tp.tv_sec, tp.tv_usec / 1000000.00, buffer);
				} else {
					rc = FAILURE;
				}
			}
			break;
	}

	if (buffer) {
		efree(buffer);
	}

	return rc;
} /* }}} */

PHPDBG_API int phpdbg_rlog(FILE *fp, const char *fmt, ...) { /* {{{ */
	int rc = 0;

	va_list args;
	struct timeval tp;

	va_start(args, fmt);
	if (gettimeofday(&tp, NULL) == SUCCESS) {
		char friendly[100];
		char *format = NULL, *buffer = NULL;
		const time_t tt = tp.tv_sec;

		strftime(friendly, 100, "%a %b %d %T.%%04d %Y", localtime(&tt));
		asprintf(
			&buffer, friendly, tp.tv_usec/1000);
		asprintf(
			&format, "[%s]: %s\n", buffer, fmt);
		rc = vfprintf(
			fp, format, args);

		free(format);
		free(buffer);
	}
	va_end(args);

	return rc;
} /* }}} */

PHPDBG_API const phpdbg_color_t *phpdbg_get_color(const char *name, size_t name_length TSRMLS_DC) /* {{{ */
{
	const phpdbg_color_t *color = colors;

	while (color && color->name) {
		if (name_length == color->name_length &&
			memcmp(name, color->name, name_length) == SUCCESS) {
			phpdbg_debug(
				"phpdbg_get_color(%s, %lu): %s", name, name_length, color->code);
			return color;
		}
		++color;
	}

	phpdbg_debug(
		"phpdbg_get_color(%s, %lu): failed", name, name_length);

	return NULL;
} /* }}} */

PHPDBG_API void phpdbg_set_color(int element, const phpdbg_color_t *color TSRMLS_DC) /* {{{ */
{
	PHPDBG_G(colors)[element] = color;
} /* }}} */

PHPDBG_API void phpdbg_set_color_ex(int element, const char *name, size_t name_length TSRMLS_DC) /* {{{ */
{
	const phpdbg_color_t *color = phpdbg_get_color(name, name_length TSRMLS_CC);

	if (color) {
		phpdbg_set_color(element, color TSRMLS_CC);
	} else PHPDBG_G(colors)[element] = colors;
} /* }}} */

PHPDBG_API const phpdbg_color_t* phpdbg_get_colors(TSRMLS_D) /* {{{ */
{
	return colors;
} /* }}} */

PHPDBG_API void phpdbg_set_prompt(const char *prompt TSRMLS_DC) /* {{{ */
{
	/* free formatted prompt */
	if (PHPDBG_G(prompt)[1]) {
		free(PHPDBG_G(prompt)[1]);
		PHPDBG_G(prompt)[1] = NULL;
	}
	/* free old prompt */
	if (PHPDBG_G(prompt)[0]) {
		free(PHPDBG_G(prompt)[0]);
		PHPDBG_G(prompt)[0] = NULL;
	}

	/* copy new prompt */
	PHPDBG_G(prompt)[0] = strdup(prompt);
} /* }}} */

PHPDBG_API const char *phpdbg_get_prompt(TSRMLS_D) /* {{{ */
{
	/* find cached prompt */
	if (PHPDBG_G(prompt)[1]) {
		return PHPDBG_G(prompt)[1];
	}

	/* create cached prompt */
	if ((PHPDBG_G(flags) & PHPDBG_IS_COLOURED)) {
		asprintf(
			&PHPDBG_G(prompt)[1], "\033[%sm%s\033[0m ",
			PHPDBG_G(colors)[PHPDBG_COLOR_PROMPT]->code,
			PHPDBG_G(prompt)[0]);
	} else {
		asprintf(
			&PHPDBG_G(prompt)[1], "%s ",
			PHPDBG_G(prompt)[0]);
	}

	return PHPDBG_G(prompt)[1];
} /* }}} */
