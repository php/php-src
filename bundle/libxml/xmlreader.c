/*
 * xmlreader.c: implements the xmlTextReader streaming node API
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */

#define IN_LIBXML
#include "libxml.h"

#include <string.h> /* for memset() only ! */

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <libxml/xmlmemory.h>
#include <libxml/xmlIO.h>
#include <libxml/xmlreader.h>

/* #define DEBUG_CALLBACKS */
/* #define DEBUG_READER */

/**
 * TODO:
 *
 * macro to flag unimplemented blocks
 */
#define TODO 								\
    xmlGenericError(xmlGenericErrorContext,				\
	    "Unimplemented block at %s:%d\n",				\
            __FILE__, __LINE__);

#ifdef DEBUG_READER
#define DUMP_READER xmlTextReaderDebug(reader);
#else
#define DUMP_READER
#endif

/************************************************************************
 *									*
 *	The parser: maps the Text Reader API on top of the existing	*
 *		parsing routines building a tree			*
 *									*
 ************************************************************************/

#define XML_TEXTREADER_INPUT	1
#define XML_TEXTREADER_CTXT	2

typedef enum {
    XML_TEXTREADER_MODE_NORMAL = 0,
    XML_TEXTREADER_MODE_EOF = 1
} xmlTextReaderMode;

typedef enum {
    XML_TEXTREADER_NONE = -1,
    XML_TEXTREADER_START= 0,
    XML_TEXTREADER_ELEMENT= 1,
    XML_TEXTREADER_END= 2,
    XML_TEXTREADER_EMPTY= 3,
    XML_TEXTREADER_BACKTRACK= 4
} xmlTextReaderState;

struct _xmlTextReader {
    int				mode;	/* the parsing mode */
    int				allocs;	/* what structure were deallocated */
    xmlTextReaderState		state;
    xmlParserCtxtPtr		ctxt;	/* the parser context */
    xmlSAXHandlerPtr		sax;	/* the parser SAX callbacks */
    xmlParserInputBufferPtr	input;	/* the input */
    startElementSAXFunc		startElement;/* initial SAX callbacks */
    endElementSAXFunc		endElement;  /* idem */
    unsigned int 		base;	/* base of the segment in the input */
    unsigned int 		cur;	/* current position in the input */
    xmlNodePtr			node;	/* current node */
    int				depth;  /* depth of the current node */
};

#ifdef DEBUG_READER
static void
xmlTextReaderDebug(xmlTextReaderPtr reader) {
    if ((reader == NULL) || (reader->ctxt == NULL)) {
	fprintf(stderr, "xmlTextReader NULL\n");
	return;
    }
    fprintf(stderr, "xmlTextReader: state %d depth %d ",
	    reader->state, reader->depth);
    if (reader->node == NULL) {
	fprintf(stderr, "node = NULL\n");
    } else {
	fprintf(stderr, "node %s\n", reader->node->name);
    }
    fprintf(stderr, "  input: base %d, cur %d, depth %d: ",
	    reader->base, reader->cur, reader->ctxt->nodeNr);
    if (reader->input->buffer == NULL) {
	fprintf(stderr, "buffer is NULL\n");
    } else {
#ifdef LIBXML_DEBUG_ENABLED
	xmlDebugDumpString(stderr,
		&reader->input->buffer->content[reader->cur]);
#endif
	fprintf(stderr, "\n");
    }
}
#endif

/**
 * xmlTextReaderStartElement:
 * @ctx: the user data (XML parser context)
 * @fullname:  The element name, including namespace prefix
 * @atts:  An array of name/value attributes pairs, NULL terminated
 *
 * called when an opening tag has been processed.
 */
static void
xmlTextReaderStartElement(void *ctx, const xmlChar *fullname,
	                  const xmlChar **atts) {
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlTextReaderPtr reader = ctxt->_private;

#ifdef DEBUG_CALLBACKS
    printf("xmlTextReaderStartElement(%s)\n", fullname);
#endif
    if ((reader != NULL) && (reader->startElement != NULL))
	reader->startElement(ctx, fullname, atts);
    reader->state = XML_TEXTREADER_ELEMENT;
}

/**
 * xmlTextReaderEndElement:
 * @ctx: the user data (XML parser context)
 * @fullname:  The element name, including namespace prefix
 *
 * called when an ending tag has been processed.
 */
static void
xmlTextReaderEndElement(void *ctx, const xmlChar *fullname) {
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlTextReaderPtr reader = ctxt->_private;

#ifdef DEBUG_CALLBACKS
    printf("xmlTextReaderEndElement(%s)\n", fullname);
#endif
    if ((reader != NULL) && (reader->endElement != NULL))
	reader->endElement(ctx, fullname);
    if (reader->state == XML_TEXTREADER_ELEMENT)
	reader->state = XML_TEXTREADER_EMPTY;
    else
	reader->state = XML_TEXTREADER_END;
}

/**
 * xmlTextReaderPushData:
 * @reader:  the xmlTextReaderPtr used
 *
 * Push data down the progressive parser until a significant callback
 * got raised.
 *
 * Returns -1 in case of failure, 0 otherwise
 */
static int
xmlTextReaderPushData(xmlTextReaderPtr reader) {
    unsigned int cur = reader->cur;
    xmlBufferPtr inbuf;
    int val;

    if ((reader->input == NULL) || (reader->input->buffer == NULL))
	return(-1);

    reader->state = XML_TEXTREADER_NONE;
    inbuf = reader->input->buffer;
    while (reader->state == XML_TEXTREADER_NONE) {
	if (cur >= inbuf->use) {
	    /*
	     * Refill the buffer unless we are at the end of the stream
	     */
	    if (reader->mode != XML_TEXTREADER_MODE_EOF) {
		val = xmlParserInputBufferRead(reader->input, 4096);
		if (val <= 0) {
		    reader->mode = XML_TEXTREADER_MODE_EOF;
		    return(val);
		}
	    } else 
		break;
	}
	if ((inbuf->content[cur] == '>') || (inbuf->content[cur] == '&')) {
	    cur = cur + 1;
	    val = xmlParseChunk(reader->ctxt,
		          (const char *) &inbuf->content[reader->cur],
			  cur - reader->cur, 0);
	    if (val != 0)
		return(-1);
	    reader->cur = cur;
	    break;
	} else {
	    cur = cur + 1;

	    /*
	     * One may have to force a flush at some point when parsing really
	     * large CDATA sections
	     */
	    if ((cur - reader->cur > 4096) && (reader->base == 0) &&
		(reader->mode == XML_TEXTREADER_MODE_NORMAL)) {
		cur = cur + 1;
		val = xmlParseChunk(reader->ctxt,
			      (const char *) &inbuf->content[reader->cur],
			      cur - reader->cur, 0);
		if (val != 0)
		    return(-1);
		reader->cur = cur;
	    }
	}
    }
    /*
     * Discard the consumed input when needed and possible
     */
    if (reader->mode == XML_TEXTREADER_MODE_NORMAL) {
	if ((reader->cur >= 4096) && (reader->base == 0)) {
	    val = xmlBufferShrink(inbuf, cur);
	    if (val >= 0) {
		reader->cur -= val;
	    }
	}
    }

    /*
     * At the end of the stream signal that the work is done to the Push
     * parser.
     */
    if ((reader->mode == XML_TEXTREADER_MODE_EOF) && (cur >= inbuf->use)) {
	val = xmlParseChunk(reader->ctxt,
		(const char *) &inbuf->content[reader->cur], 0, 1);
    }
    return(0);
}

/**
 * xmlTextReaderRead:
 * @reader:  the xmlTextReaderPtr used
 *
 *  Moves the position of the current instance to the next node in
 *  the stream, exposing its properties.
 *
 *  Returns 1 if the node was read successfully, 0 if there is no more
 *          nodes to read, or -1 in case of error
 */
int
xmlTextReaderRead(xmlTextReaderPtr reader) {
    int val, olddepth;
    xmlTextReaderState oldstate;
    xmlNodePtr oldnode;

    if ((reader == NULL) || (reader->ctxt == NULL))
	return(-1);
    if (reader->ctxt->wellFormed != 1)
	return(-1);

#ifdef DEBUG_READER
    fprintf(stderr, "\nREAD ");
    DUMP_READER
#endif
    if (reader->node == NULL) {
	/*
	 * Initial state
	 */
	do {
	    val = xmlTextReaderPushData(reader);
	    if (val < 0)
		return(-1);
	} while ((reader->ctxt->node == NULL) &&
		 (reader->mode != XML_TEXTREADER_MODE_EOF));
	if (reader->ctxt->node == NULL) {
	    if (reader->ctxt->myDoc != NULL)
		reader->node = reader->ctxt->myDoc->children;
	    if (reader->node == NULL)
		return(-1);
	} else {
	    reader->node = reader->ctxt->node;
	}
	reader->depth = 1;
	return(1);
    }
    oldstate = reader->state;
    olddepth = reader->ctxt->nodeNr;
    oldnode = reader->node;
    /*
     * If we are not backtracking on ancestors or examined nodes,
     * that the parser didn't finished or that we arent at the end
     * of stream, continue processing.
     */
    if (oldstate != XML_TEXTREADER_BACKTRACK) {
	while (((reader->node->children == NULL) ||
		(reader->node->type == XML_ENTITY_REF_NODE) ||
		(reader->node->type == XML_DTD_NODE)) &&
	       (reader->node->next == NULL) &&
	       (reader->ctxt->nodeNr == olddepth) &&
	       (reader->ctxt->instate != XML_PARSER_EOF)) {
	    val = xmlTextReaderPushData(reader);
	    if (val < 0)
		return(-1);
	    if (reader->node == NULL)
		return(0);
	}
	if ((reader->node->children != NULL) &&
	    (reader->node->type != XML_ENTITY_REF_NODE) &&
	    (reader->node->type != XML_DTD_NODE)) {
	    reader->node = reader->node->children;
	    reader->depth++;
	    if ((reader->state != XML_TEXTREADER_ELEMENT) &&
		(reader->state != XML_TEXTREADER_EMPTY))
		reader->state = XML_TEXTREADER_ELEMENT;
	    DUMP_READER
	    return(1);
	}
    }
    if (reader->node->next != NULL) {
	if ((oldstate == XML_TEXTREADER_ELEMENT) &&
            (reader->node->type == XML_ELEMENT_NODE)) {
	    reader->state = XML_TEXTREADER_END;
	    DUMP_READER
	    return(1);
	}
	reader->node = reader->node->next;
	reader->state = XML_TEXTREADER_ELEMENT;
	DUMP_READER
	/*
	 * Cleanup of the old node
	 */
	if (oldnode->type != XML_DTD_NODE) {
	    xmlUnlinkNode(oldnode);
	    xmlFreeNode(oldnode);
	}

	return(1);
    }
    reader->node = reader->node->parent;
    if ((reader->node == NULL) ||
	(reader->node->type == XML_DOCUMENT_NODE) ||
#ifdef LIBXML_DOCB_ENABLED
	(reader->node->type == XML_DOCB_DOCUMENT_NODE) ||
#endif
	(reader->node->type == XML_HTML_DOCUMENT_NODE)) {
	reader->node = NULL;
	reader->depth = 0;

	/*
	 * Cleanup of the old node
	 */
	if (oldnode->type != XML_DTD_NODE) {
	    xmlUnlinkNode(oldnode);
	    xmlFreeNode(oldnode);
	}

	return(0);
    }
    reader->depth--;
    reader->state = XML_TEXTREADER_BACKTRACK;
    DUMP_READER
    return(1);
}

/************************************************************************
 *									*
 *			Constructor and destructors			*
 *									*
 ************************************************************************/
/**
 * xmlNewTextReader:
 * @input: the xmlParserInputBufferPtr used to read data
 *
 * Create an xmlTextReader structure fed with @input
 *
 * Returns the new xmlTextReaderPtr or NULL in case of error
 */
xmlTextReaderPtr
xmlNewTextReader(xmlParserInputBufferPtr input) {
    xmlTextReaderPtr ret;
    int val;

    if (input == NULL)
	return(NULL);
    ret = xmlMalloc(sizeof(xmlTextReader));
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewTextReader : malloc failed\n");
	return(NULL);
    }
    memset(ret, 0, sizeof(xmlTextReader));
    ret->input = input;
    ret->sax = (xmlSAXHandler *) xmlMalloc(sizeof(xmlSAXHandler));
    if (ret->sax == NULL) {
	xmlFree(ret);
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewTextReader : malloc failed\n");
	return(NULL);
    }
    memcpy(ret->sax, &xmlDefaultSAXHandler, sizeof(xmlSAXHandler));
    ret->startElement = ret->sax->startElement;
    ret->sax->startElement = xmlTextReaderStartElement;
    ret->endElement = ret->sax->endElement;
    ret->sax->endElement = xmlTextReaderEndElement;

    ret->mode = XML_TEXTREADER_MODE_NORMAL;
    ret->node = NULL;
    val = xmlParserInputBufferRead(input, 4);
    if (val >= 4) {
	ret->ctxt = xmlCreatePushParserCtxt(ret->sax, NULL,
			(const char *) ret->input->buffer->content, 4, NULL);
	ret->base = 0;
	ret->cur = 4;
    } else {
	ret->ctxt = xmlCreatePushParserCtxt(ret->sax, NULL, NULL, 0, NULL);
	ret->base = 0;
	ret->cur = 0;
    }
    ret->ctxt->_private = ret;
    ret->allocs = XML_TEXTREADER_CTXT;
    return(ret);

}

/**
 * xmlNewTextReaderFilename:
 * @URI: the URI of the resource to process
 *
 * Create an xmlTextReader structure fed with the resource at @URI
 *
 * Returns the new xmlTextReaderPtr or NULL in case of error
 */
xmlTextReaderPtr
xmlNewTextReaderFilename(const char *URI) {
    xmlParserInputBufferPtr input;
    xmlTextReaderPtr ret;

    input = xmlParserInputBufferCreateFilename(URI, XML_CHAR_ENCODING_NONE);
    if (input == NULL)
	return(NULL);
    ret = xmlNewTextReader(input);
    if (ret == NULL) {
	xmlFreeParserInputBuffer(input);
	return(NULL);
    }
    ret->allocs |= XML_TEXTREADER_INPUT;
    return(ret);
}

/**
 * xmlFreeTextReader:
 * @reader:  the xmlTextReaderPtr
 *
 * Deallocate all the resources associated to the reader
 */
void
xmlFreeTextReader(xmlTextReaderPtr reader) {
    if (reader == NULL)
	return;
    if (reader->ctxt != NULL) {
	if (reader->ctxt->myDoc != NULL) {
	    xmlFreeDoc(reader->ctxt->myDoc);
	    reader->ctxt->myDoc = NULL;
	}
	if (reader->allocs & XML_TEXTREADER_CTXT)
	    xmlFreeParserCtxt(reader->ctxt);
    }
    if (reader->sax != NULL)
	xmlFree(reader->sax);
    if ((reader->input != NULL)  && (reader->allocs & XML_TEXTREADER_INPUT))
	xmlFreeParserInputBuffer(reader->input);
    xmlFree(reader);
}

/************************************************************************
 *									*
 *			Acces API to the current node			*
 *									*
 ************************************************************************/
/**
 * xmlTextReaderAttributeCount:
 * @reader:  the xmlTextReaderPtr used
 *
 * Provides the number of attributes of the current node
 *
 * Returns 0 i no attributes, -1 in case of error or the attribute count
 */
int
xmlTextReaderAttributeCount(xmlTextReaderPtr reader) {
    int ret;
    xmlAttrPtr attr;

    if (reader == NULL)
	return(-1);
    if (reader->node == NULL)
	return(0);
    if (reader->node->type != XML_ELEMENT_NODE)
	return(0);
    if ((reader->state == XML_TEXTREADER_END) ||
	(reader->state == XML_TEXTREADER_BACKTRACK))
	return(0);
    ret = 0;
    attr = reader->node->properties;
    while (attr != NULL) {
	ret++;
	attr = attr->next;
    }
    return(ret);
}

/**
 * xmlTextReaderNodeType:
 * @reader:  the xmlTextReaderPtr used
 *
 * Get the node type of the current node
 * Reference:
 * http://dotgnu.org/pnetlib-doc/System/Xml/XmlNodeType.html
 *
 * Returns the xmlNodeType of the current node or -1 in case of error
 */
int
xmlTextReaderNodeType(xmlTextReaderPtr reader) {
    if (reader == NULL)
	return(-1);
    if (reader->node == NULL)
	return(0);
    switch (reader->node->type) {
        case XML_ELEMENT_NODE:
	    if ((reader->state == XML_TEXTREADER_END) ||
		(reader->state == XML_TEXTREADER_BACKTRACK))
		return(15);
	    return(1);
        case XML_ATTRIBUTE_NODE:
	    return(2);
        case XML_TEXT_NODE:
	    return(3); /* TODO: SignificantWhitespace == 14 Whitespace == 13 */
        case XML_CDATA_SECTION_NODE:
	    return(4);
        case XML_ENTITY_REF_NODE:
	    return(5);
        case XML_ENTITY_NODE:
	    return(6);
        case XML_PI_NODE:
	    return(7);
        case XML_COMMENT_NODE:
	    return(8);
        case XML_DOCUMENT_NODE:
        case XML_HTML_DOCUMENT_NODE:
#ifdef LIBXML_DOCB_ENABLED
        case XML_DOCB_DOCUMENT_NODE:
#endif
	    return(9);
        case XML_DOCUMENT_FRAG_NODE:
	    return(11);
        case XML_NOTATION_NODE:
	    return(12);
        case XML_DOCUMENT_TYPE_NODE:
        case XML_DTD_NODE:
	    return(10);

        case XML_ELEMENT_DECL:
        case XML_ATTRIBUTE_DECL:
        case XML_ENTITY_DECL:
        case XML_NAMESPACE_DECL:
        case XML_XINCLUDE_START:
        case XML_XINCLUDE_END:
	    return(0);
    }
    return(-1);
}

/**
 * xmlTextReaderIsEmptyElement:
 * @reader:  the xmlTextReaderPtr used
 *
 * Check if the current node is empty
 *
 * Returns 1 if empty, 0 if not and -1 in case of error
 */
int
xmlTextReaderIsEmptyElement(xmlTextReaderPtr reader) {
    if ((reader == NULL) || (reader->node == NULL))
	return(-1);
    if (reader->node->children != NULL)
	return(0);
    if ((reader->state == XML_TEXTREADER_EMPTY) ||
	(reader->state == XML_TEXTREADER_BACKTRACK))
	return(1);
    return(0);
}

/**
 * xmlTextReaderLocalName:
 * @reader:  the xmlTextReaderPtr used
 *
 * The local name of the node.
 *
 * Returns the local name or NULL if not available
 */
xmlChar *
xmlTextReaderLocalName(xmlTextReaderPtr reader) {
    if ((reader == NULL) || (reader->node == NULL))
	return(NULL);
    if ((reader->node->type != XML_ELEMENT_NODE) &&
	(reader->node->type != XML_ATTRIBUTE_NODE))
	return(xmlTextReaderName(reader));
    return(xmlStrdup(reader->node->name));
}

/**
 * xmlTextReaderName:
 * @reader:  the xmlTextReaderPtr used
 *
 * The qualified name of the node, equal to Prefix :LocalName.
 *
 * Returns the local name or NULL if not available
 */
xmlChar *
xmlTextReaderName(xmlTextReaderPtr reader) {
    xmlChar *ret;

    if ((reader == NULL) || (reader->node == NULL))
	return(NULL);
    switch (reader->node->type) {
        case XML_ELEMENT_NODE:
        case XML_ATTRIBUTE_NODE:
	    if ((reader->node->ns == NULL) ||
		(reader->node->ns->prefix == NULL))
		return(xmlStrdup(reader->node->name));
	    
	    ret = xmlStrdup(reader->node->ns->prefix);
	    ret = xmlStrcat(ret, BAD_CAST ":");
	    ret = xmlStrcat(ret, reader->node->name);
	    return(ret);
        case XML_TEXT_NODE:
	    return(xmlStrdup(BAD_CAST "#text"));
        case XML_CDATA_SECTION_NODE:
	    return(xmlStrdup(BAD_CAST "#cdata-section"));
        case XML_ENTITY_NODE:
        case XML_ENTITY_REF_NODE:
	    return(xmlStrdup(reader->node->name));
        case XML_PI_NODE:
	    return(xmlStrdup(reader->node->name));
        case XML_COMMENT_NODE:
	    return(xmlStrdup(BAD_CAST "#comment"));
        case XML_DOCUMENT_NODE:
        case XML_HTML_DOCUMENT_NODE:
#ifdef LIBXML_DOCB_ENABLED
        case XML_DOCB_DOCUMENT_NODE:
#endif
	    return(xmlStrdup(BAD_CAST "#document"));
        case XML_DOCUMENT_FRAG_NODE:
	    return(xmlStrdup(BAD_CAST "#document-fragment"));
        case XML_NOTATION_NODE:
	    return(xmlStrdup(reader->node->name));
        case XML_DOCUMENT_TYPE_NODE:
        case XML_DTD_NODE:
	    return(xmlStrdup(reader->node->name));

        case XML_ELEMENT_DECL:
        case XML_ATTRIBUTE_DECL:
        case XML_ENTITY_DECL:
        case XML_NAMESPACE_DECL:
        case XML_XINCLUDE_START:
        case XML_XINCLUDE_END:
	    return(NULL);
    }
    return(NULL);
}

/**
 * xmlTextReaderPrefix:
 * @reader:  the xmlTextReaderPtr used
 *
 * A shorthand reference to the namespace associated with the node.
 *
 * Returns the prefix or NULL if not available
 */
xmlChar *
xmlTextReaderPrefix(xmlTextReaderPtr reader) {
    if ((reader == NULL) || (reader->node == NULL))
	return(NULL);
    if ((reader->node->type != XML_ELEMENT_NODE) &&
	(reader->node->type != XML_ATTRIBUTE_NODE))
	return(NULL);
    if ((reader->node->ns != NULL) || (reader->node->ns->prefix != NULL))
	return(xmlStrdup(reader->node->ns->prefix));
    return(NULL);
}

/**
 * xmlTextReaderNamespaceUri:
 * @reader:  the xmlTextReaderPtr used
 *
 * The URI defining the namespace associated with the node.
 *
 * Returns the namespace URI or NULL if not available
 */
xmlChar *
xmlTextReaderNamespaceUri(xmlTextReaderPtr reader) {
    if ((reader == NULL) || (reader->node == NULL))
	return(NULL);
    if ((reader->node->type != XML_ELEMENT_NODE) &&
	(reader->node->type != XML_ATTRIBUTE_NODE))
	return(NULL);
    if (reader->node->ns != NULL)
	return(xmlStrdup(reader->node->ns->href));
    return(NULL);
}

/**
 * xmlTextReaderBaseUri:
 * @reader:  the xmlTextReaderPtr used
 *
 * The base URI of the node.
 *
 * Returns the base URI or NULL if not available
 */
xmlChar *
xmlTextReaderBaseUri(xmlTextReaderPtr reader) {
    if ((reader == NULL) || (reader->node == NULL))
	return(NULL);
    return(xmlNodeGetBase(NULL, reader->node));
}

/**
 * xmlTextReaderDepth:
 * @reader:  the xmlTextReaderPtr used
 *
 * The depth of the node in the tree.
 *
 * Returns the depth or -1 in case of error
 */
int
xmlTextReaderDepth(xmlTextReaderPtr reader) {
    if (reader == NULL)
	return(-1);
    if (reader->node == NULL)
	return(0);

    return(reader->depth);
}

/**
 * xmlTextReaderHasAttributes:
 * @reader:  the xmlTextReaderPtr used
 *
 * Whether the node has attributes.
 *
 * Returns 1 if true, 0 if false, and -1 in case or error
 */
int
xmlTextReaderHasAttributes(xmlTextReaderPtr reader) {
    if (reader == NULL)
	return(-1);
    if (reader->node == NULL)
	return(0);

    if ((reader->node->type == XML_ELEMENT_NODE) &&
	(reader->node->properties != NULL))
	return(1);
    /* TODO: handle the xmlDecl */
    return(0);
}

/**
 * xmlTextReaderHasValue:
 * @reader:  the xmlTextReaderPtr used
 *
 * Whether the node can have a text value.
 *
 * Returns 1 if true, 0 if false, and -1 in case or error
 */
int
xmlTextReaderHasValue(xmlTextReaderPtr reader) {
    if (reader == NULL)
	return(-1);
    if (reader->node == NULL)
	return(0);

    switch (reader->node->type) {
        case XML_ATTRIBUTE_NODE:
        case XML_TEXT_NODE:
        case XML_CDATA_SECTION_NODE:
        case XML_PI_NODE:
        case XML_COMMENT_NODE:
	    return(1);
	default:
	    return(0);
    }
    return(0);
}

/**
 * xmlTextReaderValue:
 * @reader:  the xmlTextReaderPtr used
 *
 * Provides the text value of the node if present
 *
 * Returns the string or NULL if not available. The retsult must be deallocated
 *     with xmlFree()
 */
xmlChar *
xmlTextReaderValue(xmlTextReaderPtr reader) {
    if (reader == NULL)
	return(NULL);
    if (reader->node == NULL)
	return(NULL);

    switch (reader->node->type) {
        case XML_ATTRIBUTE_NODE:{
	    xmlAttrPtr attr = (xmlAttrPtr) reader->node;

	    if (attr->parent != NULL)
		return (xmlNodeListGetString
			(attr->parent->doc, attr->children, 1));
	    else
		return (xmlNodeListGetString(NULL, attr->children, 1));
	    break;
	}
        case XML_TEXT_NODE:
        case XML_CDATA_SECTION_NODE:
        case XML_PI_NODE:
        case XML_COMMENT_NODE:
            if (reader->node->content != NULL)
                return (xmlStrdup(reader->node->content));
	default:
	    return(NULL);
    }
    return(NULL);
}

/**
 * xmlTextReaderIsDefault:
 * @reader:  the xmlTextReaderPtr used
 *
 * Whether an Attribute  node was generated from the default value
 * defined in the DTD or schema.
 *
 * Returns 0 if not defaulted, 1 if defaulted, and -1 in case of error
 */
int
xmlTextReaderIsDefault(xmlTextReaderPtr reader) {
    if (reader == NULL)
	return(-1);
    return(0);
}

/**
 * xmlTextReaderQuoteChar:
 * @reader:  the xmlTextReaderPtr used
 *
 * The quotation mark character used to enclose the value of an attribute.
 *
 * Returns " or ' and -1 in case of error
 */
int
xmlTextReaderQuoteChar(xmlTextReaderPtr reader) {
    if (reader == NULL)
	return(-1);
    /* TODO maybe lookup the attribute value for " first */
    return((int) '"');
}

/**
 * xmlTextReaderXmlLang:
 * @reader:  the xmlTextReaderPtr used
 *
 * The xml:lang scope within which the node resides.
 *
 * Returns the xml:lang value or NULL if none exists.
 */
xmlChar *
xmlTextReaderXmlLang(xmlTextReaderPtr reader) {
    if (reader == NULL)
	return(NULL);
    if (reader->node == NULL)
	return(NULL);
    return(xmlNodeGetLang(reader->node));
}

