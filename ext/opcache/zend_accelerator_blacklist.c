/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

#include "main/php.h"
#include "main/fopen_wrappers.h"
#include "ZendAccelerator.h"
#include "zend_accelerator_blacklist.h"

#if ZEND_EXTENSION_API_NO >= PHP_5_3_X_API_NO
# include "ext/ereg/php_regex.h"
#else
# include "main/php_regex.h"
#endif

#ifdef ZEND_WIN32
# define REGEX_MODE (REG_EXTENDED|REG_NOSUB|REG_ICASE)
#else
# define REGEX_MODE (REG_EXTENDED|REG_NOSUB)
#endif

#ifdef HAVE_GLOB
#ifdef PHP_WIN32
#include "win32/glob.h"
#else
#include <glob.h>
#endif
#endif

#define ZEND_BLACKLIST_BLOCK_SIZE	32

struct _zend_regexp_list {
	regex_t           comp_regex;
	zend_regexp_list *next;
};

zend_blacklist accel_blacklist;

void zend_accel_blacklist_init(zend_blacklist *blacklist)
{
	blacklist->pos = 0;
	blacklist->size = ZEND_BLACKLIST_BLOCK_SIZE;

	if (blacklist->entries != NULL) {
		zend_accel_blacklist_shutdown(blacklist);
	}

	blacklist->entries = (zend_blacklist_entry *) calloc(sizeof(zend_blacklist_entry), blacklist->size);
	if (!blacklist->entries) {
		zend_accel_error(ACCEL_LOG_FATAL, "Blacklist initialization: no memory\n");
		return;
	}
	blacklist->regexp_list = NULL;
}

static void blacklist_report_regexp_error(regex_t *comp_regex, int reg_err)
{
	char *errbuf;
	int errsize = regerror(reg_err, comp_regex, NULL, 0);
	errbuf = malloc(errsize);
	if (!errbuf) {
		zend_accel_error(ACCEL_LOG_ERROR, "Blacklist compilation: no memory\n");
		return;
	}
	regerror(reg_err, comp_regex, errbuf, errsize);
	zend_accel_error(ACCEL_LOG_ERROR, "Blacklist compilation: %s\n", errbuf);
	free(errbuf);
}

static void zend_accel_blacklist_update_regexp(zend_blacklist *blacklist)
{
	char *regexp;
	int i, j, clen, reg_err, end = 0, rlen = 6;
	zend_regexp_list **regexp_list_it, *it;

	if (blacklist->pos == 0) {
		/* we have no blacklist to talk about */
		return;
	}

	regexp_list_it = &(blacklist->regexp_list);
	for (i = 0; i < blacklist->pos; i++) {
		rlen += blacklist->entries[i].path_length * 2 + 2;

		/* don't create a regexp buffer bigger than 12K)*/
		if ((i + 1 == blacklist->pos) || ((rlen + blacklist->entries[i + 1].path_length * 2 + 2) > (12 * 1024))) {
			regexp = (char *)malloc(rlen);
			if (!regexp) {
				zend_accel_error(ACCEL_LOG_ERROR, "malloc() failed\n");
				return;
			}
			regexp[0] = '^';
			regexp[1] = '(';

			clen = 2;
			for (j = end; j <= i; j++) {

				int c;
				if (j != end) {
					regexp[clen++] = '|';
				}
				/* copy mangled filename */
				for (c = 0; c < blacklist->entries[j].path_length; c++) {
					if (strchr("^.[]$()|*+?{}\\", blacklist->entries[j].path[c])) {
						regexp[clen++] = '\\';
					}
					regexp[clen++] = blacklist->entries[j].path[c];
				}
			}
			regexp[clen++] = ')';
			regexp[clen] = '\0';

			it = (zend_regexp_list*)malloc(sizeof(zend_regexp_list));
			if (!it) {
				zend_accel_error(ACCEL_LOG_ERROR, "malloc() failed\n");
				return;
			}
			it->next = NULL;

			if ((reg_err = regcomp(&it->comp_regex, regexp, REGEX_MODE)) != 0) {
				blacklist_report_regexp_error(&it->comp_regex, reg_err);
			}
			/* prepare for the next iteration */
			free(regexp);
			end = i + 1;
			rlen = 6;
			*regexp_list_it = it;
			regexp_list_it = &it->next;
		}
	}
}

void zend_accel_blacklist_shutdown(zend_blacklist *blacklist)
{
	zend_blacklist_entry *p = blacklist->entries, *end = blacklist->entries + blacklist->pos;

	while (p<end) {
		free(p->path);
		p++;
	}
	free(blacklist->entries);
	blacklist->entries = NULL;
	if (blacklist->regexp_list) {
		zend_regexp_list *temp, *it = blacklist->regexp_list;
		while (it) {
			regfree(&it->comp_regex);
			temp = it;
			it = it->next;
			free(temp);
		}
	}
}

static inline void zend_accel_blacklist_allocate(zend_blacklist *blacklist)
{
	if (blacklist->pos == blacklist->size) {
		blacklist->size += ZEND_BLACKLIST_BLOCK_SIZE;
		blacklist->entries = (zend_blacklist_entry *) realloc(blacklist->entries, sizeof(zend_blacklist_entry)*blacklist->size);
	}
}

#ifdef HAVE_GLOB
static void zend_accel_blacklist_loadone(zend_blacklist *blacklist, char *filename)
#else
void zend_accel_blacklist_load(zend_blacklist *blacklist, char *filename)
#endif
{
	char buf[MAXPATHLEN + 1], real_path[MAXPATHLEN + 1];
	FILE *fp;
	int path_length;
	TSRMLS_FETCH();

	if ((fp = fopen(filename, "r")) == NULL) {
		zend_accel_error(ACCEL_LOG_WARNING, "Cannot load blacklist file: %s\n", filename);
		return;
	}

	zend_accel_error(ACCEL_LOG_DEBUG,"Loading blacklist file:  '%s'", filename);

	memset(buf, 0, sizeof(buf));
	memset(real_path, 0, sizeof(real_path));

	while (fgets(buf, MAXPATHLEN, fp) != NULL) {
		char *path_dup, *pbuf;
		path_length = strlen(buf);
		if (path_length > 0 && buf[path_length - 1] == '\n') {
			buf[--path_length] = 0;
			if (path_length > 0 && buf[path_length - 1] == '\r') {
				buf[--path_length] = 0;
			}
		}

		/* Strip ctrl-m prefix */
		pbuf = &buf[0];
		while (*pbuf == '\r') {
			*pbuf++ = 0;
			path_length--;
		}

		/* strip \" */
		if (pbuf[0] == '\"' && pbuf[path_length - 1]== '\"') {
			*pbuf++ = 0;
			path_length -= 2;
		}

		if (path_length == 0) {
			continue;
		}

		/* skip comments */
		if (pbuf[0]==';') {
			continue;
		}

		path_dup = zend_strndup(pbuf, path_length);
		expand_filepath(path_dup, real_path TSRMLS_CC);
		path_length = strlen(real_path);

		free(path_dup);

		zend_accel_blacklist_allocate(blacklist);
		blacklist->entries[blacklist->pos].path_length = path_length;
		blacklist->entries[blacklist->pos].path = (char *)malloc(path_length + 1);
		if (!blacklist->entries[blacklist->pos].path) {
			zend_accel_error(ACCEL_LOG_ERROR, "malloc() failed\n");
			return;
		}
		blacklist->entries[blacklist->pos].id = blacklist->pos;
		memcpy(blacklist->entries[blacklist->pos].path, real_path, path_length + 1);
		blacklist->pos++;
	}
	fclose(fp);
	zend_accel_blacklist_update_regexp(blacklist);
}

#ifdef HAVE_GLOB
void zend_accel_blacklist_load(zend_blacklist *blacklist, char *filename)
{
	glob_t globbuf;
	int    ret, i;

	memset(&globbuf, 0, sizeof(glob_t));

	ret = glob(filename, 0, NULL, &globbuf);
#ifdef GLOB_NOMATCH
	if (ret == GLOB_NOMATCH || !globbuf.gl_pathc) {
#else
	if (!globbuf.gl_pathc) {
#endif
		zend_accel_error(ACCEL_LOG_WARNING, "No blacklist file found matching: %s\n", filename);
	} else {
		for(i=0 ; i<globbuf.gl_pathc; i++) {
			zend_accel_blacklist_loadone(blacklist, globbuf.gl_pathv[i]);
		}
		globfree(&globbuf);
	}
}
#endif

zend_bool zend_accel_blacklist_is_blacklisted(zend_blacklist *blacklist, char *verify_path)
{
	int ret = 0;
	zend_regexp_list *regexp_list_it = blacklist->regexp_list;

	if (regexp_list_it == NULL) {
		return 0;
	}
	while (regexp_list_it != NULL) {
		if (regexec(&(regexp_list_it->comp_regex), verify_path, 0, NULL, 0) == 0) {
			ret = 1;
			break;
		}
		regexp_list_it = regexp_list_it->next;
	}
	return ret;
}

void zend_accel_blacklist_apply(zend_blacklist *blacklist, apply_func_arg_t func, void *argument TSRMLS_DC)
{
	int i;

	for (i = 0; i < blacklist->pos; i++) {
		func(&blacklist->entries[i], argument TSRMLS_CC);
	}
}
