/*
  This file is part of libXMLRPC - a C library for xml-encoded function calls.

  Author: Dan Libby (dan@libby.com)
*/


/*-**********************************************************************
* TODO:                                                                 *
*  - [SOAP-ENC:position] read sparse arrays (and write?)                *
*  - [SOAP-ENC:offset] read partially transmitted arrays  (and write?)  *
*  - read "flattened" multi-dimensional arrays. (don't bother writing)  *
*                                                                       *
* BUGS:                                                                 *
*  - does not read schema. thus only knows soap pre-defined types.      *
*  - references (probably) do not work. untested.                       *
*  - does not expose SOAP-ENV:Header to application at all.             *
*  - does not use namespaces correctly, thus:                           *
*    - namespaces are hard-coded in comparison tokens                   *
*    - if a sender uses another namespace identifer, it will break      *
************************************************************************/


static const char rcsid[] = "#(@) $Id:";

#ifdef _WIN32
#include "xmlrpc_win32.h"
#endif
#include <string.h>
#include <stdlib.h>
#include "xml_to_soap.h"
#include "base64.h"

/* list of tokens used in vocab */
#define TOKEN_ANY				 "xsd:ur-type"
#define TOKEN_ARRAY          "SOAP-ENC:Array"
#define TOKEN_ARRAY_TYPE     "SOAP-ENC:arrayType"
#define TOKEN_BASE64         "SOAP-ENC:base64"
#define TOKEN_BOOLEAN        "xsd:boolean"
#define TOKEN_DATETIME       "xsd:timeInstant"
#define TOKEN_DOUBLE         "xsd:double"
#define TOKEN_FLOAT          "xsd:float"
#define TOKEN_ID             "id"
#define TOKEN_INT            "xsd:int"
#define TOKEN_NULL           "xsi:null"
#define TOKEN_STRING         "xsd:string"
#define TOKEN_STRUCT			 "xsd:struct"
#define TOKEN_TYPE           "xsi:type"
#define TOKEN_FAULT			 "SOAP-ENV:Fault"
#define TOKEN_MUSTUNDERSTAND "SOAP-ENV:mustUnderstand"
#define TOKEN_ACTOR			 "SOAP-ENV:actor"
#define TOKEN_ACTOR_NEXT		 "http://schemas.xmlsoap.org/soap/actor/next"

#define TOKEN_XMLRPC_FAULTCODE   "faultCode"
#define TOKEN_XMLRPC_FAULTSTRING "faultString"
#define TOKEN_SOAP_FAULTCODE     "faultcode"
#define TOKEN_SOAP_FAULTSTRING   "faultstring"
#define TOKEN_SOAP_FAULTDETAILS  "details"
#define TOKEN_SOAP_FAULTACTOR    "actor"


// determine if a string represents a soap type, as used in
// element names
static inline int is_soap_type(const char* soap_type) {
	return(strstr(soap_type, "SOAP-ENC:") || strstr(soap_type, "xsd:")) ? 1 : 0;
}

/* utility func to generate a new attribute. possibly should be in xml_element.c?? */
static xml_element_attr* new_attr(const char* key, const char* val) {
	xml_element_attr* attr = malloc(sizeof(xml_element_attr));
	if (attr) {
		attr->key = key ? strdup(key) : NULL;
		attr->val = val ? strdup(val) : NULL;
	}
	return attr;
}

struct array_info {
	char          kids_type[30];
	unsigned long size;
	/* ... ? */
};


/* parses soap arrayType attribute to generate an array_info structure.
 * TODO: should deal with sparse, flattened, & multi-dimensional arrays
 */
static struct array_info* parse_array_type_info(const char* array_type) {
	struct array_info* ai = NULL;
	if (array_type) {
		ai = (struct array_info*)calloc(1, sizeof(struct array_info));
		if (ai) {
			char buf[128], *p;
			snprintf(buf, sizeof(buf), "%s", array_type);
			p = strchr(buf, '[');
			if (p) {
				*p = 0;
			}
			strcpy(ai->kids_type, buf);
		}
	}
	return ai;
}

/* performs heuristics on an xmlrpc_vector_array to determine
 * appropriate soap arrayType string.
 */
static const char* get_array_soap_type(XMLRPC_VALUE node) {
	XMLRPC_VALUE_TYPE_EASY type = xmlrpc_type_none;

	XMLRPC_VALUE xIter = XMLRPC_VectorRewind(node);
	int loopCount = 0;
	const char* soapType = TOKEN_ANY;

	type = XMLRPC_GetValueTypeEasy(xIter);
	xIter = XMLRPC_VectorNext(node);

	while (xIter) {
		/* 50 seems like a decent # of loops.  That will likely
		 * cover most cases.  Any more and we start to sacrifice
		 * performance.
		 */
		if ( (XMLRPC_GetValueTypeEasy(xIter) != type) || loopCount >= 50) {
			type = xmlrpc_type_none;
			break;
		}
		loopCount ++;

		xIter = XMLRPC_VectorNext(node);
	}
	switch (type) {
	case xmlrpc_type_none:
		soapType = TOKEN_ANY;
		break;
	case xmlrpc_type_empty:
		soapType = TOKEN_NULL;
		break;
	case xmlrpc_type_int:
		soapType = TOKEN_INT;
		break;
	case xmlrpc_type_double:
		soapType = TOKEN_DOUBLE;
		break;
	case xmlrpc_type_boolean:
		soapType = TOKEN_BOOLEAN;
		break;
	case xmlrpc_type_string:
		soapType = TOKEN_STRING;
		break;
	case xmlrpc_type_base64:
		soapType = TOKEN_BASE64;
		break;
	case xmlrpc_type_datetime:
		soapType = TOKEN_DATETIME;
		break;
	case xmlrpc_type_struct:
		soapType = TOKEN_STRUCT;
		break;
	case xmlrpc_type_array:
		soapType = TOKEN_ARRAY;
		break;
	case xmlrpc_type_mixed:
		soapType = TOKEN_STRUCT;
		break;
	}
	return soapType;
}

/* determines wether a node is a fault or not, and of which type:
 * 0 = not a fault,
 * 1 = xmlrpc style fault
 * 2 = soap style fault.
 */
static inline int get_fault_type(XMLRPC_VALUE node) {
	if (XMLRPC_VectorGetValueWithID(node, TOKEN_XMLRPC_FAULTCODE) &&
		 XMLRPC_VectorGetValueWithID(node, TOKEN_XMLRPC_FAULTSTRING)) {
		return 1;
	}
	else if (XMLRPC_VectorGetValueWithID(node, TOKEN_SOAP_FAULTCODE) &&
				XMLRPC_VectorGetValueWithID(node, TOKEN_SOAP_FAULTSTRING)) {
		return 2;
	}
	return 0;
}

/* input: an XMLRPC_VALUE representing a fault struct in xml-rpc style.
 * output: an XMLRPC_VALUE representing a fault struct in soap style,
 *  with xmlrpc codes mapped to soap codes, and all other values preserved.
 *  note that the returned value is a completely new value, and must be freed.
 *  the input value is untouched.
 */
static XMLRPC_VALUE gen_fault_xmlrpc(XMLRPC_VALUE node, xml_element* el_target) {
	XMLRPC_VALUE xDup = XMLRPC_DupValueNew(node);
	XMLRPC_VALUE xCode = XMLRPC_VectorGetValueWithID(xDup, TOKEN_XMLRPC_FAULTCODE);
	XMLRPC_VALUE xStr = XMLRPC_VectorGetValueWithID(xDup, TOKEN_XMLRPC_FAULTSTRING);

	XMLRPC_SetValueID(xCode, TOKEN_SOAP_FAULTCODE, 0);
	XMLRPC_SetValueID(xStr, TOKEN_SOAP_FAULTSTRING, 0);

	/* rough mapping of xmlrpc fault codes to soap codes */
	switch (XMLRPC_GetValueInt(xCode)) {
	case -32700:		  // "parse error. not well formed",
	case -32701:		  // "parse error. unsupported encoding"
	case -32702:		  // "parse error. invalid character for encoding"
	case -32600:		  // "server error. invalid xml-rpc.  not conforming to spec."
	case -32601:		  // "server error. requested method not found"
	case -32602:		  // "server error. invalid method parameters"
		XMLRPC_SetValueString(xCode, "SOAP-ENV:Client", 0);
		break;
	case -32603:		  // "server error. internal xml-rpc error"
	case -32500:		  // "application error"
	case -32400:		  // "system error"
	case -32300:		  // "transport error
		XMLRPC_SetValueString(xCode, "SOAP-ENV:Server", 0);
		break;
	}
	return xDup;
}

// returns a new XMLRPC_VALUE representing a soap fault, comprised of a struct with four keys.
static XMLRPC_VALUE gen_soap_fault(const char* fault_code, const char* fault_string, 
											  const char* actor, const char* details) {
	XMLRPC_VALUE xReturn = XMLRPC_CreateVector(TOKEN_FAULT, xmlrpc_vector_struct);
	XMLRPC_AddValuesToVector(xReturn,
									 XMLRPC_CreateValueString(TOKEN_SOAP_FAULTCODE, fault_code, 0),
									 XMLRPC_CreateValueString(TOKEN_SOAP_FAULTSTRING, fault_string, 0),
									 XMLRPC_CreateValueString(TOKEN_SOAP_FAULTACTOR, actor, 0),
									 XMLRPC_CreateValueString(TOKEN_SOAP_FAULTDETAILS, details, 0),
									 NULL);
	return xReturn;
}

/* translates xml soap dom to native data structures. recursive. */
XMLRPC_VALUE xml_element_to_SOAP_REQUEST_worker(XMLRPC_REQUEST request, 
																XMLRPC_VALUE xParent,
																struct array_info* parent_array,
																XMLRPC_VALUE xCurrent, 
																xml_element* el, 
																int depth) {
	XMLRPC_REQUEST_TYPE rtype = xmlrpc_request_none;

	// no current element on first call
	if (!xCurrent) {
		xCurrent = XMLRPC_CreateValueEmpty();
	}

	// increment recursion depth guage
	depth ++;

	// safety first. must have a valid element
	if (el && el->name) {
		const char* id = NULL;
		const char* type = NULL, *arrayType=NULL, *actor = NULL;
		xml_element_attr* attr_iter = Q_Head(&el->attrs);
		int b_must_understand = 0;
		
		// in soap, types may be specified in either element name -or- with xsi:type attribute.
		if (is_soap_type(el->name)) {
			type = el->name;
		}
		// if our parent node, by definition a vector, is not an array, then
		// our element name must be our key identifier.
		else if (XMLRPC_GetVectorType(xParent) != xmlrpc_vector_array) {
			id = el->name;
			if(!strcmp(id, "item")) {
			}
		}

		// iterate through element attributes, pick out useful stuff.
		while (attr_iter) {
			// element's type
			if (!strcmp(attr_iter->key, TOKEN_TYPE)) {
				type = attr_iter->val;
			}
			// array type
			else if (!strcmp(attr_iter->key, TOKEN_ARRAY_TYPE)) {
				arrayType = attr_iter->val;
			}
			// must understand, sometimes present in headers.
			else if (!strcmp(attr_iter->key, TOKEN_MUSTUNDERSTAND)) {
				b_must_understand = strchr(attr_iter->val, '1') ? 1 : 0;
			}
			// actor, used in conjuction with must understand.
			else if (!strcmp(attr_iter->key, TOKEN_ACTOR)) {
				actor = attr_iter->val;
			}
			attr_iter = Q_Next(&el->attrs);
		}

		// check if caller says we must understand something in a header.
		if (b_must_understand) {
			// is must understand actually indended for us?
			// BUG: spec says we should also determine if actor is our URL, but
			//      we do not have that information.
			if (!actor || !strcmp(actor, TOKEN_ACTOR_NEXT)) {
				// TODO: implement callbacks or other mechanism for applications
				// to "understand" these headers. For now, we just bail if we
				// get a mustUnderstand header intended for us.
				XMLRPC_RequestSetError(request, 
											  gen_soap_fault("SOAP-ENV:MustUnderstand",
																  "SOAP Must Understand Error",
																  "", ""));
				return xCurrent;
			}
		}

		// set id (key) if one was found.
		if (id) {
			XMLRPC_SetValueID_Case(xCurrent, id, 0, xmlrpc_case_exact);
		}

		// according to soap spec, 
		// depth 1 = Envelope, 2 = Header, Body & Fault, 3 = methodcall or response.
		if (depth == 3) {
			const char* methodname = el->name;
			char* p = NULL;

			// BUG: we determine request or response type using presence of "Response" in element name.
			// According to spec, this is only recommended, not required. Apparently, implementations
			// are supposed to know the type of action based on state, which strikes me as a bit lame.
			// Anyway, we don't have that state info, thus we use Response as a heuristic.
			rtype =
#ifdef strcasestr
			strcasestr(el->name, "response") ? xmlrpc_request_response : xmlrpc_request_call;
#else
			strstr(el->name, "esponse") ? xmlrpc_request_response : xmlrpc_request_call;
#endif
			XMLRPC_RequestSetRequestType(request, rtype);

			// Get methodname.  strip xml namespace crap.
			p = strchr(el->name, ':');
			if (p) {
				methodname = p + 1;
			}
			if (rtype == xmlrpc_request_call) {
				XMLRPC_RequestSetMethodName(request, methodname);
			}
		}


		// Next, we begin to convert actual values.
		// if no children, then must be a scalar value.
		if (!Q_Size(&el->children)) {
			if (!type && parent_array && parent_array->kids_type[0]) {
				type = parent_array->kids_type;
			}
			if (!type || !strcmp(type, TOKEN_STRING)) {
				XMLRPC_SetValueString(xCurrent, el->text.str, el->text.len);
			}
			else if (!strcmp(type, TOKEN_INT)) {
				XMLRPC_SetValueInt(xCurrent, atoi(el->text.str));
			}
			else if (!strcmp(type, TOKEN_BOOLEAN)) {
				XMLRPC_SetValueBoolean(xCurrent, atoi(el->text.str));
			}
			else if (!strcmp(type, TOKEN_DOUBLE) ||
						!strcmp(type, TOKEN_FLOAT)) {
				XMLRPC_SetValueDouble(xCurrent, atof(el->text.str));
			}
			else if (!strcmp(type, TOKEN_NULL)) {
				// already an empty val. do nothing.
			}
			else if (!strcmp(type, TOKEN_DATETIME)) {
				XMLRPC_SetValueDateTime_ISO8601(xCurrent, el->text.str);
			}
			else if (!strcmp(type, TOKEN_BASE64)) {
				struct buffer_st buf;
				base64_decode(&buf, el->text.str, el->text.len);
				XMLRPC_SetValueBase64(xCurrent, buf.data, buf.offset);
				buffer_delete(&buf);
			}
		}
		// Element has children, thus a vector, or "compound type" in soap-speak.
		else {
			struct array_info* ai = NULL;
			xml_element* iter = (xml_element*)Q_Head(&el->children);

			if (!type || !strcmp(type, TOKEN_STRUCT)) {
				XMLRPC_SetIsVector(xCurrent, xmlrpc_vector_struct);
			}
			else if (!strcmp(type, TOKEN_ARRAY) || arrayType != NULL) {
				// determine magic associated with soap array type.
				// this is passed down as we recurse, so our children have access to the info.
				ai = parse_array_type_info(arrayType);	// alloc'ed ai free'd below.
				XMLRPC_SetIsVector(xCurrent, xmlrpc_vector_array);
			}
			else {
				// mixed is probably closest thing we have to compound type.
				XMLRPC_SetIsVector(xCurrent, xmlrpc_vector_mixed);
			}
			// Recurse, adding values as we go.  Check for error during recursion
			// and if found, bail.  this short-circuits us out of the recursion.
			while ( iter && !XMLRPC_RequestGetError(request) ) {
				XMLRPC_VALUE xNext = NULL;
				// top level elements don't actually represent values, so we just pass the
				// current value along until we are deep enough.
				if ( depth <= 2 ||
					  (rtype == xmlrpc_request_response && depth <= 3) ) {
					xml_element_to_SOAP_REQUEST_worker(request, NULL, ai, xCurrent, iter, depth);
				}
				// ready to do some actual de-serialization. create a new empty value and
				// pass that along to be init'd, then add it to our current vector.
				else {
					xNext = XMLRPC_CreateValueEmpty();
					xml_element_to_SOAP_REQUEST_worker(request, xCurrent, ai, xNext, iter, depth);
					XMLRPC_AddValueToVector(xCurrent, xNext);
				}
				iter = (xml_element*)Q_Next(&el->children);
			}
			// cleanup
			if (ai) {
				free(ai);
			}
		}
	}
	return xCurrent;
}

// Convert soap xml dom to XMLRPC_VALUE, sans request info.  untested.
XMLRPC_VALUE xml_element_to_SOAP_VALUE(xml_element* el)
{
	return xml_element_to_SOAP_REQUEST_worker(NULL, NULL, NULL, NULL, el, 0);
}

// Convert soap xml dom to XMLRPC_REQUEST
XMLRPC_VALUE xml_element_to_SOAP_REQUEST(XMLRPC_REQUEST request, xml_element* el)
{
	if (request) {
		return XMLRPC_RequestSetData(request, xml_element_to_SOAP_REQUEST_worker(request, NULL, NULL, NULL, el, 0));
	}
	return NULL;
}


/* translates data structures to soap/xml. recursive */
xml_element* SOAP_to_xml_element_worker(XMLRPC_REQUEST request, XMLRPC_VALUE node) {
#define BUF_SIZE 128
	xml_element* elem_val = NULL;
	if (node) {
		int bFreeNode = 0;  /* sometimes we may need to free 'node' variable */
		char buf[BUF_SIZE];
		XMLRPC_VALUE_TYPE_EASY type = XMLRPC_GetValueTypeEasy(node);
		char* pName = NULL, *pAttrType = NULL;

		// create our return value element
		elem_val = xml_elem_new();

		switch (type) {
		case xmlrpc_type_struct:
		case xmlrpc_type_mixed:
		case xmlrpc_type_array:
			if (type == xmlrpc_type_array) {
				// array's are _very_ special in soap.
				// TODO: Should handle sparse/partial arrays here.

				// determine soap array type.
				const char* type = get_array_soap_type(node);
				xml_element_attr* attr_array_type = NULL;

				// specify array kids type and array size.  
				snprintf(buf, sizeof(buf), "%s[%i]", type, XMLRPC_VectorSize(node));
				attr_array_type = new_attr(TOKEN_ARRAY_TYPE, buf);

				Q_PushTail(&elem_val->attrs, attr_array_type);

				pAttrType = TOKEN_ARRAY;
			}
			// check for fault, which is a rather special case. 
			// (can't these people design anything consistent/simple/elegant?)
			else if (type == xmlrpc_type_struct) {
				int fault_type = get_fault_type(node);
				if (fault_type) {
					if (fault_type == 1) {
						// gen fault from xmlrpc style fault codes              
						// notice that we get a new node, which must be freed herein.
						node = gen_fault_xmlrpc(node, elem_val);
						bFreeNode = 1;
					}
					pName = TOKEN_FAULT;
				}
			}

			{
				/* recurse through sub-elements */
				XMLRPC_VALUE xIter = XMLRPC_VectorRewind(node);
				while ( xIter ) {
					xml_element* next_el = SOAP_to_xml_element_worker(request, xIter);
					if (next_el) {
						Q_PushTail(&elem_val->children, next_el);
					}
					xIter = XMLRPC_VectorNext(node);
				}
			}

			break;

			// handle scalar types
		case xmlrpc_type_empty:
			pAttrType = TOKEN_NULL;
			break;
		case xmlrpc_type_string:
			pAttrType = TOKEN_STRING;
			simplestring_addn(&elem_val->text, XMLRPC_GetValueString(node), XMLRPC_GetValueStringLen(node));
			break;
		case xmlrpc_type_int:
			pAttrType = TOKEN_INT;
			snprintf(buf, BUF_SIZE, "%i", XMLRPC_GetValueInt(node));
			simplestring_add(&elem_val->text, buf);
			break;
		case xmlrpc_type_boolean:
			pAttrType = TOKEN_BOOLEAN;
			snprintf(buf, BUF_SIZE, "%i", XMLRPC_GetValueBoolean(node));
			simplestring_add(&elem_val->text, buf);
			break;
		case xmlrpc_type_double:
			pAttrType = TOKEN_DOUBLE;
			snprintf(buf, BUF_SIZE, "%f", XMLRPC_GetValueDouble(node));
			simplestring_add(&elem_val->text, buf);
			break;
		case xmlrpc_type_datetime:
			{
				time_t tt = XMLRPC_GetValueDateTime(node);
				struct tm *tm = localtime (&tt);
				pAttrType = TOKEN_DATETIME;
				if(strftime (buf, BUF_SIZE, "%Y-%m-%dT%H:%M:%SZ", tm)) {
					simplestring_add(&elem_val->text, buf);
				}
			}
			break;
		case xmlrpc_type_base64:
			{
				struct buffer_st buf;
				pAttrType = TOKEN_BASE64;
				base64_encode(&buf, XMLRPC_GetValueBase64(node), XMLRPC_GetValueStringLen(node));
				simplestring_addn(&elem_val->text, buf.data, buf.offset );
				buffer_delete(&buf);
			}
			break;
			break;
		default:
			break;
		}

		// determining element's name is a bit tricky, due to soap semantics.
		if (!pName) {
			// if the value's type is known...
			if (pAttrType) {
				// see if it has an id (key). If so, use that as name, 
				// and type as an attribute.
				pName = (char*)XMLRPC_GetValueID(node);
				if (pName) {
					Q_PushTail(&elem_val->attrs, new_attr(TOKEN_TYPE, pAttrType));
				}

				// otherwise, use the type as the name.
				else {
					pName = pAttrType;
				}
			}
			// if the value's type is not known... (a rare case?)
			else {
				// see if it has an id (key). otherwise, default to generic "item"
				pName = (char*)XMLRPC_GetValueID(node);
				if (!pName) {
					pName = "item";
				}
			}
		}
		elem_val->name = strdup(pName);

		// cleanup
		if (bFreeNode) {
			XMLRPC_CleanupValue(node);
		}
	}
	return elem_val;
}

// convert XMLRPC_VALUE to soap xml dom.  untested.
xml_element* SOAP_VALUE_to_xml_element(XMLRPC_VALUE node) {
	return SOAP_to_xml_element_worker(NULL, node);
}

// convert XMLRPC_REQUEST to soap xml dom.  
xml_element* SOAP_REQUEST_to_xml_element(XMLRPC_REQUEST request) {
	xml_element* root = xml_elem_new();

	// safety first.
	if (root) {
		xml_element* body = xml_elem_new();
		root->name = strdup("SOAP-ENV:Envelope");

		/* silly namespace stuff */
		Q_PushTail(&root->attrs, new_attr("xmlns:SOAP-ENV", "http://schemas.xmlsoap.org/soap/envelope/"));
		Q_PushTail(&root->attrs, new_attr("xmlns:xsi", "http://www.w3.org/1999/XMLSchema-instance"));
		Q_PushTail(&root->attrs, new_attr("xmlns:xsd", "http://www.w3.org/1999/XMLSchema"));
		Q_PushTail(&root->attrs, new_attr("xmlns:SOAP-ENC", "http://schemas.xmlsoap.org/soap/encoding/"));
		Q_PushTail(&root->attrs, new_attr("xmlns:si", "http://soapinterop.org/xsd"));
		Q_PushTail(&root->attrs, new_attr("xmlns:ns6", "http://testuri.org"));
		Q_PushTail(&root->attrs, new_attr("SOAP-ENV:encodingStyle", "http://schemas.xmlsoap.org/soap/encoding/"));

		//Q_PushHead(&root->attrs, new_attr("xmlns:ks", "http://kitchen.sink.org/soap/everything/under/sun"));
		//      JUST KIDDING!! :-)  ---->                -------------------------------------------------

		if (body) {
			// go ahead and serialize first...
			xml_element* el_serialized =  
			SOAP_to_xml_element_worker(request, 
												XMLRPC_RequestGetData(request));

			/* check for fault, in which case, there is no intermediate element */
			if (el_serialized && !strcmp(el_serialized->name, TOKEN_FAULT)) {
				Q_PushTail(&body->children, el_serialized);
			}
			// usual case: not a fault. Add Response element in between.
			else {
				xml_element* rpc = xml_elem_new();

				if (rpc) {
					const char* methodname = XMLRPC_RequestGetMethodName(request);
					XMLRPC_REQUEST_TYPE rtype = XMLRPC_RequestGetRequestType(request);

					// if we are making a request, we want to use the methodname as is.
					if (rtype == xmlrpc_request_call) {
						if (methodname) {
							rpc->name = strdup(methodname);
						}
					}
					// if it's a response, we append "Response". Also, given xmlrpc-epi
					// API/architecture, it's likely that we don't have a methodname for
					// the response, so we have to check that.
					else {
						char buf[128];
						snprintf(buf, sizeof(buf), "%s%s", 
									methodname ? methodname : "",
									"Response");

						rpc->name = strdup(buf);
					}

					// add serialized data to method call/response.
					// add method call/response to body element
					if (rpc->name) {
						if(el_serialized) {
							if(Q_Size(&el_serialized->children) && rtype == xmlrpc_request_call) {
								xml_element* iter = (xml_element*)Q_Head(&el_serialized->children);
								while(iter) {
									Q_PushTail(&rpc->children, iter);
									iter = (xml_element*)Q_Next(&el_serialized->children);
								}
								xml_elem_free_non_recurse(el_serialized);
							}
							else {
								Q_PushTail(&rpc->children, el_serialized);
							}
						}

						Q_PushTail(&body->children, rpc);
					}
					else {
						// no method name?!
						// TODO: fault here...?
					}
				}
			}
			body->name = strdup("SOAP-ENV:Body");
			Q_PushTail(&root->children, body);
		}
	}

	return root;
}

