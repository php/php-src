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


static const char rcsid[] = "#(@) $Id$";

#include "php.h"
#include "main/snprintf.h"
#ifdef _WIN32
#include "xmlrpc_win32.h"
#endif
#include <string.h>
#include <stdlib.h>
#include "xml_to_xmlrpc.h"
#include "base64.h"

/* list of tokens used in vocab */
#define ELEM_ARRAY          "array"
#define ELEM_BASE64         "base64"
#define ELEM_BOOLEAN        "boolean"
#define ELEM_DATA           "data"
#define ELEM_DATETIME       "dateTime.iso8601"
#define ELEM_DOUBLE         "double"
#define ELEM_FAULT          "fault"
#define ELEM_FAULTCODE      "faultCode"
#define ELEM_FAULTSTRING    "faultString"
#define ELEM_I4             "i4"
#define ELEM_INT            "int"
#define ELEM_MEMBER         "member"
#define ELEM_METHODCALL     "methodCall"
#define ELEM_METHODNAME     "methodName"
#define ELEM_METHODRESPONSE "methodResponse"
#define ELEM_NAME           "name"
#define ELEM_PARAM          "param"
#define ELEM_PARAMS         "params"
#define ELEM_STRING         "string"
#define ELEM_STRUCT         "struct"
#define ELEM_VALUE          "value"


XMLRPC_VALUE xml_element_to_XMLRPC_REQUEST_worker(XMLRPC_REQUEST request, XMLRPC_VALUE parent_vector, XMLRPC_VALUE current_val, xml_element* el) {
   if (!current_val) {
      /* This should only be the case for the first element */
      current_val = XMLRPC_CreateValueEmpty();
   }

	if (el->name) {

      /* first, deal with the crazy/stupid fault format */
      if (!strcmp(el->name, ELEM_FAULT)) {
			xml_element* fault_value = (xml_element*)Q_Head(&el->children);
			XMLRPC_SetIsVector(current_val, xmlrpc_vector_struct);

         if(fault_value) {
            xml_element* fault_struct = (xml_element*)Q_Head(&fault_value->children);
            if(fault_struct) {
               xml_element* iter = (xml_element*)Q_Head(&fault_struct->children);

               while (iter) {
                  XMLRPC_VALUE xNextVal = XMLRPC_CreateValueEmpty();
                  xml_element_to_XMLRPC_REQUEST_worker(request, current_val, xNextVal, iter);
                  XMLRPC_AddValueToVector(current_val, xNextVal);
                  iter = (xml_element*)Q_Next(&fault_struct->children);
               }
            }
         }
      }
		else if (!strcmp(el->name, ELEM_DATA)	/* should be ELEM_ARRAY, but there is an extra level. weird */
			 || (!strcmp(el->name, ELEM_PARAMS) && 
				  (XMLRPC_RequestGetRequestType(request) == xmlrpc_request_call)) ) {		/* this "PARAMS" concept is silly.  dave?! */
         xml_element* iter = (xml_element*)Q_Head(&el->children);
         XMLRPC_SetIsVector(current_val, xmlrpc_vector_array);

         while (iter) {
            XMLRPC_VALUE xNextVal = XMLRPC_CreateValueEmpty();
            xml_element_to_XMLRPC_REQUEST_worker(request, current_val, xNextVal, iter);
            XMLRPC_AddValueToVector(current_val, xNextVal);
            iter = (xml_element*)Q_Next(&el->children);
         }
		}
		else if (!strcmp(el->name, ELEM_STRUCT)) {
         xml_element* iter = (xml_element*)Q_Head(&el->children);
         XMLRPC_SetIsVector(current_val, xmlrpc_vector_struct);

         while ( iter ) {
            XMLRPC_VALUE xNextVal = XMLRPC_CreateValueEmpty();
            xml_element_to_XMLRPC_REQUEST_worker(request, current_val, xNextVal, iter);
            XMLRPC_AddValueToVector(current_val, xNextVal);
            iter = (xml_element*)Q_Next(&el->children);
         }
		}
		else if (!strcmp(el->name, ELEM_STRING) || 
                 (!strcmp(el->name, ELEM_VALUE) && Q_Size(&el->children) == 0)) {
         XMLRPC_SetValueString(current_val, el->text.str, el->text.len);
		}
		else if (!strcmp(el->name, ELEM_NAME)) {
         XMLRPC_SetValueID_Case(current_val, el->text.str, 0, xmlrpc_case_exact);
		}
		else if (!strcmp(el->name, ELEM_INT) || !strcmp(el->name, ELEM_I4)) {
         XMLRPC_SetValueInt(current_val, atoi(el->text.str));
		}
		else if (!strcmp(el->name, ELEM_BOOLEAN)) {
         XMLRPC_SetValueBoolean(current_val, atoi(el->text.str));
		}
		else if (!strcmp(el->name, ELEM_DOUBLE)) {
         XMLRPC_SetValueDouble(current_val, atof(el->text.str));
		}
		else if (!strcmp(el->name, ELEM_DATETIME)) {
         XMLRPC_SetValueDateTime_ISO8601(current_val, el->text.str);
		}
		else if (!strcmp(el->name, ELEM_BASE64)) {
         struct buffer_st buf;
         base64_decode_xmlrpc(&buf, el->text.str, el->text.len);
         XMLRPC_SetValueBase64(current_val, buf.data, buf.offset);
         buffer_delete(&buf);
		}
		else {
         xml_element* iter;

         if (!strcmp(el->name, ELEM_METHODCALL)) {
            if (request) {
               XMLRPC_RequestSetRequestType(request, xmlrpc_request_call);
            }
			}
			else if (!strcmp(el->name, ELEM_METHODRESPONSE)) {
            if (request) {
               XMLRPC_RequestSetRequestType(request, xmlrpc_request_response);
            }
			}
			else if (!strcmp(el->name, ELEM_METHODNAME)) {
            if (request) {
               XMLRPC_RequestSetMethodName(request, el->text.str);
            }
         }

         iter = (xml_element*)Q_Head(&el->children);
         while ( iter ) {
            xml_element_to_XMLRPC_REQUEST_worker(request, parent_vector, 
                                                 current_val, iter);
            iter = (xml_element*)Q_Next(&el->children);
         }
      }
   }
   return current_val;
}

XMLRPC_VALUE xml_element_to_XMLRPC_VALUE(xml_element* el)
{
   return xml_element_to_XMLRPC_REQUEST_worker(NULL, NULL, NULL, el);
}

XMLRPC_VALUE xml_element_to_XMLRPC_REQUEST(XMLRPC_REQUEST request, xml_element* el)
{
   if (request) {
      return XMLRPC_RequestSetData(request, xml_element_to_XMLRPC_REQUEST_worker(request, NULL, NULL, el));
   }
   return NULL;
}

xml_element* XMLRPC_to_xml_element_worker(XMLRPC_VALUE current_vector, XMLRPC_VALUE node, 
                                          XMLRPC_REQUEST_TYPE request_type, int depth) {
#define BUF_SIZE 512
   xml_element* root = NULL;
   if (node) {
      char buf[BUF_SIZE];
      XMLRPC_VALUE_TYPE type = XMLRPC_GetValueType(node);
      XMLRPC_VECTOR_TYPE vtype = XMLRPC_GetVectorType(node);
      xml_element* elem_val = xml_elem_new();

      /* special case for when root element is not an array */
      if (depth == 0 && 
          !(type == xmlrpc_vector && 
            vtype == xmlrpc_vector_array && 
            request_type == xmlrpc_request_call) ) {
         int bIsFault = (vtype == xmlrpc_vector_struct && XMLRPC_VectorGetValueWithID(node, ELEM_FAULTCODE));

         xml_element* next_el = XMLRPC_to_xml_element_worker(NULL, node, request_type, depth + 1);
         if (next_el) {
            Q_PushTail(&elem_val->children, next_el);
         }
         elem_val->name = strdup(bIsFault ? ELEM_FAULT : ELEM_PARAMS);
		}
		else {
         switch (type) {
			case xmlrpc_empty: /*  treat null value as empty string in xmlrpc. */
         case xmlrpc_string:
            elem_val->name = strdup(ELEM_STRING);
            simplestring_addn(&elem_val->text, XMLRPC_GetValueString(node), XMLRPC_GetValueStringLen(node));
            break;
         case xmlrpc_int:
            elem_val->name = strdup(ELEM_INT);
            snprintf(buf, BUF_SIZE, "%i", XMLRPC_GetValueInt(node));
            simplestring_add(&elem_val->text, buf);
            break;
         case xmlrpc_boolean:
            elem_val->name = strdup(ELEM_BOOLEAN);
            snprintf(buf, BUF_SIZE, "%i", XMLRPC_GetValueBoolean(node));
            simplestring_add(&elem_val->text, buf);
            break;
         case xmlrpc_double:
            {
                TSRMLS_FETCH();
                elem_val->name = strdup(ELEM_DOUBLE);
                ap_php_snprintf(buf, BUF_SIZE, "%.*G", (int) EG(precision), XMLRPC_GetValueDouble(node));
                simplestring_add(&elem_val->text, buf);
            }
            break;
         case xmlrpc_datetime:
            elem_val->name = strdup(ELEM_DATETIME);
            simplestring_add(&elem_val->text, XMLRPC_GetValueDateTime_ISO8601(node));
            break;
         case xmlrpc_base64:
            {
               struct buffer_st buf;
               elem_val->name = strdup(ELEM_BASE64);
               base64_encode_xmlrpc(&buf, XMLRPC_GetValueBase64(node), XMLRPC_GetValueStringLen(node));
               simplestring_addn(&elem_val->text, buf.data, buf.offset );
               buffer_delete(&buf);
            }
            break;
         case xmlrpc_vector:
            {
               XMLRPC_VECTOR_TYPE my_type = XMLRPC_GetVectorType(node);
               XMLRPC_VALUE xIter = XMLRPC_VectorRewind(node);
               xml_element* root_vector_elem = elem_val;

               switch (my_type) {
               case xmlrpc_vector_array:
                  {
                      if(depth == 0) {
                         elem_val->name = strdup(ELEM_PARAMS);
                      }
                      else {
                         /* Hi my name is Dave and I like to make things as confusing
                          * as possible, thus I will throw in this 'data' element
                          * where it absolutely does not belong just so that people
                          * cannot code arrays and structs in a similar and straight
                          * forward manner. Have a good day.
                          *
                          * GRRRRRRRRR!
                          */
                         xml_element* data = xml_elem_new();
                         data->name = strdup(ELEM_DATA);
    
                         elem_val->name = strdup(ELEM_ARRAY);
                         Q_PushTail(&elem_val->children, data);
                         root_vector_elem = data;
                      }
                  }
                  break;
               case xmlrpc_vector_mixed:       /* not officially supported */
               case xmlrpc_vector_struct:
                  elem_val->name = strdup(ELEM_STRUCT);
                  break;
               default:
                  break;
               }

               /* recurse through sub-elements */
               while ( xIter ) {
                  xml_element* next_el = XMLRPC_to_xml_element_worker(node, xIter, request_type, depth + 1);
                  if (next_el) {
                     Q_PushTail(&root_vector_elem->children, next_el);
                  }
                  xIter = XMLRPC_VectorNext(node);
               }
            }
            break;
         default:
            break;
         }
      }

      {
         XMLRPC_VECTOR_TYPE vtype = XMLRPC_GetVectorType(current_vector);

         if (depth == 1) {
            xml_element* value = xml_elem_new();
            value->name = strdup(ELEM_VALUE);

            /* yet another hack for the "fault" crap */
            if (XMLRPC_VectorGetValueWithID(node, ELEM_FAULTCODE)) {
               root = value;
				}
				else {
               xml_element* param = xml_elem_new();
               param->name = strdup(ELEM_PARAM);

               Q_PushTail(&param->children, value);

               root = param;
            }
            Q_PushTail(&value->children, elem_val);
			}
			else if (vtype == xmlrpc_vector_struct || vtype == xmlrpc_vector_mixed) {
            xml_element* member = xml_elem_new();
            xml_element* name = xml_elem_new();
            xml_element* value = xml_elem_new();

            member->name = strdup(ELEM_MEMBER);
            name->name = strdup(ELEM_NAME);
            value->name = strdup(ELEM_VALUE);

            simplestring_add(&name->text, XMLRPC_GetValueID(node));

            Q_PushTail(&member->children, name);
            Q_PushTail(&member->children, value);
            Q_PushTail(&value->children, elem_val);

            root = member;
			}
			else if (vtype == xmlrpc_vector_array) {
            xml_element* value = xml_elem_new();

            value->name = strdup(ELEM_VALUE);

            Q_PushTail(&value->children, elem_val);

            root = value;
			}
			else if (vtype == xmlrpc_vector_none) {
            /* no parent.  non-op */
            root = elem_val;
			}
			else {
            xml_element* value = xml_elem_new();

            value->name = strdup(ELEM_VALUE);

            Q_PushTail(&value->children, elem_val);

            root = value;
         }
      }
   }
   return root;
}

xml_element* XMLRPC_VALUE_to_xml_element(XMLRPC_VALUE node) {
   return XMLRPC_to_xml_element_worker(NULL, node, xmlrpc_request_none, 0);
}

xml_element* XMLRPC_REQUEST_to_xml_element(XMLRPC_REQUEST request) {
   xml_element* wrapper = NULL;
   if (request) {
      const char* pStr = NULL;
      XMLRPC_REQUEST_TYPE request_type = XMLRPC_RequestGetRequestType(request);
      XMLRPC_VALUE xParams = XMLRPC_RequestGetData(request);

      wrapper = xml_elem_new();

      if (request_type == xmlrpc_request_call) {
         pStr = ELEM_METHODCALL;
		}
		else if (request_type == xmlrpc_request_response) {
         pStr = ELEM_METHODRESPONSE;
      }
      if (pStr) {
         wrapper->name = strdup(pStr);
      }

		if(request_type == xmlrpc_request_call) {
      pStr = XMLRPC_RequestGetMethodName(request);

      if (pStr) {
         xml_element* method = xml_elem_new();
         method->name = strdup(ELEM_METHODNAME);
         simplestring_add(&method->text, pStr);
         Q_PushTail(&wrapper->children, method);
      }
		}
      if (xParams) {
         Q_PushTail(&wrapper->children, 
                    XMLRPC_to_xml_element_worker(NULL, XMLRPC_RequestGetData(request), XMLRPC_RequestGetRequestType(request), 0));
		}
		else {
         /* Despite the spec, the xml-rpc list folk want me to send an empty params element */
         xml_element* params = xml_elem_new();
         params->name = strdup(ELEM_PARAMS);
         Q_PushTail(&wrapper->children, params);
      }
   }
   return wrapper;
}

