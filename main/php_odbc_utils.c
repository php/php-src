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
   | Authors: Calvin Buckley <calvin@cmpct.info>                          |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"

/*
 * This files contains functions shared between ext/pdo_odbc and ext/odbc,
 * relating to i.e. connection string quoting rules.
 *
 * The declarations are PHPAPI due to being available for shared/static
 * versions.
 */

/**
 * Determines if a string matches the ODBC quoting rules.
 *
 * A valid quoted string begins with a '{', ends with a '}', and has no '}'
 * inside of the string that aren't repeated (as to be escaped).
 *
 * These rules are what .NET also follows.
 */
PHPAPI bool php_odbc_connstr_is_quoted(const char *str)
{
	/* ODBC quotes are curly braces */
	if (str[0] != '{') {
		return false;
	}
	/* Check for } that aren't doubled up or at the end of the string */
	size_t length = strlen(str);
	for (size_t i = 0; i < length; i++) {
		if (str[i] == '}' && str[i + 1] == '}') {
			/* Skip over so we don't count it again */
			i++;
		} else if (str[i] == '}' && str[i + 1] != '\0') {
			/* If not at the end, not quoted */
			return false;
		}
	}
	return true;
}

/**
 * Determines if a value for a connection string should be quoted.
 *
 * The ODBC specification mentions:
 * "Because of connection string and initialization file grammar, keywords and
 * attribute values that contain the characters []{}(),;?*=!@ not enclosed
 * with braces should be avoided."
 *
 * Note that it assumes that the string is *not* already quoted. You should
 * check beforehand.
 */
PHPAPI bool php_odbc_connstr_should_quote(const char *str)
{
	return strpbrk(str, "[]{}(),;?*=!@") != NULL;
}

/**
 * Estimates the worst-case scenario for a quoted version of a string's size.
 */
PHPAPI size_t php_odbc_connstr_estimate_quote_length(const char *in_str)
{
	/* Assume all '}'. Include '{,' '}', and the null terminator too */
	return (strlen(in_str) * 2) + 3;
}

/**
 * Quotes a string with ODBC rules.
 *
 * Some characters (curly braces, semicolons) are special and must be quoted.
 * In the case of '}' in a quoted string, they must be escaped SQL style; that
 * is, repeated.
 */
PHPAPI size_t php_odbc_connstr_quote(char *out_str, const char *in_str, size_t out_str_size)
{
	*out_str++ = '{';
	out_str_size--;
	while (out_str_size > 2) {
		if (*in_str == '\0') {
			break;
		} else if (*in_str == '}' && out_str_size - 1 > 2) {
			/* enough room to append */
			*out_str++ = '}';
			*out_str++ = *in_str++;
			out_str_size -= 2;
		} else if (*in_str == '}') {
			/* not enough, truncate here */
			break;
		} else {
			*out_str++ = *in_str++;
			out_str_size--;
		}
	}
	/* append termination */
	*out_str++ = '}';
	*out_str++ = '\0';
	out_str_size -= 2;
	/* return how many characters were left */
	return strlen(in_str);
}
