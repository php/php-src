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
  | Author: Hartmut Holzgraefe  <hholzgra@php.net>                       |
  +----------------------------------------------------------------------+

  $Id$ 

  This module contains a lot of stuff taken from Apache mod_mime_magic,
  so the license section is a little bit longer than usual:

  ====================================================================
 * The Apache Software License, Version 1.1
 *
 * Copyright (c) 2000 The Apache Software Foundation.  All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "Apache" and "Apache Software Foundation" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache",
 *    nor may "Apache" appear in their name, without prior written
 *    permission of the Apache Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation.  For more
 * information on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 *
 * Portions of this software are based upon public domain software
 * originally written at the National Center for Supercomputing Applications,
 * University of Illinois, Urbana-Champaign.
 */

/*
 * mod_mime_magic: MIME type lookup via file magic numbers
 * Copyright (c) 1996-1997 Cisco Systems, Inc.
 *
 * This software was submitted by Cisco Systems to the Apache Group in July
 * 1997.  Future revisions and derivatives of this source code must
 * acknowledge Cisco Systems as the original contributor of this module.
 * All other licensing and usage conditions are those of the Apache Group.
 *
 * Some of this code is derived from the free version of the file command
 * originally posted to comp.sources.unix.  Copyright info for that program
 * is included below as required.
 * ---------------------------------------------------------------------------
 * - Copyright (c) Ian F. Darwin, 1987. Written by Ian F. Darwin.
 *
 * This software is not subject to any license of the American Telephone and
 * Telegraph Company or of the Regents of the University of California.
 *
 * Permission is granted to anyone to use this software for any purpose on any
 * computer system, and to alter it and redistribute it freely, subject to
 * the following restrictions:
 *
 * 1. The author is not responsible for the consequences of use of this
 * software, no matter how awful, even if they arise from flaws in it.
 *
 * 2. The origin of this software must not be misrepresented, either by
 * explicit claim or by omission.  Since few users ever read sources, credits
 * must appear in the documentation.
 *
 * 3. Altered versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.  Since few users ever read
 * sources, credits must appear in the documentation.
 *
 * 4. This notice may not be removed or altered.
 * -------------------------------------------------------------------------
 *
 * For compliance with Mr Darwin's terms: this has been very significantly
 * modified from the free "file" command.
 * - all-in-one file for compilation convenience when moving from one
 *   version of Apache to the next.
 * - Memory allocation is done through the Apache API's pool structure.
 * - All functions have had necessary Apache API request or server
 *   structures passed to them where necessary to call other Apache API
 *   routines.  (i.e. usually for logging, files, or memory allocation in
 *   itself or a called function.)
 * - struct magic has been converted from an array to a single-ended linked
 *   list because it only grows one record at a time, it's only accessed
 *   sequentially, and the Apache API has no equivalent of realloc().
 * - Functions have been changed to get their parameters from the server
 *   configuration instead of globals.  (It should be reentrant now but has
 *   not been tested in a threaded environment.)
 * - Places where it used to print results to stdout now saves them in a
 *   list where they're used to set the MIME type in the Apache request
 *   record.
 * - Command-line flags have been removed since they will never be used here.
 *
 * Ian Kluft <ikluft@cisco.com>
 * Engineering Information Framework
 * Central Engineering
 * Cisco Systems, Inc.
 * San Jose, CA, USA
 *
 * Initial installation          July/August 1996
 * Misc bug fixes                May 1997
 * Submission to Apache Group    July 1997
 *
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
 
#include "php.h"
#include "php_streams.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_mime_magic.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef PHP_WIN32
#define PHP_MIME_MAGIC_FILE_PATH PHP_PREFIX "\\magic.mime"
#endif

#define MODNAME "mime_magic"

static int apprentice(void);
static int ascmagic(unsigned char *, int);
static int is_tar(unsigned char *, int);
static int softmagic(unsigned char *, int);
static void tryit(unsigned char *, int, int);

static int getvalue(struct magic *, char **);
static int hextoint(int);
static char *getstr(char *, char *, int, int *);
static int parse(char *, int);

static int match(unsigned char *, int);
static int mget(union VALUETYPE *, unsigned char *,
		struct magic *, int);
static int mcheck(union VALUETYPE *, struct magic *);
static void mprint(union VALUETYPE *, struct magic *);
static int mconvert(union VALUETYPE *, struct magic *);
static int magic_rsl_get(char **, char **);
static int magic_process(char * TSRMLS_DC);

static long from_oct(int, char *);
static int fsmagic(char *fn TSRMLS_DC);


#if HAVE_ZLIB
static int zmagic(unsigned char *, int);
#endif

static magic_req_rec *magic_set_config(void);
static void magic_free_config(magic_req_rec *);

ZEND_DECLARE_MODULE_GLOBALS(mime_magic)


/* True global resources - no need for thread safety here */
static magic_server_config_rec mime_global;

/* {{{ mime_magic_functions[] */
function_entry mime_magic_functions[] = {
	PHP_FE(mime_content_type,	NULL)	   
	{NULL, NULL, NULL}	
};
/* }}} */

/* {{{ mime_magic_module_entry
 */
zend_module_entry mime_magic_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"mime_magic",
	mime_magic_functions,
	PHP_MINIT(mime_magic),
	PHP_MSHUTDOWN(mime_magic),
	NULL,
	NULL,
	PHP_MINFO(mime_magic),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", 
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_MIME_MAGIC
ZEND_GET_MODULE(mime_magic)
#endif

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
STD_PHP_INI_ENTRY("mime_magic.magicfile", PHP_MIME_MAGIC_FILE_PATH, PHP_INI_SYSTEM, OnUpdateString, magicfile, zend_mime_magic_globals, mime_magic_globals)
PHP_INI_END()
/* }}} */

/* {{{ php_mime_magic_init_globals
 */
static void php_mime_magic_init_globals(zend_mime_magic_globals *mime_magic_globals)
{
	mime_global.magicfile = NULL;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(mime_magic)
{
	ZEND_INIT_MODULE_GLOBALS(mime_magic, php_mime_magic_init_globals, NULL);
	REGISTER_INI_ENTRIES();

	mime_global.magicfile = MIME_MAGIC_G(magicfile);

	if(mime_global.magicfile) {
		apprentice();
	}
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(mime_magic)
{
	UNREGISTER_INI_ENTRIES();
	if (mime_global.magic != NULL && mime_global.magic != (struct magic *) -1) {
		struct magic *iter = mime_global.magic;
		while (iter != NULL) {
			struct magic *iter_next = iter->next;	
			free(iter);
			iter = iter_next;
		}
	}
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(mime_magic)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "mime_magic support", "enabled");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */


/* {{{ proto string mime_content_type(string filename)
   Return content-type for file */
PHP_FUNCTION(mime_content_type)
{
	char *filename = NULL;
	int filename_len;
	magic_server_config_rec *conf = &mime_global;
	char *content_type=NULL, *content_encoding=NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &filename_len) == FAILURE) {
		return;
	}

	if (conf->magic == (struct magic *)-1) {
		php_error(E_ERROR, MODNAME " could not be initialized, magic file %s is not avaliable",  conf->magicfile);
		RETURN_FALSE;
	} 

	if(!conf->magic) {
		php_error(E_WARNING, MODNAME " not initialized");
		RETURN_FALSE;
	}

	MIME_MAGIC_G(req_dat) = magic_set_config();

	if(MIME_MAGIC_OK != magic_process(filename TSRMLS_CC)) {
		RETVAL_FALSE;
	} else if(MIME_MAGIC_OK != magic_rsl_get(&content_type, &content_encoding)) {
		RETVAL_FALSE;
	} else {
		RETVAL_STRING(content_type, 1);
	}

	if(content_type) efree(content_type);
	if(content_encoding) efree(content_encoding);	

	magic_free_config(MIME_MAGIC_G(req_dat));
}
/* }}} */

#define    EATAB {while (isspace((unsigned char) *l))  ++l;}

/*
 * apprentice - load configuration from the magic file r
 *  API request record
 */
static int apprentice(void)
{
    FILE *f;
    char line[BUFSIZ + 1];
    int errs = 0;
    int lineno;

    char *fname;
    magic_server_config_rec *conf = &mime_global;

    fname = conf->magicfile; /* todo cwd? */
    f = fopen(fname, "rb");
    if (f == NULL) {
		conf->magic = (struct magic *)-1;
		return -1;
    }

    /* set up the magic list (empty) */
    conf->magic = conf->last = NULL;

    /* parse it */
    for (lineno = 1; fgets(line, BUFSIZ, f) != NULL; lineno++) {
		int ws_offset;

		/* delete newline */
		if (line[0]) {
			line[strlen(line) - 1] = '\0';
		}

		/* skip leading whitespace */
		ws_offset = 0;
		while (line[ws_offset] && isspace(line[ws_offset])) {
			ws_offset++;
		}

		/* skip blank lines */
		if (line[ws_offset] == 0) {
			continue;
		}

		/* comment, do not parse */
		if (line[ws_offset] == '#')
			continue;

		/* parse it */
		if (parse(line + ws_offset, lineno) != 0)
			++errs;
    }

    (void) fclose(f);

    return (errs ? -1 : 0);
}

/*
 * extend the sign bit if the comparison is to be signed
 */
static unsigned long signextend(struct magic *m, unsigned long v)
{
    if (!(m->flag & UNSIGNED))
		switch (m->type) {
			/*
			 * Do not remove the casts below.  They are vital. When later
			 * compared with the data, the sign extension must have happened.
			 */
		case BYTE:
			v = (char) v;
			break;
		case SHORT:
		case BESHORT:
		case LESHORT:
			v = (short) v;
			break;
		case DATE:
		case BEDATE:
		case LEDATE:
		case LONG:
		case BELONG:
		case LELONG:
			v = (long) v;
			break;
		case STRING:
			break;
		default:
			php_error(E_WARNING,
						 MODNAME ": can't happen: m->type=%d", m->type);
			return -1;
		}
    return v;
}

/*
 * parse one line from magic file, put into magic[index++] if valid
 */
static int parse(char *l, int lineno)
{
    struct magic *m;
    char *t, *s;
    magic_server_config_rec *conf = &mime_global;

    /* allocate magic structure entry */
    m = (struct magic *) calloc(1, sizeof(struct magic));

    /* append to linked list */
    m->next = NULL;
    if (!conf->magic || !conf->last) {
		conf->magic = conf->last = m;
    }
    else {
		conf->last->next = m;
		conf->last = m;
    }

    /* set values in magic structure */
    m->flag = 0;
    m->cont_level = 0;
    m->lineno = lineno;

    while (*l == '>') {
		++l;			/* step over */
		m->cont_level++;
    }

    if (m->cont_level != 0 && *l == '(') {
		++l;			/* step over */
		m->flag |= INDIR;
    }

    /* get offset, then skip over it */
    m->offset = (int) strtol(l, &t, 0);
    if (l == t) {
		php_error(E_WARNING,
					 MODNAME ": (line %d) offset `%s' invalid", lineno, l);
    }
    l = t;

    if (m->flag & INDIR) {
		m->in.type = LONG;
		m->in.offset = 0;
		/*
		 * read [.lbs][+-]nnnnn)
		 */
		if (*l == '.') {
			switch (*++l) {
			case 'l':
				m->in.type = LONG;
				break;
			case 's':
				m->in.type = SHORT;
				break;
			case 'b':
				m->in.type = BYTE;
				break;
			default:
				php_error(E_WARNING,
							 MODNAME ": indirect offset type %c invalid", *l);
				break;
			}
			l++;
		}
		s = l;
		if (*l == '+' || *l == '-')
			l++;
		if (isdigit((unsigned char) *l)) {
			m->in.offset = strtol(l, &t, 0);
			if (*s == '-')
				m->in.offset = -m->in.offset;
		}
		else
			t = l;
		if (*t++ != ')') {
			php_error(E_WARNING,
						 MODNAME ": missing ')' in indirect offset");
		}
		l = t;
    }


    while (isdigit((unsigned char) *l))
		++l;
    EATAB;

#define NBYTE           4
#define NSHORT          5
#define NLONG           4
#define NSTRING         6
#define NDATE           4
#define NBESHORT        7
#define NBELONG         6
#define NBEDATE         6
#define NLESHORT        7
#define NLELONG         6
#define NLEDATE         6

    if (*l == 'u') {
		++l;
		m->flag |= UNSIGNED;
    }

    /* get type, skip it */
    if (strncmp(l, "byte", NBYTE) == 0) {
		m->type = BYTE;
		l += NBYTE;
    }
    else if (strncmp(l, "short", NSHORT) == 0) {
		m->type = SHORT;
		l += NSHORT;
    }
    else if (strncmp(l, "long", NLONG) == 0) {
		m->type = LONG;
		l += NLONG;
    }
    else if (strncmp(l, "string", NSTRING) == 0) {
		m->type = STRING;
		l += NSTRING;
    }
    else if (strncmp(l, "date", NDATE) == 0) {
		m->type = DATE;
		l += NDATE;
    }
    else if (strncmp(l, "beshort", NBESHORT) == 0) {
		m->type = BESHORT;
		l += NBESHORT;
    }
    else if (strncmp(l, "belong", NBELONG) == 0) {
		m->type = BELONG;
		l += NBELONG;
    }
    else if (strncmp(l, "bedate", NBEDATE) == 0) {
		m->type = BEDATE;
		l += NBEDATE;
    }
    else if (strncmp(l, "leshort", NLESHORT) == 0) {
		m->type = LESHORT;
		l += NLESHORT;
    }
    else if (strncmp(l, "lelong", NLELONG) == 0) {
		m->type = LELONG;
		l += NLELONG;
    }
    else if (strncmp(l, "ledate", NLEDATE) == 0) {
		m->type = LEDATE;
		l += NLEDATE;
    }
    else {
		php_error(E_WARNING,
					 MODNAME ": type %s invalid", l);
		return -1;
    }
    /* New-style anding: "0 byte&0x80 =0x80 dynamically linked" */
    if (*l == '&') {
		++l;
		m->mask = signextend(m, strtol(l, &l, 0));
    }
    else
		m->mask = ~0L;
    EATAB;

    switch (*l) {
    case '>':
    case '<':
		/* Old-style anding: "0 byte &0x80 dynamically linked" */
    case '&':
    case '^':
    case '=':
		m->reln = *l;
		++l;
		break;
    case '!':
		if (m->type != STRING) {
			m->reln = *l;
			++l;
			break;
		}
		/* FALL THROUGH */
    default:
		if (*l == 'x' && isspace((unsigned char) l[1])) {
			m->reln = *l;
			++l;
			goto GetDesc;	/* Bill The Cat */
		}
		m->reln = '=';
		break;
    }
    EATAB;

    if (getvalue(m, &l))
		return -1;
    /*
     * now get last part - the description
     */
 GetDesc:
    EATAB;
    if (l[0] == '\b') {
		++l;
		m->nospflag = 1;
    }
    else if ((l[0] == '\\') && (l[1] == 'b')) {
		++l;
		++l;
		m->nospflag = 1;
    }
    else
		m->nospflag = 0;
    strncpy(m->desc, l, sizeof(m->desc) - 1);
    m->desc[sizeof(m->desc) - 1] = '\0';

    return 0;
}

/*
 * Read a numeric value from a pointer, into the value union of a magic
 * pointer, according to the magic type.  Update the string pointer to point
 * just after the number read.  Return 0 for success, non-zero for failure.
 */
static int getvalue(struct magic *m, char **p)
{
    int slen;

    if (m->type == STRING) {
		*p = getstr(*p, m->value.s, sizeof(m->value.s), &slen);
		m->vallen = slen;
    }
    else if (m->reln != 'x')
		m->value.l = signextend(m, strtol(*p, p, 0));
    return 0;
}

/*
 * Convert a string containing C character escapes.  Stop at an unescaped
 * space or tab. Copy the converted version to "p", returning its length in
 * *slen. Return updated scan pointer as function result.
 */
static char *getstr(register char *s, register char *p,
					int plen, int *slen)
{
    char *origs = s, *origp = p;
    char *pmax = p + plen - 1;
    register int c;
    register int val;

    while ((c = *s++) != '\0') {
		if (isspace((unsigned char) c))
			break;
		if (p >= pmax) {
			php_error(E_WARNING,
						 MODNAME ": string too long: %s", origs);
			break;
		}
		if (c == '\\') {
			switch (c = *s++) {

			case '\0':
				goto out;

			default:
				*p++ = (char) c;
				break;

			case 'n':
				*p++ = '\n';
				break;

			case 'r':
				*p++ = '\r';
				break;

			case 'b':
				*p++ = '\b';
				break;

			case 't':
				*p++ = '\t';
				break;

			case 'f':
				*p++ = '\f';
				break;

			case 'v':
				*p++ = '\v';
				break;

				/* \ and up to 3 octal digits */
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
				val = c - '0';
				c = *s++;	/* try for 2 */
				if (c >= '0' && c <= '7') {
					val = (val << 3) | (c - '0');
					c = *s++;	/* try for 3 */
					if (c >= '0' && c <= '7')
						val = (val << 3) | (c - '0');
					else
						--s;
				}
				else
					--s;
				*p++ = (char) val;
				break;

				/* \x and up to 3 hex digits */
			case 'x':
				val = 'x';	/* Default if no digits */
				c = hextoint(*s++);	/* Get next char */
				if (c >= 0) {
					val = c;
					c = hextoint(*s++);
					if (c >= 0) {
						val = (val << 4) + c;
						c = hextoint(*s++);
						if (c >= 0) {
							val = (val << 4) + c;
						}
						else
							--s;
					}
					else
						--s;
				}
				else
					--s;
				*p++ = (char) val;
				break;
			}
		}
		else
			*p++ = (char) c;
    }
 out:
    *p = '\0';
    *slen = p - origp;
    return s;
}

/* Single hex char to int; -1 if not a hex char. */
static int hextoint(int c)
{
    if (isdigit((unsigned char) c))
		return c - '0';
    if ((c >= 'a') && (c <= 'f'))
		return c + 10 - 'a';
    if ((c >= 'A') && (c <= 'F'))
		return c + 10 - 'A';
    return -1;
}


/*
 * RSL (result string list) processing routines
 *
 * These collect strings that would have been printed in fragments by file(1)
 * into a list of magic_rsl structures with the strings. When complete,
 * they're concatenated together to become the MIME content and encoding
 * types.
 *
 * return value conventions for these functions: functions which return int:
 * failure = -1, other = result functions which return pointers: failure = 0,
 * other = result
 */

/* allocate a per-request structure and put it in the request record */
static magic_req_rec *magic_set_config(void)
{
    magic_req_rec *req_dat = (magic_req_rec *) emalloc(sizeof(magic_req_rec));

    req_dat->head = req_dat->tail = (magic_rsl *) NULL;
    return req_dat;
}

static void magic_free_config(magic_req_rec *req_dat) {
	magic_rsl *curr, *next;

	if(!req_dat) return;

	curr = req_dat->head;
	while(curr) {
		next = curr->next;
		efree(curr->str);
		efree(curr);
		curr = next;
	}

	efree(req_dat);
}

/* add a string to the result string list for this request */
/* it is the responsibility of the caller to allocate "str" */
static int magic_rsl_add(char *str)
{
    magic_req_rec *req_dat;  
    magic_rsl *rsl;
	TSRMLS_FETCH();

	req_dat = MIME_MAGIC_G(req_dat);
	
   /* make sure we have a list to put it in */
    if (!req_dat) {
		php_error(E_WARNING,
					  MODNAME ": request config should not be NULL");
		if (!(req_dat = magic_set_config())) {
			/* failure */
			return -1;
		}
    }

    /* allocate the list entry */
    rsl = (magic_rsl *) emalloc(sizeof(magic_rsl));

    /* fill it */
    rsl->str = estrdup(str);
    rsl->next = (magic_rsl *) NULL;

    /* append to the list */
    if (req_dat->head && req_dat->tail) {
		req_dat->tail->next = rsl;
		req_dat->tail = rsl;
    }
    else {
		req_dat->head = req_dat->tail = rsl;
    }

    /* success */
    return 0;
}

/* RSL hook for puts-type functions */
static int magic_rsl_puts(char *str)
{
    return magic_rsl_add(str);
}

/* RSL hook for printf-type functions */
static int magic_rsl_printf(char *str,...)
{
    va_list ap;

    char buf[MAXMIMESTRING];

    /* assemble the string into the buffer */
    va_start(ap, str);
    vsnprintf(buf, sizeof(buf), str, ap);
    va_end(ap);

    /* add the buffer to the list */
    return magic_rsl_add(buf);
}

/* RSL hook for putchar-type functions */
static int magic_rsl_putchar(char c)
{
    char str[2];

    /* high overhead for 1 char - just hope they don't do this much */
    str[0] = c;
    str[1] = '\0';
    return magic_rsl_add(str);
}

/* allocate and copy a contiguous string from a result string list */
static char *rsl_strdup(int start_frag, int start_pos, int len)
{
    char *result;		/* return value */
    int cur_frag,		/* current fragment number/counter */
        cur_pos,		/* current position within fragment */
        res_pos;		/* position in result string */
    magic_rsl *frag;		/* list-traversal pointer */
    magic_req_rec *req_dat;
	TSRMLS_FETCH();

    req_dat =  MIME_MAGIC_G(req_dat);

    /* allocate the result string */
    result = (char *) emalloc(len + 1);

    /* loop through and collect the string */
    res_pos = 0;
    for (frag = req_dat->head, cur_frag = 0;
		 frag->next;
		 frag = frag->next, cur_frag++) {
		/* loop to the first fragment */
		if (cur_frag < start_frag)
			continue;

		/* loop through and collect chars */
		for (cur_pos = (cur_frag == start_frag) ? start_pos : 0;
			 frag->str[cur_pos];
			 cur_pos++) {
			if (cur_frag >= start_frag
				&& cur_pos >= start_pos
				&& res_pos <= len) {
				result[res_pos++] = frag->str[cur_pos];
				if (res_pos > len) {
					break;
				}
			}
		}
    }

    /* clean up and return */
    result[res_pos] = 0;

    return result;
}

/*
 * magic_process - process input file r        Apache API request record
 * (formerly called "process" in file command, prefix added for clarity) Opens
 * the file and reads a fixed-size buffer to begin processing the contents.
 */
static int magic_process(char *filename TSRMLS_DC)
{
	php_stream *stream;
    unsigned char buf[HOWMANY + 1];	/* one extra for terminating '\0' */
    int nbytes = 0;		/* number of bytes read from a datafile */
    int result;

    /*
     * first try judging the file based on its filesystem status
     */
    switch ((result = fsmagic(filename TSRMLS_CC))) {
    case MIME_MAGIC_DONE:
		magic_rsl_putchar('\n');
		return MIME_MAGIC_OK;
    case MIME_MAGIC_OK:
		break;
    default:
		/* fatal error, bail out */
		return result;
    }

    stream = php_stream_open_wrapper(filename, "rb", IGNORE_PATH | ENFORCE_SAFE_MODE | REPORT_ERRORS, NULL);

    if (stream == NULL) {
		/* We can't open it, but we were able to stat it. */
		php_error(E_WARNING,
					  MODNAME ": can't read `%s'", filename);
		/* let some other handler decide what the problem is */
		return MIME_MAGIC_DECLINED;
    }

    /*
     * try looking at the first HOWMANY bytes
     */
    if ((nbytes = php_stream_read(stream, (char *) buf, sizeof(buf) - 1)) == -1) {
		php_error(E_WARNING,
					  MODNAME ": read failed: %s", filename);
		return MIME_MAGIC_ERROR;
    }

    if (nbytes == 0)
		magic_rsl_puts(MIME_TEXT_UNKNOWN);
    else {
		buf[nbytes++] = '\0';	/* null-terminate it */
		tryit(buf, nbytes, 1); 
    }

    (void) php_stream_close(stream);
    (void) magic_rsl_putchar('\n');

    return MIME_MAGIC_OK;
}

static void tryit(unsigned char *buf, int nb, int checkzmagic)
{
    /*
     * Try compression stuff
     */
#if HAVE_ZLIB
	if (checkzmagic == 1) {  
		if (zmagic(buf, nb) == 1)
			return;
	}
#endif

    /*
     * try tests in /etc/magic (or surrogate magic file)
     */
    if (softmagic(buf, nb) == 1)
		return;

    /*
     * try known keywords, check for ascii-ness too.
     */
    if (ascmagic(buf, nb) == 1)
		return;

    /*
     * abandon hope, all ye who remain here
     */
    magic_rsl_puts(MIME_BINARY_UNKNOWN);
}


/*
 * return MIME_MAGIC_DONE to indicate it's been handled
 * return MIME_MAGIC_OK to indicate it's a regular file still needing handling
 * other returns indicate a failure of some sort
 */
static int fsmagic(char *filename TSRMLS_DC)
{
	php_stream_statbuf stat_ssb;

	if(!php_stream_stat_path(filename, &stat_ssb)) {
		return MIME_MAGIC_OK;
	}

    switch (stat_ssb.sb.st_mode & S_IFMT) {
    case S_IFDIR:
		magic_rsl_puts(DIR_MAGIC_TYPE);
		return MIME_MAGIC_DONE;
    case S_IFCHR:
		/*
		 * (void) magic_rsl_printf(r,"character special (%d/%d)",
		 * major(sb->st_rdev), minor(sb->st_rdev));
		 */
		(void) magic_rsl_puts(MIME_BINARY_UNKNOWN);
		return MIME_MAGIC_DONE;
#ifdef S_IFBLK
    case S_IFBLK:
		/*
		 * (void) magic_rsl_printf(r,"block special (%d/%d)",
		 * major(sb->st_rdev), minor(sb->st_rdev));
		 */
		(void) magic_rsl_puts(MIME_BINARY_UNKNOWN);
		return MIME_MAGIC_DONE;
		/* TODO add code to handle V7 MUX and Blit MUX files */
#endif
#ifdef    S_IFIFO
    case S_IFIFO:
		/*
		 * magic_rsl_puts(r,"fifo (named pipe)");
		 */
		(void) magic_rsl_puts(MIME_BINARY_UNKNOWN);
		return MIME_MAGIC_DONE;
#endif
#ifdef    S_IFLNK
    case S_IFLNK:
		/* We used stat(), the only possible reason for this is that the
		 * symlink is broken.
		 */
		php_error(E_WARNING,
					  MODNAME ": broken symlink (%s)", filename);
		return MIME_MAGIC_ERROR;
#endif
#ifdef    S_IFSOCK
#ifndef __COHERENT__
    case S_IFSOCK:
		magic_rsl_puts(MIME_BINARY_UNKNOWN);
		return MIME_MAGIC_DONE;
#endif
#endif
    case S_IFREG:
		break;
	case 0:
		break;
    default:
		php_error(E_WARNING,
					  MODNAME ": invalid mode 0%o.", (unsigned int)stat_ssb.sb.st_mode);
		return MIME_MAGIC_ERROR;
    }

    /*
     * regular file, check next possibility
     */
    if (stat_ssb.sb.st_size == 0) {
		magic_rsl_puts(MIME_TEXT_UNKNOWN);
		return MIME_MAGIC_DONE;
    }
    return MIME_MAGIC_OK;
}

/*
 * softmagic - lookup one file in database (already read from /etc/magic by
 * apprentice.c). Passed the name and FILE * of one file to be typed.
 */
/* ARGSUSED1 *//* nbytes passed for regularity, maybe need later */
static int softmagic(unsigned char *buf, int nbytes)
{
    if (match(buf, nbytes))
		return 1;

    return 0;
}

/*
 * Go through the whole list, stopping if you find a match.  Process all the
 * continuations of that match before returning.
 *
 * We support multi-level continuations:
 *
 * At any time when processing a successful top-level match, there is a current
 * continuation level; it represents the level of the last successfully
 * matched continuation.
 *
 * Continuations above that level are skipped as, if we see one, it means that
 * the continuation that controls them - i.e, the lower-level continuation
 * preceding them - failed to match.
 *
 * Continuations below that level are processed as, if we see one, it means
 * we've finished processing or skipping higher-level continuations under the
 * control of a successful or unsuccessful lower-level continuation, and are
 * now seeing the next lower-level continuation and should process it.  The
 * current continuation level reverts to the level of the one we're seeing.
 *
 * Continuations at the current level are processed as, if we see one, there's
 * no lower-level continuation that may have failed.
 *
 * If a continuation matches, we bump the current continuation level so that
 * higher-level continuations are processed.
 */
static int match(unsigned char *s, int nbytes)
{
    int cont_level = 0;
    int need_separator = 0;
    union VALUETYPE p;
    magic_server_config_rec *conf = &mime_global;
    struct magic *m;

    for (m = conf->magic; m; m = m->next) {
		/* check if main entry matches */
		if (!mget(&p, s, m, nbytes) ||
			!mcheck(&p, m)) {
			struct magic *m_cont;

			/*
			 * main entry didn't match, flush its continuations
			 */
			if (!m->next || (m->next->cont_level == 0)) {
				continue;
			}

			m_cont = m->next;
			while (m_cont && (m_cont->cont_level != 0)) {
				/*
				 * this trick allows us to keep *m in sync when the continue
				 * advances the pointer
				 */
				m = m_cont;
				m_cont = m_cont->next;
			}
			continue;
		}

		/* if we get here, the main entry rule was a match */
		/* this will be the last run through the loop */

		/* print the match */
		mprint(&p, m);

		/*
		 * If we printed something, we'll need to print a blank before we
		 * print something else.
		 */
		if (m->desc[0])
			need_separator = 1;
		/* and any continuations that match */
		cont_level++;
		/*
		 * while (m && m->next && m->next->cont_level != 0 && ( m = m->next
		 * ))
		 */
		m = m->next;
		while (m && (m->cont_level != 0)) {
			if (cont_level >= m->cont_level) {
				if (cont_level > m->cont_level) {
					/*
					 * We're at the end of the level "cont_level"
					 * continuations.
					 */
					cont_level = m->cont_level;
				}
				if (mget(&p, s, m, nbytes) &&
					mcheck(&p, m)) {
					/*
					 * This continuation matched. Print its message, with a
					 * blank before it if the previous item printed and this
					 * item isn't empty.
					 */
					/* space if previous printed */
					if (need_separator
						&& (m->nospflag == 0)
						&& (m->desc[0] != '\0')
						) {
						(void) magic_rsl_putchar(' ');
						need_separator = 0;
					}
					mprint(&p, m);
					if (m->desc[0])
						need_separator = 1;

					/*
					 * If we see any continuations at a higher level, process
					 * them.
					 */
					cont_level++;
				}
			}

			/* move to next continuation record */
			m = m->next;
		}
		return 1;		/* all through */
    }
    return 0;			/* no match at all */
}

/* an optimization over plain strcmp() */
#define    STREQ(a, b)    (*(a) == *(b) && strcmp((a), (b)) == 0)

static int ascmagic(unsigned char *buf, int nbytes)
{
    int has_escapes = 0;
    unsigned char *s;
    char nbuf[HOWMANY + 1];	/* one extra for terminating '\0' */
    char *token;
    register struct names *p;
    int small_nbytes;

    /* these are easy, do them first */

    /*
     * for troff, look for . + letter + letter or .\"; this must be done to
     * disambiguate tar archives' ./file and other trash from real troff
     * input.
     */
    if (*buf == '.') {
		unsigned char *tp = buf + 1;

		while (isspace(*tp))
			++tp;		/* skip leading whitespace */
		if ((isalnum(*tp) || *tp == '\\') &&
			(isalnum(*(tp + 1)) || *tp == '"')) {
			magic_rsl_puts("application/x-troff");
			return 1;
		}
    }
    if ((*buf == 'c' || *buf == 'C') && isspace(*(buf + 1))) {
		/* Fortran */
		magic_rsl_puts("text/plain");
		return 1;
    }

    /* look for tokens from names.h - this is expensive!, so we'll limit
     * ourselves to only SMALL_HOWMANY bytes */
    small_nbytes = (nbytes > SMALL_HOWMANY) ? SMALL_HOWMANY : nbytes;
    /* make a copy of the buffer here because strtok() will destroy it */
    s = (unsigned char *) memcpy(nbuf, buf, small_nbytes);
    s[small_nbytes] = '\0';
    has_escapes = (memchr(s, '\033', small_nbytes) != NULL);
    /* XXX: not multithread safe */
    while ((token = strtok((char *) s, " \t\n\r\f")) != NULL) {
		s = NULL;		/* make strtok() keep on tokin' */
		for (p = names; p < names + NNAMES; p++) {
			if (STREQ(p->name, token)) {
				magic_rsl_puts(types[p->type]);
				if (has_escapes)
					magic_rsl_puts(" (with escape sequences)");
				return 1;
			}
		}
    }

    switch (is_tar(buf, nbytes)) {
    case 1:
		/* V7 tar archive */
		magic_rsl_puts("application/x-tar");
		return 1;
    case 2:
		/* POSIX tar archive */
		magic_rsl_puts("application/x-tar");
		return 1;
    }

    /* all else fails, but it is ascii... */
    if (has_escapes) {
		/* text with escape sequences */
		/* we leave this open for further differentiation later */
		magic_rsl_puts("text/plain");
    }
    else {
		/* plain text */
		magic_rsl_puts("text/plain");
    }
    return 1;
}

/*
 * is_tar() -- figure out whether file is a tar archive.
 *
 * Stolen (by author of file utility) from the public domain tar program: Public
 * Domain version written 26 Aug 1985 John Gilmore (ihnp4!hoptoad!gnu).
 *
 * @(#)list.c 1.18 9/23/86 Public Domain - gnu $Id: mod_mime_magic.c,v 1.7
 * 1997/06/24 00:41:02 ikluft Exp ikluft $
 *
 * Comments changed and some code/comments reformatted for file command by Ian
 * Darwin.
 */

#define    isodigit(c)    ( ((c) >= '0') && ((c) <= '7') )

/*
 * Return 0 if the checksum is bad (i.e., probably not a tar archive), 1 for
 * old UNIX tar file, 2 for Unix Std (POSIX) tar file.
 */

static int is_tar(unsigned char *buf, int nbytes)
{
    register union record *header = (union record *) buf;
    register int i;
    register long sum, recsum;
    register char *p;

    if (nbytes < sizeof(union record))
		return 0;

    recsum = from_oct(8, header->header.chksum);

    sum = 0;
    p = header->charptr;
    for (i = sizeof(union record); --i >= 0;) {
		/*
		 * We can't use unsigned char here because of old compilers, e.g. V7.
		 */
		sum += 0xFF & *p++;
    }

    /* Adjust checksum to count the "chksum" field as blanks. */
    for (i = sizeof(header->header.chksum); --i >= 0;)
		sum -= 0xFF & header->header.chksum[i];
    sum += ' ' * sizeof header->header.chksum;

    if (sum != recsum)
		return 0;		/* Not a tar archive */

    if (0 == strcmp(header->header.magic, TMAGIC))
		return 2;		/* Unix Standard tar archive */

    return 1;			/* Old fashioned tar archive */
}


/*
 * Quick and dirty octal conversion.
 *
 * Result is -1 if the field is invalid (all blank, or nonoctal).
 */
static long from_oct(int digs, char *where)
{
    register long value;

    while (isspace(*where)) {	/* Skip spaces */
		where++;
		if (--digs <= 0)
			return -1;		/* All blank field */
    }
    value = 0;
    while (digs > 0 && isodigit(*where)) {	/* Scan til nonoctal */
		value = (value << 3) | (*where++ - '0');
		--digs;
    }

    if (digs > 0 && *where && !isspace(*where))
		return -1;		/* Ended on non-space/nul */

    return value;
}

static int mget(union VALUETYPE *p, unsigned char *s,
				struct magic *m, int nbytes)
{
    long offset = m->offset;

    if (offset + sizeof(union VALUETYPE) > nbytes)
		return 0;

    memcpy(p, s + offset, sizeof(union VALUETYPE));

    if (!mconvert(p, m))
		return 0;

    if (m->flag & INDIR) {

		switch (m->in.type) {
		case BYTE:
			offset = p->b + m->in.offset;
			break;
		case SHORT:
			offset = p->h + m->in.offset;
			break;
		case LONG:
			offset = p->l + m->in.offset;
			break;
		}

		if (offset + sizeof(union VALUETYPE) > nbytes)
			return 0;

		memcpy(p, s + offset, sizeof(union VALUETYPE));

		if (!mconvert(p, m))
			return 0;
    }
    return 1;
}

static int mcheck(union VALUETYPE *p, struct magic *m)
{
    register unsigned long l = m->value.l;
    register unsigned long v;
    int matched;

    if ((m->value.s[0] == 'x') && (m->value.s[1] == '\0')) {
		php_error(E_WARNING,
					  MODNAME ": BOINK");
		return 1;
    }

    switch (m->type) {
    case BYTE:
		v = p->b;
		break;

    case SHORT:
    case BESHORT:
    case LESHORT:
		v = p->h;
		break;

    case LONG:
    case BELONG:
    case LELONG:
    case DATE:
    case BEDATE:
    case LEDATE:
		v = p->l;
		break;

    case STRING:
		l = 0;
		/*
		 * What we want here is: v = strncmp(m->value.s, p->s, m->vallen);
		 * but ignoring any nulls.  bcmp doesn't give -/+/0 and isn't
		 * universally available anyway.
		 */
		v = 0;
		{
			register unsigned char *a = (unsigned char *) m->value.s;
			register unsigned char *b = (unsigned char *) p->s;
			register int len = m->vallen;

			while (--len >= 0)
				if ((v = *b++ - *a++) != 0)
					break;
		}
		break;
    default:
		/*  bogosity, pretend that it just wasn't a match */
		php_error(E_WARNING,
					  MODNAME ": invalid type %d in mcheck().", m->type);
		return 0;
    }

    v = signextend(m, v) & m->mask;

    switch (m->reln) {
    case 'x':
		matched = 1;
		break;

    case '!':
		matched = v != l;
		break;

    case '=':
		matched = v == l;
		break;

    case '>':
		if (m->flag & UNSIGNED) {
			matched = v > l;
		}
		else {
			matched = (long) v > (long) l;
		}
		break;

    case '<':
		if (m->flag & UNSIGNED) {
			matched = v < l;
		}
		else {
			matched = (long) v < (long) l;
		}
		break;

    case '&':
		matched = (v & l) == l;
		break;

    case '^':
		matched = (v & l) != l;
		break;

    default:
		/* bogosity, pretend it didn't match */
		matched = 0;
		php_error(E_WARNING,
					  MODNAME ": mcheck: can't happen: invalid relation %d.",
					  m->reln);
		break;
    }

    return matched;
}

#if HAVE_ZLIB 
/*
 * compress routines: zmagic() - returns 0 if not recognized, uncompresses
 * and prints information if recognized uncompress(s, method, old, n, newch)
 * - uncompress old into new, using method, return sizeof new
 */

static int zmagic(unsigned char *buf, int nbytes)
{
	if (buf[0] != 0x1f) return 0;

	switch(buf[1]) {
	case 0x9d: /* .Z "x-compress" */
		break; /* not yet supportet */

	case 0x8b: /* .gz "x-gzip" */
#if 0
		if ((newsize = magic_uncompress(i, &newbuf, nbytes)) > 0) {
			tryit(newbuf, newsize, 0);
			
			/* set encoding type in the request record */
			/* TODO r->content_encoding = compr[i].encoding; */
		}
#endif
		break; /* not yet supported */
		
	case 0x1e: /* simply packed ? */
		break; /* not yet supported */
    }

	return 0;
}
#endif 

static void mprint(union VALUETYPE *p, struct magic *m)
{
    char *pp, *rt;
    unsigned long v;

    switch (m->type) {
    case BYTE:
		v = p->b;
		break;

    case SHORT:
    case BESHORT:
    case LESHORT:
		v = p->h;
		break;

    case LONG:
    case BELONG:
    case LELONG:
		v = p->l;
		break;

    case STRING:
		if (m->reln == '=') {
			(void) magic_rsl_printf(m->desc, m->value.s);
		}
		else {
			(void) magic_rsl_printf(m->desc, p->s);
		}
		return;

    case DATE:
    case BEDATE:
    case LEDATE:
		/* XXX: not multithread safe */
		pp = ctime((time_t *) & p->l);
		if ((rt = strchr(pp, '\n')) != NULL)
			*rt = '\0';
		(void) magic_rsl_printf(m->desc, pp);
		return;
    default:
		php_error(E_WARNING,
					  MODNAME ": invalid m->type (%d) in mprint().",
					  m->type);
		return;
    }

    v = signextend(m, v) & m->mask;
    (void) magic_rsl_printf(m->desc, (unsigned long) v);
}

/*
 * Convert the byte order of the data we are looking at
 */
static int mconvert(union VALUETYPE *p, struct magic *m)
{
    char *rt;

    switch (m->type) {
    case BYTE:
    case SHORT:
    case LONG:
    case DATE:
		return 1;
    case STRING:
		/* Null terminate and eat the return */
		p->s[sizeof(p->s) - 1] = '\0';
		if ((rt = strchr(p->s, '\n')) != NULL)
			*rt = '\0';
		return 1;
    case BESHORT:
		p->h = (short) ((p->hs[0] << 8) | (p->hs[1]));
		return 1;
    case BELONG:
    case BEDATE:
		p->l = (long)
			((p->hl[0] << 24) | (p->hl[1] << 16) | (p->hl[2] << 8) | (p->hl[3]));
		return 1;
    case LESHORT:
		p->h = (short) ((p->hs[1] << 8) | (p->hs[0]));
		return 1;
    case LELONG:
    case LEDATE:
		p->l = (long)
			((p->hl[3] << 24) | (p->hl[2] << 16) | (p->hl[1] << 8) | (p->hl[0]));
		return 1;
    default:
		php_error(E_WARNING,
					  MODNAME ": invalid type %d in mconvert().", m->type);
		return 0;
    }
}


/* states for the state-machine algorithm in magic_rsl_to_request() */
typedef enum {
    rsl_leading_space, rsl_type, rsl_subtype, rsl_separator, rsl_encoding
} rsl_states;

/* process the RSL and set the MIME info in the request record */
static int magic_rsl_get(char **content_type, char **content_encoding)
{
    int cur_frag,		/* current fragment number/counter */
        cur_pos,		/* current position within fragment */
        type_frag,		/* content type starting point: fragment */
        type_pos,		/* content type starting point: position */
        type_len,		/* content type length */
        encoding_frag,		/* content encoding starting point: fragment */
        encoding_pos,		/* content encoding starting point: position */
        encoding_len;		/* content encoding length */

    magic_rsl *frag;		/* list-traversal pointer */
    rsl_states state;

    magic_req_rec *req_dat;

	TSRMLS_FETCH();

    req_dat = MIME_MAGIC_G(req_dat);

    /* check if we have a result */
    if (!req_dat || !req_dat->head) {
		/* empty - no match, we defer to other Apache modules */
		return MIME_MAGIC_DECLINED;
    }

    /* start searching for the type and encoding */
    state = rsl_leading_space;
    type_frag = type_pos = type_len = 0;
    encoding_frag = encoding_pos = encoding_len = 0;
    for (frag = req_dat->head, cur_frag = 0;
		 frag && frag->next;
		 frag = frag->next, cur_frag++) {
		/* loop through the characters in the fragment */
		for (cur_pos = 0; frag->str[cur_pos]; cur_pos++) {
			if (isspace(frag->str[cur_pos])) {
				/* process whitespace actions for each state */
				if (state == rsl_leading_space) {
					/* eat whitespace in this state */
					continue;
				}
				else if (state == rsl_type) {
					/* whitespace: type has no slash! */
					return MIME_MAGIC_DECLINED;
				}
				else if (state == rsl_subtype) {
					/* whitespace: end of MIME type */
					state++;
					continue;
				}
				else if (state == rsl_separator) {
					/* eat whitespace in this state */
					continue;
				}
				else if (state == rsl_encoding) {
					/* whitespace: end of MIME encoding */
					/* we're done */
					frag = req_dat->tail;
					break;
				}
				else {
					/* should not be possible */
					/* abandon malfunctioning module */
					php_error(E_WARNING,
								  MODNAME ": bad state %d (ws)", state);
					return MIME_MAGIC_DECLINED;
				}
				/* NOTREACHED */
			}
			else if (state == rsl_type &&
					 frag->str[cur_pos] == '/') {
				/* copy the char and go to rsl_subtype state */
				type_len++;
				state++;
			}
			else {
				/* process non-space actions for each state */
				if (state == rsl_leading_space) {
					/* non-space: begin MIME type */
					state++;
					type_frag = cur_frag;
					type_pos = cur_pos;
					type_len = 1;
					continue;
				}
				else if (state == rsl_type ||
						 state == rsl_subtype) {
					/* non-space: adds to type */
					type_len++;
					continue;
				}
				else if (state == rsl_separator) {
					/* non-space: begin MIME encoding */
					state++;
					encoding_frag = cur_frag;
					encoding_pos = cur_pos;
					encoding_len = 1;
					continue;
				}
				else if (state == rsl_encoding) {
					/* non-space: adds to encoding */
					encoding_len++;
					continue;
				}
				else {
					/* should not be possible */
					/* abandon malfunctioning module */
					php_error(E_WARNING,
								  MODNAME ": bad state %d (ns)", state);
					return MIME_MAGIC_DECLINED;
				}
				/* NOTREACHED */
			}
			/* NOTREACHED */
		}
    }

    /* if we ended prior to state rsl_subtype, we had incomplete info */
    if (state != rsl_subtype && state != rsl_separator &&
		state != rsl_encoding) {
		/* defer to other modules */
		return MIME_MAGIC_DECLINED;
    }

    /* save the info in the request record */
    if (state == rsl_subtype || state == rsl_encoding ||
		state == rsl_encoding) {
        char *tmp;
		tmp = rsl_strdup(type_frag, type_pos, type_len);
		/* XXX: this could be done at config time I'm sure... but I'm
		 * confused by all this magic_rsl stuff. -djg */
		/* TODO ap_content_type_tolower(tmp); */
		*content_type = tmp;
    }
    if (state == rsl_encoding) {
        char *tmp;
		tmp = rsl_strdup(encoding_frag,
						 encoding_pos, encoding_len);
		/* XXX: this could be done at config time I'm sure... but I'm
		 * confused by all this magic_rsl stuff. -djg */
		/* TODO ap_str_tolower(tmp); */
		*content_encoding = tmp;
    }

    /* detect memory allocation errors */
    if (!content_type ||
		(state == rsl_encoding && !*content_encoding)) {
		return MIME_MAGIC_ERROR;
    }

    /* success! */
    return MIME_MAGIC_OK;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
