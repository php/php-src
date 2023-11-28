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
#include "php_xsl.h"
#include <libxslt/variables.h>
#include "ext/libxml/php_libxml.h"


static zend_result php_xsl_xslt_apply_params(xsltTransformContextPtr ctxt, HashTable *params)
{
	zend_string *string_key;
	zval *value;

	ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(params, string_key, value) {
		ZEND_ASSERT(string_key != NULL);
		/* Already a string because of setParameter() */
		ZEND_ASSERT(Z_TYPE_P(value) == IS_STRING);

		int result = xsltQuoteOneUserParam(ctxt, (const xmlChar *) ZSTR_VAL(string_key), (const xmlChar *) Z_STRVAL_P(value));
		if (result < 0) {
			php_error_docref(NULL, E_WARNING, "Could not apply parameter \"%s\"", ZSTR_VAL(string_key));
			return FAILURE;
		}
	} ZEND_HASH_FOREACH_END();

	return SUCCESS;
}

static void xsl_proxy_factory(xmlNodePtr node, zval *child, dom_object *intern, xmlXPathParserContextPtr ctxt)
{
	ZEND_ASSERT(node->type != XML_NAMESPACE_DECL);

	/**
	 * Upon freeing libxslt's context, every document that is not the *main* document will be freed by libxslt.
	 * If a node of a document that is *not the main* document gets returned to userland, we'd free the node twice:
	 * first by the cleanup of the xslt context, and then by our own refcounting mechanism.
	 * To prevent this, we'll take a copy if the node is not from the main document.
	 * It is important that we do not copy the node unconditionally, because that means that:
	 *  - modifications to the node will only modify the copy, and not the original
	 *  - accesses to the parent, path, ... will not work
	 */
	xsltTransformContextPtr transform_ctxt = (xsltTransformContextPtr) ctxt->context->extra;
	if (node->doc != transform_ctxt->document->doc) {
		node = xmlDocCopyNode(node, intern->document->ptr, 1);
	}
	php_dom_create_object(node, child, intern);
}

static void xsl_ext_function_php(xmlXPathParserContextPtr ctxt, int nargs, php_dom_xpath_nodeset_evaluation_mode evaluation_mode) /* {{{ */
{
	bool error = false;
	xsl_object *intern;

	if (! zend_is_executing()) {
		xsltGenericError(xsltGenericErrorContext,
		"xsltExtFunctionTest: Function called from outside of PHP\n");
		error = true;
	} else {
		xsltTransformContextPtr tctxt = xsltXPathGetTransformContext(ctxt);
		if (tctxt == NULL) {
			xsltGenericError(xsltGenericErrorContext,
			"xsltExtFunctionTest: failed to get the transformation context\n");
			error = true;
		} else {
			intern = (xsl_object*)tctxt->_private;
			if (intern == NULL) {
				xsltGenericError(xsltGenericErrorContext,
				"xsltExtFunctionTest: failed to get the internal object\n");
				error = true;
			}
		}
	}

	if (error) {
		php_dom_xpath_callbacks_clean_argument_stack(ctxt, nargs);
	} else {
		php_dom_xpath_callbacks_call(&intern->xpath_callbacks, ctxt, nargs, evaluation_mode, (dom_object *) intern->doc, xsl_proxy_factory);
	}
}
/* }}} */

void xsl_ext_function_string_php(xmlXPathParserContextPtr ctxt, int nargs) /* {{{ */
{
	xsl_ext_function_php(ctxt, nargs, PHP_DOM_XPATH_EVALUATE_NODESET_TO_STRING);
}
/* }}} */

void xsl_ext_function_object_php(xmlXPathParserContextPtr ctxt, int nargs) /* {{{ */
{
	xsl_ext_function_php(ctxt, nargs, PHP_DOM_XPATH_EVALUATE_NODESET_TO_NODESET);
}
/* }}} */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#
Since:
*/
PHP_METHOD(XSLTProcessor, importStylesheet)
{
	zval *id, *docp = NULL;
	xmlDoc *doc = NULL, *newdoc = NULL;
	xsltStylesheetPtr sheetp, oldsheetp;
	xsl_object *intern;
	int clone_docu = 0;
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
		zend_argument_value_error(1, "must be a valid XML node");
		RETURN_THROWS();
	}

	/* libxslt uses _private, so we must copy the imported
	stylesheet document otherwise the node proxies will be a mess */
	newdoc = xmlCopyDoc(doc, 1);
	xmlNodeSetBase((xmlNodePtr) newdoc, (xmlChar *)doc->URL);
	PHP_LIBXML_SANITIZE_GLOBALS(parse);
	ZEND_DIAGNOSTIC_IGNORED_START("-Wdeprecated-declarations")
	xmlSubstituteEntitiesDefault(1);
	xmlLoadExtDtdDefaultValue = XML_DETECT_IDS | XML_COMPLETE_ATTRS;
	ZEND_DIAGNOSTIC_IGNORED_END

	sheetp = xsltParseStylesheetDoc(newdoc);
	PHP_LIBXML_RESTORE_GLOBALS(parse);

	if (!sheetp) {
		xmlFreeDoc(newdoc);
		RETURN_FALSE;
	}

	intern = Z_XSL_P(id);

	member = ZSTR_INIT_LITERAL("cloneDocument", 0);
	cloneDocu = zend_std_read_property(Z_OBJ_P(id), member, BP_VAR_R, NULL, &rv);
	clone_docu = zend_is_true(cloneDocu);
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
		zend_argument_value_error(1, "must be a valid XML node");
		return NULL;
	}

	if (style == NULL) {
		zend_string *name = get_active_function_or_method_name();
		zend_throw_error(NULL, "%s() can only be called after a stylesheet has been imported",
			ZSTR_VAL(name));
		zend_string_release(name);
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

	if (intern->parameter) {
		zend_result status = php_xsl_xslt_apply_params(ctxt, intern->parameter);
		if (UNEXPECTED(status != SUCCESS) && EG(exception)) {
			goto out;
		}
	}

	member = ZSTR_INIT_LITERAL("doXInclude", 0);
	doXInclude = zend_std_read_property(Z_OBJ_P(id), member, BP_VAR_R, NULL, &rv);
	ctxt->xinclude = zend_is_true(doXInclude);
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
		newdocp = xsltApplyStylesheetUser(style, doc, /* params (handled manually) */ NULL, /* output */ NULL, f, ctxt);
	}

out:
	if (f) {
		fclose(f);
	}

	xsltFreeTransformContext(ctxt);
	if (secPrefs) {
		xsltFreeSecurityPrefs(secPrefs);
	}

	php_dom_xpath_callbacks_clean_node_list(&intern->xpath_callbacks);

	php_libxml_decrement_doc_ref(intern->doc);
	efree(intern->doc);
	intern->doc = NULL;

	return newdocp;

}
/* }}} */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#
Since:
*/
PHP_METHOD(XSLTProcessor, transformToDoc)
{
	zval *id, *docp = NULL;
	xmlDoc *newdocp;
	xsltStylesheetPtr sheetp;
	zend_class_entry *ret_class = NULL;
	xsl_object *intern;

	id = ZEND_THIS;
	intern = Z_XSL_P(id);
	sheetp = (xsltStylesheetPtr) intern->ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "o|C!", &docp, &ret_class) == FAILURE) {
		RETURN_THROWS();
	}

	newdocp = php_xsl_apply_stylesheet(id, intern, sheetp, docp);

	if (newdocp) {
		if (ret_class) {
			zend_string *curclass_name;
			zend_class_entry *curce;
			php_libxml_node_object *interndoc;

			curce = Z_OBJCE_P(docp);
			curclass_name = curce->name;
			while (curce->parent != NULL) {
				curce = curce->parent;
			}

			if (!instanceof_function(ret_class, curce)) {
				xmlFreeDoc(newdocp);
				zend_argument_type_error(2, "must be a class name compatible with %s, %s given",
					ZSTR_VAL(curclass_name), ZSTR_VAL(ret_class->name)
				);
				RETURN_THROWS();
			}

			object_init_ex(return_value, ret_class);

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

/* {{{ */
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

/* {{{ */
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

/* {{{ */
PHP_METHOD(XSLTProcessor, setParameter)
{

	zval *id = ZEND_THIS;
	zval *entry, new_string;
	HashTable *array_value;
	xsl_object *intern;
	char *namespace;
	size_t namespace_len;
	zend_string *string_key, *name, *value = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STRING(namespace, namespace_len)
		Z_PARAM_ARRAY_HT_OR_STR(array_value, name)
		Z_PARAM_OPTIONAL
		Z_PARAM_PATH_STR_OR_NULL(value)
	ZEND_PARSE_PARAMETERS_END();

	intern = Z_XSL_P(id);

	if (array_value) {
		if (value) {
			zend_argument_value_error(3, "must be null when argument #2 ($name) is an array");
			RETURN_THROWS();
		}

		ZEND_HASH_FOREACH_STR_KEY_VAL(array_value, string_key, entry) {
			zval tmp;
			zend_string *str;

			if (string_key == NULL) {
				zend_argument_type_error(2, "must contain only string keys");
				RETURN_THROWS();
			}

			if (UNEXPECTED(CHECK_NULL_PATH(ZSTR_VAL(string_key), ZSTR_LEN(string_key)))) {
				zend_argument_value_error(3, "must not contain keys with any null bytes");
				RETURN_THROWS();
			}

			str = zval_try_get_string(entry);
			if (UNEXPECTED(!str)) {
				RETURN_THROWS();
			}

			if (UNEXPECTED(CHECK_NULL_PATH(ZSTR_VAL(str), ZSTR_LEN(str)))) {
				zend_string_release(str);
				zend_argument_value_error(3, "must not contain values with any null bytes");
				RETURN_THROWS();
			}

			ZVAL_STR(&tmp, str);
			zend_hash_update(intern->parameter, string_key, &tmp);
		} ZEND_HASH_FOREACH_END();
		RETURN_TRUE;
	} else {
		if (!value) {
			zend_argument_value_error(3, "cannot be null when argument #2 ($name) is a string");
			RETURN_THROWS();
		}

		if (UNEXPECTED(CHECK_NULL_PATH(ZSTR_VAL(name), ZSTR_LEN(name)))) {
			zend_argument_value_error(2, "must not contain any null bytes");
			RETURN_THROWS();
		}

		ZVAL_STR_COPY(&new_string, value);

		zend_hash_update(intern->parameter, name, &new_string);
		RETURN_TRUE;
	}
}
/* }}} end XSLTProcessor::setParameter */

/* {{{ */
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
		RETURN_STR_COPY(Z_STR_P(value));
	} else {
		RETURN_FALSE;
	}
}
/* }}} end XSLTProcessor::getParameter */

/* {{{ */
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

/* {{{ */
PHP_METHOD(XSLTProcessor, registerPHPFunctions)
{
	xsl_object *intern = Z_XSL_P(ZEND_THIS);

	zend_string *name = NULL;
	HashTable *callable_ht = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_HT_OR_STR_OR_NULL(callable_ht, name)
	ZEND_PARSE_PARAMETERS_END();

	php_dom_xpath_callbacks_update_method_handler(&intern->xpath_callbacks, NULL, name, callable_ht);
}
/* }}} end XSLTProcessor::registerPHPFunctions(); */

/* {{{ */
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

/* {{{ */
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

/* {{{ */
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

/* {{{ */
PHP_METHOD(XSLTProcessor, hasExsltSupport)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

#ifdef HAVE_XSL_EXSLT
	RETURN_TRUE;
#else
	RETURN_FALSE;
#endif
}
/* }}} end XSLTProcessor::hasExsltSupport(); */
