/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
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

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#if HAVE_LIBXML && HAVE_DOM
#include "php_dom.h"

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_documentfragement_construct, 0, 0, 0)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_documentfragement_appendXML, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO();
/* }}} */

/*
* class DOMDocumentFragment extends DOMNode
*
* URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-B63ED1A3
* Since:
*/

const zend_function_entry php_dom_documentfragment_class_functions[] = {
	PHP_ME(domdocumentfragment, __construct, arginfo_dom_documentfragement_construct, ZEND_ACC_PUBLIC)
	PHP_ME(domdocumentfragment, appendXML, arginfo_dom_documentfragement_appendXML, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/* {{{ proto void DOMDocumentFragment::__construct() */
PHP_METHOD(domdocumentfragment, __construct)
{

	zval *id = getThis();
	xmlNodePtr nodep = NULL, oldnode = NULL;
	dom_object *intern;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "") == FAILURE) {
		return;
	}

	nodep = xmlNewDocFragment(NULL);

	if (!nodep) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		RETURN_FALSE;
	}

	intern = Z_DOMOBJ_P(id);
	oldnode = dom_object_get_node(intern);
	if (oldnode != NULL) {
		php_libxml_node_free_resource(oldnode );
	}
	/* php_dom_set_object(intern, nodep); */
	php_libxml_increment_node_ptr((php_libxml_node_object *)intern, nodep, (void *)intern);
}
/* }}} end DOMDocumentFragment::__construct */

/* php_dom_xmlSetTreeDoc is a custom implementation of xmlSetTreeDoc
 needed for hack in appendXML due to libxml bug - no need to share this function */
static void php_dom_xmlSetTreeDoc(xmlNodePtr tree, xmlDocPtr doc) /* {{{ */
{
    xmlAttrPtr prop;
	xmlNodePtr cur;

    if (tree) {
		if(tree->type == XML_ELEMENT_NODE) {
			prop = tree->properties;
			while (prop != NULL) {
				prop->doc = doc;
				if (prop->children) {
					cur = prop->children;
					while (cur != NULL) {
						php_dom_xmlSetTreeDoc(cur, doc);
						cur = cur->next;
					}
				}
				prop = prop->next;
			}
		}
		if (tree->children != NULL) {
			cur = tree->children;
			while (cur != NULL) {
				php_dom_xmlSetTreeDoc(cur, doc);
				cur = cur->next;
			}
		}
		tree->doc = doc;
    }
}
/* }}} */

/* {{{ proto void DOMDocumentFragment::appendXML(string data) */
PHP_METHOD(domdocumentfragment, appendXML) {
	zval *id;
	xmlNode *nodep;
	dom_object *intern;
	char *data = NULL;
	size_t data_len = 0;
	int err;
	xmlNodePtr lst;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &id, dom_documentfragment_class_entry, &data, &data_len) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	if (dom_node_is_read_only(nodep) == SUCCESS) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	if (data) {
		err = xmlParseBalancedChunkMemory(nodep->doc, NULL, NULL, 0, (xmlChar *) data, &lst);
		if (err != 0) {
			RETURN_FALSE;
		}
		/* Following needed due to bug in libxml2 <= 2.6.14
		ifdef after next libxml release as bug is fixed in their cvs */
		php_dom_xmlSetTreeDoc(lst, nodep->doc);
		/* End stupid hack */

		xmlAddChildList(nodep,lst);
	}

	RETURN_TRUE;
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
