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


/*
 * Helper function for making PHP classes
 */

#include <php.h>
#include "common.h"
#include "class.h"

void orbit_class_function_call(
		zend_class_entry * pClass,
		int dataType,
		zend_property_reference *pPropertyReference, 
		Class_Constructor pConstructor, 
		Class_CallFunction pCallFunction,
		INTERNAL_FUNCTION_PARAMETERS)
{
	/* get object */
	zval * object = pPropertyReference->object;

	/* get function name */
	zend_overloaded_element * function_name =
		(zend_overloaded_element *)pPropertyReference->elements_list->tail->data;

	/* handle parameters */
	zval ** arguments = orbit_new_n(zval *, ZEND_NUM_ARGS());
		/*(zval **)emalloc(sizeof(zval *) * ZEND_NUM_ARGS());*/
	if (getParametersArray(ht, ZEND_NUM_ARGS(), arguments) == FAILURE)
	{
		/* TODO: handle error */
	}

	if (!object)
	{
		zend_error(E_WARNING, "Invalid Satellite class");\
		return;
	}

	/* constructor or normal function? */
	if (zend_llist_count(pPropertyReference->elements_list) == 1
			&& !strcasecmp(Z_STRVAL(function_name->element), pClass->name))
	{
		/* constructor */
		if (pConstructor)
		{
			void * p_data = NULL;
			zend_bool success = (*pConstructor)(&p_data, ZEND_NUM_ARGS(), arguments);

			if (success)
				orbit_save_data(object, dataType, p_data);
		}
		else
		{
			zend_error(E_WARNING, "(Satellite) This class has no constructor");\
		}
	}
	else
	{
		/* normal function */
		if (pCallFunction)
		{
			void * p_data = orbit_retrieve_data(object, dataType);

			if (p_data == NULL)
			{
				/* 
				 * this means that the constructor has failed earlier! 
				 * -- or should NULL be allowed here? 
				 */
				php_error(E_WARNING, "(Satellite) Class has no data!");
				RETVAL_NULL();
				goto orbit_class_function_call_exit;
			}
			
			/* pval * return_value is a part of INTERNAL_FUNCTION_PARAMETERS */
			(*pCallFunction)(p_data, Z_STRVAL(function_name->element),
											 ZEND_NUM_ARGS(), arguments, return_value);
		}
		else
		{
			zend_error(E_WARNING, "(Satellite) Can't call functions in this class");\
		}
	}

orbit_class_function_call_exit:
	satellite_delete(arguments);
	
	/* seems to be required! */
	zval_dtor(&function_name->element);
}

/*
 * save a corba object to a php object
 */
void orbit_save_data(zval * php_object, int type, void * data)
{
	pval * orbit_data_handle = NULL;
	long id = zend_list_insert(
			data, 										/* data */
			type								/* type */
			);
	

	/* 
	 * do it like they do in php_COM_call_function_handler 
	 * (insert into some magic hash index)
	 */
	ALLOC_ZVAL(orbit_data_handle);	/* allocate memory for value */
	
	Z_TYPE_P(orbit_data_handle) = IS_LONG;
	Z_LVAL_P(orbit_data_handle) = id;
	
	pval_copy_constructor(orbit_data_handle);	/* why? */
	
	INIT_PZVAL(orbit_data_handle);	/* set reference count */

	zend_hash_index_update(
			Z_OBJPROP_P(php_object), 					/* hashtable */
			0, 																/* hash??? */
			&orbit_data_handle,								/* data */
			sizeof(pval *),										/* data size */
			NULL															/* destination */
			);
}

/*
 * retrieve a corba object from a php object
 */
void * orbit_retrieve_data(const zval * php_object, int wanted_type)
{
	void * data = NULL;
	pval ** orbit_data_handle = NULL;
	int type = 0;

	if (!php_object)
		return NULL;
	
	/* get handle to corba data */
	zend_hash_index_find(										
			Z_OBJPROP_P(php_object),						/* hash table */	
			0, 																	/* hash??? */
			(void **)&orbit_data_handle					/* data */
			);

	if (orbit_data_handle == NULL || *orbit_data_handle == NULL)
	{
		return NULL;
	}

	/* get corba data */
	data = zend_list_find(
			Z_LVAL_PP(orbit_data_handle), 	/* id */
			&type																/* type */
			);

	/* verify corba object */
	if (!data || (type != wanted_type))
	{
		/* TODO: handle error */
		return NULL;
	}
	
	return data;
}


