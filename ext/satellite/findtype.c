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
 * Used by typemanager.c and typecode.c
 */
#include <orb/orbit.h>
#include "findtype.h"

typedef struct
{
	const char * 					mpWantedId;
	IDL_tree_type		mWantedType;
	IdlInfo	mIdlInfo;
	CORBA_boolean mSuccess;
} FindTypeData;

static CORBA_boolean orbit_find_type_compare(
		FindTypeData * pData, IDL_tree ident, IDL_tree type)
{
	if (pData == NULL || ident == NULL || type == NULL)
		return TRUE;	/* continue */
	
	/* compare ids */
	if (strcmp(pData->mpWantedId, IDL_IDENT(ident).repo_id) == 0 || 
			strcasecmp(pData->mpWantedId, IDL_IDENT(ident).str) == 0)
	{
		pData->mIdlInfo.mIdent = ident;
		pData->mIdlInfo.mType = type;
		pData->mSuccess = TRUE;
		return FALSE;	/* stop */
	}

	return TRUE;
}

static CORBA_boolean orbit_find_type_helper(IDL_tree_func_data *tnfd, FindTypeData *pData)
{
	IDL_tree tree = tnfd->tree;
/*	IDL_tree real_tree = tree; */
	IDL_tree ident = NULL;
	
	/* must be any type or right type */
	if (pData->mWantedType != IDLN_ANY && pData->mWantedType != IDL_NODE_TYPE(tree))
	{
		return TRUE;	/* continue */
	}

	switch(IDL_NODE_TYPE(tree))
	{
		case IDLN_ATTR_DCL:
			/* attributes */
			/* real_tree = IDL_ATTR_DCL(tree).param_type_spec; */
			
			/* 
			 * XXX: this will only handle the FIRST 
			 * ident connected to the attribute!
			 */
			ident = IDL_LIST(IDL_ATTR_DCL(tree).simple_declarations).data;
			break;

		case IDLN_EXCEPT_DCL:
			ident = IDL_EXCEPT_DCL(tree).ident;
			
		case IDLN_INTERFACE:
			ident = IDL_INTERFACE(tree).ident;
			break;

		case IDLN_OP_DCL:
			ident = IDL_OP_DCL(tree).ident;
			break;

		case IDLN_TYPE_DCL:
			/* typedefs */
			/* real_tree = IDL_TYPE_DCL(tree).type_spec; */
			
			/* 
			 * XXX: this will only handle the FIRST 
			 * ident connected to the type!
			 */
			ident = IDL_LIST(IDL_TYPE_DCL(tree).dcls).data;
			break;

		case IDLN_TYPE_ENUM:
			ident = IDL_TYPE_ENUM(tree).ident;
			break;

		case IDLN_TYPE_STRUCT:
			ident = IDL_TYPE_STRUCT(tree).ident;
			break;

		/* TODO: handle more types! */

		default:
	}

	return orbit_find_type_compare(pData, ident, /*real_*/tree);
}

CORBA_boolean orbit_find_type(
		IDL_tree tree, const char * pWantedId, IDL_tree_type wantedType, IdlInfo * pIdlInfo)
{
	FindTypeData data;

	memset(&data, 0, sizeof(data));
	data.mpWantedId = pWantedId;
	data.mWantedType = wantedType;

	IDL_tree_walk_in_order(
			tree, 
			(IDL_tree_func)orbit_find_type_helper,
			&data);

	*pIdlInfo = data.mIdlInfo;
	
	return data.mSuccess;
}

IDL_tree orbit_find_type_simple(IDL_tree tree, const char * pWantedId)
{
	IdlInfo info;
 	CORBA_boolean success = orbit_find_type(tree, pWantedId, IDLN_ANY, &info);
	return success ? info.mType : NULL;
}

