#ifndef _ZEND_INI_SCANNER_H
#define _ZEND_INI_SCANNER_H

#if defined(ZTS) && defined(__cplusplus)
class ZendIniFlexLexer : public yyFlexLexer
{
public:
	virtual ~ZendIniFlexLexer();
	int lex_scan(zval *zendlval);
	void BeginState(int state);
};
#endif	/* ZTS && __cplusplus */

int zend_ini_scanner_get_lineno();

#endif /* _ZEND_INI_SCANNER_H */