#include <libxml/parser.h>

#ifndef __XML_ERROR_H__
#define __XML_ERROR_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    XML_ERR_OK = 0,
    XML_ERR_INTERNAL_ERROR,
    XML_ERR_NO_MEMORY,
    
    XML_ERR_DOCUMENT_START, /* 3 */
    XML_ERR_DOCUMENT_EMPTY,
    XML_ERR_DOCUMENT_END,

    XML_ERR_INVALID_HEX_CHARREF, /* 6 */
    XML_ERR_INVALID_DEC_CHARREF,
    XML_ERR_INVALID_CHARREF,
    XML_ERR_INVALID_CHAR,

    XML_ERR_CHARREF_AT_EOF, /* 10 */
    XML_ERR_CHARREF_IN_PROLOG,
    XML_ERR_CHARREF_IN_EPILOG,
    XML_ERR_CHARREF_IN_DTD,
    XML_ERR_ENTITYREF_AT_EOF,
    XML_ERR_ENTITYREF_IN_PROLOG,
    XML_ERR_ENTITYREF_IN_EPILOG,
    XML_ERR_ENTITYREF_IN_DTD,
    XML_ERR_PEREF_AT_EOF,
    XML_ERR_PEREF_IN_PROLOG,
    XML_ERR_PEREF_IN_EPILOG,
    XML_ERR_PEREF_IN_INT_SUBSET,

    XML_ERR_ENTITYREF_NO_NAME, /* 22 */
    XML_ERR_ENTITYREF_SEMICOL_MISSING,

    XML_ERR_PEREF_NO_NAME, /* 24 */
    XML_ERR_PEREF_SEMICOL_MISSING,

    XML_ERR_UNDECLARED_ENTITY, /* 26 */
    XML_WAR_UNDECLARED_ENTITY,
    XML_ERR_UNPARSED_ENTITY,
    XML_ERR_ENTITY_IS_EXTERNAL,
    XML_ERR_ENTITY_IS_PARAMETER,

    XML_ERR_UNKNOWN_ENCODING, /* 31 */
    XML_ERR_UNSUPPORTED_ENCODING,

    XML_ERR_STRING_NOT_STARTED, /* 33 */
    XML_ERR_STRING_NOT_CLOSED,
    XML_ERR_NS_DECL_ERROR,

    XML_ERR_ENTITY_NOT_STARTED, /* 36 */
    XML_ERR_ENTITY_NOT_FINISHED,
    
    XML_ERR_LT_IN_ATTRIBUTE, /* 38 */
    XML_ERR_ATTRIBUTE_NOT_STARTED,
    XML_ERR_ATTRIBUTE_NOT_FINISHED,
    XML_ERR_ATTRIBUTE_WITHOUT_VALUE,
    XML_ERR_ATTRIBUTE_REDEFINED,

    XML_ERR_LITERAL_NOT_STARTED, /* 43 */
    XML_ERR_LITERAL_NOT_FINISHED,
    
    XML_ERR_COMMENT_NOT_FINISHED, /* 45 */

    XML_ERR_PI_NOT_STARTED, /* 47 */
    XML_ERR_PI_NOT_FINISHED,

    XML_ERR_NOTATION_NOT_STARTED, /* 49 */
    XML_ERR_NOTATION_NOT_FINISHED,

    XML_ERR_ATTLIST_NOT_STARTED, /* 51 */
    XML_ERR_ATTLIST_NOT_FINISHED,

    XML_ERR_MIXED_NOT_STARTED, /* 53 */
    XML_ERR_MIXED_NOT_FINISHED,

    XML_ERR_ELEMCONTENT_NOT_STARTED, /* 55 */
    XML_ERR_ELEMCONTENT_NOT_FINISHED,

    XML_ERR_XMLDECL_NOT_STARTED, /* 57 */
    XML_ERR_XMLDECL_NOT_FINISHED,

    XML_ERR_CONDSEC_NOT_STARTED, /* 59 */
    XML_ERR_CONDSEC_NOT_FINISHED,

    XML_ERR_EXT_SUBSET_NOT_FINISHED, /* 61 */

    XML_ERR_DOCTYPE_NOT_FINISHED, /* 62 */

    XML_ERR_MISPLACED_CDATA_END, /* 63 */
    XML_ERR_CDATA_NOT_FINISHED,

    XML_ERR_RESERVED_XML_NAME, /* 65 */

    XML_ERR_SPACE_REQUIRED, /* 66 */
    XML_ERR_SEPARATOR_REQUIRED,
    XML_ERR_NMTOKEN_REQUIRED,
    XML_ERR_NAME_REQUIRED,
    XML_ERR_PCDATA_REQUIRED,
    XML_ERR_URI_REQUIRED,
    XML_ERR_PUBID_REQUIRED,
    XML_ERR_LT_REQUIRED,
    XML_ERR_GT_REQUIRED,
    XML_ERR_LTSLASH_REQUIRED,
    XML_ERR_EQUAL_REQUIRED,

    XML_ERR_TAG_NAME_MISMATCH, /* 77 */
    XML_ERR_TAG_NOT_FINISHED,

    XML_ERR_STANDALONE_VALUE, /* 79 */

    XML_ERR_ENCODING_NAME, /* 80 */

    XML_ERR_HYPHEN_IN_COMMENT, /* 81 */

    XML_ERR_INVALID_ENCODING, /* 82 */

    XML_ERR_EXT_ENTITY_STANDALONE, /* 83 */

    XML_ERR_CONDSEC_INVALID, /* 84 */

    XML_ERR_VALUE_REQUIRED, /* 85 */

    XML_ERR_NOT_WELL_BALANCED, /* 86 */
    XML_ERR_EXTRA_CONTENT, /* 87 */
    XML_ERR_ENTITY_CHAR_ERROR, /* 88 */
    XML_ERR_ENTITY_PE_INTERNAL, /* 88 */
    XML_ERR_ENTITY_LOOP, /* 89 */
    XML_ERR_ENTITY_BOUNDARY, /* 90 */
    XML_ERR_INVALID_URI, /* 91 */
    XML_ERR_URI_FRAGMENT, /* 92 */
    XML_WAR_CATALOG_PI, /* 93 */
    XML_ERR_NO_DTD  /* 94 */
}xmlParserErrors;

/**
 * xmlGenericErrorFunc:
 * @ctx:  a parsing context
 * @msg:  the message
 * @...:  the extra arguments of the varags to format the message
 *
 * Signature of the function to use when there is an error and
 * no parsing or validity context available .
 */
typedef void (*xmlGenericErrorFunc) (void *ctx,
				 const char *msg,
				 ...);

/*
 * Use the following function to reset the two global variables
 * xmlGenericError and xmlGenericErrorContext.
 */
void	xmlSetGenericErrorFunc	(void *ctx,
				 xmlGenericErrorFunc handler);
void	initGenericErrorDefaultFunc(xmlGenericErrorFunc *handler);

/*
 * Default message routines used by SAX and Valid context for error
 * and warning reporting.
 */
void	xmlParserError		(void *ctx,
				 const char *msg,
				 ...);
void	xmlParserWarning	(void *ctx,
				 const char *msg,
				 ...);
void	xmlParserValidityError	(void *ctx,
				 const char *msg,
				 ...);
void	xmlParserValidityWarning(void *ctx,
				 const char *msg,
				 ...);
void	xmlParserPrintFileInfo	(xmlParserInputPtr input);
void	xmlParserPrintFileContext(xmlParserInputPtr input);

#ifdef __cplusplus
}
#endif
#endif /* __XML_ERROR_H__ */
