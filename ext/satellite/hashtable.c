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
 * HashTable helpers
 */
#include "hashtable.h"
#include "zend_API.h"

#ifndef FALSE
#define FALSE	0
#endif

/*
 * duplicate a zval
 */
static zval * orbit_duplicate_zval(const zval * pValue)
{
	zval * p_new_value = NULL;

	if (pValue == NULL)
		return NULL;
	
	ALLOC_ZVAL(p_new_value);											/* allocate */
	memcpy(p_new_value, pValue, sizeof(zval));		/* raw data copy */
	zval_copy_ctor(p_new_value);									/* smart data copy */
	INIT_PZVAL(p_new_value);											/* set reference count */

	return p_new_value;
}

/*
 * store a zval in a hashtable with a key
 */
zend_bool orbit_store_by_key(HashTable * pHashTable, const char * pKey, const zval * pValue)
{
	int result = FAILURE;
	void * p_destination = NULL;
	zval * p_new_value = NULL;

	if (pHashTable == NULL || pKey == NULL || pValue == NULL)
		return FALSE;

	p_new_value = orbit_duplicate_zval(pValue);

	result = zend_hash_update(
		pHashTable,
		(char *)pKey,	/* discard const */
		strlen(pKey)+1,
		p_new_value,
		sizeof(zval),
		&p_destination
	);	
	
	return result == SUCCESS;
}

/*
 * find a zval in a hashtable with a key
 */
zval * orbit_find_by_key(HashTable * pHashTable, const char * pKey)
{
	int result = FAILURE;
	zval * p_value = NULL;

	result = zend_hash_find(
		pHashTable,
		(char *)pKey,	/* discard const */
		strlen(pKey)+1,
		(void**)&p_value
		);

	if (result != SUCCESS)
	{
		p_value = NULL;
	}

	return p_value;
}


