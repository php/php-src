/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2017 The PHP Group                                |
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

#include "ext/pcre/php_pcre.h"

#define ZEND_BLACKLIST_BLOCK_SIZE	32

struct _zend_regexp_list {
	pcre             *re;
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

static void blacklist_report_regexp_error(const char *pcre_error, int pcre_error_offset)
{
	zend_accel_error(ACCEL_LOG_ERROR, "Blacklist compilation failed (offset: %d), %s\n", pcre_error_offset, pcre_error);
}

static void zend_accel_blacklist_update_regexp(zend_blacklist *blacklist)
{
	const char *pcre_error;
	int i, pcre_error_offset;
	zend_regexp_list **regexp_list_it, *it;
	char regexp[12*1024], *p, *end, *c, *backtrack = NULL;

	if (blacklist->pos == 0) {
		/* we have no blacklist to talk about */
		return;
	}

	regexp_list_it = &(blacklist->regexp_list);

	regexp[0] = '^';
	regexp[1] = '(';
	p = regexp + 2;
	end = regexp + sizeof(regexp) - sizeof("[^\\\\]*)\0");

	for (i = 0; i < blacklist->pos; ) {
		c = blacklist->entries[i].path;
		if (p + blacklist->entries[i].path_length < end) {
			while (*c && p < end) {
				switch (*c) {
					case '?':
						c++;
#ifdef ZEND_WIN32
				 		p[0] = '[';			/* * => [^\\] on Win32 */
					 	p[1] = '^';
					 	p[2] = '\\';
					 	p[3] = '\\';
					 	p[4] = ']';
						p += 5;
#else
					 	p[0] = '[';			/* * => [^/] on *nix */
					 	p[1] = '^';
					 	p[2] = '/';
					 	p[3] = ']';
						p += 4;
#endif
						break;
					case '*':
						c++;
						if (*c == '*') {
							c++;
						 	p[0] = '.';			/* ** => .* */
							p[1] = '*';
							p += 2;
						} else {
#ifdef ZEND_WIN32
						 	p[0] = '[';			/* * => [^\\]* on Win32 */
						 	p[1] = '^';
						 	p[2] = '\\';
						 	p[3] = '\\';
						 	p[4] = ']';
						 	p[5] = '*';
							p += 6;
#else
						 	p[0] = '[';			/* * => [^/]* on *nix */
						 	p[1] = '^';
						 	p[2] = '/';
						 	p[3] = ']';
						 	p[4] = '*';
							p += 5;
#endif
						}
						break;
					case '^':
					case '.':
					case '[':
					case ']':
					case '$':
					case '(':
					case ')':
					case '|':
					case '+':
					case '{':
					case '}':
					case '\\':
						*p++ = '\\';
						/* break missing intentionally */
					default:
						*p++ = *c++;
				}
			}
		}

		if (*c || i == blacklist->pos - 1) {
			if (*c) {
				if (!backtrack) {
					zend_accel_error(ACCEL_LOG_ERROR, "Too long blacklist entry\n");
				}
				p = backtrack;
			} else {
				i++;
			}
			*p++ = ')';
			*p++ = '\0';

			it = (zend_regexp_list*)malloc(sizeof(zend_regexp_list));
			if (!it) {
				zend_accel_error(ACCEL_LOG_ERROR, "malloc() failed\n");
				return;
			}
			it->next = NULL;

			if ((it->re = pcre_compile(regexp, PCRE_NO_AUTO_CAPTURE, &pcre_error, &pcre_error_offset, 0)) == NULL) {
				free(it);
				blacklist_report_regexp_error(pcre_error, pcre_error_offset);
				return;
			}
			/* prepare for the next iteration */
			p = regexp + 2;
			*regexp_list_it = it;
			regexp_list_it = &it->next;
		} else {
			backtrack = p;
			*p++ = '|';
			i++;
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
			pcre_free(it->re);
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
	char buf[MAXPATHLEN + 1], real_path[MAXPATHLEN + 1], *blacklist_path = NULL;
	FILE *fp;
	int path_length, blacklist_path_length;

	if ((fp = fopen(filename, "r")) == NULL) {
		zend_accel_error(ACCEL_LOG_WARNING, "Cannot load blacklist file: %s\n", filename);
		return;
	}

	zend_accel_error(ACCEL_LOG_DEBUG,"Loading blacklist file:  '%s'", filename);

	if (VCWD_REALPATH(filename, buf)) {
		blacklist_path_length = zend_dirname(buf, strlen(buf));
		blacklist_path = zend_strndup(buf, blacklist_path_length);
	}

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
		if (blacklist_path) {
			expand_filepath_ex(path_dup, real_path, blacklist_path, blacklist_path_length);
		} else {
			expand_filepath(path_dup, real_path);
		}
		path_length = strlen(real_path);

		free(path_dup);

		zend_accel_blacklist_allocate(blacklist);
		blacklist->entries[blacklist->pos].path_length = path_length;
		blacklist->entries[blacklist->pos].path = (char *)malloc(path_length + 1);
		if (!blacklist->entries[blacklist->pos].path) {
			zend_accel_error(ACCEL_LOG_ERROR, "malloc() failed\n");
			fclose(fp);
			return;
		}
		blacklist->entries[blacklist->pos].id = blacklist->pos;
		memcpy(blacklist->entries[blacklist->pos].path, real_path, path_length + 1);
		blacklist->pos++;
	}
	fclose(fp);
	if (blacklist_path) {
		free(blacklist_path);
	}
	zend_accel_blacklist_update_regexp(blacklist);
}

#ifdef HAVE_GLOB
void zend_accel_blacklist_load(zend_blacklist *blacklist, char *filename)
{
	glob_t globbuf;
	int    ret;
	unsigned int i;

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
		if (pcre_exec(regexp_list_it->re, NULL, verify_path, strlen(verify_path), 0, 0, NULL, 0) >= 0) {
			ret = 1;
			break;
		}
		regexp_list_it = regexp_list_it->next;
	}
	return ret;
}

void zend_accel_blacklist_apply(zend_blacklist *blacklist, blacklist_apply_func_arg_t func, void *argument)
{
	int i;

	for (i = 0; i < blacklist->pos; i++) {
		func(&blacklist->entries[i], argument);
	}
}
