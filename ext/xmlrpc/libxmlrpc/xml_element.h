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

#ifndef __XML_ELEMENT_H__
 #define __XML_ELEMENT_H__

/* includes */
#include <stdio.h>
#include "queue.h"
#include "simplestring.h"
#include "encodings.h"

#ifdef __cplusplus
extern "C" {
#endif

/****d* enum/XML_ELEM_VERBOSITY
 * NAME
 *   XML_ELEM_VERBOSITY
 * NOTES
 *   verbosity/readability options for generated xml
 * SEE ALSO
 *   XML_ELEM_OUTPUT_OPTIONS
 * SOURCE
 */
typedef enum _xml_elem_verbosity {
   xml_elem_no_white_space,    /* compact xml with no white space            */
   xml_elem_newlines_only,     /* add newlines for enhanced readability      */
   xml_elem_pretty             /* add newlines and indent accordint to depth */
} XML_ELEM_VERBOSITY;
/******/


/****d* enum/XML_ELEM_ESCAPING
 * NAME
 *   XML_ELEM_ESCAPING
 * NOTES
 * xml escaping options for generated xml
 * SEE ALSO
 *   XML_ELEM_OUTPUT_OPTIONS
 * SOURCE
 */
typedef enum _xml_elem_escaping {
   xml_elem_no_escaping             = 0x000,
   xml_elem_markup_escaping         = 0x002,   /* entity escape xml special chars         */
   xml_elem_non_ascii_escaping      = 0x008,   /* entity escape chars above 127           */
   xml_elem_non_print_escaping      = 0x010,   /* entity escape non print (illegal) chars */
   xml_elem_cdata_escaping          = 0x020,   /* wrap in cdata section                   */
} XML_ELEM_ESCAPING;
/******/


/****s* struct/XML_ELEM_OUTPUT_OPTIONS
 * NAME
 *   XML_ELEM_OUTPUT_OPTIONS
 * NOTES
 *   defines various output options
 * SOURCE
 */
typedef struct _xml_output_options {
   XML_ELEM_VERBOSITY           verbosity;      /* length/verbosity of xml        */
   XML_ELEM_ESCAPING            escaping;       /* how to escape special chars    */
   const char*                  encoding;       /* <?xml encoding="<encoding>" ?> */
} STRUCT_XML_ELEM_OUTPUT_OPTIONS, *XML_ELEM_OUTPUT_OPTIONS;
/******/

/****s* struct/XML_ELEM_INPUT_OPTIONS
 * NAME
 *   XML_ELEM_INPUT_OPTIONS
 * NOTES
 *   defines various input options
 * SOURCE
 */
typedef struct _xml_input_options {
  ENCODING_ID                  encoding;       /* which encoding to use.       */
} STRUCT_XML_ELEM_INPUT_OPTIONS, *XML_ELEM_INPUT_OPTIONS;
/******/

/****s* struct/XML_ELEM_ERROR
 * NAME
 *   XML_ELEM_ERROR
 * NOTES
 *   defines an xml parser error
 * SOURCE
 */
typedef struct _xml_elem_error {
  int parser_code;
  const char* parser_error;
  long line;
  long column;
  long byte_index;
} STRUCT_XML_ELEM_ERROR, *XML_ELEM_ERROR;
/******/


/*-************************
* begin xml element stuff *
**************************/

/****s* struct/xml_elem_attr
 * NAME
 *  xml_elem_attr
 * NOTES
 *   representation of an xml attribute, foo="bar"
 * SOURCE
 */
typedef struct _xml_element_attr {
   char* key;        /* attribute key   */
   char* val;        /* attribute value */
} xml_element_attr;
/******/

/****s* struct/xml_elem_attr
 * NAME
 *  xml_elem_attr
 * NOTES
 *   representation of an xml element, eg <candidate name="Harry Browne" party="Libertarian"/>
 * SOURCE
 */
typedef struct _xml_element {
   const char*   name;           /* element identifier */
   simplestring  text;           /* text contained between element begin/end pairs */
   struct _xml_element* parent;  /* element's parent */

   queue        attrs;           /* attribute list */
   queue        children;        /* child element list */
} xml_element;
/******/

void xml_elem_free(xml_element* root);
void xml_elem_free_non_recurse(xml_element* root);
xml_element* xml_elem_new(void);
char* xml_elem_serialize_to_string(xml_element *el, XML_ELEM_OUTPUT_OPTIONS options, int *buf_len);
void xml_elem_serialize_to_stream(xml_element *el, FILE *output, XML_ELEM_OUTPUT_OPTIONS options);
xml_element* xml_elem_parse_buf(const char* in_buf, int len, XML_ELEM_INPUT_OPTIONS options, XML_ELEM_ERROR error);

/*-**********************
* end xml element stuff *
************************/

/*-**********************
* Begin xml_element API *
************************/

/****d* VALUE/XMLRPC_MACROS
 * NAME
 *   Some Helpful Macros
 * NOTES
 *   Some macros for making life easier.  Should be self-explanatory.
 * SEE ALSO
 *   XMLRPC_AddValueToVector ()
 *   XMLRPC_VectorGetValueWithID_Case ()
 *   XMLRPC_VALUE
 * SOURCE
 */
#define xml_elem_next_element(el) ((el) ? (xml_element *)Q_Next(&el->children) : NULL)
#define xml_elem_head_element(el) ((el) ? (xml_element *)Q_Head(&el->children) : NULL)
#define xml_elem_next_attr(el) ((el) ? (xml_element_attr *)Q_Next(&el->attrs) : NULL)
#define xml_elem_head_attr(el) ((el) ? (xml_element_attr *)Q_Head(&el->attrs) : NULL)
#define xml_elem_get_name(el) (char *)((el) ? el->name : NULL)
#define xml_elem_get_val(el) (char *)((el) ? el->text.str : NULL)
/******/


/*-********************
* End xml_element API *
**********************/

#ifdef __cplusplus
}
#endif

#endif /* __XML_ELEMENT_H__ */
