/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Shane Caraveo <shane@php.net>                               |
   |          Wez Furlong <wez@thebrainroom.com>                          |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#define IS_EXT_MODULE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "SAPI.h"

#define PHP_XML_INTERNAL
#include "zend_variables.h"
#include "ext/standard/php_string.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h"

#if HAVE_LIBXML

#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/tree.h>
#include <libxml/uri.h>
#include <libxml/xmlerror.h>
#include <libxml/xmlsave.h>
#ifdef LIBXML_SCHEMAS_ENABLED
#include <libxml/relaxng.h>
#endif

#include "php_libxml.h"

#define PHP_LIBXML_ERROR 0
#define PHP_LIBXML_CTX_ERROR 1
#define PHP_LIBXML_CTX_WARNING 2

/* a true global for initialization */
static int _php_libxml_initialized = 0;
static int _php_libxml_per_request_initialization = 1;
static xmlExternalEntityLoader _php_libxml_default_entity_loader;

typedef struct _php_libxml_func_handler {
	php_libxml_export_node export_func;
} php_libxml_func_handler;

static HashTable php_libxml_exports;

static ZEND_DECLARE_MODULE_GLOBALS(libxml)
static PHP_GINIT_FUNCTION(libxml);

static PHP_FUNCTION(libxml_set_streams_context);
static PHP_FUNCTION(libxml_use_internal_errors);
static PHP_FUNCTION(libxml_get_last_error);
static PHP_FUNCTION(libxml_clear_errors);
static PHP_FUNCTION(libxml_get_errors);
static PHP_FUNCTION(libxml_set_external_entity_loader);
static PHP_FUNCTION(libxml_disable_entity_loader);

static zend_class_entry *libxmlerror_class_entry;

/* {{{ dynamically loadable module stuff */
#ifdef COMPILE_DL_LIBXML
ZEND_GET_MODULE(libxml)
#endif /* COMPILE_DL_LIBXML */
/* }}} */

/* {{{ function prototypes */
static PHP_MINIT_FUNCTION(libxml);
static PHP_RINIT_FUNCTION(libxml);
static PHP_MSHUTDOWN_FUNCTION(libxml);
static PHP_MINFO_FUNCTION(libxml);
static int php_libxml_post_deactivate();

/* }}} */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO(arginfo_libxml_set_streams_context, 0)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_libxml_use_internal_errors, 0, 0, 0)
	ZEND_ARG_INFO(0, use_errors)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_libxml_get_last_error, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_libxml_get_errors, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_libxml_clear_errors, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_libxml_disable_entity_loader, 0, 0, 0)
	ZEND_ARG_INFO(0, disable)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_libxml_set_external_entity_loader, 0, 0, 1)
	ZEND_ARG_INFO(0, resolver_function)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ extension definition structures */
static const zend_function_entry libxml_functions[] = {
	PHP_FE(libxml_set_streams_context, arginfo_libxml_set_streams_context)
	PHP_FE(libxml_use_internal_errors, arginfo_libxml_use_internal_errors)
	PHP_FE(libxml_get_last_error, arginfo_libxml_get_last_error)
	PHP_FE(libxml_clear_errors, arginfo_libxml_clear_errors)
	PHP_FE(libxml_get_errors, arginfo_libxml_get_errors)
	PHP_FE(libxml_disable_entity_loader, arginfo_libxml_disable_entity_loader)
	PHP_FE(libxml_set_external_entity_loader, arginfo_libxml_set_external_entity_loader)
	PHP_FE_END
};

zend_module_entry libxml_module_entry = {
	STANDARD_MODULE_HEADER,
	"libxml",                /* extension name */
	libxml_functions,        /* extension function list */
	PHP_MINIT(libxml),       /* extension-wide startup function */
	PHP_MSHUTDOWN(libxml),   /* extension-wide shutdown function */
	PHP_RINIT(libxml),       /* per-request startup function */
	NULL,                    /* per-request shutdown function */
	PHP_MINFO(libxml),       /* information function */
	NO_VERSION_YET,
	PHP_MODULE_GLOBALS(libxml), /* globals descriptor */
	PHP_GINIT(libxml),          /* globals ctor */
	NULL,                       /* globals dtor */
	php_libxml_post_deactivate, /* post deactivate */
	STANDARD_MODULE_PROPERTIES_EX
};

/* }}} */

/* {{{ internal functions for interoperability */
static int php_libxml_clear_object(php_libxml_node_object *object TSRMLS_DC)
{
	if (object->properties) {
		object->properties = NULL;
	}
	php_libxml_decrement_node_ptr(object TSRMLS_CC);
	return php_libxml_decrement_doc_ref(object TSRMLS_CC);
}

static int php_libxml_unregister_node(xmlNodePtr nodep TSRMLS_DC)
{
	php_libxml_node_object *wrapper;

	php_libxml_node_ptr *nodeptr = nodep->_private;

	if (nodeptr != NULL) {
		wrapper = nodeptr->_private;
		if (wrapper) {
			php_libxml_clear_object(wrapper TSRMLS_CC);
		} else {
			if (nodeptr->node != NULL && nodeptr->node->type != XML_DOCUMENT_NODE) {
				nodeptr->node->_private = NULL;
			}
			nodeptr->node = NULL;
		}
	}

	return -1;
}

static void php_libxml_node_free(xmlNodePtr node)
{
	if(node) {
		if (node->_private != NULL) {
			((php_libxml_node_ptr *) node->_private)->node = NULL;
		}
		switch (node->type) {
			case XML_ATTRIBUTE_NODE:
				xmlFreeProp((xmlAttrPtr) node);
				break;
			case XML_ENTITY_DECL:
			case XML_ELEMENT_DECL:
			case XML_ATTRIBUTE_DECL:
				break;
			case XML_NOTATION_NODE:
				/* These require special handling */
				if (node->name != NULL) {
					xmlFree((char *) node->name);
				}
				if (((xmlEntityPtr) node)->ExternalID != NULL) {
					xmlFree((char *) ((xmlEntityPtr) node)->ExternalID);
				}
				if (((xmlEntityPtr) node)->SystemID != NULL) {
					xmlFree((char *) ((xmlEntityPtr) node)->SystemID);
				}
				xmlFree(node);
				break;
			case XML_NAMESPACE_DECL:
				if (node->ns) {
					xmlFreeNs(node->ns);
					node->ns = NULL;
				}
				node->type = XML_ELEMENT_NODE;
			default:
				xmlFreeNode(node);
		}
	}
}

static void php_libxml_node_free_list(xmlNodePtr node TSRMLS_DC)
{
	xmlNodePtr curnode;

	if (node != NULL) {
		curnode = node;
		while (curnode != NULL) {
			node = curnode;
			switch (node->type) {
				/* Skip property freeing for the following types */
				case XML_NOTATION_NODE:
				case XML_ENTITY_DECL:
					break;
				case XML_ENTITY_REF_NODE:
					php_libxml_node_free_list((xmlNodePtr) node->properties TSRMLS_CC);
					break;
				case XML_ATTRIBUTE_NODE:
						if ((node->doc != NULL) && (((xmlAttrPtr) node)->atype == XML_ATTRIBUTE_ID)) {
							xmlRemoveID(node->doc, (xmlAttrPtr) node);
						}
				case XML_ATTRIBUTE_DECL:
				case XML_DTD_NODE:
				case XML_DOCUMENT_TYPE_NODE:
				case XML_NAMESPACE_DECL:
				case XML_TEXT_NODE:
					php_libxml_node_free_list(node->children TSRMLS_CC);
					break;
				default:
					php_libxml_node_free_list(node->children TSRMLS_CC);
					php_libxml_node_free_list((xmlNodePtr) node->properties TSRMLS_CC);
			}

			curnode = node->next;
			xmlUnlinkNode(node);
			if (php_libxml_unregister_node(node TSRMLS_CC) == 0) {
				node->doc = NULL;
			}
			php_libxml_node_free(node);
		}
	}
}

/* }}} */

/* {{{ startup, shutdown and info functions */
static PHP_GINIT_FUNCTION(libxml)
{
	libxml_globals->stream_context = NULL;
	libxml_globals->error_buffer.c = NULL;
	libxml_globals->error_list = NULL;
	libxml_globals->entity_loader.fci.size = 0;
}

static void _php_libxml_destroy_fci(zend_fcall_info *fci)
{
	if (fci->size > 0) {
		zval_ptr_dtor(&fci->function_name);
		if (fci->object_ptr != NULL) {
			zval_ptr_dtor(&fci->object_ptr);
		}
		fci->size = 0;
	}
}

/* Channel libxml file io layer through the PHP streams subsystem.
 * This allows use of ftps:// and https:// urls */

static void *php_libxml_streams_IO_open_wrapper(const char *filename, const char *mode, const int read_only)
{
	php_stream_statbuf ssbuf;
	php_stream_context *context = NULL;
	php_stream_wrapper *wrapper = NULL;
	char *resolved_path, *path_to_open = NULL;
	void *ret_val = NULL;
	int isescaped=0;
	xmlURI *uri;

	TSRMLS_FETCH();

	uri = xmlParseURI(filename);
	if (uri && (uri->scheme == NULL ||
			(xmlStrncmp(BAD_CAST uri->scheme, BAD_CAST "file", 4) == 0))) {
		resolved_path = xmlURIUnescapeString(filename, 0, NULL);
		isescaped = 1;
	} else {
		resolved_path = (char *)filename;
	}

	if (uri) {
		xmlFreeURI(uri);
	}

	if (resolved_path == NULL) {
		return NULL;
	}

	/* logic copied from _php_stream_stat, but we only want to fail
	   if the wrapper supports stat, otherwise, figure it out from
	   the open.  This logic is only to support hiding warnings
	   that the streams layer puts out at times, but for libxml we
	   may try to open files that don't exist, but it is not a failure
	   in xml processing (eg. DTD files)  */
	wrapper = php_stream_locate_url_wrapper(resolved_path, &path_to_open, 0 TSRMLS_CC);
	if (wrapper && read_only && wrapper->wops->url_stat) {
		if (wrapper->wops->url_stat(wrapper, path_to_open, PHP_STREAM_URL_STAT_QUIET, &ssbuf, NULL TSRMLS_CC) == -1) {
			if (isescaped) {
				xmlFree(resolved_path);
			}
			return NULL;
		}
	}

	context = php_stream_context_from_zval(LIBXML(stream_context), 0);
	
	ret_val = php_stream_open_wrapper_ex(path_to_open, (char *)mode, REPORT_ERRORS, NULL, context);
	if (isescaped) {
		xmlFree(resolved_path);
	}
	return ret_val;
}

static void *php_libxml_streams_IO_open_read_wrapper(const char *filename)
{
	return php_libxml_streams_IO_open_wrapper(filename, "rb", 1);
}

static void *php_libxml_streams_IO_open_write_wrapper(const char *filename)
{
	return php_libxml_streams_IO_open_wrapper(filename, "wb", 0);
}

static int php_libxml_streams_IO_read(void *context, char *buffer, int len)
{
	TSRMLS_FETCH();
	return php_stream_read((php_stream*)context, buffer, len);
}

static int php_libxml_streams_IO_write(void *context, const char *buffer, int len)
{
	TSRMLS_FETCH();
	return php_stream_write((php_stream*)context, buffer, len);
}

static int php_libxml_streams_IO_close(void *context)
{
	TSRMLS_FETCH();
	return php_stream_close((php_stream*)context);
}

static xmlParserInputBufferPtr
php_libxml_input_buffer_noload(const char *URI, xmlCharEncoding enc)
{
	return NULL;
}

static xmlParserInputBufferPtr
php_libxml_input_buffer_create_filename(const char *URI, xmlCharEncoding enc)
{
	xmlParserInputBufferPtr ret;
	void *context = NULL;

	if (URI == NULL)
		return(NULL);

	context = php_libxml_streams_IO_open_read_wrapper(URI);

	if (context == NULL) {
		return(NULL);
	}

	/* Allocate the Input buffer front-end. */
	ret = xmlAllocParserInputBuffer(enc);
	if (ret != NULL) {
		ret->context = context;
		ret->readcallback = php_libxml_streams_IO_read;
		ret->closecallback = php_libxml_streams_IO_close;
	} else
		php_libxml_streams_IO_close(context);

	return(ret);
}

static xmlOutputBufferPtr
php_libxml_output_buffer_create_filename(const char *URI,
                              xmlCharEncodingHandlerPtr encoder,
                              int compression ATTRIBUTE_UNUSED)
{
	xmlOutputBufferPtr ret;
	xmlURIPtr puri;
	void *context = NULL;
	char *unescaped = NULL;

	if (URI == NULL)
		return(NULL);

	puri = xmlParseURI(URI);
	if (puri != NULL) {
		if (puri->scheme != NULL)
			unescaped = xmlURIUnescapeString(URI, 0, NULL);
		xmlFreeURI(puri);
	}

	if (unescaped != NULL) {
		context = php_libxml_streams_IO_open_write_wrapper(unescaped);
		xmlFree(unescaped);
	}

	/* try with a non-escaped URI this may be a strange filename */
	if (context == NULL) {
		context = php_libxml_streams_IO_open_write_wrapper(URI);
	}

	if (context == NULL) {
		return(NULL);
	}

	/* Allocate the Output buffer front-end. */
	ret = xmlAllocOutputBuffer(encoder);
	if (ret != NULL) {
		ret->context = context;
		ret->writecallback = php_libxml_streams_IO_write;
		ret->closecallback = php_libxml_streams_IO_close;
	}

	return(ret);
}

static int _php_libxml_free_error(xmlErrorPtr error)
{
	/* This will free the libxml alloc'd memory */
	xmlResetError(error);
	return 1;
}

static void _php_list_set_error_structure(xmlErrorPtr error, const char *msg)
{
	xmlError error_copy;
	int ret;

	TSRMLS_FETCH();

	memset(&error_copy, 0, sizeof(xmlError));

	if (error) {
		ret = xmlCopyError(error, &error_copy);
	} else {
		error_copy.domain = 0;
		error_copy.code = XML_ERR_INTERNAL_ERROR;
		error_copy.level = XML_ERR_ERROR;
		error_copy.line = 0;
		error_copy.node = NULL;
		error_copy.int1 = 0;
		error_copy.int2 = 0;
		error_copy.ctxt = NULL;
		error_copy.message = xmlStrdup(msg);
		error_copy.file = NULL;
		error_copy.str1 = NULL;
		error_copy.str2 = NULL;
		error_copy.str3 = NULL;
		ret = 0;
	}

	if (ret == 0) {
		zend_llist_add_element(LIBXML(error_list), &error_copy);
	}
}

static void php_libxml_ctx_error_level(int level, void *ctx, const char *msg TSRMLS_DC)
{
	xmlParserCtxtPtr parser;

	parser = (xmlParserCtxtPtr) ctx;

	if (parser != NULL && parser->input != NULL) {
		if (parser->input->filename) {
			php_error_docref(NULL TSRMLS_CC, level, "%s in %s, line: %d", msg, parser->input->filename, parser->input->line);
		} else {
			php_error_docref(NULL TSRMLS_CC, level, "%s in Entity, line: %d", msg, parser->input->line);
		}
	}
}

void php_libxml_issue_error(int level, const char *msg TSRMLS_DC)
{
	if (LIBXML(error_list)) {
		_php_list_set_error_structure(NULL, msg);
	} else {
		php_error_docref(NULL TSRMLS_CC, level, "%s", msg);
	}
}

static void php_libxml_internal_error_handler(int error_type, void *ctx, const char **msg, va_list ap)
{
	char *buf;
	int len, len_iter, output = 0;

	TSRMLS_FETCH();

	len = vspprintf(&buf, 0, *msg, ap);
	len_iter = len;

	/* remove any trailing \n */
	while (len_iter && buf[--len_iter] == '\n') {
		buf[len_iter] = '\0';
		output = 1;
	}

	smart_str_appendl(&LIBXML(error_buffer), buf, len);

	efree(buf);

	if (output == 1) {
		if (LIBXML(error_list)) {
			_php_list_set_error_structure(NULL, LIBXML(error_buffer).c);
		} else {
			switch (error_type) {
				case PHP_LIBXML_CTX_ERROR:
					php_libxml_ctx_error_level(E_WARNING, ctx, LIBXML(error_buffer).c TSRMLS_CC);
					break;
				case PHP_LIBXML_CTX_WARNING:
					php_libxml_ctx_error_level(E_NOTICE, ctx, LIBXML(error_buffer).c TSRMLS_CC);
					break;
				default:
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", LIBXML(error_buffer).c);
			}
		}
		smart_str_free(&LIBXML(error_buffer));
	}
}

static xmlParserInputPtr _php_libxml_external_entity_loader(const char *URL,
		const char *ID, xmlParserCtxtPtr context)
{
	xmlParserInputPtr	ret			= NULL;
	const char			*resource	= NULL;
	zval				*public		= NULL,
						*system		= NULL,
						*ctxzv		= NULL,
						**params[]	= {&public, &system, &ctxzv},
						*retval_ptr	= NULL;
	int					retval;
	zend_fcall_info		*fci;
	TSRMLS_FETCH();

	fci = &LIBXML(entity_loader).fci;
	
	if (fci->size == 0) {
		/* no custom user-land callback set up; delegate to original loader */
		return _php_libxml_default_entity_loader(URL, ID, context);
	}

	ALLOC_INIT_ZVAL(public);
	if (ID != NULL) {
		ZVAL_STRING(public, ID, 1);
	}
	ALLOC_INIT_ZVAL(system);
	if (URL != NULL) {
		ZVAL_STRING(system, URL, 1);
	}
	MAKE_STD_ZVAL(ctxzv);
	array_init_size(ctxzv, 4);

#define ADD_NULL_OR_STRING_KEY(memb) \
	if (context->memb == NULL) { \
		add_assoc_null_ex(ctxzv, #memb, sizeof(#memb)); \
	} else { \
		add_assoc_string_ex(ctxzv, #memb, sizeof(#memb), \
				(char *)context->memb, 1); \
	}
	
	ADD_NULL_OR_STRING_KEY(directory)
	ADD_NULL_OR_STRING_KEY(intSubName)
	ADD_NULL_OR_STRING_KEY(extSubURI)
	ADD_NULL_OR_STRING_KEY(extSubSystem)
	
#undef ADD_NULL_OR_STRING_KEY
	
	fci->retval_ptr_ptr	= &retval_ptr;
	fci->params			= params;
	fci->param_count	= sizeof(params)/sizeof(*params);
	fci->no_separation	= 1;
	
	retval = zend_call_function(fci, &LIBXML(entity_loader).fcc TSRMLS_CC);
	if (retval != SUCCESS || fci->retval_ptr_ptr == NULL) {
		php_libxml_ctx_error(context,
				"Call to user entity loader callback '%s' has failed",
				fci->function_name);
	} else {
		retval_ptr = *fci->retval_ptr_ptr;
		if (retval_ptr == NULL) {
			php_libxml_ctx_error(context,
					"Call to user entity loader callback '%s' has failed; "
					"probably it has thrown an exception",
					fci->function_name);
		} else if (Z_TYPE_P(retval_ptr) == IS_STRING) {
is_string:
			resource = Z_STRVAL_P(retval_ptr);
		} else if (Z_TYPE_P(retval_ptr) == IS_RESOURCE) {
			php_stream *stream;
			php_stream_from_zval_no_verify(stream, &retval_ptr);
			if (stream == NULL) {
				php_libxml_ctx_error(context,
						"The user entity loader callback '%s' has returned a "
						"resource, but it is not a stream",
						fci->function_name);
			} else {
				/* TODO: allow storing the encoding in the stream context? */
				xmlCharEncoding enc = XML_CHAR_ENCODING_NONE;
				xmlParserInputBufferPtr pib = xmlAllocParserInputBuffer(enc);
				if (pib == NULL) {
					php_libxml_ctx_error(context, "Could not allocate parser "
							"input buffer");
				} else {
					/* make stream not being closed when the zval is freed */
					zend_list_addref(stream->rsrc_id);
					pib->context = stream;
					pib->readcallback = php_libxml_streams_IO_read;
					pib->closecallback = php_libxml_streams_IO_close;
					
					ret = xmlNewIOInputStream(context, pib, enc);
					if (ret == NULL) {
						xmlFreeParserInputBuffer(pib);
					}
				}
			}
		} else if (Z_TYPE_P(retval_ptr) != IS_NULL) {
			/* retval not string nor resource nor null; convert to string */
			SEPARATE_ZVAL(&retval_ptr);
			convert_to_string(retval_ptr);
			goto is_string;
		} /* else is null; don't try anything */
	}

	if (ret == NULL) {
		if (resource == NULL) {
			if (ID == NULL) {
				ID = "NULL";
			}
			php_libxml_ctx_error(context,
					"Failed to load external entity \"%s\"\n", ID);
		} else {
			/* we got the resource in the form of a string; open it */
			ret = xmlNewInputFromFile(context, resource);
		}
	}

	zval_ptr_dtor(&public);
	zval_ptr_dtor(&system);
	zval_ptr_dtor(&ctxzv);
	if (retval_ptr != NULL) {
		zval_ptr_dtor(&retval_ptr);
	}
	return ret;
}

static xmlParserInputPtr _php_libxml_pre_ext_ent_loader(const char *URL,
		const char *ID, xmlParserCtxtPtr context)
{
	TSRMLS_FETCH();

	/* Check whether we're running in a PHP context, since the entity loader
	 * we've defined is an application level (true global) setting.
	 * If we are, we also want to check whether we've finished activating
	 * the modules (RINIT phase). Using our external entity loader during a
	 * RINIT should not be problem per se (though during MINIT it is, because
	 * we don't even have a resource list by then), but then whether one
	 * extension would be using the custom external entity loader or not
	 * could depend on extension loading order
	 * (if _php_libxml_per_request_initialization */
	if (xmlGenericError == php_libxml_error_handler && PG(modules_activated)) {
		return _php_libxml_external_entity_loader(URL, ID, context);
	} else {
		return _php_libxml_default_entity_loader(URL, ID, context);
	}
}

PHP_LIBXML_API void php_libxml_ctx_error(void *ctx, const char *msg, ...)
{
	va_list args;
	va_start(args, msg);
	php_libxml_internal_error_handler(PHP_LIBXML_CTX_ERROR, ctx, &msg, args);
	va_end(args);
}

PHP_LIBXML_API void php_libxml_ctx_warning(void *ctx, const char *msg, ...)
{
	va_list args;
	va_start(args, msg);
	php_libxml_internal_error_handler(PHP_LIBXML_CTX_WARNING, ctx, &msg, args);
	va_end(args);
}

PHP_LIBXML_API void php_libxml_structured_error_handler(void *userData, xmlErrorPtr error)
{
	_php_list_set_error_structure(error, NULL);

	return;
}

PHP_LIBXML_API void php_libxml_error_handler(void *ctx, const char *msg, ...)
{
	va_list args;
	va_start(args, msg);
	php_libxml_internal_error_handler(PHP_LIBXML_ERROR, ctx, &msg, args);
	va_end(args);
}


PHP_LIBXML_API void php_libxml_initialize(void)
{
	if (!_php_libxml_initialized) {
		/* we should be the only one's to ever init!! */
		xmlInitParser();
		
		_php_libxml_default_entity_loader = xmlGetExternalEntityLoader();
		xmlSetExternalEntityLoader(_php_libxml_pre_ext_ent_loader);

		zend_hash_init(&php_libxml_exports, 0, NULL, NULL, 1);

		_php_libxml_initialized = 1;
	}
}

PHP_LIBXML_API void php_libxml_shutdown(void)
{
	if (_php_libxml_initialized) {
#if defined(LIBXML_SCHEMAS_ENABLED)
		xmlRelaxNGCleanupTypes();
#endif
		xmlCleanupParser();
		zend_hash_destroy(&php_libxml_exports);
		
		xmlSetExternalEntityLoader(_php_libxml_default_entity_loader);
		_php_libxml_initialized = 0;
	}
}

PHP_LIBXML_API zval *php_libxml_switch_context(zval *context TSRMLS_DC)
{
	zval *oldcontext;

	oldcontext = LIBXML(stream_context);
	LIBXML(stream_context) = context;
	return oldcontext;

}

static PHP_MINIT_FUNCTION(libxml)
{
	zend_class_entry ce;

	php_libxml_initialize();

	REGISTER_LONG_CONSTANT("LIBXML_VERSION",			LIBXML_VERSION,			CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("LIBXML_DOTTED_VERSION",	LIBXML_DOTTED_VERSION,	CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("LIBXML_LOADED_VERSION",	(char *)xmlParserVersion,		CONST_CS | CONST_PERSISTENT);

	/* For use with loading xml */
	REGISTER_LONG_CONSTANT("LIBXML_NOENT",		XML_PARSE_NOENT,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_DTDLOAD",	XML_PARSE_DTDLOAD,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_DTDATTR",	XML_PARSE_DTDATTR,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_DTDVALID",	XML_PARSE_DTDVALID,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_NOERROR",	XML_PARSE_NOERROR,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_NOWARNING",	XML_PARSE_NOWARNING,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_NOBLANKS",	XML_PARSE_NOBLANKS,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_XINCLUDE",	XML_PARSE_XINCLUDE,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_NSCLEAN",	XML_PARSE_NSCLEAN,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_NOCDATA",	XML_PARSE_NOCDATA,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_NONET",		XML_PARSE_NONET,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_PEDANTIC",	XML_PARSE_PEDANTIC,		CONST_CS | CONST_PERSISTENT);
#if LIBXML_VERSION >= 20621
	REGISTER_LONG_CONSTANT("LIBXML_COMPACT",	XML_PARSE_COMPACT,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_NOXMLDECL",	XML_SAVE_NO_DECL,		CONST_CS | CONST_PERSISTENT);
#endif
#if LIBXML_VERSION >= 20703
	REGISTER_LONG_CONSTANT("LIBXML_PARSEHUGE",	XML_PARSE_HUGE,			CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("LIBXML_NOEMPTYTAG",	LIBXML_SAVE_NOEMPTYTAG,	CONST_CS | CONST_PERSISTENT);

	/* Additional constants for use with loading html */
#if LIBXML_VERSION >= 20707
	REGISTER_LONG_CONSTANT("LIBXML_HTML_NOIMPLIED",	HTML_PARSE_NOIMPLIED,		CONST_CS | CONST_PERSISTENT);
#endif

#if LIBXML_VERSION >= 20708
	REGISTER_LONG_CONSTANT("LIBXML_HTML_NODEFDTD",	HTML_PARSE_NODEFDTD,		CONST_CS | CONST_PERSISTENT);
#endif

	/* Error levels */
	REGISTER_LONG_CONSTANT("LIBXML_ERR_NONE",		XML_ERR_NONE,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_ERR_WARNING",	XML_ERR_WARNING,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_ERR_ERROR",		XML_ERR_ERROR,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_ERR_FATAL",		XML_ERR_FATAL,		CONST_CS | CONST_PERSISTENT);

	INIT_CLASS_ENTRY(ce, "LibXMLError", NULL);
	libxmlerror_class_entry = zend_register_internal_class(&ce TSRMLS_CC);

	if (sapi_module.name) {
		static const char * const supported_sapis[] = {
			"cgi-fcgi",
			"fpm-fcgi",
			"litespeed",
			NULL
		};
		const char * const *sapi_name;

		for (sapi_name = supported_sapis; *sapi_name; sapi_name++) {
			if (strcmp(sapi_module.name, *sapi_name) == 0) {
				_php_libxml_per_request_initialization = 0;
				break;
			}
		}
	}

	if (!_php_libxml_per_request_initialization) {
		/* report errors via handler rather than stderr */
		xmlSetGenericErrorFunc(NULL, php_libxml_error_handler);
		xmlParserInputBufferCreateFilenameDefault(php_libxml_input_buffer_create_filename);
		xmlOutputBufferCreateFilenameDefault(php_libxml_output_buffer_create_filename);
	}
	
	return SUCCESS;
}


static PHP_RINIT_FUNCTION(libxml)
{
	if (_php_libxml_per_request_initialization) {
		/* report errors via handler rather than stderr */
		xmlSetGenericErrorFunc(NULL, php_libxml_error_handler);
		xmlParserInputBufferCreateFilenameDefault(php_libxml_input_buffer_create_filename);
		xmlOutputBufferCreateFilenameDefault(php_libxml_output_buffer_create_filename);
	}
	return SUCCESS;
}


static PHP_MSHUTDOWN_FUNCTION(libxml)
{
	if (!_php_libxml_per_request_initialization) {
		xmlSetGenericErrorFunc(NULL, NULL);

		xmlParserInputBufferCreateFilenameDefault(NULL);
		xmlOutputBufferCreateFilenameDefault(NULL);
	}
	php_libxml_shutdown();

	return SUCCESS;
}

static int php_libxml_post_deactivate()
{
	TSRMLS_FETCH();
	/* reset libxml generic error handling */
	if (_php_libxml_per_request_initialization) {
		xmlSetGenericErrorFunc(NULL, NULL);

		xmlParserInputBufferCreateFilenameDefault(NULL);
		xmlOutputBufferCreateFilenameDefault(NULL);
	}
	xmlSetStructuredErrorFunc(NULL, NULL);

	if (LIBXML(stream_context)) {
		/* the steam_context resource will be released by resource list destructor */
		efree(LIBXML(stream_context));
		LIBXML(stream_context) = NULL;
	}
	smart_str_free(&LIBXML(error_buffer));
	if (LIBXML(error_list)) {
		zend_llist_destroy(LIBXML(error_list));
		efree(LIBXML(error_list));
		LIBXML(error_list) = NULL;
	}
	xmlResetLastError();
	
	_php_libxml_destroy_fci(&LIBXML(entity_loader).fci);

	return SUCCESS;
}


static PHP_MINFO_FUNCTION(libxml)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "libXML support", "active");
	php_info_print_table_row(2, "libXML Compiled Version", LIBXML_DOTTED_VERSION);
	php_info_print_table_row(2, "libXML Loaded Version", (char *)xmlParserVersion);
	php_info_print_table_row(2, "libXML streams", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto void libxml_set_streams_context(resource streams_context) 
   Set the streams context for the next libxml document load or write */
static PHP_FUNCTION(libxml_set_streams_context)
{
	zval *arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &arg) == FAILURE) {
		return;
	}
	if (LIBXML(stream_context)) {
		zval_ptr_dtor(&LIBXML(stream_context));
		LIBXML(stream_context) = NULL;
	}
	Z_ADDREF_P(arg);
	LIBXML(stream_context) = arg;
}
/* }}} */

/* {{{ proto bool libxml_use_internal_errors([boolean use_errors]) 
   Disable libxml errors and allow user to fetch error information as needed */
static PHP_FUNCTION(libxml_use_internal_errors)
{
	xmlStructuredErrorFunc current_handler;
	zend_bool use_errors=0, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &use_errors) == FAILURE) {
		return;
	}

	current_handler = xmlStructuredError;
	if (current_handler && current_handler == php_libxml_structured_error_handler) {
		retval = 1;
	} else {
		retval = 0;
	}

	if (ZEND_NUM_ARGS() == 0) {
		RETURN_BOOL(retval);
	}

	if (use_errors == 0) {
		xmlSetStructuredErrorFunc(NULL, NULL);
		if (LIBXML(error_list)) {
			zend_llist_destroy(LIBXML(error_list));
			efree(LIBXML(error_list));
			LIBXML(error_list) = NULL;
		}
	} else {
		xmlSetStructuredErrorFunc(NULL, php_libxml_structured_error_handler);
		if (LIBXML(error_list) == NULL) {
			LIBXML(error_list) = (zend_llist *) emalloc(sizeof(zend_llist));
			zend_llist_init(LIBXML(error_list), sizeof(xmlError), (llist_dtor_func_t) _php_libxml_free_error, 0);
		}
	}
	RETURN_BOOL(retval);
}
/* }}} */

/* {{{ proto object libxml_get_last_error() 
   Retrieve last error from libxml */
static PHP_FUNCTION(libxml_get_last_error)
{
	xmlErrorPtr error;

	error = xmlGetLastError();
	
	if (error) {
		object_init_ex(return_value, libxmlerror_class_entry);
		add_property_long(return_value, "level", error->level);
		add_property_long(return_value, "code", error->code);
		add_property_long(return_value, "column", error->int2);
		if (error->message) {
			add_property_string(return_value, "message", error->message, 1);
		} else {
			add_property_stringl(return_value, "message", "", 0, 1);
		}
		if (error->file) {
			add_property_string(return_value, "file", error->file, 1);
		} else {
			add_property_stringl(return_value, "file", "", 0, 1);
		}
		add_property_long(return_value, "line", error->line);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto object libxml_get_errors()
   Retrieve array of errors */
static PHP_FUNCTION(libxml_get_errors)
{
	
	xmlErrorPtr error;

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	if (LIBXML(error_list)) {

		error = zend_llist_get_first(LIBXML(error_list));

		while (error != NULL) {
			zval *z_error;
			MAKE_STD_ZVAL(z_error);

			object_init_ex(z_error, libxmlerror_class_entry);
			add_property_long(z_error, "level", error->level);
			add_property_long(z_error, "code", error->code);
			add_property_long(z_error, "column", error->int2);
			if (error->message) {
				add_property_string(z_error, "message", error->message, 1);
			} else {
				add_property_stringl(z_error, "message", "", 0, 1);
			}
			if (error->file) {
				add_property_string(z_error, "file", error->file, 1);
			} else {
				add_property_stringl(z_error, "file", "", 0, 1);
			}
			add_property_long(z_error, "line", error->line);
			add_next_index_zval(return_value, z_error);

			error = zend_llist_get_next(LIBXML(error_list));
		}
	}
}
/* }}} */

/* {{{ proto void libxml_clear_errors() 
   Clear last error from libxml */
static PHP_FUNCTION(libxml_clear_errors)
{
	xmlResetLastError();
	if (LIBXML(error_list)) {
		zend_llist_clean(LIBXML(error_list));
	}
}
/* }}} */

/* {{{ proto bool libxml_disable_entity_loader([boolean disable]) 
   Disable/Enable ability to load external entities */
static PHP_FUNCTION(libxml_disable_entity_loader)
{
	zend_bool disable = 1;
	xmlParserInputBufferCreateFilenameFunc old;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &disable) == FAILURE) {
		return;
	}

	if (disable == 0) {
		old = xmlParserInputBufferCreateFilenameDefault(php_libxml_input_buffer_create_filename);
	} else {
		old = xmlParserInputBufferCreateFilenameDefault(php_libxml_input_buffer_noload);
	}

	if (old == php_libxml_input_buffer_noload) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto void libxml_set_external_entity_loader(callback resolver_function) 
   Changes the default external entity loader */
static PHP_FUNCTION(libxml_set_external_entity_loader)
{
	zend_fcall_info			fci;
	zend_fcall_info_cache	fcc;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "f!", &fci, &fcc)
			== FAILURE) {
		return;
	}
	
	_php_libxml_destroy_fci(&LIBXML(entity_loader).fci);
	
	if (fci.size > 0) { /* argument not null */
		LIBXML(entity_loader).fci = fci;
		Z_ADDREF_P(fci.function_name);
		if (fci.object_ptr != NULL) {
			Z_ADDREF_P(fci.object_ptr);
		}
		LIBXML(entity_loader).fcc = fcc;
	}
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ Common functions shared by extensions */
int php_libxml_xmlCheckUTF8(const unsigned char *s)
{
	int i;
	unsigned char c;

	for (i = 0; (c = s[i++]);) {
		if ((c & 0x80) == 0) {
		} else if ((c & 0xe0) == 0xc0) {
			if ((s[i++] & 0xc0) != 0x80) {
				return 0;
			}
		} else if ((c & 0xf0) == 0xe0) {
			if ((s[i++] & 0xc0) != 0x80 || (s[i++] & 0xc0) != 0x80) {
				return 0;
			}
		} else if ((c & 0xf8) == 0xf0) {
			if ((s[i++] & 0xc0) != 0x80 || (s[i++] & 0xc0) != 0x80 || (s[i++] & 0xc0) != 0x80) {
				return 0;
			}
		} else {
			return 0;
		}
	}
	return 1;
}

int php_libxml_register_export(zend_class_entry *ce, php_libxml_export_node export_function)
{
	php_libxml_func_handler export_hnd;
	
	/* Initialize in case this module hasnt been loaded yet */
	php_libxml_initialize();
	export_hnd.export_func = export_function;

	return zend_hash_add(&php_libxml_exports, ce->name, ce->name_length + 1, &export_hnd, sizeof(export_hnd), NULL);
}

PHP_LIBXML_API xmlNodePtr php_libxml_import_node(zval *object TSRMLS_DC)
{
	zend_class_entry *ce = NULL;
	xmlNodePtr node = NULL;
	php_libxml_func_handler *export_hnd;

	if (object->type == IS_OBJECT) {
		ce = Z_OBJCE_P(object);
		while (ce->parent != NULL) {
			ce = ce->parent;
		}
		if (zend_hash_find(&php_libxml_exports, ce->name, ce->name_length + 1, (void **) &export_hnd)  == SUCCESS) {
			node = export_hnd->export_func(object TSRMLS_CC);
		}
	}
	return node;
}

PHP_LIBXML_API int php_libxml_increment_node_ptr(php_libxml_node_object *object, xmlNodePtr node, void *private_data TSRMLS_DC)
{
	int ret_refcount = -1;

	if (object != NULL && node != NULL) {
		if (object->node != NULL) {
			if (object->node->node == node) {
				return object->node->refcount;
			} else {
				php_libxml_decrement_node_ptr(object TSRMLS_CC);
			}
		}
		if (node->_private != NULL) {
			object->node = node->_private;
			ret_refcount = ++object->node->refcount;
			/* Only dom uses _private */
			if (object->node->_private == NULL) {
				object->node->_private = private_data;
			}
		} else {
			ret_refcount = 1;
			object->node = emalloc(sizeof(php_libxml_node_ptr));
			object->node->node = node;
			object->node->refcount = 1;
			object->node->_private = private_data;
			node->_private = object->node;
		}
	}

	return ret_refcount;
}

PHP_LIBXML_API int php_libxml_decrement_node_ptr(php_libxml_node_object *object TSRMLS_DC)
{
	int ret_refcount = -1;
	php_libxml_node_ptr *obj_node;

	if (object != NULL && object->node != NULL) {
		obj_node = (php_libxml_node_ptr *) object->node;
		ret_refcount = --obj_node->refcount;
		if (ret_refcount == 0) {
			if (obj_node->node != NULL) {
				obj_node->node->_private = NULL;
			}
			efree(obj_node);
		} 
		object->node = NULL;
	}

	return ret_refcount;
}

PHP_LIBXML_API int php_libxml_increment_doc_ref(php_libxml_node_object *object, xmlDocPtr docp TSRMLS_DC)
{
	int ret_refcount = -1;

	if (object->document != NULL) {
		object->document->refcount++;
		ret_refcount = object->document->refcount;
	} else if (docp != NULL) {
		ret_refcount = 1;
		object->document = emalloc(sizeof(php_libxml_ref_obj));
		object->document->ptr = docp;
		object->document->refcount = ret_refcount;
		object->document->doc_props = NULL;
	}

	return ret_refcount;
}

PHP_LIBXML_API int php_libxml_decrement_doc_ref(php_libxml_node_object *object TSRMLS_DC)
{
	int ret_refcount = -1;

	if (object != NULL && object->document != NULL) {
		ret_refcount = --object->document->refcount;
		if (ret_refcount == 0) {
			if (object->document->ptr != NULL) {
				xmlFreeDoc((xmlDoc *) object->document->ptr);
			}
			if (object->document->doc_props != NULL) {
				if (object->document->doc_props->classmap) {
					zend_hash_destroy(object->document->doc_props->classmap);
					FREE_HASHTABLE(object->document->doc_props->classmap);
				}
				efree(object->document->doc_props);
			}
			efree(object->document);
			object->document = NULL;
		}
	}

	return ret_refcount;
}

PHP_LIBXML_API void php_libxml_node_free_resource(xmlNodePtr node TSRMLS_DC)
{
	if (!node) {
		return;
	}

	switch (node->type) {
		case XML_DOCUMENT_NODE:
		case XML_HTML_DOCUMENT_NODE:
			break;
		default:
			if (node->parent == NULL || node->type == XML_NAMESPACE_DECL) {
				php_libxml_node_free_list((xmlNodePtr) node->children TSRMLS_CC);
				switch (node->type) {
					/* Skip property freeing for the following types */
					case XML_ATTRIBUTE_DECL:
					case XML_DTD_NODE:
					case XML_DOCUMENT_TYPE_NODE:
					case XML_ENTITY_DECL:
					case XML_ATTRIBUTE_NODE:
					case XML_NAMESPACE_DECL:
					case XML_TEXT_NODE:
						break;
					default:
						php_libxml_node_free_list((xmlNodePtr) node->properties TSRMLS_CC);
				}
				if (php_libxml_unregister_node(node TSRMLS_CC) == 0) {
					node->doc = NULL;
				}
				php_libxml_node_free(node);
			} else {
				php_libxml_unregister_node(node TSRMLS_CC);
			}
	}
}

PHP_LIBXML_API void php_libxml_node_decrement_resource(php_libxml_node_object *object TSRMLS_DC)
{
	int ret_refcount = -1;
	xmlNodePtr nodep;
	php_libxml_node_ptr *obj_node;

	if (object != NULL && object->node != NULL) {
		obj_node = (php_libxml_node_ptr *) object->node;
		nodep = object->node->node;
		ret_refcount = php_libxml_decrement_node_ptr(object TSRMLS_CC);
		if (ret_refcount == 0) {
			php_libxml_node_free_resource(nodep TSRMLS_CC);
		} else {
			if (obj_node && object == obj_node->_private) {
				obj_node->_private = NULL;
			}
		}
	}
	if (object != NULL && object->document != NULL) {
		/* Safe to call as if the resource were freed then doc pointer is NULL */
		php_libxml_decrement_doc_ref(object TSRMLS_CC);
	}
}
/* }}} */

#ifdef PHP_WIN32
PHP_LIBXML_API BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	return xmlDllMain(hinstDLL, fdwReason, lpvReserved);
}
#endif

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
