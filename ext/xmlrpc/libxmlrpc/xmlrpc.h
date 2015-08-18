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

#ifndef XMLRPC_ALREADY_INCLUDED
#define XMLRPC_ALREADY_INCLUDED 1

/* includes */
#include "xml_element.h"
#include <time.h> /* for time_t */

#ifdef __cplusplus
extern "C" {
#endif

/* allow version to be specified via compile line define */
#ifndef XMLRPC_LIB_VERSION
 #define XMLRPC_LIB_VERSION "0.51"
#endif

/* this number, representing the date, must be increased each time the API changes */
#define XMLRPC_API_NO 20020623

/* this string should be changed with each packaged release */
#define XMLRPC_VERSION_STR "xmlrpc-epi v. " XMLRPC_LIB_VERSION

/* where to find more info. shouldn't need to change much */
#define XMLRPC_HOME_PAGE_STR "http://xmlprc-epi.sourceforge.net/"


/****d* VALUE/XMLRPC_VALUE_TYPE
 * NAME
 *   XMLRPC_VALUE_TYPE
 * NOTES
 *   Defines data types for XMLRPC_VALUE
 *   Deprecated for public use.  See XMLRPC_VALUE_TYPE_EASY
 * SEE ALSO
 *   XMLRPC_VECTOR_TYPE
 *   XMLRPC_REQUEST_TYPE
 * SOURCE
 */
typedef enum _XMLRPC_VALUE_TYPE {
   xmlrpc_none,                   /* not a value                    */
   xmlrpc_empty,                  /* empty value, eg NULL           */
   xmlrpc_base64,                 /* base64 value, eg binary data   */
   xmlrpc_boolean,                /* boolean  [0 | 1]               */
   xmlrpc_datetime,               /* datetime [ISO8601 | time_t]    */
   xmlrpc_double,                 /* double / floating point        */
   xmlrpc_int,                    /* integer                        */
   xmlrpc_string,                 /* string                         */
   xmlrpc_vector                  /* vector, aka list, array        */
} XMLRPC_VALUE_TYPE;
/*******/

/****d* VALUE/XMLRPC_VECTOR_TYPE
 * NAME
 *   XMLRPC_VECTOR_TYPE
 * NOTES
 *   Defines data types for XMLRPC_VECTOR.
 *   Deprecated for public use.  See XMLRPC_VALUE_TYPE_EASY
 * SEE ALSO
 *   XMLRPC_VALUE_TYPE
 *   XMLRPC_REQUEST_TYPE
 * SOURCE
 */
typedef enum _XMLRPC_VECTOR_TYPE {
   xmlrpc_vector_none,            /* not an array                   */
   xmlrpc_vector_array,           /* no values may have key names   */
   xmlrpc_vector_mixed,           /* some values may have key names */
   xmlrpc_vector_struct           /* all values must have key names */
} XMLRPC_VECTOR_TYPE;
/*******/

/****d* VALUE/XMLRPC_VALUE_TYPE_EASY
 * NAME
 *   XMLRPC_VALUE_TYPE_EASY
 * NOTES
 *   Defines data types for XMLRPC_VALUE, including vector types.
 * SEE ALSO
 *   XMLRPC_VECTOR_TYPE
 *   XMLRPC_REQUEST_TYPE
 * SOURCE
 */
typedef enum _XMLRPC_VALUE_TYPE_EASY {
   xmlrpc_type_none,               /* not a value                    */
   xmlrpc_type_empty,              /* empty value, eg NULL           */
   xmlrpc_type_base64,             /* base64 value, eg binary data   */
   xmlrpc_type_boolean,            /* boolean  [0 | 1]               */
   xmlrpc_type_datetime,           /* datetime [ISO8601 | time_t]    */
   xmlrpc_type_double,             /* double / floating point        */
   xmlrpc_type_int,                /* integer                        */
   xmlrpc_type_string,             /* string                         */
/* -- IMPORTANT: identical to XMLRPC_VALUE_TYPE to this point. --   */
	xmlrpc_type_array,              /* vector array                   */
	xmlrpc_type_mixed,              /* vector mixed                   */
	xmlrpc_type_struct              /* vector struct                  */
} XMLRPC_VALUE_TYPE_EASY;
/*******/


/****d* VALUE/XMLRPC_REQUEST_TYPE
 * NAME
 *   XMLRPC_REQUEST_TYPE
 * NOTES
 *   Defines data types for XMLRPC_REQUEST
 * SEE ALSO
 *   XMLRPC_VALUE_TYPE
 *   XMLRPC_VECTOR_TYPE
 * SOURCE
 */
typedef enum _xmlrpc_request_type {
   xmlrpc_request_none,          /* not a valid request            */
   xmlrpc_request_call,          /* calling/invoking a method      */
   xmlrpc_request_response,      /* responding to a method call    */
} XMLRPC_REQUEST_TYPE;
/*******/

/****d* VALUE/XMLRPC_ERROR_CODE
 * NAME
 *   XMLRPC_ERROR_CODE
 * NOTES
 *   All existing error codes
 * SEE ALSO
 *   XMLRPC_REQUEST_ERROR
 * SOURCE
 */
typedef enum _xmlrpc_error_code {
   xmlrpc_error_none                      = 0,              /* not an error                                      */
   xmlrpc_error_parse_xml_syntax          = -32700,
   xmlrpc_error_parse_unknown_encoding    = -32701,
   xmlrpc_error_parse_bad_encoding        = -32702,
   xmlrpc_error_invalid_xmlrpc            = -32600,
   xmlrpc_error_unknown_method            = -32601,
   xmlrpc_error_invalid_params            = -32602,
   xmlrpc_error_internal_server           = -32603,
   xmlrpc_error_application               = -32500,
   xmlrpc_error_system                    = -32400,
   xmlrpc_error_transport                 = -32300
} XMLRPC_ERROR_CODE;
/******/

#define xmlrpc_error_parse_xml_syntax_str       "parse error. not well formed."
#define xmlrpc_error_parse_unknown_encoding_str "parse error. unknown encoding"
#define xmlrpc_error_parse_bad_encoding_str     "parse error. invalid character for encoding"
#define xmlrpc_error_invalid_xmlrpc_str         "server error. xml-rpc not conforming to spec"
#define xmlrpc_error_unknown_method_str         "server error. method not found."
#define xmlrpc_error_invalid_params_str         "server error. invalid method parameters"
#define xmlrpc_error_internal_server_str        "server error. internal xmlrpc library error"
#define xmlrpc_error_application_str            "application error."
#define xmlrpc_error_system_str                 "system error."
#define xmlrpc_error_transport_str              "transport error."



/****d* VALUE/XMLRPC_VERSION
 * NAME
 *   XMLRPC_VERSION
 * NOTES
 *   Defines xml vocabulary used for generated xml
 * SEE ALSO
 *   XMLRPC_REQUEST_OUTPUT_OPTIONS
 *   XMLRPC_REQUEST_To_XML ()
 * SOURCE
 */
typedef enum _xmlrpc_version {
   xmlrpc_version_none = 0,      /* not a recognized vocabulary    */
   xmlrpc_version_1_0 = 1,       /* xmlrpc 1.0 standard vocab      */
   xmlrpc_version_simple = 2,    /* alt more readable vocab        */
   xmlrpc_version_danda = 2,     /* same as simple. legacy         */
	xmlrpc_version_soap_1_1 = 3	/* SOAP. version 1.1              */
} XMLRPC_VERSION;
/******/

/****s* VALUE/XMLRPC_REQUEST_OUTPUT_OPTIONS
 * NAME
 *   XMLRPC_REQUEST_OUTPUT_OPTIONS
 * NOTES
 *   Defines output options for generated xml
 * SEE ALSO
 *   XMLRPC_VERSION
 *   XML_ELEM_OUTPUT_OPTIONS
 *   XMLRPC_REQUEST_To_XML ()
 * SOURCE
 */
typedef struct _xmlrpc_request_output_options {
   STRUCT_XML_ELEM_OUTPUT_OPTIONS xml_elem_opts;  /* xml_element specific output options */
   XMLRPC_VERSION                 version;        /* xml vocabulary to use               */
} STRUCT_XMLRPC_REQUEST_OUTPUT_OPTIONS, *XMLRPC_REQUEST_OUTPUT_OPTIONS;
/******/

/****s* VALUE/XMLRPC_REQUEST_INPUT_OPTIONS
 * NAME
 *   XMLRPC_REQUEST_INPUT_OPTIONS
 * NOTES
 *   Defines options for reading in xml data
 * SEE ALSO
 *   XMLRPC_VERSION
 *   XML_ELEM_INPUT_OPTIONS
 *   XMLRPC_REQUEST_From_XML ()
 * SOURCE
 */
typedef struct _xmlrpc_request_input_options {
   STRUCT_XML_ELEM_INPUT_OPTIONS  xml_elem_opts;  /* xml_element specific output options */
} STRUCT_XMLRPC_REQUEST_INPUT_OPTIONS, *XMLRPC_REQUEST_INPUT_OPTIONS;
/******/

/****s* VALUE/XMLRPC_ERROR
 * NAME
 *   XMLRPC_ERROR
 * NOTES
 *   For the reporting and handling of errors
 * SOURCE
 */
typedef struct _xmlrpc_error {
   XMLRPC_ERROR_CODE      code;
   STRUCT_XML_ELEM_ERROR  xml_elem_error;  /* xml_element errors (parser errors) */
} STRUCT_XMLRPC_ERROR, *XMLRPC_ERROR;
/******/


/****d* VALUE/XMLRPC_CASE_COMPARISON
 * NAME
 *   XMLRPC_CASE_COMPARISON
 * NOTES
 *   Defines case comparison options for XMLRPC_VALUE/VECTOR API's
 * SEE ALSO
 *   XMLRPC_CASE
 *   XMLRPC_VALUE
 * SOURCE
 */
typedef enum _xmlrpc_case_comparison {
   xmlrpc_case_insensitive,      /* use case-insensitive compare */
   xmlrpc_case_sensitive         /* use case-sensitive compare   */
} XMLRPC_CASE_COMPARISON;
/******/

/****d* VALUE/XMLRPC_CASE
 * NAME
 *   XMLRPC_CASE
 * NOTES
 *   Defines case behavior when setting IDs in XMLRPC_VALUE API's
 * SEE ALSO
 *   XMLRPC_CASE_COMPARISON
 *   XMLRPC_VALUE
 * SOURCE
 */
typedef enum _xmlrpc_case {
   xmlrpc_case_exact,            /* leave case alone             */
   xmlrpc_case_lower,            /* lower-case id                */
   xmlrpc_case_upper             /* upper-case id                */
} XMLRPC_CASE;
/******/

/* if you don't like these defaults, you can set them with XMLRPC_SetDefaultIdCase*() */
#define XMLRPC_DEFAULT_ID_CASE              XMLRPC_GetDefaultIdCase()
#define XMLRPC_DEFAULT_ID_CASE_SENSITIVITY  XMLRPC_GetDefaultIdCaseComparison()

/* opaque (non-public) types. defined locally in xmlrpc.c */
typedef struct _xmlrpc_request* XMLRPC_REQUEST;
typedef struct _xmlrpc_server* XMLRPC_SERVER;
typedef struct _xmlrpc_value* XMLRPC_VALUE;

/****d* VALUE/XMLRPC_Callback
 * NAME
 *   XMLRPC_Callback
 * NOTES
 *   Function prototype for user defined method handlers (callbacks).
 * SEE ALSO
 *   XMLRPC_ServerRegisterMethod ()
 *   XMLRPC_ServerCallMethod ()
 *   XMLRPC_REQUEST
 *   XMLRPC_VALUE
 * SOURCE
 */
typedef XMLRPC_VALUE (*XMLRPC_Callback)(XMLRPC_SERVER server, XMLRPC_REQUEST input, void* userData);
/******/

/* ID Case Defaults */
XMLRPC_CASE XMLRPC_GetDefaultIdCase(void);
XMLRPC_CASE XMLRPC_SetDefaultIdCase(XMLRPC_CASE id_case);
XMLRPC_CASE_COMPARISON XMLRPC_GetDefaultIdCaseComparison(void);
XMLRPC_CASE_COMPARISON XMLRPC_SetDefaultIdCaseComparison(XMLRPC_CASE_COMPARISON id_case);

/* Vector manipulation */
int XMLRPC_VectorSize(XMLRPC_VALUE value);
XMLRPC_VALUE XMLRPC_VectorRewind(XMLRPC_VALUE value);
XMLRPC_VALUE XMLRPC_VectorNext(XMLRPC_VALUE value);
int XMLRPC_SetIsVector(XMLRPC_VALUE value, XMLRPC_VECTOR_TYPE type);
int XMLRPC_AddValueToVector(XMLRPC_VALUE target, XMLRPC_VALUE source);
int XMLRPC_AddValuesToVector(XMLRPC_VALUE target, ...);
int XMLRPC_VectorRemoveValue(XMLRPC_VALUE vector, XMLRPC_VALUE value);
XMLRPC_VALUE XMLRPC_VectorGetValueWithID_Case(XMLRPC_VALUE vector, const char* id, XMLRPC_CASE_COMPARISON id_case);


/* Create values */
XMLRPC_VALUE XMLRPC_CreateValueBoolean(const char* id, int truth);
XMLRPC_VALUE XMLRPC_CreateValueBase64(const char* id, const char* s, int len);
XMLRPC_VALUE XMLRPC_CreateValueDateTime(const char* id, time_t time);
XMLRPC_VALUE XMLRPC_CreateValueDateTime_ISO8601(const char* id, const char *s);
XMLRPC_VALUE XMLRPC_CreateValueDouble(const char* id, double f);
XMLRPC_VALUE XMLRPC_CreateValueInt(const char* id, int i);
XMLRPC_VALUE XMLRPC_CreateValueString(const char* id, const char* s, int len);
XMLRPC_VALUE XMLRPC_CreateValueEmpty(void);
XMLRPC_VALUE XMLRPC_CreateVector(const char* id, XMLRPC_VECTOR_TYPE type);

/* Cleanup values */
void XMLRPC_CleanupValue(XMLRPC_VALUE value);

/* Request error */
XMLRPC_VALUE XMLRPC_RequestSetError (XMLRPC_REQUEST request, XMLRPC_VALUE error);
XMLRPC_VALUE XMLRPC_RequestGetError (XMLRPC_REQUEST request);

/* Copy values */
XMLRPC_VALUE XMLRPC_CopyValue(XMLRPC_VALUE value);
XMLRPC_VALUE XMLRPC_DupValueNew(XMLRPC_VALUE xSource);

/* Set Values */
void XMLRPC_SetValueDateTime(XMLRPC_VALUE value, time_t time);
void XMLRPC_SetValueDateTime_ISO8601(XMLRPC_VALUE value, const char* s);
void XMLRPC_SetValueDouble(XMLRPC_VALUE value, double val);
void XMLRPC_SetValueInt(XMLRPC_VALUE value, int val);
void XMLRPC_SetValueBoolean(XMLRPC_VALUE value, int val);
const char *XMLRPC_SetValueString(XMLRPC_VALUE value, const char* s, int len);
void XMLRPC_SetValueBase64(XMLRPC_VALUE value, const char* s, int len);
const char *XMLRPC_SetValueID_Case(XMLRPC_VALUE value, const char* id, int len, XMLRPC_CASE id_case);
#define XMLRPC_SetValueID(value, id, len) XMLRPC_SetValueID_Case(value, id, len, XMLRPC_DEFAULT_ID_CASE)

/* Get Values */
const char* XMLRPC_GetValueString(XMLRPC_VALUE value);
int XMLRPC_GetValueStringLen(XMLRPC_VALUE value);
int XMLRPC_GetValueInt(XMLRPC_VALUE value);
int XMLRPC_GetValueBoolean(XMLRPC_VALUE value);
double XMLRPC_GetValueDouble(XMLRPC_VALUE value);
const char* XMLRPC_GetValueBase64(XMLRPC_VALUE value);
time_t XMLRPC_GetValueDateTime(XMLRPC_VALUE value);
const char* XMLRPC_GetValueDateTime_ISO8601(XMLRPC_VALUE value);
const char* XMLRPC_GetValueID(XMLRPC_VALUE value);

/* Type introspection */
XMLRPC_VALUE_TYPE XMLRPC_GetValueType(XMLRPC_VALUE v);
XMLRPC_VALUE_TYPE_EASY XMLRPC_GetValueTypeEasy(XMLRPC_VALUE v);
XMLRPC_VECTOR_TYPE XMLRPC_GetVectorType(XMLRPC_VALUE v);

/* Parsing and Creating XML */
XMLRPC_REQUEST XMLRPC_REQUEST_FromXML(const char* in_buf, int len, XMLRPC_REQUEST_INPUT_OPTIONS in_options);
XMLRPC_VALUE XMLRPC_VALUE_FromXML(const char* in_buf, int len, XMLRPC_REQUEST_INPUT_OPTIONS in_options);
char* XMLRPC_REQUEST_ToXML(XMLRPC_REQUEST request, int *buf_len);
char* XMLRPC_VALUE_ToXML(XMLRPC_VALUE val, int* buf_len);

/* Request manipulation funcs */
const char* XMLRPC_RequestSetMethodName(XMLRPC_REQUEST request, const char* methodName);
const char* XMLRPC_RequestGetMethodName(XMLRPC_REQUEST request);
XMLRPC_REQUEST XMLRPC_RequestNew(void);
void XMLRPC_RequestFree(XMLRPC_REQUEST request, int bFreeIO);
XMLRPC_REQUEST_OUTPUT_OPTIONS XMLRPC_RequestSetOutputOptions(XMLRPC_REQUEST request, XMLRPC_REQUEST_OUTPUT_OPTIONS output);
XMLRPC_REQUEST_OUTPUT_OPTIONS XMLRPC_RequestGetOutputOptions(XMLRPC_REQUEST request);
XMLRPC_VALUE XMLRPC_RequestSetData(XMLRPC_REQUEST request, XMLRPC_VALUE data);
XMLRPC_VALUE XMLRPC_RequestGetData(XMLRPC_REQUEST request);
XMLRPC_REQUEST_TYPE XMLRPC_RequestSetRequestType(XMLRPC_REQUEST request, XMLRPC_REQUEST_TYPE type);
XMLRPC_REQUEST_TYPE XMLRPC_RequestGetRequestType(XMLRPC_REQUEST request);

/* Server Creation/Destruction; Method Registration and Invocation */
XMLRPC_SERVER XMLRPC_ServerCreate(void);
XMLRPC_SERVER XMLRPC_GetGlobalServer(void);   /* better to use XMLRPC_ServerCreate if you can */
void XMLRPC_ServerDestroy(XMLRPC_SERVER server);
int XMLRPC_ServerRegisterMethod(XMLRPC_SERVER server, const char *name, XMLRPC_Callback cb);
XMLRPC_Callback XMLRPC_ServerFindMethod(XMLRPC_SERVER server, const char* callName);
XMLRPC_VALUE XMLRPC_ServerCallMethod(XMLRPC_SERVER server, XMLRPC_REQUEST request, void* userData);

#include "xmlrpc_introspection.h"

/* Fault interrogation funcs */
int XMLRPC_ValueIsFault (XMLRPC_VALUE value);
int XMLRPC_ResponseIsFault(XMLRPC_REQUEST response);
int XMLRPC_GetValueFaultCode (XMLRPC_VALUE value);
int XMLRPC_GetResponseFaultCode(XMLRPC_REQUEST response);
const char* XMLRPC_GetValueFaultString (XMLRPC_VALUE value);
const char* XMLRPC_GetResponseFaultString (XMLRPC_REQUEST response);


/* Public Utility funcs */
XMLRPC_VALUE XMLRPC_UtilityCreateFault(int fault_code, const char* fault_string);
void XMLRPC_Free(void* mem);
const char*  XMLRPC_GetVersionString(void);

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

/* Append values to vector */
#define XMLRPC_VectorAppendString(vector, id, s, len) XMLRPC_AddValueToVector(vector, XMLRPC_CreateValueString(id, s, len))
#define XMLRPC_VectorAppendBase64(vector, id, s, len) XMLRPC_AddValueToVector(vector, XMLRPC_CreateValueBase64(id, s, len))
#define XMLRPC_VectorAppendDateTime(vector, id, time) XMLRPC_AddValueToVector(vector, XMLRPC_CreateValueDateTime(id, time))
#define XMLRPC_VectorAppendDateTime_ISO8601(vector, id, s) XMLRPC_AddValueToVector(vector, XMLRPC_CreateValueDateTime_ISO8601(id, s))
#define XMLRPC_VectorAppendDouble(vector, id, f) XMLRPC_AddValueToVector(vector, XMLRPC_CreateValueDouble(id, f))
#define XMLRPC_VectorAppendInt(vector, id, i) XMLRPC_AddValueToVector(vector, XMLRPC_CreateValueInt(id, i))
#define XMLRPC_VectorAppendBoolean(vector, id, i) XMLRPC_AddValueToVector(vector, XMLRPC_CreateValueBoolean(id, i))

/* Get named values from vector */
#define XMLRPC_VectorGetValueWithID(vector, id) XMLRPC_VectorGetValueWithID_Case(vector, id, XMLRPC_DEFAULT_ID_CASE_SENSITIVITY)
#define XMLRPC_VectorGetStringWithID(vector, id) XMLRPC_GetValueString(XMLRPC_VectorGetValueWithID(vector, id))
#define XMLRPC_VectorGetBase64WithID(vector, id) XMLRPC_GetValueBase64(XMLRPC_VectorGetValueWithID(vector, id))
#define XMLRPC_VectorGetDateTimeWithID(vector, id) XMLRPC_GetValueDateTime(XMLRPC_VectorGetValueWithID(vector, id))
#define XMLRPC_VectorGetDoubleWithID(vector, id) XMLRPC_GetValueDouble(XMLRPC_VectorGetValueWithID(vector, id))
#define XMLRPC_VectorGetIntWithID(vector, id) XMLRPC_GetValueInt(XMLRPC_VectorGetValueWithID(vector, id))
#define XMLRPC_VectorGetBooleanWithID(vector, id) XMLRPC_GetValueBoolean(XMLRPC_VectorGetValueWithID(vector, id))

/******/


#ifdef __cplusplus
}
#endif

#endif /* not XMLRPC_ALREADY_INCLUDED */



