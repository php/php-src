/*
 * basic mcve php module
 *
 */

#ifndef _PHP_MCVE_H
#define _PHP_MCVE_H

extern zend_module_entry php_mcve_module_entry;

#define mcve_module_ptr &php_mcve_module_entry
#define phpext_mcve_ptr mcve_module_ptr

//#if COMPILE_DL
//  DLEXPORT zend_module_entry *get_module(void) { return
//&php_mcve_module_entry; }
//#endif


#define PHP_MCVE_VERSION	"3.0"

#define MCVE_CONST (CONST_CS | CONST_PERSISTENT)

PHP_MINIT_FUNCTION(mcve);
PHP_MINFO_FUNCTION(mcve);

PHP_FUNCTION(mcve_initengine);
PHP_FUNCTION(mcve_initconn);
PHP_FUNCTION(mcve_deleteresponse);
PHP_FUNCTION(mcve_destroyconn);
PHP_FUNCTION(mcve_setdropfile);
PHP_FUNCTION(mcve_setip);
PHP_FUNCTION(mcve_setssl);
PHP_FUNCTION(mcve_setblocking);
PHP_FUNCTION(mcve_settimeout);
PHP_FUNCTION(mcve_verifyconnection);
PHP_FUNCTION(mcve_verifysslcert);
PHP_FUNCTION(mcve_maxconntimeout);
PHP_FUNCTION(mcve_connectionerror);
PHP_FUNCTION(mcve_deletetrans);
PHP_FUNCTION(mcve_connect);
PHP_FUNCTION(mcve_transnew);
PHP_FUNCTION(mcve_transparam);
PHP_FUNCTION(mcve_transsend);
PHP_FUNCTION(mcve_ping);
PHP_FUNCTION(mcve_responseparam);
PHP_FUNCTION(mcve_returnstatus);
PHP_FUNCTION(mcve_returncode);
PHP_FUNCTION(mcve_transactionssent);
PHP_FUNCTION(mcve_transactionitem);
PHP_FUNCTION(mcve_transactionbatch);
PHP_FUNCTION(mcve_transactionid);
PHP_FUNCTION(mcve_transactionauth);
PHP_FUNCTION(mcve_transactionavs);
PHP_FUNCTION(mcve_transactioncv);
PHP_FUNCTION(mcve_transactiontext);
PHP_FUNCTION(mcve_getuserparam);
PHP_FUNCTION(mcve_monitor);
PHP_FUNCTION(mcve_transinqueue);
PHP_FUNCTION(mcve_checkstatus);
PHP_FUNCTION(mcve_completeauthorizations);
PHP_FUNCTION(mcve_sale);
PHP_FUNCTION(mcve_preauth);
PHP_FUNCTION(mcve_override);
PHP_FUNCTION(mcve_void);
PHP_FUNCTION(mcve_preauthcompletion);
PHP_FUNCTION(mcve_force);
PHP_FUNCTION(mcve_return);
PHP_FUNCTION(mcve_iscommadelimited);
PHP_FUNCTION(mcve_parsecommadelimited);
PHP_FUNCTION(mcve_getcommadelimited);
PHP_FUNCTION(mcve_getcell);
PHP_FUNCTION(mcve_getcellbynum);
PHP_FUNCTION(mcve_numcolumns);
PHP_FUNCTION(mcve_numrows);
PHP_FUNCTION(mcve_getheader);
PHP_FUNCTION(mcve_destroyengine);
PHP_FUNCTION(mcve_settle);
PHP_FUNCTION(mcve_qc);
PHP_FUNCTION(mcve_gut);
PHP_FUNCTION(mcve_gft);
PHP_FUNCTION(mcve_ub);
PHP_FUNCTION(mcve_gl);
PHP_FUNCTION(mcve_chkpwd);
PHP_FUNCTION(mcve_bt);
PHP_FUNCTION(mcve_uwait);
PHP_FUNCTION(mcve_text_code);
PHP_FUNCTION(mcve_text_avs);
PHP_FUNCTION(mcve_text_cv);
PHP_FUNCTION(mcve_chngpwd);
PHP_FUNCTION(mcve_listusers);
PHP_FUNCTION(mcve_adduser);
PHP_FUNCTION(mcve_enableuser);
PHP_FUNCTION(mcve_disableuser);
PHP_FUNCTION(mcve_getuserarg);
PHP_FUNCTION(mcve_adduserarg);
PHP_FUNCTION(mcve_deleteusersetup);
PHP_FUNCTION(mcve_initusersetup);
PHP_FUNCTION(mcve_deluser);
PHP_FUNCTION(mcve_edituser);
PHP_FUNCTION(mcve_liststats);
#endif /* _PHP_MCVE_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
