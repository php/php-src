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
   | Author: David Eriksson <david@2good.com>                            |
   +----------------------------------------------------------------------+
 */

/*
 * $Id$
 * vim: syntax=c tabstop=2 shiftwidth=2
 */

/* -----------------------------------------------------------------------
 *
 * OrbitObject class
 *
 * There are three ways to create an object
 *
 * (1) OrbitObject_Constructor, for new OrbitObject(...) in PHP
 * (2) OrbitObject_Create, used when a CORBA method returns an object
 * (3) OrbitObject_Wakeup, used on "unserialization"
 *
 * -----------------------------------------------------------------------
 */
#include <php.h>
#include "php_orbit.h"

#include "class.h"
#include "corba.h"
#include "common.h"
#include "object.h"
#include "typemanager.h"
#include "namedvalue_to_zval.h"
#include "zval_to_namedvalue.h"
#include "typecode.h" /* for satellite_release_typecode */

#define PROFILE	0

#if PROFILE
#include <sys/time.h>
#include <unistd.h>
#endif

struct _OrbitObject
{
	CORBA_Object mCorbaObject;
	InterfaceType * mpInterface;
};

static void OrbitObject_Wakeup(INTERNAL_FUNCTION_PARAMETERS);
static void OrbitObject_Sleep(INTERNAL_FUNCTION_PARAMETERS);

static zend_function_entry OrbitObject_class_functions[] =
{
	{"__sleep", OrbitObject_Sleep},
	{"__wakeup", OrbitObject_Wakeup},
	{NULL, NULL}
};

#define MY_IMPLEMENT_CLASS(name, flags)\
IMPLEMENT_DECLARATIONS(name, flags)		\
IMPLEMENT_FUNCTION_CALL(name, flags)	\
IMPLEMENT_PUT_PROPERTY(name, flags)		\
IMPLEMENT_GET_PROPERTY(name, flags)		\
IMPLEMENT_INIT_EX(name, flags, ##name##_class_functions, _##name##_FunctionCall, _##name##_GetProperty, _##name##_PutProperty)\
IMPLEMENT_DATA_HELPERS(name, flags)
	
MY_IMPLEMENT_CLASS(OrbitObject, ~0);

CORBA_Object OrbitObject_GetCorbaObject(OrbitObject * pObject)
{
	if (pObject == NULL)
		return FALSE;
	else
		return pObject->mCorbaObject;
}
#define IOR_PROPERTY_KEY		"IOR"

/*
 * prepare for serialization
 *
 * this means creating a property containing the object IOR and returning an 
 * array containing the names of properties we want serialized
 *
 * TODO: save environment as property?!
 * 
 */
static void OrbitObject_Sleep(INTERNAL_FUNCTION_PARAMETERS)
{
	CORBA_char * p_ior = NULL;
	/* get object data */
	OrbitObject * p_object = OrbitObject_RetrieveData(this_ptr);
	
	/* validate data */
	if (p_object == NULL)
	{
		goto error;
	}

	/* get IOR from object */
	p_ior = CORBA_ORB_object_to_string(
			orbit_get_orb(), p_object->mCorbaObject, orbit_get_environment());
	if (p_ior == NULL || orbit_caught_exception())
		goto error;
	
	/* add property to zval */
	add_property_string(this_ptr, IOR_PROPERTY_KEY, p_ior, TRUE);
	
	/* create array */
	array_init(return_value);

	/* add name of property IOR to array */
	add_next_index_string(return_value, IOR_PROPERTY_KEY, TRUE);

	return;

error:
	RETURN_NULL();
}

/*
 * initialize OrbitObject structure
 */
static zend_bool OrbitObject_InitializeData(OrbitObject * pObject, const char * pIor)
{
	/* get object */
	pObject->mCorbaObject = CORBA_ORB_string_to_object(
			orbit_get_orb(), 
			pIor, 
			orbit_get_environment());

	if (pObject->mCorbaObject == NULL || orbit_caught_exception())
	{
		zend_error(E_WARNING, "(Satellite) Unable to resolve IOR");
		goto error;
	}

	/* find type info */
	pObject->mpInterface = 
		TypeManager_FindInterface(pObject->mCorbaOZ_TYPE_P(bject)_id);

	if (pObject->mpInterface == NULL)
	{
		zend_error(E_WARNING, "(Satellite) unknown interface '%s'",
				pObject->mCorbaOZ_TYPE_P(bject)_id);
		/* TODO: set exception */
		goto error;
	}

	return TRUE;

error:
	return FALSE;
}

/*
 * recover from serialization
 *
 * this means reading the property IOR and reconstructing the object!
 *
 */
static void OrbitObject_Wakeup(INTERNAL_FUNCTION_PARAMETERS)
{
	zval ** pp_ior = NULL;
	
	/* create data object */
	OrbitObject * p_object = orbit_new(OrbitObject);

	/* find IOR property */
	if (zend_hash_find(
				Z_OBJPROP_P(this_ptr), 
				IOR_PROPERTY_KEY, 
				sizeof(IOR_PROPERTY_KEY), 
				(void**)&pp_ior) != SUCCESS)
	{
		/* TODO: set exception */
		goto error;
	}

	if (Z_TYPE_PP(pp_ior) != IS_STRING)
	{
		/* TODO: set exception */
		goto error;
	}

	/* initialize data */
	if (!OrbitObject_InitializeData(
				p_object, 
				Z_STRVAL_PP(pp_ior)))
	{
		goto error;
	}

	/* save data to zval */
	OrbitObject_SaveData(this_ptr, p_object);

	return;

error:
	zend_error(E_WARNING, "(Satellite) Invalid serialized object");
	OrbitObject_Destructor(p_object);
}


/* instansiate a class */
zend_bool OrbitObject_Create(CORBA_Object source, zval * pDestination)
{
	OrbitObject * p_object = NULL;
	
	/* source might be NULL */
	if (source == NULL)
	{
		ZVAL_NULL(pDestination);
		return TRUE;
	}
	
	/* allocate object */
	p_object = orbit_new(OrbitObject);

	/* save corba object */
	p_object->mCorbaObject = source;
	
	/* find type info */
	p_object->mpInterface = 
		TypeManager_FindInterface(p_object->mCorbaOZ_TYPE_P(bject)_id);

	if (p_object->mpInterface == NULL)
	{
		zend_error(E_WARNING, "(Satellite) unknown interface '%s'",
				p_object->mCorbaOZ_TYPE_P(bject)_id);
		goto error;
	}

	object_init_ex(pDestination, &OrbitObject_class_entry);

	/* save orbit data */
	OrbitObject_SaveData(pDestination, p_object);

	return TRUE;
	
error:
	OrbitObject_Destructor(p_object);
	return FALSE;
}



/*
 * constructor
 *
 * parameters: IOR, OrbitEnvironment
 */
zend_bool  OrbitObject_Constructor(OrbitObject  ** ppObject, 
		int parameterCount, const zval ** ppParameters)
{
	OrbitObject * p_object = NULL;
	
	/* allocate object */
	p_object = orbit_new(OrbitObject);

	/* check parameter count */
	if (parameterCount != 1)
	{
		zend_wrong_param_count(TSRMLS_C);
		goto error;
	}

	/* validate parameter types */
	if (ppParameters[0]->type != IS_STRING)
	{
		zend_error(E_WARNING, "(Satellite) IOR is not a string");
		goto error;
	}

	/* initialize data object */
	if ( !OrbitObject_InitializeData(
			p_object,
			ppParameters[0]->value.str.val) )
	{
		goto error;
	}
	
	*ppObject = p_object;
	return TRUE;

error:
	OrbitObject_Destructor(p_object);
	*ppObject = NULL;

	/* TODO: all errors above should set exceptions! */

	return FALSE;
}

/*
 * destructor
 */
zend_bool  OrbitObject_Destructor(OrbitObject  * pObject)
{
	InterfaceType_release(pObject->mpInterface);
	
	if (pObject->mCorbaObject != NULL)
		CORBA_Object_release((CORBA_Object)pObject->mCorbaObject, NULL);

	orbit_delete(pObject);

	return TRUE;
}

/* 
 * prepare a function call result 
 */
static CORBA_NamedValue * OrbitObject_PrepareResult(OrbitObject * pObject, 
		OperationType * pOperation)
{
	CORBA_NamedValue * p_result = orbit_new(CORBA_NamedValue);

	p_result->argument._type = OperationType_GetReturnType(pOperation);

	if (p_result->argument._type == NULL)
	{
		orbit_delete(p_result);
		p_result = NULL;
	}
	else
	{
		orbit_zval_to_namedvalue(NULL, p_result);
	}
	
	return p_result;
}

/*
 * add an argument to a function call
 */
static zend_bool OrbitObject_AddSingleArgument(OrbitObject * pObject, 
		CORBA_Request request, ParameterType * pParameter, const zval * pSource, CORBA_NamedValue * pDestination)
{
	pDestination->argument._type = ParameterType_GetType(pParameter);
	pDestination->arg_modes 		 = ParameterType_GetPassingMode(pParameter);

	/* if the argument is output only, don't care about input value */
	if (pDestination->arg_modes == CORBA_ARG_OUT)
		pSource = NULL;

	if (!orbit_zval_to_namedvalue(pSource, pDestination))
		return FALSE;

	/* add parameter to request */
	CORBA_Request_add_arg(
		request,											/* request  */
		NULL,													/* arg_name */
		pDestination->argument._type,		/* type     */
		pDestination->argument._value,		/* value    */
		pDestination->len,								/* length   */
		pDestination->arg_modes,					/* flags    */
		orbit_get_environment()
	);

	if (orbit_caught_exception())
		return FALSE;

	return TRUE;
}

/*
 * add the function call arguments
 */
static zend_bool OrbitObject_AddArguments(OrbitObject * pObject, 
		CORBA_Request request, OperationType * pOperation, int argumentCount, 
		const zval ** ppArguments, CORBA_NamedValue ** ppNamedValue)
{
	ParameterType * p_parameter = NULL;
	int i = 0;
	zend_bool success = FALSE;

	if (argumentCount < 1)
		return TRUE;	/* nothing to do */

	p_parameter = OperationType_GetFirstParameter(pOperation);

	if (NULL == p_parameter)
		return FALSE; /* oups! */
	
	do
	{
		ppNamedValue[i] = satellite_new(CORBA_NamedValue);
		success = OrbitObject_AddSingleArgument( pObject, 
				request, p_parameter, ppArguments[i], ppNamedValue[i]);

		if (!success)
			goto error;

		i++;
	} while (i < argumentCount && ParameterType_GetNext(p_parameter));

	/* i should equal argument count and there should be no more parameters */
	if (i != argumentCount || ParameterType_GetNext(p_parameter))
	{
/*		printf("%i, %i, %i\n", i, argumentCount, ParameterType_IsValid(p_parameter));*/
		
		/* bad number of arguments */
		zend_wrong_param_count(TSRMLS_C);
		goto error;
	}

	success = TRUE;
	goto exit;

error:
	success = FALSE;

exit:
	orbit_delete(p_parameter);
	return success;

}

/*
 * release a namedvalue that we have allocated
 *
 * (move to another file?)
 */
static void satellite_release_namedvalue(CORBA_NamedValue * pNamedValue)
{
	if (pNamedValue == NULL)
		return;
	
	/* clear value */
	if (pNamedValue->argument._value != NULL)
	{
		/* allocated with ORBit_alloc_tcval */
		CORBA_free(pNamedValue->argument._value);
	}

	/* clear typecode */
	satellite_release_typecode(pNamedValue->argument._type);

	/* allocated with satellite_new */
	satellite_delete(pNamedValue);
}

static void satellite_release_namedvalue_list(
		CORBA_NamedValue ** ppNamedValue, int length)
{
	int i;
	
	if (ppNamedValue == NULL)
		return;
	
	for (i = 0; i < length; i++)
	{
		/* release named values */
		satellite_release_namedvalue(ppNamedValue[i]);
		ppNamedValue[i] = NULL;
	}

	satellite_delete(ppNamedValue);
}

/*
 * post-process arguments (take care of output parameters, release memory)
 */
static zend_bool OrbitObject_ReleaseArguments(OrbitObject * pObject, 
		CORBA_Request request, OperationType * pOperation, int argumentCount, 
		const zval ** ppArguments, CORBA_NamedValue ** ppNamedValue)
{
	/* TODO: handle output parameters */

	return TRUE;
}



/*
 * call a function
 */
zend_bool  OrbitObject_CallFunction(OrbitObject  * pObject, 
		const char * pFunctionName, int parameterCount, const zval ** ppParameters, zval * pReturnValue)
{
	zend_bool success;
	CORBA_Request request = NULL;
	OperationType * p_operation = NULL;
	CORBA_NamedValue ** pp_arguments = NULL;
	CORBA_NamedValue * p_result = NULL;
	CORBA_TypeCode * p_exception_list = NULL;
	
#if PROFILE
	int i;
	struct timeval tv[4];
	gettimeofday(&tv[0], NULL);
#endif
	
	/* clear exception */
	CORBA_exception_free(orbit_get_environment());
	
	p_operation = InterfaceType_FindOperation(pObject->mpInterface, pFunctionName);

	if (p_operation == NULL)
	{
		/* no such operation */
		zend_error(E_WARNING, "(Satellite) unknown operation name '%s' in interface '%s'",
				pFunctionName, pObject->mCorbaOZ_TYPE_P(bject)_id);
		goto error;
	}

	p_exception_list = OperationType_GetExceptionList(p_operation);
	
	/* XXX: it's ok if this returns NULL, because the operation may be void! */
	p_result = OrbitObject_PrepareResult(pObject, p_operation);

	/* create the request */
	CORBA_Object_create_request2(
		pObject->mCorbaObject,									/* object      */
		NULL,																		/* context     */
		OperationType_GetName(p_operation),			/* name        */
		NULL,																		/* arg_list    */
		p_result,																/* result      */
		p_exception_list,												/* user exception list */
		&request,																/* request     */
		CORBA_OUT_LIST_MEMORY,									/* flags       */
		orbit_get_environment()									/* environment */
	);

	/* check for exception */
	if (orbit_caught_exception())
		goto error;

	/* add parameters */
	pp_arguments = satellite_new_n(CORBA_NamedValue*, parameterCount);
	success = OrbitObject_AddArguments(pObject, 
			request, p_operation, parameterCount, ppParameters, pp_arguments);

	if (!success)
	{
		/* bad arguments */
		goto error;
	}

#if PROFILE
	gettimeofday(&tv[1], NULL);
#endif

	/* send request and get response */
	CORBA_Request_invoke(request, 0, orbit_get_environment());

#if PROFILE
	gettimeofday(&tv[2], NULL);
#endif

	if (orbit_caught_exception())
		goto error;

	/* release arguments */
	success = OrbitObject_ReleaseArguments(pObject, 
			request, p_operation, parameterCount, ppParameters, pp_arguments);
	if (!success)
		goto error;
	
	/* take care of return value */
	if (p_result != NULL)
	{
		orbit_namedvalue_to_zval(p_result, pReturnValue);
	}

#if PROFILE
	gettimeofday(&tv[3], NULL);
	printf("%s\n", OperationType_GetName(p_operation));
	for(i = 0; i < 4; i++)
		printf("%i:%i\n", tv[i].tv_sec, tv[i].tv_usec);
#endif

	success = TRUE;
	goto exit;
	
error:
	/* TODO: all errors above should set exceptions! */
	success = FALSE;

exit:
	CORBA_Object_release((CORBA_Object)request, orbit_get_environment());
	satellite_release_namedvalue(p_result);	
	satellite_release_namedvalue_list(pp_arguments, parameterCount);
	orbit_delete(p_operation);
	TypeCodeList_release(p_exception_list);
	
	return success;
}

#define GET_PREFIX "_get_"
#define SET_PREFIX "_set_"
#define GET_PREFIX_LENGTH (sizeof(GET_PREFIX)-1)
#define SET_PREFIX_LENGTH (sizeof(SET_PREFIX)-1)


/*
 * add an argument to a function call
 */
static zend_bool OrbitObject_AddAttributeArgument(OrbitObject * pObject, 
		CORBA_Request request, AttributeType * pAttribute, const zval * pSource)
{
	CORBA_NamedValue destination;
	memset(&destination, 0, sizeof(CORBA_NamedValue));
	
	destination.argument._type = AttributeType_GetType(pAttribute);
	destination.arg_modes 		 = CORBA_ARG_IN;

	if (!orbit_zval_to_namedvalue(pSource, &destination))
		return FALSE;

	/* add parameter to request */
	CORBA_Request_add_arg(
		request,											/* request  */
		NULL,													/* arg_name */
		destination.argument._type,		/* type     */
		destination.argument._value,		/* value    */
		destination.len,								/* length   */
		destination.arg_modes,					/* flags    */
		orbit_get_environment()
	);

	if (orbit_caught_exception())
		return FALSE;

	return TRUE;
}



/*
 * set a php property, or rather a corba attribute
 */
zend_bool  OrbitObject_PutProperty(OrbitObject  * pObject, 
		const char * pPropertyName, const zval * pValue)
{
	zend_bool success;
	char * p_name = NULL;
	CORBA_Request request = NULL;
	AttributeType * p_attribute = NULL;

	/* clear exception */
	CORBA_exception_free(orbit_get_environment());

	/* find attribute type */
	p_attribute = InterfaceType_FindAttribute(pObject->mpInterface, pPropertyName);

	if (p_attribute == NULL)
	{
		/*printf("InterfaceType_FindAttribute failed for property %s\n", pPropertyName);*/
		/* no such atttribute */
		zend_error(E_WARNING, "(Satellite) unknown attribute name '%s' in interface '%s'",
				pPropertyName, pObject->mCorbaOZ_TYPE_P(bject)_id);
		goto OrbitObject_PutProperty_error;
	}

	if (AttributeType_IsReadonly(p_attribute))
	{
		/* can't set a readonly attribute! */
		goto OrbitObject_PutProperty_error;
	}

	/* create operation name */
	p_name = orbit_new_n(char, strlen(pPropertyName) + SET_PREFIX_LENGTH + 1);
	strcpy(p_name, SET_PREFIX);
	strcat(p_name, AttributeType_GetName(p_attribute));

	/* create the request */
	CORBA_Object_create_request(
		pObject->mCorbaObject,									/* object      */
		NULL,																		/* context     */
		p_name,																	/* name        */
		NULL,																		/* arg_list    */
		NULL,																		/* result      */
		&request,																/* request     */
		CORBA_OUT_LIST_MEMORY,									/* flags       */
		orbit_get_environment()									/* environment */
	);

	/* check for exception */
	if (orbit_caught_exception())
		goto OrbitObject_PutProperty_error;

	if (request == NULL)
		goto OrbitObject_PutProperty_error;

	success = OrbitObject_AddAttributeArgument(
			pObject, request, p_attribute, pValue);

	if (!success)
		goto OrbitObject_PutProperty_error;

	/* send request and get response */
	CORBA_Request_invoke(request, 0, orbit_get_environment());

	if (orbit_caught_exception())
		goto OrbitObject_PutProperty_error;
	
	success = TRUE;
	goto OrbitObject_PutProperty_exit;
	
OrbitObject_PutProperty_error:
	/* TODO: all errors above should set exceptions! */
	success = FALSE;
	
OrbitObject_PutProperty_exit:
	CORBA_Object_release((CORBA_Object)request, orbit_get_environment());
	orbit_delete(p_attribute);
	orbit_delete(p_name);
	
	return success;
}

/* 
 * prepare a function call result 
 */
static CORBA_NamedValue * OrbitObject_PrepareAttributeResult(OrbitObject * pObject, 
		AttributeType * pAttribute)
{
	CORBA_NamedValue * p_result = orbit_new(CORBA_NamedValue);

	p_result->argument._type = AttributeType_GetType(pAttribute);

	if (p_result->argument._type == NULL)
	{
/*		printf("AttributeType_GetType failed for attribute %s\n", 
				AttributeType_GetName(pAttribute));*/
		orbit_delete(p_result);
		p_result = NULL;
	}
	else
	{
		orbit_zval_to_namedvalue(NULL, p_result);
	}
	
	return p_result;
}


/*
 * get a php property, equal to a corba attribute
 */
zend_bool  OrbitObject_GetProperty(OrbitObject  * pObject, 
		const char * pPropertyName, zval * pReturnValue)
{
	zend_bool success;
	char * p_name = NULL;
	CORBA_Request request = NULL;
	AttributeType * p_attribute = NULL;
	CORBA_NamedValue * p_result = NULL;

#if PROFILE
	int i;
	struct timeval tv[4];
	gettimeofday(&tv[0], NULL);
#endif
	
	/* clear exception */
	CORBA_exception_free(orbit_get_environment());

	/* find attribute type */
	p_attribute = InterfaceType_FindAttribute(pObject->mpInterface, pPropertyName);

	if (p_attribute == NULL)
	{
		/*printf("InterfaceType_FindAttribute failed for property %s\n", pPropertyName);*/
		/* no such atttribute */
		zend_error(E_WARNING, "(Satellite) unknown attribute name '%s' in interface '%s'",
				pPropertyName, pObject->mCorbaOZ_TYPE_P(bject)_id);
		goto OrbitObject_GetProperty_error;
	}

	/* prepare result */
	p_result = OrbitObject_PrepareAttributeResult(pObject, p_attribute);

	if (p_result == NULL)
	{
		/* probably bad return type */
		goto OrbitObject_GetProperty_error;
	}

	/* create operation name */
	p_name = orbit_new_n(char, strlen(pPropertyName) + GET_PREFIX_LENGTH + 1);
	strcpy(p_name, GET_PREFIX);
	strcat(p_name, AttributeType_GetName(p_attribute));

	/* create the request */
	CORBA_Object_create_request(
		pObject->mCorbaObject,									/* object      */
		NULL,																		/* context     */
		p_name,																	/* name        */
		NULL,																		/* arg_list    */
		p_result,																/* result      */
		&request,																/* request     */
		CORBA_OUT_LIST_MEMORY,									/* flags       */
		orbit_get_environment()									/* environment */
	);

	/* check for exception */
	if (orbit_caught_exception())
		goto OrbitObject_GetProperty_error;

	if (request == NULL)
		goto OrbitObject_GetProperty_error;

#if PROFILE
	gettimeofday(&tv[1], NULL);
#endif

	/* send request and get response */
	CORBA_Request_invoke(request, 0, orbit_get_environment());

#if PROFILE
	gettimeofday(&tv[2], NULL);
#endif
	
	if (orbit_caught_exception())
		goto OrbitObject_GetProperty_error;
	
	/* take care of return value */
	orbit_namedvalue_to_zval(p_result, pReturnValue);

#if 0 /* PROFILE */
	gettimeofday(&tv[3], NULL);
	printf("%s\n", p_name);
	for(i = 0; i < 4; i++)
		printf("%i:%i\n", tv[i].tv_sec, tv[i].tv_usec);
#endif
	
	success = TRUE;
	goto OrbitObject_GetProperty_exit;
	
OrbitObject_GetProperty_error:
	/* TODO: all errors above should set exceptions! */
	success = FALSE;
	
OrbitObject_GetProperty_exit:
	CORBA_Object_release((CORBA_Object)request, orbit_get_environment());
	orbit_delete(p_attribute);
	orbit_delete(p_name);
	satellite_release_namedvalue(p_result);
	
	return success;
}

PHP_FUNCTION(satellite_object_to_string)
{
	zval **arg = NULL;
	char * p_ior = NULL;
	OrbitObject * p_object = NULL;

	if (zend_get_parameters_ex(1, &arg) != SUCCESS)
	{
		zend_error(E_WARNING, "(Satellite) Bad parameter count");
		goto error;
	}
	
	p_object = OrbitObject_RetrieveData(*arg);
	if (!p_object)
	{
		zend_error(E_WARNING, "(Satellite) Object has no data");
		goto error;
	}

	if (!p_object->mCorbaObject)
	{
		zend_error(E_WARNING, "(Satellite) Object has no CORBA object");
		goto error;
	}

  p_ior = CORBA_ORB_object_to_string(orbit_get_orb(), p_object->mCorbaObject, 
			orbit_get_environment());

	if (!p_ior /*|| orbit_caught_exception()*/)
	{
		zend_error(E_WARNING, "(Satellite) CORBA call failed");
		goto error;
	}

	RETURN_STRING(p_ior, TRUE);

error:
	RETURN_NULL();
}

