/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Christian Stocker <chregu@php.net>                          |
   |          Rob Richards <rrichards@php.net>                            |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_dom.h"


/*
* class domcharacterdata extends domnode 
*
* URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-FF21A306
* Since: 
*/

zend_function_entry php_dom_characterdata_class_functions[] = {
	PHP_FALIAS(substringData, dom_characterdata_substring_data, NULL)
	PHP_FALIAS(appendData, dom_characterdata_append_data, NULL)
	PHP_FALIAS(insertData, dom_characterdata_insert_data, NULL)
	PHP_FALIAS(deleteData, dom_characterdata_delete_data, NULL)
	PHP_FALIAS(replaceData, dom_characterdata_replace_data, NULL)
	{NULL, NULL, NULL}
};

/* {{{ proto data	string	
readonly=no 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-72AB8359
Since: 
*/
int dom_characterdata_data_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlNodePtr nodep;
	xmlChar *content;

	nodep = obj->ptr;

	ALLOC_ZVAL(*retval);
	
	if ((content = xmlNodeGetContent(nodep)) != NULL) {
		ZVAL_STRING(*retval, content, 1);
	} else {
		ZVAL_EMPTY_STRING(*retval);
	}

	xmlFree(content);

	return SUCCESS;
}

int dom_characterdata_data_write(dom_object *obj, zval *newval TSRMLS_DC)
{
	xmlNode *nodep;

	nodep = obj->ptr;
	xmlNodeSetContentLen(nodep, Z_STRVAL_P(newval), Z_STRLEN_P(newval) + 1);
	return SUCCESS;
}

/* }}} */


long dom_utf16Length (xmlChar *utf8str) {
	long len = 0L, i;
	char c;

	for (i = 0L; (c = utf8str[i]) != '\0'; i++)
		if ((c & 0xf8) == 0xf0)
			len += 2L;
		else if ((c & 0xc0) != 0x80)
			len++;

	return len;
}

/* {{{ proto length	unsigned long	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-7D61178C
Since: 
*/
int dom_characterdata_length_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlNodePtr nodep;
	xmlChar *content;
	long length;

	nodep = obj->ptr;

	ALLOC_ZVAL(*retval);
	
	content = xmlNodeGetContent(nodep);
	length = dom_utf16Length(content);
	xmlFree(content);
	ZVAL_LONG(*retval, length);

	return SUCCESS;
}

/* }}} */




/* {{{ proto domstring dom_characterdata_substring_data(unsigned long offset, unsigned long count);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-6531BCCF
Since: 
*/
PHP_FUNCTION(dom_characterdata_substring_data)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_characterdata_substring_data */


/* {{{ proto dom_void dom_characterdata_append_data(string arg);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-32791A2F
Since: 
*/
PHP_FUNCTION(dom_characterdata_append_data)
{
	zval *id;
	xmlNode *nodep;
	char *arg;
	int arg_len;


	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	xmlTextConcat(nodep, arg, arg_len);

	RETURN_TRUE;
}
/* }}} end dom_characterdata_append_data */


/* {{{ proto dom_void dom_characterdata_insert_data(unsigned long offset, string arg);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-3EDB695F
Since: 
*/
PHP_FUNCTION(dom_characterdata_insert_data)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_characterdata_insert_data */


/* {{{ proto dom_void dom_characterdata_delete_data(unsigned long offset, unsigned long count);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-7C603781
Since: 
*/
PHP_FUNCTION(dom_characterdata_delete_data)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_characterdata_delete_data */


/* {{{ proto dom_void dom_characterdata_replace_data(unsigned long offset, unsigned long count, string arg);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-E5CBA7FB
Since: 
*/
PHP_FUNCTION(dom_characterdata_replace_data)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_characterdata_replace_data */
