/*
 * parser.c : an XML 1.0 parser, namespaces and validity support are mostly
 *            implemented on top of the SAX interfaces
 *
 * References:
 *   The XML specification:
 *     http://www.w3.org/TR/REC-xml
 *   Original 1.0 version:
 *     http://www.w3.org/TR/1998/REC-xml-19980210
 *   XML second edition working draft
 *     http://www.w3.org/TR/2000/WD-xml-2e-20000814
 *
 * Okay this is a big file, the parser core is around 7000 lines, then it
 * is followed by the progressive parser top routines, then the various
 * high level APIs to call the parser and a few miscellaneous functions.
 * A number of helper functions and deprecated ones have been moved to
 * parserInternals.c to reduce this file size.
 * As much as possible the functions are associated with their relative
 * production in the XML specification. A few productions defining the
 * different ranges of character are actually implanted either in 
 * parserInternals.h or parserInternals.c
 * The DOM tree build is realized from the default SAX callbacks in
 * the module SAX.c.
 * The routines doing the validation checks are in valid.c and called either
 * from the SAX callbacks or as standalone functions using a preparsed
 * document.
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

#include <stdlib.h>
#include <string.h>
#include <libxml/xmlmemory.h>
#include <libxml/threads.h>
#include <libxml/globals.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/valid.h>
#include <libxml/entities.h>
#include <libxml/xmlerror.h>
#include <libxml/encoding.h>
#include <libxml/xmlIO.h>
#include <libxml/uri.h>
#ifdef LIBXML_CATALOG_ENABLED
#include <libxml/catalog.h>
#endif

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


#define XML_PARSER_BIG_BUFFER_SIZE 300
#define XML_PARSER_BUFFER_SIZE 100

#define SAX_COMPAT_MODE BAD_CAST "SAX compatibility mode document"

/*
 * List of XML prefixed PI allowed by W3C specs
 */

static const char *xmlW3CPIs[] = {
    "xml-stylesheet",
    NULL
};

/* DEPR void xmlParserHandleReference(xmlParserCtxtPtr ctxt); */
xmlEntityPtr xmlParseStringPEReference(xmlParserCtxtPtr ctxt,
                                       const xmlChar **str);

static int
xmlParseExternalEntityPrivate(xmlDocPtr doc, xmlParserCtxtPtr oldctxt,
	              xmlSAXHandlerPtr sax,
		      void *user_data, int depth, const xmlChar *URL,
		      const xmlChar *ID, xmlNodePtr *list);

static void
xmlAddEntityReference(xmlEntityPtr ent, xmlNodePtr firstNode,
                      xmlNodePtr lastNode);

static int
xmlParseBalancedChunkMemoryInternal(xmlParserCtxtPtr oldctxt,
		      const xmlChar *string, void *user_data, xmlNodePtr *lst);
/************************************************************************
 *									*
 * 		Parser stacks related functions and macros		*
 *									*
 ************************************************************************/

xmlEntityPtr xmlParseStringEntityRef(xmlParserCtxtPtr ctxt,
                                     const xmlChar ** str);

/**
 * inputPush:
 * @ctxt:  an XML parser context
 * @value:  the parser input
 *
 * Pushes a new parser input on top of the input stack
 *
 * Returns 0 in case of error, the index in the stack otherwise
 */
extern int
inputPush(xmlParserCtxtPtr ctxt, xmlParserInputPtr value)
{
    if (ctxt->inputNr >= ctxt->inputMax) {
        ctxt->inputMax *= 2;
        ctxt->inputTab =
            (xmlParserInputPtr *) xmlRealloc(ctxt->inputTab,
                                             ctxt->inputMax *
                                             sizeof(ctxt->inputTab[0]));
        if (ctxt->inputTab == NULL) {
            xmlGenericError(xmlGenericErrorContext, "realloc failed !\n");
            return (0);
        }
    }
    ctxt->inputTab[ctxt->inputNr] = value;
    ctxt->input = value;
    return (ctxt->inputNr++);
}
/**
 * inputPop:
 * @ctxt: an XML parser context
 *
 * Pops the top parser input from the input stack
 *
 * Returns the input just removed
 */
extern xmlParserInputPtr
inputPop(xmlParserCtxtPtr ctxt)
{
    xmlParserInputPtr ret;

    if (ctxt->inputNr <= 0)
        return (0);
    ctxt->inputNr--;
    if (ctxt->inputNr > 0)
        ctxt->input = ctxt->inputTab[ctxt->inputNr - 1];
    else
        ctxt->input = NULL;
    ret = ctxt->inputTab[ctxt->inputNr];
    ctxt->inputTab[ctxt->inputNr] = 0;
    return (ret);
}
/**
 * nodePush:
 * @ctxt:  an XML parser context
 * @value:  the element node
 *
 * Pushes a new element node on top of the node stack
 *
 * Returns 0 in case of error, the index in the stack otherwise
 */
extern int
nodePush(xmlParserCtxtPtr ctxt, xmlNodePtr value)
{
    if (ctxt->nodeNr >= ctxt->nodeMax) {
        ctxt->nodeMax *= 2;
        ctxt->nodeTab =
            (xmlNodePtr *) xmlRealloc(ctxt->nodeTab,
                                      ctxt->nodeMax *
                                      sizeof(ctxt->nodeTab[0]));
        if (ctxt->nodeTab == NULL) {
            xmlGenericError(xmlGenericErrorContext, "realloc failed !\n");
            return (0);
        }
    }
    ctxt->nodeTab[ctxt->nodeNr] = value;
    ctxt->node = value;
    return (ctxt->nodeNr++);
}
/**
 * nodePop:
 * @ctxt: an XML parser context
 *
 * Pops the top element node from the node stack
 *
 * Returns the node just removed
 */
extern xmlNodePtr
nodePop(xmlParserCtxtPtr ctxt)
{
    xmlNodePtr ret;

    if (ctxt->nodeNr <= 0)
        return (0);
    ctxt->nodeNr--;
    if (ctxt->nodeNr > 0)
        ctxt->node = ctxt->nodeTab[ctxt->nodeNr - 1];
    else
        ctxt->node = NULL;
    ret = ctxt->nodeTab[ctxt->nodeNr];
    ctxt->nodeTab[ctxt->nodeNr] = 0;
    return (ret);
}
/**
 * namePush:
 * @ctxt:  an XML parser context
 * @value:  the element name
 *
 * Pushes a new element name on top of the name stack
 *
 * Returns 0 in case of error, the index in the stack otherwise
 */
extern int
namePush(xmlParserCtxtPtr ctxt, xmlChar * value)
{
    if (ctxt->nameNr >= ctxt->nameMax) {
        ctxt->nameMax *= 2;
        ctxt->nameTab =
            (xmlChar * *)xmlRealloc(ctxt->nameTab,
                                    ctxt->nameMax *
                                    sizeof(ctxt->nameTab[0]));
        if (ctxt->nameTab == NULL) {
            xmlGenericError(xmlGenericErrorContext, "realloc failed !\n");
            return (0);
        }
    }
    ctxt->nameTab[ctxt->nameNr] = value;
    ctxt->name = value;
    return (ctxt->nameNr++);
}
/**
 * namePop:
 * @ctxt: an XML parser context
 *
 * Pops the top element name from the name stack
 *
 * Returns the name just removed
 */
extern xmlChar *
namePop(xmlParserCtxtPtr ctxt)
{
    xmlChar *ret;

    if (ctxt->nameNr <= 0)
        return (0);
    ctxt->nameNr--;
    if (ctxt->nameNr > 0)
        ctxt->name = ctxt->nameTab[ctxt->nameNr - 1];
    else
        ctxt->name = NULL;
    ret = ctxt->nameTab[ctxt->nameNr];
    ctxt->nameTab[ctxt->nameNr] = 0;
    return (ret);
}

static int spacePush(xmlParserCtxtPtr ctxt, int val) {
    if (ctxt->spaceNr >= ctxt->spaceMax) {
	ctxt->spaceMax *= 2;
        ctxt->spaceTab = (int *) xmlRealloc(ctxt->spaceTab,
	             ctxt->spaceMax * sizeof(ctxt->spaceTab[0]));
        if (ctxt->spaceTab == NULL) {
	    xmlGenericError(xmlGenericErrorContext,
		    "realloc failed !\n");
	    return(0);
	}
    }
    ctxt->spaceTab[ctxt->spaceNr] = val;
    ctxt->space = &ctxt->spaceTab[ctxt->spaceNr];
    return(ctxt->spaceNr++);
}

static int spacePop(xmlParserCtxtPtr ctxt) {
    int ret;
    if (ctxt->spaceNr <= 0) return(0);
    ctxt->spaceNr--;
    if (ctxt->spaceNr > 0)
	ctxt->space = &ctxt->spaceTab[ctxt->spaceNr - 1];
    else
        ctxt->space = NULL;
    ret = ctxt->spaceTab[ctxt->spaceNr];
    ctxt->spaceTab[ctxt->spaceNr] = -1;
    return(ret);
}

/*
 * Macros for accessing the content. Those should be used only by the parser,
 * and not exported.
 *
 * Dirty macros, i.e. one often need to make assumption on the context to
 * use them
 *
 *   CUR_PTR return the current pointer to the xmlChar to be parsed.
 *           To be used with extreme caution since operations consuming
 *           characters may move the input buffer to a different location !
 *   CUR     returns the current xmlChar value, i.e. a 8 bit value if compiled
 *           This should be used internally by the parser
 *           only to compare to ASCII values otherwise it would break when
 *           running with UTF-8 encoding.
 *   RAW     same as CUR but in the input buffer, bypass any token
 *           extraction that may have been done
 *   NXT(n)  returns the n'th next xmlChar. Same as CUR is should be used only
 *           to compare on ASCII based substring.
 *   SKIP(n) Skip n xmlChar, and must also be used only to skip ASCII defined
 *           strings within the parser.
 *
 * Clean macros, not dependent of an ASCII context, expect UTF-8 encoding
 *
 *   NEXT    Skip to the next character, this does the proper decoding
 *           in UTF-8 mode. It also pop-up unfinished entities on the fly.
 *   NEXTL(l) Skip l xmlChar in the input buffer
 *   CUR_CHAR(l) returns the current unicode character (int), set l
 *           to the number of xmlChars used for the encoding [0-5].
 *   CUR_SCHAR  same but operate on a string instead of the context
 *   COPY_BUF  copy the current unicode char to the target buffer, increment
 *            the index
 *   GROW, SHRINK  handling of input buffers
 */

#define RAW (*ctxt->input->cur)
#define CUR (*ctxt->input->cur)
#define NXT(val) ctxt->input->cur[(val)]
#define CUR_PTR ctxt->input->cur

#define SKIP(val) do {							\
    ctxt->nbChars += (val),ctxt->input->cur += (val);			\
    if (*ctxt->input->cur == '%') xmlParserHandlePEReference(ctxt);	\
    if ((*ctxt->input->cur == 0) &&					\
        (xmlParserInputGrow(ctxt->input, INPUT_CHUNK) <= 0))		\
	    xmlPopInput(ctxt);						\
  } while (0)

#define SHRINK if (ctxt->input->cur - ctxt->input->base > INPUT_CHUNK) \
	xmlSHRINK (ctxt);

static void xmlSHRINK (xmlParserCtxtPtr ctxt) {
    xmlParserInputShrink(ctxt->input);
    if ((*ctxt->input->cur == 0) &&
        (xmlParserInputGrow(ctxt->input, INPUT_CHUNK) <= 0))
	    xmlPopInput(ctxt);
  }

#define GROW if (ctxt->input->end - ctxt->input->cur < INPUT_CHUNK) \
	xmlGROW (ctxt);

static void xmlGROW (xmlParserCtxtPtr ctxt) {
    xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
    if ((*ctxt->input->cur == 0) &&
        (xmlParserInputGrow(ctxt->input, INPUT_CHUNK) <= 0))
	    xmlPopInput(ctxt);
  }

#define SKIP_BLANKS xmlSkipBlankChars(ctxt)

#define NEXT xmlNextChar(ctxt)

#define NEXT1 {								\
	ctxt->input->cur++;						\
	ctxt->nbChars++;						\
	if (*ctxt->input->cur == 0)					\
	    xmlParserInputGrow(ctxt->input, INPUT_CHUNK);		\
    }

#define NEXTL(l) do {							\
    if (*(ctxt->input->cur) == '\n') {					\
	ctxt->input->line++; ctxt->input->col = 1;			\
    } else ctxt->input->col++;						\
    ctxt->input->cur += l;				\
    if (*ctxt->input->cur == '%') xmlParserHandlePEReference(ctxt);	\
  } while (0)

#define CUR_CHAR(l) xmlCurrentChar(ctxt, &l)
#define CUR_SCHAR(s, l) xmlStringCurrentChar(ctxt, s, &l)

#define COPY_BUF(l,b,i,v)						\
    if (l == 1) b[i++] = (xmlChar) v;					\
    else i += xmlCopyCharMultiByte(&b[i],v)

/**
 * xmlSkipBlankChars:
 * @ctxt:  the XML parser context
 *
 * skip all blanks character found at that point in the input streams.
 * It pops up finished entities in the process if allowable at that point.
 *
 * Returns the number of space chars skipped
 */

int
xmlSkipBlankChars(xmlParserCtxtPtr ctxt) {
    int res = 0;

    /*
     * It's Okay to use CUR/NEXT here since all the blanks are on
     * the ASCII range.
     */
    if ((ctxt->inputNr == 1) && (ctxt->instate != XML_PARSER_DTD)) {
	const xmlChar *cur;
	/*
	 * if we are in the document content, go really fast
	 */
	cur = ctxt->input->cur;
	while (IS_BLANK(*cur)) {
	    if (*cur == '\n') {
		ctxt->input->line++; ctxt->input->col = 1;
	    }
	    cur++;
	    res++;
	    if (*cur == 0) {
		ctxt->input->cur = cur;
		xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
		cur = ctxt->input->cur;
	    }
	}
	ctxt->input->cur = cur;
    } else {
	int cur;
	do {
	    cur = CUR;
	    while (IS_BLANK(cur)) { /* CHECKED tstblanks.xml */
		NEXT;
		cur = CUR;
		res++;
	    }
	    while ((cur == 0) && (ctxt->inputNr > 1) &&
		   (ctxt->instate != XML_PARSER_COMMENT)) {
		xmlPopInput(ctxt);
		cur = CUR;
	    }
	    /*
	     * Need to handle support of entities branching here
	     */
	    if (*ctxt->input->cur == '%') xmlParserHandlePEReference(ctxt);
	} while (IS_BLANK(cur)); /* CHECKED tstblanks.xml */
    }
    return(res);
}

/************************************************************************
 *									*
 *		Commodity functions to handle entities			*
 *									*
 ************************************************************************/

/**
 * xmlPopInput:
 * @ctxt:  an XML parser context
 *
 * xmlPopInput: the current input pointed by ctxt->input came to an end
 *          pop it and return the next char.
 *
 * Returns the current xmlChar in the parser context
 */
xmlChar
xmlPopInput(xmlParserCtxtPtr ctxt) {
    if (ctxt->inputNr == 1) return(0); /* End of main Input */
    if (xmlParserDebugEntities)
	xmlGenericError(xmlGenericErrorContext,
		"Popping input %d\n", ctxt->inputNr);
    xmlFreeInputStream(inputPop(ctxt));
    if ((*ctxt->input->cur == 0) &&
        (xmlParserInputGrow(ctxt->input, INPUT_CHUNK) <= 0))
	    return(xmlPopInput(ctxt));
    return(CUR);
}

/**
 * xmlPushInput:
 * @ctxt:  an XML parser context
 * @input:  an XML parser input fragment (entity, XML fragment ...).
 *
 * xmlPushInput: switch to a new input stream which is stacked on top
 *               of the previous one(s).
 */
void
xmlPushInput(xmlParserCtxtPtr ctxt, xmlParserInputPtr input) {
    if (input == NULL) return;

    if (xmlParserDebugEntities) {
	if ((ctxt->input != NULL) && (ctxt->input->filename))
	    xmlGenericError(xmlGenericErrorContext,
		    "%s(%d): ", ctxt->input->filename,
		    ctxt->input->line);
	xmlGenericError(xmlGenericErrorContext,
		"Pushing input %d : %.30s\n", ctxt->inputNr+1, input->cur);
    }
    inputPush(ctxt, input);
    GROW;
}

/**
 * xmlParseCharRef:
 * @ctxt:  an XML parser context
 *
 * parse Reference declarations
 *
 * [66] CharRef ::= '&#' [0-9]+ ';' |
 *                  '&#x' [0-9a-fA-F]+ ';'
 *
 * [ WFC: Legal Character ]
 * Characters referred to using character references must match the
 * production for Char. 
 *
 * Returns the value parsed (as an int), 0 in case of error
 */
int
xmlParseCharRef(xmlParserCtxtPtr ctxt) {
    unsigned int val = 0;
    int count = 0;

    /*
     * Using RAW/CUR/NEXT is okay since we are working on ASCII range here
     */
    if ((RAW == '&') && (NXT(1) == '#') &&
        (NXT(2) == 'x')) {
	SKIP(3);
	GROW;
	while (RAW != ';') { /* loop blocked by count */
	    if (count++ > 20) {
		count = 0;
		GROW;
	    }
	    if ((RAW >= '0') && (RAW <= '9')) 
	        val = val * 16 + (CUR - '0');
	    else if ((RAW >= 'a') && (RAW <= 'f') && (count < 20))
	        val = val * 16 + (CUR - 'a') + 10;
	    else if ((RAW >= 'A') && (RAW <= 'F') && (count < 20))
	        val = val * 16 + (CUR - 'A') + 10;
	    else {
		ctxt->errNo = XML_ERR_INVALID_HEX_CHARREF;
	        if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData, 
		         "xmlParseCharRef: invalid hexadecimal value\n");
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		val = 0;
		break;
	    }
	    NEXT;
	    count++;
	}
	if (RAW == ';') {
	    /* on purpose to avoid reentrancy problems with NEXT and SKIP */
	    ctxt->nbChars ++;
	    ctxt->input->cur++;
	}
    } else if  ((RAW == '&') && (NXT(1) == '#')) {
	SKIP(2);
	GROW;
	while (RAW != ';') { /* loop blocked by count */
	    if (count++ > 20) {
		count = 0;
		GROW;
	    }
	    if ((RAW >= '0') && (RAW <= '9')) 
	        val = val * 10 + (CUR - '0');
	    else {
		ctxt->errNo = XML_ERR_INVALID_DEC_CHARREF;
	        if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData, 
		         "xmlParseCharRef: invalid decimal value\n");
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		val = 0;
		break;
	    }
	    NEXT;
	    count++;
	}
	if (RAW == ';') {
	    /* on purpose to avoid reentrancy problems with NEXT and SKIP */
	    ctxt->nbChars ++;
	    ctxt->input->cur++;
	}
    } else {
	ctxt->errNo = XML_ERR_INVALID_CHARREF;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	       "xmlParseCharRef: invalid value\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    }

    /*
     * [ WFC: Legal Character ]
     * Characters referred to using character references must match the
     * production for Char. 
     */
    if (IS_CHAR(val)) {
        return(val);
    } else {
	ctxt->errNo = XML_ERR_INVALID_CHAR;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
                             "xmlParseCharRef: invalid xmlChar value %d\n",
	                     val);
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    }
    return(0);
}

/**
 * xmlParseStringCharRef:
 * @ctxt:  an XML parser context
 * @str:  a pointer to an index in the string
 *
 * parse Reference declarations, variant parsing from a string rather
 * than an an input flow.
 *
 * [66] CharRef ::= '&#' [0-9]+ ';' |
 *                  '&#x' [0-9a-fA-F]+ ';'
 *
 * [ WFC: Legal Character ]
 * Characters referred to using character references must match the
 * production for Char. 
 *
 * Returns the value parsed (as an int), 0 in case of error, str will be
 *         updated to the current value of the index
 */
static int
xmlParseStringCharRef(xmlParserCtxtPtr ctxt, const xmlChar **str) {
    const xmlChar *ptr;
    xmlChar cur;
    int val = 0;

    if ((str == NULL) || (*str == NULL)) return(0);
    ptr = *str;
    cur = *ptr;
    if ((cur == '&') && (ptr[1] == '#') && (ptr[2] == 'x')) {
	ptr += 3;
	cur = *ptr;
	while (cur != ';') { /* Non input consuming loop */
	    if ((cur >= '0') && (cur <= '9')) 
	        val = val * 16 + (cur - '0');
	    else if ((cur >= 'a') && (cur <= 'f'))
	        val = val * 16 + (cur - 'a') + 10;
	    else if ((cur >= 'A') && (cur <= 'F'))
	        val = val * 16 + (cur - 'A') + 10;
	    else {
		ctxt->errNo = XML_ERR_INVALID_HEX_CHARREF;
	        if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData, 
		         "xmlParseStringCharRef: invalid hexadecimal value\n");
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		val = 0;
		break;
	    }
	    ptr++;
	    cur = *ptr;
	}
	if (cur == ';')
	    ptr++;
    } else if  ((cur == '&') && (ptr[1] == '#')){
	ptr += 2;
	cur = *ptr;
	while (cur != ';') { /* Non input consuming loops */
	    if ((cur >= '0') && (cur <= '9')) 
	        val = val * 10 + (cur - '0');
	    else {
		ctxt->errNo = XML_ERR_INVALID_DEC_CHARREF;
	        if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData, 
		         "xmlParseStringCharRef: invalid decimal value\n");
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		val = 0;
		break;
	    }
	    ptr++;
	    cur = *ptr;
	}
	if (cur == ';')
	    ptr++;
    } else {
	ctxt->errNo = XML_ERR_INVALID_CHARREF;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	       "xmlParseStringCharRef: invalid value\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	return(0);
    }
    *str = ptr;

    /*
     * [ WFC: Legal Character ]
     * Characters referred to using character references must match the
     * production for Char. 
     */
    if (IS_CHAR(val)) {
        return(val);
    } else {
	ctxt->errNo = XML_ERR_INVALID_CHAR;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
		             "xmlParseStringCharRef: invalid xmlChar value %d\n", val);
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    }
    return(0);
}

/**
 * xmlNewBlanksWrapperInputStream:
 * @ctxt:  an XML parser context
 * @entity:  an Entity pointer
 *
 * Create a new input stream for wrapping
 * blanks around a PEReference
 *
 * Returns the new input stream or NULL
 */
 
static void deallocblankswrapper (xmlChar *str) {xmlFree(str);}
 
static xmlParserInputPtr
xmlNewBlanksWrapperInputStream(xmlParserCtxtPtr ctxt, xmlEntityPtr entity) {
    xmlParserInputPtr input;
    xmlChar *buffer;
    size_t length;
    if (entity == NULL) {
        ctxt->errNo = XML_ERR_INTERNAL_ERROR;
        if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	      "internal: xmlNewBlanksWrapperInputStream entity = NULL\n");
	ctxt->errNo = XML_ERR_INTERNAL_ERROR;
	return(NULL);
    }
    if (xmlParserDebugEntities)
	xmlGenericError(xmlGenericErrorContext,
		"new blanks wrapper for entity: %s\n", entity->name);
    input = xmlNewInputStream(ctxt);
    if (input == NULL) {
	return(NULL);
    }
    length = xmlStrlen(entity->name) + 5;
    buffer = xmlMalloc(length);
    if (buffer == NULL) {
    	return(NULL);
    }
    buffer [0] = ' ';
    buffer [1] = '%';
    buffer [length-3] = ';';
    buffer [length-2] = ' ';
    buffer [length-1] = 0;
    memcpy(buffer + 2, entity->name, length - 5);
    input->free = deallocblankswrapper;
    input->base = buffer;
    input->cur = buffer;
    input->length = length;
    input->end = &buffer[length];
    return(input);
}

/**
 * xmlParserHandlePEReference:
 * @ctxt:  the parser context
 * 
 * [69] PEReference ::= '%' Name ';'
 *
 * [ WFC: No Recursion ]
 * A parsed entity must not contain a recursive
 * reference to itself, either directly or indirectly. 
 *
 * [ WFC: Entity Declared ]
 * In a document without any DTD, a document with only an internal DTD
 * subset which contains no parameter entity references, or a document
 * with "standalone='yes'", ...  ... The declaration of a parameter
 * entity must precede any reference to it...
 *
 * [ VC: Entity Declared ]
 * In a document with an external subset or external parameter entities
 * with "standalone='no'", ...  ... The declaration of a parameter entity
 * must precede any reference to it...
 *
 * [ WFC: In DTD ]
 * Parameter-entity references may only appear in the DTD.
 * NOTE: misleading but this is handled.
 *
 * A PEReference may have been detected in the current input stream
 * the handling is done accordingly to 
 *      http://www.w3.org/TR/REC-xml#entproc
 * i.e. 
 *   - Included in literal in entity values
 *   - Included as Parameter Entity reference within DTDs
 */
void
xmlParserHandlePEReference(xmlParserCtxtPtr ctxt) {
    xmlChar *name;
    xmlEntityPtr entity = NULL;
    xmlParserInputPtr input;

    if (RAW != '%') return;
    switch(ctxt->instate) {
	case XML_PARSER_CDATA_SECTION:
	    return;
        case XML_PARSER_COMMENT:
	    return;
	case XML_PARSER_START_TAG:
	    return;
	case XML_PARSER_END_TAG:
	    return;
        case XML_PARSER_EOF:
	    ctxt->errNo = XML_ERR_PEREF_AT_EOF;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData, "PEReference at EOF\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    return;
        case XML_PARSER_PROLOG:
	case XML_PARSER_START:
	case XML_PARSER_MISC:
	    ctxt->errNo = XML_ERR_PEREF_IN_PROLOG;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData, "PEReference in prolog!\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    return;
	case XML_PARSER_ENTITY_DECL:
        case XML_PARSER_CONTENT:
        case XML_PARSER_ATTRIBUTE_VALUE:
        case XML_PARSER_PI:
	case XML_PARSER_SYSTEM_LITERAL:
	case XML_PARSER_PUBLIC_LITERAL:
	    /* we just ignore it there */
	    return;
        case XML_PARSER_EPILOG:
	    ctxt->errNo = XML_ERR_PEREF_IN_EPILOG;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData, "PEReference in epilog!\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    return;
	case XML_PARSER_ENTITY_VALUE:
	    /*
	     * NOTE: in the case of entity values, we don't do the
	     *       substitution here since we need the literal
	     *       entity value to be able to save the internal
	     *       subset of the document.
	     *       This will be handled by xmlStringDecodeEntities
	     */
	    return;
        case XML_PARSER_DTD:
	    /*
	     * [WFC: Well-Formedness Constraint: PEs in Internal Subset]
	     * In the internal DTD subset, parameter-entity references
	     * can occur only where markup declarations can occur, not
	     * within markup declarations.
	     * In that case this is handled in xmlParseMarkupDecl
	     */
	    if ((ctxt->external == 0) && (ctxt->inputNr == 1))
		return;
	    if (IS_BLANK(NXT(1)) || NXT(1) == 0)
		return;
            break;
        case XML_PARSER_IGNORE:
            return;
    }

    NEXT;
    name = xmlParseName(ctxt);
    if (xmlParserDebugEntities)
	xmlGenericError(xmlGenericErrorContext,
		"PEReference: %s\n", name);
    if (name == NULL) {
        ctxt->errNo = XML_ERR_PEREF_NO_NAME;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, "xmlParserHandlePEReference: no name\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    } else {
	if (RAW == ';') {
	    NEXT;
	    if ((ctxt->sax != NULL) && (ctxt->sax->getParameterEntity != NULL))
		entity = ctxt->sax->getParameterEntity(ctxt->userData, name);
	    if (entity == NULL) {
	        
		/*
		 * [ WFC: Entity Declared ]
		 * In a document without any DTD, a document with only an
		 * internal DTD subset which contains no parameter entity
		 * references, or a document with "standalone='yes'", ...
		 * ... The declaration of a parameter entity must precede
		 * any reference to it...
		 */
		if ((ctxt->standalone == 1) ||
		    ((ctxt->hasExternalSubset == 0) &&
		     (ctxt->hasPErefs == 0))) {
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData,
			 "PEReference: %%%s; not found\n", name);
		    ctxt->wellFormed = 0;
		    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	        } else {
		    /*
		     * [ VC: Entity Declared ]
		     * In a document with an external subset or external
		     * parameter entities with "standalone='no'", ...
		     * ... The declaration of a parameter entity must precede
		     * any reference to it...
		     */
		    if ((!ctxt->disableSAX) &&
			(ctxt->validate) && (ctxt->vctxt.error != NULL)) {
			ctxt->vctxt.error(ctxt->vctxt.userData,
			     "PEReference: %%%s; not found\n", name);
		    } else if ((!ctxt->disableSAX) &&
			(ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
			ctxt->sax->warning(ctxt->userData,
			 "PEReference: %%%s; not found\n", name);
		    ctxt->valid = 0;
		}
	    } else if (ctxt->input->free != deallocblankswrapper) {
		    input = xmlNewBlanksWrapperInputStream(ctxt, entity);
		    xmlPushInput(ctxt, input);
	    } else {
	        if ((entity->etype == XML_INTERNAL_PARAMETER_ENTITY) ||
		    (entity->etype == XML_EXTERNAL_PARAMETER_ENTITY)) {
		    xmlChar start[4];
		    xmlCharEncoding enc;

		    /*
		     * handle the extra spaces added before and after
		     * c.f. http://www.w3.org/TR/REC-xml#as-PE
		     * this is done independently.
		     */
		    input = xmlNewEntityInputStream(ctxt, entity);
		    xmlPushInput(ctxt, input);

		    /* 
		     * Get the 4 first bytes and decode the charset
		     * if enc != XML_CHAR_ENCODING_NONE
		     * plug some encoding conversion routines.
		     */
		    GROW
	            if (entity->length >= 4) {
			start[0] = RAW;
			start[1] = NXT(1);
			start[2] = NXT(2);
			start[3] = NXT(3);
			enc = xmlDetectCharEncoding(start, 4);
			if (enc != XML_CHAR_ENCODING_NONE) {
			    xmlSwitchEncoding(ctxt, enc);
			}
		    }

		    if ((entity->etype == XML_EXTERNAL_PARAMETER_ENTITY) &&
			(RAW == '<') && (NXT(1) == '?') &&
			(NXT(2) == 'x') && (NXT(3) == 'm') &&
			(NXT(4) == 'l') && (IS_BLANK(NXT(5)))) {
			xmlParseTextDecl(ctxt);
		    }
		} else {
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData,
			 "xmlParserHandlePEReference: %s is not a parameter entity\n",
			                 name);
		    ctxt->wellFormed = 0;
		    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		}
	    }
	} else {
	    ctxt->errNo = XML_ERR_PEREF_SEMICOL_MISSING;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData,
				 "xmlParserHandlePEReference: expecting ';'\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	}
	xmlFree(name);
    }
}

/*
 * Macro used to grow the current buffer.
 */
#define growBuffer(buffer) {						\
    buffer##_size *= 2;							\
    buffer = (xmlChar *)						\
    		xmlRealloc(buffer, buffer##_size * sizeof(xmlChar));	\
    if (buffer == NULL) {						\
	xmlGenericError(xmlGenericErrorContext,	"realloc failed");	\
	return(NULL);							\
    }									\
}

/**
 * xmlStringDecodeEntities:
 * @ctxt:  the parser context
 * @str:  the input string
 * @what:  combination of XML_SUBSTITUTE_REF and XML_SUBSTITUTE_PEREF
 * @end:  an end marker xmlChar, 0 if none
 * @end2:  an end marker xmlChar, 0 if none
 * @end3:  an end marker xmlChar, 0 if none
 * 
 * Takes a entity string content and process to do the adequate substitutions.
 *
 * [67] Reference ::= EntityRef | CharRef
 *
 * [69] PEReference ::= '%' Name ';'
 *
 * Returns A newly allocated string with the substitution done. The caller
 *      must deallocate it !
 */
xmlChar *
xmlStringDecodeEntities(xmlParserCtxtPtr ctxt, const xmlChar *str, int what,
		        xmlChar end, xmlChar  end2, xmlChar end3) {
    xmlChar *buffer = NULL;
    int buffer_size = 0;

    xmlChar *current = NULL;
    xmlEntityPtr ent;
    int c,l;
    int nbchars = 0;

    if (str == NULL)
	return(NULL);

    if (ctxt->depth > 40) {
	ctxt->errNo = XML_ERR_ENTITY_LOOP;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
		"Detected entity reference loop\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	return(NULL);
    }

    /*
     * allocate a translation buffer.
     */
    buffer_size = XML_PARSER_BIG_BUFFER_SIZE;
    buffer = (xmlChar *) xmlMalloc(buffer_size * sizeof(xmlChar));
    if (buffer == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		        "xmlStringDecodeEntities: malloc failed");
	return(NULL);
    }

    /*
     * OK loop until we reach one of the ending char or a size limit.
     * we are operating on already parsed values.
     */
    c = CUR_SCHAR(str, l);
    while ((c != 0) && (c != end) && /* non input consuming loop */
	   (c != end2) && (c != end3)) {

	if (c == 0) break;
        if ((c == '&') && (str[1] == '#')) {
	    int val = xmlParseStringCharRef(ctxt, &str);
	    if (val != 0) {
		COPY_BUF(0,buffer,nbchars,val);
	    }
	} else if ((c == '&') && (what & XML_SUBSTITUTE_REF)) {
	    if (xmlParserDebugEntities)
		xmlGenericError(xmlGenericErrorContext,
			"String decoding Entity Reference: %.30s\n",
			str);
	    ent = xmlParseStringEntityRef(ctxt, &str);
	    if ((ent != NULL) &&
		(ent->etype == XML_INTERNAL_PREDEFINED_ENTITY)) {
		if (ent->content != NULL) {
		    COPY_BUF(0,buffer,nbchars,ent->content[0]);
		} else {
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData,
			    "internal error entity has no content\n");
		}
	    } else if ((ent != NULL) && (ent->content != NULL)) {
		xmlChar *rep;

		ctxt->depth++;
		rep = xmlStringDecodeEntities(ctxt, ent->content, what,
			                      0, 0, 0);
		ctxt->depth--;
		if (rep != NULL) {
		    current = rep;
		    while (*current != 0) { /* non input consuming loop */
			buffer[nbchars++] = *current++;
			if (nbchars >
		            buffer_size - XML_PARSER_BUFFER_SIZE) {
			    growBuffer(buffer);
			}
		    }
		    xmlFree(rep);
		}
	    } else if (ent != NULL) {
		int i = xmlStrlen(ent->name);
		const xmlChar *cur = ent->name;

		buffer[nbchars++] = '&';
		if (nbchars > buffer_size - i - XML_PARSER_BUFFER_SIZE) {
		    growBuffer(buffer);
		}
		for (;i > 0;i--)
		    buffer[nbchars++] = *cur++;
		buffer[nbchars++] = ';';
	    }
	} else if (c == '%' && (what & XML_SUBSTITUTE_PEREF)) {
	    if (xmlParserDebugEntities)
		xmlGenericError(xmlGenericErrorContext,
			"String decoding PE Reference: %.30s\n", str);
	    ent = xmlParseStringPEReference(ctxt, &str);
	    if (ent != NULL) {
		xmlChar *rep;

		ctxt->depth++;
		rep = xmlStringDecodeEntities(ctxt, ent->content, what,
			                      0, 0, 0);
		ctxt->depth--;
		if (rep != NULL) {
		    current = rep;
		    while (*current != 0) { /* non input consuming loop */
			buffer[nbchars++] = *current++;
			if (nbchars >
		            buffer_size - XML_PARSER_BUFFER_SIZE) {
			    growBuffer(buffer);
			}
		    }
		    xmlFree(rep);
		}
	    }
	} else {
	    COPY_BUF(l,buffer,nbchars,c);
	    str += l;
	    if (nbchars > buffer_size - XML_PARSER_BUFFER_SIZE) {
	      growBuffer(buffer);
	    }
	}
	c = CUR_SCHAR(str, l);
    }
    buffer[nbchars++] = 0;
    return(buffer);
}


/************************************************************************
 *									*
 *		Commodity functions to handle xmlChars			*
 *									*
 ************************************************************************/

/**
 * xmlStrndup:
 * @cur:  the input xmlChar *
 * @len:  the len of @cur
 *
 * a strndup for array of xmlChar's
 *
 * Returns a new xmlChar * or NULL
 */
xmlChar *
xmlStrndup(const xmlChar *cur, int len) {
    xmlChar *ret;
    
    if ((cur == NULL) || (len < 0)) return(NULL);
    ret = (xmlChar *) xmlMalloc((len + 1) * sizeof(xmlChar));
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"malloc of %ld byte failed\n",
	        (len + 1) * (long)sizeof(xmlChar));
        return(NULL);
    }
    memcpy(ret, cur, len * sizeof(xmlChar));
    ret[len] = 0;
    return(ret);
}

/**
 * xmlStrdup:
 * @cur:  the input xmlChar *
 *
 * a strdup for array of xmlChar's. Since they are supposed to be
 * encoded in UTF-8 or an encoding with 8bit based chars, we assume
 * a termination mark of '0'.
 *
 * Returns a new xmlChar * or NULL
 */
xmlChar *
xmlStrdup(const xmlChar *cur) {
    const xmlChar *p = cur;

    if (cur == NULL) return(NULL);
    while (*p != 0) p++; /* non input consuming */
    return(xmlStrndup(cur, p - cur));
}

/**
 * xmlCharStrndup:
 * @cur:  the input char *
 * @len:  the len of @cur
 *
 * a strndup for char's to xmlChar's
 *
 * Returns a new xmlChar * or NULL
 */

xmlChar *
xmlCharStrndup(const char *cur, int len) {
    int i;
    xmlChar *ret;
    
    if ((cur == NULL) || (len < 0)) return(NULL);
    ret = (xmlChar *) xmlMalloc((len + 1) * sizeof(xmlChar));
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext, "malloc of %ld byte failed\n",
	        (len + 1) * (long)sizeof(xmlChar));
        return(NULL);
    }
    for (i = 0;i < len;i++)
        ret[i] = (xmlChar) cur[i];
    ret[len] = 0;
    return(ret);
}

/**
 * xmlCharStrdup:
 * @cur:  the input char *
 *
 * a strdup for char's to xmlChar's
 *
 * Returns a new xmlChar * or NULL
 */

xmlChar *
xmlCharStrdup(const char *cur) {
    const char *p = cur;

    if (cur == NULL) return(NULL);
    while (*p != '\0') p++; /* non input consuming */
    return(xmlCharStrndup(cur, p - cur));
}

/**
 * xmlStrcmp:
 * @str1:  the first xmlChar *
 * @str2:  the second xmlChar *
 *
 * a strcmp for xmlChar's
 *
 * Returns the integer result of the comparison
 */

int
xmlStrcmp(const xmlChar *str1, const xmlChar *str2) {
    register int tmp;

    if (str1 == str2) return(0);
    if (str1 == NULL) return(-1);
    if (str2 == NULL) return(1);
    do {
        tmp = *str1++ - *str2;
	if (tmp != 0) return(tmp);
    } while (*str2++ != 0);
    return 0;
}

/**
 * xmlStrEqual:
 * @str1:  the first xmlChar *
 * @str2:  the second xmlChar *
 *
 * Check if both string are equal of have same content
 * Should be a bit more readable and faster than xmlStrEqual()
 *
 * Returns 1 if they are equal, 0 if they are different
 */

int
xmlStrEqual(const xmlChar *str1, const xmlChar *str2) {
    if (str1 == str2) return(1);
    if (str1 == NULL) return(0);
    if (str2 == NULL) return(0);
    do {
	if (*str1++ != *str2) return(0);
    } while (*str2++);
    return(1);
}

/**
 * xmlStrncmp:
 * @str1:  the first xmlChar *
 * @str2:  the second xmlChar *
 * @len:  the max comparison length
 *
 * a strncmp for xmlChar's
 *
 * Returns the integer result of the comparison
 */

int
xmlStrncmp(const xmlChar *str1, const xmlChar *str2, int len) {
    register int tmp;

    if (len <= 0) return(0);
    if (str1 == str2) return(0);
    if (str1 == NULL) return(-1);
    if (str2 == NULL) return(1);
    do {
        tmp = *str1++ - *str2;
	if (tmp != 0 || --len == 0) return(tmp);
    } while (*str2++ != 0);
    return 0;
}

static const xmlChar casemap[256] = {
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
    0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
    0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
    0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
    0x40,0x61,0x62,0x63,0x64,0x65,0x66,0x67,
    0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
    0x78,0x79,0x7A,0x7B,0x5C,0x5D,0x5E,0x5F,
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,
    0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
    0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,
    0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,
    0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
    0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,
    0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
    0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,
    0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
    0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
    0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
    0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,
    0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
    0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,
    0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,
    0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,
    0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF
};

/**
 * xmlStrcasecmp:
 * @str1:  the first xmlChar *
 * @str2:  the second xmlChar *
 *
 * a strcasecmp for xmlChar's
 *
 * Returns the integer result of the comparison
 */

int
xmlStrcasecmp(const xmlChar *str1, const xmlChar *str2) {
    register int tmp;

    if (str1 == str2) return(0);
    if (str1 == NULL) return(-1);
    if (str2 == NULL) return(1);
    do {
	tmp = casemap[*str1++] - casemap[*str2];
	if (tmp != 0) return(tmp);
    } while (*str2++ != 0);
    return 0;
}

/**
 * xmlStrncasecmp:
 * @str1:  the first xmlChar *
 * @str2:  the second xmlChar *
 * @len:  the max comparison length
 *
 * a strncasecmp for xmlChar's
 *
 * Returns the integer result of the comparison
 */

int
xmlStrncasecmp(const xmlChar *str1, const xmlChar *str2, int len) {
    register int tmp;

    if (len <= 0) return(0);
    if (str1 == str2) return(0);
    if (str1 == NULL) return(-1);
    if (str2 == NULL) return(1);
    do {
	tmp = casemap[*str1++] - casemap[*str2];
	if (tmp != 0 || --len == 0) return(tmp);
    } while (*str2++ != 0);
    return 0;
}

/**
 * xmlStrchr:
 * @str:  the xmlChar * array
 * @val:  the xmlChar to search
 *
 * a strchr for xmlChar's
 *
 * Returns the xmlChar * for the first occurrence or NULL.
 */

const xmlChar *
xmlStrchr(const xmlChar *str, xmlChar val) {
    if (str == NULL) return(NULL);
    while (*str != 0) { /* non input consuming */
        if (*str == val) return((xmlChar *) str);
	str++;
    }
    return(NULL);
}

/**
 * xmlStrstr:
 * @str:  the xmlChar * array (haystack)
 * @val:  the xmlChar to search (needle)
 *
 * a strstr for xmlChar's
 *
 * Returns the xmlChar * for the first occurrence or NULL.
 */

const xmlChar *
xmlStrstr(const xmlChar *str, const xmlChar *val) {
    int n;
    
    if (str == NULL) return(NULL);
    if (val == NULL) return(NULL);
    n = xmlStrlen(val);

    if (n == 0) return(str);
    while (*str != 0) { /* non input consuming */
        if (*str == *val) {
	    if (!xmlStrncmp(str, val, n)) return((const xmlChar *) str);
	}
	str++;
    }
    return(NULL);
}

/**
 * xmlStrcasestr:
 * @str:  the xmlChar * array (haystack)
 * @val:  the xmlChar to search (needle)
 *
 * a case-ignoring strstr for xmlChar's
 *
 * Returns the xmlChar * for the first occurrence or NULL.
 */

const xmlChar *
xmlStrcasestr(const xmlChar *str, xmlChar *val) {
    int n;
    
    if (str == NULL) return(NULL);
    if (val == NULL) return(NULL);
    n = xmlStrlen(val);

    if (n == 0) return(str);
    while (*str != 0) { /* non input consuming */
	if (casemap[*str] == casemap[*val])
	    if (!xmlStrncasecmp(str, val, n)) return(str);
	str++;
    }
    return(NULL);
}

/**
 * xmlStrsub:
 * @str:  the xmlChar * array (haystack)
 * @start:  the index of the first char (zero based)
 * @len:  the length of the substring
 *
 * Extract a substring of a given string
 *
 * Returns the xmlChar * for the first occurrence or NULL.
 */

xmlChar *
xmlStrsub(const xmlChar *str, int start, int len) {
    int i;
    
    if (str == NULL) return(NULL);
    if (start < 0) return(NULL);
    if (len < 0) return(NULL);

    for (i = 0;i < start;i++) {
        if (*str == 0) return(NULL);
	str++;
    }
    if (*str == 0) return(NULL);
    return(xmlStrndup(str, len));
}

/**
 * xmlStrlen:
 * @str:  the xmlChar * array
 *
 * length of a xmlChar's string
 *
 * Returns the number of xmlChar contained in the ARRAY.
 */

int
xmlStrlen(const xmlChar *str) {
    int len = 0;

    if (str == NULL) return(0);
    while (*str != 0) { /* non input consuming */
	str++;
	len++;
    }
    return(len);
}

/**
 * xmlStrncat:
 * @cur:  the original xmlChar * array
 * @add:  the xmlChar * array added
 * @len:  the length of @add
 *
 * a strncat for array of xmlChar's, it will extend @cur with the len
 * first bytes of @add.
 *
 * Returns a new xmlChar *, the original @cur is reallocated if needed
 * and should not be freed
 */

xmlChar *
xmlStrncat(xmlChar *cur, const xmlChar *add, int len) {
    int size;
    xmlChar *ret;

    if ((add == NULL) || (len == 0))
        return(cur);
    if (cur == NULL)
        return(xmlStrndup(add, len));

    size = xmlStrlen(cur);
    ret = (xmlChar *) xmlRealloc(cur, (size + len + 1) * sizeof(xmlChar));
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlStrncat: realloc of %ld byte failed\n",
	        (size + len + 1) * (long)sizeof(xmlChar));
        return(cur);
    }
    memcpy(&ret[size], add, len * sizeof(xmlChar));
    ret[size + len] = 0;
    return(ret);
}

/**
 * xmlStrcat:
 * @cur:  the original xmlChar * array
 * @add:  the xmlChar * array added
 *
 * a strcat for array of xmlChar's. Since they are supposed to be
 * encoded in UTF-8 or an encoding with 8bit based chars, we assume
 * a termination mark of '0'.
 *
 * Returns a new xmlChar * containing the concatenated string.
 */
xmlChar *
xmlStrcat(xmlChar *cur, const xmlChar *add) {
    const xmlChar *p = add;

    if (add == NULL) return(cur);
    if (cur == NULL) 
        return(xmlStrdup(add));

    while (*p != 0) p++; /* non input consuming */
    return(xmlStrncat(cur, add, p - add));
}

/************************************************************************
 *									*
 *		Commodity functions, cleanup needed ?			*
 *									*
 ************************************************************************/

/**
 * areBlanks:
 * @ctxt:  an XML parser context
 * @str:  a xmlChar *
 * @len:  the size of @str
 *
 * Is this a sequence of blank chars that one can ignore ?
 *
 * Returns 1 if ignorable 0 otherwise.
 */

static int areBlanks(xmlParserCtxtPtr ctxt, const xmlChar *str, int len) {
    int i, ret;
    xmlNodePtr lastChild;

    /*
     * Don't spend time trying to differentiate them, the same callback is
     * used !
     */
    if (ctxt->sax->ignorableWhitespace == ctxt->sax->characters)
	return(0);

    /*
     * Check for xml:space value.
     */
    if (*(ctxt->space) == 1)
	return(0);

    /*
     * Check that the string is made of blanks
     */
    for (i = 0;i < len;i++)
        if (!(IS_BLANK(str[i]))) return(0);

    /*
     * Look if the element is mixed content in the DTD if available
     */
    if (ctxt->node == NULL) return(0);
    if (ctxt->myDoc != NULL) {
	ret = xmlIsMixedElement(ctxt->myDoc, ctxt->node->name);
        if (ret == 0) return(1);
        if (ret == 1) return(0);
    }

    /*
     * Otherwise, heuristic :-\
     */
    if (RAW != '<') return(0);
    if ((ctxt->node->children == NULL) &&
	(RAW == '<') && (NXT(1) == '/')) return(0);

    lastChild = xmlGetLastChild(ctxt->node);
    if (lastChild == NULL) {
        if ((ctxt->node->type != XML_ELEMENT_NODE) &&
            (ctxt->node->content != NULL)) return(0);
    } else if (xmlNodeIsText(lastChild))
        return(0);
    else if ((ctxt->node->children != NULL) &&
             (xmlNodeIsText(ctxt->node->children)))
        return(0);
    return(1);
}

/************************************************************************
 *									*
 *		Extra stuff for namespace support			*
 *	Relates to http://www.w3.org/TR/WD-xml-names			*
 *									*
 ************************************************************************/

/**
 * xmlSplitQName:
 * @ctxt:  an XML parser context
 * @name:  an XML parser context
 * @prefix:  a xmlChar ** 
 *
 * parse an UTF8 encoded XML qualified name string
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
xmlSplitQName(xmlParserCtxtPtr ctxt, const xmlChar *name, xmlChar **prefix) {
    xmlChar buf[XML_MAX_NAMELEN + 5];
    xmlChar *buffer = NULL;
    int len = 0;
    int max = XML_MAX_NAMELEN;
    xmlChar *ret = NULL;
    const xmlChar *cur = name;
    int c;

    *prefix = NULL;

#ifndef XML_XML_NAMESPACE
    /* xml: prefix is not really a namespace */
    if ((cur[0] == 'x') && (cur[1] == 'm') &&
        (cur[2] == 'l') && (cur[3] == ':'))
	return(xmlStrdup(name));
#endif

    /* nasty but valid */
    if (cur[0] == ':')
	return(xmlStrdup(name));

    c = *cur++;
    while ((c != 0) && (c != ':') && (len < max)) { /* tested bigname.xml */
	buf[len++] = c;
	c = *cur++;
    }
    if (len >= max) {
	/*
	 * Okay someone managed to make a huge name, so he's ready to pay
	 * for the processing speed.
	 */
	max = len * 2;
	
	buffer = (xmlChar *) xmlMalloc(max * sizeof(xmlChar));
	if (buffer == NULL) {
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData,
				 "xmlSplitQName: out of memory\n");
	    return(NULL);
	}
	memcpy(buffer, buf, len);
	while ((c != 0) && (c != ':')) { /* tested bigname.xml */
	    if (len + 10 > max) {
		max *= 2;
		buffer = (xmlChar *) xmlRealloc(buffer,
						max * sizeof(xmlChar));
		if (buffer == NULL) {
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData,
					 "xmlSplitQName: out of memory\n");
		    return(NULL);
		}
	    }
	    buffer[len++] = c;
	    c = *cur++;
	}
	buffer[len] = 0;
    }
    
    if (buffer == NULL)
	ret = xmlStrndup(buf, len);
    else {
	ret = buffer;
	buffer = NULL;
	max = XML_MAX_NAMELEN;
    }


    if (c == ':') {
	c = *cur;
	if (c == 0) return(ret);
        *prefix = ret;
	len = 0;

	/*
	 * Check that the first character is proper to start
	 * a new name
	 */
	if (!(((c >= 0x61) && (c <= 0x7A)) ||
	      ((c >= 0x41) && (c <= 0x5A)) ||
	      (c == '_') || (c == ':'))) {
	    int l;
	    int first = CUR_SCHAR(cur, l);

	    if (!IS_LETTER(first) && (first != '_')) {
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
			    "Name %s is not XML Namespace compliant\n",
			             name);
	    }
	}
	cur++;

	while ((c != 0) && (len < max)) { /* tested bigname2.xml */
	    buf[len++] = c;
	    c = *cur++;
	}
	if (len >= max) {
	    /*
	     * Okay someone managed to make a huge name, so he's ready to pay
	     * for the processing speed.
	     */
	    max = len * 2;
	    
	    buffer = (xmlChar *) xmlMalloc(max * sizeof(xmlChar));
	    if (buffer == NULL) {
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
				     "xmlSplitQName: out of memory\n");
		return(NULL);
	    }
	    memcpy(buffer, buf, len);
	    while (c != 0) { /* tested bigname2.xml */
		if (len + 10 > max) {
		    max *= 2;
		    buffer = (xmlChar *) xmlRealloc(buffer,
						    max * sizeof(xmlChar));
		    if (buffer == NULL) {
			if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			    ctxt->sax->error(ctxt->userData,
					     "xmlSplitQName: out of memory\n");
			return(NULL);
		    }
		}
		buffer[len++] = c;
		c = *cur++;
	    }
	    buffer[len] = 0;
	}
	
	if (buffer == NULL)
	    ret = xmlStrndup(buf, len);
	else {
	    ret = buffer;
	}
    }

    return(ret);
}

/************************************************************************
 *									*
 *			The parser itself				*
 *	Relates to http://www.w3.org/TR/REC-xml				*
 *									*
 ************************************************************************/

static xmlChar * xmlParseNameComplex(xmlParserCtxtPtr ctxt);
/**
 * xmlParseName:
 * @ctxt:  an XML parser context
 *
 * parse an XML name.
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
xmlParseName(xmlParserCtxtPtr ctxt) {
    const xmlChar *in;
    xmlChar *ret;
    int count = 0;

    GROW;

    /*
     * Accelerator for simple ASCII names
     */
    in = ctxt->input->cur;
    if (((*in >= 0x61) && (*in <= 0x7A)) ||
	((*in >= 0x41) && (*in <= 0x5A)) ||
	(*in == '_') || (*in == ':')) {
	in++;
	while (((*in >= 0x61) && (*in <= 0x7A)) ||
	       ((*in >= 0x41) && (*in <= 0x5A)) ||
	       ((*in >= 0x30) && (*in <= 0x39)) ||
	       (*in == '_') || (*in == '-') ||
	       (*in == ':') || (*in == '.'))
	    in++;
	if ((*in > 0) && (*in < 0x80)) {
	    count = in - ctxt->input->cur;
	    ret = xmlStrndup(ctxt->input->cur, count);
	    ctxt->input->cur = in;
	    return(ret);
	}
    }
    return(xmlParseNameComplex(ctxt));
}

/**
 * xmlParseNameAndCompare:
 * @ctxt:  an XML parser context
 *
 * parse an XML name and compares for match
 * (specialized for endtag parsing)
 *
 *
 * Returns NULL for an illegal name, (xmlChar*) 1 for success
 * and the name for mismatch
 */

static xmlChar *
xmlParseNameAndCompare(xmlParserCtxtPtr ctxt, xmlChar const *other) {
    const xmlChar *cmp = other;
    const xmlChar *in;
    xmlChar *ret;

    GROW;
    
    in = ctxt->input->cur;
    while (*in != 0 && *in == *cmp) {
    	++in;
	++cmp;
    }
    if (*cmp == 0 && (*in == '>' || IS_BLANK (*in))) {
    	/* success */
	ctxt->input->cur = in;
	return (xmlChar*) 1;
    }
    /* failure (or end of input buffer), check with full function */
    ret = xmlParseName (ctxt);
    if (ret != 0 && xmlStrEqual (ret, other)) {
    	xmlFree (ret);
	return (xmlChar*) 1;
    }
    return ret;
}

static xmlChar *
xmlParseNameComplex(xmlParserCtxtPtr ctxt) {
    xmlChar buf[XML_MAX_NAMELEN + 5];
    int len = 0, l;
    int c;
    int count = 0;

    /*
     * Handler for more complex cases
     */
    GROW;
    c = CUR_CHAR(l);
    if ((c == ' ') || (c == '>') || (c == '/') || /* accelerators */
	(!IS_LETTER(c) && (c != '_') &&
         (c != ':'))) {
	return(NULL);
    }

    while ((c != ' ') && (c != '>') && (c != '/') && /* test bigname.xml */
	   ((IS_LETTER(c)) || (IS_DIGIT(c)) ||
            (c == '.') || (c == '-') ||
	    (c == '_') || (c == ':') || 
	    (IS_COMBINING(c)) ||
	    (IS_EXTENDER(c)))) {
	if (count++ > 100) {
	    count = 0;
	    GROW;
	}
	COPY_BUF(l,buf,len,c);
	NEXTL(l);
	c = CUR_CHAR(l);
	if (len >= XML_MAX_NAMELEN) {
	    /*
	     * Okay someone managed to make a huge name, so he's ready to pay
	     * for the processing speed.
	     */
	    xmlChar *buffer;
	    int max = len * 2;
	    
	    buffer = (xmlChar *) xmlMalloc(max * sizeof(xmlChar));
	    if (buffer == NULL) {
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
			             "xmlParseNameComplex: out of memory\n");
		return(NULL);
	    }
	    memcpy(buffer, buf, len);
	    while ((IS_LETTER(c)) || (IS_DIGIT(c)) || /* test bigname.xml */
		   (c == '.') || (c == '-') ||
		   (c == '_') || (c == ':') || 
		   (IS_COMBINING(c)) ||
		   (IS_EXTENDER(c))) {
		if (count++ > 100) {
		    count = 0;
		    GROW;
		}
		if (len + 10 > max) {
		    max *= 2;
		    buffer = (xmlChar *) xmlRealloc(buffer,
			                            max * sizeof(xmlChar));
		    if (buffer == NULL) {
			if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			    ctxt->sax->error(ctxt->userData,
				     "xmlParseNameComplex: out of memory\n");
			return(NULL);
		    }
		}
		COPY_BUF(l,buffer,len,c);
		NEXTL(l);
		c = CUR_CHAR(l);
	    }
	    buffer[len] = 0;
	    return(buffer);
	}
    }
    return(xmlStrndup(buf, len));
}

/**
 * xmlParseStringName:
 * @ctxt:  an XML parser context
 * @str:  a pointer to the string pointer (IN/OUT)
 *
 * parse an XML name.
 *
 * [4] NameChar ::= Letter | Digit | '.' | '-' | '_' | ':' |
 *                  CombiningChar | Extender
 *
 * [5] Name ::= (Letter | '_' | ':') (NameChar)*
 *
 * [6] Names ::= Name (S Name)*
 *
 * Returns the Name parsed or NULL. The @str pointer 
 * is updated to the current location in the string.
 */

static xmlChar *
xmlParseStringName(xmlParserCtxtPtr ctxt, const xmlChar** str) {
    xmlChar buf[XML_MAX_NAMELEN + 5];
    const xmlChar *cur = *str;
    int len = 0, l;
    int c;

    c = CUR_SCHAR(cur, l);
    if (!IS_LETTER(c) && (c != '_') &&
        (c != ':')) {
	return(NULL);
    }

    while ((IS_LETTER(c)) || (IS_DIGIT(c)) || /* test bigentname.xml */
           (c == '.') || (c == '-') ||
	   (c == '_') || (c == ':') || 
	   (IS_COMBINING(c)) ||
	   (IS_EXTENDER(c))) {
	COPY_BUF(l,buf,len,c);
	cur += l;
	c = CUR_SCHAR(cur, l);
	if (len >= XML_MAX_NAMELEN) { /* test bigentname.xml */
	    /*
	     * Okay someone managed to make a huge name, so he's ready to pay
	     * for the processing speed.
	     */
	    xmlChar *buffer;
	    int max = len * 2;
	    
	    buffer = (xmlChar *) xmlMalloc(max * sizeof(xmlChar));
	    if (buffer == NULL) {
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
			             "xmlParseStringName: out of memory\n");
		return(NULL);
	    }
	    memcpy(buffer, buf, len);
	    while ((IS_LETTER(c)) || (IS_DIGIT(c)) || /* test bigentname.xml */
		   (c == '.') || (c == '-') ||
		   (c == '_') || (c == ':') || 
		   (IS_COMBINING(c)) ||
		   (IS_EXTENDER(c))) {
		if (len + 10 > max) {
		    max *= 2;
		    buffer = (xmlChar *) xmlRealloc(buffer,
			                            max * sizeof(xmlChar));
		    if (buffer == NULL) {
			if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			    ctxt->sax->error(ctxt->userData,
				     "xmlParseStringName: out of memory\n");
			return(NULL);
		    }
		}
		COPY_BUF(l,buffer,len,c);
		cur += l;
		c = CUR_SCHAR(cur, l);
	    }
	    buffer[len] = 0;
	    *str = cur;
	    return(buffer);
	}
    }
    *str = cur;
    return(xmlStrndup(buf, len));
}

/**
 * xmlParseNmtoken:
 * @ctxt:  an XML parser context
 * 
 * parse an XML Nmtoken.
 *
 * [7] Nmtoken ::= (NameChar)+
 *
 * [8] Nmtokens ::= Nmtoken (S Nmtoken)*
 *
 * Returns the Nmtoken parsed or NULL
 */

xmlChar *
xmlParseNmtoken(xmlParserCtxtPtr ctxt) {
    xmlChar buf[XML_MAX_NAMELEN + 5];
    int len = 0, l;
    int c;
    int count = 0;

    GROW;
    c = CUR_CHAR(l);

    while ((IS_LETTER(c)) || (IS_DIGIT(c)) || /* test bigtoken.xml */
           (c == '.') || (c == '-') ||
	   (c == '_') || (c == ':') || 
	   (IS_COMBINING(c)) ||
	   (IS_EXTENDER(c))) {
	if (count++ > 100) {
	    count = 0;
	    GROW;
	}
	COPY_BUF(l,buf,len,c);
	NEXTL(l);
	c = CUR_CHAR(l);
	if (len >= XML_MAX_NAMELEN) {
	    /*
	     * Okay someone managed to make a huge token, so he's ready to pay
	     * for the processing speed.
	     */
	    xmlChar *buffer;
	    int max = len * 2;
	    
	    buffer = (xmlChar *) xmlMalloc(max * sizeof(xmlChar));
	    if (buffer == NULL) {
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
			             "xmlParseNmtoken: out of memory\n");
		return(NULL);
	    }
	    memcpy(buffer, buf, len);
	    while ((IS_LETTER(c)) || (IS_DIGIT(c)) || /* test bigtoken.xml */
		   (c == '.') || (c == '-') ||
		   (c == '_') || (c == ':') || 
		   (IS_COMBINING(c)) ||
		   (IS_EXTENDER(c))) {
		if (count++ > 100) {
		    count = 0;
		    GROW;
		}
		if (len + 10 > max) {
		    max *= 2;
		    buffer = (xmlChar *) xmlRealloc(buffer,
			                            max * sizeof(xmlChar));
		    if (buffer == NULL) {
			if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			    ctxt->sax->error(ctxt->userData,
				     "xmlParseNmtoken: out of memory\n");
			return(NULL);
		    }
		}
		COPY_BUF(l,buffer,len,c);
		NEXTL(l);
		c = CUR_CHAR(l);
	    }
	    buffer[len] = 0;
	    return(buffer);
	}
    }
    if (len == 0)
        return(NULL);
    return(xmlStrndup(buf, len));
}

/**
 * xmlParseEntityValue:
 * @ctxt:  an XML parser context
 * @orig:  if non-NULL store a copy of the original entity value
 *
 * parse a value for ENTITY declarations
 *
 * [9] EntityValue ::= '"' ([^%&"] | PEReference | Reference)* '"' |
 *	               "'" ([^%&'] | PEReference | Reference)* "'"
 *
 * Returns the EntityValue parsed with reference substituted or NULL
 */

xmlChar *
xmlParseEntityValue(xmlParserCtxtPtr ctxt, xmlChar **orig) {
    xmlChar *buf = NULL;
    int len = 0;
    int size = XML_PARSER_BUFFER_SIZE;
    int c, l;
    xmlChar stop;
    xmlChar *ret = NULL;
    const xmlChar *cur = NULL;
    xmlParserInputPtr input;

    if (RAW == '"') stop = '"';
    else if (RAW == '\'') stop = '\'';
    else {
	ctxt->errNo = XML_ERR_ENTITY_NOT_STARTED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, "EntityValue: \" or ' expected\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	return(NULL);
    }
    buf = (xmlChar *) xmlMalloc(size * sizeof(xmlChar));
    if (buf == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		"malloc of %d byte failed\n", size);
	return(NULL);
    }

    /*
     * The content of the entity definition is copied in a buffer.
     */

    ctxt->instate = XML_PARSER_ENTITY_VALUE;
    input = ctxt->input;
    GROW;
    NEXT;
    c = CUR_CHAR(l);
    /*
     * NOTE: 4.4.5 Included in Literal
     * When a parameter entity reference appears in a literal entity
     * value, ... a single or double quote character in the replacement
     * text is always treated as a normal data character and will not
     * terminate the literal. 
     * In practice it means we stop the loop only when back at parsing
     * the initial entity and the quote is found
     */
    while ((IS_CHAR(c)) && ((c != stop) || /* checked */
	   (ctxt->input != input))) {
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
	/*
	 * Pop-up of finished entities.
	 */
	while ((RAW == 0) && (ctxt->inputNr > 1)) /* non input consuming */
	    xmlPopInput(ctxt);

	GROW;
	c = CUR_CHAR(l);
	if (c == 0) {
	    GROW;
	    c = CUR_CHAR(l);
	}
    }
    buf[len] = 0;

    /*
     * Raise problem w.r.t. '&' and '%' being used in non-entities
     * reference constructs. Note Charref will be handled in
     * xmlStringDecodeEntities()
     */
    cur = buf;
    while (*cur != 0) { /* non input consuming */
	if ((*cur == '%') || ((*cur == '&') && (cur[1] != '#'))) {
	    xmlChar *name;
	    xmlChar tmp = *cur;

	    cur++;
	    name = xmlParseStringName(ctxt, &cur);
            if ((name == NULL) || (*cur != ';')) {
		ctxt->errNo = XML_ERR_ENTITY_CHAR_ERROR;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
	    "EntityValue: '%c' forbidden except for entities references\n",
	                             tmp);
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    }
	    if ((tmp == '%') && (ctxt->inSubset == 1) &&
		(ctxt->inputNr == 1)) {
		ctxt->errNo = XML_ERR_ENTITY_PE_INTERNAL;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
	    "EntityValue: PEReferences forbidden in internal subset\n",
	                             tmp);
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    }
	    if (name != NULL)
		xmlFree(name);
	}
	cur++;
    }

    /*
     * Then PEReference entities are substituted.
     */
    if (c != stop) {
	ctxt->errNo = XML_ERR_ENTITY_NOT_FINISHED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, "EntityValue: \" expected\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	xmlFree(buf);
    } else {
	NEXT;
	/*
	 * NOTE: 4.4.7 Bypassed
	 * When a general entity reference appears in the EntityValue in
	 * an entity declaration, it is bypassed and left as is.
	 * so XML_SUBSTITUTE_REF is not set here.
	 */
	ret = xmlStringDecodeEntities(ctxt, buf, XML_SUBSTITUTE_PEREF,
				      0, 0, 0);
	if (orig != NULL) 
	    *orig = buf;
	else
	    xmlFree(buf);
    }
    
    return(ret);
}

/**
 * xmlParseAttValue:
 * @ctxt:  an XML parser context
 *
 * parse a value for an attribute
 * Note: the parser won't do substitution of entities here, this
 * will be handled later in xmlStringGetNodeList
 *
 * [10] AttValue ::= '"' ([^<&"] | Reference)* '"' |
 *                   "'" ([^<&'] | Reference)* "'"
 *
 * 3.3.3 Attribute-Value Normalization:
 * Before the value of an attribute is passed to the application or
 * checked for validity, the XML processor must normalize it as follows: 
 * - a character reference is processed by appending the referenced
 *   character to the attribute value
 * - an entity reference is processed by recursively processing the
 *   replacement text of the entity 
 * - a whitespace character (#x20, #xD, #xA, #x9) is processed by
 *   appending #x20 to the normalized value, except that only a single
 *   #x20 is appended for a "#xD#xA" sequence that is part of an external
 *   parsed entity or the literal entity value of an internal parsed entity 
 * - other characters are processed by appending them to the normalized value 
 * If the declared value is not CDATA, then the XML processor must further
 * process the normalized attribute value by discarding any leading and
 * trailing space (#x20) characters, and by replacing sequences of space
 * (#x20) characters by a single space (#x20) character.  
 * All attributes for which no declaration has been read should be treated
 * by a non-validating parser as if declared CDATA.
 *
 * Returns the AttValue parsed or NULL. The value has to be freed by the caller.
 */

xmlChar *
xmlParseAttValueComplex(xmlParserCtxtPtr ctxt);

xmlChar *
xmlParseAttValue(xmlParserCtxtPtr ctxt) {
    xmlChar limit = 0;
    const xmlChar *in = NULL;
    xmlChar *ret = NULL;
    SHRINK;
    GROW;
    in = (xmlChar *) CUR_PTR;
    if (*in != '"' && *in != '\'') {
      ctxt->errNo = XML_ERR_ATTRIBUTE_NOT_STARTED;
      if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
          ctxt->sax->error(ctxt->userData, "AttValue: \" or ' expected\n");
      ctxt->wellFormed = 0;
      if (ctxt->recovery == 0) ctxt->disableSAX = 1;
      return(NULL);
    } 
    ctxt->instate = XML_PARSER_ATTRIBUTE_VALUE;
    limit = *in;
    ++in;
    
    while (*in != limit && *in >= 0x20 && *in <= 0x7f && 
      *in != '&' && *in != '<'
    ) {
      ++in;
    }
    if (*in != limit) {
      return xmlParseAttValueComplex(ctxt);
    }
    ++in;
    ret = xmlStrndup (CUR_PTR + 1, in - CUR_PTR - 2);
    CUR_PTR = in;
    return ret;
}

/**
 * xmlParseAttValueComplex:
 * @ctxt:  an XML parser context
 *
 * parse a value for an attribute, this is the fallback function
 * of xmlParseAttValue() when the attribute parsing requires handling
 * of non-ASCII characters.
 *
 * Returns the AttValue parsed or NULL. The value has to be freed by the caller.
 */
xmlChar *
xmlParseAttValueComplex(xmlParserCtxtPtr ctxt) {
    xmlChar limit = 0;
    xmlChar *buf = NULL;
    int len = 0;
    int buf_size = 0;
    int c, l;
    xmlChar *current = NULL;
    xmlEntityPtr ent;


    SHRINK;
    if (NXT(0) == '"') {
	ctxt->instate = XML_PARSER_ATTRIBUTE_VALUE;
	limit = '"';
        NEXT;
    } else if (NXT(0) == '\'') {
	limit = '\'';
	ctxt->instate = XML_PARSER_ATTRIBUTE_VALUE;
        NEXT;
    } else {
	ctxt->errNo = XML_ERR_ATTRIBUTE_NOT_STARTED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, "AttValue: \" or ' expected\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	return(NULL);
    }
    
    /*
     * allocate a translation buffer.
     */
    buf_size = XML_PARSER_BUFFER_SIZE;
    buf = (xmlChar *) xmlMalloc(buf_size * sizeof(xmlChar));
    if (buf == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		        "xmlParseAttValue: malloc failed");
	return(NULL);
    }

    /*
     * OK loop until we reach one of the ending char or a size limit.
     */
    c = CUR_CHAR(l);
    while ((NXT(0) != limit) && /* checked */
	   (c != '<')) {
	if (c == 0) break;
	if (c == '&') {
	    if (NXT(1) == '#') {
		int val = xmlParseCharRef(ctxt);
		if (val == '&') {
		    if (ctxt->replaceEntities) {
			if (len > buf_size - 10) {
			    growBuffer(buf);
			}
			buf[len++] = '&';
		    } else {
			/*
			 * The reparsing will be done in xmlStringGetNodeList()
			 * called by the attribute() function in SAX.c
			 */
			static xmlChar buffer[6] = "&#38;";

			if (len > buf_size - 10) {
			    growBuffer(buf);
			}
			current = &buffer[0];
			while (*current != 0) { /* non input consuming */
			    buf[len++] = *current++;
			}
		    }
		} else {
		    if (len > buf_size - 10) {
			growBuffer(buf);
		    }
		    len += xmlCopyChar(0, &buf[len], val);
		}
	    } else {
		ent = xmlParseEntityRef(ctxt);
		if ((ent != NULL) && 
		    (ctxt->replaceEntities != 0)) {
		    xmlChar *rep;

		    if (ent->etype != XML_INTERNAL_PREDEFINED_ENTITY) {
			rep = xmlStringDecodeEntities(ctxt, ent->content,
						      XML_SUBSTITUTE_REF, 0, 0, 0);
			if (rep != NULL) {
			    current = rep;
			    while (*current != 0) { /* non input consuming */
				buf[len++] = *current++;
				if (len > buf_size - 10) {
				    growBuffer(buf);
				}
			    }
			    xmlFree(rep);
			}
		    } else {
			if (len > buf_size - 10) {
			    growBuffer(buf);
			}
			if (ent->content != NULL)
			    buf[len++] = ent->content[0];
		    }
		} else if (ent != NULL) {
		    int i = xmlStrlen(ent->name);
		    const xmlChar *cur = ent->name;

		    /*
		     * This may look absurd but is needed to detect
		     * entities problems
		     */
		    if ((ent->etype != XML_INTERNAL_PREDEFINED_ENTITY) &&
			(ent->content != NULL)) {
			xmlChar *rep;
			rep = xmlStringDecodeEntities(ctxt, ent->content,
						      XML_SUBSTITUTE_REF, 0, 0, 0);
			if (rep != NULL)
			    xmlFree(rep);
		    }

		    /*
		     * Just output the reference
		     */
		    buf[len++] = '&';
		    if (len > buf_size - i - 10) {
			growBuffer(buf);
		    }
		    for (;i > 0;i--)
			buf[len++] = *cur++;
		    buf[len++] = ';';
		}
	    }
	} else {
	    if ((c == 0x20) || (c == 0xD) || (c == 0xA) || (c == 0x9)) {
		COPY_BUF(l,buf,len,0x20);
		if (len > buf_size - 10) {
		    growBuffer(buf);
		}
	    } else {
		COPY_BUF(l,buf,len,c);
		if (len > buf_size - 10) {
		    growBuffer(buf);
		}
	    }
	    NEXTL(l);
	}
	GROW;
	c = CUR_CHAR(l);
    }
    buf[len++] = 0;
    if (RAW == '<') {
	ctxt->errNo = XML_ERR_LT_IN_ATTRIBUTE;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	       "Unescaped '<' not allowed in attributes values\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    } else if (RAW != limit) {
	ctxt->errNo = XML_ERR_ATTRIBUTE_NOT_FINISHED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, "AttValue: ' expected\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    } else
	NEXT;
    return(buf);
}

/**
 * xmlParseSystemLiteral:
 * @ctxt:  an XML parser context
 * 
 * parse an XML Literal
 *
 * [11] SystemLiteral ::= ('"' [^"]* '"') | ("'" [^']* "'")
 *
 * Returns the SystemLiteral parsed or NULL
 */

xmlChar *
xmlParseSystemLiteral(xmlParserCtxtPtr ctxt) {
    xmlChar *buf = NULL;
    int len = 0;
    int size = XML_PARSER_BUFFER_SIZE;
    int cur, l;
    xmlChar stop;
    int state = ctxt->instate;
    int count = 0;

    SHRINK;
    if (RAW == '"') {
        NEXT;
	stop = '"';
    } else if (RAW == '\'') {
        NEXT;
	stop = '\'';
    } else {
	ctxt->errNo = XML_ERR_LITERAL_NOT_STARTED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	                     "SystemLiteral \" or ' expected\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	return(NULL);
    }
    
    buf = (xmlChar *) xmlMalloc(size * sizeof(xmlChar));
    if (buf == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		"malloc of %d byte failed\n", size);
	return(NULL);
    }
    ctxt->instate = XML_PARSER_SYSTEM_LITERAL;
    cur = CUR_CHAR(l);
    while ((IS_CHAR(cur)) && (cur != stop)) { /* checked */
	if (len + 5 >= size) {
	    size *= 2;
	    buf = (xmlChar *) xmlRealloc(buf, size * sizeof(xmlChar));
	    if (buf == NULL) {
		xmlGenericError(xmlGenericErrorContext,
			"realloc of %d byte failed\n", size);
		ctxt->instate = (xmlParserInputState) state;
		return(NULL);
	    }
	}
	count++;
	if (count > 50) {
	    GROW;
	    count = 0;
	}
	COPY_BUF(l,buf,len,cur);
	NEXTL(l);
	cur = CUR_CHAR(l);
	if (cur == 0) {
	    GROW;
	    SHRINK;
	    cur = CUR_CHAR(l);
	}
    }
    buf[len] = 0;
    ctxt->instate = (xmlParserInputState) state;
    if (!IS_CHAR(cur)) {
	ctxt->errNo = XML_ERR_LITERAL_NOT_FINISHED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, "Unfinished SystemLiteral\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    } else {
	NEXT;
    }
    return(buf);
}

/**
 * xmlParsePubidLiteral:
 * @ctxt:  an XML parser context
 *
 * parse an XML public literal
 *
 * [12] PubidLiteral ::= '"' PubidChar* '"' | "'" (PubidChar - "'")* "'"
 *
 * Returns the PubidLiteral parsed or NULL.
 */

xmlChar *
xmlParsePubidLiteral(xmlParserCtxtPtr ctxt) {
    xmlChar *buf = NULL;
    int len = 0;
    int size = XML_PARSER_BUFFER_SIZE;
    xmlChar cur;
    xmlChar stop;
    int count = 0;
    xmlParserInputState oldstate = ctxt->instate;

    SHRINK;
    if (RAW == '"') {
        NEXT;
	stop = '"';
    } else if (RAW == '\'') {
        NEXT;
	stop = '\'';
    } else {
	ctxt->errNo = XML_ERR_LITERAL_NOT_STARTED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	                     "SystemLiteral \" or ' expected\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	return(NULL);
    }
    buf = (xmlChar *) xmlMalloc(size * sizeof(xmlChar));
    if (buf == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		"malloc of %d byte failed\n", size);
	return(NULL);
    }
    ctxt->instate = XML_PARSER_PUBLIC_LITERAL;
    cur = CUR;
    while ((IS_PUBIDCHAR(cur)) && (cur != stop)) { /* checked */
	if (len + 1 >= size) {
	    size *= 2;
	    buf = (xmlChar *) xmlRealloc(buf, size * sizeof(xmlChar));
	    if (buf == NULL) {
		xmlGenericError(xmlGenericErrorContext,
			"realloc of %d byte failed\n", size);
		return(NULL);
	    }
	}
	buf[len++] = cur;
	count++;
	if (count > 50) {
	    GROW;
	    count = 0;
	}
	NEXT;
	cur = CUR;
	if (cur == 0) {
	    GROW;
	    SHRINK;
	    cur = CUR;
	}
    }
    buf[len] = 0;
    if (cur != stop) {
	ctxt->errNo = XML_ERR_LITERAL_NOT_FINISHED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, "Unfinished PubidLiteral\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    } else {
	NEXT;
    }
    ctxt->instate = oldstate;
    return(buf);
}

void xmlParseCharDataComplex(xmlParserCtxtPtr ctxt, int cdata);
/**
 * xmlParseCharData:
 * @ctxt:  an XML parser context
 * @cdata:  int indicating whether we are within a CDATA section
 *
 * parse a CharData section.
 * if we are within a CDATA section ']]>' marks an end of section.
 *
 * The right angle bracket (>) may be represented using the string "&gt;",
 * and must, for compatibility, be escaped using "&gt;" or a character
 * reference when it appears in the string "]]>" in content, when that
 * string is not marking the end of a CDATA section. 
 *
 * [14] CharData ::= [^<&]* - ([^<&]* ']]>' [^<&]*)
 */

void
xmlParseCharData(xmlParserCtxtPtr ctxt, int cdata) {
    const xmlChar *in;
    int nbchar = 0;
    int line = ctxt->input->line;
    int col = ctxt->input->col;

    SHRINK;
    GROW;
    /*
     * Accelerated common case where input don't need to be
     * modified before passing it to the handler.
     */
    if (!cdata) {
	in = ctxt->input->cur;
	do {
get_more:
	    while (((*in >= 0x20) && (*in != '<') && (*in != ']') &&
		    (*in != '&') && (*in <= 0x7F)) || (*in == 0x09))
		in++;
	    if (*in == 0xA) {
		ctxt->input->line++;
		in++;
		while (*in == 0xA) {
		    ctxt->input->line++;
		    in++;
		}
		goto get_more;
	    }
	    if (*in == ']') {
		if ((in[1] == ']') && (in[2] == '>')) {
		    ctxt->errNo = XML_ERR_MISPLACED_CDATA_END;
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData,
			   "Sequence ']]>' not allowed in content\n");
		    ctxt->input->cur = in;
		    ctxt->wellFormed = 0;
		    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		    return;
		}
		in++;
		goto get_more;
	    }
	    nbchar = in - ctxt->input->cur;
	    if (nbchar > 0) {
		if (IS_BLANK(*ctxt->input->cur)) {
		    const xmlChar *tmp = ctxt->input->cur;
		    ctxt->input->cur = in;
		    if (areBlanks(ctxt, tmp, nbchar)) {
			if (ctxt->sax->ignorableWhitespace != NULL)
			    ctxt->sax->ignorableWhitespace(ctxt->userData,
						   tmp, nbchar);
		    } else {
			if (ctxt->sax->characters != NULL)
			    ctxt->sax->characters(ctxt->userData,
						  tmp, nbchar);
		    }
                    line = ctxt->input->line;
                    col = ctxt->input->col;
		} else {
		    if (ctxt->sax->characters != NULL)
			ctxt->sax->characters(ctxt->userData,
					      ctxt->input->cur, nbchar);
                    line = ctxt->input->line;
                    col = ctxt->input->col;
		}
	    }
	    ctxt->input->cur = in;
	    if (*in == 0xD) {
		in++;
		if (*in == 0xA) {
		    ctxt->input->cur = in;
		    in++;
		    ctxt->input->line++;
		    continue; /* while */
		}
		in--;
	    }
	    if (*in == '<') {
		return;
	    }
	    if (*in == '&') {
		return;
	    }
	    SHRINK;
	    GROW;
	    in = ctxt->input->cur;
	} while ((*in >= 0x20) && (*in <= 0x7F));
	nbchar = 0;
    }
    ctxt->input->line = line;
    ctxt->input->col = col;
    xmlParseCharDataComplex(ctxt, cdata);
}

/**
 * xmlParseCharDataComplex:
 * @ctxt:  an XML parser context
 * @cdata:  int indicating whether we are within a CDATA section
 *
 * parse a CharData section.this is the fallback function
 * of xmlParseCharData() when the parsing requires handling
 * of non-ASCII characters.
 */
void
xmlParseCharDataComplex(xmlParserCtxtPtr ctxt, int cdata) {
    xmlChar buf[XML_PARSER_BIG_BUFFER_SIZE + 5];
    int nbchar = 0;
    int cur, l;
    int count = 0;

    SHRINK;
    GROW;
    cur = CUR_CHAR(l);
    while ((cur != '<') && /* checked */
           (cur != '&') && 
	   (IS_CHAR(cur))) /* test also done in xmlCurrentChar() */ {
	if ((cur == ']') && (NXT(1) == ']') &&
	    (NXT(2) == '>')) {
	    if (cdata) break;
	    else {
		ctxt->errNo = XML_ERR_MISPLACED_CDATA_END;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
		       "Sequence ']]>' not allowed in content\n");
		/* Should this be relaxed ??? I see a "must here */
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    }
	}
	COPY_BUF(l,buf,nbchar,cur);
	if (nbchar >= XML_PARSER_BIG_BUFFER_SIZE) {
	    /*
	     * OK the segment is to be consumed as chars.
	     */
	    if ((ctxt->sax != NULL) && (!ctxt->disableSAX)) {
		if (areBlanks(ctxt, buf, nbchar)) {
		    if (ctxt->sax->ignorableWhitespace != NULL)
			ctxt->sax->ignorableWhitespace(ctxt->userData,
			                               buf, nbchar);
		} else {
		    if (ctxt->sax->characters != NULL)
			ctxt->sax->characters(ctxt->userData, buf, nbchar);
		}
	    }
	    nbchar = 0;
	}
	count++;
	if (count > 50) {
	    GROW;
	    count = 0;
	}
	NEXTL(l);
	cur = CUR_CHAR(l);
    }
    if (nbchar != 0) {
	/*
	 * OK the segment is to be consumed as chars.
	 */
	if ((ctxt->sax != NULL) && (!ctxt->disableSAX)) {
	    if (areBlanks(ctxt, buf, nbchar)) {
		if (ctxt->sax->ignorableWhitespace != NULL)
		    ctxt->sax->ignorableWhitespace(ctxt->userData, buf, nbchar);
	    } else {
		if (ctxt->sax->characters != NULL)
		    ctxt->sax->characters(ctxt->userData, buf, nbchar);
	    }
	}
    }
}

/**
 * xmlParseExternalID:
 * @ctxt:  an XML parser context
 * @publicID:  a xmlChar** receiving PubidLiteral
 * @strict: indicate whether we should restrict parsing to only
 *          production [75], see NOTE below
 *
 * Parse an External ID or a Public ID
 *
 * NOTE: Productions [75] and [83] interact badly since [75] can generate
 *       'PUBLIC' S PubidLiteral S SystemLiteral
 *
 * [75] ExternalID ::= 'SYSTEM' S SystemLiteral
 *                   | 'PUBLIC' S PubidLiteral S SystemLiteral
 *
 * [83] PublicID ::= 'PUBLIC' S PubidLiteral
 *
 * Returns the function returns SystemLiteral and in the second
 *                case publicID receives PubidLiteral, is strict is off
 *                it is possible to return NULL and have publicID set.
 */

xmlChar *
xmlParseExternalID(xmlParserCtxtPtr ctxt, xmlChar **publicID, int strict) {
    xmlChar *URI = NULL;

    SHRINK;

    *publicID = NULL;
    if ((RAW == 'S') && (NXT(1) == 'Y') &&
         (NXT(2) == 'S') && (NXT(3) == 'T') &&
	 (NXT(4) == 'E') && (NXT(5) == 'M')) {
        SKIP(6);
	if (!IS_BLANK(CUR)) {
	    ctxt->errNo = XML_ERR_SPACE_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData,
		    "Space required after 'SYSTEM'\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	}
        SKIP_BLANKS;
	URI = xmlParseSystemLiteral(ctxt);
	if (URI == NULL) {
	    ctxt->errNo = XML_ERR_URI_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData,
	          "xmlParseExternalID: SYSTEM, no URI\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
        }
    } else if ((RAW == 'P') && (NXT(1) == 'U') &&
	       (NXT(2) == 'B') && (NXT(3) == 'L') &&
	       (NXT(4) == 'I') && (NXT(5) == 'C')) {
        SKIP(6);
	if (!IS_BLANK(CUR)) {
	    ctxt->errNo = XML_ERR_SPACE_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData,
		    "Space required after 'PUBLIC'\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	}
        SKIP_BLANKS;
	*publicID = xmlParsePubidLiteral(ctxt);
	if (*publicID == NULL) {
	    ctxt->errNo = XML_ERR_PUBID_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData, 
	          "xmlParseExternalID: PUBLIC, no Public Identifier\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	}
	if (strict) {
	    /*
	     * We don't handle [83] so "S SystemLiteral" is required.
	     */
	    if (!IS_BLANK(CUR)) {
		ctxt->errNo = XML_ERR_SPACE_REQUIRED;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
			"Space required after the Public Identifier\n");
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    }
	} else {
	    /*
	     * We handle [83] so we return immediately, if 
	     * "S SystemLiteral" is not detected. From a purely parsing
	     * point of view that's a nice mess.
	     */
	    const xmlChar *ptr;
	    GROW;

	    ptr = CUR_PTR;
	    if (!IS_BLANK(*ptr)) return(NULL);
	    
	    while (IS_BLANK(*ptr)) ptr++; /* TODO: dangerous, fix ! */
	    if ((*ptr != '\'') && (*ptr != '"')) return(NULL);
	}
        SKIP_BLANKS;
	URI = xmlParseSystemLiteral(ctxt);
	if (URI == NULL) {
	    ctxt->errNo = XML_ERR_URI_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData, 
	           "xmlParseExternalID: PUBLIC, no URI\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
        }
    }
    return(URI);
}

/**
 * xmlParseComment:
 * @ctxt:  an XML parser context
 *
 * Skip an XML (SGML) comment <!-- .... -->
 *  The spec says that "For compatibility, the string "--" (double-hyphen)
 *  must not occur within comments. "
 *
 * [15] Comment ::= '<!--' ((Char - '-') | ('-' (Char - '-')))* '-->'
 */
void
xmlParseComment(xmlParserCtxtPtr ctxt) {
    xmlChar *buf = NULL;
    int len;
    int size = XML_PARSER_BUFFER_SIZE;
    int q, ql;
    int r, rl;
    int cur, l;
    xmlParserInputState state;
    xmlParserInputPtr input = ctxt->input;
    int count = 0;

    /*
     * Check that there is a comment right here.
     */
    if ((RAW != '<') || (NXT(1) != '!') ||
        (NXT(2) != '-') || (NXT(3) != '-')) return;

    state = ctxt->instate;
    ctxt->instate = XML_PARSER_COMMENT;
    SHRINK;
    SKIP(4);
    buf = (xmlChar *) xmlMalloc(size * sizeof(xmlChar));
    if (buf == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		"malloc of %d byte failed\n", size);
	ctxt->instate = state;
	return;
    }
    q = CUR_CHAR(ql);
    NEXTL(ql);
    r = CUR_CHAR(rl);
    NEXTL(rl);
    cur = CUR_CHAR(l);
    len = 0;
    while (IS_CHAR(cur) && /* checked */
           ((cur != '>') ||
	    (r != '-') || (q != '-'))) {
	if ((r == '-') && (q == '-')) {
	    ctxt->errNo = XML_ERR_HYPHEN_IN_COMMENT;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData,
	       "Comment must not contain '--' (double-hyphen)`\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	}
	if (len + 5 >= size) {
	    size *= 2;
	    buf = (xmlChar *) xmlRealloc(buf, size * sizeof(xmlChar));
	    if (buf == NULL) {
		xmlGenericError(xmlGenericErrorContext,
			"realloc of %d byte failed\n", size);
		ctxt->instate = state;
		return;
	    }
	}
	COPY_BUF(ql,buf,len,q);
	q = r;
	ql = rl;
	r = cur;
	rl = l;

	count++;
	if (count > 50) {
	    GROW;
	    count = 0;
	}
	NEXTL(l);
	cur = CUR_CHAR(l);
	if (cur == 0) {
	    SHRINK;
	    GROW;
	    cur = CUR_CHAR(l);
	}
    }
    buf[len] = 0;
    if (!IS_CHAR(cur)) {
	ctxt->errNo = XML_ERR_COMMENT_NOT_FINISHED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	                     "Comment not terminated \n<!--%.50s\n", buf);
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	xmlFree(buf);
    } else {
	if (input != ctxt->input) {
	    ctxt->errNo = XML_ERR_ENTITY_BOUNDARY;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData, 
"Comment doesn't start and stop in the same entity\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	}
        NEXT;
	if ((ctxt->sax != NULL) && (ctxt->sax->comment != NULL) &&
	    (!ctxt->disableSAX))
	    ctxt->sax->comment(ctxt->userData, buf);
	xmlFree(buf);
    }
    ctxt->instate = state;
}

/**
 * xmlParsePITarget:
 * @ctxt:  an XML parser context
 * 
 * parse the name of a PI
 *
 * [17] PITarget ::= Name - (('X' | 'x') ('M' | 'm') ('L' | 'l'))
 *
 * Returns the PITarget name or NULL
 */

xmlChar *
xmlParsePITarget(xmlParserCtxtPtr ctxt) {
    xmlChar *name;

    name = xmlParseName(ctxt);
    if ((name != NULL) &&
        ((name[0] == 'x') || (name[0] == 'X')) &&
        ((name[1] == 'm') || (name[1] == 'M')) &&
        ((name[2] == 'l') || (name[2] == 'L'))) {
	int i;
	if ((name[0] == 'x') && (name[1] == 'm') &&
	    (name[2] == 'l') && (name[3] == 0)) {
	    ctxt->errNo = XML_ERR_RESERVED_XML_NAME;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData,
		 "XML declaration allowed only at the start of the document\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    return(name);
	} else if (name[3] == 0) {
	    ctxt->errNo = XML_ERR_RESERVED_XML_NAME;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData, "Invalid PI name\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    return(name);
	}
	for (i = 0;;i++) {
	    if (xmlW3CPIs[i] == NULL) break;
	    if (xmlStrEqual(name, (const xmlChar *)xmlW3CPIs[i]))
	        return(name);
	}
	if ((ctxt->sax != NULL) && (ctxt->sax->warning != NULL)) {
	    ctxt->errNo = XML_ERR_RESERVED_XML_NAME;
	    ctxt->sax->warning(ctxt->userData,
	         "xmlParsePITarget: invalid name prefix 'xml'\n");
	}
    }
    return(name);
}

#ifdef LIBXML_CATALOG_ENABLED
/**
 * xmlParseCatalogPI:
 * @ctxt:  an XML parser context
 * @catalog:  the PI value string
 * 
 * parse an XML Catalog Processing Instruction.
 *
 * <?oasis-xml-catalog catalog="http://example.com/catalog.xml"?>
 *
 * Occurs only if allowed by the user and if happening in the Misc
 * part of the document before any doctype informations
 * This will add the given catalog to the parsing context in order
 * to be used if there is a resolution need further down in the document
 */

static void
xmlParseCatalogPI(xmlParserCtxtPtr ctxt, const xmlChar *catalog) {
    xmlChar *URL = NULL;
    const xmlChar *tmp, *base;
    xmlChar marker;

    tmp = catalog;
    while (IS_BLANK(*tmp)) tmp++;
    if (xmlStrncmp(tmp, BAD_CAST"catalog", 7))
	goto error;
    tmp += 7;
    while (IS_BLANK(*tmp)) tmp++;
    if (*tmp != '=') {
	return;
    }
    tmp++;
    while (IS_BLANK(*tmp)) tmp++;
    marker = *tmp;
    if ((marker != '\'') && (marker != '"'))
	goto error;
    tmp++;
    base = tmp;
    while ((*tmp != 0) && (*tmp != marker)) tmp++;
    if (*tmp == 0)
	goto error;
    URL = xmlStrndup(base, tmp - base);
    tmp++;
    while (IS_BLANK(*tmp)) tmp++;
    if (*tmp != 0)
	goto error;

    if (URL != NULL) {
	ctxt->catalogs = xmlCatalogAddLocal(ctxt->catalogs, URL);
	xmlFree(URL);
    }
    return;

error:
    ctxt->errNo = XML_WAR_CATALOG_PI;
    if ((ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
	ctxt->sax->warning(ctxt->userData, 
	     "Catalog PI syntax error: %s\n", catalog);
    if (URL != NULL)
	xmlFree(URL);
}
#endif

/**
 * xmlParsePI:
 * @ctxt:  an XML parser context
 * 
 * parse an XML Processing Instruction.
 *
 * [16] PI ::= '<?' PITarget (S (Char* - (Char* '?>' Char*)))? '?>'
 *
 * The processing is transfered to SAX once parsed.
 */

void
xmlParsePI(xmlParserCtxtPtr ctxt) {
    xmlChar *buf = NULL;
    int len = 0;
    int size = XML_PARSER_BUFFER_SIZE;
    int cur, l;
    xmlChar *target;
    xmlParserInputState state;
    int count = 0;

    if ((RAW == '<') && (NXT(1) == '?')) {
	xmlParserInputPtr input = ctxt->input;
	state = ctxt->instate;
        ctxt->instate = XML_PARSER_PI;
	/*
	 * this is a Processing Instruction.
	 */
	SKIP(2);
	SHRINK;

	/*
	 * Parse the target name and check for special support like
	 * namespace.
	 */
        target = xmlParsePITarget(ctxt);
	if (target != NULL) {
	    if ((RAW == '?') && (NXT(1) == '>')) {
		if (input != ctxt->input) {
		    ctxt->errNo = XML_ERR_ENTITY_BOUNDARY;
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData, 
    "PI declaration doesn't start and stop in the same entity\n");
		    ctxt->wellFormed = 0;
		    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		}
		SKIP(2);

		/*
		 * SAX: PI detected.
		 */
		if ((ctxt->sax) && (!ctxt->disableSAX) &&
		    (ctxt->sax->processingInstruction != NULL))
		    ctxt->sax->processingInstruction(ctxt->userData,
		                                     target, NULL);
		ctxt->instate = state;
		xmlFree(target);
		return;
	    }
	    buf = (xmlChar *) xmlMalloc(size * sizeof(xmlChar));
	    if (buf == NULL) {
		xmlGenericError(xmlGenericErrorContext,
			"malloc of %d byte failed\n", size);
		ctxt->instate = state;
		return;
	    }
	    cur = CUR;
	    if (!IS_BLANK(cur)) {
		ctxt->errNo = XML_ERR_SPACE_REQUIRED;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
		      "xmlParsePI: PI %s space expected\n", target);
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    }
            SKIP_BLANKS;
	    cur = CUR_CHAR(l);
	    while (IS_CHAR(cur) && /* checked */
		   ((cur != '?') || (NXT(1) != '>'))) {
		if (len + 5 >= size) {
		    size *= 2;
		    buf = (xmlChar *) xmlRealloc(buf, size * sizeof(xmlChar));
		    if (buf == NULL) {
			xmlGenericError(xmlGenericErrorContext,
				"realloc of %d byte failed\n", size);
			ctxt->instate = state;
			return;
		    }
		}
		count++;
		if (count > 50) {
		    GROW;
		    count = 0;
		}
		COPY_BUF(l,buf,len,cur);
		NEXTL(l);
		cur = CUR_CHAR(l);
		if (cur == 0) {
		    SHRINK;
		    GROW;
		    cur = CUR_CHAR(l);
		}
	    }
	    buf[len] = 0;
	    if (cur != '?') {
		ctxt->errNo = XML_ERR_PI_NOT_FINISHED;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
		      "xmlParsePI: PI %s never end ...\n", target);
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    } else {
		if (input != ctxt->input) {
		    ctxt->errNo = XML_ERR_ENTITY_BOUNDARY;
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData, 
    "PI declaration doesn't start and stop in the same entity\n");
		    ctxt->wellFormed = 0;
		    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		}
		SKIP(2);

#ifdef LIBXML_CATALOG_ENABLED
		if (((state == XML_PARSER_MISC) ||
	             (state == XML_PARSER_START)) &&
		    (xmlStrEqual(target, XML_CATALOG_PI))) {
		    xmlCatalogAllow allow = xmlCatalogGetDefaults();
		    if ((allow == XML_CATA_ALLOW_DOCUMENT) ||
			(allow == XML_CATA_ALLOW_ALL))
			xmlParseCatalogPI(ctxt, buf);
		}
#endif


		/*
		 * SAX: PI detected.
		 */
		if ((ctxt->sax) && (!ctxt->disableSAX) &&
		    (ctxt->sax->processingInstruction != NULL))
		    ctxt->sax->processingInstruction(ctxt->userData,
		                                     target, buf);
	    }
	    xmlFree(buf);
	    xmlFree(target);
	} else {
	    ctxt->errNo = XML_ERR_PI_NOT_STARTED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData,
		       "xmlParsePI : no target name\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	}
	ctxt->instate = state;
    }
}

/**
 * xmlParseNotationDecl:
 * @ctxt:  an XML parser context
 *
 * parse a notation declaration
 *
 * [82] NotationDecl ::= '<!NOTATION' S Name S (ExternalID |  PublicID) S? '>'
 *
 * Hence there is actually 3 choices:
 *     'PUBLIC' S PubidLiteral
 *     'PUBLIC' S PubidLiteral S SystemLiteral
 * and 'SYSTEM' S SystemLiteral
 *
 * See the NOTE on xmlParseExternalID().
 */

void
xmlParseNotationDecl(xmlParserCtxtPtr ctxt) {
    xmlChar *name;
    xmlChar *Pubid;
    xmlChar *Systemid;
    
    if ((RAW == '<') && (NXT(1) == '!') &&
        (NXT(2) == 'N') && (NXT(3) == 'O') &&
        (NXT(4) == 'T') && (NXT(5) == 'A') &&
        (NXT(6) == 'T') && (NXT(7) == 'I') &&
        (NXT(8) == 'O') && (NXT(9) == 'N')) {
	xmlParserInputPtr input = ctxt->input;
	SHRINK;
	SKIP(10);
	if (!IS_BLANK(CUR)) {
	    ctxt->errNo = XML_ERR_SPACE_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData,
		                 "Space required after '<!NOTATION'\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    return;
	}
	SKIP_BLANKS;

        name = xmlParseName(ctxt);
	if (name == NULL) {
	    ctxt->errNo = XML_ERR_NOTATION_NOT_STARTED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData,
		                 "NOTATION: Name expected here\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    return;
	}
	if (!IS_BLANK(CUR)) {
	    ctxt->errNo = XML_ERR_SPACE_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData, 
		     "Space required after the NOTATION name'\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    return;
	}
	SKIP_BLANKS;

	/*
	 * Parse the IDs.
	 */
	Systemid = xmlParseExternalID(ctxt, &Pubid, 0);
	SKIP_BLANKS;

	if (RAW == '>') {
	    if (input != ctxt->input) {
		ctxt->errNo = XML_ERR_ENTITY_BOUNDARY;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData, 
"Notation declaration doesn't start and stop in the same entity\n");
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    }
	    NEXT;
	    if ((ctxt->sax != NULL) && (!ctxt->disableSAX) &&
		(ctxt->sax->notationDecl != NULL))
		ctxt->sax->notationDecl(ctxt->userData, name, Pubid, Systemid);
	} else {
	    ctxt->errNo = XML_ERR_NOTATION_NOT_FINISHED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData,
		       "'>' required to close NOTATION declaration\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	}
	xmlFree(name);
	if (Systemid != NULL) xmlFree(Systemid);
	if (Pubid != NULL) xmlFree(Pubid);
    }
}

/**
 * xmlParseEntityDecl:
 * @ctxt:  an XML parser context
 *
 * parse <!ENTITY declarations
 *
 * [70] EntityDecl ::= GEDecl | PEDecl
 *
 * [71] GEDecl ::= '<!ENTITY' S Name S EntityDef S? '>'
 *
 * [72] PEDecl ::= '<!ENTITY' S '%' S Name S PEDef S? '>'
 *
 * [73] EntityDef ::= EntityValue | (ExternalID NDataDecl?)
 *
 * [74] PEDef ::= EntityValue | ExternalID
 *
 * [76] NDataDecl ::= S 'NDATA' S Name
 *
 * [ VC: Notation Declared ]
 * The Name must match the declared name of a notation.
 */

void
xmlParseEntityDecl(xmlParserCtxtPtr ctxt) {
    xmlChar *name = NULL;
    xmlChar *value = NULL;
    xmlChar *URI = NULL, *literal = NULL;
    xmlChar *ndata = NULL;
    int isParameter = 0;
    xmlChar *orig = NULL;
    int skipped;
    
    GROW;
    if ((RAW == '<') && (NXT(1) == '!') &&
        (NXT(2) == 'E') && (NXT(3) == 'N') &&
        (NXT(4) == 'T') && (NXT(5) == 'I') &&
        (NXT(6) == 'T') && (NXT(7) == 'Y')) {
	xmlParserInputPtr input = ctxt->input;
	SHRINK;
	SKIP(8);
	skipped = SKIP_BLANKS;
	if (skipped == 0) {
	    ctxt->errNo = XML_ERR_SPACE_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData,
		                 "Space required after '<!ENTITY'\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	}

	if (RAW == '%') {
	    NEXT;
	    skipped = SKIP_BLANKS;
	    if (skipped == 0) {
		ctxt->errNo = XML_ERR_SPACE_REQUIRED;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
		                     "Space required after '%'\n");
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    }
	    isParameter = 1;
	}

        name = xmlParseName(ctxt);
	if (name == NULL) {
	    ctxt->errNo = XML_ERR_NAME_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData, "xmlParseEntityDecl: no name\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
            return;
	}
        skipped = SKIP_BLANKS;
	if (skipped == 0) {
	    ctxt->errNo = XML_ERR_SPACE_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData,
		     "Space required after the entity name\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	}

	ctxt->instate = XML_PARSER_ENTITY_DECL;
	/*
	 * handle the various case of definitions...
	 */
	if (isParameter) {
	    if ((RAW == '"') || (RAW == '\'')) {
	        value = xmlParseEntityValue(ctxt, &orig);
		if (value) {
		    if ((ctxt->sax != NULL) &&
			(!ctxt->disableSAX) && (ctxt->sax->entityDecl != NULL))
			ctxt->sax->entityDecl(ctxt->userData, name,
		                    XML_INTERNAL_PARAMETER_ENTITY,
				    NULL, NULL, value);
		}
	    } else {
	        URI = xmlParseExternalID(ctxt, &literal, 1);
		if ((URI == NULL) && (literal == NULL)) {
		    ctxt->errNo = XML_ERR_VALUE_REQUIRED;
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData,
			    "Entity value required\n");
		    ctxt->wellFormed = 0;
		    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		}
		if (URI) {
		    xmlURIPtr uri;

		    uri = xmlParseURI((const char *) URI);
		    if (uri == NULL) {
			ctxt->errNo = XML_ERR_INVALID_URI;
			if ((ctxt->sax != NULL) &&
			    (!ctxt->disableSAX) &&
			    (ctxt->sax->error != NULL))
			    ctxt->sax->error(ctxt->userData,
				        "Invalid URI: %s\n", URI);
			/*
			 * This really ought to be a well formedness error
			 * but the XML Core WG decided otherwise c.f. issue
			 * E26 of the XML erratas.
			 */
		    } else {
			if (uri->fragment != NULL) {
			    ctxt->errNo = XML_ERR_URI_FRAGMENT;
			    if ((ctxt->sax != NULL) &&
				(!ctxt->disableSAX) &&
				(ctxt->sax->error != NULL))
				ctxt->sax->error(ctxt->userData,
					    "Fragment not allowed: %s\n", URI);
			    /*
			     * Okay this is foolish to block those but not
			     * invalid URIs.
			     */
			    ctxt->wellFormed = 0;
			} else {
			    if ((ctxt->sax != NULL) &&
				(!ctxt->disableSAX) &&
				(ctxt->sax->entityDecl != NULL))
				ctxt->sax->entityDecl(ctxt->userData, name,
					    XML_EXTERNAL_PARAMETER_ENTITY,
					    literal, URI, NULL);
			}
			xmlFreeURI(uri);
		    }
		}
	    }
	} else {
	    if ((RAW == '"') || (RAW == '\'')) {
	        value = xmlParseEntityValue(ctxt, &orig);
		if ((ctxt->sax != NULL) &&
		    (!ctxt->disableSAX) && (ctxt->sax->entityDecl != NULL))
		    ctxt->sax->entityDecl(ctxt->userData, name,
				XML_INTERNAL_GENERAL_ENTITY,
				NULL, NULL, value);
		/*
		 * For expat compatibility in SAX mode.
		 */
		if ((ctxt->myDoc == NULL) ||
		    (xmlStrEqual(ctxt->myDoc->version, SAX_COMPAT_MODE))) {
		    if (ctxt->myDoc == NULL) {
			ctxt->myDoc = xmlNewDoc(SAX_COMPAT_MODE);
		    }
		    if (ctxt->myDoc->intSubset == NULL)
			ctxt->myDoc->intSubset = xmlNewDtd(ctxt->myDoc,
					    BAD_CAST "fake", NULL, NULL);

		    entityDecl(ctxt, name, XML_INTERNAL_GENERAL_ENTITY,
			       NULL, NULL, value);
		}
	    } else {
	        URI = xmlParseExternalID(ctxt, &literal, 1);
		if ((URI == NULL) && (literal == NULL)) {
		    ctxt->errNo = XML_ERR_VALUE_REQUIRED;
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData,
			    "Entity value required\n");
		    ctxt->wellFormed = 0;
		    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		}
		if (URI) {
		    xmlURIPtr uri;

		    uri = xmlParseURI((const char *)URI);
		    if (uri == NULL) {
			ctxt->errNo = XML_ERR_INVALID_URI;
			if ((ctxt->sax != NULL) &&
			    (!ctxt->disableSAX) &&
			    (ctxt->sax->error != NULL))
			    ctxt->sax->error(ctxt->userData,
				        "Invalid URI: %s\n", URI);
			/*
			 * This really ought to be a well formedness error
			 * but the XML Core WG decided otherwise c.f. issue
			 * E26 of the XML erratas.
			 */
		    } else {
			if (uri->fragment != NULL) {
			    ctxt->errNo = XML_ERR_URI_FRAGMENT;
			    if ((ctxt->sax != NULL) &&
				(!ctxt->disableSAX) &&
				(ctxt->sax->error != NULL))
				ctxt->sax->error(ctxt->userData,
					    "Fragment not allowed: %s\n", URI);
			    /*
			     * Okay this is foolish to block those but not
			     * invalid URIs.
			     */
			    ctxt->wellFormed = 0;
			}
			xmlFreeURI(uri);
		    }
		}
		if ((RAW != '>') && (!IS_BLANK(CUR))) {
		    ctxt->errNo = XML_ERR_SPACE_REQUIRED;
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData,
			    "Space required before 'NDATA'\n");
		    ctxt->wellFormed = 0;
		    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		}
		SKIP_BLANKS;
		if ((RAW == 'N') && (NXT(1) == 'D') &&
		    (NXT(2) == 'A') && (NXT(3) == 'T') &&
		    (NXT(4) == 'A')) {
		    SKIP(5);
		    if (!IS_BLANK(CUR)) {
			ctxt->errNo = XML_ERR_SPACE_REQUIRED;
			if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			    ctxt->sax->error(ctxt->userData,
			        "Space required after 'NDATA'\n");
			ctxt->wellFormed = 0;
			if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		    }
		    SKIP_BLANKS;
		    ndata = xmlParseName(ctxt);
		    if ((ctxt->sax != NULL) && (!ctxt->disableSAX) &&
		        (ctxt->sax->unparsedEntityDecl != NULL))
			ctxt->sax->unparsedEntityDecl(ctxt->userData, name,
				    literal, URI, ndata);
		} else {
		    if ((ctxt->sax != NULL) &&
		        (!ctxt->disableSAX) && (ctxt->sax->entityDecl != NULL))
			ctxt->sax->entityDecl(ctxt->userData, name,
				    XML_EXTERNAL_GENERAL_PARSED_ENTITY,
				    literal, URI, NULL);
		    /*
		     * For expat compatibility in SAX mode.
		     * assuming the entity repalcement was asked for
		     */
		    if ((ctxt->replaceEntities != 0) &&
			((ctxt->myDoc == NULL) ||
			(xmlStrEqual(ctxt->myDoc->version, SAX_COMPAT_MODE)))) {
			if (ctxt->myDoc == NULL) {
			    ctxt->myDoc = xmlNewDoc(SAX_COMPAT_MODE);
			}

			if (ctxt->myDoc->intSubset == NULL)
			    ctxt->myDoc->intSubset = xmlNewDtd(ctxt->myDoc,
						BAD_CAST "fake", NULL, NULL);
			entityDecl(ctxt, name,
				    XML_EXTERNAL_GENERAL_PARSED_ENTITY,
				    literal, URI, NULL);
		    }
		}
	    }
	}
	SKIP_BLANKS;
	if (RAW != '>') {
	    ctxt->errNo = XML_ERR_ENTITY_NOT_FINISHED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData, 
	            "xmlParseEntityDecl: entity %s not terminated\n", name);
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	} else {
	    if (input != ctxt->input) {
		ctxt->errNo = XML_ERR_ENTITY_BOUNDARY;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData, 
"Entity declaration doesn't start and stop in the same entity\n");
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    }
	    NEXT;
	}
	if (orig != NULL) {
	    /*
	     * Ugly mechanism to save the raw entity value.
	     */
	    xmlEntityPtr cur = NULL;

	    if (isParameter) {
	        if ((ctxt->sax != NULL) &&
		    (ctxt->sax->getParameterEntity != NULL))
		    cur = ctxt->sax->getParameterEntity(ctxt->userData, name);
	    } else {
	        if ((ctxt->sax != NULL) &&
		    (ctxt->sax->getEntity != NULL))
		    cur = ctxt->sax->getEntity(ctxt->userData, name);
		if ((cur == NULL) && (ctxt->userData==ctxt)) {
		    cur = getEntity(ctxt, name);
		}
	    }
            if (cur != NULL) {
	        if (cur->orig != NULL)
		    xmlFree(orig);
		else
		    cur->orig = orig;
	    } else
		xmlFree(orig);
	}
	if (name != NULL) xmlFree(name);
	if (value != NULL) xmlFree(value);
	if (URI != NULL) xmlFree(URI);
	if (literal != NULL) xmlFree(literal);
	if (ndata != NULL) xmlFree(ndata);
    }
}

/**
 * xmlParseDefaultDecl:
 * @ctxt:  an XML parser context
 * @value:  Receive a possible fixed default value for the attribute
 *
 * Parse an attribute default declaration
 *
 * [60] DefaultDecl ::= '#REQUIRED' | '#IMPLIED' | (('#FIXED' S)? AttValue)
 *
 * [ VC: Required Attribute ]
 * if the default declaration is the keyword #REQUIRED, then the
 * attribute must be specified for all elements of the type in the
 * attribute-list declaration.
 *
 * [ VC: Attribute Default Legal ]
 * The declared default value must meet the lexical constraints of
 * the declared attribute type c.f. xmlValidateAttributeDecl()
 *
 * [ VC: Fixed Attribute Default ]
 * if an attribute has a default value declared with the #FIXED
 * keyword, instances of that attribute must match the default value. 
 *
 * [ WFC: No < in Attribute Values ]
 * handled in xmlParseAttValue()
 *
 * returns: XML_ATTRIBUTE_NONE, XML_ATTRIBUTE_REQUIRED, XML_ATTRIBUTE_IMPLIED
 *          or XML_ATTRIBUTE_FIXED. 
 */

int
xmlParseDefaultDecl(xmlParserCtxtPtr ctxt, xmlChar **value) {
    int val;
    xmlChar *ret;

    *value = NULL;
    if ((RAW == '#') && (NXT(1) == 'R') &&
        (NXT(2) == 'E') && (NXT(3) == 'Q') &&
        (NXT(4) == 'U') && (NXT(5) == 'I') &&
        (NXT(6) == 'R') && (NXT(7) == 'E') &&
        (NXT(8) == 'D')) {
	SKIP(9);
	return(XML_ATTRIBUTE_REQUIRED);
    }
    if ((RAW == '#') && (NXT(1) == 'I') &&
        (NXT(2) == 'M') && (NXT(3) == 'P') &&
        (NXT(4) == 'L') && (NXT(5) == 'I') &&
        (NXT(6) == 'E') && (NXT(7) == 'D')) {
	SKIP(8);
	return(XML_ATTRIBUTE_IMPLIED);
    }
    val = XML_ATTRIBUTE_NONE;
    if ((RAW == '#') && (NXT(1) == 'F') &&
        (NXT(2) == 'I') && (NXT(3) == 'X') &&
        (NXT(4) == 'E') && (NXT(5) == 'D')) {
	SKIP(6);
	val = XML_ATTRIBUTE_FIXED;
	if (!IS_BLANK(CUR)) {
	    ctxt->errNo = XML_ERR_SPACE_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData,
		                 "Space required after '#FIXED'\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	}
	SKIP_BLANKS;
    }
    ret = xmlParseAttValue(ctxt);
    ctxt->instate = XML_PARSER_DTD;
    if (ret == NULL) {
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	       "Attribute default value declaration error\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    } else
        *value = ret;
    return(val);
}

/**
 * xmlParseNotationType:
 * @ctxt:  an XML parser context
 *
 * parse an Notation attribute type.
 *
 * Note: the leading 'NOTATION' S part has already being parsed...
 *
 * [58] NotationType ::= 'NOTATION' S '(' S? Name (S? '|' S? Name)* S? ')'
 *
 * [ VC: Notation Attributes ]
 * Values of this type must match one of the notation names included
 * in the declaration; all notation names in the declaration must be declared. 
 *
 * Returns: the notation attribute tree built while parsing
 */

xmlEnumerationPtr
xmlParseNotationType(xmlParserCtxtPtr ctxt) {
    xmlChar *name;
    xmlEnumerationPtr ret = NULL, last = NULL, cur;

    if (RAW != '(') {
	ctxt->errNo = XML_ERR_NOTATION_NOT_STARTED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	                     "'(' required to start 'NOTATION'\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	return(NULL);
    }
    SHRINK;
    do {
        NEXT;
	SKIP_BLANKS;
        name = xmlParseName(ctxt);
	if (name == NULL) {
	    ctxt->errNo = XML_ERR_NAME_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData, 
		                 "Name expected in NOTATION declaration\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    return(ret);
	}
	cur = xmlCreateEnumeration(name);
	xmlFree(name);
	if (cur == NULL) return(ret);
	if (last == NULL) ret = last = cur;
	else {
	    last->next = cur;
	    last = cur;
	}
	SKIP_BLANKS;
    } while (RAW == '|');
    if (RAW != ')') {
	ctxt->errNo = XML_ERR_NOTATION_NOT_FINISHED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	                     "')' required to finish NOTATION declaration\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	if ((last != NULL) && (last != ret))
	    xmlFreeEnumeration(last);
	return(ret);
    }
    NEXT;
    return(ret);
}

/**
 * xmlParseEnumerationType:
 * @ctxt:  an XML parser context
 *
 * parse an Enumeration attribute type.
 *
 * [59] Enumeration ::= '(' S? Nmtoken (S? '|' S? Nmtoken)* S? ')'
 *
 * [ VC: Enumeration ]
 * Values of this type must match one of the Nmtoken tokens in
 * the declaration
 *
 * Returns: the enumeration attribute tree built while parsing
 */

xmlEnumerationPtr
xmlParseEnumerationType(xmlParserCtxtPtr ctxt) {
    xmlChar *name;
    xmlEnumerationPtr ret = NULL, last = NULL, cur;

    if (RAW != '(') {
	ctxt->errNo = XML_ERR_ATTLIST_NOT_STARTED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	                     "'(' required to start ATTLIST enumeration\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	return(NULL);
    }
    SHRINK;
    do {
        NEXT;
	SKIP_BLANKS;
        name = xmlParseNmtoken(ctxt);
	if (name == NULL) {
	    ctxt->errNo = XML_ERR_NMTOKEN_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData, 
		                 "NmToken expected in ATTLIST enumeration\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    return(ret);
	}
	cur = xmlCreateEnumeration(name);
	xmlFree(name);
	if (cur == NULL) return(ret);
	if (last == NULL) ret = last = cur;
	else {
	    last->next = cur;
	    last = cur;
	}
	SKIP_BLANKS;
    } while (RAW == '|');
    if (RAW != ')') {
	ctxt->errNo = XML_ERR_ATTLIST_NOT_FINISHED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	                     "')' required to finish ATTLIST enumeration\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	return(ret);
    }
    NEXT;
    return(ret);
}

/**
 * xmlParseEnumeratedType:
 * @ctxt:  an XML parser context
 * @tree:  the enumeration tree built while parsing
 *
 * parse an Enumerated attribute type.
 *
 * [57] EnumeratedType ::= NotationType | Enumeration
 *
 * [58] NotationType ::= 'NOTATION' S '(' S? Name (S? '|' S? Name)* S? ')'
 *
 *
 * Returns: XML_ATTRIBUTE_ENUMERATION or XML_ATTRIBUTE_NOTATION
 */

int
xmlParseEnumeratedType(xmlParserCtxtPtr ctxt, xmlEnumerationPtr *tree) {
    if ((RAW == 'N') && (NXT(1) == 'O') &&
        (NXT(2) == 'T') && (NXT(3) == 'A') &&
        (NXT(4) == 'T') && (NXT(5) == 'I') &&
	(NXT(6) == 'O') && (NXT(7) == 'N')) {
	SKIP(8);
	if (!IS_BLANK(CUR)) {
	    ctxt->errNo = XML_ERR_SPACE_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData,
		                 "Space required after 'NOTATION'\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    return(0);
	}
        SKIP_BLANKS;
	*tree = xmlParseNotationType(ctxt);
	if (*tree == NULL) return(0);
	return(XML_ATTRIBUTE_NOTATION);
    }
    *tree = xmlParseEnumerationType(ctxt);
    if (*tree == NULL) return(0);
    return(XML_ATTRIBUTE_ENUMERATION);
}

/**
 * xmlParseAttributeType:
 * @ctxt:  an XML parser context
 * @tree:  the enumeration tree built while parsing
 *
 * parse the Attribute list def for an element
 *
 * [54] AttType ::= StringType | TokenizedType | EnumeratedType
 *
 * [55] StringType ::= 'CDATA'
 *
 * [56] TokenizedType ::= 'ID' | 'IDREF' | 'IDREFS' | 'ENTITY' |
 *                        'ENTITIES' | 'NMTOKEN' | 'NMTOKENS'
 *
 * Validity constraints for attribute values syntax are checked in
 * xmlValidateAttributeValue()
 *
 * [ VC: ID ]
 * Values of type ID must match the Name production. A name must not
 * appear more than once in an XML document as a value of this type;
 * i.e., ID values must uniquely identify the elements which bear them.
 *
 * [ VC: One ID per Element Type ]
 * No element type may have more than one ID attribute specified.
 *
 * [ VC: ID Attribute Default ]
 * An ID attribute must have a declared default of #IMPLIED or #REQUIRED.
 *
 * [ VC: IDREF ]
 * Values of type IDREF must match the Name production, and values
 * of type IDREFS must match Names; each IDREF Name must match the value
 * of an ID attribute on some element in the XML document; i.e. IDREF
 * values must match the value of some ID attribute.
 *
 * [ VC: Entity Name ]
 * Values of type ENTITY must match the Name production, values
 * of type ENTITIES must match Names; each Entity Name must match the
 * name of an unparsed entity declared in the DTD.  
 *
 * [ VC: Name Token ]
 * Values of type NMTOKEN must match the Nmtoken production; values
 * of type NMTOKENS must match Nmtokens. 
 *
 * Returns the attribute type
 */
int 
xmlParseAttributeType(xmlParserCtxtPtr ctxt, xmlEnumerationPtr *tree) {
    SHRINK;
    if ((RAW == 'C') && (NXT(1) == 'D') &&
        (NXT(2) == 'A') && (NXT(3) == 'T') &&
        (NXT(4) == 'A')) {
	SKIP(5);
	return(XML_ATTRIBUTE_CDATA);
     } else if ((RAW == 'I') && (NXT(1) == 'D') &&
        (NXT(2) == 'R') && (NXT(3) == 'E') &&
        (NXT(4) == 'F') && (NXT(5) == 'S')) {
	SKIP(6);
	return(XML_ATTRIBUTE_IDREFS);
     } else if ((RAW == 'I') && (NXT(1) == 'D') &&
        (NXT(2) == 'R') && (NXT(3) == 'E') &&
        (NXT(4) == 'F')) {
	SKIP(5);
	return(XML_ATTRIBUTE_IDREF);
     } else if ((RAW == 'I') && (NXT(1) == 'D')) {
        SKIP(2);
	return(XML_ATTRIBUTE_ID);
     } else if ((RAW == 'E') && (NXT(1) == 'N') &&
        (NXT(2) == 'T') && (NXT(3) == 'I') &&
        (NXT(4) == 'T') && (NXT(5) == 'Y')) {
	SKIP(6);
	return(XML_ATTRIBUTE_ENTITY);
     } else if ((RAW == 'E') && (NXT(1) == 'N') &&
        (NXT(2) == 'T') && (NXT(3) == 'I') &&
        (NXT(4) == 'T') && (NXT(5) == 'I') &&
        (NXT(6) == 'E') && (NXT(7) == 'S')) {
	SKIP(8);
	return(XML_ATTRIBUTE_ENTITIES);
     } else if ((RAW == 'N') && (NXT(1) == 'M') &&
        (NXT(2) == 'T') && (NXT(3) == 'O') &&
        (NXT(4) == 'K') && (NXT(5) == 'E') &&
        (NXT(6) == 'N') && (NXT(7) == 'S')) {
	SKIP(8);
	return(XML_ATTRIBUTE_NMTOKENS);
     } else if ((RAW == 'N') && (NXT(1) == 'M') &&
        (NXT(2) == 'T') && (NXT(3) == 'O') &&
        (NXT(4) == 'K') && (NXT(5) == 'E') &&
        (NXT(6) == 'N')) {
	SKIP(7);
	return(XML_ATTRIBUTE_NMTOKEN);
     }
     return(xmlParseEnumeratedType(ctxt, tree));
}

/**
 * xmlParseAttributeListDecl:
 * @ctxt:  an XML parser context
 *
 * : parse the Attribute list def for an element
 *
 * [52] AttlistDecl ::= '<!ATTLIST' S Name AttDef* S? '>'
 *
 * [53] AttDef ::= S Name S AttType S DefaultDecl
 *
 */
void
xmlParseAttributeListDecl(xmlParserCtxtPtr ctxt) {
    xmlChar *elemName;
    xmlChar *attrName;
    xmlEnumerationPtr tree;

    if ((RAW == '<') && (NXT(1) == '!') &&
        (NXT(2) == 'A') && (NXT(3) == 'T') &&
        (NXT(4) == 'T') && (NXT(5) == 'L') &&
        (NXT(6) == 'I') && (NXT(7) == 'S') &&
        (NXT(8) == 'T')) {
	xmlParserInputPtr input = ctxt->input;

	SKIP(9);
	if (!IS_BLANK(CUR)) {
	    ctxt->errNo = XML_ERR_SPACE_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData,
		                 "Space required after '<!ATTLIST'\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	}
        SKIP_BLANKS;
        elemName = xmlParseName(ctxt);
	if (elemName == NULL) {
	    ctxt->errNo = XML_ERR_NAME_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData,
		                 "ATTLIST: no name for Element\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    return;
	}
	SKIP_BLANKS;
	GROW;
	while (RAW != '>') {
	    const xmlChar *check = CUR_PTR;
	    int type;
	    int def;
	    xmlChar *defaultValue = NULL;

	    GROW;
            tree = NULL;
	    attrName = xmlParseName(ctxt);
	    if (attrName == NULL) {
		ctxt->errNo = XML_ERR_NAME_REQUIRED;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
		                     "ATTLIST: no name for Attribute\n");
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		break;
	    }
	    GROW;
	    if (!IS_BLANK(CUR)) {
		ctxt->errNo = XML_ERR_SPACE_REQUIRED;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData, 
		        "Space required after the attribute name\n");
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
                if (attrName != NULL)
		    xmlFree(attrName);
                if (defaultValue != NULL)
		    xmlFree(defaultValue);
		break;
	    }
	    SKIP_BLANKS;

	    type = xmlParseAttributeType(ctxt, &tree);
	    if (type <= 0) {
                if (attrName != NULL)
		    xmlFree(attrName);
                if (defaultValue != NULL)
		    xmlFree(defaultValue);
	        break;
	    }

	    GROW;
	    if (!IS_BLANK(CUR)) {
		ctxt->errNo = XML_ERR_SPACE_REQUIRED;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData, 
		        "Space required after the attribute type\n");
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
                if (attrName != NULL)
		    xmlFree(attrName);
                if (defaultValue != NULL)
		    xmlFree(defaultValue);
	        if (tree != NULL)
		    xmlFreeEnumeration(tree);
		break;
	    }
	    SKIP_BLANKS;

	    def = xmlParseDefaultDecl(ctxt, &defaultValue);
	    if (def <= 0) {
                if (attrName != NULL)
		    xmlFree(attrName);
                if (defaultValue != NULL)
		    xmlFree(defaultValue);
	        if (tree != NULL)
		    xmlFreeEnumeration(tree);
	        break;
	    }

	    GROW;
            if (RAW != '>') {
		if (!IS_BLANK(CUR)) {
		    ctxt->errNo = XML_ERR_SPACE_REQUIRED;
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData, 
			"Space required after the attribute default value\n");
		    ctxt->wellFormed = 0;
		    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		    if (attrName != NULL)
			xmlFree(attrName);
		    if (defaultValue != NULL)
			xmlFree(defaultValue);
		    if (tree != NULL)
			xmlFreeEnumeration(tree);
		    break;
		}
		SKIP_BLANKS;
	    }
	    if (check == CUR_PTR) {
		ctxt->errNo = XML_ERR_INTERNAL_ERROR;
	        if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData, 
		    "xmlParseAttributeListDecl: detected internal error\n");
		if (attrName != NULL)
		    xmlFree(attrName);
		if (defaultValue != NULL)
		    xmlFree(defaultValue);
	        if (tree != NULL)
		    xmlFreeEnumeration(tree);
		break;
	    }
	    if ((ctxt->sax != NULL) && (!ctxt->disableSAX) &&
		(ctxt->sax->attributeDecl != NULL))
		ctxt->sax->attributeDecl(ctxt->userData, elemName, attrName,
	                        type, def, defaultValue, tree);
	    if (attrName != NULL)
		xmlFree(attrName);
	    if (defaultValue != NULL)
	        xmlFree(defaultValue);
	    GROW;
	}
	if (RAW == '>') {
	    if (input != ctxt->input) {
		ctxt->errNo = XML_ERR_ENTITY_BOUNDARY;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData, 
"Attribute list declaration doesn't start and stop in the same entity\n");
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    }
	    NEXT;
	}

	xmlFree(elemName);
    }
}

/**
 * xmlParseElementMixedContentDecl:
 * @ctxt:  an XML parser context
 * @inputchk:  the input used for the current entity, needed for boundary checks
 *
 * parse the declaration for a Mixed Element content
 * The leading '(' and spaces have been skipped in xmlParseElementContentDecl
 * 
 * [51] Mixed ::= '(' S? '#PCDATA' (S? '|' S? Name)* S? ')*' |
 *                '(' S? '#PCDATA' S? ')'
 *
 * [ VC: Proper Group/PE Nesting ] applies to [51] too (see [49])
 *
 * [ VC: No Duplicate Types ]
 * The same name must not appear more than once in a single
 * mixed-content declaration. 
 *
 * returns: the list of the xmlElementContentPtr describing the element choices
 */
xmlElementContentPtr
xmlParseElementMixedContentDecl(xmlParserCtxtPtr ctxt, xmlParserInputPtr inputchk) {
    xmlElementContentPtr ret = NULL, cur = NULL, n;
    xmlChar *elem = NULL;

    GROW;
    if ((RAW == '#') && (NXT(1) == 'P') &&
        (NXT(2) == 'C') && (NXT(3) == 'D') &&
        (NXT(4) == 'A') && (NXT(5) == 'T') &&
        (NXT(6) == 'A')) {
	SKIP(7);
	SKIP_BLANKS;
	SHRINK;
	if (RAW == ')') {
	    if ((ctxt->validate) && (ctxt->input != inputchk)) {
		ctxt->errNo = XML_ERR_ENTITY_BOUNDARY;
		if (ctxt->vctxt.error != NULL)
		    ctxt->vctxt.error(ctxt->vctxt.userData, 
"Element content declaration doesn't start and stop in the same entity\n");
		ctxt->valid = 0;
	    }
	    NEXT;
	    ret = xmlNewElementContent(NULL, XML_ELEMENT_CONTENT_PCDATA);
	    if (RAW == '*') {
		ret->ocur = XML_ELEMENT_CONTENT_MULT;
		NEXT;
	    }
	    return(ret);
	}
	if ((RAW == '(') || (RAW == '|')) {
	    ret = cur = xmlNewElementContent(NULL, XML_ELEMENT_CONTENT_PCDATA);
	    if (ret == NULL) return(NULL);
	}
	while (RAW == '|') {
	    NEXT;
	    if (elem == NULL) {
	        ret = xmlNewElementContent(NULL, XML_ELEMENT_CONTENT_OR);
		if (ret == NULL) return(NULL);
		ret->c1 = cur;
		if (cur != NULL)
		    cur->parent = ret;
		cur = ret;
	    } else {
	        n = xmlNewElementContent(NULL, XML_ELEMENT_CONTENT_OR);
		if (n == NULL) return(NULL);
		n->c1 = xmlNewElementContent(elem, XML_ELEMENT_CONTENT_ELEMENT);
		if (n->c1 != NULL)
		    n->c1->parent = n;
	        cur->c2 = n;
		if (n != NULL)
		    n->parent = cur;
		cur = n;
		xmlFree(elem);
	    }
	    SKIP_BLANKS;
	    elem = xmlParseName(ctxt);
	    if (elem == NULL) {
		ctxt->errNo = XML_ERR_NAME_REQUIRED;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData, 
			"xmlParseElementMixedContentDecl : Name expected\n");
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		xmlFreeElementContent(cur);
		return(NULL);
	    }
	    SKIP_BLANKS;
	    GROW;
	}
	if ((RAW == ')') && (NXT(1) == '*')) {
	    if (elem != NULL) {
		cur->c2 = xmlNewElementContent(elem,
		                               XML_ELEMENT_CONTENT_ELEMENT);
		if (cur->c2 != NULL)
		    cur->c2->parent = cur;
	        xmlFree(elem);
            }
	    ret->ocur = XML_ELEMENT_CONTENT_MULT;
	    if ((ctxt->validate) && (ctxt->input != inputchk)) {
		ctxt->errNo = XML_ERR_ENTITY_BOUNDARY;
		if (ctxt->vctxt.error != NULL)
		    ctxt->vctxt.error(ctxt->vctxt.userData, 
"Element content declaration doesn't start and stop in the same entity\n");
		ctxt->valid = 0;
	    }
	    SKIP(2);
	} else {
	    if (elem != NULL) xmlFree(elem);
	    xmlFreeElementContent(ret);
	    ctxt->errNo = XML_ERR_MIXED_NOT_STARTED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData, 
		    "xmlParseElementMixedContentDecl : '|' or ')*' expected\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    return(NULL);
	}

    } else {
	ctxt->errNo = XML_ERR_PCDATA_REQUIRED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, 
		"xmlParseElementMixedContentDecl : '#PCDATA' expected\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    }
    return(ret);
}

/**
 * xmlParseElementChildrenContentDecl:
 * @ctxt:  an XML parser context
 * @inputchk:  the input used for the current entity, needed for boundary checks
 *
 * parse the declaration for a Mixed Element content
 * The leading '(' and spaces have been skipped in xmlParseElementContentDecl
 * 
 *
 * [47] children ::= (choice | seq) ('?' | '*' | '+')?
 *
 * [48] cp ::= (Name | choice | seq) ('?' | '*' | '+')?
 *
 * [49] choice ::= '(' S? cp ( S? '|' S? cp )* S? ')'
 *
 * [50] seq ::= '(' S? cp ( S? ',' S? cp )* S? ')'
 *
 * [ VC: Proper Group/PE Nesting ] applies to [49] and [50]
 * TODO Parameter-entity replacement text must be properly nested
 *	with parenthesized groups. That is to say, if either of the
 *	opening or closing parentheses in a choice, seq, or Mixed
 *	construct is contained in the replacement text for a parameter
 *	entity, both must be contained in the same replacement text. For
 *	interoperability, if a parameter-entity reference appears in a
 *	choice, seq, or Mixed construct, its replacement text should not
 *	be empty, and neither the first nor last non-blank character of
 *	the replacement text should be a connector (| or ,).
 *
 * Returns the tree of xmlElementContentPtr describing the element 
 *          hierarchy.
 */
xmlElementContentPtr
xmlParseElementChildrenContentDecl
(xmlParserCtxtPtr ctxt, xmlParserInputPtr inputchk) {
    xmlElementContentPtr ret = NULL, cur = NULL, last = NULL, op = NULL;
    xmlChar *elem;
    xmlChar type = 0;

    SKIP_BLANKS;
    GROW;
    if (RAW == '(') {
	xmlParserInputPtr input = ctxt->input;

        /* Recurse on first child */
	NEXT;
	SKIP_BLANKS;
        cur = ret = xmlParseElementChildrenContentDecl(ctxt, input);
	SKIP_BLANKS;
	GROW;
    } else {
	elem = xmlParseName(ctxt);
	if (elem == NULL) {
	    ctxt->errNo = XML_ERR_ELEMCONTENT_NOT_STARTED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData, 
		"xmlParseElementChildrenContentDecl : Name or '(' expected\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    return(NULL);
	}
        cur = ret = xmlNewElementContent(elem, XML_ELEMENT_CONTENT_ELEMENT);
	GROW;
	if (RAW == '?') {
	    cur->ocur = XML_ELEMENT_CONTENT_OPT;
	    NEXT;
	} else if (RAW == '*') {
	    cur->ocur = XML_ELEMENT_CONTENT_MULT;
	    NEXT;
	} else if (RAW == '+') {
	    cur->ocur = XML_ELEMENT_CONTENT_PLUS;
	    NEXT;
	} else {
	    cur->ocur = XML_ELEMENT_CONTENT_ONCE;
	}
	xmlFree(elem);
	GROW;
    }
    SKIP_BLANKS;
    SHRINK;
    while (RAW != ')') {
        /*
	 * Each loop we parse one separator and one element.
	 */
        if (RAW == ',') {
	    if (type == 0) type = CUR;

	    /*
	     * Detect "Name | Name , Name" error
	     */
	    else if (type != CUR) {
		ctxt->errNo = XML_ERR_SEPARATOR_REQUIRED;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData, 
		    "xmlParseElementChildrenContentDecl : '%c' expected\n",
		    type);
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		if ((last != NULL) && (last != ret))
		    xmlFreeElementContent(last);
		if (ret != NULL)
		    xmlFreeElementContent(ret);
		return(NULL);
	    }
	    NEXT;

	    op = xmlNewElementContent(NULL, XML_ELEMENT_CONTENT_SEQ);
	    if (op == NULL) {
		if ((last != NULL) && (last != ret))
		    xmlFreeElementContent(last);
	        xmlFreeElementContent(ret);
		return(NULL);
	    }
	    if (last == NULL) {
		op->c1 = ret;
		if (ret != NULL)
		    ret->parent = op;
		ret = cur = op;
	    } else {
	        cur->c2 = op;
		if (op != NULL)
		    op->parent = cur;
		op->c1 = last;
		if (last != NULL)
		    last->parent = op;
		cur =op;
		last = NULL;
	    }
	} else if (RAW == '|') {
	    if (type == 0) type = CUR;

	    /*
	     * Detect "Name , Name | Name" error
	     */
	    else if (type != CUR) {
		ctxt->errNo = XML_ERR_SEPARATOR_REQUIRED;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData, 
		    "xmlParseElementChildrenContentDecl : '%c' expected\n",
		    type);
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		if ((last != NULL) && (last != ret))
		    xmlFreeElementContent(last);
		if (ret != NULL)
		    xmlFreeElementContent(ret);
		return(NULL);
	    }
	    NEXT;

	    op = xmlNewElementContent(NULL, XML_ELEMENT_CONTENT_OR);
	    if (op == NULL) {
		if ((last != NULL) && (last != ret))
		    xmlFreeElementContent(last);
		if (ret != NULL)
		    xmlFreeElementContent(ret);
		return(NULL);
	    }
	    if (last == NULL) {
		op->c1 = ret;
		if (ret != NULL)
		    ret->parent = op;
		ret = cur = op;
	    } else {
	        cur->c2 = op;
		if (op != NULL)
		    op->parent = cur;
		op->c1 = last;
		if (last != NULL)
		    last->parent = op;
		cur =op;
		last = NULL;
	    }
	} else {
	    ctxt->errNo = XML_ERR_ELEMCONTENT_NOT_FINISHED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData, 
	    "xmlParseElementChildrenContentDecl : ',' '|' or ')' expected\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    if (ret != NULL)
		xmlFreeElementContent(ret);
	    return(NULL);
	}
	GROW;
	SKIP_BLANKS;
	GROW;
	if (RAW == '(') {
	    xmlParserInputPtr input = ctxt->input;
	    /* Recurse on second child */
	    NEXT;
	    SKIP_BLANKS;
	    last = xmlParseElementChildrenContentDecl(ctxt, input);
	    SKIP_BLANKS;
	} else {
	    elem = xmlParseName(ctxt);
	    if (elem == NULL) {
		ctxt->errNo = XML_ERR_ELEMCONTENT_NOT_STARTED;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData, 
		"xmlParseElementChildrenContentDecl : Name or '(' expected\n");
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		if (ret != NULL)
		    xmlFreeElementContent(ret);
		return(NULL);
	    }
	    last = xmlNewElementContent(elem, XML_ELEMENT_CONTENT_ELEMENT);
	    xmlFree(elem);
	    if (RAW == '?') {
		last->ocur = XML_ELEMENT_CONTENT_OPT;
		NEXT;
	    } else if (RAW == '*') {
		last->ocur = XML_ELEMENT_CONTENT_MULT;
		NEXT;
	    } else if (RAW == '+') {
		last->ocur = XML_ELEMENT_CONTENT_PLUS;
		NEXT;
	    } else {
		last->ocur = XML_ELEMENT_CONTENT_ONCE;
	    }
	}
	SKIP_BLANKS;
	GROW;
    }
    if ((cur != NULL) && (last != NULL)) {
        cur->c2 = last;
	if (last != NULL)
	    last->parent = cur;
    }
    if ((ctxt->validate) && (ctxt->input != inputchk)) {
	ctxt->errNo = XML_ERR_ENTITY_BOUNDARY;
	if (ctxt->vctxt.error != NULL)
	    ctxt->vctxt.error(ctxt->vctxt.userData, 
"Element content declaration doesn't start and stop in the same entity\n");
	ctxt->valid = 0;
    }
    NEXT;
    if (RAW == '?') {
	if (ret != NULL)
	    ret->ocur = XML_ELEMENT_CONTENT_OPT;
	NEXT;
    } else if (RAW == '*') {
	if (ret != NULL) {
	    ret->ocur = XML_ELEMENT_CONTENT_MULT;
	    cur = ret;
	    /*
	     * Some normalization:
	     * (a | b* | c?)* == (a | b | c)*
	     */
	    while (cur->type == XML_ELEMENT_CONTENT_OR) {
		if ((cur->c1 != NULL) &&
	            ((cur->c1->ocur == XML_ELEMENT_CONTENT_OPT) ||
		     (cur->c1->ocur == XML_ELEMENT_CONTENT_MULT)))
		    cur->c1->ocur = XML_ELEMENT_CONTENT_ONCE;
		if ((cur->c2 != NULL) &&
	            ((cur->c2->ocur == XML_ELEMENT_CONTENT_OPT) ||
		     (cur->c2->ocur == XML_ELEMENT_CONTENT_MULT)))
		    cur->c2->ocur = XML_ELEMENT_CONTENT_ONCE;
		cur = cur->c2;
	    }
	}
	NEXT;
    } else if (RAW == '+') {
	if (ret != NULL) {
	    int found = 0;

	    ret->ocur = XML_ELEMENT_CONTENT_PLUS;
	    /*
	     * Some normalization:
	     * (a | b*)+ == (a | b)*
	     * (a | b?)+ == (a | b)*
	     */
	    while (cur->type == XML_ELEMENT_CONTENT_OR) {
		if ((cur->c1 != NULL) &&
	            ((cur->c1->ocur == XML_ELEMENT_CONTENT_OPT) ||
		     (cur->c1->ocur == XML_ELEMENT_CONTENT_MULT))) {
		    cur->c1->ocur = XML_ELEMENT_CONTENT_ONCE;
		    found = 1;
		}
		if ((cur->c2 != NULL) &&
	            ((cur->c2->ocur == XML_ELEMENT_CONTENT_OPT) ||
		     (cur->c2->ocur == XML_ELEMENT_CONTENT_MULT))) {
		    cur->c2->ocur = XML_ELEMENT_CONTENT_ONCE;
		    found = 1;
		}
		cur = cur->c2;
	    }
	    if (found)
		ret->ocur = XML_ELEMENT_CONTENT_MULT;
	}
	NEXT;
    }
    return(ret);
}

/**
 * xmlParseElementContentDecl:
 * @ctxt:  an XML parser context
 * @name:  the name of the element being defined.
 * @result:  the Element Content pointer will be stored here if any
 *
 * parse the declaration for an Element content either Mixed or Children,
 * the cases EMPTY and ANY are handled directly in xmlParseElementDecl
 * 
 * [46] contentspec ::= 'EMPTY' | 'ANY' | Mixed | children
 *
 * returns: the type of element content XML_ELEMENT_TYPE_xxx
 */

int
xmlParseElementContentDecl(xmlParserCtxtPtr ctxt, xmlChar *name,
                           xmlElementContentPtr *result) {

    xmlElementContentPtr tree = NULL;
    xmlParserInputPtr input = ctxt->input;
    int res;

    *result = NULL;

    if (RAW != '(') {
	ctxt->errNo = XML_ERR_ELEMCONTENT_NOT_STARTED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, 
		"xmlParseElementContentDecl : %s '(' expected\n", name);
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	return(-1);
    }
    NEXT;
    GROW;
    SKIP_BLANKS;
    if ((RAW == '#') && (NXT(1) == 'P') &&
        (NXT(2) == 'C') && (NXT(3) == 'D') &&
        (NXT(4) == 'A') && (NXT(5) == 'T') &&
        (NXT(6) == 'A')) {
        tree = xmlParseElementMixedContentDecl(ctxt, input);
	res = XML_ELEMENT_TYPE_MIXED;
    } else {
        tree = xmlParseElementChildrenContentDecl(ctxt, input);
	res = XML_ELEMENT_TYPE_ELEMENT;
    }
    SKIP_BLANKS;
    *result = tree;
    return(res);
}

/**
 * xmlParseElementDecl:
 * @ctxt:  an XML parser context
 *
 * parse an Element declaration.
 *
 * [45] elementdecl ::= '<!ELEMENT' S Name S contentspec S? '>'
 *
 * [ VC: Unique Element Type Declaration ]
 * No element type may be declared more than once
 *
 * Returns the type of the element, or -1 in case of error
 */
int
xmlParseElementDecl(xmlParserCtxtPtr ctxt) {
    xmlChar *name;
    int ret = -1;
    xmlElementContentPtr content  = NULL;

    GROW;
    if ((RAW == '<') && (NXT(1) == '!') &&
        (NXT(2) == 'E') && (NXT(3) == 'L') &&
        (NXT(4) == 'E') && (NXT(5) == 'M') &&
        (NXT(6) == 'E') && (NXT(7) == 'N') &&
        (NXT(8) == 'T')) {
	xmlParserInputPtr input = ctxt->input;

	SKIP(9);
	if (!IS_BLANK(CUR)) {
	    ctxt->errNo = XML_ERR_SPACE_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData, 
		    "Space required after 'ELEMENT'\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	}
        SKIP_BLANKS;
        name = xmlParseName(ctxt);
	if (name == NULL) {
	    ctxt->errNo = XML_ERR_NAME_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData,
		   "xmlParseElementDecl: no name for Element\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    return(-1);
	}
	while ((RAW == 0) && (ctxt->inputNr > 1))
	    xmlPopInput(ctxt);
	if (!IS_BLANK(CUR)) {
	    ctxt->errNo = XML_ERR_SPACE_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData, 
		    "Space required after the element name\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	}
        SKIP_BLANKS;
	if ((RAW == 'E') && (NXT(1) == 'M') &&
	    (NXT(2) == 'P') && (NXT(3) == 'T') &&
	    (NXT(4) == 'Y')) {
	    SKIP(5);
	    /*
	     * Element must always be empty.
	     */
	    ret = XML_ELEMENT_TYPE_EMPTY;
	} else if ((RAW == 'A') && (NXT(1) == 'N') &&
	           (NXT(2) == 'Y')) {
	    SKIP(3);
	    /*
	     * Element is a generic container.
	     */
	    ret = XML_ELEMENT_TYPE_ANY;
	} else if (RAW == '(') {
	    ret = xmlParseElementContentDecl(ctxt, name, &content);
	} else {
	    /*
	     * [ WFC: PEs in Internal Subset ] error handling.
	     */
	    if ((RAW == '%') && (ctxt->external == 0) &&
	        (ctxt->inputNr == 1)) {
		ctxt->errNo = XML_ERR_PEREF_IN_INT_SUBSET;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData, 
	  "PEReference: forbidden within markup decl in internal subset\n");
	    } else {
		ctxt->errNo = XML_ERR_ELEMCONTENT_NOT_STARTED;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData, 
		      "xmlParseElementDecl: 'EMPTY', 'ANY' or '(' expected\n");
            }
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    if (name != NULL) xmlFree(name);
	    return(-1);
	}

	SKIP_BLANKS;
	/*
	 * Pop-up of finished entities.
	 */
	while ((RAW == 0) && (ctxt->inputNr > 1))
	    xmlPopInput(ctxt);
	SKIP_BLANKS;

	if (RAW != '>') {
	    ctxt->errNo = XML_ERR_GT_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData, 
	          "xmlParseElementDecl: expected '>' at the end\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	} else {
	    if (input != ctxt->input) {
		ctxt->errNo = XML_ERR_ENTITY_BOUNDARY;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData, 
"Element declaration doesn't start and stop in the same entity\n");
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    }
		
	    NEXT;
	    if ((ctxt->sax != NULL) && (!ctxt->disableSAX) &&
		(ctxt->sax->elementDecl != NULL))
	        ctxt->sax->elementDecl(ctxt->userData, name, ret,
		                       content);
	}
	if (content != NULL) {
	    xmlFreeElementContent(content);
	}
	if (name != NULL) {
	    xmlFree(name);
	}
    }
    return(ret);
}

/**
 * xmlParseConditionalSections
 * @ctxt:  an XML parser context
 *
 * [61] conditionalSect ::= includeSect | ignoreSect 
 * [62] includeSect ::= '<![' S? 'INCLUDE' S? '[' extSubsetDecl ']]>' 
 * [63] ignoreSect ::= '<![' S? 'IGNORE' S? '[' ignoreSectContents* ']]>'
 * [64] ignoreSectContents ::= Ignore ('<![' ignoreSectContents ']]>' Ignore)*
 * [65] Ignore ::= Char* - (Char* ('<![' | ']]>') Char*)
 */

static void
xmlParseConditionalSections(xmlParserCtxtPtr ctxt) {
    SKIP(3);
    SKIP_BLANKS;
    if ((RAW == 'I') && (NXT(1) == 'N') && (NXT(2) == 'C') &&
        (NXT(3) == 'L') && (NXT(4) == 'U') && (NXT(5) == 'D') &&
        (NXT(6) == 'E')) {
	SKIP(7);
	SKIP_BLANKS;
	if (RAW != '[') {
	    ctxt->errNo = XML_ERR_CONDSEC_INVALID;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData,
	    "XML conditional section '[' expected\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	} else {
	    NEXT;
	}
	if (xmlParserDebugEntities) {
	    if ((ctxt->input != NULL) && (ctxt->input->filename))
		xmlGenericError(xmlGenericErrorContext,
			"%s(%d): ", ctxt->input->filename,
			ctxt->input->line);
	    xmlGenericError(xmlGenericErrorContext,
		    "Entering INCLUDE Conditional Section\n");
	}

	while ((RAW != 0) && ((RAW != ']') || (NXT(1) != ']') ||
	       (NXT(2) != '>'))) {
	    const xmlChar *check = CUR_PTR;
	    int cons = ctxt->input->consumed;

	    if ((RAW == '<') && (NXT(1) == '!') && (NXT(2) == '[')) {
		xmlParseConditionalSections(ctxt);
	    } else if (IS_BLANK(CUR)) {
		NEXT;
	    } else if (RAW == '%') {
		xmlParsePEReference(ctxt);
	    } else
		xmlParseMarkupDecl(ctxt);

	    /*
	     * Pop-up of finished entities.
	     */
	    while ((RAW == 0) && (ctxt->inputNr > 1))
		xmlPopInput(ctxt);

	    if ((CUR_PTR == check) && (cons == ctxt->input->consumed)) {
		ctxt->errNo = XML_ERR_EXT_SUBSET_NOT_FINISHED;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
			"Content error in the external subset\n");
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		break;
	    }
	}
	if (xmlParserDebugEntities) {
	    if ((ctxt->input != NULL) && (ctxt->input->filename))
		xmlGenericError(xmlGenericErrorContext,
			"%s(%d): ", ctxt->input->filename,
			ctxt->input->line);
	    xmlGenericError(xmlGenericErrorContext,
		    "Leaving INCLUDE Conditional Section\n");
	}

    } else if ((RAW == 'I') && (NXT(1) == 'G') && (NXT(2) == 'N') &&
            (NXT(3) == 'O') && (NXT(4) == 'R') && (NXT(5) == 'E')) {
	int state;
	int instate;
	int depth = 0;

	SKIP(6);
	SKIP_BLANKS;
	if (RAW != '[') {
	    ctxt->errNo = XML_ERR_CONDSEC_INVALID;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData,
	    "XML conditional section '[' expected\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	} else {
	    NEXT;
	}
	if (xmlParserDebugEntities) {
	    if ((ctxt->input != NULL) && (ctxt->input->filename))
		xmlGenericError(xmlGenericErrorContext,
			"%s(%d): ", ctxt->input->filename,
			ctxt->input->line);
	    xmlGenericError(xmlGenericErrorContext,
		    "Entering IGNORE Conditional Section\n");
	}

	/*
	 * Parse up to the end of the conditional section
	 * But disable SAX event generating DTD building in the meantime
	 */
	state = ctxt->disableSAX;
	instate = ctxt->instate;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	ctxt->instate = XML_PARSER_IGNORE;

	while ((depth >= 0) && (RAW != 0)) {
	  if ((RAW == '<') && (NXT(1) == '!') && (NXT(2) == '[')) {
	    depth++;
	    SKIP(3);
	    continue;
	  }
	  if ((RAW == ']') && (NXT(1) == ']') && (NXT(2) == '>')) {
	    if (--depth >= 0) SKIP(3);
	    continue;
	  }
	  NEXT;
	  continue;
	}

	ctxt->disableSAX = state;
	ctxt->instate = instate;

	if (xmlParserDebugEntities) {
	    if ((ctxt->input != NULL) && (ctxt->input->filename))
		xmlGenericError(xmlGenericErrorContext,
			"%s(%d): ", ctxt->input->filename,
			ctxt->input->line);
	    xmlGenericError(xmlGenericErrorContext,
		    "Leaving IGNORE Conditional Section\n");
	}

    } else {
	ctxt->errNo = XML_ERR_CONDSEC_INVALID;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	"XML conditional section INCLUDE or IGNORE keyword expected\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    }

    if (RAW == 0)
        SHRINK;

    if (RAW == 0) {
	ctxt->errNo = XML_ERR_CONDSEC_NOT_FINISHED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	        "XML conditional section not closed\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    } else {
        SKIP(3);
    }
}

/**
 * xmlParseMarkupDecl:
 * @ctxt:  an XML parser context
 * 
 * parse Markup declarations
 *
 * [29] markupdecl ::= elementdecl | AttlistDecl | EntityDecl |
 *                     NotationDecl | PI | Comment
 *
 * [ VC: Proper Declaration/PE Nesting ]
 * Parameter-entity replacement text must be properly nested with
 * markup declarations. That is to say, if either the first character
 * or the last character of a markup declaration (markupdecl above) is
 * contained in the replacement text for a parameter-entity reference,
 * both must be contained in the same replacement text.
 *
 * [ WFC: PEs in Internal Subset ]
 * In the internal DTD subset, parameter-entity references can occur
 * only where markup declarations can occur, not within markup declarations.
 * (This does not apply to references that occur in external parameter
 * entities or to the external subset.) 
 */
void
xmlParseMarkupDecl(xmlParserCtxtPtr ctxt) {
    GROW;
    xmlParseElementDecl(ctxt);
    xmlParseAttributeListDecl(ctxt);
    xmlParseEntityDecl(ctxt);
    xmlParseNotationDecl(ctxt);
    xmlParsePI(ctxt);
    xmlParseComment(ctxt);
    /*
     * This is only for internal subset. On external entities,
     * the replacement is done before parsing stage
     */
    if ((ctxt->external == 0) && (ctxt->inputNr == 1))
	xmlParsePEReference(ctxt);

    /*
     * Conditional sections are allowed from entities included
     * by PE References in the internal subset.
     */
    if ((ctxt->external == 0) && (ctxt->inputNr > 1)) {
        if ((RAW == '<') && (NXT(1) == '!') && (NXT(2) == '[')) {
	    xmlParseConditionalSections(ctxt);
	}
    }

    ctxt->instate = XML_PARSER_DTD;
}

/**
 * xmlParseTextDecl:
 * @ctxt:  an XML parser context
 * 
 * parse an XML declaration header for external entities
 *
 * [77] TextDecl ::= '<?xml' VersionInfo? EncodingDecl S? '?>'
 *
 * Question: Seems that EncodingDecl is mandatory ? Is that a typo ?
 */

void
xmlParseTextDecl(xmlParserCtxtPtr ctxt) {
    xmlChar *version;

    /*
     * We know that '<?xml' is here.
     */
    if ((RAW == '<') && (NXT(1) == '?') &&
	(NXT(2) == 'x') && (NXT(3) == 'm') &&
	(NXT(4) == 'l') && (IS_BLANK(NXT(5)))) {
	SKIP(5);
    } else {
	ctxt->errNo = XML_ERR_XMLDECL_NOT_STARTED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	                     "Text declaration '<?xml' required\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;

	return;
    }

    if (!IS_BLANK(CUR)) {
	ctxt->errNo = XML_ERR_SPACE_REQUIRED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	                     "Space needed after '<?xml'\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    }
    SKIP_BLANKS;

    /*
     * We may have the VersionInfo here.
     */
    version = xmlParseVersionInfo(ctxt);
    if (version == NULL)
	version = xmlCharStrdup(XML_DEFAULT_VERSION);
    else {
	if (!IS_BLANK(CUR)) {
	    ctxt->errNo = XML_ERR_SPACE_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData, "Space needed here\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	}
    }
    ctxt->input->version = version;

    /*
     * We must have the encoding declaration
     */
    xmlParseEncodingDecl(ctxt);
    if (ctxt->errNo == XML_ERR_UNSUPPORTED_ENCODING) {
	/*
	 * The XML REC instructs us to stop parsing right here
	 */
        return;
    }

    SKIP_BLANKS;
    if ((RAW == '?') && (NXT(1) == '>')) {
        SKIP(2);
    } else if (RAW == '>') {
        /* Deprecated old WD ... */
	ctxt->errNo = XML_ERR_XMLDECL_NOT_FINISHED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	                     "XML declaration must end-up with '?>'\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	NEXT;
    } else {
	ctxt->errNo = XML_ERR_XMLDECL_NOT_FINISHED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	                     "parsing XML declaration: '?>' expected\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	MOVETO_ENDTAG(CUR_PTR);
	NEXT;
    }
}

/**
 * xmlParseExternalSubset:
 * @ctxt:  an XML parser context
 * @ExternalID: the external identifier
 * @SystemID: the system identifier (or URL)
 * 
 * parse Markup declarations from an external subset
 *
 * [30] extSubset ::= textDecl? extSubsetDecl
 *
 * [31] extSubsetDecl ::= (markupdecl | conditionalSect | PEReference | S) *
 */
void
xmlParseExternalSubset(xmlParserCtxtPtr ctxt, const xmlChar *ExternalID,
                       const xmlChar *SystemID) {
    GROW;
    if ((RAW == '<') && (NXT(1) == '?') &&
        (NXT(2) == 'x') && (NXT(3) == 'm') &&
	(NXT(4) == 'l')) {
	xmlParseTextDecl(ctxt);
	if (ctxt->errNo == XML_ERR_UNSUPPORTED_ENCODING) {
	    /*
	     * The XML REC instructs us to stop parsing right here
	     */
	    ctxt->instate = XML_PARSER_EOF;
	    return;
	}
    }
    if (ctxt->myDoc == NULL) {
        ctxt->myDoc = xmlNewDoc(BAD_CAST "1.0");
    }
    if ((ctxt->myDoc != NULL) && (ctxt->myDoc->intSubset == NULL))
        xmlCreateIntSubset(ctxt->myDoc, NULL, ExternalID, SystemID);

    ctxt->instate = XML_PARSER_DTD;
    ctxt->external = 1;
    while (((RAW == '<') && (NXT(1) == '?')) ||
           ((RAW == '<') && (NXT(1) == '!')) ||
	   (RAW == '%') || IS_BLANK(CUR)) {
	const xmlChar *check = CUR_PTR;
	int cons = ctxt->input->consumed;

	GROW;
        if ((RAW == '<') && (NXT(1) == '!') && (NXT(2) == '[')) {
	    xmlParseConditionalSections(ctxt);
	} else if (IS_BLANK(CUR)) {
	    NEXT;
	} else if (RAW == '%') {
            xmlParsePEReference(ctxt);
	} else
	    xmlParseMarkupDecl(ctxt);

	/*
	 * Pop-up of finished entities.
	 */
	while ((RAW == 0) && (ctxt->inputNr > 1))
	    xmlPopInput(ctxt);

	if ((CUR_PTR == check) && (cons == ctxt->input->consumed)) {
	    ctxt->errNo = XML_ERR_EXT_SUBSET_NOT_FINISHED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData,
		    "Content error in the external subset\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    break;
	}
    }
    
    if (RAW != 0) {
	ctxt->errNo = XML_ERR_EXT_SUBSET_NOT_FINISHED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	        "Extra content at the end of the document\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    }

}

/**
 * xmlParseReference:
 * @ctxt:  an XML parser context
 * 
 * parse and handle entity references in content, depending on the SAX
 * interface, this may end-up in a call to character() if this is a
 * CharRef, a predefined entity, if there is no reference() callback.
 * or if the parser was asked to switch to that mode.
 *
 * [67] Reference ::= EntityRef | CharRef
 */
void
xmlParseReference(xmlParserCtxtPtr ctxt) {
    xmlEntityPtr ent;
    xmlChar *val;
    if (RAW != '&') return;

    if (NXT(1) == '#') {
	int i = 0;
	xmlChar out[10];
	int hex = NXT(2);
	int value = xmlParseCharRef(ctxt);
	
	if (ctxt->charset != XML_CHAR_ENCODING_UTF8) {
	    /*
	     * So we are using non-UTF-8 buffers
	     * Check that the char fit on 8bits, if not
	     * generate a CharRef.
	     */
	    if (value <= 0xFF) {
		out[0] = value;
		out[1] = 0;
		if ((ctxt->sax != NULL) && (ctxt->sax->characters != NULL) &&
		    (!ctxt->disableSAX))
		    ctxt->sax->characters(ctxt->userData, out, 1);
	    } else {
		if ((hex == 'x') || (hex == 'X'))
		    snprintf((char *)out, sizeof(out), "#x%X", value);
		else
		    snprintf((char *)out, sizeof(out), "#%d", value);
		if ((ctxt->sax != NULL) && (ctxt->sax->reference != NULL) &&
		    (!ctxt->disableSAX))
		    ctxt->sax->reference(ctxt->userData, out);
	    }
	} else {
	    /*
	     * Just encode the value in UTF-8
	     */
	    COPY_BUF(0 ,out, i, value);
	    out[i] = 0;
	    if ((ctxt->sax != NULL) && (ctxt->sax->characters != NULL) &&
		(!ctxt->disableSAX))
		ctxt->sax->characters(ctxt->userData, out, i);
	}
    } else {
	ent = xmlParseEntityRef(ctxt);
	if (ent == NULL) return;
	if (!ctxt->wellFormed)
	    return;
	if ((ent->name != NULL) && 
	    (ent->etype != XML_INTERNAL_PREDEFINED_ENTITY)) {
	    xmlNodePtr list = NULL;
	    int ret;


	    /*
	     * The first reference to the entity trigger a parsing phase
	     * where the ent->children is filled with the result from
	     * the parsing.
	     */
	    if (ent->children == NULL) {
		xmlChar *value;
		value = ent->content;

		/*
		 * Check that this entity is well formed
		 */
		if ((value != NULL) &&
		    (value[1] == 0) && (value[0] == '<') &&
		    (xmlStrEqual(ent->name, BAD_CAST "lt"))) {
		    /*
		     * DONE: get definite answer on this !!!
		     * Lots of entity decls are used to declare a single
		     * char 
		     *    <!ENTITY lt     "<">
		     * Which seems to be valid since
		     * 2.4: The ampersand character (&) and the left angle
		     * bracket (<) may appear in their literal form only
		     * when used ... They are also legal within the literal
		     * entity value of an internal entity declaration;i
		     * see "4.3.2 Well-Formed Parsed Entities". 
		     * IMHO 2.4 and 4.3.2 are directly in contradiction.
		     * Looking at the OASIS test suite and James Clark 
		     * tests, this is broken. However the XML REC uses
		     * it. Is the XML REC not well-formed ????
		     * This is a hack to avoid this problem
		     *
		     * ANSWER: since lt gt amp .. are already defined,
		     *   this is a redefinition and hence the fact that the
		     *   content is not well balanced is not a Wf error, this
		     *   is lousy but acceptable.
		     */
		    list = xmlNewDocText(ctxt->myDoc, value);
		    if (list != NULL) {
			if ((ent->etype == XML_INTERNAL_GENERAL_ENTITY) &&
			    (ent->children == NULL)) {
			    ent->children = list;
			    ent->last = list;
			    list->parent = (xmlNodePtr) ent;
			} else {
			    xmlFreeNodeList(list);
			}
		    } else if (list != NULL) {
			xmlFreeNodeList(list);
		    }
		} else {
		    /*
		     * 4.3.2: An internal general parsed entity is well-formed
		     * if its replacement text matches the production labeled
		     * content.
		     */

		    void *user_data;
		    /*
		     * This is a bit hackish but this seems the best
		     * way to make sure both SAX and DOM entity support
		     * behaves okay.
		     */
		    if (ctxt->userData == ctxt)
			user_data = NULL;
		    else
			user_data = ctxt->userData;

		    if (ent->etype == XML_INTERNAL_GENERAL_ENTITY) {
			ctxt->depth++;
			ret = xmlParseBalancedChunkMemoryInternal(ctxt,
					   value, user_data, &list);
			ctxt->depth--;
		    } else if (ent->etype ==
			       XML_EXTERNAL_GENERAL_PARSED_ENTITY) {
			ctxt->depth++;
			ret = xmlParseExternalEntityPrivate(ctxt->myDoc, ctxt,
				   ctxt->sax, user_data, ctxt->depth,
				   ent->URI, ent->ExternalID, &list);
			ctxt->depth--;
		    } else {
			ret = -1;
			if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			    ctxt->sax->error(ctxt->userData,
				"Internal: invalid entity type\n");
		    }
		    if (ret == XML_ERR_ENTITY_LOOP) {
			ctxt->errNo = XML_ERR_ENTITY_LOOP;
			if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			    ctxt->sax->error(ctxt->userData,
				"Detected entity reference loop\n");
			ctxt->wellFormed = 0;
			if (ctxt->recovery == 0) ctxt->disableSAX = 1;
			return;
		    } else if ((ret == 0) && (list != NULL)) {
			if (((ent->etype == XML_INTERNAL_GENERAL_ENTITY) ||
			 (ent->etype == XML_EXTERNAL_GENERAL_PARSED_ENTITY))&&
			    (ent->children == NULL)) {
			    ent->children = list;
			    if (ctxt->replaceEntities) {
				/*
				 * Prune it directly in the generated document
				 * except for single text nodes.
				 */
				if ((list->type == XML_TEXT_NODE) &&
				    (list->next == NULL)) {
				    list->parent = (xmlNodePtr) ent;
				    list = NULL;
				} else {
				    while (list != NULL) {
					list->parent = (xmlNodePtr) ctxt->node;
					list->doc = ctxt->myDoc;
					if (list->next == NULL)
					    ent->last = list;
					list = list->next;
			 	    }
				    list = ent->children;
				    if (ent->etype == XML_EXTERNAL_GENERAL_PARSED_ENTITY)
				      xmlAddEntityReference(ent, list, NULL);
				}
			    } else {
				while (list != NULL) {
				    list->parent = (xmlNodePtr) ent;
				    if (list->next == NULL)
					ent->last = list;
				    list = list->next;
				}
			    }
			} else {
			    xmlFreeNodeList(list);
			    list = NULL;
			}
		    } else if (ret > 0) {
			ctxt->errNo = ret;
			if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			    ctxt->sax->error(ctxt->userData,
				"Entity value required\n");
			ctxt->wellFormed = 0;
			if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		    } else if (list != NULL) {
			xmlFreeNodeList(list);
			list = NULL;
		    }
		}
	    }
	    if ((ctxt->sax != NULL) && (ctxt->sax->reference != NULL) &&
		(ctxt->replaceEntities == 0) && (!ctxt->disableSAX)) {
		/*
		 * Create a node.
		 */
		ctxt->sax->reference(ctxt->userData, ent->name);
		return;
	    } else if (ctxt->replaceEntities) {
		if ((ctxt->node != NULL) && (ent->children != NULL)) {
		    /*
		     * Seems we are generating the DOM content, do
		     * a simple tree copy for all references except the first
		     * In the first occurrence list contains the replacement
		     */
		    if (list == NULL) {
			xmlNodePtr new = NULL, cur, firstChild = NULL;
			cur = ent->children;
			while (cur != NULL) {
			    new = xmlCopyNode(cur, 1);
			    if (firstChild == NULL){
			      firstChild = new;
			    }
			    xmlAddChild(ctxt->node, new);
			    if (cur == ent->last)
				break;
			    cur = cur->next;
			}
			if (ent->etype == XML_EXTERNAL_GENERAL_PARSED_ENTITY)			      
			  xmlAddEntityReference(ent, firstChild, new);
		    } else {
			/*
			 * the name change is to avoid coalescing of the
			 * node with a possible previous text one which
			 * would make ent->children a dangling pointer
			 */
			if (ent->children->type == XML_TEXT_NODE)
			    ent->children->name = xmlStrdup(BAD_CAST "nbktext");
			if ((ent->last != ent->children) &&
			    (ent->last->type == XML_TEXT_NODE))
			    ent->last->name = xmlStrdup(BAD_CAST "nbktext");
			xmlAddChildList(ctxt->node, ent->children);
		    }

		    /*
		     * This is to avoid a nasty side effect, see
		     * characters() in SAX.c
		     */
		    ctxt->nodemem = 0;
		    ctxt->nodelen = 0;
		    return;
		} else {
		    /*
		     * Probably running in SAX mode
		     */
		    xmlParserInputPtr input;

		    input = xmlNewEntityInputStream(ctxt, ent);
		    xmlPushInput(ctxt, input);
		    if ((ent->etype == XML_EXTERNAL_GENERAL_PARSED_ENTITY) &&
			(RAW == '<') && (NXT(1) == '?') &&
			(NXT(2) == 'x') && (NXT(3) == 'm') &&
			(NXT(4) == 'l') && (IS_BLANK(NXT(5)))) {
			xmlParseTextDecl(ctxt);
			if (ctxt->errNo == XML_ERR_UNSUPPORTED_ENCODING) {
			    /*
			     * The XML REC instructs us to stop parsing right here
			     */
			    ctxt->instate = XML_PARSER_EOF;
			    return;
			}
			if (input->standalone == 1) {
			    ctxt->errNo = XML_ERR_EXT_ENTITY_STANDALONE;
			    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
				ctxt->sax->error(ctxt->userData,
				"external parsed entities cannot be standalone\n");
			    ctxt->wellFormed = 0;
			    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
			}
		    }
		    return;
		}
	    }
	} else {
	    val = ent->content;
	    if (val == NULL) return;
	    /*
	     * inline the entity.
	     */
	    if ((ctxt->sax != NULL) && (ctxt->sax->characters != NULL) &&
		(!ctxt->disableSAX))
		ctxt->sax->characters(ctxt->userData, val, xmlStrlen(val));
	}
    }
}

/**
 * xmlParseEntityRef:
 * @ctxt:  an XML parser context
 *
 * parse ENTITY references declarations
 *
 * [68] EntityRef ::= '&' Name ';'
 *
 * [ WFC: Entity Declared ]
 * In a document without any DTD, a document with only an internal DTD
 * subset which contains no parameter entity references, or a document
 * with "standalone='yes'", the Name given in the entity reference
 * must match that in an entity declaration, except that well-formed
 * documents need not declare any of the following entities: amp, lt,
 * gt, apos, quot.  The declaration of a parameter entity must precede
 * any reference to it.  Similarly, the declaration of a general entity
 * must precede any reference to it which appears in a default value in an
 * attribute-list declaration. Note that if entities are declared in the
 * external subset or in external parameter entities, a non-validating
 * processor is not obligated to read and process their declarations;
 * for such documents, the rule that an entity must be declared is a
 * well-formedness constraint only if standalone='yes'.
 *
 * [ WFC: Parsed Entity ]
 * An entity reference must not contain the name of an unparsed entity
 *
 * Returns the xmlEntityPtr if found, or NULL otherwise.
 */
xmlEntityPtr
xmlParseEntityRef(xmlParserCtxtPtr ctxt) {
    xmlChar *name;
    xmlEntityPtr ent = NULL;

    GROW;
    
    if (RAW == '&') {
        NEXT;
        name = xmlParseName(ctxt);
	if (name == NULL) {
	    ctxt->errNo = XML_ERR_NAME_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData,
		                 "xmlParseEntityRef: no name\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	} else {
	    if (RAW == ';') {
	        NEXT;
		/*
		 * Ask first SAX for entity resolution, otherwise try the
		 * predefined set.
		 */
		if (ctxt->sax != NULL) {
		    if (ctxt->sax->getEntity != NULL)
			ent = ctxt->sax->getEntity(ctxt->userData, name);
		    if (ent == NULL)
		        ent = xmlGetPredefinedEntity(name);
		    if ((ent == NULL) && (ctxt->userData==ctxt)) {
			ent = getEntity(ctxt, name);
		    }
		}
		/*
		 * [ WFC: Entity Declared ]
		 * In a document without any DTD, a document with only an
		 * internal DTD subset which contains no parameter entity
		 * references, or a document with "standalone='yes'", the
		 * Name given in the entity reference must match that in an
		 * entity declaration, except that well-formed documents
		 * need not declare any of the following entities: amp, lt,
		 * gt, apos, quot.
		 * The declaration of a parameter entity must precede any
		 * reference to it.
		 * Similarly, the declaration of a general entity must
		 * precede any reference to it which appears in a default
		 * value in an attribute-list declaration. Note that if
		 * entities are declared in the external subset or in
		 * external parameter entities, a non-validating processor
		 * is not obligated to read and process their declarations;
		 * for such documents, the rule that an entity must be
		 * declared is a well-formedness constraint only if
		 * standalone='yes'. 
		 */
		if (ent == NULL) {
		    if ((ctxt->standalone == 1) ||
		        ((ctxt->hasExternalSubset == 0) &&
			 (ctxt->hasPErefs == 0))) {
			ctxt->errNo = XML_ERR_UNDECLARED_ENTITY;
			if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			    ctxt->sax->error(ctxt->userData, 
				 "Entity '%s' not defined\n", name);
			ctxt->wellFormed = 0;
			ctxt->valid = 0;
			if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		    } else {
			ctxt->errNo = XML_WAR_UNDECLARED_ENTITY;
			if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			    ctxt->sax->error(ctxt->userData, 
				 "Entity '%s' not defined\n", name);
			ctxt->valid = 0;
		    }
		}

		/*
		 * [ WFC: Parsed Entity ]
		 * An entity reference must not contain the name of an
		 * unparsed entity
		 */
		else if (ent->etype == XML_EXTERNAL_GENERAL_UNPARSED_ENTITY) {
		    ctxt->errNo = XML_ERR_UNPARSED_ENTITY;
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData, 
			     "Entity reference to unparsed entity %s\n", name);
		    ctxt->wellFormed = 0;
		    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		}

		/*
		 * [ WFC: No External Entity References ]
		 * Attribute values cannot contain direct or indirect
		 * entity references to external entities.
		 */
		else if ((ctxt->instate == XML_PARSER_ATTRIBUTE_VALUE) &&
		         (ent->etype == XML_EXTERNAL_GENERAL_PARSED_ENTITY)) {
		    ctxt->errNo = XML_ERR_ENTITY_IS_EXTERNAL;
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData, 
		     "Attribute references external entity '%s'\n", name);
		    ctxt->wellFormed = 0;
		    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		}
		/*
		 * [ WFC: No < in Attribute Values ]
		 * The replacement text of any entity referred to directly or
		 * indirectly in an attribute value (other than "&lt;") must
		 * not contain a <. 
		 */
		else if ((ctxt->instate == XML_PARSER_ATTRIBUTE_VALUE) &&
		         (ent != NULL) &&
			 (!xmlStrEqual(ent->name, BAD_CAST "lt")) &&
		         (ent->content != NULL) &&
			 (xmlStrchr(ent->content, '<'))) {
		    ctxt->errNo = XML_ERR_LT_IN_ATTRIBUTE;
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData, 
	 "'<' in entity '%s' is not allowed in attributes values\n", name);
		    ctxt->wellFormed = 0;
		    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		}

		/*
		 * Internal check, no parameter entities here ...
		 */
		else {
		    switch (ent->etype) {
			case XML_INTERNAL_PARAMETER_ENTITY:
			case XML_EXTERNAL_PARAMETER_ENTITY:
			ctxt->errNo = XML_ERR_ENTITY_IS_PARAMETER;
			if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			    ctxt->sax->error(ctxt->userData, 
		     "Attempt to reference the parameter entity '%s'\n", name);
			ctxt->wellFormed = 0;
			if (ctxt->recovery == 0) ctxt->disableSAX = 1;
			break;
			default:
			break;
		    }
		}

		/*
		 * [ WFC: No Recursion ]
		 * A parsed entity must not contain a recursive reference
		 * to itself, either directly or indirectly. 
		 * Done somewhere else
		 */

	    } else {
		ctxt->errNo = XML_ERR_ENTITYREF_SEMICOL_MISSING;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
		                     "xmlParseEntityRef: expecting ';'\n");
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    }
	    xmlFree(name);
	}
    }
    return(ent);
}

/**
 * xmlParseStringEntityRef:
 * @ctxt:  an XML parser context
 * @str:  a pointer to an index in the string
 *
 * parse ENTITY references declarations, but this version parses it from
 * a string value.
 *
 * [68] EntityRef ::= '&' Name ';'
 *
 * [ WFC: Entity Declared ]
 * In a document without any DTD, a document with only an internal DTD
 * subset which contains no parameter entity references, or a document
 * with "standalone='yes'", the Name given in the entity reference
 * must match that in an entity declaration, except that well-formed
 * documents need not declare any of the following entities: amp, lt,
 * gt, apos, quot.  The declaration of a parameter entity must precede
 * any reference to it.  Similarly, the declaration of a general entity
 * must precede any reference to it which appears in a default value in an
 * attribute-list declaration. Note that if entities are declared in the
 * external subset or in external parameter entities, a non-validating
 * processor is not obligated to read and process their declarations;
 * for such documents, the rule that an entity must be declared is a
 * well-formedness constraint only if standalone='yes'.
 *
 * [ WFC: Parsed Entity ]
 * An entity reference must not contain the name of an unparsed entity
 *
 * Returns the xmlEntityPtr if found, or NULL otherwise. The str pointer
 * is updated to the current location in the string.
 */
xmlEntityPtr
xmlParseStringEntityRef(xmlParserCtxtPtr ctxt, const xmlChar ** str) {
    xmlChar *name;
    const xmlChar *ptr;
    xmlChar cur;
    xmlEntityPtr ent = NULL;

    if ((str == NULL) || (*str == NULL))
        return(NULL);
    ptr = *str;
    cur = *ptr;
    if (cur == '&') {
        ptr++;
	cur = *ptr;
        name = xmlParseStringName(ctxt, &ptr);
	if (name == NULL) {
	    ctxt->errNo = XML_ERR_NAME_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData,
		                 "xmlParseStringEntityRef: no name\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	} else {
	    if (*ptr == ';') {
	        ptr++;
		/*
		 * Ask first SAX for entity resolution, otherwise try the
		 * predefined set.
		 */
		if (ctxt->sax != NULL) {
		    if (ctxt->sax->getEntity != NULL)
			ent = ctxt->sax->getEntity(ctxt->userData, name);
		    if (ent == NULL)
		        ent = xmlGetPredefinedEntity(name);
		    if ((ent == NULL) && (ctxt->userData==ctxt)) {
			ent = getEntity(ctxt, name);
		    }
		}
		/*
		 * [ WFC: Entity Declared ]
		 * In a document without any DTD, a document with only an
		 * internal DTD subset which contains no parameter entity
		 * references, or a document with "standalone='yes'", the
		 * Name given in the entity reference must match that in an
		 * entity declaration, except that well-formed documents
		 * need not declare any of the following entities: amp, lt,
		 * gt, apos, quot.
		 * The declaration of a parameter entity must precede any
		 * reference to it.
		 * Similarly, the declaration of a general entity must
		 * precede any reference to it which appears in a default
		 * value in an attribute-list declaration. Note that if
		 * entities are declared in the external subset or in
		 * external parameter entities, a non-validating processor
		 * is not obligated to read and process their declarations;
		 * for such documents, the rule that an entity must be
		 * declared is a well-formedness constraint only if
		 * standalone='yes'. 
		 */
		if (ent == NULL) {
		    if ((ctxt->standalone == 1) ||
		        ((ctxt->hasExternalSubset == 0) &&
			 (ctxt->hasPErefs == 0))) {
			ctxt->errNo = XML_ERR_UNDECLARED_ENTITY;
			if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			    ctxt->sax->error(ctxt->userData, 
				 "Entity '%s' not defined\n", name);
			ctxt->wellFormed = 0;
			if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		    } else {
			ctxt->errNo = XML_WAR_UNDECLARED_ENTITY;
			if ((ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
			    ctxt->sax->warning(ctxt->userData, 
				 "Entity '%s' not defined\n", name);
		    }
		}

		/*
		 * [ WFC: Parsed Entity ]
		 * An entity reference must not contain the name of an
		 * unparsed entity
		 */
		else if (ent->etype == XML_EXTERNAL_GENERAL_UNPARSED_ENTITY) {
		    ctxt->errNo = XML_ERR_UNPARSED_ENTITY;
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData, 
			     "Entity reference to unparsed entity %s\n", name);
		    ctxt->wellFormed = 0;
		    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		}

		/*
		 * [ WFC: No External Entity References ]
		 * Attribute values cannot contain direct or indirect
		 * entity references to external entities.
		 */
		else if ((ctxt->instate == XML_PARSER_ATTRIBUTE_VALUE) &&
		         (ent->etype == XML_EXTERNAL_GENERAL_PARSED_ENTITY)) {
		    ctxt->errNo = XML_ERR_ENTITY_IS_EXTERNAL;
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData, 
		     "Attribute references external entity '%s'\n", name);
		    ctxt->wellFormed = 0;
		    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		}
		/*
		 * [ WFC: No < in Attribute Values ]
		 * The replacement text of any entity referred to directly or
		 * indirectly in an attribute value (other than "&lt;") must
		 * not contain a <. 
		 */
		else if ((ctxt->instate == XML_PARSER_ATTRIBUTE_VALUE) &&
		         (ent != NULL) &&
			 (!xmlStrEqual(ent->name, BAD_CAST "lt")) &&
		         (ent->content != NULL) &&
			 (xmlStrchr(ent->content, '<'))) {
		    ctxt->errNo = XML_ERR_LT_IN_ATTRIBUTE;
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData, 
	 "'<' in entity '%s' is not allowed in attributes values\n", name);
		    ctxt->wellFormed = 0;
		    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		}

		/*
		 * Internal check, no parameter entities here ...
		 */
		else {
		    switch (ent->etype) {
			case XML_INTERNAL_PARAMETER_ENTITY:
			case XML_EXTERNAL_PARAMETER_ENTITY:
			ctxt->errNo = XML_ERR_ENTITY_IS_PARAMETER;
			if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			    ctxt->sax->error(ctxt->userData, 
		     "Attempt to reference the parameter entity '%s'\n", name);
			ctxt->wellFormed = 0;
			if (ctxt->recovery == 0) ctxt->disableSAX = 1;
			break;
			default:
			break;
		    }
		}

		/*
		 * [ WFC: No Recursion ]
		 * A parsed entity must not contain a recursive reference
		 * to itself, either directly or indirectly. 
		 * Done somewhere else
		 */

	    } else {
		ctxt->errNo = XML_ERR_ENTITYREF_SEMICOL_MISSING;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
				 "xmlParseStringEntityRef: expecting ';'\n");
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    }
	    xmlFree(name);
	}
    }
    *str = ptr;
    return(ent);
}

/**
 * xmlParsePEReference:
 * @ctxt:  an XML parser context
 *
 * parse PEReference declarations
 * The entity content is handled directly by pushing it's content as
 * a new input stream.
 *
 * [69] PEReference ::= '%' Name ';'
 *
 * [ WFC: No Recursion ]
 * A parsed entity must not contain a recursive
 * reference to itself, either directly or indirectly. 
 *
 * [ WFC: Entity Declared ]
 * In a document without any DTD, a document with only an internal DTD
 * subset which contains no parameter entity references, or a document
 * with "standalone='yes'", ...  ... The declaration of a parameter
 * entity must precede any reference to it...
 *
 * [ VC: Entity Declared ]
 * In a document with an external subset or external parameter entities
 * with "standalone='no'", ...  ... The declaration of a parameter entity
 * must precede any reference to it...
 *
 * [ WFC: In DTD ]
 * Parameter-entity references may only appear in the DTD.
 * NOTE: misleading but this is handled.
 */
void
xmlParsePEReference(xmlParserCtxtPtr ctxt) {
    xmlChar *name;
    xmlEntityPtr entity = NULL;
    xmlParserInputPtr input;

    if (RAW == '%') {
        NEXT;
        name = xmlParseName(ctxt);
	if (name == NULL) {
	    ctxt->errNo = XML_ERR_NAME_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData,
		                 "xmlParsePEReference: no name\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	} else {
	    if (RAW == ';') {
	        NEXT;
		if ((ctxt->sax != NULL) &&
		    (ctxt->sax->getParameterEntity != NULL))
		    entity = ctxt->sax->getParameterEntity(ctxt->userData,
		                                           name);
		if (entity == NULL) {
		    /*
		     * [ WFC: Entity Declared ]
		     * In a document without any DTD, a document with only an
		     * internal DTD subset which contains no parameter entity
		     * references, or a document with "standalone='yes'", ...
		     * ... The declaration of a parameter entity must precede
		     * any reference to it...
		     */
		    if ((ctxt->standalone == 1) ||
			((ctxt->hasExternalSubset == 0) &&
			 (ctxt->hasPErefs == 0))) {
			ctxt->errNo = XML_ERR_UNDECLARED_ENTITY;
			if ((!ctxt->disableSAX) &&
			    (ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			    ctxt->sax->error(ctxt->userData,
			     "PEReference: %%%s; not found\n", name);
			ctxt->wellFormed = 0;
			if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		    } else {
			/*
			 * [ VC: Entity Declared ]
			 * In a document with an external subset or external
			 * parameter entities with "standalone='no'", ...
			 * ... The declaration of a parameter entity must precede
			 * any reference to it...
			 */
			if ((!ctxt->disableSAX) &&
			    (ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
			    ctxt->sax->warning(ctxt->userData,
			     "PEReference: %%%s; not found\n", name);
			ctxt->valid = 0;
		    }
		} else {
		    /*
		     * Internal checking in case the entity quest barfed
		     */
		    if ((entity->etype != XML_INTERNAL_PARAMETER_ENTITY) &&
		        (entity->etype != XML_EXTERNAL_PARAMETER_ENTITY)) {
			if ((ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
			    ctxt->sax->warning(ctxt->userData,
			 "Internal: %%%s; is not a parameter entity\n", name);
	        } else if (ctxt->input->free != deallocblankswrapper) {
		    input = xmlNewBlanksWrapperInputStream(ctxt, entity);
		    xmlPushInput(ctxt, input);
		    } else {
			/*
			 * TODO !!!
			 * handle the extra spaces added before and after
			 * c.f. http://www.w3.org/TR/REC-xml#as-PE
			 */
			input = xmlNewEntityInputStream(ctxt, entity);
			xmlPushInput(ctxt, input);
			if ((entity->etype == XML_EXTERNAL_PARAMETER_ENTITY) &&
			    (RAW == '<') && (NXT(1) == '?') &&
			    (NXT(2) == 'x') && (NXT(3) == 'm') &&
			    (NXT(4) == 'l') && (IS_BLANK(NXT(5)))) {
			    xmlParseTextDecl(ctxt);
			    if (ctxt->errNo == XML_ERR_UNSUPPORTED_ENCODING) {
				/*
				 * The XML REC instructs us to stop parsing
				 * right here
				 */
				ctxt->instate = XML_PARSER_EOF;
				xmlFree(name);
				return;
			    }
			}
		    }
		}
		ctxt->hasPErefs = 1;
	    } else {
		ctxt->errNo = XML_ERR_ENTITYREF_SEMICOL_MISSING;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
		                     "xmlParsePEReference: expecting ';'\n");
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    }
	    xmlFree(name);
	}
    }
}

/**
 * xmlParseStringPEReference:
 * @ctxt:  an XML parser context
 * @str:  a pointer to an index in the string
 *
 * parse PEReference declarations
 *
 * [69] PEReference ::= '%' Name ';'
 *
 * [ WFC: No Recursion ]
 * A parsed entity must not contain a recursive
 * reference to itself, either directly or indirectly. 
 *
 * [ WFC: Entity Declared ]
 * In a document without any DTD, a document with only an internal DTD
 * subset which contains no parameter entity references, or a document
 * with "standalone='yes'", ...  ... The declaration of a parameter
 * entity must precede any reference to it...
 *
 * [ VC: Entity Declared ]
 * In a document with an external subset or external parameter entities
 * with "standalone='no'", ...  ... The declaration of a parameter entity
 * must precede any reference to it...
 *
 * [ WFC: In DTD ]
 * Parameter-entity references may only appear in the DTD.
 * NOTE: misleading but this is handled.
 *
 * Returns the string of the entity content.
 *         str is updated to the current value of the index
 */
xmlEntityPtr
xmlParseStringPEReference(xmlParserCtxtPtr ctxt, const xmlChar **str) {
    const xmlChar *ptr;
    xmlChar cur;
    xmlChar *name;
    xmlEntityPtr entity = NULL;

    if ((str == NULL) || (*str == NULL)) return(NULL);
    ptr = *str;
    cur = *ptr;
    if (cur == '%') {
        ptr++;
	cur = *ptr;
        name = xmlParseStringName(ctxt, &ptr);
	if (name == NULL) {
	    ctxt->errNo = XML_ERR_NAME_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData,
		                 "xmlParseStringPEReference: no name\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	} else {
	    cur = *ptr;
	    if (cur == ';') {
		ptr++;
		cur = *ptr;
		if ((ctxt->sax != NULL) &&
		    (ctxt->sax->getParameterEntity != NULL))
		    entity = ctxt->sax->getParameterEntity(ctxt->userData,
		                                           name);
		if (entity == NULL) {
		    /*
		     * [ WFC: Entity Declared ]
		     * In a document without any DTD, a document with only an
		     * internal DTD subset which contains no parameter entity
		     * references, or a document with "standalone='yes'", ...
		     * ... The declaration of a parameter entity must precede
		     * any reference to it...
		     */
		    if ((ctxt->standalone == 1) ||
			((ctxt->hasExternalSubset == 0) &&
			 (ctxt->hasPErefs == 0))) {
			ctxt->errNo = XML_ERR_UNDECLARED_ENTITY;
			if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			    ctxt->sax->error(ctxt->userData,
			     "PEReference: %%%s; not found\n", name);
			ctxt->wellFormed = 0;
			if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		    } else {
			/*
			 * [ VC: Entity Declared ]
			 * In a document with an external subset or external
			 * parameter entities with "standalone='no'", ...
			 * ... The declaration of a parameter entity must
			 * precede any reference to it...
			 */
			if ((ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
			    ctxt->sax->warning(ctxt->userData,
			     "PEReference: %%%s; not found\n", name);
			ctxt->valid = 0;
		    }
		} else {
		    /*
		     * Internal checking in case the entity quest barfed
		     */
		    if ((entity->etype != XML_INTERNAL_PARAMETER_ENTITY) &&
		        (entity->etype != XML_EXTERNAL_PARAMETER_ENTITY)) {
			if ((ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
			    ctxt->sax->warning(ctxt->userData,
			 "Internal: %%%s; is not a parameter entity\n", name);
		    }
		}
		ctxt->hasPErefs = 1;
	    } else {
		ctxt->errNo = XML_ERR_ENTITYREF_SEMICOL_MISSING;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
		                     "xmlParseStringPEReference: expecting ';'\n");
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    }
	    xmlFree(name);
	}
    }
    *str = ptr;
    return(entity);
}

/**
 * xmlParseDocTypeDecl:
 * @ctxt:  an XML parser context
 *
 * parse a DOCTYPE declaration
 *
 * [28] doctypedecl ::= '<!DOCTYPE' S Name (S ExternalID)? S? 
 *                      ('[' (markupdecl | PEReference | S)* ']' S?)? '>'
 *
 * [ VC: Root Element Type ]
 * The Name in the document type declaration must match the element
 * type of the root element. 
 */

void
xmlParseDocTypeDecl(xmlParserCtxtPtr ctxt) {
    xmlChar *name = NULL;
    xmlChar *ExternalID = NULL;
    xmlChar *URI = NULL;

    /*
     * We know that '<!DOCTYPE' has been detected.
     */
    SKIP(9);

    SKIP_BLANKS;

    /*
     * Parse the DOCTYPE name.
     */
    name = xmlParseName(ctxt);
    if (name == NULL) {
	ctxt->errNo = XML_ERR_NAME_REQUIRED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, 
	        "xmlParseDocTypeDecl : no DOCTYPE name !\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    }
    ctxt->intSubName = name;

    SKIP_BLANKS;

    /*
     * Check for SystemID and ExternalID
     */
    URI = xmlParseExternalID(ctxt, &ExternalID, 1);

    if ((URI != NULL) || (ExternalID != NULL)) {
        ctxt->hasExternalSubset = 1;
    }
    ctxt->extSubURI = URI;
    ctxt->extSubSystem = ExternalID;

    SKIP_BLANKS;

    /*
     * Create and update the internal subset.
     */
    if ((ctxt->sax != NULL) && (ctxt->sax->internalSubset != NULL) &&
	(!ctxt->disableSAX))
	ctxt->sax->internalSubset(ctxt->userData, name, ExternalID, URI);

    /*
     * Is there any internal subset declarations ?
     * they are handled separately in xmlParseInternalSubset()
     */
    if (RAW == '[')
	return;

    /*
     * We should be at the end of the DOCTYPE declaration.
     */
    if (RAW != '>') {
	ctxt->errNo = XML_ERR_DOCTYPE_NOT_FINISHED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, "DOCTYPE improperly terminated\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    }
    NEXT;
}

/**
 * xmlParseInternalSubset:
 * @ctxt:  an XML parser context
 *
 * parse the internal subset declaration
 *
 * [28 end] ('[' (markupdecl | PEReference | S)* ']' S?)? '>'
 */

static void
xmlParseInternalSubset(xmlParserCtxtPtr ctxt) {
    /*
     * Is there any DTD definition ?
     */
    if (RAW == '[') {
        ctxt->instate = XML_PARSER_DTD;
        NEXT;
	/*
	 * Parse the succession of Markup declarations and 
	 * PEReferences.
	 * Subsequence (markupdecl | PEReference | S)*
	 */
	while (RAW != ']') {
	    const xmlChar *check = CUR_PTR;
	    int cons = ctxt->input->consumed;

	    SKIP_BLANKS;
	    xmlParseMarkupDecl(ctxt);
	    xmlParsePEReference(ctxt);

	    /*
	     * Pop-up of finished entities.
	     */
	    while ((RAW == 0) && (ctxt->inputNr > 1))
		xmlPopInput(ctxt);

	    if ((CUR_PTR == check) && (cons == ctxt->input->consumed)) {
		ctxt->errNo = XML_ERR_INTERNAL_ERROR;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData, 
	     "xmlParseInternalSubset: error detected in Markup declaration\n");
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		break;
	    }
	}
	if (RAW == ']') { 
	    NEXT;
	    SKIP_BLANKS;
	}
    }

    /*
     * We should be at the end of the DOCTYPE declaration.
     */
    if (RAW != '>') {
	ctxt->errNo = XML_ERR_DOCTYPE_NOT_FINISHED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, "DOCTYPE improperly terminated\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    }
    NEXT;
}

/**
 * xmlParseAttribute:
 * @ctxt:  an XML parser context
 * @value:  a xmlChar ** used to store the value of the attribute
 *
 * parse an attribute
 *
 * [41] Attribute ::= Name Eq AttValue
 *
 * [ WFC: No External Entity References ]
 * Attribute values cannot contain direct or indirect entity references
 * to external entities.
 *
 * [ WFC: No < in Attribute Values ]
 * The replacement text of any entity referred to directly or indirectly in
 * an attribute value (other than "&lt;") must not contain a <. 
 * 
 * [ VC: Attribute Value Type ]
 * The attribute must have been declared; the value must be of the type
 * declared for it.
 *
 * [25] Eq ::= S? '=' S?
 *
 * With namespace:
 *
 * [NS 11] Attribute ::= QName Eq AttValue
 *
 * Also the case QName == xmlns:??? is handled independently as a namespace
 * definition.
 *
 * Returns the attribute name, and the value in *value.
 */

xmlChar *
xmlParseAttribute(xmlParserCtxtPtr ctxt, xmlChar **value) {
    xmlChar *name, *val;

    *value = NULL;
    GROW;
    name = xmlParseName(ctxt);
    if (name == NULL) {
	ctxt->errNo = XML_ERR_NAME_REQUIRED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, "error parsing attribute name\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
        return(NULL);
    }

    /*
     * read the value
     */
    SKIP_BLANKS;
    if (RAW == '=') {
        NEXT;
	SKIP_BLANKS;
	val = xmlParseAttValue(ctxt);
	ctxt->instate = XML_PARSER_CONTENT;
    } else {
	ctxt->errNo = XML_ERR_ATTRIBUTE_WITHOUT_VALUE;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	       "Specification mandate value for attribute %s\n", name);
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	xmlFree(name);
	return(NULL);
    }

    /*
     * Check that xml:lang conforms to the specification
     * No more registered as an error, just generate a warning now
     * since this was deprecated in XML second edition
     */
    if ((ctxt->pedantic) && (xmlStrEqual(name, BAD_CAST "xml:lang"))) {
	if (!xmlCheckLanguageID(val)) {
	    if ((ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
		ctxt->sax->warning(ctxt->userData,
		   "Malformed value for xml:lang : %s\n", val);
	}
    }

    /*
     * Check that xml:space conforms to the specification
     */
    if (xmlStrEqual(name, BAD_CAST "xml:space")) {
	if (xmlStrEqual(val, BAD_CAST "default"))
	    *(ctxt->space) = 0;
	else if (xmlStrEqual(val, BAD_CAST "preserve"))
	    *(ctxt->space) = 1;
	else {
	    ctxt->errNo = XML_ERR_ATTRIBUTE_WITHOUT_VALUE;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData,
"Invalid value for xml:space : \"%s\", \"default\" or \"preserve\" expected\n",
                                 val);
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	}
    }

    *value = val;
    return(name);
}

/**
 * xmlParseStartTag:
 * @ctxt:  an XML parser context
 * 
 * parse a start of tag either for rule element or
 * EmptyElement. In both case we don't parse the tag closing chars.
 *
 * [40] STag ::= '<' Name (S Attribute)* S? '>'
 *
 * [ WFC: Unique Att Spec ]
 * No attribute name may appear more than once in the same start-tag or
 * empty-element tag. 
 *
 * [44] EmptyElemTag ::= '<' Name (S Attribute)* S? '/>'
 *
 * [ WFC: Unique Att Spec ]
 * No attribute name may appear more than once in the same start-tag or
 * empty-element tag. 
 *
 * With namespace:
 *
 * [NS 8] STag ::= '<' QName (S Attribute)* S? '>'
 *
 * [NS 10] EmptyElement ::= '<' QName (S Attribute)* S? '/>'
 *
 * Returns the element name parsed
 */

xmlChar *
xmlParseStartTag(xmlParserCtxtPtr ctxt) {
    xmlChar *name;
    xmlChar *attname;
    xmlChar *attvalue;
    const xmlChar **atts = NULL;
    int nbatts = 0;
    int maxatts = 0;
    int i;

    if (RAW != '<') return(NULL);
    NEXT1;

    name = xmlParseName(ctxt);
    if (name == NULL) {
	ctxt->errNo = XML_ERR_NAME_REQUIRED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, 
	     "xmlParseStartTag: invalid element name\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
        return(NULL);
    }

    /*
     * Now parse the attributes, it ends up with the ending
     *
     * (S Attribute)* S?
     */
    SKIP_BLANKS;
    GROW;

    while ((RAW != '>') && 
	   ((RAW != '/') || (NXT(1) != '>')) &&
	   (IS_CHAR(RAW))) {
	const xmlChar *q = CUR_PTR;
	int cons = ctxt->input->consumed;

	attname = xmlParseAttribute(ctxt, &attvalue);
        if ((attname != NULL) && (attvalue != NULL)) {
	    /*
	     * [ WFC: Unique Att Spec ]
	     * No attribute name may appear more than once in the same
	     * start-tag or empty-element tag. 
	     */
	    for (i = 0; i < nbatts;i += 2) {
	        if (xmlStrEqual(atts[i], attname)) {
		    ctxt->errNo = XML_ERR_ATTRIBUTE_REDEFINED;
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData,
			        "Attribute %s redefined\n",
			                 attname);
		    ctxt->wellFormed = 0;
		    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		    xmlFree(attname);
		    xmlFree(attvalue);
		    goto failed;
		}
	    }

	    /*
	     * Add the pair to atts
	     */
	    if (atts == NULL) {
	        maxatts = 10;
	        atts = (const xmlChar **) xmlMalloc(maxatts * sizeof(xmlChar *));
		if (atts == NULL) {
		    xmlGenericError(xmlGenericErrorContext,
			    "malloc of %ld byte failed\n",
			    maxatts * (long)sizeof(xmlChar *));
		    return(NULL);
		}
	    } else if (nbatts + 4 > maxatts) {
	        maxatts *= 2;
	        atts = (const xmlChar **) xmlRealloc((void *) atts,
						     maxatts * sizeof(xmlChar *));
		if (atts == NULL) {
		    xmlGenericError(xmlGenericErrorContext,
			    "realloc of %ld byte failed\n",
			    maxatts * (long)sizeof(xmlChar *));
		    return(NULL);
		}
	    }
	    atts[nbatts++] = attname;
	    atts[nbatts++] = attvalue;
	    atts[nbatts] = NULL;
	    atts[nbatts + 1] = NULL;
	} else {
	    if (attname != NULL)
		xmlFree(attname);
	    if (attvalue != NULL)
		xmlFree(attvalue);
	}

failed:     

	if ((RAW == '>') || (((RAW == '/') && (NXT(1) == '>'))))
	    break;
	if (!IS_BLANK(RAW)) {
	    ctxt->errNo = XML_ERR_SPACE_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData,
		    "attributes construct error\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	}
	SKIP_BLANKS;
        if ((cons == ctxt->input->consumed) && (q == CUR_PTR)) {
	    ctxt->errNo = XML_ERR_INTERNAL_ERROR;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData, 
	         "xmlParseStartTag: problem parsing attributes\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    break;
	}
        GROW;
    }

    /*
     * SAX: Start of Element !
     */
    if ((ctxt->sax != NULL) && (ctxt->sax->startElement != NULL) &&
	(!ctxt->disableSAX))
        ctxt->sax->startElement(ctxt->userData, name, atts);

    if (atts != NULL) {
        for (i = 0;i < nbatts;i++) xmlFree((xmlChar *) atts[i]);
	xmlFree((void *) atts);
    }
    return(name);
}

/**
 * xmlParseEndTag:
 * @ctxt:  an XML parser context
 *
 * parse an end of tag
 *
 * [42] ETag ::= '</' Name S? '>'
 *
 * With namespace
 *
 * [NS 9] ETag ::= '</' QName S? '>'
 */

void
xmlParseEndTag(xmlParserCtxtPtr ctxt) {
    xmlChar *name;
    xmlChar *oldname;

    GROW;
    if ((RAW != '<') || (NXT(1) != '/')) {
	ctxt->errNo = XML_ERR_LTSLASH_REQUIRED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, "xmlParseEndTag: '</' not found\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	return;
    }
    SKIP(2);

    name = xmlParseNameAndCompare(ctxt,ctxt->name);

    /*
     * We should definitely be at the ending "S? '>'" part
     */
    GROW;
    SKIP_BLANKS;
    if ((!IS_CHAR(RAW)) || (RAW != '>')) {
	ctxt->errNo = XML_ERR_GT_REQUIRED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, "End tag : expected '>'\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    } else
	NEXT1;

    /*
     * [ WFC: Element Type Match ]
     * The Name in an element's end-tag must match the element type in the
     * start-tag. 
     *
     */
    if (name != (xmlChar*)1) {
	ctxt->errNo = XML_ERR_TAG_NAME_MISMATCH;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL)) {
	    if (name != NULL) {
		ctxt->sax->error(ctxt->userData,
		     "Opening and ending tag mismatch: %s and %s\n",
		                 ctxt->name, name);
            } else {
		ctxt->sax->error(ctxt->userData,
		     "Ending tag error for: %s\n", ctxt->name);
	    }

	}     
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
#if 0
	else {
	    /*
	     * Recover in case of one missing close
	     */
	    if ((ctxt->nameNr > 2) && 
		(xmlStrEqual(ctxt->nameTab[ctxt->nameNr -2], name))) {
		namePop(ctxt);
		spacePop(ctxt);
	    }
	}
#endif
	if (name != NULL)
	    xmlFree(name);		 
    }

    /*
     * SAX: End of Tag
     */
    if ((ctxt->sax != NULL) && (ctxt->sax->endElement != NULL) &&
	(!ctxt->disableSAX))
        ctxt->sax->endElement(ctxt->userData, ctxt->name);

    oldname = namePop(ctxt);
    spacePop(ctxt);
    if (oldname != NULL) {
#ifdef DEBUG_STACK
	xmlGenericError(xmlGenericErrorContext,"Close: popped %s\n", oldname);
#endif
	xmlFree(oldname);
    }
    return;
}

/**
 * xmlParseCDSect:
 * @ctxt:  an XML parser context
 * 
 * Parse escaped pure raw content.
 *
 * [18] CDSect ::= CDStart CData CDEnd
 *
 * [19] CDStart ::= '<![CDATA['
 *
 * [20] Data ::= (Char* - (Char* ']]>' Char*))
 *
 * [21] CDEnd ::= ']]>'
 */
void
xmlParseCDSect(xmlParserCtxtPtr ctxt) {
    xmlChar *buf = NULL;
    int len = 0;
    int size = XML_PARSER_BUFFER_SIZE;
    int r, rl;
    int	s, sl;
    int cur, l;
    int count = 0;

    if ((NXT(0) == '<') && (NXT(1) == '!') &&
	(NXT(2) == '[') && (NXT(3) == 'C') &&
	(NXT(4) == 'D') && (NXT(5) == 'A') &&
	(NXT(6) == 'T') && (NXT(7) == 'A') &&
	(NXT(8) == '[')) {
	SKIP(9);
    } else
        return;

    ctxt->instate = XML_PARSER_CDATA_SECTION;
    r = CUR_CHAR(rl);
    if (!IS_CHAR(r)) {
	ctxt->errNo = XML_ERR_CDATA_NOT_FINISHED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	                     "CData section not finished\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	ctxt->instate = XML_PARSER_CONTENT;
        return;
    }
    NEXTL(rl);
    s = CUR_CHAR(sl);
    if (!IS_CHAR(s)) {
	ctxt->errNo = XML_ERR_CDATA_NOT_FINISHED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	                     "CData section not finished\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	ctxt->instate = XML_PARSER_CONTENT;
        return;
    }
    NEXTL(sl);
    cur = CUR_CHAR(l);
    buf = (xmlChar *) xmlMalloc(size * sizeof(xmlChar));
    if (buf == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		"malloc of %d byte failed\n", size);
	return;
    }
    while (IS_CHAR(cur) &&
           ((r != ']') || (s != ']') || (cur != '>'))) {
	if (len + 5 >= size) {
	    size *= 2;
	    buf = (xmlChar *) xmlRealloc(buf, size * sizeof(xmlChar));
	    if (buf == NULL) {
		xmlGenericError(xmlGenericErrorContext,
			"realloc of %d byte failed\n", size);
		return;
	    }
	}
	COPY_BUF(rl,buf,len,r);
	r = s;
	rl = sl;
	s = cur;
	sl = l;
	count++;
	if (count > 50) {
	    GROW;
	    count = 0;
	}
	NEXTL(l);
	cur = CUR_CHAR(l);
    }
    buf[len] = 0;
    ctxt->instate = XML_PARSER_CONTENT;
    if (cur != '>') {
	ctxt->errNo = XML_ERR_CDATA_NOT_FINISHED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	                     "CData section not finished\n%.50s\n", buf);
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	xmlFree(buf);
        return;
    }
    NEXTL(l);

    /*
     * OK the buffer is to be consumed as cdata.
     */
    if ((ctxt->sax != NULL) && (!ctxt->disableSAX)) {
	if (ctxt->sax->cdataBlock != NULL)
	    ctxt->sax->cdataBlock(ctxt->userData, buf, len);
	else if (ctxt->sax->characters != NULL)
	    ctxt->sax->characters(ctxt->userData, buf, len);
    }
    xmlFree(buf);
}

/**
 * xmlParseContent:
 * @ctxt:  an XML parser context
 *
 * Parse a content:
 *
 * [43] content ::= (element | CharData | Reference | CDSect | PI | Comment)*
 */

void
xmlParseContent(xmlParserCtxtPtr ctxt) {
    GROW;
    while ((RAW != 0) &&
	   ((RAW != '<') || (NXT(1) != '/'))) {
	const xmlChar *test = CUR_PTR;
	int cons = ctxt->input->consumed;
	const xmlChar *cur = ctxt->input->cur;

	/*
	 * First case : a Processing Instruction.
	 */
	if ((*cur == '<') && (cur[1] == '?')) {
	    xmlParsePI(ctxt);
	}

	/*
	 * Second case : a CDSection
	 */
	else if ((*cur == '<') && (NXT(1) == '!') &&
	    (NXT(2) == '[') && (NXT(3) == 'C') &&
	    (NXT(4) == 'D') && (NXT(5) == 'A') &&
	    (NXT(6) == 'T') && (NXT(7) == 'A') &&
	    (NXT(8) == '[')) {
	    xmlParseCDSect(ctxt);
	}

	/*
	 * Third case :  a comment
	 */
	else if ((*cur == '<') && (NXT(1) == '!') &&
		 (NXT(2) == '-') && (NXT(3) == '-')) {
	    xmlParseComment(ctxt);
	    ctxt->instate = XML_PARSER_CONTENT;
	}

	/*
	 * Fourth case :  a sub-element.
	 */
	else if (*cur == '<') {
	    xmlParseElement(ctxt);
	}

	/*
	 * Fifth case : a reference. If if has not been resolved,
	 *    parsing returns it's Name, create the node 
	 */

	else if (*cur == '&') {
	    xmlParseReference(ctxt);
	}

	/*
	 * Last case, text. Note that References are handled directly.
	 */
	else {
	    xmlParseCharData(ctxt, 0);
	}

	GROW;
	/*
	 * Pop-up of finished entities.
	 */
	while ((RAW == 0) && (ctxt->inputNr > 1))
	    xmlPopInput(ctxt);
	SHRINK;

	if ((cons == ctxt->input->consumed) && (test == CUR_PTR)) {
	    ctxt->errNo = XML_ERR_INTERNAL_ERROR;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData,
		     "detected an error in element content\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    ctxt->instate = XML_PARSER_EOF;
            break;
	}
    }
}

/**
 * xmlParseElement:
 * @ctxt:  an XML parser context
 *
 * parse an XML element, this is highly recursive
 *
 * [39] element ::= EmptyElemTag | STag content ETag
 *
 * [ WFC: Element Type Match ]
 * The Name in an element's end-tag must match the element type in the
 * start-tag. 
 *
 * [ VC: Element Valid ]
 * An element is valid if there is a declaration matching elementdecl
 * where the Name matches the element type and one of the following holds:
 *  - The declaration matches EMPTY and the element has no content.
 *  - The declaration matches children and the sequence of child elements
 *    belongs to the language generated by the regular expression in the
 *    content model, with optional white space (characters matching the
 *    nonterminal S) between each pair of child elements. 
 *  - The declaration matches Mixed and the content consists of character
 *    data and child elements whose types match names in the content model. 
 *  - The declaration matches ANY, and the types of any child elements have
 *    been declared.
 */

void
xmlParseElement(xmlParserCtxtPtr ctxt) {
    xmlChar *name;
    xmlChar *oldname;
    xmlParserNodeInfo node_info;
    xmlNodePtr ret;

    /* Capture start position */
    if (ctxt->record_info) {
        node_info.begin_pos = ctxt->input->consumed +
                          (CUR_PTR - ctxt->input->base);
	node_info.begin_line = ctxt->input->line;
    }

    if (ctxt->spaceNr == 0)
	spacePush(ctxt, -1);
    else
	spacePush(ctxt, *ctxt->space);

    name = xmlParseStartTag(ctxt);
    if (name == NULL) {
	spacePop(ctxt);
        return;
    }
    namePush(ctxt, name);
    ret = ctxt->node;

    /*
     * [ VC: Root Element Type ]
     * The Name in the document type declaration must match the element
     * type of the root element. 
     */
    if (ctxt->validate && ctxt->wellFormed && ctxt->myDoc &&
        ctxt->node && (ctxt->node == ctxt->myDoc->children))
        ctxt->valid &= xmlValidateRoot(&ctxt->vctxt, ctxt->myDoc);

    /*
     * Check for an Empty Element.
     */
    if ((RAW == '/') && (NXT(1) == '>')) {
        SKIP(2);
	if ((ctxt->sax != NULL) && (ctxt->sax->endElement != NULL) &&
	    (!ctxt->disableSAX))
	    ctxt->sax->endElement(ctxt->userData, name);
	oldname = namePop(ctxt);
	spacePop(ctxt);
	if (oldname != NULL) {
#ifdef DEBUG_STACK
	    xmlGenericError(xmlGenericErrorContext,"Close: popped %s\n", oldname);
#endif
	    xmlFree(oldname);
	}
	if ( ret != NULL && ctxt->record_info ) {
	   node_info.end_pos = ctxt->input->consumed +
			      (CUR_PTR - ctxt->input->base);
	   node_info.end_line = ctxt->input->line;
	   node_info.node = ret;
	   xmlParserAddNodeInfo(ctxt, &node_info);
	}
	return;
    }
    if (RAW == '>') {
        NEXT1;
    } else {
	ctxt->errNo = XML_ERR_GT_REQUIRED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	                     "Couldn't find end of Start Tag %s\n",
	                     name);
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;

	/*
	 * end of parsing of this node.
	 */
	nodePop(ctxt);
	oldname = namePop(ctxt);
	spacePop(ctxt);
	if (oldname != NULL) {
#ifdef DEBUG_STACK
	    xmlGenericError(xmlGenericErrorContext,"Close: popped %s\n", oldname);
#endif
	    xmlFree(oldname);
	}

	/*
	 * Capture end position and add node
	 */
	if ( ret != NULL && ctxt->record_info ) {
	   node_info.end_pos = ctxt->input->consumed +
			      (CUR_PTR - ctxt->input->base);
	   node_info.end_line = ctxt->input->line;
	   node_info.node = ret;
	   xmlParserAddNodeInfo(ctxt, &node_info);
	}
	return;
    }

    /*
     * Parse the content of the element:
     */
    xmlParseContent(ctxt);
    if (!IS_CHAR(RAW)) {
	ctxt->errNo = XML_ERR_TAG_NOT_FINISHED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	         "Premature end of data in tag %s\n", name);
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;

	/*
	 * end of parsing of this node.
	 */
	nodePop(ctxt);
	oldname = namePop(ctxt);
	spacePop(ctxt);
	if (oldname != NULL) {
#ifdef DEBUG_STACK
	    xmlGenericError(xmlGenericErrorContext,"Close: popped %s\n", oldname);
#endif
	    xmlFree(oldname);
	}
	return;
    }

    /*
     * parse the end of tag: '</' should be here.
     */
    xmlParseEndTag(ctxt);

    /*
     * Capture end position and add node
     */
    if ( ret != NULL && ctxt->record_info ) {
       node_info.end_pos = ctxt->input->consumed +
                          (CUR_PTR - ctxt->input->base);
       node_info.end_line = ctxt->input->line;
       node_info.node = ret;
       xmlParserAddNodeInfo(ctxt, &node_info);
    }
}

/**
 * xmlParseVersionNum:
 * @ctxt:  an XML parser context
 *
 * parse the XML version value.
 *
 * [26] VersionNum ::= ([a-zA-Z0-9_.:] | '-')+
 *
 * Returns the string giving the XML version number, or NULL
 */
xmlChar *
xmlParseVersionNum(xmlParserCtxtPtr ctxt) {
    xmlChar *buf = NULL;
    int len = 0;
    int size = 10;
    xmlChar cur;

    buf = (xmlChar *) xmlMalloc(size * sizeof(xmlChar));
    if (buf == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		"malloc of %d byte failed\n", size);
	return(NULL);
    }
    cur = CUR;
    while (((cur >= 'a') && (cur <= 'z')) ||
           ((cur >= 'A') && (cur <= 'Z')) ||
           ((cur >= '0') && (cur <= '9')) ||
           (cur == '_') || (cur == '.') ||
	   (cur == ':') || (cur == '-')) {
	if (len + 1 >= size) {
	    size *= 2;
	    buf = (xmlChar *) xmlRealloc(buf, size * sizeof(xmlChar));
	    if (buf == NULL) {
		xmlGenericError(xmlGenericErrorContext,
			"realloc of %d byte failed\n", size);
		return(NULL);
	    }
	}
	buf[len++] = cur;
	NEXT;
	cur=CUR;
    }
    buf[len] = 0;
    return(buf);
}

/**
 * xmlParseVersionInfo:
 * @ctxt:  an XML parser context
 * 
 * parse the XML version.
 *
 * [24] VersionInfo ::= S 'version' Eq (' VersionNum ' | " VersionNum ")
 * 
 * [25] Eq ::= S? '=' S?
 *
 * Returns the version string, e.g. "1.0"
 */

xmlChar *
xmlParseVersionInfo(xmlParserCtxtPtr ctxt) {
    xmlChar *version = NULL;
    const xmlChar *q;

    if ((RAW == 'v') && (NXT(1) == 'e') &&
        (NXT(2) == 'r') && (NXT(3) == 's') &&
	(NXT(4) == 'i') && (NXT(5) == 'o') &&
	(NXT(6) == 'n')) {
	SKIP(7);
	SKIP_BLANKS;
	if (RAW != '=') {
	    ctxt->errNo = XML_ERR_EQUAL_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData,
		                 "xmlParseVersionInfo : expected '='\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    return(NULL);
        }
	NEXT;
	SKIP_BLANKS;
	if (RAW == '"') {
	    NEXT;
	    q = CUR_PTR;
	    version = xmlParseVersionNum(ctxt);
	    if (RAW != '"') {
		ctxt->errNo = XML_ERR_STRING_NOT_CLOSED;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData, 
		                     "String not closed\n%.50s\n", q);
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    } else
	        NEXT;
	} else if (RAW == '\''){
	    NEXT;
	    q = CUR_PTR;
	    version = xmlParseVersionNum(ctxt);
	    if (RAW != '\'') {
		ctxt->errNo = XML_ERR_STRING_NOT_CLOSED;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
		                     "String not closed\n%.50s\n", q);
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    } else
	        NEXT;
	} else {
	    ctxt->errNo = XML_ERR_STRING_NOT_STARTED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData,
		      "xmlParseVersionInfo : expected ' or \"\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	}
    }
    return(version);
}

/**
 * xmlParseEncName:
 * @ctxt:  an XML parser context
 *
 * parse the XML encoding name
 *
 * [81] EncName ::= [A-Za-z] ([A-Za-z0-9._] | '-')*
 *
 * Returns the encoding name value or NULL
 */
xmlChar *
xmlParseEncName(xmlParserCtxtPtr ctxt) {
    xmlChar *buf = NULL;
    int len = 0;
    int size = 10;
    xmlChar cur;

    cur = CUR;
    if (((cur >= 'a') && (cur <= 'z')) ||
        ((cur >= 'A') && (cur <= 'Z'))) {
	buf = (xmlChar *) xmlMalloc(size * sizeof(xmlChar));
	if (buf == NULL) {
	    xmlGenericError(xmlGenericErrorContext,
		    "malloc of %d byte failed\n", size);
	    return(NULL);
	}
	
	buf[len++] = cur;
	NEXT;
	cur = CUR;
	while (((cur >= 'a') && (cur <= 'z')) ||
	       ((cur >= 'A') && (cur <= 'Z')) ||
	       ((cur >= '0') && (cur <= '9')) ||
	       (cur == '.') || (cur == '_') ||
	       (cur == '-')) {
	    if (len + 1 >= size) {
		size *= 2;
		buf = (xmlChar *) xmlRealloc(buf, size * sizeof(xmlChar));
		if (buf == NULL) {
		    xmlGenericError(xmlGenericErrorContext,
			    "realloc of %d byte failed\n", size);
		    return(NULL);
		}
	    }
	    buf[len++] = cur;
	    NEXT;
	    cur = CUR;
	    if (cur == 0) {
	        SHRINK;
		GROW;
		cur = CUR;
	    }
        }
	buf[len] = 0;
    } else {
	ctxt->errNo = XML_ERR_ENCODING_NAME;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, "Invalid XML encoding name\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    }
    return(buf);
}

/**
 * xmlParseEncodingDecl:
 * @ctxt:  an XML parser context
 * 
 * parse the XML encoding declaration
 *
 * [80] EncodingDecl ::= S 'encoding' Eq ('"' EncName '"' |  "'" EncName "'")
 *
 * this setups the conversion filters.
 *
 * Returns the encoding value or NULL
 */

xmlChar *
xmlParseEncodingDecl(xmlParserCtxtPtr ctxt) {
    xmlChar *encoding = NULL;
    const xmlChar *q;

    SKIP_BLANKS;
    if ((RAW == 'e') && (NXT(1) == 'n') &&
        (NXT(2) == 'c') && (NXT(3) == 'o') &&
	(NXT(4) == 'd') && (NXT(5) == 'i') &&
	(NXT(6) == 'n') && (NXT(7) == 'g')) {
	SKIP(8);
	SKIP_BLANKS;
	if (RAW != '=') {
	    ctxt->errNo = XML_ERR_EQUAL_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData,
		                 "xmlParseEncodingDecl : expected '='\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    return(NULL);
        }
	NEXT;
	SKIP_BLANKS;
	if (RAW == '"') {
	    NEXT;
	    q = CUR_PTR;
	    encoding = xmlParseEncName(ctxt);
	    if (RAW != '"') {
		ctxt->errNo = XML_ERR_STRING_NOT_CLOSED;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData, 
		                     "String not closed\n%.50s\n", q);
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    } else
	        NEXT;
	} else if (RAW == '\''){
	    NEXT;
	    q = CUR_PTR;
	    encoding = xmlParseEncName(ctxt);
	    if (RAW != '\'') {
		ctxt->errNo = XML_ERR_STRING_NOT_CLOSED;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
		                     "String not closed\n%.50s\n", q);
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    } else
	        NEXT;
	} else {
	    ctxt->errNo = XML_ERR_STRING_NOT_STARTED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData,
		     "xmlParseEncodingDecl : expected ' or \"\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	}
	if (encoding != NULL) {
	    xmlCharEncoding enc;
	    xmlCharEncodingHandlerPtr handler;

	    if (ctxt->input->encoding != NULL)
		xmlFree((xmlChar *) ctxt->input->encoding);
	    ctxt->input->encoding = encoding;

	    enc = xmlParseCharEncoding((const char *) encoding);
	    /*
	     * registered set of known encodings
	     */
	    if (enc != XML_CHAR_ENCODING_ERROR) {
		xmlSwitchEncoding(ctxt, enc);
		if (ctxt->errNo == XML_ERR_UNSUPPORTED_ENCODING) {
		    ctxt->input->encoding = NULL;
		    xmlFree(encoding);
		    return(NULL);
		}
	    } else {
	        /*
		 * fallback for unknown encodings
		 */
                handler = xmlFindCharEncodingHandler((const char *) encoding);
		if (handler != NULL) {
		    xmlSwitchToEncoding(ctxt, handler);
		} else {
		    ctxt->errNo = XML_ERR_UNSUPPORTED_ENCODING;
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData,
			     "Unsupported encoding %s\n", encoding);
		    return(NULL);
		}
	    }
	}
    }
    return(encoding);
}

/**
 * xmlParseSDDecl:
 * @ctxt:  an XML parser context
 *
 * parse the XML standalone declaration
 *
 * [32] SDDecl ::= S 'standalone' Eq
 *                 (("'" ('yes' | 'no') "'") | ('"' ('yes' | 'no')'"')) 
 *
 * [ VC: Standalone Document Declaration ]
 * TODO The standalone document declaration must have the value "no"
 * if any external markup declarations contain declarations of:
 *  - attributes with default values, if elements to which these
 *    attributes apply appear in the document without specifications
 *    of values for these attributes, or
 *  - entities (other than amp, lt, gt, apos, quot), if references
 *    to those entities appear in the document, or
 *  - attributes with values subject to normalization, where the
 *    attribute appears in the document with a value which will change
 *    as a result of normalization, or
 *  - element types with element content, if white space occurs directly
 *    within any instance of those types.
 *
 * Returns 1 if standalone, 0 otherwise
 */

int
xmlParseSDDecl(xmlParserCtxtPtr ctxt) {
    int standalone = -1;

    SKIP_BLANKS;
    if ((RAW == 's') && (NXT(1) == 't') &&
        (NXT(2) == 'a') && (NXT(3) == 'n') &&
	(NXT(4) == 'd') && (NXT(5) == 'a') &&
	(NXT(6) == 'l') && (NXT(7) == 'o') &&
	(NXT(8) == 'n') && (NXT(9) == 'e')) {
	SKIP(10);
        SKIP_BLANKS;
	if (RAW != '=') {
	    ctxt->errNo = XML_ERR_EQUAL_REQUIRED;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData,
		    "XML standalone declaration : expected '='\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    return(standalone);
        }
	NEXT;
	SKIP_BLANKS;
        if (RAW == '\''){
	    NEXT;
	    if ((RAW == 'n') && (NXT(1) == 'o')) {
	        standalone = 0;
                SKIP(2);
	    } else if ((RAW == 'y') && (NXT(1) == 'e') &&
	               (NXT(2) == 's')) {
	        standalone = 1;
		SKIP(3);
            } else {
		ctxt->errNo = XML_ERR_STANDALONE_VALUE;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
		                     "standalone accepts only 'yes' or 'no'\n");
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    }
	    if (RAW != '\'') {
		ctxt->errNo = XML_ERR_STRING_NOT_CLOSED;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData, "String not closed\n");
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    } else
	        NEXT;
	} else if (RAW == '"'){
	    NEXT;
	    if ((RAW == 'n') && (NXT(1) == 'o')) {
	        standalone = 0;
		SKIP(2);
	    } else if ((RAW == 'y') && (NXT(1) == 'e') &&
	               (NXT(2) == 's')) {
	        standalone = 1;
                SKIP(3);
            } else {
		ctxt->errNo = XML_ERR_STANDALONE_VALUE;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData,
		        "standalone accepts only 'yes' or 'no'\n");
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    }
	    if (RAW != '"') {
		ctxt->errNo = XML_ERR_STRING_NOT_CLOSED;
		if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		    ctxt->sax->error(ctxt->userData, "String not closed\n");
		ctxt->wellFormed = 0;
		if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	    } else
	        NEXT;
	} else {
	    ctxt->errNo = XML_ERR_STRING_NOT_STARTED;
            if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData,
		                 "Standalone value not found\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
        }
    }
    return(standalone);
}

/**
 * xmlParseXMLDecl:
 * @ctxt:  an XML parser context
 * 
 * parse an XML declaration header
 *
 * [23] XMLDecl ::= '<?xml' VersionInfo EncodingDecl? SDDecl? S? '?>'
 */

void
xmlParseXMLDecl(xmlParserCtxtPtr ctxt) {
    xmlChar *version;

    /*
     * We know that '<?xml' is here.
     */
    SKIP(5);

    if (!IS_BLANK(RAW)) {
	ctxt->errNo = XML_ERR_SPACE_REQUIRED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, "Blank needed after '<?xml'\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    }
    SKIP_BLANKS;

    /*
     * We must have the VersionInfo here.
     */
    version = xmlParseVersionInfo(ctxt);
    if (version == NULL) {
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	    "Malformed declaration expecting version\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    } else {
	if (!xmlStrEqual(version, (const xmlChar *) XML_DEFAULT_VERSION)) {
	    /*
	     * TODO: Blueberry should be detected here
	     */
	    if ((ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
		ctxt->sax->warning(ctxt->userData, "Unsupported version '%s'\n",
				   version);
	}
	if (ctxt->version != NULL)
	    xmlFree((void *) ctxt->version);
	ctxt->version = version;
    }

    /*
     * We may have the encoding declaration
     */
    if (!IS_BLANK(RAW)) {
        if ((RAW == '?') && (NXT(1) == '>')) {
	    SKIP(2);
	    return;
	}
	ctxt->errNo = XML_ERR_SPACE_REQUIRED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, "Blank needed here\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    }
    xmlParseEncodingDecl(ctxt);
    if (ctxt->errNo == XML_ERR_UNSUPPORTED_ENCODING) {
	/*
	 * The XML REC instructs us to stop parsing right here
	 */
        return;
    }

    /*
     * We may have the standalone status.
     */
    if ((ctxt->input->encoding != NULL) && (!IS_BLANK(RAW))) {
        if ((RAW == '?') && (NXT(1) == '>')) {
	    SKIP(2);
	    return;
	}
	ctxt->errNo = XML_ERR_SPACE_REQUIRED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, "Blank needed here\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    }
    SKIP_BLANKS;
    ctxt->input->standalone = xmlParseSDDecl(ctxt);

    SKIP_BLANKS;
    if ((RAW == '?') && (NXT(1) == '>')) {
        SKIP(2);
    } else if (RAW == '>') {
        /* Deprecated old WD ... */
	ctxt->errNo = XML_ERR_XMLDECL_NOT_FINISHED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, 
	                     "XML declaration must end-up with '?>'\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	NEXT;
    } else {
	ctxt->errNo = XML_ERR_XMLDECL_NOT_FINISHED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
	                     "parsing XML declaration: '?>' expected\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	MOVETO_ENDTAG(CUR_PTR);
	NEXT;
    }
}

/**
 * xmlParseMisc:
 * @ctxt:  an XML parser context
 * 
 * parse an XML Misc* optional field.
 *
 * [27] Misc ::= Comment | PI |  S
 */

void
xmlParseMisc(xmlParserCtxtPtr ctxt) {
    while (((RAW == '<') && (NXT(1) == '?')) ||
           ((RAW == '<') && (NXT(1) == '!') &&
	    (NXT(2) == '-') && (NXT(3) == '-')) ||
           IS_BLANK(CUR)) {
        if ((RAW == '<') && (NXT(1) == '?')) {
	    xmlParsePI(ctxt);
	} else if (IS_BLANK(CUR)) {
	    NEXT;
	} else
	    xmlParseComment(ctxt);
    }
}

/**
 * xmlParseDocument:
 * @ctxt:  an XML parser context
 * 
 * parse an XML document (and build a tree if using the standard SAX
 * interface).
 *
 * [1] document ::= prolog element Misc*
 *
 * [22] prolog ::= XMLDecl? Misc* (doctypedecl Misc*)?
 *
 * Returns 0, -1 in case of error. the parser context is augmented
 *                as a result of the parsing.
 */

int
xmlParseDocument(xmlParserCtxtPtr ctxt) {
    xmlChar start[4];
    xmlCharEncoding enc;

    xmlInitParser();

    GROW;

    /*
     * SAX: beginning of the document processing.
     */
    if ((ctxt->sax) && (ctxt->sax->setDocumentLocator))
        ctxt->sax->setDocumentLocator(ctxt->userData, &xmlDefaultSAXLocator);

    if (ctxt->encoding == (const xmlChar *)XML_CHAR_ENCODING_NONE) {
	/* 
	 * Get the 4 first bytes and decode the charset
	 * if enc != XML_CHAR_ENCODING_NONE
	 * plug some encoding conversion routines.
	 */
	start[0] = RAW;
	start[1] = NXT(1);
	start[2] = NXT(2);
	start[3] = NXT(3);
	enc = xmlDetectCharEncoding(start, 4);
	if (enc != XML_CHAR_ENCODING_NONE) {
	    xmlSwitchEncoding(ctxt, enc);
	}
    }


    if (CUR == 0) {
	ctxt->errNo = XML_ERR_DOCUMENT_EMPTY;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, "Document is empty\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    }

    /*
     * Check for the XMLDecl in the Prolog.
     */
    GROW;
    if ((RAW == '<') && (NXT(1) == '?') &&
        (NXT(2) == 'x') && (NXT(3) == 'm') &&
	(NXT(4) == 'l') && (IS_BLANK(NXT(5)))) {

	/*
	 * Note that we will switch encoding on the fly.
	 */
	xmlParseXMLDecl(ctxt);
	if (ctxt->errNo == XML_ERR_UNSUPPORTED_ENCODING) {
	    /*
	     * The XML REC instructs us to stop parsing right here
	     */
	    return(-1);
	}
	ctxt->standalone = ctxt->input->standalone;
	SKIP_BLANKS;
    } else {
	ctxt->version = xmlCharStrdup(XML_DEFAULT_VERSION);
    }
    if ((ctxt->sax) && (ctxt->sax->startDocument) && (!ctxt->disableSAX))
        ctxt->sax->startDocument(ctxt->userData);

    /*
     * The Misc part of the Prolog
     */
    GROW;
    xmlParseMisc(ctxt);

    /*
     * Then possibly doc type declaration(s) and more Misc
     * (doctypedecl Misc*)?
     */
    GROW;
    if ((RAW == '<') && (NXT(1) == '!') &&
	(NXT(2) == 'D') && (NXT(3) == 'O') &&
	(NXT(4) == 'C') && (NXT(5) == 'T') &&
	(NXT(6) == 'Y') && (NXT(7) == 'P') &&
	(NXT(8) == 'E')) {

	ctxt->inSubset = 1;
	xmlParseDocTypeDecl(ctxt);
	if (RAW == '[') {
	    ctxt->instate = XML_PARSER_DTD;
	    xmlParseInternalSubset(ctxt);
	}

	/*
	 * Create and update the external subset.
	 */
	ctxt->inSubset = 2;
	if ((ctxt->sax != NULL) && (ctxt->sax->externalSubset != NULL) &&
	    (!ctxt->disableSAX))
	    ctxt->sax->externalSubset(ctxt->userData, ctxt->intSubName,
	                              ctxt->extSubSystem, ctxt->extSubURI);
	ctxt->inSubset = 0;


	ctxt->instate = XML_PARSER_PROLOG;
	xmlParseMisc(ctxt);
    }

    /*
     * Time to start parsing the tree itself
     */
    GROW;
    if (RAW != '<') {
	ctxt->errNo = XML_ERR_DOCUMENT_EMPTY;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
		    "Start tag expected, '<' not found\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	ctxt->instate = XML_PARSER_EOF;
    } else {
	ctxt->instate = XML_PARSER_CONTENT;
	xmlParseElement(ctxt);
	ctxt->instate = XML_PARSER_EPILOG;


	/*
	 * The Misc part at the end
	 */
	xmlParseMisc(ctxt);

	if (RAW != 0) {
	    ctxt->errNo = XML_ERR_DOCUMENT_END;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData,
		    "Extra content at the end of the document\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	}
	ctxt->instate = XML_PARSER_EOF;
    }

    /*
     * SAX: end of the document processing.
     */
    if ((ctxt->sax) && (ctxt->sax->endDocument != NULL))
        ctxt->sax->endDocument(ctxt->userData);

    /*
     * Remove locally kept entity definitions if the tree was not built
     */
    if ((ctxt->myDoc != NULL) &&
	(xmlStrEqual(ctxt->myDoc->version, SAX_COMPAT_MODE))) {
	xmlFreeDoc(ctxt->myDoc);
	ctxt->myDoc = NULL;
    }

    if (! ctxt->wellFormed) {
	ctxt->valid = 0;
	return(-1);
    }
    return(0);
}

/**
 * xmlParseExtParsedEnt:
 * @ctxt:  an XML parser context
 * 
 * parse a general parsed entity
 * An external general parsed entity is well-formed if it matches the
 * production labeled extParsedEnt.
 *
 * [78] extParsedEnt ::= TextDecl? content
 *
 * Returns 0, -1 in case of error. the parser context is augmented
 *                as a result of the parsing.
 */

int
xmlParseExtParsedEnt(xmlParserCtxtPtr ctxt) {
    xmlChar start[4];
    xmlCharEncoding enc;

    xmlDefaultSAXHandlerInit();

    GROW;

    /*
     * SAX: beginning of the document processing.
     */
    if ((ctxt->sax) && (ctxt->sax->setDocumentLocator))
        ctxt->sax->setDocumentLocator(ctxt->userData, &xmlDefaultSAXLocator);

    /* 
     * Get the 4 first bytes and decode the charset
     * if enc != XML_CHAR_ENCODING_NONE
     * plug some encoding conversion routines.
     */
    start[0] = RAW;
    start[1] = NXT(1);
    start[2] = NXT(2);
    start[3] = NXT(3);
    enc = xmlDetectCharEncoding(start, 4);
    if (enc != XML_CHAR_ENCODING_NONE) {
        xmlSwitchEncoding(ctxt, enc);
    }


    if (CUR == 0) {
	ctxt->errNo = XML_ERR_DOCUMENT_EMPTY;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, "Document is empty\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    }

    /*
     * Check for the XMLDecl in the Prolog.
     */
    GROW;
    if ((RAW == '<') && (NXT(1) == '?') &&
        (NXT(2) == 'x') && (NXT(3) == 'm') &&
	(NXT(4) == 'l') && (IS_BLANK(NXT(5)))) {

	/*
	 * Note that we will switch encoding on the fly.
	 */
	xmlParseXMLDecl(ctxt);
	if (ctxt->errNo == XML_ERR_UNSUPPORTED_ENCODING) {
	    /*
	     * The XML REC instructs us to stop parsing right here
	     */
	    return(-1);
	}
	SKIP_BLANKS;
    } else {
	ctxt->version = xmlCharStrdup(XML_DEFAULT_VERSION);
    }
    if ((ctxt->sax) && (ctxt->sax->startDocument) && (!ctxt->disableSAX))
        ctxt->sax->startDocument(ctxt->userData);

    /*
     * Doing validity checking on chunk doesn't make sense
     */
    ctxt->instate = XML_PARSER_CONTENT;
    ctxt->validate = 0;
    ctxt->loadsubset = 0;
    ctxt->depth = 0;

    xmlParseContent(ctxt);
   
    if ((RAW == '<') && (NXT(1) == '/')) {
	ctxt->errNo = XML_ERR_NOT_WELL_BALANCED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
		"chunk is not well balanced\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    } else if (RAW != 0) {
	ctxt->errNo = XML_ERR_EXTRA_CONTENT;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
		"extra content at the end of well balanced chunk\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    }

    /*
     * SAX: end of the document processing.
     */
    if ((ctxt->sax) && (ctxt->sax->endDocument != NULL))
        ctxt->sax->endDocument(ctxt->userData);

    if (! ctxt->wellFormed) return(-1);
    return(0);
}

/************************************************************************
 *									*
 * 		Progressive parsing interfaces				*
 *									*
 ************************************************************************/

/**
 * xmlParseLookupSequence:
 * @ctxt:  an XML parser context
 * @first:  the first char to lookup
 * @next:  the next char to lookup or zero
 * @third:  the next char to lookup or zero
 *
 * Try to find if a sequence (first, next, third) or  just (first next) or
 * (first) is available in the input stream.
 * This function has a side effect of (possibly) incrementing ctxt->checkIndex
 * to avoid rescanning sequences of bytes, it DOES change the state of the
 * parser, do not use liberally.
 *
 * Returns the index to the current parsing point if the full sequence
 *      is available, -1 otherwise.
 */
static int
xmlParseLookupSequence(xmlParserCtxtPtr ctxt, xmlChar first,
                       xmlChar next, xmlChar third) {
    int base, len;
    xmlParserInputPtr in;
    const xmlChar *buf;

    in = ctxt->input;
    if (in == NULL) return(-1);
    base = in->cur - in->base;
    if (base < 0) return(-1);
    if (ctxt->checkIndex > base)
        base = ctxt->checkIndex;
    if (in->buf == NULL) {
	buf = in->base;
	len = in->length;
    } else {
	buf = in->buf->buffer->content;
	len = in->buf->buffer->use;
    }
    /* take into account the sequence length */
    if (third) len -= 2;
    else if (next) len --;
    for (;base < len;base++) {
        if (buf[base] == first) {
	    if (third != 0) {
		if ((buf[base + 1] != next) ||
		    (buf[base + 2] != third)) continue;
	    } else if (next != 0) {
		if (buf[base + 1] != next) continue;
	    }
	    ctxt->checkIndex = 0;
#ifdef DEBUG_PUSH
	    if (next == 0)
		xmlGenericError(xmlGenericErrorContext,
			"PP: lookup '%c' found at %d\n",
			first, base);
	    else if (third == 0)
		xmlGenericError(xmlGenericErrorContext,
			"PP: lookup '%c%c' found at %d\n",
			first, next, base);
	    else 
		xmlGenericError(xmlGenericErrorContext,
			"PP: lookup '%c%c%c' found at %d\n",
			first, next, third, base);
#endif
	    return(base - (in->cur - in->base));
	}
    }
    ctxt->checkIndex = base;
#ifdef DEBUG_PUSH
    if (next == 0)
	xmlGenericError(xmlGenericErrorContext,
		"PP: lookup '%c' failed\n", first);
    else if (third == 0)
	xmlGenericError(xmlGenericErrorContext,
		"PP: lookup '%c%c' failed\n", first, next);
    else	
	xmlGenericError(xmlGenericErrorContext,
		"PP: lookup '%c%c%c' failed\n", first, next, third);
#endif
    return(-1);
}

/**
 * xmlParseTryOrFinish:
 * @ctxt:  an XML parser context
 * @terminate:  last chunk indicator
 *
 * Try to progress on parsing
 *
 * Returns zero if no parsing was possible
 */
static int
xmlParseTryOrFinish(xmlParserCtxtPtr ctxt, int terminate) {
    int ret = 0;
    int avail;
    xmlChar cur, next;

#ifdef DEBUG_PUSH
    switch (ctxt->instate) {
	case XML_PARSER_EOF:
	    xmlGenericError(xmlGenericErrorContext,
		    "PP: try EOF\n"); break;
	case XML_PARSER_START:
	    xmlGenericError(xmlGenericErrorContext,
		    "PP: try START\n"); break;
	case XML_PARSER_MISC:
	    xmlGenericError(xmlGenericErrorContext,
		    "PP: try MISC\n");break;
	case XML_PARSER_COMMENT:
	    xmlGenericError(xmlGenericErrorContext,
		    "PP: try COMMENT\n");break;
	case XML_PARSER_PROLOG:
	    xmlGenericError(xmlGenericErrorContext,
		    "PP: try PROLOG\n");break;
	case XML_PARSER_START_TAG:
	    xmlGenericError(xmlGenericErrorContext,
		    "PP: try START_TAG\n");break;
	case XML_PARSER_CONTENT:
	    xmlGenericError(xmlGenericErrorContext,
		    "PP: try CONTENT\n");break;
	case XML_PARSER_CDATA_SECTION:
	    xmlGenericError(xmlGenericErrorContext,
		    "PP: try CDATA_SECTION\n");break;
	case XML_PARSER_END_TAG:
	    xmlGenericError(xmlGenericErrorContext,
		    "PP: try END_TAG\n");break;
	case XML_PARSER_ENTITY_DECL:
	    xmlGenericError(xmlGenericErrorContext,
		    "PP: try ENTITY_DECL\n");break;
	case XML_PARSER_ENTITY_VALUE:
	    xmlGenericError(xmlGenericErrorContext,
		    "PP: try ENTITY_VALUE\n");break;
	case XML_PARSER_ATTRIBUTE_VALUE:
	    xmlGenericError(xmlGenericErrorContext,
		    "PP: try ATTRIBUTE_VALUE\n");break;
	case XML_PARSER_DTD:
	    xmlGenericError(xmlGenericErrorContext,
		    "PP: try DTD\n");break;
	case XML_PARSER_EPILOG:
	    xmlGenericError(xmlGenericErrorContext,
		    "PP: try EPILOG\n");break;
	case XML_PARSER_PI:
	    xmlGenericError(xmlGenericErrorContext,
		    "PP: try PI\n");break;
        case XML_PARSER_IGNORE:
            xmlGenericError(xmlGenericErrorContext,
		    "PP: try IGNORE\n");break;
    }
#endif

    while (1) {
	SHRINK;

	/*
	 * Pop-up of finished entities.
	 */
	while ((RAW == 0) && (ctxt->inputNr > 1))
	    xmlPopInput(ctxt);

	if (ctxt->input ==NULL) break;
	if (ctxt->input->buf == NULL)
	    avail = ctxt->input->length - (ctxt->input->cur - ctxt->input->base);
	else {
	    /*
	     * If we are operating on converted input, try to flush
	     * remainng chars to avoid them stalling in the non-converted
	     * buffer.
	     */
	    if ((ctxt->input->buf->raw != NULL) &&
		(ctxt->input->buf->raw->use > 0)) {
		int base = ctxt->input->base -
		           ctxt->input->buf->buffer->content;
		int current = ctxt->input->cur - ctxt->input->base;

		xmlParserInputBufferPush(ctxt->input->buf, 0, "");
		ctxt->input->base = ctxt->input->buf->buffer->content + base;
		ctxt->input->cur = ctxt->input->base + current;
		ctxt->input->end =
		    &ctxt->input->buf->buffer->content[
		                       ctxt->input->buf->buffer->use];
	    }
	    avail = ctxt->input->buf->buffer->use -
		    (ctxt->input->cur - ctxt->input->base);
	}
        if (avail < 1)
	    goto done;
        switch (ctxt->instate) {
            case XML_PARSER_EOF:
	        /*
		 * Document parsing is done !
		 */
	        goto done;
            case XML_PARSER_START:
		if (ctxt->charset == XML_CHAR_ENCODING_NONE) {
		    xmlChar start[4];
		    xmlCharEncoding enc;

		    /*
		     * Very first chars read from the document flow.
		     */
		    if (avail < 4)
			goto done;

		    /* 
		     * Get the 4 first bytes and decode the charset
		     * if enc != XML_CHAR_ENCODING_NONE
		     * plug some encoding conversion routines.
		     */
		    start[0] = RAW;
		    start[1] = NXT(1);
		    start[2] = NXT(2);
		    start[3] = NXT(3);
		    enc = xmlDetectCharEncoding(start, 4);
		    if (enc != XML_CHAR_ENCODING_NONE) {
			xmlSwitchEncoding(ctxt, enc);
		    }
		    break;
		}

		cur = ctxt->input->cur[0];
		next = ctxt->input->cur[1];
		if (cur == 0) {
		    if ((ctxt->sax) && (ctxt->sax->setDocumentLocator))
			ctxt->sax->setDocumentLocator(ctxt->userData,
						      &xmlDefaultSAXLocator);
		    ctxt->errNo = XML_ERR_DOCUMENT_EMPTY;
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData, "Document is empty\n");
		    ctxt->wellFormed = 0;
		    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		    ctxt->instate = XML_PARSER_EOF;
#ifdef DEBUG_PUSH
		    xmlGenericError(xmlGenericErrorContext,
			    "PP: entering EOF\n");
#endif
		    if ((ctxt->sax) && (ctxt->sax->endDocument != NULL))
			ctxt->sax->endDocument(ctxt->userData);
		    goto done;
		}
	        if ((cur == '<') && (next == '?')) {
		    /* PI or XML decl */
		    if (avail < 5) return(ret);
		    if ((!terminate) &&
		        (xmlParseLookupSequence(ctxt, '?', '>', 0) < 0))
			return(ret);
		    if ((ctxt->sax) && (ctxt->sax->setDocumentLocator))
			ctxt->sax->setDocumentLocator(ctxt->userData,
						      &xmlDefaultSAXLocator);
		    if ((ctxt->input->cur[2] == 'x') &&
			(ctxt->input->cur[3] == 'm') &&
			(ctxt->input->cur[4] == 'l') &&
			(IS_BLANK(ctxt->input->cur[5]))) {
			ret += 5;
#ifdef DEBUG_PUSH
			xmlGenericError(xmlGenericErrorContext,
				"PP: Parsing XML Decl\n");
#endif
			xmlParseXMLDecl(ctxt);
			if (ctxt->errNo == XML_ERR_UNSUPPORTED_ENCODING) {
			    /*
			     * The XML REC instructs us to stop parsing right
			     * here
			     */
			    ctxt->instate = XML_PARSER_EOF;
			    return(0);
			}
			ctxt->standalone = ctxt->input->standalone;
			if ((ctxt->encoding == NULL) &&
			    (ctxt->input->encoding != NULL))
			    ctxt->encoding = xmlStrdup(ctxt->input->encoding);
			if ((ctxt->sax) && (ctxt->sax->startDocument) &&
			    (!ctxt->disableSAX))
			    ctxt->sax->startDocument(ctxt->userData);
			ctxt->instate = XML_PARSER_MISC;
#ifdef DEBUG_PUSH
			xmlGenericError(xmlGenericErrorContext,
				"PP: entering MISC\n");
#endif
		    } else {
			ctxt->version = xmlCharStrdup(XML_DEFAULT_VERSION);
			if ((ctxt->sax) && (ctxt->sax->startDocument) &&
			    (!ctxt->disableSAX))
			    ctxt->sax->startDocument(ctxt->userData);
			ctxt->instate = XML_PARSER_MISC;
#ifdef DEBUG_PUSH
			xmlGenericError(xmlGenericErrorContext,
				"PP: entering MISC\n");
#endif
		    }
		} else {
		    if ((ctxt->sax) && (ctxt->sax->setDocumentLocator))
			ctxt->sax->setDocumentLocator(ctxt->userData,
						      &xmlDefaultSAXLocator);
		    ctxt->version = xmlCharStrdup(XML_DEFAULT_VERSION);
		    if ((ctxt->sax) && (ctxt->sax->startDocument) &&
		        (!ctxt->disableSAX))
			ctxt->sax->startDocument(ctxt->userData);
		    ctxt->instate = XML_PARSER_MISC;
#ifdef DEBUG_PUSH
		    xmlGenericError(xmlGenericErrorContext,
			    "PP: entering MISC\n");
#endif
		}
		break;
            case XML_PARSER_MISC:
		SKIP_BLANKS;
		if (ctxt->input->buf == NULL)
		    avail = ctxt->input->length - (ctxt->input->cur - ctxt->input->base);
		else
		    avail = ctxt->input->buf->buffer->use - (ctxt->input->cur - ctxt->input->base);
		if (avail < 2)
		    goto done;
		cur = ctxt->input->cur[0];
		next = ctxt->input->cur[1];
	        if ((cur == '<') && (next == '?')) {
		    if ((!terminate) &&
		        (xmlParseLookupSequence(ctxt, '?', '>', 0) < 0))
			goto done;
#ifdef DEBUG_PUSH
		    xmlGenericError(xmlGenericErrorContext,
			    "PP: Parsing PI\n");
#endif
		    xmlParsePI(ctxt);
		} else if ((cur == '<') && (next == '!') &&
		    (ctxt->input->cur[2] == '-') && (ctxt->input->cur[3] == '-')) {
		    if ((!terminate) &&
		        (xmlParseLookupSequence(ctxt, '-', '-', '>') < 0))
			goto done;
#ifdef DEBUG_PUSH
		    xmlGenericError(xmlGenericErrorContext,
			    "PP: Parsing Comment\n");
#endif
		    xmlParseComment(ctxt);
		    ctxt->instate = XML_PARSER_MISC;
		} else if ((cur == '<') && (next == '!') &&
		    (ctxt->input->cur[2] == 'D') && (ctxt->input->cur[3] == 'O') &&
		    (ctxt->input->cur[4] == 'C') && (ctxt->input->cur[5] == 'T') &&
		    (ctxt->input->cur[6] == 'Y') && (ctxt->input->cur[7] == 'P') &&
		    (ctxt->input->cur[8] == 'E')) {
		    if ((!terminate) &&
		        (xmlParseLookupSequence(ctxt, '>', 0, 0) < 0))
			goto done;
#ifdef DEBUG_PUSH
		    xmlGenericError(xmlGenericErrorContext,
			    "PP: Parsing internal subset\n");
#endif
		    ctxt->inSubset = 1;
		    xmlParseDocTypeDecl(ctxt);
		    if (RAW == '[') {
			ctxt->instate = XML_PARSER_DTD;
#ifdef DEBUG_PUSH
			xmlGenericError(xmlGenericErrorContext,
				"PP: entering DTD\n");
#endif
		    } else {
			/*
			 * Create and update the external subset.
			 */
			ctxt->inSubset = 2;
			if ((ctxt->sax != NULL) && (!ctxt->disableSAX) &&
			    (ctxt->sax->externalSubset != NULL))
			    ctxt->sax->externalSubset(ctxt->userData,
				    ctxt->intSubName, ctxt->extSubSystem,
				    ctxt->extSubURI);
			ctxt->inSubset = 0;
			ctxt->instate = XML_PARSER_PROLOG;
#ifdef DEBUG_PUSH
			xmlGenericError(xmlGenericErrorContext,
				"PP: entering PROLOG\n");
#endif
		    }
		} else if ((cur == '<') && (next == '!') &&
		           (avail < 9)) {
		    goto done;
		} else {
		    ctxt->instate = XML_PARSER_START_TAG;
#ifdef DEBUG_PUSH
		    xmlGenericError(xmlGenericErrorContext,
			    "PP: entering START_TAG\n");
#endif
		}
		break;
            case XML_PARSER_IGNORE:
		xmlGenericError(xmlGenericErrorContext,
			"PP: internal error, state == IGNORE");
	        ctxt->instate = XML_PARSER_DTD;
#ifdef DEBUG_PUSH
		xmlGenericError(xmlGenericErrorContext,
			"PP: entering DTD\n");
#endif
	        break;
            case XML_PARSER_PROLOG:
		SKIP_BLANKS;
		if (ctxt->input->buf == NULL)
		    avail = ctxt->input->length - (ctxt->input->cur - ctxt->input->base);
		else
		    avail = ctxt->input->buf->buffer->use - (ctxt->input->cur - ctxt->input->base);
		if (avail < 2) 
		    goto done;
		cur = ctxt->input->cur[0];
		next = ctxt->input->cur[1];
	        if ((cur == '<') && (next == '?')) {
		    if ((!terminate) &&
		        (xmlParseLookupSequence(ctxt, '?', '>', 0) < 0))
			goto done;
#ifdef DEBUG_PUSH
		    xmlGenericError(xmlGenericErrorContext,
			    "PP: Parsing PI\n");
#endif
		    xmlParsePI(ctxt);
		} else if ((cur == '<') && (next == '!') &&
		    (ctxt->input->cur[2] == '-') && (ctxt->input->cur[3] == '-')) {
		    if ((!terminate) &&
		        (xmlParseLookupSequence(ctxt, '-', '-', '>') < 0))
			goto done;
#ifdef DEBUG_PUSH
		    xmlGenericError(xmlGenericErrorContext,
			    "PP: Parsing Comment\n");
#endif
		    xmlParseComment(ctxt);
		    ctxt->instate = XML_PARSER_PROLOG;
		} else if ((cur == '<') && (next == '!') &&
		           (avail < 4)) {
		    goto done;
		} else {
		    ctxt->instate = XML_PARSER_START_TAG;
#ifdef DEBUG_PUSH
		    xmlGenericError(xmlGenericErrorContext,
			    "PP: entering START_TAG\n");
#endif
		}
		break;
            case XML_PARSER_EPILOG:
		SKIP_BLANKS;
		if (ctxt->input->buf == NULL)
		    avail = ctxt->input->length - (ctxt->input->cur - ctxt->input->base);
		else
		    avail = ctxt->input->buf->buffer->use - (ctxt->input->cur - ctxt->input->base);
		if (avail < 2)
		    goto done;
		cur = ctxt->input->cur[0];
		next = ctxt->input->cur[1];
	        if ((cur == '<') && (next == '?')) {
		    if ((!terminate) &&
		        (xmlParseLookupSequence(ctxt, '?', '>', 0) < 0))
			goto done;
#ifdef DEBUG_PUSH
		    xmlGenericError(xmlGenericErrorContext,
			    "PP: Parsing PI\n");
#endif
		    xmlParsePI(ctxt);
		    ctxt->instate = XML_PARSER_EPILOG;
		} else if ((cur == '<') && (next == '!') &&
		    (ctxt->input->cur[2] == '-') && (ctxt->input->cur[3] == '-')) {
		    if ((!terminate) &&
		        (xmlParseLookupSequence(ctxt, '-', '-', '>') < 0))
			goto done;
#ifdef DEBUG_PUSH
		    xmlGenericError(xmlGenericErrorContext,
			    "PP: Parsing Comment\n");
#endif
		    xmlParseComment(ctxt);
		    ctxt->instate = XML_PARSER_EPILOG;
		} else if ((cur == '<') && (next == '!') &&
		           (avail < 4)) {
		    goto done;
		} else {
		    ctxt->errNo = XML_ERR_DOCUMENT_END;
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData,
			    "Extra content at the end of the document\n");
		    ctxt->wellFormed = 0;
		    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		    ctxt->instate = XML_PARSER_EOF;
#ifdef DEBUG_PUSH
		    xmlGenericError(xmlGenericErrorContext,
			    "PP: entering EOF\n");
#endif
		    if ((ctxt->sax) && (ctxt->sax->endDocument != NULL))
			ctxt->sax->endDocument(ctxt->userData);
		    goto done;
		}
		break;
            case XML_PARSER_START_TAG: {
	        xmlChar *name, *oldname;

		if ((avail < 2) && (ctxt->inputNr == 1))
		    goto done;
		cur = ctxt->input->cur[0];
	        if (cur != '<') {
		    ctxt->errNo = XML_ERR_DOCUMENT_EMPTY;
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData,
				"Start tag expect, '<' not found\n");
		    ctxt->wellFormed = 0;
		    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		    ctxt->instate = XML_PARSER_EOF;
#ifdef DEBUG_PUSH
		    xmlGenericError(xmlGenericErrorContext,
			    "PP: entering EOF\n");
#endif
		    if ((ctxt->sax) && (ctxt->sax->endDocument != NULL))
			ctxt->sax->endDocument(ctxt->userData);
		    goto done;
		}
		if ((!terminate) &&
		    (xmlParseLookupSequence(ctxt, '>', 0, 0) < 0))
		    goto done;
		if (ctxt->spaceNr == 0)
		    spacePush(ctxt, -1);
		else
		    spacePush(ctxt, *ctxt->space);
		name = xmlParseStartTag(ctxt);
		if (name == NULL) {
		    spacePop(ctxt);
		    ctxt->instate = XML_PARSER_EOF;
#ifdef DEBUG_PUSH
		    xmlGenericError(xmlGenericErrorContext,
			    "PP: entering EOF\n");
#endif
		    if ((ctxt->sax) && (ctxt->sax->endDocument != NULL))
			ctxt->sax->endDocument(ctxt->userData);
		    goto done;
		}
		namePush(ctxt, xmlStrdup(name));

		/*
		 * [ VC: Root Element Type ]
		 * The Name in the document type declaration must match
		 * the element type of the root element. 
		 */
		if (ctxt->validate && ctxt->wellFormed && ctxt->myDoc &&
		    ctxt->node && (ctxt->node == ctxt->myDoc->children))
		    ctxt->valid &= xmlValidateRoot(&ctxt->vctxt, ctxt->myDoc);

		/*
		 * Check for an Empty Element.
		 */
		if ((RAW == '/') && (NXT(1) == '>')) {
		    SKIP(2);
		    if ((ctxt->sax != NULL) &&
			(ctxt->sax->endElement != NULL) && (!ctxt->disableSAX))
			ctxt->sax->endElement(ctxt->userData, name);
		    xmlFree(name);
		    oldname = namePop(ctxt);
		    spacePop(ctxt);
		    if (oldname != NULL) {
#ifdef DEBUG_STACK
			xmlGenericError(xmlGenericErrorContext,"Close: popped %s\n", oldname);
#endif
			xmlFree(oldname);
		    }
		    if (ctxt->name == NULL) {
			ctxt->instate = XML_PARSER_EPILOG;
#ifdef DEBUG_PUSH
			xmlGenericError(xmlGenericErrorContext,
				"PP: entering EPILOG\n");
#endif
		    } else {
			ctxt->instate = XML_PARSER_CONTENT;
#ifdef DEBUG_PUSH
			xmlGenericError(xmlGenericErrorContext,
				"PP: entering CONTENT\n");
#endif
		    }
		    break;
		}
		if (RAW == '>') {
		    NEXT;
		} else {
		    ctxt->errNo = XML_ERR_GT_REQUIRED;
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData,
					 "Couldn't find end of Start Tag %s\n",
					 name);
		    ctxt->wellFormed = 0;
		    if (ctxt->recovery == 0) ctxt->disableSAX = 1;

		    /*
		     * end of parsing of this node.
		     */
		    nodePop(ctxt);
		    oldname = namePop(ctxt);
		    spacePop(ctxt);
		    if (oldname != NULL) {
#ifdef DEBUG_STACK
			xmlGenericError(xmlGenericErrorContext,"Close: popped %s\n", oldname);
#endif
			xmlFree(oldname);
		    }
		}
		xmlFree(name);
		ctxt->instate = XML_PARSER_CONTENT;
#ifdef DEBUG_PUSH
		xmlGenericError(xmlGenericErrorContext,
			"PP: entering CONTENT\n");
#endif
                break;
	    }
            case XML_PARSER_CONTENT: {
		const xmlChar *test;
		int cons;
		if ((avail < 2) && (ctxt->inputNr == 1))
		    goto done;
		cur = ctxt->input->cur[0];
		next = ctxt->input->cur[1];

		test = CUR_PTR;
	        cons = ctxt->input->consumed;
	        if ((cur == '<') && (next == '?')) {
		    if ((!terminate) &&
		        (xmlParseLookupSequence(ctxt, '?', '>', 0) < 0))
			goto done;
#ifdef DEBUG_PUSH
		    xmlGenericError(xmlGenericErrorContext,
			    "PP: Parsing PI\n");
#endif
		    xmlParsePI(ctxt);
		} else if ((cur == '<') && (next == '!') &&
		           (ctxt->input->cur[2] == '-') && (ctxt->input->cur[3] == '-')) {
		    if ((!terminate) &&
		        (xmlParseLookupSequence(ctxt, '-', '-', '>') < 0))
			goto done;
#ifdef DEBUG_PUSH
		    xmlGenericError(xmlGenericErrorContext,
			    "PP: Parsing Comment\n");
#endif
		    xmlParseComment(ctxt);
		    ctxt->instate = XML_PARSER_CONTENT;
		} else if ((cur == '<') && (ctxt->input->cur[1] == '!') &&
		    (ctxt->input->cur[2] == '[') && (NXT(3) == 'C') &&
		    (ctxt->input->cur[4] == 'D') && (NXT(5) == 'A') &&
		    (ctxt->input->cur[6] == 'T') && (NXT(7) == 'A') &&
		    (ctxt->input->cur[8] == '[')) {
		    SKIP(9);
		    ctxt->instate = XML_PARSER_CDATA_SECTION;
#ifdef DEBUG_PUSH
		    xmlGenericError(xmlGenericErrorContext,
			    "PP: entering CDATA_SECTION\n");
#endif
		    break;
		} else if ((cur == '<') && (next == '!') &&
		           (avail < 9)) {
		    goto done;
		} else if ((cur == '<') && (next == '/')) {
		    ctxt->instate = XML_PARSER_END_TAG;
#ifdef DEBUG_PUSH
		    xmlGenericError(xmlGenericErrorContext,
			    "PP: entering END_TAG\n");
#endif
		    break;
		} else if (cur == '<') {
		    ctxt->instate = XML_PARSER_START_TAG;
#ifdef DEBUG_PUSH
		    xmlGenericError(xmlGenericErrorContext,
			    "PP: entering START_TAG\n");
#endif
		    break;
		} else if (cur == '&') {
		    if ((!terminate) &&
		        (xmlParseLookupSequence(ctxt, ';', 0, 0) < 0))
			goto done;
#ifdef DEBUG_PUSH
		    xmlGenericError(xmlGenericErrorContext,
			    "PP: Parsing Reference\n");
#endif
		    xmlParseReference(ctxt);
		} else {
		    /* TODO Avoid the extra copy, handle directly !!! */
		    /*
		     * Goal of the following test is:
		     *  - minimize calls to the SAX 'character' callback
		     *    when they are mergeable
		     *  - handle an problem for isBlank when we only parse
		     *    a sequence of blank chars and the next one is
		     *    not available to check against '<' presence.
		     *  - tries to homogenize the differences in SAX
		     *    callbacks between the push and pull versions
		     *    of the parser.
		     */
		    if ((ctxt->inputNr == 1) &&
		        (avail < XML_PARSER_BIG_BUFFER_SIZE)) {
			if ((!terminate) &&
			    (xmlParseLookupSequence(ctxt, '<', 0, 0) < 0))
			    goto done;
                    }
		    ctxt->checkIndex = 0;
#ifdef DEBUG_PUSH
		    xmlGenericError(xmlGenericErrorContext,
			    "PP: Parsing char data\n");
#endif
		    xmlParseCharData(ctxt, 0);
		}
		/*
		 * Pop-up of finished entities.
		 */
		while ((RAW == 0) && (ctxt->inputNr > 1))
		    xmlPopInput(ctxt);
		if ((cons == ctxt->input->consumed) && (test == CUR_PTR)) {
		    ctxt->errNo = XML_ERR_INTERNAL_ERROR;
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData,
			     "detected an error in element content\n");
		    ctxt->wellFormed = 0;
		    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		    ctxt->instate = XML_PARSER_EOF;
		    break;
		}
		break;
	    }
            case XML_PARSER_CDATA_SECTION: {
	        /*
		 * The Push mode need to have the SAX callback for 
		 * cdataBlock merge back contiguous callbacks.
		 */
		int base;

		base = xmlParseLookupSequence(ctxt, ']', ']', '>');
		if (base < 0) {
		    if (avail >= XML_PARSER_BIG_BUFFER_SIZE + 2) {
			if ((ctxt->sax != NULL) && (!ctxt->disableSAX)) {
			    if (ctxt->sax->cdataBlock != NULL)
				ctxt->sax->cdataBlock(ctxt->userData, ctxt->input->cur,
					  XML_PARSER_BIG_BUFFER_SIZE);
			}
			SKIP(XML_PARSER_BIG_BUFFER_SIZE);
			ctxt->checkIndex = 0;
		    }
		    goto done;
		} else {
		    if ((ctxt->sax != NULL) && (base > 0) &&
			(!ctxt->disableSAX)) {
			if (ctxt->sax->cdataBlock != NULL)
			    ctxt->sax->cdataBlock(ctxt->userData,
						  ctxt->input->cur, base);
		    }
		    SKIP(base + 3);
		    ctxt->checkIndex = 0;
		    ctxt->instate = XML_PARSER_CONTENT;
#ifdef DEBUG_PUSH
		    xmlGenericError(xmlGenericErrorContext,
			    "PP: entering CONTENT\n");
#endif
		}
		break;
	    }
            case XML_PARSER_END_TAG:
		if (avail < 2)
		    goto done;
		if ((!terminate) &&
		    (xmlParseLookupSequence(ctxt, '>', 0, 0) < 0))
		    goto done;
		xmlParseEndTag(ctxt);
		if (ctxt->name == NULL) {
		    ctxt->instate = XML_PARSER_EPILOG;
#ifdef DEBUG_PUSH
		    xmlGenericError(xmlGenericErrorContext,
			    "PP: entering EPILOG\n");
#endif
		} else {
		    ctxt->instate = XML_PARSER_CONTENT;
#ifdef DEBUG_PUSH
		    xmlGenericError(xmlGenericErrorContext,
			    "PP: entering CONTENT\n");
#endif
		}
		break;
            case XML_PARSER_DTD: {
	        /*
		 * Sorry but progressive parsing of the internal subset
		 * is not expected to be supported. We first check that
		 * the full content of the internal subset is available and
		 * the parsing is launched only at that point.
		 * Internal subset ends up with "']' S? '>'" in an unescaped
		 * section and not in a ']]>' sequence which are conditional
		 * sections (whoever argued to keep that crap in XML deserve
		 * a place in hell !).
		 */
		int base, i;
		xmlChar *buf;
	        xmlChar quote = 0;

		base = ctxt->input->cur - ctxt->input->base;
		if (base < 0) return(0);
		if (ctxt->checkIndex > base)
		    base = ctxt->checkIndex;
		buf = ctxt->input->buf->buffer->content;
		for (;(unsigned int) base < ctxt->input->buf->buffer->use;
		     base++) {
		    if (quote != 0) {
		        if (buf[base] == quote)
			    quote = 0;
			continue;    
		    }
		    if (buf[base] == '"') {
		        quote = '"';
			continue;
		    }
		    if (buf[base] == '\'') {
		        quote = '\'';
			continue;
		    }
		    if (buf[base] == ']') {
		        if ((unsigned int) base +1 >=
		            ctxt->input->buf->buffer->use)
			    break;
			if (buf[base + 1] == ']') {
			    /* conditional crap, skip both ']' ! */
			    base++;
			    continue;
			}
		        for (i = 0;
		     (unsigned int) base + i < ctxt->input->buf->buffer->use;
		             i++) {
			    if (buf[base + i] == '>')
			        goto found_end_int_subset;
			}
		        break;
		    }
		}
		/*
		 * We didn't found the end of the Internal subset
		 */
		if (quote == 0) 
		    ctxt->checkIndex = base;
#ifdef DEBUG_PUSH
		if (next == 0)
		    xmlGenericError(xmlGenericErrorContext,
			    "PP: lookup of int subset end filed\n");
#endif
	        goto done;

found_end_int_subset:
		xmlParseInternalSubset(ctxt);
		ctxt->inSubset = 2;
		if ((ctxt->sax != NULL) && (!ctxt->disableSAX) &&
		    (ctxt->sax->externalSubset != NULL))
		    ctxt->sax->externalSubset(ctxt->userData, ctxt->intSubName,
			    ctxt->extSubSystem, ctxt->extSubURI);
		ctxt->inSubset = 0;
		ctxt->instate = XML_PARSER_PROLOG;
		ctxt->checkIndex = 0;
#ifdef DEBUG_PUSH
		xmlGenericError(xmlGenericErrorContext,
			"PP: entering PROLOG\n");
#endif
                break;
	    }
            case XML_PARSER_COMMENT:
		xmlGenericError(xmlGenericErrorContext,
			"PP: internal error, state == COMMENT\n");
		ctxt->instate = XML_PARSER_CONTENT;
#ifdef DEBUG_PUSH
		xmlGenericError(xmlGenericErrorContext,
			"PP: entering CONTENT\n");
#endif
		break;
            case XML_PARSER_PI:
		xmlGenericError(xmlGenericErrorContext,
			"PP: internal error, state == PI\n");
		ctxt->instate = XML_PARSER_CONTENT;
#ifdef DEBUG_PUSH
		xmlGenericError(xmlGenericErrorContext,
			"PP: entering CONTENT\n");
#endif
		break;
            case XML_PARSER_ENTITY_DECL:
		xmlGenericError(xmlGenericErrorContext,
			"PP: internal error, state == ENTITY_DECL\n");
		ctxt->instate = XML_PARSER_DTD;
#ifdef DEBUG_PUSH
		xmlGenericError(xmlGenericErrorContext,
			"PP: entering DTD\n");
#endif
		break;
            case XML_PARSER_ENTITY_VALUE:
		xmlGenericError(xmlGenericErrorContext,
			"PP: internal error, state == ENTITY_VALUE\n");
		ctxt->instate = XML_PARSER_CONTENT;
#ifdef DEBUG_PUSH
		xmlGenericError(xmlGenericErrorContext,
			"PP: entering DTD\n");
#endif
		break;
            case XML_PARSER_ATTRIBUTE_VALUE:
		xmlGenericError(xmlGenericErrorContext,
			"PP: internal error, state == ATTRIBUTE_VALUE\n");
		ctxt->instate = XML_PARSER_START_TAG;
#ifdef DEBUG_PUSH
		xmlGenericError(xmlGenericErrorContext,
			"PP: entering START_TAG\n");
#endif
		break;
            case XML_PARSER_SYSTEM_LITERAL:
		xmlGenericError(xmlGenericErrorContext,
			"PP: internal error, state == SYSTEM_LITERAL\n");
		ctxt->instate = XML_PARSER_START_TAG;
#ifdef DEBUG_PUSH
		xmlGenericError(xmlGenericErrorContext,
			"PP: entering START_TAG\n");
#endif
		break;
            case XML_PARSER_PUBLIC_LITERAL:
		xmlGenericError(xmlGenericErrorContext,
			"PP: internal error, state == PUBLIC_LITERAL\n");
		ctxt->instate = XML_PARSER_START_TAG;
#ifdef DEBUG_PUSH
		xmlGenericError(xmlGenericErrorContext,
			"PP: entering START_TAG\n");
#endif
		break;
	}
    }
done:    
#ifdef DEBUG_PUSH
    xmlGenericError(xmlGenericErrorContext, "PP: done %d\n", ret);
#endif
    return(ret);
}

/**
 * xmlParseChunk:
 * @ctxt:  an XML parser context
 * @chunk:  an char array
 * @size:  the size in byte of the chunk
 * @terminate:  last chunk indicator
 *
 * Parse a Chunk of memory
 *
 * Returns zero if no error, the xmlParserErrors otherwise.
 */
int
xmlParseChunk(xmlParserCtxtPtr ctxt, const char *chunk, int size,
              int terminate) {
    if ((size > 0) && (chunk != NULL) && (ctxt->input != NULL) &&
        (ctxt->input->buf != NULL) && (ctxt->instate != XML_PARSER_EOF))  {
	int base = ctxt->input->base - ctxt->input->buf->buffer->content;
	int cur = ctxt->input->cur - ctxt->input->base;
	
	xmlParserInputBufferPush(ctxt->input->buf, size, chunk);	      
	ctxt->input->base = ctxt->input->buf->buffer->content + base;
	ctxt->input->cur = ctxt->input->base + cur;
	ctxt->input->end =
	    &ctxt->input->buf->buffer->content[ctxt->input->buf->buffer->use];
#ifdef DEBUG_PUSH
	xmlGenericError(xmlGenericErrorContext, "PP: pushed %d\n", size);
#endif

	if ((terminate) || (ctxt->input->buf->buffer->use > 80))
	    xmlParseTryOrFinish(ctxt, terminate);
    } else if (ctxt->instate != XML_PARSER_EOF) {
	if ((ctxt->input != NULL) && ctxt->input->buf != NULL) {
	    xmlParserInputBufferPtr in = ctxt->input->buf;
	    if ((in->encoder != NULL) && (in->buffer != NULL) &&
		    (in->raw != NULL)) {
		int nbchars;
		    
		nbchars = xmlCharEncInFunc(in->encoder, in->buffer, in->raw);
		if (nbchars < 0) {
		    xmlGenericError(xmlGenericErrorContext,
				    "xmlParseChunk: encoder error\n");
		    return(XML_ERR_INVALID_ENCODING);
		}
	    }
	}
    }
    xmlParseTryOrFinish(ctxt, terminate);
    if (terminate) {
	/*
	 * Check for termination
	 */
	    int avail = 0;
	    if (ctxt->input->buf == NULL)
                avail = ctxt->input->length -
		        (ctxt->input->cur - ctxt->input->base);
            else
                avail = ctxt->input->buf->buffer->use -
		        (ctxt->input->cur - ctxt->input->base);
			    
	if ((ctxt->instate != XML_PARSER_EOF) &&
	    (ctxt->instate != XML_PARSER_EPILOG)) {
	    ctxt->errNo = XML_ERR_DOCUMENT_END;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData,
		    "Extra content at the end of the document\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	} 
	if ((ctxt->instate == XML_PARSER_EPILOG) && (avail > 0)) {
	    ctxt->errNo = XML_ERR_DOCUMENT_END;
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData,
		    "Extra content at the end of the document\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;

	}
	if (ctxt->instate != XML_PARSER_EOF) {
	    if ((ctxt->sax) && (ctxt->sax->endDocument != NULL))
		ctxt->sax->endDocument(ctxt->userData);
	}
	ctxt->instate = XML_PARSER_EOF;
    }
    return((xmlParserErrors) ctxt->errNo);	      
}

/************************************************************************
 *									*
 * 		I/O front end functions to the parser			*
 *									*
 ************************************************************************/

/**
 * xmlStopParser:
 * @ctxt:  an XML parser context
 *
 * Blocks further parser processing
 */
void           
xmlStopParser(xmlParserCtxtPtr ctxt) {
    ctxt->instate = XML_PARSER_EOF;
    if (ctxt->input != NULL)
	ctxt->input->cur = BAD_CAST"";
}

/**
 * xmlCreatePushParserCtxt:
 * @sax:  a SAX handler
 * @user_data:  The user data returned on SAX callbacks
 * @chunk:  a pointer to an array of chars
 * @size:  number of chars in the array
 * @filename:  an optional file name or URI
 *
 * Create a parser context for using the XML parser in push mode.
 * If @buffer and @size are non-NULL, the data is used to detect
 * the encoding.  The remaining characters will be parsed so they
 * don't need to be fed in again through xmlParseChunk.
 * To allow content encoding detection, @size should be >= 4
 * The value of @filename is used for fetching external entities
 * and error/warning reports.
 *
 * Returns the new parser context or NULL
 */

xmlParserCtxtPtr
xmlCreatePushParserCtxt(xmlSAXHandlerPtr sax, void *user_data, 
                        const char *chunk, int size, const char *filename) {
    xmlParserCtxtPtr ctxt;
    xmlParserInputPtr inputStream;
    xmlParserInputBufferPtr buf;
    xmlCharEncoding enc = XML_CHAR_ENCODING_NONE;

    /*
     * plug some encoding conversion routines
     */
    if ((chunk != NULL) && (size >= 4))
	enc = xmlDetectCharEncoding((const xmlChar *) chunk, size);

    buf = xmlAllocParserInputBuffer(enc);
    if (buf == NULL) return(NULL);

    ctxt = xmlNewParserCtxt();
    if (ctxt == NULL) {
	xmlFree(buf);
	return(NULL);
    }
    if (sax != NULL) {
	if (ctxt->sax != &xmlDefaultSAXHandler)
	    xmlFree(ctxt->sax);
	ctxt->sax = (xmlSAXHandlerPtr) xmlMalloc(sizeof(xmlSAXHandler));
	if (ctxt->sax == NULL) {
	    xmlFree(buf);
	    xmlFree(ctxt);
	    return(NULL);
	}
	memcpy(ctxt->sax, sax, sizeof(xmlSAXHandler));
	if (user_data != NULL)
	    ctxt->userData = user_data;
    }	
    if (filename == NULL) {
	ctxt->directory = NULL;
    } else {
        ctxt->directory = xmlParserGetDirectory(filename);
    }

    inputStream = xmlNewInputStream(ctxt);
    if (inputStream == NULL) {
	xmlFreeParserCtxt(ctxt);
	return(NULL);
    }

    if (filename == NULL)
	inputStream->filename = NULL;
    else
	inputStream->filename = (char *)
	    xmlNormalizeWindowsPath((const xmlChar *) filename);
    inputStream->buf = buf;
    inputStream->base = inputStream->buf->buffer->content;
    inputStream->cur = inputStream->buf->buffer->content;
    inputStream->end = 
	&inputStream->buf->buffer->content[inputStream->buf->buffer->use];

    inputPush(ctxt, inputStream);

    if ((size > 0) && (chunk != NULL) && (ctxt->input != NULL) &&
        (ctxt->input->buf != NULL))  {	      
	int base = ctxt->input->base - ctxt->input->buf->buffer->content;
	int cur = ctxt->input->cur - ctxt->input->base;

	xmlParserInputBufferPush(ctxt->input->buf, size, chunk);	      

	ctxt->input->base = ctxt->input->buf->buffer->content + base;
	ctxt->input->cur = ctxt->input->base + cur;
	ctxt->input->end =
	    &ctxt->input->buf->buffer->content[ctxt->input->buf->buffer->use];
#ifdef DEBUG_PUSH
	xmlGenericError(xmlGenericErrorContext, "PP: pushed %d\n", size);
#endif
    }

    if (enc != XML_CHAR_ENCODING_NONE) {
        xmlSwitchEncoding(ctxt, enc);
    }

    return(ctxt);
}

/**
 * xmlCreateIOParserCtxt:
 * @sax:  a SAX handler
 * @user_data:  The user data returned on SAX callbacks
 * @ioread:  an I/O read function
 * @ioclose:  an I/O close function
 * @ioctx:  an I/O handler
 * @enc:  the charset encoding if known
 *
 * Create a parser context for using the XML parser with an existing
 * I/O stream
 *
 * Returns the new parser context or NULL
 */
xmlParserCtxtPtr
xmlCreateIOParserCtxt(xmlSAXHandlerPtr sax, void *user_data,
	xmlInputReadCallback   ioread, xmlInputCloseCallback  ioclose,
	void *ioctx, xmlCharEncoding enc) {
    xmlParserCtxtPtr ctxt;
    xmlParserInputPtr inputStream;
    xmlParserInputBufferPtr buf;

    buf = xmlParserInputBufferCreateIO(ioread, ioclose, ioctx, enc);
    if (buf == NULL) return(NULL);

    ctxt = xmlNewParserCtxt();
    if (ctxt == NULL) {
	xmlFree(buf);
	return(NULL);
    }
    if (sax != NULL) {
	if (ctxt->sax != &xmlDefaultSAXHandler)
	    xmlFree(ctxt->sax);
	ctxt->sax = (xmlSAXHandlerPtr) xmlMalloc(sizeof(xmlSAXHandler));
	if (ctxt->sax == NULL) {
	    xmlFree(buf);
	    xmlFree(ctxt);
	    return(NULL);
	}
	memcpy(ctxt->sax, sax, sizeof(xmlSAXHandler));
	if (user_data != NULL)
	    ctxt->userData = user_data;
    }	

    inputStream = xmlNewIOInputStream(ctxt, buf, enc);
    if (inputStream == NULL) {
	xmlFreeParserCtxt(ctxt);
	return(NULL);
    }
    inputPush(ctxt, inputStream);

    return(ctxt);
}

/************************************************************************
 *									*
 * 		Front ends when parsing a DTD				*
 *									*
 ************************************************************************/

/**
 * xmlIOParseDTD:
 * @sax:  the SAX handler block or NULL
 * @input:  an Input Buffer
 * @enc:  the charset encoding if known
 *
 * Load and parse a DTD
 * 
 * Returns the resulting xmlDtdPtr or NULL in case of error.
 * @input will be freed at parsing end.
 */

xmlDtdPtr
xmlIOParseDTD(xmlSAXHandlerPtr sax, xmlParserInputBufferPtr input,
	      xmlCharEncoding enc) {
    xmlDtdPtr ret = NULL;
    xmlParserCtxtPtr ctxt;
    xmlParserInputPtr pinput = NULL;
    xmlChar start[4];

    if (input == NULL)
	return(NULL);

    ctxt = xmlNewParserCtxt();
    if (ctxt == NULL) {
	return(NULL);
    }

    /*
     * Set-up the SAX context
     */
    if (sax != NULL) { 
	if (ctxt->sax != NULL)
	    xmlFree(ctxt->sax);
        ctxt->sax = sax;
        ctxt->userData = NULL;
    }

    /*
     * generate a parser input from the I/O handler
     */

    pinput = xmlNewIOInputStream(ctxt, input, enc);
    if (pinput == NULL) {
        if (sax != NULL) ctxt->sax = NULL;
	xmlFreeParserCtxt(ctxt);
	return(NULL);
    }

    /*
     * plug some encoding conversion routines here.
     */
    xmlPushInput(ctxt, pinput);

    pinput->filename = NULL;
    pinput->line = 1;
    pinput->col = 1;
    pinput->base = ctxt->input->cur;
    pinput->cur = ctxt->input->cur;
    pinput->free = NULL;

    /*
     * let's parse that entity knowing it's an external subset.
     */
    ctxt->inSubset = 2;
    ctxt->myDoc = xmlNewDoc(BAD_CAST "1.0");
    ctxt->myDoc->extSubset = xmlNewDtd(ctxt->myDoc, BAD_CAST "none",
	                               BAD_CAST "none", BAD_CAST "none");

    if (enc == XML_CHAR_ENCODING_NONE) {
	/* 
	 * Get the 4 first bytes and decode the charset
	 * if enc != XML_CHAR_ENCODING_NONE
	 * plug some encoding conversion routines.
	 */
	start[0] = RAW;
	start[1] = NXT(1);
	start[2] = NXT(2);
	start[3] = NXT(3);
	enc = xmlDetectCharEncoding(start, 4);
	if (enc != XML_CHAR_ENCODING_NONE) {
	    xmlSwitchEncoding(ctxt, enc);
	}
    }

    xmlParseExternalSubset(ctxt, BAD_CAST "none", BAD_CAST "none");

    if (ctxt->myDoc != NULL) {
	if (ctxt->wellFormed) {
	    ret = ctxt->myDoc->extSubset;
	    ctxt->myDoc->extSubset = NULL;
	} else {
	    ret = NULL;
	}
        xmlFreeDoc(ctxt->myDoc);
        ctxt->myDoc = NULL;
    }
    if (sax != NULL) ctxt->sax = NULL;
    xmlFreeParserCtxt(ctxt);
    
    return(ret);
}

/**
 * xmlSAXParseDTD:
 * @sax:  the SAX handler block
 * @ExternalID:  a NAME* containing the External ID of the DTD
 * @SystemID:  a NAME* containing the URL to the DTD
 *
 * Load and parse an external subset.
 * 
 * Returns the resulting xmlDtdPtr or NULL in case of error.
 */

xmlDtdPtr
xmlSAXParseDTD(xmlSAXHandlerPtr sax, const xmlChar *ExternalID,
                          const xmlChar *SystemID) {
    xmlDtdPtr ret = NULL;
    xmlParserCtxtPtr ctxt;
    xmlParserInputPtr input = NULL;
    xmlCharEncoding enc;

    if ((ExternalID == NULL) && (SystemID == NULL)) return(NULL);

    ctxt = xmlNewParserCtxt();
    if (ctxt == NULL) {
	return(NULL);
    }

    /*
     * Set-up the SAX context
     */
    if (sax != NULL) { 
	if (ctxt->sax != NULL)
	    xmlFree(ctxt->sax);
        ctxt->sax = sax;
        ctxt->userData = NULL;
    }

    /*
     * Ask the Entity resolver to load the damn thing
     */

    if ((ctxt->sax != NULL) && (ctxt->sax->resolveEntity != NULL))
	input = ctxt->sax->resolveEntity(ctxt->userData, ExternalID, SystemID);
    if (input == NULL) {
        if (sax != NULL) ctxt->sax = NULL;
	xmlFreeParserCtxt(ctxt);
	return(NULL);
    }

    /*
     * plug some encoding conversion routines here.
     */
    xmlPushInput(ctxt, input);
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
    ctxt->inSubset = 2;
    ctxt->myDoc = xmlNewDoc(BAD_CAST "1.0");
    ctxt->myDoc->extSubset = xmlNewDtd(ctxt->myDoc, BAD_CAST "none",
	                               ExternalID, SystemID);
    xmlParseExternalSubset(ctxt, ExternalID, SystemID);

    if (ctxt->myDoc != NULL) {
	if (ctxt->wellFormed) {
	    ret = ctxt->myDoc->extSubset;
	    ctxt->myDoc->extSubset = NULL;
	} else {
	    ret = NULL;
	}
        xmlFreeDoc(ctxt->myDoc);
        ctxt->myDoc = NULL;
    }
    if (sax != NULL) ctxt->sax = NULL;
    xmlFreeParserCtxt(ctxt);
    
    return(ret);
}

/**
 * xmlParseDTD:
 * @ExternalID:  a NAME* containing the External ID of the DTD
 * @SystemID:  a NAME* containing the URL to the DTD
 *
 * Load and parse an external subset.
 * 
 * Returns the resulting xmlDtdPtr or NULL in case of error.
 */

xmlDtdPtr
xmlParseDTD(const xmlChar *ExternalID, const xmlChar *SystemID) {
    return(xmlSAXParseDTD(NULL, ExternalID, SystemID));
}

/************************************************************************
 *									*
 * 		Front ends when parsing an Entity			*
 *									*
 ************************************************************************/

/**
 * xmlParseCtxtExternalEntity:
 * @ctx:  the existing parsing context
 * @URL:  the URL for the entity to load
 * @ID:  the System ID for the entity to load
 * @lst:  the return value for the set of parsed nodes
 *
 * Parse an external general entity within an existing parsing context
 * An external general parsed entity is well-formed if it matches the
 * production labeled extParsedEnt.
 *
 * [78] extParsedEnt ::= TextDecl? content
 *
 * Returns 0 if the entity is well formed, -1 in case of args problem and
 *    the parser error code otherwise
 */

int
xmlParseCtxtExternalEntity(xmlParserCtxtPtr ctx, const xmlChar *URL,
	               const xmlChar *ID, xmlNodePtr *lst) {
    xmlParserCtxtPtr ctxt;
    xmlDocPtr newDoc;
    xmlSAXHandlerPtr oldsax = NULL;
    int ret = 0;
    xmlChar start[4];
    xmlCharEncoding enc;

    if (ctx->depth > 40) {
	return(XML_ERR_ENTITY_LOOP);
    }

    if (lst != NULL)
        *lst = NULL;
    if ((URL == NULL) && (ID == NULL))
	return(-1);
    if (ctx->myDoc == NULL) /* @@ relax but check for dereferences */
	return(-1);


    ctxt = xmlCreateEntityParserCtxt(URL, ID, NULL);
    if (ctxt == NULL) return(-1);
    ctxt->userData = ctxt;
    oldsax = ctxt->sax;
    ctxt->sax = ctx->sax;
    newDoc = xmlNewDoc(BAD_CAST "1.0");
    if (newDoc == NULL) {
	xmlFreeParserCtxt(ctxt);
	return(-1);
    }
    if (ctx->myDoc != NULL) {
	newDoc->intSubset = ctx->myDoc->intSubset;
	newDoc->extSubset = ctx->myDoc->extSubset;
    }
    if (ctx->myDoc->URL != NULL) {
	newDoc->URL = xmlStrdup(ctx->myDoc->URL);
    }
    newDoc->children = xmlNewDocNode(newDoc, NULL, BAD_CAST "pseudoroot", NULL);
    if (newDoc->children == NULL) {
	ctxt->sax = oldsax;
	xmlFreeParserCtxt(ctxt);
	newDoc->intSubset = NULL;
	newDoc->extSubset = NULL;
        xmlFreeDoc(newDoc);
	return(-1);
    }
    nodePush(ctxt, newDoc->children);
    if (ctx->myDoc == NULL) {
	ctxt->myDoc = newDoc;
    } else {
	ctxt->myDoc = ctx->myDoc;
	newDoc->children->doc = ctx->myDoc;
    }

    /* 
     * Get the 4 first bytes and decode the charset
     * if enc != XML_CHAR_ENCODING_NONE
     * plug some encoding conversion routines.
     */
    GROW
    start[0] = RAW;
    start[1] = NXT(1);
    start[2] = NXT(2);
    start[3] = NXT(3);
    enc = xmlDetectCharEncoding(start, 4);
    if (enc != XML_CHAR_ENCODING_NONE) {
        xmlSwitchEncoding(ctxt, enc);
    }

    /*
     * Parse a possible text declaration first
     */
    if ((RAW == '<') && (NXT(1) == '?') &&
	(NXT(2) == 'x') && (NXT(3) == 'm') &&
	(NXT(4) == 'l') && (IS_BLANK(NXT(5)))) {
	xmlParseTextDecl(ctxt);
    }

    /*
     * Doing validity checking on chunk doesn't make sense
     */
    ctxt->instate = XML_PARSER_CONTENT;
    ctxt->validate = ctx->validate;
    ctxt->loadsubset = ctx->loadsubset;
    ctxt->depth = ctx->depth + 1;
    ctxt->replaceEntities = ctx->replaceEntities;
    if (ctxt->validate) {
	ctxt->vctxt.error = ctx->vctxt.error;
	ctxt->vctxt.warning = ctx->vctxt.warning;
    } else {
	ctxt->vctxt.error = NULL;
	ctxt->vctxt.warning = NULL;
    }
    ctxt->vctxt.nodeTab = NULL;
    ctxt->vctxt.nodeNr = 0;
    ctxt->vctxt.nodeMax = 0;
    ctxt->vctxt.node = NULL;

    xmlParseContent(ctxt);
   
    if ((RAW == '<') && (NXT(1) == '/')) {
	ctxt->errNo = XML_ERR_NOT_WELL_BALANCED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
		"chunk is not well balanced\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    } else if (RAW != 0) {
	ctxt->errNo = XML_ERR_EXTRA_CONTENT;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
		"extra content at the end of well balanced chunk\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    }
    if (ctxt->node != newDoc->children) {
	ctxt->errNo = XML_ERR_NOT_WELL_BALANCED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
		"chunk is not well balanced\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    }

    if (!ctxt->wellFormed) {
        if (ctxt->errNo == 0)
	    ret = 1;
	else
	    ret = ctxt->errNo;
    } else {
	if (lst != NULL) {
	    xmlNodePtr cur;

	    /*
	     * Return the newly created nodeset after unlinking it from
	     * they pseudo parent.
	     */
	    cur = newDoc->children->children;
	    *lst = cur;
	    while (cur != NULL) {
		cur->parent = NULL;
		cur = cur->next;
	    }
            newDoc->children->children = NULL;
	}
	ret = 0;
    }
    ctxt->sax = oldsax;
    xmlFreeParserCtxt(ctxt);
    newDoc->intSubset = NULL;
    newDoc->extSubset = NULL;
    xmlFreeDoc(newDoc);
    
    return(ret);
}

/**
 * xmlParseExternalEntityPrivate:
 * @doc:  the document the chunk pertains to
 * @oldctxt:  the previous parser context if available
 * @sax:  the SAX handler bloc (possibly NULL)
 * @user_data:  The user data returned on SAX callbacks (possibly NULL)
 * @depth:  Used for loop detection, use 0
 * @URL:  the URL for the entity to load
 * @ID:  the System ID for the entity to load
 * @list:  the return value for the set of parsed nodes
 *
 * Private version of xmlParseExternalEntity()
 *
 * Returns 0 if the entity is well formed, -1 in case of args problem and
 *    the parser error code otherwise
 */

static int
xmlParseExternalEntityPrivate(xmlDocPtr doc, xmlParserCtxtPtr oldctxt,
	              xmlSAXHandlerPtr sax,
		      void *user_data, int depth, const xmlChar *URL,
		      const xmlChar *ID, xmlNodePtr *list) {
    xmlParserCtxtPtr ctxt;
    xmlDocPtr newDoc;
    xmlSAXHandlerPtr oldsax = NULL;
    int ret = 0;
    xmlChar start[4];
    xmlCharEncoding enc;

    if (depth > 40) {
	return(XML_ERR_ENTITY_LOOP);
    }



    if (list != NULL)
        *list = NULL;
    if ((URL == NULL) && (ID == NULL))
	return(-1);
    if (doc == NULL) /* @@ relax but check for dereferences */
	return(-1);


    ctxt = xmlCreateEntityParserCtxt(URL, ID, NULL);
    if (ctxt == NULL) return(-1);
    ctxt->userData = ctxt;
    if (oldctxt != NULL) {
	ctxt->_private = oldctxt->_private;
	ctxt->loadsubset = oldctxt->loadsubset;
	ctxt->validate = oldctxt->validate;
	ctxt->external = oldctxt->external;
    } else {
	/*
	 * Doing validity checking on chunk without context
	 * doesn't make sense
	 */
	ctxt->_private = NULL;
	ctxt->validate = 0;
	ctxt->external = 2;
	ctxt->loadsubset = 0;
    }
    if (sax != NULL) {
	oldsax = ctxt->sax;
        ctxt->sax = sax;
	if (user_data != NULL)
	    ctxt->userData = user_data;
    }
    newDoc = xmlNewDoc(BAD_CAST "1.0");
    if (newDoc == NULL) {
	xmlFreeParserCtxt(ctxt);
	return(-1);
    }
    if (doc != NULL) {
	newDoc->intSubset = doc->intSubset;
	newDoc->extSubset = doc->extSubset;
    }
    if (doc->URL != NULL) {
	newDoc->URL = xmlStrdup(doc->URL);
    }
    newDoc->children = xmlNewDocNode(newDoc, NULL, BAD_CAST "pseudoroot", NULL);
    if (newDoc->children == NULL) {
	if (sax != NULL)
	    ctxt->sax = oldsax;
	xmlFreeParserCtxt(ctxt);
	newDoc->intSubset = NULL;
	newDoc->extSubset = NULL;
        xmlFreeDoc(newDoc);
	return(-1);
    }
    nodePush(ctxt, newDoc->children);
    if (doc == NULL) {
	ctxt->myDoc = newDoc;
    } else {
	ctxt->myDoc = doc;
	newDoc->children->doc = doc;
    }

    /* 
     * Get the 4 first bytes and decode the charset
     * if enc != XML_CHAR_ENCODING_NONE
     * plug some encoding conversion routines.
     */
    GROW;
    start[0] = RAW;
    start[1] = NXT(1);
    start[2] = NXT(2);
    start[3] = NXT(3);
    enc = xmlDetectCharEncoding(start, 4);
    if (enc != XML_CHAR_ENCODING_NONE) {
        xmlSwitchEncoding(ctxt, enc);
    }

    /*
     * Parse a possible text declaration first
     */
    if ((RAW == '<') && (NXT(1) == '?') &&
	(NXT(2) == 'x') && (NXT(3) == 'm') &&
	(NXT(4) == 'l') && (IS_BLANK(NXT(5)))) {
	xmlParseTextDecl(ctxt);
    }

    ctxt->instate = XML_PARSER_CONTENT;
    ctxt->depth = depth;

    xmlParseContent(ctxt);
   
    if ((RAW == '<') && (NXT(1) == '/')) {
	ctxt->errNo = XML_ERR_NOT_WELL_BALANCED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
		"chunk is not well balanced\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    } else if (RAW != 0) {
	ctxt->errNo = XML_ERR_EXTRA_CONTENT;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
		"extra content at the end of well balanced chunk\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    }
    if (ctxt->node != newDoc->children) {
	ctxt->errNo = XML_ERR_NOT_WELL_BALANCED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
		"chunk is not well balanced\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    }

    if (!ctxt->wellFormed) {
        if (ctxt->errNo == 0)
	    ret = 1;
	else
	    ret = ctxt->errNo;
    } else {
	if (list != NULL) {
	    xmlNodePtr cur;

	    /*
	     * Return the newly created nodeset after unlinking it from
	     * they pseudo parent.
	     */
	    cur = newDoc->children->children;
	    *list = cur;
	    while (cur != NULL) {
		cur->parent = NULL;
		cur = cur->next;
	    }
            newDoc->children->children = NULL;
	}
	ret = 0;
    }
    if (sax != NULL) 
	ctxt->sax = oldsax;
    xmlFreeParserCtxt(ctxt);
    newDoc->intSubset = NULL;
    newDoc->extSubset = NULL;
    xmlFreeDoc(newDoc);
    
    return(ret);
}

/**
 * xmlParseExternalEntity:
 * @doc:  the document the chunk pertains to
 * @sax:  the SAX handler bloc (possibly NULL)
 * @user_data:  The user data returned on SAX callbacks (possibly NULL)
 * @depth:  Used for loop detection, use 0
 * @URL:  the URL for the entity to load
 * @ID:  the System ID for the entity to load
 * @lst:  the return value for the set of parsed nodes
 *
 * Parse an external general entity
 * An external general parsed entity is well-formed if it matches the
 * production labeled extParsedEnt.
 *
 * [78] extParsedEnt ::= TextDecl? content
 *
 * Returns 0 if the entity is well formed, -1 in case of args problem and
 *    the parser error code otherwise
 */

int
xmlParseExternalEntity(xmlDocPtr doc, xmlSAXHandlerPtr sax, void *user_data,
	  int depth, const xmlChar *URL, const xmlChar *ID, xmlNodePtr *lst) {
    return(xmlParseExternalEntityPrivate(doc, NULL, sax, user_data, depth, URL,
		                       ID, lst));
}

/**
 * xmlParseBalancedChunkMemory:
 * @doc:  the document the chunk pertains to
 * @sax:  the SAX handler bloc (possibly NULL)
 * @user_data:  The user data returned on SAX callbacks (possibly NULL)
 * @depth:  Used for loop detection, use 0
 * @string:  the input string in UTF8 or ISO-Latin (zero terminated)
 * @lst:  the return value for the set of parsed nodes
 *
 * Parse a well-balanced chunk of an XML document
 * called by the parser
 * The allowed sequence for the Well Balanced Chunk is the one defined by
 * the content production in the XML grammar:
 *
 * [43] content ::= (element | CharData | Reference | CDSect | PI | Comment)*
 *
 * Returns 0 if the chunk is well balanced, -1 in case of args problem and
 *    the parser error code otherwise
 */

int
xmlParseBalancedChunkMemory(xmlDocPtr doc, xmlSAXHandlerPtr sax,
     void *user_data, int depth, const xmlChar *string, xmlNodePtr *lst) {
    return xmlParseBalancedChunkMemoryRecover( doc, sax, user_data,
                                                depth, string, lst, 0 );
}

/**
 * xmlParseBalancedChunkMemoryInternal:
 * @oldctxt:  the existing parsing context
 * @string:  the input string in UTF8 or ISO-Latin (zero terminated)
 * @user_data:  the user data field for the parser context
 * @lst:  the return value for the set of parsed nodes
 *
 *
 * Parse a well-balanced chunk of an XML document
 * called by the parser
 * The allowed sequence for the Well Balanced Chunk is the one defined by
 * the content production in the XML grammar:
 *
 * [43] content ::= (element | CharData | Reference | CDSect | PI | Comment)*
 *
 * Returns 0 if the chunk is well balanced, -1 in case of args problem and
 *    the parser error code otherwise
 *    
 * In case recover is set to 1, the nodelist will not be empty even if
 * the parsed chunk is not well balanced. 
 */
static int
xmlParseBalancedChunkMemoryInternal(xmlParserCtxtPtr oldctxt,
	const xmlChar *string, void *user_data, xmlNodePtr *lst) {
    xmlParserCtxtPtr ctxt;
    xmlDocPtr newDoc = NULL;
    xmlSAXHandlerPtr oldsax = NULL;
    xmlNodePtr content = NULL;
    int size;
    int ret = 0;

    if (oldctxt->depth > 40) {
	return(XML_ERR_ENTITY_LOOP);
    }


    if (lst != NULL)
        *lst = NULL;
    if (string == NULL)
        return(-1);

    size = xmlStrlen(string);

    ctxt = xmlCreateMemoryParserCtxt((char *) string, size);
    if (ctxt == NULL) return(-1);
    if (user_data != NULL)
	ctxt->userData = user_data;
    else
	ctxt->userData = ctxt;

    oldsax = ctxt->sax;
    ctxt->sax = oldctxt->sax;
    ctxt->_private = oldctxt->_private;
    if (oldctxt->myDoc == NULL) {
	newDoc = xmlNewDoc(BAD_CAST "1.0");
	if (newDoc == NULL) {
	    ctxt->sax = oldsax;
	    xmlFreeParserCtxt(ctxt);
	    return(-1);
	}
	ctxt->myDoc = newDoc;
    } else {
	ctxt->myDoc = oldctxt->myDoc;
        content = ctxt->myDoc->children;
    }
    ctxt->myDoc->children = xmlNewDocNode(ctxt->myDoc, NULL,
	                                  BAD_CAST "pseudoroot", NULL);
    if (ctxt->myDoc->children == NULL) {
	ctxt->sax = oldsax;
	xmlFreeParserCtxt(ctxt);
	if (newDoc != NULL)
	    xmlFreeDoc(newDoc);
	return(-1);
    }
    nodePush(ctxt, ctxt->myDoc->children);
    ctxt->instate = XML_PARSER_CONTENT;
    ctxt->depth = oldctxt->depth + 1;

    /*
     * Doing validity checking on chunk doesn't make sense
     */
    ctxt->validate = 0;
    ctxt->loadsubset = oldctxt->loadsubset;

    xmlParseContent(ctxt);
    if ((RAW == '<') && (NXT(1) == '/')) {
	ctxt->errNo = XML_ERR_NOT_WELL_BALANCED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
		"chunk is not well balanced\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    } else if (RAW != 0) {
	ctxt->errNo = XML_ERR_EXTRA_CONTENT;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
		"extra content at the end of well balanced chunk\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    }
    if (ctxt->node != ctxt->myDoc->children) {
	ctxt->errNo = XML_ERR_NOT_WELL_BALANCED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
		"chunk is not well balanced\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    }

    if (!ctxt->wellFormed) {
        if (ctxt->errNo == 0)
	    ret = 1;
	else
	    ret = ctxt->errNo;
    } else {
      ret = 0;
    }
    
    if ((lst != NULL) && (ret == 0)) {
	xmlNodePtr cur;

	/*
	 * Return the newly created nodeset after unlinking it from
	 * they pseudo parent.
	 */
	cur = ctxt->myDoc->children->children;
	*lst = cur;
	while (cur != NULL) {
	    cur->parent = NULL;
	    cur = cur->next;
	}
	ctxt->myDoc->children->children = NULL;
    }
    if (ctxt->myDoc != NULL) {
	xmlFreeNode(ctxt->myDoc->children);
        ctxt->myDoc->children = content;
    }
	
    ctxt->sax = oldsax;
    xmlFreeParserCtxt(ctxt);
    if (newDoc != NULL)
	xmlFreeDoc(newDoc);
    
    return(ret);
}

/**
 * xmlParseBalancedChunkMemoryRecover:
 * @doc:  the document the chunk pertains to
 * @sax:  the SAX handler bloc (possibly NULL)
 * @user_data:  The user data returned on SAX callbacks (possibly NULL)
 * @depth:  Used for loop detection, use 0
 * @string:  the input string in UTF8 or ISO-Latin (zero terminated)
 * @lst:  the return value for the set of parsed nodes
 * @recover: return nodes even if the data is broken (use 0)
 *
 *
 * Parse a well-balanced chunk of an XML document
 * called by the parser
 * The allowed sequence for the Well Balanced Chunk is the one defined by
 * the content production in the XML grammar:
 *
 * [43] content ::= (element | CharData | Reference | CDSect | PI | Comment)*
 *
 * Returns 0 if the chunk is well balanced, -1 in case of args problem and
 *    the parser error code otherwise
 *    
 * In case recover is set to 1, the nodelist will not be empty even if
 * the parsed chunk is not well balanced. 
 */
int
xmlParseBalancedChunkMemoryRecover(xmlDocPtr doc, xmlSAXHandlerPtr sax,
     void *user_data, int depth, const xmlChar *string, xmlNodePtr *lst, 
     int recover) {
    xmlParserCtxtPtr ctxt;
    xmlDocPtr newDoc;
    xmlSAXHandlerPtr oldsax = NULL;
    xmlNodePtr content;
    int size;
    int ret = 0;

    if (depth > 40) {
	return(XML_ERR_ENTITY_LOOP);
    }


    if (lst != NULL)
        *lst = NULL;
    if (string == NULL)
        return(-1);

    size = xmlStrlen(string);

    ctxt = xmlCreateMemoryParserCtxt((char *) string, size);
    if (ctxt == NULL) return(-1);
    ctxt->userData = ctxt;
    if (sax != NULL) {
	oldsax = ctxt->sax;
        ctxt->sax = sax;
	if (user_data != NULL)
	    ctxt->userData = user_data;
    }
    newDoc = xmlNewDoc(BAD_CAST "1.0");
    if (newDoc == NULL) {
	xmlFreeParserCtxt(ctxt);
	return(-1);
    }
    if (doc != NULL) {
	newDoc->intSubset = doc->intSubset;
	newDoc->extSubset = doc->extSubset;
    }
    newDoc->children = xmlNewDocNode(newDoc, NULL, BAD_CAST "pseudoroot", NULL);
    if (newDoc->children == NULL) {
	if (sax != NULL)
	    ctxt->sax = oldsax;
	xmlFreeParserCtxt(ctxt);
	newDoc->intSubset = NULL;
	newDoc->extSubset = NULL;
        xmlFreeDoc(newDoc);
	return(-1);
    }
    nodePush(ctxt, newDoc->children);
    if (doc == NULL) {
	ctxt->myDoc = newDoc;
    } else {
	ctxt->myDoc = newDoc;
	newDoc->children->doc = doc;
    }
    ctxt->instate = XML_PARSER_CONTENT;
    ctxt->depth = depth;

    /*
     * Doing validity checking on chunk doesn't make sense
     */
    ctxt->validate = 0;
    ctxt->loadsubset = 0;

    if ( doc != NULL ){
        content = doc->children;
        doc->children = NULL;
        xmlParseContent(ctxt);
        doc->children = content;
    }
    else {
        xmlParseContent(ctxt);
    }
    if ((RAW == '<') && (NXT(1) == '/')) {
	ctxt->errNo = XML_ERR_NOT_WELL_BALANCED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
		"chunk is not well balanced\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    } else if (RAW != 0) {
	ctxt->errNo = XML_ERR_EXTRA_CONTENT;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
		"extra content at the end of well balanced chunk\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    }
    if (ctxt->node != newDoc->children) {
	ctxt->errNo = XML_ERR_NOT_WELL_BALANCED;
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData,
		"chunk is not well balanced\n");
	ctxt->wellFormed = 0;
	if (ctxt->recovery == 0) ctxt->disableSAX = 1;
    }

    if (!ctxt->wellFormed) {
        if (ctxt->errNo == 0)
	    ret = 1;
	else
	    ret = ctxt->errNo;
    } else {
      ret = 0;
    }
    
    if (lst != NULL && (ret == 0 || recover == 1)) {
      xmlNodePtr cur;

	    /*
	     * Return the newly created nodeset after unlinking it from
	     * they pseudo parent.
	     */
	    cur = newDoc->children->children;
	    *lst = cur;
	    while (cur != NULL) {
		cur->parent = NULL;
		cur = cur->next;
	    }
            newDoc->children->children = NULL;
	}
	
    if (sax != NULL) 
	ctxt->sax = oldsax;
    xmlFreeParserCtxt(ctxt);
    newDoc->intSubset = NULL;
    newDoc->extSubset = NULL;
    xmlFreeDoc(newDoc);
    
    return(ret);
}

/**
 * xmlSAXParseEntity:
 * @sax:  the SAX handler block
 * @filename:  the filename
 *
 * parse an XML external entity out of context and build a tree.
 * It use the given SAX function block to handle the parsing callback.
 * If sax is NULL, fallback to the default DOM tree building routines.
 *
 * [78] extParsedEnt ::= TextDecl? content
 *
 * This correspond to a "Well Balanced" chunk
 *
 * Returns the resulting document tree
 */

xmlDocPtr
xmlSAXParseEntity(xmlSAXHandlerPtr sax, const char *filename) {
    xmlDocPtr ret;
    xmlParserCtxtPtr ctxt;
    char *directory = NULL;

    ctxt = xmlCreateFileParserCtxt(filename);
    if (ctxt == NULL) {
	return(NULL);
    }
    if (sax != NULL) {
	if (ctxt->sax != NULL)
	    xmlFree(ctxt->sax);
        ctxt->sax = sax;
        ctxt->userData = NULL;
    }

    if ((ctxt->directory == NULL) && (directory == NULL))
        directory = xmlParserGetDirectory(filename);

    xmlParseExtParsedEnt(ctxt);

    if (ctxt->wellFormed)
	ret = ctxt->myDoc;
    else {
        ret = NULL;
        xmlFreeDoc(ctxt->myDoc);
        ctxt->myDoc = NULL;
    }
    if (sax != NULL)
        ctxt->sax = NULL;
    xmlFreeParserCtxt(ctxt);
    
    return(ret);
}

/**
 * xmlParseEntity:
 * @filename:  the filename
 *
 * parse an XML external entity out of context and build a tree.
 *
 * [78] extParsedEnt ::= TextDecl? content
 *
 * This correspond to a "Well Balanced" chunk
 *
 * Returns the resulting document tree
 */

xmlDocPtr
xmlParseEntity(const char *filename) {
    return(xmlSAXParseEntity(NULL, filename));
}

/**
 * xmlCreateEntityParserCtxt:
 * @URL:  the entity URL
 * @ID:  the entity PUBLIC ID
 * @base:  a possible base for the target URI
 *
 * Create a parser context for an external entity
 * Automatic support for ZLIB/Compress compressed document is provided
 * by default if found at compile-time.
 *
 * Returns the new parser context or NULL
 */
xmlParserCtxtPtr
xmlCreateEntityParserCtxt(const xmlChar *URL, const xmlChar *ID,
	                  const xmlChar *base) {
    xmlParserCtxtPtr ctxt;
    xmlParserInputPtr inputStream;
    char *directory = NULL;
    xmlChar *uri;
    
    ctxt = xmlNewParserCtxt();
    if (ctxt == NULL) {
	return(NULL);
    }

    uri = xmlBuildURI(URL, base);

    if (uri == NULL) {
	inputStream = xmlLoadExternalEntity((char *)URL, (char *)ID, ctxt);
	if (inputStream == NULL) {
	    xmlFreeParserCtxt(ctxt);
	    return(NULL);
	}

	inputPush(ctxt, inputStream);

	if ((ctxt->directory == NULL) && (directory == NULL))
	    directory = xmlParserGetDirectory((char *)URL);
	if ((ctxt->directory == NULL) && (directory != NULL))
	    ctxt->directory = directory;
    } else {
	inputStream = xmlLoadExternalEntity((char *)uri, (char *)ID, ctxt);
	if (inputStream == NULL) {
	    xmlFree(uri);
	    xmlFreeParserCtxt(ctxt);
	    return(NULL);
	}

	inputPush(ctxt, inputStream);

	if ((ctxt->directory == NULL) && (directory == NULL))
	    directory = xmlParserGetDirectory((char *)uri);
	if ((ctxt->directory == NULL) && (directory != NULL))
	    ctxt->directory = directory;
	xmlFree(uri);
    }

    return(ctxt);
}

/************************************************************************
 *									*
 * 		Front ends when parsing from a file			*
 *									*
 ************************************************************************/

/**
 * xmlCreateFileParserCtxt:
 * @filename:  the filename
 *
 * Create a parser context for a file content. 
 * Automatic support for ZLIB/Compress compressed document is provided
 * by default if found at compile-time.
 *
 * Returns the new parser context or NULL
 */
xmlParserCtxtPtr
xmlCreateFileParserCtxt(const char *filename)
{
    xmlParserCtxtPtr ctxt;
    xmlParserInputPtr inputStream;
    char *directory = NULL;
    xmlChar *normalized;

    ctxt = xmlNewParserCtxt();
    if (ctxt == NULL) {
	if (xmlDefaultSAXHandler.error != NULL) {
	    xmlDefaultSAXHandler.error(NULL, "out of memory\n");
	}
	return(NULL);
    }

    normalized = xmlNormalizeWindowsPath((const xmlChar *) filename);
    if (normalized == NULL) {
	xmlFreeParserCtxt(ctxt);
	return(NULL);
    }
    inputStream = xmlLoadExternalEntity((char *) normalized, NULL, ctxt);
    if (inputStream == NULL) {
	xmlFreeParserCtxt(ctxt);
	xmlFree(normalized);
	return(NULL);
    }

    inputPush(ctxt, inputStream);
    if ((ctxt->directory == NULL) && (directory == NULL))
        directory = xmlParserGetDirectory((char *) normalized);
    if ((ctxt->directory == NULL) && (directory != NULL))
        ctxt->directory = directory;

    xmlFree(normalized);

    return(ctxt);
}

/**
 * xmlSAXParseFileWithData:
 * @sax:  the SAX handler block
 * @filename:  the filename
 * @recovery:  work in recovery mode, i.e. tries to read no Well Formed
 *             documents
 * @data:  the userdata
 *
 * parse an XML file and build a tree. Automatic support for ZLIB/Compress
 * compressed document is provided by default if found at compile-time.
 * It use the given SAX function block to handle the parsing callback.
 * If sax is NULL, fallback to the default DOM tree building routines.
 *
 * User data (void *) is stored within the parser context in the
 * context's _private member, so it is available nearly everywhere in libxml
 *
 * Returns the resulting document tree
 */

xmlDocPtr
xmlSAXParseFileWithData(xmlSAXHandlerPtr sax, const char *filename,
                        int recovery, void *data) {
    xmlDocPtr ret;
    xmlParserCtxtPtr ctxt;
    char *directory = NULL;

    xmlInitParser();

    ctxt = xmlCreateFileParserCtxt(filename);
    if (ctxt == NULL) {
	return(NULL);
    }
    if (sax != NULL) {
	if (ctxt->sax != NULL)
	    xmlFree(ctxt->sax);
        ctxt->sax = sax;
    }
    if (data!=NULL) {
	ctxt->_private=data;
    }

    if ((ctxt->directory == NULL) && (directory == NULL))
        directory = xmlParserGetDirectory(filename);
    if ((ctxt->directory == NULL) && (directory != NULL))
        ctxt->directory = (char *) xmlStrdup((xmlChar *) directory);

    ctxt->recovery = recovery;

    xmlParseDocument(ctxt);

    if ((ctxt->wellFormed) || recovery) ret = ctxt->myDoc;
    else {
       ret = NULL;
       xmlFreeDoc(ctxt->myDoc);
       ctxt->myDoc = NULL;
    }
    if (sax != NULL)
        ctxt->sax = NULL;
    xmlFreeParserCtxt(ctxt);
    
    return(ret);
}

/**
 * xmlSAXParseFile:
 * @sax:  the SAX handler block
 * @filename:  the filename
 * @recovery:  work in recovery mode, i.e. tries to read no Well Formed
 *             documents
 *
 * parse an XML file and build a tree. Automatic support for ZLIB/Compress
 * compressed document is provided by default if found at compile-time.
 * It use the given SAX function block to handle the parsing callback.
 * If sax is NULL, fallback to the default DOM tree building routines.
 *
 * Returns the resulting document tree
 */

xmlDocPtr
xmlSAXParseFile(xmlSAXHandlerPtr sax, const char *filename,
                          int recovery) {
    return(xmlSAXParseFileWithData(sax,filename,recovery,NULL));
}

/**
 * xmlRecoverDoc:
 * @cur:  a pointer to an array of xmlChar
 *
 * parse an XML in-memory document and build a tree.
 * In the case the document is not Well Formed, a tree is built anyway
 * 
 * Returns the resulting document tree
 */

xmlDocPtr
xmlRecoverDoc(xmlChar *cur) {
    return(xmlSAXParseDoc(NULL, cur, 1));
}

/**
 * xmlParseFile:
 * @filename:  the filename
 *
 * parse an XML file and build a tree. Automatic support for ZLIB/Compress
 * compressed document is provided by default if found at compile-time.
 *
 * Returns the resulting document tree if the file was wellformed,
 * NULL otherwise.
 */

xmlDocPtr
xmlParseFile(const char *filename) {
    return(xmlSAXParseFile(NULL, filename, 0));
}

/**
 * xmlRecoverFile:
 * @filename:  the filename
 *
 * parse an XML file and build a tree. Automatic support for ZLIB/Compress
 * compressed document is provided by default if found at compile-time.
 * In the case the document is not Well Formed, a tree is built anyway
 *
 * Returns the resulting document tree
 */

xmlDocPtr
xmlRecoverFile(const char *filename) {
    return(xmlSAXParseFile(NULL, filename, 1));
}


/**
 * xmlSetupParserForBuffer:
 * @ctxt:  an XML parser context
 * @buffer:  a xmlChar * buffer
 * @filename:  a file name
 *
 * Setup the parser context to parse a new buffer; Clears any prior
 * contents from the parser context. The buffer parameter must not be
 * NULL, but the filename parameter can be
 */
void
xmlSetupParserForBuffer(xmlParserCtxtPtr ctxt, const xmlChar* buffer,
                             const char* filename)
{
    xmlParserInputPtr input;

    input = xmlNewInputStream(ctxt);
    if (input == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		        "malloc");
        xmlFree(ctxt);
        return;
    }
  
    xmlClearParserCtxt(ctxt);
    if (filename != NULL)
        input->filename = xmlMemStrdup(filename);
    input->base = buffer;
    input->cur = buffer;
    input->end = &buffer[xmlStrlen(buffer)];
    inputPush(ctxt, input);
}

/**
 * xmlSAXUserParseFile:
 * @sax:  a SAX handler
 * @user_data:  The user data returned on SAX callbacks
 * @filename:  a file name
 *
 * parse an XML file and call the given SAX handler routines.
 * Automatic support for ZLIB/Compress compressed document is provided
 * 
 * Returns 0 in case of success or a error number otherwise
 */
int
xmlSAXUserParseFile(xmlSAXHandlerPtr sax, void *user_data,
                    const char *filename) {
    int ret = 0;
    xmlParserCtxtPtr ctxt;
    
    ctxt = xmlCreateFileParserCtxt(filename);
    if (ctxt == NULL) return -1;
    if (ctxt->sax != &xmlDefaultSAXHandler)
	xmlFree(ctxt->sax);
    ctxt->sax = sax;
    if (user_data != NULL)
	ctxt->userData = user_data;
    
    xmlParseDocument(ctxt);
    
    if (ctxt->wellFormed)
	ret = 0;
    else {
        if (ctxt->errNo != 0)
	    ret = ctxt->errNo;
	else
	    ret = -1;
    }
    if (sax != NULL)
	ctxt->sax = NULL;
    xmlFreeParserCtxt(ctxt);
    
    return ret;
}

/************************************************************************
 *									*
 * 		Front ends when parsing from memory			*
 *									*
 ************************************************************************/

/**
 * xmlCreateMemoryParserCtxt:
 * @buffer:  a pointer to a char array
 * @size:  the size of the array
 *
 * Create a parser context for an XML in-memory document.
 *
 * Returns the new parser context or NULL
 */
xmlParserCtxtPtr
xmlCreateMemoryParserCtxt(const char *buffer, int size) {
    xmlParserCtxtPtr ctxt;
    xmlParserInputPtr input;
    xmlParserInputBufferPtr buf;

    if (buffer == NULL)
	return(NULL);
    if (size <= 0)
	return(NULL);

    ctxt = xmlNewParserCtxt();
    if (ctxt == NULL)
	return(NULL);

    buf = xmlParserInputBufferCreateMem(buffer, size, XML_CHAR_ENCODING_NONE);
    if (buf == NULL) {
	xmlFreeParserCtxt(ctxt);
	return(NULL);
    }

    input = xmlNewInputStream(ctxt);
    if (input == NULL) {
	xmlFreeParserInputBuffer(buf);
	xmlFreeParserCtxt(ctxt);
	return(NULL);
    }

    input->filename = NULL;
    input->buf = buf;
    input->base = input->buf->buffer->content;
    input->cur = input->buf->buffer->content;
    input->end = &input->buf->buffer->content[input->buf->buffer->use];

    inputPush(ctxt, input);
    return(ctxt);
}

/**
 * xmlSAXParseMemoryWithData:
 * @sax:  the SAX handler block
 * @buffer:  an pointer to a char array
 * @size:  the size of the array
 * @recovery:  work in recovery mode, i.e. tries to read no Well Formed
 *             documents
 * @data:  the userdata
 *
 * parse an XML in-memory block and use the given SAX function block
 * to handle the parsing callback. If sax is NULL, fallback to the default
 * DOM tree building routines.
 *
 * User data (void *) is stored within the parser context in the
 * context's _private member, so it is available nearly everywhere in libxml
 *
 * Returns the resulting document tree
 */

xmlDocPtr
xmlSAXParseMemoryWithData(xmlSAXHandlerPtr sax, const char *buffer,
	          int size, int recovery, void *data) {
    xmlDocPtr ret;
    xmlParserCtxtPtr ctxt;

    ctxt = xmlCreateMemoryParserCtxt(buffer, size);
    if (ctxt == NULL) return(NULL);
    if (sax != NULL) {
	if (ctxt->sax != NULL)
	    xmlFree(ctxt->sax);
        ctxt->sax = sax;
    }
    if (data!=NULL) {
	ctxt->_private=data;
    }

    xmlParseDocument(ctxt);

    if ((ctxt->wellFormed) || recovery) ret = ctxt->myDoc;
    else {
       ret = NULL;
       xmlFreeDoc(ctxt->myDoc);
       ctxt->myDoc = NULL;
    }
    if (sax != NULL) 
	ctxt->sax = NULL;
    xmlFreeParserCtxt(ctxt);
    
    return(ret);
}

/**
 * xmlSAXParseMemory:
 * @sax:  the SAX handler block
 * @buffer:  an pointer to a char array
 * @size:  the size of the array
 * @recovery:  work in recovery mode, i.e. tries to read not Well Formed
 *             documents
 *
 * parse an XML in-memory block and use the given SAX function block
 * to handle the parsing callback. If sax is NULL, fallback to the default
 * DOM tree building routines.
 * 
 * Returns the resulting document tree
 */
xmlDocPtr
xmlSAXParseMemory(xmlSAXHandlerPtr sax, const char *buffer,
	          int size, int recovery) {
    return xmlSAXParseMemoryWithData(sax, buffer, size, recovery, NULL);
}

/**
 * xmlParseMemory:
 * @buffer:  an pointer to a char array
 * @size:  the size of the array
 *
 * parse an XML in-memory block and build a tree.
 * 
 * Returns the resulting document tree
 */

xmlDocPtr xmlParseMemory(const char *buffer, int size) {
   return(xmlSAXParseMemory(NULL, buffer, size, 0));
}

/**
 * xmlRecoverMemory:
 * @buffer:  an pointer to a char array
 * @size:  the size of the array
 *
 * parse an XML in-memory block and build a tree.
 * In the case the document is not Well Formed, a tree is built anyway
 * 
 * Returns the resulting document tree
 */

xmlDocPtr xmlRecoverMemory(const char *buffer, int size) {
   return(xmlSAXParseMemory(NULL, buffer, size, 1));
}

/**
 * xmlSAXUserParseMemory:
 * @sax:  a SAX handler
 * @user_data:  The user data returned on SAX callbacks
 * @buffer:  an in-memory XML document input
 * @size:  the length of the XML document in bytes
 *
 * A better SAX parsing routine.
 * parse an XML in-memory buffer and call the given SAX handler routines.
 * 
 * Returns 0 in case of success or a error number otherwise
 */
int xmlSAXUserParseMemory(xmlSAXHandlerPtr sax, void *user_data,
			  const char *buffer, int size) {
    int ret = 0;
    xmlParserCtxtPtr ctxt;
    xmlSAXHandlerPtr oldsax = NULL;
    
    if (sax == NULL) return -1;
    ctxt = xmlCreateMemoryParserCtxt(buffer, size);
    if (ctxt == NULL) return -1;
    oldsax = ctxt->sax;
    ctxt->sax = sax;
    if (user_data != NULL)
	ctxt->userData = user_data;
    
    xmlParseDocument(ctxt);
    
    if (ctxt->wellFormed)
	ret = 0;
    else {
        if (ctxt->errNo != 0)
	    ret = ctxt->errNo;
	else
	    ret = -1;
    }
    ctxt->sax = oldsax;
    xmlFreeParserCtxt(ctxt);
    
    return ret;
}

/**
 * xmlCreateDocParserCtxt:
 * @cur:  a pointer to an array of xmlChar
 *
 * Creates a parser context for an XML in-memory document.
 *
 * Returns the new parser context or NULL
 */
xmlParserCtxtPtr
xmlCreateDocParserCtxt(xmlChar *cur) {
    int len;

    if (cur == NULL)
	return(NULL);
    len = xmlStrlen(cur);
    return(xmlCreateMemoryParserCtxt((char *)cur, len));
}

/**
 * xmlSAXParseDoc:
 * @sax:  the SAX handler block
 * @cur:  a pointer to an array of xmlChar
 * @recovery:  work in recovery mode, i.e. tries to read no Well Formed
 *             documents
 *
 * parse an XML in-memory document and build a tree.
 * It use the given SAX function block to handle the parsing callback.
 * If sax is NULL, fallback to the default DOM tree building routines.
 * 
 * Returns the resulting document tree
 */

xmlDocPtr
xmlSAXParseDoc(xmlSAXHandlerPtr sax, xmlChar *cur, int recovery) {
    xmlDocPtr ret;
    xmlParserCtxtPtr ctxt;

    if (cur == NULL) return(NULL);


    ctxt = xmlCreateDocParserCtxt(cur);
    if (ctxt == NULL) return(NULL);
    if (sax != NULL) { 
        ctxt->sax = sax;
        ctxt->userData = NULL;
    }

    xmlParseDocument(ctxt);
    if ((ctxt->wellFormed) || recovery) ret = ctxt->myDoc;
    else {
       ret = NULL;
       xmlFreeDoc(ctxt->myDoc);
       ctxt->myDoc = NULL;
    }
    if (sax != NULL) 
	ctxt->sax = NULL;
    xmlFreeParserCtxt(ctxt);
    
    return(ret);
}

/**
 * xmlParseDoc:
 * @cur:  a pointer to an array of xmlChar
 *
 * parse an XML in-memory document and build a tree.
 * 
 * Returns the resulting document tree
 */

xmlDocPtr
xmlParseDoc(xmlChar *cur) {
    return(xmlSAXParseDoc(NULL, cur, 0));
}

/************************************************************************
 *									*
 * 	Specific function to keep track of entities references		*
 * 	and used by the XSLT debugger					*
 *									*
 ************************************************************************/

static xmlEntityReferenceFunc xmlEntityRefFunc = NULL;

/**
 * xmlAddEntityReference:
 * @ent : A valid entity
 * @firstNode : A valid first node for children of entity
 * @lastNode : A valid last node of children entity 
 *
 * Notify of a reference to an entity of type XML_EXTERNAL_GENERAL_PARSED_ENTITY
 */
static void
xmlAddEntityReference(xmlEntityPtr ent, xmlNodePtr firstNode,
                      xmlNodePtr lastNode)
{
    if (xmlEntityRefFunc != NULL) {
        (*xmlEntityRefFunc) (ent, firstNode, lastNode);
    }
}


/**
 * xmlSetEntityReferenceFunc:
 * @func: A valid function
 *
 * Set the function to call call back when a xml reference has been made
 */
void
xmlSetEntityReferenceFunc(xmlEntityReferenceFunc func)
{
    xmlEntityRefFunc = func;
}

/************************************************************************
 *									*
 * 				Miscellaneous				*
 *									*
 ************************************************************************/

#ifdef LIBXML_XPATH_ENABLED
#include <libxml/xpath.h>
#endif

extern void xmlGenericErrorDefaultFunc(void *ctx, const char *msg, ...);
static int xmlParserInitialized = 0;

/**
 * xmlInitParser:
 *
 * Initialization function for the XML parser.
 * This is not reentrant. Call once before processing in case of
 * use in multithreaded programs.
 */

void
xmlInitParser(void) {
    if (xmlParserInitialized != 0)
	return;

    if ((xmlGenericError == xmlGenericErrorDefaultFunc) ||
	(xmlGenericError == NULL))
	initGenericErrorDefaultFunc(NULL);
    xmlInitThreads();
    xmlInitMemory();
    xmlInitCharEncodingHandlers();
    xmlInitializePredefinedEntities();
    xmlDefaultSAXHandlerInit();
    xmlRegisterDefaultInputCallbacks();
    xmlRegisterDefaultOutputCallbacks();
#ifdef LIBXML_HTML_ENABLED
    htmlInitAutoClose();
    htmlDefaultSAXHandlerInit();
#endif
#ifdef LIBXML_XPATH_ENABLED
    xmlXPathInit();
#endif
    xmlParserInitialized = 1;
}

/**
 * xmlCleanupParser:
 *
 * Cleanup function for the XML parser. It tries to reclaim all
 * parsing related global memory allocated for the parser processing.
 * It doesn't deallocate any document related memory. Calling this
 * function should not prevent reusing the parser.
 */

void
xmlCleanupParser(void) {
    xmlCleanupCharEncodingHandlers();
    xmlCleanupPredefinedEntities();
#ifdef LIBXML_CATALOG_ENABLED
    xmlCatalogCleanup();
#endif
    xmlCleanupThreads();
    xmlParserInitialized = 0;
}
