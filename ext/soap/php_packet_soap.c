#include "php_soap.h"

int parse_packet_soap(zval *this_ptr, char *buffer, int buffer_size, sdlFunctionPtr fn, char *fn_name, zval ***ret, int *num_params)
{
	xmlDocPtr response;
	xmlNodePtr trav, trav2, env, body, resp, cur, fault;
	zval **tmp_ret;
	TSRMLS_FETCH();

	response = xmlParseMemory(buffer, buffer_size);
	xmlCleanupParser();

	(*num_params) = 0;

	trav = response->children;
	FOREACHNODE(trav,"Envelope",env)
	{
		trav2 = env->children;
		FOREACHNODE(trav2,"Body",body)
		{
			fault = get_node(body->children,"Fault");
			if(fault != NULL)
			{
				char *faultcode = NULL, *faultstring = NULL, *faultactor = NULL;
				zval *details = NULL;
				xmlNodePtr tmp;

				tmp = get_node(fault->children,"faultcode");
				if(tmp != NULL && tmp->children != NULL)
					faultcode = tmp->children->content;

				tmp = get_node(fault->children,"faultstring");
				if(tmp != NULL && tmp->children != NULL)
					faultstring = tmp->children->content;

				tmp = get_node(fault->children,"faultactor");
				if(tmp != NULL && tmp->children != NULL)
					faultactor = tmp->children->content;

				tmp = get_node(fault->children,"detail");
				if(tmp != NULL)
				{
					encodePtr enc;
					enc = get_conversion(UNKNOWN_TYPE);
					details = enc->to_zval(enc->details, tmp);
				}

				add_soap_fault(this_ptr, faultcode, faultstring, faultactor, details);
			}
			else
			{
				resp = body->children;
				if(fn != NULL)
				{
					cur = get_node(resp, fn->responseName);
					if(cur != NULL)
					{
						int num_resp = zend_hash_num_elements(fn->responseParameters);
						if(num_resp <= 1)
						{
							sdlParamPtr *h_param, param;
							xmlNodePtr val;

							zend_hash_internal_pointer_reset(fn->responseParameters);
							if(zend_hash_get_current_data(fn->responseParameters, (void **)&h_param) == SUCCESS)
							{
								param = (*h_param);
								val = get_node(cur->children, param->paramName);
								if(val != NULL)
								{
									encodePtr enc;
									tmp_ret = emalloc(sizeof(zval **));
									if(param != NULL)
										enc = param->encode;
									else
										enc = get_conversion(UNKNOWN_TYPE);

									tmp_ret[0] = master_to_zval(enc, val);
									(*ret) = tmp_ret;
									(*num_params) = 1;
								}
								else
									php_error(E_ERROR, "Can't find response parameter \"%s\"", param->paramName);
							}
						}
						else
						{
							php_error(E_ERROR,"Doesn't handle multiple return values");
						}
					}
				}
				else
				{
					cur = resp;
					while(cur && cur->type != XML_ELEMENT_NODE)
							cur = cur->next;
					if(cur != NULL)
					{
						xmlNodePtr val;
						val = cur->children;
						while(val && val->type != XML_ELEMENT_NODE)
							val = val->next;

						if(val != NULL)
						{
							encodePtr enc;
							enc = get_conversion(UNKNOWN_TYPE);
							tmp_ret = emalloc(sizeof(zval **));
							tmp_ret[0] = master_to_zval(enc, val);
							(*ret) = tmp_ret;
							(*num_params) = 1;
						}
					}
				}
			}
		}
		ENDFOREACH(trav2);
	}
	ENDFOREACH(trav);
	xmlFreeDoc(response);
	return TRUE;
}
