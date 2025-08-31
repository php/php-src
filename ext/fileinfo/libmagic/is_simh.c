/*-
 * Copyright (c) 2023 Christos Zoulas
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
 * Parse SIM-H tape files
 * http://simh.trailing-edge.com/docs/simh_magtape.pdf
 */

#ifndef TEST
#include "file.h"

#ifndef lint
FILE_RCSID("@(#)$File: is_simh.c,v 1.10 2023/07/27 19:39:55 christos Exp $")
#endif

#include <string.h>
#include <stddef.h>
#include "magic.h"
#else
#include <stdint.h>
#include <sys/types.h>
#include <string.h>
#include <stddef.h>
#define CAST(a, b) (a)(b)
#endif


#ifdef DEBUG
#include <stdio.h>
#define DPRINTF(fmt, ...) printf(fmt, __VA_ARGS__)
#else
#define DPRINTF(fmt, ...)
#endif

/*
 * if SIMH_TAPEMARKS == 0:
 *	check all the records and tapemarks
 * otherwise:
 *	check only up-to the number of tapemarks specified
 */
#ifndef SIMH_TAPEMARKS
#define SIMH_TAPEMARKS 10
#endif

typedef union {
	char s[4];
	uint32_t u;
} myword; 

static myword simh_bo;

#define NEED_SWAP	(simh_bo.u == CAST(uint32_t, 0x01020304))

/*
 * swap an int
 */
static uint32_t
swap4(uint32_t sv)
{
	myword d, s;
	s.u = sv;
	d.s[0] = s.s[3];
	d.s[1] = s.s[2];
	d.s[2] = s.s[1];
	d.s[3] = s.s[0];
	return d.u;
}


static uint32_t
getlen(const unsigned char **uc)
{
	uint32_t n;
	memcpy(&n, *uc, sizeof(n));
	*uc += sizeof(n);
	if (NEED_SWAP)
		n = swap4(n);
	if (n == 0xffffffff)	/* check for End of Medium */
		return n;
	n &= 0x00ffffff;	/* keep only the record len */
	if (n & 1)
		n++;
	return n;
}

static int
simh_parse(const unsigned char *uc, const unsigned char *ue)
{
	uint32_t nbytes, cbytes;
	const unsigned char *orig_uc = uc;
	size_t nt = 0, nr = 0;

	(void)memcpy(simh_bo.s, "\01\02\03\04", 4);

	while (ue - uc >= CAST(ptrdiff_t, sizeof(nbytes))) {
		nbytes = getlen(&uc);
		if ((nt > 0 || nr > 0) && nbytes == 0xFFFFFFFF)
			/* EOM after at least one record or tapemark */
			break;
		if (nbytes == 0) {
			nt++;	/* count tapemarks */
#if SIMH_TAPEMARKS
			if (nt == SIMH_TAPEMARKS)
				break;
#endif
			continue;
		}
		/* handle a data record */
		uc += nbytes;
		if (ue - uc < CAST(ptrdiff_t, sizeof(nbytes)))
			break;
		cbytes = getlen(&uc);
		if (nbytes != cbytes)
			return 0;
		nr++;
	}
	if (nt * sizeof(uint32_t) == CAST(size_t, uc - orig_uc))
		return 0;	/* All examined data was tapemarks (0) */
	if (nr == 0)		/* No records */
		return 0;
	return 1;
}

#ifndef TEST
int
file_is_simh(struct magic_set *ms, const struct buffer *b)
{
	const unsigned char *uc = CAST(const unsigned char *, b->fbuf);
	const unsigned char *ue = uc + b->flen;
	int mime = ms->flags & MAGIC_MIME;

	if ((ms->flags & (MAGIC_APPLE|MAGIC_EXTENSION)) != 0)
		return 0;

	if (!simh_parse(uc, ue))
		return 0;

	if (mime == MAGIC_MIME_ENCODING)
		return 1;

	if (mime) {
		if (file_printf(ms, "application/SIMH-tape-data") == -1)
			return -1;
		return 1;
	}

	if (file_printf(ms, "SIMH tape data") == -1)
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
	int fd;
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
	printf("is simh %d\n", simh_parse(p, p + st.st_size));
	return 0;
}
#endif
