/* $Id$ */
/*
** Copyright 1998 - 1999 Double Precision, Inc.  See COPYING for
** distribution information.
*/

#include "php.h"
#include	"php_mailparse.h"

struct rfc2045findstruct {
	const char *partnum;
	struct rfc2045 *ptr;
} ;

static void do_decode(struct rfc2045 *p, struct rfc2045id *id, void *ptr)
{
	struct rfc2045findstruct *fs=(struct rfc2045findstruct *)ptr;
	const char *partnum=fs->partnum;
	unsigned n;

	while (id)
	{
		if (!isdigit((int)(unsigned char)*partnum))	return;
		n=0;
		while (isdigit((int)(unsigned char)*partnum))
			n=n*10 + *partnum++ - '0';
		if (*partnum)
		{
			if (*partnum != '.')	return;
			++partnum;
		}
		if (n != (unsigned)id->idnum)	return;
		id=id->next;
	}
	if ( *partnum == '\0')	fs->ptr=p;
}


struct rfc2045 *rfc2045_find(struct rfc2045 *p, const char *str)
{
	struct	rfc2045findstruct fs;

	fs.partnum=str;
	fs.ptr=0;
	rfc2045_decode(p, &do_decode, &fs);
	return (fs.ptr);
}
