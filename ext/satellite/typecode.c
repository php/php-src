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
 * functions to convert IDL_tree info to CORBA_TypeCodes
 *
 * ought to be cleaned up...
 *
 * -----------------------------------------------------------------------
 */
#include "corba.h"
#include "common.h"
#include "findtype.h"
#include "typecode.h"

void satellite_release_typecode(CORBA_TypeCode typecode)
{
	CORBA_Object_release((CORBA_Object)typecode, orbit_get_environment());
}

static CORBA_TypeCode orbit_idl_tree_type_to_typecode_enum(IDL_tree type)
{
	CORBA_TypeCode type_code;
	CORBA_EnumMemberSeq * p_members = NULL;

	p_members = CORBA_EnumMemberSeq__alloc();
	p_members->_maximum = p_members->_length = 1;
	p_members->_buffer = CORBA_sequence_CORBA_Identifier_allocbuf(p_members->_length);

	p_members->_buffer[0] = "dummy";

	/* 
	 * TODO: initialize p_members with members...
	 *       look at orbit_idl_tree_type_to_typecode_struct
	 */

	type_code = CORBA_ORB_create_enum_tc(
			orbit_get_orb(),
			IDL_IDENT(IDL_TYPE_ENUM(type).ident).repo_id,
			IDL_IDENT(IDL_TYPE_ENUM(type).ident).str,
			p_members,
			orbit_get_environment());

	if (orbit_error_test("CORBA_ORB_create_enum_tc"))
	{
		goto error;
	}

	if (type_code == NULL)
	{
/*		printf("unable to create enum typecode for type %s\n", 
				IDL_IDENT(IDL_TYPE_ENUM(type).ident).str);*/
	}

	goto exit;

error:
	type_code = NULL;

exit:
	CORBA_free(p_members);
	return type_code;
}

/*
 * create a member sequence for a struct or exception
 */
static CORBA_StructMemberSeq * orbit_create_member_sequence(IDL_tree member_list)
{
	int i = 0;
	CORBA_StructMemberSeq * p_members = NULL;
	IDL_tree first_member = member_list;
	
	p_members = CORBA_sequence_CORBA_StructMember__alloc();
	/* this is initially set to false, why? */
	p_members->_release = CORBA_TRUE;

	/* ok with empty list */
	if (member_list == NULL)
		return p_members;
	
	/* first iteration, find out member count */
	do
	{
		IDL_tree declaration = IDL_MEMBER(IDL_LIST(member_list).data).dcls;
		if (declaration == NULL)	/* very unlikely! */
			continue;

		do
		{		
			p_members->_length++;
		} while ((declaration = IDL_LIST(declaration).next));
	} while ((member_list = IDL_LIST(member_list).next));

	p_members->_maximum = p_members->_length;
	p_members->_buffer = 
		CORBA_sequence_CORBA_StructMember_allocbuf(p_members->_length);
	
	/* second iteration, set member data */
	member_list = first_member;
	do
	{
		IDL_tree declaration = IDL_MEMBER(IDL_LIST(member_list).data).dcls;
		CORBA_TypeCode type_code = NULL;
		
		if (declaration == NULL)	/* very unlikely! */
			continue;

		type_code = orbit_idl_tree_type_to_typecode(
				IDL_MEMBER(IDL_LIST(member_list).data).type_spec);

		if (type_code == NULL)
		{
			zend_error(E_WARNING, "(Satellite) unknown type for member %s", IDL_IDENT(IDL_LIST(declaration).data).str);
			goto error;
		}
		
		do
		{		
			char * p_name = IDL_IDENT(IDL_LIST(declaration).data).str;

			p_members->_buffer[i].name = CORBA_string_dup(p_name);
			Z_TYPE(p_members->_buffer[i]) = (CORBA_TypeCode)
				CORBA_Object_duplicate((CORBA_Object)type_code, orbit_get_environment());
			Z_TYPE(p_members->_buffer[i])_def = NULL; /* XXX */

			i++;
		} while ((declaration = IDL_LIST(declaration).next));

		satellite_release_typecode(type_code);
		
	} while ((member_list = IDL_LIST(member_list).next));

	goto exit;

error:
	/* TODO: error clean up */
	CORBA_free(p_members);
	p_members = NULL;
	
exit:
	return p_members;
}

/*
 * create a CORBA_TypeCode for a struct
 */
static CORBA_TypeCode orbit_idl_tree_type_to_typecode_struct(IDL_tree type)
{
	CORBA_TypeCode type_code = NULL;
	CORBA_StructMemberSeq * p_members = NULL;

#if 0
	if (IDL_TYPE_STRUCT(type).member_list == NULL)
	{
		printf("no struct members\n");
		goto error;
	}
#endif

	p_members = orbit_create_member_sequence(IDL_TYPE_STRUCT(type).member_list);
	if (p_members == NULL)
		goto error;
	
	type_code = CORBA_ORB_create_struct_tc(
			orbit_get_orb(),
			IDL_IDENT(IDL_TYPE_STRUCT(type).ident).repo_id,
			IDL_IDENT(IDL_TYPE_STRUCT(type).ident).str,
			p_members,
			orbit_get_environment());

	if (type_code == NULL)
		goto error;

	goto exit;

error:
	satellite_release_typecode(type_code);

exit:
	CORBA_free(p_members);
	return type_code;
}


static CORBA_TypeCode orbit_idl_tree_type_to_typecode_float(IDL_tree type)
{
	enum IDL_float_type float_type = IDL_TYPE_FLOAT(type).f_type;

	switch (float_type)
	{
		case IDL_FLOAT_TYPE_DOUBLE:	return TC_CORBA_double;

		default:
		/*	php_error(E_WARNING, "only double floats supported at the moment at the moment");*/
			return NULL;
	}
}


static CORBA_TypeCode orbit_idl_tree_type_to_typecode_integer(IDL_tree type)
{
	enum IDL_integer_type integer_type = IDL_TYPE_INTEGER(type).f_type;

	switch (integer_type)
	{
		case IDL_INTEGER_TYPE_SHORT:	return TC_CORBA_short;
		case IDL_INTEGER_TYPE_LONG: 	return TC_CORBA_long;

		default:
/*			php_error(E_WARNING, "unsupported integer type %i", integer_type);*/
			return NULL;
	}
}

static CORBA_TypeCode orbit_idl_tree_type_to_typecode_sequence(IDL_tree type)
{
	CORBA_TypeCode simple_typecode = orbit_idl_tree_type_to_typecode(
				IDL_TYPE_SEQUENCE(type).simple_type_spec);

	CORBA_TypeCode typecode = CORBA_ORB_create_sequence_tc( 
			orbit_get_orb(), 
			0, /* bound */
			simple_typecode, 
			orbit_get_environment() );

#if 0
	if (typecode != NULL)
	{
		typecode->parent.interface = 0;
		typecode->parent.refs = ORBIT_REFCNT_STATIC;	/* correct? */
	}
#endif

	satellite_release_typecode(simple_typecode);

	return typecode;
}

/*
 * get real type when we have an object/structure/union/enum ident
 */
static CORBA_TypeCode orbit_idl_tree_type_to_typecode_ident(IDL_tree type)
{
	IDL_tree real_type = NULL;
	IDL_tree whole_tree = type;
	
	/* go back to to top of tree (ugly code of the day!) */
	while (whole_tree->up)
		whole_tree = whole_tree->up;
	
	/* find the real type! */
	real_type = orbit_find_type_simple(whole_tree, IDL_IDENT_REPO_ID(type));

	/* check return value */
	if (real_type == NULL)
	{
/*		php_error(E_WARNING, 
			"unknown repository id %s",
			IDL_IDENT_REPO_ID(type));*/
		return FALSE;
	}

	/* call our generic function again... (hehehe) */
	return orbit_idl_tree_type_to_typecode(real_type);
}

/*
 * make exception typecode
 */
static CORBA_TypeCode orbit_idl_tree_type_to_typecode_exception(IDL_tree type)
{
	CORBA_TypeCode type_code = NULL;
	CORBA_StructMemberSeq * p_members = NULL;

#if 0
	if (IDL_EXCEPT_DCL(type).members == NULL)
	{
		printf("no members\n");
		goto error;
	}
#endif

	p_members = orbit_create_member_sequence(IDL_EXCEPT_DCL(type).members);
	if (p_members == NULL)
		goto error;
	
	type_code = CORBA_ORB_create_exception_tc(
			orbit_get_orb(),
			IDL_IDENT(IDL_EXCEPT_DCL(type).ident).repo_id,
			IDL_IDENT(IDL_EXCEPT_DCL(type).ident).str,
			p_members,
			orbit_get_environment());

	goto exit;

error:
	satellite_release_typecode(type_code);

exit:
	CORBA_free(p_members);
	return type_code;

}

static CORBA_TypeCode orbit_idl_tree_type_to_typecode_interface(IDL_tree type)
{
	CORBA_TypeCode typecode = CORBA_ORB_create_interface_tc(
		orbit_get_orb(),
		IDL_IDENT(IDL_INTERFACE(type).ident).repo_id,
		IDL_IDENT(IDL_INTERFACE(type).ident).str,
		orbit_get_environment());

#if 0
	if (typecode != NULL)
	{
		typecode->parent.interface = 0;
		typecode->parent.refs = ORBIT_REFCNT_STATIC;	/* correct? */
	}
#endif

	return typecode;
}

static CORBA_TypeCode orbit_idl_tree_type_to_typecode_string(IDL_tree type)
{
	CORBA_TypeCode typecode = CORBA_ORB_create_string_tc(
		orbit_get_orb(),
		0,	/* bound */
		orbit_get_environment());

#if 0
	if (typecode != NULL)
	{
		typecode->parent.interface = 0;
		typecode->parent.refs = ORBIT_REFCNT_STATIC;	/* correct? */
	}
#endif

	return typecode;
}

/* 
 * get a CORBA_TypeCode from a IDL_tree node type
 */
CORBA_TypeCode orbit_idl_tree_type_to_typecode(IDL_tree type)
{
	CORBA_TypeCode type_code = NULL;
	
	if (type == NULL)
	{
		/* not good! */
		return FALSE;
	}

	switch (IDL_NODE_TYPE(type))
	{
		case IDLN_EXCEPT_DCL:
			type_code = orbit_idl_tree_type_to_typecode_exception(type);
			break;
		
		case IDLN_IDENT:
			type_code = orbit_idl_tree_type_to_typecode_ident(type);
			break;

		case IDLN_INTERFACE:
			/*type_code = TC_CORBA_Object; */
			type_code = orbit_idl_tree_type_to_typecode_interface(type);
			break;

		case IDLN_TYPE_BOOLEAN:
			type_code = TC_CORBA_boolean;
			break;

		case IDLN_TYPE_DCL:
			type_code = 
				orbit_idl_tree_type_to_typecode(IDL_TYPE_DCL(type).type_spec);
			break;

		case IDLN_TYPE_ENUM:
			type_code = orbit_idl_tree_type_to_typecode_enum(type);
			break;

		case IDLN_TYPE_FLOAT:
			type_code = orbit_idl_tree_type_to_typecode_float(type);
			break;
			
		case IDLN_TYPE_INTEGER:
			type_code = orbit_idl_tree_type_to_typecode_integer(type);
			break;

		case IDLN_TYPE_SEQUENCE:
			type_code = orbit_idl_tree_type_to_typecode_sequence(type);
			break;

		case IDLN_TYPE_STRING:
			/*type_code = TC_CORBA_string; */
			type_code = orbit_idl_tree_type_to_typecode_string(type);
			break;
		
		case IDLN_TYPE_STRUCT:
			type_code = orbit_idl_tree_type_to_typecode_struct(type);
			break;
	
		/* TODO: handle more types */
				
		default:
/*			printf("orbit_idl_tree_type_to_typecode can't handle type %s\n",
					IDL_NODE_TYPE_NAME(type));*/
	}

	return type_code;
}


