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
  | Author: Rob Richards <rrichards@php.net>                             |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_xmlwriter.h"

zend_class_entry *xmlwriter_class_entry;

static zend_function_entry xmlwriter_functions[] = {
	PHP_FE(xmlwriter_open_uri,			NULL)
	PHP_FE(xmlwriter_open_memory,		NULL)
#if LIBXML_VERSION >= 20605
	PHP_FE(xmlwriter_set_indent,		NULL)
	PHP_FE(xmlwriter_set_indent_string, NULL)
#endif
	PHP_FE(xmlwriter_start_attribute,	NULL)
	PHP_FE(xmlwriter_end_attribute,		NULL)
	PHP_FE(xmlwriter_start_attribute_ns,	NULL)
	PHP_FE(xmlwriter_write_attribute,	NULL)
	PHP_FE(xmlwriter_write_attribute_ns,	NULL)
	PHP_FE(xmlwriter_start_element,		NULL)
	PHP_FE(xmlwriter_end_element,		NULL)
	PHP_FE(xmlwriter_start_element_ns,	NULL)
	PHP_FE(xmlwriter_write_element,		NULL)
	PHP_FE(xmlwriter_write_element_ns,	NULL)
	PHP_FE(xmlwriter_start_pi,			NULL)
	PHP_FE(xmlwriter_end_pi,			NULL)
	PHP_FE(xmlwriter_write_pi,			NULL)
	PHP_FE(xmlwriter_start_cdata,		NULL)
	PHP_FE(xmlwriter_end_cdata,			NULL)
	PHP_FE(xmlwriter_write_cdata,		NULL)
	PHP_FE(xmlwriter_text,				NULL)
	PHP_FE(xmlwriter_start_document,	NULL)
	PHP_FE(xmlwriter_end_document,		NULL)
	PHP_FE(xmlwriter_write_comment,		NULL)
	PHP_FE(xmlwriter_start_dtd,			NULL)
	PHP_FE(xmlwriter_end_dtd,			NULL)
	PHP_FE(xmlwriter_write_dtd,			NULL)
	PHP_FE(xmlwriter_start_dtd_element,	NULL)
	PHP_FE(xmlwriter_end_dtd_element,	NULL)
	PHP_FE(xmlwriter_output_memory,		NULL)
	{NULL, NULL, NULL}
};

/* {{{ function prototypes */
PHP_MINIT_FUNCTION(xmlwriter);
PHP_MSHUTDOWN_FUNCTION(xmlwriter);
PHP_MINFO_FUNCTION(xmlwriter);

static int le_xmlwriter;

/* _xmlwriter_get_valid_file_path should be made a 
	common function in libxml extension as code is common to a few xml extensions */
char *_xmlwriter_get_valid_file_path(char *source, char *resolved_path, int resolved_path_len  TSRMLS_DC) {
	xmlURI *uri;
	xmlChar *escsource;
	char *file_dest;
	int isFileUri = 0;

	uri = xmlCreateURI();
	escsource = xmlURIEscapeStr(source, ":");
	xmlParseURIReference(uri, escsource);
	xmlFree(escsource);

	if (uri->scheme != NULL) {
		/* absolute file uris - libxml only supports localhost or empty host */
		if (strncasecmp(source, "file:///",8) == 0) {
			isFileUri = 1;
#ifdef PHP_WIN32
			source += 8;
#else
			source += 7;
#endif
		} else if (strncasecmp(source, "file://localhost/",17) == 0) {
			isFileUri = 1;
#ifdef PHP_WIN32
			source += 17;
#else
			source += 16;
#endif
		}
	}

	file_dest = source;

	if ((uri->scheme == NULL || isFileUri)) {
		/* XXX possible buffer overflow if VCWD_REALPATH does not know size of resolved_path */
		if (! VCWD_REALPATH(source, resolved_path)) {
			expand_filepath(source, resolved_path TSRMLS_CC);
		}
		file_dest = resolved_path;
	}

	xmlFreeURI(uri);

	return file_dest;
}

/* {{{ xmlwriter_module_entry
 */
zend_module_entry xmlwriter_module_entry = {
	STANDARD_MODULE_HEADER,
	"xmlwriter",
	xmlwriter_functions,
	PHP_MINIT(xmlwriter),
	PHP_MSHUTDOWN(xmlwriter),
	NULL,
	NULL,
	PHP_MINFO(xmlwriter),
	"0.1",
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_XMLWRITER
ZEND_GET_MODULE(xmlwriter)
#endif

/* {{{ xmlwriter_objects_clone */
void xmlwriter_objects_clone(void *object, void **object_clone TSRMLS_DC)
{
	/* TODO */
}
/* }}} */

static void xmlwriter_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
	xmlwriter_object *intern;

	intern = (xmlwriter_object *) rsrc->ptr;
	if (intern) {
		if (intern->ptr) {
			xmlFreeTextWriter(intern->ptr);
			intern->ptr = NULL;
		}
		if (intern->output) {
			xmlBufferFree(intern->output);
			intern->output = NULL;
		}
		efree(intern);
	}
}

#if LIBXML_VERSION >= 20605
/* {{{ proto bool xmlwriter_set_indent(resource xmlwriter, bool)
Toggle indentation on/off - returns FALSE on error */
PHP_FUNCTION(xmlwriter_set_indent)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	int indent, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rb", &pind, &indent) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterSetIndent(ptr, indent);
		if (retval == 0) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_set_indent_string(resource xmlwriter, string indentString)
Set string used for indenting - returns FALSE on error */
PHP_FUNCTION(xmlwriter_set_indent_string)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *indent;
	int indent_len, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &pind, &indent, &indent_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterSetIndentString(ptr, indent);
		if (retval == 0) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}
#endif

/* {{{ proto bool xmlwriter_start_attribute(resource xmlwriter, string name)
Create start attribute - returns FALSE on error */
PHP_FUNCTION(xmlwriter_start_attribute)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name;
	int name_len, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &pind, &name, &name_len) == FAILURE) {
		return;
	}

	retval = xmlValidateName((xmlChar *) name, 0);
	if (retval != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Attribute Name");
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterStartAttribute(ptr, name);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_end_attribute(resource xmlwriter)
End attribute - returns FALSE on error */
PHP_FUNCTION(xmlwriter_end_attribute)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	int retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &pind) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterEndAttribute(ptr);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_start_attribute_ns(resource xmlwriter, string prefix, string name, string uri)
Create start namespaced attribute - returns FALSE on error */
PHP_FUNCTION(xmlwriter_start_attribute_ns)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name, *prefix, *uri;
	int name_len, prefix_len, uri_len, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsss", &pind, 
		&prefix, &prefix_len, &name, &name_len, &uri, &uri_len) == FAILURE) {
		return;
	}

	retval = xmlValidateName((xmlChar *) name, 0);
	if (retval != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Attribute Name");
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterStartAttributeNS(ptr, prefix, name, uri);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_write_attribute(resource xmlwriter, string name, string content)
Write full attribute - returns FALSE on error */
PHP_FUNCTION(xmlwriter_write_attribute)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name, *content;
	int name_len, content_len, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &pind, 
		&name, &name_len, &content, &content_len) == FAILURE) {
		return;
	}

	retval = xmlValidateName((xmlChar *) name, 0);
	if (retval != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Attribute Name");
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterWriteAttribute(ptr, name, content);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_write_attribute_ns(resource xmlwriter, string prefix, string name, string uri, string content)
Write full namespaced attribute - returns FALSE on error */
PHP_FUNCTION(xmlwriter_write_attribute_ns)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name, *prefix, *uri, *content;
	int name_len, prefix_len, uri_len, content_len, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rssss", &pind, 
		&prefix, &prefix_len, &name, &name_len, &uri, &uri_len, &content, &content_len) == FAILURE) {
		return;
	}

	retval = xmlValidateName((xmlChar *) name, 0);
	if (retval != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Attribute Name");
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterWriteAttributeNS(ptr, prefix, name, uri, content);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_start_element(resource xmlwriter, string name)
Create start element tag - returns FALSE on error */
PHP_FUNCTION(xmlwriter_start_element)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name;
	int name_len, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &pind, &name, &name_len) == FAILURE) {
		return;
	}

	retval = xmlValidateName((xmlChar *) name, 0);
	if (retval != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Element Name");
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterStartElement(ptr, name);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_start_element_ns(resource xmlwriter, string prefix, string name, string uri)
Create start namespaced element tag - returns FALSE on error */
PHP_FUNCTION(xmlwriter_start_element_ns)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name, *prefix, *uri;
	int name_len, prefix_len, uri_len, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsss", &pind, 
		&prefix, &prefix_len, &name, &name_len, &uri, &uri_len) == FAILURE) {
		return;
	}

	retval = xmlValidateName((xmlChar *) name, 0);
	if (retval != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Element Name");
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterStartElementNS(ptr, prefix, name, uri);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_end_element(resource xmlwriter)
End current element - returns FALSE on error */
PHP_FUNCTION(xmlwriter_end_element)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	int retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &pind) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterEndElement(ptr);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_write_element(resource xmlwriter, string name, string content)
Write full element tag - returns FALSE on error */
PHP_FUNCTION(xmlwriter_write_element)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name, *content;
	int name_len, content_len, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &pind, 
		&name, &name_len, &content, &content_len) == FAILURE) {
		return;
	}

	retval = xmlValidateName((xmlChar *) name, 0);
	if (retval != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Element Name");
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterWriteElement(ptr, name, content);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_write_element_ns(resource xmlwriter, string prefix, string name, string uri, string content)
Write full namesapced element tag - returns FALSE on error */
PHP_FUNCTION(xmlwriter_write_element_ns)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name, *prefix, *uri, *content;
	int name_len, prefix_len, uri_len, content_len, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rssss", &pind, 
		&prefix, &prefix_len, &name, &name_len, &uri, &uri_len, &content, &content_len) == FAILURE) {
		return;
	}

	retval = xmlValidateName((xmlChar *) name, 0);
	if (retval != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Element Name");
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterWriteElementNS(ptr, prefix, name, uri, content);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_start_pi(resource xmlwriter, string target)
Create start PI tag - returns FALSE on error */
PHP_FUNCTION(xmlwriter_start_pi)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *target;
	int target_len, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &pind, &target, &target_len) == FAILURE) {
		return;
	}

	retval = xmlValidateName((xmlChar *) target, 0);
	if (retval != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid PI Target");
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterStartPI(ptr, target);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_end_pi(resource xmlwriter)
End current PI - returns FALSE on error */
PHP_FUNCTION(xmlwriter_end_pi)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	int retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &pind) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterEndPI(ptr);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_write_pi(resource xmlwriter, string target, string content)
Write full PI tag - returns FALSE on error */
PHP_FUNCTION(xmlwriter_write_pi)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *target, *content;
	int target_len, content_len, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &pind, 
		&target, &target_len, &content, &content_len) == FAILURE) {
		return;
	}

	retval = xmlValidateName((xmlChar *) target, 0);
	if (retval != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid PI Target");
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterWritePI(ptr, target, content);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_start_cdata(resource xmlwriter)
Create start CDATA tag - returns FALSE on error */
PHP_FUNCTION(xmlwriter_start_cdata)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	int retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &pind) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterStartCDATA(ptr);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_end_cdata(resource xmlwriter)
End current CDATA - returns FALSE on error */
PHP_FUNCTION(xmlwriter_end_cdata)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	int retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &pind) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterEndCDATA(ptr);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_write_cdata(resource xmlwriter, string content)
Write full CDATA tag - returns FALSE on error */
PHP_FUNCTION(xmlwriter_write_cdata)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *content;
	int content_len, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &pind, 
		&content, &content_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterWriteCDATA(ptr, content);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_text(resource xmlwriter, string content)
Write text - returns FALSE on error */
PHP_FUNCTION(xmlwriter_text)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *content;
	int content_len, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &pind, &content, &content_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterWriteString(ptr, content);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_write_comment(resource xmlwriter, string content)
Write full comment tag - returns FALSE on error */
PHP_FUNCTION(xmlwriter_write_comment)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *content;
	int content_len, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &pind, 
		&content, &content_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterWriteComment(ptr, content);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_start_document(resource xmlwriter, string version, string encoding, string standalone)
Create document tag - returns FALSE on error */
PHP_FUNCTION(xmlwriter_start_document)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *version, *enc, *alone;
	int version_len, enc_len, alone_len, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &pind, &version, &version_len, &enc, &enc_len, &alone, &alone_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterStartDocument(ptr, version, enc, alone);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_end_document(resource xmlwriter)
End current document - returns FALSE on error */
PHP_FUNCTION(xmlwriter_end_document)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	int retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &pind) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterEndDocument(ptr);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}


/* {{{ proto bool xmlwriter_start_dtd(resource xmlwriter, string name, string pubid, string sysid)
Create start DTD tag - returns FALSE on error */
PHP_FUNCTION(xmlwriter_start_dtd)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name, *pubid, *sysid;
	int name_len, pubid_len, sysid_len, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &pind, &name, &name_len, &pubid, &pubid_len, &sysid, &sysid_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterStartDTD(ptr, name, pubid, sysid);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_end_dtd(resource xmlwriter)
End current DTD - returns FALSE on error */
PHP_FUNCTION(xmlwriter_end_dtd)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	int retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &pind) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterEndDTD(ptr);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_write_dtd(resource xmlwriter, string name, string pubid, string sysid, string subset)
Write full DTD tag - returns FALSE on error */
PHP_FUNCTION(xmlwriter_write_dtd)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name, *pubid, *sysid, *subset;
	int name_len, pubid_len, sysid_len, subset_len, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &pind, &name, &name_len, &pubid, &pubid_len, &sysid, &sysid_len, &subset, &subset_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterWriteDTD(ptr, name, pubid, sysid, subset);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_start_dtd_element(resource xmlwriter, string name)
Create start DTD element - returns FALSE on error */
PHP_FUNCTION(xmlwriter_start_dtd_element)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name;
	int name_len, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &pind, &name, &name_len) == FAILURE) {
		return;
	}

	retval = xmlValidateName((xmlChar *) name, 0);
	if (retval != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Attribute Name");
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterStartDTDElement(ptr, name);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_end_dtd_element(resource xmlwriter)
End current DTD element - returns FALSE on error */
PHP_FUNCTION(xmlwriter_end_dtd_element)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	int retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &pind) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterEndDTDElement(ptr);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_write_dtd_element(resource xmlwriter, string name, string content)
Write full DTD element tag - returns FALSE on error */
PHP_FUNCTION(xmlwriter_write_dtd_element)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name, *content;
	int name_len, content_len, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &pind, 
		&name, &name_len, &content, &content_len) == FAILURE) {
		return;
	}

	retval = xmlValidateName((xmlChar *) name, 0);
	if (retval != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Element Name");
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterWriteDTDElement(ptr, name, content);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_start_dtd_attlist(resource xmlwriter, string name)
Create start DTD AttList - returns FALSE on error */
PHP_FUNCTION(xmlwriter_start_dtd_attlist)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name;
	int name_len, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &pind, &name, &name_len) == FAILURE) {
		return;
	}

	retval = xmlValidateName((xmlChar *) name, 0);
	if (retval != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Attribute Name");
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterStartDTDAttlist(ptr, name);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_end_dtd_attlist(resource xmlwriter)
End current DTD AttList - returns FALSE on error */
PHP_FUNCTION(xmlwriter_end_dtd_attlist)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	int retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &pind) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterEndDTDAttlist(ptr);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_write_dtd_attlist(resource xmlwriter, string name, string content)
Write full DTD AttList tag - returns FALSE on error */
PHP_FUNCTION(xmlwriter_write_dtd_attlist)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name, *content;
	int name_len, content_len, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &pind, 
		&name, &name_len, &content, &content_len) == FAILURE) {
		return;
	}

	retval = xmlValidateName((xmlChar *) name, 0);
	if (retval != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Element Name");
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterWriteDTDAttlist(ptr, name, content);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_start_dtd_entity(resource xmlwriter, string name, bool isparam)
Create start DTD Entity - returns FALSE on error */
PHP_FUNCTION(xmlwriter_start_dtd_entity)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name;
	int name_len, isparm, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &pind, &name, &name_len, &isparm) == FAILURE) {
		return;
	}

	retval = xmlValidateName((xmlChar *) name, 0);
	if (retval != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Attribute Name");
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterStartDTDEntity(ptr, isparm, name);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_end_dtd_entity(resource xmlwriter)
End current DTD Entity - returns FALSE on error */
PHP_FUNCTION(xmlwriter_end_dtd_entity)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	int retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &pind) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterEndDTDEntity(ptr);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto bool xmlwriter_write_dtd_entity(resource xmlwriter, string name, string content)
Write full DTD Entity tag - returns FALSE on error */
PHP_FUNCTION(xmlwriter_write_dtd_entity)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name, *content;
	int name_len, content_len, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &pind, 
		&name, &name_len, &content, &content_len) == FAILURE) {
		return;
	}

	retval = xmlValidateName((xmlChar *) name, 0);
	if (retval != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Element Name");
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterWriteDTDAttlist(ptr, name, content);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

/* {{{ proto resource xmlwriter_open_uri(resource xmlwriter, string source)
Create new xmlwriter using source uri for output */
PHP_FUNCTION(xmlwriter_open_uri)
{
	char *valid_file = NULL;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *source;
	char resolved_path[MAXPATHLEN + 1];
	int source_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &source, &source_len) == FAILURE) {
		WRONG_PARAM_COUNT;
		return;
	}

	valid_file = _xmlwriter_get_valid_file_path(source, resolved_path, MAXPATHLEN  TSRMLS_CC);
	if (!valid_file) {
		RETURN_FALSE;
	}

	ptr = xmlNewTextWriterFilename(valid_file, 0);
	if (! ptr) {
		RETURN_FALSE;
	}

	intern = emalloc(sizeof(xmlwriter_object));
	intern->ptr = ptr;
	intern->output = NULL;

	ZEND_REGISTER_RESOURCE(return_value,intern,le_xmlwriter);

}

/* {{{ proto resource xmlwriter_open_memory()
Create new xmlwriter using memory for string output */
PHP_FUNCTION(xmlwriter_open_memory)
{
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	xmlBufferPtr buffer;

	buffer = xmlBufferCreate();

	if (buffer == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to create output buffer");
		RETURN_FALSE;
	}

	ptr = xmlNewTextWriterMemory(buffer, 0);
	if (! ptr) {
		xmlBufferFree(buffer);
		RETURN_FALSE;
	}

	intern = emalloc(sizeof(xmlwriter_object));
	intern->ptr = ptr;
	intern->output = buffer;

	ZEND_REGISTER_RESOURCE(return_value,intern,le_xmlwriter);

}

/* {{{ proto string xmlwriter_output_memory(resource xmlwriter [,bool flush])
Output current buffer as string */
PHP_FUNCTION(xmlwriter_output_memory)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	xmlBufferPtr buffer;
	int flush = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|b", &pind, &flush) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	ptr = intern->ptr;
	buffer = intern->output;

	if (ptr && buffer) {
		xmlTextWriterFlush(ptr);
		RETVAL_STRING(buffer->content, 1);
		if (flush) {
			xmlBufferEmpty(buffer);
		}
		return;
	}
	
	RETURN_EMPTY_STRING()
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(xmlwriter)
{
	
	le_xmlwriter = zend_register_list_destructors_ex(xmlwriter_dtor, NULL, "xmlwriter", module_number);

	return SUCCESS;
}
/* }}} */


/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(xmlwriter)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(xmlwriter)
{
	php_info_print_table_start();
	{
		php_info_print_table_row(2, "XMLWriter", "enabled");
	}
	php_info_print_table_end();
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
