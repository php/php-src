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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include "php.h"
#if defined(COMPILE_DL)
#include "phpdl.h"
#include "functions/dl.h"
#endif
#include "php3_snmp.h"
#include <sys/types.h>
#if MSVC5
#include <winsock.h>
#include <errno.h>
#include <process.h>
#include "win32/time.h"
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <netdb.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_SNMP

#ifndef __P
#ifdef __GNUC__
#define __P(args) args
#else
#define __P(args) ()
#endif
#endif

#include "asn1.h"
#include "snmp_api.h"
#include "snmp_client.h"
#include "snmp_impl.h"
#include "snmp.h"
#include "parse.h"
#include "mib.h"

/* ucd-snmp 3.3.1 changed the name of a few #defines... They've been changed back to the original ones in 3.5.3! */
#ifndef SNMP_MSG_GET
#define SNMP_MSG_GET GET_REQ_MSG
#define SNMP_MSG_GETNEXT GETNEXT_REQ_MSG
#endif

void _php3_snmp(INTERNAL_FUNCTION_PARAMETERS, int st);

/* constant - can be shared among threads */
static oid objid_mib[] = {1, 3, 6, 1, 2, 1};

/* Add missing prototype */
void sprint_variable(char *, oid *, int, struct variable_list *);

function_entry snmp_functions[] = {
	PHP_FE(snmpget, NULL)
	PHP_FE(snmpwalk, NULL)
	PHP_FE(snmprealwalk, NULL)
    {NULL,NULL,NULL}
};

php3_module_entry snmp_module_entry = {
	"SNMP",snmp_functions,php3i_snmp_init,NULL,NULL,NULL,php3_info_snmp,STANDARD_MODULE_PROPERTIES
};

#if COMPILE_DL
DLEXPORT php3_module_entry *get_module() { return &snmp_module_entry; };
#endif

/* THREAD_LS snmp_module php3_snmp_module; - may need one of these at some point */

int php3i_snmp_init(INIT_FUNC_ARGS) {
	init_mib();
	return SUCCESS;
}

void php3_info_snmp(ZEND_MODULE_INFO_FUNC_ARGS) {
	php3_printf("ucd-snmp");
}


/*
 * Generic SNMP object fetcher
 *
 * st=1 GET
 * st=2 WALK
 * st=3 WALK (returns OIDs additionally)
 */
void _php3_snmp(INTERNAL_FUNCTION_PARAMETERS, int st) {
	pval *a1, *a2, *a3, *a4, *a5;
	struct snmp_session session, *ss;
	struct snmp_pdu *pdu=NULL, *response;
	struct variable_list *vars;
    char *objid;
    oid name[MAX_NAME_LEN];
    int name_length;
    int status, count,rootlen=0,gotroot=0;
	oid root[MAX_NAME_LEN];
	char buf[2048];
	char buf2[2048];
	int keepwalking=1;
	long timeout=SNMP_DEFAULT_TIMEOUT;
	long retries=SNMP_DEFAULT_RETRIES;
	int myargc = ARG_COUNT(ht);
	
	if (myargc<3 || myargc>5 || getParameters(ht, myargc, &a1, &a2, &a3, &a4, &a5) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(a1);
	convert_to_string(a2);
	convert_to_string(a3);
	if(myargc>3) {
		convert_to_long(a4);
		timeout=a4->value.lval;
	}
	if(myargc>4) {
		convert_to_long(a5);
		retries=a5->value.lval;
	}
	objid=a3->value.str.val;
	
	if (st>=2) { /* walk */
		rootlen = MAX_NAME_LEN;
		if (strlen(objid)) { /* on a walk, an empty string means top of tree - no error */
			if (read_objid(objid, root, &rootlen)) {
				gotroot = 1;
			} else {
				php3_error(E_WARNING,"Invalid object identifier: %s\n", objid);
			}
		}
    	if (gotroot == 0) {
			memmove((char *)root, (char *)objid_mib, sizeof(objid_mib));
        	rootlen = sizeof(objid_mib) / sizeof(oid);
        	gotroot = 1;
    	}
	}

	memset(&session, 0, sizeof(struct snmp_session));
	session.peername = a1->value.str.val;

	session.version = SNMP_VERSION_1;
	/*
	 * FIXME: potential memory leak
	 * This is a workaround for an "artifact" (Mike Slifcak)
	 * in (at least) ucd-snmp 3.6.1 which frees
	 * memory it did not allocate
	 */
	session.community = (u_char *) strdup(a2->value.str.val);
	session.community_len = a2->value.str.len;
	session.retries = retries;
	session.timeout = timeout;

	session.authenticator = NULL;
	snmp_synch_setup(&session);
	ss = snmp_open(&session);
	if (ss == NULL){
		php3_error(E_WARNING,"Couldn't open snmp\n");
		RETURN_FALSE;
	}
	if (st>=2) {
		memmove((char *)name, (char *)root, rootlen * sizeof(oid));
		name_length = rootlen;
		/* prepare result array */
		array_init(return_value);	
	}

	while(keepwalking) {
		keepwalking=0;
		if (st==1) pdu = snmp_pdu_create(SNMP_MSG_GET);
		else if (st>=2) pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);

		if (st==1) {
			name_length = MAX_NAME_LEN;
			if (!read_objid(objid, name, &name_length)) {
				php3_error(E_WARNING,"Invalid object identifier: %s\n", objid);
				RETURN_FALSE;
			}
		}
		snmp_add_null_var(pdu, name, name_length);

retry:
		status = snmp_synch_response(ss, pdu, &response);
		if (status == STAT_SUCCESS) {
			if (response->errstat == SNMP_ERR_NOERROR) {
				for(vars = response->variables; vars; vars = vars->next_variable) {
					if (st>=2 && (vars->name_length < rootlen || memcmp(root, vars->name, rootlen * sizeof(oid))))
						continue;       /* not part of this subtree */

					sprint_value(buf,vars->name, vars->name_length, vars);
#if 0
					Debug("snmp response is: %s\n",buf);
#endif
					if (st==1) {
						RETVAL_STRING(buf,1);
					} else if (st==2) {
						/* Add to returned array */
						add_next_index_string(return_value,buf,1);
					} else if (st==3) {
						sprint_objid(buf2, name, name_length);
						add_assoc_string(return_value,buf2,buf,1);
					}
					if (st>=2) {
						if (vars->type != SNMP_ENDOFMIBVIEW && vars->type != SNMP_NOSUCHOBJECT && vars->type != SNMP_NOSUCHINSTANCE) {
							memmove((char *)name, (char *)vars->name,vars->name_length * sizeof(oid));
							name_length = vars->name_length;
							keepwalking = 1;
						}
					}
				}	
			} else {
				if (st!=2 || response->errstat != SNMP_ERR_NOSUCHNAME) {
					php3_error(E_WARNING,"Error in packet.\nReason: %s\n", snmp_errstring(response->errstat));
					if (response->errstat == SNMP_ERR_NOSUCHNAME) {
						for(count=1, vars = response->variables; vars && count != response->errindex;
							vars = vars->next_variable, count++);
						if (vars) sprint_objid(buf,vars->name, vars->name_length);
						php3_error(E_WARNING,"This name does not exist: %s\n",buf);
					}
					if (st==1) {
						if ((pdu = snmp_fix_pdu(response, SNMP_MSG_GET)) != NULL) goto retry;
					} else if (st>=2) {
						if ((pdu = snmp_fix_pdu(response, SNMP_MSG_GETNEXT)) != NULL) goto retry;
					}
					RETURN_FALSE;
				}
			}
		} else if (status == STAT_TIMEOUT) {
			php3_error(E_WARNING,"No Response from %s\n", a1->value.str.val);
			RETURN_FALSE;
		} else {    /* status == STAT_ERROR */
			php3_error(E_WARNING,"An error occurred, Quitting\n");
			RETURN_FALSE;
		}
		if (response) snmp_free_pdu(response);
	} /* keepwalking */
	snmp_close(ss);
}

/* {{{ proto string snmpget(string host, string community, string object_id [, int timeout [, int retries]]) 
   Fetch an SNMP object */
void php3_snmpget(INTERNAL_FUNCTION_PARAMETERS) {
	_php3_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);
}
/* }}} */

/* {{{ proto string snmpwalk(string host, string community, string object_id [, int timeout [, int retries]]) 
   Return all objects under the specified object id */
void php3_snmpwalk(INTERNAL_FUNCTION_PARAMETERS) {
	return _php3_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU,2);
}
/* }}} */

/* {{{ proto string snmprealwalk(string host, string community, string object_id [, int timeout [, int retries]])
   Return all objects including their respective object id withing the specified one */
PHP_FUNCTION(snmprealwalk)
{
	return _php3_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU,3);
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
