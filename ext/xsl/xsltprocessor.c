/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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
				if (!try_convert_to_string(value)) {
					efree(params);
					return NULL;
				}
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
	zval *args = NULL;
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
								node = xmlDocCopyNode(node, domintern->document->ptr, 1);
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

/* {{{ proto void XSLTProcessor::importStylesheet(domdocument doc)
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#
Since:
*/
PHP_METHOD(XSLTProcessor, importStylesheet)
{
	zval *id, *docp = NULL;
	xmlDoc *doc = NULL, *newdoc = NULL;
	xsltStylesheetPtr sheetp, oldsheetp;
	xsl_object *intern;
	int prevSubstValue, prevExtDtdValue, clone_docu = 0;
	xmlNode *nodep = NULL;
	zval *cloneDocu, rv;
	zend_string *member;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &docp) == FAILURE) {
		RETURN_THROWS();
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

	member = zend_string_init("cloneDocument", sizeof("cloneDocument")-1, 0);
	cloneDocu = zend_std_read_property(Z_OBJ_P(id), member, BP_VAR_IS, NULL, &rv);
	if (Z_TYPE_P(cloneDocu) != IS_NULL) {
		convert_to_long(cloneDocu);
		clone_docu = Z_LVAL_P(cloneDocu);
	}
	zend_string_release_ex(member, 0);
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
/* }}} end XSLTProcessor::importStylesheet */

static xmlDocPtr php_xsl_apply_stylesheet(zval *id, xsl_object *intern, xsltStylesheetPtr style, zval *docp) /* {{{ */
{
	xmlDocPtr newdocp = NULL;
	xmlDocPtr doc = NULL;
	xmlNodePtr node = NULL;
	xsltTransformContextPtr ctxt;
	php_libxml_node_object *object;
	char **params = NULL;
	int clone;
	zval *doXInclude, rv;
	zend_string *member;
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

	member = zend_string_init("doXInclude", sizeof("doXInclude")-1, 0);
	doXInclude = zend_std_read_property(Z_OBJ_P(id), member, BP_VAR_IS, NULL, &rv);
	if (Z_TYPE_P(doXInclude) != IS_NULL) {
		convert_to_long(doXInclude);
		ctxt->xinclude = Z_LVAL_P(doXInclude);
	}
	zend_string_release_ex(member, 0);

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

/* {{{ proto domdocument XSLTProcessor::transformToDoc(domnode doc)
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#
Since:
*/
PHP_METHOD(XSLTProcessor, transformToDoc)
{
	zval *id, *docp = NULL;
	xmlDoc *newdocp;
	xsltStylesheetPtr sheetp;
	zend_string *ret_class = NULL;
	xsl_object *intern;

	id = ZEND_THIS;
	intern = Z_XSL_P(id);
	sheetp = (xsltStylesheetPtr) intern->ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "o|S!", &docp, &ret_class) == FAILURE) {
		RETURN_THROWS();
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
/* }}} end XSLTProcessor::transformToDoc */

/* {{{ proto int XSLTProcessor::transformToUri(domdocument doc, string uri)
*/
PHP_METHOD(XSLTProcessor, transformToUri)
{
	zval *id, *docp = NULL;
	xmlDoc *newdocp;
	xsltStylesheetPtr sheetp;
	int ret;
	size_t uri_len;
	char *uri;
	xsl_object *intern;

	id = ZEND_THIS;
	intern = Z_XSL_P(id);
	sheetp = (xsltStylesheetPtr) intern->ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "op", &docp, &uri, &uri_len) == FAILURE) {
		RETURN_THROWS();
	}

	newdocp = php_xsl_apply_stylesheet(id, intern, sheetp, docp);

	ret = -1;
	if (newdocp) {
		ret = xsltSaveResultToFilename(uri, newdocp, sheetp, 0);
		xmlFreeDoc(newdocp);
	}

	RETVAL_LONG(ret);
}
/* }}} end XSLTProcessor::transformToUri */

/* {{{ proto string XSLTProcessor::transformToXml(domdocument doc)
*/
PHP_METHOD(XSLTProcessor, transformToXml)
{
	zval *id, *docp = NULL;
	xmlDoc *newdocp;
	xsltStylesheetPtr sheetp;
	int ret;
	xmlChar *doc_txt_ptr;
	int doc_txt_len;
	xsl_object *intern;

	id = ZEND_THIS;
	intern = Z_XSL_P(id);
	sheetp = (xsltStylesheetPtr) intern->ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &docp) == FAILURE) {
		RETURN_THROWS();
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
/* }}} end XSLTProcessor::transformToXml */

/* {{{ proto bool XSLTProcessor::setParameter(string namespace, mixed name [, string value])
*/
PHP_METHOD(XSLTProcessor, setParameter)
{

	zval *id = ZEND_THIS;
	zval *array_value, *entry, new_string;
	xsl_object *intern;
	char *namespace;
	size_t namespace_len;
	zend_string *string_key, *name, *value;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "sa", &namespace, &namespace_len, &array_value) == SUCCESS) {
		intern = Z_XSL_P(id);
		ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(array_value), string_key, entry) {
			zval tmp;
			zend_string *str;

			if (string_key == NULL) {
				php_error_docref(NULL, E_WARNING, "Invalid parameter array");
				RETURN_FALSE;
			}
			str = zval_try_get_string(entry);
			if (UNEXPECTED(!str)) {
				RETURN_THROWS();
			}
			ZVAL_STR(&tmp, str);
			zend_hash_update(intern->parameter, string_key, &tmp);
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
/* }}} end XSLTProcessor::setParameter */

/* {{{ proto string XSLTProcessor::getParameter(string namespace, string name)
*/
PHP_METHOD(XSLTProcessor, getParameter)
{
	zval *id = ZEND_THIS;
	char *namespace;
	size_t namespace_len = 0;
	zval *value;
	zend_string *name;
	xsl_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sS", &namespace, &namespace_len, &name) == FAILURE) {
		RETURN_THROWS();
	}
	intern = Z_XSL_P(id);
	if ((value = zend_hash_find(intern->parameter, name)) != NULL) {
		RETURN_STR(zval_get_string(value));
	} else {
		RETURN_FALSE;
	}
}
/* }}} end XSLTProcessor::getParameter */

/* {{{ proto bool XSLTProcessor::removeParameter(string namespace, string name)
*/
PHP_METHOD(XSLTProcessor, removeParameter)
{
	zval *id = ZEND_THIS;
	size_t namespace_len = 0;
	char *namespace;
	zend_string *name;
	xsl_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sS", &namespace, &namespace_len, &name) == FAILURE) {
		RETURN_THROWS();
	}
	intern = Z_XSL_P(id);
	if (zend_hash_del(intern->parameter, name) == SUCCESS) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} end XSLTProcessor::removeParameter */

/* {{{ proto void XSLTProcessor::registerPHPFunctions([mixed $restrict])
*/
PHP_METHOD(XSLTProcessor, registerPHPFunctions)
{
	zval *id = ZEND_THIS;
	xsl_object *intern;
	zval *entry, new_string;
	zend_string *restrict_str = NULL;
	HashTable *restrict_ht = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_ARRAY_HT_OR_NULL(restrict_str, restrict_ht)
	ZEND_PARSE_PARAMETERS_END();

	intern = Z_XSL_P(id);

	if (restrict_ht) {
		ZEND_HASH_FOREACH_VAL(restrict_ht, entry) {
			zend_string *str = zval_try_get_string(entry);
			if (UNEXPECTED(!str)) {
				return;
			}
			ZVAL_LONG(&new_string, 1);
			zend_hash_update(intern->registered_phpfunctions, str, &new_string);
			zend_string_release(str);
		} ZEND_HASH_FOREACH_END();

		intern->registerPhpFunctions = 2;
	} else if (restrict_str) {
		ZVAL_LONG(&new_string, 1);
		zend_hash_update(intern->registered_phpfunctions, restrict_str, &new_string);
		intern->registerPhpFunctions = 2;
	} else {
		intern->registerPhpFunctions = 1;
	}
}
/* }}} end XSLTProcessor::registerPHPFunctions(); */

/* {{{ proto bool XSLTProcessor::setProfiling(string filename) */
PHP_METHOD(XSLTProcessor, setProfiling)
{
	zval *id = ZEND_THIS;
	xsl_object *intern;
	char *filename = NULL;
	size_t filename_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "p!", &filename, &filename_len) == FAILURE) {
		RETURN_THROWS();
	}

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
}
/* }}} end XSLTProcessor::setProfiling */

/* {{{ proto int XSLTProcessor::setSecurityPrefs(int securityPrefs) */
PHP_METHOD(XSLTProcessor, setSecurityPrefs)
{
	zval *id = ZEND_THIS;
	xsl_object *intern;
	zend_long securityPrefs, oldSecurityPrefs;

 	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &securityPrefs) == FAILURE) {
		RETURN_THROWS();
	}
	intern = Z_XSL_P(id);
	oldSecurityPrefs = intern->securityPrefs;
	intern->securityPrefs = securityPrefs;
	/* set this to 1 so that we know, it was set through this method. Can be removed, when we remove the ini setting */
	intern->securityPrefsSet = 1;
	RETURN_LONG(oldSecurityPrefs);
}
/* }}} end XSLTProcessor::setSecurityPrefs */

/* {{{ proto int XSLTProcessor::getSecurityPrefs() */
PHP_METHOD(XSLTProcessor, getSecurityPrefs)
{
	zval *id = ZEND_THIS;
	xsl_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	intern = Z_XSL_P(id);

	RETURN_LONG(intern->securityPrefs);
}
/* }}} end XSLTProcessor::getSecurityPrefs */

/* {{{ proto bool XSLTProcessor::hasExsltSupport()
*/
PHP_METHOD(XSLTProcessor, hasExsltSupport)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

#if HAVE_XSL_EXSLT
	RETURN_TRUE;
#else
	RETURN_FALSE;
#endif
}
/* }}} end XSLTProcessor::hasExsltSupport(); */
