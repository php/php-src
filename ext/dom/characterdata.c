/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Christian Stocker <chregu@php.net>                          |
   |          Rob Richards <rrichards@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#if HAVE_LIBXML && HAVE_DOM
#include "php_dom.h"


/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_characterdata_substring_data, 0, 0, 2)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_characterdata_append_data, 0, 0, 1)
	ZEND_ARG_INFO(0, arg)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_characterdata_insert_data, 0, 0, 2)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, arg)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_characterdata_delete_data, 0, 0, 2)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_characterdata_replace_data, 0, 0, 3)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, count)
	ZEND_ARG_INFO(0, arg)
ZEND_END_ARG_INFO();
/* }}} */

/*
* class DOMCharacterData extends DOMNode
*
* URL: https://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-FF21A306
* Since:
*/

const zend_function_entry php_dom_characterdata_class_functions[] = {
	PHP_FALIAS(substringData, dom_characterdata_substring_data, arginfo_dom_characterdata_substring_data)
	PHP_FALIAS(appendData, dom_characterdata_append_data, arginfo_dom_characterdata_append_data)
	PHP_FALIAS(insertData, dom_characterdata_insert_data, arginfo_dom_characterdata_insert_data)
	PHP_FALIAS(deleteData, dom_characterdata_delete_data, arginfo_dom_characterdata_delete_data)
	PHP_FALIAS(replaceData, dom_characterdata_replace_data, arginfo_dom_characterdata_replace_data)
	PHP_FE_END
};

/* {{{ data	string
readonly=no
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-72AB8359
Since:
*/
int dom_characterdata_data_read(dom_object *obj, zval *retval)
{
	xmlNodePtr nodep = dom_object_get_node(obj);
	xmlChar *content;

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0);
		return FAILURE;
	}

	if ((content = xmlNodeGetContent(nodep)) != NULL) {
		ZVAL_STRING(retval, (char *) content);
		xmlFree(content);
	} else {
		ZVAL_EMPTY_STRING(retval);
	}

	return SUCCESS;
}

int dom_characterdata_data_write(dom_object *obj, zval *newval)
{
	xmlNode *nodep = dom_object_get_node(obj);
	zend_string *str;

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0);
		return FAILURE;
	}

	str = zval_get_string(newval);

	xmlNodeSetContentLen(nodep, (xmlChar *) ZSTR_VAL(str), ZSTR_LEN(str) + 1);

	zend_string_release_ex(str, 0);
	return SUCCESS;
}

/* }}} */

/* {{{ length	long
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-7D61178C
Since:
*/
int dom_characterdata_length_read(dom_object *obj, zval *retval)
{
	xmlNodePtr nodep = dom_object_get_node(obj);
	xmlChar *content;
	long length = 0;

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0);
		return FAILURE;
	}

	content = xmlNodeGetContent(nodep);

	if (content) {
		length = xmlUTF8Strlen(content);
		xmlFree(content);
	}

	ZVAL_LONG(retval, length);

	return SUCCESS;
}

/* }}} */

/* {{{ proto string dom_characterdata_substring_data(int offset, int count);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-6531BCCF
Since:
*/
PHP_FUNCTION(dom_characterdata_substring_data)
{
	zval       *id;
	xmlChar    *cur;
	xmlChar    *substring;
	xmlNodePtr  node;
	zend_long        offset, count;
	int         length;
	dom_object	*intern;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oll", &id, dom_characterdata_class_entry, &offset, &count) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(node, id, xmlNodePtr, intern);

	cur = xmlNodeGetContent(node);
	if (cur == NULL) {
		RETURN_FALSE;
	}

	length = xmlUTF8Strlen(cur);

	if (offset < 0 || count < 0 || ZEND_LONG_INT_OVFL(offset) || ZEND_LONG_INT_OVFL(count) || offset > length) {
		xmlFree(cur);
		php_dom_throw_error(INDEX_SIZE_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	if ((offset + count) > length) {
		count = length - offset;
	}

	substring = xmlUTF8Strsub(cur, (int)offset, (int)count);
	xmlFree(cur);

	if (substring) {
		RETVAL_STRING((char *) substring);
		xmlFree(substring);
	} else {
		RETVAL_EMPTY_STRING();
	}
}
/* }}} end dom_characterdata_substring_data */

/* {{{ proto void dom_characterdata_append_data(string arg);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-32791A2F
Since:
*/
PHP_FUNCTION(dom_characterdata_append_data)
{
	zval *id;
	xmlNode *nodep;
	dom_object *intern;
	char *arg;
	size_t arg_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &id, dom_characterdata_class_entry, &arg, &arg_len) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);
#if LIBXML_VERSION < 20627
/* Implement logic from libxml xmlTextConcat to add support for comments and PI */
    if ((nodep->content == (xmlChar *) &(nodep->properties)) ||
        ((nodep->doc != NULL) && (nodep->doc->dict != NULL) &&
		xmlDictOwns(nodep->doc->dict, nodep->content))) {
	nodep->content = xmlStrncatNew(nodep->content, arg, arg_len);
    } else {
        nodep->content = xmlStrncat(nodep->content, arg, arg_len);
    }
    nodep->properties = NULL;
#else
	xmlTextConcat(nodep, (xmlChar *) arg, arg_len);
#endif
	RETURN_TRUE;
}
/* }}} end dom_characterdata_append_data */

/* {{{ proto void dom_characterdata_insert_data(int offset, string arg);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-3EDB695F
Since:
*/
PHP_FUNCTION(dom_characterdata_insert_data)
{
	zval *id;
	xmlChar		*cur, *first, *second;
	xmlNodePtr  node;
	char		*arg;
	zend_long        offset;
	int         length;
	size_t arg_len;
	dom_object	*intern;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ols", &id, dom_characterdata_class_entry, &offset, &arg, &arg_len) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(node, id, xmlNodePtr, intern);

	cur = xmlNodeGetContent(node);
	if (cur == NULL) {
		RETURN_FALSE;
	}

	length = xmlUTF8Strlen(cur);

	if (offset < 0 || ZEND_LONG_INT_OVFL(offset) || offset > length) {
		xmlFree(cur);
		php_dom_throw_error(INDEX_SIZE_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	first = xmlUTF8Strndup(cur, (int)offset);
	second = xmlUTF8Strsub(cur, (int)offset, length - (int)offset);
	xmlFree(cur);

	xmlNodeSetContent(node, first);
	xmlNodeAddContent(node, (xmlChar *) arg);
	xmlNodeAddContent(node, second);

	xmlFree(first);
	xmlFree(second);

	RETURN_TRUE;
}
/* }}} end dom_characterdata_insert_data */

/* {{{ proto void dom_characterdata_delete_data(int offset, int count);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-7C603781
Since:
*/
PHP_FUNCTION(dom_characterdata_delete_data)
{
	zval *id;
	xmlChar    *cur, *substring, *second;
	xmlNodePtr  node;
	zend_long        offset, count;
	int         length;
	dom_object	*intern;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oll", &id, dom_characterdata_class_entry, &offset, &count) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(node, id, xmlNodePtr, intern);

	cur = xmlNodeGetContent(node);
	if (cur == NULL) {
		RETURN_FALSE;
	}

	length = xmlUTF8Strlen(cur);

	if (offset < 0 || count < 0 || ZEND_LONG_INT_OVFL(offset) || ZEND_LONG_INT_OVFL(count) || offset > length) {
		xmlFree(cur);
		php_dom_throw_error(INDEX_SIZE_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	if (offset > 0) {
		substring = xmlUTF8Strsub(cur, 0, (int)offset);
	} else {
		substring = NULL;
	}

	if ((offset + count) > length) {
		count = length - offset;
	}

	second = xmlUTF8Strsub(cur, (int)offset + (int)count, length - (int)offset);
	substring = xmlStrcat(substring, second);

	xmlNodeSetContent(node, substring);

	xmlFree(cur);
	xmlFree(second);
	xmlFree(substring);

	RETURN_TRUE;
}
/* }}} end dom_characterdata_delete_data */

/* {{{ proto void dom_characterdata_replace_data(int offset, int count, string arg);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-E5CBA7FB
Since:
*/
PHP_FUNCTION(dom_characterdata_replace_data)
{
	zval *id;
	xmlChar		*cur, *substring, *second = NULL;
	xmlNodePtr  node;
	char		*arg;
	zend_long        offset, count;
	int         length;
	size_t arg_len;
	dom_object	*intern;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Olls", &id, dom_characterdata_class_entry, &offset, &count, &arg, &arg_len) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(node, id, xmlNodePtr, intern);

	cur = xmlNodeGetContent(node);
	if (cur == NULL) {
		RETURN_FALSE;
	}

	length = xmlUTF8Strlen(cur);

	if (offset < 0 || count < 0 || ZEND_LONG_INT_OVFL(offset) || ZEND_LONG_INT_OVFL(count) || offset > length) {
		xmlFree(cur);
		php_dom_throw_error(INDEX_SIZE_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	if (offset > 0) {
		substring = xmlUTF8Strsub(cur, 0, (int)offset);
	} else {
		substring = NULL;
	}

	if ((offset + count) > length) {
		count = length - offset;
	}

	if (offset < length) {
		second = xmlUTF8Strsub(cur, (int)offset + count, length - (int)offset);
	}

	substring = xmlStrcat(substring, (xmlChar *) arg);
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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
