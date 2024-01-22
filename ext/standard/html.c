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
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Jaakko Hyvätti <jaakko.hyvatti@iki.fi>                      |
   |          Wez Furlong    <wez@thebrainroom.com>                       |
   |          Gustavo Lopes  <cataphract@php.net>                         |
   +----------------------------------------------------------------------+
*/

/*
 * HTML entity resources:
 *
 * http://www.unicode.org/Public/MAPPINGS/OBSOLETE/UNI2SGML.TXT
 *
 * XHTML 1.0 DTD
 * http://www.w3.org/TR/2002/REC-xhtml1-20020801/dtds.html#h-A2
 *
 * From HTML 4.01 strict DTD:
 * http://www.w3.org/TR/html4/HTMLlat1.ent
 * http://www.w3.org/TR/html4/HTMLsymbol.ent
 * http://www.w3.org/TR/html4/HTMLspecial.ent
 *
 * HTML 5:
 * http://dev.w3.org/html5/spec/Overview.html#named-character-references
 */

#include "php.h"
#ifdef PHP_WIN32
#include "config.w32.h"
#else
#include <php_config.h>
#endif
#include "php_standard.h"
#include "php_string.h"
#include "SAPI.h"
#include <locale.h>
#ifdef HAVE_LANGINFO_H
#include <langinfo.h>
#endif

#include <zend_hash.h>
#include "html_tables.h"

/* Macro for disabling flag of translation of non-basic entities where this isn't supported.
 * Not appropriate for html_entity_decode/htmlspecialchars_decode */
#define LIMIT_ALL(all, doctype, charset) do { \
	(all) = (all) && !CHARSET_PARTIAL_SUPPORT((charset)) && ((doctype) != ENT_HTML_DOC_XML1); \
} while (0)

#define MB_FAILURE(pos, advance) do { \
	*cursor = pos + (advance); \
	*status = FAILURE; \
	return 0; \
} while (0)

#define CHECK_LEN(pos, chars_need) ((str_len - (pos)) >= (chars_need))

/* valid as single byte character or leading byte */
#define utf8_lead(c)  ((c) < 0x80 || ((c) >= 0xC2 && (c) <= 0xF4))
/* whether it's actually valid depends on other stuff;
 * this macro cannot check for non-shortest forms, surrogates or
 * code points above 0x10FFFF */
#define utf8_trail(c) ((c) >= 0x80 && (c) <= 0xBF)

#define gb2312_lead(c) ((c) != 0x8E && (c) != 0x8F && (c) != 0xA0 && (c) != 0xFF)
#define gb2312_trail(c) ((c) >= 0xA1 && (c) <= 0xFE)

#define sjis_lead(c) ((c) != 0x80 && (c) != 0xA0 && (c) < 0xFD)
#define sjis_trail(c) ((c) >= 0x40  && (c) != 0x7F && (c) < 0xFD)

/* {{{ get_default_charset */
static char *get_default_charset(void) {
	if (PG(internal_encoding) && PG(internal_encoding)[0]) {
		return PG(internal_encoding);
	} else if (SG(default_charset) && SG(default_charset)[0] ) {
		return SG(default_charset);
	}
	return NULL;
}
/* }}} */

/* {{{ get_next_char */
static inline unsigned int get_next_char(
		enum entity_charset charset,
		const unsigned char *str,
		size_t str_len,
		size_t *cursor,
		zend_result *status)
{
	size_t pos = *cursor;
	unsigned int this_char = 0;

	*status = SUCCESS;
	assert(pos <= str_len);

	if (!CHECK_LEN(pos, 1))
		MB_FAILURE(pos, 1);

	switch (charset) {
	case cs_utf_8:
		{
			/* We'll follow strategy 2. from section 3.6.1 of UTR #36:
			 * "In a reported illegal byte sequence, do not include any
			 *  non-initial byte that encodes a valid character or is a leading
			 *  byte for a valid sequence." */
			unsigned char c;
			c = str[pos];
			if (c < 0x80) {
				this_char = c;
				pos++;
			} else if (c < 0xc2) {
				MB_FAILURE(pos, 1);
			} else if (c < 0xe0) {
				if (!CHECK_LEN(pos, 2))
					MB_FAILURE(pos, 1);

				if (!utf8_trail(str[pos + 1])) {
					MB_FAILURE(pos, utf8_lead(str[pos + 1]) ? 1 : 2);
				}
				this_char = ((c & 0x1f) << 6) | (str[pos + 1] & 0x3f);
				if (this_char < 0x80) { /* non-shortest form */
					MB_FAILURE(pos, 2);
				}
				pos += 2;
			} else if (c < 0xf0) {
				size_t avail = str_len - pos;

				if (avail < 3 ||
						!utf8_trail(str[pos + 1]) || !utf8_trail(str[pos + 2])) {
					if (avail < 2 || utf8_lead(str[pos + 1]))
						MB_FAILURE(pos, 1);
					else if (avail < 3 || utf8_lead(str[pos + 2]))
						MB_FAILURE(pos, 2);
					else
						MB_FAILURE(pos, 3);
				}

				this_char = ((c & 0x0f) << 12) | ((str[pos + 1] & 0x3f) << 6) | (str[pos + 2] & 0x3f);
				if (this_char < 0x800) { /* non-shortest form */
					MB_FAILURE(pos, 3);
				} else if (this_char >= 0xd800 && this_char <= 0xdfff) { /* surrogate */
					MB_FAILURE(pos, 3);
				}
				pos += 3;
			} else if (c < 0xf5) {
				size_t avail = str_len - pos;

				if (avail < 4 ||
						!utf8_trail(str[pos + 1]) || !utf8_trail(str[pos + 2]) ||
						!utf8_trail(str[pos + 3])) {
					if (avail < 2 || utf8_lead(str[pos + 1]))
						MB_FAILURE(pos, 1);
					else if (avail < 3 || utf8_lead(str[pos + 2]))
						MB_FAILURE(pos, 2);
					else if (avail < 4 || utf8_lead(str[pos + 3]))
						MB_FAILURE(pos, 3);
					else
						MB_FAILURE(pos, 4);
				}

				this_char = ((c & 0x07) << 18) | ((str[pos + 1] & 0x3f) << 12) | ((str[pos + 2] & 0x3f) << 6) | (str[pos + 3] & 0x3f);
				if (this_char < 0x10000 || this_char > 0x10FFFF) { /* non-shortest form or outside range */
					MB_FAILURE(pos, 4);
				}
				pos += 4;
			} else {
				MB_FAILURE(pos, 1);
			}
		}
		break;

	case cs_big5:
		/* reference http://demo.icu-project.org/icu-bin/convexp?conv=big5 */
		{
			unsigned char c = str[pos];
			if (c >= 0x81 && c <= 0xFE) {
				unsigned char next;
				if (!CHECK_LEN(pos, 2))
					MB_FAILURE(pos, 1);

				next = str[pos + 1];

				if ((next >= 0x40 && next <= 0x7E) ||
						(next >= 0xA1 && next <= 0xFE)) {
					this_char = (c << 8) | next;
				} else {
					MB_FAILURE(pos, 1);
				}
				pos += 2;
			} else {
				this_char = c;
				pos += 1;
			}
		}
		break;

	case cs_big5hkscs:
		{
			unsigned char c = str[pos];
			if (c >= 0x81 && c <= 0xFE) {
				unsigned char next;
				if (!CHECK_LEN(pos, 2))
					MB_FAILURE(pos, 1);

				next = str[pos + 1];

				if ((next >= 0x40 && next <= 0x7E) ||
						(next >= 0xA1 && next <= 0xFE)) {
					this_char = (c << 8) | next;
				} else if (next != 0x80 && next != 0xFF) {
					MB_FAILURE(pos, 1);
				} else {
					MB_FAILURE(pos, 2);
				}
				pos += 2;
			} else {
				this_char = c;
				pos += 1;
			}
		}
		break;

	case cs_gb2312: /* EUC-CN */
		{
			unsigned char c = str[pos];
			if (c >= 0xA1 && c <= 0xFE) {
				unsigned char next;
				if (!CHECK_LEN(pos, 2))
					MB_FAILURE(pos, 1);

				next = str[pos + 1];

				if (gb2312_trail(next)) {
					this_char = (c << 8) | next;
				} else if (gb2312_lead(next)) {
					MB_FAILURE(pos, 1);
				} else {
					MB_FAILURE(pos, 2);
				}
				pos += 2;
			} else if (gb2312_lead(c)) {
				this_char = c;
				pos += 1;
			} else {
				MB_FAILURE(pos, 1);
			}
		}
		break;

	case cs_sjis:
		{
			unsigned char c = str[pos];
			if ((c >= 0x81 && c <= 0x9F) || (c >= 0xE0 && c <= 0xFC)) {
				unsigned char next;
				if (!CHECK_LEN(pos, 2))
					MB_FAILURE(pos, 1);

				next = str[pos + 1];

				if (sjis_trail(next)) {
					this_char = (c << 8) | next;
				} else if (sjis_lead(next)) {
					MB_FAILURE(pos, 1);
				} else {
					MB_FAILURE(pos, 2);
				}
				pos += 2;
			} else if (c < 0x80 || (c >= 0xA1 && c <= 0xDF)) {
				this_char = c;
				pos += 1;
			} else {
				MB_FAILURE(pos, 1);
			}
		}
		break;

	case cs_eucjp:
		{
			unsigned char c = str[pos];

			if (c >= 0xA1 && c <= 0xFE) {
				unsigned next;
				if (!CHECK_LEN(pos, 2))
					MB_FAILURE(pos, 1);
				next = str[pos + 1];

				if (next >= 0xA1 && next <= 0xFE) {
					/* this a jis kanji char */
					this_char = (c << 8) | next;
				} else {
					MB_FAILURE(pos, (next != 0xA0 && next != 0xFF) ? 1 : 2);
				}
				pos += 2;
			} else if (c == 0x8E) {
				unsigned next;
				if (!CHECK_LEN(pos, 2))
					MB_FAILURE(pos, 1);

				next = str[pos + 1];
				if (next >= 0xA1 && next <= 0xDF) {
					/* JIS X 0201 kana */
					this_char = (c << 8) | next;
				} else {
					MB_FAILURE(pos, (next != 0xA0 && next != 0xFF) ? 1 : 2);
				}
				pos += 2;
			} else if (c == 0x8F) {
				size_t avail = str_len - pos;

				if (avail < 3 || !(str[pos + 1] >= 0xA1 && str[pos + 1] <= 0xFE) ||
						!(str[pos + 2] >= 0xA1 && str[pos + 2] <= 0xFE)) {
					if (avail < 2 || (str[pos + 1] != 0xA0 && str[pos + 1] != 0xFF))
						MB_FAILURE(pos, 1);
					else if (avail < 3 || (str[pos + 2] != 0xA0 && str[pos + 2] != 0xFF))
						MB_FAILURE(pos, 2);
					else
						MB_FAILURE(pos, 3);
				} else {
					/* JIS X 0212 hojo-kanji */
					this_char = (c << 16) | (str[pos + 1] << 8) | str[pos + 2];
				}
				pos += 3;
			} else if (c != 0xA0 && c != 0xFF) {
				/* character encoded in 1 code unit */
				this_char = c;
				pos += 1;
			} else {
				MB_FAILURE(pos, 1);
			}
		}
		break;
	default:
		/* single-byte charsets */
		this_char = str[pos++];
		break;
	}

	*cursor = pos;
	return this_char;
}
/* }}} */

/* {{{ php_next_utf8_char
 * Public interface for get_next_char used with UTF-8 */
PHPAPI unsigned int php_next_utf8_char(
		const unsigned char *str,
		size_t str_len,
		size_t *cursor,
		zend_result *status)
{
	return get_next_char(cs_utf_8, str, str_len, cursor, status);
}
/* }}} */

/* {{{ entity_charset determine_charset
 * Returns the charset identifier based on an explicitly provided charset,
 * the internal_encoding and default_charset ini settings, or UTF-8 by default. */
static enum entity_charset determine_charset(const char *charset_hint, bool quiet)
{
	if (!charset_hint || !*charset_hint) {
		charset_hint = get_default_charset();
	}

	if (charset_hint && *charset_hint) {
		size_t len = strlen(charset_hint);
		/* now walk the charset map and look for the codeset */
		for (size_t i = 0; i < sizeof(charset_map)/sizeof(charset_map[0]); i++) {
			if (len == charset_map[i].codeset_len &&
			    zend_binary_strcasecmp(charset_hint, len, charset_map[i].codeset, len) == 0) {
				return charset_map[i].charset;
			}
		}

		if (!quiet) {
			php_error_docref(NULL, E_WARNING, "Charset \"%s\" is not supported, assuming UTF-8",
					charset_hint);
		}
	}

	return cs_utf_8;
}
/* }}} */

/* {{{ php_utf32_utf8 */
static inline size_t php_utf32_utf8(unsigned char *buf, unsigned k)
{
	size_t retval = 0;

	/* assert(0x0 <= k <= 0x10FFFF); */

	if (k < 0x80) {
		buf[0] = k;
		retval = 1;
	} else if (k < 0x800) {
		buf[0] = 0xc0 | (k >> 6);
		buf[1] = 0x80 | (k & 0x3f);
		retval = 2;
	} else if (k < 0x10000) {
		buf[0] = 0xe0 | (k >> 12);
		buf[1] = 0x80 | ((k >> 6) & 0x3f);
		buf[2] = 0x80 | (k & 0x3f);
		retval = 3;
	} else {
		buf[0] = 0xf0 | (k >> 18);
		buf[1] = 0x80 | ((k >> 12) & 0x3f);
		buf[2] = 0x80 | ((k >> 6) & 0x3f);
		buf[3] = 0x80 | (k & 0x3f);
		retval = 4;
	}
	/* UTF-8 has been restricted to max 4 bytes since RFC 3629 */

	return retval;
}
/* }}} */

/* {{{ unimap_bsearc_cmp
 * Binary search of unicode code points in unicode <--> charset mapping.
 * Returns the code point in the target charset (whose mapping table was given) or 0 if
 * the unicode code point is not in the table.
 */
static inline unsigned char unimap_bsearch(const uni_to_enc *table, unsigned code_key_a, size_t num)
{
	const uni_to_enc *l = table,
					 *h = &table[num-1],
					 *m;
	unsigned short code_key;

	/* we have no mappings outside the BMP */
	if (code_key_a > 0xFFFFU)
		return 0;

	code_key = (unsigned short) code_key_a;

	while (l <= h) {
		m = l + (h - l) / 2;
		if (code_key < m->un_code_point)
			h = m - 1;
		else if (code_key > m->un_code_point)
			l = m + 1;
		else
			return m->cs_code;
	}
	return 0;
}
/* }}} */

/* {{{ map_from_unicode */
static inline int map_from_unicode(unsigned code, enum entity_charset charset, unsigned *res)
{
	unsigned char found;
	const uni_to_enc *table;
	size_t table_size;

	switch (charset) {
	case cs_8859_1:
		/* identity mapping of code points to unicode */
		if (code > 0xFF) {
			return FAILURE;
		}
		*res = code;
		break;

	case cs_8859_5:
		if (code <= 0xA0 || code == 0xAD /* soft hyphen */) {
			*res = code;
		} else if (code == 0x2116) {
			*res = 0xF0; /* numero sign */
		} else if (code == 0xA7) {
			*res = 0xFD; /* section sign */
		} else if (code >= 0x0401 && code <= 0x045F) {
			if (code == 0x040D || code == 0x0450 || code == 0x045D)
				return FAILURE;
			*res = code - 0x360;
		} else {
			return FAILURE;
		}
		break;

	case cs_8859_15:
		if (code < 0xA4 || (code > 0xBE && code <= 0xFF)) {
			*res = code;
		} else { /* between A4 and 0xBE */
			found = unimap_bsearch(unimap_iso885915,
				code, sizeof(unimap_iso885915) / sizeof(*unimap_iso885915));
			if (found)
				*res = found;
			else
				return FAILURE;
		}
		break;

	case cs_cp1252:
		if (code <= 0x7F || (code >= 0xA0 && code <= 0xFF)) {
			*res = code;
		} else {
			found = unimap_bsearch(unimap_win1252,
				code, sizeof(unimap_win1252) / sizeof(*unimap_win1252));
			if (found)
				*res = found;
			else
				return FAILURE;
		}
		break;

	case cs_macroman:
		if (code == 0x7F)
			return FAILURE;
		table = unimap_macroman;
		table_size = sizeof(unimap_macroman) / sizeof(*unimap_macroman);
		goto table_over_7F;
	case cs_cp1251:
		table = unimap_win1251;
		table_size = sizeof(unimap_win1251) / sizeof(*unimap_win1251);
		goto table_over_7F;
	case cs_koi8r:
		table = unimap_koi8r;
		table_size = sizeof(unimap_koi8r) / sizeof(*unimap_koi8r);
		goto table_over_7F;
	case cs_cp866:
		table = unimap_cp866;
		table_size = sizeof(unimap_cp866) / sizeof(*unimap_cp866);

table_over_7F:
		if (code <= 0x7F) {
			*res = code;
		} else {
			found = unimap_bsearch(table, code, table_size);
			if (found)
				*res = found;
			else
				return FAILURE;
		}
		break;

	/* from here on, only map the possible characters in the ASCII range.
	 * to improve support here, it's a matter of building the unicode mappings.
	 * See <http://www.unicode.org/Public/6.0.0/ucd/Unihan.zip> */
	case cs_sjis:
	case cs_eucjp:
		/* we interpret 0x5C as the Yen symbol. This is not universal.
		 * See <http://www.w3.org/Submission/japanese-xml/#ambiguity_of_yen> */
		if (code >= 0x20 && code <= 0x7D) {
			if (code == 0x5C)
				return FAILURE;
			*res = code;
		} else {
			return FAILURE;
		}
		break;

	case cs_big5:
	case cs_big5hkscs:
	case cs_gb2312:
		if (code >= 0x20 && code <= 0x7D) {
			*res = code;
		} else {
			return FAILURE;
		}
		break;

	default:
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

/* {{{ */
static inline void map_to_unicode(unsigned code, const enc_to_uni *table, unsigned *res)
{
	/* only single byte encodings are currently supported; assumed code <= 0xFF */
	*res = table->inner[ENT_ENC_TO_UNI_STAGE1(code)]->uni_cp[ENT_ENC_TO_UNI_STAGE2(code)];
}
/* }}} */

/* {{{ unicode_cp_is_allowed */
static inline int unicode_cp_is_allowed(unsigned uni_cp, int document_type)
{
	/* XML 1.0				HTML 4.01			HTML 5
	 * 0x09..0x0A			0x09..0x0A			0x09..0x0A
	 * 0x0D					0x0D				0x0C..0x0D
	 * 0x0020..0xD7FF		0x20..0x7E			0x20..0x7E
	 *						0x00A0..0xD7FF		0x00A0..0xD7FF
	 * 0xE000..0xFFFD		0xE000..0x10FFFF	0xE000..0xFDCF
	 * 0x010000..0x10FFFF						0xFDF0..0x10FFFF (*)
	 *
	 * (*) exclude code points where ((code & 0xFFFF) >= 0xFFFE)
	 *
	 * References:
	 * XML 1.0:   <http://www.w3.org/TR/REC-xml/#charsets>
	 * HTML 4.01: <http://www.w3.org/TR/1999/PR-html40-19990824/sgml/sgmldecl.html>
	 * HTML 5:    <http://dev.w3.org/html5/spec/Overview.html#preprocessing-the-input-stream>
	 *
	 * Not sure this is the relevant part for HTML 5, though. I opted to
	 * disallow the characters that would result in a parse error when
	 * preprocessing of the input stream. See also section 8.1.3.
	 *
	 * It's unclear if XHTML 1.0 allows C1 characters. I'll opt to apply to
	 * XHTML 1.0 the same rules as for XML 1.0.
	 * See <http://cmsmcq.com/2007/C1.xml>.
	 */

	switch (document_type) {
	case ENT_HTML_DOC_HTML401:
		return (uni_cp >= 0x20 && uni_cp <= 0x7E) ||
			(uni_cp == 0x0A || uni_cp == 0x09 || uni_cp == 0x0D) ||
			(uni_cp >= 0xA0 && uni_cp <= 0xD7FF) ||
			(uni_cp >= 0xE000 && uni_cp <= 0x10FFFF);
	case ENT_HTML_DOC_HTML5:
		return (uni_cp >= 0x20 && uni_cp <= 0x7E) ||
			(uni_cp >= 0x09 && uni_cp <= 0x0D && uni_cp != 0x0B) || /* form feed U+0C allowed */
			(uni_cp >= 0xA0 && uni_cp <= 0xD7FF) ||
			(uni_cp >= 0xE000 && uni_cp <= 0x10FFFF &&
				((uni_cp & 0xFFFF) < 0xFFFE) && /* last two of each plane (nonchars) disallowed */
				(uni_cp < 0xFDD0 || uni_cp > 0xFDEF)); /* U+FDD0-U+FDEF (nonchars) disallowed */
	case ENT_HTML_DOC_XHTML:
	case ENT_HTML_DOC_XML1:
		return (uni_cp >= 0x20 && uni_cp <= 0xD7FF) ||
			(uni_cp == 0x0A || uni_cp == 0x09 || uni_cp == 0x0D) ||
			(uni_cp >= 0xE000 && uni_cp <= 0x10FFFF && uni_cp != 0xFFFE && uni_cp != 0xFFFF);
	default:
		return 1;
	}
}
/* }}} */

/* {{{ unicode_cp_is_allowed */
static inline int numeric_entity_is_allowed(unsigned uni_cp, int document_type)
{
	/* less restrictive than unicode_cp_is_allowed */
	switch (document_type) {
	case ENT_HTML_DOC_HTML401:
		/* all non-SGML characters (those marked with UNUSED in DESCSET) should be
		 * representable with numeric entities */
		return uni_cp <= 0x10FFFF;
	case ENT_HTML_DOC_HTML5:
		/* 8.1.4. The numeric character reference forms described above are allowed to
		 * reference any Unicode code point other than U+0000, U+000D, permanently
		 * undefined Unicode characters (noncharacters), and control characters other
		 * than space characters (U+0009, U+000A, U+000C and U+000D) */
		/* seems to allow surrogate characters, then */
		return (uni_cp >= 0x20 && uni_cp <= 0x7E) ||
			(uni_cp >= 0x09 && uni_cp <= 0x0C && uni_cp != 0x0B) || /* form feed U+0C allowed, but not U+0D */
			(uni_cp >= 0xA0 && uni_cp <= 0x10FFFF &&
				((uni_cp & 0xFFFF) < 0xFFFE) && /* last two of each plane (nonchars) disallowed */
				(uni_cp < 0xFDD0 || uni_cp > 0xFDEF)); /* U+FDD0-U+FDEF (nonchars) disallowed */
	case ENT_HTML_DOC_XHTML:
	case ENT_HTML_DOC_XML1:
		/* OTOH, XML 1.0 requires "character references to match the production for Char
		 * See <http://www.w3.org/TR/REC-xml/#NT-CharRef> */
		return unicode_cp_is_allowed(uni_cp, document_type);
	default:
		return 1;
	}
}
/* }}} */

/* {{{ process_numeric_entity
 * Auxiliary function to traverse_for_entities.
 * On input, *buf should point to the first character after # and on output, it's the last
 * byte read, no matter if there was success or insuccess.
 */
static inline int process_numeric_entity(const char **buf, unsigned *code_point)
{
	zend_long code_l;
	int hexadecimal = (**buf == 'x' || **buf == 'X'); /* TODO: XML apparently disallows "X" */
	char *endptr;

	if (hexadecimal)
		(*buf)++;

	/* strtol allows whitespace and other stuff in the beginning
		* we're not interested */
	if ((hexadecimal && !isxdigit(**buf)) ||
			(!hexadecimal && !isdigit(**buf))) {
		return FAILURE;
	}

	code_l = ZEND_STRTOL(*buf, &endptr, hexadecimal ? 16 : 10);
	/* we're guaranteed there were valid digits, so *endptr > buf */
	*buf = endptr;

	if (**buf != ';')
		return FAILURE;

	/* many more are invalid, but that depends on whether it's HTML
	 * (and which version) or XML. */
	if (code_l > Z_L(0x10FFFF))
		return FAILURE;

	if (code_point != NULL)
		*code_point = (unsigned)code_l;

	return SUCCESS;
}
/* }}} */

/* {{{ process_named_entity */
static inline int process_named_entity_html(const char **buf, const char **start, size_t *length)
{
	*start = *buf;

	/* "&" is represented by a 0x26 in all supported encodings. That means
	 * the byte after represents a character or is the leading byte of a
	 * sequence of 8-bit code units. If in the ranges below, it represents
	 * necessarily an alpha character because none of the supported encodings
	 * has an overlap with ASCII in the leading byte (only on the second one) */
	while ((**buf >= 'a' && **buf <= 'z') ||
			(**buf >= 'A' && **buf <= 'Z') ||
			(**buf >= '0' && **buf <= '9')) {
		(*buf)++;
	}

	if (**buf != ';')
		return FAILURE;

	/* cast to size_t OK as the quantity is always non-negative */
	*length = *buf - *start;

	if (*length == 0)
		return FAILURE;

	return SUCCESS;
}
/* }}} */

/* {{{ resolve_named_entity_html */
static int resolve_named_entity_html(const char *start, size_t length, const entity_ht *ht, unsigned *uni_cp1, unsigned *uni_cp2)
{
	const entity_cp_map *s;
	zend_ulong hash = zend_inline_hash_func(start, length);

	s = ht->buckets[hash % ht->num_elems];
	while (s->entity) {
		if (s->entity_len == length) {
			if (memcmp(start, s->entity, length) == 0) {
				*uni_cp1 = s->codepoint1;
				*uni_cp2 = s->codepoint2;
				return SUCCESS;
			}
		}
		s++;
	}
	return FAILURE;
}
/* }}} */

static inline size_t write_octet_sequence(unsigned char *buf, enum entity_charset charset, unsigned code) {
	/* code is not necessarily a unicode code point */
	switch (charset) {
	case cs_utf_8:
		return php_utf32_utf8(buf, code);

	case cs_8859_1:
	case cs_cp1252:
	case cs_8859_15:
	case cs_koi8r:
	case cs_cp1251:
	case cs_8859_5:
	case cs_cp866:
	case cs_macroman:
		/* single byte stuff */
		*buf = code;
		return 1;

	case cs_big5:
	case cs_big5hkscs:
	case cs_sjis:
	case cs_gb2312:
		/* we don't have complete unicode mappings for these yet in entity_decode,
		 * and we opt to pass through the octet sequences for these in htmlentities
		 * instead of converting to an int and then converting back. */
#if 0
		return php_mb2_int_to_char(buf, code);
#else
#if ZEND_DEBUG
		assert(code <= 0xFFU);
#endif
		*buf = code;
		return 1;
#endif

	case cs_eucjp:
#if 0 /* idem */
		return php_mb2_int_to_char(buf, code);
#else
#if ZEND_DEBUG
		assert(code <= 0xFFU);
#endif
		*buf = code;
		return 1;
#endif

	default:
		assert(0);
		return 0;
	}
}

/* {{{ traverse_for_entities
 * Auxiliary function to php_unescape_html_entities().
 * - The argument "all" determines if all numeric entities are decode or only those
 *   that correspond to quotes (depending on quote_style).
 */
/* maximum expansion (factor 1.2) for HTML 5 with &nGt; and &nLt; */
/* +2 is 1 because of rest (probably unnecessary), 1 because of terminating 0 */
#define TRAVERSE_FOR_ENTITIES_EXPAND_SIZE(oldlen) ((oldlen) + (oldlen) / 5 + 2)
static void traverse_for_entities(
	const char *old,
	size_t oldlen,
	zend_string *ret, /* should have allocated TRAVERSE_FOR_ENTITIES_EXPAND_SIZE(olden) */
	int all,
	int flags,
	const entity_ht *inv_map,
	enum entity_charset charset)
{
	const char *p,
			   *lim;
	char	   *q;
	int doctype = flags & ENT_HTML_DOC_TYPE_MASK;

	lim = old + oldlen; /* terminator address */
	assert(*lim == '\0');

	for (p = old, q = ZSTR_VAL(ret); p < lim;) {
		unsigned code, code2 = 0;
		const char *next = NULL; /* when set, next > p, otherwise possible inf loop */

		/* Shift JIS, Big5 and HKSCS use multi-byte encodings where an
		 * ASCII range byte can be part of a multi-byte sequence.
		 * However, they start at 0x40, therefore if we find a 0x26 byte,
		 * we're sure it represents the '&' character. */

		/* assumes there are no single-char entities */
		if (p[0] != '&' || (p + 3 >= lim)) {
			*(q++) = *(p++);
			continue;
		}

		/* now p[3] is surely valid and is no terminator */

		/* numerical entity */
		if (p[1] == '#') {
			next = &p[2];
			if (process_numeric_entity(&next, &code) == FAILURE)
				goto invalid_code;

			/* If we're in htmlspecialchars_decode, we're only decoding entities
			 * that represent &, <, >, " and '. Is this one of them? */
			if (!all && (code > 63U ||
					stage3_table_be_apos_00000[code].data.ent.entity == NULL))
				goto invalid_code;

			/* are we allowed to decode this entity in this document type?
			 * HTML 5 is the only that has a character that cannot be used in
			 * a numeric entity but is allowed literally (U+000D). The
			 * unoptimized version would be ... || !numeric_entity_is_allowed(code) */
			if (!unicode_cp_is_allowed(code, doctype) ||
					(doctype == ENT_HTML_DOC_HTML5 && code == 0x0D))
				goto invalid_code;
		} else {
			const char *start;
			size_t ent_len;

			next = &p[1];
			start = next;

			if (process_named_entity_html(&next, &start, &ent_len) == FAILURE)
				goto invalid_code;

			if (resolve_named_entity_html(start, ent_len, inv_map, &code, &code2) == FAILURE) {
				if (doctype == ENT_HTML_DOC_XHTML && ent_len == 4 && start[0] == 'a'
							&& start[1] == 'p' && start[2] == 'o' && start[3] == 's') {
					/* uses html4 inv_map, which doesn't include apos;. This is a
					 * hack to support it */
					code = (unsigned) '\'';
				} else {
					goto invalid_code;
				}
			}
		}

		assert(*next == ';');

		if (((code == '\'' && !(flags & ENT_HTML_QUOTE_SINGLE)) ||
				(code == '"' && !(flags & ENT_HTML_QUOTE_DOUBLE)))
				/* && code2 == '\0' always true for current maps */)
			goto invalid_code;

		/* UTF-8 doesn't need mapping (ISO-8859-1 doesn't either, but
		 * the call is needed to ensure the codepoint <= U+00FF)  */
		if (charset != cs_utf_8) {
			/* replace unicode code point */
			if (map_from_unicode(code, charset, &code) == FAILURE || code2 != 0)
				goto invalid_code; /* not representable in target charset */
		}

		q += write_octet_sequence((unsigned char*)q, charset, code);
		if (code2) {
			q += write_octet_sequence((unsigned char*)q, charset, code2);
		}

		/* jump over the valid entity; may go beyond size of buffer; np */
		p = next + 1;
		continue;

invalid_code:
		for (; p < next; p++) {
			*(q++) = *p;
		}
	}

	*q = '\0';
	ZSTR_LEN(ret) = (size_t)(q - ZSTR_VAL(ret));
}
/* }}} */

/* {{{ unescape_inverse_map */
static const entity_ht *unescape_inverse_map(int all, int flags)
{
	int document_type = flags & ENT_HTML_DOC_TYPE_MASK;

	if (all) {
		switch (document_type) {
		case ENT_HTML_DOC_HTML401:
		case ENT_HTML_DOC_XHTML: /* but watch out for &apos;...*/
			return &ent_ht_html4;
		case ENT_HTML_DOC_HTML5:
			return &ent_ht_html5;
		default:
			return &ent_ht_be_apos;
		}
	} else {
		switch (document_type) {
		case ENT_HTML_DOC_HTML401:
			return &ent_ht_be_noapos;
		default:
			return &ent_ht_be_apos;
		}
	}
}
/* }}} */

/* {{{ determine_entity_table
 * Entity table to use. Note that entity tables are defined in terms of
 * unicode code points */
static entity_table_opt determine_entity_table(int all, int doctype)
{
	entity_table_opt retval = {0};

	assert(!(doctype == ENT_HTML_DOC_XML1 && all));

	if (all) {
		retval.ms_table = (doctype == ENT_HTML_DOC_HTML5) ?
			entity_ms_table_html5 : entity_ms_table_html4;
	} else {
		retval.table = (doctype == ENT_HTML_DOC_HTML401) ?
			stage3_table_be_noapos_00000 : stage3_table_be_apos_00000;
	}
	return retval;
}
/* }}} */

/* {{{ php_unescape_html_entities
 * The parameter "all" should be true to decode all possible entities, false to decode
 * only the basic ones, i.e., those in basic_entities_ex + the numeric entities
 * that correspond to quotes.
 */
PHPAPI zend_string *php_unescape_html_entities(zend_string *str, int all, int flags, const char *hint_charset)
{
	zend_string *ret;
	enum entity_charset charset;
	const entity_ht *inverse_map;
	size_t new_size;

	if (!memchr(ZSTR_VAL(str), '&', ZSTR_LEN(str))) {
		return zend_string_copy(str);
	}

	if (all) {
		charset = determine_charset(hint_charset, /* quiet */ 0);
	} else {
		charset = cs_8859_1; /* charset shouldn't matter, use ISO-8859-1 for performance */
	}

	/* don't use LIMIT_ALL! */

	new_size = TRAVERSE_FOR_ENTITIES_EXPAND_SIZE(ZSTR_LEN(str));
	if (ZSTR_LEN(str) > new_size) {
		/* overflow, refuse to do anything */
		return zend_string_copy(str);
	}

	ret = zend_string_alloc(new_size, 0);

	inverse_map = unescape_inverse_map(all, flags);

	/* replace numeric entities */
	traverse_for_entities(ZSTR_VAL(str), ZSTR_LEN(str), ret, all, flags, inverse_map, charset);

	return ret;
}
/* }}} */

PHPAPI zend_string *php_escape_html_entities(const unsigned char *old, size_t oldlen, int all, int flags, const char *hint_charset)
{
	return php_escape_html_entities_ex(old, oldlen, all, flags, hint_charset, 1, /* quiet */ 0);
}

/* {{{ find_entity_for_char */
static inline void find_entity_for_char(
	unsigned int k,
	enum entity_charset charset,
	const entity_stage1_row *table,
	const unsigned char **entity,
	size_t *entity_len,
	const unsigned char *old,
	size_t oldlen,
	size_t *cursor)
{
	unsigned stage1_idx = ENT_STAGE1_INDEX(k);
	const entity_stage3_row *c;

	if (stage1_idx > 0x1D) {
		*entity     = NULL;
		*entity_len = 0;
		return;
	}

	c = &table[stage1_idx][ENT_STAGE2_INDEX(k)][ENT_STAGE3_INDEX(k)];

	if (!c->ambiguous) {
		*entity     = (const unsigned char *)c->data.ent.entity;
		*entity_len = c->data.ent.entity_len;
	} else {
		/* peek at next char */
		size_t cursor_before = *cursor;
		zend_result status = SUCCESS;
		unsigned next_char;

		if (!(*cursor < oldlen))
			goto no_suitable_2nd;

		next_char = get_next_char(charset, old, oldlen, cursor, &status);

		if (status == FAILURE)
			goto no_suitable_2nd;

		{
			const entity_multicodepoint_row *s, *e;

			s = &c->data.multicodepoint_table[1];
			e = s - 1 + c->data.multicodepoint_table[0].leading_entry.size;
			/* we could do a binary search but it's not worth it since we have
			 * at most two entries... */
			for ( ; s <= e; s++) {
				if (s->normal_entry.second_cp == next_char) {
					*entity     = (const unsigned char *) s->normal_entry.entity;
					*entity_len = s->normal_entry.entity_len;
					return;
				}
			}
		}
no_suitable_2nd:
		*cursor = cursor_before;
		*entity = (const unsigned char *)
			c->data.multicodepoint_table[0].leading_entry.default_entity;
		*entity_len = c->data.multicodepoint_table[0].leading_entry.default_entity_len;
	}
}
/* }}} */

/* {{{ find_entity_for_char_basic */
static inline void find_entity_for_char_basic(
	unsigned int k,
	const entity_stage3_row *table,
	const unsigned char **entity,
	size_t *entity_len)
{
	if (k >= 64U) {
		*entity     = NULL;
		*entity_len = 0;
		return;
	}

	*entity     = (const unsigned char *) table[k].data.ent.entity;
	*entity_len = table[k].data.ent.entity_len;
}
/* }}} */

/* {{{ php_escape_html_entities */
PHPAPI zend_string *php_escape_html_entities_ex(const unsigned char *old, size_t oldlen, int all, int flags, const char *hint_charset, bool double_encode, bool quiet)
{
	size_t cursor, maxlen, len;
	zend_string *replaced;
	enum entity_charset charset = determine_charset(hint_charset, quiet);
	int doctype = flags & ENT_HTML_DOC_TYPE_MASK;
	entity_table_opt entity_table;
	const enc_to_uni *to_uni_table = NULL;
	const entity_ht *inv_map = NULL; /* used for !double_encode */
	/* only used if flags includes ENT_HTML_IGNORE_ERRORS or ENT_HTML_SUBSTITUTE_DISALLOWED_CHARS */
	const unsigned char *replacement = NULL;
	size_t replacement_len = 0;

	if (all) { /* replace with all named entities */
		if (!quiet && CHARSET_PARTIAL_SUPPORT(charset)) {
			php_error_docref(NULL, E_NOTICE, "Only basic entities "
				"substitution is supported for multi-byte encodings other than UTF-8; "
				"functionality is equivalent to htmlspecialchars");
		}
		LIMIT_ALL(all, doctype, charset);
	}
	entity_table = determine_entity_table(all, doctype);
	if (all && !CHARSET_UNICODE_COMPAT(charset)) {
		to_uni_table = enc_to_uni_index[charset];
	}

	if (!double_encode) {
		/* first arg is 1 because we want to identify valid named entities
		 * even if we are only encoding the basic ones */
		inv_map = unescape_inverse_map(1, flags);
	}

	if (flags & (ENT_HTML_SUBSTITUTE_ERRORS | ENT_HTML_SUBSTITUTE_DISALLOWED_CHARS)) {
		if (charset == cs_utf_8) {
			replacement = (const unsigned char*)"\xEF\xBF\xBD";
			replacement_len = sizeof("\xEF\xBF\xBD") - 1;
		} else {
			replacement = (const unsigned char*)"&#xFFFD;";
			replacement_len = sizeof("&#xFFFD;") - 1;
		}
	}

	/* initial estimate */
	if (oldlen < 64) {
		maxlen = 128;
	} else {
		maxlen = zend_safe_addmult(oldlen, 2, 0, "html_entities");
	}

	replaced = zend_string_alloc(maxlen, 0);
	len = 0;
	cursor = 0;
	while (cursor < oldlen) {
		const unsigned char *mbsequence = NULL;
		size_t mbseqlen					= 0,
		       cursor_before			= cursor;
		zend_result status				= SUCCESS;
		unsigned int this_char			= get_next_char(charset, old, oldlen, &cursor, &status);

		/* guarantee we have at least 40 bytes to write.
		 * In HTML5, entities may take up to 33 bytes */
		if (len > maxlen - 40) { /* maxlen can never be smaller than 128 */
			replaced = zend_string_safe_realloc(replaced, maxlen, 1, 128, 0);
			maxlen += 128;
		}

		if (status == FAILURE) {
			/* invalid MB sequence */
			if (flags & ENT_HTML_IGNORE_ERRORS) {
				continue;
			} else if (flags & ENT_HTML_SUBSTITUTE_ERRORS) {
				memcpy(&ZSTR_VAL(replaced)[len], replacement, replacement_len);
				len += replacement_len;
				continue;
			} else {
				zend_string_efree(replaced);
				return ZSTR_EMPTY_ALLOC();
			}
		} else { /* SUCCESS */
			mbsequence = &old[cursor_before];
			mbseqlen = cursor - cursor_before;
		}

		if (this_char != '&') { /* no entity on this position */
			const unsigned char *rep	= NULL;
			size_t				rep_len	= 0;

			if (((this_char == '\'' && !(flags & ENT_HTML_QUOTE_SINGLE)) ||
					(this_char == '"' && !(flags & ENT_HTML_QUOTE_DOUBLE))))
				goto pass_char_through;

			if (all) { /* false that CHARSET_PARTIAL_SUPPORT(charset) */
				if (to_uni_table != NULL) {
					/* !CHARSET_UNICODE_COMPAT therefore not UTF-8; since UTF-8
					 * is the only multibyte encoding with !CHARSET_PARTIAL_SUPPORT,
					 * we're using a single byte encoding */
					map_to_unicode(this_char, to_uni_table, &this_char);
					if (this_char == 0xFFFF) /* no mapping; pass through */
						goto pass_char_through;
				}
				/* the cursor may advance */
				find_entity_for_char(this_char, charset, entity_table.ms_table, &rep,
					&rep_len, old, oldlen, &cursor);
			} else {
				find_entity_for_char_basic(this_char, entity_table.table, &rep, &rep_len);
			}

			if (rep != NULL) {
				ZSTR_VAL(replaced)[len++] = '&';
				memcpy(&ZSTR_VAL(replaced)[len], rep, rep_len);
				len += rep_len;
				ZSTR_VAL(replaced)[len++] = ';';
			} else {
				/* we did not find an entity for this char.
				 * check for its validity, if its valid pass it unchanged */
				if (flags & ENT_HTML_SUBSTITUTE_DISALLOWED_CHARS) {
					if (CHARSET_UNICODE_COMPAT(charset)) {
						if (!unicode_cp_is_allowed(this_char, doctype)) {
							mbsequence = replacement;
							mbseqlen = replacement_len;
						}
					} else if (to_uni_table) {
						if (!all) /* otherwise we already did this */
							map_to_unicode(this_char, to_uni_table, &this_char);
						if (!unicode_cp_is_allowed(this_char, doctype)) {
							mbsequence = replacement;
							mbseqlen = replacement_len;
						}
					} else {
						/* not a unicode code point, unless, coincidentally, it's in
						 * the 0x20..0x7D range (except 0x5C in sjis). We know nothing
						 * about other code points, because we have no tables. Since
						 * Unicode code points in that range are not disallowed in any
						 * document type, we could do nothing. However, conversion
						 * tables frequently map 0x00-0x1F to the respective C0 code
						 * points. Let's play it safe and admit that's the case */
						if (this_char <= 0x7D &&
								!unicode_cp_is_allowed(this_char, doctype)) {
							mbsequence = replacement;
							mbseqlen = replacement_len;
						}
					}
				}
pass_char_through:
				if (mbseqlen > 1) {
					memcpy(ZSTR_VAL(replaced) + len, mbsequence, mbseqlen);
					len += mbseqlen;
				} else {
					ZSTR_VAL(replaced)[len++] = mbsequence[0];
				}
			}
		} else { /* this_char == '&' */
			if (double_encode) {
encode_amp:
				memcpy(&ZSTR_VAL(replaced)[len], "&amp;", sizeof("&amp;") - 1);
				len += sizeof("&amp;") - 1;
			} else { /* no double encode */
				/* check if entity is valid */
				size_t ent_len; /* not counting & or ; */
				/* peek at next char */
				if (old[cursor] == '#') { /* numeric entity */
					unsigned code_point;
					int valid;
					char *pos = (char*)&old[cursor+1];
					valid = process_numeric_entity((const char **)&pos, &code_point);
					if (valid == FAILURE)
						goto encode_amp;
					if (flags & ENT_HTML_SUBSTITUTE_DISALLOWED_CHARS) {
						if (!numeric_entity_is_allowed(code_point, doctype))
							goto encode_amp;
					}
					ent_len = pos - (char*)&old[cursor];
				} else { /* named entity */
					/* check for vality of named entity */
					const char *start = (const char *) &old[cursor],
							   *next = start;
					unsigned   dummy1, dummy2;

					if (process_named_entity_html(&next, &start, &ent_len) == FAILURE)
						goto encode_amp;
					if (resolve_named_entity_html(start, ent_len, inv_map, &dummy1, &dummy2) == FAILURE) {
						if (!(doctype == ENT_HTML_DOC_XHTML && ent_len == 4 && start[0] == 'a'
									&& start[1] == 'p' && start[2] == 'o' && start[3] == 's')) {
							/* uses html4 inv_map, which doesn't include apos;. This is a
							 * hack to support it */
							goto encode_amp;
						}
					}
				}
				/* checks passed; copy entity to result */
				/* entity size is unbounded, we may need more memory */
				/* at this point maxlen - len >= 40 */
				if (maxlen - len < ent_len + 2 /* & and ; */) {
					/* ent_len < oldlen, which is certainly <= SIZE_MAX/2 */
					replaced = zend_string_safe_realloc(replaced, maxlen, 1, ent_len + 128, 0);
					maxlen += ent_len + 128;
				}
				ZSTR_VAL(replaced)[len++] = '&';
				memcpy(&ZSTR_VAL(replaced)[len], &old[cursor], ent_len);
				len += ent_len;
				ZSTR_VAL(replaced)[len++] = ';';
				cursor += ent_len + 1;
			}
		}
	}
	ZSTR_VAL(replaced)[len] = '\0';
	ZSTR_LEN(replaced) = len;

	return replaced;
}
/* }}} */

/* {{{ php_html_entities */
static void php_html_entities(INTERNAL_FUNCTION_PARAMETERS, int all)
{
	zend_string *str, *hint_charset = NULL;
	zend_long flags = ENT_QUOTES|ENT_SUBSTITUTE;
	zend_string *replaced;
	bool double_encode = 1;

	ZEND_PARSE_PARAMETERS_START(1, 4)
		Z_PARAM_STR(str)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(flags)
		Z_PARAM_STR_OR_NULL(hint_charset)
		Z_PARAM_BOOL(double_encode);
	ZEND_PARSE_PARAMETERS_END();

	replaced = php_escape_html_entities_ex(
		(unsigned char*)ZSTR_VAL(str), ZSTR_LEN(str), all, (int) flags,
		hint_charset ? ZSTR_VAL(hint_charset) : NULL, double_encode, /* quiet */ 0);
	RETVAL_STR(replaced);
}
/* }}} */

/* {{{ register_html_constants */
void register_html_constants(INIT_FUNC_ARGS)
{
}
/* }}} */

/* {{{ Convert special characters to HTML entities */
PHP_FUNCTION(htmlspecialchars)
{
	php_html_entities(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ Convert special HTML entities back to characters */
PHP_FUNCTION(htmlspecialchars_decode)
{
	zend_string *str;
	zend_long quote_style = ENT_QUOTES|ENT_SUBSTITUTE;
	zend_string *replaced;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(str)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(quote_style)
	ZEND_PARSE_PARAMETERS_END();

	replaced = php_unescape_html_entities(str, 0 /*!all*/, (int)quote_style, NULL);
	RETURN_STR(replaced);
}
/* }}} */

/* {{{ Convert all HTML entities to their applicable characters */
PHP_FUNCTION(html_entity_decode)
{
	zend_string *str, *hint_charset = NULL;
	zend_long quote_style = ENT_QUOTES|ENT_SUBSTITUTE;
	zend_string *replaced;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STR(str)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(quote_style)
		Z_PARAM_STR_OR_NULL(hint_charset)
	ZEND_PARSE_PARAMETERS_END();

	replaced = php_unescape_html_entities(
		str, 1 /*all*/, (int)quote_style, hint_charset ? ZSTR_VAL(hint_charset) : NULL);
	RETURN_STR(replaced);
}
/* }}} */


/* {{{ Convert all applicable characters to HTML entities */
PHP_FUNCTION(htmlentities)
{
	php_html_entities(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ write_s3row_data */
static inline void write_s3row_data(
	const entity_stage3_row *r,
	unsigned orig_cp,
	enum entity_charset charset,
	zval *arr)
{
	char key[9] = ""; /* two unicode code points in UTF-8 */
	char entity[LONGEST_ENTITY_LENGTH + 2] = {'&'};
	size_t written_k1;

	written_k1 = write_octet_sequence((unsigned char*)key, charset, orig_cp);

	if (!r->ambiguous) {
		size_t l = r->data.ent.entity_len;
		memcpy(&entity[1], r->data.ent.entity, l);
		entity[l + 1] = ';';
		add_assoc_stringl_ex(arr, key, written_k1, entity, l + 2);
	} else {
		unsigned i,
			     num_entries;
		const entity_multicodepoint_row *mcpr = r->data.multicodepoint_table;

		if (mcpr[0].leading_entry.default_entity != NULL) {
			size_t l = mcpr[0].leading_entry.default_entity_len;
			memcpy(&entity[1], mcpr[0].leading_entry.default_entity, l);
			entity[l + 1] = ';';
			add_assoc_stringl_ex(arr, key, written_k1, entity, l + 2);
		}
		num_entries = mcpr[0].leading_entry.size;
		for (i = 1; i <= num_entries; i++) {
			size_t   l,
				     written_k2;
			unsigned uni_cp,
					 spe_cp;

			uni_cp = mcpr[i].normal_entry.second_cp;
			l = mcpr[i].normal_entry.entity_len;

			if (!CHARSET_UNICODE_COMPAT(charset)) {
				if (map_from_unicode(uni_cp, charset, &spe_cp) == FAILURE)
					continue; /* non representable in this charset */
			} else {
				spe_cp = uni_cp;
			}

			written_k2 = write_octet_sequence((unsigned char*)&key[written_k1], charset, spe_cp);
			memcpy(&entity[1], mcpr[i].normal_entry.entity, l);
			entity[l + 1] = ';';
			add_assoc_stringl_ex(arr, key, written_k1 + written_k2, entity, l + 2);
		}
	}
}
/* }}} */

/* {{{ Returns the internal translation table used by htmlspecialchars and htmlentities */
PHP_FUNCTION(get_html_translation_table)
{
	zend_long all = PHP_HTML_SPECIALCHARS,
		 flags = ENT_QUOTES|ENT_SUBSTITUTE;
	int doctype;
	entity_table_opt entity_table;
	const enc_to_uni *to_uni_table = NULL;
	char *charset_hint = NULL;
	size_t charset_hint_len;
	enum entity_charset charset;

	/* in this function we have to jump through some loops because we're
	 * getting the translated table from data structures that are optimized for
	 * random access, not traversal */

	ZEND_PARSE_PARAMETERS_START(0, 3)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(all)
		Z_PARAM_LONG(flags)
		Z_PARAM_STRING(charset_hint, charset_hint_len)
	ZEND_PARSE_PARAMETERS_END();

	charset = determine_charset(charset_hint, /* quiet */ 0);
	doctype = flags & ENT_HTML_DOC_TYPE_MASK;
	LIMIT_ALL(all, doctype, charset);

	array_init(return_value);

	entity_table = determine_entity_table((int)all, doctype);
	if (all && !CHARSET_UNICODE_COMPAT(charset)) {
		to_uni_table = enc_to_uni_index[charset];
	}

	if (all) { /* PHP_HTML_ENTITIES (actually, any non-zero value for 1st param) */
		const entity_stage1_row *ms_table = entity_table.ms_table;

		if (CHARSET_UNICODE_COMPAT(charset)) {
			unsigned i, j, k,
					 max_i, max_j, max_k;
			/* no mapping to unicode required */
			if (CHARSET_SINGLE_BYTE(charset)) { /* ISO-8859-1 */
				max_i = 1; max_j = 4; max_k = 64;
			} else {
				max_i = 0x1E; max_j = 64; max_k = 64;
			}

			for (i = 0; i < max_i; i++) {
				if (ms_table[i] == empty_stage2_table)
					continue;
				for (j = 0; j < max_j; j++) {
					if (ms_table[i][j] == empty_stage3_table)
						continue;
					for (k = 0; k < max_k; k++) {
						const entity_stage3_row *r = &ms_table[i][j][k];
						unsigned code;

						if (r->data.ent.entity == NULL)
							continue;

						code = ENT_CODE_POINT_FROM_STAGES(i, j, k);
						if (((code == '\'' && !(flags & ENT_HTML_QUOTE_SINGLE)) ||
								(code == '"' && !(flags & ENT_HTML_QUOTE_DOUBLE))))
							continue;
						write_s3row_data(r, code, charset, return_value);
					}
				}
			}
		} else {
			/* we have to iterate through the set of code points for this
			 * encoding and map them to unicode code points */
			unsigned i;
			for (i = 0; i <= 0xFF; i++) {
				const entity_stage3_row *r;
				unsigned uni_cp;

				/* can be done before mapping, they're invariant */
				if (((i == '\'' && !(flags & ENT_HTML_QUOTE_SINGLE)) ||
						(i == '"' && !(flags & ENT_HTML_QUOTE_DOUBLE))))
					continue;

				map_to_unicode(i, to_uni_table, &uni_cp);
				r = &ms_table[ENT_STAGE1_INDEX(uni_cp)][ENT_STAGE2_INDEX(uni_cp)][ENT_STAGE3_INDEX(uni_cp)];
				if (r->data.ent.entity == NULL)
					continue;

				write_s3row_data(r, i, charset, return_value);
			}
		}
	} else {
		/* we could use sizeof(stage3_table_be_apos_00000) as well */
		unsigned	  j,
					  numelems = sizeof(stage3_table_be_noapos_00000) /
							sizeof(*stage3_table_be_noapos_00000);

		for (j = 0; j < numelems; j++) {
			const entity_stage3_row *r = &entity_table.table[j];
			if (r->data.ent.entity == NULL)
				continue;

			if (((j == '\'' && !(flags & ENT_HTML_QUOTE_SINGLE)) ||
					(j == '"' && !(flags & ENT_HTML_QUOTE_DOUBLE))))
				continue;

			/* charset is indifferent, used cs_8859_1 for efficiency */
			write_s3row_data(r, j, cs_8859_1, return_value);
		}
	}
}
/* }}} */
