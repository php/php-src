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

#ifndef __typemanager_h__
#define __typemanager_h__

#include <orb/orbit.h>

typedef struct _TypeManager TypeManager;
typedef struct _InterfaceType InterfaceType;
typedef struct _StructType StructType;
typedef struct _EnumType EnumType;
typedef struct _ExceptionType ExceptionType;
typedef struct _OperationType OperationType;
typedef struct _AttributeType AttributeType;
typedef struct _MemberType MemberType;
typedef struct _ParameterType ParameterType;
typedef struct _EnumMemberType EnumMemberType;

/* type manager */
CORBA_boolean TypeManager_Init(const char * pIdlDirectory);
CORBA_boolean TypeManager_Shutdown();

CORBA_boolean TypeManager_LoadFile(const char * pFilename);
InterfaceType * TypeManager_FindInterface(/*TypeManager * pTypeManager, */
		const char * pRepositoryId);
StructType * TypeManager_FindStruct(/*TypeManager * pTypeManager, */
		const char * pRepositoryId);
EnumType * TypeManager_FindEnum(/*TypeManager * pTypeManager, */
		const char * pRepositoryId);
ExceptionType * TypeManager_FindException(/*TypeManager * pTypeManager, */
		const char * pRepositoryId);

/* interface */
OperationType * InterfaceType_FindOperation(InterfaceType * pInterface, 
		const char * name);
AttributeType * InterfaceType_FindAttribute(InterfaceType * pInterface, 
		const char * name);

/* operation */
ParameterType * OperationType_GetFirstParameter(OperationType * pOperation);
CORBA_TypeCode OperationType_GetReturnType(OperationType * pOperation);
char * OperationType_GetName(OperationType * pOperation);
CORBA_TypeCode * OperationType_GetExceptionList(OperationType * pOperation);

/* parameter */
CORBA_boolean ParameterType_GetNext(ParameterType * pParameter);
int ParameterType_GetPassingMode(ParameterType * pParameter);
CORBA_TypeCode ParameterType_GetType(ParameterType * pParameter);
CORBA_boolean ParameterType_IsValid(ParameterType * pParameter);

/* attribute */
char * AttributeType_GetName(AttributeType * pAttribute);
CORBA_TypeCode AttributeType_GetType(AttributeType * pAttribute);
CORBA_boolean AttributeType_IsReadonly(AttributeType * pAttribute);
CORBA_boolean AttributeType_IsValid(AttributeType * pAttribute);

/* struct */
MemberType * StructType_GetFirstMember(StructType * pStruct);
char * StructType_GetRepositoryId(StructType * pStruct);

/* exception */
MemberType * ExceptionType_GetFirstMember(ExceptionType * pException);
char * ExceptionType_GetRepositoryId(ExceptionType * pException);
CORBA_TypeCode ExceptionType_GetTypeCode(ExceptionType * pException);

/* member */
CORBA_boolean MemberType_GetNext(MemberType * pMember);
char * MemberType_GetName(MemberType * pMember);
CORBA_TypeCode MemberType_GetType(MemberType * pMember);
CORBA_boolean MemberType_IsValid(MemberType * pMember);

/* enum */
EnumMemberType * EnumType_GetFirstMember(EnumType * pEnum);
CORBA_boolean EnumMemberType_GetNext(EnumMemberType * pEnumMember);
CORBA_boolean EnumMemberType_IsValid(EnumMemberType * pEnumMember);
char * EnumMemberType_GetName(EnumMemberType * pEnumMember);
char * EnumType_GetRepositoryId(EnumType * pEnum);

/* release types */
void AttributeType_release(AttributeType * pMember);
void EnumMemberType_release(EnumMemberType * pMember);
void ExceptionType_release(ExceptionType * pException);
void InterfaceType_release(InterfaceType * pInterface);
void MemberType_release(MemberType * pMember);
void OperationType_release(OperationType * pOperation);
void ParameterType_release(ParameterType * pParameter);
void StructType_release(StructType * pStruct);
void TypeCodeList_release(CORBA_TypeCode * p_typecodes);

#endif /* __typemanager_h__ */

