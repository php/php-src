/*
 * SAX.h : Default SAX handler interfaces.
 *
 * See Copyright for the status of this software.
 *
 * Daniel Veillard <daniel@veillard.com>
 */


#ifndef __XML_SAX_H__
#define __XML_SAX_H__

#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/xlink.h>

#ifdef __cplusplus
extern "C" {
#endif
const xmlChar *	getPublicId			(void *ctx);
const xmlChar *	getSystemId			(void *ctx);
void		setDocumentLocator		(void *ctx,
						 xmlSAXLocatorPtr loc);
    
int		getLineNumber			(void *ctx);
int		getColumnNumber			(void *ctx);

int		isStandalone			(void *ctx);
int		hasInternalSubset		(void *ctx);
int		hasExternalSubset		(void *ctx);

void		internalSubset			(void *ctx,
						 const xmlChar *name,
						 const xmlChar *ExternalID,
						 const xmlChar *SystemID);
void		externalSubset			(void *ctx,
						 const xmlChar *name,
						 const xmlChar *ExternalID,
						 const xmlChar *SystemID);
xmlEntityPtr	getEntity			(void *ctx,
						 const xmlChar *name);
xmlEntityPtr	getParameterEntity		(void *ctx,
						 const xmlChar *name);
xmlParserInputPtr resolveEntity			(void *ctx,
						 const xmlChar *publicId,
						 const xmlChar *systemId);

void		entityDecl			(void *ctx,
						 const xmlChar *name,
						 int type,
						 const xmlChar *publicId,
						 const xmlChar *systemId,
						 xmlChar *content);
void		attributeDecl			(void *ctx,
						 const xmlChar *elem,
						 const xmlChar *fullname,
						 int type,
						 int def,
						 const xmlChar *defaultValue,
						 xmlEnumerationPtr tree);
void		elementDecl			(void *ctx,
						 const xmlChar *name,
						 int type,
						 xmlElementContentPtr content);
void		notationDecl			(void *ctx,
						 const xmlChar *name,
						 const xmlChar *publicId,
						 const xmlChar *systemId);
void		unparsedEntityDecl		(void *ctx,
						 const xmlChar *name,
						 const xmlChar *publicId,
						 const xmlChar *systemId,
						 const xmlChar *notationName);

void		startDocument			(void *ctx);
void		endDocument			(void *ctx);
void		attribute			(void *ctx,
						 const xmlChar *fullname,
						 const xmlChar *value);
void		startElement			(void *ctx,
						 const xmlChar *fullname,
						 const xmlChar **atts);
void		endElement			(void *ctx,
						 const xmlChar *name);
void		reference			(void *ctx,
						 const xmlChar *name);
void		characters			(void *ctx,
						 const xmlChar *ch,
						 int len);
void		ignorableWhitespace		(void *ctx,
						 const xmlChar *ch,
						 int len);
void		processingInstruction		(void *ctx,
						 const xmlChar *target,
						 const xmlChar *data);
void		globalNamespace			(void *ctx,
						 const xmlChar *href,
						 const xmlChar *prefix);
void		setNamespace			(void *ctx,
						 const xmlChar *name);
xmlNsPtr	getNamespace			(void *ctx);
int		checkNamespace			(void *ctx,
						 xmlChar *nameSpace);
void		namespaceDecl			(void *ctx,
						 const xmlChar *href,
						 const xmlChar *prefix);
void		comment				(void *ctx,
						 const xmlChar *value);
void		cdataBlock			(void *ctx,
						 const xmlChar *value,
						 int len);

void		initxmlDefaultSAXHandler	(xmlSAXHandler *hdlr,
						 int warning);
#ifdef LIBXML_HTML_ENABLED
void		inithtmlDefaultSAXHandler	(xmlSAXHandler *hdlr);
#endif
#ifdef LIBXML_DOCB_ENABLED
void		initdocbDefaultSAXHandler	(xmlSAXHandler *hdlr);
#endif
void		xmlDefaultSAXHandlerInit	(void);
void		htmlDefaultSAXHandlerInit	(void);
void		docbDefaultSAXHandlerInit	(void);
#ifdef __cplusplus
}
#endif
#endif /* __XML_SAX_H__ */
