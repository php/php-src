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


/****h* ABOUT/system_methods
 * AUTHOR
 *   Dan Libby, aka danda  (dan@libby.com)
 * HISTORY
 *   $Log$
 *   Revision 1.7  2001/09/29 21:58:05  danda
 *   adding cvs log to history section
 *
 *   4/28/2001 -- danda -- adding system.multicall and separating out system methods.
 * TODO
 * NOTES
 *******/


#include "queue.h"
#include "xmlrpc.h"
#include "xmlrpc_private.h"
#include "xmlrpc_introspection_private.h"
#include "system_methods_private.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>


static const char* xsm_introspection_xml =
"<?xml version='1.0' ?>"

"<introspection version='1.0'>"
 "<typeList>"

 "<typeDescription name='system.value' basetype='struct' desc='description of a value'>"
   "<value type='string' name='name' optional='yes'>value identifier</value>"
   "<value type='string' name='type'>value&apos;s xmlrpc or user-defined type</value>"
   "<value type='string' name='description'>value&apos;s textual description</value> "
   "<value type='boolean' name='optional'>true if value is optional, else it is required</value> "
   "<value type='any' name='member' optional='yes'>a child of this element. n/a for scalar types</value> "
 "</typeDescription>"

 "<typeDescription name='system.valueList' basetype='array' desc='list of value descriptions'>"
   "<value type='system.value'/>"
 "</typeDescription>"

 "<typeDescription name='system.stringList' basetype='array' desc='list of strings'>"
   "<value type='string'/>"
 "</typeDescription>"


 "</typeList>"

 "<methodList>"

 "<!-- system.describeMethods -->"
 "<methodDescription name='system.describeMethods'>"
  "<author>Dan Libby</author>"
  "<purpose>fully describes the methods and types implemented by this XML-RPC server.</purpose>"
  "<version>1.1</version>"
  "<signatures>"
   "<signature>"
    "<params>"
     "<value type='array' name='methodList' optional='yes' desc='a list of methods to be described. if omitted, all are described.'>"
      "<value type='string'>a valid method name</value>"
     "</value>"
    "</params>"
    "<returns>"
     "<value type='struct' desc='contains methods list and types list'>"
      "<value type='array' name='methodList' desc='a list of methods'>"
       "<value type='struct' desc='representation of a single method'>"
        "<value type='string' name='name'>method name</value>"
        "<value type='string' name='version' optional='yes'>method version</value>"
        "<value type='string' name='author' optional='yes'>method author</value>"
        "<value type='string' name='purpose' optional='yes'>method purpose</value>"
        "<value type='array' name='signatures' desc='list of method signatures'>"
         "<value type='struct' desc='representation of a single signature'>"
          "<value type='system.valueList' name='params' optional='yes'>parameter list</value>"
          "<value type='system.valueList' name='returns' optional='yes'>return value list</value>"
         "</value>"
        "</value>"
        "<value type='system.stringList' name='bugs' optional='yes'>list of known bugs</value>"
        "<value type='system.stringList' name='errors' optional='yes'>list of possible errors and error codes</value>"
        "<value type='system.stringList' name='examples' optional='yes'>list of examples</value>"
        "<value type='system.stringList' name='history' optional='yes'>list of modifications</value>"
        "<value type='system.stringList' name='notes' optional='yes'>list of notes</value>"
        "<value type='system.stringList' name='see' optional='yes'>see also.  list of related methods</value>"
        "<value type='system.stringList' name='todo' optional='yes'>list of unimplemented features</value>"
       "</value>"
      "</value>"
      "<value type='array' name='typeList' desc='a list of type descriptions. Typically used for referencing complex types'>"
       "<value type='system.value'>a type description</value>"
      "</value>"
     "</value>"
    "</returns>"
   "</signature>"
  "</signatures>"
  "<see>"
   "<item name='system.listMethods' />"
   "<item name='system.methodSignature' />"
   "<item name='system.methodHelp' />"
  "</see>"
  "<example/>"
  "<error/>"
  "<note/>"
  "<bug/>"
  "<todo/>"
 "</methodDescription>"

 "<!-- system.listMethods -->"
 "<methodDescription name='system.listMethods'>"
  "<author>Dan Libby</author>"
  "<purpose>enumerates the methods implemented by this XML-RPC server.</purpose>"
  "<version>1.0</version>"
  "<signatures>"
   "<signature>"
    "<returns>"
     "<value type='array' desc='an array of strings'>"
      "<value type='string'>name of a method implemented by the server.</value>"
     "</value>"
    "</returns>"
   "</signature>"
  "</signatures>"
  "<see>"
   "<item name='system.describeMethods' />"
   "<item name='system.methodSignature' />"
   "<item name='system.methodHelp' />"
  "</see>"
  "<example/>"
  "<error/>"
  "<note/>"
  "<bug/>"
  "<todo/>"
 "</methodDescription>"

 "<!-- system.methodHelp -->"
 "<methodDescription name='system.methodHelp'>"
  "<author>Dan Libby</author>"
  "<purpose>provides documentation string for a single method</purpose>"
  "<version>1.0</version>"
  "<signatures>"
   "<signature>"
    "<params>"
     "<value type='string' name='methodName'>name of the method for which documentation is desired</value>"
    "</params>"
    "<returns>"
     "<value type='string'>help text if defined for the method passed, otherwise an empty string</value>"
    "</returns>"
   "</signature>"
  "</signatures>"
  "<see>"
   "<item name='system.listMethods' />"
   "<item name='system.methodSignature' />"
   "<item name='system.methodHelp' />"
  "</see>"
  "<example/>"
  "<error/>"
  "<note/>"
  "<bug/>"
  "<todo/>"
 "</methodDescription>"

 "<!-- system.methodSignature -->"
 "<methodDescription name='system.methodSignature'>"
  "<author>Dan Libby</author>"
  "<purpose>provides 1 or more signatures for a single method</purpose>"
  "<version>1.0</version>"
  "<signatures>"
   "<signature>"
    "<params>"
     "<value type='string' name='methodName'>name of the method for which documentation is desired</value>"
    "</params>"
    "<returns>"
     "<value type='array' desc='a list of arrays, each representing a signature'>"
      "<value type='array' desc='a list of strings. the first element represents the method return value. subsequent elements represent parameters.'>"
       "<value type='string'>a string indicating the xmlrpc type of a value. one of: string, int, double, base64, datetime, array, struct</value>"
      "</value>"
     "</value>"
    "</returns>"
   "</signature>"
  "</signatures>"
  "<see>"
   "<item name='system.listMethods' />"
   "<item name='system.methodHelp' />"
   "<item name='system.describeMethods' />"
  "</see>"
  "<example/>"
  "<error/>"
  "<note/>"
  "<bug/>"
  "<todo/>"
 "</methodDescription>"

 "<!-- system.multiCall -->"
 "<methodDescription name='system.multiCall'>"
  "<author>Dan Libby</author>"
  "<purpose>executes multiple methods in sequence and returns the results</purpose>"
  "<version>1.0</version>"
  "<signatures>"
   "<signature>"
    "<params>"
     "<value type='array' name='methodList' desc='an array of method call structs'>"
      "<value type='struct' desc='a struct representing a single method call'>"
       "<value type='string' name='methodName' desc='name of the method to be executed'/>"
       "<value type='array' name='params' desc='an array representing the params to a method. sub-elements should match method signature'/>"
      "</value>"
     "</value>"
    "</params>"
    "<returns>"
     "<value type='array' desc='an array of method responses'>"
      "<value type='array' desc='an array containing a single value, which is the method&apos;s response'/>"
     "</value>"
    "</returns>"
   "</signature>"
  "</signatures>"
  "<see>"
   "<item name='system.listMethods' />"
   "<item name='system.methodHelp' />"
   "<item name='system.describeMethods' />"
  "</see>"
  "<example/>"
  "<error/>"
  "<note/>"
  "<bug/>"
  "<todo/>"
 "</methodDescription>"

 "<!-- system.getCapabilities -->"
 "<methodDescription name='system.getCapabilities'>"
  "<author>Dan Libby</author>"
  "<purpose>returns a list of capabilities supported by this server</purpose>"
  "<version>1.0</version>"
  "<notes><item>spec url: http://groups.yahoo.com/group/xml-rpc/message/2897</item></notes>"
  "<signatures>"
   "<signature>"
    "<returns>"
     "<value type='struct' desc='list of capabilities, each with a unique key defined by the capability&apos;s spec'>"
      "<value type='struct' desc='definition of a single capability'>"
       "<value type='string' name='specURL'>www address of the specification defining this capability</value>"
       "<value type='int' name='specVersion'>version of the spec that this server's implementation conforms to</value>"
      "</value>"
     "</value>"
    "</returns>"
   "</signature>"
  "</signatures>"
  "<see>"
   "<item name='system.listMethods' />"
   "<item name='system.methodHelp' />"
   "<item name='system.describeMethods' />"
  "</see>"
  "<example/>"
  "<error/>"
  "<note/>"
  "<bug/>"
  "<todo/>"
 "</methodDescription>"

 "</methodList>"
"</introspection>";


/* forward declarations for static (non public, non api) funcs */
static XMLRPC_VALUE xsm_system_multicall_cb(XMLRPC_SERVER server, XMLRPC_REQUEST input, void* userData);
static XMLRPC_VALUE xsm_system_get_capabilities_cb(XMLRPC_SERVER server, XMLRPC_REQUEST input, void* userData);

/*-*******************
* System Methods API *
*********************/

static void xsm_lazy_doc_methods_cb(XMLRPC_SERVER server, void* userData) {
   XMLRPC_VALUE xDesc = XMLRPC_IntrospectionCreateDescription(xsm_introspection_xml, NULL);
   XMLRPC_ServerAddIntrospectionData(server, xDesc);
   XMLRPC_CleanupValue(xDesc);
}

void xsm_register(XMLRPC_SERVER server) {
   xi_register_system_methods(server);

   XMLRPC_ServerRegisterMethod(server, xsm_token_system_multicall, xsm_system_multicall_cb);
   XMLRPC_ServerRegisterMethod(server, xsm_token_system_get_capabilities, xsm_system_get_capabilities_cb);

   /* callback for documentation generation should it be requested */
   XMLRPC_ServerRegisterIntrospectionCallback(server, xsm_lazy_doc_methods_cb);
}

XMLRPC_VALUE xsm_system_multicall_cb(XMLRPC_SERVER server, XMLRPC_REQUEST input, void* userData) {
   XMLRPC_VALUE xArray = XMLRPC_VectorRewind(XMLRPC_RequestGetData(input));
   XMLRPC_VALUE xReturn = XMLRPC_CreateVector(0, xmlrpc_vector_array);

   if (xArray) {
      XMLRPC_VALUE xMethodIter = XMLRPC_VectorRewind(xArray);

      while (xMethodIter) {
         XMLRPC_REQUEST request = XMLRPC_RequestNew();
         if(request) {
            const char* methodName = XMLRPC_VectorGetStringWithID(xMethodIter, "methodName");
            XMLRPC_VALUE params = XMLRPC_VectorGetValueWithID(xMethodIter, "params");

            if(methodName && params) {
               XMLRPC_VALUE xRandomArray = XMLRPC_CreateVector(0, xmlrpc_vector_array);
               XMLRPC_RequestSetMethodName(request, methodName);
               XMLRPC_RequestSetData(request, params);
               XMLRPC_RequestSetRequestType(request, xmlrpc_request_call);

               XMLRPC_AddValueToVector(xRandomArray, 
                                       XMLRPC_ServerCallMethod(server, request, userData));

               XMLRPC_AddValueToVector(xReturn, xRandomArray);
            }
            XMLRPC_RequestFree(request, 1);
         }
         xMethodIter = XMLRPC_VectorNext(xArray);
      }
   }
   return xReturn;
}


XMLRPC_VALUE xsm_system_get_capabilities_cb(XMLRPC_SERVER server, XMLRPC_REQUEST input, void* userData) {
   XMLRPC_VALUE xReturn = XMLRPC_CreateVector(0, xmlrpc_vector_struct);
   XMLRPC_VALUE xFaults = XMLRPC_CreateVector("faults_interop", xmlrpc_vector_struct);
   XMLRPC_VALUE xIntro = XMLRPC_CreateVector("introspection", xmlrpc_vector_struct);

   /* support for fault spec */
   XMLRPC_VectorAppendString(xFaults, "specURL", "http://xmlrpc-epi.sourceforge.net/specs/rfc.fault_codes.php", 0);
   XMLRPC_VectorAppendInt(xFaults, "specVersion", 20010516);

   /* support for introspection spec */
   XMLRPC_VectorAppendString(xIntro, "specURL", "http://xmlrpc-epi.sourceforge.net/specs/rfc.introspection.php", 0);
   XMLRPC_VectorAppendInt(xIntro, "specVersion", 20010516);

   XMLRPC_AddValuesToVector(xReturn,
                            xFaults,
                            xIntro,
                            NULL);

   return xReturn;
                            
}

/*-***********************
* End System Methods API *
*************************/



