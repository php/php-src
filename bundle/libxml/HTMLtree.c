/*
 * HTMLtree.c : implementation of access function for an HTML tree.
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */


#define IN_LIBXML
#include "libxml.h"
#ifdef LIBXML_HTML_ENABLED

#include <string.h> /* for memset() only ! */

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <libxml/xmlmemory.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <libxml/entities.h>
#include <libxml/valid.h>
#include <libxml/xmlerror.h>
#include <libxml/parserInternals.h>
#include <libxml/globals.h>
#include <libxml/uri.h>

/************************************************************************
 *									*
 *   		Getting/Setting encoding meta tags			*
 *									*
 ************************************************************************/

/**
 * htmlGetMetaEncoding:
 * @doc:  the document
 * 
 * Encoding definition lookup in the Meta tags
 *
 * Returns the current encoding as flagged in the HTML source
 */
const xmlChar *
htmlGetMetaEncoding(htmlDocPtr doc) {
    htmlNodePtr cur;
    const xmlChar *content;
    const xmlChar *encoding;

    if (doc == NULL)
	return(NULL);
    cur = doc->children;

    /*
     * Search the html
     */
    while (cur != NULL) {
	if ((cur->type == XML_ELEMENT_NODE) && (cur->name != NULL)) {
	    if (xmlStrEqual(cur->name, BAD_CAST"html"))
		break;
	    if (xmlStrEqual(cur->name, BAD_CAST"head"))
		goto found_head;
	    if (xmlStrEqual(cur->name, BAD_CAST"meta"))
		goto found_meta;
	}
	cur = cur->next;
    }
    if (cur == NULL)
	return(NULL);
    cur = cur->children;

    /*
     * Search the head
     */
    while (cur != NULL) {
	if ((cur->type == XML_ELEMENT_NODE) && (cur->name != NULL)) {
	    if (xmlStrEqual(cur->name, BAD_CAST"head"))
		break;
	    if (xmlStrEqual(cur->name, BAD_CAST"meta"))
		goto found_meta;
	}
	cur = cur->next;
    }
    if (cur == NULL)
	return(NULL);
found_head:
    cur = cur->children;

    /*
     * Search the meta elements
     */
found_meta:
    while (cur != NULL) {
	if ((cur->type == XML_ELEMENT_NODE) && (cur->name != NULL)) {
	    if (xmlStrEqual(cur->name, BAD_CAST"meta")) {
		xmlAttrPtr attr = cur->properties;
		int http;
		const xmlChar *value;

		content = NULL;
		http = 0;
		while (attr != NULL) {
		    if ((attr->children != NULL) &&
		        (attr->children->type == XML_TEXT_NODE) &&
		        (attr->children->next == NULL)) {
			value = attr->children->content;
			if ((!xmlStrcasecmp(attr->name, BAD_CAST"http-equiv"))
			 && (!xmlStrcasecmp(value, BAD_CAST"Content-Type")))
			    http = 1;
			else if ((value != NULL)
			 && (!xmlStrcasecmp(attr->name, BAD_CAST"content")))
			    content = value;
			if ((http != 0) && (content != NULL))
			    goto found_content;
		    }
		    attr = attr->next;
		}
	    }
	}
	cur = cur->next;
    }
    return(NULL);

found_content:
    encoding = xmlStrstr(content, BAD_CAST"charset=");
    if (encoding == NULL) 
	encoding = xmlStrstr(content, BAD_CAST"Charset=");
    if (encoding == NULL) 
	encoding = xmlStrstr(content, BAD_CAST"CHARSET=");
    if (encoding != NULL) {
	encoding += 8;
    } else {
	encoding = xmlStrstr(content, BAD_CAST"charset =");
	if (encoding == NULL) 
	    encoding = xmlStrstr(content, BAD_CAST"Charset =");
	if (encoding == NULL) 
	    encoding = xmlStrstr(content, BAD_CAST"CHARSET =");
	if (encoding != NULL)
	    encoding += 9;
    }
    if (encoding != NULL) {
	while ((*encoding == ' ') || (*encoding == '\t')) encoding++;
    }
    return(encoding);
}

/**
 * htmlSetMetaEncoding:
 * @doc:  the document
 * @encoding:  the encoding string
 * 
 * Sets the current encoding in the Meta tags
 * NOTE: this will not change the document content encoding, just
 * the META flag associated.
 *
 * Returns 0 in case of success and -1 in case of error
 */
int
htmlSetMetaEncoding(htmlDocPtr doc, const xmlChar *encoding) {
    htmlNodePtr cur, meta;
    const xmlChar *content;
    char newcontent[100];


    if (doc == NULL)
	return(-1);

    if (encoding != NULL) {
	snprintf(newcontent, sizeof(newcontent), "text/html; charset=%s",
                encoding);
	newcontent[sizeof(newcontent) - 1] = 0;
    }

    cur = doc->children;

    /*
     * Search the html
     */
    while (cur != NULL) {
	if ((cur->type == XML_ELEMENT_NODE) && (cur->name != NULL)) {
	    if (xmlStrcasecmp(cur->name, BAD_CAST"html") == 0)
		break;
	    if (xmlStrcasecmp(cur->name, BAD_CAST"head") == 0)
		goto found_head;
	    if (xmlStrcasecmp(cur->name, BAD_CAST"meta") == 0)
		goto found_meta;
	}
	cur = cur->next;
    }
    if (cur == NULL)
	return(-1);
    cur = cur->children;

    /*
     * Search the head
     */
    while (cur != NULL) {
	if ((cur->type == XML_ELEMENT_NODE) && (cur->name != NULL)) {
	    if (xmlStrcasecmp(cur->name, BAD_CAST"head") == 0)
		break;
	    if (xmlStrcasecmp(cur->name, BAD_CAST"meta") == 0)
		goto found_meta;
	}
	cur = cur->next;
    }
    if (cur == NULL)
	return(-1);
found_head:
    if (cur->children == NULL) {
	if (encoding == NULL)
	    return(0);
	meta = xmlNewDocNode(doc, NULL, BAD_CAST"meta", NULL);
	xmlAddChild(cur, meta);
	xmlNewProp(meta, BAD_CAST"http-equiv", BAD_CAST"Content-Type");
	xmlNewProp(meta, BAD_CAST"content", BAD_CAST newcontent);
	return(0);
    }
    cur = cur->children;

found_meta:
    if (encoding != NULL) {
	/*
	 * Create a new Meta element with the right attributes
	 */

	meta = xmlNewDocNode(doc, NULL, BAD_CAST"meta", NULL);
	xmlAddPrevSibling(cur, meta);
	xmlNewProp(meta, BAD_CAST"http-equiv", BAD_CAST"Content-Type");
	xmlNewProp(meta, BAD_CAST"content", BAD_CAST newcontent);
    }

    /*
     * Search and destroy all the remaining the meta elements carrying
     * encoding informations
     */
    while (cur != NULL) {
	if ((cur->type == XML_ELEMENT_NODE) && (cur->name != NULL)) {
	    if (xmlStrcasecmp(cur->name, BAD_CAST"meta") == 0) {
		xmlAttrPtr attr = cur->properties;
		int http;
		const xmlChar *value;

		content = NULL;
		http = 0;
		while (attr != NULL) {
		    if ((attr->children != NULL) &&
		        (attr->children->type == XML_TEXT_NODE) &&
		        (attr->children->next == NULL)) {
			value = attr->children->content;
			if ((!xmlStrcasecmp(attr->name, BAD_CAST"http-equiv"))
			 && (!xmlStrcasecmp(value, BAD_CAST"Content-Type")))
			    http = 1;
			else 
                        {
                           if ((value != NULL) && 
				(!xmlStrcasecmp(attr->name, BAD_CAST"content")))
			      content = value;
                        }
		        if ((http != 0) && (content != NULL))
			    break;
		    }
		    attr = attr->next;
		}
		if ((http != 0) && (content != NULL)) {
		    meta = cur;
		    cur = cur->next;
		    xmlUnlinkNode(meta);
                    xmlFreeNode(meta);
		    continue;
		}

	    }
	}
	cur = cur->next;
    }
    return(0);
}

/**
 * booleanHTMLAttrs:
 *
 * These are the HTML attributes which will be output
 * in minimized form, i.e. <option selected="selected"> will be
 * output as <option selected>, as per XSLT 1.0 16.2 "HTML Output Method"
 *
 */
static const char* htmlBooleanAttrs[] = {
  "checked", "compact", "declare", "defer", "disabled", "ismap",
  "multiple", "nohref", "noresize", "noshade", "nowrap", "readonly",
  "selected", NULL
};


/**
 * htmlIsBooleanAttr:
 * @name:  the name of the attribute to check
 *
 * Determine if a given attribute is a boolean attribute.
 * 
 * returns: false if the attribute is not boolean, true otherwise.
 */
int
htmlIsBooleanAttr(const xmlChar *name)
{
    int i = 0;

    while (htmlBooleanAttrs[i] != NULL) {
        if (xmlStrcasecmp((const xmlChar *)htmlBooleanAttrs[i], name) == 0)
            return 1;
        i++;
    }
    return 0;
}

/************************************************************************
 *									*
 *   		Dumping HTML tree content to a simple buffer		*
 *									*
 ************************************************************************/

static int
htmlNodeDumpFormat(xmlBufferPtr buf, xmlDocPtr doc, xmlNodePtr cur,
	           int format);

/**
 * htmlNodeDumpFormat:
 * @buf:  the HTML buffer output
 * @doc:  the document
 * @cur:  the current node
 * @format:  should formatting spaces been added
 *
 * Dump an HTML node, recursive behaviour,children are printed too.
 *
 * Returns the number of byte written or -1 in case of error
 */
static int
htmlNodeDumpFormat(xmlBufferPtr buf, xmlDocPtr doc, xmlNodePtr cur,
	           int format) {
    unsigned int use;
    int ret;
    xmlOutputBufferPtr outbuf;

    if (cur == NULL) {
	return (-1);
    }
    if (buf == NULL) {
	return (-1);
    }
    outbuf = (xmlOutputBufferPtr) xmlMalloc(sizeof(xmlOutputBuffer));
    if (outbuf == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		        "htmlNodeDumpFormat: out of memory!\n");
	return (-1);
    }
    memset(outbuf, 0, (size_t) sizeof(xmlOutputBuffer));
    outbuf->buffer = buf;
    outbuf->encoder = NULL;
    outbuf->writecallback = NULL;
    outbuf->closecallback = NULL;
    outbuf->context = NULL;
    outbuf->written = 0;

    use = buf->use;
    htmlNodeDumpFormatOutput(outbuf, doc, cur, NULL, format);
    xmlFree(outbuf);
    ret = buf->use - use;
    return (ret);
}

/**
 * htmlNodeDump:
 * @buf:  the HTML buffer output
 * @doc:  the document
 * @cur:  the current node
 *
 * Dump an HTML node, recursive behaviour,children are printed too,
 * and formatting returns are added.
 *
 * Returns the number of byte written or -1 in case of error
 */
int
htmlNodeDump(xmlBufferPtr buf, xmlDocPtr doc, xmlNodePtr cur) {
    return(htmlNodeDumpFormat(buf, doc, cur, 1));
}

/**
 * htmlNodeDumpFileFormat:
 * @out:  the FILE pointer
 * @doc:  the document
 * @cur:  the current node
 * @encoding: the document encoding
 * @format:  should formatting spaces been added
 *
 * Dump an HTML node, recursive behaviour,children are printed too.
 *
 * TODO: if encoding == NULL try to save in the doc encoding
 *
 * returns: the number of byte written or -1 in case of failure.
 */
int
htmlNodeDumpFileFormat(FILE *out, xmlDocPtr doc,
	               xmlNodePtr cur, const char *encoding, int format) {
    xmlOutputBufferPtr buf;
    xmlCharEncodingHandlerPtr handler = NULL;
    int ret;

    if (encoding != NULL) {
	xmlCharEncoding enc;

	enc = xmlParseCharEncoding(encoding);
	if (enc != XML_CHAR_ENCODING_UTF8) {
	    handler = xmlFindCharEncodingHandler(encoding);
	    if (handler == NULL)
		return(-1);
	}
    }

    /*
     * Fallback to HTML or ASCII when the encoding is unspecified
     */
    if (handler == NULL)
	handler = xmlFindCharEncodingHandler("HTML");
    if (handler == NULL)
	handler = xmlFindCharEncodingHandler("ascii");

    /* 
     * save the content to a temp buffer.
     */
    buf = xmlOutputBufferCreateFile(out, handler);
    if (buf == NULL) return(0);

    htmlNodeDumpFormatOutput(buf, doc, cur, encoding, format);

    ret = xmlOutputBufferClose(buf);
    return(ret);
}

/**
 * htmlNodeDumpFile:
 * @out:  the FILE pointer
 * @doc:  the document
 * @cur:  the current node
 *
 * Dump an HTML node, recursive behaviour,children are printed too,
 * and formatting returns are added.
 */
void
htmlNodeDumpFile(FILE *out, xmlDocPtr doc, xmlNodePtr cur) {
    htmlNodeDumpFileFormat(out, doc, cur, NULL, 1);
}

/**
 * htmlDocDumpMemory:
 * @cur:  the document
 * @mem:  OUT: the memory pointer
 * @size:  OUT: the memory length
 *
 * Dump an HTML document in memory and return the xmlChar * and it's size.
 * It's up to the caller to free the memory.
 */
void
htmlDocDumpMemory(xmlDocPtr cur, xmlChar**mem, int *size) {
    xmlOutputBufferPtr buf;
    xmlCharEncodingHandlerPtr handler = NULL;
    const char *encoding;

    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"htmlDocDumpMemory : document == NULL\n");
#endif
	*mem = NULL;
	*size = 0;
	return;
    }

    encoding = (const char *) htmlGetMetaEncoding(cur);

    if (encoding != NULL) {
	xmlCharEncoding enc;

	enc = xmlParseCharEncoding(encoding);
	if (enc != cur->charset) {
	    if (cur->charset != XML_CHAR_ENCODING_UTF8) {
		/*
		 * Not supported yet
		 */
		*mem = NULL;
		*size = 0;
		return;
	    }

	    handler = xmlFindCharEncodingHandler(encoding);
	    if (handler == NULL) {
		*mem = NULL;
		*size = 0;
		return;
	    }
	}
    }

    /*
     * Fallback to HTML or ASCII when the encoding is unspecified
     */
    if (handler == NULL)
	handler = xmlFindCharEncodingHandler("HTML");
    if (handler == NULL)
	handler = xmlFindCharEncodingHandler("ascii");

    buf = xmlAllocOutputBuffer(handler);
    if (buf == NULL) {
	*mem = NULL;
	*size = 0;
	return;
    }

    htmlDocContentDumpOutput(buf, cur, NULL);
    xmlOutputBufferFlush(buf);
    if (buf->conv != NULL) {
	*size = buf->conv->use;
	*mem = xmlStrndup(buf->conv->content, *size);
    } else {
	*size = buf->buffer->use;
	*mem = xmlStrndup(buf->buffer->content, *size);
    }
    (void)xmlOutputBufferClose(buf);
}


/************************************************************************
 *									*
 *   		Dumping HTML tree content to an I/O output buffer	*
 *									*
 ************************************************************************/


/**
 * htmlDtdDumpOutput:
 * @buf:  the HTML buffer output
 * @doc:  the document
 * @encoding:  the encoding string
 * 
 * TODO: check whether encoding is needed
 *
 * Dump the HTML document DTD, if any.
 */
static void
htmlDtdDumpOutput(xmlOutputBufferPtr buf, xmlDocPtr doc,
	          const char *encoding ATTRIBUTE_UNUSED) {
    xmlDtdPtr cur = doc->intSubset;

    if (cur == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"htmlDtdDumpOutput : no internal subset\n");
	return;
    }
    xmlOutputBufferWriteString(buf, "<!DOCTYPE ");
    xmlOutputBufferWriteString(buf, (const char *)cur->name);
    if (cur->ExternalID != NULL) {
	xmlOutputBufferWriteString(buf, " PUBLIC ");
	xmlBufferWriteQuotedString(buf->buffer, cur->ExternalID);
	if (cur->SystemID != NULL) {
	    xmlOutputBufferWriteString(buf, " ");
	    xmlBufferWriteQuotedString(buf->buffer, cur->SystemID);
	} 
    }  else if (cur->SystemID != NULL) {
	xmlOutputBufferWriteString(buf, " SYSTEM ");
	xmlBufferWriteQuotedString(buf->buffer, cur->SystemID);
    }
    xmlOutputBufferWriteString(buf, ">\n");
}

/**
 * htmlAttrDumpOutput:
 * @buf:  the HTML buffer output
 * @doc:  the document
 * @cur:  the attribute pointer
 * @encoding:  the encoding string
 *
 * Dump an HTML attribute
 */
static void
htmlAttrDumpOutput(xmlOutputBufferPtr buf, xmlDocPtr doc, xmlAttrPtr cur,
	           const char *encoding ATTRIBUTE_UNUSED) {
    xmlChar *value;

    /*
     * TODO: The html output method should not escape a & character
     *       occurring in an attribute value immediately followed by
     *       a { character (see Section B.7.1 of the HTML 4.0 Recommendation).
     */

    if (cur == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"htmlAttrDumpOutput : property == NULL\n");
	return;
    }
    xmlOutputBufferWriteString(buf, " ");
    xmlOutputBufferWriteString(buf, (const char *)cur->name);
    if ((cur->children != NULL) && (!htmlIsBooleanAttr(cur->name))) {
	value = xmlNodeListGetString(doc, cur->children, 0);
	if (value) {
	    xmlOutputBufferWriteString(buf, "=");
	    if ((!xmlStrcasecmp(cur->name, BAD_CAST "href")) ||
		(!xmlStrcasecmp(cur->name, BAD_CAST "src"))) {
		xmlChar *escaped;
		xmlChar *tmp = value;

		while (IS_BLANK(*tmp)) tmp++;

		escaped = xmlURIEscapeStr(tmp, BAD_CAST"@/:=?;#%&");
		if (escaped != NULL) {
		    xmlBufferWriteQuotedString(buf->buffer, escaped);
		    xmlFree(escaped);
		} else {
		    xmlBufferWriteQuotedString(buf->buffer, value);
		}
	    } else {
		xmlBufferWriteQuotedString(buf->buffer, value);
	    }
	    xmlFree(value);
	} else  {
	    xmlOutputBufferWriteString(buf, "=\"\"");
	}
    }
}

/**
 * htmlAttrListDumpOutput:
 * @buf:  the HTML buffer output
 * @doc:  the document
 * @cur:  the first attribute pointer
 * @encoding:  the encoding string
 *
 * Dump a list of HTML attributes
 */
static void
htmlAttrListDumpOutput(xmlOutputBufferPtr buf, xmlDocPtr doc, xmlAttrPtr cur, const char *encoding) {
    if (cur == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"htmlAttrListDumpOutput : property == NULL\n");
	return;
    }
    while (cur != NULL) {
        htmlAttrDumpOutput(buf, doc, cur, encoding);
	cur = cur->next;
    }
}



/**
 * htmlNodeListDumpOutput:
 * @buf:  the HTML buffer output
 * @doc:  the document
 * @cur:  the first node
 * @encoding:  the encoding string
 * @format:  should formatting spaces been added
 *
 * Dump an HTML node list, recursive behaviour,children are printed too.
 */
static void
htmlNodeListDumpOutput(xmlOutputBufferPtr buf, xmlDocPtr doc,
	               xmlNodePtr cur, const char *encoding, int format) {
    if (cur == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"htmlNodeListDumpOutput : node == NULL\n");
	return;
    }
    while (cur != NULL) {
        htmlNodeDumpFormatOutput(buf, doc, cur, encoding, format);
	cur = cur->next;
    }
}

/**
 * htmlNodeDumpFormatOutput:
 * @buf:  the HTML buffer output
 * @doc:  the document
 * @cur:  the current node
 * @encoding:  the encoding string
 * @format:  should formatting spaces been added
 *
 * Dump an HTML node, recursive behaviour,children are printed too.
 */
void
htmlNodeDumpFormatOutput(xmlOutputBufferPtr buf, xmlDocPtr doc,
	                 xmlNodePtr cur, const char *encoding, int format) {
    const htmlElemDesc * info;

    if (cur == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"htmlNodeDumpFormatOutput : node == NULL\n");
	return;
    }
    /*
     * Special cases.
     */
    if (cur->type == XML_DTD_NODE)
	return;
    if (cur->type == XML_HTML_DOCUMENT_NODE) {
	htmlDocContentDumpOutput(buf, (xmlDocPtr) cur, encoding);
	return;
    }
    if (cur->type == HTML_TEXT_NODE) {
	if (cur->content != NULL) {
	    if (((cur->name == (const xmlChar *)xmlStringText) ||
		 (cur->name != (const xmlChar *)xmlStringTextNoenc)) &&
		((cur->parent == NULL) ||
		 ((xmlStrcasecmp(cur->parent->name, BAD_CAST "script")) &&
		  (xmlStrcasecmp(cur->parent->name, BAD_CAST "style"))))) {
		xmlChar *buffer;

		buffer = xmlEncodeEntitiesReentrant(doc, cur->content);
		if (buffer != NULL) {
		    xmlOutputBufferWriteString(buf, (const char *)buffer);
		    xmlFree(buffer);
		}
	    } else {
		xmlOutputBufferWriteString(buf, (const char *)cur->content);
	    }
	}
	return;
    }
    if (cur->type == HTML_COMMENT_NODE) {
	if (cur->content != NULL) {
	    xmlOutputBufferWriteString(buf, "<!--");
	    xmlOutputBufferWriteString(buf, (const char *)cur->content);
	    xmlOutputBufferWriteString(buf, "-->");
	}
	return;
    }
    if (cur->type == HTML_PI_NODE) {
	if (cur->name == NULL)
	    return;
	xmlOutputBufferWriteString(buf, "<?");
	xmlOutputBufferWriteString(buf, (const char *)cur->name);
	if (cur->content != NULL) {
	    xmlOutputBufferWriteString(buf, " ");
	    xmlOutputBufferWriteString(buf, (const char *)cur->content);
	}
	xmlOutputBufferWriteString(buf, ">");
	return;
    }
    if (cur->type == HTML_ENTITY_REF_NODE) {
        xmlOutputBufferWriteString(buf, "&");
	xmlOutputBufferWriteString(buf, (const char *)cur->name);
        xmlOutputBufferWriteString(buf, ";");
	return;
    }
    if (cur->type == HTML_PRESERVE_NODE) {
	if (cur->content != NULL) {
	    xmlOutputBufferWriteString(buf, (const char *)cur->content);
	}
	return;
    }

    /*
     * Get specific HTML info for that node.
     */
    info = htmlTagLookup(cur->name);

    xmlOutputBufferWriteString(buf, "<");
    xmlOutputBufferWriteString(buf, (const char *)cur->name);
    if (cur->properties != NULL)
        htmlAttrListDumpOutput(buf, doc, cur->properties, encoding);

    if ((info != NULL) && (info->empty)) {
        xmlOutputBufferWriteString(buf, ">");
	if ((format) && (!info->isinline) && (cur->next != NULL)) {
	    if ((cur->next->type != HTML_TEXT_NODE) &&
		(cur->next->type != HTML_ENTITY_REF_NODE) &&
		(cur->parent != NULL) &&
		(!xmlStrEqual(cur->parent->name, BAD_CAST "pre")))
		xmlOutputBufferWriteString(buf, "\n");
	}
	return;
    }
    if (((cur->type == XML_ELEMENT_NODE) || (cur->content == NULL)) &&
	(cur->children == NULL)) {
        if ((info != NULL) && (info->saveEndTag != 0) &&
	    (xmlStrcmp(BAD_CAST info->name, BAD_CAST "html")) &&
	    (xmlStrcmp(BAD_CAST info->name, BAD_CAST "body"))) {
	    xmlOutputBufferWriteString(buf, ">");
	} else {
	    xmlOutputBufferWriteString(buf, "></");
	    xmlOutputBufferWriteString(buf, (const char *)cur->name);
	    xmlOutputBufferWriteString(buf, ">");
	}
	if ((format) && (cur->next != NULL) &&
            (info != NULL) && (!info->isinline)) {
	    if ((cur->next->type != HTML_TEXT_NODE) &&
		(cur->next->type != HTML_ENTITY_REF_NODE) &&
		(cur->parent != NULL) &&
		(!xmlStrEqual(cur->parent->name, BAD_CAST "pre")))
		xmlOutputBufferWriteString(buf, "\n");
	}
	return;
    }
    xmlOutputBufferWriteString(buf, ">");
    if ((cur->type != XML_ELEMENT_NODE) &&
	(cur->content != NULL)) {
	    /*
	     * Uses the OutputBuffer property to automatically convert
	     * invalids to charrefs
	     */

            xmlOutputBufferWriteString(buf, (const char *) cur->content);
    }
    if (cur->children != NULL) {
        if ((format) && (info != NULL) && (!info->isinline) &&
	    (cur->children->type != HTML_TEXT_NODE) &&
	    (cur->children->type != HTML_ENTITY_REF_NODE) &&
	    (cur->children != cur->last) &&
	    (!xmlStrEqual(cur->name, BAD_CAST "pre")))
	    xmlOutputBufferWriteString(buf, "\n");
	htmlNodeListDumpOutput(buf, doc, cur->children, encoding, format);
        if ((format) && (info != NULL) && (!info->isinline) &&
	    (cur->last->type != HTML_TEXT_NODE) &&
	    (cur->last->type != HTML_ENTITY_REF_NODE) &&
	    (cur->children != cur->last) &&
	    (!xmlStrEqual(cur->name, BAD_CAST "pre")))
	    xmlOutputBufferWriteString(buf, "\n");
    }
    xmlOutputBufferWriteString(buf, "</");
    xmlOutputBufferWriteString(buf, (const char *)cur->name);
    xmlOutputBufferWriteString(buf, ">");
    if ((format) && (info != NULL) && (!info->isinline) &&
	(cur->next != NULL)) {
        if ((cur->next->type != HTML_TEXT_NODE) &&
	    (cur->next->type != HTML_ENTITY_REF_NODE) &&
	    (cur->parent != NULL) &&
	    (!xmlStrEqual(cur->parent->name, BAD_CAST "pre")))
	    xmlOutputBufferWriteString(buf, "\n");
    }
}

/**
 * htmlNodeDumpOutput:
 * @buf:  the HTML buffer output
 * @doc:  the document
 * @cur:  the current node
 * @encoding:  the encoding string
 *
 * Dump an HTML node, recursive behaviour,children are printed too,
 * and formatting returns/spaces are added.
 */
void
htmlNodeDumpOutput(xmlOutputBufferPtr buf, xmlDocPtr doc,
	           xmlNodePtr cur, const char *encoding) {
    htmlNodeDumpFormatOutput(buf, doc, cur, encoding, 1);
}

/**
 * htmlDocContentDumpFormatOutput:
 * @buf:  the HTML buffer output
 * @cur:  the document
 * @encoding:  the encoding string
 * @format:  should formatting spaces been added
 *
 * Dump an HTML document.
 */
void
htmlDocContentDumpFormatOutput(xmlOutputBufferPtr buf, xmlDocPtr cur,
	                       const char *encoding, int format) {
    int type;

    /*
     * force to output the stuff as HTML, especially for entities
     */
    type = cur->type;
    cur->type = XML_HTML_DOCUMENT_NODE;
    if (cur->intSubset != NULL) {
        htmlDtdDumpOutput(buf, cur, NULL);
    }
    if (cur->children != NULL) {
        htmlNodeListDumpOutput(buf, cur, cur->children, encoding, format);
    }
    xmlOutputBufferWriteString(buf, "\n");
    cur->type = (xmlElementType) type;
}

/**
 * htmlDocContentDumpOutput:
 * @buf:  the HTML buffer output
 * @cur:  the document
 * @encoding:  the encoding string
 *
 * Dump an HTML document. Formating return/spaces are added.
 */
void
htmlDocContentDumpOutput(xmlOutputBufferPtr buf, xmlDocPtr cur,
	                 const char *encoding) {
    htmlDocContentDumpFormatOutput(buf, cur, encoding, 1);
}

/************************************************************************
 *									*
 *		Saving functions front-ends				*
 *									*
 ************************************************************************/

/**
 * htmlDocDump:
 * @f:  the FILE*
 * @cur:  the document
 *
 * Dump an HTML document to an open FILE.
 *
 * returns: the number of byte written or -1 in case of failure.
 */
int
htmlDocDump(FILE *f, xmlDocPtr cur) {
    xmlOutputBufferPtr buf;
    xmlCharEncodingHandlerPtr handler = NULL;
    const char *encoding;
    int ret;

    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"htmlDocDump : document == NULL\n");
#endif
	return(-1);
    }

    encoding = (const char *) htmlGetMetaEncoding(cur);

    if (encoding != NULL) {
	xmlCharEncoding enc;

	enc = xmlParseCharEncoding(encoding);
	if (enc != cur->charset) {
	    if (cur->charset != XML_CHAR_ENCODING_UTF8) {
		/*
		 * Not supported yet
		 */
		return(-1);
	    }

	    handler = xmlFindCharEncodingHandler(encoding);
	    if (handler == NULL)
		return(-1);
	}
    }

    /*
     * Fallback to HTML or ASCII when the encoding is unspecified
     */
    if (handler == NULL)
	handler = xmlFindCharEncodingHandler("HTML");
    if (handler == NULL)
	handler = xmlFindCharEncodingHandler("ascii");

    buf = xmlOutputBufferCreateFile(f, handler);
    if (buf == NULL) return(-1);
    htmlDocContentDumpOutput(buf, cur, NULL);

    ret = xmlOutputBufferClose(buf);
    return(ret);
}

/**
 * htmlSaveFile:
 * @filename:  the filename (or URL)
 * @cur:  the document
 *
 * Dump an HTML document to a file. If @filename is "-" the stdout file is
 * used.
 * returns: the number of byte written or -1 in case of failure.
 */
int
htmlSaveFile(const char *filename, xmlDocPtr cur) {
    xmlOutputBufferPtr buf;
    xmlCharEncodingHandlerPtr handler = NULL;
    const char *encoding;
    int ret;

    encoding = (const char *) htmlGetMetaEncoding(cur);

    if (encoding != NULL) {
	xmlCharEncoding enc;

	enc = xmlParseCharEncoding(encoding);
	if (enc != cur->charset) {
	    if (cur->charset != XML_CHAR_ENCODING_UTF8) {
		/*
		 * Not supported yet
		 */
		return(-1);
	    }

	    handler = xmlFindCharEncodingHandler(encoding);
	    if (handler == NULL)
		return(-1);
	}
    }

    /*
     * Fallback to HTML or ASCII when the encoding is unspecified
     */
    if (handler == NULL)
	handler = xmlFindCharEncodingHandler("HTML");
    if (handler == NULL)
	handler = xmlFindCharEncodingHandler("ascii");

    /* 
     * save the content to a temp buffer.
     */
    buf = xmlOutputBufferCreateFilename(filename, handler, cur->compression);
    if (buf == NULL) return(0);

    htmlDocContentDumpOutput(buf, cur, NULL);

    ret = xmlOutputBufferClose(buf);
    return(ret);
}

/**
 * htmlSaveFileFormat:
 * @filename:  the filename
 * @cur:  the document
 * @format:  should formatting spaces been added
 * @encoding: the document encoding
 *
 * Dump an HTML document to a file using a given encoding.
 * 
 * returns: the number of byte written or -1 in case of failure.
 */
int
htmlSaveFileFormat(const char *filename, xmlDocPtr cur,
	           const char *encoding, int format) {
    xmlOutputBufferPtr buf;
    xmlCharEncodingHandlerPtr handler = NULL;
    int ret;

    if (encoding != NULL) {
	xmlCharEncoding enc;

	enc = xmlParseCharEncoding(encoding);
	if (enc != cur->charset) {
	    if (cur->charset != XML_CHAR_ENCODING_UTF8) {
		/*
		 * Not supported yet
		 */
		return(-1);
	    }

	    handler = xmlFindCharEncodingHandler(encoding);
	    if (handler == NULL)
		return(-1);
            htmlSetMetaEncoding(cur, (const xmlChar *) encoding);
	}
    } else {
	htmlSetMetaEncoding(cur, (const xmlChar *) "UTF-8");
    }

    /*
     * Fallback to HTML or ASCII when the encoding is unspecified
     */
    if (handler == NULL)
	handler = xmlFindCharEncodingHandler("HTML");
    if (handler == NULL)
	handler = xmlFindCharEncodingHandler("ascii");

    /* 
     * save the content to a temp buffer.
     */
    buf = xmlOutputBufferCreateFilename(filename, handler, 0);
    if (buf == NULL) return(0);

    htmlDocContentDumpFormatOutput(buf, cur, encoding, format);

    ret = xmlOutputBufferClose(buf);
    return(ret);
}

/**
 * htmlSaveFileEnc:
 * @filename:  the filename
 * @cur:  the document
 * @encoding: the document encoding
 *
 * Dump an HTML document to a file using a given encoding
 * and formatting returns/spaces are added.
 * 
 * returns: the number of byte written or -1 in case of failure.
 */
int
htmlSaveFileEnc(const char *filename, xmlDocPtr cur, const char *encoding) {
    return(htmlSaveFileFormat(filename, cur, encoding, 1));
}



#endif /* LIBXML_HTML_ENABLED */
