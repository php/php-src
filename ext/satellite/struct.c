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
 * OrbitStruct class
 * 
 * There are three ways to create a structure
 *
 * (1) OrbitStruct_Constructor, for new OrbitStruct(...) in PHP
 * (2) OrbitStruct_Create, used when a CORBA method returns a struct
 * (3) OrbitStruct_Wakeup, used on "unserialization"
 * -----------------------------------------------------------------------
 */
#include "struct.h"
#include "common.h"
#include "typemanager.h"
#include "hashtable.h"

/*
 * enable/disable incomplete serialization support
 */
#define SERIALIZABLE_STRUCT		0

struct _OrbitStruct
{
	HashTable mMembers;
	zend_bool mIsException;	/* otherwise normal struct */
	union
	{
		StructType * mpStructType;
		ExceptionType * mpExceptionType;
	} u;
};

#if SERIALIZABLE_STRUCT

static void OrbitStruct_Wakeup(INTERNAL_FUNCTION_PARAMETERS);
static void OrbitStruct_Sleep(INTERNAL_FUNCTION_PARAMETERS);

static zend_function_entry OrbitStruct_class_functions[] =
{
	{"__sleep", OrbitStruct_Sleep},
	{"__wakeup", OrbitStruct_Wakeup},
	{NULL, NULL}
};

#define MY_IMPLEMENT_CLASS(name, flags)\
IMPLEMENT_DECLARATIONS(name, flags)		\
IMPLEMENT_FUNCTION_CALL(name, flags)	\
IMPLEMENT_PUT_PROPERTY(name, flags)		\
IMPLEMENT_GET_PROPERTY(name, flags)		\
IMPLEMENT_INIT_EX(name, flags, ##name##_class_functions, _##name##_FunctionCall, _##name##_GetProperty, _##name##_PutProperty)\
IMPLEMENT_DATA_HELPERS(name, flags)
	
MY_IMPLEMENT_CLASS(OrbitStruct, NO_FUNCTIONS);

#else	/* !SERIALIZABLE_STRUCT */

IMPLEMENT_CLASS(OrbitStruct, NO_FUNCTIONS);

#endif	/* SERIALIZABLE_STRUCT */

#if SERIALIZABLE_STRUCT

/*
 * prepare for serialization
 */
static void OrbitStruct_Sleep(INTERNAL_FUNCTION_PARAMETERS)
{
	/* get struct data */
	OrbitStruct * p_struct = OrbitStruct_RetrieveData(this_ptr);
	
	/* validate data */
	if (p_struct == NULL)
	{
		goto error;
	}

#if 0
	/* add property to zval */
	add_property_string(this_ptr, IOR_PROPERTY_KEY, p_ior, TRUE);
	
	/* create array */
	array_init(return_value);

	/* add name of property IOR to array */
	add_next_index_string(return_value, IOR_PROPERTY_KEY, TRUE);
#endif
	
	return;

error:
	RETURN_NULL();
}

static void OrbitStruct_Wakeup(INTERNAL_FUNCTION_PARAMETERS)
{
}

#endif	/* SERIALIZABLE_STRUCT */

char * OrbitStruct_GetRepositoryId(OrbitStruct * pStruct)
{
	if (pStruct->mIsException)
		return ExceptionType_GetRepositoryId(pStruct->u.mpExceptionType);
	else
		return StructType_GetRepositoryId(pStruct->u.mpStructType);
}

static zend_bool OrbitStruct_InitializeMembers(OrbitStruct * pStruct)
{
	MemberType * p_member = NULL;
 
	if (pStruct->mIsException)
		p_member = ExceptionType_GetFirstMember(pStruct->u.mpExceptionType);
	else
		p_member = StructType_GetFirstMember(pStruct->u.mpStructType);
	
	zend_hash_init(											
			&pStruct->mMembers,					/* hash table */
			0,													/* size */
			NULL,												/* hash function */
			ZVAL_DESTRUCTOR,						/* destructor */
			0);													/* persistent */

	if (!MemberType_IsValid(p_member))
		return TRUE;	/* no members */

	do
	{
		zval * p_value = NULL;
		char * p_name = MemberType_GetName(p_member);

		ALLOC_ZVAL(p_value);
		ZVAL_NULL(p_value);
		
		zend_hash_add(
				&pStruct->mMembers,
				p_name,
				strlen(p_name)+1,	/* important: include terminating zero byte */
				p_value,
				sizeof(zval),
				NULL
				);
	} while (MemberType_GetNext(p_member));
	
	return TRUE;
}

static zend_bool OrbitStruct_Initialize(const char * pId, OrbitStruct * pStruct)
{
	/* find type info */
	pStruct->u.mpStructType = TypeManager_FindStruct(pId);

	if (pStruct->u.mpStructType == NULL)
	{
		/* not a struct -- maybe an exception? */
		pStruct->u.mpExceptionType = TypeManager_FindException(pId);
		if (pStruct->u.mpExceptionType == NULL)
		{
			zend_error(E_WARNING, "(Satellite) unknown struct or exception '%s'", pId);
		
			goto error;	
		}

		pStruct->mIsException = TRUE;
	}

	/* initialize members */
	if (!OrbitStruct_InitializeMembers(pStruct))
		goto error;	

	return TRUE;

error:
	return FALSE;
}

/*
 * used by orbit_namedvalue_to_zval_struct(...)
 */
zend_bool OrbitStruct_Create(const char * pId, zval * pDestination)
{
	/* allocate buffer */
	OrbitStruct * p_struct = orbit_new(OrbitStruct);
	
	/* initialize struct */
	if (!OrbitStruct_Initialize(pId, p_struct))
		goto error;

	object_init_ex(pDestination, &OrbitStruct_class_entry);

	/* save orbit data */
	OrbitStruct_SaveData(pDestination, p_struct);

	return TRUE;

error:
	orbit_delete(p_struct);
	return FALSE;
}

/*
 * Constructor
 *
 * Parameters: Repository ID of struct
 */
zend_bool  OrbitStruct_Constructor(OrbitStruct  ** ppStruct, 
		int parameterCount, const zval ** ppParameters)
{
	/* allocate buffer */
	OrbitStruct * p_struct = orbit_new(OrbitStruct);

	/* check parameter count */
	if (parameterCount != 1)
	{
		zend_wrong_param_count(TSRMLS_C);
		goto error;
	}

	/* validate parameter types */
	if (Z_TYPE_P(ppParameters[0]) != IS_STRING)
		goto error;
	
	/* initialize struct */
	if (!OrbitStruct_Initialize(Z_STRVAL_P(ppParameters[0]), p_struct))
		goto error;
	
	*ppStruct = p_struct;
	return TRUE;

error:
	OrbitStruct_Destructor(p_struct);
	*ppStruct = NULL;
	return FALSE;
}

zend_bool  OrbitStruct_Destructor(OrbitStruct  * pStruct)
{
	if (pStruct->mIsException)
		ExceptionType_release(pStruct->u.mpExceptionType);
	else
		StructType_release(pStruct->u.mpStructType);
	
	/* will crash on uninitialized members structure :-( */
	zend_hash_destroy(&pStruct->mMembers);
	orbit_delete(pStruct);

	return TRUE;
}

/* not used */
zend_bool  OrbitStruct_CallFunction(OrbitStruct  * pStruct, 
		const char * pFunctionName, int parameterCount, const zval ** ppParameters, zval * pResult)
{
	return FALSE;
}

zend_bool  OrbitStruct_PutProperty(OrbitStruct  * pStruct, 
		const char * pPropertyName, const zval * pValue)
{
	zend_bool result;
	
	result = orbit_store_by_key(&pStruct->mMembers, pPropertyName, pValue);

	if (!result)
	{
		zend_error(E_WARNING, "(Satellite) unknown member '%s' in struct '%s'",
				pPropertyName, OrbitStruct_GetRepositoryId(pStruct));
	}

	return result;
}

zend_bool  OrbitStruct_GetProperty(OrbitStruct  * pStruct, 
		const char * pPropertyName, zval * pReturnValue)
{
	zval * p_value = orbit_find_by_key(&pStruct->mMembers, pPropertyName);

	if (p_value == NULL)
	{
		zend_error(E_WARNING, "(Satellite) unknown member '%s' in struct '%s'",
				pPropertyName, OrbitStruct_GetRepositoryId(pStruct));

		ZVAL_NULL(pReturnValue);
		return FALSE;
	}
	else
	{
		memcpy(pReturnValue, p_value, sizeof(zval));		/* raw data copy */
		zval_copy_ctor(pReturnValue);									/* smart data copy */
		INIT_PZVAL(pReturnValue);											/* set reference count */
		return TRUE;
	}
}

