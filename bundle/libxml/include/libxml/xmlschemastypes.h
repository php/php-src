/*
 * schemastypes.c : interface of the XML Schema Datatypes
 *             definition and validity checking
 *
 * See Copyright for the status of this software.
 *
 * Daniel Veillard <veillard@redhat.com>
 */


#ifndef __XML_SCHEMA_TYPES_H__
#define __XML_SCHEMA_TYPES_H__

#include <libxml/xmlversion.h>

#ifdef LIBXML_SCHEMAS_ENABLED

#include <libxml/schemasInternals.h>

#ifdef __cplusplus
extern "C" {
#endif

void		xmlSchemaInitTypes		(void);
void		xmlSchemaCleanupTypes		(void);
xmlSchemaTypePtr xmlSchemaGetPredefinedType	(const xmlChar *name,
						 const xmlChar *ns);
int		xmlSchemaValidatePredefinedType	(xmlSchemaTypePtr type,
						 const xmlChar *value,
						 xmlSchemaValPtr *val);
int		xmlSchemaValidateFacet		(xmlSchemaTypePtr base,
						 xmlSchemaFacetPtr facet,
						 const xmlChar *value,
						 xmlSchemaValPtr val);
void		xmlSchemaFreeValue		(xmlSchemaValPtr val);

#ifdef __cplusplus
}
#endif

#endif /* LIBXML_SCHEMAS_ENABLED */
#endif /* __XML_SCHEMA_TYPES_H__ */
