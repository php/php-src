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

/* only non-public things should be in this file.  It is fine for any .c file
 * in xmlrpc/src to include it, but users of the public API should never
 * include it, and thus *.h files that are part of the public API should
 * never include it, or they would break if this file is not present.
 */

#ifndef XMLRPC_PRIVATE_ALREADY_INCLUDED
/*
 * Avoid include redundancy.
 */
#define XMLRPC_PRIVATE_ALREADY_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif


/*----------------------------------------------------------------------------
 * xmlrpc_private.h
 *
 * Purpose:
 *   define non-public intra-library routines & data
 * Comments:
 */

/*----------------------------------------------------------------------------
 * Constants
 */


/*----------------------------------------------------------------------------
 * Includes
 */

/*----------------------------------------------------------------------------
 * Structures
 */

/* Some of these are typedef'd in xmlrpc.h for public use */

typedef struct _xmlrpc_vector* XMLRPC_VECTOR;

/****s* VALUE/XMLRPC_VALUE
 * NAME
 *   XMLRPC_VALUE
 * NOTES
 *   A value of variable data type. The most important object in this API.  :)
 *
 *  This struct is opaque to callers and should be accessed only via accessor functions.
 * SEE ALSO
 *   XMLRPC_REQUEST
 *   XMLRPC_CreateValueEmpty ()
 *   XMLRPC_CleanupValue ()
 * SOURCE
 */
typedef struct _xmlrpc_value {
   XMLRPC_VALUE_TYPE type; /* data type of this value                        */
   XMLRPC_VECTOR v;        /* vector type specific info                      */
   simplestring str;       /* string value buffer                            */
   simplestring id;        /* id of this value.  possibly empty.             */
   int i;                  /* integer value.                                 */
   double d;               /* double value                                   */
   int iRefCount;          /* So we know when we can delete the value      . */
} STRUCT_XMLRPC_VALUE;
/******/

/****s* VALUE/XMLRPC_REQUEST
 * NAME
 *   XMLRPC_REQUEST
 * NOTES
 *   Internal representation of an XML request.
 *
 *  This struct is opaque to callers and should be accessed only via accessor functions.
 *
 * SEE ALSO
 *   XMLRPC_VALUE
 *   XMLRPC_RequestNew ()
 *   XMLRPC_RequestFree ()
 * SOURCE
 */
typedef struct _xmlrpc_request {
   XMLRPC_VALUE                         io;           /* data associated with this request */
   simplestring                         methodName;   /* name of method being called       */
   XMLRPC_REQUEST_TYPE                  request_type; /* type of request                   */
   STRUCT_XMLRPC_REQUEST_OUTPUT_OPTIONS output;       /* xml output options                */
   XMLRPC_VALUE                         error;        /* error codes                       */
} STRUCT_XMLRPC_REQUEST;
/******/

/* Vector type. Used by XMLRPC_VALUE.  Never visible to users of the API. */
typedef struct _xmlrpc_vector {
   XMLRPC_VECTOR_TYPE type;                           /* vector type                       */
   queue *q;                                          /* list of child values              */
} STRUCT_XMLRPC_VECTOR;
/******/

/****s* VALUE/XMLRPC_SERVER
 * NAME
 *   XMLRPC_SERVER
 * NOTES
 *   internal representation of an xmlrpc server
 *
 *  This struct is opaque to callers and should be accessed only via accessor functions.
 *
 * SEE ALSO
 *   XMLRPC_ServerCreate ()
 *   XMLRPC_ServerDestroy ()
 * SOURCE
 */
typedef struct _xmlrpc_server {
   queue methodlist;                                  /* list of callback methods          */
   queue docslist;                                    /* list of introspection callbacks   */
   XMLRPC_VALUE xIntrospection;
} STRUCT_XMLRPC_SERVER;
/******/

typedef struct _server_method {
   char*                   name;
   XMLRPC_VALUE            desc;
   XMLRPC_Callback         method;
} server_method;


/*----------------------------------------------------------------------------
 * Globals
 */

/*----------------------------------------------------------------------------
 * Functions
 */
server_method* find_method(XMLRPC_SERVER server, const char* name);
const char* type_to_str(XMLRPC_VALUE_TYPE type, XMLRPC_VECTOR_TYPE vtype);

/*----------------------------------------------------------------------------
 * Macros
 */
#define my_free(thing)  if(thing) {free(thing); thing = 0;}


#ifdef __cplusplus
}
#endif


#endif /* XMLRPC_PRIVATE_ALREADY_INCLUDED */

