/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available through the world-wide-web at                              |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Masaki Fujimoto <fujimoto@php.net>                          |
   |          Rui Hirokawa <hirokawa@php.net>                             |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_MULTIBYTE_H
#define ZEND_MULTIBYTE_H

#ifdef ZEND_MULTIBYTE

#define BOM_UTF32_BE	"\x00\x00\xfe\xff"
#define	BOM_UTF32_LE	"\xff\xfe\x00\x00"
#define	BOM_UTF16_BE	"\xfe\xff"
#define	BOM_UTF16_LE	"\xff\xfe"
#define	BOM_UTF8		"\xef\xbb\xbf"

typedef size_t (*zend_encoding_filter)(unsigned char **str, size_t *str_length, const unsigned char *buf, size_t length TSRMLS_DC);

typedef char* (*zend_encoding_detector)(const unsigned char *string, size_t length, char *list TSRMLS_DC);

typedef int (*zend_encoding_converter)(unsigned char **to, size_t *to_length, const unsigned char *from, size_t from_length, const char *encoding_to, const char *encoding_from TSRMLS_DC);

typedef size_t (*zend_encoding_oddlen)(const unsigned char *string, size_t length, const char *encoding TSRMLS_DC);

typedef struct _zend_encoding {
	zend_encoding_filter input_filter;		/* escape input filter */
	zend_encoding_filter output_filter;		/* escape output filter */
	const char *name;					/* encoding name */
	const char *(*aliases)[];			/* encoding name aliases */
	zend_bool compatible;						/* flex compatible or not */
} zend_encoding;


/*
 * zend multibyte APIs
 */
BEGIN_EXTERN_C()
ZEND_API int zend_multibyte_set_script_encoding(const char *encoding_list,
size_t encoding_list_size TSRMLS_DC);
ZEND_API int zend_multibyte_set_internal_encoding(const char *encoding_name TSRMLS_DC);
ZEND_API int zend_multibyte_set_functions(zend_encoding_detector encoding_detector, zend_encoding_converter encoding_converter, zend_encoding_oddlen encoding_oddlen TSRMLS_DC);
ZEND_API int zend_multibyte_set_filter(zend_encoding *onetime_encoding TSRMLS_DC);
ZEND_API zend_encoding* zend_multibyte_fetch_encoding(const char *encoding_name);
ZEND_API size_t zend_multibyte_script_encoding_filter(unsigned char **to, size_t 
*to_length, const unsigned char *from, size_t from_length TSRMLS_DC);
ZEND_API size_t zend_multibyte_internal_encoding_filter(unsigned char **to, size_t *to_length, const unsigned char *from, size_t from_length TSRMLS_DC);

/* in zend_language_scanner.l */
ZEND_API void zend_multibyte_yyinput_again(zend_encoding_filter old_input_filter, zend_encoding *old_encoding TSRMLS_DC);
ZEND_API int zend_multibyte_yyinput(zend_file_handle *file_handle, char *buf, size_t len TSRMLS_DC);
ZEND_API int zend_multibyte_read_script(unsigned char *buf, size_t n TSRMLS_DC);
END_EXTERN_C()

#endif /* ZEND_MULTIBYTE */

#endif /* ZEND_MULTIBYTE_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 tw=78
 * vim<600: sw=4 ts=4 tw=78
 */
