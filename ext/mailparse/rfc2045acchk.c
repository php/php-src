/* $Id$ */
/*
** Copyright 1998 - 1999 Double Precision, Inc.  See COPYING for
** distribution information.
*/
#include "php.h"
#include "php_mailparse.h"


int rfc2045_ac_check(struct rfc2045 *p, int rwmode)
{
	int	flag=0;		/* Flag - rewriting suggested */
	struct	rfc2045 *c;
	int	hasnon7bit=p->has8bitchars;
			/* hasnon7bit: 8bit chars in this section or subsections */
	const char *te;
	int	is8bitte;
	TSRMLS_FETCH();

	for (c=p->firstpart; c; c=c->next)
		if (!c->isdummy)
		{
			if (rfc2045_ac_check(c, rwmode))	flag=1;
			if (strcmp(c->content_transfer_encoding, "7bit") &&
				strcmp(c->content_transfer_encoding, "quoted-printable"))
				hasnon7bit=1;
			if (c->has8bitchars)
				p->has8bitchars=1;
		}

	if (RFC2045_ISMIME1DEF(p->mime_version) && !p->content_type)
	{
		p->content_type = estrdup("text/plain");
		if (p->mime_version)
		{
			flag=1;
		}
	}

	if (RFC2045_ISMIME1DEF(p->mime_version)
		&& !rfc2045_getattr(p->content_type_attr, "charset")
		&& strncasecmp(p->content_type, "text/", 5) == 0)
	{
		rfc2045_setattr(&p->content_type_attr, "charset",
			MAILPARSEG(def_charset));

		if (p->mime_version

			&& p->firstpart == 0 /* sam - don't trigger rewrites on changes to multipart headers */

			)
		{
			flag=1;
		}
	}

	if (RFC2045_ISMIME1DEF(p->mime_version)
		&& !p->content_transfer_encoding)
	{
		p->content_transfer_encoding = estrdup(hasnon7bit ? "8bit":"7bit");
		if (p->mime_version

			&& p->firstpart == 0 /* sam - don't trigger rewrites on changes to multipart headers */
			)
		{
			flag=1;
		}
	}

#if 0
	if (RFC2045_ISMIME1DEF(p->mime_version)
		&& strncmp(p->content_type, "text/", 5) == 0 && !hasnon7bit
		&& strcmp(p->content_transfer_encoding, "7bit"))
	{
		if (p->mime_version)
		{
			flag=1;
		}
	}
#endif

	if (RFC2045_ISMIME1DEF(p->mime_version))
	{
		/* Check for conversions */

		te=p->content_transfer_encoding;
		is8bitte=strcasecmp(te, "base64") &&
			strcasecmp(te, "quoted-printable") &&
			strcasecmp(te, "7bit");	/* 8 bit contents */

		if (is8bitte && !p->has8bitchars && !p->haslongline)
		{
			if (p->rw_transfer_encoding)
				efree(p->rw_transfer_encoding);
			p->rw_transfer_encoding=estrdup("7bit");
			flag=1;
			is8bitte=0;
		}

		if (rwmode == RFC2045_RW_7BIT && (is8bitte || p->haslongline))
		{
			if (p->rw_transfer_encoding)
				efree(p->rw_transfer_encoding);
			p->rw_transfer_encoding=estrdup("quoted-printable");
			flag=1;
		}
		else if (rwmode == RFC2045_RW_8BIT &&
			strcasecmp(te, "quoted-printable") == 0 &&
			!p->haslongline)
		{
			if (p->rw_transfer_encoding)
				efree(p->rw_transfer_encoding);
			p->rw_transfer_encoding=estrdup(hasnon7bit ? "8bit":"7bit");
			flag=1;
		}
	}

	if (!p->mime_version)
	{
		p->mime_version = estrdup("1.0");
	}
	return (flag);
}
