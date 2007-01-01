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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if PHP_WIN32
#include "config.w32.h"
#else
#include <php_config.h>
#endif

#if HAVE_MCVE

/* standard php include(s) */
#include "ext/standard/head.h"
#include "ext/standard/php_standard.h"
#include "ext/standard/info.h"

/* local include(s) */
#include <mcve.h>

/* local php include(s) */
#include "php_mcve.h"

/* {{{ globals */
static int le_conn;	/* connection resource */
static int le_user;  /* store add/edit/get user information */

static int mcve_init;  /* For Safe Memory Deallocation */
/* }}} */

/* {{{ extension definition structures */
static unsigned char second_args_force_ref[] = { 2, BYREF_NONE, BYREF_FORCE };
function_entry mcve_functions[] = {
	PHP_FE(m_initengine,			NULL)
	PHP_FE(m_initconn,			NULL)
	PHP_FE(m_deleteresponse,		NULL)
	PHP_FE(m_destroyconn,			NULL)
	PHP_FE(m_setdropfile,			NULL)
	PHP_FE(m_setip,				NULL)
	PHP_FE(m_setssl,			NULL)
	PHP_FE(m_setssl_files,			NULL)
	PHP_FE(m_settimeout,			NULL)
	PHP_FE(m_setblocking,			NULL)
	PHP_FE(m_verifyconnection,		NULL)
	PHP_FE(m_verifysslcert,			NULL)
	PHP_FE(m_maxconntimeout,		NULL)
	PHP_FE(m_connectionerror,		NULL)
	PHP_FE(m_deletetrans,			NULL)
	PHP_FE(m_connect,			NULL)
	PHP_FE(m_transnew,			NULL)
	PHP_FE(m_transparam,			NULL)
	PHP_FE(m_transsend,			NULL)
	PHP_FE(m_ping,				NULL)
	PHP_FE(m_responseparam,			NULL)
	PHP_FE(m_returnstatus,			NULL)
	PHP_FE(m_returncode,			NULL)
	PHP_FE(m_transactionssent,		NULL)
	PHP_FE(m_transactionitem,		NULL)
	PHP_FE(m_transactionbatch,		NULL)
	PHP_FE(m_transactionid,			NULL)
	PHP_FE(m_transactionauth,		NULL)
	PHP_FE(m_transactiontext,		NULL)
	PHP_FE(m_transactionavs,		NULL)
	PHP_FE(m_transactioncv,			NULL)
	PHP_FE(m_getuserparam,			NULL)
	PHP_FE(m_monitor,			NULL)
	PHP_FE(m_transinqueue,			NULL)
	PHP_FE(m_checkstatus,			NULL)
	PHP_FE(m_completeauthorizations,	second_arg_force_ref)
	PHP_FE(m_sale,				NULL)
	PHP_FE(m_preauth,			NULL)
	PHP_FE(m_void,				NULL)
	PHP_FE(m_preauthcompletion,		NULL)
	PHP_FE(m_force,				NULL)
	PHP_FE(m_override,			NULL)
	PHP_FE(m_return,			NULL)
	PHP_FE(m_iscommadelimited,		NULL)
	PHP_FE(m_parsecommadelimited,		NULL)
	PHP_FE(m_getcommadelimited,		NULL)
	PHP_FE(m_getcell,			NULL)
	PHP_FE(m_getcellbynum,			NULL)
	PHP_FE(m_numcolumns,			NULL)
	PHP_FE(m_numrows,			NULL)
	PHP_FE(m_getheader,			NULL)
	PHP_FE(m_destroyengine,			NULL)
	PHP_FE(m_settle,			NULL)
	PHP_FE(m_gut,				NULL)
	PHP_FE(m_gl,				NULL)
	PHP_FE(m_gft,				NULL)
	PHP_FE(m_qc,				NULL)
	PHP_FE(m_ub,				NULL)
	PHP_FE(m_chkpwd,			NULL)
	PHP_FE(m_bt,				NULL)
	PHP_FE(m_uwait,				NULL)
	PHP_FE(m_text_code,			NULL)
	PHP_FE(m_text_avs,			NULL)
	PHP_FE(m_text_cv,			NULL)
	PHP_FE(m_chngpwd,			NULL)
	PHP_FE(m_listusers,			NULL)
	PHP_FE(m_adduser,			NULL)
	PHP_FE(m_enableuser,			NULL)
	PHP_FE(m_disableuser,			NULL)
	PHP_FE(m_getuserarg,			NULL)
	PHP_FE(m_adduserarg,			NULL)
	PHP_FE(m_deleteusersetup,		NULL)
	PHP_FE(m_initusersetup,			NULL)
	PHP_FE(m_deluser,			NULL)
	PHP_FE(m_edituser,			NULL)
	PHP_FE(m_liststats,			NULL)
	/* Backwards compatability functions below */
	PHP_FE(mcve_initengine,			NULL)
	PHP_FE(mcve_initconn,			NULL)
	PHP_FE(mcve_deleteresponse,		NULL)
	PHP_FE(mcve_destroyconn,		NULL)
	PHP_FE(mcve_setdropfile,		NULL)
	PHP_FE(mcve_setip,			NULL)
	PHP_FE(mcve_setssl,			NULL)
	PHP_FE(mcve_setssl_files,		NULL)
	PHP_FE(mcve_settimeout,			NULL)
	PHP_FE(mcve_setblocking,		NULL)
	PHP_FE(mcve_verifyconnection,		NULL)
	PHP_FE(mcve_verifysslcert,		NULL)
	PHP_FE(mcve_maxconntimeout,		NULL)
	PHP_FE(mcve_connectionerror,		NULL)
	PHP_FE(mcve_deletetrans,		NULL)
	PHP_FE(mcve_connect,			NULL)
	PHP_FE(mcve_transnew,			NULL)
	PHP_FE(mcve_transparam,			NULL)
	PHP_FE(mcve_transsend,			NULL)
	PHP_FE(mcve_ping,			NULL)
	PHP_FE(mcve_responseparam,		NULL)
	PHP_FE(mcve_returnstatus,		NULL)
	PHP_FE(mcve_returncode,			NULL)
	PHP_FE(mcve_transactionssent,		NULL)
	PHP_FE(mcve_transactionitem,		NULL)
	PHP_FE(mcve_transactionbatch,		NULL)
	PHP_FE(mcve_transactionid,		NULL)
	PHP_FE(mcve_transactionauth,		NULL)
	PHP_FE(mcve_transactiontext,		NULL)
	PHP_FE(mcve_transactionavs,		NULL)
	PHP_FE(mcve_transactioncv,		NULL)
	PHP_FE(mcve_getuserparam,		NULL)
	PHP_FE(mcve_monitor,			NULL)
	PHP_FE(mcve_transinqueue,		NULL)
	PHP_FE(mcve_checkstatus,		NULL)
	PHP_FE(mcve_completeauthorizations,	second_args_force_ref)
	PHP_FE(mcve_sale,			NULL)
	PHP_FE(mcve_preauth,			NULL)
	PHP_FE(mcve_void,			NULL)
	PHP_FE(mcve_preauthcompletion,		NULL)
	PHP_FE(mcve_force,			NULL)
	PHP_FE(mcve_override,			NULL)
	PHP_FE(mcve_return,			NULL)
	PHP_FE(mcve_iscommadelimited,		NULL)
	PHP_FE(mcve_parsecommadelimited,	NULL)
	PHP_FE(mcve_getcommadelimited,		NULL)
	PHP_FE(mcve_getcell,			NULL)
	PHP_FE(mcve_getcellbynum,		NULL)
	PHP_FE(mcve_numcolumns,			NULL)
	PHP_FE(mcve_numrows,			NULL)
	PHP_FE(mcve_getheader,			NULL)
	PHP_FE(mcve_destroyengine,		NULL)
	PHP_FE(mcve_settle,			NULL)
	PHP_FE(mcve_gut,			NULL)
	PHP_FE(mcve_gl,				NULL)
	PHP_FE(mcve_gft,			NULL)
	PHP_FE(mcve_qc,				NULL)
	PHP_FE(mcve_ub,				NULL)
	PHP_FE(mcve_chkpwd,			NULL)
	PHP_FE(mcve_bt,				NULL)
	PHP_FE(mcve_uwait,			NULL)
	PHP_FE(mcve_text_code,			NULL)
	PHP_FE(mcve_text_avs,			NULL)
	PHP_FE(mcve_text_cv,			NULL)
	PHP_FE(mcve_chngpwd,			NULL)
	PHP_FE(mcve_listusers,			NULL)
	PHP_FE(mcve_adduser,			NULL)
	PHP_FE(mcve_enableuser,			NULL)
	PHP_FE(mcve_disableuser,		NULL)
	PHP_FE(mcve_getuserarg,			NULL)
	PHP_FE(mcve_adduserarg,			NULL)
	PHP_FE(mcve_deleteusersetup,		NULL)
	PHP_FE(mcve_initusersetup,		NULL)
	PHP_FE(mcve_deluser,			NULL)
	PHP_FE(mcve_edituser,			NULL)
	PHP_FE(mcve_liststats,			NULL)
	{ NULL, NULL, NULL }
};

zend_module_entry mcve_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"mcve",			/* module name */
	mcve_functions,	/* struct of functions (see above) */
	PHP_MINIT(mcve),	/* module initialization functions */
	NULL,			/* module shutdown functions */
	NULL,			/* request initialization functions */
	NULL,			/* request shutdown functions */
	PHP_MINFO(mcve),	/* declare the info function for phpinfo() */
#if ZEND_MODULE_API_NO >= 20010901
	PHP_MCVE_VERSION,  /* declare MCVE version number */
#endif
	STANDARD_MODULE_PROPERTIES
};

/* }}} */

/* declare the module for dynamic loading */
#ifdef COMPILE_DL_MCVE
ZEND_GET_MODULE(mcve)
#endif

/* {{{ MCVE_CONN destructor */
static void _free_mcve_conn(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	MCVE_CONN *conn;

	conn = (MCVE_CONN *)rsrc->ptr;
	MCVE_DestroyConn(conn);  
	efree(conn);
}
/* }}} */

/* {{{ module initialization */
PHP_MINIT_FUNCTION(mcve)
{
	/* register the MCVE_CONN destructor */
	le_conn = zend_register_list_destructors_ex(_free_mcve_conn, NULL, "mcve connection", module_number);

	/* Key definitions for Transaction Parameters */
	REGISTER_LONG_CONSTANT("MC_TRANTYPE", MC_TRANTYPE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USERNAME", MC_USERNAME, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_PASSWORD", MC_PASSWORD, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_ACCOUNT", MC_ACCOUNT, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRACKDATA", MC_TRACKDATA, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_EXPDATE", MC_EXPDATE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_STREET", MC_STREET, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_ZIP", MC_ZIP, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_CV", MC_CV, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_COMMENTS", MC_COMMENTS, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_CLERKID", MC_CLERKID, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_STATIONID", MC_STATIONID, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_APPRCODE", MC_APPRCODE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_AMOUNT", MC_AMOUNT, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_PTRANNUM", MC_PTRANNUM, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TTID", MC_TTID, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER", MC_USER, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_PWD", MC_PWD, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_ACCT", MC_ACCT, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_BDATE", MC_BDATE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_EDATE", MC_EDATE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_BATCH", MC_BATCH, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_FILE", MC_FILE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_ADMIN", MC_ADMIN, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_AUDITTYPE", MC_AUDITTYPE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_CUSTOM", MC_CUSTOM, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_EXAMOUNT", MC_EXAMOUNT, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_EXCHARGES", MC_EXCHARGES, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_RATE", MC_RATE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_RENTERNAME", MC_RENTERNAME, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_RETURNCITY", MC_RETURNCITY, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_RETURNSTATE", MC_RETURNSTATE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_RETURNLOCATION", MC_RETURNLOCATION, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_PRIORITY", MC_PRIORITY, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_INQUIRY", MC_INQUIRY, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_CARDTYPES", MC_CARDTYPES, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_SUB", MC_SUB, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_MARKER", MC_MARKER, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_DEVICETYPE", MC_DEVICETYPE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_ERRORCODE", MC_ERRORCODE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_NEWBATCH", MC_NEWBATCH, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_CURR", MC_CURR, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_DESCMERCH", MC_DESCMERCH, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_DESCLOC", MC_DESCLOC, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_ORIGTYPE", MC_ORIGTYPE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_PIN", MC_PIN, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_VOIDORIGTYPE", MC_VOIDORIGTYPE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TIMESTAMP", MC_TIMESTAMP, MCVE_CONST);

	/* Args for priorities */
	REGISTER_LONG_CONSTANT("MC_PRIO_HIGH", MC_PRIO_HIGH, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_PRIO_NORMAL", MC_PRIO_NORMAL, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_PRIO_LOW", MC_PRIO_LOW, MCVE_CONST);

	/* Args for adding a user */
	REGISTER_LONG_CONSTANT("MC_USER_PROC", MC_USER_PROC, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_USER", MC_USER_USER, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_PWD", MC_USER_PWD, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_INDCODE", MC_USER_INDCODE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_MERCHID", MC_USER_MERCHID, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_BANKID", MC_USER_BANKID, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_TERMID", MC_USER_TERMID, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_CLIENTNUM", MC_USER_CLIENTNUM, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_STOREID", MC_USER_STOREID, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_AGENTID", MC_USER_AGENTID, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_CHAINID", MC_USER_CHAINID, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_ZIPCODE", MC_USER_ZIPCODE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_TIMEZONE", MC_USER_TIMEZONE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_MERCHCAT", MC_USER_MERCHCAT, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_MERNAME", MC_USER_MERNAME, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_MERCHLOC", MC_USER_MERCHLOC, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_STATECODE", MC_USER_STATECODE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_PHONE", MC_USER_PHONE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_SUB", MC_USER_SUB, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_CARDTYPES", MC_USER_CARDTYPES, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_MODE", MC_USER_MODE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_VNUMBER", MC_USER_VNUMBER, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_ROUTINGID", MC_USER_ROUTINGID, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_PPROPERTY", MC_USER_PPROPERTY, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_PID", MC_USER_PID, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_PIDPWD", MC_USER_PIDPWD, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_SMID", MC_USER_SMID, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_SMIDPWD", MC_USER_SMIDPWD, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_USDDIV", MC_USER_USDDIV, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_AUDDIV", MC_USER_AUDDIV, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_DKKDIV", MC_USER_DKKDIV, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_GBPDIV", MC_USER_GBPDIV, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_HKDDIV", MC_USER_HKDDIV, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_JPYDIV", MC_USER_JPYDIV, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_NZDDIV", MC_USER_NZDDIV, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_NOKDIV", MC_USER_NOKDIV, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_SGDDIV", MC_USER_SGDDIV, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_ZARDIV", MC_USER_ZARDIV, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_SEKDIV", MC_USER_SEKDIV, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_CHFDIV", MC_USER_CHFDIV, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_CADDIV", MC_USER_CADDIV, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_USER_DIVNUM", MC_USER_DIVNUM, MCVE_CONST);

        /* Values for cardtypes */
	REGISTER_LONG_CONSTANT("MC_CARD_VISA", MC_CARD_VISA, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_CARD_MC", MC_CARD_MC, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_CARD_AMEX", MC_CARD_AMEX, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_CARD_DISC", MC_CARD_DISC, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_CARD_JCB", MC_CARD_JCB, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_CARD_CB", MC_CARD_CB, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_CARD_DC", MC_CARD_DC, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_CARD_GIFT", MC_CARD_GIFT, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_CARD_OTHER", MC_CARD_OTHER, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_CARD_ALL", MC_CARD_ALL, MCVE_CONST);

	/* Values for modes */
	REGISTER_LONG_CONSTANT("MC_MODE_AUTH", MC_MODE_AUTH, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_MODE_SETTLE", MC_MODE_SETTLE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_MODE_BOTH", MC_MODE_BOTH, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_MODE_ALL", MC_MODE_ALL, MCVE_CONST);

	/* Values for EXCHARGES */
	REGISTER_LONG_CONSTANT("MC_EXCHARGES_REST", MC_EXCHARGES_REST, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_EXCHARGES_GIFT", MC_EXCHARGES_GIFT, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_EXCHARGES_MINI", MC_EXCHARGES_MINI, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_EXCHARGES_TELE", MC_EXCHARGES_TELE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_EXCHARGES_OTHER", MC_EXCHARGES_OTHER, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_EXCHARGES_LAUND", MC_EXCHARGES_LAUND, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_EXCHARGES_NONE", MC_EXCHARGES_NONE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_EXCHARGES_GAS", MC_EXCHARGES_GAS, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_EXCHARGES_MILE", MC_EXCHARGES_MILE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_EXCHARGES_LATE", MC_EXCHARGES_LATE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_EXCHARGES_1WAY", MC_EXCHARGES_1WAY, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_EXCHARGES_VIOL", MC_EXCHARGES_VIOL, MCVE_CONST);

	/* Value definitions for Transaction Types */
	REGISTER_LONG_CONSTANT("MC_TRAN_SALE", MC_TRAN_SALE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_REDEMPTION", MC_TRAN_REDEMPTION, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_PREAUTH", MC_TRAN_PREAUTH, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_VOID", MC_TRAN_VOID, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_PREAUTHCOMPLETE", MC_TRAN_PREAUTHCOMPLETE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_FORCE", MC_TRAN_FORCE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_OVERRIDE", MC_TRAN_OVERRIDE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_RETURN", MC_TRAN_RETURN, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_RELOAD", MC_TRAN_RELOAD, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_CREDIT", MC_TRAN_CREDIT, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_SETTLE", MC_TRAN_SETTLE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_INCREMENTAL", MC_TRAN_INCREMENTAL, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_REVERSAL", MC_TRAN_REVERSAL, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_ACTIVATE", MC_TRAN_ACTIVATE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_BALANCEINQ", MC_TRAN_BALANCEINQ, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_CASHOUT", MC_TRAN_CASHOUT, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_TOREVERSAL", MC_TRAN_TOREVERSAL, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_SETTLERFR", MC_TRAN_SETTLERFR, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_ISSUE", MC_TRAN_ISSUE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_TIP", MC_TRAN_TIP, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_MERCHRETURN", MC_TRAN_MERCHRETURN, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_IVRREQ", MC_TRAN_IVRREQ, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_IVRRESP", MC_TRAN_IVRRESP, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_ADMIN", MC_TRAN_ADMIN, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_PING", MC_TRAN_PING, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_CHKPWD", MC_TRAN_CHKPWD, MCVE_CONST);

	/* Engine Admin Transaction Types */
	REGISTER_LONG_CONSTANT("MC_TRAN_CHNGPWD", MC_TRAN_CHNGPWD, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_LISTSTATS", MC_TRAN_LISTSTATS, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_LISTUSERS", MC_TRAN_LISTUSERS, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_GETUSERINFO", MC_TRAN_GETUSERINFO, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_ADDUSER", MC_TRAN_ADDUSER, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_EDITUSER", MC_TRAN_EDITUSER, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_DELUSER", MC_TRAN_DELUSER, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_ENABLEUSER", MC_TRAN_ENABLEUSER, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_DISABLEUSER", MC_TRAN_DISABLEUSER, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_IMPORT", MC_TRAN_IMPORT, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_EXPORT", MC_TRAN_EXPORT, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_ERRORLOG", MC_TRAN_ERRORLOG, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_CLEARERRORLOG", MC_TRAN_CLEARERRORLOG, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_TRAN_GETSUBACCTS", MC_TRAN_GETSUBACCTS, MCVE_CONST);

	/* Value definitions for Admin Types */
	REGISTER_LONG_CONSTANT("MC_ADMIN_GUT", MC_ADMIN_GUT, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_ADMIN_GL", MC_ADMIN_GL, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_ADMIN_GFT", MC_ADMIN_GFT, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_ADMIN_BT", MC_ADMIN_BT, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_ADMIN_UB", MC_ADMIN_UB, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_ADMIN_QC", MC_ADMIN_QC, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_ADMIN_RS", MC_ADMIN_RS, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_ADMIN_CTH", MC_ADMIN_CTH, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_ADMIN_CFH", MC_ADMIN_CFH, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_ADMIN_FORCESETTLE", MC_ADMIN_FORCESETTLE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_ADMIN_SETBATCHNUM", MC_ADMIN_SETBATCHNUM, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_ADMIN_RENUMBERBATCH", MC_ADMIN_RENUMBERBATCH, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_ADMIN_FIELDEDIT", MC_ADMIN_FIELDEDIT, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MC_ADMIN_CLOSEBATCH", MC_ADMIN_CLOSEBATCH, MCVE_CONST);

	REGISTER_LONG_CONSTANT("M_UNUSED", MCVE_UNUSED, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_NEW", MCVE_NEW, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_PENDING", MCVE_PENDING, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_DONE", MCVE_DONE, MCVE_CONST);

	REGISTER_LONG_CONSTANT("M_GOOD", MCVE_GOOD, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_BAD", MCVE_BAD, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_STREET", MCVE_STREET, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_ZIP", MCVE_ZIP, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_UNKNOWN", MCVE_UNKNOWN, MCVE_CONST);

	REGISTER_LONG_CONSTANT("M_ERROR", MCVE_ERROR, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_FAIL", MCVE_FAIL, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_SUCCESS", MCVE_SUCCESS, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_AUTH", MCVE_AUTH, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_DENY", MCVE_DENY, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_CALL", MCVE_CALL, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_DUPL", MCVE_DUPL, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_PKUP", MCVE_PKUP, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_RETRY", MCVE_RETRY, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_SETUP", MCVE_SETUP, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_TIMEOUT", MCVE_TIMEOUT, MCVE_CONST);

	REGISTER_LONG_CONSTANT("M_SALE", MCVE_SALE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_PREAUTH", MCVE_PREAUTH, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_FORCE", MCVE_FORCE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_OVERRIDE", MCVE_OVERRIDE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_RETURN", MCVE_RETURN, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_SETTLE", MCVE_SETTLE, MCVE_CONST);

	REGISTER_LONG_CONSTANT("M_PROC", MCVE_PROC, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_USER", MCVE_USER, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_PWD", MCVE_PWD, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_INDCODE", MCVE_INDCODE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_MERCHID", MCVE_MERCHID, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_BANKID", MCVE_BANKID, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_TERMID", MCVE_TERMID, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_CLIENTNUM", MCVE_CLIENTNUM, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_STOREID", MCVE_STOREID, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_AGENTID", MCVE_AGENTID, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_CHAINID", MCVE_CHAINID, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_ZIPCODE", MCVE_ZIPCODE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_TIMEZONE", MCVE_TIMEZONE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_MERCHCAT", MCVE_MERCHCAT, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_MERNAME", MCVE_MERNAME, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_MERCHLOC", MCVE_MERCHLOC, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_STATECODE", MCVE_STATECODE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("M_SERVICEPHONE", MCVE_SERVICEPHONE, MCVE_CONST);

	/* COMPATABILITY VARIABLES */
	REGISTER_LONG_CONSTANT("MCVE_UNUSED", MCVE_UNUSED, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_NEW", MCVE_NEW, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_PENDING", MCVE_PENDING, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_DONE", MCVE_DONE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_GOOD", MCVE_GOOD, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_BAD", MCVE_BAD, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_STREET", MCVE_STREET, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_ZIP", MCVE_ZIP, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_UNKNOWN", MCVE_UNKNOWN, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_ERROR", MCVE_ERROR, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_FAIL", MCVE_FAIL, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_SUCCESS", MCVE_SUCCESS, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_AUTH", MCVE_AUTH, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_DENY", MCVE_DENY, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_CALL", MCVE_CALL, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_DUPL", MCVE_DUPL, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_PKUP", MCVE_PKUP, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_RETRY", MCVE_RETRY, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_SETUP", MCVE_SETUP, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_TIMEOUT", MCVE_TIMEOUT, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_SALE", MCVE_SALE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_PREAUTH", MCVE_PREAUTH, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_FORCE", MCVE_FORCE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_OVERRIDE", MCVE_OVERRIDE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_RETURN", MCVE_RETURN, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_SETTLE", MCVE_SETTLE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_PROC", MCVE_PROC, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_USER", MCVE_USER, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_PWD", MCVE_PWD, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_INDCODE", MCVE_INDCODE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_MERCHID", MCVE_MERCHID, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_BANKID", MCVE_BANKID, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_TERMID", MCVE_TERMID, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_CLIENTNUM", MCVE_CLIENTNUM, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_STOREID", MCVE_STOREID, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_AGENTID", MCVE_AGENTID, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_CHAINID", MCVE_CHAINID, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_ZIPCODE", MCVE_ZIPCODE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_TIMEZONE", MCVE_TIMEZONE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_MERCHCAT", MCVE_MERCHCAT, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_MERNAME", MCVE_MERNAME, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_MERCHLOC", MCVE_MERCHLOC, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_STATECODE", MCVE_STATECODE, MCVE_CONST);
	REGISTER_LONG_CONSTANT("MCVE_SERVICEPHONE", MCVE_SERVICEPHONE, MCVE_CONST);


	return SUCCESS;
}
/* }}} */

/* {{{ module information */
PHP_MINFO_FUNCTION(mcve)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "mcve support", "enabled");
	php_info_print_table_row(2, "version", PHP_MCVE_VERSION);
	php_info_print_table_end();
}
/* }}} */


/* {{{ proto int m_initengine(string location)
   Ready the client for IP/SSL Communication */
PHP_FUNCTION(m_initengine)
{
	int ret;
	zval **arg;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(arg);

	if (mcve_init) {
		MCVE_DestroyEngine();
	}
	ret = MCVE_InitEngine(Z_STRVAL_PP(arg));
	mcve_init = 1;

	RETURN_LONG(ret);
}
/* }}} */

/* {{{ proto resource m_initconn(void)
   Create and initialize an MCVE_CONN structure */
PHP_FUNCTION(m_initconn)
{
	MCVE_CONN *conn;

	conn = emalloc(sizeof(MCVE_CONN));

	MCVE_InitConn(conn);

	ZEND_REGISTER_RESOURCE(return_value, conn, le_conn);
}
/* }}} */


/* {{{ proto bool m_deleteresponse(resource conn, int identifier)
   Delete specified transaction from MCVE_CONN structure */
PHP_FUNCTION(m_deleteresponse)
{
	MCVE_CONN *conn;
	zval **arg1, **arg2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_long_ex(arg2);

	MCVE_DeleteResponse(conn, Z_LVAL_PP(arg2));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool m_deletetrans(resource conn, int identifier)
   Delete specified transaction from MCVE_CONN structure */
PHP_FUNCTION(m_deletetrans)
{
	MCVE_CONN *conn;
	zval **arg1, **arg2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection",
						le_conn);

	convert_to_long_ex(arg2);

	MCVE_DeleteTrans(conn, Z_LVAL_PP(arg2));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void m_destroyconn(resource conn)
   Destroy the connection and MCVE_CONN structure */
PHP_FUNCTION(m_destroyconn)
{
#if 0
	MCVE_CONN *conn;
#endif 
	zval **arg;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE)
		WRONG_PARAM_COUNT;

/* If MCVE_DestroyConn() is called within a PHP script, the Resource handle is
   never cleared, as there does not appear to be an UNREGISTER or DESTROY resource
   call in the Zend API.  What happens is this uninitialized memory location is
   passed again to the MCVE_DestroyConn() function at script exit (cleanup), and
   causes weird stuff. So we just go ahead and let the PHP garbage collector call
   our _free_mcve_conn()  we registered (le_conn) to clean up */
#if 0
	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg, -1, "mcve connection", le_conn);

	MCVE_DestroyConn(conn);
#endif
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int m_setdropfile(resource conn, string directory)
   Set the connection method to Drop-File */
PHP_FUNCTION(m_setdropfile)
{
	MCVE_CONN *conn;
	int retval;
	zval **arg1, **arg2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_string_ex(arg2);

	retval = MCVE_SetDropFile(conn, Z_STRVAL_PP(arg2));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_setip(resource conn, string host, int port)
   Set the connection method to IP */
PHP_FUNCTION(m_setip)
{
	MCVE_CONN *conn;
	int retval;
	zval **arg1, **arg2, **arg3;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_string_ex(arg2);
	convert_to_long_ex(arg3);

	retval = MCVE_SetIP(conn, Z_STRVAL_PP(arg2), Z_LVAL_PP(arg3));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_setssl(resource conn, string host, int port)
   Set the connection method to SSL */
PHP_FUNCTION(m_setssl)
{
	MCVE_CONN *conn;
	int retval;
	zval **arg1, **arg2, **arg3;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_string_ex(arg2);
	convert_to_long_ex(arg3);

	retval = MCVE_SetSSL(conn, Z_STRVAL_PP(arg2), Z_LVAL_PP(arg3));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_setssl_files(resource conn, string sslkeyfile, string sslcertfile)
   Set certificate key files and certificates if server requires client certificate
   verification
*/
PHP_FUNCTION(m_setssl_files)
{
	MCVE_CONN *conn;
	int retval;
	zval **arg1, **arg2, **arg3;

#ifndef LIBMONETRA_BELOW_4_2
	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE)
		WRONG_PARAM_COUNT;
	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);
#else
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;
#endif

	convert_to_string_ex(arg2);

#ifndef LIBMONETRA_BELOW_4_2
	convert_to_string_ex(arg3);
	retval = MCVE_SetSSL_Files(conn, Z_STRVAL_PP(arg2), Z_STRVAL_PP(arg3));
#else
	convert_to_string_ex(arg1);	
	retval = MCVE_SetSSL_Files(Z_STRVAL_PP(arg1), Z_STRVAL_PP(arg2));
#endif
	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_settimeout(resource conn, int seconds)
   Set maximum transaction time (per trans)
*/
PHP_FUNCTION(m_settimeout)
{
	MCVE_CONN *conn;
	int retval;
	zval **arg1, **arg2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_long_ex(arg2);

	retval = MCVE_SetTimeout(conn, Z_LVAL_PP(arg2));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_setblocking(resource conn, int tf)
   Set blocking/non-blocking mode for connection
*/
PHP_FUNCTION(m_setblocking)
{
	MCVE_CONN *conn;
	int retval;
	zval **arg1, **arg2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_long_ex(arg2);

	retval = MCVE_SetBlocking(conn, Z_LVAL_PP(arg2));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto bool m_verifyconnection(resource conn, int tf)
   Set whether or not to PING upon connect to verify connection
*/
PHP_FUNCTION(m_verifyconnection)
{
	MCVE_CONN *conn;
	zval **arg1, **arg2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_long_ex(arg2);

	MCVE_VerifyConnection(conn, Z_LVAL_PP(arg2));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool m_verifysslcert(resource conn, int tf)
   Set whether or not to verify the server ssl certificate
*/
PHP_FUNCTION(m_verifysslcert)
{
	MCVE_CONN *conn;
	zval **arg1, **arg2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_long_ex(arg2);

	MCVE_VerifySSLCert(conn, Z_LVAL_PP(arg2));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool m_maxconntimeout(resource conn, int secs)
   The maximum amount of time the API will attempt a connection to MCVE
*/
PHP_FUNCTION(m_maxconntimeout)
{
	MCVE_CONN *conn;
	zval **arg1, **arg2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_long_ex(arg2);

	MCVE_MaxConnTimeout(conn, Z_LVAL_PP(arg2));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int m_connect(resource conn)
   Establish the connection to MCVE */
PHP_FUNCTION(m_connect)
{
	MCVE_CONN *conn;
	int retval;
	zval **arg;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg, -1, "mcve connection", le_conn);

	retval = MCVE_Connect(conn);

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto string m_connectionerror(resource conn)
   Get a textual representation of why a connection failed */
PHP_FUNCTION(m_connectionerror)
{
	MCVE_CONN *conn;
	char *retval;
	zval **arg;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg, -1, "mcve connection", le_conn);

	retval = MCVE_ConnectionError(conn);

	if (retval == NULL) {
		RETVAL_STRING("",1);
	} else {
		RETVAL_STRING(retval, 1);
	}
}
/* }}} */

/* {{{ proto int m_transactionssent(resource conn)
   Check to see if outgoing buffer is clear */
PHP_FUNCTION(m_transactionssent)
{
	MCVE_CONN *conn;
	int retval;
	zval **arg;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg, -1, "mcve connection", le_conn);

	retval = MCVE_TransactionsSent(conn);

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_ping(resource conn)
   Send a ping request to MCVE */
PHP_FUNCTION(m_ping)
{
	MCVE_CONN *conn;
	int retval;
	zval **arg;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg, -1, "mcve connection", le_conn);

	retval = MCVE_Ping(conn);

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_transnew(resource conn)
   Start a new transaction */
PHP_FUNCTION(m_transnew)
{
	MCVE_CONN *conn;
	int retval;
	zval **arg;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg, -1, "mcve connection", le_conn);

	retval = MCVE_TransNew(conn);

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_transparam(resource conn, long identifier, int key, ...)
   Add a parameter to a transaction */
PHP_FUNCTION(m_transparam)
{
	MCVE_CONN *conn;
	int retval;
	int key;
	zval **arg1, **arg2, **arg3, **arg4, **arg5;

	if (ZEND_NUM_ARGS() == 4) {
		if (zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE)
			WRONG_PARAM_COUNT;
	} else if (ZEND_NUM_ARGS() == 5) {
		if (zend_get_parameters_ex(5, &arg1, &arg2, &arg3, &arg4, &arg5) == FAILURE)
			WRONG_PARAM_COUNT;
	} else {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg3);

	key = Z_LVAL_PP(arg3);

	if (key == MC_CUSTOM && ZEND_NUM_ARGS() != 5)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_long_ex(arg2);

	switch (key) {
		/* Typecast to MCVE_int64 */
		case MC_TTID:
			convert_to_long_ex(arg4);
			retval = MCVE_TransParam(conn, (long)Z_LVAL_PP(arg2), key, (MCVE_int64)Z_LVAL_PP(arg4));
		break;

		/* Typecast to long */

		case MC_PTRANNUM:
		case MC_TIMESTAMP:
		case MC_MARKER:
			convert_to_long_ex(arg4);
			retval = MCVE_TransParam(conn, (long)Z_LVAL_PP(arg2), key, (long)Z_LVAL_PP(arg4));
		break;

		/* Typecast to int */
		case MC_ADMIN:
		case MC_SUB:
		case MC_TRANTYPE:
		case MC_AUDITTYPE:
		case MC_EXCHARGES:
		case MC_INQUIRY:
		case MC_PRIORITY:
		case MC_CARDTYPES:
		case MC_ORIGTYPE:
		case MC_VOIDORIGTYPE:
			convert_to_long_ex(arg4);
			retval = MCVE_TransParam(conn, (long)Z_LVAL_PP(arg2), key, (int)Z_LVAL_PP(arg4));
		break;

		/* Typecast to double */
		case MC_AMOUNT:
		case MC_EXAMOUNT:
		case MC_RATE:
			convert_to_double_ex(arg4);
			retval = MCVE_TransParam(conn, (long)Z_LVAL_PP(arg2), key, (double)Z_DVAL_PP(arg4));
		break;

		case MC_CUSTOM:
			convert_to_string_ex(arg4);
			convert_to_string_ex(arg5);
			retval = MCVE_TransParam(conn, (long)Z_LVAL_PP(arg2), key, Z_STRVAL_PP(arg4), Z_STRVAL_PP(arg5));
		break;

		default:
			convert_to_string_ex(arg4);
			retval = MCVE_TransParam(conn, (long)Z_LVAL_PP(arg2), key, Z_STRVAL_PP(arg4));
		break;
	}

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_transsend(resource conn, long identifier)
   Finalize and send the transaction */
PHP_FUNCTION(m_transsend)
{
	MCVE_CONN *conn;
	int retval;
	zval **arg1, **arg2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_long_ex(arg2);

	retval = MCVE_TransSend(conn, Z_LVAL_PP(arg2));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto string m_responseparam(resource conn, long identifier, string key)
   Get a custom response parameter */
PHP_FUNCTION(m_responseparam)
{
	MCVE_CONN *conn;
	char *retval;
	zval **arg1, **arg2, **arg3;

	if (ZEND_NUM_ARGS() != 3 ||
		zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_long_ex(arg2);
	convert_to_string_ex(arg3);

	retval = MCVE_ResponseParam(conn, Z_LVAL_PP(arg2), Z_STRVAL_PP(arg3));

	if (retval == NULL) {
		RETVAL_STRING("",1);
	} else {
		RETVAL_STRING(retval, 1);
	}
}
/* }}} */

/* {{{ proto string m_getuserparam(resource conn, long identifier, int key)
   Get a user response parameter */
PHP_FUNCTION(m_getuserparam)
{
	MCVE_CONN *conn;
	char *retval;
	zval **arg1, **arg2, **arg3;

	if (ZEND_NUM_ARGS() != 3 ||
		zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);

	retval = MCVE_GetUserParam(conn, Z_LVAL_PP(arg2), Z_LVAL_PP(arg3));

	if (retval == NULL) {
		RETVAL_STRING("",1);
	} else {
		RETVAL_STRING(retval, 1);
	}
}
/* }}} */

/* {{{ proto int m_returnstatus(resource conn, int identifier)
   Check to see if the transaction was successful */
PHP_FUNCTION(m_returnstatus)
{
	MCVE_CONN *conn;
	int retval;
	zval **arg1, **arg2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_long_ex(arg2);

	retval = MCVE_ReturnStatus(conn, Z_LVAL_PP(arg2));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_returncode(resource conn, int identifier)
   Grab the exact return code from the transaction */
PHP_FUNCTION(m_returncode)
{
	MCVE_CONN *conn;
	int retval;
	zval **arg1, **arg2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_long_ex(arg2);

	retval = MCVE_ReturnCode(conn, Z_LVAL_PP(arg2));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_transactionitem(resource conn, int identifier)
   Get the ITEM number in the associated batch for this transaction */
PHP_FUNCTION(m_transactionitem)
{
	MCVE_CONN *conn;
	int retval;
	zval **arg1, **arg2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_long_ex(arg2);

	retval = MCVE_TransactionItem(conn, Z_LVAL_PP(arg2));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_transactionavs(resource conn, int identifier)
   Get the Address Verification return status */
PHP_FUNCTION(m_transactionavs)
{
	MCVE_CONN *conn;
	int retval;
	zval **arg1, **arg2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_long_ex(arg2);

	retval = MCVE_TransactionAVS(conn, Z_LVAL_PP(arg2));

	RETURN_LONG(retval);
}
/* }}} */


/* {{{ proto int m_transactioncv(resource conn, int identifier)
   Get the CVC2/CVV2/CID return status */
PHP_FUNCTION(m_transactioncv)
{
	MCVE_CONN *conn;
	int retval;
	zval **arg1, **arg2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_long_ex(arg2);

	retval = MCVE_TransactionCV(conn, Z_LVAL_PP(arg2));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_transactionbatch(resource conn, int identifier)
   Get the batch number associated with the transaction */
PHP_FUNCTION(m_transactionbatch)
{
	MCVE_CONN *conn;
	int retval;
	zval **arg1, **arg2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_long_ex(arg2);

	retval = MCVE_TransactionBatch(conn, Z_LVAL_PP(arg2));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_transactionid(resource conn, int identifier)
   Get the unique system id for the transaction
*/
PHP_FUNCTION(m_transactionid)
{
	MCVE_CONN *conn;
	int retval;
	zval **arg1, **arg2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_long_ex(arg2);

	retval = MCVE_TransactionID(conn, Z_LVAL_PP(arg2));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto string m_transactionauth(resource conn, int identifier)
   Get the authorization number returned for the transaction (alpha-numeric) */
PHP_FUNCTION(m_transactionauth)
{
	MCVE_CONN *conn;
	char *retval;
	zval **arg1, **arg2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_long_ex(arg2);

	retval = MCVE_TransactionAuth(conn, Z_LVAL_PP(arg2));

	if (retval == NULL) {
		RETVAL_STRING("",1);
	} else {
		RETVAL_STRING(retval, 1);
	}
}
/* }}} */

/* {{{ proto string m_transactiontext(resource conn, int identifier)
   Get verbiage (text) return from MCVE or processing institution */
PHP_FUNCTION(m_transactiontext)
{
	MCVE_CONN *conn;
	char *retval;
	zval **arg1, **arg2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_long_ex(arg2);

	retval = MCVE_TransactionText(conn, Z_LVAL_PP(arg2));
	if (retval == NULL) {
		RETVAL_STRING("",1);
	} else {
		RETVAL_STRING(retval, 1);
	}
}
/* }}} */

/* {{{ proto int m_monitor(resource conn)
   Perform communication with MCVE (send/receive data)   Non-blocking */
PHP_FUNCTION(m_monitor)
{
	MCVE_CONN *conn;
	int retval;
	zval **arg;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg, -1, "mcve connection", le_conn);

	retval = MCVE_Monitor(conn);

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_transinqueue(resource conn)
   Number of transactions in client-queue */
PHP_FUNCTION(m_transinqueue)
{
	MCVE_CONN *conn;
	int retval;
	zval **arg;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg, -1, "mcve connection", le_conn);

	retval = MCVE_TransInQueue(conn);

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_checkstatus(resource conn, int identifier)
   Check to see if a transaction has completed */
PHP_FUNCTION(m_checkstatus)
{
	MCVE_CONN *conn;
	long retval;
	zval **arg1, **arg2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_long_ex(arg2);

	retval = MCVE_CheckStatus(conn, Z_LVAL_PP(arg2));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_completeauthorizations(resource conn, int &array)
   Number of complete authorizations in queue, returning an array of their identifiers */
PHP_FUNCTION(m_completeauthorizations)
{
	MCVE_CONN *conn;
	long i, *list, listnum;
	zval **arg1, **arg2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2))
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	listnum = MCVE_CompleteAuthorizations(conn, &list);

	if (listnum) {
		/* should use ZVAL_DESTRUCTOR */
		pval_destructor(*arg2);
		array_init(*arg2);
		for (i = 0; i < listnum; i++)
			add_index_long(*arg2, i, list[i]);
		free(list);
	}

	RETURN_LONG(listnum);
}
/* }}} */

/* {{{ proto int m_sale(resource conn, string username, string password, string trackdata, string account, string expdate, float amount, string street, string zip, string cv, string comments, string clerkid, string stationid, int ptrannum)
   Send a SALE to MCVE */
PHP_FUNCTION(m_sale)
{
	MCVE_CONN *conn;
	long retval;
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7, **arg8;
	zval **arg9, **arg10, **arg11, **arg12, **arg13, **arg14;

	if (ZEND_NUM_ARGS() != 14 || zend_get_parameters_ex(14, &arg1, &arg2,
		&arg3, &arg4, &arg5, &arg6, &arg7, &arg8, &arg9, &arg10, &arg11,
		&arg12, &arg13, &arg14) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);
	convert_to_string_ex(arg4);
	convert_to_string_ex(arg5);
	convert_to_string_ex(arg6);
	convert_to_double_ex(arg7);
	convert_to_string_ex(arg8);
	convert_to_string_ex(arg9);
	convert_to_string_ex(arg10);
	convert_to_string_ex(arg11);
	convert_to_string_ex(arg12);
	convert_to_string_ex(arg13);
	convert_to_long_ex(arg14);

	retval = MCVE_Sale(conn, Z_STRVAL_PP(arg2), Z_STRVAL_PP(arg3),
		Z_STRVAL_PP(arg4), Z_STRVAL_PP(arg5), Z_STRVAL_PP(arg6),
		Z_DVAL_PP(arg7), Z_STRVAL_PP(arg8), Z_STRVAL_PP(arg9),
		Z_STRVAL_PP(arg10), Z_STRVAL_PP(arg11), Z_STRVAL_PP(arg12),
		Z_STRVAL_PP(arg13), Z_LVAL_PP(arg14));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_preauth(resource conn, string username, string password, string trackdata, string account, string expdate, float amount, string street, string zip, string cv, string comments,	string clerkid, string stationid, int ptrannum)
   Send a PREAUTHORIZATION to MCVE */
PHP_FUNCTION(m_preauth)
{
	MCVE_CONN *conn;
	long retval;
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7, **arg8;
	zval **arg9, **arg10, **arg11, **arg12, **arg13, **arg14;

	if (ZEND_NUM_ARGS() != 14 || zend_get_parameters_ex(14, &arg1, &arg2,
		&arg3, &arg4, &arg5, &arg6, &arg7, &arg8, &arg9, &arg10, &arg11,
		&arg12, &arg13, &arg14) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);
	convert_to_string_ex(arg4);
	convert_to_string_ex(arg5);
	convert_to_string_ex(arg6);
	convert_to_double_ex(arg7);
	convert_to_string_ex(arg8);
	convert_to_string_ex(arg9);
	convert_to_string_ex(arg10);
	convert_to_string_ex(arg11);
	convert_to_string_ex(arg12);
	convert_to_string_ex(arg13);
	convert_to_long_ex(arg14);

	retval = MCVE_PreAuth(conn, Z_STRVAL_PP(arg2), Z_STRVAL_PP(arg3),
		Z_STRVAL_PP(arg4), Z_STRVAL_PP(arg5), Z_STRVAL_PP(arg6),
		Z_DVAL_PP(arg7), Z_STRVAL_PP(arg8), Z_STRVAL_PP(arg9),
		Z_STRVAL_PP(arg10), Z_STRVAL_PP(arg11), Z_STRVAL_PP(arg12),
		Z_STRVAL_PP(arg13), Z_LVAL_PP(arg14));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_override(resource conn, string username, string password, string trackdata, string account, string expdate, float amount, string street, string zip, string cv, string comments, string clerkid, string stationid, int ptrannum)
   Send an OVERRIDE to MCVE */
PHP_FUNCTION(m_override)
{
	MCVE_CONN *conn;
	long retval;
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7, **arg8;
	zval **arg9, **arg10, **arg11, **arg12, **arg13, **arg14;

	if (ZEND_NUM_ARGS() != 14 || zend_get_parameters_ex(14, &arg1, &arg2,
		&arg3, &arg4, &arg5, &arg6, &arg7, &arg8, &arg9, &arg10, &arg11,
		&arg12, &arg13, &arg14) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);
	convert_to_string_ex(arg4);
	convert_to_string_ex(arg5);
	convert_to_string_ex(arg6);
	convert_to_double_ex(arg7);
	convert_to_string_ex(arg8);
	convert_to_string_ex(arg9);
	convert_to_string_ex(arg10);
	convert_to_string_ex(arg11);
	convert_to_string_ex(arg12);
	convert_to_string_ex(arg13);
	convert_to_long_ex(arg14);

	retval = MCVE_Override(conn, Z_STRVAL_PP(arg2), Z_STRVAL_PP(arg3),
		Z_STRVAL_PP(arg4), Z_STRVAL_PP(arg5), Z_STRVAL_PP(arg6),
		Z_DVAL_PP(arg7), Z_STRVAL_PP(arg8), Z_STRVAL_PP(arg9),
		Z_STRVAL_PP(arg10), Z_STRVAL_PP(arg11), Z_STRVAL_PP(arg12),
		Z_STRVAL_PP(arg13), Z_LVAL_PP(arg14));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_void(resource conn, string username, string password, int sid, int ptrannum)
   VOID a transaction in the settlement queue */
PHP_FUNCTION(m_void)
{
	MCVE_CONN *conn;
	long retval;
	zval **arg1, **arg2, **arg3, **arg4, **arg5;

	if (ZEND_NUM_ARGS() != 5 ||
		zend_get_parameters_ex(5, &arg1, &arg2, &arg3, &arg4, &arg5) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);
	convert_to_long_ex(arg4);
	convert_to_long_ex(arg5);

	retval = MCVE_Void(conn, Z_STRVAL_PP(arg2), Z_STRVAL_PP(arg3), (MCVE_int64)Z_LVAL_PP(arg4), Z_LVAL_PP(arg5));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_preauthcompletion(resource conn, string username, string password, float finalamount, int sid, int ptrannum)
   Complete a PREAUTHORIZATION... Ready it for settlement */
PHP_FUNCTION(m_preauthcompletion)
{
	MCVE_CONN *conn;
	long retval;
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6;

	if (ZEND_NUM_ARGS() != 6 ||
		zend_get_parameters_ex(6, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_long_ex(arg5);
	convert_to_long_ex(arg6);

	retval = MCVE_PreAuthCompletion(conn, Z_STRVAL_PP(arg2),
		Z_STRVAL_PP(arg3), Z_DVAL_PP(arg4), Z_LVAL_PP(arg5), Z_LVAL_PP(arg6));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_force(resiurce conn, string username, string password, string trackdata, string account, string expdate, float amount, string authcode, string comments, string clerkid, string stationid, int ptrannum)
   Send a FORCE to MCVE.  (typically, a phone-authorization) */
PHP_FUNCTION(m_force)
{
	MCVE_CONN *conn;
	long retval;
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7, **arg8;
	zval **arg9, **arg10, **arg11, **arg12;

	if (ZEND_NUM_ARGS() != 12 || zend_get_parameters_ex(12, &arg1, &arg2,
		&arg3, &arg4, &arg5, &arg6, &arg7, &arg8, &arg9, &arg10, &arg11, &arg12) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);
	convert_to_string_ex(arg4);
	convert_to_string_ex(arg5);
	convert_to_string_ex(arg6);
	convert_to_double_ex(arg7);
	convert_to_string_ex(arg8);
	convert_to_string_ex(arg9);
	convert_to_string_ex(arg10);
	convert_to_string_ex(arg11);
	convert_to_long_ex(arg12);

	retval = MCVE_Force(conn, Z_STRVAL_PP(arg2), Z_STRVAL_PP(arg3),
		Z_STRVAL_PP(arg4), Z_STRVAL_PP(arg5), Z_STRVAL_PP(arg6),
		Z_DVAL_PP(arg7), Z_STRVAL_PP(arg8), Z_STRVAL_PP(arg9),
		Z_STRVAL_PP(arg10), Z_STRVAL_PP(arg11), Z_LVAL_PP(arg12));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_return(int conn, string username, string password, string trackdata, string account, string expdate, float amount, string comments, string clerkid, string stationid, int ptrannum)
   Issue a RETURN or CREDIT to MCVE */
PHP_FUNCTION(m_return)
{
	MCVE_CONN *conn;
	long retval;
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7, **arg8;
	zval **arg9, **arg10, **arg11;

	if (ZEND_NUM_ARGS() != 11 || zend_get_parameters_ex(11, &arg1, &arg2,
		&arg3, &arg4, &arg5, &arg6, &arg7, &arg8, &arg9, &arg10, &arg11) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);
	convert_to_string_ex(arg4);
	convert_to_string_ex(arg5);
	convert_to_string_ex(arg6);
	convert_to_double_ex(arg7);
	convert_to_string_ex(arg8);
	convert_to_string_ex(arg9);
	convert_to_string_ex(arg10);
	convert_to_long_ex(arg11);

	retval = MCVE_Return(conn, Z_STRVAL_PP(arg2), Z_STRVAL_PP(arg3),
		Z_STRVAL_PP(arg4), Z_STRVAL_PP(arg5), Z_STRVAL_PP(arg6),
		Z_DVAL_PP(arg7), Z_STRVAL_PP(arg8), Z_STRVAL_PP(arg9),
		Z_STRVAL_PP(arg10), Z_LVAL_PP(arg11));

	RETURN_LONG(retval);
}
/* }}} */


/* {{{ proto int m_settle(resource conn, string username, string password, string batch)
   Issue a settlement command to do a batch deposit */
PHP_FUNCTION(m_settle)
{
	MCVE_CONN *conn;
	long retval;
	zval **arg1, **arg2, **arg3, **arg4;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);
	convert_to_string_ex(arg4);

	retval = MCVE_Settle(conn, Z_STRVAL_PP(arg2), Z_STRVAL_PP(arg3), Z_STRVAL_PP(arg4));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_ub(resource conn, string username, string password)
   Get a list of all Unsettled batches */
PHP_FUNCTION(m_ub)
{
	MCVE_CONN *conn;
	long retval;
	zval **arg1, **arg2, **arg3;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);

	retval = MCVE_Ub(conn, Z_STRVAL_PP(arg2), Z_STRVAL_PP(arg3));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_qc(resource conn, string username, string password, string clerkid, string stationid, string comments, int ptrannum)
   Audit MCVE for a list of transactions in the outgoing queue */
PHP_FUNCTION(m_qc)
{
	MCVE_CONN *conn;
	long retval;
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7;

	if (ZEND_NUM_ARGS() != 7 ||
		zend_get_parameters_ex(7, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);
	convert_to_string_ex(arg4);
	convert_to_string_ex(arg5);
	convert_to_string_ex(arg6);
	convert_to_long_ex(arg7);

	retval = MCVE_Qc(conn, Z_STRVAL_PP(arg2), Z_STRVAL_PP(arg3), Z_STRVAL_PP(arg4),
		Z_STRVAL_PP(arg5), Z_STRVAL_PP(arg6), Z_LVAL_PP(arg7));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_gut(resource conn, string username, string password, int type, string account, string clerkid, string stationid, string comments, int ptrannum, string startdate, string enddate)
   Audit MCVE for Unsettled Transactions */
PHP_FUNCTION(m_gut)
{
	MCVE_CONN *conn;
	long retval;
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7, **arg8, **arg9, **arg10, **arg11;

	if (ZEND_NUM_ARGS() != 11 || zend_get_parameters_ex(11, &arg1, &arg2,
		&arg3, &arg4, &arg5, &arg6, &arg7, &arg8, &arg9, &arg10, &arg11) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);
	convert_to_long_ex(arg4);
	convert_to_string_ex(arg5);
	convert_to_string_ex(arg6);
	convert_to_string_ex(arg7);
	convert_to_string_ex(arg8);
	convert_to_double_ex(arg9);
	convert_to_string_ex(arg10);
	convert_to_string_ex(arg11);

	retval = MCVE_Gut(conn, Z_STRVAL_PP(arg2), Z_STRVAL_PP(arg3), Z_LVAL_PP(arg4),
		Z_STRVAL_PP(arg5), Z_STRVAL_PP(arg6), Z_STRVAL_PP(arg7), Z_STRVAL_PP(arg8),
		Z_DVAL_PP(arg9), Z_STRVAL_PP(arg10),Z_STRVAL_PP(arg11));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_gl(int conn, string username, string password, int type, string account, string batch, string clerkid, string stationid, string comments, int ptrannum, string startdate, string enddate)
   Audit MCVE for settled transactions */
PHP_FUNCTION(m_gl)
{
	MCVE_CONN *conn;
	long retval;
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7, **arg8, **arg9, **arg10, **arg11, **arg12;

	if (ZEND_NUM_ARGS() != 12 || zend_get_parameters_ex(12, &arg1, &arg2,
		&arg3, &arg4, &arg5, &arg6, &arg7, &arg8, &arg9, &arg10, &arg11, &arg12) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);
	convert_to_long_ex(arg4);
	convert_to_string_ex(arg5);
	convert_to_string_ex(arg6);
	convert_to_string_ex(arg7);
	convert_to_string_ex(arg8);
	convert_to_string_ex(arg9);
	convert_to_double_ex(arg10);
	convert_to_string_ex(arg11);
	convert_to_string_ex(arg12);
	
	retval = MCVE_Gl(conn, Z_STRVAL_PP(arg2), Z_STRVAL_PP(arg3), Z_LVAL_PP(arg4), 
		Z_STRVAL_PP(arg5), Z_STRVAL_PP(arg6), Z_STRVAL_PP(arg7), Z_STRVAL_PP(arg8),
		Z_STRVAL_PP(arg9), Z_DVAL_PP(arg10), Z_STRVAL_PP(arg11), Z_STRVAL_PP(arg12));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_gft(resource conn, string username, string password, int type, string account, string clerkid, string stationid, string comments, int ptrannum, string startdate, string enddate)
   Audit MCVE for Failed transactions
*/
PHP_FUNCTION(m_gft)
{
	MCVE_CONN *conn;
	long retval;
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7, **arg8, **arg9, **arg10, **arg11;

	if (ZEND_NUM_ARGS() != 11 || zend_get_parameters_ex(11, &arg1, &arg2,
		&arg3, &arg4, &arg5, &arg6, &arg7, &arg8, &arg9, &arg10, &arg11) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);
	convert_to_long_ex(arg4);
	convert_to_string_ex(arg5);
	convert_to_string_ex(arg6);
	convert_to_string_ex(arg7);
	convert_to_string_ex(arg8);
	convert_to_long_ex(arg9);
	convert_to_string_ex(arg10);
	convert_to_string_ex(arg11);

	retval = MCVE_Gft(conn, Z_STRVAL_PP(arg2), Z_STRVAL_PP(arg3), Z_LVAL_PP(arg4),
		Z_STRVAL_PP(arg5), Z_STRVAL_PP(arg6), Z_STRVAL_PP(arg7), Z_STRVAL_PP(arg8),
		Z_LVAL_PP(arg9), Z_STRVAL_PP(arg10), Z_STRVAL_PP(arg11));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_chkpwd(resource conn, string username, string password)
   Verify Password */
PHP_FUNCTION(m_chkpwd)
{
	MCVE_CONN *conn;
	long retval;
	zval **arg1, **arg2, **arg3;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2,	&arg3) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);

	retval = MCVE_Chkpwd(conn, Z_STRVAL_PP(arg2), Z_STRVAL_PP(arg3));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_bt(resource conn, string username, string password)
   Get unsettled batch totals */
PHP_FUNCTION(m_bt)
{
	MCVE_CONN *conn;
	long retval;
	zval **arg1, **arg2, **arg3;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);

	retval = MCVE_Bt(conn, Z_STRVAL_PP(arg2), Z_STRVAL_PP(arg3));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto string m_getcell(resource conn, int identifier, string column, int row)
   Get a specific cell from a comma delimited response by column name */
PHP_FUNCTION(m_getcell)
{
	MCVE_CONN *conn;
	char *retval;
	zval **arg1, **arg2, **arg3, **arg4;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_long_ex(arg2);
	convert_to_string_ex(arg3);
	convert_to_long_ex(arg4);

	retval = MCVE_GetCell(conn, Z_LVAL_PP(arg2), Z_STRVAL_PP(arg3), Z_LVAL_PP(arg4));

	if (retval == NULL) {
		RETURN_STRING("", 1);
	} else {
		RETURN_STRING(retval, 1);
	}
}
/* }}} */

/* {{{ proto string m_getcellbynum(resource conn, int identifier, int column, int row)
   Get a specific cell from a comma delimited response by column number */
PHP_FUNCTION(m_getcellbynum)
{
	MCVE_CONN *conn;
	char *retval;
	zval **arg1, **arg2, **arg3, **arg4;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_long_ex(arg4);

	retval = MCVE_GetCellByNum(conn, Z_LVAL_PP(arg2), Z_LVAL_PP(arg3), Z_LVAL_PP(arg4));

	if (retval == NULL) {
		RETURN_STRING("", 1);
	} else {
		RETURN_STRING(retval, 1);
	}
}
/* }}} */

/* {{{ proto int m_numcolumns(resource conn, int identifier)
   Number of columns returned in a comma delimited response */
PHP_FUNCTION(m_numcolumns)
{
	MCVE_CONN *conn;
	long retval;
	zval **arg1, **arg2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_long_ex(arg2);

	retval = MCVE_NumColumns(conn, Z_LVAL_PP(arg2));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_numrows(resource conn, int identifier)
   Number of rows returned in a comma delimited response */
PHP_FUNCTION(m_numrows)
{
	MCVE_CONN *conn;
	long retval;
	zval **arg1, **arg2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_long_ex(arg2);

	retval = MCVE_NumRows(conn, Z_LVAL_PP(arg2));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_iscommadelimited(resource conn, int identifier)
   Checks to see if response is comma delimited */
PHP_FUNCTION(m_iscommadelimited)
{
	MCVE_CONN *conn;
	long retval;
	zval **arg1, **arg2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_long_ex(arg2);

	retval = MCVE_IsCommaDelimited(conn, Z_LVAL_PP(arg2));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_parsecommadelimited(resource conn, int identifier)
   Parse the comma delimited response so m_getcell, etc will work */
PHP_FUNCTION(m_parsecommadelimited)
{
	MCVE_CONN *conn;
	long retval;
	zval **arg1, **arg2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_long_ex(arg2);

	retval = MCVE_ParseCommaDelimited(conn, Z_LVAL_PP(arg2));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto string m_getcommadelimited(resource conn, int identifier)
   Get the RAW comma delimited data returned from MCVE */
PHP_FUNCTION(m_getcommadelimited)
{
	MCVE_CONN *conn;
	char *retval;
	zval **arg1, **arg2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_long_ex(arg2);

	retval = MCVE_GetCommaDelimited(conn, Z_LVAL_PP(arg2));

	RETURN_STRING(retval, 1);
}
/* }}} */

/* {{{ proto string m_getheader(resource conn, int identifier, int column_num)
   Get the name of the column in a comma-delimited response */
PHP_FUNCTION(m_getheader)
{
	MCVE_CONN *conn;
	char *retval;
	zval **arg1, **arg2, **arg3;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);

	retval = MCVE_GetHeader(conn, Z_LVAL_PP(arg2), Z_LVAL_PP(arg3));

	RETURN_STRING(retval, 1);
}
/* }}} */

/* {{{ proto void m_destroyengine(void)
   Free memory associated with IP/SSL connectivity */
PHP_FUNCTION(m_destroyengine)
{
	MCVE_DestroyEngine();
	mcve_init = 0;
}
/* }}} */

/* {{{ proto int m_chngpwd(resource conn, string admin_password, string new_password)
   Change the system administrator's password */
PHP_FUNCTION(m_chngpwd)
{
	MCVE_CONN *conn;
	long retval;
	zval **arg1, **arg2, **arg3;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);

	retval = MCVE_Chngpwd(conn, Z_STRVAL_PP(arg2), Z_STRVAL_PP(arg3));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_listusers(resource conn, string admin_password)
   List all users on MCVE system */
PHP_FUNCTION(m_listusers)
{
	MCVE_CONN *conn;
	long retval;
	zval **arg1, **arg2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_string_ex(arg2);

	retval = MCVE_ListUsers(conn, Z_STRVAL_PP(arg2));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_enableuser(resource conn, string admin_password, string username)
   Enable an inactive MCVE user account */
PHP_FUNCTION(m_enableuser)
{
	MCVE_CONN *conn;
	long retval;
	zval **arg1, **arg2, **arg3;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);

	retval = MCVE_EnableUser(conn, Z_STRVAL_PP(arg2), Z_STRVAL_PP(arg3));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_disableuser(resource conn, string admin_password, string username)
   Disable an active MCVE user account */
PHP_FUNCTION(m_disableuser)
{
	MCVE_CONN *conn;
	long retval;
	zval **arg1, **arg2, **arg3;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);

	retval = MCVE_DisableUser(conn, Z_STRVAL_PP(arg2), Z_STRVAL_PP(arg3));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_deluser(resource conn, string admin_password, string username)
   Delete an MCVE user account */
PHP_FUNCTION(m_deluser)
{
	MCVE_CONN *conn;
	long retval;
	zval **arg1, **arg2, **arg3;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);

	retval = MCVE_DelUser(conn, Z_STRVAL_PP(arg2), Z_STRVAL_PP(arg3));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_liststats(resource conn, string admin_password)
   List statistics for all users on MCVE system */
PHP_FUNCTION(m_liststats)
{
	MCVE_CONN *conn;
	long retval;
	zval **arg1, **arg2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);

	convert_to_string_ex(arg2);

	retval = MCVE_ListStats(conn, Z_STRVAL_PP(arg2));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto resource m_initusersetup(void)
   Initialize structure to store user data */
PHP_FUNCTION(m_initusersetup)
{
	MCVE_UserSetup *usersetup;

	usersetup = emalloc(sizeof(MCVE_UserSetup));

	MCVE_InitUserSetup(usersetup);

	ZEND_REGISTER_RESOURCE(return_value, usersetup, le_user);
}
/* }}} */

/* {{{ proto void m_deleteusersetup(resource usersetup)
   Deallocate data associated with usersetup structure */
PHP_FUNCTION(m_deleteusersetup)
{
	MCVE_UserSetup *usersetup;
	zval **arg;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(usersetup, MCVE_UserSetup *, arg, -1, "mcve user setup", le_user);

	MCVE_DeleteUserSetup(usersetup);
	efree(usersetup);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int m_adduserarg(resource usersetup, int argtype, string argval)
   Add a value to user configuration structure */
PHP_FUNCTION(m_adduserarg)
{
	MCVE_UserSetup *usersetup;
	long retval;
	zval **arg1, **arg2, **arg3;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(usersetup, MCVE_UserSetup *, arg1, -1, "mcve user setup", le_user);

	convert_to_long_ex(arg2);
	convert_to_string_ex(arg3);

	retval = MCVE_AddUserArg(usersetup, Z_LVAL_PP(arg2), Z_STRVAL_PP(arg3));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto string m_getuserarg(resource usersetup, int argtype)
   Grab a value from usersetup structure */
PHP_FUNCTION(m_getuserarg)
{
	MCVE_UserSetup *usersetup;
	char *retval;
	zval **arg1, **arg2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(usersetup, MCVE_UserSetup *, arg1, -1, "mcve user setup", le_user);

	convert_to_long_ex(arg2);

	retval = MCVE_GetUserArg(usersetup, Z_LVAL_PP(arg2));

	RETURN_STRING(retval, 1);
}
/* }}} */

/* {{{ proto int m_adduser(resource conn, string admin_password, int usersetup)
   Add an MCVE user using usersetup structure */
PHP_FUNCTION(m_adduser)
{
	MCVE_CONN *conn;
	MCVE_UserSetup *usersetup;
	long retval;
	zval **arg1, **arg2, **arg3;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);
	ZEND_FETCH_RESOURCE(usersetup, MCVE_UserSetup *, arg3, -1, "mcve user setup", le_user);

	convert_to_string_ex(arg2);

	retval = MCVE_AddUser(conn, Z_STRVAL_PP(arg2), usersetup);

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int m_edituser(resource conn, string admin_password, int usersetup)
   Edit MCVE user using usersetup structure */
PHP_FUNCTION(m_edituser)
{
	MCVE_CONN *conn;
	MCVE_UserSetup *usersetup;
	long retval;
	zval **arg1, **arg2, **arg3;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(conn, MCVE_CONN *, arg1, -1, "mcve connection", le_conn);
	ZEND_FETCH_RESOURCE(usersetup, MCVE_UserSetup *, arg3, -1, "mcve user setup", le_user);

	convert_to_string_ex(arg2);

	retval = MCVE_EditUser(conn, Z_STRVAL_PP(arg2), usersetup);

	RETURN_LONG(retval);
}
/* }}} */


/* {{{ proto int m_uwait(long microsecs)
   Wait x microsecs */
PHP_FUNCTION(m_uwait)
{
	long retval;
	zval **arg;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long_ex(arg);

	retval = MCVE_uwait(Z_LVAL_PP(arg));

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto string m_text_code(string code)
   Get a textual representation of the return_code */
PHP_FUNCTION(m_text_code)
{
	char *retval;
	zval **arg;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long_ex(arg);

	retval = MCVE_TEXT_Code(Z_LVAL_PP(arg));

	if (retval == NULL) {
		RETVAL_STRING("",1);
	} else {
		RETVAL_STRING(retval, 1);
	}
}
/* }}} */

/* {{{ proto string m_text_avs(string code)
   Get a textual representation of the return_avs */
PHP_FUNCTION(m_text_avs)
{
	char *retval;
	zval **arg;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long_ex(arg);

	retval = MCVE_TEXT_AVS(Z_LVAL_PP(arg));

	if (retval == NULL) {
		RETVAL_STRING("",1);
	} else {
		RETVAL_STRING(retval, 1);
	}
}
/* }}} */

/* {{{ proto string m_text_cv(int code)
   Get a textual representation of the return_cv */
PHP_FUNCTION(m_text_cv)
{
	char *retval;
	zval **arg;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long_ex(arg);

	retval = MCVE_TEXT_CV(Z_LVAL_PP(arg));

	if (retval == NULL) {
		RETVAL_STRING("",1);
	} else {
		RETVAL_STRING(retval, 1);
	}
}
/* }}} */

/* Map new funnction names to old function names for compatability */
M_map_function(m_initengine,			mcve_initengine)
M_map_function(m_initconn,			mcve_initconn)
M_map_function(m_deleteresponse,		mcve_deleteresponse)
M_map_function(m_destroyconn,			mcve_destroyconn)
M_map_function(m_setdropfile,			mcve_setdropfile)
M_map_function(m_setip,				mcve_setip)
M_map_function(m_setssl,			mcve_setssl)
M_map_function(m_setssl_files,			mcve_setssl_files)
M_map_function(m_settimeout,			mcve_settimeout)
M_map_function(m_setblocking,			mcve_setblocking)
M_map_function(m_verifyconnection,		mcve_verifyconnection)
M_map_function(m_verifysslcert,			mcve_verifysslcert)
M_map_function(m_maxconntimeout,		mcve_maxconntimeout)
M_map_function(m_connectionerror,		mcve_connectionerror)
M_map_function(m_deletetrans,			mcve_deletetrans)
M_map_function(m_connect,			mcve_connect)
M_map_function(m_transnew,			mcve_transnew)
M_map_function(m_transparam,			mcve_transparam)
M_map_function(m_transsend,			mcve_transsend)
M_map_function(m_ping,				mcve_ping)
M_map_function(m_responseparam,			mcve_responseparam)
M_map_function(m_returnstatus,			mcve_returnstatus)
M_map_function(m_returncode,			mcve_returncode)
M_map_function(m_transactionssent,		mcve_transactionssent)
M_map_function(m_transactionitem,		mcve_transactionitem)
M_map_function(m_transactionbatch,		mcve_transactionbatch)
M_map_function(m_transactionid,			mcve_transactionid)
M_map_function(m_transactionauth,		mcve_transactionauth)
M_map_function(m_transactiontext,		mcve_transactiontext)
M_map_function(m_transactionavs,		mcve_transactionavs)
M_map_function(m_transactioncv,			mcve_transactioncv)
M_map_function(m_getuserparam,			mcve_getuserparam)
M_map_function(m_monitor,			mcve_monitor)
M_map_function(m_transinqueue,			mcve_transinqueue)
M_map_function(m_checkstatus,			mcve_checkstatus)
M_map_function(m_completeauthorizations,	mcve_completeauthorizations)
M_map_function(m_sale,				mcve_sale)
M_map_function(m_preauth,			mcve_preauth)
M_map_function(m_void,				mcve_void)
M_map_function(m_preauthcompletion,		mcve_preauthcompletion)
M_map_function(m_force,				mcve_force)
M_map_function(m_override,			mcve_override)
M_map_function(m_return,			mcve_return)
M_map_function(m_iscommadelimited,		mcve_iscommadelimited)
M_map_function(m_parsecommadelimited,		mcve_parsecommadelimited)
M_map_function(m_getcommadelimited,		mcve_getcommadelimited)
M_map_function(m_getcell,			mcve_getcell)
M_map_function(m_getcellbynum,			mcve_getcellbynum)
M_map_function(m_numcolumns,			mcve_numcolumns)
M_map_function(m_numrows,			mcve_numrows)
M_map_function(m_getheader,			mcve_getheader)
M_map_function(m_destroyengine,			mcve_destroyengine)
M_map_function(m_settle,			mcve_settle)
M_map_function(m_gut,				mcve_gut)
M_map_function(m_gl,				mcve_gl)
M_map_function(m_gft,				mcve_gft)
M_map_function(m_qc,				mcve_qc)
M_map_function(m_ub,				mcve_ub)
M_map_function(m_chkpwd,			mcve_chkpwd)
M_map_function(m_bt,				mcve_bt)
M_map_function(m_uwait,				mcve_uwait)
M_map_function(m_text_code,			mcve_text_code)
M_map_function(m_text_avs,			mcve_text_avs)
M_map_function(m_text_cv,			mcve_text_cv)
M_map_function(m_chngpwd,			mcve_chngpwd)
M_map_function(m_listusers,			mcve_listusers)
M_map_function(m_adduser,			mcve_adduser)
M_map_function(m_enableuser,			mcve_enableuser)
M_map_function(m_disableuser,			mcve_disableuser)
M_map_function(m_getuserarg,			mcve_getuserarg)
M_map_function(m_adduserarg,			mcve_adduserarg)
M_map_function(m_deleteusersetup,		mcve_deleteusersetup)
M_map_function(m_initusersetup,			mcve_initusersetup)
M_map_function(m_deluser,			mcve_deluser)
M_map_function(m_edituser,			mcve_edituser)
M_map_function(m_liststats,			mcve_liststats)
 
#endif

/* END OF MCVE PHP EXTENSION */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

