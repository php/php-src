/**
 * catalog.h: interfaces of the Catalog handling system
 *
 * Reference:  SGML Open Technical Resolution TR9401:1997.
 *             http://www.jclark.com/sp/catalog.htm
 *
 *             XML Catalogs Working Draft 12 Jun 2001
 *             http://www.oasis-open.org/committees/entity/spec-2001-06-12.html
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */

#ifndef __XML_CATALOG_H__
#define __XML_CATALOG_H__

#include <stdio.h>

#include <libxml/xmlversion.h>

#ifdef LIBXML_CATALOG_ENABLED

#ifdef __cplusplus
extern "C" {
#endif

/**
 * XML_CATALOGS_NAMESPACE:
 *
 * The namespace for the XML Catalogs elements.
 */
#define XML_CATALOGS_NAMESPACE					\
    (const xmlChar *) "urn:oasis:names:tc:entity:xmlns:xml:catalog"
/**
 * XML_CATALOG_PI:
 *
 * The specific XML Catalog Processing Instuction name.
 */
#define XML_CATALOG_PI						\
    (const xmlChar *) "oasis-xml-catalog"

/*
 * The API is voluntarily limited to general cataloging.
 */
typedef enum {
    XML_CATA_PREFER_NONE = 0,
    XML_CATA_PREFER_PUBLIC = 1,
    XML_CATA_PREFER_SYSTEM
} xmlCatalogPrefer;

typedef enum {
    XML_CATA_ALLOW_NONE = 0,
    XML_CATA_ALLOW_GLOBAL = 1,
    XML_CATA_ALLOW_DOCUMENT = 2,
    XML_CATA_ALLOW_ALL = 3
} xmlCatalogAllow;

typedef struct _xmlCatalog xmlCatalog;
typedef xmlCatalog *xmlCatalogPtr;

/*
 * Operations on a given catalog.
 */
xmlCatalogPtr	xmlNewCatalog		(int sgml);
xmlCatalogPtr	xmlLoadACatalog		(const char *filename);
xmlCatalogPtr	xmlLoadSGMLSuperCatalog	(const char *filename);
int		xmlConvertSGMLCatalog	(xmlCatalogPtr catal);
int		xmlACatalogAdd		(xmlCatalogPtr catal,
					 const xmlChar *type,
					 const xmlChar *orig,
					 const xmlChar *replace);
int		xmlACatalogRemove	(xmlCatalogPtr catal,
					 const xmlChar *value);
xmlChar *	xmlACatalogResolve	(xmlCatalogPtr catal,
					 const xmlChar *pubID,
	                                 const xmlChar *sysID);
xmlChar *	xmlACatalogResolveSystem(xmlCatalogPtr catal,
					 const xmlChar *sysID);
xmlChar *	xmlACatalogResolvePublic(xmlCatalogPtr catal,
					 const xmlChar *pubID);
xmlChar *	xmlACatalogResolveURI	(xmlCatalogPtr catal,
					 const xmlChar *URI);
void		xmlACatalogDump		(xmlCatalogPtr catal,
					 FILE *out);
void		xmlFreeCatalog		(xmlCatalogPtr catal);
int		xmlCatalogIsEmpty	(xmlCatalogPtr catal);

/*
 * Global operations.
 */
void		xmlInitializeCatalog	(void);
int		xmlLoadCatalog		(const char *filename);
void		xmlLoadCatalogs		(const char *paths);
void		xmlCatalogCleanup	(void);
void		xmlCatalogDump		(FILE *out);
xmlChar *	xmlCatalogResolve	(const xmlChar *pubID,
	                                 const xmlChar *sysID);
xmlChar *	xmlCatalogResolveSystem	(const xmlChar *sysID);
xmlChar *	xmlCatalogResolvePublic	(const xmlChar *pubID);
xmlChar *	xmlCatalogResolveURI	(const xmlChar *URI);
int		xmlCatalogAdd		(const xmlChar *type,
					 const xmlChar *orig,
					 const xmlChar *replace);
int		xmlCatalogRemove	(const xmlChar *value);
xmlDocPtr	xmlParseCatalogFile	(const char *filename);
int		xmlCatalogConvert	(void);

/*
 * Strictly minimal interfaces for per-document catalogs used
 * by the parser.
 */
void		xmlCatalogFreeLocal	(void *catalogs);
void *		xmlCatalogAddLocal	(void *catalogs,
					 const xmlChar *URL);
xmlChar *	xmlCatalogLocalResolve	(void *catalogs,
					 const xmlChar *pubID,
	                                 const xmlChar *sysID);
xmlChar *	xmlCatalogLocalResolveURI(void *catalogs,
					 const xmlChar *URI);
/*
 * Preference settings.
 */
int		xmlCatalogSetDebug	(int level);
xmlCatalogPrefer xmlCatalogSetDefaultPrefer(xmlCatalogPrefer prefer);
void		xmlCatalogSetDefaults	(xmlCatalogAllow allow);
xmlCatalogAllow	xmlCatalogGetDefaults	(void);


/* DEPRECATED interfaces */
const xmlChar *	xmlCatalogGetSystem	(const xmlChar *sysID);
const xmlChar *	xmlCatalogGetPublic	(const xmlChar *pubID);

#ifdef __cplusplus
}
#endif
#endif /* LIBXML_CATALOG_ENABLED */
#endif /* __XML_CATALOG_H__ */
