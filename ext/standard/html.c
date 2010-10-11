/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2010 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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

/* $Id$ */

/*
 * HTML entity resources:
 *
 * http://msdn.microsoft.com/workshop/author/dhtml/reference/charsets/charset2.asp
 * http://msdn.microsoft.com/workshop/author/dhtml/reference/charsets/charset3.asp
 * http://www.unicode.org/Public/MAPPINGS/OBSOLETE/UNI2SGML.TXT
 *
 * http://www.w3.org/TR/2002/REC-xhtml1-20020801/dtds.html#h-A2
 *
 * From HTML 4.01 strict DTD:
 * http://www.w3.org/TR/html4/HTMLlat1.ent
 * http://www.w3.org/TR/html4/HTMLsymbol.ent
 * http://www.w3.org/TR/html4/HTMLspecial.ent
 */

#include "php.h"
#if PHP_WIN32
#include "config.w32.h"
#else
#include <php_config.h>
#endif
#include "php_standard.h"
#include "php_string.h"
#include "SAPI.h"
#if HAVE_LOCALE_H
#include <locale.h>
#endif
#if HAVE_LANGINFO_H
#include <langinfo.h>
#endif

#if HAVE_MBSTRING
# include "ext/mbstring/mbstring.h"
ZEND_EXTERN_MODULE_GLOBALS(mbstring)
#endif

#include "html_tables.h"

#define MB_RETURN { \
			*newpos = pos;       \
		  	mbseq[mbpos] = '\0'; \
		  	*mbseqlen = mbpos;   \
		  	return this_char; }
					
#define MB_WRITE(mbchar) { \
			mbspace--;  \
			if (mbspace == 0) {      \
				MB_RETURN;           \
			}                        \
			mbseq[mbpos++] = (mbchar); }

/* skip one byte and return */
#define MB_FAILURE(pos) do { \
	*newpos = pos + 1; \
	*status = FAILURE; \
	return 0; \
} while (0)

#define CHECK_LEN(pos, chars_need)			\
	if (chars_need < 1) {						\
		if((str_len - (pos)) < chars_need) {	\
			*newpos = pos;						\
			*status = FAILURE;					\
			return 0;							\
		}										\
	} else {									\
		if((str_len - (pos)) < chars_need) {	\
			*newpos = pos + 1;					\
			*status = FAILURE;					\
			return 0;							\
		}										\
	}

/* {{{ get_next_char
 */
inline static unsigned int get_next_char(enum entity_charset charset,
		unsigned char * str,
		int str_len,
		int * newpos,
		unsigned char * mbseq,
		int * mbseqlen, 
		int *status)
{
	int pos = *newpos;
	int mbpos = 0;
	int mbspace = *mbseqlen;
	unsigned int this_char = 0;
	unsigned char next_char;

	*status = SUCCESS;

	if (mbspace <= 0) {
		*mbseqlen = 0;
		CHECK_LEN(pos, 1);
		*newpos = pos + 1;
		return str[pos];
	}

	switch (charset) {
		case cs_utf_8:
			{
				unsigned char c;
				CHECK_LEN(pos, 1);
				c = str[pos];
				if (c < 0x80) {
					MB_WRITE(c);
					this_char = c;
					pos++;
				} else if (c < 0xc0) {
					MB_FAILURE(pos);
				} else if (c < 0xe0) {
					CHECK_LEN(pos, 2);
					if (str[pos + 1] < 0x80 || str[pos + 1] > 0xbf) {
						MB_FAILURE(pos);
					}
					this_char = ((c & 0x1f) << 6) | (str[pos + 1] & 0x3f);
					if (this_char < 0x80) {
						MB_FAILURE(pos);
					}
					MB_WRITE((unsigned char)c);
					MB_WRITE((unsigned char)str[pos + 1]);
					pos += 2;
				} else if (c < 0xf0) {
					CHECK_LEN(pos, 3);
					if (str[pos + 1] < 0x80 || str[pos + 1] > 0xbf) {
						MB_FAILURE(pos);
					}
					if (str[pos + 2] < 0x80 || str[pos + 2] > 0xbf) {
						MB_FAILURE(pos);
					}
					this_char = ((c & 0x0f) << 12) | ((str[pos + 1] & 0x3f) << 6) | (str[pos + 2] & 0x3f);
					if (this_char < 0x800) {
						MB_FAILURE(pos);
					} else if (this_char >= 0xd800 && this_char <= 0xdfff) {
						MB_FAILURE(pos);
					}
					MB_WRITE((unsigned char)c);
					MB_WRITE((unsigned char)str[pos + 1]);
					MB_WRITE((unsigned char)str[pos + 2]);
					pos += 3;
				} else if (c < 0xf8) {
					CHECK_LEN(pos, 4);
					if (str[pos + 1] < 0x80 || str[pos + 1] > 0xbf) {
						MB_FAILURE(pos);
					}
					if (str[pos + 2] < 0x80 || str[pos + 2] > 0xbf) {
						MB_FAILURE(pos);
					}
					if (str[pos + 3] < 0x80 || str[pos + 3] > 0xbf) {
						MB_FAILURE(pos);
					}
					this_char = ((c & 0x07) << 18) | ((str[pos + 1] & 0x3f) << 12) | ((str[pos + 2] & 0x3f) << 6) | (str[pos + 3] & 0x3f);
					if (this_char < 0x10000) {
						MB_FAILURE(pos);
					}
					MB_WRITE((unsigned char)c);
					MB_WRITE((unsigned char)str[pos + 1]);
					MB_WRITE((unsigned char)str[pos + 2]);
					MB_WRITE((unsigned char)str[pos + 3]);
					pos += 4;
				} else {
					MB_FAILURE(pos);
				}
			}
			break;
		case cs_big5:
		case cs_gb2312:
		case cs_big5hkscs:
			{
				CHECK_LEN(pos, 1);
				this_char = str[pos++];
				/* check if this is the first of a 2-byte sequence */
				if (this_char >= 0x81 && this_char <= 0xfe) {
					/* peek at the next char */
					CHECK_LEN(pos, 1);
					next_char = str[pos++];
					if ((next_char >= 0x40 && next_char <= 0x7e) ||
							(next_char >= 0xa1 && next_char <= 0xfe)) {
						/* yes, this a wide char */
						MB_WRITE(this_char);
						MB_WRITE(next_char);
						this_char = (this_char << 8) | next_char;
					} else {
						MB_FAILURE(pos);
					}
				} else {
					MB_WRITE(this_char);
				}
			}
			break;
		case cs_sjis:
			{
				CHECK_LEN(pos, 1);
				this_char = str[pos++];
				/* check if this is the first of a 2-byte sequence */
				if ((this_char >= 0x81 && this_char <= 0x9f) ||
					(this_char >= 0xe0 && this_char <= 0xfc)) {
					/* peek at the next char */
					CHECK_LEN(pos, 1);
					next_char = str[pos++];
					if ((next_char >= 0x40 && next_char <= 0x7e) ||
						(next_char >= 0x80 && next_char <= 0xfc))
					{
						/* yes, this a wide char */
						MB_WRITE(this_char);
						MB_WRITE(next_char);
						this_char = (this_char << 8) | next_char;
					} else {
						MB_FAILURE(pos);
					}
				} else {
					MB_WRITE(this_char);
				}
				break;
			}
		case cs_eucjp:
			{
				CHECK_LEN(pos, 1);
				this_char = str[pos++];
				/* check if this is the first of a multi-byte sequence */
				if (this_char >= 0xa1 && this_char <= 0xfe) {
					/* peek at the next char */
					CHECK_LEN(pos, 1);
					next_char = str[pos++];
					if (next_char >= 0xa1 && next_char <= 0xfe) {
						/* yes, this a jis kanji char */
						MB_WRITE(this_char);
						MB_WRITE(next_char);
						this_char = (this_char << 8) | next_char;
					} else {
						MB_FAILURE(pos);
					}
				} else if (this_char == 0x8e) {
					/* peek at the next char */
					CHECK_LEN(pos, 1);
					next_char = str[pos++];
					if (next_char >= 0xa1 && next_char <= 0xdf) {
						/* JIS X 0201 kana */
						MB_WRITE(this_char);
						MB_WRITE(next_char);
						this_char = (this_char << 8) | next_char;
					} else {
						MB_FAILURE(pos);
					}
				} else if (this_char == 0x8f) {
					/* peek at the next two char */
					unsigned char next2_char;
					CHECK_LEN(pos, 2);
					next_char = str[pos];
					next2_char = str[pos + 1];
					pos += 2;
					if ((next_char >= 0xa1 && next_char <= 0xfe) &&
						(next2_char >= 0xa1 && next2_char <= 0xfe)) {
						/* JIS X 0212 hojo-kanji */
						MB_WRITE(this_char);
						MB_WRITE(next_char);
						MB_WRITE(next2_char);
						this_char = (this_char << 16) | (next_char << 8) | next2_char;
					} else {
						MB_FAILURE(pos);
					}
				} else {
					MB_WRITE(this_char);
				}
				break;
			}
		default:
			/* single-byte charsets */
			CHECK_LEN(pos, 1);
			this_char = str[pos++];
			MB_WRITE(this_char);
			break;
	}
	MB_RETURN;
}
/* }}} */

/* {{{ entity_charset determine_charset
 * returns the charset identifier based on current locale or a hint.
 * defaults to UTF-8 */
static enum entity_charset determine_charset(char *charset_hint TSRMLS_DC)
{
	int i;
	enum entity_charset charset = cs_utf_8;
	int len = 0;
	zval *uf_result = NULL;

	/* Default is now UTF-8 */
	if (charset_hint == NULL)
		return cs_utf_8;

	if ((len = strlen(charset_hint)) != 0) {
		goto det_charset;
	}
#if HAVE_MBSTRING
#if !defined(COMPILE_DL_MBSTRING)
	/* XXX: Ugly things. Why don't we look for a more sophisticated way? */
	switch (MBSTRG(current_internal_encoding)) {
		case mbfl_no_encoding_8859_1:
			return cs_8859_1;

		case mbfl_no_encoding_utf8:
			return cs_utf_8;

		case mbfl_no_encoding_euc_jp:
		case mbfl_no_encoding_eucjp_win:
			return cs_eucjp;

		case mbfl_no_encoding_sjis:
		case mbfl_no_encoding_sjis_open:
		case mbfl_no_encoding_cp932:
			return cs_sjis;

		case mbfl_no_encoding_cp1252:
			return cs_cp1252;

		case mbfl_no_encoding_8859_15:
			return cs_8859_15;

		case mbfl_no_encoding_big5:
			return cs_big5;

		case mbfl_no_encoding_euc_cn:
		case mbfl_no_encoding_hz:
		case mbfl_no_encoding_cp936:
			return cs_gb2312;

		case mbfl_no_encoding_koi8r:
			return cs_koi8r;

		case mbfl_no_encoding_cp866:
			return cs_cp866;

		case mbfl_no_encoding_cp1251:
			return cs_cp1251;

		case mbfl_no_encoding_8859_5:
			return cs_8859_5;

		default:
			;
	}
#else
	{
		zval nm_mb_internal_encoding;

		ZVAL_STRING(&nm_mb_internal_encoding, "mb_internal_encoding", 0);

		if (call_user_function_ex(CG(function_table), NULL, &nm_mb_internal_encoding, &uf_result, 0, NULL, 1, NULL TSRMLS_CC) != FAILURE) {

			charset_hint = Z_STRVAL_P(uf_result);
			len = Z_STRLEN_P(uf_result);
			
			if (len == 4) { /* sizeof(none|auto|pass)-1 */
				if (!memcmp("pass", charset_hint, sizeof("pass") - 1) || 
				    !memcmp("auto", charset_hint, sizeof("auto") - 1) || 
				    !memcmp("none", charset_hint, sizeof("none") - 1)) {
					
					charset_hint = NULL;
					len = 0;
				}
			}
			goto det_charset;
		}
	}
#endif
#endif

	charset_hint = SG(default_charset);
	if (charset_hint != NULL && (len=strlen(charset_hint)) != 0) {
		goto det_charset;
	}

	/* try to detect the charset for the locale */
#if HAVE_NL_LANGINFO && HAVE_LOCALE_H && defined(CODESET)
	charset_hint = nl_langinfo(CODESET);
	if (charset_hint != NULL && (len=strlen(charset_hint)) != 0) {
		goto det_charset;
	}
#endif

#if HAVE_LOCALE_H
	/* try to figure out the charset from the locale */
	{
		char *localename;
		char *dot, *at;

		/* lang[_territory][.codeset][@modifier] */
		localename = setlocale(LC_CTYPE, NULL);

		dot = strchr(localename, '.');
		if (dot) {
			dot++;
			/* locale specifies a codeset */
			at = strchr(dot, '@');
			if (at)
				len = at - dot;
			else
				len = strlen(dot);
			charset_hint = dot;
		} else {
			/* no explicit name; see if the name itself
			 * is the charset */
			charset_hint = localename;
			len = strlen(charset_hint);
		}
	}
#endif

det_charset:

	if (charset_hint) {
		int found = 0;
		
		/* now walk the charset map and look for the codeset */
		for (i = 0; charset_map[i].codeset; i++) {
			if (len == strlen(charset_map[i].codeset) && strncasecmp(charset_hint, charset_map[i].codeset, len) == 0) {
				charset = charset_map[i].charset;
				found = 1;
				break;
			}
		}
		if (!found) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "charset `%s' not supported, assuming utf-8",
					charset_hint);
		}
	}
	if (uf_result != NULL) {
		zval_ptr_dtor(&uf_result);
	}
	return charset;
}
/* }}} */

/* {{{ php_utf32_utf8 */
static size_t php_utf32_utf8(unsigned char *buf, unsigned k)
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

/* {{{ php_mb2_int_to_char
 * Convert back big endian int representation of sequence of one or two 8-bit code units. */
static size_t php_mb2_int_to_char(unsigned char *buf, unsigned k)
{
	assert(k <= 0xFFFFU);
	/* one or two bytes */
	if (k <= 0xFFU) { /* 1 */
		buf[0] = k;
		return 1U;
	} else { /* 2 */
		buf[0] = k >> 8;
		buf[1] = k & 0xFFU;
		return 2U;
	}
}
/* }}} */

/* {{{ php_mb3_int_to_char
 * Convert back big endian int representation of sequence of one to three 8-bit code units.
 * For EUC-JP. */
static size_t php_mb3_int_to_char(unsigned char *buf, unsigned k)
{
	assert(k <= 0xFFFFFFU);
	/* one to three bytes */
	if (k <= 0xFFU) { /* 1 */
		buf[0] = k;
		return 1U;
	} else if (k <= 0xFFFFU) { /* 2 */
		buf[0] = k >> 8;
		buf[1] = k & 0xFFU;
		return 2U;
	} else {
		buf[0] = k >> 16;
		buf[1] = (k >> 8) & 0xFFU;
		buf[2] = k & 0xFFU;
		return 3U;
	}
}
/* }}} */


/* {{{ unimap_bsearc_cmp
 * Binary search of unicode code points in unicode <--> charset mapping.
 * Returns the code point in the target charset (whose mapping table was given) or 0 if
 * the unicode code point is not in the table.
 */
static unsigned char unimap_bsearch(const unicode_mapping *table, unsigned code_key_a, size_t num)
{
	const unicode_mapping *l = table,
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
static int map_from_unicode(unsigned code, enum entity_charset charset, unsigned *res)
{
	unsigned char found;
	const unicode_mapping *table;
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
		} else if (code >= 0x0401 && code <= 0x044F) {
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
		if (code >= 0x20 && code <= 0x7D) {
			if (code == 0x5C) /* 0x5C is mapped to the yen symbol */
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

/* {{{ process_numeric_entity
 * Auxiliary function to traverse_for_entities.
 * On input, *buf should point to the first character after # and on output, it's the last
 * byte read, no matter if there was success or insuccess. 
 */
static int process_numeric_entity(char **buf, unsigned *code_point, int all)
{
	long code_l;
	int hexadecimal = (**buf == 'x' || **buf == 'X');

	if (hexadecimal)
		(*buf)++;
			
	/* strtol allows whitespace and other stuff in the beginning
		* we're not interested */
	if (hexadecimal && !isxdigit(**buf) ||
			!hexadecimal && !isdigit(**buf)) {
		return FAILURE;
	}

	code_l = strtol(*buf, buf, hexadecimal ? 16 : 10);

	if (**buf != ';')
		return FAILURE;

	/* many more are invalid, but that depends on whether it's HTML
		* (and which version) or XML. Rejecting 0 is handy because that's
		* the return of strtol if no character was read */
	if (code_l <= 0L || code_l > 0x10FFFFL)
		return FAILURE;
	
	*code_point = (unsigned)code_l;

	if (!all) {
		if (*code_point != '\'' && *code_point != '"')
			return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

/* {{{ process_named_entity */
static int process_named_entity(char **buf, unsigned *code_unit_seq, HashTable *inv_map)
{
	size_t length;
	char *start = *buf;
	unsigned *stored_code;

	/* "&" is represented by a 0x26 in all supported encodings. That means
	 * the byte after represents a character or is the leading byte of an
	 * sequence of 8-bit code units. If in the ranges below, it represents
	 * necessarily a alpha character because none of the supported encodings
	 * has an overlap with ASCII in the leading byte (only on the second one) */
	while (**buf >= 'a' && **buf <= 'z' ||
			**buf >= 'A' && **buf <= 'Z' ||
			**buf >= '0' && **buf <= '9') {
		(*buf)++;
	}

	if (**buf != ';')
		return FAILURE;

	/* cast to size_t OK as the quantity is always non-negative */
	length = *buf - start;
	if (length == 0 || length > 31) /* 31 is arbitrary */
		return FAILURE;

	if (zend_hash_find(inv_map, start, (uint)length, (void**)&stored_code) == FAILURE)
		return FAILURE;

	*code_unit_seq = *stored_code;

	return SUCCESS;
}
/* }}} */

/* {{{ traverse_for_entities
 * Auxiliary function to php_unescape_html_entities().
 * - The argument "all" determines if all numeric entities are decode or only those
 *   that correspond to quotes (depending on quote_style). Typically used with the inv_map
 *   stored under the key 0 in BG(inverse_ent_maps).
 * - Using cs_terminator as charset is legal and has the effect of defaulting to UTF-8. Used
 *   when the encoding doesn't (or shouldn't...) matter.
 */
static void traverse_for_entities(char *ret, int *retlen_p, int all, int quote_style, HashTable *inv_map, enum entity_charset charset)
{
	int retlen;
	char *p, *q, *lim;

	/* note: this function assumes the entities always take equal or more space
	 * than the characters they represent in whatever supported external encoding.
	 * The supported encoding that can generate the longest code unit sequences is
	 * UTF-8 (4 bytes). Theoretically, there could be entities with only 3 chars
	 * (e.g. &z;) that would map to outside-the-BMP unicode code points and hence
	 * needed 4 bytes and would overflow, but we have no such thing. */

	if (charset == cs_terminator) /* caller doesn't care; we choose one */
		charset = cs_utf_8;

	retlen = *retlen_p;

	lim = ret + retlen; /* terminator address */
	assert(*lim == '\0');

	for (p = ret, q = ret; p < lim;) {
		unsigned code;
		char *next = NULL;
		/* code is unicode code point or a set of 8-bit code units packed into
		 * an integer with the least significant bit being the last byte? */
		int unicode;

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
			if (process_numeric_entity(&next, &code, all) == FAILURE)
				goto invalid_code;
			unicode = 1;
		} else if (inv_map != NULL) {
			next = &p[1];
			if (process_named_entity(&next, &code, inv_map) == FAILURE)
				goto invalid_code;
			unicode = 0;
		} else {
			goto invalid_code;
		}
		
		assert(*next == ';');
		
		if (code == '\'' && !(quote_style & ENT_HTML_QUOTE_SINGLE) ||
				code == '"' && !(quote_style & ENT_HTML_QUOTE_DOUBLE))
			goto invalid_code;

		if (unicode && charset != cs_utf_8) {
			/* replace unicode code point */
			if (map_from_unicode(code, charset, &code) == FAILURE)
				goto invalid_code; /* not representable in target charset */
		}
		
		switch (charset) {
		case cs_utf_8:
			{
				size_t written;
				written = php_utf32_utf8((unsigned char*)q, code);
				q += written;
				/* Since we're writing in place, we hope we didn't write more than we read */
				assert(written <= (size_t)(next - p) + 1);
				break;
			}

		case cs_8859_1:
		case cs_cp1252:
		case cs_8859_15:
		case cs_koi8r:
		case cs_cp1251:
		case cs_8859_5:
		case cs_cp866:
		case cs_macroman:
			/* single byte stuff */
			*(q++) = code;
			break;

		case cs_big5:
		case cs_big5hkscs:
		case cs_sjis:
		case cs_gb2312:
			/* we don't have named entity or unicode mappings for these yet,
			 * so we're guaranteed code <= 0xFF */
#if 0
			q += php_mb2_int_to_char((unsigned char*)q, code);
#else
			assert(code <= 0xFFU);
			*(q++) = code;
#endif
			break;

		case cs_eucjp:
#if 0 /* idem */
			q += php_mb2_int_to_char((unsigned char*)q, code);
#else
			assert(code <= 0xFFU);
			*(q++) = code;
#endif
			break;

		default:
			/* for backwards compatilibity */
			goto invalid_code;
			break;
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
	*retlen_p = (size_t)(q - ret);
}
/* }}} */

/* {{{ inv_ent_maps_dtor
 * Hash table destructor for BG(inverse_ent_maps)
 */
static void inv_ent_maps_dtor(HashTable **ht) {
	zend_hash_destroy(*ht);
	pefree(*ht, 1);
}
/* }}} */

/* {{{ unescape_inverse_map
 *     Auxiliary function to php_unescape_html_entities()
 * charset can be cs_terminator for only basic entities.
 */
static HashTable *unescape_inverse_map(enum entity_charset charset TSRMLS_DC)
{
	HashTable **inverse_map;

	/* we accept charset = cs_terminator (for specialchars) */

	if (!BG(inverse_ent_maps)) {
		BG(inverse_ent_maps) = pemalloc(sizeof *BG(inverse_ent_maps), 1);
		zend_hash_init(BG(inverse_ent_maps), cs_numelems, NULL, (dtor_func_t)inv_ent_maps_dtor, 1);
	}
	if (zend_hash_index_find(BG(inverse_ent_maps), (ulong)charset, (void**)&inverse_map) == FAILURE) {
		HashTable *ht = pemalloc(sizeof *ht, 1);
		uint capacity = 0;
		int j, t;

		/* determine upper bound for capacity of hashtable */
		for (j = 0; entity_map[j].charset != cs_terminator; j++) {
			if (entity_map[j].charset == charset)
				capacity += entity_map[j].endchar - entity_map[j].basechar + 1;
		}

		/* no destructor as we'll be storing ints */
		zend_hash_init(ht, capacity, NULL, NULL, 1);

		/* store new hash table */
		t = zend_hash_index_update(BG(inverse_ent_maps), (ulong)charset, &ht, sizeof(ht), (void**)&inverse_map);
		assert(t == SUCCESS);

		/* build inverse map */
		for (j = 0; entity_map[j].charset != cs_terminator; j++) {
			unsigned k;

			if (entity_map[j].charset != charset)
				continue;

			for (k = entity_map[j].basechar; k <= entity_map[j].endchar; k++) {
				unsigned table_offset   = k - entity_map[j].basechar;
				const char* entity_name = entity_map[j].table[table_offset];

				if (entity_name == NULL || *entity_name == '#')
					continue;

				t = zend_hash_update(ht, entity_name, strlen(entity_name), &k, sizeof(k), NULL);
				assert(t == SUCCESS);
			}
		}

		/* and add the basic entitites */
		for (j = 0; basic_entities_ex[j].charcode != 0; j++) {
			const basic_entity_t *ent = &basic_entities_ex[j];
			unsigned k = ent->charcode;
		
			t = zend_hash_update(ht, &ent->entity[1] /* skip & */,
				ent->entitylen - 2 /* skip & and ; */, &k, sizeof(k), NULL);
			assert(t == SUCCESS);
		}
	}

	return *inverse_map;
}

/* {{{ php_unescape_html_entities
 * The parameter "all" should be true to decode all possible entities, false to decode
 * only the basic ones, i.e., those in basic_entities_ex + the numeric entities
 * that correspond to quotes.
 */
PHPAPI char *php_unescape_html_entities(unsigned char *old, int oldlen, int *newlen, int all, int quote_style, char *hint_charset TSRMLS_DC)
{
	int retlen;
	char *ret;
	enum entity_charset charset;
	HashTable *inverse_map = NULL;

	if (all) {
		charset = determine_charset(hint_charset TSRMLS_CC);
	} else {
		charset = cs_terminator;
	}

	ret = estrndup(old, oldlen);
	retlen = oldlen;
	if (retlen == 0) {
		goto empty_source;
	}

	/* charset == cs_terminator if !all */
	inverse_map = unescape_inverse_map(charset TSRMLS_CC);
	
	/* replace numeric entities */
	/* !all implies charset == cs_terminator && inverse_map == BG(inverse_ent_maps)[0] */
	traverse_for_entities(ret, &retlen, all, quote_style, inverse_map, charset);

empty_source:	
	*newlen = retlen;
	return ret;
}
/* }}} */

PHPAPI char *php_escape_html_entities(unsigned char *old, int oldlen, int *newlen, int all, int quote_style, char *hint_charset TSRMLS_DC)
{
	return php_escape_html_entities_ex(old, oldlen, newlen, all, quote_style, hint_charset, 1 TSRMLS_CC);
}


/* {{{ php_escape_html_entities
 */
PHPAPI char *php_escape_html_entities_ex(unsigned char *old, int oldlen, int *newlen, int all, int quote_style, char *hint_charset, zend_bool double_encode TSRMLS_DC)
{
	int i, j, maxlen, len;
	char *replaced;
	enum entity_charset charset = determine_charset(hint_charset TSRMLS_CC);
	int matches_map;

	maxlen = 2 * oldlen;
	if (maxlen < 128)
		maxlen = 128;
	replaced = emalloc (maxlen);
	len = 0;
	i = 0;
	while (i < oldlen) {
		unsigned char mbsequence[16];	/* allow up to 15 characters in a multibyte sequence */
		int mbseqlen = sizeof(mbsequence);
		int status = SUCCESS;
		unsigned int this_char = get_next_char(charset, old, oldlen, &i, mbsequence, &mbseqlen, &status);

		if(status == FAILURE) {
			/* invalid MB sequence */
			if (quote_style & ENT_HTML_IGNORE_ERRORS) {
				continue;
			}
			efree(replaced);
			if(!PG(display_errors)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid multibyte sequence in argument");
			}
			*newlen = 0;
			return STR_EMPTY_ALLOC();
		}
		matches_map = 0;

		if (len + 16 > maxlen)
			replaced = erealloc (replaced, maxlen += 128);

		if (all) {
			/* look for a match in the maps for this charset */
			unsigned char *rep = NULL;


			for (j = 0; entity_map[j].charset != cs_terminator; j++) {
				if (entity_map[j].charset == charset
						&& this_char >= entity_map[j].basechar
						&& this_char <= entity_map[j].endchar) {
					rep = (unsigned char*)entity_map[j].table[this_char - entity_map[j].basechar];
					if (rep == NULL) {
						/* there is no entity for this position; fall through and
						 * just output the character itself */
						break;
					}

					matches_map = 1;
					break;
				}
			}

			if (matches_map) {
				int l = strlen(rep);
				/* increase the buffer size */
				if (len + 2 + l >= maxlen) {
					replaced = erealloc(replaced, maxlen += 128);
				}

				replaced[len++] = '&';
				strlcpy(replaced + len, rep, maxlen);
				len += l;
				replaced[len++] = ';';
			}
		}
		if (!matches_map) {	
			int is_basic = 0;

			if (this_char == '&') {
				if (double_encode) {
encode_amp:
					memcpy(replaced + len, "&amp;", sizeof("&amp;") - 1);
					len += sizeof("&amp;") - 1;
				} else {
					char *e = memchr(old + i, ';', oldlen - i);
					char *s = old + i;

					if (!e || (e - s) > 10) { /* minor optimization to avoid "entities" over 10 chars in length */
						goto encode_amp;
					} else {
						if (*s == '#') { /* numeric entities */
							s++;
							/* Hex (&#x5A;) */
							if (*s == 'x' || *s == 'X') {
								s++;
								while (s < e) {
									if (!isxdigit((int)*(unsigned char *)s++)) {
										goto encode_amp;
									}
								}
							/* Dec (&#90;)*/
							} else {
								while (s < e) {
									if (!isdigit((int)*(unsigned char *)s++)) {
										goto encode_amp;
									}
								}
							}
						} else { /* text entities */
							while (s < e) {
								if (!isalnum((int)*(unsigned char *)s++)) {
									goto encode_amp;
								}
							}
						}
						replaced[len++] = '&';
					}
				}
				is_basic = 1;
			} else {
				for (j = 0; basic_entities[j].charcode != 0; j++) {
					if ((basic_entities[j].charcode != this_char) ||
							(basic_entities[j].flags &&
							(quote_style & basic_entities[j].flags) == 0)) {
						continue;
					}

					memcpy(replaced + len, basic_entities[j].entity, basic_entities[j].entitylen);
					len += basic_entities[j].entitylen;
		
					is_basic = 1;
					break;
				}
			}

			if (!is_basic) {
				/* a wide char without a named entity; pass through the original sequence */
				if (mbseqlen > 1) {
					memcpy(replaced + len, mbsequence, mbseqlen);
					len += mbseqlen;
				} else {
					replaced[len++] = (unsigned char)this_char;
				}
			}
		}
	}
	replaced[len] = '\0';
	*newlen = len;

	return replaced;


}
/* }}} */

/* {{{ php_html_entities
 */
static void php_html_entities(INTERNAL_FUNCTION_PARAMETERS, int all)
{
	char *str, *hint_charset = NULL;
	int str_len, hint_charset_len = 0;
	int len;
	long quote_style = ENT_COMPAT;
	char *replaced;
	zend_bool double_encode = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ls!b", &str, &str_len, &quote_style, &hint_charset, &hint_charset_len, &double_encode) == FAILURE) {
		return;
	}

	replaced = php_escape_html_entities_ex(str, str_len, &len, all, quote_style, hint_charset, double_encode TSRMLS_CC);
	RETVAL_STRINGL(replaced, len, 0);
}
/* }}} */

#define HTML_SPECIALCHARS 	0
#define HTML_ENTITIES	 	1

/* {{{ register_html_constants
 */
void register_html_constants(INIT_FUNC_ARGS)
{
	REGISTER_LONG_CONSTANT("HTML_SPECIALCHARS", HTML_SPECIALCHARS, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("HTML_ENTITIES", HTML_ENTITIES, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("ENT_COMPAT", ENT_COMPAT, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("ENT_QUOTES", ENT_QUOTES, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("ENT_NOQUOTES", ENT_NOQUOTES, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("ENT_IGNORE", ENT_IGNORE, CONST_PERSISTENT|CONST_CS);
}
/* }}} */

/* {{{ proto string htmlspecialchars(string string [, int quote_style[, string charset[, bool double_encode]]])
   Convert special characters to HTML entities */
PHP_FUNCTION(htmlspecialchars)
{
	php_html_entities(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto string htmlspecialchars_decode(string string [, int quote_style])
   Convert special HTML entities back to characters */
PHP_FUNCTION(htmlspecialchars_decode)
{
	char *str;
	int str_len, len;
	long quote_style = ENT_COMPAT;
	char *replaced;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &str, &str_len, &quote_style) == FAILURE) {
		return;
	}

	replaced = php_unescape_html_entities(str, str_len, &len, 0 /*!all*/, quote_style, NULL TSRMLS_CC);
	if (replaced) {
		RETURN_STRINGL(replaced, len, 0);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string html_entity_decode(string string [, int quote_style][, string charset])
   Convert all HTML entities to their applicable characters */
PHP_FUNCTION(html_entity_decode)
{
	char *str, *hint_charset = NULL;
	int str_len, hint_charset_len = 0, len;
	long quote_style = ENT_COMPAT;
	char *replaced;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ls", &str, &str_len,
							  &quote_style, &hint_charset, &hint_charset_len) == FAILURE) {
		return;
	}

	replaced = php_unescape_html_entities(str, str_len, &len, 1 /*all*/, quote_style, hint_charset TSRMLS_CC);
	if (replaced) {
		RETURN_STRINGL(replaced, len, 0);
	}
	RETURN_FALSE;
}
/* }}} */


/* {{{ proto string htmlentities(string string [, int quote_style[, string charset[, bool double_encode]]])
   Convert all applicable characters to HTML entities */
PHP_FUNCTION(htmlentities)
{
	php_html_entities(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto array get_html_translation_table([int table [, int quote_style]])
   Returns the internal translation table used by htmlspecialchars and htmlentities */
PHP_FUNCTION(get_html_translation_table)
{
	long which = HTML_SPECIALCHARS, quote_style = ENT_COMPAT;
	unsigned int i;
	int j;
	char ind[2];
	enum entity_charset charset = determine_charset(NULL TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|ll", &which, &quote_style) == FAILURE) {
		return;
	}

	array_init(return_value);

	ind[1] = 0;

	switch (which) {
		case HTML_ENTITIES:
			for (j=0; entity_map[j].charset != cs_terminator; j++) {
				if (entity_map[j].charset != charset)
					continue;
				for (i = 0; i <= entity_map[j].endchar - entity_map[j].basechar; i++) {
					char buffer[16];

					if (entity_map[j].table[i] == NULL)
						continue;
					/* what about wide chars here ?? */
					ind[0] = i + entity_map[j].basechar;
					snprintf(buffer, sizeof(buffer), "&%s;", entity_map[j].table[i]);
					add_assoc_string(return_value, ind, buffer, 1);

				}
			}
			/* break thru */

		case HTML_SPECIALCHARS:
			for (j = 0; basic_entities_ex[j].charcode != 0; j++) {
				void *dummy;

				if (basic_entities_ex[j].flags && (quote_style & basic_entities_ex[j].flags) == 0)
					continue;
				
				ind[0] = (unsigned char)basic_entities_ex[j].charcode;
				if (zend_hash_find(Z_ARRVAL_P(return_value), ind, sizeof(ind), &dummy) == FAILURE) {
					/* in case of the single quote, which is repeated, the first one wins,
					 * so don't replace the existint mapping */
					add_assoc_stringl(return_value, ind, basic_entities_ex[j].entity,
						basic_entities_ex[j].entitylen, 1);
				}
			}

			break;
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
