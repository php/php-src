/*
 * xmlreader.h : Interfaces, constants and types of the XML streaming API.
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */

#ifndef __XML_XMLREADER_H__
#define __XML_XMLREADER_H__

#include <libxml/tree.h>
#include <libxml/xmlIO.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _xmlTextReader xmlTextReader;
typedef xmlTextReader *xmlTextReaderPtr;

/*
 * Constructors & Destructor
 */
xmlTextReaderPtr	xmlNewTextReader	(xmlParserInputBufferPtr input);
xmlTextReaderPtr	xmlNewTextReaderFilename(const char *URI);
void			xmlFreeTextReader	(xmlTextReaderPtr reader);

/*
 * Iterators
 */
int		xmlTextReaderRead	(xmlTextReaderPtr reader);

/*
 * Attributes of the node
 */
int		xmlTextReaderAttributeCount(xmlTextReaderPtr reader);
xmlChar *	xmlTextReaderBaseUri	(xmlTextReaderPtr reader);
int		xmlTextReaderDepth	(xmlTextReaderPtr reader);
int		xmlTextReaderHasAttributes(xmlTextReaderPtr reader);
int		xmlTextReaderHasValue(xmlTextReaderPtr reader);
int		xmlTextReaderIsDefault	(xmlTextReaderPtr reader);
int		xmlTextReaderIsEmptyElement(xmlTextReaderPtr reader);
xmlChar *	xmlTextReaderLocalName	(xmlTextReaderPtr reader);
xmlChar *	xmlTextReaderName	(xmlTextReaderPtr reader);
xmlChar *	xmlTextReaderNamespaceUri(xmlTextReaderPtr reader);
int		xmlTextReaderNodeType	(xmlTextReaderPtr reader);
xmlChar *	xmlTextReaderPrefix	(xmlTextReaderPtr reader);
int		xmlTextReaderQuoteChar	(xmlTextReaderPtr reader);
xmlChar *	xmlTextReaderValue	(xmlTextReaderPtr reader);
xmlChar *	xmlTextReaderXmlLang	(xmlTextReaderPtr reader);
#ifdef __cplusplus
}
#endif
#endif /* __XML_XMLREADER_H__ */

