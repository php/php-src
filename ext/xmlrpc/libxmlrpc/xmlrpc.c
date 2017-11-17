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


/****h* ABOUT/xmlrpc
 * NAME
 *   XMLRPC_VALUE
 * AUTHOR
 *   Dan Libby, aka danda  (dan@libby.com)
 * CREATION DATE
 *   9/1999 - 10/2000
 * HISTORY
 *   $Log$
 *   Revision 1.8.4.3.2.1  2008/09/10 00:07:44  felipe
 *   MFH:
 *   - Merged fix from SF project (Import Jeff Lawsons patches for XML datetime bug fixes)
 *     Fixed bugs:
 *     #45226 (xmlrpc_set_type() segfaults with valid ISO8601 date string)
 *     #18916 (xmlrpc_set_type() "not working")
 *
 *   Revision 1.8.4.3  2007/09/18 19:49:53  iliaa
 *
 *   Fixed bug #42189 (xmlrpc_set_type() crashes php on invalid datetime
 *     values).
 *
 *   Revision 1.8.4.2  2007/06/07 09:07:36  tony2001
 *   MFH: php_localtime_r() checks
 *
 *   Revision 1.8.4.1  2006/11/30 16:38:37  iliaa
 *   last set of zts fixes
 *
 *   Revision 1.8  2005/03/28 00:07:24  edink
 *   Reshufle includes to make it compile on windows
 *
 *   Revision 1.7  2005/03/26 03:13:58  sniper
 *   - Made it possible to build ext/xmlrpc with libxml2
 *
 *   Revision 1.6  2004/04/27 17:33:59  iliaa
 *   Removed C++ style comments.
 *
 *   Revision 1.5  2003/12/16 21:00:21  sniper
 *   Fix some compile warnings (patch by Joe Orton)
 *
 *   Revision 1.4  2002/07/05 04:43:53  danda
 *   merged in updates from SF project.  bring php repository up to date with xmlrpc-epi version 0.51
 *
 *   Revision 1.22  2002/03/09 23:15:44  danda
 *   add fault interrogation funcs
 *
 *   Revision 1.21  2002/03/09 22:27:41  danda
 *   win32 build patches contributed by Jeff Lawson
 *
 *   Revision 1.20  2002/02/13 20:58:50  danda
 *   patch to make source more windows friendly, contributed by Jeff Lawson
 *
 *   Revision 1.19  2001/10/12 23:25:54  danda
 *   default to writing xmlrpc
 *
 *   Revision 1.18  2001/09/29 21:58:05  danda
 *   adding cvs log to history section
 *
 *   10/15/2000 -- danda -- adding robodoc documentation
 *   08/2000 -- danda -- PHP C extension that uses XMLRPC
 *   08/2000 -- danda -- support for two vocabularies: danda-rpc and xml-rpc
 *   09/1999 -- danda -- Initial API, before I even knew of standard XMLRPC vocab. Response only.
 *   07/2000 -- danda -- wrote new implementation to be compatible with xmlrpc standard and
 *                       incorporated some ideas from ensor, most notably the separation of
 *                       xml dom from xmlrpc api.
 *   06/2000 -- danda -- played with expat-ensor from www.ensor.org.  Cool, but some flaws.
 * TODO
 * PORTABILITY
 *   Coded on RedHat Linux 6.2.  Builds on Solaris x86.  Should build on just
 *   about anything with minor mods.
 * NOTES
 *   Welcome to XMLRPC.  For more info on the specification and history, see
 *   http://www.xmlrpc.org.
 *
 *   This code aims to be a full-featured C implementation of XMLRPC.  It does not
 *   have any networking code.  Rather, it is intended to be plugged into apps
 *   or libraries with existing networking facilities, eg PHP, apache, perl, mozilla,
 *   home-brew application servers, etc.
 *
 *   Usage Paradigm:
 *     The user of this library will typically be implementing either an XMLRPC server,
 *     an XMLRPC client, or both.  The client will use the library to build an in-memory
 *     representation of a request, and then serialize (encode) that request into XML. The
 *     client will then send the XML to the server via external mechanism.  The server will
 *     de-serialize the XML back into an binary representation, call the appropriate registered
 *     method -- thereby generating a response.  The response will be serialized into XML and
 *     sent back to the client.  The client will de-serialize it into memory, and can
 *     iterate through the results via API.
 *
 *     Both the request and the response may consist of arbitrarily long, arbitrarily nested
 *     values.  The values may be one of several types, as defined by XMLRPC_VALUE_TYPE.
 *
 *   Features and Architecture:
 *     - The XML parsing (xml_element.c) is completely independent of the XMLRPC api. In fact,
 *       it can be used as a standalone dom implementation.
 *     - Because of this, the same XMLRPC data can be serialized into multiple xml vocabularies.
 *       It is simply a matter of writing a transport.  So far, two transports have been defined.
 *       The default xmlrpc vocab (xml_to_xmlrpc.c), and simple-rpc (xml_to_dandarpc.c) which is
 *       proprietary, but imho more readable, and nice for proprietary legacy reasons.
 *     - Various output options, including: xml escaping via CDATA or entity, case folding,
 *       vocab version, and character encoding.
 *     - One to One mapping between C structures and actual values, unlike ensor which forces
 *       one to understand the arcana of the xmlrpc vocab.
 *     - support for mixed indexed/keyed vector types, making it more compatible with
 *       languages such as PHP.
 *     - quite speedy compared to implementations written in interpreted languages. Also, uses
 *       intelligent string handling, so not many strlen() calls, etc.
 *     - comprehensive API for manipulation of values
 *******/

#include "ext/xml/expat_compat.h"
#include "main/php_reentrancy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <ctype.h>

#include "queue.h"
#include "xmlrpc.h"
#include "base64.h"

#include "xml_to_xmlrpc.h"
#include "xml_to_dandarpc.h"
#include "xml_to_soap.h"
#include "xml_element.h"
#include "xmlrpc_private.h"
#include "xmlrpc_introspection_private.h"
#include "system_methods_private.h"



/*-*********************
* Begin Time Functions *
***********************/

static time_t mkgmtime(struct tm *tm)
{
    static const int mdays[12] = {0,31,59,90,120,151,181,212,243,273,304,334};

    return ((((((tm->tm_year - 70) * 365) + mdays[tm->tm_mon] + tm->tm_mday-1 +
                  (tm->tm_year-68-1+(tm->tm_mon>=2))/4) * 24) + tm->tm_hour) * 60 +
        tm->tm_min) * 60 + tm->tm_sec;
}

static int date_from_ISO8601 (const char *text, time_t * value) {
   struct tm tm;
   int n;
   int i;
   char buf[30];


	if (strchr (text, '-')) {
		char *p = (char *) text, *p2 = buf;
		while (p && *p) {
			if (*p != '-') {
				*p2 = *p;
				p2++;
				if (p2-buf >= sizeof(buf)) {
					return -1;
				}
			}
			p++;
		}
		*p2 = 0;
		text = buf;
	}

	if (strlen(text)<17) {
		return -1;
	}

   tm.tm_isdst = -1;

#define XMLRPC_IS_NUMBER(x) if (x < '0' || x > '9') return -1;

   n = 1000;
   tm.tm_year = 0;
   for(i = 0; i < 4; i++) {
      XMLRPC_IS_NUMBER(text[i])
      tm.tm_year += (text[i]-'0')*n;
      n /= 10;
   }
   n = 10;
   tm.tm_mon = 0;
   for(i = 0; i < 2; i++) {
      XMLRPC_IS_NUMBER(text[i+4])
      tm.tm_mon += (text[i+4]-'0')*n;
      n /= 10;
   }
   tm.tm_mon --;
   if(tm.tm_mon < 0 || tm.tm_mon > 11) {
       return -1;
   }

   n = 10;
   tm.tm_mday = 0;
   for(i = 0; i < 2; i++) {
      XMLRPC_IS_NUMBER(text[i+6])
      tm.tm_mday += (text[i+6]-'0')*n;
      n /= 10;
   }

   n = 10;
   tm.tm_hour = 0;
   for(i = 0; i < 2; i++) {
      XMLRPC_IS_NUMBER(text[i+9])
      tm.tm_hour += (text[i+9]-'0')*n;
      n /= 10;
   }

   n = 10;
   tm.tm_min = 0;
   for(i = 0; i < 2; i++) {
      XMLRPC_IS_NUMBER(text[i+12])
      tm.tm_min += (text[i+12]-'0')*n;
      n /= 10;
   }

   n = 10;
   tm.tm_sec = 0;
   for(i = 0; i < 2; i++) {
      XMLRPC_IS_NUMBER(text[i+15])
      tm.tm_sec += (text[i+15]-'0')*n;
      n /= 10;
   }

   tm.tm_year -= 1900;

   *value = mkgmtime(&tm);

   return 0;

}

static int date_to_ISO8601 (time_t value, char *buf, int length) {
   struct tm *tm, tmbuf;
   tm = php_gmtime_r(&value, &tmbuf);
   if (!tm) {
	   return 0;
   }
#if 0  /* TODO: soap seems to favor this method. xmlrpc the latter. */
	return strftime (buf, length, "%Y-%m-%dT%H:%M:%SZ", tm);
#else
   return strftime(buf, length, "%Y%m%dT%H:%M:%SZ", tm);
#endif
}

/*-*******************
* End Time Functions *
*********************/


/*-***************************
* Begin XMLRPC_REQUEST funcs *
*****************************/

/****f* REQUEST/XMLRPC_RequestNew
 * NAME
 *   XMLRPC_RequestNew
 * SYNOPSIS
 *   XMLRPC_REQUEST XMLRPC_RequestNew()
 * FUNCTION
 *   Creates a new XMLRPC_Request data struct
 * INPUTS
 *   none
 * SEE ALSO
 *   XMLRPC_RequestFree ()
 * SOURCE
 */
XMLRPC_REQUEST XMLRPC_RequestNew() {
   XMLRPC_REQUEST xRequest = ecalloc(1, sizeof(STRUCT_XMLRPC_REQUEST));
   if(xRequest) {
      simplestring_init(&xRequest->methodName);
   }
   return xRequest;
}

/*******/

/****f* REQUEST/XMLRPC_RequestFree
 * NAME
 *   XMLRPC_RequestFree
 * SYNOPSIS
 *   void XMLRPC_RequestFree(XMLRPC_REQUEST request, int bFreeIO)
 * FUNCTION
 *   Free XMLRPC Request and all sub-values
 * INPUTS
 *   request -- previously allocated request struct
 *   bFreeIO -- 1 = also free request value data, if any, 0 = ignore.
 * SEE ALSO
 *   XMLRPC_RequestNew ()
 *   XMLRPC_CleanupValue ()
 * SOURCE
 */
void XMLRPC_RequestFree(XMLRPC_REQUEST request, int bFreeIO) {
   if(request) {
      simplestring_free(&request->methodName);

      if(request->io && bFreeIO) {
         XMLRPC_CleanupValue(request->io);
      }
      if(request->error) {
         XMLRPC_CleanupValue(request->error);
      }
      my_free(request);
   }
}

/*******/

/* Set Method Name to call */
/****f* REQUEST/XMLRPC_RequestSetMethodName
 * NAME
 *   XMLRPC_RequestSetMethodName
 * SYNOPSIS
 *   const char* XMLRPC_RequestSetMethodName(XMLRPC_REQUEST request, const char* methodName)
 * FUNCTION
 *   Set name of method to call with this request.
 * INPUTS
 *   request -- previously allocated request struct
 *   methodName -- name of method
 * SEE ALSO
 *   XMLRPC_RequestNew ()
 *   XMLRPC_RequestGetMethodName ()
 *   XMLRPC_RequestFree ()
 * SOURCE
 */
const char* XMLRPC_RequestSetMethodName(XMLRPC_REQUEST request, const char* methodName) {
   if(request) {
      simplestring_clear(&request->methodName);
      simplestring_add(&request->methodName, methodName);
      return request->methodName.str;
   }
   return NULL;
}

/*******/

/****f* REQUEST/XMLRPC_RequestGetMethodName
 * NAME
 *   XMLRPC_RequestGetMethodName
 * SYNOPSIS
 *   const char* XMLRPC_RequestGetMethodName(XMLRPC_REQUEST request)
 * FUNCTION
 *   Get name of method called by this request
 * INPUTS
 *   request -- previously allocated request struct
 * SEE ALSO
 *   XMLRPC_RequestNew ()
 *   XMLRPC_RequestSetMethodName ()
 *   XMLRPC_RequestFree ()
 * SOURCE
 */
const char* XMLRPC_RequestGetMethodName(XMLRPC_REQUEST request) {
   return request ? request->methodName.str : NULL;
}

/*******/

/****f* REQUEST/XMLRPC_RequestSetRequestType
 * NAME
 *   XMLRPC_RequestSetRequestType
 * SYNOPSIS
 *   XMLRPC_REQUEST_TYPE XMLRPC_RequestSetRequestType(XMLRPC_REQUEST request, XMLRPC_REQUEST_TYPE type)
 * FUNCTION
 *   A request struct may be allocated by a caller or by xmlrpc
 *   in response to a request.  This allows setting the
 *   request type.
 * INPUTS
 *   request -- previously allocated request struct
 *   type    -- request type [xmlrpc_method_call | xmlrpc_method_response]
 * SEE ALSO
 *   XMLRPC_RequestNew ()
 *   XMLRPC_RequestGetRequestType ()
 *   XMLRPC_RequestFree ()
 *   XMLRPC_REQUEST_TYPE
 * SOURCE
 */
XMLRPC_REQUEST_TYPE XMLRPC_RequestSetRequestType (XMLRPC_REQUEST request,
																  XMLRPC_REQUEST_TYPE type) {
   if(request) {
      request->request_type = type;
      return request->request_type;
   }
   return xmlrpc_request_none;
}

/*******/

/****f* REQUEST/XMLRPC_RequestGetRequestType
 * NAME
 *   XMLRPC_RequestGetRequestType
 * SYNOPSIS
 *   XMLRPC_REQUEST_TYPE XMLRPC_RequestGetRequestType(XMLRPC_REQUEST request)
 * FUNCTION
 *   A request struct may be allocated by a caller or by xmlrpc
 *   in response to a request.  This allows setting the
 *   request type.
 * INPUTS
 *   request -- previously allocated request struct
 * RESULT
 *   type    -- request type [xmlrpc_method_call | xmlrpc_method_response]
 * SEE ALSO
 *   XMLRPC_RequestNew ()
 *   XMLRPC_RequestSetRequestType ()
 *   XMLRPC_RequestFree ()
 *   XMLRPC_REQUEST_TYPE
 * SOURCE
 */
XMLRPC_REQUEST_TYPE XMLRPC_RequestGetRequestType(XMLRPC_REQUEST request) {
   return request ? request->request_type : xmlrpc_request_none;
}

/*******/


/****f* REQUEST/XMLRPC_RequestSetData
 * NAME
 *   XMLRPC_RequestSetData
 * SYNOPSIS
 *   XMLRPC_VALUE XMLRPC_RequestSetData(XMLRPC_REQUEST request, XMLRPC_VALUE data)
 * FUNCTION
 *   Associates a block of xmlrpc data with the request.  The
 *   data is *not* copied.  A pointer is kept.  The caller
 *   should be careful not to doubly free the data value,
 *   which may optionally be free'd by XMLRPC_RequestFree().
 * INPUTS
 *   request -- previously allocated request struct
 *   data    -- previously allocated data struct
 * RESULT
 *   XMLRPC_VALUE -- pointer to value stored, or NULL
 * SEE ALSO
 *   XMLRPC_RequestNew ()
 *   XMLRPC_RequestGetData ()
 *   XMLRPC_RequestFree ()
 *   XMLRPC_REQUEST
 *   XMLRPC_VALUE
 * SOURCE
 */
XMLRPC_VALUE XMLRPC_RequestSetData(XMLRPC_REQUEST request, XMLRPC_VALUE data) {
   if(request && data) {
		if (request->io) {
			XMLRPC_CleanupValue (request->io);
		}
      request->io = XMLRPC_CopyValue(data);
      return request->io;
   }
   return NULL;
}

/*******/

/****f* REQUEST/XMLRPC_RequestGetData
 * NAME
 *   XMLRPC_RequestGetData
 * SYNOPSIS
 *   XMLRPC_VALUE XMLRPC_RequestGetData(XMLRPC_REQUEST request)
 * FUNCTION
 *   Returns data associated with request, if any.
 * INPUTS
 *   request -- previously allocated request struct
 * RESULT
 *   XMLRPC_VALUE -- pointer to value stored, or NULL
 * SEE ALSO
 *   XMLRPC_RequestNew ()
 *   XMLRPC_RequestSetData ()
 *   XMLRPC_RequestFree ()
 *   XMLRPC_REQUEST
 *   XMLRPC_VALUE
 * SOURCE
 */
XMLRPC_VALUE XMLRPC_RequestGetData(XMLRPC_REQUEST request) {
   return request ? request->io : NULL;
}

/*******/

/****f* REQUEST/XMLRPC_RequestSetError
 * NAME
 *   XMLRPC_RequestSetError
 * SYNOPSIS
 *   XMLRPC_VALUE XMLRPC_RequestSetError(XMLRPC_REQUEST request, XMLRPC_VALUE error)
 * FUNCTION
 *   Associates a block of xmlrpc data, representing an error
 *   condition, with the request.
 * INPUTS
 *   request -- previously allocated request struct
 *   error   -- previously allocated error code or struct
 * RESULT
 *   XMLRPC_VALUE -- pointer to value stored, or NULL
 * NOTES
 *   This is a private function for usage by internals only.
 * SEE ALSO
 *   XMLRPC_RequestGetError ()
 * SOURCE
 */
XMLRPC_VALUE XMLRPC_RequestSetError (XMLRPC_REQUEST request, XMLRPC_VALUE error) {
	if (request && error) {
		if (request->error) {
			XMLRPC_CleanupValue (request->error);
		}
		request->error = XMLRPC_CopyValue (error);
		return request->error;
	}
	return NULL;
}

/*******/

/****f* REQUEST/XMLRPC_RequestGetError
 * NAME
 *   XMLRPC_RequestGetError
 * SYNOPSIS
 *   XMLRPC_VALUE XMLRPC_RequestGetError(XMLRPC_REQUEST request)
 * FUNCTION
 *   Returns error data associated with request, if any.
 * INPUTS
 *   request -- previously allocated request struct
 * RESULT
 *   XMLRPC_VALUE -- pointer to error value stored, or NULL
 * NOTES
 *   This is a private function for usage by internals only.
 * SEE ALSO
 *   XMLRPC_RequestSetError ()
 *   XMLRPC_RequestFree ()
 * SOURCE
 */
XMLRPC_VALUE XMLRPC_RequestGetError (XMLRPC_REQUEST request) {
	return request ? request->error : NULL;
}

/*******/


/****f* REQUEST/XMLRPC_RequestSetOutputOptions
 * NAME
 *   XMLRPC_RequestSetOutputOptions
 * SYNOPSIS
 *   XMLRPC_REQUEST_OUTPUT_OPTIONS XMLRPC_RequestSetOutputOptions(XMLRPC_REQUEST request, XMLRPC_REQUEST_OUTPUT_OPTIONS output)
 * FUNCTION
 *   Sets output options used for generating XML. The output struct
 *   is copied, and may be freed by the caller.
 * INPUTS
 *   request -- previously allocated request struct
 *   output  -- output options struct initialized by caller
 * RESULT
 *   XMLRPC_REQUEST_OUTPUT_OPTIONS -- pointer to value stored, or NULL
 * SEE ALSO
 *   XMLRPC_RequestNew ()
 *   XMLRPC_RequestGetOutputOptions ()
 *   XMLRPC_RequestFree ()
 *   XMLRPC_REQUEST
 *   XMLRPC_REQUEST_OUTPUT_OPTIONS
 * SOURCE
 */
XMLRPC_REQUEST_OUTPUT_OPTIONS XMLRPC_RequestSetOutputOptions(XMLRPC_REQUEST request, XMLRPC_REQUEST_OUTPUT_OPTIONS output) {
   if(request && output) {
		memcpy (&request->output, output,
				  sizeof (STRUCT_XMLRPC_REQUEST_OUTPUT_OPTIONS));
      return &request->output;
   }
   return NULL;
}

/*******/


/****f* REQUEST/XMLRPC_RequestGetOutputOptions
 * NAME
 *   XMLRPC_RequestGetOutputOptions
 * SYNOPSIS
 *   XMLRPC_REQUEST_OUTPUT_OPTIONS XMLRPC_RequestGetOutputOptions(XMLRPC_REQUEST request)
 * FUNCTION
 *   Gets a pointer to output options used for generating XML.
 * INPUTS
 *   request -- previously allocated request struct
 * RESULT
 *   XMLRPC_REQUEST_OUTPUT_OPTIONS -- pointer to options stored, or NULL
 * SEE ALSO
 *   XMLRPC_RequestNew ()
 *   XMLRPC_RequestSetOutputOptions ()
 *   XMLRPC_RequestFree ()
 *   XMLRPC_REQUEST
 *   XMLRPC_REQUEST_OUTPUT_OPTIONS
 * SOURCE
 */
XMLRPC_REQUEST_OUTPUT_OPTIONS XMLRPC_RequestGetOutputOptions(XMLRPC_REQUEST request) {
   return request ? &request->output : NULL;
}

/*******/

/*-*************************
* End XMLRPC_REQUEST funcs *
***************************/


/*-***************************
* Begin Serializiation funcs *
*****************************/

/****f* SERIALIZE/XMLRPC_VALUE_ToXML
 * NAME
 *   XMLRPC_VALUE_ToXML
 * SYNOPSIS
 *   char* XMLRPC_VALUE_ToXML(XMLRPC_VALUE val)
 * FUNCTION
 *   encode XMLRPC_VALUE into XML buffer.  Note that the generated
 *   buffer will not contain a methodCall.
 * INPUTS
 *   val -- previously allocated XMLRPC_VALUE
 *   buf_len -- length of returned buffer, if not null
 * RESULT
 *   char* -- newly allocated buffer containing XML.
 *   It is the caller's responsibility to free it.
 * SEE ALSO
 *   XMLRPC_REQUEST_ToXML ()
 *   XMLRPC_VALUE_FromXML ()
 *   XMLRPC_Free ()
 *   XMLRPC_VALUE
 * SOURCE
 */
char* XMLRPC_VALUE_ToXML(XMLRPC_VALUE val, int* buf_len) {
   xml_element *root_elem = XMLRPC_VALUE_to_xml_element(val);
   char* pRet = NULL;

   if(root_elem) {
      pRet = xml_elem_serialize_to_string(root_elem, NULL, buf_len);
      xml_elem_free(root_elem);
   }
   return pRet;
}

/*******/

/****f* SERIALIZE/XMLRPC_REQUEST_ToXML
 * NAME
 *   XMLRPC_REQUEST_ToXML
 * SYNOPSIS
 *   char* XMLRPC_REQUEST_ToXML(XMLRPC_REQUEST request)
 * FUNCTION
 *   encode XMLRPC_REQUEST into XML buffer
 * INPUTS
 *   request -- previously allocated XMLRPC_REQUEST
 *   buf_len -- size of returned buf, if not null
 * RESULT
 *   char* -- newly allocated buffer containing XML.
 *   It is the caller's responsibility to free it.
 * SEE ALSO
 *   XMLRPC_REQUEST_ToXML ()
 *   XMLRPC_REQUEST_FromXML ()
 *   XMLRPC_Free ()
 *   XMLRPC_VALUE_ToXML ()
 *   XMLRPC_REQUEST
 * SOURCE
 */
char* XMLRPC_REQUEST_ToXML(XMLRPC_REQUEST request, int* buf_len) {
      char* pRet = NULL;
	if (request) {
		xml_element *root_elem = NULL;
		if (request->output.version == xmlrpc_version_simple) {
			root_elem = DANDARPC_REQUEST_to_xml_element (request);
		}
		else if (request->output.version == xmlrpc_version_1_0 ||
					request->output.version == xmlrpc_version_none) {
			root_elem = XMLRPC_REQUEST_to_xml_element (request);
		}
		else if (request->output.version == xmlrpc_version_soap_1_1) {
			root_elem = SOAP_REQUEST_to_xml_element (request);
		}

      if(root_elem) {
			pRet =
			xml_elem_serialize_to_string (root_elem,
													&request->output.xml_elem_opts,
													buf_len);
         xml_elem_free(root_elem);
      }
   }
	return pRet;
}

/*******/

/****f* SERIALIZE/XMLRPC_VALUE_FromXML
 * NAME
 *   XMLRPC_VALUE_FromXML
 * SYNOPSIS
 *   XMLRPC_VALUE XMLRPC_VALUE_FromXML(const char* in_buf, int le
 * FUNCTION
 *   Retrieve XMLRPC_VALUE from XML buffer. Note that this will
 *   ignore any methodCall.  See XMLRPC_REQUEST_FromXML
 * INPUTS
 *   in_buf -- character buffer containing XML
 *   len    -- length of buffer
 * RESULT
 *   XMLRPC_VALUE -- newly allocated data, or NULL if error. Should
 *   be free'd by caller.
 * SEE ALSO
 *   XMLRPC_VALUE_ToXML ()
 *   XMLRPC_REQUEST_FromXML ()
 *   XMLRPC_VALUE
 * SOURCE
 */
XMLRPC_VALUE XMLRPC_VALUE_FromXML (const char *in_buf, int len, XMLRPC_REQUEST_INPUT_OPTIONS in_options) {
   XMLRPC_VALUE xResponse = NULL;
   XMLRPC_REQUEST req = XMLRPC_REQUEST_FromXML(in_buf, len, in_options);

   if(req) {
      xResponse = req->io;
      XMLRPC_RequestFree(req, 0);
   }
   return xResponse;
}

/*******/

/* map parser errors to standard xml-rpc errors */
static XMLRPC_VALUE map_expat_errors(XML_ELEM_ERROR error) {
   XMLRPC_VALUE xReturn = NULL;
   if(error) {
      XMLRPC_ERROR_CODE code;
      char buf[1024];
      snprintf(buf, sizeof(buf),
               "error occurred at line %ld, column %ld, byte index %ld",
					 error->line, error->column, error->byte_index);

      /* expat specific errors */
      switch(error->parser_code) {
      case XML_ERROR_UNKNOWN_ENCODING:
         code = xmlrpc_error_parse_unknown_encoding;
         break;
      case XML_ERROR_INCORRECT_ENCODING:
         code = xmlrpc_error_parse_bad_encoding;
         break;
      default:
         code = xmlrpc_error_parse_xml_syntax;
         break;
      }
      xReturn = XMLRPC_UtilityCreateFault(code, buf);
   }
   return xReturn;
}

/****f* SERIALIZE/XMLRPC_REQUEST_FromXML
 * NAME
 *   XMLRPC_REQUEST_FromXML
 * SYNOPSIS
 *   XMLRPC_REQUEST XMLRPC_REQUEST_FromXML(const char* in_buf, int le
 * FUNCTION
 *   Retrieve XMLRPC_REQUEST from XML buffer
 * INPUTS
 *   in_buf -- character buffer containing XML
 *   len    -- length of buffer
 * RESULT
 *   XMLRPC_REQUEST -- newly allocated data, or NULL if error. Should
 *   be free'd by caller.
 * SEE ALSO
 *   XMLRPC_REQUEST_ToXML ()
 *   XMLRPC_VALUE_FromXML ()
 *   XMLRPC_REQUEST
 * SOURCE
 */
XMLRPC_REQUEST XMLRPC_REQUEST_FromXML (const char *in_buf, int len,
													XMLRPC_REQUEST_INPUT_OPTIONS in_options) {
   XMLRPC_REQUEST request = XMLRPC_RequestNew();
   STRUCT_XML_ELEM_ERROR error = {0};

   if(request) {
		xml_element *root_elem =
		xml_elem_parse_buf (in_buf, len,
								  (in_options ? &in_options->xml_elem_opts : NULL),
								  &error);

      if(root_elem) {
         if(!strcmp(root_elem->name, "simpleRPC")) {
            request->output.version = xmlrpc_version_simple;
            xml_element_to_DANDARPC_REQUEST(request, root_elem);
         }
			else if (!strcmp (root_elem->name, "SOAP-ENV:Envelope")) {
				request->output.version = xmlrpc_version_soap_1_1;
				xml_element_to_SOAP_REQUEST (request, root_elem);
			}
         else {
            request->output.version = xmlrpc_version_1_0;
            xml_element_to_XMLRPC_REQUEST(request, root_elem);
         }
         xml_elem_free(root_elem);
      }
      else {
         if(error.parser_error) {
				XMLRPC_RequestSetError (request, map_expat_errors (&error));
         }
      }
   }

   return request;
}

/*******/

/*-************************
* End Serialization Funcs *
**************************/



/****f* VALUE/XMLRPC_CreateValueEmpty
 * NAME
 *   XMLRPC_CreateValueEmpty
 * SYNOPSIS
 *   XMLRPC_VALUE XMLRPC_CreateValueEmpty ()
 * FUNCTION
 *   Create an XML value to be used/modified elsewhere.
 * INPUTS
 * RESULT
 *   XMLRPC_VALUE.  The new value, or NULL on failure.
 * SEE ALSO
 *   XMLRPC_CleanupValue ()
 *   XMLRPC_VALUE
 * SOURCE
 */
XMLRPC_VALUE XMLRPC_CreateValueEmpty() {
   XMLRPC_VALUE v = ecalloc(1, sizeof(STRUCT_XMLRPC_VALUE));
   if(v) {
#ifdef XMLRPC_DEBUG_REFCOUNT
		printf ("calloc'd 0x%x\n", v);
#endif
      v->type = xmlrpc_empty;
      simplestring_init(&v->id);
      simplestring_init(&v->str);
   }
   return v;
}

/*******/

/****f* VALUE/XMLRPC_SetValueID_Case
 * NAME
 *   XMLRPC_SetValueID_Case
 * SYNOPSIS
 *   const char *XMLRPC_SetValueID_Case(XMLRPC_VALUE value, const char* id, int len, XMLRPC_CASE id_case)
 * FUNCTION
 *   Assign an ID (key) to an XMLRPC value.
 * INPUTS
 *   value     The xml value who's ID we will set.
 *   id        The desired new id.
 *   len       length of id string if known, or 0 if unknown.
 *   id_case   one of XMLRPC_CASE
 * RESULT
 *   const char*  pointer to the newly allocated id string, or NULL
 * SEE ALSO
 *   XMLRPC_SetValueID ()
 *   XMLRPC_GetValueID ()
 *   XMLRPC_VALUE
 *   XMLRPC_CASE
 * SOURCE
 */
const char *XMLRPC_SetValueID_Case(XMLRPC_VALUE value, const char* id, int len, XMLRPC_CASE id_case) {
   const char* pRetval = NULL;
   if(value) {
      if(id) {
         simplestring_clear(&value->id);
         (len > 0) ? simplestring_addn(&value->id, id, len) :
                     simplestring_add(&value->id, id);

         /* upper or lower case string in place if required. could be a separate func. */
         if(id_case == xmlrpc_case_lower || id_case == xmlrpc_case_upper) {
            int i;
            for(i = 0; i < value->id.len; i++) {
					value->id.str[i] =
					(id_case ==
					 xmlrpc_case_lower) ? tolower (value->id.
															 str[i]) : toupper (value->
																					  id.
																					  str[i]);
            }
         }

         pRetval = value->id.str;

#ifdef XMLRPC_DEBUG_REFCOUNT
         printf("set value id: %s\n", pRetval);
#endif
      }
   }

   return pRetval;
}

/*******/


/****f* VALUE/XMLRPC_SetValueString
 * NAME
 *   XMLRPC_SetValueString
 * SYNOPSIS
 *   const char *XMLRPC_SetValueString(XMLRPC_VALUE value, const char* val, int len)
 * FUNCTION
 *   Assign a string value to an XMLRPC_VALUE, and set it to type xmlrpc_string
 * INPUTS
 *   value     The xml value who's ID we will set.
 *   val        The desired new string val.
 *   len       length of val string if known, or 0 if unknown.
 * RESULT
 *   const char*  pointer to the newly allocated value string, or NULL
 * SEE ALSO
 *   XMLRPC_GetValueString ()
 *   XMLRPC_VALUE
 *   XMLRPC_VALUE_TYPE
 * SOURCE
 */
const char *XMLRPC_SetValueString(XMLRPC_VALUE value, const char* val, int len) {
   char *pRetval = NULL;
   if(value && val) {
      simplestring_clear(&value->str);
      (len > 0) ? simplestring_addn(&value->str, val, len) :
                  simplestring_add(&value->str, val);
      value->type = xmlrpc_string;
      pRetval = (char *)value->str.str;
   }

   return pRetval;
}

/*******/

/****f* VALUE/XMLRPC_SetValueInt
 * NAME
 *   XMLRPC_SetValueInt
 * SYNOPSIS
 *   void XMLRPC_SetValueInt(XMLRPC_VALUE value, int val)
 * FUNCTION
 *   Assign an int value to an XMLRPC_VALUE, and set it to type xmlrpc_int
 * INPUTS
 *   value     The xml value who's ID we will set.
 *   val        The desired new integer value
 * RESULT
 * SEE ALSO
 *   XMLRPC_GetValueInt ()
 *   XMLRPC_VALUE
 *   XMLRPC_VALUE_TYPE
 * SOURCE
 */
void XMLRPC_SetValueInt(XMLRPC_VALUE value, int val) {
   if(value) {
      value->type = xmlrpc_int;
      value->i = val;
   }
}

/*******/

/****f* VALUE/XMLRPC_SetValueBoolean
 * NAME
 *   XMLRPC_SetValueBoolean
 * SYNOPSIS
 *   void XMLRPC_SetValueBoolean(XMLRPC_VALUE value, int val)
 * FUNCTION
 *   Assign a boolean value to an XMLRPC_VALUE, and set it to type xmlrpc_boolean
 * INPUTS
 *   value     The xml value who's value we will set.
 *   val        The desired new boolean value. [0 | 1]
 * RESULT
 * SEE ALSO
 *   XMLRPC_GetValueBoolean ()
 *   XMLRPC_VALUE
 *   XMLRPC_VALUE_TYPE
 * SOURCE
 */
void XMLRPC_SetValueBoolean(XMLRPC_VALUE value, int val) {
   if(value) {
      value->type = xmlrpc_boolean;
      value->i = val ? 1 : 0;
   }
}

/*******/


/****f* VECTOR/XMLRPC_SetIsVector
 * NAME
 *   XMLRPC_SetIsVector
 * SYNOPSIS
 *   int XMLRPC_SetIsVector(XMLRPC_VALUE value, XMLRPC_VECTOR_TYPE type)
 * FUNCTION
 *   Set the XMLRPC_VALUE to be a vector (list) type.  The vector may be one of
 *   [xmlrpc_array | xmlrpc_struct | xmlrpc_mixed].  An array has only index values.
 *   A struct has key/val pairs.  Mixed allows both index and key/val combinations.
 * INPUTS
 *   value     The xml value who's vector type we will set
 *   type      New type of vector as enumerated by XMLRPC_VECTOR_TYPE
 * RESULT
 *   int       1 if successful, 0 otherwise
 * SEE ALSO
 *   XMLRPC_GetValueType ()
 *   XMLRPC_GetVectorType ()
 *   XMLRPC_VALUE
 *   XMLRPC_VECTOR_TYPE
 *   XMLRPC_VALUE_TYPE
 * SOURCE
 */
int XMLRPC_SetIsVector(XMLRPC_VALUE value, XMLRPC_VECTOR_TYPE type) {
   int bSuccess = 0;

	if (value) {
		/* we can change the type so long as nothing is currently stored. */
		if(value->type == xmlrpc_vector) {
			if(value->v) {
				if(!Q_Size(value->v->q)) {
					value->v->type = type;
				}
			}
		}
		else {
      value->v = ecalloc(1, sizeof(STRUCT_XMLRPC_VECTOR));
      if(value->v) {
         value->v->q = (queue*)emalloc(sizeof(queue));
         if(value->v->q) {
            Q_Init(value->v->q);
            value->v->type = type;
            value->type = xmlrpc_vector;
            bSuccess = 1;
         }
      }
   }
	}

   return bSuccess;
}

/*******/

/****f* VECTOR/XMLRPC_CreateVector
 * NAME
 *   XMLRPC_CreateVector
 * SYNOPSIS
 *   XMLRPC_VALUE XMLRPC_CreateVector(const char* id, XMLRPC_VECTOR_TYPE type)
 * FUNCTION
 *   Create a new vector and optionally set an id.
 * INPUTS
 *   id        The id of the vector, or NULL
 *   type      New type of vector as enumerated by XMLRPC_VECTOR_TYPE
 * RESULT
 *   XMLRPC_VALUE  The new vector, or NULL on failure.
 * SEE ALSO
 *   XMLRPC_CreateValueEmpty ()
 *   XMLRPC_SetIsVector ()
 *   XMLRPC_GetValueType ()
 *   XMLRPC_GetVectorType ()
 *   XMLRPC_VALUE
 *   XMLRPC_VECTOR_TYPE
 *   XMLRPC_VALUE_TYPE
 * SOURCE
 */
XMLRPC_VALUE XMLRPC_CreateVector(const char* id, XMLRPC_VECTOR_TYPE type) {
   XMLRPC_VALUE val = NULL;

   val = XMLRPC_CreateValueEmpty();
   if(val) {
      if(XMLRPC_SetIsVector(val, type)) {
         if(id) {
            const char *pSVI = NULL;

            pSVI = XMLRPC_SetValueID(val, id, 0);
            if(NULL == pSVI) {
               val = NULL;
            }
         }
      }
      else {
         val = NULL;
      }
   }
   return val;
}

/*******/


/* Not yet implemented.
 *
 * This should use a hash to determine if a given target id has already
 * been appended.
 *
 * Alternately, it could walk the entire vector, but that could be quite
 * slow for very large lists.
 */
static int isDuplicateEntry(XMLRPC_VALUE target, XMLRPC_VALUE source) {
   return 0;
}

/****f* VECTOR/XMLRPC_AddValueToVector
 * NAME
 *   XMLRPC_AddValueToVector
 * SYNOPSIS
 *   int XMLRPC_AddValueToVector(XMLRPC_VALUE target, XMLRPC_VALUE source)
 * FUNCTION
 *   Add (append) an existing XMLRPC_VALUE to a vector.
 * INPUTS
 *   target    The target vector
 *   source    The source value to append
 * RESULT
 *   int       1 if successful, else 0
 * SEE ALSO
 *   XMLRPC_AddValuesToVector ()
 *   XMLRPC_VectorGetValueWithID_Case ()
 *   XMLRPC_VALUE
 * NOTES
 *   The function will fail and return 0 if an attempt is made to add
 *   a value with an ID into a vector of type xmlrpc_vector_array. Such
 *   values can only be added to xmlrpc_vector_struct.
 * SOURCE
 */
int XMLRPC_AddValueToVector(XMLRPC_VALUE target, XMLRPC_VALUE source) {
   if(target && source) {
      if(target->type == xmlrpc_vector && target->v &&
         target->v->q && target->v->type != xmlrpc_vector_none) {

         /* guard against putting value of unknown type into vector */
         switch(source->type) {
            case xmlrpc_empty:
            case xmlrpc_base64:
            case xmlrpc_boolean:
            case xmlrpc_datetime:
            case xmlrpc_double:
            case xmlrpc_int:
            case xmlrpc_string:
            case xmlrpc_vector:
               /* Guard against putting a key/val pair into an array vector */
               if( !(source->id.len && target->v->type == xmlrpc_vector_array) ) {
					if (isDuplicateEntry (target, source)
						 || Q_PushTail (target->v->q, XMLRPC_CopyValue (source))) {
                     return 1;
                  }
               }
               else {
					/* fprintf (stderr,
								"xmlrpc: attempted to add key/val pair to vector of type array\n"); */
               }
               break;
            default:
				/* fprintf (stderr,
							"xmlrpc: attempted to add value of unknown type to vector\n"); */
               break;
         }
      }
   }
   return 0;
}

/*******/


/****f* VECTOR/XMLRPC_AddValuesToVector
 * NAME
 *   XMLRPC_AddValuesToVector
 * SYNOPSIS
 *   XMLRPC_AddValuesToVector ( target, val1, val2, val3, val(n), 0 )
 *   XMLRPC_AddValuesToVector( XMLRPC_VALUE, ... )
 * FUNCTION
 *   Add (append) a series of existing XMLRPC_VALUE to a vector.
 * INPUTS
 *   target    The target vector
 *   ...       The source value(s) to append.  The last item *must* be 0.
 * RESULT
 *   int       1 if successful, else 0
 * SEE ALSO
 *   XMLRPC_AddValuesToVector ()
 *   XMLRPC_VectorGetValueWithID_Case ()
 *   XMLRPC_VALUE
 * NOTES
 *   This function may actually return failure after it has already modified
 *     or added items to target.  You can not trust the state of target
 *     if this function returns failure.
 * SOURCE
 */
int XMLRPC_AddValuesToVector(XMLRPC_VALUE target, ...) {
   int iRetval = 0;

   if(target) {
      if(target->type == xmlrpc_vector) {
         XMLRPC_VALUE v = NULL;
         va_list vl;

         va_start(vl, target);

         do {
            v = va_arg(vl, XMLRPC_VALUE);
            if(v) {
               if(!XMLRPC_AddValueToVector(target, v)) {
                  iRetval = 0;
                  break;
               }
            }
			}
			while (v);

         va_end(vl);

         if(NULL == v) {
            iRetval = 1;
         }
      }
   }
   return iRetval;
}

/*******/


/****f* VECTOR/XMLRPC_VectorGetValueWithID_Case
 * NAME
 *   XMLRPC_VectorGetValueWithID_Case
 * SYNOPSIS
 *   XMLRPC_VALUE XMLRPC_VectorGetValueWithID_Case(XMLRPC_VALUE vector, const char* id, XMLRPC_CASE_COMPARISON id_case)
 * FUNCTION
 *   Get value from vector matching id (key)
 * INPUTS
 *   vector    The source vector
 *   id        The key to find
 *   id_case   Rule for how to match key
 * RESULT
 *   int       1 if successful, else 0
 * SEE ALSO
 *   XMLRPC_SetValueID_Case ()
 *   XMLRPC_VALUE
 *   XMLRPC_CASE_COMPARISON
 * SOURCE
 */
XMLRPC_VALUE XMLRPC_VectorGetValueWithID_Case (XMLRPC_VALUE vector, const char *id,
															  XMLRPC_CASE_COMPARISON id_case) {
   if(vector && vector->v && vector->v->q) {
       q_iter qi = Q_Iter_Head_F(vector->v->q);

       while(qi) {
          XMLRPC_VALUE xIter = Q_Iter_Get_F(qi);
          if(xIter && xIter->id.str) {
             if(id_case == xmlrpc_case_sensitive) {
                if(!strcmp(xIter->id.str, id)) {
                   return xIter;
                }
             }
             else if(id_case == xmlrpc_case_insensitive) {
                if(!strcasecmp(xIter->id.str, id)) {
                   return xIter;
                }
             }
          }
          qi = Q_Iter_Next_F(qi);
       }
   }
   return NULL;
}

/*******/


int XMLRPC_VectorRemoveValue(XMLRPC_VALUE vector, XMLRPC_VALUE value) {
   if(vector && vector->v && vector->v->q && value) {
       q_iter qi = Q_Iter_Head_F(vector->v->q);

       while(qi) {
          XMLRPC_VALUE xIter = Q_Iter_Get_F(qi);
          if(xIter == value) {
             XMLRPC_CleanupValue(xIter);
             Q_Iter_Del(vector->v->q, qi);
             return 1;
          }
          qi = Q_Iter_Next_F(qi);
       }
   }
   return 0;
}


/****f* VALUE/XMLRPC_CreateValueString
 * NAME
 *   XMLRPC_CreateValueString
 * SYNOPSIS
 *   XMLRPC_VALUE XMLRPC_CreateValueString(const char* id, const char* val, int len)
 * FUNCTION
 *   Create an XMLRPC_VALUE, and assign a string to it
 * INPUTS
 *   id        The id of the value, or NULL
 *   val       The desired new string val.
 *   len       length of val string if known, or 0 if unknown.
 * RESULT
 *   newly allocated XMLRPC_VALUE, or NULL
 * SEE ALSO
 *   XMLRPC_GetValueString ()
 *   XMLRPC_CreateValueEmpty ()
 *   XMLRPC_VALUE
 *   XMLRPC_VALUE_TYPE
 * SOURCE
 */
XMLRPC_VALUE XMLRPC_CreateValueString(const char* id, const char* val, int len) {
   XMLRPC_VALUE value = NULL;
   if(val) {
      value = XMLRPC_CreateValueEmpty();
      if(value) {
         XMLRPC_SetValueString(value, val, len);
         if(id) {
            XMLRPC_SetValueID(value, id, 0);
         }
      }
   }
   return value;
}

/*******/

/****f* VALUE/XMLRPC_CreateValueInt
 * NAME
 *   XMLRPC_CreateValueInt
 * SYNOPSIS
 *   XMLRPC_VALUE XMLRPC_CreateValueInt(const char* id, int i)
 * FUNCTION
 *   Create an XMLRPC_VALUE, and assign an int to it
 * INPUTS
 *   id        The id of the value, or NULL
 *   i         The desired new int val.
 * RESULT
 *   newly allocated XMLRPC_VALUE, or NULL
 * SEE ALSO
 *   XMLRPC_GetValueInt ()
 *   XMLRPC_CreateValueEmpty ()
 *   XMLRPC_VALUE
 *   XMLRPC_VALUE_TYPE
 * SOURCE
 */
XMLRPC_VALUE XMLRPC_CreateValueInt(const char* id, int i) {
   XMLRPC_VALUE val = XMLRPC_CreateValueEmpty();
   if(val) {
      XMLRPC_SetValueInt(val, i);
      if(id) {
         XMLRPC_SetValueID(val, id, 0);
      }
   }
   return val;
}

/*******/

/****f* VALUE/XMLRPC_CreateValueBoolean
 * NAME
 *   XMLRPC_CreateValueBoolean
 * SYNOPSIS
 *   XMLRPC_VALUE XMLRPC_CreateValueBoolean(const char* id, int i)
 * FUNCTION
 *   Create an XMLRPC_VALUE, and assign an int to it
 * INPUTS
 *   id        The id of the value, or NULL
 *   i         The desired new int val.
 * RESULT
 *   newly allocated XMLRPC_VALUE, or NULL
 * SEE ALSO
 *   XMLRPC_GetValueBoolean ()
 *   XMLRPC_CreateValueEmpty ()
 *   XMLRPC_VALUE
 *   XMLRPC_VALUE_TYPE
 * SOURCE
 */
XMLRPC_VALUE XMLRPC_CreateValueBoolean(const char* id, int i) {
   XMLRPC_VALUE val = XMLRPC_CreateValueEmpty();
   if(val) {
      XMLRPC_SetValueBoolean(val, i);
      if(id) {
         XMLRPC_SetValueID(val, id, 0);
      }
   }
   return val;
}

/*******/


/****f* VALUE/XMLRPC_CleanupValue
 * NAME
 *   XMLRPC_CleanupValue
 * SYNOPSIS
 *   void XMLRPC_CleanupValue(XMLRPC_VALUE value)
 * FUNCTION
 *   Frees all memory allocated for an XMLRPC_VALUE and any of its children (if a vector)
 * INPUTS
 *   value     The id of the value to be cleaned up.
 * RESULT
 *   void
 * NOTES
 *   Normally this function will be called for the topmost vector, thus free-ing
 *   all children.  If a child of a vector is free'd first, results are undefined.
 *   Failure to call this function *will* cause memory leaks.
 *
 *   Also, this function is implemented using reference counting.  Thus a value
 *   may be added and freed from multiple parents so long as a reference is added
 *   first using XMLRPC_CopyValue()
 * SEE ALSO
 *   XMLRPC_RequestFree ()
 *   XMLRPC_CreateValueEmpty ()
 *   XMLRPC_CopyValue()
 *   XMLRPC_VALUE
 * SOURCE
 */
void XMLRPC_CleanupValue(XMLRPC_VALUE value) {
   if(value) {
      if(value->iRefCount > 0) {
         value->iRefCount --;
      }

#ifdef XMLRPC_DEBUG_REFCOUNT
      if(value->id.str) {
			printf ("decremented refcount of %s, now %i\n", value->id.str,
					  value->iRefCount);
      }
      else {
			printf ("decremented refcount of 0x%x, now %i\n", value,
					  value->iRefCount);
      }
#endif

      if(value->type == xmlrpc_vector) {
         if(value->v) {
            if(value->iRefCount == 0) {
               XMLRPC_VALUE cur = (XMLRPC_VALUE)Q_Head(value->v->q);
               while( cur ) {
                  XMLRPC_CleanupValue(cur);

                  /* Make sure some idiot didn't include a vector as a child of itself
                   * and thus it would have already free'd these.
                   */
                  if(value->v && value->v->q) {
                     cur = Q_Next(value->v->q);
                  }
                  else {
                     break;
                  }
               }

               Q_Destroy(value->v->q);
               my_free(value->v->q);
               my_free(value->v);
            }
         }
      }


      if(value->iRefCount == 0) {

         /* guard against freeing invalid types */
         switch(value->type) {
            case xmlrpc_empty:
            case xmlrpc_base64:
            case xmlrpc_boolean:
            case xmlrpc_datetime:
            case xmlrpc_double:
            case xmlrpc_int:
            case xmlrpc_string:
            case xmlrpc_vector:
#ifdef XMLRPC_DEBUG_REFCOUNT
               if(value->id.str) {
                  printf("free'd %s\n", value->id.str);
               }
               else {
                  printf("free'd 0x%x\n", value);
               }
#endif
               simplestring_free(&value->id);
               simplestring_free(&value->str);

               memset(value, 0, sizeof(STRUCT_XMLRPC_VALUE));
               my_free(value);
               break;
            default:
				/* fprintf (stderr,
							"xmlrpc: attempted to free value of invalid type\n"); */
               break;
         }
      }
   }
}

/*******/


/****f* VALUE/XMLRPC_SetValueDateTime
 * NAME
 *   XMLRPC_SetValueDateTime
 * SYNOPSIS
 *   void XMLRPC_SetValueDateTime(XMLRPC_VALUE value, time_t time)
 * FUNCTION
 *   Assign time value to XMLRPC_VALUE
 * INPUTS
 *   value     The target XMLRPC_VALUE
 *   time      The desired new unix time value (time_t)
 * RESULT
 *   void
 * SEE ALSO
 *   XMLRPC_GetValueDateTime ()
 *   XMLRPC_SetValueDateTime_ISO8601 ()
 *   XMLRPC_CreateValueDateTime ()
 *   XMLRPC_VALUE
 * SOURCE
 */
void XMLRPC_SetValueDateTime(XMLRPC_VALUE value, time_t time) {
   if(value) {
      char timeBuf[30];
      value->type = xmlrpc_datetime;
      value->i = time;

      timeBuf[0] = 0;

      date_to_ISO8601(time, timeBuf, sizeof(timeBuf));

      if(timeBuf[0]) {
         XMLRPC_SetValueDateTime_ISO8601 (value, timeBuf);
      }
   }
}

/*******/

/****f* VALUE/XMLRPC_CopyValue
 * NAME
 *   XMLRPC_CopyValue
 * SYNOPSIS
 *   XMLRPC_VALUE XMLRPC_CopyValue(XMLRPC_VALUE value)
 * FUNCTION
 *   Make a copy (reference) of an XMLRPC_VALUE
 * INPUTS
 *   value     The target XMLRPC_VALUE
 * RESULT
 *   XMLRPC_VALUE -- address of the copy
 * SEE ALSO
 *   XMLRPC_CleanupValue ()
 *   XMLRPC_DupValueNew ()
 * NOTES
 *   This function is implemented via reference counting, so the
 *   returned value is going to be the same as the passed in value.
 *   The value must be freed the same number of times it is copied
 *   or there will be a memory leak.
 * SOURCE
 */
XMLRPC_VALUE XMLRPC_CopyValue(XMLRPC_VALUE value) {
   if(value) {
      value->iRefCount ++;
#ifdef XMLRPC_DEBUG_REFCOUNT
      if(value->id.str) {
			printf ("incremented refcount of %s, now %i\n", value->id.str,
					  value->iRefCount);
		}
		else {
			printf ("incremented refcount of 0x%x, now %i\n", value,
					  value->iRefCount);
      }
#endif
   }
   return value;
}

/*******/


/****f* VALUE/XMLRPC_DupValueNew
 * NAME
 *   XMLRPC_DupValueNew
 * SYNOPSIS
 *   XMLRPC_VALUE XMLRPC_DupValueNew(XMLRPC_VALUE value)
 * FUNCTION
 *   Make a duplicate (non reference) of an XMLRPC_VALUE with newly allocated mem.
 * INPUTS
 *   value     The source XMLRPC_VALUE to duplicate
 * RESULT
 *   XMLRPC_VALUE -- address of the duplicate value
 * SEE ALSO
 *   XMLRPC_CleanupValue ()
 *   XMLRPC_CopyValue ()
 * NOTES
 *   Use this when function when you need to modify the contents of
 *   the copied value separately from the original.
 *
 *   this function is recursive, thus the value and all of its children
 *   (if any) will be duplicated.
 * SOURCE
 */
XMLRPC_VALUE XMLRPC_DupValueNew (XMLRPC_VALUE xSource) {
	XMLRPC_VALUE xReturn = NULL;
	if (xSource) {
		xReturn = XMLRPC_CreateValueEmpty ();
		if (xSource->id.len) {
			XMLRPC_SetValueID (xReturn, xSource->id.str, xSource->id.len);
		}

		switch (xSource->type) {
		case xmlrpc_int:
		case xmlrpc_boolean:
			XMLRPC_SetValueInt (xReturn, xSource->i);
			break;
		case xmlrpc_string:
		case xmlrpc_base64:
			XMLRPC_SetValueString (xReturn, xSource->str.str, xSource->str.len);
			break;
		case xmlrpc_datetime:
			XMLRPC_SetValueDateTime (xReturn, xSource->i);
			break;
		case xmlrpc_double:
			XMLRPC_SetValueDouble (xReturn, xSource->d);
			break;
		case xmlrpc_vector:
			{
				q_iter qi = Q_Iter_Head_F (xSource->v->q);
				XMLRPC_SetIsVector (xReturn, xSource->v->type);

				while (qi) {
					XMLRPC_VALUE xIter = Q_Iter_Get_F (qi);
					XMLRPC_AddValueToVector (xReturn, XMLRPC_DupValueNew (xIter));
					qi = Q_Iter_Next_F (qi);
				}
			}
			break;
		default:
			break;
		}
	}
	return xReturn;
}

/*******/



/****f* VALUE/XMLRPC_CreateValueDateTime
 * NAME
 *   XMLRPC_CreateValueDateTime
 * SYNOPSIS
 *   XMLRPC_VALUE XMLRPC_CreateValueDateTime(const char* id, time_t time)
 * FUNCTION
 *   Create new datetime value from time_t
 * INPUTS
 *   id        id of the new value, or NULL
 *   time      The desired unix time value (time_t)
 * RESULT
 *   void
 * SEE ALSO
 *   XMLRPC_GetValueDateTime ()
 *   XMLRPC_SetValueDateTime ()
 *   XMLRPC_CreateValueDateTime_ISO8601 ()
 *   XMLRPC_VALUE
 * SOURCE
 */
XMLRPC_VALUE XMLRPC_CreateValueDateTime(const char* id, time_t time) {
   XMLRPC_VALUE val = XMLRPC_CreateValueEmpty();
   if(val) {
      XMLRPC_SetValueDateTime(val, time);
      if(id) {
         XMLRPC_SetValueID(val, id, 0);
      }
   }
   return val;
}

/*******/


/****f* VALUE/XMLRPC_SetValueDateTime_ISO8601
 * NAME
 *   XMLRPC_SetValueDateTime_ISO8601
 * SYNOPSIS
 *   void XMLRPC_SetValueDateTime_ISO8601(XMLRPC_VALUE value, const char* s)
 * FUNCTION
 *   Set datetime value from IS08601 encoded string
 * INPUTS
 *   value     The target XMLRPC_VALUE
 *   s         The desired new time value
 * RESULT
 *   void
 * BUGS
 *   This function currently attempts to convert the time string to a valid unix time
 *   value before passing it. Behavior when the string is invalid or out of range
 *   is not well defined, but will probably result in Jan 1, 1970 (0) being passed.
 * SEE ALSO
 *   XMLRPC_GetValueDateTime_ISO8601 ()
 *   XMLRPC_CreateValueDateTime_ISO8601 ()
 *   XMLRPC_CreateValueDateTime ()
 *   XMLRPC_VALUE
 * SOURCE
 */
void XMLRPC_SetValueDateTime_ISO8601(XMLRPC_VALUE value, const char* s) {
   if(value) {
      time_t time_val = 0;
      if(s) {
         value->type = xmlrpc_datetime;
         date_from_ISO8601(s, &time_val);
         value->i = time_val;
         simplestring_clear(&value->str);
         simplestring_add(&value->str, s);
      }
   }
}

/*******/

/****f* VALUE/XMLRPC_CreateValueDateTime_ISO8601
 * NAME
 *   XMLRPC_CreateValueDateTime_ISO8601
 * SYNOPSIS
 *   XMLRPC_VALUE XMLRPC_CreateValueDateTime_ISO8601(const char* id, const char *s)
 * FUNCTION
 *   Create datetime value from IS08601 encoded string
 * INPUTS
 *   id        The id of the new value, or NULL
 *   s         The desired new time value
 * RESULT
 *   newly allocated XMLRPC_VALUE, or NULL if no value created.
 * BUGS
 *   See XMLRPC_SetValueDateTime_ISO8601 ()
 * SEE ALSO
 *   XMLRPC_GetValueDateTime_ISO8601 ()
 *   XMLRPC_SetValueDateTime_ISO8601 ()
 *   XMLRPC_CreateValueDateTime ()
 *   XMLRPC_VALUE
 * SOURCE
 */
XMLRPC_VALUE XMLRPC_CreateValueDateTime_ISO8601(const char* id, const char *s) {
   XMLRPC_VALUE val = XMLRPC_CreateValueEmpty();
   if(val) {
      XMLRPC_SetValueDateTime_ISO8601(val, s);
      if(id) {
         XMLRPC_SetValueID(val, id, 0);
      }
   }
   return val;
}

/*******/


/****f* VALUE/XMLRPC_SetValueBase64
 * NAME
 *   XMLRPC_SetValueBase64
 * SYNOPSIS
 *   void XMLRPC_SetValueBase64(XMLRPC_VALUE value, const char* s, int len)
 * FUNCTION
 *   Set base64 value.  Base64 is useful for transferring binary data, such as an image.
 * INPUTS
 *   value     The target XMLRPC_VALUE
 *   s         The desired new binary value
 *   len       The length of s, or NULL. If buffer is not null terminated, len *must* be passed.
 * RESULT
 *   void
 * NOTES
 *   Data is set/stored/retrieved as passed in, but is base64 encoded for XML transfer, and
 *   decoded on the other side.  This is transparent to the caller.
 * SEE ALSO
 *   XMLRPC_GetValueBase64 ()
 *   XMLRPC_CreateValueBase64 ()
 *   XMLRPC_VALUE
 * SOURCE
 */
void XMLRPC_SetValueBase64(XMLRPC_VALUE value, const char* s, int len) {
   if(value && s) {
      simplestring_clear(&value->str);
      (len > 0) ? simplestring_addn(&value->str, s, len) :
                  simplestring_add(&value->str, s);
      value->type = xmlrpc_base64;
   }
}

/*******/


/****f* VALUE/XMLRPC_CreateValueBase64
 * NAME
 *   XMLRPC_CreateValueBase64
 * SYNOPSIS
 *   XMLRPC_VALUE XMLRPC_CreateValueBase64(const char* id, const char* s, int len)
 * FUNCTION
 *   Create base64 value.  Base64 is useful for transferring binary data, such as an image.
 * INPUTS
 *   id        id of the new value, or NULL
 *   s         The desired new binary value
 *   len       The length of s, or NULL. If buffer is not null terminated, len *must* be passed.
 * RESULT
 *   newly allocated XMLRPC_VALUE, or NULL if error
 * NOTES
 *   See XMLRPC_SetValueBase64 ()
 * SEE ALSO
 *   XMLRPC_GetValueBase64 ()
 *   XMLRPC_SetValueBase64 ()
 *   XMLRPC_VALUE
 * SOURCE
 */
XMLRPC_VALUE XMLRPC_CreateValueBase64(const char* id, const char* s, int len) {
   XMLRPC_VALUE val = XMLRPC_CreateValueEmpty();
   if(val) {
      XMLRPC_SetValueBase64(val, s, len);
      if(id) {
         XMLRPC_SetValueID(val, id, 0);
      }
   }
   return val;
}

/*******/

/****f* VALUE/XMLRPC_SetValueDouble
 * NAME
 *   XMLRPC_SetValueDouble
 * SYNOPSIS
 *   void XMLRPC_SetValueDouble(XMLRPC_VALUE value, double val)
 * FUNCTION
 *   Set double (floating point) value.
 * INPUTS
 *   value     The target XMLRPC_VALUE
 *   val       The desired new double value
 * RESULT
 *   void
 * SEE ALSO
 *   XMLRPC_GetValueDouble ()
 *   XMLRPC_CreateValueDouble ()
 *   XMLRPC_VALUE
 * SOURCE
 */
void XMLRPC_SetValueDouble(XMLRPC_VALUE value, double val) {
   if(value) {
      value->type = xmlrpc_double;
      value->d = val;
   }
}

/*******/

/****f* VALUE/XMLRPC_CreateValueDouble
 * NAME
 *   XMLRPC_CreateValueDouble
 * SYNOPSIS
 *   XMLRPC_VALUE XMLRPC_CreateValueDouble(const char* id, double d)
 * FUNCTION
 *   Create double (floating point) value.
 * INPUTS
 *   id        id of the newly created value, or NULL
 *   d         The desired new double value
 * RESULT
 *   void
 * SEE ALSO
 *   XMLRPC_GetValueDouble ()
 *   XMLRPC_CreateValueDouble ()
 *   XMLRPC_VALUE
 * SOURCE
 */
XMLRPC_VALUE XMLRPC_CreateValueDouble(const char* id, double d) {
   XMLRPC_VALUE val = XMLRPC_CreateValueEmpty();
   if(val) {
      XMLRPC_SetValueDouble(val, d);
      if(id) {
         XMLRPC_SetValueID(val, id, 0);
      }
   }
   return val;
}

/*******/

/****f* VALUE/XMLRPC_GetValueString
 * NAME
 *   XMLRPC_GetValueString
 * SYNOPSIS
 *   const char* XMLRPC_GetValueString(XMLRPC_VALUE value)
 * FUNCTION
 *   retrieve string value
 * INPUTS
 *   value     source XMLRPC_VALUE of type xmlrpc_string
 * RESULT
 *   void
 * SEE ALSO
 *   XMLRPC_SetValueString ()
 *   XMLRPC_GetValueType ()
 *   XMLRPC_VALUE
 * SOURCE
 */
const char* XMLRPC_GetValueString(XMLRPC_VALUE value) {
    return ((value && value->type == xmlrpc_string) ? value->str.str : 0);
}

/*******/

/****f* VALUE/XMLRPC_GetValueStringLen
 * NAME
 *   XMLRPC_GetValueStringLen
 * SYNOPSIS
 *   int XMLRPC_GetValueStringLen(XMLRPC_VALUE value)
 * FUNCTION
 *   determine length of string value
 * INPUTS
 *   value     XMLRPC_VALUE of type xmlrpc_string
 * RESULT
 *   length of string, or 0
 * NOTES
 * SEE ALSO
 *   XMLRPC_SetValueString ()
 *   XMLRPC_GetValueString ()
 * SOURCE
 */
int XMLRPC_GetValueStringLen(XMLRPC_VALUE value) {
    return ((value) ? value->str.len : 0);
}

/*******/

/****f* VALUE/XMLRPC_GetValueInt
 * NAME
 *   XMLRPC_GetValueInt
 * SYNOPSIS
 *   int XMLRPC_GetValueInt(XMLRPC_VALUE value)
 * FUNCTION
 *   retrieve integer value.
 * INPUTS
 *   value     XMLRPC_VALUE of type xmlrpc_int
 * RESULT
 *   integer value or 0 if value is not valid int
 * NOTES
 *   use XMLRPC_GetValueType () to be sure if 0 is real return value or not
 * SEE ALSO
 *   XMLRPC_SetValueInt ()
 *   XMLRPC_CreateValueInt ()
 * SOURCE
 */
int XMLRPC_GetValueInt(XMLRPC_VALUE value) {
    return ((value && value->type == xmlrpc_int) ? value->i : 0);
}

/*******/

/****f* VALUE/XMLRPC_GetValueBoolean
 * NAME
 *   XMLRPC_GetValueBoolean
 * SYNOPSIS
 *   int XMLRPC_GetValueBoolean(XMLRPC_VALUE value)
 * FUNCTION
 *   retrieve boolean value.
 * INPUTS
 *   XMLRPC_VALUE of type xmlrpc_boolean
 * RESULT
 *   boolean value or 0 if value is not valid boolean
 * NOTES
 *   use XMLRPC_GetValueType() to be sure if 0 is real value or not
 * SEE ALSO
 *   XMLRPC_SetValueBoolean ()
 *   XMLRPC_CreateValueBoolean ()
 * SOURCE
 */
int XMLRPC_GetValueBoolean(XMLRPC_VALUE value) {
    return ((value && value->type == xmlrpc_boolean) ? value->i : 0);
}

/*******/

/****f* VALUE/XMLRPC_GetValueDouble
 * NAME
 *   XMLRPC_GetValueDouble
 * SYNOPSIS
 *   double XMLRPC_GetValueDouble(XMLRPC_VALUE value)
 * FUNCTION
 *   retrieve double value
 * INPUTS
 *   XMLRPC_VALUE of type xmlrpc_double
 * RESULT
 *   double value or 0 if value is not valid double.
 * NOTES
 *   use XMLRPC_GetValueType() to be sure if 0 is real value or not
 * SEE ALSO
 *   XMLRPC_SetValueDouble ()
 *   XMLRPC_CreateValueDouble ()
 * SOURCE
 */
double XMLRPC_GetValueDouble(XMLRPC_VALUE value) {
    return ((value && value->type == xmlrpc_double) ? value->d : 0);
}

/*******/

/****f* VALUE/XMLRPC_GetValueBase64
 * NAME
 *   XMLRPC_GetValueBase64
 * SYNOPSIS
 *   const char* XMLRPC_GetValueBase64(XMLRPC_VALUE value)
 * FUNCTION
 *   retrieve binary value
 * INPUTS
 *   XMLRPC_VALUE of type xmlrpc_base64
 * RESULT
 *   pointer to binary value or 0 if value is not valid.
 * SEE ALSO
 *   XMLRPC_SetValueBase64 ()
 *   XMLRPC_CreateValueBase64 ()
 * NOTES
 *   Call XMLRPC_GetValueStringLen() to retrieve real length of binary data.  strlen()
 *   will not be accurate, as returned data may contain embedded nulls.
 * SOURCE
 */
const char* XMLRPC_GetValueBase64(XMLRPC_VALUE value) {
    return ((value && value->type == xmlrpc_base64) ? value->str.str : 0);
}

/*******/

/****f* VALUE/XMLRPC_GetValueDateTime
 * NAME
 *   XMLRPC_GetValueDateTime
 * SYNOPSIS
 *   time_t XMLRPC_GetValueDateTime(XMLRPC_VALUE value)
 * FUNCTION
 *   retrieve time_t value
 * INPUTS
 *   XMLRPC_VALUE of type xmlrpc_datetime
 * RESULT
 *   time_t value or 0 if value is not valid datetime.
 * NOTES
 *   use XMLRPC_GetValueType() to be sure if 0 is real value or not
 * SEE ALSO
 *   XMLRPC_SetValueDateTime ()
 *   XMLRPC_GetValueDateTime_ISO8601 ()
 *   XMLRPC_CreateValueDateTime ()
 * SOURCE
 */
time_t XMLRPC_GetValueDateTime(XMLRPC_VALUE value) {
    return (time_t)((value && value->type == xmlrpc_datetime) ? value->i : 0);
}

/*******/

/****f* VALUE/XMLRPC_GetValueDateTime_IOS8601
 * NAME
 *   XMLRPC_GetValueDateTime_IOS8601
 * SYNOPSIS
 *   const char* XMLRPC_GetValueDateTime_IOS8601(XMLRPC_VALUE value)
 * FUNCTION
 *   retrieve ISO8601 formatted time value
 * INPUTS
 *   XMLRPC_VALUE of type xmlrpc_datetime
 * RESULT
 *   const char* value or 0 if value is not valid datetime.
 * SEE ALSO
 *   XMLRPC_SetValueDateTime_IOS8601 ()
 *   XMLRPC_GetValueDateTime ()
 *   XMLRPC_CreateValueDateTime_IOS8601 ()
 * SOURCE
 */
const char* XMLRPC_GetValueDateTime_ISO8601(XMLRPC_VALUE value) {
    return ((value && value->type == xmlrpc_datetime) ? value->str.str : 0);
}

/*******/

/* Get ID (key) of value or NULL */
/****f* VALUE/XMLRPC_GetValueID
 * NAME
 *   XMLRPC_GetValueID
 * SYNOPSIS
 *   const char* XMLRPC_GetValueID(XMLRPC_VALUE value)
 * FUNCTION
 *   retrieve id (key) of value
 * INPUTS
 *   XMLRPC_VALUE of any type
 * RESULT
 *   const char* pointer to id of value, or NULL
 * NOTES
 * SEE ALSO
 *   XMLRPC_SetValueID()
 *   XMLRPC_CreateValueEmpty()
 * SOURCE
 */
const char* XMLRPC_GetValueID(XMLRPC_VALUE value) {
    return (const char*)((value && value->id.len) ? value->id.str : 0);
}

/*******/


/****f* VECTOR/XMLRPC_VectorSize
 * NAME
 *   XMLRPC_VectorSize
 * SYNOPSIS
 *   int XMLRPC_VectorSize(XMLRPC_VALUE value)
 * FUNCTION
 *   retrieve size of vector
 * INPUTS
 *   XMLRPC_VALUE of type xmlrpc_vector
 * RESULT
 *   count of items in vector
 * NOTES
 *   This is a cheap operation even on large vectors.  Vector size is
 *   maintained by queue during add/remove ops.
 * SEE ALSO
 *   XMLRPC_AddValueToVector ()
 * SOURCE
 */
int XMLRPC_VectorSize(XMLRPC_VALUE value) {
   int size = 0;
   if(value && value->type == xmlrpc_vector && value->v) {
      size = Q_Size(value->v->q);
   }
   return size;
}

/*******/

/****f* VECTOR/XMLRPC_VectorRewind
 * NAME
 *   XMLRPC_VectorRewind
 * SYNOPSIS
 *   XMLRPC_VALUE XMLRPC_VectorRewind(XMLRPC_VALUE value)
 * FUNCTION
 *   reset vector to first item
 * INPUTS
 *   XMLRPC_VALUE of type xmlrpc_vector
 * RESULT
 *   first XMLRPC_VALUE in list, or NULL if empty or error.
 * NOTES
 *   Be careful to rewind any vector passed in to you if you expect to
 *   iterate through the entire list.
 * SEE ALSO
 *   XMLRPC_VectorNext ()
 * SOURCE
 */
XMLRPC_VALUE XMLRPC_VectorRewind(XMLRPC_VALUE value) {
   XMLRPC_VALUE xReturn = NULL;
   if(value && value->type == xmlrpc_vector && value->v) {
      xReturn = (XMLRPC_VALUE)Q_Head(value->v->q);
   }
   return xReturn;
}

/*******/

/****f* VECTOR/XMLRPC_VectorNext
 * NAME
 *   XMLRPC_VectorNext
 * SYNOPSIS
 *   XMLRPC_VALUE XMLRPC_VectorNext(XMLRPC_VALUE value)
 * FUNCTION
 *   Iterate vector to next item in list.
 * INPUTS
 *   XMLRPC_VALUE of type xmlrpc_vector
 * RESULT
 *   Next XMLRPC_VALUE in vector, or NULL if at end.
 * NOTES
 * SEE ALSO
 *   XMLRPC_VectorRewind ()
 * SOURCE
 */
XMLRPC_VALUE XMLRPC_VectorNext(XMLRPC_VALUE value) {
   XMLRPC_VALUE xReturn = NULL;
   if(value && value->type == xmlrpc_vector && value->v) {
      xReturn = (XMLRPC_VALUE)Q_Next(value->v->q);
   }
   return xReturn;
}

/*******/

/****f* VALUE/XMLRPC_GetValueType
 * NAME
 *   XMLRPC_GetValueType
 * SYNOPSIS
 *   XMLRPC_VALUE_TYPE XMLRPC_GetValueType(XMLRPC_VALUE value)
 * FUNCTION
 *   determine data type of the XMLRPC_VALUE
 * INPUTS
 *   XMLRPC_VALUE target of query
 * RESULT
 *   data type of value as enumerated by XMLRPC_VALUE_TYPE
 * NOTES
 *   all values are of type xmlrpc_empty until set.
 *   Deprecated for public use.  See XMLRPC_GetValueTypeEasy
 * SEE ALSO
 *   XMLRPC_SetValue*
 *   XMLRPC_CreateValue*
 *   XMLRPC_Append*
 *   XMLRPC_GetValueTypeEasy ()
 * SOURCE
 */
XMLRPC_VALUE_TYPE XMLRPC_GetValueType(XMLRPC_VALUE value) {
   return value ? value->type : xmlrpc_empty;
}

/*******/

/* Vector type accessor */
/****f* VALUE/XMLRPC_GetVectorType
 * NAME
 *   XMLRPC_GetVectorType
 * SYNOPSIS
 *   XMLRPC_VECTOR_TYPE XMLRPC_GetVectorType(XMLRPC_VALUE value)
 * FUNCTION
 *   determine vector type of the XMLRPC_VALUE
 * INPUTS
 *   XMLRPC_VALUE of type xmlrpc_vector
 * RESULT
 *   vector type of value as enumerated by XMLRPC_VECTOR_TYPE.
 *   xmlrpc_none if not a value.
 * NOTES
 *   xmlrpc_none is returned if value is not a vector
 *   Deprecated for public use.  See XMLRPC_GetValueTypeEasy
 * SEE ALSO
 *   XMLRPC_SetIsVector ()
 *   XMLRPC_GetValueType ()
 *   XMLRPC_GetValueTypeEasy ()
 * SOURCE
 */
XMLRPC_VECTOR_TYPE XMLRPC_GetVectorType(XMLRPC_VALUE value) {
   return(value && value->v) ? value->v->type : xmlrpc_none;
}

/*******/

/****f* VALUE/XMLRPC_GetValueTypeEasy
 * NAME
 *   XMLRPC_GetValueTypeEasy
 * SYNOPSIS
 *   XMLRPC_VALUE_TYPE_EASY XMLRPC_GetValueTypeEasy(XMLRPC_VALUE value)
 * FUNCTION
 *   determine data type of the XMLRPC_VALUE. includes vector types.
 * INPUTS
 *   XMLRPC_VALUE target of query
 * RESULT
 *   data type of value as enumerated by XMLRPC_VALUE_TYPE_EASY
 *   xmlrpc_type_none if not a value.
 * NOTES
 *   all values are of type xmlrpc_type_empty until set.
 * SEE ALSO
 *   XMLRPC_SetValue*
 *   XMLRPC_CreateValue*
 *   XMLRPC_Append*
 * SOURCE
 */
XMLRPC_VALUE_TYPE_EASY XMLRPC_GetValueTypeEasy (XMLRPC_VALUE value) {
	if (value) {
		switch (value->type) {
		case xmlrpc_vector:
			switch (value->v->type) {
			case xmlrpc_vector_none:
				return xmlrpc_type_none;
			case xmlrpc_vector_struct:
				return xmlrpc_type_struct;
			case xmlrpc_vector_mixed:
				return xmlrpc_type_mixed;
			case xmlrpc_vector_array:
				return xmlrpc_type_array;
			}
		default:
			/* evil cast, but we know they are the same */
			return(XMLRPC_VALUE_TYPE_EASY) value->type;
		}
	}
	return xmlrpc_none;
}

/*******/



/*-*******************
* Begin Server Funcs *
*********************/


/****f* VALUE/XMLRPC_ServerCreate
 * NAME
 *   XMLRPC_ServerCreate
 * SYNOPSIS
 *   XMLRPC_SERVER XMLRPC_ServerCreate()
 * FUNCTION
 *   Allocate/Init XMLRPC Server Resources.
 * INPUTS
 *   none
 * RESULT
 *   newly allocated XMLRPC_SERVER
 * NOTES
 * SEE ALSO
 *   XMLRPC_ServerDestroy ()
 *   XMLRPC_GetGlobalServer ()
 * SOURCE
 */
XMLRPC_SERVER XMLRPC_ServerCreate() {
   XMLRPC_SERVER server = ecalloc(1, sizeof(STRUCT_XMLRPC_SERVER));
   if(server) {
      Q_Init(&server->methodlist);
      Q_Init(&server->docslist);

      /* register system methods */
      xsm_register(server);
   }
   return server;
}

/*******/

/* Return global server.  Not locking! Not Thread Safe! */
/****f* VALUE/XMLRPC_GetGlobalServer
 * NAME
 *   XMLRPC_GetGlobalServer
 * SYNOPSIS
 *   XMLRPC_SERVER XMLRPC_GetGlobalServer()
 * FUNCTION
 *   Allocates a global (process-wide) server, or returns pointer if pre-existing.
 * INPUTS
 *   none
 * RESULT
 *   pointer to global server, or 0 if error.
 * NOTES
 *   ***WARNING*** This function is not thread safe.  It is included only for the very lazy.
 *   Multi-threaded programs that use this may experience problems.
 * BUGS
 *   There is currently no way to cleanup the global server gracefully.
 * SEE ALSO
 *   XMLRPC_ServerCreate ()
 * SOURCE
 */
XMLRPC_SERVER XMLRPC_GetGlobalServer() {
   static XMLRPC_SERVER xsServer = 0;
   if(!xsServer) {
      xsServer = XMLRPC_ServerCreate();
   }
   return xsServer;
}

/*******/

/****f* VALUE/XMLRPC_ServerDestroy
 * NAME
 *   XMLRPC_ServerDestroy
 * SYNOPSIS
 *   void XMLRPC_ServerDestroy(XMLRPC_SERVER server)
 * FUNCTION
 *   Free Server Resources
 * INPUTS
 *   server     The server to be free'd
 * RESULT
 *   void
 * NOTES
 *   This frees the server struct and any methods that have been added.
 * SEE ALSO
 *   XMLRPC_ServerCreate ()
 * SOURCE
 */
void XMLRPC_ServerDestroy(XMLRPC_SERVER server) {
   if(server) {
      doc_method* dm = Q_Head(&server->docslist);
      server_method* sm = Q_Head(&server->methodlist);
      while( dm ) {
         my_free(dm);
         dm = Q_Next(&server->docslist);
      }
      while( sm ) {
         my_free(sm->name);
         if(sm->desc) {
            XMLRPC_CleanupValue(sm->desc);
         }
         my_free(sm);
         sm = Q_Next(&server->methodlist);
      }
      if (server->xIntrospection) {
         XMLRPC_CleanupValue(server->xIntrospection);
      }

      Q_Destroy(&server->methodlist);
      Q_Destroy(&server->docslist);
      my_free(server);
   }
}

/*******/


/****f* VALUE/XMLRPC_ServerRegisterMethod
 * NAME
 *   XMLRPC_ServerRegisterMethod
 * SYNOPSIS
 *   void XMLRPC_ServerRegisterMethod(XMLRPC_SERVER server, const char *name, XMLRPC_Callback cb)
 * FUNCTION
 *   Register new XMLRPC method with server
 * INPUTS
 *   server     The XMLRPC_SERVER to register the method with
 *   name       public name of the method
 *   cb         C function that implements the method
 * RESULT
 *   int  - 1 if success, else 0
 * NOTES
 *   A C function must be registered for every "method" that the server recognizes.  The
 *   method name is equivalent to <methodCall><name> method name </name></methodCall> in the
 *   XML syntax.
 * SEE ALSO
 *   XMLRPC_ServerFindMethod ()
 *   XMLRPC_ServerCallMethod ()
 * SOURCE
 */
int XMLRPC_ServerRegisterMethod(XMLRPC_SERVER server, const char *name, XMLRPC_Callback cb) {
   if(server && name && cb) {

      server_method* sm = emalloc(sizeof(server_method));

      if(sm) {
         sm->name = estrdup(name);
         sm->method = cb;
         sm->desc = NULL;

         return Q_PushTail(&server->methodlist, sm);
      }
   }
   return 0;
}

/*******/

server_method* find_method(XMLRPC_SERVER server, const char* name) {
   server_method* sm;

   q_iter qi = Q_Iter_Head_F(&server->methodlist);

   while( qi ) {
      sm = Q_Iter_Get_F(qi);
      if(sm && !strcmp(sm->name, name)) {
         return sm;
      }
      qi = Q_Iter_Next_F(qi);
   }
   return NULL;
}


const char* type_to_str(XMLRPC_VALUE_TYPE type, XMLRPC_VECTOR_TYPE vtype) {
    switch(type) {
       case xmlrpc_none:
          return "none";
       case xmlrpc_empty:
          return "empty";
       case xmlrpc_base64:
          return "base64";
       case xmlrpc_boolean:
          return "boolean";
       case xmlrpc_datetime:
          return "datetime";
       case xmlrpc_double:
          return "double";
       case xmlrpc_int:
          return "int";
       case xmlrpc_string:
          return "string";
       case xmlrpc_vector:
          switch(vtype) {
             case xmlrpc_vector_none:
                return "none";
             case xmlrpc_vector_array:
                return "array";
             case xmlrpc_vector_mixed:
                return "mixed vector (struct)";
             case xmlrpc_vector_struct:
                return "struct";
          }
    }
    return "unknown";
}

/****f* VALUE/XMLRPC_ServerFindMethod
 * NAME
 *   XMLRPC_ServerFindMethod
 * SYNOPSIS
 *   XMLRPC_Callback XMLRPC_ServerFindMethod(XMLRPC_SERVER server, const char* callName)
 * FUNCTION
 *   retrieve C callback associated with a given method name.
 * INPUTS
 *   server     The XMLRPC_SERVER the method is registered with
 *   callName   the method to find
 * RESULT
 *   previously registered XMLRPC_Callback, or NULL
 * NOTES
 *   Typically, this is used to determine if a requested method exists, without actually calling it.
 * SEE ALSO
 *   XMLRPC_ServerCallMethod ()
 *   XMLRPC_ServerRegisterMethod ()
 * SOURCE
 */
XMLRPC_Callback XMLRPC_ServerFindMethod(XMLRPC_SERVER server, const char* callName) {
   if(server && callName) {
      q_iter qi = Q_Iter_Head_F(&server->methodlist);
      while( qi ) {
         server_method* sm = Q_Iter_Get_F(qi);
         if(sm && !strcmp(sm->name, callName)) {
            return sm->method;
         }
         qi = Q_Iter_Next_F(qi);
      }
   }
   return NULL;
}

/*******/


/* Call method specified in request */
/****f* VALUE/XMLRPC_ServerCallMethod
 * NAME
 *   XMLRPC_ServerCallMethod
 * SYNOPSIS
 *   XMLRPC_VALUE XMLRPC_ServerCallMethod(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData)
 * FUNCTION
 *
 * INPUTS
 *   server     The XMLRPC_SERVER the method is registered with
 *   request    the request to handle
 *   userData   any additional data to pass to the C callback, or NULL
 * RESULT
 *   XMLRPC_VALUE allocated by the callback, or NULL
 * NOTES
 *   It is typically the caller's responsibility to free the returned value.
 *
 *   Often the caller will want to serialize the result as XML, via
 *   XMLRPC_VALUE_To_XML () or XMLRPC_REQUEST_To_XML ()
 * SEE ALSO
 *   XMLRPC_ServerFindMethod ()
 *   XMLRPC_ServerRegisterMethod ()
 *   XMLRPC_CleanupValue ()
 * SOURCE
 */
XMLRPC_VALUE XMLRPC_ServerCallMethod(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData) {
   XMLRPC_VALUE xReturn = NULL;

   /* check for error set during request parsing / generation */
   if(request && request->error) {
      xReturn = XMLRPC_CopyValue(request->error);
   }
	else if (server && request) {
		XMLRPC_Callback cb =
		XMLRPC_ServerFindMethod (server, request->methodName.str);
      if(cb) {
         xReturn = cb(server, request, userData);
      }
      else {
			xReturn =
			XMLRPC_UtilityCreateFault (xmlrpc_error_unknown_method,
												request->methodName.str);
      }
   }
   return xReturn;
}

/*******/

/*-*****************
* End server funcs *
*******************/


/*-***********************************
* Begin XMLRPC General Options funcs *
*************************************/

/* For options used by XMLRPC_VALUE funcs that otherwise do not have
 * parameters for options.  Kind of gross.  :(
 */
typedef struct _xmlrpc_options {
   XMLRPC_CASE id_case;
   XMLRPC_CASE_COMPARISON id_case_compare;
}
STRUCT_XMLRPC_OPTIONS, *XMLRPC_OPTIONS;

static XMLRPC_OPTIONS XMLRPC_GetDefaultOptions() {
   static STRUCT_XMLRPC_OPTIONS options = {
      xmlrpc_case_exact,
      xmlrpc_case_sensitive
   };
   return &options;
}

/****f* VALUE/XMLRPC_GetDefaultIdCase
 * NAME
 *   XMLRPC_GetDefaultIdCase
 * SYNOPSIS
 *   XMLRPC_CASE XMLRPC_GetDefaultIdCase()
 * FUNCTION
 *   Gets default case options used by XMLRPC_VALUE funcs
 * INPUTS
 *   none
 * RESULT
 *   XMLRPC_CASE
 * BUGS
 *   Nasty and gross.  Should be server specific, but that requires changing all
 *  the XMLRPC_VALUE api's.
 * SEE ALSO
 *   XMLRPC_SetDefaultIdCase ()
 * SOURCE
 */
XMLRPC_CASE XMLRPC_GetDefaultIdCase() {
   XMLRPC_OPTIONS options = XMLRPC_GetDefaultOptions();
   return options->id_case;
}

/*******/

/****f* VALUE/XMLRPC_SetDefaultIdCase
 * NAME
 *   XMLRPC_SetDefaultIdCase
 * SYNOPSIS
 *   XMLRPC_CASE XMLRPC_SetDefaultIdCase(XMLRPC_CASE id_case)
 * FUNCTION
 *   Sets default case options used by XMLRPC_VALUE funcs
 * INPUTS
 *   id_case   case options as enumerated by XMLRPC_CASE
 * RESULT
 *   XMLRPC_CASE -- newly set option
 * BUGS
 *   Nasty and gross.  Should be server specific, but that requires changing all
 *  the XMLRPC_VALUE api's.
 * SEE ALSO
 *   XMLRPC_GetDefaultIdCase ()
 * SOURCE
 */
XMLRPC_CASE XMLRPC_SetDefaultIdCase(XMLRPC_CASE id_case) {
   XMLRPC_OPTIONS options = XMLRPC_GetDefaultOptions();
   options->id_case = id_case;
   return options->id_case;
}

/*******/

/****f* VALUE/XMLRPC_GetDefaultIdCaseComparison
 * NAME
 *   XMLRPC_GetDefaultIdCaseComparison
 * SYNOPSIS
 *   XMLRPC_CASE XMLRPC_GetDefaultIdCaseComparison( )
 * FUNCTION
 *   Gets default case comparison options used by XMLRPC_VALUE funcs
 * INPUTS
 *   none
 * RESULT
 *   XMLRPC_CASE_COMPARISON default
 * BUGS
 *   Nasty and gross.  Should be server specific, but that requires changing all
 *  the XMLRPC_VALUE api's.
 * SEE ALSO
 *   XMLRPC_SetDefaultIdCaseComparison ()
 * SOURCE
 */
XMLRPC_CASE_COMPARISON XMLRPC_GetDefaultIdCaseComparison() {
   XMLRPC_OPTIONS options = XMLRPC_GetDefaultOptions();
   return options->id_case_compare;
}

/*******/

/****f* VALUE/XMLRPC_SetDefaultIdCaseComparison
 * NAME
 *   XMLRPC_SetDefaultIdCaseComparison
 * SYNOPSIS
 *   XMLRPC_CASE XMLRPC_SetDefaultIdCaseComparison( XMLRPC_CASE_COMPARISON id_case_compare )
 * FUNCTION
 *   Gets default case comparison options used by XMLRPC_VALUE funcs
 * INPUTS
 *   id_case_compare  case comparison rule to set as default
 * RESULT
 *   XMLRPC_CASE_COMPARISON newly set default
 * BUGS
 *   Nasty and gross.  Should be server specific, but that requires changing all
 *  the XMLRPC_VALUE api's.
 * SEE ALSO
 *   XMLRPC_GetDefaultIdCaseComparison ()
 * SOURCE
 */
XMLRPC_CASE_COMPARISON XMLRPC_SetDefaultIdCaseComparison(XMLRPC_CASE_COMPARISON id_case_compare) {
   XMLRPC_OPTIONS options = XMLRPC_GetDefaultOptions();
   options->id_case_compare = id_case_compare;
   return options->id_case_compare;
}

/*******/

/*-*********************************
* End XMLRPC General Options funcs *
***********************************/


/*-******************
* Fault API funcs   *
********************/

/****f* UTILITY/XMLRPC_UtilityCreateFault
 * NAME
 *   XMLRPC_UtilityCreateFault
 * SYNOPSIS
 *   XMLRPC_VALUE XMLRPC_UtilityCreateFault( int fault_code, const char* fault_string )
 * FUNCTION
 *   generates a struct containing a string member with id "faultString" and an int member
 *   with id "faultCode". When using the xmlrpc xml serialization, these will be translated
 *   to <fault><value><struct>... format.
 * INPUTS
 *   fault_code     application specific error code. can be 0.
 *   fault_string   application specific error string.  cannot be null.
 * RESULT
 *   XMLRPC_VALUE a newly created struct vector representing the error, or null on error.
 * NOTES
 *   This is a utility function. xmlrpc "faults" are not directly represented in this xmlrpc
 *   API or data structures. It is the author's view, that this API is intended for simple
 *   data types, and a "fault" is a complex data type consisting of multiple simple data
 *   types.  This function is provided for convenience only, the same result could be
 *   achieved directly by the application.
 *
 *   This function now supports some "standardized" fault codes, as specified at.
 *   http://xmlrpc-epi.sourceforge.net/specs/rfc.fault_codes.php.
 *   If one of these fault codes is received, the description string will automatically
 *   be prefixed with a standard error string and 2 newlines.
 *
 *   The actual transformation between this complex type and the xml "<fault>" element takes
 *   place in the xmlrpc to xml serialization layer.  This step is not performed when using the
 *   simplerpc serialization, meaning that there will be no "<fault>" element in that
 *   serialization. There will simply be a standard struct with 2 child elements.
 *   imho, the "<fault>" element is unnecessary and/or out of place as part of the standard API.
 *
 * SOURCE
 */
XMLRPC_VALUE XMLRPC_UtilityCreateFault(int fault_code, const char* fault_string) {
   XMLRPC_VALUE xOutput = NULL;

   char* string = NULL;
   simplestring description;
   simplestring_init(&description);

   switch (fault_code) {
	case xmlrpc_error_parse_xml_syntax:
		string = xmlrpc_error_parse_xml_syntax_str;
		break;
	case xmlrpc_error_parse_unknown_encoding:
		string = xmlrpc_error_parse_unknown_encoding_str;
		break;
	case xmlrpc_error_parse_bad_encoding:
		string = xmlrpc_error_parse_bad_encoding_str;
		break;
	case xmlrpc_error_invalid_xmlrpc:
		string = xmlrpc_error_invalid_xmlrpc_str;
		break;
	case xmlrpc_error_unknown_method:
		string = xmlrpc_error_unknown_method_str;
		break;
	case xmlrpc_error_invalid_params:
		string = xmlrpc_error_invalid_params_str;
		break;
	case xmlrpc_error_internal_server:
		string = xmlrpc_error_internal_server_str;
		break;
	case xmlrpc_error_application:
		string = xmlrpc_error_application_str;
		break;
	case xmlrpc_error_system:
		string = xmlrpc_error_system_str;
		break;
	case xmlrpc_error_transport:
		string = xmlrpc_error_transport_str;
		break;
   }

   simplestring_add(&description, string);

   if(string && fault_string) {
      simplestring_add(&description, "\n\n");
   }
   simplestring_add(&description, fault_string);


   if(description.len) {
      xOutput = XMLRPC_CreateVector(NULL, xmlrpc_vector_struct);

		XMLRPC_VectorAppendString (xOutput, "faultString", description.str,
											description.len);
      XMLRPC_VectorAppendInt(xOutput, "faultCode", fault_code);
   }

   simplestring_free(&description);

   return xOutput;
}

/*******/


/****f* FAULT/XMLRPC_ValueIsFault
 * NAME
 *   XMLRPC_ValueIsFault
 * SYNOPSIS
 *   int XMLRPC_ValueIsFault (XMLRPC_VALUE value)
 * FUNCTION
 *   Determines if a value encapsulates a fault "object"
 * INPUTS
 *   value  any XMLRPC_VALUE
 * RESULT
 *   1 if it is a fault, else 0
 * SEE ALSO
 *   XMLRPC_ResponseIsFault ()
 * SOURCE
 */
int XMLRPC_ValueIsFault (XMLRPC_VALUE value) {
   if( XMLRPC_VectorGetValueWithID(value, "faultCode") &&
       XMLRPC_VectorGetValueWithID(value, "faultString") ) {
      return 1;
   }
   return 0;
}
/*******/


/****f* FAULT/XMLRPC_ResponseIsFault
 * NAME
 *   XMLRPC_ResponseIsFault
 * SYNOPSIS
 *   int XMLRPC_ResponseIsFault (XMLRPC_REQUEST response)
 * FUNCTION
 *   Determines if a response contains an encapsulated fault "object"
 * INPUTS
 *   value  any XMLRPC_REQUEST. typically of type xmlrpc_request_response
 * RESULT
 *   1 if it contains a fault, else 0
 * SEE ALSO
 *   XMLRPC_ValueIsFault ()
 * SOURCE
 */
int XMLRPC_ResponseIsFault(XMLRPC_REQUEST response) {
   return XMLRPC_ValueIsFault( XMLRPC_RequestGetData(response) );
}

/*******/

/****f* FAULT/XMLRPC_GetValueFaultCode
 * NAME
 *   XMLRPC_GetValueFaultCode
 * SYNOPSIS
 *   int XMLRPC_GetValueFaultCode (XMLRPC_VALUE value)
 * FUNCTION
 *   returns fault code from a struct, if any
 * INPUTS
 *   value  XMLRPC_VALUE of type xmlrpc_vector_struct.
 * RESULT
 *   fault code, else 0.
 * BUGS
 *   impossible to distinguish faultCode == 0 from faultCode not present.
 * SEE ALSO
 *   XMLRPC_GetResponseFaultCode ()
 * SOURCE
 */
int XMLRPC_GetValueFaultCode (XMLRPC_VALUE value) {
   return XMLRPC_VectorGetIntWithID(value, "faultCode");
}

/*******/

/****f* FAULT/XMLRPC_GetResponseFaultCode
 * NAME
 *   XMLRPC_GetResponseFaultCode
 * SYNOPSIS
 *   int XMLRPC_GetResponseFaultCode(XMLRPC_REQUEST response)
 * FUNCTION
 *   returns fault code from a response, if any
 * INPUTS
 *   response  XMLRPC_REQUEST. typically of type xmlrpc_request_response.
 * RESULT
 *   fault code, else 0.
 * BUGS
 *   impossible to distinguish faultCode == 0 from faultCode not present.
 * SEE ALSO
 *   XMLRPC_GetValueFaultCode ()
 * SOURCE
 */
int XMLRPC_GetResponseFaultCode(XMLRPC_REQUEST response) {
   return XMLRPC_GetValueFaultCode( XMLRPC_RequestGetData(response) );
}

/*******/


/****f* FAULT/XMLRPC_GetValueFaultString
 * NAME
 *   XMLRPC_GetValueFaultString
 * SYNOPSIS
 *   const char* XMLRPC_GetValueFaultString (XMLRPC_VALUE value)
 * FUNCTION
 *   returns fault string from a struct, if any
 * INPUTS
 *   value  XMLRPC_VALUE of type xmlrpc_vector_struct.
 * RESULT
 *   fault string, else 0.
 * SEE ALSO
 *   XMLRPC_GetResponseFaultString ()
 * SOURCE
 */
const char* XMLRPC_GetValueFaultString (XMLRPC_VALUE value) {
   return XMLRPC_VectorGetStringWithID(value, "faultString");
}

/*******/

/****f* FAULT/XMLRPC_GetResponseFaultString
 * NAME
 *   XMLRPC_GetResponseFaultString
 * SYNOPSIS
 *   const char* XMLRPC_GetResponseFaultString (XMLRPC_REQUEST response)
 * FUNCTION
 *   returns fault string from a response, if any
 * INPUTS
 *   response  XMLRPC_REQUEST. typically of type xmlrpc_request_response.
 * RESULT
 *   fault string, else 0.
 * SEE ALSO
 *   XMLRPC_GetValueFaultString ()
 * SOURCE
 */
const char* XMLRPC_GetResponseFaultString (XMLRPC_REQUEST response) {
   return XMLRPC_GetValueFaultString( XMLRPC_RequestGetData(response) );
}

/*******/


/*-******************
* Utility API funcs *
********************/


/****f* UTILITY/XMLRPC_Free
 * NAME
 *   XMLRPC_Free
 * SYNOPSIS
 *   void XMLRPC_Free(void* mem)
 * FUNCTION
 *   frees a block of memory allocated by xmlrpc.
 * INPUTS
 *   mem    memory to free
 * RESULT
 *   void
 * NOTES
 *   Useful for OS's where memory must be free'd
 *   in the same library in which it is allocated.
 * SOURCE
 */
void XMLRPC_Free(void* mem) {
   my_free(mem);
}

/*******/


/****f* UTILITY/XMLRPC_GetVersionString
 * NAME
 *   XMLRPC_GetVersionString
 * SYNOPSIS
 *   const char* XMLRPC_GetVersionString()
 * FUNCTION
 *   returns library version string
 * INPUTS
 *
 * RESULT
 *   const char*
 * NOTES
 * SOURCE
 */
const char*  XMLRPC_GetVersionString() {
   return XMLRPC_VERSION_STR;
}

/*******/


/*-**********************
* End Utility API funcs *
************************/
