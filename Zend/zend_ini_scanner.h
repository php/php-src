#ifndef _ZEND_INI_SCANNER_H
#define _ZEND_INI_SCANNER_H

BEGIN_EXTERN_C()
int zend_ini_scanner_get_lineno(TSRMLS_D);
char *zend_ini_scanner_get_filename(TSRMLS_D);
int zend_ini_open_file_for_scanning(zend_file_handle *fh TSRMLS_DC);
void zend_ini_close_file(zend_file_handle *fh TSRMLS_DC);
int ini_lex(zval *ini_lval TSRMLS_DC);
END_EXTERN_C()

#endif /* _ZEND_INI_SCANNER_H */
