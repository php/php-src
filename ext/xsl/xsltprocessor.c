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
#include "php_xsl.h"
#include "ext/libxml/php_libxml.h"

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_xsl_xsltprocessor_import_stylesheet, 0, 0, 1)
	ZEND_ARG_INFO(0, doc)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_xsl_xsltprocessor_transform_to_doc, 0, 0, 1)
	ZEND_ARG_INFO(0, doc)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_xsl_xsltprocessor_transform_to_uri, 0, 0, 2)
	ZEND_ARG_INFO(0, doc)
	ZEND_ARG_INFO(0, uri)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_xsl_xsltprocessor_transform_to_xml, 0, 0, 1)
	ZEND_ARG_INFO(0, doc)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_xsl_xsltprocessor_set_parameter, 0, 0, 2)
	ZEND_ARG_INFO(0, namespace)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_xsl_xsltprocessor_get_parameter, 0, 0, 2)
	ZEND_ARG_INFO(0, namespace)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_xsl_xsltprocessor_remove_parameter, 0, 0, 2)
	ZEND_ARG_INFO(0, namespace)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_xsl_xsltprocessor_has_exslt_support, 0, 0, 0)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_xsl_xsltprocessor_register_php_functions, 0, 0, 0)
	ZEND_ARG_INFO(0, restrict)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_xsl_xsltprocessor_set_profiling, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_xsl_xsltprocessor_set_security_prefs, 0, 0, 1)
	ZEND_ARG_INFO(0, securityPrefs)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_xsl_xsltprocessor_get_security_prefs, 0, 0, 0)
ZEND_END_ARG_INFO();
/* }}} */

/*
* class xsl_xsltprocessor
*
* URL: https://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#
* Since:
*/

const zend_function_entry php_xsl_xsltprocessor_class_functions[] = {
	PHP_FALIAS(importStylesheet, xsl_xsltprocessor_import_stylesheet, arginfo_xsl_xsltprocessor_import_stylesheet)
	PHP_FALIAS(transformToDoc, xsl_xsltprocessor_transform_to_doc, arginfo_xsl_xsltprocessor_transform_to_doc)
	PHP_FALIAS(transformToUri, xsl_xsltprocessor_transform_to_uri, arginfo_xsl_xsltprocessor_transform_to_uri)
	PHP_FALIAS(transformToXml, xsl_xsltprocessor_transform_to_xml, arginfo_xsl_xsltprocessor_transform_to_xml)
	PHP_FALIAS(setParameter, xsl_xsltprocessor_set_parameter, arginfo_xsl_xsltprocessor_set_parameter)
	PHP_FALIAS(getParameter, xsl_xsltprocessor_get_parameter, arginfo_xsl_xsltprocessor_get_parameter)
	PHP_FALIAS(removeParameter, xsl_xsltprocessor_remove_parameter, arginfo_xsl_xsltprocessor_remove_parameter)
	PHP_FALIAS(hasExsltSupport, xsl_xsltprocessor_has_exslt_support, arginfo_xsl_xsltprocessor_has_exslt_support)
	PHP_FALIAS(registerPHPFunctions, xsl_xsltprocessor_register_php_functions, arginfo_xsl_xsltprocessor_register_php_functions)
	PHP_FALIAS(setProfiling, xsl_xsltprocessor_set_profiling, arginfo_xsl_xsltprocessor_set_profiling)
	PHP_FALIAS(setSecurityPrefs, xsl_xsltprocessor_set_security_prefs, arginfo_xsl_xsltprocessor_set_security_prefs)
	PHP_FALIAS(getSecurityPrefs, xsl_xsltprocessor_get_security_prefs, arginfo_xsl_xsltprocessor_get_security_prefs)
	PHP_FE_END
};

/* {{{ php_xsl_xslt_string_to_xpathexpr()
   Translates a string to a XPath Expression */
static char *php_xsl_xslt_string_to_xpathexpr(const char *str)
{
	const xmlChar *string = (const xmlChar *)str;

	xmlChar *value;
	int str_len;

	str_len = xmlStrlen(string) + 3;

	if (xmlStrchr(string, '"')) {
		if (xmlStrchr(string, '\'')) {
			php_error_docref(NULL, E_WARNING, "Cannot create XPath expression (string contains both quote and double-quotes)");
			return NULL;
		}
		value = (xmlChar*) safe_emalloc (str_len, sizeof(xmlChar), 0);
		snprintf((char*)value, str_len, "'%s'", string);
	} else {
		value = (xmlChar*) safe_emalloc (str_len, sizeof(xmlChar), 0);
		snprintf((char *)value, str_len, "\"%s\"", string);
	}
	return (char *) value;
}
/* }}} */

/* {{{ php_xsl_xslt_make_params()
   Translates a PHP array to a libxslt parameters array */
static char **php_xsl_xslt_make_params(HashTable *parht, int xpath_params)
{

	int parsize;
	zval *value;
	char *xpath_expr;
	zend_string *string_key;
	char **params = NULL;
	int i = 0;

	parsize = (2 * zend_hash_num_elements(parht) + 1) * sizeof(char *);
	params = (char **)safe_emalloc((2 * zend_hash_num_elements(parht) + 1), sizeof(char *), 0);
	memset((char *)params, 0, parsize);

	ZEND_HASH_FOREACH_STR_KEY_VAL(parht, string_key, value) {
		if (string_key == NULL) {
			php_error_docref(NULL, E_WARNING, "Invalid argument or parameter array");
			efree(params);
			return NULL;
		} else {
			if (Z_TYPE_P(value) != IS_STRING) {
				convert_to_string(value);
			}

			if (!xpath_params) {
				xpath_expr = php_xsl_xslt_string_to_xpathexpr(Z_STRVAL_P(value));
			} else {
				xpath_expr = estrndup(Z_STRVAL_P(value), Z_STRLEN_P(value));
			}
			if (xpath_expr) {
				params[i++] = estrndup(ZSTR_VAL(string_key), ZSTR_LEN(string_key));
				params[i++] = xpath_expr;
			}
		}
	} ZEND_HASH_FOREACH_END();

	params[i++] = NULL;

	return params;
}
/* }}} */

static void xsl_ext_function_php(xmlXPathParserContextPtr ctxt, int nargs, int type) /* {{{ */
{
	xsltTransformContextPtr tctxt;
	zval *args;
	zval retval;
	int result, i;
	int error = 0;
	zend_fcall_info fci;
	zval handler;
	xmlXPathObjectPtr obj;
	char *str;
	xsl_object *intern;
	zend_string *callable = NULL;


	if (! zend_is_executing()) {
		xsltGenericError(xsltGenericErrorContext,
		"xsltExtFunctionTest: Function called from outside of PHP\n");
		error = 1;
	} else {
		tctxt = xsltXPathGetTransformContext(ctxt);
		if (tctxt == NULL) {
			xsltGenericError(xsltGenericErrorContext,
			"xsltExtFunctionTest: failed to get the transformation context\n");
			error = 1;
		} else {
			intern = (xsl_object*)tctxt->_private;
			if (intern == NULL) {
				xsltGenericError(xsltGenericErrorContext,
				"xsltExtFunctionTest: failed to get the internal object\n");
				error = 1;
			}
			else if (intern->registerPhpFunctions == 0) {
				xsltGenericError(xsltGenericErrorContext,
				"xsltExtFunctionTest: PHP Object did not register PHP functions\n");
				error = 1;
			}
		}
	}

	if (error == 1) {
		for (i = nargs - 1; i >= 0; i--) {
			obj = valuePop(ctxt);
			if (obj) {
				xmlXPathFreeObject(obj);
			}
		}
		return;
	}

	fci.param_count = nargs - 1;
	if (fci.param_count > 0) {
		args = safe_emalloc(fci.param_count, sizeof(zval), 0);
	}
	/* Reverse order to pop values off ctxt stack */
	for (i = nargs - 2; i >= 0; i--) {
		obj = valuePop(ctxt);
		if (obj == NULL) {
			ZVAL_NULL(&args[i]);
			continue;
		}
		switch (obj->type) {
			case XPATH_STRING:
				ZVAL_STRING(&args[i], (char *)obj->stringval);
				break;
			case XPATH_BOOLEAN:
				ZVAL_BOOL(&args[i],  obj->boolval);
				break;
			case XPATH_NUMBER:
				ZVAL_DOUBLE(&args[i], obj->floatval);
				break;
			case XPATH_NODESET:
				if (type == 1) {
					str = (char*)xmlXPathCastToString(obj);
					ZVAL_STRING(&args[i], str);
					xmlFree(str);
				} else if (type == 2) {
					int j;
					dom_object *domintern = (dom_object *)intern->doc;
					if (obj->nodesetval && obj->nodesetval->nodeNr > 0) {
						array_init(&args[i]);
						for (j = 0; j < obj->nodesetval->nodeNr; j++) {
							xmlNodePtr node = obj->nodesetval->nodeTab[j];
							zval child;
							/* not sure, if we need this... it's copied from xpath.c */
							if (node->type == XML_NAMESPACE_DECL) {
								xmlNsPtr curns;
								xmlNodePtr nsparent;

								nsparent = node->_private;
								curns = xmlNewNs(NULL, node->name, NULL);
								if (node->children) {
									curns->prefix = xmlStrdup((xmlChar *)node->children);
								}
								if (node->children) {
									node = xmlNewDocNode(node->doc, NULL, (xmlChar *) node->children, node->name);
								} else {
									node = xmlNewDocNode(node->doc, NULL, (const xmlChar *) "xmlns", node->name);
								}
								node->type = XML_NAMESPACE_DECL;
								node->parent = nsparent;
								node->ns = curns;
							} else {
								node = xmlDocCopyNodeList(domintern->document->ptr, node);
							}

							php_dom_create_object(node, &child, domintern);
							add_next_index_zval(&args[i], &child);
						}
					} else {
						ZVAL_EMPTY_ARRAY(&args[i]);
					}
				}
				break;
			default:
				str = (char *) xmlXPathCastToString(obj);
				ZVAL_STRING(&args[i], str);
				xmlFree(str);
		}
		xmlXPathFreeObject(obj);
	}

	fci.size = sizeof(fci);
	if (fci.param_count > 0) {
		fci.params = args;
	} else {
		fci.params = NULL;
	}


	obj = valuePop(ctxt);
	if (obj == NULL || obj->stringval == NULL) {
		php_error_docref(NULL, E_WARNING, "Handler name must be a string");
		xmlXPathFreeObject(obj);
		valuePush(ctxt, xmlXPathNewString((const xmlChar *) ""));
		if (fci.param_count > 0) {
			for (i = 0; i < nargs - 1; i++) {
				zval_ptr_dtor(&args[i]);
			}
			efree(args);
		}
		return;
	}
	ZVAL_STRING(&handler, (char *) obj->stringval);
	xmlXPathFreeObject(obj);

	ZVAL_COPY_VALUE(&fci.function_name, &handler);
	fci.object = NULL;
	fci.retval = &retval;
	fci.no_separation = 0;
	/*fci.function_handler_cache = &function_ptr;*/
	if (!zend_make_callable(&handler, &callable)) {
		php_error_docref(NULL, E_WARNING, "Unable to call handler %s()", ZSTR_VAL(callable));
		valuePush(ctxt, xmlXPathNewString((const xmlChar *) ""));
	} else if ( intern->registerPhpFunctions == 2 && zend_hash_exists(intern->registered_phpfunctions, callable) == 0) {
		php_error_docref(NULL, E_WARNING, "Not allowed to call handler '%s()'", ZSTR_VAL(callable));
		/* Push an empty string, so that we at least have an xslt result... */
		valuePush(ctxt, xmlXPathNewString((const xmlChar *) ""));
	} else {
		result = zend_call_function(&fci, NULL);
		if (result == FAILURE) {
			if (Z_TYPE(handler) == IS_STRING) {
				php_error_docref(NULL, E_WARNING, "Unable to call handler %s()", Z_STRVAL(handler));
				valuePush(ctxt, xmlXPathNewString((const xmlChar *) ""));
			}
		/* retval is == NULL, when an exception occurred, don't report anything, because PHP itself will handle that */
		} else if (Z_ISUNDEF(retval)) {
		} else {
			if (Z_TYPE(retval) == IS_OBJECT && instanceof_function(Z_OBJCE(retval), dom_node_class_entry)) {
				xmlNode *nodep;
				dom_object *obj;
				if (intern->node_list == NULL) {
					intern->node_list = zend_new_array(0);
				}
				Z_ADDREF(retval);
				zend_hash_next_index_insert(intern->node_list, &retval);
				obj = Z_DOMOBJ_P(&retval);
				nodep = dom_object_get_node(obj);
				valuePush(ctxt, xmlXPathNewNodeSet(nodep));
			} else if (Z_TYPE(retval) == IS_TRUE || Z_TYPE(retval) == IS_FALSE) {
				valuePush(ctxt, xmlXPathNewBoolean(Z_TYPE(retval) == IS_TRUE));
			} else if (Z_TYPE(retval) == IS_OBJECT) {
				php_error_docref(NULL, E_WARNING, "A PHP Object cannot be converted to a XPath-string");
				valuePush(ctxt, xmlXPathNewString((const xmlChar *) ""));
			} else {
				convert_to_string_ex(&retval);
				valuePush(ctxt, xmlXPathNewString((xmlChar *) Z_STRVAL(retval)));
			}
			zval_ptr_dtor(&retval);
		}
	}
	zend_string_release_ex(callable, 0);
	zval_ptr_dtor(&handler);
	if (fci.param_count > 0) {
		for (i = 0; i < nargs - 1; i++) {
			zval_ptr_dtor(&args[i]);
		}
		efree(args);
	}
}
/* }}} */

void xsl_ext_function_string_php(xmlXPathParserContextPtr ctxt, int nargs) /* {{{ */
{
	xsl_ext_function_php(ctxt, nargs, 1);
}
/* }}} */

void xsl_ext_function_object_php(xmlXPathParserContextPtr ctxt, int nargs) /* {{{ */
{
	xsl_ext_function_php(ctxt, nargs, 2);
}
/* }}} */

/* {{{ proto void xsl_xsltprocessor_import_stylesheet(domdocument doc)
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#
Since:
*/
PHP_FUNCTION(xsl_xsltprocessor_import_stylesheet)
{
	zval *id, *docp = NULL;
	xmlDoc *doc = NULL, *newdoc = NULL;
	xsltStylesheetPtr sheetp, oldsheetp;
	xsl_object *intern;
	int prevSubstValue, prevExtDtdValue, clone_docu = 0;
	xmlNode *nodep = NULL;
	zval *cloneDocu, member, rv;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oo", &id, xsl_xsltprocessor_class_entry, &docp) == FAILURE) {
		RETURN_FALSE;
	}

	nodep = php_libxml_import_node(docp);

	if (nodep) {
		doc = nodep->doc;
	}
	if (doc == NULL) {
		php_error(E_WARNING, "Invalid Document");
		RETURN_FALSE;
	}

	/* libxslt uses _private, so we must copy the imported
	stylesheet document otherwise the node proxies will be a mess */
	newdoc = xmlCopyDoc(doc, 1);
	xmlNodeSetBase((xmlNodePtr) newdoc, (xmlChar *)doc->URL);
	prevSubstValue = xmlSubstituteEntitiesDefault(1);
	prevExtDtdValue = xmlLoadExtDtdDefaultValue;
	xmlLoadExtDtdDefaultValue = XML_DETECT_IDS | XML_COMPLETE_ATTRS;

	sheetp = xsltParseStylesheetDoc(newdoc);
	xmlSubstituteEntitiesDefault(prevSubstValue);
	xmlLoadExtDtdDefaultValue = prevExtDtdValue;

	if (!sheetp) {
		xmlFreeDoc(newdoc);
		RETURN_FALSE;
	}

	intern = Z_XSL_P(id);

	ZVAL_STRING(&member, "cloneDocument");
	cloneDocu = zend_std_read_property(id, &member, BP_VAR_IS, NULL, &rv);
	if (Z_TYPE_P(cloneDocu) != IS_NULL) {
		convert_to_long(cloneDocu);
		clone_docu = Z_LVAL_P(cloneDocu);
	}
	zval_ptr_dtor(&member);
	if (clone_docu == 0) {
		/* check if the stylesheet is using xsl:key, if yes, we have to clone the document _always_ before a transformation */
		nodep = xmlDocGetRootElement(sheetp->doc);
		if (nodep && (nodep = nodep->children)) {
			while (nodep) {
				if (nodep->type == XML_ELEMENT_NODE && xmlStrEqual(nodep->name, (const xmlChar *) "key") && xmlStrEqual(nodep->ns->href, XSLT_NAMESPACE)) {
					intern->hasKeys = 1;
					break;
				}
				nodep = nodep->next;
			}
		}
	} else {
		intern->hasKeys = clone_docu;
	}

	if ((oldsheetp = (xsltStylesheetPtr)intern->ptr)) {
		/* free wrapper */
		if (((xsltStylesheetPtr) intern->ptr)->_private != NULL) {
			((xsltStylesheetPtr) intern->ptr)->_private = NULL;
		}
		xsltFreeStylesheet((xsltStylesheetPtr) intern->ptr);
		intern->ptr = NULL;
	}

	php_xsl_set_object(id, sheetp);
	RETVAL_TRUE;
}
/* }}} end xsl_xsltprocessor_import_stylesheet */

static xmlDocPtr php_xsl_apply_stylesheet(zval *id, xsl_object *intern, xsltStylesheetPtr style, zval *docp) /* {{{ */
{
	xmlDocPtr newdocp = NULL;
	xmlDocPtr doc = NULL;
	xmlNodePtr node = NULL;
	xsltTransformContextPtr ctxt;
	php_libxml_node_object *object;
	char **params = NULL;
	int clone;
	zval *doXInclude, member, rv;
	FILE *f;
	int secPrefsError = 0;
	int secPrefsValue;
	xsltSecurityPrefsPtr secPrefs = NULL;

	node = php_libxml_import_node(docp);

	if (node) {
		doc = node->doc;
	}
	if (doc == NULL) {
		php_error_docref(NULL, E_WARNING, "Invalid Document");
		return NULL;
	}

	if (style == NULL) {
		php_error_docref(NULL, E_WARNING, "No stylesheet associated to this object");
		return NULL;
	}

	if (intern->profiling) {
		if (php_check_open_basedir(intern->profiling)) {
			f = NULL;
		} else {
			f = VCWD_FOPEN(intern->profiling, "w");
		}
	} else {
		f = NULL;
	}

	if (intern->parameter) {
		params = php_xsl_xslt_make_params(intern->parameter, 0);
	}

	intern->doc = emalloc(sizeof(php_libxml_node_object));
	memset(intern->doc, 0, sizeof(php_libxml_node_object));

	if (intern->hasKeys == 1) {
		doc = xmlCopyDoc(doc, 1);
	} else {
		object = Z_LIBXML_NODE_P(docp);
		intern->doc->document = object->document;
	}

	php_libxml_increment_doc_ref(intern->doc, doc);

	ctxt = xsltNewTransformContext(style, doc);
	ctxt->_private = (void *) intern;

	ZVAL_STRING(&member, "doXInclude");
	doXInclude = zend_std_read_property(id, &member, BP_VAR_IS, NULL, &rv);
	if (Z_TYPE_P(doXInclude) != IS_NULL) {
		convert_to_long(doXInclude);
		ctxt->xinclude = Z_LVAL_P(doXInclude);
	}
	zval_ptr_dtor(&member);

	secPrefsValue = intern->securityPrefs;

	/* if securityPrefs is set to NONE, we don't have to do any checks, but otherwise... */
	if (secPrefsValue != XSL_SECPREF_NONE) {
		secPrefs = xsltNewSecurityPrefs();
		if (secPrefsValue & XSL_SECPREF_READ_FILE ) {
			if (0 != xsltSetSecurityPrefs(secPrefs, XSLT_SECPREF_READ_FILE, xsltSecurityForbid)) {
				secPrefsError = 1;
			}
		}
		if (secPrefsValue & XSL_SECPREF_WRITE_FILE ) {
			if (0 != xsltSetSecurityPrefs(secPrefs, XSLT_SECPREF_WRITE_FILE, xsltSecurityForbid)) {
				secPrefsError = 1;
			}
		}
		if (secPrefsValue & XSL_SECPREF_CREATE_DIRECTORY ) {
			if (0 != xsltSetSecurityPrefs(secPrefs, XSLT_SECPREF_CREATE_DIRECTORY, xsltSecurityForbid)) {
				secPrefsError = 1;
			}
		}
		if (secPrefsValue & XSL_SECPREF_READ_NETWORK) {
			if (0 != xsltSetSecurityPrefs(secPrefs, XSLT_SECPREF_READ_NETWORK, xsltSecurityForbid)) {
				secPrefsError = 1;
			}
		}
		if (secPrefsValue & XSL_SECPREF_WRITE_NETWORK) {
			if (0 != xsltSetSecurityPrefs(secPrefs, XSLT_SECPREF_WRITE_NETWORK, xsltSecurityForbid)) {
				secPrefsError = 1;
			}
		}

		if (0 != xsltSetCtxtSecurityPrefs(secPrefs, ctxt)) {
			secPrefsError = 1;
		}
	}

	if (secPrefsError == 1) {
		php_error_docref(NULL, E_WARNING, "Can't set libxslt security properties, not doing transformation for security reasons");
	} else {
		newdocp = xsltApplyStylesheetUser(style, doc, (const char**) params,  NULL, f, ctxt);
	}
	if (f) {
		fclose(f);
	}

	xsltFreeTransformContext(ctxt);
	if (secPrefs) {
		xsltFreeSecurityPrefs(secPrefs);
	}

	if (intern->node_list != NULL) {
		zend_hash_destroy(intern->node_list);
		FREE_HASHTABLE(intern->node_list);
		intern->node_list = NULL;
	}

	php_libxml_decrement_doc_ref(intern->doc);
	efree(intern->doc);
	intern->doc = NULL;

	if (params) {
		clone = 0;
		while(params[clone]) {
			efree(params[clone++]);
		}
		efree(params);
	}

	return newdocp;

}
/* }}} */

/* {{{ proto domdocument xsl_xsltprocessor_transform_to_doc(domnode doc)
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#
Since:
*/
PHP_FUNCTION(xsl_xsltprocessor_transform_to_doc)
{
	zval *id, *docp = NULL;
	xmlDoc *newdocp;
	xsltStylesheetPtr sheetp;
	zend_string *ret_class = NULL;
	xsl_object *intern;

	id = getThis();
	intern = Z_XSL_P(id);
	sheetp = (xsltStylesheetPtr) intern->ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "o|S!", &docp, &ret_class) == FAILURE) {
		RETURN_FALSE;
	}

	newdocp = php_xsl_apply_stylesheet(id, intern, sheetp, docp);

	if (newdocp) {
		if (ret_class) {
			zend_string *curclass_name;
			zend_class_entry *curce, *ce;
			php_libxml_node_object *interndoc;

			curce = Z_OBJCE_P(docp);
			curclass_name = curce->name;
			while (curce->parent != NULL) {
				curce = curce->parent;
			}

			ce = zend_lookup_class(ret_class);
			if (ce == NULL || !instanceof_function(ce, curce)) {
				xmlFreeDoc(newdocp);
				php_error_docref(NULL, E_WARNING,
					"Expecting class compatible with %s, '%s' given", ZSTR_VAL(curclass_name), ZSTR_VAL(ret_class));
				RETURN_FALSE;
			}

			object_init_ex(return_value, ce);

			interndoc = Z_LIBXML_NODE_P(return_value);
			php_libxml_increment_doc_ref(interndoc, newdocp);
			php_libxml_increment_node_ptr(interndoc, (xmlNodePtr)newdocp, (void *)interndoc);
		} else {
			php_dom_create_object((xmlNodePtr) newdocp, return_value, NULL);
		}
	} else {
		RETURN_FALSE;
	}

}
/* }}} end xsl_xsltprocessor_transform_to_doc */

/* {{{ proto int xsl_xsltprocessor_transform_to_uri(domdocument doc, string uri)
*/
PHP_FUNCTION(xsl_xsltprocessor_transform_to_uri)
{
	zval *id, *docp = NULL;
	xmlDoc *newdocp;
	xsltStylesheetPtr sheetp;
	int ret;
	size_t uri_len;
	char *uri;
	xsl_object *intern;

	id = getThis();
	intern = Z_XSL_P(id);
	sheetp = (xsltStylesheetPtr) intern->ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "op", &docp, &uri, &uri_len) == FAILURE) {
		RETURN_FALSE;
	}

	newdocp = php_xsl_apply_stylesheet(id, intern, sheetp, docp);

	ret = -1;
	if (newdocp) {
		ret = xsltSaveResultToFilename(uri, newdocp, sheetp, 0);
		xmlFreeDoc(newdocp);
	}

	RETVAL_LONG(ret);
}
/* }}} end xsl_xsltprocessor_transform_to_uri */

/* {{{ proto string xsl_xsltprocessor_transform_to_xml(domdocument doc)
*/
PHP_FUNCTION(xsl_xsltprocessor_transform_to_xml)
{
	zval *id, *docp = NULL;
	xmlDoc *newdocp;
	xsltStylesheetPtr sheetp;
	int ret;
	xmlChar *doc_txt_ptr;
	int doc_txt_len;
	xsl_object *intern;

	id = getThis();
	intern = Z_XSL_P(id);
	sheetp = (xsltStylesheetPtr) intern->ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &docp) == FAILURE) {
		RETURN_FALSE;
	}

	newdocp = php_xsl_apply_stylesheet(id, intern, sheetp, docp);

	ret = -1;
	if (newdocp) {
		ret = xsltSaveResultToString(&doc_txt_ptr, &doc_txt_len, newdocp, sheetp);
		if (doc_txt_ptr && doc_txt_len) {
			RETVAL_STRINGL((char *) doc_txt_ptr, doc_txt_len);
			xmlFree(doc_txt_ptr);
		}
		xmlFreeDoc(newdocp);
	}

	if (ret < 0) {
		RETURN_FALSE;
	}
}
/* }}} end xsl_xsltprocessor_transform_to_xml */

/* {{{ proto bool xsl_xsltprocessor_set_parameter(string namespace, mixed name [, string value])
*/
PHP_FUNCTION(xsl_xsltprocessor_set_parameter)
{

	zval *id;
	zval *array_value, *entry, new_string;
	xsl_object *intern;
	char *namespace;
	size_t namespace_len;
	zend_string *string_key, *name, *value;
	DOM_GET_THIS(id);

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "sa", &namespace, &namespace_len, &array_value) == SUCCESS) {
		intern = Z_XSL_P(id);
		ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(array_value), string_key, entry) {
			if (string_key == NULL) {
				php_error_docref(NULL, E_WARNING, "Invalid parameter array");
				RETURN_FALSE;
			}
			convert_to_string_ex(entry);
			Z_TRY_ADDREF_P(entry);
			zend_hash_update(intern->parameter, string_key, entry);
		} ZEND_HASH_FOREACH_END();
		RETURN_TRUE;
	} else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "sSS", &namespace, &namespace_len, &name, &value) == SUCCESS) {

		intern = Z_XSL_P(id);

		ZVAL_STR_COPY(&new_string, value);

		zend_hash_update(intern->parameter, name, &new_string);
		RETURN_TRUE;
	} else {
		WRONG_PARAM_COUNT;
	}

}
/* }}} end xsl_xsltprocessor_set_parameter */

/* {{{ proto string xsl_xsltprocessor_get_parameter(string namespace, string name)
*/
PHP_FUNCTION(xsl_xsltprocessor_get_parameter)
{
	zval *id;
	char *namespace;
	size_t namespace_len = 0;
	zval *value;
	zend_string *name;
	xsl_object *intern;

	DOM_GET_THIS(id);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sS", &namespace, &namespace_len, &name) == FAILURE) {
		RETURN_FALSE;
	}
	intern = Z_XSL_P(id);
	if ((value = zend_hash_find(intern->parameter, name)) != NULL) {
		RETURN_STR(zval_get_string(value));
	} else {
		RETURN_FALSE;
	}
}
/* }}} end xsl_xsltprocessor_get_parameter */

/* {{{ proto bool xsl_xsltprocessor_remove_parameter(string namespace, string name)
*/
PHP_FUNCTION(xsl_xsltprocessor_remove_parameter)
{
	zval *id;
	size_t namespace_len = 0;
	char *namespace;
	zend_string *name;
	xsl_object *intern;

	DOM_GET_THIS(id);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sS", &namespace, &namespace_len, &name) == FAILURE) {
		RETURN_FALSE;
	}
	intern = Z_XSL_P(id);
	if (zend_hash_del(intern->parameter, name) == SUCCESS) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} end xsl_xsltprocessor_remove_parameter */

/* {{{ proto void xsl_xsltprocessor_register_php_functions([mixed $restrict])
*/
PHP_FUNCTION(xsl_xsltprocessor_register_php_functions)
{
	zval *id;
	xsl_object *intern;
	zval *array_value, *entry, new_string;
	zend_string *name;

	DOM_GET_THIS(id);

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "a",  &array_value) == SUCCESS) {
		intern = Z_XSL_P(id);

		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(array_value), entry) {
			convert_to_string_ex(entry);
			ZVAL_LONG(&new_string ,1);
			zend_hash_update(intern->registered_phpfunctions, Z_STR_P(entry), &new_string);
		} ZEND_HASH_FOREACH_END();

		intern->registerPhpFunctions = 2;
	} else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "S",  &name) == SUCCESS) {
		intern = Z_XSL_P(id);

		ZVAL_LONG(&new_string,1);
		zend_hash_update(intern->registered_phpfunctions, name, &new_string);
		intern->registerPhpFunctions = 2;

	} else {
		intern = Z_XSL_P(id);
		intern->registerPhpFunctions = 1;
	}

}
/* }}} end xsl_xsltprocessor_register_php_functions(); */

/* {{{ proto bool xsl_xsltprocessor_set_profiling(string filename) */
PHP_FUNCTION(xsl_xsltprocessor_set_profiling)
{
	zval *id;
	xsl_object *intern;
	char *filename = NULL;
	size_t filename_len;
	DOM_GET_THIS(id);

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "p!", &filename, &filename_len) == SUCCESS) {
		intern = Z_XSL_P(id);
		if (intern->profiling) {
			efree(intern->profiling);
		}
		if (filename != NULL) {
			intern->profiling = estrndup(filename, filename_len);
		} else {
			intern->profiling = NULL;
		}
		RETURN_TRUE;
	} else {
		WRONG_PARAM_COUNT;
	}
}
/* }}} end xsl_xsltprocessor_set_profiling */

/* {{{ proto int xsl_xsltprocessor_set_security_prefs(int securityPrefs) */
PHP_FUNCTION(xsl_xsltprocessor_set_security_prefs)
{
	zval *id;
	xsl_object *intern;
	zend_long securityPrefs, oldSecurityPrefs;

	DOM_GET_THIS(id);
 	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &securityPrefs) == FAILURE) {
		return;
	}
	intern = Z_XSL_P(id);
	oldSecurityPrefs = intern->securityPrefs;
	intern->securityPrefs = securityPrefs;
	/* set this to 1 so that we know, it was set through this method. Can be removed, when we remove the ini setting */
	intern->securityPrefsSet = 1;
	RETURN_LONG(oldSecurityPrefs);
}
/* }}} end xsl_xsltprocessor_set_security_prefs */

/* {{{ proto int xsl_xsltprocessor_get_security_prefs() */
PHP_FUNCTION(xsl_xsltprocessor_get_security_prefs)
{
	zval *id;
	xsl_object *intern;

	DOM_GET_THIS(id);
	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "") == SUCCESS) {
		intern = Z_XSL_P(id);
		RETURN_LONG(intern->securityPrefs);
	} else {
		WRONG_PARAM_COUNT;
	}
}
/* }}} end xsl_xsltprocessor_get_security_prefs */

/* {{{ proto bool xsl_xsltprocessor_has_exslt_support()
*/
PHP_FUNCTION(xsl_xsltprocessor_has_exslt_support)
{
#if HAVE_XSL_EXSLT
	RETURN_TRUE;
#else
	RETURN_FALSE;
#endif
}
/* }}} end xsl_xsltprocessor_has_exslt_support(); */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
