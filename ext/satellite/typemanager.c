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

/*
 * Type manager
 */
#include <string.h>
#include <orb/orbit.h>
#include "common.h"
#include "typemanager.h"
#include "typecode.h"
#include "findtype.h"
#include <glob.h>

struct _TypeManager 
{
/*	IDL_ns		mNamespace;*/
	IDL_tree	mRoot;
};

static TypeManager gTypeManager;

struct _InterfaceType 
{
	IdlInfo mIdlInfo;
};

struct _StructType 
{
	IdlInfo mIdlInfo;
};

struct _EnumType 
{
	IdlInfo mIdlInfo;
};

struct _ExceptionType 
{
	IdlInfo mIdlInfo;
};

struct _OperationType 
{
	IdlInfo mIdlInfo;
};

struct _AttributeType 
{
	IdlInfo mIdlInfo;
};

struct _MemberType 
{
	IDL_tree mOuterList;
	IDL_tree mInnerList;
};

struct _ParameterType 
{
	IDL_tree mList;
};

struct _EnumMemberType 
{
	IDL_tree mList;
};

#define PATTERN "/*.idl"
#define PATTERN_LENGTH (sizeof(PATTERN)-1)

CORBA_boolean TypeManager_Init(const char * pIdlDirectory)
{
	gTypeManager.mRoot = NULL;

	if (pIdlDirectory != NULL)
	{
		glob_t result;
		char * p_pattern = 
			orbit_new_n(char, strlen(pIdlDirectory) + PATTERN_LENGTH + 1);
		strcpy(p_pattern, pIdlDirectory);
		strcat(p_pattern, PATTERN);
		
		if (glob(p_pattern, 0, NULL, &result) == 0)
		{
			/* success... */
			int i;
			for (i = 0; i < result.gl_pathc; i++)
			{
				TypeManager_LoadFile(result.gl_pathv[i]);
			}

			globfree(&result);
		}

		orbit_delete(p_pattern);
	}
	
	return TRUE;
}

CORBA_boolean TypeManager_Shutdown()
{
	IDL_tree_free(gTypeManager.mRoot);
	return TRUE;
}

CORBA_boolean TypeManager_LoadFile(const char * pFilename)
{
  int x;
	IDL_tree root;
	IDL_ns namespace;
	IDL_tree list;

  x = IDL_parse_filename(
		pFilename, 
		"", 
		NULL, 
		&root, &namespace,
		IDLF_TYPECODES|IDLF_CODEFRAGS,
		IDL_WARNING1);

  if(IDL_SUCCESS != x) 
	{
/*		php_error(E_WARNING, "IDL_parse_filename returned %i");*/
		return FALSE;
  }

	list = IDL_list_new(root);
	
	if (gTypeManager.mRoot == NULL)
		gTypeManager.mRoot = list;
	else
		IDL_list_concat(gTypeManager.mRoot,list);

	/* get rid of this immediatly? */
	IDL_ns_free(namespace);
	
	return TRUE;
}


/* type manager functions */
InterfaceType * TypeManager_FindInterface(/*TypeManager * pTypeManager, */
		const char * pRepositoryId)
{
	InterfaceType * pInterface = orbit_new(InterfaceType);
	CORBA_boolean success = orbit_find_type(
			gTypeManager.mRoot, pRepositoryId, IDLN_INTERFACE, &pInterface->mIdlInfo);

	if (!success)
	{
		orbit_delete(pInterface);
		return NULL;
	}
		
	return pInterface;
}

StructType * TypeManager_FindStruct(/*TypeManager * pTypeManager, */
		const char * pRepositoryId)
{
	StructType * pStruct = orbit_new(StructType);
	CORBA_boolean success = orbit_find_type(
			gTypeManager.mRoot, pRepositoryId, IDLN_TYPE_STRUCT, &pStruct->mIdlInfo);

	if (!success)
	{
		orbit_delete(pStruct);
		pStruct = NULL;
	}
	
	return pStruct;
}

EnumType * TypeManager_FindEnum(/*TypeManager * pTypeManager, */
		const char * pRepositoryId)
{
	EnumType * pEnum = orbit_new(EnumType);
	CORBA_boolean success = orbit_find_type(
			gTypeManager.mRoot, pRepositoryId, IDLN_TYPE_ENUM, &pEnum->mIdlInfo);
	if (!success)
	{
		orbit_delete(pEnum);
		pEnum = NULL;
	}
	return pEnum;
}

ExceptionType * TypeManager_FindException(/*TypeManager * pTypeManager, */
		const char * pRepositoryId)
{
	ExceptionType * pException = orbit_new(ExceptionType);
	CORBA_boolean success = orbit_find_type(
			gTypeManager.mRoot, pRepositoryId, IDLN_EXCEPT_DCL, &pException->mIdlInfo);
	if (!success)
	{
		orbit_delete(pException);
		pException = NULL;
	}
	return pException;
}


	
/* 
   find an operation in an interface tree, 
   recursing into inherited interfaces  
*/
static CORBA_boolean
findoperation_recurse( IDL_tree tree, const char *name,
		       IdlInfo *pIdlInfo )
{
	IDL_tree inherited;
	CORBA_boolean success;

	success = orbit_find_type( IDL_INTERFACE(tree).body,
				   name, IDLN_OP_DCL, 
				   pIdlInfo );
	if ( success ) 
		return CORBA_TRUE;

	for ( inherited = IDL_INTERFACE(tree).inheritance_spec;
	      inherited != NULL;
	      inherited = IDL_LIST(inherited).next ) {
		IDL_tree ident = IDL_LIST(inherited).data;
		InterfaceType *iface;

		iface = TypeManager_FindInterface( IDL_IDENT(ident).repo_id );
		success = findoperation_recurse( iface->mIdlInfo.mType, 
						 name, 
						 pIdlInfo );
		orbit_delete( iface );

		if ( success )
			return CORBA_TRUE;

		zend_error( E_WARNING, "operation %s not found in iface %s\n", 
			    name, IDL_IDENT(ident).repo_id );
	}

	return CORBA_FALSE;
}

/* find an operation in an interface based on case-insensitive name */
OperationType * InterfaceType_FindOperation( InterfaceType * pInterface, 
					     const char * name )
{
	OperationType *pOperation = orbit_new(OperationType);
	IDL_tree tree = pInterface->mIdlInfo.mType;
	CORBA_boolean success;

	success = findoperation_recurse( tree, name, 
					 &pOperation->mIdlInfo );
	
	if ( success )
		return pOperation;

	orbit_delete(pOperation);
	return NULL;
}

/* find an attribute in an interface based on case-insensitive name */
AttributeType * InterfaceType_FindAttribute(InterfaceType * pInterface, 
		const char * name)
{
	AttributeType * pAttribute = orbit_new(AttributeType);
	CORBA_boolean success = orbit_find_type(
			pInterface->mIdlInfo.mType, name, IDLN_ATTR_DCL, &pAttribute->mIdlInfo);
	if (!success)
	{
		orbit_delete(pAttribute);
		pAttribute = NULL;
	}
	return pAttribute;
}

/* return type */
CORBA_TypeCode OperationType_GetReturnType(OperationType * pOperation)
{
	return orbit_idl_tree_type_to_typecode(
			IDL_OP_DCL(pOperation->mIdlInfo.mType).op_type_spec);
}

/* walk parameter list */
ParameterType * OperationType_GetFirstParameter(OperationType * pOperation)
{
	ParameterType * pParameter = orbit_new(ParameterType);
	pParameter->mList = 
		IDL_OP_DCL(pOperation->mIdlInfo.mType).parameter_dcls;
	return pParameter;
}

char * OperationType_GetName(OperationType * pOperation)
{
	if (pOperation && pOperation->mIdlInfo.mIdent)
		return IDL_IDENT(pOperation->mIdlInfo.mIdent).str;
	else
		return NULL;
}

void TypeCodeList_release(CORBA_TypeCode * p_typecodes)
{
	CORBA_TypeCode * p_item = p_typecodes;
	
	if (p_typecodes == NULL)
		return;
	
	/* delete each typecode */
	while (*p_item)
	{
		satellite_release_typecode(*p_item);
		p_item++; 
	}
	
	satellite_delete(p_typecodes);
}

/*
 * the caller is responsible for calling TypeCodeList_release() 
 * on the return value!
 */
CORBA_TypeCode * OperationType_GetExceptionList(OperationType * pOperation)
{
	int i;
	IDL_tree exception_list = IDL_OP_DCL(pOperation->mIdlInfo.mType).raises_expr;
	CORBA_TypeCode * p_typecodes = NULL;
	
	p_typecodes = orbit_new_n(CORBA_TypeCode, 
														IDL_list_length(exception_list) + 1);

	/* the list data is of type IDL_IDENT */
	for (i = 0; exception_list != NULL; i++)
	{
		/* get typecode for exception */
		p_typecodes[i] = 
			orbit_idl_tree_type_to_typecode(IDL_LIST(exception_list).data);
		
		exception_list = IDL_LIST(exception_list).next;
	}

	/* set typecode for last element */
	p_typecodes[i] = CORBA_OBJECT_NIL;

	return p_typecodes;

#if 0
error:
	TypeCodeList_release(p_typecodes);
	return NULL;
#endif
}


	
CORBA_boolean ParameterType_GetNext(ParameterType * pParameter)
{
	if (ParameterType_IsValid(pParameter))
	{
		pParameter->mList = IDL_LIST(pParameter->mList).next;
		return pParameter->mList != NULL;
	}
	else
		return FALSE;	
}

CORBA_boolean ParameterType_IsValid(ParameterType * pParameter)
{
	return pParameter && pParameter->mList;
}

/* get typecode for parameter */
CORBA_TypeCode ParameterType_GetType(ParameterType * pParameter)
{
	return orbit_idl_tree_type_to_typecode(
			IDL_PARAM_DCL(IDL_LIST(pParameter->mList).data).param_type_spec);
}

/* return CORBA_ARG_IN, CORBA_ARG_OUT or CORBA_ARG_INOUT  */ 
int ParameterType_GetPassingMode(ParameterType * pParameter)
{
	int flags = 0;
	enum IDL_param_attr attr;
	
	if (pParameter == NULL || pParameter->mList == NULL)
		return flags;

	attr = IDL_PARAM_DCL(IDL_LIST(pParameter->mList).data).attr;
	
	/* check parameter passing mode */
	switch (attr)
	{
		case IDL_PARAM_IN: 		flags = CORBA_ARG_IN; 		break;
		case IDL_PARAM_OUT: 	flags = CORBA_ARG_OUT; 		break;
		case IDL_PARAM_INOUT: flags = CORBA_ARG_INOUT;	break;
		default:
			/* should not get here! */
	}

	return flags;

}

CORBA_boolean AttributeType_IsValid(AttributeType * pAttribute)
{
	return 
		pAttribute != NULL && 
		pAttribute->mIdlInfo.mType != NULL &&
		pAttribute->mIdlInfo.mIdent != NULL;
}

/* is readonly? */
CORBA_boolean AttributeType_IsReadonly(AttributeType * pAttribute)
{
	if (AttributeType_IsValid(pAttribute))
		return IDL_ATTR_DCL(pAttribute->mIdlInfo.mType).f_readonly;
	else
		return FALSE;
}


/* get typecode for attribute */
CORBA_TypeCode AttributeType_GetType(AttributeType * pAttribute)
{
	if (AttributeType_IsValid(pAttribute))
		return orbit_idl_tree_type_to_typecode(
			IDL_ATTR_DCL(pAttribute->mIdlInfo.mType).param_type_spec);
	else
		return NULL;
}

/* get name of Attribute in true case */
char * AttributeType_GetName(AttributeType * pAttribute)
{
	if (AttributeType_IsValid(pAttribute))
		return IDL_IDENT(pAttribute->mIdlInfo.mIdent).str;
	else
		return NULL;
}

/* structure handling */
MemberType * StructType_GetFirstMember(StructType * pStruct)
{
	/* very similar to ExceptionType_GetFirstMember */
	MemberType * pMember = orbit_new(MemberType);
	pMember->mOuterList = 
		IDL_TYPE_STRUCT(pStruct->mIdlInfo.mType).member_list;
	pMember->mInnerList = 
		IDL_MEMBER(IDL_LIST(pMember->mOuterList).data).dcls;
	return pMember;
}

char * StructType_GetRepositoryId(StructType * pStruct)
{
	return IDL_IDENT(IDL_TYPE_STRUCT(pStruct->mIdlInfo.mType).ident).repo_id;
}

/* exception handling */
MemberType * ExceptionType_GetFirstMember(ExceptionType * pException)
{
	/* very similar to StructType_GetFirstMember */
	MemberType * pMember = orbit_new(MemberType);
	pMember->mOuterList = 
		IDL_EXCEPT_DCL(pException->mIdlInfo.mType).members;
	pMember->mInnerList = 
		IDL_MEMBER(IDL_LIST(pMember->mOuterList).data).dcls;
	return pMember;
}

char * ExceptionType_GetRepositoryId(ExceptionType * pException)
{
	return IDL_IDENT(IDL_EXCEPT_DCL(pException->mIdlInfo.mType).ident).repo_id;
}

/* get a typecode for this exception (used by orbit_exception_value) */
CORBA_TypeCode ExceptionType_GetTypeCode(ExceptionType * pException)
{
	return orbit_idl_tree_type_to_typecode(pException->mIdlInfo.mType);
}

/*
 * Structure and exception members
 */
CORBA_boolean MemberType_IsValid(MemberType * pMember)
{
	return pMember && pMember->mOuterList && pMember->mInnerList;
}

CORBA_boolean MemberType_GetNext(MemberType * pMember)
{
	if (pMember == NULL || pMember->mOuterList == NULL || pMember->mInnerList == NULL)
		return FALSE;
		
	pMember->mInnerList = IDL_LIST(pMember->mInnerList).next;

	/* if we're at the end of the inner list, advance the outer list */
	if (pMember->mInnerList == NULL)
	{
		pMember->mOuterList = IDL_LIST(pMember->mOuterList).next;
		if (pMember->mOuterList == NULL)
			return FALSE;

		/* initialize inner list */
		pMember->mInnerList = 
			IDL_MEMBER(IDL_LIST(pMember->mOuterList).data).dcls;
	}

	return TRUE;
}

char * MemberType_GetName(MemberType * pMember)
{
	if (pMember == NULL || pMember->mInnerList == NULL)
		return NULL;
	else
		return IDL_IDENT(IDL_LIST(pMember->mInnerList).data).str;
}

/* for struct/exception */
CORBA_TypeCode MemberType_GetType(MemberType * pMember);
/* not used because this is only cared for in typecode.c */


/*
 * Enumeration
 */

char * EnumType_GetRepositoryId(EnumType * pEnum)
{
	return IDL_IDENT(IDL_TYPE_ENUM(pEnum->mIdlInfo.mType).ident).repo_id;
}


EnumMemberType * EnumType_GetFirstMember(EnumType * pEnum)
{
	if (pEnum != NULL)
	{
		EnumMemberType * pEnumMember = orbit_new(EnumMemberType);
			
		pEnumMember->mList = 
			IDL_TYPE_ENUM(pEnum->mIdlInfo.mType).enumerator_list;
		
		return pEnumMember;
	}
	else
		return NULL;
}

CORBA_boolean EnumMemberType_IsValid(EnumMemberType * pEnumMember)
{
	return pEnumMember && pEnumMember->mList;
}

CORBA_boolean EnumMemberType_GetNext(EnumMemberType * pEnumMember)
{
	/* reuse object! */
	pEnumMember->mList = IDL_LIST(pEnumMember->mList).next;

	return pEnumMember->mList != NULL;
}

char * EnumMemberType_GetName(EnumMemberType * pEnumMember)
{
	return IDL_IDENT(IDL_LIST(pEnumMember->mList).data).str;
}


/* release types */
void AttributeType_release(AttributeType * pMember) { orbit_delete(pMember); }
void EnumMemberType_release(EnumMemberType * pMember) { orbit_delete(pMember); }
void ExceptionType_release(ExceptionType * pException) { orbit_delete(pException); }
void InterfaceType_release(InterfaceType * pInterface) { orbit_delete(pInterface); }
void MemberType_release(MemberType * pMember) { orbit_delete(pMember); }
void OperationType_release(OperationType * pOperation) { orbit_delete(pOperation); }
void ParameterType_release(ParameterType * pParameter) { orbit_delete(pParameter); }
void StructType_release(StructType * pStruct) { orbit_delete(pStruct); }


