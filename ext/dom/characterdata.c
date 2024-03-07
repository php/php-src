/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
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
#if defined(HAVE_LIBXML) && defined(HAVE_DOM)
#include "php_dom.h"

/*
* class DOMCharacterData extends DOMNode
*
* URL: https://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-FF21A306
* Since:
*/

/* For some peculiar reason, many of these methods operate on unsigned numbers.
 * Unfortunately, "old DOM" doesn't, so we have to conditionally convert...
 * And the reason we're using "unsigned int" instead of "unsigned zend_long" is because libxml2 internally works with ints. */
static bool dom_convert_number_unsigned(dom_object *intern, zend_long input, unsigned int *output)
{
	if (input < 0) {
		if (php_dom_follow_spec_intern(intern)) {
			*output = (unsigned int) input;
		} else {
			php_dom_throw_error(INDEX_SIZE_ERR, dom_get_strict_error(intern->document));
			return false;
		}
	} else {
		*output = input;
	}
	return true;
}

/* {{{ data	string
readonly=no
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-72AB8359
Since:
*/
zend_result dom_characterdata_data_read(dom_object *obj, zval *retval)
{
	xmlNodePtr nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	php_dom_get_content_into_zval(nodep, retval, false);

	return SUCCESS;
}

zend_result dom_characterdata_data_write(dom_object *obj, zval *newval)
{
	xmlNode *nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	/* Typed property, this is already a string */
	ZEND_ASSERT(Z_TYPE_P(newval) == IS_STRING);
	zend_string *str = Z_STR_P(newval);

	xmlNodeSetContentLen(nodep, (xmlChar *) ZSTR_VAL(str), ZSTR_LEN(str));

	return SUCCESS;
}

/* }}} */

/* {{{ length	long
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-7D61178C
Since:
*/
zend_result dom_characterdata_length_read(dom_object *obj, zval *retval)
{
	xmlNodePtr nodep = dom_object_get_node(obj);
	long length = 0;

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	if (nodep->content) {
		length = xmlUTF8Strlen(nodep->content);
	}

	ZVAL_LONG(retval, length);

	return SUCCESS;
}

/* }}} */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-6531BCCF
Modern spec URL: https://dom.spec.whatwg.org/#dom-characterdata-substringdata
Since:
*/
PHP_METHOD(DOMCharacterData, substringData)
{
	zval *id;
	xmlChar *cur;
	xmlChar *substring;
	xmlNodePtr node;
	zend_long offset_input, count_input;
	unsigned int count, offset;
	int length;
	dom_object *intern;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ll", &offset_input, &count_input) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(node, id, xmlNodePtr, intern);

	cur = node->content;
	if (cur == NULL) {
		/* TODO: is this even possible? */
		cur = BAD_CAST "";
	}

	length = xmlUTF8Strlen(cur);
	if (ZEND_LONG_INT_OVFL(offset_input) || ZEND_LONG_INT_OVFL(count_input)) {
		php_dom_throw_error(INDEX_SIZE_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	if (!dom_convert_number_unsigned(intern, offset_input, &offset) || !dom_convert_number_unsigned(intern, count_input, &count)) {
		RETURN_FALSE;
	}

	if (offset > length) {
		php_dom_throw_error(INDEX_SIZE_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	if ((offset + count) > length) {
		count = length - offset;
	}

	substring = xmlUTF8Strsub(cur, (int)offset, (int)count);

	if (substring) {
		RETVAL_STRING((char *) substring);
		xmlFree(substring);
	} else {
		RETVAL_EMPTY_STRING();
	}
}
/* }}} end dom_characterdata_substring_data */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-32791A2F
Modern spec URL: https://dom.spec.whatwg.org/#dom-characterdata-appenddata
Since:
*/
static void dom_character_data_append_data(INTERNAL_FUNCTION_PARAMETERS, bool return_true)
{
	zval *id;
	xmlNode *nodep;
	dom_object *intern;
	char *arg;
	size_t arg_len;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &arg, &arg_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);
	xmlTextConcat(nodep, (xmlChar *) arg, arg_len);
	if (return_true) {
		RETURN_TRUE;
	}
}

PHP_METHOD(DOMCharacterData, appendData)
{
	dom_character_data_append_data(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

PHP_METHOD(DOM_CharacterData, appendData)
{
	dom_character_data_append_data(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}
/* }}} end dom_characterdata_append_data */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-3EDB695F
Modern spec URL: https://dom.spec.whatwg.org/#dom-characterdata-insertdata
Since:
*/
static void dom_character_data_insert_data(INTERNAL_FUNCTION_PARAMETERS, bool return_true)
{
	zval *id;
	xmlChar		*cur, *first, *second;
	xmlNodePtr  node;
	char		*arg;
	zend_long        offset_input;
	unsigned int offset;
	int         length;
	size_t arg_len;
	dom_object	*intern;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ls", &offset_input, &arg, &arg_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(node, id, xmlNodePtr, intern);

	cur = node->content;
	if (cur == NULL) {
		/* TODO: is this even possible? */
		cur = BAD_CAST "";
	}

	length = xmlUTF8Strlen(cur);

	if (ZEND_LONG_INT_OVFL(offset_input)) {
		php_dom_throw_error(INDEX_SIZE_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	if (!dom_convert_number_unsigned(intern, offset_input, &offset)) {
		RETURN_FALSE;
	}

	if (offset > length) {
		php_dom_throw_error(INDEX_SIZE_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	first = xmlUTF8Strndup(cur, (int)offset);
	second = xmlUTF8Strsub(cur, (int)offset, length - (int)offset);

	xmlNodeSetContent(node, first);
	xmlNodeAddContent(node, (xmlChar *) arg);
	xmlNodeAddContent(node, second);

	xmlFree(first);
	xmlFree(second);

	if (return_true) {
		RETURN_TRUE;
	}
}

PHP_METHOD(DOMCharacterData, insertData)
{
	dom_character_data_insert_data(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

PHP_METHOD(DOM_CharacterData, insertData)
{
	dom_character_data_insert_data(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}
/* }}} end dom_characterdata_insert_data */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-7C603781
Modern spec URL: https://dom.spec.whatwg.org/#dom-characterdata-deletedata
Since:
*/
static void dom_character_data_delete_data(INTERNAL_FUNCTION_PARAMETERS, bool return_true)
{
	zval *id;
	xmlChar    *cur, *substring, *second;
	xmlNodePtr  node;
	zend_long        offset, count_input;
	unsigned int count;
	int         length;
	dom_object	*intern;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ll", &offset, &count_input) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(node, id, xmlNodePtr, intern);

	cur = node->content;
	if (cur == NULL) {
		/* TODO: is this even possible? */
		cur = BAD_CAST "";
	}

	length = xmlUTF8Strlen(cur);

	if (offset < 0 || ZEND_LONG_INT_OVFL(offset) || ZEND_LONG_INT_OVFL(count_input) || offset > length) {
		php_dom_throw_error(INDEX_SIZE_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	if (!dom_convert_number_unsigned(intern, count_input, &count)) {
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

	xmlFree(second);
	xmlFree(substring);

	if (return_true) {
		RETURN_TRUE;
	}
}

PHP_METHOD(DOMCharacterData, deleteData)
{
	dom_character_data_delete_data(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

PHP_METHOD(DOM_CharacterData, deleteData)
{
	dom_character_data_delete_data(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}
/* }}} end dom_characterdata_delete_data */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-E5CBA7FB
Modern spec URL: https://dom.spec.whatwg.org/#dom-characterdata-replacedata
Since:
*/
static void dom_character_data_replace_data(INTERNAL_FUNCTION_PARAMETERS, bool return_true)
{
	zval *id;
	xmlChar		*cur, *substring, *second = NULL;
	xmlNodePtr  node;
	char		*arg;
	zend_long        offset, count_input;
	unsigned int count;
	int         length;
	size_t arg_len;
	dom_object	*intern;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "lls", &offset, &count_input, &arg, &arg_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(node, id, xmlNodePtr, intern);

	cur = node->content;
	if (cur == NULL) {
		/* TODO: is this even possible? */
		cur = BAD_CAST "";
	}

	length = xmlUTF8Strlen(cur);

	if (offset < 0 || ZEND_LONG_INT_OVFL(offset) || ZEND_LONG_INT_OVFL(count_input) || offset > length) {
		php_dom_throw_error(INDEX_SIZE_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	if (!dom_convert_number_unsigned(intern, count_input, &count)) {
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

	if (second) {
		xmlFree(second);
	}
	xmlFree(substring);

	if (return_true) {
		RETURN_TRUE;
	}
}

PHP_METHOD(DOMCharacterData, replaceData)
{
	dom_character_data_replace_data(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

PHP_METHOD(DOM_CharacterData, replaceData)
{
	dom_character_data_replace_data(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}
/* }}} end dom_characterdata_replace_data */

#endif
