/*
 * debugXML.c : This is a set of routines used for debugging the tree
 *              produced by the XML parser.
 *
 * See Copyright for the status of this software.
 *
 * Daniel Veillard <daniel@veillard.com>
 */

#define IN_LIBXML
#include "libxml.h"
#ifdef LIBXML_DEBUG_ENABLED

#include <string.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#include <libxml/xmlmemory.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/valid.h>
#include <libxml/debugXML.h>
#include <libxml/HTMLtree.h>
#include <libxml/HTMLparser.h>
#include <libxml/xmlerror.h>
#include <libxml/globals.h>
#include <libxml/xpathInternals.h>

/**
 * xmlDebugDumpString:
 * @output:  the FILE * for the output
 * @str:  the string
 *
 * Dumps informations about the string, shorten it if necessary
 */
void
xmlDebugDumpString(FILE * output, const xmlChar * str)
{
    int i;

    if (output == NULL)
	output = stdout;
    if (str == NULL) {
        fprintf(output, "(NULL)");
        return;
    }
    for (i = 0; i < 40; i++)
        if (str[i] == 0)
            return;
        else if (IS_BLANK(str[i]))
            fputc(' ', output);
        else if (str[i] >= 0x80)
            fprintf(output, "#%X", str[i]);
        else
            fputc(str[i], output);
    fprintf(output, "...");
}

static void
xmlDebugDumpDtdNode(FILE *output, xmlDtdPtr dtd, int depth) {
    int i;
    char shift[100];

    for (i = 0;((i < depth) && (i < 25));i++)
        shift[2 * i] = shift[2 * i + 1] = ' ';
    shift[2 * i] = shift[2 * i + 1] = 0;

    fprintf(output, shift);

    if (dtd == NULL) {
	fprintf(output, "DTD node is NULL\n");
	return;
    }

    if (dtd->type != XML_DTD_NODE) {
	fprintf(output, "PBM: not a DTD\n");
	return;
    }
    if (dtd->name != NULL)
	fprintf(output, "DTD(%s)", dtd->name);
    else
	fprintf(output, "DTD");
    if (dtd->ExternalID != NULL)
	fprintf(output, ", PUBLIC %s", dtd->ExternalID);
    if (dtd->SystemID != NULL)
	fprintf(output, ", SYSTEM %s", dtd->SystemID);
    fprintf(output, "\n");
    /*
     * Do a bit of checking
     */
    if (dtd->parent == NULL)
	fprintf(output, "PBM: DTD has no parent\n");
    if (dtd->doc == NULL)
	fprintf(output, "PBM: DTD has no doc\n");
    if ((dtd->parent != NULL) && (dtd->doc != dtd->parent->doc))
	fprintf(output, "PBM: DTD doc differs from parent's one\n");
    if (dtd->prev == NULL) {
	if ((dtd->parent != NULL) && (dtd->parent->children != (xmlNodePtr)dtd))
	    fprintf(output, "PBM: DTD has no prev and not first of list\n");
    } else {
	if (dtd->prev->next != (xmlNodePtr) dtd)
	    fprintf(output, "PBM: DTD prev->next : back link wrong\n");
    }
    if (dtd->next == NULL) {
	if ((dtd->parent != NULL) && (dtd->parent->last != (xmlNodePtr) dtd))
	    fprintf(output, "PBM: DTD has no next and not last of list\n");
    } else {
	if (dtd->next->prev != (xmlNodePtr) dtd)
	    fprintf(output, "PBM: DTD next->prev : forward link wrong\n");
    }
}

static void
xmlDebugDumpAttrDecl(FILE *output, xmlAttributePtr attr, int depth) {
    int i;
    char shift[100];

    for (i = 0;((i < depth) && (i < 25));i++)
        shift[2 * i] = shift[2 * i + 1] = ' ';
    shift[2 * i] = shift[2 * i + 1] = 0;

    fprintf(output, shift);

    if (attr == NULL) {
	fprintf(output, "Attribute declaration is NULL\n");
	return;
    }
    if (attr->type != XML_ATTRIBUTE_DECL) {
	fprintf(output, "PBM: not a Attr\n");
	return;
    }
    if (attr->name != NULL)
	fprintf(output, "ATTRDECL(%s)", attr->name);
    else
	fprintf(output, "PBM ATTRDECL noname!!!");
    if (attr->elem != NULL)
	fprintf(output, " for %s", attr->elem);
    else
	fprintf(output, " PBM noelem!!!");
    switch (attr->atype) {
        case XML_ATTRIBUTE_CDATA:
	    fprintf(output, " CDATA");
	    break;
        case XML_ATTRIBUTE_ID:
	    fprintf(output, " ID");
	    break;
        case XML_ATTRIBUTE_IDREF:
	    fprintf(output, " IDREF");
	    break;
        case XML_ATTRIBUTE_IDREFS:
	    fprintf(output, " IDREFS");
	    break;
        case XML_ATTRIBUTE_ENTITY:
	    fprintf(output, " ENTITY");
	    break;
        case XML_ATTRIBUTE_ENTITIES:
	    fprintf(output, " ENTITIES");
	    break;
        case XML_ATTRIBUTE_NMTOKEN:
	    fprintf(output, " NMTOKEN");
	    break;
        case XML_ATTRIBUTE_NMTOKENS:
	    fprintf(output, " NMTOKENS");
	    break;
        case XML_ATTRIBUTE_ENUMERATION:
	    fprintf(output, " ENUMERATION");
	    break;
        case XML_ATTRIBUTE_NOTATION:
	    fprintf(output, " NOTATION ");
	    break;
    }
    if (attr->tree != NULL) {
	int indx;
	xmlEnumerationPtr cur = attr->tree;

	for (indx = 0;indx < 5; indx++) {
	    if (indx != 0)
		fprintf(output, "|%s", cur->name);
	    else
		fprintf(output, " (%s", cur->name);
	    cur = cur->next;
	    if (cur == NULL) break;
	}
	if (cur == NULL)
	    fprintf(output, ")");
	else
	    fprintf(output, "...)");
    }
    switch (attr->def) {
        case XML_ATTRIBUTE_NONE:
	    break;
        case XML_ATTRIBUTE_REQUIRED:
	    fprintf(output, " REQUIRED");
	    break;
        case XML_ATTRIBUTE_IMPLIED:
	    fprintf(output, " IMPLIED");
	    break;
        case XML_ATTRIBUTE_FIXED:
	    fprintf(output, " FIXED");
	    break;
    }
    if (attr->defaultValue != NULL) {
	fprintf(output, "\"");
	xmlDebugDumpString(output, attr->defaultValue);
	fprintf(output, "\"");
    }
    fprintf(output, "\n");

    /*
     * Do a bit of checking
     */
    if (attr->parent == NULL)
	fprintf(output, "PBM: Attr has no parent\n");
    if (attr->doc == NULL)
	fprintf(output, "PBM: Attr has no doc\n");
    if ((attr->parent != NULL) && (attr->doc != attr->parent->doc))
	fprintf(output, "PBM: Attr doc differs from parent's one\n");
    if (attr->prev == NULL) {
	if ((attr->parent != NULL) && (attr->parent->children != (xmlNodePtr)attr))
	    fprintf(output, "PBM: Attr has no prev and not first of list\n");
    } else {
	if (attr->prev->next != (xmlNodePtr) attr)
	    fprintf(output, "PBM: Attr prev->next : back link wrong\n");
    }
    if (attr->next == NULL) {
	if ((attr->parent != NULL) && (attr->parent->last != (xmlNodePtr) attr))
	    fprintf(output, "PBM: Attr has no next and not last of list\n");
    } else {
	if (attr->next->prev != (xmlNodePtr) attr)
	    fprintf(output, "PBM: Attr next->prev : forward link wrong\n");
    }
}

static void
xmlDebugDumpElemDecl(FILE *output, xmlElementPtr elem, int depth) {
    int i;
    char shift[100];

    for (i = 0;((i < depth) && (i < 25));i++)
        shift[2 * i] = shift[2 * i + 1] = ' ';
    shift[2 * i] = shift[2 * i + 1] = 0;

    fprintf(output, shift);

    if (elem == NULL) {
	fprintf(output, "Element declaration is NULL\n");
	return;
    }
    if (elem->type != XML_ELEMENT_DECL) {
	fprintf(output, "PBM: not a Elem\n");
	return;
    }
    if (elem->name != NULL) {
	fprintf(output, "ELEMDECL(");
	xmlDebugDumpString(output, elem->name);
	fprintf(output, ")");
    } else
	fprintf(output, "PBM ELEMDECL noname!!!");
    switch (elem->etype) {
	case XML_ELEMENT_TYPE_UNDEFINED: 
	    fprintf(output, ", UNDEFINED");
	    break;
	case XML_ELEMENT_TYPE_EMPTY: 
	    fprintf(output, ", EMPTY");
	    break;
	case XML_ELEMENT_TYPE_ANY: 
	    fprintf(output, ", ANY");
	    break;
	case XML_ELEMENT_TYPE_MIXED: 
	    fprintf(output, ", MIXED ");
	    break;
	case XML_ELEMENT_TYPE_ELEMENT: 
	    fprintf(output, ", MIXED ");
	    break;
    }
    if ((elem->type != XML_ELEMENT_NODE) &&
	(elem->content != NULL)) {
	char buf[5001];

	buf[0] = 0;
	xmlSnprintfElementContent(buf, 5000, elem->content, 1);
	buf[5000] = 0;
	fprintf(output, "%s", buf);
    }
    fprintf(output, "\n");

    /*
     * Do a bit of checking
     */
    if (elem->parent == NULL)
	fprintf(output, "PBM: Elem has no parent\n");
    if (elem->doc == NULL)
	fprintf(output, "PBM: Elem has no doc\n");
    if ((elem->parent != NULL) && (elem->doc != elem->parent->doc))
	fprintf(output, "PBM: Elem doc differs from parent's one\n");
    if (elem->prev == NULL) {
	if ((elem->parent != NULL) && (elem->parent->children != (xmlNodePtr)elem))
	    fprintf(output, "PBM: Elem has no prev and not first of list\n");
    } else {
	if (elem->prev->next != (xmlNodePtr) elem)
	    fprintf(output, "PBM: Elem prev->next : back link wrong\n");
    }
    if (elem->next == NULL) {
	if ((elem->parent != NULL) && (elem->parent->last != (xmlNodePtr) elem))
	    fprintf(output, "PBM: Elem has no next and not last of list\n");
    } else {
	if (elem->next->prev != (xmlNodePtr) elem)
	    fprintf(output, "PBM: Elem next->prev : forward link wrong\n");
    }
}

static void
xmlDebugDumpEntityDecl(FILE *output, xmlEntityPtr ent, int depth) {
    int i;
    char shift[100];

    for (i = 0;((i < depth) && (i < 25));i++)
        shift[2 * i] = shift[2 * i + 1] = ' ';
    shift[2 * i] = shift[2 * i + 1] = 0;

    fprintf(output, shift);

    if (ent == NULL) {
	fprintf(output, "Entity declaration is NULL\n");
	return;
    }
    if (ent->type != XML_ENTITY_DECL) {
	fprintf(output, "PBM: not a Entity decl\n");
	return;
    }
    if (ent->name != NULL) {
	fprintf(output, "ENTITYDECL(");
	xmlDebugDumpString(output, ent->name);
	fprintf(output, ")");
    } else
	fprintf(output, "PBM ENTITYDECL noname!!!");
    switch (ent->etype) {
	case XML_INTERNAL_GENERAL_ENTITY: 
	    fprintf(output, ", internal\n");
	    break;
	case XML_EXTERNAL_GENERAL_PARSED_ENTITY: 
	    fprintf(output, ", external parsed\n");
	    break;
	case XML_EXTERNAL_GENERAL_UNPARSED_ENTITY: 
	    fprintf(output, ", unparsed\n");
	    break;
	case XML_INTERNAL_PARAMETER_ENTITY: 
	    fprintf(output, ", parameter\n");
	    break;
	case XML_EXTERNAL_PARAMETER_ENTITY: 
	    fprintf(output, ", external parameter\n");
	    break;
	case XML_INTERNAL_PREDEFINED_ENTITY: 
	    fprintf(output, ", predefined\n");
	    break;
    }
    if (ent->ExternalID) {
        fprintf(output, shift);
        fprintf(output, " ExternalID=%s\n", ent->ExternalID);
    }
    if (ent->SystemID) {
        fprintf(output, shift);
        fprintf(output, " SystemID=%s\n", ent->SystemID);
    }
    if (ent->URI != NULL) {
        fprintf(output, shift);
        fprintf(output, " URI=%s\n", ent->URI);
    }
    if (ent->content) {
        fprintf(output, shift);
	fprintf(output, " content=");
	xmlDebugDumpString(output, ent->content);
	fprintf(output, "\n");
    }

    /*
     * Do a bit of checking
     */
    if (ent->parent == NULL)
	fprintf(output, "PBM: Ent has no parent\n");
    if (ent->doc == NULL)
	fprintf(output, "PBM: Ent has no doc\n");
    if ((ent->parent != NULL) && (ent->doc != ent->parent->doc))
	fprintf(output, "PBM: Ent doc differs from parent's one\n");
    if (ent->prev == NULL) {
	if ((ent->parent != NULL) && (ent->parent->children != (xmlNodePtr)ent))
	    fprintf(output, "PBM: Ent has no prev and not first of list\n");
    } else {
	if (ent->prev->next != (xmlNodePtr) ent)
	    fprintf(output, "PBM: Ent prev->next : back link wrong\n");
    }
    if (ent->next == NULL) {
	if ((ent->parent != NULL) && (ent->parent->last != (xmlNodePtr) ent))
	    fprintf(output, "PBM: Ent has no next and not last of list\n");
    } else {
	if (ent->next->prev != (xmlNodePtr) ent)
	    fprintf(output, "PBM: Ent next->prev : forward link wrong\n");
    }
}

static void
xmlDebugDumpNamespace(FILE *output, xmlNsPtr ns, int depth) {
    int i;
    char shift[100];

    for (i = 0;((i < depth) && (i < 25));i++)
        shift[2 * i] = shift[2 * i + 1] = ' ';
    shift[2 * i] = shift[2 * i + 1] = 0;

    fprintf(output, shift);

    if (ns == NULL) {
	fprintf(output, "namespace node is NULL\n");
	return;
    }
    if (ns->type != XML_NAMESPACE_DECL) {
        fprintf(output, "invalid namespace node %d\n", ns->type);
	return;
    }
    if (ns->href == NULL) {
	if (ns->prefix != NULL)
	    fprintf(output, "incomplete namespace %s href=NULL\n", ns->prefix);
	else
	    fprintf(output, "incomplete default namespace href=NULL\n");
    } else {
	if (ns->prefix != NULL)
	    fprintf(output, "namespace %s href=", ns->prefix);
	else
	    fprintf(output, "default namespace href=");

	xmlDebugDumpString(output, ns->href);
	fprintf(output, "\n");
    }
}

static void
xmlDebugDumpNamespaceList(FILE *output, xmlNsPtr ns, int depth) {
    while (ns != NULL) {
        xmlDebugDumpNamespace(output, ns, depth);
	ns = ns->next;
    }
}

static void
xmlDebugDumpEntity(FILE *output, xmlEntityPtr ent, int depth) {
    int i;
    char shift[100];

    for (i = 0;((i < depth) && (i < 25));i++)
        shift[2 * i] = shift[2 * i + 1] = ' ';
    shift[2 * i] = shift[2 * i + 1] = 0;

    fprintf(output, shift);
    
    if (ent == NULL) {
	fprintf(output, "Entity is NULL\n");
	return;
    }
    switch (ent->etype) {
        case XML_INTERNAL_GENERAL_ENTITY:
	    fprintf(output, "INTERNAL_GENERAL_ENTITY ");
	    break;
        case XML_EXTERNAL_GENERAL_PARSED_ENTITY:
	    fprintf(output, "EXTERNAL_GENERAL_PARSED_ENTITY ");
	    break;
        case XML_EXTERNAL_GENERAL_UNPARSED_ENTITY:
	    fprintf(output, "EXTERNAL_GENERAL_UNPARSED_ENTITY ");
	    break;
        case XML_INTERNAL_PARAMETER_ENTITY:
	    fprintf(output, "INTERNAL_PARAMETER_ENTITY ");
	    break;
        case XML_EXTERNAL_PARAMETER_ENTITY:
	    fprintf(output, "EXTERNAL_PARAMETER_ENTITY ");
	    break;
	default:
	    fprintf(output, "ENTITY_%d ! ", ent->etype);
    }
    fprintf(output, "%s\n", ent->name);
    if (ent->ExternalID) {
        fprintf(output, shift);
        fprintf(output, "ExternalID=%s\n", ent->ExternalID);
    }
    if (ent->SystemID) {
        fprintf(output, shift);
        fprintf(output, "SystemID=%s\n", ent->SystemID);
    }
    if (ent->URI) {
        fprintf(output, shift);
        fprintf(output, "URI=%s\n", ent->URI);
    }
    if (ent->content) {
        fprintf(output, shift);
	fprintf(output, "content=");
	xmlDebugDumpString(output, ent->content);
	fprintf(output, "\n");
    }
}

/**
 * xmlDebugDumpAttr:
 * @output:  the FILE * for the output
 * @attr:  the attribute
 * @depth:  the indentation level.
 *
 * Dumps debug information for the attribute
 */
void
xmlDebugDumpAttr(FILE *output, xmlAttrPtr attr, int depth) {
    int i;
    char shift[100];

    for (i = 0;((i < depth) && (i < 25));i++)
        shift[2 * i] = shift[2 * i + 1] = ' ';
    shift[2 * i] = shift[2 * i + 1] = 0;

    fprintf(output, shift);
    
    if (attr == NULL) {
	fprintf(output, "Attr is NULL");
	return;
    }
    fprintf(output, "ATTRIBUTE ");
    xmlDebugDumpString(output, attr->name);
    fprintf(output, "\n");
    if (attr->children != NULL) 
        xmlDebugDumpNodeList(output, attr->children, depth + 1);

    /*
     * Do a bit of checking
     */
    if (attr->parent == NULL)
	fprintf(output, "PBM: Attr has no parent\n");
    if (attr->doc == NULL)
	fprintf(output, "PBM: Attr has no doc\n");
    if ((attr->parent != NULL) && (attr->doc != attr->parent->doc))
	fprintf(output, "PBM: Attr doc differs from parent's one\n");
    if (attr->prev == NULL) {
	if ((attr->parent != NULL) && (attr->parent->properties != attr))
	    fprintf(output, "PBM: Attr has no prev and not first of list\n");
    } else {
	if (attr->prev->next != attr)
	    fprintf(output, "PBM: Attr prev->next : back link wrong\n");
    }
    if (attr->next != NULL) {
	if (attr->next->prev != attr)
	    fprintf(output, "PBM: Attr next->prev : forward link wrong\n");
    }
}

/**
 * xmlDebugDumpAttrList:
 * @output:  the FILE * for the output
 * @attr:  the attribute list
 * @depth:  the indentation level.
 *
 * Dumps debug information for the attribute list
 */
void
xmlDebugDumpAttrList(FILE * output, xmlAttrPtr attr, int depth)
{
    if (output == NULL)
	output = stdout;
    while (attr != NULL) {
        xmlDebugDumpAttr(output, attr, depth);
        attr = attr->next;
    }
}

/**
 * xmlDebugDumpOneNode:
 * @output:  the FILE * for the output
 * @node:  the node
 * @depth:  the indentation level.
 *
 * Dumps debug information for the element node, it is not recursive
 */
void
xmlDebugDumpOneNode(FILE * output, xmlNodePtr node, int depth)
{
    int i;
    char shift[100];

    if (output == NULL)
	output = stdout;
    for (i = 0; ((i < depth) && (i < 25)); i++)
        shift[2 * i] = shift[2 * i + 1] = ' ';
    shift[2 * i] = shift[2 * i + 1] = 0;

    if (node == NULL) {
	fprintf(output, shift);
	fprintf(output, "node is NULL\n");
	return;
    }
    switch (node->type) {
        case XML_ELEMENT_NODE:
            fprintf(output, shift);
            fprintf(output, "ELEMENT ");
            if ((node->ns != NULL) && (node->ns->prefix != NULL)) {
                xmlDebugDumpString(output, node->ns->prefix);
                fprintf(output, ":");
            }
            xmlDebugDumpString(output, node->name);
            fprintf(output, "\n");
            break;
        case XML_ATTRIBUTE_NODE:
            fprintf(output, shift);
            fprintf(output, "Error, ATTRIBUTE found here\n");
            break;
        case XML_TEXT_NODE:
            fprintf(output, shift);
	    if (node->name == (const xmlChar *) xmlStringTextNoenc)
		fprintf(output, "TEXT no enc\n");
	    else
		fprintf(output, "TEXT\n");
            break;
        case XML_CDATA_SECTION_NODE:
            fprintf(output, shift);
            fprintf(output, "CDATA_SECTION\n");
            break;
        case XML_ENTITY_REF_NODE:
            fprintf(output, shift);
            fprintf(output, "ENTITY_REF(%s)\n", node->name);
            break;
        case XML_ENTITY_NODE:
            fprintf(output, shift);
            fprintf(output, "ENTITY\n");
            break;
        case XML_PI_NODE:
            fprintf(output, shift);
            fprintf(output, "PI %s\n", node->name);
            break;
        case XML_COMMENT_NODE:
            fprintf(output, shift);
            fprintf(output, "COMMENT\n");
            break;
        case XML_DOCUMENT_NODE:
        case XML_HTML_DOCUMENT_NODE:
            fprintf(output, shift);
            fprintf(output, "Error, DOCUMENT found here\n");
            break;
        case XML_DOCUMENT_TYPE_NODE:
            fprintf(output, shift);
            fprintf(output, "DOCUMENT_TYPE\n");
            break;
        case XML_DOCUMENT_FRAG_NODE:
            fprintf(output, shift);
            fprintf(output, "DOCUMENT_FRAG\n");
            break;
        case XML_NOTATION_NODE:
            fprintf(output, shift);
            fprintf(output, "NOTATION\n");
            break;
        case XML_DTD_NODE:
            xmlDebugDumpDtdNode(output, (xmlDtdPtr) node, depth);
            return;
        case XML_ELEMENT_DECL:
            xmlDebugDumpElemDecl(output, (xmlElementPtr) node, depth);
            return;
        case XML_ATTRIBUTE_DECL:
            xmlDebugDumpAttrDecl(output, (xmlAttributePtr) node, depth);
            return;
        case XML_ENTITY_DECL:
            xmlDebugDumpEntityDecl(output, (xmlEntityPtr) node, depth);
            return;
        case XML_NAMESPACE_DECL:
            xmlDebugDumpNamespace(output, (xmlNsPtr) node, depth);
            return;
        case XML_XINCLUDE_START:
            fprintf(output, shift);
            fprintf(output, "INCLUDE START\n");
            return;
        case XML_XINCLUDE_END:
            fprintf(output, shift);
            fprintf(output, "INCLUDE END\n");
            return;
        default:
            fprintf(output, shift);
            fprintf(output, "NODE_%d !!!\n", node->type);
            return;
    }
    if (node->doc == NULL) {
        fprintf(output, shift);
        fprintf(output, "doc == NULL !!!\n");
    }
    if (node->nsDef != NULL)
        xmlDebugDumpNamespaceList(output, node->nsDef, depth + 1);
    if (node->properties != NULL)
        xmlDebugDumpAttrList(output, node->properties, depth + 1);
    if (node->type != XML_ENTITY_REF_NODE) {
        if ((node->type != XML_ELEMENT_NODE) && (node->content != NULL)) {
            shift[2 * i] = shift[2 * i + 1] = ' ';
            shift[2 * i + 2] = shift[2 * i + 3] = 0;
            fprintf(output, shift);
            fprintf(output, "content=");
            xmlDebugDumpString(output, node->content);
            fprintf(output, "\n");
        }
    } else {
        xmlEntityPtr ent;

        ent = xmlGetDocEntity(node->doc, node->name);
        if (ent != NULL)
            xmlDebugDumpEntity(output, ent, depth + 1);
    }
    /*
     * Do a bit of checking
     */
    if (node->parent == NULL)
        fprintf(output, "PBM: Node has no parent\n");
    if (node->doc == NULL)
        fprintf(output, "PBM: Node has no doc\n");
    if ((node->parent != NULL) && (node->doc != node->parent->doc))
        fprintf(output, "PBM: Node doc differs from parent's one\n");
    if (node->prev == NULL) {
        if ((node->parent != NULL) && (node->parent->children != node))
            fprintf(output,
                    "PBM: Node has no prev and not first of list\n");
    } else {
        if (node->prev->next != node)
            fprintf(output, "PBM: Node prev->next : back link wrong\n");
    }
    if (node->next == NULL) {
        if ((node->parent != NULL) && (node->parent->last != node))
            fprintf(output,
                    "PBM: Node has no next and not last of list\n");
    } else {
        if (node->next->prev != node)
            fprintf(output, "PBM: Node next->prev : forward link wrong\n");
    }
}

/**
 * xmlDebugDumpNode:
 * @output:  the FILE * for the output
 * @node:  the node
 * @depth:  the indentation level.
 *
 * Dumps debug information for the element node, it is recursive
 */
void
xmlDebugDumpNode(FILE * output, xmlNodePtr node, int depth)
{
    if (output == NULL)
	output = stdout;
    if (node == NULL) {
	int i;
	char shift[100];
    	
	for (i = 0; ((i < depth) && (i < 25)); i++)
	    shift[2 * i] = shift[2 * i + 1] = ' ';
	shift[2 * i] = shift[2 * i + 1] = 0;
	
	fprintf(output, shift);
	fprintf(output, "node is NULL\n");
	return;
    }	
    xmlDebugDumpOneNode(output, node, depth);
    if ((node->children != NULL) && (node->type != XML_ENTITY_REF_NODE))
        xmlDebugDumpNodeList(output, node->children, depth + 1);
}

/**
 * xmlDebugDumpNodeList:
 * @output:  the FILE * for the output
 * @node:  the node list
 * @depth:  the indentation level.
 *
 * Dumps debug information for the list of element node, it is recursive
 */
void
xmlDebugDumpNodeList(FILE * output, xmlNodePtr node, int depth)
{
    if (output == NULL)
	output = stdout;
    while (node != NULL) {
        xmlDebugDumpNode(output, node, depth);
        node = node->next;
    }
}


/**
 * xmlDebugDumpDocumentHead:
 * @output:  the FILE * for the output
 * @doc:  the document
 *
 * Dumps debug information cncerning the document, not recursive
 */
void
xmlDebugDumpDocumentHead(FILE * output, xmlDocPtr doc)
{
    if (output == NULL)
        output = stdout;
    if (doc == NULL) {
        fprintf(output, "DOCUMENT == NULL !\n");
        return;
    }

    switch (doc->type) {
        case XML_ELEMENT_NODE:
            fprintf(output, "Error, ELEMENT found here ");
            break;
        case XML_ATTRIBUTE_NODE:
            fprintf(output, "Error, ATTRIBUTE found here\n");
            break;
        case XML_TEXT_NODE:
            fprintf(output, "Error, TEXT\n");
            break;
        case XML_CDATA_SECTION_NODE:
            fprintf(output, "Error, CDATA_SECTION\n");
            break;
        case XML_ENTITY_REF_NODE:
            fprintf(output, "Error, ENTITY_REF\n");
            break;
        case XML_ENTITY_NODE:
            fprintf(output, "Error, ENTITY\n");
            break;
        case XML_PI_NODE:
            fprintf(output, "Error, PI\n");
            break;
        case XML_COMMENT_NODE:
            fprintf(output, "Error, COMMENT\n");
            break;
        case XML_DOCUMENT_NODE:
            fprintf(output, "DOCUMENT\n");
            break;
        case XML_HTML_DOCUMENT_NODE:
            fprintf(output, "HTML DOCUMENT\n");
            break;
        case XML_DOCUMENT_TYPE_NODE:
            fprintf(output, "Error, DOCUMENT_TYPE\n");
            break;
        case XML_DOCUMENT_FRAG_NODE:
            fprintf(output, "Error, DOCUMENT_FRAG\n");
            break;
        case XML_NOTATION_NODE:
            fprintf(output, "Error, NOTATION\n");
            break;
        default:
            fprintf(output, "NODE_%d\n", doc->type);
    }
    if (doc->name != NULL) {
        fprintf(output, "name=");
        xmlDebugDumpString(output, BAD_CAST doc->name);
        fprintf(output, "\n");
    }
    if (doc->version != NULL) {
        fprintf(output, "version=");
        xmlDebugDumpString(output, doc->version);
        fprintf(output, "\n");
    }
    if (doc->encoding != NULL) {
        fprintf(output, "encoding=");
        xmlDebugDumpString(output, doc->encoding);
        fprintf(output, "\n");
    }
    if (doc->URL != NULL) {
        fprintf(output, "URL=");
        xmlDebugDumpString(output, doc->URL);
        fprintf(output, "\n");
    }
    if (doc->standalone)
        fprintf(output, "standalone=true\n");
    if (doc->oldNs != NULL)
        xmlDebugDumpNamespaceList(output, doc->oldNs, 0);
}

/**
 * xmlDebugDumpDocument:
 * @output:  the FILE * for the output
 * @doc:  the document
 *
 * Dumps debug information for the document, it's recursive
 */
void
xmlDebugDumpDocument(FILE * output, xmlDocPtr doc)
{
    if (output == NULL)
        output = stdout;
    if (doc == NULL) {
        fprintf(output, "DOCUMENT == NULL !\n");
        return;
    }
    xmlDebugDumpDocumentHead(output, doc);
    if (((doc->type == XML_DOCUMENT_NODE) ||
         (doc->type == XML_HTML_DOCUMENT_NODE)) && (doc->children != NULL))
        xmlDebugDumpNodeList(output, doc->children, 1);
}

/**
 * xmlDebugDumpDTD:
 * @output:  the FILE * for the output
 * @dtd:  the DTD
 *
 * Dumps debug information for the DTD
 */
void
xmlDebugDumpDTD(FILE * output, xmlDtdPtr dtd)
{
    if (output == NULL)
	output = stdout;
    if (dtd == NULL) {
	fprintf(output, "DTD is NULL\n");
        return;
    }
    if (dtd->type != XML_DTD_NODE) {
        fprintf(output, "PBM: not a DTD\n");
        return;
    }
    if (dtd->name != NULL)
        fprintf(output, "DTD(%s)", dtd->name);
    else
        fprintf(output, "DTD");
    if (dtd->ExternalID != NULL)
        fprintf(output, ", PUBLIC %s", dtd->ExternalID);
    if (dtd->SystemID != NULL)
        fprintf(output, ", SYSTEM %s", dtd->SystemID);
    fprintf(output, "\n");
    /*
     * Do a bit of checking
     */
    if ((dtd->parent != NULL) && (dtd->doc != dtd->parent->doc))
        fprintf(output, "PBM: DTD doc differs from parent's one\n");
    if (dtd->prev == NULL) {
        if ((dtd->parent != NULL)
            && (dtd->parent->children != (xmlNodePtr) dtd))
            fprintf(output,
                    "PBM: DTD has no prev and not first of list\n");
    } else {
        if (dtd->prev->next != (xmlNodePtr) dtd)
            fprintf(output, "PBM: DTD prev->next : back link wrong\n");
    }
    if (dtd->next == NULL) {
        if ((dtd->parent != NULL)
            && (dtd->parent->last != (xmlNodePtr) dtd))
            fprintf(output, "PBM: DTD has no next and not last of list\n");
    } else {
        if (dtd->next->prev != (xmlNodePtr) dtd)
            fprintf(output, "PBM: DTD next->prev : forward link wrong\n");
    }
    if (dtd->children == NULL)
        fprintf(output, "    DTD is empty\n");
    else
        xmlDebugDumpNodeList(output, dtd->children, 1);
}

static void
xmlDebugDumpEntityCallback(xmlEntityPtr cur, FILE *output) {
    if (cur == NULL) {
	fprintf(output, "Entity is NULL");
	return;
    }
    fprintf(output, "%s : ", cur->name);
    switch (cur->etype) {
	case XML_INTERNAL_GENERAL_ENTITY:
	    fprintf(output, "INTERNAL GENERAL, ");
	    break;
	case XML_EXTERNAL_GENERAL_PARSED_ENTITY:
	    fprintf(output, "EXTERNAL PARSED, ");
	    break;
	case XML_EXTERNAL_GENERAL_UNPARSED_ENTITY:
	    fprintf(output, "EXTERNAL UNPARSED, ");
	    break;
	case XML_INTERNAL_PARAMETER_ENTITY:
	    fprintf(output, "INTERNAL PARAMETER, ");
	    break;
	case XML_EXTERNAL_PARAMETER_ENTITY:
	    fprintf(output, "EXTERNAL PARAMETER, ");
	    break;
	default:
	    fprintf(output, "UNKNOWN TYPE %d",
		    cur->etype);
    }
    if (cur->ExternalID != NULL) 
	fprintf(output, "ID \"%s\"", cur->ExternalID);
    if (cur->SystemID != NULL)
	fprintf(output, "SYSTEM \"%s\"", cur->SystemID);
    if (cur->orig != NULL)
	fprintf(output, "\n orig \"%s\"", cur->orig);
    if ((cur->type != XML_ELEMENT_NODE) &&
	(cur->content != NULL))
	fprintf(output, "\n content \"%s\"", cur->content);
    fprintf(output, "\n");	
}

/**
 * xmlDebugDumpEntities:
 * @output:  the FILE * for the output
 * @doc:  the document
 *
 * Dumps debug information for all the entities in use by the document
 */
void
xmlDebugDumpEntities(FILE * output, xmlDocPtr doc)
{
    if (output == NULL)
        output = stdout;
    if (doc == NULL) {
        fprintf(output, "DOCUMENT == NULL !\n");
        return;
    }

    switch (doc->type) {
        case XML_ELEMENT_NODE:
            fprintf(output, "Error, ELEMENT found here ");
            break;
        case XML_ATTRIBUTE_NODE:
            fprintf(output, "Error, ATTRIBUTE found here\n");
            break;
        case XML_TEXT_NODE:
            fprintf(output, "Error, TEXT\n");
            break;
        case XML_CDATA_SECTION_NODE:
            fprintf(output, "Error, CDATA_SECTION\n");
            break;
        case XML_ENTITY_REF_NODE:
            fprintf(output, "Error, ENTITY_REF\n");
            break;
        case XML_ENTITY_NODE:
            fprintf(output, "Error, ENTITY\n");
            break;
        case XML_PI_NODE:
            fprintf(output, "Error, PI\n");
            break;
        case XML_COMMENT_NODE:
            fprintf(output, "Error, COMMENT\n");
            break;
        case XML_DOCUMENT_NODE:
            fprintf(output, "DOCUMENT\n");
            break;
        case XML_HTML_DOCUMENT_NODE:
            fprintf(output, "HTML DOCUMENT\n");
            break;
        case XML_DOCUMENT_TYPE_NODE:
            fprintf(output, "Error, DOCUMENT_TYPE\n");
            break;
        case XML_DOCUMENT_FRAG_NODE:
            fprintf(output, "Error, DOCUMENT_FRAG\n");
            break;
        case XML_NOTATION_NODE:
            fprintf(output, "Error, NOTATION\n");
            break;
        default:
            fprintf(output, "NODE_%d\n", doc->type);
    }
    if ((doc->intSubset != NULL) && (doc->intSubset->entities != NULL)) {
        xmlEntitiesTablePtr table = (xmlEntitiesTablePtr)
            doc->intSubset->entities;

        fprintf(output, "Entities in internal subset\n");
        xmlHashScan(table, (xmlHashScanner) xmlDebugDumpEntityCallback,
                    output);
    } else
        fprintf(output, "No entities in internal subset\n");
    if ((doc->extSubset != NULL) && (doc->extSubset->entities != NULL)) {
        xmlEntitiesTablePtr table = (xmlEntitiesTablePtr)
            doc->extSubset->entities;

        fprintf(output, "Entities in external subset\n");
        xmlHashScan(table, (xmlHashScanner) xmlDebugDumpEntityCallback,
                    output);
    } else
        fprintf(output, "No entities in external subset\n");
}

/**
 * xmlLsCountNode:
 * @node:  the node to count
 *
 * Count the children of @node.
 *
 * Returns the number of children of @node.
 */
int
xmlLsCountNode(xmlNodePtr node) {
    int ret = 0;
    xmlNodePtr list = NULL;
    
    if (node == NULL)
	return(0);

    switch (node->type) {
	case XML_ELEMENT_NODE:
	    list = node->children;
	    break;
	case XML_DOCUMENT_NODE:
	case XML_HTML_DOCUMENT_NODE:
#ifdef LIBXML_DOCB_ENABLED
	case XML_DOCB_DOCUMENT_NODE:
#endif
	    list = ((xmlDocPtr) node)->children;
	    break;
	case XML_ATTRIBUTE_NODE:
	    list = ((xmlAttrPtr) node)->children;
	    break;
	case XML_TEXT_NODE:
	case XML_CDATA_SECTION_NODE:
	case XML_PI_NODE:
	case XML_COMMENT_NODE:
	    if (node->content != NULL) {
		ret = xmlStrlen(node->content);
            }
	    break;
	case XML_ENTITY_REF_NODE:
	case XML_DOCUMENT_TYPE_NODE:
	case XML_ENTITY_NODE:
	case XML_DOCUMENT_FRAG_NODE:
	case XML_NOTATION_NODE:
	case XML_DTD_NODE:
        case XML_ELEMENT_DECL:
        case XML_ATTRIBUTE_DECL:
        case XML_ENTITY_DECL:
	case XML_NAMESPACE_DECL:
	case XML_XINCLUDE_START:
	case XML_XINCLUDE_END:
	    ret = 1;
	    break;
    }
    for (;list != NULL;ret++) 
        list = list->next;
    return(ret);
}

/**
 * xmlLsOneNode:
 * @output:  the FILE * for the output
 * @node:  the node to dump
 *
 * Dump to @output the type and name of @node.
 */
void
xmlLsOneNode(FILE *output, xmlNodePtr node) {
    if (node == NULL) {
	fprintf(output, "NULL\n");
	return;
    }
    switch (node->type) {
	case XML_ELEMENT_NODE:
	    fprintf(output, "-");
	    break;
	case XML_ATTRIBUTE_NODE:
	    fprintf(output, "a");
	    break;
	case XML_TEXT_NODE:
	    fprintf(output, "t");
	    break;
	case XML_CDATA_SECTION_NODE:
	    fprintf(output, "C");
	    break;
	case XML_ENTITY_REF_NODE:
	    fprintf(output, "e");
	    break;
	case XML_ENTITY_NODE:
	    fprintf(output, "E");
	    break;
	case XML_PI_NODE:
	    fprintf(output, "p");
	    break;
	case XML_COMMENT_NODE:
	    fprintf(output, "c");
	    break;
	case XML_DOCUMENT_NODE:
	    fprintf(output, "d");
	    break;
	case XML_HTML_DOCUMENT_NODE:
	    fprintf(output, "h");
	    break;
	case XML_DOCUMENT_TYPE_NODE:
	    fprintf(output, "T");
	    break;
	case XML_DOCUMENT_FRAG_NODE:
	    fprintf(output, "F");
	    break;
	case XML_NOTATION_NODE:
	    fprintf(output, "N");
	    break;
	case XML_NAMESPACE_DECL:
	    fprintf(output, "n");
	    break;
	default:
	    fprintf(output, "?");
    }
    if (node->type != XML_NAMESPACE_DECL) {
	if (node->properties != NULL)
	    fprintf(output, "a");
	else	
	    fprintf(output, "-");
	if (node->nsDef != NULL) 
	    fprintf(output, "n");
	else	
	    fprintf(output, "-");
    }

    fprintf(output, " %8d ", xmlLsCountNode(node));

    switch (node->type) {
	case XML_ELEMENT_NODE:
	    if (node->name != NULL)
		fprintf(output, "%s", node->name);
	    break;
	case XML_ATTRIBUTE_NODE:
	    if (node->name != NULL)
		fprintf(output, "%s", node->name);
	    break;
	case XML_TEXT_NODE:
	    if (node->content != NULL) {
		xmlDebugDumpString(output, node->content);
            }
	    break;
	case XML_CDATA_SECTION_NODE:
	    break;
	case XML_ENTITY_REF_NODE:
	    if (node->name != NULL)
		fprintf(output, "%s", node->name);
	    break;
	case XML_ENTITY_NODE:
	    if (node->name != NULL)
		fprintf(output, "%s", node->name);
	    break;
	case XML_PI_NODE:
	    if (node->name != NULL)
		fprintf(output, "%s", node->name);
	    break;
	case XML_COMMENT_NODE:
	    break;
	case XML_DOCUMENT_NODE:
	    break;
	case XML_HTML_DOCUMENT_NODE:
	    break;
	case XML_DOCUMENT_TYPE_NODE:
	    break;
	case XML_DOCUMENT_FRAG_NODE:
	    break;
	case XML_NOTATION_NODE:
	    break;
	case XML_NAMESPACE_DECL: {
	    xmlNsPtr ns = (xmlNsPtr) node;

	    if (ns->prefix == NULL)
		fprintf(output, "default -> %s", ns->href);
	    else
		fprintf(output, "%s -> %s", ns->prefix, ns->href);
	    break;
	}
	default:
	    if (node->name != NULL)
		fprintf(output, "%s", node->name);
    }
    fprintf(output, "\n");
}

/**
 * xmlBoolToText:
 * @boolval: a bool to turn into text
 *
 * Convenient way to turn bool into text 
 *
 * Returns a pointer to either "True" or "False"
 */
const char *
xmlBoolToText(int boolval)
{
    if (boolval)
        return("True");
    else
        return("False");
}

/****************************************************************
 *								*
 *	 	The XML shell related functions			*
 *								*
 ****************************************************************/



/*
 * TODO: Improvement/cleanups for the XML shell
 *     - allow to shell out an editor on a subpart
 *     - cleanup function registrations (with help) and calling
 *     - provide registration routines
 */

/**
 * xmlShellPrintXPathError:
 * @errorType: valid xpath error id
 * @arg: the argument that cause xpath to fail
 *
 * Print the xpath error to libxml default error channel
 */
void
xmlShellPrintXPathError(int errorType, const char *arg)
{
    const char *default_arg = "Result";

    if (!arg)
        arg = default_arg;

    switch (errorType) {
        case XPATH_UNDEFINED:
            xmlGenericError(xmlGenericErrorContext,
                            "%s: no such node\n", arg);
            break;

        case XPATH_BOOLEAN:
            xmlGenericError(xmlGenericErrorContext,
                            "%s is a Boolean\n", arg);
            break;
        case XPATH_NUMBER:
            xmlGenericError(xmlGenericErrorContext,
                            "%s is a number\n", arg);
            break;
        case XPATH_STRING:
            xmlGenericError(xmlGenericErrorContext,
                            "%s is a string\n", arg);
            break;
        case XPATH_POINT:
            xmlGenericError(xmlGenericErrorContext,
                            "%s is a point\n", arg);
            break;
        case XPATH_RANGE:
            xmlGenericError(xmlGenericErrorContext,
                            "%s is a range\n", arg);
            break;
        case XPATH_LOCATIONSET:
            xmlGenericError(xmlGenericErrorContext,
                            "%s is a range\n", arg);
            break;
        case XPATH_USERS:
            xmlGenericError(xmlGenericErrorContext,
                            "%s is user-defined\n", arg);
            break;
        case XPATH_XSLT_TREE:
            xmlGenericError(xmlGenericErrorContext,
                            "%s is an XSLT value tree\n", arg);
            break;
    }
    xmlGenericError(xmlGenericErrorContext,
                    "Try casting the result string function (xpath builtin)\n",
                    arg);
}


/**
 * xmlShellPrintNodeCtxt:
 * @ctxt : a non-null shell context
 * @node : a non-null node to print to the output FILE
 *
 * Print node to the output FILE
 */
static void
xmlShellPrintNodeCtxt(xmlShellCtxtPtr ctxt,xmlNodePtr node)
{
    FILE *fp;

    if (!node)
        return;
    if (ctxt == NULL)
	fp = stdout;
    else
	fp = ctxt->output;

    if (node->type == XML_DOCUMENT_NODE)
        xmlDocDump(fp, (xmlDocPtr) node);
    else if (node->type == XML_ATTRIBUTE_NODE)
        xmlDebugDumpAttrList(fp, (xmlAttrPtr) node, 0);
    else
        xmlElemDump(fp, node->doc, node);

    fprintf(fp, "\n");
}

/**
 * xmlShellPrintNode:
 * @node : a non-null node to print to the output FILE
 *
 * Print node to the output FILE
 */
void
xmlShellPrintNode(xmlNodePtr node)
{
    xmlShellPrintNodeCtxt(NULL, node);
}

/**
 * xmlShellPrintXPathResultCtxt:
 * @ctxt: a valid shell context
 * @list: a valid result generated by an xpath evaluation
 *
 * Prints result to the output FILE
 */
static void
xmlShellPrintXPathResultCtxt(xmlShellCtxtPtr ctxt,xmlXPathObjectPtr list)
{
    int i = 0;
    if (!ctxt)
       return;

    if (list != NULL) {
        switch (list->type) {
            case XPATH_NODESET:{
                    int indx;

                    if (list->nodesetval) {
                        for (indx = 0; indx < list->nodesetval->nodeNr;
                             indx++) {
                            if (i > 0)
                                fprintf(stderr, " -------\n");
                            xmlShellPrintNodeCtxt(ctxt,
				    list->nodesetval->nodeTab[indx]);
                        }
                    } else {
                        xmlGenericError(xmlGenericErrorContext,
                                        "Empty node set\n");
                    }
                    break;
                }
            case XPATH_BOOLEAN:
                xmlGenericError(xmlGenericErrorContext,
                                "Is a Boolean:%s\n",
                                xmlBoolToText(list->boolval));
                break;
            case XPATH_NUMBER:
                xmlGenericError(xmlGenericErrorContext,
                                "Is a number:%0g\n", list->floatval);
                break;
            case XPATH_STRING:
                xmlGenericError(xmlGenericErrorContext,
                                "Is a string:%s\n", list->stringval);
                break;

            default:
                xmlShellPrintXPathError(list->type, NULL);
        }
    }
}

/**
 * xmlShellPrintXPathResult:
 * @list: a valid result generated by an xpath evaluation
 *
 * Prints result to the output FILE
 */
void
xmlShellPrintXPathResult(xmlXPathObjectPtr list)
{
    xmlShellPrintXPathResultCtxt(NULL, list);
}

/**
 * xmlShellList:
 * @ctxt:  the shell context
 * @arg:  unused
 * @node:  a node
 * @node2:  unused
 *
 * Implements the XML shell function "ls"
 * Does an Unix like listing of the given node (like a directory)
 *
 * Returns 0
 */
int
xmlShellList(xmlShellCtxtPtr ctxt,
             char *arg ATTRIBUTE_UNUSED, xmlNodePtr node,
             xmlNodePtr node2 ATTRIBUTE_UNUSED)
{
    xmlNodePtr cur;
    if (!ctxt)
        return (0);
    if (node == NULL) {
	fprintf(ctxt->output, "NULL\n");
	return (0);
    }
    if ((node->type == XML_DOCUMENT_NODE) ||
        (node->type == XML_HTML_DOCUMENT_NODE)) {
        cur = ((xmlDocPtr) node)->children;
    } else if (node->type == XML_NAMESPACE_DECL) {
        xmlLsOneNode(ctxt->output, node);
        return (0);
    } else if (node->children != NULL) {
        cur = node->children;
    } else {
        xmlLsOneNode(ctxt->output, node);
        return (0);
    }
    while (cur != NULL) {
        xmlLsOneNode(ctxt->output, cur);
        cur = cur->next;
    }
    return (0);
}

/**
 * xmlShellBase:
 * @ctxt:  the shell context
 * @arg:  unused
 * @node:  a node
 * @node2:  unused
 *
 * Implements the XML shell function "base"
 * dumps the current XML base of the node
 *
 * Returns 0
 */
int
xmlShellBase(xmlShellCtxtPtr ctxt,
             char *arg ATTRIBUTE_UNUSED, xmlNodePtr node,
             xmlNodePtr node2 ATTRIBUTE_UNUSED)
{
    xmlChar *base;
    if (!ctxt)
        return 0;
    if (node == NULL) {
	fprintf(ctxt->output, "NULL\n");
	return (0);
    }    

    base = xmlNodeGetBase(node->doc, node);

    if (base == NULL) {
        fprintf(ctxt->output, " No base found !!!\n");
    } else {
        fprintf(ctxt->output, "%s\n", base);
        xmlFree(base);
    }
    return (0);
}

/**
 * xmlShellSetBase:
 * @ctxt:  the shell context
 * @arg:  the new base
 * @node:  a node
 * @node2:  unused
 *
 * Implements the XML shell function "setbase"
 * change the current XML base of the node
 *
 * Returns 0
 */
static int
xmlShellSetBase(xmlShellCtxtPtr ctxt ATTRIBUTE_UNUSED,
             char *arg ATTRIBUTE_UNUSED, xmlNodePtr node,
             xmlNodePtr node2 ATTRIBUTE_UNUSED)
{
    xmlNodeSetBase(node, (xmlChar*) arg);
    return (0);
}

/**
 * xmlShellGrep:
 * @ctxt:  the shell context
 * @arg:  the string or regular expression to find
 * @node:  a node
 * @node2:  unused
 *
 * Implements the XML shell function "grep"
 * dumps informations about the node (namespace, attributes, content).
 *
 * Returns 0
 */
static int
xmlShellGrep(xmlShellCtxtPtr ctxt ATTRIBUTE_UNUSED,
            char *arg, xmlNodePtr node, xmlNodePtr node2 ATTRIBUTE_UNUSED)
{
    if (!ctxt)
        return (0);
    if (node == NULL)
	return (0);
    if (arg == NULL)
	return (0);
#ifdef LIBXML_REGEXP_ENABLED
    if ((xmlStrchr((xmlChar *) arg, '?')) ||
	(xmlStrchr((xmlChar *) arg, '*')) ||
	(xmlStrchr((xmlChar *) arg, '.')) ||
	(xmlStrchr((xmlChar *) arg, '['))) {
    }
#endif
    while (node != NULL) {
        if (node->type == XML_COMMENT_NODE) {
	    if (xmlStrstr(node->content, (xmlChar *) arg)) {

		fprintf(ctxt->output, "%s : ", xmlGetNodePath(node));
                xmlShellList(ctxt, NULL, node, NULL);
	    }
        } else if (node->type == XML_TEXT_NODE) {
	    if (xmlStrstr(node->content, (xmlChar *) arg)) {

		fprintf(ctxt->output, "%s : ", xmlGetNodePath(node->parent));
                xmlShellList(ctxt, NULL, node->parent, NULL);
	    }
        }

        /*
         * Browse the full subtree, deep first
         */

        if ((node->type == XML_DOCUMENT_NODE) ||
            (node->type == XML_HTML_DOCUMENT_NODE)) {
            node = ((xmlDocPtr) node)->children;
        } else if ((node->children != NULL)
                   && (node->type != XML_ENTITY_REF_NODE)) {
            /* deep first */
            node = node->children;
        } else if (node->next != NULL) {
            /* then siblings */
            node = node->next;
        } else {
            /* go up to parents->next if needed */
            while (node != NULL) {
                if (node->parent != NULL) {
                    node = node->parent;
                }
                if (node->next != NULL) {
                    node = node->next;
                    break;
                }
                if (node->parent == NULL) {
                    node = NULL;
                    break;
                }
            }
	}
    }
    return (0);
}

/**
 * xmlShellDir:
 * @ctxt:  the shell context
 * @arg:  unused
 * @node:  a node
 * @node2:  unused
 *
 * Implements the XML shell function "dir"
 * dumps informations about the node (namespace, attributes, content).
 *
 * Returns 0
 */
int
xmlShellDir(xmlShellCtxtPtr ctxt ATTRIBUTE_UNUSED,
            char *arg ATTRIBUTE_UNUSED, xmlNodePtr node,
            xmlNodePtr node2 ATTRIBUTE_UNUSED)
{
    if (!ctxt)
        return (0);
    if (node == NULL) {
	fprintf(ctxt->output, "NULL\n");
	return (0);
    }    
    if ((node->type == XML_DOCUMENT_NODE) ||
        (node->type == XML_HTML_DOCUMENT_NODE)) {
        xmlDebugDumpDocumentHead(ctxt->output, (xmlDocPtr) node);
    } else if (node->type == XML_ATTRIBUTE_NODE) {
        xmlDebugDumpAttr(ctxt->output, (xmlAttrPtr) node, 0);
    } else {
        xmlDebugDumpOneNode(ctxt->output, node, 0);
    }
    return (0);
}

/**
 * xmlShellCat:
 * @ctxt:  the shell context
 * @arg:  unused
 * @node:  a node
 * @node2:  unused
 *
 * Implements the XML shell function "cat"
 * dumps the serialization node content (XML or HTML).
 *
 * Returns 0
 */
int
xmlShellCat(xmlShellCtxtPtr ctxt, char *arg ATTRIBUTE_UNUSED,
            xmlNodePtr node, xmlNodePtr node2 ATTRIBUTE_UNUSED)
{
    if (!ctxt)
        return (0);
    if (node == NULL) {
	fprintf(ctxt->output, "NULL\n");
	return (0);
    }    
    if (ctxt->doc->type == XML_HTML_DOCUMENT_NODE) {
#ifdef LIBXML_HTML_ENABLED
        if (node->type == XML_HTML_DOCUMENT_NODE)
            htmlDocDump(ctxt->output, (htmlDocPtr) node);
        else
            htmlNodeDumpFile(ctxt->output, ctxt->doc, node);
#else
        if (node->type == XML_DOCUMENT_NODE)
            xmlDocDump(ctxt->output, (xmlDocPtr) node);
        else
            xmlElemDump(ctxt->output, ctxt->doc, node);
#endif /* LIBXML_HTML_ENABLED */
    } else {
        if (node->type == XML_DOCUMENT_NODE)
            xmlDocDump(ctxt->output, (xmlDocPtr) node);
        else
            xmlElemDump(ctxt->output, ctxt->doc, node);
    }
    fprintf(ctxt->output, "\n");
    return (0);
}

/**
 * xmlShellLoad:
 * @ctxt:  the shell context
 * @filename:  the file name
 * @node:  unused
 * @node2:  unused
 *
 * Implements the XML shell function "load"
 * loads a new document specified by the filename
 *
 * Returns 0 or -1 if loading failed
 */
int
xmlShellLoad(xmlShellCtxtPtr ctxt, char *filename,
             xmlNodePtr node ATTRIBUTE_UNUSED,
             xmlNodePtr node2 ATTRIBUTE_UNUSED)
{
    xmlDocPtr doc;
    int html = 0;

    if (ctxt->doc != NULL)
        html = (ctxt->doc->type == XML_HTML_DOCUMENT_NODE);

    if (html) {
#ifdef LIBXML_HTML_ENABLED
        doc = htmlParseFile(filename, NULL);
#else
        fprintf(ctxt->output, "HTML support not compiled in\n");
        doc = NULL;
#endif /* LIBXML_HTML_ENABLED */
    } else {
        doc = xmlParseFile(filename);
    }
    if (doc != NULL) {
        if (ctxt->loaded == 1) {
            xmlFreeDoc(ctxt->doc);
        }
        ctxt->loaded = 1;
#ifdef LIBXML_XPATH_ENABLED
        xmlXPathFreeContext(ctxt->pctxt);
#endif /* LIBXML_XPATH_ENABLED */
        xmlFree(ctxt->filename);
        ctxt->doc = doc;
        ctxt->node = (xmlNodePtr) doc;
#ifdef LIBXML_XPATH_ENABLED
        ctxt->pctxt = xmlXPathNewContext(doc);
#endif /* LIBXML_XPATH_ENABLED */
        ctxt->filename = (char *) xmlStrdup((xmlChar *) filename);
    } else
        return (-1);
    return (0);
}

/**
 * xmlShellWrite:
 * @ctxt:  the shell context
 * @filename:  the file name
 * @node:  a node in the tree
 * @node2:  unused
 *
 * Implements the XML shell function "write"
 * Write the current node to the filename, it saves the serialization
 * of the subtree under the @node specified
 *
 * Returns 0 or -1 in case of error
 */
int
xmlShellWrite(xmlShellCtxtPtr ctxt, char *filename, xmlNodePtr node,
              xmlNodePtr node2 ATTRIBUTE_UNUSED)
{
    if (node == NULL)
        return (-1);
    if ((filename == NULL) || (filename[0] == 0)) {
        xmlGenericError(xmlGenericErrorContext,
                        "Write command requires a filename argument\n");
        return (-1);
    }
#ifdef W_OK
    if (access((char *) filename, W_OK)) {
        xmlGenericError(xmlGenericErrorContext,
                        "Cannot write to %s\n", filename);
        return (-1);
    }
#endif
    switch (node->type) {
        case XML_DOCUMENT_NODE:
            if (xmlSaveFile((char *) filename, ctxt->doc) < -1) {
                xmlGenericError(xmlGenericErrorContext,
                                "Failed to write to %s\n", filename);
                return (-1);
            }
            break;
        case XML_HTML_DOCUMENT_NODE:
#ifdef LIBXML_HTML_ENABLED
            if (htmlSaveFile((char *) filename, ctxt->doc) < 0) {
                xmlGenericError(xmlGenericErrorContext,
                                "Failed to write to %s\n", filename);
                return (-1);
            }
#else
            if (xmlSaveFile((char *) filename, ctxt->doc) < -1) {
                xmlGenericError(xmlGenericErrorContext,
                                "Failed to write to %s\n", filename);
                return (-1);
            }
#endif /* LIBXML_HTML_ENABLED */
            break;
        default:{
                FILE *f;

                f = fopen((char *) filename, "w");
                if (f == NULL) {
                    xmlGenericError(xmlGenericErrorContext,
                                    "Failed to write to %s\n", filename);
                    return (-1);
                }
                xmlElemDump(f, ctxt->doc, node);
                fclose(f);
            }
    }
    return (0);
}

/**
 * xmlShellSave:
 * @ctxt:  the shell context
 * @filename:  the file name (optional)
 * @node:  unused
 * @node2:  unused
 *
 * Implements the XML shell function "save"
 * Write the current document to the filename, or it's original name
 *
 * Returns 0 or -1 in case of error
 */
int
xmlShellSave(xmlShellCtxtPtr ctxt, char *filename,
             xmlNodePtr node ATTRIBUTE_UNUSED,
             xmlNodePtr node2 ATTRIBUTE_UNUSED)
{
    if (ctxt->doc == NULL)
        return (-1);
    if ((filename == NULL) || (filename[0] == 0))
        filename = ctxt->filename;
#ifdef W_OK
    if (access((char *) filename, W_OK)) {
        xmlGenericError(xmlGenericErrorContext,
                        "Cannot save to %s\n", filename);
        return (-1);
    }
#endif
    switch (ctxt->doc->type) {
        case XML_DOCUMENT_NODE:
            if (xmlSaveFile((char *) filename, ctxt->doc) < 0) {
                xmlGenericError(xmlGenericErrorContext,
                                "Failed to save to %s\n", filename);
            }
            break;
        case XML_HTML_DOCUMENT_NODE:
#ifdef LIBXML_HTML_ENABLED
            if (htmlSaveFile((char *) filename, ctxt->doc) < 0) {
                xmlGenericError(xmlGenericErrorContext,
                                "Failed to save to %s\n", filename);
            }
#else
            if (xmlSaveFile((char *) filename, ctxt->doc) < 0) {
                xmlGenericError(xmlGenericErrorContext,
                                "Failed to save to %s\n", filename);
            }
#endif /* LIBXML_HTML_ENABLED */
            break;
        default:
            xmlGenericError(xmlGenericErrorContext,
	    "To save to subparts of a document use the 'write' command\n");
            return (-1);

    }
    return (0);
}

/**
 * xmlShellValidate:
 * @ctxt:  the shell context
 * @dtd:  the DTD URI (optional)
 * @node:  unused
 * @node2:  unused
 *
 * Implements the XML shell function "validate"
 * Validate the document, if a DTD path is provided, then the validation
 * is done against the given DTD.
 *
 * Returns 0 or -1 in case of error
 */
int
xmlShellValidate(xmlShellCtxtPtr ctxt, char *dtd,
                 xmlNodePtr node ATTRIBUTE_UNUSED,
                 xmlNodePtr node2 ATTRIBUTE_UNUSED)
{
    xmlValidCtxt vctxt;
    int res = -1;

    vctxt.userData = stderr;
    vctxt.error = (xmlValidityErrorFunc) fprintf;
    vctxt.warning = (xmlValidityWarningFunc) fprintf;

    if ((dtd == NULL) || (dtd[0] == 0)) {
        res = xmlValidateDocument(&vctxt, ctxt->doc);
    } else {
        xmlDtdPtr subset;

        subset = xmlParseDTD(NULL, (xmlChar *) dtd);
        if (subset != NULL) {
            res = xmlValidateDtd(&vctxt, ctxt->doc, subset);

            xmlFreeDtd(subset);
        }
    }
    return (res);
}

/**
 * xmlShellDu:
 * @ctxt:  the shell context
 * @arg:  unused
 * @tree:  a node defining a subtree
 * @node2:  unused
 *
 * Implements the XML shell function "du"
 * show the structure of the subtree under node @tree
 * If @tree is null, the command works on the current node.
 *
 * Returns 0 or -1 in case of error
 */
int
xmlShellDu(xmlShellCtxtPtr ctxt,
           char *arg ATTRIBUTE_UNUSED, xmlNodePtr tree,
           xmlNodePtr node2 ATTRIBUTE_UNUSED)
{
    xmlNodePtr node;
    int indent = 0, i;

    if (!ctxt)
	return (-1);

    if (tree == NULL)
        return (-1);
    node = tree;
    while (node != NULL) {
        if ((node->type == XML_DOCUMENT_NODE) ||
            (node->type == XML_HTML_DOCUMENT_NODE)) {
            fprintf(ctxt->output, "/\n");
        } else if (node->type == XML_ELEMENT_NODE) {
            for (i = 0; i < indent; i++)
                fprintf(ctxt->output, "  ");
            fprintf(ctxt->output, "%s\n", node->name);
        } else {
        }

        /*
         * Browse the full subtree, deep first
         */

        if ((node->type == XML_DOCUMENT_NODE) ||
            (node->type == XML_HTML_DOCUMENT_NODE)) {
            node = ((xmlDocPtr) node)->children;
        } else if ((node->children != NULL)
                   && (node->type != XML_ENTITY_REF_NODE)) {
            /* deep first */
            node = node->children;
            indent++;
        } else if ((node != tree) && (node->next != NULL)) {
            /* then siblings */
            node = node->next;
        } else if (node != tree) {
            /* go up to parents->next if needed */
            while (node != tree) {
                if (node->parent != NULL) {
                    node = node->parent;
                    indent--;
                }
                if ((node != tree) && (node->next != NULL)) {
                    node = node->next;
                    break;
                }
                if (node->parent == NULL) {
                    node = NULL;
                    break;
                }
                if (node == tree) {
                    node = NULL;
                    break;
                }
            }
            /* exit condition */
            if (node == tree)
                node = NULL;
        } else
            node = NULL;
    }
    return (0);
}

/**
 * xmlShellPwd:
 * @ctxt:  the shell context
 * @buffer:  the output buffer
 * @node:  a node 
 * @node2:  unused
 *
 * Implements the XML shell function "pwd"
 * Show the full path from the root to the node, if needed building
 * thumblers when similar elements exists at a given ancestor level.
 * The output is compatible with XPath commands.
 *
 * Returns 0 or -1 in case of error
 */
int
xmlShellPwd(xmlShellCtxtPtr ctxt ATTRIBUTE_UNUSED, char *buffer,
            xmlNodePtr node, xmlNodePtr node2 ATTRIBUTE_UNUSED)
{
    xmlChar *path;

    if (node == NULL)
        return (-1);

    path = xmlGetNodePath(node);
    if (path == NULL)
	return (-1);

    /*
     * This test prevents buffer overflow, because this routine
     * is only called by xmlShell, in which the second argument is
     * 500 chars long.
     * It is a dirty hack before a cleaner solution is found.
     * Documentation should mention that the second argument must
     * be at least 500 chars long, and could be stripped if too long.
     */
    snprintf(buffer, 499, "%s", path);
    buffer[499] = '0';
    xmlFree(path);

    return (0);
}

/**
 * xmlShell:
 * @doc:  the initial document
 * @filename:  the output buffer
 * @input:  the line reading function
 * @output:  the output FILE*, defaults to stdout if NULL
 *
 * Implements the XML shell 
 * This allow to load, validate, view, modify and save a document
 * using a environment similar to a UNIX commandline.
 */
void
xmlShell(xmlDocPtr doc, char *filename, xmlShellReadlineFunc input,
         FILE * output)
{
    char prompt[500] = "/ > ";
    char *cmdline = NULL, *cur;
    int nbargs;
    char command[100];
    char arg[400];
    int i;
    xmlShellCtxtPtr ctxt;
    xmlXPathObjectPtr list;

    if (doc == NULL)
        return;
    if (filename == NULL)
        return;
    if (input == NULL)
        return;
    if (output == NULL)
        output = stdout;
    ctxt = (xmlShellCtxtPtr) xmlMalloc(sizeof(xmlShellCtxt));
    if (ctxt == NULL)
        return;
    ctxt->loaded = 0;
    ctxt->doc = doc;
    ctxt->input = input;
    ctxt->output = output;
    ctxt->filename = (char *) xmlStrdup((xmlChar *) filename);
    ctxt->node = (xmlNodePtr) ctxt->doc;

#ifdef LIBXML_XPATH_ENABLED
    ctxt->pctxt = xmlXPathNewContext(ctxt->doc);
    if (ctxt->pctxt == NULL) {
        xmlFree(ctxt);
        return;
    }
#endif /* LIBXML_XPATH_ENABLED */
    while (1) {
        if (ctxt->node == (xmlNodePtr) ctxt->doc)
            snprintf(prompt, sizeof(prompt), "%s > ", "/");
        else if (ctxt->node->name)
            snprintf(prompt, sizeof(prompt), "%s > ", ctxt->node->name);
        else
            snprintf(prompt, sizeof(prompt), "? > ");
        prompt[sizeof(prompt) - 1] = 0;

        /*
         * Get a new command line
         */
        cmdline = ctxt->input(prompt);
        if (cmdline == NULL)
            break;

        /*
         * Parse the command itself
         */
        cur = cmdline;
        nbargs = 0;
        while ((*cur == ' ') || (*cur == '\t'))
            cur++;
        i = 0;
        while ((*cur != ' ') && (*cur != '\t') &&
               (*cur != '\n') && (*cur != '\r')) {
            if (*cur == 0)
                break;
            command[i++] = *cur++;
        }
        command[i] = 0;
        if (i == 0)
            continue;
        nbargs++;

        /*
         * Parse the argument
         */
        while ((*cur == ' ') || (*cur == '\t'))
            cur++;
        i = 0;
        while ((*cur != '\n') && (*cur != '\r') && (*cur != 0)) {
            if (*cur == 0)
                break;
            arg[i++] = *cur++;
        }
        arg[i] = 0;
        if (i != 0)
            nbargs++;

        /*
         * start interpreting the command
         */
        if (!strcmp(command, "exit"))
            break;
        if (!strcmp(command, "quit"))
            break;
        if (!strcmp(command, "bye"))
            break;
		if (!strcmp(command, "help")) {
		  fprintf(ctxt->output, "\tbase         display XML base of the node\n");
		  fprintf(ctxt->output, "\tsetbase URI  change the XML base of the node\n");
		  fprintf(ctxt->output, "\tbye          leave shell\n");
		  fprintf(ctxt->output, "\tcat [node]   display node or current node\n");
		  fprintf(ctxt->output, "\tcd [path]    change directory to path or to root\n");
		  fprintf(ctxt->output, "\tdir [path]   dumps informations about the node (namespace, attributes, content)\n");
		  fprintf(ctxt->output, "\tdu [path]    show the structure of the subtree under path or the current node\n");
		  fprintf(ctxt->output, "\texit         leave shell\n");
		  fprintf(ctxt->output, "\thelp         display this help\n");
		  fprintf(ctxt->output, "\tfree         display memory usage\n");
		  fprintf(ctxt->output, "\tload [name]  load a new document with name\n");
		  fprintf(ctxt->output, "\tls [path]    list contents of path or the current directory\n");
#ifdef LIBXML_XPATH_ENABLED
		  fprintf(ctxt->output, "\txpath expr   evaluate the XPath expression in that context and print the result\n");
#endif /* LIBXML_XPATH_ENABLED */
		  fprintf(ctxt->output, "\tpwd          display current working directory\n");
		  fprintf(ctxt->output, "\tquit         leave shell\n");
		  fprintf(ctxt->output, "\tsave [name]  save this document to name or the original name\n");
		  fprintf(ctxt->output, "\tvalidate     check the document for errors\n");
		  fprintf(ctxt->output, "\twrite [name] write the current node to the filename\n");
		  fprintf(ctxt->output, "\tgrep string  search for a string in the subtree\n");
        } else if (!strcmp(command, "validate")) {
            xmlShellValidate(ctxt, arg, NULL, NULL);
        } else if (!strcmp(command, "load")) {
            xmlShellLoad(ctxt, arg, NULL, NULL);
        } else if (!strcmp(command, "save")) {
            xmlShellSave(ctxt, arg, NULL, NULL);
        } else if (!strcmp(command, "write")) {
            xmlShellWrite(ctxt, arg, NULL, NULL);
        } else if (!strcmp(command, "grep")) {
            xmlShellGrep(ctxt, arg, ctxt->node, NULL);
        } else if (!strcmp(command, "free")) {
            if (arg[0] == 0) {
                xmlMemShow(ctxt->output, 0);
            } else {
                int len = 0;

                sscanf(arg, "%d", &len);
                xmlMemShow(ctxt->output, len);
            }
        } else if (!strcmp(command, "pwd")) {
            char dir[500];

            if (!xmlShellPwd(ctxt, dir, ctxt->node, NULL))
                fprintf(ctxt->output, "%s\n", dir);
        } else if (!strcmp(command, "du")) {
            xmlShellDu(ctxt, NULL, ctxt->node, NULL);
        } else if (!strcmp(command, "base")) {
            xmlShellBase(ctxt, NULL, ctxt->node, NULL);
#ifdef LIBXML_XPATH_ENABLED
        } else if (!strcmp(command, "xpath")) {
            if (arg[0] == 0) {
		xmlGenericError(xmlGenericErrorContext,
				"xpath: expression required\n");
	    } else {
                ctxt->pctxt->node = ctxt->node;
                list = xmlXPathEval((xmlChar *) arg, ctxt->pctxt);
		xmlXPathDebugDumpObject(ctxt->output, list, 0);
		xmlXPathFreeObject(list);
	    }
#endif /* LIBXML_XPATH_ENABLED */
        } else if (!strcmp(command, "setbase")) {
            xmlShellSetBase(ctxt, arg, ctxt->node, NULL);
        } else if ((!strcmp(command, "ls")) || (!strcmp(command, "dir"))) {
            int dir = (!strcmp(command, "dir"));

            if (arg[0] == 0) {
                if (dir)
                    xmlShellDir(ctxt, NULL, ctxt->node, NULL);
                else
                    xmlShellList(ctxt, NULL, ctxt->node, NULL);
            } else {
                ctxt->pctxt->node = ctxt->node;
#ifdef LIBXML_XPATH_ENABLED
                ctxt->pctxt->node = ctxt->node;
                list = xmlXPathEval((xmlChar *) arg, ctxt->pctxt);
#else
                list = NULL;
#endif /* LIBXML_XPATH_ENABLED */
                if (list != NULL) {
                    switch (list->type) {
                        case XPATH_UNDEFINED:
                            xmlGenericError(xmlGenericErrorContext,
                                            "%s: no such node\n", arg);
                            break;
                        case XPATH_NODESET:{
                                int indx;

				if (list->nodesetval == NULL)
				    break;

                                for (indx = 0;
                                     indx < list->nodesetval->nodeNr;
                                     indx++) {
                                    if (dir)
                                        xmlShellDir(ctxt, NULL,
                                                    list->nodesetval->
                                                    nodeTab[indx], NULL);
                                    else
                                        xmlShellList(ctxt, NULL,
                                                     list->nodesetval->
                                                     nodeTab[indx], NULL);
                                }
                                break;
                            }
                        case XPATH_BOOLEAN:
                            xmlGenericError(xmlGenericErrorContext,
                                            "%s is a Boolean\n", arg);
                            break;
                        case XPATH_NUMBER:
                            xmlGenericError(xmlGenericErrorContext,
                                            "%s is a number\n", arg);
                            break;
                        case XPATH_STRING:
                            xmlGenericError(xmlGenericErrorContext,
                                            "%s is a string\n", arg);
                            break;
                        case XPATH_POINT:
                            xmlGenericError(xmlGenericErrorContext,
                                            "%s is a point\n", arg);
                            break;
                        case XPATH_RANGE:
                            xmlGenericError(xmlGenericErrorContext,
                                            "%s is a range\n", arg);
                            break;
                        case XPATH_LOCATIONSET:
                            xmlGenericError(xmlGenericErrorContext,
                                            "%s is a range\n", arg);
                            break;
                        case XPATH_USERS:
                            xmlGenericError(xmlGenericErrorContext,
                                            "%s is user-defined\n", arg);
                            break;
                        case XPATH_XSLT_TREE:
                            xmlGenericError(xmlGenericErrorContext,
                                            "%s is an XSLT value tree\n",
                                            arg);
                            break;
                    }
#ifdef LIBXML_XPATH_ENABLED
                    xmlXPathFreeObject(list);
#endif
                } else {
                    xmlGenericError(xmlGenericErrorContext,
                                    "%s: no such node\n", arg);
                }
                ctxt->pctxt->node = NULL;
            }
        } else if (!strcmp(command, "cd")) {
            if (arg[0] == 0) {
                ctxt->node = (xmlNodePtr) ctxt->doc;
            } else {
#ifdef LIBXML_XPATH_ENABLED
                ctxt->pctxt->node = ctxt->node;
                list = xmlXPathEval((xmlChar *) arg, ctxt->pctxt);
#else
                list = NULL;
#endif /* LIBXML_XPATH_ENABLED */
                if (list != NULL) {
                    switch (list->type) {
                        case XPATH_UNDEFINED:
                            xmlGenericError(xmlGenericErrorContext,
                                            "%s: no such node\n", arg);
                            break;
                        case XPATH_NODESET:
                            if (list->nodesetval != NULL) {
				if (list->nodesetval->nodeNr == 1) {
				    ctxt->node = list->nodesetval->nodeTab[0];
				} else
				    xmlGenericError(xmlGenericErrorContext,
						    "%s is a %d Node Set\n",
						    arg,
						    list->nodesetval->nodeNr);
                            } else
                                xmlGenericError(xmlGenericErrorContext,
                                                "%s is an empty Node Set\n",
                                                arg);
                            break;
                        case XPATH_BOOLEAN:
                            xmlGenericError(xmlGenericErrorContext,
                                            "%s is a Boolean\n", arg);
                            break;
                        case XPATH_NUMBER:
                            xmlGenericError(xmlGenericErrorContext,
                                            "%s is a number\n", arg);
                            break;
                        case XPATH_STRING:
                            xmlGenericError(xmlGenericErrorContext,
                                            "%s is a string\n", arg);
                            break;
                        case XPATH_POINT:
                            xmlGenericError(xmlGenericErrorContext,
                                            "%s is a point\n", arg);
                            break;
                        case XPATH_RANGE:
                            xmlGenericError(xmlGenericErrorContext,
                                            "%s is a range\n", arg);
                            break;
                        case XPATH_LOCATIONSET:
                            xmlGenericError(xmlGenericErrorContext,
                                            "%s is a range\n", arg);
                            break;
                        case XPATH_USERS:
                            xmlGenericError(xmlGenericErrorContext,
                                            "%s is user-defined\n", arg);
                            break;
                        case XPATH_XSLT_TREE:
                            xmlGenericError(xmlGenericErrorContext,
                                            "%s is an XSLT value tree\n",
                                            arg);
                            break;
                    }
#ifdef LIBXML_XPATH_ENABLED
                    xmlXPathFreeObject(list);
#endif
                } else {
                    xmlGenericError(xmlGenericErrorContext,
                                    "%s: no such node\n", arg);
                }
                ctxt->pctxt->node = NULL;
            }
        } else if (!strcmp(command, "cat")) {
            if (arg[0] == 0) {
                xmlShellCat(ctxt, NULL, ctxt->node, NULL);
            } else {
                ctxt->pctxt->node = ctxt->node;
#ifdef LIBXML_XPATH_ENABLED
                ctxt->pctxt->node = ctxt->node;
                list = xmlXPathEval((xmlChar *) arg, ctxt->pctxt);
#else
                list = NULL;
#endif /* LIBXML_XPATH_ENABLED */
                if (list != NULL) {
                    switch (list->type) {
                        case XPATH_UNDEFINED:
                            xmlGenericError(xmlGenericErrorContext,
                                            "%s: no such node\n", arg);
                            break;
                        case XPATH_NODESET:{
                                int indx;

				if (list->nodesetval == NULL)
				    break;

                                for (indx = 0;
                                     indx < list->nodesetval->nodeNr;
                                     indx++) {
                                    if (i > 0)
                                        fprintf(ctxt->output, " -------\n");
                                    xmlShellCat(ctxt, NULL,
                                                list->nodesetval->
                                                nodeTab[indx], NULL);
                                }
                                break;
                            }
                        case XPATH_BOOLEAN:
                            xmlGenericError(xmlGenericErrorContext,
                                            "%s is a Boolean\n", arg);
                            break;
                        case XPATH_NUMBER:
                            xmlGenericError(xmlGenericErrorContext,
                                            "%s is a number\n", arg);
                            break;
                        case XPATH_STRING:
                            xmlGenericError(xmlGenericErrorContext,
                                            "%s is a string\n", arg);
                            break;
                        case XPATH_POINT:
                            xmlGenericError(xmlGenericErrorContext,
                                            "%s is a point\n", arg);
                            break;
                        case XPATH_RANGE:
                            xmlGenericError(xmlGenericErrorContext,
                                            "%s is a range\n", arg);
                            break;
                        case XPATH_LOCATIONSET:
                            xmlGenericError(xmlGenericErrorContext,
                                            "%s is a range\n", arg);
                            break;
                        case XPATH_USERS:
                            xmlGenericError(xmlGenericErrorContext,
                                            "%s is user-defined\n", arg);
                            break;
                        case XPATH_XSLT_TREE:
                            xmlGenericError(xmlGenericErrorContext,
                                            "%s is an XSLT value tree\n",
                                            arg);
                            break;
                    }
#ifdef LIBXML_XPATH_ENABLED
                    xmlXPathFreeObject(list);
#endif
                } else {
                    xmlGenericError(xmlGenericErrorContext,
                                    "%s: no such node\n", arg);
                }
                ctxt->pctxt->node = NULL;
            }
        } else {
            xmlGenericError(xmlGenericErrorContext,
                            "Unknown command %s\n", command);
        }
        free(cmdline);          /* not xmlFree here ! */
    }
#ifdef LIBXML_XPATH_ENABLED
    xmlXPathFreeContext(ctxt->pctxt);
#endif /* LIBXML_XPATH_ENABLED */
    if (ctxt->loaded) {
        xmlFreeDoc(ctxt->doc);
    }
    if (ctxt->filename != NULL)
        xmlFree(ctxt->filename);
    xmlFree(ctxt);
    if (cmdline != NULL)
        free(cmdline);          /* not xmlFree here ! */
}

#endif /* LIBXML_DEBUG_ENABLED */
