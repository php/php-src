/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "main/php.h"
#include "main/fopen_wrappers.h"
#include "ZendAccelerator.h"
#include "zend_accelerator_blocklist.h"

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

#define ZEND_BLOCKLIST_BLOCK_SIZE	32

struct _zend_regexp_list {
	pcre2_code       *re;
	zend_regexp_list *next;
};

zend_blocklist accel_blocklist;

void zend_accel_blocklist_init(zend_blocklist *blocklist)
{
	blocklist->pos = 0;
	blocklist->size = ZEND_BLOCKLIST_BLOCK_SIZE;

	if (blocklist->entries != NULL) {
		zend_accel_blocklist_shutdown(blocklist);
	}

	blocklist->entries = (zend_blocklist_entry *) calloc(sizeof(zend_blocklist_entry), blocklist->size);
	if (!blocklist->entries) {
		zend_accel_error(ACCEL_LOG_FATAL, "Blocklist initialization: no memory\n");
		return;
	}
	blocklist->regexp_list = NULL;
}

static void blocklist_report_regexp_error(const char *pcre_error, int pcre_error_offset)
{
	zend_accel_error(ACCEL_LOG_ERROR, "Blocklist compilation failed (offset: %d), %s\n", pcre_error_offset, pcre_error);
}

static void zend_accel_blocklist_update_regexp(zend_blocklist *blocklist)
{
	PCRE2_UCHAR pcre_error[128];
	int i, errnumber;
	PCRE2_SIZE pcre_error_offset;
	zend_regexp_list **regexp_list_it, *it;
	char regexp[12*1024], *p, *end, *c, *backtrack = NULL;
	pcre2_compile_context *cctx = php_pcre_cctx();

	if (blocklist->pos == 0) {
		/* we have no blocklist to talk about */
		return;
	}

	regexp_list_it = &(blocklist->regexp_list);

	regexp[0] = '^';
	regexp[1] = '(';
	p = regexp + 2;
	end = regexp + sizeof(regexp) - sizeof("[^\\\\]*)\0");

	for (i = 0; i < blocklist->pos; ) {
		c = blocklist->entries[i].path;
		if (p + blocklist->entries[i].path_length < end) {
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

		if (*c || i == blocklist->pos - 1) {
			if (*c) {
				if (!backtrack) {
					zend_accel_error(ACCEL_LOG_ERROR, "Too long blocklist entry\n");
				}
				p = backtrack;
			} else {
				i++;
			}
			*p++ = ')';

			it = (zend_regexp_list*)malloc(sizeof(zend_regexp_list));
			if (!it) {
				zend_accel_error(ACCEL_LOG_ERROR, "malloc() failed\n");
				return;
			}
			it->next = NULL;

			if ((it->re = pcre2_compile((PCRE2_SPTR)regexp, p - regexp, PCRE2_NO_AUTO_CAPTURE, &errnumber, &pcre_error_offset, cctx)) == NULL) {
				free(it);
				pcre2_get_error_message(errnumber, pcre_error, sizeof(pcre_error));
				blocklist_report_regexp_error((char *)pcre_error, pcre_error_offset);
				return;
			}
#ifdef HAVE_PCRE_JIT_SUPPORT
			if (0 > pcre2_jit_compile(it->re, PCRE2_JIT_COMPLETE)) {
				/* Don't return here, even JIT could fail to compile, the pattern is still usable. */
				pcre2_get_error_message(errnumber, pcre_error, sizeof(pcre_error));
				zend_accel_error(ACCEL_LOG_WARNING, "Blocklist JIT compilation failed, %s\n", pcre_error);
			}
#endif
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

void zend_accel_blocklist_shutdown(zend_blocklist *blocklist)
{
	zend_blocklist_entry *p = blocklist->entries, *end = blocklist->entries + blocklist->pos;

	while (p<end) {
		free(p->path);
		p++;
	}
	free(blocklist->entries);
	blocklist->entries = NULL;
	if (blocklist->regexp_list) {
		zend_regexp_list *temp, *it = blocklist->regexp_list;
		while (it) {
			pcre2_code_free(it->re);
			temp = it;
			it = it->next;
			free(temp);
		}
	}
}

static inline void zend_accel_blocklist_allocate(zend_blocklist *blocklist)
{
	if (blocklist->pos == blocklist->size) {
		blocklist->size += ZEND_BLOCKLIST_BLOCK_SIZE;
		blocklist->entries = (zend_blocklist_entry *) realloc(blocklist->entries, sizeof(zend_blocklist_entry)*blocklist->size);
	}
}

static void zend_accel_blocklist_loadone(zend_blocklist *blocklist, char *filename)
{
	char buf[MAXPATHLEN + 1], real_path[MAXPATHLEN + 1], *blocklist_path = NULL;
	FILE *fp;
	int path_length, blocklist_path_length;

	if ((fp = fopen(filename, "r")) == NULL) {
		zend_accel_error(ACCEL_LOG_WARNING, "Cannot load blocklist file: %s\n", filename);
		return;
	}

	zend_accel_error(ACCEL_LOG_DEBUG,"Loading blocklist file:  '%s'", filename);

	if (VCWD_REALPATH(filename, buf)) {
		blocklist_path_length = zend_dirname(buf, strlen(buf));
		blocklist_path = zend_strndup(buf, blocklist_path_length);
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
		if (blocklist_path) {
			expand_filepath_ex(path_dup, real_path, blocklist_path, blocklist_path_length);
		} else {
			expand_filepath(path_dup, real_path);
		}
		path_length = strlen(real_path);

		free(path_dup);

		zend_accel_blocklist_allocate(blocklist);
		blocklist->entries[blocklist->pos].path_length = path_length;
		blocklist->entries[blocklist->pos].path = (char *)malloc(path_length + 1);
		if (!blocklist->entries[blocklist->pos].path) {
			zend_accel_error(ACCEL_LOG_ERROR, "malloc() failed\n");
			fclose(fp);
			return;
		}
		blocklist->entries[blocklist->pos].id = blocklist->pos;
		memcpy(blocklist->entries[blocklist->pos].path, real_path, path_length + 1);
		blocklist->pos++;
	}
	fclose(fp);
	if (blocklist_path) {
		free(blocklist_path);
	}
}

void zend_accel_blocklist_load(zend_blocklist *blocklist, char *filename)
{
#ifdef HAVE_GLOB
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
		zend_accel_error(ACCEL_LOG_WARNING, "No blocklist file found matching: %s\n", filename);
	} else {
		for(i=0 ; i<globbuf.gl_pathc; i++) {
			zend_accel_blocklist_loadone(blocklist, globbuf.gl_pathv[i]);
		}
		globfree(&globbuf);
	}
#else
	zend_accel_blocklist_loadone(blocklist, filename);
#endif
	zend_accel_blocklist_update_regexp(blocklist);
}

zend_bool zend_accel_blocklist_is_blocklisted(zend_blocklist *blocklist, char *verify_path, size_t verify_path_len)
{
	int ret = 0;
	zend_regexp_list *regexp_list_it = blocklist->regexp_list;
	pcre2_match_context *mctx = php_pcre_mctx();

	if (regexp_list_it == NULL) {
		return 0;
	}
	while (regexp_list_it != NULL) {
		pcre2_match_data *match_data = php_pcre_create_match_data(0, regexp_list_it->re);
		if (!match_data) {
			/* Alloc failed, but next one could still come through and match. */
			continue;
		}
		int rc = pcre2_match(regexp_list_it->re, (PCRE2_SPTR)verify_path, verify_path_len, 0, 0, match_data, mctx);
		if (rc >= 0) {
			ret = 1;
			php_pcre_free_match_data(match_data);
			break;
		}
		php_pcre_free_match_data(match_data);
		regexp_list_it = regexp_list_it->next;
	}
	return ret;
}

void zend_accel_blocklist_apply(zend_blocklist *blocklist, blocklist_apply_func_arg_t func, void *argument)
{
	int i;

	for (i = 0; i < blocklist->pos; i++) {
		func(&blocklist->entries[i], argument);
	}
}
