#define _WIN32_DCOM

#include "../handler.h"
#include "../php_rpc.h"

#include "com.h"
#include "com_wrapper.h"
#include "conversion.h"

#include <oleauto.h>


/* protos */
static int com_hash(rpc_string, rpc_string *, void *, int, char *, int);
static int com_name(rpc_string, rpc_string *, void *, int);
static int com_ctor(rpc_string, void **, int , zval ***);
static int com_dtor(void **);
static int com_call(rpc_string, void **, zval **, int, zval ***);
static int com_get(rpc_string, zval *, void **);
static int com_set(rpc_string, zval *, void **);
static int com_compare(void **, void **);
static int com_has_property(rpc_string, void **);
static int com_unset_property(rpc_string, void **);
static int com_get_properties(HashTable **, void **);

static PHP_INI_MH(OnTypelibFileChange);


/* globals */
static IBindCtx *pBindCtx;


/* register rpc callback function */
RPC_REGISTER_HANDLERS_START(com)
TRUE,							/* poolable */
HASH_AS_INT_WITH_SIGNATURE,
com_hash,
com_name,
com_ctor,
com_dtor,
com_call,
com_get,
com_set,
com_compare,
com_has_property,
com_unset_property,
com_get_properties
RPC_REGISTER_HANDLERS_END()

/* register ini settings */
RPC_INI_START(com)
PHP_INI_ENTRY_EX("com.allow_dcom", "0", PHP_INI_SYSTEM, NULL, php_ini_boolean_displayer_cb)
PHP_INI_ENTRY_EX("com.autoregister_typelib", "0", PHP_INI_SYSTEM, NULL, php_ini_boolean_displayer_cb)
PHP_INI_ENTRY_EX("com.autoregister_verbose", "0", PHP_INI_SYSTEM, NULL, php_ini_boolean_displayer_cb)
PHP_INI_ENTRY_EX("com.autoregister_casesensitive", "1", PHP_INI_SYSTEM, NULL, php_ini_boolean_displayer_cb)
PHP_INI_ENTRY("com.typelib_file", "", PHP_INI_SYSTEM, OnTypelibFileChange)
RPC_INI_END()

/* register userspace functions */
RPC_FUNCTION_ENTRY_START(com)
	ZEND_FALIAS(com_invoke, rpc_call, NULL)
	ZEND_FE(com_addref, NULL)
	ZEND_FE(com_release, NULL)
RPC_FUNCTION_ENTRY_END()

/* register class methods */
RPC_METHOD_ENTRY_START(com)
	ZEND_FALIAS(addref, com_addref, NULL)
	ZEND_FALIAS(release, com_release, NULL)
RPC_METHOD_ENTRY_END()


/* init function that is called before the class is registered
 * so you can do any tricky stuff in here
 */
RPC_INIT_FUNCTION(com)
{
	CreateBindCtx(0, &pBindCtx);
}

RPC_SHUTDOWN_FUNCTION(com)
{
	pBindCtx->lpVtbl->Release(pBindCtx);
}

/* rpc handler functions */

static int com_hash(rpc_string name, rpc_string *hash, void *data, int num_args, char *arg_types, int type)
{
	OLECHAR *olestr = php_char_to_OLECHAR(name.str, name.len, CP_ACP, FALSE);

	switch (type) {
		case CLASS:
		{
			CLSID *clsid = malloc(sizeof(CLSID));

			if (FAILED(CLSIDFromString(olestr, clsid))) {
				/* Perhaps this is a Moniker? */
				free(clsid);

				hash->str = strdup(name.str);
				hash->len = name.len;
			} else {
				hash->str = (char *) clsid;
				/* str is actually not a string but a CLSID struct, thus set len to 0.
				 * nevertheless clsid is freed by the rpc_string_dtor
				 */
				hash->len = 0;
			}

			efree(olestr);

			return SUCCESS;
		}

		case METHOD:
		case PROPERTY:
		{
			DISPID *dispid = malloc(sizeof(DISPID));
			
			if(SUCCEEDED(php_COM_get_ids_of_names((comval *) data, olestr, dispid))) {
				hash->str = (char *) dispid;
				/* str is actually not a string but a DISPID struct, thus set len to 0.
				 * nevertheless dispid is freed by the rpc_string_dtor
				 */
				hash->len = 0;

				efree(olestr);

				return SUCCESS;
			} else {
				free(dispid);
				efree(olestr);

				return FAILURE;
			}
		}
	}

	efree(olestr);

	return FAILURE;
}

static int com_name(rpc_string hash, rpc_string *name, void *data, int type)
{
	if (hash.len != 0) {
		/* not a GUID, perhaps a Moniker */
		name->str = strdup(hash.str);
		name->len = hash.len;

		return SUCCESS;
	} else {
		switch (type) {
			case CLASS:
			{
				OLECHAR *olestr;

				StringFromCLSID((CLSID *) hash.str, &olestr);
				name->str = php_OLECHAR_to_char(olestr, &(name->len), CP_ACP, TRUE);
				CoTaskMemFree(olestr);

				return SUCCESS;
			}

			case METHOD:
			case PROPERTY:
				/* not used yet */
				break;
		}
	}

	return FAILURE;
}

static int com_ctor(rpc_string class_name, void **data, int num_args, zval **args[])
{
	zval **server_name = NULL;
	zval **code_page = NULL;
	zval **typelib = NULL;
	zval **user_name=NULL;
	zval **password=NULL;
	zval **domain=NULL;
	int mode = 0;
	comval *obj;
	HRESULT hr;
	CLSCTX flags = CLSCTX_SERVER;

	switch (num_args) {
		case 3:
			typelib = args[2];
			convert_to_string_ex(typelib);
			/* break missing intentionally */
		case 2:
			code_page = args[1];
			convert_to_long_ex(code_page);
			/* break missing intentionally */
		case 1:
			server_name = args[0];
			/* break missing intentionally */
			break;

		case 0:
			/* nothing to do */
			break;

		default:
			/* exception */
			return FAILURE;
	}

	if (server_name != NULL) {
		/* What is server name? A String or an array? */
		if (Z_TYPE_PP(server_name) == IS_NULL) {
			server_name = NULL;
		} else if (Z_TYPE_PP(server_name) == IS_ARRAY) {
			zval **tmp;
			/* Aha - we have a number of possible arguments.
			 * They are in the hash By name: Server, Domain, Username, Password
			 * Flags.
			 * This has been crafted to maintian maximum backward compatability.
			 * If the server name is specified as a string, then the function
			 * should behave as before by defaulting username and password and
			 * using the (I believe) incorrect CLSCTX_SERVER instantiation
			 * paramter. However if server is specified in this array then we
			 * use either CLSCTX_REMOTE_SERVER or whatever flags are specified
			 * in the array
			 */
			HashTable *ht = Z_ARRVAL_PP(server_name);
			if (FAILURE == zend_hash_find(ht, "Server", 7, (void **) &tmp)) {
				server_name = NULL;
			} else {
				server_name = tmp;
				convert_to_string_ex(server_name);
				/* CLSCTX_SERVER includes INPROC and LOCAL SERVER. This means
				 * that any local server will be instantiated BEFORE even
				 * looking on a remote server. Thus if we have a server name,
				 * probably we want to access a remote machine or we would not
				 * have bothered specifying it. So it would be wrong to to
				 * connect locally. Futher, unless the name passed is a GUID,
				 * there has to be something to map the Prog.Id to GUID and
				 * unless that has been modified to remove the information
				 * about local instantiation CLSCTX_SERVER would force a local
				 * instantiation This setting can be overridden below if the
				 * user specifies a flags element */
				flags = CLSCTX_REMOTE_SERVER;
			}
			if (FAILURE == zend_hash_find(ht, "username", 9, (void **) &tmp)) {
				user_name = NULL;
			} else {
				user_name = tmp;
				convert_to_string_ex(user_name);
			}
			if (FAILURE == zend_hash_find(ht, "domain", 7, (void **) &tmp)) {
				domain = NULL;
			} else {
				domain = tmp;
				convert_to_string_ex(domain);
			}
			if (FAILURE == zend_hash_find(ht, "password", 9, (void **) &tmp)) {
				password=NULL;
			} else {
				password = tmp;
				convert_to_string_ex(password);
			}
			if (SUCCESS == zend_hash_find(ht, "flags", 6, (void **) &tmp)) {
				convert_to_long_ex(tmp);
				flags = (CLSCTX) Z_LVAL_PP(tmp);
			}
		}

		if (server_name != NULL) {
			if (!INI_INT("com.allow_dcom")) {
				rpc_error(E_WARNING, "DCOM is disabled");
				return FAILURE;
			} else {
				flags = CLSCTX_REMOTE_SERVER;
				convert_to_string_ex(server_name);
			}
		}
	}

	ALLOC_COM(obj);
	*data = obj;

	if (code_page != NULL) {
		C_CODEPAGE(obj) = Z_LVAL_PP(code_page);
	}

	if (class_name.len) {
		/* Perhaps this is a Moniker? */
		IMoniker *pMoniker;
		ULONG ulEaten;

		if (server_name) {
			hr = MK_E_SYNTAX;
		} else {
			OLECHAR *olestr = php_char_to_OLECHAR(class_name.str, class_name.len, obj->codepage, FALSE);

			if (SUCCEEDED(hr = MkParseDisplayNameEx(pBindCtx, olestr, &ulEaten, &pMoniker))) {
				hr = pMoniker->lpVtbl->BindToObject(pMoniker, pBindCtx, NULL, &IID_IDispatch, (LPVOID *) &C_DISPATCH(obj));
				pMoniker->lpVtbl->Release(pMoniker);
			}

			efree(olestr);
		}

		if (FAILED(hr)) {
			char *error_message;

			php_COM_destruct(obj);
			error_message = php_COM_error_message(hr);  
			rpc_error(E_WARNING,"Invalid ProgID, GUID string, or Moniker: %s", error_message);
			LocalFree(error_message);
			
			return FAILURE;
		}
	} else {
		/* obtain IDispatch */
		if (!server_name) {
			hr = CoCreateInstance((CLSID *) class_name.str, NULL, flags, &IID_IDispatch, (LPVOID *) &C_DISPATCH(obj));
		} else {
			COSERVERINFO server_info;
			MULTI_QI pResults;
			COAUTHIDENTITY authid;
			COAUTHINFO authinfo = {RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, &authid, EOAC_NONE};

			server_info.dwReserved1=0;
			server_info.dwReserved2=0;
			server_info.pwszName = php_char_to_OLECHAR(Z_STRVAL_PP(server_name), Z_STRLEN_PP(server_name), obj->codepage, FALSE);
			if (user_name) {
				/* Parse Username into domain\username */
				authid.User = (WCHAR *) Z_STRVAL_PP(user_name);
				authid.UserLength = Z_STRLEN_PP(user_name);
				if (password) {
					authid.Password = (USHORT *) Z_STRVAL_PP(password);
					authid.PasswordLength = Z_STRLEN_PP(password);
				} else {
					authid.Password = (USHORT *) "";
					authid.PasswordLength = 0;
				}
				if (domain) {
					authid.Domain = (USHORT *) Z_STRVAL_PP(domain);
					authid.DomainLength = Z_STRLEN_PP(domain);
				} else {
					authid.Domain = (USHORT *) "";
					authid.DomainLength = 0;
				}
				authid.Flags = SEC_WINNT_AUTH_IDENTITY_ANSI;

				server_info.pAuthInfo=&authinfo;
			} else {
				server_info.pAuthInfo=NULL;
			}

			pResults.pIID = &IID_IDispatch;
			pResults.pItf = NULL;
			pResults.hr = S_OK;
			hr=CoCreateInstanceEx((CLSID *) class_name.str, NULL, flags, &server_info, 1, &pResults);
			if (SUCCEEDED(hr)) {
				hr = pResults.hr;
				C_DISPATCH(obj) = (IDispatch *) pResults.pItf;
			}
			efree(server_info.pwszName);
		}

		if (FAILED(hr)) {
			char *error_message, *clsid;

			php_COM_destruct(obj);
			error_message = php_COM_error_message(hr);
			clsid = php_COM_string_from_CLSID((CLSID *)class_name.str);
			rpc_error(E_WARNING,"Unable to obtain IDispatch interface for CLSID %s: %s", clsid, error_message);
			LocalFree(error_message);
			efree(clsid);

			return FAILURE;
		}
	}

	php_COM_set(obj, &C_DISPATCH(obj), TRUE);

	if (INI_INT("com.autoregister_casesensitive")) {
		mode |= CONST_CS;
	}

	if (C_HASTLIB(obj)) {
		if (INI_INT("com.autoregister_typelib")) {
			ITypeLib *pTL;
			unsigned int idx;

			/* @todo check if typlib isn't already loaded */
			if (C_TYPEINFO_VT(obj)->GetContainingTypeLib(C_TYPEINFO(obj), &pTL, &idx) == S_OK) {
				php_COM_load_typelib(pTL, mode);
				pTL->lpVtbl->Release(pTL);
			}
		}
	} else {
		if (typelib != NULL) {
			ITypeLib *pTL;

			if ((pTL = php_COM_find_typelib(Z_STRVAL_PP(typelib), mode)) != NULL) {
				C_HASTLIB(obj) = SUCCEEDED(pTL->lpVtbl->GetTypeInfo(pTL, 0, &C_TYPEINFO(obj)));
				/* idx 0 should deliver the ITypeInfo for the IDispatch Interface */
				if (INI_INT("com.autoregister_typelib")) {
					php_COM_load_typelib(pTL, mode);
				}
				pTL->lpVtbl->Release(pTL);
			}
		}
	}

#if 0
	RETURN_RESOURCE(zend_list_insert(obj, IS_COM));
#endif

	return SUCCESS;
}

static int com_dtor(void **data)
{
	return SUCCESS;
}

static int com_call(rpc_string method_name, void **data, zval **return_value, int num_args, zval **args[])
{
	return SUCCESS;
}

static int com_get(rpc_string property_name, zval *return_value, void **data)
{
	return SUCCESS;
}

static int com_set(rpc_string property_name, zval *value, void **data)
{
	return SUCCESS;
}

static int com_compare(void **data1, void **data2)
{
	return SUCCESS;
}

static int com_has_property(rpc_string property_name, void **data)
{
	return SUCCESS;
}

static int com_unset_property(rpc_string property_name, void **data)
{
	return SUCCESS;
}

static int com_get_properties(HashTable **properties, void **data)
{
	return SUCCESS;
}

/* custom functions */
ZEND_FUNCTION(com_addref)
{
}

ZEND_FUNCTION(com_release)
{
}

/* ini callbacks */
static PHP_INI_MH(OnTypelibFileChange)
{
	FILE *typelib_file;
	char *typelib_name_buffer;
	char *strtok_buf = NULL;
	int interactive;
	interactive = CG(interactive);

	if (!new_value || (typelib_file = VCWD_FOPEN(new_value, "r"))==NULL) {
		return FAILURE;
	}

	if (interactive) {
		printf("Loading type libraries...");
		fflush(stdout);
	}

	typelib_name_buffer = (char *) emalloc(sizeof(char)*1024);

	while (fgets(typelib_name_buffer, 1024, typelib_file)) {
		ITypeLib *pTL;
		char *typelib_name;
		char *modifier, *ptr;
		int mode = CONST_CS | CONST_PERSISTENT;	/* CONST_PERSISTENT is ok here */

		if (typelib_name_buffer[0]==';') {
			continue;
		}
		typelib_name = php_strtok_r(typelib_name_buffer, "\r\n", &strtok_buf); /* get rid of newlines */
		if (typelib_name == NULL) {
			continue;
		}
		typelib_name = php_strtok_r(typelib_name, "#", &strtok_buf);
		modifier = php_strtok_r(NULL, "#", &strtok_buf);
		if (modifier != NULL) {
			if (!strcmp(modifier, "cis") || !strcmp(modifier, "case_insensitive")) {
				mode &= ~CONST_CS;
			}
		}

		/* Remove leading/training white spaces on search_string */
		while (isspace(*typelib_name)) {/* Ends on '\0' in worst case */
			typelib_name ++;
		}
		ptr = typelib_name + strlen(typelib_name) - 1;
		while ((ptr != typelib_name) && isspace(*ptr)) {
			*ptr = '\0';
			ptr--;
		}

		if (interactive) {
			printf("\rLoading %-60s\r", typelib_name);
		}

		if ((pTL = php_COM_find_typelib(typelib_name, mode)) != NULL) {
			php_COM_load_typelib(pTL, mode);
			pTL->lpVtbl->Release(pTL);
		}
	}

	efree(typelib_name_buffer);
	fclose(typelib_file);

	if (interactive) {
		printf("\r%70s\r", "");
	}

	return SUCCESS;
}
