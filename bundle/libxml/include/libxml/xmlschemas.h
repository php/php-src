/*
 * schemas.h : interface to the XML Schemas handling and schema validity
 *             checking
 *
 * See Copyright for the status of this software.
 *
 * Daniel.Veillard@w3.org
 */


#ifndef __XML_SCHEMA_H__
#define __XML_SCHEMA_H__

#include <libxml/xmlversion.h>

#ifdef LIBXML_SCHEMAS_ENABLED

#include <libxml/tree.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    XML_SCHEMAS_ERR_OK		= 0,
    XML_SCHEMAS_ERR_NOROOT	= 1,
    XML_SCHEMAS_ERR_UNDECLAREDELEM,
    XML_SCHEMAS_ERR_NOTTOPLEVEL,
    XML_SCHEMAS_ERR_MISSING,
    XML_SCHEMAS_ERR_WRONGELEM,
    XML_SCHEMAS_ERR_NOTYPE,
    XML_SCHEMAS_ERR_NOROLLBACK,
    XML_SCHEMAS_ERR_ISABSTRACT,
    XML_SCHEMAS_ERR_NOTEMPTY,
    XML_SCHEMAS_ERR_ELEMCONT,
    XML_SCHEMAS_ERR_HAVEDEFAULT,
    XML_SCHEMAS_ERR_NOTNILLABLE,
    XML_SCHEMAS_ERR_EXTRACONTENT,
    XML_SCHEMAS_ERR_INVALIDATTR,
    XML_SCHEMAS_ERR_INVALIDELEM,
    XML_SCHEMAS_ERR_NOTDETERMINIST,
    XML_SCHEMAS_ERR_CONSTRUCT,
    XML_SCHEMAS_ERR_INTERNAL,
    XML_SCHEMAS_ERR_NOTSIMPLE,
    XML_SCHEMAS_ERR_ATTRUNKNOWN,
    XML_SCHEMAS_ERR_ATTRINVALID,
    XML_SCHEMAS_ERR_,
    XML_SCHEMAS_ERR_XXX
} xmlSchemaValidError;


/**
 * The schemas related types are kept internal
 */
typedef struct _xmlSchema xmlSchema;
typedef xmlSchema *xmlSchemaPtr;

/**
 * A schemas validation context
 */
typedef void (*xmlSchemaValidityErrorFunc) (void *ctx, const char *msg, ...);
typedef void (*xmlSchemaValidityWarningFunc) (void *ctx, const char *msg, ...);

typedef struct _xmlSchemaParserCtxt xmlSchemaParserCtxt;
typedef xmlSchemaParserCtxt *xmlSchemaParserCtxtPtr;

typedef struct _xmlSchemaValidCtxt xmlSchemaValidCtxt;
typedef xmlSchemaValidCtxt *xmlSchemaValidCtxtPtr;

/*
 * Interfaces for parsing.
 */
xmlSchemaParserCtxtPtr xmlSchemaNewParserCtxt	(const char *URL);
xmlSchemaParserCtxtPtr xmlSchemaNewMemParserCtxt(const char *buffer,
						 int size);
void		xmlSchemaFreeParserCtxt	(xmlSchemaParserCtxtPtr ctxt);
void		xmlSchemaSetParserErrors(xmlSchemaParserCtxtPtr ctxt,
					 xmlSchemaValidityErrorFunc err,
					 xmlSchemaValidityWarningFunc warn,
					 void *ctx);
xmlSchemaPtr	xmlSchemaParse		(xmlSchemaParserCtxtPtr ctxt);
void		xmlSchemaFree		(xmlSchemaPtr schema);
void		xmlSchemaDump		(FILE *output,
					 xmlSchemaPtr schema);
/*
 * Interfaces for validating
 */
void		xmlSchemaSetValidErrors	(xmlSchemaValidCtxtPtr ctxt,
					 xmlSchemaValidityErrorFunc err,
					 xmlSchemaValidityWarningFunc warn,
					 void *ctx);
xmlSchemaValidCtxtPtr	xmlSchemaNewValidCtxt	(xmlSchemaPtr schema);
void			xmlSchemaFreeValidCtxt	(xmlSchemaValidCtxtPtr ctxt);
int			xmlSchemaValidateDoc	(xmlSchemaValidCtxtPtr ctxt,
					 	 xmlDocPtr instance);
int			xmlSchemaValidateStream	(xmlSchemaValidCtxtPtr ctxt,
						 xmlParserInputBufferPtr input,
						 xmlCharEncoding enc,
					 	 xmlSAXHandlerPtr sax,
						 void *user_data);
#ifdef __cplusplus
}
#endif

#endif /* LIBXML_SCHEMAS_ENABLED */
#endif /* __XML_SCHEMA_H__ */
