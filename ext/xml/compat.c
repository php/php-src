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
   | Authors: Sterling Hughes <sterling@php.net>                          |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "expat_compat.h"

#ifdef LIBXML_EXPAT_COMPAT

#define IS_NS_DECL(__ns) \
	((__ns) != NULL && strlen(__ns) == 5 && *(__ns) == 'x' && *((__ns)+1) == 'm' && \
	 *((__ns)+2) == 'l' && *((__ns)+3) == 'n' && *((__ns)+4) == 's')

static void
_find_namespace_decl(XML_Parser parser, const xmlChar *tagname, const xmlChar **attr)
{
	xmlChar **attr_p = (xmlChar **) attr;
	xmlChar  *name;
	xmlChar  *value;
	xmlChar  *partial;
	xmlChar  *namespace;

	while (attr_p && *attr_p) {
		name = attr_p[0];
		value = xmlStrdup(attr_p[1]);

		partial = xmlSplitQName(parser->parser, name, &namespace);
		if (IS_NS_DECL(namespace)) {
			if (parser->h_start_ns) {
				parser->h_start_ns(parser->user, partial, (const XML_Char *) value);
			}
			xmlHashAddEntry(parser->_ns_map, partial, value);
			xmlHashAddEntry(parser->_reverse_ns_map, tagname, xmlStrdup(partial));
			break;
		}

		xmlFree(value);
		attr_p += 2;
	}
}

static void 
_qualify_namespace(XML_Parser parser, const xmlChar *name, xmlChar **qualified)
{
	xmlChar *partial;
	xmlChar *namespace;
	int      len;
		
	partial = xmlSplitQName(parser->parser, name, &namespace);
	if (namespace) {
		xmlChar *nsvalue;

		nsvalue = xmlHashLookup(parser->_ns_map, namespace);
		if (nsvalue) {
			len = strlen(nsvalue) + strlen(partial) + 1; /* colon */
			*qualified = malloc(len+1);
			memcpy(*qualified, nsvalue, strlen(nsvalue));
			memcpy(*qualified + strlen(nsvalue), ":", 1);
			memcpy(*qualified + strlen(nsvalue) + 1, partial, strlen(partial));
			(*qualified)[len] = '\0';
		} else {
			*qualified = xmlStrdup(name);
		}
	} else {
		*qualified = xmlStrdup(name);
	}
}

static void
_start_element_handler(void *user, const xmlChar *name, const xmlChar **attributes)
{
	XML_Parser  parser = (XML_Parser) user;
	xmlChar    *qualified_name = NULL;

	if (parser->namespace) {
		_find_namespace_decl(parser, name, attributes);
		_qualify_namespace(parser, name, &qualified_name);
	} else {
		qualified_name = xmlStrdup(name);
	}

	parser->h_start_element(parser->user, (const XML_Char *) qualified_name, (const XML_Char **) attributes);

	xmlFree(qualified_name);
}

static void
_end_element_handler(void *user, const xmlChar *name)
{
	xmlChar    *qualified_name;
	XML_Parser  parser = (XML_Parser) user;

	if (parser->namespace) {
		xmlChar *nsname;

		nsname = xmlHashLookup(parser->_reverse_ns_map, name);
		if (nsname && parser->h_end_ns) {
			parser->h_end_ns(parser->user, nsname);
		}
        
		_qualify_namespace(parser, name, &qualified_name);
	} else {
		qualified_name = xmlStrdup(name);
	}

	parser->h_end_element(parser->user, (const XML_Char *) qualified_name);

	xmlFree(qualified_name);
}

static void
_cdata_handler(void *user, const xmlChar *cdata, int cdata_len)
{
	XML_Parser parser = (XML_Parser) user;

	parser->h_cdata(parser->user, (const XML_Char *) cdata, cdata_len);
}

static void
_pi_handler(void *user, const xmlChar *target, const xmlChar *data)
{
	XML_Parser parser = (XML_Parser) user;

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

	parser->h_unparsed_entity_decl(parser->user, name, NULL, sys_id, pub_id, notation);
}

static void
_notation_decl_handler(void *user, const xmlChar *notation, const xmlChar *sys_id, const xmlChar *pub_id)
{
	XML_Parser parser = (XML_Parser) user;

	parser->h_notation_decl(parser->user, notation, NULL, sys_id, pub_id);
}

static void
_external_entity_ref_handler(void *user, const xmlChar *names, int type, const xmlChar *sys_id, const xmlChar *pub_id, xmlChar *content)
{
	XML_Parser parser = (XML_Parser) user;

	parser->h_external_entity_ref(parser->user, names, NULL, sys_id, pub_id);
}

static xmlSAXHandler 
php_xml_compat_handlers = {
    NULL, /* internalSubset */
	NULL, /* isStandalone */
	NULL, /* hasInternalSubset */
	NULL, /* hasExternalSubset */
	NULL, /* resolveEntity */
	NULL, /* getEntity */
	_external_entity_ref_handler, /* entityDecl */
	_notation_decl_handler,
	NULL, /* attributeDecl */
	NULL, /* elementDecl */
	_unparsed_entity_decl_handler, /* unparsedEntity */
	NULL, /* setDocumentLocator */
	NULL, /* startDocument */
	NULL, /* endDocument */
	_start_element_handler,
	_end_element_handler,
	NULL, /* reference */
	_cdata_handler,
	NULL, /* ignorableWhitespace */
	_pi_handler,
	NULL, /* comment */
	NULL, /* warning */
	NULL, /* error */
	NULL /* fatalError */
};

XML_Parser 
XML_ParserCreate(const XML_Char *encoding)
{
	XML_Parser parser;

	parser = (XML_Parser) calloc(1, sizeof(struct _XML_Parser));
	parser->parser = xmlCreatePushParserCtxt((xmlSAXHandlerPtr) &php_xml_compat_handlers, (void *) parser, NULL, 0, NULL);
	if (parser->parser == NULL) {
		return NULL;
	}
	parser->parser->encoding = xmlStrdup(encoding);

	return parser;
}

XML_Parser
XML_ParserCreateNS(const XML_Char *encoding, const XML_Char sep)
{
	XML_Parser parser;

	parser = (XML_Parser) calloc(1, sizeof(struct _XML_Parser));
	parser->parser = xmlCreatePushParserCtxt((xmlSAXHandlerPtr) &php_xml_compat_handlers, (void *) parser, NULL, 0, NULL);
	if (parser->parser == NULL) {
		return NULL;
	}
	parser->parser->encoding = xmlStrdup(encoding);
	parser->namespace = 1;
	parser->_ns_map = xmlHashCreate(10);
	parser->_reverse_ns_map = xmlHashCreate(10);

	return parser;
}

void
XML_SetUserData(XML_Parser parser, void *user)
{
	parser->user = user;
}

void *
XML_GetUserData(XML_Parser parser)
{
	return parser->user;
}

void
XML_SetElementHandler(XML_Parser parser, XML_StartElementHandler start, XML_EndElementHandler end)
{
	parser->h_start_element = start;
	parser->h_end_element = end;
}

void
XML_SetCharacterDataHandler(XML_Parser parser, XML_CharacterDataHandler cdata)
{
	parser->h_cdata = cdata;
}

void
XML_SetProcessingInstructionHandler(XML_Parser parser, XML_ProcessingInstructionHandler pi)
{
	parser->h_pi = pi;
}

void 
XML_SetDefaultHandler(XML_Parser parser, XML_DefaultHandler d)
{
	parser->h_default = d;
}

void
XML_SetUnparsedEntityDeclHandler(XML_Parser parser, XML_UnparsedEntityDeclHandler unparsed_decl)
{
	parser->h_unparsed_entity_decl = unparsed_decl;
}

void
XML_SetNotationDeclHandler(XML_Parser parser, XML_NotationDeclHandler notation_decl)
{
	parser->h_notation_decl = notation_decl;
}

void
XML_SetExternalEntityRefHandler(XML_Parser parser, XML_ExternalEntityRefHandler ext_entity)
{
	parser->h_external_entity_ref = ext_entity;
}

void
XML_SetStartNamespaceDeclHandler(XML_Parser parser, XML_StartNamespaceDeclHandler start_ns)
{
	parser->h_start_ns = start_ns;
}

void
XML_SetEndNamespaceDeclHandler(XML_Parser parser, XML_EndNamespaceDeclHandler end_ns)
{
	parser->h_end_ns = end_ns;
}

int
XML_Parse(XML_Parser parser, const XML_Char *data, int data_len, int is_final)
{
	return !xmlParseChunk(parser->parser, data, data_len, is_final);
}

int
XML_GetErrorCode(XML_Parser parser)
{
	return parser->parser->errNo;
}

const XML_Char *error_mapping[] = {
	"Unknown Error"
};

const XML_Char *
XML_ErrorString(int code)
{
	return error_mapping[code];
}

int
XML_GetCurrentLineNumber(XML_Parser parser)
{
	return parser->parser->input->line;
}

int
XML_GetCurrentColumnNumber(XML_Parser parser)
{
	return parser->parser->input->col;
}

int
XML_GetCurrentByteIndex(XML_Parser parser)
{
	return parser->parser->input->consumed;
}

const XML_Char *XML_ExpatVersion(void)
{
	return "1.0";
}

static void
_free_ns_name(void *ptr, xmlChar *name)
{
	xmlFree(ptr);
}

void
XML_ParserFree(XML_Parser parser)
{
	if (parser->namespace) {
		xmlHashFree(parser->_ns_map,         _free_ns_name);
		xmlHashFree(parser->_reverse_ns_map, _free_ns_name);
	}
	xmlFreeParserCtxt(parser->parser);
	free(parser);
}

#endif /* LIBXML_EXPAT_COMPAT */

/**
 * Local Variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: fdm=marker
 * vim: ts=4 noet sw=4
 */
