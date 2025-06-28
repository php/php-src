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
   | Authors: Max Kellermann <max.kellermann@ionos.com>                   |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_INI_BUILDER_H
#define PHP_INI_BUILDER_H

#include "php.h"

/**
 * A class which helps with constructing INI entries from the command
 * line.
 */
struct php_ini_builder {
	char *value;
	size_t length;
};

BEGIN_EXTERN_C()

static inline void php_ini_builder_init(struct php_ini_builder *b)
{
	b->value = NULL;
	b->length = 0;
}

static inline void php_ini_builder_deinit(struct php_ini_builder *b)
{
	free(b->value);
}

/**
 * Null-terminate the buffer and return it.
 */
static inline char *php_ini_builder_finish(struct php_ini_builder *b)
{
	if (b->value != NULL) {
		/* null-terminate the string */
		b->value[b->length] = '\0';
	}

	return b->value;
}

/**
 * Make room for more data.
 *
 * @param delta the number of bytes to be appended
 */
static inline void php_ini_builder_realloc(struct php_ini_builder *b, size_t delta)
{
	/* reserve enough space for the null terminator */
	b->value = realloc(b->value, b->length + delta + 1);
}

/**
 * Prepend a string.
 *
 * @param src the source string
 * @param length the size of the source string
 */
PHPAPI void php_ini_builder_prepend(struct php_ini_builder *b, const char *src, size_t length);

#define php_ini_builder_prepend_literal(b, l) php_ini_builder_prepend(b, l, strlen(l))

/**
 * Append an unquoted name/value pair.
 */
PHPAPI void php_ini_builder_unquoted(struct php_ini_builder *b, const char *name, size_t name_length, const char *value, size_t value_length);

/**
 * Append a quoted name/value pair.
 */
PHPAPI void php_ini_builder_quoted(struct php_ini_builder *b, const char *name, size_t name_length, const char *value, size_t value_length);

/**
 * Parse an INI entry from the command-line option "--define".
 */
PHPAPI void php_ini_builder_define(struct php_ini_builder *b, const char *arg);

END_EXTERN_C()

#endif
