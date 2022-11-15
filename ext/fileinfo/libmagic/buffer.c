/*
 * Copyright (c) Christos Zoulas 2017.
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice immediately at the beginning of the file, without modification,
 *    this list of conditions, and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include "file.h"

#ifndef	lint
FILE_RCSID("@(#)$File: buffer.c,v 1.8 2020/02/16 15:52:49 christos Exp $")
#endif	/* lint */

#include "magic.h"
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

void
buffer_init(struct buffer *b, int fd, const zend_stat_t *st, const void *data,
    size_t len)
{
	b->fd = fd;
	if (st)
		memcpy(&b->st, st, sizeof(b->st));
	else if (b->fd == -1 || zend_fstat(b->fd, &b->st) == -1)
		memset(&b->st, 0, sizeof(b->st));
	b->fbuf = data;
	b->flen = len;
	b->eoff = 0;
	b->ebuf = NULL;
	b->elen = 0;
}

void
buffer_fini(struct buffer *b)
{
	efree(b->ebuf);
}

int
buffer_fill(const struct buffer *bb)
{
	struct buffer *b = CCAST(struct buffer *, bb);

	if (b->elen != 0)
		return b->elen == FILE_BADSIZE ? -1 : 0;

	if (!S_ISREG(b->st.st_mode))
		goto out;

	b->elen =  CAST(size_t, b->st.st_size) < b->flen ?
	    CAST(size_t, b->st.st_size) : b->flen;
	if ((b->ebuf = emalloc(b->elen)) == NULL)
		goto out;

	b->eoff = b->st.st_size - b->elen;
	if (FINFO_LSEEK_FUNC(b->fd, b->eoff, SEEK_SET) == (zend_off_t)-1 ||
		FINFO_READ_FUNC(b->fd, b->ebuf, b->elen) != (ssize_t)b->elen)
	{
		efree(b->ebuf);
		b->ebuf = NULL;
		goto out;
	}

	return 0;
out:
	b->elen = FILE_BADSIZE;
	return -1;
}
