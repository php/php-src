/*
 * DOCBparser.c : an attempt to parse SGML Docbook documents
 *
 * This is extremely hackish. It also adds one extension
 *    <?sgml-declaration encoding="ISO-8859-1"?>
 * allowing to store the encoding of the document within the instance.
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */

#define IN_LIBXML
#include "libxml.h"
#ifdef LIBXML_DOCB_ENABLED

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
#include <libxml/SAX.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/xmlerror.h>
#include <libxml/DOCBparser.h>
#include <libxml/entities.h>
#include <libxml/encoding.h>
#include <libxml/valid.h>
#include <libxml/xmlIO.h>
#include <libxml/uri.h>
#include <libxml/globals.h>

/*
 * DocBook XML current versions
 */

#define XML_DOCBOOK_XML_PUBLIC (const xmlChar *)			\
             "-//OASIS//DTD DocBook XML V4.1.2//EN"
#define XML_DOCBOOK_XML_SYSTEM (const xmlChar *)			\
             "http://www.oasis-open.org/docbook/xml/4.1.2/docbookx.dtd"

/*
 * Internal description of an SGML entity
 */
typedef struct _docbEntityDesc docbEntityDesc;
typedef docbEntityDesc *docbEntityDescPtr;
struct _docbEntityDesc {
    int value;         /* the UNICODE value for the character */
    const char *name;  /* The entity name */
    const char *desc;   /* the description */
};

static int             docbParseCharRef(docbParserCtxtPtr ctxt);
static xmlEntityPtr    docbParseEntityRef(docbParserCtxtPtr ctxt,
                                        xmlChar **str);
static void            docbParseElement(docbParserCtxtPtr ctxt);
static void            docbParseContent(docbParserCtxtPtr ctxt);

/*
 * Internal description of an SGML element
 */
typedef struct _docbElemDesc docbElemDesc;
typedef docbElemDesc *docbElemDescPtr;
struct _docbElemDesc {
    const char *name;  /* The tag name */
    int startTag;       /* Whether the start tag can be implied */
    int endTag;         /* Whether the end tag can be implied */
    int empty;          /* Is this an empty element ? */
    int depr;           /* Is this a deprecated element ? */
    int dtd;            /* 1: only in Loose DTD, 2: only Frameset one */
    const char *desc;   /* the description */
};


#define DOCB_MAX_NAMELEN 1000
#define DOCB_PARSER_BIG_BUFFER_SIZE 1000
#define DOCB_PARSER_BUFFER_SIZE 100

/* #define DEBUG */
/* #define DEBUG_PUSH */

/************************************************************************
 *                                                                     *
 *             Parser stacks related functions and macros              *
 *                                                                     *
 ************************************************************************/

/**
 * docbnamePush:
 * @ctxt:  a DocBook SGML parser context
 * @value:  the element name
 *
 * Pushes a new element name on top of the name stack
 *
 * Returns 0 in case of error, the index in the stack otherwise
 */
static int
docbnamePush(docbParserCtxtPtr ctxt, xmlChar * value)
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
 * docbnamePop:
 * @ctxt: a DocBook SGML parser context
 *
 * Pops the top element name from the name stack
 *
 * Returns the name just removed
 */
static xmlChar *
docbnamePop(docbParserCtxtPtr ctxt)
{
    xmlChar *ret;

    if (ctxt->nameNr < 0)
        return (0);
    ctxt->nameNr--;
    if (ctxt->nameNr < 0)
        return (0);
    if (ctxt->nameNr > 0)
        ctxt->name = ctxt->nameTab[ctxt->nameNr - 1];
    else
        ctxt->name = NULL;
    ret = ctxt->nameTab[ctxt->nameNr];
    ctxt->nameTab[ctxt->nameNr] = 0;
    return (ret);
}

/*
 * Macros for accessing the content. Those should be used only by the parser,
 * and not exported.
 *
 * Dirty macros, i.e. one need to make assumption on the context to use them
 *
 *   CUR_PTR return the current pointer to the xmlChar to be parsed.
 *   CUR     returns the current xmlChar value, i.e. a 8 bit value if compiled
 *           in ISO-Latin or UTF-8, and the current 16 bit value if compiled
 *           in UNICODE mode. This should be used internally by the parser
 *           only to compare to ASCII values otherwise it would break when
 *           running with UTF-8 encoding.
 *   NXT(n)  returns the n'th next xmlChar. Same as CUR is should be used only
 *           to compare on ASCII based substring.
 *   UPP(n)  returns the n'th next xmlChar converted to uppercase. Same as CUR
 *           it should be used only to compare on ASCII based substring.
 *   SKIP(n) Skip n xmlChar, and must also be used only to skip ASCII defined
 *           strings within the parser.
 *
 * Clean macros, not dependent of an ASCII context, expect UTF-8 encoding
 *
 *   CURRENT Returns the current char value, with the full decoding of
 *           UTF-8 if we are using this mode. It returns an int.
 *   NEXT    Skip to the next character, this does the proper decoding
 *           in UTF-8 mode. It also pop-up unfinished entities on the fly.
 *   COPY(to) copy one char to *to, increment CUR_PTR and to accordingly
 */

#define UPPER (toupper(*ctxt->input->cur))

#define SKIP(val) ctxt->nbChars += (val),ctxt->input->cur += (val)

#define NXT(val) ctxt->input->cur[(val)]

#define UPP(val) (toupper(ctxt->input->cur[(val)]))

#define CUR_PTR ctxt->input->cur

#define SHRINK  xmlParserInputShrink(ctxt->input)

#define GROW  xmlParserInputGrow(ctxt->input, INPUT_CHUNK)

#define CURRENT ((int) (*ctxt->input->cur))

#define SKIP_BLANKS docbSkipBlankChars(ctxt)

/* Imported from XML */

/* #define CUR (ctxt->token ? ctxt->token : (int) (*ctxt->input->cur)) */
#define CUR ((int) (*ctxt->input->cur))
#define NEXT xmlNextChar(ctxt),ctxt->nbChars++

#define RAW (ctxt->token ? -1 : (*ctxt->input->cur))
#define NXT(val) ctxt->input->cur[(val)]
#define CUR_PTR ctxt->input->cur


#define NEXTL(l) do {                                                  \
    if (*(ctxt->input->cur) == '\n') {                                 \
       ctxt->input->line++; ctxt->input->col = 1;                      \
    } else ctxt->input->col++;                                         \
    ctxt->token = 0; ctxt->input->cur += l; ctxt->nbChars++;           \
  } while (0)
    
/************
    \
    if (*ctxt->input->cur == '%') xmlParserHandlePEReference(ctxt);    \
    if (*ctxt->input->cur == '&') xmlParserHandleReference(ctxt);
 ************/

#define CUR_CHAR(l) docbCurrentChar(ctxt, &l)
#define CUR_SCHAR(s, l) xmlStringCurrentChar(ctxt, s, &l)

#define COPY_BUF(l,b,i,v)                                              \
    if (l == 1) b[i++] = (xmlChar) v;                                  \
    else i += xmlCopyChar(l,&b[i],v)

/**
 * docbCurrentChar:
 * @ctxt:  the DocBook SGML parser context
 * @len:  pointer to the length of the char read
 *
 * The current char value, if using UTF-8 this may actually span multiple
 * bytes in the input buffer. Implement the end of line normalization:
 * 2.11 End-of-Line Handling
 * If the encoding is unspecified, in the case we find an ISO-Latin-1
 * char, then the encoding converter is plugged in automatically.
 *
 * Returns the current char value and its length
 */

static int
docbCurrentChar(xmlParserCtxtPtr ctxt, int *len) {
    if (ctxt->instate == XML_PARSER_EOF)
       return(0);

    if (ctxt->token != 0) {
       *len = 0;
       return(ctxt->token);
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
               ctxt->errNo = XML_ERR_INVALID_ENCODING;
               if ((ctxt->sax != NULL) &&
                   (ctxt->sax->error != NULL))
                   ctxt->sax->error(ctxt->userData, 
                                    "Char 0x%X out of allowed range\n", val);
               ctxt->wellFormed = 0;
               if (ctxt->recovery == 0) ctxt->disableSAX = 1;
           }    
           return(val);
       } else {
           /* 1-byte code */
           *len = 1;
           return((int) *ctxt->input->cur);
       }
    }
    /*
     * Assume it's a fixed length encoding (1) with
     * a compatible encoding for the ASCII set, since
     * XML constructs only use < 128 chars
     */
    *len = 1;
    if ((int) *ctxt->input->cur < 0x80)
       return((int) *ctxt->input->cur);

    /*
     * Humm this is bad, do an automatic flow conversion
     */
    xmlSwitchEncoding(ctxt, XML_CHAR_ENCODING_8859_1);
    ctxt->charset = XML_CHAR_ENCODING_UTF8;
    return(xmlCurrentChar(ctxt, len));

encoding_error:
    /*
     * If we detect an UTF8 error that probably mean that the
     * input encoding didn't get properly advertized in the
     * declaration header. Report the error and switch the encoding
     * to ISO-Latin-1 (if you don't like this policy, just declare the
     * encoding !)
     */
    ctxt->errNo = XML_ERR_INVALID_ENCODING;
    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL)) {
       ctxt->sax->error(ctxt->userData, 
                        "Input is not proper UTF-8, indicate encoding !\n");
       ctxt->sax->error(ctxt->userData, "Bytes: 0x%02X 0x%02X 0x%02X 0x%02X\n",
                       ctxt->input->cur[0], ctxt->input->cur[1],
                       ctxt->input->cur[2], ctxt->input->cur[3]);
    }

    ctxt->charset = XML_CHAR_ENCODING_8859_1; 
    *len = 1;
    return((int) *ctxt->input->cur);
}

#if 0
/**
 * sgmlNextChar:
 * @ctxt:  the DocBook SGML parser context
 *
 * Skip to the next char input char.
 */

static void
sgmlNextChar(docbParserCtxtPtr ctxt) {
    if (ctxt->instate == XML_PARSER_EOF)
       return;
    if ((*ctxt->input->cur == 0) &&
        (xmlParserInputGrow(ctxt->input, INPUT_CHUNK) <= 0)) {
           xmlPopInput(ctxt);
    } else {
        if (*(ctxt->input->cur) == '\n') {
           ctxt->input->line++; ctxt->input->col = 1;
       } else ctxt->input->col++;
       ctxt->input->cur++;
       ctxt->nbChars++;
        if (*ctxt->input->cur == 0)
           xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
    }
}
#endif

/**
 * docbSkipBlankChars:
 * @ctxt:  the DocBook SGML parser context
 *
 * skip all blanks character found at that point in the input streams.
 *
 * Returns the number of space chars skipped
 */

static int
docbSkipBlankChars(xmlParserCtxtPtr ctxt) {
    int res = 0;

    while (IS_BLANK(*(ctxt->input->cur))) {
       if ((*ctxt->input->cur == 0) &&
           (xmlParserInputGrow(ctxt->input, INPUT_CHUNK) <= 0)) {
               xmlPopInput(ctxt);
       } else {
           if (*(ctxt->input->cur) == '\n') {
               ctxt->input->line++; ctxt->input->col = 1;
           } else ctxt->input->col++;
           ctxt->input->cur++;
           ctxt->nbChars++;
           if (*ctxt->input->cur == 0)
               xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
       }
       res++;
    }
    return(res);
}



/************************************************************************
 *                                                                     *
 *             The list of SGML elements and their properties          *
 *                                                                     *
 ************************************************************************/

/*
 *  Start Tag: 1 means the start tag can be ommited
 *  End Tag:   1 means the end tag can be ommited
 *             2 means it's forbidden (empty elements)
 *  Depr:      this element is deprecated
 *  DTD:       1 means that this element is valid only in the Loose DTD
 *             2 means that this element is valid only in the Frameset DTD
 *
 * Name,Start Tag,End Tag,  Empty,  Depr.,    DTD, Description
 */
static docbElemDesc
docbookElementTable[] = {
{ "abbrev",    0,      0,      0,      3,      0, "" }, /* word */
{ "abstract",  0,      0,      0,      9,      0, "" }, /* title */
{ "accel",     0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "ackno",     0,      0,      0,      4,      0, "" }, /* docinfo */
{ "acronym",   0,      0,      0,      3,      0, "" }, /* word */
{ "action",    0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "address",   0,      0,      0,      1,      0, "" },
{ "affiliation",0,     0,      0,      9,      0, "" }, /* shortaffil */
{ "alt",       0,      0,      0,      1,      0, "" },
{ "anchor",    0,      2,      1,      0,      0, "" },
{ "answer",    0,      0,      0,      9,      0, "" }, /* label */
{ "appendix",  0,      0,      0,      9,      0, "" }, /* appendixinfo */
{ "appendixinfo",0,    0,      0,      9,      0, "" }, /* graphic */
{ "application",0,     0,      0,      2,      0, "" }, /* para */
{ "area",      0,      2,      1,      0,      0, "" },
{ "areaset",   0,      0,      0,      9,      0, "" }, /* area */
{ "areaspec",  0,      0,      0,      9,      0, "" }, /* area */
{ "arg",       0,      0,      0,      1,      0, "" },
{ "artheader", 0,      0,      0,      9,      0, "" },
{ "article",   0,      0,      0,      9,      0, "" }, /* div.title.content */
{ "articleinfo",0,     0,      0,      9,      0, "" }, /* graphic */
{ "artpagenums",0,     0,      0,      4,      0, "" }, /* docinfo */
{ "attribution",0,     0,      0,      2,      0, "" }, /* para */
{ "audiodata", 0,      2,      1,      0,      0, "" },
{ "audioobject",0,     0,      0,      9,      0, "" }, /* objectinfo */
{ "authorblurb",0,     0,      0,      9,      0, "" }, /* title */
{ "authorgroup",0,     0,      0,      9,      0, "" }, /* author */
{ "authorinitials",0,  0,      0,      4,      0, "" }, /* docinfo */
{ "author",    0,      0,      0,      9,      0, "" }, /* person.ident.mix */
{ "beginpage", 0,      2,      1,      0,      0, "" },
{ "bibliodiv", 0,      0,      0,      9,      0, "" }, /* sect.title.content */
{ "biblioentry",0,     0,      0,      9,      0, "" }, /* articleinfo */
{ "bibliography",0,    0,      0,      9,      0, "" }, /* bibliographyinfo */
{ "bibliographyinfo",0,        0,      0,      9,      0, "" }, /* graphic */
{ "bibliomisc",        0,      0,      0,      2,      0, "" }, /* para */
{ "bibliomixed",0,     0,      0,      1,      0, "" }, /* %bibliocomponent.mix, bibliomset) */
{ "bibliomset",        0,      0,      0,      1,      0, "" }, /* %bibliocomponent.mix; | bibliomset) */
{ "biblioset", 0,      0,      0,      9,      0, "" }, /* bibliocomponent.mix */
{ "blockquote",        0,      0,      0,      9,      0, "" }, /* title */
{ "book",      0,      0,      0,      9,      0, "" }, /* div.title.content */
{ "bookinfo",  0,      0,      0,      9,      0, "" }, /* graphic */
{ "bridgehead",        0,      0,      0,      8,      0, "" }, /* title */
{ "callout",   0,      0,      0,      9,      0, "" }, /* component.mix */
{ "calloutlist",0,     0,      0,      9,      0, "" }, /* formalobject.title.content */
{ "caption",   0,      0,      0,      9,      0, "" }, /* textobject.mix */
{ "caution",   0,      0,      0,      9,      0, "" }, /* title */
{ "chapter",   0,      0,      0,      9,      0, "" }, /* chapterinfo */
{ "chapterinfo",0,     0,      0,      9,      0, "" }, /* graphic */
{ "citation",  0,      0,      0,      2,      0, "" }, /* para */
{ "citerefentry",0,    0,      0,      9,      0, "" }, /* refentrytitle */
{ "citetitle", 0,      0,      0,      2,      0, "" }, /* para */
{ "city",      0,      0,      0,      4,      0, "" }, /* docinfo */
{ "classname", 0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "classsynopsisinfo",0,0,     0,      9,      0, "" }, /* cptr */
{ "classsynopsis",0,   0,      0,      9,      0, "" }, /* ooclass */
{ "cmdsynopsis",0,     0,      0,      9,      0, "" }, /* command */
{ "co",                0,      2,      1,      0,      0, "" },
{ "collab",    0,      0,      0,      9,      0, "" }, /* collabname */
{ "collabname",        0,      0,      0,      4,      0, "" }, /* docinfo */
{ "colophon",  0,      0,      0,      9,      0, "" }, /* sect.title.content */
{ "colspec",   0,      2,      1,      0,      0, "" },
{ "colspec",   0,      2,      1,      0,      0, "" },
{ "command",   0,      0,      0,      9,      0, "" }, /* cptr */
{ "computeroutput",0,  0,      0,      9,      0, "" }, /* cptr */
{ "confdates", 0,      0,      0,      4,      0, "" }, /* docinfo */
{ "confgroup", 0,      0,      0,      9,      0, "" }, /* confdates */
{ "confnum",   0,      0,      0,      4,      0, "" }, /* docinfo */
{ "confsponsor",0,     0,      0,      4,      0, "" }, /* docinfo */
{ "conftitle", 0,      0,      0,      4,      0, "" }, /* docinfo */
{ "constant",  0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "constructorsynopsis",0,0,   0,      9,      0, "" }, /* modifier */
{ "contractnum",0,     0,      0,      4,      0, "" }, /* docinfo */
{ "contractsponsor",0, 0,      0,      4,      0, "" }, /* docinfo */
{ "contrib",   0,      0,      0,      4,      0, "" }, /* docinfo */
{ "copyright", 0,      0,      0,      9,      0, "" }, /* year */
{ "corpauthor",        0,      0,      0,      4,      0, "" }, /* docinfo */
{ "corpname",  0,      0,      0,      4,      0, "" }, /* docinfo */
{ "country",   0,      0,      0,      4,      0, "" }, /* docinfo */
{ "database",  0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "date",      0,      0,      0,      4,      0, "" }, /* docinfo */
{ "dedication",        0,      0,      0,      9,      0, "" }, /* sect.title.content */
{ "destructorsynopsis",0,0,    0,      9,      0, "" }, /* modifier */
{ "docinfo",   0,      0,      0,      9,      0, "" },
{ "edition",   0,      0,      0,      4,      0, "" }, /* docinfo */
{ "editor",    0,      0,      0,      9,      0, "" }, /* person.ident.mix */
{ "email",     0,      0,      0,      4,      0, "" }, /* docinfo */
{ "emphasis",  0,      0,      0,      2,      0, "" }, /* para */
{ "entry",     0,      0,      0,      9,      0, "" }, /* tbl.entry.mdl */
{ "entrytbl",  0,      0,      0,      9,      0, "" }, /* tbl.entrytbl.mdl */
{ "envar",     0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "epigraph",  0,      0,      0,      9,      0, "" }, /* attribution */
{ "equation",  0,      0,      0,      9,      0, "" }, /* formalobject.title.content */
{ "errorcode", 0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "errorname", 0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "errortype", 0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "example",   0,      0,      0,      9,      0, "" }, /* formalobject.title.content */
{ "exceptionname",0,   0,      0,      7,      0, "" }, /* smallcptr */
{ "fax",       0,      0,      0,      4,      0, "" }, /* docinfo */
{ "fieldsynopsis",     0,      0,      0,      9,      0, "" }, /* modifier */
{ "figure",    0,      0,      0,      9,      0, "" }, /* formalobject.title.content */
{ "filename",  0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "firstname", 0,      0,      0,      4,      0, "" }, /* docinfo */
{ "firstterm", 0,      0,      0,      3,      0, "" }, /* word */
{ "footnote",  0,      0,      0,      9,      0, "" }, /* footnote.mix */
{ "footnoteref",0,     2,      1,      0,      0, "" },
{ "foreignphrase",0,   0,      0,      2,      0, "" }, /* para */
{ "formalpara",        0,      0,      0,      9,      0, "" }, /* title */
{ "funcdef",   0,      0,      0,      1,      0, "" },
{ "funcparams",        0,      0,      0,      9,      0, "" }, /* cptr */
{ "funcprototype",0,   0,      0,      9,      0, "" }, /* funcdef */
{ "funcsynopsis",0,    0,      0,      9,      0, "" }, /* funcsynopsisinfo */
{ "funcsynopsisinfo",  0,      0,      0,      9,      0, "" }, /* cptr */
{ "function",  0,      0,      0,      9,      0, "" }, /* cptr */
{ "glossary",  0,      0,      0,      9,      0, "" }, /* glossaryinfo */
{ "glossaryinfo",0,    0,      0,      9,      0, "" }, /* graphic */
{ "glossdef",  0,      0,      0,      9,      0, "" }, /* glossdef.mix */
{ "glossdiv",  0,      0,      0,      9,      0, "" }, /* sect.title.content */
{ "glossentry",        0,      0,      0,      9,      0, "" }, /* glossterm */
{ "glosslist", 0,      0,      0,      9,      0, "" }, /* glossentry */
{ "glossseealso",0,    0,      1,      2,      0, "" }, /* para */
{ "glosssee",  0,      0,      1,      2,      0, "" }, /* para */
{ "glossterm", 0,      0,      0,      2,      0, "" }, /* para */
{ "graphic",   0,      0,      0,      9,      0, "" },
{ "graphicco", 0,      0,      0,      9,      0, "" }, /* areaspec */
{ "group",     0,      0,      0,      9,      0, "" }, /* arg */
{ "guibutton", 0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "guiicon",   0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "guilabel",  0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "guimenuitem",0,     0,      0,      7,      0, "" }, /* smallcptr */
{ "guimenu",   0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "guisubmenu",        0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "hardware",  0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "highlights",        0,      0,      0,      9,      0, "" }, /* highlights.mix */
{ "holder",    0,      0,      0,      4,      0, "" }, /* docinfo */
{ "honorific", 0,      0,      0,      4,      0, "" }, /* docinfo */
{ "imagedata", 0,      2,      1,      0,      0, "" },
{ "imageobjectco",0,   0,      0,      9,      0, "" }, /* areaspec */
{ "imageobject",0,     0,      0,      9,      0, "" }, /* objectinfo */
{ "important", 0,      0,      0,      9,      0, "" }, /* title */
{ "indexdiv",  0,      0,      0,      9,      0, "" }, /* sect.title.content */
{ "indexentry",        0,      0,      0,      9,      0, "" }, /* primaryie */
{ "index",     0,      0,      0,      9,      0, "" }, /* indexinfo */
{ "indexinfo", 0,      0,      0,      9,      0, "" }, /* graphic */
{ "indexterm", 0,      0,      0,      9,      0, "" }, /* primary */
{ "informalequation",0,        0,      0,      9,      0, "" }, /* equation.content */
{ "informalexample",0, 0,      0,      9,      0, "" }, /* example.mix */
{ "informalfigure",0,  0,      0,      9,      0, "" }, /* figure.mix */
{ "informaltable",0,   0,      0,      9,      0, "" }, /* graphic */
{ "initializer",0,     0,      0,      7,      0, "" }, /* smallcptr */
{ "inlineequation",0,  0,      0,      9,      0, "" }, /* inlineequation.content */
{ "inlinegraphic",0,   0,      0,      9,      0, "" },
{ "inlinemediaobject",0,0,     0,      9,      0, "" }, /* objectinfo */
{ "interfacename",0,   0,      0,      7,      0, "" }, /* smallcptr */
{ "interface", 0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "invpartnumber",0,   0,      0,      4,      0, "" }, /* docinfo */
{ "isbn",      0,      0,      0,      4,      0, "" }, /* docinfo */
{ "issn",      0,      0,      0,      4,      0, "" }, /* docinfo */
{ "issuenum",  0,      0,      0,      4,      0, "" }, /* docinfo */
{ "itemizedlist",0,    0,      0,      9,      0, "" }, /* formalobject.title.content */
{ "itermset",  0,      0,      0,      9,      0, "" }, /* indexterm */
{ "jobtitle",  0,      0,      0,      4,      0, "" }, /* docinfo */
{ "keycap",    0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "keycode",   0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "keycombo",  0,      0,      0,      9,      0, "" }, /* keycap */
{ "keysym",    0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "keyword",   0,      0,      0,      1,      0, "" },
{ "keywordset",        0,      0,      0,      9,      0, "" }, /* keyword */
{ "label",     0,      0,      0,      3,      0, "" }, /* word */
{ "legalnotice",0,     0,      0,      9,      0, "" }, /* title */
{ "lineage",   0,      0,      0,      4,      0, "" }, /* docinfo */
{ "lineannotation",0,  0,      0,      2,      0, "" }, /* para */
{ "link",      0,      0,      0,      2,      0, "" }, /* para */
{ "listitem",  0,      0,      0,      9,      0, "" }, /* component.mix */
{ "literal",   0,      0,      0,      9,      0, "" }, /* cptr */
{ "literallayout",0,   0,      0,      2,      0, "" }, /* para */
{ "lot",       0,      0,      0,      9,      0, "" }, /* bookcomponent.title.content */
{ "lotentry",  0,      0,      0,      2,      0, "" }, /* para */
{ "manvolnum", 0,      0,      0,      3,      0, "" }, /* word */
{ "markup",    0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "medialabel",        0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "mediaobjectco",0,   0,      0,      9,      0, "" }, /* objectinfo */
{ "mediaobject",0,     0,      0,      9,      0, "" }, /* objectinfo */
{ "member",    0,      0,      0,      2,      0, "" }, /* para */
{ "menuchoice",        0,      0,      0,      9,      0, "" }, /* shortcut */
{ "methodname",        0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "methodparam",0,     0,      0,      9,      0, "" }, /* modifier */
{ "methodsynopsis",0,  0,      0,      9,      0, "" }, /* modifier */
{ "modespec",  0,      0,      0,      4,      0, "" }, /* docinfo */
{ "modifier",  0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "mousebutton",0,     0,      0,      7,      0, "" }, /* smallcptr */
{ "msgaud",    0,      0,      0,      2,      0, "" }, /* para */
{ "msgentry",  0,      0,      0,      9,      0, "" }, /* msg */
{ "msgexplan", 0,      0,      0,      9,      0, "" }, /* title */
{ "msginfo",   0,      0,      0,      9,      0, "" }, /* msglevel */
{ "msglevel",  0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "msgmain",   0,      0,      0,      9,      0, "" }, /* title */
{ "msgorig",   0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "msgrel",    0,      0,      0,      9,      0, "" }, /* title */
{ "msgset",    0,      0,      0,      9,      0, "" }, /* formalobject.title.content */
{ "msgsub",    0,      0,      0,      9,      0, "" }, /* title */
{ "msgtext",   0,      0,      0,      9,      0, "" }, /* component.mix */
{ "msg",       0,      0,      0,      9,      0, "" }, /* title */
{ "note",      0,      0,      0,      9,      0, "" }, /* title */
{ "objectinfo",        0,      0,      0,      9,      0, "" }, /* graphic */
{ "olink",     0,      0,      0,      2,      0, "" }, /* para */
{ "ooclass",   0,      0,      0,      9,      0, "" }, /* modifier */
{ "ooexception",0,     0,      0,      9,      0, "" }, /* modifier */
{ "oointerface",0,     0,      0,      9,      0, "" }, /* modifier */
{ "optional",  0,      0,      0,      9,      0, "" }, /* cptr */
{ "option",    0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "orderedlist",0,     0,      0,      9,      0, "" }, /* formalobject.title.content */
{ "orgdiv",    0,      0,      0,      4,      0, "" }, /* docinfo */
{ "orgname",   0,      0,      0,      4,      0, "" }, /* docinfo */
{ "otheraddr", 0,      0,      0,      4,      0, "" }, /* docinfo */
{ "othercredit",0,     0,      0,      9,      0, "" }, /* person.ident.mix */
{ "othername", 0,      0,      0,      4,      0, "" }, /* docinfo */
{ "pagenums",  0,      0,      0,      4,      0, "" }, /* docinfo */
{ "paramdef",  0,      0,      0,      1,      0, "" },
{ "parameter", 0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "para",      0,      0,      0,      2,      0, "" }, /* para */
{ "partinfo",  0,      0,      0,      9,      0, "" }, /* graphic */
{ "partintro", 0,      0,      0,      9,      0, "" }, /* div.title.content */
{ "part",      0,      0,      0,      9,      0, "" }, /* partinfo */
{ "phone",     0,      0,      0,      4,      0, "" }, /* docinfo */
{ "phrase",    0,      0,      0,      2,      0, "" }, /* para */
{ "pob",       0,      0,      0,      4,      0, "" }, /* docinfo */
{ "postcode",  0,      0,      0,      4,      0, "" }, /* docinfo */
{ "prefaceinfo",0,     0,      0,      9,      0, "" }, /* graphic */
{ "preface",   0,      0,      0,      9,      0, "" }, /* prefaceinfo */
{ "primaryie", 0,      0,      0,      4,      0, "" }, /* ndxterm */
{ "primary", 0,      0,      0,      9,      0, "" }, /* ndxterm */
{ "printhistory",0,    0,      0,      9,      0, "" }, /* para.class */
{ "procedure", 0,      0,      0,      9,      0, "" }, /* formalobject.title.content */
{ "productname",0,     0,      0,      2,      0, "" }, /* para */
{ "productnumber",0,   0,      0,      4,      0, "" }, /* docinfo */
{ "programlistingco",0,        0,      0,      9,      0, "" }, /* areaspec */
{ "programlisting",0,  0,      0,      2,      0, "" }, /* para */
{ "prompt",    0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "property",  0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "pubdate",   0,      0,      0,      4,      0, "" }, /* docinfo */
{ "publishername",0,   0,      0,      4,      0, "" }, /* docinfo */
{ "publisher", 0,      0,      0,      9,      0, "" }, /* publishername */
{ "pubsnumber",        0,      0,      0,      4,      0, "" }, /* docinfo */
{ "qandadiv",  0,      0,      0,      9,      0, "" }, /* formalobject.title.content */
{ "qandaentry",        0,      0,      0,      9,      0, "" }, /* revhistory */
{ "qandaset",  0,      0,      0,      9,      0, "" }, /* formalobject.title.content */
{ "question",  0,      0,      0,      9,      0, "" }, /* label */
{ "quote",     0,      0,      0,      2,      0, "" }, /* para */
{ "refclass",  0,      0,      0,      9,      0, "" }, /* refclass.char.mix */
{ "refdescriptor",0,   0,      0,      9,      0, "" }, /* refname.char.mix */
{ "refentryinfo",0,    0,      0,      9,      0, "" }, /* graphic */
{ "refentry",  0,      0,      0,      9,      0, "" }, /* ndxterm.class */
{ "refentrytitle",0,   0,      0,      2,      0, "" }, /* para */
{ "referenceinfo",0,   0,      0,      9,      0, "" }, /* graphic */
{ "reference", 0,      0,      0,      9,      0, "" }, /* referenceinfo */
{ "refmeta",   0,      0,      0,      9,      0, "" }, /* ndxterm.class */
{ "refmiscinfo",0,     0,      0,      4,      0, "" }, /* docinfo */
{ "refnamediv",        0,      0,      0,      9,      0, "" }, /* refdescriptor */
{ "refname",   0,      0,      0,      9,      0, "" }, /* refname.char.mix */
{ "refpurpose",        0,      0,      0,      9,      0, "" }, /* refinline.char.mix */
{ "refsect1info",0,    0,      0,      9,      0, "" }, /* graphic */
{ "refsect1",  0,      0,      0,      9,      0, "" }, /* refsect */
{ "refsect2info",0,    0,      0,      9,      0, "" }, /* graphic */
{ "refsect2",  0,      0,      0,      9,      0, "" }, /* refsect */
{ "refsect3info",0,    0,      0,      9,      0, "" }, /* graphic */
{ "refsect3",  0,      0,      0,      9,      0, "" }, /* refsect */
{ "refsynopsisdivinfo",0,0,    0,      9,      0, "" }, /* graphic */
{ "refsynopsisdiv",0,  0,      0,      9,      0, "" }, /* refsynopsisdivinfo */
{ "releaseinfo",0,     0,      0,      4,      0, "" }, /* docinfo */
{ "remark",    0,      0,      0,      2,      0, "" }, /* para */
{ "replaceable",0,     0,      0,      1,      0, "" },
{ "returnvalue",0,     0,      0,      7,      0, "" }, /* smallcptr */
{ "revdescription",0,  0,      0,      9,      0, "" }, /* revdescription.mix */
{ "revhistory",        0,      0,      0,      9,      0, "" }, /* revision */
{ "revision",  0,      0,      0,      9,      0, "" }, /* revnumber */
{ "revnumber", 0,      0,      0,      4,      0, "" }, /* docinfo */
{ "revremark", 0,      0,      0,      4,      0, "" }, /* docinfo */
{ "row",       0,      0,      0,      9,      0, "" }, /* tbl.row.mdl */
{ "row",       0,      0,      0,      9,      0, "" }, /* tbl.row.mdl */
{ "sbr",       0,      2,      1,      0,      0, "" },
{ "screenco",  0,      0,      0,      9,      0, "" }, /* areaspec */
{ "screeninfo",        0,      0,      0,      2,      0, "" }, /* para */
{ "screen",    0,      0,      0,      2,      0, "" }, /* para */
{ "screenshot",        0,      0,      0,      9,      0, "" }, /* screeninfo */
{ "secondaryie",0,     0,      0,      4,      0, "" }, /* ndxterm */
{ "secondary", 0,      0,      0,      4,      0, "" }, /* ndxterm */
{ "sect1info", 0,      0,      0,      9,      0, "" }, /* graphic */
{ "sect1",     0,      0,      0,      9,      0, "" }, /* sect */
{ "sect2info", 0,      0,      0,      9,      0, "" }, /* graphic */
{ "sect2",     0,      0,      0,      9,      0, "" }, /* sect */
{ "sect3info", 0,      0,      0,      9,      0, "" }, /* graphic */
{ "sect3",     0,      0,      0,      9,      0, "" }, /* sect */
{ "sect4info", 0,      0,      0,      9,      0, "" }, /* graphic */
{ "sect4",     0,      0,      0,      9,      0, "" }, /* sect */
{ "sect5info", 0,      0,      0,      9,      0, "" }, /* graphic */
{ "sect5",     0,      0,      0,      9,      0, "" }, /* sect */
{ "sectioninfo",0,     0,      0,      9,      0, "" }, /* graphic */
{ "section",   0,      0,      0,      9,      0, "" }, /* sectioninfo */
{ "seealsoie", 0,      0,      0,      4,      0, "" }, /* ndxterm */
{ "seealso",   0,      0,      0,      4,      0, "" }, /* ndxterm */
{ "seeie",     0,      0,      0,      4,      0, "" }, /* ndxterm */
{ "see",       0,      0,      0,      4,      0, "" }, /* ndxterm */
{ "seglistitem",0,     0,      0,      9,      0, "" }, /* seg */
{ "segmentedlist",0,   0,      0,      9,      0, "" }, /* formalobject.title.content */
{ "seg",       0,      0,      0,      2,      0, "" }, /* para */
{ "segtitle",  0,      0,      0,      8,      0, "" }, /* title */
{ "seriesvolnums",     0,      0,      0,      4,      0, "" }, /* docinfo */
{ "set",       0,      0,      0,      9,      0, "" }, /* div.title.content */
{ "setindexinfo",0,    0,      0,      9,      0, "" }, /* graphic */
{ "setindex",  0,      0,      0,      9,      0, "" }, /* setindexinfo */
{ "setinfo",   0,      0,      0,      9,      0, "" }, /* graphic */
{ "sgmltag",   0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "shortaffil",        0,      0,      0,      4,      0, "" }, /* docinfo */
{ "shortcut",  0,      0,      0,      9,      0, "" }, /* keycap */
{ "sidebarinfo",0,     0,      0,      9,      0, "" }, /* graphic */
{ "sidebar",   0,      0,      0,      9,      0, "" }, /* sidebarinfo */
{ "simpara",   0,      0,      0,      2,      0, "" }, /* para */
{ "simplelist",        0,      0,      0,      9,      0, "" }, /* member */
{ "simplemsgentry",    0,      0,      0,      9,      0, "" }, /* msgtext */
{ "simplesect",        0,      0,      0,      9,      0, "" }, /* sect.title.content */
{ "spanspec",  0,      2,      1,      0,      0, "" },
{ "state",     0,      0,      0,      4,      0, "" }, /* docinfo */
{ "step",      0,      0,      0,      9,      0, "" }, /* title */
{ "street",    0,      0,      0,      4,      0, "" }, /* docinfo */
{ "structfield",0,     0,      0,      7,      0, "" }, /* smallcptr */
{ "structname",        0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "subjectset",        0,      0,      0,      9,      0, "" }, /* subject */
{ "subject",   0,      0,      0,      9,      0, "" }, /* subjectterm */
{ "subjectterm",0,     0,      0,      1,      0, "" },
{ "subscript", 0,      0,      0,      1,      0, "" },
{ "substeps",  0,      0,      0,      9,      0, "" }, /* step */
{ "subtitle",  0,      0,      0,      8,      0, "" }, /* title */
{ "superscript",       0,      0,      0,      1,      0, "" },
{ "surname",   0,      0,      0,      4,      0, "" }, /* docinfo */
{ "symbol",    0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "synopfragment",     0,      0,      0,      9,      0, "" }, /* arg */
{ "synopfragmentref",  0,      0,      0,      1,      0, "" },
{ "synopsis",  0,      0,      0,      2,      0, "" }, /* para */
{ "systemitem",        0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "table",     0,      0,      0,      9,      0, "" }, /* tbl.table.mdl */
/* { "%tbl.table.name;",       0,      0,      0,      9,      0, "" },*/ /* tbl.table.mdl */
{ "tbody",     0,      0,      0,      9,      0, "" }, /* row */
{ "tbody",     0,      0,      0,      9,      0, "" }, /* row */
{ "term",      0,      0,      0,      2,      0, "" }, /* para */
{ "tertiaryie",        0,      0,      0,      4,      0, "" }, /* ndxterm */
{ "tertiary ", 0,      0,      0,      4,      0, "" }, /* ndxterm */
{ "textobject",        0,      0,      0,      9,      0, "" }, /* objectinfo */
{ "tfoot",     0,      0,      0,      9,      0, "" }, /* tbl.hdft.mdl */
{ "tgroup",    0,      0,      0,      9,      0, "" }, /* tbl.tgroup.mdl */
{ "tgroup",    0,      0,      0,      9,      0, "" }, /* tbl.tgroup.mdl */
{ "thead",     0,      0,      0,      9,      0, "" }, /* row */
{ "thead",     0,      0,      0,      9,      0, "" }, /* tbl.hdft.mdl */
{ "tip",       0,      0,      0,      9,      0, "" }, /* title */
{ "titleabbrev",0,     0,      0,      8,      0, "" }, /* title */
{ "title",     0,      0,      0,      8,      0, "" }, /* title */
{ "tocback",   0,      0,      0,      2,      0, "" }, /* para */
{ "toc",       0,      0,      0,      9,      0, "" }, /* bookcomponent.title.content */
{ "tocchap",   0,      0,      0,      9,      0, "" }, /* tocentry */
{ "tocentry",  0,      0,      0,      2,      0, "" }, /* para */
{ "tocfront",  0,      0,      0,      2,      0, "" }, /* para */
{ "toclevel1", 0,      0,      0,      9,      0, "" }, /* tocentry */
{ "toclevel2", 0,      0,      0,      9,      0, "" }, /* tocentry */
{ "toclevel3", 0,      0,      0,      9,      0, "" }, /* tocentry */
{ "toclevel4", 0,      0,      0,      9,      0, "" }, /* tocentry */
{ "toclevel5", 0,      0,      0,      9,      0, "" }, /* tocentry */
{ "tocpart",   0,      0,      0,      9,      0, "" }, /* tocentry */
{ "token",     0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "trademark", 0,      0,      0,      1,      0, "" },
{ "type",      0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "ulink",     0,      0,      0,      2,      0, "" }, /* para */
{ "userinput", 0,      0,      0,      9,      0, "" }, /* cptr */
{ "varargs",   0,      2,      1,      0,      0, "" },
{ "variablelist",0,    0,      0,      9,      0, "" }, /* formalobject.title.content */
{ "varlistentry",0,    0,      0,      9,      0, "" }, /* term */
{ "varname",   0,      0,      0,      7,      0, "" }, /* smallcptr */
{ "videodata", 0,      2,      1,      0,      0, "" },
{ "videoobject",0,     0,      0,      9,      0, "" }, /* objectinfo */
{ "void",      0,      2,      1,      0,      0, "" },
{ "volumenum", 0,      0,      0,      4,      0, "" }, /* docinfo */
{ "warning",   0,      0,      0,      9,      0, "" }, /* title */
{ "wordasword",        0,      0,      0,      3,      0, "" }, /* word */
{ "xref",      0,      2,      1,      0,      0, "" },
{ "year",      0,      0,      0,      4,      0, "" }, /* docinfo */
};

#if 0
/*
 * start tags that imply the end of a current element
 * any tag of each line implies the end of the current element if the type of
 * that element is in the same line
 */
static const char *docbEquEnd[] = {
"dt", "dd", "li", "option", NULL,
"h1", "h2", "h3", "h4", "h5", "h6", NULL,
"ol", "menu", "dir", "address", "pre", "listing", "xmp", NULL,
NULL
};
#endif

/*
 * according the SGML DTD, HR should be added to the 2nd line above, as it
 * is not allowed within a H1, H2, H3, etc. But we should tolerate that case
 * because many documents contain rules in headings...
 */

/*
 * start tags that imply the end of current element
 */
static const char *docbStartClose[] = {
NULL
};

static const char** docbStartCloseIndex[100];
static int docbStartCloseIndexinitialized = 0;

/************************************************************************
 *                                                                     *
 *             functions to handle SGML specific data                  *
 *                                                                     *
 ************************************************************************/

/**
 * docbInitAutoClose:
 *
 * Initialize the docbStartCloseIndex for fast lookup of closing tags names.
 *
 */
static void
docbInitAutoClose(void) {
    int indx, i = 0;

    if (docbStartCloseIndexinitialized) return;

    for (indx = 0;indx < 100;indx ++) docbStartCloseIndex[indx] = NULL;
    indx = 0;
    while ((docbStartClose[i] != NULL) && (indx < 100 - 1)) {
        docbStartCloseIndex[indx++] = &docbStartClose[i];
       while (docbStartClose[i] != NULL) i++;
       i++;
    }
}

/**
 * docbTagLookup:
 * @tag:  The tag name
 *
 * Lookup the SGML tag in the ElementTable
 *
 * Returns the related docbElemDescPtr or NULL if not found.
 */
static docbElemDescPtr
docbTagLookup(const xmlChar *tag) {
    unsigned int i;

    for (i = 0; i < (sizeof(docbookElementTable) /
                     sizeof(docbookElementTable[0]));i++) {
        if (xmlStrEqual(tag, BAD_CAST docbookElementTable[i].name))
           return(&docbookElementTable[i]);
    }
    return(NULL);
}

/**
 * docbCheckAutoClose:
 * @newtag:  The new tag name
 * @oldtag:  The old tag name
 *
 * Checks whether the new tag is one of the registered valid tags for
 * closing old.
 * Initialize the docbStartCloseIndex for fast lookup of closing tags names.
 *
 * Returns 0 if no, 1 if yes.
 */
static int
docbCheckAutoClose(const xmlChar *newtag, const xmlChar *oldtag) {
    int i, indx;
    const char **closed = NULL;

    if (docbStartCloseIndexinitialized == 0) docbInitAutoClose();

    /* inefficient, but not a big deal */
    for (indx = 0; indx < 100;indx++) {
        closed = docbStartCloseIndex[indx];
       if (closed == NULL) return(0);
       if (xmlStrEqual(BAD_CAST *closed, newtag)) break;
    }

    i = closed - docbStartClose;
    i++;
    while (docbStartClose[i] != NULL) {
        if (xmlStrEqual(BAD_CAST docbStartClose[i], oldtag)) {
           return(1);
       }
       i++;
    }
    return(0);
}

/**
 * docbAutoCloseOnClose:
 * @ctxt:  an SGML parser context
 * @newtag:  The new tag name
 *
 * The DocBook DTD allows an ending tag to implicitly close other tags.
 */
static void
docbAutoCloseOnClose(docbParserCtxtPtr ctxt, const xmlChar *newtag) {
    docbElemDescPtr info;
    xmlChar *oldname;
    int i;

    if ((newtag[0] == '/') && (newtag[1] == 0))
       return;

#ifdef DEBUG
    xmlGenericError(xmlGenericErrorContext,"Close of %s stack: %d elements\n", newtag, ctxt->nameNr);
    for (i = 0;i < ctxt->nameNr;i++) 
        xmlGenericError(xmlGenericErrorContext,"%d : %s\n", i, ctxt->nameTab[i]);
#endif

    for (i = (ctxt->nameNr - 1);i >= 0;i--) {
        if (xmlStrEqual(newtag, ctxt->nameTab[i])) break;
    }
    if (i < 0) return;

    while (!xmlStrEqual(newtag, ctxt->name)) {
       info = docbTagLookup(ctxt->name);
       if ((info == NULL) || (info->endTag == 1)) {
#ifdef DEBUG
           xmlGenericError(xmlGenericErrorContext,"docbAutoCloseOnClose: %s closes %s\n", newtag, ctxt->name);
#endif
        } else {
           if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
               ctxt->sax->error(ctxt->userData,
                "Opening and ending tag mismatch: %s and %s\n",
                                newtag, ctxt->name);
           ctxt->wellFormed = 0;
       }
       if ((ctxt->sax != NULL) && (ctxt->sax->endElement != NULL))
           ctxt->sax->endElement(ctxt->userData, ctxt->name);
       oldname = docbnamePop(ctxt);
       if (oldname != NULL) {
#ifdef DEBUG
           xmlGenericError(xmlGenericErrorContext,"docbAutoCloseOnClose: popped %s\n", oldname);
#endif
           xmlFree(oldname);
       }       
    }
}

/**
 * docbAutoClose:
 * @ctxt:  an SGML parser context
 * @newtag:  The new tag name or NULL
 *
 * The DocBook DTD allows a tag to implicitly close other tags.
 * The list is kept in docbStartClose array. This function is
 * called when a new tag has been detected and generates the
 * appropriates closes if possible/needed.
 * If newtag is NULL this mean we are at the end of the resource
 * and we should check 
 */
static void
docbAutoClose(docbParserCtxtPtr ctxt, const xmlChar *newtag) {
    xmlChar *oldname;
    while ((newtag != NULL) && (ctxt->name != NULL) && 
           (docbCheckAutoClose(newtag, ctxt->name))) {
#ifdef DEBUG
       xmlGenericError(xmlGenericErrorContext,"docbAutoClose: %s closes %s\n", newtag, ctxt->name);
#endif
       if ((ctxt->sax != NULL) && (ctxt->sax->endElement != NULL))
           ctxt->sax->endElement(ctxt->userData, ctxt->name);
       oldname = docbnamePop(ctxt);
       if (oldname != NULL) {
#ifdef DEBUG
           xmlGenericError(xmlGenericErrorContext,"docbAutoClose: popped %s\n", oldname);
#endif
           xmlFree(oldname);
        }
    }
}

/**
 * docbAutoCloseTag:
 * @doc:  the SGML document
 * @name:  The tag name
 * @elem:  the SGML element
 *
 * The DocBook DTD allows a tag to implicitly close other tags.
 * The list is kept in docbStartClose array. This function checks
 * if the element or one of it's children would autoclose the
 * given tag.
 *
 * Returns 1 if autoclose, 0 otherwise
 */
static int
docbAutoCloseTag(docbDocPtr doc, const xmlChar *name, docbNodePtr elem) {
    docbNodePtr child;

    if (elem == NULL) return(1);
    if (xmlStrEqual(name, elem->name)) return(0);
    if (docbCheckAutoClose(elem->name, name)) return(1);
    child = elem->children;
    while (child != NULL) {
        if (docbAutoCloseTag(doc, name, child)) return(1);
       child = child->next;
    }
    return(0);
}

/************************************************************************
 *                                                                     *
 *             The list of SGML predefined entities                    *
 *                                                                     *
 ************************************************************************/


static docbEntityDesc
docbookEntitiesTable[] = {
/*
 * the 4 absolute ones, plus apostrophe.
 */
{ 0x0026, "amp", "AMPERSAND" },
{ 0x003C, "lt",        "LESS-THAN SIGN" },

/*
 * Converted with VI macros from docbook ent files
 */
{ 0x0021, "excl", "EXCLAMATION MARK" },
{ 0x0022, "quot", "QUOTATION MARK" },
{ 0x0023, "num", "NUMBER SIGN" },
{ 0x0024, "dollar", "DOLLAR SIGN" },
{ 0x0025, "percnt", "PERCENT SIGN" },
{ 0x0027, "apos", "APOSTROPHE" },
{ 0x0028, "lpar", "LEFT PARENTHESIS" },
{ 0x0029, "rpar", "RIGHT PARENTHESIS" },
{ 0x002A, "ast", "ASTERISK OPERATOR" },
{ 0x002B, "plus", "PLUS SIGN" },
{ 0x002C, "comma", "COMMA" },
{ 0x002D, "hyphen", "HYPHEN-MINUS" },
{ 0x002E, "period", "FULL STOP" },
{ 0x002F, "sol", "SOLIDUS" },
{ 0x003A, "colon", "COLON" },
{ 0x003B, "semi", "SEMICOLON" },
{ 0x003D, "equals", "EQUALS SIGN" },
{ 0x003E, "gt", "GREATER-THAN SIGN" },
{ 0x003F, "quest", "QUESTION MARK" },
{ 0x0040, "commat", "COMMERCIAL AT" },
{ 0x005B, "lsqb", "LEFT SQUARE BRACKET" },
{ 0x005C, "bsol", "REVERSE SOLIDUS" },
{ 0x005D, "rsqb", "RIGHT SQUARE BRACKET" },
{ 0x005E, "circ", "RING OPERATOR" },
{ 0x005F, "lowbar", "LOW LINE" },
{ 0x0060, "grave", "GRAVE ACCENT" },
{ 0x007B, "lcub", "LEFT CURLY BRACKET" },
{ 0x007C, "verbar", "VERTICAL LINE" },
{ 0x007D, "rcub", "RIGHT CURLY BRACKET" },
{ 0x00A0, "nbsp", "NO-BREAK SPACE" },
{ 0x00A1, "iexcl", "INVERTED EXCLAMATION MARK" },
{ 0x00A2, "cent", "CENT SIGN" },
{ 0x00A3, "pound", "POUND SIGN" },
{ 0x00A4, "curren", "CURRENCY SIGN" },
{ 0x00A5, "yen", "YEN SIGN" },
{ 0x00A6, "brvbar", "BROKEN BAR" },
{ 0x00A7, "sect", "SECTION SIGN" },
{ 0x00A8, "die", "" },
{ 0x00A8, "Dot", "" },
{ 0x00A8, "uml", "" },
{ 0x00A9, "copy", "COPYRIGHT SIGN" },
{ 0x00AA, "ordf", "FEMININE ORDINAL INDICATOR" },
{ 0x00AB, "laquo", "LEFT-POINTING DOUBLE ANGLE QUOTATION MARK" },
{ 0x00AC, "not", "NOT SIGN" },
{ 0x00AD, "shy", "SOFT HYPHEN" },
{ 0x00AE, "reg", "REG TRADE MARK SIGN" },
{ 0x00AF, "macr", "MACRON" },
{ 0x00B0, "deg", "DEGREE SIGN" },
{ 0x00B1, "plusmn", "PLUS-MINUS SIGN" },
{ 0x00B2, "sup2", "SUPERSCRIPT TWO" },
{ 0x00B3, "sup3", "SUPERSCRIPT THREE" },
{ 0x00B4, "acute", "ACUTE ACCENT" },
{ 0x00B5, "micro", "MICRO SIGN" },
{ 0x00B6, "para", "PILCROW SIGN" },
{ 0x00B7, "middot", "MIDDLE DOT" },
{ 0x00B8, "cedil", "CEDILLA" },
{ 0x00B9, "sup1", "SUPERSCRIPT ONE" },
{ 0x00BA, "ordm", "MASCULINE ORDINAL INDICATOR" },
{ 0x00BB, "raquo", "RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK" },
{ 0x00BC, "frac14", "VULGAR FRACTION ONE QUARTER" },
{ 0x00BD, "frac12", "VULGAR FRACTION ONE HALF" },
{ 0x00BD, "half", "VULGAR FRACTION ONE HALF" },
{ 0x00BE, "frac34", "VULGAR FRACTION THREE QUARTERS" },
{ 0x00BF, "iquest", "INVERTED QUESTION MARK" },
{ 0x00C0, "Agrave", "LATIN CAPITAL LETTER A WITH GRAVE" },
{ 0x00C1, "Aacute", "LATIN CAPITAL LETTER A WITH ACUTE" },
{ 0x00C2, "Acirc", "LATIN CAPITAL LETTER A WITH CIRCUMFLEX" },
{ 0x00C3, "Atilde", "LATIN CAPITAL LETTER A WITH TILDE" },
{ 0x00C4, "Auml", "LATIN CAPITAL LETTER A WITH DIAERESIS" },
{ 0x00C5, "Aring", "LATIN CAPITAL LETTER A WITH RING ABOVE" },
{ 0x00C6, "AElig", "LATIN CAPITAL LETTER AE" },
{ 0x00C7, "Ccedil", "LATIN CAPITAL LETTER C WITH CEDILLA" },
{ 0x00C8, "Egrave", "LATIN CAPITAL LETTER E WITH GRAVE" },
{ 0x00C9, "Eacute", "LATIN CAPITAL LETTER E WITH ACUTE" },
{ 0x00CA, "Ecirc", "LATIN CAPITAL LETTER E WITH CIRCUMFLEX" },
{ 0x00CB, "Euml", "LATIN CAPITAL LETTER E WITH DIAERESIS" },
{ 0x00CC, "Igrave", "LATIN CAPITAL LETTER I WITH GRAVE" },
{ 0x00CD, "Iacute", "LATIN CAPITAL LETTER I WITH ACUTE" },
{ 0x00CE, "Icirc", "LATIN CAPITAL LETTER I WITH CIRCUMFLEX" },
{ 0x00CF, "Iuml", "LATIN CAPITAL LETTER I WITH DIAERESIS" },
{ 0x00D0, "ETH", "LATIN CAPITAL LETTER ETH" },
{ 0x00D1, "Ntilde", "LATIN CAPITAL LETTER N WITH TILDE" },
{ 0x00D2, "Ograve", "LATIN CAPITAL LETTER O WITH GRAVE" },
{ 0x00D3, "Oacute", "LATIN CAPITAL LETTER O WITH ACUTE" },
{ 0x00D4, "Ocirc", "LATIN CAPITAL LETTER O WITH CIRCUMFLEX" },
{ 0x00D5, "Otilde", "LATIN CAPITAL LETTER O WITH TILDE" },
{ 0x00D6, "Ouml", "LATIN CAPITAL LETTER O WITH DIAERESIS" },
{ 0x00D7, "times", "MULTIPLICATION SIGN" },
{ 0x00D8, "Oslash", "LATIN CAPITAL LETTER O WITH STROKE" },
{ 0x00D9, "Ugrave", "LATIN CAPITAL LETTER U WITH GRAVE" },
{ 0x00DA, "Uacute", "LATIN CAPITAL LETTER U WITH ACUTE" },
{ 0x00DB, "Ucirc", "LATIN CAPITAL LETTER U WITH CIRCUMFLEX" },
{ 0x00DC, "Uuml", "LATIN CAPITAL LETTER U WITH DIAERESIS" },
{ 0x00DD, "Yacute", "LATIN CAPITAL LETTER Y WITH ACUTE" },
{ 0x00DE, "THORN", "LATIN CAPITAL LETTER THORN" },
{ 0x00DF, "szlig", "LATIN SMALL LETTER SHARP S" },
{ 0x00E0, "agrave", "LATIN SMALL LETTER A WITH GRAVE" },
{ 0x00E1, "aacute", "LATIN SMALL LETTER A WITH ACUTE" },
{ 0x00E2, "acirc", "LATIN SMALL LETTER A WITH CIRCUMFLEX" },
{ 0x00E3, "atilde", "LATIN SMALL LETTER A WITH TILDE" },
{ 0x00E4, "auml", "LATIN SMALL LETTER A WITH DIAERESIS" },
{ 0x00E5, "aring", "LATIN SMALL LETTER A WITH RING ABOVE" },
{ 0x00E6, "aelig", "LATIN SMALL LETTER AE" },
{ 0x00E7, "ccedil", "LATIN SMALL LETTER C WITH CEDILLA" },
{ 0x00E8, "egrave", "LATIN SMALL LETTER E WITH GRAVE" },
{ 0x00E9, "eacute", "LATIN SMALL LETTER E WITH ACUTE" },
{ 0x00EA, "ecirc", "LATIN SMALL LETTER E WITH CIRCUMFLEX" },
{ 0x00EB, "euml", "LATIN SMALL LETTER E WITH DIAERESIS" },
{ 0x00EC, "igrave", "LATIN SMALL LETTER I WITH GRAVE" },
{ 0x00ED, "iacute", "LATIN SMALL LETTER I WITH ACUTE" },
{ 0x00EE, "icirc", "LATIN SMALL LETTER I WITH CIRCUMFLEX" },
{ 0x00EF, "iuml", "LATIN SMALL LETTER I WITH DIAERESIS" },
{ 0x00F0, "eth", "LATIN SMALL LETTER ETH" },
{ 0x00F1, "ntilde", "LATIN SMALL LETTER N WITH TILDE" },
{ 0x00F2, "ograve", "LATIN SMALL LETTER O WITH GRAVE" },
{ 0x00F3, "oacute", "LATIN SMALL LETTER O WITH ACUTE" },
{ 0x00F4, "ocirc", "LATIN SMALL LETTER O WITH CIRCUMFLEX" },
{ 0x00F5, "otilde", "LATIN SMALL LETTER O WITH TILDE" },
{ 0x00F6, "ouml", "LATIN SMALL LETTER O WITH DIAERESIS" },
{ 0x00F7, "divide", "DIVISION SIGN" },
{ 0x00F8, "oslash", "CIRCLED DIVISION SLASH" },
{ 0x00F9, "ugrave", "LATIN SMALL LETTER U WITH GRAVE" },
{ 0x00FA, "uacute", "LATIN SMALL LETTER U WITH ACUTE" },
{ 0x00FB, "ucirc", "LATIN SMALL LETTER U WITH CIRCUMFLEX" },
{ 0x00FC, "uuml", "LATIN SMALL LETTER U WITH DIAERESIS" },
{ 0x00FD, "yacute", "LATIN SMALL LETTER Y WITH ACUTE" },
{ 0x00FE, "thorn", "LATIN SMALL LETTER THORN" },
{ 0x00FF, "yuml", "LATIN SMALL LETTER Y WITH DIAERESIS" },
{ 0x0100, "Amacr", "LATIN CAPITAL LETTER A WITH MACRON" },
{ 0x0101, "amacr", "LATIN SMALL LETTER A WITH MACRON" },
{ 0x0102, "Abreve", "LATIN CAPITAL LETTER A WITH BREVE" },
{ 0x0103, "abreve", "LATIN SMALL LETTER A WITH BREVE" },
{ 0x0104, "Aogon", "LATIN CAPITAL LETTER A WITH OGONEK" },
{ 0x0105, "aogon", "LATIN SMALL LETTER A WITH OGONEK" },
{ 0x0106, "Cacute", "LATIN CAPITAL LETTER C WITH ACUTE" },
{ 0x0107, "cacute", "LATIN SMALL LETTER C WITH ACUTE" },
{ 0x0108, "Ccirc", "LATIN CAPITAL LETTER C WITH CIRCUMFLEX" },
{ 0x0109, "ccirc", "LATIN SMALL LETTER C WITH CIRCUMFLEX" },
{ 0x010A, "Cdot", "LATIN CAPITAL LETTER C WITH DOT ABOVE" },
{ 0x010B, "cdot", "DOT OPERATOR" },
{ 0x010C, "Ccaron", "LATIN CAPITAL LETTER C WITH CARON" },
{ 0x010D, "ccaron", "LATIN SMALL LETTER C WITH CARON" },
{ 0x010E, "Dcaron", "LATIN CAPITAL LETTER D WITH CARON" },
{ 0x010F, "dcaron", "LATIN SMALL LETTER D WITH CARON" },
{ 0x0110, "Dstrok", "LATIN CAPITAL LETTER D WITH STROKE" },
{ 0x0111, "dstrok", "LATIN SMALL LETTER D WITH STROKE" },
{ 0x0112, "Emacr", "LATIN CAPITAL LETTER E WITH MACRON" },
{ 0x0113, "emacr", "LATIN SMALL LETTER E WITH MACRON" },
{ 0x0116, "Edot", "LATIN CAPITAL LETTER E WITH DOT ABOVE" },
{ 0x0117, "edot", "LATIN SMALL LETTER E WITH DOT ABOVE" },
{ 0x0118, "Eogon", "LATIN CAPITAL LETTER E WITH OGONEK" },
{ 0x0119, "eogon", "LATIN SMALL LETTER E WITH OGONEK" },
{ 0x011A, "Ecaron", "LATIN CAPITAL LETTER E WITH CARON" },
{ 0x011B, "ecaron", "LATIN SMALL LETTER E WITH CARON" },
{ 0x011C, "Gcirc", "LATIN CAPITAL LETTER G WITH CIRCUMFLEX" },
{ 0x011D, "gcirc", "LATIN SMALL LETTER G WITH CIRCUMFLEX" },
{ 0x011E, "Gbreve", "LATIN CAPITAL LETTER G WITH BREVE" },
{ 0x011F, "gbreve", "LATIN SMALL LETTER G WITH BREVE" },
{ 0x0120, "Gdot", "LATIN CAPITAL LETTER G WITH DOT ABOVE" },
{ 0x0121, "gdot", "LATIN SMALL LETTER G WITH DOT ABOVE" },
{ 0x0122, "Gcedil", "LATIN CAPITAL LETTER G WITH CEDILLA" },
{ 0x0124, "Hcirc", "LATIN CAPITAL LETTER H WITH CIRCUMFLEX" },
{ 0x0125, "hcirc", "LATIN SMALL LETTER H WITH CIRCUMFLEX" },
{ 0x0126, "Hstrok", "LATIN CAPITAL LETTER H WITH STROKE" },
{ 0x0127, "hstrok", "LATIN SMALL LETTER H WITH STROKE" },
{ 0x0128, "Itilde", "LATIN CAPITAL LETTER I WITH TILDE" },
{ 0x0129, "itilde", "LATIN SMALL LETTER I WITH TILDE" },
{ 0x012A, "Imacr", "LATIN CAPITAL LETTER I WITH MACRON" },
{ 0x012B, "imacr", "LATIN SMALL LETTER I WITH MACRON" },
{ 0x012E, "Iogon", "LATIN CAPITAL LETTER I WITH OGONEK" },
{ 0x012F, "iogon", "LATIN SMALL LETTER I WITH OGONEK" },
{ 0x0130, "Idot", "LATIN CAPITAL LETTER I WITH DOT ABOVE" },
{ 0x0131, "inodot", "LATIN SMALL LETTER DOTLESS I" },
{ 0x0131, "inodot", "LATIN SMALL LETTER DOTLESS I" },
{ 0x0132, "IJlig", "LATIN CAPITAL LIGATURE IJ" },
{ 0x0133, "ijlig", "LATIN SMALL LIGATURE IJ" },
{ 0x0134, "Jcirc", "LATIN CAPITAL LETTER J WITH CIRCUMFLEX" },
{ 0x0135, "jcirc", "LATIN SMALL LETTER J WITH CIRCUMFLEX" },
{ 0x0136, "Kcedil", "LATIN CAPITAL LETTER K WITH CEDILLA" },
{ 0x0137, "kcedil", "LATIN SMALL LETTER K WITH CEDILLA" },
{ 0x0138, "kgreen", "LATIN SMALL LETTER KRA" },
{ 0x0139, "Lacute", "LATIN CAPITAL LETTER L WITH ACUTE" },
{ 0x013A, "lacute", "LATIN SMALL LETTER L WITH ACUTE" },
{ 0x013B, "Lcedil", "LATIN CAPITAL LETTER L WITH CEDILLA" },
{ 0x013C, "lcedil", "LATIN SMALL LETTER L WITH CEDILLA" },
{ 0x013D, "Lcaron", "LATIN CAPITAL LETTER L WITH CARON" },
{ 0x013E, "lcaron", "LATIN SMALL LETTER L WITH CARON" },
{ 0x013F, "Lmidot", "LATIN CAPITAL LETTER L WITH MIDDLE DOT" },
{ 0x0140, "lmidot", "LATIN SMALL LETTER L WITH MIDDLE DOT" },
{ 0x0141, "Lstrok", "LATIN CAPITAL LETTER L WITH STROKE" },
{ 0x0142, "lstrok", "LATIN SMALL LETTER L WITH STROKE" },
{ 0x0143, "Nacute", "LATIN CAPITAL LETTER N WITH ACUTE" },
{ 0x0144, "nacute", "LATIN SMALL LETTER N WITH ACUTE" },
{ 0x0145, "Ncedil", "LATIN CAPITAL LETTER N WITH CEDILLA" },
{ 0x0146, "ncedil", "LATIN SMALL LETTER N WITH CEDILLA" },
{ 0x0147, "Ncaron", "LATIN CAPITAL LETTER N WITH CARON" },
{ 0x0148, "ncaron", "LATIN SMALL LETTER N WITH CARON" },
{ 0x0149, "napos", "LATIN SMALL LETTER N PRECEDED BY APOSTROPHE" },
{ 0x014A, "ENG", "LATIN CAPITAL LETTER ENG" },
{ 0x014B, "eng", "LATIN SMALL LETTER ENG" },
{ 0x014C, "Omacr", "LATIN CAPITAL LETTER O WITH MACRON" },
{ 0x014D, "omacr", "LATIN SMALL LETTER O WITH MACRON" },
{ 0x0150, "Odblac", "LATIN CAPITAL LETTER O WITH DOUBLE ACUTE" },
{ 0x0151, "odblac", "LATIN SMALL LETTER O WITH DOUBLE ACUTE" },
{ 0x0152, "OElig", "LATIN CAPITAL LIGATURE OE" },
{ 0x0153, "oelig", "LATIN SMALL LIGATURE OE" },
{ 0x0154, "Racute", "LATIN CAPITAL LETTER R WITH ACUTE" },
{ 0x0155, "racute", "LATIN SMALL LETTER R WITH ACUTE" },
{ 0x0156, "Rcedil", "LATIN CAPITAL LETTER R WITH CEDILLA" },
{ 0x0157, "rcedil", "LATIN SMALL LETTER R WITH CEDILLA" },
{ 0x0158, "Rcaron", "LATIN CAPITAL LETTER R WITH CARON" },
{ 0x0159, "rcaron", "LATIN SMALL LETTER R WITH CARON" },
{ 0x015A, "Sacute", "LATIN CAPITAL LETTER S WITH ACUTE" },
{ 0x015B, "sacute", "LATIN SMALL LETTER S WITH ACUTE" },
{ 0x015C, "Scirc", "LATIN CAPITAL LETTER S WITH CIRCUMFLEX" },
{ 0x015D, "scirc", "LATIN SMALL LETTER S WITH CIRCUMFLEX" },
{ 0x015E, "Scedil", "LATIN CAPITAL LETTER S WITH CEDILLA" },
{ 0x015F, "scedil", "LATIN SMALL LETTER S WITH CEDILLA" },
{ 0x0160, "Scaron", "LATIN CAPITAL LETTER S WITH CARON" },
{ 0x0161, "scaron", "LATIN SMALL LETTER S WITH CARON" },
{ 0x0162, "Tcedil", "LATIN CAPITAL LETTER T WITH CEDILLA" },
{ 0x0163, "tcedil", "LATIN SMALL LETTER T WITH CEDILLA" },
{ 0x0164, "Tcaron", "LATIN CAPITAL LETTER T WITH CARON" },
{ 0x0165, "tcaron", "LATIN SMALL LETTER T WITH CARON" },
{ 0x0166, "Tstrok", "LATIN CAPITAL LETTER T WITH STROKE" },
{ 0x0167, "tstrok", "LATIN SMALL LETTER T WITH STROKE" },
{ 0x0168, "Utilde", "LATIN CAPITAL LETTER U WITH TILDE" },
{ 0x0169, "utilde", "LATIN SMALL LETTER U WITH TILDE" },
{ 0x016A, "Umacr", "LATIN CAPITAL LETTER U WITH MACRON" },
{ 0x016B, "umacr", "LATIN SMALL LETTER U WITH MACRON" },
{ 0x016C, "Ubreve", "LATIN CAPITAL LETTER U WITH BREVE" },
{ 0x016D, "ubreve", "LATIN SMALL LETTER U WITH BREVE" },
{ 0x016E, "Uring", "LATIN CAPITAL LETTER U WITH RING ABOVE" },
{ 0x016F, "uring", "LATIN SMALL LETTER U WITH RING ABOVE" },
{ 0x0170, "Udblac", "LATIN CAPITAL LETTER U WITH DOUBLE ACUTE" },
{ 0x0171, "udblac", "LATIN SMALL LETTER U WITH DOUBLE ACUTE" },
{ 0x0172, "Uogon", "LATIN CAPITAL LETTER U WITH OGONEK" },
{ 0x0173, "uogon", "LATIN SMALL LETTER U WITH OGONEK" },
{ 0x0174, "Wcirc", "LATIN CAPITAL LETTER W WITH CIRCUMFLEX" },
{ 0x0175, "wcirc", "LATIN SMALL LETTER W WITH CIRCUMFLEX" },
{ 0x0176, "Ycirc", "LATIN CAPITAL LETTER Y WITH CIRCUMFLEX" },
{ 0x0177, "ycirc", "LATIN SMALL LETTER Y WITH CIRCUMFLEX" },
{ 0x0178, "Yuml", "LATIN CAPITAL LETTER Y WITH DIAERESIS" },
{ 0x0179, "Zacute", "LATIN CAPITAL LETTER Z WITH ACUTE" },
{ 0x017A, "zacute", "LATIN SMALL LETTER Z WITH ACUTE" },
{ 0x017B, "Zdot", "LATIN CAPITAL LETTER Z WITH DOT ABOVE" },
{ 0x017C, "zdot", "LATIN SMALL LETTER Z WITH DOT ABOVE" },
{ 0x017D, "Zcaron", "LATIN CAPITAL LETTER Z WITH CARON" },
{ 0x017E, "zcaron", "LATIN SMALL LETTER Z WITH CARON" },
{ 0x0192, "fnof", "LATIN SMALL LETTER F WITH HOOK" },
{ 0x01F5, "gacute", "LATIN SMALL LETTER G WITH ACUTE" },
{ 0x02C7, "caron", "CARON" },
{ 0x02D8, "breve", "BREVE" },
{ 0x02D9, "dot", "DOT ABOVE" },
{ 0x02DA, "ring", "RING ABOVE" },
{ 0x02DB, "ogon", "OGONEK" },
{ 0x02DC, "tilde", "TILDE" },
{ 0x02DD, "dblac", "DOUBLE ACUTE ACCENT" },
{ 0x0386, "Aacgr", "GREEK CAPITAL LETTER ALPHA WITH TONOS" },
{ 0x0388, "Eacgr", "GREEK CAPITAL LETTER EPSILON WITH TONOS" },
{ 0x0389, "EEacgr", "GREEK CAPITAL LETTER ETA WITH TONOS" },
{ 0x038A, "Iacgr", "GREEK CAPITAL LETTER IOTA WITH TONOS" },
{ 0x038C, "Oacgr", "GREEK CAPITAL LETTER OMICRON WITH TONOS" },
{ 0x038E, "Uacgr", "GREEK CAPITAL LETTER UPSILON WITH TONOS" },
{ 0x038F, "OHacgr", "GREEK CAPITAL LETTER OMEGA WITH TONOS" },
{ 0x0390, "idiagr", "GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS" },
{ 0x0391, "Agr", "GREEK CAPITAL LETTER ALPHA" },
{ 0x0392, "Bgr", "GREEK CAPITAL LETTER BETA" },
{ 0x0393, "b.Gamma", "GREEK CAPITAL LETTER GAMMA" },
{ 0x0393, "Gamma", "GREEK CAPITAL LETTER GAMMA" },
{ 0x0393, "Ggr", "GREEK CAPITAL LETTER GAMMA" },
{ 0x0394, "b.Delta", "GREEK CAPITAL LETTER DELTA" },
{ 0x0394, "Delta", "GREEK CAPITAL LETTER DELTA" },
{ 0x0394, "Dgr", "GREEK CAPITAL LETTER DELTA" },
{ 0x0395, "Egr", "GREEK CAPITAL LETTER EPSILON" },
{ 0x0396, "Zgr", "GREEK CAPITAL LETTER ZETA" },
{ 0x0397, "EEgr", "GREEK CAPITAL LETTER ETA" },
{ 0x0398, "b.Theta", "GREEK CAPITAL LETTER THETA" },
{ 0x0398, "Theta", "GREEK CAPITAL LETTER THETA" },
{ 0x0398, "THgr", "GREEK CAPITAL LETTER THETA" },
{ 0x0399, "Igr", "GREEK CAPITAL LETTER IOTA" },
{ 0x039A, "Kgr", "GREEK CAPITAL LETTER KAPPA" },
{ 0x039B, "b.Lambda", "GREEK CAPITAL LETTER LAMDA" },
{ 0x039B, "Lambda", "GREEK CAPITAL LETTER LAMDA" },
{ 0x039B, "Lgr", "GREEK CAPITAL LETTER LAMDA" },
{ 0x039C, "Mgr", "GREEK CAPITAL LETTER MU" },
{ 0x039D, "Ngr", "GREEK CAPITAL LETTER NU" },
{ 0x039E, "b.Xi", "GREEK CAPITAL LETTER XI" },
{ 0x039E, "Xgr", "GREEK CAPITAL LETTER XI" },
{ 0x039E, "Xi", "GREEK CAPITAL LETTER XI" },
{ 0x039F, "Ogr", "GREEK CAPITAL LETTER OMICRON" },
{ 0x03A0, "b.Pi", "GREEK CAPITAL LETTER PI" },
{ 0x03A0, "Pgr", "GREEK CAPITAL LETTER PI" },
{ 0x03A0, "Pi", "GREEK CAPITAL LETTER PI" },
{ 0x03A1, "Rgr", "GREEK CAPITAL LETTER RHO" },
{ 0x03A3, "b.Sigma", "GREEK CAPITAL LETTER SIGMA" },
{ 0x03A3, "Sgr", "GREEK CAPITAL LETTER SIGMA" },
{ 0x03A3, "Sigma", "GREEK CAPITAL LETTER SIGMA" },
{ 0x03A4, "Tgr", "GREEK CAPITAL LETTER TAU" },
{ 0x03A5, "Ugr", "" },
{ 0x03A6, "b.Phi", "GREEK CAPITAL LETTER PHI" },
{ 0x03A6, "PHgr", "GREEK CAPITAL LETTER PHI" },
{ 0x03A6, "Phi", "GREEK CAPITAL LETTER PHI" },
{ 0x03A7, "KHgr", "GREEK CAPITAL LETTER CHI" },
{ 0x03A8, "b.Psi", "GREEK CAPITAL LETTER PSI" },
{ 0x03A8, "PSgr", "GREEK CAPITAL LETTER PSI" },
{ 0x03A8, "Psi", "GREEK CAPITAL LETTER PSI" },
{ 0x03A9, "b.Omega", "GREEK CAPITAL LETTER OMEGA" },
{ 0x03A9, "OHgr", "GREEK CAPITAL LETTER OMEGA" },
{ 0x03A9, "Omega", "GREEK CAPITAL LETTER OMEGA" },
{ 0x03AA, "Idigr", "GREEK CAPITAL LETTER IOTA WITH DIALYTIKA" },
{ 0x03AB, "Udigr", "GREEK CAPITAL LETTER UPSILON WITH DIALYTIKA" },
{ 0x03AC, "aacgr", "GREEK SMALL LETTER ALPHA WITH TONOS" },
{ 0x03AD, "eacgr", "GREEK SMALL LETTER EPSILON WITH TONOS" },
{ 0x03AE, "eeacgr", "GREEK SMALL LETTER ETA WITH TONOS" },
{ 0x03AF, "iacgr", "GREEK SMALL LETTER IOTA WITH TONOS" },
{ 0x03B0, "udiagr", "GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS" },
{ 0x03B1, "agr", "" },
{ 0x03B1, "alpha", "" },
{ 0x03B1, "b.alpha", "" },
{ 0x03B2, "b.beta", "GREEK SMALL LETTER BETA" },
{ 0x03B2, "beta", "GREEK SMALL LETTER BETA" },
{ 0x03B2, "bgr", "GREEK SMALL LETTER BETA" },
{ 0x03B3, "b.gamma", "GREEK SMALL LETTER GAMMA" },
{ 0x03B3, "gamma", "GREEK SMALL LETTER GAMMA" },
{ 0x03B3, "ggr", "GREEK SMALL LETTER GAMMA" },
{ 0x03B4, "b.delta", "GREEK SMALL LETTER DELTA" },
{ 0x03B4, "delta", "GREEK SMALL LETTER DELTA" },
{ 0x03B4, "dgr", "GREEK SMALL LETTER DELTA" },
{ 0x03B5, "b.epsi", "" },
{ 0x03B5, "b.epsis", "" },
{ 0x03B5, "b.epsiv", "" },
{ 0x03B5, "egr", "" },
{ 0x03B5, "epsiv", "" },
{ 0x03B6, "b.zeta", "GREEK SMALL LETTER ZETA" },
{ 0x03B6, "zeta", "GREEK SMALL LETTER ZETA" },
{ 0x03B6, "zgr", "GREEK SMALL LETTER ZETA" },
{ 0x03B7, "b.eta", "GREEK SMALL LETTER ETA" },
{ 0x03B7, "eegr", "GREEK SMALL LETTER ETA" },
{ 0x03B7, "eta", "GREEK SMALL LETTER ETA" },
{ 0x03B8, "b.thetas", "" },
{ 0x03B8, "thetas", "" },
{ 0x03B8, "thgr", "" },
{ 0x03B9, "b.iota", "GREEK SMALL LETTER IOTA" },
{ 0x03B9, "igr", "GREEK SMALL LETTER IOTA" },
{ 0x03B9, "iota", "GREEK SMALL LETTER IOTA" },
{ 0x03BA, "b.kappa", "GREEK SMALL LETTER KAPPA" },
{ 0x03BA, "kappa", "GREEK SMALL LETTER KAPPA" },
{ 0x03BA, "kgr", "GREEK SMALL LETTER KAPPA" },
{ 0x03BB, "b.lambda", "GREEK SMALL LETTER LAMDA" },
{ 0x03BB, "lambda", "GREEK SMALL LETTER LAMDA" },
{ 0x03BB, "lgr", "GREEK SMALL LETTER LAMDA" },
{ 0x03BC, "b.mu", "GREEK SMALL LETTER MU" },
{ 0x03BC, "mgr", "GREEK SMALL LETTER MU" },
{ 0x03BC, "mu", "GREEK SMALL LETTER MU" },
{ 0x03BD, "b.nu", "GREEK SMALL LETTER NU" },
{ 0x03BD, "ngr", "GREEK SMALL LETTER NU" },
{ 0x03BD, "nu", "GREEK SMALL LETTER NU" },
{ 0x03BE, "b.xi", "GREEK SMALL LETTER XI" },
{ 0x03BE, "xgr", "GREEK SMALL LETTER XI" },
{ 0x03BE, "xi", "GREEK SMALL LETTER XI" },
{ 0x03BF, "ogr", "GREEK SMALL LETTER OMICRON" },
{ 0x03C0, "b.pi", "GREEK SMALL LETTER PI" },
{ 0x03C0, "pgr", "GREEK SMALL LETTER PI" },
{ 0x03C0, "pi", "GREEK SMALL LETTER PI" },
{ 0x03C1, "b.rho", "GREEK SMALL LETTER RHO" },
{ 0x03C1, "rgr", "GREEK SMALL LETTER RHO" },
{ 0x03C1, "rho", "GREEK SMALL LETTER RHO" },
{ 0x03C2, "b.sigmav", "" },
{ 0x03C2, "sfgr", "" },
{ 0x03C2, "sigmav", "" },
{ 0x03C3, "b.sigma", "GREEK SMALL LETTER SIGMA" },
{ 0x03C3, "sgr", "GREEK SMALL LETTER SIGMA" },
{ 0x03C3, "sigma", "GREEK SMALL LETTER SIGMA" },
{ 0x03C4, "b.tau", "GREEK SMALL LETTER TAU" },
{ 0x03C4, "tau", "GREEK SMALL LETTER TAU" },
{ 0x03C4, "tgr", "GREEK SMALL LETTER TAU" },
{ 0x03C5, "b.upsi", "GREEK SMALL LETTER UPSILON" },
{ 0x03C5, "ugr", "GREEK SMALL LETTER UPSILON" },
{ 0x03C5, "upsi", "GREEK SMALL LETTER UPSILON" },
{ 0x03C6, "b.phis", "GREEK SMALL LETTER PHI" },
{ 0x03C6, "phgr", "GREEK SMALL LETTER PHI" },
{ 0x03C6, "phis", "GREEK SMALL LETTER PHI" },
{ 0x03C7, "b.chi", "GREEK SMALL LETTER CHI" },
{ 0x03C7, "chi", "GREEK SMALL LETTER CHI" },
{ 0x03C7, "khgr", "GREEK SMALL LETTER CHI" },
{ 0x03C8, "b.psi", "GREEK SMALL LETTER PSI" },
{ 0x03C8, "psgr", "GREEK SMALL LETTER PSI" },
{ 0x03C8, "psi", "GREEK SMALL LETTER PSI" },
{ 0x03C9, "b.omega", "GREEK SMALL LETTER OMEGA" },
{ 0x03C9, "ohgr", "GREEK SMALL LETTER OMEGA" },
{ 0x03C9, "omega", "GREEK SMALL LETTER OMEGA" },
{ 0x03CA, "idigr", "GREEK SMALL LETTER IOTA WITH DIALYTIKA" },
{ 0x03CB, "udigr", "GREEK SMALL LETTER UPSILON WITH DIALYTIKA" },
{ 0x03CC, "oacgr", "GREEK SMALL LETTER OMICRON WITH TONOS" },
{ 0x03CD, "uacgr", "GREEK SMALL LETTER UPSILON WITH TONOS" },
{ 0x03CE, "ohacgr", "GREEK SMALL LETTER OMEGA WITH TONOS" },
{ 0x03D1, "b.thetav", "" },
{ 0x03D1, "thetav", "" },
{ 0x03D2, "b.Upsi", "" },
{ 0x03D2, "Upsi", "" },
{ 0x03D5, "b.phiv", "GREEK PHI SYMBOL" },
{ 0x03D5, "phiv", "GREEK PHI SYMBOL" },
{ 0x03D6, "b.piv", "GREEK PI SYMBOL" },
{ 0x03D6, "piv", "GREEK PI SYMBOL" },
{ 0x03DC, "b.gammad", "GREEK LETTER DIGAMMA" },
{ 0x03DC, "gammad", "GREEK LETTER DIGAMMA" },
{ 0x03F0, "b.kappav", "GREEK KAPPA SYMBOL" },
{ 0x03F0, "kappav", "GREEK KAPPA SYMBOL" },
{ 0x03F1, "b.rhov", "GREEK RHO SYMBOL" },
{ 0x03F1, "rhov", "GREEK RHO SYMBOL" },
{ 0x0401, "IOcy", "CYRILLIC CAPITAL LETTER IO" },
{ 0x0402, "DJcy", "CYRILLIC CAPITAL LETTER DJE" },
{ 0x0403, "GJcy", "CYRILLIC CAPITAL LETTER GJE" },
{ 0x0404, "Jukcy", "CYRILLIC CAPITAL LETTER UKRAINIAN IE" },
{ 0x0405, "DScy", "CYRILLIC CAPITAL LETTER DZE" },
{ 0x0406, "Iukcy", "CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I" },
{ 0x0407, "YIcy", "CYRILLIC CAPITAL LETTER YI" },
{ 0x0408, "Jsercy", "CYRILLIC CAPITAL LETTER JE" },
{ 0x0409, "LJcy", "CYRILLIC CAPITAL LETTER LJE" },
{ 0x040A, "NJcy", "CYRILLIC CAPITAL LETTER NJE" },
{ 0x040B, "TSHcy", "CYRILLIC CAPITAL LETTER TSHE" },
{ 0x040C, "KJcy", "CYRILLIC CAPITAL LETTER KJE" },
{ 0x040E, "Ubrcy", "CYRILLIC CAPITAL LETTER SHORT U" },
{ 0x040F, "DZcy", "CYRILLIC CAPITAL LETTER DZHE" },
{ 0x0410, "Acy", "CYRILLIC CAPITAL LETTER A" },
{ 0x0411, "Bcy", "CYRILLIC CAPITAL LETTER BE" },
{ 0x0412, "Vcy", "CYRILLIC CAPITAL LETTER VE" },
{ 0x0413, "Gcy", "CYRILLIC CAPITAL LETTER GHE" },
{ 0x0414, "Dcy", "CYRILLIC CAPITAL LETTER DE" },
{ 0x0415, "IEcy", "CYRILLIC CAPITAL LETTER IE" },
{ 0x0416, "ZHcy", "CYRILLIC CAPITAL LETTER ZHE" },
{ 0x0417, "Zcy", "CYRILLIC CAPITAL LETTER ZE" },
{ 0x0418, "Icy", "CYRILLIC CAPITAL LETTER I" },
{ 0x0419, "Jcy", "CYRILLIC CAPITAL LETTER SHORT I" },
{ 0x041A, "Kcy", "CYRILLIC CAPITAL LETTER KA" },
{ 0x041B, "Lcy", "CYRILLIC CAPITAL LETTER EL" },
{ 0x041C, "Mcy", "CYRILLIC CAPITAL LETTER EM" },
{ 0x041D, "Ncy", "CYRILLIC CAPITAL LETTER EN" },
{ 0x041E, "Ocy", "CYRILLIC CAPITAL LETTER O" },
{ 0x041F, "Pcy", "CYRILLIC CAPITAL LETTER PE" },
{ 0x0420, "Rcy", "CYRILLIC CAPITAL LETTER ER" },
{ 0x0421, "Scy", "CYRILLIC CAPITAL LETTER ES" },
{ 0x0422, "Tcy", "CYRILLIC CAPITAL LETTER TE" },
{ 0x0423, "Ucy", "CYRILLIC CAPITAL LETTER U" },
{ 0x0424, "Fcy", "CYRILLIC CAPITAL LETTER EF" },
{ 0x0425, "KHcy", "CYRILLIC CAPITAL LETTER HA" },
{ 0x0426, "TScy", "CYRILLIC CAPITAL LETTER TSE" },
{ 0x0427, "CHcy", "CYRILLIC CAPITAL LETTER CHE" },
{ 0x0428, "SHcy", "CYRILLIC CAPITAL LETTER SHA" },
{ 0x0429, "SHCHcy", "CYRILLIC CAPITAL LETTER SHCHA" },
{ 0x042A, "HARDcy", "CYRILLIC CAPITAL LETTER HARD SIGN" },
{ 0x042B, "Ycy", "CYRILLIC CAPITAL LETTER YERU" },
{ 0x042C, "SOFTcy", "CYRILLIC CAPITAL LETTER SOFT SIGN" },
{ 0x042D, "Ecy", "CYRILLIC CAPITAL LETTER E" },
{ 0x042E, "YUcy", "CYRILLIC CAPITAL LETTER YU" },
{ 0x042F, "YAcy", "CYRILLIC CAPITAL LETTER YA" },
{ 0x0430, "acy", "CYRILLIC SMALL LETTER A" },
{ 0x0431, "bcy", "CYRILLIC SMALL LETTER BE" },
{ 0x0432, "vcy", "CYRILLIC SMALL LETTER VE" },
{ 0x0433, "gcy", "CYRILLIC SMALL LETTER GHE" },
{ 0x0434, "dcy", "CYRILLIC SMALL LETTER DE" },
{ 0x0435, "iecy", "CYRILLIC SMALL LETTER IE" },
{ 0x0436, "zhcy", "CYRILLIC SMALL LETTER ZHE" },
{ 0x0437, "zcy", "CYRILLIC SMALL LETTER ZE" },
{ 0x0438, "icy", "CYRILLIC SMALL LETTER I" },
{ 0x0439, "jcy", "CYRILLIC SMALL LETTER SHORT I" },
{ 0x043A, "kcy", "CYRILLIC SMALL LETTER KA" },
{ 0x043B, "lcy", "CYRILLIC SMALL LETTER EL" },
{ 0x043C, "mcy", "CYRILLIC SMALL LETTER EM" },
{ 0x043D, "ncy", "CYRILLIC SMALL LETTER EN" },
{ 0x043E, "ocy", "CYRILLIC SMALL LETTER O" },
{ 0x043F, "pcy", "CYRILLIC SMALL LETTER PE" },
{ 0x0440, "rcy", "CYRILLIC SMALL LETTER ER" },
{ 0x0441, "scy", "CYRILLIC SMALL LETTER ES" },
{ 0x0442, "tcy", "CYRILLIC SMALL LETTER TE" },
{ 0x0443, "ucy", "CYRILLIC SMALL LETTER U" },
{ 0x0444, "fcy", "CYRILLIC SMALL LETTER EF" },
{ 0x0445, "khcy", "CYRILLIC SMALL LETTER HA" },
{ 0x0446, "tscy", "CYRILLIC SMALL LETTER TSE" },
{ 0x0447, "chcy", "CYRILLIC SMALL LETTER CHE" },
{ 0x0448, "shcy", "CYRILLIC SMALL LETTER SHA" },
{ 0x0449, "shchcy", "CYRILLIC SMALL LETTER SHCHA" },
{ 0x044A, "hardcy", "CYRILLIC SMALL LETTER HARD SIGN" },
{ 0x044B, "ycy", "CYRILLIC SMALL LETTER YERU" },
{ 0x044C, "softcy", "CYRILLIC SMALL LETTER SOFT SIGN" },
{ 0x044D, "ecy", "CYRILLIC SMALL LETTER E" },
{ 0x044E, "yucy", "CYRILLIC SMALL LETTER YU" },
{ 0x044F, "yacy", "CYRILLIC SMALL LETTER YA" },
{ 0x0451, "iocy", "CYRILLIC SMALL LETTER IO" },
{ 0x0452, "djcy", "CYRILLIC SMALL LETTER DJE" },
{ 0x0453, "gjcy", "CYRILLIC SMALL LETTER GJE" },
{ 0x0454, "jukcy", "CYRILLIC SMALL LETTER UKRAINIAN IE" },
{ 0x0455, "dscy", "CYRILLIC SMALL LETTER DZE" },
{ 0x0456, "iukcy", "CYRILLIC SMALL LETTER BYELORUSSIAN-UKRAINIAN I" },
{ 0x0457, "yicy", "CYRILLIC SMALL LETTER YI" },
{ 0x0458, "jsercy", "CYRILLIC SMALL LETTER JE" },
{ 0x0459, "ljcy", "CYRILLIC SMALL LETTER LJE" },
{ 0x045A, "njcy", "CYRILLIC SMALL LETTER NJE" },
{ 0x045B, "tshcy", "CYRILLIC SMALL LETTER TSHE" },
{ 0x045C, "kjcy", "CYRILLIC SMALL LETTER KJE" },
{ 0x045E, "ubrcy", "CYRILLIC SMALL LETTER SHORT U" },
{ 0x045F, "dzcy", "CYRILLIC SMALL LETTER DZHE" },
{ 0x2002, "ensp", "EN SPACE" },
{ 0x2003, "emsp", "EM SPACE" },
{ 0x2004, "emsp13", "THREE-PER-EM SPACE" },
{ 0x2005, "emsp14", "FOUR-PER-EM SPACE" },
{ 0x2007, "numsp", "FIGURE SPACE" },
{ 0x2008, "puncsp", "PUNCTUATION SPACE" },
{ 0x2009, "thinsp", "THIN SPACE" },
{ 0x200A, "hairsp", "HAIR SPACE" },
{ 0x2010, "dash", "HYPHEN" },
{ 0x2013, "ndash", "EN DASH" },
{ 0x2014, "mdash", "EM DASH" },
{ 0x2015, "horbar", "HORIZONTAL BAR" },
{ 0x2016, "Verbar", "DOUBLE VERTICAL LINE" },
{ 0x2018, "lsquo", "" },
{ 0x2018, "rsquor", "" },
{ 0x2019, "rsquo", "RIGHT SINGLE QUOTATION MARK" },
{ 0x201A, "lsquor", "SINGLE LOW-9 QUOTATION MARK" },
{ 0x201C, "ldquo", "" },
{ 0x201C, "rdquor", "" },
{ 0x201D, "rdquo", "RIGHT DOUBLE QUOTATION MARK" },
{ 0x201E, "ldquor", "DOUBLE LOW-9 QUOTATION MARK" },
{ 0x2020, "dagger", "DAGGER" },
{ 0x2021, "Dagger", "DOUBLE DAGGER" },
{ 0x2022, "bull", "BULLET" },
{ 0x2025, "nldr", "TWO DOT LEADER" },
{ 0x2026, "hellip", "HORIZONTAL ELLIPSIS" },
{ 0x2026, "mldr", "HORIZONTAL ELLIPSIS" },
{ 0x2030, "permil", "PER MILLE SIGN" },
{ 0x2032, "prime", "PRIME" },
{ 0x2032, "vprime", "PRIME" },
{ 0x2033, "Prime", "DOUBLE PRIME" },
{ 0x2034, "tprime", "TRIPLE PRIME" },
{ 0x2035, "bprime", "REVERSED PRIME" },
{ 0x2041, "caret", "CARET" },
{ 0x2043, "hybull", "HYPHEN BULLET" },
{ 0x20DB, "tdot", "COMBINING THREE DOTS ABOVE" },
{ 0x20DC, "DotDot", "COMBINING FOUR DOTS ABOVE" },
{ 0x2105, "incare", "CARE OF" },
{ 0x210B, "hamilt", "SCRIPT CAPITAL H" },
{ 0x210F, "planck", "PLANCK CONSTANT OVER TWO PI" },
{ 0x2111, "image", "BLACK-LETTER CAPITAL I" },
{ 0x2112, "lagran", "SCRIPT CAPITAL L" },
{ 0x2113, "ell", "SCRIPT SMALL L" },
{ 0x2116, "numero", "NUMERO SIGN" },
{ 0x2117, "copysr", "SOUND RECORDING COPYRIGHT" },
{ 0x2118, "weierp", "SCRIPT CAPITAL P" },
{ 0x211C, "real", "BLACK-LETTER CAPITAL R" },
{ 0x211E, "rx", "PRESCRIPTION TAKE" },
{ 0x2122, "trade", "TRADE MARK SIGN" },
{ 0x2126, "ohm", "OHM SIGN" },
{ 0x212B, "angst", "ANGSTROM SIGN" },
{ 0x212C, "bernou", "SCRIPT CAPITAL B" },
{ 0x2133, "phmmat", "SCRIPT CAPITAL M" },
{ 0x2134, "order", "SCRIPT SMALL O" },
{ 0x2135, "aleph", "ALEF SYMBOL" },
{ 0x2136, "beth", "BET SYMBOL" },
{ 0x2137, "gimel", "GIMEL SYMBOL" },
{ 0x2138, "daleth", "DALET SYMBOL" },
{ 0x2153, "frac13", "VULGAR FRACTION ONE THIRD" },
{ 0x2154, "frac23", "VULGAR FRACTION TWO THIRDS" },
{ 0x2155, "frac15", "VULGAR FRACTION ONE FIFTH" },
{ 0x2156, "frac25", "VULGAR FRACTION TWO FIFTHS" },
{ 0x2157, "frac35", "VULGAR FRACTION THREE FIFTHS" },
{ 0x2158, "frac45", "VULGAR FRACTION FOUR FIFTHS" },
{ 0x2159, "frac16", "VULGAR FRACTION ONE SIXTH" },
{ 0x215A, "frac56", "VULGAR FRACTION FIVE SIXTHS" },
{ 0x215B, "frac18", "" },
{ 0x215C, "frac38", "" },
{ 0x215D, "frac58", "" },
{ 0x215E, "frac78", "" },
{ 0x2190, "larr", "LEFTWARDS DOUBLE ARROW" },
{ 0x2191, "uarr", "UPWARDS ARROW" },
{ 0x2192, "rarr", "RIGHTWARDS DOUBLE ARROW" },
{ 0x2193, "darr", "DOWNWARDS ARROW" },
{ 0x2194, "harr", "LEFT RIGHT ARROW" },
{ 0x2194, "xhArr", "LEFT RIGHT ARROW" },
{ 0x2194, "xharr", "LEFT RIGHT ARROW" },
{ 0x2195, "varr", "UP DOWN ARROW" },
{ 0x2196, "nwarr", "NORTH WEST ARROW" },
{ 0x2197, "nearr", "NORTH EAST ARROW" },
{ 0x2198, "drarr", "SOUTH EAST ARROW" },
{ 0x2199, "dlarr", "SOUTH WEST ARROW" },
{ 0x219A, "nlarr", "LEFTWARDS ARROW WITH STROKE" },
{ 0x219B, "nrarr", "RIGHTWARDS ARROW WITH STROKE" },
{ 0x219D, "rarrw", "RIGHTWARDS SQUIGGLE ARROW" },
{ 0x219E, "Larr", "LEFTWARDS TWO HEADED ARROW" },
{ 0x21A0, "Rarr", "RIGHTWARDS TWO HEADED ARROW" },
{ 0x21A2, "larrtl", "LEFTWARDS ARROW WITH TAIL" },
{ 0x21A3, "rarrtl", "RIGHTWARDS ARROW WITH TAIL" },
{ 0x21A6, "map", "RIGHTWARDS ARROW FROM BAR" },
{ 0x21A9, "larrhk", "LEFTWARDS ARROW WITH HOOK" },
{ 0x21AA, "rarrhk", "RIGHTWARDS ARROW WITH HOOK" },
{ 0x21AB, "larrlp", "LEFTWARDS ARROW WITH LOOP" },
{ 0x21AC, "rarrlp", "RIGHTWARDS ARROW WITH LOOP" },
{ 0x21AD, "harrw", "LEFT RIGHT WAVE ARROW" },
{ 0x21AE, "nharr", "LEFT RIGHT ARROW WITH STROKE" },
{ 0x21B0, "lsh", "UPWARDS ARROW WITH TIP LEFTWARDS" },
{ 0x21B1, "rsh", "UPWARDS ARROW WITH TIP RIGHTWARDS" },
{ 0x21B6, "cularr", "ANTICLOCKWISE TOP SEMICIRCLE ARROW" },
{ 0x21B7, "curarr", "CLOCKWISE TOP SEMICIRCLE ARROW" },
{ 0x21BA, "olarr", "ANTICLOCKWISE OPEN CIRCLE ARROW" },
{ 0x21BB, "orarr", "CLOCKWISE OPEN CIRCLE ARROW" },
{ 0x21BC, "lharu", "LEFTWARDS HARPOON WITH BARB UPWARDS" },
{ 0x21BD, "lhard", "LEFTWARDS HARPOON WITH BARB DOWNWARDS" },
{ 0x21BE, "uharr", "UPWARDS HARPOON WITH BARB RIGHTWARDS" },
{ 0x21BF, "uharl", "UPWARDS HARPOON WITH BARB LEFTWARDS" },
{ 0x21C0, "rharu", "RIGHTWARDS HARPOON WITH BARB UPWARDS" },
{ 0x21C1, "rhard", "RIGHTWARDS HARPOON WITH BARB DOWNWARDS" },
{ 0x21C2, "dharr", "DOWNWARDS HARPOON WITH BARB RIGHTWARDS" },
{ 0x21C3, "dharl", "DOWNWARDS HARPOON WITH BARB LEFTWARDS" },
{ 0x21C4, "rlarr2", "RIGHTWARDS ARROW OVER LEFTWARDS ARROW" },
{ 0x21C6, "lrarr2", "LEFTWARDS ARROW OVER RIGHTWARDS ARROW" },
{ 0x21C7, "larr2", "LEFTWARDS PAIRED ARROWS" },
{ 0x21C8, "uarr2", "UPWARDS PAIRED ARROWS" },
{ 0x21C9, "rarr2", "RIGHTWARDS PAIRED ARROWS" },
{ 0x21CA, "darr2", "DOWNWARDS PAIRED ARROWS" },
{ 0x21CB, "lrhar2", "LEFTWARDS HARPOON OVER RIGHTWARDS HARPOON" },
{ 0x21CC, "rlhar2", "RIGHTWARDS HARPOON OVER LEFTWARDS HARPOON" },
{ 0x21CD, "nlArr", "LEFTWARDS DOUBLE ARROW WITH STROKE" },
{ 0x21CE, "nhArr", "LEFT RIGHT DOUBLE ARROW WITH STROKE" },
{ 0x21CF, "nrArr", "RIGHTWARDS DOUBLE ARROW WITH STROKE" },
{ 0x21D0, "lArr", "LEFTWARDS ARROW" },
{ 0x21D0, "xlArr", "LEFTWARDS DOUBLE ARROW" },
{ 0x21D1, "uArr", "UPWARDS DOUBLE ARROW" },
{ 0x21D2, "rArr", "RIGHTWARDS ARROW" },
{ 0x21D2, "xrArr", "RIGHTWARDS DOUBLE ARROW" },
{ 0x21D3, "dArr", "DOWNWARDS DOUBLE ARROW" },
{ 0x21D4, "hArr", "" },
{ 0x21D4, "iff", "LEFT RIGHT DOUBLE ARROW" },
{ 0x21D5, "vArr", "UP DOWN DOUBLE ARROW" },
{ 0x21DA, "lAarr", "LEFTWARDS TRIPLE ARROW" },
{ 0x21DB, "rAarr", "RIGHTWARDS TRIPLE ARROW" },
{ 0x2200, "forall", "" },
{ 0x2201, "comp", "COMPLEMENT" },
{ 0x2202, "part", "" },
{ 0x2203, "exist", "" },
{ 0x2204, "nexist", "THERE DOES NOT EXIST" },
{ 0x2205, "empty", "" },
{ 0x2207, "nabla", "NABLA" },
{ 0x2209, "notin", "" },
{ 0x220A, "epsi", "" },
{ 0x220A, "epsis", "" },
{ 0x220A, "isin", "" },
{ 0x220D, "bepsi", "SMALL CONTAINS AS MEMBER" },
{ 0x220D, "ni", "" },
{ 0x220F, "prod", "N-ARY PRODUCT" },
{ 0x2210, "amalg", "N-ARY COPRODUCT" },
{ 0x2210, "coprod", "N-ARY COPRODUCT" },
{ 0x2210, "samalg", "" },
{ 0x2211, "sum", "N-ARY SUMMATION" },
{ 0x2212, "minus", "MINUS SIGN" },
{ 0x2213, "mnplus", "" },
{ 0x2214, "plusdo", "DOT PLUS" },
{ 0x2216, "setmn", "SET MINUS" },
{ 0x2216, "ssetmn", "SET MINUS" },
{ 0x2217, "lowast", "ASTERISK OPERATOR" },
{ 0x2218, "compfn", "RING OPERATOR" },
{ 0x221A, "radic", "" },
{ 0x221D, "prop", "" },
{ 0x221D, "vprop", "" },
{ 0x221E, "infin", "" },
{ 0x221F, "ang90", "RIGHT ANGLE" },
{ 0x2220, "ang", "ANGLE" },
{ 0x2221, "angmsd", "MEASURED ANGLE" },
{ 0x2222, "angsph", "" },
{ 0x2223, "mid", "" },
{ 0x2224, "nmid", "DOES NOT DIVIDE" },
{ 0x2225, "par", "PARALLEL TO" },
{ 0x2225, "spar", "PARALLEL TO" },
{ 0x2226, "npar", "NOT PARALLEL TO" },
{ 0x2226, "nspar", "NOT PARALLEL TO" },
{ 0x2227, "and", "" },
{ 0x2228, "or", "" },
{ 0x2229, "cap", "" },
{ 0x222A, "cup", "" },
{ 0x222B, "int", "" },
{ 0x222E, "conint", "" },
{ 0x2234, "there4", "" },
{ 0x2235, "becaus", "BECAUSE" },
{ 0x223C, "sim", "" },
{ 0x223C, "thksim", "TILDE OPERATOR" },
{ 0x223D, "bsim", "" },
{ 0x2240, "wreath", "WREATH PRODUCT" },
{ 0x2241, "nsim", "" },
{ 0x2243, "sime", "" },
{ 0x2244, "nsime", "" },
{ 0x2245, "cong", "" },
{ 0x2247, "ncong", "NEITHER APPROXIMATELY NOR ACTUALLY EQUAL TO" },
{ 0x2248, "ap", "" },
{ 0x2248, "thkap", "ALMOST EQUAL TO" },
{ 0x2249, "nap", "NOT ALMOST EQUAL TO" },
{ 0x224A, "ape", "" },
{ 0x224C, "bcong", "ALL EQUAL TO" },
{ 0x224D, "asymp", "EQUIVALENT TO" },
{ 0x224E, "bump", "" },
{ 0x224F, "bumpe", "" },
{ 0x2250, "esdot", "" },
{ 0x2251, "eDot", "" },
{ 0x2252, "efDot", "" },
{ 0x2253, "erDot", "" },
{ 0x2254, "colone", "" },
{ 0x2255, "ecolon", "" },
{ 0x2256, "ecir", "" },
{ 0x2257, "cire", "" },
{ 0x2259, "wedgeq", "ESTIMATES" },
{ 0x225C, "trie", "" },
{ 0x2260, "ne", "" },
{ 0x2261, "equiv", "" },
{ 0x2262, "nequiv", "NOT IDENTICAL TO" },
{ 0x2264, "le", "" },
{ 0x2264, "les", "LESS-THAN OR EQUAL TO" },
{ 0x2265, "ge", "GREATER-THAN OR EQUAL TO" },
{ 0x2265, "ges", "GREATER-THAN OR EQUAL TO" },
{ 0x2266, "lE", "" },
{ 0x2267, "gE", "" },
{ 0x2268, "lnE", "" },
{ 0x2268, "lne", "" },
{ 0x2268, "lvnE", "LESS-THAN BUT NOT EQUAL TO" },
{ 0x2269, "gnE", "" },
{ 0x2269, "gne", "" },
{ 0x2269, "gvnE", "GREATER-THAN BUT NOT EQUAL TO" },
{ 0x226A, "Lt", "MUCH LESS-THAN" },
{ 0x226B, "Gt", "MUCH GREATER-THAN" },
{ 0x226C, "twixt", "BETWEEN" },
{ 0x226E, "nlt", "NOT LESS-THAN" },
{ 0x226F, "ngt", "NOT GREATER-THAN" },
{ 0x2270, "nlE", "" },
{ 0x2270, "nle", "NEITHER LESS-THAN NOR EQUAL TO" },
{ 0x2270, "nles", "" },
{ 0x2271, "ngE", "" },
{ 0x2271, "nge", "NEITHER GREATER-THAN NOR EQUAL TO" },
{ 0x2271, "nges", "" },
{ 0x2272, "lap", "LESS-THAN OR EQUIVALENT TO" },
{ 0x2272, "lsim", "LESS-THAN OR EQUIVALENT TO" },
{ 0x2273, "gap", "GREATER-THAN OR EQUIVALENT TO" },
{ 0x2273, "gsim", "GREATER-THAN OR EQUIVALENT TO" },
{ 0x2276, "lg", "LESS-THAN OR GREATER-THAN" },
{ 0x2277, "gl", "" },
{ 0x227A, "pr", "" },
{ 0x227B, "sc", "" },
{ 0x227C, "cupre", "" },
{ 0x227C, "pre", "" },
{ 0x227D, "sccue", "" },
{ 0x227D, "sce", "" },
{ 0x227E, "prap", "" },
{ 0x227E, "prsim", "" },
{ 0x227F, "scap", "" },
{ 0x227F, "scsim", "" },
{ 0x2280, "npr", "DOES NOT PRECEDE" },
{ 0x2281, "nsc", "DOES NOT SUCCEED" },
{ 0x2282, "sub", "" },
{ 0x2283, "sup", "" },
{ 0x2284, "nsub", "NOT A SUBSET OF" },
{ 0x2285, "nsup", "NOT A SUPERSET OF" },
{ 0x2286, "subE", "" },
{ 0x2286, "sube", "" },
{ 0x2287, "supE", "" },
{ 0x2287, "supe", "" },
{ 0x2288, "nsubE", "" },
{ 0x2288, "nsube", "" },
{ 0x2289, "nsupE", "" },
{ 0x2289, "nsupe", "" },
{ 0x228A, "subne", "" },
{ 0x228A, "subnE", "SUBSET OF WITH NOT EQUAL TO" },
{ 0x228A, "vsubne", "SUBSET OF WITH NOT EQUAL TO" },
{ 0x228B, "supnE", "" },
{ 0x228B, "supne", "" },
{ 0x228B, "vsupnE", "SUPERSET OF WITH NOT EQUAL TO" },
{ 0x228B, "vsupne", "SUPERSET OF WITH NOT EQUAL TO" },
{ 0x228E, "uplus", "MULTISET UNION" },
{ 0x228F, "sqsub", "" },
{ 0x2290, "sqsup", "" },
{ 0x2291, "sqsube", "" },
{ 0x2292, "sqsupe", "" },
{ 0x2293, "sqcap", "SQUARE CAP" },
{ 0x2294, "sqcup", "SQUARE CUP" },
{ 0x2295, "oplus", "CIRCLED PLUS" },
{ 0x2296, "ominus", "CIRCLED MINUS" },
{ 0x2297, "otimes", "CIRCLED TIMES" },
{ 0x2298, "osol", "CIRCLED DIVISION SLASH" },
{ 0x2299, "odot", "CIRCLED DOT OPERATOR" },
{ 0x229A, "ocir", "CIRCLED RING OPERATOR" },
{ 0x229B, "oast", "CIRCLED ASTERISK OPERATOR" },
{ 0x229D, "odash", "CIRCLED DASH" },
{ 0x229E, "plusb", "SQUARED PLUS" },
{ 0x229F, "minusb", "SQUARED MINUS" },
{ 0x22A0, "timesb", "SQUARED TIMES" },
{ 0x22A1, "sdotb", "SQUARED DOT OPERATOR" },
{ 0x22A2, "vdash", "" },
{ 0x22A3, "dashv", "" },
{ 0x22A4, "top", "DOWN TACK" },
{ 0x22A5, "bottom", "" },
{ 0x22A5, "perp", "" },
{ 0x22A7, "models", "MODELS" },
{ 0x22A8, "vDash", "" },
{ 0x22A9, "Vdash", "" },
{ 0x22AA, "Vvdash", "" },
{ 0x22AC, "nvdash", "DOES NOT PROVE" },
{ 0x22AD, "nvDash", "NOT TRUE" },
{ 0x22AE, "nVdash", "DOES NOT FORCE" },
{ 0x22AF, "nVDash", "NEGATED DOUBLE VERTICAL BAR DOUBLE RIGHT TURNSTILE" },
{ 0x22B2, "vltri", "" },
{ 0x22B3, "vrtri", "" },
{ 0x22B4, "ltrie", "" },
{ 0x22B5, "rtrie", "" },
{ 0x22B8, "mumap", "MULTIMAP" },
{ 0x22BA, "intcal", "INTERCALATE" },
{ 0x22BB, "veebar", "" },
{ 0x22BC, "barwed", "NAND" },
{ 0x22C4, "diam", "DIAMOND OPERATOR" },
{ 0x22C5, "sdot", "DOT OPERATOR" },
{ 0x22C6, "sstarf", "STAR OPERATOR" },
{ 0x22C6, "star", "STAR OPERATOR" },
{ 0x22C7, "divonx", "DIVISION TIMES" },
{ 0x22C8, "bowtie", "" },
{ 0x22C9, "ltimes", "LEFT NORMAL FACTOR SEMIDIRECT PRODUCT" },
{ 0x22CA, "rtimes", "RIGHT NORMAL FACTOR SEMIDIRECT PRODUCT" },
{ 0x22CB, "lthree", "LEFT SEMIDIRECT PRODUCT" },
{ 0x22CC, "rthree", "RIGHT SEMIDIRECT PRODUCT" },
{ 0x22CD, "bsime", "" },
{ 0x22CE, "cuvee", "CURLY LOGICAL OR" },
{ 0x22CF, "cuwed", "CURLY LOGICAL AND" },
{ 0x22D0, "Sub", "" },
{ 0x22D1, "Sup", "" },
{ 0x22D2, "Cap", "DOUBLE INTERSECTION" },
{ 0x22D3, "Cup", "DOUBLE UNION" },
{ 0x22D4, "fork", "" },
{ 0x22D6, "ldot", "" },
{ 0x22D7, "gsdot", "" },
{ 0x22D8, "Ll", "" },
{ 0x22D9, "Gg", "VERY MUCH GREATER-THAN" },
{ 0x22DA, "lEg", "" },
{ 0x22DA, "leg", "" },
{ 0x22DB, "gEl", "" },
{ 0x22DB, "gel", "" },
{ 0x22DC, "els", "" },
{ 0x22DD, "egs", "" },
{ 0x22DE, "cuepr", "" },
{ 0x22DF, "cuesc", "" },
{ 0x22E0, "npre", "DOES NOT PRECEDE OR EQUAL" },
{ 0x22E1, "nsce", "DOES NOT SUCCEED OR EQUAL" },
{ 0x22E6, "lnsim", "" },
{ 0x22E7, "gnsim", "GREATER-THAN BUT NOT EQUIVALENT TO" },
{ 0x22E8, "prnap", "" },
{ 0x22E8, "prnsim", "" },
{ 0x22E9, "scnap", "" },
{ 0x22E9, "scnsim", "" },
{ 0x22EA, "nltri", "NOT NORMAL SUBGROUP OF" },
{ 0x22EB, "nrtri", "DOES NOT CONTAIN AS NORMAL SUBGROUP" },
{ 0x22EC, "nltrie", "NOT NORMAL SUBGROUP OF OR EQUAL TO" },
{ 0x22ED, "nrtrie", "DOES NOT CONTAIN AS NORMAL SUBGROUP OR EQUAL" },
{ 0x22EE, "vellip", "" },
{ 0x2306, "Barwed", "PERSPECTIVE" },
{ 0x2308, "lceil", "LEFT CEILING" },
{ 0x2309, "rceil", "RIGHT CEILING" },
{ 0x230A, "lfloor", "LEFT FLOOR" },
{ 0x230B, "rfloor", "RIGHT FLOOR" },
{ 0x230C, "drcrop", "BOTTOM RIGHT CROP" },
{ 0x230D, "dlcrop", "BOTTOM LEFT CROP" },
{ 0x230E, "urcrop", "TOP RIGHT CROP" },
{ 0x230F, "ulcrop", "TOP LEFT CROP" },
{ 0x2315, "telrec", "TELEPHONE RECORDER" },
{ 0x2316, "target", "POSITION INDICATOR" },
{ 0x231C, "ulcorn", "TOP LEFT CORNER" },
{ 0x231D, "urcorn", "TOP RIGHT CORNER" },
{ 0x231E, "dlcorn", "BOTTOM LEFT CORNER" },
{ 0x231F, "drcorn", "BOTTOM RIGHT CORNER" },
{ 0x2322, "frown", "" },
{ 0x2322, "sfrown", "FROWN" },
{ 0x2323, "smile", "" },
{ 0x2323, "ssmile", "SMILE" },
{ 0x2423, "blank", "OPEN BOX" },
{ 0x24C8, "oS", "CIRCLED LATIN CAPITAL LETTER S" },
{ 0x2500, "boxh", "BOX DRAWINGS LIGHT HORIZONTAL" },
{ 0x2502, "boxv", "BOX DRAWINGS LIGHT VERTICAL" },
{ 0x250C, "boxdr", "BOX DRAWINGS LIGHT DOWN AND RIGHT" },
{ 0x2510, "boxdl", "BOX DRAWINGS LIGHT DOWN AND LEFT" },
{ 0x2514, "boxur", "BOX DRAWINGS LIGHT UP AND RIGHT" },
{ 0x2518, "boxul", "BOX DRAWINGS LIGHT UP AND LEFT" },
{ 0x251C, "boxvr", "BOX DRAWINGS LIGHT VERTICAL AND RIGHT" },
{ 0x2524, "boxvl", "BOX DRAWINGS LIGHT VERTICAL AND LEFT" },
{ 0x252C, "boxhd", "BOX DRAWINGS LIGHT DOWN AND HORIZONTAL" },
{ 0x2534, "boxhu", "BOX DRAWINGS LIGHT UP AND HORIZONTAL" },
{ 0x253C, "boxvh", "BOX DRAWINGS LIGHT VERTICAL AND HORIZONTAL" },
{ 0x2550, "boxH", "BOX DRAWINGS DOUBLE HORIZONTAL" },
{ 0x2551, "boxV", "BOX DRAWINGS DOUBLE VERTICAL" },
{ 0x2552, "boxDR", "BOX DRAWINGS DOWN SINGLE AND RIGHT DOUBLE" },
{ 0x2553, "boxDr", "BOX DRAWINGS DOWN DOUBLE AND RIGHT SINGLE" },
{ 0x2554, "boxdR", "BOX DRAWINGS DOUBLE DOWN AND RIGHT" },
{ 0x2555, "boxDL", "BOX DRAWINGS DOWN SINGLE AND LEFT DOUBLE" },
{ 0x2556, "boxdL", "BOX DRAWINGS DOWN DOUBLE AND LEFT SINGLE" },
{ 0x2557, "boxDl", "BOX DRAWINGS DOUBLE DOWN AND LEFT" },
{ 0x2558, "boxUR", "BOX DRAWINGS UP SINGLE AND RIGHT DOUBLE" },
{ 0x2559, "boxuR", "BOX DRAWINGS UP DOUBLE AND RIGHT SINGLE" },
{ 0x255A, "boxUr", "BOX DRAWINGS DOUBLE UP AND RIGHT" },
{ 0x255B, "boxUL", "BOX DRAWINGS UP SINGLE AND LEFT DOUBLE" },
{ 0x255C, "boxUl", "BOX DRAWINGS UP DOUBLE AND LEFT SINGLE" },
{ 0x255D, "boxuL", "BOX DRAWINGS DOUBLE UP AND LEFT" },
{ 0x255E, "boxvR", "BOX DRAWINGS VERTICAL SINGLE AND RIGHT DOUBLE" },
{ 0x255F, "boxVR", "BOX DRAWINGS VERTICAL DOUBLE AND RIGHT SINGLE" },
{ 0x2560, "boxVr", "BOX DRAWINGS DOUBLE VERTICAL AND RIGHT" },
{ 0x2561, "boxvL", "BOX DRAWINGS VERTICAL SINGLE AND LEFT DOUBLE" },
{ 0x2562, "boxVL", "BOX DRAWINGS VERTICAL DOUBLE AND LEFT SINGLE" },
{ 0x2563, "boxVl", "BOX DRAWINGS DOUBLE VERTICAL AND LEFT" },
{ 0x2564, "boxhD", "BOX DRAWINGS DOWN SINGLE AND HORIZONTAL DOUBLE" },
{ 0x2565, "boxHD", "BOX DRAWINGS DOWN DOUBLE AND HORIZONTAL SINGLE" },
{ 0x2566, "boxHd", "BOX DRAWINGS DOUBLE DOWN AND HORIZONTAL" },
{ 0x2567, "boxhU", "BOX DRAWINGS UP SINGLE AND HORIZONTAL DOUBLE" },
{ 0x2568, "boxHU", "BOX DRAWINGS UP DOUBLE AND HORIZONTAL SINGLE" },
{ 0x2569, "boxHu", "BOX DRAWINGS DOUBLE UP AND HORIZONTAL" },
{ 0x256A, "boxvH", "BOX DRAWINGS VERTICAL SINGLE AND HORIZONTAL DOUBLE" },
{ 0x256B, "boxVH", "BOX DRAWINGS VERTICAL DOUBLE AND HORIZONTAL SINGLE" },
{ 0x256C, "boxVh", "BOX DRAWINGS DOUBLE VERTICAL AND HORIZONTAL" },
{ 0x2580, "uhblk", "UPPER HALF BLOCK" },
{ 0x2584, "lhblk", "LOWER HALF BLOCK" },
{ 0x2588, "block", "FULL BLOCK" },
{ 0x2591, "blk14", "LIGHT SHADE" },
{ 0x2592, "blk12", "MEDIUM SHADE" },
{ 0x2593, "blk34", "DARK SHADE" },
{ 0x25A1, "square", "WHITE SQUARE" },
{ 0x25A1, "squ", "WHITE SQUARE" },
{ 0x25AA, "squf", "" },
{ 0x25AD, "rect", "WHITE RECTANGLE" },
{ 0x25AE, "marker", "BLACK VERTICAL RECTANGLE" },
{ 0x25B3, "xutri", "WHITE UP-POINTING TRIANGLE" },
{ 0x25B4, "utrif", "BLACK UP-POINTING TRIANGLE" },
{ 0x25B5, "utri", "WHITE UP-POINTING TRIANGLE" },
{ 0x25B8, "rtrif", "BLACK RIGHT-POINTING TRIANGLE" },
{ 0x25B9, "rtri", "WHITE RIGHT-POINTING TRIANGLE" },
{ 0x25BD, "xdtri", "WHITE DOWN-POINTING TRIANGLE" },
{ 0x25BE, "dtrif", "BLACK DOWN-POINTING TRIANGLE" },
{ 0x25BF, "dtri", "WHITE DOWN-POINTING TRIANGLE" },
{ 0x25C2, "ltrif", "BLACK LEFT-POINTING TRIANGLE" },
{ 0x25C3, "ltri", "WHITE LEFT-POINTING TRIANGLE" },
{ 0x25CA, "loz", "LOZENGE" },
{ 0x25CB, "cir", "WHITE CIRCLE" },
{ 0x25CB, "xcirc", "WHITE CIRCLE" },
{ 0x2605, "starf", "BLACK STAR" },
{ 0x260E, "phone", "TELEPHONE SIGN" },
{ 0x2640, "female", "" },
{ 0x2642, "male", "MALE SIGN" },
{ 0x2660, "spades", "BLACK SPADE SUIT" },
{ 0x2663, "clubs", "BLACK CLUB SUIT" },
{ 0x2665, "hearts", "BLACK HEART SUIT" },
{ 0x2666, "diams", "BLACK DIAMOND SUIT" },
{ 0x2669, "sung", "" },
{ 0x266D, "flat", "MUSIC FLAT SIGN" },
{ 0x266E, "natur", "MUSIC NATURAL SIGN" },
{ 0x266F, "sharp", "MUSIC SHARP SIGN" },
{ 0x2713, "check", "CHECK MARK" },
{ 0x2717, "cross", "BALLOT X" },
{ 0x2720, "malt", "MALTESE CROSS" },
{ 0x2726, "lozf", "" },
{ 0x2736, "sext", "SIX POINTED BLACK STAR" },
{ 0x3008, "lang", "" },
{ 0x3009, "rang", "" },
{ 0xE291, "rpargt", "" },
{ 0xE2A2, "lnap", "" },
{ 0xE2AA, "nsmid", "" },
{ 0xE2B3, "prnE", "" },
{ 0xE2B5, "scnE", "" },
{ 0xE2B8, "vsubnE", "" },
{ 0xE301, "smid", "" },
{ 0xE411, "gnap", "" },
{ 0xFB00, "fflig", "" },
{ 0xFB01, "filig", "" },
{ 0xFB02, "fllig", "" },
{ 0xFB03, "ffilig", "" },
{ 0xFB04, "ffllig", "" },
{ 0xFE68, "sbsol", "SMALL REVERSE SOLIDUS" },
};

/************************************************************************
 *                                                                     *
 *             Commodity functions to handle entities                  *
 *                                                                     *
 ************************************************************************/

/*
 * Macro used to grow the current buffer.
 */
#define growBuffer(buffer) {                                           \
    buffer##_size *= 2;                                                \
    buffer = (xmlChar *) xmlRealloc(buffer, buffer##_size * sizeof(xmlChar)); \
    if (buffer == NULL) {                                              \
       xmlGenericError(xmlGenericErrorContext, "realloc failed");      \
       return(NULL);                                                   \
    }                                                                  \
}

/**
 * docbEntityLookup:
 * @name: the entity name
 *
 * Lookup the given entity in EntitiesTable
 *
 * TODO: the linear scan is really ugly, an hash table is really needed.
 *
 * Returns the associated docbEntityDescPtr if found, NULL otherwise.
 */
static docbEntityDescPtr
docbEntityLookup(const xmlChar *name) {
    unsigned int i;

    for (i = 0;i < (sizeof(docbookEntitiesTable)/
                    sizeof(docbookEntitiesTable[0]));i++) {
        if (xmlStrEqual(name, BAD_CAST docbookEntitiesTable[i].name)) {
#ifdef DEBUG
            xmlGenericError(xmlGenericErrorContext,"Found entity %s\n", name);
#endif
            return(&docbookEntitiesTable[i]);
       }
    }
    return(NULL);
}

/**
 * docbEntityValueLookup:
 * @value: the entity's unicode value
 *
 * Lookup the given entity in EntitiesTable
 *
 * TODO: the linear scan is really ugly, an hash table is really needed.
 *
 * Returns the associated docbEntityDescPtr if found, NULL otherwise.
 */
static docbEntityDescPtr
docbEntityValueLookup(int value) {
    unsigned int i;
#ifdef DEBUG
    int lv = 0;
#endif

    for (i = 0;i < (sizeof(docbookEntitiesTable)/
                    sizeof(docbookEntitiesTable[0]));i++) {
        if (docbookEntitiesTable[i].value >= value) {
           if (docbookEntitiesTable[i].value > value)
               break;
#ifdef DEBUG
           xmlGenericError(xmlGenericErrorContext,"Found entity %s\n", docbookEntitiesTable[i].name);
#endif
            return(&docbookEntitiesTable[i]);
       }
#ifdef DEBUG
       if (lv > docbookEntitiesTable[i].value) {
           xmlGenericError(xmlGenericErrorContext,
                   "docbookEntitiesTable[] is not sorted (%d > %d)!\n",
                   lv, docbookEntitiesTable[i].value);
       }
       lv = docbookEntitiesTable[i].value;
#endif
    }
    return(NULL);
}

#if 0
/**
 * UTF8ToSgml:
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @in:  a pointer to an array of UTF-8 chars
 * @inlen:  the length of @in
 *
 * Take a block of UTF-8 chars in and try to convert it to an ASCII
 * plus SGML entities block of chars out.
 *
 * Returns 0 if success, -2 if the transcoding fails, or -1 otherwise
 * The value of @inlen after return is the number of octets consumed
 *     as the return value is positive, else unpredictable.
 * The value of @outlen after return is the number of octets consumed.
 */
int
UTF8ToSgml(unsigned char* out, int *outlen,
           const unsigned char* in, int *inlen) {
    const unsigned char* processed = in;
    const unsigned char* outend;
    const unsigned char* outstart = out;
    const unsigned char* instart = in;
    const unsigned char* inend;
    unsigned int c, d;
    int trailing;

    if (in == NULL) {
        /*
        * initialization nothing to do
        */
       *outlen = 0;
       *inlen = 0;
       return(0);
    }
    inend = in + (*inlen);
    outend = out + (*outlen);
    while (in < inend) {
       d = *in++;
       if      (d < 0x80)  { c= d; trailing= 0; }
       else if (d < 0xC0) {
           /* trailing byte in leading position */
           *outlen = out - outstart;
           *inlen = processed - instart;
           return(-2);
        } else if (d < 0xE0)  { c= d & 0x1F; trailing= 1; }
        else if (d < 0xF0)  { c= d & 0x0F; trailing= 2; }
        else if (d < 0xF8)  { c= d & 0x07; trailing= 3; }
       else {
           /* no chance for this in Ascii */
           *outlen = out - outstart;
           *inlen = processed - instart;
           return(-2);
       }

       if (inend - in < trailing) {
           break;
       } 

       for ( ; trailing; trailing--) {
           if ((in >= inend) || (((d= *in++) & 0xC0) != 0x80))
               break;
           c <<= 6;
           c |= d & 0x3F;
       }

       /* assertion: c is a single UTF-4 value */
       if (c < 0x80) {
           if (out + 1 >= outend)
               break;
           *out++ = c;
       } else {
           int len;
           docbEntityDescPtr ent;

           /*
            * Try to lookup a predefined SGML entity for it
            */

           ent = docbEntityValueLookup(c);
           if (ent == NULL) {
               /* no chance for this in Ascii */
               *outlen = out - outstart;
               *inlen = processed - instart;
               return(-2);
           }
           len = strlen(ent->name);
           if (out + 2 + len >= outend)
               break;
           *out++ = '&';
           memcpy(out, ent->name, len);
           out += len;
           *out++ = ';';
       }
       processed = in;
    }
    *outlen = out - outstart;
    *inlen = processed - instart;
    return(0);
}
#endif

/**
 * docbEncodeEntities:
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @in:  a pointer to an array of UTF-8 chars
 * @inlen:  the length of @in
 * @quoteChar: the quote character to escape (' or ") or zero.
 *
 * Take a block of UTF-8 chars in and try to convert it to an ASCII
 * plus SGML entities block of chars out.
 *
 * Returns 0 if success, -2 if the transcoding fails, or -1 otherwise
 * The value of @inlen after return is the number of octets consumed
 *     as the return value is positive, else unpredictable.
 * The value of @outlen after return is the number of octets consumed.
 */
int
docbEncodeEntities(unsigned char* out, int *outlen,
                  const unsigned char* in, int *inlen, int quoteChar) {
    const unsigned char* processed = in;
    const unsigned char* outend = out + (*outlen);
    const unsigned char* outstart = out;
    const unsigned char* instart = in;
    const unsigned char* inend = in + (*inlen);
    unsigned int c, d;
    int trailing;

    while (in < inend) {
       d = *in++;
       if      (d < 0x80)  { c= d; trailing= 0; }
       else if (d < 0xC0) {
           /* trailing byte in leading position */
           *outlen = out - outstart;
           *inlen = processed - instart;
           return(-2);
        } else if (d < 0xE0)  { c= d & 0x1F; trailing= 1; }
        else if (d < 0xF0)  { c= d & 0x0F; trailing= 2; }
        else if (d < 0xF8)  { c= d & 0x07; trailing= 3; }
       else {
           /* no chance for this in Ascii */
           *outlen = out - outstart;
           *inlen = processed - instart;
           return(-2);
       }

       if (inend - in < trailing)
           break;

       while (trailing--) {
           if (((d= *in++) & 0xC0) != 0x80) {
               *outlen = out - outstart;
               *inlen = processed - instart;
               return(-2);
           }
           c <<= 6;
           c |= d & 0x3F;
       }

       /* assertion: c is a single UTF-4 value */
       if (c < 0x80 && c != (unsigned int) quoteChar && c != '&' && c != '<' && c != '>') {
           if (out >= outend)
               break;
           *out++ = c;
       } else {
           docbEntityDescPtr ent;
           const char *cp;
           char nbuf[16];
           int len;

           /*
            * Try to lookup a predefined SGML entity for it
            */
           ent = docbEntityValueLookup(c);
           if (ent == NULL) {
               snprintf(nbuf, sizeof(nbuf), "#%u", c);
               cp = nbuf;
           }
           else
               cp = ent->name;
           len = strlen(cp);
           if (out + 2 + len > outend)
               break;
           *out++ = '&';
           memcpy(out, cp, len);
           out += len;
           *out++ = ';';
       }
       processed = in;
    }
    *outlen = out - outstart;
    *inlen = processed - instart;
    return(0);
}


/************************************************************************
 *                                                                     *
 *             Commodity functions to handle streams                   *
 *                                                                     *
 ************************************************************************/

/**
 * docbNewInputStream:
 * @ctxt:  an SGML parser context
 *
 * Create a new input stream structure
 * Returns the new input stream or NULL
 */
static docbParserInputPtr
docbNewInputStream(docbParserCtxtPtr ctxt) {
    docbParserInputPtr input;

    input = (xmlParserInputPtr) xmlMalloc(sizeof(docbParserInput));
    if (input == NULL) {
        ctxt->errNo = XML_ERR_NO_MEMORY;
       if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
           ctxt->sax->error(ctxt->userData, 
                            "malloc: couldn't allocate a new input stream\n");
       return(NULL);
    }
    memset(input, 0, sizeof(docbParserInput));
    input->filename = NULL;
    input->directory = NULL;
    input->base = NULL;
    input->cur = NULL;
    input->buf = NULL;
    input->line = 1;
    input->col = 1;
    input->buf = NULL;
    input->free = NULL;
    input->version = NULL;
    input->consumed = 0;
    input->length = 0;
    return(input);
}


/************************************************************************
 *                                                                     *
 *             Commodity functions, cleanup needed ?                   *
 *                                                                     *
 ************************************************************************/

/**
 * areBlanks:
 * @ctxt:  an SGML parser context
 * @str:  a xmlChar *
 * @len:  the size of @str
 *
 * Is this a sequence of blank chars that one can ignore ?
 *
 * Returns 1 if ignorable 0 otherwise.
 */

static int areBlanks(docbParserCtxtPtr ctxt, const xmlChar *str, int len) {
    int i;
    xmlNodePtr lastChild;

    for (i = 0;i < len;i++)
        if (!(IS_BLANK(str[i]))) return(0);

    if (CUR == 0) return(1);
    if (CUR != '<') return(0);
    if (ctxt->name == NULL)
       return(1);
    if (ctxt->node == NULL) return(0);
    lastChild = xmlGetLastChild(ctxt->node);
    if (lastChild == NULL) {
        if ((ctxt->node->type != XML_ELEMENT_NODE) &&
	    (ctxt->node->content != NULL)) return(0);
    } else if (xmlNodeIsText(lastChild))
        return(0);
    return(1);
}

/************************************************************************
 *									*
 *                     External entities support			*
 *									*
 ************************************************************************/

/**
 * docbParseCtxtExternalEntity:
 * @ctx:  the existing parsing context
 * @URL:  the URL for the entity to load
 * @ID:  the System ID for the entity to load
 * @list:  the return value for the set of parsed nodes
 *
 * Parse an external general entity within an existing parsing context
 *
 * Returns 0 if the entity is well formed, -1 in case of args problem and
 *    the parser error code otherwise
 */

static int
docbParseCtxtExternalEntity(xmlParserCtxtPtr ctx, const xmlChar *URL,
	                    const xmlChar *ID, xmlNodePtr *list) {
    xmlParserCtxtPtr ctxt;
    xmlDocPtr newDoc;
    xmlSAXHandlerPtr oldsax = NULL;
    int ret = 0;

    if (ctx->depth > 40) {
	return(XML_ERR_ENTITY_LOOP);
    }

    if (list != NULL)
        *list = NULL;
    if ((URL == NULL) && (ID == NULL))
	return(-1);
    if (ctx->myDoc == NULL) /* @@ relax but check for dereferences */
	return(-1);


    ctxt = xmlCreateEntityParserCtxt(URL, ID, ctx->myDoc->URL);
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
     * Parse a possible text declaration first
     */
    GROW;
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
	/* Allocate the Node stack */
	ctxt->vctxt.nodeTab = (xmlNodePtr *) xmlMalloc(4 * sizeof(xmlNodePtr));
	if (ctxt->vctxt.nodeTab == NULL) {
	    xmlGenericError(xmlGenericErrorContext,
		    "docbParseCtxtExternalEntity: out of memory\n");
	    ctxt->validate = 0;
	    ctxt->vctxt.error = NULL;
	    ctxt->vctxt.warning = NULL;
	} else {
	    ctxt->vctxt.nodeNr = 0;
	    ctxt->vctxt.nodeMax = 4;
	    ctxt->vctxt.node = NULL;
	}
    } else {
	ctxt->vctxt.error = NULL;
	ctxt->vctxt.warning = NULL;
    }

    docbParseContent(ctxt);
   
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
    ctxt->sax = oldsax;
    xmlFreeParserCtxt(ctxt);
    newDoc->intSubset = NULL;
    newDoc->extSubset = NULL;
    xmlFreeDoc(newDoc);
    
    return(ret);
}

/************************************************************************
 *									*
 *			The parser itself				*
 *									*
 ************************************************************************/

/**
 * docbParseSGMLName:
 * @ctxt:  an SGML parser context
 *
 * parse an SGML tag or attribute name, note that we convert it to lowercase
 * since SGML names are not case-sensitive.
 *
 * Returns the Tag Name parsed or NULL
 */

static xmlChar *
docbParseSGMLName(docbParserCtxtPtr ctxt) {
    xmlChar *ret = NULL;
    int i = 0;
    xmlChar loc[DOCB_PARSER_BUFFER_SIZE];

    if (!IS_LETTER(CUR) && (CUR != '_') &&
        (CUR != ':')) return(NULL);

    while ((i < DOCB_PARSER_BUFFER_SIZE) &&
           ((IS_LETTER(CUR)) || (IS_DIGIT(CUR)) ||
          (CUR == ':') || (CUR == '_'))) {
       if ((CUR >= 'A') && (CUR <= 'Z')) loc[i] = CUR + 0x20;
        else loc[i] = CUR;
       i++;
       
       NEXT;
    }
    
    ret = xmlStrndup(loc, i);

    return(ret);
}

/**
 * docbParseName:
 * @ctxt:  an SGML parser context
 *
 * parse an SGML name, this routine is case sensitive.
 *
 * Returns the Name parsed or NULL
 */

static xmlChar *
docbParseName(docbParserCtxtPtr ctxt) {
    xmlChar buf[DOCB_MAX_NAMELEN];
    int len = 0;

    GROW;
    if (!IS_LETTER(CUR) && (CUR != '_')) {
       return(NULL);
    }

    while ((IS_LETTER(CUR)) || (IS_DIGIT(CUR)) ||
           (CUR == '.') || (CUR == '-') ||
          (CUR == '_') || (CUR == ':') || 
          (IS_COMBINING(CUR)) ||
          (IS_EXTENDER(CUR))) {
       buf[len++] = CUR;
       NEXT;
       if (len >= DOCB_MAX_NAMELEN) {
           xmlGenericError(xmlGenericErrorContext, 
              "docbParseName: reached DOCB_MAX_NAMELEN limit\n");
           while ((IS_LETTER(CUR)) || (IS_DIGIT(CUR)) ||
                  (CUR == '.') || (CUR == '-') ||
                  (CUR == '_') || (CUR == ':') || 
                  (IS_COMBINING(CUR)) ||
                  (IS_EXTENDER(CUR)))
                NEXT;
           break;
       }
    }
    return(xmlStrndup(buf, len));
}

/**
 * docbParseSGMLAttribute:
 * @ctxt:  an SGML parser context
 * @stop:  a char stop value
 * 
 * parse an SGML attribute value till the stop (quote), if
 * stop is 0 then it stops at the first space
 *
 * Returns the attribute parsed or NULL
 */

static xmlChar *
docbParseSGMLAttribute(docbParserCtxtPtr ctxt, const xmlChar stop) {
    xmlChar *buffer = NULL;
    int buffer_size = 0;
    xmlChar *out = NULL;
    xmlChar *name = NULL;

    xmlChar *cur = NULL;
    xmlEntityPtr xent;
    docbEntityDescPtr ent;

    /*
     * allocate a translation buffer.
     */
    buffer_size = DOCB_PARSER_BIG_BUFFER_SIZE;
    buffer = (xmlChar *) xmlMalloc(buffer_size * sizeof(xmlChar));
    if (buffer == NULL) {
       xmlGenericError(xmlGenericErrorContext,
	               "docbParseSGMLAttribute: malloc failed");
       return(NULL);
    }
    out = buffer;

    /*
     * Ok loop until we reach one of the ending chars
     */
    while ((CUR != 0) && (CUR != stop) && (CUR != '>')) {
       if ((stop == 0) && (IS_BLANK(CUR))) break;
        if (CUR == '&') {
           if (NXT(1) == '#') {
               unsigned int c;
               int bits;

               c = docbParseCharRef(ctxt);
               if      (c <    0x80)
                       { *out++  = c;                bits= -6; }
               else if (c <   0x800)
                       { *out++  =((c >>  6) & 0x1F) | 0xC0;  bits=  0; }
               else if (c < 0x10000)
                       { *out++  =((c >> 12) & 0x0F) | 0xE0;  bits=  6; }
               else                 
                       { *out++  =((c >> 18) & 0x07) | 0xF0;  bits= 12; }
        
               for ( ; bits >= 0; bits-= 6) {
                   *out++  = ((c >> bits) & 0x3F) | 0x80;
               }
           } else {
               xent = docbParseEntityRef(ctxt, &name);
               if (name == NULL) {
                   *out++ = '&';
                   if (out - buffer > buffer_size - 100) {
                       int indx = out - buffer;

                       growBuffer(buffer);
                       out = &buffer[indx];
                   }
                   *out++ = '&';
               } else {
		   ent = docbEntityLookup(name);
		   if (ent == NULL) {
		       *out++ = '&';
		       cur = name;
		       while (*cur != 0) {
			   if (out - buffer > buffer_size - 100) {
			       int indx = out - buffer;

			       growBuffer(buffer);
			       out = &buffer[indx];
			   }
			   *out++ = *cur++;
		       }
		       xmlFree(name);
		   } else {
		       unsigned int c;
		       int bits;

		       if (out - buffer > buffer_size - 100) {
			   int indx = out - buffer;

			   growBuffer(buffer);
			   out = &buffer[indx];
		       }
		       c = (xmlChar)ent->value;
		       if      (c <    0x80)
			   { *out++  = c;                bits= -6; }
		       else if (c <   0x800)
			   { *out++  =((c >>  6) & 0x1F) | 0xC0;  bits=  0; }
		       else if (c < 0x10000)
			   { *out++  =((c >> 12) & 0x0F) | 0xE0;  bits=  6; }
		       else                 
			   { *out++  =((c >> 18) & 0x07) | 0xF0;  bits= 12; }
		
		       for ( ; bits >= 0; bits-= 6) {
			   *out++  = ((c >> bits) & 0x3F) | 0x80;
		       }
		       xmlFree(name);
		   }
	       }
           }
       } else {
           unsigned int c;
           int bits;

           if (out - buffer > buffer_size - 100) {
               int indx = out - buffer;

               growBuffer(buffer);
               out = &buffer[indx];
           }
           c = CUR;
           if      (c <    0x80)
                   { *out++  = c;                bits= -6; }
           else if (c <   0x800)
                   { *out++  =((c >>  6) & 0x1F) | 0xC0;  bits=  0; }
           else if (c < 0x10000)
                   { *out++  =((c >> 12) & 0x0F) | 0xE0;  bits=  6; }
           else                 
                   { *out++  =((c >> 18) & 0x07) | 0xF0;  bits= 12; }
     
           for ( ; bits >= 0; bits-= 6) {
               *out++  = ((c >> bits) & 0x3F) | 0x80;
           }
           NEXT;
       }
    }
    *out++ = 0;
    return(buffer);
}


/**
 * docbParseEntityRef:
 * @ctxt:  an SGML parser context
 * @str:  location to store the entity name
 *
 * parse an SGML ENTITY references
 *
 * [68] EntityRef ::= '&' Name ';'
 *
 * Returns the associated xmlEntityPtr if found, or NULL otherwise,
 *         if non-NULL *str will have to be freed by the caller.
 */
static xmlEntityPtr
docbParseEntityRef(docbParserCtxtPtr ctxt, xmlChar **str) {
    xmlChar *name;
    xmlEntityPtr ent = NULL;
    *str = NULL;

    if (CUR == '&') {
        NEXT;
        name = docbParseName(ctxt);
        if (name == NULL) {
            if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
                ctxt->sax->error(ctxt->userData,
			   "docbParseEntityRef: no name\n");
            ctxt->wellFormed = 0;
        } else {
           GROW;
            if (CUR == ';') {
                *str = name;

		/*
		 * Ask first SAX for entity resolution, otherwise try the
		 * predefined set.
		 */
		if (ctxt->sax != NULL) {
		    if (ctxt->sax->getEntity != NULL)
			ent = ctxt->sax->getEntity(ctxt->userData, name);
		    if (ent == NULL)
		        ent = xmlGetPredefinedEntity(name);
		}
	        NEXT;
            } else {
                if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
                    ctxt->sax->error(ctxt->userData,
                                    "docbParseEntityRef: expecting ';'\n");
                *str = name;
            }
        }
    }
    return(ent);
}

/**
 * docbParseAttValue:
 * @ctxt:  an SGML parser context
 *
 * parse a value for an attribute
 * Note: the parser won't do substitution of entities here, this
 * will be handled later in xmlStringGetNodeList, unless it was
 * asked for ctxt->replaceEntities != 0 
 *
 * Returns the AttValue parsed or NULL.
 */

static xmlChar *
docbParseAttValue(docbParserCtxtPtr ctxt) {
    xmlChar *ret = NULL;

    if (CUR == '"') {
        NEXT;
       ret = docbParseSGMLAttribute(ctxt, '"');
        if (CUR != '"') {
           if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
               ctxt->sax->error(ctxt->userData, "AttValue: ' expected\n");
           ctxt->wellFormed = 0;
       } else
           NEXT;
    } else if (CUR == '\'') {
        NEXT;
       ret = docbParseSGMLAttribute(ctxt, '\'');
        if (CUR != '\'') {
           if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
               ctxt->sax->error(ctxt->userData, "AttValue: ' expected\n");
           ctxt->wellFormed = 0;
       } else
           NEXT;
    } else {
        /*
        * That's an SGMLism, the attribute value may not be quoted
        */
       ret = docbParseSGMLAttribute(ctxt, 0);
       if (ret == NULL) {
           if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
               ctxt->sax->error(ctxt->userData, "AttValue: no value found\n");
           ctxt->wellFormed = 0;
       }
    }
    return(ret);
}

/**
 * docbParseSystemLiteral:
 * @ctxt:  an SGML parser context
 * 
 * parse an SGML Literal
 *
 * [11] SystemLiteral ::= ('"' [^"]* '"') | ("'" [^']* "'")
 *
 * Returns the SystemLiteral parsed or NULL
 */

static xmlChar *
docbParseSystemLiteral(docbParserCtxtPtr ctxt) {
    const xmlChar *q;
    xmlChar *ret = NULL;

    if (CUR == '"') {
        NEXT;
       q = CUR_PTR;
       while ((IS_CHAR(CUR)) && (CUR != '"'))
           NEXT;
       if (!IS_CHAR(CUR)) {
           if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
               ctxt->sax->error(ctxt->userData, "Unfinished SystemLiteral\n");
           ctxt->wellFormed = 0;
       } else {
           ret = xmlStrndup(q, CUR_PTR - q);
           NEXT;
        }
    } else if (CUR == '\'') {
        NEXT;
       q = CUR_PTR;
       while ((IS_CHAR(CUR)) && (CUR != '\''))
           NEXT;
       if (!IS_CHAR(CUR)) {
           if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
               ctxt->sax->error(ctxt->userData, "Unfinished SystemLiteral\n");
           ctxt->wellFormed = 0;
       } else {
           ret = xmlStrndup(q, CUR_PTR - q);
           NEXT;
        }
    } else {
       if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
           ctxt->sax->error(ctxt->userData,
                            "SystemLiteral \" or ' expected\n");
       ctxt->wellFormed = 0;
    }
    
    return(ret);
}

/**
 * docbParsePubidLiteral:
 * @ctxt:  an SGML parser context
 *
 * parse an SGML public literal
 *
 * [12] PubidLiteral ::= '"' PubidChar* '"' | "'" (PubidChar - "'")* "'"
 *
 * Returns the PubidLiteral parsed or NULL.
 */

static xmlChar *
docbParsePubidLiteral(docbParserCtxtPtr ctxt) {
    const xmlChar *q;
    xmlChar *ret = NULL;
    /*
     * Name ::= (Letter | '_') (NameChar)*
     */
    if (CUR == '"') {
        NEXT;
       q = CUR_PTR;
       while (IS_PUBIDCHAR(CUR)) NEXT;
       if (CUR != '"') {
           if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
               ctxt->sax->error(ctxt->userData, "Unfinished PubidLiteral\n");
           ctxt->wellFormed = 0;
       } else {
           ret = xmlStrndup(q, CUR_PTR - q);
           NEXT;
       }
    } else if (CUR == '\'') {
        NEXT;
       q = CUR_PTR;
       while ((IS_LETTER(CUR)) && (CUR != '\''))
           NEXT;
       if (!IS_LETTER(CUR)) {
           if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
               ctxt->sax->error(ctxt->userData, "Unfinished PubidLiteral\n");
           ctxt->wellFormed = 0;
       } else {
           ret = xmlStrndup(q, CUR_PTR - q);
           NEXT;
       }
    } else {
       if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
           ctxt->sax->error(ctxt->userData, "SystemLiteral \" or ' expected\n");
       ctxt->wellFormed = 0;
    }
    
    return(ret);
}

/**
 * docbParseCharData:
 * @ctxt:  an SGML parser context
 * @cdata:  int indicating whether we are within a CDATA section
 *
 * parse a CharData section.
 * if we are within a CDATA section ']]>' marks an end of section.
 *
 * [14] CharData ::= [^<&]* - ([^<&]* ']]>' [^<&]*)
 */

static void
docbParseCharData(docbParserCtxtPtr ctxt) {
    xmlChar buf[DOCB_PARSER_BIG_BUFFER_SIZE + 5];
    int nbchar = 0;
    int cur, l;

    SHRINK;
    cur = CUR_CHAR(l);
    while (((cur != '<') || (ctxt->token == '<')) &&
           ((cur != '&') || (ctxt->token == '&')) && 
          (IS_CHAR(cur))) {
       COPY_BUF(l,buf,nbchar,cur);
       if (nbchar >= DOCB_PARSER_BIG_BUFFER_SIZE) {
           /*
            * Ok the segment is to be consumed as chars.
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
       NEXTL(l);
       cur = CUR_CHAR(l);
    }
    if (nbchar != 0) {
       /*
        * Ok the segment is to be consumed as chars.
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
 * docbParseExternalID:
 * @ctxt:  an SGML parser context
 * @publicID:  a xmlChar** receiving PubidLiteral
 *
 * Parse an External ID or a Public ID
 *
 * Returns the function returns SystemLiteral and in the second
 *                case publicID receives PubidLiteral,
 *                it is possible to return NULL and have publicID set.
 */

static xmlChar *
docbParseExternalID(docbParserCtxtPtr ctxt, xmlChar **publicID) {
    xmlChar *URI = NULL;

    if ((UPPER == 'S') && (UPP(1) == 'Y') &&
         (UPP(2) == 'S') && (UPP(3) == 'T') &&
        (UPP(4) == 'E') && (UPP(5) == 'M')) {
        SKIP(6);
       if (!IS_BLANK(CUR)) {
           if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
               ctxt->sax->error(ctxt->userData,
                   "Space required after 'SYSTEM'\n");
           ctxt->wellFormed = 0;
       }
        SKIP_BLANKS;
       URI = docbParseSystemLiteral(ctxt);
       if (URI == NULL) {
           if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
               ctxt->sax->error(ctxt->userData,
                 "docbParseExternalID: SYSTEM, no URI\n");
           ctxt->wellFormed = 0;
        }
    } else if ((UPPER == 'P') && (UPP(1) == 'U') &&
              (UPP(2) == 'B') && (UPP(3) == 'L') &&
              (UPP(4) == 'I') && (UPP(5) == 'C')) {
        SKIP(6);
       if (!IS_BLANK(CUR)) {
           if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
               ctxt->sax->error(ctxt->userData,
                   "Space required after 'PUBLIC'\n");
           ctxt->wellFormed = 0;
       }
        SKIP_BLANKS;
       *publicID = docbParsePubidLiteral(ctxt);
       if (*publicID == NULL) {
           if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
               ctxt->sax->error(ctxt->userData, 
                 "docbParseExternalID: PUBLIC, no Public Identifier\n");
           ctxt->wellFormed = 0;
       }
        SKIP_BLANKS;
        if ((CUR == '"') || (CUR == '\'')) {
           URI = docbParseSystemLiteral(ctxt);
       }
    }
    return(URI);
}

/**
 * docbParsePI:
 * @ctxt:  an XML parser context
 * 
 * parse an XML Processing Instruction.
 *
 * [16] PI ::= '<?' PITarget (S (Char* - (Char* '?>' Char*)))? '?>'
 *
 * The processing is transfered to SAX once parsed.
 */

static void
docbParsePI(xmlParserCtxtPtr ctxt) {
    xmlChar *buf = NULL;
    int len = 0;
    int size = DOCB_PARSER_BUFFER_SIZE;
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
	target = xmlParseName(ctxt);
	if (target != NULL) {
	    xmlChar *encoding = NULL;

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
	    if (xmlStrEqual(target, BAD_CAST "sgml-declaration")) {

		encoding = xmlParseEncodingDecl(ctxt);
		if (encoding == NULL) {
		    xmlGenericError(xmlGenericErrorContext,
			"sgml-declaration: failed to find/handle encoding\n");
#ifdef DEBUG
		} else {
		    xmlGenericError(xmlGenericErrorContext,
			    "switched to encoding %s\n", encoding);
#endif
		}

	    }
	    buf = (xmlChar *) xmlMalloc(size * sizeof(xmlChar));
	    if (buf == NULL) {
		xmlGenericError(xmlGenericErrorContext,
			"malloc of %d byte failed\n", size);
		ctxt->instate = state;
		return;
	    }
	    cur = CUR;
	    if (encoding != NULL) {
		len = snprintf((char *) buf, size - 1,
			       " encoding = \"%s\"", encoding);
		if (len < 0) 
		    len = size;
	    } else {
		if (!IS_BLANK(cur)) {
		    ctxt->errNo = XML_ERR_SPACE_REQUIRED;
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData,
			  "docbParsePI: PI %s space expected\n", target);
		    ctxt->wellFormed = 0;
		    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		}
		SKIP_BLANKS;
	    }
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
		      "docbParsePI: PI %s never end ...\n", target);
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
		       "docbParsePI : no target name\n");
	    ctxt->wellFormed = 0;
	    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
	}
	ctxt->instate = state;
    }
}

/**
 * docbParseComment:
 * @ctxt:  an SGML parser context
 *
 * Parse an XML (SGML) comment <!-- .... -->
 *
 * [15] Comment ::= '<!--' ((Char - '-') | ('-' (Char - '-')))* '-->'
 */
static void
docbParseComment(docbParserCtxtPtr ctxt) {
    xmlChar *buf = NULL;
    int len;
    int size = DOCB_PARSER_BUFFER_SIZE;
    int q, ql;
    int r, rl;
    int cur, l;
    xmlParserInputState state;

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
    while (IS_CHAR(cur) &&
           ((cur != '>') ||
           (r != '-') || (q != '-'))) {
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
       xmlFree(buf);
    } else {
        NEXT;
       if ((ctxt->sax != NULL) && (ctxt->sax->comment != NULL) &&
           (!ctxt->disableSAX))
           ctxt->sax->comment(ctxt->userData, buf);
       xmlFree(buf);
    }
    ctxt->instate = state;
}

/**
 * docbParseCharRef:
 * @ctxt:  an SGML parser context
 *
 * parse Reference declarations
 *
 * [66] CharRef ::= '&#' [0-9]+ ';' |
 *                  '&#x' [0-9a-fA-F]+ ';'
 *
 * Returns the value parsed (as an int)
 */
static int
docbParseCharRef(docbParserCtxtPtr ctxt) {
    int val = 0;

    if ((CUR == '&') && (NXT(1) == '#') &&
        (NXT(2) == 'x')) {
       SKIP(3);
       while (CUR != ';') {
           if ((CUR >= '0') && (CUR <= '9')) 
               val = val * 16 + (CUR - '0');
           else if ((CUR >= 'a') && (CUR <= 'f'))
               val = val * 16 + (CUR - 'a') + 10;
           else if ((CUR >= 'A') && (CUR <= 'F'))
               val = val * 16 + (CUR - 'A') + 10;
           else {
               if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
                   ctxt->sax->error(ctxt->userData, 
                        "docbParseCharRef: invalid hexadecimal value\n");
               ctxt->wellFormed = 0;
               val = 0;
               break;
           }
           NEXT;
       }
       if (CUR == ';')
           NEXT;
    } else if  ((CUR == '&') && (NXT(1) == '#')) {
       SKIP(2);
       while (CUR != ';') {
           if ((CUR >= '0') && (CUR <= '9')) 
               val = val * 10 + (CUR - '0');
           else {
               if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
                   ctxt->sax->error(ctxt->userData, 
                        "docbParseCharRef: invalid decimal value\n");
               ctxt->wellFormed = 0;
               val = 0;
               break;
           }
           NEXT;
       }
       if (CUR == ';')
           NEXT;
    } else {
       if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
           ctxt->sax->error(ctxt->userData, "docbParseCharRef: invalid value\n");
       ctxt->wellFormed = 0;
    }
    /*
     * Check the value IS_CHAR ...
     */
    if (IS_CHAR(val)) {
        return(val);
    } else {
       if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
           ctxt->sax->error(ctxt->userData, "docbParseCharRef: invalid xmlChar value %d\n",
                            val);
       ctxt->wellFormed = 0;
    }
    return(0);
}


/**
 * docbParseDocTypeDecl:
 * @ctxt:  an SGML parser context
 *
 * parse a DOCTYPE declaration
 *
 * [28] doctypedecl ::= '<!DOCTYPE' S Name (S ExternalID)? S? 
 *                      ('[' (markupdecl | PEReference | S)* ']' S?)? '>'
 */

static void
docbParseDocTypeDecl(docbParserCtxtPtr ctxt) {
    xmlChar *name;
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
    name = docbParseName(ctxt);
    if (name == NULL) {
       if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
           ctxt->sax->error(ctxt->userData, "docbParseDocTypeDecl : no DOCTYPE name !\n");
       ctxt->wellFormed = 0;
    }
    /*
     * Check that upper(name) == "SGML" !!!!!!!!!!!!!
     */

    SKIP_BLANKS;

    /*
     * Check for SystemID and ExternalID
     */
    URI = docbParseExternalID(ctxt, &ExternalID);
    SKIP_BLANKS;

    /*
     * Create or update the document accordingly to the DOCTYPE
     * But use the predefined PUBLIC and SYSTEM ID of DocBook XML
     */
    if ((ctxt->sax != NULL) && (ctxt->sax->internalSubset != NULL) &&
       (!ctxt->disableSAX))
       ctxt->sax->internalSubset(ctxt->userData, name,
	                         XML_DOCBOOK_XML_PUBLIC,
				 XML_DOCBOOK_XML_SYSTEM);

    if (RAW != '>') {
       if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
           ctxt->sax->error(ctxt->userData,
		   "docbParseDocTypeDecl : internal subset not handled\n");
    } else {
	NEXT;
    }

    /*
     * Cleanup, since we don't use all those identifiers
     */
    if (URI != NULL) xmlFree(URI);
    if (ExternalID != NULL) xmlFree(ExternalID);
    if (name != NULL) xmlFree(name);
}

/**
 * docbParseAttribute:
 * @ctxt:  an SGML parser context
 * @value:  a xmlChar ** used to store the value of the attribute
 *
 * parse an attribute
 *
 * [41] Attribute ::= Name Eq AttValue
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

static xmlChar *
docbParseAttribute(docbParserCtxtPtr ctxt, xmlChar **value) {
    xmlChar *name, *val = NULL;

    *value = NULL;
    name = docbParseName(ctxt);
    if (name == NULL) {
       if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
           ctxt->sax->error(ctxt->userData, "error parsing attribute name\n");
       ctxt->wellFormed = 0;
        return(NULL);
    }

    /*
     * read the value
     */
    SKIP_BLANKS;
    if (CUR == '=') {
        NEXT;
       SKIP_BLANKS;
       val = docbParseAttValue(ctxt);
       /******
    } else {
        * TODO : some attribute must have values, some may not
       if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
           ctxt->sax->warning(ctxt->userData,
              "No value for attribute %s\n", name); */
    }

    *value = val;
    return(name);
}

/**
 * docbCheckEncoding:
 * @ctxt:  an SGML parser context
 * @attvalue: the attribute value
 *
 * Checks an http-equiv attribute from a Meta tag to detect
 * the encoding
 * If a new encoding is detected the parser is switched to decode
 * it and pass UTF8
 */
static void
docbCheckEncoding(docbParserCtxtPtr ctxt, const xmlChar *attvalue) {
    const xmlChar *encoding;

    if ((ctxt == NULL) || (attvalue == NULL))
       return;

    encoding = xmlStrstr(attvalue, BAD_CAST"charset=");
    if (encoding == NULL) 
       encoding = xmlStrstr(attvalue, BAD_CAST"Charset=");
    if (encoding == NULL) 
       encoding = xmlStrstr(attvalue, BAD_CAST"CHARSET=");
    if (encoding != NULL) {
       encoding += 8;
    } else {
       encoding = xmlStrstr(attvalue, BAD_CAST"charset =");
       if (encoding == NULL) 
           encoding = xmlStrstr(attvalue, BAD_CAST"Charset =");
       if (encoding == NULL) 
           encoding = xmlStrstr(attvalue, BAD_CAST"CHARSET =");
       if (encoding != NULL)
           encoding += 9;
    }
    /*
     * Restricted from 2.3.5 */
    if (encoding != NULL) {
       xmlCharEncoding enc;

       if (ctxt->input->encoding != NULL)
           xmlFree((xmlChar *) ctxt->input->encoding);
       ctxt->input->encoding = encoding;

       enc = xmlParseCharEncoding((const char *) encoding);
       if (enc == XML_CHAR_ENCODING_8859_1) {
           ctxt->charset = XML_CHAR_ENCODING_8859_1; 
       } else if (enc != XML_CHAR_ENCODING_UTF8) {
           if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
               ctxt->sax->error(ctxt->userData,
                    "Unsupported encoding %s\n", encoding);
           /* xmlFree(encoding); */
           ctxt->wellFormed = 0;
           if (ctxt->recovery == 0) ctxt->disableSAX = 1;
           ctxt->errNo = XML_ERR_UNSUPPORTED_ENCODING;
       }
    }
}

/**
 * docbCheckMeta:
 * @ctxt:  an SGML parser context
 * @atts:  the attributes values
 *
 * Checks an attributes from a Meta tag
 */
static void
docbCheckMeta(docbParserCtxtPtr ctxt, const xmlChar **atts) {
    int i;
    const xmlChar *att, *value;
    int http = 0;
    const xmlChar *content = NULL;

    if ((ctxt == NULL) || (atts == NULL))
       return;

    i = 0;
    att = atts[i++];
    while (att != NULL) {
       value = atts[i++];
       if ((value != NULL) &&
           ((xmlStrEqual(att, BAD_CAST"http-equiv")) ||
            (xmlStrEqual(att, BAD_CAST"Http-Equiv")) ||
            (xmlStrEqual(att, BAD_CAST"HTTP-EQUIV"))) &&
           ((xmlStrEqual(value, BAD_CAST"Content-Type")) ||
            (xmlStrEqual(value, BAD_CAST"content-type")) ||
            (xmlStrEqual(value, BAD_CAST"CONTENT-TYPE"))))
           http = 1;
       else if ((value != NULL) &&
                ((xmlStrEqual(att, BAD_CAST"content")) ||
                 (xmlStrEqual(att, BAD_CAST"Content")) ||
                 (xmlStrEqual(att, BAD_CAST"CONTENT"))))
           content = value;
       att = atts[i++];
    }
    if ((http) && (content != NULL))
       docbCheckEncoding(ctxt, content);

}

/**
 * docbParseStartTag:
 * @ctxt:  an SGML parser context
 * 
 * parse a start of tag either for rule element or
 * EmptyElement. In both case we don't parse the tag closing chars.
 *
 * [40] STag ::= '<' Name (S Attribute)* S? '>'
 *
 * [44] EmptyElemTag ::= '<' Name (S Attribute)* S? '/>'
 *
 * With namespace:
 *
 * [NS 8] STag ::= '<' QName (S Attribute)* S? '>'
 *
 * [NS 10] EmptyElement ::= '<' QName (S Attribute)* S? '/>'
 *
 */

static void
docbParseStartTag(docbParserCtxtPtr ctxt) {
    xmlChar *name;
    xmlChar *attname;
    xmlChar *attvalue;
    const xmlChar **atts = NULL;
    int nbatts = 0;
    int maxatts = 0;
    int meta = 0;
    int i;

    if (CUR != '<') return;
    NEXT;

    GROW;
    name = docbParseSGMLName(ctxt);
    if (name == NULL) {
       if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
           ctxt->sax->error(ctxt->userData, 
            "docbParseStartTag: invalid element name\n");
       ctxt->wellFormed = 0;
        return;
    }
    if (xmlStrEqual(name, BAD_CAST"meta"))
       meta = 1;

    /*
     * Check for auto-closure of SGML elements.
     */
    docbAutoClose(ctxt, name);

    /*
     * Now parse the attributes, it ends up with the ending
     *
     * (S Attribute)* S?
     */
    SKIP_BLANKS;
    while ((IS_CHAR(CUR)) &&
           (CUR != '>') && 
          ((CUR != '/') || (NXT(1) != '>'))) {
       long cons = ctxt->nbChars;

       GROW;
       attname = docbParseAttribute(ctxt, &attvalue);
        if (attname != NULL) {

           /*
            * Well formedness requires at most one declaration of an attribute
            */
           for (i = 0; i < nbatts;i += 2) {
               if (xmlStrEqual(atts[i], attname)) {
                   if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
                       ctxt->sax->error(ctxt->userData,
                                        "Attribute %s redefined\n",
                                        attname);
                   ctxt->wellFormed = 0;
                   xmlFree(attname);
                   if (attvalue != NULL)
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
                   if (name != NULL) xmlFree(name);
                   return;
               }
           } else if (nbatts + 4 > maxatts) {
               maxatts *= 2;
               atts = (const xmlChar **) xmlRealloc((void *)atts, maxatts * sizeof(xmlChar *));
               if (atts == NULL) {
                   xmlGenericError(xmlGenericErrorContext,
                           "realloc of %ld byte failed\n",
                           maxatts * (long)sizeof(xmlChar *));
                   if (name != NULL) xmlFree(name);
                   return;
               }
           }
           atts[nbatts++] = attname;
           atts[nbatts++] = attvalue;
           atts[nbatts] = NULL;
           atts[nbatts + 1] = NULL;
       }

failed:
       SKIP_BLANKS;
        if (cons == ctxt->nbChars) {
           if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
               ctxt->sax->error(ctxt->userData, 
                "docbParseStartTag: problem parsing attributes\n");
           ctxt->wellFormed = 0;
           break;
       }
    }

    /*
     * Handle specific association to the META tag
     */
    if (meta)
       docbCheckMeta(ctxt, atts);

    /*
     * SAX: Start of Element !
     */
    docbnamePush(ctxt, xmlStrdup(name));
#ifdef DEBUG
    xmlGenericError(xmlGenericErrorContext,"Start of element %s: pushed %s\n", name, ctxt->name);
#endif    
    if ((ctxt->sax != NULL) && (ctxt->sax->startElement != NULL))
        ctxt->sax->startElement(ctxt->userData, name, atts);

    if (atts != NULL) {
        for (i = 0;i < nbatts;i++) {
           if (atts[i] != NULL)
               xmlFree((xmlChar *) atts[i]);
       }
       xmlFree((void *) atts);
    }
    if (name != NULL) xmlFree(name);
}

/**
 * docbParseEndTag:
 * @ctxt:  an SGML parser context
 *
 * parse an end of tag
 *
 * [42] ETag ::= '</' Name S? '>'
 *
 * With namespace
 *
 * [NS 9] ETag ::= '</' QName S? '>'
 */

static void
docbParseEndTag(docbParserCtxtPtr ctxt) {
    xmlChar *name;
    xmlChar *oldname;
    int i;

    if ((CUR != '<') || (NXT(1) != '/')) {
       if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
           ctxt->sax->error(ctxt->userData, "docbParseEndTag: '</' not found\n");
       ctxt->wellFormed = 0;
       return;
    }
    SKIP(2);

    name = docbParseSGMLName(ctxt);
    if (name == NULL) {
       if (CUR == '>') {
           NEXT;
           oldname = docbnamePop(ctxt);
           if (oldname != NULL) {
               if ((ctxt->sax != NULL) && (ctxt->sax->endElement != NULL))
                   ctxt->sax->endElement(ctxt->userData, name);
#ifdef DEBUG
               xmlGenericError(xmlGenericErrorContext,"End of tag </>: popping out %s\n", oldname);
#endif
               xmlFree(oldname);
#ifdef DEBUG
           } else {
               xmlGenericError(xmlGenericErrorContext,"End of tag </>: stack empty !!!\n");
#endif
           }
           return;
       } else
           return;
    }

    /*
     * We should definitely be at the ending "S? '>'" part
     */
    SKIP_BLANKS;
    if ((!IS_CHAR(CUR)) || (CUR != '>')) {
       if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
           ctxt->sax->error(ctxt->userData, "End tag : expected '>'\n");
       ctxt->wellFormed = 0;
    } else
       NEXT;

    /*
     * If the name read is not one of the element in the parsing stack
     * then return, it's just an error.
     */
    for (i = (ctxt->nameNr - 1);i >= 0;i--) {
        if (xmlStrEqual(name, ctxt->nameTab[i])) break;
    }
    if (i < 0) {
       if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
           ctxt->sax->error(ctxt->userData,
            "Unexpected end tag : %s\n", name);
       xmlFree(name);
       ctxt->wellFormed = 0;
       return;
    }


    /*
     * Check for auto-closure of SGML elements.
     */

    docbAutoCloseOnClose(ctxt, name);

    /*
     * Well formedness constraints, opening and closing must match.
     * With the exception that the autoclose may have popped stuff out
     * of the stack.
     */
    if (((name[0] != '/') || (name[1] != 0)) &&
       (!xmlStrEqual(name, ctxt->name))) {
#ifdef DEBUG
       xmlGenericError(xmlGenericErrorContext,"End of tag %s: expecting %s\n", name, ctxt->name);
#endif
        if ((ctxt->name != NULL) && 
           (!xmlStrEqual(ctxt->name, name))) {
           if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
               ctxt->sax->error(ctxt->userData,
                "Opening and ending tag mismatch: %s and %s\n",
                                name, ctxt->name);
           ctxt->wellFormed = 0;
        }
    }

    /*
     * SAX: End of Tag
     */
    oldname = ctxt->name;
    if (((name[0] == '/') && (name[1] == 0)) ||
       ((oldname != NULL) && (xmlStrEqual(oldname, name)))) {
       if ((ctxt->sax != NULL) && (ctxt->sax->endElement != NULL))
           ctxt->sax->endElement(ctxt->userData, name);
       oldname = docbnamePop(ctxt);
       if (oldname != NULL) {
#ifdef DEBUG
           xmlGenericError(xmlGenericErrorContext,"End of tag %s: popping out %s\n", name, oldname);
#endif
           xmlFree(oldname);
#ifdef DEBUG
       } else {
           xmlGenericError(xmlGenericErrorContext,"End of tag %s: stack empty !!!\n", name);
#endif
       }
    }

    if (name != NULL)
       xmlFree(name);

    return;
}


/**
 * docbParseReference:
 * @ctxt:  an SGML parser context
 * 
 * parse and handle entity references in content,
 * this will end-up in a call to character() since this is either a
 * CharRef, or a predefined entity.
 */
static void
docbParseReference(docbParserCtxtPtr ctxt) {
    docbEntityDescPtr ent;
    xmlEntityPtr xent;
    xmlChar out[6];
    xmlChar *name;
    if (CUR != '&') return;

    if (NXT(1) == '#') {
       unsigned int c;
       int bits, i = 0;

       c = docbParseCharRef(ctxt);
        if      (c <    0x80) { out[i++]= c;                bits= -6; }
        else if (c <   0x800) { out[i++]=((c >>  6) & 0x1F) | 0xC0;  bits=  0; }
        else if (c < 0x10000) { out[i++]=((c >> 12) & 0x0F) | 0xE0;  bits=  6; }
        else                  { out[i++]=((c >> 18) & 0x07) | 0xF0;  bits= 12; }
 
        for ( ; bits >= 0; bits-= 6) {
            out[i++]= ((c >> bits) & 0x3F) | 0x80;
        }
       out[i] = 0;

       if ((ctxt->sax != NULL) && (ctxt->sax->characters != NULL))
           ctxt->sax->characters(ctxt->userData, out, i);
    } else {
	/*
	 * Lookup the entity in the table.
	 */
       xent = docbParseEntityRef(ctxt, &name);
       if (xent != NULL) {
	    if (((ctxt->replaceEntities) || (ctxt->loadsubset)) &&
		((xent->children == NULL) &&
		(xent->etype == XML_EXTERNAL_GENERAL_PARSED_ENTITY))) {
		    /*
		     * we really need to fetch and parse the external entity
		     */
		    int parse;
		    xmlNodePtr children = NULL;

		    parse = docbParseCtxtExternalEntity(ctxt,
			       xent->SystemID, xent->ExternalID, &children);
		    xmlAddChildList((xmlNodePtr) xent, children);
	    }
	    if (ctxt->replaceEntities) {
		if ((ctxt->node != NULL) && (xent->children != NULL)) {
		    /*
		     * Seems we are generating the DOM content, do
		     * a simple tree copy
		     */
		    xmlNodePtr new;
		    new = xmlCopyNodeList(xent->children);
		    
		    xmlAddChildList(ctxt->node, new);
		    /*
		     * This is to avoid a nasty side effect, see
		     * characters() in SAX.c
		     */
		    ctxt->nodemem = 0;
		    ctxt->nodelen = 0;
		}
	    } else {
		if ((ctxt->sax != NULL) && (ctxt->sax->reference != NULL) &&
		    (ctxt->replaceEntities == 0) && (!ctxt->disableSAX)) {
		    /*
		     * Create a node.
		     */
		    ctxt->sax->reference(ctxt->userData, xent->name);
		}
	    }
       } else if (name != NULL) {
	   ent = docbEntityLookup(name);
	   if ((ent == NULL) || (ent->value <= 0)) {
	       if ((ctxt->sax != NULL) && (ctxt->sax->characters != NULL)) {
		   ctxt->sax->characters(ctxt->userData, BAD_CAST "&", 1);
		   ctxt->sax->characters(ctxt->userData, name, xmlStrlen(name));
		   /* ctxt->sax->characters(ctxt->userData, BAD_CAST ";", 1); */
	       }
	   } else {
	       unsigned int c;
	       int bits, i = 0;

	       c = ent->value;
	       if      (c <    0x80)
		       { out[i++]= c;                bits= -6; }
	       else if (c <   0x800)
		       { out[i++]=((c >>  6) & 0x1F) | 0xC0;  bits=  0; }
	       else if (c < 0x10000)
		       { out[i++]=((c >> 12) & 0x0F) | 0xE0;  bits=  6; }
	       else                 
		       { out[i++]=((c >> 18) & 0x07) | 0xF0;  bits= 12; }
	 
	       for ( ; bits >= 0; bits-= 6) {
		   out[i++]= ((c >> bits) & 0x3F) | 0x80;
	       }
	       out[i] = 0;

	       if ((ctxt->sax != NULL) && (ctxt->sax->characters != NULL))
		   ctxt->sax->characters(ctxt->userData, out, i);
	   }
       } else {
           if ((ctxt->sax != NULL) && (ctxt->sax->characters != NULL))
               ctxt->sax->characters(ctxt->userData, BAD_CAST "&", 1);
           return;
       }
       if (name != NULL)
	   xmlFree(name);
    }
}

/**
 * docbParseContent:
 * @ctxt:  an SGML parser context
 * @name:  the node name
 *
 * Parse a content: comment, sub-element, reference or text.
 *
 */
static void
docbParseContent(docbParserCtxtPtr ctxt)
{
    xmlChar *currentNode;
    int depth;

    currentNode = xmlStrdup(ctxt->name);
    depth = ctxt->nameNr;
    while (1) {
        long cons = ctxt->nbChars;

        GROW;
        /*
         * Our tag or one of it's parent or children is ending.
         */
        if ((CUR == '<') && (NXT(1) == '/')) {
            docbParseEndTag(ctxt);
            if (currentNode != NULL)
                xmlFree(currentNode);
            return;
        }

        /*
         * Has this node been popped out during parsing of
         * the next element
         */
        if ((!xmlStrEqual(currentNode, ctxt->name)) &&
            (depth >= ctxt->nameNr)) {
            if (currentNode != NULL)
                xmlFree(currentNode);
            return;
        }

        /*
         * Sometimes DOCTYPE arrives in the middle of the document
         */
        if ((CUR == '<') && (NXT(1) == '!') &&
            (UPP(2) == 'D') && (UPP(3) == 'O') &&
            (UPP(4) == 'C') && (UPP(5) == 'T') &&
            (UPP(6) == 'Y') && (UPP(7) == 'P') && (UPP(8) == 'E')) {
            if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
                ctxt->sax->error(ctxt->userData,
                                 "Misplaced DOCTYPE declaration\n");
            ctxt->wellFormed = 0;
            docbParseDocTypeDecl(ctxt);
        }

        /*
         * First case :  a comment
         */
        if ((CUR == '<') && (NXT(1) == '!') &&
            (NXT(2) == '-') && (NXT(3) == '-')) {
            docbParseComment(ctxt);
        }
	
        /*
         * Second case :  a PI
         */
	else if ((RAW == '<') && (NXT(1) == '?')) {
            docbParsePI(ctxt);
        }

        /*
         * Third case :  a sub-element.
         */
        else if (CUR == '<') {
            docbParseElement(ctxt);
        }

        /*
         * Fourth case : a reference. If if has not been resolved,
         *    parsing returns it's Name, create the node 
         */
        else if (CUR == '&') {
            docbParseReference(ctxt);
        }

        /*
         * Fifth : end of the resource
         */
        else if (CUR == 0) {
            docbAutoClose(ctxt, NULL);
            if (ctxt->nameNr == 0)
                break;
        }

        /*
         * Last case, text. Note that References are handled directly.
         */
        else {
            docbParseCharData(ctxt);
        }

        if (cons == ctxt->nbChars) {
            if (ctxt->node != NULL) {
                if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
                    ctxt->sax->error(ctxt->userData,
                                     "detected an error in element content\n");
                ctxt->wellFormed = 0;
            }
            break;
        }

        GROW;
    }
    if (currentNode != NULL)
        xmlFree(currentNode);
}

/**
 * docbParseElement:
 * @ctxt:  an SGML parser context
 *
 * parse an SGML element, this is highly recursive
 *
 * [39] element ::= EmptyElemTag | STag content ETag
 *
 * [41] Attribute ::= Name Eq AttValue
 */

static void
docbParseElement(docbParserCtxtPtr ctxt) {
    xmlChar *name;
    xmlChar *currentNode = NULL;
    docbElemDescPtr info;
    docbParserNodeInfo node_info;
    xmlChar *oldname;
    int depth = ctxt->nameNr;

    /* Capture start position */
    if (ctxt->record_info) {
        node_info.begin_pos = ctxt->input->consumed +
                          (CUR_PTR - ctxt->input->base);
       node_info.begin_line = ctxt->input->line;
    }

    oldname = xmlStrdup(ctxt->name);
    docbParseStartTag(ctxt);
    name = ctxt->name;
#ifdef DEBUG
    if (oldname == NULL)
       xmlGenericError(xmlGenericErrorContext,
               "Start of element %s\n", name);
    else if (name == NULL)     
       xmlGenericError(xmlGenericErrorContext,
               "Start of element failed, was %s\n", oldname);
    else       
       xmlGenericError(xmlGenericErrorContext,
               "Start of element %s, was %s\n", name, oldname);
#endif
    if (((depth == ctxt->nameNr) && (xmlStrEqual(oldname, ctxt->name))) ||
        (name == NULL)) {
       if (CUR == '>')
           NEXT;
       if (oldname != NULL)
           xmlFree(oldname);
        return;
    }
    if (oldname != NULL)
       xmlFree(oldname);

    /*
     * Lookup the info for that element.
     */
    info = docbTagLookup(name);
    if (info == NULL) {
       if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
           ctxt->sax->error(ctxt->userData, "Tag %s unknown\n",
                            name);
       ctxt->wellFormed = 0;
    } else if (info->depr) {
/***************************
       if ((ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
           ctxt->sax->warning(ctxt->userData, "Tag %s is deprecated\n",
                              name);
 ***************************/
    }

    /*
     * Check for an Empty Element labeled the XML/SGML way
     */
    if ((CUR == '/') && (NXT(1) == '>')) {
        SKIP(2);
       if ((ctxt->sax != NULL) && (ctxt->sax->endElement != NULL))
           ctxt->sax->endElement(ctxt->userData, name);
       oldname = docbnamePop(ctxt);
#ifdef DEBUG
        xmlGenericError(xmlGenericErrorContext,"End of tag the XML way: popping out %s\n", oldname);
#endif
       if (oldname != NULL)
           xmlFree(oldname);
       return;
    }

    if (CUR == '>') {
        NEXT;
    } else {
       if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
           ctxt->sax->error(ctxt->userData,
                            "Couldn't find end of Start Tag %s\n",
                            name);
       ctxt->wellFormed = 0;

       /*
        * end of parsing of this node.
        */
       if (xmlStrEqual(name, ctxt->name)) { 
           nodePop(ctxt);
           oldname = docbnamePop(ctxt);
#ifdef DEBUG
           xmlGenericError(xmlGenericErrorContext,"End of start tag problem: popping out %s\n", oldname);
#endif
           if (oldname != NULL)
               xmlFree(oldname);
       }    

       /*
        * Capture end position and add node
        */
       if ( currentNode != NULL && ctxt->record_info ) {
          node_info.end_pos = ctxt->input->consumed +
                             (CUR_PTR - ctxt->input->base);
          node_info.end_line = ctxt->input->line;
          node_info.node = ctxt->node;
          xmlParserAddNodeInfo(ctxt, &node_info);
       }
       return;
    }

    /*
     * Check for an Empty Element from DTD definition
     */
    if ((info != NULL) && (info->empty)) {
       if ((ctxt->sax != NULL) && (ctxt->sax->endElement != NULL))
           ctxt->sax->endElement(ctxt->userData, name);
       oldname = docbnamePop(ctxt);
#ifdef DEBUG
       xmlGenericError(xmlGenericErrorContext,"End of empty tag %s : popping out %s\n", name, oldname);
#endif
       if (oldname != NULL)
           xmlFree(oldname);
       return;
    }

    /*
     * Parse the content of the element:
     */
    currentNode = xmlStrdup(ctxt->name);
    depth = ctxt->nameNr;
    while (IS_CHAR(CUR)) {
       docbParseContent(ctxt);
       if (ctxt->nameNr < depth) break; 
    }  

    if (!IS_CHAR(CUR)) {
       /************
       if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
           ctxt->sax->error(ctxt->userData,
                "Premature end of data in tag %s\n", currentNode);
       ctxt->wellFormed = 0;
        *************/

       /*
        * end of parsing of this node.
        */
       nodePop(ctxt);
       oldname = docbnamePop(ctxt);
#ifdef DEBUG
       xmlGenericError(xmlGenericErrorContext,"Premature end of tag %s : popping out %s\n", name, oldname);
#endif
       if (oldname != NULL)
           xmlFree(oldname);
       if (currentNode != NULL)
           xmlFree(currentNode);
       return;
    }

    /*
     * Capture end position and add node
     */
    if ( currentNode != NULL && ctxt->record_info ) {
       node_info.end_pos = ctxt->input->consumed +
                          (CUR_PTR - ctxt->input->base);
       node_info.end_line = ctxt->input->line;
       node_info.node = ctxt->node;
       xmlParserAddNodeInfo(ctxt, &node_info);
    }
    if (currentNode != NULL)
       xmlFree(currentNode);
}

/**
 * docbParseEntityDecl:
 * @ctxt:  an SGML parser context
 *
 * parse <!ENTITY declarations
 *
 */

static void
docbParseEntityDecl(xmlParserCtxtPtr ctxt) {
    xmlChar *name = NULL;
    xmlChar *value = NULL;
    xmlChar *URI = NULL, *literal = NULL;
    xmlChar *ndata = NULL;
    int isParameter = 0;
    xmlChar *orig = NULL;
    
    GROW;
    if ((RAW == '<') && (NXT(1) == '!') &&
        (UPP(2) == 'E') && (UPP(3) == 'N') &&
        (UPP(4) == 'T') && (UPP(5) == 'I') &&
        (UPP(6) == 'T') && (UPP(7) == 'Y')) {
       xmlParserInputPtr input = ctxt->input;
       ctxt->instate = XML_PARSER_ENTITY_DECL;
       SHRINK;
       SKIP(8);
       if (!IS_BLANK(CUR)) {
           ctxt->errNo = XML_ERR_SPACE_REQUIRED;
           if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
               ctxt->sax->error(ctxt->userData,
                                "Space required after '<!ENTITY'\n");
           ctxt->wellFormed = 0;
           if (ctxt->recovery == 0) ctxt->disableSAX = 1;
       }
       SKIP_BLANKS;

       if (RAW == '%') {
           NEXT;
           if (!IS_BLANK(CUR)) {
               ctxt->errNo = XML_ERR_SPACE_REQUIRED;
               if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
                   ctxt->sax->error(ctxt->userData,
                                    "Space required after '%'\n");
               ctxt->wellFormed = 0;
               if (ctxt->recovery == 0) ctxt->disableSAX = 1;
           }
           SKIP_BLANKS;
           isParameter = 1;
       }

        name = xmlParseName(ctxt);
       if (name == NULL) {
           ctxt->errNo = XML_ERR_NAME_REQUIRED;
           if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
               ctxt->sax->error(ctxt->userData, "sgmlarseEntityDecl: no name\n");
           ctxt->wellFormed = 0;
           if (ctxt->recovery == 0) ctxt->disableSAX = 1;
            return;
       }
       if (!IS_BLANK(CUR)) {
           ctxt->errNo = XML_ERR_SPACE_REQUIRED;
           if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
               ctxt->sax->error(ctxt->userData,
                    "Space required after the entity name\n");
           ctxt->wellFormed = 0;
           if (ctxt->recovery == 0) ctxt->disableSAX = 1;
       }
        SKIP_BLANKS;

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
                       ctxt->wellFormed = 0;
                   } else {
                       if (uri->fragment != NULL) {
                           ctxt->errNo = XML_ERR_URI_FRAGMENT;
                           if ((ctxt->sax != NULL) &&
                               (!ctxt->disableSAX) &&
                               (ctxt->sax->error != NULL))
                               ctxt->sax->error(ctxt->userData,
                                           "Fragment not allowed: %s\n", URI);
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
                       ctxt->wellFormed = 0;
                   } else {
                       if (uri->fragment != NULL) {
                           ctxt->errNo = XML_ERR_URI_FRAGMENT;
                           if ((ctxt->sax != NULL) &&
                               (!ctxt->disableSAX) &&
                               (ctxt->sax->error != NULL))
                               ctxt->sax->error(ctxt->userData,
                                           "Fragment not allowed: %s\n", URI);
                           ctxt->wellFormed = 0;
                       }
                       xmlFreeURI(uri);
                   }
               }
               if ((RAW != '>') && (!IS_BLANK(CUR))) {
                   ctxt->errNo = XML_ERR_SPACE_REQUIRED;
                   if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
                       ctxt->sax->error(ctxt->userData,
                           "Space required before content model\n");
                   ctxt->wellFormed = 0;
                   if (ctxt->recovery == 0) ctxt->disableSAX = 1;
               }
               SKIP_BLANKS;

               /*
                * SGML specific: here we can get the content model
                */
               if (RAW != '>') {
                   xmlChar *contmod;

                   contmod = xmlParseName(ctxt);

                   if (contmod == NULL) {
                       ctxt->errNo = XML_ERR_SPACE_REQUIRED;
                       if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
                           ctxt->sax->error(ctxt->userData,
                               "Could not parse entity content model\n");
                       ctxt->wellFormed = 0;
                       if (ctxt->recovery == 0) ctxt->disableSAX = 1;
                   } else {
                       if (xmlStrEqual(contmod, BAD_CAST"NDATA")) {
                           if (!IS_BLANK(CUR)) {
                               ctxt->errNo = XML_ERR_SPACE_REQUIRED;
                               if ((ctxt->sax != NULL) &&
                                   (ctxt->sax->error != NULL))
                                   ctxt->sax->error(ctxt->userData,
                                       "Space required after 'NDATA'\n");
                               ctxt->wellFormed = 0;
                               if (ctxt->recovery == 0) ctxt->disableSAX = 1;
                           }
                           SKIP_BLANKS;
                           ndata = xmlParseName(ctxt);
                           if ((ctxt->sax != NULL) && (!ctxt->disableSAX) &&
                               (ctxt->sax->unparsedEntityDecl != NULL)) {
                               ctxt->sax->unparsedEntityDecl(ctxt->userData,
                                       name, literal, URI, ndata);
                           }
                       } else if (xmlStrEqual(contmod, BAD_CAST"SUBDOC")) {
                           if ((ctxt->sax != NULL) &&
                               (ctxt->sax->warning != NULL))
                               ctxt->sax->warning(ctxt->userData,
                                   "SUBDOC entities are not supported\n");
                           SKIP_BLANKS;
                           ndata = xmlParseName(ctxt);
                           if ((ctxt->sax != NULL) && (!ctxt->disableSAX) &&
                               (ctxt->sax->unparsedEntityDecl != NULL)) {
                               ctxt->sax->unparsedEntityDecl(ctxt->userData,
                                       name, literal, URI, ndata);
                           }
                       } else if (xmlStrEqual(contmod, BAD_CAST"CDATA")) {
                           if ((ctxt->sax != NULL) &&
                               (ctxt->sax->warning != NULL))
                               ctxt->sax->warning(ctxt->userData,
                                   "CDATA entities are not supported\n");
                           SKIP_BLANKS;
                           ndata = xmlParseName(ctxt);
                           if ((ctxt->sax != NULL) && (!ctxt->disableSAX) &&
                               (ctxt->sax->unparsedEntityDecl != NULL)) {
                               ctxt->sax->unparsedEntityDecl(ctxt->userData,
                                       name, literal, URI, ndata);
                           }
                       }
                       xmlFree(contmod);
                   }
               } else {
                   if ((ctxt->sax != NULL) &&
                       (!ctxt->disableSAX) && (ctxt->sax->entityDecl != NULL))
                       ctxt->sax->entityDecl(ctxt->userData, name,
                                   XML_EXTERNAL_GENERAL_PARSED_ENTITY,
                                   literal, URI, NULL);
               }
           }
       }
       SKIP_BLANKS;
       if (RAW != '>') {
           ctxt->errNo = XML_ERR_ENTITY_NOT_FINISHED;
           if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
               ctxt->sax->error(ctxt->userData, 
                   "docbParseEntityDecl: entity %s not terminated\n", name);
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
 * docbParseMarkupDecl:
 * @ctxt:  an SGML parser context
 * 
 * parse Markup declarations
 *
 * [29] markupdecl ::= elementdecl | AttlistDecl | EntityDecl |
 *                     NotationDecl | PI | Comment
 */
static void
docbParseMarkupDecl(xmlParserCtxtPtr ctxt) {
    GROW;
    xmlParseElementDecl(ctxt);
    xmlParseAttributeListDecl(ctxt);
    docbParseEntityDecl(ctxt);
    xmlParseNotationDecl(ctxt);
    docbParsePI(ctxt);
    xmlParseComment(ctxt);
    /*
     * This is only for internal subset. On external entities,
     * the replacement is done before parsing stage
     */
    if ((ctxt->external == 0) && (ctxt->inputNr == 1))
       xmlParsePEReference(ctxt);
    ctxt->instate = XML_PARSER_DTD;
}

/**
 * docbParseInternalSubset:
 * @ctxt:  an SGML parser context
 *
 * parse the internal subset declaration
 *
 * [28 end] ('[' (markupdecl | PEReference | S)* ']' S?)? '>'
 */

static void
docbParseInternalSubset(xmlParserCtxtPtr ctxt) {
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
           docbParseMarkupDecl(ctxt);
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
            "docbParseInternalSubset: error detected in Markup declaration\n");
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
 * docbParseMisc:
 * @ctxt:  an XML parser context
 * 
 * parse an XML Misc* optional field.
 *
 * [27] Misc ::= Comment | PI |  S
 */

static void
docbParseMisc(xmlParserCtxtPtr ctxt) {
    while (((RAW == '<') && (NXT(1) == '?')) ||
           ((RAW == '<') && (NXT(1) == '!') &&
           (NXT(2) == '-') && (NXT(3) == '-')) ||
           IS_BLANK(CUR)) {
        if ((RAW == '<') && (NXT(1) == '?')) {
            docbParsePI(ctxt);
        } else if (IS_BLANK(CUR)) {
            NEXT;
        } else
            xmlParseComment(ctxt);
    }
}

/**
 * docbParseDocument:
 * @ctxt:  an SGML parser context
 * 
 * parse an SGML document (and build a tree if using the standard SAX
 * interface).
 *
 * Returns 0, -1 in case of error. the parser context is augmented
 *                as a result of the parsing.
 */

int
docbParseDocument(docbParserCtxtPtr ctxt) {
    xmlChar start[4];
    xmlCharEncoding enc;
    xmlDtdPtr dtd;

    docbDefaultSAXHandlerInit();
    ctxt->html = 2;

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

    /*
     * Wipe out everything which is before the first '<'
     */
    SKIP_BLANKS;
    if (CUR == 0) {
       if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
           ctxt->sax->error(ctxt->userData, "Document is empty\n");
       ctxt->wellFormed = 0;
    }

    if ((ctxt->sax) && (ctxt->sax->startDocument) && (!ctxt->disableSAX))
       ctxt->sax->startDocument(ctxt->userData);


    /*
     * The Misc part of the Prolog
     */
    GROW;
    docbParseMisc(ctxt);

    /*
     * Then possibly doc type declaration(s) and more Misc
     * (doctypedecl Misc*)?
     */
    GROW;
    if ((RAW == '<') && (NXT(1) == '!') &&
       (UPP(2) == 'D') && (UPP(3) == 'O') &&
       (UPP(4) == 'C') && (UPP(5) == 'T') &&
       (UPP(6) == 'Y') && (UPP(7) == 'P') &&
       (UPP(8) == 'E')) {

       ctxt->inSubset = 1;
       docbParseDocTypeDecl(ctxt);
       if (RAW == '[') {
           ctxt->instate = XML_PARSER_DTD;
           docbParseInternalSubset(ctxt);
       }

       /*
        * Create and update the external subset.
        */
       ctxt->inSubset = 2;
       if ((ctxt->sax != NULL) && (ctxt->sax->internalSubset != NULL) &&
           (!ctxt->disableSAX))
           ctxt->sax->internalSubset(ctxt->userData, ctxt->intSubName,
                                     ctxt->extSubSystem, ctxt->extSubURI);
       ctxt->inSubset = 0;


       ctxt->instate = XML_PARSER_PROLOG;
       docbParseMisc(ctxt);
    }

    /*
     * Time to start parsing the tree itself
     */
    docbParseContent(ctxt);

    /*
     * autoclose
     */
    if (CUR == 0)
       docbAutoClose(ctxt, NULL);


    /*
     * SAX: end of the document processing.
     */
    if ((ctxt->sax) && (ctxt->sax->endDocument != NULL))
        ctxt->sax->endDocument(ctxt->userData);

    if (ctxt->myDoc != NULL) {
       dtd = ctxt->myDoc->intSubset;
       ctxt->myDoc->standalone = -1;
       if (dtd == NULL)
           ctxt->myDoc->intSubset = 
               xmlCreateIntSubset(ctxt->myDoc, BAD_CAST "SGML", 
                   BAD_CAST "-//W3C//DTD SGML 4.0 Transitional//EN",
                   BAD_CAST "http://www.w3.org/TR/REC-docbook/loose.dtd");
    }
    if (! ctxt->wellFormed) return(-1);
    return(0);
}


/************************************************************************
 *                                                                     *
 *                     Parser contexts handling                        *
 *                                                                     *
 ************************************************************************/

/**
 * docbInitParserCtxt:
 * @ctxt:  an SGML parser context
 *
 * Initialize a parser context
 */

static void
docbInitParserCtxt(docbParserCtxtPtr ctxt)
{
    docbSAXHandler *sax;

    if (ctxt == NULL) return;
    memset(ctxt, 0, sizeof(docbParserCtxt));

    sax = (docbSAXHandler *) xmlMalloc(sizeof(docbSAXHandler));
    if (sax == NULL) {
        xmlGenericError(xmlGenericErrorContext,
               "docbInitParserCtxt: out of memory\n");
    }
    memset(sax, 0, sizeof(docbSAXHandler));

    /* Allocate the Input stack */
    ctxt->inputTab = (docbParserInputPtr *) 
                      xmlMalloc(5 * sizeof(docbParserInputPtr));
    if (ctxt->inputTab == NULL) {
        xmlGenericError(xmlGenericErrorContext,
               "docbInitParserCtxt: out of memory\n");
    }
    ctxt->inputNr = 0;
    ctxt->inputMax = 5;
    ctxt->input = NULL;
    ctxt->version = NULL;
    ctxt->encoding = NULL;
    ctxt->standalone = -1;
    ctxt->instate = XML_PARSER_START;

    /* Allocate the Node stack */
    ctxt->nodeTab = (docbNodePtr *) xmlMalloc(10 * sizeof(docbNodePtr));
    ctxt->nodeNr = 0;
    ctxt->nodeMax = 10;
    ctxt->node = NULL;

    /* Allocate the Name stack */
    ctxt->nameTab = (xmlChar **) xmlMalloc(10 * sizeof(xmlChar *));
    ctxt->nameNr = 0;
    ctxt->nameMax = 10;
    ctxt->name = NULL;

    if (sax == NULL) ctxt->sax = &docbDefaultSAXHandler;
    else {
        ctxt->sax = sax;
       memcpy(sax, &docbDefaultSAXHandler, sizeof(docbSAXHandler));
    }
    ctxt->userData = ctxt;
    ctxt->myDoc = NULL;
    ctxt->wellFormed = 1;
    ctxt->linenumbers = xmlLineNumbersDefaultValue;
    ctxt->replaceEntities = xmlSubstituteEntitiesDefaultValue;
    ctxt->html = 2;
    ctxt->record_info = 0;
    ctxt->validate = 0;
    ctxt->nbChars = 0;
    ctxt->checkIndex = 0;
    xmlInitNodeInfoSeq(&ctxt->node_seq);
}

/**
 * docbFreeParserCtxt:
 * @ctxt:  an SGML parser context
 *
 * Free all the memory used by a parser context. However the parsed
 * document in ctxt->myDoc is not freed.
 */

void
docbFreeParserCtxt(docbParserCtxtPtr ctxt)
{
    xmlFreeParserCtxt(ctxt);
}

/**
 * docbCreateDocParserCtxt:
 * @cur:  a pointer to an array of xmlChar
 * @encoding: the SGML document encoding, or NULL
 *
 * Create a parser context for an SGML document.
 *
 * Returns the new parser context or NULL
 */
static docbParserCtxtPtr
docbCreateDocParserCtxt(xmlChar *cur, const char *encoding ATTRIBUTE_UNUSED) {
    docbParserCtxtPtr ctxt;
    docbParserInputPtr input;
    /* sgmlCharEncoding enc; */

    ctxt = (docbParserCtxtPtr) xmlMalloc(sizeof(docbParserCtxt));
    if (ctxt == NULL) {
        xmlGenericError(xmlGenericErrorContext, "malloc failed");
        return(NULL);
    }
    docbInitParserCtxt(ctxt);
    input = (docbParserInputPtr) xmlMalloc(sizeof(docbParserInput));
    if (input == NULL) {
        xmlGenericError(xmlGenericErrorContext, "malloc failed");
        xmlFree(ctxt);
        return(NULL);
    }
    memset(input, 0, sizeof(docbParserInput));

    input->line = 1;
    input->col = 1;
    input->base = cur;
    input->cur = cur;

    inputPush(ctxt, input);
    return(ctxt);
}

/************************************************************************
 *                                                                     *
 *             Progressive parsing interfaces                          *
 *                                                                     *
 ************************************************************************/

/**
 * docbParseLookupSequence:
 * @ctxt:  an SGML parser context
 * @first:  the first char to lookup
 * @next:  the next char to lookup or zero
 * @third:  the next char to lookup or zero
 *
 * Try to find if a sequence (first, next, third) or  just (first next) or
 * (first) is available in the input stream.
 * This function has a side effect of (possibly) incrementing ctxt->checkIndex
 * to avoid rescanning sequences of bytes, it DOES change the state of the
 * parser, do not use liberally.
 * This is basically similar to xmlParseLookupSequence()
 *
 * Returns the index to the current parsing point if the full sequence
 *      is available, -1 otherwise.
 */
static int
docbParseLookupSequence(docbParserCtxtPtr ctxt, xmlChar first,
                       xmlChar next, xmlChar third) {
    int base, len;
    docbParserInputPtr in;
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
                       "HPP: lookup '%c' found at %d\n",
                       first, base);
           else if (third == 0)
               xmlGenericError(xmlGenericErrorContext,
                       "HPP: lookup '%c%c' found at %d\n",
                       first, next, base);
           else 
               xmlGenericError(xmlGenericErrorContext,
                       "HPP: lookup '%c%c%c' found at %d\n",
                       first, next, third, base);
#endif
           return(base - (in->cur - in->base));
       }
    }
    ctxt->checkIndex = base;
#ifdef DEBUG_PUSH
    if (next == 0)
       xmlGenericError(xmlGenericErrorContext,
               "HPP: lookup '%c' failed\n", first);
    else if (third == 0)
       xmlGenericError(xmlGenericErrorContext,
               "HPP: lookup '%c%c' failed\n", first, next);
    else       
       xmlGenericError(xmlGenericErrorContext,
               "HPP: lookup '%c%c%c' failed\n", first, next, third);
#endif
    return(-1);
}

/**
 * docbParseTryOrFinish:
 * @ctxt:  an SGML parser context
 * @terminate:  last chunk indicator
 *
 * Try to progress on parsing
 *
 * Returns zero if no parsing was possible
 */
static int
docbParseTryOrFinish(docbParserCtxtPtr ctxt, int terminate) {
    int ret = 0;
    docbParserInputPtr in;
    int avail = 0;
    xmlChar cur, next;

#ifdef DEBUG_PUSH
    switch (ctxt->instate) {
       case XML_PARSER_EOF:
           xmlGenericError(xmlGenericErrorContext,
                   "HPP: try EOF\n"); break;
       case XML_PARSER_START:
           xmlGenericError(xmlGenericErrorContext,
                   "HPP: try START\n"); break;
       case XML_PARSER_MISC:
           xmlGenericError(xmlGenericErrorContext,
                   "HPP: try MISC\n");break;
       case XML_PARSER_COMMENT:
           xmlGenericError(xmlGenericErrorContext,
                   "HPP: try COMMENT\n");break;
       case XML_PARSER_PROLOG:
           xmlGenericError(xmlGenericErrorContext,
                   "HPP: try PROLOG\n");break;
       case XML_PARSER_START_TAG:
           xmlGenericError(xmlGenericErrorContext,
                   "HPP: try START_TAG\n");break;
       case XML_PARSER_CONTENT:
           xmlGenericError(xmlGenericErrorContext,
                   "HPP: try CONTENT\n");break;
       case XML_PARSER_CDATA_SECTION:
           xmlGenericError(xmlGenericErrorContext,
                   "HPP: try CDATA_SECTION\n");break;
       case XML_PARSER_END_TAG:
           xmlGenericError(xmlGenericErrorContext,
                   "HPP: try END_TAG\n");break;
       case XML_PARSER_ENTITY_DECL:
           xmlGenericError(xmlGenericErrorContext,
                   "HPP: try ENTITY_DECL\n");break;
       case XML_PARSER_ENTITY_VALUE:
           xmlGenericError(xmlGenericErrorContext,
                   "HPP: try ENTITY_VALUE\n");break;
       case XML_PARSER_ATTRIBUTE_VALUE:
           xmlGenericError(xmlGenericErrorContext,
                   "HPP: try ATTRIBUTE_VALUE\n");break;
       case XML_PARSER_DTD:
           xmlGenericError(xmlGenericErrorContext,
                   "HPP: try DTD\n");break;
       case XML_PARSER_EPILOG:
           xmlGenericError(xmlGenericErrorContext,
                   "HPP: try EPILOG\n");break;
       case XML_PARSER_PI:
           xmlGenericError(xmlGenericErrorContext,
                   "HPP: try PI\n");break;
    }
#endif

    while (1) {

       in = ctxt->input;
       if (in == NULL) break;
       if (in->buf == NULL)
           avail = in->length - (in->cur - in->base);
       else
           avail = in->buf->buffer->use - (in->cur - in->base);
       if ((avail == 0) && (terminate)) {
           docbAutoClose(ctxt, NULL);
           if ((ctxt->nameNr == 0) && (ctxt->instate != XML_PARSER_EOF)) { 
               /*
                * SAX: end of the document processing.
                */
               ctxt->instate = XML_PARSER_EOF;
               if ((ctxt->sax) && (ctxt->sax->endDocument != NULL))
                   ctxt->sax->endDocument(ctxt->userData);
           }
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
               /*
                * Very first chars read from the document flow.
                */
               cur = in->cur[0];
               if (IS_BLANK(cur)) {
                   SKIP_BLANKS;
                   if (in->buf == NULL)
                       avail = in->length - (in->cur - in->base);
                   else
                       avail = in->buf->buffer->use - (in->cur - in->base);
               }
               if ((ctxt->sax) && (ctxt->sax->setDocumentLocator))
                   ctxt->sax->setDocumentLocator(ctxt->userData,
                                                 &xmlDefaultSAXLocator);
               if ((ctxt->sax) && (ctxt->sax->startDocument) &&
                   (!ctxt->disableSAX))
                   ctxt->sax->startDocument(ctxt->userData);

               cur = in->cur[0];
               next = in->cur[1];
               if ((cur == '<') && (next == '!') &&
                   (UPP(2) == 'D') && (UPP(3) == 'O') &&
                   (UPP(4) == 'C') && (UPP(5) == 'T') &&
                   (UPP(6) == 'Y') && (UPP(7) == 'P') &&
                   (UPP(8) == 'E')) {
                   if ((!terminate) &&
                       (docbParseLookupSequence(ctxt, '>', 0, 0) < 0))
                       goto done;
#ifdef DEBUG_PUSH
                   xmlGenericError(xmlGenericErrorContext,
                           "HPP: Parsing internal subset\n");
#endif
                   docbParseDocTypeDecl(ctxt);
                   ctxt->instate = XML_PARSER_PROLOG;
#ifdef DEBUG_PUSH
                   xmlGenericError(xmlGenericErrorContext,
                           "HPP: entering PROLOG\n");
#endif
                } else {
                   ctxt->instate = XML_PARSER_MISC;
               }
#ifdef DEBUG_PUSH
               xmlGenericError(xmlGenericErrorContext,
                       "HPP: entering MISC\n");
#endif
               break;
            case XML_PARSER_MISC:
               SKIP_BLANKS;
               if (in->buf == NULL)
                   avail = in->length - (in->cur - in->base);
               else
                   avail = in->buf->buffer->use - (in->cur - in->base);
               if (avail < 2)
                   goto done;
               cur = in->cur[0];
               next = in->cur[1];
               if ((cur == '<') && (next == '!') &&
                   (in->cur[2] == '-') && (in->cur[3] == '-')) {
                   if ((!terminate) &&
                       (docbParseLookupSequence(ctxt, '-', '-', '>') < 0))
                       goto done;
#ifdef DEBUG_PUSH
                   xmlGenericError(xmlGenericErrorContext,
                           "HPP: Parsing Comment\n");
#endif
                   docbParseComment(ctxt);
                   ctxt->instate = XML_PARSER_MISC;
               } else if ((cur == '<') && (next == '!') &&
                   (UPP(2) == 'D') && (UPP(3) == 'O') &&
                   (UPP(4) == 'C') && (UPP(5) == 'T') &&
                   (UPP(6) == 'Y') && (UPP(7) == 'P') &&
                   (UPP(8) == 'E')) {
                   if ((!terminate) &&
                       (docbParseLookupSequence(ctxt, '>', 0, 0) < 0))
                       goto done;
#ifdef DEBUG_PUSH
                   xmlGenericError(xmlGenericErrorContext,
                           "HPP: Parsing internal subset\n");
#endif
                   docbParseDocTypeDecl(ctxt);
                   ctxt->instate = XML_PARSER_PROLOG;
#ifdef DEBUG_PUSH
                   xmlGenericError(xmlGenericErrorContext,
                           "HPP: entering PROLOG\n");
#endif
               } else if ((cur == '<') && (next == '!') &&
                          (avail < 9)) {
                   goto done;
               } else {
                   ctxt->instate = XML_PARSER_START_TAG;
#ifdef DEBUG_PUSH
                   xmlGenericError(xmlGenericErrorContext,
                           "HPP: entering START_TAG\n");
#endif
               }
               break;
            case XML_PARSER_PROLOG:
               SKIP_BLANKS;
               if (in->buf == NULL)
                   avail = in->length - (in->cur - in->base);
               else
                   avail = in->buf->buffer->use - (in->cur - in->base);
               if (avail < 2) 
                   goto done;
               cur = in->cur[0];
               next = in->cur[1];
               if ((cur == '<') && (next == '!') &&
                   (in->cur[2] == '-') && (in->cur[3] == '-')) {
                   if ((!terminate) &&
                       (docbParseLookupSequence(ctxt, '-', '-', '>') < 0))
                       goto done;
#ifdef DEBUG_PUSH
                   xmlGenericError(xmlGenericErrorContext,
                           "HPP: Parsing Comment\n");
#endif
                   docbParseComment(ctxt);
                   ctxt->instate = XML_PARSER_PROLOG;
               } else if ((cur == '<') && (next == '!') &&
                          (avail < 4)) {
                   goto done;
               } else {
                   ctxt->instate = XML_PARSER_START_TAG;
#ifdef DEBUG_PUSH
                   xmlGenericError(xmlGenericErrorContext,
                           "HPP: entering START_TAG\n");
#endif
               }
               break;
            case XML_PARSER_EPILOG:
               if (in->buf == NULL)
                   avail = in->length - (in->cur - in->base);
               else
                   avail = in->buf->buffer->use - (in->cur - in->base);
               if (avail < 1)
                   goto done;
               cur = in->cur[0];
               if (IS_BLANK(cur)) {
                   docbParseCharData(ctxt);
                   goto done;
               }
               if (avail < 2)
                   goto done;
               next = in->cur[1];
               if ((cur == '<') && (next == '!') &&
                   (in->cur[2] == '-') && (in->cur[3] == '-')) {
                   if ((!terminate) &&
                       (docbParseLookupSequence(ctxt, '-', '-', '>') < 0))
                       goto done;
#ifdef DEBUG_PUSH
                   xmlGenericError(xmlGenericErrorContext,
                           "HPP: Parsing Comment\n");
#endif
                   docbParseComment(ctxt);
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
                   ctxt->instate = XML_PARSER_EOF;
#ifdef DEBUG_PUSH
                   xmlGenericError(xmlGenericErrorContext,
                           "HPP: entering EOF\n");
#endif
                   if ((ctxt->sax) && (ctxt->sax->endDocument != NULL))
                       ctxt->sax->endDocument(ctxt->userData);
                   goto done;
               }
               break;
            case XML_PARSER_START_TAG: {
               xmlChar *name, *oldname;
               int depth = ctxt->nameNr;
               docbElemDescPtr info;

               if (avail < 2)
                   goto done;
               cur = in->cur[0];
               if (cur != '<') {
                   ctxt->instate = XML_PARSER_CONTENT;
#ifdef DEBUG_PUSH
                   xmlGenericError(xmlGenericErrorContext,
                           "HPP: entering CONTENT\n");
#endif
                   break;
               }
               if ((!terminate) &&
                   (docbParseLookupSequence(ctxt, '>', 0, 0) < 0))
                   goto done;

               oldname = xmlStrdup(ctxt->name);
               docbParseStartTag(ctxt);
               name = ctxt->name;
#ifdef DEBUG
               if (oldname == NULL)
                   xmlGenericError(xmlGenericErrorContext,
                           "Start of element %s\n", name);
               else if (name == NULL)  
                   xmlGenericError(xmlGenericErrorContext,
                           "Start of element failed, was %s\n",
                           oldname);
               else    
                   xmlGenericError(xmlGenericErrorContext,
                           "Start of element %s, was %s\n",
                           name, oldname);
#endif
               if (((depth == ctxt->nameNr) &&
                    (xmlStrEqual(oldname, ctxt->name))) ||
                   (name == NULL)) {
                   if (CUR == '>')
                       NEXT;
                   if (oldname != NULL)
                       xmlFree(oldname);
                   break;
               }
               if (oldname != NULL)
                   xmlFree(oldname);

               /*
                * Lookup the info for that element.
                */
               info = docbTagLookup(name);
               if (info == NULL) {
                   if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
                       ctxt->sax->error(ctxt->userData, "Tag %s unknown\n",
                                        name);
                   ctxt->wellFormed = 0;
               } else if (info->depr) {
                   /***************************
                   if ((ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
                       ctxt->sax->warning(ctxt->userData,
                                          "Tag %s is deprecated\n",
                                          name);
                    ***************************/
               }

               /*
                * Check for an Empty Element labeled the XML/SGML way
                */
               if ((CUR == '/') && (NXT(1) == '>')) {
                   SKIP(2);
                   if ((ctxt->sax != NULL) && (ctxt->sax->endElement != NULL))
                       ctxt->sax->endElement(ctxt->userData, name);
                   oldname = docbnamePop(ctxt);
#ifdef DEBUG
                   xmlGenericError(xmlGenericErrorContext,"End of tag the XML way: popping out %s\n",
                           oldname);
#endif
                   if (oldname != NULL)
                       xmlFree(oldname);
                   ctxt->instate = XML_PARSER_CONTENT;
#ifdef DEBUG_PUSH
                   xmlGenericError(xmlGenericErrorContext,
                           "HPP: entering CONTENT\n");
#endif
                   break;
               }

               if (CUR == '>') {
                   NEXT;
               } else {
                   if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
                       ctxt->sax->error(ctxt->userData, 
                                        "Couldn't find end of Start Tag %s\n",
                                        name);
                   ctxt->wellFormed = 0;

                   /*
                    * end of parsing of this node.
                    */
                   if (xmlStrEqual(name, ctxt->name)) { 
                       nodePop(ctxt);
                       oldname = docbnamePop(ctxt);
#ifdef DEBUG
                       xmlGenericError(xmlGenericErrorContext,
                        "End of start tag problem: popping out %s\n", oldname);
#endif
                       if (oldname != NULL)
                           xmlFree(oldname);
                   }    

                   ctxt->instate = XML_PARSER_CONTENT;
#ifdef DEBUG_PUSH
                   xmlGenericError(xmlGenericErrorContext,
                           "HPP: entering CONTENT\n");
#endif
                   break;
               }

               /*
                * Check for an Empty Element from DTD definition
                */
               if ((info != NULL) && (info->empty)) {
                   if ((ctxt->sax != NULL) && (ctxt->sax->endElement != NULL))
                       ctxt->sax->endElement(ctxt->userData, name);
                   oldname = docbnamePop(ctxt);
#ifdef DEBUG
                   xmlGenericError(xmlGenericErrorContext,"End of empty tag %s : popping out %s\n", name, oldname);
#endif
                   if (oldname != NULL)
                       xmlFree(oldname);
               }
               ctxt->instate = XML_PARSER_CONTENT;
#ifdef DEBUG_PUSH
               xmlGenericError(xmlGenericErrorContext,
                       "HPP: entering CONTENT\n");
#endif
                break;
           }
            case XML_PARSER_CONTENT: {
               long cons;
                /*
                * Handle preparsed entities and charRef
                */
               if (ctxt->token != 0) {
                   xmlChar chr[2] = { 0 , 0 } ;

                   chr[0] = (xmlChar) ctxt->token;
                   if ((ctxt->sax != NULL) && (ctxt->sax->characters != NULL))
                       ctxt->sax->characters(ctxt->userData, chr, 1);
                   ctxt->token = 0;
                   ctxt->checkIndex = 0;
               }
               if ((avail == 1) && (terminate)) {
                   cur = in->cur[0];
                   if ((cur != '<') && (cur != '&')) {
                       if (ctxt->sax != NULL) {
                           if (IS_BLANK(cur)) {
                               if (ctxt->sax->ignorableWhitespace != NULL)
                                   ctxt->sax->ignorableWhitespace(
                                           ctxt->userData, &cur, 1);
                           } else {
                               if (ctxt->sax->characters != NULL)
                                   ctxt->sax->characters(
                                           ctxt->userData, &cur, 1);
                           }
                       }
                       ctxt->token = 0;
                       ctxt->checkIndex = 0;
                       NEXT;
                   }
                   break;
               }
               if (avail < 2)
                   goto done;
               cur = in->cur[0];
               next = in->cur[1];
               cons = ctxt->nbChars;
               /*
                * Sometimes DOCTYPE arrives in the middle of the document
                */
               if ((cur == '<') && (next == '!') &&
                   (UPP(2) == 'D') && (UPP(3) == 'O') &&
                   (UPP(4) == 'C') && (UPP(5) == 'T') &&
                   (UPP(6) == 'Y') && (UPP(7) == 'P') &&
                   (UPP(8) == 'E')) {
                   if ((!terminate) &&
                       (docbParseLookupSequence(ctxt, '>', 0, 0) < 0))
                       goto done;
                   if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
                       ctxt->sax->error(ctxt->userData,
                            "Misplaced DOCTYPE declaration\n");
                   ctxt->wellFormed = 0;
                   docbParseDocTypeDecl(ctxt);
               } else if ((cur == '<') && (next == '!') &&
                   (in->cur[2] == '-') && (in->cur[3] == '-')) {
                   if ((!terminate) &&
                       (docbParseLookupSequence(ctxt, '-', '-', '>') < 0))
                       goto done;
#ifdef DEBUG_PUSH
                   xmlGenericError(xmlGenericErrorContext,
                           "HPP: Parsing Comment\n");
#endif
                   docbParseComment(ctxt);
                   ctxt->instate = XML_PARSER_CONTENT;
               } else if ((cur == '<') && (next == '!') && (avail < 4)) {
                   goto done;
               } else if ((cur == '<') && (next == '/')) {
                   ctxt->instate = XML_PARSER_END_TAG;
                   ctxt->checkIndex = 0;
#ifdef DEBUG_PUSH
                   xmlGenericError(xmlGenericErrorContext,
                           "HPP: entering END_TAG\n");
#endif
                   break;
               } else if (cur == '<') {
                   ctxt->instate = XML_PARSER_START_TAG;
                   ctxt->checkIndex = 0;
#ifdef DEBUG_PUSH
                   xmlGenericError(xmlGenericErrorContext,
                           "HPP: entering START_TAG\n");
#endif
                   break;
               } else if (cur == '&') {
                   if ((!terminate) &&
                       (docbParseLookupSequence(ctxt, ';', 0, 0) < 0))
                       goto done;
#ifdef DEBUG_PUSH
                   xmlGenericError(xmlGenericErrorContext,
                           "HPP: Parsing Reference\n");
#endif
                   /* TODO: check generation of subtrees if noent !!! */
                   docbParseReference(ctxt);
               } else {
                   /* TODO Avoid the extra copy, handle directly !!!!!! */
                   /*
                    * Goal of the following test is:
                    *  - minimize calls to the SAX 'character' callback
                    *    when they are mergeable
                    */
                   if ((ctxt->inputNr == 1) &&
                       (avail < DOCB_PARSER_BIG_BUFFER_SIZE)) {
                       if ((!terminate) &&
                           (docbParseLookupSequence(ctxt, '<', 0, 0) < 0))
                           goto done;
                    }
                   ctxt->checkIndex = 0;
#ifdef DEBUG_PUSH
                   xmlGenericError(xmlGenericErrorContext,
                           "HPP: Parsing char data\n");
#endif
                   docbParseCharData(ctxt);
               }
               if (cons == ctxt->nbChars) {
                   if (ctxt->node != NULL) {
                       if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
                           ctxt->sax->error(ctxt->userData,
                                "detected an error in element content\n");
                       ctxt->wellFormed = 0;
                       NEXT;
                   }
                   break;
               }

               break;
           }
            case XML_PARSER_END_TAG:
               if (avail < 2)
                   goto done;
               if ((!terminate) &&
                   (docbParseLookupSequence(ctxt, '>', 0, 0) < 0))
                   goto done;
               docbParseEndTag(ctxt);
               if (ctxt->nameNr == 0) {
                   ctxt->instate = XML_PARSER_EPILOG;
               } else {
                   ctxt->instate = XML_PARSER_CONTENT;
               }
               ctxt->checkIndex = 0;
#ifdef DEBUG_PUSH
               xmlGenericError(xmlGenericErrorContext,
                       "HPP: entering CONTENT\n");
#endif
               break;
            case XML_PARSER_CDATA_SECTION:
               xmlGenericError(xmlGenericErrorContext,
                       "HPP: internal error, state == CDATA\n");
               ctxt->instate = XML_PARSER_CONTENT;
               ctxt->checkIndex = 0;
#ifdef DEBUG_PUSH
               xmlGenericError(xmlGenericErrorContext,
                       "HPP: entering CONTENT\n");
#endif
               break;
            case XML_PARSER_DTD:
               xmlGenericError(xmlGenericErrorContext,
                       "HPP: internal error, state == DTD\n");
               ctxt->instate = XML_PARSER_CONTENT;
               ctxt->checkIndex = 0;
#ifdef DEBUG_PUSH
               xmlGenericError(xmlGenericErrorContext,
                       "HPP: entering CONTENT\n");
#endif
               break;
            case XML_PARSER_COMMENT:
               xmlGenericError(xmlGenericErrorContext,
                       "HPP: internal error, state == COMMENT\n");
               ctxt->instate = XML_PARSER_CONTENT;
               ctxt->checkIndex = 0;
#ifdef DEBUG_PUSH
               xmlGenericError(xmlGenericErrorContext,
                       "HPP: entering CONTENT\n");
#endif
               break;
            case XML_PARSER_PI:
               xmlGenericError(xmlGenericErrorContext,
                       "HPP: internal error, state == PI\n");
               ctxt->instate = XML_PARSER_CONTENT;
               ctxt->checkIndex = 0;
#ifdef DEBUG_PUSH
               xmlGenericError(xmlGenericErrorContext,
                       "HPP: entering CONTENT\n");
#endif
               break;
            case XML_PARSER_ENTITY_DECL:
               xmlGenericError(xmlGenericErrorContext,
                       "HPP: internal error, state == ENTITY_DECL\n");
               ctxt->instate = XML_PARSER_CONTENT;
               ctxt->checkIndex = 0;
#ifdef DEBUG_PUSH
               xmlGenericError(xmlGenericErrorContext,
                       "HPP: entering CONTENT\n");
#endif
               break;
            case XML_PARSER_ENTITY_VALUE:
               xmlGenericError(xmlGenericErrorContext,
                       "HPP: internal error, state == ENTITY_VALUE\n");
               ctxt->instate = XML_PARSER_CONTENT;
               ctxt->checkIndex = 0;
#ifdef DEBUG_PUSH
               xmlGenericError(xmlGenericErrorContext,
                       "HPP: entering DTD\n");
#endif
               break;
            case XML_PARSER_ATTRIBUTE_VALUE:
               xmlGenericError(xmlGenericErrorContext,
                       "HPP: internal error, state == ATTRIBUTE_VALUE\n");
               ctxt->instate = XML_PARSER_START_TAG;
               ctxt->checkIndex = 0;
#ifdef DEBUG_PUSH
               xmlGenericError(xmlGenericErrorContext,
                       "HPP: entering START_TAG\n");
#endif
               break;
           case XML_PARSER_SYSTEM_LITERAL:
               xmlGenericError(xmlGenericErrorContext,
                       "HPP: internal error, state == XML_PARSER_SYSTEM_LITERAL\n");
               ctxt->instate = XML_PARSER_CONTENT;
               ctxt->checkIndex = 0;
#ifdef DEBUG_PUSH
               xmlGenericError(xmlGenericErrorContext,
                       "HPP: entering CONTENT\n");
#endif
               break;

           case XML_PARSER_IGNORE:
               xmlGenericError(xmlGenericErrorContext,
                       "HPP: internal error, state == XML_PARSER_IGNORE\n");
               ctxt->instate = XML_PARSER_CONTENT;
               ctxt->checkIndex = 0;
#ifdef DEBUG_PUSH
               xmlGenericError(xmlGenericErrorContext,
                       "HPP: entering CONTENT\n");
#endif
               break;
	    case XML_PARSER_PUBLIC_LITERAL:
		xmlGenericError(xmlGenericErrorContext,
			"HPP: internal error, state == XML_PARSER_LITERAL\n");
		ctxt->instate = XML_PARSER_CONTENT;
		ctxt->checkIndex = 0;
#ifdef DEBUG_PUSH
		xmlGenericError(xmlGenericErrorContext,
			"HPP: entering CONTENT\n");
#endif
		break;
       }
    }
done:    
    if ((avail == 0) && (terminate)) {
       docbAutoClose(ctxt, NULL);
       if ((ctxt->nameNr == 0) && (ctxt->instate != XML_PARSER_EOF)) { 
           /*
            * SAX: end of the document processing.
            */
           ctxt->instate = XML_PARSER_EOF;
           if ((ctxt->sax) && (ctxt->sax->endDocument != NULL))
               ctxt->sax->endDocument(ctxt->userData);
       }
    }
    if ((ctxt->myDoc != NULL) &&
       ((terminate) || (ctxt->instate == XML_PARSER_EOF) ||
        (ctxt->instate == XML_PARSER_EPILOG))) {
       xmlDtdPtr dtd;
       dtd = ctxt->myDoc->intSubset;
       if (dtd == NULL)
           ctxt->myDoc->intSubset = 
               xmlCreateIntSubset(ctxt->myDoc, BAD_CAST "SGML", 
                   BAD_CAST "-//W3C//DTD SGML 4.0 Transitional//EN",
                   BAD_CAST "http://www.w3.org/TR/REC-docbook/loose.dtd");
    }
#ifdef DEBUG_PUSH
    xmlGenericError(xmlGenericErrorContext, "HPP: done %d\n", ret);
#endif
    return(ret);
}

/**
 * docbParseChunk:
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
docbParseChunk(docbParserCtxtPtr ctxt, const char *chunk, int size,
              int terminate) {
    if ((size > 0) && (chunk != NULL) && (ctxt->input != NULL) &&
        (ctxt->input->buf != NULL) && (ctxt->instate != XML_PARSER_EOF))  {
       int base = ctxt->input->base - ctxt->input->buf->buffer->content;
       int cur = ctxt->input->cur - ctxt->input->base;
       
       xmlParserInputBufferPush(ctxt->input->buf, size, chunk);              
       ctxt->input->base = ctxt->input->buf->buffer->content + base;
       ctxt->input->cur = ctxt->input->base + cur;
#ifdef DEBUG_PUSH
       xmlGenericError(xmlGenericErrorContext, "HPP: pushed %d\n", size);
#endif

       if ((terminate) || (ctxt->input->buf->buffer->use > 80))
           docbParseTryOrFinish(ctxt, terminate);
    } else if (ctxt->instate != XML_PARSER_EOF) {
       xmlParserInputBufferPush(ctxt->input->buf, 0, "");
        docbParseTryOrFinish(ctxt, terminate);
    }
    if (terminate) {
       if ((ctxt->instate != XML_PARSER_EOF) &&
           (ctxt->instate != XML_PARSER_EPILOG) &&
           (ctxt->instate != XML_PARSER_MISC)) {
           ctxt->errNo = XML_ERR_DOCUMENT_END;
           if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
               ctxt->sax->error(ctxt->userData,
                   "Extra content at the end of the document\n");
           ctxt->wellFormed = 0;
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
 *                                                                     *
 *                     User entry points                               *
 *                                                                     *
 ************************************************************************/

/**
 * docbCreatePushParserCtxt:
 * @sax:  a SAX handler
 * @user_data:  The user data returned on SAX callbacks
 * @chunk:  a pointer to an array of chars
 * @size:  number of chars in the array
 * @filename:  an optional file name or URI
 * @enc:  an optional encoding
 *
 * Create a parser context for using the DocBook SGML parser in push mode
 * To allow content encoding detection, @size should be >= 4
 * The value of @filename is used for fetching external entities
 * and error/warning reports.
 *
 * Returns the new parser context or NULL
 */
docbParserCtxtPtr
docbCreatePushParserCtxt(docbSAXHandlerPtr sax, void *user_data, 
                         const char *chunk, int size, const char *filename,
                        xmlCharEncoding enc) {
    docbParserCtxtPtr ctxt;
    docbParserInputPtr inputStream;
    xmlParserInputBufferPtr buf;

    buf = xmlAllocParserInputBuffer(enc);
    if (buf == NULL) return(NULL);

    ctxt = (docbParserCtxtPtr) xmlMalloc(sizeof(docbParserCtxt));
    if (ctxt == NULL) {
       xmlFree(buf);
       return(NULL);
    }
    memset(ctxt, 0, sizeof(docbParserCtxt));
    docbInitParserCtxt(ctxt);
    if (sax != NULL) {
       if (ctxt->sax != &docbDefaultSAXHandler)
           xmlFree(ctxt->sax);
       ctxt->sax = (docbSAXHandlerPtr) xmlMalloc(sizeof(docbSAXHandler));
       if (ctxt->sax == NULL) {
           xmlFree(buf);
           xmlFree(ctxt);
           return(NULL);
       }
       memcpy(ctxt->sax, sax, sizeof(docbSAXHandler));
       if (user_data != NULL)
           ctxt->userData = user_data;
    }  
    if (filename == NULL) {
       ctxt->directory = NULL;
    } else {
        ctxt->directory = xmlParserGetDirectory(filename);
    }

    inputStream = docbNewInputStream(ctxt);
    if (inputStream == NULL) {
       xmlFreeParserCtxt(ctxt);
       return(NULL);
    }

    if (filename == NULL)
       inputStream->filename = NULL;
    else
       inputStream->filename = xmlMemStrdup(filename);
    inputStream->buf = buf;
    inputStream->base = inputStream->buf->buffer->content;
    inputStream->cur = inputStream->buf->buffer->content;

    inputPush(ctxt, inputStream);

    if ((size > 0) && (chunk != NULL) && (ctxt->input != NULL) &&
        (ctxt->input->buf != NULL))  {       
       xmlParserInputBufferPush(ctxt->input->buf, size, chunk);              
#ifdef DEBUG_PUSH
       xmlGenericError(xmlGenericErrorContext, "HPP: pushed %d\n", size);
#endif
    }

    return(ctxt);
}

/**
 * docbSAXParseDoc:
 * @cur:  a pointer to an array of xmlChar
 * @encoding:  a free form C string describing the SGML document encoding, or NULL
 * @sax:  the SAX handler block
 * @userData: if using SAX, this pointer will be provided on callbacks. 
 *
 * parse an SGML in-memory document and build a tree.
 * It use the given SAX function block to handle the parsing callback.
 * If sax is NULL, fallback to the default DOM tree building routines.
 * 
 * Returns the resulting document tree
 */

docbDocPtr
docbSAXParseDoc(xmlChar *cur, const char *encoding, docbSAXHandlerPtr sax, void *userData) {
    docbDocPtr ret;
    docbParserCtxtPtr ctxt;

    if (cur == NULL) return(NULL);


    ctxt = docbCreateDocParserCtxt(cur, encoding);
    if (ctxt == NULL) return(NULL);
    if (sax != NULL) { 
        ctxt->sax = sax;
        ctxt->userData = userData;
    }

    docbParseDocument(ctxt);
    ret = ctxt->myDoc;
    if (sax != NULL) {
       ctxt->sax = NULL;
       ctxt->userData = NULL;
    }
    docbFreeParserCtxt(ctxt);
    
    return(ret);
}

/**
 * docbParseDoc:
 * @cur:  a pointer to an array of xmlChar
 * @encoding:  a free form C string describing the SGML document encoding, or NULL
 *
 * parse an SGML in-memory document and build a tree.
 * 
 * Returns the resulting document tree
 */

docbDocPtr
docbParseDoc(xmlChar *cur, const char *encoding) {
    return(docbSAXParseDoc(cur, encoding, NULL, NULL));
}


/**
 * docbCreateFileParserCtxt:
 * @filename:  the filename
 * @encoding:  the SGML document encoding, or NULL
 *
 * Create a parser context for a file content. 
 * Automatic support for ZLIB/Compress compressed document is provided
 * by default if found at compile-time.
 *
 * Returns the new parser context or NULL
 */
docbParserCtxtPtr
docbCreateFileParserCtxt(const char *filename,
	                 const char *encoding ATTRIBUTE_UNUSED)
{
    docbParserCtxtPtr ctxt;
    docbParserInputPtr inputStream;
    xmlParserInputBufferPtr buf;
    /* sgmlCharEncoding enc; */

    buf = xmlParserInputBufferCreateFilename(filename, XML_CHAR_ENCODING_NONE);
    if (buf == NULL) return(NULL);

    ctxt = (docbParserCtxtPtr) xmlMalloc(sizeof(docbParserCtxt));
    if (ctxt == NULL) {
        xmlGenericError(xmlGenericErrorContext, "malloc failed");
        return(NULL);
    }
    memset(ctxt, 0, sizeof(docbParserCtxt));
    docbInitParserCtxt(ctxt);
    inputStream = (docbParserInputPtr) xmlMalloc(sizeof(docbParserInput));
    if (inputStream == NULL) {
        xmlGenericError(xmlGenericErrorContext, "malloc failed");
        xmlFree(ctxt);
        return(NULL);
    }
    memset(inputStream, 0, sizeof(docbParserInput));

    inputStream->filename = (char *)
	xmlNormalizeWindowsPath((const xmlChar *)filename);
    inputStream->line = 1;
    inputStream->col = 1;
    inputStream->buf = buf;
    inputStream->directory = NULL;

    inputStream->base = inputStream->buf->buffer->content;
    inputStream->cur = inputStream->buf->buffer->content;
    inputStream->free = NULL;

    inputPush(ctxt, inputStream);
    return(ctxt);
}

/**
 * docbSAXParseFile:
 * @filename:  the filename
 * @encoding:  a free form C string describing the SGML document encoding, or NULL
 * @sax:  the SAX handler block
 * @userData: if using SAX, this pointer will be provided on callbacks. 
 *
 * parse an SGML file and build a tree. Automatic support for ZLIB/Compress
 * compressed document is provided by default if found at compile-time.
 * It use the given SAX function block to handle the parsing callback.
 * If sax is NULL, fallback to the default DOM tree building routines.
 *
 * Returns the resulting document tree
 */

docbDocPtr
docbSAXParseFile(const char *filename, const char *encoding, docbSAXHandlerPtr sax, 
                 void *userData) {
    docbDocPtr ret;
    docbParserCtxtPtr ctxt;
    docbSAXHandlerPtr oldsax = NULL;

    ctxt = docbCreateFileParserCtxt(filename, encoding);
    if (ctxt == NULL) return(NULL);
    if (sax != NULL) {
       oldsax = ctxt->sax;
        ctxt->sax = sax;
        ctxt->userData = userData;
    }

    docbParseDocument(ctxt);

    ret = ctxt->myDoc;
    if (sax != NULL) {
        ctxt->sax = oldsax;
        ctxt->userData = NULL;
    }
    docbFreeParserCtxt(ctxt);
    
    return(ret);
}

/**
 * docbParseFile:
 * @filename:  the filename
 * @encoding:  a free form C string describing document encoding, or NULL
 *
 * parse a Docbook SGML file and build a tree. Automatic support for
 * ZLIB/Compress compressed document is provided by default if found
 * at compile-time.
 *
 * Returns the resulting document tree
 */

docbDocPtr
docbParseFile(const char *filename, const char *encoding) {
    return(docbSAXParseFile(filename, encoding, NULL, NULL));
}

#endif /* LIBXML_DOCB_ENABLED */
