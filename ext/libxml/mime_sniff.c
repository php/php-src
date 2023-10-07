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
   | Authors: Niels Dossche <nielsdos@php.net>                            |
   +----------------------------------------------------------------------+
*/

/* This file implements the MIME sniff algorithm from https://mimesniff.spec.whatwg.org/#parsing-a-mime-type (Date: 2023-09-27)
 * It is a strict implementation of the algorithm, i.e. it does not accept malformed headers.
 * In particular, it exposes php_dom_sniff_charset() to parse the charset from the Content-Type header.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#ifdef HAVE_LIBXML

#include "php_libxml.h"

static bool is_not_slash(char c)
{
	return c != '/';
}

static bool is_not_semicolon(char c)
{
	return c != ';';
}

static bool is_not_semicolon_or_equals(char c)
{
	return c != ';' && c != '=';
}

static bool is_not_quote_or_backslash(char c)
{
	return c != '"' && c != '\\';
}

/* https://fetch.spec.whatwg.org/#http-tab-or-space */
static bool is_http_tab_or_space(char c)
{
	return c == 0x09 || c == 0x20;
}

/* https://fetch.spec.whatwg.org/#http-whitespace */
static bool is_http_whitespace(char c)
{
	return c == 0x0A || c == 0x0D || is_http_tab_or_space(c);
}

/* https://mimesniff.spec.whatwg.org/#http-quoted-string-token-code-point */
static bool is_http_quoted_string_token(unsigned char c) /* Note: unsigned is important to let the >= 0x20 check work properly! */
{
	return c == 0x09 || (c >= 0x20 && c != 0x7F);
}

/* https://infra.spec.whatwg.org/#collect-a-sequence-of-code-points
 * Implemented by returning the length of the sequence */
static zend_always_inline size_t collect_a_sequence_of_code_points(const char *position, const char *end, bool (*condition)(char))
{
	const char *start = position;
	while (position < end && condition(*position)) {
		position++;
	}
	return position - start;
}

/* https://fetch.spec.whatwg.org/#collect-an-http-quoted-string with extract-value always true */
static zend_string *collect_an_http_quoted_string_with_extract_value(const char *position, const char *end, const char **position_out)
{
	/* 1. Saving positionStart is not necessary, as in the extract-value == true variant we don't use it */

	/* 2. Let value be the empty string */
	zend_string *value = zend_string_alloc(end - position /* can't be longer than this */, false);
	ZSTR_LEN(value) = 0;

	/* 3. Assert */
	ZEND_ASSERT(*position == '"');

	/* 4. Advance */
	position++;

	/* 5. While true */
	while (true) {
		/* 5.1. Append the result of collect a sequence of code points that are not '"' or '\\' */
		size_t length = collect_a_sequence_of_code_points(position, end, is_not_quote_or_backslash);
		memcpy(ZSTR_VAL(value) + ZSTR_LEN(value), position, length);
		ZSTR_LEN(value) += length;
		position += length;

		/* 5.2. Past end check */
		if (position >= end) {
			break;
		}

		/* 5.3. quoteOrBackslash is the code point at position */
		char quote_or_backslash = *position;

		/* 5.4. Advance */
		position++;

		/* 5.5. quote_or_backslash is '\\', deal with escaping */
		if (quote_or_backslash == '\\') {
			/* 5.5.1. Past end check */
			if (position >= end) {
				ZSTR_VAL(value)[ZSTR_LEN(value)] = '\\';
				ZSTR_LEN(value)++;
				break;
			}

			/* 5.5.2. Append code point at position */
			ZSTR_VAL(value)[ZSTR_LEN(value)] = *position;
			ZSTR_LEN(value)++;

			/* 5.5.3. Advance */
			position++;
		} else {
			/* 5.6. Otherwise: assert and break */
			ZEND_ASSERT(quote_or_backslash == '"');
			break;
		}
	}

	ZSTR_VAL(value)[ZSTR_LEN(value)] = '\0';

	*position_out = position;

	/* 6. extract-value is always true, return value */
	/* Step 7 is not needed because we always return here already */
	return value;
}

/* https://infra.spec.whatwg.org/#ascii-alphanumeric */
static bool is_ascii_alpha_numeric(char c)
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

/* https://mimesniff.spec.whatwg.org/#http-token-code-point */
static bool is_http_token(char c)
{
	return c == 0x21
		|| (c >= 0x23 && c <= 0x27)
		|| c == 0x2A || c == 0x2B || c == 0x2D || c == 0x2E
		|| c == 0x5E || c == 0x5F
		|| c == 0x60
		|| c == 0x7C || c == 0x7E
		|| is_ascii_alpha_numeric(c);
}

static bool is_empty_string_or_does_not_solely_contain_http_token_code_points(const char *start, size_t len)
{
	if (len == 0) {
		return true;
	}
	while (len > 0) {
		if (!is_http_token(*start)) {
			return true;
		}
		len--;
		start++;
	}
	return false;
}

static bool solely_contains_http_quoted_string_tokens(const char *start, size_t len)
{
	while (len > 0) {
		if (!is_http_quoted_string_token(*start)) {
			return false;
		}
		len--;
		start++;
	}
	return true;
}

/* https://mimesniff.spec.whatwg.org/#parsing-a-mime-type
 * Note: We only care about the charset detection */
PHP_LIBXML_API zend_string *php_libxml_sniff_charset_from_string(const char *start, const char *end)
{
	/* 1. Remove leading & trailing HTTP whitespace */
	while (start < end && is_http_whitespace(*start)) {
		start++;
	}
	while (start < end && is_http_whitespace(*end)) {
		end--;
	}

	/* 2. Position variable: no-op because we move the start pointer instead */

	/* 3. Collect sequence of code points that are not '/' (for type) */
	size_t type_length = collect_a_sequence_of_code_points(start, end, is_not_slash);

	/* 4. Empty string or not solely http tokens */
	if (is_empty_string_or_does_not_solely_contain_http_token_code_points(start, type_length)) {
		return NULL;
	}
	start += type_length;

	/* 5. Failure if past end of input (note: end is one past the last char; in practice this is only possible if no '/' was found) */
	if (start >= end) {
		return NULL;
	}

	/* 6. Skip '/' */
	start++;

	/* 7. Collect sequence of code points that are not ';' (for subtype) */
	size_t subtype_length = collect_a_sequence_of_code_points(start, end, is_not_semicolon);

	/* 8. Remove trailing HTTP whitespace from subtype, but we don't care about subtype, so no-op */

	/* 9. Empty string or not solely http tokens */
	if (is_empty_string_or_does_not_solely_contain_http_token_code_points(start, subtype_length)) {
		return NULL;
	}
	start += subtype_length;

	/* 10. Initialise stuff, no-op as well as we don't care about anything other than charset */

	/* 11. Loop with check: position not past end */
	while (start < end) {
		/* 11.1. Advance position */
		start++;

		/* 11.2. Collect sequence that *is* HTTP whitespace */
		size_t whitespace_length = collect_a_sequence_of_code_points(start, end, is_http_whitespace);
		start += whitespace_length;

		/* 11.3. Collect a sequence of code points that are not ';' or '=' (for parameterName) */
		size_t parameter_name_length = collect_a_sequence_of_code_points(start, end, is_not_semicolon_or_equals);
		const char *parameter_name = start;
		start += parameter_name_length;

		/* 11.4. Convert parameter_name to ASCII lowercase, no-op because we are only interested in charset which we'll match down below */

		/* 11.5. Position past input check */
		if (start < end) {
			if (*start == ';') {
				continue;
			}
			start++;
		} else {
			/* 11.6. */
			break;
		}

		/* 11.7. Let parameterValue be null */
		zend_string *parameter_value = NULL;

		/* 11.8. Quoted string check */
		if (*start == '"') {
			/* 11.8.1. Set parameterValue to the result of collecting an HTTP quoted string */
			parameter_value = collect_an_http_quoted_string_with_extract_value(start, end, &start);

			/* 11.8.2. Collect a sequence of code points that are not ';' */
			start += collect_a_sequence_of_code_points(start, end, is_not_semicolon);
		} else {
			/* 9. Otherwise */
			/* 9.1. Set parameterValue to the result of collecting a sequence of code points that are not ';' */
			size_t parameter_value_length = collect_a_sequence_of_code_points(start, end, is_not_semicolon);
			parameter_value = zend_string_init(start, parameter_value_length, false);
			start += parameter_name_length;

			/* 9.2. Remove trailing HTTP whitespace from parameterValue */
			while (ZSTR_LEN(parameter_value) > 0 && is_http_whitespace(ZSTR_VAL(parameter_value)[ZSTR_LEN(parameter_value) - 1])) {
				ZSTR_LEN(parameter_value)--;
			}
			ZSTR_VAL(parameter_value)[ZSTR_LEN(parameter_value)] = '\0';

			/* 9.3. Continue if parameterValue is empty */
			if (ZSTR_LEN(parameter_value) == 0) {
				zend_string_release_ex(parameter_value, false);
				continue;
			}
		}

		/* 10. We diverge from the spec here: we're only interested in charset.
		 *     Furthermore, as only the first match matters, we can stop immediately with the loop once we set the charset. */
		if (parameter_name_length == strlen("charset")
			&& strncasecmp(parameter_name, "charset", strlen("charset")) == 0 /* Because of lowercasing in step 11.4 */
			&& solely_contains_http_quoted_string_tokens(ZSTR_VAL(parameter_value), ZSTR_LEN(parameter_value))) {
			return parameter_value;
		}

		zend_string_release_ex(parameter_value, false);
	}

	/* 12. Return mimetype, a no-op / spec divergence */
	return NULL;
}

PHP_LIBXML_API zend_string *php_libxml_sniff_charset_from_stream(const php_stream *s)
{
	if (Z_TYPE(s->wrapperdata) == IS_ARRAY) {
		zval *header;

		ZEND_HASH_FOREACH_VAL_IND(Z_ARRVAL(s->wrapperdata), header) {
			const char buf[] = "Content-Type:";
			if (Z_TYPE_P(header) == IS_STRING &&
					!zend_binary_strncasecmp(Z_STRVAL_P(header), Z_STRLEN_P(header), buf, sizeof(buf)-1, sizeof(buf)-1)) {
				return php_libxml_sniff_charset_from_string(Z_STRVAL_P(header) + sizeof(buf) - 1, Z_STRVAL_P(header) + Z_STRLEN_P(header));
			}
		} ZEND_HASH_FOREACH_END();
	}

	return NULL;
}

#endif  /* HAVE_LIBXML */
