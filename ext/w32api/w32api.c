/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: James Moore <jmoore@php.net>                                |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_W32API
#include <stdio.h>
#include <stdlib.h>
#define  WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"
#include "php_w32api.h"

/* {{{ w32api_functions[]
 */
function_entry w32api_functions[] = {
	PHP_FE(w32api_register_function,				NULL)
	PHP_FE(w32api_deftype,							NULL)
	PHP_FE(w32api_init_dtype,						NULL)
	PHP_FE(w32api_set_call_method,					NULL)
    {NULL, NULL, NULL}
};
/* }}} */

/* {{{ w32api_module_entry
 */
zend_module_entry w32api_module_entry = {
	STANDARD_MODULE_HEADER,
	"w32api",
	w32api_functions,
	PHP_MINIT(w32api),
	PHP_MSHUTDOWN(w32api),
	NULL,
	NULL,
	PHP_MINFO(w32api),
    "0.1", /* Replace with version number for your extension */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

ZEND_DECLARE_MODULE_GLOBALS(w32api)

#ifdef COMPILE_DL_W32API
ZEND_GET_MODULE(w32api)
#endif

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(w32api)
{
	php_info_print_table_start();
    php_info_print_table_row(2, "Win32 API Support", "enabled" );
    php_info_print_table_end();
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(w32api)
{
	ZEND_INIT_MODULE_GLOBALS(w32api, php_w32api_init_globals, NULL);
	register_constants(module_number);

	W32_G(le_dynaparm) = zend_register_list_destructors_ex(w32api_free_dynaparm, NULL, "dynaparm", module_number);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(w32api)
{
	if(W32_G(regfuncs))
	{
		FREE_HASHTABLE(W32_G(regfuncs));
	}

	if(W32_G(libraries))
	{
		php_w32api_unload_libraries();
		FREE_HASHTABLE(W32_G(libraries));
	}

	if(W32_G(libraries))
	{
		FREE_HASHTABLE(W32_G(types));
	}

	return SUCCESS;
}
/* }}} */

/* {{{ DYNAPARM dtor */
static void w32api_free_dynaparm(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	DYNAPARM *dparam;

	dparam = (DYNAPARM *)rsrc->ptr;

	if(dparam->pArg)
		efree(dparam->pArg);

	efree(dparam);
}
/* }}} */

/* {{{ */
static void php_w32api_unload_libraries()
{
	TSRMLS_FETCH();
	zend_hash_destroy(W32_G(libraries));
}
/* }}} */

/* {{{ */
static void php_w32api_dtor_library(void *data)
{
	FreeLibrary((HINSTANCE)data);
}
/* }}} */

/* {{{ */
static void php_w32api_init_globals(zend_w32api_globals *w32api_globals)
{
	TSRMLS_FETCH();

	w32api_globals->regfuncs = NULL;
	w32api_globals->libraries = NULL;
	w32api_globals->types = NULL;
	w32api_globals->call_type = DC_CALL_STD;

	ALLOC_HASHTABLE(W32_G(regfuncs));
	zend_hash_init(W32_G(regfuncs), 1, NULL, NULL, 1); 

	ALLOC_HASHTABLE(W32_G(libraries));
	zend_hash_init(W32_G(libraries), 1, NULL, php_w32api_dtor_library, 1);

	ALLOC_HASHTABLE(W32_G(types));
	zend_hash_init(W32_G(types), 5, NULL, NULL, 1);

}
/* }}} */

/* {{{ */
static void register_constants(int module_number)
{
	TSRMLS_FETCH();

	W32_REG_CONST(DC_MICROSOFT)
	W32_REG_CONST(DC_BORLAND)
	W32_REG_CONST(DC_CALL_CDECL)
	W32_REG_CONST(DC_CALL_STD)
	W32_REG_CONST(DC_RETVAL_MATH4)
	W32_REG_CONST(DC_RETVAL_MATH8)
	W32_REG_CONST(DC_CALL_STD_BO)
	W32_REG_CONST(DC_CALL_STD_MS)
	W32_REG_CONST(DC_CALL_STD_M8)
	W32_REG_CONST(DC_FLAG_ARGPTR)

}
/* }}} */

/* {{{ proto void w32api_set_call_method(int method)
	Sets the calling method used */
PHP_FUNCTION(w32api_set_call_method)
{
	zval **method;

	if(zend_get_parameters_ex(1, &method) == FAILURE)
	{
		WRONG_PARAM_COUNT
	}

	switch((*method)->value.lval)
	{
		case DC_CALL_CDECL:
			W32_G(call_type) = DC_CALL_CDECL;
			break;
		default:
			W32_G(call_type) = DC_CALL_STD;
			break;
	}

	RETURN_TRUE
	
}
/* }}} */

/* {{{ proto bool w32api_register_function(string libary, string function_name)
	Registers function function_name from library with PHP */
PHP_FUNCTION(w32api_register_function)
{
	HINSTANCE hinstLib;
	FARPROC ProcAdd;
	W32APIFE *fe;
	BOOL fRunTimeLinkSuccess = FALSE;
	zval **libname, **funcname, **retval_type;
	void *tmp;
	runtime_struct *rst;

	if(zend_get_parameters_ex(3, &libname, &funcname, &retval_type) == FAILURE)
	{
		WRONG_PARAM_COUNT
	}

	convert_to_string_ex(libname);
	convert_to_string_ex(funcname);
	convert_to_string_ex(retval_type);

	fe = (W32APIFE *)emalloc(sizeof(W32APIFE));
	fe->retvaltype = 0;
	//TODO: Check library isnt alreay loaded

	hinstLib = LoadLibrary((*libname)->value.str.val);

	if(hinstLib == NULL)
	{
		php_error(E_WARNING, "Could not load dynamic link library %s", (*libname)->value.str.val);
		RETURN_FALSE
	}

	zend_hash_add(W32_G(libraries),
				  (*libname)->value.str.val,
				  strlen((*libname)->value.str.val),
				  &hinstLib,
				  sizeof(HINSTANCE),
				  NULL);

	//TODO: Check function handle isnt already loaded

	ProcAdd = (FARPROC) GetProcAddress(hinstLib, (*funcname)->value.str.val);

	if(ProcAdd == NULL)
	{
		php_error(E_WARNING, "Could not get handle for function %s", (*funcname)->value.str.val);
		RETURN_FALSE
	}

	fe->fp = ProcAdd;

	if(!strcmp((*retval_type)->value.str.val, "long"))
	{
		fe->rettype = malloc(5*sizeof(char));
		fe->rettype  = strdup("long\0");
	}
	else if (!strcmp((*retval_type)->value.str.val, "int"))
	{
		fe->rettype = malloc(4*sizeof(char));
		fe->rettype = strdup("long\0");
	}
	else if (!strcmp((*retval_type)->value.str.val, "string"))
	{
		fe->rettype = malloc(7*sizeof(char));
		fe->rettype = strdup("string\0");
	}
	else if (!strcmp((*retval_type)->value.str.val, "byte"))
	{
		fe->rettype = malloc(5*sizeof(char));
		fe->rettype = strdup("byte\0");
	}
	else if (!strcmp((*retval_type)->value.str.val, "double"))
	{
		fe->rettype = malloc(7*sizeof(char));
		fe->rettype = strdup("double\0");
	}
	else if (!strcmp((*retval_type)->value.str.val, "bool"))
	{
		fe->rettype = malloc(5*sizeof(char));
		fe->rettype = strdup("bool\0");
	}
	else
	{
			/**
			 * this could be a userdef'd type so lets
			 * search the ht for that.
			 */
			if(zend_hash_find(W32_G(types), (*retval_type)->value.str.val, (*retval_type)->value.str.len, (void **) &tmp) == FAILURE)
			{
				php_error(E_WARNING, "Unknown type %s", (*retval_type)->value.str.val);
				RETURN_FALSE;
			}
			rst = tmp;
			fe->rettype = malloc(sizeof(char) * strlen(rst->name) + 1);
			memcpy(fe->rettype, rst->name, strlen(rst->name) + 1);
			fe->retvaltype = 1;
	}
	
	if(zend_hash_add(W32_G(regfuncs), php_strtolower((*funcname)->value.str.val, (*funcname)->value.str.len), (*funcname)->value.str.len, fe, sizeof(W32APIFE), NULL) == FAILURE)
	{
		php_error(E_WARNING, "Could not register function %s into hash");
		RETURN_FALSE;
	}	

	/**
	 * We now need to add the function into the global namespace, the best way to do this is
	 * to register it as a new module then it will definatly be removed on shutdown
	 */
	{
		zend_module_entry *temp_module_entry;
		char *fname;
		function_entry *tmp_functions;

		tmp_functions = malloc(sizeof(function_entry) * 2);
		fname = malloc((*funcname)->value.str.len + 1);
		memcpy(fname, (*funcname)->value.str.val, (*funcname)->value.str.len + 1);

		tmp_functions[0].fname = php_strtolower(fname, (*funcname)->value.str.len);
		tmp_functions[0].handler = zif_w32api_invoke_function;
		tmp_functions[0].func_arg_types = NULL;
		tmp_functions[1].fname = NULL;
		tmp_functions[1].handler = NULL; 
		tmp_functions[1].func_arg_types = NULL;


		temp_module_entry = malloc(sizeof(zend_module_entry));
		temp_module_entry->size = sizeof(zend_module_entry);
		temp_module_entry->zend_api = ZEND_MODULE_API_NO;
		temp_module_entry->zend_debug = ZEND_DEBUG;
		temp_module_entry->zts = USING_ZTS;
		temp_module_entry->name = fname;
		temp_module_entry->functions = tmp_functions;
		temp_module_entry->module_startup_func = temp_module_entry->module_shutdown_func =
		temp_module_entry->request_startup_func = temp_module_entry->request_shutdown_func = NULL;
		temp_module_entry->info_func = NULL;
		temp_module_entry->version = NULL;
		temp_module_entry->global_startup_func = temp_module_entry->global_shutdown_func = NULL;
		temp_module_entry->globals_id = 0;
		temp_module_entry->module_started = 0;
		temp_module_entry->type = 0;
		temp_module_entry->handle = NULL;
		temp_module_entry->module_number = 0;

		if(zend_register_module(temp_module_entry) != SUCCESS)
		{
			php_error(E_WARNING, "could not register function %s into the function table", (*funcname)->value.str.val);
			RETURN_FALSE
		}


	}



	RETURN_TRUE
};
/* }}} */

/* {{{ proto mixed w32api_invoke_function(string funcname, ....)
	Invokes function funcname with the arguments passed after the function name */
PHP_FUNCTION(w32api_invoke_function)
{
	zval ***args = (zval ***)NULL;
	void *tmp;
	W32APIFE *fe;
	char *funcname;
	int argc = ZEND_NUM_ARGS();
	runtime_struct *rst;
	RESULT retval;
	DYNAPARM *Param, *drval;
	LPVOID pVParam;
	int VParamsz;
	int i;

	args = emalloc(argc * sizeof(zval **));
	Param = (DYNAPARM *)emalloc((argc) * sizeof(DYNAPARM));

	if(zend_get_parameters_array_ex(argc, args) == FAILURE)
	{
		WRONG_PARAM_COUNT
	}

	funcname = get_active_function_name(TSRMLS_C);

	if(zend_hash_find(W32_G(regfuncs), funcname, strlen(funcname), (void **) &tmp) == FAILURE)
	{
		php_error(E_WARNING, "Could not find function handle for function %s", funcname);
		RETURN_FALSE;
	}

	fe = (W32APIFE *)tmp;

	// Build the DYNPARAM array.
	for(i = 0; i < (argc); i++)
	{
		Param[i] = w32api_convert_zval_to_dynparam(args[(i)] TSRMLS_CC);
	}


	/**
	 * We need to check the return type, if its a complex return type then we need to sort out pVParam and
	 * VParamsz and pass them as the last two parameters of the call to the invoke of the function.
	 */
	if(fe->retvaltype) // Complex return type
	{
		tmp = NULL;
		if(zend_hash_find(W32_G(types), fe->rettype, strlen(fe->rettype), (void **) &tmp) == FAILURE)
		{
			php_error(E_WARNING, "Unknown type %s", fe->rettype);
			RETURN_FALSE;
		}
		rst = tmp;
		VParamsz = rst->size;
		pVParam = malloc(rst->size);
	}
	else
	{
		pVParam = NULL;
		VParamsz = 0;
	}

	retval = php_w32api_dynamic_dll_call(W32_G(call_type), (ulong)(fe->fp), (argc), Param, pVParam, VParamsz);

	if(!strcmp(fe->rettype, "long"))
	{
		RETURN_LONG(retval.Long);
	}
	else if (!strcmp(fe->rettype, "int"))
	{
		RETURN_LONG(retval.Int);
	}
	else if (!strcmp(fe->rettype, "string"))
	{
		RETURN_STRING(retval.Pointer, 1);
	}
	else if (!strcmp(fe->rettype, "byte"))
	{
		php_error(E_WARNING, "byte return values are not supported right now");
		RETURN_FALSE;
	}
	else if (!strcmp(fe->rettype, "double"))
	{
		RETURN_DOUBLE(retval.Double);
	}
	else if (!strcmp(fe->rettype, "bool"))
	{
		if(retval.Int)
		{
			RETURN_TRUE;
		}
		else
		{
			RETURN_FALSE;
		}
	}
	else
	{
		/**
		 * This is returned in pRet, we need to get type and build a DYNAPARM for
		 * the return value and return the RESOURCE.
		 */
		
		drval = malloc(sizeof(DYNAPARM));
		drval->pArg = pVParam;
		drval->nWidth = VParamsz;
		drval->dwFlags = 0;
		ZEND_REGISTER_RESOURCE(return_value, drval, W32_G(le_dynaparm));
	}

}
/* }}} */

/* {{{ Dynamic calling of dll functions by pushing onto the stack manually. */
PHP_W32API_API RESULT php_w32api_dynamic_dll_call( int Flags, DWORD lpFunction, 
												   int nArgs, DYNAPARM Param[], 
												   LPVOID pRet, int nRetSize)
{
	/**
	 * Here we dont know the function we are calling or the arguments
	 * it expects so we must do quite a lot of work, normally done by
	 * the compiler ourselves, this is far easier to do it Assembly than
	 * in C.. here goes (jmoore - 05/11/2001).
	 *
	 * Based on the code by Ton Plooy <tonp@xs4all.nl>
	 * See Also MSFT KB Article ID: Q171729 for more background.
	 *
	 * We will support two calling mechanisms, __stdcall and __cdecl(WINAPIV).
	 */
	RESULT Res = { 0 };
	int i, nInd, nSize;										
	DWORD dwEAX, dwEDX, dwVal, *pStack, dwStSize = 0;
	BYTE *pArg;

	_asm {
		mov pStack, esp
		sub esp, 0x100
	}

    for (i = nArgs; i > 0; i--) {
        nInd  = i - 1;
        nSize = (Param[nInd].nWidth + 3) / 4 * 4;
        pArg  = (BYTE *)Param[nInd].pArg + nSize - 4;
        dwStSize += (DWORD)nSize;
        while (nSize > 0) {
            if (Param[nInd].dwFlags & DC_FLAG_ARGPTR) {
                dwVal = *(DWORD *)pArg;
                pArg -= 4;             
			}
            else {

                dwVal = Param[nInd].dwArg;
            }

            pStack--;           
            *pStack = dwVal;    
            nSize -= 4;
        }
    }

	if((pRet != NULL) && ((Flags & DC_BORLAND) || (nRetSize > 8))) 
	{
	    dwStSize += 4;
        pStack--;     
        *pStack = (DWORD)pRet; 
    }

	_asm {
		add esp, 0x100
		sub esp, dwStSize
		call [lpFunction]
		mov dwEAX, eax
		mov dwEDX, edx
	}

    if (Flags & DC_CALL_CDECL) {
        _asm add esp, dwStSize
    }

    if (Flags & DC_RETVAL_MATH4) {
        _asm fstp dword ptr [Res]
    }
    else if (Flags & DC_RETVAL_MATH8) {
        _asm fstp qword ptr [Res]
    }
    else if (pRet == NULL) {
        _asm{
			mov  eax, [dwEAX]
			mov  DWORD PTR [Res], eax
			mov  edx, [dwEDX]
			mov  DWORD PTR [Res + 4], edx
		}
    }
    else if (((Flags & DC_BORLAND) == 0) && (nRetSize <= 8)) {
        // Microsoft optimized less than 8-bytes structure passing
        _asm {
			mov ecx, DWORD PTR [pRet]
			mov eax, [dwEAX]
			mov DWORD PTR [ecx], eax
			mov edx, [dwEDX]
			mov DWORD PTR [ecx + 4], edx
		}
    }
    return Res;
}
/* }}} */

/* {{{ Conversion function for zvals to dynparams */
DYNAPARM w32api_convert_zval_to_dynparam(zval ** carg TSRMLS_DC)
{
	DYNAPARM dparam, *tparam;
	dparam.dwFlags = 0;

	switch((*carg)->type)
	{
		case IS_RESOURCE:
			tparam = (DYNAPARM *) zend_fetch_resource(carg TSRMLS_CC, -1, "dynaparm", NULL, 1, W32_G(le_dynaparm));
			if(!tparam)
			{
				php_error(E_ERROR, "Error when fetching argument");
			}
			dparam = *tparam;
			break;
		case IS_LONG:
			dparam.nWidth = sizeof(long);
			dparam.dwArg = (*carg)->value.lval;
			break;
		case IS_DOUBLE:
			dparam.nWidth = sizeof(float);
			dparam.pArg = &(*carg)->value.dval;
			dparam.dwFlags = DC_FLAG_ARGPTR;
			break;
		case IS_STRING:
			dparam.nWidth = sizeof(char *);
			dparam.pArg = (*carg)->value.str.val;
			break;
		case IS_BOOL:
			dparam.nWidth = sizeof(BOOL);
			dparam.dwArg = ((*carg)->value.lval == 0)?FALSE:TRUE;
			break;
		case IS_NULL:
			dparam.nWidth = sizeof(void *);
			dparam.pArg = NULL;
			break;
		default:
			php_error(E_ERROR, "Cant convert variable to type dynparam");
	}

	return dparam;
}
/* }}} */

/**
 * Typedef functions, We need to be flexible about what types we are going
 * to pass and retrive from functions in the win32 api. this means we need
 * to be able to create structs of any different type at runtime. We can do
 * this in asm. For example:
 *
 * typedef struct james {
 *		char firstname[81];
 *      char lastname[81];
 * }
 *
 * An instance of the above struct (lets call this instance iJames. iJames
 * is a pointer to the first letter of firstname (the base address), firstname
 * then fills the following 81 bytes (some of these may be empty), lastname is
 * at the offset iJames+81, 
 *
 * |-  81 Bytes  -|-  81 Bytes  -|
 * +------//------+------//------+
 * | James\0      | Moore\0      |
 * +------//------+------//------+
 * ^              ^
 * iJames         iJames[81]
 *
 * We could store a value in ax in this field by
 * the assembly command:
 * 
 * move ac iJames[81]
 *
 * Unions are easy in asm as the length of memory they use is equal to the size
 * of their largest member. For example:
 *
 * typedef union foo {
 *      int i;
 *      char j;
 * }
 *
 * The length of an int might be 4 bytes, the length of a char might be 1 byte. 
 * So if we create an instance of foo called bar then it would have the following 
 * layout in memory:
 *
 * +---+------------+
 * |   ¦            |
 * +---+------------+
 * ^^^^^
 * Memory area for char
 * ^^^^^^^^^^^^^^^^^^
 * Memory area for int
 *
 * Therefore even if there was only a char held in this section and the union was within 
 * a struct the next offset would still be base address + 4 not +1 so we need to deal
 * with this too.
 *
 * When defining types the user can call the w32api_deftype() function, this takes 2n+1 args where
 * n is the number of members the type has. The first argument is the name of the struct struct
 * after that is the type of the member followed by the members name (in pairs).
 *	
 *
 * James Moore <jmoore@php.net> 6/11/2001
 *
 */

/* {{{ proto int w32api_deftype(string typename, string member1_type, string member1_name, ...)
	Defines a type for use with other w32api_functions. */
PHP_FUNCTION(w32api_deftype)
{
	zval ***args;
	int argc = ZEND_NUM_ARGS();
	int i;
	runtime_struct *rst, *orst;
	void *tmp;
	field *fields, *ptr;

	args = (zval ***)emalloc(sizeof(zval **) * argc);
	rst = malloc(sizeof(runtime_struct));

	ptr = (field *)emalloc(sizeof(field) *((argc-1)/2));

	fields = ptr;

	if((zend_get_parameters_array_ex(argc, args) ==  FAILURE) || ((argc % 2) !=  1))
	{
		WRONG_PARAM_COUNT
	}

	for(i=2; i<argc; i++)
	{
		convert_to_string_ex(args[i]);
	}

	convert_to_string_ex(args[0]);

	rst->name = (*args[0])->value.str.val;
	rst->size = 0;

	/**
	 * We now take each parameter pair and fill out the field struct
	 * for each parameter pair.
	 */
	for(i=1; i<argc; i += 2)
	{
		ptr->type = malloc((*args[i])->value.str.len + 1);
		memcpy(ptr->type, (*args[i])->value.str.val, (*args[i])->value.str.len + 1);
		
		ptr->fname = malloc((*args[i+1])->value.str.len + 1);
		memcpy(ptr->fname, (*args[i+1])->value.str.val, (*args[i+1])->value.str.len + 1);

		ptr->fsize = 0;

		if(!strcmp(ptr->type, "long"))
		{
			ptr->fsize = sizeof(long);
		}
		else if (!strcmp(ptr->type, "int"))
		{
				ptr->fsize = sizeof(int);
		}
		else if (!strcmp(ptr->type, "string"))
		{
				ptr->fsize = sizeof(char *);
		}
		else if (!strcmp(ptr->type, "byte"))
		{
				ptr->fsize = 1;
		}
		else if (!strcmp(ptr->type, "double"))
		{
				ptr->fsize = sizeof(double);
		}
		else if (!strcmp(ptr->type, "bool"))
		{
				ptr->fsize = sizeof(BOOL);
		}
		else
		{
				/**
				 * this could be a userdef'd type so lets
				 * search the ht for that.
				 */
				if(zend_hash_find(W32_G(types), ptr->type, strlen(ptr->type), (void **) &tmp) == FAILURE)
				{
					php_error(E_WARNING, "Unknown type %s", ptr->type);
					RETURN_FALSE;
				}
				orst = tmp;
				ptr->fsize = orst->size;
		}

		rst->size += ptr->fsize;
		ptr++;
	}

	rst->fields = fields;

	if(zend_hash_add(W32_G(types), rst->name, strlen(rst->name), rst, sizeof(runtime_struct), NULL) == FAILURE)
	{
		php_error(E_WARNING, "Error registering type %s", rst->name);
		RETURN_FALSE;
	}

	RETURN_TRUE;

}
/* }}} */

/* {{{ proto resource w32api_init_dtype(string typename, mixed val1, mixed val2);
	Creates an instance to the data type typename and fills it with the values val1, val2, the function
	then returns a DYNAPARM which can be passed when invoking a function as a parameter.*/
PHP_FUNCTION(w32api_init_dtype)
{
	DYNAPARM *dparam, *tparam;
	void *rtstruct, *tmp;
	runtime_struct *rst;
	field *ptr;
	char *m;
	zval ***args;
	zval **curarg;
	int i, j,argc = ZEND_NUM_ARGS();

	args = emalloc(sizeof(zval **) * argc);
	dparam = emalloc(sizeof(DYNAPARM));

	if(zend_get_parameters_array_ex(argc, args) != SUCCESS)
	{
		WRONG_PARAM_COUNT
	}

	convert_to_string_ex(args[0]);

	if(zend_hash_find(W32_G(types), (*args[0])->value.str.val, (*args[0])->value.str.len, (void **)&tmp) == FAILURE)
	{
		php_error(E_WARNING, "Unknown type %s",(*args[0])->value.str.val);
		RETURN_FALSE
	}

	rst = (runtime_struct *)tmp;

	rtstruct = emalloc(rst->size);
	rtstruct = memset(rtstruct, 0, rst->size);
	tmp = rtstruct;
	curarg = args[1];
	ptr = rst->fields;

	i = 0;
	j = (argc-1);

	while(i<j)
	{
		if(!strcmp(ptr->type, "long"))
		{
			convert_to_long_ex(curarg);
			memcpy(tmp, &(*curarg)->value.lval, ptr->fsize);
		}
		else if (!strcmp(ptr->type, "int"))
		{
			convert_to_long_ex(curarg);
			memcpy(tmp, &(*curarg)->value.lval, ptr->fsize);
		}
		else if (!strcmp(ptr->type, "string"))
		{
			convert_to_string_ex(curarg);
			m = emalloc(sizeof(char) * (*curarg)->value.str.len + 1);
			memcpy(m, (*curarg)->value.str.val, (*curarg)->value.str.len + 1);
			memcpy(tmp, &m, ptr->fsize);
		}
		else if (!strcmp(ptr->type, "byte"))
		{
			/* use Lower order bytes */
			convert_to_long_ex(curarg);
			memcpy(tmp, &(*curarg)->value.lval, ptr->fsize);
		}
		else if (!strcmp(ptr->type, "double"))
		{
			convert_to_double_ex(curarg);
			memcpy(tmp, &(*curarg)->value.dval, ptr->fsize);
		}
		else if (!strcmp(ptr->type, "bool"))
		{
			convert_to_boolean_ex(curarg);
			memcpy(tmp, &(*curarg)->value.lval, ptr->fsize);
		}
		else
		{
			/**
			 * OK we have a user type here, we need to treat the param
			 * as a resource and fetch the DYNAPARM its contained in
			 * then copy the contents of its LPVOID pointer into our
			 * memory space.
			 */
			ZEND_FETCH_RESOURCE(tparam, DYNAPARM *, curarg, -1, "dynaparm", W32_G(le_dynaparm));
			memcpy(tmp, tparam->pArg, ptr->fsize);
		}

		/**
		 * We need somthing that is 1 byte 
		 */
		(char)tmp += ptr->fsize;
		(void *)tmp;

		curarg++;
		ptr++;
		i++;
	}

	dparam->dwFlags = 0;
	dparam->nWidth = rst->size;
	dparam->pArg = rtstruct;
	
	ZEND_REGISTER_RESOURCE(return_value, dparam, W32_G(le_dynaparm));

}
/* }}} */


#endif /* HAVE_W32API */