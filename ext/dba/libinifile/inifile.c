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
   | Author: Marcus Boerger <helly@php.net>                               |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_globals.h"
#include "safe_mode.h"
#include "php_network.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "inifile.h"

/* ret = -1 means that database was opened for read-only
 * ret = 0  success
 * ret = 1  key already exists - nothing done
 */

/* {{{ inifile_version */
char *inifile_version() 
{
	return "1.0, $Revision$";
}
/* }}} */ 

/* {{{ inifile_free_key */
void inifile_key_free(key_type *key)
{
	if (key->group) {
		efree(key->group);
	}
	if (key->name) {
		efree(key->name);
	}
	memset(key, 0, sizeof(key_type));
}
/* }}} */

/* {{{ inifile_free_val */
void inifile_val_free(val_type *val)
{
	if (val->value) {
		efree(val->value);
	}
	memset(val, 0, sizeof(val_type));
}
/* }}} */

/* {{{ inifile_free_val */
void inifile_line_free(line_type *ln)
{
	inifile_key_free(&ln->key);
	inifile_val_free(&ln->val);
	ln->pos = 0;
}
/* }}} */

/* {{{ inifile_alloc */
inifile * inifile_alloc(php_stream *fp, int readonly, int persistent TSRMLS_DC)
{
	inifile *dba;
	int fd = 0;

	if (!readonly) {
		if (!php_stream_truncate_supported(fp)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't truncate this stream");
			return NULL;
		}
		if (SUCCESS != php_stream_cast(fp, PHP_STREAM_AS_FD, (void*)&fd, 1)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not cast stream");
			return NULL;
		}
	}
	dba = pemalloc(sizeof(inifile), persistent);
	memset(dba, 0, sizeof(inifile));
	dba->fp = fp;
	dba->fd = fd;
	dba->readonly = readonly;
	return dba;
}
/* }}} */

/* {{{ inifile_free */
void inifile_free(inifile *dba, int persistent)
{
	if (dba) {
		inifile_line_free(&dba->curr);
		inifile_line_free(&dba->next);
		pefree(dba, persistent);
	}
}
/* }}} */

/* {{{ inifile_key_split */
key_type inifile_key_split(const char *group_name)
{
	key_type key;
	char *name;
	
	if (group_name[0] == '[' && (name = strchr(group_name, ']')) != NULL) {
		key.group = estrndup(group_name+1, name - (group_name + 1));
		key.name = estrdup(name+1);
	} else {
		key.group = estrdup("");
		key.name = estrdup(group_name);
	}
	return key;
}
/* }}} */

/* {{{ inifile_key_string */
char * inifile_key_string(const key_type *key)
{
	if (key->group && *key->group) {
		char *result;
		spprintf(&result, 0, "[%s]%s", key->group, key->name ? key->name : "");
		return result;
	} else if (key->name) {
		return estrdup(key->name);
	} else {
		return NULL;
	}
}
/* }}} */

/* {{{ etrim */
static char *etrim(const char *str)
{
	char *val;
	size_t l;
	
	if (!str) {
		return NULL;
	}
	val = (char*)str;
	while (strchr(" \t\r\n", *val)) {
		val++;
	}
	l = strlen(val);
	while (l && (strchr(" \t\r\n", val[l-1]))) {
		l--;
	}
	return estrndup(val, l);
}
/* }}} */

/* {{{ inifile_findkey
 */
static int inifile_read(inifile *dba, line_type *ln TSRMLS_DC) {
	char *fline;
	char *pos;

	inifile_val_free(&ln->val);
	while ((fline = php_stream_gets(dba->fp, NULL, 0)) != NULL) {
		if (fline) {
			if (fline[0] == '[') {
				/* A value name cannot start with '['
				 * So either we find a ']' or we found an error
				 */
				pos = strchr(fline+1, ']');
				if (pos) {
					*pos = '\0';
					inifile_key_free(&ln->key);
					ln->key.group = etrim(fline+1);
					ln->key.name = estrdup("");
					ln->pos = php_stream_tell(dba->fp);
					efree(fline);
					return 1;
				} else {
					efree(fline);
					continue;
				}
			} else {
				pos = strchr(fline, '=');
				if (pos) {
					*pos = '\0';
					/* keep group or make empty if not existent */
					if (!ln->key.group) {
						ln->key.group = estrdup("");
					}
					if (ln->key.name) {
						efree(ln->key.name);
					}
					ln->key.name = etrim(fline);
					ln->val.value = etrim(pos+1);
					ln->pos = php_stream_tell(dba->fp);
					efree(fline);
					return 1;
				} else {
					/* simply ignore lines without '='
					 * those should be comments
					 */
					 efree(fline);
					 continue;
				}
			}
		}
	}
	inifile_line_free(ln);
	return 0;
}
/* }}} */

/* {{{ inifile_key_cmp */
/* 0 = EQUAL
 * 1 = GROUP-EQUAL,NAME-DIFFERENT
 * 2 = DIFFERENT
 */
static int inifile_key_cmp(const key_type *k1, const key_type *k2 TSRMLS_DC)
{
	assert(k1->group && k1->name && k2->group && k2->name);
	
	if (!strcasecmp(k1->group, k2->group)) {
		if (!strcasecmp(k1->name, k2->name)) {
			return 0;
		} else {
			return 1;
		}
	} else {
		return 2;
	}
}
/* }}} */

/* {{{ inifile_fetch
 */
val_type inifile_fetch(inifile *dba, const key_type *key, int skip TSRMLS_DC) {
	line_type ln = {{NULL,NULL},{NULL}};
	val_type val;
	int res, grp_eq = 0;

	if (skip == -1 && dba->next.key.group && dba->next.key.name && !inifile_key_cmp(&dba->next.key, key TSRMLS_CC)) {
		/* we got position already from last fetch */
		php_stream_seek(dba->fp, dba->next.pos, SEEK_SET);
	} else {
		/* specific instance or not same key -> restart search */
		/* the slow way: restart and seacrch */
		php_stream_rewind(dba->fp);
		inifile_line_free(&dba->next);
	}
	if (skip == -1) {
		skip = 0;
	}
	while(inifile_read(dba, &ln TSRMLS_CC)) {
		if (!(res=inifile_key_cmp(&ln.key, key TSRMLS_CC))) {
			if (!skip) {
				val.value = estrdup(ln.val.value ? ln.val.value : "");
				/* allow faster access by updating key read into next */
				inifile_line_free(&dba->next);
				dba->next = ln;
				dba->next.pos = php_stream_tell(dba->fp);
				return val;
			}
			skip--;
		} else if (res == 1) {
			grp_eq = 1;
		} else if (grp_eq) {
			/* we are leaving group now: that means we cannot find the key */
			break;
		}
	}
	inifile_line_free(&ln);
	dba->next.pos = php_stream_tell(dba->fp);
	return ln.val;
}
/* }}} */

/* {{{ inifile_firstkey
 */
int inifile_firstkey(inifile *dba TSRMLS_DC) {
	inifile_line_free(&dba->curr);
	dba->curr.pos = 0;
	return inifile_nextkey(dba TSRMLS_CC);
}
/* }}} */

/* {{{ inifile_nextkey
 */
int inifile_nextkey(inifile *dba TSRMLS_DC) {
	line_type ln = {{NULL,NULL},{NULL}};

	/*inifile_line_free(&dba->next); ??? */
	php_stream_seek(dba->fp, dba->curr.pos, SEEK_SET);
	ln.key.group = estrdup(dba->curr.key.group ? dba->curr.key.group : "");
	inifile_read(dba, &ln TSRMLS_CC);
	inifile_line_free(&dba->curr);
	dba->curr = ln;
	return ln.key.group || ln.key.name;
}	
/* }}} */

/* {{{ inifile_truncate
 */
static int inifile_truncate(inifile *dba, size_t size TSRMLS_DC)
{
	int res;

	if ((res=php_stream_truncate_set_size(dba->fp, size)) != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error in ftruncate: %d", res);
		return FAILURE;
	}
	php_stream_seek(dba->fp, size, SEEK_SET);
	return SUCCESS;
}
/* }}} */

/* {{{ inifile_find_group
 * if found pos_grp_start points to "[group_name]"
 */
static int inifile_find_group(inifile *dba, const key_type *key, size_t *pos_grp_start TSRMLS_DC) 
{
	int ret = FAILURE;

	php_stream_flush(dba->fp);
	php_stream_seek(dba->fp, 0, SEEK_SET);
	inifile_line_free(&dba->curr);
	inifile_line_free(&dba->next);

	if (key->group && strlen(key->group)) {
		int res;
		line_type ln = {{NULL,NULL},{NULL}};

		res = 1;
		while(inifile_read(dba, &ln TSRMLS_CC)) {
			if ((res=inifile_key_cmp(&ln.key, key TSRMLS_CC)) < 2) {
				ret = SUCCESS;
				break;
			}
			*pos_grp_start = php_stream_tell(dba->fp);
		}
		inifile_line_free(&ln);
	} else {
		*pos_grp_start = 0;
		ret = SUCCESS;
	}
	if (ret == FAILURE) {
		*pos_grp_start = php_stream_tell(dba->fp);
	}
	return ret;
}
/* }}} */

/* {{{ inifile_next_group
 * only valid after a call to inifile_find_group
 * if any next group is found pos_grp_start points to "[group_name]" or whitespace before that
 */
static int inifile_next_group(inifile *dba, const key_type *key, size_t *pos_grp_start TSRMLS_DC) 
{
	int ret = FAILURE;
	line_type ln = {{NULL,NULL},{NULL}};

	*pos_grp_start = php_stream_tell(dba->fp);
	ln.key.group = estrdup(key->group);
	while(inifile_read(dba, &ln TSRMLS_CC)) {
		if (inifile_key_cmp(&ln.key, key TSRMLS_CC) == 2) {
			ret = SUCCESS;
			break;
		}
		*pos_grp_start = php_stream_tell(dba->fp);
	}
	inifile_line_free(&ln);
	return ret;
}
/* }}} */

/* {{{ inifile_copy_to
 */
static int inifile_copy_to(inifile *dba, size_t pos_start, size_t pos_end, inifile **ini_copy TSRMLS_DC)
{
	php_stream *fp;
	
	if (pos_start == pos_end) {
		*ini_copy = NULL;
		return SUCCESS;
	}
	if ((fp = php_stream_temp_create(0, 64 * 1024)) == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not create temporary stream");
		*ini_copy = NULL;
		return FAILURE;
	}

	if ((*ini_copy = inifile_alloc(fp, 1, 0 TSRMLS_CC)) == NULL) {
		/* writes error */
		return FAILURE;
	}
	php_stream_seek(dba->fp, pos_start, SEEK_SET);
	if (!php_stream_copy_to_stream(dba->fp, fp, pos_end - pos_start)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not copy group [%d - %d] to temporary stream", pos_start, pos_end);
		return FAILURE;
	} 
	return SUCCESS;
}
/* }}} */

/* {{{ inifile_filter
 * copy from to dba while ignoring key name (group must equal)
 */
static int inifile_filter(inifile *dba, inifile *from, const key_type *key TSRMLS_DC) 
{
	size_t pos_start = 0, pos_next = 0, pos_curr;
	int ret = SUCCESS;
	line_type ln = {{NULL,NULL},{NULL}};

	php_stream_seek(from->fp, 0, SEEK_SET);
	php_stream_seek(dba->fp, 0, SEEK_END);
	while(inifile_read(from, &ln TSRMLS_CC)) {
		switch(inifile_key_cmp(&ln.key, key TSRMLS_CC)) {
		case 0:
			pos_curr = php_stream_tell(from->fp);
			if (pos_start != pos_next) {
				php_stream_seek(from->fp, pos_start, SEEK_SET);
				if (!php_stream_copy_to_stream(from->fp, dba->fp, pos_next - pos_start)) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not copy [%d - %d] from temporary stream", pos_next, pos_start);
					ret = FAILURE;
				}
				php_stream_seek(from->fp, pos_curr, SEEK_SET);
			}
			pos_next = pos_start = pos_curr;
			break;
		case 1:
			pos_next = php_stream_tell(from->fp);
			break;
		case 2:
			/* the function is meant to process only entries from same group */
			assert(0);
			break;
		}
	}
	if (pos_start != pos_next) {
		php_stream_seek(from->fp, pos_start, SEEK_SET);
		if (!php_stream_copy_to_stream(from->fp, dba->fp, pos_next - pos_start)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not copy [%d - %d] from temporary stream", pos_next, pos_start);
			ret = FAILURE;
		}
	}
	inifile_line_free(&ln);
	return SUCCESS;
}
/* }}} */

/* {{{ inifile_delete_replace_append
 */
static int inifile_delete_replace_append(inifile *dba, const key_type *key, const val_type *value, int append TSRMLS_DC) 
{
	size_t pos_grp_start, pos_grp_next;
	inifile *ini_tmp = NULL;
	php_stream *fp_tmp = NULL;
	int ret;

	/* 1) Search group start
	 * 2) Search next group
	 * 3) If not append: Copy group to ini_tmp
	 * 4) Open temp_stream and copy remainder
	 * 5) Truncate stream
	 * 6) If not append AND key.name given: Filtered copy back from ini_tmp 
	 *    to stream. Otherwise the user wanted to delete the group.
	 * 7) Append value if given
	 * 8) Append temporary stream
	 */

	assert(!append || (key->name && value)); /* missuse */

	/* 1 - 3 */
	inifile_find_group(dba, key, &pos_grp_start TSRMLS_CC);
	inifile_next_group(dba, key, &pos_grp_next TSRMLS_CC);
	if (append) {
		ret = SUCCESS;
	} else {
		ret = inifile_copy_to(dba, pos_grp_start, pos_grp_next, &ini_tmp TSRMLS_CC);
	}

	/* 4 */
	if (ret == SUCCESS) {
		fp_tmp = php_stream_temp_create(0, 64 * 1024);
		if (!fp_tmp) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not create temporary stream");
			ret = FAILURE;
		} else {
			php_stream_seek(dba->fp, 0, SEEK_END);
			if (pos_grp_next != php_stream_tell(dba->fp)) {
				php_stream_seek(dba->fp, pos_grp_next, SEEK_SET);
				if (!php_stream_copy_to_stream(dba->fp, fp_tmp, PHP_STREAM_COPY_ALL)) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not copy remainder to temporary stream");
					ret = FAILURE;
				}
			}
		}
	}
	
	/* 5 */
	if (ret == SUCCESS) {
		ret = inifile_truncate(dba, append ? pos_grp_next : pos_grp_start TSRMLS_CC); /* writes error on fail */
	}

	if (ret == SUCCESS) {
		if (key->name && strlen(key->name)) {
			/* 6 */
			if (!append && ini_tmp) {
				ret = inifile_filter(dba, ini_tmp, key TSRMLS_CC);
			}

			/* 7 */
			/* important: do not query ret==SUCCESS again: inifile_filter might fail but
			 * however next operation must be done.
			 */
			if (value) {
				if (pos_grp_start == pos_grp_next && key->group && strlen(key->group)) {
					php_stream_printf(dba->fp TSRMLS_CC, "[%s]\n", key->group);
				}
				php_stream_printf(dba->fp TSRMLS_CC, "%s=%s\n", key->name, value->value ? value->value : "");
			}
		}
		
		/* 8 */ 
		/* important: do not query ret==SUCCESS again: inifile_filter might fail but
		 * however next operation must be done.
		 */
		if (fp_tmp && php_stream_tell(fp_tmp)) {
			php_stream_seek(fp_tmp, 0, SEEK_SET);
			php_stream_seek(dba->fp, 0, SEEK_END);
			if (!php_stream_copy_to_stream(fp_tmp, dba->fp, PHP_STREAM_COPY_ALL)) {
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "Could not copy from temporary stream - ini file truncated");
				ret = FAILURE;
			}
		}
	}

	if (ini_tmp) {
		php_stream_close(ini_tmp->fp);
		inifile_free(ini_tmp, 0);
	}
	if (fp_tmp) {
		php_stream_close(fp_tmp);
	}
	php_stream_flush(dba->fp);
	php_stream_seek(dba->fp, 0, SEEK_SET);

	return ret;
}
/* }}} */

/* {{{ inifile_delete
 */
int inifile_delete(inifile *dba, const key_type *key TSRMLS_DC) 
{
	return inifile_delete_replace_append(dba, key, NULL, 0 TSRMLS_CC);
}	
/* }}} */

/* {{{ inifile_relace
 */
int inifile_replace(inifile *dba, const key_type *key, const val_type *value TSRMLS_DC) 
{
	return inifile_delete_replace_append(dba, key, value, 0 TSRMLS_CC);
}
/* }}} */

/* {{{ inifile_append
 */
int inifile_append(inifile *dba, const key_type *key, const val_type *value TSRMLS_DC) 
{
	return inifile_delete_replace_append(dba, key, value, 1 TSRMLS_CC);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
