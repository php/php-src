/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stig Sæther Bakken <ssb@fast.no>                            |
   |          Thies C. Arntzen <thies@thieso.net>                         | 
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#define IS_EXT_MODULE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#define PHP_XML_INTERNAL
#include "php_xml.h"
#include "zend_variables.h"
#include "ext/standard/php_string.h"
#include "ext/standard/info.h"

#if HAVE_LIBEXPAT

# include "ext/standard/head.h"

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

#ifdef ZTS
int xml_globals_id;
#else
PHP_XML_API php_xml_globals xml_globals;
#endif

/* {{{ dynamically loadable module stuff */
#ifdef COMPILE_DL_XML
ZEND_GET_MODULE(xml)
#endif /* COMPILE_DL_XML */
/* }}} */

/* {{{ function prototypes */
PHP_MINIT_FUNCTION(xml);
PHP_RINIT_FUNCTION(xml);
PHP_MSHUTDOWN_FUNCTION(xml);
PHP_RSHUTDOWN_FUNCTION(xml);
PHP_MINFO_FUNCTION(xml);

static void xml_parser_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC);
static void xml_set_handler(zval **, zval **);
inline static unsigned short xml_encode_iso_8859_1(unsigned char);
inline static char xml_decode_iso_8859_1(unsigned short);
inline static unsigned short xml_encode_us_ascii(unsigned char);
inline static char xml_decode_us_ascii(unsigned short);
static XML_Char *xml_utf8_encode(const char *, int, int *, const XML_Char *);
static zval *xml_call_handler(xml_parser *, zval *, int, zval **);
static zval *_xml_xmlchar_zval(const XML_Char *, int, const XML_Char *);
static int _xml_xmlcharlen(const XML_Char *);
static void _xml_add_to_info(xml_parser *parser,char *name);
inline static char *_xml_decode_tag(xml_parser *parser, const char *tag);

void _xml_startElementHandler(void *, const char *, const char **);
void _xml_endElementHandler(void *, const char *);
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
static unsigned char third_and_fourth_args_force_ref[] = { 4, BYREF_NONE, BYREF_NONE, BYREF_FORCE, BYREF_FORCE };

function_entry xml_functions[] = {
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

zend_module_entry xml_module_entry = {
    STANDARD_MODULE_HEADER,
	"xml",                /* extension name */
	xml_functions,        /* extension function list */
	PHP_MINIT(xml),       /* extension-wide startup function */
	PHP_MSHUTDOWN(xml),   /* extension-wide shutdown function */
	PHP_RINIT(xml),       /* per-request startup function */
	PHP_RSHUTDOWN(xml),   /* per-request shutdown function */
	PHP_MINFO(xml),       /* information function */
    NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
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

/* True globals, no need for thread safety */
static int le_xml_parser; 

/* }}} */

/* {{{ startup, shutdown and info functions */
#ifdef ZTS
static void php_xml_init_globals(php_xml_globals *xml_globals_p TSRMLS_DC)
{
	XML(default_encoding) = "ISO-8859-1";
}
#endif

PHP_MINIT_FUNCTION(xml)
{
	le_xml_parser =	zend_register_list_destructors_ex(xml_parser_dtor, NULL, "xml", module_number);

#ifdef ZTS
	ts_allocate_id(&xml_globals_id, sizeof(php_xml_globals), (ts_allocate_ctor) php_xml_init_globals, NULL);
#else
	XML(default_encoding) = "ISO-8859-1";
#endif

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
	
	return SUCCESS;
}


PHP_RINIT_FUNCTION(xml)
{
    return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(xml)
{
	return SUCCESS;
}


PHP_RSHUTDOWN_FUNCTION(xml)
{
    return SUCCESS;
}


PHP_MINFO_FUNCTION(xml)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "XML Support", "active");
	php_info_print_table_row(2, "XML Namespace Support", "active");
	php_info_print_table_row(2, "EXPAT Version",XML_ExpatVersion());
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

	/* please leave this commented - or ask thies@thieso.net before doing it (again) */
#ifdef ZEND_ENGINE_2
	if (parser->object) {
		zval_ptr_dtor(&parser->object);
	}
#endif
	
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

	efree(parser);
}
/* }}} */

/* {{{ xml_set_handler() */
static void xml_set_handler(zval **handler, zval **data)
{
	/* IS_ARRAY might indicate that we're using array($obj, 'method') syntax */
	if (Z_TYPE_PP(data) != IS_ARRAY) {
		convert_to_string_ex(data);
	}

	/* If we have already a handler, release it */
	if (*handler) {
		zval_ptr_dtor(handler);
	}

	zval_add_ref(data);
	*handler = *data;
}
/* }}} */

/* {{{ xml_call_handler() */
static zval *xml_call_handler(xml_parser *parser, zval *handler, int argc, zval **argv)
{
	TSRMLS_FETCH();

	if (parser && handler) {
		zval *retval;
		int i;	
		int result;

		MAKE_STD_ZVAL(retval);
		ZVAL_FALSE(retval);

		result = call_user_function(EG(function_table), &parser->object, handler, retval, argc, argv TSRMLS_CC);

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

			zval_dtor(retval);
			efree(retval);
		}

		for (i = 0; i < argc; i++) {
			zval_ptr_dtor(&(argv[i]));
		}

		if (result == FAILURE) {
			return NULL;
		} else {
			return retval;
		}
	}
	return NULL;
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
static XML_Char *xml_utf8_encode(const char *s, int len, int *newlen, const XML_Char *encoding)
{
	int pos = len;
	char *newbuf;
	unsigned short c;
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
	newbuf = emalloc(len * 4 + 1);
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
			c = ((s[0]&7)<<18) | ((s[1]&63)<<12) | ((s[2]&63)<<6) | (s[3]&63);
			s += 4;
			pos -= 4;
		} else if (c >= 0xe0) { /* three bytes encoded, 16 bits */
			c = ((s[0]&63)<<12) | ((s[1]&63)<<6) | (s[2]&63);
			s += 3;
			pos -= 3;
		} else if (c >= 0xc0) { /* two bytes encoded, 11 bits */
			c = ((s[0]&63)<<6) | (s[1]&63);
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
		
		if (array_init(values) == FAILURE) {
			TSRMLS_FETCH();

			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to initialize array");
			return;
		}
		
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
void _xml_startElementHandler(void *userData, const char *name, const char **attributes)
{
	xml_parser *parser = (xml_parser *)userData;
	const char **attrs = attributes;
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
			
			if ((retval = xml_call_handler(parser, parser->startElementHandler, 3, args))) {
				zval_dtor(retval);
				efree(retval);
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

			attributes = attrs;

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
				zval_dtor(atr);
				FREE_ZVAL(atr);
			}

			zend_hash_next_index_insert(Z_ARRVAL_P(parser->data),&tag,sizeof(zval*),(void *) &parser->ctag);
		}

		efree(tag_name);
	}
}
/* }}} */

/* {{{ _xml_endElementHandler() */
void _xml_endElementHandler(void *userData, const char *name)
{
	xml_parser *parser = (xml_parser *)userData;
	char *tag_name;

	if (parser) {
		zval *retval, *args[2];

		tag_name = _xml_decode_tag(parser, name);

		if (parser->endElementHandler) {
			args[0] = _xml_resource_zval(parser->index);
			args[1] = _xml_string_zval(tag_name);

			if ((retval = xml_call_handler(parser, parser->endElementHandler, 2, args))) {
				zval_dtor(retval);
				efree(retval);
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
			if ((retval = xml_call_handler(parser, parser->characterDataHandler, 2, args))) {
				zval_dtor(retval);
				efree(retval);
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
		if ((retval = xml_call_handler(parser, parser->processingInstructionHandler, 3, args))) {
			zval_dtor(retval);
			efree(retval);
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
		if ((retval = xml_call_handler(parser, parser->defaultHandler, 2, args))) {
			zval_dtor(retval);
			efree(retval);
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
		if ((retval = xml_call_handler(parser, parser->unparsedEntityDeclHandler, 6, args))) {
			zval_dtor(retval);
			efree(retval);
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
		if ((retval = xml_call_handler(parser, parser->notationDeclHandler, 5, args))) {
			zval_dtor(retval);
			efree(retval);
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
		if ((retval = xml_call_handler(parser, parser->externalEntityRefHandler, 5, args))) {
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
		if ((retval = xml_call_handler(parser, parser->startNamespaceDeclHandler, 3, args))) {
			zval_dtor(retval);
			efree(retval);
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
		if ((retval = xml_call_handler(parser, parser->endNamespaceDeclHandler, 2, args))) {
			zval_dtor(retval);
			efree(retval);
		}
	}
}
/* }}} */

/************************* EXTENSION FUNCTIONS *************************/

/* {{{ proto resource xml_parser_create([string encoding]) 
   Create an XML parser */
PHP_FUNCTION(xml_parser_create)
{
	xml_parser *parser;
	int argc;
	zval **encodingArg;
	XML_Char *encoding;
	
	argc = ZEND_NUM_ARGS();

	if (argc > 1 || zend_get_parameters_ex(argc, &encodingArg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (argc == 1) {
		convert_to_string_ex(encodingArg);
		/* The supported encoding types are hardcoded here because
		 * we are limited to the encodings supported by expat/xmltok.
		 */
		if (strncasecmp(Z_STRVAL_PP(encodingArg), "ISO-8859-1",
						Z_STRLEN_PP(encodingArg)) == 0) {
			encoding = "ISO-8859-1";
		} else if (strncasecmp(Z_STRVAL_PP(encodingArg), "UTF-8",
						Z_STRLEN_PP(encodingArg)) == 0) {
			encoding = "UTF-8";
		} else if (strncasecmp(Z_STRVAL_PP(encodingArg), "US-ASCII",
						Z_STRLEN_PP(encodingArg)) == 0) {
			encoding = "US-ASCII";
		} else { /* UTF-16 not supported */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s: unsupported source encoding \"%s\"", Z_STRVAL_PP(encodingArg));
			RETURN_FALSE;
		}
	} else {
		encoding = XML(default_encoding);
	}

	parser = ecalloc(sizeof(xml_parser), 1);
	parser->parser = XML_ParserCreate(encoding);
	parser->target_encoding = encoding;
	parser->case_folding = 1;
	parser->object = NULL;
	XML_SetUserData(parser->parser, parser);

	ZEND_REGISTER_RESOURCE(return_value,parser,le_xml_parser);
	parser->index = Z_LVAL_P(return_value);
}
/* }}} */

/* {{{ proto resource xml_parser_create_ns([string encoding [, string sep]]) 
   Create an XML parser */
PHP_FUNCTION(xml_parser_create_ns)
{
	xml_parser *parser;
	int argc;
	zval **encodingArg, **sepArg;
	XML_Char *encoding, *sep;
	
	argc = ZEND_NUM_ARGS();

	if (argc > 2 || zend_get_parameters_ex(argc, &encodingArg, &sepArg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (argc >= 1) {
		convert_to_string_ex(encodingArg);
		/* The supported encoding types are hardcoded here because
		 * we are limited to the encodings supported by expat/xmltok.
		 */
		if (strncasecmp(Z_STRVAL_PP(encodingArg), "ISO-8859-1",
						Z_STRLEN_PP(encodingArg)) == 0) {
			encoding = "ISO-8859-1";
		} else if (strncasecmp(Z_STRVAL_PP(encodingArg), "UTF-8",
						Z_STRLEN_PP(encodingArg)) == 0) {
			encoding = "UTF-8";
		} else if (strncasecmp(Z_STRVAL_PP(encodingArg), "US-ASCII",
						Z_STRLEN_PP(encodingArg)) == 0) {
			encoding = "US-ASCII";
		} else { /* UTF-16 not supported */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s: unsupported source encoding \"%s\"", Z_STRVAL_PP(encodingArg));
			RETURN_FALSE;
		}
	} else {
		encoding = XML(default_encoding);
	}

	if (argc == 2){
		convert_to_string_ex(sepArg);
		sep = Z_STRVAL_PP(sepArg);
	} else {
		sep = ":";
	}

	parser = ecalloc(sizeof(xml_parser), 1);
	parser->parser = XML_ParserCreateNS(encoding, sep[0]);
	parser->target_encoding = encoding;
	parser->case_folding = 1;
	parser->object = NULL;
	XML_SetUserData(parser->parser, parser);

	ZEND_REGISTER_RESOURCE(return_value,parser,le_xml_parser);
	parser->index = Z_LVAL_P(return_value);
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
#ifdef ZEND_ENGINE_2
	if (parser->object) {
		zval_ptr_dtor(&parser->object);
	}
#endif
	
	parser->object = *mythis;

	/* please leave this commented - or ask thies@thieso.net before doing it (again) */
#ifdef ZEND_ENGINE_2
	zval_add_ref(&parser->object); 
#endif

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
	XML_SetExternalEntityRefHandler(parser->parser, _xml_externalEntityRefHandler);
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

	ret = XML_Parse(parser->parser, Z_STRVAL_PP(data), Z_STRLEN_PP(data), isFinal);
	RETVAL_LONG(ret);
}

/* }}} */

/* {{{ proto int xml_parse_into_struct(resource parser, string data, array &struct, array &index) 
   Parsing a XML document */

PHP_FUNCTION(xml_parse_into_struct)
{
	xml_parser *parser;
	zval **pind, **data, **xdata, **info = 0;
	int argc, ret;

	argc = ZEND_NUM_ARGS();
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
	parser->ltags = emalloc(XML_MAXLEVEL * sizeof(char *));

	XML_SetDefaultHandler(parser->parser, _xml_defaultHandler);
	XML_SetElementHandler(parser->parser, _xml_startElementHandler, _xml_endElementHandler);
	XML_SetCharacterDataHandler(parser->parser, _xml_characterDataHandler);

	ret = XML_Parse(parser->parser, Z_STRVAL_PP(data), Z_STRLEN_PP(data), 1);

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
