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

#if		HAVE_W32API

#ifndef PHP_W32API_H
#define PHP_W32API_H

/* ================================================================================================
 * Type Definitions
 * ================================================================================================
 */
typedef struct _w32api_lib_handle									w32api_lib_handle;
typedef struct _w32api_func_handle									w32api_func_handle;
typedef struct _w32api_type_handle									w32api_type_handle; 
typedef struct _w32api_type_instance								w32api_type_instance;
typedef struct _arguments											arguments;
typedef struct _argument											argument;
typedef union  _w32api_parser_function_definition_union				w32api_parser_function_definition_union; 
typedef struct _w32api_func_handle_ptr								w32api_func_handle_ptr;
typedef struct _w32api_type_handle_ptr								w32api_type_handle_ptr;
typedef union  _w32api_parser_type_definition_union					w32api_parser_type_definition_union;
typedef struct _member												member;
typedef struct _members												members;
typedef union  _w32api_result										w32api_result;
typedef struct _w32api_dynamic_param								w32api_dynamic_param; 

struct _w32api_lib_handle 
{
	HINSTANCE					handle;								/* Handle for our library */
	char						*library_name;						/* name of our library */
	int							ref_count;							/* reference counter */
};

struct _w32api_func_handle_ptr										/* Temporary structure */
{																	/* To work around problems */
	w32api_func_handle			*hnd;								/* at 3am.. Ill sort it out */
};																	/* When I can think straight */
																	
struct _w32api_type_handle_ptr										/* Ditto.. should really combine */
{																	/* These two into a union at least */
	w32api_type_handle			*hnd;
};

struct _w32api_type_handle 
{
	char						*type_name;							/* Name of our type */
	long						size;								/* Size of type */
	members						*member_list;						/* Pointer List of members */
	long						member_count;						/* Number of members */
};

struct _w32api_type_instance
{
	w32api_type_handle			*type;								/* pointer to w32api_type_handle */
	zval						**values;							/* First element of an array of ptr's to zvals */
};

struct _w32api_func_handle 
{
	FARPROC						handle;								/* Handle for our function */
	w32api_lib_handle			*lib;								/* Pointer to the library handle */
	char						*function_name;						/* Name of our function (Alias is store if supplied) */
	long						return_type_id;						/* ID of return type */
	char						*return_type_name;					/* Name of return type (if W32API_COMPLEX) */
	long						flags;								/* Flags for function */
	arguments					*argument_list;						/* Pointer List of arguments */
};

struct _arguments
{
	argument					*arg;								/* Current Argument */
	arguments					*next_arg;							/* Next Arugment */
	arguments					*prev_arg;							/* Previous Argument */
};

struct _argument
{
	long						type_id;							/* ID of the return type */
	char						*type_name;							/* Name of type (if W32API_COMPLEX) */
	char						*argument_name;						/* Name of argument, currently not used */
	long						flags;								/* Currently used for byref/byval */
};

struct _member
{
	char						*member_name;
	long						member_type_id;
	char						*member_type_name;
	long						flags;
	long						offset;
};

struct _members
{
	member						*member;
	members						*next_member;
	members						*prev_member;
};

union _w32api_result
{
	int							ival;							
    unsigned long				lval;			        
	DWORD						dwval;
    void						*ptr;							
    float						fval;				            
    double						dval;						    
    __int64						i64val;		  			        
};

struct _w32api_dynamic_param 
{
	long						flags;
	int							width;
	union {
		unsigned long			argument;				
		void					*argument_ptr;					
	};
};


union _w32api_parser_function_definition_union 
{
	char *s;
	arguments *arg;
};

union _w32api_parser_type_definition_union 
{
	char *s;
	members *type;
};

/* ================================================================================================
 * Constants
 * ================================================================================================
 */

/* Recognised Base types */
#define W32API_UNKNOWN									-1
#define W32API_NULL										1
#define W32API_INT										2
#define W32API_LONG										3
#define W32API_DOUBLE									4
#define W32API_FLOAT									5
#define W32API_STRING									6
#define W32API_BYTE										7
#define W32API_BOOL										8
#define W32API_COMPLEX									9

/* Function Flags */
#define W32API_ARGPTR									(1<<0)
#define W32API_BORLAND									(1<<1)
#define W32API_CDECL									(1<<2)
#define W32API_REAL4									(1<<3)
#define W32API_REAL8									(1<<4)

/* ================================================================================================
 * Utility Macros
 * ================================================================================================
 */
#define PROP_SET_ARGS									zend_property_reference *property_reference, pval *value
#define PROP_GET_ARGS									zend_property_reference *property_reference

#define W32API_PROP_SET_FUNCTION_N(class_name)			w32api_set_property_handler_##class_name
#define W32API_PROP_GET_FUNCTION_N(class_name)			w32api_get_property_handler_##class_name
#define W32API_CALL_FUNCTION_N(class_name)				w32api_call_handler_##class_name

#define W32API_PROP_SET_FUNCTION(class_name)			PHP_W32API_API int W32API_PROP_SET_FUNCTION_N(class_name)(PROP_SET_ARGS)
#define W32API_PROP_GET_FUNCTION(class_name)			PHP_W32API_API zval W32API_PROP_GET_FUNCTION_N(class_name)(PROP_GET_ARGS)
#define W32API_CALL_FUNCITON(class_name)				PHP_W32API_API void W32API_CALL_FUNCTION_N(class_name)(INTERNAL_FUNCTION_PARAMETERS, zend_property_reference *property_reference)

#define W32API_CLASS_FUNCTION(class_name, function_name) PHP_FUNCTION(##class_name##function_name)
#define W32API_CLASS_FN(class_name, function_name) PHP_FN(##class_name##function_name)
#define W32API_CLASS_FE(class_name, function_name, function_args) {#function_name, W32API_CLASS_FN(class_name, function_name), function_args},

/* ================================================================================================
 * Module exports, Global Variables and General Definitions
 * ================================================================================================
 */
extern zend_module_entry w32api_module_entry;
#define phpext_w32api_ptr &w32api_module_entry;

#define PHP_W32API_API __declspec(dllexport)

#ifdef ZTS
#include "TSRM.h"
#endif /* ZTS */

ZEND_BEGIN_MODULE_GLOBALS(w32api)
	zend_class_entry	*win32_ce;				/* The class entry for our win32 class */
	zend_class_entry	*type_ce;				/* The class entry for our type class */
	HashTable			*funcs;					/* Functions we registered */
	HashTable			*libraries;				/* Libraries we load using LoadLibrary */
	HashTable			*types;					/* Types we have registed with us */
	HashTable			*callbacks;				/* Callbacks we have registered with us */
	long				le_type_instance;		/* Resource hanlde for runtime instances */
ZEND_END_MODULE_GLOBALS(w32api)

ZEND_DECLARE_MODULE_GLOBALS(w32api)

#ifdef ZTS
#define WG(v) TSRMG(w32api_globals_id, zend_w32api_globals *, v)
#else
#define WG(v) (w32api_globals.v)
#endif


/* ================================================================================================
 * Startup, Shutdown and Info Functions
 * ================================================================================================
 */
PHP_MINIT_FUNCTION(w32api);
PHP_MSHUTDOWN_FUNCTION(w32api);
PHP_RINIT_FUNCTION(w32api);
PHP_RSHUTDOWN_FUNCTION(w32api);
PHP_MINFO_FUNCTION(w32api);
static void php_w32api_init_globals(zend_w32api_globals *w32api_globals);
static void php_w32api_hash_lib_dtor(void *data);
static void php_w32api_hash_func_dtor(void *data);
static void php_w32api_hash_callback_dtor(void *data);
static void php_w32api_hash_type_dtor(void *data);
static void w32api_type_instance_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC);

/* ================================================================================================
 * Win32 Class Functions
 * ================================================================================================
 */
int win32_class_init(TSRMLS_D);
int win32_class_rshutdown(TSRMLS_D);
W32API_CLASS_FUNCTION(win32, registerfunction);
W32API_CLASS_FUNCTION(win32, unregisterfunction);
W32API_CLASS_FUNCTION(win32, registercallback);
W32API_CLASS_FUNCTION(win32, definetype);
W32API_CLASS_FUNCTION(win32, gettypesize);
W32API_CLASS_FUNCTION(win32, inittype);
W32API_CLASS_FUNCTION(win32, decref);
W32API_CLASS_FUNCTION(win32, invokefunction);			


/* ================================================================================================
 * Type Class Functions
 * ================================================================================================
 */
int type_class_init(TSRMLS_DC);

W32API_CLASS_FUNCTION(type, clone);
W32API_PROP_SET_FUNCTION(type);
W32API_PROP_GET_FUNCTION(type);

/* ================================================================================================
 * Utility Functions
 * ================================================================================================
 */

static int php_w32api_load_function (char *definition, int definition_len, int flags TSRMLS_DC);
static void php_w32api_unload_function (w32api_func_handle **fh TSRMLS_DC);

static int php_w32api_load_library (char *library_name, w32api_lib_handle **lh TSRMLS_DC);
static void php_w32api_unload_library (w32api_lib_handle *lh, int flags TSRMLS_DC);

static int php_w32api_register_callback(char *function_definition, int function_definition_len TSRMLS_DC);

static void php_w32api_free_arguments(arguments *argument_list);
static void php_w32api_free_members(members *member_list);

static int php_w32api_get_type_id_from_name(char *type);
static unsigned char *php_w32api_do_arg_types(arguments **arguments);

void php_w32api_marshall_zval_to_c(argument *arg, w32api_dynamic_param *dp, zval *pzval TSRMLS_DC);

static void php_w32api_init_type(w32api_type_handle *th, zval *obj TSRMLS_DC);
static int php_w32api_do_prop_get(zval *object, zval *return_value, zend_llist_element **element TSRMLS_DC);
static int php_w32api_do_prop_set(zval *object, zval *value, zend_llist_element **element TSRMLS_DC);
static void *php_w32api_complex_marshall_zval_to_c(zval *pzval, int *width, void *data TSRMLS_DC);

/* ================================================================================================
 * Parser & Scanner Functions
 * ================================================================================================
 */
#define w32api_parser_load_alias_function w32api_parser_load_function_ex
#define w32api_parser_load_function(return_type, function_name, arguments, library_name) w32api_parser_load_function_ex (return_type, function_name, NULL, arguments, library_name)
#define w32api_parser_make_argument_byref(arg_type, arg_name) w32api_parser_make_argument(arg_type, arg_name, BYREF_FORCE)
#define w32api_parser_make_argument_byval(arg_type, arg_name) w32api_parser_make_argument(arg_type, arg_name, BYREF_NONE)

#define w32api_parser_type_make_byref_value(member_name, member_type) w32api_parser_type_make_value(member_name, member_type, BYREF_FORCE)
#define w32api_parser_type_make_byval_value(member_name, member_type) w32api_parser_type_make_value(member_name, member_type, BYREF_NONE)

w32api_func_handle *w32api_parser_load_function_ex(char *return_type, char *function_name, char *alias_name, arguments *argument_list, char *library_name);
arguments *w32api_parser_make_argument(char *arg_type, char *arg_name, int byref);
arguments *w32api_parser_join_arguments(arguments *lval, arguments *rval);
int w32api_function_definition_error(char *s);

w32api_type_handle *w32api_parser_register_type(char *type_name, members *member_list);
members *w32api_parser_type_make_value(char *member_name, char *type_name, long flags);
members *w32api_parser_type_join_values(members *lval, members *rval);
int w32api_type_definition_error(char *s);

struct yy_buffer_state *w32api_function_definition_scan_bytes(char *bytes, int len);	/* Definied in w32api_function_definition_scanner.c */
int w32api_function_definition_parse(void *fh);											/* Definied in w32api_function_definition_parser.c */

struct yy_buffer_state *w32api_type_definition_scan_bytes(char *bytes, int len);		/* Definied in w32api_type_definition_scanner.c */
int w32api_type_definition_parse(void *th);												/* Definied in w32api_type_definition_parser.c */


/* ================================================================================================
 * Various Debugging Functions
 * ================================================================================================
 */
#ifndef NDEBUG
W32API_CLASS_FUNCTION(win32, dump_library_hash);
W32API_CLASS_FUNCTION(win32, dump_function_hash);
W32API_CLASS_FUNCTION(win32, dump_callback_hash);
W32API_CLASS_FUNCTION(win32, dump_type_hash);

int php_w32api_dump_library_hash_cb(void *pData TSRMLS_DC);
int php_w32api_dump_function_hash_cb(void *pData TSRMLS_DC);
int php_w32api_dump_callback_hash_cb(void *pData TSRMLS_DC);
int php_w32api_dump_type_hash_cb(void *pData TSRMLS_DC);

void php_w32api_print_arguments(arguments *argument_list);
void php_w32api_print_members(members *member_list);

#endif  /* ifndef	NDEBUG */ 

#endif	/* ifndef	PHP_W32API_H */
#endif  /* if		HAVE_W32API  */