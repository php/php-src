/*
 * xinclude.c : Code to implement XInclude processing
 *
 * World Wide Web Consortium W3C Last Call Working Draft 16 May 2001
 * http://www.w3.org/TR/2001/WD-xinclude-20010516/
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */

/*
 * TODO: compute XPointers nodesets
 * TODO: add an node intermediate API and handle recursion at this level
 */

#define IN_LIBXML
#include "libxml.h"

#include <string.h>
#include <libxml/xmlmemory.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/uri.h>
#include <libxml/xpointer.h>
#include <libxml/parserInternals.h>
#include <libxml/xmlerror.h>
#include <libxml/encoding.h>
#include <libxml/globals.h>

#ifdef LIBXML_XINCLUDE_ENABLED
#include <libxml/xinclude.h>

#define XINCLUDE_NS (const xmlChar *) "http://www.w3.org/2001/XInclude"
#define XINCLUDE_NODE (const xmlChar *) "include"
#define XINCLUDE_FALLBACK (const xmlChar *) "fallback"
#define XINCLUDE_HREF (const xmlChar *) "href"
#define XINCLUDE_PARSE (const xmlChar *) "parse"
#define XINCLUDE_PARSE_XML (const xmlChar *) "xml"
#define XINCLUDE_PARSE_TEXT (const xmlChar *) "text"
#define XINCLUDE_PARSE_ENCODING (const xmlChar *) "encoding"

/* #define DEBUG_XINCLUDE  */
#ifdef DEBUG_XINCLUDE
#ifdef LIBXML_DEBUG_ENABLED
#include <libxml/debugXML.h>
#endif
#endif

/************************************************************************
 *									*
 *			XInclude contexts handling			*
 *									*
 ************************************************************************/

/*
 * An XInclude context
 */
typedef xmlChar *xmlURL;

typedef struct _xmlXIncludeRef xmlXIncludeRef;
typedef xmlXIncludeRef *xmlXIncludeRefPtr;
struct _xmlXIncludeRef {
    xmlChar              *URI; /* the rully resolved resource URL */
    xmlChar         *fragment; /* the fragment in the URI */
    xmlDocPtr		  doc; /* the parsed document */
    xmlNodePtr            ref; /* the node making the reference in the source */
    xmlNodePtr            inc; /* the included copy */
    int                   xml; /* xml or txt */
    int                 count; /* how many refs use that specific doc */
};

typedef struct _xmlXIncludeCtxt xmlXIncludeCtxt;
typedef xmlXIncludeCtxt *xmlXIncludeCtxtPtr;
struct _xmlXIncludeCtxt {
    xmlDocPtr             doc; /* the source document */
    int               incBase; /* the first include for this document */
    int                 incNr; /* number of includes */
    int                incMax; /* size of includes tab */
    xmlXIncludeRefPtr *incTab; /* array of included references */

    int                 txtNr; /* number of unparsed documents */
    int                txtMax; /* size of unparsed documents tab */
    xmlNodePtr        *txtTab; /* array of unparsed text nodes */
    xmlURL         *txturlTab; /* array of unparsed txtuments URLs */
};

static int
xmlXIncludeDoProcess(xmlXIncludeCtxtPtr ctxt, xmlDocPtr doc);

/**
 * xmlXIncludeFreeRef:
 * @ref: the XInclude reference
 *
 * Free an XInclude reference
 */
static void
xmlXIncludeFreeRef(xmlXIncludeRefPtr ref) {
    if (ref == NULL)
	return;
#ifdef DEBUG_XINCLUDE
    xmlGenericError(xmlGenericErrorContext, "Freeing ref\n");
#endif
    if (ref->doc != NULL) {
#ifdef DEBUG_XINCLUDE
	xmlGenericError(xmlGenericErrorContext, "Freeing doc %s\n", ref->URI);
#endif
	xmlFreeDoc(ref->doc);
    }
    if (ref->URI != NULL)
	xmlFree(ref->URI);
    if (ref->fragment != NULL)
	xmlFree(ref->fragment);
    xmlFree(ref);
}

/**
 * xmlXIncludeNewRef:
 * @ctxt: the XInclude context
 * @URI:  the resource URI
 *
 * Creates a new reference within an XInclude context
 *
 * Returns the new set
 */
static xmlXIncludeRefPtr
xmlXIncludeNewRef(xmlXIncludeCtxtPtr ctxt, const xmlChar *URI,
	          xmlNodePtr ref) {
    xmlXIncludeRefPtr ret;

#ifdef DEBUG_XINCLUDE
    xmlGenericError(xmlGenericErrorContext, "New ref %s\n", URI);
#endif
    ret = (xmlXIncludeRefPtr) xmlMalloc(sizeof(xmlXIncludeRef));
    if (ret == NULL)
	return(NULL);
    memset(ret, 0, sizeof(xmlXIncludeRef));
    if (URI == NULL)
	ret->URI = NULL;
    else
	ret->URI = xmlStrdup(URI);
    ret->fragment = NULL;
    ret->ref = ref;
    ret->doc = 0;
    ret->count = 0;
    ret->xml = 0;
    ret->inc = NULL;
    if (ctxt->incMax == 0) {
	ctxt->incMax = 4;
        ctxt->incTab = (xmlXIncludeRefPtr *) xmlMalloc(ctxt->incMax *
					      sizeof(ctxt->incTab[0]));
        if (ctxt->incTab == NULL) {
	    xmlGenericError(xmlGenericErrorContext,
		    "malloc failed !\n");
	    xmlXIncludeFreeRef(ret);
	    return(NULL);
	}
    }
    if (ctxt->incNr >= ctxt->incMax) {
	ctxt->incMax *= 2;
        ctxt->incTab = (xmlXIncludeRefPtr *) xmlRealloc(ctxt->incTab,
	             ctxt->incMax * sizeof(ctxt->incTab[0]));
        if (ctxt->incTab == NULL) {
	    xmlGenericError(xmlGenericErrorContext,
		    "realloc failed !\n");
	    xmlXIncludeFreeRef(ret);
	    return(NULL);
	}
    }
    ctxt->incTab[ctxt->incNr++] = ret;
    return(ret);
}

/**
 * xmlXIncludeNewContext:
 * @doc:  an XML Document
 *
 * Creates a new XInclude context
 *
 * Returns the new set
 */
static xmlXIncludeCtxtPtr
xmlXIncludeNewContext(xmlDocPtr doc) {
    xmlXIncludeCtxtPtr ret;

#ifdef DEBUG_XINCLUDE
    xmlGenericError(xmlGenericErrorContext, "New context\n");
#endif
    if (doc == NULL)
	return(NULL);
    ret = (xmlXIncludeCtxtPtr) xmlMalloc(sizeof(xmlXIncludeCtxt));
    if (ret == NULL)
	return(NULL);
    memset(ret, 0, sizeof(xmlXIncludeCtxt));
    ret->doc = doc;
    ret->incNr = 0;
    ret->incBase = 0;
    ret->incMax = 0;
    ret->incTab = NULL;
    return(ret);
}

/**
 * xmlXIncludeFreeContext:
 * @ctxt: the XInclude context
 *
 * Free an XInclude context
 */
static void
xmlXIncludeFreeContext(xmlXIncludeCtxtPtr ctxt) {
    int i;

#ifdef DEBUG_XINCLUDE
    xmlGenericError(xmlGenericErrorContext, "Freeing context\n");
#endif
    if (ctxt == NULL)
	return;
    for (i = 0;i < ctxt->incNr;i++) {
	if (ctxt->incTab[i] != NULL)
	    xmlXIncludeFreeRef(ctxt->incTab[i]);
    }
    for (i = 0;i < ctxt->txtNr;i++) {
	if (ctxt->txturlTab[i] != NULL)
	    xmlFree(ctxt->txturlTab[i]);
    }
    if (ctxt->incTab != NULL)
	xmlFree(ctxt->incTab);
    if (ctxt->txtTab != NULL)
	xmlFree(ctxt->txtTab);
    if (ctxt->txturlTab != NULL)
	xmlFree(ctxt->txturlTab);
    xmlFree(ctxt);
}

/**
 * xmlXIncludeAddNode:
 * @ctxt:  the XInclude context
 * @cur:  the new node
 * 
 * Add a new node to process to an XInclude context
 */
static int
xmlXIncludeAddNode(xmlXIncludeCtxtPtr ctxt, xmlNodePtr cur) {
    xmlXIncludeRefPtr ref;
    xmlURIPtr uri;
    xmlChar *URL;
    xmlChar *fragment = NULL;
    xmlChar *href;
    xmlChar *parse;
    xmlChar *base;
    xmlChar *URI;
    int xml = 1; /* default Issue 64 */


    if (ctxt == NULL)
	return(-1);
    if (cur == NULL)
	return(-1);

#ifdef DEBUG_XINCLUDE
    xmlGenericError(xmlGenericErrorContext, "Add node\n");
#endif
    /*
     * read the attributes
     */
    href = xmlGetNsProp(cur, XINCLUDE_NS, XINCLUDE_HREF);
    if (href == NULL) {
	href = xmlGetProp(cur, XINCLUDE_HREF);
	if (href == NULL) {
	    xmlGenericError(xmlGenericErrorContext, "XInclude: no href\n");
	    return(-1);
	}
    }
    parse = xmlGetNsProp(cur, XINCLUDE_NS, XINCLUDE_PARSE);
    if (parse == NULL) {
	parse = xmlGetProp(cur, XINCLUDE_PARSE);
    }
    if (parse != NULL) {
	if (xmlStrEqual(parse, XINCLUDE_PARSE_XML))
	    xml = 1;
	else if (xmlStrEqual(parse, XINCLUDE_PARSE_TEXT))
	    xml = 0;
	else {
	    xmlGenericError(xmlGenericErrorContext,
		    "XInclude: invalid value %s for %s\n",
		            parse, XINCLUDE_PARSE);
	    if (href != NULL)
		xmlFree(href);
	    if (parse != NULL)
		xmlFree(parse);
	    return(-1);
	}
    }

    /*
     * compute the URI
     */
    base = xmlNodeGetBase(ctxt->doc, cur);
    if (base == NULL) {
	URI = xmlBuildURI(href, ctxt->doc->URL);
    } else {
	URI = xmlBuildURI(href, base);
    }
    if (URI == NULL) {
	xmlChar *escbase;
	xmlChar *eschref;
	/*
	 * Some escaping may be needed
	 */
	escbase = xmlURIEscape(base);
	eschref = xmlURIEscape(href);
	URI = xmlBuildURI(eschref, escbase);
	if (escbase != NULL)
	    xmlFree(escbase);
	if (eschref != NULL)
	    xmlFree(eschref);
    }
    if (parse != NULL)
	xmlFree(parse);
    if (href != NULL)
	xmlFree(href);
    if (base != NULL)
	xmlFree(base);
    if (URI == NULL) {
	xmlGenericError(xmlGenericErrorContext, "XInclude: failed build URL\n");
	return(-1);
    }

    /*
     * Check the URL and remove any fragment identifier
     */
    uri = xmlParseURI((const char *)URI);
    if (uri == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		    "XInclude: invalid value URI %s\n", URI);
	return(-1);
    }
    if (uri->fragment != NULL) {
	fragment = (xmlChar *) uri->fragment;
	uri->fragment = NULL;
    }
    URL = xmlSaveUri(uri);
    xmlFreeURI(uri);
    xmlFree(URI);
    if (URL == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		    "XInclude: invalid value URI %s\n", URI);
	if (fragment != NULL)
	    xmlFree(fragment);
	return(-1);
    }

    ref = xmlXIncludeNewRef(ctxt, URL, cur);
    if (ref == NULL) {
	return(-1);
    }
    ref->fragment = fragment;
    ref->doc = NULL;
    ref->xml = xml;
    ref->count = 1;
    xmlFree(URL);
    return(0);
}

/**
 * xmlXIncludeRecurseDoc:
 * @ctxt:  the XInclude context
 * @doc:  the new document
 * @url:  the associated URL
 * 
 * The XInclude recursive nature is handled at this point.
 */
static void
xmlXIncludeRecurseDoc(xmlXIncludeCtxtPtr ctxt, xmlDocPtr doc,
	              const xmlURL url ATTRIBUTE_UNUSED) {
    xmlXIncludeCtxtPtr newctxt;
    int i;

#ifdef DEBUG_XINCLUDE
    xmlGenericError(xmlGenericErrorContext, "Recursing in doc %s\n", doc->URL);
#endif
    /*
     * Handle recursion here.
     */

    newctxt = xmlXIncludeNewContext(doc);
    if (newctxt != NULL) {
	/*
	 * Copy the existing document set
	 */
	newctxt->incMax = ctxt->incMax;
	newctxt->incNr = ctxt->incNr;
        newctxt->incTab = (xmlXIncludeRefPtr *) xmlMalloc(newctxt->incMax *
		                          sizeof(newctxt->incTab[0]));
        if (newctxt->incTab == NULL) {
	    xmlGenericError(xmlGenericErrorContext,
		    "malloc failed !\n");
	    xmlFree(newctxt);
	    return;
	}

	/*
	 * Inherit the documents already in use by others includes
	 */
	newctxt->incBase = ctxt->incNr;
	for (i = 0;i < ctxt->incNr;i++) {
	    newctxt->incTab[i] = ctxt->incTab[i];
	    newctxt->incTab[i]->count++; /* prevent the recursion from
					    freeing it */
	}
	xmlXIncludeDoProcess(newctxt, doc);
	for (i = 0;i < ctxt->incNr;i++) {
	    newctxt->incTab[i]->count--;
	    newctxt->incTab[i] = NULL;
	}
	xmlXIncludeFreeContext(newctxt);
    }
#ifdef DEBUG_XINCLUDE
    xmlGenericError(xmlGenericErrorContext, "Done recursing in doc %s\n", url);
#endif
}

/**
 * xmlXIncludeAddTxt:
 * @ctxt:  the XInclude context
 * @txt:  the new text node
 * @url:  the associated URL
 * 
 * Add a new txtument to the list
 */
static void
xmlXIncludeAddTxt(xmlXIncludeCtxtPtr ctxt, xmlNodePtr txt, const xmlURL url) {
#ifdef DEBUG_XINCLUDE
    xmlGenericError(xmlGenericErrorContext, "Adding text %s\n", url);
#endif
    if (ctxt->txtMax == 0) {
	ctxt->txtMax = 4;
        ctxt->txtTab = (xmlNodePtr *) xmlMalloc(ctxt->txtMax *
		                          sizeof(ctxt->txtTab[0]));
        if (ctxt->txtTab == NULL) {
	    xmlGenericError(xmlGenericErrorContext,
		    "malloc failed !\n");
	    return;
	}
        ctxt->txturlTab = (xmlURL *) xmlMalloc(ctxt->txtMax *
		                          sizeof(ctxt->txturlTab[0]));
        if (ctxt->txturlTab == NULL) {
	    xmlGenericError(xmlGenericErrorContext,
		    "malloc failed !\n");
	    return;
	}
    }
    if (ctxt->txtNr >= ctxt->txtMax) {
	ctxt->txtMax *= 2;
        ctxt->txtTab = (xmlNodePtr *) xmlRealloc(ctxt->txtTab,
	             ctxt->txtMax * sizeof(ctxt->txtTab[0]));
        if (ctxt->txtTab == NULL) {
	    xmlGenericError(xmlGenericErrorContext,
		    "realloc failed !\n");
	    return;
	}
        ctxt->txturlTab = (xmlURL *) xmlRealloc(ctxt->txturlTab,
	             ctxt->txtMax * sizeof(ctxt->txturlTab[0]));
        if (ctxt->txturlTab == NULL) {
	    xmlGenericError(xmlGenericErrorContext,
		    "realloc failed !\n");
	    return;
	}
    }
    ctxt->txtTab[ctxt->txtNr] = txt;
    ctxt->txturlTab[ctxt->txtNr] = xmlStrdup(url);
    ctxt->txtNr++;
}

/************************************************************************
 *									*
 *			Node copy with specific semantic		*
 *									*
 ************************************************************************/

/**
 * xmlXIncludeCopyNode:
 * @ctxt:  the XInclude context
 * @target:  the document target
 * @source:  the document source
 * @elem:  the element
 * 
 * Make a copy of the node while preserving the XInclude semantic
 * of the Infoset copy
 */
static xmlNodePtr
xmlXIncludeCopyNode(xmlXIncludeCtxtPtr ctxt, xmlDocPtr target,
	            xmlDocPtr source, xmlNodePtr elem) {
    xmlNodePtr result = NULL;

    if ((ctxt == NULL) || (target == NULL) || (source == NULL) ||
	(elem == NULL))
	return(NULL);
    if (elem->type == XML_DTD_NODE)
	return(NULL);
    result = xmlDocCopyNode(elem, target, 1);
    return(result);
}

/**
 * xmlXIncludeCopyNodeList:
 * @ctxt:  the XInclude context
 * @target:  the document target
 * @source:  the document source
 * @elem:  the element list
 * 
 * Make a copy of the node list while preserving the XInclude semantic
 * of the Infoset copy
 */
static xmlNodePtr
xmlXIncludeCopyNodeList(xmlXIncludeCtxtPtr ctxt, xmlDocPtr target,
	                xmlDocPtr source, xmlNodePtr elem) {
    xmlNodePtr cur, res, result = NULL, last = NULL;

    if ((ctxt == NULL) || (target == NULL) || (source == NULL) ||
	(elem == NULL))
	return(NULL);
    cur = elem;
    while (cur != NULL) {
	res = xmlXIncludeCopyNode(ctxt, target, source, cur);
	if (res != NULL) {
	    if (result == NULL) {
		result = last = res;
	    } else {
		last->next = res;
		res->prev = last;
		last = res;
	    }
	}
	cur = cur->next;
    }
    return(result);
}

/**
 * xmlXInclueGetNthChild:
 * @cur:  the node
 * @no:  the child number
 *
 * Returns the @no'th element child of @cur or NULL
 */
static xmlNodePtr
xmlXIncludeGetNthChild(xmlNodePtr cur, int no) {
    int i;
    if (cur == NULL) 
	return(cur);
    cur = cur->children;
    for (i = 0;i <= no;cur = cur->next) {
	if (cur == NULL) 
	    return(cur);
	if ((cur->type == XML_ELEMENT_NODE) ||
	    (cur->type == XML_DOCUMENT_NODE) ||
	    (cur->type == XML_HTML_DOCUMENT_NODE)) {
	    i++;
	    if (i == no)
		break;
	}
    }
    return(cur);
}

xmlNodePtr xmlXPtrAdvanceNode(xmlNodePtr cur);

/**
 * xmlXIncludeCopyRange:
 * @ctxt:  the XInclude context
 * @target:  the document target
 * @source:  the document source
 * @obj:  the XPointer result from the evaluation.
 *
 * Build a node list tree copy of the XPointer result.
 *
 * Returns an xmlNodePtr list or NULL.
 *         the caller has to free the node tree.
 */
static xmlNodePtr
xmlXIncludeCopyRange(xmlXIncludeCtxtPtr ctxt, xmlDocPtr target,
	                xmlDocPtr source, xmlXPathObjectPtr range) {
    /* pointers to generated nodes */
    xmlNodePtr list = NULL, last = NULL, parent = NULL, tmp;
    /* pointers to traversal nodes */
    xmlNodePtr start, cur, end;
    int index1, index2;

    if ((ctxt == NULL) || (target == NULL) || (source == NULL) ||
	(range == NULL))
	return(NULL);
    if (range->type != XPATH_RANGE)
	return(NULL);
    start = (xmlNodePtr) range->user;

    if (start == NULL)
	return(NULL);
    end = range->user2;
    if (end == NULL)
	return(xmlDocCopyNode(start, target, 1));

    cur = start;
    index1 = range->index;
    index2 = range->index2;
    while (cur != NULL) {
	if (cur == end) {
	    if (cur->type == XML_TEXT_NODE) {
		const xmlChar *content = cur->content;
		int len;

		if (content == NULL) {
		    tmp = xmlNewTextLen(NULL, 0);
		} else {
		    len = index2;
		    if ((cur == start) && (index1 > 1)) {
			content += (index1 - 1);
			len -= (index1 - 1);
			index1 = 0;
		    } else {
			len = index2;
		    }
		    tmp = xmlNewTextLen(content, len);
		}
		/* single sub text node selection */
		if (list == NULL)
		    return(tmp);
		/* prune and return full set */
		if (last != NULL)
		    xmlAddNextSibling(last, tmp);
		else 
		    xmlAddChild(parent, tmp);
		return(list);
	    } else {
		tmp = xmlDocCopyNode(cur, target, 0);
		if (list == NULL)
		    list = tmp;
		else {
		    if (last != NULL)
			xmlAddNextSibling(last, tmp);
		    else
			xmlAddChild(parent, tmp);
		}
		last = NULL;
		parent = tmp;

		if (index2 > 1) {
		    end = xmlXIncludeGetNthChild(cur, index2 - 1);
		    index2 = 0;
		}
		if ((cur == start) && (index1 > 1)) {
		    cur = xmlXIncludeGetNthChild(cur, index1 - 1);
		    index1 = 0;
		} else {
		    cur = cur->children;
		}
		/*
		 * Now gather the remaining nodes from cur to end
		 */
		continue; /* while */
	    }
	} else if ((cur == start) &&
		   (list == NULL) /* looks superfluous but ... */ ) {
	    if ((cur->type == XML_TEXT_NODE) ||
		(cur->type == XML_CDATA_SECTION_NODE)) {
		const xmlChar *content = cur->content;

		if (content == NULL) {
		    tmp = xmlNewTextLen(NULL, 0);
		} else {
		    if (index1 > 1) {
			content += (index1 - 1);
		    }
		    tmp = xmlNewText(content);
		}
		last = list = tmp;
	    } else {
		if ((cur == start) && (index1 > 1)) {
		    tmp = xmlDocCopyNode(cur, target, 0);
		    list = tmp;
		    parent = tmp;
		    last = NULL;
		    cur = xmlXIncludeGetNthChild(cur, index1 - 1);
		    index1 = 0;
		    /*
		     * Now gather the remaining nodes from cur to end
		     */
		    continue; /* while */
		}
		tmp = xmlDocCopyNode(cur, target, 1);
		list = tmp;
		parent = NULL;
		last = tmp;
	    }
	} else {
	    tmp = NULL;
	    switch (cur->type) {
		case XML_DTD_NODE:
		case XML_ELEMENT_DECL:
		case XML_ATTRIBUTE_DECL:
		case XML_ENTITY_NODE:
		    /* Do not copy DTD informations */
		    break;
		case XML_ENTITY_DECL:
		    /* handle crossing entities -> stack needed */
		    break;
		case XML_XINCLUDE_START:
		case XML_XINCLUDE_END:
		    /* don't consider it part of the tree content */
		    break;
		case XML_ATTRIBUTE_NODE:
		    /* Humm, should not happen ! */
		    break;
		default:
		    tmp = xmlDocCopyNode(cur, target, 1);
		    break;
	    }
	    if (tmp != NULL) {
		if ((list == NULL) || ((last == NULL) && (parent == NULL)))  {
		    return(NULL);
		}
		if (last != NULL)
		    xmlAddNextSibling(last, tmp);
		else {
		    xmlAddChild(parent, tmp);
		    last = tmp;
		}
	    }
	}
	/*
	 * Skip to next node in document order
	 */
	if ((list == NULL) || ((last == NULL) && (parent == NULL)))  {
	    return(NULL);
	}
	cur = xmlXPtrAdvanceNode(cur);
    }
    return(list);
}

/**
 * xmlXIncludeBuildNodeList:
 * @ctxt:  the XInclude context
 * @target:  the document target
 * @source:  the document source
 * @obj:  the XPointer result from the evaluation.
 *
 * Build a node list tree copy of the XPointer result.
 * This will drop Attributes and Namespace declarations.
 *
 * Returns an xmlNodePtr list or NULL.
 *         the caller has to free the node tree.
 */
static xmlNodePtr
xmlXIncludeCopyXPointer(xmlXIncludeCtxtPtr ctxt, xmlDocPtr target,
	                xmlDocPtr source, xmlXPathObjectPtr obj) {
    xmlNodePtr list = NULL, last = NULL;
    int i;

    if ((ctxt == NULL) || (target == NULL) || (source == NULL) ||
	(obj == NULL))
	return(NULL);
    switch (obj->type) {
        case XPATH_NODESET: {
	    xmlNodeSetPtr set = obj->nodesetval;
	    if (set == NULL)
		return(NULL);
	    for (i = 0;i < set->nodeNr;i++) {
		if (set->nodeTab[i] == NULL)
		    continue;
		switch (set->nodeTab[i]->type) {
		    case XML_TEXT_NODE:
		    case XML_CDATA_SECTION_NODE:
		    case XML_ELEMENT_NODE:
		    case XML_ENTITY_REF_NODE:
		    case XML_ENTITY_NODE:
		    case XML_PI_NODE:
		    case XML_COMMENT_NODE:
		    case XML_DOCUMENT_NODE:
		    case XML_HTML_DOCUMENT_NODE:
#ifdef LIBXML_DOCB_ENABLED
		    case XML_DOCB_DOCUMENT_NODE:
#endif
		    case XML_XINCLUDE_START:
		    case XML_XINCLUDE_END:
			break;
		    case XML_ATTRIBUTE_NODE:
		    case XML_NAMESPACE_DECL:
		    case XML_DOCUMENT_TYPE_NODE:
		    case XML_DOCUMENT_FRAG_NODE:
		    case XML_NOTATION_NODE:
		    case XML_DTD_NODE:
		    case XML_ELEMENT_DECL:
		    case XML_ATTRIBUTE_DECL:
		    case XML_ENTITY_DECL:
			continue; /* for */
		}
		if (last == NULL)
		    list = last = xmlXIncludeCopyNode(ctxt, target, source,
			                              set->nodeTab[i]);
		else {
		    xmlAddNextSibling(last,
			    xmlXIncludeCopyNode(ctxt, target, source,
				                set->nodeTab[i]));
		    if (last->next != NULL)
			last = last->next;
		}
	    }
	    break;
	}
	case XPATH_LOCATIONSET: {
	    xmlLocationSetPtr set = (xmlLocationSetPtr) obj->user;
	    if (set == NULL)
		return(NULL);
	    for (i = 0;i < set->locNr;i++) {
		if (last == NULL)
		    list = last = xmlXIncludeCopyXPointer(ctxt, target, source,
			                                  set->locTab[i]);
		else
		    xmlAddNextSibling(last,
			    xmlXIncludeCopyXPointer(ctxt, target, source,
				                    set->locTab[i]));
		if (last != NULL) {
		    while (last->next != NULL)
			last = last->next;
		}
	    }
	    break;
	}
	case XPATH_RANGE:
	    return(xmlXIncludeCopyRange(ctxt, target, source, obj));
	case XPATH_POINT:
	    /* points are ignored in XInclude */
	    break;
	default:
	    break;
    }
    return(list);
}
/************************************************************************
 *									*
 *			XInclude I/O handling				*
 *									*
 ************************************************************************/

/**
 * xmlXIncludeLoadDoc:
 * @ctxt:  the XInclude context
 * @url:  the associated URL
 * @nr:  the xinclude node number
 * 
 * Load the document, and store the result in the XInclude context
 *
 * Returns 0 in case of success, -1 in case of failure
 */
static int
xmlXIncludeLoadDoc(xmlXIncludeCtxtPtr ctxt, const xmlChar *url, int nr) {
    xmlDocPtr doc;
    xmlURIPtr uri;
    xmlChar *URL;
    xmlChar *fragment = NULL;
    int i = 0;

#ifdef DEBUG_XINCLUDE
    xmlGenericError(xmlGenericErrorContext, "Loading doc %s:%d\n", url, nr);
#endif
    /*
     * Check the URL and remove any fragment identifier
     */
    uri = xmlParseURI((const char *)url);
    if (uri == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		    "XInclude: invalid value URI %s\n", url);
	return(-1);
    }
    if (uri->fragment != NULL) {
	fragment = (xmlChar *) uri->fragment;
	uri->fragment = NULL;
    }
    URL = xmlSaveUri(uri);
    xmlFreeURI(uri);
    if (URL == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		    "XInclude: invalid value URI %s\n", url);
	if (fragment != NULL)
	    xmlFree(fragment);
	return(-1);
    }

    /*
     * Handling of references to the local document are done
     * directly through ctxt->doc.
     */
    if ((URL[0] == 0) || (URL[0] == '#')) {
	doc = NULL;
        goto loaded;
    }

    /*
     * Prevent reloading twice the document.
     */
    for (i = 0; i < ctxt->incNr; i++) {
	if ((xmlStrEqual(URL, ctxt->incTab[i]->URI)) &&
	    (ctxt->incTab[i]->doc != NULL)) {
	    doc = ctxt->incTab[i]->doc;
#ifdef DEBUG_XINCLUDE
	    printf("Already loaded %s\n", URL);
#endif
	    goto loaded;
	}
    }

    /*
     * Load it.
     */
#ifdef DEBUG_XINCLUDE
    printf("loading %s\n", URL);
#endif
    doc = xmlParseFile((const char *)URL);
    if (doc == NULL) {
	xmlFree(URL);
	if (fragment != NULL)
	    xmlFree(fragment);
	return(-1);
    }
    ctxt->incTab[nr]->doc = doc;

    /*
     * TODO: Make sure we have all entities fixed up
     */

    /*
     * We don't need the DTD anymore, free up space
    if (doc->intSubset != NULL) {
	xmlUnlinkNode((xmlNodePtr) doc->intSubset);
	xmlFreeNode((xmlNodePtr) doc->intSubset);
	doc->intSubset = NULL;
    }
    if (doc->extSubset != NULL) {
	xmlUnlinkNode((xmlNodePtr) doc->extSubset);
	xmlFreeNode((xmlNodePtr) doc->extSubset);
	doc->extSubset = NULL;
    }
     */
    xmlXIncludeRecurseDoc(ctxt, doc, URL);

loaded:
    if (fragment == NULL) {
	/*
	 * Add the top children list as the replacement copy.
	 */
	if (doc == NULL)
	{
	    /* Hopefully a DTD declaration won't be copied from
	     * the same document */
	    ctxt->incTab[nr]->inc = xmlCopyNodeList(ctxt->doc->children);
	} else {
	    ctxt->incTab[nr]->inc = xmlXIncludeCopyNodeList(ctxt, ctxt->doc,
		                                       doc, doc->children);
	}
    } else {
	/*
	 * Computes the XPointer expression and make a copy used
	 * as the replacement copy.
	 */
	xmlXPathObjectPtr xptr;
	xmlXPathContextPtr xptrctxt;
	xmlNodeSetPtr set;

	if (doc == NULL) {
	    xptrctxt = xmlXPtrNewContext(ctxt->doc, ctxt->incTab[nr]->ref,
		                         NULL);
	} else {
	    xptrctxt = xmlXPtrNewContext(doc, NULL, NULL);
	}
	if (xptrctxt == NULL) {
	    xmlGenericError(xmlGenericErrorContext,
			"XInclude: could create XPointer context\n");
	    xmlFree(URL);
	    xmlFree(fragment);
	    return(-1);
	}
	xptr = xmlXPtrEval(fragment, xptrctxt);
	if (xptr == NULL) {
	    xmlGenericError(xmlGenericErrorContext,
			"XInclude: XPointer evaluation failed: #%s\n",
			fragment);
	    xmlXPathFreeContext(xptrctxt);
	    xmlFree(URL);
	    xmlFree(fragment);
	    return(-1);
	}
	switch (xptr->type) {
	    case XPATH_UNDEFINED:
	    case XPATH_BOOLEAN:
	    case XPATH_NUMBER:
	    case XPATH_STRING:
	    case XPATH_POINT:
	    case XPATH_USERS:
	    case XPATH_XSLT_TREE:
		xmlGenericError(xmlGenericErrorContext,
			"XInclude: XPointer is not a range: #%s\n",
			        fragment);
		xmlXPathFreeContext(xptrctxt);
		xmlFree(URL);
		xmlFree(fragment);
		return(-1);
	    case XPATH_NODESET:
	    case XPATH_RANGE:
	    case XPATH_LOCATIONSET:
		break;
	}
	set = xptr->nodesetval;
	if (set != NULL) {
	    for (i = 0;i < set->nodeNr;i++) {
		if (set->nodeTab[i] == NULL)
		    continue;
		switch (set->nodeTab[i]->type) {
		    case XML_TEXT_NODE:
		    case XML_CDATA_SECTION_NODE:
		    case XML_ELEMENT_NODE:
		    case XML_ENTITY_REF_NODE:
		    case XML_ENTITY_NODE:
		    case XML_PI_NODE:
		    case XML_COMMENT_NODE:
		    case XML_DOCUMENT_NODE:
		    case XML_HTML_DOCUMENT_NODE:
#ifdef LIBXML_DOCB_ENABLED
		    case XML_DOCB_DOCUMENT_NODE:
#endif
			continue;
		    case XML_ATTRIBUTE_NODE:
			xmlGenericError(xmlGenericErrorContext,
			"XInclude: XPointer selects an attribute: #%s\n",
					fragment);
			set->nodeTab[i] = NULL;
			continue;
		    case XML_NAMESPACE_DECL:
			xmlGenericError(xmlGenericErrorContext,
			"XInclude: XPointer selects a namespace: #%s\n",
					fragment);
			set->nodeTab[i] = NULL;
			continue;
		    case XML_DOCUMENT_TYPE_NODE:
		    case XML_DOCUMENT_FRAG_NODE:
		    case XML_NOTATION_NODE:
		    case XML_DTD_NODE:
		    case XML_ELEMENT_DECL:
		    case XML_ATTRIBUTE_DECL:
		    case XML_ENTITY_DECL:
		    case XML_XINCLUDE_START:
		    case XML_XINCLUDE_END:
			xmlGenericError(xmlGenericErrorContext,
			"XInclude: XPointer selects unexpected nodes: #%s\n",
					fragment);
			set->nodeTab[i] = NULL;
			set->nodeTab[i] = NULL;
			continue; /* for */
		}
	    }
	}
	ctxt->incTab[nr]->inc =
	    xmlXIncludeCopyXPointer(ctxt, ctxt->doc, doc, xptr);
	xmlXPathFreeObject(xptr);
	xmlXPathFreeContext(xptrctxt);
	xmlFree(fragment);
    }

    /*
     * Do the xml:base fixup if needed
     */
    if ((doc != NULL) && (URL != NULL) && (xmlStrchr(URL, (xmlChar) '/'))) {
	xmlNodePtr node;

	node = ctxt->incTab[nr]->inc;
	while (node != NULL) {
	    if (node->type == XML_ELEMENT_NODE)
		xmlNodeSetBase(node, URL);
	    node = node->next;
	}
    }
    if ((nr < ctxt->incNr) && (ctxt->incTab[nr]->doc != NULL) &&
	(ctxt->incTab[nr]->count <= 1)) {
#ifdef DEBUG_XINCLUDE
        printf("freeing %s\n", ctxt->incTab[nr]->doc->URL);
#endif
	xmlFreeDoc(ctxt->incTab[nr]->doc);
	ctxt->incTab[nr]->doc = NULL;
    }
    xmlFree(URL);
    return(0);
}

/**
 * xmlXIncludeLoadTxt:
 * @ctxt:  the XInclude context
 * @url:  the associated URL
 * @nr:  the xinclude node number
 * 
 * Load the content, and store the result in the XInclude context
 *
 * Returns 0 in case of success, -1 in case of failure
 */
static int
xmlXIncludeLoadTxt(xmlXIncludeCtxtPtr ctxt, const xmlChar *url, int nr) {
    xmlParserInputBufferPtr buf;
    xmlNodePtr node;
    xmlURIPtr uri;
    xmlChar *URL;
    int i;
    xmlChar *encoding = NULL;
    xmlCharEncoding enc = 0;

    /*
     * Check the URL and remove any fragment identifier
     */
    uri = xmlParseURI((const char *)url);
    if (uri == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		    "XInclude: invalid value URI %s\n", url);
	return(-1);
    }
    if (uri->fragment != NULL) {
	xmlGenericError(xmlGenericErrorContext,
		"XInclude: fragment identifier forbidden for text: %s\n",
		uri->fragment);
	xmlFreeURI(uri);
	return(-1);
    }
    URL = xmlSaveUri(uri);
    xmlFreeURI(uri);
    if (URL == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		    "XInclude: invalid value URI %s\n", url);
	return(-1);
    }

    /*
     * Handling of references to the local document are done
     * directly through ctxt->doc.
     */
    if (URL[0] == 0) {
	xmlGenericError(xmlGenericErrorContext,
		"XInclude: text serialization of document not available\n");
	xmlFree(URL);
	return(-1);
    }

    /*
     * Prevent reloading twice the document.
     */
    for (i = 0; i < ctxt->txtNr; i++) {
	if (xmlStrEqual(URL, ctxt->txturlTab[i])) {
	    node = xmlCopyNode(ctxt->txtTab[i], 1);
	    goto loaded;
	}
    }
    /*
     * Try to get the encoding if available
     */
    if ((ctxt->incTab[nr] != NULL) && (ctxt->incTab[nr]->ref != NULL)) {
	encoding = xmlGetProp(ctxt->incTab[nr]->ref, XINCLUDE_PARSE_ENCODING);
    }
    if (encoding != NULL) {
	/*
	 * TODO: we should not have to remap to the xmlCharEncoding
	 *       predefined set, a better interface than
	 *       xmlParserInputBufferCreateFilename should allow any
	 *       encoding supported by iconv
	 */
        enc = xmlParseCharEncoding((const char *) encoding);
	if (enc == XML_CHAR_ENCODING_ERROR) {
	    xmlGenericError(xmlGenericErrorContext,
		    "XInclude: encoding %s not supported\n", encoding);
	    xmlFree(encoding);
	    xmlFree(URL);
	    return(-1);
	}
	xmlFree(encoding);
    }

    /*
     * Load it.
     */
    buf = xmlParserInputBufferCreateFilename((const char *)URL, enc);
    if (buf == NULL) {
	xmlFree(URL);
	return(-1);
    }
    node = xmlNewText(NULL);

    /*
     * Scan all chars from the resource and add the to the node
     */
    while (xmlParserInputBufferRead(buf, 128) > 0) {
	int len;
	const xmlChar *content;

	content = xmlBufferContent(buf->buffer);
	len = xmlBufferLength(buf->buffer);
	for (i = 0;i < len;) {
	    int cur;
	    int l;

	    cur = xmlStringCurrentChar(NULL, &content[i], &l);
	    if (!IS_CHAR(cur)) {
		xmlGenericError(xmlGenericErrorContext,
		    "XInclude: %s contains invalid char %d\n", URL, cur);
	    } else {
		xmlNodeAddContentLen(node, &content[i], l);
	    }
	    i += l;
	}
	xmlBufferShrink(buf->buffer, len);
    }
    xmlFreeParserInputBuffer(buf);
    xmlXIncludeAddTxt(ctxt, node, URL);

loaded:
    /*
     * Add the element as the replacement copy.
     */
    ctxt->incTab[nr]->inc = node;
    xmlFree(URL);
    return(0);
}

/**
 * xmlXIncludeLoadFallback:
 * @ctxt:  the XInclude context
 * @fallback:  the fallback node
 * @nr:  the xinclude node number
 * 
 * Load the content of the fallback node, and store the result
 * in the XInclude context
 *
 * Returns 0 in case of success, -1 in case of failure
 */
static int
xmlXIncludeLoadFallback(xmlXIncludeCtxtPtr ctxt, xmlNodePtr fallback, int nr) {
    if ((fallback == NULL) || (ctxt == NULL))
	return(-1);

    ctxt->incTab[nr]->inc = xmlCopyNode(fallback->children, 1);
    return(0);
}

/************************************************************************
 *									*
 *			XInclude Processing				*
 *									*
 ************************************************************************/

/**
 * xmlXIncludePreProcessNode:
 * @ctxt: an XInclude context
 * @node: an XInclude node
 *
 * Implement the XInclude preprocessing, currently just adding the element
 * for further processing.
 *
 * Returns the result list or NULL in case of error
 */
static xmlNodePtr
xmlXIncludePreProcessNode(xmlXIncludeCtxtPtr ctxt, xmlNodePtr node) {
    xmlXIncludeAddNode(ctxt, node);
    return(0);
}

#if 0
/**
 * xmlXIncludePreloadNode:
 * @ctxt: an XInclude context
 * @nr: the node number
 *
 * Do some precomputations and preload shared documents
 *
 * Returns 0 if substitution succeeded, -1 if some processing failed
 */
static int
xmlXIncludePreloadNode(xmlXIncludeCtxtPtr ctxt, int nr) {
    xmlNodePtr cur;
    xmlChar *href;
    xmlChar *parse;
    xmlChar *base;
    xmlChar *URI;
    int xml = 1; /* default Issue 64 */
    xmlURIPtr uri;
    xmlChar *URL;
    xmlChar *fragment = NULL;
    int i;


    if (ctxt == NULL)
	return(-1);
    if ((nr < 0) || (nr >= ctxt->incNr))
	return(-1);
    cur = ctxt->incTab[nr]->ref;
    if (cur == NULL)
	return(-1);

    /*
     * read the attributes
     */
    href = xmlGetNsProp(cur, XINCLUDE_NS, XINCLUDE_HREF);
    if (href == NULL) {
	href = xmlGetProp(cur, XINCLUDE_HREF);
	if (href == NULL) {
	    xmlGenericError(xmlGenericErrorContext, "XInclude: no href\n");
	    return(-1);
	}
    }
    parse = xmlGetNsProp(cur, XINCLUDE_NS, XINCLUDE_PARSE);
    if (parse == NULL) {
	parse = xmlGetProp(cur, XINCLUDE_PARSE);
    }
    if (parse != NULL) {
	if (xmlStrEqual(parse, XINCLUDE_PARSE_XML))
	    xml = 1;
	else if (xmlStrEqual(parse, XINCLUDE_PARSE_TEXT))
	    xml = 0;
	else {
	    xmlGenericError(xmlGenericErrorContext,
		    "XInclude: invalid value %s for %s\n",
		            parse, XINCLUDE_PARSE);
	    if (href != NULL)
		xmlFree(href);
	    if (parse != NULL)
		xmlFree(parse);
	    return(-1);
	}
    }

    /*
     * compute the URI
     */
    base = xmlNodeGetBase(ctxt->doc, cur);
    if (base == NULL) {
	URI = xmlBuildURI(href, ctxt->doc->URL);
    } else {
	URI = xmlBuildURI(href, base);
    }
    if (URI == NULL) {
	xmlChar *escbase;
	xmlChar *eschref;
	/*
	 * Some escaping may be needed
	 */
	escbase = xmlURIEscape(base);
	eschref = xmlURIEscape(href);
	URI = xmlBuildURI(eschref, escbase);
	if (escbase != NULL)
	    xmlFree(escbase);
	if (eschref != NULL)
	    xmlFree(eschref);
    }
    if (parse != NULL)
	xmlFree(parse);
    if (href != NULL)
	xmlFree(href);
    if (base != NULL)
	xmlFree(base);
    if (URI == NULL) {
	xmlGenericError(xmlGenericErrorContext, "XInclude: failed build URL\n");
	return(-1);
    }

    /*
     * Check the URL and remove any fragment identifier
     */
    uri = xmlParseURI((const char *)URI);
    if (uri == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		    "XInclude: invalid value URI %s\n", URI);
	xmlFree(URI);
	return(-1);
    }
    if (uri->fragment != NULL) {
	fragment = (xmlChar *) uri->fragment;
	uri->fragment = NULL;
    }
    URL = xmlSaveUri(uri);
    xmlFreeURI(uri);
    if (URL == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		    "XInclude: invalid value URI %s\n", URI);
	if (fragment != NULL)
	    xmlFree(fragment);
	xmlFree(URI);
	return(-1);
    }
    xmlFree(URI);
    if (fragment != NULL)
	xmlFree(fragment);

    for (i = 0; i < nr; i++) {
	if (xmlStrEqual(URL, ctxt->incTab[i]->URI)) { 
#ifdef DEBUG_XINCLUDE
	    printf("Incrementing count for %d : %s\n", i, ctxt->incTab[i]->URI);
#endif
	    ctxt->incTab[i]->count++;
	    break;
	}
    }
    xmlFree(URL);
    return(0);
}
#endif

/**
 * xmlXIncludeLoadNode:
 * @ctxt: an XInclude context
 * @nr: the node number
 *
 * Find and load the infoset replacement for the given node.
 *
 * Returns 0 if substitution succeeded, -1 if some processing failed
 */
static int
xmlXIncludeLoadNode(xmlXIncludeCtxtPtr ctxt, int nr) {
    xmlNodePtr cur;
    xmlChar *href;
    xmlChar *parse;
    xmlChar *base;
    xmlChar *URI;
    int xml = 1; /* default Issue 64 */
    int ret;

    if (ctxt == NULL)
	return(-1);
    if ((nr < 0) || (nr >= ctxt->incNr))
	return(-1);
    cur = ctxt->incTab[nr]->ref;
    if (cur == NULL)
	return(-1);

    /*
     * read the attributes
     */
    href = xmlGetNsProp(cur, XINCLUDE_NS, XINCLUDE_HREF);
    if (href == NULL) {
	href = xmlGetProp(cur, XINCLUDE_HREF);
	if (href == NULL) {
	    xmlGenericError(xmlGenericErrorContext, "XInclude: no href\n");
	    return(-1);
	}
    }
    parse = xmlGetNsProp(cur, XINCLUDE_NS, XINCLUDE_PARSE);
    if (parse == NULL) {
	parse = xmlGetProp(cur, XINCLUDE_PARSE);
    }
    if (parse != NULL) {
	if (xmlStrEqual(parse, XINCLUDE_PARSE_XML))
	    xml = 1;
	else if (xmlStrEqual(parse, XINCLUDE_PARSE_TEXT))
	    xml = 0;
	else {
	    xmlGenericError(xmlGenericErrorContext,
		    "XInclude: invalid value %s for %s\n",
		            parse, XINCLUDE_PARSE);
	    if (href != NULL)
		xmlFree(href);
	    if (parse != NULL)
		xmlFree(parse);
	    return(-1);
	}
    }

    /*
     * compute the URI
     */
    base = xmlNodeGetBase(ctxt->doc, cur);
    if (base == NULL) {
	URI = xmlBuildURI(href, ctxt->doc->URL);
    } else {
	URI = xmlBuildURI(href, base);
    }
    if (URI == NULL) {
	xmlChar *escbase;
	xmlChar *eschref;
	/*
	 * Some escaping may be needed
	 */
	escbase = xmlURIEscape(base);
	eschref = xmlURIEscape(href);
	URI = xmlBuildURI(eschref, escbase);
	if (escbase != NULL)
	    xmlFree(escbase);
	if (eschref != NULL)
	    xmlFree(eschref);
    }
    if (URI == NULL) {
	xmlGenericError(xmlGenericErrorContext, "XInclude: failed build URL\n");
	if (parse != NULL)
	    xmlFree(parse);
	if (href != NULL)
	    xmlFree(href);
	if (base != NULL)
	    xmlFree(base);
	return(-1);
    }
#ifdef DEBUG_XINCLUDE
    xmlGenericError(xmlGenericErrorContext, "parse: %s\n",
	    xml ? "xml": "text");
    xmlGenericError(xmlGenericErrorContext, "URI: %s\n", URI);
#endif

    /*
     * Cleanup
     */
    if (xml) {
	ret = xmlXIncludeLoadDoc(ctxt, URI, nr);
	/* xmlXIncludeGetFragment(ctxt, cur, URI); */
    } else {
	ret = xmlXIncludeLoadTxt(ctxt, URI, nr);
    }
    if (ret < 0) {
	xmlNodePtr children;

	/*
	 * Time to try a fallback if availble
	 */
#ifdef DEBUG_XINCLUDE
	xmlGenericError(xmlGenericErrorContext, "error looking for fallback\n");
#endif
	children = cur->children;
	while (children != NULL) {
	    if ((children->type == XML_ELEMENT_NODE) &&
		(children->ns != NULL) &&
		(xmlStrEqual(children->name, XINCLUDE_FALLBACK)) &&
		(xmlStrEqual(children->ns->href, XINCLUDE_NS))) {
		ret = xmlXIncludeLoadFallback(ctxt, children, nr);
		if (ret == 0)
		    break;
	    }
	    children = children->next;
	}
    }
    if (ret < 0) {
	xmlGenericError(xmlGenericErrorContext,
		    "XInclude: could not load %s, and no fallback was found\n",
		        URI);
    }

    /*
     * Cleanup
     */
    if (URI != NULL)
	xmlFree(URI);
    if (parse != NULL)
	xmlFree(parse);
    if (href != NULL)
	xmlFree(href);
    if (base != NULL)
	xmlFree(base);
    return(0);
}

/**
 * xmlXIncludeIncludeNode:
 * @ctxt: an XInclude context
 * @nr: the node number
 *
 * Inplement the infoset replacement for the given node
 *
 * Returns 0 if substitution succeeded, -1 if some processing failed
 */
static int
xmlXIncludeIncludeNode(xmlXIncludeCtxtPtr ctxt, int nr) {
    xmlNodePtr cur, end, list;

    if (ctxt == NULL)
	return(-1);
    if ((nr < 0) || (nr >= ctxt->incNr))
	return(-1);
    cur = ctxt->incTab[nr]->ref;
    if (cur == NULL)
	return(-1);

    /*
     * Change the current node as an XInclude start one, and add an
     * entity end one
     */
    cur->type = XML_XINCLUDE_START;
    end = xmlNewNode(cur->ns, cur->name);
    if (end == NULL) {
	xmlGenericError(xmlGenericErrorContext, 
		"XInclude: failed to build node\n");
	return(-1);
    }
    end->type = XML_XINCLUDE_END;
    xmlAddNextSibling(cur, end);

    /*
     * Add the list of nodes
     */
    list = ctxt->incTab[nr]->inc;
    ctxt->incTab[nr]->inc = NULL;
    while (list != NULL) {
	cur = list;
	list = list->next;

        xmlAddPrevSibling(end, cur);
    }

    
    return(0);
}

/**
 * xmlXIncludeTestNode:
 * @node: an XInclude node
 *
 * test if the node is an XInclude node
 *
 * Returns 1 true, 0 otherwise
 */
static int
xmlXIncludeTestNode(xmlNodePtr node) {
    if (node == NULL)
	return(0);
    if (node->ns == NULL)
	return(0);
    if ((xmlStrEqual(node->name, XINCLUDE_NODE)) &&
	(xmlStrEqual(node->ns->href, XINCLUDE_NS))) return(1);
    return(0);
}

/**
 * xmlXIncludeDoProcess:
 * @ctxt: 
 * @doc: an XML document
 *
 * Implement the XInclude substitution on the XML document @doc
 *
 * Returns 0 if no substitution were done, -1 if some processing failed
 *    or the number of substitutions done.
 */
static int
xmlXIncludeDoProcess(xmlXIncludeCtxtPtr ctxt, xmlDocPtr doc) {
    xmlNodePtr cur;
    int ret = 0;
    int i;

    if (doc == NULL)
	return(-1);
    if (ctxt == NULL)
	return(-1);

    /*
     * First phase: lookup the elements in the document
     */
    cur = xmlDocGetRootElement(doc);
    if (xmlXIncludeTestNode(cur))
	xmlXIncludePreProcessNode(ctxt, cur);
    while (cur != NULL) {
	/* TODO: need to work on entities -> stack */
	if ((cur->children != NULL) &&
	    (cur->children->type != XML_ENTITY_DECL)) {
	    cur = cur->children;
	    if (xmlXIncludeTestNode(cur))
		xmlXIncludePreProcessNode(ctxt, cur);
	} else if (cur->next != NULL) {
	    cur = cur->next;
	    if (xmlXIncludeTestNode(cur))
		xmlXIncludePreProcessNode(ctxt, cur);
	} else {
	    do {
		cur = cur->parent;
		if (cur == NULL) break; /* do */
		if (cur->next != NULL) {
		    cur = cur->next;
		    if (xmlXIncludeTestNode(cur))
			xmlXIncludePreProcessNode(ctxt, cur);
		    break; /* do */
		}
	    } while (cur != NULL);
	}
    }

    /*
     * Second Phase : collect the infosets fragments
     */
    /*
    for (i = ctxt->incBase;i < ctxt->incNr; i++) {
        xmlXIncludePreloadNode(ctxt, i);
    }
     */
    for (i = ctxt->incBase;i < ctxt->incNr; i++) {
        xmlXIncludeLoadNode(ctxt, i);
    }

    /*
     * Third phase: extend the original document infoset.
     */
    for (i = ctxt->incBase;i < ctxt->incNr; i++) {
	xmlXIncludeIncludeNode(ctxt, i);
    }

    return(ret);
}

/**
 * xmlXIncludeProcess:
 * @doc: an XML document
 *
 * Implement the XInclude substitution on the XML document @doc
 *
 * Returns 0 if no substitution were done, -1 if some processing failed
 *    or the number of substitutions done.
 */
int
xmlXIncludeProcess(xmlDocPtr doc) {
    xmlXIncludeCtxtPtr ctxt;
    int ret = 0;

    if (doc == NULL)
	return(-1);
    ctxt = xmlXIncludeNewContext(doc);
    if (ctxt == NULL)
	return(-1);
    ret = xmlXIncludeDoProcess(ctxt, doc);

    xmlXIncludeFreeContext(ctxt);
    return(ret);
}

#else /* !LIBXML_XINCLUDE_ENABLED */
#endif
