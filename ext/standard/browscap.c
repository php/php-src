/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"
#include "php_regex.h"
#include "php_browscap.h"
#include "php_ini.h"

#include "zend_globals.h"

HashTable browser_hash;

#define DEFAULT_SECTION_NAME "Default Browser Capability Settings"

static int browser_reg_compare(zval **browser,int num_args, va_list args, zend_hash_key *key)
{
	zval **browser_name;
	regex_t r;
	char *lookup_browser_name = va_arg(args,char *);
	zval **found_browser_entry = va_arg(args,zval **);

	if (*found_browser_entry) { /* already found */
		return 0;
	}
	if(zend_hash_find((*browser)->value.obj.properties, "browser_name_pattern",sizeof("browser_name_pattern"),(void **) &browser_name) == FAILURE) {
		return 0;
	}

	if (!strchr((*browser_name)->value.str.val,'*')) {
		return 0;
	}
	if (regcomp(&r,(*browser_name)->value.str.val,REG_NOSUB)!=0) {
		return 0;
	}
	if (regexec(&r,lookup_browser_name,0,NULL,0)==0) {
		*found_browser_entry = *browser;
	}
	regfree(&r);
	return 0;
}

/* {{{ proto object get_browser(string browser_name)
   Get information about the capabilities of a browser */
PHP_FUNCTION(get_browser)
{
	zval **agent_name,**agent;
	zval *found_browser_entry;
	char *lookup_browser_name;

	if (!INI_STR("browscap")) {
		RETURN_FALSE;
	}
	
	switch(ZEND_NUM_ARGS()) {
		case 0:
			if (zend_hash_find(&EG(symbol_table), "HTTP_USER_AGENT", sizeof("HTTP_USER_AGENT"), (void **) &agent_name)==FAILURE) {
				zend_error(E_WARNING,"HTTP_USER_AGENT variable is not set, cannot determine user agent name");
				RETURN_FALSE;
			}
			break;
		case 1:
			if (zend_get_parameters_ex(1,&agent_name)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	convert_to_string_ex(agent_name);

	if (zend_hash_find(&browser_hash, (*agent_name)->value.str.val,(*agent_name)->value.str.len+1, (void **) &agent)==FAILURE) {
		lookup_browser_name = (*agent_name)->value.str.val;
		found_browser_entry = NULL;
		zend_hash_apply_with_arguments(&browser_hash,(int (*)(void *, int, va_list, zend_hash_key *)) browser_reg_compare,2,lookup_browser_name,&found_browser_entry);
		
		if (found_browser_entry) {
			agent = &found_browser_entry;
		} else if (zend_hash_find(&browser_hash, DEFAULT_SECTION_NAME, sizeof(DEFAULT_SECTION_NAME), (void **) &agent)==FAILURE) {
			RETURN_FALSE;
		}
	}
	
	*return_value = **agent;
	return_value->type = IS_OBJECT;
	zval_copy_ctor(return_value);
	return_value->value.obj.properties->pDestructor = ZVAL_DESTRUCTOR;

	while (zend_hash_find((*agent)->value.obj.properties, "parent",sizeof("parent"), (void **) &agent_name)==SUCCESS) {
		zval *tmp_copy;

		if (zend_hash_find(&browser_hash,(*agent_name)->value.str.val, (*agent_name)->value.str.len+1, (void **)&agent)==FAILURE) {
			break;
		}
		zend_hash_merge(return_value->value.obj.properties,(*agent)->value.obj.properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp_copy, sizeof(zval *), 0);
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
