/* $Source$ */
/* $Id$ */

/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000, 2001 The PHP Group             |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: 								  |
   |  Initial version     by  Alex Barkov <bar@izhcom.ru>                 |
   |                      and Ramil Kalimullin <ram@izhcom.ru>            |
   |  Further development by  Sergey Kartashoff <gluke@biosys.net>        |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "php_mnogo.h"
#include "ext/standard/php_standard.h"
#include "ext/standard/info.h"
#include "php_globals.h"

#ifdef HAVE_MNOGOSEARCH

#define UDM_FIELD_URLID		1
#define UDM_FIELD_URL		2
#define UDM_FIELD_CONTENT	3	
#define UDM_FIELD_TITLE		4
#define UDM_FIELD_KEYWORDS	5
#define UDM_FIELD_DESC		6
#define UDM_FIELD_DESCRIPTION 	7
#define UDM_FIELD_TEXT		8
#define UDM_FIELD_SIZE		9
#define UDM_FIELD_SCORE		10
#define UDM_FIELD_RATING	11
#define UDM_FIELD_MODIFIED	12
#define UDM_FIELD_ORDER		13
#define UDM_FIELD_CRC		14

/* udm_set_agent_param constants */
#define UDM_PARAM_PAGE_SIZE	1
#define UDM_PARAM_PAGE_NUM	2
#define UDM_PARAM_SEARCH_MODE	3
#define UDM_PARAM_CACHE_MODE	4
#define UDM_PARAM_TRACK_MODE	5
#define UDM_PARAM_CHARSET	6
#define UDM_PARAM_STOPTABLE	7
#define UDM_PARAM_STOPFILE	8

/* udm_add_search_limit constants */
#define UDM_LIMIT_URL		1
#define UDM_LIMIT_TAG		2
#define UDM_LIMIT_LANG		3
#define UDM_LIMIT_CAT		4

#define UDM_TRACK_ENABLED	1
#define UDM_TRACK_DISABLED	0

/* udm_get_res_param constants */
#define UDM_PARAM_NUM_ROWS	256
#define UDM_PARAM_FOUND		257
#define UDM_PARAM_WORDINFO	258

/* True globals, no need for thread safety */
static int le_link,le_res;

#include <udmsearch.h>

function_entry mnogosearch_functions[] = {
	PHP_FE(udm_alloc_agent,		NULL)
	PHP_FE(udm_set_agent_param,	NULL)
	
	PHP_FE(udm_add_search_limit,	NULL)
	PHP_FE(udm_clear_search_limits,	NULL)
	
	PHP_FE(udm_errno,		NULL)
	PHP_FE(udm_error,		NULL)

	PHP_FE(udm_find,		NULL)
	PHP_FE(udm_get_res_param,	NULL)
	PHP_FE(udm_get_res_field,	NULL)
	
	PHP_FE(udm_free_res,		NULL)
	PHP_FE(udm_free_agent,		NULL)
	
	{NULL, NULL, NULL}
};


zend_module_entry mnogosearch_module_entry = {
	"mnogosearch", 
	mnogosearch_functions, 
	PHP_MINIT(mnogosearch), 
	PHP_MSHUTDOWN(mnogosearch), 
	PHP_RINIT(mnogosearch), 
	NULL,
	PHP_MINFO(mnogosearch), 
	STANDARD_MODULE_PROPERTIES
};


#ifdef COMPILE_DL_MNOGOSEARCH
ZEND_GET_MODULE(mnogosearch)
#endif

static void _free_udm_agent(zend_rsrc_list_entry *rsrc){
	UDM_AGENT * Agent = (UDM_AGENT *)rsrc->ptr;
	UdmFreeAgent(Agent);
}

static void _free_udm_res(zend_rsrc_list_entry *rsrc){
	UDM_RESULT * Res = (UDM_RESULT *)rsrc->ptr;
	UdmFreeResult(Res);	
}

DLEXPORT PHP_MINIT_FUNCTION(mnogosearch)
{
	UdmInit();
	le_link = zend_register_list_destructors_ex(_free_udm_agent,NULL,"mnogosearch agent",module_number);
	le_res = zend_register_list_destructors_ex(_free_udm_res,NULL,"mnogosearch result",module_number);

	REGISTER_LONG_CONSTANT("UDM_FIELD_URLID",	UDM_FIELD_URLID,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_FIELD_URL",		UDM_FIELD_URL,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_FIELD_CONTENT",	UDM_FIELD_CONTENT,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_FIELD_TITLE",	UDM_FIELD_TITLE,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_FIELD_KEYWORDS",	UDM_FIELD_KEYWORDS,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_FIELD_DESC",	UDM_FIELD_DESC,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_FIELD_DESCRIPTION",	UDM_FIELD_DESCRIPTION,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_FIELD_TEXT",	UDM_FIELD_TEXT,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_FIELD_SIZE",	UDM_FIELD_SIZE,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_FIELD_SCORE",	UDM_FIELD_SCORE,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_FIELD_RATING",	UDM_FIELD_RATING,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_FIELD_MODIFIED",	UDM_FIELD_MODIFIED,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_FIELD_ORDER",	UDM_FIELD_ORDER,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_FIELD_CRC",		UDM_FIELD_CRC,CONST_CS | CONST_PERSISTENT);

	/* udm_set_agent_param constants */
	REGISTER_LONG_CONSTANT("UDM_PARAM_PAGE_SIZE",	UDM_PARAM_PAGE_SIZE,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_PARAM_PAGE_NUM",	UDM_PARAM_PAGE_NUM,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_PARAM_SEARCH_MODE",	UDM_PARAM_SEARCH_MODE,CONST_CS | CONST_PERSISTENT);	
	REGISTER_LONG_CONSTANT("UDM_PARAM_CACHE_MODE",	UDM_PARAM_CACHE_MODE,CONST_CS | CONST_PERSISTENT);	
	REGISTER_LONG_CONSTANT("UDM_PARAM_TRACK_MODE",	UDM_PARAM_TRACK_MODE,CONST_CS | CONST_PERSISTENT);	
	REGISTER_LONG_CONSTANT("UDM_PARAM_CHARSET",	UDM_PARAM_CHARSET,CONST_CS | CONST_PERSISTENT);	
	REGISTER_LONG_CONSTANT("UDM_PARAM_STOPTABLE",	UDM_PARAM_STOPTABLE,CONST_CS | CONST_PERSISTENT);	
	REGISTER_LONG_CONSTANT("UDM_PARAM_STOPFILE",	UDM_PARAM_STOPFILE,CONST_CS | CONST_PERSISTENT);	
	
	/* udm_add_search_limit constants */
	REGISTER_LONG_CONSTANT("UDM_LIMIT_CAT",		UDM_LIMIT_CAT,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_LIMIT_URL",		UDM_LIMIT_URL,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_LIMIT_TAG",		UDM_LIMIT_TAG,CONST_CS | CONST_PERSISTENT);	
	REGISTER_LONG_CONSTANT("UDM_LIMIT_LANG",	UDM_LIMIT_LANG,CONST_CS | CONST_PERSISTENT);	
	
	/* udm_get_res_param constants */
	REGISTER_LONG_CONSTANT("UDM_PARAM_FOUND",	UDM_PARAM_FOUND,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_PARAM_NUM_ROWS",	UDM_PARAM_NUM_ROWS,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_PARAM_WORDINFO",	UDM_PARAM_WORDINFO,CONST_CS | CONST_PERSISTENT);

	/* search modes */
	REGISTER_LONG_CONSTANT("UDM_MODE_ALL",		UDM_MODE_ALL,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_MODE_ANY",		UDM_MODE_ANY,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_MODE_BOOL",		UDM_MODE_BOOL,CONST_CS | CONST_PERSISTENT);

	/* search cache params */
	REGISTER_LONG_CONSTANT("UDM_CACHE_ENABLED",	UDM_CACHE_ENABLED,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_CACHE_DISABLED",	UDM_CACHE_DISABLED,CONST_CS | CONST_PERSISTENT);
	
	/* track mode params */
	REGISTER_LONG_CONSTANT("UDM_TRACK_ENABLED",	UDM_TRACK_ENABLED,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_TRACK_DISABLED",	UDM_TRACK_DISABLED,CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}


DLEXPORT PHP_MSHUTDOWN_FUNCTION(mnogosearch)
{
	return SUCCESS;
}


DLEXPORT PHP_RINIT_FUNCTION(mnogosearch)
{
	return SUCCESS;
}


DLEXPORT PHP_MINFO_FUNCTION(mnogosearch)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "mnoGoSearch Support", "enabled" );
	php_info_print_table_end();
}


/* {{{ proto int udm_alloc_agent(string dbaddr [, string dbmode])
   Allocate mnoGoSearch session */
DLEXPORT PHP_FUNCTION(udm_alloc_agent)
{
	switch(ZEND_NUM_ARGS()){

		case 1: {
				pval **yydbaddr;
				char *dbaddr;
				UDM_ENV   * Env;
				UDM_AGENT * Agent;
				
				if(zend_get_parameters_ex(1,&yydbaddr)==FAILURE){
					RETURN_FALSE;
				}
				convert_to_string_ex(yydbaddr);
				dbaddr = (*yydbaddr)->value.str.val;
				
				Env=UdmAllocEnv();
				UdmEnvSetDBAddr(Env,dbaddr);
				Agent=UdmAllocAgent(Env,0,UDM_OPEN_MODE_READ);
				
				ZEND_REGISTER_RESOURCE(return_value,Agent,le_link);
			}
			break;
			
		case 2: {
				pval **yydbaddr;
				pval **yydbmode;
				char *dbaddr;
				char *dbmode;
				UDM_ENV   * Env;
				UDM_AGENT * Agent;
				
				if(zend_get_parameters_ex(2,&yydbaddr,&yydbmode)==FAILURE){
					RETURN_FALSE;
				}
				convert_to_string_ex(yydbaddr);
				convert_to_string_ex(yydbmode);
				dbaddr = (*yydbaddr)->value.str.val;
				dbmode = (*yydbmode)->value.str.val;
				
				Env=UdmAllocEnv();				
				UdmEnvSetDBAddr(Env,dbaddr);
				UdmEnvSetDBMode(Env,dbmode);				
				Agent=UdmAllocAgent(Env,0,UDM_OPEN_MODE_READ);				
				
				ZEND_REGISTER_RESOURCE(return_value,Agent,le_link);
			}
			break;
			
		default:
			WRONG_PARAM_COUNT;
			break;
	}
}
/* }}} */


/* {{{ proto int udm_set_agent_param(int agent, int var, string val)
   Set mnoGoSearch agent session parameters */
DLEXPORT PHP_FUNCTION(udm_set_agent_param)
{
	pval **yyagent, **yyvar, **yyval;
	char *val;
	int var;
	UDM_AGENT * Agent;

	switch(ZEND_NUM_ARGS()){
	
		case 3: 		
			if(zend_get_parameters_ex(3,&yyagent,&yyvar,&yyval)==FAILURE){
				RETURN_FALSE;
			}
			convert_to_long_ex(yyvar);
			convert_to_string_ex(yyval);
			ZEND_FETCH_RESOURCE(Agent, UDM_AGENT *, yyagent, -1, "mnoGoSearch-agent", le_link);
			var = (*yyvar)->value.lval;
			val = (*yyval)->value.str.val;
			
			break;
			
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	switch(var){
		case UDM_PARAM_PAGE_SIZE: 
			Agent->page_size=atoi(val);
			if(Agent->page_size<1)Agent->page_size=20;
			
			break;
			
		case UDM_PARAM_PAGE_NUM: 
			Agent->page_number=atoi(val);
			if(Agent->page_number<0)Agent->page_number=0;
			
			break;
			
		case UDM_PARAM_SEARCH_MODE:
			switch (atoi(val)){
					case UDM_MODE_ALL:
						Agent->search_mode=UDM_MODE_ALL;
						break;
						
					case UDM_MODE_ANY:
						Agent->search_mode=UDM_MODE_ANY;
						break;
						
					case UDM_MODE_BOOL: 
						Agent->search_mode=UDM_MODE_BOOL;
						break;
						
					default:
						php_error(E_WARNING,"Udm_Set_Agent_Param: Unknown search mode");
						RETURN_FALSE;
						break;
			}
			
			break;
			
		case UDM_PARAM_CACHE_MODE: 
			switch (atoi(val)){
				case UDM_CACHE_ENABLED:
					Agent->cache_mode=UDM_CACHE_ENABLED;
					break;
					
				case UDM_CACHE_DISABLED:
					Agent->cache_mode=UDM_CACHE_DISABLED;
					break;
					
				default:
					Agent->cache_mode=UDM_CACHE_DISABLED;
					php_error(E_WARNING,"Udm_Set_Agent_Param: Unknown cache mode");
					RETURN_FALSE;
					break;
			}
			
			break;
			
		case UDM_PARAM_TRACK_MODE: 
			switch (atoi(val)){
				case UDM_TRACK_ENABLED:
					Agent->track_mode|=UDM_TRACK_QUERIES;
					break;
					
				case UDM_TRACK_DISABLED:
					Agent->track_mode &= ~(UDM_TRACK_QUERIES);    
					break;
					
				default:
					php_error(E_WARNING,"Udm_Set_Agent_Param: Unknown track mode");
					RETURN_FALSE;
					break;
			}
			
			break;
			
		case UDM_PARAM_CHARSET:
			Agent->Conf->local_charset=UdmGetCharset(val);
			Agent->charset=Agent->Conf->local_charset;

			break;
			
		case UDM_PARAM_STOPTABLE:
			strcat(Agent->Conf->stop_tables," ");
			strcat(Agent->Conf->stop_tables,val);

			break;

		case UDM_PARAM_STOPFILE: 
			if (UdmFileLoadStopList(Agent->Conf,val)) {
				php_error(E_WARNING,Agent->Conf->errstr);
				RETURN_FALSE;
			}
			    
			break;
			
		default:
			php_error(E_WARNING,"Udm_Set_Agent_Param: Unknown agent session parameter");
			RETURN_FALSE;
			break;
	}
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto int udm_add_search_limit(int agent, int var, string val)
   Add mnoGoSearch search restrictions */
DLEXPORT PHP_FUNCTION(udm_add_search_limit)
{
	pval **yyagent, **yyvar, **yyval;
	char *val;
	int var;
	UDM_AGENT * Agent;

	switch(ZEND_NUM_ARGS()){
	
		case 3: 		
			if(zend_get_parameters_ex(3,&yyagent,&yyvar,&yyval)==FAILURE){
				RETURN_FALSE;
			}
			convert_to_long_ex(yyvar);
			convert_to_string_ex(yyval);
			ZEND_FETCH_RESOURCE(Agent, UDM_AGENT *, yyagent, -1, "mnoGoSearch-agent", le_link);
			var = (*yyvar)->value.lval;
			val = (*yyval)->value.str.val;
			
			break;
			
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	switch(var){
		case UDM_LIMIT_URL: 
			UdmAddURLLimit(Agent->Conf,val);
		
			break;
			
		case UDM_LIMIT_TAG: 
			UdmAddTagLimit(Agent->Conf,val);
		
			break;

		case UDM_LIMIT_LANG: 
			UdmAddLangLimit(Agent->Conf,val);
			
			break;

		case UDM_LIMIT_CAT: 
			UdmAddCatLimit(Agent->Conf,val);
			
			break;
			
		default:
			php_error(E_WARNING,"Udm_Add_Search_Limit: Unknown search limit parameter");
			RETURN_FALSE;
			break;
	}
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto int udm_clear_search_limits(int agent)
   Clear all mnoGoSearch search restrictions */
DLEXPORT PHP_FUNCTION(udm_clear_search_limits)
{
	pval ** yyagent;
	UDM_AGENT * Agent;
	switch(ZEND_NUM_ARGS()){
		case 1: {
				if (zend_get_parameters_ex(1, &yyagent)==FAILURE) {
					RETURN_FALSE;
				}
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(Agent, UDM_AGENT *, yyagent, -1, "mnoGoSearch-Agent", le_link);
	
	UdmClearLimits(Agent->Conf);
	
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto int udm_find(int agent, string query)
   Perform search */
DLEXPORT PHP_FUNCTION(udm_find)
{
	pval ** yyquery, ** yyagent;
	UDM_RESULT * Res;
	UDM_AGENT * Agent;
	int id=-1;

	switch(ZEND_NUM_ARGS()){
		case 2: {
				if (zend_get_parameters_ex(2, &yyagent,&yyquery)==FAILURE) {
					RETURN_FALSE;
				}
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(Agent, UDM_AGENT *, yyagent, id, "mnoGoSearch-Agent", le_link);
	convert_to_string_ex(yyquery);
	
	if (Res=UdmFind(Agent,UdmTolower((*yyquery)->value.str.val,Agent->charset))) {
	    ZEND_REGISTER_RESOURCE(return_value,Res,le_res);
	} else {
	    RETURN_FALSE;
	}	
}
/* }}} */


/* {{{ proto string udm_get_res_field(int res, int row, int field)
   Fetch mnoGoSearch result field */
DLEXPORT PHP_FUNCTION(udm_get_res_field){
	pval **yyres, **yyrow_num, **yyfield_name;

	UDM_RESULT * Res;
	int row,field;
	
	switch(ZEND_NUM_ARGS()){
		case 3: {
				if (zend_get_parameters_ex(3, &yyres,&yyrow_num,&yyfield_name)==FAILURE){
					RETURN_FALSE;
				}
				convert_to_string_ex(yyrow_num);
				convert_to_string_ex(yyfield_name);
				field=atoi((*yyfield_name)->value.str.val);
				row=atoi((*yyrow_num)->value.str.val);
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(Res, UDM_RESULT *, yyres, -1, "mnoGoSearch-Result", le_res);
	if(row<Res->num_rows){
		switch(field){
			case UDM_FIELD_URL: 		
				RETURN_STRING((Res->Doc[row].url),1);
				break;
				
			case UDM_FIELD_CONTENT: 	
				RETURN_STRING((Res->Doc[row].content_type),1);
				break;
				
			case UDM_FIELD_TITLE:		
				RETURN_STRING((Res->Doc[row].title),1);
				break;
				
			case UDM_FIELD_KEYWORDS:	
				RETURN_STRING((Res->Doc[row].keywords),1);
				break;
				
			case UDM_FIELD_DESC:		
			case UDM_FIELD_DESCRIPTION:		
				RETURN_STRING((Res->Doc[row].description),1);
				break;
				
			case UDM_FIELD_TEXT:		
				RETURN_STRING((Res->Doc[row].text),1);
				break;
				
			case UDM_FIELD_SIZE:		
				RETURN_LONG((Res->Doc[row].size));
				break;
				
			case UDM_FIELD_URLID:		
				RETURN_LONG((Res->Doc[row].url_id));
				break;
				
			case UDM_FIELD_SCORE:		
			case UDM_FIELD_RATING:		
				RETURN_LONG((Res->Doc[row].rating));
				break;
				
			case UDM_FIELD_MODIFIED:	
				RETURN_LONG((Res->Doc[row].last_mod_time));
				break;

			case UDM_FIELD_ORDER:	
				RETURN_LONG((Res->Doc[row].order));
				break;
				
			case UDM_FIELD_CRC:	
				RETURN_LONG((Res->Doc[row].crc32));
				break;
				
			default: 
				php_error(E_WARNING,"Udm_Get_Res_Field: Unknown mnoGoSearch field name");
				RETURN_FALSE;
				break;
		}
	}else{
		php_error(E_WARNING,"Udm_Get_Res_Field: row number too large");
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto string udm_get_res_param(int res, int param)
   Get mnoGoSearch result parameters */
DLEXPORT PHP_FUNCTION(udm_get_res_param)
{
	pval ** yyres, ** yyparam;
	int param;
	UDM_RESULT * Res;
	switch(ZEND_NUM_ARGS()){
		case 2: {
				if (zend_get_parameters_ex(2, &yyres, &yyparam)==FAILURE) {
					RETURN_FALSE;
				}
				convert_to_long_ex(yyparam);
				param=((*yyparam)->value.lval);
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(Res, UDM_RESULT *, yyres, -1, "mnoGoSearch-Result", le_res);
	switch(param){
		case UDM_PARAM_NUM_ROWS: 	
			RETURN_LONG(Res->num_rows);
			break;
		
		case UDM_PARAM_FOUND:	 	
			RETURN_LONG(Res->total_found);
			break;
		
		case UDM_PARAM_WORDINFO: 	
			RETURN_STRING(Res->wordinfo,1);
			break;

		default:
			php_error(E_WARNING,"Udm_Get_Res_Param: Unknown mnoGoSearch param name");
			RETURN_FALSE;
			break;
	}
}
/* }}} */


/* {{{ proto int udm_free_res(int res)
   mnoGoSearch free result */
DLEXPORT PHP_FUNCTION(udm_free_res)
{
	pval ** yyres;
	UDM_RESULT * Res;
	switch(ZEND_NUM_ARGS()){
		case 1: {
				if (zend_get_parameters_ex(1, &yyres)==FAILURE) {
					RETURN_FALSE;
				}
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(Res, UDM_RESULT *, yyres, -1, "mnoGoSearch-Result", le_res);
	zend_list_delete((*yyres)->value.lval);
	
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto int udm_errno(int agent)
   mnoGoSearch error number */
DLEXPORT PHP_FUNCTION(udm_errno)
{
	pval ** yyagent;
	UDM_AGENT * Agent;
	switch(ZEND_NUM_ARGS()){
		case 1: {
				if (zend_get_parameters_ex(1, &yyagent)==FAILURE) {
					RETURN_FALSE;
				}
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(Agent, UDM_AGENT *, yyagent, -1, "mnoGoSearch-Agent", le_link);
	RETURN_LONG(UdmDBErrorCode(Agent->db));
}
/* }}} */


/* {{{ proto string udm_error(int agent)
   mnoGoSearch error message */
DLEXPORT PHP_FUNCTION(udm_error)
{
	pval ** yyagent;
	UDM_AGENT * Agent;
	
	switch(ZEND_NUM_ARGS()){
		case 1: {
				if (zend_get_parameters_ex(1, &yyagent)==FAILURE) {
					RETURN_FALSE;
				}
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(Agent, UDM_AGENT *, yyagent, -1, "mnoGoSearch-Agent", le_link);
	RETURN_STRING(UdmDBErrorMsg(Agent->db),1);
}
/* }}} */


/* {{{ proto int udm_free_agent(int agent)
   Free mnoGoSearch session */
DLEXPORT PHP_FUNCTION(udm_free_agent)
{
	pval ** yyagent;
	UDM_RESULT * Agent;
	switch(ZEND_NUM_ARGS()){
		case 1: {
				if (zend_get_parameters_ex(1, &yyagent)==FAILURE) {
					RETURN_FALSE;
				}
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(Agent, UDM_RESULT *, yyagent, -1, "mnoGoSearch-agent", le_link);
	zend_list_delete((*yyagent)->value.lval);
	
	RETURN_TRUE;
}
/* }}} */


#endif


/*
 * Local variables:
 * End:
 */

