/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Adam Dickmeiss <adam@indexdata.dk>                          |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"

#if HAVE_YAZ
#include "ext/standard/info.h"
#include "php_yaz.h"

#include <yaz/proto.h>
#include <yaz/tcpip.h>
#include <yaz/pquery.h>
#include <yaz/diagbib1.h>
#include <yaz/otherinfo.h>
#include <yaz/marcdisp.h>
#include <yaz/yaz-util.h>
#include <yaz/yaz-version.h>

#define MAX_ASSOC 100

#define PHP_YAZ_DEBUG 1

typedef struct Yaz_ResultSetInfo *Yaz_ResultSet;
typedef struct Yaz_AssociationInfo *Yaz_Association;
typedef struct Yaz_RecordCacheInfo *Yaz_RecordCache;

struct Yaz_RecordCacheInfo {
	Z_NamePlusRecordList recordList;
	Yaz_RecordCache next;
};

struct Yaz_ResultSetInfo {
	Z_Query *query;
	Z_ReferenceId *referenceId;
	ODR odr;
	int resultCount;
	Yaz_ResultSet next;
	Z_NamePlusRecordList *recordList;
};

#define PHP_YAZ_SELECT_READ 1
#define PHP_YAZ_SELECT_WRITE 2
#define PHP_YAZ_STATE_CONNECTING	1
#define PHP_YAZ_STATE_ESTABLISHED	2
#define PHP_YAZ_STATE_CLOSED		3

#define PHP_YAZ_ERROR_CONNECTION_LOST (-1)
#define PHP_YAZ_ERROR_DECODE (-2)
#define PHP_YAZ_ERROR_ENCODE (-3)
#define PHP_YAZ_ERROR_CONNECT (-4)
#define PHP_YAZ_ERROR_INIT (-5)
#define PHP_YAZ_ERROR_TIMEOUT (-6)

struct Yaz_AssociationInfo {
	char *host_port;
	int num_databaseNames;
	char **databaseNames;
	COMSTACK cs;
	char *cookie;
	char *auth_open;
	char *user;
	char *group;
	char *pass;
	int error;
	char *addinfo;
	Yaz_ResultSet resultSets;
	int order;
	int state;
	int mask_select;
	int reconnect_flag;
	ODR odr_in;
	ODR odr_out;
	char *buf_out;
	int len_out;
	char *buf_in;
	int len_in;
	int (*action)(Yaz_Association t);
	int resultSetStartPoint;
	int numberOfRecordsRequested;
	char *elementSetNames;
	char *preferredRecordSyntax;
};

static Yaz_Association yaz_association_mk ()
{
	Yaz_Association p = xmalloc (sizeof(*p));
	p->host_port = 0;
	p->num_databaseNames = 0;
	p->databaseNames = 0;
	p->cs = 0;
	p->cookie = 0;
	p->auth_open = 0;
	p->user = 0;
	p->group = 0;
	p->pass = 0;
	p->error = 0;
	p->addinfo = 0;
	p->resultSets = 0;
	p->order = 0;
	p->state = PHP_YAZ_STATE_CLOSED;
	p->mask_select = 0;
	p->reconnect_flag = 0;
	p->odr_in = odr_createmem (ODR_DECODE);
	p->odr_out = odr_createmem (ODR_ENCODE);
	p->buf_out = 0;
	p->len_out = 0;
	p->buf_in = 0;
	p->len_in = 0;
	p->action = 0;
	p->resultSetStartPoint = 1;
	p->numberOfRecordsRequested = 10;
	p->elementSetNames = 0;
	p->preferredRecordSyntax = 0;
	return p;
}

static void yaz_association_destroy (Yaz_Association p)
{
	int i;
	if (!p)
		return ;
	xfree (p->host_port);
	for (i = 0; i<p->num_databaseNames; i++)
		xfree (p->databaseNames[i]);
	xfree (p->databaseNames);
	if (p->cs)
		cs_close (p->cs);
	xfree (p->cookie);
	xfree (p->auth_open);
	xfree (p->user);
	xfree (p->group);
	xfree (p->pass);
	xfree (p->addinfo);
	odr_destroy (p->odr_in);
	odr_destroy (p->odr_out);
	/* buf_out */
	/* buf_in */
	/* action */
	xfree (p->elementSetNames);
	xfree (p->preferredRecordSyntax);
}

static Yaz_ResultSet yaz_resultset_mk()
{
	ODR odr = odr_createmem (ODR_ENCODE);
	Yaz_ResultSet p = odr_malloc (odr, sizeof(*p));
	
	p->query = 0;
	p->referenceId = 0;
	p->odr = odr;
	p->resultCount = 0;
	p->next = 0;
	p->recordList = 0;
	return p;
}

static void yaz_resultset_destroy (Yaz_ResultSet p)
{
	if (!p)
		return;
	if (p->odr)
		odr_destroy (p->odr);
}

#ifdef ZTS
static MUTEX_T yaz_mutex;
#endif

static Yaz_Association *shared_associations;
static int order_associations;

/*
  when id = 0, it means all targes...
  id = yaz_connect(zurl, user, group, pass);
  yaz_set_db(id, db)
  yaz_error(id)
  yaz_errno(id)
  yaz_addinfo(id)
  
  yaz_wait()
  yaz_range(id, from, to, syntax, elementset)
  yaz_search(id, type, query)
  yaz_hits(id)
  yaz_record(id, pos)
  
  yaz_scan(id)
  yaz_scan_point(id, scanterm, before, after)
  yaz_scan_result(id, pos)
  
  yaz_close(id)
*/

function_entry yaz_functions [] = {
	PHP_FE(yaz_connect, NULL)
	PHP_FE(yaz_close, NULL)
	PHP_FE(yaz_search, NULL)
	PHP_FE(yaz_wait, NULL)
	PHP_FE(yaz_errno, NULL)
	PHP_FE(yaz_error, NULL)
	PHP_FE(yaz_addinfo, NULL)
	PHP_FE(yaz_hits, NULL)
	PHP_FE(yaz_record, NULL)
	PHP_FE(yaz_syntax, NULL)
	PHP_FE(yaz_element, NULL)
	PHP_FE(yaz_range, NULL)
	{NULL, NULL, NULL}
};

static Yaz_Association get_assoc (pval **id)
{
	Yaz_Association assoc;
	int i;
	convert_to_long_ex(id);
	i = (*id)->value.lval;
	
#ifdef ZTS
	tsrm_mutex_lock (yaz_mutex);
#endif
	if (i < 1 || i > MAX_ASSOC || !shared_associations ||
		!(assoc = shared_associations[i-1]))
	{
#ifdef ZTS
		tsrm_mutex_unlock (yaz_mutex);
#endif
		return 0;
	}
	return assoc;
}

static void release_assoc (Yaz_Association assoc)
{
#ifdef ZTS
	if (assoc)
		tsrm_mutex_unlock(yaz_mutex);
#endif
}
static void do_close (Yaz_Association p)
{
	p->mask_select = 0;
	p->state = PHP_YAZ_STATE_CLOSED;
	if (p->cs)
	{
		cs_close (p->cs);
		p->cs = 0;
	}
}

static void do_connect (Yaz_Association p)
{
	void *addr;
	char *cp;
	
	p->reconnect_flag = 0;
	p->cs = cs_create (tcpip_type, 0, PROTO_Z3950);

	/* see if the host_port is prefixed by proxy. */
	cp = strchr (p->host_port, ',');
	if (cp)
		*cp = '\0';	
	addr = tcpip_strtoaddr (p->host_port);
	if (cp)
		*cp = ',';
	if (!addr)
	{
		do_close(p);
		p->error = PHP_YAZ_ERROR_CONNECT;
		return;
	}
	cs_connect (p->cs, addr);
	p->state = PHP_YAZ_STATE_CONNECTING;
	p->mask_select = PHP_YAZ_SELECT_READ | PHP_YAZ_SELECT_WRITE;
}

static void response_diag (Yaz_Association t, Z_DiagRec *p)
{
	Z_DefaultDiagFormat *r;
	char *addinfo = 0;

	xfree (t->addinfo);
	t->addinfo = 0;
	if (p->which != Z_DiagRec_defaultFormat)
	{
		t->error = PHP_YAZ_ERROR_DECODE;
		return;
	}
	r = p->u.defaultFormat;
#ifdef ASN_COMPILED
	switch (r->which)
	{
	case Z_DefaultDiagFormat_v2Addinfo:
		addinfo = r->u.v2Addinfo;
		break;
	case Z_DefaultDiagFormat_v3Addinfo:
		addinfo = r->u.v3Addinfo;
		break;
	}
#else
	addinfo = r->addinfo;
#endif
	if (addinfo)
		t->addinfo = xstrdup (addinfo);
	t->error = *r->condition;
}

static int send_present (Yaz_Association t);

static void handle_records (Yaz_Association t, Z_Records *sr,
							int present_phase)
{
	if (sr && sr->which == Z_Records_NSD)
	{
#ifdef ASN_COMPILED
		Z_DiagRec dr, *dr_p = &dr;
		dr.which = Z_DiagRec_defaultFormat;
		dr.u.defaultFormat = sr->u.nonSurrogateDiagnostic;
		
		response_diag (t, dr_p);
#else
		response_diag (t, sr->u.nonSurrogateDiagnostic);
#endif
	}
	else if (sr && sr->which == Z_Records_multipleNSD)
	{
		if (sr->u.multipleNonSurDiagnostics->num_diagRecs >= 1)
			response_diag(t, sr->u.multipleNonSurDiagnostics->diagRecs[0]);
		else
			t->error = PHP_YAZ_ERROR_DECODE;
	}
	else 
	{
		if (t->numberOfRecordsRequested > 0 && !t->resultSets->recordList)
		{
			int i;
			
			t->resultSets->recordList =
				odr_malloc (t->resultSets->odr,
							sizeof(*t->resultSets->recordList));
			t->resultSets->recordList->records =
				odr_malloc (t->resultSets->odr, t->numberOfRecordsRequested
							 * sizeof(*t->resultSets->recordList->records));
			for (i = 0; i < t->numberOfRecordsRequested; i++)
				t->resultSets->recordList->records[i] = 0;
			if (t->numberOfRecordsRequested + t->resultSetStartPoint-1 > 
				t->resultSets->resultCount)
				t->numberOfRecordsRequested = t->resultSets->resultCount -
					t->resultSetStartPoint + 1;
			t->resultSets->recordList->num_records =
				t->numberOfRecordsRequested;
		}
		if (sr && sr->which == Z_Records_DBOSD)
		{
			int j, i;
			NMEM nmem = odr_extract_mem (t->odr_in);
			Z_NamePlusRecordList *p =
				sr->u.databaseOrSurDiagnostics;
			for (j = 0; j < t->resultSets->recordList->num_records; j++)
				if (!t->resultSets->recordList->records[j])
					break;
			for (i = 0; i<p->num_records; i++)
				t->resultSets->recordList->records[i+j] = p->records[i];
			/* transfer our response to search_nmem .. we need it later */
			nmem_transfer (t->resultSets->odr->mem, nmem);
			nmem_destroy (nmem);
			if (present_phase && p->num_records == 0)
			{
				/* present response and we didn't get any records! */
				t->error = PHP_YAZ_ERROR_DECODE;
				t->resultSets->recordList = 0;
			}
		}
		else if (present_phase)
		{
			/* present response and we didn't get any records! */
			t->error = PHP_YAZ_ERROR_DECODE;
			t->resultSets->recordList = 0;
		}
	}
}

static void search_response (Yaz_Association t, Z_SearchResponse *sr)
{
	t->resultSets->resultCount = *sr->resultCount;
	handle_records (t, sr->records, 0);
}

static void present_response (Yaz_Association t, Z_PresentResponse *pr)
{
	handle_records (t, pr->records, 1);
}

static void handle_apdu (Yaz_Association t, Z_APDU *apdu)
{
	Z_InitResponse *initrs;

	t->mask_select = 0;

	switch(apdu->which)
	{
	case Z_APDU_initResponse:
		initrs = apdu->u.initResponse;
		if (!*initrs->result)
		{
			t->error = PHP_YAZ_ERROR_INIT;
		}
		else
		{
			char *cookie =
				yaz_oi_get_string_oidval (&apdu->u.initResponse->otherInfo,
										  VAL_COOKIE, 1, 0);
			if (cookie)
			{
				xfree(t->cookie);
				t->cookie = xstrdup(cookie);
			}
			(*t->action) (t);
		}
		break;
	case Z_APDU_searchResponse:
		search_response (t, apdu->u.searchResponse);
		send_present (t);
		break;
	case Z_APDU_presentResponse:
		present_response (t, apdu->u.presentResponse);
		send_present (t);
		break;
#if 0
	case Z_APDU_scanResponse:
		zlog (req, t->name, " scan response");
		scanResponse (req, t, apdu->u.scanResponse);
		break;
#endif
#if USE_ES
	case Z_APDU_extendedServicesResponse:
		zlog (req, t->name, " ES response");
		esResponse (req, t, apdu->u.extendedServicesResponse);
		break;
#endif
	case Z_APDU_close:
		do_close(t);
		if (t->reconnect_flag)
			do_connect (t);
		else
			t->error = PHP_YAZ_ERROR_CONNECTION_LOST;
		break;
	default:
		do_close (t);
		t->error = PHP_YAZ_ERROR_DECODE;
		break;
	}
}

static int do_read (Yaz_Association t)
{
	int r;
	Z_APDU *apdu;

	r = cs_get (t->cs, &t->buf_in, &t->len_in);
	if (r == 1)
		return 0;
	if (r <= 0)
	{
		do_close (t);
		if (t->reconnect_flag)
		{
			do_connect (t);
		}
		else
		{
			do_close (t);
			t->error = PHP_YAZ_ERROR_CONNECTION_LOST;
		}
	}
	else
	{
		odr_reset (t->odr_in);
		odr_setbuf (t->odr_in, t->buf_in, r, 0);
		if (!z_APDU (t->odr_in, &apdu, 0, 0))
		{
			do_close (t);
			t->error = PHP_YAZ_ERROR_DECODE;
		}
		else
		{
			/* apdu_log(req, t->odr_print, apdu); */
			handle_apdu (t, apdu);
		}
	}
	return 1;
}

static int do_write (Yaz_Association t)
{
	int r;
	
	if ((r=cs_put (t->cs, t->buf_out, t->len_out)) < 0)
	{
		if (t->reconnect_flag)
		{
			do_close (t);
			do_connect (t);
		}
		else
		{
			if (t->state == PHP_YAZ_STATE_CONNECTING)
				t->error = PHP_YAZ_ERROR_CONNECT;
			else
				t->error = PHP_YAZ_ERROR_CONNECTION_LOST;
			do_close (t);
			return 1;
		}
	}
	else if (r == 1)
	{
		t->state = PHP_YAZ_STATE_ESTABLISHED;
		t->mask_select = PHP_YAZ_SELECT_READ|PHP_YAZ_SELECT_WRITE;
	}
	else
	{
		t->state = PHP_YAZ_STATE_ESTABLISHED;
		t->mask_select = PHP_YAZ_SELECT_READ;
	}
	return 0;
}


static int send_APDU (Yaz_Association t, Z_APDU *a)
{
	if (t->cookie)
	{
		Z_OtherInformation **oi;
		yaz_oi_APDU(a, &oi);
		yaz_oi_set_string_oidval(oi, t->odr_out, VAL_COOKIE, 1, t->cookie);
	}
/* from ZAP */
#if 0
	if (req->request->connection)
	{
		Z_OtherInformation **oi;
		zlog (req, t->name, " encoding client_ip");
		yaz_oi_APDU(a, &oi);
		yaz_oi_set_string_oidval(oi, t->odr_out, VAL_CLIENT_IP, 1,
								 req->request->connection->remote_ip);
	}
#endif
	if (!z_APDU(t->odr_out, &a, 0, 0))
	{
		do_close (t);
		t->error = PHP_YAZ_ERROR_ENCODE;
		return -1;
	}
	/* apdu_log(req, t->odr_print, a); */
	t->buf_out = odr_getbuf(t->odr_out, &t->len_out, 0);
	odr_reset(t->odr_out);
	do_write (t);
	return 0;	
}

static char **set_DatabaseNames (Yaz_Association t, int *num)
{
	char **databaseNames;
	char *c, *cp = strchr (t->host_port, '/');
	int no = 2;

	if (cp)
	{
		c = cp;
		while ((c = strchr(c, '+')))
		{
			c++;
			no++;
		}
	}
	else
		cp = "/Default";
	databaseNames = odr_malloc (t->odr_out, no * sizeof(*databaseNames));
	no = 0;
	while (*cp)
	{
		c = ++cp;
		c = strchr (c, '+');
		if (!c)
			c = cp + strlen(cp);
		else if (c == cp)
			continue;
		/* cp ptr to first char of db name, c is char following db name */
		databaseNames[no] = odr_malloc (t->odr_out, 1+c-cp);
		memcpy (databaseNames[no], cp, c-cp);
		databaseNames[no][c-cp] = '\0';
		no++;
		cp = c;
	}
	databaseNames[no] = NULL;
	*num = no;
	return databaseNames;
}

static int send_search (Yaz_Association t)
{
	Yaz_ResultSet r = t->resultSets;
	Z_APDU *apdu = zget_APDU(t->odr_out, Z_APDU_searchRequest);
	Z_SearchRequest *sreq = apdu->u.searchRequest;
	
	/* resultSetPrepare (req, t, req->cur_pa); */
	if (t->resultSetStartPoint == 1)
	{
		sreq->largeSetLowerBound = odr_malloc (t->odr_out, sizeof(int));
		*sreq->largeSetLowerBound = 999999;
		sreq->smallSetUpperBound = &t->numberOfRecordsRequested;
		sreq->mediumSetPresentNumber = &t->numberOfRecordsRequested;
		if (t->elementSetNames && *t->elementSetNames)
		{
			Z_ElementSetNames *esn = odr_malloc (t->odr_out, sizeof(*esn));
			
			esn->which = Z_ElementSetNames_generic;
			esn->u.generic = t->elementSetNames;
			sreq->mediumSetElementSetNames = esn;
			sreq->smallSetElementSetNames = esn;
		}
	}
	else
	{
		sreq->smallSetUpperBound = odr_malloc (t->odr_out, sizeof(int));
		*sreq->smallSetUpperBound = 0;
		sreq->largeSetLowerBound = odr_malloc (t->odr_out, sizeof(int));
		*sreq->largeSetLowerBound = 1;
		sreq->mediumSetPresentNumber = odr_malloc (t->odr_out, sizeof(int));
		*sreq->mediumSetPresentNumber = 0;
	}
	sreq->query = r->query;
	if (t->preferredRecordSyntax && *t->preferredRecordSyntax)
	{
		struct oident ident;
		
		ident.proto = PROTO_Z3950;
		ident.oclass = CLASS_RECSYN;
		ident.value = oid_getvalbyname (t->preferredRecordSyntax);
		sreq->preferredRecordSyntax =
			odr_oiddup (t->odr_out, oid_getoidbyent (&ident));
	}
	sreq->databaseNames = set_DatabaseNames (t, &sreq->num_databaseNames);
	
	send_APDU (t, apdu);
	return 1;
}

static int send_present (Yaz_Association t)
{
	Z_APDU *apdu = zget_APDU(t->odr_out, Z_APDU_presentRequest);
	Z_PresentRequest *req = apdu->u.presentRequest;
	int i = 0;
	
	if (!t->resultSets->recordList)	  /* no records to retrieve at all .. */
	{
		return 0;
	}
	
	while (1)
	{
		if (i >= t->resultSets->recordList->num_records) 
		{						/* got all records ... */
			/* searchHits (zreq, t); */
			return 0;
		}
		if (!t->resultSets->recordList->records[i])
			break;
		i++;
	}
	/* got record(s) to retrieve */
	
	req->resultSetStartPoint = odr_malloc (t->odr_out, sizeof(int));
	*req->resultSetStartPoint = t->resultSetStartPoint + i;
	
	req->numberOfRecordsRequested = odr_malloc (t->odr_out, sizeof(int));
	*req->numberOfRecordsRequested = t->resultSets->recordList->num_records - i;
	
	if (t->preferredRecordSyntax && *t->preferredRecordSyntax)
	{
		struct oident ident;
		
		ident.proto = PROTO_Z3950;
		ident.oclass = CLASS_RECSYN;
		ident.value = oid_getvalbyname (t->preferredRecordSyntax);
		req->preferredRecordSyntax =
			odr_oiddup (t->odr_out, oid_getoidbyent (&ident));
	}
	
	if (t->elementSetNames && *t->elementSetNames)
	{
		Z_ElementSetNames *esn = odr_malloc (t->odr_out, sizeof(*esn));
		Z_RecordComposition *compo = odr_malloc (t->odr_out, sizeof(*compo));
		
		esn->which = Z_ElementSetNames_generic;
		esn->u.generic = t->elementSetNames;
		compo->which = Z_RecordComp_simple;
		compo->u.simple = esn;
		req->recordComposition = compo;
	}
	send_APDU (t, apdu);
	return 1;
}

static void send_init(Yaz_Association t)
{
	char *cp;
	int i = 0;
	Z_APDU *apdu = zget_APDU(t->odr_out, Z_APDU_initRequest);
	Z_InitRequest *ireq = apdu->u.initRequest;
	Z_IdPass *pass = odr_malloc(t->odr_out, sizeof(*pass));
	Z_IdAuthentication *auth = odr_malloc(t->odr_out, sizeof(*auth));
	const char *auth_open = t->auth_open;
	const char *auth_groupId = t->group;
	const char *auth_userId = t->user;
	const char *auth_password = t->pass;
	
	ODR_MASK_SET(ireq->options, Z_Options_search);
	ODR_MASK_SET(ireq->options, Z_Options_present);
	ODR_MASK_SET(ireq->options, Z_Options_namedResultSets);
	ODR_MASK_SET(ireq->options, Z_Options_scan);
	
	ODR_MASK_SET(ireq->protocolVersion, Z_ProtocolVersion_1);
	ODR_MASK_SET(ireq->protocolVersion, Z_ProtocolVersion_2);
	ODR_MASK_SET(ireq->protocolVersion, Z_ProtocolVersion_3);
	
	ireq->implementationName = "PHP/YAZ";
	
	*ireq->maximumRecordSize = 1024*1024;
	*ireq->preferredMessageSize = 1024*1024;
	
	if (auth_open && *auth_open)
	{
		auth->which = Z_IdAuthentication_open;
		auth->u.open = odr_malloc(t->odr_out, strlen(auth_open)+1);
		strcpy(auth->u.open, auth_open);
		ireq->idAuthentication = auth;
	}
	pass->groupId = 0;
	if (auth_groupId && *auth_groupId)
	{
		pass->groupId = odr_malloc(t->odr_out, strlen(auth_groupId)+1);
		strcpy(pass->groupId, auth_groupId);
		i++;
	}
	pass->userId = 0;
	if (auth_userId && *auth_userId)
	{
		pass->userId = odr_malloc(t->odr_out, strlen(auth_userId)+1);
		strcpy(pass->userId, auth_userId);
		i++;
	}
	pass->password = 0;
	if (auth_password && *auth_password)
	{
		pass->password = odr_malloc(t->odr_out, strlen(auth_password)+1);
		strcpy(pass->password, auth_password);
		i++;
	}
	if(i)
	{
		auth->which = Z_IdAuthentication_idPass;
		auth->u.idPass = pass;
		ireq->idAuthentication = auth;
	}

	/* see if proxy has been specified ... */
	cp = strchr (t->host_port, ',');
	if (cp && cp[1])
		yaz_oi_set_string_oidval(&ireq->otherInfo, t->odr_out,
					 VAL_PROXY, 1, cp+1);
	send_APDU (t, apdu);
}

static int do_event (int *id)
{
	fd_set input, output;
	int i;
	int no = 0;
	int max_fd = 0;
	struct timeval tv;
	
	tv.tv_sec = 15;
	tv.tv_usec = 0;
	
#ifdef ZTS
	tsrm_mutex_lock (yaz_mutex);
#endif
	FD_ZERO (&input);
	FD_ZERO (&output);
	for (i = 0; i < MAX_ASSOC; i++)
	{
		Yaz_Association p = shared_associations[i];
		int fd;
		if (!p || p->order != order_associations || !p->cs)
			continue;
		fd = cs_fileno (p->cs);
		if (max_fd < fd)
			max_fd = fd;
		if (p->mask_select & PHP_YAZ_SELECT_READ)
		{
			FD_SET (fd, &input);
			no++;
		}
		if (p->mask_select & PHP_YAZ_SELECT_WRITE)
		{
			FD_SET (fd, &output);
			no++;
		}
	}
#ifdef ZTS
	tsrm_mutex_unlock (yaz_mutex);
#endif
	if (!no)
		return 0;
	no = select (max_fd+1, &input, &output, 0, &tv);
#ifdef ZTS
	tsrm_mutex_lock (yaz_mutex);
#endif
	for (i = 0; i<MAX_ASSOC; i++)
	{
		int fd;
		Yaz_Association p = shared_associations[i];
		if (!p || p->order != order_associations || !p->cs)
			continue;
		*id = i+1;
		fd =cs_fileno(p->cs);
		if (no <= 0)
		{
			if (p->mask_select)	   /* only mark for those still pending */
			{
				if (p->state == PHP_YAZ_STATE_CONNECTING)
					p->error = PHP_YAZ_ERROR_CONNECT;
				else
					p->error = PHP_YAZ_ERROR_TIMEOUT;
				do_close (p);
			}
		}
		else if (p->state == PHP_YAZ_STATE_CONNECTING)
		{
			if (FD_ISSET (fd, &input))
			{
				do_close(p);
				p->error = PHP_YAZ_ERROR_CONNECT;
			}
			else if (FD_ISSET (fd, &output))
			{
				send_init(p);
			}
		}
		else if (p->state == PHP_YAZ_STATE_ESTABLISHED)
		{
			if (FD_ISSET (fd, &input))
				do_read (p);
			if (p->cs && FD_ISSET (fd, &output))
				do_write (p);
		}
		else
		{
			do_close (p);
			p->error = PHP_YAZ_ERROR_CONNECTION_LOST;
		}
	}
#ifdef ZTS
	tsrm_mutex_unlock (yaz_mutex);
#endif
	return no;
}

/* {{{ proto int yaz_connect(string zurl [, string user [, string group, string pass]])
   Create target with given zurl. Returns positive id if successful. */
PHP_FUNCTION(yaz_connect)
{
	int i;
	char *cp;
	char *zurl_str, *user_str = 0, *group_str = 0, *pass_str = 0;
	pval **zurl, **user = 0, **group = 0, **pass = 0;
	if (ZEND_NUM_ARGS() == 1)
	{
		if (zend_get_parameters_ex (1, &zurl) == FAILURE)
			WRONG_PARAM_COUNT;
	}
	else if (ZEND_NUM_ARGS() == 2)
	{
		if (zend_get_parameters_ex (2, &zurl, &user) == FAILURE)
			WRONG_PARAM_COUNT;
		convert_to_string_ex (user);
		user_str = (*user)->value.str.val;
	}
	else if (ZEND_NUM_ARGS() == 4)
	{
		if (zend_get_parameters_ex (4, &zurl, &user, &group, &pass) == FAILURE)
			WRONG_PARAM_COUNT;
		convert_to_string_ex (user);
		user_str = (*user)->value.str.val;
		convert_to_string_ex (group);
		group_str = (*group)->value.str.val;
		convert_to_string_ex (pass);
		pass_str = (*pass)->value.str.val;
	}
	else
	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex (zurl);
	zurl_str = (*zurl)->value.str.val;
	for (cp = zurl_str; *cp && strchr("\t\n ", *cp); cp++)
		;
	if (!*cp)
		RETURN_LONG(0);
		
	/* see if we have it already ... */
#ifdef ZTS
	tsrm_mutex_lock (yaz_mutex);
#endif
	for (i = 0; i<MAX_ASSOC; i++)
		if (shared_associations[i] && shared_associations[i]->host_port &&
			shared_associations[i]->order != order_associations &&
			!strcmp (shared_associations[i]->host_port, zurl_str))
			break;
	if (i == MAX_ASSOC)
	{
		/* we didn't have it (or already in use) */
		int i0 = -1;
		int min_order = 2000000000;
		/* find completely free slot or the oldest one */
		for (i = 0; i<MAX_ASSOC && shared_associations[i]; i++)
			if (shared_associations[i]->order < min_order
				&& shared_associations[i]->order != order_associations)
			{
				min_order = shared_associations[i]->order;
				i0 = i;
			}
		if (i == MAX_ASSOC)
		{
			i = i0;
			if (i == -1)
			{
#ifdef ZTS
				tsrm_mutex_unlock (yaz_mutex);
#endif
				RETURN_LONG(0);          /* no free slot */
			}
			else                         /* "best" free slot */
				yaz_association_destroy(shared_associations[i]);
		}
		shared_associations[i] = yaz_association_mk ();
		shared_associations[i]->host_port = xstrdup (zurl_str);
	}
	shared_associations[i]->order = order_associations;
	shared_associations[i]->error = 0;
	shared_associations[i]->numberOfRecordsRequested = 10;
	shared_associations[i]->resultSetStartPoint = 1;
	if (user && !group && !pass)
		shared_associations[i]->auth_open = xstrdup (user_str);
	if (user && group && pass)
	{
		shared_associations[i]->user = xstrdup (user_str);
		shared_associations[i]->group = xstrdup (group_str);
		shared_associations[i]->pass = xstrdup (pass_str);
	}
#ifdef ZTS
	tsrm_mutex_unlock (yaz_mutex);
#endif
	RETURN_LONG(i+1);
}
/* }}} */

/* {{{ proto int yaz_close(int id)
   Destory and close target */
PHP_FUNCTION(yaz_close)
{
	Yaz_Association p;
	pval **id;
	int i;
	if (ZEND_NUM_ARGS() != 1)
		WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex (1, &id) == FAILURE)
		RETURN_FALSE;
	p = get_assoc (id);
	if (!p)
		RETURN_FALSE;
	convert_to_long_ex (id);
	i = (*id)->value.lval - 1;
	yaz_association_destroy (shared_associations[i]);
	shared_associations[i] = 0;
	release_assoc (p);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int yaz_search(int id, string type, string query)
   Specify query of type for search - returns true if successful */
PHP_FUNCTION(yaz_search)
{
	char *query_str, *type_str;
	pval **id, **type, **query;
	Yaz_Association p;
	Yaz_ResultSet r;
	if (ZEND_NUM_ARGS() != 3)
		WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(3, &id, &type, &query) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	p = get_assoc (id);
	if (!p)
	{
		RETURN_FALSE;
	}
	convert_to_string_ex (type);
	type_str = (*type)->value.str.val;
	convert_to_string_ex (query);
	query_str = (*query)->value.str.val;
	yaz_resultset_destroy (p->resultSets);
	r = p->resultSets = yaz_resultset_mk();
	r->query = odr_malloc (r->odr, sizeof(*r->query));
	if (!strcmp (type_str, "rpn"))
	{
		r->query->which = Z_Query_type_1;
		r->query->u.type_1 = p_query_rpn (r->odr, PROTO_Z3950, query_str);
		if (!r->query->u.type_1)
		{
			yaz_resultset_destroy(r);
			p->resultSets = 0;
			RETVAL_FALSE;
		}
		else
		{
			RETVAL_TRUE;
		}
	}
	else
	{
		yaz_resultset_destroy(r);
		p->resultSets = 0;
		RETVAL_FALSE;
	}
	release_assoc (p);
}
/* }}} */

/* {{{ proto int yaz_wait()
   Process all outstanding events. */
PHP_FUNCTION(yaz_wait)
{
	int i;
	int id;
#ifdef ZTS
	tsrm_mutex_lock (yaz_mutex);
#endif
	for (i = 0; i<MAX_ASSOC; i++)
	{
		Yaz_Association p = shared_associations[i];
		if (!p || p->order != order_associations || !p->resultSets)
			continue;
		p->action = send_search;
		if (!p->cs)
		{
			do_connect (p);
		}
		else
		{
			p->reconnect_flag = 1;
			send_search (p);
		}
	}
#ifdef ZTS
	tsrm_mutex_unlock (yaz_mutex);
#endif
	while (do_event(&id))
		;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int yaz_errno(int id)
   Return last error number (>0 for bib-1 diagnostic, <0 for other error, 0 for no error */
PHP_FUNCTION(yaz_errno)
{
	pval **id;
	Yaz_Association p;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &id) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	p = get_assoc (id);
	if (!p)
	{
		RETURN_LONG(0);
	}
	RETVAL_LONG(p->error);
	release_assoc (p);
}
/* }}} */

/* {{{ proto string yaz_error(int id)
   Return last error message */
PHP_FUNCTION(yaz_error)
{
	pval **id;
	Yaz_Association p;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &id) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	p = get_assoc (id);
	if (p && p->error)
	{
		const char *msg = 0;
		if (p->error < 0)
		{
			switch (p->error)
			{
			case PHP_YAZ_ERROR_CONNECTION_LOST:
				msg = "connection lost";
				break;
			case PHP_YAZ_ERROR_DECODE:
				msg = "decoding failure";
				break;
			case PHP_YAZ_ERROR_ENCODE:
				msg = "encoding failure";
				break;
			case PHP_YAZ_ERROR_CONNECT:
				msg = "connect failed";
				break;
			case PHP_YAZ_ERROR_INIT:
				msg = "initialization failed";
				break;
			case PHP_YAZ_ERROR_TIMEOUT:
				msg = "timeout failure";
				break;
			default:
				msg = "unknown failure";
				break;
			}
		}
		else
		{
			msg = diagbib1_str (p->error);
			if (!msg)
				msg = "unknown diagnostic";
		}
		/* Not macro using because RETURN_STRING throws away const */
		return_value->value.str.len = strlen(msg);
		return_value->value.str.val =
			estrndup(msg, return_value->value.str.len);
		return_value->type = IS_STRING;
	}
	release_assoc (p);
}
/* }}} */

/* {{{ proto string yaz_addinfo(int id)
   Return additional info for last error (empty string if none) */
PHP_FUNCTION(yaz_addinfo)
{
	pval **id;
	Yaz_Association p;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &id) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	p = get_assoc (id);
	if (p && p->error > 0 && p->addinfo && *p->addinfo)
	{
		RETVAL_STRING(p->addinfo, 1);
	}
	release_assoc (p);
}
/* }}} */

/* {{{ proto int yaz_hits(int id)
   Return number of hits (result count) for last search */
PHP_FUNCTION(yaz_hits)
{
	pval **id;
	Yaz_Association p;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &id) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	p = get_assoc (id);
	if (!p || !p->resultSets)
	{
		RETVAL_LONG(0);
	}
	else
	{
		RETVAL_LONG(p->resultSets->resultCount);
	}
	release_assoc (p);
}
/* }}} */

static Z_GenericRecord *marc_to_grs1(const char *buf, ODR o, Odr_oid *oid)
{
    int entry_p;
    int record_length;
    int indicator_length;
    int identifier_length;
    int base_address;
    int length_data_entry;
    int length_starting;
    int length_implementation;
    int max_elements = 256;
    Z_GenericRecord *r = odr_malloc (o, sizeof(*r));
    r->elements = odr_malloc (o, sizeof(*r->elements) * max_elements);
    r->num_elements = 0;
	
    record_length = atoi_n (buf, 5);
    if (record_length < 25)
        return 0;
    indicator_length = atoi_n (buf+10, 1);
    identifier_length = atoi_n (buf+11, 1);
    base_address = atoi_n (buf+12, 4);
	
    length_data_entry = atoi_n (buf+20, 1);
    length_starting = atoi_n (buf+21, 1);
    length_implementation = atoi_n (buf+22, 1);
	
    for (entry_p = 24; buf[entry_p] != ISO2709_FS; )
    {
        entry_p += 3+length_data_entry+length_starting;
        if (entry_p >= record_length)
            return 0;
    }
    base_address = entry_p+1;
    for (entry_p = 24; buf[entry_p] != ISO2709_FS; )
    {
		Z_TaggedElement *tag;
        int data_length;
		int data_offset;
		int end_offset;
		int i;
		char tag_str[4];
		
        memcpy (tag_str, buf+entry_p, 3);
		entry_p += 3;
        tag_str[3] = '\0';
		
        if ((r->num_elements + 1) >= max_elements)
		{
			Z_TaggedElement **tmp = r->elements;
			
			/* double array space, throw away old buffer (nibble memory) */
			r->elements = odr_malloc(o, sizeof(*r->elements) *
									 (max_elements *= 2));
			memcpy(r->elements, tmp, r->num_elements * sizeof(*tmp));
		}
		tag = r->elements[r->num_elements++] = odr_malloc (o, sizeof(*tag));
		tag->tagType = odr_malloc(o, sizeof(*tag->tagType));
		*tag->tagType = 3;
		tag->tagOccurrence = 0;
		tag->metaData = 0;
		tag->appliedVariant = 0;
		tag->tagValue = odr_malloc (o, sizeof(*tag->tagValue));
		tag->tagValue->which = Z_StringOrNumeric_string;
		tag->tagValue->u.string = odr_strdup(o, tag_str);
		
		tag->content = odr_malloc(o, sizeof(*tag->content));
		tag->content->which = Z_ElementData_subtree;
		
		tag->content->u.subtree =
			odr_malloc (o, sizeof(*tag->content->u.subtree));
		tag->content->u.subtree->elements = odr_malloc (o, sizeof(*r->elements));
		tag->content->u.subtree->num_elements = 1;
		
		tag = tag->content->u.subtree->elements[0] =
			odr_malloc (o, sizeof(**tag->content->u.subtree->elements));
		
		tag->tagType = odr_malloc(o, sizeof(*tag->tagType));
		*tag->tagType = 3;
		tag->tagOccurrence = 0;
		tag->metaData = 0;
		tag->appliedVariant = 0;
		tag->tagValue = odr_malloc (o, sizeof(*tag->tagValue));
		tag->tagValue->which = Z_StringOrNumeric_string;
		tag->content = odr_malloc(o, sizeof(*tag->content));
		
		data_length = atoi_n (buf+entry_p, length_data_entry);
		entry_p += length_data_entry;
		data_offset = atoi_n (buf+entry_p, length_starting);
		entry_p += length_starting;
		i = data_offset + base_address;
		end_offset = i+data_length-1;
		
        if (memcmp (tag_str, "00", 2) && indicator_length)
		{
			/* indicator */
			tag->tagValue->u.string = odr_malloc(o, indicator_length+1);
			memcpy (tag->tagValue->u.string, buf + i, indicator_length);
			tag->tagValue->u.string[indicator_length] = '\0';
			i += indicator_length;
			
			tag->content->which = Z_ElementData_subtree;

			tag->content->u.subtree =
				odr_malloc (o, sizeof(*tag->content->u.subtree));
			tag->content->u.subtree->elements =
				odr_malloc (o, 256 * sizeof(*r->elements));
			tag->content->u.subtree->num_elements = 0;
			
			while (buf[i] != ISO2709_RS && buf[i] != ISO2709_FS
				   && i < end_offset)
			{
				int i0;
				/* prepare tag */
                Z_TaggedElement *parent_tag = tag;
				Z_TaggedElement *tag = odr_malloc (o, sizeof(*tag));
				
                if (parent_tag->content->u.subtree->num_elements < 256)
                    parent_tag->content->u.subtree->elements[
						parent_tag->content->u.subtree->num_elements++] = tag;
				
				tag->tagType = odr_malloc(o, sizeof(*tag->tagType));
				*tag->tagType = 3;
				tag->tagOccurrence = 0;
				tag->metaData = 0;
				tag->appliedVariant = 0;
				tag->tagValue = odr_malloc (o, sizeof(*tag->tagValue));
				tag->tagValue->which = Z_StringOrNumeric_string;
				
				/* sub field */
				tag->tagValue->u.string = odr_malloc (o, identifier_length);
				memcpy (tag->tagValue->u.string, buf+i+1, identifier_length-1);
				tag->tagValue->u.string[identifier_length-1] = '\0';
				i += identifier_length;
				
				/* data ... */
				tag->content = odr_malloc(o, sizeof(*tag->content));
				tag->content->which = Z_ElementData_string;
				
				i0 = i;
				while (buf[i] != ISO2709_RS && buf[i] != ISO2709_IDFS &&
					   buf[i] != ISO2709_FS && i < end_offset)
					i++;
				
				tag->content->u.string = odr_malloc (o, i - i0 + 1);
				memcpy (tag->content->u.string, buf + i0, i - i0);
				tag->content->u.string[i - i0] = '\0';
			}
		}
		else
		{
			int i0 = i;
			
			tag->tagValue->u.string = "@";
			tag->content->which = Z_ElementData_string;
			
			while (buf[i] != ISO2709_RS && buf[i] != ISO2709_FS &&
				   i < end_offset)
				i++;
			tag->content->u.string = odr_malloc (o, i - i0 +1);
			memcpy (tag->content->u.string, buf+i0, i - i0);
			tag->content->u.string[i-i0] = '\0';
		}
	}
	return r;
}

static void retval_grs1 (zval *return_value, Z_GenericRecord *p)
{
	Z_GenericRecord *grs[20];
	int eno[20];
	int level = 0;

	if (array_init(return_value) == FAILURE)
	{
		RETURN_FALSE;
	}
	eno[level] = 0;
	grs[level] = p;
	while (level >= 0)
	{
		zval *my_zval;
		Z_TaggedElement *e = 0;
		Z_GenericRecord *p = grs[level];
		int i;
		char tag[256];
		int taglen = 0;

		if (eno[level] >= p->num_elements)
		{
			--level;
			if (level >= 0)
				eno[level]++;
			continue;
		}
		/* eno[level]++; */

		*tag = '\0';
		for (i = 0; i<=level; i++)
		{
			int tag_type = 3;
			e = grs[i]->elements[eno[i]];

			if (e->tagType)
				tag_type = *e->tagType;

			taglen = strlen(tag);
			sprintf (tag+taglen, "(%d,", tag_type);
			taglen = strlen(tag);

			if (e->tagValue->which == Z_StringOrNumeric_string)
			{
				int len = strlen(e->tagValue->u.string);
				memcpy (tag + taglen, e->tagValue->u.string, len);
				tag[taglen+len] = '\0';
			}
			else if (e->tagValue->which == Z_StringOrNumeric_numeric)
			{
				sprintf (tag + taglen, "%d", *e->tagValue->u.numeric);
			}
			taglen = strlen(tag);
			strcpy (tag + taglen, ")");
		}
		ALLOC_ZVAL(my_zval);
		array_init(my_zval);
		INIT_PZVAL(my_zval);
		
		add_next_index_string(my_zval, tag, 1);

		switch (e->content->which)
		{
		case Z_ElementData_string:
			add_next_index_string (my_zval, e->content->u.string, 1);
			break;
		case Z_ElementData_numeric:
			add_next_index_long (my_zval, *e->content->u.numeric);
			break;
		case Z_ElementData_trueOrFalse:
			add_next_index_long (my_zval, *e->content->u.trueOrFalse);
			break;
		case Z_ElementData_subtree:
			level++;
			grs[level] = e->content->u.subtree;
			eno[level] = -1;
		}
		zend_hash_next_index_insert (return_value->value.ht,
									 (void *) &my_zval, sizeof(zval *), NULL);
		eno[level]++;
	}
}


/* {{{ proto string yaz_record(int id, int pos, string type)
   Return record information at given result set position */
PHP_FUNCTION(yaz_record)
{
	pval **pval_id, **pval_pos, **pval_type;
	Yaz_Association p;
	int pos;
	char *type;

	if (ZEND_NUM_ARGS() != 3)
		WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(3, &pval_id, &pval_pos, &pval_type) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	p = get_assoc (pval_id);

	convert_to_long_ex(pval_pos);
	pos = (*pval_pos)->value.lval;

	convert_to_string_ex(pval_type);
	type = (*pval_type)->value.str.val;

	if (p && p->resultSets && p->resultSets->recordList &&
		pos >= p->resultSetStartPoint &&
		pos < p->resultSetStartPoint + p->resultSets->recordList->num_records)
	{
		Z_NamePlusRecord *npr =
			p->resultSets->recordList->records[pos - p->resultSetStartPoint];
		if (npr->which == Z_NamePlusRecord_databaseRecord)
		{
			Z_External *r = (Z_External *) npr->u.databaseRecord;
			oident *ent = oid_getentbyoid(r->direct_reference);
			
			if (!strcmp (type, "syntax"))
			{
				if (ent && ent->desc)
					RETVAL_STRING(ent->desc, 1);
			}
			else if (!strcmp (type, "string"))
			{
				if (r->which == Z_External_sutrs && ent->value == VAL_SUTRS)
				{
					RETVAL_STRINGL(r->u.sutrs->buf, r->u.sutrs->len, 1);
				}
				else if (r->which == Z_External_octet)
				{
					char *buf = (char *) (r->u.octet_aligned->buf);
					int len = r->u.octet_aligned->len;
					
					switch (ent->value)
					{
					case VAL_SOIF:
					case VAL_HTML:
						break;
					case VAL_TEXT_XML:
					case VAL_APPLICATION_XML:
						break;
					default:
						break;
					}
					RETVAL_STRINGL(buf, len, 1);
				}
			}
			else if (!strcmp (type, "array"))
			{
				if (r->which == Z_External_grs1 && ent->value == VAL_GRS1)
				{
					retval_grs1 (return_value, r->u.grs1);
				}
				else if (r->which == Z_External_octet)
				{
					char *buf = (char *) (r->u.octet_aligned->buf);
					ODR odr = odr_createmem (ODR_DECODE);
					Z_GenericRecord *rec = 0;

					switch (ent->value)
					{
					case VAL_SOIF:
					case VAL_HTML:
						break;
					case VAL_TEXT_XML:
					case VAL_APPLICATION_XML:
						/* text2grs1 (&buf, &len, t->odr_in, 0, 0); */
						break;
					default:
						rec = marc_to_grs1 (buf, odr, r->direct_reference);
					}
					if (rec)
						retval_grs1 (return_value, rec);
					odr_destroy (odr);
				}
			}
		}
	}
	release_assoc (p);
}
/* }}} */


/* {{{ proto int yaz_syntax(int id, string syntax)
   Set record syntax for retrieval */
PHP_FUNCTION(yaz_syntax)
{
	pval **pval_id, **pval_syntax;
	Yaz_Association p;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2, &pval_id, &pval_syntax) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	p = get_assoc (pval_id);
	if (p)
	{
		convert_to_string_ex (pval_syntax);
		xfree (p->preferredRecordSyntax);
		p->preferredRecordSyntax = xstrdup ((*pval_syntax)->value.str.val);
	}
	release_assoc (p);
}
/* }}} */

/* {{{ proto int yaz_element(int id, string elementsetname)
   Set Element-Set-Name for retrieval */
PHP_FUNCTION(yaz_element)
{
	pval **pval_id, **pval_element;
	Yaz_Association p;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2, &pval_id, &pval_element) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	p = get_assoc (pval_id);
	if (p)
	{
		convert_to_string_ex (pval_element);
		xfree (p->elementSetNames);
		p->elementSetNames = xstrdup ((*pval_element)->value.str.val);
	}
	release_assoc (p);
}
/* }}} */

/* {{{ proto int yaz_range(int id, int start, int number)
   Set result set start point and number of records to request */

PHP_FUNCTION(yaz_range)
{
	pval **pval_id, **pval_start, **pval_number;
	Yaz_Association p;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3, &pval_id, &pval_start, &pval_number) ==
		FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	p = get_assoc (pval_id);
	if (p)
	{
		convert_to_long_ex (pval_start);
		p->resultSetStartPoint = (*pval_start)->value.lval;
		convert_to_long_ex (pval_number);
		p->numberOfRecordsRequested = (*pval_number)->value.lval;
	}
	release_assoc (p);
}
/* }}} */

PHP_MINIT_FUNCTION(yaz)
{
	int i;
	nmem_init();
#ifdef ZTS
	yaz_mutex = tsrm_mutex_alloc();
#endif
	order_associations = 1;
	shared_associations = xmalloc (sizeof(*shared_associations) * MAX_ASSOC);
	for (i = 0; i<MAX_ASSOC; i++)
		shared_associations[i] = 0;
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(yaz)
{
	int i;

	if (shared_associations)
	{
		for (i = 0; i<MAX_ASSOC; i++)
			yaz_association_destroy (shared_associations[i]);
		xfree (shared_associations);
		shared_associations = 0;
		nmem_exit();
	}
#ifdef ZTS
	tsrm_mutex_free (yaz_mutex);
#endif
	return SUCCESS;
}

PHP_MINFO_FUNCTION(yaz)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "YAZ Support", "enabled");
	php_info_print_table_row(2, "YAZ Version", YAZ_VERSION);
	php_info_print_table_end();
}

PHP_RSHUTDOWN_FUNCTION(yaz)
{
	int i;

#ifdef ZTS
	tsrm_mutex_lock (yaz_mutex);
#endif
	if (shared_associations)
	{
		for (i = 0; i<MAX_ASSOC; i++)
			/* destroy those where password has been used */
			if (shared_associations[i] &&
				(shared_associations[i]->user ||
				 shared_associations[i]->auth_open))
			{
				yaz_association_destroy(shared_associations[i]);
				shared_associations[i] = 0;
			}
	}
#ifdef ZTS
	tsrm_mutex_unlock (yaz_mutex);
#endif
	return SUCCESS;
}

PHP_RINIT_FUNCTION(yaz)
{
#ifdef ZTS
	tsrm_mutex_lock (yaz_mutex);
#endif
	order_associations++;
#ifdef ZTS
	tsrm_mutex_unlock (yaz_mutex);
#endif
	return SUCCESS;
}

zend_module_entry yaz_module_entry = {
	"YAZ",
	yaz_functions,
	PHP_MINIT(yaz),
	PHP_MSHUTDOWN(yaz),
	PHP_RINIT(yaz),
	PHP_RSHUTDOWN(yaz),
	PHP_MINFO(yaz),
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_YAZ
ZEND_GET_MODULE(yaz)
#endif


#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
