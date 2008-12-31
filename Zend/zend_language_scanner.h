/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2009 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_SCANNER_H
#define ZEND_SCANNER_H

typedef struct _zend_lex_state {
	YY_BUFFER_STATE buffer_state;
	int state;
	zend_file_handle *in;
	uint lineno;
	char *filename;

#ifdef ZEND_MULTIBYTE
	/* original (unfiltered) script */
	char *script_org;
	int script_org_size;

	/* filtered script */
	char *script_filtered;
	int script_filtered_size;

	/* input/ouput filters */
	zend_encoding_filter input_filter;
	zend_encoding_filter output_filter;
	zend_encoding *script_encoding;
	zend_encoding *internal_encoding;
#endif /* ZEND_MULTIBYTE */
} zend_lex_state;


void zend_fatal_scanner_error(char *);
BEGIN_EXTERN_C()
int zend_compare_file_handles(zend_file_handle *fh1, zend_file_handle *fh2);
ZEND_API void zend_save_lexical_state(zend_lex_state *lex_state TSRMLS_DC);
ZEND_API void zend_restore_lexical_state(zend_lex_state *lex_state TSRMLS_DC);
ZEND_API int zend_prepare_string_for_scanning(zval *str, char *filename TSRMLS_DC);

END_EXTERN_C()

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
