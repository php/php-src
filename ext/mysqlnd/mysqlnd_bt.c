/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2014 The PHP Group                                |
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

/* $Id: mysqlnd_debug.c 309303 2011-03-16 12:42:59Z andrey $ */

#include "php.h"
#include "Zend/zend_builtin_functions.h"

/* Follows code borrowed from zend_builtin_functions.c because the functions there are static */

/* {{{ gettraceasstring() macros */
#define TRACE_APPEND_CHR(chr)                                            \
	*str = (char*)erealloc(*str, *len + 1 + 1);                          \
	(*str)[(*len)++] = chr

#define TRACE_APPEND_STRL(val, vallen)                                   \
	{                                                                    \
		int l = vallen;                                                  \
		*str = (char*)erealloc(*str, *len + l + 1);                      \
		memcpy((*str) + *len, val, l);                                   \
		*len += l;                                                       \
	}

#define TRACE_APPEND_STR(val)                                            \
	TRACE_APPEND_STRL(val, sizeof(val)-1)

#define TRACE_APPEND_KEY(key)                                            \
	if ((tmp = zend_hash_str_find(ht, key, sizeof(key) - 1)) != NULL) {	 \
	    TRACE_APPEND_STRL(Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));             \
	}

/* }}} */

static int
mysqlnd_build_trace_args(zval *arg TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	char **str;
	int *len;

	str = va_arg(args, char**);
	len = va_arg(args, int*);

	/* the trivial way would be to do:
	 * conver_to_string_ex(arg);
	 * append it and kill the now tmp arg.
	 * but that could cause some E_NOTICE and also damn long lines.
	 */

	switch (Z_TYPE_P(arg)) {
		case IS_NULL:
			TRACE_APPEND_STR("NULL, ");
			break;
		case IS_STRING: {
			int l_added;
			TRACE_APPEND_CHR('\'');
			if (Z_STRLEN_P(arg) > 15) {
				TRACE_APPEND_STRL(Z_STRVAL_P(arg), 15);
				TRACE_APPEND_STR("...', ");
				l_added = 15 + 6 + 1; /* +1 because of while (--l_added) */
			} else {
				l_added = Z_STRLEN_P(arg);
				TRACE_APPEND_STRL(Z_STRVAL_P(arg), l_added);
				TRACE_APPEND_STR("', ");
				l_added += 3 + 1;
			}
			while (--l_added) {
				if ((*str)[*len - l_added] < 32) {
					(*str)[*len - l_added] = '?';
				}
			}
			break;
		}
		case IS_FALSE:
			TRACE_APPEND_STR("false, ");
			break;
		case IS_TRUE:
			TRACE_APPEND_STR("true, ");
			break;
		case IS_RESOURCE:
			TRACE_APPEND_STR("Resource id #");
			/* break; */
		case IS_LONG: {
			long lval = Z_LVAL_P(arg);
			char s_tmp[MAX_LENGTH_OF_LONG + 1];
			int l_tmp = zend_sprintf(s_tmp, "%ld", lval);  /* SAFE */
			TRACE_APPEND_STRL(s_tmp, l_tmp);
			TRACE_APPEND_STR(", ");
			break;
		}
		case IS_DOUBLE: {
			double dval = Z_DVAL_P(arg);
			char *s_tmp;
			int l_tmp;

			s_tmp = emalloc(MAX_LENGTH_OF_DOUBLE + EG(precision) + 1);
			l_tmp = zend_sprintf(s_tmp, "%.*G", (int) EG(precision), dval);  /* SAFE */
			TRACE_APPEND_STRL(s_tmp, l_tmp);
			/* %G already handles removing trailing zeros from the fractional part, yay */
			efree(s_tmp);
			TRACE_APPEND_STR(", ");
			break;
		}
		case IS_ARRAY:
			TRACE_APPEND_STR("Array, ");
			break;
		case IS_OBJECT: {
			zend_string *class_name;

			TRACE_APPEND_STR("Object(");

			class_name = zend_get_object_classname(Z_OBJ_P(arg) TSRMLS_CC);

			TRACE_APPEND_STRL(class_name->val, class_name->len);

			TRACE_APPEND_STR("), ");
			break;
		}
		default:
			break;
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

static int
mysqlnd_build_trace_string(zval *frame TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	char *s_tmp, **str;
	int *len, *num;
	long line;
	HashTable *ht = Z_ARRVAL_P(frame);
	zval *file, *tmp;
	uint * level;

	level = va_arg(args, uint *);
	str = va_arg(args, char**);
	len = va_arg(args, int*);
	num = va_arg(args, int*);

	if (!*level) {
		return ZEND_HASH_APPLY_KEEP;
	}
	--*level;

	s_tmp = emalloc(1 + MAX_LENGTH_OF_LONG + 1 + 1);
	sprintf(s_tmp, "#%d ", (*num)++);
	TRACE_APPEND_STRL(s_tmp, strlen(s_tmp));
	efree(s_tmp);
	if ((file = zend_hash_str_find(ht, "file", sizeof("file") - 1)) != NULL) {
		if ((tmp = zend_hash_str_find(ht, "line", sizeof("line") - 1)) != NULL) {
			line = Z_LVAL_P(tmp);
		} else {
			line = 0;
		}
		s_tmp = emalloc(Z_STRLEN_P(file) + MAX_LENGTH_OF_LONG + 4 + 1);
		sprintf(s_tmp, "%s(%ld): ", Z_STRVAL_P(file), line);
		TRACE_APPEND_STRL(s_tmp, strlen(s_tmp));
		efree(s_tmp);
	} else {
		TRACE_APPEND_STR("[internal function]: ");
	}
	TRACE_APPEND_KEY("class");
	TRACE_APPEND_KEY("type");
	TRACE_APPEND_KEY("function");
	TRACE_APPEND_CHR('(');
	if ((tmp = zend_hash_str_find(ht, "args", sizeof("args") - 1)) != NULL) {
		int last_len = *len;
		zend_hash_apply_with_arguments(Z_ARRVAL_P(tmp) TSRMLS_CC, mysqlnd_build_trace_args, 2, str, len);
		if (last_len != *len) {
			*len -= 2; /* remove last ', ' */
		}
	}
	TRACE_APPEND_STR(")\n");
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

PHPAPI char *
mysqlnd_get_backtrace(uint max_levels, size_t * length TSRMLS_DC) /* {{{ */
{
	zval trace;
	char *res = estrdup(""), **str = &res, *s_tmp;
	int res_len = 0, *len = &res_len, num = 0;
	if (max_levels == 0) {
		max_levels = 99999;
	}

	zend_fetch_debug_backtrace(&trace, 0, 0, 0 TSRMLS_CC);

	zend_hash_apply_with_arguments(Z_ARRVAL(trace) TSRMLS_CC, mysqlnd_build_trace_string, 4, &max_levels, str, len, &num);
	zval_ptr_dtor(&trace);

	if (max_levels) {
		s_tmp = emalloc(1 + MAX_LENGTH_OF_LONG + 7 + 1);
		sprintf(s_tmp, "#%d {main}", num);
		TRACE_APPEND_STRL(s_tmp, strlen(s_tmp));
		efree(s_tmp);
	}

	res[res_len] = '\0';
	*length = res_len;

	return res;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
