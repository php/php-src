/*
 * tree.h : describes the structures found in an tree resulting
 *          from an XML parsing.
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 *
 */

#ifndef __XML_TREE_H__
#define __XML_TREE_H__

#include <stdio.h>
#include <libxml/xmlversion.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Some of the basic types pointer to structures:
 */
/* xmlIO.h */
typedef struct _xmlParserInputBuffer xmlParserInputBuffer;
typedef xmlParserInputBuffer *xmlParserInputBufferPtr;

typedef struct _xmlOutputBuffer xmlOutputBuffer;
typedef xmlOutputBuffer *xmlOutputBufferPtr;

/* parser.h */
typedef struct _xmlParserInput xmlParserInput;
typedef xmlParserInput *xmlParserInputPtr;

typedef struct _xmlParserCtxt xmlParserCtxt;
typedef xmlParserCtxt *xmlParserCtxtPtr;

typedef struct _xmlSAXLocator xmlSAXLocator;
typedef xmlSAXLocator *xmlSAXLocatorPtr;

typedef struct _xmlSAXHandler xmlSAXHandler;
typedef xmlSAXHandler *xmlSAXHandlerPtr;

/* entities.h */
typedef struct _xmlEntity xmlEntity;
typedef xmlEntity *xmlEntityPtr;

/**
 * BASE_BUFFER_SIZE:
 *
 * default buffer size 4000.
 */
#define BASE_BUFFER_SIZE 4000

/**
 * XML_XML_NAMESPACE:
 *
 * This is the namespace for the special xml: prefix predefined in the
 * XML Namespace specification.
 */
#define XML_XML_NAMESPACE \
    (const xmlChar *) "http://www.w3.org/XML/1998/namespace"

/*
 * The different element types carried by an XML tree.
 *
 * NOTE: This is synchronized with DOM Level1 values
 *       See http://www.w3.org/TR/REC-DOM-Level-1/
 *
 * Actually this had diverged a bit, and now XML_DOCUMENT_TYPE_NODE should
 * be deprecated to use an XML_DTD_NODE.
 */
typedef enum {
    XML_ELEMENT_NODE=		1,
    XML_ATTRIBUTE_NODE=		2,
    XML_TEXT_NODE=		3,
    XML_CDATA_SECTION_NODE=	4,
    XML_ENTITY_REF_NODE=	5,
    XML_ENTITY_NODE=		6,
    XML_PI_NODE=		7,
    XML_COMMENT_NODE=		8,
    XML_DOCUMENT_NODE=		9,
    XML_DOCUMENT_TYPE_NODE=	10,
    XML_DOCUMENT_FRAG_NODE=	11,
    XML_NOTATION_NODE=		12,
    XML_HTML_DOCUMENT_NODE=	13,
    XML_DTD_NODE=		14,
    XML_ELEMENT_DECL=		15,
    XML_ATTRIBUTE_DECL=		16,
    XML_ENTITY_DECL=		17,
    XML_NAMESPACE_DECL=		18,
    XML_XINCLUDE_START=		19,
    XML_XINCLUDE_END=		20
#ifdef LIBXML_DOCB_ENABLED
   ,XML_DOCB_DOCUMENT_NODE=	21
#endif
} xmlElementType;

/**
 * xmlChar:
 *
 * This is a basic byte in an UTF-8 encoded string.
 * It's unsigned allowing to pinpoint case where char * are assigned
 * to xmlChar * (possibly making serialization back impossible).
 */

typedef unsigned char xmlChar;

/**
 * BAD_CAST:
 *
 * Macro to cast a string to an xmlChar * when one know its safe.
 */
#define BAD_CAST (xmlChar *)

/**
 * xmlNotation:
 *
 * A DTD Notation definition.
 */

typedef struct _xmlNotation xmlNotation;
typedef xmlNotation *xmlNotationPtr;
struct _xmlNotation {
    const xmlChar               *name;	        /* Notation name */
    const xmlChar               *PublicID;	/* Public identifier, if any */
    const xmlChar               *SystemID;	/* System identifier, if any */
};

/**
 * xmlAttributeType:
 *
 * A DTD Attribute type definition.
 */

typedef enum {
    XML_ATTRIBUTE_CDATA = 1,
    XML_ATTRIBUTE_ID,
    XML_ATTRIBUTE_IDREF	,
    XML_ATTRIBUTE_IDREFS,
    XML_ATTRIBUTE_ENTITY,
    XML_ATTRIBUTE_ENTITIES,
    XML_ATTRIBUTE_NMTOKEN,
    XML_ATTRIBUTE_NMTOKENS,
    XML_ATTRIBUTE_ENUMERATION,
    XML_ATTRIBUTE_NOTATION
} xmlAttributeType;

/**
 * xmlAttributeDefault:
 *
 * A DTD Attribute default definition.
 */

typedef enum {
    XML_ATTRIBUTE_NONE = 1,
    XML_ATTRIBUTE_REQUIRED,
    XML_ATTRIBUTE_IMPLIED,
    XML_ATTRIBUTE_FIXED
} xmlAttributeDefault;

/**
 * xmlEnumeration:
 *
 * List structure used when there is an enumeration in DTDs.
 */

typedef struct _xmlEnumeration xmlEnumeration;
typedef xmlEnumeration *xmlEnumerationPtr;
struct _xmlEnumeration {
    struct _xmlEnumeration    *next;	/* next one */
    const xmlChar            *name;	/* Enumeration name */
};

/**
 * xmlAttribute:
 *
 * An Attribute declaration in a DTD.
 */

typedef struct _xmlAttribute xmlAttribute;
typedef xmlAttribute *xmlAttributePtr;
struct _xmlAttribute {
    void           *_private;	        /* application data */
    xmlElementType          type;       /* XML_ATTRIBUTE_DECL, must be second ! */
    const xmlChar          *name;	/* Attribute name */
    struct _xmlNode    *children;	/* NULL */
    struct _xmlNode        *last;	/* NULL */
    struct _xmlDtd       *parent;	/* -> DTD */
    struct _xmlNode        *next;	/* next sibling link  */
    struct _xmlNode        *prev;	/* previous sibling link  */
    struct _xmlDoc          *doc;       /* the containing document */

    struct _xmlAttribute  *nexth;	/* next in hash table */
    xmlAttributeType       atype;	/* The attribute type */
    xmlAttributeDefault      def;	/* the default */
    const xmlChar  *defaultValue;	/* or the default value */
    xmlEnumerationPtr       tree;       /* or the enumeration tree if any */
    const xmlChar        *prefix;	/* the namespace prefix if any */
    const xmlChar          *elem;	/* Element holding the attribute */
};

/**
 * xmlElementContentType:
 *
 * Possible definitions of element content types.
 */
typedef enum {
    XML_ELEMENT_CONTENT_PCDATA = 1,
    XML_ELEMENT_CONTENT_ELEMENT,
    XML_ELEMENT_CONTENT_SEQ,
    XML_ELEMENT_CONTENT_OR
} xmlElementContentType;

/**
 * xmlElementContentOccur:
 *
 * Possible definitions of element content occurrences.
 */
typedef enum {
    XML_ELEMENT_CONTENT_ONCE = 1,
    XML_ELEMENT_CONTENT_OPT,
    XML_ELEMENT_CONTENT_MULT,
    XML_ELEMENT_CONTENT_PLUS
} xmlElementContentOccur;

/**
 * xmlElementContent:
 *
 * An XML Element content as stored after parsing an element definition
 * in a DTD.
 */

typedef struct _xmlElementContent xmlElementContent;
typedef xmlElementContent *xmlElementContentPtr;
struct _xmlElementContent {
    xmlElementContentType     type;	/* PCDATA, ELEMENT, SEQ or OR */
    xmlElementContentOccur    ocur;	/* ONCE, OPT, MULT or PLUS */
    const xmlChar             *name;	/* Element name */
    struct _xmlElementContent *c1;	/* first child */
    struct _xmlElementContent *c2;	/* second child */
    struct _xmlElementContent *parent;	/* parent */
    const xmlChar             *prefix;	/* Namespace prefix */
};

/**
 * xmlElementTypeVal:
 *
 * The different possibilities for an element content type.
 */

typedef enum {
    XML_ELEMENT_TYPE_UNDEFINED = 0,
    XML_ELEMENT_TYPE_EMPTY = 1,
    XML_ELEMENT_TYPE_ANY,
    XML_ELEMENT_TYPE_MIXED,
    XML_ELEMENT_TYPE_ELEMENT
} xmlElementTypeVal;


#ifdef __cplusplus
}
#endif
#include <libxml/xmlregexp.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * xmlElement:
 *
 * An XML Element declaration from a DTD.
 */

typedef struct _xmlElement xmlElement;
typedef xmlElement *xmlElementPtr;
struct _xmlElement {
    void           *_private;	        /* application data */
    xmlElementType          type;       /* XML_ELEMENT_DECL, must be second ! */
    const xmlChar          *name;	/* Element name */
    struct _xmlNode    *children;	/* NULL */
    struct _xmlNode        *last;	/* NULL */
    struct _xmlDtd       *parent;	/* -> DTD */
    struct _xmlNode        *next;	/* next sibling link  */
    struct _xmlNode        *prev;	/* previous sibling link  */
    struct _xmlDoc          *doc;       /* the containing document */

    xmlElementTypeVal      etype;	/* The type */
    xmlElementContentPtr content;	/* the allowed element content */
    xmlAttributePtr   attributes;	/* List of the declared attributes */
    const xmlChar        *prefix;	/* the namespace prefix if any */
#ifdef LIBXML_REGEXP_ENABLED
    xmlRegexpPtr       contModel;	/* the validating regexp */
#else
    void	      *contModel;
#endif
};


/**
 * XML_LOCAL_NAMESPACE:
 *
 * A namespace declaration node.
 */
#define XML_LOCAL_NAMESPACE XML_NAMESPACE_DECL
typedef xmlElementType xmlNsType;

/**
 * xmlNs:
 *
 * An XML namespace.
 * Note that prefix == NULL is valid, it defines the default namespace
 * within the subtree (until overridden).
 *
 * xmlNsType is unified with xmlElementType.
 */

typedef struct _xmlNs xmlNs;
typedef xmlNs *xmlNsPtr;
struct _xmlNs {
    struct _xmlNs  *next;	/* next Ns link for this node  */
    xmlNsType      type;	/* global or local */
    const xmlChar *href;	/* URL for the namespace */
    const xmlChar *prefix;	/* prefix for the namespace */
    void           *_private;   /* application data */
};

/**
 * xmlDtd:
 *
 * An XML DTD, as defined by <!DOCTYPE ... There is actually one for
 * the internal subset and for the external subset.
 */
typedef struct _xmlDtd xmlDtd;
typedef xmlDtd *xmlDtdPtr;
struct _xmlDtd {
    void           *_private;	/* application data */
    xmlElementType  type;       /* XML_DTD_NODE, must be second ! */
    const xmlChar *name;	/* Name of the DTD */
    struct _xmlNode *children;	/* the value of the property link */
    struct _xmlNode *last;	/* last child link */
    struct _xmlDoc  *parent;	/* child->parent link */
    struct _xmlNode *next;	/* next sibling link  */
    struct _xmlNode *prev;	/* previous sibling link  */
    struct _xmlDoc  *doc;	/* the containing document */

    /* End of common part */
    void          *notations;   /* Hash table for notations if any */
    void          *elements;    /* Hash table for elements if any */
    void          *attributes;  /* Hash table for attributes if any */
    void          *entities;    /* Hash table for entities if any */
    const xmlChar *ExternalID;	/* External identifier for PUBLIC DTD */
    const xmlChar *SystemID;	/* URI for a SYSTEM or PUBLIC DTD */
    void          *pentities;   /* Hash table for param entities if any */
};

/**
 * xmlAttr:
 *
 * An attribute on an XML node.
 */
typedef struct _xmlAttr xmlAttr;
typedef xmlAttr *xmlAttrPtr;
struct _xmlAttr {
    void           *_private;	/* application data */
    xmlElementType   type;      /* XML_ATTRIBUTE_NODE, must be second ! */
    const xmlChar   *name;      /* the name of the property */
    struct _xmlNode *children;	/* the value of the property */
    struct _xmlNode *last;	/* NULL */
    struct _xmlNode *parent;	/* child->parent link */
    struct _xmlAttr *next;	/* next sibling link  */
    struct _xmlAttr *prev;	/* previous sibling link  */
    struct _xmlDoc  *doc;	/* the containing document */
    xmlNs           *ns;        /* pointer to the associated namespace */
    xmlAttributeType atype;     /* the attribute type if validating */
};

/**
 * xmlID:
 *
 * An XML ID instance.
 */

typedef struct _xmlID xmlID;
typedef xmlID *xmlIDPtr;
struct _xmlID {
    struct _xmlID    *next;	/* next ID */
    const xmlChar    *value;	/* The ID name */
    xmlAttrPtr        attr;	/* The attribute holding it */
};

/**
 * xmlRef:
 *
 * An XML IDREF instance.
 */

typedef struct _xmlRef xmlRef;
typedef xmlRef *xmlRefPtr;
struct _xmlRef {
    struct _xmlRef    *next;	/* next Ref */
    const xmlChar     *value;	/* The Ref name */
    xmlAttrPtr        attr;	/* The attribute holding it */
};

/**
 * xmlBufferAllocationScheme:
 *
 * A buffer allocation scheme can be defined to either match exactly the
 * need or double it's allocated size each time it is found too small.
 */

typedef enum {
    XML_BUFFER_ALLOC_DOUBLEIT,
    XML_BUFFER_ALLOC_EXACT
} xmlBufferAllocationScheme;

/**
 * xmlBuffer:
 *
 * A buffer structure.
 */
typedef struct _xmlBuffer xmlBuffer;
typedef xmlBuffer *xmlBufferPtr;
struct _xmlBuffer {
    xmlChar *content;		/* The buffer content UTF8 */
    unsigned int use;		/* The buffer size used */
    unsigned int size;		/* The buffer size */
    xmlBufferAllocationScheme alloc; /* The realloc method */
};

/**
 * xmlNode:
 *
 * A node in an XML tree.
 */
typedef struct _xmlNode xmlNode;
typedef xmlNode *xmlNodePtr;
struct _xmlNode {
    void           *_private;	/* application data */
    xmlElementType   type;	/* type number, must be second ! */
    const xmlChar   *name;      /* the name of the node, or the entity */
    struct _xmlNode *children;	/* parent->childs link */
    struct _xmlNode *last;	/* last child link */
    struct _xmlNode *parent;	/* child->parent link */
    struct _xmlNode *next;	/* next sibling link  */
    struct _xmlNode *prev;	/* previous sibling link  */
    struct _xmlDoc  *doc;	/* the containing document */

    /* End of common part */
    xmlNs           *ns;        /* pointer to the associated namespace */
    xmlChar         *content;   /* the content */
    struct _xmlAttr *properties;/* properties list */
    xmlNs           *nsDef;     /* namespace definitions on this node */
};

/**
 * XML_GET_CONTENT:
 *
 * Macro to extract the content pointer of a node.
 */
#define XML_GET_CONTENT(n)					\
    ((n)->type == XML_ELEMENT_NODE ? NULL : (n)->content)

/**
 * XML_GET_LINE:
 *
 * Macro to extract the line number of an element node. 
 * This will work only if line numbering is activated by
 * calling xmlLineNumbersDefault(1) before parsing.
 */
#define XML_GET_LINE(n)						\
    ((n)->type == XML_ELEMENT_NODE ? (int) (n)->content : 0)

/**
 * xmlDoc:
 *
 * An XML document.
 */
typedef struct _xmlDoc xmlDoc;
typedef xmlDoc *xmlDocPtr;
struct _xmlDoc {
    void           *_private;	/* application data */
    xmlElementType  type;       /* XML_DOCUMENT_NODE, must be second ! */
    char           *name;	/* name/filename/URI of the document */
    struct _xmlNode *children;	/* the document tree */
    struct _xmlNode *last;	/* last child link */
    struct _xmlNode *parent;	/* child->parent link */
    struct _xmlNode *next;	/* next sibling link  */
    struct _xmlNode *prev;	/* previous sibling link  */
    struct _xmlDoc  *doc;	/* autoreference to itself */

    /* End of common part */
    int             compression;/* level of zlib compression */
    int             standalone; /* standalone document (no external refs) */
    struct _xmlDtd  *intSubset;	/* the document internal subset */
    struct _xmlDtd  *extSubset;	/* the document external subset */
    struct _xmlNs   *oldNs;	/* Global namespace, the old way */
    const xmlChar  *version;	/* the XML version string */
    const xmlChar  *encoding;   /* external initial encoding, if any */
    void           *ids;        /* Hash table for ID attributes if any */
    void           *refs;       /* Hash table for IDREFs attributes if any */
    const xmlChar  *URL;	/* The URI for that document */
    int             charset;    /* encoding of the in-memory content
				   actually an xmlCharEncoding */
};

/**
 * xmlChildrenNode:
 *
 * Macro for compatibility naming layer with libxml1.
 */
#ifndef xmlChildrenNode
#define xmlChildrenNode children
#endif

/**
 * xmlRootNode:
 *
 * Macro for compatibility naming layer with libxml1.
 */
#ifndef xmlRootNode
#define xmlRootNode children
#endif

/*
 * Variables.
 */
#if 0
LIBXML_DLL_IMPORT extern int oldXMLWDcompatibility;/* maintain compatibility with old WD */
LIBXML_DLL_IMPORT extern int xmlIndentTreeOutput;  /* try to indent the tree dumps */
LIBXML_DLL_IMPORT extern xmlBufferAllocationScheme xmlBufferAllocScheme; /* alloc scheme to use */
LIBXML_DLL_IMPORT extern int xmlSaveNoEmptyTags; /* save empty tags as <empty></empty> */
LIBXML_DLL_IMPORT extern int xmlDefaultBufferSize; /* default buffer size */
#endif

/*
 * Handling Buffers.
 */

void		xmlSetBufferAllocationScheme(xmlBufferAllocationScheme scheme);
xmlBufferAllocationScheme	 xmlGetBufferAllocationScheme(void);

xmlBufferPtr	xmlBufferCreate		(void);
xmlBufferPtr	xmlBufferCreateSize	(size_t size);
int		xmlBufferResize		(xmlBufferPtr buf,
					 unsigned int size);
void		xmlBufferFree		(xmlBufferPtr buf);
int		xmlBufferDump		(FILE *file,
					 xmlBufferPtr buf);
void		xmlBufferAdd		(xmlBufferPtr buf,
					 const xmlChar *str,
					 int len);
void		xmlBufferAddHead	(xmlBufferPtr buf,
					 const xmlChar *str,
					 int len);
void		xmlBufferCat		(xmlBufferPtr buf,
					 const xmlChar *str);
void		xmlBufferCCat		(xmlBufferPtr buf,
					 const char *str);
int		xmlBufferShrink		(xmlBufferPtr buf,
					 unsigned int len);
int		xmlBufferGrow		(xmlBufferPtr buf,
					 unsigned int len);
void		xmlBufferEmpty		(xmlBufferPtr buf);
const xmlChar*	xmlBufferContent	(const xmlBufferPtr buf);
void		xmlBufferSetAllocationScheme(xmlBufferPtr buf,
					 xmlBufferAllocationScheme scheme);
int		xmlBufferLength		(const xmlBufferPtr buf);

/*
 * Creating/freeing new structures.
 */
xmlDtdPtr	xmlCreateIntSubset	(xmlDocPtr doc,
					 const xmlChar *name,
					 const xmlChar *ExternalID,
					 const xmlChar *SystemID);
xmlDtdPtr	xmlNewDtd		(xmlDocPtr doc,
					 const xmlChar *name,
					 const xmlChar *ExternalID,
					 const xmlChar *SystemID);
xmlDtdPtr	xmlGetIntSubset		(xmlDocPtr doc);
void		xmlFreeDtd		(xmlDtdPtr cur);
xmlNsPtr	xmlNewGlobalNs		(xmlDocPtr doc,
					 const xmlChar *href,
					 const xmlChar *prefix);
xmlNsPtr	xmlNewNs		(xmlNodePtr node,
					 const xmlChar *href,
					 const xmlChar *prefix);
void		xmlFreeNs		(xmlNsPtr cur);
void		xmlFreeNsList		(xmlNsPtr cur);
xmlDocPtr 	xmlNewDoc		(const xmlChar *version);
void		xmlFreeDoc		(xmlDocPtr cur);
xmlAttrPtr	xmlNewDocProp		(xmlDocPtr doc,
					 const xmlChar *name,
					 const xmlChar *value);
xmlAttrPtr	xmlNewProp		(xmlNodePtr node,
					 const xmlChar *name,
					 const xmlChar *value);
xmlAttrPtr	xmlNewNsProp		(xmlNodePtr node,
					 xmlNsPtr ns,
					 const xmlChar *name,
					 const xmlChar *value);
xmlAttrPtr	xmlNewNsPropEatName	(xmlNodePtr node,
					 xmlNsPtr ns,
					 xmlChar *name,
					 const xmlChar *value);
void		xmlFreePropList		(xmlAttrPtr cur);
void		xmlFreeProp		(xmlAttrPtr cur);
xmlAttrPtr	xmlCopyProp		(xmlNodePtr target,
					 xmlAttrPtr cur);
xmlAttrPtr	xmlCopyPropList		(xmlNodePtr target,
					 xmlAttrPtr cur);
xmlDtdPtr	xmlCopyDtd		(xmlDtdPtr dtd);
xmlDocPtr	xmlCopyDoc		(xmlDocPtr doc,
					 int recursive);

/*
 * Creating new nodes.
 */
xmlNodePtr	xmlNewDocNode		(xmlDocPtr doc,
					 xmlNsPtr ns,
					 const xmlChar *name,
					 const xmlChar *content);
xmlNodePtr	xmlNewDocNodeEatName	(xmlDocPtr doc,
					 xmlNsPtr ns,
					 xmlChar *name,
					 const xmlChar *content);
xmlNodePtr	xmlNewDocRawNode	(xmlDocPtr doc,
					 xmlNsPtr ns,
					 const xmlChar *name,
					 const xmlChar *content);
xmlNodePtr	xmlNewNode		(xmlNsPtr ns,
					 const xmlChar *name);
xmlNodePtr	xmlNewNodeEatName	(xmlNsPtr ns,
					 xmlChar *name);
xmlNodePtr	xmlNewChild		(xmlNodePtr parent,
					 xmlNsPtr ns,
					 const xmlChar *name,
					 const xmlChar *content);
xmlNodePtr	xmlNewTextChild		(xmlNodePtr parent,
					 xmlNsPtr ns,
					 const xmlChar *name,
					 const xmlChar *content);
xmlNodePtr	xmlNewDocText		(xmlDocPtr doc,
					 const xmlChar *content);
xmlNodePtr	xmlNewText		(const xmlChar *content);
xmlNodePtr	xmlNewPI		(const xmlChar *name,
					 const xmlChar *content);
xmlNodePtr	xmlNewDocTextLen	(xmlDocPtr doc,
					 const xmlChar *content,
					 int len);
xmlNodePtr	xmlNewTextLen		(const xmlChar *content,
					 int len);
xmlNodePtr	xmlNewDocComment	(xmlDocPtr doc,
					 const xmlChar *content);
xmlNodePtr	xmlNewComment		(const xmlChar *content);
xmlNodePtr	xmlNewCDataBlock	(xmlDocPtr doc,
					 const xmlChar *content,
					 int len);
xmlNodePtr	xmlNewCharRef		(xmlDocPtr doc,
					 const xmlChar *name);
xmlNodePtr	xmlNewReference		(xmlDocPtr doc,
					 const xmlChar *name);
xmlNodePtr	xmlCopyNode		(const xmlNodePtr node,
					 int recursive);
xmlNodePtr	xmlDocCopyNode		(const xmlNodePtr node,
					 xmlDocPtr doc,
					 int recursive);
xmlNodePtr	xmlCopyNodeList		(const xmlNodePtr node);
xmlNodePtr	xmlNewDocFragment	(xmlDocPtr doc);

/*
 * Navigating.
 */
long		xmlGetLineNo		(xmlNodePtr node);
xmlChar *	xmlGetNodePath		(xmlNodePtr node);
xmlNodePtr	xmlDocGetRootElement	(xmlDocPtr doc);
xmlNodePtr	xmlGetLastChild		(xmlNodePtr parent);
int		xmlNodeIsText		(xmlNodePtr node);
int		xmlIsBlankNode		(xmlNodePtr node);

/*
 * Changing the structure.
 */
xmlNodePtr	xmlDocSetRootElement	(xmlDocPtr doc,
					 xmlNodePtr root);
void		xmlNodeSetName		(xmlNodePtr cur,
					 const xmlChar *name);
xmlNodePtr	xmlAddChild		(xmlNodePtr parent,
					 xmlNodePtr cur);
xmlNodePtr	xmlAddChildList		(xmlNodePtr parent,
					 xmlNodePtr cur);
xmlNodePtr	xmlReplaceNode		(xmlNodePtr old,
					 xmlNodePtr cur);
xmlNodePtr	xmlAddSibling		(xmlNodePtr cur,
					 xmlNodePtr elem);
xmlNodePtr	xmlAddPrevSibling	(xmlNodePtr cur,
					 xmlNodePtr elem);
xmlNodePtr	xmlAddNextSibling	(xmlNodePtr cur,
					 xmlNodePtr elem);
void		xmlUnlinkNode		(xmlNodePtr cur);
xmlNodePtr	xmlTextMerge		(xmlNodePtr first,
					 xmlNodePtr second);
void		xmlTextConcat		(xmlNodePtr node,
					 const xmlChar *content,
					 int len);
void		xmlFreeNodeList		(xmlNodePtr cur);
void		xmlFreeNode		(xmlNodePtr cur);
void		xmlSetTreeDoc		(xmlNodePtr tree,
					 xmlDocPtr doc);
void		xmlSetListDoc		(xmlNodePtr list,
					 xmlDocPtr doc);

/*
 * Namespaces.
 */
xmlNsPtr	xmlSearchNs		(xmlDocPtr doc,
					 xmlNodePtr node,
					 const xmlChar *nameSpace);
xmlNsPtr	xmlSearchNsByHref	(xmlDocPtr doc,
					 xmlNodePtr node,
					 const xmlChar *href);
xmlNsPtr *	xmlGetNsList		(xmlDocPtr doc,
					 xmlNodePtr node);
void		xmlSetNs		(xmlNodePtr node,
					 xmlNsPtr ns);
xmlNsPtr	xmlCopyNamespace	(xmlNsPtr cur);
xmlNsPtr	xmlCopyNamespaceList	(xmlNsPtr cur);

/*
 * Changing the content.
 */
xmlAttrPtr	xmlSetProp		(xmlNodePtr node,
					 const xmlChar *name,
					 const xmlChar *value);
xmlChar *	xmlGetProp		(xmlNodePtr node,
					 const xmlChar *name);
xmlAttrPtr	xmlHasProp		(xmlNodePtr node,
					 const xmlChar *name);
xmlAttrPtr	xmlHasNsProp		(xmlNodePtr node,
					 const xmlChar *name,
					 const xmlChar *nameSpace);
xmlAttrPtr	xmlSetNsProp		(xmlNodePtr node,
					 xmlNsPtr ns,
					 const xmlChar *name,
					 const xmlChar *value);
xmlChar *	xmlGetNsProp		(xmlNodePtr node,
					 const xmlChar *name,
					 const xmlChar *nameSpace);
xmlNodePtr	xmlStringGetNodeList	(xmlDocPtr doc,
					 const xmlChar *value);
xmlNodePtr	xmlStringLenGetNodeList	(xmlDocPtr doc,
					 const xmlChar *value,
					 int len);
xmlChar *	xmlNodeListGetString	(xmlDocPtr doc,
					 xmlNodePtr list,
					 int inLine);
xmlChar *	xmlNodeListGetRawString	(xmlDocPtr doc,
					 xmlNodePtr list,
					 int inLine);
void		xmlNodeSetContent	(xmlNodePtr cur,
					 const xmlChar *content);
void		xmlNodeSetContentLen	(xmlNodePtr cur,
					 const xmlChar *content,
					 int len);
void		xmlNodeAddContent	(xmlNodePtr cur,
					 const xmlChar *content);
void		xmlNodeAddContentLen	(xmlNodePtr cur,
					 const xmlChar *content,
					 int len);
xmlChar *	xmlNodeGetContent	(xmlNodePtr cur);
xmlChar *	xmlNodeGetLang		(xmlNodePtr cur);
void		xmlNodeSetLang		(xmlNodePtr cur,
					 const xmlChar *lang);
int		xmlNodeGetSpacePreserve	(xmlNodePtr cur);
void		xmlNodeSetSpacePreserve (xmlNodePtr cur,
					 int val);
xmlChar *	xmlNodeGetBase		(xmlDocPtr doc,
					 xmlNodePtr cur);
void		xmlNodeSetBase		(xmlNodePtr cur,
					 xmlChar *uri);

/*
 * Removing content.
 */
int		xmlRemoveProp		(xmlAttrPtr cur);
int		xmlUnsetProp		(xmlNodePtr node,
					 const xmlChar *name);
int		xmlUnsetNsProp		(xmlNodePtr node,
					 xmlNsPtr ns,
					 const xmlChar *name);

/*
 * Internal, don't use.
 */
void		xmlBufferWriteCHAR	(xmlBufferPtr buf,
					 const xmlChar *string);
void		xmlBufferWriteChar	(xmlBufferPtr buf,
					 const char *string);
void		xmlBufferWriteQuotedString(xmlBufferPtr buf,
					 const xmlChar *string);

/*
 * Namespace handling.
 */
int		xmlReconciliateNs	(xmlDocPtr doc,
					 xmlNodePtr tree);

/*
 * Saving.
 */
void		xmlDocDumpFormatMemory	(xmlDocPtr cur,
					 xmlChar **mem,
					 int *size,
					 int format);
void		xmlDocDumpMemory	(xmlDocPtr cur,
					 xmlChar **mem,
					 int *size);
void		xmlDocDumpMemoryEnc	(xmlDocPtr out_doc,
					 xmlChar **doc_txt_ptr,
					 int * doc_txt_len,
					 const char *txt_encoding);
void		xmlDocDumpFormatMemoryEnc(xmlDocPtr out_doc,
					 xmlChar **doc_txt_ptr,
					 int * doc_txt_len,
					 const char *txt_encoding,
					 int format);
int		xmlDocFormatDump(FILE *f,
					 xmlDocPtr cur,
					 int format);
int		xmlDocDump		(FILE *f,
					 xmlDocPtr cur);
void		xmlElemDump		(FILE *f,
					 xmlDocPtr doc,
					 xmlNodePtr cur);
int		xmlSaveFile		(const char *filename,
					 xmlDocPtr cur);
int		xmlSaveFormatFile	(const char *filename,
					 xmlDocPtr cur,
					 int format);
int		xmlNodeDump		(xmlBufferPtr buf,
					 xmlDocPtr doc,
					 xmlNodePtr cur,
					 int level,
					 int format);

int		xmlSaveFileTo		(xmlOutputBufferPtr buf,
					 xmlDocPtr cur,
					 const char *encoding);
int             xmlSaveFormatFileTo     (xmlOutputBufferPtr buf,
					 xmlDocPtr cur,
				         const char *encoding,
				         int format);
void		xmlNodeDumpOutput	(xmlOutputBufferPtr buf,
					 xmlDocPtr doc,
					 xmlNodePtr cur,
					 int level,
					 int format,
					 const char *encoding);

int		xmlSaveFormatFileEnc    (const char *filename,
					 xmlDocPtr cur,
					 const char *encoding,
					 int format);

int		xmlSaveFileEnc		(const char *filename,
					 xmlDocPtr cur,
					 const char *encoding);

/*
 * XHTML
 */
int		xmlIsXHTML		(const xmlChar *systemID,
					 const xmlChar *publicID);

/*
 * Compression.
 */
int		xmlGetDocCompressMode	(xmlDocPtr doc);
void		xmlSetDocCompressMode	(xmlDocPtr doc,
					 int mode);
int		xmlGetCompressMode	(void);
void		xmlSetCompressMode	(int mode);

#ifdef __cplusplus
}
#endif
#ifndef __XML_PARSER_H__
#include <libxml/xmlmemory.h>
#endif

#endif /* __XML_TREE_H__ */

