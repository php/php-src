#ifndef _ZEND_INI_SCANNER_H
#define _ZEND_INI_SCANNER_H

#if defined(ZTS) && defined(__cplusplus)
class ZendIniFlexLexer : public yyFlexLexer
{
public:
	virtual ~ZendIniFlexLexer();
	int lex_scan(zval *zendlval);
	void BeginState(int state);

	char *filename;
};
#endif	/* ZTS && __cplusplus */

BEGIN_EXTERN_C()
int zend_ini_scanner_get_lineno(void);
char *zend_ini_scanner_get_filename(void);
int zend_ini_open_file_for_scanning(zend_file_handle *fh);
void zend_ini_close_file(zend_file_handle *fh);
int ini_lex(zval *ini_lval);
END_EXTERN_C()

#endif /* _ZEND_INI_SCANNER_H */
