/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of the GNU General Public License as published by |
   | the Free Software Foundation; either version 2 of the License, or    |
   | (at your option) any later version.                                  |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of the GNU General Public License    |
   | along with this program; if not, write to the Free Software          |
   | Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.            |
   +----------------------------------------------------------------------+
   | Authors: Uwe Steinmann                                               |
   |                                                                      |
   +----------------------------------------------------------------------+
 */

/* $Id$ */
#if COMPILE_DL
#include "dl/phpdl.h"
#endif

#include <stdlib.h>

#if !(WIN32|WINNT)
#include "config.h"
#endif
#include "php.h"
#include "ext/standard/head.h"

#if HYPERWAVE


#include "hw.h"

#if APACHE
#  ifndef DEBUG
#  undef palloc
#  endif
#endif

hw_module php3_hw_module;

function_entry hw_functions[] = {
	{"hw_connect",			php3_hw_connect,		NULL},
	{"hw_pconnect",			php3_hw_pconnect,		NULL},
	{"hw_close",			php3_hw_close,			NULL},
	{"hw_root",			php3_hw_root,			NULL},
	{"hw_info",			php3_hw_info,			NULL},
	{"hw_connection_info",		php3_hw_connection_info,	NULL},
	{"hw_error",			php3_hw_error,			NULL},
	{"hw_errormsg",			php3_hw_errormsg,		NULL},
	{"hw_getparentsobj",		php3_hw_getparentsobj,		NULL},
	{"hw_getparents",		php3_hw_getparents,		NULL},
	{"hw_children",			php3_hw_children,		NULL},
	{"hw_childrenobj",		php3_hw_childrenobj,		NULL},
	{"hw_getchildcoll",		php3_hw_getchildcoll,		NULL},
	{"hw_getchildcollobj",		php3_hw_getchildcollobj,	NULL},
	{"hw_getobject",		php3_hw_getobject,		NULL},
	{"hw_getandlock",		php3_hw_getandlock,		NULL},
	{"hw_unlock",			php3_hw_unlock,			NULL},
	{"hw_gettext",			php3_hw_gettext,		NULL},
	{"hw_edittext",			php3_hw_edittext,		NULL},
	{"hw_getcgi",			php3_hw_getcgi,			NULL},
	{"hw_getremote",		php3_hw_getremote,		NULL},
	{"hw_getremotechildren",	php3_hw_getremotechildren,	NULL},
	{"hw_pipedocument",		php3_hw_pipedocument,		NULL},
	{"hw_pipecgi",			php3_hw_pipecgi,		NULL},
	{"hw_insertdocument",		php3_hw_insertdocument,		NULL},
	{"hw_mv",			php3_hw_mv,			NULL},
	{"hw_cp",			php3_hw_cp,			NULL},
	{"hw_deleteobject",		php3_hw_deleteobject,		NULL},
	{"hw_changeobject",		php3_hw_changeobject,		NULL},
	{"hw_docbyanchor",		php3_hw_docbyanchor,		NULL},
	{"hw_docbyanchorobj",		php3_hw_docbyanchorobj,		NULL},
	{"hw_getobjectbyquery",		php3_hw_getobjectbyquery,	NULL},
	{"hw_getobjectbyqueryobj",	php3_hw_getobjectbyqueryobj,	NULL},
	{"hw_getobjectbyquerycoll",	php3_hw_getobjectbyquerycoll,	NULL},
	{"hw_getobjectbyquerycollobj",	php3_hw_getobjectbyquerycollobj,NULL},
	{"hw_getchilddoccoll",		php3_hw_getchilddoccoll,	NULL},
	{"hw_getchilddoccollobj",	php3_hw_getchilddoccollobj,	NULL},
	{"hw_getanchors",		php3_hw_getanchors,		NULL},
	{"hw_getanchorsobj",		php3_hw_getanchorsobj,		NULL},
	{"hw_getusername",		php3_hw_getusername,		NULL},
	{"hw_setlinkroot",		php3_hw_setlinkroot,		NULL},
	{"hw_identify",			php3_hw_identify,		NULL},
	{"hw_free_document",		php3_hw_free_document,		NULL},
	{"hw_new_document",		php3_hw_new_document,		NULL},
	{"hw_output_document",		php3_hw_output_document,	NULL},
	{"hw_outputdocument",		php3_hw_output_document,	NULL},
	{"hw_document_size",		php3_hw_document_size,		NULL},
	{"hw_documentsize",		php3_hw_document_size,		NULL},
	{"hw_document_attributes",	php3_hw_document_attributes,	NULL},
	{"hw_documentattributes",	php3_hw_document_attributes,	NULL},
	{"hw_document_bodytag",		php3_hw_document_bodytag,	NULL},
	{"hw_documentbodytag",		php3_hw_document_bodytag,	NULL},
	{"hw_document_content",		php3_hw_document_content,	NULL},
	{"hw_objrec2array",		php3_hw_objrec2array,		NULL},
	{"hw_array2objrec",		php3_hw_array2objrec,		NULL},
	{"hw_incollections",		php3_hw_incollections,		NULL},
	{"hw_inscoll",			php3_hw_inscoll,		NULL},
	{"hw_insertobject",		php3_hw_insertobject,		NULL},
	{"hw_insdoc",			php3_hw_insdoc,			NULL},
	{"hw_getsrcbydestobj",		php3_hw_getsrcbydestobj,	NULL},
	{"hw_getrellink",		php3_hw_getrellink,		NULL},
	{"hw_who",			php3_hw_who,			NULL},
	{"hw_stat",			php3_hw_stat,			NULL},
	{"hw_dummy",			php3_hw_dummy,			NULL},
	{NULL, NULL, NULL}
};

php3_module_entry hw_module_entry = {
	"HyperWave", hw_functions, php3_minit_hw, NULL, NULL, NULL, php3_info_hw, 0, 0, 0, NULL
};

void print_msg(hg_msg *msg, char *str, int txt);

#if COMPILE_DL
DLEXPORT php3_module_entry *get_module(void) { return &hw_module_entry; }
#endif

void _close_hw_link(hw_connection *conn)
{
	if(conn->hostname)
		free(conn->hostname);
	if(conn->username)
		free(conn->username);
	close(conn->socket);
	free(conn);
	php3_hw_module.num_links--;
}

void _close_hw_plink(hw_connection *conn)
{
	if(conn->hostname)
		free(conn->hostname);
	if(conn->username)
		free(conn->username);
	close(conn->socket);
	free(conn);
	php3_hw_module.num_links--;
	php3_hw_module.num_persistent--;
}

void _free_hw_document(hw_document *doc)
{
	if(doc->data)
		free(doc->data);
	if(doc->attributes)
		free(doc->attributes);
	if(doc->bodytag)
		free(doc->bodytag);
	free(doc);
}

/* creates an array in return value and frees all memory
 * Also adds as an assoc. array at the end of the return array with
 * statistics.
 */
int make_return_objrec(pval **return_value, char **objrecs, int count)
{
	pval stat_arr;
	int i;
	int hidden, collhead, fullcollhead, total;
        int collheadnr, fullcollheadnr;

	if (array_init(*return_value) == FAILURE) {
		/* Ups, failed! Let's at least free the memory */
		for(i=0; i<count; i++)
			efree(objrecs[i]);
		efree(objrecs);
		return -1;
	}

	hidden = collhead = fullcollhead = total = 0;
        collheadnr = fullcollheadnr = -1;
	for(i=0; i<count; i++) {
		/* Fill the array with entries. No need to free objrecs[i], since
		 * it is not duplicated in add_next_index_string().
		 */
		if(NULL != objrecs[i]) {
			if(0 == fnAttributeCompare(objrecs[i], "PresentationHints", "Hidden"))
				hidden++;
			if(0 == fnAttributeCompare(objrecs[i], "PresentationHints", "CollectionHead")) {
				collhead++;
				collheadnr = total;
			}
			if(0 == fnAttributeCompare(objrecs[i], "PresentationHints", "FullCollectionHead")) {
				fullcollhead++;
				fullcollheadnr = total;
			}
			total++;
			add_next_index_string(*return_value, objrecs[i], 0);
		}
	}
	efree(objrecs);

	/* Array for statistics */
	if (array_init(&stat_arr) == FAILURE) {
		return -1;
	}

	add_assoc_long(&stat_arr, "Hidden", hidden);
	add_assoc_long(&stat_arr, "CollectionHead", collhead);
	add_assoc_long(&stat_arr, "FullCollectionHead", fullcollhead);
	add_assoc_long(&stat_arr, "Total", total);
	add_assoc_long(&stat_arr, "CollectionHeadNr", collheadnr);
	add_assoc_long(&stat_arr, "FullCollectionHeadNr", fullcollheadnr);

	/* Add the stat array */
	_php3_hash_next_index_insert((*return_value)->value.ht, &stat_arr, sizeof(pval), NULL);

	/* The title array can now be freed, but I don't know how */
	return 0;
}

/*
** creates an array return value from object record
*/
int make_return_array_from_objrec(pval **return_value, char *objrec) {
	pval title_arr, desc_arr;
	char *attrname, *str, *temp, language[3], *title;
	int iTitle, iDesc;
	int hasTitle = 0;
	int hasDescription = 0;

	if (array_init(*return_value) == FAILURE) {
		(*return_value)->type = IS_STRING;
		(*return_value)->value.str.val = empty_string;
		(*return_value)->value.str.len = 0;
		return -1;
	}

	/* Array for titles. Only if we have at least one title */
/*	if(0 == strncmp(objrec, "Title=", 6)) { */
		if (array_init(&title_arr) == FAILURE) {
			return -1;
		}
		hasTitle = 1;
/*	} */

	/* Array for Descriptions. Only if we have at least one description */
/*	if(0 == strncmp(objrec, "Description=", 12)) { */
		if (array_init(&desc_arr) == FAILURE) {
			return -1;
		}
		hasDescription = 1;
/*	} */

	/* Fill Array of titles and descriptions */
	temp = estrdup(objrec);
	attrname = strtok(temp, "\n");
	while(attrname != NULL) {
		str = attrname;
		iTitle = 0;
		iDesc = 0;
		if(0 == strncmp(attrname, "Title=", 6)) {
			str += 6;
			iTitle = 1;
		} else if(0 == strncmp(attrname, "Description=", 12)) {
			str += 12;
			iDesc = 1;
		}
		if(iTitle || iDesc) {			/* Poor error check if end of string */
			if(str[2] == ':') {
				str[2] = '\0';
				strcpy(language, str);
				str += 3;
			} else
				strcpy(language, "xx");

			title = str;
/*			while((*str != '=') && (*str != '\0'))
				str++;
			*str = '\0';
*/			if(iTitle)
				add_assoc_string(&title_arr, language, title, 1);
			else
				add_assoc_string(&desc_arr, language, title, 1);
		}
		attrname = strtok(NULL, "\n");
	}
	efree(temp);

	/* Add the title array, if we have one */
	if(hasTitle) {
		_php3_hash_update((*return_value)->value.ht, "Title", 6, &title_arr, sizeof(pval), NULL);

		/* The title array can now be freed, but I don't know how */
	}

	if(hasDescription) {
	/* Add the description array, if we have one */
		_php3_hash_update((*return_value)->value.ht, "Description", 12, &desc_arr, sizeof(pval), NULL);

		/* The description array can now be freed, but I don't know how */
	}

	/* All other attributes. Make a another copy first */
	temp = estrdup(objrec);
	attrname = strtok(temp, "\n");
	while(attrname != NULL) {
		str = attrname;
		/* We don't want to insert titles a second time */
		if((0 != strncmp(attrname, "Title=", 6)) &&
		   (0 != strncmp(attrname, "Description=", 12))) {
			while((*str != '=') && (*str != '\0'))
				str++;
			*str = '\0';
			str++;
			add_assoc_string(*return_value, attrname, str, 1);
		}
		attrname = strtok(NULL, "\n");
	}
	efree(temp);

	return(0);
}

static char * make_objrec_from_array(HashTable *lht) {
	int i, count, keytype;
	ulong length;
	char *key, str[1024], *objrec = NULL;
	pval *keydata;

	if(NULL == lht)
		return NULL;

	if(0 == (count = _php3_hash_num_elements(lht)))
		return NULL;
	
	_php3_hash_internal_pointer_reset(lht);
	objrec = malloc(1);
	*objrec = '\0';
	for(i=0; i<count; i++) {
		keytype = _php3_hash_get_current_key(lht, &key, &length);
		if(HASH_KEY_IS_STRING == keytype) {
			_php3_hash_get_current_data(lht, (void **) &keydata);
			switch(keydata->type) {
				case IS_STRING:
					sprintf(str, "%s=%s\n", key, keydata->value.str.val);
					break;
				case IS_LONG:
					sprintf(str, "%s=0x%lX\n", key, keydata->value.lval);
					break;
			}
			efree(key);
			objrec = realloc(objrec, strlen(objrec)+strlen(str)+1);
			strcat(objrec, str);
		}
		_php3_hash_move_forward(lht);
	}
	return objrec;
}

static int * make_ints_from_array(HashTable *lht) {
	int i, count;
	int *objrec = NULL;
	pval *keydata;

	if(NULL == lht)
		return NULL;

	if(0 == (count = _php3_hash_num_elements(lht)))
		return NULL;
	
	_php3_hash_internal_pointer_reset(lht);
	if(NULL == (objrec = emalloc(count*sizeof(int))))
		return NULL;
	for(i=0; i<count; i++) {
		_php3_hash_get_current_data(lht, (void **) &keydata);
		switch(keydata->type) {
			case IS_LONG:
				objrec[i] = keydata->value.lval;
				break;
			default:
				objrec[i] = 0;
		}
		_php3_hash_move_forward(lht);
	}
	return objrec;
}

int php3_minit_hw(INIT_FUNC_ARGS) {

	if (cfg_get_long("hw.allow_persistent",&php3_hw_module.allow_persistent)==FAILURE) {
		php3_hw_module.allow_persistent=1;
	}
	if (cfg_get_long("hw.max_persistent",&php3_hw_module.max_persistent)==FAILURE) {
		php3_hw_module.max_persistent=-1;
	}
	if (cfg_get_long("hw.max_links",&php3_hw_module.max_links)==FAILURE) {
		php3_hw_module.max_links=-1;
	}
	php3_hw_module.num_persistent=0;
	php3_hw_module.le_socketp = register_list_destructors(_close_hw_link,NULL);
	php3_hw_module.le_psocketp = register_list_destructors(NULL,_close_hw_plink);
	php3_hw_module.le_document = register_list_destructors(_free_hw_document,NULL);

	return SUCCESS;
}

static void php3_hw_do_connect(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	pval *argv[4];
	int argc;
	int sockfd;
	int port = 0;
	char *host = NULL;
	char *userdata = NULL;
	char *server_string = NULL;
	char *username = NULL;
	char *password = NULL;
	char *hashed_details;
	char *str = NULL;
	char buffer[20];
	int hashed_details_length;
	hw_connection *ptr;
	int do_swap;
	int version = 0;
	TLS_VARS;
	
	argc = ARG_COUNT(ht);
	switch(argc) {
		case 2:
			if (getParametersArray(ht, argc, argv) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		case 4:
			if (getParametersArray(ht, argc, argv) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	/* Host: */
	convert_to_string(argv[0]);
	host = (char *) estrndup(argv[0]->value.str.val,argv[0]->value.str.len);

	/* Port: */
	convert_to_long(argv[1]);
	port = argv[1]->value.lval;

	/* Username and Password */
	if(argc > 2) {
		/* Username */
		convert_to_string(argv[2]);
		username = (char *) estrndup(argv[2]->value.str.val,argv[2]->value.str.len);
		/* Password */
		convert_to_string(argv[3]);
		password = (char *) estrndup(argv[3]->value.str.val,argv[3]->value.str.len);
	}

	/* Create identifier string for connection */
	sprintf(buffer, "%d", port);
	hashed_details_length = strlen(host)+strlen(buffer)+8;
	if(NULL == (hashed_details = (char *) emalloc(hashed_details_length+1))) {
		if(host) efree(host);
		if(password) efree(password);
		if(username) efree(username);
		php3_error(E_ERROR, "Could not get memory for connection details");
		RETURN_FALSE;
	}
	sprintf(hashed_details,"hw_%s_%d",host,port);

	if (persistent) {
		list_entry *le;

		/* try to find if we already have this link in our persistent list */
		if (_php3_hash_find(plist, hashed_details, hashed_details_length+1, (void **) &le)==FAILURE) {
			list_entry new_le;

			if (php3_hw_module.max_links!=-1 && php3_hw_module.num_links>=php3_hw_module.max_links) {
				php3_error(E_ERROR,"Hyperwave:  Too many open links (%d)",php3_hw_module.num_links);
				if(host) efree(host);
				if(username) efree(username);
				if(password) efree(password);
				efree(hashed_details);
				RETURN_FALSE;
			}
			if (php3_hw_module.max_persistent!=-1 && php3_hw_module.num_persistent>=php3_hw_module.max_persistent) {
				php3_error(E_ERROR,"Hyperwave: Too many open persistent links (%d)",php3_hw_module.num_persistent);
				if(host) efree(host);
				if(username) efree(username);
				if(password) efree(password);
				efree(hashed_details);
				RETURN_FALSE;
			}

			if ( (sockfd = open_hg_connection(host, port)) < 0 )  {
				php3_error(E_ERROR, "Could not open connection to %s, Port: %d (retval=%d)", host, port, sockfd);
				if(host) efree(host);
				if(username) efree(username);
				if(password) efree(password);
				efree(hashed_details);
				RETURN_FALSE;
				}
	
			if(NULL == (ptr = malloc(sizeof(hw_connection)))) {
				php3_error(E_ERROR, "Could not get memory for connection structure");
				if(host) efree(host);
				if(username) efree(username);
				if(password) efree(password);
				efree(hashed_details);
				RETURN_FALSE;
			}
	
			if(0 != (ptr->lasterror = initialize_hg_connection(sockfd, &do_swap, &version, &userdata, &server_string, username, password))) {
				php3_error(E_ERROR, "Could not initalize hyperwave connection");
				if(host) efree(host);
				if(username) efree(username);
				if(password) efree(password);
				if(userdata) efree(userdata);
				if(server_string) free(server_string);
				efree(hashed_details);
				RETURN_FALSE;
				}

			if(username) efree(username);
			if(password) efree(password);
	
			ptr->version = version;
			ptr->server_string = server_string;
			ptr->socket = sockfd;
			ptr->swap_on = do_swap;
			ptr->linkroot = 0;
			ptr->hostname = strdup(host);
			ptr->username = strdup("anonymous");
	
			new_le.ptr = (void *) ptr;
			new_le.type = php3_hw_module.le_psocketp;;

			if (_php3_hash_update(plist,hashed_details,hashed_details_length+1,(void *) &new_le, sizeof(list_entry), NULL)==FAILURE) {
				php3_error(E_ERROR, "Could not hash table with connection details");
				if(host) efree(host);
				if(username) efree(username);
				if(password) efree(password);
				if(server_string) free(server_string);
				efree(hashed_details);
				RETURN_FALSE;
			}

			php3_hw_module.num_links++;
			php3_hw_module.num_persistent++;
		} else {
			/*php3_printf("Found already open connection\n"); */
			if (le->type != php3_hw_module.le_psocketp) {
				RETURN_FALSE;
			}
			ptr = le->ptr;
		}

		return_value->value.lval = php3_list_insert(ptr,php3_hw_module.le_psocketp);
		return_value->type = IS_LONG;
	
	} else {
		list_entry *index_ptr,new_index_ptr;

		/* first we check the hash for the hashed_details key.  if it exists,
		 * it should point us to the right offset where the actual hyperwave link sits.
		 * if it doesn't, open a new hyperwave link, add it to the resource list,
		 * and add a pointer to it with hashed_details as the key.
		 */
		if (_php3_hash_find(list,hashed_details,hashed_details_length+1,(void **) &index_ptr)==SUCCESS) {
			int type,link;
			void *ptr;
	
			if (index_ptr->type != le_index_ptr) {
				RETURN_FALSE;
			}
			link = (int) index_ptr->ptr;
			ptr = (hw_connection *) php3_list_find(link,&type);   /* check if the link is still there */
			if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
				return_value->value.lval = php3_hw_module.default_link = link;
				return_value->type = IS_LONG;
				efree(hashed_details);
				if(username) efree(username);
				if(password) efree(password);
	  		if(host) efree(host);
				return;
			} else {
				_php3_hash_del(list,hashed_details,hashed_details_length+1);
			}
		}
	
		if ( (sockfd = open_hg_connection(host, port)) < 0 )  {
			php3_error(E_ERROR, "Could not open connection to %s, Port: %d (retval=%d", host, port, sockfd);
		  if(host) efree(host);
			if(username) efree(username);
			if(password) efree(password);
			efree(hashed_details);
			RETURN_FALSE;
			}
	
		if(NULL == (ptr = malloc(sizeof(hw_connection)))) {
			if(host) efree(host);
			if(username) efree(username);
			if(password) efree(password);
			efree(hashed_details);
			RETURN_FALSE;
		}
	
		if(0 != (ptr->lasterror = initialize_hg_connection(sockfd, &do_swap, &version, &userdata, &server_string, username, password))) {
			php3_error(E_ERROR, "Could not initalize hyperwave connection");
			if(host) efree(host);
			if(username) efree(username);
			if(password) efree(password);
			if(userdata) efree(userdata);
			if(server_string) free(server_string);
			efree(hashed_details);
			RETURN_FALSE;
			}

		if(username) efree(username);
		if(password) efree(password);
	
		ptr->version = version;
		ptr->server_string = server_string;
		ptr->socket = sockfd;
		ptr->swap_on = do_swap;
		ptr->linkroot = 0;
		ptr->hostname = strdup(host);
		ptr->username = strdup("anonymous");
	
		return_value->value.lval = php3_list_insert(ptr,php3_hw_module.le_socketp);
		return_value->type = IS_LONG;
	
		new_index_ptr.ptr = (void *) return_value->value.lval;
		new_index_ptr.type = le_index_ptr;
		if (_php3_hash_update(list,hashed_details,hashed_details_length+1,(void *) &new_index_ptr, sizeof(list_entry), NULL)==FAILURE) {
			php3_error(E_ERROR, "Could not update connection details in hash table");
			if(host) efree(host);
			efree(hashed_details);
			RETURN_FALSE;
		}
	
	}

	efree(hashed_details);
	if(host) efree(host);
	php3_hw_module.default_link=return_value->value.lval;

	/* At this point we have a working connection. If userdata was given
	   we are also indentified.
	   If there is no userdata because hw_connect was called without username
	   and password, we don't evaluate userdata.
	*/
	if(NULL == userdata)
		return;

	if(ptr->username) free(ptr->username);
	str = userdata;
	while((*str != 0) && (*str != ' '))
		str++;
	if(*str != '\0')
		ptr->username = strdup(++str);
	else
		ptr->username = NULL;
	efree(userdata);
}

/* Start of user level functions */
/* ***************************** */
/* {{{ proto int hw_connect(string host, int port [string username [, string password]])
   Connect to the Hyperwave server */
void php3_hw_connect(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_hw_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
}
/* }}} */

/* {{{ proto int hw_pconnect(string host, int port [string username [, string password]])
   Connect to the Hyperwave server persistent */
void php3_hw_pconnect(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_hw_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);
}
/* }}} */

/* {{{ proto void hw_close(int link)
   Close connection to Hyperwave server */
void php3_hw_close(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	id=arg1->value.lval;
	ptr = php3_list_find(id,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}
	php3_list_delete(id);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void hw_info(int link)
   Outputs info string */
void php3_hw_info(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg1;
	int id, type;
	hw_connection *ptr;
	char *str;
	TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	id=arg1->value.lval;
	ptr = php3_list_find(id,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}
	if(NULL != (str = get_hw_info(ptr))) {
		/*
		php3_printf("%s\n", str);
		efree(str);
		*/
		return_value->value.str.len = strlen(str);
		return_value->value.str.val = str;
		return_value->type = IS_STRING;
		return;
		}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int hw_error(int link)
   Returns last error number */
void php3_hw_error(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg1;
	int id, type;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	id=arg1->value.lval;
	ptr = php3_list_find(id,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}
	RETURN_LONG(ptr->lasterror);
}
/* }}} */

/* {{{ proto string hw_errormsg(int link)
   Returns last error message */
void php3_hw_errormsg(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg1;
	int id, type;
	hw_connection *ptr;
	char errstr[100];
	TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	id=arg1->value.lval;
	ptr = php3_list_find(id,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	switch(ptr->lasterror) {
		case 0:
			sprintf(errstr, "No error");
			break;
		case NOACCESS:
			sprintf(errstr, "No access");
			break;
		case NODOCS:
			sprintf(errstr, "No documents");
			break;
		case NONAME:
			sprintf(errstr, "No collection name");
			break;
		case NODOC:
			sprintf(errstr, "Object is not a document");
			break;
		case NOOBJ:
			sprintf(errstr, "No object received");
			break;
		case NOCOLLS:
			sprintf(errstr, "No collections received");
			break;
		case DBSTUBNG:
			sprintf(errstr, "Connection to low-level database failed");
			break;
		case NOTFOUND:
			sprintf(errstr, "Object not found");
			break;
		case EXIST:
			sprintf(errstr, "Collection already exists");
			break;
		case FATHERDEL:
			sprintf(errstr, "parent collection disappeared");
			break;
		case FATHNOCOLL:
			sprintf(errstr, "parent collection not a collection");
			break;
		case NOTEMPTY:
			sprintf(errstr, "Collection not empty");
			break;
		case DESTNOCOLL:
			sprintf(errstr, "Destination not a collection");
			break;
		case SRCEQDEST:
			sprintf(errstr, "Source equals destination");
			break;
		case REQPEND:
			sprintf(errstr, "Request pending");
			break;
		case TIMEOUT:
			sprintf(errstr, "Timeout");
			break;
		case NAMENOTUNIQUE:
			sprintf(errstr, "Name not unique");
			break;
		case WRITESTOPPED:
			sprintf(errstr, "Database now read-only; try again later");
			break;
		case LOCKED:
			sprintf(errstr, "Object locked; try again later");
			break;
		default:
			sprintf(errstr, "Unknown error: %d", ptr->lasterror);
		}
	RETURN_STRING(errstr, 1);
}
/* }}} */

/* {{{ proto hw_root(void)
   Returns object id of root collection */
void php3_hw_root(INTERNAL_FUNCTION_PARAMETERS)
{
	TLS_VARS;
	return_value->value.lval = 0;
	return_value->type = IS_LONG;
}
/* }}} */

char *php3_hw_command(INTERNAL_FUNCTION_PARAMETERS, int comm) {
	pval *arg1;
	int link, type;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		return NULL;
	}
	convert_to_long(arg1);
	link=arg1->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",link);
		return NULL;
	}

	set_swap(ptr->swap_on);
	{
	char *object = NULL;
	if (0 != (ptr->lasterror = send_command(ptr->socket, comm, &object)))
		return NULL;

	return object;
	}
}

/* {{{ proto string hw_stat(int link)
   Returns status string */
void php3_hw_stat(INTERNAL_FUNCTION_PARAMETERS) {
        char *object;

	object = php3_hw_command(INTERNAL_FUNCTION_PARAM_PASSTHRU, STAT_COMMAND);
	if(object == NULL)
		RETURN_FALSE;

	return_value->value.str.val = object;
	return_value->value.str.len = strlen(object);
	return_value->type = IS_STRING;
}
/* }}} */

/* {{{ proto array hw_who(int link)
   Returns names and info of users loged in */
void php3_hw_who(INTERNAL_FUNCTION_PARAMETERS) {
	pval user_arr;
        char *object, *ptr, *temp, *attrname;
	int i;

	object = php3_hw_command(INTERNAL_FUNCTION_PARAM_PASSTHRU, WHO_COMMAND);
	if(object == NULL)
		RETURN_FALSE;

	ptr = object;

        while((*ptr != '\0') && (*ptr != '\n'))
		ptr++;
        while((*ptr != '\0') && (*ptr != '\n'))
		ptr++;
	if(*ptr == '\0') {
		efree(object);
		RETURN_FALSE;
	}

	if (array_init(return_value) == FAILURE) {
		efree(object);
		RETURN_FALSE;
	}

	temp = estrdup(ptr);
	attrname = strtok(temp, "\n");
	i = 0;
	while(attrname != NULL) {
		char *name;

		if (array_init(&user_arr) == FAILURE) {
			efree(object);
			RETURN_FALSE;
		}

		ptr = attrname;
		if(*ptr == '*')
			add_assoc_long(&user_arr, "self", 1);
		else
			add_assoc_long(&user_arr, "self", 0);
			
		ptr++;
		name = ptr;
		while((*ptr != '\0') && (*ptr != ' '))
			ptr++;
		*ptr = '\0';
		add_assoc_string(&user_arr, "id", name, 1);

		ptr++;
		name = ptr;
		while((*ptr != '\0') && (*ptr != ' '))
			ptr++;
		*ptr = '\0';
		add_assoc_string(&user_arr, "name", name, 1);

		ptr++;
		while((*ptr != '\0') && (*ptr == ' '))
			ptr++;

		name = ptr;
		while((*ptr != '\0') && (*ptr != ' '))
			ptr++;
		*ptr = '\0';
		add_assoc_string(&user_arr, "system", name, 1);

		ptr++;
		while((*ptr != '\0') && (*ptr == ' '))
			ptr++;

		name = ptr;
		while((*ptr != '\0') && (*ptr != ' '))
			ptr++;
		*ptr = '\0';
		add_assoc_string(&user_arr, "onSinceDate", name, 1);

		ptr++;
		while((*ptr != '\0') && (*ptr == ' '))
			ptr++;

		name = ptr;
		while((*ptr != '\0') && (*ptr != ' '))
			ptr++;
		*ptr = '\0';
		add_assoc_string(&user_arr, "onSinceTime", name, 1);

		ptr++;
		while((*ptr != '\0') && (*ptr == ' '))
			ptr++;

		name = ptr;
		while((*ptr != '\0') && (*ptr != ' '))
			ptr++;
		*ptr = '\0';
		add_assoc_string(&user_arr, "TotalTime", name, 1);

		/* Add the user array */
		_php3_hash_index_update(return_value->value.ht, i++, &user_arr, sizeof(pval), NULL);

		/* The user array can now be freed, but I don't know how */

		attrname = strtok(NULL, "\n");
	}
	efree(temp);
	efree(object);

}
/* }}} */

void php3_hw_dummy(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3;
	int link, id, type, msgid;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	convert_to_long(arg3);
	link=arg1->value.lval;
	id=arg2->value.lval;
	msgid=arg3->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	{
	char *object = NULL;
	if (0 != (ptr->lasterror = send_dummy(ptr->socket, id, msgid, &object)))
		RETURN_FALSE;

php3_printf("%s", object);
	return_value->value.str.val = object;
	return_value->value.str.len = strlen(object);
	return_value->type = IS_STRING;
	}
}

/* {{{ proto string hw_getobject(int link, int objid)
   Returns object record  */
void php3_hw_getobject(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int link, id, type;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	link=arg1->value.lval;
	id=arg2->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	{
	char *object = NULL;
	if (0 != (ptr->lasterror = send_getobject(ptr->socket, id, &object)))
		RETURN_FALSE;

	RETURN_STRING(object, 0);
	/*
	make_return_array_from_objrec(&return_value, object);
	efree(object);
	*/
	}
}
/* }}} */

/* {{{ proto int hw_insertobject(int link, string objrec, string parms)
   Inserts an object */
void php3_hw_insertobject(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3;
	int link, type;
	char *objrec, *parms;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_string(arg2);
	convert_to_string(arg3);
	link=arg1->value.lval;
	objrec=arg2->value.str.val;
	parms=arg3->value.str.val;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",link);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	{
	int objid;
	if (0 != (ptr->lasterror = send_insertobject(ptr->socket, objrec, parms, &objid)))
		RETURN_FALSE;

	RETURN_LONG(objid);
	}
}
/* }}} */

/* {{{ proto string hw_getandlock(int link, int objid)
   Returns object record and locks object */
void php3_hw_getandlock(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int link, id, type;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	link=arg1->value.lval;
	id=arg2->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	{
	char *object = NULL;
	char *attrname, *str;
	if (0 != (ptr->lasterror = send_getandlock(ptr->socket, id, &object)))
		RETURN_FALSE;

	RETURN_STRING(object, 0);
	}
}
/* }}} */

/* {{{ proto void hw_unlock(int link, int objid)
   Unlocks object */
void php3_hw_unlock(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int link, id, type;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	link=arg1->value.lval;
	id=arg2->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_unlock(ptr->socket, id)))
		RETURN_FALSE;

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void hw_deleteobject(int link, int objid)
   Deletes object */
void php3_hw_deleteobject(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int link, id, type;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	link=arg1->value.lval;
	id=arg2->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_deleteobject(ptr->socket, id)))
		RETURN_FALSE;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void hw_changeobject(int link, int objid, array attributes)
   Changes attributes of an object */
void php3_hw_changeobject(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3;
	int link, id, type, i;
	hw_connection *ptr;
	char *modification, *oldobjrec, buf[200];
	HashTable *newobjarr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1); /* Connection */
	convert_to_long(arg2); /* object ID */
	convert_to_array(arg3); /* Array with new attributes */
	link=arg1->value.lval;
	id=arg2->value.lval;
	newobjarr=arg3->value.ht;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	/* get the old object record */
	if(0 != (ptr->lasterror = send_getobject(ptr->socket, id, &oldobjrec)))
		RETURN_FALSE;

	_php3_hash_internal_pointer_reset(newobjarr);
	modification = strdup("");
	for(i=0; i<_php3_hash_num_elements(newobjarr); i++) {
		char *key, *str, *str1, newattribute[200];
		pval *data;
		int j, noinsert=1;
		ulong ind;

		_php3_hash_get_current_key(newobjarr, &key, &ind);
		_php3_hash_get_current_data(newobjarr, (void *) &data);
		switch(data->type) {
			case IS_STRING:
				if(strlen(data->value.str.val) == 0)
					noinsert = 0;
				else
					sprintf(newattribute, "%s=%s", key, data->value.str.val);
				break;
			default:
				sprintf(newattribute, "%s", "");
		}

		if(!noinsert) {
			modification = fnInsStr(modification, 0, "\\");
			modification = fnInsStr(modification, 0, newattribute);
			modification = fnInsStr(modification, 0, "add ");

			/* Retrieve the old attribute from object record */
			if(NULL != (str = strstr(oldobjrec, key))) {
				str1 = str;
				j = 0;
				while((str1 != NULL) && (*str1 != '\n')) {
					buf[j++] = *str1++;
				}
				buf[j] = '\0';
				modification = fnInsStr(modification, 0, "\\");
				modification = fnInsStr(modification, 0, buf);
				modification = fnInsStr(modification, 0, "rem ");
			} 
		}
		efree(key);
		_php3_hash_move_forward(newobjarr);
	}
	efree(oldobjrec);

	set_swap(ptr->swap_on);
	modification[strlen(modification)-1] = '\0';
/*	php3_printf("0x%X, %s", id, modification); */
	if (0 != (ptr->lasterror = send_changeobject(ptr->socket, id, modification)))
		RETURN_FALSE;
	free(modification);
	RETURN_TRUE;
}
/* }}} */

void php3_hw_mvcp(INTERNAL_FUNCTION_PARAMETERS, int mvcp) {
	pval *arg1, *arg2, *arg3, *arg4, **objvIDs;
	int link, type, dest=0, from=0;
	HashTable *src_arr;
	hw_connection *ptr;
	int collIDcount, docIDcount, i, *docIDs, *collIDs;
	TLS_VARS;

	switch(mvcp) {
		case MOVE: /* Move also has fromID */
			if (ARG_COUNT(ht) != 4 || getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE)
				WRONG_PARAM_COUNT;
			break;
		case COPY:
			if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE)
				WRONG_PARAM_COUNT;
			break;
	}
	convert_to_long(arg1);
	convert_to_array(arg2);
	convert_to_long(arg3);
	link=arg1->value.lval;
	src_arr=arg2->value.ht;
	switch(mvcp) {
		case MOVE: /* Move also has fromID, which is arg3 --> arg4 becomes destID */
			convert_to_long(arg4);
			from=arg3->value.lval;
			dest=arg4->value.lval;
			break;
		case COPY: /* No fromID for Copy needed --> arg3 is destID */
			dest=arg3->value.lval;
			from = 0;
			break;
	}
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",link);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);

	if(NULL == (objvIDs = emalloc(_php3_hash_num_elements(src_arr) * sizeof(pval *)))) {
		RETURN_FALSE;
		}

	if(getParametersArray(src_arr, _php3_hash_num_elements(src_arr), objvIDs) == FAILURE) {
		RETURN_FALSE;
		}

	if(NULL == (collIDs = emalloc(_php3_hash_num_elements(src_arr) * sizeof(int)))) {
		efree(objvIDs);
		RETURN_FALSE;
		}

	if(NULL == (docIDs = emalloc(_php3_hash_num_elements(src_arr) * sizeof(int)))) {
		efree(objvIDs);
		efree(collIDs);
		RETURN_FALSE;
		}

	collIDcount = docIDcount = 0;
	for(i=0; i<_php3_hash_num_elements(src_arr); i++) {
		char *objrec;
		if(objvIDs[i]->type == IS_LONG) {
			if(0 != (ptr->lasterror = send_getobject(ptr->socket, objvIDs[i]->value.lval, &objrec))) {
				efree(objvIDs);
				efree(collIDs);
				efree(docIDs);
				RETURN_FALSE;
			}
			if(0 == fnAttributeCompare(objrec, "DocumentType", "collection"))
				collIDs[collIDcount++] = objvIDs[i]->value.lval;
			else
				docIDs[docIDcount++] = objvIDs[i]->value.lval;
			efree(objrec);
		}
	}
	efree(objvIDs);

	if (0 != (ptr->lasterror = send_mvcpdocscoll(ptr->socket, docIDs, docIDcount, from, dest, mvcp))) {
		efree(collIDs);
		efree(docIDs);
		RETURN_FALSE;
	}

	if (0 != (ptr->lasterror = send_mvcpcollscoll(ptr->socket, collIDs, collIDcount, from, dest, mvcp))) {
		efree(collIDs);
		efree(docIDs);
		RETURN_FALSE;
	}

	efree(collIDs);
	efree(docIDs);

	RETURN_LONG(docIDcount + collIDcount);
}

/* {{{ proto void hw_mv(int link, array objrec, int from, int dest)
   Moves object */
void php3_hw_mv(INTERNAL_FUNCTION_PARAMETERS) {
	php3_hw_mvcp(INTERNAL_FUNCTION_PARAM_PASSTHRU, MOVE);
}
/* }}} */

/* {{{ proto void hw_cp(int link, array objrec, int dest)
   Copies object */
void php3_hw_cp(INTERNAL_FUNCTION_PARAMETERS) {
	php3_hw_mvcp(INTERNAL_FUNCTION_PARAM_PASSTHRU, COPY);
}
/* }}} */

/* {{{ proto hwdoc hw_gettext(int link, int objid[, int rootid])
   Returns text document. Links are relative to rootid if given */
void php3_hw_gettext(INTERNAL_FUNCTION_PARAMETERS) {
	pval *argv[3];
	int argc, link, id, type, mode;
	int rootid = 0;
	hw_document *doc;
	hw_connection *ptr;
	TLS_VARS;

	argc = ARG_COUNT(ht);
	if((argc > 3) || (argc < 2))
		WRONG_PARAM_COUNT;
		
	if (getParametersArray(ht, argc, argv) == FAILURE)
		RETURN_FALSE;

	convert_to_long(argv[0]);
	convert_to_long(argv[1]);
	if(argc == 3) {
		convert_to_long(argv[2]);
		rootid = argv[2]->value.lval;
		mode = 1;
	} else
		mode = 0;
	link=argv[0]->value.lval;
	id=argv[1]->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	{
	char *object = NULL;
	char *attributes = NULL;
	char *bodytag = NULL;
	int count;
	/* !!!! memory for object and attributes is allocated with malloc !!!! */
	if (0 != (ptr->lasterror = send_gettext(ptr->socket, id, mode, rootid, &attributes, &bodytag, &object, &count)))
		RETURN_FALSE;
	doc = malloc(sizeof(hw_document));
	doc->data = object;
	doc->attributes = attributes;
	doc->bodytag = bodytag;
	doc->size = count;
	return_value->value.lval = php3_list_insert(doc,php3_hw_module.le_document);
	return_value->type = IS_LONG;
	}
}
/* }}} */

/* {{{ proto void hw_edittext(int link, hwdoc doc)
   Modifies text document */
void php3_hw_edittext(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int link, id, doc, type;
	hw_connection *ptr;
	hw_document *docptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	link=arg1->value.lval;
	ptr = php3_list_find(link,&type);

	/* FIXME id is not set anywhere */
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	doc=arg2->value.lval;
	docptr = php3_list_find(doc,&type);

	/* FIXME id is not set anywhere */
	if(!docptr || (type!=php3_hw_module.le_document)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	{
	if (0 != (ptr->lasterror =  send_edittext(ptr->socket, docptr->attributes, docptr->data))) {
		RETURN_FALSE;
		}
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto hwdoc hw_getcgi(int link, int objid)
   Returns the output of a cgi script */
void php3_hw_getcgi(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int link, id, type;
	hw_document *doc;
	hw_connection *ptr;
	char cgi_env_str[1000];
	TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	link=arg1->value.lval;
	id=arg2->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	{
	char *object = NULL;
	char *attributes = NULL;
	int count;

	/* Here is another undocument function of Hyperwave.
	   If you call a cgi script with getcgi-message, you will
	   have to provide the complete cgi enviroment, since it is
	   only known to the webserver (or wavemaster). This is done
	   by extending the object record with the following incomplete
	   string. It should contain any enviroment variable a cgi script
	   requires.
	*/
#if (WIN32|WINNT)
	sprintf(cgi_env_str, "CGI_REQUEST_METHOD=%s\nCGI_PATH_INFO=%s\nCGI_QUERY_STRING=%s",
	                     getenv("REQUEST_METHOD"),
	                     getenv("PATH_INFO"),
	                     getenv("QUERY_STRING"));
#else
	sprintf(cgi_env_str, "CGI_REQUEST_METHOD=%s\nCGI_PATH_INFO=%s\nCGI_QUERY_STRING=%s",
	                     GLOBAL(request_info).request_method,
	                     GLOBAL(request_info).path_info,
	                     GLOBAL(request_info).query_string);
#endif
	/* !!!! memory for object and attributes is allocated with malloc !!!! */
	if (0 != (ptr->lasterror = send_getcgi(ptr->socket, id, cgi_env_str, &attributes, &object, &count)))
		RETURN_FALSE;
	doc = malloc(sizeof(hw_document));
	doc->data = object;
	doc->attributes = attributes;
	doc->bodytag = NULL;
	doc->size = count;
	return_value->value.lval = php3_list_insert(doc,php3_hw_module.le_document);
	return_value->type = IS_LONG;
	}
}
/* }}} */

/* {{{ proto hwdoc hw_getremote(int link, int objid)
   Returns the output of a remote document */
void php3_hw_getremote(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int link, id, type;
	hw_document *doc;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	link=arg1->value.lval;
	id=arg2->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	{
	char *object = NULL;
	char *attributes = NULL;
	int count;
	/* !!!! memory for object and attributes is allocated with malloc !!!! */
	if (0 != (ptr->lasterror = send_getremote(ptr->socket, id, &attributes, &object, &count)))
		RETURN_FALSE;
	doc = malloc(sizeof(hw_document));
	doc->data = object;
	doc->attributes = attributes;
	doc->bodytag = NULL;
	doc->size = count;
	return_value->value.lval = php3_list_insert(doc,php3_hw_module.le_document);
	return_value->type = IS_LONG;
	}
}
/* }}} */

/* {{{ proto [array|hwdoc] hw_getremotechildren(int link, int objid)
   Returns the remote document if only one or an array of object records */
void php3_hw_getremotechildren(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int link, type, i;
	hw_connection *ptr;
	char *objrec;
	TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_string(arg2);
	link=arg1->value.lval;
	objrec=arg2->value.str.val;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d", link);
		RETURN_FALSE;
	}
	set_swap(ptr->swap_on);
	{
	int count, *offsets;
	char *remainder, *ptr1;
	if (0 != (ptr->lasterror = send_getremotechildren(ptr->socket, objrec, &remainder, &offsets, &count)))
		RETURN_FALSE;

	if(strncmp(remainder, "ObjectID=0 ", 10)) {
		hw_document *doc;
		remainder[offsets[0]-18] = '\0';
/*php3_printf("offset = %d, remainder = %s---", offsets[0], remainder);*/
		doc = malloc(sizeof(hw_document));
		doc->data = strdup(remainder);
		doc->attributes = strdup(objrec);
		doc->bodytag = NULL;
		doc->size = offsets[0]-18;
		return_value->value.lval = php3_list_insert(doc,php3_hw_module.le_document);
		return_value->type = IS_LONG;
	} else {
		if (array_init(return_value) == FAILURE) {
			efree(offsets);
			RETURN_FALSE;
		}

		ptr1 = remainder;
		for(i=0; i<count; i++) {
			*(ptr1+offsets[i]-1) = '\0';
			add_index_string(return_value, i, ptr1, 1);
			ptr1 += offsets[i];
		}
	}

	efree(offsets);
	efree(remainder);
	}
}
/* }}} */

/* {{{ proto void hw_setlinkroot(int link, int rootid)
   Set the id to which links are calculated */
void php3_hw_setlinkroot(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int link, type, rootid;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	link = arg1->value.lval;
	rootid = arg2->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",link);
		RETURN_FALSE;
	}

	ptr->linkroot = rootid;
	RETURN_LONG(rootid);
}
/* }}} */

/* {{{ proto hwdoc hw_pipedocument(int link, int objid)
   Returns document */
void php3_hw_pipedocument(INTERNAL_FUNCTION_PARAMETERS) {
	pval *argv[3];
	int link, id, type, argc, mode;
	int rootid = 0;
	hw_connection *ptr;
	hw_document *doc;
#if APACHE
	server_rec *serv = GLOBAL(php3_rqst)->server;
#endif
	TLS_VARS;

	argc = ARG_COUNT(ht);
	if((argc > 2) || (argc < 2))
		WRONG_PARAM_COUNT;
		
	if (getParametersArray(ht, argc, argv) == FAILURE)
		RETURN_FALSE;

	convert_to_long(argv[0]);
	convert_to_long(argv[1]);
/*	if(argc == 3) {
		convert_to_long(argv[2]);
		rootid = argv[2]->value.lval;
		if(rootid != 0)
			mode = 1;
	}
*/	link=argv[0]->value.lval;
	id=argv[1]->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d", link);
		RETURN_FALSE;
	}

	mode = 0;
	if(ptr->linkroot > 0)
		mode = 1;
	rootid = ptr->linkroot;

	set_swap(ptr->swap_on);
	{
	char *object = NULL;
	char *attributes = NULL;
	char *bodytag = NULL;
	int count;
	/* !!!! memory for object, bodytag and attributes is allocated with malloc !!!! */
	if (0 != (ptr->lasterror =  send_pipedocument(ptr->socket,
#if APACHE
  serv->server_hostname,
#else
  getenv("HOST"),
#endif
   id, mode, rootid, &attributes, &bodytag, &object, &count)))
		RETURN_FALSE;

	doc = malloc(sizeof(hw_document));
	doc->data = object;
	doc->attributes = attributes;
	doc->bodytag = bodytag;
	doc->size = count;
fprintf(stderr, "size = %d\n", count);
	return_value->value.lval = php3_list_insert(doc,php3_hw_module.le_document);
	return_value->type = IS_LONG;
	}
}  /* }}} */

/* {{{ proto hwdoc hw_pipecgi(int link, int objid)
   Returns output of cgi script */
void php3_hw_pipecgi(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int link, id, type;
	hw_connection *ptr;
	hw_document *doc;
	char cgi_env_str[1000];
#if APACHE
	server_rec *serv = GLOBAL(php3_rqst)->server;
#endif
	TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	link=arg1->value.lval;
	id=arg2->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	{
	char *object = NULL;
	char *attributes = NULL;
	int count;

#if (WIN32|WINNT)
	sprintf(cgi_env_str, "CGI_REQUEST_METHOD=%s\nCGI_PATH_INFO=%s\nCGI_QUERY_STRING=%s",
	                     getenv("REQUEST_METHOD"),
	                     getenv("PATH_INFO"),
	                     getenv("QUERY_STRING"));
#else
	sprintf(cgi_env_str, "CGI_REQUEST_METHOD=%s\nCGI_PATH_INFO=%s\nCGI_QUERY_STRING=%s",
	                     GLOBAL(request_info).request_method,
	                     GLOBAL(request_info).path_info,
	                     GLOBAL(request_info).query_string);
#endif
	/* !!!! memory for object, bodytag and attributes is allocated with malloc !!!! */
	if (0 != (ptr->lasterror =  send_pipecgi(ptr->socket,
#if APACHE
  serv->server_hostname,
#else
  getenv("HOST"),
#endif
  id, cgi_env_str, &attributes, &object, &count)))
		RETURN_FALSE;

	doc = malloc(sizeof(hw_document));
	doc->data = object;
	doc->attributes = attributes;
	doc->bodytag = NULL;
	doc->size = count;
	return_value->value.lval = php3_list_insert(doc,php3_hw_module.le_document);
	return_value->type = IS_LONG;
	}
}  /* }}} */

/* {{{ proto void hw_insertdocument(int link, int parentid, hwdoc doc) 
   Insert new document */
void php3_hw_insertdocument(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3;
	int link, id, doc, type;
	hw_connection *ptr;
	hw_document *docptr;
#if APACHE
	server_rec *serv = GLOBAL(php3_rqst)->server;
#endif
	TLS_VARS;

	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	convert_to_long(arg3);
	link=arg1->value.lval;
	id=arg2->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find connection identifier %d",link);
		RETURN_FALSE;
	}

	doc=arg3->value.lval;
	docptr = php3_list_find(doc,&type);
	if(!docptr || (type!=php3_hw_module.le_document)) {
		php3_error(E_WARNING,"Unable to find document identifier %d",doc);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	{
	if (0 != (ptr->lasterror =  send_putdocument(ptr->socket,
#if APACHE
  serv->server_hostname,
#else
  getenv("HOST"),
#endif
             id, docptr->attributes, docptr->data, docptr->size))) {
		RETURN_FALSE;
		}
	}
	RETURN_TRUE;
}  /* }}} */

/* {{{ proto hwdoc hw_new_document(int link, string data, string objrec, int size)
   Create a new document */
void php3_hw_new_document(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3;
	hw_document *doc;

	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string(arg1);
	convert_to_string(arg2);
	convert_to_long(arg3);

	doc = malloc(sizeof(hw_document));
	doc->data = malloc(arg3->value.lval);
        memcpy(doc->data, arg2->value.str.val, arg3->value.lval);
	doc->attributes = strdup(arg1->value.str.val);
	doc->bodytag = NULL;
	doc->size = arg3->value.lval;
	return_value->value.lval = php3_list_insert(doc,php3_hw_module.le_document);
	return_value->type = IS_LONG;
}  /* }}} */

/* {{{ proto void hw_free_document(hwdoc doc)
   Frees memory of document */
void php3_hw_free_document(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	hw_document *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	id=arg1->value.lval;
	ptr = php3_list_find(id,&type);
	if(!ptr || (type!=php3_hw_module.le_document)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}
	php3_list_delete(id);
	RETURN_TRUE;
}  /* }}} */

/* {{{ proto void hw_output_document(hwdoc doc)
   Prints document */
void php3_hw_output_document(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type, count;
	hw_document *ptr;
	char *ptr1;
	TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	id=arg1->value.lval;
	ptr = php3_list_find(id,&type);
	if(!ptr || (type!=php3_hw_module.le_document)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	ptr1 = ptr->data;
	count = 0;
	while(count < ptr->size) {
		php3_write(ptr1++, 1);
		count++;
	}

	RETURN_TRUE;
}  /* }}} */

/* {{{ proto string hw_document_bodytag(hwdoc doc [, string prefix])
   Return bodytag prefixed by prefix */
void php3_hw_document_bodytag(INTERNAL_FUNCTION_PARAMETERS) {
	pval *argv[2];
	int id, type, argc;
	hw_document *ptr;
	char *temp, *str = NULL;
	TLS_VARS;

	argc = ARG_COUNT(ht);
	if((argc > 2) || (argc < 1))
		WRONG_PARAM_COUNT;
		
	if (getParametersArray(ht, argc, argv) == FAILURE)
		RETURN_FALSE;
	
	convert_to_long(argv[0]);
	id=argv[0]->value.lval;
	ptr = php3_list_find(id,&type);
	if(!ptr || (type!=php3_hw_module.le_document)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	if(argc == 2) {
		convert_to_string(argv[1]);
		str=argv[1]->value.str.val;
	}

	if(str != NULL) {
		temp = emalloc(argv[1]->value.str.len + strlen(ptr->bodytag) + 2);
		strcpy(temp, ptr->bodytag);
		strcpy(temp+strlen(ptr->bodytag)-1, str);
		strcpy(temp+strlen(ptr->bodytag)-1+argv[1]->value.str.len, ">\n");
		RETURN_STRING(temp, 0);
	} else {
		RETURN_STRING(ptr->bodytag, 1);
	}
}  /* }}} */

/* {{{ proto string hw_document_content(hwdoc doc)
   Returns content of document */
void php3_hw_document_content(INTERNAL_FUNCTION_PARAMETERS) {
	pval *argv[1];
	int id, type, argc;
	hw_document *ptr;
	TLS_VARS;

	argc = ARG_COUNT(ht);
	if(argc != 1)
		WRONG_PARAM_COUNT;
		
	if (getParametersArray(ht, argc, argv) == FAILURE)
		RETURN_FALSE;
	
	convert_to_long(argv[0]);
	id=argv[0]->value.lval;
	ptr = php3_list_find(id,&type);
	if(!ptr || (type!=php3_hw_module.le_document)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	RETURN_STRING(ptr->data, 1);
}  /* }}} */

/* {{{ proto int hw_document_content(hwdoc doc)
   Returns size of document */
void php3_hw_document_size(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	hw_document *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	id=arg1->value.lval;
	ptr = php3_list_find(id,&type);
	if(!ptr || (type!=php3_hw_module.le_document)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	RETURN_LONG(ptr->size);
}  /* }}} */

/* {{{ proto string hw_document_content(hwdoc doc)
   Returns object record of document */
void php3_hw_document_attributes(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	hw_document *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	id=arg1->value.lval;
	ptr = php3_list_find(id,&type);
	if(!ptr || (type!=php3_hw_module.le_document)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	RETURN_STRING(ptr->attributes, 1);
/*	make_return_array_from_objrec(&return_value, ptr->attributes); */
}  /* }}} */

/* {{{ proto array hw_getparentsobj(int link, int objid)
   Returns array of parent object records */
void php3_hw_getparentsobj(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int link, id, type;
	int count;
	char  **childObjRecs = NULL;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	link=arg1->value.lval;
	id=arg2->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);

	if (0 != (ptr->lasterror = send_getparentsobj(ptr->socket, id, &childObjRecs, &count))) {
		php3_error(E_WARNING, "send_command (getparentsobj) returned -1\n");
		RETURN_FALSE;
	}

	/* create return value and free all memory */
	if( 0 > make_return_objrec(&return_value, childObjRecs, count))
		RETURN_FALSE;
}  /* }}} */

/* {{{ proto array hw_getparents(int link, int objid)
   Returns array of parent object ids */
void php3_hw_getparents(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int link, id, type;
	int count;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	link=arg1->value.lval;
	id=arg2->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
        {
	int  *childIDs = NULL;
	int i;

	if (0 != (ptr->lasterror = send_getparents(ptr->socket, id, &childIDs, &count))) {
		php3_error(E_WARNING, "send_command (getparents) returned -1\n");
		RETURN_FALSE;
	}

	if (array_init(return_value) == FAILURE) {
		efree(childIDs);
		RETURN_FALSE;
	}

	for(i=0; i<count; i++) {
		add_index_long(return_value, i, childIDs[i]);
	}
	efree(childIDs);
	}

}  /* }}} */

/* {{{ proto array hw_children(int link, int objid)
   Returns array of children object ids */
void php3_hw_children(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int link, id, type;
	int count;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	link=arg1->value.lval;
	id=arg2->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	{
	int  *childIDs = NULL;
	int i;

	if (0 != (ptr->lasterror = send_children(ptr->socket, id, &childIDs, &count))){
		php3_error(E_WARNING, "send_command (getchildcoll) returned -1\n");
		RETURN_FALSE;
	}

	if (array_init(return_value) == FAILURE) {
		efree(childIDs);
		RETURN_FALSE;
	}

	for(i=0; i<count; i++) {
		add_index_long(return_value, i, childIDs[i]);
	}
	efree(childIDs);
	}
		
}  /* }}} */

/* {{{ proto array hw_children(int link, int objid)
   Returns array of children object records */
void php3_hw_childrenobj(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int link, id, type;
	int count;
	char  **childObjRecs = NULL;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	link=arg1->value.lval;
	id=arg2->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);

	if (0 != (ptr->lasterror = send_childrenobj(ptr->socket, id, &childObjRecs, &count))) {
		php3_error(E_WARNING, "send_command (getchildcollobj) returned -1\n");
		RETURN_FALSE;
	}

	/* create return value and free all memory */
	if( 0 > make_return_objrec(&return_value, childObjRecs, count))
		RETURN_FALSE;
}  /* }}} */

/* {{{ proto array hw_childcoll(int link, int objid)
   Returns array of child collection object ids */
void php3_hw_getchildcoll(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int link, id, type;
	int count;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	link=arg1->value.lval;
	id=arg2->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	{
	int  *childIDs = NULL;
	int i;

	if (0 != (ptr->lasterror = send_getchildcoll(ptr->socket, id, &childIDs, &count))){
		php3_error(E_WARNING, "send_command (getchildcoll) returned -1\n");
		RETURN_FALSE;
	}

	if (array_init(return_value) == FAILURE) {
		efree(childIDs);
		RETURN_FALSE;
	}

	for(i=0; i<count; i++) {
		add_index_long(return_value, i, childIDs[i]);
	}
	efree(childIDs);
	}
		
}  /* }}} */

/* {{{ proto array hw_childcollobj(int link, int objid)
   Returns array of child collection object records */
void php3_hw_getchildcollobj(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int link, id, type;
	int count;
	char  **childObjRecs = NULL;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	link=arg1->value.lval;
	id=arg2->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);

	if (0 != (ptr->lasterror = send_getchildcollobj(ptr->socket, id, &childObjRecs, &count))) {
		php3_error(E_WARNING, "send_command (getchildcollobj) returned -1\n");
		RETURN_FALSE;
	}

	/* create return value and free all memory */
	if( 0 > make_return_objrec(&return_value, childObjRecs, count))
		RETURN_FALSE;
}  /* }}} */

/* {{{ proto int hw_docbyanchor(int link, int anchorid)
   Returns objid of document belonging to anchorid */
void php3_hw_docbyanchor(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int link, id, type;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	link=arg1->value.lval;
	id=arg2->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	{
	int objectID;
	if (0 != (ptr->lasterror = send_docbyanchor(ptr->socket, id, &objectID)))
		RETURN_FALSE;

	RETURN_LONG(objectID);
	}
}  /* }}} */

/* {{{ proto array hw_docbyanchorobj(int link, int anchorid)
   Returns object record of document belonging to anchorid */
void php3_hw_docbyanchorobj(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int link, id, type;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	link=arg1->value.lval;
	id=arg2->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	{
	char *object = NULL;
	if (0 != (ptr->lasterror = send_docbyanchorobj(ptr->socket, id, &object)))
		RETURN_FALSE;

	RETURN_STRING(object, 0);
	/*
	make_return_array_from_objrec(&return_value, object);
	efree(object);
	*/
	}
}  /* }}} */

/* {{{ proto array hw_getobjectbyquery(int link, string query, int maxhits)
   Search for query and return maxhits objids */
void php3_hw_getobjectbyquery(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3;
	int link, type, maxhits;
	char *query;
	int count, i;
	int  *childIDs = NULL;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_string(arg2);
	convert_to_long(arg3);
	link=arg1->value.lval;
	query=arg2->value.str.val;
	maxhits=arg3->value.lval;
	if (maxhits < 0) maxhits=0x7FFFFFFF;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",link);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_getobjbyquery(ptr->socket, query, maxhits, &childIDs, &count))) {
		php3_error(E_WARNING, "send_command (getchildcoll) returned -1\n");
		RETURN_FALSE;
	}

	if (array_init(return_value) == FAILURE) {
		efree(childIDs);
		RETURN_FALSE;
	}

	for(i=0; i<count; i++)
		add_index_long(return_value, i, childIDs[i]);
	efree(childIDs);
}  /* }}} */

/* {{{ proto array hw_getobjectbyqueryobj(int link, string query, int maxhits)
   Search for query and return maxhits object records */
void php3_hw_getobjectbyqueryobj(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3;
	int link, type, maxhits;
	char *query;
	int count;
	char  **childObjRecs = NULL;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_string(arg2);
	convert_to_long(arg3);
	link=arg1->value.lval;
	query=arg2->value.str.val;
	maxhits=arg3->value.lval;
	if (maxhits < 0) maxhits=0x7FFFFFFF;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",link);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_getobjbyqueryobj(ptr->socket, query, maxhits, &childObjRecs, &count))) {
		php3_error(E_WARNING, "send_command (getchildcoll) returned -1\n");
		RETURN_FALSE;
	}

	/* create return value and free all memory */
	if( 0 > make_return_objrec(&return_value, childObjRecs, count))
		RETURN_FALSE;
}  /* }}} */

/* {{{ proto array hw_getobjectbyquerycoll(int link, int collid, string query, int maxhits)
   Search for query in collection and return maxhits objids */
void php3_hw_getobjectbyquerycoll(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3, *arg4;
	int link, id, type, maxhits;
	char *query;
	int count, i;
	hw_connection *ptr;
	int  *childIDs = NULL;
	TLS_VARS;

	if (ARG_COUNT(ht) != 4 || getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	convert_to_string(arg3);
	convert_to_long(arg4);
	link=arg1->value.lval;
	id=arg2->value.lval;
	query=arg3->value.str.val;
	maxhits=arg4->value.lval;
	if (maxhits < 0) maxhits=0x7FFFFFFF;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_getobjbyquerycoll(ptr->socket, id, query, maxhits, &childIDs, &count))) {
		php3_error(E_WARNING, "send_command (getchildcoll) returned -1\n");
		RETURN_FALSE;
	}

	if (array_init(return_value) == FAILURE) {
		efree(childIDs);
		RETURN_FALSE;
	}

	for(i=0; i<count; i++)
		add_index_long(return_value, i, childIDs[i]);
	efree(childIDs);
}  /* }}} */

/* {{{ proto array hw_getobjectbyquerycollobj(int link, int collid, string query, int maxhits)
   Search for query in collection and return maxhits object records */
void php3_hw_getobjectbyquerycollobj(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3, *arg4;
	int link, id, type, maxhits;
	char *query;
	int count;
	hw_connection *ptr;
	char  **childObjRecs = NULL;
	TLS_VARS;

	if (ARG_COUNT(ht) != 4 || getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	convert_to_string(arg3);
	convert_to_long(arg4);
	link=arg1->value.lval;
	id=arg2->value.lval;
	query=arg3->value.str.val;
	maxhits=arg4->value.lval;
	if (maxhits < 0) maxhits=0x7FFFFFFF;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_getobjbyquerycollobj(ptr->socket, id, query, maxhits, &childObjRecs, &count))) {
		php3_error(E_WARNING, "send_command (getchildcoll) returned -1\n");
		RETURN_FALSE;
	}

	/* create return value and free all memory */
	if( 0 > make_return_objrec(&return_value, childObjRecs, count))
		RETURN_FALSE;
}  /* }}} */

/* {{{ proto array hw_getchilddoccoll(int link, int objid)
   Returns all children ids which are documents */
void php3_hw_getchilddoccoll(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int link, id, type;
	int count, i;
	int  *childIDs = NULL;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	link=arg1->value.lval;
	id=arg2->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_getchilddoccoll(ptr->socket, id, &childIDs, &count))) {
		php3_error(E_WARNING, "send_command (getchilddoccoll) returned -1\n");
		RETURN_FALSE;
	}

	if (array_init(return_value) == FAILURE) {
		efree(childIDs);
		RETURN_FALSE;
	}

	for(i=0; i<count; i++)
		add_index_long(return_value, i, childIDs[i]);
	efree(childIDs);
}  /* }}} */

/* {{{ proto array hw_getchilddoccollobj(int link, int objid)
   Returns all children object records which are documents */
void php3_hw_getchilddoccollobj(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int link, id, type;
	int count;
	char  **childObjRecs = NULL;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	link=arg1->value.lval;
	id=arg2->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_getchilddoccollobj(ptr->socket, id, &childObjRecs, &count))) {
		php3_error(E_WARNING, "send_command (getchilddoccollobj) returned -1\n");
		RETURN_FALSE;
	}

	/* create return value and free all memory */
	if( 0 > make_return_objrec(&return_value, childObjRecs, count))
		RETURN_FALSE;

}  /* }}} */

/* {{{ proto array hw_getanchors(int link, int objid)
   Return all anchors of object */
void php3_hw_getanchors(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int link, id, type;
	int count, i;
	int  *anchorIDs = NULL;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	link=arg1->value.lval;
	id=arg2->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_getanchors(ptr->socket, id, &anchorIDs, &count))) {
		php3_error(E_WARNING, "send_command (getanchors) returned -1\n");
		RETURN_FALSE;
	}

	if (array_init(return_value) == FAILURE) {
		efree(anchorIDs);
		RETURN_FALSE;
	}

	for(i=0; i<count; i++)
		add_index_long(return_value, i, anchorIDs[i]);
	efree(anchorIDs);
}  /* }}} */

/* {{{ proto array hw_getanchorsobj(int link, int objid)
   Return all object records of anchors of object */
void php3_hw_getanchorsobj(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int link, id, type;
	int count;
	char  **anchorObjRecs = NULL;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	link=arg1->value.lval;
	id=arg2->value.lval;
	ptr = (hw_connection *) php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",id);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_getanchorsobj(ptr->socket, id, &anchorObjRecs, &count))) {
		php3_error(E_WARNING, "send_command (getanchors) returned -1\n");
		RETURN_FALSE;
	}

	/* create return value and free all memory */
	if( 0 > make_return_objrec(&return_value, anchorObjRecs, count))
		RETURN_FALSE;
}  /* }}} */

/* {{{ proto string hw_getusername(int link)
   Returns the current user name */
void php3_hw_getusername(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int link, type;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	link = arg1->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",link);
		RETURN_FALSE;
	}

	return_value->value.str.val = estrdup(ptr->username);
	return_value->value.str.len = strlen(ptr->username);
	return_value->type = IS_STRING;
}  /* }}} */


/* {{{ proto void hw_identify(int link, string username, string password)
   Identifies at Hyperwave server */
void php3_hw_identify(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3;
	int link, type;
	char *name, *passwd, *userdata;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_string(arg2);
	convert_to_string(arg3);
	link = arg1->value.lval;
	name=arg2->value.str.val;
	passwd=arg3->value.str.val;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",link);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	{
	char *str;

	if (0 != (ptr->lasterror = send_identify(ptr->socket, name, passwd, &userdata))) {
		php3_error(E_WARNING, "send_identify returned -1\n");
		if(ptr->username) free(ptr->username);
		ptr->username = NULL;
		RETURN_FALSE;
	}

	return_value->value.str.val = userdata;
	return_value->value.str.len = strlen(userdata);
	return_value->type = IS_STRING;
	if(ptr->username) free(ptr->username);
	str = userdata;
	while((*str != 0) && (*str != ' '))
		str++;
	if(*str != '\0')
		ptr->username = strdup(++str);
	else
		ptr->username = NULL;
	}
}
/* }}} */

/* {{{ proto array hw_objrec2array(string objrec)
   Returns object array of object record*/
void php3_hw_objrec2array(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg1);
	make_return_array_from_objrec(&return_value, arg1->value.str.val);
}
/* }}} */

/* {{{ proto string hw_array2objrec(array objarr)
   Returns object record of object array */
void php3_hw_array2objrec(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	char *objrec, *retobj;
	TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg1);
	objrec = make_objrec_from_array(arg1->value.ht);
	if(objrec) {
		retobj = estrdup(objrec);
		free(objrec);
		RETURN_STRING(retobj, 0);
	} else
		RETURN_FALSE;
}
/* }}} */

/* {{{ proto array hw_incollections(int link, array objids, array collids, int para)
   Returns object ids which are in collections */
void php3_hw_incollections(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3, *arg4;
	int type, link, i;
	hw_connection *ptr;
	int cobjids, ccollids, *objectIDs, *collIDs, cretids, *retIDs, retcoll;
	TLS_VARS;

	if (ARG_COUNT(ht) != 4 || getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_array(arg2);
	convert_to_array(arg3);
	convert_to_long(arg4);
	link = arg1->value.lval;
	retcoll=arg4->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",link);
		RETURN_FALSE;
	}

	cobjids = _php3_hash_num_elements(arg2->value.ht);
	if(NULL == (objectIDs = make_ints_from_array(arg2->value.ht))) {
		php3_error(E_WARNING, "Could not create Int Array from Array\n");
		RETURN_FALSE;
	}

	ccollids = _php3_hash_num_elements(arg3->value.ht);
	if(NULL == (collIDs = make_ints_from_array(arg3->value.ht))) {
		php3_error(E_WARNING, "Could not create Int Array from Array\n");
		efree(objectIDs);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_incollections(ptr->socket, retcoll,
                                                      cobjids, objectIDs,
                                                      ccollids, collIDs,
                                                      &cretids, &retIDs))) {
		if(objectIDs) efree(objectIDs);
		if(collIDs) efree(collIDs);
		RETURN_FALSE;
	}

	if(objectIDs) efree(objectIDs);
	if(collIDs) efree(collIDs);

	if (array_init(return_value) == FAILURE) {
		efree(retIDs);
		RETURN_FALSE;
	}

	for(i=0; i<cretids; i++)
		add_index_long(return_value, i, retIDs[i]);
	efree(retIDs);

}
/* }}} */

/* {{{ proto void hw_inscoll(int link, int parentid, array objarr)
   Inserts collection */
void php3_hw_inscoll(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3;
	char *objrec;
	int id, newid, type, link;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	convert_to_array(arg3);
	link = arg1->value.lval;
	id=arg2->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",link);
		RETURN_FALSE;
	}

	if(NULL == (objrec = make_objrec_from_array(arg3->value.ht))) {
		php3_error(E_WARNING, "Could not create Object Record from Array\n");
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_inscoll(ptr->socket, id, objrec, &newid))) {
		if(objrec) free(objrec);
		RETURN_FALSE;
	}

	if(objrec) free(objrec);
	RETURN_LONG(newid);
}
/* }}} */

/* {{{ proto void hw_inscoll(int link, int parentid, array objarr [, string text])
   Inserts document */
void php3_hw_insdoc(INTERNAL_FUNCTION_PARAMETERS) {
	pval *argv[4];
	char *objrec, *text;
	int id, newid, type, link, argc;
	hw_connection *ptr;
	TLS_VARS;

	argc = ARG_COUNT(ht);
	if((argc < 3) || (argc > 4))
		WRONG_PARAM_COUNT;

	if (getParametersArray(ht, argc, argv) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long(argv[0]);
	convert_to_long(argv[1]);
	convert_to_string(argv[2]);
	if(argc == 4) {
		convert_to_string(argv[3]);
		text = argv[3]->value.str.val;
	} else {
		text = NULL;
	}
	link = argv[0]->value.lval;
	id = argv[1]->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",link);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
        objrec = argv[2]->value.str.val;
	if (0 != (ptr->lasterror = send_insdoc(ptr->socket, id, objrec, text, &newid))) {
		RETURN_FALSE;
	}

	RETURN_LONG(newid);
}
/* }}} */

/* {{{ proto int hw_getsrcbydestobj(int link, int destid)
   Returns object id of source docuent by destination anchor */
void php3_hw_getsrcbydestobj(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int link, type, id;
	int count;
	char  **childObjRecs = NULL;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	link=arg1->value.lval;
	id=arg2->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",link);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_getsrcbydest(ptr->socket, id, &childObjRecs, &count))) {
		php3_error(E_WARNING, "send_command (getsrcbydest) returned -1\n");
		RETURN_FALSE;
	}

	/* create return value and free all memory */
	if( 0 > make_return_objrec(&return_value, childObjRecs, count))
		RETURN_FALSE;
}
/* }}} */

/* {{{ proto string hw_getrellink(int link, int rootid, int sourceid, int destid)
   Get link form source to dest relative to rootid */
void php3_hw_getrellink(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3, *arg4;
	int link, type;
	int rootid, destid, sourceid;
	char *anchorstr;
	hw_connection *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 4 || getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	convert_to_long(arg3);
	convert_to_long(arg4);
	link=arg1->value.lval;
	rootid=arg2->value.lval;
	sourceid=arg3->value.lval;
	destid=arg4->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",link);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = getrellink(ptr->socket, rootid, sourceid, destid, &anchorstr))) {
		php3_error(E_WARNING, "command (getrellink) returned -1\n");
		RETURN_FALSE;
	}

	RETURN_STRING(anchorstr, 0);
}
/* }}} */
	

void php3_info_hw()
{
	php3_printf("HG-CSP Version: 7.17");
}

void php3_hw_connection_info(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg1;
	hw_connection *ptr;
	int link, type;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	link=arg1->value.lval;
	ptr = php3_list_find(link,&type);
	if(!ptr || (type!=php3_hw_module.le_socketp && type!=php3_hw_module.le_psocketp)) {
		php3_error(E_WARNING,"Unable to find file identifier %d",link);
		RETURN_FALSE;
	}
	
	php3_printf("Hyperwave Info:\nhost=%s,\nserver string=%s\nversion=%d\nswap=%d\n", ptr->hostname, ptr->server_string, ptr->version, ptr->swap_on);
}

void print_msg(hg_msg *msg, char *str, int txt)
{
     char *ptr;
     int  i;

     fprintf(stdout, "\nprint_msg: >>%s<<\n", str);
     fprintf(stdout, "print_msg: length  = %d\n", msg->length);
     fprintf(stdout, "print_msg: msgid = %d\n", msg->version_msgid);
     fprintf(stdout, "print_msg: msg_type  = %d\n", msg->msg_type);
     if ( msg->length > HEADER_LENGTH )  {
          ptr = msg->buf;
          for ( i = 0; i < msg->length-HEADER_LENGTH; i++ )  {
               if ( *ptr == '\n' )
                    fprintf(stdout, "%c", *ptr++);
               else if ( iscntrl(*ptr) )
                    {fprintf(stdout, "."); ptr++;}
               else
                    fprintf(stdout, "%c", *ptr++);
          }
     }
     fprintf(stdout, "\n\n");
}

#endif

/*
 * Local variables:
 * tab-width: 4
 * End:
 */
