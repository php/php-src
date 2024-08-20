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

/* The HTML contexts imply these internal flags for decoding HTML text. */
#ifndef PHP_HTML_DECODE_CHARACTER_REFERENCES
#define PHP_HTML_DECODE_CHARACTER_REFERENCES 0x01 /* Decode character references. */
#endif

#ifndef PHP_HTML_DECODE_REMOVE_NULL_BYTES
#define PHP_HTML_DECODE_REMOVE_NULL_BYTES 0x02 /* Remove NULL bytes instead of replacing them. */
#endif

#ifndef PHP_HTML_DECODE_AMBIGUOUS_AMPERSAND
#define PHP_HTML_DECODE_AMBIGUOUS_AMPERSAND 0x04 /* Decode character references followed by ambiguous characters. */
#endif

PHPAPI zend_string *php_escape_html_entities(const unsigned char *old, size_t oldlen, int all, int flags, const char *hint_charset);
PHPAPI zend_string *php_escape_html_entities_ex(const unsigned char *old, size_t oldlen, int all, int flags, const char *hint_charset, bool double_encode, bool quiet);
PHPAPI zend_string *php_unescape_html_entities(zend_string *str, int all, int flags, const char *hint_charset);
PHPAPI unsigned int php_next_utf8_char(const unsigned char *str, size_t str_len, size_t *cursor, zend_result *status);
PHPAPI zend_string *php_decode_html(const zend_long flags, const zend_string *html, const zend_long offset, const zend_long length);
PHPAPI zend_string *php_decode_html_ref(const zend_long flags, const zend_string *html, const zend_long offset, int *matched_byte_length);

extern PHPAPI zend_class_entry *html_context_ce;

PHPAPI int php_html_decoding_flags_from_context(zend_object *context);

#endif /* HTML_H */
