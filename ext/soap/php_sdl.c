#include "php_soap.h"

encodePtr get_encoder_from_prefix(sdlPtr sdl, xmlNodePtr data, char *type)
{
	encodePtr enc = NULL;
	TSRMLS_FETCH();

	enc = get_conversion_from_type(data, type);
	if(enc == NULL && sdl)
		enc = get_conversion_from_type_ex(sdl->encoders, data, type);
	if(enc == NULL)
		enc = get_conversion(UNKNOWN_TYPE);

	return enc;
}

encodePtr get_encoder(sdlPtr sdl, char *ns, char *type)
{
	encodePtr enc = NULL;
	char *nscat;
	TSRMLS_FETCH();

	nscat = emalloc(strlen(ns) + strlen(type) + 2);
	sprintf(nscat, "%s:%s", ns, type);

	enc = get_encoder_ex(sdl, nscat);

	efree(nscat);
	return enc;
}

encodePtr get_encoder_ex(sdlPtr sdl, char *nscat)
{
	encodePtr enc = NULL;
	TSRMLS_FETCH();

	enc = get_conversion_from_href_type(nscat);
	if(enc == NULL && sdl)
		enc = get_conversion_from_href_type_ex(sdl->encoders, nscat, strlen(nscat));
	if(enc == NULL)
		enc = get_conversion(UNKNOWN_TYPE);
	return enc;
}

encodePtr get_create_encoder(sdlPtr sdl, sdlTypePtr cur_type, char *ns, char *type)
{
	encodePtr enc = NULL;
	smart_str nscat = {0};
	TSRMLS_FETCH();

	smart_str_appends(&nscat, ns);
	smart_str_appendc(&nscat, ':');
	smart_str_appends(&nscat, type);
	smart_str_0(&nscat);

	enc = get_conversion_from_href_type(nscat.c);
	if(enc == NULL)
		enc = get_conversion_from_href_type_ex(sdl->encoders, nscat.c, nscat.len);
	if(enc == NULL)
		enc = create_encoder(sdl, cur_type, ns, type);

	smart_str_free(&nscat);
	return enc;
}

encodePtr create_encoder(sdlPtr sdl, sdlTypePtr cur_type, char *ns, char *type)
{
	smart_str nscat = {0};
	encodePtr enc;

	enc = malloc(sizeof(encode));
	memset(enc, 0, sizeof(encode));

	smart_str_appends(&nscat, ns);
	smart_str_appendc(&nscat, ':');
	smart_str_appends(&nscat, type);
	smart_str_0(&nscat);

	enc->details.ns = strdup(ns);
	enc->details.type_str = strdup(type);
	enc->details.sdl_type = cur_type;
	enc->to_xml = sdl_guess_convert_xml;
	enc->to_zval = sdl_guess_convert_zval;

	if(sdl->encoders == NULL)
	{
		sdl->encoders = malloc(sizeof(HashTable));
		zend_hash_init(sdl->encoders, 0, NULL, delete_encoder, 1);
	}
	zend_hash_add(sdl->encoders, nscat.c, nscat.len + 1, &enc, sizeof(encodePtr), NULL);
	smart_str_free(&nscat);
	return enc;
}

zval *sdl_guess_convert_zval(encodeType enc, xmlNodePtr data)
{
	sdlTypePtr type;
	zval *ret;

	type = enc.sdl_type;

	if(type->encode)
	{
		if(type->encode->details.type == IS_ARRAY ||
			type->encode->details.type == SOAP_ENC_ARRAY)
			ret = to_zval_array(enc, data);
		else
			ret = master_to_zval(type->encode, data);
	} 
	else if(zend_hash_num_elements(type->elements) == 1)
	{
		sdlTypePtr *t;
		zend_hash_internal_pointer_reset(type->elements);
		if(zend_hash_get_current_data(type->elements, (void **)&t) != FAILURE &&
			(*t)->max_occurs != 1)
			ret = to_zval_array(enc, data);
	}
	if(ret)
		return ret;
	else
		return guess_zval_convert(enc, data);
}
 
xmlNodePtr sdl_guess_convert_xml(encodeType enc, zval *data, int style)
{
	sdlTypePtr type;
	xmlNodePtr ret = NULL;

	type = enc.sdl_type;

	if(type->encode)
	{
		if(type->encode->details.type == IS_ARRAY ||
			type->encode->details.type == SOAP_ENC_ARRAY)
			ret = sdl_to_xml_array(type, data, style);
		else
			ret = master_to_xml(type->encode, data, style);
	}
	else if(type->elements)
	{
		sdlTypePtr *t;
		if(zend_hash_num_elements(type->elements) == 1)
		{
			zend_hash_internal_pointer_reset(type->elements);
			if(zend_hash_get_current_data(type->elements, (void **)&t) != FAILURE &&
				(*t)->max_occurs != 1)
				ret = sdl_to_xml_array((*t), data, style);
		}
		if(!ret)
			ret = sdl_to_xml_object(type, data, style);
	}
	else
		ret = guess_xml_convert(enc, data, style);

	//set_ns_and_type(ret, enc);
	return ret;
}

xmlNodePtr sdl_to_xml_object(sdlTypePtr type, zval *data, int style)
{
	xmlNodePtr ret;
	sdlTypePtr *t, tmp;

	ret = xmlNewNode(NULL, "BOGUS");

	zend_hash_internal_pointer_reset(type->elements);
	while(zend_hash_get_current_data(type->elements, (void **)&t) != FAILURE)
	{
		zval **prop;
 		tmp = *t;
		if(zend_hash_find(Z_OBJPROP_P(data), tmp->name, strlen(tmp->name) + 1, (void **)&prop) == FAILURE)
		{
			if(tmp->nullable == FALSE)
				php_error(E_ERROR, "Error encoding object to xml missing property \"%s\"", tmp->name);
		}
		else
		{
			xmlNodePtr newNode;

			newNode = master_to_xml(tmp->encode, (*prop), style);
			xmlNodeSetName(newNode, tmp->name);
			xmlAddChild(ret, newNode);
		}
		zend_hash_move_forward(type->elements);
	}

	return ret;
}

xmlNodePtr sdl_to_xml_array(sdlTypePtr type, zval *data, int style)
{
	smart_str array_type_and_size = {0}, array_type = {0};
	int i;
	xmlNodePtr xmlParam;
	TSRMLS_FETCH();

	xmlParam = xmlNewNode(NULL,"BOGUS");

	FIND_ZVAL_NULL(data, xmlParam);

	if(Z_TYPE_P(data) == IS_ARRAY)
	{
		sdlAttributePtr *arrayType;
		i = zend_hash_num_elements(Z_ARRVAL_P(data));

		if(style == SOAP_ENCODED)
		{
			if(type->attributes &&
				zend_hash_find(type->attributes, SOAP_ENC_NAMESPACE":arrayType", 
					sizeof(SOAP_ENC_NAMESPACE":arrayType"), 
					(void **)&arrayType) == SUCCESS)
			{
				xmlAttrPtr *wsdl;
				if(zend_hash_find((*arrayType)->extraAttributes, WSDL_NAMESPACE":arrayType", sizeof(WSDL_NAMESPACE":arrayType"), (void **)&wsdl) == SUCCESS)
				{
					char *ns = NULL, *value;
					smart_str *prefix = encode_new_ns();
					smart_str smart_ns = {0};
					xmlNsPtr myNs;

					parse_namespace((*wsdl)->children->content, &value, &ns);
					myNs = xmlSearchNs((*wsdl)->doc, (*wsdl)->parent, ns);

					smart_str_appendl(&smart_ns, "xmlns:", sizeof("xmlns:") - 1);
					smart_str_appendl(&smart_ns, prefix->c, prefix->len);
					smart_str_0(&smart_ns);

					xmlSetProp(xmlParam, smart_ns.c, myNs->href);
					smart_str_appends(&array_type_and_size, prefix->c);
					smart_str_appendc(&array_type_and_size, ':');
					smart_str_appends(&array_type_and_size, value);
					smart_str_0(&array_type_and_size);
				}
			}
			else
			{
				smart_str_appends(&array_type_and_size, type->name);
				smart_str_appendc(&array_type_and_size, '[');
				smart_str_append_long(&array_type_and_size, i);
				smart_str_appendc(&array_type_and_size, ']');
				smart_str_0(&array_type_and_size);
			}
			xmlSetProp(xmlParam, SOAP_ENC_NS_PREFIX":arrayType", array_type_and_size.c);

			smart_str_free(&array_type_and_size);
			smart_str_free(&array_type);
		}

		zend_hash_internal_pointer_reset(data->value.ht);
		for(;i > 0;i--)
		{
			xmlNodePtr xparam;
			zval **zdata;
			encodePtr enc;
			zend_hash_get_current_data(data->value.ht, (void **)&zdata);

			enc = get_conversion((*zdata)->type);
			xparam = master_to_xml(enc, (*zdata), style);

			xmlNodeSetName(xparam, type->name);
			xmlAddChild(xmlParam, xparam);
			zend_hash_move_forward(data->value.ht);
		}
	}

	if(style == SOAP_ENCODED)
		set_ns_and_type_ex(xmlParam, type->namens, type->name);
	return xmlParam;
}

zval *sdl_convert_zval(encodeType enc, xmlNodePtr data)
{
	zval *ret;
	MAKE_STD_ZVAL(ret);
	ZVAL_STRING(ret, "blah", 1);

	return ret;
}

/*
zval *sdl_convert_zval(xmlNodePtr data, sdlTypePtr type)
{
	found = zend_hash_find(EG(class_table), class_name, class_name_len + 1, (void **)&ce);
	if(found != FAILURE)
	{
		service->type = SOAP_CLASS;
		service->soap_class.ce = ce;
	}
}
//this function will take a zval and apply all attributes of sldTypePtr
zval *sdl_convert_zval_to_zval(zval *data, sdlTypePtr type)
{
}
*/

sdlPtr get_sdl(char *uri)
{
	sdlPtr tmp, *hndl;
	TSRMLS_FETCH();

	tmp = NULL;
	hndl = NULL;
	if(zend_hash_find(SOAP_GLOBAL(sdls), uri, strlen(uri), (void **)&hndl) == FAILURE)
	{
		tmp = load_wsdl(uri, NULL);
		zend_hash_add(SOAP_GLOBAL(sdls), uri, strlen(uri), &tmp, sizeof(sdlPtr), NULL);
	}
	else
		tmp = *hndl;

	return tmp;
}

sdlBindingPtr get_binding_from_type(sdlPtr sdl, int type)
{
	sdlBindingPtr *binding;

	for(zend_hash_internal_pointer_reset(sdl->bindings);
		zend_hash_get_current_data(sdl->bindings, (void **) &binding) == SUCCESS;
		zend_hash_move_forward(sdl->bindings))
	{
		if((*binding)->bindingType == type)
			return *binding;
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

	zend_hash_find(sdl->bindings, key.c, key.len, (void **)&binding);

	smart_str_free(&key);
	return binding;
}

int load_php_sdl()
{
/*	xmlNodePtr rootElement;
	xmlNodePtr services;

	SOAP_SERVER_GLOBAL_VARS();
	SOAP_SERVER_GLOBAL(availableServices) = xmlParseFile(servicesFile);
	rootElement = SOAP_SERVER_GLOBAL(availableServices)->children;
	services = rootElement->children;

	do
	{
		if(IS_ELEMENT_TYPE(services,PHP_SOAPSERVER_SERVICE))
		{
			phpSoapServicePtr newService;
			xmlNodePtr attrib, trav;
			HashTable *fn = NULL;
			HashTable *include = NULL;
			HashTable *cl = NULL;

			//Init New Service
			newService = emalloc(sizeof(phpSoapService));
			newService->serviceNode = services;
			newService->started = FALSE;

			fn = newService->functions = emalloc(sizeof(HashTable));
			include = newService->include_files = emalloc(sizeof(HashTable));
			zend_hash_init(fn, 0, NULL, free_function, 0);
			zend_hash_init(include, 0, NULL, ZVAL_PTR_DTOR, 0);

			attrib = services->properties;
			trav = attrib;
			//Get Attributes of Service
			do
			{
				if(IS_ATTRIBUTE_TYPE(trav,PHP_SOAPSERVER_SERVICE_NAME))
				{
					char* name = ATTRIBUTE_VALUE(trav);

					//Assign Service Vals
					ALLOC_INIT_ZVAL(newService->serviceName);
					ZVAL_STRING(newService->serviceName,name,1);
				}

				if(IS_ATTRIBUTE_TYPE(trav,PHP_SOAPSERVER_SERVICE_STARTED))
				{
					char* started = ATTRIBUTE_VALUE(trav);

					//Assign Service Vals
					if(!stricmp(started,"true"))
						newService->started = TRUE;
				}
			}
			while(trav = trav->next);

			//Get ChildNodes of Service
			trav = services->children;
			do
			{
				//Include Files
				if(IS_ELEMENT_TYPE(trav,PHP_SOAPSERVER_SERVICE_INCLUDE_FILE))
				{
					xmlNodePtr trav1 = trav->properties;
					do
					{
						if(IS_ATTRIBUTE_TYPE(trav1,PHP_SOAPSERVER_SERVICE_INCLUDE_FILE_NAME))
						{
							char* name = ATTRIBUTE_VALUE(trav1);
							zval* z_name;
							ALLOC_INIT_ZVAL(z_name);
							ZVAL_STRING(z_name,name,1);
							zend_hash_next_index_insert(include,&z_name,sizeof(zval),NULL);
						}
					}
					while(trav1 = trav1->next);
				}

				//Functions
				if(IS_ELEMENT_TYPE(trav,PHP_SOAPSERVER_SERVICE_FUNCTION))
				{
					phpSoapServiceFunctionPtr function;
					xmlNodePtr trav1;
					HashTable *par = NULL;

					function = emalloc(sizeof(phpSoapServiceFunction));
					function->functionNode = trav;

					par = function->functionParams = emalloc(sizeof(HashTable));
					zend_hash_init(par, 0, NULL, free_param, 0);

					trav1 = trav->properties;

					do
					{
						if(IS_ATTRIBUTE_TYPE(trav1,PHP_SOAPSERVER_SERVICE_FUNCTION_NAME))
						{
							char* name = ATTRIBUTE_VALUE(trav1);
							ALLOC_INIT_ZVAL(function->functionName);
							ZVAL_STRING(function->functionName,name,1);
						}
					}
					while(trav1 = trav1->next);

					trav1 = trav->children;
					do
					{
						if(IS_ELEMENT_TYPE(trav1,PHP_SOAPSERVER_SERVICE_FUNCTION_PARAM))
						{
							phpSoapServiceParamPtr param;
							xmlNodePtr trav2;

							param = emalloc(sizeof(phpSoapServiceParam));
							param->paramNode = trav1;

							trav2 = trav1->properties;

							do
							{
								if(IS_ATTRIBUTE_TYPE(trav2,PHP_SOAPSERVER_SERVICE_FUNCTION_PARAM_NAME))
								{
									char* name = ATTRIBUTE_VALUE(trav2);
									ALLOC_INIT_ZVAL(param->paramName);
									ZVAL_STRING(param->paramName,name,1);
								}
								else if(IS_ATTRIBUTE_TYPE(trav2,PHP_SOAPSERVER_SERVICE_FUNCTION_PARAM_TYPE))
								{
									char* type = ATTRIBUTE_VALUE(trav2);
									ALLOC_INIT_ZVAL(param->paramType);
									ZVAL_STRING(param->paramType,type,1);
								}
								else if(IS_ATTRIBUTE_TYPE(trav2,PHP_SOAPSERVER_SERVICE_FUNCTION_PARAM_POSITION))
								{
									char* val = ATTRIBUTE_VALUE(trav2);
									ALLOC_INIT_ZVAL(param->paramName);
									ZVAL_LONG(param->paramName,atoi(val));
								}
							}
							while(trav2 = trav2->next);
							zend_hash_add(par,Z_STRVAL_P(param->paramName),Z_STRLEN_P(param->paramName),param,sizeof(phpSoapServiceParam),NULL);
						}
					}
					while(trav1 = trav1->next);
					zend_hash_add(fn,Z_STRVAL_P(function->functionName),Z_STRLEN_P(function->functionName),function,sizeof(phpSoapServiceFunction),NULL);
				}

				//Classes
				if(IS_ELEMENT_TYPE(trav,PHP_SOAPSERVER_SERVICE_CLASS))
				{
					xmlNodePtr att, func;
					att = trav->properties;

					if(fn == NULL)
					{
						fn = newService->functions = emalloc(sizeof(HashTable));
						zend_hash_init(fn, 0, NULL, ZVAL_PTR_DTOR, 0);
					}

				}

			}while(trav = trav->next);

			zend_hash_add(SOAP_SERVER_GLOBAL(services),Z_STRVAL_P(newService->serviceName),Z_STRLEN_P(newService->serviceName),newService,sizeof(phpSoapService),NULL);
		}
	}
	while(services = services->next);
*/
	return TRUE;
}

int write_php_sdl()
{
	return TRUE;
}

sdlPtr load_wsdl(char *struri, sdlPtr parent)
{
	xmlDocPtr wsdl;
	xmlNodePtr root, definitions, types, binding, schema, service, import;
	xmlNodePtr trav, trav2, trav3;
	xmlAttrPtr targetNamespace;
	sdlPtr tmpsdl;
	TSRMLS_FETCH();

	if(!parent)
	{
		tmpsdl = malloc(sizeof(sdl));
		memset(tmpsdl, 0, sizeof(sdl));
		tmpsdl->source = strdup(struri);
	}
	else
		tmpsdl = parent;

	wsdl = xmlParseFile(struri);
	xmlCleanupParser();

	if(!wsdl)
		php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: Could't load");

	tmpsdl->doc = wsdl;
	root = wsdl->children;
	definitions = get_node(root, "definitions");
	if(!definitions)
		php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: Could't find definitions");

	targetNamespace = get_attribute(definitions->properties, "targetNamespace");
	if(targetNamespace)
		tmpsdl->target_ns = strdup(targetNamespace->children->content);

	types = get_node(definitions->children, "types");
	if(types)
	{
		trav = types->children;
		FOREACHNODE(trav, "schema", schema)
		{
			load_schema(&tmpsdl, schema);
		}
		ENDFOREACH(trav);
	}
	
	trav = definitions->children;
	FOREACHNODE(trav, "import", import)
	{
		xmlAttrPtr tmp = get_attribute(import->properties, "location");
		if(tmp)
			load_wsdl(tmp->children->content, tmpsdl);
	}
	ENDFOREACH(trav);

	service = get_node(definitions->children, "service");
	if(service != NULL)
	{
		xmlAttrPtr name;
		xmlNodePtr trav, port;

		name = get_attribute(service->properties, "name");
		if(!name)
			php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: No name associated with service");

		trav = service->children;
		FOREACHNODE(trav, "port", port)
		{
			xmlAttrPtr type, name, bindingAttr, location;
			xmlNodePtr portType, operation;
			xmlNodePtr address;
			char *ns, *ctype;
			sdlBindingPtr tmpbinding;

			tmpbinding = malloc(sizeof(sdlBinding));
			memset(tmpbinding, 0, sizeof(sdlBinding));

			name = get_attribute(port->properties, "name");
			if(!name)
				php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: No name associated with port");

			bindingAttr = get_attribute(port->properties, "binding");
			if(bindingAttr == NULL)
				php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: No binding associated with port");

			//find address and figure out binding type
			address = get_node(port->children, "address");
			if(!address)
				php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: No address associated with port");

			location = get_attribute(address->properties, "location");
			if(!location)
				php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: No location associated with port");

			tmpbinding->location = strdup(location->children->content);

			if(address->ns && !strcmp(address->ns->href, WSDL_SOAP_NAMESPACE))
				tmpbinding->bindingType = BINDING_SOAP;
			else if(address->ns && !strcmp(address->ns->href, WSDL_HTTP_NAMESPACE))
				tmpbinding->bindingType = BINDING_HTTP;

			parse_namespace(bindingAttr->children->content, &ctype, &ns);
			binding = get_node_with_attribute(definitions->children, "binding", "name", ctype);
			if(ns) efree(ns); if(ctype) efree(ctype);

			if(!binding)
				php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: No binding");

			if(tmpbinding->bindingType == BINDING_SOAP)
			{
				sdlSoapBindingPtr soapBinding;
				xmlNodePtr soapBindingNode;
				xmlAttrPtr tmp;
				
				soapBinding = malloc(sizeof(sdlSoapBinding));
				memset(soapBinding, 0, sizeof(sdlSoapBinding));

				soapBindingNode = get_node_ex(binding->children, "binding", WSDL_SOAP_NAMESPACE);
				if(soapBindingNode)
				{
					tmp = get_attribute(soapBindingNode->properties, "style");
					if(tmp && !strcmp(tmp->children->content, "document"))
						soapBinding->style = SOAP_DOCUMENT;
					else
						soapBinding->style = SOAP_RPC;

					tmp = get_attribute(soapBindingNode->properties, "transport");
					if(tmp)
					{
						if(strcmp(tmp->children->content, WSDL_HTTP_TRANSPORT))
							php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: PHP-SOAP doesn't support transport '%s'", tmp->children->content);
							
						soapBinding->transport = strdup(tmp->children->content);
					}
					tmpbinding->bindingAttributes = (void *)soapBinding;
				}
			}

			name = get_attribute(binding->properties, "name");
			if(name == NULL)
				php_error(E_ERROR, "Error parsing wsdl (Missing \"name\" attribute for \"binding\")");

			tmpbinding->name = strdup(name->children->content);

			type = get_attribute(binding->properties, "type");
			if(type == NULL)
				php_error(E_ERROR, "Error parsing wsdl (Missing \"type\" attribute for \"binding\")");

			parse_namespace(type->children->content, &ctype, &ns);
			portType = get_node_with_attribute(definitions->children, "portType", "name", ctype);
			if(portType == NULL)
				php_error(E_ERROR, "Error parsing wsdl (Missing \"portType\" with name \"%s\")", name->children->content);
			if(ctype) efree(ctype);	if(ns) efree(ns);

			trav2 = binding->children;
			FOREACHNODE(trav2, "operation", operation)
			{
				sdlFunctionPtr function;
				xmlNodePtr input, output, fault, portTypeOperation, portTypeInput, msgInput, msgOutput;
				xmlAttrPtr op_name, paramOrder;

				op_name = get_attribute(operation->properties, "name");
				if(op_name == NULL)
					php_error(E_ERROR, "Error parsing wsdl (Missing \"name\" attribute for \"operation\")");

				portTypeOperation = get_node_with_attribute(portType->children, "operation", "name", op_name->children->content);
				if(portTypeOperation == NULL)
					php_error(E_ERROR, "Error parsing wsdl (Missing \"portType/operation\" with name \"%s\")", op_name->children->content);

				function = malloc(sizeof(sdlFunction));
				function->functionName = strdup(op_name->children->content);
				function->requestParameters = NULL;
				function->responseParameters = NULL;
				function->responseName = NULL;
				function->bindingAttributes = NULL;
				function->bindingType = tmpbinding->bindingType;

				if(tmpbinding->bindingType == BINDING_SOAP)
				{
					sdlSoapBindingFunctionPtr soapFunctionBinding;
					sdlSoapBindingPtr soapBinding;
					xmlNodePtr soapOperation;
					xmlAttrPtr tmp;

					soapFunctionBinding = malloc(sizeof(sdlSoapBindingFunction));
					memset(soapFunctionBinding, 0, sizeof(sdlSoapBindingFunction));
					soapBinding = (sdlSoapBindingPtr)tmpbinding->bindingAttributes;
					soapFunctionBinding->style = soapBinding->style;

					soapOperation = get_node_ex(operation->children, "operation", WSDL_SOAP_NAMESPACE);
					if(soapOperation)
					{
						tmp = get_attribute(soapOperation->properties, "soapAction");
						if(tmp)
							soapFunctionBinding->soapAction = strdup(tmp->children->content);

						tmp = get_attribute(soapOperation->properties, "style");
						if(tmp && !strcmp(tmp->children->content, "rpc"))
							soapFunctionBinding->style = SOAP_RPC;
						else if(!soapBinding->style)
							soapFunctionBinding->style = SOAP_DOCUMENT;
					}

					function->bindingAttributes = (void *)soapFunctionBinding;
				}

				input = get_node(operation->children, "input");
				if(input != NULL)
				{
					xmlAttrPtr message;
					xmlNodePtr part;
					char *ns, *ctype;

					portTypeInput = get_node(portTypeOperation->children, "input");
					message = get_attribute(portTypeInput->properties, "message");
					if(message == NULL)
						php_error(E_ERROR, "Error parsing wsdl (Missing name for \"input\" of \"%s\")", op_name->children->content);

					function->requestName = strdup(function->functionName);
					function->requestParameters = malloc(sizeof(HashTable));
					zend_hash_init(function->requestParameters, 0, NULL, delete_paramater, 1);

					parse_namespace(message->children->content, &ctype, &ns);
					msgInput = get_node_with_attribute(definitions->children, "message", "name", ctype);
					if(msgInput == NULL)
						php_error(E_ERROR, "Error parsing wsdl (Missing \"message\" with name \"%s\")", message->children->content);
					if(ctype) efree(ctype);	if(ns) efree(ns);

					if(tmpbinding->bindingType == BINDING_SOAP)
					{
						sdlSoapBindingFunctionPtr soapFunctionBinding = function->bindingAttributes;
						xmlNodePtr body;
						xmlAttrPtr tmp;

						body = get_node_ex(input->children, "body", WSDL_SOAP_NAMESPACE);
						if(body)
						{
							tmp = get_attribute(body->properties, "use");
							if(tmp && !strcmp(tmp->children->content, "literal"))
								soapFunctionBinding->input.use = SOAP_LITERAL;
							else
								soapFunctionBinding->input.use = SOAP_ENCODED;

							tmp = get_attribute(body->properties, "namespace");
							if(tmp)
								soapFunctionBinding->input.ns = strdup(tmp->children->content);

							tmp = get_attribute(body->properties, "parts");
							if(tmp)
								soapFunctionBinding->input.parts = strdup(tmp->children->content);

							tmp = get_attribute(body->properties, "encodingStyle");
							if(tmp)
								soapFunctionBinding->input.encodingStyle = strdup(tmp->children->content);
						}
					}

					trav3 = msgInput->children;
					FOREACHNODE(trav3, "part", part)
					{
						xmlAttrPtr element, type, name;
						sdlParamPtr param;

						param = malloc(sizeof(sdlParam));
						param->order = 0;

						name = get_attribute(part->properties, "name");
						if(name == NULL)
							php_error(E_ERROR, "Error parsing wsdl (No name associated with part \"%s\")", msgInput->name);

						param->paramName = strdup(name->children->content);

						element = get_attribute(part->properties, "element");
						if(element != NULL)
							param->encode = get_encoder_from_prefix(tmpsdl, part, element->children->content);

						type = get_attribute(part->properties, "type");
						if(type != NULL)
							param->encode = get_encoder_from_prefix(tmpsdl, part, type->children->content);

						zend_hash_next_index_insert(function->requestParameters, &param, sizeof(sdlParamPtr), NULL);
					}
					ENDFOREACH(trav3);
				}

				paramOrder = get_attribute(portTypeOperation->properties, "parameterOrder");
				if(paramOrder)
				{

				}

				output = get_node(portTypeOperation->children, "output");
				if(output != NULL)
				{
					xmlAttrPtr message;
					xmlNodePtr part;
					char *ns, *ctype;


					function->responseName = malloc(strlen(function->functionName) + strlen("Response") + 1);
					sprintf(function->responseName, "%sResponse\0", function->functionName);
					function->responseParameters = malloc(sizeof(HashTable));
					zend_hash_init(function->responseParameters, 0, NULL, delete_paramater, 1);

					message = get_attribute(output->properties, "message");
					if(message == NULL)
						php_error(E_ERROR, "Error parsing wsdl (Missing name for \"output\" of \"%s\")", op_name->children->content);

					parse_namespace(message->children->content, &ctype, &ns);
					msgOutput = get_node_with_attribute(definitions->children, "message", "name", ctype);
					if(msgOutput == NULL)
						php_error(E_ERROR, "Error parsing wsdl (Missing \"message\" with name \"%s\")", message->children->content);
					if(ctype) efree(ctype); if(ns) efree(ns);

					if(tmpbinding->bindingType == BINDING_SOAP)
					{
						sdlSoapBindingFunctionPtr soapFunctionBinding = function->bindingAttributes;
						xmlNodePtr body;
						xmlAttrPtr tmp;

						body = get_node_ex(output->children, "body", WSDL_SOAP_NAMESPACE);
						if(body)
						{
							tmp = get_attribute(body->properties, "use");
							if(tmp && !strcmp(tmp->children->content, "literal"))
								soapFunctionBinding->output.use = SOAP_LITERAL;
							else
								soapFunctionBinding->output.use = SOAP_ENCODED;

							tmp = get_attribute(body->properties, "namespace");
							if(tmp)
								soapFunctionBinding->output.ns = strdup(tmp->children->content);

							tmp = get_attribute(body->properties, "parts");
							if(tmp)
								soapFunctionBinding->output.parts = strdup(tmp->children->content);

							tmp = get_attribute(body->properties, "encodingStyle");
							if(tmp)
								soapFunctionBinding->output.encodingStyle = strdup(tmp->children->content);
						}
					}

					trav3 = msgOutput->children;
					FOREACHNODE(trav3, "part", part)
					{
						sdlParamPtr param;
						xmlAttrPtr element, type, name;

						param = malloc(sizeof(sdlParam));
						param->order = 0;

						name = get_attribute(part->properties, "name");
						if(name == NULL)
							php_error(E_ERROR, "Error parsing wsdl (No name associated with part \"%s\")", msgOutput->name);

						param->paramName = strdup(name->children->content);

						element = get_attribute(part->properties, "element");
						if(element)
							param->encode = get_encoder_from_prefix(tmpsdl, part, element->children->content);

						type = get_attribute(part->properties, "type");
						if(type)
							param->encode = get_encoder_from_prefix(tmpsdl, part, type->children->content);

						zend_hash_next_index_insert(function->responseParameters, &param, sizeof(sdlParamPtr), NULL);
					}
					ENDFOREACH(trav3);
				}

				fault = get_node(operation->children, "fault");
				if(!fault)
				{
				}

				if(!tmpbinding->functions)
				{
					tmpbinding->functions = malloc(sizeof(HashTable));
					zend_hash_init(tmpbinding->functions, 0, NULL, delete_function, 1);
				}

				zend_hash_add(tmpbinding->functions, php_strtolower(function->functionName, strlen(function->functionName)), strlen(function->functionName), &function, sizeof(sdlFunctionPtr), NULL);
			}
			ENDFOREACH(trav2);

			if(!tmpsdl->bindings)
			{
				tmpsdl->bindings = malloc(sizeof(HashTable));
				zend_hash_init(tmpsdl->bindings, 0, NULL, delete_binding, 1);
			}

			zend_hash_add(tmpsdl->bindings, tmpbinding->name, strlen(tmpbinding->name), &tmpbinding, sizeof(sdlBindingPtr), NULL);
		}
		ENDFOREACH(trav);

	}
	else
		php_error(E_ERROR, "Error parsing wsdl (\"Couldn't bind to service\")");

	return tmpsdl;
}

int write_wsdl()
{
	return TRUE;
}

int write_ms_sdl()
{
	return TRUE;
}

int load_ms_sdl(char *struri,int force_load)
{
/* Commenting this out. Does anyone need it?

	if(get_sdl(struri) == NULL || force_load)
	{
		SOAP_TLS_VARS();
		xmlDocPtr sdl = xmlParseFile(struri);
		xmlNodePtr schema,trav,trav2,req,res,paramOrd,reqRes,address,serviceAdd,service,soap,serviceDesc,root = sdl->children;
		xmlAttrPtr tmpattr,uri;
		char *add,*functionName,*soapAction,*request,*response,*parameterOrder,*value,*namespace;
		SDLPtr sdlPtr;
		SoapFunctionPtr tmpFunction;
		zval *tempZval;
		serviceDesc = get_node(root,"serviceDescription");
		soap = get_node(serviceDesc->children,"soap");
		trav = soap->children;
		sdlPtr = emalloc(sizeof(SDL));

		ALLOC_INIT_ZVAL(sdlPtr->sdlUri);
		ZVAL_STRING(sdlPtr->sdlUri,struri,1);

		FOREACHNODE(trav,"service",service)
		{
			sdlPtr->soapFunctions = emalloc(sizeof(HashTable));
			sdlPtr->addresses = emalloc(sizeof(HashTable));
		    zend_hash_init(sdlPtr->soapFunctions, 0, NULL, delete_function, 0);
		    zend_hash_init(sdlPtr->addresses, 0, NULL, ZVAL_PTR_DTOR, 0);

			serviceAdd = get_node(service->children,"addresses");
			trav2 = serviceAdd->children;
			ALLOC_INIT_ZVAL(tempZval);
			FOREACHNODE(trav2,"address",address)
			{
				uri = get_attribute(address->properties,"uri");
				add = uri->children->content;
				ZVAL_STRING(tempZval,add,1);
				zend_hash_next_index_insert(sdlPtr->addresses,tempZval,sizeof(zval),NULL);
			}
			ENDFOREACH(trav2);
			trav2 = service->children;
			FOREACHNODE(trav2,"requestResponse",reqRes)
			{
				tmpFunction = emalloc(sizeof(SoapFunction));

				tmpattr = get_attribute(reqRes->properties,"name");
				functionName = tmpattr->children->content;
				ALLOC_INIT_ZVAL(tmpFunction->functionName);
				ZVAL_STRING(tmpFunction->functionName,functionName,1);

				tmpattr = get_attribute(reqRes->properties,"soapAction");
				soapAction = tmpattr->children->content;
				ALLOC_INIT_ZVAL(tmpFunction->soapAction);
				ZVAL_STRING(tmpFunction->soapAction,soapAction,1);

				//Request
				req = get_node(reqRes->children,"request");
				tmpattr = get_attribute(req->properties,"ref");
				if(tmpattr != NULL)
				{
					request = tmpattr->children->content;
					parse_namespace(request,&value,&namespace);
					ALLOC_INIT_ZVAL(tmpFunction->requestName);
					ZVAL_STRING(tmpFunction->requestName,value,1);
					tmpFunction->requestParameters = emalloc(sizeof(HashTable));
					zend_hash_init(tmpFunction->requestParameters, 0, NULL, delete_paramater, 0);
					efree(value);
					efree(namespace);
				}

				//Response
				res = get_node(reqRes->children,"response");
				tmpattr = get_attribute(res->properties,"ref");
				if(tmpattr != NULL)
				{
					response = tmpattr->children->content;
					parse_namespace(response,&value,&namespace);
					ALLOC_INIT_ZVAL(tmpFunction->responseName);
					ZVAL_STRING(tmpFunction->responseName,value,1);
					tmpFunction->responseParameters = emalloc(sizeof(HashTable));
					zend_hash_init(tmpFunction->responseParameters, 0, NULL, delete_paramater, 0);
					efree(value);
					efree(namespace);
				}

				//Parameters
				paramOrd = get_node(reqRes->children,"parameterorder");
				if(paramOrd != NULL)
				{
					zval *space,*array,**strval;
					int count,i;
					ALLOC_INIT_ZVAL(space);
					ZVAL_STRING(space," ",0);
					parameterOrder = paramOrd->children->content;
					ZVAL_STRING(tempZval,parameterOrder,1);
					ALLOC_INIT_ZVAL(array);
					array_init(array);

					//Split on space
					php_explode(space, tempZval, array, -1);
					zend_hash_internal_pointer_reset(array->value.ht);
					count = zend_hash_num_elements(array->value.ht);

					for(i = 0;i < count;i++)
					{
						SoapParamPtr param;
						param = emalloc(sizeof(SoapParam));
						param->order = i+1;
						param->type = NULL;
						zend_hash_get_current_data(array->value.ht,(void **)&strval);
						ALLOC_INIT_ZVAL(param->paramName);
						ZVAL_STRING(param->paramName,Z_STRVAL_PP(strval),1);
						zend_hash_next_index_insert(tmpFunction->requestParameters,param,sizeof(SoapParam),NULL);
						zend_hash_move_forward(array->value.ht);
					}
				}
				zend_hash_add(sdlPtr->soapFunctions,(char *)php_strtolower(functionName,strlen(functionName)),strlen(functionName),tmpFunction,sizeof(SoapFunction),NULL);
			}
			ENDFOREACH(trav2);
		}
		ENDFOREACH(trav);

		trav = serviceDesc->children;
		FOREACHNODE(trav,"schema",schema)
		{
			load_schema(&sdlPtr, schema);
		}
		ENDFOREACH(trav);
		sdlPtr->have_sdl = 1;
		map_types_to_functions(sdlPtr);
		zend_hash_add(SOAP_GLOBAL(SDLs),struri,strlen(struri),sdlPtr,sizeof(SDL),NULL);
	}*/
	return FALSE;
}

void delete_type(void *data)
{
	sdlTypePtr type = *((sdlTypePtr*)data);
	if(type->name)
		free(type->name);
	if(type->namens)
		free(type->namens);
	if(type->elements)
	{
		zend_hash_destroy(type->elements);
		free(type->elements);
	}
	if(type->attributes)
	{
		zend_hash_destroy(type->attributes);
		free(type->attributes);
	}
	if(type->restrictions)
	{
		delete_restriction_var_int(&type->restrictions->minExclusive);
		delete_restriction_var_int(&type->restrictions->minInclusive);
		delete_restriction_var_int(&type->restrictions->maxExclusive);
		delete_restriction_var_int(&type->restrictions->maxInclusive);
		delete_restriction_var_int(&type->restrictions->totalDigits);
		delete_restriction_var_int(&type->restrictions->fractionDigits);
		delete_restriction_var_int(&type->restrictions->length);
		delete_restriction_var_int(&type->restrictions->minLength);
		delete_restriction_var_int(&type->restrictions->maxLength);
		delete_schema_restriction_var_char(&type->restrictions->whiteSpace);
		delete_schema_restriction_var_char(&type->restrictions->pattern);
		zend_hash_destroy(type->restrictions->enumeration);
		free(type->restrictions->enumeration);
		free(type->restrictions);
	}
	free(type);
}

void delete_attribute(void *attribute)
{
	sdlAttributePtr attr = *((sdlAttributePtr*)attribute);

	if(attr->def)
		free(attr->def);
	if(attr->fixed)
		free(attr->fixed);
	if(attr->form)
		free(attr->form);
	if(attr->id)
		free(attr->id);
	if(attr->name)
		free(attr->name);
	if(attr->ref)
		free(attr->ref);
	if(attr->type)
		free(attr->type);
	if(attr->use)
		free(attr->use);
	if(attr->extraAttributes)
	{
		zend_hash_destroy(attr->extraAttributes);
		free(attr->extraAttributes);
	}
}
