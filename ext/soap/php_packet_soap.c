#include "php_soap.h"

/* SOAP client calls this function to parse response from SOAP server */
int parse_packet_soap(zval *this_ptr, char *buffer, int buffer_size, sdlFunctionPtr fn, char *fn_name, zval *return_value TSRMLS_DC)
{
	char* envelope_ns;
	xmlDocPtr response;
	xmlNodePtr trav, env, head, body, resp, cur, fault;
	int param_count = 0;
	int old_error_reporting;

	ZVAL_NULL(return_value);

	old_error_reporting = EG(error_reporting);
	EG(error_reporting) &= ~(E_WARNING|E_NOTICE|E_USER_WARNING|E_USER_NOTICE);

	/* Parse XML packet */
	response = xmlParseMemory(buffer, buffer_size);
	xmlCleanupParser();
	
	EG(error_reporting) = old_error_reporting;
	
	if (!response) {
		add_soap_fault(this_ptr, "SOAP-ENV:Client", "looks like we got no XML document", NULL, NULL TSRMLS_CC);
		return FALSE;
	}

	/* Get <Envelope> element */
	env = NULL;
	trav = response->children;
	while (trav != NULL) {
		if (trav->type == XML_ELEMENT_NODE) {
			if (env == NULL && node_is_equal_ex(trav,"Envelope",SOAP_1_1_ENV)) {
				env = trav;
				envelope_ns = SOAP_1_1_ENV;
			} else if (env == NULL && node_is_equal_ex(trav,"Envelope",SOAP_1_2_ENV)) {
				env = trav;
				envelope_ns = SOAP_1_2_ENV;
			} else {
				add_soap_fault(this_ptr, "SOAP-ENV:Client", "looks like we got bad SOAP response\n", NULL, NULL TSRMLS_CC);
				xmlFreeDoc(response);
				return FALSE;
			}
		}
		trav = trav->next;
	}
	if (env == NULL) {
		add_soap_fault(this_ptr, "SOAP-ENV:Client", "looks like we got XML without \"Envelope\" element\n", NULL, NULL TSRMLS_CC);
		xmlFreeDoc(response);
		return FALSE;
	}

	/* Get <Header> element */
	head = NULL;
	trav = env->children;
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL && node_is_equal_ex(trav,"Header",envelope_ns)) {
		head = trav;
		trav = trav->next;
	}

	/* Get <Body> element */
	body = NULL;
	while (trav != NULL) {
		if (trav->type == XML_ELEMENT_NODE) {
			if (body == NULL && node_is_equal_ex(trav,"Body",envelope_ns)) {
				body = trav;
			} else {
				add_soap_fault(this_ptr, "SOAP-ENV:Client", "looks like we got bad SOAP response\n", NULL, NULL TSRMLS_CC);
				xmlFreeDoc(response);
				return FALSE;
			}
		}
		trav = trav->next;
	}
	if (body == NULL) {
		add_soap_fault(this_ptr, "SOAP-ENV:Client", "looks like we got \"Envelope\" without \"Body\" element\n", NULL, NULL TSRMLS_CC);
		xmlFreeDoc(response);
		return FALSE;
	}

	/* Check if <Body> contains <Fault> element */
	fault = get_node_ex(body->children,"Fault",envelope_ns);
	if (fault != NULL) {
		char *faultcode = NULL, *faultstring = NULL, *faultactor = NULL;
		zval *details = NULL;
		xmlNodePtr tmp;

		tmp = get_node(fault->children,"faultcode");
		if (tmp != NULL && tmp->children != NULL) {
			faultcode = tmp->children->content;
		}

		tmp = get_node(fault->children,"faultstring");
		if (tmp != NULL && tmp->children != NULL) {
			faultstring = tmp->children->content;
		}

		tmp = get_node(fault->children,"faultactor");
		if (tmp != NULL && tmp->children != NULL) {
			faultactor = tmp->children->content;
		}

		tmp = get_node(fault->children,"detail");
		if (tmp != NULL) {
			encodePtr enc;
			enc = get_conversion(UNKNOWN_TYPE);
			details = enc->to_zval(enc->details, tmp);
		}

		add_soap_fault(this_ptr, faultcode, faultstring, faultactor, details TSRMLS_CC);
		xmlFreeDoc(response);
		return FALSE;
	}

	/* Parse content of <Body> element */
	resp = body->children;
	if (fn != NULL) {
	  /* Function has WSDL description */
		sdlParamPtr *h_param, param = NULL;
		xmlNodePtr val = NULL;
		char *name, *ns = NULL;
		zval* tmp;

		if (fn->binding->bindingType == BINDING_SOAP) {
			sdlSoapBindingFunctionPtr fnb = (sdlSoapBindingFunctionPtr)fn->bindingAttributes;
			int res_count = zend_hash_num_elements(fn->responseParameters);

			array_init(return_value);
			zend_hash_internal_pointer_reset(fn->responseParameters);
			while (zend_hash_get_current_data(fn->responseParameters, (void **)&h_param) == SUCCESS) {
				param = (*h_param);
				if (fnb->style == SOAP_DOCUMENT) {
					name = param->encode->details.type_str;
					ns = param->encode->details.ns;
				} else {
					name = fn->responseName;
					/* ns = ? */
				}

				/* Get value of parameter */
				cur = get_node_ex(resp, name, ns);
				if (!cur) {
					cur = get_node(resp, name);
					/* TODO: produce warning invalid ns */
				}
				if (cur) {
					if (fnb->style == SOAP_DOCUMENT) {
						val = cur;
					} else {
						val = get_node(cur->children, param->paramName);
						if (val == NULL && res_count == 1) {
							val = get_node(cur->children, "return");
						}
						if (val == NULL && res_count == 1) {
							val = get_node(cur->children, "result");
						}
					}
				}

				if (!val) {
					/* TODO: may be "nil" is not OK? */
					MAKE_STD_ZVAL(tmp);
					ZVAL_NULL(tmp);
/*
					add_soap_fault(this_ptr, "SOAP-ENV:Client", "Can't find response data", NULL, NULL TSRMLS_CC);
					xmlFreeDoc(response);
					return FALSE;
*/
				} else {
					/* Decoding value of parameter */
					if (param != NULL) {
						tmp = master_to_zval(param->encode, val);
					} else {
						tmp = master_to_zval(get_conversion(UNKNOWN_TYPE), val);
					}
				}
				add_assoc_zval(return_value, param->paramName, tmp);
				/*add_assoc_zval(return_value, (char*)val->name, tmp);*/

				param_count++;

				zend_hash_move_forward(fn->responseParameters);
			}
		}
	} else {
	  /* Function hasn't WSDL description */
		cur = resp;
		array_init(return_value);
		while (cur && cur->type != XML_ELEMENT_NODE) {
			cur = cur->next;
		}
		if (cur != NULL) {
			xmlNodePtr val;
			val = cur->children;
			while (val != NULL) {
				while (val && val->type != XML_ELEMENT_NODE) {
					val = val->next;
				}
				if (val != NULL) {
					encodePtr enc;
					zval *tmp;
					enc = get_conversion(UNKNOWN_TYPE);
					tmp = master_to_zval(enc, val);
					if (val->name) {
						add_assoc_zval(return_value, (char*)val->name, tmp);
					} else {
						add_next_index_zval(return_value, tmp);
					}
					++param_count;
					val = val->next;
				}
			}
		}
	}

	if (Z_TYPE_P(return_value) == IS_ARRAY) {
		if (param_count == 0) {
			zval_dtor(return_value);
			ZVAL_NULL(return_value);
		} else if (param_count == 1) {
			zval *tmp = *(zval**)Z_ARRVAL_P(return_value)->pListHead->pData;
			tmp->refcount++;
			zval_dtor(return_value);
			*return_value = *tmp;
			FREE_ZVAL(tmp);
		}
	}

	xmlFreeDoc(response);
	return TRUE;
}
