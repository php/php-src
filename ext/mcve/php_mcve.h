/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2007 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Brad House <brad@mainstreetsoftworks.com>                   |
   |          Chris Faulhaber <jedgar@fxp.org>                            |
   +----------------------------------------------------------------------+
*/
/* $Id$ */

#ifndef _PHP_MCVE_H
#define _PHP_MCVE_H

extern zend_module_entry mcve_module_entry;

#define mcve_module_ptr &mcve_module_entry
#define phpext_mcve_ptr mcve_module_ptr

#define PHP_MCVE_VERSION	"4.0"

#define MCVE_CONST (CONST_CS | CONST_PERSISTENT)

#define M_map_function(newname, oldname) \
		PHP_FUNCTION(oldname) { \
			PHP_FN(newname)(INTERNAL_FUNCTION_PARAM_PASSTHRU); \
		} 

PHP_MINIT_FUNCTION(mcve);
PHP_MINFO_FUNCTION(mcve);

PHP_FUNCTION(m_initengine);
PHP_FUNCTION(m_initconn);
PHP_FUNCTION(m_deleteresponse);
PHP_FUNCTION(m_destroyconn);
PHP_FUNCTION(m_setdropfile);
PHP_FUNCTION(m_setip);
PHP_FUNCTION(m_setssl);
PHP_FUNCTION(m_setssl_files);
PHP_FUNCTION(m_setblocking);
PHP_FUNCTION(m_settimeout);
PHP_FUNCTION(m_verifyconnection);
PHP_FUNCTION(m_verifysslcert);
PHP_FUNCTION(m_maxconntimeout);
PHP_FUNCTION(m_connectionerror);
PHP_FUNCTION(m_deletetrans);
PHP_FUNCTION(m_connect);
PHP_FUNCTION(m_transnew);
PHP_FUNCTION(m_transparam);
PHP_FUNCTION(m_transsend);
PHP_FUNCTION(m_ping);
PHP_FUNCTION(m_responseparam);
PHP_FUNCTION(m_returnstatus);
PHP_FUNCTION(m_returncode);
PHP_FUNCTION(m_transactionssent);
PHP_FUNCTION(m_transactionitem);
PHP_FUNCTION(m_transactionbatch);
PHP_FUNCTION(m_transactionid);
PHP_FUNCTION(m_transactionauth);
PHP_FUNCTION(m_transactionavs);
PHP_FUNCTION(m_transactioncv);
PHP_FUNCTION(m_transactiontext);
PHP_FUNCTION(m_getuserparam);
PHP_FUNCTION(m_monitor);
PHP_FUNCTION(m_transinqueue);
PHP_FUNCTION(m_checkstatus);
PHP_FUNCTION(m_completeauthorizations);
PHP_FUNCTION(m_sale);
PHP_FUNCTION(m_preauth);
PHP_FUNCTION(m_override);
PHP_FUNCTION(m_void);
PHP_FUNCTION(m_preauthcompletion);
PHP_FUNCTION(m_force);
PHP_FUNCTION(m_return);
PHP_FUNCTION(m_iscommadelimited);
PHP_FUNCTION(m_parsecommadelimited);
PHP_FUNCTION(m_getcommadelimited);
PHP_FUNCTION(m_getcell);
PHP_FUNCTION(m_getcellbynum);
PHP_FUNCTION(m_numcolumns);
PHP_FUNCTION(m_numrows);
PHP_FUNCTION(m_getheader);
PHP_FUNCTION(m_destroyengine);
PHP_FUNCTION(m_settle);
PHP_FUNCTION(m_qc);
PHP_FUNCTION(m_gut);
PHP_FUNCTION(m_gft);
PHP_FUNCTION(m_ub);
PHP_FUNCTION(m_gl);
PHP_FUNCTION(m_chkpwd);
PHP_FUNCTION(m_bt);
PHP_FUNCTION(m_uwait);
PHP_FUNCTION(m_text_code);
PHP_FUNCTION(m_text_avs);
PHP_FUNCTION(m_text_cv);
PHP_FUNCTION(m_chngpwd);
PHP_FUNCTION(m_listusers);
PHP_FUNCTION(m_adduser);
PHP_FUNCTION(m_enableuser);
PHP_FUNCTION(m_disableuser);
PHP_FUNCTION(m_getuserarg);
PHP_FUNCTION(m_adduserarg);
PHP_FUNCTION(m_deleteusersetup);
PHP_FUNCTION(m_initusersetup);
PHP_FUNCTION(m_deluser);
PHP_FUNCTION(m_edituser);
PHP_FUNCTION(m_liststats);

/* prototypes for compatability functions */
PHP_FUNCTION(mcve_initengine);
PHP_FUNCTION(mcve_initconn);
PHP_FUNCTION(mcve_deleteresponse);
PHP_FUNCTION(mcve_destroyconn);
PHP_FUNCTION(mcve_setdropfile);
PHP_FUNCTION(mcve_setip);
PHP_FUNCTION(mcve_setssl);
PHP_FUNCTION(mcve_setssl_files);
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
