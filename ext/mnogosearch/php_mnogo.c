/* $Source$ */
/* $Id$ */

/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors:                                                             |
   |  Initial version     by  Alex Barkov <bar@izhcom.ru>                 |
   |                      and Ramil Kalimullin <ram@izhcom.ru>            |
   |  Further development by  Sergey Kartashoff <gluke@mail.ru>           |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_mnogo.h"
#include "ext/standard/php_standard.h"
#include "ext/standard/info.h"
#include "php_globals.h"

#ifdef HAVE_MNOGOSEARCH

#define UDMSTRSIZ               1024*5

#define UDM_FIELD_URLID		1
#define UDM_FIELD_URL		2
#define UDM_FIELD_CONTENT	3	
#define UDM_FIELD_TITLE		4
#define UDM_FIELD_KEYWORDS	5
#define UDM_FIELD_DESC		6
#define UDM_FIELD_TEXT		7
#define UDM_FIELD_SIZE		8
#define UDM_FIELD_RATING	9
#define UDM_FIELD_MODIFIED	10
#define UDM_FIELD_ORDER		11
#define UDM_FIELD_CRC		12
#define UDM_FIELD_CATEGORY	13
#define UDM_FIELD_LANG		14
#define UDM_FIELD_CHARSET	15
#define UDM_FIELD_SITEID	16
#define UDM_FIELD_POP_RANK	17
#define UDM_FIELD_ORIGINID	18

/* udm_set_agent_param constants */
#define UDM_PARAM_PAGE_SIZE		1
#define UDM_PARAM_PAGE_NUM		2
#define UDM_PARAM_SEARCH_MODE		3
#define UDM_PARAM_CACHE_MODE		4
#define UDM_PARAM_TRACK_MODE		5
#define UDM_PARAM_CHARSET		6
#define UDM_PARAM_STOPTABLE		7
#define UDM_PARAM_STOPFILE		8
#define UDM_PARAM_WEIGHT_FACTOR		9
#define UDM_PARAM_WORD_MATCH		10
#define UDM_PARAM_PHRASE_MODE		11
#define UDM_PARAM_MIN_WORD_LEN		12
#define UDM_PARAM_MAX_WORD_LEN		13
#define UDM_PARAM_ISPELL_PREFIXES	14
#define UDM_PARAM_CROSS_WORDS		15
#define UDM_PARAM_VARDIR		16
#define UDM_PARAM_LOCAL_CHARSET		17
#define UDM_PARAM_BROWSER_CHARSET	18
#define UDM_PARAM_HLBEG			19
#define UDM_PARAM_HLEND			20
#define UDM_PARAM_SYNONYM		21
#define UDM_PARAM_SEARCHD		22
#define UDM_PARAM_QSTRING		23
#define UDM_PARAM_REMOTE_ADDR		24
#define UDM_PARAM_QUERY			25
#define UDM_PARAM_STORED		26
#define UDM_PARAM_GROUPBYSITE		27
#define UDM_PARAM_SITEID		28
#define UDM_PARAM_DETECT_CLONES		29

/* udm_add_search_limit constants */
#define UDM_LIMIT_URL		1
#define UDM_LIMIT_TAG		2
#define UDM_LIMIT_LANG		3
#define UDM_LIMIT_CAT		4
#define UDM_LIMIT_DATE		5
#define UDM_LIMIT_TYPE		6

/* word match type */
#define UDM_MATCH_WORD		0
#define UDM_MATCH_BEGIN		1
#define UDM_MATCH_SUBSTR	2
#define UDM_MATCH_END		3

/* track modes */
#define UDM_TRACK_ENABLED	1
#define UDM_TRACK_DISABLED	0

/* cache modes */
#define UDM_CACHE_ENABLED	1
#define UDM_CACHE_DISABLED	0

/* phrase modes */
#define UDM_PHRASE_ENABLED	1
#define UDM_PHRASE_DISABLED	0

/* prefix modes */
#define UDM_PREFIXES_ENABLED	1
#define UDM_PREFIXES_DISABLED	0

/* crosswords modes */
#define UDM_CROSS_WORDS_ENABLED	 1
#define UDM_CROSS_WORDS_DISABLED 0

/* General modes */
#define UDM_ENABLED	 	1
#define UDM_DISABLED 		0

/* udm_get_res_param constants */
#define UDM_PARAM_NUM_ROWS	256
#define UDM_PARAM_FOUND		257
#define UDM_PARAM_WORDINFO	258
#define UDM_PARAM_SEARCHTIME	259
#define UDM_PARAM_FIRST_DOC	260
#define UDM_PARAM_LAST_DOC	261
#define UDM_PARAM_WORDINFO_ALL	262

/* udm_load_ispell_data constants */
#define UDM_ISPELL_TYPE_AFFIX	1
#define UDM_ISPELL_TYPE_SPELL	2
#define UDM_ISPELL_TYPE_DB	3
#define UDM_ISPELL_TYPE_SERVER	4

/* True globals, no need for thread safety */
static int le_link,le_res;

#include <udm_config.h>
#include <udmsearch.h>

/* {{{ mnogosearch_functions[]
 */
function_entry mnogosearch_functions[] = {
	PHP_FE(udm_api_version,		NULL)

#if UDM_VERSION_ID >= 30200			
	PHP_FE(udm_check_charset,	NULL)
#if UDM_VERSION_ID == 30203
	PHP_FE(udm_open_stored,	NULL)
	PHP_FE(udm_check_stored,NULL)
	PHP_FE(udm_close_stored,NULL)
#endif
#if UDM_VERSION_ID >= 30203
	PHP_FE(udm_crc32,	NULL)
#endif
#if UDM_VERSION_ID >= 30204
	PHP_FE(udm_parse_query_string,	NULL)
	PHP_FE(udm_make_excerpt,	NULL)
	PHP_FE(udm_set_agent_param_ex,	NULL)
	PHP_FE(udm_get_res_field_ex,	NULL)
#endif
#endif

	PHP_FE(udm_alloc_agent,		NULL)
	PHP_FE(udm_set_agent_param,	NULL)
	
	PHP_FE(udm_load_ispell_data,	NULL)
	PHP_FE(udm_free_ispell_data,	NULL)
	
	PHP_FE(udm_add_search_limit,	NULL)
	PHP_FE(udm_clear_search_limits,	NULL)
	
	PHP_FE(udm_errno,		NULL)
	PHP_FE(udm_error,		NULL)

	PHP_FE(udm_find,		NULL)
	PHP_FE(udm_get_res_param,	NULL)
	PHP_FE(udm_get_res_field,	NULL)
	
	PHP_FE(udm_cat_list,		NULL)
	PHP_FE(udm_cat_path,		NULL)
	
	PHP_FE(udm_free_res,		NULL)
	PHP_FE(udm_free_agent,		NULL)

#if UDM_VERSION_ID > 30110
	PHP_FE(udm_get_doc_count,	NULL)
#endif
	
	{NULL, NULL, NULL}
};
/* }}} */

zend_module_entry mnogosearch_module_entry = {
	STANDARD_MODULE_HEADER,
	"mnogosearch", 
	mnogosearch_functions, 
	PHP_MINIT(mnogosearch), 
	PHP_MSHUTDOWN(mnogosearch), 
	PHP_RINIT(mnogosearch), 
	NULL,
	PHP_MINFO(mnogosearch), 
    NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};


#ifdef COMPILE_DL_MNOGOSEARCH
ZEND_GET_MODULE(mnogosearch)
#endif

static void _free_udm_agent(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	UDM_AGENT * Agent = (UDM_AGENT *)rsrc->ptr;
#if UDM_VERSION_ID >= 30204
	UdmEnvFree(Agent->Conf);
	UdmAgentFree(Agent);
#else
	UdmFreeEnv(Agent->Conf);
	UdmFreeAgent(Agent);
#endif
}

static void _free_udm_res(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	UDM_RESULT * Res = (UDM_RESULT *)rsrc->ptr;

#if UDM_VERSION_ID >= 30204	
	UdmResultFree(Res);
#else
	UdmFreeResult(Res);
#endif
}

/* {{{ PHP_MINIT_FUNCTION
 */
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
	REGISTER_LONG_CONSTANT("UDM_FIELD_DESCRIPTION",	UDM_FIELD_DESC,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_FIELD_TEXT",	UDM_FIELD_TEXT,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_FIELD_SIZE",	UDM_FIELD_SIZE,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_FIELD_RATING",	UDM_FIELD_RATING,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_FIELD_SCORE",	UDM_FIELD_RATING,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_FIELD_MODIFIED",	UDM_FIELD_MODIFIED,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_FIELD_ORDER",	UDM_FIELD_ORDER,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_FIELD_CRC",		UDM_FIELD_CRC,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_FIELD_CATEGORY",	UDM_FIELD_CATEGORY,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_FIELD_LANG",	UDM_FIELD_LANG,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_FIELD_CHARSET",	UDM_FIELD_CHARSET,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_FIELD_SITEID",	UDM_FIELD_SITEID,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_FIELD_POP_RANK",	UDM_FIELD_POP_RANK,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_FIELD_ORIGINID",	UDM_FIELD_ORIGINID,CONST_CS | CONST_PERSISTENT);

	/* udm_set_agent_param constants */
	REGISTER_LONG_CONSTANT("UDM_PARAM_PAGE_SIZE",	UDM_PARAM_PAGE_SIZE,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_PARAM_PAGE_NUM",	UDM_PARAM_PAGE_NUM,CONST_CS | CONST_PERSISTENT);
	
	REGISTER_LONG_CONSTANT("UDM_PARAM_SEARCH_MODE",	UDM_PARAM_SEARCH_MODE,CONST_CS | CONST_PERSISTENT);	
	REGISTER_LONG_CONSTANT("UDM_PARAM_CACHE_MODE",	UDM_PARAM_CACHE_MODE,CONST_CS | CONST_PERSISTENT);	
	REGISTER_LONG_CONSTANT("UDM_PARAM_TRACK_MODE",	UDM_PARAM_TRACK_MODE,CONST_CS | CONST_PERSISTENT);	
	REGISTER_LONG_CONSTANT("UDM_PARAM_PHRASE_MODE",	UDM_PARAM_PHRASE_MODE,CONST_CS | CONST_PERSISTENT);	
	
	REGISTER_LONG_CONSTANT("UDM_PARAM_CHARSET",	UDM_PARAM_CHARSET,CONST_CS | CONST_PERSISTENT);	
	REGISTER_LONG_CONSTANT("UDM_PARAM_LOCAL_CHARSET",UDM_PARAM_LOCAL_CHARSET,CONST_CS | CONST_PERSISTENT);	
	REGISTER_LONG_CONSTANT("UDM_PARAM_BROWSER_CHARSET",UDM_PARAM_BROWSER_CHARSET,CONST_CS | CONST_PERSISTENT);	
	
	REGISTER_LONG_CONSTANT("UDM_PARAM_STOPTABLE",	UDM_PARAM_STOPTABLE,CONST_CS | CONST_PERSISTENT);	
	REGISTER_LONG_CONSTANT("UDM_PARAM_STOP_TABLE",	UDM_PARAM_STOPTABLE,CONST_CS | CONST_PERSISTENT);	
	REGISTER_LONG_CONSTANT("UDM_PARAM_STOPFILE",	UDM_PARAM_STOPFILE,CONST_CS | CONST_PERSISTENT);	
	REGISTER_LONG_CONSTANT("UDM_PARAM_STOP_FILE",	UDM_PARAM_STOPFILE,CONST_CS | CONST_PERSISTENT);	
	
	REGISTER_LONG_CONSTANT("UDM_PARAM_WEIGHT_FACTOR",UDM_PARAM_WEIGHT_FACTOR,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_PARAM_WORD_MATCH",  UDM_PARAM_WORD_MATCH,CONST_CS | CONST_PERSISTENT);	
	
	REGISTER_LONG_CONSTANT("UDM_PARAM_MAX_WORD_LEN",UDM_PARAM_MAX_WORD_LEN,CONST_CS | CONST_PERSISTENT);	
	REGISTER_LONG_CONSTANT("UDM_PARAM_MAX_WORDLEN", UDM_PARAM_MAX_WORD_LEN,CONST_CS | CONST_PERSISTENT);	
	REGISTER_LONG_CONSTANT("UDM_PARAM_MIN_WORD_LEN",UDM_PARAM_MIN_WORD_LEN,CONST_CS | CONST_PERSISTENT);	
	REGISTER_LONG_CONSTANT("UDM_PARAM_MIN_WORDLEN", UDM_PARAM_MIN_WORD_LEN,CONST_CS | CONST_PERSISTENT);	
	
	REGISTER_LONG_CONSTANT("UDM_PARAM_ISPELL_PREFIXES",UDM_PARAM_ISPELL_PREFIXES,CONST_CS | CONST_PERSISTENT);	
	REGISTER_LONG_CONSTANT("UDM_PARAM_ISPELL_PREFIX",UDM_PARAM_ISPELL_PREFIXES,CONST_CS | CONST_PERSISTENT);	
	REGISTER_LONG_CONSTANT("UDM_PARAM_PREFIXES",	UDM_PARAM_ISPELL_PREFIXES,CONST_CS | CONST_PERSISTENT);	
	REGISTER_LONG_CONSTANT("UDM_PARAM_PREFIX",	UDM_PARAM_ISPELL_PREFIXES,CONST_CS | CONST_PERSISTENT);
		
	REGISTER_LONG_CONSTANT("UDM_PARAM_CROSS_WORDS",	UDM_PARAM_CROSS_WORDS,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_PARAM_CROSSWORDS",	UDM_PARAM_CROSS_WORDS,CONST_CS | CONST_PERSISTENT);	
	
	REGISTER_LONG_CONSTANT("UDM_PARAM_VARDIR",	UDM_PARAM_VARDIR,CONST_CS | CONST_PERSISTENT);	
	REGISTER_LONG_CONSTANT("UDM_PARAM_DATADIR",	UDM_PARAM_VARDIR,CONST_CS | CONST_PERSISTENT);	
	
	REGISTER_LONG_CONSTANT("UDM_PARAM_HLBEG",	UDM_PARAM_HLBEG,CONST_CS | CONST_PERSISTENT);	
	REGISTER_LONG_CONSTANT("UDM_PARAM_HLEND",	UDM_PARAM_HLEND,CONST_CS | CONST_PERSISTENT);	
	
	REGISTER_LONG_CONSTANT("UDM_PARAM_SYNONYM",	UDM_PARAM_SYNONYM,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_PARAM_SEARCHD",	UDM_PARAM_SEARCHD,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_PARAM_STORED",	UDM_PARAM_STORED,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_PARAM_GROUPBYSITE",	UDM_PARAM_GROUPBYSITE,CONST_CS | CONST_PERSISTENT);
	
	REGISTER_LONG_CONSTANT("UDM_PARAM_QSTRING",	UDM_PARAM_QSTRING,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_PARAM_REMOTE_ADDR",	UDM_PARAM_REMOTE_ADDR,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_PARAM_QUERY",	UDM_PARAM_QUERY,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_PARAM_SITEID",	UDM_PARAM_SITEID,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_PARAM_DETECT_CLONES",UDM_PARAM_DETECT_CLONES,CONST_CS | CONST_PERSISTENT);
	
	/* udm_add_search_limit constants */
	REGISTER_LONG_CONSTANT("UDM_LIMIT_CAT",		UDM_LIMIT_CAT,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_LIMIT_URL",		UDM_LIMIT_URL,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_LIMIT_TAG",		UDM_LIMIT_TAG,CONST_CS | CONST_PERSISTENT);	
	REGISTER_LONG_CONSTANT("UDM_LIMIT_LANG",	UDM_LIMIT_LANG,CONST_CS | CONST_PERSISTENT);	
	REGISTER_LONG_CONSTANT("UDM_LIMIT_DATE",	UDM_LIMIT_DATE,CONST_CS | CONST_PERSISTENT);	
	REGISTER_LONG_CONSTANT("UDM_LIMIT_TYPE",	UDM_LIMIT_TYPE,CONST_CS | CONST_PERSISTENT);	
	
	/* udm_get_res_param constants */
	REGISTER_LONG_CONSTANT("UDM_PARAM_FOUND",	UDM_PARAM_FOUND,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_PARAM_NUM_ROWS",	UDM_PARAM_NUM_ROWS,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_PARAM_WORDINFO",	UDM_PARAM_WORDINFO,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_PARAM_WORDINFO_ALL",UDM_PARAM_WORDINFO_ALL,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_PARAM_WORD_INFO",	UDM_PARAM_WORDINFO,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_PARAM_SEARCHTIME",	UDM_PARAM_SEARCHTIME,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_PARAM_SEARCH_TIME",	UDM_PARAM_SEARCHTIME,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_PARAM_FIRST_DOC",	UDM_PARAM_FIRST_DOC,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_PARAM_LAST_DOC",	UDM_PARAM_LAST_DOC,CONST_CS | CONST_PERSISTENT);

	/* search modes */
	REGISTER_LONG_CONSTANT("UDM_MODE_ALL",		UDM_MODE_ALL,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_MODE_ANY",		UDM_MODE_ANY,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_MODE_BOOL",		UDM_MODE_BOOL,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_MODE_PHRASE",	UDM_MODE_PHRASE,CONST_CS | CONST_PERSISTENT);

	/* search cache params */
	REGISTER_LONG_CONSTANT("UDM_CACHE_ENABLED",	UDM_ENABLED,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_CACHE_DISABLED",	UDM_DISABLED,CONST_CS | CONST_PERSISTENT);
	
	/* track mode params */
	REGISTER_LONG_CONSTANT("UDM_TRACK_ENABLED",	UDM_ENABLED,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_TRACK_DISABLED",	UDM_DISABLED,CONST_CS | CONST_PERSISTENT);
	
	/* phrase mode params */
	REGISTER_LONG_CONSTANT("UDM_PHRASE_ENABLED",	UDM_ENABLED,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_PHRASE_DISABLED",	UDM_DISABLED,CONST_CS | CONST_PERSISTENT);

	/* general params */
	REGISTER_LONG_CONSTANT("UDM_ENABLED",		UDM_ENABLED,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_DISABLED",		UDM_DISABLED,CONST_CS | CONST_PERSISTENT);
	
	/* crosswords mode params */
	REGISTER_LONG_CONSTANT("UDM_CROSS_WORDS_ENABLED",UDM_ENABLED,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_CROSSWORDS_ENABLED",UDM_ENABLED,CONST_CS | CONST_PERSISTENT);	
	REGISTER_LONG_CONSTANT("UDM_CROSS_WORDS_DISABLED",UDM_DISABLED,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_CROSSWORDS_DISABLED",UDM_DISABLED,CONST_CS | CONST_PERSISTENT);
	
	/* prefixes mode params */
	REGISTER_LONG_CONSTANT("UDM_PREFIXES_ENABLED",	UDM_ENABLED,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_PREFIX_ENABLED",	UDM_ENABLED,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_ISPELL_PREFIXES_ENABLED",UDM_ENABLED,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_ISPELL_PREFIX_ENABLED",UDM_ENABLED,CONST_CS | CONST_PERSISTENT);
	
	REGISTER_LONG_CONSTANT("UDM_PREFIXES_DISABLED",	UDM_DISABLED,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_PREFIX_DISABLED",	UDM_DISABLED,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_ISPELL_PREFIXES_DISABLED",UDM_DISABLED,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_ISPELL_PREFIX_DISABLED",UDM_DISABLED,CONST_CS | CONST_PERSISTENT);
	
	/* ispell type params */
	REGISTER_LONG_CONSTANT("UDM_ISPELL_TYPE_AFFIX",	UDM_ISPELL_TYPE_AFFIX,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_ISPELL_TYPE_SPELL",	UDM_ISPELL_TYPE_SPELL,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_ISPELL_TYPE_DB",	UDM_ISPELL_TYPE_DB,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_ISPELL_TYPE_SERVER",UDM_ISPELL_TYPE_SERVER,CONST_CS | CONST_PERSISTENT);

	/* word match mode params */
	REGISTER_LONG_CONSTANT("UDM_MATCH_WORD",	UDM_MATCH_WORD,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_MATCH_BEGIN",	UDM_MATCH_BEGIN,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_MATCH_SUBSTR",	UDM_MATCH_SUBSTR,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDM_MATCH_END",		UDM_MATCH_END,CONST_CS | CONST_PERSISTENT);
	
	return SUCCESS;
}
/* }}} */

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
	char buf[32];
	
	php_info_print_table_start();
	php_info_print_table_row(2, "mnoGoSearch Support", "enabled" );
	
	sprintf(buf,"%d", UDM_VERSION_ID);
	php_info_print_table_row(2, "mnoGoSearch library version", buf );
	php_info_print_table_end();
}

static char* MyRemoveHiLightDup(const char *s){
	size_t	len=strlen(s)+1;
	char	*res=malloc(len);
	char	*d;
	
	for(d=res;s[0];s++){
		switch(s[0]){
			case '\2':
			case '\3':
				break;
			default:
				*d++=*s;
		}
	}
	*d='\0';
	return res;
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
				dbaddr = Z_STRVAL_PP(yydbaddr);
			
#if UDM_VERSION_ID >= 30204
				Env=UdmEnvInit(NULL);
				UdmVarListReplaceStr(&Env->Vars,"SyslogFacility","local7");
				UdmSetLogLevel(Env,0);
				UdmOpenLog("mnoGoSearch-php",Env,0);

				if(!memcmp(dbaddr,"searchd:",8)){
					UDM_URL	Url;
					UdmURLParse(&Url,dbaddr);
					UdmDBListAdd(&Env->sdcl,Url.hostinfo);
				}
				
				UdmVarListReplaceStr(&Env->Vars,"DBAddr",dbaddr);
				if(UDM_OK!=UdmDBSetAddr(Env->db,dbaddr,UDM_OPEN_MODE_READ)){
				    sprintf(Env->errstr,"Invalid DBAddr: '%s'",dbaddr);
				    Env->errcode=1;
				    php_error(E_WARNING,"%s(): Invalid DBAddr", get_active_function_name(TSRMLS_C));
				    RETURN_FALSE;
				}
				
				Agent=UdmAgentInit(NULL,Env,0);
#elif UDM_VERSION_ID >= 30200
				Env=UdmAllocEnv();
				Env->vars=UdmAllocVarList();
				Env->DBAddr=strdup(dbaddr);
				UdmEnvSetDBMode(Env,"single");
				Agent=UdmAllocAgent(Env,0,UDM_OPEN_MODE_READ);
#else				
				Env=UdmAllocEnv();
				UdmEnvSetDBAddr(Env,dbaddr);
				Agent=UdmAllocAgent(Env,0,UDM_OPEN_MODE_READ);
#endif							
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
				dbaddr = Z_STRVAL_PP(yydbaddr);
				dbmode = Z_STRVAL_PP(yydbmode);
				
#if UDM_VERSION_ID >= 30204
				Env=UdmEnvInit(NULL);
				UdmVarListReplaceStr(&Env->Vars,"SyslogFacility","local7");
				UdmSetLogLevel(Env,0);
				UdmOpenLog("mnoGoSearch-php",Env,0);

				if(!memcmp(dbaddr,"searchd:",8)){
					UDM_URL	Url;
					UdmURLParse(&Url,dbaddr);
					UdmDBListAdd(&Env->sdcl,Url.hostinfo);
				}
				UdmVarListReplaceStr(&Env->Vars,"DBAddr",dbaddr);
				if(UDM_OK!=UdmDBSetAddr(Env->db,dbaddr,UDM_OPEN_MODE_READ)){
				    sprintf(Env->errstr,"Invalid DBAddr: '%s'",dbaddr);
				    Env->errcode=1;
				    php_error(E_WARNING,"%s(): Invalid DBAddr", get_active_function_name(TSRMLS_C));
				    RETURN_FALSE;
				}
				Agent=UdmAgentInit(NULL,Env,0);
#elif UDM_VERSION_ID >= 30200
				Env=UdmAllocEnv();
				Env->vars=UdmAllocVarList();
				Env->DBAddr=strdup(dbaddr);
				UdmEnvSetDBMode(Env,dbmode);
				Agent=UdmAllocAgent(Env,0,UDM_OPEN_MODE_READ);
#else
				Env=UdmAllocEnv();				
				UdmEnvSetDBAddr(Env,dbaddr);
				UdmEnvSetDBMode(Env,dbmode);
				Agent=UdmAllocAgent(Env,0,UDM_OPEN_MODE_READ);
#endif
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
			var = Z_LVAL_PP(yyvar);
			val = Z_STRVAL_PP(yyval);
			
			break;
			
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	switch(var){
		case UDM_PARAM_PAGE_SIZE: 
#if UDM_VERSION_ID >= 30204
			UdmVarListReplaceStr(&Agent->Conf->Vars,"ps",val);
#else
			Agent->page_size=atoi(val);
			if(Agent->page_size<1)Agent->page_size=20;
#endif
			break;
			
		case UDM_PARAM_PAGE_NUM: 
#if UDM_VERSION_ID >= 30204
			UdmVarListReplaceStr(&Agent->Conf->Vars,"np",val);
#else
			Agent->page_number=atoi(val);
			if(Agent->page_number<0)Agent->page_number=0;
#endif
			break;

		case UDM_PARAM_SEARCH_MODE:
			switch (atoi(val)){
					case UDM_MODE_ALL:
#if UDM_VERSION_ID >= 30204
						UdmVarListReplaceStr(&Agent->Conf->Vars,"m","all");
#else
						Agent->search_mode=UDM_MODE_ALL;
#endif
						break;
						
					case UDM_MODE_ANY:
#if UDM_VERSION_ID >= 30204
						UdmVarListReplaceStr(&Agent->Conf->Vars,"m","any");
#else
						Agent->search_mode=UDM_MODE_ANY;
#endif
						break;
						
					case UDM_MODE_BOOL: 
#if UDM_VERSION_ID >= 30204
						UdmVarListReplaceStr(&Agent->Conf->Vars,"m","bool");
#else
						Agent->search_mode=UDM_MODE_BOOL;
#endif
						break;

					case UDM_MODE_PHRASE: 
#if UDM_VERSION_ID >= 30200
    						php_error(E_WARNING,"%s(): Unknown search mode", get_active_function_name(TSRMLS_C));
						RETURN_FALSE;
#else
						Agent->search_mode=UDM_MODE_PHRASE;
#endif
						break;
						
					default:
#if UDM_VERSION_ID >= 30204
						UdmVarListReplaceStr(&Agent->Conf->Vars,"m","all");
#else
						Agent->search_mode=UDM_MODE_ALL;
#endif						
						php_error(E_WARNING,"%s(): Unknown search mode", get_active_function_name(TSRMLS_C));
						RETURN_FALSE;
						break;
			}
			break;

		case UDM_PARAM_WORD_MATCH:
			switch (atoi(val)){
					case UDM_MATCH_WORD:					
#if UDM_VERSION_ID >= 30204
						UdmVarListReplaceStr(&Agent->Conf->Vars,"wm","wrd");
#else
						Agent->word_match=UDM_MATCH_WORD;
#endif
						break;

					case UDM_MATCH_BEGIN:
#if UDM_VERSION_ID >= 30204
						UdmVarListReplaceStr(&Agent->Conf->Vars,"wm","beg");
#else
						Agent->word_match=UDM_MATCH_BEGIN;
#endif
						break;

					case UDM_MATCH_END:
#if UDM_VERSION_ID >= 30204
						UdmVarListReplaceStr(&Agent->Conf->Vars,"wm","end");
#else
						Agent->word_match=UDM_MATCH_END;
#endif
						break;

					case UDM_MATCH_SUBSTR:
#if UDM_VERSION_ID >= 30204
						UdmVarListReplaceStr(&Agent->Conf->Vars,"wm","sub");
#else
						Agent->word_match=UDM_MATCH_SUBSTR;
#endif
						break;
						
					default:
#if UDM_VERSION_ID >= 30204
						UdmVarListReplaceStr(&Agent->Conf->Vars,"wm","wrd");
#else
						Agent->word_match=UDM_MATCH_WORD;
#endif						
						php_error(E_WARNING,"%s(): Unknown word match mode", get_active_function_name(TSRMLS_C));
						RETURN_FALSE;
						break;
			}
			break;
		case UDM_PARAM_CACHE_MODE: 
			switch (atoi(val)){
				case UDM_ENABLED:
#if UDM_VERSION_ID < 30200
					Agent->cache_mode=UDM_ENABLED;
#elif UDM_VERSION_ID >= 30204
					UdmVarListReplaceStr(&Agent->Conf->Vars,"Cache","yes");
#else		
					UdmReplaceStrVar(Agent->Conf->vars,"Cache","yes",UDM_VARSRC_GLOBAL);
#endif
					break;
					
				case UDM_DISABLED:
#if UDM_VERSION_ID < 30200								
					Agent->cache_mode=UDM_DISABLED;
#elif UDM_VERSION_ID >= 30204
					UdmVarListReplaceStr(&Agent->Conf->Vars,"Cache","no");
#else										
					UdmReplaceStrVar(Agent->Conf->vars,"Cache","no",UDM_VARSRC_GLOBAL);
#endif
					break;
					
				default:
#if UDM_VERSION_ID < 30200												
					Agent->cache_mode=UDM_DISABLED;
#elif UDM_VERSION_ID >= 30204
					UdmVarListReplaceStr(&Agent->Conf->Vars,"Cache","no");
#else									
					UdmReplaceStrVar(Agent->Conf->vars,"Cache","no",UDM_VARSRC_GLOBAL);
#endif						
					php_error(E_WARNING,"%s(): Unknown cache mode", get_active_function_name(TSRMLS_C));
					RETURN_FALSE;
					break;
			}
			
			break;
			
		case UDM_PARAM_TRACK_MODE: 
			switch (atoi(val)){
				case UDM_ENABLED:
#if UDM_VERSION_ID < 30200												
					Agent->track_mode|=UDM_TRACK_QUERIES;
#elif UDM_VERSION_ID >= 30204
					UdmVarListReplaceStr(&Agent->Conf->Vars,"TrackQuery","yes");
#else
					UdmReplaceStrVar(Agent->Conf->vars,"TrackQuery","yes",UDM_VARSRC_GLOBAL);
#endif
					break;
					
				case UDM_DISABLED:
#if UDM_VERSION_ID < 30200						
					Agent->track_mode &= ~(UDM_TRACK_QUERIES);    
#elif UDM_VERSION_ID >= 30204
					UdmVarListReplaceStr(&Agent->Conf->Vars,"TrackQuery","no");
#else
					UdmReplaceStrVar(Agent->Conf->vars,"TrackQuery","no",UDM_VARSRC_GLOBAL);
#endif
					
					break;
					
				default:
#if UDM_VERSION_ID < 30200						
					Agent->track_mode &= ~(UDM_TRACK_QUERIES);    
#elif UDM_VERSION_ID >= 30204
					UdmVarListReplaceStr(&Agent->Conf->Vars,"TrackQuery","no");
#else
					UdmReplaceStrVar(Agent->Conf->vars,"TrackQuery","no",UDM_VARSRC_GLOBAL);
#endif				
					php_error(E_WARNING,"%s(): Unknown track mode", get_active_function_name(TSRMLS_C));
					RETURN_FALSE;
					break;
			}
			
			break;
		
		case UDM_PARAM_PHRASE_MODE: 
#if UDM_VERSION_ID < 30200								
			switch (atoi(val)){
				case UDM_ENABLED:
					Agent->Conf->use_phrases=UDM_ENABLED;
					break;
					
				case UDM_DISABLED:
					Agent->Conf->use_phrases=UDM_DISABLED;
					break;
					
				default:
					php_error(E_WARNING,"%s(): Unknown phrase mode", get_active_function_name(TSRMLS_C));
					RETURN_FALSE;
					break;
			}
#endif			
			break;

		case UDM_PARAM_ISPELL_PREFIXES: 
			switch (atoi(val)){
				case UDM_ENABLED:
#if UDM_VERSION_ID < 30200								
					Agent->Conf->ispell_mode |= UDM_ISPELL_USE_PREFIXES;
#elif UDM_VERSION_ID >= 30204
					UdmVarListReplaceStr(&Agent->Conf->Vars,"IspellUsePrefixes","1");
#else
					UdmAddIntVar(Agent->Conf->vars, "IspellUsePrefixes", 1, UDM_VARSRC_GLOBAL);
#endif								
					break;
					
				case UDM_DISABLED:
#if UDM_VERSION_ID < 30200												
					Agent->Conf->ispell_mode &= ~UDM_ISPELL_USE_PREFIXES;
#elif UDM_VERSION_ID >= 30204
					UdmVarListReplaceStr(&Agent->Conf->Vars,"IspellUsePrefixes","0");
#else
					UdmAddIntVar(Agent->Conf->vars, "IspellUsePrefixes", 0, UDM_VARSRC_GLOBAL);
#endif																	
					break;

				
				default:
#if UDM_VERSION_ID < 30200								
					Agent->Conf->ispell_mode |= UDM_ISPELL_USE_PREFIXES;
#elif UDM_VERSION_ID >= 30204
					UdmVarListReplaceStr(&Agent->Conf->Vars,"IspellUsePrefixes","0");
#else
					UdmAddIntVar(Agent->Conf->vars, "IspellUsePrefixes", 1, UDM_VARSRC_GLOBAL);					
#endif												
					php_error(E_WARNING,"%s(): Unknown ispell prefixes mode", get_active_function_name(TSRMLS_C));
					RETURN_FALSE;
					break;
			}

			break;

		case UDM_PARAM_CHARSET:
		case UDM_PARAM_LOCAL_CHARSET:
#if UDM_VERSION_ID < 30200						
			Agent->Conf->local_charset=UdmGetCharset(val);
			Agent->charset=Agent->Conf->local_charset;
#elif UDM_VERSION_ID >= 30204
			UdmVarListReplaceStr(&Agent->Conf->Vars,"LocalCharset",val);
			{
			    const char * charset=UdmVarListFindStr(&Agent->Conf->Vars,"LocalCharset","iso-8859-1");
			    Agent->Conf->lcs=UdmGetCharSet(charset);
			}
#else
			Agent->Conf->local_charset=strdup(val);
			UdmReplaceStrVar(Agent->Conf->vars,"LocalCharset",val,UDM_VARSRC_GLOBAL);
#endif

			break;
			
#if UDM_VERSION_ID >= 30200
		case UDM_PARAM_BROWSER_CHARSET:
#if UDM_VERSION_ID >= 30204
			UdmVarListReplaceStr(&Agent->Conf->Vars,"BrowserCharset",val);
			{
			    const char * charset=UdmVarListFindStr(&Agent->Conf->Vars,"BrowserCharset","iso-8859-1");
			    Agent->Conf->bcs=UdmGetCharSet(charset);
			}
#else
			Agent->Conf->browser_charset=strdup(val);
			UdmReplaceStrVar(Agent->Conf->vars,"BrowserCharset",val,UDM_VARSRC_GLOBAL);
#endif			
			break;

		case UDM_PARAM_HLBEG:
#if UDM_VERSION_ID >= 30204
			UdmVarListReplaceStr(&Agent->Conf->Vars,"HlBeg",val);
#else
			UdmReplaceStrVar(Agent->Conf->vars,"HlBeg",val,UDM_VARSRC_GLOBAL);
#endif			
			break;

		case UDM_PARAM_HLEND:
#if UDM_VERSION_ID >= 30204
			UdmVarListReplaceStr(&Agent->Conf->Vars,"HlEnd",val);
#else
			UdmReplaceStrVar(Agent->Conf->vars,"HlEnd",val,UDM_VARSRC_GLOBAL);
#endif			
			break;
			
		case UDM_PARAM_SYNONYM:
			if (UdmSynonymListLoad(Agent->Conf,val)) {
				php_error(E_WARNING, "%s(): %s", get_active_function_name(TSRMLS_C),Agent->Conf->errstr);
				RETURN_FALSE;
#if UDM_VERSION_ID >= 30204
			} else UdmSynonymListSort(&(Agent->Conf->Synonyms));
#else
			} else UdmSynonymListSort(&(Agent->Conf->SynList));
#endif			
			break;
			
		case UDM_PARAM_SEARCHD:
#if UDM_VERSION_ID <= 30203
			UdmSDCLientListAdd(&(Agent->Conf->sdcl),val);
			{
				size_t i;
				for(i=0;i<Agent->Conf->sdcl.nclients;i++){
					UdmSDCLientListAdd(&Agent->sdcl,Agent->Conf->sdcl.Clients[i].addr);
				}
			}
#endif			
			break;

		case UDM_PARAM_QSTRING:
#if UDM_VERSION_ID >= 30204
			UdmVarListReplaceStr(&Agent->Conf->Vars,"QUERY_STRING",val);
#else
			UdmReplaceStrVar(Agent->Conf->vars,"QUERY_STRING",val,UDM_VARSRC_GLOBAL);
#endif			
			break;


		case UDM_PARAM_REMOTE_ADDR:
#if UDM_VERSION_ID >= 30204
			UdmVarListReplaceStr(&Agent->Conf->Vars,"IP",val);
#else
			UdmReplaceStrVar(Agent->Conf->vars,"IP",val,UDM_VARSRC_GLOBAL);
#endif			
			break;			
#endif
			
		case UDM_PARAM_STOPTABLE:
#if UDM_VERSION_ID < 30200								
			strcat(Agent->Conf->stop_tables," ");
			strcat(Agent->Conf->stop_tables,val);
#endif
			break;

		case UDM_PARAM_STOPFILE: 
#if UDM_VERSION_ID >= 30204
			if (UdmStopListLoad(Agent->Conf,val)) {
#else
			if (UdmFileLoadStopList(Agent->Conf,val)) {
#endif
				php_error(E_WARNING, "%s(): %s", Agent->Conf->errstr, get_active_function_name(TSRMLS_C));
				RETURN_FALSE;
			}
			break;
			
		case UDM_PARAM_WEIGHT_FACTOR: 
#if UDM_VERSION_ID < 30200										
			Agent->weight_factor=strdup(val);
#elif UDM_VERSION_ID >= 30204
			UdmVarListReplaceStr(&Agent->Conf->Vars,"wf",val);
#else
			UdmReplaceStrVar(Agent->Conf->vars,"wf",val,UDM_VARSRC_GLOBAL);
			{
				size_t len;
	
		                len=strlen(val);
				if((len>0)&&(len<256)){
				    const char *sec;
				    int sn;
				
				    for(sn=0;sn<256;sn++){
					    Agent->wf[sn]=0;
				    }
				
				    for(sec=val+len-1;sec>=val;sec--){
					    Agent->wf[len-(sec-val)]=UdmHex2Int(*sec);
	                	    }
        			}                                                          			
			}
#endif			    
			break;
			
		case UDM_PARAM_MIN_WORD_LEN: 
#if UDM_VERSION_ID >= 30204
			Agent->Conf->WordParam.min_word_len=atoi(val);
#else
			Agent->Conf->min_word_len=atoi(val);
#endif			    
			break;
			
		case UDM_PARAM_MAX_WORD_LEN: 
#if UDM_VERSION_ID >= 30204
			Agent->Conf->WordParam.max_word_len=atoi(val);
#else
			Agent->Conf->max_word_len=atoi(val);
#endif			    
			break;
			
		case UDM_PARAM_CROSS_WORDS: 
			switch (atoi(val)){
				case UDM_ENABLED:
#if UDM_VERSION_ID >= 30204
					UdmVarListReplaceStr(&Agent->Conf->Vars,"CrossWords","yes");
#else
					Agent->Conf->use_crossword=UDM_ENABLED;
#endif
					break;
					
				case UDM_DISABLED:
#if UDM_VERSION_ID >= 30204
					UdmVarListReplaceStr(&Agent->Conf->Vars,"CrossWords","no");
#else
					Agent->Conf->use_crossword=UDM_DISABLED;
#endif
					break;
					
				default:
					php_error(E_WARNING,"%s(): Unknown crosswords mode", get_active_function_name(TSRMLS_C));
					RETURN_FALSE;
					break;
			}
			
			break;

#if UDM_VERSION_ID > 30112

		case UDM_PARAM_VARDIR:
#if UDM_VERSION_ID < 30200
			udm_snprintf(Agent->Conf->vardir,sizeof(Agent->Conf->vardir)-1,"%s%s",val,UDMSLASHSTR);
#elif UDM_VERSION_ID >= 30204
			UdmVarListReplaceStr(&Agent->Conf->Vars,"Vardir",val);
			snprintf(Agent->Conf->vardir,sizeof(Agent->Conf->vardir)-1,"%s%s",val,UDMSLASHSTR);
#else			
			snprintf(Agent->Conf->vardir,sizeof(Agent->Conf->vardir)-1,"%s%s",val,UDMSLASHSTR);
#endif			
			break;
#endif			

#if UDM_VERSION_ID >= 30204
		case UDM_PARAM_QUERY:
			UdmVarListReplaceStr(&Agent->Conf->Vars,"q",val);

			break;

		case UDM_PARAM_STORED:
			UdmVarListReplaceStr(&Agent->Conf->Vars,"StoredAddr",val);

			break;
#endif
			
#if UDM_VERSION_ID >= 30207
		case UDM_PARAM_GROUPBYSITE: 
			switch (atoi(val)){
				case UDM_ENABLED:
					UdmVarListReplaceStr(&Agent->Conf->Vars,"GroupBySite","yes");
					
					break;
					
				case UDM_DISABLED:
					UdmVarListReplaceStr(&Agent->Conf->Vars,"GroupBySite","no");

					break;
					
				default:
					php_error(E_WARNING,"%s(): Unknown groupbysite mode", get_active_function_name(TSRMLS_C));
					RETURN_FALSE;
					break;
			}
			
			break;
			
		case UDM_PARAM_SITEID: 
			UdmVarListReplaceStr(&Agent->Conf->Vars,"site",val);
		
			break;

		case UDM_PARAM_DETECT_CLONES: 
			switch (atoi(val)){
				case UDM_ENABLED:
					UdmVarListReplaceStr(&Agent->Conf->Vars,"DetectClones","yes");
					
					break;
					
				case UDM_DISABLED:
					UdmVarListReplaceStr(&Agent->Conf->Vars,"DetectClones","no");

					break;
					
				default:
					php_error(E_WARNING,"%s(): Unknown clones mode", get_active_function_name(TSRMLS_C));
					RETURN_FALSE;
					break;
			}
			
			break;
#endif

		default:
			php_error(E_WARNING,"%s(): Unknown agent session parameter", get_active_function_name(TSRMLS_C));
			RETURN_FALSE;
			break;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int udm_load_ispell_data(int agent, int var, string val1, [string charset], string val2, int flag)
   Load ispell data */
DLEXPORT PHP_FUNCTION(udm_load_ispell_data)
{
	pval **yyagent, **yyvar, **yyval1, **yyval2, **yyflag, **yycharset ;
	char *val1, *val2, *charset;
	int var, flag;
	UDM_AGENT * Agent;

	switch(ZEND_NUM_ARGS()){
	
		case 5: 		
			if(zend_get_parameters_ex(5,&yyagent,&yyvar,&yyval1,&yyval2,&yyflag)==FAILURE){
				RETURN_FALSE;
			}
			convert_to_long_ex(yyvar);
			convert_to_long_ex(yyflag);
			convert_to_string_ex(yyval1);
			convert_to_string_ex(yyval2);
			ZEND_FETCH_RESOURCE(Agent, UDM_AGENT *, yyagent, -1, "mnoGoSearch-agent", le_link);
			var  = Z_LVAL_PP(yyvar);
			flag = Z_LVAL_PP(yyflag);
			val1 = Z_STRVAL_PP(yyval1);
			val2 = Z_STRVAL_PP(yyval2);
			charset = "us-ascii";
			
			break;

		case 6: 		
			if(zend_get_parameters_ex(6,&yyagent,&yyvar,&yyval1,&yycharset,&yyval2,&yyflag)==FAILURE){
				RETURN_FALSE;
			}
			convert_to_long_ex(yyvar);
			convert_to_long_ex(yyflag);
			convert_to_string_ex(yyval1);
			convert_to_string_ex(yyval2);
			convert_to_string_ex(yycharset);
			ZEND_FETCH_RESOURCE(Agent, UDM_AGENT *, yyagent, -1, "mnoGoSearch-agent", le_link);
			var  = Z_LVAL_PP(yyvar);
			flag = Z_LVAL_PP(yyflag);
			val1 = Z_STRVAL_PP(yyval1);
			val2 = Z_STRVAL_PP(yyval2);
			charset = Z_STRVAL_PP(yycharset);
			
			break;
			
		default:
			WRONG_PARAM_COUNT;
			break;
	}


	switch(var){
#if UDM_VERSION_ID < 30200		
		case UDM_ISPELL_TYPE_DB: 
			Agent->Conf->ispell_mode |= UDM_ISPELL_MODE_DB;
			
			if (UdmDBImportAffixes(Agent,Agent->charset) || 
			    UdmImportDictionaryFromDB(Agent)) {
				RETURN_FALSE;
			} 
									
			break;
			
#if UDM_VERSION_ID > 30111

		case UDM_ISPELL_TYPE_SERVER:
			Agent->Conf->ispell_mode &= ~UDM_ISPELL_MODE_DB;
			Agent->Conf->ispell_mode |=  UDM_ISPELL_MODE_SERVER;
			
			Agent->Conf->spellhost = strdup(val1);
		
			break;
			
#endif			
#endif

		case UDM_ISPELL_TYPE_AFFIX: 
#if UDM_VERSION_ID < 30200		
			Agent->Conf->ispell_mode &= ~UDM_ISPELL_MODE_DB;

#if UDM_VERSION_ID > 30111
			Agent->Conf->ispell_mode &= ~UDM_ISPELL_MODE_SERVER;
#endif
			
			if (UdmImportAffixes(Agent->Conf,val1,val2,NULL,0)) {
				php_error(E_WARNING,"%s(): Cannot load affix file %s", get_active_function_name(TSRMLS_C),val2);
				RETURN_FALSE;
			}
#else
			if (UdmImportAffixes(Agent->Conf,val1,charset,val2)) {
				php_error(E_WARNING,"%s(): Cannot load affix file %s", get_active_function_name(TSRMLS_C),val2);
				RETURN_FALSE;
			}
    
#endif
			break;
			
		case UDM_ISPELL_TYPE_SPELL: 
#if UDM_VERSION_ID < 30200				
			Agent->Conf->ispell_mode &= ~UDM_ISPELL_MODE_DB;
			
#if UDM_VERSION_ID > 30111
			Agent->Conf->ispell_mode &= ~UDM_ISPELL_MODE_SERVER;
#endif
			
			if (UdmImportDictionary(Agent->Conf,val1,val2,1,"")) {
				php_error(E_WARNING,"%s(): Cannot load spell file %s", get_active_function_name(TSRMLS_C),val2);
				RETURN_FALSE;
			}
#else
			if (UdmImportDictionary(Agent->Conf,val1,charset,val2,0,"")) {
				php_error(E_WARNING,"%s(): Cannot load spell file %s", get_active_function_name(TSRMLS_C),val2);
				RETURN_FALSE;
			}
#endif
			break;


		default:
			php_error(E_WARNING,"%s(): Unknown ispell type parameter", get_active_function_name(TSRMLS_C));
			RETURN_FALSE;
			break;
	}
	
	if (flag) {
#if UDM_VERSION_ID >= 30204
		if(Agent->Conf->Spells.nspell) {
			UdmSortDictionary(&Agent->Conf->Spells);
		  	UdmSortAffixes(&Agent->Conf->Affixes,&Agent->Conf->Spells);
		}
#else
		if(Agent->Conf->nspell) {
			UdmSortDictionary(Agent->Conf);
		  	UdmSortAffixes(Agent->Conf);
		}
#endif
	}
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int udm_free_ispell_data(int agent)
   Free memory allocated for ispell data */
DLEXPORT PHP_FUNCTION(udm_free_ispell_data)
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

#if UDM_VERSION_ID >= 30204
	UdmSpellListFree(&Agent->Conf->Spells);
	UdmAffixListFree(&Agent->Conf->Affixes);
#elif UDM_VERSION_ID > 30111
	UdmFreeIspell(Agent->Conf);
#endif
	
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
			var = Z_LVAL_PP(yyvar);
			val = Z_STRVAL_PP(yyval);
			
			break;
			
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	switch(var){
		case UDM_LIMIT_URL: 
#if UDM_VERSION_ID >= 30204			
			UdmVarListAddStr(&Agent->Conf->Vars,"ul",val);
#else
			UdmAddURLLimit(Agent->Conf,val);
#endif		
			break;
			
		case UDM_LIMIT_TAG: 
#if UDM_VERSION_ID >= 30204			
			UdmVarListAddStr(&Agent->Conf->Vars,"t",val);
#else
			UdmAddTagLimit(Agent->Conf,val);
#endif		
			break;

		case UDM_LIMIT_LANG: 
#if UDM_VERSION_ID >= 30204			
			UdmVarListAddStr(&Agent->Conf->Vars,"lang",val);
#else
			UdmAddLangLimit(Agent->Conf,val);
#endif			
			break;

		case UDM_LIMIT_CAT: 
#if UDM_VERSION_ID >= 30204
			UdmVarListAddStr(&Agent->Conf->Vars,"cat",val);
#else
			UdmAddCatLimit(Agent->Conf,val);
#endif			
			break;

		case UDM_LIMIT_TYPE: 
#if UDM_VERSION_ID >= 30207
			UdmVarListAddStr(&Agent->Conf->Vars,"type",val);
#endif			
			break;
			
		case UDM_LIMIT_DATE: 
#if UDM_VERSION_ID < 30200			
			{
			struct udm_stl_info_t stl_info = { 0, 0, 0 };
			char *edate;
			
			if (val[0] == '>') {
				Z_TYPE(stl_info)=1;
			} else if (val[0] == '<') {
				Z_TYPE(stl_info)=-1;
			} else if((val[0]=='#')&&(edate=strchr(val,','))){
				Z_TYPE(stl_info)=2;
				stl_info.t2=(time_t)(atol(edate+1));
			} else {
				php_error(E_WARNING,"%s(): Incorrect date limit format", get_active_function_name(TSRMLS_C));
				RETURN_FALSE;
			}			
			
			stl_info.t1=(time_t)(atol(val+1));
			UdmAddTimeLimit(Agent->Conf,&stl_info);
			}
#endif
			break;
		default:
			php_error(E_WARNING,"%s(): Unknown search limit parameter", get_active_function_name(TSRMLS_C));
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
	int i;
	
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
#if UDM_VERSION_ID >= 30204	
	for(i=0;i<Agent->Conf->Vars.nvars;i++){
		if ((!strcasecmp("ul",Agent->Conf->Vars.Var[i].name))||
		    (!strcasecmp("cat",Agent->Conf->Vars.Var[i].name))||
		    (!strcasecmp("t",Agent->Conf->Vars.Var[i].name))||
		    (!strcasecmp("lang",Agent->Conf->Vars.Var[i].name))) {
		    UDM_FREE(Agent->Conf->Vars.Var[i].name);
		    UDM_FREE(Agent->Conf->Vars.Var[i].val);
		    Agent->Conf->Vars.nvars--;
		}
	}
#else
	UdmClearLimits(Agent->Conf);
#endif	
	RETURN_TRUE;
}
/* }}} */

#if UDM_VERSION_ID >= 30200
/* {{{ proto int udm_check_charset(int agent, string charset)
   Check if the given charset is known to mnogosearch */
DLEXPORT PHP_FUNCTION(udm_check_charset)
{
	pval ** yycharset, ** yyagent;
	UDM_AGENT * Agent;
	int id=-1;

	switch(ZEND_NUM_ARGS()){
		case 2: {
				if (zend_get_parameters_ex(2, &yyagent,&yycharset)==FAILURE) {
					RETURN_FALSE;
				}
			}
			break;
		default:				
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(Agent, UDM_AGENT *, yyagent, id, "mnoGoSearch-Agent", le_link);
	convert_to_string_ex(yycharset);
	

	if (UdmGetCharSet(Z_STRVAL_PP(yycharset))) {
	    RETURN_TRUE;
	} else {
	    RETURN_FALSE;
	}	
}
/* }}} */
#endif

#if UDM_VERSION_ID >= 30203
/* {{{ proto int udm_crc32(int agent, string str)
   Return CRC32 checksum of gived string */
DLEXPORT PHP_FUNCTION(udm_crc32)
{
	pval ** yystr, ** yyagent;
	char *str;
	int crc32;
	char buf[32];
	UDM_AGENT * Agent;
	int id=-1;

	switch(ZEND_NUM_ARGS()){
		case 2: {
				if (zend_get_parameters_ex(2, &yyagent,&yystr)==FAILURE) {
					RETURN_FALSE;
				}
			}
			break;
		default:				
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(Agent, UDM_AGENT *, yyagent, id, "mnoGoSearch-Agent", le_link);
	convert_to_string_ex(yystr);
	str = Z_STRVAL_PP(yystr);

	crc32=UdmCRC32((str),strlen(str));
	snprintf(buf,sizeof(buf)-1,"%u",crc32);
	
	RETURN_STRING(buf,1);
}
/* }}} */
#endif

#if UDM_VERSION_ID == 30203

static ssize_t UdmRecvAll(int s, void *buf, size_t len, int flags) {
  size_t received = 0, r;
  char *b = buf;
  while ( (received < len) && ((r = recv(s, &b[received], len - received, flags)) >= 0 ) ) {
    received += r;
  }
  return received;
}


/* {{{ proto int udm_open_stored(int agent, string storedaddr)
   Open connection to stored  */
DLEXPORT PHP_FUNCTION(udm_open_stored)
{
	pval ** yyagent, ** yyaddr;
	
	int s;
	char * addr;
	const char *hello = "F\0";
	
	UDM_AGENT * Agent;
	int id=-1;

	switch(ZEND_NUM_ARGS()){
		case 2: {
				if (zend_get_parameters_ex(2, &yyagent, &yyaddr)==FAILURE) {
					RETURN_FALSE;
				}
			}
			break;
		default:				
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(Agent, UDM_AGENT *, yyagent, id, "mnoGoSearch-Agent", le_link);
	
	convert_to_string_ex(yyaddr);
	addr = Z_STRVAL_PP(yyaddr);

	UdmReplaceStrVar(Agent->Conf->vars,"StoredAddr",addr,UDM_VARSRC_GLOBAL);
	Agent->Conf->stored_addr = strdup(UdmFindStrVar(Agent->Conf->vars, "StoredAddr", "localhost"));
	
	s = UdmStoreOpen(Agent->Conf);
	
	if (s >= 0) {
	    send(s, hello, 1, 0);
	    RETURN_LONG(s);
	} else RETURN_FALSE;	
}
/* }}} */

/* {{{ proto int udm_close_stored(int agent, int link)
   Open connection to stored  */
DLEXPORT PHP_FUNCTION(udm_close_stored)
{
	pval ** yylink, ** yyagent;
	
	int s;
	unsigned int rec_id = 0;
	
	UDM_AGENT * Agent;
	int id=-1;

	switch(ZEND_NUM_ARGS()){
		case 2: {
				if (zend_get_parameters_ex(2, &yyagent, &yylink)==FAILURE) {
					RETURN_FALSE;
				}
			}
			break;
		default:				
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(Agent, UDM_AGENT *, yyagent, id, "mnoGoSearch-Agent", le_link);
	
	convert_to_long_ex(yylink);
	s = Z_LVAL_PP(yylink);

	
	send(s, &rec_id, sizeof(rec_id), 0);
	closesocket(s);
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int udm_check_stored(int agent, int link, string doc_id)
   Open connection to stored  */
DLEXPORT PHP_FUNCTION(udm_check_stored)
{
	pval ** yyid, ** yylink, ** yyagent;
	
	int s;
	unsigned int rec_id = 0;
	int store_found;
	
	UDM_AGENT * Agent;
	int id=-1;

	switch(ZEND_NUM_ARGS()){
		case 3: {
				if (zend_get_parameters_ex(3, &yyagent, &yylink, &yyid)==FAILURE) {
					RETURN_FALSE;
				}
			}
			break;
		default:				
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(Agent, UDM_AGENT *, yyagent, id, "mnoGoSearch-Agent", le_link);
	
	convert_to_long_ex(yylink);
	s = Z_LVAL_PP(yylink);
	
	convert_to_string_ex(yyid);
	
	rec_id=strtoul(Z_STRVAL_PP(yyid),NULL,10);

	send(s, &rec_id, sizeof(rec_id), 0);
	if (UdmRecvAll(s, &store_found, sizeof(store_found), MSG_WAITALL) < 0) {
	    RETURN_LONG(store_found);
	} else RETURN_FALSE;
}
/* }}} */
#endif

#if UDM_VERSION_ID >= 30204
/* {{{ proto int udm_parse_query_string(int agent, string str)
   Parses query string, initialises variables and search limits taken from it */
DLEXPORT PHP_FUNCTION(udm_parse_query_string)
{
	pval ** yystr, ** yyagent;
	char *str;
	UDM_AGENT * Agent;
	int id=-1;

	switch(ZEND_NUM_ARGS()){
		case 2: {
				if (zend_get_parameters_ex(2, &yyagent,&yystr)==FAILURE) {
					RETURN_FALSE;
				}
			}
			break;
		default:				
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(Agent, UDM_AGENT *, yyagent, id, "mnoGoSearch-Agent", le_link);
	convert_to_string_ex(yystr);
	str = Z_STRVAL_PP(yystr);

	UdmParseQueryString(Agent,&Agent->Conf->Vars,str);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int udm_make_excerpt(int agent, int res, int row)
   Perform search */
DLEXPORT PHP_FUNCTION(udm_make_excerpt)
{
	pval ** yyagent, **yyres, **yyrow_num;
	UDM_RESULT * Res;
	UDM_AGENT * Agent;
	int id=-1, row;

	switch(ZEND_NUM_ARGS()){
		case 3: {
				if (zend_get_parameters_ex(3, &yyagent, &yyres, &yyrow_num)==FAILURE) {
					RETURN_FALSE;
				}
				convert_to_string_ex(yyrow_num);
				row=atoi(Z_STRVAL_PP(yyrow_num));
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(Agent, UDM_AGENT *, yyagent, id, "mnoGoSearch-Agent", le_link);
	ZEND_FETCH_RESOURCE(Res, UDM_RESULT *, yyres, -1, "mnoGoSearch-Result", le_res);
	
	if(row<Res->num_rows){
		char		*al;
		char		*Excerpt;
		
		al = (char *)MyRemoveHiLightDup((const char *)(UdmVarListFindStr(&(Res->Doc[row].Sections), "URL", "")));
		UdmVarListReplaceInt(&(Res->Doc[row].Sections), "STORED_ID", UdmCRC32(al, strlen(al)));
		free(al);
		
		Excerpt = UdmExcerptDoc(Agent, Res, &(Res->Doc[row]), 256);
		
		if (Excerpt != NULL) {
			char *HlExcerpt = UdmHlConvert(&Res->WWList, Excerpt, Agent->Conf->lcs, Agent->Conf->bcs);
			UdmVarListReplaceInt(&(Res->Doc[row].Sections),"ST",1);
			UdmVarListReplaceStr(&(Res->Doc[row].Sections),"Body",HlExcerpt);
			UDM_FREE(HlExcerpt);
			UDM_FREE(Excerpt);
		} else {
		        UdmVarListReplaceInt(&(Res->Doc[row].Sections),"ST",0);
			RETURN_FALSE;
		}
	}else{
		php_error(E_WARNING,"%s(): row number too large", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int udm_set_agent_param_ex(int agent, string var, string val)
   Set mnoGoSearch agent session parameters extended */
DLEXPORT PHP_FUNCTION(udm_set_agent_param_ex)
{
	pval **yyagent, **yyvar, **yyval;
	char *val, *var;
	UDM_AGENT * Agent;

	switch(ZEND_NUM_ARGS()){
	
		case 3: 		
			if(zend_get_parameters_ex(3,&yyagent,&yyvar,&yyval)==FAILURE){
				RETURN_FALSE;
			}
			convert_to_string_ex(yyvar);
			convert_to_string_ex(yyval);
			ZEND_FETCH_RESOURCE(Agent, UDM_AGENT *, yyagent, -1, "mnoGoSearch-agent", le_link);
			var = Z_STRVAL_PP(yyvar);
			val = Z_STRVAL_PP(yyval);
			
			break;
			
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	UdmVarListReplaceStr(&Agent->Conf->Vars,var,val);
	
	if (!strcasecmp(var,"LocalCharset")) {
		const char * charset=UdmVarListFindStr(&Agent->Conf->Vars,"LocalCharset","iso-8859-1");
		Agent->Conf->lcs=UdmGetCharSet(charset);
	} else if (!strcasecmp(var,"BrowserCharset")) {
		const char * charset=UdmVarListFindStr(&Agent->Conf->Vars,"BrowserCharset","iso-8859-1");
		Agent->Conf->bcs=UdmGetCharSet(charset);
	} else if (!strcasecmp(var,"Synonym")) {
		if (UdmSynonymListLoad(Agent->Conf,val)) {
			php_error(E_WARNING, "%s(): %s", get_active_function_name(TSRMLS_C),Agent->Conf->errstr);
			RETURN_FALSE;
		} else UdmSynonymListSort(&(Agent->Conf->Synonyms));
	} else if (!strcasecmp(var,"Stopwordfile")) {
		if (UdmStopListLoad(Agent->Conf,val)) {
			php_error(E_WARNING, "%s(): %s", Agent->Conf->errstr, get_active_function_name(TSRMLS_C));
			RETURN_FALSE;
		}
	} else if (!strcasecmp(var,"MinWordLen")) {
		Agent->Conf->WordParam.min_word_len=atoi(val);
	} else if (!strcasecmp(var,"MaxWordLen")) {
		Agent->Conf->WordParam.max_word_len=atoi(val);
	} else if (!strcasecmp(var,"VarDir")) {
		snprintf(Agent->Conf->vardir,sizeof(Agent->Conf->vardir)-1,"%s%s",val,UDMSLASHSTR);
	}

	RETURN_TRUE;
}

/* {{{ proto string udm_get_res_field_ex(int res, int row, string field)
   Fetch mnoGoSearch result field */
DLEXPORT PHP_FUNCTION(udm_get_res_field_ex)
{
	pval **yyres, **yyrow_num, **yyfield_name;

	UDM_RESULT * Res;
	int row;
	char *field;
	
	switch(ZEND_NUM_ARGS()){
		case 3: {
				if (zend_get_parameters_ex(3, &yyres,&yyrow_num,&yyfield_name)==FAILURE){
					RETURN_FALSE;
				}
				convert_to_string_ex(yyrow_num);
				convert_to_string_ex(yyfield_name);
				field = Z_STRVAL_PP(yyfield_name);
				row = atoi(Z_STRVAL_PP(yyrow_num));
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE(Res, UDM_RESULT *, yyres, -1, "mnoGoSearch-Result", le_res);
	if(row<Res->num_rows){
		if (!strcasecmp(field,"URL")) {
		    char	*al;
		    al = (char *)MyRemoveHiLightDup((const char *)(UdmVarListFindStr(&(Res->Doc[row].Sections), field, "")));
		    UdmVarListReplaceStr(&Res->Doc[row].Sections,field,al);
		    free(al);
		}
		RETURN_STRING((char *)UdmVarListFindStr(&Res->Doc[row].Sections,field,""),1);
	} else {
		php_error(E_WARNING,"%s(): row number too large", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}
}
/* }}} */
#endif

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
	
#if UDM_VERSION_ID < 30200
	if ((Res=UdmFind(Agent,UdmTolower(Z_STRVAL_PP(yyquery),Agent->charset)))) {
#else
	if ((Res=UdmFind(Agent,Z_STRVAL_PP(yyquery)))) {
#endif	
	    ZEND_REGISTER_RESOURCE(return_value,Res,le_res);
	} else {
	    RETURN_FALSE;
	}	
}
/* }}} */

/* {{{ proto string udm_get_res_field(int res, int row, int field)
   Fetch mnoGoSearch result field */
DLEXPORT PHP_FUNCTION(udm_get_res_field)
{
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
				field=atoi(Z_STRVAL_PP(yyfield_name));
				row=atoi(Z_STRVAL_PP(yyrow_num));
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
#if UDM_VERSION_ID >= 30204
			    {
				char	*al;
				al = (char *)MyRemoveHiLightDup((const char *)(UdmVarListFindStr(&(Res->Doc[row].Sections), "URL", "")));
				UdmVarListReplaceStr(&Res->Doc[row].Sections,"URL",al);
				free(al);
				
				RETURN_STRING((char *)UdmVarListFindStr(&Res->Doc[row].Sections,"URL",""),1);
			    }
#else
				RETURN_STRING((Res->Doc[row].url)?(Res->Doc[row].url):"",1);
#endif
				break;
				
			case UDM_FIELD_CONTENT: 	
#if UDM_VERSION_ID >= 30204
				RETURN_STRING((char *)UdmVarListFindStr(&(Res->Doc[row].Sections),"Content-Type",""),1);
#else
				RETURN_STRING((Res->Doc[row].content_type)?(Res->Doc[row].content_type):"",1);
#endif
				break;
				
			case UDM_FIELD_TITLE:		
#if UDM_VERSION_ID >= 30204
				RETURN_STRING((char *)UdmVarListFindStr(&(Res->Doc[row].Sections),"Title",""),1);
#else
				RETURN_STRING((Res->Doc[row].title)?(Res->Doc[row].title):"",1);
#endif
				break;
				
			case UDM_FIELD_KEYWORDS:	
#if UDM_VERSION_ID >= 30204
				RETURN_STRING((char *)UdmVarListFindStr(&(Res->Doc[row].Sections),"Meta.Keywords",""),1);
#else
				RETURN_STRING((Res->Doc[row].keywords)?(Res->Doc[row].keywords):"",1);
#endif
				break;
				
			case UDM_FIELD_DESC:		
#if UDM_VERSION_ID >= 30204
				RETURN_STRING((char *)UdmVarListFindStr(&(Res->Doc[row].Sections),"Meta.Description",""),1);
#else
				RETURN_STRING((Res->Doc[row].description)?(Res->Doc[row].description):"",1);
#endif
				break;
				
			case UDM_FIELD_TEXT:		
#if UDM_VERSION_ID >= 30204
				RETURN_STRING((char *)UdmVarListFindStr(&(Res->Doc[row].Sections),"Body",""),1);
#else
				RETURN_STRING((Res->Doc[row].text)?(Res->Doc[row].text):"",1);
#endif
				break;
				
			case UDM_FIELD_SIZE:		
#if UDM_VERSION_ID >= 30204
				RETURN_LONG(UdmVarListFindInt(&(Res->Doc[row].Sections),"Content-Length",0));
#else
				RETURN_LONG((Res->Doc[row].size));
#endif
				break;
				
			case UDM_FIELD_URLID:
#if UDM_VERSION_ID >= 30204
				RETURN_LONG(UdmVarListFindInt(&(Res->Doc[row].Sections),"ID",0));
#else
				RETURN_LONG((Res->Doc[row].url_id));
#endif
				break;
				
			case UDM_FIELD_RATING:		
#if UDM_VERSION_ID >= 30204
				RETURN_STRING((char *)UdmVarListFindStr(&(Res->Doc[row].Sections),"Score",""),1);
#else
				RETURN_LONG((Res->Doc[row].rating));
#endif
				break;
				
			case UDM_FIELD_MODIFIED:	
#if UDM_VERSION_ID >= 30204
				RETURN_STRING((char *)UdmVarListFindStr(&(Res->Doc[row].Sections),"Last-Modified",""),1);
#else
				RETURN_LONG((Res->Doc[row].last_mod_time));
#endif
				break;

			case UDM_FIELD_ORDER:	
#if UDM_VERSION_ID >= 30204
				RETURN_LONG(UdmVarListFindInt(&(Res->Doc[row].Sections),"Order",0));
#else
				RETURN_LONG((Res->Doc[row].order));
#endif
				break;
				
			case UDM_FIELD_CRC:	
#if UDM_VERSION_ID >= 30204
				RETURN_LONG(UdmVarListFindInt(&(Res->Doc[row].Sections),"crc32",0));
#else
				RETURN_LONG((Res->Doc[row].crc32));
#endif
				break;
				
			case UDM_FIELD_CATEGORY:		
#if UDM_VERSION_ID >= 30204
				RETURN_STRING((char *)UdmVarListFindStr(&(Res->Doc[row].Sections),"Category",""),1);
#else
				RETURN_STRING((Res->Doc[row].category)?(Res->Doc[row].category):"",1);
#endif
				break;

#if UDM_VERSION_ID >= 30203		
			case UDM_FIELD_LANG:		
#if UDM_VERSION_ID >= 30204
				RETURN_STRING((char *)UdmVarListFindStr(&(Res->Doc[row].Sections),"Content-Language",""),1);
#else
				RETURN_STRING((Res->Doc[row].lang)?(Res->Doc[row].lang):"",1);
#endif
				break;

			case UDM_FIELD_CHARSET:		
#if UDM_VERSION_ID >= 30204
				RETURN_STRING((char *)UdmVarListFindStr(&(Res->Doc[row].Sections),"Charset",""),1);
#else
				RETURN_STRING((Res->Doc[row].charset)?(Res->Doc[row].charset):"",1);
#endif
				break;
#endif			

#if UDM_VERSION_ID >= 30207
			case UDM_FIELD_SITEID:
				RETURN_LONG(UdmVarListFindInt(&(Res->Doc[row].Sections),"Site_id",0));
				
				break;

			case UDM_FIELD_POP_RANK:
				RETURN_STRING((char *)UdmVarListFindStr(&(Res->Doc[row].Sections),"Pop_Rank",""),1);
				
				break;

			case UDM_FIELD_ORIGINID:
				RETURN_LONG(UdmVarListFindInt(&(Res->Doc[row].Sections),"Origin-Id",0));

				break;
#endif
				
			default: 
				php_error(E_WARNING,"%s(): Unknown mnoGoSearch field name", get_active_function_name(TSRMLS_C));
				RETURN_FALSE;
				break;
		}
	}else{
		php_error(E_WARNING,"%s(): row number too large", get_active_function_name(TSRMLS_C));
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
				param=(Z_LVAL_PP(yyparam));
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
#if UDM_VERSION_ID >= 30204
			{
			    int len,i;
			    for(len = i = 0; i < Res->WWList.nwords; i++) 
				len += Res->WWList.Word[i].len;
			    {	
				size_t wsize=(1+len*15)*sizeof(char);
				char *wordinfo = (char*) malloc(wsize);
				int corder = -1, ccount = 0;
	  
				*wordinfo = '\0';
	  
				for(i = 0; i < Res->WWList.nwords; i++){
				    if ((Res->WWList.Word[i].count > 0) || (Res->WWList.Word[i].origin == UDM_WORD_ORIGIN_QUERY)) {
				    if(wordinfo[0]) strcat(wordinfo,", ");
					sprintf(UDM_STREND(wordinfo)," %s : %d", Res->WWList.Word[i].word, Res->WWList.Word[i].count);
				    } else if (Res->WWList.Word[i].origin == UDM_WORD_ORIGIN_STOP) {
					if(wordinfo[0]) strcat(wordinfo,", ");
					sprintf(UDM_STREND(wordinfo)," %s : stopword", Res->WWList.Word[i].word);
				    }
				}
				RETURN_STRING(wordinfo,1);
			    }
			}
#else
			RETURN_STRING((Res->wordinfo)?(Res->wordinfo):"",1);
#endif
			break;

#if UDM_VERSION_ID >= 30204
		case UDM_PARAM_WORDINFO_ALL: 
			{
			    int len,i;
			    for(len = i = 0; i < Res->WWList.nwords; i++) 
				len += Res->WWList.Word[i].len;
			    {	
				size_t wsize=(1+len*15)*sizeof(char);
				char *wordinfo = (char*) malloc(wsize);
				int corder = -1, ccount = 0;
	  
				*wordinfo = '\0';
				
				for(i = 0; i < Res->WWList.nwords; i++){
				    if (Res->WWList.Word[i].order != corder) {
					if(wordinfo[0]) {
					    sprintf(UDM_STREND(wordinfo)," / %d, ", ccount);
					}
					ccount = Res->WWList.Word[i].count;
					if (Res->WWList.Word[i].origin == UDM_WORD_ORIGIN_STOP) {
					    sprintf(UDM_STREND(wordinfo)," %s : stopword", Res->WWList.Word[i].word);
					} else {
					    sprintf(UDM_STREND(wordinfo)," %s : %d", Res->WWList.Word[i].word, Res->WWList.Word[i].count);
					    corder = Res->WWList.Word[i].order; 
					}
				    } else {
					ccount += Res->WWList.Word[i].count;
				    }
				}
				if (Res->WWList.nwords) {
				    sprintf(UDM_STREND(wordinfo)," / %d", ccount);
				}
				RETURN_STRING(wordinfo,1);
			    }
			}
			break;
#endif
			
		case UDM_PARAM_SEARCHTIME: 	
			RETURN_DOUBLE(((double)Res->work_time)/1000);
			break;			
			
		case UDM_PARAM_FIRST_DOC:	 	
			RETURN_LONG(Res->first);
			break;

		case UDM_PARAM_LAST_DOC:	 	
			RETURN_LONG(Res->last);
			break;

		default:
			php_error(E_WARNING,"%s(): Unknown mnoGoSearch param name", get_active_function_name(TSRMLS_C));
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
	zend_list_delete(Z_LVAL_PP(yyres));
	
	RETURN_TRUE;
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
	zend_list_delete(Z_LVAL_PP(yyagent));
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int udm_errno(int agent)
   Get mnoGoSearch error number */
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
#if UDM_VERSION_ID >= 30204
	RETURN_LONG(UdmEnvErrCode(Agent->Conf));
#else
	RETURN_LONG(UdmDBErrorCode(Agent->db));
#endif
}
/* }}} */

/* {{{ proto string udm_error(int agent)
   Get mnoGoSearch error message */
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
#if UDM_VERSION_ID >= 30204
	RETURN_STRING((UdmEnvErrMsg(Agent->Conf))?(UdmEnvErrMsg(Agent->Conf)):"",1);
#else
	RETURN_STRING((UdmDBErrorMsg(Agent->db))?(UdmDBErrorMsg(Agent->db)):"",1);
#endif
}
/* }}} */

/* {{{ proto int udm_api_version()
   Get mnoGoSearch API version */
DLEXPORT PHP_FUNCTION(udm_api_version)
{
	RETURN_LONG(UDM_VERSION_ID);
}
/* }}} */

/* {{{ proto array udm_cat_list(int agent, string category)
   Get mnoGoSearch categories list with the same root */
DLEXPORT PHP_FUNCTION(udm_cat_list)
{
	pval ** yycat, ** yyagent;
	UDM_AGENT * Agent;
	char *cat;
#if UDM_VERSION_ID >= 30204	
	UDM_CATEGORY C;
#else	
	UDM_CATEGORY *c=NULL;
#endif
	char *buf=NULL;
	int id=-1;

	switch(ZEND_NUM_ARGS()){
		case 2: {
				if (zend_get_parameters_ex(2, &yyagent,&yycat)==FAILURE) {
					RETURN_FALSE;
				}
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(Agent, UDM_AGENT *, yyagent, id, "mnoGoSearch-Agent", le_link);
	convert_to_string_ex(yycat);
	cat = Z_STRVAL_PP(yycat);

#if UDM_VERSION_ID >= 30204
	bzero(&C,sizeof(C));
	strncpy(C.addr,cat,sizeof(C.addr)-1);
	if(!UdmCatAction(Agent,&C,UDM_CAT_ACTION_LIST,Agent->Conf->db)){
#else
	if((c=UdmCatList(Agent,cat))){
#endif
		if (array_init(return_value)==FAILURE) {
			RETURN_FALSE;
		}
		
		if (!(buf=calloc(1,UDMSTRSIZ+1))) {
			RETURN_FALSE;
		}
		
#if UDM_VERSION_ID >= 30204
		{
		    int i;
		    if (C.ncategories) {
			for(i=0;i<C.ncategories;i++){
			    snprintf(buf, UDMSTRSIZ, "%s%s",C.Category[i].link[0]?"@ ":"", C.Category[i].name);
			    add_next_index_string(return_value, C.Category[i].link[0]?C.Category[i].link:C.Category[i].path, 1);
			    add_next_index_string(return_value, buf, 1);
			}
			free(C.Category);
		    } else {
			RETURN_FALSE;
		    }
		}
#else
		while(c->rec_id){			
			snprintf(buf, UDMSTRSIZ, "%s%s",c->link[0]?"@ ":"", c->name);				 
			add_next_index_string(return_value, c->link[0]?c->link:c->path, 1);
			add_next_index_string(return_value, buf, 1);
			c++;
		}
#endif		
		free(buf);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array udm_cat_path(int agent, string category)
   Get mnoGoSearch categories path from the root to the given catgory */
DLEXPORT PHP_FUNCTION(udm_cat_path)
{
	pval ** yycat, ** yyagent;
	UDM_AGENT * Agent;
	char *cat;
#if UDM_VERSION_ID >= 30204	
	UDM_CATEGORY C;
#else	
	UDM_CATEGORY *c=NULL;
#endif
	char *buf=NULL;
	int id=-1;

	switch(ZEND_NUM_ARGS()){
		case 2: {
				if (zend_get_parameters_ex(2, &yyagent,&yycat)==FAILURE) {
					RETURN_FALSE;
				}
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE(Agent, UDM_AGENT *, yyagent, id, "mnoGoSearch-Agent", le_link);
	convert_to_string_ex(yycat);
	cat = Z_STRVAL_PP(yycat);

#if UDM_VERSION_ID >= 30204
	bzero(&C,sizeof(C));
	strncpy(C.addr,cat,sizeof(C.addr)-1);
	if(!UdmCatAction(Agent,&C,UDM_CAT_ACTION_PATH,Agent->Conf->db)){
#else
	if((c=UdmCatPath(Agent,cat))){
#endif
		if (array_init(return_value)==FAILURE) {
			RETURN_FALSE;
		}
		
		if (!(buf=calloc(1,UDMSTRSIZ+1))) {
			RETURN_FALSE;
		}
		
#if UDM_VERSION_ID >= 30204
		{
		    int i;
		    if (C.ncategories) {
			for(i=0;i<C.ncategories;i++){			
			    snprintf(buf, UDMSTRSIZ, "%s%s",C.Category[i].link[0]?"@ ":"", C.Category[i].name);
			    add_next_index_string(return_value, C.Category[i].link[0]?C.Category[i].link:C.Category[i].path, 1);
			    add_next_index_string(return_value, buf, 1);
			}
			free(C.Category);
		    } else {
			RETURN_FALSE;
		    }
		}
#else
		while(c->rec_id){			
			snprintf(buf, UDMSTRSIZ, "%s%s",c->link[0]?"@ ":"", c->name);				 
			add_next_index_string(return_value, c->link[0]?c->link:c->path, 1);
			add_next_index_string(return_value, buf, 1);
			c++;
		}
#endif		
		free(buf);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

#if UDM_VERSION_ID > 30110
/* {{{ proto int udm_get_doc_count(int agent)
   Get total number of documents in database */
DLEXPORT PHP_FUNCTION(udm_get_doc_count)
{
	pval ** yyagent;
	UDM_AGENT * Agent;
	int id=-1;

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
	ZEND_FETCH_RESOURCE(Agent, UDM_AGENT *, yyagent, id, "mnoGoSearch-Agent", le_link);
#if UDM_VERSION_ID >= 30204
	if (!Agent->doccount) UdmURLAction(Agent,NULL,UDM_URL_ACTION_DOCCOUNT,Agent->Conf->db);
	RETURN_LONG(Agent->doccount);
#else
	RETURN_LONG(UdmGetDocCount(Agent));
#endif
}
/* }}} */
#endif


#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
