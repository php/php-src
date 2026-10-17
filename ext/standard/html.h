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
   | Author: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                        |
   +----------------------------------------------------------------------+
*/

#ifndef HTML_H
#define HTML_H

#define ENT_HTML_QUOTE_NONE			0
#define ENT_HTML_QUOTE_SINGLE		1
#define ENT_HTML_QUOTE_DOUBLE		2
#define ENT_HTML_IGNORE_ERRORS		4
#define ENT_HTML_SUBSTITUTE_ERRORS	8
#define ENT_HTML_DOC_TYPE_MASK		(16|32)
#define ENT_HTML_DOC_HTML401		0
#define ENT_HTML_DOC_XML1			16
#define ENT_HTML_DOC_XHTML			32
#define ENT_HTML_DOC_HTML5			(16|32)
/* reserve bit 6 */
#define ENT_HTML_SUBSTITUTE_DISALLOWED_CHARS	128

#define PHP_HTML_SPECIALCHARS 	0
#define PHP_HTML_ENTITIES	 	1

#define ENT_COMPAT		ENT_HTML_QUOTE_DOUBLE
#define ENT_QUOTES		(ENT_HTML_QUOTE_DOUBLE | ENT_HTML_QUOTE_SINGLE)
#define ENT_NOQUOTES	ENT_HTML_QUOTE_NONE
#define ENT_IGNORE		ENT_HTML_IGNORE_ERRORS
#define ENT_SUBSTITUTE	ENT_HTML_SUBSTITUTE_ERRORS
#define ENT_HTML401		0
#define ENT_XML1		16
#define ENT_XHTML		32
#define ENT_HTML5		(16|32)
#define ENT_DISALLOWED	128

PHPAPI zend_string *php_escape_html_entities(const unsigned char *old, size_t oldlen, int all, int flags, const char *hint_charset);
PHPAPI zend_string *php_escape_html_entities_ex(const unsigned char *old, size_t oldlen, int all, int flags, const char *hint_charset, bool double_encode, bool quiet);
PHPAPI zend_string *php_unescape_html_entities(zend_string *str, int all, int flags, const char *hint_charset);
PHPAPI unsigned int php_next_utf8_char(const unsigned char *str, size_t str_len, size_t *cursor, zend_result *status);

/* Same contract as php_next_utf8_char(). Decodes valid UTF-8 inline; invalid input goes
 * to php_next_utf8_char(). */
static zend_always_inline unsigned int php_next_utf8_char_inline(
		const unsigned char *str, size_t str_len, size_t *cursor, zend_result *status)
{
	size_t pos = *cursor;
	size_t avail;

	ZEND_ASSERT(pos <= str_len);
	avail = str_len - pos;
	if (EXPECTED(avail >= 1)) {
		unsigned int c = str[pos];

		if (c < 0x80) {
			*cursor = pos + 1;
			*status = SUCCESS;
			return c;
		}
		if (EXPECTED(c < 0xe0)) {
			/* C2..DF + continuation byte: always a valid shortest form */
			if (c >= 0xc2 && avail >= 2 && (str[pos + 1] & 0xc0) == 0x80) {
				*cursor = pos + 2;
				*status = SUCCESS;
				return ((c & 0x1f) << 6) | (str[pos + 1] & 0x3f);
			}
		} else if (c < 0xf0) {
			if (avail >= 3 && (str[pos + 1] & 0xc0) == 0x80 && (str[pos + 2] & 0xc0) == 0x80) {
				unsigned int cp = ((c & 0x0f) << 12) | ((str[pos + 1] & 0x3f) << 6) | (str[pos + 2] & 0x3f);
				/* neither overlong nor a surrogate */
				if (cp >= 0x800 && (cp < 0xd800 || cp > 0xdfff)) {
					*cursor = pos + 3;
					*status = SUCCESS;
					return cp;
				}
			}
		} else if (c < 0xf5) {
			if (avail >= 4 && (str[pos + 1] & 0xc0) == 0x80 && (str[pos + 2] & 0xc0) == 0x80
					&& (str[pos + 3] & 0xc0) == 0x80) {
				unsigned int cp = ((c & 0x07) << 18) | ((str[pos + 1] & 0x3f) << 12)
					| ((str[pos + 2] & 0x3f) << 6) | (str[pos + 3] & 0x3f);
				/* neither overlong nor above U+10FFFF */
				if (cp >= 0x10000 && cp <= 0x10ffff) {
					*cursor = pos + 4;
					*status = SUCCESS;
					return cp;
				}
			}
		}
	}

	return php_next_utf8_char(str, str_len, cursor, status);
}

#endif /* HTML_H */
