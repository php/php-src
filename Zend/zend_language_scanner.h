/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2001 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 0.92 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/0_92.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#ifndef ZEND_SCANNER_H
#define ZEND_SCANNER_H

#ifdef __cplusplus
class ZendFlexLexer : public yyFlexLexer
{
public:
	virtual ~ZendFlexLexer();
	int lex_scan(zval *zendlval TSRMLS_DC);
	void BeginState(int state);
};

#endif	/* ZTS */


typedef struct _zend_lex_state {
#ifndef __cplusplus
	YY_BUFFER_STATE buffer_state;
	int state;
	FILE *in;
#else
	ZendFlexLexer *ZFL;
	istream *input_file;
#endif
	uint lineno;
	char *filename;
} zend_lex_state;


void zend_fatal_scanner_error(char *);
BEGIN_EXTERN_C()
int zend_compare_file_handles(zend_file_handle *fh1, zend_file_handle *fh2);
END_EXTERN_C()

#endif
