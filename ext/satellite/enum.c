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
 * OrbitEnum class
 *
 * -----------------------------------------------------------------------
 */
#include "enum.h"
#include "typemanager.h"
#include "common.h"
#include "hashtable.h"

struct _OrbitEnum
{
	EnumType * mpEnumType;
	HashTable mMembers;
};

IMPLEMENT_CLASS(OrbitEnum, CONSTRUCTOR|DESTRUCTOR|GET_PROPERTY)


static zend_bool OrbitEnum_InitializeMembers(OrbitEnum * pEnum)
{
	int enum_value = 0;
	EnumMemberType * p_member = NULL;
	
	p_member = EnumType_GetFirstMember(pEnum->mpEnumType);

	zend_hash_init(											
			&pEnum->mMembers,					/* hash table */
			0,													/* size */
			NULL,												/* hash function */
			ZVAL_DESTRUCTOR,						/* destructor */
			0);													/* persistent */

	if (!EnumMemberType_IsValid(p_member))
		return TRUE;	/* no members */
	
	do
	{
		zval * p_value = NULL;
		char * p_name = EnumMemberType_GetName(p_member);

		ALLOC_ZVAL(p_value);
		ZVAL_LONG(p_value, enum_value);
		
		zend_hash_add(
			&pEnum->mMembers,
			p_name,
			strlen(p_name)+1,
			p_value,
			sizeof(zval),
			NULL
			);

		/* set value for next member */
		enum_value++;
	
	} while (EnumMemberType_GetNext(p_member));

	return TRUE;
}
	
zend_bool OrbitEnum_Constructor(OrbitEnum ** ppEnum, int parameterCount, 
		const zval ** ppParameters)
{
	OrbitEnum * p_enum = orbit_new(OrbitEnum);

	/* check parameter count */
	if (parameterCount != 1)
	{
		zend_wrong_param_count(TSRMLS_C);
		goto error;
	}

	/* validate parameter types */
	if (ppParameters[0]->type != IS_STRING)
		goto error;

	/* find type information */
	p_enum->mpEnumType = TypeManager_FindEnum(ppParameters[0]->value.str.val);
	if (p_enum->mpEnumType == NULL)
	{
		zend_error(E_WARNING, "(Satellite) unknown enum '%s'", ppParameters[0]->value.str.val);
		goto error;
	}

	/* initialize members */
	if (!OrbitEnum_InitializeMembers(p_enum))
		goto error;

	*ppEnum = p_enum;
	return TRUE;
	
error:
/*	printf("OrbitEnum_Constructor failed\n");*/
	OrbitEnum_Destructor(p_enum);
	*ppEnum = NULL;
	return FALSE;
}

zend_bool OrbitEnum_Destructor(OrbitEnum * pEnum)
{
/*	printf("OrbitEnum_Destructor\n");*/
	
	if (pEnum != NULL)
	{
		orbit_delete(pEnum->mpEnumType);
	}

	zend_hash_destroy(&pEnum->mMembers);
	orbit_delete(pEnum);

	return TRUE;
}

/* not used */
zend_bool  OrbitEnum_CallFunction(OrbitEnum  * pEnum, 
		const char * pFunctionName, int parameterCount, const zval ** ppParameters, zval * pResult)
{
	return FALSE;
}

/* not used */
zend_bool OrbitEnum_PutProperty(OrbitEnum * pEnum, 
		const char * pPropertyName, const zval * pValue)
{
	return FALSE;
}

zend_bool OrbitEnum_GetProperty(OrbitEnum * pEnum, 
		const char * pPropertyName, zval * pReturnValue)
{
	zval * p_value = orbit_find_by_key(&pEnum->mMembers, pPropertyName);

	if (p_value == NULL)
	{
		zend_error(E_WARNING, "(Satellite) unknown member '%s' in enum '%s'",
				pPropertyName, EnumType_GetRepositoryId(pEnum->mpEnumType));

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

