/*
 * SAX.c : Default SAX handler to build a tree.
 *
 * See Copyright for the status of this software.
 *
 * Daniel Veillard <daniel@veillard.com>
 */


#define IN_LIBXML
#include "libxml.h"
#include <stdlib.h>
#include <string.h>
#include <libxml/xmlmemory.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/valid.h>
#include <libxml/entities.h>
#include <libxml/xmlerror.h>
#include <libxml/debugXML.h>
#include <libxml/xmlIO.h>
#include <libxml/SAX.h>
#include <libxml/uri.h>
#include <libxml/valid.h>
#include <libxml/HTMLtree.h>
#include <libxml/globals.h>

/* #define DEBUG_SAX */
/* #define DEBUG_SAX_TREE */

/**
 * getPublicId:
 * @ctx: the user data (XML parser context)
 *
 * Provides the public ID e.g. "-//SGMLSOURCE//DTD DEMO//EN"
 *
 * Returns a xmlChar *
 */
const xmlChar *
getPublicId(void *ctx ATTRIBUTE_UNUSED)
{
    /* xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx; */
    return(NULL);
}

/**
 * getSystemId:
 * @ctx: the user data (XML parser context)
 *
 * Provides the system ID, basically URL or filename e.g.
 * http://www.sgmlsource.com/dtds/memo.dtd
 *
 * Returns a xmlChar *
 */
const xmlChar *
getSystemId(void *ctx)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    return((const xmlChar *) ctxt->input->filename); 
}

/**
 * getLineNumber:
 * @ctx: the user data (XML parser context)
 *
 * Provide the line number of the current parsing point.
 *
 * Returns an int
 */
int
getLineNumber(void *ctx)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    return(ctxt->input->line);
}

/**
 * getColumnNumber:
 * @ctx: the user data (XML parser context)
 *
 * Provide the column number of the current parsing point.
 *
 * Returns an int
 */
int
getColumnNumber(void *ctx)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    return(ctxt->input->col);
}

/**
 * isStandalone:
 * @ctx: the user data (XML parser context)
 *
 * Is this document tagged standalone ?
 *
 * Returns 1 if true
 */
int
isStandalone(void *ctx)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    return(ctxt->myDoc->standalone == 1);
}

/**
 * hasInternalSubset:
 * @ctx: the user data (XML parser context)
 *
 * Does this document has an internal subset
 *
 * Returns 1 if true
 */
int
hasInternalSubset(void *ctx)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    return(ctxt->myDoc->intSubset != NULL);
}

/**
 * hasExternalSubset:
 * @ctx: the user data (XML parser context)
 *
 * Does this document has an external subset
 *
 * Returns 1 if true
 */
int
hasExternalSubset(void *ctx)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    return(ctxt->myDoc->extSubset != NULL);
}

/**
 * internalSubset:
 * @ctx:  the user data (XML parser context)
 * @name:  the root element name
 * @ExternalID:  the external ID
 * @SystemID:  the SYSTEM ID (e.g. filename or URL)
 *
 * Callback on internal subset declaration.
 */
void
internalSubset(void *ctx, const xmlChar *name,
	       const xmlChar *ExternalID, const xmlChar *SystemID)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlDtdPtr dtd;
#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.internalSubset(%s, %s, %s)\n",
            name, ExternalID, SystemID);
#endif

    if (ctxt->myDoc == NULL)
	return;
    dtd = xmlGetIntSubset(ctxt->myDoc);
    if (dtd != NULL) {
	if (ctxt->html)
	    return;
	xmlUnlinkNode((xmlNodePtr) dtd);
	xmlFreeDtd(dtd);
	ctxt->myDoc->intSubset = NULL;
    }
    ctxt->myDoc->intSubset = 
	xmlCreateIntSubset(ctxt->myDoc, name, ExternalID, SystemID);
}

/**
 * externalSubset:
 * @ctx: the user data (XML parser context)
 * @name:  the root element name
 * @ExternalID:  the external ID
 * @SystemID:  the SYSTEM ID (e.g. filename or URL)
 *
 * Callback on external subset declaration.
 */
void
externalSubset(void *ctx, const xmlChar *name,
	       const xmlChar *ExternalID, const xmlChar *SystemID)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.externalSubset(%s, %s, %s)\n",
            name, ExternalID, SystemID);
#endif
    if (((ExternalID != NULL) || (SystemID != NULL)) &&
        (((ctxt->validate) || (ctxt->loadsubset != 0)) &&
	 (ctxt->wellFormed && ctxt->myDoc))) {
	/*
	 * Try to fetch and parse the external subset.
	 */
	xmlParserInputPtr oldinput;
	int oldinputNr;
	int oldinputMax;
	xmlParserInputPtr *oldinputTab;
	xmlParserInputPtr input = NULL;
	xmlCharEncoding enc;
	int oldcharset;

	/*
	 * Ask the Entity resolver to load the damn thing
	 */
	if ((ctxt->sax != NULL) && (ctxt->sax->resolveEntity != NULL))
	    input = ctxt->sax->resolveEntity(ctxt->userData, ExternalID,
	                                        SystemID);
	if (input == NULL) {
	    return;
	}

	xmlNewDtd(ctxt->myDoc, name, ExternalID, SystemID);

	/*
	 * make sure we won't destroy the main document context
	 */
	oldinput = ctxt->input;
	oldinputNr = ctxt->inputNr;
	oldinputMax = ctxt->inputMax;
	oldinputTab = ctxt->inputTab;
	oldcharset = ctxt->charset;

	ctxt->inputTab = (xmlParserInputPtr *)
	                 xmlMalloc(5 * sizeof(xmlParserInputPtr));
	if (ctxt->inputTab == NULL) {
	    ctxt->errNo = XML_ERR_NO_MEMORY;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData, 
		     "externalSubset: out of memory\n");
	    ctxt->errNo = XML_ERR_NO_MEMORY;
	    ctxt->input = oldinput;
	    ctxt->inputNr = oldinputNr;
	    ctxt->inputMax = oldinputMax;
	    ctxt->inputTab = oldinputTab;
	    ctxt->charset = oldcharset;
	    return;
	}
	ctxt->inputNr = 0;
	ctxt->inputMax = 5;
	ctxt->input = NULL;
	xmlPushInput(ctxt, input);

	/*
	 * On the fly encoding conversion if needed
	 */
	enc = xmlDetectCharEncoding(ctxt->input->cur, 4);
	xmlSwitchEncoding(ctxt, enc);

	if (input->filename == NULL)
	    input->filename = (char *) xmlStrdup(SystemID);
	input->line = 1;
	input->col = 1;
	input->base = ctxt->input->cur;
	input->cur = ctxt->input->cur;
	input->free = NULL;

	/*
	 * let's parse that entity knowing it's an external subset.
	 */
	xmlParseExternalSubset(ctxt, ExternalID, SystemID);

        /*
	 * Free up the external entities
	 */

	while (ctxt->inputNr > 1)
	    xmlPopInput(ctxt);
	xmlFreeInputStream(ctxt->input);
        xmlFree(ctxt->inputTab);

	/*
	 * Restore the parsing context of the main entity
	 */
	ctxt->input = oldinput;
	ctxt->inputNr = oldinputNr;
	ctxt->inputMax = oldinputMax;
	ctxt->inputTab = oldinputTab;
	ctxt->charset = oldcharset;
	/* ctxt->wellFormed = oldwellFormed; */
    }
}

/**
 * resolveEntity:
 * @ctx: the user data (XML parser context)
 * @publicId: The public ID of the entity
 * @systemId: The system ID of the entity
 *
 * The entity loader, to control the loading of external entities,
 * the application can either:
 *    - override this resolveEntity() callback in the SAX block
 *    - or better use the xmlSetExternalEntityLoader() function to
 *      set up it's own entity resolution routine
 *
 * Returns the xmlParserInputPtr if inlined or NULL for DOM behaviour.
 */
xmlParserInputPtr
resolveEntity(void *ctx, const xmlChar *publicId, const xmlChar *systemId)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlParserInputPtr ret;
    xmlChar *URI;
    const char *base = NULL;

    if (ctxt->input != NULL)
	base = ctxt->input->filename;
    if (base == NULL)
	base = ctxt->directory;

    URI = xmlBuildURI(systemId, (const xmlChar *) base);

#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.resolveEntity(%s, %s)\n", publicId, systemId);
#endif

    ret = xmlLoadExternalEntity((const char *) URI,
				(const char *) publicId, ctxt);
    if (URI != NULL)
	xmlFree(URI);
    return(ret);
}

/**
 * getEntity:
 * @ctx: the user data (XML parser context)
 * @name: The entity name
 *
 * Get an entity by name
 *
 * Returns the xmlEntityPtr if found.
 */
xmlEntityPtr
getEntity(void *ctx, const xmlChar *name)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlEntityPtr ret;

#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.getEntity(%s)\n", name);
#endif

    if ((ctxt->myDoc != NULL) && (ctxt->myDoc->standalone == 1)) {
	if (ctxt->inSubset == 2) {
	    ctxt->myDoc->standalone = 0;
	    ret = xmlGetDocEntity(ctxt->myDoc, name);
	    ctxt->myDoc->standalone = 1;
	} else {
	    ret = xmlGetDocEntity(ctxt->myDoc, name);
	    if (ret == NULL) {
		ctxt->myDoc->standalone = 0;
		ret = xmlGetDocEntity(ctxt->myDoc, name);
		if (ret != NULL) {
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt, 
		 "Entity(%s) document marked standalone but require external subset\n",
					 name);
		    ctxt->valid = 0;
		    ctxt->wellFormed = 0;
		}
		ctxt->myDoc->standalone = 1;
	    }
	}
    } else {
	ret = xmlGetDocEntity(ctxt->myDoc, name);
    }
    if ((ret != NULL) && (ctxt->validate) && (ret->children == NULL) &&
	(ret->etype == XML_EXTERNAL_GENERAL_PARSED_ENTITY)) {
	/*
	 * for validation purposes we really need to fetch and
	 * parse the external entity
	 */
	xmlNodePtr children;

        xmlParseCtxtExternalEntity(ctxt, ret->URI, ret->ExternalID, &children);
	xmlAddChildList((xmlNodePtr) ret, children);
    }
    return(ret);
}

/**
 * getParameterEntity:
 * @ctx: the user data (XML parser context)
 * @name: The entity name
 *
 * Get a parameter entity by name
 *
 * Returns the xmlEntityPtr if found.
 */
xmlEntityPtr
getParameterEntity(void *ctx, const xmlChar *name)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlEntityPtr ret;

#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.getParameterEntity(%s)\n", name);
#endif

    ret = xmlGetParameterEntity(ctxt->myDoc, name);
    return(ret);
}


/**
 * entityDecl:
 * @ctx: the user data (XML parser context)
 * @name:  the entity name 
 * @type:  the entity type 
 * @publicId: The public ID of the entity
 * @systemId: The system ID of the entity
 * @content: the entity value (without processing).
 *
 * An entity definition has been parsed
 */
void
entityDecl(void *ctx, const xmlChar *name, int type,
          const xmlChar *publicId, const xmlChar *systemId, xmlChar *content)
{
    xmlEntityPtr ent;
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;

#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.entityDecl(%s, %d, %s, %s, %s)\n",
            name, type, publicId, systemId, content);
#endif
    if (ctxt->inSubset == 1) {
	ent = xmlAddDocEntity(ctxt->myDoc, name, type, publicId,
		              systemId, content);
	if ((ent == NULL) && (ctxt->pedantic) &&
	    (ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
	    ctxt->sax->warning(ctxt, 
	     "Entity(%s) already defined in the internal subset\n", name);
	if ((ent != NULL) && (ent->URI == NULL) && (systemId != NULL)) {
	    xmlChar *URI;
	    const char *base = NULL;

	    if (ctxt->input != NULL)
		base = ctxt->input->filename;
	    if (base == NULL)
		base = ctxt->directory;
	
	    URI = xmlBuildURI(systemId, (const xmlChar *) base);
	    ent->URI = URI;
	}
    } else if (ctxt->inSubset == 2) {
	ent = xmlAddDtdEntity(ctxt->myDoc, name, type, publicId,
		              systemId, content);
	if ((ent == NULL) && (ctxt->pedantic) &&
	    (ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
	    ctxt->sax->warning(ctxt, 
	     "Entity(%s) already defined in the external subset\n", name);
	if ((ent != NULL) && (ent->URI == NULL) && (systemId != NULL)) {
	    xmlChar *URI;
	    const char *base = NULL;

	    if (ctxt->input != NULL)
		base = ctxt->input->filename;
	    if (base == NULL)
		base = ctxt->directory;
	
	    URI = xmlBuildURI(systemId, (const xmlChar *) base);
	    ent->URI = URI;
	}
    } else {
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt, 
	     "SAX.entityDecl(%s) called while not in subset\n", name);
    }
}

/**
 * attributeDecl:
 * @ctx: the user data (XML parser context)
 * @elem:  the name of the element
 * @fullname:  the attribute name 
 * @type:  the attribute type 
 * @def:  the type of default value
 * @defaultValue: the attribute default value
 * @tree:  the tree of enumerated value set
 *
 * An attribute definition has been parsed
 */
void
attributeDecl(void *ctx, const xmlChar *elem, const xmlChar *fullname,
              int type, int def, const xmlChar *defaultValue,
	      xmlEnumerationPtr tree)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlAttributePtr attr;
    xmlChar *name = NULL, *prefix = NULL;

#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.attributeDecl(%s, %s, %d, %d, %s, ...)\n",
            elem, fullname, type, def, defaultValue);
#endif
    name = xmlSplitQName(ctxt, fullname, &prefix);
    ctxt->vctxt.valid = 1;
    if (ctxt->inSubset == 1)
	attr = xmlAddAttributeDecl(&ctxt->vctxt, ctxt->myDoc->intSubset, elem,
	       name, prefix, (xmlAttributeType) type,
	       (xmlAttributeDefault) def, defaultValue, tree);
    else if (ctxt->inSubset == 2)
	attr = xmlAddAttributeDecl(&ctxt->vctxt, ctxt->myDoc->extSubset, elem,
	   name, prefix, (xmlAttributeType) type, 
	   (xmlAttributeDefault) def, defaultValue, tree);
    else {
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt, 
	     "SAX.attributeDecl(%s) called while not in subset\n", name);
	return;
    }
    if (ctxt->vctxt.valid == 0)
	ctxt->valid = 0;
    if ((attr != NULL) && (ctxt->validate) && (ctxt->wellFormed) &&
        (ctxt->myDoc != NULL) && (ctxt->myDoc->intSubset != NULL))
	ctxt->valid &= xmlValidateAttributeDecl(&ctxt->vctxt, ctxt->myDoc,
	                                        attr);
    if (prefix != NULL)
	xmlFree(prefix);
    if (name != NULL)
	xmlFree(name);
}

/**
 * elementDecl:
 * @ctx: the user data (XML parser context)
 * @name:  the element name 
 * @type:  the element type 
 * @content: the element value tree
 *
 * An element definition has been parsed
 */
void
elementDecl(void *ctx, const xmlChar * name, int type,
            xmlElementContentPtr content)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlElementPtr elem = NULL;

#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
                    "SAX.elementDecl(%s, %d, ...)\n", name, type);
#endif

    if (ctxt->inSubset == 1)
        elem = xmlAddElementDecl(&ctxt->vctxt, ctxt->myDoc->intSubset,
                                 name, (xmlElementTypeVal) type, content);
    else if (ctxt->inSubset == 2)
        elem = xmlAddElementDecl(&ctxt->vctxt, ctxt->myDoc->extSubset,
                                 name, (xmlElementTypeVal) type, content);
    else {
        if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
            ctxt->sax->error(ctxt,
                             "SAX.elementDecl(%s) called while not in subset\n",
                             name);
        return;
    }
    if (elem == NULL)
        ctxt->valid = 0;
    if (ctxt->validate && ctxt->wellFormed &&
        ctxt->myDoc && ctxt->myDoc->intSubset)
        ctxt->valid &=
            xmlValidateElementDecl(&ctxt->vctxt, ctxt->myDoc, elem);
}

/**
 * notationDecl:
 * @ctx: the user data (XML parser context)
 * @name: The name of the notation
 * @publicId: The public ID of the entity
 * @systemId: The system ID of the entity
 *
 * What to do when a notation declaration has been parsed.
 */
void
notationDecl(void *ctx, const xmlChar *name,
	     const xmlChar *publicId, const xmlChar *systemId)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlNotationPtr nota = NULL;

#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.notationDecl(%s, %s, %s)\n", name, publicId, systemId);
#endif

    if ((publicId == NULL) && (systemId == NULL)) {
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt, 
	     "SAX.notationDecl(%s) externalID or PublicID missing\n", name);
	ctxt->valid = 0;
	ctxt->wellFormed = 0;
	return;
    } else if (ctxt->inSubset == 1)
	nota = xmlAddNotationDecl(&ctxt->vctxt, ctxt->myDoc->intSubset, name,
                              publicId, systemId);
    else if (ctxt->inSubset == 2)
	nota = xmlAddNotationDecl(&ctxt->vctxt, ctxt->myDoc->extSubset, name,
                              publicId, systemId);
    else {
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt, 
	     "SAX.notationDecl(%s) called while not in subset\n", name);
	return;
    }
    if (nota == NULL) ctxt->valid = 0;
    if (ctxt->validate && ctxt->wellFormed &&
        ctxt->myDoc && ctxt->myDoc->intSubset)
	ctxt->valid &= xmlValidateNotationDecl(&ctxt->vctxt, ctxt->myDoc,
	                                       nota);
}

/**
 * unparsedEntityDecl:
 * @ctx: the user data (XML parser context)
 * @name: The name of the entity
 * @publicId: The public ID of the entity
 * @systemId: The system ID of the entity
 * @notationName: the name of the notation
 *
 * What to do when an unparsed entity declaration is parsed
 */
void
unparsedEntityDecl(void *ctx, const xmlChar *name,
		   const xmlChar *publicId, const xmlChar *systemId,
		   const xmlChar *notationName)
{
    xmlEntityPtr ent;
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.unparsedEntityDecl(%s, %s, %s, %s)\n",
            name, publicId, systemId, notationName);
#endif
#if 0
    Done in xmlValidateDtdFinal now.
    if (ctxt->validate && ctxt->wellFormed && ctxt->myDoc) {
	int ret;
	ret = xmlValidateNotationUse(&ctxt->vctxt, ctxt->myDoc,
	                                      notationName);
	if (ret == 0) {
	    ctxt->wellFormed = 0;
	    ctxt->valid = 0;
	}
    }
#endif
    if (ctxt->inSubset == 1) {
	ent = xmlAddDocEntity(ctxt->myDoc, name,
			XML_EXTERNAL_GENERAL_UNPARSED_ENTITY,
			publicId, systemId, notationName);
	if ((ent == NULL) && (ctxt->pedantic) &&
	    (ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
	    ctxt->sax->warning(ctxt, 
	     "Entity(%s) already defined in the internal subset\n", name);
	if ((ent != NULL) && (ent->URI == NULL) && (systemId != NULL)) {
	    xmlChar *URI;
	    const char *base = NULL;

	    if (ctxt->input != NULL)
		base = ctxt->input->filename;
	    if (base == NULL)
		base = ctxt->directory;
	
	    URI = xmlBuildURI(systemId, (const xmlChar *) base);
	    ent->URI = URI;
	}
    } else if (ctxt->inSubset == 2) {
	ent = xmlAddDtdEntity(ctxt->myDoc, name,
			XML_EXTERNAL_GENERAL_UNPARSED_ENTITY,
			publicId, systemId, notationName);
	if ((ent == NULL) && (ctxt->pedantic) &&
	    (ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
	    ctxt->sax->warning(ctxt, 
	     "Entity(%s) already defined in the external subset\n", name);
	if ((ent != NULL) && (ent->URI == NULL) && (systemId != NULL)) {
	    xmlChar *URI;
	    const char *base = NULL;

	    if (ctxt->input != NULL)
		base = ctxt->input->filename;
	    if (base == NULL)
		base = ctxt->directory;
	
	    URI = xmlBuildURI(systemId, (const xmlChar *) base);
	    ent->URI = URI;
	}
    } else {
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt, 
	     "SAX.unparsedEntityDecl(%s) called while not in subset\n", name);
    }
}

/**
 * setDocumentLocator:
 * @ctx: the user data (XML parser context)
 * @loc: A SAX Locator
 *
 * Receive the document locator at startup, actually xmlDefaultSAXLocator
 * Everything is available on the context, so this is useless in our case.
 */
void
setDocumentLocator(void *ctx ATTRIBUTE_UNUSED, xmlSAXLocatorPtr loc ATTRIBUTE_UNUSED)
{
    /* xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx; */
#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.setDocumentLocator()\n");
#endif
}

/**
 * startDocument:
 * @ctx: the user data (XML parser context)
 *
 * called when the document start being processed.
 */
void
startDocument(void *ctx)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlDocPtr doc;

#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.startDocument()\n");
#endif
    if (ctxt->html) {
	if (ctxt->myDoc == NULL)
#ifdef LIBXML_HTML_ENABLED
	    ctxt->myDoc = htmlNewDocNoDtD(NULL, NULL);
#else
        xmlGenericError(xmlGenericErrorContext,
		"libxml2 built without HTML support\n");
#endif
    } else {
	doc = ctxt->myDoc = xmlNewDoc(ctxt->version);
	if (doc != NULL) {
	    if (ctxt->encoding != NULL)
		doc->encoding = xmlStrdup(ctxt->encoding);
	    else
		doc->encoding = NULL;
	    doc->standalone = ctxt->standalone;
	}
    }
    if ((ctxt->myDoc != NULL) && (ctxt->myDoc->URL == NULL) &&
	(ctxt->input != NULL) && (ctxt->input->filename != NULL)) {
        ctxt->myDoc->URL = xmlStrdup((const xmlChar *) ctxt->input->filename);
    }
}

/**
 * endDocument:
 * @ctx: the user data (XML parser context)
 *
 * called when the document end has been detected.
 */
void
endDocument(void *ctx)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.endDocument()\n");
#endif
    if (ctxt->validate && ctxt->wellFormed &&
        ctxt->myDoc && ctxt->myDoc->intSubset)
	ctxt->valid &= xmlValidateDocumentFinal(&ctxt->vctxt, ctxt->myDoc);

    /*
     * Grab the encoding if it was added on-the-fly
     */
    if ((ctxt->encoding != NULL) && (ctxt->myDoc != NULL) &&
	(ctxt->myDoc->encoding == NULL)) {
	ctxt->myDoc->encoding = ctxt->encoding;
	ctxt->encoding = NULL;
    }
    if ((ctxt->inputTab[0]->encoding != NULL) && (ctxt->myDoc != NULL) &&
	(ctxt->myDoc->encoding == NULL)) {
	ctxt->myDoc->encoding = xmlStrdup(ctxt->inputTab[0]->encoding);
    }
    if ((ctxt->charset != XML_CHAR_ENCODING_NONE) && (ctxt->myDoc != NULL) &&
	(ctxt->myDoc->charset == XML_CHAR_ENCODING_NONE)) {
	ctxt->myDoc->charset = ctxt->charset;
    }
}

/**
 * my_attribute:
 * @ctx: the user data (XML parser context)
 * @fullname:  The attribute name, including namespace prefix
 * @value:  The attribute value
 * @prefix: the prefix on the element node
 *
 * Handle an attribute that has been read by the parser.
 * The default handling is to convert the attribute into an
 * DOM subtree and past it in a new xmlAttr element added to
 * the element.
 */
static void
my_attribute(void *ctx, const xmlChar *fullname, const xmlChar *value,
	     const xmlChar *prefix)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlAttrPtr ret;
    xmlChar *name;
    xmlChar *ns;
    xmlChar *nval;
    xmlNsPtr namespace;

/****************
#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
    "SAX.attribute(%s, %s)\n", fullname, value);
#endif
 ****************/
    /*
     * Split the full name into a namespace prefix and the tag name
     */
    name = xmlSplitQName(ctxt, fullname, &ns);

    /*
     * Do the last stage of the attribute normalization
     * Needed for HTML too:
     *   http://www.w3.org/TR/html4/types.html#h-6.2
     */
    ctxt->vctxt.valid = 1;
    nval = xmlValidCtxtNormalizeAttributeValue(&ctxt->vctxt,
	                                   ctxt->myDoc, ctxt->node,
					   fullname, value);
    if (ctxt->vctxt.valid != 1) {
	ctxt->valid = 0;
    }
    if (nval != NULL)
	value = nval;

    /*
     * Check whether it's a namespace definition
     */
    if ((!ctxt->html) && (ns == NULL) &&
        (name[0] == 'x') && (name[1] == 'm') && (name[2] == 'l') &&
        (name[3] == 'n') && (name[4] == 's') && (name[5] == 0)) {
	xmlNsPtr nsret;

	if (value[0] != 0) {
	    xmlURIPtr uri;

	    uri = xmlParseURI((const char *)value);
	    if (uri == NULL) {
		if ((ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
		    ctxt->sax->warning(ctxt->userData, 
			 "nmlns: %s not a valid URI\n", value);
	    } else {
		if (uri->scheme == NULL) {
		    if ((ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
			ctxt->sax->warning(ctxt->userData, 
			     "nmlns: URI %s is not absolute\n", value);
		}
		xmlFreeURI(uri);
	    }
	}

	/* a default namespace definition */
	nsret = xmlNewNs(ctxt->node, value, NULL);

	/*
	 * Validate also for namespace decls, they are attributes from
	 * an XML-1.0 perspective
	 */
        if (nsret != NULL && ctxt->validate && ctxt->wellFormed &&
	    ctxt->myDoc && ctxt->myDoc->intSubset)
	    ctxt->valid &= xmlValidateOneNamespace(&ctxt->vctxt, ctxt->myDoc,
					   ctxt->node, prefix, nsret, value);
	if (name != NULL) 
	    xmlFree(name);
	if (nval != NULL)
	    xmlFree(nval);
	return;
    }
    if ((!ctxt->html) &&
	(ns != NULL) && (ns[0] == 'x') && (ns[1] == 'm') && (ns[2] == 'l') &&
        (ns[3] == 'n') && (ns[4] == 's') && (ns[5] == 0)) {
	xmlNsPtr nsret;

	if (value[0] == 0) {
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData, 
		     "Empty namespace name for prefix %s\n", name);
	}
	/* a standard namespace definition */
	nsret = xmlNewNs(ctxt->node, value, name);
	xmlFree(ns);
	/*
	 * Validate also for namespace decls, they are attributes from
	 * an XML-1.0 perspective
	 */
        if (nsret != NULL && ctxt->validate && ctxt->wellFormed &&
	    ctxt->myDoc && ctxt->myDoc->intSubset)
	    ctxt->valid &= xmlValidateOneNamespace(&ctxt->vctxt, ctxt->myDoc,
					   ctxt->node, prefix, nsret, value);
	if (name != NULL) 
	    xmlFree(name);
	if (nval != NULL)
	    xmlFree(nval);
	return;
    }

    if (ns != NULL)
	namespace = xmlSearchNs(ctxt->myDoc, ctxt->node, ns);
    else {
	namespace = NULL;
    }

    /* !!!!!! <a toto:arg="" xmlns:toto="http://toto.com"> */
    ret = xmlNewNsPropEatName(ctxt->node, namespace, name, NULL);

    if (ret != NULL) {
        if ((ctxt->replaceEntities == 0) && (!ctxt->html)) {
	    xmlNodePtr tmp;

	    ret->children = xmlStringGetNodeList(ctxt->myDoc, value);
	    tmp = ret->children;
	    while (tmp != NULL) {
		tmp->parent = (xmlNodePtr) ret;
		if (tmp->next == NULL)
		    ret->last = tmp;
		tmp = tmp->next;
	    }
	} else if (value != NULL) {
	    ret->children = xmlNewDocText(ctxt->myDoc, value);
	    ret->last = ret->children;
	    if (ret->children != NULL)
		ret->children->parent = (xmlNodePtr) ret;
	}
    }

    if ((!ctxt->html) && ctxt->validate && ctxt->wellFormed &&
        ctxt->myDoc && ctxt->myDoc->intSubset) {
	
	/*
	 * If we don't substitute entities, the validation should be
	 * done on a value with replaced entities anyway.
	 */
        if (!ctxt->replaceEntities) {
	    xmlChar *val;

	    ctxt->depth++;
	    val = xmlStringDecodeEntities(ctxt, value, XML_SUBSTITUTE_REF,
		                          0,0,0);
	    ctxt->depth--;
	    
	    if (val == NULL)
		ctxt->valid &= xmlValidateOneAttribute(&ctxt->vctxt,
				ctxt->myDoc, ctxt->node, ret, value);
	    else {
		xmlChar *nvalnorm;

		/*
		 * Do the last stage of the attribute normalization
		 * It need to be done twice ... it's an extra burden related
		 * to the ability to keep references in attributes
		 */
		nvalnorm = xmlValidNormalizeAttributeValue(ctxt->myDoc,
					    ctxt->node, fullname, val);
		if (nvalnorm != NULL) {
		    xmlFree(val);
		    val = nvalnorm;
		}

		ctxt->valid &= xmlValidateOneAttribute(&ctxt->vctxt,
			        ctxt->myDoc, ctxt->node, ret, val);
                xmlFree(val);
	    }
	} else {
	    ctxt->valid &= xmlValidateOneAttribute(&ctxt->vctxt, ctxt->myDoc,
					       ctxt->node, ret, value);
	}
    } else if (((ctxt->replaceEntities == 0) && (ctxt->external != 2)) ||
	       ((ctxt->replaceEntities != 0) && (ctxt->inSubset == 0))) {
        /*
	 * when validating, the ID registration is done at the attribute
	 * validation level. Otherwise we have to do specific handling here.
	 */
	if (xmlIsID(ctxt->myDoc, ctxt->node, ret))
	    xmlAddID(&ctxt->vctxt, ctxt->myDoc, value, ret);
	else if (xmlIsRef(ctxt->myDoc, ctxt->node, ret))
	    xmlAddRef(&ctxt->vctxt, ctxt->myDoc, value, ret);
    }

    if (nval != NULL)
	xmlFree(nval);
    if (ns != NULL) 
	xmlFree(ns);
}

/**
 * attribute:
 * @ctx: the user data (XML parser context)
 * @fullname:  The attribute name, including namespace prefix
 * @value:  The attribute value
 *
 * Handle an attribute that has been read by the parser.
 * The default handling is to convert the attribute into an
 * DOM subtree and past it in a new xmlAttr element added to
 * the element.
 */
void
attribute(void *ctx, const xmlChar *fullname, const xmlChar *value)
{
    my_attribute(ctx, fullname, value, NULL);
}

/*
 * xmlCheckDefaultedAttributes:
 *
 * Check defaulted attributes from the DTD
 */
static void
xmlCheckDefaultedAttributes(xmlParserCtxtPtr ctxt, const xmlChar *name,
	const xmlChar *prefix, const xmlChar **atts) {
    xmlElementPtr elemDecl;
    const xmlChar *att;
    int internal = 1;
    int i;

    elemDecl = xmlGetDtdQElementDesc(ctxt->myDoc->intSubset, name, prefix);
    if (elemDecl == NULL) {
	elemDecl = xmlGetDtdQElementDesc(ctxt->myDoc->extSubset, name, prefix);
	internal = 0;
    }

process_external_subset:

    if (elemDecl != NULL) {
	xmlAttributePtr attr = elemDecl->attributes;
	/*
	 * Check against defaulted attributes from the external subset
	 * if the document is stamped as standalone
	 */
	if ((ctxt->myDoc->standalone == 1) &&
	    (ctxt->myDoc->extSubset != NULL) &&
	    (ctxt->validate)) {
	    while (attr != NULL) {
		if ((attr->defaultValue != NULL) &&
		    (xmlGetDtdQAttrDesc(ctxt->myDoc->extSubset,
					attr->elem, attr->name,
					attr->prefix) == attr) &&
		    (xmlGetDtdQAttrDesc(ctxt->myDoc->intSubset,
					attr->elem, attr->name,
					attr->prefix) == NULL)) {
		    xmlChar *fulln;

		    if (attr->prefix != NULL) {
			fulln = xmlStrdup(attr->prefix);
			fulln = xmlStrcat(fulln, BAD_CAST ":");
			fulln = xmlStrcat(fulln, attr->name);
		    } else {
			fulln = xmlStrdup(attr->name);
		    }

		    /*
		     * Check that the attribute is not declared in the
		     * serialization
		     */
		    att = NULL;
		    if (atts != NULL) {
			i = 0;
			att = atts[i];
			while (att != NULL) {
			    if (xmlStrEqual(att, fulln))
				break;
			    i += 2;
			    att = atts[i];
			}
		    }
		    if (att == NULL) {
			if (ctxt->vctxt.error != NULL)
			    ctxt->vctxt.error(ctxt->vctxt.userData,
      "standalone: attribute %s on %s defaulted from external subset\n",
					      fulln, attr->elem);
			ctxt->valid = 0;
		    }
		}
		attr = attr->nexth;
	    }
	}

	/*
	 * Actually insert defaulted values when needed
	 */
	attr = elemDecl->attributes;
	while (attr != NULL) {
	    /*
	     * Make sure that attributes redefinition occuring in the
	     * internal subset are not overriden by definitions in the
	     * external subset.
	     */
	    if (attr->defaultValue != NULL) {
		/*
		 * the element should be instantiated in the tree if:
		 *  - this is a namespace prefix
		 *  - the user required for completion in the tree
		 *    like XSLT
		 *  - there isn't already an attribute definition 
		 *    in the internal subset overriding it.
		 */
		if (((attr->prefix != NULL) &&
		     (xmlStrEqual(attr->prefix, BAD_CAST "xmlns"))) ||
		    ((attr->prefix == NULL) &&
		     (xmlStrEqual(attr->name, BAD_CAST "xmlns"))) ||
		    (ctxt->loadsubset & XML_COMPLETE_ATTRS)) {
		    xmlAttributePtr tst;

		    tst = xmlGetDtdQAttrDesc(ctxt->myDoc->intSubset,
					     attr->elem, attr->name,
					     attr->prefix);
		    if ((tst == attr) || (tst == NULL)) {
			xmlChar *fulln;

			if (attr->prefix != NULL) {
			    fulln = xmlStrdup(attr->prefix);
			    fulln = xmlStrcat(fulln, BAD_CAST ":");
			    fulln = xmlStrcat(fulln, attr->name);
			} else {
			    fulln = xmlStrdup(attr->name);
			}

			/*
			 * Check that the attribute is not declared in the
			 * serialization
			 */
			att = NULL;
			if (atts != NULL) {
			    i = 0;
			    att = atts[i];
			    while (att != NULL) {
				if (xmlStrEqual(att, fulln))
				    break;
				i += 2;
				att = atts[i];
			    }
			}
			if (att == NULL) {
			    attribute(ctxt, fulln, attr->defaultValue);
			}
			xmlFree(fulln);
		    }
		}
	    }
	    attr = attr->nexth;
	}
	if (internal == 1) {
	    elemDecl = xmlGetDtdQElementDesc(ctxt->myDoc->extSubset,
		                             name, prefix);
	    internal = 0;
	    goto process_external_subset;
	}
    }
}

/**
 * startElement:
 * @ctx: the user data (XML parser context)
 * @fullname:  The element name, including namespace prefix
 * @atts:  An array of name/value attributes pairs, NULL terminated
 *
 * called when an opening tag has been processed.
 */
void
startElement(void *ctx, const xmlChar *fullname, const xmlChar **atts)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlNodePtr ret;
    xmlNodePtr parent = ctxt->node;
    xmlNsPtr ns;
    xmlChar *name;
    xmlChar *prefix;
    const xmlChar *att;
    const xmlChar *value;
    int i;

#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.startElement(%s)\n", fullname);
#endif

    /*
     * First check on validity:
     */
    if (ctxt->validate && (ctxt->myDoc->extSubset == NULL) && 
        ((ctxt->myDoc->intSubset == NULL) ||
	 ((ctxt->myDoc->intSubset->notations == NULL) && 
	  (ctxt->myDoc->intSubset->elements == NULL) &&
	  (ctxt->myDoc->intSubset->attributes == NULL) && 
	  (ctxt->myDoc->intSubset->entities == NULL)))) {
	if (ctxt->vctxt.error != NULL) {
            ctxt->vctxt.error(ctxt->vctxt.userData,
	      "Validation failed: no DTD found !\n");
	}
	ctxt->validate = 0;
	ctxt->valid = 0;
	ctxt->errNo = XML_ERR_NO_DTD;
    }
       

    /*
     * Split the full name into a namespace prefix and the tag name
     */
    name = xmlSplitQName(ctxt, fullname, &prefix);


    /*
     * Note : the namespace resolution is deferred until the end of the
     *        attributes parsing, since local namespace can be defined as
     *        an attribute at this level.
     */
    ret = xmlNewDocNodeEatName(ctxt->myDoc, NULL, name, NULL);
    if (ret == NULL) return;
    if (ctxt->myDoc->children == NULL) {
#ifdef DEBUG_SAX_TREE
	xmlGenericError(xmlGenericErrorContext, "Setting %s as root\n", name);
#endif
        xmlAddChild((xmlNodePtr) ctxt->myDoc, (xmlNodePtr) ret);
    } else if (parent == NULL) {
        parent = ctxt->myDoc->children;
    }
    ctxt->nodemem = -1;
    if (ctxt->linenumbers) {
	if (ctxt->input != NULL)
	    ret->content = (void *) (long) ctxt->input->line;
    }

    /*
     * We are parsing a new node.
     */
#ifdef DEBUG_SAX_TREE
    xmlGenericError(xmlGenericErrorContext, "pushing(%s)\n", name);
#endif
    nodePush(ctxt, ret);

    /*
     * Link the child element
     */
    if (parent != NULL) {
        if (parent->type == XML_ELEMENT_NODE) {
#ifdef DEBUG_SAX_TREE
	    xmlGenericError(xmlGenericErrorContext,
		    "adding child %s to %s\n", name, parent->name);
#endif
	    xmlAddChild(parent, ret);
	} else {
#ifdef DEBUG_SAX_TREE
	    xmlGenericError(xmlGenericErrorContext,
		    "adding sibling %s to ", name);
	    xmlDebugDumpOneNode(stderr, parent, 0);
#endif
	    xmlAddSibling(parent, ret);
	}
    }

    /*
     * Insert all the defaulted attributes from the DTD especially namespaces
     */
    if ((!ctxt->html) &&
	((ctxt->myDoc->intSubset != NULL) ||
	 (ctxt->myDoc->extSubset != NULL))) {
	xmlCheckDefaultedAttributes(ctxt, name, prefix, atts);
    }

    /*
     * process all the attributes whose name start with "xmlns"
     */
    if (atts != NULL) {
        i = 0;
	att = atts[i++];
	value = atts[i++];
	if (!ctxt->html) {
	    while ((att != NULL) && (value != NULL)) {
		if ((att[0] == 'x') && (att[1] == 'm') && (att[2] == 'l') &&
		    (att[3] == 'n') && (att[4] == 's'))
		    my_attribute(ctxt, att, value, prefix);

		att = atts[i++];
		value = atts[i++];
	    }
	}
    }

    /*
     * Search the namespace, note that since the attributes have been
     * processed, the local namespaces are available.
     */
    ns = xmlSearchNs(ctxt->myDoc, ret, prefix);
    if ((ns == NULL) && (parent != NULL))
	ns = xmlSearchNs(ctxt->myDoc, parent, prefix);
    if ((prefix != NULL) && (ns == NULL)) {
	ns = xmlNewNs(ret, NULL, prefix);
	if ((ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
	    ctxt->sax->warning(ctxt->userData, 
		 "Namespace prefix %s is not defined\n", prefix);
    }

    /*
     * set the namespace node, making sure that if the default namspace
     * is unbound on a parent we simply kee it NULL
     */
    if ((ns != NULL) && (ns->href != NULL) &&
	((ns->href[0] != 0) || (ns->prefix != NULL)))
	xmlSetNs(ret, ns);

    /*
     * process all the other attributes
     */
    if (atts != NULL) {
        i = 0;
	att = atts[i++];
	value = atts[i++];
	if (ctxt->html) {
	    while (att != NULL) {
		attribute(ctxt, att, value);
		att = atts[i++];
		value = atts[i++];
	    }
	} else {
	    while ((att != NULL) && (value != NULL)) {
		if ((att[0] != 'x') || (att[1] != 'm') || (att[2] != 'l') ||
		    (att[3] != 'n') || (att[4] != 's'))
		    attribute(ctxt, att, value);

		/*
		 * Next ones
		 */
		att = atts[i++];
		value = atts[i++];
	    }
	}
    }

    /*
     * If it's the Document root, finish the DTD validation and
     * check the document root element for validity
     */
    if ((ctxt->validate) && (ctxt->vctxt.finishDtd == 0)) {
	int chk;

	chk = xmlValidateDtdFinal(&ctxt->vctxt, ctxt->myDoc);
	if (chk <= 0)
	    ctxt->valid = 0;
	if (chk < 0)
	    ctxt->wellFormed = 0;
	ctxt->valid &= xmlValidateRoot(&ctxt->vctxt, ctxt->myDoc);
	ctxt->vctxt.finishDtd = 1;
    }

    if (prefix != NULL)
	xmlFree(prefix);

}

/**
 * endElement:
 * @ctx: the user data (XML parser context)
 * @name:  The element name
 *
 * called when the end of an element has been detected.
 */
void
endElement(void *ctx, const xmlChar *name ATTRIBUTE_UNUSED)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlParserNodeInfo node_info;
    xmlNodePtr cur = ctxt->node;

#ifdef DEBUG_SAX
    if (name == NULL)
        xmlGenericError(xmlGenericErrorContext, "SAX.endElement(NULL)\n");
    else
	xmlGenericError(xmlGenericErrorContext, "SAX.endElement(%s)\n", name);
#endif
    
    /* Capture end position and add node */
    if (cur != NULL && ctxt->record_info) {
      node_info.end_pos = ctxt->input->cur - ctxt->input->base;
      node_info.end_line = ctxt->input->line;
      node_info.node = cur;
      xmlParserAddNodeInfo(ctxt, &node_info);
    }
    ctxt->nodemem = -1;

    if (ctxt->validate && ctxt->wellFormed &&
        ctxt->myDoc && ctxt->myDoc->intSubset)
        ctxt->valid &= xmlValidateOneElement(&ctxt->vctxt, ctxt->myDoc,
					     cur);

    
    /*
     * end of parsing of this node.
     */
#ifdef DEBUG_SAX_TREE
    xmlGenericError(xmlGenericErrorContext, "popping(%s)\n", cur->name);
#endif
    nodePop(ctxt);
}

/**
 * reference:
 * @ctx: the user data (XML parser context)
 * @name:  The entity name
 *
 * called when an entity reference is detected. 
 */
void
reference(void *ctx, const xmlChar *name)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlNodePtr ret;

#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.reference(%s)\n", name);
#endif
    if (name[0] == '#')
	ret = xmlNewCharRef(ctxt->myDoc, name);
    else
	ret = xmlNewReference(ctxt->myDoc, name);
#ifdef DEBUG_SAX_TREE
    xmlGenericError(xmlGenericErrorContext,
	    "add reference %s to %s \n", name, ctxt->node->name);
#endif
    xmlAddChild(ctxt->node, ret);
}

/**
 * characters:
 * @ctx: the user data (XML parser context)
 * @ch:  a xmlChar string
 * @len: the number of xmlChar
 *
 * receiving some chars from the parser.
 */
void
characters(void *ctx, const xmlChar *ch, int len)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlNodePtr lastChild;

#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.characters(%.30s, %d)\n", ch, len);
#endif
    /*
     * Handle the data if any. If there is no child
     * add it as content, otherwise if the last child is text,
     * concatenate it, else create a new node of type text.
     */

    if (ctxt->node == NULL) {
#ifdef DEBUG_SAX_TREE
	xmlGenericError(xmlGenericErrorContext,
		"add chars: ctxt->node == NULL !\n");
#endif
        return;
    }
    lastChild = xmlGetLastChild(ctxt->node);
#ifdef DEBUG_SAX_TREE
    xmlGenericError(xmlGenericErrorContext,
	    "add chars to %s \n", ctxt->node->name);
#endif

    /*
     * Here we needed an accelerator mechanism in case of very large
     * elements. Use an attribute in the structure !!!
     */
    if (lastChild == NULL) {
	/* first node, first time */
	xmlNodeAddContentLen(ctxt->node, ch, len);
	if (ctxt->node->children != NULL) {
	    ctxt->nodelen = len;
	    ctxt->nodemem = len + 1;
	}
    } else {
	int coalesceText = (lastChild != NULL) &&
	    (lastChild->type == XML_TEXT_NODE) &&
	    (lastChild->name == xmlStringText);
	if ((coalesceText) && (ctxt->nodemem != 0)) {
	    /*
	     * The whole point of maintaining nodelen and nodemem,
	     * xmlTextConcat is too costly, i.e. compute length,
	     * reallocate a new buffer, move data, append ch. Here
	     * We try to minimaze realloc() uses and avoid copying
	     * and recomputing length over and over.
	     */
	    if (ctxt->nodelen + len >= ctxt->nodemem) {
		xmlChar *newbuf;
		int size;

		size = ctxt->nodemem + len;
		size *= 2;
                newbuf = (xmlChar *) xmlRealloc(lastChild->content,size);
		if (newbuf == NULL) {
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData, 
			     "SAX.characters(): out of memory\n");
		    return;
		}
		ctxt->nodemem = size;
		lastChild->content = newbuf;
	    }
	    memcpy(&lastChild->content[ctxt->nodelen], ch, len);
	    ctxt->nodelen += len;
	    lastChild->content[ctxt->nodelen] = 0;
	} else if (coalesceText) {
	    xmlTextConcat(lastChild, ch, len);
	    if (ctxt->node->children != NULL) {
		ctxt->nodelen = xmlStrlen(lastChild->content);
		ctxt->nodemem = ctxt->nodelen + 1;
	    }
	} else {
	    /* Mixed content, first time */
	    lastChild = xmlNewTextLen(ch, len);
	    xmlAddChild(ctxt->node, lastChild);
	    if (ctxt->node->children != NULL) {
		ctxt->nodelen = len;
		ctxt->nodemem = len + 1;
	    }
	}
    }
}

/**
 * ignorableWhitespace:
 * @ctx: the user data (XML parser context)
 * @ch:  a xmlChar string
 * @len: the number of xmlChar
 *
 * receiving some ignorable whitespaces from the parser.
 * UNUSED: by default the DOM building will use characters
 */
void
ignorableWhitespace(void *ctx ATTRIBUTE_UNUSED, const xmlChar *ch ATTRIBUTE_UNUSED, int len ATTRIBUTE_UNUSED)
{
    /* xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx; */
#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.ignorableWhitespace(%.30s, %d)\n", ch, len);
#endif
}

/**
 * processingInstruction:
 * @ctx: the user data (XML parser context)
 * @target:  the target name
 * @data: the PI data's
 *
 * A processing instruction has been parsed.
 */
void
processingInstruction(void *ctx, const xmlChar *target,
                      const xmlChar *data)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlNodePtr ret;
    xmlNodePtr parent = ctxt->node;

#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.processingInstruction(%s, %s)\n", target, data);
#endif

    ret = xmlNewPI(target, data);
    if (ret == NULL) return;
    parent = ctxt->node;

    if (ctxt->inSubset == 1) {
	xmlAddChild((xmlNodePtr) ctxt->myDoc->intSubset, ret);
	return;
    } else if (ctxt->inSubset == 2) {
	xmlAddChild((xmlNodePtr) ctxt->myDoc->extSubset, ret);
	return;
    }
    if ((ctxt->myDoc->children == NULL) || (parent == NULL)) {
#ifdef DEBUG_SAX_TREE
	    xmlGenericError(xmlGenericErrorContext,
		    "Setting PI %s as root\n", target);
#endif
        xmlAddChild((xmlNodePtr) ctxt->myDoc, (xmlNodePtr) ret);
	return;
    }
    if (parent->type == XML_ELEMENT_NODE) {
#ifdef DEBUG_SAX_TREE
	xmlGenericError(xmlGenericErrorContext,
		"adding PI %s child to %s\n", target, parent->name);
#endif
	xmlAddChild(parent, ret);
    } else {
#ifdef DEBUG_SAX_TREE
	xmlGenericError(xmlGenericErrorContext,
		"adding PI %s sibling to ", target);
	xmlDebugDumpOneNode(stderr, parent, 0);
#endif
	xmlAddSibling(parent, ret);
    }
}

/**
 * globalNamespace:
 * @ctx: the user data (XML parser context)
 * @href:  the namespace associated URN
 * @prefix: the namespace prefix
 *
 * An old global namespace has been parsed.
 */
void
globalNamespace(void *ctx, const xmlChar *href, const xmlChar *prefix)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.globalNamespace(%s, %s)\n", href, prefix);
#endif
    xmlNewGlobalNs(ctxt->myDoc, href, prefix);
}

/**
 * setNamespace:
 * @ctx: the user data (XML parser context)
 * @name:  the namespace prefix
 *
 * Set the current element namespace.
 */

void
setNamespace(void *ctx, const xmlChar *name)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlNsPtr ns;
    xmlNodePtr parent;

#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext, "SAX.setNamespace(%s)\n", name);
#endif
    ns = xmlSearchNs(ctxt->myDoc, ctxt->node, name);
    if (ns == NULL) { /* ctxt->node may not have a parent yet ! */
        if (ctxt->nodeNr >= 2) {
	    parent = ctxt->nodeTab[ctxt->nodeNr - 2];
	    if (parent != NULL)
		ns = xmlSearchNs(ctxt->myDoc, parent, name);
	}
    }
    xmlSetNs(ctxt->node, ns);
}

/**
 * getNamespace:
 * @ctx: the user data (XML parser context)
 *
 * Get the current element namespace.
 *
 * Returns the xmlNsPtr or NULL if none
 */

xmlNsPtr
getNamespace(void *ctx)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlNsPtr ret;

#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext, "SAX.getNamespace()\n");
#endif
    ret = ctxt->node->ns;
    return(ret);
}

/**
 * checkNamespace:
 * @ctx: the user data (XML parser context)
 * @namespace: the namespace to check against
 *
 * Check that the current element namespace is the same as the
 * one read upon parsing.
 *
 * Returns 1 if true 0 otherwise
 */

int
checkNamespace(void *ctx, xmlChar *namespace)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlNodePtr cur = ctxt->node;

#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.checkNamespace(%s)\n", namespace);
#endif

    /*
     * Check that the Name in the ETag is the same as in the STag.
     */
    if (namespace == NULL) {
        if ((cur->ns != NULL) && (cur->ns->prefix != NULL)) {
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt, 
		 "End tags for %s don't hold the namespace %s\n",
		                 cur->name, cur->ns->prefix);
	    ctxt->wellFormed = 0;
	}
    } else {
        if ((cur->ns == NULL) || (cur->ns->prefix == NULL)) {
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt, 
		 "End tags %s holds a prefix %s not used by the open tag\n",
		                 cur->name, namespace);
	    ctxt->wellFormed = 0;
	} else if (!xmlStrEqual(namespace, cur->ns->prefix)) {
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt, 
    "Start and End tags for %s don't use the same namespaces: %s and %s\n",
	                         cur->name, cur->ns->prefix, namespace);
	    ctxt->wellFormed = 0;
	} else
	    return(1);
    }
    return(0);
}

/**
 * namespaceDecl:
 * @ctx: the user data (XML parser context)
 * @href:  the namespace associated URN
 * @prefix: the namespace prefix
 *
 * A namespace has been parsed.
 */
void
namespaceDecl(void *ctx, const xmlChar *href, const xmlChar *prefix)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
#ifdef DEBUG_SAX
    if (prefix == NULL)
	xmlGenericError(xmlGenericErrorContext,
		"SAX.namespaceDecl(%s, NULL)\n", href);
    else
	xmlGenericError(xmlGenericErrorContext,
		"SAX.namespaceDecl(%s, %s)\n", href, prefix);
#endif
    xmlNewNs(ctxt->node, href, prefix);
}

/**
 * comment:
 * @ctx: the user data (XML parser context)
 * @value:  the comment content
 *
 * A comment has been parsed.
 */
void
comment(void *ctx, const xmlChar *value)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlNodePtr ret;
    xmlNodePtr parent = ctxt->node;

#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext, "SAX.comment(%s)\n", value);
#endif
    ret = xmlNewDocComment(ctxt->myDoc, value);
    if (ret == NULL) return;

    if (ctxt->inSubset == 1) {
	xmlAddChild((xmlNodePtr) ctxt->myDoc->intSubset, ret);
	return;
    } else if (ctxt->inSubset == 2) {
	xmlAddChild((xmlNodePtr) ctxt->myDoc->extSubset, ret);
	return;
    }
    if ((ctxt->myDoc->children == NULL) || (parent == NULL)) {
#ifdef DEBUG_SAX_TREE
	    xmlGenericError(xmlGenericErrorContext,
		    "Setting comment as root\n");
#endif
        xmlAddChild((xmlNodePtr) ctxt->myDoc, (xmlNodePtr) ret);
	return;
    }
    if (parent->type == XML_ELEMENT_NODE) {
#ifdef DEBUG_SAX_TREE
	xmlGenericError(xmlGenericErrorContext,
		"adding comment child to %s\n", parent->name);
#endif
	xmlAddChild(parent, ret);
    } else {
#ifdef DEBUG_SAX_TREE
	xmlGenericError(xmlGenericErrorContext,
		"adding comment sibling to ");
	xmlDebugDumpOneNode(stderr, parent, 0);
#endif
	xmlAddSibling(parent, ret);
    }
}

/**
 * cdataBlock:
 * @ctx: the user data (XML parser context)
 * @value:  The pcdata content
 * @len:  the block length
 *
 * called when a pcdata block has been parsed
 */
void
cdataBlock(void *ctx, const xmlChar *value, int len)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlNodePtr ret, lastChild;

#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.pcdata(%.10s, %d)\n", value, len);
#endif
    lastChild = xmlGetLastChild(ctxt->node);
#ifdef DEBUG_SAX_TREE
    xmlGenericError(xmlGenericErrorContext,
	    "add chars to %s \n", ctxt->node->name);
#endif
    if ((lastChild != NULL) &&
        (lastChild->type == XML_CDATA_SECTION_NODE)) {
	xmlTextConcat(lastChild, value, len);
    } else {
	ret = xmlNewCDataBlock(ctxt->myDoc, value, len);
	xmlAddChild(ctxt->node, ret);
    }
}

/**
 * initxmlDefaultSAXHandler:
 * @hdlr:  the SAX handler
 * @warning:  flag if non-zero sets the handler warning procedure
 *
 * Initialize the default XML SAX handler
 */
void
initxmlDefaultSAXHandler(xmlSAXHandler *hdlr, int warning)
{
    if(hdlr->initialized == 1)
	return;

    hdlr->internalSubset = internalSubset;
    hdlr->externalSubset = externalSubset;
    hdlr->isStandalone = isStandalone;
    hdlr->hasInternalSubset = hasInternalSubset;
    hdlr->hasExternalSubset = hasExternalSubset;
    hdlr->resolveEntity = resolveEntity;
    hdlr->getEntity = getEntity;
    hdlr->getParameterEntity = getParameterEntity;
    hdlr->entityDecl = entityDecl;
    hdlr->attributeDecl = attributeDecl;
    hdlr->elementDecl = elementDecl;
    hdlr->notationDecl = notationDecl;
    hdlr->unparsedEntityDecl = unparsedEntityDecl;
    hdlr->setDocumentLocator = setDocumentLocator;
    hdlr->startDocument = startDocument;
    hdlr->endDocument = endDocument;
    hdlr->startElement = startElement;
    hdlr->endElement = endElement;
    hdlr->reference = reference;
    hdlr->characters = characters;
    hdlr->cdataBlock = cdataBlock;
    hdlr->ignorableWhitespace = characters;
    hdlr->processingInstruction = processingInstruction;
    hdlr->comment = comment;
    /* if (xmlGetWarningsDefaultValue == 0) */
    if (warning == 0)
	hdlr->warning = NULL;
    else
	hdlr->warning = xmlParserWarning;
    hdlr->error = xmlParserError;
    hdlr->fatalError = xmlParserError;

    hdlr->initialized = 1;
}

/**
 * xmlDefaultSAXHandlerInit:
 *
 * Initialize the default SAX handler
 */
void
xmlDefaultSAXHandlerInit(void)
{
    initxmlDefaultSAXHandler(&xmlDefaultSAXHandler, xmlGetWarningsDefaultValue);
}

#ifdef LIBXML_HTML_ENABLED

/**
 * inithtmlDefaultSAXHandler:
 * @hdlr:  the SAX handler
 *
 * Initialize the default HTML SAX handler
 */
void
inithtmlDefaultSAXHandler(xmlSAXHandler *hdlr)
{
    if(hdlr->initialized == 1)
	return;

    hdlr->internalSubset = internalSubset;
    hdlr->externalSubset = NULL;
    hdlr->isStandalone = NULL;
    hdlr->hasInternalSubset = NULL;
    hdlr->hasExternalSubset = NULL;
    hdlr->resolveEntity = NULL;
    hdlr->getEntity = getEntity;
    hdlr->getParameterEntity = NULL;
    hdlr->entityDecl = NULL;
    hdlr->attributeDecl = NULL;
    hdlr->elementDecl = NULL;
    hdlr->notationDecl = NULL;
    hdlr->unparsedEntityDecl = NULL;
    hdlr->setDocumentLocator = setDocumentLocator;
    hdlr->startDocument = startDocument;
    hdlr->endDocument = endDocument;
    hdlr->startElement = startElement;
    hdlr->endElement = endElement;
    hdlr->reference = NULL;
    hdlr->characters = characters;
    hdlr->cdataBlock = cdataBlock;
    hdlr->ignorableWhitespace = ignorableWhitespace;
    hdlr->processingInstruction = NULL;
    hdlr->comment = comment;
    hdlr->warning = xmlParserWarning;
    hdlr->error = xmlParserError;
    hdlr->fatalError = xmlParserError;

    hdlr->initialized = 1;
}

/**
 * htmlDefaultSAXHandlerInit:
 *
 * Initialize the default SAX handler
 */
void
htmlDefaultSAXHandlerInit(void)
{
	inithtmlDefaultSAXHandler(&htmlDefaultSAXHandler);
}

#endif /* LIBXML_HTML_ENABLED */

#ifdef LIBXML_DOCB_ENABLED

/**
 * initdocbDefaultSAXHandler:
 * @hdlr:  the SAX handler
 *
 * Initialize the default DocBook SAX handler
 */
void
initdocbDefaultSAXHandler(xmlSAXHandler *hdlr)
{
    if(hdlr->initialized == 1)
	return;

    hdlr->internalSubset = internalSubset;
    hdlr->externalSubset = NULL;
    hdlr->isStandalone = isStandalone;
    hdlr->hasInternalSubset = hasInternalSubset;
    hdlr->hasExternalSubset = hasExternalSubset;
    hdlr->resolveEntity = resolveEntity;
    hdlr->getEntity = getEntity;
    hdlr->getParameterEntity = NULL;
    hdlr->entityDecl = entityDecl;
    hdlr->attributeDecl = NULL;
    hdlr->elementDecl = NULL;
    hdlr->notationDecl = NULL;
    hdlr->unparsedEntityDecl = NULL;
    hdlr->setDocumentLocator = setDocumentLocator;
    hdlr->startDocument = startDocument;
    hdlr->endDocument = endDocument;
    hdlr->startElement = startElement;
    hdlr->endElement = endElement;
    hdlr->reference = reference;
    hdlr->characters = characters;
    hdlr->cdataBlock = NULL;
    hdlr->ignorableWhitespace = ignorableWhitespace;
    hdlr->processingInstruction = NULL;
    hdlr->comment = comment;
    hdlr->warning = xmlParserWarning;
    hdlr->error = xmlParserError;
    hdlr->fatalError = xmlParserError;

    hdlr->initialized = 1;
}

/**
 * docbDefaultSAXHandlerInit:
 *
 * Initialize the default SAX handler
 */
void
docbDefaultSAXHandlerInit(void)
{
    initdocbDefaultSAXHandler(&docbDefaultSAXHandler);
}

#endif /* LIBXML_DOCB_ENABLED */
