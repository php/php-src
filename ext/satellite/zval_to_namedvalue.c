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
 * Functions to convert a zval to a CORBA_NamedValue
 *
 * -----------------------------------------------------------------------
 */
#include <zend_API.h>
#include "zval_to_namedvalue.h"
#include "object.h"
#include "struct.h"
#include "common.h"
#include "corba.h"

#include <ORBitutil/util.h>		/* for ALIGN_ADDRESS() */

/*
 * value should be a CORBA_boolean *
 */
static zend_bool satellite_zval_to_namedvalue_boolean(const zval * pSource, 
		CORBA_NamedValue * pDestination) 
{ 
	CORBA_boolean * p_value = (CORBA_boolean *)pDestination->argument._value;

	pDestination->len = sizeof(CORBA_boolean);

	if (pSource == NULL)
		return TRUE;
		
	if (pSource->type != IS_BOOL && pSource->type != IS_LONG)
		return FALSE;

	*p_value = pSource->value.lval != 0;

	return TRUE;
}


/*
 * value should be a double *
 */
static zend_bool satellite_zval_to_namedvalue_double(const zval * pSource, 
		CORBA_NamedValue * pDestination) 
{ 
	CORBA_double * p_value = (CORBA_double *)pDestination->argument._value;

	pDestination->len = sizeof(CORBA_double);

	if (pSource == NULL)
		return TRUE;
		
	/*convert_to_double(pSource);*/
	if (pSource->type != IS_DOUBLE)
		return FALSE;

	*p_value = pSource->value.dval;

	return TRUE;
}

/*
 * value should be a long *
 */
static zend_bool satellite_zval_to_namedvalue_long(const zval * pSource, 
		CORBA_NamedValue * pDestination) 
{ 
	CORBA_long * p_value = (CORBA_long *)pDestination->argument._value;
	pDestination->len = sizeof(CORBA_long);

	if (pSource == NULL)
		return TRUE;
	
	convert_to_long((zval*)pSource);	/* so long "const" */
	if (pSource->type != IS_LONG)
		return FALSE;

	*p_value = pSource->value.lval;

	return TRUE;
}

/*
 * value should be a short *
 */
static zend_bool satellite_zval_to_namedvalue_short(const zval * pSource, 
		CORBA_NamedValue * pDestination) 
{ 
	CORBA_short * p_value = (CORBA_short *)pDestination->argument._value;
	pDestination->len = sizeof(CORBA_short);

	if (pSource == NULL)
		return TRUE;
	
/*	convert_to_long((zval *)pSource);*/	/* discard const */;
	if (pSource->type != IS_LONG)
	{
/*		printf("source value type is %i in satellite_zval_to_namedvalue_short\n", pSource->type);*/
		return FALSE;
	}

	*p_value = pSource->value.lval;

	return TRUE;
}

/*
 * convert an OrbitObject to a CORBA object
 *
 * value should just be a CORBA_Object
 */
static zend_bool satellite_zval_to_namedvalue_objref(const zval * pSource, 
		CORBA_NamedValue * pDestination) 
{ 
	OrbitObject * pObject = NULL;
	CORBA_Object * p_value = (CORBA_Object*)&pDestination->argument._value;
		
	pDestination->len = 1;

	if (pSource == NULL)
		return TRUE; /* nothing else to do */

	if (pSource->type != IS_OBJECT)
		goto error;

	/* see that it's a corba object */
	if (strcmp(pSource->value.obj.ce->name, "OrbitObject") != 0)
		goto error; /* bad class type */

	pObject = OrbitObject_RetrieveData(pSource);
	if (pObject == NULL)
		goto error; /* unexpected error */
		
	*p_value = OrbitObject_GetCorbaObject(pObject);
	
	/* debug */
/*	(CORBA_Object)pDestination->argument._value = OrbitObject_GetCorbaObject(pObject);*/

	if (*p_value == NULL)
	{
/*		printf("NULL object in satellite_zval_to_namedvalue_objref\n");*/
		goto error;
	}

/*	printf("satellite_zval_to_namedvalue_objref: %s\n", (*p_value)->type_id);*/
		
	return TRUE;

error:
	return FALSE;
}

/*
 * convert a PHP array to a CORBA sequence
 *
 * value should be a CORBA_sequence_octet *
 */
static zend_bool satellite_zval_to_namedvalue_sequence(const zval * pSource, 
		CORBA_NamedValue * pDestination) 
{ 
	CORBA_sequence_octet * p_sequence_octet = 
		(CORBA_sequence_octet *)pDestination->argument._value;
	HashTable * p_hashtable = NULL;
	int member_count = 0;

	if (pSource == NULL)
		return TRUE;	/* nothing to do */

	if (pSource->type != IS_ARRAY)
		goto error;	/* bad source type */

	p_hashtable = pSource->value.ht;
	member_count = zend_hash_num_elements(p_hashtable);

	/* prepare sequence octet */
	ORBit_sequence_octet_init(p_sequence_octet, member_count);

	/*
	 * XXX: potential bug nest follows
	 */
	if (member_count > 0)
	{
		CORBA_NamedValue destination_value;
		zval * p_source_value;
		HashPosition position = NULL;
		int i;
		void ** p_members = (void **)p_sequence_octet->_buffer;

		/* prepare to start walking the hash table */
		zend_hash_internal_pointer_reset_ex(p_hashtable, &position);

		for(i = 0; i < member_count; i++)
		{
			if (position == NULL)
				goto error;

#if 0
			/* the function call gets messed up */
			if (zend_hash_get_current_data_ex(
						p_hashtable, /* XXX */, &position) == FAILURE)
				goto error;
#else
			p_source_value = *(zval**)position->pData;
#endif
			
			memset(&destination_value, 0, sizeof(destination_value));
			
			destination_value.argument._type = CORBA_TypeCode_content_type(
					pDestination->argument._type, orbit_get_environment());

			if (orbit_caught_exception())
				goto error;

			/* now get a NamedValue for this element */
			if (!orbit_zval_to_namedvalue(p_source_value, &destination_value))
				goto error;
			
			/* put values in our value */
			p_members[i] = *(void**)destination_value.argument._value;
			
			/* get next in hashtable */
			if (zend_hash_move_forward_ex(p_hashtable, &position) == FAILURE)
				goto error;
		}
	}
	
/*	pDestination->argument._value = p_sequence_octet;*/
	return TRUE;
	
error:
	ORBit_sequence_octet_fini(p_sequence_octet); /* appropriate destruction? */
	return FALSE; 
}

/*
 * convert a PHP string to a CORBA string
 *
 * value should be a char **
 */
static zend_bool satellite_zval_to_namedvalue_string(const zval * pSource, 
		CORBA_NamedValue * pDestination) 
{
	char ** p_value = (char **)pDestination->argument._value;
	
	if (pSource == NULL)
		return TRUE; 	/* do nothing */

#if 0
	convert_to_string((zval *)pSource);	/* discard const */
	if (pSource->type == IS_NULL)
		return TRUE; 	/* do nothing */
#endif
	
	if (pSource->type != IS_STRING)
		goto error;

	*p_value 						= CORBA_string_dup(pSource->value.str.val);
	pDestination->len		= strlen(*p_value);

	return TRUE;

error:
	return FALSE;
}

/*
 * convert an OrbitStruct object to a CORBA structure
 *
 * XXX: this is trouble!
 *
 */
static zend_bool satellite_zval_to_namedvalue_struct(const zval * pSource, 
		CORBA_NamedValue * pDestination) 
{ 
	OrbitStruct * p_struct = NULL;
	int member_count = 0;
	CORBA_TypeCode type = NULL;
	
	pDestination->len = 1;
	
	/* nothing else to do if we have no source object */
	if (pSource == NULL)
		return TRUE;

	/* see that it's an object */
	if (pSource->type != IS_OBJECT)
		goto error; /* bad source type */

	/* see that it's a structure object */
	if (strcmp(pSource->value.obj.ce->name, "OrbitStruct") != 0)
		goto error; /* bad class type */
		
	/* get struct info */
	p_struct = OrbitStruct_RetrieveData(pSource);
	if (p_struct == NULL)
		goto error; /* unexpected error */

	/* make type a shortcut to the typecode */
	type = pDestination->argument._type;

	/* check respository ids */
	{
		char * p_source_repository_id =	OrbitStruct_GetRepositoryId(p_struct);
		char * p_destination_repository_id = CORBA_TypeCode_id(type, orbit_get_environment());

		if (orbit_caught_exception())
			goto error;

		if (strcmp(p_destination_repository_id, p_source_repository_id) != 0)
			goto error;	/* bad struct */
	}

	/* now we know the source and destination matches! */
	member_count = CORBA_TypeCode_member_count(type, orbit_get_environment());
	if (orbit_caught_exception())
		goto error;

	if (member_count > 0)
	{
		int i;
		CORBA_NamedValue destination_value;
		zval source_value;
		zend_bool success;
		char * p_buffer = (char *)pDestination->argument._value;
		int element_size = 0;

		for (i = 0; i < member_count; i++)
		{
			int alignment;
			/* get member name */
			char * p_name = 
				CORBA_TypeCode_member_name(type, i, orbit_get_environment());
			if (orbit_caught_exception())
				goto error;

			/* get source value */
			success = OrbitStruct_GetProperty(p_struct, p_name, &source_value);
			if (!success)
			{
				goto error;
			}

			/* prepare destination value */
			memset(&destination_value, 0, sizeof(CORBA_NamedValue));
			destination_value.argument._type = 
					CORBA_TypeCode_member_type(type, i, orbit_get_environment());
			if (destination_value.argument._type == NULL)
				goto error;

			/* convert member */
			success = orbit_zval_to_namedvalue(&source_value, &destination_value);
			if (!success)
				goto error;

			element_size = ORBit_gather_alloc_info(destination_value.argument._type);
			
			/* align pointer and set value */
			alignment = ORBit_find_alignment(destination_value.argument._type);
			p_buffer = ALIGN_ADDRESS(p_buffer, alignment);

			/* copy value to struct */
			memcpy(p_buffer, destination_value.argument._value, element_size);
			p_buffer += element_size;
		}
	}
	
	return TRUE;

error:
	return FALSE;
}

/*
 * public function to convert form a zval to CORBA_NamedValue
 *
 * The data type specific functions does not own 
 * pDestination->argument._value!!!
 */
zend_bool orbit_zval_to_namedvalue(const zval * pSource, 
		CORBA_NamedValue * pDestination)
{
	zend_bool success = FALSE;
	CORBA_TCKind kind; 
	
	if (pDestination->argument._type == NULL)
	{
/*		printf("pDestination->argument._type is NULL in orbit_zval_to_namedvalue\n");*/
		return FALSE;
	}
	
	kind = CORBA_TypeCode_kind(
			pDestination->argument._type, 
			orbit_get_environment());

	if (orbit_caught_exception())
		return FALSE;

	/* allocate memory for value */
	if (pSource != NULL)
	{
		/* use special function to allocate value -- good or bad idea? */
		pDestination->argument._value = 
			ORBit_alloc_tcval(pDestination->argument._type, 1);
	}
	
	switch (kind)
	{
		case CORBA_tk_boolean:
			success = satellite_zval_to_namedvalue_boolean(pSource, pDestination);
			break;

		case CORBA_tk_double:
			success = satellite_zval_to_namedvalue_double(pSource, pDestination);
			break;

		case CORBA_tk_enum:
		case CORBA_tk_long:
			success = satellite_zval_to_namedvalue_long(pSource, pDestination);
			break;

		case CORBA_tk_objref:
			success = satellite_zval_to_namedvalue_objref(pSource, pDestination);
			break;

		case CORBA_tk_sequence:
			success = satellite_zval_to_namedvalue_sequence(pSource, pDestination);
			break;

		case CORBA_tk_short:
			success = satellite_zval_to_namedvalue_short(pSource, pDestination);
			break;

		case CORBA_tk_string:	/* 18 */
			success = satellite_zval_to_namedvalue_string(pSource, pDestination);
			break;

		case CORBA_tk_struct:		/* 15 */
			success = satellite_zval_to_namedvalue_struct(pSource, pDestination);
			break;

		default:
/*			printf("unsupported corba TCKind %i\n", kind);*/
/*			php_error(E_WARNING, "unsupported corba TCKind %i", kind);*/
	}

	if (!success)
	{
/*			printf("orbit_zval_to_namedvalue failed for TCKind %i\n", kind);*/
		/* 
		 * does this always crash? then we better look for another way of
		 * freeing the data!
		 */
/*		CORBA_free(pDestination->argument._value); */
		pDestination->argument._value = NULL;
	}

	return success;
}

