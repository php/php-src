/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Christian Cartus <chc@idgruppe.de>                          |
   +----------------------------------------------------------------------+
 */
 
/* $Id$ */

/* This has been built and tested on Solaris 2.6.
 * It may not compile or execute correctly on other systems.
 */

#ifndef MSVC5
#include "php_config.h"
#endif


#if HAVE_SYSVSHM

#include <errno.h>

#include "php.h"
#include "php3_var.h"
#include "php3_sysvshm.h"


function_entry sysvshm_functions[] = {
	{"shm_attach",			php3_sysvshm_attach,		NULL},
	{"shm_detach",			php3_sysvshm_detach,		NULL},
	{"shm_remove",			php3_sysvshm_remove,		NULL},
	{"shm_put_var",			php3_sysvshm_put_var,		NULL},
	{"shm_get_var",			php3_sysvshm_get_var,		NULL},
	{"shm_remove_var",		php3_sysvshm_remove_var,	NULL},
	{NULL, NULL, NULL}
};

php3_module_entry sysvshm_module_entry = {
	"System V Shared-Memory", sysvshm_functions, php3_minit_sysvshm, NULL, NULL, NULL, NULL, STANDARD_MODULE_PROPERTIES
};


#if COMPILE_DL
php3_module_entry *get_module() { return &sysvshm_module_entry; }
#endif


THREAD_LS sysvshm_module php3_sysvshm_module;



static void php3i_release_sysvshm(sysvshm_shm *shm_ptr) {

	shmdt((void*)shm_ptr->ptr);
}



int php3_minit_sysvshm(INIT_FUNC_ARGS)
{    
	php3_sysvshm_module.le_shm = register_list_destructors(php3i_release_sysvshm, NULL);
	if (cfg_get_long("sysvshm.init_mem",
                      &php3_sysvshm_module.init_mem)==FAILURE) {
		php3_sysvshm_module.init_mem=10000;
    	}	
	return SUCCESS;
}


/* {{{ proto int shm_attach(int key, int size, int flag)
   Return an id for the shared memory with the given key. */
void php3_sysvshm_attach(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg_key,*arg_size,*arg_flag;
	long shm_size,shm_flag;
	sysvshm_shm *shm_list_ptr;
	char *shm_ptr;
	sysvshm_chunk_head *chunk_ptr;
	key_t shm_key;
	long shm_id,list_id;

	shm_flag = 0666;
	shm_size = php3_sysvshm_module.init_mem;
	
	switch (ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &arg_key)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(arg_key);
			shm_key = arg_key->value.lval;
			break;
		case 2:
			if (getParameters(ht, 2, &arg_key, &arg_size)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(arg_key);
			shm_key = arg_key->value.lval;
			convert_to_long(arg_size);
			shm_size=arg_size->value.lval;
			break;
		case 3:
			if (getParameters(ht, 3, &arg_key, &arg_size, &arg_flag)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(arg_key);
			shm_key = arg_key->value.lval;
			convert_to_long(arg_size);
			shm_size=arg_size->value.lval;
			convert_to_long(arg_flag);
			shm_flag = arg_flag->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}


	if((shm_list_ptr = (sysvshm_shm *) emalloc(sizeof(sysvshm_shm)))==NULL) {
		php3_error(E_WARNING, "shm_attach() failed for key 0x%x: cannot allocate internal listelement", shm_key);
		RETURN_FALSE;
	}

	/* get the id from a specified key or create new shared memory */
	if((shm_id=shmget(shm_key,0,0))<0) {
		if(shm_size<sizeof(sysvshm_chunk_head)) {
			php3_error(E_WARNING, "shm_attach() failed for key 0x%x: memorysize too small", shm_key);
			RETURN_FALSE;
		}
		if((shm_id=shmget(shm_key,shm_size,shm_flag|IPC_CREAT|IPC_EXCL))<0) {
			php3_error(E_WARNING, "shmget() failed for key 0x%x: %s", shm_key, strerror(errno));
			RETURN_FALSE;
		}
	}

	if((shm_ptr = shmat(shm_id,NULL,0))==NULL) {
		php3_error(E_WARNING, "shmget() failed for key 0x%x: %s", shm_key, strerror(errno));
		RETURN_FALSE;
	}

	/* check if shm is already initialized */
	chunk_ptr= (sysvshm_chunk_head*) shm_ptr;
	if(strcmp((char*)&(chunk_ptr->magic),"PHP3SM")!=0) {
		strcpy((char*)&(chunk_ptr->magic),"PHP3SM");	
		chunk_ptr->start=sizeof(sysvshm_chunk_head);
		chunk_ptr->end=chunk_ptr->start;
		chunk_ptr->total=shm_size;
		chunk_ptr->free=shm_size-chunk_ptr->end;
	}


	shm_list_ptr->key   = shm_key;
	shm_list_ptr->id = shm_id;
	shm_list_ptr->ptr = chunk_ptr;
	list_id = php3_list_insert(shm_list_ptr, php3_sysvshm_module.le_shm);
	RETURN_LONG(list_id);
}
/* }}} */



/* {{{ proto int shm_detach(int id)
   releases the shared memory attachment with the given id. */
void php3_sysvshm_detach(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg_id;
	long id;
	sysvshm_shm *shm_list_ptr;
	int type;


	switch (ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &arg_id)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(arg_id);
			id = arg_id->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	shm_list_ptr = (sysvshm_shm *) php3_list_find(id, &type);
	if (type!=php3_sysvshm_module.le_shm) {
		php3_error(E_WARNING, "%d is not a SysV shared memory index", id);
		RETURN_FALSE;
	}

	if(shmdt((void*)shm_list_ptr->ptr)<0) {
		php3_error(E_WARNING, "shm_detach() failed for id 0x%x: %s", id, strerror(errno));
		RETURN_FALSE;
	}
}
/* }}} */





/* {{{ proto int shm_remove(int key)
   removes the shared memory with the given key. */
void php3_sysvshm_remove(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg_key;
	long id;
	key_t key;


	switch (ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &arg_key)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(arg_key);
			key = arg_key->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}


	if((id=shmget(key,0,0))<0) {
		php3_error(E_WARNING, "%d is not a existing SysV shared memory key", key);
		RETURN_FALSE;
	}
	if(shmctl(id,IPC_RMID,NULL)<0) {
		php3_error(E_WARNING, "shm_remove() failed for key 0x%x: %s", key, strerror(errno));
		RETURN_FALSE;
	} 
	RETURN_TRUE;
}
/* }}} */



/* {{{ proto int shm_put(int id, int key, object *variable)
   insert a variable into shared memory. */
void php3_sysvshm_put_var(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg_id, *arg_key, *arg_var;
	long key, id;
	sysvshm_shm *shm_list_ptr;
	int type;
	pval shm_var;
	int ret;	

	switch (ARG_COUNT(ht)) {
		case 3:
			if (getParameters(ht, 3, &arg_id, &arg_key,&arg_var)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(arg_id);
			id = arg_id->value.lval;
			convert_to_long(arg_key);
			key = arg_key->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	shm_list_ptr = (sysvshm_shm *) php3_list_find(id, &type);
	if (type!=php3_sysvshm_module.le_shm) {
		php3_error(E_WARNING, "%d is not a SysV shared memory index", id);
		RETURN_FALSE;
	}

	/* setup string-variable and serialize */
	shm_var.type=IS_STRING;
	shm_var.value.str.len=0;
	shm_var.value.str.val=emalloc(1);
	shm_var.value.str.val[0]=0;
	php3api_var_serialize(&shm_var,arg_var);
	/* insert serialized variable into shared memory */
	ret=php3int_put_shmdata(shm_list_ptr->ptr,key,shm_var.value.str.val,shm_var.value.str.len);

	/* free string */
	efree(shm_var.value.str.val);
	
	if(ret==-1) {
		php3_error(E_WARNING, "not enough shared memory left");
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */




/* {{{ proto string/float/int/array shm_get_var(int id, int key)
   returns a variable into shared memory. */
void php3_sysvshm_get_var(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg_id, *arg_key;
	long key, id;
	sysvshm_shm *shm_list_ptr;
	int type;
	char *shm_data;	
	long shm_varpos;
	sysvshm_chunk *shm_var;
	
	switch (ARG_COUNT(ht)) {
		case 2:
			if (getParameters(ht, 2, &arg_id, &arg_key)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(arg_id);
			id = arg_id->value.lval;
			convert_to_long(arg_key);
			key = arg_key->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	shm_list_ptr = (sysvshm_shm *) php3_list_find(id, &type);
	if (type!=php3_sysvshm_module.le_shm) {
		php3_error(E_WARNING, "%d is not a SysV shared memory index", id);
		RETURN_FALSE;
	}

	/* setup string-variable and serialize */
	/* get serialized variable from shared memory */
	shm_varpos=php3int_check_shmdata((shm_list_ptr->ptr),key);

	if(shm_varpos<0) {
		php3_error(E_WARNING, "variable key %d doesn't exist", key);
		RETURN_FALSE;
	}
	shm_var=(sysvshm_chunk*)((char*)shm_list_ptr->ptr+shm_varpos);
	shm_data=&shm_var->mem;
	
	if(php3api_var_unserialize(return_value, &shm_data, shm_data+shm_var->length)!=1) {
		php3_error(E_WARNING, "variable data in shared memory is corruped");
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int shm_remove_var(int id, int key)
   removes variable from shared memory. */
void php3_sysvshm_remove_var(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg_id, *arg_key;
	long key, id;
	sysvshm_shm *shm_list_ptr;
	int type;
	long shm_varpos;
	
	switch (ARG_COUNT(ht)) {
		case 2:
			if (getParameters(ht, 2, &arg_id, &arg_key)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(arg_id);
			id = arg_id->value.lval;
			convert_to_long(arg_key);
			key = arg_key->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	shm_list_ptr = (sysvshm_shm *) php3_list_find(id, &type);
	if (type!=php3_sysvshm_module.le_shm) {
		php3_error(E_WARNING, "%d is not a SysV shared memory index", id);
		RETURN_FALSE;
	}

	shm_varpos=php3int_check_shmdata((shm_list_ptr->ptr),key);

	if(shm_varpos<0) {
		php3_error(E_WARNING, "variable key %d doesn't exist", key);
		RETURN_FALSE;
	}
	php3int_remove_shmdata((shm_list_ptr->ptr),shm_varpos);	
	RETURN_TRUE;
}
/* }}} */







/* inserts an ascii-string into shared memory */
int php3int_put_shmdata(sysvshm_chunk_head *ptr,long key,char *data, long len) {
	sysvshm_chunk* shm_var;
	long total_size;
	long shm_varpos;	

	total_size=((long)(len+sizeof(sysvshm_chunk)-1)/4)*4+4;    /* 4-byte alligment */
	
	if(ptr->free<total_size) {
		return -1;   /* not enough memeory */
	}

	if((shm_varpos=php3int_check_shmdata(ptr,key))>0) {
		php3int_remove_shmdata(ptr, shm_varpos);	
	}
	shm_var=(sysvshm_chunk*)((char *)ptr+ptr->end);	
	shm_var->key=key;
	shm_var->length=len;
	shm_var->next=total_size;   
	memcpy(&(shm_var->mem),data,len);	
	ptr->end+=total_size;
	ptr->free-=total_size;
	return 0;
}


long php3int_check_shmdata(sysvshm_chunk_head *ptr, long key) {
	long pos;
	sysvshm_chunk *shm_var;

	pos=ptr->start;
			
	for(;;) {
		if(pos>=ptr->end) {
			return -1;
		}
		shm_var=(sysvshm_chunk*)((char*)ptr+pos);
		if(shm_var->key==key) {
			return pos;
		}	
		pos+=shm_var->next;
	}
	return -1;
}


int php3int_remove_shmdata(sysvshm_chunk_head *ptr, long shm_varpos) {
	sysvshm_chunk *chunk_ptr, *next_chunk_ptr;
	long memcpy_len;
	
	chunk_ptr=(sysvshm_chunk*)((char*)ptr+shm_varpos);
	next_chunk_ptr=(sysvshm_chunk*)((char*)ptr+shm_varpos+chunk_ptr->next);
	
	memcpy_len=ptr->end-shm_varpos-chunk_ptr->next;
	ptr->free+=chunk_ptr->next;
	ptr->end-=chunk_ptr->next;
	if(memcpy_len>0)
		memcpy(chunk_ptr,next_chunk_ptr,memcpy_len);
	return 0;
}



#endif /* HAVE_SYSVSHM */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
