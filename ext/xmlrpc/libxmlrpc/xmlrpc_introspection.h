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

/* IMPORTANT!
 *
 * only public (official API) things should be in this file. Anything else
 * should go in <group>_private.h, or in the appropriate .c file.
 */


#ifndef __XI_INTROSPECTION_H
/*
 * Avoid include redundancy.
 */
#define __XI_INTROSPECTION_H

/*----------------------------------------------------------------------------
 * xmlrpc_introspection.h
 *
 * Purpose:
 *   define public introspection API
 * Comments:
 */

/*----------------------------------------------------------------------------
 * Constants
 */
 #define xi_token_params "params"
 #define xi_token_returns "returns"
 #define xi_token_related "related"
 #define xi_token_sub "sub"


/*----------------------------------------------------------------------------
 * Includes
 */

/*----------------------------------------------------------------------------
 * Structures
 */

 /****d* VALUE/XMLRPC_IntrospectionCallback
 * NAME
 *   XMLRPC_IntrospectionCallback
 * NOTES
 *   Function prototype for lazy documentation generation (not generated until requested).
 * SOURCE
 */
typedef void (*XMLRPC_IntrospectionCallback)(XMLRPC_SERVER server, void* userData);
/******/


/*----------------------------------------------------------------------------
 * Globals
 */

/*----------------------------------------------------------------------------
 * Functions
 */
XMLRPC_VALUE XMLRPC_IntrospectionCreateDescription(const char* xml, XMLRPC_ERROR error);
int XMLRPC_ServerAddIntrospectionData(XMLRPC_SERVER server, XMLRPC_VALUE desc);
int XMLRPC_ServerRegisterIntrospectionCallback(XMLRPC_SERVER server, XMLRPC_IntrospectionCallback cb);

/*----------------------------------------------------------------------------
 * Macros
 */


#endif /* __XI_INTROSPECTION_H */
