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
   | Author: Hartmut Holzgraefe <hartmut@six.de>                          |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include "php.h"

#include "php_globals.h"

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "basic_functions.h"
#include "url_scanner.h"

#ifndef BUFSIZE
#define BUFSIZE 256
#endif

int php_url_scanner_activate(TSRMLS_D)
{
	url_adapt(NULL,0,NULL,NULL);
	return SUCCESS;
}


int php_url_scanner_deactivate(TSRMLS_D)
{
	url_adapt(NULL,0,NULL,NULL);
	return SUCCESS;
}

/* {{{ url_attr_addon
 */
static char *url_attr_addon(const char *tag,const char *attr,const char *val,const char *buf)
{
	int flag = 0;
	TSRMLS_FETCH();

	if(!strcasecmp(tag,"a") && !strcasecmp(attr,"href")) {
		flag = 1;
	} else if(!strcasecmp(tag,"area" ) && !strcasecmp(attr,"href"   )) {
		flag = 1;
	} else if(!strcasecmp(tag,"form" ) && !strcasecmp(attr,"action" )) {
		flag = 1;
	} else if(!strcasecmp(tag,"frame") && !strcasecmp(attr,"source" )) {
		flag = 1;
	} else if(!strcasecmp(tag,"img"  ) && !strcasecmp(attr,"action" )) {
		flag = 1;
	}
	if(flag) {		
		if(!strstr(val,buf)&&!strchr(val,':'))
			{
				char *result = (char *)emalloc(strlen(buf)+strlen(PG(arg_separator).output)+1);
				int n;

				if(strchr(val,'?')) {
					strcpy(result,PG(arg_separator).output);
					n=strlen(PG(arg_separator).output);
				} else {
					*result='?';
					n=1;
				}
				strcpy(result+n,buf);
				return result;
			}
	} 
	return NULL;
}
/* }}} */

#define US BG(url_adapt_state)

/* {{{ url_adapt_ext
 */
char *url_adapt_ext(const char *src, size_t srclen, const char *name, const char *val, size_t *newlen)
{
	char buf[1024];

	snprintf(buf, sizeof(buf)-1, "%s=%s", name, val);

	return url_adapt(src, srclen, buf, newlen);
}
/* }}} */

/* {{{ url_adapt
 */
char *url_adapt(const char *src, size_t srclen, const char *data, size_t *newlen)
{
	char *out,*outp;
	int maxl,n;
	TSRMLS_FETCH();

	if(src==NULL) {
		US.state=STATE_NORMAL;
		if(US.tag)  { efree(US.tag);  US.tag =NULL; }
		if(US.attr) { efree(US.attr); US.attr=NULL; }
		if(US.val)  { efree(US.val);  US.val =NULL; }
		return NULL;
	}

	if(srclen==0) 
		srclen=strlen(src);

	out=malloc(srclen+1);
	maxl=srclen;
	n=srclen;

	*newlen=0;
	outp=out;

	while(n--) {
		switch(US.state) {
		case STATE_NORMAL:
			if(*src=='<') 
				US.state=STATE_TAG_START;
			break;

		case STATE_TAG_START:
			if(! isalnum(*src))
				US.state=STATE_NORMAL;
			US.state=STATE_TAG;
			US.ml=BUFSIZE;
			US.p=US.tag=erealloc(US.tag,US.ml);
			*(US.p)++=*src;
			US.l=1;
			break;

		case STATE_TAG:
			if(isalnum(*src)) {
				*(US.p)++ = *src;
				US.l++; 
				if(US.l==US.ml) {
					US.ml+=BUFSIZE;
					US.tag=erealloc(US.tag,US.ml);
					US.p = US.tag+US.l;
				}
			} else if (isspace(*src)) {
				US.state = STATE_IN_TAG;
				*US.p='\0';
				US.tag=erealloc(US.tag,US.l);
			} else {
				US.state = STATE_NORMAL;
				efree(US.tag);
				US.tag=NULL;
			}
			break;

		case STATE_IN_TAG:
			if(isalnum(*src)) {
				US.state=STATE_TAG_ATTR;
				US.ml=BUFSIZE;
				US.p=US.attr=erealloc(US.attr,US.ml);
				*(US.p)++=*src;
				US.l=1;
			} else if (! isspace(*src)) {
				US.state = STATE_NORMAL;
				efree(US.tag);
				US.tag=NULL;
			}
			break;

		case STATE_TAG_ATTR:
			if(isalnum(*src)) {
				*US.p++=*src;
				++US.l;
				if(US.l==US.ml) {
					US.ml+=BUFSIZE;
					US.attr=erealloc(US.attr,US.ml);
					US.p = US.attr+US.l;
				}
				if(US.l==US.ml) {
					US.ml+=BUFSIZE;
					US.attr=erealloc(US.attr,US.ml);
					US.p = US.attr+US.l;
				}
			} else if(isspace(*src)||(*src=='=')){
				US.state=STATE_TAG_IS;
				*US.p=0;
				US.attr=erealloc(US.attr,US.l);
			} else if(*src=='>') {
				US.state=STATE_NORMAL;
			} else {
				efree(US.attr);
				US.attr=NULL;
				US.state=STATE_IN_TAG;
			}
			break;
			
		case STATE_TAG_IS:
		case STATE_TAG_IS2:
			if(*src=='>'){
					US.state=STATE_NORMAL;
					if(! (US.attr_done)) {
						char *p;
						p=url_attr_addon(US.tag,US.attr,"",data);
						if(p) {
							int l= strlen(p);
							maxl+=l;
							out=realloc(out,maxl);
							outp=out+*newlen;
							strcpy(outp,p);
							outp+=l;
							*newlen+=l;
							efree(p);
						}
					}
			} else if(*src=='#') {
				if(! (US.attr_done)) {
					char *p;
					US.attr_done=1;
					p=url_attr_addon(US.tag,US.attr,"#",data);
					if(p) {
						int l= strlen(p);
						maxl+=l;
						out=realloc(out,maxl);
						outp=out+*newlen;
						strcpy(outp,p);
						outp+=l;
						*newlen+=l;
						efree(p);
					}
				}
			} else if(!isspace(*src)&&(*src!='=')) {
				US.ml=BUFSIZE;
				US.p=US.val=erealloc(US.val,US.ml);
				US.l=0;
				US.attr_done=0;
				if((*src=='"')||(*src=='\'')) {
					US.state=STATE_TAG_QVAL2;
					US.delim=*src;
				} else {
					US.state=STATE_TAG_VAL;
					*US.p++=*src;
					US.l++;
				}
			}
			break;


		case STATE_TAG_QVAL2:
			if(*src=='#') {
				if(! (US.attr_done)) {
					char *p;
					US.attr_done=1;
					*US.p='\0';
					p=url_attr_addon(US.tag,US.attr,US.val,data);
					if(p) {
						int l= strlen(p);
						maxl+=l;
						out=realloc(out,maxl);
						outp=out+*newlen;
						strcpy(outp,p);
						outp+=l;
						*newlen+=l;
						efree(p);
					}
				}
			} else if(*src==US.delim) {
				US.state=STATE_IN_TAG;
				*US.p='\0';
				if(! (US.attr_done)) {
					char *p;
					p=url_attr_addon(US.tag,US.attr,US.val,data);
					if(p) {
						int l= strlen(p);
						maxl+=l;
						out=realloc(out,maxl);
						outp=out+*newlen;
						strcpy(outp,p);
						outp+=l;
						*newlen+=l;
						efree(p);
					}
				}
				break;
			} else if(*src=='\\') {
				US.state=STATE_TAG_QVAL2b;
			} else if (*src=='>') {
				US.state=STATE_NORMAL;
			}
			
			*US.p++=*src;
			++US.l; 
			if(US.l==US.ml) {
				US.ml+=BUFSIZE;
				US.val=erealloc(US.val,US.ml);
				US.p = US.val+US.l;
			}
			
			break;
			
		case STATE_TAG_QVAL2b:
			US.state=STATE_TAG_QVAL2;
			*US.p++=*src;
			++US.l; 
			if(US.l==US.ml) {
				US.ml+=BUFSIZE;
				US.val=erealloc(US.val,US.ml);
				US.p = US.val+US.l;
			}
			break;

		case STATE_TAG_VAL:
		case STATE_TAG_VAL2:
			if(*src=='#') {
				if(! (US.attr_done)) {
					char *p;
					US.attr_done=1;
					*US.p='\0';
					p=url_attr_addon(US.tag,US.attr,US.val,data);
					if(p) {
						int l= strlen(p);
						maxl+=l;
						out=realloc(out,maxl);
						outp=out+*newlen;
						strcpy(outp,p);
						outp+=l;
						*newlen+=l;
						efree(p);
					}
					}
			} else if(isspace(*src)||(*src=='>')) {
				US.state=(*src=='>')?STATE_NORMAL:STATE_IN_TAG;
				*US.p='\0';
				if(! (US.attr_done)) {
					char *p;
					p=url_attr_addon(US.tag,US.attr,US.val,data);
					if(p) {
						int l= strlen(p);
						maxl+=l;
						out=realloc(out,maxl);
						outp=out+*newlen;
						strcpy(outp,p);
						outp+=l;
						*newlen+=l;
						efree(p);
					}
				}
			} else {
				*US.p++=*src;
				US.l++; 
				if(US.l==US.ml) {
					US.ml+=BUFSIZE;
					US.val=erealloc(US.val,US.ml);
					US.p = US.val+US.l;
				}
			}
			break;
		default:
			break;
		}

		*outp++=*src++;
		*newlen+=1;
	}
	*outp='\0';
	return out;
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
