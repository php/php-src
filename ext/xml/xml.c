/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2009 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stig Sæther Bakken <ssb@php.net>                            |
   |          Thies C. Arntzen <thies@thieso.net>                         |
   |          Sterling Hughes <sterling@php.net>                          |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#define IS_EXT_MODULE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#define PHP_XML_INTERNAL
#include "zend_variables.h"
#include "ext/standard/php_string.h"
#include "ext/standard/info.h"

#if HAVE_XML

#include "php_xml.h"
# include "ext/standard/head.h"
#ifdef LIBXML_EXPAT_COMPAT
#include "ext/libxml/php_libxml.h"
#endif

/* Short-term TODO list:
 * - Implement XML_ExternalEntityParserCreate()
 * - XML_SetCommentHandler
 * - XML_SetCdataSectionHandler
 * - XML_SetParamEntityParsing
 */

/* Long-term TODO list:
 * - Fix the expat library so you can install your own memory manager
 *   functions
 */

/* Known bugs:
 * - Weird things happen with <![CDATA[]]> sections.
 */

ZEND_DECLARE_MODULE_GLOBALS(xml)

/* {{{ dynamically loadable module stuff */
#ifdef COMPILE_DL_XML
ZEND_GET_MODULE(xml)
# ifdef PHP_WIN32
# include "zend_arg_defs.c"
# endif
#endif /* COMPILE_DL_XML */
/* }}} */

/* {{{ function prototypes */
PHP_MINIT_FUNCTION(xml);
PHP_MINFO_FUNCTION(xml);
static PHP_GINIT_FUNCTION(xml);

static void xml_parser_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC);
static void xml_set_handler(zval **, zval **);
inline static unsigned short xml_encode_iso_8859_1(unsigned char);
inline static char xml_decode_iso_8859_1(unsigned short);
inline static unsigned short xml_encode_us_ascii(unsigned char);
inline static char xml_decode_us_ascii(unsigned short);
static zval *xml_call_handler(xml_parser *, zval *, zend_function *, int, zval **);
static zval *_xml_xmlchar_zval(const XML_Char *, int, const XML_Char *);
static int _xml_xmlcharlen(const XML_Char *);
static void _xml_add_to_info(xml_parser *parser,char *name);
inline static char *_xml_decode_tag(xml_parser *parser, const char *tag);

void _xml_startElementHandler(void *, const XML_Char *, const XML_Char **);
void _xml_endElementHandler(void *, const XML_Char *);
void _xml_characterDataHandler(void *, const XML_Char *, int);
void _xml_processingInstructionHandler(void *, const XML_Char *, const XML_Char *);
void _xml_defaultHandler(void *, const XML_Char *, int);
void _xml_unparsedEntityDeclHandler(void *, const XML_Char *, const XML_Char *, const XML_Char *, const XML_Char *, const XML_Char *);
void _xml_notationDeclHandler(void *, const XML_Char *, const XML_Char *, const XML_Char *, const XML_Char *);
int  _xml_externalEntityRefHandler(XML_Parser, const XML_Char *, const XML_Char *, const XML_Char *, const XML_Char *);

void _xml_startNamespaceDeclHandler(void *, const XML_Char *, const XML_Char *);
void _xml_endNamespaceDeclHandler(void *, const XML_Char *);
/* }}} */

/* {{{ extension definition structures */
static
	ZEND_BEGIN_ARG_INFO(third_and_fourth_args_force_ref, 0)
		ZEND_ARG_PASS_INFO(0)
		ZEND_ARG_PASS_INFO(0)
		ZEND_ARG_PASS_INFO(1)
		ZEND_ARG_PASS_INFO(1)
	ZEND_END_ARG_INFO();

zend_function_entry xml_functions[] = {
	PHP_FE(xml_parser_create, NULL)
	PHP_FE(xml_parser_create_ns, NULL)
	PHP_FE(xml_set_object, second_arg_force_ref)
	PHP_FE(xml_set_element_handler, NULL)
	PHP_FE(xml_set_character_data_handler, NULL)
	PHP_FE(xml_set_processing_instruction_handler, NULL)
	PHP_FE(xml_set_default_handler, NULL)
	PHP_FE(xml_set_unparsed_entity_decl_handler, NULL)
	PHP_FE(xml_set_notation_decl_handler, NULL)
	PHP_FE(xml_set_external_entity_ref_handler, NULL)
	PHP_FE(xml_set_start_namespace_decl_handler, NULL)
	PHP_FE(xml_set_end_namespace_decl_handler, NULL)
	PHP_FE(xml_parse, NULL)
	PHP_FE(xml_parse_into_struct, third_and_fourth_args_force_ref)
	PHP_FE(xml_get_error_code, NULL)
	PHP_FE(xml_error_string, NULL)
	PHP_FE(xml_get_current_line_number, NULL)
	PHP_FE(xml_get_current_column_number, NULL)
	PHP_FE(xml_get_current_byte_index, NULL)
	PHP_FE(xml_parser_free, NULL)
	PHP_FE(xml_parser_set_option, NULL)
	PHP_FE(xml_parser_get_option, NULL)
	PHP_FE(utf8_encode, NULL)
	PHP_FE(utf8_decode, NULL)
	{NULL, NULL, NULL}
};

#ifdef LIBXML_EXPAT_COMPAT
static zend_module_dep xml_deps[] = {
	ZEND_MOD_REQUIRED("libxml")
	{NULL, NULL, NULL}
};
#endif

zend_module_entry xml_module_entry = {
#ifdef LIBXML_EXPAT_COMPAT
    STANDARD_MODULE_HEADER_EX, NULL,
	xml_deps,
#else
    STANDARD_MODULE_HEADER,
#endif
	"xml",                /* extension name */
	xml_functions,        /* extension function list */
	PHP_MINIT(xml),       /* extension-wide startup function */
	NULL,                 /* extension-wide shutdown function */
	NULL,                 /* per-request startup function */
	NULL,                 /* per-request shutdown function */
	PHP_MINFO(xml),       /* information function */
    NO_VERSION_YET,
    PHP_MODULE_GLOBALS(xml), /* globals descriptor */
    PHP_GINIT(xml),          /* globals ctor */
    NULL,                    /* globals dtor */
    NULL,                    /* post deactivate */
	STANDARD_MODULE_PROPERTIES_EX
};

/* All the encoding functions are set to NULL right now, since all
 * the encoding is currently done internally by expat/xmltok.
 */
xml_encoding xml_encodings[] = {
	{ "ISO-8859-1", xml_decode_iso_8859_1, xml_encode_iso_8859_1 },
	{ "US-ASCII",   xml_decode_us_ascii,   xml_encode_us_ascii   },
	{ "UTF-8",      NULL,                  NULL                  },
	{ NULL,         NULL,                  NULL                  }
};

static XML_Memory_Handling_Suite php_xml_mem_hdlrs;

/* True globals, no need for thread safety */
static int le_xml_parser; 

/* }}} */

/* {{{ startup, shutdown and info functions */
static PHP_GINIT_FUNCTION(xml)
{
	xml_globals->default_encoding = "UTF-8";
}

static void *php_xml_malloc_wrapper(size_t sz)
{
	return emalloc(sz);
}

static void *php_xml_realloc_wrapper(void *ptr, size_t sz)
{
	return erealloc(ptr, sz);
}

static void php_xml_free_wrapper(void *ptr)
{
	if (ptr != NULL) {
		efree(ptr);
	}
}

PHP_MINIT_FUNCTION(xml)
{
	le_xml_parser =	zend_register_list_destructors_ex(xml_parser_dtor, NULL, "xml", module_number);

	REGISTER_LONG_CONSTANT("XML_ERROR_NONE", XML_ERROR_NONE, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_NO_MEMORY", XML_ERROR_NO_MEMORY, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_SYNTAX", XML_ERROR_SYNTAX, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_NO_ELEMENTS", XML_ERROR_NO_ELEMENTS, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_INVALID_TOKEN", XML_ERROR_INVALID_TOKEN, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_UNCLOSED_TOKEN", XML_ERROR_UNCLOSED_TOKEN, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_PARTIAL_CHAR", XML_ERROR_PARTIAL_CHAR, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_TAG_MISMATCH", XML_ERROR_TAG_MISMATCH, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_DUPLICATE_ATTRIBUTE", XML_ERROR_DUPLICATE_ATTRIBUTE, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_JUNK_AFTER_DOC_ELEMENT", XML_ERROR_JUNK_AFTER_DOC_ELEMENT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_PARAM_ENTITY_REF", XML_ERROR_PARAM_ENTITY_REF, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_UNDEFINED_ENTITY", XML_ERROR_UNDEFINED_ENTITY, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_RECURSIVE_ENTITY_REF", XML_ERROR_RECURSIVE_ENTITY_REF, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_ASYNC_ENTITY", XML_ERROR_ASYNC_ENTITY, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_BAD_CHAR_REF", XML_ERROR_BAD_CHAR_REF, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_BINARY_ENTITY_REF", XML_ERROR_BINARY_ENTITY_REF, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_ATTRIBUTE_EXTERNAL_ENTITY_REF", XML_ERROR_ATTRIBUTE_EXTERNAL_ENTITY_REF, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_MISPLACED_XML_PI", XML_ERROR_MISPLACED_XML_PI, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_UNKNOWN_ENCODING", XML_ERROR_UNKNOWN_ENCODING, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_INCORRECT_ENCODING", XML_ERROR_INCORRECT_ENCODING, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_UNCLOSED_CDATA_SECTION", XML_ERROR_UNCLOSED_CDATA_SECTION, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_EXTERNAL_ENTITY_HANDLING", XML_ERROR_EXTERNAL_ENTITY_HANDLING, CONST_CS|CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("XML_OPTION_CASE_FOLDING", PHP_XML_OPTION_CASE_FOLDING, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_OPTION_TARGET_ENCODING", PHP_XML_OPTION_TARGET_ENCODING, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_OPTION_SKIP_TAGSTART", PHP_XML_OPTION_SKIP_TAGSTART, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_OPTION_SKIP_WHITE", PHP_XML_OPTION_SKIP_WHITE, CONST_CS|CONST_PERSISTENT);

	/* this object should not be pre-initialised at compile time,
	   as the order of members may vary */  

	php_xml_mem_hdlrs.malloc_fcn = php_xml_malloc_wrapper;
	php_xml_mem_hdlrs.realloc_fcn = php_xml_realloc_wrapper;
	php_xml_mem_hdlrs.free_fcn = php_xml_free_wrapper;

#ifdef LIBXML_EXPAT_COMPAT
	REGISTER_STRING_CONSTANT("XML_SAX_IMPL", "libxml", CONST_CS|CONST_PERSISTENT);
#else
	REGISTER_STRING_CONSTANT("XML_SAX_IMPL", "expat", CONST_CS|CONST_PERSISTENT);
#endif

	return SUCCESS;
}

PHP_MINFO_FUNCTION(xml)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "XML Support", "active");
	php_info_print_table_row(2, "XML Namespace Support", "active");
#if defined(LIBXML_DOTTED_VERSION) && defined(LIBXML_EXPAT_COMPAT)
	php_info_print_table_row(2, "libxml2 Version", LIBXML_DOTTED_VERSION);
#else
	php_info_print_table_row(2, "EXPAT Version", XML_ExpatVersion());
#endif
	php_info_print_table_end();
}
/* }}} */

/* {{{ extension-internal functions */
static zval *_xml_resource_zval(long value)
{
	zval *ret;
	TSRMLS_FETCH();

	MAKE_STD_ZVAL(ret);

	Z_TYPE_P(ret) = IS_RESOURCE;
	Z_LVAL_P(ret) = value;

	zend_list_addref(value);

	return ret;
}

static zval *_xml_string_zval(const char *str)
{
	zval *ret;
	int len = strlen(str);
	MAKE_STD_ZVAL(ret);

	Z_TYPE_P(ret) = IS_STRING;
	Z_STRLEN_P(ret) = len;
	Z_STRVAL_P(ret) = estrndup(str, len);
	return ret;
}

static zval *_xml_xmlchar_zval(const XML_Char *s, int len, const XML_Char *encoding)
{
	zval *ret;
	MAKE_STD_ZVAL(ret);
	
	if (s == NULL) {
		ZVAL_FALSE(ret);
		return ret;
	}
	if (len == 0) {
		len = _xml_xmlcharlen(s);
	}
	Z_TYPE_P(ret) = IS_STRING;
	Z_STRVAL_P(ret) = xml_utf8_decode(s, len, &Z_STRLEN_P(ret), encoding);
	return ret;
}
/* }}} */

/* {{{ xml_parser_dtor() */
static void xml_parser_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	xml_parser *parser = (xml_parser *)rsrc->ptr;
	
	if (parser->parser) {
		XML_ParserFree(parser->parser);
	}
	if (parser->ltags) {
		int inx;
		for (inx = 0; inx < parser->level; inx++)
			efree(parser->ltags[ inx ]);
		efree(parser->ltags);
	}
	if (parser->startElementHandler) {
		zval_ptr_dtor(&parser->startElementHandler);
	}
	if (parser->endElementHandler) {
		zval_ptr_dtor(&parser->endElementHandler);
	}
	if (parser->characterDataHandler) {
		zval_ptr_dtor(&parser->characterDataHandler);
	}
	if (parser->processingInstructionHandler) {
		zval_ptr_dtor(&parser->processingInstructionHandler);
	}
	if (parser->defaultHandler) {
		zval_ptr_dtor(&parser->defaultHandler);
	}
	if (parser->unparsedEntityDeclHandler) {
		zval_ptr_dtor(&parser->unparsedEntityDeclHandler);
	}
	if (parser->notationDeclHandler) {
		zval_ptr_dtor(&parser->notationDeclHandler);
	}
	if (parser->externalEntityRefHandler) {
		zval_ptr_dtor(&parser->externalEntityRefHandler);
	}
	if (parser->unknownEncodingHandler) {
		zval_ptr_dtor(&parser->unknownEncodingHandler);
	}
	if (parser->startNamespaceDeclHandler) {
		zval_ptr_dtor(&parser->startNamespaceDeclHandler);
	}
	if (parser->endNamespaceDeclHandler) {
		zval_ptr_dtor(&parser->endNamespaceDeclHandler);
	}
	if (parser->baseURI) {
		efree(parser->baseURI);
	}
	if (parser->object) {
		zval_ptr_dtor(&parser->object);
	}

	efree(parser);
}
/* }}} */

/* {{{ xml_set_handler() */
static void xml_set_handler(zval **handler, zval **data)
{
	/* If we have already a handler, release it */
	if (*handler) {
		zval_ptr_dtor(handler);
	}

	/* IS_ARRAY might indicate that we're using array($obj, 'method') syntax */
	if (Z_TYPE_PP(data) != IS_ARRAY) {

		convert_to_string_ex(data);
		if (Z_STRLEN_PP(data) == 0) {
			*handler = NULL;
			return;
		}
	}

	zval_add_ref(data);

	*handler = *data;
}
/* }}} */

/* {{{ xml_call_handler() */
static zval *xml_call_handler(xml_parser *parser, zval *handler, zend_function *function_ptr, int argc, zval **argv)
{
	int i;	
	TSRMLS_FETCH();

	if (parser && handler && !EG(exception)) {
		zval ***args;
		zval *retval;
		int result;
		zend_fcall_info fci;

		args = safe_emalloc(sizeof(zval **), argc, 0);
		for (i = 0; i < argc; i++) {
			args[i] = &argv[i];
		}
		
		fci.size = sizeof(fci);
		fci.function_table = EG(function_table);
		fci.function_name = handler;
		fci.symbol_table = NULL;
		fci.object_pp = &parser->object;
		fci.retval_ptr_ptr = &retval;
		fci.param_count = argc;
		fci.params = args;
		fci.no_separation = 0;
		/*fci.function_handler_cache = &function_ptr;*/

		result = zend_call_function(&fci, NULL TSRMLS_CC);
		if (result == FAILURE) {
			zval **method;
			zval **obj;

			if (Z_TYPE_P(handler) == IS_STRING) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to call handler %s()", Z_STRVAL_P(handler));
			} else if (zend_hash_index_find(Z_ARRVAL_P(handler), 0, (void **) &obj) == SUCCESS &&
					   zend_hash_index_find(Z_ARRVAL_P(handler), 1, (void **) &method) == SUCCESS &&
					   Z_TYPE_PP(obj) == IS_OBJECT &&
					   Z_TYPE_PP(method) == IS_STRING) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to call handler %s::%s()", Z_OBJCE_PP(obj)->name, Z_STRVAL_PP(method));
			} else 
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to call handler");
		}

		for (i = 0; i < argc; i++) {
			zval_ptr_dtor(args[i]);
		}
		efree(args);

		if (result == FAILURE) {
			return NULL;
		} else {
			return EG(exception) ? NULL : retval;
		}
	} else {
		for (i = 0; i < argc; i++) {
			zval_ptr_dtor(&argv[i]);
		}
		return NULL;
	}
}
/* }}} */

/* {{{ xml_encode_iso_8859_1() */
inline static unsigned short xml_encode_iso_8859_1(unsigned char c)
{
	return (unsigned short)c;
}
/* }}} */

/* {{{ xml_decode_iso_8859_1() */
inline static char xml_decode_iso_8859_1(unsigned short c)
{
	return (char)(c > 0xff ? '?' : c);
}
/* }}} */

/* {{{ xml_encode_us_ascii() */
inline static unsigned short xml_encode_us_ascii(unsigned char c)
{
	return (unsigned short)c;
}
/* }}} */

/* {{{ xml_decode_us_ascii() */
inline static char xml_decode_us_ascii(unsigned short c)
{
	return (char)(c > 0x7f ? '?' : c);
}
/* }}} */

/* {{{ xml_get_encoding() */
static xml_encoding *xml_get_encoding(const XML_Char *name)
{
	xml_encoding *enc = &xml_encodings[0];

	while (enc && enc->name) {
		if (strcasecmp(name, enc->name) == 0) {
			return enc;
		}
		enc++;
	}
	return NULL;
}
/* }}} */

/* {{{ xml_utf8_encode */
PHPAPI char *xml_utf8_encode(const char *s, int len, int *newlen, const XML_Char *encoding)
{
	int pos = len;
	char *newbuf;
	unsigned int c;
	unsigned short (*encoder)(unsigned char) = NULL;
	xml_encoding *enc = xml_get_encoding(encoding);

	*newlen = 0;
	if (enc) {
		encoder = enc->encoding_function;
	} else {
		/* If the target encoding was unknown, fail */
		return NULL;
	}
	if (encoder == NULL) {
		/* If no encoder function was specified, return the data as-is.
		 */
		newbuf = emalloc(len + 1);
		memcpy(newbuf, s, len);
		*newlen = len;
		newbuf[*newlen] = '\0';
		return newbuf;
	}
	/* This is the theoretical max (will never get beyond len * 2 as long
	 * as we are converting from single-byte characters, though) */
	newbuf = safe_emalloc(len, 4, 1);
	while (pos > 0) {
		c = encoder ? encoder((unsigned char)(*s)) : (unsigned short)(*s);
		if (c < 0x80) {
			newbuf[(*newlen)++] = (char) c;
		} else if (c < 0x800) {
			newbuf[(*newlen)++] = (0xc0 | (c >> 6));
			newbuf[(*newlen)++] = (0x80 | (c & 0x3f));
		} else if (c < 0x10000) {
			newbuf[(*newlen)++] = (0xe0 | (c >> 12));
			newbuf[(*newlen)++] = (0xc0 | ((c >> 6) & 0x3f));
			newbuf[(*newlen)++] = (0x80 | (c & 0x3f));
		} else if (c < 0x200000) {
			newbuf[(*newlen)++] = (0xf0 | (c >> 18));
			newbuf[(*newlen)++] = (0xe0 | ((c >> 12) & 0x3f));
			newbuf[(*newlen)++] = (0xc0 | ((c >> 6) & 0x3f));
			newbuf[(*newlen)++] = (0x80 | (c & 0x3f));
		}
		pos--;
		s++;
	}
	newbuf[*newlen] = 0;
	newbuf = erealloc(newbuf, (*newlen)+1);
	return newbuf;
}
/* }}} */

/* {{{ xml_utf8_decode */
PHPAPI char *xml_utf8_decode(const XML_Char *s, int len, int *newlen, const XML_Char *encoding)
{
	int pos = len;
	char *newbuf = emalloc(len + 1);
	unsigned short c;
	char (*decoder)(unsigned short) = NULL;
	xml_encoding *enc = xml_get_encoding(encoding);

	*newlen = 0;
	if (enc) {
		decoder = enc->decoding_function;
	}
	if (decoder == NULL) {
		/* If the target encoding was unknown, or no decoder function
		 * was specified, return the UTF-8-encoded data as-is.
		 */
		memcpy(newbuf, s, len);
		*newlen = len;
		newbuf[*newlen] = '\0';
		return newbuf;
	}
	while (pos > 0) {
		c = (unsigned char)(*s);
		if (c >= 0xf0) { /* four bytes encoded, 21 bits */
			if(pos-4 >= 0) {
				c = ((s[0]&7)<<18) | ((s[1]&63)<<12) | ((s[2]&63)<<6) | (s[3]&63);
			} else {
				c = '?';	
			}
			s += 4;
			pos -= 4;
		} else if (c >= 0xe0) { /* three bytes encoded, 16 bits */
			if(pos-3 >= 0) {
				c = ((s[0]&63)<<12) | ((s[1]&63)<<6) | (s[2]&63);
			} else {
				c = '?';
			}
			s += 3;
			pos -= 3;
		} else if (c >= 0xc0) { /* two bytes encoded, 11 bits */
			if(pos-2 >= 0) {
				c = ((s[0]&63)<<6) | (s[1]&63);
			} else {
				c = '?';
			}
			s += 2;
			pos -= 2;
		} else {
			s++;
			pos--;
		}
		newbuf[*newlen] = decoder ? decoder(c) : c;
		++*newlen;
	}
	if (*newlen < len) {
		newbuf = erealloc(newbuf, *newlen + 1);
	}
	newbuf[*newlen] = '\0';
	return newbuf;
}
/* }}} */

/* {{{ _xml_xmlcharlen() */
static int _xml_xmlcharlen(const XML_Char *s)
{
	int len = 0;

	while (*s) {
		len++;
		s++;
	}
	return len;
}
/* }}} */

/* {{{ _xml_zval_strdup() */
PHPAPI char *_xml_zval_strdup(zval *val)
{
	if (Z_TYPE_P(val) == IS_STRING) {
		char *buf = emalloc(Z_STRLEN_P(val) + 1);
		memcpy(buf, Z_STRVAL_P(val), Z_STRLEN_P(val));
		buf[Z_STRLEN_P(val)] = '\0';
		return buf;
	}
	return NULL;
}
/* }}} */

/* {{{ _xml_add_to_info */
static void _xml_add_to_info(xml_parser *parser,char *name)
{
	zval **element, *values;

	if (! parser->info) {
		return;
	}

	if (zend_hash_find(Z_ARRVAL_P(parser->info),name,strlen(name) + 1,(void **) &element) == FAILURE) {
		MAKE_STD_ZVAL(values);
		
		array_init(values);
		
		zend_hash_update(Z_ARRVAL_P(parser->info), name, strlen(name)+1, (void *) &values, sizeof(zval*), (void **) &element);
	} 
			
	add_next_index_long(*element,parser->curtag);
	
	parser->curtag++;
}
/* }}} */

/* {{{ _xml_decode_tag() */
static char *_xml_decode_tag(xml_parser *parser, const char *tag)
{
	char *newstr;
	int out_len;

	newstr = xml_utf8_decode(tag, strlen(tag), &out_len, parser->target_encoding);

	if (parser->case_folding) {
		php_strtoupper(newstr, out_len);
	}

	return newstr;
}
/* }}} */

/* {{{ _xml_startElementHandler() */
void _xml_startElementHandler(void *userData, const XML_Char *name, const XML_Char **attributes)
{
	xml_parser *parser = (xml_parser *)userData;
	const char **attrs = (const char **) attributes;
	char *tag_name;
	char *att, *val;
	int val_len;
	zval *retval, *args[3];

	if (parser) {
		parser->level++;

		tag_name = _xml_decode_tag(parser, name);

		if (parser->startElementHandler) {
			args[0] = _xml_resource_zval(parser->index);
			args[1] = _xml_string_zval(tag_name);
			MAKE_STD_ZVAL(args[2]);
			array_init(args[2]);

			while (attributes && *attributes) {
				att = _xml_decode_tag(parser, attributes[0]);
				val = xml_utf8_decode(attributes[1], strlen(attributes[1]), &val_len, parser->target_encoding);

				add_assoc_stringl(args[2], att, val, val_len, 0);

				attributes += 2;

				efree(att);
			}
			
			if ((retval = xml_call_handler(parser, parser->startElementHandler, parser->startElementPtr, 3, args))) {
				zval_ptr_dtor(&retval);
			}
		} 

		if (parser->data) {
			zval *tag, *atr;
			int atcnt = 0;

			MAKE_STD_ZVAL(tag);
			MAKE_STD_ZVAL(atr);

			array_init(tag);
			array_init(atr);

			_xml_add_to_info(parser,((char *) tag_name) + parser->toffset);

			add_assoc_string(tag,"tag",((char *) tag_name) + parser->toffset,1); /* cast to avoid gcc-warning */
			add_assoc_string(tag,"type","open",1);
			add_assoc_long(tag,"level",parser->level);

			parser->ltags[parser->level-1] = estrdup(tag_name);
			parser->lastwasopen = 1;

			attributes = (const XML_Char **) attrs;

			while (attributes && *attributes) {
				att = _xml_decode_tag(parser, attributes[0]);
				val = xml_utf8_decode(attributes[1], strlen(attributes[1]), &val_len, parser->target_encoding);
				
				add_assoc_stringl(atr,att,val,val_len,0);

				atcnt++;
				attributes += 2;

				efree(att);
			}

			if (atcnt) {
				zend_hash_add(Z_ARRVAL_P(tag),"attributes",sizeof("attributes"),&atr,sizeof(zval*),NULL);
			} else {
				zval_ptr_dtor(&atr);
			}

			zend_hash_next_index_insert(Z_ARRVAL_P(parser->data),&tag,sizeof(zval*),(void *) &parser->ctag);
		}

		efree(tag_name);
	}
}
/* }}} */

/* {{{ _xml_endElementHandler() */
void _xml_endElementHandler(void *userData, const XML_Char *name)
{
	xml_parser *parser = (xml_parser *)userData;
	char *tag_name;

	if (parser) {
		zval *retval, *args[2];

		tag_name = _xml_decode_tag(parser, name);

		if (parser->endElementHandler) {
			args[0] = _xml_resource_zval(parser->index);
			args[1] = _xml_string_zval(tag_name);

			if ((retval = xml_call_handler(parser, parser->endElementHandler, parser->endElementPtr, 2, args))) {
				zval_ptr_dtor(&retval);
			}
		} 

		if (parser->data) {
			zval *tag;

			if (parser->lastwasopen) {
				add_assoc_string(*(parser->ctag),"type","complete",1);
			} else {
				MAKE_STD_ZVAL(tag);

				array_init(tag);
				  
				_xml_add_to_info(parser,((char *) tag_name) + parser->toffset);

				add_assoc_string(tag,"tag",((char *) tag_name) + parser->toffset,1); /* cast to avoid gcc-warning */
				add_assoc_string(tag,"type","close",1);
				add_assoc_long(tag,"level",parser->level);
				  
				zend_hash_next_index_insert(Z_ARRVAL_P(parser->data),&tag,sizeof(zval*),NULL);
			}

			parser->lastwasopen = 0;
		}

		efree(tag_name);

		if (parser->ltags) {
			efree(parser->ltags[parser->level-1]);
		}

		parser->level--;
	}
}
/* }}} */

/* {{{ _xml_characterDataHandler() */
void _xml_characterDataHandler(void *userData, const XML_Char *s, int len)
{
	xml_parser *parser = (xml_parser *)userData;

	if (parser) {
		zval *retval, *args[2];

		if (parser->characterDataHandler) {
			args[0] = _xml_resource_zval(parser->index);
			args[1] = _xml_xmlchar_zval(s, len, parser->target_encoding);
			if ((retval = xml_call_handler(parser, parser->characterDataHandler, parser->characterDataPtr, 2, args))) {
				zval_ptr_dtor(&retval);
			}
		} 

		if (parser->data) {
			int i;
			int doprint = 0;

			char *decoded_value;
			int decoded_len;
			
			decoded_value = xml_utf8_decode(s,len,&decoded_len,parser->target_encoding);
			for (i = 0; i < decoded_len; i++) {
				switch (decoded_value[i]) {
				case ' ':
				case '\t':
				case '\n':
					continue;
				default:
					doprint = 1;
					break;
				}
				if (doprint) {
					break;
				}
			}
			if (doprint || (! parser->skipwhite)) {
				if (parser->lastwasopen) {
					zval **myval;
					
					/* check if the current tag already has a value - if yes append to that! */
					if (zend_hash_find(Z_ARRVAL_PP(parser->ctag),"value",sizeof("value"),(void **) &myval) == SUCCESS) {
						int newlen = Z_STRLEN_PP(myval) + decoded_len;
						Z_STRVAL_PP(myval) = erealloc(Z_STRVAL_PP(myval),newlen+1);
						strcpy(Z_STRVAL_PP(myval) + Z_STRLEN_PP(myval),decoded_value);
						Z_STRLEN_PP(myval) += decoded_len;
						efree(decoded_value);
					} else {
						add_assoc_string(*(parser->ctag),"value",decoded_value,0);
					}
					
				} else {
					zval *tag;
					zval **curtag, **mytype, **myval;
					HashPosition hpos=NULL;

					zend_hash_internal_pointer_end_ex(Z_ARRVAL_P(parser->data), &hpos);

					if (hpos && (zend_hash_get_current_data_ex(Z_ARRVAL_P(parser->data), (void **) &curtag, &hpos) == SUCCESS)) {
						if (zend_hash_find(Z_ARRVAL_PP(curtag),"type",sizeof("type"),(void **) &mytype) == SUCCESS) {
							if (!strcmp(Z_STRVAL_PP(mytype), "cdata")) {
								if (zend_hash_find(Z_ARRVAL_PP(curtag),"value",sizeof("value"),(void **) &myval) == SUCCESS) {
									int newlen = Z_STRLEN_PP(myval) + decoded_len;
									Z_STRVAL_PP(myval) = erealloc(Z_STRVAL_PP(myval),newlen+1);
									strcpy(Z_STRVAL_PP(myval) + Z_STRLEN_PP(myval),decoded_value);
									Z_STRLEN_PP(myval) += decoded_len;
									efree(decoded_value);
									return;
								}
							}
						}
					}

					MAKE_STD_ZVAL(tag);
					
					array_init(tag);
					
					_xml_add_to_info(parser,parser->ltags[parser->level-1] + parser->toffset);

					add_assoc_string(tag,"tag",parser->ltags[parser->level-1] + parser->toffset,1);
					add_assoc_string(tag,"value",decoded_value,0);
					add_assoc_string(tag,"type","cdata",1);
					add_assoc_long(tag,"level",parser->level);

					zend_hash_next_index_insert(Z_ARRVAL_P(parser->data),&tag,sizeof(zval*),NULL);
				}
			} else {
				efree(decoded_value);
			}
		}
	}
}
/* }}} */

/* {{{ _xml_processingInstructionHandler() */
void _xml_processingInstructionHandler(void *userData, const XML_Char *target, const XML_Char *data)
{
	xml_parser *parser = (xml_parser *)userData;

	if (parser && parser->processingInstructionHandler) {
		zval *retval, *args[3];

		args[0] = _xml_resource_zval(parser->index);
		args[1] = _xml_xmlchar_zval(target, 0, parser->target_encoding);
		args[2] = _xml_xmlchar_zval(data, 0, parser->target_encoding);
		if ((retval = xml_call_handler(parser, parser->processingInstructionHandler, parser->processingInstructionPtr, 3, args))) {
			zval_ptr_dtor(&retval);
		}
	}
}
/* }}} */

/* {{{ _xml_defaultHandler() */
void _xml_defaultHandler(void *userData, const XML_Char *s, int len)
{
	xml_parser *parser = (xml_parser *)userData;

	if (parser && parser->defaultHandler) {
		zval *retval, *args[2];

		args[0] = _xml_resource_zval(parser->index);
		args[1] = _xml_xmlchar_zval(s, len, parser->target_encoding);
		if ((retval = xml_call_handler(parser, parser->defaultHandler, parser->defaultPtr, 2, args))) {
			zval_ptr_dtor(&retval);
		}
	}
}
/* }}} */

/* {{{ _xml_unparsedEntityDeclHandler() */
void _xml_unparsedEntityDeclHandler(void *userData, 
										 const XML_Char *entityName, 
										 const XML_Char *base,
										 const XML_Char *systemId,
										 const XML_Char *publicId,
										 const XML_Char *notationName)
{
	xml_parser *parser = (xml_parser *)userData;

	if (parser && parser->unparsedEntityDeclHandler) {
		zval *retval, *args[6];

		args[0] = _xml_resource_zval(parser->index);
		args[1] = _xml_xmlchar_zval(entityName, 0, parser->target_encoding);
		args[2] = _xml_xmlchar_zval(base, 0, parser->target_encoding);
		args[3] = _xml_xmlchar_zval(systemId, 0, parser->target_encoding);
		args[4] = _xml_xmlchar_zval(publicId, 0, parser->target_encoding);
		args[5] = _xml_xmlchar_zval(notationName, 0, parser->target_encoding);
		if ((retval = xml_call_handler(parser, parser->unparsedEntityDeclHandler, parser->unparsedEntityDeclPtr, 6, args))) {
			zval_ptr_dtor(&retval);
		}
	}
}
/* }}} */

/* {{{ _xml_notationDeclHandler() */
void _xml_notationDeclHandler(void *userData,
							  const XML_Char *notationName,
							  const XML_Char *base,
							  const XML_Char *systemId,
							  const XML_Char *publicId)
{
	xml_parser *parser = (xml_parser *)userData;

	if (parser && parser->notationDeclHandler) {
		zval *retval, *args[5];

		args[0] = _xml_resource_zval(parser->index);
		args[1] = _xml_xmlchar_zval(notationName, 0, parser->target_encoding);
		args[2] = _xml_xmlchar_zval(base, 0, parser->target_encoding);
		args[3] = _xml_xmlchar_zval(systemId, 0, parser->target_encoding);
		args[4] = _xml_xmlchar_zval(publicId, 0, parser->target_encoding);
		if ((retval = xml_call_handler(parser, parser->notationDeclHandler, parser->notationDeclPtr, 5, args))) {
			zval_ptr_dtor(&retval);
		}
	}
}
/* }}} */

/* {{{ _xml_externalEntityRefHandler() */
int _xml_externalEntityRefHandler(XML_Parser parserPtr,
								   const XML_Char *openEntityNames,
								   const XML_Char *base,
								   const XML_Char *systemId,
								   const XML_Char *publicId)
{
	xml_parser *parser = XML_GetUserData(parserPtr);
	int ret = 0; /* abort if no handler is set (should be configurable?) */

	if (parser && parser->externalEntityRefHandler) {
		zval *retval, *args[5];

		args[0] = _xml_resource_zval(parser->index);
		args[1] = _xml_xmlchar_zval(openEntityNames, 0, parser->target_encoding);
		args[2] = _xml_xmlchar_zval(base, 0, parser->target_encoding);
		args[3] = _xml_xmlchar_zval(systemId, 0, parser->target_encoding);
		args[4] = _xml_xmlchar_zval(publicId, 0, parser->target_encoding);
		if ((retval = xml_call_handler(parser, parser->externalEntityRefHandler, parser->externalEntityRefPtr, 5, args))) {
			convert_to_long(retval);
			ret = Z_LVAL_P(retval);
			efree(retval);
		} else {
			ret = 0;
		}
	}
	return ret;
}
/* }}} */

/* {{{ _xml_startNamespaceDeclHandler() */
void _xml_startNamespaceDeclHandler(void *userData,const XML_Char *prefix, const XML_Char *uri)
{
	xml_parser *parser = (xml_parser *)userData;

	if (parser && parser->startNamespaceDeclHandler) {
		zval *retval, *args[3];

		args[0] = _xml_resource_zval(parser->index);
		args[1] = _xml_xmlchar_zval(prefix, 0, parser->target_encoding);
		args[2] = _xml_xmlchar_zval(uri, 0, parser->target_encoding);
		if ((retval = xml_call_handler(parser, parser->startNamespaceDeclHandler, parser->startNamespaceDeclPtr, 3, args))) {
			zval_ptr_dtor(&retval);
		}
	}
}
/* }}} */

/* {{{ _xml_endNamespaceDeclHandler() */
void _xml_endNamespaceDeclHandler(void *userData, const XML_Char *prefix)
{
	xml_parser *parser = (xml_parser *)userData;

	if (parser && parser->endNamespaceDeclHandler) {
		zval *retval, *args[2];

		args[0] = _xml_resource_zval(parser->index);
		args[1] = _xml_xmlchar_zval(prefix, 0, parser->target_encoding);
		if ((retval = xml_call_handler(parser, parser->endNamespaceDeclHandler, parser->endNamespaceDeclPtr, 2, args))) {
			zval_ptr_dtor(&retval);
		}
	}
}
/* }}} */

/************************* EXTENSION FUNCTIONS *************************/

static void php_xml_parser_create_impl(INTERNAL_FUNCTION_PARAMETERS, int ns_support)
{
	xml_parser *parser;
	int auto_detect = 0;

	char *encoding_param = NULL;
	int encoding_param_len = 0;

	char *ns_param = NULL;
	int ns_param_len = 0;
	
	XML_Char *encoding;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, (ns_support ? "|ss": "|s"), &encoding_param, &encoding_param_len, &ns_param, &ns_param_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (encoding_param != NULL) {
		/* The supported encoding types are hardcoded here because
		 * we are limited to the encodings supported by expat/xmltok.
		 */
		if (encoding_param_len == 0) {
			encoding = XML(default_encoding);
			auto_detect = 1;
		} else if (strcasecmp(encoding_param, "ISO-8859-1") == 0) {
			encoding = "ISO-8859-1";
		} else if (strcasecmp(encoding_param, "UTF-8") == 0) {
			encoding = "UTF-8";
		} else if (strcasecmp(encoding_param, "US-ASCII") == 0) {
			encoding = "US-ASCII";
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "unsupported source encoding \"%s\"", encoding_param);
			RETURN_FALSE;
		}
	} else {
		encoding = XML(default_encoding);
	}

	if (ns_support && ns_param == NULL){
		ns_param = ":";
	}

	parser = ecalloc(1, sizeof(xml_parser));
	parser->parser = XML_ParserCreate_MM((auto_detect ? NULL : encoding),
                                         &php_xml_mem_hdlrs, ns_param);

	parser->target_encoding = encoding;
	parser->case_folding = 1;
	parser->object = NULL;
	parser->isparsing = 0;

	XML_SetUserData(parser->parser, parser);

	ZEND_REGISTER_RESOURCE(return_value, parser,le_xml_parser);
	parser->index = Z_LVAL_P(return_value);
}

/* {{{ proto resource xml_parser_create([string encoding]) 
   Create an XML parser */
PHP_FUNCTION(xml_parser_create)
{
	php_xml_parser_create_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);	
}
/* }}} */

/* {{{ proto resource xml_parser_create_ns([string encoding [, string sep]]) 
   Create an XML parser */
PHP_FUNCTION(xml_parser_create_ns)
{
	php_xml_parser_create_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto int xml_set_object(resource parser, object &obj) 
   Set up object which should be used for callbacks */
PHP_FUNCTION(xml_set_object)
{
	xml_parser *parser;
	zval **pind, **mythis;

	if (ZEND_NUM_ARGS() != 2 ||
		zend_get_parameters_ex(2, &pind, &mythis) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (Z_TYPE_PP(mythis) != IS_OBJECT) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument 2 has wrong type");
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(parser,xml_parser *,pind, -1, "XML Parser", le_xml_parser);

	/* please leave this commented - or ask thies@thieso.net before doing it (again) */
	if (parser->object) {
		zval_ptr_dtor(&parser->object);
	}

	/* please leave this commented - or ask thies@thieso.net before doing it (again) */
/* #ifdef ZEND_ENGINE_2
	zval_add_ref(&parser->object); 
#endif */

	ALLOC_ZVAL(parser->object);
	*parser->object = **mythis;
	zval_copy_ctor(parser->object);
	INIT_PZVAL(parser->object);

	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int xml_set_element_handler(resource parser, string shdl, string ehdl) 
   Set up start and end element handlers */
PHP_FUNCTION(xml_set_element_handler)
{
	xml_parser *parser;
	zval **pind, **shdl, **ehdl;

	if (ZEND_NUM_ARGS() != 3 ||
		zend_get_parameters_ex(3, &pind, &shdl, &ehdl) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(parser,xml_parser *,pind, -1, "XML Parser", le_xml_parser);

	xml_set_handler(&parser->startElementHandler, shdl);
	xml_set_handler(&parser->endElementHandler, ehdl);
	XML_SetElementHandler(parser->parser, _xml_startElementHandler, _xml_endElementHandler);
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int xml_set_character_data_handler(resource parser, string hdl) 
   Set up character data handler */
PHP_FUNCTION(xml_set_character_data_handler)
{
	xml_parser *parser;
	zval **pind, **hdl;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &pind, &hdl) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(parser,xml_parser *, pind, -1, "XML Parser", le_xml_parser);

	xml_set_handler(&parser->characterDataHandler, hdl);
	XML_SetCharacterDataHandler(parser->parser, _xml_characterDataHandler);
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int xml_set_processing_instruction_handler(resource parser, string hdl) 
   Set up processing instruction (PI) handler */
PHP_FUNCTION(xml_set_processing_instruction_handler)
{
	xml_parser *parser;
	zval **pind, **hdl;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &pind, &hdl) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(parser,xml_parser *, pind, -1, "XML Parser", le_xml_parser);

	xml_set_handler(&parser->processingInstructionHandler, hdl);
	XML_SetProcessingInstructionHandler(parser->parser, _xml_processingInstructionHandler);
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int xml_set_default_handler(resource parser, string hdl) 
   Set up default handler */
PHP_FUNCTION(xml_set_default_handler)
{
	xml_parser *parser;
	zval **pind, **hdl;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &pind, &hdl) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(parser,xml_parser *, pind, -1, "XML Parser", le_xml_parser);

	xml_set_handler(&parser->defaultHandler, hdl);
	XML_SetDefaultHandler(parser->parser, _xml_defaultHandler);
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int xml_set_unparsed_entity_decl_handler(resource parser, string hdl) 
   Set up unparsed entity declaration handler */
PHP_FUNCTION(xml_set_unparsed_entity_decl_handler)
{
	xml_parser *parser;
	zval **pind, **hdl;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &pind, &hdl) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(parser,xml_parser *, pind, -1, "XML Parser", le_xml_parser);

	xml_set_handler(&parser->unparsedEntityDeclHandler, hdl);
	XML_SetUnparsedEntityDeclHandler(parser->parser, _xml_unparsedEntityDeclHandler);
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int xml_set_notation_decl_handler(resource parser, string hdl) 
   Set up notation declaration handler */
PHP_FUNCTION(xml_set_notation_decl_handler)
{
	xml_parser *parser;
	zval **pind, **hdl;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &pind, &hdl) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(parser,xml_parser *, pind, -1, "XML Parser", le_xml_parser);

	xml_set_handler(&parser->notationDeclHandler, hdl);
	XML_SetNotationDeclHandler(parser->parser, _xml_notationDeclHandler);
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int xml_set_external_entity_ref_handler(resource parser, string hdl) 
   Set up external entity reference handler */
PHP_FUNCTION(xml_set_external_entity_ref_handler)
{
	xml_parser *parser;
	zval **pind, **hdl;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &pind, &hdl) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(parser,xml_parser *, pind, -1, "XML Parser", le_xml_parser);

	xml_set_handler(&parser->externalEntityRefHandler, hdl);
	XML_SetExternalEntityRefHandler(parser->parser, (void *) _xml_externalEntityRefHandler);
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int xml_set_start_namespace_decl_handler(resource parser, string hdl) 
   Set up character data handler */
PHP_FUNCTION(xml_set_start_namespace_decl_handler)
{
	xml_parser *parser;
	zval **pind, **hdl;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &pind, &hdl) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(parser,xml_parser *, pind, -1, "XML Parser", le_xml_parser);

	xml_set_handler(&parser->startNamespaceDeclHandler, hdl);
	XML_SetStartNamespaceDeclHandler(parser->parser, _xml_startNamespaceDeclHandler);
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int xml_set_end_namespace_decl_handler(resource parser, string hdl) 
   Set up character data handler */
PHP_FUNCTION(xml_set_end_namespace_decl_handler)
{
	xml_parser *parser;
	zval **pind, **hdl;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &pind, &hdl) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(parser,xml_parser *, pind, -1, "XML Parser", le_xml_parser);

	xml_set_handler(&parser->endNamespaceDeclHandler, hdl);
	XML_SetEndNamespaceDeclHandler(parser->parser, _xml_endNamespaceDeclHandler);
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int xml_parse(resource parser, string data [, int isFinal]) 
   Start parsing an XML document */
PHP_FUNCTION(xml_parse)
{
	xml_parser *parser;
	zval **pind, **data, **final;
	int argc, isFinal, ret;

	argc = ZEND_NUM_ARGS();
	if (argc < 2 || argc > 3 || zend_get_parameters_ex(argc, &pind, &data, &final) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(parser,xml_parser *, pind, -1, "XML Parser", le_xml_parser);

	convert_to_string_ex(data);

	if (argc == 3) {
		convert_to_long_ex(final);
		isFinal = Z_LVAL_PP(final);
	} else {
		isFinal = 0;
	}

	parser->isparsing = 1;
	ret = XML_Parse(parser->parser, Z_STRVAL_PP(data), Z_STRLEN_PP(data), isFinal);
	parser->isparsing = 0;
	RETVAL_LONG(ret);
}

/* }}} */

/* {{{ proto int xml_parse_into_struct(resource parser, string data, array &struct, array &index) 
   Parsing a XML document */

PHP_FUNCTION(xml_parse_into_struct)
{
	xml_parser *parser;
	zval **pind, **data, **xdata, **info = 0;
	int ret;

	if (zend_get_parameters_ex(4, &pind, &data, &xdata,&info) == SUCCESS) {
		zval_dtor(*info);
		array_init(*info);
	} else if (zend_get_parameters_ex(3, &pind, &data, &xdata) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(parser,xml_parser *, pind, -1, "XML Parser", le_xml_parser);

	convert_to_string_ex(data);
	zval_dtor(*xdata);
	array_init(*xdata);

	parser->data = *xdata;
	if (info)
	  parser->info = *info;
	parser->level = 0;
	parser->ltags = safe_emalloc(XML_MAXLEVEL, sizeof(char *), 0);

	XML_SetDefaultHandler(parser->parser, _xml_defaultHandler);
	XML_SetElementHandler(parser->parser, _xml_startElementHandler, _xml_endElementHandler);
	XML_SetCharacterDataHandler(parser->parser, _xml_characterDataHandler);

	parser->isparsing = 1;
	ret = XML_Parse(parser->parser, Z_STRVAL_PP(data), Z_STRLEN_PP(data), 1);
	parser->isparsing = 0;

	RETVAL_LONG(ret);
}
/* }}} */

/* {{{ proto int xml_get_error_code(resource parser) 
   Get XML parser error code */
PHP_FUNCTION(xml_get_error_code)
{
	xml_parser *parser;
	zval **pind;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &pind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(parser,xml_parser *, pind, -1, "XML Parser", le_xml_parser);

	RETVAL_LONG((long)XML_GetErrorCode(parser->parser));
}
/* }}} */

/* {{{ proto string xml_error_string(int code)
   Get XML parser error string */
PHP_FUNCTION(xml_error_string)
{
	zval **code;
	char *str;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &code) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(code);
	str = (char *)XML_ErrorString((int)Z_LVAL_PP(code));
	if (str) {
		RETVAL_STRING(str, 1);
	}
}
/* }}} */

/* {{{ proto int xml_get_current_line_number(resource parser) 
   Get current line number for an XML parser */
PHP_FUNCTION(xml_get_current_line_number)
{
	xml_parser *parser;
	zval **pind;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &pind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(parser,xml_parser *, pind, -1, "XML Parser", le_xml_parser);

	RETVAL_LONG(XML_GetCurrentLineNumber(parser->parser));
}
/* }}} */

/* {{{ proto int xml_get_current_column_number(resource parser)
   Get current column number for an XML parser */
PHP_FUNCTION(xml_get_current_column_number)
{
	xml_parser *parser;
	zval **pind;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &pind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(parser,xml_parser *, pind, -1, "XML Parser", le_xml_parser);

	RETVAL_LONG(XML_GetCurrentColumnNumber(parser->parser));
}
/* }}} */

/* {{{ proto int xml_get_current_byte_index(resource parser) 
   Get current byte index for an XML parser */
PHP_FUNCTION(xml_get_current_byte_index)
{
	xml_parser *parser;
	zval **pind;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &pind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(parser,xml_parser *, pind, -1, "XML Parser", le_xml_parser);

	RETVAL_LONG(XML_GetCurrentByteIndex(parser->parser));
}
/* }}} */

/* {{{ proto int xml_parser_free(resource parser) 
   Free an XML parser */
PHP_FUNCTION(xml_parser_free)
{
	zval **pind;
	xml_parser *parser;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &pind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(parser,xml_parser *, pind, -1, "XML Parser", le_xml_parser);

	if (parser->isparsing == 1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Parser cannot be freed while it is parsing.");
		RETURN_FALSE;
	}

	if (zend_list_delete(parser->index) == FAILURE) {
		RETURN_FALSE;
	}

	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int xml_parser_set_option(resource parser, int option, mixed value) 
   Set options in an XML parser */
PHP_FUNCTION(xml_parser_set_option)
{
	xml_parser *parser;
	zval **pind, **opt, **val;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &pind, &opt, &val) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(parser,xml_parser *, pind, -1, "XML Parser", le_xml_parser);

	convert_to_long_ex(opt);

	switch (Z_LVAL_PP(opt)) {
		case PHP_XML_OPTION_CASE_FOLDING:
			convert_to_long_ex(val);
			parser->case_folding = Z_LVAL_PP(val);
			break;
		case PHP_XML_OPTION_SKIP_TAGSTART:
			convert_to_long_ex(val);
			parser->toffset = Z_LVAL_PP(val);
			break;
		case PHP_XML_OPTION_SKIP_WHITE:
			convert_to_long_ex(val);
			parser->skipwhite = Z_LVAL_PP(val);
			break;
		case PHP_XML_OPTION_TARGET_ENCODING: {
			xml_encoding *enc;
			convert_to_string_ex(val);
			enc = xml_get_encoding(Z_STRVAL_PP(val));
			if (enc == NULL) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unsupported target encoding \"%s\"", Z_STRVAL_PP(val));
				RETURN_FALSE;
			}
			parser->target_encoding = enc->name;
			break;
		}
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown option");
			RETURN_FALSE;
			break;
	}
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int xml_parser_get_option(resource parser, int option) 
   Get options from an XML parser */
PHP_FUNCTION(xml_parser_get_option)
{
	xml_parser *parser;
	zval **pind, **opt;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &pind, &opt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(parser,xml_parser *, pind, -1, "XML Parser", le_xml_parser);

	convert_to_long_ex(opt);

	switch (Z_LVAL_PP(opt)) {
		case PHP_XML_OPTION_CASE_FOLDING:
			RETURN_LONG(parser->case_folding);
			break;
		case PHP_XML_OPTION_TARGET_ENCODING:
			RETURN_STRING(parser->target_encoding, 1);
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown option");
			RETURN_FALSE;
			break;
	}

	RETVAL_FALSE;	/* never reached */
}
/* }}} */

/* {{{ proto string utf8_encode(string data) 
   Encodes an ISO-8859-1 string to UTF-8 */
PHP_FUNCTION(utf8_encode)
{
	zval **arg;
	XML_Char *encoded;
	int len;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg);
	encoded = xml_utf8_encode(Z_STRVAL_PP(arg), Z_STRLEN_PP(arg), &len, "ISO-8859-1");
	if (encoded == NULL) {
		RETURN_FALSE;
	}
	RETVAL_STRINGL(encoded, len, 0);
}
/* }}} */

/* {{{ proto string utf8_decode(string data) 
   Converts a UTF-8 encoded string to ISO-8859-1 */
PHP_FUNCTION(utf8_decode)
{
	zval **arg;
	XML_Char *decoded;
	int len;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg);
	decoded = xml_utf8_decode(Z_STRVAL_PP(arg), Z_STRLEN_PP(arg), &len, "ISO-8859-1");
	if (decoded == NULL) {
		RETURN_FALSE;
	}
	RETVAL_STRINGL(decoded, len, 0);
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
