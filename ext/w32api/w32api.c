/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
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

/*
 * Win32 API Extension for PHP 4
 * =============================
 *
 * This extension allows PHP Developers to access the underlying functions in the
 * Win32 API Dll's in a generic way, it provides a mechanism for the user to load
 * and register functions from dll's and then to call them, it also implements a 
 * generic callback handler which allows the user to pass a pointer to one of
 * their PHP functions to act as a callback from the C function enabling the PHP
 * programmer to handle callbacks in PHP seemlessly. Finally the extension marshalls
 * from PHP types (Zvals) to C structures seemlessly once it has been told about them
 *
 * I would like to thank Ton Plooy for his article on a similar subject which gave me
 * the initial idea for a generic dll caller and some good pointers on how to actaully
 * implement it.
 */

/*
 * Interface the PHP Programmer Sees
 * ==================================
 *
 * To keep namespaces tidy the module will introduce two classes, the first is
 * the win32 class which has several functions to load and register functions from
 * the underlying DLL's. The Win32 class also has functions for informing the
 * module about the C types we will be using so it can marshall and demarshall to and from 
 * the C Interface. . Our second class is the Type class. The type class is our container
 * for complex C types which are registered and initiated using our win32 class functions.
 *
 * Win32 Class Functions
 * =====================
 *
 * static int Win32::RegisterFunction( string Definition, long flags )
 * -------------------------------------------------------------------
 *
 * This function is used for registering a dll function with the module. The Definition should take
 * the form:
 * Definition: <return_type> <function_name> [Alias <alias_name] (<argument_list>) From <some_dll>
 *
 *      return_type :-          Either a simple type or a type that has 
 *                              been registered with the module
 *
 *      function_name :-        The name of the function within the dll, if this is not found
 *                              we append an A to the function name to check for a ASCII version,
 *                              if this is not found we fail.
 *
 *      alias_name    :-        If this is provided then we register the function under this name
 *                              in the PHP Symbol table.
 *
 *      argument_list :-        A comma seperated list of arguments in the form <argument_type>
 *                              [&]argument_name.
 *
 *          argument_type :-        Argument type is a type known to the module
 *
 *          argument_name :-        This is not currently used although if it is proceeded by an
 *                                  & (Ampersand) then a pointer is passed rather than the value.
 *
 *      some_dll :-             This is the name of the dll to find the function in.
 *
 * On success the function returns TRUE, on error FALSE, The function issues appropriate errors
 * to allow the PHP Progammer to catch individual errors.
 *
 * static int Win32::UnregisterFunction(string FunctionName)
 * ---------------------------------------------------------
 *
 * Allows the PHP programmer to force a funciton to be unregistered saving on memory resources. Can
 * be useful in long running scripts.
 *
 * Returns TRUE on success, FALSE on error.
 *
 *
 * static int Win32::DefineType( string Definition )
 * -------------------------------------------------
 *
 * This function is used to register a C-Type which will be used when calling a function, it only 
 * supports the equivilent of C structures at this time so if you need to use a union you must use the 
 * largest member of that union as the type in the struct for this to work.
 *
 * The definition string takes the form:
 *
 * Definition: <type_name> { <type_list> }
 *
 *      type_name :-            A unique name for this type.
 *
 *      type_list :-            Takes for form <member_type> [&]<member_name>
 *
 *          member_type :-          The type of this member.
 *
 *          member_name :-          The name for this member, if an & (ampersand) preceeds
 *                                  the name it will be stripped and a pointer rather than
 *                                  the value will be used.
 *
 * Returns TRUE on success, FALSE on error.
 *
 *
 * static int Win32::GetTypeSize(mixed TypeHandle)
 * -----------------------------------------------
 *
 * This function returns the size of a type registered with the module. The parameter should
 * either be the name of the type or an instance of it. The Function returns FALSE on error 
 * ***USE === to distinguish between this and a size of 0*** or the size of the type on success.
 *
 *
 * static mixed Win32::InitType(String TypeName)
 * ---------------------------------------------
 *
 * Creates an instance of the type so that the PHP Programmer can assign values and then pass
 * it to a C Function. Returns NULL on error.
 *
 * static int Win32::DecRef(mixed var)
 * -----------------------------------
 * Decreases the reference count on a variable only if we incremented the refcount when passing
 * the variable to a C function.
 *
 * TYPE Functions
 * ==============
 *
 * mixed Type::Type(String TypeName)
 * ---------------------------------
 * See Win32::InitType,
 *
 * mixed Type::Clone()
 * -------------------
 *
 * Allows you to make an exact copy of the class, this should be used rather than the &= syntax
 * Due to the nesting within classes. This function WILL become redundant in PHP 5.
 */

/*
 * Implementation Details
 * ======================
 *
 * This module will only work on the Intel platform.
 *
 * We basically want to set up this structure:
 *
 *          +-----------+
 *          |    PHP    |
 *          +-----------+
 *        Call |    /|\ 
 *            \|/    | call_user_function_ex
 *   +------------------------+
 *   |  Win 32 API Extension  |
 *   +------------------------+
 *             |    /|\       
 *            \|/    | Callback
 *         +-------------+
 *         |   C-Space   |
 *         +-------------+
 *
 * Win32 Needs to then Marshall between zvals and 
 *
 * Marshalling from ZVAL's to C Types.
 * -----------------------------------
 * For simple types this is very easy as we either just copy the value or a pointer
 * to it onto the stack, if we copy a pointer then we must increase the refcount on
 * the zval and must also make sure we get it passed to us by reference.
 *
 * The problem here is when to dereference the zval again as we may get into the following
 * situation
 *
 * We call somefunction giving it an argument by reference (IE we pass a pointer to the value union of a zval)
 * we must increase the ref count on the zval to avoid the possibility of a GPE (IE the zval is dtord and then
 * the function uses the zval in some sort of callback we could end up with a GPE)
 * But if we increase the zval's refcount without dtoring it anywhere it would cause a mem leak.
 *
 * Therefore we probably need to keep a local reference table so we can either allow the user to call 
 * Win32::DecRef($var) to decrement the reference count (We would want to keep a local table to avoid anyone
 * breaking Zend's handling off it as well..))
 *
 * Then at MSHUTDOWN we free this hashtable decrementing the refcount as needed..
 *
 * Complex types are less clear cut on how to handle them. My prefered method is to always
 * keep these in a C Format but to allow access to these via a wrapper object which calculates
 * the offset of the data to allow access to it from PHP. This also allows us to do no conversion
 * when dealing with C types coming to us allowing us to deal with pointers in a more clear way.
 *
 *
 * Enabling C Code to call PHP Code in a generic fashion
 * -----------------------------------------------------
 * What we do here is we use _declspec(naked) to tell the compiler we will handle all stack operations
 * ourself, we also then create (At runtime) function prologues which we place on the heap which push
 * extra arguments onto the stack which tell us which php_function is being called back and the callback type
 * which has been registered with us. 
 *
 */

#if	HAVE_W32API

#include <stdio.h>
#include <stdlib.h>

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"
#include "php_w32api.h"


/* =====================================================================================================
 * PHP Module Startup, Shutdown & Info Code
 * =====================================================================================================
 */

#ifdef COMPILE_DL_W32API
ZEND_GET_MODULE(w32api)
#endif

/* {{{ w32api_module_entry
 */
zend_module_entry w32api_module_entry = {
	STANDARD_MODULE_HEADER,
	"Win32 API",
	NULL,										/* We define no global functions */
	PHP_MINIT(w32api),
	PHP_MSHUTDOWN(w32api),
	PHP_RINIT(w32api),
	PHP_RSHUTDOWN(w32api),
	PHP_MINFO(w32api),
	"0.2",
	STANDARD_MODULE_PROPERTIES
};
/* }}} */


/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(w32api)
{
	/* Setup out module globals */
	ZEND_INIT_MODULE_GLOBALS(w32api, php_w32api_init_globals, NULL);


	if(win32_class_init(TSRMLS_C) != SUCCESS)
	{
		return FAILURE;
	}

	if(type_class_init(TSRMLS_C) != SUCCESS)
	{
		return FAILURE;
	}

	WG(le_type_instance) = zend_register_list_destructors_ex(w32api_type_instance_dtor, NULL, "Type Instance", module_number);

	/* Function Flags */
	REGISTER_LONG_CONSTANT( "W32API_ARGPTR",	W32API_ARGPTR,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT( "W32API_BORLAND",	W32API_BORLAND,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT( "W32API_CDECL",		W32API_CDECL,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT( "W32API_REAL4",		W32API_REAL4,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT( "W32API_REAL8",		W32API_REAL8,	CONST_CS | CONST_PERSISTENT);

	
	return SUCCESS;

};
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(w32api)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(w32api)
{
	/* Allocate Request Specific HT's here
	 */
	ALLOC_HASHTABLE(WG(funcs));
	zend_hash_init(WG(funcs), 1, NULL, php_w32api_hash_func_dtor, 1);

	ALLOC_HASHTABLE(WG(libraries));
	zend_hash_init(WG(libraries), 1, NULL, php_w32api_hash_lib_dtor, 1);

	ALLOC_HASHTABLE(WG(callbacks));
	zend_hash_init(WG(callbacks), 1, NULL, php_w32api_hash_callback_dtor, 1);

	ALLOC_HASHTABLE(WG(types));
	zend_hash_init(WG(types), 1, NULL, php_w32api_hash_type_dtor, 1);


	return SUCCESS;

};
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(w32api)
{

	win32_class_rshutdown(TSRMLS_C);

	/* Must be dtor'd before libraries */
	zend_hash_destroy(WG(funcs));
	FREE_HASHTABLE(WG(funcs));

	zend_hash_destroy(WG(libraries));
	FREE_HASHTABLE(WG(libraries)); /* we may only want to do this on MSHUTDOWN but for now it can be here */

	zend_hash_destroy(WG(callbacks));
	FREE_HASHTABLE(WG(callbacks));

	zend_hash_destroy(WG(types));
	FREE_HASHTABLE(WG(types));


	return SUCCESS;
}
/* }}} */


/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(w32api)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Win32 API Support", "enabled" );
	php_info_print_table_end();
}
/* }}} */

/* {{{ php_w32api_init_globals
 */
static void php_w32api_init_globals(zend_w32api_globals *w32api_globals)
{
	w32api_globals->win32_ce = NULL;
	w32api_globals->type_ce = NULL;
	w32api_globals->funcs = NULL;
	w32api_globals->libraries = NULL;
	w32api_globals->callbacks = NULL;
	w32api_globals->types = NULL;
}
/* }}} */

/* {{{ php_w32api_hash_lib_dtor (void *data)
 * Dtor function called when hash is destroied, unloads library
 */
static void php_w32api_hash_lib_dtor(void *data)
{
	w32api_lib_handle	*lh;							/* Library Handle */
	TSRMLS_FETCH();										/* Get thread safe stuff */
	lh = (w32api_lib_handle *)data;

	FreeLibrary(lh->handle);
	efree(lh->library_name);
}
/* }}} */

/* {{{ php_w32api_hash_func_dtor (void *data)
 * Dtor function called when hash is destroied, unloads function.
 */
static void php_w32api_hash_func_dtor(void *data)
{
	w32api_func_handle	**fh;							/* Function Handle */
	TSRMLS_FETCH();										/* Get thread safe stuff */

	fh = (w32api_func_handle **)data;
	php_w32api_unload_function(fh TSRMLS_CC);
}
/* }}} */

/* {{{ php_w32api_hash_callback_dtor
 * DTOR function called when hash is destroied, removes callback.
 */
static void php_w32api_hash_callback_dtor(void *data)
{
	w32api_func_handle **fh;
	
	fh = (w32api_func_handle **)data;

	php_w32api_free_arguments((*fh)->argument_list);
	efree((*fh)->function_name);
	
	if((*fh)->return_type_name)
		efree((*fh)->return_type_name);

	efree(*fh);
}

/* {{{ php_w32api_hash_type_dtor
 * DTOR function called when hash is destroied, removes callback.
 */
static void php_w32api_hash_type_dtor(void *data)
{
	w32api_type_handle **th;
	
	th = (w32api_type_handle **)data;

	php_w32api_free_members((*th)->member_list);
	efree((*th)->type_name);
	efree(*th);
}

static void w32api_type_instance_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	w32api_type_instance *ti;
	int i = 0;

	if(!rsrc || !rsrc->ptr)
		return;

	ti = (w32api_type_instance *)rsrc->ptr;

	for(i = 0; i < ti->type->member_count; i++)
	{
		if(ti->values[i])
			zval_ptr_dtor(&ti->values[i]);
	}

	efree(ti);

	return;
}
/* =====================================================================================================
 * Utility Functions
 * =====================================================================================================
 */

/* {{{ php_w32api_unload_library
 * Expects two arguments, the first is the pointer to a w32api_lib_handle
 * and the second is a flag, if the flag is 0 then the reference counter is
 * use if it is one then the library is unloaded irrespective of the reference
 * counter
 */
static void php_w32api_unload_library (w32api_lib_handle *lh, int flags TSRMLS_DC)
{

	if(flags == 0)
	{
		lh->ref_count--;
	}

	if((flags == 1) || (lh->ref_count == 0))
	{
		/* remove outselves from the hashtable */
		zend_hash_del(WG(libraries), lh->library_name, strlen(lh->library_name) + 1);
	}
}
/* }}} */

/* {{{ php_w32api_unload_function
 * Expects one argument, a pointer to a w32api_func_handle, unloads this
 * function from both the function table internally and the PHP function 
 * table then it decrements the reference counter on the library.
 */
static void php_w32api_unload_function (w32api_func_handle **fh TSRMLS_DC)
{
	zend_function *function = NULL;

	efree((*fh)->return_type_name);
	php_w32api_free_arguments((*fh)->argument_list);

	/* If needs be we need to retrieve function ptr from hash table 
	 * and free anything we allocate when creating them at runtime (most notably 
	 * arg_types
	 */
	if(zend_hash_find(  &WG(win32_ce)->function_table,
						(*fh)->function_name,
						strlen((*fh)->function_name) + 1, 
						(void **)&function) == SUCCESS)
	{
		zend_internal_function *internal_function = (zend_internal_function *)function;
		if(internal_function->arg_types)
			efree(internal_function->arg_types);
	}

	/* Remove from Function Table */
	zend_hash_del(&WG(win32_ce)->function_table, (*fh)->function_name, strlen((*fh)->function_name) + 1);
	php_w32api_unload_library((*fh)->lib, 0 TSRMLS_CC);

	efree((*fh)->function_name);
	efree(*fh);

}
/* }}} */

/* {{{ php_w32api_load_function
 * Expects three arguments, The definition of the function in string format, the definitions length
 * and a pointer to a pointer to a function handle. returns SUCCESS or FAILURE.
 */
static int php_w32api_load_function (char *definition, int definition_len, int flags TSRMLS_DC)
{
	zend_function function;
	zend_internal_function *internal_function = (zend_internal_function *)&function;
	w32api_func_handle **fh;
	w32api_func_handle_ptr hnd;

	fh = emalloc(sizeof(w32api_func_handle *));
	*fh = NULL;

	/* Parse function */
	w32api_function_definition_scan_bytes(definition, definition_len);
	if((w32api_function_definition_parse((void *)&hnd) != 0))
	{
		*fh = hnd.hnd;
		if(*fh != NULL)
			efree(*fh);

		efree(fh);
		return FAILURE;
	}
	*fh = hnd.hnd;

	if(!*fh)
		return FAILURE;


	if(zend_hash_exists(&WG(win32_ce)->function_table, (*fh)->function_name, strlen((*fh)->function_name) + 1))
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING, 
				   "A function by the name %s already has been registered, cannot redefine function", 
				   (*fh)->function_name);

		/* We dont want to unload function as it already exists so lets just free it ourselves */
		php_w32api_unload_library((*fh)->lib, 0 TSRMLS_CC);
		php_w32api_free_arguments((*fh)->argument_list);
		efree((*fh)->return_type_name);
		efree((*fh)->function_name);
		efree(*fh);
		efree(fh);

		return FAILURE;
	}

	/* Insert it into our hash table */
	if(zend_hash_add( WG(funcs), 
					  (*fh)->function_name, 
					  strlen((*fh)->function_name) + 1, 
					  fh, 
					  sizeof(w32api_func_handle),
					  NULL) != SUCCESS)
	{

		php_error_docref(NULL TSRMLS_CC, E_WARNING, 
				   "Loading of function %s failed: Could not insert function handle into hash", 
				   (*fh)->function_name);

		/* Tidy up */
		zend_hash_del(WG(funcs), (*fh)->function_name, strlen((*fh)->function_name) +1);
		return FAILURE;
	}
	
	/* Insert function into win32_ce's function_table */
	internal_function->type = ZEND_INTERNAL_FUNCTION;
	internal_function->handler = W32API_CLASS_FN(win32, invokefunction);
	internal_function->function_name = (*fh)->function_name;
	internal_function->arg_types = php_w32api_do_arg_types(&(*fh)->argument_list);

	if(zend_hash_add(&WG(win32_ce)->function_table, (*fh)->function_name,
					 strlen((*fh)->function_name) + 1, &function, sizeof(zend_function), NULL) == FAILURE)
	{
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Could not register funciton %s into function table", (*fh)->function_name);
		zend_hash_del(WG(funcs), (*fh)->function_name, strlen((*fh)->function_name) +1);

		return FAILURE;;
	}
	
	if(flags)
	{
		(*fh)->flags = (*fh)->flags | flags;
	}


	return SUCCESS;
}
/* }}} */

/* {{{ php_w32api_unload_type
 * Expects one argument, a pointer to a w32api_type_handle, unloads this
 * type.
 */
static void php_w32api_unload_type (w32api_type_handle **th TSRMLS_DC)
{
	php_w32api_free_members((*th)->member_list);

	zend_hash_del(WG(types), (*th)->type_name, strlen((*th)->type_name) + 1);

}
/* }}} */

/* {{{ php_w32api_register_type
 */
static int php_w32api_register_type(char *type_definition, int type_definition_len TSRMLS_DC)
{
	w32api_type_handle **th;
	w32api_type_handle_ptr hnd;

	th = emalloc(sizeof(w32api_type_handle *));
	*th = NULL;

	w32api_type_definition_scan_bytes(type_definition, type_definition_len);
	if(w32api_type_definition_parse((void *)&hnd) != 0)
	{
		*th = hnd.hnd;

		/* Leaks */
		if(*th != NULL)
			efree(*th);

		efree(th);
		return FAILURE;
	}

	*th = hnd.hnd;

	if(!*th)
		return FAILURE;

	if((zend_hash_exists(WG(callbacks), (*th)->type_name, strlen((*th)->type_name) +1)) || 
	   (zend_hash_exists(WG(types), (*th)->type_name, strlen((*th)->type_name) + 1)))
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
				   "A type or callback by the name %s already has been registered, cannot redefine type or callback", 
				   (*th)->type_name);

		/* We dont want to unload function as it already exists so lets just free it ourselves */
		php_w32api_free_members((*th)->member_list);
		efree((*th)->type_name);
		efree(*th);
		efree(th);

		return FAILURE;
	}

	/* Insert it into our hash table */
	if(zend_hash_add( WG(types), 
					  (*th)->type_name, 
					  strlen((*th)->type_name) + 1, 
					  th, 
					  sizeof(w32api_type_handle *),
					  NULL) != SUCCESS)
	{

		php_error_docref(NULL TSRMLS_CC, E_WARNING, 
				   "Loading of type %s failed: Could not insert type handle into hash", 
				   (*th)->type_name);

		/* Tidy up */
		zend_hash_del(WG(types), (*th)->type_name, 	strlen((*th)->type_name) + 1);
		return FAILURE;
	}

	return SUCCESS;

}
/* }}} */


/* {{{ php_w32api_register_callback
 */
static int php_w32api_register_callback(char *function_definition, int function_definition_len TSRMLS_DC)
{
	w32api_func_handle **fh;
	w32api_func_handle_ptr hnd;

	char *new_definition = NULL;

	fh = emalloc(sizeof(w32api_func_handle *));
	*fh = NULL;

	new_definition = emalloc(function_definition_len + sizeof(" from cb.cb"));

	snprintf(new_definition, function_definition_len + sizeof(" from cb.cb"), "%s from cb.cb", function_definition);


	/* Parse function */
	w32api_function_definition_scan_bytes(new_definition, function_definition_len + sizeof(" from cb.cb"));
	if(w32api_function_definition_parse((void *)&hnd) != 0)
	{
		*fh = hnd.hnd;

		/* Leaks */
		if(*fh != NULL)
			efree(*fh);

		efree(fh);
		return FAILURE;
	}
	*fh = hnd.hnd;

	if(!*fh)
		return FAILURE;


	if(zend_hash_exists(WG(callbacks), (*fh)->function_name, strlen((*fh)->function_name) + 1))
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING, 
				   "A callback by the name %s already has been registered, cannot redefine type", 
				   (*fh)->function_name);

		/* We dont want to unload function as it already exists so lets just free it ourselves */
		php_w32api_free_arguments((*fh)->argument_list);
		efree((*fh)->return_type_name);
		efree((*fh)->function_name);
		efree(*fh);
		efree(fh);

		return FAILURE;
	}

	/* Insert it into our hash table */
	if(zend_hash_add( WG(callbacks), 
					  (*fh)->function_name, 
					  strlen((*fh)->function_name) + 1, 
					  fh, 
					  sizeof(w32api_func_handle *),
					  NULL) != SUCCESS)
	{

		php_error_docref(NULL TSRMLS_CC, E_WARNING,
				   "Loading of function %s failed: Could not insert function handle into hash", 
				   (*fh)->function_name);

		/* Tidy up */
		zend_hash_del(WG(callbacks), (*fh)->function_name, 	strlen((*fh)->function_name) + 1);
		return FAILURE;
	}

	return SUCCESS;



}
/* }}} */


/* {{{ php_w32api_free_arguments
 * Expects one argument, the head of a list of arguments to free 
 */
static void php_w32api_free_arguments(arguments *argument_list)
{
	if(argument_list == NULL)
		return;

	efree(argument_list->arg->argument_name);
	efree(argument_list->arg->type_name);
	efree(argument_list->arg);

	if(argument_list->next_arg != NULL)
	{
		php_w32api_free_arguments(argument_list->next_arg);
	}
	
	efree(argument_list);

	return;
}
/* }}} */

/* {{{ php_w32api_free_members
 * Expects one argument, the head of a list of members to free 
 */
static void php_w32api_free_members(members *member_list)
{
	if(member_list == NULL)
		return;

	efree(member_list->member->member_name);
	
	if(member_list->member->member_type_name != NULL)
		efree(member_list->member->member_type_name);

	efree(member_list->member);

	php_w32api_free_members(member_list->next_member);
	efree(member_list);
	return;
}
/* }}} */

/* {{{ php_w32api_load_library
 * Expects two parameters, first is libraries name the second is a pointer
 * to a pointer to w32api_lib_handle which will recieve the resultant handle.
 * returns SUCCESS on success and FAILURE on failure.
 */
static int php_w32api_load_library (char *library_name, w32api_lib_handle **lh TSRMLS_DC)
{
	if(zend_hash_find(WG(libraries), library_name, strlen(library_name) + 1, (void **)lh) == SUCCESS)
	{
		(*lh)->ref_count++;
		return SUCCESS;
	}

	*lh = (w32api_lib_handle *) emalloc( sizeof(w32api_lib_handle) );
	(*lh)->ref_count = 1;
	(*lh)->library_name = estrdup(library_name);

	(*lh)->handle = LoadLibrary((*lh)->library_name);

	if(!(*lh)->handle)												 /* Could not load library */
	{
		LPVOID message_buffer;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					   FORMAT_MESSAGE_FROM_SYSTEM | 
					   FORMAT_MESSAGE_IGNORE_INSERTS,
					   NULL,
					   GetLastError(),
					   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
					   (LPTSTR)&message_buffer,
					   0,
					   NULL);

		/* Tidy up */
		efree((*lh)->library_name);
		efree(*lh);
		efree(lh);

		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Loading of library failed: %s", message_buffer);
		LocalFree(message_buffer);

		return FAILURE;
	}

	/* Add to hash */
	if(zend_hash_add( WG(libraries), 
					  (*lh)->library_name, 
					  strlen((*lh)->library_name) + 1, 
					  *lh, 
					  sizeof(w32api_lib_handle),
					  NULL) != SUCCESS)
	{

		php_error_docref(NULL TSRMLS_CC, E_WARNING, 
				   "Loading of library %s failed: Could not insert library handle into hash", 
				   (*lh)->library_name);

		/* Tidy up */
		efree((*lh)->library_name);
		efree(*lh);
		efree(lh);

		return FAILURE;
	}


	return SUCCESS;

}
/* }}} */


/* {{{ php_w32api_do_arg_types
 */
static unsigned char *php_w32api_do_arg_types(arguments **argument_list)
{
		int i = 0;
		int j = 0;
		arguments *curr_arg = NULL;
		unsigned char *retval = NULL;

		if(!(argument_list) || !(*argument_list))
			return NULL;

		curr_arg = *argument_list;
		
		/* See how much room we need to emalloc */
		while(curr_arg)
		{
			i++;

			if(curr_arg->arg->flags & BYREF_FORCE)
			{
				j = i;
			}
			
			curr_arg = curr_arg->next_arg;
		}

		/* Check to see if any args are by ref */
		if( j == 0 )
			return NULL;

		retval = (unsigned char *)emalloc(sizeof(unsigned char) * j + 1);
		retval[0] = (unsigned char)j;

		curr_arg = *argument_list;

		for(i=1; i <= j; i++)
		{

			retval[i] = (unsigned char)curr_arg->arg->flags;
			curr_arg = curr_arg->next_arg;

		}

		return retval;
}
/* }}} */

static int php_w32api_get_type_size(int type_id, char *type_name, int flags)
{
	TSRMLS_FETCH();

	if(flags & BYREF_FORCE)
	{
		return sizeof(void *);								/* Pointers are always the same size */
	}

	switch(type_id)
	{									
		case W32API_NULL:									
			return sizeof(void *);
		case W32API_INT:		
			return sizeof(int);
		case W32API_LONG:	
			return sizeof(long);
		case W32API_DOUBLE:		
			return sizeof(double);
		case W32API_FLOAT:		
			return sizeof(float);
		case W32API_STRING:		
			return sizeof(char *);
		case W32API_BYTE:		
			return sizeof(char);
		case W32API_BOOL:		
			return sizeof(int);
		case W32API_COMPLEX:
			{
				w32api_type_handle **th;
				
				if(zend_hash_find(WG(types), type_name, strlen(type_name) +1, (void **)&th) != SUCCESS)
				{
						php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unknown type %s", type_name);
						return -1;
				}

				return (*th)->size;

			}
			break;		
		case W32API_UNKNOWN:
		default:
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unknown type %s", type_name);
			return -1;
	}
}

static int php_w32api_get_type_id_from_name(char *type)
{

	TSRMLS_FETCH();

	if(!strcmp(type, "long"))
	{
		return W32API_LONG;
	}
	else if(!strcmp(type, "int"))
	{
		return W32API_INT;
	}
	else if (!strcmp(type, "string")) 
	{
		return W32API_STRING;
	}
	else if (!strcmp(type, "byte"))
	{
		return W32API_BYTE;
	}
	else if (!strcmp(type, "bool"))
	{
		return W32API_BOOL;
	}
	else if (!strcmp(type, "double"))
	{
		return W32API_DOUBLE;
	}
	else if (!strcmp(type, "float"))
	{
		return W32API_FLOAT;
	}
	else if (!strcmp(type, "void"))
	{
		return W32API_NULL;
	}
	else
	{
		if(zend_hash_exists(WG(types), type, strlen(type) +1))
		{
			return W32API_COMPLEX;
		}
		else
		{
			return W32API_UNKNOWN;
		}
	} 
}

static void php_w32api_init_type(w32api_type_handle *th, zval *obj TSRMLS_DC)
{
	w32api_type_instance *ti;
	zval *rsrc_handle = NULL;

	ti = emalloc(sizeof(w32api_type_instance));

	if(!obj)
		MAKE_STD_ZVAL(obj);

	object_init_ex(obj, WG(type_ce));

	ti->type = th;
	ti->values = emalloc(sizeof(zval *) * th->member_count);
	memset(ti->values, '\0', sizeof(zval *) * th->member_count);

	MAKE_STD_ZVAL(rsrc_handle);
	ZEND_REGISTER_RESOURCE(rsrc_handle, ti, WG(le_type_instance));

	zend_hash_index_update(Z_OBJPROP_P(obj), 0, &rsrc_handle, sizeof(zval *), NULL);
}


static int php_w32api_do_prop_get(zval *object, zval *return_value, zend_llist_element **element TSRMLS_DC)
{
	w32api_type_instance *th;
	zval **type_instance_handle;
	members *current_member;
	char *property_name;
	int i = 0;

	zend_hash_index_find(Z_OBJPROP_P(object), 0, (void **) &type_instance_handle);

	th = (w32api_type_instance *)zend_fetch_resource(type_instance_handle TSRMLS_CC, 
													  -1, "Complex Type Instance", NULL, 
													   1, WG(le_type_instance));	

	if(!th)
		return FAILURE;

	property_name = Z_STRVAL(((zend_overloaded_element *)(*element)->data)->element);

	current_member = th->type->member_list;

	while(strcmp(current_member->member->member_name, property_name) != 0)
	{
		i++;

		if(current_member->next_member != NULL)
			current_member = current_member->next_member;
		else
			return FAILURE;
	}

	*return_value = *(th->values[i]);
	zval_copy_ctor(return_value);

	return SUCCESS;
}

static int php_w32api_do_prop_set(zval *object, zval *value, zend_llist_element **element TSRMLS_DC)
{
	w32api_type_instance *th;
	zval **type_instance_handle;
	zval *new_var;
	members *current_member;
	char *property_name;
	int i = 0;

	zend_hash_index_find(Z_OBJPROP_P(object), 0, (void **) &type_instance_handle);

	th = (w32api_type_instance *)zend_fetch_resource(type_instance_handle TSRMLS_CC, 
													  -1, "Complex Type Instance", NULL, 
													   1, WG(le_type_instance));	

	if(!th)
		return FAILURE;

	property_name = Z_STRVAL(((zend_overloaded_element *)(*element)->data)->element);

	current_member = th->type->member_list;

	while(strcmp(current_member->member->member_name, property_name) != 0)
	{
		i++;

		if(current_member->next_member != NULL)
			current_member = current_member->next_member;
		else
			return FAILURE;
	}
	
	if(current_member->member->flags & BYREF_FORCE)
	{
		if(th->values[i])
			zval_ptr_dtor(&th->values[i]);

		MAKE_STD_ZVAL(new_var);
		*new_var = *value;
		zval_copy_ctor(new_var);
		th->values[i] = new_var;

	}
	else
	{
		th->values[i] = value;
		zval_add_ref(&value);
	}

	return SUCCESS;
}

w32api_result php_w32api_do_dynamic_dll_call(w32api_func_handle *fh, int argc, w32api_dynamic_param *params, void *return_buffer, int return_buffer_size)
{

	/**
	 * Theory Behind Implementation
	 * ============================
	 * We have four main jobs:
	 * 1) Push arguments onto stach aligned at 4 bytes.
	 * 2) Call Function
	 * 3) Get Return Values
	 * 4) Perform any cleanup needed.
	 *
	 * Pushing arguments onto the stack is fairly simple, just push from right to left
	 * so for a function with the prototype int sum(int a, int b) we would push b and
	 * then a in that order.
	 *
	 * Calling the function is fine as we already have the pointer to the function which
	 * we can use with call [function_pointer] to make the actual call.
	 *
	 * Return values are where we begin to get complicated. Now for simple return values up
	 * to 8 bytes they are returned via the EAX/EDX register pair. This means we can just
	 * copy the EAX/EDX pair to the win32_result sturcture and be sure we get any simple
	 * return type. If the return type is more than 8 bytes then things get complicated.
	 * When calling we must pass a hidden argument on the stach which points to a tempory
	 * buffer with enough memory to hold the return value, this return value is then copied
	 * to the correct varaible by us. Microsoft being the nice bunnies they are, decided to
	 * copy an optimization Borland introduced under win16 which is to pass structs of under
	 * 8 bytes directly via EAX/EDX pair. One final notable exception is dealing with floating
	 * point return types where we need to retrive the floating point number of the systems
	 * math coprocessor stack using the fstp call.
	 *
	 * Finally if its a __cdecl call we have to clean up the stack, otherwise the callee does this.
	 *
	 */

	w32api_result result = { 0 };
	DWORD *stack_pointer, stack_size = 0, eaxv, edxv;
	BYTE *arg_ptr = NULL;
	int size = 0, i = 0;
	FARPROC fp = fh->handle;

	_asm mov stack_pointer, esp		// Store stack pointer (esp) in stack_pointer
	_asm sub esp, 0x100				// Give ourselves 256 bytes on the stack


	for(i = (argc - 1); i >= 0; i--)
	{
		size = (params[i].width + 3)/4 * 4;
		arg_ptr = (unsigned char *)params[i].argument_ptr + size - 4;
		stack_size += (unsigned long)size;

		while(size > 0)
		{
			stack_pointer--;
			if(params[i].flags == W32API_ARGPTR)
			{
				*stack_pointer = *(unsigned long *)arg_ptr;
				arg_ptr -= 4;
			}
			else
			{
				*stack_pointer = params[i].argument;
			}

			size -= 4;
		}
	}

	if((return_buffer) && ((fh->flags & W32API_BORLAND) || (return_buffer_size > 8)))
	{

		stack_size += 4;
		stack_pointer--;
		*stack_pointer = (unsigned long)return_buffer;
	}

	_asm add esp, 0x100
	_asm sub esp, stack_size
	_asm call [fp]
	_asm mov eaxv, eax
	_asm mov edxv, edx

	if(fh->flags & W32API_CDECL)
	{
		_asm add esp, stack_size
	}

	if(fh->flags & W32API_REAL4)
		_asm fstp dword ptr [result]
	else if (fh->flags & W32API_REAL8)
		_asm fstp qword ptr [result]
	else if (!return_buffer)
	{
		_asm mov eax, [eaxv]
		_asm mov edx, [edxv]
		_asm mov DWORD PTR [result], eax
		_asm mov DWORD PTR [result + 4], edx
	}
	else if (!(fh->flags & W32API_BORLAND) && (return_buffer_size <= 8))
	{
		_asm mov ecx, DWORD PTR [return_buffer]
		_asm mov eax, [eaxv]
		_asm mov DWORD PTR [ecx], eax
		_asm mov edx, [edxv]
		_asm mov DWORD PTR [ecx + 4], edx
	}

	return result;
}

void php_w32api_marshall_zval_to_c(argument *arg, w32api_dynamic_param *dp, zval *pzval TSRMLS_DC)
{
	dp->flags = 0;

	/* We should have been passed a write reference when
	 * BYREF_FORCE is Set so we just add a reference
	 * when we pass it to the function,
	 * TODO: register the reference internally for safe unreferencing
	 */

	switch(arg->type_id)
	{
		case W32API_INT:
			convert_to_long_ex(&pzval);
			if(arg->flags & BYREF_FORCE)
			{
				dp->argument = (unsigned long)&pzval->value.lval;
				dp->width = sizeof(int *);
				
			}
			else
			{
				dp->argument = (int)pzval->value.lval;
				dp->width = sizeof(int);
			}
			break;
		case W32API_LONG:
			convert_to_long_ex(&pzval);

			if(arg->flags & BYREF_FORCE)
			{
				dp->argument = (unsigned long)&pzval->value.lval;
				dp->width = sizeof(int *);
				zval_add_ref(&pzval);
			}
			else
			{
				dp->argument = pzval->value.lval;
				dp->width = sizeof(int);
			}
			break;
		case W32API_STRING:

			convert_to_string_ex(&pzval);
			if(!(arg->flags & BYREF_FORCE))
			{
				/* Need to free this when we demarshall */
				dp->argument = (unsigned long)estrndup(Z_STRVAL_P(pzval), Z_STRLEN_P(pzval));
			}
			else
			{
				dp->argument = (unsigned long)Z_STRVAL_P(pzval);
				zval_add_ref(&pzval);
			}
		
			dp->width = sizeof(char *);
			break;

		case W32API_DOUBLE:
			convert_to_double_ex(&pzval);

			if(arg->flags & BYREF_FORCE)
			{
				dp->argument = (unsigned long)&pzval->value.dval;
				dp->width = sizeof(double *);
				zval_add_ref(&pzval);
			}
			else
			{
				dp->argument_ptr = &pzval->value.dval;
				dp->width = sizeof(double);
				dp->flags = W32API_ARGPTR;
			}
			break;
		case W32API_FLOAT:
			convert_to_double_ex(&pzval);

			if(arg->flags & BYREF_FORCE)
			{
				dp->argument = (unsigned long)&pzval->value.dval;
				dp->width = sizeof(double *);
				zval_add_ref(&pzval);
			}
			else
			{
				dp->argument_ptr = &pzval->value.dval;
				dp->width = sizeof(float);
				dp->flags = W32API_ARGPTR;
			}
			break;
		case W32API_BYTE:
					/* Thanks sterling */
			convert_to_string_ex(&pzval);
			if(arg->flags & BYREF_FORCE)
			{
				dp->argument = (unsigned long)&Z_STRVAL_P(pzval);
				dp->width = sizeof(char *);
				zval_add_ref(&pzval);
			}
			else
			{
				dp->argument = (char)Z_STRVAL_P(pzval)[0];
				dp->width = sizeof(char);
			}
			break;
		case W32API_BOOL:
			convert_to_boolean_ex(&pzval);

			if(arg->flags & BYREF_FORCE)
			{
				dp->argument = (unsigned long)&pzval->value.lval;
				dp->width = sizeof(int *);
				zval_add_ref(&pzval);
			}
			else
			{
				dp->argument = (int)pzval->value.lval;
				dp->width = sizeof(int);
			}
			break;
		case W32API_COMPLEX:
			if(Z_TYPE_P(pzval) != IS_OBJECT)
			{
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "Variable passed as complex value is not an object");
				break;
			}

			if(arg->flags & BYREF_FORCE)
			{
				int width= 0;
				void **ptr = NULL;
				ptr = emalloc(sizeof(void *));
				*ptr = php_w32api_complex_marshall_zval_to_c(pzval, &width, NULL TSRMLS_CC);
				dp->argument = (unsigned long)ptr;
				dp->width = width;
			}
			else
			{
				int width= 0;
				dp->argument_ptr = php_w32api_complex_marshall_zval_to_c(pzval, &width, NULL TSRMLS_CC);
				dp->width = width;
			}
			break;

		case W32API_UNKNOWN:
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unknown type when calling function, marshalling failed");
			break;
	}

}

static void *php_w32api_complex_marshall_zval_to_c(zval *pzval, int *width, void *return_value TSRMLS_DC)
{
	w32api_type_instance *th;
	zval **type_instance_handle;
	members *current_member;
	char *offset = return_value;
	int i = 0;


	if(return_value == NULL)
	{
		/* First call */
/*		return_value = emalloc(th->type->size);

		zend_hash_index_find(Z_OBJPROP_P(object), 0, (void **) &type_instance_handle);
		th = (w32api_type_instance *)zend_fetch_resource(type_instance_handle TSRMLS_CC, 
														  -1, "Complex Type Instance", NULL, 
														   1, WG(le_type_instance));	

		if(!th)
			return NULL;


		for(i = 0; i < th->type->member_count; i++)
		{
			

		}
*/	}
	

}


/* =====================================================================================================
 * Win32 Class Code
 * =====================================================================================================
 */

/* {{{ win32_class_functions[]
 */
function_entry win32_class_functions[] = {
	W32API_CLASS_FE(win32, registerfunction, NULL)
	W32API_CLASS_FE(win32, unregisterfunction, NULL)
	W32API_CLASS_FE(win32, registercallback, NULL)
	W32API_CLASS_FE(win32, definetype, NULL)
	W32API_CLASS_FE(win32, gettypesize, NULL)
	W32API_CLASS_FE(win32, inittype, NULL)
	W32API_CLASS_FE(win32, decref, NULL)
#ifndef NDEBUG
	W32API_CLASS_FE(win32, dump_function_hash, NULL)
	W32API_CLASS_FE(win32, dump_library_hash, NULL)
	W32API_CLASS_FE(win32, dump_callback_hash, NULL)
	W32API_CLASS_FE(win32, dump_type_hash, NULL)
#endif
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ win32_class_init(TSRMLS_D)
 */
int win32_class_init(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, 
					 "win32", 
					  win32_class_functions);

	WG(win32_ce) = zend_register_internal_class(&ce TSRMLS_CC);

	return SUCCESS;
}
/* }}} */

/* {{{ win32_class_rshutdown(TSRMLS_D)
 * Cleans up at the end of the shutdown removing and freeing anything we added to the function
 * table.
 */
int win32_class_rshutdown(TSRMLS_D)
{

	return SUCCESS;
}
/* }}} */

/* {{{ proto: int Win32::RegisterFunction(string definition [, int flags])
 *	Registers and Loads a function from an underlying Dll 
 */
W32API_CLASS_FUNCTION(win32, registerfunction)
{
	char *function_definition = NULL;
	int function_definition_len;
	long flags = 0;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l",
							 &function_definition,
							 &function_definition_len,
							 &flags) == FAILURE)
	{
		return;
	}

	if(php_w32api_load_function(function_definition, function_definition_len, flags TSRMLS_CC) != SUCCESS)
	{
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Registering Function %s failed", function_definition);
		RETURN_FALSE;
	}

	RETURN_TRUE;


}
/* }}} */

/* {{{ proto: int Win32::UnregisterFunction(string function_name)
 *	Unregisters a previously loaded function
 */
W32API_CLASS_FUNCTION(win32, unregisterfunction)
{
	char *function_name = NULL;
	int function_name_len;
	w32api_func_handle **fh = NULL;
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l",
							 &function_name,
							 &function_name_len) == FAILURE)
	{
		return;
	}

	/* Our deleteor handler removes us from the WG(win32_ce)->function_table
	 * so no need to delete specifically from there
	 */
	zend_hash_del(WG(funcs), function_name, strlen(function_name) + 1);

	RETURN_TRUE;

}
/* }}} */

/* {{{ proto: int Win32::RegisterCallback(string definition)
 *	Registers a callback type
 */
W32API_CLASS_FUNCTION(win32, registercallback)
{
	char *function_definition = NULL;
	int function_definition_len;
	w32api_func_handle **fh = NULL;
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l",
							 &function_definition,
							 &function_definition_len) == FAILURE)
	{
		return;
	}

	if(php_w32api_register_callback(function_definition, function_definition_len TSRMLS_CC) != SUCCESS)
	{
		RETURN_FALSE;
	}

	RETURN_TRUE;

}
/* }}} */


/* {{{ proto: int Win32::DefineType(string definition)
 *	Defines a C Like Type for use.
 */
W32API_CLASS_FUNCTION(win32, definetype)
{
	char *type_definition = NULL;
	int type_definition_len;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
							 &type_definition,
							 &type_definition_len) == FAILURE)
	{
		return;
	}

	if(php_w32api_register_type(type_definition, type_definition_len TSRMLS_CC) != SUCCESS)
	{
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Registering Type %s failed", type_definition);
		RETURN_FALSE;
	}

	RETURN_TRUE;

}
/* }}} */

/* {{{ proto: int Win32::GetTypeSize(string type_name)
 *	Returns the size of a registered type
 */
W32API_CLASS_FUNCTION(win32, gettypesize)
{
	char *type = NULL;
	int type_len;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
							 &type,
							 &type_len) == FAILURE)
	{
		return;
	}

	RETURN_LONG(php_w32api_get_type_size(php_w32api_get_type_id_from_name(type), type, BYREF_NONE));
	

}
/* }}} */

/* {{{ proto: int Win32::InitType(string TypeName)
 *	Creates an instance of type TypeName
 */
W32API_CLASS_FUNCTION(win32, inittype)
{
	char *type_name = NULL;
	int type_name_len = 0;
	w32api_type_handle **th = NULL;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
							 &type_name, &type_name_len) == FAILURE)
	{
		return;
	}

	if(zend_hash_find(WG(types), type_name, type_name_len +1, (void **)&th) == FAILURE)
	{
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Could not retrieve type handle for type %s from hash table", type_name);
		RETURN_FALSE;
	}

	php_w32api_init_type(*th, return_value TSRMLS_CC);

}
/* }}} */

/* {{{ proto: int Win32::DecRef(mixed Variable)
 *	Decreases the reference count on a variable
 */
W32API_CLASS_FUNCTION(win32, decref)
{

}
/* }}} */

/* {{{ XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
 *	THIS FUNCTION IS NOT PUBLICALLY ACCESSABLE
 *  IT IS USED AS A GENERIC HANDLER FOR W32API
 *  CALLS.
 */
W32API_CLASS_FUNCTION(win32, invokefunction)
{
	char *function_name = get_active_function_name(TSRMLS_C);
	int argc = ZEND_NUM_ARGS();
	int i = 0;
	w32api_dynamic_param *params = NULL;
	w32api_dynamic_param *current_dynamic_param = NULL;
	w32api_func_handle **fh = NULL;
	w32api_result res = {0};
	void *w32api_return_buffer = NULL;
	int w32api_return_buffer_size = 0;
	zval **func_arguments = NULL;
	zval *current_zval = NULL;
	arguments *curr_arg = NULL;
	w32api_type_handle *th = NULL;

	if(zend_hash_find(WG(funcs), function_name, strlen(function_name) +1, (void **)&fh) == FAILURE)
	{
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Could not retrieve function handle from hash table");
		RETURN_FALSE;
	}

	if(argc)
	{
		if(zend_get_parameters_array_ex(argc, &func_arguments) == FAILURE)
		{
			WRONG_PARAM_COUNT
		}

		params = (w32api_dynamic_param *)emalloc(sizeof(w32api_dynamic_param) * argc);

		curr_arg = (*fh)->argument_list;
		current_dynamic_param = params;

		for(i = 0; i < argc; i++)
		{
			current_zval = func_arguments[i];
			php_w32api_marshall_zval_to_c(curr_arg->arg, current_dynamic_param, current_zval TSRMLS_CC);

			current_dynamic_param++;
			curr_arg = curr_arg->next_arg;
		}
	}
	else
	{
		params = NULL;
	}

	if((*fh)->return_type_id == W32API_COMPLEX)
	{
		if(zend_hash_find(WG(types), (*fh)->return_type_name, strlen((*fh)->return_type_name) +1, (void **)&th) != SUCCESS)
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Could not find type handle for type %s", (*fh)->return_type_name);

		w32api_return_buffer = emalloc(th->size);
		w32api_return_buffer_size = th->size;
	}



	res = php_w32api_do_dynamic_dll_call(*fh, argc, params, w32api_return_buffer, w32api_return_buffer_size);

	if(argc)
	/* We should demarshall here not just efree */
		efree(params);

	switch((*fh)->return_type_id)
	{
		case W32API_LONG:
			RETURN_LONG(res.lval);
			break;
		case W32API_INT:
			RETURN_LONG(res.ival);
			break;
		case W32API_STRING:
		case W32API_BYTE:
			RETURN_STRING(res.ptr, 1);
			break;
		case W32API_DOUBLE:
			RETURN_DOUBLE(res.dval);
			break;
		case W32API_FLOAT:
			RETURN_DOUBLE(res.fval);
			break;
		case W32API_BOOL:
			if(res.ival)
			{
				RETURN_TRUE;
			}
			else
			{
				RETURN_FALSE;
			}
			break;
		case W32API_COMPLEX:
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown return type %s", (*fh)->return_type_name);
	}

}
/* }}} */

#ifndef NDEBUG
W32API_CLASS_FUNCTION(win32, dump_library_hash)
{
	zend_hash_apply(WG(libraries), (apply_func_t)php_w32api_dump_library_hash_cb TSRMLS_CC);
}

W32API_CLASS_FUNCTION(win32, dump_function_hash)
{
	zend_hash_apply(WG(funcs), (apply_func_t)php_w32api_dump_function_hash_cb TSRMLS_CC);
}

W32API_CLASS_FUNCTION(win32, dump_callback_hash)
{

	zend_hash_apply(WG(callbacks), (apply_func_t)php_w32api_dump_callback_hash_cb TSRMLS_CC);
}


W32API_CLASS_FUNCTION(win32, dump_type_hash)
{

	zend_hash_apply(WG(types), (apply_func_t)php_w32api_dump_type_hash_cb TSRMLS_CC);
}

int php_w32api_dump_library_hash_cb(void *pData TSRMLS_DC)
{
	w32api_lib_handle *lh = pData;

	printf("=====================================================================\n");
	printf("Library Name:    \t\t%s\n", lh->library_name);
	printf("Reference Count: \t\t%d\n", lh->ref_count);
	printf("Library Handle:  \t\t%p\n", lh->handle);
	printf("Lib ptr loc      \t\t%p\n", lh);
	printf("ll n loc         \t\t%p\n", &lh->ref_count);

	printf("=====================================================================\n");

	return 0;
}

int php_w32api_dump_function_hash_cb(void *pData TSRMLS_DC)
{
	w32api_func_handle **fh = pData;

	printf("=====================================================================\n");
	printf("Function Name:    \t\t%s\n", (*fh)->function_name);
	printf("Return Type Name: \t\t%s\n", (*fh)->return_type_name);
	printf("Library Name:     \t\t%s\n", (*fh)->lib->library_name );
	printf("Function Flags:   \t\t%d\n", (*fh)->flags);
	printf("Function Handle:  \t\t%p\n", (*fh)->handle);
	printf("Return Type ID:   \t\t%d\n", (*fh)->return_type_id);
	printf("Return Type Name: \t\t%s\n", (*fh)->return_type_name);
	printf("## Arguments ##\n");
	printf("---------------------------------------------------------------------\n");
	php_w32api_print_arguments((*fh)->argument_list);
	printf("=====================================================================\n\n");

	return 0;
}

int php_w32api_dump_callback_hash_cb(void *pData TSRMLS_DC)
{
	w32api_func_handle **fh = pData;

	printf("=====================================================================\n");
	printf("Callback Name:    \t\t%s\n", (*fh)->function_name);
	printf("Return Type Name: \t\t%s\n", (*fh)->return_type_name);
	printf("Callback Flags:   \t\t%d\n", (*fh)->flags);
	printf("Return Type ID:   \t\t%d\n", (*fh)->return_type_id);
	printf("Return Type Name: \t\t%s\n", (*fh)->return_type_name);
	printf("## Arguments ##\n");
	printf("---------------------------------------------------------------------\n");
	php_w32api_print_arguments((*fh)->argument_list);
	printf("=====================================================================\n\n");

	return 0;
}

int php_w32api_dump_type_hash_cb(void *pData TSRMLS_DC)
{
	w32api_type_handle **th = pData;


	printf("=====================================================================\n");
	printf("Type Name:        \t\t%s\n", (*th)->type_name);
	printf("Type Size:        \t\t%d\n", (*th)->size);
	printf("Member Count:     \t\t%d\n", (*th)->member_count);
	printf("## Members ##\n");
	printf("---------------------------------------------------------------------\n");
	php_w32api_print_members((*th)->member_list);
	printf("=====================================================================\n\n");

	return 0;
}

void php_w32api_print_members(members *member_list)
{
	if(member_list == NULL)
		return;

	printf("\tMember Name:     \t%s\n", member_list->member->member_name);
	printf("\tMember Flags:    \t%d\n", member_list->member->flags);
	printf("\tMember Type ID:  \t%d\n", member_list->member->member_type_id);
	printf("\tMember Type Name:\t%s\n", member_list->member->member_type_name);
	printf("\tMember Offset:   \t%d\n", member_list->member->offset);
	printf("---------------------------------------------------------------------\n");

	php_w32api_print_members(member_list->next_member);

}
void php_w32api_print_arguments(arguments *argument_list)
{
	if(argument_list == NULL)
		return;

	printf("\tArgument Name:   \t%s\n", argument_list->arg->argument_name);
	printf("\tArgument Flags:  \t%d\n", argument_list->arg->flags);
	printf("\tArgument Type ID:\t%d\n", argument_list->arg->type_id);
	printf("\tArg Type Name:   \t%s\n", argument_list->arg->type_name);
	printf("---------------------------------------------------------------------\n");

	php_w32api_print_arguments(argument_list->next_arg);
}
#endif
/* =====================================================================================================
 * Type Class Code
 * =====================================================================================================
 */

/* {{{ type_class_functions[]
 */
function_entry type_class_functions[] = {
	W32API_CLASS_FE(type, clone, NULL)
	{NULL, NULL, NULL}
};
/* }}} */


/* {{{ type_class_init(TSRMLS_DC)
 */
int type_class_init(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_OVERLOADED_CLASS_ENTRY(ce, 
								"type", 
								type_class_functions, 
								NULL, 
								W32API_PROP_GET_FUNCTION_N(type),
								W32API_PROP_SET_FUNCTION_N(type));

	WG(type_ce) = zend_register_internal_class(&ce TSRMLS_CC);

	return SUCCESS;
}
/* }}} */

/* {{{ W32API_PROP_SET_FUNCTION(type)
 */
/* Most of this code is borrowed from php-gtk. Thanks to Andrei and Zeev for their 
 * help with the prop-get/set functions 
 */
W32API_PROP_SET_FUNCTION(type)
{
	zval result, temp;
	zval *temp_ptr = &temp;
	zval *new_val;
	zend_overloaded_element *overloaded_property;
	zend_llist_element *element;
	zval **object = &property_reference->object;
	int setter_retval, getter_retval;
	TSRMLS_FETCH();


	/* If we have $foo->bar->baz->boo->bin we have to do sucessive propgets
	 * Until we can do our prop set (thanks Zeev, Andrei)
	 */
	for (element=property_reference->elements_list->head; element != property_reference->elements_list->tail; element=element->next) {
		overloaded_property = (zend_overloaded_element *)element->data;

		getter_retval = FAILURE;
		if (Z_TYPE_P(overloaded_property) == OE_IS_OBJECT) {
			if (Z_TYPE_PP(object) == IS_NULL ||
				(Z_TYPE_PP(object) == IS_BOOL && Z_LVAL_PP(object) == 0) ||
				(Z_TYPE_PP(object) == IS_STRING && Z_STRLEN_PP(object) == 0)) {
				object_init(*object);
			}
		

			/* Trying to access a property on a non-object. */
			if (Z_TYPE_PP(object) != IS_OBJECT) {
				return FAILURE;
			} 

			getter_retval = php_w32api_do_prop_get(*object, &result, &element TSRMLS_CC);

			if (getter_retval == SUCCESS) {
				temp = result;
				object = &temp_ptr;
			} else {
				if ((getter_retval = zend_hash_find(Z_OBJPROP_PP(object),
											   Z_STRVAL(overloaded_property->element),
											   Z_STRLEN(overloaded_property->element)+1,
											   (void **)&object)) == FAILURE) {
					MAKE_STD_ZVAL(new_val);
					ZVAL_NULL(new_val);
					zend_hash_update(Z_OBJPROP_PP(object),
									 Z_STRVAL(overloaded_property->element),
									 Z_STRLEN(overloaded_property->element)+1,
									 &new_val, sizeof(void *), (void **)&object);
				}
			}

		} else if (Z_TYPE_P(overloaded_property) == OE_IS_ARRAY) {
			if (Z_TYPE_PP(object) == IS_NULL ||
				(Z_TYPE_PP(object) == IS_BOOL && Z_LVAL_PP(object) == 0) ||
				(Z_TYPE_PP(object) == IS_STRING && Z_STRLEN_PP(object) == 0)) {
				array_init(*object);
			}

			/* Trying to access index on a non-array. */
			if (Z_TYPE_PP(object) != IS_ARRAY) {
				return FAILURE;
			}
			
			if (Z_TYPE(overloaded_property->element) == IS_STRING) {
				getter_retval = zend_hash_find(Z_ARRVAL_PP(object),
											   Z_STRVAL(overloaded_property->element),
											   Z_STRLEN(overloaded_property->element)+1,
											   (void **)&object);
			} else if (Z_TYPE(overloaded_property->element) == IS_LONG) {
				getter_retval = zend_hash_index_find(Z_ARRVAL_PP(object),
													 Z_LVAL(overloaded_property->element),
													 (void **)&object);
			}

			if (getter_retval == FAILURE) {
				MAKE_STD_ZVAL(new_val);
				ZVAL_NULL(new_val);

				if (Z_TYPE(overloaded_property->element) == IS_STRING) {
					zend_hash_update(Z_ARRVAL_PP(object),
									  Z_STRVAL(overloaded_property->element),
									  Z_STRLEN(overloaded_property->element)+1,
									  &new_val, sizeof(void *), (void **)&object);
				} else if (Z_TYPE(overloaded_property->element) == IS_LONG) {
					zend_hash_index_update(Z_ARRVAL_PP(object),
										   Z_LVAL(overloaded_property->element),
										   &new_val, sizeof(void *), (void **)&object);
				}
			}
		}

		zval_dtor(&overloaded_property->element);
	}
	
	/* object is now the object we want to set our property on */
	overloaded_property = (zend_overloaded_element *) element->data;
	setter_retval = php_w32api_do_prop_set(*object, value, &element TSRMLS_CC);	
	zval_dtor(&overloaded_property->element);

	return setter_retval;

}
/* }}} */

/* {{{ W32API_PROP_GET_FUNCTION(type)
 */
W32API_PROP_GET_FUNCTION(type)
{
	zval result;
	zval *result_ptr = &result;
	zval **prop_result;
	zend_overloaded_element *overloaded_property;
	zend_llist_element *element;
	zval object = *property_reference->object;
	int getter_retval;
	TSRMLS_FETCH();

	for (element=property_reference->elements_list->head; element; element=element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;

		getter_retval = FAILURE;
		ZVAL_NULL(&result);
		if (Z_TYPE_P(overloaded_property) == OE_IS_OBJECT) {
			/* Trying to access a property on a non-object. */
			if (Z_TYPE(object) != IS_OBJECT ||
				Z_TYPE(overloaded_property->element) != IS_STRING) {
				return result;
			}

			if ((getter_retval = php_w32api_do_prop_get(&object, &result, &element TSRMLS_CC) == FAILURE)) {
								if ((getter_retval = zend_hash_find(Z_OBJPROP(object),
											   Z_STRVAL(overloaded_property->element),
											   Z_STRLEN(overloaded_property->element)+1,
											   (void **)&prop_result)) == SUCCESS) {
					result = **prop_result;
				}
			}
		} else if (Z_TYPE_P(overloaded_property) == OE_IS_ARRAY) {
			/* Trying to access index on a non-array. */
			if (Z_TYPE(object) != IS_ARRAY) {
				return result;
			}

			if (Z_TYPE(overloaded_property->element) == IS_STRING) {
				getter_retval = zend_hash_find(Z_ARRVAL(object),
											   Z_STRVAL(overloaded_property->element),
											   Z_STRLEN(overloaded_property->element)+1,
											   (void **)&prop_result);
			} else if (Z_TYPE(overloaded_property->element) == IS_LONG) {
				getter_retval = zend_hash_index_find(Z_ARRVAL(object),
													 Z_LVAL(overloaded_property->element),
													 (void **)&prop_result);
			}
			if (getter_retval == SUCCESS)
				result = **prop_result;
		}

		zval_dtor(&overloaded_property->element);

		object = result;

		if (getter_retval == FAILURE) {
			return result;
		}
	}

	zval_add_ref(&result_ptr);
	SEPARATE_ZVAL(&result_ptr);
	return *result_ptr;
}
/* }}} */

/* {{{ proto: Type Type::Clone()
 *	Creates an exact clone of the object.
 */
W32API_CLASS_FUNCTION(type, clone)
{

}
/* }}} */

/* =====================================================================================================
 * Scanner & Parser Functions
 * =====================================================================================================
 */

/* -----------------------------
 * Function Definition Functions
 * -----------------------------
 */

/* {{{ w32api_parser_load_function_ex
 * Callback for the parser, if the library name is cb.cb we are registering a 
 * callback so the LoadLibary and GetProcAddress code is skipped 
 */
w32api_func_handle *w32api_parser_load_function_ex(char *return_type, char *function_name, char *alias_name, arguments *argument_list, char *library_name)
{
	w32api_func_handle *return_value;
	TSRMLS_FETCH();

	return_value = (w32api_func_handle *)emalloc(sizeof(w32api_func_handle));
	memset(return_value, '\0', sizeof(w32api_func_handle));

	return_value->argument_list = argument_list;
	return_value->flags = 0;
	return_value->function_name = (alias_name)?alias_name:function_name;				/* This is estrdup'd in the Scanner already!! */
	return_value->return_type_name = return_type;									/* This is estrdup'd in the Scanner already!! */
	return_value->return_type_id = php_w32api_get_type_id_from_name(return_type);

	if(strcmp("cb.cb", library_name))													/* Bit of a hack but we are registering a callback */
	{
		php_w32api_load_library(library_name, &return_value->lib TSRMLS_CC);

		if(!return_value->lib)
		{
			/* php_w32api_load_library has already given error */
			efree(return_value);
			return NULL;
		}

		return_value->handle = GetProcAddress(return_value->lib->handle, function_name);

		if(!return_value->handle)
		{
			/* Check for variation ending with A */
			char *ascii_name = NULL;
				
			ascii_name = emalloc(strlen(function_name) + 2);
			strcpy(ascii_name, function_name);
			ascii_name[strlen(function_name)] = 'A';
			ascii_name[strlen(function_name) + 1] = '\0';
			return_value->handle = GetProcAddress(return_value->lib->handle, ascii_name);
			efree(ascii_name);
			
			if(!return_value->handle)
			{
				/* TODO: php_error_docref and GetLastError etc */
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not load function %s", function_name);
				efree(return_value);
				return NULL;
			}
		}
	}

	/* We want function_name in lowercase now */
	php_strtolower(return_value->function_name, strlen(return_value->function_name));

	/* Free it if we have a alias */
	if(alias_name)
		efree(function_name);

	return return_value;
}
/* }}} */	

/* {{{ w32api_parser_make_argument
 * Helper function for the parser
 */
arguments *w32api_parser_make_argument(char *arg_type, char *arg_name, int byref)
{
	arguments *return_value = NULL;								/* Pointer to our return value */
	argument *argument_value = NULL;							/* Our actual argument */

	argument_value = emalloc(sizeof(argument));
	return_value = emalloc(sizeof(arguments));

	return_value->arg = argument_value;
	return_value->next_arg = return_value->prev_arg = NULL;

	argument_value->flags = byref;
	argument_value->argument_name = arg_name;
	argument_value->type_name = arg_type;
	argument_value->type_id = php_w32api_get_type_id_from_name(arg_type);

	if(argument_value->type_id == W32API_UNKNOWN) {
		TSRMLS_FETCH();
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Unknown type %s used as arugment type", arg_type);
	}

	return return_value;
}
/* }}} */	

/* {{{ w32api_parser_join_arguments
 * Helper function for the parser
 */
arguments *w32api_parser_join_arguments(arguments *lval, arguments *rval)
{
	lval->next_arg = rval;
	rval->prev_arg = lval;

	return lval;
}
/* }}} */	

/* {{{ w32api_function_definition_error
 * Error function for the parser
 */
int w32api_function_definition_error(char *s)
{
	TSRMLS_FETCH();
	php_error_docref(NULL TSRMLS_CC, E_ERROR, "Function Definition Parse Error: %s", s);
	return 0;
}
/* }}} */	

/* -------------------------
 * Type Definition Functions
 * -------------------------
 */
w32api_type_handle *w32api_parser_register_type(char *type_name, members *member_list)
{
	w32api_type_handle *return_value = NULL;
	members *current_member;
	int offset = 0;
	int member_count = 0;

	return_value = emalloc(sizeof(w32api_type_handle));

	return_value->member_list = member_list;
	return_value->type_name = type_name;										/* estrdup'd in parser */
	return_value->member_count = 0;
	
	current_member = return_value->member_list;


	while(current_member != NULL)
	{
		return_value->member_count++;
		current_member->member->offset = offset;
		offset += php_w32api_get_type_size(current_member->member->member_type_id, current_member->member->member_type_name, current_member->member->flags);
		current_member = current_member->next_member;
	}


	return_value->size = offset;

	return return_value;


}

members *w32api_parser_type_make_value(char *type_name, char *member_name, long flags)
{
	members *return_value = NULL;												/* Pointer to our return value */
	member  *member_value = NULL;												/* Our actual member */

	member_value = emalloc(sizeof(member));
	return_value = emalloc(sizeof(members));

	return_value->member = member_value;
	return_value->next_member = return_value->prev_member = NULL;

	member_value->member_name = member_name;									/* estrdup'd in parser */
	member_value->member_type_name = type_name;									/* estrdup'd in parser */
	member_value->member_type_id = php_w32api_get_type_id_from_name(type_name);
	member_value->flags = flags;

	return return_value;
}


members *w32api_parser_type_join_values(members *lval, members *rval)
{
	lval->next_member = rval;
	rval->prev_member = lval;

	return lval;
}

/* {{{ w32api_function_definition_error
 * Error function for the parser
 */
int w32api_type_definition_error(char *s)
{
	TSRMLS_FETCH();
	php_error_docref(NULL TSRMLS_CC, E_ERROR, "Type Definition Parse Error: %s", s);
	return 0;
}
/* }}} */	


#endif /* HAVE_W32API */