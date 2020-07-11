/*
 * Copyright (c) Ian F. Darwin 1986-1995.
 * Software written by Ian F. Darwin and others;
 * maintained 1995-present by Christos Zoulas and others.
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
/*
 * ASCII magic -- try to detect text encoding.
 *
 * Extensively modified by Eric Fischer <enf@pobox.com> in July, 2000,
 * to handle character codes other than ASCII on a unified basis.
 */

#include "file.h"

#ifndef	lint
FILE_RCSID("@(#)$File: ascmagic.c,v 1.104 2019/05/07 02:27:11 christos Exp $")
#endif	/* lint */

#include "magic.h"
#include <string.h>
#include <memory.h>
#include <ctype.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#define MAXLINELEN 300	/* longest sane line length */
#define ISSPC(x) ((x) == ' ' || (x) == '\t' || (x) == '\r' || (x) == '\n' \
		  || (x) == 0x85 || (x) == '\f')

private unsigned char *encode_utf8(unsigned char *, size_t, unichar *, size_t);
private size_t trim_nuls(const unsigned char *, size_t);

/*
 * Undo the NUL-termination kindly provided by process()
 * but leave at least one byte to look at
 */
private size_t
trim_nuls(const unsigned char *buf, size_t nbytes)
{
	while (nbytes > 1 && buf[nbytes - 1] == '\0')
		nbytes--;

	return nbytes;
}

protected int
file_ascmagic(struct magic_set *ms, const struct buffer *b, int text)
{
	unichar *ubuf = NULL;
	size_t ulen = 0;
	int rv = 1;
	struct buffer bb;

	const char *code = NULL;
	const char *code_mime = NULL;
	const char *type = NULL;

	bb = *b;
	bb.flen = trim_nuls(CAST(const unsigned char *, b->fbuf), b->flen);
	/*
	 * Avoid trimming at an odd byte if the original buffer was evenly
	 * sized; this avoids losing the last character on UTF-16 LE text
	 */
	if ((bb.flen & 1) && !(b->flen & 1))
		bb.flen++;

	/* If file doesn't look like any sort of text, give up. */
	if (file_encoding(ms, &bb, &ubuf, &ulen, &code, &code_mime,
	    &type) == 0)
		rv = 0;
        else
		rv = file_ascmagic_with_encoding(ms, &bb,
		    ubuf, ulen, code, type, text);

	efree(ubuf);

	return rv;
}

protected int
file_ascmagic_with_encoding(struct magic_set *ms,
    const struct buffer *b, unichar *ubuf, size_t ulen, const char *code,
    const char *type, int text)
{
	struct buffer bb;
	const unsigned char *buf = CAST(const unsigned char *, b->fbuf);
	size_t nbytes = b->flen;
	unsigned char *utf8_buf = NULL, *utf8_end;
	size_t mlen, i, len;
	int rv = -1;
	int mime = ms->flags & MAGIC_MIME;
	int need_separator = 0;

	const char *subtype = NULL;
	const char *subtype_mime = NULL;

	int has_escapes = 0;
	int has_backspace = 0;
	int seen_cr = 0;

	int n_crlf = 0;
	int n_lf = 0;
	int n_cr = 0;
	int n_nel = 0;
	int executable = 0;

	size_t last_line_end = CAST(size_t, -1);
	int has_long_lines = 0;

	nbytes = trim_nuls(buf, nbytes);

	/* If we have fewer than 2 bytes, give up. */
	if (nbytes <= 1) {
		rv = 0;
		goto done;
	}

	if (ulen > 0 && (ms->flags & MAGIC_NO_CHECK_SOFT) == 0) {
		/* Convert ubuf to UTF-8 and try text soft magic */
		/* malloc size is a conservative overestimate; could be
		   improved, or at least realloced after conversion. */
		mlen = ulen * 6;
		if ((utf8_buf = CAST(unsigned char *, emalloc(mlen))) == NULL) {
			file_oomem(ms, mlen);
			goto done;
		}
		if ((utf8_end = encode_utf8(utf8_buf, mlen, ubuf, ulen))
		    == NULL)
			goto done;
		buffer_init(&bb, b->fd, &b->st, utf8_buf,
		    CAST(size_t, utf8_end - utf8_buf));

		if ((rv = file_softmagic(ms, &bb, NULL, NULL,
		    TEXTTEST, text)) == 0)
			rv = -1;
		else
			need_separator = 1;
		buffer_fini(&bb);
		if ((ms->flags & (MAGIC_APPLE|MAGIC_EXTENSION))) {
			rv = rv == -1 ? 0 : 1;
			goto done;
		}
	}
	if ((ms->flags & (MAGIC_APPLE|MAGIC_EXTENSION)))
		return 0;

	/* Now try to discover other details about the file. */
	for (i = 0; i < ulen; i++) {
		if (ubuf[i] == '\n') {
			if (seen_cr)
				n_crlf++;
			else
				n_lf++;
			last_line_end = i;
		} else if (seen_cr)
			n_cr++;

		seen_cr = (ubuf[i] == '\r');
		if (seen_cr)
			last_line_end = i;

		if (ubuf[i] == 0x85) { /* X3.64/ECMA-43 "next line" character */
			n_nel++;
			last_line_end = i;
		}

		/* If this line is _longer_ than MAXLINELEN, remember it. */
		if (i > last_line_end + MAXLINELEN)
			has_long_lines = 1;

		if (ubuf[i] == '\033')
			has_escapes = 1;
		if (ubuf[i] == '\b')
			has_backspace = 1;
	}

	/* Beware, if the data has been truncated, the final CR could have
	   been followed by a LF.  If we have ms->bytes_max bytes, it indicates
	   that the data might have been truncated, probably even before
	   this function was called. */
	if (seen_cr && nbytes < ms->bytes_max)
		n_cr++;

	if (strcmp(type, "binary") == 0) {
		rv = 0;
		goto done;
	}
	len = file_printedlen(ms);
	if (mime) {
		if ((mime & MAGIC_MIME_TYPE) != 0) {
			if (len) {
				/*
				 * Softmagic printed something, we
				 * are either done, or we need a separator
				 */
				if ((ms->flags & MAGIC_CONTINUE) == 0) {
					rv = 1;
					goto done;
				}
				if (need_separator && file_separator(ms) == -1)
					goto done;
			}
			if (subtype_mime) {
				if (file_printf(ms, "%s", subtype_mime) == -1)
					goto done;
			} else {
				if (file_printf(ms, "text/plain") == -1)
					goto done;
			}
		}
	} else {
		if (len) {
			switch (file_replace(ms, " text$", ", ")) {
			case 0:
				switch (file_replace(ms, " text executable$",
				    ", ")) {
				case 0:
					if (file_printf(ms, ", ") == -1)
						goto done;
					break;
				case -1:
					goto done;
				default:
					executable = 1;
					break;
				}
				break;
			case -1:
				goto done;
			default:
				break;
			}
		}

		if (file_printf(ms, "%s", code) == -1)
			goto done;

		if (subtype) {
			if (file_printf(ms, " %s", subtype) == -1)
				goto done;
		}

		if (file_printf(ms, " %s", type) == -1)
			goto done;

		if (executable)
			if (file_printf(ms, " executable") == -1)
				goto done;

		if (has_long_lines)
			if (file_printf(ms, ", with very long lines") == -1)
				goto done;

		/*
		 * Only report line terminators if we find one other than LF,
		 * or if we find none at all.
		 */
		if ((n_crlf == 0 && n_cr == 0 && n_nel == 0 && n_lf == 0) ||
		    (n_crlf != 0 || n_cr != 0 || n_nel != 0)) {
			if (file_printf(ms, ", with") == -1)
				goto done;

			if (n_crlf == 0 && n_cr == 0 && n_nel == 0 && n_lf == 0) {
				if (file_printf(ms, " no") == -1)
					goto done;
			} else {
				if (n_crlf) {
					if (file_printf(ms, " CRLF") == -1)
						goto done;
					if (n_cr || n_lf || n_nel)
						if (file_printf(ms, ",") == -1)
							goto done;
				}
				if (n_cr) {
					if (file_printf(ms, " CR") == -1)
						goto done;
					if (n_lf || n_nel)
						if (file_printf(ms, ",") == -1)
							goto done;
				}
				if (n_lf) {
					if (file_printf(ms, " LF") == -1)
						goto done;
					if (n_nel)
						if (file_printf(ms, ",") == -1)
							goto done;
				}
				if (n_nel)
					if (file_printf(ms, " NEL") == -1)
						goto done;
			}

			if (file_printf(ms, " line terminators") == -1)
				goto done;
		}

		if (has_escapes)
			if (file_printf(ms, ", with escape sequences") == -1)
				goto done;
		if (has_backspace)
			if (file_printf(ms, ", with overstriking") == -1)
				goto done;
	}
	rv = 1;
done:
	if (utf8_buf)
		efree(utf8_buf);

	return rv;
}

/*
 * Encode Unicode string as UTF-8, returning pointer to character
 * after end of string, or NULL if an invalid character is found.
 */
private unsigned char *
encode_utf8(unsigned char *buf, size_t len, unichar *ubuf, size_t ulen)
{
	size_t i;
	unsigned char *end = buf + len;

	for (i = 0; i < ulen; i++) {
		if (ubuf[i] <= 0x7f) {
			if (end - buf < 1)
				return NULL;
			*buf++ = CAST(unsigned char, ubuf[i]);
		} else if (ubuf[i] <= 0x7ff) {
			if (end - buf < 2)
				return NULL;
			*buf++ = CAST(unsigned char, (ubuf[i] >> 6) + 0xc0);
			*buf++ = CAST(unsigned char, (ubuf[i] & 0x3f) + 0x80);
		} else if (ubuf[i] <= 0xffff) {
			if (end - buf < 3)
				return NULL;
			*buf++ = CAST(unsigned char, (ubuf[i] >> 12) + 0xe0);
			*buf++ = CAST(unsigned char, ((ubuf[i] >> 6) & 0x3f) + 0x80);
			*buf++ = CAST(unsigned char, (ubuf[i] & 0x3f) + 0x80);
		} else if (ubuf[i] <= 0x1fffff) {
			if (end - buf < 4)
				return NULL;
			*buf++ = CAST(unsigned char, (ubuf[i] >> 18) + 0xf0);
			*buf++ = CAST(unsigned char, ((ubuf[i] >> 12) & 0x3f) + 0x80);
			*buf++ = CAST(unsigned char, ((ubuf[i] >>  6) & 0x3f) + 0x80);
			*buf++ = CAST(unsigned char, (ubuf[i] & 0x3f) + 0x80);
		} else if (ubuf[i] <= 0x3ffffff) {
			if (end - buf < 5)
				return NULL;
			*buf++ = CAST(unsigned char, (ubuf[i] >> 24) + 0xf8);
			*buf++ = CAST(unsigned char, ((ubuf[i] >> 18) & 0x3f) + 0x80);
			*buf++ = CAST(unsigned char, ((ubuf[i] >> 12) & 0x3f) + 0x80);
			*buf++ = CAST(unsigned char, ((ubuf[i] >>  6) & 0x3f) + 0x80);
			*buf++ = CAST(unsigned char, (ubuf[i] & 0x3f) + 0x80);
		} else if (ubuf[i] <= 0x7fffffff) {
			if (end - buf < 6)
				return NULL;
			*buf++ = CAST(unsigned char, (ubuf[i] >> 30) + 0xfc);
			*buf++ = CAST(unsigned char, ((ubuf[i] >> 24) & 0x3f) + 0x80);
			*buf++ = CAST(unsigned char, ((ubuf[i] >> 18) & 0x3f) + 0x80);
			*buf++ = CAST(unsigned char, ((ubuf[i] >> 12) & 0x3f) + 0x80);
			*buf++ = CAST(unsigned char, ((ubuf[i] >>  6) & 0x3f) + 0x80);
			*buf++ = CAST(unsigned char, (ubuf[i] & 0x3f) + 0x80);
		} else /* Invalid character */
			return NULL;
	}

	return buf;
}
