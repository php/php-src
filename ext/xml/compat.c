/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2008 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sterling Hughes <sterling@php.net>                          |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#if defined(HAVE_LIBXML) && (defined(HAVE_XML) || defined(HAVE_XMLRPC)) && !defined(HAVE_LIBEXPAT)
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

static void
_start_element_handler(void *user, const xmlChar *name, const xmlChar **attributes)
{
	XML_Parser  parser = (XML_Parser) user;
	xmlChar    *qualified_name = NULL;

	if (parser->h_start_element == NULL) {
		if (parser->h_default) {
			int attno = 0;

			qualified_name = xmlStrncatNew((xmlChar *)"<", name, xmlStrlen(name));
			if (attributes) {
				while (attributes[attno] != NULL) {
					int att_len;
					char *att_string, *att_name, *att_value;

					att_name = (char *)attributes[attno++];
					att_value = (char *)attributes[attno++];

					att_len = spprintf(&att_string, 0, " %s=\"%s\"", att_name, att_value);

					qualified_name = xmlStrncat(qualified_name, (xmlChar *)att_string, att_len);
					efree(att_string);
				}

			}
			qualified_name = xmlStrncat(qualified_name, (xmlChar *)">", 1);
			parser->h_default(parser->user, (const XML_Char *) qualified_name, xmlStrlen(qualified_name));
			xmlFree(qualified_name);
		}
		return;
	}

	qualified_name = xmlStrdup(name);

	parser->h_start_element(parser->user, (const XML_Char *) qualified_name, (const XML_Char **) attributes);

	xmlFree(qualified_name);
}

static void
_start_element_handler_ns(void *user, const xmlChar *name, const xmlChar *prefix, const xmlChar *URI, int nb_namespaces, const xmlChar ** namespaces, int nb_attributes, int nb_defaulted, const xmlChar ** attributes)
{
	XML_Parser  parser = (XML_Parser) user;
	xmlChar    *qualified_name = NULL;
	xmlChar **attrs = NULL;
	int i;
	int z = 0;
	int y = 0;
	
	if (nb_namespaces > 0 && parser->h_start_ns != NULL) {
		for (i = 0; i < nb_namespaces; i += 1) {
			parser->h_start_ns(parser->user, (const XML_Char *) namespaces[y], (const XML_Char *) namespaces[y+1]);
			y += 2;
		}
		y = 0;
	}
	
	if (parser->h_start_element == NULL && parser->h_default == NULL) {
		return;
	}
	_qualify_namespace(parser, name, URI, &qualified_name);
	
	if (attributes != NULL) {
		xmlChar    *qualified_name_attr = NULL;
		attrs = safe_emalloc((nb_attributes  * 2) + 1, sizeof(int *), 0);

		for (i = 0; i < nb_attributes; i += 1) {

			if (attributes[y+1] != NULL) {
				_qualify_namespace(parser, attributes[y] , attributes[y + 2], &qualified_name_attr);
			} else {
				qualified_name_attr = xmlStrdup(attributes[y]);
			}
			attrs[z] = qualified_name_attr;
			attrs[z + 1] = xmlStrndup(attributes[y + 3] , (int) (attributes[y + 4] - attributes[y + 3]));
			z += 2;
			y += 5;
		}

		attrs[z] = NULL;
	}
	parser->h_start_element(parser->user, (const XML_Char *) qualified_name, (const XML_Char **) attrs);
	if (attrs) {
		for (i = 0; i < z; i++) {
			xmlFree(attrs[i]);
		}
		efree(attrs);
	}
	xmlFree(qualified_name);
}

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
		if (parser->h_default) {
			char *end_element;

			spprintf(&end_element, 0, "</%s>", (char *)name);
			parser->h_default(parser->user, (const XML_Char *) end_element, strlen(end_element));
			efree(end_element);
		}
		return;
	}
	
	qualified_name = xmlStrdup(name);

	parser->h_end_element(parser->user, (const XML_Char *) qualified_name);

	xmlFree(qualified_name);
}

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

static void
_cdata_handler(void *user, const xmlChar *cdata, int cdata_len)
{
	XML_Parser parser = (XML_Parser) user;

	if (parser->h_cdata == NULL) {
		if (parser->h_default) {
			parser->h_default(parser->user, (const XML_Char *) cdata, cdata_len);
		}
		return;
	}

	parser->h_cdata(parser->user, (const XML_Char *) cdata, cdata_len);
}

static void
_pi_handler(void *user, const xmlChar *target, const xmlChar *data)
{
	XML_Parser parser = (XML_Parser) user;

	if (parser->h_pi == NULL) {
		if (parser->h_default) {
			char    *full_pi;
			spprintf(&full_pi, 0, "<?%s %s?>", (char *)target, (char *)data);
			parser->h_default(parser->user, (const XML_Char *) full_pi, xmlStrlen(full_pi));
			efree(full_pi);
		}
		return;
	}

	parser->h_pi(parser->user, (const XML_Char *) target, (const XML_Char *) data);
}

static void
_unparsed_entity_decl_handler(void *user, 
                              const xmlChar *name, 
							  const xmlChar *pub_id, 
							  const xmlChar *sys_id, 
							  const xmlChar *notation)
{
	XML_Parser parser = (XML_Parser) user;

	if (parser->h_unparsed_entity_decl == NULL) {
		return;
	}

	parser->h_unparsed_entity_decl(parser->user, name, NULL, sys_id, pub_id, notation);
}

static void
_notation_decl_handler(void *user, const xmlChar *notation, const xmlChar *pub_id, const xmlChar *sys_id)
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
	*comment_len = data_len + 7;
	
	*comment = xmlMalloc(*comment_len + 1);
	memcpy(*comment, "<!--", 4);
	memcpy(*comment + 4, data, data_len);
	memcpy(*comment + 4 + data_len, "-->", 3);

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
		xmlFree(d_comment);
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

static void
_external_entity_ref_handler(void *user, const xmlChar *names, int type, const xmlChar *sys_id, const xmlChar *pub_id, xmlChar *content)
{
	XML_Parser parser = (XML_Parser) user;

	if (parser->h_external_entity_ref == NULL) {
		return;
	}

	parser->h_external_entity_ref(parser, names, "", sys_id, pub_id);
}

static xmlEntityPtr
_get_entity(void *user, const xmlChar *name)
{
	XML_Parser parser = (XML_Parser) user;
	xmlEntityPtr ret = NULL;

	if (parser->parser->inSubset == 0) {
		ret = xmlGetPredefinedEntity(name);
		if (ret == NULL)
			ret = xmlGetDocEntity(parser->parser->myDoc, name);

		if (ret == NULL || (parser->parser->instate != XML_PARSER_ENTITY_VALUE && parser->parser->instate != XML_PARSER_ATTRIBUTE_VALUE)) {
			if (ret == NULL || ret->etype == XML_INTERNAL_GENERAL_ENTITY || ret->etype == XML_INTERNAL_PARAMETER_ENTITY || ret->etype == XML_INTERNAL_PREDEFINED_ENTITY) {
				/* Predefined entities will expand unless no cdata handler is present */
				if (parser->h_default && ! (ret && ret->etype == XML_INTERNAL_PREDEFINED_ENTITY && parser->h_cdata)) {
					xmlChar *entity;
					int      len;
					
					_build_entity(name, xmlStrlen(name), &entity, &len);
					parser->h_default(parser->user, (const xmlChar *) entity, len);
					xmlFree(entity);
				} else {
					/* expat will not expand internal entities if default handler is present otherwise
					it will expand and pass them to cdata handler */
					if (parser->h_cdata && ret) {
						parser->h_cdata(parser->user, ret->content, xmlStrlen(ret->content));
					}
				}
			} else {
				if (ret->etype == XML_EXTERNAL_GENERAL_PARSED_ENTITY) {
					_external_entity_ref_handler(user, ret->name, ret->etype, ret->SystemID, ret->ExternalID, NULL);
				}
			}
		}
	}

	return ret;
}

static xmlSAXHandler 
php_xml_compat_handlers = {
	NULL, /* internalSubset */
	NULL, /* isStandalone */
	NULL, /* hasInternalSubset */
	NULL, /* hasExternalSubset */
	NULL, /* resolveEntity */
	_get_entity, /* getEntity */
	NULL, /* entityDecl */
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
	XML_SAX2_MAGIC,
	NULL,
	_start_element_handler_ns,
	_end_element_handler_ns,
	NULL	
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
#if LIBXML_VERSION <= 20617
	/* for older versions of libxml2, allow correct detection of
	 * charset in documents with a BOM: */
	parser->parser->charset = XML_CHAR_ENCODING_NONE;
#endif

	parser->parser->replaceEntities = 1;
	parser->parser->wellFormed = 0;
	if (sep != NULL) {
		parser->use_namespace = 1;
		parser->parser->sax2 = 1;
		parser->_ns_seperator = xmlStrdup(sep);
	} else {
		/* Reset flag as XML_SAX2_MAGIC is needed for xmlCreatePushParserCtxt 
		so must be set in the handlers */
		parser->parser->sax->initialized = 1;
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
	int error;

/* The following is a hack to keep BC with PHP 4 while avoiding 
the inifite loop in libxml <= 2.6.17 which occurs when no encoding 
has been defined and none can be detected */
#if LIBXML_VERSION <= 20617
	if (parser->parser->charset == XML_CHAR_ENCODING_NONE) {
		if (data_len >= 4 || (parser->parser->input->buf->buffer->use + data_len >= 4)) {
			xmlChar start[4];
			int char_count;

			char_count = parser->parser->input->buf->buffer->use;
			if (char_count > 4) {
				char_count = 4;
			}

			memcpy(start, parser->parser->input->buf->buffer->content, (size_t)char_count);
			memcpy(start + char_count, data, (size_t)(4 - char_count));

			if (xmlDetectCharEncoding(&start[0], 4) == XML_CHAR_ENCODING_NONE) {
				parser->parser->charset = XML_CHAR_ENCODING_UTF8;
			}
		}
	}
#endif

	error = xmlParseChunk(parser->parser, data, data_len, is_final);
	if (!error) {
		return 1;
	} else if (parser->parser->lastError.level > XML_ERR_WARNING ){
		return 0;
	} else {
		return 1;
	}
}

PHPAPI int
XML_GetErrorCode(XML_Parser parser)
{
	return parser->parser->errNo;
}

static const XML_Char *const error_mapping[] = {
	"No error",
	"Internal error",
	"No memory",
	"Invalid document start",
	"Empty document",
	"Invalid document end",
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
	"PEReference at end of document",
	"PEReference in prolog",
	"PEReference in epilog",
	"PEReference: forbidden within markup decl in internal subset",
	"XML_ERR_ENTITYREF_NO_NAME",
	"EntityRef: expecting ';'",
	"PEReference: no name",
	"PEReference: expecting ';'",
	"Undeclared entity error",
	"Undeclared entity warning",
	"Unparsed Entity",
	"XML_ERR_ENTITY_IS_EXTERNAL",
	"XML_ERR_ENTITY_IS_PARAMETER",
	"Unknown encoding",
	"Unsupported encoding",
	"String not started expecting ' or \"",
	"String not closed expecting \" or '",
	"Namespace declaration error",
	"EntityValue: \" or ' expected",
	"EntityValue: \" or ' expected",
	"< in attribute",
	"Attribute not started",
	"Attribute not finished",
	"Attribute without value",
	"Attribute redefined",
	"SystemLiteral \" or ' expected",
	"SystemLiteral \" or ' expected",
	/* "XML_ERR_COMMENT_NOT_STARTED", <= eliminated on purpose */
	"Comment not finished",
	"Processing Instruction not started",
	"Processing Instruction not finished",
	"NOTATION: Name expected here",
	"'>' required to close NOTATION declaration",
	"'(' required to start ATTLIST enumeration",
	"'(' required to start ATTLIST enumeration",
	"MixedContentDecl : '|' or ')*' expected",
	"XML_ERR_MIXED_NOT_FINISHED",
	"ELEMENT in DTD not started",
	"ELEMENT in DTD not finished",
	"XML declaration not started",
	"XML declaration not finished",
	"XML_ERR_CONDSEC_NOT_STARTED",
	"XML conditional section not closed",
	"Content error in the external subset",
	"DOCTYPE not finished",
	"Sequence ']]>' not allowed in content",
	"CDATA not finished",
	"Reserved XML Name",
	"Space required",
	"XML_ERR_SEPARATOR_REQUIRED",
	"NmToken expected in ATTLIST enumeration",
	"XML_ERR_NAME_REQUIRED",
	"MixedContentDecl : '#PCDATA' expected",
	"SYSTEM or PUBLIC, the URI is missing",
	"PUBLIC, the Public Identifier is missing",
	"< required",
	"> required",
	"</ required",
	"= required",
	"Mismatched tag",
	"Tag not finished",
	"standalone accepts only 'yes' or 'no'",
	"Invalid XML encoding name",
	"Comment must not contain '--' (double-hyphen)",
	"Invalid encoding",
	"external parsed entities cannot be standalone",
	"XML conditional section '[' expected",
	"Entity value required",
	"chunk is not well balanced",
	"extra content at the end of well balanced chunk",
    "XML_ERR_ENTITY_CHAR_ERROR",
    "PEReferences forbidden in internal subset",
    "Detected an entity reference loop",
    "XML_ERR_ENTITY_BOUNDARY",
    "Invalid URI",
    "Fragment not allowed",
    "XML_WAR_CATALOG_PI",
    "XML_ERR_NO_DTD",
    "conditional section INCLUDE or IGNORE keyword expected", /* 95 */
    "Version in XML Declaration missing", /* 96 */
    "XML_WAR_UNKNOWN_VERSION", /* 97 */
    "XML_WAR_LANG_VALUE", /* 98 */
    "XML_WAR_NS_URI", /* 99 */
    "XML_WAR_NS_URI_RELATIVE", /* 100 */
    "Missing encoding in text declaration" /* 101 */
};

PHPAPI const XML_Char *
XML_ErrorString(int code)
{
	if (code < 0 || code >= (int)(sizeof(error_mapping) / sizeof(error_mapping[0]))) {
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
	if (parser->parser->myDoc) {
		xmlFreeDoc(parser->parser->myDoc);
		parser->parser->myDoc = NULL;
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
