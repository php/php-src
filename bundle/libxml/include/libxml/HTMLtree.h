/*
 * HTMLtree.h : describes the structures found in an tree resulting
 *              from an XML parsing.
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */

#ifndef __HTML_TREE_H__
#define __HTML_TREE_H__

#include <stdio.h>
#include <libxml/tree.h>
#include <libxml/HTMLparser.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * HTML_TEXT_NODE:
 *
 * Macro. A text node in a HTML document is really implemented
 * the same way as a text node in an XML document.
 */
#define HTML_TEXT_NODE		XML_TEXT_NODE
/**
 * HTML_ENTITY_REF_NODE:
 *
 * Macro. An entity reference in a HTML document is really implemented
 * the same way as an entity reference in an XML document.
 */
#define HTML_ENTITY_REF_NODE	XML_ENTITY_REF_NODE
/**
 * HTML_COMMENT_NODE:
 *
 * Macro. A comment in a HTML document is really implemented
 * the same way as a comment in an XML document.
 */
#define HTML_COMMENT_NODE	XML_COMMENT_NODE
/**
 * HTML_PRESERVE_NODE:
 *
 * Macro. A preserved node in a HTML document is really implemented
 * the same way as a CDATA section in an XML document.
 */
#define HTML_PRESERVE_NODE	XML_CDATA_SECTION_NODE
/**
 * HTML_PI_NODE:
 *
 * Macro. A processing instruction in a HTML document is really implemented
 * the same way as a processing instruction in an XML document.
 */
#define HTML_PI_NODE		XML_PI_NODE

htmlDocPtr	htmlNewDoc		(const xmlChar *URI,
					 const xmlChar *ExternalID);
htmlDocPtr	htmlNewDocNoDtD		(const xmlChar *URI,
					 const xmlChar *ExternalID);
const xmlChar *	htmlGetMetaEncoding	(htmlDocPtr doc);
int		htmlSetMetaEncoding	(htmlDocPtr doc,
					 const xmlChar *encoding);
void		htmlDocDumpMemory	(xmlDocPtr cur,
					 xmlChar **mem,
					 int *size);
int		htmlDocDump		(FILE *f,
					 xmlDocPtr cur);
int		htmlSaveFile		(const char *filename,
					 xmlDocPtr cur);
int		htmlNodeDump		(xmlBufferPtr buf,
					 xmlDocPtr doc,
					 xmlNodePtr cur);
void		htmlNodeDumpFile	(FILE *out,
					 xmlDocPtr doc,
					 xmlNodePtr cur);
int		htmlNodeDumpFileFormat	(FILE *out,
					 xmlDocPtr doc,
					 xmlNodePtr cur,
					 const char *encoding,
					 int format);
int		htmlSaveFileEnc		(const char *filename,
					 xmlDocPtr cur,
					 const char *encoding);
int		htmlSaveFileFormat	(const char *filename,
					 xmlDocPtr cur,
					 const char *encoding,
					 int format);

void		htmlNodeDumpFormatOutput(xmlOutputBufferPtr buf,
					 xmlDocPtr doc,
					 xmlNodePtr cur,
					 const char *encoding,
					 int format);
void		htmlDocContentDumpOutput(xmlOutputBufferPtr buf,
					 xmlDocPtr cur,
					 const char *encoding);
void		htmlDocContentDumpFormatOutput(xmlOutputBufferPtr buf,
					 xmlDocPtr cur,
					 const char *encoding,
					 int format);

int		htmlIsBooleanAttr	(const xmlChar *name);
void htmlNodeDumpOutput(xmlOutputBufferPtr buf, xmlDocPtr doc,
	                xmlNodePtr cur, const char *encoding);




#ifdef __cplusplus
}
#endif

#endif /* __HTML_TREE_H__ */

