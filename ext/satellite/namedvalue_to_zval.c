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
 * Functions to convert a CORBA_NamedValue to a zval
 *
 * -----------------------------------------------------------------------
 */
#include <zend_API.h>
#include "namedvalue_to_zval.h"
#include "common.h"
#include "object.h"
#include "struct.h"
#include "corba.h"

#include <ORBitutil/util.h>		/* for ALIGN_ADDRESS() */

static zend_bool satellite_any_to_zval_boolean(
		const CORBA_any * pSource, zval * pDestination)
{
	CORBA_boolean * p_value = (CORBA_boolean *)pSource->_value;
	ZVAL_BOOL(pDestination, *p_value != 0);
	return TRUE;
}

static zend_bool satellite_any_to_zval_double(
		const CORBA_any * pSource, zval * pDestination)
{
	CORBA_double * p_value = (CORBA_double *)pSource->_value;
	ZVAL_DOUBLE(pDestination, *p_value);
	return TRUE;
}

static zend_bool satellite_any_to_zval_long(
		const CORBA_any * pSource, zval * pDestination)
{
	CORBA_long * p_value = (CORBA_long *)pSource->_value;
	if (p_value)
	{
		ZVAL_LONG(pDestination, *p_value);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

static zend_bool satellite_any_to_zval_short(
		const CORBA_any * pSource, zval * pDestination)
{
	CORBA_short * p_value = (CORBA_short *)pSource->_value;
	ZVAL_LONG(pDestination, *p_value);
	return TRUE;
}


static zend_bool satellite_any_to_zval_objref(
		const CORBA_any * pSource, zval * pDestination)
{
	CORBA_Object * p_value = (CORBA_Object *)pSource->_value;
	CORBA_Object dup = 
		CORBA_Object_duplicate(*p_value, orbit_get_environment());
	
	return OrbitObject_Create(dup, pDestination);
}

static zend_bool satellite_any_to_zval_sequence(
		const CORBA_any * pSource, zval * pDestination)
{
	int i;
	int length = 0, step;
	void ** pp_members = NULL;
	zend_bool success = FALSE;
	CORBA_NamedValue source_item;
	zval * p_destination_item = NULL;
	CORBA_TypeCode content_type = NULL;
	CORBA_sequence_octet * p_value = 
		(CORBA_sequence_octet *)pSource->_value;
	
	/* get array to items */
	pp_members = (void **)p_value->_buffer;

	/* get number of members */
	length = p_value->_length;

	/* get type of items */
	content_type = CORBA_TypeCode_content_type(
			pSource->_type, orbit_get_environment());
	if (content_type == NULL)
		goto error;

	/* set zval members */
#if 0
	pDestination->type = IS_ARRAY;
	pDestination->refcount = 1;

	pDestination->value.ht = orbit_new(HashTable);

	zend_hash_init(
			pDestination->value.ht,		/* hash table */
			0, 												/* size */
			NULL, 										/* hash function */
			ZVAL_PTR_DTOR, 						/* destructor */
			0);												/* persistent */
#else
	array_init(pDestination);
#endif

	step = content_type->length ? content_type->length : 1;

	for (i = 0; i < length; i++)
	{
		p_destination_item = NULL;
		memset(&source_item, 0, sizeof(CORBA_NamedValue));

		source_item.argument._type = content_type;
		source_item.argument._value = pp_members;

		pp_members += step;

		ALLOC_ZVAL(p_destination_item);
		
		/* convert item */
		success = orbit_namedvalue_to_zval(
			&source_item,
			p_destination_item);

		if (!success)
			goto error;

		/* add to hashtable */
		INIT_PZVAL(p_destination_item);	/* testing! */
		zend_hash_next_index_insert(
			pDestination->value.ht,
			&p_destination_item,
			sizeof(zval *),
			NULL);

/*		FREE_ZVAL(p_destination_item);*/
	}

	return TRUE;
	
error:
	return FALSE;
}

static zend_bool satellite_any_to_zval_string(
		const CORBA_any * pSource, zval * pDestination)
{
	char ** pp_value = (char **)pSource->_value;
	ZVAL_STRING(pDestination, (*pp_value), TRUE);
	return TRUE;
}

static zend_bool satellite_any_to_zval_struct(
		const CORBA_any * pSource, zval * pDestination)
{
	int member_count = 0;
	CORBA_TypeCode type = NULL;
	char * p_repository_id = NULL;
	OrbitStruct * p_struct = NULL;

	/* make type a shortcut to the typecode */
	type = pSource->_type;

	/* find out the repository id */
	p_repository_id = CORBA_TypeCode_id(type, orbit_get_environment());
	/* do not check for exception because we may be converting one! */
	if (p_repository_id == NULL)
		goto error;

	if (!OrbitStruct_Create(p_repository_id, pDestination))
		goto error;

	p_struct = OrbitStruct_RetrieveData(pDestination);
	if (p_struct == NULL)
		goto error;

	/* now we know the source and destination matches! */
	member_count = CORBA_TypeCode_member_count(type, orbit_get_environment());
	/* do not check for exception because we may be converting one! */

	if (member_count > 0)
	{
		int i;
		zend_bool success;
		zval destination_item;
		CORBA_NamedValue source_item;
		char * p_buffer = (char *)pSource->_value;
		
		for (i = 0; i < member_count; i++)
		{
			int alignment;
			char * p_name = 
				CORBA_TypeCode_member_name(type, i, orbit_get_environment());
			if (p_name == NULL)
				goto error;
			
			/* prepare destination item */
			ZVAL_NULL(&destination_item);

			/* prepare source item */
			memset(&source_item, 0, sizeof(CORBA_NamedValue));
			source_item.argument._type = 
					CORBA_TypeCode_member_type(type, i, orbit_get_environment());
			if (source_item.argument._type == NULL)
				goto error;

			/* align pointer and set value */
			alignment = ORBit_find_alignment(source_item.argument._type);
			p_buffer = ALIGN_ADDRESS(p_buffer, alignment);
			source_item.argument._value = p_buffer;
			
			/* convert item */
			if (!orbit_namedvalue_to_zval(&source_item, &destination_item))
				goto error;
			
			/* set attribute */
			success = OrbitStruct_PutProperty(p_struct, p_name, &destination_item);
			if (!success)	goto error;
			
			/* advance buffer */
			p_buffer += ORBit_gather_alloc_info(source_item.argument._type);
		}
	}

	return TRUE;

error:	
	return FALSE;
}

zend_bool satellite_any_to_zval(
		const CORBA_any * pSource, zval * pDestination)
{
	zend_bool success = FALSE;
	CORBA_TCKind kind;
  
	if (pSource->_type == NULL || pDestination == NULL)
		return FALSE;
		
	kind = CORBA_TypeCode_kind(
			pSource->_type, 
			orbit_get_environment());	/* does not throw exceptions */

	switch (kind)
	{
		case CORBA_tk_boolean:
			success = satellite_any_to_zval_boolean(pSource, pDestination);
			break;
		
		case CORBA_tk_double:
			success = satellite_any_to_zval_double(pSource, pDestination);
			break;

		case CORBA_tk_enum:
		case CORBA_tk_long:
			success = satellite_any_to_zval_long(pSource, pDestination);
			break;

		case CORBA_tk_objref:
			success = satellite_any_to_zval_objref(pSource, pDestination);
			break;

		case CORBA_tk_sequence:
			success = satellite_any_to_zval_sequence(pSource, pDestination);
			break;

		case CORBA_tk_short:
			success = satellite_any_to_zval_short(pSource, pDestination);
			break;

		case CORBA_tk_string:
			success = satellite_any_to_zval_string(pSource, pDestination);
			break;

		case CORBA_tk_struct:	/* 15 */
		case CORBA_tk_except: /* 22 */
			success = satellite_any_to_zval_struct(pSource, pDestination);
			break;

		default:
/*			printf("unsupported corba TCKind %i\n", kind);*/
			zend_error(E_WARNING, "(satellite) unsupported corba TCKind %i", kind);
	}

	return success;
}

zend_bool orbit_namedvalue_to_zval(
		const CORBA_NamedValue * pSource, zval * pDestination)
{
	if (pSource == NULL)
		return FALSE;

	return satellite_any_to_zval(&pSource->argument, pDestination);
}


