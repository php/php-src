/*
  This file is part of libXMLRPC - a C library for xml-encoded function calls.

  Author: Dan Libby (dan@libby.com)
  Epinions.com may be contacted at feedback@epinions-inc.com
*/

/*
  Copyright 2001 Epinions, Inc.

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


/****h* ABOUT/xmlrpc_introspection
 * AUTHOR
 *   Dan Libby, aka danda  (dan@libby.com)
 * HISTORY
 *   $Log$
 *   Revision 1.4  2003/12/16 21:00:21  sniper
 *   Fix some compile warnings (patch by Joe Orton)
 *
 *   Revision 1.3  2002/07/05 04:43:53  danda
 *   merged in updates from SF project.  bring php repository up to date with xmlrpc-epi version 0.51
 *
 *   Revision 1.9  2001/09/29 21:58:05  danda
 *   adding cvs log to history section
 *
 *   4/10/2001 -- danda -- initial introspection support
 * TODO
 * NOTES
 *******/


#ifdef _WIN32
#include "xmlrpc_win32.h"
#endif
#include "queue.h"
#include "xmlrpc.h"
#include "xmlrpc_private.h"
#include "xmlrpc_introspection_private.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>


/* forward declarations for static (non public, non api) funcs */
static XMLRPC_VALUE xi_system_describe_methods_cb(XMLRPC_SERVER server, XMLRPC_REQUEST input, void* userData);
static XMLRPC_VALUE xi_system_list_methods_cb(XMLRPC_SERVER server, XMLRPC_REQUEST input, void* userData);
static XMLRPC_VALUE xi_system_method_signature_cb(XMLRPC_SERVER server, XMLRPC_REQUEST input, void* userData);
static XMLRPC_VALUE xi_system_method_help_cb(XMLRPC_SERVER server, XMLRPC_REQUEST input, void* userData);


/*-**********************************
* Introspection Callbacks (methods) *
************************************/

/* iterates through a list of structs and finds the one with key "name" matching
 * needle.  slow, would benefit from a struct key hash.
 */
static inline XMLRPC_VALUE find_named_value(XMLRPC_VALUE list, const char* needle) {
   XMLRPC_VALUE xIter = XMLRPC_VectorRewind(list);
   while(xIter) {
      const char* name = XMLRPC_VectorGetStringWithID(xIter, xi_token_name);
      if(name && !strcmp(name, needle)) {
         return xIter;
      }
      xIter = XMLRPC_VectorNext(list);
   }
   return NULL;
}


/* iterates through docs callbacks and calls any that have not yet been called */
static void check_docs_loaded(XMLRPC_SERVER server, void* userData) {
   if(server) {
      q_iter qi = Q_Iter_Head_F(&server->docslist);
      while( qi ) {
         doc_method* dm = Q_Iter_Get_F(qi);
         if(dm && !dm->b_called) {
            dm->method(server, userData);
            dm->b_called = 1;
         }
         qi = Q_Iter_Next_F(qi);
      }
   }
}


/* utility function for xi_system_describe_methods_cb */
static inline void describe_method(XMLRPC_SERVER server, XMLRPC_VALUE vector, const char* method) {
   if(method) {
      server_method* sm = find_method(server, method);
      if(sm) {
         XMLRPC_AddValueToVector(vector, sm->desc);
      }
   }
}



/* system.describeMethods() callback */
static XMLRPC_VALUE xi_system_describe_methods_cb(XMLRPC_SERVER server, XMLRPC_REQUEST input, void* userData) {
   XMLRPC_VALUE xParams = XMLRPC_VectorRewind(XMLRPC_RequestGetData(input));
   XMLRPC_VALUE xResponse = XMLRPC_CreateVector(NULL, xmlrpc_vector_struct);
   XMLRPC_VALUE xMethodList = XMLRPC_CreateVector("methodList", xmlrpc_vector_array);
   XMLRPC_VALUE xTypeList = NULL;
   int bAll = 1;

   /* lazy loading of introspection data */
   check_docs_loaded(server, userData);

   xTypeList = XMLRPC_VectorGetValueWithID(server->xIntrospection, "typeList");

   XMLRPC_AddValueToVector(xResponse, xTypeList);
   XMLRPC_AddValueToVector(xResponse, xMethodList);

   /* check if we have any param */
   if(xParams) {
      /* check if string or vector (1 or n) */
      XMLRPC_VALUE_TYPE type = XMLRPC_GetValueType(xParams);
      if(type == xmlrpc_string) {
         /* just one.  spit it out. */
         describe_method(server, xMethodList, XMLRPC_GetValueString(xParams));
         bAll = 0;
      }
      else if(type == xmlrpc_vector) {
         /* multiple.  spit all out */
         XMLRPC_VALUE xIter = XMLRPC_VectorRewind(xParams);
         while(xIter) {
            describe_method(server, xMethodList, XMLRPC_GetValueString(xIter));
            xIter = XMLRPC_VectorNext(xParams);
         }
         bAll = 0;
      }
   }

   /* otherwise, default to sending all methods */
   if(bAll) {
      q_iter qi = Q_Iter_Head_F(&server->methodlist);
      while( qi ) {
         server_method* sm = Q_Iter_Get_F(qi);
         if(sm) {
            XMLRPC_AddValueToVector(xMethodList, sm->desc);
         }
         qi = Q_Iter_Next_F(qi);
      }
   }

   return xResponse;
}

/* this complies with system.listMethods as defined at http://xmlrpc.usefulinc.com/doc/reserved.html */
static XMLRPC_VALUE xi_system_list_methods_cb(XMLRPC_SERVER server, XMLRPC_REQUEST input, void* userData) {
   XMLRPC_VALUE xResponse = XMLRPC_CreateVector(NULL, xmlrpc_vector_array);

   q_iter qi = Q_Iter_Head_F(&server->methodlist);
   while( qi ) {
      server_method* sm = Q_Iter_Get_F(qi);
      if(sm) {
         XMLRPC_VectorAppendString(xResponse, 0, sm->name, 0);
      }
      qi = Q_Iter_Next_F(qi);
   }
   return xResponse;
}

/* this complies with system.methodSignature as defined at
 * http://xmlrpc.usefulinc.com/doc/sysmethodsig.html
 */
static XMLRPC_VALUE xi_system_method_signature_cb(XMLRPC_SERVER server, XMLRPC_REQUEST input, void* userData) {
   const char* method = XMLRPC_GetValueString(XMLRPC_VectorRewind(XMLRPC_RequestGetData(input)));
   XMLRPC_VALUE xResponse = NULL;

   /* lazy loading of introspection data */
   check_docs_loaded(server, userData);

   if(method) {
      server_method* sm = find_method(server, method);
      if(sm && sm->desc) {
         XMLRPC_VALUE xTypesArray = XMLRPC_CreateVector(NULL, xmlrpc_vector_array);
         XMLRPC_VALUE xIter, xParams, xSig, xSigIter;
         const char* type;

         /* array of possible signatures.  */
         xResponse = XMLRPC_CreateVector(NULL, xmlrpc_vector_array);

         /* find first signature */
         xSig = XMLRPC_VectorGetValueWithID(sm->desc, xi_token_signatures);
         xSigIter = XMLRPC_VectorRewind( xSig );

         /* iterate through sigs */
         while(xSigIter) {
            /* first type is the return value */
            type = XMLRPC_VectorGetStringWithID(XMLRPC_VectorRewind(
                                                 XMLRPC_VectorGetValueWithID(xSigIter, xi_token_returns)),
                                                xi_token_type);
            XMLRPC_AddValueToVector(xTypesArray,
                                    XMLRPC_CreateValueString(NULL,
                                                             type ? type : type_to_str(xmlrpc_none, 0),
                                    0));

            /* the rest are parameters */
            xParams = XMLRPC_VectorGetValueWithID(xSigIter, xi_token_params);
            xIter = XMLRPC_VectorRewind(xParams);

            /* iter through params, adding to types array */
            while(xIter) {
               XMLRPC_AddValueToVector(xTypesArray,
                                       XMLRPC_CreateValueString(NULL,
                                                                XMLRPC_VectorGetStringWithID(xIter, xi_token_type),
                                                                0));
               xIter = XMLRPC_VectorNext(xParams);
            }

            /* add types for this signature */
            XMLRPC_AddValueToVector(xResponse, xTypesArray);

            xSigIter = XMLRPC_VectorNext( xSig );
         }
      }
   }

   return xResponse;
}

/* this complies with system.methodHelp as defined at
 * http://xmlrpc.usefulinc.com/doc/sysmethhelp.html
 */
static XMLRPC_VALUE xi_system_method_help_cb(XMLRPC_SERVER server, XMLRPC_REQUEST input, void* userData) {
   const char* method = XMLRPC_GetValueString(XMLRPC_VectorRewind(XMLRPC_RequestGetData(input)));
   XMLRPC_VALUE xResponse = NULL;

   /* lazy loading of introspection data */
   check_docs_loaded(server, userData);

   if(method) {
      server_method* sm = find_method(server, method);
      if(sm && sm->desc) {
         const char* help = XMLRPC_VectorGetStringWithID(sm->desc, xi_token_purpose);

         /* returns a documentation string, or empty string */
         xResponse = XMLRPC_CreateValueString(NULL, help ? help : xi_token_empty, 0);
      }
   }

   return xResponse;
}

/*-**************************************
* End Introspection Callbacks (methods) *
****************************************/


/*-************************
* Introspection Utilities *
**************************/

/* performs registration of introspection methods */
void xi_register_system_methods(XMLRPC_SERVER server) {
   XMLRPC_ServerRegisterMethod(server, xi_token_system_list_methods, xi_system_list_methods_cb);
   XMLRPC_ServerRegisterMethod(server, xi_token_system_method_help, xi_system_method_help_cb);
   XMLRPC_ServerRegisterMethod(server, xi_token_system_method_signature, xi_system_method_signature_cb);
   XMLRPC_ServerRegisterMethod(server, xi_token_system_describe_methods, xi_system_describe_methods_cb);
}

/* describe a value (param, return, type) */
static XMLRPC_VALUE describeValue_worker(const char* type, const char* id, const char* desc, int optional, const char* default_val, XMLRPC_VALUE sub_params) {
   XMLRPC_VALUE xParam = NULL;
   if(id || desc) {
      xParam = XMLRPC_CreateVector(NULL, xmlrpc_vector_struct);
      XMLRPC_VectorAppendString(xParam, xi_token_name, id, 0);
      XMLRPC_VectorAppendString(xParam, xi_token_type, type, 0);
      XMLRPC_VectorAppendString(xParam, xi_token_description, desc, 0);
      if(optional != 2) {
         XMLRPC_VectorAppendInt(xParam, xi_token_optional, optional);
      }
      if(optional == 1 && default_val) {
         XMLRPC_VectorAppendString(xParam, xi_token_default, default_val, 0);
      }
      XMLRPC_AddValueToVector(xParam, sub_params);
   }
   return xParam;
}


/* convert an xml tree conforming to spec <url tbd> to  XMLRPC_VALUE
 * suitable for use with XMLRPC_ServerAddIntrospectionData
 */
XMLRPC_VALUE xml_element_to_method_description(xml_element* el, XMLRPC_ERROR err) {
   XMLRPC_VALUE xReturn = NULL;

   if(el->name) {
      const char* name = NULL;
      const char* type = NULL;
      const char* basetype = NULL;
      const char* desc = NULL;
      const char* def = NULL;
      int optional = 0;
      xml_element_attr* attr_iter = Q_Head(&el->attrs);

      /* grab element attributes up front to save redundant while loops */
      while(attr_iter) {
         if(!strcmp(attr_iter->key, "name")) {
            name = attr_iter->val;
         }
         else if(!strcmp(attr_iter->key, "type")) {
            type = attr_iter->val;
         }
         else if(!strcmp(attr_iter->key, "basetype")) {
            basetype = attr_iter->val;
         }
         else if(!strcmp(attr_iter->key, "desc")) {
            desc = attr_iter->val;
         }
         else if(!strcmp(attr_iter->key, "optional")) {
            if(attr_iter->val && !strcmp(attr_iter->val, "yes")) {
               optional = 1;
            }
         }
         else if(!strcmp(attr_iter->key, "default")) {
            def = attr_iter->val;
         }
         attr_iter = Q_Next(&el->attrs);
      }

      /* value and typeDescription behave about the same */
      if(!strcmp(el->name, "value") || !strcmp(el->name, "typeDescription")) {
         XMLRPC_VALUE xSubList = NULL;
         const char* ptype = !strcmp(el->name, "value") ? type : basetype;
         if(ptype) {
            if(Q_Size(&el->children) &&
               (!strcmp(ptype, "array") || !strcmp(ptype, "struct") || !strcmp(ptype, "mixed"))) {
               xSubList = XMLRPC_CreateVector("member", xmlrpc_vector_array);

               if(xSubList) {
                  xml_element* elem_iter = Q_Head(&el->children);
                  while(elem_iter) {
                     XMLRPC_AddValueToVector(xSubList,
                                             xml_element_to_method_description(elem_iter, err));
                     elem_iter = Q_Next(&el->children);
                  }
               }
            }
            xReturn = describeValue_worker(ptype, name, (desc ? desc : (xSubList ? NULL : el->text.str)), optional, def, xSubList);
         }
      }

      /* these three kids are about equivalent */
      else if(!strcmp(el->name, "params") ||
              !strcmp(el->name, "returns") ||
              !strcmp(el->name, "signature")) {
         if(Q_Size(&el->children)) {
            xml_element* elem_iter = Q_Head(&el->children);
            xReturn = XMLRPC_CreateVector(!strcmp(el->name, "signature") ? NULL : el->name, xmlrpc_vector_struct);


            while(elem_iter) {
               XMLRPC_AddValueToVector(xReturn,
                                       xml_element_to_method_description(elem_iter, err));
               elem_iter = Q_Next(&el->children);
            }
         }
      }


      else if(!strcmp(el->name, "methodDescription")) {
         xml_element* elem_iter = Q_Head(&el->children);
         xReturn = XMLRPC_CreateVector(NULL, xmlrpc_vector_struct);

         XMLRPC_VectorAppendString(xReturn, xi_token_name, name, 0);

         while(elem_iter) {
            XMLRPC_AddValueToVector(xReturn,
                                    xml_element_to_method_description(elem_iter, err));
            elem_iter = Q_Next(&el->children);
         }
      }

      /* items are slightly special */
      else if(!strcmp(el->name, "item")) {
         xReturn = XMLRPC_CreateValueString(name, el->text.str, el->text.len);
      }

      /* sure.  we'll let any ol element with children through */
      else if(Q_Size(&el->children)) {
         xml_element* elem_iter = Q_Head(&el->children);
         xReturn = XMLRPC_CreateVector(el->name, xmlrpc_vector_mixed);

         while(elem_iter) {
            XMLRPC_AddValueToVector(xReturn,
                                    xml_element_to_method_description(elem_iter, err));
            elem_iter = Q_Next(&el->children);
         }
      }

      /* or anything at all really, so long as its got some text.
       * no reason being all snotty about a spec, right?
       */
      else if(el->name && el->text.len) {
         xReturn = XMLRPC_CreateValueString(el->name, el->text.str, el->text.len);
      }
   }

   return xReturn;
}

/*-****************************
* End Introspection Utilities *
******************************/



/*-******************
* Introspection API *
********************/


/****f* VALUE/XMLRPC_IntrospectionCreateDescription
 * NAME
 *   XMLRPC_IntrospectionCreateDescription
 * SYNOPSIS
 *   XMLRPC_VALUE XMLRPC_IntrospectionCreateDescription(const char* xml, XMLRPC_ERROR err)
 * FUNCTION
 *   converts raw xml describing types and methods into an
 *   XMLRPC_VALUE suitable for use with XMLRPC_ServerAddIntrospectionData()
 * INPUTS
 *   xml - xml data conforming to introspection spec at <url tbd>
 *   err - optional pointer to error struct. filled in if error occurs and not NULL.
 * RESULT
 *   XMLRPC_VALUE - newly created value, or NULL if fatal error.
 * BUGS
 *   Currently does little or no validation of xml.
 *   Only parse errors are currently reported in err, not structural errors.
 * SEE ALSO
 *   XMLRPC_ServerAddIntrospectionData ()
 * SOURCE
 */
XMLRPC_VALUE XMLRPC_IntrospectionCreateDescription(const char* xml, XMLRPC_ERROR err) {
   XMLRPC_VALUE xReturn = NULL;
   xml_element* root = xml_elem_parse_buf(xml, 0, 0, err ? &err->xml_elem_error : NULL);

   if(root) {
      xReturn = xml_element_to_method_description(root, err);

      xml_elem_free(root);
   }

   return xReturn;

}
/*******/


/****f* SERVER/XMLRPC_ServerAddIntrospectionData
 * NAME
 *   XMLRPC_ServerAddIntrospectionData
 * SYNOPSIS
 *   int XMLRPC_ServerAddIntrospectionData(XMLRPC_SERVER server, XMLRPC_VALUE desc)
 * FUNCTION
 *   updates server with additional introspection data
 * INPUTS
 *   server - target server
 *   desc - introspection data, should be a struct generated by
 *          XMLRPC_IntrospectionCreateDescription ()
 * RESULT
 *   int - 1 if success, else 0
 * NOTES
 *  - function will fail if neither typeList nor methodList key is present in struct.
 *  - if method or type already exists, it will be replaced.
 *  - desc is never freed by the server.  caller is responsible for cleanup.
 * BUGS
 *   - horribly slow lookups. prime candidate for hash improvements.
 *   - uglier and more complex than I like to see for API functions.
 * SEE ALSO
 *   XMLRPC_ServerAddIntrospectionData ()
 *   XMLRPC_ServerRegisterIntrospectionCallback ()
 *   XMLRPC_CleanupValue ()
 * SOURCE
 */
int XMLRPC_ServerAddIntrospectionData(XMLRPC_SERVER server, XMLRPC_VALUE desc) {
   int bSuccess = 0;
   if(server && desc) {
      XMLRPC_VALUE xNewTypes = XMLRPC_VectorGetValueWithID(desc, "typeList");
      XMLRPC_VALUE xNewMethods = XMLRPC_VectorGetValueWithID(desc, "methodList");
      XMLRPC_VALUE xServerTypes = XMLRPC_VectorGetValueWithID(server->xIntrospection, "typeList");

      if(xNewMethods) {
         XMLRPC_VALUE xMethod = XMLRPC_VectorRewind(xNewMethods);

         while(xMethod) {
            const char* name = XMLRPC_VectorGetStringWithID(xMethod, xi_token_name);
            server_method* sm = find_method(server, name);

            if(sm) {
               if(sm->desc) {
                  XMLRPC_CleanupValue(sm->desc);
               }
               sm->desc = XMLRPC_CopyValue(xMethod);
               bSuccess = 1;
            }

            xMethod = XMLRPC_VectorNext(xNewMethods);
         }
      }
      if(xNewTypes) {
         if(!xServerTypes) {
            if(!server->xIntrospection) {
               server->xIntrospection = XMLRPC_CreateVector(NULL, xmlrpc_vector_struct);
            }

            XMLRPC_AddValueToVector(server->xIntrospection, xNewTypes);
            bSuccess = 1;
         }
         else {
            XMLRPC_VALUE xIter = XMLRPC_VectorRewind(xNewTypes);
            while(xIter) {
               /* get rid of old values */
               XMLRPC_VALUE xPrev = find_named_value(xServerTypes, XMLRPC_VectorGetStringWithID(xIter, xi_token_name));
               if(xPrev) {
                  XMLRPC_VectorRemoveValue(xServerTypes, xPrev);
               }
               XMLRPC_AddValueToVector(xServerTypes, xIter);
               bSuccess = 1;
               xIter = XMLRPC_VectorNext(xNewTypes);
            }
         }
      }
   }
   return bSuccess;
}
/*******/


/****f* SERVER/XMLRPC_ServerRegisterIntrospectionCallback
 * NAME
 *   XMLRPC_ServerRegisterIntrospectionCallback
 * SYNOPSIS
 *   int XMLRPC_ServerRegisterIntrospectionCallback(XMLRPC_SERVER server, XMLRPC_IntrospectionCallback cb)
 * FUNCTION
 *   registers a callback for lazy generation of introspection data
 * INPUTS
 *   server - target server
 *   cb - callback that will generate introspection data
 * RESULT
 *   int - 1 if success, else 0
 * NOTES
 *   parsing xml and generating introspection data is fairly expensive, thus a
 *   server may wish to wait until this data is actually requested before generating
 *   it. Any number of callbacks may be registered at any time.  A given callback
 *   will only ever be called once, the first time an introspection request is
 *   processed after the time of callback registration.
 * SEE ALSO
 *   XMLRPC_ServerAddIntrospectionData ()
 *   XMLRPC_IntrospectionCreateDescription ()
 * SOURCE
 */
int XMLRPC_ServerRegisterIntrospectionCallback(XMLRPC_SERVER server, XMLRPC_IntrospectionCallback cb) {
   int bSuccess = 0;
   if(server && cb) {

      doc_method* dm = calloc(1, sizeof(doc_method));

      if(dm) {
         dm->method = cb;
         dm->b_called = 0;

         if(Q_PushTail(&server->docslist, dm)) {
            bSuccess = 1;
         }
         else {
            my_free(dm);
         }
      }
   }
   return 0;
}
/*******/

/*-**********************
* End Introspection API *
************************/



