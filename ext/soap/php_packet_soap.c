/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Brad Lafountain <rodif_bl@yahoo.com>                        |
  |          Shane Caraveo <shane@caraveo.com>                           |
  |          Dmitry Stogov <dmitry@zend.com>                             |
  +----------------------------------------------------------------------+
*/
/* $Id$ */

#include "php_soap.h"

/* SOAP client calls this function to parse response from SOAP server */
int parse_packet_soap(zval *this_ptr, char *buffer, int buffer_size, sdlFunctionPtr fn, char *fn_name, zval *return_value, zval *soap_headers)
{
	char* envelope_ns = NULL;
	xmlDocPtr response;
	xmlNodePtr trav, env, head, body, resp, cur, fault;
	xmlAttrPtr attr;
	int param_count = 0;
	int soap_version = SOAP_1_1;
	HashTable *hdrs = NULL;

	ZVAL_NULL(return_value);

	/* Response for one-way opearation */
	if (buffer_size == 0) {
		return TRUE;
	}

	/* Parse XML packet */
	response = soap_xmlParseMemory(buffer, buffer_size);

	if (!response) {
		add_soap_fault(this_ptr, "Client", "looks like we got no XML document", NULL, NULL);
		return FALSE;
	}
	if (xmlGetIntSubset(response) != NULL) {
		add_soap_fault(this_ptr, "Client", "DTD are not supported by SOAP", NULL, NULL);
		xmlFreeDoc(response);
		return FALSE;
	}

	/* Get <Envelope> element */
	env = NULL;
	trav = response->children;
	while (trav != NULL) {
		if (trav->type == XML_ELEMENT_NODE) {
			if (env == NULL && node_is_equal_ex(trav,"Envelope",SOAP_1_1_ENV_NAMESPACE)) {
				env = trav;
				envelope_ns = SOAP_1_1_ENV_NAMESPACE;
				soap_version = SOAP_1_1;
			} else if (env == NULL && node_is_equal_ex(trav,"Envelope",SOAP_1_2_ENV_NAMESPACE)) {
				env = trav;
				envelope_ns = SOAP_1_2_ENV_NAMESPACE;
				soap_version = SOAP_1_2;
			} else {
				add_soap_fault(this_ptr, "VersionMismatch", "Wrong Version", NULL, NULL);
				xmlFreeDoc(response);
				return FALSE;
			}
		}
		trav = trav->next;
	}
	if (env == NULL) {
		add_soap_fault(this_ptr, "Client", "looks like we got XML without \"Envelope\" element", NULL, NULL);
		xmlFreeDoc(response);
		return FALSE;
	}

	attr = env->properties;
	while (attr != NULL) {
		if (attr->ns == NULL) {
			add_soap_fault(this_ptr, "Client", "A SOAP Envelope element cannot have non Namespace qualified attributes", NULL, NULL);
			xmlFreeDoc(response);
			return FALSE;
		} else if (attr_is_equal_ex(attr,"encodingStyle",SOAP_1_2_ENV_NAMESPACE)) {
			if (soap_version == SOAP_1_2) {
				add_soap_fault(this_ptr, "Client", "encodingStyle cannot be specified on the Envelope", NULL, NULL);
				xmlFreeDoc(response);
				return FALSE;
			} else if (strcmp((char*)attr->children->content, SOAP_1_1_ENC_NAMESPACE) != 0) {
				add_soap_fault(this_ptr, "Client", "Unknown data encoding style", NULL, NULL);
				xmlFreeDoc(response);
				return FALSE;
			}
		}
		attr = attr->next;
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
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL && node_is_equal_ex(trav,"Body",envelope_ns)) {
		body = trav;
		trav = trav->next;
	}
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (body == NULL) {
		add_soap_fault(this_ptr, "Client", "Body must be present in a SOAP envelope", NULL, NULL);
		xmlFreeDoc(response);
		return FALSE;
	}
	attr = body->properties;
	while (attr != NULL) {
		if (attr->ns == NULL) {
			if (soap_version == SOAP_1_2) {
				add_soap_fault(this_ptr, "Client", "A SOAP Body element cannot have non Namespace qualified attributes", NULL, NULL);
				xmlFreeDoc(response);
				return FALSE;
			}
		} else if (attr_is_equal_ex(attr,"encodingStyle",SOAP_1_2_ENV_NAMESPACE)) {
			if (soap_version == SOAP_1_2) {
				add_soap_fault(this_ptr, "Client", "encodingStyle cannot be specified on the Body", NULL, NULL);
				xmlFreeDoc(response);
				return FALSE;
			} else if (strcmp((char*)attr->children->content, SOAP_1_1_ENC_NAMESPACE) != 0) {
				add_soap_fault(this_ptr, "Client", "Unknown data encoding style", NULL, NULL);
				xmlFreeDoc(response);
				return FALSE;
			}
		}
		attr = attr->next;
	}
	if (trav != NULL && soap_version == SOAP_1_2) {
		add_soap_fault(this_ptr, "Client", "A SOAP 1.2 envelope can contain only Header and Body", NULL, NULL);
		xmlFreeDoc(response);
		return FALSE;
	}

	if (head != NULL) {
		attr = head->properties;
		while (attr != NULL) {
			if (attr->ns == NULL) {
				add_soap_fault(this_ptr, "Client", "A SOAP Header element cannot have non Namespace qualified attributes", NULL, NULL);
				xmlFreeDoc(response);
				return FALSE;
			} else if (attr_is_equal_ex(attr,"encodingStyle",SOAP_1_2_ENV_NAMESPACE)) {
				if (soap_version == SOAP_1_2) {
					add_soap_fault(this_ptr, "Client", "encodingStyle cannot be specified on the Header", NULL, NULL);
					xmlFreeDoc(response);
					return FALSE;
				} else if (strcmp((char*)attr->children->content, SOAP_1_1_ENC_NAMESPACE) != 0) {
					add_soap_fault(this_ptr, "Client", "Unknown data encoding style", NULL, NULL);
					xmlFreeDoc(response);
					return FALSE;
				}
			}
			attr = attr->next;
		}
	}

	/* Check if <Body> contains <Fault> element */
	fault = get_node_ex(body->children,"Fault",envelope_ns);
	if (fault != NULL) {
		char *faultcode = NULL;
		zend_string *faultstring = NULL, *faultactor = NULL;
		zval details;
		xmlNodePtr tmp;

		ZVAL_UNDEF(&details);
		if (soap_version == SOAP_1_1) {
			tmp = get_node(fault->children, "faultcode");
			if (tmp != NULL && tmp->children != NULL) {
				faultcode = (char*)tmp->children->content;
			}

			tmp = get_node(fault->children, "faultstring");
			if (tmp != NULL && tmp->children != NULL) {
				zval zv;
				master_to_zval(&zv, get_conversion(IS_STRING), tmp);
				faultstring = Z_STR(zv);
			}

			tmp = get_node(fault->children, "faultactor");
			if (tmp != NULL && tmp->children != NULL) {
				zval zv;
				master_to_zval(&zv, get_conversion(IS_STRING), tmp);
				faultactor = Z_STR(zv);
			}

			tmp = get_node(fault->children, "detail");
			if (tmp != NULL) {
				master_to_zval(&details, NULL, tmp);
			}
		} else {
			tmp = get_node(fault->children, "Code");
			if (tmp != NULL && tmp->children != NULL) {
				tmp = get_node(tmp->children, "Value");
				if (tmp != NULL && tmp->children != NULL) {
					faultcode = (char*)tmp->children->content;
				}
			}

			tmp = get_node(fault->children,"Reason");
			if (tmp != NULL && tmp->children != NULL) {
				/* TODO: lang attribute */
				tmp = get_node(tmp->children,"Text");
				if (tmp != NULL && tmp->children != NULL) {
					zval zv;
					master_to_zval(&zv, get_conversion(IS_STRING), tmp);
					faultstring = Z_STR(zv);
				}
			}

			tmp = get_node(fault->children,"Detail");
			if (tmp != NULL) {
				master_to_zval(&details, NULL, tmp);
			}
		}
		add_soap_fault(this_ptr, faultcode, faultstring ? ZSTR_VAL(faultstring) : NULL, faultactor ? ZSTR_VAL(faultactor) : NULL, &details);
		if (faultstring) {
			zend_string_release(faultstring);
		}
		if (faultactor) {
			zend_string_release(faultactor);
		}
		if (Z_REFCOUNTED(details)) {
			Z_DELREF(details);
		}
		xmlFreeDoc(response);
		return FALSE;
	}

	/* Parse content of <Body> element */
	array_init(return_value);
	resp = body->children;
	while (resp != NULL && resp->type != XML_ELEMENT_NODE) {
		resp = resp->next;
	}
	if (resp != NULL) {
		if (fn != NULL && fn->binding && fn->binding->bindingType == BINDING_SOAP) {
		  /* Function has WSDL description */
			sdlParamPtr param = NULL;
			xmlNodePtr val = NULL;
			char *name, *ns = NULL;
			zval tmp;
			sdlSoapBindingFunctionPtr fnb = (sdlSoapBindingFunctionPtr)fn->bindingAttributes;
			int res_count;

			hdrs = fnb->output.headers;

			if (fn->responseParameters) {
				res_count = zend_hash_num_elements(fn->responseParameters);
				ZEND_HASH_FOREACH_PTR(fn->responseParameters, param) {
					if (fnb->style == SOAP_DOCUMENT) {
						if (param->element) {
							name = param->element->name;
							ns = param->element->namens;
/*
							name = param->encode->details.type_str;
							ns = param->encode->details.ns;
*/
						} else {
							name = param->paramName;
						}
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
					if (!cur && fnb->style == SOAP_RPC) {
					  cur = resp;
					}
					if (cur) {
						if (fnb->style == SOAP_DOCUMENT) {
							val = cur;
						} else {
							val = get_node(cur->children, param->paramName);
							if (res_count == 1) {
								if (val == NULL) {
									val = get_node(cur->children, "return");
								}
								if (val == NULL) {
									val = get_node(cur->children, "result");
								}
								if (val == NULL && cur->children && cur->children->next == NULL) {
									val = cur->children;
								}
							}
						}
					}

					if (!val) {
						/* TODO: may be "nil" is not OK? */
						ZVAL_NULL(&tmp);
/*
						add_soap_fault(this_ptr, "Client", "Can't find response data", NULL, NULL);
						xmlFreeDoc(response);
						return FALSE;
*/
					} else {
						/* Decoding value of parameter */
						if (param != NULL) {
							master_to_zval(&tmp, param->encode, val);
						} else {
							master_to_zval(&tmp, NULL, val);
						}
					}
					add_assoc_zval(return_value, param->paramName, &tmp);

					param_count++;
				} ZEND_HASH_FOREACH_END();
			}
		} else {
		  /* Function has no WSDL description */
			xmlNodePtr val;
			val = resp->children;
			while (val != NULL) {
				while (val && val->type != XML_ELEMENT_NODE) {
					val = val->next;
				}
				if (val != NULL) {
					if (!node_is_equal_ex(val,"result",RPC_SOAP12_NAMESPACE)) {
						zval tmp;
						zval *arr;

						master_to_zval(&tmp, NULL, val);
						if (val->name) {
							if ((arr = zend_hash_str_find(Z_ARRVAL_P(return_value), (char*)val->name, strlen((char*)val->name))) != NULL) {
								add_next_index_zval(arr, &tmp);
							} else if (val->next && get_node(val->next, (char*)val->name)) {
								zval arr;

								array_init(&arr);
								add_next_index_zval(&arr, &tmp);
								add_assoc_zval(return_value, (char*)val->name, &arr);
							} else {
								add_assoc_zval(return_value, (char*)val->name, &tmp);
							}
						} else {
							add_next_index_zval(return_value, &tmp);
						}
						++param_count;
					}
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
			zval *tmp;

			zend_hash_internal_pointer_reset(Z_ARRVAL_P(return_value));
			tmp = zend_hash_get_current_data(Z_ARRVAL_P(return_value));
			if (!Z_REFCOUNTED_P(return_value)) {
				ZVAL_COPY(return_value, tmp);
			} else {
				zend_refcounted *garbage = Z_COUNTED_P(return_value);
				ZVAL_COPY(return_value, tmp);
				_zval_dtor_func(garbage ZEND_FILE_LINE_CC);
			}
		}
	}

	if (soap_headers && head) {
		trav = head->children;
		while (trav != NULL) {
			if (trav->type == XML_ELEMENT_NODE) {
				encodePtr enc = NULL;
				zval val;

				if (hdrs) {
					smart_str key = {0};
					sdlSoapBindingFunctionHeaderPtr hdr;

					if (trav->ns) {
						smart_str_appends(&key, (char*)trav->ns->href);
						smart_str_appendc(&key,':');
					}
					smart_str_appends(&key, (char*)trav->name);
					smart_str_0(&key);
					if ((hdr = zend_hash_find_ptr(hdrs, key.s)) != NULL) {
						enc = hdr->encode;
					}
					smart_str_free(&key);
				}
				master_to_zval(&val, enc, trav);
				add_assoc_zval(soap_headers, (char*)trav->name, &val);
			}
			trav = trav->next;
		}
	}

	xmlFreeDoc(response);
	return TRUE;
}
