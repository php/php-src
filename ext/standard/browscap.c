/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */

#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "php.h"
#include "php3_browscap.h"
#include "php_ini.h"

#include "zend_globals.h"

#ifndef THREAD_SAFE
HashTable browser_hash;
static char *lookup_browser_name;
static pval *found_browser_entry;
#endif

function_entry browscap_functions[] = {
	{"get_browser",		php3_get_browser,		NULL},
	{NULL, NULL, NULL}
};

php3_module_entry browscap_module_entry = {
	"browscap", browscap_functions, php3_minit_browscap, php3_mshutdown_browscap, NULL, NULL, NULL, STANDARD_MODULE_PROPERTIES
};


static int browser_reg_compare(pval *browser)
{
	pval *browser_name;
	regex_t r;
	TLS_VARS;

	if (GLOBAL(found_browser_entry)) { /* already found */
		return 0;
	}
	_php3_hash_find(browser->value.ht,"browser_name_pattern",sizeof("browser_name_pattern"),(void **) &browser_name);
	if (!strchr(browser_name->value.str.val,'*')) {
		return 0;
	}
	if (regcomp(&r,browser_name->value.str.val,REG_NOSUB)!=0) {
		return 0;
	}
	if (regexec(&r,GLOBAL(lookup_browser_name),0,NULL,0)==0) {
		GLOBAL(found_browser_entry) = browser;
	}
	regfree(&r);
	return 0;
}

void php3_get_browser(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *agent_name,*agent,tmp;
	ELS_FETCH();

	if (!INI_STR("browscap")) {
		RETURN_FALSE;
	}
	
	switch(ARG_COUNT(ht)) {
		case 0:
			if (_php3_hash_find(&EG(symbol_table), "HTTP_USER_AGENT", sizeof("HTTP_USER_AGENT"), (void **) &agent_name)==FAILURE) {
				agent_name = &tmp;
				var_reset(agent_name);
			}
			break;
		case 1:
			if (getParameters(ht, 1, &agent_name)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	convert_to_string(agent_name);

	if (_php3_hash_find(&GLOBAL(browser_hash), agent_name->value.str.val, agent_name->value.str.len+1, (void **) &agent)==FAILURE) {
		GLOBAL(lookup_browser_name) = agent_name->value.str.val;
		GLOBAL(found_browser_entry) = NULL;
		_php3_hash_apply(&GLOBAL(browser_hash),(int (*)(void *)) browser_reg_compare);
		
		if (GLOBAL(found_browser_entry)) {
			agent = GLOBAL(found_browser_entry);
		} else if (_php3_hash_find(&GLOBAL(browser_hash), "Default Browser", sizeof("Default Browser"), (void **) &agent)==FAILURE) {
			RETURN_FALSE;
		}
	}
	
	*return_value = *agent;
	return_value->type = IS_OBJECT;
	pval_copy_constructor(return_value);
	return_value->value.ht->pDestructor = PVAL_DESTRUCTOR;

	while (_php3_hash_find(agent->value.ht, "parent", sizeof("parent"), (void **) &agent_name)==SUCCESS) {
		if (_php3_hash_find(&GLOBAL(browser_hash), agent_name->value.str.val, agent_name->value.str.len+1, (void **) &agent)==FAILURE) {
			break;
		}
		_php3_hash_merge(return_value->value.ht,agent->value.ht,(void (*)(void *pData)) pval_copy_constructor, (void *) &tmp, sizeof(pval));
	}
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
