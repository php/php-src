/*-
 * Copyright (c) 2019 Christos Zoulas
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Parse CSV object serialization format (RFC-4180, RFC-7111)
 */

#ifndef TEST
#include "file.h"

#ifndef lint
FILE_RCSID("@(#)$File: is_csv.c,v 1.7 2022/05/28 00:44:22 christos Exp $")
#endif

#include <string.h>
#include "magic.h"
#else
#include <sys/types.h>
#endif


#ifdef DEBUG
#include <stdio.h>
#define DPRINTF(fmt, ...) printf(fmt, __VA_ARGS__)
#else
#define DPRINTF(fmt, ...)
#endif

/*
 * if CSV_LINES == 0:
 *	check all the lines in the buffer
 * otherwise:
 *	check only up-to the number of lines specified
 *
 * the last line count is always ignored if it does not end in CRLF
 */
#ifndef CSV_LINES
#define CSV_LINES 10
#endif

static int csv_parse(const unsigned char *, const unsigned char *);

static const unsigned char *
eatquote(const unsigned char *uc, const unsigned char *ue)
{
	int quote = 0;

	while (uc < ue) {
		unsigned char c = *uc++;
		if (c != '"') {
			// We already got one, done.
			if (quote) {
				return --uc;
			}
			continue;
		}
		if (quote) {
			// quote-quote escapes
			quote = 0;
			continue;
		}
		// first quote
		quote = 1;
	}
	return ue;
}

static int
csv_parse(const unsigned char *uc, const unsigned char *ue)
{
	size_t nf = 0, tf = 0, nl = 0;

	while (uc < ue) {
		switch (*uc++) {
		case '"':
			// Eat until the matching quote
			uc = eatquote(uc, ue);
			break;
		case ',':
			nf++;
			break;
		case '\n':
			DPRINTF("%zu %zu %zu\n", nl, nf, tf);
			nl++;
#if CSV_LINES
			if (nl == CSV_LINES)
				return tf != 0 && tf == nf;
#endif
			if (tf == 0) {
				// First time and no fields, give up
				if (nf == 0) 
					return 0;
				// First time, set the number of fields
				tf = nf;
			} else if (tf != nf) {
				// Field number mismatch, we are done.
				return 0;
			}
			nf = 0;
			break;
		default:
			break;
		}
	}
	return tf && nl > 2;
}

#ifndef TEST
int
file_is_csv(struct magic_set *ms, const struct buffer *b, int looks_text)
{
	const unsigned char *uc = CAST(const unsigned char *, b->fbuf);
	const unsigned char *ue = uc + b->flen;
	int mime = ms->flags & MAGIC_MIME;

	if (!looks_text)
		return 0;

	if ((ms->flags & (MAGIC_APPLE|MAGIC_EXTENSION)) != 0)
		return 0;

	if (!csv_parse(uc, ue))
		return 0;

	if (mime == MAGIC_MIME_ENCODING)
		return 1;

	if (mime) {
		if (file_printf(ms, "text/csv") == -1)
			return -1;
		return 1;
	}

	if (file_printf(ms, "CSV text") == -1)
		return -1;

	return 1;
}

#else

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <err.h>

int
main(int argc, char *argv[])
{
	int fd, rv;
	struct stat st;
	unsigned char *p;

	if ((fd = open(argv[1], O_RDONLY)) == -1)
		err(EXIT_FAILURE, "Can't open `%s'", argv[1]);

	if (fstat(fd, &st) == -1)
		err(EXIT_FAILURE, "Can't stat `%s'", argv[1]);

	if ((p = CAST(char *, malloc(st.st_size))) == NULL)
		err(EXIT_FAILURE, "Can't allocate %jd bytes",
		    (intmax_t)st.st_size);
	if (read(fd, p, st.st_size) != st.st_size)
		err(EXIT_FAILURE, "Can't read %jd bytes",
		    (intmax_t)st.st_size);
	printf("is csv %d\n", csv_parse(p, p + st.st_size));
	return 0;
}
#endif
