/*
 * parserInternals.h : internals routines exported by the parser.
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 *
 */

#ifndef __XML_PARSER_INTERNALS_H__
#define __XML_PARSER_INTERNALS_H__

#include <libxml/parser.h>
#include <libxml/HTMLparser.h>

#ifdef __cplusplus
extern "C" {
#endif

 /**
  * XML_MAX_NAMELEN:
  *
  * Identifiers can be longer, but this will be more costly
  * at runtime.
  */
#define XML_MAX_NAMELEN 100

/**
 * INPUT_CHUNK:
 *
 * The parser tries to always have that amount of input ready.
 * One of the point is providing context when reporting errors.
 */
#define INPUT_CHUNK	250

/************************************************************************
 *									*
 * UNICODE version of the macros.      					*
 *									*
 ************************************************************************/
/**
 * IS_CHAR:
 * @c:  an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 * [2] Char ::= #x9 | #xA | #xD | [#x20-#xD7FF] | [#xE000-#xFFFD]
 *                  | [#x10000-#x10FFFF]
 * any Unicode character, excluding the surrogate blocks, FFFE, and FFFF.
 */
#define IS_CHAR(c)							\
    ((((c) >= 0x20) && ((c) <= 0xD7FF)) ||				\
     ((c) == 0x09) || ((c) == 0x0A) || ((c) == 0x0D) ||			\
     (((c) >= 0xE000) && ((c) <= 0xFFFD)) ||				\
     (((c) >= 0x10000) && ((c) <= 0x10FFFF)))

/**
 * IS_BLANK:
 * @c:  an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 * [3] S ::= (#x20 | #x9 | #xD | #xA)+
 */
#define IS_BLANK(c) (((c) == 0x20) || ((c) == 0x09) || ((c) == 0xA) ||	\
                     ((c) == 0x0D))

/**
 * IS_BASECHAR:
 * @c:  an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 * [85] BaseChar ::= ... long list see REC ...
 */
#define IS_BASECHAR(c) xmlIsBaseChar(c)

/**
 * IS_DIGIT:
 * @c:  an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 * [88] Digit ::= ... long list see REC ...
 */
#define IS_DIGIT(c) xmlIsDigit(c)

/**
 * IS_COMBINING:
 * @c:  an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 * [87] CombiningChar ::= ... long list see REC ...
 */
#define IS_COMBINING(c) xmlIsCombining(c)

/**
 * IS_EXTENDER:
 * @c:  an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 *
 * [89] Extender ::= #x00B7 | #x02D0 | #x02D1 | #x0387 | #x0640 |
 *                   #x0E46 | #x0EC6 | #x3005 | [#x3031-#x3035] |
 *                   [#x309D-#x309E] | [#x30FC-#x30FE]
 */
#define IS_EXTENDER(c) xmlIsExtender(c)

/**
 * IS_IDEOGRAPHIC:
 * @c:  an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 *
 * [86] Ideographic ::= [#x4E00-#x9FA5] | #x3007 | [#x3021-#x3029]
 */
#define IS_IDEOGRAPHIC(c) xmlIsIdeographic(c)

/**
 * IS_LETTER:
 * @c:  an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 *
 * [84] Letter ::= BaseChar | Ideographic 
 */
#define IS_LETTER(c) (IS_BASECHAR(c) || IS_IDEOGRAPHIC(c))


/**
 * IS_PUBIDCHAR:
 * @c:  an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 *
 * [13] PubidChar ::= #x20 | #xD | #xA | [a-zA-Z0-9] | [-'()+,./:=?;!*#@$_%]
 */
#define IS_PUBIDCHAR(c)	xmlIsPubidChar(c)

/**
 * SKIP_EOL:
 * @p:  and UTF8 string pointer
 *
 * Skips the end of line chars.
 */
#define SKIP_EOL(p) 							\
    if (*(p) == 0x13) { p++ ; if (*(p) == 0x10) p++; }			\
    if (*(p) == 0x10) { p++ ; if (*(p) == 0x13) p++; }

/**
 * MOVETO_ENDTAG:
 * @p:  and UTF8 string pointer
 *
 * Skips to the next '>' char.
 */
#define MOVETO_ENDTAG(p)						\
    while ((*p) && (*(p) != '>')) (p)++

/**
 * MOVETO_STARTTAG:
 * @p:  and UTF8 string pointer
 *
 * Skips to the next '<' char.
 */
#define MOVETO_STARTTAG(p)						\
    while ((*p) && (*(p) != '<')) (p)++

/**
 * Global variables used for predefined strings.
 */
LIBXML_DLL_IMPORT extern const xmlChar xmlStringText[];
LIBXML_DLL_IMPORT extern const xmlChar xmlStringTextNoenc[];
LIBXML_DLL_IMPORT extern const xmlChar xmlStringComment[];

/*
 * Function to finish the work of the macros where needed.
 */
int			xmlIsBaseChar	(int c);
int			xmlIsBlank	(int c);
int			xmlIsPubidChar	(int c);
int			xmlIsLetter	(int c);
int			xmlIsDigit	(int c);
int			xmlIsIdeographic(int c);
int			xmlIsExtender	(int c);
int			xmlIsCombining	(int c);
int			xmlIsChar	(int c);

/**
 * Parser context.
 */
xmlParserCtxtPtr	xmlCreateFileParserCtxt	(const char *filename);
xmlParserCtxtPtr	xmlCreateMemoryParserCtxt(const char *buffer,
						 int size);
xmlParserCtxtPtr	xmlNewParserCtxt	(void);
xmlParserCtxtPtr	xmlCreateEntityParserCtxt(const xmlChar *URL,
						 const xmlChar *ID,
						 const xmlChar *base);
int			xmlSwitchEncoding	(xmlParserCtxtPtr ctxt,
						 xmlCharEncoding enc);
int			xmlSwitchToEncoding	(xmlParserCtxtPtr ctxt,
					     xmlCharEncodingHandlerPtr handler);

/**
 * Entities
 */
void			xmlHandleEntity		(xmlParserCtxtPtr ctxt,
						 xmlEntityPtr entity);

/**
 * Input Streams.
 */
xmlParserInputPtr	xmlNewStringInputStream	(xmlParserCtxtPtr ctxt,
						 const xmlChar *buffer);
xmlParserInputPtr	xmlNewEntityInputStream	(xmlParserCtxtPtr ctxt,
						 xmlEntityPtr entity);
void			xmlPushInput		(xmlParserCtxtPtr ctxt,
						 xmlParserInputPtr input);
xmlChar			xmlPopInput		(xmlParserCtxtPtr ctxt);
void			xmlFreeInputStream	(xmlParserInputPtr input);
xmlParserInputPtr	xmlNewInputFromFile	(xmlParserCtxtPtr ctxt,
						 const char *filename);
xmlParserInputPtr	xmlNewInputStream	(xmlParserCtxtPtr ctxt);

/**
 * Namespaces.
 */
xmlChar *		xmlSplitQName		(xmlParserCtxtPtr ctxt,
						 const xmlChar *name,
						 xmlChar **prefix);
xmlChar *		xmlNamespaceParseNCName	(xmlParserCtxtPtr ctxt);
xmlChar *		xmlNamespaceParseQName	(xmlParserCtxtPtr ctxt,
						 xmlChar **prefix);
xmlChar *		xmlNamespaceParseNSDef	(xmlParserCtxtPtr ctxt);
xmlChar *		xmlParseQuotedString	(xmlParserCtxtPtr ctxt);
void			xmlParseNamespace	(xmlParserCtxtPtr ctxt);

/**
 * Generic production rules.
 */
xmlChar *		xmlScanName		(xmlParserCtxtPtr ctxt);
xmlChar *		xmlParseName		(xmlParserCtxtPtr ctxt);
xmlChar *		xmlParseNmtoken		(xmlParserCtxtPtr ctxt);
xmlChar *		xmlParseEntityValue	(xmlParserCtxtPtr ctxt,
						 xmlChar **orig);
xmlChar *		xmlParseAttValue	(xmlParserCtxtPtr ctxt);
xmlChar *		xmlParseSystemLiteral	(xmlParserCtxtPtr ctxt);
xmlChar *		xmlParsePubidLiteral	(xmlParserCtxtPtr ctxt);
void			xmlParseCharData	(xmlParserCtxtPtr ctxt,
						 int cdata);
xmlChar *		xmlParseExternalID	(xmlParserCtxtPtr ctxt,
						 xmlChar **publicID,
						 int strict);
void			xmlParseComment		(xmlParserCtxtPtr ctxt);
xmlChar *		xmlParsePITarget	(xmlParserCtxtPtr ctxt);
void			xmlParsePI		(xmlParserCtxtPtr ctxt);
void			xmlParseNotationDecl	(xmlParserCtxtPtr ctxt);
void			xmlParseEntityDecl	(xmlParserCtxtPtr ctxt);
int			xmlParseDefaultDecl	(xmlParserCtxtPtr ctxt,
						 xmlChar **value);
xmlEnumerationPtr	xmlParseNotationType	(xmlParserCtxtPtr ctxt);
xmlEnumerationPtr	xmlParseEnumerationType	(xmlParserCtxtPtr ctxt);
int			xmlParseEnumeratedType	(xmlParserCtxtPtr ctxt,
						 xmlEnumerationPtr *tree);
int			xmlParseAttributeType	(xmlParserCtxtPtr ctxt,
						 xmlEnumerationPtr *tree);
void			xmlParseAttributeListDecl(xmlParserCtxtPtr ctxt);
xmlElementContentPtr	xmlParseElementMixedContentDecl
						(xmlParserCtxtPtr ctxt,
						 xmlParserInputPtr inputchk);
xmlElementContentPtr	xmlParseElementChildrenContentDecl
						(xmlParserCtxtPtr ctxt,
						 xmlParserInputPtr inputchk);
int			xmlParseElementContentDecl(xmlParserCtxtPtr ctxt,
						 xmlChar *name,
						 xmlElementContentPtr *result);
int			xmlParseElementDecl	(xmlParserCtxtPtr ctxt);
void			xmlParseMarkupDecl	(xmlParserCtxtPtr ctxt);
int			xmlParseCharRef		(xmlParserCtxtPtr ctxt);
xmlEntityPtr		xmlParseEntityRef	(xmlParserCtxtPtr ctxt);
void			xmlParseReference	(xmlParserCtxtPtr ctxt);
void			xmlParsePEReference	(xmlParserCtxtPtr ctxt);
void			xmlParseDocTypeDecl	(xmlParserCtxtPtr ctxt);
xmlChar *		xmlParseAttribute	(xmlParserCtxtPtr ctxt,
						 xmlChar **value);
xmlChar *		xmlParseStartTag	(xmlParserCtxtPtr ctxt);
void			xmlParseEndTag		(xmlParserCtxtPtr ctxt);
void			xmlParseCDSect		(xmlParserCtxtPtr ctxt);
void			xmlParseContent		(xmlParserCtxtPtr ctxt);
void			xmlParseElement		(xmlParserCtxtPtr ctxt);
xmlChar *		xmlParseVersionNum	(xmlParserCtxtPtr ctxt);
xmlChar *		xmlParseVersionInfo	(xmlParserCtxtPtr ctxt);
xmlChar *		xmlParseEncName		(xmlParserCtxtPtr ctxt);
xmlChar *		xmlParseEncodingDecl	(xmlParserCtxtPtr ctxt);
int			xmlParseSDDecl		(xmlParserCtxtPtr ctxt);
void			xmlParseXMLDecl		(xmlParserCtxtPtr ctxt);
void			xmlParseTextDecl	(xmlParserCtxtPtr ctxt);
void			xmlParseMisc		(xmlParserCtxtPtr ctxt);
void			xmlParseExternalSubset	(xmlParserCtxtPtr ctxt,
						 const xmlChar *ExternalID,
						 const xmlChar *SystemID); 
/**
 * XML_SUBSTITUTE_NONE:
 *
 * If no entities need to be substituted.
 */
#define XML_SUBSTITUTE_NONE	0
/**
 * XML_SUBSTITUTE_REF:
 *
 * Whether general entities need to be substituted.
 */
#define XML_SUBSTITUTE_REF	1
/**
 * XML_SUBSTITUTE_PEREF:
 *
 * Whether parameter entities need to be substituted.
 */
#define XML_SUBSTITUTE_PEREF	2
/**
 * XML_SUBSTITUTE_BOTH:
 *
 * Both general and parameter entities need to be substituted.
 */
#define XML_SUBSTITUTE_BOTH 	3

xmlChar *		xmlDecodeEntities	(xmlParserCtxtPtr ctxt,
						 int len,
						 int what,
						 xmlChar end,
						 xmlChar  end2,
						 xmlChar end3);
xmlChar *		xmlStringDecodeEntities	(xmlParserCtxtPtr ctxt,
						 const xmlChar *str,
						 int what,
						 xmlChar end,
						 xmlChar  end2,
						 xmlChar end3);

/*
 * Generated by MACROS on top of parser.c c.f. PUSH_AND_POP.
 */
int			nodePush		(xmlParserCtxtPtr ctxt,
						 xmlNodePtr value);
xmlNodePtr		nodePop			(xmlParserCtxtPtr ctxt);
int			inputPush		(xmlParserCtxtPtr ctxt,
						 xmlParserInputPtr value);
xmlParserInputPtr	inputPop		(xmlParserCtxtPtr ctxt);
xmlChar                *namePop			(xmlParserCtxtPtr ctxt);
int			namePush		(xmlParserCtxtPtr ctxt,
						 xmlChar *value);

/*
 * other commodities shared between parser.c and parserInternals.
 */
int			xmlSkipBlankChars	(xmlParserCtxtPtr ctxt);
int			xmlStringCurrentChar	(xmlParserCtxtPtr ctxt,
						 const xmlChar *cur,
						 int *len);
void			xmlParserHandlePEReference(xmlParserCtxtPtr ctxt);
void			xmlParserHandleReference(xmlParserCtxtPtr ctxt);
int			xmlCheckLanguageID	(const xmlChar *lang);

/*
 * Really core function shared with HTML parser.
 */
int			xmlCurrentChar		(xmlParserCtxtPtr ctxt,
						 int *len);
int			xmlCopyCharMultiByte	(xmlChar *out,
						 int val);
int			xmlCopyChar		(int len,
						 xmlChar *out,
						 int val);
void			xmlNextChar		(xmlParserCtxtPtr ctxt);
void			xmlParserInputShrink	(xmlParserInputPtr in);

#ifdef LIBXML_HTML_ENABLED
/*
 * Actually comes from the HTML parser but launched from the init stuff.
 */
void			htmlInitAutoClose	(void);
htmlParserCtxtPtr	htmlCreateFileParserCtxt(const char *filename,
	                                         const char *encoding);
#endif

/*
 * Specific function to keep track of entities references
 * and used by the XSLT debugger.
 */
/**
 * xmlEntityReferenceFunc:
 * @ent: the entity
 * @firstNode:  the fist node in the chunk
 * @lastNode:  the last nod in the chunk
 *
 * Callback function used when one needs to be able to track back the
 * provenance of a chunk of nodes inherited from an entity replacement.
 */
typedef	void	(*xmlEntityReferenceFunc)	(xmlEntityPtr ent,
						 xmlNodePtr firstNode,
						 xmlNodePtr lastNode);
  
void		xmlSetEntityReferenceFunc	(xmlEntityReferenceFunc func);


#ifdef __cplusplus
}
#endif
#endif /* __XML_PARSER_INTERNALS_H__ */
