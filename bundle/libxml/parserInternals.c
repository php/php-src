/*
 * parserInternals.c : Internal routines (and obsolete ones) needed for the
 *                     XML and HTML parsers.
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */

#define IN_LIBXML
#include "libxml.h"

#if defined(WIN32) && !defined (__CYGWIN__)
#define XML_DIR_SEP '\\'
#else
#define XML_DIR_SEP '/'
#endif

#include <string.h>
#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_ZLIB_H
#include <zlib.h>
#endif

#include <libxml/xmlmemory.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/valid.h>
#include <libxml/entities.h>
#include <libxml/xmlerror.h>
#include <libxml/encoding.h>
#include <libxml/valid.h>
#include <libxml/xmlIO.h>
#include <libxml/uri.h>
#include <libxml/SAX.h>
#ifdef LIBXML_CATALOG_ENABLED
#include <libxml/catalog.h>
#endif
#include <libxml/globals.h>

void xmlUpgradeOldNs(xmlDocPtr doc);

/*
 * Various global defaults for parsing
 */

/**
 * xmlCheckVersion:
 * @version: the include version number
 *
 * check the compiled lib version against the include one.
 * This can warn or immediately kill the application
 */
void
xmlCheckVersion(int version) {
    int myversion = (int) LIBXML_VERSION;

    xmlInitParser();

    if ((myversion / 10000) != (version / 10000)) {
	xmlGenericError(xmlGenericErrorContext, 
		"Fatal: program compiled against libxml %d using libxml %d\n",
		(version / 10000), (myversion / 10000));
	fprintf(stderr, 
		"Fatal: program compiled against libxml %d using libxml %d\n",
		(version / 10000), (myversion / 10000));
    }
    if ((myversion / 100) < (version / 100)) {
	xmlGenericError(xmlGenericErrorContext, 
		"Warning: program compiled against libxml %d using older %d\n",
		(version / 100), (myversion / 100));
    }
}


static const char *xmlFeaturesList[] = {
    "validate",
    "load subset",
    "keep blanks",
    "disable SAX",
    "fetch external entities",
    "substitute entities",
    "gather line info",
    "user data",
    "is html",
    "is standalone",
    "stop parser",
    "document",
    "is well formed",
    "is valid",
    "SAX block",
    "SAX function internalSubset",
    "SAX function isStandalone",
    "SAX function hasInternalSubset",
    "SAX function hasExternalSubset",
    "SAX function resolveEntity",
    "SAX function getEntity",
    "SAX function entityDecl",
    "SAX function notationDecl",
    "SAX function attributeDecl",
    "SAX function elementDecl",
    "SAX function unparsedEntityDecl",
    "SAX function setDocumentLocator",
    "SAX function startDocument",
    "SAX function endDocument",
    "SAX function startElement",
    "SAX function endElement",
    "SAX function reference",
    "SAX function characters",
    "SAX function ignorableWhitespace",
    "SAX function processingInstruction",
    "SAX function comment",
    "SAX function warning",
    "SAX function error",
    "SAX function fatalError",
    "SAX function getParameterEntity",
    "SAX function cdataBlock",
    "SAX function externalSubset",
};

/**
 * xmlGetFeaturesList:
 * @len:  the length of the features name array (input/output)
 * @result:  an array of string to be filled with the features name.
 *
 * Copy at most *@len feature names into the @result array
 *
 * Returns -1 in case or error, or the total number of features,
 *            len is updated with the number of strings copied,
 *            strings must not be deallocated
 */
int
xmlGetFeaturesList(int *len, const char **result) {
    int ret, i;

    ret = sizeof(xmlFeaturesList)/sizeof(xmlFeaturesList[0]);
    if ((len == NULL) || (result == NULL))
	return(ret);
    if ((*len < 0) || (*len >= 1000))
	return(-1);
    if (*len > ret)
	*len = ret;
    for (i = 0;i < *len;i++)
	result[i] = xmlFeaturesList[i];
    return(ret);
}

/**
 * xmlGetFeature:
 * @ctxt:  an XML/HTML parser context
 * @name:  the feature name
 * @result:  location to store the result
 *
 * Read the current value of one feature of this parser instance
 *
 * Returns -1 in case or error, 0 otherwise
 */
int
xmlGetFeature(xmlParserCtxtPtr ctxt, const char *name, void *result) {
    if ((ctxt == NULL) || (name == NULL) || (result == NULL))
	return(-1);

    if (!strcmp(name, "validate")) {
	*((int *) result) = ctxt->validate;
    } else if (!strcmp(name, "keep blanks")) {
	*((int *) result) = ctxt->keepBlanks;
    } else if (!strcmp(name, "disable SAX")) {
	*((int *) result) = ctxt->disableSAX;
    } else if (!strcmp(name, "fetch external entities")) {
	*((int *) result) = ctxt->loadsubset;
    } else if (!strcmp(name, "substitute entities")) {
	*((int *) result) = ctxt->replaceEntities;
    } else if (!strcmp(name, "gather line info")) {
	*((int *) result) = ctxt->record_info;
    } else if (!strcmp(name, "user data")) {
	*((void **)result) = ctxt->userData;
    } else if (!strcmp(name, "is html")) {
	*((int *) result) = ctxt->html;
    } else if (!strcmp(name, "is standalone")) {
	*((int *) result) = ctxt->standalone;
    } else if (!strcmp(name, "document")) {
	*((xmlDocPtr *) result) = ctxt->myDoc;
    } else if (!strcmp(name, "is well formed")) {
	*((int *) result) = ctxt->wellFormed;
    } else if (!strcmp(name, "is valid")) {
	*((int *) result) = ctxt->valid;
    } else if (!strcmp(name, "SAX block")) {
	*((xmlSAXHandlerPtr *) result) = ctxt->sax;
    } else if (!strcmp(name, "SAX function internalSubset")) {
        *((internalSubsetSAXFunc *) result) = ctxt->sax->internalSubset;
    } else if (!strcmp(name, "SAX function isStandalone")) {
        *((isStandaloneSAXFunc *) result) = ctxt->sax->isStandalone;
    } else if (!strcmp(name, "SAX function hasInternalSubset")) {
        *((hasInternalSubsetSAXFunc *) result) = ctxt->sax->hasInternalSubset;
    } else if (!strcmp(name, "SAX function hasExternalSubset")) {
        *((hasExternalSubsetSAXFunc *) result) = ctxt->sax->hasExternalSubset;
    } else if (!strcmp(name, "SAX function resolveEntity")) {
        *((resolveEntitySAXFunc *) result) = ctxt->sax->resolveEntity;
    } else if (!strcmp(name, "SAX function getEntity")) {
        *((getEntitySAXFunc *) result) = ctxt->sax->getEntity;
    } else if (!strcmp(name, "SAX function entityDecl")) {
        *((entityDeclSAXFunc *) result) = ctxt->sax->entityDecl;
    } else if (!strcmp(name, "SAX function notationDecl")) {
        *((notationDeclSAXFunc *) result) = ctxt->sax->notationDecl;
    } else if (!strcmp(name, "SAX function attributeDecl")) {
        *((attributeDeclSAXFunc *) result) = ctxt->sax->attributeDecl;
    } else if (!strcmp(name, "SAX function elementDecl")) {
        *((elementDeclSAXFunc *) result) = ctxt->sax->elementDecl;
    } else if (!strcmp(name, "SAX function unparsedEntityDecl")) {
        *((unparsedEntityDeclSAXFunc *) result) = ctxt->sax->unparsedEntityDecl;
    } else if (!strcmp(name, "SAX function setDocumentLocator")) {
        *((setDocumentLocatorSAXFunc *) result) = ctxt->sax->setDocumentLocator;
    } else if (!strcmp(name, "SAX function startDocument")) {
        *((startDocumentSAXFunc *) result) = ctxt->sax->startDocument;
    } else if (!strcmp(name, "SAX function endDocument")) {
        *((endDocumentSAXFunc *) result) = ctxt->sax->endDocument;
    } else if (!strcmp(name, "SAX function startElement")) {
        *((startElementSAXFunc *) result) = ctxt->sax->startElement;
    } else if (!strcmp(name, "SAX function endElement")) {
        *((endElementSAXFunc *) result) = ctxt->sax->endElement;
    } else if (!strcmp(name, "SAX function reference")) {
        *((referenceSAXFunc *) result) = ctxt->sax->reference;
    } else if (!strcmp(name, "SAX function characters")) {
        *((charactersSAXFunc *) result) = ctxt->sax->characters;
    } else if (!strcmp(name, "SAX function ignorableWhitespace")) {
        *((ignorableWhitespaceSAXFunc *) result) = ctxt->sax->ignorableWhitespace;
    } else if (!strcmp(name, "SAX function processingInstruction")) {
        *((processingInstructionSAXFunc *) result) = ctxt->sax->processingInstruction;
    } else if (!strcmp(name, "SAX function comment")) {
        *((commentSAXFunc *) result) = ctxt->sax->comment;
    } else if (!strcmp(name, "SAX function warning")) {
        *((warningSAXFunc *) result) = ctxt->sax->warning;
    } else if (!strcmp(name, "SAX function error")) {
        *((errorSAXFunc *) result) = ctxt->sax->error;
    } else if (!strcmp(name, "SAX function fatalError")) {
        *((fatalErrorSAXFunc *) result) = ctxt->sax->fatalError;
    } else if (!strcmp(name, "SAX function getParameterEntity")) {
        *((getParameterEntitySAXFunc *) result) = ctxt->sax->getParameterEntity;
    } else if (!strcmp(name, "SAX function cdataBlock")) {
        *((cdataBlockSAXFunc *) result) = ctxt->sax->cdataBlock;
    } else if (!strcmp(name, "SAX function externalSubset")) {
        *((externalSubsetSAXFunc *) result) = ctxt->sax->externalSubset;
    } else {
	return(-1);
    }
    return(0);
}

/**
 * xmlSetFeature:
 * @ctxt:  an XML/HTML parser context
 * @name:  the feature name
 * @value:  pointer to the location of the new value
 *
 * Change the current value of one feature of this parser instance
 *
 * Returns -1 in case or error, 0 otherwise
 */
int	
xmlSetFeature(xmlParserCtxtPtr ctxt, const char *name, void *value) {
    if ((ctxt == NULL) || (name == NULL) || (value == NULL))
	return(-1);

    if (!strcmp(name, "validate")) {
	int newvalidate = *((int *) value);
	if ((!ctxt->validate) && (newvalidate != 0)) {
	    if (ctxt->vctxt.warning == NULL)
		ctxt->vctxt.warning = xmlParserValidityWarning;
	    if (ctxt->vctxt.error == NULL)
		ctxt->vctxt.error = xmlParserValidityError;
	    ctxt->vctxt.nodeMax = 0;
	}
        ctxt->validate = newvalidate;
    } else if (!strcmp(name, "keep blanks")) {
        ctxt->keepBlanks = *((int *) value);
    } else if (!strcmp(name, "disable SAX")) {
        ctxt->disableSAX = *((int *) value);
    } else if (!strcmp(name, "fetch external entities")) {
	ctxt->loadsubset = *((int *) value);
    } else if (!strcmp(name, "substitute entities")) {
        ctxt->replaceEntities = *((int *) value);
    } else if (!strcmp(name, "gather line info")) {
        ctxt->record_info = *((int *) value);
    } else if (!strcmp(name, "user data")) {
        ctxt->userData = *((void **)value);
    } else if (!strcmp(name, "is html")) {
        ctxt->html = *((int *) value);
    } else if (!strcmp(name, "is standalone")) {
        ctxt->standalone = *((int *) value);
    } else if (!strcmp(name, "document")) {
        ctxt->myDoc = *((xmlDocPtr *) value);
    } else if (!strcmp(name, "is well formed")) {
        ctxt->wellFormed = *((int *) value);
    } else if (!strcmp(name, "is valid")) {
        ctxt->valid = *((int *) value);
    } else if (!strcmp(name, "SAX block")) {
        ctxt->sax = *((xmlSAXHandlerPtr *) value);
    } else if (!strcmp(name, "SAX function internalSubset")) {
        ctxt->sax->internalSubset = *((internalSubsetSAXFunc *) value);
    } else if (!strcmp(name, "SAX function isStandalone")) {
        ctxt->sax->isStandalone = *((isStandaloneSAXFunc *) value);
    } else if (!strcmp(name, "SAX function hasInternalSubset")) {
        ctxt->sax->hasInternalSubset = *((hasInternalSubsetSAXFunc *) value);
    } else if (!strcmp(name, "SAX function hasExternalSubset")) {
        ctxt->sax->hasExternalSubset = *((hasExternalSubsetSAXFunc *) value);
    } else if (!strcmp(name, "SAX function resolveEntity")) {
        ctxt->sax->resolveEntity = *((resolveEntitySAXFunc *) value);
    } else if (!strcmp(name, "SAX function getEntity")) {
        ctxt->sax->getEntity = *((getEntitySAXFunc *) value);
    } else if (!strcmp(name, "SAX function entityDecl")) {
        ctxt->sax->entityDecl = *((entityDeclSAXFunc *) value);
    } else if (!strcmp(name, "SAX function notationDecl")) {
        ctxt->sax->notationDecl = *((notationDeclSAXFunc *) value);
    } else if (!strcmp(name, "SAX function attributeDecl")) {
        ctxt->sax->attributeDecl = *((attributeDeclSAXFunc *) value);
    } else if (!strcmp(name, "SAX function elementDecl")) {
        ctxt->sax->elementDecl = *((elementDeclSAXFunc *) value);
    } else if (!strcmp(name, "SAX function unparsedEntityDecl")) {
        ctxt->sax->unparsedEntityDecl = *((unparsedEntityDeclSAXFunc *) value);
    } else if (!strcmp(name, "SAX function setDocumentLocator")) {
        ctxt->sax->setDocumentLocator = *((setDocumentLocatorSAXFunc *) value);
    } else if (!strcmp(name, "SAX function startDocument")) {
        ctxt->sax->startDocument = *((startDocumentSAXFunc *) value);
    } else if (!strcmp(name, "SAX function endDocument")) {
        ctxt->sax->endDocument = *((endDocumentSAXFunc *) value);
    } else if (!strcmp(name, "SAX function startElement")) {
        ctxt->sax->startElement = *((startElementSAXFunc *) value);
    } else if (!strcmp(name, "SAX function endElement")) {
        ctxt->sax->endElement = *((endElementSAXFunc *) value);
    } else if (!strcmp(name, "SAX function reference")) {
        ctxt->sax->reference = *((referenceSAXFunc *) value);
    } else if (!strcmp(name, "SAX function characters")) {
        ctxt->sax->characters = *((charactersSAXFunc *) value);
    } else if (!strcmp(name, "SAX function ignorableWhitespace")) {
        ctxt->sax->ignorableWhitespace = *((ignorableWhitespaceSAXFunc *) value);
    } else if (!strcmp(name, "SAX function processingInstruction")) {
        ctxt->sax->processingInstruction = *((processingInstructionSAXFunc *) value);
    } else if (!strcmp(name, "SAX function comment")) {
        ctxt->sax->comment = *((commentSAXFunc *) value);
    } else if (!strcmp(name, "SAX function warning")) {
        ctxt->sax->warning = *((warningSAXFunc *) value);
    } else if (!strcmp(name, "SAX function error")) {
        ctxt->sax->error = *((errorSAXFunc *) value);
    } else if (!strcmp(name, "SAX function fatalError")) {
        ctxt->sax->fatalError = *((fatalErrorSAXFunc *) value);
    } else if (!strcmp(name, "SAX function getParameterEntity")) {
        ctxt->sax->getParameterEntity = *((getParameterEntitySAXFunc *) value);
    } else if (!strcmp(name, "SAX function cdataBlock")) {
        ctxt->sax->cdataBlock = *((cdataBlockSAXFunc *) value);
    } else if (!strcmp(name, "SAX function externalSubset")) {
        ctxt->sax->externalSubset = *((externalSubsetSAXFunc *) value);
    } else {
	return(-1);
    }
    return(0);
}

/************************************************************************
 *									*
 * 		Some functions to avoid too large macros		*
 *									*
 ************************************************************************/

/**
 * xmlIsChar:
 * @c:  an unicode character (int)
 *
 * Check whether the character is allowed by the production
 * [2] Char ::= #x9 | #xA | #xD | [#x20-#xD7FF] | [#xE000-#xFFFD]
 *                  | [#x10000-#x10FFFF]
 * any Unicode character, excluding the surrogate blocks, FFFE, and FFFF.
 * Also available as a macro IS_CHAR()
 *
 * Returns 0 if not, non-zero otherwise
 */
int
xmlIsChar(int c) {
    return(
     ((c) == 0x09) || ((c) == 0x0A) || ((c) == 0x0D) ||
     (((c) >= 0x20) && ((c) <= 0xD7FF)) ||
     (((c) >= 0xE000) && ((c) <= 0xFFFD)) ||
     (((c) >= 0x10000) && ((c) <= 0x10FFFF)));
}

/**
 * xmlIsBlank:
 * @c:  an unicode character (int)
 *
 * Check whether the character is allowed by the production
 * [3] S ::= (#x20 | #x9 | #xD | #xA)+
 * Also available as a macro IS_BLANK()
 *
 * Returns 0 if not, non-zero otherwise
 */
int
xmlIsBlank(int c) {
    return(((c) == 0x20) || ((c) == 0x09) || ((c) == 0xA) || ((c) == 0x0D));
}

static int xmlBaseArray[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x0000 - 0x000F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x0010 - 0x001F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x0020 - 0x002F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x0030 - 0x003F */
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x0040 - 0x004F */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, /* 0x0050 - 0x005F */
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x0060 - 0x006F */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, /* 0x0070 - 0x007F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x0080 - 0x008F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x0090 - 0x009F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x00A0 - 0x00AF */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x00B0 - 0x00BF */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x00C0 - 0x00CF */
  1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x00D0 - 0x00DF */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x00E0 - 0x00EF */
  1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x00F0 - 0x00FF */
};

/**
 * xmlIsBaseChar:
 * @c:  an unicode character (int)
 *
 * Check whether the character is allowed by the production
 * [85] BaseChar ::= ... long list see REC ...
 *
 * VI is your friend !
 * :1,$ s/\[#x\([0-9A-Z]*\)-#x\([0-9A-Z]*\)\]/     (((c) >= 0x\1) \&\& ((c) <= 0x\2)) ||/
 * and 
 * :1,$ s/#x\([0-9A-Z]*\)/     ((c) == 0x\1) ||/
 *
 * Returns 0 if not, non-zero otherwise
 */
int
xmlIsBaseChar(int c) {
    return(
      (((c) < 0x0100) ? xmlBaseArray[c] :
      (	/* accelerator */
      (((c) >= 0x0100) && ((c) <= 0x0131)) ||
      (((c) >= 0x0134) && ((c) <= 0x013E)) ||
      (((c) >= 0x0141) && ((c) <= 0x0148)) ||
      (((c) >= 0x014A) && ((c) <= 0x017E)) ||
      (((c) >= 0x0180) && ((c) <= 0x01C3)) ||
      (((c) >= 0x01CD) && ((c) <= 0x01F0)) ||
      (((c) >= 0x01F4) && ((c) <= 0x01F5)) ||
      (((c) >= 0x01FA) && ((c) <= 0x0217)) ||
      (((c) >= 0x0250) && ((c) <= 0x02A8)) ||
      (((c) >= 0x02BB) && ((c) <= 0x02C1)) ||
      ((c) == 0x0386) ||
      (((c) >= 0x0388) && ((c) <= 0x038A)) ||
      ((c) == 0x038C) ||
      (((c) >= 0x038E) && ((c) <= 0x03A1)) ||
      (((c) >= 0x03A3) && ((c) <= 0x03CE)) ||
      (((c) >= 0x03D0) && ((c) <= 0x03D6)) ||
      ((c) == 0x03DA) ||
      ((c) == 0x03DC) ||
      ((c) == 0x03DE) ||
      ((c) == 0x03E0) ||
      (((c) >= 0x03E2) && ((c) <= 0x03F3)) ||
      (((c) >= 0x0401) && ((c) <= 0x040C)) ||
      (((c) >= 0x040E) && ((c) <= 0x044F)) ||
      (((c) >= 0x0451) && ((c) <= 0x045C)) ||
      (((c) >= 0x045E) && ((c) <= 0x0481)) ||
      (((c) >= 0x0490) && ((c) <= 0x04C4)) ||
      (((c) >= 0x04C7) && ((c) <= 0x04C8)) ||
      (((c) >= 0x04CB) && ((c) <= 0x04CC)) ||
      (((c) >= 0x04D0) && ((c) <= 0x04EB)) ||
      (((c) >= 0x04EE) && ((c) <= 0x04F5)) ||
      (((c) >= 0x04F8) && ((c) <= 0x04F9)) ||
      (((c) >= 0x0531) && ((c) <= 0x0556)) ||
      ((c) == 0x0559) ||
      (((c) >= 0x0561) && ((c) <= 0x0586)) ||
      (((c) >= 0x05D0) && ((c) <= 0x05EA)) ||
      (((c) >= 0x05F0) && ((c) <= 0x05F2)) ||
      (((c) >= 0x0621) && ((c) <= 0x063A)) ||
      (((c) >= 0x0641) && ((c) <= 0x064A)) ||
      (((c) >= 0x0671) && ((c) <= 0x06B7)) ||
      (((c) >= 0x06BA) && ((c) <= 0x06BE)) ||
      (((c) >= 0x06C0) && ((c) <= 0x06CE)) ||
      (((c) >= 0x06D0) && ((c) <= 0x06D3)) ||
      ((c) == 0x06D5) ||
      (((c) >= 0x06E5) && ((c) <= 0x06E6)) ||
     (((c) >= 0x905) && (	/* accelerator */
      (((c) >= 0x0905) && ((c) <= 0x0939)) ||
      ((c) == 0x093D) ||
      (((c) >= 0x0958) && ((c) <= 0x0961)) ||
      (((c) >= 0x0985) && ((c) <= 0x098C)) ||
      (((c) >= 0x098F) && ((c) <= 0x0990)) ||
      (((c) >= 0x0993) && ((c) <= 0x09A8)) ||
      (((c) >= 0x09AA) && ((c) <= 0x09B0)) ||
      ((c) == 0x09B2) ||
      (((c) >= 0x09B6) && ((c) <= 0x09B9)) ||
      (((c) >= 0x09DC) && ((c) <= 0x09DD)) ||
      (((c) >= 0x09DF) && ((c) <= 0x09E1)) ||
      (((c) >= 0x09F0) && ((c) <= 0x09F1)) ||
      (((c) >= 0x0A05) && ((c) <= 0x0A0A)) ||
      (((c) >= 0x0A0F) && ((c) <= 0x0A10)) ||
      (((c) >= 0x0A13) && ((c) <= 0x0A28)) ||
      (((c) >= 0x0A2A) && ((c) <= 0x0A30)) ||
      (((c) >= 0x0A32) && ((c) <= 0x0A33)) ||
      (((c) >= 0x0A35) && ((c) <= 0x0A36)) ||
      (((c) >= 0x0A38) && ((c) <= 0x0A39)) ||
      (((c) >= 0x0A59) && ((c) <= 0x0A5C)) ||
      ((c) == 0x0A5E) ||
      (((c) >= 0x0A72) && ((c) <= 0x0A74)) ||
      (((c) >= 0x0A85) && ((c) <= 0x0A8B)) ||
      ((c) == 0x0A8D) ||
      (((c) >= 0x0A8F) && ((c) <= 0x0A91)) ||
      (((c) >= 0x0A93) && ((c) <= 0x0AA8)) ||
      (((c) >= 0x0AAA) && ((c) <= 0x0AB0)) ||
      (((c) >= 0x0AB2) && ((c) <= 0x0AB3)) ||
      (((c) >= 0x0AB5) && ((c) <= 0x0AB9)) ||
      ((c) == 0x0ABD) ||
      ((c) == 0x0AE0) ||
      (((c) >= 0x0B05) && ((c) <= 0x0B0C)) ||
      (((c) >= 0x0B0F) && ((c) <= 0x0B10)) ||
      (((c) >= 0x0B13) && ((c) <= 0x0B28)) ||
      (((c) >= 0x0B2A) && ((c) <= 0x0B30)) ||
      (((c) >= 0x0B32) && ((c) <= 0x0B33)) ||
      (((c) >= 0x0B36) && ((c) <= 0x0B39)) ||
      ((c) == 0x0B3D) ||
      (((c) >= 0x0B5C) && ((c) <= 0x0B5D)) ||
      (((c) >= 0x0B5F) && ((c) <= 0x0B61)) ||
      (((c) >= 0x0B85) && ((c) <= 0x0B8A)) ||
      (((c) >= 0x0B8E) && ((c) <= 0x0B90)) ||
      (((c) >= 0x0B92) && ((c) <= 0x0B95)) ||
      (((c) >= 0x0B99) && ((c) <= 0x0B9A)) ||
      ((c) == 0x0B9C) ||
      (((c) >= 0x0B9E) && ((c) <= 0x0B9F)) ||
      (((c) >= 0x0BA3) && ((c) <= 0x0BA4)) ||
      (((c) >= 0x0BA8) && ((c) <= 0x0BAA)) ||
      (((c) >= 0x0BAE) && ((c) <= 0x0BB5)) ||
      (((c) >= 0x0BB7) && ((c) <= 0x0BB9)) ||
      (((c) >= 0x0C05) && ((c) <= 0x0C0C)) ||
      (((c) >= 0x0C0E) && ((c) <= 0x0C10)) ||
      (((c) >= 0x0C12) && ((c) <= 0x0C28)) ||
      (((c) >= 0x0C2A) && ((c) <= 0x0C33)) ||
      (((c) >= 0x0C35) && ((c) <= 0x0C39)) ||
      (((c) >= 0x0C60) && ((c) <= 0x0C61)) ||
      (((c) >= 0x0C85) && ((c) <= 0x0C8C)) ||
      (((c) >= 0x0C8E) && ((c) <= 0x0C90)) ||
      (((c) >= 0x0C92) && ((c) <= 0x0CA8)) ||
      (((c) >= 0x0CAA) && ((c) <= 0x0CB3)) ||
      (((c) >= 0x0CB5) && ((c) <= 0x0CB9)) ||
      ((c) == 0x0CDE) ||
      (((c) >= 0x0CE0) && ((c) <= 0x0CE1)) ||
      (((c) >= 0x0D05) && ((c) <= 0x0D0C)) ||
      (((c) >= 0x0D0E) && ((c) <= 0x0D10)) ||
      (((c) >= 0x0D12) && ((c) <= 0x0D28)) ||
      (((c) >= 0x0D2A) && ((c) <= 0x0D39)) ||
      (((c) >= 0x0D60) && ((c) <= 0x0D61)) ||
      (((c) >= 0x0E01) && ((c) <= 0x0E2E)) ||
      ((c) == 0x0E30) ||
      (((c) >= 0x0E32) && ((c) <= 0x0E33)) ||
      (((c) >= 0x0E40) && ((c) <= 0x0E45)) ||
      (((c) >= 0x0E81) && ((c) <= 0x0E82)) ||
      ((c) == 0x0E84) ||
      (((c) >= 0x0E87) && ((c) <= 0x0E88)) ||
      ((c) == 0x0E8A) ||
      ((c) == 0x0E8D) ||
      (((c) >= 0x0E94) && ((c) <= 0x0E97)) ||
      (((c) >= 0x0E99) && ((c) <= 0x0E9F)) ||
      (((c) >= 0x0EA1) && ((c) <= 0x0EA3)) ||
      ((c) == 0x0EA5) ||
      ((c) == 0x0EA7) ||
      (((c) >= 0x0EAA) && ((c) <= 0x0EAB)) ||
      (((c) >= 0x0EAD) && ((c) <= 0x0EAE)) ||
      ((c) == 0x0EB0) ||
      (((c) >= 0x0EB2) && ((c) <= 0x0EB3)) ||
      ((c) == 0x0EBD) ||
      (((c) >= 0x0EC0) && ((c) <= 0x0EC4)) ||
      (((c) >= 0x0F40) && ((c) <= 0x0F47)) ||
      (((c) >= 0x0F49) && ((c) <= 0x0F69)) ||
     (((c) >= 0x10A0) && (	/* accelerator */
      (((c) >= 0x10A0) && ((c) <= 0x10C5)) ||
      (((c) >= 0x10D0) && ((c) <= 0x10F6)) ||
      ((c) == 0x1100) ||
      (((c) >= 0x1102) && ((c) <= 0x1103)) ||
      (((c) >= 0x1105) && ((c) <= 0x1107)) ||
      ((c) == 0x1109) ||
      (((c) >= 0x110B) && ((c) <= 0x110C)) ||
      (((c) >= 0x110E) && ((c) <= 0x1112)) ||
      ((c) == 0x113C) ||
      ((c) == 0x113E) ||
      ((c) == 0x1140) ||
      ((c) == 0x114C) ||
      ((c) == 0x114E) ||
      ((c) == 0x1150) ||
      (((c) >= 0x1154) && ((c) <= 0x1155)) ||
      ((c) == 0x1159) ||
      (((c) >= 0x115F) && ((c) <= 0x1161)) ||
      ((c) == 0x1163) ||
      ((c) == 0x1165) ||
      ((c) == 0x1167) ||
      ((c) == 0x1169) ||
      (((c) >= 0x116D) && ((c) <= 0x116E)) ||
      (((c) >= 0x1172) && ((c) <= 0x1173)) ||
      ((c) == 0x1175) ||
      ((c) == 0x119E) ||
      ((c) == 0x11A8) ||
      ((c) == 0x11AB) ||
      (((c) >= 0x11AE) && ((c) <= 0x11AF)) ||
      (((c) >= 0x11B7) && ((c) <= 0x11B8)) ||
      ((c) == 0x11BA) ||
      (((c) >= 0x11BC) && ((c) <= 0x11C2)) ||
      ((c) == 0x11EB) ||
      ((c) == 0x11F0) ||
      ((c) == 0x11F9) ||
      (((c) >= 0x1E00) && ((c) <= 0x1E9B)) ||
      (((c) >= 0x1EA0) && ((c) <= 0x1EF9)) ||
      (((c) >= 0x1F00) && ((c) <= 0x1F15)) ||
      (((c) >= 0x1F18) && ((c) <= 0x1F1D)) ||
      (((c) >= 0x1F20) && ((c) <= 0x1F45)) ||
      (((c) >= 0x1F48) && ((c) <= 0x1F4D)) ||
      (((c) >= 0x1F50) && ((c) <= 0x1F57)) ||
      ((c) == 0x1F59) ||
      ((c) == 0x1F5B) ||
      ((c) == 0x1F5D) ||
      (((c) >= 0x1F5F) && ((c) <= 0x1F7D)) ||
      (((c) >= 0x1F80) && ((c) <= 0x1FB4)) ||
      (((c) >= 0x1FB6) && ((c) <= 0x1FBC)) ||
      ((c) == 0x1FBE) ||
      (((c) >= 0x1FC2) && ((c) <= 0x1FC4)) ||
      (((c) >= 0x1FC6) && ((c) <= 0x1FCC)) ||
      (((c) >= 0x1FD0) && ((c) <= 0x1FD3)) ||
      (((c) >= 0x1FD6) && ((c) <= 0x1FDB)) ||
      (((c) >= 0x1FE0) && ((c) <= 0x1FEC)) ||
      (((c) >= 0x1FF2) && ((c) <= 0x1FF4)) ||
      (((c) >= 0x1FF6) && ((c) <= 0x1FFC)) ||
      ((c) == 0x2126) ||
      (((c) >= 0x212A) && ((c) <= 0x212B)) ||
      ((c) == 0x212E) ||
      (((c) >= 0x2180) && ((c) <= 0x2182)) ||
      (((c) >= 0x3041) && ((c) <= 0x3094)) ||
      (((c) >= 0x30A1) && ((c) <= 0x30FA)) ||
      (((c) >= 0x3105) && ((c) <= 0x312C)) ||
      (((c) >= 0xAC00) && ((c) <= 0xD7A3))) /* accelerators */ ))))));
}

/**
 * xmlIsDigit:
 * @c:  an unicode character (int)
 *
 * Check whether the character is allowed by the production
 * [88] Digit ::= ... long list see REC ...
 *
 * Returns 0 if not, non-zero otherwise
 */
int
xmlIsDigit(int c) {
    return(
      (((c) >= 0x0030) && ((c) <= 0x0039)) ||
     (((c) >= 0x660) && (	/* accelerator */
      (((c) >= 0x0660) && ((c) <= 0x0669)) ||
      (((c) >= 0x06F0) && ((c) <= 0x06F9)) ||
      (((c) >= 0x0966) && ((c) <= 0x096F)) ||
      (((c) >= 0x09E6) && ((c) <= 0x09EF)) ||
      (((c) >= 0x0A66) && ((c) <= 0x0A6F)) ||
      (((c) >= 0x0AE6) && ((c) <= 0x0AEF)) ||
      (((c) >= 0x0B66) && ((c) <= 0x0B6F)) ||
      (((c) >= 0x0BE7) && ((c) <= 0x0BEF)) ||
      (((c) >= 0x0C66) && ((c) <= 0x0C6F)) ||
      (((c) >= 0x0CE6) && ((c) <= 0x0CEF)) ||
      (((c) >= 0x0D66) && ((c) <= 0x0D6F)) ||
      (((c) >= 0x0E50) && ((c) <= 0x0E59)) ||
      (((c) >= 0x0ED0) && ((c) <= 0x0ED9)) ||
      (((c) >= 0x0F20) && ((c) <= 0x0F29))) /* accelerator */ ));
}

/**
 * xmlIsCombining:
 * @c:  an unicode character (int)
 *
 * Check whether the character is allowed by the production
 * [87] CombiningChar ::= ... long list see REC ...
 *
 * Returns 0 if not, non-zero otherwise
 */
int
xmlIsCombining(int c) {
    return(
     (((c) >= 0x300) && (	/* accelerator */
      (((c) >= 0x0300) && ((c) <= 0x0345)) ||
      (((c) >= 0x0360) && ((c) <= 0x0361)) ||
      (((c) >= 0x0483) && ((c) <= 0x0486)) ||
      (((c) >= 0x0591) && ((c) <= 0x05A1)) ||
      (((c) >= 0x05A3) && ((c) <= 0x05B9)) ||
      (((c) >= 0x05BB) && ((c) <= 0x05BD)) ||
      ((c) == 0x05BF) ||
      (((c) >= 0x05C1) && ((c) <= 0x05C2)) ||
      ((c) == 0x05C4) ||
      (((c) >= 0x064B) && ((c) <= 0x0652)) ||
      ((c) == 0x0670) ||
      (((c) >= 0x06D6) && ((c) <= 0x06DC)) ||
      (((c) >= 0x06DD) && ((c) <= 0x06DF)) ||
      (((c) >= 0x06E0) && ((c) <= 0x06E4)) ||
      (((c) >= 0x06E7) && ((c) <= 0x06E8)) ||
      (((c) >= 0x06EA) && ((c) <= 0x06ED)) ||
     (((c) >= 0x0901) && (	/* accelerator */
      (((c) >= 0x0901) && ((c) <= 0x0903)) ||
      ((c) == 0x093C) ||
      (((c) >= 0x093E) && ((c) <= 0x094C)) ||
      ((c) == 0x094D) ||
      (((c) >= 0x0951) && ((c) <= 0x0954)) ||
      (((c) >= 0x0962) && ((c) <= 0x0963)) ||
      (((c) >= 0x0981) && ((c) <= 0x0983)) ||
      ((c) == 0x09BC) ||
      ((c) == 0x09BE) ||
      ((c) == 0x09BF) ||
      (((c) >= 0x09C0) && ((c) <= 0x09C4)) ||
      (((c) >= 0x09C7) && ((c) <= 0x09C8)) ||
      (((c) >= 0x09CB) && ((c) <= 0x09CD)) ||
      ((c) == 0x09D7) ||
      (((c) >= 0x09E2) && ((c) <= 0x09E3)) ||
     (((c) >= 0x0A02) && (	/* accelerator */
      ((c) == 0x0A02) ||
      ((c) == 0x0A3C) ||
      ((c) == 0x0A3E) ||
      ((c) == 0x0A3F) ||
      (((c) >= 0x0A40) && ((c) <= 0x0A42)) ||
      (((c) >= 0x0A47) && ((c) <= 0x0A48)) ||
      (((c) >= 0x0A4B) && ((c) <= 0x0A4D)) ||
      (((c) >= 0x0A70) && ((c) <= 0x0A71)) ||
      (((c) >= 0x0A81) && ((c) <= 0x0A83)) ||
      ((c) == 0x0ABC) ||
      (((c) >= 0x0ABE) && ((c) <= 0x0AC5)) ||
      (((c) >= 0x0AC7) && ((c) <= 0x0AC9)) ||
      (((c) >= 0x0ACB) && ((c) <= 0x0ACD)) ||
      (((c) >= 0x0B01) && ((c) <= 0x0B03)) ||
      ((c) == 0x0B3C) ||
      (((c) >= 0x0B3E) && ((c) <= 0x0B43)) ||
      (((c) >= 0x0B47) && ((c) <= 0x0B48)) ||
      (((c) >= 0x0B4B) && ((c) <= 0x0B4D)) ||
      (((c) >= 0x0B56) && ((c) <= 0x0B57)) ||
      (((c) >= 0x0B82) && ((c) <= 0x0B83)) ||
      (((c) >= 0x0BBE) && ((c) <= 0x0BC2)) ||
      (((c) >= 0x0BC6) && ((c) <= 0x0BC8)) ||
      (((c) >= 0x0BCA) && ((c) <= 0x0BCD)) ||
      ((c) == 0x0BD7) ||
      (((c) >= 0x0C01) && ((c) <= 0x0C03)) ||
      (((c) >= 0x0C3E) && ((c) <= 0x0C44)) ||
      (((c) >= 0x0C46) && ((c) <= 0x0C48)) ||
      (((c) >= 0x0C4A) && ((c) <= 0x0C4D)) ||
      (((c) >= 0x0C55) && ((c) <= 0x0C56)) ||
      (((c) >= 0x0C82) && ((c) <= 0x0C83)) ||
      (((c) >= 0x0CBE) && ((c) <= 0x0CC4)) ||
      (((c) >= 0x0CC6) && ((c) <= 0x0CC8)) ||
      (((c) >= 0x0CCA) && ((c) <= 0x0CCD)) ||
      (((c) >= 0x0CD5) && ((c) <= 0x0CD6)) ||
      (((c) >= 0x0D02) && ((c) <= 0x0D03)) ||
      (((c) >= 0x0D3E) && ((c) <= 0x0D43)) ||
      (((c) >= 0x0D46) && ((c) <= 0x0D48)) ||
      (((c) >= 0x0D4A) && ((c) <= 0x0D4D)) ||
      ((c) == 0x0D57) ||
     (((c) >= 0x0E31) && (	/* accelerator */
      ((c) == 0x0E31) ||
      (((c) >= 0x0E34) && ((c) <= 0x0E3A)) ||
      (((c) >= 0x0E47) && ((c) <= 0x0E4E)) ||
      ((c) == 0x0EB1) ||
      (((c) >= 0x0EB4) && ((c) <= 0x0EB9)) ||
      (((c) >= 0x0EBB) && ((c) <= 0x0EBC)) ||
      (((c) >= 0x0EC8) && ((c) <= 0x0ECD)) ||
      (((c) >= 0x0F18) && ((c) <= 0x0F19)) ||
      ((c) == 0x0F35) ||
      ((c) == 0x0F37) ||
      ((c) == 0x0F39) ||
      ((c) == 0x0F3E) ||
      ((c) == 0x0F3F) ||
      (((c) >= 0x0F71) && ((c) <= 0x0F84)) ||
      (((c) >= 0x0F86) && ((c) <= 0x0F8B)) ||
      (((c) >= 0x0F90) && ((c) <= 0x0F95)) ||
      ((c) == 0x0F97) ||
      (((c) >= 0x0F99) && ((c) <= 0x0FAD)) ||
      (((c) >= 0x0FB1) && ((c) <= 0x0FB7)) ||
      ((c) == 0x0FB9) ||
      (((c) >= 0x20D0) && ((c) <= 0x20DC)) ||
      ((c) == 0x20E1) ||
      (((c) >= 0x302A) && ((c) <= 0x302F)) ||
      ((c) == 0x3099) ||
      ((c) == 0x309A))))))))));
}

/**
 * xmlIsExtender:
 * @c:  an unicode character (int)
 *
 * Check whether the character is allowed by the production
 * [89] Extender ::= #x00B7 | #x02D0 | #x02D1 | #x0387 | #x0640 |
 *                   #x0E46 | #x0EC6 | #x3005 | [#x3031-#x3035] |
 *                   [#x309D-#x309E] | [#x30FC-#x30FE]
 *
 * Returns 0 if not, non-zero otherwise
 */
int
xmlIsExtender(int c) {
    switch (c) {
    case 0x00B7: case 0x02D0: case 0x02D1: case 0x0387:
    case 0x0640: case 0x0E46: case 0x0EC6: case 0x3005:
    case 0x3031: case 0x3032: case 0x3033: case 0x3034:
    case 0x3035: case 0x309D: case 0x309E: case 0x30FC:
    case 0x30FD: case 0x30FE:
	return 1;
    default:
	return 0;
    }
}

/**
 * xmlIsIdeographic:
 * @c:  an unicode character (int)
 *
 * Check whether the character is allowed by the production
 * [86] Ideographic ::= [#x4E00-#x9FA5] | #x3007 | [#x3021-#x3029]
 *
 * Returns 0 if not, non-zero otherwise
 */
int
xmlIsIdeographic(int c) {
    return(((c) < 0x0100) ? 0 :
     (((c) >= 0x4e00) && ((c) <= 0x9fa5)) ||
     (((c) >= 0xf900) && ((c) <= 0xfa2d)) ||
     (((c) >= 0x3021) && ((c) <= 0x3029)) ||
      ((c) == 0x3007));
}

/**
 * xmlIsLetter:
 * @c:  an unicode character (int)
 *
 * Check whether the character is allowed by the production
 * [84] Letter ::= BaseChar | Ideographic
 *
 * Returns 0 if not, non-zero otherwise
 */
int
xmlIsLetter(int c) {
    return(IS_BASECHAR(c) || IS_IDEOGRAPHIC(c));
}

/**
 * xmlIsPubidChar:
 * @c:  an unicode character (int)
 *
 * Check whether the character is allowed by the production
 * [13] PubidChar ::= #x20 | #xD | #xA | [a-zA-Z0-9] | [-'()+,./:=?;!*#@$_%]
 *
 * Returns 0 if not, non-zero otherwise
 */
int
xmlIsPubidChar(int c) {
    return(
     ((c) == 0x20) || ((c) == 0x0D) || ((c) == 0x0A) ||
     (((c) >= 'a') && ((c) <= 'z')) ||
     (((c) >= 'A') && ((c) <= 'Z')) ||
     (((c) >= '0') && ((c) <= '9')) ||
     ((c) == '-') || ((c) == '\'') || ((c) == '(') || ((c) == ')') ||
     ((c) == '+') || ((c) == ',') || ((c) == '.') || ((c) == '/') ||
     ((c) == ':') || ((c) == '=') || ((c) == '?') || ((c) == ';') ||
     ((c) == '!') || ((c) == '*') || ((c) == '#') || ((c) == '@') ||
     ((c) == '$') || ((c) == '_') || ((c) == '%'));
}

/************************************************************************
 *									*
 * 		Input handling functions for progressive parsing	*
 *									*
 ************************************************************************/

/* #define DEBUG_INPUT */
/* #define DEBUG_STACK */
/* #define DEBUG_PUSH */


/* we need to keep enough input to show errors in context */
#define LINE_LEN        80

#ifdef DEBUG_INPUT
#define CHECK_BUFFER(in) check_buffer(in)

static
void check_buffer(xmlParserInputPtr in) {
    if (in->base != in->buf->buffer->content) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlParserInput: base mismatch problem\n");
    }
    if (in->cur < in->base) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlParserInput: cur < base problem\n");
    }
    if (in->cur > in->base + in->buf->buffer->use) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlParserInput: cur > base + use problem\n");
    }
    xmlGenericError(xmlGenericErrorContext,"buffer %x : content %x, cur %d, use %d, size %d\n",
            (int) in, (int) in->buf->buffer->content, in->cur - in->base,
	    in->buf->buffer->use, in->buf->buffer->size);
}

#else
#define CHECK_BUFFER(in) 
#endif


/**
 * xmlParserInputRead:
 * @in:  an XML parser input
 * @len:  an indicative size for the lookahead
 *
 * This function refresh the input for the parser. It doesn't try to
 * preserve pointers to the input buffer, and discard already read data
 *
 * Returns the number of xmlChars read, or -1 in case of error, 0 indicate the
 * end of this entity
 */
int
xmlParserInputRead(xmlParserInputPtr in, int len) {
    int ret;
    int used;
    int indx;

#ifdef DEBUG_INPUT
    xmlGenericError(xmlGenericErrorContext, "Read\n");
#endif
    if (in->buf == NULL) return(-1);
    if (in->base == NULL) return(-1);
    if (in->cur == NULL) return(-1);
    if (in->buf->buffer == NULL) return(-1);
    if (in->buf->readcallback == NULL) return(-1);

    CHECK_BUFFER(in);

    used = in->cur - in->buf->buffer->content;
    ret = xmlBufferShrink(in->buf->buffer, used);
    if (ret > 0) {
	in->cur -= ret;
	in->consumed += ret;
    }
    ret = xmlParserInputBufferRead(in->buf, len);
    if (in->base != in->buf->buffer->content) {
        /*
	 * the buffer has been reallocated
	 */
	indx = in->cur - in->base;
	in->base = in->buf->buffer->content;
	in->cur = &in->buf->buffer->content[indx];
    }
    in->end = &in->buf->buffer->content[in->buf->buffer->use];

    CHECK_BUFFER(in);

    return(ret);
}

/**
 * xmlParserInputGrow:
 * @in:  an XML parser input
 * @len:  an indicative size for the lookahead
 *
 * This function increase the input for the parser. It tries to
 * preserve pointers to the input buffer, and keep already read data
 *
 * Returns the number of xmlChars read, or -1 in case of error, 0 indicate the
 * end of this entity
 */
int
xmlParserInputGrow(xmlParserInputPtr in, int len) {
    int ret;
    int indx;

#ifdef DEBUG_INPUT
    xmlGenericError(xmlGenericErrorContext, "Grow\n");
#endif
    if (in->buf == NULL) return(-1);
    if (in->base == NULL) return(-1);
    if (in->cur == NULL) return(-1);
    if (in->buf->buffer == NULL) return(-1);

    CHECK_BUFFER(in);

    indx = in->cur - in->base;
    if (in->buf->buffer->use > (unsigned int) indx + INPUT_CHUNK) {

	CHECK_BUFFER(in);

        return(0);
    }
    if (in->buf->readcallback != NULL)
	ret = xmlParserInputBufferGrow(in->buf, len);
    else	
        return(0);

    /*
     * NOTE : in->base may be a "dangling" i.e. freed pointer in this
     *        block, but we use it really as an integer to do some
     *        pointer arithmetic. Insure will raise it as a bug but in
     *        that specific case, that's not !
     */
    if (in->base != in->buf->buffer->content) {
        /*
	 * the buffer has been reallocated
	 */
	indx = in->cur - in->base;
	in->base = in->buf->buffer->content;
	in->cur = &in->buf->buffer->content[indx];
    }
    in->end = &in->buf->buffer->content[in->buf->buffer->use];

    CHECK_BUFFER(in);

    return(ret);
}

/**
 * xmlParserInputShrink:
 * @in:  an XML parser input
 *
 * This function removes used input for the parser.
 */
void
xmlParserInputShrink(xmlParserInputPtr in) {
    int used;
    int ret;
    int indx;

#ifdef DEBUG_INPUT
    xmlGenericError(xmlGenericErrorContext, "Shrink\n");
#endif
    if (in->buf == NULL) return;
    if (in->base == NULL) return;
    if (in->cur == NULL) return;
    if (in->buf->buffer == NULL) return;

    CHECK_BUFFER(in);

    used = in->cur - in->buf->buffer->content;
    /*
     * Do not shrink on large buffers whose only a tiny fraction
     * was consumed
     */
    if ((int) in->buf->buffer->use > used + 2 * INPUT_CHUNK)
	return;
    if (used > INPUT_CHUNK) {
	ret = xmlBufferShrink(in->buf->buffer, used - LINE_LEN);
	if (ret > 0) {
	    in->cur -= ret;
	    in->consumed += ret;
	}
	in->end = &in->buf->buffer->content[in->buf->buffer->use];
    }

    CHECK_BUFFER(in);

    if (in->buf->buffer->use > INPUT_CHUNK) {
        return;
    }
    xmlParserInputBufferRead(in->buf, 2 * INPUT_CHUNK);
    if (in->base != in->buf->buffer->content) {
        /*
	 * the buffer has been reallocated
	 */
	indx = in->cur - in->base;
	in->base = in->buf->buffer->content;
	in->cur = &in->buf->buffer->content[indx];
    }
    in->end = &in->buf->buffer->content[in->buf->buffer->use];

    CHECK_BUFFER(in);
}

/************************************************************************
 *									*
 * 		UTF8 character input and related functions		*
 *									*
 ************************************************************************/

/**
 * xmlNextChar:
 * @ctxt:  the XML parser context
 *
 * Skip to the next char input char.
 */

void
xmlNextChar(xmlParserCtxtPtr ctxt) {
    if (ctxt->instate == XML_PARSER_EOF)
	return;

    /*
     *   2.11 End-of-Line Handling
     *   the literal two-character sequence "#xD#xA" or a standalone
     *   literal #xD, an XML processor must pass to the application
     *   the single character #xA. 
     */
    if (ctxt->charset == XML_CHAR_ENCODING_UTF8) {
	if ((*ctxt->input->cur == 0) &&
	    (xmlParserInputGrow(ctxt->input, INPUT_CHUNK) <= 0) &&
	    (ctxt->instate != XML_PARSER_COMMENT)) {
	        /*
		 * If we are at the end of the current entity and
		 * the context allows it, we pop consumed entities
		 * automatically.
		 * the auto closing should be blocked in other cases
		 */
		xmlPopInput(ctxt);
	} else {
	    if (*(ctxt->input->cur) == '\n') {
		ctxt->input->line++; ctxt->input->col = 1;
	    } else ctxt->input->col++;
	    if (ctxt->charset == XML_CHAR_ENCODING_UTF8) {
		/*
		 * We are supposed to handle UTF8, check it's valid
		 * From rfc2044: encoding of the Unicode values on UTF-8:
		 *
		 * UCS-4 range (hex.)           UTF-8 octet sequence (binary)
		 * 0000 0000-0000 007F   0xxxxxxx
		 * 0000 0080-0000 07FF   110xxxxx 10xxxxxx
		 * 0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx 
		 *
		 * Check for the 0x110000 limit too
		 */
		const unsigned char *cur = ctxt->input->cur;
		unsigned char c;

		c = *cur;
		if (c & 0x80) {
		    if (cur[1] == 0)
			xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
		    if ((cur[1] & 0xc0) != 0x80)
			goto encoding_error;
		    if ((c & 0xe0) == 0xe0) {
			unsigned int val;

			if (cur[2] == 0)
			    xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
			if ((cur[2] & 0xc0) != 0x80)
			    goto encoding_error;
			if ((c & 0xf0) == 0xf0) {
			    if (cur[3] == 0)
				xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
			    if (((c & 0xf8) != 0xf0) ||
				((cur[3] & 0xc0) != 0x80))
				goto encoding_error;
			    /* 4-byte code */
			    ctxt->input->cur += 4;
			    val = (cur[0] & 0x7) << 18;
			    val |= (cur[1] & 0x3f) << 12;
			    val |= (cur[2] & 0x3f) << 6;
			    val |= cur[3] & 0x3f;
			} else {
			  /* 3-byte code */
			    ctxt->input->cur += 3;
			    val = (cur[0] & 0xf) << 12;
			    val |= (cur[1] & 0x3f) << 6;
			    val |= cur[2] & 0x3f;
			}
			if (((val > 0xd7ff) && (val < 0xe000)) ||
			    ((val > 0xfffd) && (val < 0x10000)) ||
			    (val >= 0x110000)) {
			    if ((ctxt->sax != NULL) &&
				(ctxt->sax->error != NULL))
				ctxt->sax->error(ctxt->userData, 
				 "Char 0x%X out of allowed range\n", val);
			    ctxt->errNo = XML_ERR_INVALID_ENCODING;
			    ctxt->wellFormed = 0;
			    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
			}    
		    } else
		      /* 2-byte code */
		        ctxt->input->cur += 2;
		} else
		    /* 1-byte code */
		    ctxt->input->cur++;
	    } else {
		/*
		 * Assume it's a fixed length encoding (1) with
		 * a compatible encoding for the ASCII set, since
		 * XML constructs only use < 128 chars
		 */
	        ctxt->input->cur++;
	    }
	    ctxt->nbChars++;
	    if (*ctxt->input->cur == 0)
		xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
	}
    } else {
	ctxt->input->cur++;
	ctxt->nbChars++;
	if (*ctxt->input->cur == 0)
	    xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
    }
    if ((*ctxt->input->cur == '%') && (!ctxt->html))
	xmlParserHandlePEReference(ctxt);
    if ((*ctxt->input->cur == 0) &&
        (xmlParserInputGrow(ctxt->input, INPUT_CHUNK) <= 0))
	    xmlPopInput(ctxt);
    return;
encoding_error:
    /*
     * If we detect an UTF8 error that probably mean that the
     * input encoding didn't get properly advertised in the
     * declaration header. Report the error and switch the encoding
     * to ISO-Latin-1 (if you don't like this policy, just declare the
     * encoding !)
     */
    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL)) {
	ctxt->sax->error(ctxt->userData, 
			 "Input is not proper UTF-8, indicate encoding !\n");
	ctxt->sax->error(ctxt->userData, "Bytes: 0x%02X 0x%02X 0x%02X 0x%02X\n",
			ctxt->input->cur[0], ctxt->input->cur[1],
			ctxt->input->cur[2], ctxt->input->cur[3]);
    }
    ctxt->wellFormed = 0;
    ctxt->errNo = XML_ERR_INVALID_ENCODING;

    ctxt->charset = XML_CHAR_ENCODING_8859_1; 
    ctxt->input->cur++;
    return;
}

/**
 * xmlCurrentChar:
 * @ctxt:  the XML parser context
 * @len:  pointer to the length of the char read
 *
 * The current char value, if using UTF-8 this may actually span multiple
 * bytes in the input buffer. Implement the end of line normalization:
 * 2.11 End-of-Line Handling
 * Wherever an external parsed entity or the literal entity value
 * of an internal parsed entity contains either the literal two-character
 * sequence "#xD#xA" or a standalone literal #xD, an XML processor
 * must pass to the application the single character #xA.
 * This behavior can conveniently be produced by normalizing all
 * line breaks to #xA on input, before parsing.)
 *
 * Returns the current char value and its length
 */

int
xmlCurrentChar(xmlParserCtxtPtr ctxt, int *len) {
    if (ctxt->instate == XML_PARSER_EOF)
	return(0);

    if ((*ctxt->input->cur >= 0x20) && (*ctxt->input->cur <= 0x7F)) {
	    *len = 1;
	    return((int) *ctxt->input->cur);
    }
    if (ctxt->charset == XML_CHAR_ENCODING_UTF8) {
	/*
	 * We are supposed to handle UTF8, check it's valid
	 * From rfc2044: encoding of the Unicode values on UTF-8:
	 *
	 * UCS-4 range (hex.)           UTF-8 octet sequence (binary)
	 * 0000 0000-0000 007F   0xxxxxxx
	 * 0000 0080-0000 07FF   110xxxxx 10xxxxxx
	 * 0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx 
	 *
	 * Check for the 0x110000 limit too
	 */
	const unsigned char *cur = ctxt->input->cur;
	unsigned char c;
	unsigned int val;

	c = *cur;
	if (c & 0x80) {
	    if (cur[1] == 0)
		xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
	    if ((cur[1] & 0xc0) != 0x80)
		goto encoding_error;
	    if ((c & 0xe0) == 0xe0) {

		if (cur[2] == 0)
		    xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
		if ((cur[2] & 0xc0) != 0x80)
		    goto encoding_error;
		if ((c & 0xf0) == 0xf0) {
		    if (cur[3] == 0)
			xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
		    if (((c & 0xf8) != 0xf0) ||
			((cur[3] & 0xc0) != 0x80))
			goto encoding_error;
		    /* 4-byte code */
		    *len = 4;
		    val = (cur[0] & 0x7) << 18;
		    val |= (cur[1] & 0x3f) << 12;
		    val |= (cur[2] & 0x3f) << 6;
		    val |= cur[3] & 0x3f;
		} else {
		  /* 3-byte code */
		    *len = 3;
		    val = (cur[0] & 0xf) << 12;
		    val |= (cur[1] & 0x3f) << 6;
		    val |= cur[2] & 0x3f;
		}
	    } else {
	      /* 2-byte code */
		*len = 2;
		val = (cur[0] & 0x1f) << 6;
		val |= cur[1] & 0x3f;
	    }
	    if (!IS_CHAR(val)) {
		if ((ctxt->sax != NULL) &&
		    (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData, 
				     "Char 0x%X out of allowed range\n", val);
		ctxt->errNo = XML_ERR_INVALID_ENCODING;
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    }    
	    return(val);
	} else {
	    /* 1-byte code */
	    *len = 1;
	    if (*ctxt->input->cur == 0xD) {
		if (ctxt->input->cur[1] == 0xA) {
		    ctxt->nbChars++;
		    ctxt->input->cur++;
		}
		return(0xA);
	    }
	    return((int) *ctxt->input->cur);
	}
    }
    /*
     * Assume it's a fixed length encoding (1) with
     * a compatible encoding for the ASCII set, since
     * XML constructs only use < 128 chars
     */
    *len = 1;
    if (*ctxt->input->cur == 0xD) {
	if (ctxt->input->cur[1] == 0xA) {
	    ctxt->nbChars++;
	    ctxt->input->cur++;
	}
	return(0xA);
    }
    return((int) *ctxt->input->cur);
encoding_error:
    /*
     * An encoding problem may arise from a truncated input buffer
     * splitting a character in the middle. In that case do not raise
     * an error but return 0 to endicate an end of stream problem
     */
    if (ctxt->input->end - ctxt->input->cur < 4) {
	*len = 0;
	return(0);
    }

    /*
     * If we detect an UTF8 error that probably mean that the
     * input encoding didn't get properly advertised in the
     * declaration header. Report the error and switch the encoding
     * to ISO-Latin-1 (if you don't like this policy, just declare the
     * encoding !)
     */
    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL)) {
	ctxt->sax->error(ctxt->userData, 
			 "Input is not proper UTF-8, indicate encoding !\n");
	ctxt->sax->error(ctxt->userData, "Bytes: 0x%02X 0x%02X 0x%02X 0x%02X\n",
			ctxt->input->cur[0], ctxt->input->cur[1],
			ctxt->input->cur[2], ctxt->input->cur[3]);
    }
    ctxt->wellFormed = 0;
    ctxt->errNo = XML_ERR_INVALID_ENCODING;

    ctxt->charset = XML_CHAR_ENCODING_8859_1; 
    *len = 1;
    return((int) *ctxt->input->cur);
}

/**
 * xmlStringCurrentChar:
 * @ctxt:  the XML parser context
 * @cur:  pointer to the beginning of the char
 * @len:  pointer to the length of the char read
 *
 * The current char value, if using UTF-8 this may actually span multiple
 * bytes in the input buffer.
 *
 * Returns the current char value and its length
 */

int
xmlStringCurrentChar(xmlParserCtxtPtr ctxt, const xmlChar * cur, int *len)
{
    if ((ctxt == NULL) || (ctxt->charset == XML_CHAR_ENCODING_UTF8)) {
        /*
         * We are supposed to handle UTF8, check it's valid
         * From rfc2044: encoding of the Unicode values on UTF-8:
         *
         * UCS-4 range (hex.)           UTF-8 octet sequence (binary)
         * 0000 0000-0000 007F   0xxxxxxx
         * 0000 0080-0000 07FF   110xxxxx 10xxxxxx
         * 0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx 
         *
         * Check for the 0x110000 limit too
         */
        unsigned char c;
        unsigned int val;

        c = *cur;
        if (c & 0x80) {
            if ((cur[1] & 0xc0) != 0x80)
                goto encoding_error;
            if ((c & 0xe0) == 0xe0) {

                if ((cur[2] & 0xc0) != 0x80)
                    goto encoding_error;
                if ((c & 0xf0) == 0xf0) {
                    if (((c & 0xf8) != 0xf0) || ((cur[3] & 0xc0) != 0x80))
                        goto encoding_error;
                    /* 4-byte code */
                    *len = 4;
                    val = (cur[0] & 0x7) << 18;
                    val |= (cur[1] & 0x3f) << 12;
                    val |= (cur[2] & 0x3f) << 6;
                    val |= cur[3] & 0x3f;
                } else {
                    /* 3-byte code */
                    *len = 3;
                    val = (cur[0] & 0xf) << 12;
                    val |= (cur[1] & 0x3f) << 6;
                    val |= cur[2] & 0x3f;
                }
            } else {
                /* 2-byte code */
                *len = 2;
                val = (cur[0] & 0x1f) << 6;
                val |= cur[1] & 0x3f;
            }
            if (!IS_CHAR(val)) {
                if ((ctxt != NULL) && (ctxt->sax != NULL) &&
                    (ctxt->sax->error != NULL))
                    ctxt->sax->error(ctxt->userData,
                                     "Char 0x%X out of allowed range\n",
                                     val);
		if (ctxt != NULL) {
		    ctxt->errNo = XML_ERR_INVALID_ENCODING;
		    ctxt->wellFormed = 0;
		    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		}
            }
            return (val);
        } else {
            /* 1-byte code */
            *len = 1;
            return ((int) *cur);
        }
    }
    /*
     * Assume it's a fixed length encoding (1) with
     * a compatible encoding for the ASCII set, since
     * XML constructs only use < 128 chars
     */
    *len = 1;
    return ((int) *cur);
encoding_error:

    /*
     * If we detect an UTF8 error that probably mean that the
     * input encoding didn't get properly advertised in the
     * declaration header. Report the error and switch the encoding
     * to ISO-Latin-1 (if you don't like this policy, just declare the
     * encoding !)
     */
    if (ctxt != NULL) {
        if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL)) {
            ctxt->sax->error(ctxt->userData,
                         "Input is not proper UTF-8, indicate encoding !\n");
            ctxt->sax->error(ctxt->userData,
                             "Bytes: 0x%02X 0x%02X 0x%02X 0x%02X\n",
                             ctxt->input->cur[0], ctxt->input->cur[1],
                             ctxt->input->cur[2], ctxt->input->cur[3]);
        }
        ctxt->errNo = XML_ERR_INVALID_ENCODING;
	ctxt->wellFormed = 0;
    }

    *len = 1;
    return ((int) *cur);
}

/**
 * xmlCopyCharMultiByte:
 * @out:  pointer to an array of xmlChar
 * @val:  the char value
 *
 * append the char value in the array 
 *
 * Returns the number of xmlChar written
 */
int
xmlCopyCharMultiByte(xmlChar *out, int val) {
    /*
     * We are supposed to handle UTF8, check it's valid
     * From rfc2044: encoding of the Unicode values on UTF-8:
     *
     * UCS-4 range (hex.)           UTF-8 octet sequence (binary)
     * 0000 0000-0000 007F   0xxxxxxx
     * 0000 0080-0000 07FF   110xxxxx 10xxxxxx
     * 0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx 
     */
    if  (val >= 0x80) {
	xmlChar *savedout = out;
	int bits;
	if (val <   0x800) { *out++= (val >>  6) | 0xC0;  bits=  0; }
	else if (val < 0x10000) { *out++= (val >> 12) | 0xE0;  bits=  6;}
	else if (val < 0x110000)  { *out++= (val >> 18) | 0xF0;  bits=  12; }
	else {
	    xmlGenericError(xmlGenericErrorContext,
		    "Internal error, xmlCopyCharMultiByte 0x%X out of bound\n",
		    val);
	    return(0);
	}
	for ( ; bits >= 0; bits-= 6)
	    *out++= ((val >> bits) & 0x3F) | 0x80 ;
	return (out - savedout);
    }
    *out = (xmlChar) val;
    return 1;
}

/**
 * xmlCopyChar:
 * @len:  Ignored, compatibility
 * @out:  pointer to an array of xmlChar
 * @val:  the char value
 *
 * append the char value in the array 
 *
 * Returns the number of xmlChar written
 */

int
xmlCopyChar(int len ATTRIBUTE_UNUSED, xmlChar *out, int val) {
    /* the len parameter is ignored */
    if  (val >= 0x80) {
	return(xmlCopyCharMultiByte (out, val));
    }
    *out = (xmlChar) val;
    return 1;
}

/************************************************************************
 *									*
 *		Commodity functions to switch encodings			*
 *									*
 ************************************************************************/

/**
 * xmlSwitchEncoding:
 * @ctxt:  the parser context
 * @enc:  the encoding value (number)
 *
 * change the input functions when discovering the character encoding
 * of a given entity.
 *
 * Returns 0 in case of success, -1 otherwise
 */
int
xmlSwitchEncoding(xmlParserCtxtPtr ctxt, xmlCharEncoding enc)
{
    xmlCharEncodingHandlerPtr handler;

    switch (enc) {
	case XML_CHAR_ENCODING_ERROR:
	    ctxt->errNo = XML_ERR_UNKNOWN_ENCODING;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData, "encoding unknown\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    break;
	case XML_CHAR_ENCODING_NONE:
	    /* let's assume it's UTF-8 without the XML decl */
	    ctxt->charset = XML_CHAR_ENCODING_UTF8;
	    return(0);
	case XML_CHAR_ENCODING_UTF8:
	    /* default encoding, no conversion should be needed */
	    ctxt->charset = XML_CHAR_ENCODING_UTF8;

	    /*
	     * Errata on XML-1.0 June 20 2001
	     * Specific handling of the Byte Order Mark for
	     * UTF-8
	     */
	    if ((ctxt->input != NULL) &&
		(ctxt->input->cur[0] == 0xEF) &&
		(ctxt->input->cur[1] == 0xBB) &&
		(ctxt->input->cur[2] == 0xBF)) {
		ctxt->input->cur += 3;
	    }
	    return(0);
	default:
	    break;
    }
    handler = xmlGetCharEncodingHandler(enc);
    if (handler == NULL) {
	/*
	 * Default handlers.
	 */
	switch (enc) {
	    case XML_CHAR_ENCODING_ERROR:
		ctxt->errNo = XML_ERR_UNKNOWN_ENCODING;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData, "encoding unknown\n");
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		ctxt->charset = XML_CHAR_ENCODING_UTF8;
		break;
	    case XML_CHAR_ENCODING_NONE:
		/* let's assume it's UTF-8 without the XML decl */
		ctxt->charset = XML_CHAR_ENCODING_UTF8;
		return(0);
	    case XML_CHAR_ENCODING_UTF8:
	    case XML_CHAR_ENCODING_ASCII:
		/* default encoding, no conversion should be needed */
		ctxt->charset = XML_CHAR_ENCODING_UTF8;
		return(0);
	    case XML_CHAR_ENCODING_UTF16LE:
		break;
	    case XML_CHAR_ENCODING_UTF16BE:
		break;
	    case XML_CHAR_ENCODING_UCS4LE:
		ctxt->errNo = XML_ERR_UNSUPPORTED_ENCODING;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
		      "char encoding USC4 little endian not supported\n");
		break;
	    case XML_CHAR_ENCODING_UCS4BE:
		ctxt->errNo = XML_ERR_UNSUPPORTED_ENCODING;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
		      "char encoding USC4 big endian not supported\n");
		break;
	    case XML_CHAR_ENCODING_EBCDIC:
		ctxt->errNo = XML_ERR_UNSUPPORTED_ENCODING;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
		      "char encoding EBCDIC not supported\n");
		break;
	    case XML_CHAR_ENCODING_UCS4_2143:
		ctxt->errNo = XML_ERR_UNSUPPORTED_ENCODING;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
		      "char encoding UCS4 2143 not supported\n");
		break;
	    case XML_CHAR_ENCODING_UCS4_3412:
		ctxt->errNo = XML_ERR_UNSUPPORTED_ENCODING;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
		      "char encoding UCS4 3412 not supported\n");
		break;
	    case XML_CHAR_ENCODING_UCS2:
		ctxt->errNo = XML_ERR_UNSUPPORTED_ENCODING;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
		      "char encoding UCS2 not supported\n");
		break;
	    case XML_CHAR_ENCODING_8859_1:
	    case XML_CHAR_ENCODING_8859_2:
	    case XML_CHAR_ENCODING_8859_3:
	    case XML_CHAR_ENCODING_8859_4:
	    case XML_CHAR_ENCODING_8859_5:
	    case XML_CHAR_ENCODING_8859_6:
	    case XML_CHAR_ENCODING_8859_7:
	    case XML_CHAR_ENCODING_8859_8:
	    case XML_CHAR_ENCODING_8859_9:
		/*
		 * We used to keep the internal content in the
		 * document encoding however this turns being unmaintainable
		 * So xmlGetCharEncodingHandler() will return non-null
		 * values for this now.
		 */
		if ((ctxt->inputNr == 1) &&
		    (ctxt->encoding == NULL) &&
		    (ctxt->input->encoding != NULL)) {
		    ctxt->encoding = xmlStrdup(ctxt->input->encoding);
		}
		ctxt->charset = enc;
		return(0);
	    case XML_CHAR_ENCODING_2022_JP:
		ctxt->errNo = XML_ERR_UNSUPPORTED_ENCODING;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
		      "char encoding ISO-2022-JPnot supported\n");
		break;
	    case XML_CHAR_ENCODING_SHIFT_JIS:
		ctxt->errNo = XML_ERR_UNSUPPORTED_ENCODING;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
		      "char encoding Shift_JIS not supported\n");
		break;
	    case XML_CHAR_ENCODING_EUC_JP:
		ctxt->errNo = XML_ERR_UNSUPPORTED_ENCODING;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
		      "char encoding EUC-JPnot supported\n");
		break;
	}
    }
    if (handler == NULL)
	return(-1);
    ctxt->charset = XML_CHAR_ENCODING_UTF8;
    return(xmlSwitchToEncoding(ctxt, handler));
}

/**
 * xmlSwitchToEncoding:
 * @ctxt:  the parser context
 * @handler:  the encoding handler
 *
 * change the input functions when discovering the character encoding
 * of a given entity.
 *
 * Returns 0 in case of success, -1 otherwise
 */
int
xmlSwitchToEncoding(xmlParserCtxtPtr ctxt, xmlCharEncodingHandlerPtr handler) 
{
    int nbchars;

    if (handler != NULL) {
        if (ctxt->input != NULL) {
	    if (ctxt->input->buf != NULL) {
	        if (ctxt->input->buf->encoder != NULL) {
		    /*
		     * Check in case the auto encoding detetection triggered
		     * in already.
		     */
		    if (ctxt->input->buf->encoder == handler)
			return(0);

		    /*
		     * "UTF-16" can be used for both LE and BE
		     */
		    if ((!xmlStrncmp(BAD_CAST ctxt->input->buf->encoder->name,
				     BAD_CAST "UTF-16", 6)) &&
		        (!xmlStrncmp(BAD_CAST handler->name,
				     BAD_CAST "UTF-16", 6))) {
			return(0);
		    }

		    /*
		     * Note: this is a bit dangerous, but that's what it
		     * takes to use nearly compatible signature for different
		     * encodings.
		     */
		    xmlCharEncCloseFunc(ctxt->input->buf->encoder);
		    ctxt->input->buf->encoder = handler;
		    return(0);
		}
		ctxt->input->buf->encoder = handler;

	        /*
		 * Is there already some content down the pipe to convert ?
		 */
	        if ((ctxt->input->buf->buffer != NULL) &&
		    (ctxt->input->buf->buffer->use > 0)) {
		    int processed;

		    /*
		     * Specific handling of the Byte Order Mark for 
		     * UTF-16
		     */
		    if ((handler->name != NULL) &&
			(!strcmp(handler->name, "UTF-16LE")) && 
		        (ctxt->input->cur[0] == 0xFF) &&
		        (ctxt->input->cur[1] == 0xFE)) {
			ctxt->input->cur += 2;
		    }
		    if ((handler->name != NULL) &&
			(!strcmp(handler->name, "UTF-16BE")) && 
		        (ctxt->input->cur[0] == 0xFE) &&
		        (ctxt->input->cur[1] == 0xFF)) {
			ctxt->input->cur += 2;
		    }
		    /*
		     * Errata on XML-1.0 June 20 2001
		     * Specific handling of the Byte Order Mark for
		     * UTF-8
		     */
		    if ((handler->name != NULL) &&
			(!strcmp(handler->name, "UTF-8")) &&
			(ctxt->input->cur[0] == 0xEF) &&
			(ctxt->input->cur[1] == 0xBB) &&
			(ctxt->input->cur[2] == 0xBF)) {
			ctxt->input->cur += 3;
		    }

		    /*
		     * Shrink the current input buffer.
		     * Move it as the raw buffer and create a new input buffer
		     */
		    processed = ctxt->input->cur - ctxt->input->base;
		    xmlBufferShrink(ctxt->input->buf->buffer, processed);
		    ctxt->input->buf->raw = ctxt->input->buf->buffer;
		    ctxt->input->buf->buffer = xmlBufferCreate();

		    if (ctxt->html) {
			/*
			 * convert as much as possible of the buffer
			 */
			nbchars = xmlCharEncInFunc(ctxt->input->buf->encoder,
				                   ctxt->input->buf->buffer,
						   ctxt->input->buf->raw);
		    } else {
			/*
			 * convert just enough to get
			 * '<?xml version="1.0" encoding="xxx"?>'
			 * parsed with the autodetected encoding
			 * into the parser reading buffer.
			 */
			nbchars = xmlCharEncFirstLine(ctxt->input->buf->encoder,
						      ctxt->input->buf->buffer,
						      ctxt->input->buf->raw);
		    }
		    if (nbchars < 0) {
			xmlGenericError(xmlGenericErrorContext,
				"xmlSwitchToEncoding: encoder error\n");
			return(-1);
		    }
		    ctxt->input->base =
		    ctxt->input->cur = ctxt->input->buf->buffer->content;
		    ctxt->input->end =
			&ctxt->input->base[ctxt->input->buf->buffer->use];

		}
		return(0);
	    } else {
	        if ((ctxt->input->length == 0) || (ctxt->input->buf == NULL)) {
		    /*
		     * When parsing a static memory array one must know the
		     * size to be able to convert the buffer.
		     */
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData,
					 "xmlSwitchToEncoding : no input\n");
		    return(-1);
		} else {
		    int processed;

		    /*
		     * Shrink the current input buffer.
		     * Move it as the raw buffer and create a new input buffer
		     */
		    processed = ctxt->input->cur - ctxt->input->base;

		    ctxt->input->buf->raw = xmlBufferCreate();
		    xmlBufferAdd(ctxt->input->buf->raw, ctxt->input->cur,
				 ctxt->input->length - processed);
		    ctxt->input->buf->buffer = xmlBufferCreate();

		    /*
		     * convert as much as possible of the raw input
		     * to the parser reading buffer.
		     */
		    nbchars = xmlCharEncInFunc(ctxt->input->buf->encoder,
		                               ctxt->input->buf->buffer,
					       ctxt->input->buf->raw);
		    if (nbchars < 0) {
			xmlGenericError(xmlGenericErrorContext,
				"xmlSwitchToEncoding: encoder error\n");
			return(-1);
		    }

		    /*
		     * Conversion succeeded, get rid of the old buffer
		     */
		    if ((ctxt->input->free != NULL) &&
		        (ctxt->input->base != NULL))
			ctxt->input->free((xmlChar *) ctxt->input->base);
		    ctxt->input->base =
		    ctxt->input->cur = ctxt->input->buf->buffer->content;
		    ctxt->input->end =
			&ctxt->input->base[ctxt->input->buf->buffer->use];
		}
	    }
	} else {
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData,
		                 "xmlSwitchToEncoding : no input\n");
	    return(-1);
	}
	/*
	 * The parsing is now done in UTF8 natively
	 */
	ctxt->charset = XML_CHAR_ENCODING_UTF8;
    } else 
	return(-1);
    return(0);

}

/************************************************************************
 *									*
 *	Commodity functions to handle entities processing		*
 *									*
 ************************************************************************/

/**
 * xmlFreeInputStream:
 * @input:  an xmlParserInputPtr
 *
 * Free up an input stream.
 */
void
xmlFreeInputStream(xmlParserInputPtr input) {
    if (input == NULL) return;

    if (input->filename != NULL) xmlFree((char *) input->filename);
    if (input->directory != NULL) xmlFree((char *) input->directory);
    if (input->encoding != NULL) xmlFree((char *) input->encoding);
    if (input->version != NULL) xmlFree((char *) input->version);
    if ((input->free != NULL) && (input->base != NULL))
        input->free((xmlChar *) input->base);
    if (input->buf != NULL) 
        xmlFreeParserInputBuffer(input->buf);
    xmlFree(input);
}

/**
 * xmlNewInputStream:
 * @ctxt:  an XML parser context
 *
 * Create a new input stream structure
 * Returns the new input stream or NULL
 */
xmlParserInputPtr
xmlNewInputStream(xmlParserCtxtPtr ctxt) {
    xmlParserInputPtr input;

    input = (xmlParserInputPtr) xmlMalloc(sizeof(xmlParserInput));
    if (input == NULL) {
	if (ctxt != NULL) {
	    ctxt->errNo = XML_ERR_NO_MEMORY;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData, 
			 "malloc: couldn't allocate a new input stream\n");
	    ctxt->errNo = XML_ERR_NO_MEMORY;
	}
	return(NULL);
    }
    memset(input, 0, sizeof(xmlParserInput));
    input->line = 1;
    input->col = 1;
    input->standalone = -1;
    return(input);
}

/**
 * xmlNewIOInputStream:
 * @ctxt:  an XML parser context
 * @input:  an I/O Input
 * @enc:  the charset encoding if known
 *
 * Create a new input stream structure encapsulating the @input into
 * a stream suitable for the parser.
 *
 * Returns the new input stream or NULL
 */
xmlParserInputPtr
xmlNewIOInputStream(xmlParserCtxtPtr ctxt, xmlParserInputBufferPtr input,
	            xmlCharEncoding enc) {
    xmlParserInputPtr inputStream;

    if (xmlParserDebugEntities)
	xmlGenericError(xmlGenericErrorContext, "new input from I/O\n");
    inputStream = xmlNewInputStream(ctxt);
    if (inputStream == NULL) {
	return(NULL);
    }
    inputStream->filename = NULL;
    inputStream->buf = input;
    inputStream->base = inputStream->buf->buffer->content;
    inputStream->cur = inputStream->buf->buffer->content;
    inputStream->end = &inputStream->base[inputStream->buf->buffer->use];
    if (enc != XML_CHAR_ENCODING_NONE) {
        xmlSwitchEncoding(ctxt, enc);
    }

    return(inputStream);
}

/**
 * xmlNewEntityInputStream:
 * @ctxt:  an XML parser context
 * @entity:  an Entity pointer
 *
 * Create a new input stream based on an xmlEntityPtr
 *
 * Returns the new input stream or NULL
 */
xmlParserInputPtr
xmlNewEntityInputStream(xmlParserCtxtPtr ctxt, xmlEntityPtr entity) {
    xmlParserInputPtr input;

    if (entity == NULL) {
        ctxt->errNo = XML_ERR_INTERNAL_ERROR;
        if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	      "internal: xmlNewEntityInputStream entity = NULL\n");
	ctxt->errNo = XML_ERR_INTERNAL_ERROR;
	return(NULL);
    }
    if (xmlParserDebugEntities)
	xmlGenericError(xmlGenericErrorContext,
		"new input from entity: %s\n", entity->name);
    if (entity->content == NULL) {
	switch (entity->etype) {
            case XML_EXTERNAL_GENERAL_UNPARSED_ENTITY:
	        ctxt->errNo = XML_ERR_UNPARSED_ENTITY;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
		      "xmlNewEntityInputStream unparsed entity !\n");
                break;
            case XML_EXTERNAL_GENERAL_PARSED_ENTITY:
            case XML_EXTERNAL_PARAMETER_ENTITY:
		return(xmlLoadExternalEntity((char *) entity->URI,
		       (char *) entity->ExternalID, ctxt));
            case XML_INTERNAL_GENERAL_ENTITY:
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
	  "Internal entity %s without content !\n", entity->name);
                break;
            case XML_INTERNAL_PARAMETER_ENTITY:
		ctxt->errNo = XML_ERR_INTERNAL_ERROR;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
	  "Internal parameter entity %s without content !\n", entity->name);
                break;
            case XML_INTERNAL_PREDEFINED_ENTITY:
		ctxt->errNo = XML_ERR_INTERNAL_ERROR;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
	      "Predefined entity %s without content !\n", entity->name);
                break;
	}
	return(NULL);
    }
    input = xmlNewInputStream(ctxt);
    if (input == NULL) {
	return(NULL);
    }
    input->filename = (char *) entity->URI;
    input->base = entity->content;
    input->cur = entity->content;
    input->length = entity->length;
    input->end = &entity->content[input->length];
    return(input);
}

/**
 * xmlNewStringInputStream:
 * @ctxt:  an XML parser context
 * @buffer:  an memory buffer
 *
 * Create a new input stream based on a memory buffer.
 * Returns the new input stream
 */
xmlParserInputPtr
xmlNewStringInputStream(xmlParserCtxtPtr ctxt, const xmlChar *buffer) {
    xmlParserInputPtr input;

    if (buffer == NULL) {
	ctxt->errNo = XML_ERR_INTERNAL_ERROR;
        if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	      "internal: xmlNewStringInputStream string = NULL\n");
	return(NULL);
    }
    if (xmlParserDebugEntities)
	xmlGenericError(xmlGenericErrorContext,
		"new fixed input: %.30s\n", buffer);
    input = xmlNewInputStream(ctxt);
    if (input == NULL) {
	return(NULL);
    }
    input->base = buffer;
    input->cur = buffer;
    input->length = xmlStrlen(buffer);
    input->end = &buffer[input->length];
    return(input);
}

/**
 * xmlNewInputFromFile:
 * @ctxt:  an XML parser context
 * @filename:  the filename to use as entity
 *
 * Create a new input stream based on a file.
 *
 * Returns the new input stream or NULL in case of error
 */
xmlParserInputPtr
xmlNewInputFromFile(xmlParserCtxtPtr ctxt, const char *filename) {
    xmlParserInputBufferPtr buf;
    xmlParserInputPtr inputStream;
    char *directory = NULL;
    xmlChar *URI = NULL;

    if (xmlParserDebugEntities)
	xmlGenericError(xmlGenericErrorContext,
		"new input from file: %s\n", filename);
    if (ctxt == NULL) return(NULL);
    buf = xmlParserInputBufferCreateFilename(filename, XML_CHAR_ENCODING_NONE);
    if (buf == NULL)
	return(NULL);

    URI = xmlStrdup((xmlChar *) filename);
    directory = xmlParserGetDirectory((const char *) URI);

    inputStream = xmlNewInputStream(ctxt);
    if (inputStream == NULL) {
	if (directory != NULL) xmlFree((char *) directory);
	if (URI != NULL) xmlFree((char *) URI);
	return(NULL);
    }

    inputStream->filename = (const char *) URI;
    inputStream->directory = directory;
    inputStream->buf = buf;

    inputStream->base = inputStream->buf->buffer->content;
    inputStream->cur = inputStream->buf->buffer->content;
    inputStream->end = &inputStream->base[inputStream->buf->buffer->use];
    if ((ctxt->directory == NULL) && (directory != NULL))
        ctxt->directory = (char *) xmlStrdup((const xmlChar *) directory);
    return(inputStream);
}

/************************************************************************
 *									*
 *		Commodity functions to handle parser contexts		*
 *									*
 ************************************************************************/

/**
 * xmlInitParserCtxt:
 * @ctxt:  an XML parser context
 *
 * Initialize a parser context
 */

void
xmlInitParserCtxt(xmlParserCtxtPtr ctxt)
{
    if(ctxt==NULL) {
	xmlGenericError(xmlGenericErrorContext,
		"xmlInitParserCtxt: NULL context given\n");
        return;
    }

    xmlDefaultSAXHandlerInit();

    ctxt->sax = (xmlSAXHandler *) xmlMalloc(sizeof(xmlSAXHandler));
    if (ctxt->sax == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlInitParserCtxt: out of memory\n");
    }
    else
        memcpy(ctxt->sax, &xmlDefaultSAXHandler, sizeof(xmlSAXHandler));

    /* Allocate the Input stack */
    ctxt->inputTab = (xmlParserInputPtr *)
	        xmlMalloc(5 * sizeof(xmlParserInputPtr));
    if (ctxt->inputTab == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlInitParserCtxt: out of memory\n");
	ctxt->inputNr = 0;
	ctxt->inputMax = 0;
	ctxt->input = NULL;
	return;
    }
    ctxt->inputNr = 0;
    ctxt->inputMax = 5;
    ctxt->input = NULL;

    ctxt->version = NULL;
    ctxt->encoding = NULL;
    ctxt->standalone = -1;
    ctxt->hasExternalSubset = 0;
    ctxt->hasPErefs = 0;
    ctxt->html = 0;
    ctxt->external = 0;
    ctxt->instate = XML_PARSER_START;
    ctxt->token = 0;
    ctxt->directory = NULL;

    /* Allocate the Node stack */
    ctxt->nodeTab = (xmlNodePtr *) xmlMalloc(10 * sizeof(xmlNodePtr));
    if (ctxt->nodeTab == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlInitParserCtxt: out of memory\n");
	ctxt->nodeNr = 0;
	ctxt->nodeMax = 0;
	ctxt->node = NULL;
	ctxt->inputNr = 0;
	ctxt->inputMax = 0;
	ctxt->input = NULL;
	return;
    }
    ctxt->nodeNr = 0;
    ctxt->nodeMax = 10;
    ctxt->node = NULL;

    /* Allocate the Name stack */
    ctxt->nameTab = (xmlChar **) xmlMalloc(10 * sizeof(xmlChar *));
    if (ctxt->nameTab == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlInitParserCtxt: out of memory\n");
	ctxt->nodeNr = 0;
	ctxt->nodeMax = 0;
	ctxt->node = NULL;
	ctxt->inputNr = 0;
	ctxt->inputMax = 0;
	ctxt->input = NULL;
	ctxt->nameNr = 0;
	ctxt->nameMax = 0;
	ctxt->name = NULL;
	return;
    }
    ctxt->nameNr = 0;
    ctxt->nameMax = 10;
    ctxt->name = NULL;

    /* Allocate the space stack */
    ctxt->spaceTab = (int *) xmlMalloc(10 * sizeof(int));
    if (ctxt->spaceTab == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlInitParserCtxt: out of memory\n");
	ctxt->nodeNr = 0;
	ctxt->nodeMax = 0;
	ctxt->node = NULL;
	ctxt->inputNr = 0;
	ctxt->inputMax = 0;
	ctxt->input = NULL;
	ctxt->nameNr = 0;
	ctxt->nameMax = 0;
	ctxt->name = NULL;
	ctxt->spaceNr = 0;
	ctxt->spaceMax = 0;
	ctxt->space = NULL;
	return;
    }
    ctxt->spaceNr = 1;
    ctxt->spaceMax = 10;
    ctxt->spaceTab[0] = -1;
    ctxt->space = &ctxt->spaceTab[0];
    ctxt->userData = ctxt;
    ctxt->myDoc = NULL;
    ctxt->wellFormed = 1;
    ctxt->valid = 1;
    ctxt->loadsubset = xmlLoadExtDtdDefaultValue;
    ctxt->validate = xmlDoValidityCheckingDefaultValue;
    ctxt->pedantic = xmlPedanticParserDefaultValue;
    ctxt->linenumbers = xmlLineNumbersDefaultValue;
    ctxt->keepBlanks = xmlKeepBlanksDefaultValue;
    if (ctxt->keepBlanks == 0)
	ctxt->sax->ignorableWhitespace = ignorableWhitespace;

    ctxt->vctxt.userData = ctxt;
    ctxt->vctxt.error = xmlParserValidityError;
    ctxt->vctxt.warning = xmlParserValidityWarning;
    if (ctxt->validate) {
	if (xmlGetWarningsDefaultValue == 0)
	    ctxt->vctxt.warning = NULL;
	else
	    ctxt->vctxt.warning = xmlParserValidityWarning;
	ctxt->vctxt.nodeMax = 0;
    }
    ctxt->replaceEntities = xmlSubstituteEntitiesDefaultValue;
    ctxt->record_info = 0;
    ctxt->nbChars = 0;
    ctxt->checkIndex = 0;
    ctxt->inSubset = 0;
    ctxt->errNo = XML_ERR_OK;
    ctxt->depth = 0;
    ctxt->charset = XML_CHAR_ENCODING_UTF8;
    ctxt->catalogs = NULL;
    xmlInitNodeInfoSeq(&ctxt->node_seq);
}

/**
 * xmlFreeParserCtxt:
 * @ctxt:  an XML parser context
 *
 * Free all the memory used by a parser context. However the parsed
 * document in ctxt->myDoc is not freed.
 */

void
xmlFreeParserCtxt(xmlParserCtxtPtr ctxt)
{
    xmlParserInputPtr input;
    xmlChar *oldname;

    if (ctxt == NULL) return;

    while ((input = inputPop(ctxt)) != NULL) { /* Non consuming */
        xmlFreeInputStream(input);
    }
    while ((oldname = namePop(ctxt)) != NULL) { /* Non consuming */
	xmlFree(oldname);
    }
    if (ctxt->spaceTab != NULL) xmlFree(ctxt->spaceTab);
    if (ctxt->nameTab != NULL) xmlFree(ctxt->nameTab);
    if (ctxt->nodeTab != NULL) xmlFree(ctxt->nodeTab);
    if (ctxt->inputTab != NULL) xmlFree(ctxt->inputTab);
    if (ctxt->version != NULL) xmlFree((char *) ctxt->version);
    if (ctxt->encoding != NULL) xmlFree((char *) ctxt->encoding);
    if (ctxt->intSubName != NULL) xmlFree((char *) ctxt->intSubName);
    if (ctxt->extSubURI != NULL) xmlFree((char *) ctxt->extSubURI);
    if (ctxt->extSubSystem != NULL) xmlFree((char *) ctxt->extSubSystem);
    if ((ctxt->sax != NULL) && (ctxt->sax != &xmlDefaultSAXHandler))
        xmlFree(ctxt->sax);
    if (ctxt->directory != NULL) xmlFree((char *) ctxt->directory);
    if (ctxt->vctxt.nodeTab != NULL) xmlFree(ctxt->vctxt.nodeTab);
#ifdef LIBXML_CATALOG_ENABLED
    if (ctxt->catalogs != NULL)
	xmlCatalogFreeLocal(ctxt->catalogs);
#endif
    xmlFree(ctxt);
}

/**
 * xmlNewParserCtxt:
 *
 * Allocate and initialize a new parser context.
 *
 * Returns the xmlParserCtxtPtr or NULL
 */

xmlParserCtxtPtr
xmlNewParserCtxt()
{
    xmlParserCtxtPtr ctxt;

    ctxt = (xmlParserCtxtPtr) xmlMalloc(sizeof(xmlParserCtxt));
    if (ctxt == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewParserCtxt : cannot allocate context\n");
        xmlGenericError(xmlGenericErrorContext, "malloc failed");
	return(NULL);
    }
    memset(ctxt, 0, sizeof(xmlParserCtxt));
    xmlInitParserCtxt(ctxt);
    return(ctxt);
}

/************************************************************************
 *									*
 *		Handling of node informations				*
 *									*
 ************************************************************************/

/**
 * xmlClearParserCtxt:
 * @ctxt:  an XML parser context
 *
 * Clear (release owned resources) and reinitialize a parser context
 */

void
xmlClearParserCtxt(xmlParserCtxtPtr ctxt)
{
  if (ctxt==NULL)
    return;
  xmlClearNodeInfoSeq(&ctxt->node_seq);
  xmlInitParserCtxt(ctxt);
}

/**
 * xmlParserFindNodeInfo:
 * @ctx:  an XML parser context
 * @node:  an XML node within the tree
 *
 * Find the parser node info struct for a given node
 * 
 * Returns an xmlParserNodeInfo block pointer or NULL
 */
const xmlParserNodeInfo* xmlParserFindNodeInfo(const xmlParserCtxtPtr ctx,
                                               const xmlNodePtr node)
{
  unsigned long pos;

  /* Find position where node should be at */
  pos = xmlParserFindNodeInfoIndex(&ctx->node_seq, node);
  if (pos < ctx->node_seq.length && ctx->node_seq.buffer[pos].node == node)
    return &ctx->node_seq.buffer[pos];
  else
    return NULL;
}


/**
 * xmlInitNodeInfoSeq:
 * @seq:  a node info sequence pointer
 *
 * -- Initialize (set to initial state) node info sequence
 */
void
xmlInitNodeInfoSeq(xmlParserNodeInfoSeqPtr seq)
{
  seq->length = 0;
  seq->maximum = 0;
  seq->buffer = NULL;
}

/**
 * xmlClearNodeInfoSeq:
 * @seq:  a node info sequence pointer
 *
 * -- Clear (release memory and reinitialize) node
 *   info sequence
 */
void
xmlClearNodeInfoSeq(xmlParserNodeInfoSeqPtr seq)
{
  if ( seq->buffer != NULL )
    xmlFree(seq->buffer);
  xmlInitNodeInfoSeq(seq);
}


/**
 * xmlParserFindNodeInfoIndex:
 * @seq:  a node info sequence pointer
 * @node:  an XML node pointer
 *
 * 
 * xmlParserFindNodeInfoIndex : Find the index that the info record for
 *   the given node is or should be at in a sorted sequence
 *
 * Returns a long indicating the position of the record
 */
unsigned long xmlParserFindNodeInfoIndex(const xmlParserNodeInfoSeqPtr seq,
                                         const xmlNodePtr node)
{
  unsigned long upper, lower, middle;
  int found = 0;

  /* Do a binary search for the key */
  lower = 1;
  upper = seq->length;
  middle = 0;
  while ( lower <= upper && !found) {
    middle = lower + (upper - lower) / 2;
    if ( node == seq->buffer[middle - 1].node )
      found = 1;
    else if ( node < seq->buffer[middle - 1].node )
      upper = middle - 1;
    else
      lower = middle + 1;
  }

  /* Return position */
  if ( middle == 0 || seq->buffer[middle - 1].node < node )
    return middle;
  else 
    return middle - 1;
}


/**
 * xmlParserAddNodeInfo:
 * @ctxt:  an XML parser context
 * @info:  a node info sequence pointer
 *
 * Insert node info record into the sorted sequence
 */
void
xmlParserAddNodeInfo(xmlParserCtxtPtr ctxt,
                     const xmlParserNodeInfoPtr info)
{
    unsigned long pos;

    /* Find pos and check to see if node is already in the sequence */
    pos = xmlParserFindNodeInfoIndex(&ctxt->node_seq, (const xmlNodePtr)
                                     info->node);
    if (pos < ctxt->node_seq.length
        && ctxt->node_seq.buffer[pos].node == info->node) {
        ctxt->node_seq.buffer[pos] = *info;
    }

    /* Otherwise, we need to add new node to buffer */
    else {
        if (ctxt->node_seq.length + 1 > ctxt->node_seq.maximum) {
            xmlParserNodeInfo *tmp_buffer;
            unsigned int byte_size;

            if (ctxt->node_seq.maximum == 0)
                ctxt->node_seq.maximum = 2;
            byte_size = (sizeof(*ctxt->node_seq.buffer) *
			(2 * ctxt->node_seq.maximum));

            if (ctxt->node_seq.buffer == NULL)
                tmp_buffer = (xmlParserNodeInfo *) xmlMalloc(byte_size);
            else
                tmp_buffer =
                    (xmlParserNodeInfo *) xmlRealloc(ctxt->node_seq.buffer,
                                                     byte_size);

            if (tmp_buffer == NULL) {
                if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
                    ctxt->sax->error(ctxt->userData, "Out of memory\n");
                ctxt->errNo = XML_ERR_NO_MEMORY;
                return;
            }
            ctxt->node_seq.buffer = tmp_buffer;
            ctxt->node_seq.maximum *= 2;
        }

        /* If position is not at end, move elements out of the way */
        if (pos != ctxt->node_seq.length) {
            unsigned long i;

            for (i = ctxt->node_seq.length; i > pos; i--)
                ctxt->node_seq.buffer[i] = ctxt->node_seq.buffer[i - 1];
        }

        /* Copy element and increase length */
        ctxt->node_seq.buffer[pos] = *info;
        ctxt->node_seq.length++;
    }
}

/************************************************************************
 *									*
 *		Defaults settings					*
 *									*
 ************************************************************************/
/**
 * xmlPedanticParserDefault:
 * @val:  int 0 or 1 
 *
 * Set and return the previous value for enabling pedantic warnings.
 *
 * Returns the last value for 0 for no substitution, 1 for substitution.
 */

int
xmlPedanticParserDefault(int val) {
    int old = xmlPedanticParserDefaultValue;

    xmlPedanticParserDefaultValue = val;
    return(old);
}

/**
 * xmlLineNumbersDefault:
 * @val:  int 0 or 1 
 *
 * Set and return the previous value for enabling line numbers in elements
 * contents. This may break on old application and is turned off by default.
 *
 * Returns the last value for 0 for no substitution, 1 for substitution.
 */

int
xmlLineNumbersDefault(int val) {
    int old = xmlLineNumbersDefaultValue;

    xmlLineNumbersDefaultValue = val;
    return(old);
}

/**
 * xmlSubstituteEntitiesDefault:
 * @val:  int 0 or 1 
 *
 * Set and return the previous value for default entity support.
 * Initially the parser always keep entity references instead of substituting
 * entity values in the output. This function has to be used to change the
 * default parser behavior
 * SAX::substituteEntities() has to be used for changing that on a file by
 * file basis.
 *
 * Returns the last value for 0 for no substitution, 1 for substitution.
 */

int
xmlSubstituteEntitiesDefault(int val) {
    int old = xmlSubstituteEntitiesDefaultValue;

    xmlSubstituteEntitiesDefaultValue = val;
    return(old);
}

/**
 * xmlKeepBlanksDefault:
 * @val:  int 0 or 1 
 *
 * Set and return the previous value for default blanks text nodes support.
 * The 1.x version of the parser used an heuristic to try to detect
 * ignorable white spaces. As a result the SAX callback was generating
 * ignorableWhitespace() callbacks instead of characters() one, and when
 * using the DOM output text nodes containing those blanks were not generated.
 * The 2.x and later version will switch to the XML standard way and
 * ignorableWhitespace() are only generated when running the parser in
 * validating mode and when the current element doesn't allow CDATA or
 * mixed content.
 * This function is provided as a way to force the standard behavior 
 * on 1.X libs and to switch back to the old mode for compatibility when
 * running 1.X client code on 2.X . Upgrade of 1.X code should be done
 * by using xmlIsBlankNode() commodity function to detect the "empty"
 * nodes generated.
 * This value also affect autogeneration of indentation when saving code
 * if blanks sections are kept, indentation is not generated.
 *
 * Returns the last value for 0 for no substitution, 1 for substitution.
 */

int
xmlKeepBlanksDefault(int val) {
    int old = xmlKeepBlanksDefaultValue;

    xmlKeepBlanksDefaultValue = val;
    xmlIndentTreeOutput = !val;
    return(old);
}

/************************************************************************
 *									*
 *		Deprecated functions kept for compatibility		*
 *									*
 ************************************************************************/

/**
 * xmlCheckLanguageID:
 * @lang:  pointer to the string value
 *
 * Checks that the value conforms to the LanguageID production:
 *
 * NOTE: this is somewhat deprecated, those productions were removed from
 *       the XML Second edition.
 *
 * [33] LanguageID ::= Langcode ('-' Subcode)*
 * [34] Langcode ::= ISO639Code |  IanaCode |  UserCode
 * [35] ISO639Code ::= ([a-z] | [A-Z]) ([a-z] | [A-Z])
 * [36] IanaCode ::= ('i' | 'I') '-' ([a-z] | [A-Z])+
 * [37] UserCode ::= ('x' | 'X') '-' ([a-z] | [A-Z])+
 * [38] Subcode ::= ([a-z] | [A-Z])+
 *
 * Returns 1 if correct 0 otherwise
 **/
int
xmlCheckLanguageID(const xmlChar *lang) {
    const xmlChar *cur = lang;

    if (cur == NULL)
	return(0);
    if (((cur[0] == 'i') && (cur[1] == '-')) ||
	((cur[0] == 'I') && (cur[1] == '-'))) {
	/*
	 * IANA code
	 */
	cur += 2;
        while (((cur[0] >= 'A') && (cur[0] <= 'Z')) || /* non input consuming */
	       ((cur[0] >= 'a') && (cur[0] <= 'z')))
	    cur++;
    } else if (((cur[0] == 'x') && (cur[1] == '-')) ||
	       ((cur[0] == 'X') && (cur[1] == '-'))) {
	/*
	 * User code
	 */
	cur += 2;
        while (((cur[0] >= 'A') && (cur[0] <= 'Z')) || /* non input consuming */
	       ((cur[0] >= 'a') && (cur[0] <= 'z')))
	    cur++;
    } else if (((cur[0] >= 'A') && (cur[0] <= 'Z')) ||
	       ((cur[0] >= 'a') && (cur[0] <= 'z'))) {
	/*
	 * ISO639
	 */
	cur++;
        if (((cur[0] >= 'A') && (cur[0] <= 'Z')) ||
	    ((cur[0] >= 'a') && (cur[0] <= 'z')))
	    cur++;
	else
	    return(0);
    } else
	return(0);
    while (cur[0] != 0) { /* non input consuming */
	if (cur[0] != '-')
	    return(0);
	cur++;
        if (((cur[0] >= 'A') && (cur[0] <= 'Z')) ||
	    ((cur[0] >= 'a') && (cur[0] <= 'z')))
	    cur++;
	else
	    return(0);
        while (((cur[0] >= 'A') && (cur[0] <= 'Z')) || /* non input consuming */
	       ((cur[0] >= 'a') && (cur[0] <= 'z')))
	    cur++;
    }
    return(1);
}

/**
 * xmlDecodeEntities:
 * @ctxt:  the parser context
 * @len:  the len to decode (in bytes !), -1 for no size limit
 * @what:  combination of XML_SUBSTITUTE_REF and XML_SUBSTITUTE_PEREF
 * @end:  an end marker xmlChar, 0 if none
 * @end2:  an end marker xmlChar, 0 if none
 * @end3:  an end marker xmlChar, 0 if none
 * 
 * This function is deprecated, we now always process entities content
 * through xmlStringDecodeEntities
 *
 * TODO: remove it in next major release.
 *
 * [67] Reference ::= EntityRef | CharRef
 *
 * [69] PEReference ::= '%' Name ';'
 *
 * Returns A newly allocated string with the substitution done. The caller
 *      must deallocate it !
 */
xmlChar *
xmlDecodeEntities(xmlParserCtxtPtr ctxt ATTRIBUTE_UNUSED, int len ATTRIBUTE_UNUSED, int what ATTRIBUTE_UNUSED,
	      xmlChar end ATTRIBUTE_UNUSED, xmlChar  end2 ATTRIBUTE_UNUSED, xmlChar end3 ATTRIBUTE_UNUSED) {
#if 0
    xmlChar *buffer = NULL;
    unsigned int buffer_size = 0;
    unsigned int nbchars = 0;

    xmlChar *current = NULL;
    xmlEntityPtr ent;
    unsigned int max = (unsigned int) len;
    int c,l;
#endif

    static int deprecated = 0;
    if (!deprecated) {
	xmlGenericError(xmlGenericErrorContext,
		"xmlDecodeEntities() deprecated function reached\n");
	deprecated = 1;
    }

#if 0
    if (ctxt->depth > 40) {
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
		"Detected entity reference loop\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	ctxt->errNo = XML_ERR_ENTITY_LOOP;
	return(NULL);
    }

    /*
     * allocate a translation buffer.
     */
    buffer_size = XML_PARSER_BIG_BUFFER_SIZE;
    buffer = (xmlChar *) xmlMalloc(buffer_size * sizeof(xmlChar));
    if (buffer == NULL) {
	xmlGenericError(xmlGenericErrorContext, 
		        "xmlDecodeEntities: malloc failed");
	return(NULL);
    }

    /*
     * OK loop until we reach one of the ending char or a size limit.
     */
    GROW;
    c = CUR_CHAR(l);
    while ((nbchars < max) && (c != end) && /* NOTUSED */
           (c != end2) && (c != end3)) {
	GROW;
	if (c == 0) break;
        if ((c == '&') && (NXT(1) == '#')) {
	    int val = xmlParseCharRef(ctxt);
	    COPY_BUF(0,buffer,nbchars,val);
	    NEXTL(l);
	} else if (c == '&') &&
		   (what & XML_SUBSTITUTE_REF)) {
	    if (xmlParserDebugEntities)
		xmlGenericError(xmlGenericErrorContext,
			"decoding Entity Reference\n");
	    ent = xmlParseEntityRef(ctxt);
	    if ((ent != NULL) && 
		(ctxt->replaceEntities != 0)) {
		current = ent->content;
		while (*current != 0) { /* non input consuming loop */
		    buffer[nbchars++] = *current++;
		    if (nbchars > buffer_size - XML_PARSER_BUFFER_SIZE) {
			growBuffer(buffer);
		    }
		}
	    } else if (ent != NULL) {
		const xmlChar *cur = ent->name;

		buffer[nbchars++] = '&';
		if (nbchars > buffer_size - XML_PARSER_BUFFER_SIZE) {
		    growBuffer(buffer);
		}
		while (*cur != 0) { /* non input consuming loop */
		    buffer[nbchars++] = *cur++;
		}
		buffer[nbchars++] = ';';
	    }
	} else if (c == '%' && (what & XML_SUBSTITUTE_PEREF)) {
	    /*
	     * a PEReference induce to switch the entity flow,
	     * we break here to flush the current set of chars
	     * parsed if any. We will be called back later.
	     */
	    if (xmlParserDebugEntities)
		xmlGenericError(xmlGenericErrorContext,
			"decoding PE Reference\n");
	    if (nbchars != 0) break;

	    xmlParsePEReference(ctxt);

	    /*
	     * Pop-up of finished entities.
	     */
	    while ((RAW == 0) && (ctxt->inputNr > 1)) /* non input consuming */
		xmlPopInput(ctxt);

	    break;
	} else {
	    COPY_BUF(l,buffer,nbchars,c);
	    NEXTL(l);
	    if (nbchars > buffer_size - XML_PARSER_BUFFER_SIZE) {
	      growBuffer(buffer);
	    }
	}
	c = CUR_CHAR(l);
    }
    buffer[nbchars++] = 0;
    return(buffer);
#endif
    return(NULL);
}

/**
 * xmlNamespaceParseNCName:
 * @ctxt:  an XML parser context
 *
 * parse an XML namespace name.
 *
 * TODO: this seems not in use anymore, the namespace handling is done on
 *       top of the SAX interfaces, i.e. not on raw input.
 *
 * [NS 3] NCName ::= (Letter | '_') (NCNameChar)*
 *
 * [NS 4] NCNameChar ::= Letter | Digit | '.' | '-' | '_' |
 *                       CombiningChar | Extender
 *
 * Returns the namespace name or NULL
 */

xmlChar *
xmlNamespaceParseNCName(xmlParserCtxtPtr ctxt ATTRIBUTE_UNUSED) {
#if 0
    xmlChar buf[XML_MAX_NAMELEN + 5];
    int len = 0, l;
    int cur = CUR_CHAR(l);
#endif

    static int deprecated = 0;
    if (!deprecated) {
	xmlGenericError(xmlGenericErrorContext,
		"xmlNamespaceParseNCName() deprecated function reached\n");
	deprecated = 1;
    }

#if 0
    /* load first the value of the char !!! */
    GROW;
    if (!IS_LETTER(cur) && (cur != '_')) return(NULL);

xmlGenericError(xmlGenericErrorContext,
	"xmlNamespaceParseNCName: reached loop 3\n");
    while ((IS_LETTER(cur)) || (IS_DIGIT(cur)) || /* NOT REACHED */
           (cur == '.') || (cur == '-') ||
	   (cur == '_') ||
	   (IS_COMBINING(cur)) ||
	   (IS_EXTENDER(cur))) {
	COPY_BUF(l,buf,len,cur);
	NEXTL(l);
	cur = CUR_CHAR(l);
	if (len >= XML_MAX_NAMELEN) {
	    xmlGenericError(xmlGenericErrorContext, 
	       "xmlNamespaceParseNCName: reached XML_MAX_NAMELEN limit\n");
	    while ((IS_LETTER(cur)) || (IS_DIGIT(cur)) ||/* NOT REACHED */
		   (cur == '.') || (cur == '-') ||
		   (cur == '_') ||
		   (IS_COMBINING(cur)) ||
		   (IS_EXTENDER(cur))) {
		NEXTL(l);
		cur = CUR_CHAR(l);
	    }
	    break;
	}
    }
    return(xmlStrndup(buf, len));
#endif
    return(NULL);
}

/**
 * xmlNamespaceParseQName:
 * @ctxt:  an XML parser context
 * @prefix:  a xmlChar ** 
 *
 * TODO: this seems not in use anymore, the namespace handling is done on
 *       top of the SAX interfaces, i.e. not on raw input.
 *
 * parse an XML qualified name
 *
 * [NS 5] QName ::= (Prefix ':')? LocalPart
 *
 * [NS 6] Prefix ::= NCName
 *
 * [NS 7] LocalPart ::= NCName
 *
 * Returns the local part, and prefix is updated
 *   to get the Prefix if any.
 */

xmlChar *
xmlNamespaceParseQName(xmlParserCtxtPtr ctxt ATTRIBUTE_UNUSED, xmlChar **prefix ATTRIBUTE_UNUSED) {

    static int deprecated = 0;
    if (!deprecated) {
	xmlGenericError(xmlGenericErrorContext,
		"xmlNamespaceParseQName() deprecated function reached\n");
	deprecated = 1;
    }

#if 0
    xmlChar *ret = NULL;

    *prefix = NULL;
    ret = xmlNamespaceParseNCName(ctxt);
    if (RAW == ':') {
        *prefix = ret;
	NEXT;
	ret = xmlNamespaceParseNCName(ctxt);
    }

    return(ret);
#endif
    return(NULL);
}

/**
 * xmlNamespaceParseNSDef:
 * @ctxt:  an XML parser context
 *
 * parse a namespace prefix declaration
 *
 * TODO: this seems not in use anymore, the namespace handling is done on
 *       top of the SAX interfaces, i.e. not on raw input.
 *
 * [NS 1] NSDef ::= PrefixDef Eq SystemLiteral
 *
 * [NS 2] PrefixDef ::= 'xmlns' (':' NCName)?
 *
 * Returns the namespace name
 */

xmlChar *
xmlNamespaceParseNSDef(xmlParserCtxtPtr ctxt ATTRIBUTE_UNUSED) {
    static int deprecated = 0;
    if (!deprecated) {
	xmlGenericError(xmlGenericErrorContext,
		"xmlNamespaceParseNSDef() deprecated function reached\n");
	deprecated = 1;
    }
    return(NULL);
#if 0
    xmlChar *name = NULL;

    if ((RAW == 'x') && (NXT(1) == 'm') &&
        (NXT(2) == 'l') && (NXT(3) == 'n') &&
	(NXT(4) == 's')) {
	SKIP(5);
	if (RAW == ':') {
	    NEXT;
	    name = xmlNamespaceParseNCName(ctxt);
	}
    }
    return(name);
#endif
}

/**
 * xmlParseQuotedString:
 * @ctxt:  an XML parser context
 *
 * Parse and return a string between quotes or doublequotes
 *
 * TODO: Deprecated, to  be removed at next drop of binary compatibility
 *
 * Returns the string parser or NULL.
 */
xmlChar *
xmlParseQuotedString(xmlParserCtxtPtr ctxt ATTRIBUTE_UNUSED) {
    static int deprecated = 0;
    if (!deprecated) {
	xmlGenericError(xmlGenericErrorContext,
		"xmlParseQuotedString() deprecated function reached\n");
	deprecated = 1;
    }
    return(NULL);

#if 0
    xmlChar *buf = NULL;
    int len = 0,l;
    int size = XML_PARSER_BUFFER_SIZE;
    int c;

    buf = (xmlChar *) xmlMalloc(size * sizeof(xmlChar));
    if (buf == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		"malloc of %d byte failed\n", size);
	return(NULL);
    }
xmlGenericError(xmlGenericErrorContext,
	"xmlParseQuotedString: reached loop 4\n");
    if (RAW == '"') {
        NEXT;
	c = CUR_CHAR(l);
	while (IS_CHAR(c) && (c != '"')) { /* NOTUSED */
	    if (len + 5 >= size) {
		size *= 2;
		buf = (xmlChar *) xmlRealloc(buf, size * sizeof(xmlChar));
		if (buf == NULL) {
		    xmlGenericError(xmlGenericErrorContext,
			    "realloc of %d byte failed\n", size);
		    return(NULL);
		}
	    }
	    COPY_BUF(l,buf,len,c);
	    NEXTL(l);
	    c = CUR_CHAR(l);
	}
	if (c != '"') {
	    ctxt->errNo = XML_ERR_STRING_NOT_CLOSED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData, 
			         "String not closed \"%.50s\"\n", buf);
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
        } else {
	    NEXT;
	}
    } else if (RAW == '\''){
        NEXT;
	c = CUR;
	while (IS_CHAR(c) && (c != '\'')) { /* NOTUSED */
	    if (len + 1 >= size) {
		size *= 2;
		buf = (xmlChar *) xmlRealloc(buf, size * sizeof(xmlChar));
		if (buf == NULL) {
		    xmlGenericError(xmlGenericErrorContext,
			    "realloc of %d byte failed\n", size);
		    return(NULL);
		}
	    }
	    buf[len++] = c;
	    NEXT;
	    c = CUR;
	}
	if (RAW != '\'') {
	    ctxt->errNo = XML_ERR_STRING_NOT_CLOSED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData,
			         "String not closed \"%.50s\"\n", buf);
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
        } else {
	    NEXT;
	}
    }
    return(buf);
#endif
}

/**
 * xmlParseNamespace:
 * @ctxt:  an XML parser context
 *
 * xmlParseNamespace: parse specific PI '<?namespace ...' constructs.
 *
 * This is what the older xml-name Working Draft specified, a bunch of
 * other stuff may still rely on it, so support is still here as
 * if it was declared on the root of the Tree:-(
 *
 * TODO: remove from library
 *
 * To be removed at next drop of binary compatibility
 */

void
xmlParseNamespace(xmlParserCtxtPtr ctxt ATTRIBUTE_UNUSED) {
    static int deprecated = 0;
    if (!deprecated) {
	xmlGenericError(xmlGenericErrorContext,
		"xmlParseNamespace() deprecated function reached\n");
	deprecated = 1;
    }

#if 0
    xmlChar *href = NULL;
    xmlChar *prefix = NULL;
    int garbage = 0;

    /*
     * We just skipped "namespace" or "xml:namespace"
     */
    SKIP_BLANKS;

xmlGenericError(xmlGenericErrorContext,
	"xmlParseNamespace: reached loop 5\n");
    while (IS_CHAR(RAW) && (RAW != '>')) { /* NOT REACHED */
	/*
	 * We can have "ns" or "prefix" attributes
	 * Old encoding as 'href' or 'AS' attributes is still supported
	 */
	if ((RAW == 'n') && (NXT(1) == 's')) {
	    garbage = 0;
	    SKIP(2);
	    SKIP_BLANKS;

	    if (RAW != '=') continue;
	    NEXT;
	    SKIP_BLANKS;

	    href = xmlParseQuotedString(ctxt);
	    SKIP_BLANKS;
	} else if ((RAW == 'h') && (NXT(1) == 'r') &&
	    (NXT(2) == 'e') && (NXT(3) == 'f')) {
	    garbage = 0;
	    SKIP(4);
	    SKIP_BLANKS;

	    if (RAW != '=') continue;
	    NEXT;
	    SKIP_BLANKS;

	    href = xmlParseQuotedString(ctxt);
	    SKIP_BLANKS;
	} else if ((RAW == 'p') && (NXT(1) == 'r') &&
	           (NXT(2) == 'e') && (NXT(3) == 'f') &&
	           (NXT(4) == 'i') && (NXT(5) == 'x')) {
	    garbage = 0;
	    SKIP(6);
	    SKIP_BLANKS;

	    if (RAW != '=') continue;
	    NEXT;
	    SKIP_BLANKS;

	    prefix = xmlParseQuotedString(ctxt);
	    SKIP_BLANKS;
	} else if ((RAW == 'A') && (NXT(1) == 'S')) {
	    garbage = 0;
	    SKIP(2);
	    SKIP_BLANKS;

	    if (RAW != '=') continue;
	    NEXT;
	    SKIP_BLANKS;

	    prefix = xmlParseQuotedString(ctxt);
	    SKIP_BLANKS;
	} else if ((RAW == '?') && (NXT(1) == '>')) {
	    garbage = 0;
	    NEXT;
	} else {
            /*
	     * Found garbage when parsing the namespace
	     */
	    if (!garbage) {
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
		                     "xmlParseNamespace found garbage\n");
	    }
	    ctxt->errNo = XML_ERR_NS_DECL_ERROR;
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
            NEXT;
        }
    }

    MOVETO_ENDTAG(CUR_PTR);
    NEXT;

    /*
     * Register the DTD.
    if (href != NULL)
	if ((ctxt->sax != NULL) && (ctxt->sax->globalNamespace != NULL))
	    ctxt->sax->globalNamespace(ctxt->userData, href, prefix);
     */

    if (prefix != NULL) xmlFree(prefix);
    if (href != NULL) xmlFree(href);
#endif
}

/**
 * xmlScanName:
 * @ctxt:  an XML parser context
 *
 * Trickery: parse an XML name but without consuming the input flow
 * Needed for rollback cases. Used only when parsing entities references.
 *
 * TODO: seems deprecated now, only used in the default part of
 *       xmlParserHandleReference
 *
 * [4] NameChar ::= Letter | Digit | '.' | '-' | '_' | ':' |
 *                  CombiningChar | Extender
 *
 * [5] Name ::= (Letter | '_' | ':') (NameChar)*
 *
 * [6] Names ::= Name (S Name)*
 *
 * Returns the Name parsed or NULL
 */

xmlChar *
xmlScanName(xmlParserCtxtPtr ctxt ATTRIBUTE_UNUSED) {
    static int deprecated = 0;
    if (!deprecated) {
	xmlGenericError(xmlGenericErrorContext,
		"xmlScanName() deprecated function reached\n");
	deprecated = 1;
    }
    return(NULL);

#if 0
    xmlChar buf[XML_MAX_NAMELEN];
    int len = 0;

    GROW;
    if (!IS_LETTER(RAW) && (RAW != '_') &&
        (RAW != ':')) {
	return(NULL);
    }


    while ((IS_LETTER(NXT(len))) || (IS_DIGIT(NXT(len))) || /* NOT REACHED */
           (NXT(len) == '.') || (NXT(len) == '-') ||
	   (NXT(len) == '_') || (NXT(len) == ':') || 
	   (IS_COMBINING(NXT(len))) ||
	   (IS_EXTENDER(NXT(len)))) {
	GROW;
	buf[len] = NXT(len);
	len++;
	if (len >= XML_MAX_NAMELEN) {
	    xmlGenericError(xmlGenericErrorContext, 
	       "xmlScanName: reached XML_MAX_NAMELEN limit\n");
	    while ((IS_LETTER(NXT(len))) || /* NOT REACHED */
		   (IS_DIGIT(NXT(len))) ||
		   (NXT(len) == '.') || (NXT(len) == '-') ||
		   (NXT(len) == '_') || (NXT(len) == ':') || 
		   (IS_COMBINING(NXT(len))) ||
		   (IS_EXTENDER(NXT(len))))
		 len++;
	    break;
	}
    }
    return(xmlStrndup(buf, len));
#endif
}

/**
 * xmlParserHandleReference:
 * @ctxt:  the parser context
 * 
 * TODO: Remove, now deprecated ... the test is done directly in the
 *       content parsing
 * routines.
 *
 * [67] Reference ::= EntityRef | CharRef
 *
 * [68] EntityRef ::= '&' Name ';'
 *
 * [ WFC: Entity Declared ]
 * the Name given in the entity reference must match that in an entity
 * declaration, except that well-formed documents need not declare any
 * of the following entities: amp, lt, gt, apos, quot. 
 *
 * [ WFC: Parsed Entity ]
 * An entity reference must not contain the name of an unparsed entity
 *
 * [66] CharRef ::= '&#' [0-9]+ ';' |
 *                  '&#x' [0-9a-fA-F]+ ';'
 *
 * A PEReference may have been detected in the current input stream
 * the handling is done accordingly to 
 *      http://www.w3.org/TR/REC-xml#entproc
 */
void
xmlParserHandleReference(xmlParserCtxtPtr ctxt ATTRIBUTE_UNUSED) {
    static int deprecated = 0;
    if (!deprecated) {
	xmlGenericError(xmlGenericErrorContext,
		"xmlParserHandleReference() deprecated function reached\n");
	deprecated = 1;
    }

    return;
}

/**
 * xmlHandleEntity:
 * @ctxt:  an XML parser context
 * @entity:  an XML entity pointer.
 *
 * Default handling of defined entities, when should we define a new input
 * stream ? When do we just handle that as a set of chars ?
 *
 * OBSOLETE: to be removed at some point.
 */

void
xmlHandleEntity(xmlParserCtxtPtr ctxt ATTRIBUTE_UNUSED, xmlEntityPtr entity ATTRIBUTE_UNUSED) {
    static int deprecated = 0;
    if (!deprecated) {
	xmlGenericError(xmlGenericErrorContext,
		"xmlHandleEntity() deprecated function reached\n");
	deprecated = 1;
    }

#if 0
    int len;
    xmlParserInputPtr input;

    if (entity->content == NULL) {
	ctxt->errNo = XML_ERR_INTERNAL_ERROR;
        if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, "xmlHandleEntity %s: content == NULL\n",
	               entity->name);
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
        return;
    }
    len = xmlStrlen(entity->content);
    if (len <= 2) goto handle_as_char;

    /*
     * Redefine its content as an input stream.
     */
    input = xmlNewEntityInputStream(ctxt, entity);
    xmlPushInput(ctxt, input);
    return;

handle_as_char:
    /*
     * Just handle the content as a set of chars.
     */
    if ((ctxt->sax != NULL) && (!ctxt->disableSAX) &&
	(ctxt->sax->characters != NULL))
	ctxt->sax->characters(ctxt->userData, entity->content, len);
#endif
}

/**
 * xmlNewGlobalNs:
 * @doc:  the document carrying the namespace
 * @href:  the URI associated
 * @prefix:  the prefix for the namespace
 *
 * Creation of a Namespace, the old way using PI and without scoping
 *   DEPRECATED !!!
 * It now create a namespace on the root element of the document if found.
 * Returns NULL this functionality had been removed
 */
xmlNsPtr
xmlNewGlobalNs(xmlDocPtr doc ATTRIBUTE_UNUSED, const xmlChar *href ATTRIBUTE_UNUSED,
	       const xmlChar *prefix ATTRIBUTE_UNUSED) {
    static int deprecated = 0;
    if (!deprecated) {
	xmlGenericError(xmlGenericErrorContext,
		"xmlNewGlobalNs() deprecated function reached\n");
	deprecated = 1;
    }
    return(NULL);
#if 0
    xmlNodePtr root;

    xmlNsPtr cur;
 
    root = xmlDocGetRootElement(doc);
    if (root != NULL)
	return(xmlNewNs(root, href, prefix));
	
    /*
     * if there is no root element yet, create an old Namespace type
     * and it will be moved to the root at save time.
     */
    cur = (xmlNsPtr) xmlMalloc(sizeof(xmlNs));
    if (cur == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewGlobalNs : malloc failed\n");
	return(NULL);
    }
    memset(cur, 0, sizeof(xmlNs));
    cur->type = XML_GLOBAL_NAMESPACE;

    if (href != NULL)
	cur->href = xmlStrdup(href); 
    if (prefix != NULL)
	cur->prefix = xmlStrdup(prefix); 

    /*
     * Add it at the end to preserve parsing order ...
     */
    if (doc != NULL) {
	if (doc->oldNs == NULL) {
	    doc->oldNs = cur;
	} else {
	    xmlNsPtr prev = doc->oldNs;

	    while (prev->next != NULL) prev = prev->next;
	    prev->next = cur;
	}
    }

  return(NULL);
#endif
}

/**
 * xmlUpgradeOldNs:
 * @doc:  a document pointer
 * 
 * Upgrade old style Namespaces (PI) and move them to the root of the document.
 * DEPRECATED
 */
void
xmlUpgradeOldNs(xmlDocPtr doc ATTRIBUTE_UNUSED) {
    static int deprecated = 0;
    if (!deprecated) {
	xmlGenericError(xmlGenericErrorContext,
		"xmlUpgradeOldNs() deprecated function reached\n");
	deprecated = 1;
    }
#if 0
    xmlNsPtr cur;

    if ((doc == NULL) || (doc->oldNs == NULL)) return;
    if (doc->children == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlUpgradeOldNs: failed no root !\n");
#endif
	return;
    }

    cur = doc->oldNs;
    while (cur->next != NULL) {
	cur->type = XML_LOCAL_NAMESPACE;
        cur = cur->next;
    }
    cur->type = XML_LOCAL_NAMESPACE;
    cur->next = doc->children->nsDef;
    doc->children->nsDef = doc->oldNs;
    doc->oldNs = NULL;
#endif
}

