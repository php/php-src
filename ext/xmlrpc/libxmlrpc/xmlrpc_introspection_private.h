/*
  This file is part of libXMLRPC - a C library for xml-encoded function calls.

  Author: Dan Libby (dan@libby.com)
  Epinions.com may be contacted at feedback@epinions-inc.com
*/

/*  
  Copyright 2001 Dan Libby, Epinions, Inc. 

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

/* IMPORTANT!
 *
 * only non-public things should be in this file.  It is fine for any .c file
 * in xmlrpc/src to include it, but users of the public API should never
 * include it, and thus *.h files that are part of the public API should
 * never include it, or they would break if this file is not present.
 */


#ifndef __XI_INTROSPECTION_PRIVATE_H
/*
 * Avoid include redundancy.
 */
#define __XI_INTROSPECTION_PRIVATE_H

/*----------------------------------------------------------------------------
 * xmlrpc_introspection_private.h
 *
 * Purpose:
 *   define non-public introspection routines
 * Comments:
 */

/*----------------------------------------------------------------------------
 * Constants
 */
#define xi_token_default                    "default"
#define xi_token_description                "description"
#define xi_token_name                       "name"
#define xi_token_optional                   "optional"
#define xi_token_params                     "params"
#define xi_token_purpose                    "purpose"
#define xi_token_returns                    "returns"
#define xi_token_signatures                 "signatures"
#define xi_token_type                       "type"
#define xi_token_version                    "version"
#define xi_token_empty                      ""
#define xi_token_system_describe_methods    "system.describeMethods"
#define xi_token_system_list_methods        "system.listMethods"
#define xi_token_system_method_help         "system.methodHelp"
#define xi_token_system_method_signature    "system.methodSignature"

 
/*----------------------------------------------------------------------------
 * Includes
 */

/*----------------------------------------------------------------------------
 * Structures
 */
typedef struct _doc_method {
   XMLRPC_IntrospectionCallback         method;
   int                                  b_called;
} doc_method; 
 
/*----------------------------------------------------------------------------
 * Globals
 */

/*----------------------------------------------------------------------------
 * Functions
 */
void xi_register_system_methods(XMLRPC_SERVER server);
 
/*----------------------------------------------------------------------------
 * Macros
 */
 

#endif /* __XI_INTROSPECTION_PRIVATE_H */




