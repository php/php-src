/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Uwe Steinmann <Uwe.Steinmann@fernuni-hagen.de>               |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include <stdlib.h>
#include <errno.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_globals.h"
#include "ext/standard/php_standard.h"
#include "ext/standard/head.h"
#include "ext/standard/info.h"
#include "fopen_wrappers.h"
#include "SAPI.h"

#ifdef PHP_WIN32
#include <winsock.h>
#endif

#if HYPERWAVE

#include "php_ini.h"
#include "php_hyperwave.h"

static int le_socketp, le_psocketp, le_document;

/*hw_module php_hw_module;*/

#define HW_ATTR_NONE	1
#define HW_ATTR_LANG	2
#define HW_ATTR_NR	3

function_entry hw_functions[] = {
	PHP_FE(hw_connect,								NULL)
	PHP_FE(hw_pconnect,								NULL)
	PHP_FE(hw_close,								NULL)
	PHP_FE(hw_root,									NULL)
	PHP_FE(hw_info,									NULL)
	PHP_FE(hw_connection_info,						NULL)
	PHP_FE(hw_error,								NULL)
	PHP_FE(hw_errormsg,								NULL)
	PHP_FE(hw_getparentsobj,						NULL)
	PHP_FE(hw_getparents,							NULL)
	PHP_FE(hw_children,								NULL)
	PHP_FE(hw_childrenobj,							NULL)
	PHP_FE(hw_getchildcoll,							NULL)
	PHP_FE(hw_getchildcollobj,						NULL)
	PHP_FE(hw_getobject,							NULL)
	PHP_FE(hw_getandlock,							NULL)
	PHP_FE(hw_unlock,								NULL)
	PHP_FE(hw_gettext,								NULL)
	PHP_FE(hw_edittext,								NULL)
	PHP_FE(hw_getcgi,								NULL)
	PHP_FE(hw_getremote,							NULL)
	PHP_FE(hw_getremotechildren,					NULL)
	PHP_FE(hw_pipedocument,							NULL)
	PHP_FE(hw_pipecgi,								NULL)
	PHP_FE(hw_insertdocument,						NULL)
	PHP_FE(hw_mv,									NULL)
	PHP_FE(hw_cp,									NULL)
	PHP_FE(hw_deleteobject,							NULL)
	PHP_FE(hw_changeobject,							NULL)
	PHP_FE(hw_modifyobject,							NULL)
	PHP_FE(hw_docbyanchor,							NULL)
	PHP_FE(hw_docbyanchorobj,						NULL)
	PHP_FE(hw_getobjectbyquery,						NULL)
	PHP_FE(hw_getobjectbyqueryobj,					NULL)
	PHP_FE(hw_getobjectbyquerycoll,					NULL)
	PHP_FE(hw_getobjectbyquerycollobj,				NULL)
	PHP_FE(hw_getobjectbyftquery,						NULL)
	PHP_FE(hw_getobjectbyftqueryobj,					NULL)
	PHP_FE(hw_getobjectbyftquerycoll,					NULL)
	PHP_FE(hw_getobjectbyftquerycollobj,				NULL)
	PHP_FE(hw_getchilddoccoll,						NULL)
	PHP_FE(hw_getchilddoccollobj,					NULL)
	PHP_FE(hw_getanchors,							NULL)
	PHP_FE(hw_getanchorsobj,						NULL)
	PHP_FE(hw_getusername,							NULL)
	PHP_FE(hw_setlinkroot,							NULL)
	PHP_FE(hw_identify,								NULL)
	PHP_FE(hw_free_document,						NULL)
	PHP_FE(hw_new_document,							NULL)
	PHP_FE(hw_new_document_from_file,							NULL)
	PHP_FE(hw_output_document,						NULL)
	PHP_FE(hw_document_size,						NULL)
	PHP_FE(hw_document_attributes,					NULL)
	PHP_FE(hw_document_bodytag,						NULL)
	PHP_FE(hw_document_content,						NULL)
	PHP_FE(hw_document_setcontent,						NULL)
	PHP_FE(hw_objrec2array,							NULL)
	PHP_FE(hw_array2objrec,							NULL)
	PHP_FE(hw_incollections,						NULL)
	PHP_FE(hw_inscoll,								NULL)
	PHP_FE(hw_insertobject,							NULL)
	PHP_FE(hw_insdoc,								NULL)
	PHP_FE(hw_getsrcbydestobj,						NULL)
	PHP_FE(hw_insertanchors,							NULL)
	PHP_FE(hw_getrellink,							NULL)
	PHP_FE(hw_who,									NULL)
	PHP_FE(hw_stat,									NULL)
	PHP_FE(hw_mapid,									NULL)
	PHP_FE(hw_dummy,								NULL)
	{NULL, NULL, NULL}
};

zend_module_entry hw_module_entry = {
	STANDARD_MODULE_HEADER,
	"hyperwave", hw_functions, PHP_MINIT(hw), PHP_MSHUTDOWN(hw), NULL, NULL, PHP_MINFO(hw), NO_VERSION_YET, STANDARD_MODULE_PROPERTIES
};

/*
#ifdef ZTS
int hw_globals_id;
#else
PHP_HW_API php_hw_globals hw_globals;
#endif
*/

ZEND_DECLARE_MODULE_GLOBALS(hw)

#ifdef COMPILE_DL_HYPERWAVE
ZEND_GET_MODULE(hw)
#endif

#define HW_FETCH_LINK(hw_zval) \
	convert_to_long_ex(hw_zval); \
	link = Z_LVAL_PP(hw_zval); \
	ptr = (hw_connection *) zend_list_find(link, &type); \
	if(!ptr || (type != le_socketp && type != le_psocketp)) { \
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find file identifier %d", link); \
		RETURN_FALSE; \
	}

#define HW_FETCH_ID(hw_zval) \
	convert_to_long_ex(hw_zval); \
	id = Z_LVAL_PP(hw_zval); \
	ptr = zend_list_find(id, &type); \
	if(!ptr || (type != le_socketp && type != le_psocketp)) { \
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find file identifier %d", id); \
		RETURN_FALSE; \
	}

void print_msg(hg_msg *msg, char *str, int txt);

void _close_hw_link(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	hw_connection *conn = (hw_connection *)rsrc->ptr;

	if(conn->hostname)
		free(conn->hostname);
	if(conn->username)
		free(conn->username);
	close(conn->socket);
	free(conn);
	HwSG(num_links)--;
}

void _close_hw_plink(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	hw_connection *conn = (hw_connection *)rsrc->ptr;

	if(conn->hostname)
		free(conn->hostname);
	if(conn->username)
		free(conn->username);
	close(conn->socket);
	free(conn);
	HwSG(num_links)--;
	HwSG(num_persistent)--;
}

void _free_hw_document(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	hw_document *doc = (hw_document *)rsrc->ptr;
	if(doc->data)
		free(doc->data);
	if(doc->attributes)
		free(doc->attributes);
	if(doc->bodytag)
		free(doc->bodytag);
	free(doc);
}

static void php_hw_init_globals(zend_hw_globals *hw_globals)
{
	hw_globals->num_persistent = 0;
}

static PHP_INI_MH(OnHyperwavePort)
{
	if (new_value==NULL) {
		HwSG(default_port) = HG_SERVER_PORT;
	} else {
		HwSG(default_port) = atoi(new_value);
	}
	return SUCCESS;
}

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("hyerwave.allow_persistent", "0", PHP_INI_SYSTEM, OnUpdateInt, allow_persistent, zend_hw_globals, hw_globals)
	PHP_INI_ENTRY("hyperwave.default_port",	"418", PHP_INI_ALL,	OnHyperwavePort)
PHP_INI_END()

PHP_MINIT_FUNCTION(hw)
{
	ZEND_INIT_MODULE_GLOBALS(hw, php_hw_init_globals, NULL);

	REGISTER_INI_ENTRIES();
	le_socketp = zend_register_list_destructors_ex(_close_hw_link, NULL, "hyperwave link", module_number);
	le_psocketp = zend_register_list_destructors_ex(NULL, _close_hw_plink, "hyperwave link persistent", module_number);
	le_document = zend_register_list_destructors_ex(_free_hw_document, NULL, "hyperwave document", module_number);
	Z_TYPE(hw_module_entry) = type;

	REGISTER_LONG_CONSTANT("HW_ATTR_LANG", HW_ATTR_LANG, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("HW_ATTR_NR", HW_ATTR_NR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("HW_ATTR_NONE", HW_ATTR_NONE, CONST_CS | CONST_PERSISTENT);
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(hw)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}

/* {{{ make_return_objrec
 * creates an array in return value and frees all memory
 * Also adds as an assoc. array at the end of the return array with
 * statistics.
 */
int make_return_objrec(pval **return_value, char **objrecs, int count)
{
	zval *stat_arr;
	int i;
	int hidden, collhead, fullcollhead, total;
        int collheadnr, fullcollheadnr;

	array_init(*return_value);

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
	MAKE_STD_ZVAL(stat_arr);
	array_init(stat_arr);

	add_assoc_long(stat_arr, "Hidden", hidden);
	add_assoc_long(stat_arr, "CollectionHead", collhead);
	add_assoc_long(stat_arr, "FullCollectionHead", fullcollhead);
	add_assoc_long(stat_arr, "Total", total);
	add_assoc_long(stat_arr, "CollectionHeadNr", collheadnr);
	add_assoc_long(stat_arr, "FullCollectionHeadNr", fullcollheadnr);

	/* Add the stat array */
	zend_hash_next_index_insert(Z_ARRVAL_PP(return_value), &stat_arr, sizeof(zval), NULL);

	return 0;
}
/* }}} */

/* {{{ make2_return_array_from_objrec
** creates an array return value from object record
*/
int make2_return_array_from_objrec(pval **return_value, char *objrec, zval *sarr) {
	char *attrname, *str, *temp, language[3];
	zval *spec_arr;
	char *strtok_buf = NULL;
	
	/* Create an array with an entry containing specs for each attribute
	   and fill in the specs for Title, Description, Keyword, Group.
	   If an array is passed as the last argument use it instead.
	 */
	if(NULL != sarr) {
		spec_arr = sarr;
	} else {
		MAKE_STD_ZVAL(spec_arr);
		array_init(spec_arr);
		add_assoc_long(spec_arr, "Title", HW_ATTR_LANG);
		add_assoc_long(spec_arr, "Description", HW_ATTR_LANG);
		add_assoc_long(spec_arr, "Keyword", HW_ATTR_LANG);
		add_assoc_long(spec_arr, "Group", HW_ATTR_NONE);
		add_assoc_long(spec_arr, "HtmlAttr", HW_ATTR_NONE);
		add_assoc_long(spec_arr, "Parent", HW_ATTR_NONE);
		add_assoc_long(spec_arr, "SQLStmt", HW_ATTR_NR);
	}

	array_init(*return_value);

	/* Loop through the attributes of object record and check
	   if the attribute has a specification. If it has the value
	   is added to array in spec record. If not it is added straight
	   to the return_value array.
	*/
	temp = estrdup(objrec);
	attrname = php_strtok_r(temp, "\n", &strtok_buf);
	while(attrname != NULL) {
		zval *data, **dataptr;
		long spec;
		str = attrname;

		/* Check if a specification is available.
		   If it isn't available then insert the attribute as
		   a string into the return array
		*/
		while((*str != '=') && (*str != '\0'))
			str++;
		*str = '\0';
		str++;
		if(zend_hash_find(Z_ARRVAL_P(spec_arr), attrname, strlen(attrname)+1, (void **) &dataptr) == FAILURE) {
			add_assoc_string(*return_value, attrname, str, 1);
		} else {
			zval *newarr;
			data = *dataptr;
			spec = Z_LVAL_P(data);

			if(zend_hash_find(Z_ARRVAL_PP(return_value), attrname, strlen(attrname)+1, (void **) &dataptr) == FAILURE) {
				MAKE_STD_ZVAL(newarr);
				array_init(newarr);
				zend_hash_add(Z_ARRVAL_PP(return_value), attrname, strlen(attrname)+1, &newarr, sizeof(zval *), NULL);
			} else {
				newarr = *dataptr;
			}

			switch(spec) {
				case HW_ATTR_LANG:
					if(str[2] == ':') {
						str[2] = '\0';
						strcpy(language, str);
						str += 3;
					} else
						strcpy(language, "xx");

					add_assoc_string(newarr, language, str, 1);
					break;
				case HW_ATTR_NR:
					if(str[1] == ':') {
						str[1] = '\0';
						strcpy(language, str);
						str += 2;
					} else
						strcpy(language, "x");

					add_assoc_string(newarr, language, str, 1);
					break;
				case HW_ATTR_NONE:
					add_next_index_string(newarr, str, 1);
					break;
			}
		}

		attrname = php_strtok_r(NULL, "\n", &strtok_buf);
	}
	if(NULL == sarr){
/*
		spec_arr->refcount--;
		zend_hash_destroy(Z_ARRVAL_P(spec_arr));
		efree(Z_ARRVAL_P(spec_arr));
*/
		zval_dtor(spec_arr);
		efree(spec_arr);
	}
	efree(temp);

	return(0);
}
/* }}} */

/* {{{ make_return_array_from_objrec
 */
int make_return_array_from_objrec(pval **return_value, char *objrec) {
	char *attrname, *str, *temp, language[3], *title;
	int iTitle, iDesc, iKeyword, iGroup;
	zval *title_arr;
	zval *desc_arr;
	zval *keyword_arr;
	zval *group_arr;
	int hasTitle = 0;
	int hasDescription = 0;
	int hasKeyword = 0;
	int hasGroup = 0;
	char *strtok_buf;

	MAKE_STD_ZVAL(title_arr);
	MAKE_STD_ZVAL(desc_arr);
	MAKE_STD_ZVAL(keyword_arr);
	MAKE_STD_ZVAL(group_arr);

	array_init(*return_value);

	/* Fill Array of titles, descriptions and keywords */
	temp = estrdup(objrec);
	attrname = php_strtok_r(temp, "\n", &strtok_buf);
	while(attrname != NULL) {
		str = attrname;
		iTitle = 0;
		iDesc = 0;
		iKeyword = 0;
		iGroup = 0;
		if(0 == strncmp(attrname, "Title=", 6)) {
			if (hasTitle == 0) {
				return -1;
			}
			array_init(title_arr);
			hasTitle = 1;
			str += 6;
			iTitle = 1;
		} else if(0 == strncmp(attrname, "Description=", 12)) {
			if (hasDescription == 0) {
				return -1;
			}
			array_init(desc_arr);
			hasDescription = 1;
			str += 12;
			iDesc = 1;
		} else if(0 == strncmp(attrname, "Keyword=", 8)) {
			if (hasKeyword == 0) {
				return -1;
			}
			array_init(keyword_arr);
			hasKeyword = 1;
			str += 8;
			iKeyword = 1;
		} else if(0 == strncmp(attrname, "Group=", 6)) {
			if (hasGroup == 0) {
				return -1;
			}
			array_init(group_arr);
			hasGroup = 1;
			str += 6;
			iGroup = 1;
		} 
		if(iTitle || iDesc || iKeyword) {	/* Poor error check if end of string */
			if(str[2] == ':') {
				str[2] = '\0';
				strcpy(language, str);
				str += 3;
			} else
				strcpy(language, "xx");

			title = str;
			if(iTitle)
				add_assoc_string(title_arr, language, title, 1);
			else if(iDesc)
				add_assoc_string(desc_arr, language, title, 1);
			else if(iKeyword)
				add_assoc_string(keyword_arr, language, title, 1);
		} else if(iGroup) {
			if(iGroup)
				add_next_index_string(group_arr, str, 1);
		}
		attrname = php_strtok_r(NULL, "\n", &strtok_buf);
	}
	efree(temp);

	/* Add the title array, if we have one */
	if(hasTitle) {
		zend_hash_update(Z_ARRVAL_PP(return_value), "Title", 6, &title_arr, sizeof(zval *), NULL);

	} else {
		efree(title_arr);
	}


	if(hasDescription) {
	/* Add the description array, if we have one */
		zend_hash_update(Z_ARRVAL_PP(return_value), "Description", 12, &desc_arr, sizeof(zval *), NULL);

	} else {
		efree(desc_arr);
	}

	if(hasKeyword) {
	/* Add the keyword array, if we have one */
		zend_hash_update(Z_ARRVAL_PP(return_value), "Keyword", 8, &keyword_arr, sizeof(zval *), NULL);

	} else {
		efree(keyword_arr);
	}

	if(hasGroup) {
	/* Add the Group array, if we have one */
		zend_hash_update(Z_ARRVAL_PP(return_value), "Group", 6, &group_arr, sizeof(zval *), NULL);

	} else {
		efree(group_arr);
	}

	/* All other attributes. Make a another copy first */
	temp = estrdup(objrec);
	attrname = php_strtok_r(temp, "\n", &strtok_buf);
	while(attrname != NULL) {
		str = attrname;
		/* We don't want to insert titles, descr., keywords a second time */
		if((0 != strncmp(attrname, "Title=", 6)) &&
		   (0 != strncmp(attrname, "Description=", 12)) &&
		   (0 != strncmp(attrname, "Group=", 6)) &&
		   (0 != strncmp(attrname, "Keyword=", 8))) {
			while((*str != '=') && (*str != '\0'))
				str++;
			*str = '\0';
			str++;
			add_assoc_string(*return_value, attrname, str, 1);
		}
		attrname = php_strtok_r(NULL, "\n", &strtok_buf);
	}
	efree(temp);

	return(0);
}
/* }}} */

#define BUFFERLEN 1024
/* {{{ make_objrec_from_array
 */
static char * make_objrec_from_array(HashTable *lht, char delim) {
	int i, count, keytype;
	ulong idx;
	uint length;
	char *key, str[BUFFERLEN], *objrec = NULL;
	zval *keydata, **keydataptr;

	if(NULL == lht)
		return NULL;

	if(0 == (count = zend_hash_num_elements(lht)))
		return NULL;

	if(delim == 0)
		delim = '=';

	zend_hash_internal_pointer_reset(lht);
	objrec = malloc(1);
	*objrec = '\0';
	for(i=0; i<count; i++) {
		keytype = zend_hash_get_current_key_ex(lht, &key, &length, &idx, 0, NULL);
/*		if(HASH_KEY_IS_STRING == keytype) { */
			zend_hash_get_current_data(lht, (void **) &keydataptr);
			keydata = *keydataptr;
			switch(Z_TYPE_P(keydata)) {
				case IS_STRING:
					if(HASH_KEY_IS_STRING == keytype)
						snprintf(str, BUFFERLEN, "%s%c%s\n", key, delim, Z_STRVAL_P(keydata));
					else if(HASH_KEY_IS_LONG == keytype)
						snprintf(str, BUFFERLEN, "%ld%c%s\n", idx, delim, Z_STRVAL_P(keydata));
					else
						snprintf(str, BUFFERLEN, "%s\n", Z_STRVAL_P(keydata));
					break;
				case IS_LONG:
					if(HASH_KEY_IS_STRING == keytype)
						snprintf(str, BUFFERLEN, "%s%c0x%lX\n", key, delim, Z_LVAL_P(keydata));
					else if(HASH_KEY_IS_LONG == keytype)
						snprintf(str, BUFFERLEN, "%ld%c%s\n", idx, delim, Z_STRVAL_P(keydata));
					else
						snprintf(str, BUFFERLEN, "0x%lX\n", Z_LVAL_P(keydata));
					break;
				case IS_ARRAY: {
					int i, len, keylen, count;
					char *strarr, *ptr, *ptr1;
					count = zend_hash_num_elements(Z_ARRVAL_P(keydata));
					if(count > 0) {
						strarr = make_objrec_from_array(Z_ARRVAL_P(keydata), ':');
						len = strlen(strarr) - 1;
						keylen = strlen(key);
						if(NULL == (ptr = malloc(len + 1 + count*(keylen+1)))) {
							free(objrec);
							return(NULL);
						}
						ptr1 = ptr;
						*ptr1 = '\0';
						strcpy(ptr1, key);
						ptr1 += keylen;
						*ptr1++ = '=';
						for(i=0; i<len; i++) {
							*ptr1++ = strarr[i];
							if(strarr[i] == '\n') {
								strcpy(ptr1, key);
								ptr1 += keylen;
								*ptr1++ = '=';
							}/* else if(strarr[i] == '=')
								ptr1[-1] = ':'; */
						}
						*ptr1++ = '\n';
						*ptr1 = '\0';
						strlcpy(str, ptr, sizeof(str));
					}
					break;
				}
			}
			objrec = realloc(objrec, strlen(objrec)+strlen(str)+1);
			strcat(objrec, str);
/*		} */
		zend_hash_move_forward(lht);
	}
	return objrec;
}
/* }}} */
#undef BUFFERLEN

/* {{{ make_ints_from_array
 */
static int * make_ints_from_array(HashTable *lht) {
	int i, count;
	int *objids = NULL;
	zval **keydata;

	if(NULL == lht)
		return NULL;

	if(0 == (count = zend_hash_num_elements(lht)))
		return NULL;
	
	zend_hash_internal_pointer_reset(lht);
	if(NULL == (objids = emalloc(count*sizeof(int))))
		return NULL;
	for(i=0; i<count; i++) {
		zend_hash_get_current_data(lht, (void **) &keydata);
		switch(Z_TYPE_PP(keydata)) {
			case IS_LONG:
				objids[i] = Z_LVAL_PP(keydata);
				break;
			default:
				objids[i] = 0;
		}
		zend_hash_move_forward(lht);
	}
	return objids;
}
/* }}} */

/* {{{ make_strs_from_array
 */
static char **make_strs_from_array(HashTable *arrht) {
	char **carr = NULL;
	char **ptr;
	zval *data, **dataptr;

	zend_hash_internal_pointer_reset(arrht);
	if(NULL == (carr = emalloc(zend_hash_num_elements(arrht) * sizeof(char *))))
		return(NULL);	
	ptr = carr;

	/* Iterate through hash */
	while(zend_hash_get_current_data(arrht, (void **) &dataptr) == SUCCESS) {
		data = *dataptr;
		switch(Z_TYPE_P(data)) {
			case IS_STRING:
				*ptr = estrdup(Z_STRVAL_P(data));
/*fprintf(stderr, "carr[] = %s\n", *ptr); */
				break;
			default:
				*ptr = NULL;
		}
		ptr++;

		zend_hash_move_forward(arrht);
	}
	return(carr);
}
/* }}} */

#define BUFFERLEN 30
/* {{{ php_hw_do_connect
 */
static void php_hw_do_connect(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	zval **argv[4];
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
	char buffer[BUFFERLEN];
	int hashed_details_length;
	hw_connection *ptr;
	int do_swap;
	int version = 0;
	
	argc = ZEND_NUM_ARGS();
	switch(argc) {
		case 2:
		case 4:
			if (zend_get_parameters_array_ex(argc, argv) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	/* Host: */
	convert_to_string_ex(argv[0]);
	host = (char *) estrndup(Z_STRVAL_PP(argv[0]), Z_STRLEN_PP(argv[0]));

	/* Port: */
	convert_to_long_ex(argv[1]);
	port = Z_LVAL_PP(argv[1]);

	/* Username and Password */
	if(argc > 2) {
		/* Username */
		convert_to_string_ex(argv[2]);
		username = (char *) estrndup(Z_STRVAL_PP(argv[2]), Z_STRLEN_PP(argv[2]));
		/* Password */
		convert_to_string_ex(argv[3]);
		password = (char *) estrndup(Z_STRVAL_PP(argv[3]), Z_STRLEN_PP(argv[3]));
	}

	/* Create identifier string for connection */
	snprintf(buffer, BUFFERLEN, "%d", port);
	hashed_details_length = strlen(host)+strlen(buffer)+8;
	if(NULL == (hashed_details = (char *) emalloc(hashed_details_length+1))) {
		if(host) efree(host);
		if(password) efree(password);
		if(username) efree(username);
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Could not get memory for connection details");
		RETURN_FALSE;
	}
	sprintf(hashed_details, "hw_%s_%d", host, port);

	if (persistent) {
		list_entry *le;

		/* try to find if we already have this link in our persistent list */
		if (zend_hash_find(&EG(persistent_list), hashed_details, hashed_details_length+1, (void **) &le)==FAILURE) {
			list_entry new_le;

			if (HwSG(max_links)!=-1 && HwSG(num_links)>=HwSG(max_links)) {
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "Too many open links (%d)", HwSG(num_links));
				if(host) efree(host);
				if(username) efree(username);
				if(password) efree(password);
				efree(hashed_details);
				RETURN_FALSE;
			}
			if (HwSG(max_persistent!=-1) && HwSG(num_persistent)>=HwSG(max_persistent)) {
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "Too many open persistent links (%d)", HwSG(num_persistent));
				if(host) efree(host);
				if(username) efree(username);
				if(password) efree(password);
				efree(hashed_details);
				RETURN_FALSE;
			}

			if ( (sockfd = open_hg_connection(host, port)) < 0 )  {
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "Could not open connection to %s, Port: %d (retval=%d, errno=%d)", host, port, sockfd, errno);
				if(host) efree(host);
				if(username) efree(username);
				if(password) efree(password);
				efree(hashed_details);
				RETURN_FALSE;
				}
	
			if(NULL == (ptr = malloc(sizeof(hw_connection)))) {
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "Could not get memory for connection structure");
				if(host) efree(host);
				if(username) efree(username);
				if(password) efree(password);
				efree(hashed_details);
				RETURN_FALSE;
			}
	
			if(0 != (ptr->lasterror = initialize_hg_connection(sockfd, &do_swap, &version, &userdata, &server_string, username, password))) {
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "Could not initalize hyperwave connection");
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
			Z_TYPE(new_le) = le_psocketp;

			if (zend_hash_update(&EG(persistent_list), hashed_details, hashed_details_length+1, (void *) &new_le, sizeof(list_entry), NULL)==FAILURE) {
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "Could not hash table with connection details");
				if(host) efree(host);
				if(username) efree(username);
				if(password) efree(password);
				if(server_string) free(server_string);
				efree(hashed_details);
				RETURN_FALSE;
			}

			HwSG(num_links)++;
			HwSG(num_persistent)++;
		} else {
			/*php_printf("Found already open connection\n"); */
			if (Z_TYPE_P(le) != le_psocketp) {
				RETURN_FALSE;
			}
			ptr = le->ptr;
		}

		Z_LVAL_P(return_value) = zend_list_insert(ptr, le_psocketp);
		Z_TYPE_P(return_value) = IS_RESOURCE;
	
	} else {
		list_entry *index_ptr, new_index_ptr;

		/* first we check the hash for the hashed_details key.  if it exists,
		 * it should point us to the right offset where the actual hyperwave link sits.
		 * if it doesn't, open a new hyperwave link, add it to the resource list,
		 * and add a pointer to it with hashed_details as the key.
		 */
		if (zend_hash_find(&EG(regular_list), hashed_details, hashed_details_length+1, (void **) &index_ptr)==SUCCESS) {
			int type, link;
			void *ptr;
	
			if (Z_TYPE_P(index_ptr) != le_index_ptr) {
				RETURN_FALSE;
			}
			link = (int) index_ptr->ptr;
			ptr = (hw_connection *) zend_list_find(link, &type);   /* check if the link is still there */
			if(!ptr || (type!=le_socketp && type!=le_psocketp)) {
				Z_LVAL_P(return_value) = HwSG(default_link) = link;
				Z_TYPE_P(return_value) = IS_LONG;
				efree(hashed_details);
				if(username) efree(username);
				if(password) efree(password);
	  		if(host) efree(host);
				return;
			} else {
				zend_hash_del(&EG(regular_list), hashed_details, hashed_details_length+1);
			}
		}
	
		if ( (sockfd = open_hg_connection(host, port)) < 0 )  {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Could not open connection to %s, Port: %d (retval=%d", host, port, sockfd);
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
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Could not initalize hyperwave connection");
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
	
		Z_LVAL_P(return_value) = zend_list_insert(ptr, le_socketp);
		Z_TYPE_P(return_value) = IS_RESOURCE;
	
		new_index_ptr.ptr = (void *) Z_LVAL_P(return_value);
		Z_TYPE(new_index_ptr) = le_index_ptr;
		if (zend_hash_update(&EG(regular_list), hashed_details, hashed_details_length+1, (void *) &new_index_ptr, sizeof(list_entry), NULL)==FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Could not update connection details in hash table");
			if(host) efree(host);
			efree(hashed_details);
			RETURN_FALSE;
		}
	
	}

	efree(hashed_details);
	if(host) efree(host);
	HwSG(default_link)=Z_LVAL_P(return_value);

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
/* }}} */
#undef BUFFERLEN

/* Start of user level functions */
/* ***************************** */
/* {{{ proto int hw_connect(string host, int port [string username [, string password]])
   Connect to the Hyperwave server */
PHP_FUNCTION(hw_connect)
{
	php_hw_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int hw_pconnect(string host, int port [, string username [, string password]])
   Connect to the Hyperwave server persistent */
PHP_FUNCTION(hw_pconnect)
{
	php_hw_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto void hw_close(int link)
   Close connection to Hyperwave server */
PHP_FUNCTION(hw_close)
{
	zval **arg1;
	int id, type;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_ID(arg1);
	zend_list_delete(id);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void hw_info(int link)
   Outputs info string */
PHP_FUNCTION(hw_info)
{
	pval **arg1;
	int id, type;
	hw_connection *ptr;
	char *str;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_ID(arg1);
	if(NULL != (str = get_hw_info(ptr))) {
		/*
		php_printf("%s\n", str);
		efree(str);
		*/
		Z_STRLEN_P(return_value) = strlen(str);
		Z_STRVAL_P(return_value) = str;
		Z_TYPE_P(return_value) = IS_STRING;
		return;
		}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int hw_error(int link)
   Returns last error number */
PHP_FUNCTION(hw_error)
{
	pval **arg1;
	int id, type;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_ID(arg1);
	RETURN_LONG(ptr->lasterror);
}
/* }}} */

/* {{{ proto string hw_errormsg(int link)
   Returns last error message */
PHP_FUNCTION(hw_errormsg)
{
	pval **arg1;
	int id, type;
	hw_connection *ptr;
	char errstr[100];

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_ID(arg1);

	switch (ptr->lasterror) {
		case 0:
			sprintf(errstr, "No error");
			break;
		case NOACCESS:
			sprintf(errstr, "Access denied");
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
		case CHANGEBASEFLD:
			sprintf(errstr, "Change of base-attribute");
			break;
		case NOTREMOVED:
			sprintf(errstr, "Attribute not removed");
			break;
		case FLDEXISTS:
			sprintf(errstr, "Attribute exists");
			break;
		case CMDSYNTAX:
			sprintf(errstr, "Syntax error in command");
			break;
		case NOLANGUAGE:
			sprintf(errstr, "No or unknown language specified");
			break;
		case WRGTYPE:
			sprintf(errstr, "Wrong type in object");
			break;
		case WRGVERSION:
			sprintf(errstr, "Client version too old");
			break;
		case CONNECTION:
			sprintf(errstr, "No connection to other server");
			break;
		case SYNC:
			sprintf(errstr, "Synchronization error");
			break;
		case NOPATH:
			sprintf(errstr, "No path entry");
			break;
		case WRGPATH:
			sprintf(errstr, "Wrong path entry");
			break;
		case PASSWD:
			sprintf(errstr, "Wrong password (server-to-server server authentication)");
			break;
		case LC_NO_MORE_USERS:
			sprintf(errstr, "No more users for license");
			break;
		case LC_NO_MORE_DOCS:
			sprintf(errstr, "No more documents for this session and license");
			break;
		case RSERV_NRESP:
			sprintf(errstr, "Remote server not responding");
			break;
		case Q_OVERFLOW:
			sprintf(errstr, "Query overflow");
			break;
		case USR_BREAK:
			sprintf(errstr, "Break by user");
			break;
		case N_IMPL:
			sprintf(errstr, "Not implemented");
			break;
		case WRG_VALUE:
			sprintf(errstr, "Wrong value");
			break;
		case INSUFF_FUNDS:
			sprintf(errstr, "Insufficient funds");
			break;
		case REORG:
			sprintf(errstr, "Reorganization in progress");
			break;
		case USER_LIMIT:
			sprintf(errstr, "Limit of simultaneous users reached");
			break;
		case FTCONNECT:
			sprintf(errstr, "No connection to fulltext server");
			break;
		case FTTIMEOUT:
			sprintf(errstr, "Connection timed out");
			break;
		case FTINDEX:
			sprintf(errstr, "Something wrong with fulltext index");
			break;
		case FTSYNTAX:
			sprintf(errstr, "Query syntax error");
			break;
		case REQUESTPENDING:
			sprintf(errstr, "Request pending");
			break;
		case NOCONNECTION:
			sprintf(errstr, "No connection to document server");
			break;
		case WRONGVERSION:
			sprintf(errstr, "Wrong protocol version");
			break;
		case NOTINITIALIZED:
			sprintf(errstr, "Not initialized");
			break;
		case BADREQUEST:
			sprintf(errstr, "Bad request");
			break;
		case BADLRN:
			sprintf(errstr, "Bad document number");
			break;
		case OPENSTORE_WRITE:
			sprintf(errstr, "Cannot write to local store");
			break;
		case OPENSTORE_READ:
			sprintf(errstr, "Cannot read from local store");
			break;
		case READSTORE:
			sprintf(errstr, "Store read error");
			break;
		case WRITESTORE:
			sprintf(errstr, "Write error");
			break;
		case CLOSESTORE:
			sprintf(errstr, "Close error");
			break;
		case BADPATH:
			sprintf(errstr, "Bad path");
			break;
		case NOPATHDC:
			sprintf(errstr, "No path");
			break;
		case OPENFILE:
			sprintf(errstr, "Cannot open file");
			break;
		case READFILE:
			sprintf(errstr, "Cannot read from file // same");
			break;
		case WRITEFILE:
			sprintf(errstr, "Cannot write to file");
			break;
		case CONNECTCLIENT:
			sprintf(errstr, "Could not connect to client");
			break;
		case ACCEPT:
			sprintf(errstr, "Could not accept connection");
			break;
		case READSOCKET:
			sprintf(errstr, "Could not read from socket");
			break;
		case WRITESOCKET:
			sprintf(errstr, "Could not write to socket");
			break;
		case TOOMUCHDATA:
			sprintf(errstr, "Received too much data");
			break;
		case TOOFEWDATA:
			sprintf(errstr, "Received too few data // ...");
			break;
		case NOTIMPLEMENTED:
			sprintf(errstr, "Not implemented");
			break;
		case USERBREAK:
			sprintf(errstr, "User break");
			break;
		case INTERNAL:
			sprintf(errstr, "Internal error");
			break;
		case INVALIDOBJECT:
			sprintf(errstr, "Invalid object");
			break;
		case JOBTIMEOUT:
			sprintf(errstr, "Job timed out");
			break;
		case OPENPORT:
			sprintf(errstr, "Cannot open port // ... for several resons");
			break;
		case NODATA:
			sprintf(errstr, "Received no data");
			break;
		case NOPORT:
			sprintf(errstr, "No port to handle this request");
			break;
		case NOTCACHED:
			sprintf(errstr, "Document not cached");
			break;
		case BADCACHETYPE:
			sprintf(errstr, "Bad cache type");
			break;
		case OPENCACHE_WRITE:
			sprintf(errstr, "Cannot write to cache");
			break;
		case OPENCACHE_READ:
			sprintf(errstr, "Cannot read from cache // same");
			break;
		case NOSOURCE:
			sprintf(errstr, "Do not know what to read");
			break;
		case CLOSECACHE:
			sprintf(errstr, "Could not insert into cache");
			break;
		case CONNECTREMOTE:
			sprintf(errstr, "Could not connect to remote server");
			break;
		case LOCKREFUSED:
			sprintf(errstr, "Lock refused // could not lock the stores");
			break;
		default:
			sprintf(errstr, "Unknown error: %d", ptr->lasterror);
		}
	RETURN_STRING(errstr, 1);
}
/* }}} */

/* {{{ proto int hw_root(void)
   Returns object id of root collection */
PHP_FUNCTION(hw_root)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	Z_LVAL_P(return_value) = 0;
	Z_TYPE_P(return_value) = IS_LONG;
}
/* }}} */

/* {{{ php_hw_command
 */
char *php_hw_command(INTERNAL_FUNCTION_PARAMETERS, int comm) {
	pval **arg1;
	int link, type;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		return NULL;
	}
	convert_to_long_ex(arg1);
	link = Z_LVAL_PP(arg1);
	ptr = (hw_connection *) zend_list_find(link, &type);
	if(!ptr || (type != le_socketp && type != le_psocketp)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find file identifier %d", link);
		return NULL;
	}

	set_swap(ptr->swap_on);
	{
		char *object = NULL;
		if (0 != (ptr->lasterror = send_command(ptr->socket, comm, &object))) {
			return NULL;
		}
		return object;
	}
	return NULL;
}
/* }}} */

/* {{{ proto string hw_stat(int link)
   Returns status string */
PHP_FUNCTION(hw_stat)
{
        char *object;

	object = php_hw_command(INTERNAL_FUNCTION_PARAM_PASSTHRU, STAT_COMMAND);
	if(object == NULL)
		RETURN_FALSE;

	Z_STRVAL_P(return_value) = object;
	Z_STRLEN_P(return_value) = strlen(object);
	Z_TYPE_P(return_value) = IS_STRING;
}
/* }}} */

/* {{{ proto array hw_who(int link)
   Returns names and info of users loged in */
PHP_FUNCTION(hw_who)
{
	zval *user_arr;
        char *object, *ptr, *temp, *attrname;
	int i;
	char *strtok_buf;

	object = php_hw_command(INTERNAL_FUNCTION_PARAM_PASSTHRU, WHO_COMMAND);
	if(object == NULL)
		RETURN_FALSE;

	ptr = object;

php_printf("%s\n", ptr);
	/* Skip first two lines, they just contain:
        Users in Database

        */
        while((*ptr != '\0') && (*ptr != '\n'))
		ptr++;
        while((*ptr != '\0') && (*ptr != '\n'))
		ptr++;
	if(*ptr == '\0') {
		efree(object);
		RETURN_FALSE;
	}

	array_init(return_value);

	temp = estrdup(ptr);
	attrname = php_strtok_r(temp, "\n", &strtok_buf);
	i = 0;
	while(attrname != NULL) {
		char *name;

		ALLOC_ZVAL(user_arr);
		array_init(user_arr);

		ptr = attrname;
		if(*ptr++ == '*')
			add_assoc_long(user_arr, "self", 1);
		else
			add_assoc_long(user_arr, "self", 0);
			
		ptr++;
		name = ptr;
		while((*ptr != '\0') && (*ptr != ' '))
			ptr++;
		*ptr = '\0';
		add_assoc_string(user_arr, "id", name, 1);

		ptr++;
		name = ptr;
		while((*ptr != '\0') && (*ptr != ' '))
			ptr++;
		*ptr = '\0';
		add_assoc_string(user_arr, "name", name, 1);

		ptr++;
		while((*ptr != '\0') && (*ptr == ' '))
			ptr++;

		name = ptr;
		while((*ptr != '\0') && (*ptr != ' '))
			ptr++;
		*ptr = '\0';
		add_assoc_string(user_arr, "system", name, 1);

		ptr++;
		while((*ptr != '\0') && (*ptr == ' '))
			ptr++;

		name = ptr;
		while((*ptr != '\0') && (*ptr != ' '))
			ptr++;
		*ptr = '\0';
		add_assoc_string(user_arr, "onSinceDate", name, 1);

		ptr++;
		while((*ptr != '\0') && (*ptr == ' '))
			ptr++;

		name = ptr;
		while((*ptr != '\0') && (*ptr != ' '))
			ptr++;
		*ptr = '\0';
		add_assoc_string(user_arr, "onSinceTime", name, 1);

		ptr++;
		while((*ptr != '\0') && (*ptr == ' '))
			ptr++;

		name = ptr;
		while((*ptr != '\0') && (*ptr != ' '))
			ptr++;
		*ptr = '\0';
		add_assoc_string(user_arr, "TotalTime", name, 1);

		/* Add the user array */
		zend_hash_index_update(Z_ARRVAL_P(return_value), i++, &user_arr, sizeof(pval), NULL);

		attrname = php_strtok_r(NULL, "\n", &strtok_buf);
	}
	efree(temp);
	efree(object);

}
/* }}} */

/* {{{ proto string hw_dummy(int link, int id, int msgid)
   Hyperwave dummy function */
PHP_FUNCTION(hw_dummy)
{
	pval **arg1, **arg2, **arg3;
	int link, id, type, msgid;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);
	id=Z_LVAL_PP(arg2);
	msgid=Z_LVAL_PP(arg3);
	HW_FETCH_LINK(arg1);

	set_swap(ptr->swap_on);
	{
	char *object = NULL;
	if (0 != (ptr->lasterror = send_dummy(ptr->socket, id, msgid, &object)))
		RETURN_FALSE;

php_printf("%s", object);
	Z_STRVAL_P(return_value) = object;
	Z_STRLEN_P(return_value) = strlen(object);
	Z_TYPE_P(return_value) = IS_STRING;
	}
}
/* }}} */

/* {{{ proto string hw_getobject(int link, int objid [, string query])
   Returns object record  */
PHP_FUNCTION(hw_getobject)
{
	pval **argv[3];
	int argc, link, id, type, multi;
	char *query;
	hw_connection *ptr;

	argc = ZEND_NUM_ARGS();
	if(argc < 2 || argc > 3)
		WRONG_PARAM_COUNT;
	if (zend_get_parameters_array_ex(argc, argv) == FAILURE)
		WRONG_PARAM_COUNT;

	HW_FETCH_LINK(argv[0]);
	if(Z_TYPE_PP(argv[1]) == IS_ARRAY) {
		multi = 1;
		convert_to_array_ex(argv[1]);
	} else {
		multi = 0;
		convert_to_long_ex(argv[1]);
	}

	if(argc == 3) {
		convert_to_string_ex(argv[2]);
		query = Z_STRVAL_PP(argv[2]);
	} else
		query = NULL;

	set_swap(ptr->swap_on);
	if(multi) {
		char **objects = NULL;
		int count, *ids, i;
		HashTable *lht;
		zval **keydata;

		lht = Z_ARRVAL_PP(argv[1]);
		if(0 == (count = zend_hash_num_elements(lht))) {
			RETURN_FALSE;
		}
		ids = emalloc(count * sizeof(hw_objectID));

		zend_hash_internal_pointer_reset(lht);
		for(i=0; i<count; i++) {
			zend_hash_get_current_data(lht, (void **) &keydata);
			switch(Z_TYPE_PP(keydata)) {
				case IS_LONG:
					ids[i] = Z_LVAL_PP(keydata);
					break;
				default:
					ids[i] = Z_LVAL_PP(keydata);
			}
			zend_hash_move_forward(lht);
		}

		if (0 != (ptr->lasterror = send_objectbyidquery(ptr->socket, ids, &count, query, &objects))) {
			efree(ids);
			RETURN_FALSE;
			}
		efree(ids);
		array_init(return_value);

		for(i=0; i<count; i++) {
			add_index_string(return_value, i, objects[i], 0);
		}
		efree(objects);
		
	} else {
		char *object = NULL;
		id=Z_LVAL_PP(argv[1]);
		if (0 != (ptr->lasterror = send_getobject(ptr->socket, id, &object)))
			RETURN_FALSE;

		RETURN_STRING(object, 0);
	}
}
/* }}} */

/* {{{ proto int hw_insertobject(int link, string objrec, string parms)
   Inserts an object */
PHP_FUNCTION(hw_insertobject)
{
	zval **arg1, **arg2, **arg3;
	int link, type;
	char *objrec, *parms;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);
	objrec=Z_STRVAL_PP(arg2);
	parms=Z_STRVAL_PP(arg3);

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
PHP_FUNCTION(hw_getandlock)
{
	zval **arg1, **arg2;
	int link, id, type;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	id=Z_LVAL_PP(arg2);

	set_swap(ptr->swap_on);
	{
	char *object = NULL;
	if (0 != (ptr->lasterror = send_getandlock(ptr->socket, id, &object)))
		RETURN_FALSE;

	RETURN_STRING(object, 0);
	}
}
/* }}} */

/* {{{ proto void hw_unlock(int link, int objid)
   Unlocks object */
PHP_FUNCTION(hw_unlock)
{
	zval **arg1, **arg2;
	int link, id, type;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	id=Z_LVAL_PP(arg2);

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_unlock(ptr->socket, id)))
		RETURN_FALSE;

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void hw_deleteobject(int link, int objid)
   Deletes object */
PHP_FUNCTION(hw_deleteobject)
{
	zval **arg1, **arg2;
	int link, id, type;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	id=Z_LVAL_PP(arg2);

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_deleteobject(ptr->socket, id)))
		RETURN_FALSE;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void hw_changeobject(int link, int objid, array attributes)
   Changes attributes of an object (obsolete) */
#define BUFFERLEN 200
PHP_FUNCTION(hw_changeobject)
{
	zval **arg1, **arg2, **arg3;
	int link, id, type, i;
	hw_connection *ptr;
	char *modification, *oldobjrec, buf[BUFFERLEN];
	HashTable *newobjarr;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2); /* object ID */
	convert_to_array_ex(arg3); /* Array with new attributes */
	id=Z_LVAL_PP(arg2);
	newobjarr=Z_ARRVAL_PP(arg3);

	/* get the old object record */
	if(0 != (ptr->lasterror = send_getandlock(ptr->socket, id, &oldobjrec)))
		RETURN_FALSE;

	zend_hash_internal_pointer_reset(newobjarr);
	modification = strdup("");
	for(i=0; i<zend_hash_num_elements(newobjarr); i++) {
		char *key, *str, *str1, newattribute[BUFFERLEN];
		pval *data, **dataptr;
		int j, noinsert=1;
		ulong ind;

		zend_hash_get_current_key(newobjarr, &key, &ind, 0);
		zend_hash_get_current_data(newobjarr, (void *) &dataptr);
		data = *dataptr;
		switch(Z_TYPE_P(data)) {
			case IS_STRING:
				if(strlen(Z_STRVAL_P(data)) == 0)
					snprintf(newattribute, BUFFERLEN, "rem %s", key);
				else
					snprintf(newattribute, BUFFERLEN, "add %s=%s", key, Z_STRVAL_P(data));
				noinsert = 0;
				break;
			default:
				newattribute[0] = '\0';
		}
		if(!noinsert) {
			modification = fnInsStr(modification, 0, "\\");
			modification = fnInsStr(modification, 0, newattribute);
/*			modification = fnInsStr(modification, 0, "add "); */

			/* Retrieve the old attribute from object record */
			if(NULL != (str = strstr(oldobjrec, key))) {
				str1 = str;
				j = 0;
				while((str1 != NULL) && (*str1 != '\n') && (j < BUFFERLEN-1)) {
					buf[j++] = *str1++;
				}
				buf[j] = '\0';
				modification = fnInsStr(modification, 0, "\\");
				modification = fnInsStr(modification, 0, buf);
				modification = fnInsStr(modification, 0, "rem ");
			} 
		}
		zend_hash_move_forward(newobjarr);
	}
	efree(oldobjrec);

	set_swap(ptr->swap_on);
	modification[strlen(modification)-1] = '\0';
	if (0 != (ptr->lasterror = send_changeobject(ptr->socket, id, modification))) {
		free(modification);
		send_unlock(ptr->socket, id);
		RETURN_FALSE;
	}
	free(modification);
	if (0 != (ptr->lasterror = send_unlock(ptr->socket, id))) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
#undef BUFFERLEN
/* }}} */

/* {{{ proto void hw_modifyobject(int link, int objid, array remattributes, array addattributes [, int mode])
   Modifies attributes of an object */
#define BUFFERLEN 200
PHP_FUNCTION(hw_modifyobject)
{
	zval **argv[5];
	int argc;
	int link, id, type, i, mode;
	hw_connection *ptr;
	char *modification;
	HashTable *remobjarr, *addobjarr;

	argc = ZEND_NUM_ARGS();
	if((argc > 5) || (argc < 4))
		WRONG_PARAM_COUNT;

	if (zend_get_parameters_array_ex(argc, argv) == FAILURE)
	if(argc < 4) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(argv[0]);
	convert_to_long_ex(argv[1]); /* object ID */
	convert_to_array_ex(argv[2]); /* Array with attributes to remove */
	convert_to_array_ex(argv[3]); /* Array with attributes to add */
	if(argc == 5) {
		convert_to_long_ex(argv[4]);
		mode = Z_LVAL_PP(argv[4]);
	} else
		mode = 0;
	id=Z_LVAL_PP(argv[1]);
	remobjarr=Z_ARRVAL_PP(argv[2]);
	addobjarr=Z_ARRVAL_PP(argv[3]);

	modification = strdup("");
	if(addobjarr != NULL) {
		zend_hash_internal_pointer_reset(addobjarr);
		for(i=0; i<zend_hash_num_elements(addobjarr); i++) {
			char *key, addattribute[BUFFERLEN];
			zval *data, **dataptr;
			int noinsert=1;
			ulong ind;

			zend_hash_get_current_key(addobjarr, &key, &ind, 0);
			zend_hash_get_current_data(addobjarr, (void *) &dataptr);
			data = *dataptr;
			switch(Z_TYPE_P(data)) {
				case IS_STRING:
					if(strlen(Z_STRVAL_P(data)) > 0) {
						snprintf(addattribute, BUFFERLEN, "add %s=%s", key, Z_STRVAL_P(data));
/* fprintf(stderr, "add: %s\n", addattribute); */
						noinsert = 0;
					}
					break;
				case IS_ARRAY: {
					int i, len, keylen, count;
					char *strarr, *ptr, *ptr1;
					count = zend_hash_num_elements(Z_ARRVAL_P(data));
					if(count > 0) {
						strarr = make_objrec_from_array(Z_ARRVAL_P(data), ':');
						len = strlen(strarr) - 1;
						keylen = strlen(key);
						if(NULL == (ptr = malloc(len + 1 + count*(keylen+1+4)))) {
							if(modification)
								free(modification);
							RETURN_FALSE;
						}
						ptr1 = ptr;
						*ptr1 = '\0';
						strcpy(ptr1, "add ");
						ptr1 += 4;
						strcpy(ptr1, key);
						ptr1 += keylen;
						*ptr1++ = '=';
						for(i=0; i<len; i++) {
							*ptr1++ = strarr[i];
							if(strarr[i] == '\n') {
								ptr1[-1] = '\\';
								strcpy(ptr1, "add ");
								ptr1 += 4;
								strcpy(ptr1, key);
								ptr1 += keylen;
								*ptr1++ = '=';
							} /* else if(strarr[i] == '=')
								ptr1[-1] = ':'; */
						}
						*ptr1 = '\0';
						strlcpy(addattribute, ptr, sizeof(addattribute));
						noinsert = 0;
					}
					break;
				}
			}
			if(!noinsert) {
				modification = fnInsStr(modification, 0, "\\");
				modification = fnInsStr(modification, 0, addattribute);
			}
			zend_hash_move_forward(addobjarr);
		}
	}

	if(remobjarr != NULL) {
		int nr;
		zend_hash_internal_pointer_reset(remobjarr);
		nr = zend_hash_num_elements(remobjarr);
		for(i=0; i<nr; i++) {
			char *key, remattribute[BUFFERLEN];
			zval *data, **dataptr;
			int noinsert=1;
			ulong ind;

			zend_hash_get_current_key(remobjarr, &key, &ind, 0);
			zend_hash_get_current_data(remobjarr, (void *) &dataptr);
			data = *dataptr;
			switch(Z_TYPE_P(data)) {
				case IS_STRING:
					if(strlen(Z_STRVAL_P(data)) > 0) {
						snprintf(remattribute, BUFFERLEN, "rem %s=%s", key, Z_STRVAL_P(data));
						noinsert = 0;
					} else {
						snprintf(remattribute, BUFFERLEN, "rem %s", key);
						noinsert = 0;
 					}
					break;
				case IS_ARRAY: {
					int i, len, keylen, count;
					char *strarr, *ptr, *ptr1;
					count = zend_hash_num_elements(Z_ARRVAL_P(data));
					if(count > 0) {
						strarr = make_objrec_from_array(Z_ARRVAL_P(data), ':');
						len = strlen(strarr) - 1;
						keylen = strlen(key);
						if(NULL == (ptr = malloc(len + 1 + count*(keylen+1+4)))) {
							if(modification)
								free(modification);
							RETURN_FALSE;
						}
						ptr1 = ptr;
						*ptr1 = '\0';
						strcpy(ptr1, "rem ");
						ptr1 += 4;
						strcpy(ptr1, key);
						ptr1 += keylen;
						*ptr1++ = '=';
						for(i=0; i<len; i++) {
							*ptr1++ = strarr[i];
							if(strarr[i] == '\n') {
								ptr1[-1] = '\\';
								strcpy(ptr1, "rem ");
								ptr1 += 4;
								strcpy(ptr1, key);
								ptr1 += keylen;
								*ptr1++ = '=';
							} /* else if(strarr[i] == '=')
								ptr1[-1] = ':'; */
						}
						*ptr1++ = '\n';
						*ptr1 = '\0';
						strlcpy(remattribute, ptr, sizeof(remattribute));
						noinsert = 0;
					}
					break;
				}
			}
			if(!noinsert) {
				modification = fnInsStr(modification, 0, "\\");
				modification = fnInsStr(modification, 0, remattribute);
			}
			zend_hash_move_forward(remobjarr);
		}
	}

	set_swap(ptr->swap_on);
	modification[strlen(modification)-1] = '\0';
	if(strlen(modification) == 0) {
		ptr->lasterror = 0;
		free(modification);
		RETURN_TRUE;
	}
/*	fprintf(stderr, "modifyobject: %s\n", modification); */
	switch(mode) {
		case 0:
			if (0 == (ptr->lasterror = send_lock(ptr->socket, id))) {
				if (0 == (ptr->lasterror = send_changeobject(ptr->socket, id, modification))) {
					if (0 != (ptr->lasterror = send_unlock(ptr->socket, id))) {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Aiii, Changeobject failed and couldn't unlock object (id = 0x%X)", id);
						free(modification);
						RETURN_FALSE;
					}
					free(modification);
					RETURN_FALSE;
				} else {
					send_unlock(ptr->socket, id);
					free(modification);
					RETURN_FALSE;
				}
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not lock object (id = 0x%X)", id);
				free(modification);
				RETURN_FALSE;
			}
			break;
		case 1:
/* WARNING: send_groupchangobject() only works right, if each attribute
   can be modified. Doing a changeobject recursively often tries to
   modify objects which cannot be modified e.g. because an attribute cannot
   be removed. In such a case no further modification on that object is done.
   Doing a 'rem Rights\add Rights=R:a' will fail completely if the attribute
   Rights is not there already. The object locking is done in send_groupchangeobject();
*/
			if (0 != (ptr->lasterror = send_groupchangeobject(ptr->socket, id, modification))) {
				free(modification);
				RETURN_FALSE;
			}
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Mode must be 0 or 1 (recursive)");
	}
	free(modification);
	RETURN_TRUE;
}
#undef BUFFERLEN
/* }}} */

/* {{{ php_hw_mvcp
 */
void php_hw_mvcp(INTERNAL_FUNCTION_PARAMETERS, int mvcp) {
	zval **arg1, **arg2, **arg3, **arg4;
	int link, type, dest=0, from=0, count;
	HashTable *src_arr;
	hw_connection *ptr;
	int collIDcount, docIDcount, i, *docIDs, *collIDs;

	switch(mvcp) {
		case MOVE: /* Move also has fromID */
			if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE)
				WRONG_PARAM_COUNT;
			break;
		case COPY:
			if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE)
				WRONG_PARAM_COUNT;
			break;
	}
	HW_FETCH_LINK(arg1);
	convert_to_array_ex(arg2);
	convert_to_long_ex(arg3);
	link=Z_LVAL_PP(arg1);
	src_arr=Z_ARRVAL_PP(arg2);
	switch(mvcp) {
		case MOVE: /* Move also has fromID, which is arg3 --> arg4 becomes destID */
			convert_to_long_ex(arg4);
			from=Z_LVAL_PP(arg3);
			dest=Z_LVAL_PP(arg4);
			break;
		case COPY: /* No fromID for Copy needed --> arg3 is destID */
			dest=Z_LVAL_PP(arg3);
			from = 0;
			break;
	}

	set_swap(ptr->swap_on);

	count = zend_hash_num_elements(src_arr);
	if(NULL == (collIDs = emalloc(count * sizeof(int)))) {
		RETURN_FALSE;
		}

	if(NULL == (docIDs = emalloc(count * sizeof(int)))) {
		efree(collIDs);
		RETURN_FALSE;
		}

	collIDcount = docIDcount = 0;
	zend_hash_internal_pointer_reset(src_arr);
	for(i=0; i<count; i++) {
		char *objrec;
		zval *keydata, **keydataptr;
		zend_hash_get_current_data(src_arr, (void **) &keydataptr);
		keydata = *keydataptr;
		if(Z_TYPE_P(keydata) == IS_LONG) {
			if(0 != (ptr->lasterror = send_getobject(ptr->socket, Z_LVAL_P(keydata), &objrec))) {
				efree(collIDs);
				efree(docIDs);
				RETURN_FALSE;
			}
			if(0 == fnAttributeCompare(objrec, "DocumentType", "collection"))
				collIDs[collIDcount++] = Z_LVAL_P(keydata);
			else
				docIDs[docIDcount++] = Z_LVAL_P(keydata);
			efree(objrec);
		}
		zend_hash_move_forward(src_arr);
	}

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
/* }}} */

/* {{{ proto void hw_mv(int link, array objrec, int from, int dest)
   Moves object */
PHP_FUNCTION(hw_mv)
{
	php_hw_mvcp(INTERNAL_FUNCTION_PARAM_PASSTHRU, MOVE);
}
/* }}} */

/* {{{ proto void hw_cp(int link, array objrec, int dest)
   Copies object */
PHP_FUNCTION(hw_cp)
{
	php_hw_mvcp(INTERNAL_FUNCTION_PARAM_PASSTHRU, COPY);
}
/* }}} */

/* {{{ proto hwdoc hw_gettext(int link, int objid [, int rootid])
   Returns text document. Links are relative to rootid if given */
PHP_FUNCTION(hw_gettext)
{
	zval **argv[3];
	int argc, link, id, type, mode;
	int rootid = 0;
	char *urlprefix;
	hw_document *doc;
	hw_connection *ptr;

	argc = ZEND_NUM_ARGS();
	if(argc > 3 || argc < 2 || (zend_get_parameters_array_ex(argc, argv) == FAILURE)) {
		WRONG_PARAM_COUNT;
	}	

	HW_FETCH_LINK(argv[0]);
	convert_to_long_ex(argv[1]);
	mode = 0;
	urlprefix = NULL;
	if(argc == 3) {
		switch(Z_TYPE_PP(argv[2])) {
			case IS_LONG:
				convert_to_long_ex(argv[2]);
				rootid = Z_LVAL_PP(argv[2]);
				mode = 1;
				break;
			case IS_STRING:	
				convert_to_string_ex(argv[2]);
				urlprefix = Z_STRVAL_PP(argv[2]);
				break;
		}
	}
	id=Z_LVAL_PP(argv[1]);

	set_swap(ptr->swap_on);
	{
	char *object = NULL;
	char *attributes = NULL;
	char *bodytag = NULL;
	int count;
	/* !!!! memory for object and attributes is allocated with malloc !!!! */
	if (0 != (ptr->lasterror = send_gettext(ptr->socket, id, mode, rootid, &attributes, &bodytag, &object, &count, urlprefix)))
		RETURN_FALSE;
	doc = malloc(sizeof(hw_document));
	doc->data = object;
	doc->attributes = attributes;
	doc->bodytag = bodytag;
	doc->size = count;
	Z_LVAL_P(return_value) = zend_list_insert(doc, le_document);
	Z_TYPE_P(return_value) = IS_LONG;
	}
}
/* }}} */

/* {{{ proto void hw_edittext(int link, hwdoc doc)
   Modifies text document */
PHP_FUNCTION(hw_edittext)
{
	zval **arg1, **arg2;
	int link, doc, type;
	hw_connection *ptr;
	hw_document *docptr;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);

	doc=Z_LVAL_PP(arg2);
	docptr = zend_list_find(doc, &type);

	if(!docptr || (type!=le_document)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find document identifier %d", doc);
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
   Returns the output of a CGI script */
#define BUFFERLEN 1000
/* FIX ME: The buffer cgi_env_str should be allocated dynamically */
PHP_FUNCTION(hw_getcgi)
{
	zval **arg1, **arg2;
	int link, id, type;
	hw_document *doc;
	hw_connection *ptr;
	char cgi_env_str[BUFFERLEN];

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	id=Z_LVAL_PP(arg2);

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
#ifdef PHP_WIN32
	snprintf(cgi_env_str, BUFFERLEN, "CGI_REQUEST_METHOD=%s\nCGI_PATH_INFO=%s\nCGI_QUERY_STRING=%s",
	                     getenv("REQUEST_METHOD"),
	                     getenv("PATH_INFO"),
	                     getenv("QUERY_STRING"));
#else
	snprintf(cgi_env_str, BUFFERLEN, "CGI_REQUEST_METHOD=%s\nCGI_PATH_INFO=%s\nCGI_QUERY_STRING=%s",
	                     SG(request_info).request_method,
	                     SG(request_info).request_uri,
	                     SG(request_info).query_string);
#endif
	/* !!!! memory for object and attributes is allocated with malloc !!!! */
	if (0 != (ptr->lasterror = send_getcgi(ptr->socket, id, cgi_env_str, &attributes, &object, &count)))
		RETURN_FALSE;
	doc = malloc(sizeof(hw_document));
	doc->data = object;
	doc->attributes = attributes;
	doc->bodytag = NULL;
	doc->size = count;
	Z_LVAL_P(return_value) = zend_list_insert(doc, le_document);
	Z_TYPE_P(return_value) = IS_LONG;
	}
}
#undef BUFFERLEN
/* }}} */

/* {{{ proto int hw_getremote(int link, int objid)
   Returns the content of a remote document */
PHP_FUNCTION(hw_getremote)
{
	zval **arg1, **arg2;
	int link, id, type;
	hw_document *doc;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	id=Z_LVAL_PP(arg2);

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
	Z_LVAL_P(return_value) = zend_list_insert(doc, le_document);
	Z_TYPE_P(return_value) = IS_LONG;
	}
}
/* }}} */

/* {{{ proto [array|int] hw_getremotechildren(int link, string objrec)
   Returns the remote document or an array of object records */
PHP_FUNCTION(hw_getremotechildren)
{
	zval **arg1, **arg2;
	int link, type, i;
	hw_connection *ptr;
	char *objrec;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_string_ex(arg2);
	objrec=Z_STRVAL_PP(arg2);

	set_swap(ptr->swap_on);
	{
	int count, *offsets;
	char *remainder, *ptr1;
	if (0 != (ptr->lasterror = send_getremotechildren(ptr->socket, objrec, &remainder, &offsets, &count)))
		RETURN_FALSE;

/*
for(i=0;i<count;i++)
  php_printf("offset[%d] = %d--\n", i, offsets[i]);
php_printf("count = %d, remainder = <HR>%s---<HR>", count, remainder);
*/
	/* The remainder depends on the number of returned objects and
	   whether the MimeType of the object to retrieve is set. If
	   the MimeType is set the result will start with the
	   HTTP header 'Content-type: mimetype', otherwise it will be
	   a list of object records and therefore starts with
	   'ObjectID=0'. In the first case the offset and count are somewhat
	   strange. Quite often count had a value of 6 which appears to be
	   meaningless, but if you sum up the offsets you get the length
	   of the remainder which is the lenght of the document.
	   The document must have been chopped up into 6 pieces, each ending
	   with 'ServerId=0xYYYYYYYY'.
	   In the second case the offset contains the lenght of
	   each object record; count contains the number of object records.
	   Even if a remote object has children
	   (several sql statements) but the MimeType is set, it will
	   return a document in the format of MimeType. On the other
	   hand a remote object does not have any children but just
	   returns a docuement will not be shown unless the MimeType
	   is set. It returns the pure object record of the object without
	   the SQLStatement attribute. Quite senseless.
           Though, this behavior depends on how the hgi gateway in Hyperwave
	   is implemented.
	*/
	if(strncmp(remainder, "ObjectID=0 ", 10)) {
		hw_document *doc;
		char *ptr;
		int i, j, len;
		/* For some reason there is always the string
		   'SeverId=0xYYYYYYYY' at the end, so we cut it off.
		   The document may as well be divided into several pieces
		   and each of them has the ServerId at the end.
		   The following will put the pieces back together and
		   strip the ServerId. count contains the number of pieces.
		*/
		for(i=0, len=0; i<count; i++)
			len += offsets[i]-18;
/*fprintf(stderr, "len = %d\n", len); */
		doc = malloc(sizeof(hw_document));
		doc->data = malloc(len+1);
		ptr = doc->data;
		for(i=0, j=0; i<count; i++) {
			memcpy((char *)ptr, (char *)&remainder[j], offsets[i]-18);
/*fprintf(stderr, "rem = %s\n", &remainder[j]); */
			j += offsets[i];
			ptr += offsets[i] - 18;
		}
		*ptr = '\0';
		doc->attributes = strdup(objrec);
		doc->bodytag = NULL;
		doc->size = strlen(doc->data);
		Z_LVAL_P(return_value) = zend_list_insert(doc, le_document);
		Z_TYPE_P(return_value) = IS_LONG;
	} else {
		array_init(return_value);

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
PHP_FUNCTION(hw_setlinkroot)
{
	zval **arg1, **arg2;
	int link, type, rootid;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	rootid = Z_LVAL_PP(arg2);

	ptr->linkroot = rootid;
	RETURN_LONG(rootid);
}
/* }}} */

/* {{{ proto hwdoc hw_pipedocument(int link, int objid [, array urlprefixes])
   Returns document with links inserted. Optionally a array with five urlprefixes may be passed, which will be inserted for the different types of anchors. This should be a named array with the following keys: HW_DEFAULT_LINK, HW_IMAGE_LINK, HW_BACKGROUND_LINK, HW_INTAG_LINK, and HW_APPLET_LINK */
PHP_FUNCTION(hw_pipedocument)
{
	zval **arg1, **arg2, **arg3;
	int i, link, id, type, argc, mode;
	int rootid = 0;
	HashTable *prefixarray;
	char **urlprefix;
	hw_connection *ptr;
	hw_document *doc;
#if APACHE
	server_rec *serv = ((request_rec *) SG(server_context))->server;
#endif

	argc = ZEND_NUM_ARGS();
	switch(argc)
	{
	case 2:
		if (zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
			WRONG_PARAM_COUNT;
		break;
	case 3:
		if (zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE)
			WRONG_PARAM_COUNT;
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	
	id=Z_LVAL_PP(arg2);

	/* check for the array with urlprefixes */
	if(argc == 3) {
		convert_to_array_ex(arg3);
		prefixarray =Z_ARRVAL_PP(arg3);
		if((prefixarray == NULL) || (zend_hash_num_elements(prefixarray) != 5)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "You must provide 5 urlprefixes (you have provided %d)", zend_hash_num_elements(prefixarray));
			RETURN_FALSE;
		}

		urlprefix = emalloc(5*sizeof(char *));
		zend_hash_internal_pointer_reset(prefixarray);
		for(i=0; i<5; i++) {
			char *key;
			zval *data, **dataptr;
			ulong ind;
			
			zend_hash_get_current_key(prefixarray, &key, &ind, 0);
			zend_hash_get_current_data(prefixarray, (void *) &dataptr);
			data = *dataptr;
			if (Z_TYPE_P(data) != IS_STRING) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s must be a String", key);
				RETURN_FALSE;
			} else if ( strcmp(key, "HW_DEFAULT_LINK") == 0 ) {
				urlprefix[HW_DEFAULT_LINK] = Z_STRVAL_P(data);
			} else if ( strcmp(key, "HW_IMAGE_LINK") == 0 ) {
				urlprefix[HW_IMAGE_LINK] = Z_STRVAL_P(data);
			} else if ( strcmp(key, "HW_BACKGROUND_LINK") == 0 ) {
				urlprefix[HW_BACKGROUND_LINK] = Z_STRVAL_P(data);
			} else if ( strcmp(key, "HW_INTAG_LINK") == 0 ) {
				urlprefix[HW_INTAG_LINK] = Z_STRVAL_P(data);
			} else if ( strcmp(key, "HW_APPLET_LINK") == 0 ) {
				urlprefix[HW_APPLET_LINK] = Z_STRVAL_P(data);
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s is not a valid urlprefix", key);
				RETURN_FALSE;
			}
			zend_hash_move_forward(prefixarray);
		}
	} else {
		urlprefix = NULL;
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
  getenv("HOSTNAME"),
#endif
   id, mode, rootid, &attributes, &bodytag, &object, &count, urlprefix)))
		RETURN_FALSE;
		
	if(urlprefix) efree(urlprefix);

	doc = malloc(sizeof(hw_document));
	doc->data = object;
	doc->attributes = attributes;
	doc->bodytag = bodytag;
	doc->size = count;
/* fprintf(stderr, "size = %d\n", count); */
	Z_LVAL_P(return_value) = zend_list_insert(doc, le_document);
	Z_TYPE_P(return_value) = IS_LONG;
	}
}
/* }}} */

/* {{{ proto hwdoc hw_pipedocument(int link, int objid)
   Returns document */
PHP_FUNCTION(hw_oldpipedocument)
{
	zval **argv[3];
	int link, id, type, argc, mode;
	int rootid = 0;
	hw_connection *ptr;
	hw_document *doc;
#if APACHE
	server_rec *serv = ((request_rec *) SG(server_context))->server;
#endif

	argc = ZEND_NUM_ARGS();
	if(argc != 2 || (zend_get_parameters_array_ex(argc, argv) == FAILURE)) {
		WRONG_PARAM_COUNT;
	}

	HW_FETCH_LINK(argv[0]);
	convert_to_long_ex(argv[1]);
/*	if(argc == 3) {
		convert_to_long_ex(argv[2]);
		rootid = Z_LVAL_PP(argv[2]);
		if(rootid != 0)
			mode = 1;
	}
*/
	id=Z_LVAL_PP(argv[1]);

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
  getenv("HOSTNAME"),
#endif
   id, mode, rootid, &attributes, &bodytag, &object, &count, NULL)))
		RETURN_FALSE;

	doc = malloc(sizeof(hw_document));
	doc->data = object;
	doc->attributes = attributes;
	doc->bodytag = bodytag;
	doc->size = count;
/* fprintf(stderr, "size = %d\n", count); */
	Z_LVAL_P(return_value) = zend_list_insert(doc, le_document);
	Z_TYPE_P(return_value) = IS_LONG;
	}
}
/* }}} */

/* {{{ proto hwdoc hw_pipecgi(int link, int objid)
   Returns output of CGI script */
#define BUFFERLEN 1000
/* FIX ME: The buffer cgi_env_str should be allocated dynamically */
PHP_FUNCTION(hw_pipecgi)
{
	zval **arg1, **arg2;
	int link, id, type;
	hw_connection *ptr;
	hw_document *doc;
	char cgi_env_str[1000];
#if APACHE
	server_rec *serv = ((request_rec *) SG(server_context))->server;
#endif

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	id=Z_LVAL_PP(arg2);

	set_swap(ptr->swap_on);
	{
	char *object = NULL;
	char *attributes = NULL;
	int count;

#ifdef PHP_WIN32
	snprintf(cgi_env_str, BUFFERLEN, "CGI_REQUEST_METHOD=%s\nCGI_PATH_INFO=%s\nCGI_QUERY_STRING=%s",
	                     getenv("REQUEST_METHOD"),
	                     getenv("PATH_INFO"),
	                     getenv("QUERY_STRING"));
#else
	snprintf(cgi_env_str, BUFFERLEN, "CGI_REQUEST_METHOD=%s\nCGI_PATH_INFO=%s\nCGI_QUERY_STRING=%s",
	                     SG(request_info).request_method,
	                     SG(request_info).request_uri,
	                     SG(request_info).query_string);
#endif
	/* !!!! memory for object, bodytag and attributes is allocated with malloc !!!! */
	if (0 != (ptr->lasterror =  send_pipecgi(ptr->socket,
#if APACHE
  serv->server_hostname,
#else
  getenv("HOSTNAME"),
#endif
  id, cgi_env_str, &attributes, &object, &count)))
		RETURN_FALSE;

	doc = malloc(sizeof(hw_document));
	doc->data = object;
	doc->attributes = attributes;
	doc->bodytag = NULL;
	doc->size = count;
	Z_LVAL_P(return_value) = zend_list_insert(doc, le_document);
	Z_TYPE_P(return_value) = IS_LONG;
	}
}
#undef BUFFERLEN
/* }}} */

/* {{{ proto void hw_insertdocument(int link, int parentid, hwdoc doc) 
   Insert new document */
PHP_FUNCTION(hw_insertdocument)
{
	zval **arg1, **arg2, **arg3;
	int link, id, doc, type;
	hw_connection *ptr;
	hw_document *docptr;
	hw_objectID objid;
#if APACHE
	server_rec *serv = ((request_rec *) SG(server_context))->server;
#endif

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);
	id=Z_LVAL_PP(arg2);

	doc=Z_LVAL_PP(arg3);
	docptr = zend_list_find(doc, &type);
	if(!docptr || (type!=le_document)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find document identifier %d", doc);
		RETURN_FALSE;
	}

	set_swap(ptr->swap_on);
	{
	if (0 != (ptr->lasterror =  send_putdocument(ptr->socket,
#if APACHE
  serv->server_hostname,
#else
  getenv("HOSTNAME"),
#endif
             id, docptr->attributes, docptr->data, docptr->size, &objid))) {
		RETURN_FALSE;
		}
	}
	RETURN_LONG(objid);
}
/* }}} */

/* {{{ proto hwdoc hw_new_document(string objrec, string data, int size)
   Create a new document */
PHP_FUNCTION(hw_new_document)
{
	zval **arg1, **arg2, **arg3;
	char *ptr;
	hw_document *doc;

	if (ZEND_NUM_ARGS() != 3 || (zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE)) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg1);
	convert_to_string_ex(arg2);
	convert_to_long_ex(arg3);

	doc = malloc(sizeof(hw_document));
	if(NULL == doc)
		RETURN_FALSE;
	doc->data = malloc(Z_LVAL_PP(arg3)+1);
	if(NULL == doc->data) {
		free(doc);
		RETURN_FALSE;
	}
	memcpy(doc->data, Z_STRVAL_PP(arg2), Z_LVAL_PP(arg3));
	ptr = doc->data;
	ptr[Z_LVAL_PP(arg3)] = '\0';
	doc->attributes = strdup(Z_STRVAL_PP(arg1));
	doc->bodytag = NULL;
	doc->size = Z_LVAL_PP(arg3);
	Z_LVAL_P(return_value) = zend_list_insert(doc, le_document);
	Z_TYPE_P(return_value) = IS_LONG;
}
/* }}} */

/* {{{ proto hwdoc hw_new_document_from_file(string objrec, string filename)
   Create a new document from a file */
PHP_FUNCTION(hw_new_document_from_file)
{
	pval **arg1, **arg2;
	char *ptr;
	php_stream *stream;
	int use_include_path=0;
	hw_document *doc;

	if (ZEND_NUM_ARGS() != 2 || (zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg1);
	convert_to_string_ex(arg2);

	stream = php_stream_open_wrapper(Z_STRVAL_PP(arg2), "r", use_include_path|ENFORCE_SAFE_MODE|REPORT_ERRORS,
			NULL TSRMLS_CC);

	if (stream == NULL)	{
		RETURN_FALSE;
	}

	doc = malloc(sizeof(hw_document));
	if(NULL == doc)
		RETURN_FALSE;

	doc->size = php_stream_copy_to_mem(stream, &doc->data, PHP_STREAM_COPY_ALL, 1);

	php_stream_close(stream);

	/* I'm not sure if it is necessary to add a '\0'. It depends on whether
	 * PHP-Strings has to be null terminated. doc->size doesn't count the
	 * '\0'.
	 */
	doc->data = realloc(doc->data, doc->size+1);
	ptr = doc->data;
	ptr[doc->size] = '\0';
	doc->attributes = strdup(Z_STRVAL_PP(arg1));
	doc->bodytag = NULL;
	Z_LVAL_P(return_value) = zend_list_insert(doc, le_document);
	Z_TYPE_P(return_value) = IS_LONG;
}
/* }}} */

/* {{{ proto void hw_free_document(hwdoc doc)
   Frees memory of document */
PHP_FUNCTION(hw_free_document)
{
	zval **arg1;
	int id, type;
	hw_document *ptr;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_ID(arg1);
	zend_list_delete(id);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void hw_outputdocument(hwdoc doc)
   An alias for hw_output_document */
/* }}} */

/* {{{ proto void hw_output_document(hwdoc doc)
   Prints document */
PHP_FUNCTION(hw_output_document)
{
	zval **arg1;
	int id, type;
	hw_document *ptr;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_ID(arg1);

	php_write(ptr->data, ptr->size TSRMLS_CC);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string hw_documentbodytag(hwdoc doc [, string prefix])
   An alias for hw_document_bodytag */
/* }}} */

/* {{{ proto string hw_document_bodytag(hwdoc doc [, string prefix])
   Return bodytag prefixed by prefix */
PHP_FUNCTION(hw_document_bodytag)
{
	zval **argv[2];
	int id, type, argc;
	hw_document *ptr;
	char *temp, *str = NULL;

	argc = ZEND_NUM_ARGS();
	if(argc > 2 || argc < 1 || (zend_get_parameters_array_ex(argc, argv) == FAILURE)) {
		WRONG_PARAM_COUNT;
	}
	
	HW_FETCH_ID(argv[0]);

	if(argc == 2) {
		convert_to_string_ex(argv[1]);
		str=Z_STRVAL_PP(argv[1]);
	}

	if(str != NULL) {
		temp = emalloc(Z_STRLEN_PP(argv[1]) + strlen(ptr->bodytag) + 2);
		strcpy(temp, ptr->bodytag);
		strcpy(temp+strlen(ptr->bodytag)-1, str);
		strcpy(temp+strlen(ptr->bodytag)-1+Z_STRLEN_PP(argv[1]), ">\n");
		RETURN_STRING(temp, 0);
	} else {
/* fprintf(stderr, "hw_document_bodytag: %s (%s)\n", ptr->bodytag, ptr->attributes); */
		if(ptr->bodytag) {
			RETURN_STRING(ptr->bodytag, 1);
		} else {
			RETURN_EMPTY_STRING();
		}
	}
}
/* }}} */

/* {{{ proto string hw_document_content(hwdoc doc)
   Returns content of document */
PHP_FUNCTION(hw_document_content)
{
	zval **arg1;
	int id, type;
	hw_document *ptr;

	if (ZEND_NUM_ARGS() != 1 || (zend_get_parameters_ex(1, &arg1) == FAILURE)) {
		RETURN_FALSE;
	}
	
	HW_FETCH_ID(arg1);

	RETURN_STRINGL(ptr->data, ptr->size, 1);
}
/* }}} */

/* {{{ proto int hw_document_setcontent(hwdoc doc, string content)
   Sets/replaces content of document */
PHP_FUNCTION(hw_document_setcontent)
{
	zval **arg1, **arg2;
	int id, type;
	hw_document *ptr;
	char *str;

	if (ZEND_NUM_ARGS() != 2 || (zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)) {
		WRONG_PARAM_COUNT;
	}
	
	HW_FETCH_ID(arg1);
	convert_to_string_ex(arg2);

	str = ptr->data;
	if(NULL != (ptr->data = strdup(Z_STRVAL_PP(arg2)))) {
		ptr->size = strlen(ptr->data);
		free(str);
		RETURN_TRUE;
	} else {
		ptr->data = str;
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int hw_documentsize(hwdoc doc)
   An alias for hw_document_size */
/* }}} */

/* {{{ proto int hw_document_size(hwdoc doc)
   Returns size of document */
PHP_FUNCTION(hw_document_size)
{
	zval **arg1;
	int id, type;
	hw_document *ptr;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_ID(arg1);

	RETURN_LONG(ptr->size);
}
/* }}} */

/* {{{ proto string hw_documentattributes(hwdoc doc)
   An alias for hw_document_attributes */
/* }}} */

/* {{{ proto string hw_document_attributes(hwdoc doc)
   Returns object record of document */
PHP_FUNCTION(hw_document_attributes)
{
	zval **arg1;
	int id, type;
	hw_document *ptr;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_ID(arg1);

	RETURN_STRING(ptr->attributes, 1);
/*	make_return_array_from_objrec(&return_value, ptr->attributes); */
}
/* }}} */

/* {{{ proto array hw_getparentsobj(int link, int objid)
   Returns array of parent object records */
PHP_FUNCTION(hw_getparentsobj)
{
	zval **arg1, **arg2;
	int link, id, type;
	int count;
	char  **childObjRecs = NULL;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	id=Z_LVAL_PP(arg2);

	set_swap(ptr->swap_on);

	if (0 != (ptr->lasterror = send_getparentsobj(ptr->socket, id, &childObjRecs, &count))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Command returned %d\n", ptr->lasterror);
		RETURN_FALSE;
	}

	/* create return value and free all memory */
	if( 0 > make_return_objrec(&return_value, childObjRecs, count))
		RETURN_FALSE;
}
/* }}} */

/* {{{ proto array hw_getparents(int link, int objid)
   Returns array of parent object ids */
PHP_FUNCTION(hw_getparents)
{
	zval **arg1, **arg2;
	int link, id, type;
	int count;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	id=Z_LVAL_PP(arg2);

	set_swap(ptr->swap_on);
        {
	int  *childIDs = NULL;
	int i;

	if (0 != (ptr->lasterror = send_getparents(ptr->socket, id, &childIDs, &count))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Command returned %d\n", ptr->lasterror);
		RETURN_FALSE;
	}

	array_init(return_value);

	for(i=0; i<count; i++) {
		add_index_long(return_value, i, childIDs[i]);
	}
	efree(childIDs);
	}

}
/* }}} */

/* {{{ proto array hw_children(int link, int objid)
   Returns array of children object ids */
PHP_FUNCTION(hw_children)
{
	zval **arg1, **arg2;
	int link, id, type;
	int count;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	id=Z_LVAL_PP(arg2);

	set_swap(ptr->swap_on);
	{
	int  *childIDs = NULL;
	int i;

	if (0 != (ptr->lasterror = send_children(ptr->socket, id, &childIDs, &count))){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Command returned %d\n", ptr->lasterror);
		RETURN_FALSE;
	}

	array_init(return_value);

	for(i=0; i<count; i++) {
		add_index_long(return_value, i, childIDs[i]);
	}
	efree(childIDs);
	}
		
}
/* }}} */

/* {{{ proto array hw_childrenobj(int link, int objid)
   Returns array of children object records */
PHP_FUNCTION(hw_childrenobj)
{
	pval **arg1, **arg2;
	int link, id, type;
	int count;
	char  **childObjRecs = NULL;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	id=Z_LVAL_PP(arg2);

	set_swap(ptr->swap_on);

	if (0 != (ptr->lasterror = send_childrenobj(ptr->socket, id, &childObjRecs, &count))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Command returned %d\n", ptr->lasterror);
		RETURN_FALSE;
	}

	/* create return value and free all memory */
	if( 0 > make_return_objrec(&return_value, childObjRecs, count))
		RETURN_FALSE;
}
/* }}} */

/* {{{ proto array hw_getchildcoll(int link, int objid)
   Returns array of child collection object ids */
PHP_FUNCTION(hw_getchildcoll)
{
	pval **arg1, **arg2;
	int link, id, type;
	int count;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	id=Z_LVAL_PP(arg2);

	set_swap(ptr->swap_on);
	{
	int  *childIDs = NULL;
	int i;

	if (0 != (ptr->lasterror = send_getchildcoll(ptr->socket, id, &childIDs, &count))){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Command returned %d\n", ptr->lasterror);
		RETURN_FALSE;
	}

	array_init(return_value);

	for(i=0; i<count; i++) {
		add_index_long(return_value, i, childIDs[i]);
	}
	efree(childIDs);
	}
		
}
/* }}} */

/* {{{ proto array hw_getchildcollobj(int link, int objid)
   Returns array of child collection object records */
PHP_FUNCTION(hw_getchildcollobj)
{
	pval **arg1, **arg2;
	int link, id, type;
	int count;
	char  **childObjRecs = NULL;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	id=Z_LVAL_PP(arg2);

	set_swap(ptr->swap_on);

	if (0 != (ptr->lasterror = send_getchildcollobj(ptr->socket, id, &childObjRecs, &count))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Command returned %d\n", ptr->lasterror);
		RETURN_FALSE;
	}

	/* create return value and free all memory */
	if( 0 > make_return_objrec(&return_value, childObjRecs, count))
		RETURN_FALSE;
}
/* }}} */

/* {{{ proto int hw_docbyanchor(int link, int anchorid)
   Returns objid of document belonging to anchorid */
PHP_FUNCTION(hw_docbyanchor)
{
	pval **arg1, **arg2;
	int link, id, type;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	id=Z_LVAL_PP(arg2);

	set_swap(ptr->swap_on);
	{
	int objectID;
	if (0 != (ptr->lasterror = send_docbyanchor(ptr->socket, id, &objectID)))
		RETURN_FALSE;

	RETURN_LONG(objectID);
	}
}
/* }}} */

/* {{{ proto array hw_docbyanchorobj(int link, int anchorid)
   Returns object record of document belonging to anchorid */
PHP_FUNCTION(hw_docbyanchorobj)
{
	pval **arg1, **arg2;
	int link, id, type;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	id=Z_LVAL_PP(arg2);

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
}
/* }}} */

/* {{{ proto array hw_getobjectbyquery(int link, string query, int maxhits)
   Search for query and return maxhits objids */
PHP_FUNCTION(hw_getobjectbyquery)
{
	pval **arg1, **arg2, **arg3;
	int link, type, maxhits;
	char *query;
	int count, i;
	int  *childIDs = NULL;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_string_ex(arg2);
	convert_to_long_ex(arg3);
	query=Z_STRVAL_PP(arg2);
	maxhits=Z_LVAL_PP(arg3);
	if (maxhits < 0) maxhits=0x7FFFFFFF;

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_getobjbyquery(ptr->socket, query, maxhits, &childIDs, &count))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Command returned %d\n", ptr->lasterror);
		RETURN_FALSE;
	}

	array_init(return_value);

	for(i=0; i<count; i++)
		add_index_long(return_value, i, childIDs[i]);
	efree(childIDs);
}
/* }}} */

/* {{{ proto array hw_getobjectbyqueryobj(int link, string query, int maxhits)
   Search for query and return maxhits object records */
PHP_FUNCTION(hw_getobjectbyqueryobj)
{
	pval **arg1, **arg2, **arg3;
	int link, type, maxhits;
	char *query;
	int count;
	char  **childObjRecs = NULL;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_string_ex(arg2);
	convert_to_long_ex(arg3);
	query=Z_STRVAL_PP(arg2);
	maxhits=Z_LVAL_PP(arg3);
	if (maxhits < 0) maxhits=0x7FFFFFFF;

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_getobjbyqueryobj(ptr->socket, query, maxhits, &childObjRecs, &count))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Command returned %d\n", ptr->lasterror);
		RETURN_FALSE;
	}

	/* create return value and free all memory */
	if( 0 > make_return_objrec(&return_value, childObjRecs, count))
		RETURN_FALSE;
}
/* }}} */

/* {{{ proto array hw_getobjectbyquerycoll(int link, int collid, string query, int maxhits)
   Search for query in collection and return maxhits objids */
PHP_FUNCTION(hw_getobjectbyquerycoll)
{
	pval **arg1, **arg2, **arg3, **arg4;
	int link, id, type, maxhits;
	char *query;
	int count, i;
	hw_connection *ptr;
	int  *childIDs = NULL;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	convert_to_string_ex(arg3);
	convert_to_long_ex(arg4);
	id=Z_LVAL_PP(arg2);
	query=Z_STRVAL_PP(arg3);
	maxhits=Z_LVAL_PP(arg4);
	if (maxhits < 0) maxhits=0x7FFFFFFF;

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_getobjbyquerycoll(ptr->socket, id, query, maxhits, &childIDs, &count))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Command returned %d\n", ptr->lasterror);
		RETURN_FALSE;
	}

	array_init(return_value);

	for(i=0; i<count; i++)
		add_index_long(return_value, i, childIDs[i]);
	efree(childIDs);
}
/* }}} */

/* {{{ proto array hw_getobjectbyquerycollobj(int link, int collid, string query, int maxhits)
   Search for query in collection and return maxhits object records */
PHP_FUNCTION(hw_getobjectbyquerycollobj)
{
	pval **arg1, **arg2, **arg3, **arg4;
	int link, id, type, maxhits;
	char *query;
	int count;
	hw_connection *ptr;
	char  **childObjRecs = NULL;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	convert_to_string_ex(arg3);
	convert_to_long_ex(arg4);
	id=Z_LVAL_PP(arg2);
	query=Z_STRVAL_PP(arg3);
	maxhits=Z_LVAL_PP(arg4);
	if (maxhits < 0) maxhits=0x7FFFFFFF;

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_getobjbyquerycollobj(ptr->socket, id, query, maxhits, &childObjRecs, &count))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Command returned %d\n", ptr->lasterror);
		RETURN_FALSE;
	}

	/* create return value and free all memory */
	if( 0 > make_return_objrec(&return_value, childObjRecs, count))
		RETURN_FALSE;
}
/* }}} */

/* {{{ proto array hw_getobjectbyftquery(int link, string query, int maxhits)
   Search for query as fulltext and return maxhits objids */
PHP_FUNCTION(hw_getobjectbyftquery)
{
	pval **arg1, **arg2, **arg3;
	int link, type, maxhits;
	char *query;
	int count, i;
	int  *childIDs = NULL;
	float *weights;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_string_ex(arg2);
	convert_to_long_ex(arg3);
	query=Z_STRVAL_PP(arg2);
	maxhits=Z_LVAL_PP(arg3);
	if (maxhits < 0) maxhits=0x7FFFFFFF;

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_getobjbyftquery(ptr->socket, query, maxhits, &childIDs, &weights, &count))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Command returned %d\n", ptr->lasterror);
		RETURN_FALSE;
	}

	array_init(return_value) ;

	for(i=0; i<count; i++)
		add_index_long(return_value, i, childIDs[i]);
	efree(childIDs);
}
/* }}} */

/* {{{ proto array hw_getobjectbyftqueryobj(int link, string query, int maxhits)
   Search for query as fulltext and return maxhits object records */
PHP_FUNCTION(hw_getobjectbyftqueryobj)
{
	pval **arg1, **arg2, **arg3;
	int link, type, maxhits;
	char *query;
	int count;
	char  **childObjRecs = NULL;
	float *weights;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_string_ex(arg2);
	convert_to_long_ex(arg3);
	query=Z_STRVAL_PP(arg2);
	maxhits=Z_LVAL_PP(arg3);
	if (maxhits < 0) maxhits=0x7FFFFFFF;

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_getobjbyftqueryobj(ptr->socket, query, maxhits, &childObjRecs, &weights, &count))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Command returned %d\n", ptr->lasterror);
		RETURN_FALSE;
	}

	/* create return value and free all memory */
	if( 0 > make_return_objrec(&return_value, childObjRecs, count))
		RETURN_FALSE;
}
/* }}} */

/* {{{ proto array hw_getobjectbyftquerycoll(int link, int collid, string query, int maxhits)
   Search for fulltext query in collection and return maxhits objids */
PHP_FUNCTION(hw_getobjectbyftquerycoll)
{
	pval **arg1, **arg2, **arg3, **arg4;
	int link, id, type, maxhits;
	char *query;
	int count, i;
	hw_connection *ptr;
	int  *childIDs = NULL;
	float *weights;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	convert_to_string_ex(arg3);
	convert_to_long_ex(arg4);
	id=Z_LVAL_PP(arg2);
	query=Z_STRVAL_PP(arg3);
	maxhits=Z_LVAL_PP(arg4);
	if (maxhits < 0) maxhits=0x7FFFFFFF;

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_getobjbyftquerycoll(ptr->socket, id, query, maxhits, &childIDs, &weights, &count))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Command returned %d\n", ptr->lasterror);
		RETURN_FALSE;
	}

	array_init(return_value);

	for(i=0; i<count; i++)
		add_index_long(return_value, i, childIDs[i]);
	efree(childIDs);
}
/* }}} */

/* {{{ proto array hw_getobjectbyftquerycollobj(int link, int collid, string query, int maxhits)
   Search for fulltext query in collection and return maxhits object records */
PHP_FUNCTION(hw_getobjectbyftquerycollobj)
{
	pval **arg1, **arg2, **arg3, **arg4;
	int link, id, type, maxhits;
	char *query;
	int count;
	hw_connection *ptr;
	char  **childObjRecs = NULL;
	float *weights;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	convert_to_string_ex(arg3);
	convert_to_long_ex(arg4);
	id=Z_LVAL_PP(arg2);
	query=Z_STRVAL_PP(arg3);
	maxhits=Z_LVAL_PP(arg4);
	if (maxhits < 0) maxhits=0x7FFFFFFF;

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_getobjbyftquerycollobj(ptr->socket, id, query, maxhits, &childObjRecs, &weights, &count))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Command returned %d\n", ptr->lasterror);
		RETURN_FALSE;
	}

	/* create return value and free all memory */
	if( 0 > make_return_objrec(&return_value, childObjRecs, count))
		RETURN_FALSE;
}
/* }}} */

/* {{{ proto array hw_getchilddoccoll(int link, int objid)
   Returns all children ids which are documents */
PHP_FUNCTION(hw_getchilddoccoll)
{
	pval **arg1, **arg2;
	int link, id, type;
	int count, i;
	int  *childIDs = NULL;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	id=Z_LVAL_PP(arg2);

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_getchilddoccoll(ptr->socket, id, &childIDs, &count))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Command returned %d\n", ptr->lasterror);
		RETURN_FALSE;
	}

	array_init(return_value);

	for(i=0; i<count; i++)
		add_index_long(return_value, i, childIDs[i]);
	efree(childIDs);
}
/* }}} */

/* {{{ proto array hw_getchilddoccollobj(int link, int objid)
   Returns all children object records which are documents */
PHP_FUNCTION(hw_getchilddoccollobj)
{
	pval **arg1, **arg2;
	int link, id, type;
	int count;
	char  **childObjRecs = NULL;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	id=Z_LVAL_PP(arg2);

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_getchilddoccollobj(ptr->socket, id, &childObjRecs, &count))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Command returned %d\n", ptr->lasterror);
		RETURN_FALSE;
	}

	/* create return value and free all memory */
	if( 0 > make_return_objrec(&return_value, childObjRecs, count))
		RETURN_FALSE;

}
/* }}} */

/* {{{ proto array hw_getanchors(int link, int objid)
   Return all anchors of object */
PHP_FUNCTION(hw_getanchors)
{
	pval **arg1, **arg2;
	int link, id, type;
	int count, i;
	int  *anchorIDs = NULL;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	id=Z_LVAL_PP(arg2);

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_getanchors(ptr->socket, id, &anchorIDs, &count))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Command returned %d\n", ptr->lasterror);
		RETURN_FALSE;
	}

	array_init(return_value);

	for(i=0; i<count; i++)
		add_index_long(return_value, i, anchorIDs[i]);
	efree(anchorIDs);
}
/* }}} */

/* {{{ proto array hw_getanchorsobj(int link, int objid)
   Return all object records of anchors of object */
PHP_FUNCTION(hw_getanchorsobj)
{
	pval **arg1, **arg2;
	int link, id, type;
	int count;
	char  **anchorObjRecs = NULL;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	id=Z_LVAL_PP(arg2);

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_getanchorsobj(ptr->socket, id, &anchorObjRecs, &count))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Command returned %d\n", ptr->lasterror);
		RETURN_FALSE;
	}

	/* create return value and free all memory */
	if( 0 > make_return_objrec(&return_value, anchorObjRecs, count))
		RETURN_FALSE;
}
/* }}} */

/* {{{ proto string hw_getusername(int link)
   Returns the current user name */
PHP_FUNCTION(hw_getusername)
{
	pval **arg1;
	int link, type;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);

	Z_STRVAL_P(return_value) = estrdup(ptr->username);
	Z_STRLEN_P(return_value) = strlen(ptr->username);
	Z_TYPE_P(return_value) = IS_STRING;
}
/* }}} */

/* {{{ proto void hw_identify(int link, string username, string password)
   Identifies at Hyperwave server */
PHP_FUNCTION(hw_identify)
{
	pval **arg1, **arg2, **arg3;
	int link, type;
	char *name, *passwd, *userdata;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);
	name=Z_STRVAL_PP(arg2);
	passwd=Z_STRVAL_PP(arg3);

	set_swap(ptr->swap_on);
	{
	char *str;

	if (0 != (ptr->lasterror = send_identify(ptr->socket, name, passwd, &userdata))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Command returned %d\n", ptr->lasterror);
		if(ptr->username) free(ptr->username);
		ptr->username = NULL;
		RETURN_FALSE;
	}

	Z_STRVAL_P(return_value) = userdata;
	Z_STRLEN_P(return_value) = strlen(userdata);
	Z_TYPE_P(return_value) = IS_STRING;
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

/* {{{ proto array hw_objrec2array(string objrec, [array format])
   Returns object array of object record */
PHP_FUNCTION(hw_objrec2array)
{
	zval **arg1, **arg2;

	switch(ZEND_NUM_ARGS()) {
		case 1:
			if(zend_get_parameters_ex(1, &arg1) == FAILURE)
				WRONG_PARAM_COUNT;
			convert_to_string_ex(arg1);
			make2_return_array_from_objrec(&return_value, Z_STRVAL_PP(arg1), NULL);
			break;
		case 2:
			if(zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
				WRONG_PARAM_COUNT;
			convert_to_array_ex(arg2);
			convert_to_string_ex(arg1);
			make2_return_array_from_objrec(&return_value, Z_STRVAL_PP(arg1), *arg2);
			break;
		default:
			WRONG_PARAM_COUNT;
	}
}
/* }}} */

/* {{{ proto string hw_array2objrec(array objarr)
   Returns object record of object array */
PHP_FUNCTION(hw_array2objrec)
{
	pval **arg1;
	char *objrec, *retobj;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);
	objrec = make_objrec_from_array(Z_ARRVAL_PP(arg1), '=');
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
PHP_FUNCTION(hw_incollections)
{
	pval **arg1, **arg2, **arg3, **arg4;
	int type, link, i;
	hw_connection *ptr;
	int cobjids, ccollids, *objectIDs, *collIDs, cretids, *retIDs, retcoll;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_array_ex(arg2);
	convert_to_array_ex(arg3);
	convert_to_long_ex(arg4);
	retcoll=Z_LVAL_PP(arg4);

	cobjids = zend_hash_num_elements(Z_ARRVAL_PP(arg2));
	if(NULL == (objectIDs = make_ints_from_array(Z_ARRVAL_PP(arg2)))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not create Int Array from Array");
		RETURN_FALSE;
	}

	ccollids = zend_hash_num_elements(Z_ARRVAL_PP(arg3));
	if(NULL == (collIDs = make_ints_from_array(Z_ARRVAL_PP(arg3)))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not create Int Array from Array");
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

	array_init(return_value);

	for(i=0; i<cretids; i++)
		add_index_long(return_value, i, retIDs[i]);
	efree(retIDs);

}
/* }}} */

/* {{{ proto void hw_inscoll(int link, int parentid, array objarr)
   Inserts collection */
PHP_FUNCTION(hw_inscoll)
{
	pval **arg1, **arg2, **arg3;
	char *objrec;
	int id, newid, type, link;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	convert_to_array_ex(arg3);
	id=Z_LVAL_PP(arg2);

	if(NULL == (objrec = make_objrec_from_array(Z_ARRVAL_PP(arg3), '='))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not create Object Record from Array");
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

/* {{{ proto void hw_insdoc(int link, int parentid, string objrec [, string text])
   Inserts document */
PHP_FUNCTION(hw_insdoc)
{
	zval **argv[4];
	char *objrec, *text;
	int id, newid, type, link, argc;
	hw_connection *ptr;

	argc = ZEND_NUM_ARGS();
	if((argc < 3) || (argc > 4))
		WRONG_PARAM_COUNT;

	if (zend_get_parameters_array_ex(argc, argv) == FAILURE)
		WRONG_PARAM_COUNT;

	HW_FETCH_LINK(argv[0]);
	convert_to_long_ex(argv[1]);
	convert_to_string_ex(argv[2]);
	if(argc == 4) {
		convert_to_string_ex(argv[3]);
		text = Z_STRVAL_PP(argv[3]);
	} else {
		text = NULL;
	}
	id = Z_LVAL_PP(argv[1]);

	set_swap(ptr->swap_on);
        objrec = Z_STRVAL_PP(argv[2]);
	if (0 != (ptr->lasterror = send_insdoc(ptr->socket, id, objrec, text, &newid))) {
		RETURN_FALSE;
	}

	RETURN_LONG(newid);
}
/* }}} */

/* {{{ proto int hw_getsrcbydestobj(int link, int destid)
   Returns object id of source docuent by destination anchor */
PHP_FUNCTION(hw_getsrcbydestobj)
{
	pval **arg1, **arg2;
	int link, type, id;
	int count;
	char  **childObjRecs = NULL;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	id=Z_LVAL_PP(arg2);

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_getsrcbydest(ptr->socket, id, &childObjRecs, &count))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Command returned %d", ptr->lasterror);
		RETURN_FALSE;
	}

	/* create return value and free all memory */
	if( 0 > make_return_objrec(&return_value, childObjRecs, count))
		RETURN_FALSE;
}
/* }}} */

/* {{{ proto int hw_mapid(int link, int serverid, int destid)
   Returns virtual object id of document on remote Hyperwave server */
PHP_FUNCTION(hw_mapid)
{
	pval **arg1, **arg2, **arg3;
	int link, type, servid, id, virtid;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);
	servid=Z_LVAL_PP(arg2);
	id=Z_LVAL_PP(arg3);

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = send_mapid(ptr->socket, servid, id, &virtid))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Command returned %d", ptr->lasterror);
		RETURN_FALSE;
	}
	RETURN_LONG(virtid);
}
/* }}} */

/* {{{ proto string hw_getrellink(int link, int rootid, int sourceid, int destid)
   Get link from source to dest relative to rootid */
PHP_FUNCTION(hw_getrellink)
{
	pval **arg1, **arg2, **arg3, **arg4;
	int link, type;
	int rootid, destid, sourceid;
	char *anchorstr;
	hw_connection *ptr;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_long_ex(arg4);
	rootid=Z_LVAL_PP(arg2);
	sourceid=Z_LVAL_PP(arg3);
	destid=Z_LVAL_PP(arg4);

	set_swap(ptr->swap_on);
	if (0 != (ptr->lasterror = getrellink(ptr->socket, rootid, sourceid, destid, &anchorstr))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Command returned %d", ptr->lasterror);
		RETURN_FALSE;
	}

	RETURN_STRING(anchorstr, 0);
}
/* }}} */
	
/* {{{ proto string hw_insertanchors(int hwdoc, array anchorecs, array dest [, array urlprefixes])
   Inserts only anchors into text */
PHP_FUNCTION(hw_insertanchors)
{
	pval **arg1, **arg2, **arg3, **arg4;
	hw_document *hwdoc;
	int type, docid, error, argc, count;
	char **anchorrecs;
	char **dest;
	char **urlprefix;
	char *bodytag = NULL;
	HashTable *arrht;
	HashTable *prefixarray;

	argc = ZEND_NUM_ARGS();
	switch(argc)
	{
	case 3:
		if (zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE)
			WRONG_PARAM_COUNT;
		break;
	case 4:
		if (zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE)
			WRONG_PARAM_COUNT;
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg1);
	convert_to_array_ex(arg2);
	convert_to_array_ex(arg3);
	docid=Z_LVAL_PP(arg1);
	hwdoc = zend_list_find(docid, &type);
	if(!hwdoc || (type!=le_document)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find file identifier %d", docid);
		RETURN_FALSE;
	}

	/* check for the array with urlprefixes */
	if(argc == 4) {
		int i;
		convert_to_array_ex(arg4);
		prefixarray =Z_ARRVAL_PP(arg4);
		if((prefixarray == NULL) || (zend_hash_num_elements(prefixarray) != 5)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "You must provide 5 urlprefixes (you have provided %d)", zend_hash_num_elements(prefixarray));
			RETURN_FALSE;
		}

		urlprefix = emalloc(5*sizeof(char *));
		zend_hash_internal_pointer_reset(prefixarray);
		for(i=0; i<5; i++) {
			char *key;
			zval *data, **dataptr;
			ulong ind;
			
			zend_hash_get_current_key(prefixarray, &key, &ind, 0);
			zend_hash_get_current_data(prefixarray, (void *) &dataptr);
			data = *dataptr;
			if (Z_TYPE_P(data) != IS_STRING) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s must be a String", key);
				RETURN_FALSE;
			} else if ( strcmp(key, "HW_DEFAULT_LINK") == 0 ) {
				urlprefix[HW_DEFAULT_LINK] = Z_STRVAL_P(data);
			} else if ( strcmp(key, "HW_IMAGE_LINK") == 0 ) {
				urlprefix[HW_IMAGE_LINK] = Z_STRVAL_P(data);
			} else if ( strcmp(key, "HW_BACKGROUND_LINK") == 0 ) {
				urlprefix[HW_BACKGROUND_LINK] = Z_STRVAL_P(data);
			} else if ( strcmp(key, "HW_INTAG_LINK") == 0 ) {
				urlprefix[HW_INTAG_LINK] = Z_STRVAL_P(data);
			} else if ( strcmp(key, "HW_APPLET_LINK") == 0 ) {
				urlprefix[HW_APPLET_LINK] = Z_STRVAL_P(data);
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s is not a valid urlprefix", key);
				RETURN_FALSE;
			}
			zend_hash_move_forward(prefixarray);
		}
	} else {
		urlprefix = NULL;
	}

	if(zend_hash_num_elements(Z_ARRVAL_PP(arg2)) != zend_hash_num_elements(Z_ARRVAL_PP(arg3))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unequal number of elements in arrays");
		RETURN_FALSE;
	}

	/* Turn PHP-Array of strings into C-Array of strings */
	arrht = Z_ARRVAL_PP(arg2);
	anchorrecs = make_strs_from_array(arrht);
	arrht = Z_ARRVAL_PP(arg3);
	dest = make_strs_from_array(arrht);

	if (0 != (error = send_insertanchors(&(hwdoc->data), &count, anchorrecs, dest, zend_hash_num_elements(arrht), urlprefix, &bodytag))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Command returned %d", error);
		RETURN_FALSE;
	}
/*fprintf(stderr, "in hw_insertanchors: %s\n", hwdoc->attributes); */
	hwdoc->size = count;
	if(hwdoc->bodytag) free (hwdoc->bodytag);
	hwdoc->bodytag = bodytag;

	RETURN_TRUE;
}
/* }}} */
	

PHP_MINFO_FUNCTION(hw)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Hyperwave Support", "enabled");
	php_info_print_table_row(2, "HG-CSP Version", "7.17");
	php_info_print_table_end();
	DISPLAY_INI_ENTRIES();
}

/* {{{ proto void hw_connection_info(int link)
   Prints information about the connection to Hyperwave server */
PHP_FUNCTION(hw_connection_info)
{
	pval **arg1;
	hw_connection *ptr;
	int link, type;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	HW_FETCH_LINK(arg1);
	
	php_printf("Hyperwave Info:\nhost=%s,\nserver string=%s\nversion=%d\nswap=%d\n", ptr->hostname, ptr->server_string, ptr->version, ptr->swap_on);
}
/* }}} */

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
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
