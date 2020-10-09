/*
 * Copyright (c) Christos Zoulas 2003.
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
#ifndef _MAGIC_H
#define _MAGIC_H

#include <sys/types.h>

#define	MAGIC_NONE		0x0000000 /* No flags */
#define	MAGIC_DEBUG		0x0000001 /* Turn on debugging */
#define	MAGIC_SYMLINK		0x0000002 /* Follow symlinks */
#define	MAGIC_COMPRESS		0x0000004 /* Check inside compressed files */
#define	MAGIC_DEVICES		0x0000008 /* Look at the contents of devices */
#define	MAGIC_MIME_TYPE		0x0000010 /* Return the MIME type */
#define	MAGIC_CONTINUE		0x0000020 /* Return all matches */
#define	MAGIC_CHECK		0x0000040 /* Print warnings to stderr */
#define	MAGIC_PRESERVE_ATIME	0x0000080 /* Restore access time on exit */
#define	MAGIC_RAW		0x0000100 /* Don't convert unprintable chars */
#define	MAGIC_ERROR		0x0000200 /* Handle ENOENT etc as real errors */
#define	MAGIC_MIME_ENCODING	0x0000400 /* Return the MIME encoding */
#define MAGIC_MIME		(MAGIC_MIME_TYPE|MAGIC_MIME_ENCODING)
#define	MAGIC_APPLE		0x0000800 /* Return the Apple creator/type */
#define	MAGIC_EXTENSION		0x1000000 /* Return a /-separated list of
					   * extensions */
#define MAGIC_COMPRESS_TRANSP	0x2000000 /* Check inside compressed files
					   * but not report compression */
#define MAGIC_NODESC		(MAGIC_EXTENSION|MAGIC_MIME|MAGIC_APPLE)

#define	MAGIC_NO_CHECK_COMPRESS	0x0001000 /* Don't check for compressed files */
#define	MAGIC_NO_CHECK_TAR	0x0002000 /* Don't check for tar files */
#define	MAGIC_NO_CHECK_SOFT	0x0004000 /* Don't check magic entries */
#define	MAGIC_NO_CHECK_APPTYPE	0x0008000 /* Don't check application type */
#define	MAGIC_NO_CHECK_ELF	0x0010000 /* Don't check for elf details */
#define	MAGIC_NO_CHECK_TEXT	0x0020000 /* Don't check for text files */
#define	MAGIC_NO_CHECK_CDF	0x0040000 /* Don't check for cdf files */
#define MAGIC_NO_CHECK_CSV	0x0080000 /* Don't check for CSV files */
#define	MAGIC_NO_CHECK_TOKENS	0x0100000 /* Don't check tokens */
#define MAGIC_NO_CHECK_ENCODING 0x0200000 /* Don't check text encodings */
#define MAGIC_NO_CHECK_JSON	0x0400000 /* Don't check for JSON files */

/* No built-in tests; only consult the magic file */
#define MAGIC_NO_CHECK_BUILTIN	( \
	MAGIC_NO_CHECK_COMPRESS	| \
	MAGIC_NO_CHECK_TAR	| \
/*	MAGIC_NO_CHECK_SOFT	| */ \
	MAGIC_NO_CHECK_APPTYPE	| \
	MAGIC_NO_CHECK_ELF	| \
	MAGIC_NO_CHECK_TEXT	| \
	MAGIC_NO_CHECK_CSV	| \
	MAGIC_NO_CHECK_CDF	| \
	MAGIC_NO_CHECK_TOKENS	| \
	MAGIC_NO_CHECK_ENCODING	| \
	MAGIC_NO_CHECK_JSON	| \
	0			  \
)

#define MAGIC_SNPRINTB "\177\020\
b\0debug\0\
b\1symlink\0\
b\2compress\0\
b\3devices\0\
b\4mime_type\0\
b\5continue\0\
b\6check\0\
b\7preserve_atime\0\
b\10raw\0\
b\11error\0\
b\12mime_encoding\0\
b\13apple\0\
b\14no_check_compress\0\
b\15no_check_tar\0\
b\16no_check_soft\0\
b\17no_check_sapptype\0\
b\20no_check_elf\0\
b\21no_check_text\0\
b\22no_check_cdf\0\
b\23no_check_reserved0\0\
b\24no_check_tokens\0\
b\25no_check_encoding\0\
b\26no_check_json\0\
b\27no_check_reserved2\0\
b\30extension\0\
b\31transp_compression\0\
"

/* Defined for backwards compatibility (renamed) */
#define	MAGIC_NO_CHECK_ASCII	MAGIC_NO_CHECK_TEXT

/* Defined for backwards compatibility; do nothing */
#define	MAGIC_NO_CHECK_FORTRAN	0x000000 /* Don't check ascii/fortran */
#define	MAGIC_NO_CHECK_TROFF	0x000000 /* Don't check ascii/troff */

#define MAGIC_VERSION		539	/* This implementation */


#ifdef __cplusplus
extern "C" {
#endif

typedef struct magic_set *magic_t;
magic_t magic_open(int);
void magic_close(magic_t);

const char *magic_getpath(const char *, int);
const char *magic_file(magic_t, const char *);
const char *magic_stream(magic_t, php_stream *);
const char *magic_descriptor(magic_t, int);
const char *magic_buffer(magic_t, const void *, size_t);

const char *magic_error(magic_t);
int magic_getflags(magic_t);
int magic_setflags(magic_t, int);

int magic_version(void);
int magic_load(magic_t, const char *);
int magic_load_buffers(magic_t, void **, size_t *, size_t);

int magic_compile(magic_t, const char *);
int magic_check(magic_t, const char *);
int magic_list(magic_t, const char *);
int magic_errno(magic_t);

#define MAGIC_PARAM_INDIR_MAX		0
#define MAGIC_PARAM_NAME_MAX		1
#define MAGIC_PARAM_ELF_PHNUM_MAX	2
#define MAGIC_PARAM_ELF_SHNUM_MAX	3
#define MAGIC_PARAM_ELF_NOTES_MAX	4
#define MAGIC_PARAM_REGEX_MAX		5
#define	MAGIC_PARAM_BYTES_MAX		6

int magic_setparam(magic_t, int, const void *);
int magic_getparam(magic_t, int, void *);

#ifdef __cplusplus
};
#endif

#endif /* _MAGIC_H */
