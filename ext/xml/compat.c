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
   | Authors: Sterling Hughes <sterling@php.net>                          |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#if defined(HAVE_LIBXML) && defined(HAVE_XML) && !defined(HAVE_LIBEXPAT)
#include "expat_compat.h"

typedef struct _php_xml_ns {
	xmlNsPtr nsptr;
	int ref_count;
	void *next;
	void *prev;
} php_xml_ns;

#ifdef LIBXML_EXPAT_COMPAT

#define IS_NS_DECL(__ns) \
	((__ns) != NULL && strlen(__ns) == 5 && *(__ns) == 'x' && *((__ns)+1) == 'm' && \
	 *((__ns)+2) == 'l' && *((__ns)+3) == 'n' && *((__ns)+4) == 's')

#if LIBXML_VERSION >= 20600 
static void 
_qualify_namespace(XML_Parser parser, const xmlChar *name, const xmlChar *URI, xmlChar **qualified)
{
	if (URI) {
			/* Use libxml functions otherwise its memory deallocation is screwed up */
			*qualified = xmlStrdup(URI);
			*qualified = xmlStrncat(*qualified, parser->_ns_seperator, 1);
			*qualified = xmlStrncat(*qualified, name, strlen(name));
	} else {
		*qualified = xmlStrdup(name);
	}
}
#endif

static void
_start_element_handler(void *user, const xmlChar *name, const xmlChar **attributes)
{
	XML_Parser  parser = (XML_Parser) user;
	xmlChar    *qualified_name = NULL;

	if (parser->h_start_element == NULL) {
		return;
	}

	qualified_name = xmlStrdup(name);

	parser->h_start_element(parser->user, (const XML_Char *) qualified_name, (const XML_Char **) attributes);

	xmlFree(qualified_name);
}

#if LIBXML_VERSION >= 20600 
static void
_start_element_handler_ns(void *user, const xmlChar *name, const xmlChar *prefix, const xmlChar *URI, int nb_namespaces, const xmlChar ** namespaces, int nb_attributes, int nb_defaulted, const xmlChar ** attributes)
{
	XML_Parser  parser = (XML_Parser) user;
	xmlChar    *qualified_name = NULL;

	if (parser->h_start_element == NULL) {
		return;
	}
	_qualify_namespace(parser, name, URI, &qualified_name);
	parser->h_start_element(parser->user, (const XML_Char *) qualified_name, (const XML_Char **) attributes);

	xmlFree(qualified_name);
}
#endif

static void
_namespace_handler(XML_Parser parser, xmlNsPtr nsptr)
{
	if (nsptr != NULL) {
		_namespace_handler(parser, nsptr->next);
		parser->h_end_ns(parser->user, nsptr->prefix);
	}
}

static void
_end_element_handler(void *user, const xmlChar *name)
{
	xmlChar    *qualified_name;
	XML_Parser  parser = (XML_Parser) user;

	if (parser->h_end_element == NULL) {
		return;
	}
	
	qualified_name = xmlStrdup(name);

	parser->h_end_element(parser->user, (const XML_Char *) qualified_name);

	xmlFree(qualified_name);
}

#if LIBXML_VERSION >= 20600 
static void
_end_element_handler_ns(void *user, const xmlChar *name, const xmlChar * prefix, const xmlChar *URI)
{
	xmlChar    *qualified_name;
	XML_Parser  parser = (XML_Parser) user;

	if (parser->h_end_element == NULL) {
		return;
	}

	_qualify_namespace(parser, name, URI,  &qualified_name);

	parser->h_end_element(parser->user, (const XML_Char *) qualified_name);

	xmlFree(qualified_name);
}
#endif

static void
_cdata_handler(void *user, const xmlChar *cdata, int cdata_len)
{
	XML_Parser parser = (XML_Parser) user;

	if (parser->h_cdata == NULL) {
		return;
	}

	parser->h_cdata(parser->user, (const XML_Char *) cdata, cdata_len);
}

static void
_pi_handler(void *user, const xmlChar *target, const xmlChar *data)
{
	XML_Parser parser = (XML_Parser) user;

	if (parser->h_pi == NULL) {
		return;
	}

	parser->h_pi(parser->user, (const XML_Char *) target, (const XML_Char *) data);
}

static void
_unparsed_entity_decl_handler(void *user, 
                              const xmlChar *name, 
							  const xmlChar *sys_id, 
							  const xmlChar *pub_id, 
							  const xmlChar *notation)
{
	XML_Parser parser = (XML_Parser) user;

	if (parser->h_unparsed_entity_decl == NULL) {
		return;
	}

	parser->h_unparsed_entity_decl(parser->user, name, NULL, sys_id, pub_id, notation);
}

static void
_notation_decl_handler(void *user, const xmlChar *notation, const xmlChar *sys_id, const xmlChar *pub_id)
{
	XML_Parser parser = (XML_Parser) user;

	if (parser->h_notation_decl == NULL) {
		return;
	}

	parser->h_notation_decl(parser->user, notation, NULL, sys_id, pub_id);
}

static void
_build_comment(const xmlChar *data, int data_len, xmlChar **comment, int *comment_len)
{
	*comment_len = data_len + 6;
	
	*comment = xmlMalloc(*comment_len + 1);
	memcpy(*comment, "<--", 3);
	memcpy(*comment + 3, data, data_len);
	memcpy(*comment + 3 + data_len, "-->", 3);

	(*comment)[*comment_len] = '\0';
}

static void
_comment_handler(void *user, const xmlChar *comment)
{
	XML_Parser parser = (XML_Parser) user;

	if (parser->h_default) {
		xmlChar *d_comment;
		int      d_comment_len;

		_build_comment(comment, xmlStrlen(comment), &d_comment, &d_comment_len);
		parser->h_default(parser->user, d_comment, d_comment_len);
	}
}

static void
_build_entity(const xmlChar *name, int len, xmlChar **entity, int *entity_len) 
{
	*entity_len = len + 2;
	*entity = xmlMalloc(*entity_len + 1);
	(*entity)[0] = '&';
	memcpy(*entity+1, name, len);
	(*entity)[len+1] = ';';
	(*entity)[*entity_len] = '\0';
}

static xmlEntityPtr
_get_entity(void *user, const xmlChar *name)
{
	XML_Parser parser = (XML_Parser) user;

	if (parser->h_default) {
		xmlChar *entity;
		int      len;
		
		_build_entity(name, xmlStrlen(name), &entity, &len);
		parser->h_default(parser->user, (const xmlChar *) entity, len);
	}

	return NULL;
}

static void
_external_entity_ref_handler(void *user, const xmlChar *names, int type, const xmlChar *sys_id, const xmlChar *pub_id, xmlChar *content)
{
	XML_Parser parser = (XML_Parser) user;

	if (parser->h_external_entity_ref == NULL) {
		return;
	}

	parser->h_external_entity_ref(parser, names, "", sys_id, pub_id);
}

static xmlSAXHandler 
php_xml_compat_handlers = {
	NULL, /* internalSubset */
	NULL, /* isStandalone */
	NULL, /* hasInternalSubset */
	NULL, /* hasExternalSubset */
	NULL, /* resolveEntity */
	_get_entity, /* getEntity */
	_external_entity_ref_handler, /* entityDecl */
	_notation_decl_handler,
	NULL, /* attributeDecl */
	NULL, /* elementDecl */
	_unparsed_entity_decl_handler, /* unparsedEntity */
	NULL, /* setDocumentLocator */
	NULL, /* startDocument */
	NULL, /* endDocument */
	_start_element_handler, /* startElement */
	_end_element_handler, /* endElement */
	NULL, /* reference */
	_cdata_handler,
	NULL, /* ignorableWhitespace */
	_pi_handler,
	_comment_handler, /* comment */
	NULL, /* warning */
	NULL, /* error */
	NULL,  /* fatalError */
	NULL,  /* getParameterEntity */
	_cdata_handler, /* cdataBlock */
	NULL, /* externalSubset */
	1
#if LIBXML_VERSION >= 20600 
	,
	NULL,
	_start_element_handler_ns,
	_end_element_handler_ns,
	NULL
#endif
	
};

PHPAPI XML_Parser 
XML_ParserCreate(const XML_Char *encoding)
{
	return XML_ParserCreate_MM(encoding, NULL, NULL);
}

PHPAPI XML_Parser
XML_ParserCreateNS(const XML_Char *encoding, const XML_Char sep)
{
	XML_Char tmp[2];
	tmp[0] = sep;
	tmp[1] = '\0';
	return XML_ParserCreate_MM(encoding, NULL, tmp);
}

PHPAPI XML_Parser
XML_ParserCreate_MM(const XML_Char *encoding, const XML_Memory_Handling_Suite *memsuite, const XML_Char *sep)
{
	XML_Parser parser;

	parser = (XML_Parser) emalloc(sizeof(struct _XML_Parser));
	memset(parser, 0, sizeof(struct _XML_Parser));
	parser->use_namespace = 0;
	parser->_ns_seperator = NULL;

	parser->parser = xmlCreatePushParserCtxt((xmlSAXHandlerPtr) &php_xml_compat_handlers, (void *) parser, NULL, 0, NULL);
	if (parser->parser == NULL) {
		efree(parser);
		return NULL;
	}
	if (encoding != NULL) {
		parser->parser->encoding = xmlStrdup(encoding);
	} else {
		parser->parser->charset = XML_CHAR_ENCODING_NONE;
	}
	parser->parser->replaceEntities = 1;
	if (sep != NULL) {
		parser->use_namespace = 1;
#if LIBXML_VERSION >= 20600
		parser->parser->sax2 = 1;
#endif
		parser->_ns_seperator = xmlStrdup(sep);
	}
	return parser;
}

PHPAPI void
XML_SetUserData(XML_Parser parser, void *user)
{
	parser->user = user;
}

PHPAPI void *
XML_GetUserData(XML_Parser parser)
{
	return parser->user;
}

PHPAPI void
XML_SetElementHandler(XML_Parser parser, XML_StartElementHandler start, XML_EndElementHandler end)
{
	parser->h_start_element = start;
	parser->h_end_element = end;
}

PHPAPI void
XML_SetCharacterDataHandler(XML_Parser parser, XML_CharacterDataHandler cdata)
{
	parser->h_cdata = cdata;
}

PHPAPI void
XML_SetProcessingInstructionHandler(XML_Parser parser, XML_ProcessingInstructionHandler pi)
{
	parser->h_pi = pi;
}

PHPAPI void
XML_SetCommentHandler(XML_Parser parser, XML_CommentHandler comment)
{
	parser->h_comment = comment;
}

PHPAPI void 
XML_SetDefaultHandler(XML_Parser parser, XML_DefaultHandler d)
{
	parser->h_default = d;
}

PHPAPI void
XML_SetUnparsedEntityDeclHandler(XML_Parser parser, XML_UnparsedEntityDeclHandler unparsed_decl)
{
	parser->h_unparsed_entity_decl = unparsed_decl;
}

PHPAPI void
XML_SetNotationDeclHandler(XML_Parser parser, XML_NotationDeclHandler notation_decl)
{
	parser->h_notation_decl = notation_decl;
}

PHPAPI void
XML_SetExternalEntityRefHandler(XML_Parser parser, XML_ExternalEntityRefHandler ext_entity)
{
	parser->h_external_entity_ref = ext_entity;
}

PHPAPI void
XML_SetStartNamespaceDeclHandler(XML_Parser parser, XML_StartNamespaceDeclHandler start_ns)
{
	parser->h_start_ns = start_ns;
}

PHPAPI void
XML_SetEndNamespaceDeclHandler(XML_Parser parser, XML_EndNamespaceDeclHandler end_ns)
{
	parser->h_end_ns = end_ns;
}

PHPAPI int
XML_Parse(XML_Parser parser, const XML_Char *data, int data_len, int is_final)
{
#if LIBXML_VERSION >= 20600
	int error;
	error = xmlParseChunk(parser->parser, data, data_len, is_final);
	if (!error) {
		return 1;
	} else if (parser->parser->lastError.level > XML_ERR_WARNING ){
		return 0;
	} else {
		return 1;
	}
#else
	return !xmlParseChunk(parser->parser, data, data_len, is_final);
#endif
}

PHPAPI int
XML_GetErrorCode(XML_Parser parser)
{
	return parser->parser->errNo;
}

const XML_Char *error_mapping[] = {
	"No error",
	"Internal error",
	"No memory",
	"XML_ERR_DOCUMENT_START",
	"Empty document",
	"XML_ERR_DOCUMENT_END",
	"Invalid hexadecimal character reference",
	"Invalid decimal character reference",
	"Invalid character reference",
	"Invalid character",
	"XML_ERR_CHARREF_AT_EOF",
	"XML_ERR_CHARREF_IN_PROLOG",
	"XML_ERR_CHARREF_IN_EPILOG",
	"XML_ERR_CHARREF_IN_DTD",
	"XML_ERR_ENTITYREF_AT_EOF",
	"XML_ERR_ENTITYREF_IN_PROLOG",
	"XML_ERR_ENTITYREF_IN_EPILOG",
	"XML_ERR_ENTITYREF_IN_DTD",
	"XML_ERR_PEREF_AT_EOF",
	"XML_ERR_PEREF_IN_PROLOG",
	"XML_ERR_PEREF_IN_EPILOG",
	"XML_ERR_PEREF_IN_INT_SUBSET",
	"XML_ERR_ENTITYREF_NO_NAME",
	"XML_ERR_ENTITYREF_SEMICOL_MISSING",
	"XML_ERR_PEREF_NO_NAME",
	"XML_ERR_PEREF_SEMICOL_MISSING",
	"Undeclared entity error",
	"Undeclared entity warning",
	"Unparsed Entity",
	"XML_ERR_ENTITY_IS_EXTERNAL",
	"XML_ERR_ENTITY_IS_PARAMETER",
	"Unknown encoding",
	"Unsupported encoding",
	"XML_ERR_STRING_NOT_STARTED",
	"XML_ERR_STRING_NOT_CLOSED",
	"Namespace declaration error",
	"XML_ERR_ENTITY_NOT_STARTED",
	"XML_ERR_ENTITY_NOT_FINISHED",
	"XML_ERR_LT_IN_ATTRIBUTE",
	"XML_ERR_ATTRIBUTE_NOT_STARTED",
	"XML_ERR_ATTRIBUTE_NOT_FINISHED",
	"XML_ERR_ATTRIBUTE_WITHOUT_VALUE",
	"XML_ERR_ATTRIBUTE_REDEFINED",
	"XML_ERR_LITERAL_NOT_STARTED",
	"XML_ERR_LITERAL_NOT_FINISHED",
	/* "XML_ERR_COMMENT_NOT_STARTED", <= eliminated on purpose */
	"XML_ERR_COMMENT_NOT_FINISHED",
	"XML_ERR_PI_NOT_STARTED",
	"XML_ERR_PI_NOT_FINISHED",
	"XML_ERR_NOTATION_NOT_STARTED",
	"XML_ERR_NOTATION_NOT_FINISHED",
	"XML_ERR_ATTLIST_NOT_STARTED",
	"XML_ERR_ATTLIST_NOT_FINISHED",
	"XML_ERR_MIXED_NOT_STARTED",
	"XML_ERR_MIXED_NOT_FINISHED",
	"XML_ERR_ELEMCONTENT_NOT_STARTED",
	"XML_ERR_ELEMCONTENT_NOT_FINISHED",
	"XML_ERR_XMLDECL_NOT_STARTED",
	"XML_ERR_XMLDECL_NOT_FINISHED",
	"XML_ERR_CONDSEC_NOT_STARTED",
	"XML_ERR_CONDSEC_NOT_FINISHED",
	"XML_ERR_EXT_SUBSET_NOT_FINISHED",
	"XML_ERR_DOCTYPE_NOT_FINISHED",
	"XML_ERR_MISPLACED_CDATA_END",
	"XML_ERR_CDATA_NOT_FINISHED",
	"XML_ERR_RESERVED_XML_NAME",
	"XML_ERR_SPACE_REQUIRED",
	"XML_ERR_SEPARATOR_REQUIRED",
	"XML_ERR_NMTOKEN_REQUIRED",
	"XML_ERR_NAME_REQUIRED",
	"XML_ERR_PCDATA_REQUIRED",
	"XML_ERR_URI_REQUIRED",
	"XML_ERR_PUBID_REQUIRED",
	"XML_ERR_LT_REQUIRED",
	"XML_ERR_GT_REQUIRED",
	"XML_ERR_LTSLASH_REQUIRED",
	"XML_ERR_EQUAL_REQUIRED",
	"XML_ERR_TAG_NAME_MISMATCH",
	"XML_ERR_TAG_NOT_FINISHED",
	"XML_ERR_STANDALONE_VALUE",
	"XML_ERR_ENCODING_NAME",
	"XML_ERR_HYPHEN_IN_COMMENT",
	"Invalid encoding",
	"XML_ERR_EXT_ENTITY_STANDALONE",
	"XML_ERR_CONDSEC_INVALID",
	"XML_ERR_VALUE_REQUIRED",
	"XML_ERR_NOT_WELL_BALANCED",
	"XML_ERR_EXTRA_CONTENT",
    "XML_ERR_ENTITY_CHAR_ERROR",
    "XML_ERR_ENTITY_PE_INTERNAL",
    "XML_ERR_ENTITY_LOOP",
    "XML_ERR_ENTITY_BOUNDARY",
    "Invalid URI",
    "XML_ERR_URI_FRAGMENT",
    "XML_WAR_CATALOG_PI",
    "XML_ERR_NO_DTD",
    "XML_ERR_CONDSEC_INVALID_KEYWORD", /* 95 */
    "XML_ERR_VERSION_MISSING", /* 96 */
    "XML_WAR_UNKNOWN_VERSION", /* 97 */
    "XML_WAR_LANG_VALUE", /* 98 */
    "XML_WAR_NS_URI", /* 99 */
    "XML_WAR_NS_URI_RELATIVE", /* 100 */
    "XML_ERR_MISSING_ENCODING" /* 101 */
};

PHPAPI const XML_Char *
XML_ErrorString(int code)
{
	if (code < 0 || code >= (int)sizeof(error_mapping)) {
		return "Unknown";
	}
	return error_mapping[code];
}

PHPAPI int
XML_GetCurrentLineNumber(XML_Parser parser)
{
	return parser->parser->input->line;
}

PHPAPI int
XML_GetCurrentColumnNumber(XML_Parser parser)
{
	return parser->parser->input->col;
}

PHPAPI int
XML_GetCurrentByteIndex(XML_Parser parser)
{
	return parser->parser->input->consumed +
			(parser->parser->input->cur - parser->parser->input->base);
}

PHPAPI int
XML_GetCurrentByteCount(XML_Parser parser)
{
	/* WARNING: this is identical to ByteIndex; it should probably
	 * be different */
	return parser->parser->input->consumed +
			(parser->parser->input->cur - parser->parser->input->base);
}

PHPAPI const XML_Char *XML_ExpatVersion(void)
{
	return "1.0";
}

PHPAPI void
XML_ParserFree(XML_Parser parser)
{
	if (parser->use_namespace) {
		if (parser->_ns_seperator) {
			xmlFree(parser->_ns_seperator);
		}
	}
	xmlFreeParserCtxt(parser->parser);
	efree(parser);
}

#endif /* LIBXML_EXPAT_COMPAT */
#endif

/**
 * Local Variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: fdm=marker
 * vim: ts=4 noet sw=4
 */
