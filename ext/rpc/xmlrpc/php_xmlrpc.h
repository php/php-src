/*
  This file is part of, or distributed with, libXMLRPC - a C library for 
  xml-encoded function calls.

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

/* auto-generated portions of this file are also subject to the php license */

/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Dan Libby                                                    |
   +----------------------------------------------------------------------+
 */

#ifndef _PHP_XMLRPC_H
#define _PHP_XMLRPC_H

/* You should tweak config.m4 so this symbol (or some else suitable)
   gets defined.
*/
#if 1 /* HAVE_XMLRPC */

extern zend_module_entry xmlrpc_module_entry;
#define phpext_xmlrpc_ptr &xmlrpc_module_entry

#ifdef PHP_WIN32
#define PHP_XMLRPC_API __declspec(dllexport)
#else
#define PHP_XMLRPC_API
#endif

PHP_MINIT_FUNCTION(xmlrpc);
PHP_MSHUTDOWN_FUNCTION(xmlrpc);
PHP_RINIT_FUNCTION(xmlrpc);
PHP_RSHUTDOWN_FUNCTION(xmlrpc);
PHP_MINFO_FUNCTION(xmlrpc);

PHP_FUNCTION(xmlrpc_encode);
PHP_FUNCTION(xmlrpc_decode);
PHP_FUNCTION(xmlrpc_decode_request);
PHP_FUNCTION(xmlrpc_encode_request);
PHP_FUNCTION(xmlrpc_get_type);
PHP_FUNCTION(xmlrpc_set_type);
PHP_FUNCTION(xmlrpc_is_fault);
PHP_FUNCTION(xmlrpc_server_create);
PHP_FUNCTION(xmlrpc_server_destroy);
PHP_FUNCTION(xmlrpc_server_register_method);
PHP_FUNCTION(xmlrpc_server_call_method);
PHP_FUNCTION(xmlrpc_parse_method_descriptions);
PHP_FUNCTION(xmlrpc_server_add_introspection_data);
PHP_FUNCTION(xmlrpc_server_register_introspection_callback);

/* Fill in this structure and use entries in it
   for thread safety instead of using true globals.
*/
typedef struct {
	int x; /* fix error in msvc, cannot have empty structs */
} zend_xmlrpc_globals;

/* In every function that needs to use variables in zend_xmlrpc_globals,
   do call XMLRPCLS_FETCH(); after declaring other variables used by
   that function, and always refer to them as XMLRPCG(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#else

#define phpext_xmlrpc_ptr NULL

#endif

#endif	/* _PHP_XMLRPC_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
