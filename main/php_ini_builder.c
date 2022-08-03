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
   | Authors: Max Kellermann <max.kellermann@ionos.com>                   |
   +----------------------------------------------------------------------+
*/

#include "php_ini_builder.h"

#include <ctype.h>
#include <string.h>

PHPAPI void php_ini_builder_prepend(struct php_ini_builder *b, const char *src, size_t length)
{
	php_ini_builder_realloc(b, length);
	if (b->length > 0)
		memmove(b->value + length, b->value, b->length);
	memcpy(b->value, src, length);
	b->length += length;
}

PHPAPI void php_ini_builder_unquoted(struct php_ini_builder *b, const char *name, size_t name_length, const char *value, size_t value_length)
{
	php_ini_builder_realloc(b, name_length + 1 + value_length + 1);

	memcpy(b->value + b->length, name, name_length);
	b->length += name_length;

	b->value[b->length++] = '=';

	memcpy(b->value + b->length, value, value_length);
	b->length += value_length;

	b->value[b->length++] = '\n';
}

PHPAPI void php_ini_builder_quoted(struct php_ini_builder *b, const char *name, size_t name_length, const char *value, size_t value_length)
{
	php_ini_builder_realloc(b, name_length + 2 + value_length + 2);

	memcpy(b->value + b->length, name, name_length);
	b->length += name_length;

	b->value[b->length++] = '=';
	b->value[b->length++] = '"';

	memcpy(b->value + b->length, value, value_length);
	b->length += value_length;

	b->value[b->length++] = '"';
	b->value[b->length++] = '\n';
}

PHPAPI void php_ini_builder_define(struct php_ini_builder *b, const char *arg)
{
	const size_t len = strlen(arg);
	const char *val = strchr(arg, '=');

	if (val != NULL) {
		val++;
		if (!isalnum(*val) && *val != '"' && *val != '\'' && *val != '\0') {
			php_ini_builder_quoted(b, arg, val - arg - 1, val, arg + len - val);
		} else {
			php_ini_builder_realloc(b, len + strlen("\n"));
			memcpy(b->value + b->length, arg, len);
			b->length += len;
			b->value[b->length++] = '\n';
		}
	} else {
		php_ini_builder_unquoted(b, arg, len, "1", 1);
	}
}

