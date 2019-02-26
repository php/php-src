/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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
   | Authors: Stig Sæther Bakken <ssb@php.net>                            |
   |          Thies C. Arntzen <thies@thieso.net>                         |
   |          Sterling Hughes <sterling@php.net>                          |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#include "zend_variables.h"
#include "ext/standard/php_string.h"
#include "ext/standard/info.h"
#include "ext/standard/html.h"

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

ZEND_BEGIN_MODULE_GLOBALS(xml)
	XML_Char *default_encoding;
ZEND_END_MODULE_GLOBALS(xml)

ZEND_DECLARE_MODULE_GLOBALS(xml)

#define XML(v) ZEND_MODULE_GLOBALS_ACCESSOR(xml, v)

typedef struct {
	int case_folding;
	XML_Parser parser;
	XML_Char *target_encoding;

	zval index;
	zval startElementHandler;
	zval endElementHandler;
	zval characterDataHandler;
	zval processingInstructionHandler;
	zval defaultHandler;
	zval unparsedEntityDeclHandler;
	zval notationDeclHandler;
	zval externalEntityRefHandler;
	zval unknownEncodingHandler;
	zval startNamespaceDeclHandler;
	zval endNamespaceDeclHandler;

	zend_function *startElementPtr;
	zend_function *endElementPtr;
	zend_function *characterDataPtr;
	zend_function *processingInstructionPtr;
	zend_function *defaultPtr;
	zend_function *unparsedEntityDeclPtr;
	zend_function *notationDeclPtr;
	zend_function *externalEntityRefPtr;
	zend_function *unknownEncodingPtr;
	zend_function *startNamespaceDeclPtr;
	zend_function *endNamespaceDeclPtr;

	zval object;

	zval data;
	zval info;
	int level;
	int toffset;
	int curtag;
	zval *ctag;
	char **ltags;
	int lastwasopen;
	int skipwhite;
	int isparsing;

	XML_Char *baseURI;
} xml_parser;


typedef struct {
	XML_Char *name;
	char (*decoding_function)(unsigned short);
	unsigned short (*encoding_function)(unsigned char);
} xml_encoding;


enum php_xml_option {
    PHP_XML_OPTION_CASE_FOLDING = 1,
    PHP_XML_OPTION_TARGET_ENCODING,
    PHP_XML_OPTION_SKIP_TAGSTART,
    PHP_XML_OPTION_SKIP_WHITE
};

/* {{{ dynamically loadable module stuff */
#ifdef COMPILE_DL_XML
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(xml)
#endif /* COMPILE_DL_XML */
/* }}} */

#define XML_MAXLEVEL 255 /* XXX this should be dynamic */

#define SKIP_TAGSTART(str) ((str) + (parser->toffset > (int)strlen(str) ? strlen(str) : parser->toffset))


/* {{{ function prototypes */
PHP_MINIT_FUNCTION(xml);
PHP_MINFO_FUNCTION(xml);
static PHP_GINIT_FUNCTION(xml);

PHP_FUNCTION(xml_parser_create);
PHP_FUNCTION(xml_parser_create_ns);
PHP_FUNCTION(xml_set_object);
PHP_FUNCTION(xml_set_element_handler);
PHP_FUNCTION(xml_set_character_data_handler);
PHP_FUNCTION(xml_set_processing_instruction_handler);
PHP_FUNCTION(xml_set_default_handler);
PHP_FUNCTION(xml_set_unparsed_entity_decl_handler);
PHP_FUNCTION(xml_set_notation_decl_handler);
PHP_FUNCTION(xml_set_external_entity_ref_handler);
PHP_FUNCTION(xml_set_start_namespace_decl_handler);
PHP_FUNCTION(xml_set_end_namespace_decl_handler);
PHP_FUNCTION(xml_parse);
PHP_FUNCTION(xml_get_error_code);
PHP_FUNCTION(xml_error_string);
PHP_FUNCTION(xml_get_current_line_number);
PHP_FUNCTION(xml_get_current_column_number);
PHP_FUNCTION(xml_get_current_byte_index);
PHP_FUNCTION(xml_parser_free);
PHP_FUNCTION(xml_parser_set_option);
PHP_FUNCTION(xml_parser_get_option);
PHP_FUNCTION(xml_parse_into_struct);

static zend_string *xml_utf8_decode(const XML_Char *, size_t, const XML_Char *);
static void xml_parser_dtor(zend_resource *rsrc);
static void xml_set_handler(zval *, zval *);
inline static unsigned short xml_encode_iso_8859_1(unsigned char);
inline static char xml_decode_iso_8859_1(unsigned short);
inline static unsigned short xml_encode_us_ascii(unsigned char);
inline static char xml_decode_us_ascii(unsigned short);
static void xml_call_handler(xml_parser *, zval *, zend_function *, int, zval *, zval *);
static void _xml_xmlchar_zval(const XML_Char *, int, const XML_Char *, zval *);
static int _xml_xmlcharlen(const XML_Char *);
static void _xml_add_to_info(xml_parser *parser,char *name);
inline static zend_string *_xml_decode_tag(xml_parser *parser, const char *tag);

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
ZEND_BEGIN_ARG_INFO_EX(arginfo_xml_parser_create, 0, 0, 0)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xml_parser_create_ns, 0, 0, 0)
	ZEND_ARG_INFO(0, encoding)
	ZEND_ARG_INFO(0, sep)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xml_set_object, 0, 0, 2)
	ZEND_ARG_INFO(0, parser)
	ZEND_ARG_INFO(0, obj)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xml_set_element_handler, 0, 0, 3)
	ZEND_ARG_INFO(0, parser)
	ZEND_ARG_INFO(0, shdl)
	ZEND_ARG_INFO(0, ehdl)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xml_set_character_data_handler, 0, 0, 2)
	ZEND_ARG_INFO(0, parser)
	ZEND_ARG_INFO(0, hdl)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xml_set_processing_instruction_handler, 0, 0, 2)
	ZEND_ARG_INFO(0, parser)
	ZEND_ARG_INFO(0, hdl)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xml_set_default_handler, 0, 0, 2)
	ZEND_ARG_INFO(0, parser)
	ZEND_ARG_INFO(0, hdl)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xml_set_unparsed_entity_decl_handler, 0, 0, 2)
	ZEND_ARG_INFO(0, parser)
	ZEND_ARG_INFO(0, hdl)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xml_set_notation_decl_handler, 0, 0, 2)
	ZEND_ARG_INFO(0, parser)
	ZEND_ARG_INFO(0, hdl)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xml_set_external_entity_ref_handler, 0, 0, 2)
	ZEND_ARG_INFO(0, parser)
	ZEND_ARG_INFO(0, hdl)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xml_set_start_namespace_decl_handler, 0, 0, 2)
	ZEND_ARG_INFO(0, parser)
	ZEND_ARG_INFO(0, hdl)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xml_set_end_namespace_decl_handler, 0, 0, 2)
	ZEND_ARG_INFO(0, parser)
	ZEND_ARG_INFO(0, hdl)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xml_parse, 0, 0, 2)
	ZEND_ARG_INFO(0, parser)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, isfinal)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xml_parse_into_struct, 0, 0, 3)
	ZEND_ARG_INFO(0, parser)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(1, values)
	ZEND_ARG_INFO(1, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xml_get_error_code, 0, 0, 1)
	ZEND_ARG_INFO(0, parser)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xml_error_string, 0, 0, 1)
	ZEND_ARG_INFO(0, code)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xml_get_current_line_number, 0, 0, 1)
	ZEND_ARG_INFO(0, parser)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xml_get_current_column_number, 0, 0, 1)
	ZEND_ARG_INFO(0, parser)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xml_get_current_byte_index, 0, 0, 1)
	ZEND_ARG_INFO(0, parser)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xml_parser_free, 0, 0, 1)
	ZEND_ARG_INFO(0, parser)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xml_parser_set_option, 0, 0, 3)
	ZEND_ARG_INFO(0, parser)
	ZEND_ARG_INFO(0, option)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xml_parser_get_option, 0, 0, 2)
	ZEND_ARG_INFO(0, parser)
	ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO()

static const zend_function_entry xml_functions[] = {
	PHP_FE(xml_parser_create,					arginfo_xml_parser_create)
	PHP_FE(xml_parser_create_ns,				arginfo_xml_parser_create_ns)
	PHP_FE(xml_set_object, 						arginfo_xml_set_object)
	PHP_FE(xml_set_element_handler,				arginfo_xml_set_element_handler)
	PHP_FE(xml_set_character_data_handler,		arginfo_xml_set_character_data_handler)
	PHP_FE(xml_set_processing_instruction_handler, 	arginfo_xml_set_processing_instruction_handler)
	PHP_FE(xml_set_default_handler, 				arginfo_xml_set_default_handler)
	PHP_FE(xml_set_unparsed_entity_decl_handler,arginfo_xml_set_unparsed_entity_decl_handler)
	PHP_FE(xml_set_notation_decl_handler,		arginfo_xml_set_notation_decl_handler)
	PHP_FE(xml_set_external_entity_ref_handler,	arginfo_xml_set_external_entity_ref_handler)
	PHP_FE(xml_set_start_namespace_decl_handler,arginfo_xml_set_start_namespace_decl_handler)
	PHP_FE(xml_set_end_namespace_decl_handler,	arginfo_xml_set_end_namespace_decl_handler)
	PHP_FE(xml_parse,							arginfo_xml_parse)
	PHP_FE(xml_parse_into_struct, 				arginfo_xml_parse_into_struct)
	PHP_FE(xml_get_error_code,					arginfo_xml_get_error_code)
	PHP_FE(xml_error_string,					arginfo_xml_error_string)
	PHP_FE(xml_get_current_line_number,			arginfo_xml_get_current_line_number)
	PHP_FE(xml_get_current_column_number,		arginfo_xml_get_current_column_number)
	PHP_FE(xml_get_current_byte_index,			arginfo_xml_get_current_byte_index)
	PHP_FE(xml_parser_free, 					arginfo_xml_parser_free)
	PHP_FE(xml_parser_set_option, 				arginfo_xml_parser_set_option)
	PHP_FE(xml_parser_get_option,				arginfo_xml_parser_get_option)
	PHP_FE_END
};

#ifdef LIBXML_EXPAT_COMPAT
static const zend_module_dep xml_deps[] = {
	ZEND_MOD_REQUIRED("libxml")
	ZEND_MOD_END
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
    PHP_XML_VERSION,
    PHP_MODULE_GLOBALS(xml), /* globals descriptor */
    PHP_GINIT(xml),          /* globals ctor */
    NULL,                    /* globals dtor */
    NULL,                    /* post deactivate */
	STANDARD_MODULE_PROPERTIES_EX
};

/* All the encoding functions are set to NULL right now, since all
 * the encoding is currently done internally by expat/xmltok.
 */
const xml_encoding xml_encodings[] = {
	{ (XML_Char *)"ISO-8859-1", xml_decode_iso_8859_1, xml_encode_iso_8859_1 },
	{ (XML_Char *)"US-ASCII",   xml_decode_us_ascii,   xml_encode_us_ascii   },
	{ (XML_Char *)"UTF-8",      NULL,                  NULL                  },
	{ (XML_Char *)NULL,         NULL,                  NULL                  }
};

static XML_Memory_Handling_Suite php_xml_mem_hdlrs;

/* True globals, no need for thread safety */
static int le_xml_parser;

/* }}} */

/* {{{ startup, shutdown and info functions */
static PHP_GINIT_FUNCTION(xml)
{
#if defined(COMPILE_DL_XML) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	xml_globals->default_encoding = (XML_Char*)"UTF-8";
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

static void _xml_xmlchar_zval(const XML_Char *s, int len, const XML_Char *encoding, zval *ret)
{
	if (s == NULL) {
		ZVAL_FALSE(ret);
		return;
	}
	if (len == 0) {
		len = _xml_xmlcharlen(s);
	}
	ZVAL_STR(ret, xml_utf8_decode(s, len, encoding));
}
/* }}} */

/* {{{ xml_parser_dtor() */
static void xml_parser_dtor(zend_resource *rsrc)
{
	xml_parser *parser = (xml_parser *)rsrc->ptr;

	if (parser->parser) {
		XML_ParserFree(parser->parser);
	}
	if (parser->ltags) {
		int inx;
		for (inx = 0; ((inx < parser->level) && (inx < XML_MAXLEVEL)); inx++)
			efree(parser->ltags[ inx ]);
		efree(parser->ltags);
	}
	if (!Z_ISUNDEF(parser->startElementHandler)) {
		zval_ptr_dtor(&parser->startElementHandler);
	}
	if (!Z_ISUNDEF(parser->endElementHandler)) {
		zval_ptr_dtor(&parser->endElementHandler);
	}
	if (!Z_ISUNDEF(parser->characterDataHandler)) {
		zval_ptr_dtor(&parser->characterDataHandler);
	}
	if (!Z_ISUNDEF(parser->processingInstructionHandler)) {
		zval_ptr_dtor(&parser->processingInstructionHandler);
	}
	if (!Z_ISUNDEF(parser->defaultHandler)) {
		zval_ptr_dtor(&parser->defaultHandler);
	}
	if (!Z_ISUNDEF(parser->unparsedEntityDeclHandler)) {
		zval_ptr_dtor(&parser->unparsedEntityDeclHandler);
	}
	if (!Z_ISUNDEF(parser->notationDeclHandler)) {
		zval_ptr_dtor(&parser->notationDeclHandler);
	}
	if (!Z_ISUNDEF(parser->externalEntityRefHandler)) {
		zval_ptr_dtor(&parser->externalEntityRefHandler);
	}
	if (!Z_ISUNDEF(parser->unknownEncodingHandler)) {
		zval_ptr_dtor(&parser->unknownEncodingHandler);
	}
	if (!Z_ISUNDEF(parser->startNamespaceDeclHandler)) {
		zval_ptr_dtor(&parser->startNamespaceDeclHandler);
	}
	if (!Z_ISUNDEF(parser->endNamespaceDeclHandler)) {
		zval_ptr_dtor(&parser->endNamespaceDeclHandler);
	}
	if (parser->baseURI) {
		efree(parser->baseURI);
	}
	if (!Z_ISUNDEF(parser->object)) {
		zval_ptr_dtor(&parser->object);
	}

	efree(parser);
}
/* }}} */

/* {{{ xml_set_handler() */
static void xml_set_handler(zval *handler, zval *data)
{
	/* If we have already a handler, release it */
	if (handler) {
		zval_ptr_dtor(handler);
	}

	/* IS_ARRAY might indicate that we're using array($obj, 'method') syntax */
	if (Z_TYPE_P(data) != IS_ARRAY && Z_TYPE_P(data) != IS_OBJECT) {
		convert_to_string_ex(data);
		if (Z_STRLEN_P(data) == 0) {
			ZVAL_UNDEF(handler);
			return;
		}
	}

	ZVAL_COPY(handler, data);
}
/* }}} */

/* {{{ xml_call_handler() */
static void xml_call_handler(xml_parser *parser, zval *handler, zend_function *function_ptr, int argc, zval *argv, zval *retval)
{
	int i;

	ZVAL_UNDEF(retval);
	if (parser && handler && !EG(exception)) {
		int result;
		zend_fcall_info fci;

		fci.size = sizeof(fci);
		ZVAL_COPY_VALUE(&fci.function_name, handler);
		fci.object = Z_OBJ(parser->object);
		fci.retval = retval;
		fci.param_count = argc;
		fci.params = argv;
		fci.no_separation = 0;
		/*fci.function_handler_cache = &function_ptr;*/

		result = zend_call_function(&fci, NULL);
		if (result == FAILURE) {
			zval *method;
			zval *obj;

			if (Z_TYPE_P(handler) == IS_STRING) {
				php_error_docref(NULL, E_WARNING, "Unable to call handler %s()", Z_STRVAL_P(handler));
			} else if (Z_TYPE_P(handler) == IS_ARRAY &&
					   (obj = zend_hash_index_find(Z_ARRVAL_P(handler), 0)) != NULL &&
					   (method = zend_hash_index_find(Z_ARRVAL_P(handler), 1)) != NULL &&
					   Z_TYPE_P(obj) == IS_OBJECT &&
					   Z_TYPE_P(method) == IS_STRING) {
				php_error_docref(NULL, E_WARNING, "Unable to call handler %s::%s()", ZSTR_VAL(Z_OBJCE_P(obj)->name), Z_STRVAL_P(method));
			} else
				php_error_docref(NULL, E_WARNING, "Unable to call handler");
		}
	}
	for (i = 0; i < argc; i++) {
		zval_ptr_dtor(&argv[i]);
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
static const xml_encoding *xml_get_encoding(const XML_Char *name)
{
	const xml_encoding *enc = &xml_encodings[0];

	while (enc && enc->name) {
		if (strcasecmp((char *)name, (char *)enc->name) == 0) {
			return enc;
		}
		enc++;
	}
	return NULL;
}
/* }}} */

/* {{{ xml_utf8_decode() */
static zend_string *xml_utf8_decode(const XML_Char *s, size_t len, const XML_Char *encoding)
{
	size_t pos = 0;
	unsigned int c;
	char (*decoder)(unsigned short) = NULL;
	const xml_encoding *enc = xml_get_encoding(encoding);
	zend_string *str;

	if (enc) {
		decoder = enc->decoding_function;
	}

	if (decoder == NULL) {
		/* If the target encoding was unknown, or no decoder function
		 * was specified, return the UTF-8-encoded data as-is.
		 */
		str = zend_string_init((char *)s, len, 0);
		return str;
	}

	str = zend_string_alloc(len, 0);
	ZSTR_LEN(str) = 0;
	while (pos < len) {
		int status = FAILURE;
		c = php_next_utf8_char((const unsigned char*)s, (size_t) len, &pos, &status);

		if (status == FAILURE || c > 0xFFU) {
			c = '?';
		}

		ZSTR_VAL(str)[ZSTR_LEN(str)++] = decoder ? (unsigned int)decoder(c) : c;
	}
	ZSTR_VAL(str)[ZSTR_LEN(str)] = '\0';
	if (ZSTR_LEN(str) < len) {
		str = zend_string_truncate(str, ZSTR_LEN(str), 0);
	}

	return str;
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

/* {{{ _xml_add_to_info() */
static void _xml_add_to_info(xml_parser *parser,char *name)
{
	zval *element;

	if (Z_ISUNDEF(parser->info)) {
		return;
	}

	if ((element = zend_hash_str_find(Z_ARRVAL(parser->info), name, strlen(name))) == NULL) {
		zval values;
		array_init(&values);
		element = zend_hash_str_update(Z_ARRVAL(parser->info), name, strlen(name), &values);
	}

	add_next_index_long(element, parser->curtag);

	parser->curtag++;
}
/* }}} */

/* {{{ _xml_decode_tag() */
static zend_string *_xml_decode_tag(xml_parser *parser, const char *tag)
{
	zend_string *str;

	str = xml_utf8_decode((const XML_Char *)tag, strlen(tag), parser->target_encoding);

	if (parser->case_folding) {
		php_strtoupper(ZSTR_VAL(str), ZSTR_LEN(str));
	}

	return str;
}
/* }}} */

/* {{{ _xml_startElementHandler() */
void _xml_startElementHandler(void *userData, const XML_Char *name, const XML_Char **attributes)
{
	xml_parser *parser = (xml_parser *)userData;
	const char **attrs = (const char **) attributes;
	zend_string *att, *tag_name, *val;
	zval retval, args[3];

	if (parser) {
		parser->level++;

		tag_name = _xml_decode_tag(parser, (const char *)name);

		if (!Z_ISUNDEF(parser->startElementHandler)) {
			ZVAL_COPY(&args[0], &parser->index);
			ZVAL_STRING(&args[1], SKIP_TAGSTART(ZSTR_VAL(tag_name)));
			array_init(&args[2]);

			while (attributes && *attributes) {
				zval tmp;

				att = _xml_decode_tag(parser, (const char *)attributes[0]);
				val = xml_utf8_decode(attributes[1], strlen((char *)attributes[1]), parser->target_encoding);

				ZVAL_STR(&tmp, val);
				zend_symtable_update(Z_ARRVAL(args[2]), att, &tmp);

				attributes += 2;

				zend_string_release_ex(att, 0);
			}

			xml_call_handler(parser, &parser->startElementHandler, parser->startElementPtr, 3, args, &retval);
			zval_ptr_dtor(&retval);
		}

		if (!Z_ISUNDEF(parser->data)) {
			if (parser->level <= XML_MAXLEVEL)  {
				zval tag, atr;
				int atcnt = 0;

				array_init(&tag);
				array_init(&atr);

				_xml_add_to_info(parser, ZSTR_VAL(tag_name) + parser->toffset);

				add_assoc_string(&tag, "tag", SKIP_TAGSTART(ZSTR_VAL(tag_name))); /* cast to avoid gcc-warning */
				add_assoc_string(&tag, "type", "open");
				add_assoc_long(&tag, "level", parser->level);

				parser->ltags[parser->level-1] = estrdup(ZSTR_VAL(tag_name));
				parser->lastwasopen = 1;

				attributes = (const XML_Char **) attrs;

				while (attributes && *attributes) {
					zval tmp;

					att = _xml_decode_tag(parser, (const char *)attributes[0]);
					val = xml_utf8_decode(attributes[1], strlen((char *)attributes[1]), parser->target_encoding);

					ZVAL_STR(&tmp, val);
					zend_symtable_update(Z_ARRVAL(atr), att, &tmp);

					atcnt++;
					attributes += 2;

					zend_string_release_ex(att, 0);
				}

				if (atcnt) {
					zend_hash_str_add(Z_ARRVAL(tag), "attributes", sizeof("attributes") - 1, &atr);
				} else {
					zval_ptr_dtor(&atr);
				}

				parser->ctag = zend_hash_next_index_insert(Z_ARRVAL(parser->data), &tag);
			} else if (parser->level == (XML_MAXLEVEL + 1)) {
							php_error_docref(NULL, E_WARNING, "Maximum depth exceeded - Results truncated");
			}
		}

		zend_string_release_ex(tag_name, 0);
	}
}
/* }}} */

/* {{{ _xml_endElementHandler() */
void _xml_endElementHandler(void *userData, const XML_Char *name)
{
	xml_parser *parser = (xml_parser *)userData;

	if (parser) {
		zval retval, args[2];

		zend_string *tag_name = _xml_decode_tag(parser, (const char *)name);

		if (!Z_ISUNDEF(parser->endElementHandler)) {
			ZVAL_COPY(&args[0], &parser->index);
			ZVAL_STRING(&args[1], SKIP_TAGSTART(ZSTR_VAL(tag_name)));

			xml_call_handler(parser, &parser->endElementHandler, parser->endElementPtr, 2, args, &retval);
			zval_ptr_dtor(&retval);
		}

		if (!Z_ISUNDEF(parser->data)) {
			zval tag;

			if (parser->lastwasopen) {
				add_assoc_string(parser->ctag, "type", "complete");
			} else {
				array_init(&tag);

				_xml_add_to_info(parser, ZSTR_VAL(tag_name) + parser->toffset);

				add_assoc_string(&tag, "tag", SKIP_TAGSTART(ZSTR_VAL(tag_name))); /* cast to avoid gcc-warning */
				add_assoc_string(&tag, "type", "close");
				add_assoc_long(&tag, "level", parser->level);

				zend_hash_next_index_insert(Z_ARRVAL(parser->data), &tag);
			}

			parser->lastwasopen = 0;
		}

		zend_string_release_ex(tag_name, 0);

		if ((parser->ltags) && (parser->level <= XML_MAXLEVEL)) {
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
		zval retval, args[2];

		if (!Z_ISUNDEF(parser->characterDataHandler)) {
			ZVAL_COPY(&args[0], &parser->index);
			_xml_xmlchar_zval(s, len, parser->target_encoding, &args[1]);
			xml_call_handler(parser, &parser->characterDataHandler, parser->characterDataPtr, 2, args, &retval);
			zval_ptr_dtor(&retval);
		}

		if (!Z_ISUNDEF(parser->data)) {
			size_t i;
			int doprint = 0;
			zend_string *decoded_value;

			decoded_value = xml_utf8_decode(s, len, parser->target_encoding);
			for (i = 0; i < ZSTR_LEN(decoded_value); i++) {
				switch (ZSTR_VAL(decoded_value)[i]) {
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
					zval *myval;

					/* check if the current tag already has a value - if yes append to that! */
					if ((myval = zend_hash_str_find(Z_ARRVAL_P(parser->ctag), "value", sizeof("value") - 1))) {
						int newlen = Z_STRLEN_P(myval) + ZSTR_LEN(decoded_value);
						Z_STR_P(myval) = zend_string_extend(Z_STR_P(myval), newlen, 0);
						strncpy(Z_STRVAL_P(myval) + Z_STRLEN_P(myval) - ZSTR_LEN(decoded_value),
								ZSTR_VAL(decoded_value), ZSTR_LEN(decoded_value) + 1);
						zend_string_release_ex(decoded_value, 0);
					} else {
						add_assoc_str(parser->ctag, "value", decoded_value);
					}

				} else {
					zval tag;
					zval *curtag, *mytype, *myval;

					ZEND_HASH_REVERSE_FOREACH_VAL(Z_ARRVAL(parser->data), curtag) {
						if ((mytype = zend_hash_str_find(Z_ARRVAL_P(curtag),"type", sizeof("type") - 1))) {
							if (!strcmp(Z_STRVAL_P(mytype), "cdata")) {
								if ((myval = zend_hash_str_find(Z_ARRVAL_P(curtag), "value", sizeof("value") - 1))) {
									int newlen = Z_STRLEN_P(myval) + ZSTR_LEN(decoded_value);
									Z_STR_P(myval) = zend_string_extend(Z_STR_P(myval), newlen, 0);
									strncpy(Z_STRVAL_P(myval) + Z_STRLEN_P(myval) - ZSTR_LEN(decoded_value),
											ZSTR_VAL(decoded_value), ZSTR_LEN(decoded_value) + 1);
									zend_string_release_ex(decoded_value, 0);
									return;
								}
							}
						}
						break;
					} ZEND_HASH_FOREACH_END();

					if (parser->level <= XML_MAXLEVEL && parser->level > 0) {
						array_init(&tag);

						_xml_add_to_info(parser,SKIP_TAGSTART(parser->ltags[parser->level-1]));

						add_assoc_string(&tag, "tag", SKIP_TAGSTART(parser->ltags[parser->level-1]));
						add_assoc_str(&tag, "value", decoded_value);
						add_assoc_string(&tag, "type", "cdata");
						add_assoc_long(&tag, "level", parser->level);

						zend_hash_next_index_insert(Z_ARRVAL(parser->data), &tag);
					} else if (parser->level == (XML_MAXLEVEL + 1)) {
											php_error_docref(NULL, E_WARNING, "Maximum depth exceeded - Results truncated");
					}
				}
			} else {
				zend_string_release_ex(decoded_value, 0);
			}
		}
	}
}
/* }}} */

/* {{{ _xml_processingInstructionHandler() */
void _xml_processingInstructionHandler(void *userData, const XML_Char *target, const XML_Char *data)
{
	xml_parser *parser = (xml_parser *)userData;

	if (parser && !Z_ISUNDEF(parser->processingInstructionHandler)) {
		zval retval, args[3];

		ZVAL_COPY(&args[0], &parser->index);
		_xml_xmlchar_zval(target, 0, parser->target_encoding, &args[1]);
		_xml_xmlchar_zval(data, 0, parser->target_encoding, &args[2]);
		xml_call_handler(parser, &parser->processingInstructionHandler, parser->processingInstructionPtr, 3, args, &retval);
		zval_ptr_dtor(&retval);
	}
}
/* }}} */

/* {{{ _xml_defaultHandler() */
void _xml_defaultHandler(void *userData, const XML_Char *s, int len)
{
	xml_parser *parser = (xml_parser *)userData;

	if (parser && !Z_ISUNDEF(parser->defaultHandler)) {
		zval retval, args[2];

		ZVAL_COPY(&args[0], &parser->index);
		_xml_xmlchar_zval(s, len, parser->target_encoding, &args[1]);
		xml_call_handler(parser, &parser->defaultHandler, parser->defaultPtr, 2, args, &retval);
		zval_ptr_dtor(&retval);
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

	if (parser && !Z_ISUNDEF(parser->unparsedEntityDeclHandler)) {
		zval retval, args[6];

		ZVAL_COPY(&args[0], &parser->index);
		_xml_xmlchar_zval(entityName, 0, parser->target_encoding, &args[1]);
		_xml_xmlchar_zval(base, 0, parser->target_encoding, &args[2]);
		_xml_xmlchar_zval(systemId, 0, parser->target_encoding, &args[3]);
		_xml_xmlchar_zval(publicId, 0, parser->target_encoding, &args[4]);
		_xml_xmlchar_zval(notationName, 0, parser->target_encoding, &args[5]);
		xml_call_handler(parser, &parser->unparsedEntityDeclHandler, parser->unparsedEntityDeclPtr, 6, args, &retval);
		zval_ptr_dtor(&retval);
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

	if (parser && !Z_ISUNDEF(parser->notationDeclHandler)) {
		zval retval, args[5];

		ZVAL_COPY(&args[0], &parser->index);
		_xml_xmlchar_zval(notationName, 0, parser->target_encoding, &args[1]);
		_xml_xmlchar_zval(base, 0, parser->target_encoding, &args[2]);
		_xml_xmlchar_zval(systemId, 0, parser->target_encoding, &args[3]);
		_xml_xmlchar_zval(publicId, 0, parser->target_encoding, &args[4]);
		xml_call_handler(parser, &parser->notationDeclHandler, parser->notationDeclPtr, 5, args, &retval);
		zval_ptr_dtor(&retval);
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

	if (parser && !Z_ISUNDEF(parser->externalEntityRefHandler)) {
		zval retval, args[5];

		ZVAL_COPY(&args[0], &parser->index);
		_xml_xmlchar_zval(openEntityNames, 0, parser->target_encoding, &args[1]);
		_xml_xmlchar_zval(base, 0, parser->target_encoding, &args[2]);
		_xml_xmlchar_zval(systemId, 0, parser->target_encoding, &args[3]);
		_xml_xmlchar_zval(publicId, 0, parser->target_encoding, &args[4]);
		xml_call_handler(parser, &parser->externalEntityRefHandler, parser->externalEntityRefPtr, 5, args, &retval);
		if (!Z_ISUNDEF(retval)) {
			convert_to_long(&retval);
			ret = Z_LVAL(retval);
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

	if (parser && !Z_ISUNDEF(parser->startNamespaceDeclHandler)) {
		zval retval, args[3];

		ZVAL_COPY(&args[0], &parser->index);
		_xml_xmlchar_zval(prefix, 0, parser->target_encoding, &args[1]);
		_xml_xmlchar_zval(uri, 0, parser->target_encoding, &args[2]);
		xml_call_handler(parser, &parser->startNamespaceDeclHandler, parser->startNamespaceDeclPtr, 3, args, &retval);
		zval_ptr_dtor(&retval);
	}
}
/* }}} */

/* {{{ _xml_endNamespaceDeclHandler() */
void _xml_endNamespaceDeclHandler(void *userData, const XML_Char *prefix)
{
	xml_parser *parser = (xml_parser *)userData;

	if (parser && !Z_ISUNDEF(parser->endNamespaceDeclHandler)) {
		zval retval, args[2];

		ZVAL_COPY(&args[0], &parser->index);
		_xml_xmlchar_zval(prefix, 0, parser->target_encoding, &args[1]);
		xml_call_handler(parser, &parser->endNamespaceDeclHandler, parser->endNamespaceDeclPtr, 2, args, &retval);
		zval_ptr_dtor(&retval);
	}
}
/* }}} */

/************************* EXTENSION FUNCTIONS *************************/

static void php_xml_parser_create_impl(INTERNAL_FUNCTION_PARAMETERS, int ns_support) /* {{{ */
{
	xml_parser *parser;
	int auto_detect = 0;

	char *encoding_param = NULL;
	size_t encoding_param_len = 0;

	char *ns_param = NULL;
	size_t ns_param_len = 0;

	XML_Char *encoding;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), (ns_support ? "|ss": "|s"), &encoding_param, &encoding_param_len, &ns_param, &ns_param_len) == FAILURE) {
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
			encoding = (XML_Char*)"ISO-8859-1";
		} else if (strcasecmp(encoding_param, "UTF-8") == 0) {
			encoding = (XML_Char*)"UTF-8";
		} else if (strcasecmp(encoding_param, "US-ASCII") == 0) {
			encoding = (XML_Char*)"US-ASCII";
		} else {
			php_error_docref(NULL, E_WARNING, "unsupported source encoding \"%s\"", encoding_param);
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
                                         &php_xml_mem_hdlrs, (XML_Char*)ns_param);

	parser->target_encoding = encoding;
	parser->case_folding = 1;
	parser->isparsing = 0;

	XML_SetUserData(parser->parser, parser);

	RETVAL_RES(zend_register_resource(parser, le_xml_parser));
	ZVAL_COPY(&parser->index, return_value);
}
/* }}} */

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
	zval *pind, *mythis;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ro", &pind, &mythis) == FAILURE) {
		return;
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

	/* please leave this commented - or ask thies@thieso.net before doing it (again) */
	if (!Z_ISUNDEF(parser->object)) {
		zval_ptr_dtor(&parser->object);
	}

	/* please leave this commented - or ask thies@thieso.net before doing it (again) */
	/* zval_add_ref(&parser->object); */

	Z_ADDREF_P(mythis);
	ZVAL_OBJ(&parser->object, Z_OBJ_P(mythis));

	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int xml_set_element_handler(resource parser, string shdl, string ehdl)
   Set up start and end element handlers */
PHP_FUNCTION(xml_set_element_handler)
{
	xml_parser *parser;
	zval *pind, *shdl, *ehdl;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rzz", &pind, &shdl, &ehdl) == FAILURE) {
		return;
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

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
	zval *pind, *hdl;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rz", &pind, &hdl) == FAILURE) {
		return;
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

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
	zval *pind, *hdl;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rz", &pind, &hdl) == FAILURE) {
		return;
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

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
	zval *pind, *hdl;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rz", &pind, &hdl) == FAILURE) {
		return;
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

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
	zval *pind, *hdl;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rz", &pind, &hdl) == FAILURE) {
		return;
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

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
	zval *pind, *hdl;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rz", &pind, &hdl) == FAILURE) {
		return;
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

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
	zval *pind, *hdl;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rz", &pind, &hdl) == FAILURE) {
		return;
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

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
	zval *pind, *hdl;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rz", &pind, &hdl) == FAILURE) {
		return;
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

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
	zval *pind, *hdl;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rz", &pind, &hdl) == FAILURE) {
		return;
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

	xml_set_handler(&parser->endNamespaceDeclHandler, hdl);
	XML_SetEndNamespaceDeclHandler(parser->parser, _xml_endNamespaceDeclHandler);
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int xml_parse(resource parser, string data [, bool isFinal])
   Start parsing an XML document */
PHP_FUNCTION(xml_parse)
{
	xml_parser *parser;
	zval *pind;
	char *data;
	size_t data_len;
	int ret;
	zend_bool isFinal = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs|b", &pind, &data, &data_len, &isFinal) == FAILURE) {
		return;
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

	parser->isparsing = 1;
	ret = XML_Parse(parser->parser, (XML_Char*)data, data_len, isFinal);
	parser->isparsing = 0;
	RETVAL_LONG(ret);
}

/* }}} */

/* {{{ proto int xml_parse_into_struct(resource parser, string data, array &values [, array &index ])
   Parsing a XML document */

PHP_FUNCTION(xml_parse_into_struct)
{
	xml_parser *parser;
	zval *pind, *xdata, *info = NULL;
	char *data;
	size_t data_len;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rsz|z", &pind, &data, &data_len, &xdata, &info) == FAILURE) {
		return;
	}

	if (info) {
		info = zend_try_array_init(info);
		if (!info) {
			return;
		}
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

	xdata = zend_try_array_init(xdata);
	if (!xdata) {
		return;
	}

	ZVAL_COPY_VALUE(&parser->data, xdata);

	if (info) {
		ZVAL_COPY_VALUE(&parser->info, info);
	}

	parser->level = 0;
	parser->ltags = safe_emalloc(XML_MAXLEVEL, sizeof(char *), 0);

	XML_SetElementHandler(parser->parser, _xml_startElementHandler, _xml_endElementHandler);
	XML_SetCharacterDataHandler(parser->parser, _xml_characterDataHandler);

	parser->isparsing = 1;
	ret = XML_Parse(parser->parser, (XML_Char*)data, data_len, 1);
	parser->isparsing = 0;

	RETVAL_LONG(ret);
}
/* }}} */

/* {{{ proto int xml_get_error_code(resource parser)
   Get XML parser error code */
PHP_FUNCTION(xml_get_error_code)
{
	xml_parser *parser;
	zval *pind;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pind) == FAILURE) {
		return;
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

	RETURN_LONG((zend_long)XML_GetErrorCode(parser->parser));
}
/* }}} */

/* {{{ proto string xml_error_string(int code)
   Get XML parser error string */
PHP_FUNCTION(xml_error_string)
{
	zend_long code;
	char *str;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &code) == FAILURE) {
		return;
	}

	str = (char *)XML_ErrorString((int)code);
	if (str) {
		RETVAL_STRING(str);
	}
}
/* }}} */

/* {{{ proto int xml_get_current_line_number(resource parser)
   Get current line number for an XML parser */
PHP_FUNCTION(xml_get_current_line_number)
{
	xml_parser *parser;
	zval *pind;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pind) == FAILURE) {
		return;
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

	RETVAL_LONG(XML_GetCurrentLineNumber(parser->parser));
}
/* }}} */

/* {{{ proto int xml_get_current_column_number(resource parser)
   Get current column number for an XML parser */
PHP_FUNCTION(xml_get_current_column_number)
{
	xml_parser *parser;
	zval *pind;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pind) == FAILURE) {
		return;
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

	RETVAL_LONG(XML_GetCurrentColumnNumber(parser->parser));
}
/* }}} */

/* {{{ proto int xml_get_current_byte_index(resource parser)
   Get current byte index for an XML parser */
PHP_FUNCTION(xml_get_current_byte_index)
{
	xml_parser *parser;
	zval *pind;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pind) == FAILURE) {
		return;
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

	RETVAL_LONG(XML_GetCurrentByteIndex(parser->parser));
}
/* }}} */

/* {{{ proto int xml_parser_free(resource parser)
   Free an XML parser */
PHP_FUNCTION(xml_parser_free)
{
	zval *pind;
	xml_parser *parser;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pind) == FAILURE) {
		return;
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

	if (parser->isparsing == 1) {
		php_error_docref(NULL, E_WARNING, "Parser cannot be freed while it is parsing.");
		RETURN_FALSE;
	}

	if (zend_list_delete(Z_RES(parser->index)) == FAILURE) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int xml_parser_set_option(resource parser, int option, mixed value)
   Set options in an XML parser */
PHP_FUNCTION(xml_parser_set_option)
{
	xml_parser *parser;
	zval *pind, *val;
	zend_long opt;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rlz", &pind, &opt, &val) == FAILURE) {
		return;
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

	switch (opt) {
		case PHP_XML_OPTION_CASE_FOLDING:
			parser->case_folding = zval_get_long(val);
			break;
		case PHP_XML_OPTION_SKIP_TAGSTART:
			parser->toffset = zval_get_long(val);
			if (parser->toffset < 0) {
				php_error_docref(NULL, E_NOTICE, "tagstart ignored, because it is out of range");
				parser->toffset = 0;
			}
			break;
		case PHP_XML_OPTION_SKIP_WHITE:
			parser->skipwhite = zval_get_long(val);
			break;
		case PHP_XML_OPTION_TARGET_ENCODING: {
			const xml_encoding *enc;
			if (!try_convert_to_string(val)) {
				return;
			}

			enc = xml_get_encoding((XML_Char*)Z_STRVAL_P(val));
			if (enc == NULL) {
				php_error_docref(NULL, E_WARNING, "Unsupported target encoding \"%s\"", Z_STRVAL_P(val));
				RETURN_FALSE;
			}
			parser->target_encoding = enc->name;
			break;
		}
		default:
			php_error_docref(NULL, E_WARNING, "Unknown option");
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
	zval *pind;
	zend_long opt;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &pind, &opt) == FAILURE) {
		return;
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

	switch (opt) {
		case PHP_XML_OPTION_CASE_FOLDING:
			RETURN_LONG(parser->case_folding);
			break;
		case PHP_XML_OPTION_SKIP_TAGSTART:
			RETURN_LONG(parser->toffset);
			break;
		case PHP_XML_OPTION_SKIP_WHITE:
			RETURN_LONG(parser->skipwhite);
			break;
		case PHP_XML_OPTION_TARGET_ENCODING:
			RETURN_STRING((char *)parser->target_encoding);
			break;
		default:
			php_error_docref(NULL, E_WARNING, "Unknown option");
			RETURN_FALSE;
			break;
	}

	RETVAL_FALSE;	/* never reached */
}
/* }}} */

#endif
