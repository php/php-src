/*
 * tree.c : implementation of access function for an XML tree.
 *
 * References:
 *   XHTML 1.0 W3C REC: http://www.w3.org/TR/2002/REC-xhtml1-20020801/
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 *
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
#ifdef HAVE_ZLIB_H
#include <zlib.h>
#endif

#include <libxml/xmlmemory.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/uri.h>
#include <libxml/entities.h>
#include <libxml/valid.h>
#include <libxml/xmlerror.h>
#include <libxml/parserInternals.h>
#include <libxml/globals.h>
#ifdef LIBXML_HTML_ENABLED
#include <libxml/HTMLtree.h>
#endif

xmlNsPtr xmlNewReconciliedNs(xmlDocPtr doc, xmlNodePtr tree, xmlNsPtr ns);

/************************************************************************
 *									*
 * 		A few static variables and macros			*
 *									*
 ************************************************************************/
/* #undef xmlStringText */
const xmlChar xmlStringText[] = { 't', 'e', 'x', 't', 0 };
/* #undef xmlStringTextNoenc */
const xmlChar xmlStringTextNoenc[] =
              { 't', 'e', 'x', 't', 'n', 'o', 'e', 'n', 'c', 0 };
/* #undef xmlStringComment */
const xmlChar xmlStringComment[] = { 'c', 'o', 'm', 'm', 'e', 'n', 't', 0 };

static int xmlCompressMode = 0;
static int xmlCheckDTD = 1;

#define UPDATE_LAST_CHILD_AND_PARENT(n) if ((n) != NULL) {		\
    xmlNodePtr ulccur = (n)->children;					\
    if (ulccur == NULL) {						\
        (n)->last = NULL;						\
    } else {								\
        while (ulccur->next != NULL) {					\
	       	ulccur->parent = (n);					\
		ulccur = ulccur->next;					\
	}								\
	ulccur->parent = (n);						\
	(n)->last = ulccur;						\
}}

/* #define DEBUG_BUFFER */
/* #define DEBUG_TREE */

/************************************************************************
 *									*
 *		Functions to move to entities.c once the 		*
 *		API freeze is smoothen and they can be made public.	*
 *									*
 ************************************************************************/
#include <libxml/hash.h>
 
/**
 * xmlGetEntityFromDtd:
 * @dtd:  A pointer to the DTD to search
 * @name:  The entity name
 *
 * Do an entity lookup in the DTD entity hash table and
 * return the corresponding entity, if found.
 * 
 * Returns A pointer to the entity structure or NULL if not found.
 */
static xmlEntityPtr
xmlGetEntityFromDtd(xmlDtdPtr dtd, const xmlChar *name) {
    xmlEntitiesTablePtr table;
    
    if((dtd != NULL) && (dtd->entities != NULL)) {
	table = (xmlEntitiesTablePtr) dtd->entities;
	return((xmlEntityPtr) xmlHashLookup(table, name));
    	/* return(xmlGetEntityFromTable(table, name)); */
    }
    return(NULL);
}
/**
 * xmlGetParameterEntityFromDtd:
 * @dtd:  A pointer to the DTD to search
 * @name:  The entity name
 * 
 * Do an entity lookup in the DTD pararmeter entity hash table and
 * return the corresponding entity, if found.
 *
 * Returns A pointer to the entity structure or NULL if not found.
 */
static xmlEntityPtr
xmlGetParameterEntityFromDtd(xmlDtdPtr dtd, const xmlChar *name) {
    xmlEntitiesTablePtr table;
    
    if ((dtd != NULL) && (dtd->pentities != NULL)) {
	table = (xmlEntitiesTablePtr) dtd->pentities;
	return((xmlEntityPtr) xmlHashLookup(table, name));
	/* return(xmlGetEntityFromTable(table, name)); */
    }
    return(NULL);
}

/************************************************************************
 *									*
 *		Allocation and deallocation of basic structures		*
 *									*
 ************************************************************************/
 
/**
 * xmlSetBufferAllocationScheme:
 * @scheme:  allocation method to use
 * 
 * Set the buffer allocation method.  Types are
 * XML_BUFFER_ALLOC_EXACT - use exact sizes, keeps memory usage down
 * XML_BUFFER_ALLOC_DOUBLEIT - double buffer when extra needed, 
 *                             improves performance
 */
void
xmlSetBufferAllocationScheme(xmlBufferAllocationScheme scheme) {
    xmlBufferAllocScheme = scheme;
}

/**
 * xmlGetBufferAllocationScheme:
 *
 * Types are
 * XML_BUFFER_ALLOC_EXACT - use exact sizes, keeps memory usage down
 * XML_BUFFER_ALLOC_DOUBLEIT - double buffer when extra needed, 
 *                             improves performance
 * 
 * Returns the current allocation scheme
 */
xmlBufferAllocationScheme
xmlGetBufferAllocationScheme(void) {
    return(xmlBufferAllocScheme);
}

/**
 * xmlNewNs:
 * @node:  the element carrying the namespace
 * @href:  the URI associated
 * @prefix:  the prefix for the namespace
 *
 * Creation of a new Namespace. This function will refuse to create
 * a namespace with a similar prefix than an existing one present on this
 * node.
 * We use href==NULL in the case of an element creation where the namespace
 * was not defined.
 * Returns a new namespace pointer or NULL
 */
xmlNsPtr
xmlNewNs(xmlNodePtr node, const xmlChar *href, const xmlChar *prefix) {
    xmlNsPtr cur;

    if ((node != NULL) && (node->type != XML_ELEMENT_NODE))
	return(NULL);

    if ((prefix != NULL) && (xmlStrEqual(prefix, BAD_CAST "xml")))
	return(NULL);

    /*
     * Allocate a new Namespace and fill the fields.
     */
    cur = (xmlNsPtr) xmlMalloc(sizeof(xmlNs));
    if (cur == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewNs : malloc failed\n");
	return(NULL);
    }
    memset(cur, 0, sizeof(xmlNs));
    cur->type = XML_LOCAL_NAMESPACE;

    if (href != NULL)
	cur->href = xmlStrdup(href); 
    if (prefix != NULL)
	cur->prefix = xmlStrdup(prefix); 

    /*
     * Add it at the end to preserve parsing order ...
     * and checks for existing use of the prefix
     */
    if (node != NULL) {
	if (node->nsDef == NULL) {
	    node->nsDef = cur;
	} else {
	    xmlNsPtr prev = node->nsDef;

	    if (((prev->prefix == NULL) && (cur->prefix == NULL)) ||
		(xmlStrEqual(prev->prefix, cur->prefix))) {
		xmlFreeNs(cur);
		return(NULL);
	    }    
	    while (prev->next != NULL) {
	        prev = prev->next;
		if (((prev->prefix == NULL) && (cur->prefix == NULL)) ||
		    (xmlStrEqual(prev->prefix, cur->prefix))) {
		    xmlFreeNs(cur);
		    return(NULL);
		}    
	    }
	    prev->next = cur;
	}
    }
    return(cur);
}

/**
 * xmlSetNs:
 * @node:  a node in the document
 * @ns:  a namespace pointer
 *
 * Associate a namespace to a node, a posteriori.
 */
void
xmlSetNs(xmlNodePtr node, xmlNsPtr ns) {
    if (node == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlSetNs: node == NULL\n");
#endif
	return;
    }
    node->ns = ns;
}

/**
 * xmlFreeNs:
 * @cur:  the namespace pointer
 *
 * Free up the structures associated to a namespace
 */
void
xmlFreeNs(xmlNsPtr cur) {
    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlFreeNs : ns == NULL\n");
#endif
	return;
    }
    if (cur->href != NULL) xmlFree((char *) cur->href);
    if (cur->prefix != NULL) xmlFree((char *) cur->prefix);
    xmlFree(cur);
}

/**
 * xmlFreeNsList:
 * @cur:  the first namespace pointer
 *
 * Free up all the structures associated to the chained namespaces.
 */
void
xmlFreeNsList(xmlNsPtr cur) {
    xmlNsPtr next;
    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlFreeNsList : ns == NULL\n");
#endif
	return;
    }
    while (cur != NULL) {
        next = cur->next;
        xmlFreeNs(cur);
	cur = next;
    }
}

/**
 * xmlNewDtd:
 * @doc:  the document pointer
 * @name:  the DTD name
 * @ExternalID:  the external ID
 * @SystemID:  the system ID
 *
 * Creation of a new DTD for the external subset. To create an
 * internal subset, use xmlCreateIntSubset().
 *
 * Returns a pointer to the new DTD structure
 */
xmlDtdPtr
xmlNewDtd(xmlDocPtr doc, const xmlChar *name,
                    const xmlChar *ExternalID, const xmlChar *SystemID) {
    xmlDtdPtr cur;

    if ((doc != NULL) && (doc->extSubset != NULL)) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewDtd(%s): document %s already have a DTD %s\n",
	    /* !!! */ (char *) name, doc->name,
	    /* !!! */ (char *)doc->extSubset->name);
#endif
	return(NULL);
    }

    /*
     * Allocate a new DTD and fill the fields.
     */
    cur = (xmlDtdPtr) xmlMalloc(sizeof(xmlDtd));
    if (cur == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewDtd : malloc failed\n");
	return(NULL);
    }
    memset(cur, 0 , sizeof(xmlDtd));
    cur->type = XML_DTD_NODE;

    if (name != NULL)
	cur->name = xmlStrdup(name); 
    if (ExternalID != NULL)
	cur->ExternalID = xmlStrdup(ExternalID); 
    if (SystemID != NULL)
	cur->SystemID = xmlStrdup(SystemID); 
    if (doc != NULL)
	doc->extSubset = cur;
    cur->doc = doc;

    return(cur);
}

/**
 * xmlGetIntSubset:
 * @doc:  the document pointer
 *
 * Get the internal subset of a document
 * Returns a pointer to the DTD structure or NULL if not found
 */

xmlDtdPtr
xmlGetIntSubset(xmlDocPtr doc) {
    xmlNodePtr cur;

    if (doc == NULL)
	return(NULL);
    cur = doc->children;
    while (cur != NULL) {
	if (cur->type == XML_DTD_NODE)
	    return((xmlDtdPtr) cur);
	cur = cur->next;
    }
    return((xmlDtdPtr) doc->intSubset);
}

/**
 * xmlCreateIntSubset:
 * @doc:  the document pointer
 * @name:  the DTD name
 * @ExternalID:  the external (PUBLIC) ID
 * @SystemID:  the system ID
 *
 * Create the internal subset of a document
 * Returns a pointer to the new DTD structure
 */
xmlDtdPtr
xmlCreateIntSubset(xmlDocPtr doc, const xmlChar *name,
                   const xmlChar *ExternalID, const xmlChar *SystemID) {
    xmlDtdPtr cur;

    if ((doc != NULL) && (xmlGetIntSubset(doc) != NULL)) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,

     "xmlCreateIntSubset(): document %s already have an internal subset\n",
	    doc->name);
#endif
	return(NULL);
    }

    /*
     * Allocate a new DTD and fill the fields.
     */
    cur = (xmlDtdPtr) xmlMalloc(sizeof(xmlDtd));
    if (cur == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlCreateIntSubset : malloc failed\n");
	return(NULL);
    }
    memset(cur, 0, sizeof(xmlDtd));
    cur->type = XML_DTD_NODE;

    if (name != NULL)
	cur->name = xmlStrdup(name); 
    if (ExternalID != NULL)
	cur->ExternalID = xmlStrdup(ExternalID); 
    if (SystemID != NULL)
	cur->SystemID = xmlStrdup(SystemID); 
    if (doc != NULL) {
	doc->intSubset = cur;
	cur->parent = doc;
	cur->doc = doc;
	if (doc->children == NULL) {
	    doc->children = (xmlNodePtr) cur;
	    doc->last = (xmlNodePtr) cur;
	} else {
	    if (doc->type == XML_HTML_DOCUMENT_NODE) {
		xmlNodePtr prev;

		prev = doc->children;
		prev->prev = (xmlNodePtr) cur;
		cur->next = prev;
		doc->children = (xmlNodePtr) cur;
	    } else {
		xmlNodePtr next;

		next = doc->children;
		while ((next != NULL) && (next->type != XML_ELEMENT_NODE))
		    next = next->next;
		if (next == NULL) {
		    cur->prev = doc->last;
		    cur->prev->next = (xmlNodePtr) cur;
		    cur->next = NULL;
		    doc->last = (xmlNodePtr) cur;
		} else {
		    cur->next = next;
		    cur->prev = next->prev;
		    if (cur->prev == NULL)
			doc->children = (xmlNodePtr) cur;
		    else
			cur->prev->next = (xmlNodePtr) cur;
		    next->prev = (xmlNodePtr) cur;
		}
	    }
	}
    }
    return(cur);
}

/**
 * xmlFreeDtd:
 * @cur:  the DTD structure to free up
 *
 * Free a DTD structure.
 */
void
xmlFreeDtd(xmlDtdPtr cur) {
    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlFreeDtd : DTD == NULL\n");
#endif
	return;
    }
    if (cur->children != NULL) {
	xmlNodePtr next, c = cur->children;

	/*
	 * Cleanup all the DTD comments they are not in the DTD
	 * indexes.
	 */
        while (c != NULL) {
	    next = c->next;
	    if (c->type == XML_COMMENT_NODE) {
		xmlUnlinkNode(c);
		xmlFreeNode(c);
	    }
	    c = next;
	}
    }
    if (cur->name != NULL) xmlFree((char *) cur->name);
    if (cur->SystemID != NULL) xmlFree((char *) cur->SystemID);
    if (cur->ExternalID != NULL) xmlFree((char *) cur->ExternalID);
    /* TODO !!! */
    if (cur->notations != NULL)
        xmlFreeNotationTable((xmlNotationTablePtr) cur->notations);
    
    if (cur->elements != NULL)
        xmlFreeElementTable((xmlElementTablePtr) cur->elements);
    if (cur->attributes != NULL)
        xmlFreeAttributeTable((xmlAttributeTablePtr) cur->attributes);
    if (cur->entities != NULL)
        xmlFreeEntitiesTable((xmlEntitiesTablePtr) cur->entities);
    if (cur->pentities != NULL)
        xmlFreeEntitiesTable((xmlEntitiesTablePtr) cur->pentities);

    xmlFree(cur);
}

/**
 * xmlNewDoc:
 * @version:  xmlChar string giving the version of XML "1.0"
 *
 * Creates a new XML document
 *
 * Returns a new document
 */
xmlDocPtr
xmlNewDoc(const xmlChar *version) {
    xmlDocPtr cur;

    if (version == NULL)
	version = (const xmlChar *) "1.0";

    /*
     * Allocate a new document and fill the fields.
     */
    cur = (xmlDocPtr) xmlMalloc(sizeof(xmlDoc));
    if (cur == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewDoc : malloc failed\n");
	return(NULL);
    }
    memset(cur, 0, sizeof(xmlDoc));
    cur->type = XML_DOCUMENT_NODE;

    cur->version = xmlStrdup(version); 
    cur->standalone = -1;
    cur->compression = -1; /* not initialized */
    cur->doc = cur;
    cur->charset = XML_CHAR_ENCODING_UTF8;
    return(cur);
}

/**
 * xmlFreeDoc:
 * @cur:  pointer to the document
 *
 * Free up all the structures used by a document, tree included.
 */
void
xmlFreeDoc(xmlDocPtr cur) {
    xmlDtdPtr extSubset, intSubset;

    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlFreeDoc : document == NULL\n");
#endif
	return;
    }
    /*
     * Do this before freeing the children list to avoid ID lookups
     */
    if (cur->ids != NULL) xmlFreeIDTable((xmlIDTablePtr) cur->ids);
    cur->ids = NULL;
    if (cur->refs != NULL) xmlFreeRefTable((xmlRefTablePtr) cur->refs);
    cur->refs = NULL;
    extSubset = cur->extSubset;
    intSubset = cur->intSubset;
    if (intSubset == extSubset)
	extSubset = NULL;
    if (extSubset != NULL) {
	xmlUnlinkNode((xmlNodePtr) cur->extSubset);
	cur->extSubset = NULL;
	xmlFreeDtd(extSubset);
    }
    if (intSubset != NULL) {
	xmlUnlinkNode((xmlNodePtr) cur->intSubset);
	cur->intSubset = NULL;
	xmlFreeDtd(intSubset);
    }

    if (cur->children != NULL) xmlFreeNodeList(cur->children);

    if (cur->version != NULL) xmlFree((char *) cur->version);
    if (cur->name != NULL) xmlFree((char *) cur->name);
    if (cur->encoding != NULL) xmlFree((char *) cur->encoding);
    if (cur->oldNs != NULL) xmlFreeNsList(cur->oldNs);
    if (cur->URL != NULL) xmlFree((char *) cur->URL);
    xmlFree(cur);
}

/**
 * xmlStringLenGetNodeList:
 * @doc:  the document
 * @value:  the value of the text
 * @len:  the length of the string value
 *
 * Parse the value string and build the node list associated. Should
 * produce a flat tree with only TEXTs and ENTITY_REFs.
 * Returns a pointer to the first child
 */
xmlNodePtr
xmlStringLenGetNodeList(xmlDocPtr doc, const xmlChar *value, int len) {
    xmlNodePtr ret = NULL, last = NULL;
    xmlNodePtr node;
    xmlChar *val;
    const xmlChar *cur = value;
    const xmlChar *q;
    xmlEntityPtr ent;

    if (value == NULL) return(NULL);

    q = cur;
    while ((*cur != 0) && (cur - value < len)) {
	if (*cur == '&') {
	    /*
	     * Save the current text.
	     */
            if (cur != q) {
		if ((last != NULL) && (last->type == XML_TEXT_NODE)) {
		    xmlNodeAddContentLen(last, q, cur - q);
		} else {
		    node = xmlNewDocTextLen(doc, q, cur - q);
		    if (node == NULL) return(ret);
		    if (last == NULL)
			last = ret = node;
		    else {
			last->next = node;
			node->prev = last;
			last = node;
		    }
		}
	    }
	    /*
	     * Read the entity string
	     */
	    cur++;
	    q = cur;
	    while ((*cur != 0) && (cur - value < len) && (*cur != ';')) cur++;
	    if ((*cur == 0) || (cur - value >= len)) {
#ifdef DEBUG_TREE
	        xmlGenericError(xmlGenericErrorContext,
		    "xmlStringLenGetNodeList: unterminated entity %30s\n", q);
#endif
	        return(ret);
	    }
            if (cur != q) {
		/*
		 * Predefined entities don't generate nodes
		 */
		val = xmlStrndup(q, cur - q);
		ent = xmlGetDocEntity(doc, val);
		if ((ent != NULL) &&
		    (ent->etype == XML_INTERNAL_PREDEFINED_ENTITY)) {
		    if (last == NULL) {
		        node = xmlNewDocText(doc, ent->content);
			last = ret = node;
		    } else
		        xmlNodeAddContent(last, ent->content);
		        
		} else {
		    /*
		     * Create a new REFERENCE_REF node
		     */
		    node = xmlNewReference(doc, val);
		    if (node == NULL) {
			if (val != NULL) xmlFree(val);
		        return(ret);
		    }
			else if ((ent != NULL) && (ent->children == NULL)) {
				xmlNodePtr tmp;

				ent->children =
					xmlStringGetNodeList(doc, (const xmlChar*)node->content);
				tmp = ent->children;
				while (tmp) {
					tmp->parent = (xmlNodePtr)ent;
					tmp = tmp->next;
				}
			}
		    if (last == NULL)
			last = ret = node;
		    else {
			last->next = node;
			node->prev = last;
			last = node;
		    }
		}
		xmlFree(val);
	    }
	    cur++;
	    q = cur;
	} else 
	    cur++;
    }
    if (cur != q) {
        /*
	 * Handle the last piece of text.
	 */
	if ((last != NULL) && (last->type == XML_TEXT_NODE)) {
	    xmlNodeAddContentLen(last, q, cur - q);
	} else {
	    node = xmlNewDocTextLen(doc, q, cur - q);
	    if (node == NULL) return(ret);
	    if (last == NULL)
		last = ret = node;
	    else {
		last->next = node;
		node->prev = last;
		last = node;
	    }
	}
    }
    return(ret);
}

/**
 * xmlStringGetNodeList:
 * @doc:  the document
 * @value:  the value of the attribute
 *
 * Parse the value string and build the node list associated. Should
 * produce a flat tree with only TEXTs and ENTITY_REFs.
 * Returns a pointer to the first child
 */
xmlNodePtr
xmlStringGetNodeList(xmlDocPtr doc, const xmlChar *value) {
    xmlNodePtr ret = NULL, last = NULL;
    xmlNodePtr node;
    xmlChar *val;
    const xmlChar *cur = value;
    const xmlChar *q;
    xmlEntityPtr ent;

    if (value == NULL) return(NULL);

    q = cur;
    while (*cur != 0) {
	if (cur[0] == '&') {
	    int charval = 0;
	    xmlChar tmp;

	    /*
	     * Save the current text.
	     */
            if (cur != q) {
		if ((last != NULL) && (last->type == XML_TEXT_NODE)) {
		    xmlNodeAddContentLen(last, q, cur - q);
		} else {
		    node = xmlNewDocTextLen(doc, q, cur - q);
		    if (node == NULL) return(ret);
		    if (last == NULL)
			last = ret = node;
		    else {
			last->next = node;
			node->prev = last;
			last = node;
		    }
		}
	    }
	    q = cur;
	    if ((cur[1] == '#') && (cur[2] == 'x')) {
		cur += 3;
		tmp = *cur;
		while (tmp != ';') { /* Non input consuming loop */
		    if ((tmp >= '0') && (tmp <= '9')) 
			charval = charval * 16 + (tmp - '0');
		    else if ((tmp >= 'a') && (tmp <= 'f'))
			charval = charval * 16 + (tmp - 'a') + 10;
		    else if ((tmp >= 'A') && (tmp <= 'F'))
			charval = charval * 16 + (tmp - 'A') + 10;
		    else {
			xmlGenericError(xmlGenericErrorContext,
		    "xmlStringGetNodeList: invalid hexadecimal charvalue\n");
			charval = 0;
			break;
		    }
		    cur++;
		    tmp = *cur;
		}
		if (tmp == ';')
		    cur++;
		q = cur;
	    } else if  (cur[1] == '#') {
		cur += 2;
		tmp = *cur;
		while (tmp != ';') { /* Non input consuming loops */
		    if ((tmp >= '0') && (tmp <= '9')) 
			charval = charval * 10 + (tmp - '0');
		    else {
			xmlGenericError(xmlGenericErrorContext,
		    "xmlStringGetNodeList: invalid decimal charvalue\n");
			charval = 0;
			break;
		    }
		    cur++;
		    tmp = *cur;
		}
		if (tmp == ';')
		    cur++;
		q = cur;
	    } else {
		/*
		 * Read the entity string
		 */
		cur++;
		q = cur;
		while ((*cur != 0) && (*cur != ';')) cur++;
		if (*cur == 0) {
#ifdef DEBUG_TREE
		    xmlGenericError(xmlGenericErrorContext,
			    "xmlStringGetNodeList: unterminated entity %30s\n", q);
#endif
		    return(ret);
		}
		if (cur != q) {
		    /*
		     * Predefined entities don't generate nodes
		     */
		    val = xmlStrndup(q, cur - q);
		    ent = xmlGetDocEntity(doc, val);
		    if ((ent != NULL) &&
			(ent->etype == XML_INTERNAL_PREDEFINED_ENTITY)) {
			if (last == NULL) {
			    node = xmlNewDocText(doc, ent->content);
			    last = ret = node;
			} else if (last->type != XML_TEXT_NODE) {
			    node = xmlNewDocText(doc, ent->content);
			    last = xmlAddNextSibling(last, node);
			} else
			    xmlNodeAddContent(last, ent->content);
			    
		    } else {
			/*
			 * Create a new REFERENCE_REF node
			 */
			node = xmlNewReference(doc, val);
			if (node == NULL) {
			    if (val != NULL) xmlFree(val);
			    return(ret);
			}
			else if ((ent != NULL) && (ent->children == NULL)) {
			    xmlNodePtr temp;

			    ent->children = xmlStringGetNodeList(doc,
				    (const xmlChar*)node->content);
			    temp = ent->children;
			    while (temp) {
				temp->parent = (xmlNodePtr)ent;
				temp = temp->next;
			    }
			}
			if (last == NULL) {
			    last = ret = node;
			} else {
			    last = xmlAddNextSibling(last, node);
			}
		    }
		    xmlFree(val);
		}
		cur++;
		q = cur;
	    }
	    if (charval != 0) {
		xmlChar buf[10];
		int len;

		len = xmlCopyCharMultiByte(buf, charval);
		buf[len] = 0;
		node = xmlNewDocText(doc, buf);
		if (node != NULL) {
		    if (last == NULL) {
			last = ret = node;
		    } else {
			last = xmlAddNextSibling(last, node);
		    }
		}

		charval = 0;
	    }
	} else
	    cur++;
    }
    if ((cur != q) || (ret == NULL)) {
        /*
	 * Handle the last piece of text.
	 */
	if ((last != NULL) && (last->type == XML_TEXT_NODE)) {
	    xmlNodeAddContentLen(last, q, cur - q);
	} else {
	    node = xmlNewDocTextLen(doc, q, cur - q);
	    if (node == NULL) return(ret);
	    if (last == NULL) {
		last = ret = node;
	    } else {
		last = xmlAddNextSibling(last, node);
	    }
	}
    }
    return(ret);
}

/**
 * xmlNodeListGetString:
 * @doc:  the document
 * @list:  a Node list
 * @inLine:  should we replace entity contents or show their external form
 *
 * Build the string equivalent to the text contained in the Node list
 * made of TEXTs and ENTITY_REFs
 *
 * Returns a pointer to the string copy, the caller must free it with xmlFree().
 */
xmlChar *
xmlNodeListGetString(xmlDocPtr doc, xmlNodePtr list, int inLine)
{
    xmlNodePtr node = list;
    xmlChar *ret = NULL;
    xmlEntityPtr ent;

    if (list == NULL)
        return (NULL);

    while (node != NULL) {
        if ((node->type == XML_TEXT_NODE) ||
            (node->type == XML_CDATA_SECTION_NODE)) {
            if (inLine) {
                ret = xmlStrcat(ret, node->content);
            } else {
                xmlChar *buffer;

                buffer = xmlEncodeEntitiesReentrant(doc, node->content);
                if (buffer != NULL) {
                    ret = xmlStrcat(ret, buffer);
                    xmlFree(buffer);
                }
            }
        } else if (node->type == XML_ENTITY_REF_NODE) {
            if (inLine) {
                ent = xmlGetDocEntity(doc, node->name);
                if (ent != NULL) {
                    xmlChar *buffer;

                    /* an entity content can be any "well balanced chunk",
                     * i.e. the result of the content [43] production:
                     * http://www.w3.org/TR/REC-xml#NT-content.
                     * So it can contain text, CDATA section or nested
                     * entity reference nodes (among others).
                     * -> we recursive  call xmlNodeListGetString()
                     * which handles these types */
                    buffer = xmlNodeListGetString(doc, ent->children, 1);
                    if (buffer != NULL) {
                        ret = xmlStrcat(ret, buffer);
                        xmlFree(buffer);
                    }
                } else {
                    ret = xmlStrcat(ret, node->content);
                }
            } else {
                xmlChar buf[2];

                buf[0] = '&';
                buf[1] = 0;
                ret = xmlStrncat(ret, buf, 1);
                ret = xmlStrcat(ret, node->name);
                buf[0] = ';';
                buf[1] = 0;
                ret = xmlStrncat(ret, buf, 1);
            }
        }
#if 0
        else {
            xmlGenericError(xmlGenericErrorContext,
                            "xmlGetNodeListString : invalid node type %d\n",
                            node->type);
        }
#endif
        node = node->next;
    }
    return (ret);
}
/**
 * xmlNodeListGetRawString:
 * @doc:  the document
 * @list:  a Node list
 * @inLine:  should we replace entity contents or show their external form
 *
 * Builds the string equivalent to the text contained in the Node list
 * made of TEXTs and ENTITY_REFs, contrary to xmlNodeListGetString()
 * this function doesn't do any character encoding handling.
 *
 * Returns a pointer to the string copy, the caller must free it with xmlFree().
 */
xmlChar *
xmlNodeListGetRawString(xmlDocPtr doc, xmlNodePtr list, int inLine)
{
    xmlNodePtr node = list;
    xmlChar *ret = NULL;
    xmlEntityPtr ent;

    if (list == NULL)
        return (NULL);

    while (node != NULL) {
        if ((node->type == XML_TEXT_NODE) ||
            (node->type == XML_CDATA_SECTION_NODE)) {
            if (inLine) {
                ret = xmlStrcat(ret, node->content);
            } else {
                xmlChar *buffer;

                buffer = xmlEncodeSpecialChars(doc, node->content);
                if (buffer != NULL) {
                    ret = xmlStrcat(ret, buffer);
                    xmlFree(buffer);
                }
            }
        } else if (node->type == XML_ENTITY_REF_NODE) {
            if (inLine) {
                ent = xmlGetDocEntity(doc, node->name);
                if (ent != NULL) {
                    xmlChar *buffer;

                    /* an entity content can be any "well balanced chunk",
                     * i.e. the result of the content [43] production:
                     * http://www.w3.org/TR/REC-xml#NT-content.
                     * So it can contain text, CDATA section or nested
                     * entity reference nodes (among others).
                     * -> we recursive  call xmlNodeListGetRawString()
                     * which handles these types */
                    buffer =
                        xmlNodeListGetRawString(doc, ent->children, 1);
                    if (buffer != NULL) {
                        ret = xmlStrcat(ret, buffer);
                        xmlFree(buffer);
                    }
                } else {
                    ret = xmlStrcat(ret, node->content);
                }
            } else {
                xmlChar buf[2];

                buf[0] = '&';
                buf[1] = 0;
                ret = xmlStrncat(ret, buf, 1);
                ret = xmlStrcat(ret, node->name);
                buf[0] = ';';
                buf[1] = 0;
                ret = xmlStrncat(ret, buf, 1);
            }
        }
#if 0
        else {
            xmlGenericError(xmlGenericErrorContext,
                            "xmlGetNodeListString : invalid node type %d\n",
                            node->type);
        }
#endif
        node = node->next;
    }
    return (ret);
}

/**
 * xmlNewProp:
 * @node:  the holding node
 * @name:  the name of the attribute
 * @value:  the value of the attribute
 *
 * Create a new property carried by a node.
 * Returns a pointer to the attribute
 */
xmlAttrPtr
xmlNewProp(xmlNodePtr node, const xmlChar *name, const xmlChar *value) {
    xmlAttrPtr cur;
    xmlDocPtr doc = NULL;

    if (name == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewProp : name == NULL\n");
#endif
	return(NULL);
    }

    /*
     * Allocate a new property and fill the fields.
     */
    cur = (xmlAttrPtr) xmlMalloc(sizeof(xmlAttr));
    if (cur == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewProp : malloc failed\n");
	return(NULL);
    }
    memset(cur, 0, sizeof(xmlAttr));
    cur->type = XML_ATTRIBUTE_NODE;

    cur->parent = node; 
    if (node != NULL) {
	doc = node->doc;
	cur->doc = doc;
    }
    cur->name = xmlStrdup(name);
    if (value != NULL) {
	xmlChar *buffer;
	xmlNodePtr tmp;

	buffer = xmlEncodeEntitiesReentrant(doc, value);
	cur->children = xmlStringGetNodeList(doc, buffer);
	cur->last = NULL;
	tmp = cur->children;
	while (tmp != NULL) {
	    tmp->parent = (xmlNodePtr) cur;
	    tmp->doc = doc;
	    if (tmp->next == NULL)
		cur->last = tmp;
	    tmp = tmp->next;
	}
	xmlFree(buffer);
    }	

    /*
     * Add it at the end to preserve parsing order ...
     */
    if (node != NULL) {
	if (node->properties == NULL) {
	    node->properties = cur;
	} else {
	    xmlAttrPtr prev = node->properties;

	    while (prev->next != NULL) prev = prev->next;
	    prev->next = cur;
	    cur->prev = prev;
	}
    }
    return(cur);
}

/**
 * xmlNewNsProp:
 * @node:  the holding node
 * @ns:  the namespace
 * @name:  the name of the attribute
 * @value:  the value of the attribute
 *
 * Create a new property tagged with a namespace and carried by a node.
 * Returns a pointer to the attribute
 */
xmlAttrPtr
xmlNewNsProp(xmlNodePtr node, xmlNsPtr ns, const xmlChar *name,
           const xmlChar *value) {
    xmlAttrPtr cur;
    xmlDocPtr doc = NULL;

    if (name == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewNsProp : name == NULL\n");
#endif
	return(NULL);
    }

    /*
     * Allocate a new property and fill the fields.
     */
    cur = (xmlAttrPtr) xmlMalloc(sizeof(xmlAttr));
    if (cur == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewNsProp : malloc failed\n");
	return(NULL);
    }
    memset(cur, 0, sizeof(xmlAttr));
    cur->type = XML_ATTRIBUTE_NODE;

    cur->parent = node; 
    if (node != NULL) {
	doc = node->doc;
	cur->doc = doc;
    }
    cur->ns = ns;
    cur->name = xmlStrdup(name);
    if (value != NULL) {
	xmlChar *buffer;
	xmlNodePtr tmp;

	buffer = xmlEncodeEntitiesReentrant(doc, value);
	cur->children = xmlStringGetNodeList(doc, buffer);
	cur->last = NULL;
	tmp = cur->children;
	while (tmp != NULL) {
	    tmp->parent = (xmlNodePtr) cur;
	    if (tmp->next == NULL)
		cur->last = tmp;
	    tmp = tmp->next;
	}
	xmlFree(buffer);
    }

    /*
     * Add it at the end to preserve parsing order ...
     */
    if (node != NULL) {
	if (node->properties == NULL) {
	    node->properties = cur;
	} else {
	    xmlAttrPtr prev = node->properties;

	    while (prev->next != NULL) prev = prev->next;
	    prev->next = cur;
	    cur->prev = prev;
	}
    }
    return(cur);
}

/**
 * xmlNewNsPropEatName:
 * @node:  the holding node
 * @ns:  the namespace
 * @name:  the name of the attribute
 * @value:  the value of the attribute
 *
 * Create a new property tagged with a namespace and carried by a node.
 * Returns a pointer to the attribute
 */
xmlAttrPtr
xmlNewNsPropEatName(xmlNodePtr node, xmlNsPtr ns, xmlChar *name,
           const xmlChar *value) {
    xmlAttrPtr cur;
    xmlDocPtr doc = NULL;

    if (name == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewNsPropEatName : name == NULL\n");
#endif
	return(NULL);
    }

    /*
     * Allocate a new property and fill the fields.
     */
    cur = (xmlAttrPtr) xmlMalloc(sizeof(xmlAttr));
    if (cur == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewNsPropEatName : malloc failed\n");
	return(NULL);
    }
    memset(cur, 0, sizeof(xmlAttr));
    cur->type = XML_ATTRIBUTE_NODE;

    cur->parent = node; 
    if (node != NULL) {
	doc = node->doc;
	cur->doc = doc;
    }
    cur->ns = ns;
    cur->name = name;
    if (value != NULL) {
	xmlChar *buffer;
	xmlNodePtr tmp;

	buffer = xmlEncodeEntitiesReentrant(doc, value);
	cur->children = xmlStringGetNodeList(doc, buffer);
	cur->last = NULL;
	tmp = cur->children;
	while (tmp != NULL) {
	    tmp->parent = (xmlNodePtr) cur;
	    if (tmp->next == NULL)
		cur->last = tmp;
	    tmp = tmp->next;
	}
	xmlFree(buffer);
    }

    /*
     * Add it at the end to preserve parsing order ...
     */
    if (node != NULL) {
	if (node->properties == NULL) {
	    node->properties = cur;
	} else {
	    xmlAttrPtr prev = node->properties;

	    while (prev->next != NULL) prev = prev->next;
	    prev->next = cur;
	    cur->prev = prev;
	}
    }
    return(cur);
}

/**
 * xmlNewDocProp:
 * @doc:  the document
 * @name:  the name of the attribute
 * @value:  the value of the attribute
 *
 * Create a new property carried by a document.
 * Returns a pointer to the attribute
 */
xmlAttrPtr
xmlNewDocProp(xmlDocPtr doc, const xmlChar *name, const xmlChar *value) {
    xmlAttrPtr cur;

    if (name == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewDocProp : name == NULL\n");
#endif
	return(NULL);
    }

    /*
     * Allocate a new property and fill the fields.
     */
    cur = (xmlAttrPtr) xmlMalloc(sizeof(xmlAttr));
    if (cur == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewDocProp : malloc failed\n");
	return(NULL);
    }
    memset(cur, 0, sizeof(xmlAttr));
    cur->type = XML_ATTRIBUTE_NODE;

    cur->name = xmlStrdup(name);
    cur->doc = doc; 
    if (value != NULL) {
	xmlNodePtr tmp;

	cur->children = xmlStringGetNodeList(doc, value);
	cur->last = NULL;

	tmp = cur->children;
	while (tmp != NULL) {
	    tmp->parent = (xmlNodePtr) cur;
	    if (tmp->next == NULL)
		cur->last = tmp;
	    tmp = tmp->next;
	}
    }
    return(cur);
}

/**
 * xmlFreePropList:
 * @cur:  the first property in the list
 *
 * Free a property and all its siblings, all the children are freed too.
 */
void
xmlFreePropList(xmlAttrPtr cur) {
    xmlAttrPtr next;
    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlFreePropList : property == NULL\n");
#endif
	return;
    }
    while (cur != NULL) {
        next = cur->next;
        xmlFreeProp(cur);
	cur = next;
    }
}

/**
 * xmlFreeProp:
 * @cur:  an attribute
 *
 * Free one attribute, all the content is freed too
 */
void
xmlFreeProp(xmlAttrPtr cur) {
    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlFreeProp : property == NULL\n");
#endif
	return;
    }
    /* Check for ID removal -> leading to invalid references ! */
    if ((cur->parent != NULL) && (cur->parent->doc != NULL) &&
	((cur->parent->doc->intSubset != NULL) ||
	 (cur->parent->doc->extSubset != NULL))) {
        if (xmlIsID(cur->parent->doc, cur->parent, cur))
	    xmlRemoveID(cur->parent->doc, cur);
    }
    if (cur->name != NULL) xmlFree((char *) cur->name);
    if (cur->children != NULL) xmlFreeNodeList(cur->children);
    xmlFree(cur);
}

/**
 * xmlRemoveProp:
 * @cur:  an attribute
 *
 * Unlink and free one attribute, all the content is freed too
 * Note this doesn't work for namespace definition attributes
 *
 * Returns 0 if success and -1 in case of error.
 */
int
xmlRemoveProp(xmlAttrPtr cur) {
    xmlAttrPtr tmp;
    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlRemoveProp : cur == NULL\n");
#endif
	return(-1);
    }
    if (cur->parent == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlRemoveProp : cur->parent == NULL\n");
#endif
	return(-1);
    }
    tmp = cur->parent->properties;
    if (tmp == cur) {
        cur->parent->properties = cur->next;
	xmlFreeProp(cur);
	return(0);
    }
    while (tmp != NULL) {
	if (tmp->next == cur) {
	    tmp->next = cur->next;
	    if (tmp->next != NULL)
		tmp->next->prev = tmp;
	    xmlFreeProp(cur);
	    return(0);
	}
        tmp = tmp->next;
    }
#ifdef DEBUG_TREE
    xmlGenericError(xmlGenericErrorContext,
	    "xmlRemoveProp : attribute not owned by its node\n");
#endif
    return(-1);
}

/**
 * xmlNewPI:
 * @name:  the processing instruction name
 * @content:  the PI content
 *
 * Creation of a processing instruction element.
 * Returns a pointer to the new node object.
 */
xmlNodePtr
xmlNewPI(const xmlChar *name, const xmlChar *content) {
    xmlNodePtr cur;

    if (name == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewPI : name == NULL\n");
#endif
	return(NULL);
    }

    /*
     * Allocate a new node and fill the fields.
     */
    cur = (xmlNodePtr) xmlMalloc(sizeof(xmlNode));
    if (cur == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewPI : malloc failed\n");
	return(NULL);
    }
    memset(cur, 0, sizeof(xmlNode));
    cur->type = XML_PI_NODE;

    cur->name = xmlStrdup(name);
    if (content != NULL) {
	cur->content = xmlStrdup(content);
    }
    return(cur);
}

/**
 * xmlNewNode:
 * @ns:  namespace if any
 * @name:  the node name
 *
 * Creation of a new node element. @ns is optional (NULL).
 *
 * Returns a pointer to the new node object.
 */
xmlNodePtr
xmlNewNode(xmlNsPtr ns, const xmlChar *name) {
    xmlNodePtr cur;

    if (name == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewNode : name == NULL\n");
#endif
	return(NULL);
    }

    /*
     * Allocate a new node and fill the fields.
     */
    cur = (xmlNodePtr) xmlMalloc(sizeof(xmlNode));
    if (cur == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewNode : malloc failed\n");
	return(NULL);
    }
    memset(cur, 0, sizeof(xmlNode));
    cur->type = XML_ELEMENT_NODE;
    
    cur->name = xmlStrdup(name);
    cur->ns = ns;
    return(cur);
}

/**
 * xmlNewNodeEatName:
 * @ns:  namespace if any
 * @name:  the node name
 *
 * Creation of a new node element. @ns is optional (NULL).
 *
 * Returns a pointer to the new node object.
 */
xmlNodePtr
xmlNewNodeEatName(xmlNsPtr ns, xmlChar *name) {
    xmlNodePtr cur;

    if (name == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewNode : name == NULL\n");
#endif
	return(NULL);
    }

    /*
     * Allocate a new node and fill the fields.
     */
    cur = (xmlNodePtr) xmlMalloc(sizeof(xmlNode));
    if (cur == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewNode : malloc failed\n");
	return(NULL);
    }
    memset(cur, 0, sizeof(xmlNode));
    cur->type = XML_ELEMENT_NODE;
    
    cur->name = name;
    cur->ns = ns;
    return(cur);
}

/**
 * xmlNewDocNode:
 * @doc:  the document
 * @ns:  namespace if any
 * @name:  the node name
 * @content:  the XML text content if any
 *
 * Creation of a new node element within a document. @ns and @content
 * are optional (NULL).
 * NOTE: @content is supposed to be a piece of XML CDATA, so it allow entities
 *       references, but XML special chars need to be escaped first by using
 *       xmlEncodeEntitiesReentrant(). Use xmlNewDocRawNode() if you don't
 *       need entities support.
 *
 * Returns a pointer to the new node object.
 */
xmlNodePtr
xmlNewDocNode(xmlDocPtr doc, xmlNsPtr ns,
              const xmlChar *name, const xmlChar *content) {
    xmlNodePtr cur;

    cur = xmlNewNode(ns, name);
    if (cur != NULL) {
        cur->doc = doc;
	if (content != NULL) {
	    cur->children = xmlStringGetNodeList(doc, content);
	    UPDATE_LAST_CHILD_AND_PARENT(cur)
	}
    }
    return(cur);
}

/**
 * xmlNewDocNodeEatName:
 * @doc:  the document
 * @ns:  namespace if any
 * @name:  the node name
 * @content:  the XML text content if any
 *
 * Creation of a new node element within a document. @ns and @content
 * are optional (NULL).
 * NOTE: @content is supposed to be a piece of XML CDATA, so it allow entities
 *       references, but XML special chars need to be escaped first by using
 *       xmlEncodeEntitiesReentrant(). Use xmlNewDocRawNode() if you don't
 *       need entities support.
 *
 * Returns a pointer to the new node object.
 */
xmlNodePtr
xmlNewDocNodeEatName(xmlDocPtr doc, xmlNsPtr ns,
              xmlChar *name, const xmlChar *content) {
    xmlNodePtr cur;

    cur = xmlNewNodeEatName(ns, name);
    if (cur != NULL) {
        cur->doc = doc;
	if (content != NULL) {
	    cur->children = xmlStringGetNodeList(doc, content);
	    UPDATE_LAST_CHILD_AND_PARENT(cur)
	}
    }
    return(cur);
}


/**
 * xmlNewDocRawNode:
 * @doc:  the document
 * @ns:  namespace if any
 * @name:  the node name
 * @content:  the text content if any
 *
 * Creation of a new node element within a document. @ns and @content
 * are optional (NULL).
 *
 * Returns a pointer to the new node object.
 */
xmlNodePtr
xmlNewDocRawNode(xmlDocPtr doc, xmlNsPtr ns,
                 const xmlChar *name, const xmlChar *content) {
    xmlNodePtr cur;

    cur = xmlNewNode(ns, name);
    if (cur != NULL) {
        cur->doc = doc;
	if (content != NULL) {
	    cur->children = xmlNewDocText(doc, content);
	    UPDATE_LAST_CHILD_AND_PARENT(cur)
	}
    }
    return(cur);
}

/**
 * xmlNewDocFragment:
 * @doc:  the document owning the fragment
 *
 * Creation of a new Fragment node.
 * Returns a pointer to the new node object.
 */
xmlNodePtr
xmlNewDocFragment(xmlDocPtr doc) {
    xmlNodePtr cur;

    /*
     * Allocate a new DocumentFragment node and fill the fields.
     */
    cur = (xmlNodePtr) xmlMalloc(sizeof(xmlNode));
    if (cur == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewDocFragment : malloc failed\n");
	return(NULL);
    }
    memset(cur, 0, sizeof(xmlNode));
    cur->type = XML_DOCUMENT_FRAG_NODE;

    cur->doc = doc;
    return(cur);
}

/**
 * xmlNewText:
 * @content:  the text content
 *
 * Creation of a new text node.
 * Returns a pointer to the new node object.
 */
xmlNodePtr
xmlNewText(const xmlChar *content) {
    xmlNodePtr cur;

    /*
     * Allocate a new node and fill the fields.
     */
    cur = (xmlNodePtr) xmlMalloc(sizeof(xmlNode));
    if (cur == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewText : malloc failed\n");
	return(NULL);
    }
    memset(cur, 0, sizeof(xmlNode));
    cur->type = XML_TEXT_NODE;

    cur->name = xmlStringText;
    if (content != NULL) {
	cur->content = xmlStrdup(content);
    }
    return(cur);
}

/**
 * xmlNewTextChild:
 * @parent:  the parent node
 * @ns:  a namespace if any
 * @name:  the name of the child
 * @content:  the text content of the child if any.
 *
 * Creation of a new child element, added at the end of @parent children list.
 * @ns and @content parameters are optional (NULL). If content is non NULL,
 * a child TEXT node will be created containing the string content.
 *
 * Returns a pointer to the new node object.
 */
xmlNodePtr
xmlNewTextChild(xmlNodePtr parent, xmlNsPtr ns,
            const xmlChar *name, const xmlChar *content) {
    xmlNodePtr cur, prev;

    if (parent == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewTextChild : parent == NULL\n");
#endif
	return(NULL);
    }

    if (name == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewTextChild : name == NULL\n");
#endif
	return(NULL);
    }

    /*
     * Allocate a new node
     */
    if (ns == NULL)
	cur = xmlNewDocRawNode(parent->doc, parent->ns, name, content);
    else
	cur = xmlNewDocRawNode(parent->doc, ns, name, content);
    if (cur == NULL) return(NULL);

    /*
     * add the new element at the end of the children list.
     */
    cur->type = XML_ELEMENT_NODE;
    cur->parent = parent;
    cur->doc = parent->doc;
    if (parent->children == NULL) {
        parent->children = cur;
	parent->last = cur;
    } else {
        prev = parent->last;
	prev->next = cur;
	cur->prev = prev;
	parent->last = cur;
    }

    return(cur);
}

/**
 * xmlNewCharRef:
 * @doc: the document
 * @name:  the char ref string, starting with # or "&# ... ;"
 *
 * Creation of a new character reference node.
 * Returns a pointer to the new node object.
 */
xmlNodePtr
xmlNewCharRef(xmlDocPtr doc, const xmlChar *name) {
    xmlNodePtr cur;

    /*
     * Allocate a new node and fill the fields.
     */
    cur = (xmlNodePtr) xmlMalloc(sizeof(xmlNode));
    if (cur == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewCharRef : malloc failed\n");
	return(NULL);
    }
    memset(cur, 0, sizeof(xmlNode));
    cur->type = XML_ENTITY_REF_NODE;

    cur->doc = doc;
    if (name[0] == '&') {
        int len;
        name++;
	len = xmlStrlen(name);
	if (name[len - 1] == ';')
	    cur->name = xmlStrndup(name, len - 1);
	else
	    cur->name = xmlStrndup(name, len);
    } else
	cur->name = xmlStrdup(name);
    return(cur);
}

/**
 * xmlNewReference:
 * @doc: the document
 * @name:  the reference name, or the reference string with & and ;
 *
 * Creation of a new reference node.
 * Returns a pointer to the new node object.
 */
xmlNodePtr
xmlNewReference(xmlDocPtr doc, const xmlChar *name) {
    xmlNodePtr cur;
    xmlEntityPtr ent;

    /*
     * Allocate a new node and fill the fields.
     */
    cur = (xmlNodePtr) xmlMalloc(sizeof(xmlNode));
    if (cur == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewReference : malloc failed\n");
	return(NULL);
    }
    memset(cur, 0, sizeof(xmlNode));
    cur->type = XML_ENTITY_REF_NODE;

    cur->doc = doc;
    if (name[0] == '&') {
        int len;
        name++;
	len = xmlStrlen(name);
	if (name[len - 1] == ';')
	    cur->name = xmlStrndup(name, len - 1);
	else
	    cur->name = xmlStrndup(name, len);
    } else
	cur->name = xmlStrdup(name);

    ent = xmlGetDocEntity(doc, cur->name);
    if (ent != NULL) {
	cur->content = ent->content;
	/*
	 * The parent pointer in entity is a DTD pointer and thus is NOT
	 * updated.  Not sure if this is 100% correct.
	 *  -George
	 */
	cur->children = (xmlNodePtr) ent;
	cur->last = (xmlNodePtr) ent;
    }
    return(cur);
}

/**
 * xmlNewDocText:
 * @doc: the document
 * @content:  the text content
 *
 * Creation of a new text node within a document.
 * Returns a pointer to the new node object.
 */
xmlNodePtr
xmlNewDocText(xmlDocPtr doc, const xmlChar *content) {
    xmlNodePtr cur;

    cur = xmlNewText(content);
    if (cur != NULL) cur->doc = doc;
    return(cur);
}

/**
 * xmlNewTextLen:
 * @content:  the text content
 * @len:  the text len.
 *
 * Creation of a new text node with an extra parameter for the content's length
 * Returns a pointer to the new node object.
 */
xmlNodePtr
xmlNewTextLen(const xmlChar *content, int len) {
    xmlNodePtr cur;

    /*
     * Allocate a new node and fill the fields.
     */
    cur = (xmlNodePtr) xmlMalloc(sizeof(xmlNode));
    if (cur == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewTextLen : malloc failed\n");
	return(NULL);
    }
    memset(cur, 0, sizeof(xmlNode));
    cur->type = XML_TEXT_NODE;

    cur->name = xmlStringText;
    if (content != NULL) {
	cur->content = xmlStrndup(content, len);
    }
    return(cur);
}

/**
 * xmlNewDocTextLen:
 * @doc: the document
 * @content:  the text content
 * @len:  the text len.
 *
 * Creation of a new text node with an extra content length parameter. The
 * text node pertain to a given document.
 * Returns a pointer to the new node object.
 */
xmlNodePtr
xmlNewDocTextLen(xmlDocPtr doc, const xmlChar *content, int len) {
    xmlNodePtr cur;

    cur = xmlNewTextLen(content, len);
    if (cur != NULL) cur->doc = doc;
    return(cur);
}

/**
 * xmlNewComment:
 * @content:  the comment content
 *
 * Creation of a new node containing a comment.
 * Returns a pointer to the new node object.
 */
xmlNodePtr
xmlNewComment(const xmlChar *content) {
    xmlNodePtr cur;

    /*
     * Allocate a new node and fill the fields.
     */
    cur = (xmlNodePtr) xmlMalloc(sizeof(xmlNode));
    if (cur == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewComment : malloc failed\n");
	return(NULL);
    }
    memset(cur, 0, sizeof(xmlNode));
    cur->type = XML_COMMENT_NODE;

    cur->name = xmlStringComment;
    if (content != NULL) {
	cur->content = xmlStrdup(content);
    }
    return(cur);
}

/**
 * xmlNewCDataBlock:
 * @doc:  the document
 * @content:  the CDATA block content content
 * @len:  the length of the block
 *
 * Creation of a new node containing a CDATA block.
 * Returns a pointer to the new node object.
 */
xmlNodePtr
xmlNewCDataBlock(xmlDocPtr doc, const xmlChar *content, int len) {
    xmlNodePtr cur;

    /*
     * Allocate a new node and fill the fields.
     */
    cur = (xmlNodePtr) xmlMalloc(sizeof(xmlNode));
    if (cur == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewCDataBlock : malloc failed\n");
	return(NULL);
    }
    memset(cur, 0, sizeof(xmlNode));
    cur->type = XML_CDATA_SECTION_NODE;
    cur->doc = doc;

    if (content != NULL) {
	cur->content = xmlStrndup(content, len);
    }
    return(cur);
}

/**
 * xmlNewDocComment:
 * @doc:  the document
 * @content:  the comment content
 *
 * Creation of a new node containing a comment within a document.
 * Returns a pointer to the new node object.
 */
xmlNodePtr
xmlNewDocComment(xmlDocPtr doc, const xmlChar *content) {
    xmlNodePtr cur;

    cur = xmlNewComment(content);
    if (cur != NULL) cur->doc = doc;
    return(cur);
}

/**
 * xmlSetTreeDoc:
 * @tree:  the top element
 * @doc:  the document
 *
 * update all nodes under the tree to point to the right document
 */
void
xmlSetTreeDoc(xmlNodePtr tree, xmlDocPtr doc) {
    xmlAttrPtr prop;

    if (tree == NULL)
	return;
    if (tree->doc != doc) {
	if(tree->type == XML_ELEMENT_NODE) {
	    prop = tree->properties;
	    while (prop != NULL) {
		prop->doc = doc;
		xmlSetListDoc(prop->children, doc);
		prop = prop->next;
	    }
	}
	if (tree->children != NULL)
	    xmlSetListDoc(tree->children, doc);
	tree->doc = doc;
    }
}

/**
 * xmlSetListDoc:
 * @list:  the first element
 * @doc:  the document
 *
 * update all nodes in the list to point to the right document
 */
void
xmlSetListDoc(xmlNodePtr list, xmlDocPtr doc) {
    xmlNodePtr cur;

    if (list == NULL)
	return;
    cur = list;
    while (cur != NULL) {
	if (cur->doc != doc)
	    xmlSetTreeDoc(cur, doc);
	cur = cur->next;
    }
}


/**
 * xmlNewChild:
 * @parent:  the parent node
 * @ns:  a namespace if any
 * @name:  the name of the child
 * @content:  the XML content of the child if any.
 *
 * Creation of a new child element, added at the end of @parent children list.
 * @ns and @content parameters are optional (NULL). If content is non NULL,
 * a child list containing the TEXTs and ENTITY_REFs node will be created.
 * NOTE: @content is supposed to be a piece of XML CDATA, so it allow entities
 *       references, but XML special chars need to be escaped first by using
 *       xmlEncodeEntitiesReentrant(). Use xmlNewTextChild() if entities
 *       support is not needed.
 *
 * Returns a pointer to the new node object.
 */
xmlNodePtr
xmlNewChild(xmlNodePtr parent, xmlNsPtr ns,
            const xmlChar *name, const xmlChar *content) {
    xmlNodePtr cur, prev;

    if (parent == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewChild : parent == NULL\n");
#endif
	return(NULL);
    }

    if (name == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewChild : name == NULL\n");
#endif
	return(NULL);
    }

    /*
     * Allocate a new node
     */
    if (parent->type == XML_ELEMENT_NODE) {
	if (ns == NULL)
	    cur = xmlNewDocNode(parent->doc, parent->ns, name, content);
	else
	    cur = xmlNewDocNode(parent->doc, ns, name, content);
    } else if ((parent->type == XML_DOCUMENT_NODE) ||
	       (parent->type == XML_HTML_DOCUMENT_NODE)) {
	if (ns == NULL)
	    cur = xmlNewDocNode((xmlDocPtr) parent, NULL, name, content);
	else
	    cur = xmlNewDocNode((xmlDocPtr) parent, ns, name, content);
    } else if (parent->type == XML_DOCUMENT_FRAG_NODE) {
	    cur = xmlNewDocNode( parent->doc, ns, name, content);
    } else {
	return(NULL);
    }
    if (cur == NULL) return(NULL);

    /*
     * add the new element at the end of the children list.
     */
    cur->type = XML_ELEMENT_NODE;
    cur->parent = parent;
    cur->doc = parent->doc;
    if (parent->children == NULL) {
        parent->children = cur;
	parent->last = cur;
    } else {
        prev = parent->last;
	prev->next = cur;
	cur->prev = prev;
	parent->last = cur;
    }

    return(cur);
}

/**
 * xmlAddNextSibling:
 * @cur:  the child node
 * @elem:  the new node
 *
 * Add a new node @elem as the next sibling of @cur
 * If the new node was already inserted in a document it is
 * first unlinked from its existing context.
 * As a result of text merging @elem may be freed.
 * If the new node is ATTRIBUTE, it is added into properties instead of children.
 * If there is an attribute with equal name, it is first destroyed. 
 *
 * Returns the new node or NULL in case of error.
 */
xmlNodePtr
xmlAddNextSibling(xmlNodePtr cur, xmlNodePtr elem) {
    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlAddNextSibling : cur == NULL\n");
#endif
	return(NULL);
    }
    if (elem == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlAddNextSibling : elem == NULL\n");
#endif
	return(NULL);
    }

    xmlUnlinkNode(elem);

    if (elem->type == XML_TEXT_NODE) {
	if (cur->type == XML_TEXT_NODE) {
	    xmlNodeAddContent(cur, elem->content);
	    xmlFreeNode(elem);
	    return(cur);
	}
	if ((cur->next != NULL) && (cur->next->type == XML_TEXT_NODE) &&
            (cur->name == cur->next->name)) {
	    xmlChar *tmp;

	    tmp = xmlStrdup(elem->content);
	    tmp = xmlStrcat(tmp, cur->next->content);
	    xmlNodeSetContent(cur->next, tmp);
	    xmlFree(tmp);
	    xmlFreeNode(elem);
	    return(cur->next);
	}
    } else if (elem->type == XML_ATTRIBUTE_NODE) {
        /* check if an attribute with the same name exists */
	xmlAttrPtr attr;

	if (elem->ns == NULL)
	    attr = xmlHasProp(cur->parent, elem->name);
	else
	    attr = xmlHasNsProp(cur->parent, elem->name, elem->ns->href);
	if ((attr != NULL) && (attr != (xmlAttrPtr) elem)) {
	    /* different instance, destroy it (attributes must be unique) */
	    xmlFreeProp(attr);
	}
    }

    if (elem->doc != cur->doc) {
	xmlSetTreeDoc(elem, cur->doc);
    }
    elem->parent = cur->parent;
    elem->prev = cur;
    elem->next = cur->next;
    cur->next = elem;
    if (elem->next != NULL)
	elem->next->prev = elem;
    if ((elem->parent != NULL) && (elem->parent->last == cur) && (elem->type != XML_ATTRIBUTE_NODE))
	elem->parent->last = elem;
    return(elem);
}

/**
 * xmlAddPrevSibling:
 * @cur:  the child node
 * @elem:  the new node
 *
 * Add a new node @elem as the previous sibling of @cur
 * merging adjacent TEXT nodes (@elem may be freed)
 * If the new node was already inserted in a document it is
 * first unlinked from its existing context.
 * If the new node is ATTRIBUTE, it is added into properties instead of children.
 * If there is an attribute with equal name, it is first destroyed. 
 *
 * Returns the new node or NULL in case of error.
 */
xmlNodePtr
xmlAddPrevSibling(xmlNodePtr cur, xmlNodePtr elem) {
    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlAddPrevSibling : cur == NULL\n");
#endif
	return(NULL);
    }
    if (elem == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlAddPrevSibling : elem == NULL\n");
#endif
	return(NULL);
    }

    xmlUnlinkNode(elem);

    if (elem->type == XML_TEXT_NODE) {
	if (cur->type == XML_TEXT_NODE) {
	    xmlChar *tmp;

	    tmp = xmlStrdup(elem->content);
	    tmp = xmlStrcat(tmp, cur->content);
	    xmlNodeSetContent(cur, tmp);
	    xmlFree(tmp);
	    xmlFreeNode(elem);
	    return(cur);
	}
	if ((cur->prev != NULL) && (cur->prev->type == XML_TEXT_NODE) &&
            (cur->name == cur->prev->name)) {
	    xmlNodeAddContent(cur->prev, elem->content);
	    xmlFreeNode(elem);
	    return(cur->prev);
	}
    } else if (elem->type == XML_ATTRIBUTE_NODE) {
        /* check if an attribute with the same name exists */
	xmlAttrPtr attr;

	if (elem->ns == NULL)
	    attr = xmlHasProp(cur->parent, elem->name);
	else
	    attr = xmlHasNsProp(cur->parent, elem->name, elem->ns->href);
	if ((attr != NULL) && (attr != (xmlAttrPtr) elem)) {
	    /* different instance, destroy it (attributes must be unique) */
	    xmlFreeProp(attr);
	}
    }

    if (elem->doc != cur->doc) {
	xmlSetTreeDoc(elem, cur->doc);
    }
    elem->parent = cur->parent;
    elem->next = cur;
    elem->prev = cur->prev;
    cur->prev = elem;
    if (elem->prev != NULL)
	elem->prev->next = elem;
    if (elem->parent != NULL) {
	if (elem->type == XML_ATTRIBUTE_NODE) {
	    if (elem->parent->properties == (xmlAttrPtr) cur) {
		elem->parent->properties = (xmlAttrPtr) elem;
	    }
	} else {
	    if (elem->parent->children == cur) {
		elem->parent->children = elem;
	    }
	}
    }
    return(elem);
}

/**
 * xmlAddSibling:
 * @cur:  the child node
 * @elem:  the new node
 *
 * Add a new element @elem to the list of siblings of @cur
 * merging adjacent TEXT nodes (@elem may be freed)
 * If the new element was already inserted in a document it is
 * first unlinked from its existing context.
 *
 * Returns the new element or NULL in case of error.
 */
xmlNodePtr
xmlAddSibling(xmlNodePtr cur, xmlNodePtr elem) {
    xmlNodePtr parent;

    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlAddSibling : cur == NULL\n");
#endif
	return(NULL);
    }

    if (elem == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlAddSibling : elem == NULL\n");
#endif
	return(NULL);
    }

    /*
     * Constant time is we can rely on the ->parent->last to find
     * the last sibling.
     */
    if ((cur->parent != NULL) && 
	(cur->parent->children != NULL) &&
	(cur->parent->last != NULL) &&
	(cur->parent->last->next == NULL)) {
	cur = cur->parent->last;
    } else {
	while (cur->next != NULL) cur = cur->next;
    }

    xmlUnlinkNode(elem);

    if ((cur->type == XML_TEXT_NODE) && (elem->type == XML_TEXT_NODE)) {
	xmlNodeAddContent(cur, elem->content);
	xmlFreeNode(elem);
	return(cur);
    }

    if (elem->doc != cur->doc) {
	xmlSetTreeDoc(elem, cur->doc);
    }
    parent = cur->parent;
    elem->prev = cur;
    elem->next = NULL;
    elem->parent = parent;
    cur->next = elem;
    if (parent != NULL)
	parent->last = elem;

    return(elem);
}

/**
 * xmlAddChildList:
 * @parent:  the parent node
 * @cur:  the first node in the list
 *
 * Add a list of node at the end of the child list of the parent
 * merging adjacent TEXT nodes (@cur may be freed)
 *
 * Returns the last child or NULL in case of error.
 */
xmlNodePtr
xmlAddChildList(xmlNodePtr parent, xmlNodePtr cur) {
    xmlNodePtr prev;

    if (parent == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlAddChildList : parent == NULL\n");
#endif
	return(NULL);
    }

    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlAddChildList : child == NULL\n");
#endif
	return(NULL);
    }

    if ((cur->doc != NULL) && (parent->doc != NULL) &&
        (cur->doc != parent->doc)) {
#ifdef DEBUG_TREE
	xmlGenericError(xmlGenericErrorContext,
		"Elements moved to a different document\n");
#endif
    }

    /*
     * add the first element at the end of the children list.
     */
    if (parent->children == NULL) {
        parent->children = cur;
    } else {
	/*
	 * If cur and parent->last both are TEXT nodes, then merge them.
	 */
	if ((cur->type == XML_TEXT_NODE) && 
	    (parent->last->type == XML_TEXT_NODE) &&
	    (cur->name == parent->last->name)) {
	    xmlNodeAddContent(parent->last, cur->content);
	    /*
	     * if it's the only child, nothing more to be done.
	     */
	    if (cur->next == NULL) {
		xmlFreeNode(cur);
		return(parent->last);
	    }
	    prev = cur;
	    cur = cur->next;
	    xmlFreeNode(prev);
	}
        prev = parent->last;
	prev->next = cur;
	cur->prev = prev;
    }
    while (cur->next != NULL) {
	cur->parent = parent;
	if (cur->doc != parent->doc) {
	    xmlSetTreeDoc(cur, parent->doc);
	}
        cur = cur->next;
    }
    cur->parent = parent;
    cur->doc = parent->doc; /* the parent may not be linked to a doc ! */
    parent->last = cur;

    return(cur);
}

/**
 * xmlAddChild:
 * @parent:  the parent node
 * @cur:  the child node
 *
 * Add a new node to @parent, at the end of the child (or property) list
 * merging adjacent TEXT nodes (in which case @cur is freed)
 * If the new node was already inserted in a document it is
 * first unlinked from its existing context.
 * If the new node is ATTRIBUTE, it is added into properties instead of children.
 * If there is an attribute with equal name, it is first destroyed. 
 *
 * Returns the child or NULL in case of error.
 */
xmlNodePtr
xmlAddChild(xmlNodePtr parent, xmlNodePtr cur) {
    xmlNodePtr prev;

    if (parent == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlAddChild : parent == NULL\n");
#endif
	return(NULL);
    }

    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlAddChild : child == NULL\n");
#endif
	return(NULL);
    }

    /*
     * If cur is a TEXT node, merge its content with adjacent TEXT nodes
     * cur is then freed.
     */
    if (cur->type == XML_TEXT_NODE) {
	if ((parent->type == XML_TEXT_NODE) &&
	    (parent->content != NULL)) {
	    xmlNodeAddContent(parent, cur->content);
	    xmlFreeNode(cur);
	    return(parent);
	}
	if ((parent->last != NULL) && (parent->last->type == XML_TEXT_NODE) &&
	    (parent->last->name == cur->name)) {
	    xmlNodeAddContent(parent->last, cur->content);
	    xmlFreeNode(cur);
	    return(parent->last);
	}
    }

    /*
     * add the new element at the end of the children list.
     */
    cur->parent = parent;
    if (cur->doc != parent->doc) {
	xmlSetTreeDoc(cur, parent->doc);
    }

    /*
     * Coalescing
     */
    if ((parent->type == XML_TEXT_NODE) &&
	(parent->content != NULL)) {
	xmlNodeAddContent(parent, cur->content);
	xmlFreeNode(cur);
	return(parent);
    }
    if (cur->type == XML_ATTRIBUTE_NODE) {
	if (parent->properties == NULL) {
	    parent->properties = (xmlAttrPtr) cur;
	} else {
	    /* check if an attribute with the same name exists */
	    xmlAttrPtr lastattr;

	    if (cur->ns == NULL)
		lastattr = xmlHasProp(parent, cur->name);
	    else
		lastattr = xmlHasNsProp(parent, cur->name, cur->ns->href);
	    if ((lastattr != NULL) && (lastattr != (xmlAttrPtr) cur)) {
		/* different instance, destroy it (attributes must be unique) */
		xmlFreeProp(lastattr);
	    }
	    /* find the end */
	    lastattr = parent->properties;
	    while (lastattr->next != NULL) {
		lastattr = lastattr->next;
	    }
	    lastattr->next = (xmlAttrPtr) cur;
	    ((xmlAttrPtr) cur)->prev = lastattr;
	}
    } else {
	if (parent->children == NULL) {
	    parent->children = cur;
	    parent->last = cur;
	} else {
	    prev = parent->last;
	    prev->next = cur;
	    cur->prev = prev;
	    parent->last = cur;
	}
    }
    return(cur);
}

/**
 * xmlGetLastChild:
 * @parent:  the parent node
 *
 * Search the last child of a node.
 * Returns the last child or NULL if none.
 */
xmlNodePtr
xmlGetLastChild(xmlNodePtr parent) {
    if (parent == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlGetLastChild : parent == NULL\n");
#endif
	return(NULL);
    }
    return(parent->last);
}

/**
 * xmlFreeNodeList:
 * @cur:  the first node in the list
 *
 * Free a node and all its siblings, this is a recursive behaviour, all
 * the children are freed too.
 */
void
xmlFreeNodeList(xmlNodePtr cur) {
    xmlNodePtr next;
    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlFreeNodeList : node == NULL\n");
#endif
	return;
    }
    if (cur->type == XML_NAMESPACE_DECL) {
	xmlFreeNsList((xmlNsPtr) cur);
	return;
    }
    while (cur != NULL) {
        next = cur->next;
	/* unroll to speed up freeing the document */
	if (cur->type != XML_DTD_NODE) {
	    if ((cur->children != NULL) &&
		(cur->type != XML_ENTITY_REF_NODE))
		xmlFreeNodeList(cur->children);
	    if (((cur->type == XML_ELEMENT_NODE) ||
		 (cur->type == XML_XINCLUDE_START) ||
		 (cur->type == XML_XINCLUDE_END)) &&
		(cur->properties != NULL))
		xmlFreePropList(cur->properties);
	    if ((cur->type != XML_ELEMENT_NODE) &&
		(cur->type != XML_XINCLUDE_START) &&
		(cur->type != XML_XINCLUDE_END) &&
		(cur->type != XML_ENTITY_REF_NODE)) {
		if (cur->content != NULL) xmlFree(cur->content);
	    }
	    if (((cur->type == XML_ELEMENT_NODE) ||
	         (cur->type == XML_XINCLUDE_START) ||
		 (cur->type == XML_XINCLUDE_END)) &&
		(cur->nsDef != NULL))
		xmlFreeNsList(cur->nsDef);

	    /*
	     * When a node is a text node or a comment, it uses a global static
	     * variable for the name of the node.
	     *
	     * The xmlStrEqual comparisons need to be done when (happened with
	     * XML::libXML and XML::libXSLT) the library is included twice
	     * statically in the binary and a tree allocated by one occurrence
	     * of the lib gets freed by the other occurrence, in this case
	     * the string addresses compare are not sufficient.
	     */
	    if ((cur->name != NULL) &&
		(cur->name != xmlStringText) &&
		(cur->name != xmlStringTextNoenc) &&
		(cur->name != xmlStringComment)) {
		if (cur->type == XML_TEXT_NODE) {
		    if ((!xmlStrEqual(cur->name, xmlStringText)) &&
			(!xmlStrEqual(cur->name, xmlStringTextNoenc)))
			xmlFree((char *) cur->name);
		} else if (cur->type == XML_COMMENT_NODE) {
		    if (!xmlStrEqual(cur->name, xmlStringComment))
			xmlFree((char *) cur->name);
		} else
		    xmlFree((char *) cur->name);
	    }
	    /* TODO : derecursivate this function */
	    xmlFree(cur);
	}
	cur = next;
    }
}

/**
 * xmlFreeNode:
 * @cur:  the node
 *
 * Free a node, this is a recursive behaviour, all the children are freed too.
 * This doesn't unlink the child from the list, use xmlUnlinkNode() first.
 */
void
xmlFreeNode(xmlNodePtr cur) {
    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlFreeNode : node == NULL\n");
#endif
	return;
    }
    /* use xmlFreeDtd for DTD nodes */
    if (cur->type == XML_DTD_NODE) {
	xmlFreeDtd((xmlDtdPtr) cur);
	return;
    }
    if (cur->type == XML_NAMESPACE_DECL) {
	xmlFreeNs((xmlNsPtr) cur);
        return;
    }
    if (cur->type == XML_ATTRIBUTE_NODE) {
	xmlFreeProp((xmlAttrPtr) cur);
	return;
    }
    if ((cur->children != NULL) &&
	(cur->type != XML_ENTITY_REF_NODE))
	xmlFreeNodeList(cur->children);
    if (((cur->type == XML_ELEMENT_NODE) ||
	 (cur->type == XML_XINCLUDE_START) ||
	 (cur->type == XML_XINCLUDE_END)) &&
	(cur->properties != NULL))
	xmlFreePropList(cur->properties);
    if ((cur->type != XML_ELEMENT_NODE) &&
	(cur->content != NULL) &&
	(cur->type != XML_ENTITY_REF_NODE) &&
	(cur->type != XML_XINCLUDE_END) &&
	(cur->type != XML_XINCLUDE_START)) {
	xmlFree(cur->content);
    }

    /*
     * When a node is a text node or a comment, it uses a global static
     * variable for the name of the node.
     *
     * The xmlStrEqual comparisons need to be done when (happened with
     * XML::libXML and XML::libXSLT) the library is included twice statically
     * in the binary and a tree allocated by one occurence of the lib gets
     * freed by the other occurrence, in this case the string addresses compare
     * are not sufficient.
     */
    if ((cur->name != NULL) &&
	(cur->name != xmlStringText) &&
	(cur->name != xmlStringTextNoenc) &&
	(cur->name != xmlStringComment)) {
	if (cur->type == XML_TEXT_NODE) {
            if ((!xmlStrEqual(cur->name, xmlStringText)) &&
		(!xmlStrEqual(cur->name, xmlStringTextNoenc)))
		xmlFree((char *) cur->name);
	} else if (cur->type == XML_COMMENT_NODE) {
            if (!xmlStrEqual(cur->name, xmlStringComment))
		xmlFree((char *) cur->name);
	} else
	    xmlFree((char *) cur->name);
    }

    if (((cur->type == XML_ELEMENT_NODE) ||
	 (cur->type == XML_XINCLUDE_START) ||
	 (cur->type == XML_XINCLUDE_END)) &&
	(cur->nsDef != NULL))
	xmlFreeNsList(cur->nsDef);
    xmlFree(cur);
}

/**
 * xmlUnlinkNode:
 * @cur:  the node
 *
 * Unlink a node from it's current context, the node is not freed
 */
void
xmlUnlinkNode(xmlNodePtr cur) {
    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlUnlinkNode : node == NULL\n");
#endif
	return;
    }
    if (cur->type == XML_DTD_NODE) {
	xmlDocPtr doc;
	doc = cur->doc;
	if (doc->intSubset == (xmlDtdPtr) cur)
	    doc->intSubset = NULL;
	if (doc->extSubset == (xmlDtdPtr) cur)
	    doc->extSubset = NULL;
    }
    if (cur->parent != NULL) {
	xmlNodePtr parent;
	parent = cur->parent;
	if (cur->type == XML_ATTRIBUTE_NODE) {
	    if (parent->properties == (xmlAttrPtr) cur)
		parent->properties = ((xmlAttrPtr) cur)->next;
	} else {
	    if (parent->children == cur)
		parent->children = cur->next;
	    if (parent->last == cur)
		parent->last = cur->prev;
	}
	cur->parent = NULL;
    }
    if (cur->next != NULL)
        cur->next->prev = cur->prev;
    if (cur->prev != NULL)
        cur->prev->next = cur->next;
    cur->next = cur->prev = NULL;
}

/**
 * xmlReplaceNode:
 * @old:  the old node
 * @cur:  the node
 *
 * Unlink the old node from it's current context, prune the new one
 * at the same place. If @cur was already inserted in a document it is
 * first unlinked from its existing context.
 *
 * Returns the @old node
 */
xmlNodePtr
xmlReplaceNode(xmlNodePtr old, xmlNodePtr cur) {
    if (old == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlReplaceNode : old == NULL\n");
#endif
	return(NULL);
    }
    if (cur == NULL) {
	xmlUnlinkNode(old);
	return(old);
    }
    if (cur == old) {
	return(old);
    }
    if ((old->type==XML_ATTRIBUTE_NODE) && (cur->type!=XML_ATTRIBUTE_NODE)) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlReplaceNode : Trying to replace attribute node with other node type\n");
#endif
	return(old);
    }
    if ((cur->type==XML_ATTRIBUTE_NODE) && (old->type!=XML_ATTRIBUTE_NODE)) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlReplaceNode : Trying to replace a non-attribute node with attribute node\n");
#endif
	return(old);
    }
    if ((old->type==XML_ATTRIBUTE_NODE) && (cur->type!=XML_ATTRIBUTE_NODE)) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlReplaceNode : Trying to replace attribute node with other node type\n");
#endif
	return(old);
    }
    if ((cur->type==XML_ATTRIBUTE_NODE) && (old->type!=XML_ATTRIBUTE_NODE)) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlReplaceNode : Trying to replace a non-attribute node with attribute node\n");
#endif
	return(old);
    }
    xmlUnlinkNode(cur);
    cur->doc = old->doc;
    cur->parent = old->parent;
    cur->next = old->next;
    if (cur->next != NULL)
	cur->next->prev = cur;
    cur->prev = old->prev;
    if (cur->prev != NULL)
	cur->prev->next = cur;
    if (cur->parent != NULL) {
	if (cur->type == XML_ATTRIBUTE_NODE) {
	    if (cur->parent->properties == (xmlAttrPtr)old)
		cur->parent->properties = ((xmlAttrPtr) cur);
	} else {
	    if (cur->parent->children == old)
		cur->parent->children = cur;
	    if (cur->parent->last == old)
		cur->parent->last = cur;
	}
    }
    old->next = old->prev = NULL;
    old->parent = NULL;
    return(old);
}

/************************************************************************
 *									*
 *		Copy operations						*
 *									*
 ************************************************************************/
 
/**
 * xmlCopyNamespace:
 * @cur:  the namespace
 *
 * Do a copy of the namespace.
 *
 * Returns: a new #xmlNsPtr, or NULL in case of error.
 */
xmlNsPtr
xmlCopyNamespace(xmlNsPtr cur) {
    xmlNsPtr ret;

    if (cur == NULL) return(NULL);
    switch (cur->type) {
	case XML_LOCAL_NAMESPACE:
	    ret = xmlNewNs(NULL, cur->href, cur->prefix);
	    break;
	default:
#ifdef DEBUG_TREE
	    xmlGenericError(xmlGenericErrorContext,
		    "xmlCopyNamespace: invalid type %d\n", cur->type);
#endif
	    return(NULL);
    }
    return(ret);
}

/**
 * xmlCopyNamespaceList:
 * @cur:  the first namespace
 *
 * Do a copy of an namespace list.
 *
 * Returns: a new #xmlNsPtr, or NULL in case of error.
 */
xmlNsPtr
xmlCopyNamespaceList(xmlNsPtr cur) {
    xmlNsPtr ret = NULL;
    xmlNsPtr p = NULL,q;

    while (cur != NULL) {
        q = xmlCopyNamespace(cur);
	if (p == NULL) {
	    ret = p = q;
	} else {
	    p->next = q;
	    p = q;
	}
	cur = cur->next;
    }
    return(ret);
}

static xmlNodePtr
xmlStaticCopyNodeList(xmlNodePtr node, xmlDocPtr doc, xmlNodePtr parent);
/**
 * xmlCopyProp:
 * @target:  the element where the attribute will be grafted
 * @cur:  the attribute
 *
 * Do a copy of the attribute.
 *
 * Returns: a new #xmlAttrPtr, or NULL in case of error.
 */
xmlAttrPtr
xmlCopyProp(xmlNodePtr target, xmlAttrPtr cur) {
    xmlAttrPtr ret;

    if (cur == NULL) return(NULL);
    if (target != NULL)
	ret = xmlNewDocProp(target->doc, cur->name, NULL);
    else if (cur->parent != NULL)
	ret = xmlNewDocProp(cur->parent->doc, cur->name, NULL);
    else if (cur->children != NULL)
	ret = xmlNewDocProp(cur->children->doc, cur->name, NULL);
    else
	ret = xmlNewDocProp(NULL, cur->name, NULL);
    if (ret == NULL) return(NULL);
    ret->parent = target;

    if ((cur->ns != NULL) && (target != NULL)) {
      xmlNsPtr ns;
/*
 *      if (target->doc) 
 *	ns = xmlSearchNs(target->doc, target, cur->ns->prefix);
 *      else if (cur->doc)  / * target may not yet have a doc : KPI * /
 *	ns = xmlSearchNs(cur->doc, target, cur->ns->prefix);       
 *      else
 *	ns = NULL;
 *	ret->ns = ns;
 */
      ns = xmlSearchNs(target->doc, target, cur->ns->prefix);
      if (ns == NULL) {
        /*
         * Humm, we are copying an element whose namespace is defined
         * out of the new tree scope. Search it in the original tree
         * and add it at the top of the new tree
         */
        ns = xmlSearchNs(cur->doc, cur->parent, cur->ns->prefix);
        if (ns != NULL) {
          xmlNodePtr root = target;
          xmlNodePtr pred = NULL;

          while (root->parent != NULL) {
            pred = root;
            root = root->parent;
          }
          if (root == (xmlNodePtr) target->doc) {
            /* correct possibly cycling above the document elt */
            root = pred;
          }
          ret->ns = xmlNewNs(root, ns->href, ns->prefix);
        }
      } else {
        /*
         * we have to find something appropriate here since
         * we cant be sure, that the namespce we found is identified
         * by the prefix
         */
        if (xmlStrEqual(ns->href, cur->ns->href)) {
          /* this is the nice case */
          ret->ns = ns;
        } else {
          /*
           * we are in trouble: we need a new reconcilied namespace.
           * This is expensive
           */
          ret->ns = xmlNewReconciliedNs(target->doc, target, cur->ns);
        }
      }
 
    } else
        ret->ns = NULL;

    if (cur->children != NULL) {
	xmlNodePtr tmp;

	ret->children = xmlStaticCopyNodeList(cur->children, ret->doc, (xmlNodePtr) ret);
	ret->last = NULL;
	tmp = ret->children;
	while (tmp != NULL) {
	    /* tmp->parent = (xmlNodePtr)ret; */
	    if (tmp->next == NULL)
	        ret->last = tmp;
	    tmp = tmp->next;
	}
    }
    /*
     * Try to handle IDs
     */
    if ((target!= NULL) && (cur!= NULL) &&
	(target->doc != NULL) && (cur->doc != NULL) &&
	(cur->doc->ids != NULL) && (cur->parent != NULL)) {
	if (xmlIsID(cur->doc, cur->parent, cur)) {
	    xmlChar *id;

	    id = xmlNodeListGetString(cur->doc, cur->children, 1);
	    if (id != NULL) {
		xmlAddID(NULL, target->doc, id, ret);
		xmlFree(id);
	    }
	}
    }
    return(ret);
}

/**
 * xmlCopyPropList:
 * @target:  the element where the attributes will be grafted
 * @cur:  the first attribute
 *
 * Do a copy of an attribute list.
 *
 * Returns: a new #xmlAttrPtr, or NULL in case of error.
 */
xmlAttrPtr
xmlCopyPropList(xmlNodePtr target, xmlAttrPtr cur) {
    xmlAttrPtr ret = NULL;
    xmlAttrPtr p = NULL,q;

    while (cur != NULL) {
        q = xmlCopyProp(target, cur);
	if (p == NULL) {
	    ret = p = q;
	} else {
	    p->next = q;
	    q->prev = p;
	    p = q;
	}
	cur = cur->next;
    }
    return(ret);
}

/*
 * NOTE about the CopyNode operations !
 *
 * They are split into external and internal parts for one
 * tricky reason: namespaces. Doing a direct copy of a node
 * say RPM:Copyright without changing the namespace pointer to
 * something else can produce stale links. One way to do it is
 * to keep a reference counter but this doesn't work as soon
 * as one move the element or the subtree out of the scope of
 * the existing namespace. The actual solution seems to add
 * a copy of the namespace at the top of the copied tree if
 * not available in the subtree.
 * Hence two functions, the public front-end call the inner ones
 */

static xmlNodePtr
xmlStaticCopyNode(const xmlNodePtr node, xmlDocPtr doc, xmlNodePtr parent,
                  int recursive) {
    xmlNodePtr ret;

    if (node == NULL) return(NULL);
    switch (node->type) {
        case XML_TEXT_NODE:
        case XML_CDATA_SECTION_NODE:
        case XML_ELEMENT_NODE:
        case XML_DOCUMENT_FRAG_NODE:
        case XML_ENTITY_REF_NODE:
        case XML_ENTITY_NODE:
        case XML_PI_NODE:
        case XML_COMMENT_NODE:
        case XML_XINCLUDE_START:
        case XML_XINCLUDE_END:
	    break;
        case XML_ATTRIBUTE_NODE:
	    return((xmlNodePtr) xmlCopyProp(parent, (xmlAttrPtr) node));
        case XML_NAMESPACE_DECL:
	    return((xmlNodePtr) xmlCopyNamespaceList((xmlNsPtr) node));
	    
        case XML_DOCUMENT_NODE:
        case XML_HTML_DOCUMENT_NODE:
#ifdef LIBXML_DOCB_ENABLED
        case XML_DOCB_DOCUMENT_NODE:
#endif
	    return((xmlNodePtr) xmlCopyDoc((xmlDocPtr) node, recursive));
        case XML_DOCUMENT_TYPE_NODE:
        case XML_NOTATION_NODE:
        case XML_DTD_NODE:
        case XML_ELEMENT_DECL:
        case XML_ATTRIBUTE_DECL:
        case XML_ENTITY_DECL:
            return(NULL);
    }

    /*
     * Allocate a new node and fill the fields.
     */
    ret = (xmlNodePtr) xmlMalloc(sizeof(xmlNode));
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlStaticCopyNode : malloc failed\n");
	return(NULL);
    }
    memset(ret, 0, sizeof(xmlNode));
    ret->type = node->type;

    ret->doc = doc;
    ret->parent = parent; 
    if (node->name == xmlStringText)
	ret->name = xmlStringText;
    else if (node->name == xmlStringTextNoenc)
	ret->name = xmlStringTextNoenc;
    else if (node->name == xmlStringComment)
	ret->name = xmlStringComment;
    else if (node->name != NULL)
	ret->name = xmlStrdup(node->name);
    if ((node->type != XML_ELEMENT_NODE) &&
	(node->content != NULL) &&
	(node->type != XML_ENTITY_REF_NODE) &&
	(node->type != XML_XINCLUDE_END) &&
	(node->type != XML_XINCLUDE_START)) {
	ret->content = xmlStrdup(node->content);
    }else{
      if (node->type == XML_ELEMENT_NODE)
        ret->content = (void*)(long) node->content;
    }
    if (parent != NULL) {
	xmlNodePtr tmp;

        tmp = xmlAddChild(parent, ret);
	/* node could have coalesced */
	if (tmp != ret)
	    return(tmp);
    }
    
    if (!recursive) return(ret);
    if (node->nsDef != NULL)
        ret->nsDef = xmlCopyNamespaceList(node->nsDef);

    if (node->ns != NULL) {
        xmlNsPtr ns;

	ns = xmlSearchNs(doc, ret, node->ns->prefix);
	if (ns == NULL) {
	    /*
	     * Humm, we are copying an element whose namespace is defined
	     * out of the new tree scope. Search it in the original tree
	     * and add it at the top of the new tree
	     */
	    ns = xmlSearchNs(node->doc, node, node->ns->prefix);
	    if (ns != NULL) {
	        xmlNodePtr root = ret;

		while (root->parent != NULL) root = root->parent;
		ret->ns = xmlNewNs(root, ns->href, ns->prefix);
	    }
	} else {
	    /*
	     * reference the existing namespace definition in our own tree.
	     */
	    ret->ns = ns;
	}
    }
    if (node->properties != NULL)
        ret->properties = xmlCopyPropList(ret, node->properties);
    if (node->type == XML_ENTITY_REF_NODE) {
	if ((doc == NULL) || (node->doc != doc)) {
	    /*
	     * The copied node will go into a separate document, so
	     * to avoid dangling references to the ENTITY_DECL node
	     * we cannot keep the reference. Try to find it in the
	     * target document.
	     */
	    ret->children = (xmlNodePtr) xmlGetDocEntity(doc, ret->name);
	} else {
            ret->children = node->children;
	}
	ret->last = ret->children;
    } else if (node->children != NULL) {
        ret->children = xmlStaticCopyNodeList(node->children, doc, ret);
	UPDATE_LAST_CHILD_AND_PARENT(ret)
    }
    return(ret);
}

static xmlNodePtr
xmlStaticCopyNodeList(xmlNodePtr node, xmlDocPtr doc, xmlNodePtr parent) {
    xmlNodePtr ret = NULL;
    xmlNodePtr p = NULL,q;

    while (node != NULL) {
	if (node->type == XML_DTD_NODE ) {
	    if (doc == NULL) {
		node = node->next;
		continue;
	    }
	    if (doc->intSubset == NULL) {
		q = (xmlNodePtr) xmlCopyDtd( (xmlDtdPtr) node );
		q->doc = doc;
		q->parent = parent;
		doc->intSubset = (xmlDtdPtr) q;
		xmlAddChild(parent, q);
	    } else {
		q = (xmlNodePtr) doc->intSubset;
		xmlAddChild(parent, q);
	    }
	} else
	    q = xmlStaticCopyNode(node, doc, parent, 1);
	if (ret == NULL) {
	    q->prev = NULL;
	    ret = p = q;
	} else if (p != q) {
	/* the test is required if xmlStaticCopyNode coalesced 2 text nodes */
	    p->next = q;
	    q->prev = p;
	    p = q;
	}
	node = node->next;
    }
    return(ret);
}

/**
 * xmlCopyNode:
 * @node:  the node
 * @recursive:  if 1 do a recursive copy.
 *
 * Do a copy of the node.
 *
 * Returns: a new #xmlNodePtr, or NULL in case of error.
 */
xmlNodePtr
xmlCopyNode(const xmlNodePtr node, int recursive) {
    xmlNodePtr ret;

    ret = xmlStaticCopyNode(node, NULL, NULL, recursive);
    return(ret);
}

/**
 * xmlDocCopyNode:
 * @node:  the node
 * @doc:  the document
 * @recursive:  if 1 do a recursive copy.
 *
 * Do a copy of the node to a given document.
 *
 * Returns: a new #xmlNodePtr, or NULL in case of error.
 */
xmlNodePtr
xmlDocCopyNode(const xmlNodePtr node, xmlDocPtr doc, int recursive) {
    xmlNodePtr ret;

    ret = xmlStaticCopyNode(node, doc, NULL, recursive);
    return(ret);
}

/**
 * xmlCopyNodeList:
 * @node:  the first node in the list.
 *
 * Do a recursive copy of the node list.
 *
 * Returns: a new #xmlNodePtr, or NULL in case of error.
 */
xmlNodePtr xmlCopyNodeList(const xmlNodePtr node) {
    xmlNodePtr ret = xmlStaticCopyNodeList(node, NULL, NULL);
    return(ret);
}

/**
 * xmlCopyDtd:
 * @dtd:  the dtd
 *
 * Do a copy of the dtd.
 *
 * Returns: a new #xmlDtdPtr, or NULL in case of error.
 */
xmlDtdPtr
xmlCopyDtd(xmlDtdPtr dtd) {
    xmlDtdPtr ret;
    xmlNodePtr cur, p = NULL, q;

    if (dtd == NULL) return(NULL);
    ret = xmlNewDtd(NULL, dtd->name, dtd->ExternalID, dtd->SystemID);
    if (ret == NULL) return(NULL);
    if (dtd->entities != NULL)
        ret->entities = (void *) xmlCopyEntitiesTable(
	                    (xmlEntitiesTablePtr) dtd->entities);
    if (dtd->notations != NULL)
        ret->notations = (void *) xmlCopyNotationTable(
	                    (xmlNotationTablePtr) dtd->notations);
    if (dtd->elements != NULL)
        ret->elements = (void *) xmlCopyElementTable(
	                    (xmlElementTablePtr) dtd->elements);
    if (dtd->attributes != NULL)
        ret->attributes = (void *) xmlCopyAttributeTable(
	                    (xmlAttributeTablePtr) dtd->attributes);
    if (dtd->pentities != NULL)
	ret->pentities = (void *) xmlCopyEntitiesTable(
			    (xmlEntitiesTablePtr) dtd->pentities);
    
    cur = dtd->children;
    while (cur != NULL) {
	q = NULL;

	if (cur->type == XML_ENTITY_DECL) {
	    xmlEntityPtr tmp = (xmlEntityPtr) cur;
	    switch (tmp->etype) {
		case XML_INTERNAL_GENERAL_ENTITY:
		case XML_EXTERNAL_GENERAL_PARSED_ENTITY:
		case XML_EXTERNAL_GENERAL_UNPARSED_ENTITY:
		    q = (xmlNodePtr) xmlGetEntityFromDtd(ret, tmp->name);
		    break;
		case XML_INTERNAL_PARAMETER_ENTITY:
		case XML_EXTERNAL_PARAMETER_ENTITY:
    		    q = (xmlNodePtr) 
			xmlGetParameterEntityFromDtd(ret, tmp->name);
		    break;
		case XML_INTERNAL_PREDEFINED_ENTITY:
		    break;
	    }
	} else if (cur->type == XML_ELEMENT_DECL) {
	    xmlElementPtr tmp = (xmlElementPtr) cur;
	    q = (xmlNodePtr)
		xmlGetDtdQElementDesc(ret, tmp->name, tmp->prefix);
	} else if (cur->type == XML_ATTRIBUTE_DECL) {
	    xmlAttributePtr tmp = (xmlAttributePtr) cur;
	    q = (xmlNodePtr) 
		xmlGetDtdQAttrDesc(ret, tmp->elem, tmp->name, tmp->prefix);
	} else if (cur->type == XML_COMMENT_NODE) {
	    q = xmlCopyNode(cur, 0);
	}
	
	if (q == NULL) {
	    cur = cur->next;
	    continue;
	}
	
	if (p == NULL)
	    ret->children = q;
	else
    	    p->next = q;
	
    	q->prev = p;
    	q->parent = (xmlNodePtr) ret;
	q->next = NULL;
	ret->last = q;
    	p = q;
	cur = cur->next;
    }

    return(ret);
}

/**
 * xmlCopyDoc:
 * @doc:  the document
 * @recursive:  if 1 do a recursive copy.
 *
 * Do a copy of the document info. If recursive, the content tree will
 * be copied too as well as DTD, namespaces and entities.
 *
 * Returns: a new #xmlDocPtr, or NULL in case of error.
 */
xmlDocPtr
xmlCopyDoc(xmlDocPtr doc, int recursive) {
    xmlDocPtr ret;

    if (doc == NULL) return(NULL);
    ret = xmlNewDoc(doc->version);
    if (ret == NULL) return(NULL);
    if (doc->name != NULL)
        ret->name = xmlMemStrdup(doc->name);
    if (doc->encoding != NULL)
        ret->encoding = xmlStrdup(doc->encoding);
    ret->charset = doc->charset;
    ret->compression = doc->compression;
    ret->standalone = doc->standalone;
    if (!recursive) return(ret);

    ret->last = NULL;
    ret->children = NULL;
    if (doc->intSubset != NULL) {
        ret->intSubset = xmlCopyDtd(doc->intSubset);
	xmlSetTreeDoc((xmlNodePtr)ret->intSubset, ret);
	ret->intSubset->parent = ret;
    }
    if (doc->oldNs != NULL)
        ret->oldNs = xmlCopyNamespaceList(doc->oldNs);
    if (doc->children != NULL) {
	xmlNodePtr tmp;
	
	ret->children = xmlStaticCopyNodeList(doc->children, ret,
		                               (xmlNodePtr)ret);
	ret->last = NULL;
	tmp = ret->children;
	while (tmp != NULL) {
	    if (tmp->next == NULL)
	        ret->last = tmp;
	    tmp = tmp->next;
	}
    }
    return(ret);
}

/************************************************************************
 *									*
 *		Content access functions				*
 *									*
 ************************************************************************/
 
/**
 * xmlGetLineNo:
 * @node: valid node
 *
 * Get line number of node. this requires activation of this option
 * before invoking the parser by calling xmlLineNumbersDefault(1)
 *
 * Returns the line number if successful, -1 otherwise
 */
long
xmlGetLineNo(xmlNodePtr node)
{
    long result = -1;

    if (!node)
        return result;
    if (node->type == XML_ELEMENT_NODE)
        result = (long) node->content;
    else if ((node->prev != NULL) &&
             ((node->prev->type == XML_ELEMENT_NODE) ||
	      (node->prev->type == XML_TEXT_NODE)))
        result = xmlGetLineNo(node->prev);
    else if ((node->parent != NULL) &&
             ((node->parent->type == XML_ELEMENT_NODE) ||
      	      (node->parent->type == XML_TEXT_NODE)))
        result = xmlGetLineNo(node->parent);

    return result;
}

/**
 * xmlGetNodePath:
 * @node: a node
 *
 * Build a structure based Path for the given node
 *
 * Returns the new path or NULL in case of error. The caller must free
 *     the returned string
 */
xmlChar *
xmlGetNodePath(xmlNodePtr node)
{
    xmlNodePtr cur, tmp, next;
    xmlChar *buffer = NULL, *temp;
    size_t buf_len;
    xmlChar *buf;
    const char *sep;
    const char *name;
    char nametemp[100];
    int occur = 0;

    if (node == NULL)
        return (NULL);

    buf_len = 500;
    buffer = (xmlChar *) xmlMalloc(buf_len * sizeof(xmlChar));
    if (buffer == NULL)
        return (NULL);
    buf = (xmlChar *) xmlMalloc(buf_len * sizeof(xmlChar));
    if (buf == NULL) {
        xmlFree(buffer);
        return (NULL);
    }

    buffer[0] = 0;
    cur = node;
    do {
        name = "";
        sep = "?";
        occur = 0;
        if ((cur->type == XML_DOCUMENT_NODE) ||
            (cur->type == XML_HTML_DOCUMENT_NODE)) {
            if (buffer[0] == '/')
                break;
            sep = "/";
            next = NULL;
        } else if (cur->type == XML_ELEMENT_NODE) {
            sep = "/";
            name = (const char *) cur->name;
            if (cur->ns) {
                snprintf(nametemp, sizeof(nametemp) - 1,
                         "%s:%s", cur->ns->prefix, cur->name);
                nametemp[sizeof(nametemp) - 1] = 0;
                name = nametemp;
            }
            next = cur->parent;

            /*
             * Thumbler index computation
             */
            tmp = cur->prev;
            while (tmp != NULL) {
                if ((tmp->type == XML_ELEMENT_NODE) &&
		    (xmlStrEqual(cur->name, tmp->name)))
                    occur++;
                tmp = tmp->prev;
            }
            if (occur == 0) {
                tmp = cur->next;
                while (tmp != NULL && occur == 0) {
                    if ((tmp->type == XML_ELEMENT_NODE) &&
			(xmlStrEqual(cur->name, tmp->name)))
                        occur++;
                    tmp = tmp->next;
                }
                if (occur != 0)
                    occur = 1;
            } else
                occur++;
        } else if (cur->type == XML_COMMENT_NODE) {
            sep = "/";
	    name = "comment()";
            next = cur->parent;

            /*
             * Thumbler index computation
             */
            tmp = cur->prev;
            while (tmp != NULL) {
                if (tmp->type == XML_COMMENT_NODE)
		    occur++;
                tmp = tmp->prev;
            }
            if (occur == 0) {
                tmp = cur->next;
                while (tmp != NULL && occur == 0) {
		  if (tmp->type == XML_COMMENT_NODE)
		    occur++;
                    tmp = tmp->next;
                }
                if (occur != 0)
                    occur = 1;
            } else
                occur++;
        } else if ((cur->type == XML_TEXT_NODE) ||
                   (cur->type == XML_CDATA_SECTION_NODE)) {
            sep = "/";
	    name = "text()";
            next = cur->parent;

            /*
             * Thumbler index computation
             */
            tmp = cur->prev;
            while (tmp != NULL) {
                if ((cur->type == XML_TEXT_NODE) ||
		    (cur->type == XML_CDATA_SECTION_NODE))
		    occur++;
                tmp = tmp->prev;
            }
            if (occur == 0) {
                tmp = cur->next;
                while (tmp != NULL && occur == 0) {
		  if ((cur->type == XML_TEXT_NODE) ||
		      (cur->type == XML_CDATA_SECTION_NODE))
		    occur++;
                    tmp = tmp->next;
                }
                if (occur != 0)
                    occur = 1;
            } else
                occur++;
        } else if (cur->type == XML_PI_NODE) {
            sep = "/";
	    snprintf(nametemp, sizeof(nametemp) - 1,
		     "processing-instruction('%s')", cur->name);
            nametemp[sizeof(nametemp) - 1] = 0;
            name = nametemp;

	    next = cur->parent;

            /*
             * Thumbler index computation
             */
            tmp = cur->prev;
            while (tmp != NULL) {
                if ((tmp->type == XML_PI_NODE) &&
		    (xmlStrEqual(cur->name, tmp->name)))
                    occur++;
                tmp = tmp->prev;
            }
            if (occur == 0) {
                tmp = cur->next;
                while (tmp != NULL && occur == 0) {
                    if ((tmp->type == XML_PI_NODE) &&
			(xmlStrEqual(cur->name, tmp->name)))
                        occur++;
                    tmp = tmp->next;
                }
                if (occur != 0)
                    occur = 1;
            } else
                occur++;

        } else if (cur->type == XML_ATTRIBUTE_NODE) {
            sep = "/@";
            name = (const char *) (((xmlAttrPtr) cur)->name);
            next = ((xmlAttrPtr) cur)->parent;
        } else {
            next = cur->parent;
        }

        /*
         * Make sure there is enough room
         */
        if (xmlStrlen(buffer) + sizeof(nametemp) + 20 > buf_len) {
            buf_len =
                2 * buf_len + xmlStrlen(buffer) + sizeof(nametemp) + 20;
            temp = (xmlChar *) xmlRealloc(buffer, buf_len);
            if (temp == NULL) {
                xmlFree(buf);
                xmlFree(buffer);
                return (NULL);
            }
            buffer = temp;
            temp = (xmlChar *) xmlRealloc(buf, buf_len);
            if (temp == NULL) {
                xmlFree(buf);
                xmlFree(buffer);
                return (NULL);
            }
            buf = temp;
        }
        if (occur == 0)
            snprintf((char *) buf, buf_len, "%s%s%s",
                     sep, name, (char *) buffer);
        else
            snprintf((char *) buf, buf_len, "%s%s[%d]%s",
                     sep, name, occur, (char *) buffer);
        snprintf((char *) buffer, buf_len, "%s", buf);
        cur = next;
    } while (cur != NULL);
    xmlFree(buf);
    return (buffer);
}

/**
 * xmlDocGetRootElement:
 * @doc:  the document
 *
 * Get the root element of the document (doc->children is a list
 * containing possibly comments, PIs, etc ...).
 *
 * Returns the #xmlNodePtr for the root or NULL
 */
xmlNodePtr
xmlDocGetRootElement(xmlDocPtr doc) {
    xmlNodePtr ret;

    if (doc == NULL) return(NULL);
    ret = doc->children;
    while (ret != NULL) {
	if (ret->type == XML_ELEMENT_NODE)
	    return(ret);
        ret = ret->next;
    }
    return(ret);
}
 
/**
 * xmlDocSetRootElement:
 * @doc:  the document
 * @root:  the new document root element
 *
 * Set the root element of the document (doc->children is a list
 * containing possibly comments, PIs, etc ...).
 *
 * Returns the old root element if any was found
 */
xmlNodePtr
xmlDocSetRootElement(xmlDocPtr doc, xmlNodePtr root) {
    xmlNodePtr old = NULL;

    if (doc == NULL) return(NULL);
    if (root == NULL)
	return(NULL);
    xmlUnlinkNode(root);
    root->doc = doc;
    root->parent = (xmlNodePtr) doc;
    old = doc->children;
    while (old != NULL) {
	if (old->type == XML_ELEMENT_NODE)
	    break;
        old = old->next;
    }
    if (old == NULL) {
	if (doc->children == NULL) {
	    doc->children = root;
	    doc->last = root;
	} else {
	    xmlAddSibling(doc->children, root);
	}
    } else {
	xmlReplaceNode(old, root);
    }
    return(old);
}
 
/**
 * xmlNodeSetLang:
 * @cur:  the node being changed
 * @lang:  the language description
 *
 * Set the language of a node, i.e. the values of the xml:lang
 * attribute.
 */
void
xmlNodeSetLang(xmlNodePtr cur, const xmlChar *lang) {
    xmlNsPtr ns;

    if (cur == NULL) return;
    switch(cur->type) {
        case XML_TEXT_NODE:
        case XML_CDATA_SECTION_NODE:
        case XML_COMMENT_NODE:
        case XML_DOCUMENT_NODE:
        case XML_DOCUMENT_TYPE_NODE:
        case XML_DOCUMENT_FRAG_NODE:
        case XML_NOTATION_NODE:
        case XML_HTML_DOCUMENT_NODE:
        case XML_DTD_NODE:
        case XML_ELEMENT_DECL:
        case XML_ATTRIBUTE_DECL:
        case XML_ENTITY_DECL:
        case XML_PI_NODE:
        case XML_ENTITY_REF_NODE:
        case XML_ENTITY_NODE:
	case XML_NAMESPACE_DECL:
#ifdef LIBXML_DOCB_ENABLED
	case XML_DOCB_DOCUMENT_NODE:
#endif
	case XML_XINCLUDE_START:
	case XML_XINCLUDE_END:
	    return;
        case XML_ELEMENT_NODE:
        case XML_ATTRIBUTE_NODE:
	    break;
    }
    ns = xmlSearchNsByHref(cur->doc, cur, XML_XML_NAMESPACE);
    if (ns == NULL)
	return;
    xmlSetNsProp(cur, ns, BAD_CAST "lang", lang);
}
 
/**
 * xmlNodeGetLang:
 * @cur:  the node being checked
 *
 * Searches the language of a node, i.e. the values of the xml:lang
 * attribute or the one carried by the nearest ancestor.
 *
 * Returns a pointer to the lang value, or NULL if not found
 *     It's up to the caller to free the memory with xmlFree().
 */
xmlChar *
xmlNodeGetLang(xmlNodePtr cur) {
    xmlChar *lang;

    while (cur != NULL) {
        lang = xmlGetNsProp(cur, BAD_CAST "lang", XML_XML_NAMESPACE);
	if (lang != NULL)
	    return(lang);
	cur = cur->parent;
    }
    return(NULL);
}
 

/**
 * xmlNodeSetSpacePreserve:
 * @cur:  the node being changed
 * @val:  the xml:space value ("0": default, 1: "preserve")
 *
 * Set (or reset) the space preserving behaviour of a node, i.e. the
 * value of the xml:space attribute.
 */
void
xmlNodeSetSpacePreserve(xmlNodePtr cur, int val) {
    xmlNsPtr ns;

    if (cur == NULL) return;
    switch(cur->type) {
        case XML_TEXT_NODE:
        case XML_CDATA_SECTION_NODE:
        case XML_COMMENT_NODE:
        case XML_DOCUMENT_NODE:
        case XML_DOCUMENT_TYPE_NODE:
        case XML_DOCUMENT_FRAG_NODE:
        case XML_NOTATION_NODE:
        case XML_HTML_DOCUMENT_NODE:
        case XML_DTD_NODE:
        case XML_ELEMENT_DECL:
        case XML_ATTRIBUTE_DECL:
        case XML_ENTITY_DECL:
        case XML_PI_NODE:
        case XML_ENTITY_REF_NODE:
        case XML_ENTITY_NODE:
	case XML_NAMESPACE_DECL:
	case XML_XINCLUDE_START:
	case XML_XINCLUDE_END:
#ifdef LIBXML_DOCB_ENABLED
	case XML_DOCB_DOCUMENT_NODE:
#endif
	    return;
        case XML_ELEMENT_NODE:
        case XML_ATTRIBUTE_NODE:
	    break;
    }
    ns = xmlSearchNsByHref(cur->doc, cur, XML_XML_NAMESPACE);
    if (ns == NULL)
	return;
    switch (val) {
    case 0:
	xmlSetNsProp(cur, ns, BAD_CAST "space", BAD_CAST "default");
	break;
    case 1:
	xmlSetNsProp(cur, ns, BAD_CAST "space", BAD_CAST "preserve");
	break;
    }
}

/**
 * xmlNodeGetSpacePreserve:
 * @cur:  the node being checked
 *
 * Searches the space preserving behaviour of a node, i.e. the values
 * of the xml:space attribute or the one carried by the nearest
 * ancestor.
 *
 * Returns -1 if xml:space is not inherited, 0 if "default", 1 if "preserve"
 */
int
xmlNodeGetSpacePreserve(xmlNodePtr cur) {
    xmlChar *space;

    while (cur != NULL) {
	space = xmlGetNsProp(cur, BAD_CAST "space", XML_XML_NAMESPACE);
	if (space != NULL) {
	    if (xmlStrEqual(space, BAD_CAST "preserve")) {
		xmlFree(space);
		return(1);
	    }
	    if (xmlStrEqual(space, BAD_CAST "default")) {
		xmlFree(space);
		return(0);
	    }
	    xmlFree(space);
	}
	cur = cur->parent;
    }
    return(-1);
}
 
/**
 * xmlNodeSetName:
 * @cur:  the node being changed
 * @name:  the new tag name
 *
 * Set (or reset) the name of a node.
 */
void
xmlNodeSetName(xmlNodePtr cur, const xmlChar *name) {
    if (cur == NULL) return;
    if (name == NULL) return;
    switch(cur->type) {
        case XML_TEXT_NODE:
        case XML_CDATA_SECTION_NODE:
        case XML_COMMENT_NODE:
        case XML_DOCUMENT_TYPE_NODE:
        case XML_DOCUMENT_FRAG_NODE:
        case XML_NOTATION_NODE:
        case XML_HTML_DOCUMENT_NODE:
	case XML_NAMESPACE_DECL:
	case XML_XINCLUDE_START:
	case XML_XINCLUDE_END:
#ifdef LIBXML_DOCB_ENABLED
	case XML_DOCB_DOCUMENT_NODE:
#endif
	    return;
        case XML_ELEMENT_NODE:
        case XML_ATTRIBUTE_NODE:
        case XML_PI_NODE:
        case XML_ENTITY_REF_NODE:
        case XML_ENTITY_NODE:
        case XML_DTD_NODE:
        case XML_DOCUMENT_NODE:
        case XML_ELEMENT_DECL:
        case XML_ATTRIBUTE_DECL:
        case XML_ENTITY_DECL:
	    break;
    }
    if (cur->name != NULL) xmlFree((xmlChar *) cur->name);
    cur->name = xmlStrdup(name);
}
 
/**
 * xmlNodeSetBase:
 * @cur:  the node being changed
 * @uri:  the new base URI
 *
 * Set (or reset) the base URI of a node, i.e. the value of the
 * xml:base attribute.
 */
void
xmlNodeSetBase(xmlNodePtr cur, xmlChar* uri) {
    xmlNsPtr ns;

    if (cur == NULL) return;
    switch(cur->type) {
        case XML_TEXT_NODE:
        case XML_CDATA_SECTION_NODE:
        case XML_COMMENT_NODE:
        case XML_DOCUMENT_TYPE_NODE:
        case XML_DOCUMENT_FRAG_NODE:
        case XML_NOTATION_NODE:
        case XML_DTD_NODE:
        case XML_ELEMENT_DECL:
        case XML_ATTRIBUTE_DECL:
        case XML_ENTITY_DECL:
        case XML_PI_NODE:
        case XML_ENTITY_REF_NODE:
        case XML_ENTITY_NODE:
	case XML_NAMESPACE_DECL:
	case XML_XINCLUDE_START:
	case XML_XINCLUDE_END:
	    return;
        case XML_ELEMENT_NODE:
        case XML_ATTRIBUTE_NODE:
	    break;
        case XML_DOCUMENT_NODE:
#ifdef LIBXML_DOCB_ENABLED
	case XML_DOCB_DOCUMENT_NODE:
#endif
        case XML_HTML_DOCUMENT_NODE: {
	    xmlDocPtr doc = (xmlDocPtr) cur;

	    if (doc->URL != NULL)
		xmlFree((xmlChar *) doc->URL);
	    if (uri == NULL)
		doc->URL = NULL;
	    else
		doc->URL = xmlStrdup(uri);
	    return;
	}
    }
    
    ns = xmlSearchNsByHref(cur->doc, cur, XML_XML_NAMESPACE);
    if (ns == NULL)
	return;
    xmlSetNsProp(cur, ns, BAD_CAST "base", uri);
}

/**
 * xmlNodeGetBase:
 * @doc:  the document the node pertains to
 * @cur:  the node being checked
 *
 * Searches for the BASE URL. The code should work on both XML
 * and HTML document even if base mechanisms are completely different.
 * It returns the base as defined in RFC 2396 sections
 * 5.1.1. Base URI within Document Content
 * and
 * 5.1.2. Base URI from the Encapsulating Entity
 * However it does not return the document base (5.1.3), use
 * xmlDocumentGetBase() for this
 *
 * Returns a pointer to the base URL, or NULL if not found
 *     It's up to the caller to free the memory with xmlFree().
 */
xmlChar *
xmlNodeGetBase(xmlDocPtr doc, xmlNodePtr cur) {
    xmlChar *oldbase = NULL;
    xmlChar *base, *newbase;

    if ((cur == NULL) && (doc == NULL)) 
        return(NULL);
    if (doc == NULL) doc = cur->doc;	
    if ((doc != NULL) && (doc->type == XML_HTML_DOCUMENT_NODE)) {
        cur = doc->children;
	while ((cur != NULL) && (cur->name != NULL)) {
	    if (cur->type != XML_ELEMENT_NODE) {
	        cur = cur->next;
		continue;
	    }
	    if (!xmlStrcasecmp(cur->name, BAD_CAST "html")) {
	        cur = cur->children;
		continue;
	    }
	    if (!xmlStrcasecmp(cur->name, BAD_CAST "head")) {
	        cur = cur->children;
		continue;
	    }
	    if (!xmlStrcasecmp(cur->name, BAD_CAST "base")) {
                return(xmlGetProp(cur, BAD_CAST "href"));
	    }
	    cur = cur->next;
	}
	return(NULL);
    }
    while (cur != NULL) {
	if (cur->type == XML_ENTITY_DECL) {
	    xmlEntityPtr ent = (xmlEntityPtr) cur;
	    return(xmlStrdup(ent->URI));
	}
	if (cur->type == XML_ELEMENT_NODE) {
	    base = xmlGetNsProp(cur, BAD_CAST "base", XML_XML_NAMESPACE);
	    if (base != NULL) {
		if (oldbase != NULL) {
		    newbase = xmlBuildURI(oldbase, base);
		    if (newbase != NULL) {
			xmlFree(oldbase);
			xmlFree(base);
			oldbase = newbase;
		    } else {
			xmlFree(oldbase);
			xmlFree(base);
			return(NULL);
		    }
		} else {
		    oldbase = base;
		}
		if ((!xmlStrncmp(oldbase, BAD_CAST "http://", 7)) ||
		    (!xmlStrncmp(oldbase, BAD_CAST "ftp://", 6)) ||
		    (!xmlStrncmp(oldbase, BAD_CAST "urn:", 4)))
		    return(oldbase);
	    }
	}
	cur = cur->parent;
    }
    if ((doc != NULL) && (doc->URL != NULL)) {
	if (oldbase == NULL)
	    return(xmlStrdup(doc->URL));
	newbase = xmlBuildURI(oldbase, doc->URL);
	xmlFree(oldbase);
	return(newbase);
    }
    return(oldbase);
}
 
/**
 * xmlNodeGetContent:
 * @cur:  the node being read
 *
 * Read the value of a node, this can be either the text carried
 * directly by this node if it's a TEXT node or the aggregate string
 * of the values carried by this node child's (TEXT and ENTITY_REF).
 * Entity references are substituted.
 * Returns a new #xmlChar * or NULL if no content is available.
 *     It's up to the caller to free the memory with xmlFree().
 */
xmlChar *
xmlNodeGetContent(xmlNodePtr cur)
{
    if (cur == NULL)
        return (NULL);
    switch (cur->type) {
        case XML_DOCUMENT_FRAG_NODE:
        case XML_ELEMENT_NODE:{
                xmlNodePtr tmp = cur;
                xmlBufferPtr buffer;
                xmlChar *ret;

                buffer = xmlBufferCreate();
                if (buffer == NULL)
                    return (NULL);
                while (tmp != NULL) {
                    switch (tmp->type) {
                        case XML_CDATA_SECTION_NODE:
                        case XML_TEXT_NODE:
                            if (tmp->content != NULL)
                                xmlBufferCat(buffer, tmp->content);
                            break;
                        case XML_ENTITY_REF_NODE:{
                                /* recursive substitution of entity references */
                                xmlChar *cont = xmlNodeGetContent(tmp);

                                if (cont) {
                                    xmlBufferCat(buffer,
                                                 (const xmlChar *) cont);
                                    xmlFree(cont);
                                }
                                break;
                            }
                        default:
                            break;
                    }
                    /*
                     * Skip to next node
                     */
                    if (tmp->children != NULL) {
                        if (tmp->children->type != XML_ENTITY_DECL) {
                            tmp = tmp->children;
                            continue;
                        }
                    }
                    if (tmp == cur)
                        break;

                    if (tmp->next != NULL) {
                        tmp = tmp->next;
                        continue;
                    }

                    do {
                        tmp = tmp->parent;
                        if (tmp == NULL)
                            break;
                        if (tmp == cur) {
                            tmp = NULL;
                            break;
                        }
                        if (tmp->next != NULL) {
                            tmp = tmp->next;
                            break;
                        }
                    } while (tmp != NULL);
                }
                ret = buffer->content;
                buffer->content = NULL;
                xmlBufferFree(buffer);
                return (ret);
            }
        case XML_ATTRIBUTE_NODE:{
                xmlAttrPtr attr = (xmlAttrPtr) cur;

                if (attr->parent != NULL)
                    return (xmlNodeListGetString
                            (attr->parent->doc, attr->children, 1));
                else
                    return (xmlNodeListGetString(NULL, attr->children, 1));
                break;
            }
        case XML_COMMENT_NODE:
        case XML_PI_NODE:
            if (cur->content != NULL)
                return (xmlStrdup(cur->content));
            return (NULL);
        case XML_ENTITY_REF_NODE:{
                xmlEntityPtr ent;
                xmlNodePtr tmp;
                xmlBufferPtr buffer;
                xmlChar *ret;

                /* lookup entity declaration */
                ent = xmlGetDocEntity(cur->doc, cur->name);
                if (ent == NULL)
                    return (NULL);

                buffer = xmlBufferCreate();
                if (buffer == NULL)
                    return (NULL);

                /* an entity content can be any "well balanced chunk",
                 * i.e. the result of the content [43] production:
                 * http://www.w3.org/TR/REC-xml#NT-content
                 * -> we iterate through child nodes and recursive call
                 * xmlNodeGetContent() which handles all possible node types */
                tmp = ent->children;
                while (tmp) {
                    xmlChar *cont = xmlNodeGetContent(tmp);

                    if (cont) {
                        xmlBufferCat(buffer, (const xmlChar *) cont);
                        xmlFree(cont);
                    }
                    tmp = tmp->next;
                }

                ret = buffer->content;
                buffer->content = NULL;
                xmlBufferFree(buffer);
                return (ret);
            }
        case XML_ENTITY_NODE:
        case XML_DOCUMENT_NODE:
        case XML_HTML_DOCUMENT_NODE:
        case XML_DOCUMENT_TYPE_NODE:
        case XML_NOTATION_NODE:
        case XML_DTD_NODE:
        case XML_XINCLUDE_START:
        case XML_XINCLUDE_END:
#ifdef LIBXML_DOCB_ENABLED
        case XML_DOCB_DOCUMENT_NODE:
#endif
            return (NULL);
        case XML_NAMESPACE_DECL: {
	    xmlChar *tmp;

	    tmp = xmlStrdup(((xmlNsPtr) cur)->href);
            return (tmp);
	}
        case XML_ELEMENT_DECL:
            /* TODO !!! */
            return (NULL);
        case XML_ATTRIBUTE_DECL:
            /* TODO !!! */
            return (NULL);
        case XML_ENTITY_DECL:
            /* TODO !!! */
            return (NULL);
        case XML_CDATA_SECTION_NODE:
        case XML_TEXT_NODE:
            if (cur->content != NULL)
                return (xmlStrdup(cur->content));
            return (NULL);
    }
    return (NULL);
}
/**
 * xmlNodeSetContent:
 * @cur:  the node being modified
 * @content:  the new value of the content
 *
 * Replace the content of a node.
 */
void
xmlNodeSetContent(xmlNodePtr cur, const xmlChar *content) {
    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlNodeSetContent : node == NULL\n");
#endif
	return;
    }
    switch (cur->type) {
        case XML_DOCUMENT_FRAG_NODE:
        case XML_ELEMENT_NODE:
        case XML_ATTRIBUTE_NODE:
	    if (cur->children != NULL) xmlFreeNodeList(cur->children);
	    cur->children = xmlStringGetNodeList(cur->doc, content);
	    UPDATE_LAST_CHILD_AND_PARENT(cur)
	    break;
        case XML_TEXT_NODE:
        case XML_CDATA_SECTION_NODE:
        case XML_ENTITY_REF_NODE:
        case XML_ENTITY_NODE:
        case XML_PI_NODE:
        case XML_COMMENT_NODE:
	    if (cur->content != NULL) {
		xmlFree(cur->content);
	    }	
	    if (cur->children != NULL) xmlFreeNodeList(cur->children);
	    cur->last = cur->children = NULL;
	    if (content != NULL) {
		cur->content = xmlStrdup(content);
	    } else 
		cur->content = NULL;
	    break;
        case XML_DOCUMENT_NODE:
        case XML_HTML_DOCUMENT_NODE:
        case XML_DOCUMENT_TYPE_NODE:
	case XML_XINCLUDE_START:
	case XML_XINCLUDE_END:
#ifdef LIBXML_DOCB_ENABLED
	case XML_DOCB_DOCUMENT_NODE:
#endif
	    break;
        case XML_NOTATION_NODE:
	    break;
        case XML_DTD_NODE:
	    break;
	case XML_NAMESPACE_DECL:
	    break;
        case XML_ELEMENT_DECL:
	    /* TODO !!! */
	    break;
        case XML_ATTRIBUTE_DECL:
	    /* TODO !!! */
	    break;
        case XML_ENTITY_DECL:
	    /* TODO !!! */
	    break;
    }
}

/**
 * xmlNodeSetContentLen:
 * @cur:  the node being modified
 * @content:  the new value of the content
 * @len:  the size of @content
 *
 * Replace the content of a node.
 */
void
xmlNodeSetContentLen(xmlNodePtr cur, const xmlChar *content, int len) {
    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlNodeSetContentLen : node == NULL\n");
#endif
	return;
    }
    switch (cur->type) {
        case XML_DOCUMENT_FRAG_NODE:
        case XML_ELEMENT_NODE:
        case XML_ATTRIBUTE_NODE:
	    if (cur->children != NULL) xmlFreeNodeList(cur->children);
	    cur->children = xmlStringLenGetNodeList(cur->doc, content, len);
	    UPDATE_LAST_CHILD_AND_PARENT(cur)
	    break;
        case XML_TEXT_NODE:
        case XML_CDATA_SECTION_NODE:
        case XML_ENTITY_REF_NODE:
        case XML_ENTITY_NODE:
        case XML_PI_NODE:
        case XML_COMMENT_NODE:
        case XML_NOTATION_NODE:
	    if (cur->content != NULL) {
		xmlFree(cur->content);
	    }	
	    if (cur->children != NULL) xmlFreeNodeList(cur->children);
	    cur->children = cur->last = NULL;
	    if (content != NULL) {
		cur->content = xmlStrndup(content, len);
	    } else 
		cur->content = NULL;
	    break;
        case XML_DOCUMENT_NODE:
        case XML_DTD_NODE:
        case XML_HTML_DOCUMENT_NODE:
        case XML_DOCUMENT_TYPE_NODE:
	case XML_NAMESPACE_DECL:
	case XML_XINCLUDE_START:
	case XML_XINCLUDE_END:
#ifdef LIBXML_DOCB_ENABLED
	case XML_DOCB_DOCUMENT_NODE:
#endif
	    break;
        case XML_ELEMENT_DECL:
	    /* TODO !!! */
	    break;
        case XML_ATTRIBUTE_DECL:
	    /* TODO !!! */
	    break;
        case XML_ENTITY_DECL:
	    /* TODO !!! */
	    break;
    }
}

/**
 * xmlNodeAddContentLen:
 * @cur:  the node being modified
 * @content:  extra content
 * @len:  the size of @content
 * 
 * Append the extra substring to the node content.
 */
void
xmlNodeAddContentLen(xmlNodePtr cur, const xmlChar *content, int len) {
    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlNodeAddContentLen : node == NULL\n");
#endif
	return;
    }
    if (len <= 0) return;
    switch (cur->type) {
        case XML_DOCUMENT_FRAG_NODE:
        case XML_ELEMENT_NODE: {
	    xmlNodePtr last, newNode, tmp;

	    last = cur->last;
	    newNode = xmlNewTextLen(content, len);
	    if (newNode != NULL) {
		tmp = xmlAddChild(cur, newNode);
		if (tmp != newNode)
		    return;
	        if ((last != NULL) && (last->next == newNode)) {
		    xmlTextMerge(last, newNode);
		}
	    }
	    break;
	}
        case XML_ATTRIBUTE_NODE:
	    break;
        case XML_TEXT_NODE:
        case XML_CDATA_SECTION_NODE:
        case XML_ENTITY_REF_NODE:
        case XML_ENTITY_NODE:
        case XML_PI_NODE:
        case XML_COMMENT_NODE:
        case XML_NOTATION_NODE:
	    if (content != NULL) {
		cur->content = xmlStrncat(cur->content, content, len);
            }
        case XML_DOCUMENT_NODE:
        case XML_DTD_NODE:
        case XML_HTML_DOCUMENT_NODE:
        case XML_DOCUMENT_TYPE_NODE:
	case XML_NAMESPACE_DECL:
	case XML_XINCLUDE_START:
	case XML_XINCLUDE_END:
#ifdef LIBXML_DOCB_ENABLED
	case XML_DOCB_DOCUMENT_NODE:
#endif
	    break;
        case XML_ELEMENT_DECL:
        case XML_ATTRIBUTE_DECL:
        case XML_ENTITY_DECL:
	    break;
    }
}

/**
 * xmlNodeAddContent:
 * @cur:  the node being modified
 * @content:  extra content
 * 
 * Append the extra substring to the node content.
 */
void
xmlNodeAddContent(xmlNodePtr cur, const xmlChar *content) {
    int len;

    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlNodeAddContent : node == NULL\n");
#endif
	return;
    }
    if (content == NULL) return;
    len = xmlStrlen(content);
    xmlNodeAddContentLen(cur, content, len);
}

/**
 * xmlTextMerge:
 * @first:  the first text node
 * @second:  the second text node being merged
 * 
 * Merge two text nodes into one
 * Returns the first text node augmented
 */
xmlNodePtr
xmlTextMerge(xmlNodePtr first, xmlNodePtr second) {
    if (first == NULL) return(second);
    if (second == NULL) return(first);
    if (first->type != XML_TEXT_NODE) return(first);
    if (second->type != XML_TEXT_NODE) return(first);
    if (second->name != first->name)
	return(first);
    xmlNodeAddContent(first, second->content);
    xmlUnlinkNode(second);
    xmlFreeNode(second);
    return(first);
}

/**
 * xmlGetNsList:
 * @doc:  the document
 * @node:  the current node
 *
 * Search all the namespace applying to a given element.
 * Returns an NULL terminated array of all the #xmlNsPtr found
 *         that need to be freed by the caller or NULL if no
 *         namespace if defined
 */
xmlNsPtr *
xmlGetNsList(xmlDocPtr doc ATTRIBUTE_UNUSED, xmlNodePtr node)
{
    xmlNsPtr cur;
    xmlNsPtr *ret = NULL;
    int nbns = 0;
    int maxns = 10;
    int i;

    while (node != NULL) {
        if (node->type == XML_ELEMENT_NODE) {
            cur = node->nsDef;
            while (cur != NULL) {
                if (ret == NULL) {
                    ret =
                        (xmlNsPtr *) xmlMalloc((maxns + 1) *
                                               sizeof(xmlNsPtr));
                    if (ret == NULL) {
                        xmlGenericError(xmlGenericErrorContext,
                                        "xmlGetNsList : out of memory!\n");
                        return (NULL);
                    }
                    ret[nbns] = NULL;
                }
                for (i = 0; i < nbns; i++) {
                    if ((cur->prefix == ret[i]->prefix) ||
                        (xmlStrEqual(cur->prefix, ret[i]->prefix)))
                        break;
                }
                if (i >= nbns) {
                    if (nbns >= maxns) {
                        maxns *= 2;
                        ret = (xmlNsPtr *) xmlRealloc(ret,
                                                      (maxns +
                                                       1) *
                                                      sizeof(xmlNsPtr));
                        if (ret == NULL) {
                            xmlGenericError(xmlGenericErrorContext,
                                            "xmlGetNsList : realloc failed!\n");
                            return (NULL);
                        }
                    }
                    ret[nbns++] = cur;
                    ret[nbns] = NULL;
                }

                cur = cur->next;
            }
        }
        node = node->parent;
    }
    return (ret);
}

/**
 * xmlSearchNs:
 * @doc:  the document
 * @node:  the current node
 * @nameSpace:  the namespace prefix
 *
 * Search a Ns registered under a given name space for a document.
 * recurse on the parents until it finds the defined namespace
 * or return NULL otherwise.
 * @nameSpace can be NULL, this is a search for the default namespace.
 * We don't allow to cross entities boundaries. If you don't declare
 * the namespace within those you will be in troubles !!! A warning
 * is generated to cover this case.
 *
 * Returns the namespace pointer or NULL.
 */
xmlNsPtr
xmlSearchNs(xmlDocPtr doc, xmlNodePtr node, const xmlChar *nameSpace) {
    xmlNsPtr cur;

    if (node == NULL) return(NULL);
    if ((nameSpace != NULL) &&
	(xmlStrEqual(nameSpace, (const xmlChar *)"xml"))) {
	if ((doc == NULL) && (node->type == XML_ELEMENT_NODE)) {
	    /*
	     * The XML-1.0 namespace is normally held on the root
	     * element. In this case exceptionally create it on the
	     * node element.
	     */
	    cur = (xmlNsPtr) xmlMalloc(sizeof(xmlNs));
	    if (cur == NULL) {
		xmlGenericError(xmlGenericErrorContext,
			"xmlSearchNs : malloc failed\n");
		return(NULL);
	    }
	    memset(cur, 0, sizeof(xmlNs));
	    cur->type = XML_LOCAL_NAMESPACE;
	    cur->href = xmlStrdup(XML_XML_NAMESPACE); 
	    cur->prefix = xmlStrdup((const xmlChar *)"xml"); 
	    cur->next = node->nsDef;
	    node->nsDef = cur;
	    return(cur);
	}
	if (doc->oldNs == NULL) {
	    /*
	     * Allocate a new Namespace and fill the fields.
	     */
	    doc->oldNs = (xmlNsPtr) xmlMalloc(sizeof(xmlNs));
	    if (doc->oldNs == NULL) {
		xmlGenericError(xmlGenericErrorContext,
			"xmlSearchNs : malloc failed\n");
		return(NULL);
	    }
	    memset(doc->oldNs, 0, sizeof(xmlNs));
	    doc->oldNs->type = XML_LOCAL_NAMESPACE;

	    doc->oldNs->href = xmlStrdup(XML_XML_NAMESPACE); 
	    doc->oldNs->prefix = xmlStrdup((const xmlChar *)"xml"); 
	}
	return(doc->oldNs);
    }
    while (node != NULL) {
	if ((node->type == XML_ENTITY_REF_NODE) ||
	    (node->type == XML_ENTITY_NODE) ||
	    (node->type == XML_ENTITY_DECL))
	    return(NULL);
	if (node->type == XML_ELEMENT_NODE) {
	    cur = node->nsDef;
	    while (cur != NULL) {
		if ((cur->prefix == NULL) && (nameSpace == NULL) &&
		    (cur->href != NULL))
		    return(cur);
		if ((cur->prefix != NULL) && (nameSpace != NULL) &&
		    (cur->href != NULL) &&
		    (xmlStrEqual(cur->prefix, nameSpace)))
		    return(cur);
		cur = cur->next;
	    }
	}
	node = node->parent;
    }
    return(NULL);
}

/**
 * xmlSearchNsByHref:
 * @doc:  the document
 * @node:  the current node
 * @href:  the namespace value
 *
 * Search a Ns aliasing a given URI. Recurse on the parents until it finds
 * the defined namespace or return NULL otherwise.
 * Returns the namespace pointer or NULL.
 */
xmlNsPtr
xmlSearchNsByHref(xmlDocPtr doc, xmlNodePtr node, const xmlChar *href) {
    xmlNsPtr cur;
    xmlNodePtr orig = node;

    if ((node == NULL) || (href == NULL)) return(NULL);
    if (xmlStrEqual(href, XML_XML_NAMESPACE)) {
	/*
	 * Only the document can hold the XML spec namespace.
	 */
	if ((doc == NULL) && (node->type == XML_ELEMENT_NODE)) {
	    /*
	     * The XML-1.0 namespace is normally held on the root
	     * element. In this case exceptionally create it on the
	     * node element.
	     */
	    cur = (xmlNsPtr) xmlMalloc(sizeof(xmlNs));
	    if (cur == NULL) {
		xmlGenericError(xmlGenericErrorContext,
			"xmlSearchNs : malloc failed\n");
		return(NULL);
	    }
	    memset(cur, 0, sizeof(xmlNs));
	    cur->type = XML_LOCAL_NAMESPACE;
	    cur->href = xmlStrdup(XML_XML_NAMESPACE); 
	    cur->prefix = xmlStrdup((const xmlChar *)"xml"); 
	    cur->next = node->nsDef;
	    node->nsDef = cur;
	    return(cur);
	}
	if (doc->oldNs == NULL) {
	    /*
	     * Allocate a new Namespace and fill the fields.
	     */
	    doc->oldNs = (xmlNsPtr) xmlMalloc(sizeof(xmlNs));
	    if (doc->oldNs == NULL) {
		xmlGenericError(xmlGenericErrorContext,
			"xmlSearchNsByHref : malloc failed\n");
		return(NULL);
	    }
	    memset(doc->oldNs, 0, sizeof(xmlNs));
	    doc->oldNs->type = XML_LOCAL_NAMESPACE;

	    doc->oldNs->href = xmlStrdup(XML_XML_NAMESPACE); 
	    doc->oldNs->prefix = xmlStrdup((const xmlChar *)"xml"); 
	}
	return(doc->oldNs);
    }
    while (node != NULL) {
	cur = node->nsDef;
	while (cur != NULL) {
	    if ((cur->href != NULL) && (href != NULL) &&
	        (xmlStrEqual(cur->href, href))) {
		/*
		 * Check that the prefix is not shadowed between orig and node
		 */
		xmlNodePtr check = orig;
		xmlNsPtr tst;

		while (check != node) {
		    tst = check->nsDef;
		    while (tst != NULL) {
			if ((tst->prefix == NULL) && (cur->prefix == NULL))
	                    goto shadowed;
			if ((tst->prefix != NULL) && (cur->prefix != NULL) &&
			    (xmlStrEqual(tst->prefix, cur->prefix)))
	                    goto shadowed;
		        tst = tst->next;
		    }
		    check = check->parent;
		}
		return(cur);
	    }
shadowed:		    
	    cur = cur->next;
	}
	node = node->parent;
    }
    return(NULL);
}

/**
 * xmlNewReconciliedNs:
 * @doc:  the document
 * @tree:  a node expected to hold the new namespace
 * @ns:  the original namespace
 *
 * This function tries to locate a namespace definition in a tree
 * ancestors, or create a new namespace definition node similar to
 * @ns trying to reuse the same prefix. However if the given prefix is
 * null (default namespace) or reused within the subtree defined by
 * @tree or on one of its ancestors then a new prefix is generated.
 * Returns the (new) namespace definition or NULL in case of error
 */
xmlNsPtr
xmlNewReconciliedNs(xmlDocPtr doc, xmlNodePtr tree, xmlNsPtr ns) {
    xmlNsPtr def;
    xmlChar prefix[50];
    int counter = 1;

    if (tree == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewReconciliedNs : tree == NULL\n");
#endif
	return(NULL);
    }
    if (ns == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewReconciliedNs : ns == NULL\n");
#endif
	return(NULL);
    }
    /*
     * Search an existing namespace definition inherited.
     */
    def = xmlSearchNsByHref(doc, tree, ns->href);
    if (def != NULL)
        return(def);

    /*
     * Find a close prefix which is not already in use.
     * Let's strip namespace prefixes longer than 20 chars !
     */
    if (ns->prefix == NULL)
	snprintf((char *) prefix, sizeof(prefix), "default");
    else
	snprintf((char *) prefix, sizeof(prefix), "%.20s", ns->prefix);

    def = xmlSearchNs(doc, tree, prefix);
    while (def != NULL) {
        if (counter > 1000) return(NULL);
	if (ns->prefix == NULL)
	    snprintf((char *) prefix, sizeof(prefix), "default%d", counter++);
	else
	    snprintf((char *) prefix, sizeof(prefix), "%.20s%d", ns->prefix, counter++);
	def = xmlSearchNs(doc, tree, prefix);
    }

    /*
     * OK, now we are ready to create a new one.
     */
    def = xmlNewNs(tree, ns->href, prefix);
    return(def);
}

/**
 * xmlReconciliateNs:
 * @doc:  the document
 * @tree:  a node defining the subtree to reconciliate
 *
 * This function checks that all the namespaces declared within the given
 * tree are properly declared. This is needed for example after Copy or Cut
 * and then paste operations. The subtree may still hold pointers to
 * namespace declarations outside the subtree or invalid/masked. As much
 * as possible the function try to reuse the existing namespaces found in
 * the new environment. If not possible the new namespaces are redeclared
 * on @tree at the top of the given subtree.
 * Returns the number of namespace declarations created or -1 in case of error.
 */
int
xmlReconciliateNs(xmlDocPtr doc, xmlNodePtr tree) {
    xmlNsPtr *oldNs = NULL;
    xmlNsPtr *newNs = NULL;
    int sizeCache = 0;
    int nbCache = 0;

    xmlNsPtr n;
    xmlNodePtr node = tree;
    xmlAttrPtr attr;
    int ret = 0, i;

    while (node != NULL) {
        /*
	 * Reconciliate the node namespace
	 */
	if (node->ns != NULL) {
	    /*
	     * initialize the cache if needed
	     */
	    if (sizeCache == 0) {
		sizeCache = 10;
		oldNs = (xmlNsPtr *) xmlMalloc(sizeCache *
					       sizeof(xmlNsPtr));
		if (oldNs == NULL) {
		    xmlGenericError(xmlGenericErrorContext,
			    "xmlReconciliateNs : memory pbm\n");
		    return(-1);
		}
		newNs = (xmlNsPtr *) xmlMalloc(sizeCache *
					       sizeof(xmlNsPtr));
		if (newNs == NULL) {
		    xmlGenericError(xmlGenericErrorContext,
			    "xmlReconciliateNs : memory pbm\n");
		    xmlFree(oldNs);
		    return(-1);
		}
	    }
	    for (i = 0;i < nbCache;i++) {
	        if (oldNs[i] == node->ns) {
		    node->ns = newNs[i];
		    break;
		}
	    }
	    if (i == nbCache) {
	        /*
		 * OK we need to recreate a new namespace definition
		 */
		n = xmlNewReconciliedNs(doc, tree, node->ns);
		if (n != NULL) { /* :-( what if else ??? */
		    /*
		     * check if we need to grow the cache buffers.
		     */
		    if (sizeCache <= nbCache) {
		        sizeCache *= 2;
			oldNs = (xmlNsPtr *) xmlRealloc(oldNs, sizeCache *
			                               sizeof(xmlNsPtr));
		        if (oldNs == NULL) {
			    xmlGenericError(xmlGenericErrorContext,
				    "xmlReconciliateNs : memory pbm\n");
			    xmlFree(newNs);
			    return(-1);
			}
			newNs = (xmlNsPtr *) xmlRealloc(newNs, sizeCache *
			                               sizeof(xmlNsPtr));
		        if (newNs == NULL) {
			    xmlGenericError(xmlGenericErrorContext,
				    "xmlReconciliateNs : memory pbm\n");
			    xmlFree(oldNs);
			    return(-1);
			}
		    }
		    newNs[nbCache] = n;
		    oldNs[nbCache++] = node->ns;
		    node->ns = n;
                }
	    }
	}
	/*
	 * now check for namespace hold by attributes on the node.
	 */
	attr = node->properties;
	while (attr != NULL) {
	    if (attr->ns != NULL) {
		/*
		 * initialize the cache if needed
		 */
		if (sizeCache == 0) {
		    sizeCache = 10;
		    oldNs = (xmlNsPtr *) xmlMalloc(sizeCache *
						   sizeof(xmlNsPtr));
		    if (oldNs == NULL) {
			xmlGenericError(xmlGenericErrorContext,
				"xmlReconciliateNs : memory pbm\n");
			return(-1);
		    }
		    newNs = (xmlNsPtr *) xmlMalloc(sizeCache *
						   sizeof(xmlNsPtr));
		    if (newNs == NULL) {
			xmlGenericError(xmlGenericErrorContext,
				"xmlReconciliateNs : memory pbm\n");
			xmlFree(oldNs);
			return(-1);
		    }
		}
		for (i = 0;i < nbCache;i++) {
		    if (oldNs[i] == attr->ns) {
			attr->ns = newNs[i];
			break;
		    }
		}
		if (i == nbCache) {
		    /*
		     * OK we need to recreate a new namespace definition
		     */
		    n = xmlNewReconciliedNs(doc, tree, attr->ns);
		    if (n != NULL) { /* :-( what if else ??? */
			/*
			 * check if we need to grow the cache buffers.
			 */
			if (sizeCache <= nbCache) {
			    sizeCache *= 2;
			    oldNs = (xmlNsPtr *) xmlRealloc(oldNs, sizeCache *
							   sizeof(xmlNsPtr));
			    if (oldNs == NULL) {
				xmlGenericError(xmlGenericErrorContext,
				        "xmlReconciliateNs : memory pbm\n");
				xmlFree(newNs);
				return(-1);
			    }
			    newNs = (xmlNsPtr *) xmlRealloc(newNs, sizeCache *
							   sizeof(xmlNsPtr));
			    if (newNs == NULL) {
				xmlGenericError(xmlGenericErrorContext,
				        "xmlReconciliateNs : memory pbm\n");
				xmlFree(oldNs);
				return(-1);
			    }
			}
			newNs[nbCache] = n;
			oldNs[nbCache++] = attr->ns;
			attr->ns = n;
		    }
		}
	    }
	    attr = attr->next;
	}

	/*
	 * Browse the full subtree, deep first
	 */
        if (node->children != NULL) {
	    /* deep first */
	    node = node->children;
	} else if ((node != tree) && (node->next != NULL)) {
	    /* then siblings */
	    node = node->next;
	} else if (node != tree) {
	    /* go up to parents->next if needed */
	    while (node != tree) {
	        if (node->parent != NULL)
		    node = node->parent;
		if ((node != tree) && (node->next != NULL)) {
		    node = node->next;
		    break;
		}
		if (node->parent == NULL) {
		    node = NULL;
		    break;
		}
	    }
	    /* exit condition */
	    if (node == tree) 
	        node = NULL;
	} else
	    break;
    }
    if (oldNs != NULL)
	xmlFree(oldNs);
    if (newNs != NULL)
	xmlFree(newNs);
    return(ret);
}

/**
 * xmlHasProp:
 * @node:  the node
 * @name:  the attribute name
 *
 * Search an attribute associated to a node
 * This function also looks in DTD attribute declaration for #FIXED or
 * default declaration values unless DTD use has been turned off.
 *
 * Returns the attribute or the attribute declaration or NULL if 
 *         neither was found.
 */
xmlAttrPtr
xmlHasProp(xmlNodePtr node, const xmlChar *name) {
    xmlAttrPtr prop;
    xmlDocPtr doc;

    if ((node == NULL) || (name == NULL)) return(NULL);
    /*
     * Check on the properties attached to the node
     */
    prop = node->properties;
    while (prop != NULL) {
        if (xmlStrEqual(prop->name, name))  {
	    return(prop);
        }
	prop = prop->next;
    }
    if (!xmlCheckDTD) return(NULL);

    /*
     * Check if there is a default declaration in the internal
     * or external subsets
     */
    doc =  node->doc;
    if (doc != NULL) {
        xmlAttributePtr attrDecl;
        if (doc->intSubset != NULL) {
	    attrDecl = xmlGetDtdAttrDesc(doc->intSubset, node->name, name);
	    if ((attrDecl == NULL) && (doc->extSubset != NULL))
		attrDecl = xmlGetDtdAttrDesc(doc->extSubset, node->name, name);
	    if (attrDecl != NULL)
		return((xmlAttrPtr) attrDecl);
	}
    }
    return(NULL);
}

/**
 * xmlHasNsProp:
 * @node:  the node
 * @name:  the attribute name
 * @nameSpace:  the URI of the namespace
 *
 * Search for an attribute associated to a node
 * This attribute has to be anchored in the namespace specified.
 * This does the entity substitution.
 * This function looks in DTD attribute declaration for #FIXED or
 * default declaration values unless DTD use has been turned off.
 *
 * Returns the attribute or the attribute declaration or NULL
 *     if neither was found.
 */
xmlAttrPtr
xmlHasNsProp(xmlNodePtr node, const xmlChar *name, const xmlChar *nameSpace) {
    xmlAttrPtr prop;
    xmlDocPtr doc;

    if (node == NULL)
	return(NULL);

    prop = node->properties;
    if (nameSpace == NULL)
	return(xmlHasProp(node, name));
    while (prop != NULL) {
	/*
	 * One need to have
	 *   - same attribute names
	 *   - and the attribute carrying that namespace
	 */
        if ((xmlStrEqual(prop->name, name)) &&
	    ((prop->ns != NULL) && (xmlStrEqual(prop->ns->href, nameSpace)))) {
	    return(prop);
        }
	prop = prop->next;
    }
    if (!xmlCheckDTD) return(NULL);

    /*
     * Check if there is a default declaration in the internal
     * or external subsets
     */
    doc =  node->doc;
    if (doc != NULL) {
        if (doc->intSubset != NULL) {
	    xmlAttributePtr attrDecl = NULL;
	    xmlNsPtr *nsList, *cur;
	    xmlChar *ename;

	    nsList = xmlGetNsList(node->doc, node);
	    if (nsList == NULL)
		return(NULL);
	    if ((node->ns != NULL) && (node->ns->prefix != NULL)) {
		ename = xmlStrdup(node->ns->prefix);
		ename = xmlStrcat(ename, BAD_CAST ":");
		ename = xmlStrcat(ename, node->name);
	    } else {
		ename = xmlStrdup(node->name);
	    }
	    if (ename == NULL) {
		xmlFree(nsList);
		return(NULL);
	    }

	    cur = nsList;
	    while (*cur != NULL) {
		if (xmlStrEqual((*cur)->href, nameSpace)) {
		    attrDecl = xmlGetDtdQAttrDesc(doc->intSubset, ename,
			                          name, (*cur)->prefix);
		    if ((attrDecl == NULL) && (doc->extSubset != NULL))
			attrDecl = xmlGetDtdQAttrDesc(doc->extSubset, ename,
						      name, (*cur)->prefix);
		}
		cur++;
	    }
	    xmlFree(nsList);
	    xmlFree(ename);
	    return((xmlAttrPtr) attrDecl);
	}
    }
    return(NULL);
}

/**
 * xmlGetProp:
 * @node:  the node
 * @name:  the attribute name
 *
 * Search and get the value of an attribute associated to a node
 * This does the entity substitution.
 * This function looks in DTD attribute declaration for #FIXED or
 * default declaration values unless DTD use has been turned off.
 *
 * Returns the attribute value or NULL if not found.
 *     It's up to the caller to free the memory with xmlFree().
 */
xmlChar *
xmlGetProp(xmlNodePtr node, const xmlChar *name) {
    xmlAttrPtr prop;
    xmlDocPtr doc;

    if ((node == NULL) || (name == NULL)) return(NULL);
    /*
     * Check on the properties attached to the node
     */
    prop = node->properties;
    while (prop != NULL) {
        if (xmlStrEqual(prop->name, name))  {
	    xmlChar *ret;

	    ret = xmlNodeListGetString(node->doc, prop->children, 1);
	    if (ret == NULL) return(xmlStrdup((xmlChar *)""));
	    return(ret);
        }
	prop = prop->next;
    }
    if (!xmlCheckDTD) return(NULL);

    /*
     * Check if there is a default declaration in the internal
     * or external subsets
     */
    doc =  node->doc;
    if (doc != NULL) {
        xmlAttributePtr attrDecl;
        if (doc->intSubset != NULL) {
	    attrDecl = xmlGetDtdAttrDesc(doc->intSubset, node->name, name);
	    if ((attrDecl == NULL) && (doc->extSubset != NULL))
		attrDecl = xmlGetDtdAttrDesc(doc->extSubset, node->name, name);
	    if (attrDecl != NULL)
		return(xmlStrdup(attrDecl->defaultValue));
	}
    }
    return(NULL);
}

/**
 * xmlGetNsProp:
 * @node:  the node
 * @name:  the attribute name
 * @nameSpace:  the URI of the namespace
 *
 * Search and get the value of an attribute associated to a node
 * This attribute has to be anchored in the namespace specified.
 * This does the entity substitution.
 * This function looks in DTD attribute declaration for #FIXED or
 * default declaration values unless DTD use has been turned off.
 *
 * Returns the attribute value or NULL if not found.
 *     It's up to the caller to free the memory with xmlFree().
 */
xmlChar *
xmlGetNsProp(xmlNodePtr node, const xmlChar *name, const xmlChar *nameSpace) {
    xmlAttrPtr prop;
    xmlDocPtr doc;
    xmlNsPtr ns;

    if (node == NULL)
	return(NULL);

    prop = node->properties;
    if (nameSpace == NULL)
	return(xmlGetProp(node, name));
    while (prop != NULL) {
	/*
	 * One need to have
	 *   - same attribute names
	 *   - and the attribute carrying that namespace
	 */
        if ((xmlStrEqual(prop->name, name)) &&
	    ((prop->ns != NULL) &&
	     (xmlStrEqual(prop->ns->href, nameSpace)))) {
	    xmlChar *ret;

	    ret = xmlNodeListGetString(node->doc, prop->children, 1);
	    if (ret == NULL) return(xmlStrdup((xmlChar *)""));
	    return(ret);
        }
	prop = prop->next;
    }
    if (!xmlCheckDTD) return(NULL);

    /*
     * Check if there is a default declaration in the internal
     * or external subsets
     */
    doc =  node->doc;
    if (doc != NULL) {
        if (doc->intSubset != NULL) {
	    xmlAttributePtr attrDecl;

	    attrDecl = xmlGetDtdAttrDesc(doc->intSubset, node->name, name);
	    if ((attrDecl == NULL) && (doc->extSubset != NULL))
		attrDecl = xmlGetDtdAttrDesc(doc->extSubset, node->name, name);
		
	    if ((attrDecl != NULL) && (attrDecl->prefix != NULL)) {
	        /*
		 * The DTD declaration only allows a prefix search
		 */
		ns = xmlSearchNs(doc, node, attrDecl->prefix);
		if ((ns != NULL) && (xmlStrEqual(ns->href, nameSpace)))
		    return(xmlStrdup(attrDecl->defaultValue));
	    }
	}
    }
    return(NULL);
}

/**
 * xmlSetProp:
 * @node:  the node
 * @name:  the attribute name
 * @value:  the attribute value
 *
 * Set (or reset) an attribute carried by a node.
 * Returns the attribute pointer.
 */
xmlAttrPtr
xmlSetProp(xmlNodePtr node, const xmlChar *name, const xmlChar *value) {
    xmlAttrPtr prop;
    xmlDocPtr doc;

    if ((node == NULL) || (name == NULL))
	return(NULL);
    doc = node->doc;
    prop = node->properties;
    while (prop != NULL) {
        if ((xmlStrEqual(prop->name, name)) &&
	    (prop->ns == NULL)){
	    xmlNodePtr oldprop = prop->children;

	    prop->children = NULL;
	    prop->last = NULL;
	    if (value != NULL) {
	        xmlChar *buffer;
		xmlNodePtr tmp;

		buffer = xmlEncodeEntitiesReentrant(node->doc, value);
		prop->children = xmlStringGetNodeList(node->doc, buffer);
		prop->last = NULL;
		prop->doc = doc;
		tmp = prop->children;
		while (tmp != NULL) {
		    tmp->parent = (xmlNodePtr) prop;
		    tmp->doc = doc;
		    if (tmp->next == NULL)
			prop->last = tmp;
		    tmp = tmp->next;
		}
		xmlFree(buffer);
	    }
	    if (oldprop != NULL) 
	        xmlFreeNodeList(oldprop);
	    return(prop);
	}
	prop = prop->next;
    }
    prop = xmlNewProp(node, name, value);
    return(prop);
}

/**
 * xmlUnsetProp:
 * @node:  the node
 * @name:  the attribute name
 *
 * Remove an attribute carried by a node.
 * Returns 0 if successful, -1 if not found
 */
int
xmlUnsetProp(xmlNodePtr node, const xmlChar *name) {
    xmlAttrPtr prop = node->properties, prev = NULL;;

    if ((node == NULL) || (name == NULL))
	return(-1);
    while (prop != NULL) {
        if ((xmlStrEqual(prop->name, name)) &&
	    (prop->ns == NULL)) {
	    if (prev == NULL)
		node->properties = prop->next;
	    else
		prev->next = prop->next;
	    xmlFreeProp(prop);
	    return(0);
	}
	prev = prop;
	prop = prop->next;
    }
    return(-1);
}

/**
 * xmlSetNsProp:
 * @node:  the node
 * @ns:  the namespace definition
 * @name:  the attribute name
 * @value:  the attribute value
 *
 * Set (or reset) an attribute carried by a node.
 * The ns structure must be in scope, this is not checked.
 *
 * Returns the attribute pointer.
 */
xmlAttrPtr
xmlSetNsProp(xmlNodePtr node, xmlNsPtr ns, const xmlChar *name,
	     const xmlChar *value) {
    xmlAttrPtr prop;
    
    if ((node == NULL) || (name == NULL))
	return(NULL);

    if (ns == NULL)
	return(xmlSetProp(node, name, value));
    if (ns->href == NULL)
	return(NULL);
    prop = node->properties;

    while (prop != NULL) {
	/*
	 * One need to have
	 *   - same attribute names
	 *   - and the attribute carrying that namespace
	 */
        if ((xmlStrEqual(prop->name, name)) &&
	    (prop->ns != NULL) && (xmlStrEqual(prop->ns->href, ns->href))) {
	    if (prop->children != NULL) 
	        xmlFreeNodeList(prop->children);
	    prop->children = NULL;
	    prop->last = NULL;
	    prop->ns = ns;
	    if (value != NULL) {
	        xmlChar *buffer;
		xmlNodePtr tmp;

		buffer = xmlEncodeEntitiesReentrant(node->doc, value);
		prop->children = xmlStringGetNodeList(node->doc, buffer);
		prop->last = NULL;
		tmp = prop->children;
		while (tmp != NULL) {
		    tmp->parent = (xmlNodePtr) prop;
		    if (tmp->next == NULL)
			prop->last = tmp;
		    tmp = tmp->next;
		}
		xmlFree(buffer);
	    }	
	    return(prop);
        }
	prop = prop->next;
    }
    prop = xmlNewNsProp(node, ns, name, value);
    return(prop);
}

/**
 * xmlUnsetNsProp:
 * @node:  the node
 * @ns:  the namespace definition
 * @name:  the attribute name
 *
 * Remove an attribute carried by a node.
 * Returns 0 if successful, -1 if not found
 */
int
xmlUnsetNsProp(xmlNodePtr node, xmlNsPtr ns, const xmlChar *name) {
    xmlAttrPtr prop = node->properties, prev = NULL;;

    if ((node == NULL) || (name == NULL))
	return(-1);
    if (ns == NULL)
	return(xmlUnsetProp(node, name));
    if (ns->href == NULL)
	return(-1);
    while (prop != NULL) {
        if ((xmlStrEqual(prop->name, name)) &&
	    (prop->ns != NULL) && (xmlStrEqual(prop->ns->href, ns->href))) {
	    if (prev == NULL)
		node->properties = prop->next;
	    else
		prev->next = prop->next;
	    xmlFreeProp(prop);
	    return(0);
	}
	prev = prop;
	prop = prop->next;
    }
    return(-1);
}

/**
 * xmlNodeIsText:
 * @node:  the node
 * 
 * Is this node a Text node ?
 * Returns 1 yes, 0 no
 */
int
xmlNodeIsText(xmlNodePtr node) {
    if (node == NULL) return(0);

    if (node->type == XML_TEXT_NODE) return(1);
    return(0);
}

/**
 * xmlIsBlankNode:
 * @node:  the node
 * 
 * Checks whether this node is an empty or whitespace only
 * (and possibly ignorable) text-node.
 *
 * Returns 1 yes, 0 no
 */
int
xmlIsBlankNode(xmlNodePtr node) {
    const xmlChar *cur;
    if (node == NULL) return(0);

    if ((node->type != XML_TEXT_NODE) &&
        (node->type != XML_CDATA_SECTION_NODE))
	return(0);
    if (node->content == NULL) return(1);
    cur = node->content;
    while (*cur != 0) {
	if (!IS_BLANK(*cur)) return(0);
	cur++;
    }

    return(1);
}

/**
 * xmlTextConcat:
 * @node:  the node
 * @content:  the content
 * @len:  @content length
 * 
 * Concat the given string at the end of the existing node content
 */

void
xmlTextConcat(xmlNodePtr node, const xmlChar *content, int len) {
    if (node == NULL) return;

    if ((node->type != XML_TEXT_NODE) &&
        (node->type != XML_CDATA_SECTION_NODE)) {
#ifdef DEBUG_TREE
	xmlGenericError(xmlGenericErrorContext,
		"xmlTextConcat: node is not text nor CDATA\n");
#endif
        return;
    }
    node->content = xmlStrncat(node->content, content, len);
}

/************************************************************************
 *									*
 *			Output : to a FILE or in memory			*
 *									*
 ************************************************************************/

/**
 * xmlBufferCreate:
 *
 * routine to create an XML buffer.
 * returns the new structure.
 */
xmlBufferPtr
xmlBufferCreate(void) {
    xmlBufferPtr ret;

    ret = (xmlBufferPtr) xmlMalloc(sizeof(xmlBuffer));
    if (ret == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		"xmlBufferCreate : out of memory!\n");
        return(NULL);
    }
    ret->use = 0;
    ret->size = xmlDefaultBufferSize;
    ret->alloc = xmlBufferAllocScheme;
    ret->content = (xmlChar *) xmlMalloc(ret->size * sizeof(xmlChar));
    if (ret->content == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		"xmlBufferCreate : out of memory!\n");
	xmlFree(ret);
        return(NULL);
    }
    ret->content[0] = 0;
    return(ret);
}

/**
 * xmlBufferCreateSize:
 * @size: initial size of buffer
 *
 * routine to create an XML buffer.
 * returns the new structure.
 */
xmlBufferPtr
xmlBufferCreateSize(size_t size) {
    xmlBufferPtr ret;

    ret = (xmlBufferPtr) xmlMalloc(sizeof(xmlBuffer));
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlBufferCreate : out of memory!\n");
        return(NULL);
    }
    ret->use = 0;
    ret->alloc = xmlBufferAllocScheme;
    ret->size = (size ? size+2 : 0);         /* +1 for ending null */
    if (ret->size){
        ret->content = (xmlChar *) xmlMalloc(ret->size * sizeof(xmlChar));
        if (ret->content == NULL) {
            xmlGenericError(xmlGenericErrorContext,
		    "xmlBufferCreate : out of memory!\n");
            xmlFree(ret);
            return(NULL);
        }
        ret->content[0] = 0;
    } else
	ret->content = NULL;
    return(ret);
}

/**
 * xmlBufferSetAllocationScheme:
 * @buf:  the buffer to tune
 * @scheme:  allocation scheme to use
 *
 * Sets the allocation scheme for this buffer
 */
void
xmlBufferSetAllocationScheme(xmlBufferPtr buf, 
                             xmlBufferAllocationScheme scheme) {
    if (buf == NULL) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext,
		"xmlBufferSetAllocationScheme: buf == NULL\n");
#endif
        return;
    }

    buf->alloc = scheme;
}

/**
 * xmlBufferFree:
 * @buf:  the buffer to free
 *
 * Frees an XML buffer. It frees both the content and the structure which
 * encapsulate it.
 */
void
xmlBufferFree(xmlBufferPtr buf) {
    if (buf == NULL) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext,
		"xmlBufferFree: buf == NULL\n");
#endif
	return;
    }
    if (buf->content != NULL) {
        xmlFree(buf->content);
    }
    xmlFree(buf);
}

/**
 * xmlBufferEmpty:
 * @buf:  the buffer
 *
 * empty a buffer.
 */
void
xmlBufferEmpty(xmlBufferPtr buf) {
    if (buf->content == NULL) return;
    buf->use = 0;
    memset(buf->content, 0, buf->size);
}

/**
 * xmlBufferShrink:
 * @buf:  the buffer to dump
 * @len:  the number of xmlChar to remove
 *
 * Remove the beginning of an XML buffer.
 *
 * Returns the number of #xmlChar removed, or -1 in case of failure.
 */
int
xmlBufferShrink(xmlBufferPtr buf, unsigned int len) {
    if (len == 0) return(0);
    if (len > buf->use) return(-1);

    buf->use -= len;
    memmove(buf->content, &buf->content[len], buf->use * sizeof(xmlChar));

    buf->content[buf->use] = 0;
    return(len);
}

/**
 * xmlBufferGrow:
 * @buf:  the buffer
 * @len:  the minimum free size to allocate
 *
 * Grow the available space of an XML buffer.
 *
 * Returns the new available space or -1 in case of error
 */
int
xmlBufferGrow(xmlBufferPtr buf, unsigned int len) {
    int size;
    xmlChar *newbuf;

    if (len + buf->use < buf->size) return(0);

    size = buf->use + len + 100;

    newbuf = (xmlChar *) xmlRealloc(buf->content, size);
    if (newbuf == NULL) return(-1);
    buf->content = newbuf;
    buf->size = size;
    return(buf->size - buf->use);
}

/**
 * xmlBufferDump:
 * @file:  the file output
 * @buf:  the buffer to dump
 *
 * Dumps an XML buffer to  a FILE *.
 * Returns the number of #xmlChar written
 */
int
xmlBufferDump(FILE *file, xmlBufferPtr buf) {
    int ret;

    if (buf == NULL) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext,
		"xmlBufferDump: buf == NULL\n");
#endif
	return(0);
    }
    if (buf->content == NULL) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext,
		"xmlBufferDump: buf->content == NULL\n");
#endif
	return(0);
    }
    if (file == NULL)
	file = stdout;
    ret = fwrite(buf->content, sizeof(xmlChar), buf->use, file);
    return(ret);
}

/**
 * xmlBufferContent:
 * @buf:  the buffer
 *
 * Function to extract the content of a buffer
 *
 * Returns the internal content
 */

const xmlChar *
xmlBufferContent(const xmlBufferPtr buf)
{
    if(!buf)
        return NULL;

    return buf->content;
}

/**
 * xmlBufferLength:
 * @buf:  the buffer 
 *
 * Function to get the length of a buffer
 *
 * Returns the length of data in the internal content
 */

int
xmlBufferLength(const xmlBufferPtr buf)
{
    if(!buf)
        return 0;

    return buf->use;
}

/**
 * xmlBufferResize:
 * @buf:  the buffer to resize
 * @size:  the desired size
 *
 * Resize a buffer to accommodate minimum size of @size.
 *
 * Returns  0 in case of problems, 1 otherwise
 */
int
xmlBufferResize(xmlBufferPtr buf, unsigned int size)
{
    unsigned int newSize;
    xmlChar* rebuf = NULL;

    /*take care of empty case*/
    newSize = (buf->size ? buf->size*2 : size);

    /* Don't resize if we don't have to */
    if (size < buf->size)
        return 1;

    /* figure out new size */
    switch (buf->alloc){
    case XML_BUFFER_ALLOC_DOUBLEIT:
        while (size > newSize) newSize *= 2;
        break;
    case XML_BUFFER_ALLOC_EXACT:
        newSize = size+10;
        break;
    default:
        newSize = size+10;
        break;
    }

    if (buf->content == NULL)
	rebuf = (xmlChar *) xmlMalloc(newSize * sizeof(xmlChar));
    else
	rebuf = (xmlChar *) xmlRealloc(buf->content, 
				       newSize * sizeof(xmlChar));
    if (rebuf == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlBufferResize : out of memory!\n");
        return 0;
    }
    buf->content = rebuf;
    buf->size = newSize;

    return 1;
}

/**
 * xmlBufferAdd:
 * @buf:  the buffer to dump
 * @str:  the #xmlChar string
 * @len:  the number of #xmlChar to add
 *
 * Add a string range to an XML buffer. if len == -1, the length of
 * str is recomputed.
 */
void
xmlBufferAdd(xmlBufferPtr buf, const xmlChar *str, int len) {
    unsigned int needSize;

    if (str == NULL) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext,
		"xmlBufferAdd: str == NULL\n");
#endif
	return;
    }
    if (len < -1) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext,
		"xmlBufferAdd: len < 0\n");
#endif
	return;
    }
    if (len == 0) return;

    if (len < 0)
        len = xmlStrlen(str);

    if (len <= 0) return;

    needSize = buf->use + len + 2;
    if (needSize > buf->size){
        if (!xmlBufferResize(buf, needSize)){
            xmlGenericError(xmlGenericErrorContext,
		    "xmlBufferAdd : out of memory!\n");
            return;
        }
    }

    memmove(&buf->content[buf->use], str, len*sizeof(xmlChar));
    buf->use += len;
    buf->content[buf->use] = 0;
}

/**
 * xmlBufferAddHead:
 * @buf:  the buffer
 * @str:  the #xmlChar string
 * @len:  the number of #xmlChar to add
 *
 * Add a string range to the beginning of an XML buffer.
 * if len == -1, the length of @str is recomputed.
 */
void
xmlBufferAddHead(xmlBufferPtr buf, const xmlChar *str, int len) {
    unsigned int needSize;

    if (str == NULL) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext,
		"xmlBufferAddHead: str == NULL\n");
#endif
	return;
    }
    if (len < -1) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext,
		"xmlBufferAddHead: len < 0\n");
#endif
	return;
    }
    if (len == 0) return;

    if (len < 0)
        len = xmlStrlen(str);

    if (len <= 0) return;

    needSize = buf->use + len + 2;
    if (needSize > buf->size){
        if (!xmlBufferResize(buf, needSize)){
            xmlGenericError(xmlGenericErrorContext,
		    "xmlBufferAddHead : out of memory!\n");
            return;
        }
    }

    memmove(&buf->content[len], &buf->content[0], buf->use * sizeof(xmlChar));
    memmove(&buf->content[0], str, len * sizeof(xmlChar));
    buf->use += len;
    buf->content[buf->use] = 0;
}

/**
 * xmlBufferCat:
 * @buf:  the buffer to dump
 * @str:  the #xmlChar string
 *
 * Append a zero terminated string to an XML buffer.
 */
void
xmlBufferCat(xmlBufferPtr buf, const xmlChar *str) {
    if (str != NULL)
	xmlBufferAdd(buf, str, -1);
}

/**
 * xmlBufferCCat:
 * @buf:  the buffer to dump
 * @str:  the C char string
 *
 * Append a zero terminated C string to an XML buffer.
 */
void
xmlBufferCCat(xmlBufferPtr buf, const char *str) {
    const char *cur;

    if (str == NULL) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext,
		"xmlBufferCCat: str == NULL\n");
#endif
	return;
    }
    for (cur = str;*cur != 0;cur++) {
        if (buf->use  + 10 >= buf->size) {
            if (!xmlBufferResize(buf, buf->use+10)){
                xmlGenericError(xmlGenericErrorContext,
			"xmlBufferCCat : out of memory!\n");
                return;
            }
        }
        buf->content[buf->use++] = *cur;
    }
    buf->content[buf->use] = 0;
}

/**
 * xmlBufferWriteCHAR:
 * @buf:  the XML buffer
 * @string:  the string to add
 *
 * routine which manages and grows an output buffer. This one adds
 * xmlChars at the end of the buffer.
 */
void
xmlBufferWriteCHAR
(xmlBufferPtr buf, const xmlChar *string) {
    xmlBufferCat(buf, string);
}

/**
 * xmlBufferWriteChar:
 * @buf:  the XML buffer output
 * @string:  the string to add
 *
 * routine which manage and grows an output buffer. This one add
 * C chars at the end of the array.
 */
void
xmlBufferWriteChar(xmlBufferPtr buf, const char *string) {
    xmlBufferCCat(buf, string);
}


/**
 * xmlBufferWriteQuotedString:
 * @buf:  the XML buffer output
 * @string:  the string to add
 *
 * routine which manage and grows an output buffer. This one writes
 * a quoted or double quoted #xmlChar string, checking first if it holds
 * quote or double-quotes internally
 */
void
xmlBufferWriteQuotedString(xmlBufferPtr buf, const xmlChar *string) {
    if (xmlStrchr(string, '"')) {
        if (xmlStrchr(string, '\'')) {
#ifdef DEBUG_BUFFER
	    xmlGenericError(xmlGenericErrorContext,
 "xmlBufferWriteQuotedString: string contains quote and double-quotes !\n");
#endif
	}
        xmlBufferCCat(buf, "'");
        xmlBufferCat(buf, string);
        xmlBufferCCat(buf, "'");
    } else {
        xmlBufferCCat(buf, "\"");
        xmlBufferCat(buf, string);
        xmlBufferCCat(buf, "\"");
    }
}


/************************************************************************
 *									*
 *   		Dumping XML tree content to a simple buffer		*
 *									*
 ************************************************************************/

/**
 * xmlAttrSerializeContent:
 * @buf:  the XML buffer output
 * @doc:  the document
 * @attr:  the attribute pointer
 *
 * Serialize the attribute in the buffer
 */
static void
xmlAttrSerializeContent(xmlBufferPtr buf, xmlDocPtr doc, xmlAttrPtr attr)
{
    const xmlChar *cur, *base;
    xmlNodePtr children;

    children = attr->children;
    while (children != NULL) {
        switch (children->type) {
            case XML_TEXT_NODE:
                base = cur = children->content;
                while (*cur != 0) {
                    if (*cur == '\n') {
                        if (base != cur)
                            xmlBufferAdd(buf, base, cur - base);
                        xmlBufferAdd(buf, BAD_CAST "&#10;", 5);
                        cur++;
                        base = cur;
#if 0
                    } else if (*cur == '\'') {
                        if (base != cur)
                            xmlBufferAdd(buf, base, cur - base);
                        xmlBufferAdd(buf, BAD_CAST "&apos;", 6);
                        cur++;
                        base = cur;
#endif
                    } else if (*cur == '"') {
                        if (base != cur)
                            xmlBufferAdd(buf, base, cur - base);
                        xmlBufferAdd(buf, BAD_CAST "&quot;", 6);
                        cur++;
                        base = cur;
                    } else if (*cur == '<') {
                        if (base != cur)
                            xmlBufferAdd(buf, base, cur - base);
                        xmlBufferAdd(buf, BAD_CAST "&lt;", 4);
                        cur++;
                        base = cur;
                    } else if (*cur == '>') {
                        if (base != cur)
                            xmlBufferAdd(buf, base, cur - base);
                        xmlBufferAdd(buf, BAD_CAST "&gt;", 4);
                        cur++;
                        base = cur;
                    } else if (*cur == '&') {
                        if (base != cur)
                            xmlBufferAdd(buf, base, cur - base);
                        xmlBufferAdd(buf, BAD_CAST "&amp;", 5);
                        cur++;
                        base = cur;
                    } else if ((*cur >= 0x80) && ((doc == NULL) ||
                                                  (doc->encoding ==
                                                   NULL))) {
                        /*
                         * We assume we have UTF-8 content.
                         */
                        char tmp[10];
                        int val = 0, l = 1;

                        if (base != cur)
                            xmlBufferAdd(buf, base, cur - base);
                        if (*cur < 0xC0) {
                            xmlGenericError(xmlGenericErrorContext,
			    "xmlAttrSerializeContent : input not UTF-8\n");
                            if (doc != NULL)
                                doc->encoding =
                                    xmlStrdup(BAD_CAST "ISO-8859-1");
                            snprintf(tmp, sizeof(tmp), "&#%d;", *cur);
                            tmp[sizeof(tmp) - 1] = 0;
                            xmlBufferAdd(buf, (xmlChar *) tmp, -1);
                            cur++;
                            base = cur;
                            continue;
                        } else if (*cur < 0xE0) {
                            val = (cur[0]) & 0x1F;
                            val <<= 6;
                            val |= (cur[1]) & 0x3F;
                            l = 2;
                        } else if (*cur < 0xF0) {
                            val = (cur[0]) & 0x0F;
                            val <<= 6;
                            val |= (cur[1]) & 0x3F;
                            val <<= 6;
                            val |= (cur[2]) & 0x3F;
                            l = 3;
                        } else if (*cur < 0xF8) {
                            val = (cur[0]) & 0x07;
                            val <<= 6;
                            val |= (cur[1]) & 0x3F;
                            val <<= 6;
                            val |= (cur[2]) & 0x3F;
                            val <<= 6;
                            val |= (cur[3]) & 0x3F;
                            l = 4;
                        }
                        if ((l == 1) || (!IS_CHAR(val))) {
                            xmlGenericError(xmlGenericErrorContext,
			    "xmlAttrSerializeContent : char out of range\n");
                            if (doc != NULL)
                                doc->encoding =
                                    xmlStrdup(BAD_CAST "ISO-8859-1");
                            snprintf(tmp, sizeof(tmp), "&#%d;", *cur);
                            tmp[sizeof(tmp) - 1] = 0;
                            xmlBufferAdd(buf, (xmlChar *) tmp, -1);
                            cur++;
                            base = cur;
                            continue;
                        }
                        /*
                         * We could do multiple things here. Just save
                         * as a char ref
                         */
                        snprintf(tmp, sizeof(tmp), "&#x%X;", val);
                        tmp[sizeof(tmp) - 1] = 0;
                        xmlBufferAdd(buf, (xmlChar *) tmp, -1);
                        cur += l;
                        base = cur;
                    } else {
                        cur++;
                    }
                }
                if (base != cur)
                    xmlBufferAdd(buf, base, cur - base);
                break;
            case XML_ENTITY_REF_NODE:
                xmlBufferAdd(buf, BAD_CAST "&", 1);
                xmlBufferAdd(buf, children->name,
                             xmlStrlen(children->name));
                xmlBufferAdd(buf, BAD_CAST ";", 1);
                break;
            default:
                /* should not happen unless we have a badly built tree */
                break;
        }
        children = children->next;
    }
}

/**
 * xmlNodeDump:
 * @buf:  the XML buffer output
 * @doc:  the document
 * @cur:  the current node
 * @level: the imbrication level for indenting
 * @format: is formatting allowed
 *
 * Dump an XML node, recursive behaviour,children are printed too.
 * Note that format = 1 provide node indenting only if xmlIndentTreeOutput = 1
 * or xmlKeepBlanksDefault(0) was called
 *
 * Returns the number of bytes written to the buffer or -1 in case of error
 */
int
xmlNodeDump(xmlBufferPtr buf, xmlDocPtr doc, xmlNodePtr cur, int level,
            int format)
{
    unsigned int use;
    int ret;
    xmlOutputBufferPtr outbuf;

    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
                        "xmlNodeDump : node == NULL\n");
#endif
        return (-1);
    }
    if (buf == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
                        "xmlNodeDump : buf == NULL\n");
#endif
        return (-1);
    }
    outbuf = (xmlOutputBufferPtr) xmlMalloc(sizeof(xmlOutputBuffer));
    if (outbuf == NULL) {
        xmlGenericError(xmlGenericErrorContext,
                        "xmlNodeDump: out of memory!\n");
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
    xmlNodeDumpOutput(outbuf, doc, cur, level, format, NULL);
    xmlFree(outbuf);
    ret = buf->use - use;
    return (ret);
}

/**
 * xmlElemDump:
 * @f:  the FILE * for the output
 * @doc:  the document
 * @cur:  the current node
 *
 * Dump an XML/HTML node, recursive behaviour, children are printed too.
 */
void
xmlElemDump(FILE * f, xmlDocPtr doc, xmlNodePtr cur)
{
    xmlOutputBufferPtr outbuf;

    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
                        "xmlElemDump : cur == NULL\n");
#endif
        return;
    }
#ifdef DEBUG_TREE
    if (doc == NULL) {
        xmlGenericError(xmlGenericErrorContext,
                        "xmlElemDump : doc == NULL\n");
    }
#endif

    outbuf = xmlOutputBufferCreateFile(f, NULL);
    if (outbuf == NULL)
        return;
    if ((doc != NULL) && (doc->type == XML_HTML_DOCUMENT_NODE)) {
#ifdef LIBXML_HTML_ENABLED
        htmlNodeDumpOutput(outbuf, doc, cur, NULL);
#else
        xmlGenericError(xmlGenericErrorContext,
                        "HTML support not compiled in\n");
#endif /* LIBXML_HTML_ENABLED */
    } else
        xmlNodeDumpOutput(outbuf, doc, cur, 0, 1, NULL);
    xmlOutputBufferClose(outbuf);
}

/************************************************************************
 *									*
 *   		Dumping XML tree content to an I/O output buffer	*
 *									*
 ************************************************************************/

static void
xhtmlNodeDumpOutput(xmlOutputBufferPtr buf, xmlDocPtr doc, xmlNodePtr cur,
            int level, int format, const char *encoding);
static void
xmlNodeListDumpOutput(xmlOutputBufferPtr buf, xmlDocPtr doc, xmlNodePtr cur,
                  int level, int format, const char *encoding);
static void
xmlNodeDumpOutputInternal(xmlOutputBufferPtr buf, xmlDocPtr doc,
	    xmlNodePtr cur, int level, int format, const char *encoding);

/**
 * xmlNsDumpOutput:
 * @buf:  the XML buffer output
 * @cur:  a namespace
 *
 * Dump a local Namespace definition.
 * Should be called in the context of attributes dumps.
 */
static void
xmlNsDumpOutput(xmlOutputBufferPtr buf, xmlNsPtr cur) {
    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlNsDumpOutput : Ns == NULL\n");
#endif
	return;
    }
    if ((cur->type == XML_LOCAL_NAMESPACE) && (cur->href != NULL)) {
	if (xmlStrEqual(cur->prefix, BAD_CAST "xml"))
	    return;

        /* Within the context of an element attributes */
	if (cur->prefix != NULL) {
	    xmlOutputBufferWriteString(buf, " xmlns:");
	    xmlOutputBufferWriteString(buf, (const char *)cur->prefix);
	} else
	    xmlOutputBufferWriteString(buf, " xmlns");
	xmlOutputBufferWriteString(buf, "=");
	xmlBufferWriteQuotedString(buf->buffer, cur->href);
    }
}

/**
 * xmlNsListDumpOutput:
 * @buf:  the XML buffer output
 * @cur:  the first namespace
 *
 * Dump a list of local Namespace definitions.
 * Should be called in the context of attributes dumps.
 */
static void
xmlNsListDumpOutput(xmlOutputBufferPtr buf, xmlNsPtr cur) {
    while (cur != NULL) {
        xmlNsDumpOutput(buf, cur);
	cur = cur->next;
    }
}

/**
 * xmlDtdDumpOutput:
 * @buf:  the XML buffer output
 * @doc:  the document
 * @encoding:  an optional encoding string
 * 
 * Dump the XML document DTD, if any.
 */
static void
xmlDtdDumpOutput(xmlOutputBufferPtr buf, xmlDtdPtr dtd, const char *encoding) {
    if (dtd == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlDtdDumpOutput : no internal subset\n");
#endif
	return;
    }
    xmlOutputBufferWriteString(buf, "<!DOCTYPE ");
    xmlOutputBufferWriteString(buf, (const char *)dtd->name);
    if (dtd->ExternalID != NULL) {
	xmlOutputBufferWriteString(buf, " PUBLIC ");
	xmlBufferWriteQuotedString(buf->buffer, dtd->ExternalID);
	xmlOutputBufferWriteString(buf, " ");
	xmlBufferWriteQuotedString(buf->buffer, dtd->SystemID);
    }  else if (dtd->SystemID != NULL) {
	xmlOutputBufferWriteString(buf, " SYSTEM ");
	xmlBufferWriteQuotedString(buf->buffer, dtd->SystemID);
    }
    if ((dtd->entities == NULL) && (dtd->elements == NULL) &&
        (dtd->attributes == NULL) && (dtd->notations == NULL)) {
	xmlOutputBufferWriteString(buf, ">");
	return;
    }
    xmlOutputBufferWriteString(buf, " [\n");
    xmlNodeListDumpOutput(buf, dtd->doc, dtd->children, -1, 0, encoding);
    xmlOutputBufferWriteString(buf, "]>");
}

/**
 * xmlAttrDumpOutput:
 * @buf:  the XML buffer output
 * @doc:  the document
 * @cur:  the attribute pointer
 * @encoding:  an optional encoding string
 *
 * Dump an XML attribute
 */
static void
xmlAttrDumpOutput(xmlOutputBufferPtr buf, xmlDocPtr doc, xmlAttrPtr cur,
	          const char *encoding ATTRIBUTE_UNUSED) {
    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlAttrDumpOutput : property == NULL\n");
#endif
	return;
    }
    xmlOutputBufferWriteString(buf, " ");
    if ((cur->ns != NULL) && (cur->ns->prefix != NULL)) {
        xmlOutputBufferWriteString(buf, (const char *)cur->ns->prefix);
	xmlOutputBufferWriteString(buf, ":");
    }
    xmlOutputBufferWriteString(buf, (const char *)cur->name);
    xmlOutputBufferWriteString(buf, "=\"");
    xmlAttrSerializeContent(buf->buffer, doc, cur);
    xmlOutputBufferWriteString(buf, "\"");
}

/**
 * xmlAttrListDumpOutput:
 * @buf:  the XML buffer output
 * @doc:  the document
 * @cur:  the first attribute pointer
 * @encoding:  an optional encoding string
 *
 * Dump a list of XML attributes
 */
static void
xmlAttrListDumpOutput(xmlOutputBufferPtr buf, xmlDocPtr doc,
	              xmlAttrPtr cur, const char *encoding) {
    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlAttrListDumpOutput : property == NULL\n");
#endif
	return;
    }
    while (cur != NULL) {
        xmlAttrDumpOutput(buf, doc, cur, encoding);
	cur = cur->next;
    }
}



/**
 * xmlNodeListDumpOutput:
 * @buf:  the XML buffer output
 * @doc:  the document
 * @cur:  the first node
 * @level: the imbrication level for indenting
 * @format: is formatting allowed
 * @encoding:  an optional encoding string
 *
 * Dump an XML node list, recursive behaviour, children are printed too.
 * Note that format = 1 provide node indenting only if xmlIndentTreeOutput = 1
 * or xmlKeepBlanksDefault(0) was called
 */
static void
xmlNodeListDumpOutput(xmlOutputBufferPtr buf, xmlDocPtr doc,
                xmlNodePtr cur, int level, int format, const char *encoding) {
    int i;

    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlNodeListDumpOutput : node == NULL\n");
#endif
	return;
    }
    while (cur != NULL) {
	if ((format) && (xmlIndentTreeOutput) &&
	    (cur->type == XML_ELEMENT_NODE))
	    for (i = 0;i < level;i++)
		xmlOutputBufferWriteString(buf, xmlTreeIndentString);
        xmlNodeDumpOutputInternal(buf, doc, cur, level, format, encoding);
	if (format) {
	    xmlOutputBufferWriteString(buf, "\n");
	}
	cur = cur->next;
    }
}

/**
 * xmlNodeDumpOutputInternal:
 * @buf:  the XML buffer output
 * @doc:  the document
 * @cur:  the current node
 * @level: the imbrication level for indenting
 * @format: is formatting allowed
 * @encoding:  an optional encoding string
 *
 * Dump an XML node, recursive behaviour, children are printed too.
 * Note that format = 1 provide node indenting only if xmlIndentTreeOutput = 1
 * or xmlKeepBlanksDefault(0) was called
 */
static void
xmlNodeDumpOutputInternal(xmlOutputBufferPtr buf, xmlDocPtr doc,
	    xmlNodePtr cur, int level, int format, const char *encoding) {
    int i;
    xmlNodePtr tmp;

    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlNodeDumpOutput : node == NULL\n");
#endif
	return;
    }
    if (cur->type == XML_XINCLUDE_START)
	return;
    if (cur->type == XML_XINCLUDE_END)
	return;
    if (cur->type == XML_DTD_NODE) {
        xmlDtdDumpOutput(buf, (xmlDtdPtr) cur, encoding);
	return;
    }
    if (cur->type == XML_ELEMENT_DECL) {
        xmlDumpElementDecl(buf->buffer, (xmlElementPtr) cur);
	return;
    }
    if (cur->type == XML_ATTRIBUTE_DECL) {
        xmlDumpAttributeDecl(buf->buffer, (xmlAttributePtr) cur);
	return;
    }
    if (cur->type == XML_ENTITY_DECL) {
        xmlDumpEntityDecl(buf->buffer, (xmlEntityPtr) cur);
	return;
    }
    if (cur->type == XML_TEXT_NODE) {
	if (cur->content != NULL) {
	    if ((cur->name == xmlStringText) ||
		(cur->name != xmlStringTextNoenc)) {
		xmlChar *buffer;

		if (encoding == NULL)
		    buffer = xmlEncodeEntitiesReentrant(doc, cur->content);
		else
		    buffer = xmlEncodeSpecialChars(doc, cur->content);
		if (buffer != NULL) {
		    xmlOutputBufferWriteString(buf, (const char *)buffer);
		    xmlFree(buffer);
		}
	    } else {
		/*
		 * Disable escaping, needed for XSLT
		 */
		xmlOutputBufferWriteString(buf, (const char *) cur->content);
	    }
	}

	return;
    }
    if (cur->type == XML_PI_NODE) {
	if (cur->content != NULL) {
	    xmlOutputBufferWriteString(buf, "<?");
	    xmlOutputBufferWriteString(buf, (const char *)cur->name);
	    if (cur->content != NULL) {
		xmlOutputBufferWriteString(buf, " ");
		xmlOutputBufferWriteString(buf, (const char *)cur->content);
	    }
	    xmlOutputBufferWriteString(buf, "?>");
	} else {
	    xmlOutputBufferWriteString(buf, "<?");
	    xmlOutputBufferWriteString(buf, (const char *)cur->name);
	    xmlOutputBufferWriteString(buf, "?>");
	}
	return;
    }
    if (cur->type == XML_COMMENT_NODE) {
	if (cur->content != NULL) {
	    xmlOutputBufferWriteString(buf, "<!--");
	    xmlOutputBufferWriteString(buf, (const char *)cur->content);
	    xmlOutputBufferWriteString(buf, "-->");
	}
	return;
    }
    if (cur->type == XML_ENTITY_REF_NODE) {
        xmlOutputBufferWriteString(buf, "&");
	xmlOutputBufferWriteString(buf, (const char *)cur->name);
        xmlOutputBufferWriteString(buf, ";");
	return;
    }
    if (cur->type == XML_CDATA_SECTION_NODE) {
        xmlOutputBufferWriteString(buf, "<![CDATA[");
	if (cur->content != NULL)
	    xmlOutputBufferWriteString(buf, (const char *)cur->content);
        xmlOutputBufferWriteString(buf, "]]>");
	return;
    }

    if (format == 1) {
	tmp = cur->children;
	while (tmp != NULL) {
	    if ((tmp->type == XML_TEXT_NODE) || 
		(tmp->type == XML_ENTITY_REF_NODE)) {
		format = 0;
		break;
	    }
	    tmp = tmp->next;
	}
    }
    xmlOutputBufferWriteString(buf, "<");
    if ((cur->ns != NULL) && (cur->ns->prefix != NULL)) {
        xmlOutputBufferWriteString(buf, (const char *)cur->ns->prefix);
	xmlOutputBufferWriteString(buf, ":");
    }

    xmlOutputBufferWriteString(buf, (const char *)cur->name);
    if (cur->nsDef)
        xmlNsListDumpOutput(buf, cur->nsDef);
    if (cur->properties != NULL)
        xmlAttrListDumpOutput(buf, doc, cur->properties, encoding);

    if (((cur->type == XML_ELEMENT_NODE) || (cur->content == NULL)) &&
	(cur->children == NULL) && (!xmlSaveNoEmptyTags)) {
        xmlOutputBufferWriteString(buf, "/>");
	return;
    }
    xmlOutputBufferWriteString(buf, ">");
    if ((cur->type != XML_ELEMENT_NODE) && (cur->content != NULL)) {
	xmlChar *buffer;

	if (encoding == NULL)
	    buffer = xmlEncodeEntitiesReentrant(doc, cur->content);
	else
	    buffer = xmlEncodeSpecialChars(doc, cur->content);
	if (buffer != NULL) {
	    xmlOutputBufferWriteString(buf, (const char *)buffer);
	    xmlFree(buffer);
	}
    }
    if (cur->children != NULL) {
	if (format) xmlOutputBufferWriteString(buf, "\n");
	xmlNodeListDumpOutput(buf, doc, cur->children,
		        (level >= 0?level+1:-1), format, encoding);
	if ((xmlIndentTreeOutput) && (format))
	    for (i = 0;i < level;i++)
		xmlOutputBufferWriteString(buf, xmlTreeIndentString);
    }
    xmlOutputBufferWriteString(buf, "</");
    if ((cur->ns != NULL) && (cur->ns->prefix != NULL)) {
        xmlOutputBufferWriteString(buf, (const char *)cur->ns->prefix);
	xmlOutputBufferWriteString(buf, ":");
    }

    xmlOutputBufferWriteString(buf, (const char *)cur->name);
    xmlOutputBufferWriteString(buf, ">");
}

/**
 * xmlNodeDumpOutput:
 * @buf:  the XML buffer output
 * @doc:  the document
 * @cur:  the current node
 * @level: the imbrication level for indenting
 * @format: is formatting allowed
 * @encoding:  an optional encoding string
 *
 * Dump an XML node, recursive behaviour, children are printed too.
 * Note that format = 1 provide node indenting only if xmlIndentTreeOutput = 1
 * or xmlKeepBlanksDefault(0) was called
 */
void
xmlNodeDumpOutput(xmlOutputBufferPtr buf, xmlDocPtr doc, xmlNodePtr cur,
                  int level, int format, const char *encoding)
{
#ifdef LIBXML_HTML_ENABLED
    xmlDtdPtr dtd;
    int is_xhtml = 0;

    dtd = xmlGetIntSubset(doc);
    if (dtd != NULL) {
        is_xhtml = xmlIsXHTML(dtd->SystemID, dtd->ExternalID);
        if (is_xhtml < 0)
            is_xhtml = 0;
        if ((is_xhtml) && (cur->parent == (xmlNodePtr) doc) &&
            (cur->type == XML_ELEMENT_NODE) &&
            (xmlStrEqual(cur->name, BAD_CAST "html"))) {
            if (encoding != NULL)
                htmlSetMetaEncoding((htmlDocPtr) cur,
                                    (const xmlChar *) encoding);
            else
                htmlSetMetaEncoding((htmlDocPtr) cur, BAD_CAST "UTF-8");
        }
    }

    if (is_xhtml)
        xhtmlNodeDumpOutput(buf, doc, cur, level, format, encoding);
    else
#endif
        xmlNodeDumpOutputInternal(buf, doc, cur, level, format, encoding);
}

/**
 * xmlDocContentDumpOutput:
 * @buf:  the XML buffer output
 * @cur:  the document
 * @encoding:  an optional encoding string
 * @format:  should formatting spaces been added
 *
 * Dump an XML document.
 * Note that format = 1 provide node indenting only if xmlIndentTreeOutput = 1
 * or xmlKeepBlanksDefault(0) was called
 */
static void
xmlDocContentDumpOutput(xmlOutputBufferPtr buf, xmlDocPtr cur,
	                const char *encoding, int format) {
#ifdef LIBXML_HTML_ENABLED
    xmlDtdPtr dtd;
    int is_xhtml = 0;
#endif

    xmlOutputBufferWriteString(buf, "<?xml version=");
    if (cur->version != NULL) 
	xmlBufferWriteQuotedString(buf->buffer, cur->version);
    else
	xmlOutputBufferWriteString(buf, "\"1.0\"");
    if (encoding == NULL) {
	if (cur->encoding != NULL)
	    encoding = (const char *) cur->encoding;
	else if (cur->charset != XML_CHAR_ENCODING_UTF8)
	    encoding = xmlGetCharEncodingName((xmlCharEncoding) cur->charset);
    }
    if (encoding != NULL) {
        xmlOutputBufferWriteString(buf, " encoding=");
	xmlBufferWriteQuotedString(buf->buffer, (xmlChar *) encoding);
    }
    switch (cur->standalone) {
        case 0:
	    xmlOutputBufferWriteString(buf, " standalone=\"no\"");
	    break;
        case 1:
	    xmlOutputBufferWriteString(buf, " standalone=\"yes\"");
	    break;
    }
    xmlOutputBufferWriteString(buf, "?>\n");

#ifdef LIBXML_HTML_ENABLED
    dtd = xmlGetIntSubset(cur);
    if (dtd != NULL) {
	is_xhtml = xmlIsXHTML(dtd->SystemID, dtd->ExternalID);
	if (is_xhtml < 0) is_xhtml = 0;
    }
    if (is_xhtml) {
	if (encoding != NULL)
	    htmlSetMetaEncoding(cur, (const xmlChar *) encoding);
	else
	    htmlSetMetaEncoding(cur, BAD_CAST "UTF-8");
    }
#endif
    if (cur->children != NULL) {
        xmlNodePtr child = cur->children;

	while (child != NULL) {
#ifdef LIBXML_HTML_ENABLED
	    if (is_xhtml)
		xhtmlNodeDumpOutput(buf, cur, child, 0, format, encoding);
	    else
#endif
		xmlNodeDumpOutputInternal(buf, cur, child, 0, format, encoding);
	    xmlOutputBufferWriteString(buf, "\n");
	    child = child->next;
	}
    }
}

#ifdef LIBXML_HTML_ENABLED
/************************************************************************
 *									*
 *		Functions specific to XHTML serialization		*
 *									*
 ************************************************************************/

#define XHTML_STRICT_PUBLIC_ID BAD_CAST \
   "-//W3C//DTD XHTML 1.0 Strict//EN"
#define XHTML_STRICT_SYSTEM_ID BAD_CAST \
   "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"
#define XHTML_FRAME_PUBLIC_ID BAD_CAST \
   "-//W3C//DTD XHTML 1.0 Frameset//EN"
#define XHTML_FRAME_SYSTEM_ID BAD_CAST \
   "http://www.w3.org/TR/xhtml1/DTD/xhtml1-frameset.dtd"
#define XHTML_TRANS_PUBLIC_ID BAD_CAST \
   "-//W3C//DTD XHTML 1.0 Transitional//EN"
#define XHTML_TRANS_SYSTEM_ID BAD_CAST \
   "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"

#define XHTML_NS_NAME BAD_CAST "http://www.w3.org/1999/xhtml"
/**
 * xmlIsXHTML:
 * @systemID:  the system identifier
 * @publicID:  the public identifier
 *
 * Try to find if the document correspond to an XHTML DTD
 *
 * Returns 1 if true, 0 if not and -1 in case of error
 */
int
xmlIsXHTML(const xmlChar *systemID, const xmlChar *publicID) {
    if ((systemID == NULL) && (publicID == NULL))
	return(-1);
    if (publicID != NULL) {
	if (xmlStrEqual(publicID, XHTML_STRICT_PUBLIC_ID)) return(1);
	if (xmlStrEqual(publicID, XHTML_FRAME_PUBLIC_ID)) return(1);
	if (xmlStrEqual(publicID, XHTML_TRANS_PUBLIC_ID)) return(1);
    }
    if (systemID != NULL) {
	if (xmlStrEqual(systemID, XHTML_STRICT_SYSTEM_ID)) return(1);
	if (xmlStrEqual(systemID, XHTML_FRAME_SYSTEM_ID)) return(1);
	if (xmlStrEqual(systemID, XHTML_TRANS_SYSTEM_ID)) return(1);
    }
    return(0);
}

/**
 * xhtmlIsEmpty:
 * @node:  the node
 *
 * Check if a node is an empty xhtml node
 *
 * Returns 1 if the node is an empty node, 0 if not and -1 in case of error
 */
static int
xhtmlIsEmpty(xmlNodePtr node) {
    if (node == NULL)
	return(-1);
    if (node->type != XML_ELEMENT_NODE)
	return(0);
    if ((node->ns != NULL) && (!xmlStrEqual(node->ns->href, XHTML_NS_NAME)))
	return(0);
    if (node->children != NULL)
	return(0);
    switch (node->name[0]) {
	case 'a':
	    if (xmlStrEqual(node->name, BAD_CAST "area"))
		return(1);
	    return(0);
	case 'b':
	    if (xmlStrEqual(node->name, BAD_CAST "br"))
		return(1);
	    if (xmlStrEqual(node->name, BAD_CAST "base"))
		return(1);
	    if (xmlStrEqual(node->name, BAD_CAST "basefont"))
		return(1);
	    return(0);
	case 'c':
	    if (xmlStrEqual(node->name, BAD_CAST "col"))
		return(1);
	    return(0);
	case 'f':
	    if (xmlStrEqual(node->name, BAD_CAST "frame"))
		return(1);
	    return(0);
	case 'h':
	    if (xmlStrEqual(node->name, BAD_CAST "hr"))
		return(1);
	    return(0);
	case 'i':
	    if (xmlStrEqual(node->name, BAD_CAST "img"))
		return(1);
	    if (xmlStrEqual(node->name, BAD_CAST "input"))
		return(1);
	    if (xmlStrEqual(node->name, BAD_CAST "isindex"))
		return(1);
	    return(0);
	case 'l':
	    if (xmlStrEqual(node->name, BAD_CAST "link"))
		return(1);
	    return(0);
	case 'm':
	    if (xmlStrEqual(node->name, BAD_CAST "meta"))
		return(1);
	    return(0);
	case 'p':
	    if (xmlStrEqual(node->name, BAD_CAST "param"))
		return(1);
	    return(0);
    }
    return(0);
}

/**
 * xhtmlAttrListDumpOutput:
 * @buf:  the XML buffer output
 * @doc:  the document
 * @cur:  the first attribute pointer
 * @encoding:  an optional encoding string
 *
 * Dump a list of XML attributes
 */
static void
xhtmlAttrListDumpOutput(xmlOutputBufferPtr buf, xmlDocPtr doc,
	              xmlAttrPtr cur, const char *encoding) {
    xmlAttrPtr xml_lang = NULL;
    xmlAttrPtr lang = NULL;
    xmlAttrPtr name = NULL;
    xmlAttrPtr id = NULL;

    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlAttrListDumpOutput : property == NULL\n");
#endif
	return;
    }
    while (cur != NULL) {
	if ((cur->ns == NULL) && (xmlStrEqual(cur->name, BAD_CAST "id")))
	    id = cur;
	else
	if ((cur->ns == NULL) && (xmlStrEqual(cur->name, BAD_CAST "name")))
	    name = cur;
	else
	if ((cur->ns == NULL) && (xmlStrEqual(cur->name, BAD_CAST "lang")))
	    lang = cur;
	else
	if ((cur->ns != NULL) && (xmlStrEqual(cur->name, BAD_CAST "lang")) &&
	    (xmlStrEqual(cur->ns->prefix, BAD_CAST "xml")))
	    xml_lang = cur;
	else if ((cur->ns == NULL) && 
		 ((cur->children == NULL) ||
		  (cur->children->content == NULL) ||
		  (cur->children->content[0] == 0)) &&
		 (htmlIsBooleanAttr(cur->name))) {
	    if (cur->children != NULL)
		xmlFreeNode(cur->children);
	    cur->children = xmlNewText(cur->name);
	    if (cur->children != NULL)
		cur->children->parent = (xmlNodePtr) cur;
	}
        xmlAttrDumpOutput(buf, doc, cur, encoding);
	cur = cur->next;
    }
    /*
     * C.8
     */
    if ((name != NULL) && (id == NULL)) {
	xmlOutputBufferWriteString(buf, " id=\"");
	xmlAttrSerializeContent(buf->buffer, doc, name);
	xmlOutputBufferWriteString(buf, "\"");
    }
    /*
     * C.7.
     */
    if ((lang != NULL) && (xml_lang == NULL)) {
	xmlOutputBufferWriteString(buf, " xml:lang=\"");
	xmlAttrSerializeContent(buf->buffer, doc, lang);
	xmlOutputBufferWriteString(buf, "\"");
    } else 
    if ((xml_lang != NULL) && (lang == NULL)) {
	xmlOutputBufferWriteString(buf, " lang=\"");
	xmlAttrSerializeContent(buf->buffer, doc, xml_lang);
	xmlOutputBufferWriteString(buf, "\"");
    }
}

/**
 * xhtmlNodeListDumpOutput:
 * @buf:  the XML buffer output
 * @doc:  the XHTML document
 * @cur:  the first node
 * @level: the imbrication level for indenting
 * @format: is formatting allowed
 * @encoding:  an optional encoding string
 *
 * Dump an XML node list, recursive behaviour, children are printed too.
 * Note that format = 1 provide node indenting only if xmlIndentTreeOutput = 1
 * or xmlKeepBlanksDefault(0) was called
 */
static void
xhtmlNodeListDumpOutput(xmlOutputBufferPtr buf, xmlDocPtr doc,
                xmlNodePtr cur, int level, int format, const char *encoding) {
    int i;

    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xhtmlNodeListDumpOutput : node == NULL\n");
#endif
	return;
    }
    while (cur != NULL) {
	if ((format) && (xmlIndentTreeOutput) &&
	    (cur->type == XML_ELEMENT_NODE))
	    for (i = 0;i < level;i++)
		xmlOutputBufferWriteString(buf, xmlTreeIndentString);
        xhtmlNodeDumpOutput(buf, doc, cur, level, format, encoding);
	if (format) {
	    xmlOutputBufferWriteString(buf, "\n");
	}
	cur = cur->next;
    }
}

/**
 * xhtmlNodeDumpOutput:
 * @buf:  the XML buffer output
 * @doc:  the XHTML document
 * @cur:  the current node
 * @level: the imbrication level for indenting
 * @format: is formatting allowed
 * @encoding:  an optional encoding string
 *
 * Dump an XHTML node, recursive behaviour, children are printed too.
 * Note that format = 1 provide node indenting only if xmlIndentTreeOutput = 1
 * or xmlKeepBlanksDefault(0) was called
 */
static void
xhtmlNodeDumpOutput(xmlOutputBufferPtr buf, xmlDocPtr doc, xmlNodePtr cur,
            int level, int format, const char *encoding) {
    int i;
    xmlNodePtr tmp;

    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlNodeDumpOutput : node == NULL\n");
#endif
	return;
    }
    if (cur->type == XML_XINCLUDE_START)
	return;
    if (cur->type == XML_XINCLUDE_END)
	return;
    if (cur->type == XML_DTD_NODE) {
        xmlDtdDumpOutput(buf, (xmlDtdPtr) cur, encoding);
	return;
    }
    if (cur->type == XML_ELEMENT_DECL) {
        xmlDumpElementDecl(buf->buffer, (xmlElementPtr) cur);
	return;
    }
    if (cur->type == XML_ATTRIBUTE_DECL) {
        xmlDumpAttributeDecl(buf->buffer, (xmlAttributePtr) cur);
	return;
    }
    if (cur->type == XML_ENTITY_DECL) {
        xmlDumpEntityDecl(buf->buffer, (xmlEntityPtr) cur);
	return;
    }
    if (cur->type == XML_TEXT_NODE) {
	if (cur->content != NULL) {
	    if ((cur->name == xmlStringText) ||
		(cur->name != xmlStringTextNoenc)) {
		xmlChar *buffer;

		if (encoding == NULL)
		    buffer = xmlEncodeEntitiesReentrant(doc, cur->content);
		else
		    buffer = xmlEncodeSpecialChars(doc, cur->content);
		if (buffer != NULL) {
		    xmlOutputBufferWriteString(buf, (const char *)buffer);
		    xmlFree(buffer);
		}
	    } else {
		/*
		 * Disable escaping, needed for XSLT
		 */
		xmlOutputBufferWriteString(buf, (const char *) cur->content);
	    }
	}

	return;
    }
    if (cur->type == XML_PI_NODE) {
	if (cur->content != NULL) {
	    xmlOutputBufferWriteString(buf, "<?");
	    xmlOutputBufferWriteString(buf, (const char *)cur->name);
	    if (cur->content != NULL) {
		xmlOutputBufferWriteString(buf, " ");
		xmlOutputBufferWriteString(buf, (const char *)cur->content);
	    }
	    xmlOutputBufferWriteString(buf, "?>");
	} else {
	    xmlOutputBufferWriteString(buf, "<?");
	    xmlOutputBufferWriteString(buf, (const char *)cur->name);
	    xmlOutputBufferWriteString(buf, "?>");
	}
	return;
    }
    if (cur->type == XML_COMMENT_NODE) {
	if (cur->content != NULL) {
	    xmlOutputBufferWriteString(buf, "<!--");
	    xmlOutputBufferWriteString(buf, (const char *)cur->content);
	    xmlOutputBufferWriteString(buf, "-->");
	}
	return;
    }
    if (cur->type == XML_ENTITY_REF_NODE) {
        xmlOutputBufferWriteString(buf, "&");
	xmlOutputBufferWriteString(buf, (const char *)cur->name);
        xmlOutputBufferWriteString(buf, ";");
	return;
    }
    if (cur->type == XML_CDATA_SECTION_NODE) {
        xmlOutputBufferWriteString(buf, "<![CDATA[");
	if (cur->content != NULL)
	    xmlOutputBufferWriteString(buf, (const char *)cur->content);
        xmlOutputBufferWriteString(buf, "]]>");
	return;
    }

    if (format == 1) {
	tmp = cur->children;
	while (tmp != NULL) {
	    if ((tmp->type == XML_TEXT_NODE) || 
		(tmp->type == XML_ENTITY_REF_NODE)) {
		format = 0;
		break;
	    }
	    tmp = tmp->next;
	}
    }
    xmlOutputBufferWriteString(buf, "<");
    if ((cur->ns != NULL) && (cur->ns->prefix != NULL)) {
        xmlOutputBufferWriteString(buf, (const char *)cur->ns->prefix);
	xmlOutputBufferWriteString(buf, ":");
    }

    xmlOutputBufferWriteString(buf, (const char *)cur->name);
    if (cur->nsDef)
        xmlNsListDumpOutput(buf, cur->nsDef);
    if ((xmlStrEqual(cur->name, BAD_CAST "html") &&
	(cur->ns == NULL) && (cur->nsDef == NULL))) {
	/*
	 * 3.1.1. Strictly Conforming Documents A.3.1.1 3/
	 */
	xmlOutputBufferWriteString(buf,
		" xmlns=\"http://www.w3.org/1999/xhtml\"");
    }
    if (cur->properties != NULL)
        xhtmlAttrListDumpOutput(buf, doc, cur->properties, encoding);

    if ((cur->type == XML_ELEMENT_NODE) && (cur->children == NULL)) {
	if (((cur->ns == NULL) || (cur->ns->prefix == NULL)) &&
	    (xhtmlIsEmpty(cur) == 1)) {
	    /*
	     * C.2. Empty Elements
	     */
	    xmlOutputBufferWriteString(buf, " />");
	} else {
	    /*
	     * C.3. Element Minimization and Empty Element Content
	     */
	    xmlOutputBufferWriteString(buf, "></");
	    if ((cur->ns != NULL) && (cur->ns->prefix != NULL)) {
		xmlOutputBufferWriteString(buf, (const char *)cur->ns->prefix);
		xmlOutputBufferWriteString(buf, ":");
	    }
	    xmlOutputBufferWriteString(buf, (const char *)cur->name);
	    xmlOutputBufferWriteString(buf, ">");
	}
	return;
    }
    xmlOutputBufferWriteString(buf, ">");
    if ((cur->type != XML_ELEMENT_NODE) && (cur->content != NULL)) {
	xmlChar *buffer;

	if (encoding == NULL)
	    buffer = xmlEncodeEntitiesReentrant(doc, cur->content);
	else
	    buffer = xmlEncodeSpecialChars(doc, cur->content);
	if (buffer != NULL) {
	    xmlOutputBufferWriteString(buf, (const char *)buffer);
	    xmlFree(buffer);
	}
    }

    /*
     * 4.8. Script and Style elements
     */
    if ((cur->type == XML_ELEMENT_NODE) &&
	((xmlStrEqual(cur->name, BAD_CAST "script")) ||
	 (xmlStrEqual(cur->name, BAD_CAST "style"))) &&
	((cur->ns == NULL) ||
	 (xmlStrEqual(cur->ns->href, XHTML_NS_NAME)))) {
	xmlNodePtr child = cur->children;

	while (child != NULL) {
	    if ((child->type == XML_TEXT_NODE) ||
		(child->type == XML_CDATA_SECTION_NODE)) {
		/*
		 * Apparently CDATA escaping for style just break on IE,
		 * mozilla and galeon, so ...
		 */
		if (xmlStrEqual(cur->name, BAD_CAST "style") &&
		    (xmlStrchr(child->content, '<') == NULL) &&
		    (xmlStrchr(child->content, '>') == NULL) &&
		    (xmlStrchr(child->content, '&') == NULL)) {
		    xhtmlNodeDumpOutput(buf, doc, child, 0, 0, encoding);
		} else {
		    xmlOutputBufferWriteString(buf, "<![CDATA[");
		    if (child->content != NULL)
			xmlOutputBufferWriteString(buf,
				(const char *)child->content);
		    xmlOutputBufferWriteString(buf, "]]>");
		}
	    } else {
		xhtmlNodeDumpOutput(buf, doc, child, 0, 0, encoding);
	    }
	    child = child->next;
	}
    } else if (cur->children != NULL) {
	if (format) xmlOutputBufferWriteString(buf, "\n");
	xhtmlNodeListDumpOutput(buf, doc, cur->children,
		        (level >= 0?level+1:-1), format, encoding);
	if ((xmlIndentTreeOutput) && (format))
	    for (i = 0;i < level;i++)
		xmlOutputBufferWriteString(buf, xmlTreeIndentString);
    }
    xmlOutputBufferWriteString(buf, "</");
    if ((cur->ns != NULL) && (cur->ns->prefix != NULL)) {
        xmlOutputBufferWriteString(buf, (const char *)cur->ns->prefix);
	xmlOutputBufferWriteString(buf, ":");
    }

    xmlOutputBufferWriteString(buf, (const char *)cur->name);
    xmlOutputBufferWriteString(buf, ">");
}
#endif

/************************************************************************
 *									*
 *		Saving functions front-ends				*
 *									*
 ************************************************************************/

/**
 * xmlDocDumpFormatMemoryEnc:
 * @out_doc:  Document to generate XML text from
 * @doc_txt_ptr:  Memory pointer for allocated XML text
 * @doc_txt_len:  Length of the generated XML text
 * @txt_encoding:  Character encoding to use when generating XML text
 * @format:  should formatting spaces been added
 *
 * Dump the current DOM tree into memory using the character encoding specified
 * by the caller.  Note it is up to the caller of this function to free the
 * allocated memory with xmlFree().
 * Note that format = 1 provide node indenting only if xmlIndentTreeOutput = 1
 * or xmlKeepBlanksDefault(0) was called
 */

void
xmlDocDumpFormatMemoryEnc(xmlDocPtr out_doc, xmlChar **doc_txt_ptr,
		int * doc_txt_len, const char * txt_encoding,
		int format) {
    int                         dummy = 0;

    xmlCharEncoding             doc_charset;
    xmlOutputBufferPtr          out_buff = NULL;
    xmlCharEncodingHandlerPtr   conv_hdlr = NULL;

    if (doc_txt_len == NULL) {
        doc_txt_len = &dummy;   /*  Continue, caller just won't get length */
    }

    if (doc_txt_ptr == NULL) {
        *doc_txt_len = 0;
        xmlGenericError(xmlGenericErrorContext,
                    "xmlDocDumpFormatMemoryEnc:  Null return buffer pointer.");
        return;
    }

    *doc_txt_ptr = NULL;
    *doc_txt_len = 0;

    if (out_doc == NULL) {
        /*  No document, no output  */
        xmlGenericError(xmlGenericErrorContext,
                "xmlDocDumpFormatMemoryEnc:  Null DOM tree document pointer.\n");
        return;
    }

    /*
     *  Validate the encoding value, if provided.
     *  This logic is copied from xmlSaveFileEnc.
     */

    if (txt_encoding == NULL)
	txt_encoding = (const char *) out_doc->encoding;
    if (txt_encoding != NULL) {
        doc_charset = xmlParseCharEncoding(txt_encoding);

        if (out_doc->charset != XML_CHAR_ENCODING_UTF8) {
            xmlGenericError(xmlGenericErrorContext,
                    "xmlDocDumpFormatMemoryEnc: Source document not in UTF8\n");
            return;

        } else if (doc_charset != XML_CHAR_ENCODING_UTF8) {
            conv_hdlr = xmlFindCharEncodingHandler(txt_encoding);
            if ( conv_hdlr == NULL ) {
                xmlGenericError(xmlGenericErrorContext,
                                "%s:  %s %s '%s'\n",
                                "xmlDocDumpFormatMemoryEnc",
                                "Failed to identify encoding handler for",
                                "character set",
                                txt_encoding);
                return;
            }
        }
    }

    if ((out_buff = xmlAllocOutputBuffer(conv_hdlr)) == NULL ) {
        xmlGenericError(xmlGenericErrorContext,
	    "xmlDocDumpFormatMemoryEnc: Failed to allocate output buffer.\n");
        return;
    }

    xmlDocContentDumpOutput(out_buff, out_doc, txt_encoding, format);
    xmlOutputBufferFlush(out_buff);
    if (out_buff->conv != NULL) {
	*doc_txt_len = out_buff->conv->use;
	*doc_txt_ptr = xmlStrndup(out_buff->conv->content, *doc_txt_len);
    } else {
	*doc_txt_len = out_buff->buffer->use;
	*doc_txt_ptr = xmlStrndup(out_buff->buffer->content, *doc_txt_len);
    }
    (void)xmlOutputBufferClose(out_buff);

    if ((*doc_txt_ptr == NULL) && (*doc_txt_len > 0)) {
        *doc_txt_len = 0;
        xmlGenericError(xmlGenericErrorContext,
                "xmlDocDumpFormatMemoryEnc:  %s\n",
                "Failed to allocate memory for document text representation.");
    }

    return;
}

/**
 * xmlDocDumpMemory:
 * @cur:  the document
 * @mem:  OUT: the memory pointer
 * @size:  OUT: the memory length
 *
 * Dump an XML document in memory and return the #xmlChar * and it's size.
 * It's up to the caller to free the memory with xmlFree().
 */
void
xmlDocDumpMemory(xmlDocPtr cur, xmlChar**mem, int *size) {
    xmlDocDumpFormatMemoryEnc(cur, mem, size, NULL, 0);
}

/**
 * xmlDocDumpFormatMemory:
 * @cur:  the document
 * @mem:  OUT: the memory pointer
 * @size:  OUT: the memory length
 * @format:  should formatting spaces been added
 *
 *
 * Dump an XML document in memory and return the #xmlChar * and it's size.
 * It's up to the caller to free the memory with xmlFree().
 * Note that format = 1 provide node indenting only if xmlIndentTreeOutput = 1
 * or xmlKeepBlanksDefault(0) was called
 */
void
xmlDocDumpFormatMemory(xmlDocPtr cur, xmlChar**mem, int *size, int format) {
    xmlDocDumpFormatMemoryEnc(cur, mem, size, NULL, format);
}

/**
 * xmlDocDumpMemoryEnc:
 * @out_doc:  Document to generate XML text from
 * @doc_txt_ptr:  Memory pointer for allocated XML text
 * @doc_txt_len:  Length of the generated XML text
 * @txt_encoding:  Character encoding to use when generating XML text
 *
 * Dump the current DOM tree into memory using the character encoding specified
 * by the caller.  Note it is up to the caller of this function to free the
 * allocated memory with xmlFree().
 */

void
xmlDocDumpMemoryEnc(xmlDocPtr out_doc, xmlChar **doc_txt_ptr,
	            int * doc_txt_len, const char * txt_encoding) {
    xmlDocDumpFormatMemoryEnc(out_doc, doc_txt_ptr, doc_txt_len,
	                      txt_encoding, 0);
}

/**
 * xmlGetDocCompressMode:
 * @doc:  the document
 *
 * get the compression ratio for a document, ZLIB based
 * Returns 0 (uncompressed) to 9 (max compression)
 */
int
xmlGetDocCompressMode (xmlDocPtr doc) {
    if (doc == NULL) return(-1);
    return(doc->compression);
}

/**
 * xmlSetDocCompressMode:
 * @doc:  the document
 * @mode:  the compression ratio
 *
 * set the compression ratio for a document, ZLIB based
 * Correct values: 0 (uncompressed) to 9 (max compression)
 */
void
xmlSetDocCompressMode (xmlDocPtr doc, int mode) {
    if (doc == NULL) return;
    if (mode < 0) doc->compression = 0;
    else if (mode > 9) doc->compression = 9;
    else doc->compression = mode;
}

/**
 * xmlGetCompressMode:
 *
 * get the default compression mode used, ZLIB based.
 * Returns 0 (uncompressed) to 9 (max compression)
 */
int
xmlGetCompressMode(void)
{
    return (xmlCompressMode);
}

/**
 * xmlSetCompressMode:
 * @mode:  the compression ratio
 *
 * set the default compression mode used, ZLIB based
 * Correct values: 0 (uncompressed) to 9 (max compression)
 */
void
xmlSetCompressMode(int mode) {
    if (mode < 0) xmlCompressMode = 0;
    else if (mode > 9) xmlCompressMode = 9;
    else xmlCompressMode = mode;
}

/**
 * xmlDocFormatDump:
 * @f:  the FILE*
 * @cur:  the document
 * @format: should formatting spaces been added
 *
 * Dump an XML document to an open FILE.
 *
 * returns: the number of bytes written or -1 in case of failure.
 */
int
xmlDocFormatDump(FILE *f, xmlDocPtr cur, int format) {
    xmlOutputBufferPtr buf;
    const char * encoding;
    xmlCharEncodingHandlerPtr handler = NULL;
    int ret;

    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlDocDump : document == NULL\n");
#endif
	return(-1);
    }
    encoding = (const char *) cur->encoding;

    if (encoding != NULL) {
	xmlCharEncoding enc;

	enc = xmlParseCharEncoding(encoding);

	if (cur->charset != XML_CHAR_ENCODING_UTF8) {
	    xmlGenericError(xmlGenericErrorContext,
		    "xmlDocDump: document not in UTF8\n");
	    return(-1);
	}
	if (enc != XML_CHAR_ENCODING_UTF8) {
	    handler = xmlFindCharEncodingHandler(encoding);
	    if (handler == NULL) {
		xmlFree((char *) cur->encoding);
		cur->encoding = NULL;
	    }
	}
    }
    buf = xmlOutputBufferCreateFile(f, handler);
    if (buf == NULL) return(-1);
    xmlDocContentDumpOutput(buf, cur, NULL, format);

    ret = xmlOutputBufferClose(buf);
    return(ret);
}

/**
 * xmlDocDump:
 * @f:  the FILE*
 * @cur:  the document
 *
 * Dump an XML document to an open FILE.
 *
 * returns: the number of bytes written or -1 in case of failure.
 */
int
xmlDocDump(FILE *f, xmlDocPtr cur) {
	return(xmlDocFormatDump (f, cur, 0));
}

/**
 * xmlSaveFileTo:
 * @buf:  an output I/O buffer
 * @cur:  the document
 * @encoding:  the encoding if any assuming the I/O layer handles the trancoding
 *
 * Dump an XML document to an I/O buffer.
 *
 * returns: the number of bytes written or -1 in case of failure.
 */
int
xmlSaveFileTo(xmlOutputBufferPtr buf, xmlDocPtr cur, const char *encoding) {
    int ret;

    if (buf == NULL) return(0);
    xmlDocContentDumpOutput(buf, cur, encoding, 0);
    ret = xmlOutputBufferClose(buf);
    return(ret);
}

/**
 * xmlSaveFormatFileTo:
 * @buf:  an output I/O buffer
 * @cur:  the document
 * @encoding:  the encoding if any assuming the I/O layer handles the trancoding
 * @format: should formatting spaces been added
 *
 * Dump an XML document to an I/O buffer.
 *
 * returns: the number of bytes written or -1 in case of failure.
 */
int
xmlSaveFormatFileTo(xmlOutputBufferPtr buf, xmlDocPtr cur, const char *encoding, int format) {
  int ret;

  if (buf == NULL) return(0);
  xmlDocContentDumpOutput(buf, cur, encoding, format);
  ret = xmlOutputBufferClose(buf);
  return(ret);
}

/**
 * xmlSaveFormatFileEnc:
 * @filename:  the filename or URL to output
 * @cur:  the document being saved
 * @encoding:  the name of the encoding to use or NULL.
 * @format:  should formatting spaces be added.
 *
 * Dump an XML document to a file or an URL.
 *
 * Returns the number of bytes written or -1 in case of error.
 */
int
xmlSaveFormatFileEnc( const char * filename, xmlDocPtr cur,
			const char * encoding, int format ) {
    xmlOutputBufferPtr buf;
    xmlCharEncodingHandlerPtr handler = NULL;
    xmlCharEncoding enc;
    int ret;

    if (encoding == NULL)
	encoding = (const char *) cur->encoding;

    if (encoding != NULL) {

	enc = xmlParseCharEncoding(encoding);
	if (cur->charset != XML_CHAR_ENCODING_UTF8) {
	    xmlGenericError(xmlGenericErrorContext,
		    "xmlSaveFormatFileEnc: document not in UTF8\n");
	    return(-1);
	}
	if (enc != XML_CHAR_ENCODING_UTF8) {
	    handler = xmlFindCharEncodingHandler(encoding);
	    if (handler == NULL)
		return(-1);
	}
    }

#ifdef HAVE_ZLIB_H
    if (cur->compression < 0) cur->compression = xmlCompressMode;
#endif
    /* 
     * save the content to a temp buffer.
     */
    buf = xmlOutputBufferCreateFilename(filename, handler, cur->compression);
    if (buf == NULL) return(-1);

    xmlDocContentDumpOutput(buf, cur, encoding, format);

    ret = xmlOutputBufferClose(buf);
    return(ret);
}


/**
 * xmlSaveFileEnc:
 * @filename:  the filename (or URL)
 * @cur:  the document
 * @encoding:  the name of an encoding (or NULL)
 *
 * Dump an XML document, converting it to the given encoding
 *
 * returns: the number of bytes written or -1 in case of failure.
 */
int
xmlSaveFileEnc(const char *filename, xmlDocPtr cur, const char *encoding) {
    return ( xmlSaveFormatFileEnc( filename, cur, encoding, 0 ) );
}

/**
 * xmlSaveFormatFile:
 * @filename:  the filename (or URL)
 * @cur:  the document
 * @format:  should formatting spaces been added
 *
 * Dump an XML document to a file. Will use compression if
 * compiled in and enabled. If @filename is "-" the stdout file is
 * used. If @format is set then the document will be indented on output.
 *
 * returns: the number of bytes written or -1 in case of failure.
 */
int
xmlSaveFormatFile(const char *filename, xmlDocPtr cur, int format) {
    return ( xmlSaveFormatFileEnc( filename, cur, NULL, format ) );
}

/**
 * xmlSaveFile:
 * @filename:  the filename (or URL)
 * @cur:  the document
 *
 * Dump an XML document to a file. Will use compression if
 * compiled in and enabled. If @filename is "-" the stdout file is
 * used.
 * returns: the number of bytes written or -1 in case of failure.
 */
int
xmlSaveFile(const char *filename, xmlDocPtr cur) {
    return(xmlSaveFormatFileEnc(filename, cur, NULL, 0));
}

