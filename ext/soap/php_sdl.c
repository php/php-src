#include "php_soap.h"
#include "libxml/uri.h"

typedef struct sdlCtx {
	sdlPtr root;
	HashTable messages;
	HashTable bindings;
	HashTable portTypes;
	HashTable services;
} sdlCtx;

static void delete_binding(void *binding);
static void delete_function(void *function);
static void delete_paramater(void *paramater);
static void delete_document(void *doc_ptr);

encodePtr get_encoder_from_prefix(sdlPtr sdl, xmlNodePtr data, const char *type)
{
	encodePtr enc = NULL;
	TSRMLS_FETCH();

	enc = get_conversion_from_type(data, type);
	if (enc == NULL && sdl) {
		enc = get_conversion_from_type_ex(sdl->encoders, data, type);
	}
	if (enc == NULL) {
		enc = get_conversion(UNKNOWN_TYPE);
	}
	return enc;
}

static sdlTypePtr get_element(sdlPtr sdl, xmlNodePtr node, const char *type)
{
	sdlTypePtr ret = NULL;
	TSRMLS_FETCH();

	if (sdl && sdl->elements) {
		xmlNsPtr nsptr;
		char *ns, *cptype;
		sdlTypePtr *sdl_type;

		parse_namespace(type, &cptype, &ns);
		nsptr = xmlSearchNs(node->doc, node, ns);
		if (nsptr != NULL) {
			smart_str nscat = {0};

			smart_str_appends(&nscat, nsptr->href);
			smart_str_appendc(&nscat, ':');
			smart_str_appends(&nscat, cptype);
			smart_str_0(&nscat);

			if (zend_hash_find(sdl->elements, nscat.c, nscat.len + 1, (void **)&sdl_type) == SUCCESS) {
				ret = *sdl_type;
			} else if (zend_hash_find(sdl->elements, (char*)type, strlen(type) + 1, (void **)&sdl_type) == SUCCESS) {
				ret = *sdl_type;
			}
			smart_str_free(&nscat);
		} else {
			if (zend_hash_find(sdl->elements, (char*)type, strlen(type) + 1, (void **)&sdl_type) == SUCCESS) {
				ret = *sdl_type;
			}
		}

		if (cptype) {efree(cptype);}
		if (ns) {efree(ns);}
	}
	return ret;
}

encodePtr get_encoder(sdlPtr sdl, const char *ns, const char *type)
{
	encodePtr enc = NULL;
	char *nscat;

	nscat = emalloc(strlen(ns) + strlen(type) + 2);
	sprintf(nscat, "%s:%s", ns, type);

	enc = get_encoder_ex(sdl, nscat);

	efree(nscat);
	return enc;
}

encodePtr get_encoder_ex(sdlPtr sdl, const char *nscat)
{
	encodePtr enc = NULL;
	TSRMLS_FETCH();

	enc = get_conversion_from_href_type(nscat);
	if (enc == NULL && sdl) {
		enc = get_conversion_from_href_type_ex(sdl->encoders, nscat, strlen(nscat));
	}
	if (enc == NULL) {
		enc = get_conversion(UNKNOWN_TYPE);
	}
	return enc;
}

sdlBindingPtr get_binding_from_type(sdlPtr sdl, int type)
{
	sdlBindingPtr *binding;

	if (sdl == NULL) {
		return NULL;
	}

	for (zend_hash_internal_pointer_reset(sdl->bindings);
		zend_hash_get_current_data(sdl->bindings, (void **) &binding) == SUCCESS;
		zend_hash_move_forward(sdl->bindings)) {
		if ((*binding)->bindingType == type) {
			return *binding;
		}
	}
	return NULL;
}

sdlBindingPtr get_binding_from_name(sdlPtr sdl, char *name, char *ns)
{
	sdlBindingPtr binding = NULL;
	smart_str key = {0};

	smart_str_appends(&key, ns);
	smart_str_appendc(&key, ':');
	smart_str_appends(&key, name);
	smart_str_0(&key);

	zend_hash_find(sdl->bindings, key.c, key.len, (void **)&binding);

	smart_str_free(&key);
	return binding;
}

static void load_wsdl_ex(char *struri, sdlCtx *ctx, int include)
{
	sdlPtr tmpsdl = ctx->root;
	xmlDocPtr wsdl;
	xmlNodePtr root, definitions, trav;
	xmlAttrPtr targetNamespace;
	int old_error_reporting;
	TSRMLS_FETCH();

	if (zend_hash_exists(&tmpsdl->docs, struri, strlen(struri)+1)) {
	  return;
	}

	/* TODO: WSDL Caching */

	old_error_reporting = EG(error_reporting);
	EG(error_reporting) &= ~(E_WARNING|E_NOTICE|E_USER_WARNING|E_USER_NOTICE);

	wsdl = xmlParseFile(struri);
	xmlCleanupParser();

	EG(error_reporting) = old_error_reporting;


	if (!wsdl) {
		php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: Couldn't load from '%s'", struri);
	}

	zend_hash_add(&tmpsdl->docs, struri, strlen(struri)+1, (void**)&wsdl, sizeof(xmlDocPtr), NULL);

	root = wsdl->children;
	definitions = get_node(root, "definitions");
	if (!definitions) {
		if (include) {
			xmlNodePtr schema = get_node(root, "schema");
			if (schema) {
				load_schema(tmpsdl, schema);
				return;
			}
		}
		php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: Couldn't find <definitions> in '%s'", struri);
	}

	if (!include) {
		targetNamespace = get_attribute(definitions->properties, "targetNamespace");
		if (targetNamespace) {
			tmpsdl->target_ns = strdup(targetNamespace->children->content);
		}
	}

	trav = definitions->children;
	while (trav != NULL) {
		if (trav->type == XML_ELEMENT_NODE) {
			if (strcmp(trav->name,"types") == 0) {
				/* TODO: Only one "types" is allowed */
				xmlNodePtr trav2 = trav->children;
				xmlNodePtr schema;
				FOREACHNODE(trav2, "schema", schema) {
					load_schema(tmpsdl, schema);
				}
				ENDFOREACH(trav2);

			} else if (strcmp(trav->name,"import") == 0) {
				/* TODO: namespace ??? */
				xmlAttrPtr tmp = get_attribute(trav->properties, "location");
				if (tmp) {
				  xmlChar *uri;
					xmlChar *base = xmlNodeGetBase(trav->doc, trav);

					if (base == NULL) {
				    uri = xmlBuildURI(tmp->children->content, trav->doc->URL);
					} else {
	    			uri = xmlBuildURI(tmp->children->content, base);
				    xmlFree(base);
					}
					load_wsdl_ex(uri, ctx, 1);
			    xmlFree(uri);
				}

			} else if (strcmp(trav->name,"message") == 0) {
				xmlAttrPtr name = get_attribute(trav->properties, "name");
				if (name && name->children && name->children->content) {
					if (zend_hash_add(&ctx->messages, name->children->content, strlen(name->children->content)+1,&trav, sizeof(xmlNodePtr), NULL) != SUCCESS) {
						php_error(E_ERROR,"SOAP-ERROR: Parsing WSDL: <message> '%s' already defined",name->children->content);
					}
				} else {
					php_error(E_ERROR,"SOAP-ERROR: Parsing WSDL: <message> hasn't name attribute");
				}

			} else if (strcmp(trav->name,"portType") == 0) {
				xmlAttrPtr name = get_attribute(trav->properties, "name");
				if (name && name->children && name->children->content) {
					if (zend_hash_add(&ctx->portTypes, name->children->content, strlen(name->children->content)+1,&trav, sizeof(xmlNodePtr), NULL) != SUCCESS) {
						php_error(E_ERROR,"SOAP-ERROR: Parsing WSDL: <portType> '%s' already defined",name->children->content);
					}
				} else {
					php_error(E_ERROR,"SOAP-ERROR: Parsing WSDL: <portType> hasn't name attribute");
				}

			} else if (strcmp(trav->name,"binding") == 0) {
				xmlAttrPtr name = get_attribute(trav->properties, "name");
				if (name && name->children && name->children->content) {
					if (zend_hash_add(&ctx->bindings, name->children->content, strlen(name->children->content)+1,&trav, sizeof(xmlNodePtr), NULL) != SUCCESS) {
						php_error(E_ERROR,"SOAP-ERROR: Parsing WSDL: <binding> '%s' already defined",name->children->content);
					}
				} else {
					php_error(E_ERROR,"SOAP-ERROR: Parsing WSDL: <binding> hasn't name attribute");
				}

			} else if (strcmp(trav->name,"service") == 0) {
				xmlAttrPtr name = get_attribute(trav->properties, "name");
				if (name && name->children && name->children->content) {
					if (zend_hash_add(&ctx->services, name->children->content, strlen(name->children->content)+1,&trav, sizeof(xmlNodePtr), NULL) != SUCCESS) {
						php_error(E_ERROR,"SOAP-ERROR: Parsing WSDL: <service> '%s' already defined",name->children->content);
					}
				} else {
					php_error(E_ERROR,"SOAP-ERROR: Parsing WSDL: <service> hasn't name attribute");
				}

			} else {
			  /* TODO: extensibility elements */
			}
		}
		trav = trav->next;
	}
}

static sdlPtr load_wsdl(char *struri)
{
	sdlCtx ctx;
	int i,n;

	ctx.root = malloc(sizeof(sdl));
	memset(ctx.root, 0, sizeof(sdl));
	ctx.root->source = strdup(struri);
	zend_hash_init(&ctx.root->docs, 0, NULL, delete_document, 1);
	zend_hash_init(&ctx.root->functions, 0, NULL, delete_function, 1);

	zend_hash_init(&ctx.messages, 0, NULL, NULL, 0);
	zend_hash_init(&ctx.bindings, 0, NULL, NULL, 0);
	zend_hash_init(&ctx.portTypes, 0, NULL, NULL, 0);
	zend_hash_init(&ctx.services,  0, NULL, NULL, 0);

	load_wsdl_ex(struri,&ctx, 0);

	schema_pass2(ctx.root);

	n = zend_hash_num_elements(&ctx.services);
	if (n > 0) {
		zend_hash_internal_pointer_reset(&ctx.services);
		for (i = 0; i < n; i++) {
			xmlNodePtr *tmp, service;
			xmlNodePtr trav, port;

			zend_hash_get_current_data(&ctx.services, (void **)&tmp);
			service = *tmp;

			trav = service->children;
			FOREACHNODE(trav, "port", port) {
				xmlAttrPtr type, name, bindingAttr, location;
				xmlNodePtr portType, operation;
				xmlNodePtr address, binding, trav2;
				char *ns, *ctype;
				sdlBindingPtr tmpbinding;
				char *wsdl_soap_namespace = NULL;

				tmpbinding = malloc(sizeof(sdlBinding));
				memset(tmpbinding, 0, sizeof(sdlBinding));

				bindingAttr = get_attribute(port->properties, "binding");
				if (bindingAttr == NULL) {
					php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: No binding associated with <port>");
				}

				/* find address and figure out binding type */
				address = get_node(port->children, "address");
				if (!address) {
					php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: No address associated with <port>");
				}

				location = get_attribute(address->properties, "location");
				if (!location) {
					php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: No location associated with <port>");
				}

				tmpbinding->location = strdup(location->children->content);

				if (address->ns && !strcmp(address->ns->href, WSDL_SOAP11_NAMESPACE)) {
					wsdl_soap_namespace = WSDL_SOAP11_NAMESPACE;
					tmpbinding->bindingType = BINDING_SOAP;
				} else if (address->ns && !strcmp(address->ns->href, WSDL_SOAP12_NAMESPACE)) {
					wsdl_soap_namespace = WSDL_SOAP12_NAMESPACE;
					tmpbinding->bindingType = BINDING_SOAP;
				} else if (address->ns && !strcmp(address->ns->href, RPC_SOAP12_NAMESPACE)) {
					wsdl_soap_namespace = RPC_SOAP12_NAMESPACE;
					tmpbinding->bindingType = BINDING_SOAP;
				} else if (address->ns && !strcmp(address->ns->href, WSDL_HTTP11_NAMESPACE)) {
					tmpbinding->bindingType = BINDING_HTTP;
				} else if (address->ns && !strcmp(address->ns->href, WSDL_HTTP12_NAMESPACE)) {
					tmpbinding->bindingType = BINDING_HTTP;
				} else {
					if (address->ns) {
						php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: PHP-SOAP doesn't support binding '%s'",address->ns->href);
					} else {
						php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: Unknown binding type");
					}
				}

				parse_namespace(bindingAttr->children->content, &ctype, &ns);
				if (zend_hash_find(&ctx.bindings, ctype, strlen(ctype)+1, (void*)&tmp) != SUCCESS) {
					php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: No <binding> element with name '%s'", ctype);
				}
				binding = *tmp;

				if (ns) {efree(ns);}
				if (ctype) {efree(ctype);}

				if (tmpbinding->bindingType == BINDING_SOAP) {
					sdlSoapBindingPtr soapBinding;
					xmlNodePtr soapBindingNode;
					xmlAttrPtr tmp;

					soapBinding = malloc(sizeof(sdlSoapBinding));
					memset(soapBinding, 0, sizeof(sdlSoapBinding));
					soapBinding->style = SOAP_DOCUMENT;

					soapBindingNode = get_node_ex(binding->children, "binding", wsdl_soap_namespace);
					if (soapBindingNode) {
						tmp = get_attribute(soapBindingNode->properties, "style");
						if (tmp && !strcmp(tmp->children->content, "rpc")) {
							soapBinding->style = SOAP_RPC;
						}

						tmp = get_attribute(soapBindingNode->properties, "transport");
						if (tmp) {
							if (strcmp(tmp->children->content, WSDL_HTTP_TRANSPORT)) {
								php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: PHP-SOAP doesn't support transport '%s'", tmp->children->content);
							}
							soapBinding->transport = strdup(tmp->children->content);
						}
					}
					tmpbinding->bindingAttributes = (void *)soapBinding;
				}

				name = get_attribute(binding->properties, "name");
				if (name == NULL) {
					php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: Missing 'name' attribute for <binding>");
				}
				tmpbinding->name = strdup(name->children->content);

				type = get_attribute(binding->properties, "type");
				if (type == NULL) {
					php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: Missing 'type' attribute for <binding>");
				}
				parse_namespace(type->children->content, &ctype, &ns);

				if (zend_hash_find(&ctx.portTypes, ctype, strlen(ctype)+1, (void**)&tmp) != SUCCESS) {
					php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: Missing <portType> with name '%s'", name->children->content);
				}
				portType = *tmp;

				if (ctype) {efree(ctype);}
				if (ns) {efree(ns);}

				trav2 = binding->children;
				FOREACHNODE(trav2, "operation", operation) {
					sdlFunctionPtr function;
					xmlNodePtr input, output, fault, portTypeOperation, portTypeInput, portTypeOutput, msgInput, msgOutput;
					xmlAttrPtr op_name, paramOrder;

					op_name = get_attribute(operation->properties, "name");
					if (op_name == NULL) {
						php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: Missing 'name' attribute for <operation>");
					}

					portTypeOperation = get_node_with_attribute(portType->children, "operation", "name", op_name->children->content);
					if (portTypeOperation == NULL) {
						php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: Missing <portType>/<operation> with name '%s'", op_name->children->content);
					}

					function = malloc(sizeof(sdlFunction));
					function->functionName = strdup(op_name->children->content);
					function->requestParameters = NULL;
					function->responseParameters = NULL;
					function->responseName = NULL;
					function->requestName = NULL;
					function->bindingAttributes = NULL;

					if (tmpbinding->bindingType == BINDING_SOAP) {
						sdlSoapBindingFunctionPtr soapFunctionBinding;
						sdlSoapBindingPtr soapBinding;
						xmlNodePtr soapOperation;
						xmlAttrPtr tmp;

						soapFunctionBinding = malloc(sizeof(sdlSoapBindingFunction));
						memset(soapFunctionBinding, 0, sizeof(sdlSoapBindingFunction));
						soapBinding = (sdlSoapBindingPtr)tmpbinding->bindingAttributes;
						soapFunctionBinding->style = soapBinding->style;

						soapOperation = get_node_ex(operation->children, "operation", wsdl_soap_namespace);
						if (soapOperation) {
							tmp = get_attribute(soapOperation->properties, "soapAction");
							if (tmp) {
								soapFunctionBinding->soapAction = strdup(tmp->children->content);
							}

							tmp = get_attribute(soapOperation->properties, "style");
							if (tmp) {
								if (!strcmp(tmp->children->content, "rpc")) {
									soapFunctionBinding->style = SOAP_RPC;
								} else {
									soapFunctionBinding->style = SOAP_DOCUMENT;
								}
							} else {
								soapFunctionBinding->style = soapBinding->style;
							}
						}

						function->bindingAttributes = (void *)soapFunctionBinding;
					}

					input = get_node(operation->children, "input");
					portTypeInput = get_node(portTypeOperation->children, "input");

					output = get_node(operation->children, "output");
					portTypeOutput = get_node(portTypeOperation->children, "output");

					if (input != NULL) {
						xmlAttrPtr message, name;
						xmlNodePtr part, trav3;
						char *ns, *ctype;

						if (portTypeInput) {
							message = get_attribute(portTypeInput->properties, "message");
							if (message == NULL) {
								php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: Missing name for <input> of '%s'", op_name->children->content);
							}

							name = get_attribute(portTypeInput->properties, "name");
							if (name != NULL) {
								function->requestName = strdup(name->children->content);
							} else {
								function->requestName = strdup(function->functionName);
							}
							function->requestParameters = malloc(sizeof(HashTable));
							zend_hash_init(function->requestParameters, 0, NULL, delete_paramater, 1);

							parse_namespace(message->children->content, &ctype, &ns);

							if (zend_hash_find(&ctx.messages, ctype, strlen(ctype)+1, (void**)&tmp) != SUCCESS) {
								php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: Missing <message> with name '%s'", message->children->content);
							}
							msgInput = *tmp;

							if (ctype) {efree(ctype);}
							if (ns) {efree(ns);}

							if (tmpbinding->bindingType == BINDING_SOAP) {
								sdlSoapBindingFunctionPtr soapFunctionBinding = function->bindingAttributes;
								xmlNodePtr body;
								xmlAttrPtr tmp;

								body = get_node_ex(input->children, "body", wsdl_soap_namespace);
								if (body) {
									tmp = get_attribute(body->properties, "use");
									if (tmp && !strcmp(tmp->children->content, "literal")) {
										soapFunctionBinding->input.use = SOAP_LITERAL;
									} else {
										soapFunctionBinding->input.use = SOAP_ENCODED;
									}

									tmp = get_attribute(body->properties, "namespace");
									if (tmp) {
										soapFunctionBinding->input.ns = strdup(tmp->children->content);
									}

									tmp = get_attribute(body->properties, "parts");
									if (tmp) {
										soapFunctionBinding->input.parts = strdup(tmp->children->content);
									}

									tmp = get_attribute(body->properties, "encodingStyle");
									if (tmp) {
										soapFunctionBinding->input.encodingStyle = strdup(tmp->children->content);
									}
								}
							}

							trav3 = msgInput->children;
							FOREACHNODE(trav3, "part", part) {
								xmlAttrPtr element, type, name;
								sdlParamPtr param;

								param = malloc(sizeof(sdlParam));
								memset(param,0,sizeof(sdlParam));
								param->order = 0;

								name = get_attribute(part->properties, "name");
								if (name == NULL) {
									php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: No name associated with <part> '%s'", msgInput->name);
								}

								param->paramName = strdup(name->children->content);

								type = get_attribute(part->properties, "type");
								if (type != NULL) {
									param->encode = get_encoder_from_prefix(ctx.root, part, type->children->content);
								} else {
									element = get_attribute(part->properties, "element");
									if (element != NULL) {
										param->element = get_element(ctx.root, part, element->children->content);
										if (param->element) {
											param->encode = param->element->encode;
										}
									}
								}

								zend_hash_next_index_insert(function->requestParameters, &param, sizeof(sdlParamPtr), NULL);
							}
							ENDFOREACH(trav3);
						}

					}

					if (output != NULL) {
						xmlAttrPtr message, name;
						xmlNodePtr part, trav3;
						char *ns, *ctype;

						if (portTypeOutput) {
							name = get_attribute(portTypeOutput->properties, "name");
							if (name != NULL) {
								function->responseName = strdup(name->children->content);
							} else if (input == NULL) {
								function->responseName = strdup(function->functionName);
							} else {
								function->responseName = malloc(strlen(function->functionName) + strlen("Response") + 1);
								sprintf(function->responseName, "%sResponse", function->functionName);
							}
							function->responseParameters = malloc(sizeof(HashTable));
							zend_hash_init(function->responseParameters, 0, NULL, delete_paramater, 1);

							message = get_attribute(portTypeOutput->properties, "message");
							if (message == NULL) {
								php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: Missing name for <output> of '%s'", op_name->children->content);
							}

							parse_namespace(message->children->content, &ctype, &ns);
							if (zend_hash_find(&ctx.messages, ctype, strlen(ctype)+1, (void**)&tmp) != SUCCESS) {
								php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: Missing <message> with name '%s'", message->children->content);
							}
							msgOutput = *tmp;

							if (ctype) {efree(ctype);}
							if (ns) {efree(ns);}

							if (tmpbinding->bindingType == BINDING_SOAP) {
								sdlSoapBindingFunctionPtr soapFunctionBinding = function->bindingAttributes;
								xmlNodePtr body;
								xmlAttrPtr tmp;

								body = get_node_ex(output->children, "body", wsdl_soap_namespace);
								if (body) {
									tmp = get_attribute(body->properties, "use");
									if (tmp && !strcmp(tmp->children->content, "literal")) {
										soapFunctionBinding->output.use = SOAP_LITERAL;
									} else {
										soapFunctionBinding->output.use = SOAP_ENCODED;
									}

									tmp = get_attribute(body->properties, "namespace");
									if (tmp) {
										soapFunctionBinding->output.ns = strdup(tmp->children->content);
									}

									tmp = get_attribute(body->properties, "parts");
									if (tmp) {
										soapFunctionBinding->output.parts = strdup(tmp->children->content);
									}

									tmp = get_attribute(body->properties, "encodingStyle");
									if (tmp) {
										soapFunctionBinding->output.encodingStyle = strdup(tmp->children->content);
									}
								}
							}

							trav3 = msgOutput->children;
							FOREACHNODE(trav3, "part", part) {
								sdlParamPtr param;
								xmlAttrPtr element, type, name;

								param = malloc(sizeof(sdlParam));
								memset(param, 0, sizeof(sdlParam));
								param->order = 0;

								name = get_attribute(part->properties, "name");
								if (name == NULL) {
									php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: No name associated with <part> '%s'", msgOutput->name);
								}

								param->paramName = strdup(name->children->content);


								type = get_attribute(part->properties, "type");
								if (type) {
									param->encode = get_encoder_from_prefix(ctx.root, part, type->children->content);
								} else {
									element = get_attribute(part->properties, "element");
									if (element) {
										param->element = get_element(ctx.root, part, element->children->content);
										if (param->element) {
											param->encode = param->element->encode;
										}
									}
								}

								zend_hash_next_index_insert(function->responseParameters, &param, sizeof(sdlParamPtr), NULL);
							}
							ENDFOREACH(trav3);
						}
					}

					paramOrder = get_attribute(portTypeOperation->properties, "parameterOrder");
					if (paramOrder) {
						/* FIXME: */
					}

					fault = get_node(operation->children, "fault");
					if (!fault) {
					}

					function->binding = tmpbinding;

					{
						char *tmp = estrdup(function->functionName);
						int  len = strlen(tmp);

						zend_hash_add(&ctx.root->functions, php_strtolower(tmp, len), len+1, &function, sizeof(sdlFunctionPtr), NULL);
						efree(tmp);
						if (function->requestName != NULL && strcmp(function->requestName,function->functionName) != 0) {
							if (ctx.root->requests == NULL) {
								ctx.root->requests = malloc(sizeof(HashTable));
								zend_hash_init(ctx.root->requests, 0, NULL, NULL, 1);
							}
							tmp = estrdup(function->requestName);
							len = strlen(tmp);
							zend_hash_add(ctx.root->requests, php_strtolower(tmp, len), len+1, &function, sizeof(sdlFunctionPtr), NULL);
							efree(tmp);
						}
					}
				}
				ENDFOREACH(trav2);

				if (!ctx.root->bindings) {
					ctx.root->bindings = malloc(sizeof(HashTable));
					zend_hash_init(ctx.root->bindings, 0, NULL, delete_binding, 1);
				}

				zend_hash_add(ctx.root->bindings, tmpbinding->name, strlen(tmpbinding->name), &tmpbinding, sizeof(sdlBindingPtr), NULL);
			}
			ENDFOREACH(trav);

			zend_hash_move_forward(&ctx.services);
		}
	} else {
		php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: Couldn't bind to service");
	}

	schema_pass3(ctx.root);
	zend_hash_destroy(&ctx.messages);
	zend_hash_destroy(&ctx.bindings);
	zend_hash_destroy(&ctx.portTypes);
	zend_hash_destroy(&ctx.services);

	return ctx.root;
}

sdlPtr get_sdl(char *uri)
{
	sdlPtr tmp, *hndl;
	TSRMLS_FETCH();

	tmp = NULL;
	hndl = NULL;
	if (zend_hash_find(SOAP_GLOBAL(sdls), uri, strlen(uri), (void **)&hndl) == FAILURE) {
		tmp = load_wsdl(uri);
		zend_hash_add(SOAP_GLOBAL(sdls), uri, strlen(uri), &tmp, sizeof(sdlPtr), NULL);
	} else {
		tmp = *hndl;
	}

	return tmp;
}

/* Deletes */
void delete_sdl(void *handle)
{
	sdlPtr tmp = *((sdlPtr*)handle);

	zend_hash_destroy(&tmp->docs);
	zend_hash_destroy(&tmp->functions);
	if (tmp->source) {
		free(tmp->source);
	}
	if (tmp->target_ns) {
		free(tmp->target_ns);
	}
	if (tmp->encoders) {
		zend_hash_destroy(tmp->encoders);
		free(tmp->encoders);
	}
	if (tmp->types) {
		zend_hash_destroy(tmp->types);
		free(tmp->types);
	}
	if (tmp->elements) {
		zend_hash_destroy(tmp->elements);
		free(tmp->elements);
	}
	if (tmp->attributes) {
		zend_hash_destroy(tmp->attributes);
		free(tmp->attributes);
	}
	if (tmp->attributeGroups) {
		zend_hash_destroy(tmp->attributeGroups);
		free(tmp->attributeGroups);
	}
	if (tmp->groups) {
		zend_hash_destroy(tmp->groups);
		free(tmp->groups);
	}
	if (tmp->bindings) {
		zend_hash_destroy(tmp->bindings);
		free(tmp->bindings);
	}
	if (tmp->requests) {
		zend_hash_destroy(tmp->requests);
		free(tmp->requests);
	}
	free(tmp);
}

static void delete_binding(void *data)
{
	sdlBindingPtr binding = *((sdlBindingPtr*)data);

	if (binding->location) {
		free(binding->location);
	}
	if (binding->name) {
		free(binding->name);
	}

	if (binding->bindingType == BINDING_SOAP) {
		sdlSoapBindingPtr soapBind = binding->bindingAttributes;
		if (soapBind && soapBind->transport) {
			free(soapBind->transport);
		}
	}
}

static void delete_sdl_soap_binding_function_body(sdlSoapBindingFunctionBody body)
{
	if (body.ns) {
		free(body.ns);
	}
	if (body.parts) {
		free(body.parts);
	}
	if (body.encodingStyle) {
		free(body.encodingStyle);
	}
}

static void delete_function(void *data)
{
	sdlFunctionPtr function = *((sdlFunctionPtr*)data);

	if (function->functionName) {
		free(function->functionName);
	}
	if (function->requestName) {
		free(function->requestName);
	}
	if (function->responseName) {
		free(function->responseName);
	}
	if (function->requestParameters) {
		zend_hash_destroy(function->requestParameters);
		free(function->requestParameters);
	}
	if (function->responseParameters) {
		zend_hash_destroy(function->responseParameters);
		free(function->responseParameters);
	}

	if (function->bindingAttributes &&
	    function->binding && function->binding->bindingType == BINDING_SOAP) {
		sdlSoapBindingFunctionPtr soapFunction = function->bindingAttributes;
		if (soapFunction->soapAction) {
			free(soapFunction->soapAction);
		}
		delete_sdl_soap_binding_function_body(soapFunction->input);
		delete_sdl_soap_binding_function_body(soapFunction->output);
		delete_sdl_soap_binding_function_body(soapFunction->falut);
	}
}

static void delete_paramater(void *data)
{
	sdlParamPtr param = *((sdlParamPtr*)data);
	if (param->paramName) {
		free(param->paramName);
	}
	free(param);
}

static void delete_document(void *doc_ptr)
{
	xmlDocPtr doc = *((xmlDocPtr*)doc_ptr);
	xmlFreeDoc(doc);
}
