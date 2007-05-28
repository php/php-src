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

#ifdef _WIN32
#include "xmlrpc_win32.h"
#endif
#include <string.h>
#include <stdlib.h>
#include "xml_to_dandarpc.h"
#include "base64.h"

/* list of tokens used in vocab */
#define ELEM_METHODCALL     "methodCall"
#define ELEM_METHODNAME     "methodName"
#define ELEM_METHODRESPONSE "methodResponse"
#define ELEM_ROOT           "simpleRPC"

#define ATTR_ARRAY          "array"
#define ATTR_BASE64         "base64"
#define ATTR_BOOLEAN        "boolean"
#define ATTR_DATETIME       "dateTime.iso8601"
#define ATTR_DOUBLE         "double"
#define ATTR_ID             "id"
#define ATTR_INT            "int"
#define ATTR_MIXED          "mixed"
#define ATTR_SCALAR         "scalar"
#define ATTR_STRING         "string"
#define ATTR_STRUCT         "struct"
#define ATTR_TYPE           "type"
#define ATTR_VECTOR         "vector"
#define ATTR_VERSION        "version"

#define VAL_VERSION_0_9     "0.9"


XMLRPC_VALUE xml_element_to_DANDARPC_REQUEST_worker(XMLRPC_REQUEST request, XMLRPC_VALUE xCurrent, xml_element* el) {
   if(!xCurrent) {
      xCurrent = XMLRPC_CreateValueEmpty();
   }

   if(el->name) {
      const char* id = NULL;
      const char* type = NULL;
      xml_element_attr* attr_iter = Q_Head(&el->attrs);

      while(attr_iter) {
         if(!strcmp(attr_iter->key, ATTR_ID)) {
            id = attr_iter->val;
         }
         if(!strcmp(attr_iter->key, ATTR_TYPE)) {
            type = attr_iter->val;
         }
         attr_iter = Q_Next(&el->attrs);
      }

      if(id) {
         XMLRPC_SetValueID_Case(xCurrent, id, 0, xmlrpc_case_exact);
      }

      if(!strcmp(el->name, ATTR_SCALAR)) {
         if(!type || !strcmp(type, ATTR_STRING)) {
            XMLRPC_SetValueString(xCurrent, el->text.str, el->text.len);
         }
         else if(!strcmp(type, ATTR_INT)) {
            XMLRPC_SetValueInt(xCurrent, atoi(el->text.str));
         }
         else if(!strcmp(type, ATTR_BOOLEAN)) {
            XMLRPC_SetValueBoolean(xCurrent, atoi(el->text.str));
         }
         else if(!strcmp(type, ATTR_DOUBLE)) {
            XMLRPC_SetValueDouble(xCurrent, atof(el->text.str));
         }
         else if(!strcmp(type, ATTR_DATETIME)) {
            XMLRPC_SetValueDateTime_ISO8601(xCurrent, el->text.str);
         }
         else if(!strcmp(type, ATTR_BASE64)) {
            struct buffer_st buf;
            base64_decode_xmlrpc(&buf, el->text.str, el->text.len);
            XMLRPC_SetValueBase64(xCurrent, buf.data, buf.offset);
            buffer_delete(&buf);
         }
      }
      else if(!strcmp(el->name, ATTR_VECTOR)) {
         xml_element* iter = (xml_element*)Q_Head(&el->children);

         if(!type || !strcmp(type, ATTR_MIXED)) {
            XMLRPC_SetIsVector(xCurrent, xmlrpc_vector_mixed);
         }
         else if(!strcmp(type, ATTR_ARRAY)) {
				XMLRPC_SetIsVector(xCurrent, xmlrpc_vector_array);
         }
         else if(!strcmp(type, ATTR_STRUCT)) {
            XMLRPC_SetIsVector(xCurrent, xmlrpc_vector_struct);
         }
         while( iter ) {
            XMLRPC_VALUE xNext = XMLRPC_CreateValueEmpty();
            xml_element_to_DANDARPC_REQUEST_worker(request, xNext, iter);
            XMLRPC_AddValueToVector(xCurrent, xNext);
            iter = (xml_element*)Q_Next(&el->children);
         }
      }
      else {
         xml_element* iter = (xml_element*)Q_Head(&el->children);
         while( iter ) {
            xml_element_to_DANDARPC_REQUEST_worker(request, xCurrent, iter);
            iter = (xml_element*)Q_Next(&el->children);
         }

         if(!strcmp(el->name, ELEM_METHODCALL)) {
            if(request) {
               XMLRPC_RequestSetRequestType(request, xmlrpc_request_call);
            }
         }
         else if(!strcmp(el->name, ELEM_METHODRESPONSE)) {
            if(request) {
               XMLRPC_RequestSetRequestType(request, xmlrpc_request_response);
            }
         }
         else if(!strcmp(el->name, ELEM_METHODNAME)) {
            if(request) {
               XMLRPC_RequestSetMethodName(request, el->text.str);
            }
         }
      }
   }
   return xCurrent;
}

XMLRPC_VALUE xml_element_to_DANDARPC_VALUE(xml_element* el)
{
   return xml_element_to_DANDARPC_REQUEST_worker(NULL, NULL, el);
}

XMLRPC_VALUE xml_element_to_DANDARPC_REQUEST(XMLRPC_REQUEST request, xml_element* el)
{
   if(request) {
      return XMLRPC_RequestSetData(request, xml_element_to_DANDARPC_REQUEST_worker(request, NULL, el));
   }
   return NULL;
}

xml_element* DANDARPC_to_xml_element_worker(XMLRPC_REQUEST request, XMLRPC_VALUE node) {
#define BUF_SIZE 512
   xml_element* root = NULL;
   if(node) {
      char buf[BUF_SIZE];
      const char* id = XMLRPC_GetValueID(node);
      XMLRPC_VALUE_TYPE type = XMLRPC_GetValueType(node);
      XMLRPC_REQUEST_OUTPUT_OPTIONS output = XMLRPC_RequestGetOutputOptions(request);
      int bNoAddType = (type == xmlrpc_string && request && output && output->xml_elem_opts.verbosity == xml_elem_no_white_space);
      xml_element* elem_val = xml_elem_new();
      const char* pAttrType = NULL;

      xml_element_attr* attr_type = bNoAddType ? NULL : malloc(sizeof(xml_element_attr));
       
      if(attr_type) {
         attr_type->key = strdup(ATTR_TYPE);
         attr_type->val = 0;
         Q_PushTail(&elem_val->attrs, attr_type);
      }

      elem_val->name = (type == xmlrpc_vector) ? strdup(ATTR_VECTOR) : strdup(ATTR_SCALAR);

      if(id && *id) {
         xml_element_attr* attr_id = malloc(sizeof(xml_element_attr));
         if(attr_id) {
            attr_id->key = strdup(ATTR_ID);
            attr_id->val = strdup(id);
            Q_PushTail(&elem_val->attrs, attr_id);
         }
      }

      switch(type) {
         case xmlrpc_string:
            pAttrType = ATTR_STRING;
            simplestring_addn(&elem_val->text, XMLRPC_GetValueString(node), XMLRPC_GetValueStringLen(node));
            break;
         case xmlrpc_int:
            pAttrType = ATTR_INT;
            snprintf(buf, BUF_SIZE, "%i", XMLRPC_GetValueInt(node));
            simplestring_add(&elem_val->text, buf);
            break;
         case xmlrpc_boolean:
            pAttrType = ATTR_BOOLEAN;
            snprintf(buf, BUF_SIZE, "%i", XMLRPC_GetValueBoolean(node));
            simplestring_add(&elem_val->text, buf);
            break;
         case xmlrpc_double:
            pAttrType = ATTR_DOUBLE;
            snprintf(buf, BUF_SIZE, "%f", XMLRPC_GetValueDouble(node));
            simplestring_add(&elem_val->text, buf);
            break;
         case xmlrpc_datetime:
            pAttrType = ATTR_DATETIME;
            simplestring_add(&elem_val->text, XMLRPC_GetValueDateTime_ISO8601(node));
            break;
         case xmlrpc_base64:
            {
               struct buffer_st buf;
               pAttrType = ATTR_BASE64;
               base64_encode_xmlrpc(&buf, XMLRPC_GetValueBase64(node), XMLRPC_GetValueStringLen(node));
               simplestring_addn(&elem_val->text, buf.data, buf.offset );
               buffer_delete(&buf);
            }
            break;
         case xmlrpc_vector:
            {
               XMLRPC_VECTOR_TYPE my_type = XMLRPC_GetVectorType(node);
               XMLRPC_VALUE xIter = XMLRPC_VectorRewind(node);

               switch(my_type) {
                  case xmlrpc_vector_array:
                     pAttrType = ATTR_ARRAY;
                     break;
                  case xmlrpc_vector_mixed:
                     pAttrType = ATTR_MIXED;
                     break;
                  case xmlrpc_vector_struct:
                     pAttrType = ATTR_STRUCT;
                     break;
                  default:
                     break;
               }

               /* recurse through sub-elements */
               while( xIter ) {
                  xml_element* next_el = DANDARPC_to_xml_element_worker(request, xIter);
                  if(next_el) {
                     Q_PushTail(&elem_val->children, next_el);
                  }
                  xIter = XMLRPC_VectorNext(node);
               }
            }
            break;
         default:
            break;
      }
      if(pAttrType && attr_type && !bNoAddType) {
         attr_type->val = strdup(pAttrType);
      }
      root = elem_val;
   }
   return root;
}

xml_element* DANDARPC_VALUE_to_xml_element(XMLRPC_VALUE node) {
   return DANDARPC_to_xml_element_worker(NULL, node);
}

xml_element* DANDARPC_REQUEST_to_xml_element(XMLRPC_REQUEST request) {
   xml_element* wrapper = NULL;
   xml_element* root = NULL;
   if(request) {
      XMLRPC_REQUEST_TYPE request_type = XMLRPC_RequestGetRequestType(request);
      const char* pStr = NULL;
      xml_element_attr* version = malloc(sizeof(xml_element_attr));
      version->key = strdup(ATTR_VERSION);
      version->val = strdup(VAL_VERSION_0_9);
      
      wrapper = xml_elem_new();

      if(request_type == xmlrpc_request_response) {
         pStr = ELEM_METHODRESPONSE;
      }
      else if(request_type == xmlrpc_request_call) {
         pStr = ELEM_METHODCALL;
      }
      if(pStr) {
         wrapper->name = strdup(pStr);
      }

      root = xml_elem_new();
      root->name = strdup(ELEM_ROOT);
      Q_PushTail(&root->attrs, version);
      Q_PushTail(&root->children, wrapper);

      pStr = XMLRPC_RequestGetMethodName(request);

      if(pStr) {
         xml_element* method = xml_elem_new();
         method->name = strdup(ELEM_METHODNAME);
         simplestring_add(&method->text, pStr);
         Q_PushTail(&wrapper->children, method);
      }
      Q_PushTail(&wrapper->children, 
                 DANDARPC_to_xml_element_worker(request, XMLRPC_RequestGetData(request)));
   }
   return root;
}

