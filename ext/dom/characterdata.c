/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2004 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
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
#if HAVE_LIBXML && HAVE_DOM
#include "php_dom.h"


/*
* class domcharacterdata extends domnode 
*
* URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-FF21A306
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
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-72AB8359
Since: 
*/
int dom_characterdata_data_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlNodePtr nodep;
	xmlChar *content;

	nodep = dom_object_get_node(obj);

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

	nodep = dom_object_get_node(obj);
	xmlNodeSetContentLen(nodep, Z_STRVAL_P(newval), Z_STRLEN_P(newval) + 1);
	return SUCCESS;
}

/* }}} */

/* {{{ proto length	unsigned long	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-7D61178C
Since: 
*/
int dom_characterdata_length_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlNodePtr nodep;
	xmlChar *content;
	long length;

	nodep = dom_object_get_node(obj);

	ALLOC_ZVAL(*retval);
	
	content = xmlNodeGetContent(nodep);
	length = xmlUTF8Strlen(content);

	xmlFree(content);
	ZVAL_LONG(*retval, length);

	return SUCCESS;
}

/* }}} */




/* {{{ proto domstring dom_characterdata_substring_data(unsigned long offset, unsigned long count);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-6531BCCF
Since: 
*/
PHP_FUNCTION(dom_characterdata_substring_data)
{
	xmlChar    *cur;
	xmlChar    *substring;
	xmlNodePtr  node;
	long        offset, count;
	int         length;
	dom_object	*intern;

	DOM_GET_THIS_OBJ(node, getThis(), xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &offset, &count) == FAILURE) {
		return;
	}

	cur = xmlNodeGetContent(node);
	if (cur == NULL) {
		RETURN_FALSE;
	}

	length = xmlUTF8Strlen(cur);

	if (offset < 0 || count < 0 || offset > length) {
		xmlFree(cur);
		php_dom_throw_error(INDEX_SIZE_ERR, dom_get_strict_error(intern->document) TSRMLS_CC);
		RETURN_FALSE;
	}

	if ((offset + count) > length) {
		count = length - offset;
	}

	substring = xmlUTF8Strsub(cur, offset, count);
	xmlFree(cur);

	if (substring) {
		RETVAL_STRING(substring, 1);
		xmlFree(substring);
	} else {
		RETVAL_EMPTY_STRING();
	}
}
/* }}} end dom_characterdata_substring_data */


/* {{{ proto dom_void dom_characterdata_append_data(string arg);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-32791A2F
Since: 
*/
PHP_FUNCTION(dom_characterdata_append_data)
{
	zval *id;
	xmlNode *nodep;
	dom_object *intern;
	char *arg;
	int arg_len;


	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	xmlTextConcat(nodep, arg, arg_len);

	RETURN_TRUE;
}
/* }}} end dom_characterdata_append_data */


/* {{{ proto dom_void dom_characterdata_insert_data(unsigned long offset, string arg);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-3EDB695F
Since: 
*/
PHP_FUNCTION(dom_characterdata_insert_data)
{
	xmlChar		*cur, *first, *second;
	xmlNodePtr  node;
	char		*arg;
	long        offset;
	int         length, arg_len;
	dom_object	*intern;

	DOM_GET_THIS_OBJ(node, getThis(), xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &offset, &arg, &arg_len) == FAILURE) {
		return;
	}

	cur = xmlNodeGetContent(node);
	if (cur == NULL) {
		RETURN_FALSE;
	}

	length = xmlUTF8Strlen(cur);

	if (offset < 0 || offset > length) {
		xmlFree(cur);
		php_dom_throw_error(INDEX_SIZE_ERR, dom_get_strict_error(intern->document) TSRMLS_CC);
		RETURN_FALSE;
	}

	first = xmlUTF8Strndup(cur, offset);
	second = xmlUTF8Strsub(cur, offset, length - offset);
	xmlFree(cur);

	xmlNodeSetContent(node, first);
	xmlNodeAddContent(node, arg);
	xmlNodeAddContent(node, second);
	
	xmlFree(first);
	xmlFree(second);

	RETURN_TRUE;
}
/* }}} end dom_characterdata_insert_data */


/* {{{ proto dom_void dom_characterdata_delete_data(unsigned long offset, unsigned long count);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-7C603781
Since: 
*/
PHP_FUNCTION(dom_characterdata_delete_data)
{
	xmlChar    *cur, *substring, *second;
	xmlNodePtr  node;
	long        offset, count;
	int         length;
	dom_object	*intern;

	DOM_GET_THIS_OBJ(node, getThis(), xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &offset, &count) == FAILURE) {
		return;
	}

	cur = xmlNodeGetContent(node);
	if (cur == NULL) {
		RETURN_FALSE;
	}

	length = xmlUTF8Strlen(cur);

	if (offset < 0 || count < 0 || offset > length) {
		xmlFree(cur);
		php_dom_throw_error(INDEX_SIZE_ERR, dom_get_strict_error(intern->document) TSRMLS_CC);
		RETURN_FALSE;
	}

	if (offset > 0) {
		substring = xmlUTF8Strsub(cur, 0, offset);
	} else {
		substring = NULL;
	}

	if ((offset + count) > length) {
		count = length - offset;
	}

	second = xmlUTF8Strsub(cur, offset + count, length - offset);
	substring = xmlStrcat(substring, second);

	xmlNodeSetContent(node, substring);

	xmlFree(cur);
	xmlFree(second);
	xmlFree(substring);

	RETURN_TRUE;
}
/* }}} end dom_characterdata_delete_data */


/* {{{ proto dom_void dom_characterdata_replace_data(unsigned long offset, unsigned long count, string arg);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-E5CBA7FB
Since: 
*/
PHP_FUNCTION(dom_characterdata_replace_data)
{
	xmlChar		*cur, *substring, *second = NULL;
	xmlNodePtr  node;
	char		*arg;
	long        offset, count;
	int         length, arg_len;
	dom_object	*intern;

	DOM_GET_THIS_OBJ(node, getThis(), xmlNodePtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lls", &offset, &count, &arg, &arg_len) == FAILURE) {
		return;
	}

	cur = xmlNodeGetContent(node);
	if (cur == NULL) {
		RETURN_FALSE;
	}

	length = xmlUTF8Strlen(cur);

	if (offset < 0 || count < 0 || offset > length) {
		xmlFree(cur);
		php_dom_throw_error(INDEX_SIZE_ERR, dom_get_strict_error(intern->document) TSRMLS_CC);
		RETURN_FALSE;
	}

	if (offset > 0) {
		substring = xmlUTF8Strsub(cur, 0, offset);
	} else {
		substring = NULL;
	}

	if ((offset + count) > length) {
		count = length - offset;
	}

	if (offset < length) {
		second = xmlUTF8Strsub(cur, offset + count, length - offset);
	}

	substring = xmlStrcat(substring, arg);
	substring = xmlStrcat(substring, second);

	xmlNodeSetContent(node, substring);

	xmlFree(cur);
	if (second) {
		xmlFree(second);
	}
	xmlFree(substring);

	RETURN_TRUE;
}
/* }}} end dom_characterdata_replace_data */
#endif
