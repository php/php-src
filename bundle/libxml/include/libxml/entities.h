/*
 * entities.h : interface for the XML entities handling
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */

#ifndef __XML_ENTITIES_H__
#define __XML_ENTITIES_H__

#include <libxml/tree.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The different valid entity types.
 */
typedef enum {
    XML_INTERNAL_GENERAL_ENTITY = 1,
    XML_EXTERNAL_GENERAL_PARSED_ENTITY = 2,
    XML_EXTERNAL_GENERAL_UNPARSED_ENTITY = 3,
    XML_INTERNAL_PARAMETER_ENTITY = 4,
    XML_EXTERNAL_PARAMETER_ENTITY = 5,
    XML_INTERNAL_PREDEFINED_ENTITY = 6
} xmlEntityType;

/*
 * An unit of storage for an entity, contains the string, the value
 * and the linkind data needed for the linking in the hash table.
 */

struct _xmlEntity {
    void           *_private;	        /* application data */
    xmlElementType          type;       /* XML_ENTITY_DECL, must be second ! */
    const xmlChar          *name;	/* Entity name */
    struct _xmlNode    *children;	/* First child link */
    struct _xmlNode        *last;	/* Last child link */
    struct _xmlDtd       *parent;	/* -> DTD */
    struct _xmlNode        *next;	/* next sibling link  */
    struct _xmlNode        *prev;	/* previous sibling link  */
    struct _xmlDoc          *doc;       /* the containing document */

    xmlChar                *orig;	/* content without ref substitution */
    xmlChar             *content;	/* content or ndata if unparsed */
    int                   length;	/* the content length */
    xmlEntityType          etype;	/* The entity type */
    const xmlChar    *ExternalID;	/* External identifier for PUBLIC */
    const xmlChar      *SystemID;	/* URI for a SYSTEM or PUBLIC Entity */

    struct _xmlEntity     *nexte;	/* unused */
    const xmlChar           *URI;	/* the full URI as computed */
};

/*
 * All entities are stored in an hash table.
 * There is 2 separate hash tables for global and parameter entities.
 */

typedef struct _xmlHashTable xmlEntitiesTable;
typedef xmlEntitiesTable *xmlEntitiesTablePtr;

/*
 * External functions:
 */

void		xmlInitializePredefinedEntities	(void);
xmlEntityPtr		xmlAddDocEntity		(xmlDocPtr doc,
						 const xmlChar *name,
						 int type,
						 const xmlChar *ExternalID,
						 const xmlChar *SystemID,
						 const xmlChar *content);
xmlEntityPtr		xmlAddDtdEntity		(xmlDocPtr doc,
						 const xmlChar *name,
						 int type,
						 const xmlChar *ExternalID,
						 const xmlChar *SystemID,
						 const xmlChar *content);
xmlEntityPtr		xmlGetPredefinedEntity	(const xmlChar *name);
xmlEntityPtr		xmlGetDocEntity		(xmlDocPtr doc,
						 const xmlChar *name);
xmlEntityPtr		xmlGetDtdEntity		(xmlDocPtr doc,
						 const xmlChar *name);
xmlEntityPtr		xmlGetParameterEntity	(xmlDocPtr doc,
						 const xmlChar *name);
const xmlChar *		xmlEncodeEntities	(xmlDocPtr doc,
						 const xmlChar *input);
xmlChar *		xmlEncodeEntitiesReentrant(xmlDocPtr doc,
						 const xmlChar *input);
xmlChar *		xmlEncodeSpecialChars	(xmlDocPtr doc,
						 const xmlChar *input);
xmlEntitiesTablePtr	xmlCreateEntitiesTable	(void);
xmlEntitiesTablePtr	xmlCopyEntitiesTable	(xmlEntitiesTablePtr table);
void			xmlFreeEntitiesTable	(xmlEntitiesTablePtr table);
void			xmlDumpEntitiesTable	(xmlBufferPtr buf,
						 xmlEntitiesTablePtr table);
void			xmlDumpEntityDecl	(xmlBufferPtr buf,
						 xmlEntityPtr ent);
void			xmlCleanupPredefinedEntities(void);


#ifdef __cplusplus
}
#endif

# endif /* __XML_ENTITIES_H__ */
