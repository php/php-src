/*
  This file is part of libXMLRPC - a C library for xml-encoded function calls.

  Author: Dan Libby (dan@libby.com)
  Epinions.com may be contacted at feedback@epinions-inc.com
*/

/*
  Copyright 2000 Epinions, Inc.

  Subject to the following 3 conditions, Epinions, Inc.  permits you, free
  of charge, to (a) use, copy, distribute, modify, perform and display this
  software and associated documentation files (the "Software"), and (b)
  permit others to whom the Software is furnished to do so as well.

  1) The above copyright notice and this permission notice shall be included
  without modification in all copies or substantial portions of the
  Software.

  2) THE SOFTWARE IS PROVIDED "AS IS", WITHOUT ANY WARRANTY OR CONDITION OF
  ANY KIND, EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION ANY
  IMPLIED WARRANTIES OF ACCURACY, MERCHANTABILITY, FITNESS FOR A PARTICULAR
  PURPOSE OR NONINFRINGEMENT.

  3) IN NO EVENT SHALL EPINIONS, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
  SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES OR LOST PROFITS ARISING OUT
  OF OR IN CONNECTION WITH THE SOFTWARE (HOWEVER ARISING, INCLUDING
  NEGLIGENCE), EVEN IF EPINIONS, INC.  IS AWARE OF THE POSSIBILITY OF SUCH
  DAMAGES.

*/


/****h* ABOUT/xml_element
 * NAME
 *   xml_element
 * AUTHOR
 *   Dan Libby, aka danda  (dan@libby.com)
 * CREATION DATE
 *   06/2000
 * HISTORY
 *   $Log$
 *   Revision 1.9.4.1.2.1  2008/12/09 17:22:12  iliaa
 *
 *   MFH: Fixed bug #46746 (xmlrpc_decode_request outputs non-suppressable error
 *   when given bad data).
 *
 *   Revision 1.9.4.1  2006/07/30 11:34:02  tony2001
 *   MFH: fix compile warnings (#38257)
 *
 *   Revision 1.9  2005/04/22 11:06:53  jorton
 *   Fixed bug #32797 (invalid C code in xmlrpc extension).
 *
 *   Revision 1.8  2005/03/28 00:07:24  edink
 *   Reshufle includes to make it compile on windows
 *
 *   Revision 1.7  2005/03/26 03:13:58  sniper
 *   - Made it possible to build ext/xmlrpc with libxml2
 *
 *   Revision 1.6  2004/06/01 20:16:06  iliaa
 *   Fixed bug #28597 (xmlrpc_encode_request() incorrectly encodes chars in
 *   200-210 range).
 *   Patch by: fernando dot nemec at folha dot com dot br
 *
 *   Revision 1.5  2003/12/16 21:00:21  sniper
 *   Fix some compile warnings (patch by Joe Orton)
 *
 *   Revision 1.4  2002/11/26 23:01:16  fmk
 *   removing unused variables
 *
 *   Revision 1.3  2002/07/05 04:43:53  danda
 *   merged in updates from SF project.  bring php repository up to date with xmlrpc-epi version 0.51
 *
 *   Revision 1.9  2002/07/03 20:54:30  danda
 *   root element should not have a parent. patch from anon SF user
 *
 *   Revision 1.8  2002/05/23 17:46:51  danda
 *   patch from mukund - fix non utf-8 encoding conversions
 *
 *   Revision 1.7  2002/02/13 20:58:50  danda
 *   patch to make source more windows friendly, contributed by Jeff Lawson
 *
 *   Revision 1.6  2002/01/08 01:06:55  danda
 *   enable <?xml version="1.0"?> format for parsers that are very picky.
 *
 *   Revision 1.5  2001/09/29 21:58:05  danda
 *   adding cvs log to history section
 *
 *   10/15/2000 -- danda -- adding robodoc documentation
 * TODO
 *   Nicer external API. Get rid of macros.  Make opaque types, etc.
 * PORTABILITY
 *   Coded on RedHat Linux 6.2.  Builds on Solaris x86.  Should build on just
 *   about anything with minor mods.
 * NOTES
 *   This code incorporates ideas from expat-ensor from http://xml.ensor.org.
 *
 *   It was coded primarily to act as a go-between for expat and xmlrpc. To this
 *   end, it stores xml elements, their sub-elements, and their attributes in an
 *   in-memory tree.  When expat is done parsing, the tree can be walked, thus
 *   retrieving the values.  The code can also be used to build a tree via API then
 *   write out the tree to a buffer, thus "serializing" the xml.
 *
 *   It turns out this is useful for other purposes, such as parsing config files.
 *   YMMV.
 *
 *   Some Features:
 *     - output option for xml escaping data.  Choices include no escaping, entity escaping,
 *       or CDATA sections.
 *     - output option for character encoding.  Defaults to (none) utf-8.
 *     - output option for verbosity/readability.  ultra-compact, newlines, pretty/level indented.
 *
 * BUGS
 *   there must be some.
 ******/

#include "ext/xml/expat_compat.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "xml_element.h"
#include "queue.h"
#include "encodings.h"

#define my_free(thing)  if(thing) {efree(thing); thing = NULL;}

#define XML_DECL_START                 "<?xml"
#define XML_DECL_START_LEN             sizeof(XML_DECL_START) - 1
#define XML_DECL_VERSION               "version=\"1.0\""
#define XML_DECL_VERSION_LEN           sizeof(XML_DECL_VERSION) - 1
#define XML_DECL_ENCODING_ATTR         "encoding"
#define XML_DECL_ENCODING_ATTR_LEN     sizeof(XML_DECL_ENCODING_ATTR) - 1
#define XML_DECL_ENCODING_DEFAULT      "utf-8"
#define XML_DECL_ENCODING_DEFAULT_LEN  sizeof(XML_DECL_ENCODING_DEFAULT) - 1
#define XML_DECL_END                   "?>"
#define XML_DECL_END_LEN               sizeof(XML_DECL_END) - 1
#define START_TOKEN_BEGIN              "<"
#define START_TOKEN_BEGIN_LEN          sizeof(START_TOKEN_BEGIN) - 1
#define START_TOKEN_END                ">"
#define START_TOKEN_END_LEN            sizeof(START_TOKEN_END) - 1
#define EMPTY_START_TOKEN_END          "/>"
#define EMPTY_START_TOKEN_END_LEN      sizeof(EMPTY_START_TOKEN_END) - 1
#define END_TOKEN_BEGIN                "</"
#define END_TOKEN_BEGIN_LEN            sizeof(END_TOKEN_BEGIN) - 1
#define END_TOKEN_END                  ">"
#define END_TOKEN_END_LEN              sizeof(END_TOKEN_END) - 1
#define ATTR_DELIMITER                 "\""
#define ATTR_DELIMITER_LEN             sizeof(ATTR_DELIMITER) - 1
#define CDATA_BEGIN                    "<![CDATA["
#define CDATA_BEGIN_LEN                sizeof(CDATA_BEGIN) - 1
#define CDATA_END                      "]]>"
#define CDATA_END_LEN                  sizeof(CDATA_END) - 1
#define EQUALS                         "="
#define EQUALS_LEN                     sizeof(EQUALS) - 1
#define WHITESPACE                     " "
#define WHITESPACE_LEN                 sizeof(WHITESPACE) - 1
#define NEWLINE                        "\n"
#define NEWLINE_LEN                    sizeof(NEWLINE) - 1
#define MAX_VAL_BUF                    144
#define SCALAR_STR                     "SCALAR"
#define SCALAR_STR_LEN                 sizeof(SCALAR_STR) - 1
#define VECTOR_STR                     "VECTOR"
#define VECTOR_STR_LEN                 sizeof(VECTOR_STR) - 1
#define RESPONSE_STR                   "RESPONSE"
#define RESPONSE_STR_LEN               sizeof(RESPONSE_STR) - 1


/*-----------------------------
- Begin xml_element Functions -
-----------------------------*/

/****f* xml_element/xml_elem_free_non_recurse
 * NAME
 *   xml_elem_free_non_recurse
 * SYNOPSIS
 *   void xml_elem_free_non_recurse(xml_element* root)
 * FUNCTION
 *   free a single xml element.  child elements will not be freed.
 * INPUTS
 *   root - the element to free
 * RESULT
 *   void
 * NOTES
 * SEE ALSO
 *   xml_elem_free ()
 *   xml_elem_new ()
 * SOURCE
 */
void xml_elem_free_non_recurse(xml_element* root) {
   if(root) {
      xml_element_attr* attrs = Q_Head(&root->attrs);
      while(attrs) {
         my_free(attrs->key);
         my_free(attrs->val);
         my_free(attrs);
         attrs = Q_Next(&root->attrs);
      }

      Q_Destroy(&root->children);
      Q_Destroy(&root->attrs);
      if(root->name) {
          efree((char *)root->name);
          root->name = NULL;
      }
      simplestring_free(&root->text);
      my_free(root);
   }
}
/******/

/****f* xml_element/xml_elem_free
 * NAME
 *   xml_elem_free
 * SYNOPSIS
 *   void xml_elem_free(xml_element* root)
 * FUNCTION
 *   free an xml element and all of its child elements
 * INPUTS
 *   root - the root of an xml tree you would like to free
 * RESULT
 *   void
 * NOTES
 * SEE ALSO
 *   xml_elem_free_non_recurse ()
 *   xml_elem_new ()
 * SOURCE
 */
void xml_elem_free(xml_element* root) {
   if(root) {
      xml_element* kids = Q_Head(&root->children);
      while(kids) {
         xml_elem_free(kids);
         kids = Q_Next(&root->children);
      }
      xml_elem_free_non_recurse(root);
   }
}
/******/

/****f* xml_element/xml_elem_new
 * NAME
 *   xml_elem_new
 * SYNOPSIS
 *   xml_element* xml_elem_new()
 * FUNCTION
 *   allocates and initializes a new xml_element
 * INPUTS
 *   none
 * RESULT
 *   xml_element* or NULL.  NULL indicates an out-of-memory condition.
 * NOTES
 * SEE ALSO
 *   xml_elem_free ()
 *   xml_elem_free_non_recurse ()
 * SOURCE
 */
xml_element* xml_elem_new() {
   xml_element* elem = ecalloc(1, sizeof(xml_element));
   if(elem) {
      Q_Init(&elem->children);
      Q_Init(&elem->attrs);
      simplestring_init(&elem->text);

      /* init empty string in case we don't find any char data */
      simplestring_addn(&elem->text, "", 0);
   }
   return elem;
}
/******/

static int xml_elem_writefunc(int (*fptr)(void *data, const char *text, int size), const char *text, void *data, int len)
{
   return fptr && text ? fptr(data, text, len ? len : strlen(text)) : 0;
}



static int create_xml_escape(char *pString, unsigned char c)
{
  int counter = 0;

  pString[counter++] = '&';
  pString[counter++] = '#';
  if(c >= 100) {
    pString[counter++] = c / 100 + '0';
    c = c % 100;
  }
  pString[counter++] = c / 10 + '0';
  c = c % 10;

  pString[counter++] = c + '0';
  pString[counter++] = ';';
  return counter;
}

#define non_ascii(c) (c > 127)
#define non_print(c) (!isprint(c))
#define markup(c) (c == '&' || c == '\"' || c == '>' || c == '<')
#define entity_length(c) ( (c >= 100) ? 3 : ((c >= 10) ? 2 : 1) ) + 3; /* "&#" + c + ";" */

/*
 * xml_elem_entity_escape
 *
 * Purpose:
 *   escape reserved xml chars and non utf-8 chars as xml entities
 * Comments:
 *   The return value may be a new string, or null if no
 *     conversion was performed. In the latter case, *newlen will
 *     be 0.
 * Flags (to escape)
 *  xml_elem_no_escaping             = 0x000,
 *  xml_elem_entity_escaping         = 0x002,   // escape xml special chars as entities
 *  xml_elem_non_ascii_escaping      = 0x008,   // escape chars above 127
 *  xml_elem_cdata_escaping          = 0x010,   // wrap in cdata
 */
static char* xml_elem_entity_escape(const char* buf, int old_len, int *newlen, XML_ELEM_ESCAPING flags) {
  char *pRetval = 0;
  int iNewBufLen=0;

#define should_escape(c, flag) ( ((flag & xml_elem_markup_escaping) && markup(c)) || \
                                 ((flag & xml_elem_non_ascii_escaping) && non_ascii(c)) || \
                                 ((flag & xml_elem_non_print_escaping) && non_print(c)) )

  if(buf && *buf) {
    const unsigned char *bufcopy;
    char *NewBuffer;
    int ToBeXmlEscaped=0;
    int iLength;
    bufcopy = (const unsigned char *) buf;
    iLength= old_len ? old_len : strlen(buf);
    while(*bufcopy) {
      if( should_escape(*bufcopy, flags) ) {
	/* the length will increase by length of xml escape - the character length */
	iLength += entity_length(*bufcopy);
	ToBeXmlEscaped=1;
      }
      bufcopy++;
    }

    if(ToBeXmlEscaped) {

      NewBuffer= emalloc(iLength+1);
      if(NewBuffer) {
	bufcopy = (const unsigned char *) buf;
	while(*bufcopy) {
	  if(should_escape(*bufcopy, flags)) {
	    iNewBufLen += create_xml_escape(NewBuffer+iNewBufLen,*bufcopy);
	  }
	  else {
	    NewBuffer[iNewBufLen++]=*bufcopy;
	  }
	  bufcopy++;
	}
	NewBuffer[iNewBufLen] = 0;
	pRetval = NewBuffer;
      }
    }
  }

  if(newlen) {
     *newlen = iNewBufLen;
  }

  return pRetval;
}


static void xml_element_serialize(xml_element *el, int (*fptr)(void *data, const char *text, int size), void *data, XML_ELEM_OUTPUT_OPTIONS options, int depth)
{
   int i;
   static STRUCT_XML_ELEM_OUTPUT_OPTIONS default_opts = {xml_elem_pretty, xml_elem_markup_escaping | xml_elem_non_print_escaping, XML_DECL_ENCODING_DEFAULT};
   static char whitespace[] = "                                                                                               "
                              "                                                                                               "
                              "                                                                                               ";
   depth++;

   if(!el) {
      /* fprintf(stderr, "Nothing to write\n"); */
      return;
   }
   if(!options) {
      options = &default_opts;
   }

   /* print xml declaration if at root level */
   if(depth == 1) {
      xml_elem_writefunc(fptr, XML_DECL_START, data, XML_DECL_START_LEN);
      xml_elem_writefunc(fptr, WHITESPACE, data, WHITESPACE_LEN);
      xml_elem_writefunc(fptr, XML_DECL_VERSION, data, XML_DECL_VERSION_LEN);
      if(options->encoding && *options->encoding) {
          xml_elem_writefunc(fptr, WHITESPACE, data, WHITESPACE_LEN);
          xml_elem_writefunc(fptr, XML_DECL_ENCODING_ATTR, data, XML_DECL_ENCODING_ATTR_LEN);
          xml_elem_writefunc(fptr, EQUALS, data, EQUALS_LEN);
          xml_elem_writefunc(fptr, ATTR_DELIMITER, data, ATTR_DELIMITER_LEN);
          xml_elem_writefunc(fptr, options->encoding, data, 0);
          xml_elem_writefunc(fptr, ATTR_DELIMITER, data, ATTR_DELIMITER_LEN);
      }
      xml_elem_writefunc(fptr, XML_DECL_END, data, XML_DECL_END_LEN);
      if(options->verbosity != xml_elem_no_white_space) {
         xml_elem_writefunc(fptr, NEWLINE, data, NEWLINE_LEN);
      }
   }

   if(options->verbosity == xml_elem_pretty && depth > 2) {
         xml_elem_writefunc(fptr, whitespace, data, depth - 2);
   }
   /* begin element */
   xml_elem_writefunc(fptr,START_TOKEN_BEGIN, data, START_TOKEN_BEGIN_LEN);
   if(el->name) {
      xml_elem_writefunc(fptr, el->name, data, 0);

      /* write attrs, if any */
      if(Q_Size(&el->attrs)) {
         xml_element_attr* iter = Q_Head(&el->attrs);
         while( iter ) {
            xml_elem_writefunc(fptr, WHITESPACE, data, WHITESPACE_LEN);
            xml_elem_writefunc(fptr, iter->key, data, 0);
            xml_elem_writefunc(fptr, EQUALS, data, EQUALS_LEN);
            xml_elem_writefunc(fptr, ATTR_DELIMITER, data, ATTR_DELIMITER_LEN);
            xml_elem_writefunc(fptr, iter->val, data, 0);
            xml_elem_writefunc(fptr, ATTR_DELIMITER, data, ATTR_DELIMITER_LEN);

            iter = Q_Next(&el->attrs);
         }
      }
   }
   else {
      xml_elem_writefunc(fptr, "None", data, 0);
   }
   /* if no text and no children, use abbreviated form, eg: <foo/> */
   if(!el->text.len && !Q_Size(&el->children)) {
       xml_elem_writefunc(fptr, EMPTY_START_TOKEN_END, data, EMPTY_START_TOKEN_END_LEN);
   }
   /* otherwise, print element contents */
   else {
       xml_elem_writefunc(fptr, START_TOKEN_END, data, START_TOKEN_END_LEN);

       /* print text, if any */
       if(el->text.len) {
          char* escaped_str = el->text.str;
          int buflen = el->text.len;

          if(options->escaping && options->escaping != xml_elem_cdata_escaping) {
             escaped_str = xml_elem_entity_escape(el->text.str, buflen, &buflen, options->escaping );
             if(!escaped_str) {
                escaped_str = el->text.str;
             }
          }

          if(options->escaping & xml_elem_cdata_escaping) {
             xml_elem_writefunc(fptr, CDATA_BEGIN, data, CDATA_BEGIN_LEN);
          }

          xml_elem_writefunc(fptr, escaped_str, data, buflen);

          if(escaped_str != el->text.str) {
             my_free(escaped_str);
          }

          if(options->escaping & xml_elem_cdata_escaping) {
             xml_elem_writefunc(fptr, CDATA_END, data, CDATA_END_LEN);
          }
       }
       /* no text, so print child elems */
       else {
          xml_element *kids = Q_Head(&el->children);
          i = 0;
          while( kids ) {
             if(i++ == 0) {
                if(options->verbosity != xml_elem_no_white_space) {
                   xml_elem_writefunc(fptr, NEWLINE, data, NEWLINE_LEN);
                }
             }
             xml_element_serialize(kids, fptr, data, options, depth);
             kids = Q_Next(&el->children);
          }
          if(i) {
             if(options->verbosity == xml_elem_pretty && depth > 2) {
                   xml_elem_writefunc(fptr, whitespace, data, depth - 2);
             }
          }
       }

       xml_elem_writefunc(fptr, END_TOKEN_BEGIN, data, END_TOKEN_BEGIN_LEN);
       xml_elem_writefunc(fptr,el->name ? el->name : "None", data, 0);
       xml_elem_writefunc(fptr, END_TOKEN_END, data, END_TOKEN_END_LEN);
   }
   if(options->verbosity != xml_elem_no_white_space) {
      xml_elem_writefunc(fptr, NEWLINE, data, NEWLINE_LEN);
   }
}

/* print buf to file */
static int file_out_fptr(void *f, const char *text, int size)
{
   fputs(text, (FILE *)f);
   return 0;
}

/* print buf to simplestring */
static int simplestring_out_fptr(void *f, const char *text, int size)
{
   simplestring* buf = (simplestring*)f;
   if(buf) {
      simplestring_addn(buf, text, size);
   }
   return 0;
}

/****f* xml_element/xml_elem_serialize_to_string
 * NAME
 *   xml_elem_serialize_to_string
 * SYNOPSIS
 *   void xml_element_serialize_to_string(xml_element *el, XML_ELEM_OUTPUT_OPTIONS options, int *buf_len)
 * FUNCTION
 *   writes element tree as XML into a newly allocated buffer
 * INPUTS
 *   el      - root element of tree
 *   options - options determining how output is written.  see XML_ELEM_OUTPUT_OPTIONS
 *   buf_len - length of returned buffer, if not null.
 * RESULT
 *   char* or NULL. Must be free'd by caller.
 * NOTES
 * SEE ALSO
 *   xml_elem_serialize_to_stream ()
 *   xml_elem_parse_buf ()
 * SOURCE
 */
char* xml_elem_serialize_to_string(xml_element *el, XML_ELEM_OUTPUT_OPTIONS options, int *buf_len)
{
   simplestring buf;
   simplestring_init(&buf);

   xml_element_serialize(el, simplestring_out_fptr, (void *)&buf, options, 0);

   if(buf_len) {
      *buf_len = buf.len;
   }

   return buf.str;
}
/******/

/****f* xml_element/xml_elem_serialize_to_stream
 * NAME
 *   xml_elem_serialize_to_stream
 * SYNOPSIS
 *   void xml_elem_serialize_to_stream(xml_element *el, FILE *output, XML_ELEM_OUTPUT_OPTIONS options)
 * FUNCTION
 *   writes element tree as XML into a stream (typically an opened file)
 * INPUTS
 *   el      - root element of tree
 *   output  - stream handle
 *   options - options determining how output is written.  see XML_ELEM_OUTPUT_OPTIONS
 * RESULT
 *   void
 * NOTES
 * SEE ALSO
 *   xml_elem_serialize_to_string ()
 *   xml_elem_parse_buf ()
 * SOURCE
 */
void xml_elem_serialize_to_stream(xml_element *el, FILE *output, XML_ELEM_OUTPUT_OPTIONS options)
{
   xml_element_serialize(el, file_out_fptr, (void *)output, options, 0);
}
/******/

/*--------------------------*
* End xml_element Functions *
*--------------------------*/


/*----------------------
* Begin Expat Handlers *
*---------------------*/

typedef struct _xml_elem_data {
   xml_element*           root;
   xml_element*           current;
   XML_ELEM_INPUT_OPTIONS input_options;
   int                    needs_enc_conversion;
} xml_elem_data;


/* expat start of element handler */
static void _xmlrpc_startElement(void *userData, const char *name, const char **attrs)
{
   xml_element *c;
   xml_elem_data* mydata = (xml_elem_data*)userData;
   const char** p = attrs;

   if(mydata) {
      c = mydata->current;

      mydata->current = xml_elem_new();
      mydata->current->name = (char*)estrdup(name);
      mydata->current->parent = c;

      /* init attrs */
      while(p && *p) {
         xml_element_attr* attr = emalloc(sizeof(xml_element_attr));
         if(attr) {
            attr->key = estrdup(*p);
            attr->val = estrdup(*(p+1));
            Q_PushTail(&mydata->current->attrs, attr);

            p += 2;
         }
      }
   }
}

/* expat end of element handler */
static void _xmlrpc_endElement(void *userData, const char *name)
{
   xml_elem_data* mydata = (xml_elem_data*)userData;

   if(mydata && mydata->current && mydata->current->parent) {
      Q_PushTail(&mydata->current->parent->children, mydata->current);

      mydata->current = mydata->current->parent;
   }
}

/* expat char data handler */
static void _xmlrpc_charHandler(void *userData,
                        const char *s,
                        int len)
{
   xml_elem_data* mydata = (xml_elem_data*)userData;
   if(mydata && mydata->current) {

      /* Check if we need to decode utf-8 parser output to another encoding */
      if(mydata->needs_enc_conversion && mydata->input_options->encoding) {
         int new_len = 0;
         char* add_text = utf8_decode(s, len, &new_len, mydata->input_options->encoding);
         if(add_text) {
            len = new_len;
            simplestring_addn(&mydata->current->text, add_text, len);
            efree(add_text);
            return;
         }
      }
      simplestring_addn(&mydata->current->text, s, len);
   }
}
/******/

/*-------------------*
* End Expat Handlers *
*-------------------*/

/*-------------------*
* xml_elem_parse_buf *
*-------------------*/

/****f* xml_element/xml_elem_parse_buf
 * NAME
 *   xml_elem_parse_buf
 * SYNOPSIS
 *   xml_element* xml_elem_parse_buf(const char* in_buf, int len, XML_ELEM_INPUT_OPTIONS options, XML_ELEM_ERROR error)
 * FUNCTION
 *   parse a buffer containing XML into an xml_element in-memory tree
 * INPUTS
 *   in_buf   - buffer containing XML document
 *   len      - length of buffer
 *   options  - input options. optional
 *   error    - error result data. optional. check if result is null.
 * RESULT
 *   void
 * NOTES
 *   The returned data must be free'd by caller
 * SEE ALSO
 *   xml_elem_serialize_to_string ()
 *   xml_elem_free ()
 * SOURCE
 */
xml_element* xml_elem_parse_buf(const char* in_buf, int len, XML_ELEM_INPUT_OPTIONS options, XML_ELEM_ERROR error)
{
   xml_element* xReturn = NULL;
   char buf[100] = "";
   static STRUCT_XML_ELEM_INPUT_OPTIONS default_opts = {encoding_utf_8};

   if(!options) {
      options = &default_opts;
   }

   if(in_buf) {
      XML_Parser parser;
      xml_elem_data mydata = {0};

      parser = XML_ParserCreate(NULL);

      mydata.root = xml_elem_new();
      mydata.current = mydata.root;
      mydata.input_options = options;
      mydata.needs_enc_conversion = options->encoding && strcmp(options->encoding, encoding_utf_8);

      XML_SetElementHandler(parser, (XML_StartElementHandler)_xmlrpc_startElement, (XML_EndElementHandler)_xmlrpc_endElement);
      XML_SetCharacterDataHandler(parser, (XML_CharacterDataHandler)_xmlrpc_charHandler);

      /* pass the xml_elem_data struct along */
      XML_SetUserData(parser, (void*)&mydata);

      if(!len) {
         len = strlen(in_buf);
      }

      /* parse the XML */
      if(XML_Parse(parser, (const unsigned char *) in_buf, len, 1) == 0) {
         enum XML_Error err_code = XML_GetErrorCode(parser);
         int line_num = XML_GetCurrentLineNumber(parser);
         int col_num = XML_GetCurrentColumnNumber(parser);
         long byte_idx = XML_GetCurrentByteIndex(parser);
/*         int byte_total = XML_GetCurrentByteCount(parser); */
         const char * error_str = (const char *) XML_ErrorString(err_code);
         if(byte_idx > len) {
             byte_idx = len;
         }
         if(byte_idx >= 0) {
             snprintf(buf,
                      sizeof(buf),
                      "\n\tdata beginning %ld before byte index: %s\n",
                      byte_idx > 10  ? 10 : byte_idx,
                      in_buf + (byte_idx > 10 ? byte_idx - 10 : byte_idx));
         }
/*
         fprintf(stderr, "expat reports error code %i\n"
                "\tdescription: %s\n"
                "\tline: %i\n"
                "\tcolumn: %i\n"
                "\tbyte index: %ld\n"
                "\ttotal bytes: %i\n%s ",
                err_code, error_str, line_num,
                col_num, byte_idx, byte_total, buf);
*/

          /* error condition */
          if(error) {
              error->parser_code = (long)err_code;
              error->line = line_num;
              error->column = col_num;
              error->byte_index = byte_idx;
              error->parser_error = error_str;
          }
      }
      else {
         xReturn = (xml_element*)Q_Head(&mydata.root->children);
         xReturn->parent = NULL;
      }

      XML_ParserFree(parser);


      xml_elem_free_non_recurse(mydata.root);
   }

   return xReturn;
}

/******/
