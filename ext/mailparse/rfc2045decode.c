/* $Id$ */
/*
** Copyright 1998 - 1999 Double Precision, Inc.  See COPYING for
** distribution information.
*/

#include "php.h"
#include	"php_mailparse.h"

static void decode(struct rfc2045id *topid,
		struct rfc2045id **childidptr,
		struct rfc2045 *r,
		void (*func)(struct rfc2045 *, struct rfc2045id *, void *),
		void *ptr)
{
struct rfc2045id nextid;

	*childidptr=0;
	(*func)(r, topid, ptr);
	*childidptr=&nextid;
	nextid.idnum=1;
	if (r->content_type && strncmp(r->content_type, "multipart/", 10) == 0)
		nextid.idnum=0;
	for (r=r->firstpart; r; r=r->next)
	{
		if (nextid.idnum)
			decode(topid, &nextid.next, r, func, ptr);
		++nextid.idnum;
	}
}

void rfc2045_decode(struct rfc2045 *p,
	void (*func)(struct rfc2045 *, struct rfc2045id *, void *),
	void *ptr)
{
struct rfc2045id topid;

	topid.idnum=1;
	decode(&topid, &topid.next, p, func, ptr);
}
