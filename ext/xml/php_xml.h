/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
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

#ifndef PHP_XML_H
#define PHP_XML_H

#ifdef HAVE_XML
extern zend_module_entry xml_module_entry;
#define xml_module_ptr &xml_module_entry
#else
#define xml_module_ptr NULL
#endif

#ifdef HAVE_XML 

#include "ext/xml/expat_compat.h"


#ifdef XML_UNICODE
#error "UTF-16 Unicode support not implemented!"
#endif

ZEND_BEGIN_MODULE_GLOBALS(xml)
	XML_Char *default_encoding;
ZEND_END_MODULE_GLOBALS(xml)

typedef struct {
	int index;
	int case_folding;
	XML_Parser parser;
	XML_Char *target_encoding;

	zval *startElementHandler;
	zval *endElementHandler;
	zval *characterDataHandler;
	zval *processingInstructionHandler;
	zval *defaultHandler;
	zval *unparsedEntityDeclHandler;
	zval *notationDeclHandler;
	zval *externalEntityRefHandler;
	zval *unknownEncodingHandler;	
	zval *startNamespaceDeclHandler;
	zval *endNamespaceDeclHandler;

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

	zval *object;

	zval *data;
	zval *info;
	int level;
	int toffset;
	int curtag;
	zval **ctag;
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

/* for xml_parse_into_struct */
	
#define XML_MAXLEVEL 255 /* XXX this should be dynamic */
	
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
PHP_FUNCTION(utf8_encode);
PHP_FUNCTION(utf8_decode);
PHP_FUNCTION(xml_parse_into_struct);

PHPAPI char *_xml_zval_strdup(zval *val);
PHPAPI char *xml_utf8_decode(const XML_Char *, int, int *, const XML_Char *);
PHPAPI char *xml_utf8_encode(const char *s, int len, int *newlen, const XML_Char *encoding);

#endif /* HAVE_LIBEXPAT */

#define phpext_xml_ptr xml_module_ptr

#ifdef ZTS
#define XML(v) TSRMG(xml_globals_id, zend_xml_globals *, v)
#else
#define XML(v) (xml_globals.v)
#endif

#endif /* PHP_XML_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
