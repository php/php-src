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
   | Author: David Eriksson <david@2good.com>                            |
   +----------------------------------------------------------------------+
 */

/*
 * $Id$
 * vim: syntax=c tabstop=2 shiftwidth=2
 */

/* -----------------------------------------------------------------------
 * 
 * Macros and things to simplify making PHP classes
 *
 * -----------------------------------------------------------------------
 */
#ifndef __orbit_class_h__
#define __orbit_class_h__

#include <zend.h>
#include <zend_API.h>

#define CONSTRUCTOR		1
#define DESTRUCTOR		2
#define CALL_FUNCTION	4
#define PUT_PROPERTY	8
#define GET_PROPERTY	16
#define NO_FUNCTIONS		(CONSTRUCTOR|DESTRUCTOR|PUT_PROPERTY|GET_PROPERTY)
#define NO_PROPERTIES		(CONSTRUCTOR|DESTRUCTOR|CALL_FUNCTION)

/*
 * sorry everyone but the constructor itself has to allocate the data
 * structure for member variables!
 *
 * it also has to deallocate this in the destructor...
 */
typedef zend_bool (*Class_Constructor)
	(void ** ppObject, int parameterCount, zval ** ppParameters);
typedef zend_bool (*Class_Destructor)
	(void * pObject);
typedef zend_bool (*Class_CallFunction)
	(void * pObject, const char * pFunctionName, int parameterCount, zval ** ppParameters, zval * pReturnValue);
typedef zend_bool (*Class_PutProperty)
	(void * pObject, const char * pPropertyName, const zval * pValue);
typedef zend_bool (*Class_GetProperty)
	(void * pObject, const char * pPropertyName, zval * pReturnValue);

	/* put/get data connected to php object */
void orbit_save_data(zval * pPhpObject, int type, void * pData);
void * orbit_retrieve_data(const zval * pPhpObject, int type);

void orbit_class_function_call(
		zend_class_entry * pClass,
		int dataType,
		zend_property_reference *pPropertyReference, 
		Class_Constructor pConstructor, 
		Class_CallFunction pCallFunction,
		INTERNAL_FUNCTION_PARAMETERS);

/*
 * use this macro in the header file
 */
#define DECLARE_CLASS(name) \
typedef struct _##name  name##;	\
zend_bool name##_Init(int module_number);	\
void name##_SaveData(zval * pPhpObject, ##name * pData);\
##name * name##_RetrieveData(const zval * pPhpObject);\
zend_bool name##_PutProperty(##name * pObject, const char * pPropertyName, const zval * pValue);\
zend_bool name##_GetProperty(##name * pObject, const char * pPropertyName, zval * pReturnValue);\
/* end small macro */

/*
 * Wrapper for a function call
 */
#define IMPLEMENT_FUNCTION_CALL(name, flags)\
static void _##name##_FunctionCall(INTERNAL_FUNCTION_PARAMETERS, zend_property_reference * pPropertyReference)	\
{	\
	orbit_class_function_call(	\
			&name##_class_entry,	\
			name##_data_type,	\
			pPropertyReference,	\
			((flags) & CONSTRUCTOR) ? 	(Class_Constructor)name##_Constructor : NULL,	\
			((flags) & CALL_FUNCTION) ? (Class_CallFunction)name##_CallFunction : NULL,	\
			INTERNAL_FUNCTION_PARAM_PASSTHRU\
			);\
}

/*
 * wrapper for PutProperty
 */
#define IMPLEMENT_PUT_PROPERTY(name, flags)\
static int  _##name##_PutProperty(zend_property_reference * pPropertyReference, zval * pValue)\
{\
	int result = 0;\
	if ((flags) & PUT_PROPERTY)\
	{\
		##name * p_data = (##name *)orbit_retrieve_data(pPropertyReference->object, name##_data_type);\
		/* get variable name element */\
		zend_overloaded_element * p_attribute_element = \
			(zend_overloaded_element *)pPropertyReference->elements_list->tail->data;\
		/* get variable name */\
		char * p_attribute_name = p_attribute_element->element.value.str.val;\
		if (p_data)\
			result = ##name##_PutProperty(p_data, p_attribute_name, pValue) ? SUCCESS : FAILURE;\
		else\
			result = FAILURE;\
	}\
	else\
	{\
		zend_error(E_WARNING, "(Satellite) Can't set members in class");\
	}\
	return result;\
}

/*
 * wrapper for GetProperty
 */
#define IMPLEMENT_GET_PROPERTY(name, flags)\
static zval _##name##_GetProperty(zend_property_reference * pPropertyReference)\
{\
	zval value;\
	ZVAL_NULL(&value);\
	if ((flags) & GET_PROPERTY)\
	{\
		##name * p_data = (##name *)orbit_retrieve_data(pPropertyReference->object, name##_data_type);\
		/* get variable name element */\
		zend_overloaded_element * p_attribute_element = \
			(zend_overloaded_element *)pPropertyReference->elements_list->tail->data;\
		/* get variable name */\
		char * p_attribute_name = p_attribute_element->element.value.str.val;\
		if (p_data)\
			##name##_GetProperty(p_data, p_attribute_name, &value);\
	}\
	else\
	{\
		zend_error(E_WARNING, "(Satellite) Can't get members in class");\
	}\
	return value;\
}


#define IMPLEMENT_INIT_EX(name, flags, functions, functioncall, getproperty, putproperty)\
zend_bool name##_Init(int module_number)	\
{	\
	/* register data type */	\
	name##_data_type = register_list_destructors(name##_Destructor, NULL);	\
\
	/* register class */	\
	INIT_OVERLOADED_CLASS_ENTRY(	\
		name##_class_entry,			\
		#name,									\
		functions, 									\
		functioncall,	\
		getproperty, 	\
		putproperty		\
		);	\
\
	zend_register_internal_class(&name##_class_entry TSRMLS_CC);\
	return TRUE;\
}

/*
 * initialize object, must be called from PHP_MINIT_FUNCTION etc
 */
#define IMPLEMENT_INIT(name, flags) \
IMPLEMENT_INIT_EX(name, flags, NULL, _##name##_FunctionCall, _##name##_GetProperty, _##name##_PutProperty)


/* 
 * functions to save and retrieve data for the object
 */
#define IMPLEMENT_DATA_HELPERS(name, flags)\
void name##_SaveData(zval * pPhpObject, ##name * pData)\
{\
	orbit_save_data(pPhpObject, name##_data_type, pData);\
}\
##name * name##_RetrieveData(const zval * pPhpObject)\
{\
	return (##name *)orbit_retrieve_data(pPhpObject, name##_data_type);\
}

/*
 * static declarations for class
 */
#define IMPLEMENT_DECLARATIONS(name, flags)\
static zend_bool name##_Constructor(##name ** ppObject, int parameterCount, const zval ** ppParameters);\
static zend_bool name##_Destructor(##name * pObject);\
static zend_bool name##_CallFunction(##name * pObject, const char * pFunctionName, int parameterCount, const zval ** ppParameters, zval * pResult); \
\
static int name##_data_type = 0;	\
static zend_class_entry name##_class_entry;

/*
 * use this macro in the source file
 */
#define IMPLEMENT_CLASS(name, flags)	\
IMPLEMENT_DECLARATIONS(name, flags)		\
IMPLEMENT_FUNCTION_CALL(name, flags)	\
IMPLEMENT_PUT_PROPERTY(name, flags)		\
IMPLEMENT_GET_PROPERTY(name, flags)		\
IMPLEMENT_INIT(name, flags)						\
IMPLEMENT_DATA_HELPERS(name, flags)	


#endif /* __orbit_class_h__ */

