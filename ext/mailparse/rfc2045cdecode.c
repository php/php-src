/* $Id$ */
/*
 ** Copyright 1998 - 1999 Double Precision, Inc.  See COPYING for
 ** distribution information.
 */

#include	"php.h"
#include "php_mailparse.h"


static int op_func(int c, void *dat)
{
	struct rfc2045 * p = (struct rfc2045*)dat;

	rfc2045_add_workbufch(p, c);

	/* drain buffer */
	if (p->workbuflen >= 4096)	{
		(*p->udecode_func)(p->workbuf, p->workbuflen, p->misc_decode_ptr);
		p->workbuflen = 0;
	}
	
	return c;
}

void rfc2045_cdecode_start(struct rfc2045 *p,
			rfc2045_decode_user_func_t u,
			void *miscptr)
{
	enum mbfl_no_encoding from = mbfl_no_encoding_8bit;
	
	if (p->content_transfer_encoding)
	{
		from = mbfl_name2no_encoding(p->content_transfer_encoding);
		if (from == mbfl_no_encoding_invalid)	{
			zend_error(E_WARNING, "%s(): I don't know how to decode %s transfer encoding!",
					get_active_function_name(),
					p->content_transfer_encoding);
			from = mbfl_no_encoding_8bit;
		}
	}
	
	p->misc_decode_ptr=miscptr;
	p->udecode_func=u;
	p->workbuflen=0;

	if (from == mbfl_no_encoding_8bit)
		p->decode_filter = NULL;
	else
		p->decode_filter = mbfl_convert_filter_new(
			from, mbfl_no_encoding_8bit,
			op_func,
			NULL,
			p
			);
}

int rfc2045_cdecode_end(struct rfc2045 *p)
{
	if (p->decode_filter)
	{
		mbfl_convert_filter_flush(p->decode_filter);
		mbfl_convert_filter_delete(p->decode_filter);
		p->decode_filter = NULL;
		if (p->workbuflen > 0)
			(*p->udecode_func)(p->workbuf, p->workbuflen, p->misc_decode_ptr);
	}
	return 0;
}

int rfc2045_cdecode(struct rfc2045 *p, const char *s, size_t l)
{
	if (s && l)
	{
		int i;

		if (p->decode_filter)
		{
			for (i=0; i<l; i++)
			{
				if (mbfl_convert_filter_feed(s[i], p->decode_filter) < 0)
					return -1;
			}
		}
		else
			return ((*p->udecode_func)(s,l,p->misc_decode_ptr));

	}
	return (0);
}
