/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
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
* URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#
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
	{NULL, NULL, NULL}
};

/* {{{ php_xsl_xslt_string_to_xpathexpr()
   Translates a string to a XPath Expression */
static char *php_xsl_xslt_string_to_xpathexpr(const char *str TSRMLS_DC)
{
	const xmlChar *string = (const xmlChar *)str;

	xmlChar *value;
	int str_len;

	str_len = xmlStrlen(string) + 3;

	if (xmlStrchr(string, '"')) {
		if (xmlStrchr(string, '\'')) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot create XPath expression (string contains both quote and double-quotes)");
			return NULL;
		}
		value = (xmlChar*) safe_emalloc (str_len, sizeof(xmlChar), 0);
		snprintf(value, str_len, "'%s'", string);
	} else {
		value = (xmlChar*) safe_emalloc (str_len, sizeof(xmlChar), 0);
		snprintf(value, str_len, "\"%s\"", string);
	}
	return (char *) value;
}
/* }}} */

/* {{{ php_xsl_xslt_make_params()
   Translates a PHP array to a libxslt parameters array */
static char **php_xsl_xslt_make_params(HashTable *parht, int xpath_params TSRMLS_DC)
{

	int parsize;
	zval **value;
	char *xpath_expr, *string_key = NULL;
	ulong num_key;
	char **params = NULL;
	int i = 0;

	parsize = (2 * zend_hash_num_elements(parht) + 1) * sizeof(char *);
	params = (char **)safe_emalloc((2 * zend_hash_num_elements(parht) + 1), sizeof(char *), 0);
	memset((char *)params, 0, parsize);

	for (zend_hash_internal_pointer_reset(parht);
		zend_hash_get_current_data(parht, (void **)&value) == SUCCESS;
		zend_hash_move_forward(parht)) {

		if (zend_hash_get_current_key(parht, &string_key, &num_key, 1) != HASH_KEY_IS_STRING) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid argument or parameter array");
			efree(params);
			return NULL;
		} else {
			if (Z_TYPE_PP(value) != IS_STRING) {
				SEPARATE_ZVAL(value);
				convert_to_string(*value);
			}

			if (!xpath_params) {
				xpath_expr = php_xsl_xslt_string_to_xpathexpr(Z_STRVAL_PP(value) TSRMLS_CC);
			} else {
				xpath_expr = estrndup(Z_STRVAL_PP(value), Z_STRLEN_PP(value));
			}
			if (xpath_expr) {
				params[i++] = string_key;
				params[i++] = xpath_expr;
			} else {
				efree(string_key);
			}
		}
	}

	params[i++] = NULL;

	return params;
}
/* }}} */

static void xsl_ext_function_php(xmlXPathParserContextPtr ctxt, int nargs, int type) /* {{{ */
{
	xsltTransformContextPtr tctxt;
	zval **args;
	zval *retval;
	int result, i, ret;
	int error = 0;
	zend_fcall_info fci;
	zval handler;
	xmlXPathObjectPtr obj;
	char *str;
	char *callable = NULL;
	xsl_object *intern;

	TSRMLS_FETCH();

	if (! zend_is_executing(TSRMLS_C)) {
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
			intern = (xsl_object *) tctxt->_private;
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
		fci.params = safe_emalloc(fci.param_count, sizeof(zval**), 0);
		args = safe_emalloc(fci.param_count, sizeof(zval *), 0);
	}
	/* Reverse order to pop values off ctxt stack */
	for (i = nargs - 2; i >= 0; i--) {
		obj = valuePop(ctxt);
		MAKE_STD_ZVAL(args[i]);
		if (obj == NULL) {
			ZVAL_NULL(args[i]);
			continue;
		}
		switch (obj->type) {
			case XPATH_STRING:
				ZVAL_STRING(args[i],  obj->stringval, 1);
				break;
			case XPATH_BOOLEAN:
				ZVAL_BOOL(args[i],  obj->boolval);
				break;
			case XPATH_NUMBER:
				ZVAL_DOUBLE(args[i], obj->floatval);
				break;
			case XPATH_NODESET:
				if (type == 1) {
					str = xmlXPathCastToString(obj);
					ZVAL_STRING(args[i], str, 1);
					xmlFree(str);
				} else if (type == 2) {
					int j;
					dom_object *domintern = (dom_object *)intern->doc;
					array_init(args[i]);
					if (obj->nodesetval && obj->nodesetval->nodeNr > 0) {
						for (j = 0; j < obj->nodesetval->nodeNr; j++) {
							xmlNodePtr node = obj->nodesetval->nodeTab[j];
							zval *child;
							MAKE_STD_ZVAL(child);
							/* not sure, if we need this... it's copied from xpath.c */
							if (node->type == XML_NAMESPACE_DECL) {
								xmlNsPtr curns;
								xmlNodePtr nsparent;

								nsparent = node->_private;
								curns = xmlNewNs(NULL, node->name, NULL);
								if (node->children) {
									curns->prefix = xmlStrdup((char *) node->children);
								}
								if (node->children) {
									node = xmlNewDocNode(node->doc, NULL, (char *) node->children, node->name);
								} else {
									node = xmlNewDocNode(node->doc, NULL, "xmlns", node->name);
								}
								node->type = XML_NAMESPACE_DECL;
								node->parent = nsparent;
								node->ns = curns;
							} else {
								node = xmlDocCopyNodeList(domintern->document->ptr, node);
							}

							child = php_dom_create_object(node, &ret, child, domintern TSRMLS_CC);
							add_next_index_zval(args[i], child);
						}
					}
				}
				break;
			default:
				str = xmlXPathCastToString(obj);
				ZVAL_STRING(args[i], str, 1);
				xmlFree(str);
		}
		xmlXPathFreeObject(obj);
		fci.params[i] = &args[i];
	}

	fci.size = sizeof(fci);
	fci.function_table = EG(function_table);

	obj = valuePop(ctxt);
	if (obj == NULL || obj->stringval == NULL) {
		if (obj) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Handler name must be a string");
			xmlXPathFreeObject(obj);
		}
		valuePush(ctxt, xmlXPathNewString(""));
		if (fci.param_count > 0) {
			for (i = 0; i < nargs - 1; i++) {
				zval_ptr_dtor(&args[i]);
			}
			efree(args);
			efree(fci.params);
		}
		return;
	}
	INIT_PZVAL(&handler);
	ZVAL_STRING(&handler, obj->stringval, 1);
	xmlXPathFreeObject(obj);

	fci.function_name = &handler;
	fci.symbol_table = NULL;
	fci.object_ptr = NULL;
	fci.retval_ptr_ptr = &retval;
	fci.no_separation = 0;
	/*fci.function_handler_cache = &function_ptr;*/
	if (!zend_make_callable(&handler, &callable TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to call handler %s()", callable);
		valuePush(ctxt, xmlXPathNewString(""));
	} else if ( intern->registerPhpFunctions == 2 && zend_hash_exists(intern->registered_phpfunctions, callable, strlen(callable) + 1) == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Not allowed to call handler '%s()'", callable);
		/* Push an empty string, so that we at least have an xslt result... */
		valuePush(ctxt, xmlXPathNewString(""));
	} else {
		result = zend_call_function(&fci, NULL TSRMLS_CC);
		if (result == FAILURE) {
			if (Z_TYPE(handler) == IS_STRING) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to call handler %s()", Z_STRVAL_P(&handler));
				valuePush(ctxt, xmlXPathNewString(""));
			}
		/* retval is == NULL, when an exception occurred, don't report anything, because PHP itself will handle that */
		} else if (retval == NULL) {
		} else {
			if (retval->type == IS_OBJECT && instanceof_function( Z_OBJCE_P(retval), dom_node_class_entry TSRMLS_CC)) {
				xmlNode *nodep;
				dom_object *obj;
				if (intern->node_list == NULL) {
					ALLOC_HASHTABLE(intern->node_list);
					zend_hash_init(intern->node_list, 0, NULL, ZVAL_PTR_DTOR, 0);
				}
				zval_add_ref(&retval);
				zend_hash_next_index_insert(intern->node_list, &retval, sizeof(zval *), NULL);
				obj = (dom_object *)zend_object_store_get_object(retval TSRMLS_CC);
				nodep = dom_object_get_node(obj);
				valuePush(ctxt, xmlXPathNewNodeSet(nodep));
			} else if (retval->type == IS_BOOL) {
				valuePush(ctxt, xmlXPathNewBoolean(retval->value.lval));
			} else if (retval->type == IS_OBJECT) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "A PHP Object cannot be converted to a XPath-string");
				valuePush(ctxt, xmlXPathNewString(""));
			} else {
				convert_to_string_ex(&retval);
				valuePush(ctxt, xmlXPathNewString( Z_STRVAL_P(retval)));
			}
			zval_ptr_dtor(&retval);
		}
	}
	efree(callable);
	zval_dtor(&handler);
	if (fci.param_count > 0) {
		for (i = 0; i < nargs - 1; i++) {
			zval_ptr_dtor(&args[i]);
		}
		efree(args);
		efree(fci.params);
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

/* {{{ proto void xsl_xsltprocessor_import_stylesheet(domdocument doc);
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
	zend_object_handlers *std_hnd;
	zval *cloneDocu, *member;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oo", &id, xsl_xsltprocessor_class_entry, &docp) == FAILURE) {
		RETURN_FALSE;
	}

	nodep = php_libxml_import_node(docp TSRMLS_CC);

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

	intern = (xsl_object *)zend_object_store_get_object(id TSRMLS_CC);

	std_hnd = zend_get_std_object_handlers();
	MAKE_STD_ZVAL(member);
	ZVAL_STRING(member, "cloneDocument", 0);
	cloneDocu = std_hnd->read_property(id, member, BP_VAR_IS, NULL TSRMLS_CC);
	if (Z_TYPE_P(cloneDocu) != IS_NULL) {
		convert_to_long(cloneDocu);
		clone_docu = Z_LVAL_P(cloneDocu);
	}
	efree(member);
	if (clone_docu == 0) {
		/* check if the stylesheet is using xsl:key, if yes, we have to clone the document _always_ before a transformation */
		nodep = xmlDocGetRootElement(sheetp->doc);
		if (nodep && (nodep = nodep->children)) {
			while (nodep) {
				if (nodep->type == XML_ELEMENT_NODE && xmlStrEqual(nodep->name, "key") && xmlStrEqual(nodep->ns->href, XSLT_NAMESPACE)) {
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

	php_xsl_set_object(id, sheetp TSRMLS_CC);
	RETVAL_TRUE;
}
/* }}} end xsl_xsltprocessor_import_stylesheet */

static xmlDocPtr php_xsl_apply_stylesheet(zval *id, xsl_object *intern, xsltStylesheetPtr style, zval *docp TSRMLS_DC) /* {{{ */
{
	xmlDocPtr newdocp = NULL;
	xmlDocPtr doc = NULL;
	xmlNodePtr node = NULL;
	xsltTransformContextPtr ctxt;
	php_libxml_node_object *object;
	char **params = NULL;
	int clone;
	zval *doXInclude, *member;
	zend_object_handlers *std_hnd;
	FILE *f;
	int secPrefsError = 0;
	int secPrefsValue, secPrefsIni;
	xsltSecurityPrefsPtr secPrefs = NULL;

	node = php_libxml_import_node(docp TSRMLS_CC);

	if (node) {
		doc = node->doc;
	}
	if (doc == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Document");
		return NULL;
	}

	if (style == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No stylesheet associated to this object");
		return NULL;
	}

	if (intern->profiling) {
		if (php_check_open_basedir(intern->profiling TSRMLS_CC)) {
			f = NULL;
		} else {
			f = VCWD_FOPEN(intern->profiling, "w");
		}
	} else {
		f = NULL;
	}

	if (intern->parameter) {
		params = php_xsl_xslt_make_params(intern->parameter, 0 TSRMLS_CC);
	}

	intern->doc = emalloc(sizeof(php_libxml_node_object));
	memset(intern->doc, 0, sizeof(php_libxml_node_object));

	if (intern->hasKeys == 1) {
		doc = xmlCopyDoc(doc, 1);
	} else {
		object = (php_libxml_node_object *)zend_object_store_get_object(docp TSRMLS_CC);
		intern->doc->document = object->document;
	}

	php_libxml_increment_doc_ref(intern->doc, doc TSRMLS_CC);

	ctxt = xsltNewTransformContext(style, doc);
	ctxt->_private = (void *) intern;

	std_hnd = zend_get_std_object_handlers();

	MAKE_STD_ZVAL(member);
	ZVAL_STRING(member, "doXInclude", 0);
	doXInclude = std_hnd->read_property(id, member, BP_VAR_IS, NULL TSRMLS_CC);
	if (Z_TYPE_P(doXInclude) != IS_NULL) {
		convert_to_long(doXInclude);
		ctxt->xinclude = Z_LVAL_P(doXInclude);
	}
	efree(member);

	secPrefsValue = intern->securityPrefs;

	/* This whole if block can be removed, when we remove the xsl.security_prefs php.ini option in PHP 6+ */
	secPrefsIni= INI_INT("xsl.security_prefs");
	/* if secPrefsIni has the same value as secPrefsValue, all is fine */
	if (secPrefsIni != secPrefsValue) {
		if (secPrefsIni != XSL_SECPREF_DEFAULT) {
			/* if the ini value is not set to the default, throw an E_DEPRECATED warning */
			php_error_docref(NULL TSRMLS_CC, E_DEPRECATED, "The xsl.security_prefs php.ini option is deprecated; use XsltProcessor->setSecurityPrefs() instead");
			if (intern->securityPrefsSet == 0) {
				/* if securityPrefs were not set through the setSecurityPrefs method, take the ini setting */
				secPrefsValue = secPrefsIni;
			} else {
				/* else throw a notice, that the ini setting was not used */
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "The xsl.security_prefs php.ini was not used, since the  XsltProcessor->setSecurityPrefs() method was used");
			}
		}
	}

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
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't set libxslt security properties, not doing transformation for security reasons");
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

	php_libxml_decrement_doc_ref(intern->doc TSRMLS_CC);
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

/* {{{ proto domdocument xsl_xsltprocessor_transform_to_doc(domnode doc);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#
Since:
*/
PHP_FUNCTION(xsl_xsltprocessor_transform_to_doc)
{
	zval *id, *docp = NULL;
	xmlDoc *newdocp;
	xsltStylesheetPtr sheetp;
	int ret, ret_class_len=0;
	char *ret_class = NULL;
	xsl_object *intern;

	id = getThis();
	intern = (xsl_object *)zend_object_store_get_object(id TSRMLS_CC);
	sheetp = (xsltStylesheetPtr) intern->ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o|s!", &docp, &ret_class, &ret_class_len) == FAILURE) {
		RETURN_FALSE;
	}

	newdocp = php_xsl_apply_stylesheet(id, intern, sheetp, docp TSRMLS_CC);

	if (newdocp) {
		if (ret_class) {
			int found;
			char *curclass_name;
			zend_class_entry *curce, **ce;
			php_libxml_node_object *interndoc;

			curce = Z_OBJCE_P(docp);
			curclass_name = curce->name;
			while (curce->parent != NULL) {
				curce = curce->parent;
			}

			found = zend_lookup_class(ret_class, ret_class_len, &ce TSRMLS_CC);
			if ((found != SUCCESS) || !instanceof_function(*ce, curce TSRMLS_CC)) {
				xmlFreeDoc(newdocp);
				php_error_docref(NULL TSRMLS_CC, E_WARNING,
					"Expecting class compatible with %s, '%s' given", curclass_name, ret_class);
				RETURN_FALSE;
			}

			object_init_ex(return_value, *ce);

			interndoc = (php_libxml_node_object *)zend_objects_get_address(return_value TSRMLS_CC);
			php_libxml_increment_doc_ref(interndoc, newdocp TSRMLS_CC);
			php_libxml_increment_node_ptr(interndoc, (xmlNodePtr)newdocp, (void *)interndoc TSRMLS_CC);
		} else {
			DOM_RET_OBJ((xmlNodePtr) newdocp, &ret, NULL);
		}
	} else {
		RETURN_FALSE;
	}

}
/* }}} end xsl_xsltprocessor_transform_to_doc */

/* {{{ proto int xsl_xsltprocessor_transform_to_uri(domdocument doc, string uri);
*/
PHP_FUNCTION(xsl_xsltprocessor_transform_to_uri)
{
	zval *id, *docp = NULL;
	xmlDoc *newdocp;
	xsltStylesheetPtr sheetp;
	int ret, uri_len;
	char *uri;
	xsl_object *intern;

	id = getThis();
	intern = (xsl_object *)zend_object_store_get_object(id TSRMLS_CC);
	sheetp = (xsltStylesheetPtr) intern->ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "op", &docp, &uri, &uri_len) == FAILURE) {
		RETURN_FALSE;
	}

	newdocp = php_xsl_apply_stylesheet(id, intern, sheetp, docp TSRMLS_CC);

	ret = -1;
	if (newdocp) {
		ret = xsltSaveResultToFilename(uri, newdocp, sheetp, 0);
		xmlFreeDoc(newdocp);
	}

	RETVAL_LONG(ret);
}
/* }}} end xsl_xsltprocessor_transform_to_uri */

/* {{{ proto string xsl_xsltprocessor_transform_to_xml(domdocument doc);
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
	intern = (xsl_object *)zend_object_store_get_object(id TSRMLS_CC);
	sheetp = (xsltStylesheetPtr) intern->ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &docp) == FAILURE) {
		RETURN_FALSE;
	}

	newdocp = php_xsl_apply_stylesheet(id, intern, sheetp, docp TSRMLS_CC);

	ret = -1;
	if (newdocp) {
		ret = xsltSaveResultToString(&doc_txt_ptr, &doc_txt_len, newdocp, sheetp);
		if (doc_txt_ptr && doc_txt_len) {
			RETVAL_STRINGL(doc_txt_ptr, doc_txt_len, 1);
			xmlFree(doc_txt_ptr);
		}
		xmlFreeDoc(newdocp);
	}

	if (ret < 0) {
		RETURN_FALSE;
	}
}
/* }}} end xsl_xsltprocessor_transform_to_xml */

/* {{{ proto bool xsl_xsltprocessor_set_parameter(string namespace, mixed name [, string value]);
*/
PHP_FUNCTION(xsl_xsltprocessor_set_parameter)
{

	zval *id;
	zval *array_value, **entry, *new_string;
	xsl_object *intern;
	char *string_key, *name, *value, *namespace;
	ulong idx;
	int string_key_len, namespace_len, name_len, value_len;
	DOM_GET_THIS(id);

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "sa", &namespace, &namespace_len, &array_value) == SUCCESS) {
		intern = (xsl_object *)zend_object_store_get_object(id TSRMLS_CC);
		zend_hash_internal_pointer_reset(Z_ARRVAL_P(array_value));

		while (zend_hash_get_current_data(Z_ARRVAL_P(array_value), (void **)&entry) == SUCCESS) {
			SEPARATE_ZVAL(entry);
			convert_to_string_ex(entry);

			if (zend_hash_get_current_key_ex(Z_ARRVAL_P(array_value), &string_key, &string_key_len, &idx, 0, NULL) != HASH_KEY_IS_STRING) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid parameter array");
				RETURN_FALSE;
			}

			ALLOC_ZVAL(new_string);
			Z_ADDREF_PP(entry);
			COPY_PZVAL_TO_ZVAL(*new_string, *entry);

			zend_hash_update(intern->parameter, string_key, string_key_len, &new_string, sizeof(zval*), NULL);
			zend_hash_move_forward(Z_ARRVAL_P(array_value));
		}
		RETURN_TRUE;

	} else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "sss", &namespace, &namespace_len, &name, &name_len, &value, &value_len) == SUCCESS) {

		intern = (xsl_object *)zend_object_store_get_object(id TSRMLS_CC);

		MAKE_STD_ZVAL(new_string);
		ZVAL_STRING(new_string, value, 1);

		zend_hash_update(intern->parameter, name, name_len + 1, &new_string, sizeof(zval*), NULL);
		RETURN_TRUE;
	} else {
		WRONG_PARAM_COUNT;
	}

}
/* }}} end xsl_xsltprocessor_set_parameter */

/* {{{ proto string xsl_xsltprocessor_get_parameter(string namespace, string name);
*/
PHP_FUNCTION(xsl_xsltprocessor_get_parameter)
{
	zval *id;
	int name_len = 0, namespace_len = 0;
	char *name, *namespace;
	zval **value;
	xsl_object *intern;

	DOM_GET_THIS(id);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &namespace, &namespace_len, &name, &name_len) == FAILURE) {
		RETURN_FALSE;
	}
	intern = (xsl_object *)zend_object_store_get_object(id TSRMLS_CC);
	if ( zend_hash_find(intern->parameter, name, name_len + 1,  (void**) &value) == SUCCESS) {
		convert_to_string_ex(value);
		RETVAL_STRING(Z_STRVAL_PP(value),1);
	} else {
		RETURN_FALSE;
	}
}
/* }}} end xsl_xsltprocessor_get_parameter */

/* {{{ proto bool xsl_xsltprocessor_remove_parameter(string namespace, string name);
*/
PHP_FUNCTION(xsl_xsltprocessor_remove_parameter)
{
	zval *id;
	int name_len = 0, namespace_len = 0;
	char *name, *namespace;
	xsl_object *intern;

	DOM_GET_THIS(id);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &namespace, &namespace_len, &name, &name_len) == FAILURE) {
		RETURN_FALSE;
	}
	intern = (xsl_object *)zend_object_store_get_object(id TSRMLS_CC);
	if ( zend_hash_del(intern->parameter, name, name_len + 1) == SUCCESS) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} end xsl_xsltprocessor_remove_parameter */

/* {{{ proto void xsl_xsltprocessor_register_php_functions([mixed $restrict]);
*/
PHP_FUNCTION(xsl_xsltprocessor_register_php_functions)
{
	zval *id;
	xsl_object *intern;
	zval *array_value, **entry, *new_string;
	int  name_len = 0;
	char *name;

	DOM_GET_THIS(id);

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "a",  &array_value) == SUCCESS) {
		intern = (xsl_object *)zend_object_store_get_object(id TSRMLS_CC);
		zend_hash_internal_pointer_reset(Z_ARRVAL_P(array_value));

		while (zend_hash_get_current_data(Z_ARRVAL_P(array_value), (void **)&entry) == SUCCESS) {
			SEPARATE_ZVAL(entry);
			convert_to_string_ex(entry);

			MAKE_STD_ZVAL(new_string);
			ZVAL_LONG(new_string,1);

			zend_hash_update(intern->registered_phpfunctions, Z_STRVAL_PP(entry), Z_STRLEN_PP(entry) + 1, &new_string, sizeof(zval*), NULL);
			zend_hash_move_forward(Z_ARRVAL_P(array_value));
		}
		intern->registerPhpFunctions = 2;

	} else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s",  &name, &name_len) == SUCCESS) {
		intern = (xsl_object *)zend_object_store_get_object(id TSRMLS_CC);

		MAKE_STD_ZVAL(new_string);
		ZVAL_LONG(new_string,1);
		zend_hash_update(intern->registered_phpfunctions, name, name_len + 1, &new_string, sizeof(zval*), NULL);
		intern->registerPhpFunctions = 2;

	} else {
		intern = (xsl_object *)zend_object_store_get_object(id TSRMLS_CC);
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
	int filename_len;
	DOM_GET_THIS(id);

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "p!", &filename, &filename_len) == SUCCESS) {
		intern = (xsl_object *)zend_object_store_get_object(id TSRMLS_CC);
		if (intern->profiling) {
			efree(intern->profiling);
		}
		if (filename != NULL) {
			intern->profiling = estrndup(filename,filename_len);
		} else {
			intern->profiling = NULL;
		}
		RETURN_TRUE;
	} else {
		WRONG_PARAM_COUNT;
	}
}
/* }}} end xsl_xsltprocessor_set_profiling */

/* {{{ proto long xsl_xsltprocessor_set_security_prefs(long securityPrefs) */
PHP_FUNCTION(xsl_xsltprocessor_set_security_prefs)
{
	zval *id;
	xsl_object *intern;
	long securityPrefs, oldSecurityPrefs;

	DOM_GET_THIS(id);
 	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &securityPrefs) == FAILURE) {
		return;
	}
	intern = (xsl_object *)zend_object_store_get_object(id TSRMLS_CC);
	oldSecurityPrefs = intern->securityPrefs;
	intern->securityPrefs = securityPrefs;
	/* set this to 1 so that we know, it was set through this method. Can be removed, when we remove the ini setting */
	intern->securityPrefsSet = 1;
	RETURN_LONG(oldSecurityPrefs);
}
/* }}} end xsl_xsltprocessor_set_security_prefs */

/* {{{ proto long xsl_xsltprocessor_get_security_prefs() */
PHP_FUNCTION(xsl_xsltprocessor_get_security_prefs)
{
	zval *id;
	xsl_object *intern;

	DOM_GET_THIS(id);
	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "") == SUCCESS) {
		intern = (xsl_object *)zend_object_store_get_object(id TSRMLS_CC);
		RETURN_LONG(intern->securityPrefs);
	} else {
		WRONG_PARAM_COUNT;
	}
}
/* }}} end xsl_xsltprocessor_get_security_prefs */



/* {{{ proto bool xsl_xsltprocessor_has_exslt_support();
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
