/*
 * schemas.c : implementation of the XML Schema handling and
 *             schema validity checking
 *
 * See Copyright for the status of this software.
 *
 * Daniel Veillard <veillard@redhat.com>
 */

#define IN_LIBXML
#include "libxml.h"

#ifdef LIBXML_SCHEMAS_ENABLED

#include <string.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/hash.h>
#include <libxml/uri.h>

#include <libxml/xmlschemas.h>
#include <libxml/schemasInternals.h>
#include <libxml/xmlschemastypes.h>
#include <libxml/xmlautomata.h>
#include <libxml/xmlregexp.h>

#define DEBUG 1                 /* very verbose output */
#define DEBUG_CONTENT 1
#define DEBUG_TYPE 1
/* #define DEBUG_CONTENT_REGEXP 1 */
/* #define DEBUG_AUTOMATA 1 */

#define UNBOUNDED (1 << 30)
#define TODO 								\
    xmlGenericError(xmlGenericErrorContext,				\
	    "Unimplemented block at %s:%d\n",				\
            __FILE__, __LINE__);

#define XML_SCHEMAS_DEFAULT_NAMESPACE (const xmlChar *)"the default namespace"

/*
 * The XML Schemas namespaces
 */
static const xmlChar *xmlSchemaNs = (const xmlChar *)
    "http://www.w3.org/2001/XMLSchema";

static const xmlChar *xmlSchemaInstanceNs = (const xmlChar *)
    "http://www.w3.org/2001/XMLSchema-instance";

#define IS_SCHEMA(node, type)						\
   ((node != NULL) && (node->ns != NULL) &&				\
    (xmlStrEqual(node->name, (const xmlChar *) type)) &&		\
    (xmlStrEqual(node->ns->href, xmlSchemaNs)))

#define XML_SCHEMAS_PARSE_ERROR		1

struct _xmlSchemaParserCtxt {
    void *userData;			/* user specific data block */
    xmlSchemaValidityErrorFunc error;	/* the callback in case of errors */
    xmlSchemaValidityWarningFunc warning;/* the callback in case of warning */
    xmlSchemaValidError err;

    xmlSchemaPtr       schema;        /* The schema in use */
    xmlChar 	      *container;     /* the current element, group, ... */
    int counter;

    xmlChar	      *URL;
    xmlDocPtr          doc;

    const char     *buffer;
    int               size;

    /*
     * Used to build complex element content models
     */
    xmlAutomataPtr     am;
    xmlAutomataStatePtr start;
    xmlAutomataStatePtr end;
    xmlAutomataStatePtr state;
};


#define XML_SCHEMAS_ATTR_UNKNOWN 1
#define XML_SCHEMAS_ATTR_CHECKED 2

typedef struct _xmlSchemaAttrState xmlSchemaAttrState;
typedef xmlSchemaAttrState *xmlSchemaAttrStatePtr;
struct _xmlSchemaAttrState {
    xmlAttrPtr       attr;
    int              state;
};

/**
 * xmlSchemaValidCtxt:
 *
 * A Schemas validation context
 */

struct _xmlSchemaValidCtxt {
    void *userData;			/* user specific data block */
    xmlSchemaValidityErrorFunc error;	/* the callback in case of errors */
    xmlSchemaValidityWarningFunc warning;/* the callback in case of warning */

    xmlSchemaPtr            schema;        /* The schema in use */
    xmlDocPtr               doc;
    xmlParserInputBufferPtr input;
    xmlCharEncoding         enc;
    xmlSAXHandlerPtr        sax;
    void                   *user_data;

    xmlDocPtr               myDoc;
    int                     err;

    xmlNodePtr              node;
    xmlSchemaTypePtr        type;

    xmlRegExecCtxtPtr       regexp;
    xmlSchemaValPtr         value;

    int                     attrNr;
    int                     attrBase;
    int                     attrMax;
    xmlSchemaAttrStatePtr   attr;
};


/************************************************************************
 * 									*
 * 			Some predeclarations				*
 * 									*
 ************************************************************************/
static int xmlSchemaValidateSimpleValue(xmlSchemaValidCtxtPtr ctxt, 
			     xmlSchemaTypePtr type,
	                     xmlChar *value);

/************************************************************************
 * 									*
 * 			Allocation functions				*
 * 									*
 ************************************************************************/

/**
 * xmlSchemaNewSchema:
 * @ctxt:  a schema validation context (optional)
 *
 * Allocate a new Schema structure.
 *
 * Returns the newly allocated structure or NULL in case or error
 */
static xmlSchemaPtr
xmlSchemaNewSchema(xmlSchemaParserCtxtPtr ctxt)
{
    xmlSchemaPtr ret;

    ret = (xmlSchemaPtr) xmlMalloc(sizeof(xmlSchema));
    if (ret == NULL) {
        if ((ctxt != NULL) && (ctxt->error != NULL))
            ctxt->error(ctxt->userData, "Out of memory\n");
        return (NULL);
    }
    memset(ret, 0, sizeof(xmlSchema));

    return (ret);
}

/**
 * xmlSchemaNewFacet:
 * @ctxt:  a schema validation context (optional)
 *
 * Allocate a new Facet structure.
 *
 * Returns the newly allocated structure or NULL in case or error
 */
static xmlSchemaFacetPtr
xmlSchemaNewFacet(xmlSchemaParserCtxtPtr ctxt)
{
    xmlSchemaFacetPtr ret;

    ret = (xmlSchemaFacetPtr) xmlMalloc(sizeof(xmlSchemaFacet));
    if (ret == NULL) {
        if ((ctxt != NULL) && (ctxt->error != NULL))
            ctxt->error(ctxt->userData, "Out of memory\n");
        return (NULL);
    }
    memset(ret, 0, sizeof(xmlSchemaFacet));

    return (ret);
}

/**
 * xmlSchemaNewAnnot:
 * @ctxt:  a schema validation context (optional)
 * @node:  a node
 *
 * Allocate a new annotation structure.
 *
 * Returns the newly allocated structure or NULL in case or error
 */
static xmlSchemaAnnotPtr
xmlSchemaNewAnnot(xmlSchemaParserCtxtPtr ctxt, xmlNodePtr node)
{
    xmlSchemaAnnotPtr ret;

    ret = (xmlSchemaAnnotPtr) xmlMalloc(sizeof(xmlSchemaAnnot));
    if (ret == NULL) {
        if ((ctxt != NULL) && (ctxt->error != NULL))
            ctxt->error(ctxt->userData, "Out of memory\n");
        return (NULL);
    }
    memset(ret, 0, sizeof(xmlSchemaAnnot));
    ret->content = node;
    return (ret);
}

/**
 * xmlSchemaFreeAnnot:
 * @annot:  a schema type structure
 *
 * Deallocate a annotation structure
 */
static void
xmlSchemaFreeAnnot(xmlSchemaAnnotPtr annot)
{
    if (annot == NULL)
        return;
    xmlFree(annot);
}

/**
 * xmlSchemaFreeNotation:
 * @schema:  a schema notation structure
 *
 * Deallocate a Schema Notation structure.
 */
static void
xmlSchemaFreeNotation(xmlSchemaNotationPtr nota)
{
    if (nota == NULL)
        return;
    if (nota->name != NULL)
        xmlFree((xmlChar *) nota->name);
    xmlFree(nota);
}

/**
 * xmlSchemaFreeAttribute:
 * @schema:  a schema attribute structure
 *
 * Deallocate a Schema Attribute structure.
 */
static void
xmlSchemaFreeAttribute(xmlSchemaAttributePtr attr)
{
    if (attr == NULL)
        return;
    if (attr->name != NULL)
        xmlFree((xmlChar *) attr->name);
    if (attr->ref != NULL)
	xmlFree((xmlChar *) attr->ref);
    if (attr->refNs != NULL)
	xmlFree((xmlChar *) attr->refNs);
    if (attr->typeName != NULL)
	xmlFree((xmlChar *) attr->typeName);
    if (attr->typeNs != NULL)
	xmlFree((xmlChar *) attr->typeNs);
    xmlFree(attr);
}

/**
 * xmlSchemaFreeAttributeGroup:
 * @schema:  a schema attribute group structure
 *
 * Deallocate a Schema Attribute Group structure.
 */
static void
xmlSchemaFreeAttributeGroup(xmlSchemaAttributeGroupPtr attr)
{
    if (attr == NULL)
        return;
    if (attr->name != NULL)
        xmlFree((xmlChar *) attr->name);
    xmlFree(attr);
}

/**
 * xmlSchemaFreeElement:
 * @schema:  a schema element structure
 *
 * Deallocate a Schema Element structure.
 */
static void
xmlSchemaFreeElement(xmlSchemaElementPtr elem)
{
    if (elem == NULL)
        return;
    if (elem->name != NULL)
        xmlFree((xmlChar *) elem->name);
    if (elem->namedType != NULL)
        xmlFree((xmlChar *) elem->namedType);
    if (elem->namedTypeNs != NULL)
        xmlFree((xmlChar *) elem->namedTypeNs);
    if (elem->ref != NULL)
        xmlFree((xmlChar *) elem->ref);
    if (elem->refNs != NULL)
        xmlFree((xmlChar *) elem->refNs);
    if (elem->annot != NULL)
        xmlSchemaFreeAnnot(elem->annot);
    if (elem->contModel != NULL)
	xmlRegFreeRegexp(elem->contModel);
    xmlFree(elem);
}

/**
 * xmlSchemaFreeFacet:
 * @facet:  a schema facet structure
 *
 * Deallocate a Schema Facet structure.
 */
static void
xmlSchemaFreeFacet(xmlSchemaFacetPtr facet)
{
    if (facet == NULL)
        return;
    if (facet->value != NULL)
        xmlFree((xmlChar *) facet->value);
    if (facet->id != NULL)
        xmlFree((xmlChar *) facet->id);
    if (facet->val != NULL)
	xmlSchemaFreeValue(facet->val);
    if (facet->regexp != NULL)
	xmlRegFreeRegexp(facet->regexp);
    if (facet->annot != NULL)
	xmlSchemaFreeAnnot(facet->annot);
    xmlFree(facet);
}

/**
 * xmlSchemaFreeType:
 * @type:  a schema type structure
 *
 * Deallocate a Schema Type structure.
 */
void
xmlSchemaFreeType(xmlSchemaTypePtr type)
{
    if (type == NULL)
        return;
    if (type->name != NULL)
        xmlFree((xmlChar *) type->name);
    if (type->base != NULL)
        xmlFree((xmlChar *) type->base);
    if (type->baseNs != NULL)
        xmlFree((xmlChar *) type->baseNs);
    if (type->annot != NULL)
        xmlSchemaFreeAnnot(type->annot);
    if (type->facets != NULL) {
	xmlSchemaFacetPtr facet, next;

	facet = type->facets;
	while (facet != NULL) {
	    next = facet->next;
	    xmlSchemaFreeFacet(facet);
	    facet = next;
	}
    }
    xmlFree(type);
}

/**
 * xmlSchemaFree:
 * @schema:  a schema structure
 *
 * Deallocate a Schema structure.
 */
void
xmlSchemaFree(xmlSchemaPtr schema)
{
    if (schema == NULL)
        return;

    if (schema->name != NULL)
        xmlFree((xmlChar *) schema->name);
    if (schema->notaDecl != NULL)
        xmlHashFree(schema->notaDecl,
                    (xmlHashDeallocator) xmlSchemaFreeNotation);
    if (schema->attrDecl != NULL)
        xmlHashFree(schema->attrDecl,
                    (xmlHashDeallocator) xmlSchemaFreeAttribute);
    if (schema->attrgrpDecl != NULL)
        xmlHashFree(schema->attrgrpDecl,
                    (xmlHashDeallocator) xmlSchemaFreeAttributeGroup);
    if (schema->elemDecl != NULL)
        xmlHashFree(schema->elemDecl,
                    (xmlHashDeallocator) xmlSchemaFreeElement);
    if (schema->typeDecl != NULL)
        xmlHashFree(schema->typeDecl,
                    (xmlHashDeallocator) xmlSchemaFreeType);
    if (schema->annot != NULL)
	xmlSchemaFreeAnnot(schema->annot);
    if (schema->doc != NULL)
	xmlFreeDoc(schema->doc);

    xmlFree(schema);
}

/************************************************************************
 * 									*
 * 			Error functions					*
 * 									*
 ************************************************************************/

/**
 * xmlSchemaErrorContext:
 * @ctxt:  the parsing context
 * @schema:  the schema being built
 * @node:  the node being processed
 * @child:  the child being processed
 *
 * Dump a SchemaType structure
 */
static void
xmlSchemaErrorContext(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                      xmlNodePtr node, xmlNodePtr child)
{
    int line = 0;
    const xmlChar *file = NULL;
    const xmlChar *name = NULL;
    const char *type = "error";

    if ((ctxt == NULL) || (ctxt->error == NULL))
	return;

    if (child != NULL)
	node = child;

    if (node != NULL)  {
	if ((node->type == XML_DOCUMENT_NODE) ||
	    (node->type == XML_HTML_DOCUMENT_NODE)) {
	    xmlDocPtr doc = (xmlDocPtr) node;

	    file = doc->URL;
	} else {
	    /*
	     * Try to find contextual informations to report
	     */
	    if (node->type == XML_ELEMENT_NODE) {
		line = (int) node->content;
	    } else if ((node->prev != NULL) &&
		       (node->prev->type == XML_ELEMENT_NODE)) {
		line = (int) node->prev->content;
	    } else if ((node->parent != NULL) &&
		       (node->parent->type == XML_ELEMENT_NODE)) {
		line = (int) node->parent->content;
	    }
	    if ((node->doc != NULL) && (node->doc->URL != NULL))
		file = node->doc->URL;
	    if (node->name != NULL)
		name = node->name;
	}
    } 
    
    if (ctxt != NULL)
	type = "compilation error";
    else if (schema != NULL)
	type = "runtime error";

    if ((file != NULL) && (line != 0) && (name != NULL))
	ctxt->error(ctxt->userData, "%s: file %s line %d element %s\n",
		type, file, line, name);
    else if ((file != NULL) && (name != NULL))
	ctxt->error(ctxt->userData, "%s: file %s element %s\n",
		type, file, name);
    else if ((file != NULL) && (line != 0))
	ctxt->error(ctxt->userData, "%s: file %s line %d\n", type, file, line);
    else if (file != NULL)
	ctxt->error(ctxt->userData, "%s: file %s\n", type, file);
    else if (name != NULL)
	ctxt->error(ctxt->userData, "%s: element %s\n", type, name);
    else
	ctxt->error(ctxt->userData, "%s\n", type);
}

/************************************************************************
 * 									*
 * 			Debug functions					*
 * 									*
 ************************************************************************/

/**
 * xmlSchemaElementDump:
 * @elem:  an element
 * @output:  the file output
 *
 * Dump the element
 */
static void
xmlSchemaElementDump(xmlSchemaElementPtr elem, FILE * output,
		     const xmlChar *name ATTRIBUTE_UNUSED,
		     const xmlChar *context ATTRIBUTE_UNUSED,
		     const xmlChar *namespace ATTRIBUTE_UNUSED)
{
    if (elem == NULL)
        return;

    fprintf(output, "Element ");
    if (elem->flags & XML_SCHEMAS_ELEM_TOPLEVEL)
	fprintf(output, "toplevel ");
    fprintf(output, ": %s ", elem->name);
    if (namespace != NULL)
	fprintf(output, "namespace '%s' ", namespace);
    
    if (elem->flags & XML_SCHEMAS_ELEM_NILLABLE)
	fprintf(output, "nillable ");
    if (elem->flags & XML_SCHEMAS_ELEM_GLOBAL)
	fprintf(output, "global ");
    if (elem->flags & XML_SCHEMAS_ELEM_DEFAULT)
	fprintf(output, "default ");
    if (elem->flags & XML_SCHEMAS_ELEM_FIXED)
	fprintf(output, "fixed ");
    if (elem->flags & XML_SCHEMAS_ELEM_ABSTRACT)
	fprintf(output, "abstract ");
    if (elem->flags & XML_SCHEMAS_ELEM_REF)
	fprintf(output, "ref '%s' ", elem->ref);
    if (elem->id != NULL)
	fprintf(output, "id '%s' ", elem->id);
    fprintf(output, "\n");
    if ((elem->minOccurs != 1) || (elem->maxOccurs != 1)) {
	fprintf(output, "  ");
	if (elem->minOccurs != 1)
	    fprintf(output, "min: %d ", elem->minOccurs);
	if (elem->maxOccurs >= UNBOUNDED)
	    fprintf(output, "max: unbounded\n");
	else if (elem->maxOccurs != 1)
	    fprintf(output, "max: %d\n", elem->maxOccurs);
	else
	    fprintf(output, "\n");
    }
    if (elem->namedType != NULL) {
	fprintf(output, "  type: %s", elem->namedType);
	if (elem->namedTypeNs != NULL)
	    fprintf(output, " ns %s\n", elem->namedTypeNs);
	else
	    fprintf(output, "\n");
    }
    if (elem->substGroup != NULL) {
	fprintf(output, "  substitutionGroup: %s", elem->substGroup);
	if (elem->substGroupNs != NULL)
	    fprintf(output, " ns %s\n", elem->substGroupNs);
	else
	    fprintf(output, "\n");
    }
    if (elem->value != NULL)
	fprintf(output, "  default: %s", elem->value);
}

/**
 * xmlSchemaAnnotDump:
 * @output:  the file output
 * @annot:  a annotation
 *
 * Dump the annotation
 */
static void
xmlSchemaAnnotDump(FILE * output, xmlSchemaAnnotPtr annot)
{
    xmlChar *content;

    if (annot == NULL)
        return;

    content = xmlNodeGetContent(annot->content);
    if (content != NULL) {
        fprintf(output, "  Annot: %s\n", content);
        xmlFree(content);
    } else
        fprintf(output, "  Annot: empty\n");
}

/**
 * xmlSchemaTypeDump:
 * @output:  the file output
 * @type:  a type structure
 *
 * Dump a SchemaType structure
 */
static void
xmlSchemaTypeDump(xmlSchemaTypePtr type, FILE * output)
{
    if (type == NULL) {
        fprintf(output, "Type: NULL\n");
        return;
    }
    fprintf(output, "Type: ");
    if (type->name != NULL)
        fprintf(output, "%s, ", type->name);
    else
        fprintf(output, "no name");
    switch (type->type) {
        case XML_SCHEMA_TYPE_BASIC:
            fprintf(output, "basic ");
            break;
        case XML_SCHEMA_TYPE_SIMPLE:
            fprintf(output, "simple ");
            break;
        case XML_SCHEMA_TYPE_COMPLEX:
            fprintf(output, "complex ");
            break;
        case XML_SCHEMA_TYPE_SEQUENCE:
            fprintf(output, "sequence ");
            break;
        case XML_SCHEMA_TYPE_CHOICE:
            fprintf(output, "choice ");
            break;
        case XML_SCHEMA_TYPE_ALL:
            fprintf(output, "all ");
            break;
        case XML_SCHEMA_TYPE_UR:
            fprintf(output, "ur ");
            break;
        case XML_SCHEMA_TYPE_RESTRICTION:
            fprintf(output, "restriction ");
            break;
        case XML_SCHEMA_TYPE_EXTENSION:
            fprintf(output, "extension ");
            break;
        default:
            fprintf(output, "unknowntype%d ", type->type);
            break;
    }
    if (type->base != NULL) {
	fprintf(output, "base %s, ", type->base);
    }
    switch (type->contentType) {
	case XML_SCHEMA_CONTENT_UNKNOWN:
	    fprintf(output, "unknown ");
	    break;
	case XML_SCHEMA_CONTENT_EMPTY:
	    fprintf(output, "empty ");
	    break;
	case XML_SCHEMA_CONTENT_ELEMENTS:
	    fprintf(output, "element ");
	    break;
	case XML_SCHEMA_CONTENT_MIXED:
	    fprintf(output, "mixed ");
	    break;
	case XML_SCHEMA_CONTENT_MIXED_OR_ELEMENTS:
	    fprintf(output, "mixed_or_elems ");
	    break;
	case XML_SCHEMA_CONTENT_BASIC:
	    fprintf(output, "basic ");
	    break;
	case XML_SCHEMA_CONTENT_SIMPLE:
	    fprintf(output, "simple ");
	    break;
	case XML_SCHEMA_CONTENT_ANY:
	    fprintf(output, "any ");
	    break;
    }
    fprintf(output, "\n");
    if ((type->minOccurs != 1) || (type->maxOccurs != 1)) {
	fprintf(output, "  ");
	if (type->minOccurs != 1)
	    fprintf(output, "min: %d ", type->minOccurs);
	if (type->maxOccurs >= UNBOUNDED)
	    fprintf(output, "max: unbounded\n");
	else if (type->maxOccurs != 1)
	    fprintf(output, "max: %d\n", type->maxOccurs);
	else
	    fprintf(output, "\n");
    }
    if (type->annot != NULL)
        xmlSchemaAnnotDump(output, type->annot);
    if (type->subtypes != NULL) {
	xmlSchemaTypePtr sub = type->subtypes;

	fprintf(output, "  subtypes: ");
	while (sub != NULL) {
	    fprintf(output, "%s ", sub->name);
	    sub = sub->next;
	}
	fprintf(output, "\n");
    }

}

/**
 * xmlSchemaDump:
 * @output:  the file output
 * @schema:  a schema structure
 *
 * Dump a Schema structure.
 */
void
xmlSchemaDump(FILE * output, xmlSchemaPtr schema)
{
    if (schema == NULL) {
        fprintf(output, "Schemas: NULL\n");
        return;
    }
    fprintf(output, "Schemas: ");
    if (schema->name != NULL)
        fprintf(output, "%s, ", schema->name);
    else
        fprintf(output, "no name, ");
    if (schema->targetNamespace != NULL)
        fprintf(output, "%s", schema->targetNamespace);
    else
        fprintf(output, "no target namespace");
    fprintf(output, "\n");
    if (schema->annot != NULL)
        xmlSchemaAnnotDump(output, schema->annot);

    xmlHashScan(schema->typeDecl, (xmlHashScanner) xmlSchemaTypeDump,
                output);
    xmlHashScanFull(schema->elemDecl,
	            (xmlHashScannerFull) xmlSchemaElementDump, output);
}

/************************************************************************
 * 									*
 * 			Parsing functions				*
 * 									*
 ************************************************************************/

/**
 * xmlSchemaGetType:
 * @schema:  the schemas context
 * @name:  the type name
 * @ns:  the type namespace
 *
 * Lookup a type in the schemas or the predefined types
 *
 * Returns 1 if the string is NULL or made of blanks chars, 0 otherwise
 */
static xmlSchemaTypePtr
xmlSchemaGetType(xmlSchemaPtr schema, const xmlChar * name,
	         const xmlChar * namespace) {
    xmlSchemaTypePtr ret;

    if (name == NULL)
	return(NULL);
    if (schema != NULL) {
	ret = xmlHashLookup2(schema->typeDecl, name, namespace);
	if (ret != NULL)
	    return(ret);
    }
    ret = xmlSchemaGetPredefinedType(name, namespace);
#ifdef DEBUG
    if (ret == NULL) {
	if (namespace == NULL)
	    fprintf(stderr, "Unable to lookup type %s", name);
	else
	    fprintf(stderr, "Unable to lookup type %s:%s", name, namespace);
    }
#endif
    return(ret);
}

/************************************************************************
 * 									*
 * 			Parsing functions				*
 * 									*
 ************************************************************************/

#define IS_BLANK_NODE(n)						\
    (((n)->type == XML_TEXT_NODE) && (xmlSchemaIsBlank((n)->content)))

/**
 * xmlSchemaIsBlank:
 * @str:  a string
 *
 * Check if a string is ignorable
 *
 * Returns 1 if the string is NULL or made of blanks chars, 0 otherwise
 */
static int
xmlSchemaIsBlank(xmlChar *str) {
    if (str == NULL)
	return(1);
    while (*str != 0) {
	if (!(IS_BLANK(*str))) return(0);
	str++;
    }
    return(1);
}

/**
 * xmlSchemaAddNotation:
 * @ctxt:  a schema validation context
 * @schema:  the schema being built
 * @name:  the item name
 *
 * Add an XML schema Attrribute declaration
 * *WARNING* this interface is highly subject to change
 *
 * Returns the new struture or NULL in case of error
 */
static xmlSchemaNotationPtr
xmlSchemaAddNotation(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                      const xmlChar * name)
{
    xmlSchemaNotationPtr ret = NULL;
    int val;

    if ((ctxt == NULL) || (schema == NULL) || (name == NULL))
        return (NULL);

    if (schema->notaDecl == NULL)
        schema->notaDecl = xmlHashCreate(10);
    if (schema->notaDecl == NULL)
        return (NULL);

    ret = (xmlSchemaNotationPtr) xmlMalloc(sizeof(xmlSchemaNotation));
    if (ret == NULL) {
        if ((ctxt != NULL) && (ctxt->error != NULL))
            ctxt->error(ctxt->userData, "Out of memory\n");
        return (NULL);
    }
    memset(ret, 0, sizeof(xmlSchemaNotation));
    ret->name = xmlStrdup(name);
    val = xmlHashAddEntry2(schema->notaDecl, name, schema->targetNamespace,
                           ret);
    if (val != 0) {
        if ((ctxt != NULL) && (ctxt->error != NULL))
            ctxt->error(ctxt->userData, "Could not add notation %s\n",
                        name);
        xmlFree((char *) ret->name);
        xmlFree(ret);
        return (NULL);
    }
    return (ret);
}


/**
 * xmlSchemaAddAttribute:
 * @ctxt:  a schema validation context
 * @schema:  the schema being built
 * @name:  the item name
 * @container:  the container's name
 *
 * Add an XML schema Attrribute declaration
 * *WARNING* this interface is highly subject to change
 *
 * Returns the new struture or NULL in case of error
 */
static xmlSchemaAttributePtr
xmlSchemaAddAttribute(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                      const xmlChar * name)
{
    xmlSchemaAttributePtr ret = NULL;
    int val;

    if ((ctxt == NULL) || (schema == NULL) || (name == NULL))
        return (NULL);

    if (schema->attrDecl == NULL)
        schema->attrDecl = xmlHashCreate(10);
    if (schema->attrDecl == NULL)
        return (NULL);

    ret = (xmlSchemaAttributePtr) xmlMalloc(sizeof(xmlSchemaAttribute));
    if (ret == NULL) {
        if ((ctxt != NULL) && (ctxt->error != NULL))
            ctxt->error(ctxt->userData, "Out of memory\n");
        return (NULL);
    }
    memset(ret, 0, sizeof(xmlSchemaAttribute));
    ret->name = xmlStrdup(name);
    val = xmlHashAddEntry3(schema->attrDecl, name,
	                   schema->targetNamespace, ctxt->container, ret);
    if (val != 0) {
        if ((ctxt != NULL) && (ctxt->error != NULL))
            ctxt->error(ctxt->userData, "Could not add attribute %s\n",
                        name);
        xmlFree((char *) ret->name);
        xmlFree(ret);
        return (NULL);
    }
    return (ret);
}

/**
 * xmlSchemaAddAttributeGroup:
 * @ctxt:  a schema validation context
 * @schema:  the schema being built
 * @name:  the item name
 *
 * Add an XML schema Attrribute Group declaration
 *
 * Returns the new struture or NULL in case of error
 */
static xmlSchemaAttributeGroupPtr
xmlSchemaAddAttributeGroup(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                           const xmlChar * name)
{
    xmlSchemaAttributeGroupPtr ret = NULL;
    int val;

    if ((ctxt == NULL) || (schema == NULL) || (name == NULL))
        return (NULL);

    if (schema->attrgrpDecl == NULL)
        schema->attrgrpDecl = xmlHashCreate(10);
    if (schema->attrgrpDecl == NULL)
        return (NULL);

    ret = (xmlSchemaAttributeGroupPtr) xmlMalloc(sizeof(xmlSchemaAttributeGroup));
    if (ret == NULL) {
        if ((ctxt != NULL) && (ctxt->error != NULL))
            ctxt->error(ctxt->userData, "Out of memory\n");
        return (NULL);
    }
    memset(ret, 0, sizeof(xmlSchemaAttributeGroup));
    ret->name = xmlStrdup(name);
    val = xmlHashAddEntry3(schema->attrgrpDecl, name,
	                   schema->targetNamespace, ctxt->container, ret);
    if (val != 0) {
        if ((ctxt != NULL) && (ctxt->error != NULL))
            ctxt->error(ctxt->userData, "Could not add attribute group %s\n",
                        name);
        xmlFree((char *) ret->name);
        xmlFree(ret);
        return (NULL);
    }
    return (ret);
}

/**
 * xmlSchemaAddElement:
 * @ctxt:  a schema validation context
 * @schema:  the schema being built
 * @name:  the type name
 * @namespace:  the type namespace
 *
 * Add an XML schema Element declaration
 * *WARNING* this interface is highly subject to change
 *
 * Returns the new struture or NULL in case of error
 */
static xmlSchemaElementPtr
xmlSchemaAddElement(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                    const xmlChar * name, const xmlChar * namespace)
{
    xmlSchemaElementPtr ret = NULL;
    int val;

    if ((ctxt == NULL) || (schema == NULL) || (name == NULL))
        return (NULL);

    if (schema->elemDecl == NULL)
        schema->elemDecl = xmlHashCreate(10);
    if (schema->elemDecl == NULL)
        return (NULL);

    ret = (xmlSchemaElementPtr) xmlMalloc(sizeof(xmlSchemaElement));
    if (ret == NULL) {
        if ((ctxt != NULL) && (ctxt->error != NULL))
            ctxt->error(ctxt->userData, "Out of memory\n");
        return (NULL);
    }
    memset(ret, 0, sizeof(xmlSchemaElement));
    ret->name = xmlStrdup(name);
    val = xmlHashAddEntry3(schema->elemDecl, name,
	                   namespace, ctxt->container, ret);
    if (val != 0) {
	char buf[100];

	snprintf(buf, 99, "privatieelem%d", ctxt->counter++ + 1);
	val = xmlHashAddEntry3(schema->elemDecl, name, (xmlChar *) buf,
			       namespace, ret);
	if (val != 0) {
	    if ((ctxt != NULL) && (ctxt->error != NULL))
		ctxt->error(ctxt->userData, "Could not add element %s\n",
			    name);
	    xmlFree((char *) ret->name);
	    xmlFree(ret);
	    return (NULL);
	}
    }
    return (ret);
}

/**
 * xmlSchemaAddType:
 * @ctxt:  a schema validation context
 * @schema:  the schema being built
 * @name:  the item name
 *
 * Add an XML schema Simple Type definition
 * *WARNING* this interface is highly subject to change
 *
 * Returns the new struture or NULL in case of error
 */
static xmlSchemaTypePtr
xmlSchemaAddType(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                 const xmlChar * name)
{
    xmlSchemaTypePtr ret = NULL;
    int val;

    if ((ctxt == NULL) || (schema == NULL) || (name == NULL))
        return (NULL);

    if (schema->typeDecl == NULL)
        schema->typeDecl = xmlHashCreate(10);
    if (schema->typeDecl == NULL)
        return (NULL);

    ret = (xmlSchemaTypePtr) xmlMalloc(sizeof(xmlSchemaType));
    if (ret == NULL) {
        if ((ctxt != NULL) && (ctxt->error != NULL))
            ctxt->error(ctxt->userData, "Out of memory\n");
        return (NULL);
    }
    memset(ret, 0, sizeof(xmlSchemaType));
    ret->name = xmlStrdup(name);
    val = xmlHashAddEntry2(schema->typeDecl, name, schema->targetNamespace,
                           ret);
    if (val != 0) {
        if ((ctxt != NULL) && (ctxt->error != NULL))
            ctxt->error(ctxt->userData, "Could not add type %s\n", name);
        xmlFree((char *) ret->name);
        xmlFree(ret);
        return (NULL);
    }
    ret->minOccurs = 1;
    ret->maxOccurs = 1;

    return (ret);
}

/************************************************************************
 * 									*
 *		Utilities for parsing					*
 * 									*
 ************************************************************************/

/**
 * xmlGetQNameProp:
 * @ctxt:  a schema validation context
 * @node:  a subtree containing XML Schema informations
 * @name:  the attribute name
 * @namespace:  the result namespace if any
 *
 * Extract a QName Attribute value
 *
 * Returns the NCName or NULL if not found, and also update @namespace
 *    with the namespace URI
 */
static xmlChar *
xmlGetQNameProp(xmlSchemaParserCtxtPtr ctxt, xmlNodePtr node,
	        const char *name,
		xmlChar **namespace) {
    xmlChar *val, *ret, *prefix;
    xmlNsPtr ns;


    if (namespace != NULL)
	*namespace = NULL;
    val = xmlGetProp(node, (const xmlChar *) name);
    if (val == NULL)
	return(NULL);

    ret = xmlSplitQName2(val, &prefix);
    if (ret == NULL)
	return(val);
    xmlFree(val);

    ns = xmlSearchNs(node->doc, node, prefix);
    if (ns == NULL) {
	xmlSchemaErrorContext(ctxt, NULL, node, NULL);
        if ((ctxt != NULL) && (ctxt->error != NULL))
            ctxt->error(ctxt->userData,
		    "Attribute %s: the QName prefix %s is undefined\n",
		        name, prefix);
    } else {
	*namespace = xmlStrdup(ns->href);
    }
    xmlFree(prefix);
    return(ret);
}

/**
 * xmlGetMaxOccurs:
 * @ctxt:  a schema validation context
 * @node:  a subtree containing XML Schema informations
 *
 * Get the maxOccurs property
 *
 * Returns the default if not found, or the value
 */
static int
xmlGetMaxOccurs(xmlSchemaParserCtxtPtr ctxt, xmlNodePtr node) {
    xmlChar *val, *cur;
    int ret = 0;

    val = xmlGetProp(node, (const xmlChar *) "maxOccurs");
    if (val == NULL)
	return(1);

    if (xmlStrEqual(val, (const xmlChar *) "unbounded")) {
	xmlFree(val);
	return(UNBOUNDED); /* encoding it with -1 might be another option */
    }

    cur = val;
    while (IS_BLANK(*cur)) cur++;
    while ((*cur >= '0') && (*cur <= '9')) {
	ret = ret * 10 + (*cur - '0');
	cur++;
    }
    while (IS_BLANK(*cur)) cur++;
    if (*cur != 0) {
	xmlSchemaErrorContext(ctxt, NULL, node, NULL);
        if ((ctxt != NULL) && (ctxt->error != NULL))
            ctxt->error(ctxt->userData, "invalid value for minOccurs: %s\n",
		        val);
	xmlFree(val);
	return(1);
    }
    xmlFree(val);
    return(ret);
}

/**
 * xmlGetMinOccurs:
 * @ctxt:  a schema validation context
 * @node:  a subtree containing XML Schema informations
 *
 * Get the minOccurs property
 *
 * Returns the default if not found, or the value
 */
static int
xmlGetMinOccurs(xmlSchemaParserCtxtPtr ctxt, xmlNodePtr node) {
    xmlChar *val, *cur;
    int ret = 0;

    val = xmlGetProp(node, (const xmlChar *) "minOccurs");
    if (val == NULL)
	return(1);

    cur = val;
    while (IS_BLANK(*cur)) cur++;
    while ((*cur >= '0') && (*cur <= '9')) {
	ret = ret * 10 + (*cur - '0');
	cur++;
    }
    while (IS_BLANK(*cur)) cur++;
    if (*cur != 0) {
	xmlSchemaErrorContext(ctxt, NULL, node, NULL);
        if ((ctxt != NULL) && (ctxt->error != NULL))
            ctxt->error(ctxt->userData, "invalid value for minOccurs: %s\n",
		        val);
	xmlFree(val);
	return(1);
    }
    xmlFree(val);
    return(ret);
}

/**
 * xmlGetBooleanProp:
 * @ctxt:  a schema validation context
 * @node:  a subtree containing XML Schema informations
 * @name:  the attribute name
 * @def:  the default value
 *
 * Get is a bolean property is set
 *
 * Returns the default if not found, 0 if found to be false,
 *         1 if found to be true
 */
static int
xmlGetBooleanProp(xmlSchemaParserCtxtPtr ctxt, xmlNodePtr node,
	          const char *name, int def) {
    xmlChar *val;

    val = xmlGetProp(node, (const xmlChar *) name);
    if (val == NULL)
	return(def);

    if (xmlStrEqual(val, BAD_CAST"true"))
	def = 1;
    else if (xmlStrEqual(val, BAD_CAST"false"))
	def = 0;
    else {
	xmlSchemaErrorContext(ctxt, NULL, node, NULL);
        if ((ctxt != NULL) && (ctxt->error != NULL))
            ctxt->error(ctxt->userData,
		    "Attribute %s: the value %s is not boolean\n",
		        name, val);
    }
    xmlFree(val);
    return(def);
}

/************************************************************************
 * 									*
 *		Shema extraction from an Infoset			*
 * 									*
 ************************************************************************/
static xmlSchemaTypePtr xmlSchemaParseSimpleType(xmlSchemaParserCtxtPtr
                                                 ctxt, xmlSchemaPtr schema,
                                                 xmlNodePtr node);
static xmlSchemaTypePtr xmlSchemaParseComplexType(xmlSchemaParserCtxtPtr ctxt,
                                                  xmlSchemaPtr schema,
                                                  xmlNodePtr node);
static xmlSchemaTypePtr xmlSchemaParseRestriction(xmlSchemaParserCtxtPtr ctxt,
                                                  xmlSchemaPtr schema,
                                                  xmlNodePtr node,
						  int simple);
static xmlSchemaTypePtr xmlSchemaParseSequence(xmlSchemaParserCtxtPtr ctxt,
                                               xmlSchemaPtr schema,
                                               xmlNodePtr node);
static xmlSchemaTypePtr xmlSchemaParseAll(xmlSchemaParserCtxtPtr ctxt,
                                          xmlSchemaPtr schema,
                                          xmlNodePtr node);
static xmlSchemaAttributePtr xmlSchemaParseAttribute(xmlSchemaParserCtxtPtr
                                                     ctxt,
                                                     xmlSchemaPtr schema,
                                                     xmlNodePtr node);
static xmlSchemaAttributeGroupPtr
xmlSchemaParseAttributeGroup(xmlSchemaParserCtxtPtr ctxt,
                             xmlSchemaPtr schema, xmlNodePtr node);
static xmlSchemaTypePtr xmlSchemaParseChoice(xmlSchemaParserCtxtPtr ctxt,
                                             xmlSchemaPtr schema,
                                             xmlNodePtr node);
static xmlSchemaTypePtr xmlSchemaParseList(xmlSchemaParserCtxtPtr ctxt,
                                           xmlSchemaPtr schema,
                                           xmlNodePtr node);
static xmlSchemaAttributePtr
xmlSchemaParseAnyAttribute(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                           xmlNodePtr node);

/**
 * xmlSchemaParseAttrDecls:
 * @ctxt:  a schema validation context
 * @schema:  the schema being built
 * @node:  a subtree containing XML Schema informations
 * @type:  the hosting type
 *
 * parse a XML schema attrDecls declaration corresponding to
 * <!ENTITY % attrDecls  
 *       '((%attribute;| %attributeGroup;)*,(%anyAttribute;)?)'>
 */
static xmlNodePtr
xmlSchemaParseAttrDecls(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                        xmlNodePtr child, xmlSchemaTypePtr type)
{
    xmlSchemaAttributePtr lastattr, attr;

    lastattr = NULL;
    while ((IS_SCHEMA(child, "attribute")) ||
	   (IS_SCHEMA(child, "attributeGroup"))) {
	attr = NULL;
	if (IS_SCHEMA(child, "attribute")) {
	    attr = xmlSchemaParseAttribute(ctxt, schema, child);
	} else if (IS_SCHEMA(child, "attributeGroup")) {
	    attr = (xmlSchemaAttributePtr)
		xmlSchemaParseAttributeGroup(ctxt, schema, child);
	}
	if (attr != NULL) {
	    if (lastattr == NULL) {
		type->attributes = attr;
		lastattr = attr
		          ;
	    } else {
		lastattr->next = attr;
		lastattr = attr;
	    }
	}
	child = child->next;
    }
    if (IS_SCHEMA(child, "anyAttribute")) {
	attr = xmlSchemaParseAnyAttribute(ctxt, schema, child);
	if (attr != NULL) {
	    if (lastattr == NULL) {
		type->attributes = attr;
		lastattr = attr
		          ;
	    } else {
		lastattr->next = attr;
		lastattr = attr;
	    }
	}
	child = child->next;
    }
    return(child);
}

/**
 * xmlSchemaParseAnnotation:
 * @ctxt:  a schema validation context
 * @schema:  the schema being built
 * @node:  a subtree containing XML Schema informations
 *
 * parse a XML schema Attrribute declaration
 * *WARNING* this interface is highly subject to change
 *
 * Returns -1 in case of error, 0 if the declaration is inproper and
 *         1 in case of success.
 */
static xmlSchemaAnnotPtr
xmlSchemaParseAnnotation(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                         xmlNodePtr node)
{
    xmlSchemaAnnotPtr ret;

    if ((ctxt == NULL) || (schema == NULL) || (node == NULL))
        return (NULL);
    ret = xmlSchemaNewAnnot(ctxt, node);

    return (ret);
}

/**
 * xmlSchemaParseFacet:
 * @ctxt:  a schema validation context
 * @schema:  the schema being built
 * @node:  a subtree containing XML Schema informations
 *
 * parse a XML schema Facet declaration
 * *WARNING* this interface is highly subject to change
 *
 * Returns the new type structure or NULL in case of error
 */
static xmlSchemaFacetPtr
xmlSchemaParseFacet(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                  xmlNodePtr node)
{
    xmlSchemaFacetPtr facet;
    xmlNodePtr child = NULL;
    xmlChar *value;

    if ((ctxt == NULL) || (schema == NULL) || (node == NULL))
        return (NULL);

    facet = xmlSchemaNewFacet(ctxt);
    if (facet == NULL)
	return (NULL);
    facet->node = node;
    value = xmlGetProp(node, (const xmlChar *) "value");
    if (value == NULL) {
	xmlSchemaErrorContext(ctxt, schema, node, child);
        if ((ctxt != NULL) && (ctxt->error != NULL))
            ctxt->error(ctxt->userData, "Facet %s has no value\n", node->name);
	xmlSchemaFreeFacet(facet);
        return (NULL);
    }
    if (IS_SCHEMA(node, "minInclusive"))  {
        facet->type = XML_SCHEMA_FACET_MININCLUSIVE;
    } else if (IS_SCHEMA(node, "minExclusive"))  {
        facet->type = XML_SCHEMA_FACET_MINEXCLUSIVE;
    } else if (IS_SCHEMA(node, "maxInclusive"))  {
        facet->type = XML_SCHEMA_FACET_MAXINCLUSIVE;
    } else if (IS_SCHEMA(node, "maxExclusive"))  {
        facet->type = XML_SCHEMA_FACET_MAXEXCLUSIVE;
    } else if (IS_SCHEMA(node, "totalDigits"))  {
        facet->type = XML_SCHEMA_FACET_TOTALDIGITS;
    } else if (IS_SCHEMA(node, "fractionDigits"))  {
        facet->type = XML_SCHEMA_FACET_FRACTIONDIGITS;
    } else if (IS_SCHEMA(node, "pattern"))  {
        facet->type = XML_SCHEMA_FACET_PATTERN;
    } else if (IS_SCHEMA(node, "enumeration"))  {
        facet->type = XML_SCHEMA_FACET_ENUMERATION;
    } else if (IS_SCHEMA(node, "whiteSpace"))  {
        facet->type = XML_SCHEMA_FACET_WHITESPACE;
    } else if (IS_SCHEMA(node, "length"))  {
        facet->type = XML_SCHEMA_FACET_LENGTH;
    } else if (IS_SCHEMA(node, "maxLength"))  {
        facet->type = XML_SCHEMA_FACET_MAXLENGTH;
    } else if (IS_SCHEMA(node, "minLength")) {
        facet->type = XML_SCHEMA_FACET_MINLENGTH;
    } else {
	xmlSchemaErrorContext(ctxt, schema, node, child);
        if ((ctxt != NULL) && (ctxt->error != NULL))
            ctxt->error(ctxt->userData, "Unknown facet type %s\n", node->name);
	xmlSchemaFreeFacet(facet);
	return(NULL);
    }
    facet->id = xmlGetProp(node, (const xmlChar *) "id");
    facet->value = value;
    child = node->children;

    if (IS_SCHEMA(child, "annotation")) {
	facet->annot = xmlSchemaParseAnnotation(ctxt, schema, child);
	child = child->next;
    }
    if (child != NULL) {
	xmlSchemaErrorContext(ctxt, schema, node, child);
	if ((ctxt != NULL) && (ctxt->error != NULL))
	    ctxt->error(ctxt->userData,
			"Facet %s has unexpected child content\n",
			node->name);
    }
    return (facet);
}

/**
 * xmlSchemaParseAny:
 * @ctxt:  a schema validation context
 * @schema:  the schema being built
 * @node:  a subtree containing XML Schema informations
 *
 * parse a XML schema Any declaration
 * *WARNING* this interface is highly subject to change
 *
 * Returns the new type structure or NULL in case of error
 */
static xmlSchemaTypePtr
xmlSchemaParseAny(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                  xmlNodePtr node)
{
    xmlSchemaTypePtr type;
    xmlNodePtr child = NULL;
    xmlChar name[30];

    if ((ctxt == NULL) || (schema == NULL) || (node == NULL))
        return (NULL);
    snprintf((char *)name, 30, "any %d", ctxt->counter++ + 1);
    type = xmlSchemaAddType(ctxt, schema, name);
    if (type == NULL)
	return (NULL);
    type->node = node;
    type->type = XML_SCHEMA_TYPE_ANY;
    child = node->children;
    type->minOccurs = xmlGetMinOccurs(ctxt, node);
    type->maxOccurs = xmlGetMaxOccurs(ctxt, node);

    if (IS_SCHEMA(child, "annotation")) {
	type->annot = xmlSchemaParseAnnotation(ctxt, schema, child);
	child = child->next;
    }
    if (child != NULL) {
	xmlSchemaErrorContext(ctxt, schema, node, child);
	if ((ctxt != NULL) && (ctxt->error != NULL))
	    ctxt->error(ctxt->userData,
			"Sequence %s has unexpected content\n",
			type->name);
    }

    return (type);
}

/**
 * xmlSchemaParseNotation:
 * @ctxt:  a schema validation context
 * @schema:  the schema being built
 * @node:  a subtree containing XML Schema informations
 *
 * parse a XML schema Notation declaration
 *
 * Returns the new structure or NULL in case of error
 */
static xmlSchemaNotationPtr
xmlSchemaParseNotation(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                        xmlNodePtr node)
{
    xmlChar *name;
    xmlSchemaNotationPtr ret;
    xmlNodePtr child = NULL;

    if ((ctxt == NULL) || (schema == NULL) || (node == NULL))
        return (NULL);
    name = xmlGetProp(node, (const xmlChar *) "name");
    if (name == NULL) {
	xmlSchemaErrorContext(ctxt, schema, node, child);
        if ((ctxt != NULL) && (ctxt->error != NULL))
            ctxt->error(ctxt->userData, "Notation has no name\n");
        return (NULL);
    }
    ret = xmlSchemaAddNotation(ctxt, schema, name);
    if (ret == NULL) {
        xmlFree(name);
        return (NULL);
    }
    child = node->children;
    if (IS_SCHEMA(child, "annotation")) {
	ret->annot = xmlSchemaParseAnnotation(ctxt, schema, child);
	child = child->next;
    }
    if (child != NULL) {
	xmlSchemaErrorContext(ctxt, schema, node, child);
	if ((ctxt != NULL) && (ctxt->error != NULL))
	    ctxt->error(ctxt->userData,
			"notation %s has unexpected content\n",
			name);
    }

    return (ret);
}

/**
 * xmlSchemaParseAnyAttribute:
 * @ctxt:  a schema validation context
 * @schema:  the schema being built
 * @node:  a subtree containing XML Schema informations
 *
 * parse a XML schema AnyAttrribute declaration
 * *WARNING* this interface is highly subject to change
 *
 * Returns an attribute def structure or NULL
 */
static xmlSchemaAttributePtr
xmlSchemaParseAnyAttribute(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                        xmlNodePtr node)
{
    xmlChar *processContents;
    xmlSchemaAttributePtr ret;
    xmlNodePtr child = NULL;
    char name[100];

    if ((ctxt == NULL) || (schema == NULL) || (node == NULL))
        return (NULL);

    snprintf(name, 99, "anyattr %d", ctxt->counter++ + 1);
    ret = xmlSchemaAddAttribute(ctxt, schema, (xmlChar *)name);
    if (ret == NULL) {
        return (NULL);
    }
    ret->id = xmlGetProp(node, (const xmlChar *) "id");
    processContents = xmlGetProp(node, (const xmlChar *) "processContents");
    if ((processContents == NULL) ||
	(xmlStrEqual(processContents, (const xmlChar *)"strict"))) {
	ret->occurs = XML_SCHEMAS_ANYATTR_STRICT;
    } else if (xmlStrEqual(processContents, (const xmlChar *)"skip")) {
	ret->occurs = XML_SCHEMAS_ANYATTR_SKIP;
    } else if (xmlStrEqual(processContents, (const xmlChar *)"lax")) {
	ret->occurs = XML_SCHEMAS_ANYATTR_LAX;
    } else {
	xmlSchemaErrorContext(ctxt, schema, node, child);
	if ((ctxt != NULL) && (ctxt->error != NULL))
	    ctxt->error(ctxt->userData,
	    "anyAttribute has unexpected content for processContents: %s\n",
			processContents);
	ret->occurs = XML_SCHEMAS_ANYATTR_STRICT;
    }
    if (processContents != NULL)
	xmlFree(processContents);

    child = node->children;
    if (IS_SCHEMA(child, "annotation")) {
	ret->annot = xmlSchemaParseAnnotation(ctxt, schema, child);
	child = child->next;
    }
    if (child != NULL) {
	xmlSchemaErrorContext(ctxt, schema, node, child);
	if ((ctxt != NULL) && (ctxt->error != NULL))
	    ctxt->error(ctxt->userData,
			"anyAttribute %s has unexpected content\n",
			name);
    }

    return (ret);
}


/**
 * xmlSchemaParseAttribute:
 * @ctxt:  a schema validation context
 * @schema:  the schema being built
 * @node:  a subtree containing XML Schema informations
 *
 * parse a XML schema Attrribute declaration
 * *WARNING* this interface is highly subject to change
 *
 * Returns -1 in case of error, 0 if the declaration is inproper and
 *         1 in case of success.
 */
static xmlSchemaAttributePtr
xmlSchemaParseAttribute(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                        xmlNodePtr node)
{
    xmlChar *name, *refNs = NULL, *ref = NULL;
    xmlSchemaAttributePtr ret;
    xmlNodePtr child = NULL;

    if ((ctxt == NULL) || (schema == NULL) || (node == NULL))
        return (NULL);
    name = xmlGetProp(node, (const xmlChar *) "name");
    if (name == NULL) {
	char buf[100];

	ref = xmlGetQNameProp(ctxt, node, "ref", &refNs);
	if (ref == NULL) {
	    xmlSchemaErrorContext(ctxt, schema, node, child);
	    if ((ctxt != NULL) && (ctxt->error != NULL))
		ctxt->error(ctxt->userData, "Attribute has no name nor ref\n");
	    return (NULL);
	}
	snprintf(buf, 99, "anonattr%d", ctxt->counter++ + 1);
	name = xmlStrdup((xmlChar *) buf);
    }
    ret = xmlSchemaAddAttribute(ctxt, schema, name);
    if (ret == NULL) {
        xmlFree(name);
	if (ref != NULL)
	    xmlFree(ref);
        return (NULL);
    }
    xmlFree(name);
    ret->ref = ref;
    ret->refNs = refNs;
    ret->typeName = xmlGetQNameProp(ctxt, node, "type", &(ret->typeNs));
    child = node->children;
    if (IS_SCHEMA(child, "annotation")) {
	ret->annot = xmlSchemaParseAnnotation(ctxt, schema, child);
	child = child->next;
    }
    if (IS_SCHEMA(child, "simpleType")) {
	ret->subtypes = xmlSchemaParseSimpleType(ctxt, schema, child);
	child = child->next;
    }
    if (child != NULL) {
	xmlSchemaErrorContext(ctxt, schema, node, child);
	if ((ctxt != NULL) && (ctxt->error != NULL))
	    ctxt->error(ctxt->userData,
			"attribute %s has unexpected content\n",
			name);
    }

    return (ret);
}

/**
 * xmlSchemaParseAttributeGroup:
 * @ctxt:  a schema validation context
 * @schema:  the schema being built
 * @node:  a subtree containing XML Schema informations
 *
 * parse a XML schema Attribute Group declaration
 * *WARNING* this interface is highly subject to change
 *
 * Returns the attribute group or NULL in case of error.
 */
static xmlSchemaAttributeGroupPtr
xmlSchemaParseAttributeGroup(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                             xmlNodePtr node)
{
    xmlChar *name, *refNs = NULL, *ref = NULL;
    xmlSchemaAttributeGroupPtr ret;
    xmlSchemaAttributePtr last = NULL, attr;
    xmlNodePtr child = NULL;
    xmlChar *oldcontainer;

    if ((ctxt == NULL) || (schema == NULL) || (node == NULL))
        return (NULL);
    oldcontainer = ctxt->container;
    name = xmlGetProp(node, (const xmlChar *) "name");
    if (name == NULL) {
	char buf[100];

	ref = xmlGetQNameProp(ctxt, node, "ref", &refNs);
	if (ref == NULL) {
	    xmlSchemaErrorContext(ctxt, schema, node, child);
	    if ((ctxt != NULL) && (ctxt->error != NULL))
		ctxt->error(ctxt->userData,
			"AttributeGroup has no name nor ref\n");
	    return (NULL);
	}
	snprintf(buf, 99, "anonattrgroup%d", ctxt->counter++ + 1);
	name = xmlStrdup((xmlChar *) buf);
    }
    ret = xmlSchemaAddAttributeGroup(ctxt, schema, name);
    if (ret == NULL) {
        xmlFree(name);
	if (ref != NULL)
	    xmlFree(ref);
        return (NULL);
    }
    ret->ref = ref;
    ret->refNs = refNs;
    ret->type = XML_SCHEMA_TYPE_ATTRIBUTEGROUP;
    child = node->children;
    ctxt->container = name;
    if (IS_SCHEMA(child, "annotation")) {
	ret->annot = xmlSchemaParseAnnotation(ctxt, schema, child);
	child = child->next;
    }
    while ((IS_SCHEMA(child, "attribute")) ||
	   (IS_SCHEMA(child, "attributeGroup"))) {
	attr = NULL;
	if (IS_SCHEMA(child, "attribute")) {
	    attr = xmlSchemaParseAttribute(ctxt, schema, child);
	} else if (IS_SCHEMA(child, "attributeGroup")) {
	    attr = (xmlSchemaAttributePtr)
		xmlSchemaParseAttributeGroup(ctxt, schema, child);
	}
	if (attr != NULL) {
	    if (last == NULL) {
		ret->attributes = attr;
		last = attr;
	    } else {
		last->next = attr;
		last = attr;
	    }
	}
	child = child->next;
    }
    if (IS_SCHEMA(child, "anyAttribute")) {
	TODO
	child = child->next;
    }
    if (child != NULL) {
	xmlSchemaErrorContext(ctxt, schema, node, child);
	if ((ctxt != NULL) && (ctxt->error != NULL))
	    ctxt->error(ctxt->userData,
			"attribute group %s has unexpected content\n",
			name);
    }

    ctxt->container = oldcontainer;
    return (ret);
}

/**
 * xmlSchemaParseElement:
 * @ctxt:  a schema validation context
 * @schema:  the schema being built
 * @node:  a subtree containing XML Schema informations
 *
 * parse a XML schema Element declaration
 * *WARNING* this interface is highly subject to change
 *
 * Returns -1 in case of error, 0 if the declaration is inproper and
 *         1 in case of success.
 */
static xmlSchemaElementPtr
xmlSchemaParseElement(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                      xmlNodePtr node, int toplevel)
{
    xmlChar *name, *refNs = NULL, *ref = NULL, *namespace, *fixed;
    xmlSchemaElementPtr ret;
    xmlNodePtr child = NULL;
    xmlChar *oldcontainer;

    if ((ctxt == NULL) || (schema == NULL) || (node == NULL))
        return (NULL);
    oldcontainer = ctxt->container;
    name = xmlGetProp(node, (const xmlChar *) "name");
    if (name == NULL) {
	char buf[100];

	ref = xmlGetQNameProp(ctxt, node, "ref", &refNs);
	if (ref == NULL) {
	    xmlSchemaErrorContext(ctxt, schema, node, child);
	    if ((ctxt != NULL) && (ctxt->error != NULL))
		ctxt->error(ctxt->userData, "Element has no name nor ref\n");
	    return (NULL);
	}
	snprintf(buf, 99, "anonelem%d", ctxt->counter++ + 1);
	name = xmlStrdup((xmlChar *) buf);
    }
    namespace = xmlGetProp(node, (const xmlChar *) "targetNamespace");
    if (namespace == NULL)
        ret =
            xmlSchemaAddElement(ctxt, schema, name,
                                schema->targetNamespace);
    else
        ret = xmlSchemaAddElement(ctxt, schema, name, namespace);
    if (namespace != NULL)
        xmlFree(namespace);
    if (ret == NULL) {
	xmlFree(name);
	if (ref != NULL)
	    xmlFree(ref);
        return (NULL);
    }
    ret->type = XML_SCHEMA_TYPE_ELEMENT;
    ret->ref = ref;
    ret->refNs = refNs;
    if (ref != NULL)
        ret->flags |= XML_SCHEMAS_ELEM_REF;
    if (toplevel)
        ret->flags |= XML_SCHEMAS_ELEM_TOPLEVEL;
    if (xmlGetBooleanProp(ctxt, node, "nillable", 0))
        ret->flags |= XML_SCHEMAS_ELEM_NILLABLE;
    if (xmlGetBooleanProp(ctxt, node, "abstract", 0))
        ret->flags |= XML_SCHEMAS_ELEM_NILLABLE;
    ctxt->container = name;

    ret->id = xmlGetProp(node, BAD_CAST "id");
    ret->namedType = xmlGetQNameProp(ctxt, node, "type", &(ret->namedTypeNs));
    ret->substGroup = xmlGetQNameProp(ctxt, node, "substitutionGroup",
	                              &(ret->substGroupNs));
    fixed = xmlGetProp(node, BAD_CAST "fixed");
    ret->minOccurs = xmlGetMinOccurs(ctxt, node);
    ret->maxOccurs = xmlGetMaxOccurs(ctxt, node);
    
    ret->value = xmlGetProp(node, BAD_CAST "default");
    if ((ret->value != NULL) && (fixed != NULL)) {
	xmlSchemaErrorContext(ctxt, schema, node, child);
	ctxt->error(ctxt->userData,
		    "Element %s has both default and fixed\n",
		    ret->name);
	xmlFree(fixed);
    } else if (fixed != NULL) {
        ret->flags |= XML_SCHEMAS_ELEM_FIXED;
	ret->value = fixed;
    }

    child = node->children;
    if (IS_SCHEMA(child, "annotation")) {
	ret->annot = xmlSchemaParseAnnotation(ctxt, schema, child);
	child = child->next;
    }
    if (IS_SCHEMA(child, "complexType")) {
	ret->subtypes = xmlSchemaParseComplexType(ctxt, schema, child);
        child = child->next;
    } else if (IS_SCHEMA(child, "simpleType")) {
	ret->subtypes = xmlSchemaParseSimpleType(ctxt, schema, child);
        child = child->next;
    }
    while ((IS_SCHEMA(child, "unique")) ||
	   (IS_SCHEMA(child, "key")) ||
	   (IS_SCHEMA(child, "keyref"))) {
	TODO
	child = child->next;
    }
    if (child != NULL) {
	xmlSchemaErrorContext(ctxt, schema, node, child);
	if ((ctxt != NULL) && (ctxt->error != NULL))
	    ctxt->error(ctxt->userData,
			"element %s has unexpected content\n",
			name);
    }

    ctxt->container = oldcontainer;
    xmlFree(name);
    return (ret);
}

/**
 * xmlSchemaParseUnion:
 * @ctxt:  a schema validation context
 * @schema:  the schema being built
 * @node:  a subtree containing XML Schema informations
 *
 * parse a XML schema Union definition
 * *WARNING* this interface is highly subject to change
 *
 * Returns -1 in case of error, 0 if the declaration is inproper and
 *         1 in case of success.
 */
static xmlSchemaTypePtr
xmlSchemaParseUnion(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                         xmlNodePtr node)
{
    xmlSchemaTypePtr type, subtype, last = NULL;
    xmlNodePtr child = NULL;
    xmlChar name[30];

    if ((ctxt == NULL) || (schema == NULL) || (node == NULL))
        return (NULL);


    snprintf((char *)name, 30, "union %d", ctxt->counter++ + 1);
    type = xmlSchemaAddType(ctxt, schema, name);
    if (type == NULL)
        return (NULL);
    type->node = node;
    type->type = XML_SCHEMA_TYPE_LIST;
    type->id = xmlGetProp(node, BAD_CAST "id");
    type->ref = xmlGetProp(node, BAD_CAST "memberTypes");

    child = node->children;
    if (IS_SCHEMA(child, "annotation")) {
	type->annot = xmlSchemaParseAnnotation(ctxt, schema, child);
	child = child->next;
    }
    while (IS_SCHEMA(child, "simpleType")) {
	subtype = (xmlSchemaTypePtr) 
	      xmlSchemaParseSimpleType(ctxt, schema, child);
	if (subtype != NULL) {
	    if (last == NULL) {
		type->subtypes = subtype;
		last = subtype;
	    } else {
		last->next = subtype;
		last = subtype;
	    }
	    last->next = NULL;
	}
	child = child->next;
    }
    if (child != NULL) {
	xmlSchemaErrorContext(ctxt, schema, node, child);
	if ((ctxt != NULL) && (ctxt->error != NULL))
	    ctxt->error(ctxt->userData,
			"Union %s has unexpected content\n",
			type->name);
    }
    return (type);
}

/**
 * xmlSchemaParseList:
 * @ctxt:  a schema validation context
 * @schema:  the schema being built
 * @node:  a subtree containing XML Schema informations
 *
 * parse a XML schema List definition
 * *WARNING* this interface is highly subject to change
 *
 * Returns -1 in case of error, 0 if the declaration is inproper and
 *         1 in case of success.
 */
static xmlSchemaTypePtr
xmlSchemaParseList(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                         xmlNodePtr node)
{
    xmlSchemaTypePtr type, subtype;
    xmlNodePtr child = NULL;
    xmlChar name[30];

    if ((ctxt == NULL) || (schema == NULL) || (node == NULL))
        return (NULL);

    snprintf((char *)name, 30, "list %d", ctxt->counter++ + 1);
    type = xmlSchemaAddType(ctxt, schema, name);
    if (type == NULL)
        return (NULL);
    type->node = node;
    type->type = XML_SCHEMA_TYPE_LIST;
    type->id = xmlGetProp(node, BAD_CAST "id");
    type->ref = xmlGetQNameProp(ctxt, node, "ref", &(type->refNs));

    child = node->children;
    if (IS_SCHEMA(child, "annotation")) {
	type->annot = xmlSchemaParseAnnotation(ctxt, schema, child);
	child = child->next;
    }
    subtype = NULL;
    if (IS_SCHEMA(child, "simpleType")) {
	subtype = (xmlSchemaTypePtr) 
	      xmlSchemaParseSimpleType(ctxt, schema, child);
	child = child->next;
	type->subtypes = subtype;
    }
    if (child != NULL) {
	xmlSchemaErrorContext(ctxt, schema, node, child);
	if ((ctxt != NULL) && (ctxt->error != NULL))
	    ctxt->error(ctxt->userData,
			"List %s has unexpected content\n",
			type->name);
    }
    return (type);
}
/**
 * xmlSchemaParseSimpleType:
 * @ctxt:  a schema validation context
 * @schema:  the schema being built
 * @node:  a subtree containing XML Schema informations
 *
 * parse a XML schema Simple Type definition
 * *WARNING* this interface is highly subject to change
 *
 * Returns -1 in case of error, 0 if the declaration is inproper and
 *         1 in case of success.
 */
static xmlSchemaTypePtr
xmlSchemaParseSimpleType(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                         xmlNodePtr node)
{
    xmlSchemaTypePtr type, subtype;
    xmlNodePtr child = NULL;
    xmlChar *name;

    if ((ctxt == NULL) || (schema == NULL) || (node == NULL))
        return (NULL);


    name = xmlGetProp(node, (const xmlChar *) "name");
    if (name == NULL) {
	char buf[100];

	snprintf(buf, 99, "simpletype%d", ctxt->counter++ + 1);
	name = xmlStrdup((xmlChar *) buf);
    }
    if (name == NULL) {
	xmlSchemaErrorContext(ctxt, schema, node, child);
        if ((ctxt != NULL) && (ctxt->error != NULL))
            ctxt->error(ctxt->userData, "simpleType has no name\n");
        return (NULL);
    }
    type = xmlSchemaAddType(ctxt, schema, name);
    xmlFree(name);
    if (type == NULL)
        return (NULL);
    type->node = node;
    type->type = XML_SCHEMA_TYPE_SIMPLE;
    type->id = xmlGetProp(node, BAD_CAST "id");

    child = node->children;
    if (IS_SCHEMA(child, "annotation")) {
	type->annot = xmlSchemaParseAnnotation(ctxt, schema, child);
	child = child->next;
    }
    subtype = NULL;
    if (IS_SCHEMA(child, "restriction")) {
	subtype = (xmlSchemaTypePtr) 
	      xmlSchemaParseRestriction(ctxt, schema, child, 1);
	child = child->next;
    } else if (IS_SCHEMA(child, "list")) {
	subtype = (xmlSchemaTypePtr) 
	      xmlSchemaParseList(ctxt, schema, child);
	child = child->next;
    } else if (IS_SCHEMA(child, "union")) {
	subtype = (xmlSchemaTypePtr) 
	      xmlSchemaParseUnion(ctxt, schema, child);
	child = child->next;
    }
    type->subtypes = subtype;
    if (child != NULL) {
	xmlSchemaErrorContext(ctxt, schema, node, child);
	if ((ctxt != NULL) && (ctxt->error != NULL))
	    ctxt->error(ctxt->userData,
			"SimpleType %s has unexpected content\n",
			type->name);
    }

    return (type);
}


/**
 * xmlSchemaParseGroup:
 * @ctxt:  a schema validation context
 * @schema:  the schema being built
 * @node:  a subtree containing XML Schema informations
 *
 * parse a XML schema Group definition
 * *WARNING* this interface is highly subject to change
 *
 * Returns -1 in case of error, 0 if the declaration is inproper and
 *         1 in case of success.
 */
static xmlSchemaTypePtr
xmlSchemaParseGroup(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                          xmlNodePtr node)
{
    xmlSchemaTypePtr type, subtype;
    xmlNodePtr child = NULL;
    xmlChar *name, *ref = NULL, *refNs = NULL;

    if ((ctxt == NULL) || (schema == NULL) || (node == NULL))
        return (NULL);


    name = xmlGetProp(node, (const xmlChar *) "name");
    if (name == NULL) {
	char buf[100];

	ref = xmlGetQNameProp(ctxt, node, "ref", &refNs);
	if (ref == NULL) {
	    xmlSchemaErrorContext(ctxt, schema, node, child);
	    if ((ctxt != NULL) && (ctxt->error != NULL))
		ctxt->error(ctxt->userData, "Group has no name nor ref\n");
	    return (NULL);
	}
	snprintf(buf, 99, "anongroup%d", ctxt->counter++ + 1);
	name = xmlStrdup((xmlChar *) buf);
    }
    type = xmlSchemaAddType(ctxt, schema, name);
    if (type == NULL)
        return (NULL);
    type->node = node;
    type->type = XML_SCHEMA_TYPE_GROUP;
    type->id = xmlGetProp(node, BAD_CAST "id");
    type->ref = ref;
    type->refNs = refNs;
    type->minOccurs = xmlGetMinOccurs(ctxt, node);
    type->maxOccurs = xmlGetMaxOccurs(ctxt, node);

    child = node->children;
    if (IS_SCHEMA(child, "annotation")) {
	type->annot = xmlSchemaParseAnnotation(ctxt, schema, child);
	child = child->next;
    }
    subtype = NULL;
    if (IS_SCHEMA(child, "all")) {
	subtype = (xmlSchemaTypePtr) 
	      xmlSchemaParseAll(ctxt, schema, child);
	child = child->next;
    } else if (IS_SCHEMA(child, "choice")) {
	subtype = xmlSchemaParseChoice(ctxt, schema, child);
	child = child->next;
    } else if (IS_SCHEMA(child, "sequence")) {
	subtype = (xmlSchemaTypePtr)
	      xmlSchemaParseSequence(ctxt, schema, child);
	child = child->next;
    }
    if (subtype != NULL)
	type->subtypes = subtype;
    if (child != NULL) {
	xmlSchemaErrorContext(ctxt, schema, node, child);
	if ((ctxt != NULL) && (ctxt->error != NULL))
	    ctxt->error(ctxt->userData,
			"Group %s has unexpected content\n",
			type->name);
    }

    return (type);
}

/**
 * xmlSchemaParseAll:
 * @ctxt:  a schema validation context
 * @schema:  the schema being built
 * @node:  a subtree containing XML Schema informations
 *
 * parse a XML schema All definition
 * *WARNING* this interface is highly subject to change
 *
 * Returns -1 in case of error, 0 if the declaration is inproper and
 *         1 in case of success.
 */
static xmlSchemaTypePtr
xmlSchemaParseAll(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                          xmlNodePtr node)
{
    xmlSchemaTypePtr type, subtype, last = NULL;
    xmlNodePtr child = NULL;
    xmlChar name[30];

    if ((ctxt == NULL) || (schema == NULL) || (node == NULL))
        return (NULL);


    snprintf((char *)name, 30, "all%d", ctxt->counter++ + 1);
    type = xmlSchemaAddType(ctxt, schema, name);
    if (type == NULL)
        return (NULL);
    type->node = node;
    type->type = XML_SCHEMA_TYPE_ALL;
    type->id = xmlGetProp(node, BAD_CAST "id");
    type->minOccurs = xmlGetMinOccurs(ctxt, node);
    type->maxOccurs = xmlGetMaxOccurs(ctxt, node);

    child = node->children;
    if (IS_SCHEMA(child, "annotation")) {
	type->annot = xmlSchemaParseAnnotation(ctxt, schema, child);
	child = child->next;
    }
    while (IS_SCHEMA(child, "element")) {
	subtype = (xmlSchemaTypePtr) 
	      xmlSchemaParseElement(ctxt, schema, child, 0);
	if (subtype != NULL) {
	    if (last == NULL) {
		type->subtypes = subtype;
		last = subtype;
	    } else {
		last->next = subtype;
		last = subtype;
	    }
	    last->next = NULL;
	}
	child = child->next;
    }
    if (child != NULL) {
	xmlSchemaErrorContext(ctxt, schema, node, child);
	if ((ctxt != NULL) && (ctxt->error != NULL))
	    ctxt->error(ctxt->userData,
			"All %s has unexpected content\n",
			type->name);
    }

    return (type);
}

/**
 * xmlSchemaParseImport:
 * @ctxt:  a schema validation context
 * @schema:  the schema being built
 * @node:  a subtree containing XML Schema informations
 *
 * parse a XML schema Import definition
 * *WARNING* this interface is highly subject to change
 *
 * Returns -1 in case of error, 0 if the declaration is inproper and
 *         1 in case of success.
 */
static int
xmlSchemaParseImport(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                          xmlNodePtr node)
{
    xmlNodePtr child = NULL;
    xmlChar *namespace;
    xmlChar *schemaLocation;
    xmlChar *previous;
    xmlURIPtr check;

    if ((ctxt == NULL) || (schema == NULL) || (node == NULL))
        return (-1);

    namespace = xmlGetProp(node, BAD_CAST "namespace");
    if (namespace != NULL) {
	check = xmlParseURI((const char *) namespace);
	if (check == NULL) {
	    xmlSchemaErrorContext(ctxt, schema, node, child);
	    if ((ctxt != NULL) && (ctxt->error != NULL))
		ctxt->error(ctxt->userData,
			    "Import namespace attribute is not an URI: %s\n",
			    namespace);
	    xmlFree(namespace);
	    return(-1);
	} else {
	    xmlFreeURI(check);
	}
    }
    schemaLocation = xmlGetProp(node, BAD_CAST "schemaLocation");
    if (schemaLocation != NULL) {
	check = xmlParseURI((const char *) schemaLocation);
	if (check == NULL) {
	    xmlSchemaErrorContext(ctxt, schema, node, child);
	    if ((ctxt != NULL) && (ctxt->error != NULL))
		ctxt->error(ctxt->userData,
		    "Import schemaLocation attribute is not an URI: %s\n",
			    schemaLocation);
	    if (namespace != NULL)
		xmlFree(namespace);
	    xmlFree(schemaLocation);
	    return(-1);
	} else {
	    xmlFreeURI(check);
	}
    }
    if (schema->schemasImports == NULL) {
	schema->schemasImports = xmlHashCreate(10);
	if (schema->schemasImports == NULL) {
	    xmlSchemaErrorContext(ctxt, schema, node, child);
	    if ((ctxt != NULL) && (ctxt->error != NULL))
		ctxt->error(ctxt->userData,
		    "Internal: failed to build import table\n");
	    if (namespace != NULL)
		xmlFree(namespace);
	    if (schemaLocation != NULL)
		xmlFree(schemaLocation);
	    return(-1);
	}
    }
    if (namespace == NULL) {
	previous = xmlHashLookup(schema->schemasImports,
		XML_SCHEMAS_DEFAULT_NAMESPACE);
	if (schemaLocation != NULL) {
	    if (previous != NULL) {
		if (!xmlStrEqual(schemaLocation, previous)) {
		    xmlSchemaErrorContext(ctxt, schema, node, child);
		    if ((ctxt != NULL) && (ctxt->error != NULL))
			ctxt->error(ctxt->userData,
	"Redefining import for default namespace with a different URI: %s\n",
	                            schemaLocation);
		}
	    } else {
		xmlHashAddEntry(schema->schemasImports, 
			XML_SCHEMAS_DEFAULT_NAMESPACE, schemaLocation);
	    }
	}
    } else {
	previous = xmlHashLookup(schema->schemasImports, namespace);
	if (schemaLocation != NULL) {
	    if (previous != NULL) {
		if (!xmlStrEqual(schemaLocation, previous)) {
		    xmlSchemaErrorContext(ctxt, schema, node, child);
		    if ((ctxt != NULL) && (ctxt->error != NULL))
			ctxt->error(ctxt->userData,
	"Redefining import for namespace %s with a different URI: %s\n",
	                            namespace, schemaLocation);
		}
	    } else {
		xmlHashAddEntry(schema->schemasImports, 
			namespace, schemaLocation);
	    }
	}
    }

    child = node->children;
    while (IS_SCHEMA(child, "annotation")) {
	/*
	 * the annotations here are simply discarded ...
	 */
	child = child->next;
    }
    if (child != NULL) {
	xmlSchemaErrorContext(ctxt, schema, node, child);
	if ((ctxt != NULL) && (ctxt->error != NULL))
	    ctxt->error(ctxt->userData,
			"Import has unexpected content\n");
	return(-1);
    }
    return(1);
}

/**
 * xmlSchemaParseChoice:
 * @ctxt:  a schema validation context
 * @schema:  the schema being built
 * @node:  a subtree containing XML Schema informations
 *
 * parse a XML schema Choice definition
 * *WARNING* this interface is highly subject to change
 *
 * Returns -1 in case of error, 0 if the declaration is inproper and
 *         1 in case of success.
 */
static xmlSchemaTypePtr
xmlSchemaParseChoice(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                          xmlNodePtr node)
{
    xmlSchemaTypePtr type, subtype, last = NULL;
    xmlNodePtr child = NULL;
    xmlChar name[30];

    if ((ctxt == NULL) || (schema == NULL) || (node == NULL))
        return (NULL);


    snprintf((char *)name, 30, "choice %d", ctxt->counter++ + 1);
    type = xmlSchemaAddType(ctxt, schema, name);
    if (type == NULL)
        return (NULL);
    type->node = node;
    type->type = XML_SCHEMA_TYPE_CHOICE;
    type->id = xmlGetProp(node, BAD_CAST "id");
    type->minOccurs = xmlGetMinOccurs(ctxt, node);
    type->maxOccurs = xmlGetMaxOccurs(ctxt, node);

    child = node->children;
    if (IS_SCHEMA(child, "annotation")) {
	type->annot = xmlSchemaParseAnnotation(ctxt, schema, child);
	child = child->next;
    }
    while ((IS_SCHEMA(child, "element")) ||
	   (IS_SCHEMA(child, "group")) ||
	   (IS_SCHEMA(child, "any")) ||
	   (IS_SCHEMA(child, "choice")) ||
	   (IS_SCHEMA(child, "sequence"))) {
	subtype = NULL;
	if (IS_SCHEMA(child, "element")) {
	    subtype = (xmlSchemaTypePtr) 
		  xmlSchemaParseElement(ctxt, schema, child, 0);
	} else if (IS_SCHEMA(child, "group")) {
	    subtype = xmlSchemaParseGroup(ctxt, schema, child);
	} else if (IS_SCHEMA(child, "any")) {
	    subtype = xmlSchemaParseAny(ctxt, schema, child);
	} else if (IS_SCHEMA(child, "sequence")) {
	    subtype = xmlSchemaParseSequence(ctxt, schema, child);
	} else if (IS_SCHEMA(child, "choice")) {
	    subtype = xmlSchemaParseChoice(ctxt, schema, child);
	}
	if (subtype != NULL) {
	    if (last == NULL) {
		type->subtypes = subtype;
		last = subtype;
	    } else {
		last->next = subtype;
		last = subtype;
	    }
	    last->next = NULL;
	}
	child = child->next;
    }
    if (child != NULL) {
	xmlSchemaErrorContext(ctxt, schema, node, child);
	if ((ctxt != NULL) && (ctxt->error != NULL))
	    ctxt->error(ctxt->userData,
			"Choice %s has unexpected content\n",
			type->name);
    }

    return (type);
}

/**
 * xmlSchemaParseSequence:
 * @ctxt:  a schema validation context
 * @schema:  the schema being built
 * @node:  a subtree containing XML Schema informations
 *
 * parse a XML schema Sequence definition
 * *WARNING* this interface is highly subject to change
 *
 * Returns -1 in case of error, 0 if the declaration is inproper and
 *         1 in case of success.
 */
static xmlSchemaTypePtr
xmlSchemaParseSequence(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                          xmlNodePtr node)
{
    xmlSchemaTypePtr type, subtype, last = NULL;
    xmlNodePtr child = NULL;
    xmlChar name[30];

    if ((ctxt == NULL) || (schema == NULL) || (node == NULL))
        return (NULL);


    snprintf((char *)name, 30, "sequence %d", ctxt->counter++ + 1);
    type = xmlSchemaAddType(ctxt, schema, name);
    if (type == NULL)
        return (NULL);
    type->node = node;
    type->type = XML_SCHEMA_TYPE_SEQUENCE;
    type->id = xmlGetProp(node, BAD_CAST "id");
    type->minOccurs = xmlGetMinOccurs(ctxt, node);
    type->maxOccurs = xmlGetMaxOccurs(ctxt, node);

    child = node->children;
    if (IS_SCHEMA(child, "annotation")) {
	type->annot = xmlSchemaParseAnnotation(ctxt, schema, child);
	child = child->next;
    }
    while ((IS_SCHEMA(child, "element")) ||
	   (IS_SCHEMA(child, "group")) ||
	   (IS_SCHEMA(child, "any")) ||
	   (IS_SCHEMA(child, "choice")) ||
	   (IS_SCHEMA(child, "sequence"))) {
	subtype = NULL;
	if (IS_SCHEMA(child, "element")) {
	    subtype = (xmlSchemaTypePtr) 
		  xmlSchemaParseElement(ctxt, schema, child, 0);
	} else if (IS_SCHEMA(child, "group")) {
	    subtype = xmlSchemaParseGroup(ctxt, schema, child);
	} else if (IS_SCHEMA(child, "any")) {
	    subtype = xmlSchemaParseAny(ctxt, schema, child);
	} else if (IS_SCHEMA(child, "choice")) {
	    subtype = xmlSchemaParseChoice(ctxt, schema, child);
	} else if (IS_SCHEMA(child, "sequence")) {
	    subtype = xmlSchemaParseSequence(ctxt, schema, child);
	}
	if (subtype != NULL) {
	    if (last == NULL) {
		type->subtypes = subtype;
		last = subtype;
	    } else {
		last->next = subtype;
		last = subtype;
	    }
	    last->next = NULL;
	}
	child = child->next;
    }
    if (child != NULL) {
	xmlSchemaErrorContext(ctxt, schema, node, child);
	if ((ctxt != NULL) && (ctxt->error != NULL))
	    ctxt->error(ctxt->userData,
			"Sequence %s has unexpected content\n",
			type->name);
    }

    return (type);
}

/**
 * xmlSchemaParseRestriction:
 * @ctxt:  a schema validation context
 * @schema:  the schema being built
 * @node:  a subtree containing XML Schema informations
 * @simple:  is that part of a simple type.
 *
 * parse a XML schema Restriction definition
 * *WARNING* this interface is highly subject to change
 *
 * Returns the type definition or NULL in case of error
 */
static xmlSchemaTypePtr
xmlSchemaParseRestriction(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                          xmlNodePtr node, int simple)
{
    xmlSchemaTypePtr type, subtype;
    xmlSchemaFacetPtr facet, lastfacet = NULL;
    xmlNodePtr child = NULL;
    xmlChar name[30];
    xmlChar *oldcontainer;

    if ((ctxt == NULL) || (schema == NULL) || (node == NULL))
        return (NULL);

    oldcontainer = ctxt->container;

    snprintf((char *)name, 30, "restriction %d", ctxt->counter++ + 1);
    type = xmlSchemaAddType(ctxt, schema, name);
    if (type == NULL)
        return (NULL);
    type->node = node;
    type->type = XML_SCHEMA_TYPE_RESTRICTION;
    type->id = xmlGetProp(node, BAD_CAST "id");
    type->base = xmlGetQNameProp(ctxt, node, "base", &(type->baseNs));
    if ((!simple) && (type->base == NULL)) {
	xmlSchemaErrorContext(ctxt, schema, node, child);
	if ((ctxt != NULL) && (ctxt->error != NULL))
	    ctxt->error(ctxt->userData,
			"Restriction %s has no base\n",
			type->name);
    }
    ctxt->container = name;

    child = node->children;
    if (IS_SCHEMA(child, "annotation")) {
	type->annot = xmlSchemaParseAnnotation(ctxt, schema, child);
	child = child->next;
    }
    subtype = NULL;

    if (IS_SCHEMA(child, "all")) {
	subtype = (xmlSchemaTypePtr) 
	      xmlSchemaParseAll(ctxt, schema, child);
	child = child->next;
	type->subtypes = subtype;
    } else if (IS_SCHEMA(child, "choice")) {
	subtype = xmlSchemaParseChoice(ctxt, schema, child);
	child = child->next;
	type->subtypes = subtype;
    } else if (IS_SCHEMA(child, "sequence")) {
	subtype = (xmlSchemaTypePtr) 
	      xmlSchemaParseSequence(ctxt, schema, child);
	child = child->next;
	type->subtypes = subtype;
    } else if (IS_SCHEMA(child, "group")) {
	subtype = (xmlSchemaTypePtr) 
	      xmlSchemaParseGroup(ctxt, schema, child);
	child = child->next;
	type->subtypes = subtype;
    } else {
	if (IS_SCHEMA(child, "simpleType")) {
	    subtype = (xmlSchemaTypePtr) 
		  xmlSchemaParseSimpleType(ctxt, schema, child);
	    child = child->next;
	    type->baseType = subtype;
	}
	/*
	 * Facets
	 */
        while ((IS_SCHEMA(child, "minInclusive")) ||
	       (IS_SCHEMA(child, "minExclusive")) ||
	       (IS_SCHEMA(child, "maxInclusive")) ||
	       (IS_SCHEMA(child, "maxExclusive")) ||
	       (IS_SCHEMA(child, "totalDigits")) ||
	       (IS_SCHEMA(child, "fractionDigits")) ||
	       (IS_SCHEMA(child, "pattern")) ||
	       (IS_SCHEMA(child, "enumeration")) ||
	       (IS_SCHEMA(child, "whiteSpace")) ||
	       (IS_SCHEMA(child, "length")) ||
	       (IS_SCHEMA(child, "maxLength")) ||
	       (IS_SCHEMA(child, "minLength"))) {
	    facet = xmlSchemaParseFacet(ctxt, schema, child);
	    if (facet != NULL) {
		if (lastfacet == NULL) {
		    type->facets = facet;
		    lastfacet = facet;
		} else {
		    lastfacet->next = facet;
		    lastfacet = facet;
		}
		lastfacet->next = NULL;
	    }
	    child = child->next;
	}
    }
    child = xmlSchemaParseAttrDecls(ctxt, schema, child, type);
    if (child != NULL) {
	xmlSchemaErrorContext(ctxt, schema, node, child);
	if ((ctxt != NULL) && (ctxt->error != NULL))
	    ctxt->error(ctxt->userData,
			"Restriction %s has unexpected content\n",
			type->name);
    }
    ctxt->container = oldcontainer;
    return (type);
}

/**
 * xmlSchemaParseExtension:
 * @ctxt:  a schema validation context
 * @schema:  the schema being built
 * @node:  a subtree containing XML Schema informations
 *
 * parse a XML schema Extension definition
 * *WARNING* this interface is highly subject to change
 *
 * Returns the type definition or NULL in case of error
 */
static xmlSchemaTypePtr
xmlSchemaParseExtension(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                          xmlNodePtr node)
{
    xmlSchemaTypePtr type, subtype;
    xmlNodePtr child = NULL;
    xmlChar name[30];
    xmlChar *oldcontainer;

    if ((ctxt == NULL) || (schema == NULL) || (node == NULL))
        return (NULL);

    oldcontainer = ctxt->container;

    snprintf((char *)name, 30, "extension %d", ctxt->counter++ + 1);
    type = xmlSchemaAddType(ctxt, schema, name);
    if (type == NULL)
        return (NULL);
    type->node = node;
    type->type = XML_SCHEMA_TYPE_EXTENSION;
    type->id = xmlGetProp(node, BAD_CAST "id");
    ctxt->container = name;

    type->base = xmlGetQNameProp(ctxt, node, "base", &(type->baseNs));
    if (type->base == NULL) {
	xmlSchemaErrorContext(ctxt, schema, node, child);
	if ((ctxt != NULL) && (ctxt->error != NULL))
	    ctxt->error(ctxt->userData,
			"Extension %s has no base\n",
			type->name);
    }
    child = node->children;
    if (IS_SCHEMA(child, "annotation")) {
	type->annot = xmlSchemaParseAnnotation(ctxt, schema, child);
	child = child->next;
    }
    subtype = NULL;

    if (IS_SCHEMA(child, "all")) {
	subtype = xmlSchemaParseAll(ctxt, schema, child);
	child = child->next;
    } else if (IS_SCHEMA(child, "choice")) {
	subtype = xmlSchemaParseChoice(ctxt, schema, child);
	child = child->next;
    } else if (IS_SCHEMA(child, "sequence")) {
	subtype = xmlSchemaParseSequence(ctxt, schema, child);
	child = child->next;
    } else if (IS_SCHEMA(child, "group")) {
	subtype = xmlSchemaParseGroup(ctxt, schema, child);
	child = child->next;
    }
    if (subtype != NULL)
	type->subtypes = subtype;
    child = xmlSchemaParseAttrDecls(ctxt, schema, child, type);
    if (child != NULL) {
	xmlSchemaErrorContext(ctxt, schema, node, child);
	if ((ctxt != NULL) && (ctxt->error != NULL))
	    ctxt->error(ctxt->userData,
			"Extension %s has unexpected content\n",
			type->name);
    }
    ctxt->container = oldcontainer;
    return (type);
}

/**
 * xmlSchemaParseSimpleContent:
 * @ctxt:  a schema validation context
 * @schema:  the schema being built
 * @node:  a subtree containing XML Schema informations
 *
 * parse a XML schema SimpleContent definition
 * *WARNING* this interface is highly subject to change
 *
 * Returns the type definition or NULL in case of error
 */
static xmlSchemaTypePtr
xmlSchemaParseSimpleContent(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                          xmlNodePtr node)
{
    xmlSchemaTypePtr type, subtype;
    xmlNodePtr child = NULL;
    xmlChar name[30];

    if ((ctxt == NULL) || (schema == NULL) || (node == NULL))
        return (NULL);


    snprintf((char *)name, 30, "complexContent %d", ctxt->counter++ + 1);
    type = xmlSchemaAddType(ctxt, schema, name);
    if (type == NULL)
        return (NULL);
    type->node = node;
    type->type = XML_SCHEMA_TYPE_SIMPLE_CONTENT;
    type->id = xmlGetProp(node, BAD_CAST "id");

    child = node->children;
    if (IS_SCHEMA(child, "annotation")) {
	type->annot = xmlSchemaParseAnnotation(ctxt, schema, child);
	child = child->next;
    }
    subtype = NULL;
    if (IS_SCHEMA(child, "restriction")) {
	subtype = (xmlSchemaTypePtr) 
	      xmlSchemaParseRestriction(ctxt, schema, child, 0);
	child = child->next;
    } else if (IS_SCHEMA(child, "extension")) {
	subtype = (xmlSchemaTypePtr) 
	      xmlSchemaParseExtension(ctxt, schema, child);
	child = child->next;
    }
    type->subtypes = subtype;
    if (child != NULL) {
	xmlSchemaErrorContext(ctxt, schema, node, child);
	if ((ctxt != NULL) && (ctxt->error != NULL))
	    ctxt->error(ctxt->userData,
			"SimpleContent %s has unexpected content\n",
			type->name);
    }
    return (type);
}

/**
 * xmlSchemaParseComplexContent:
 * @ctxt:  a schema validation context
 * @schema:  the schema being built
 * @node:  a subtree containing XML Schema informations
 *
 * parse a XML schema ComplexContent definition
 * *WARNING* this interface is highly subject to change
 *
 * Returns the type definition or NULL in case of error
 */
static xmlSchemaTypePtr
xmlSchemaParseComplexContent(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                          xmlNodePtr node)
{
    xmlSchemaTypePtr type, subtype;
    xmlNodePtr child = NULL;
    xmlChar name[30];

    if ((ctxt == NULL) || (schema == NULL) || (node == NULL))
        return (NULL);


    snprintf((char *)name, 30, "complexContent %d", ctxt->counter++ + 1);
    type = xmlSchemaAddType(ctxt, schema, name);
    if (type == NULL)
        return (NULL);
    type->node = node;
    type->type = XML_SCHEMA_TYPE_COMPLEX_CONTENT;
    type->id = xmlGetProp(node, BAD_CAST "id");

    child = node->children;
    if (IS_SCHEMA(child, "annotation")) {
	type->annot = xmlSchemaParseAnnotation(ctxt, schema, child);
	child = child->next;
    }
    subtype = NULL;
    if (IS_SCHEMA(child, "restriction")) {
	subtype = (xmlSchemaTypePtr) 
	      xmlSchemaParseRestriction(ctxt, schema, child, 0);
	child = child->next;
    } else if (IS_SCHEMA(child, "extension")) {
	subtype = (xmlSchemaTypePtr) 
	      xmlSchemaParseExtension(ctxt, schema, child);
	child = child->next;
    }
    type->subtypes = subtype;
    if (child != NULL) {
	xmlSchemaErrorContext(ctxt, schema, node, child);
	if ((ctxt != NULL) && (ctxt->error != NULL))
	    ctxt->error(ctxt->userData,
			"ComplexContent %s has unexpected content\n",
			type->name);
    }
    return (type);
}

/**
 * xmlSchemaParseComplexType:
 * @ctxt:  a schema validation context
 * @schema:  the schema being built
 * @node:  a subtree containing XML Schema informations
 *
 * parse a XML schema Complex Type definition
 * *WARNING* this interface is highly subject to change
 *
 * Returns the type definition or NULL in case of error
 */
static xmlSchemaTypePtr
xmlSchemaParseComplexType(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                          xmlNodePtr node)
{
    xmlSchemaTypePtr type, subtype;
    xmlNodePtr child = NULL;
    xmlChar *name;
    xmlChar *oldcontainer;

    if ((ctxt == NULL) || (schema == NULL) || (node == NULL))
        return (NULL);

    oldcontainer = ctxt->container;
    name = xmlGetProp(node, (const xmlChar *) "name");
    if (name == NULL) {
	char buf[100];

	snprintf(buf, 99, "anontype%d", ctxt->counter++ + 1);
	name = xmlStrdup((xmlChar *) buf);
    }
    if (name == NULL) {
	xmlSchemaErrorContext(ctxt, schema, node, child);
        if ((ctxt != NULL) && (ctxt->error != NULL))
            ctxt->error(ctxt->userData, "complexType has no name\n");
        return (NULL);
    }
    type = xmlSchemaAddType(ctxt, schema, name);
    if (type == NULL) {
	xmlFree(name);
        return (NULL);
    }
    type->node = node;
    type->type = XML_SCHEMA_TYPE_COMPLEX;
    type->id = xmlGetProp(node, BAD_CAST "id");
    ctxt->container = name;

    child = node->children;
    if (IS_SCHEMA(child, "annotation")) {
	type->annot = xmlSchemaParseAnnotation(ctxt, schema, child);
	child = child->next;
    }
    if (IS_SCHEMA(child, "simpleContent")) {
	type->subtypes = xmlSchemaParseSimpleContent(ctxt, schema, child);
	child = child->next;
    } else if (IS_SCHEMA(child, "complexContent")) {
	type->subtypes = xmlSchemaParseComplexContent(ctxt, schema, child);
	child = child->next;
    } else {
	subtype = NULL;

	if (IS_SCHEMA(child, "all")) {
	    subtype = xmlSchemaParseAll(ctxt, schema, child);
	    child = child->next;
	} else if (IS_SCHEMA(child, "choice")) {
	    subtype = xmlSchemaParseChoice(ctxt, schema, child);
	    child = child->next;
	} else if (IS_SCHEMA(child, "sequence")) {
	    subtype = xmlSchemaParseSequence(ctxt, schema, child);
	    child = child->next;
	} else if (IS_SCHEMA(child, "group")) {
	    subtype = xmlSchemaParseGroup(ctxt, schema, child);
	    child = child->next;
	}
	if (subtype != NULL)
	    type->subtypes = subtype;
	child = xmlSchemaParseAttrDecls(ctxt, schema, child, type);
    }
    if (child != NULL) {
	xmlSchemaErrorContext(ctxt, schema, node, child);
	if ((ctxt != NULL) && (ctxt->error != NULL))
	    ctxt->error(ctxt->userData,
			"ComplexType %s has unexpected content\n",
			type->name);
    }
    ctxt->container = oldcontainer;
    xmlFree(name);
    return (type);
}


/**
 * xmlSchemaParseSchema:
 * @ctxt:  a schema validation context
 * @node:  a subtree containing XML Schema informations
 *
 * parse a XML schema definition from a node set
 * *WARNING* this interface is highly subject to change
 *
 * Returns the internal XML Schema structure built from the resource or
 *         NULL in case of error
 */
static xmlSchemaPtr
xmlSchemaParseSchema(xmlSchemaParserCtxtPtr ctxt, xmlNodePtr node)
{
    xmlSchemaPtr schema = NULL;
    xmlSchemaAnnotPtr annot;
    xmlNodePtr child = NULL;
    xmlChar *val;

    if ((ctxt == NULL) || (node == NULL))
        return (NULL);

    if (IS_SCHEMA(node, "schema")) {
        schema = xmlSchemaNewSchema(ctxt);
	if (schema == NULL)
	    return(NULL);
	schema->targetNamespace = xmlGetProp(node, BAD_CAST "targetNamespace");
	schema->id = xmlGetProp(node, BAD_CAST "id");
	schema->version = xmlGetProp(node, BAD_CAST "version");
	val = xmlGetProp(node, BAD_CAST "elementFormDefault");
	if (val != NULL) {
	    if (xmlStrEqual(val, BAD_CAST "qualified"))
		schema->flags |= XML_SCHEMAS_QUALIF_ELEM;
	    else if (!xmlStrEqual(val, BAD_CAST "unqualified")) {
		xmlSchemaErrorContext(ctxt, schema, node, child);
		if ((ctxt != NULL) && (ctxt->error != NULL)) {
		    ctxt->error(ctxt->userData,
				"Invalid value %s for elementFormDefault\n",
				val);
		}
	    }
	    xmlFree(val);
	}
	val = xmlGetProp(node, BAD_CAST "attributeFormDefault");
	if (val != NULL) {
	    if (xmlStrEqual(val, BAD_CAST "qualified"))
		schema->flags |= XML_SCHEMAS_QUALIF_ATTR;
	    else if (!xmlStrEqual(val, BAD_CAST "unqualified")) {
		xmlSchemaErrorContext(ctxt, schema, node, child);
		if ((ctxt != NULL) && (ctxt->error != NULL)) {
		    ctxt->error(ctxt->userData,
				"Invalid value %s for elementFormDefault\n",
				val);
		}
	    }
	    xmlFree(val);
	}

        child = node->children;
	while ((IS_SCHEMA(child, "include")) ||
	       (IS_SCHEMA(child, "import")) ||
	       (IS_SCHEMA(child, "redefine")) ||
	       (IS_SCHEMA(child, "annotation"))) {
	    if (IS_SCHEMA(child, "annotation")) {
		annot = xmlSchemaParseAnnotation(ctxt, schema, child);
		if (schema->annot == NULL)
		    schema->annot = annot;
		else
		    xmlSchemaFreeAnnot(annot);
	    } else if (IS_SCHEMA(child, "include")) {
		TODO
	    } else if (IS_SCHEMA(child, "import")) {
		xmlSchemaParseImport(ctxt, schema, child);
	    } else if (IS_SCHEMA(child, "redefine")) {
		TODO
	    }
	    child = child->next;
	}
	while (child != NULL) {
	    if (IS_SCHEMA(child, "complexType")) {
                xmlSchemaParseComplexType(ctxt, schema, child);
		child = child->next;
	    } else if (IS_SCHEMA(child, "simpleType")) {
                xmlSchemaParseSimpleType(ctxt, schema, child);
		child = child->next;
	    } else if (IS_SCHEMA(child, "element")) {
                xmlSchemaParseElement(ctxt, schema, child, 1);
		child = child->next;
	    } else if (IS_SCHEMA(child, "attribute")) {
                xmlSchemaParseAttribute(ctxt, schema, child);
		child = child->next;
	    } else if (IS_SCHEMA(child, "attributeGroup")) {
                xmlSchemaParseAttributeGroup(ctxt, schema, child);
		child = child->next;
	    } else if (IS_SCHEMA(child, "group")) {
		xmlSchemaParseGroup(ctxt, schema, child);
		child = child->next;
	    } else if (IS_SCHEMA(child, "notation")) {
		xmlSchemaParseNotation(ctxt, schema, child);
		child = child->next;
	    } else {
		xmlSchemaErrorContext(ctxt, schema, node, child);
		if ((ctxt != NULL) && (ctxt->error != NULL))
		    ctxt->error(ctxt->userData,
				"Schemas: unexpected element %s here \n",
				child->name);
		child = child->next;
	    }
	    while (IS_SCHEMA(child, "annotation")) {
		annot = xmlSchemaParseAnnotation(ctxt, schema, child);
		if (schema->annot == NULL)
		    schema->annot = annot;
		else
		    xmlSchemaFreeAnnot(annot);
		child = child->next;
            }
        }
    }
#ifdef DEBUG
    if (schema == NULL)
        xmlGenericError(xmlGenericErrorContext,
                        "xmlSchemaParse() failed\n");
#endif

    return (schema);
}

/************************************************************************
 * 									*
 * 			Validating using Schemas			*
 * 									*
 ************************************************************************/

/************************************************************************
 * 									*
 * 			Reading/Writing Schemas				*
 * 									*
 ************************************************************************/

/**
 * xmlSchemaNewParserCtxt:
 * @URL:  the location of the schema
 *
 * Create an XML Schemas parse context for that file/resource expected
 * to contain an XML Schemas file.
 *
 * Returns the parser context or NULL in case of error
 */
xmlSchemaParserCtxtPtr
xmlSchemaNewParserCtxt(const char *URL) {
    xmlSchemaParserCtxtPtr ret;

    if (URL == NULL)
	return(NULL);

    ret = (xmlSchemaParserCtxtPtr) xmlMalloc(sizeof(xmlSchemaParserCtxt));
    if (ret == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		"Failed to allocate new schama parser context for %s\n", URL);
        return (NULL);
    }
    memset(ret, 0, sizeof(xmlSchemaParserCtxt));
    ret->URL = xmlStrdup((const xmlChar *)URL);
    return (ret);
}

/**
 * xmlSchemaNewMemParserCtxt:
 * @buffer:  a pointer to a char array containing the schemas
 * @size:  the size of the array
 *
 * Create an XML Schemas parse context for that memory buffer expected
 * to contain an XML Schemas file.
 *
 * Returns the parser context or NULL in case of error
 */
xmlSchemaParserCtxtPtr
xmlSchemaNewMemParserCtxt(const char *buffer, int size) {
    xmlSchemaParserCtxtPtr ret;

    if ((buffer == NULL) || (size <= 0))
	return(NULL);

    ret = (xmlSchemaParserCtxtPtr) xmlMalloc(sizeof(xmlSchemaParserCtxt));
    if (ret == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		"Failed to allocate new schama parser context\n");
        return (NULL);
    }
    memset(ret, 0, sizeof(xmlSchemaParserCtxt));
    ret->buffer = buffer;
    ret->size = size;
    return (ret);
}

/**
 * xmlSchemaFreeParserCtxt:
 * @ctxt:  the schema parser context
 *
 * Free the resources associated to the schema parser context
 */
void
xmlSchemaFreeParserCtxt(xmlSchemaParserCtxtPtr ctxt) {
    if (ctxt == NULL)
	return;
    if (ctxt->URL != NULL)
	xmlFree(ctxt->URL);
    if (ctxt->doc != NULL)
	xmlFreeDoc(ctxt->doc);
    xmlFree(ctxt);
}

/************************************************************************
 *									*
 *			Building the content models			*
 *									*
 ************************************************************************/
/**
 * xmlSchemaBuildAContentModel:
 * @type:  the schema type definition
 * @ctxt:  the schema parser context
 * @name:  the element name whose content is being built
 *
 * Generate the automata sequence needed for that type
 */
static void
xmlSchemaBuildAContentModel(xmlSchemaTypePtr type,
		           xmlSchemaParserCtxtPtr ctxt,
		           const xmlChar *name) {
    if (type == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		"Found unexpected type = NULL in %s content model\n",
			name);
	return;
    }
    switch (type->type) {
	case XML_SCHEMA_TYPE_ANY:
	    /* TODO : handle the namespace too */
	    /* TODO : make that a specific transition type */
	    TODO
	    ctxt->state = xmlAutomataNewTransition(ctxt->am, ctxt->state,
		                NULL, BAD_CAST "*", NULL);
	    break;
	case XML_SCHEMA_TYPE_ELEMENT: {
	    xmlSchemaElementPtr elem = (xmlSchemaElementPtr) type;
	    /* TODO : handle the namespace too */
	    xmlAutomataStatePtr oldstate = ctxt->state;
	    if (elem->maxOccurs >= UNBOUNDED) {
		if (elem->minOccurs > 1) {
		    xmlAutomataStatePtr tmp;
		    int counter;

		    ctxt->state = xmlAutomataNewEpsilon(ctxt->am,
			                                oldstate, NULL);
		    oldstate = ctxt->state;

		    counter = xmlAutomataNewCounter(ctxt->am,
				    elem->minOccurs - 1, UNBOUNDED);

		    if (elem->refDecl != NULL) {
			xmlSchemaBuildAContentModel(
				(xmlSchemaTypePtr) elem->refDecl,
				ctxt, elem->refDecl->name);
		    } else {
			ctxt->state = xmlAutomataNewTransition(ctxt->am,
				ctxt->state, NULL, elem->name, type);
		    }
		    tmp = ctxt->state;
		    xmlAutomataNewCountedTrans(ctxt->am, tmp, oldstate,
			                       counter);
		    ctxt->state = xmlAutomataNewCounterTrans(ctxt->am, tmp,
			                       NULL, counter);

		} else {
		    if (elem->refDecl != NULL) {
			xmlSchemaBuildAContentModel(
				(xmlSchemaTypePtr) elem->refDecl,
				ctxt, elem->refDecl->name);
		    } else {
			ctxt->state = xmlAutomataNewTransition(ctxt->am,
				ctxt->state, NULL, elem->name, type);
		    }
		    xmlAutomataNewEpsilon(ctxt->am, ctxt->state, oldstate);
		    if (elem->minOccurs == 0) {
			/* basically an elem* */
			xmlAutomataNewEpsilon(ctxt->am, oldstate, ctxt->state);
		    }
		}
	    } else if ((elem->maxOccurs > 1) || (elem->minOccurs > 1)) {
		xmlAutomataStatePtr tmp;
		int counter;

		ctxt->state = xmlAutomataNewEpsilon(ctxt->am,
						    oldstate, NULL);
		oldstate = ctxt->state;

		counter = xmlAutomataNewCounter(ctxt->am,
				elem->minOccurs - 1, elem->maxOccurs - 1);

		if (elem->refDecl != NULL) {
		    xmlSchemaBuildAContentModel(
			    (xmlSchemaTypePtr) elem->refDecl,
			    ctxt, elem->refDecl->name);
		} else {
		    ctxt->state = xmlAutomataNewTransition(ctxt->am,
			    ctxt->state, NULL, elem->name, type);
		}
		tmp = ctxt->state;
		xmlAutomataNewCountedTrans(ctxt->am, tmp, oldstate,
					   counter);
		ctxt->state = xmlAutomataNewCounterTrans(ctxt->am, tmp,
					   NULL, counter);
		if (elem->minOccurs == 0) {
		    /* basically an elem? */
		    xmlAutomataNewEpsilon(ctxt->am, oldstate, ctxt->state);
		}

	    } else {
		if (elem->refDecl != NULL) {
		    xmlSchemaBuildAContentModel(
			    (xmlSchemaTypePtr) elem->refDecl,
			    ctxt, elem->refDecl->name);
		} else {
		    ctxt->state = xmlAutomataNewTransition(ctxt->am,
			    ctxt->state, NULL, elem->name, type);
		}
		if (elem->minOccurs == 0) {
		    /* basically an elem? */
		    xmlAutomataNewEpsilon(ctxt->am, oldstate, ctxt->state);
		}
	    }
	    break;
	}
	case XML_SCHEMA_TYPE_SEQUENCE: {
	    xmlSchemaTypePtr subtypes;

	    /*
	     * If max and min occurances are default (1) then
	     * simply iterate over the subtypes
	     */
	    if ((type->minOccurs == 1 ) && (type->maxOccurs == 1)) {
	        subtypes = type->subtypes;
	        while (subtypes != NULL) {
		    xmlSchemaBuildAContentModel(subtypes, ctxt, name);
		    subtypes = subtypes->next;
	        }
	    } else {
	        xmlAutomataStatePtr oldstate = ctxt->state;
	        if (type->maxOccurs >= UNBOUNDED) {
		    if (type->minOccurs > 1) {
		        xmlAutomataStatePtr tmp;
		        int counter;

		        ctxt->state = xmlAutomataNewEpsilon(ctxt->am,
			                                oldstate, NULL);
		        oldstate = ctxt->state;

		        counter = xmlAutomataNewCounter(ctxt->am,
			        	    type->minOccurs - 1, UNBOUNDED);

	                subtypes = type->subtypes;
	                while (subtypes != NULL) {
		            xmlSchemaBuildAContentModel(subtypes, ctxt, name);
		            subtypes = subtypes->next;
	                }
		        tmp = ctxt->state;
		        xmlAutomataNewCountedTrans(ctxt->am, tmp, oldstate,
		                            counter);
		        ctxt->state = xmlAutomataNewCounterTrans(ctxt->am, tmp,
		                            NULL, counter);

		    } else {
	                subtypes = type->subtypes;
	                while (subtypes != NULL) {
		            xmlSchemaBuildAContentModel(subtypes, ctxt, name);
		            subtypes = subtypes->next;
	                }
		        xmlAutomataNewEpsilon(ctxt->am, ctxt->state, oldstate);
		        if (type->minOccurs == 0) {
			    xmlAutomataNewEpsilon(ctxt->am, oldstate,
				                  ctxt->state);
		        }
		    }
	        } else if ((type->maxOccurs > 1) || (type->minOccurs > 1)) {
		    xmlAutomataStatePtr tmp;
		    int counter;

		    ctxt->state = xmlAutomataNewEpsilon(ctxt->am,
						    oldstate, NULL);
		    oldstate = ctxt->state;

		    counter = xmlAutomataNewCounter(ctxt->am,
		                  type->minOccurs - 1, type->maxOccurs - 1);

	            subtypes = type->subtypes;
	            while (subtypes != NULL) {
		        xmlSchemaBuildAContentModel(subtypes, ctxt, name);
		        subtypes = subtypes->next;
	            }
		    tmp = ctxt->state;
		    xmlAutomataNewCountedTrans(ctxt->am, tmp, oldstate,
		          counter);
		    ctxt->state = xmlAutomataNewCounterTrans(ctxt->am, tmp,
		                                             NULL, counter);
		    if (type->minOccurs == 0) {
		        xmlAutomataNewEpsilon(ctxt->am, oldstate, ctxt->state);
		    }

	        } else {
	            subtypes = type->subtypes;
	            while (subtypes != NULL) {
		        xmlSchemaBuildAContentModel(subtypes, ctxt, name);
		        subtypes = subtypes->next;
	            }
		    if (type->minOccurs == 0) {
		        xmlAutomataNewEpsilon(ctxt->am, oldstate, ctxt->state);
		    }
	        }
	    }
	    break;
	}
	case XML_SCHEMA_TYPE_CHOICE: {
	    xmlSchemaTypePtr subtypes;
	    xmlAutomataStatePtr start, end;

	    start = ctxt->state;
	    end = xmlAutomataNewState(ctxt->am);

	    /*
	     * iterate over the subtypes and remerge the end with an
	     * epsilon transition
	     */
	    if (type->maxOccurs == 1) {
		subtypes = type->subtypes;
		while (subtypes != NULL) {
		    ctxt->state = start;
		    xmlSchemaBuildAContentModel(subtypes, ctxt, name);
		    xmlAutomataNewEpsilon(ctxt->am, ctxt->state, end);
		    subtypes = subtypes->next;
		}
	    } else {
		int counter;
		xmlAutomataStatePtr hop;
		int maxOccurs = type->maxOccurs == UNBOUNDED ?
		                      UNBOUNDED : type->maxOccurs - 1;
		int minOccurs = type->minOccurs < 1 ? 0 : type->minOccurs - 1;

		/*
		 * use a counter to keep track of the number of transtions
		 * which went through the choice.
		 */
		counter = xmlAutomataNewCounter(ctxt->am, minOccurs, maxOccurs);
		hop = xmlAutomataNewState(ctxt->am);

		subtypes = type->subtypes;
		while (subtypes != NULL) {
		    ctxt->state = start;
		    xmlSchemaBuildAContentModel(subtypes, ctxt, name);
		    xmlAutomataNewEpsilon(ctxt->am, ctxt->state, hop);
		    subtypes = subtypes->next;
		}
		xmlAutomataNewCountedTrans(ctxt->am, hop, start, counter);
		xmlAutomataNewCounterTrans(ctxt->am, hop, end, counter);
	    }
	    if (type->minOccurs == 0) {
		xmlAutomataNewEpsilon(ctxt->am, start, end);
	    }
	    ctxt->state = end;
	    break;
	}
	case XML_SCHEMA_TYPE_ALL: {
	    xmlAutomataStatePtr start;
	    xmlSchemaTypePtr subtypes;
	    xmlSchemaElementPtr elem = (xmlSchemaElementPtr) type;
	    int lax;

	    subtypes = type->subtypes;
	    if (subtypes == NULL)
		break;
	    start = ctxt->state;
	    while (subtypes != NULL) {
		ctxt->state = start;
		elem = (xmlSchemaElementPtr) subtypes;

		/* TODO : handle the namespace too */
		xmlAutomataNewOnceTrans(ctxt->am, ctxt->state, ctxt->state,
			        elem->name, elem->minOccurs, elem->maxOccurs,
				subtypes);
		subtypes = subtypes->next;
	    }
	    lax = type->minOccurs == 0;
	    ctxt->state = xmlAutomataNewAllTrans(ctxt->am, ctxt->state, NULL,
		                                 lax);
	    break;
	}
	case XML_SCHEMA_TYPE_RESTRICTION:
	    if (type->subtypes != NULL)
		xmlSchemaBuildAContentModel(type->subtypes, ctxt, name);
	    break;
	case XML_SCHEMA_TYPE_EXTENSION:
	    if (type->baseType != NULL) {
		xmlSchemaTypePtr subtypes;

		xmlSchemaBuildAContentModel(type->baseType, ctxt, name);
		subtypes = type->subtypes;
		while (subtypes != NULL) {
		    xmlSchemaBuildAContentModel(subtypes, ctxt, name);
		    subtypes = subtypes->next;
		}
	    } else if (type->subtypes != NULL)
		xmlSchemaBuildAContentModel(type->subtypes, ctxt, name);
	    break;
	case XML_SCHEMA_TYPE_GROUP:
	case XML_SCHEMA_TYPE_COMPLEX:
	case XML_SCHEMA_TYPE_COMPLEX_CONTENT:
	    if (type->subtypes != NULL)
		xmlSchemaBuildAContentModel(type->subtypes, ctxt, name);
	    break;
	default:
	    xmlGenericError(xmlGenericErrorContext,
		    "Found unexpected type %d in %s content model\n",
		            type->type, name);
	    return;
    }
}
/**
 * xmlSchemaBuildContentModel:
 * @typeDecl:  the schema type definition
 * @ctxt:  the schema parser context
 *
 * Fixes the content model of the element.
 */
static void
xmlSchemaBuildContentModel(xmlSchemaElementPtr elem,
		           xmlSchemaParserCtxtPtr ctxt,
		           const xmlChar *name) {
    xmlAutomataStatePtr start;

    if (elem->contModel != NULL)
	return;
    if (elem->subtypes == NULL) {
	elem->contentType = XML_SCHEMA_CONTENT_ANY;
	return;
    }
    if (elem->subtypes->type != XML_SCHEMA_TYPE_COMPLEX)
	return;
    if (elem->subtypes->contentType == XML_SCHEMA_CONTENT_BASIC)
	return;

#ifdef DEBUG_CONTENT
    xmlGenericError(xmlGenericErrorContext,
	    "Building content model for %s\n", name);
#endif

    ctxt->am = xmlNewAutomata();
    if (ctxt->am == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		"Cannot create automata for elem %s\n", name);
	return;
    }
    start = ctxt->state = xmlAutomataGetInitState(ctxt->am);
    xmlSchemaBuildAContentModel(elem->subtypes, ctxt, name);
    xmlAutomataSetFinalState(ctxt->am, ctxt->state);
    elem->contModel = xmlAutomataCompile(ctxt->am);
    if (!xmlAutomataIsDeterminist(ctxt->am)) {
	xmlGenericError(xmlGenericErrorContext,
			"Content model of %s is not determinist:\n", name);
	ctxt->err = XML_SCHEMAS_ERR_NOTDETERMINIST;
	ctxt->state = NULL;
    } else {
#ifdef DEBUG_CONTENT_REGEXP
	xmlGenericError(xmlGenericErrorContext,
			"Content model of %s:\n", name);
	xmlRegexpPrint(stderr, elem->contModel);
#endif
	ctxt->state = NULL;
    }
    xmlFreeAutomata(ctxt->am);
    ctxt->am = NULL;
}

/**
 * xmlSchemaRefFixupCallback:
 * @elem:  the schema element context
 * @ctxt:  the schema parser context
 *
 * Free the resources associated to the schema parser context
 */
static void
xmlSchemaRefFixupCallback(xmlSchemaElementPtr elem,
		     xmlSchemaParserCtxtPtr ctxt,
		     const xmlChar *name,
		     const xmlChar *context ATTRIBUTE_UNUSED,
		     const xmlChar *namespace ATTRIBUTE_UNUSED)
{
    if ((ctxt == NULL) || (elem == NULL))
	return;
    if (elem->ref != NULL) {
	xmlSchemaElementPtr elemDecl;

	if (elem->subtypes != NULL) {
	    xmlSchemaErrorContext(ctxt, NULL, elem->node, NULL);
	    if ((ctxt != NULL) && (ctxt->error != NULL))
		ctxt->error(ctxt->userData,
			    "Schemas: element %s have both ref and subtype\n",
			    name);
	    return;
	}
	elemDecl = xmlHashLookup2(ctxt->schema->elemDecl,
				  elem->ref, elem->refNs);

        if (elemDecl == NULL) {
	    if ((ctxt != NULL) && (ctxt->error != NULL))
		ctxt->error(ctxt->userData,
			    "Schemas: element %s ref to %s not found\n",
			    name, elem->ref);
	    return;
	}
	elem->refDecl = elemDecl;
    } else if (elem->namedType != NULL) {
	xmlSchemaTypePtr typeDecl;

	if (elem->subtypes != NULL) {
	    xmlSchemaErrorContext(ctxt, NULL, elem->node, NULL);
	    if ((ctxt != NULL) && (ctxt->error != NULL))
		ctxt->error(ctxt->userData,
			    "Schemas: element %s have both type and subtype\n",
			    name);
	    return;
	}
	typeDecl = xmlSchemaGetType(ctxt->schema, elem->namedType,
		                    elem->namedTypeNs);

        if (typeDecl == NULL) {
	    if ((ctxt != NULL) && (ctxt->error != NULL))
		ctxt->error(ctxt->userData,
			    "Schemas: element %s type %s not found\n",
			    name, elem->namedType);
	    return;
	}
	elem->subtypes = typeDecl;
    }
}

/**
 * xmlSchemaTypeFixup:
 * @typeDecl:  the schema type definition
 * @ctxt:  the schema parser context
 *
 * Fixes the content model of the type.
 */
static void
xmlSchemaTypeFixup(xmlSchemaTypePtr typeDecl,
		   xmlSchemaParserCtxtPtr ctxt,
		   const xmlChar *name)
{
    if (name == NULL)
	name = typeDecl->name;
    if (typeDecl->contentType == XML_SCHEMA_CONTENT_UNKNOWN) {
	switch (typeDecl->type) {
	    case XML_SCHEMA_TYPE_SIMPLE_CONTENT: {
		xmlSchemaTypeFixup(typeDecl->subtypes, ctxt, NULL);
		typeDecl->contentType = typeDecl->subtypes->contentType;
		break;
	    }
	    case XML_SCHEMA_TYPE_RESTRICTION: {
		if (typeDecl->subtypes != NULL)
		    xmlSchemaTypeFixup(typeDecl->subtypes, ctxt, NULL);

		if (typeDecl->base != NULL) {
		    xmlSchemaTypePtr baseType;

		    baseType = xmlSchemaGetType(ctxt->schema, typeDecl->base,
						typeDecl->baseNs);
		    if (baseType == NULL) {
			if ((ctxt != NULL) && (ctxt->error != NULL))
			    ctxt->error(ctxt->userData,
				"Schemas: type %s base type %s not found\n",
					name, typeDecl->base);
		    }
		    typeDecl->baseType = baseType;
		}
		if (typeDecl->subtypes == NULL)
		    /* 1.1.1 */
		    typeDecl->contentType = XML_SCHEMA_CONTENT_EMPTY;
		else if ((typeDecl->subtypes->subtypes == NULL) &&
		    ((typeDecl->subtypes->type == XML_SCHEMA_TYPE_ALL) ||
		     (typeDecl->subtypes->type == XML_SCHEMA_TYPE_SEQUENCE)))
		    /* 1.1.2 */
		    typeDecl->contentType = XML_SCHEMA_CONTENT_EMPTY;
		else if ((typeDecl->subtypes->type == XML_SCHEMA_TYPE_CHOICE) &&
		    (typeDecl->subtypes->subtypes == NULL))
		    /* 1.1.3 */
		    typeDecl->contentType = XML_SCHEMA_CONTENT_EMPTY;
		else {
		    /* 1.2 and 2.X are applied at the other layer */
		    typeDecl->contentType = XML_SCHEMA_CONTENT_ELEMENTS;
		}
		break;
	    }
	    case XML_SCHEMA_TYPE_EXTENSION: {
		xmlSchemaContentType explicitContentType;
		xmlSchemaTypePtr base;

		if (typeDecl->base != NULL) {
		    xmlSchemaTypePtr baseType;

		    baseType = xmlSchemaGetType(ctxt->schema, typeDecl->base,
						typeDecl->baseNs);
		    if (baseType == NULL) {
			if ((ctxt != NULL) && (ctxt->error != NULL))
			    ctxt->error(ctxt->userData,
				"Schemas: type %s base type %s not found\n",
					name, typeDecl->base);
		    }
		    typeDecl->baseType = baseType;
		}
		if (typeDecl->subtypes != NULL)
		    xmlSchemaTypeFixup(typeDecl->subtypes, ctxt, NULL);

		explicitContentType = XML_SCHEMA_CONTENT_ELEMENTS;
		if (typeDecl->subtypes == NULL)
		    /* 1.1.1 */
		    explicitContentType = XML_SCHEMA_CONTENT_EMPTY;
		else if ((typeDecl->subtypes->subtypes == NULL) &&
		    ((typeDecl->subtypes->type == XML_SCHEMA_TYPE_ALL) ||
		     (typeDecl->subtypes->type == XML_SCHEMA_TYPE_SEQUENCE)))
		    /* 1.1.2 */
		    explicitContentType = XML_SCHEMA_CONTENT_EMPTY;
		else if ((typeDecl->subtypes->type == XML_SCHEMA_TYPE_CHOICE) &&
		    (typeDecl->subtypes->subtypes == NULL))
		    /* 1.1.3 */
		    explicitContentType = XML_SCHEMA_CONTENT_EMPTY;

		base = xmlSchemaGetType(ctxt->schema, typeDecl->base,
					typeDecl->baseNs);
		if (base == NULL) {
		    xmlSchemaErrorContext(ctxt, NULL, typeDecl->node, NULL);
		    if ((ctxt != NULL) && (ctxt->error != NULL))
			ctxt->error(ctxt->userData,
			"Schemas: base type %s of type %s not found\n",
				    typeDecl->base, name);
		    return;
		}
		xmlSchemaTypeFixup(base, ctxt, NULL);
		if (explicitContentType == XML_SCHEMA_CONTENT_EMPTY) {
		    /* 2.1 */
		    typeDecl->contentType = base->contentType;
		} else if (base->contentType == XML_SCHEMA_CONTENT_EMPTY) {
		    /* 2.2 imbitable ! */
		    typeDecl->contentType = XML_SCHEMA_CONTENT_ELEMENTS;
		} else {
		    /* 2.3 imbitable pareil ! */
		    typeDecl->contentType = XML_SCHEMA_CONTENT_ELEMENTS;
		}
		break;
	    }
	    case XML_SCHEMA_TYPE_COMPLEX: {
		if (typeDecl->subtypes == NULL) {
		    typeDecl->contentType = XML_SCHEMA_CONTENT_EMPTY;
		} else {
		    if (typeDecl->flags & XML_SCHEMAS_TYPE_MIXED)
			typeDecl->contentType = XML_SCHEMA_CONTENT_MIXED;
		    else {
			xmlSchemaTypeFixup(typeDecl->subtypes, ctxt, NULL);
			typeDecl->contentType = typeDecl->subtypes->contentType;
		    }
		}
		break;
	    }
	    case XML_SCHEMA_TYPE_COMPLEX_CONTENT: {
		if (typeDecl->subtypes == NULL) {
		    typeDecl->contentType = XML_SCHEMA_CONTENT_EMPTY;
		} else {
		    if (typeDecl->flags & XML_SCHEMAS_TYPE_MIXED)
			typeDecl->contentType = XML_SCHEMA_CONTENT_MIXED;
		    else {
			xmlSchemaTypeFixup(typeDecl->subtypes, ctxt, NULL);
			typeDecl->contentType = typeDecl->subtypes->contentType;
		    }
		}
		break;
	    }
	    case XML_SCHEMA_TYPE_SEQUENCE:
	    case XML_SCHEMA_TYPE_GROUP:
	    case XML_SCHEMA_TYPE_ALL:
	    case XML_SCHEMA_TYPE_CHOICE:
		typeDecl->contentType = XML_SCHEMA_CONTENT_ELEMENTS;
		break;
	    case XML_SCHEMA_TYPE_BASIC:
	    case XML_SCHEMA_TYPE_ANY:
	    case XML_SCHEMA_TYPE_FACET:
	    case XML_SCHEMA_TYPE_SIMPLE:
	    case XML_SCHEMA_TYPE_UR:
	    case XML_SCHEMA_TYPE_ELEMENT:
	    case XML_SCHEMA_TYPE_ATTRIBUTE:
	    case XML_SCHEMA_TYPE_ATTRIBUTEGROUP:
	    case XML_SCHEMA_TYPE_NOTATION:
	    case XML_SCHEMA_TYPE_LIST:
	    case XML_SCHEMA_TYPE_UNION:
	    case XML_SCHEMA_FACET_MININCLUSIVE:
	    case XML_SCHEMA_FACET_MINEXCLUSIVE:
	    case XML_SCHEMA_FACET_MAXINCLUSIVE:
	    case XML_SCHEMA_FACET_MAXEXCLUSIVE:
	    case XML_SCHEMA_FACET_TOTALDIGITS:
	    case XML_SCHEMA_FACET_FRACTIONDIGITS:
	    case XML_SCHEMA_FACET_PATTERN:
	    case XML_SCHEMA_FACET_ENUMERATION:
	    case XML_SCHEMA_FACET_WHITESPACE:
	    case XML_SCHEMA_FACET_LENGTH:
	    case XML_SCHEMA_FACET_MAXLENGTH:
	    case XML_SCHEMA_FACET_MINLENGTH:
		typeDecl->contentType = XML_SCHEMA_CONTENT_SIMPLE;
		break;
	}
    } 
#ifdef DEBUG_TYPE
    if (typeDecl->node != NULL) {
	xmlGenericError(xmlGenericErrorContext,
		"Type of %s : %s:%d :", name, typeDecl->node->doc->URL,
			xmlGetLineNo(typeDecl->node));
    } else {
	xmlGenericError(xmlGenericErrorContext,
		"Type of %s :", name);
    }
    switch (typeDecl->contentType) {
	case XML_SCHEMA_CONTENT_SIMPLE:
	    xmlGenericError(xmlGenericErrorContext,
		    "simple\n"); break;
	case XML_SCHEMA_CONTENT_ELEMENTS:
	    xmlGenericError(xmlGenericErrorContext,
		    "elements\n"); break;
        case XML_SCHEMA_CONTENT_UNKNOWN:
	    xmlGenericError(xmlGenericErrorContext,
		    "unknown !!!\n"); break;
        case XML_SCHEMA_CONTENT_EMPTY:
	    xmlGenericError(xmlGenericErrorContext,
		    "empty\n"); break;
        case XML_SCHEMA_CONTENT_MIXED:
	    xmlGenericError(xmlGenericErrorContext,
		    "mixed\n"); break;
        case XML_SCHEMA_CONTENT_MIXED_OR_ELEMENTS:
	    xmlGenericError(xmlGenericErrorContext,
		    "mixed or elems\n"); break;
        case XML_SCHEMA_CONTENT_BASIC:
	    xmlGenericError(xmlGenericErrorContext,
		    "basic\n"); break;
	default:
	    xmlGenericError(xmlGenericErrorContext,
		    "not registered !!!\n"); break;
    }
#endif
}

/**
 * xmlSchemaCheckDefaults:
 * @typeDecl:  the schema type definition
 * @ctxt:  the schema parser context
 *
 * Checks the default values types, especially for facets 
 */
static void
xmlSchemaCheckDefaults(xmlSchemaTypePtr typeDecl,
		   xmlSchemaParserCtxtPtr ctxt,
		   const xmlChar *name)
{
    static xmlSchemaTypePtr nonNegativeIntegerType = NULL;
    if (name == NULL)
	name = typeDecl->name;
    if (nonNegativeIntegerType == NULL) {
	nonNegativeIntegerType = xmlSchemaGetPredefinedType(
		BAD_CAST "nonNegativeInteger", xmlSchemaNs);
    }
    if (typeDecl->type == XML_SCHEMA_TYPE_RESTRICTION) {
	if (typeDecl->facets != NULL) {
	    xmlSchemaFacetPtr facet = typeDecl->facets;
	    while (facet != NULL) {
		switch (facet->type) {
		    case XML_SCHEMA_FACET_MININCLUSIVE:
		    case XML_SCHEMA_FACET_MINEXCLUSIVE:
		    case XML_SCHEMA_FACET_MAXINCLUSIVE:
		    case XML_SCHEMA_FACET_MAXEXCLUSIVE: {
			/*
			 * Okay we need to validate the value
			 * at that point.
			 */
			xmlSchemaValidCtxtPtr vctxt;

			vctxt = xmlSchemaNewValidCtxt(NULL);
			if (vctxt == NULL)
			    break;
			xmlSchemaValidateSimpleValue(vctxt, typeDecl,
						     facet->value);
			facet->val = vctxt->value;
			vctxt->value = NULL;
			if (facet->val == NULL) {
			    /* error code */
			    xmlSchemaErrorContext(ctxt, NULL,
				    facet->node, NULL);
			    ctxt->error(ctxt->userData,
			    "Schemas: type %s facet value %s invalid\n",
					name, facet->value);
			}
			xmlSchemaFreeValidCtxt(vctxt);
			break;
		    }
		    case XML_SCHEMA_FACET_ENUMERATION: {
			/*
			 * Okay we need to validate the value
			 * at that point.
			 */
			xmlSchemaValidCtxtPtr vctxt;
			int ret;

			vctxt = xmlSchemaNewValidCtxt(NULL);
			if (vctxt == NULL)
			    break;
			ret = xmlSchemaValidateSimpleValue(vctxt, typeDecl,
						     facet->value);
			if (ret != 0) {
			    xmlSchemaErrorContext(ctxt, NULL,
				    facet->node, NULL);
			    ctxt->error(ctxt->userData,
			    "Schemas: type %s enumeration value %s invalid\n",
					name, facet->value);
			}
			xmlSchemaFreeValidCtxt(vctxt);
			break;
		    }
		    case XML_SCHEMA_FACET_PATTERN:
			facet->regexp = xmlRegexpCompile(facet->value);
			if (facet->regexp == NULL) {
			    /* error code */
			    ctxt->error(ctxt->userData,
			"Schemas: type %s facet regexp %s invalid\n",
					name, facet->value);
			}
			break;
		    case XML_SCHEMA_FACET_TOTALDIGITS:
		    case XML_SCHEMA_FACET_FRACTIONDIGITS:
		    case XML_SCHEMA_FACET_LENGTH:
		    case XML_SCHEMA_FACET_MAXLENGTH:
		    case XML_SCHEMA_FACET_MINLENGTH: {
			int ret;

			ret = xmlSchemaValidatePredefinedType(
				     nonNegativeIntegerType, facet->value,
				     &facet->val);
			if (ret != 0) {
			    /* error code */
			    xmlSchemaErrorContext(ctxt, NULL,
				    facet->node, NULL);
			    ctxt->error(ctxt->userData,
			    "Schemas: type %s facet value %s invalid\n",
					name, facet->value);
			}
			break;
		    }
		    case XML_SCHEMA_FACET_WHITESPACE: {
			if (xmlStrEqual(facet->value, BAD_CAST"preserve")) {
			    facet->whitespace = XML_SCHEMAS_FACET_PRESERVE;
			} else if (xmlStrEqual(facet->value,
				    BAD_CAST"replace")) {
			    facet->whitespace = XML_SCHEMAS_FACET_REPLACE;
			} else if (xmlStrEqual(facet->value,
				    BAD_CAST"collapse")) {
			    facet->whitespace = XML_SCHEMAS_FACET_COLLAPSE;
			} else {
			    xmlSchemaErrorContext(ctxt, NULL,
				    facet->node, NULL);
			    ctxt->error(ctxt->userData,
			    "Schemas: type %s whiteSpace value %s invalid\n",
					name, facet->value);
			}
		    }
		    default:
			break;
		}
		facet = facet->next;
	    }
	}
    }
}

/**
 * xmlSchemaAttrGrpFixup:
 * @attrgrpDecl:  the schema attribute definition
 * @ctxt:  the schema parser context
 * @name:  the attribute name
 *
 * Fixes finish doing the computations on the attributes definitions
 */
static void
xmlSchemaAttrGrpFixup(xmlSchemaAttributeGroupPtr attrgrpDecl,
		   xmlSchemaParserCtxtPtr ctxt,
		   const xmlChar *name)
{
    if (name == NULL)
	name = attrgrpDecl->name;
    if (attrgrpDecl->attributes != NULL)
	return;
    if (attrgrpDecl->ref != NULL) {
	xmlSchemaAttributeGroupPtr ref;

	ref = xmlHashLookup2(ctxt->schema->attrgrpDecl, attrgrpDecl->ref,
		             attrgrpDecl->refNs);
	if (ref == NULL) {
	    if ((ctxt != NULL) && (ctxt->error != NULL))
		ctxt->error(ctxt->userData,
		    "Schemas: attribute group %s reference %s not found\n",
			    name, attrgrpDecl->ref);
	    return;
	}
	xmlSchemaAttrGrpFixup(ref, ctxt, NULL);
	attrgrpDecl->attributes = ref->attributes;
    } else {
	if ((ctxt != NULL) && (ctxt->error != NULL))
	    ctxt->error(ctxt->userData,
		"Schemas: attribute %s has no attributes nor reference\n",
			name);
    }
}

/**
 * xmlSchemaAttrFixup:
 * @attrDecl:  the schema attribute definition
 * @ctxt:  the schema parser context
 * @name:  the attribute name
 *
 * Fixes finish doing the computations on the attributes definitions
 */
static void
xmlSchemaAttrFixup(xmlSchemaAttributePtr attrDecl,
		   xmlSchemaParserCtxtPtr ctxt,
		   const xmlChar *name)
{
    if (name == NULL)
	name = attrDecl->name;
    if (attrDecl->subtypes != NULL)
	return;
    if (attrDecl->typeName != NULL) {
	xmlSchemaTypePtr type;

	type = xmlSchemaGetType(ctxt->schema, attrDecl->typeName,
				    attrDecl->typeNs);
	if (type == NULL) {
	    if ((ctxt != NULL) && (ctxt->error != NULL))
		ctxt->error(ctxt->userData,
		    "Schemas: attribute %s type %s not found\n",
			    name, attrDecl->typeName);
	}
	attrDecl->subtypes = type;
    } else if (attrDecl->ref != NULL) {
	xmlSchemaAttributePtr ref;

	ref = xmlHashLookup2(ctxt->schema->attrDecl, attrDecl->ref,
		             attrDecl->refNs);
	if (ref == NULL) {
	    if ((ctxt != NULL) && (ctxt->error != NULL))
		ctxt->error(ctxt->userData,
		    "Schemas: attribute %s reference %s not found\n",
			    name, attrDecl->ref);
	    return;
	}
	xmlSchemaAttrFixup(ref, ctxt, NULL);
	attrDecl->subtypes = ref->subtypes;
    } else {
	if ((ctxt != NULL) && (ctxt->error != NULL))
	    ctxt->error(ctxt->userData,
		"Schemas: attribute %s has no type nor reference\n",
			name);
    }
}

/**
 * xmlSchemaParse:
 * @ctxt:  a schema validation context
 *
 * parse a schema definition resource and build an internal
 * XML Shema struture which can be used to validate instances.
 * *WARNING* this interface is highly subject to change
 *
 * Returns the internal XML Schema structure built from the resource or
 *         NULL in case of error
 */
xmlSchemaPtr
xmlSchemaParse(xmlSchemaParserCtxtPtr ctxt)
{
    xmlSchemaPtr ret = NULL;
    xmlDocPtr doc;
    xmlNodePtr root, cur, delete;

    xmlSchemaInitTypes();

    if (ctxt == NULL)
        return (NULL);

    ctxt->counter = 0;
    ctxt->container = NULL;

    /*
     * First step is to parse the input document into an DOM/Infoset
     */
    if (ctxt->URL != NULL) {
	doc = xmlParseFile((const char *) ctxt->URL);
	if (doc == NULL) {
	    if (ctxt->error != NULL)
		ctxt->error(ctxt->userData,
			    "xmlSchemaParse: could not load %s\n", ctxt->URL);
	    return (NULL);
	}
    } else if (ctxt->buffer != NULL) {
	doc = xmlParseMemory(ctxt->buffer, ctxt->size);
	if (doc == NULL) {
	    if (ctxt->error != NULL)
		ctxt->error(ctxt->userData,
			    "xmlSchemaParse: could not parse schemas\n");
	    return (NULL);
	}
	doc->URL = xmlStrdup(BAD_CAST "in_memory_buffer");
	ctxt->URL = xmlStrdup(BAD_CAST "in_memory_buffer");
    } else {
	if (ctxt->error != NULL)
	    ctxt->error(ctxt->userData,
			"xmlSchemaParse: nothing to parse\n");
	return (NULL);
    }

    /*
     * Then extract the root and Schema parse it
     */
    root = xmlDocGetRootElement(doc);
    if (root == NULL) {
        if (ctxt->error != NULL)
            ctxt->error(ctxt->userData, "xmlSchemaParse: %s is empty\n",
                        ctxt->URL);
        return (NULL);
    }

    /*
     * Remove all the blank text nodes
     */
    delete = NULL;
    cur = root;
    while (cur != NULL) {
	if (delete != NULL) {
	    xmlUnlinkNode(delete);
	    xmlFreeNode(delete);
	    delete = NULL;
	}
	if (cur->type == XML_TEXT_NODE) {
	    if (IS_BLANK_NODE(cur)) {
		if (xmlNodeGetSpacePreserve(cur) != 1) {
		    delete = cur;
		}
	    }
	} else if ((cur->type != XML_ELEMENT_NODE) &&
		   (cur->type != XML_CDATA_SECTION_NODE)) {
	    delete = cur;
	    goto skip_children;
	}

	/*
	 * Skip to next node
	 */
	if (cur->children != NULL) {
	    if ((cur->children->type != XML_ENTITY_DECL) &&
		(cur->children->type != XML_ENTITY_REF_NODE) &&
		(cur->children->type != XML_ENTITY_NODE)) {
		cur = cur->children;
		continue;
	    }
	}
skip_children:
	if (cur->next != NULL) {
	    cur = cur->next;
	    continue;
	}
	
	do {
	    cur = cur->parent;
	    if (cur == NULL)
		break;
	    if (cur == root) {
		cur = NULL;
		break;
	    }
	    if (cur->next != NULL) {
		cur = cur->next;
		break;
	    }
	} while (cur != NULL);
    }
    if (delete != NULL) {
	xmlUnlinkNode(delete);
	xmlFreeNode(delete);
	delete = NULL;
    }

    /*
     * Then do the parsing for good
     */
    ret = xmlSchemaParseSchema(ctxt, root);
    if (ret == NULL)
	return(NULL);
    ret->doc = doc;

    /*
     * Then fix all the references.
     */
    ctxt->schema = ret;
    xmlHashScanFull(ret->elemDecl,
	            (xmlHashScannerFull) xmlSchemaRefFixupCallback, ctxt);

    /*
     * Then fixup all types properties
     */
    xmlHashScan(ret->typeDecl, (xmlHashScanner) xmlSchemaTypeFixup, ctxt);

    /*
     * Then build the content model for all elements
     */
    xmlHashScan(ret->elemDecl,
	            (xmlHashScanner) xmlSchemaBuildContentModel, ctxt);

    /*
     * Then check the defaults part of the type like facets values
     */
    xmlHashScan(ret->typeDecl, (xmlHashScanner) xmlSchemaCheckDefaults, ctxt);

    /*
     * Then fixup all attributes declarations
     */
    xmlHashScan(ret->attrDecl, (xmlHashScanner) xmlSchemaAttrFixup, ctxt);

    /*
     * Then fixup all attributes group declarations
     */
    xmlHashScan(ret->attrgrpDecl, (xmlHashScanner) xmlSchemaAttrGrpFixup, ctxt);

    return (ret);
}
 
/**
 * xmlSchemaSetParserErrors:
 * @ctxt:  a schema validation context
 * @err:  the error callback
 * @warn:  the warning callback
 * @ctx:  contextual data for the callbacks
 *
 * Set the callback functions used to handle errors for a validation context
 */
void
xmlSchemaSetParserErrors(xmlSchemaParserCtxtPtr ctxt,
	xmlSchemaValidityErrorFunc err,
	xmlSchemaValidityWarningFunc warn, void *ctx) {
    if (ctxt == NULL)
	return;
    ctxt->error = err;
    ctxt->warning = warn;
    ctxt->userData = ctx;
}

/************************************************************************
 * 									*
 * 			Simple type validation				*
 * 									*
 ************************************************************************/

/**
 * xmlSchemaValidateSimpleValue:
 * @ctxt:  a schema validation context
 * @type:  the type declaration
 * @value:  the value to validate
 *
 * Validate a value against a simple type
 *
 * Returns 0 if the value is valid, a positive error code
 *     number otherwise and -1 in case of internal or API error.
 */
static int
xmlSchemaValidateSimpleValue(xmlSchemaValidCtxtPtr ctxt, 
			     xmlSchemaTypePtr type,
	                     xmlChar *value) {
    int ret = 0;
    /*
     * First normalize the value accordingly to Schema Datatype
     * 4.3.6 whiteSpace definition of the whiteSpace facet of type
     */
    /*
     * Then check the normalized value against the lexical space of the
     * type.
     */
    if (type->type == XML_SCHEMA_TYPE_BASIC) {
	if (ctxt->value != NULL) {
	    xmlSchemaFreeValue(ctxt->value);
	    ctxt->value = NULL;
	}
	ret = xmlSchemaValidatePredefinedType(type, value, &(ctxt->value));
    } else if (type->type == XML_SCHEMA_TYPE_RESTRICTION) {
	xmlSchemaTypePtr base;
	xmlSchemaFacetPtr facet;
	int tmp;

	base = type->baseType;
	if (base != NULL) {
	    ret = xmlSchemaValidateSimpleValue(ctxt, base, value);
	} else if (type->subtypes != NULL) {
	    
	}
	/*
	 * Do not validate facets when working on building the Schemas
	 */
	if (ctxt->schema != NULL) {
	    if (ret == 0) {
		facet = type->facets;
		if ((type->type == XML_SCHEMA_TYPE_RESTRICTION) &&
		    (facet != NULL) &&
		    (facet->type == XML_SCHEMA_FACET_ENUMERATION)) {
		    while (facet != NULL) {
			ret = 1;

			tmp = xmlSchemaValidateFacet(base, facet, value,
						     ctxt->value);
			if (tmp == 0) {
			    ret = 0;
			    break;
			}
			facet = facet->next;
		    }
		} else {
		    while (facet != NULL) {
			tmp = xmlSchemaValidateFacet(base, facet, value,
						     ctxt->value);
			if (tmp != 0)
			    ret = tmp;
			facet = facet->next;
		    }
		}
	    }
	}
    } else if (type->type == XML_SCHEMA_TYPE_SIMPLE) {
	xmlSchemaTypePtr base;

	base = type->subtypes;
	if (base != NULL) {
	    ret = xmlSchemaValidateSimpleValue(ctxt, base, value);
	} else {
	    TODO
	}
    } else if (type->type == XML_SCHEMA_TYPE_LIST) {
	xmlSchemaTypePtr base;
	xmlChar *cur, *end, tmp;
	int ret2;

	base = type->subtypes;
	if (base == NULL) {
	    ctxt->err = XML_SCHEMAS_ERR_INTERNAL;
	    if (ctxt->error != NULL) {
		xmlSchemaErrorContext(NULL, ctxt->schema, type->node, NULL);
		ctxt->error(ctxt->userData,
		"Internal: List type %s has no base type\n",
			    type->name);
	    }
	    return(-1);
	}
	cur = value;
	do {
	    while (IS_BLANK(*cur)) cur++;
	    end = cur;
	    while ((*end != 0) && (!(IS_BLANK(*end)))) end++;
	    if (end == cur)
		break;
	    tmp = *end;
	    *end = 0;
	    ret2 = xmlSchemaValidateSimpleValue(ctxt, base, cur);
	    if (ret2 != 0)
		ret = 1;
	    *end = tmp;
	    cur = end;
	} while (*cur != 0);
    } else {
	TODO
    }
    return(ret);
}

/************************************************************************
 * 									*
 * 			DOM Validation code				*
 * 									*
 ************************************************************************/

static int xmlSchemaValidateContent(xmlSchemaValidCtxtPtr ctxt,
	                            xmlNodePtr node);
static int xmlSchemaValidateAttributes(xmlSchemaValidCtxtPtr ctxt,
	xmlNodePtr elem, xmlSchemaAttributePtr attributes);
static int xmlSchemaValidateType(xmlSchemaValidCtxtPtr ctxt,
	xmlNodePtr elem, xmlSchemaElementPtr elemDecl, xmlSchemaTypePtr type);

/**
 * xmlSchemaRegisterAttributes:
 * @ctxt:  a schema validation context
 * @attrs:  a list of attributes
 *
 * Register the list of attributes as the set to be validated on that element
 *
 * Returns -1 in case of error, 0 otherwise
 */
static int
xmlSchemaRegisterAttributes(xmlSchemaValidCtxtPtr ctxt,
	                    xmlAttrPtr attrs) {
    while (attrs != NULL) {
	if ((attrs->ns != NULL) &&
	    (xmlStrEqual(attrs->ns->href, xmlSchemaInstanceNs))) {
	    attrs = attrs->next;
	    continue;
	}
	if (ctxt->attrNr >= ctxt->attrMax) {
	    xmlSchemaAttrStatePtr tmp;

	    ctxt->attrMax *= 2;
	    tmp = (xmlSchemaAttrStatePtr)
		          xmlRealloc(ctxt->attr, ctxt->attrMax *
				     sizeof(xmlSchemaAttrState));
	    if (tmp == NULL) {
		ctxt->attrMax /= 2;
		return(-1);
	    }
	    ctxt->attr = tmp;
	}
	ctxt->attr[ctxt->attrNr].attr = attrs;
	ctxt->attr[ctxt->attrNr].state = XML_SCHEMAS_ATTR_UNKNOWN;
	ctxt->attrNr++;
	attrs = attrs->next;
    }
    return(0);
}

/**
 * xmlSchemaCheckAttributes:
 * @ctxt:  a schema validation context
 * @node:  the node carrying it.
 *
 * Check that the registered set of attributes on the current node
 * has been properly validated.
 *
 * Returns 0 if validity constraints are met, 1 otherwise.
 */
static int
xmlSchemaCheckAttributes(xmlSchemaValidCtxtPtr ctxt, xmlNodePtr node) {
    int ret = 0;
    int i;

    for (i = ctxt->attrBase;i < ctxt->attrNr;i++) {
	if (ctxt->attr[i].attr == NULL)
	    break;
	if (ctxt->attr[i].state == XML_SCHEMAS_ATTR_UNKNOWN) {
	    ret = 1;
	    ctxt->err = XML_SCHEMAS_ERR_ATTRUNKNOWN;
	    if (ctxt->error != NULL)
		ctxt->error(ctxt->userData,
		"Attribute %s on %s is unknown\n",
			    ctxt->attr[i].attr->name,
			    node->name);
	}
    }
    return(ret);
}

/**
 * xmlSchemaValidateSimpleContent:
 * @ctxt:  a schema validation context
 * @elem:  an element
 * @type:  the type declaration
 *
 * Validate the content of an element expected to be a simple type
 *
 * Returns 0 if the element is schemas valid, a positive error code
 *     number otherwise and -1 in case of internal or API error.
 */
static int
xmlSchemaValidateSimpleContent(xmlSchemaValidCtxtPtr ctxt,
	                       xmlNodePtr node ATTRIBUTE_UNUSED) {
    xmlNodePtr child;
    xmlSchemaTypePtr type, base;
    xmlChar *value;
    int ret = 0, tmp;

    child = ctxt->node;
    type = ctxt->type;

    /*
     * Validation Rule: Element Locally Valid (Type): 3.1.3
     */
    value = xmlNodeGetContent(child);
    /* xmlSchemaValidateSimpleValue(ctxt, type, value); */
    switch (type->type) {
	case XML_SCHEMA_TYPE_RESTRICTION: {
	    xmlSchemaFacetPtr facet;

	    base = type->baseType;
	    if (base != NULL) {
		ret = xmlSchemaValidateSimpleValue(ctxt, base, value);
	    } else {
		TODO
	    }
	    if (ret == 0) {
		facet = type->facets;
		while (facet != NULL) {
		    tmp = xmlSchemaValidateFacet(base, facet, value,
			                         ctxt->value);
		    if (tmp != 0)
			ret = tmp;
		    facet = facet->next;
		}
	    }
	    break;
	}
	default:
	    TODO
    }
    if (value != NULL)
	xmlFree(value);

    return(ret);
}

/**
 * xmlSchemaValidateCheckNodeList
 * @nodelist: the list of nodes
 *
 * Check the node list is only made of text nodes and entities pointing
 * to text nodes
 *
 * Returns 1 if true, 0 if false and -1 in case of error
 */
static int
xmlSchemaValidateCheckNodeList(xmlNodePtr nodelist) {
    while (nodelist != NULL) {
	if (nodelist->type == XML_ENTITY_REF_NODE) {
	    TODO /* implement recursion in the entity content */
	}
	if ((nodelist->type != XML_TEXT_NODE) &&
	    (nodelist->type != XML_COMMENT_NODE) &&
	    (nodelist->type != XML_PI_NODE) &&
	    (nodelist->type != XML_PI_NODE)) {
	    return(0);
	}
	nodelist = nodelist->next;
    }
    return(1);
}

/**
 * xmlSchemaSkipIgnored:
 * @ctxt:  a schema validation context
 * @type:  the current type context
 * @node:  the top node.
 *
 * Skip ignorable nodes in that context
 *
 * Returns the new sibling
 *     number otherwise and -1 in case of internal or API error.
 */
static xmlNodePtr
xmlSchemaSkipIgnored(xmlSchemaValidCtxtPtr ctxt ATTRIBUTE_UNUSED,
	             xmlSchemaTypePtr type,
	             xmlNodePtr node) {
    int mixed = 0;
    /*
     * TODO complete and handle entities
     */
    mixed = ((type->contentType == XML_SCHEMA_CONTENT_MIXED) ||
	     (type->contentType == XML_SCHEMA_CONTENT_MIXED_OR_ELEMENTS));
    while ((node != NULL) &&
	   ((node->type == XML_COMMENT_NODE) ||
	    ((mixed == 1) && (node->type == XML_TEXT_NODE)) ||
	    (((type->contentType == XML_SCHEMA_CONTENT_ELEMENTS) &&
	     (node->type == XML_TEXT_NODE) &&
	     (IS_BLANK_NODE(node)))))) {
	node = node->next;
    }
    return(node);
}

/**
 * xmlSchemaValidateCallback:
 * @ctxt:  a schema validation context
 * @name:  the name of the element detected (might be NULL)
 * @type:  the type
 *
 * A transition has been made in the automata associated to an element
 * content model
 */
static void
xmlSchemaValidateCallback(xmlSchemaValidCtxtPtr ctxt,
	                  const xmlChar *name ATTRIBUTE_UNUSED,
			  xmlSchemaTypePtr type,
			  xmlNodePtr node) {
    xmlSchemaTypePtr oldtype = ctxt->type;
    xmlNodePtr oldnode = ctxt->node;
#ifdef DEBUG_CONTENT
    xmlGenericError(xmlGenericErrorContext,
                    "xmlSchemaValidateCallback: %s, %s, %s\n",
		    name, type->name, node->name);
#endif
    ctxt->type = type;
    ctxt->node = node;
    xmlSchemaValidateContent(ctxt, node);
    ctxt->type = oldtype;
    ctxt->node = oldnode;
}


#if 0
/**
 * xmlSchemaValidateSimpleRestrictionType:
 * @ctxt:  a schema validation context
 * @node:  the top node.
 *
 * Validate the content of a restriction type.
 *
 * Returns 0 if the element is schemas valid, a positive error code
 *     number otherwise and -1 in case of internal or API error.
 */
static int
xmlSchemaValidateSimpleRestrictionType(xmlSchemaValidCtxtPtr ctxt,
                                       xmlNodePtr node)
{
    xmlNodePtr child;
    xmlSchemaTypePtr type;
    int ret;

    child = ctxt->node;
    type = ctxt->type;

    if ((ctxt == NULL) || (type == NULL)) {
        ctxt->err = XML_SCHEMAS_ERR_INTERNAL;
        if (ctxt->error != NULL)
            ctxt->error(ctxt->userData,
	    "Internal error: xmlSchemaValidateSimpleRestrictionType %s\n",
                        node->name);
        return (-1);
    }
    /*
     * Only text and text based entities references shall be found there
     */
    ret = xmlSchemaValidateCheckNodeList(child);
    if (ret < 0) {
        ctxt->err = XML_SCHEMAS_ERR_INTERNAL;
        if (ctxt->error != NULL)
            ctxt->error(ctxt->userData,
		"Internal error: xmlSchemaValidateSimpleType %s content\n",
                        node->name);
        return (-1);
    } else if (ret == 0) {
        ctxt->err = XML_SCHEMAS_ERR_NOTSIMPLE;
        if (ctxt->error != NULL)
            ctxt->error(ctxt->userData,
                        "Element %s content is not a simple type\n",
                        node->name);
        return (-1);
    }
    ctxt->type = type->subtypes;
    xmlSchemaValidateContent(ctxt, node);
    ctxt->type = type;
    return (ret);
}
#endif

/**
 * xmlSchemaValidateSimpleType:
 * @ctxt:  a schema validation context
 * @node:  the top node.
 *
 * Validate the content of an simple type.
 *
 * Returns 0 if the element is schemas valid, a positive error code
 *     number otherwise and -1 in case of internal or API error.
 */
static int
xmlSchemaValidateSimpleType(xmlSchemaValidCtxtPtr ctxt, xmlNodePtr node) {
    xmlNodePtr child;
    xmlSchemaTypePtr type;
    xmlAttrPtr attr;
    int ret;

    child = ctxt->node;
    type = ctxt->type;

    if ((ctxt == NULL) || (type == NULL)) {
	ctxt->err = XML_SCHEMAS_ERR_INTERNAL;
	if (ctxt->error != NULL)
	    ctxt->error(ctxt->userData,
		    "Internal error: xmlSchemaValidateSimpleType %s\n",
			node->name);
	return(-1);
    }
    /*
     * Only text and text based entities references shall be found there
     */
    ret = xmlSchemaValidateCheckNodeList(child);
    if (ret < 0) {
	ctxt->err = XML_SCHEMAS_ERR_INTERNAL;
	if (ctxt->error != NULL)
	    ctxt->error(ctxt->userData,
		    "Internal error: xmlSchemaValidateSimpleType %s content\n",
			node->name);
	return(-1);
    } else if (ret == 0) {
	ctxt->err = XML_SCHEMAS_ERR_NOTSIMPLE;
	if (ctxt->error != NULL)
	    ctxt->error(ctxt->userData,
		    "Element %s content is not a simple type\n",
			node->name);
	return(-1);
    }
    /*
     * Validation Rule: Element Locally Valid (Type): 3.1.1
     */
    attr = node->properties;
    while (attr != NULL) {
	if ((attr->ns == NULL) ||
	    (!xmlStrEqual(attr->ns->href, xmlSchemaInstanceNs)) ||
	    ((!xmlStrEqual(attr->name, BAD_CAST"type")) &&
	     (!xmlStrEqual(attr->name, BAD_CAST"nil")) &&
	     (!xmlStrEqual(attr->name, BAD_CAST"schemasLocation")) &&
	     (!xmlStrEqual(attr->name, BAD_CAST"noNamespaceSchemaLocation")))) {
	    ctxt->err = XML_SCHEMAS_ERR_INVALIDATTR;
	    if (ctxt->error != NULL)
		ctxt->error(ctxt->userData,
			"Element %s: attribute %s should not be present\n",
			    child->name, attr->name);
	    return(ctxt->err);
	}
    }

    ctxt->type = type->subtypes;
    ret = xmlSchemaValidateSimpleContent(ctxt, node);
    ctxt->type = type;
    return(ret);
}

/**
 * xmlSchemaValidateElementType:
 * @ctxt:  a schema validation context
 * @node:  the top node.
 *
 * Validate the content of an element type.
 * Validation Rule: Element Locally Valid (Complex Type)
 *
 * Returns 0 if the element is schemas valid, a positive error code
 *     number otherwise and -1 in case of internal or API error.
 */
static int
xmlSchemaValidateElementType(xmlSchemaValidCtxtPtr ctxt, xmlNodePtr node) {
    xmlNodePtr child;
    xmlSchemaTypePtr type;
    xmlRegExecCtxtPtr oldregexp; /* cont model of the parent */
    xmlSchemaElementPtr decl;
    int ret, attrBase;

    oldregexp = ctxt->regexp;

    child = ctxt->node;
    type = ctxt->type;

    if ((ctxt == NULL) || (type == NULL)) {
	ctxt->err = XML_SCHEMAS_ERR_INTERNAL;
	if (ctxt->error != NULL)
	    ctxt->error(ctxt->userData,
		    "Internal error: xmlSchemaValidateElementType\n",
			node->name);
	return(-1);
    }
    if (child == NULL) {
	if (type->minOccurs > 0) {
	    ctxt->err = XML_SCHEMAS_ERR_MISSING;
	    if (ctxt->error != NULL)
		ctxt->error(ctxt->userData,
			"Element %s: missing child %s\n",
			    node->name, type->name);
	}
	return(ctxt->err);
    }

    /*
     * Verify the element matches
     */
    if (!xmlStrEqual(child->name, type->name)) {
	ctxt->err = XML_SCHEMAS_ERR_WRONGELEM;
	if (ctxt->error != NULL)
	    ctxt->error(ctxt->userData,
		    "Element %s: missing child %s found %s\n",
			node->name, type->name, child->name);
        return(ctxt->err);
    }
    /*
     * Verify the attributes
     */
    attrBase = ctxt->attrBase;
    ctxt->attrBase = ctxt->attrNr;
    xmlSchemaRegisterAttributes(ctxt, child->properties);
    xmlSchemaValidateAttributes(ctxt, child, type->attributes);
    /*
     * Verify the element content recursively
     */
    decl = (xmlSchemaElementPtr) type;
    oldregexp = ctxt->regexp;
    if (decl->contModel != NULL) {
	ctxt->regexp = xmlRegNewExecCtxt(decl->contModel,
			     (xmlRegExecCallbacks) xmlSchemaValidateCallback,
			     ctxt);
#ifdef DEBUG_AUTOMATA
	xmlGenericError(xmlGenericErrorContext,
		"====> %s\n", node->name);
#endif
    }
    xmlSchemaValidateType(ctxt, child, (xmlSchemaElementPtr)type,
	                  type->subtypes);

    if (decl->contModel != NULL) {
	ret = xmlRegExecPushString(ctxt->regexp, NULL, NULL);
#ifdef DEBUG_AUTOMATA
	xmlGenericError(xmlGenericErrorContext,
		"====> %s : %d\n", node->name, ret);
#endif
	if (ret == 0) {
	    ctxt->err = XML_SCHEMAS_ERR_ELEMCONT;
	    if (ctxt->error != NULL)
		ctxt->error(ctxt->userData, "Element %s content check failed\n",
			    node->name);
	} else if (ret < 0) {
	    ctxt->err = XML_SCHEMAS_ERR_ELEMCONT;
	    if (ctxt->error != NULL)
		ctxt->error(ctxt->userData, "Element %s content check failure\n",
			    node->name);
#ifdef DEBUG_CONTENT
	} else {
	    xmlGenericError(xmlGenericErrorContext,
			    "Element %s content check succeeded\n", node->name);

#endif
	}
	xmlRegFreeExecCtxt(ctxt->regexp);
    }
    /*
     * Verify that all attributes were Schemas-validated
     */
    xmlSchemaCheckAttributes(ctxt, node);
    ctxt->attrNr = ctxt->attrBase;
    ctxt->attrBase = attrBase;

    ctxt->regexp = oldregexp;

    ctxt->node = child;
    ctxt->type = type;
    return(ctxt->err);
}

/**
 * xmlSchemaValidateBasicType:
 * @ctxt:  a schema validation context
 * @node:  the top node.
 *
 * Validate the content of an element expected to be a basic type type
 *
 * Returns 0 if the element is schemas valid, a positive error code
 *     number otherwise and -1 in case of internal or API error.
 */
static int
xmlSchemaValidateBasicType(xmlSchemaValidCtxtPtr ctxt, xmlNodePtr node) {
    int ret;
    xmlNodePtr child, cur;
    xmlSchemaTypePtr type;
    xmlChar *value;     /* lexical representation */

    child = ctxt->node;
    type = ctxt->type;

    if ((ctxt == NULL) || (type == NULL)) {
	ctxt->err = XML_SCHEMAS_ERR_INTERNAL;
	if (ctxt->error != NULL)
	    ctxt->error(ctxt->userData,
		    "Internal error: xmlSchemaValidateBasicType\n",
			node->name);
	return(-1);
    }
    /*
     * First check the content model of the node.
     */
    cur = child;
    while (cur != NULL) {
	switch (cur->type) {
	    case XML_TEXT_NODE:
	    case XML_CDATA_SECTION_NODE:
	    case XML_PI_NODE:
	    case XML_COMMENT_NODE:
	    case XML_XINCLUDE_START:
	    case XML_XINCLUDE_END:
	        break;
	    case XML_ENTITY_REF_NODE:
	    case XML_ENTITY_NODE:
	        TODO
		break;
	    case XML_ELEMENT_NODE:
		ctxt->err = XML_SCHEMAS_ERR_INVALIDELEM;
		if (ctxt->error != NULL)
		    ctxt->error(ctxt->userData,
			    "Element %s: child %s should not be present\n",
				node->name, cur->name);
		return(ctxt->err);
            case XML_ATTRIBUTE_NODE:
            case XML_DOCUMENT_NODE:
            case XML_DOCUMENT_TYPE_NODE:
            case XML_DOCUMENT_FRAG_NODE:
            case XML_NOTATION_NODE:
            case XML_HTML_DOCUMENT_NODE:
            case XML_DTD_NODE:
            case XML_ELEMENT_DECL:
            case XML_ATTRIBUTE_DECL:
            case XML_ENTITY_DECL:
            case XML_NAMESPACE_DECL:
#ifdef LIBXML_DOCB_ENABLED
            case XML_DOCB_DOCUMENT_NODE:
#endif
		ctxt->err = XML_SCHEMAS_ERR_INVALIDELEM;
		if (ctxt->error != NULL)
		    ctxt->error(ctxt->userData,
			    "Element %s: node type %d unexpected here\n",
				node->name, cur->type);
		return(ctxt->err);
	}
	cur = cur->next;
    }
    if (child == NULL)
	value = NULL;
    else
	value = xmlNodeGetContent(child->parent);

    if (ctxt->value != NULL) {
	xmlSchemaFreeValue(ctxt->value);
	ctxt->value = NULL;
    }
    ret = xmlSchemaValidatePredefinedType(type, value, &(ctxt->value));
    if (value != NULL)
	xmlFree(value);
    if (ret != 0) {
	ctxt->error(ctxt->userData,
		"Element %s: failed to validate basic type %s\n",
		    node->name, type->name);
    }
    return(ret);
}

/**
 * xmlSchemaValidateComplexType:
 * @ctxt:  a schema validation context
 * @node:  the top node.
 *
 * Validate the content of an element expected to be a complex type type
 * xmlschema-1.html#cvc-complex-type
 * Validation Rule: Element Locally Valid (Complex Type)
 *
 * Returns 0 if the element is schemas valid, a positive error code
 *     number otherwise and -1 in case of internal or API error.
 */
static int
xmlSchemaValidateComplexType(xmlSchemaValidCtxtPtr ctxt, xmlNodePtr node) {
    xmlNodePtr child;
    xmlSchemaTypePtr type, subtype;
    int ret;

    child = ctxt->node;
    type = ctxt->type;

    switch (type->contentType) {
	case XML_SCHEMA_CONTENT_EMPTY:
	    if (child != NULL) {
		if (ctxt->error != NULL)
		    ctxt->error(ctxt->userData,
			    "Element %s is supposed to be empty\n",
			        node->name);
	    }
	    if (type->attributes != NULL) {
		xmlSchemaValidateAttributes(ctxt, node, type->attributes);
	    }
	    subtype = type->subtypes;
	    while (subtype != NULL) {
		ctxt->type = subtype;
		xmlSchemaValidateComplexType(ctxt, node);
		subtype = subtype->next;
	    }
	    break;
	case XML_SCHEMA_CONTENT_ELEMENTS:
	case XML_SCHEMA_CONTENT_MIXED:
	case XML_SCHEMA_CONTENT_MIXED_OR_ELEMENTS:
	    /*
	     * Skip ignorable nodes in that context
	     */
	    child = xmlSchemaSkipIgnored(ctxt, type, child);
	    while (child != NULL) {
		if (child->type == XML_ELEMENT_NODE) {
		    ret = xmlRegExecPushString(ctxt->regexp,
					       child->name, child);
#ifdef DEBUG_AUTOMATA
		    if (ret < 0)
			xmlGenericError(xmlGenericErrorContext,
				"  --> %s Error\n", child->name);
		    else
			xmlGenericError(xmlGenericErrorContext,
				"  --> %s\n", child->name);
#endif
		}
		child = child->next;
		/*
		 * Skip ignorable nodes in that context
		 */
		child = xmlSchemaSkipIgnored(ctxt, type, child);
	    }
	    break;
	case XML_SCHEMA_CONTENT_BASIC: {
	    if (type->subtypes != NULL) {
		ctxt->type = type->subtypes;
		xmlSchemaValidateComplexType(ctxt, node);
	    }
	    if (type->baseType != NULL) {
		ctxt->type = type->baseType;
		xmlSchemaValidateBasicType(ctxt, node);
	    }
	    if (type->attributes != NULL) {
		xmlSchemaValidateAttributes(ctxt, node, type->attributes);
	    }
	    ctxt->type = type;
	    break;
        }
	default:
	    TODO
	    xmlGenericError(xmlGenericErrorContext,
		    "unimplemented content type %d\n",
		            type->contentType);
    }
    return(ctxt->err);
}

/**
 * xmlSchemaValidateContent:
 * @ctxt:  a schema validation context
 * @elem:  an element
 * @type:  the type declaration
 *
 * Validate the content of an element against the type.
 *
 * Returns 0 if the element is schemas valid, a positive error code
 *     number otherwise and -1 in case of internal or API error.
 */
static int
xmlSchemaValidateContent(xmlSchemaValidCtxtPtr ctxt, xmlNodePtr node) {
    xmlNodePtr child;
    xmlSchemaTypePtr type;

    child = ctxt->node;
    type = ctxt->type;

    xmlSchemaValidateAttributes(ctxt, node, type->attributes);

    switch (type->type) {
	case XML_SCHEMA_TYPE_ANY:
	    /* Any type will do it, fine */
	    TODO /* handle recursivity */
	    break;
	case XML_SCHEMA_TYPE_COMPLEX:
	    xmlSchemaValidateComplexType(ctxt, node);
	    break;
	case XML_SCHEMA_TYPE_ELEMENT: {
	    xmlSchemaElementPtr decl = (xmlSchemaElementPtr) type;
	    /*
	     * Handle element reference here
	     */
	    if (decl->ref != NULL) {
		if (decl->refDecl == NULL) {
		    ctxt->err = XML_SCHEMAS_ERR_INTERNAL;
		    if (ctxt->error != NULL)
			ctxt->error(ctxt->userData,
		    "Internal error: element reference %s not resolved\n",
				    decl->ref);
		    return(-1);
		}
		ctxt->type = (xmlSchemaTypePtr) decl->refDecl;
		decl = decl->refDecl;
	    }
	    xmlSchemaValidateElementType(ctxt, node);
	    ctxt->type = type;
	    break;
	}
	case XML_SCHEMA_TYPE_BASIC:
	    xmlSchemaValidateBasicType(ctxt, node);
	    break;
	case XML_SCHEMA_TYPE_FACET:
	    TODO
	    break;
	case XML_SCHEMA_TYPE_SIMPLE:
	    xmlSchemaValidateSimpleType(ctxt, node);
	    break;
	case XML_SCHEMA_TYPE_SEQUENCE:
	    TODO
	    break;
	case XML_SCHEMA_TYPE_CHOICE:
	    TODO
	    break;
	case XML_SCHEMA_TYPE_ALL:
	    TODO
	    break;
	case XML_SCHEMA_TYPE_SIMPLE_CONTENT:
	    TODO
	    break;
	case XML_SCHEMA_TYPE_COMPLEX_CONTENT:
	    TODO
	    break;
	case XML_SCHEMA_TYPE_UR:
	    TODO
	    break;
	case XML_SCHEMA_TYPE_RESTRICTION:
	    /*xmlSchemaValidateRestrictionType(ctxt, node); */
	    TODO
	    break;
	case XML_SCHEMA_TYPE_EXTENSION:
	    TODO
	    break;
	case XML_SCHEMA_TYPE_ATTRIBUTE:
	    TODO
	    break;
	case XML_SCHEMA_TYPE_GROUP:
	    TODO
	    break;
	case XML_SCHEMA_TYPE_NOTATION:
	    TODO
	    break;
	case XML_SCHEMA_TYPE_LIST:
	    TODO
	    break;
	case XML_SCHEMA_TYPE_UNION:
	    TODO
	    break;
	case XML_SCHEMA_FACET_MININCLUSIVE:
	    TODO
	    break;
	case XML_SCHEMA_FACET_MINEXCLUSIVE:
	    TODO
	    break;
	case XML_SCHEMA_FACET_MAXINCLUSIVE:
	    TODO
	    break;
	case XML_SCHEMA_FACET_MAXEXCLUSIVE:
	    TODO
	    break;
	case XML_SCHEMA_FACET_TOTALDIGITS:
	    TODO
	    break;
	case XML_SCHEMA_FACET_FRACTIONDIGITS:
	    TODO
	    break;
	case XML_SCHEMA_FACET_PATTERN:
	    TODO
	    break;
	case XML_SCHEMA_FACET_ENUMERATION:
	    TODO
	    break;
	case XML_SCHEMA_FACET_WHITESPACE:
	    TODO
	    break;
	case XML_SCHEMA_FACET_LENGTH:
	    TODO
	    break;
	case XML_SCHEMA_FACET_MAXLENGTH:
	    TODO
	    break;
	case XML_SCHEMA_FACET_MINLENGTH:
	    TODO
	    break;
	case XML_SCHEMA_TYPE_ATTRIBUTEGROUP:
	    TODO
	    break;
    }
    xmlSchemaValidateAttributes(ctxt, node, type->attributes);

    if (ctxt->node == NULL)
	return(ctxt->err);
    ctxt->node = ctxt->node->next;
    ctxt->type = type->next;
    return(ctxt->err);
}

/**
 * xmlSchemaValidateType:
 * @ctxt:  a schema validation context
 * @elem:  an element
 * @type:  the list of type declarations
 *
 * Validate the content of an element against the types.
 *
 * Returns 0 if the element is schemas valid, a positive error code
 *     number otherwise and -1 in case of internal or API error.
 */
static int
xmlSchemaValidateType(xmlSchemaValidCtxtPtr ctxt, xmlNodePtr elem,
	                  xmlSchemaElementPtr elemDecl,
			  xmlSchemaTypePtr type) {
    xmlChar *nil;

    if ((elem->content == NULL) || (type == NULL) || (elemDecl == NULL))
	return(0);
    /*
     * 3.3.4 : 2
     */
    if (elemDecl->flags & XML_SCHEMAS_ELEM_ABSTRACT) {
	ctxt->err = XML_SCHEMAS_ERR_ISABSTRACT;
        if (ctxt->error != NULL)
            ctxt->error(ctxt->userData, "Element %s is abstract\n", elem->name);
	return(ctxt->err);
    }
    /*
     * 3.3.4: 3
     */
    nil = xmlGetNsProp(elem, BAD_CAST "nil", xmlSchemaInstanceNs);
    if (elemDecl->flags & XML_SCHEMAS_ELEM_NILLABLE) {
	/* 3.3.4: 3.2 */
	if (xmlStrEqual(nil, BAD_CAST "true")) {
	    if (elem->children != NULL) {
		ctxt->err = XML_SCHEMAS_ERR_NOTEMPTY;
		if (ctxt->error != NULL)
		    ctxt->error(ctxt->userData, "Element %s is not empty\n",
			        elem->name);
		return(ctxt->err);
	    }
	    if ((elemDecl->flags & XML_SCHEMAS_ELEM_FIXED) &&
		(elemDecl->value != NULL)) {
		ctxt->err = XML_SCHEMAS_ERR_HAVEDEFAULT;
		if (ctxt->error != NULL)
		    ctxt->error(ctxt->userData,
			    "Empty element %s cannot get a fixed value\n",
			        elem->name);
		return(ctxt->err);
	    }
	}
    } else {
	/* 3.3.4: 3.1 */
	if (nil != NULL) {
	    ctxt->err = XML_SCHEMAS_ERR_NOTNILLABLE;
	    if (ctxt->error != NULL)
		ctxt->error(ctxt->userData,
			"Element %s with xs:nil but not nillable\n",
			    elem->name);
	    xmlFree(nil);
	    return(ctxt->err);
	}
    }

    /* TODO 3.3.4: 4 if the element carries xs:type*/

    ctxt->type = elemDecl->subtypes;
    ctxt->node = elem->children;
    xmlSchemaValidateContent(ctxt, elem);
    xmlSchemaValidateAttributes(ctxt, elem, elemDecl->attributes);
    
    return(ctxt->err);
}


/**
 * xmlSchemaValidateAttributes:
 * @ctxt:  a schema validation context
 * @elem:  an element
 * @attributes:  the list of attribute declarations
 *
 * Validate the attributes of an element.
 *
 * Returns 0 if the element is schemas valid, a positive error code
 *     number otherwise and -1 in case of internal or API error.
 */
static int
xmlSchemaValidateAttributes(xmlSchemaValidCtxtPtr ctxt, xmlNodePtr elem,
	                    xmlSchemaAttributePtr attributes) {
    int i, ret;
    xmlAttrPtr attr;
    xmlChar *value;
    xmlSchemaAttributeGroupPtr group = NULL;

    if (attributes == NULL)
	return(0);
    while (attributes != NULL) {
	/*
	 * Handle attribute groups
	 */
	if (attributes->type == XML_SCHEMA_TYPE_ATTRIBUTEGROUP) {
	    group = (xmlSchemaAttributeGroupPtr) attributes;
	    xmlSchemaValidateAttributes(ctxt, elem, group->attributes);
	    attributes = group->next;
	    continue;
	}
	for (i = ctxt->attrBase;i < ctxt->attrNr;i++) {
	    attr = ctxt->attr[i].attr;
	    if (attr == NULL)
		continue;
	    if (!xmlStrEqual(attr->name, attributes->name))
		continue;
	    /*
	     * TODO: handle the mess about namespaces here.
	     */
	    if ((attr->ns != NULL) /* || (attributes->ns != NULL) */) {
		TODO
	    }
	    if (attributes->subtypes == NULL) {
		ctxt->err = XML_SCHEMAS_ERR_INTERNAL;
		if (ctxt->error != NULL)
		    ctxt->error(ctxt->userData,
		"Internal error: attribute %s type not resolved\n",
				attr->name);
		continue;
	    }
	    value = xmlNodeListGetString(elem->doc, attr->children, 1);
	    ret = xmlSchemaValidateSimpleValue(ctxt, attributes->subtypes,
					       value);
	    if (ret != 0) {
		ctxt->err = XML_SCHEMAS_ERR_ATTRINVALID;
		if (ctxt->error != NULL)
		    ctxt->error(ctxt->userData,
		"attribute %s on %s does not match type\n",
				attr->name, elem->name);
	    } else {
		ctxt->attr[i].state = XML_SCHEMAS_ATTR_CHECKED;
	    }
	    if (value != NULL) {
		xmlFree(value);
	    }
	}
	attributes = attributes->next;
    }
    return(ctxt->err);
}

/**
 * xmlSchemaValidateElement:
 * @ctxt:  a schema validation context
 * @elem:  an element
 *
 * Validate an element in a tree
 *
 * Returns 0 if the element is schemas valid, a positive error code
 *     number otherwise and -1 in case of internal or API error.
 */
static int
xmlSchemaValidateElement(xmlSchemaValidCtxtPtr ctxt, xmlNodePtr elem) {
    xmlSchemaElementPtr elemDecl;
    int ret, attrBase;

    if (elem->ns != NULL)
	elemDecl = xmlHashLookup3(ctxt->schema->elemDecl,
		elem->name, elem->ns->href, NULL);
    else
	elemDecl = xmlHashLookup3(ctxt->schema->elemDecl,
		elem->name, NULL, NULL);
    /*
     * 3.3.4 : 1
     */
    if (elemDecl == NULL) {
	ctxt->err = XML_SCHEMAS_ERR_UNDECLAREDELEM;
        if (ctxt->error != NULL)
            ctxt->error(ctxt->userData, "Element %s not declared\n",
                        elem->name);
	return(ctxt->err);
    }
    if (elemDecl->subtypes == NULL) {
	ctxt->err = XML_SCHEMAS_ERR_NOTYPE;
        if (ctxt->error != NULL)
            ctxt->error(ctxt->userData, "Element %s has no type\n",
                        elem->name);
	return(ctxt->err);
    }
    /*
     * Verify the attributes
     */
    attrBase = ctxt->attrBase;
    ctxt->attrBase = ctxt->attrNr;
    xmlSchemaRegisterAttributes(ctxt, elem->properties);
    xmlSchemaValidateAttributes(ctxt, elem, elemDecl->attributes);
    /*
     * Verify the element content recursively
     */
    if (elemDecl->contModel != NULL) {
	ctxt->regexp = xmlRegNewExecCtxt(elemDecl->contModel,
			     (xmlRegExecCallbacks) xmlSchemaValidateCallback,
			     ctxt);
#ifdef DEBUG_AUTOMATA
	xmlGenericError(xmlGenericErrorContext,
		"====> %s\n", elem->name);
#endif
    }
    xmlSchemaValidateType(ctxt, elem, elemDecl, elemDecl->subtypes);
    if (elemDecl->contModel != NULL) {
	ret = xmlRegExecPushString(ctxt->regexp, NULL, NULL);
#ifdef DEBUG_AUTOMATA
	xmlGenericError(xmlGenericErrorContext,
		"====> %s : %d\n", elem->name, ret);
#endif
	if (ret == 0) {
	    ctxt->err = XML_SCHEMAS_ERR_ELEMCONT;
	    if (ctxt->error != NULL)
		ctxt->error(ctxt->userData, "Element %s content check failed\n",
			    elem->name);
	} else if (ret < 0) {
	    ctxt->err = XML_SCHEMAS_ERR_ELEMCONT;
	    if (ctxt->error != NULL)
		ctxt->error(ctxt->userData, "Element %s content check failed\n",
			    elem->name);
#ifdef DEBUG_CONTENT
	} else {
	    xmlGenericError(xmlGenericErrorContext,
			    "Element %s content check succeeded\n", elem->name);

#endif
	}
	xmlRegFreeExecCtxt(ctxt->regexp);
    }
    /*
     * Verify that all attributes were Schemas-validated
     */
    xmlSchemaCheckAttributes(ctxt, elem);
    ctxt->attrNr = ctxt->attrBase;
    ctxt->attrBase = attrBase;
    
    return(ctxt->err);
}

/**
 * xmlSchemaValidateDocument:
 * @ctxt:  a schema validation context
 * @doc:  a parsed document tree
 *
 * Validate a document tree in memory.
 *
 * Returns 0 if the document is schemas valid, a positive error code
 *     number otherwise and -1 in case of internal or API error.
 */
static int
xmlSchemaValidateDocument(xmlSchemaValidCtxtPtr ctxt, xmlDocPtr doc) {
    xmlNodePtr root;
    xmlSchemaElementPtr elemDecl;

    root = xmlDocGetRootElement(doc);
    if (root == NULL) {
	ctxt->err = XML_SCHEMAS_ERR_NOROOT;
        if (ctxt->error != NULL)
            ctxt->error(ctxt->userData, "document has no root\n");
        return(ctxt->err);
    }
    if (root->ns != NULL)
	elemDecl = xmlHashLookup3(ctxt->schema->elemDecl,
		root->name, root->ns->href, NULL);
    else
	elemDecl = xmlHashLookup3(ctxt->schema->elemDecl,
		root->name, NULL, NULL);
    if (elemDecl == NULL) {
	ctxt->err = XML_SCHEMAS_ERR_UNDECLAREDELEM;
        if (ctxt->error != NULL)
            ctxt->error(ctxt->userData, "Element %s not declared\n",
                        root->name);
    } else if ((elemDecl->flags & XML_SCHEMAS_ELEM_TOPLEVEL) == 0) {
	ctxt->err = XML_SCHEMAS_ERR_NOTTOPLEVEL;
        if (ctxt->error != NULL)
            ctxt->error(ctxt->userData, "Root element %s not toplevel\n",
                        root->name);
    }
    /*
     * Okay, start the recursive validation
     */
    xmlSchemaValidateElement(ctxt, root);

    return(ctxt->err);
}

/************************************************************************
 * 									*
 * 			SAX Validation code				*
 * 									*
 ************************************************************************/

/************************************************************************
 * 									*
 * 			Validation interfaces				*
 * 									*
 ************************************************************************/

/**
 * xmlSchemaNewValidCtxt:
 * @schema:  a precompiled XML Schemas
 *
 * Create an XML Schemas validation context based on the given schema
 *
 * Returns the validation context or NULL in case of error
 */
xmlSchemaValidCtxtPtr
xmlSchemaNewValidCtxt(xmlSchemaPtr schema) {
    xmlSchemaValidCtxtPtr ret;

    ret = (xmlSchemaValidCtxtPtr) xmlMalloc(sizeof(xmlSchemaValidCtxt));
    if (ret == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		"Failed to allocate new schama validation context\n");
        return (NULL);
    }
    memset(ret, 0, sizeof(xmlSchemaValidCtxt));
    ret->schema = schema;
    ret->attrNr = 0;
    ret->attrMax = 10;
    ret->attr = (xmlSchemaAttrStatePtr) xmlMalloc(ret->attrMax *
						  sizeof(xmlSchemaAttrState));
    if (ret->attr == NULL) {
	free(ret);
	return(NULL);
    }
    memset(ret->attr, 0, ret->attrMax * sizeof(xmlSchemaAttrState));
    return (ret);
}

/**
 * xmlSchemaFreeValidCtxt:
 * @ctxt:  the schema validation context
 *
 * Free the resources associated to the schema validation context
 */
void
xmlSchemaFreeValidCtxt(xmlSchemaValidCtxtPtr ctxt) {
    if (ctxt == NULL)
	return;
    if (ctxt->attr != NULL)
	xmlFree(ctxt->attr);
    if (ctxt->value != NULL)
	xmlSchemaFreeValue(ctxt->value);
    xmlFree(ctxt);
}

/**
 * xmlSchemaSetValidErrors:
 * @ctxt:  a schema validation context
 * @err:  the error function
 * @warn: the warning function
 * @ctx: the functions context
 *
 * Set the error and warning callback informations
 */
void
xmlSchemaSetValidErrors(xmlSchemaValidCtxtPtr ctxt,
	xmlSchemaValidityErrorFunc err,
	xmlSchemaValidityWarningFunc warn, void *ctx) {
    if (ctxt == NULL)
	return;
    ctxt->error = err;
    ctxt->warning = warn;
    ctxt->userData = ctx;
}

/**
 * xmlSchemaValidateDoc:
 * @ctxt:  a schema validation context
 * @doc:  a parsed document tree
 *
 * Validate a document tree in memory.
 *
 * Returns 0 if the document is schemas valid, a positive error code
 *     number otherwise and -1 in case of internal or API error.
 */
int
xmlSchemaValidateDoc(xmlSchemaValidCtxtPtr ctxt, xmlDocPtr doc) {
    int ret;

    if ((ctxt == NULL) || (doc == NULL))
	return(-1);

    ctxt->doc = doc;
    ret = xmlSchemaValidateDocument(ctxt, doc);
    return(ret);
}

/**
 * xmlSchemaValidateStream:
 * @ctxt:  a schema validation context
 * @input:  the input to use for reading the data
 * @enc:  an optional encoding information
 * @sax:  a SAX handler for the resulting events
 * @user_data:  the context to provide to the SAX handler.
 *
 * Validate a document tree in memory.
 *
 * Returns 0 if the document is schemas valid, a positive error code
 *     number otherwise and -1 in case of internal or API error.
 */
int	
xmlSchemaValidateStream(xmlSchemaValidCtxtPtr ctxt,
	xmlParserInputBufferPtr input, xmlCharEncoding enc,
	xmlSAXHandlerPtr sax, void *user_data) {
    if ((ctxt == NULL) || (input == NULL))
	return(-1);
    ctxt->input = input;
    ctxt->enc = enc;
    ctxt->sax = sax;
    ctxt->user_data = user_data;
    TODO
    return(0);
}

#endif /* LIBXML_SCHEMAS_ENABLED */
