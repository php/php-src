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
 * file.h - definitions for file(1) program
 * @(#)$File: file.h,v 1.248 2023/07/28 14:38:25 christos Exp $
 */

#ifndef __file_h__
#define __file_h__

#include "config.h"

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#ifdef _WIN32
# ifdef PRIu32
#  ifdef _WIN64
#   define SIZE_T_FORMAT PRIu64
#  else
#   define SIZE_T_FORMAT PRIu32
#  endif
#  define INT64_T_FORMAT PRIi64
#  define INTMAX_T_FORMAT PRIiMAX
# else
#  ifdef _WIN64
#   define SIZE_T_FORMAT "I64"
#  else
#   define SIZE_T_FORMAT ""
#  endif
#  define INT64_T_FORMAT "I64"
#  define INTMAX_T_FORMAT "I64"
# endif
#else
# define SIZE_T_FORMAT "z"
# define INT64_T_FORMAT "ll"
# define INTMAX_T_FORMAT "j"
#endif

#include <stdio.h>	/* Include that here, to make sure __P gets defined */
#include <errno.h>
#include <fcntl.h>	/* For open and flags */

#include <sys/types.h>
#ifndef WIN32
#include <sys/param.h>
#endif
/* Do this here and now, because struct stat gets re-defined on solaris */
#include <sys/stat.h>
#include <stdarg.h>

#define ENABLE_CONDITIONALS

#ifndef MAGIC
#define MAGIC "/etc/magic"
#endif

#if defined(__EMX__) || defined (WIN32)
#define PATHSEP	';'
#else
#define PATHSEP	':'
#endif

#define file_private static

#if HAVE_VISIBILITY && !defined(WIN32)
#define file_public  __attribute__ ((__visibility__("default")))
#ifndef file_protected
#define file_protected __attribute__ ((__visibility__("hidden")))
#endif
#else
#define file_public
#ifndef file_protected
#define file_protected
#endif
#endif

#ifndef __arraycount
#define __arraycount(a) (sizeof(a) / sizeof(a[0]))
#endif

#ifndef __GNUC_PREREQ__
#ifdef __GNUC__
#define	__GNUC_PREREQ__(x, y)						\
	((__GNUC__ == (x) && __GNUC_MINOR__ >= (y)) ||			\
	 (__GNUC__ > (x)))
#else
#define	__GNUC_PREREQ__(x, y)	0
#endif
#endif

#ifndef __GNUC__
#ifndef __attribute__
#define __attribute__(a)
#endif
#endif

#ifndef MIN
#define	MIN(a,b)	(((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define	MAX(a,b)	(((a) > (b)) ? (a) : (b))
#endif

#ifndef O_CLOEXEC
# define O_CLOEXEC 0
#endif

#ifndef FD_CLOEXEC
# define FD_CLOEXEC 1
#endif


/*
 * Dec 31, 23:59:59 9999
 * we need to make sure that we don't exceed 9999 because some libc
 * implementations like muslc crash otherwise. If you are unlucky
 * to be running on a system with a 32 bit time_t, then it is even less.
 */
#define	MAX_CTIME \
    CAST(time_t, sizeof(time_t) > 4 ? 0x3afff487cfULL : 0x7fffffffULL)

#define FILE_BADSIZE CAST(size_t, ~0ul)
#define MAXDESC	64		/* max len of text description/MIME type */
#define MAXMIME	80		/* max len of text MIME type */
#define MAXstring 128		/* max len of "string" types */

#define MAGICNO		0xF11E041C
#define VERSIONNO	18
#define FILE_MAGICSIZE	376

#define FILE_GUID_SIZE	sizeof("XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX")

#define	FILE_LOAD	0
#define FILE_CHECK	1
#define FILE_COMPILE	2
#define FILE_LIST	3

struct buffer {
	int fd;
	zend_stat_t st;
	const void *fbuf;
	size_t flen;
	zend_off_t eoff;
	void *ebuf;
	size_t elen;
};

union VALUETYPE {
	uint8_t b;
	uint16_t h;
	uint32_t l;
	uint64_t q;
	uint8_t hs[2];	/* 2 bytes of a fixed-endian "short" */
	uint8_t hl[4];	/* 4 bytes of a fixed-endian "long" */
	uint8_t hq[8];	/* 8 bytes of a fixed-endian "quad" */
	char s[MAXstring];	/* the search string or regex pattern */
	unsigned char us[MAXstring];
	uint64_t guid[2];
	float f;
	double d;
};

struct magic {
	/* Word 1 */
	uint16_t cont_level;	/* level of ">" */
	uint8_t flag;
#define INDIR		0x01	/* if '(...)' appears */
#define OFFADD		0x02	/* if '>&' or '>...(&' appears */
#define INDIROFFADD	0x04	/* if '>&(' appears */
#define UNSIGNED	0x08	/* comparison is unsigned */
#define NOSPACE		0x10	/* suppress space character before output */
#define BINTEST		0x20	/* test is for a binary type (set only
				   for top-level tests) */
#define TEXTTEST	0x40	/* for passing to file_softmagic */
#define OFFNEGATIVE	0x80	/* relative to the end of file */

	uint8_t factor;

	/* Word 2 */
	uint8_t reln;		/* relation (0=eq, '>'=gt, etc) */
	uint8_t vallen;		/* length of string value, if any */
	uint8_t type;		/* comparison type (FILE_*) */
	uint8_t in_type;	/* type of indirection */
#define 			FILE_INVALID		0
#define 			FILE_BYTE		1
#define				FILE_SHORT		2
#define				FILE_DEFAULT		3
#define				FILE_LONG		4
#define				FILE_STRING		5
#define				FILE_DATE		6
#define				FILE_BESHORT		7
#define				FILE_BELONG		8
#define				FILE_BEDATE		9
#define				FILE_LESHORT		10
#define				FILE_LELONG		11
#define				FILE_LEDATE		12
#define				FILE_PSTRING		13
#define				FILE_LDATE		14
#define				FILE_BELDATE		15
#define				FILE_LELDATE		16
#define				FILE_REGEX		17
#define				FILE_BESTRING16		18
#define				FILE_LESTRING16		19
#define				FILE_SEARCH		20
#define				FILE_MEDATE		21
#define				FILE_MELDATE		22
#define				FILE_MELONG		23
#define				FILE_QUAD		24
#define				FILE_LEQUAD		25
#define				FILE_BEQUAD		26
#define				FILE_QDATE		27
#define				FILE_LEQDATE		28
#define				FILE_BEQDATE		29
#define				FILE_QLDATE		30
#define				FILE_LEQLDATE		31
#define				FILE_BEQLDATE		32
#define				FILE_FLOAT		33
#define				FILE_BEFLOAT		34
#define				FILE_LEFLOAT		35
#define				FILE_DOUBLE		36
#define				FILE_BEDOUBLE		37
#define				FILE_LEDOUBLE		38
#define				FILE_BEID3		39
#define				FILE_LEID3		40
#define				FILE_INDIRECT		41
#define				FILE_QWDATE		42
#define				FILE_LEQWDATE		43
#define				FILE_BEQWDATE		44
#define				FILE_NAME		45
#define				FILE_USE		46
#define				FILE_CLEAR		47
#define				FILE_DER		48
#define				FILE_GUID		49
#define				FILE_OFFSET		50
#define				FILE_BEVARINT		51
#define				FILE_LEVARINT		52
#define				FILE_MSDOSDATE		53
#define				FILE_LEMSDOSDATE	54
#define				FILE_BEMSDOSDATE	55
#define				FILE_MSDOSTIME		56
#define				FILE_LEMSDOSTIME	57
#define				FILE_BEMSDOSTIME	58
#define				FILE_OCTAL		59
#define				FILE_NAMES_SIZE		60 /* size of array to contain all names */

#define IS_LIBMAGIC_STRING(t) \
	((t) == FILE_STRING || \
	 (t) == FILE_PSTRING || \
	 (t) == FILE_BESTRING16 || \
	 (t) == FILE_LESTRING16 || \
	 (t) == FILE_REGEX || \
	 (t) == FILE_SEARCH || \
	 (t) == FILE_INDIRECT || \
	 (t) == FILE_NAME || \
	 (t) == FILE_USE || \
	 (t) == FILE_OCTAL)

#define FILE_FMT_NONE 0
#define FILE_FMT_NUM  1 /* "cduxXi" */
#define FILE_FMT_STR  2 /* "s" */
#define FILE_FMT_QUAD 3 /* "ll" */
#define FILE_FMT_FLOAT 4 /* "eEfFgG" */
#define FILE_FMT_DOUBLE 5 /* "eEfFgG" */

	/* Word 3 */
	uint8_t in_op;		/* operator for indirection */
	uint8_t mask_op;	/* operator for mask */
#ifdef ENABLE_CONDITIONALS
	uint8_t cond;		/* conditional type */
#else
	uint8_t dummy;
#endif
	uint8_t factor_op;
#define		FILE_FACTOR_OP_PLUS	'+'
#define		FILE_FACTOR_OP_MINUS	'-'
#define		FILE_FACTOR_OP_TIMES	'*'
#define		FILE_FACTOR_OP_DIV	'/'
#define		FILE_FACTOR_OP_NONE	'\0'

#define				FILE_OPS	"&|^+-*/%"
#define				FILE_OPAND	0
#define				FILE_OPOR	1
#define				FILE_OPXOR	2
#define				FILE_OPADD	3
#define				FILE_OPMINUS	4
#define				FILE_OPMULTIPLY	5
#define				FILE_OPDIVIDE	6
#define				FILE_OPMODULO	7
#define				FILE_OPS_MASK	0x07 /* mask for above ops */
#define				FILE_UNUSED_1	0x08
#define				FILE_UNUSED_2	0x10
#define				FILE_OPSIGNED	0x20
#define				FILE_OPINVERSE	0x40
#define				FILE_OPINDIRECT	0x80

#ifdef ENABLE_CONDITIONALS
#define				COND_NONE	0
#define				COND_IF		1
#define				COND_ELIF	2
#define				COND_ELSE	3
#endif /* ENABLE_CONDITIONALS */

	/* Word 4 */
	int32_t offset;		/* offset to magic number */
	/* Word 5 */
	int32_t in_offset;	/* offset from indirection */
	/* Word 6 */
	uint32_t lineno;	/* line number in magic file */
	/* Word 7,8 */
	union {
		uint64_t _mask;	/* for use with numeric and date types */
		struct {
			uint32_t _count;	/* repeat/line count */
			uint32_t _flags;	/* modifier flags */
		} _s;		/* for use with string types */
	} _u;
#define num_mask _u._mask
#define str_range _u._s._count
#define str_flags _u._s._flags
	/* Words 9-24 */
	union VALUETYPE value;	/* either number or string */
	/* Words 25-40 */
	char desc[MAXDESC];	/* description */
	/* Words 41-60 */
	char mimetype[MAXMIME]; /* MIME type */
	/* Words 61-62 */
	char apple[8];		/* APPLE CREATOR/TYPE */
	/* Words 63-78 */
	char ext[64];		/* Popular extensions */
};

#define BIT(A)   (1 << (A))
#define STRING_COMPACT_WHITESPACE		BIT(0)
#define STRING_COMPACT_OPTIONAL_WHITESPACE	BIT(1)
#define STRING_IGNORE_LOWERCASE			BIT(2)
#define STRING_IGNORE_UPPERCASE			BIT(3)
#define REGEX_OFFSET_START			BIT(4)
#define STRING_TEXTTEST				BIT(5)
#define STRING_BINTEST				BIT(6)
#define PSTRING_1_BE				BIT(7)
#define PSTRING_1_LE				BIT(7)
#define PSTRING_2_BE				BIT(8)
#define PSTRING_2_LE				BIT(9)
#define PSTRING_4_BE				BIT(10)
#define PSTRING_4_LE				BIT(11)
#define REGEX_LINE_COUNT			BIT(11)
#define PSTRING_LEN	\
    (PSTRING_1_BE|PSTRING_2_LE|PSTRING_2_BE|PSTRING_4_LE|PSTRING_4_BE)
#define PSTRING_LENGTH_INCLUDES_ITSELF		BIT(12)
#define	STRING_TRIM				BIT(13)
#define	STRING_FULL_WORD			BIT(14)
#define CHAR_COMPACT_WHITESPACE			'W'
#define CHAR_COMPACT_OPTIONAL_WHITESPACE	'w'
#define CHAR_IGNORE_LOWERCASE			'c'
#define CHAR_IGNORE_UPPERCASE			'C'
#define CHAR_REGEX_OFFSET_START			's'
#define CHAR_TEXTTEST				't'
#define	CHAR_TRIM				'T'
#define	CHAR_FULL_WORD				'f'
#define CHAR_BINTEST				'b'
#define CHAR_PSTRING_1_BE			'B'
#define CHAR_PSTRING_1_LE			'B'
#define CHAR_PSTRING_2_BE			'H'
#define CHAR_PSTRING_2_LE			'h'
#define CHAR_PSTRING_4_BE			'L'
#define CHAR_PSTRING_4_LE			'l'
#define CHAR_PSTRING_LENGTH_INCLUDES_ITSELF     'J'
#define STRING_IGNORE_CASE		(STRING_IGNORE_LOWERCASE|STRING_IGNORE_UPPERCASE)
#define STRING_DEFAULT_RANGE		100

#define	INDIRECT_RELATIVE			BIT(0)
#define	CHAR_INDIRECT_RELATIVE			'r'

/* list of magic entries */
struct mlist {
	struct magic *magic;		/* array of magic entries */
	size_t nmagic;			/* number of entries in array */
	void *map;			/* internal resources used by entry */
	struct mlist *next, *prev;
};

#ifdef __cplusplus
#define CAST(T, b)	static_cast<T>(b)
#define RCAST(T, b)	reinterpret_cast<T>(b)
#define CCAST(T, b)	const_cast<T>(b)
#else
#define CAST(T, b)	((T)(b))
#define RCAST(T, b)	((T)(uintptr_t)(b))
#define CCAST(T, b)	((T)(uintptr_t)(b))
#endif

struct level_info {
	int32_t off;
	int got_match;
#ifdef ENABLE_CONDITIONALS
	int last_match;
	int last_cond;	/* used for error checking by parse() */
#endif
};

struct cont {
	size_t len;
	struct level_info *li;
};

#define MAGIC_SETS	2

struct magic_set {
	struct mlist *mlist[MAGIC_SETS];	/* list of regular entries */
	struct cont c;
	struct out {
		char *buf;		/* Accumulation buffer */
		size_t blen;		/* Length of buffer */
		char *pbuf;		/* Printable buffer */
	} o;
	uint32_t offset;			/* a copy of m->offset while we */
					/* are working on the magic entry */
	uint32_t eoffset;		/* offset from end of file */
	int error;
	int flags;			/* Control magic tests. */
	int event_flags;		/* Note things that happened. */
#define 		EVENT_HAD_ERR		0x01
	const char *file;
	size_t line;			/* current magic line number */
	mode_t mode;			/* copy of current stat mode */

	/* data for searches */
	struct {
		const char *s;		/* start of search in original source */
		size_t s_len;		/* length of search region */
		size_t offset;		/* starting offset in source: XXX - should this be off_t? */
		size_t rm_len;		/* match length */
	} search;

	/* FIXME: Make the string dynamically allocated so that e.g.
	   strings matched in files can be longer than MAXstring */
	union VALUETYPE ms_value;	/* either number or string */
	uint16_t indir_max;
	uint16_t name_max;
	uint16_t elf_shnum_max;
	uint16_t elf_phnum_max;
	uint16_t elf_notes_max;
	uint16_t regex_max;
	size_t bytes_max;		/* number of bytes to read from file */
	size_t encoding_max;		/* bytes to look for encoding */
	size_t	elf_shsize_max;
#ifndef FILE_BYTES_MAX
# define FILE_BYTES_MAX (7 * 1024 * 1024)/* how much of the file to look at */
#endif /* above 0x6ab0f4 map offset for HelveticaNeue.dfont */
#define	FILE_ELF_NOTES_MAX		256
#define	FILE_ELF_PHNUM_MAX		2048
#define	FILE_ELF_SHNUM_MAX		32768
#define	FILE_ELF_SHSIZE_MAX		(128 * 1024 * 1024)
#define	FILE_INDIR_MAX			50
#define	FILE_NAME_MAX			50
#define	FILE_REGEX_MAX			8192
#define	FILE_ENCODING_MAX		(64 * 1024)
#if defined(HAVE_NEWLOCALE) && defined(HAVE_USELOCALE) && defined(HAVE_FREELOCALE)
#define USE_C_LOCALE
	locale_t c_lc_ctype;
#define file_locale_used
#else
#define file_locale_used __attribute__((__unused__))
#endif
};

/* Type for Unicode characters */
typedef unsigned long file_unichar_t;

struct stat;
#define FILE_T_LOCAL	1
#define FILE_T_WINDOWS	2
file_protected const char *file_fmtdatetime(char *, size_t, uint64_t, int);
file_protected const char *file_fmtdate(char *, size_t, uint16_t);
file_protected const char *file_fmttime(char *, size_t, uint16_t);
file_protected const char *file_fmtvarint(char *, size_t, const unsigned char *,
    int);
file_protected const char *file_fmtnum(char *, size_t, const char *, int);
file_protected struct magic_set *file_ms_alloc(int);
file_protected void file_ms_free(struct magic_set *);
file_protected int file_buffer(struct magic_set *, php_stream *, zend_stat_t *, const char *, const void *,
    size_t);
file_protected int file_fsmagic(struct magic_set *, const char *, zend_stat_t *);
file_protected int file_pipe2file(struct magic_set *, int, const void *,
    size_t);
file_protected int file_vprintf(struct magic_set *, const char *, va_list)
    __attribute__((__format__(__printf__, 2, 0)));
file_protected int file_separator(struct magic_set *);
file_protected char *file_copystr(char *, size_t, size_t, const char *);
file_protected int file_checkfmt(char *, size_t, const char *);
file_protected size_t file_printedlen(const struct magic_set *);
file_protected int file_print_guid(char *, size_t, const uint64_t *);
file_protected int file_parse_guid(const char *, uint64_t *);
file_protected int file_replace(struct magic_set *, const char *, const char *);
file_protected int file_printf(struct magic_set *, const char *, ...)
    __attribute__((__format__(__printf__, 2, 3)));
file_protected int file_reset(struct magic_set *, int);
file_protected int file_tryelf(struct magic_set *, const struct buffer *);
file_protected int file_trycdf(struct magic_set *, const struct buffer *);
#ifdef PHP_FILEINFO_UNCOMPRESS
file_protected int file_zmagic(struct magic_set *, const struct buffer *,
    const char *);
#endif
file_protected int file_ascmagic(struct magic_set *, const struct buffer *,
    int);
file_protected int file_ascmagic_with_encoding(struct magic_set *,
    const struct buffer *, file_unichar_t *, size_t, const char *, const char *, int);
file_protected int file_encoding(struct magic_set *, const struct buffer *,
    file_unichar_t **, size_t *, const char **, const char **, const char **);
file_protected int file_is_json(struct magic_set *, const struct buffer *);
file_protected int file_is_csv(struct magic_set *, const struct buffer *, int,
    const char *);
file_protected int file_is_simh(struct magic_set *, const struct buffer *);
file_protected int file_is_tar(struct magic_set *, const struct buffer *);
file_protected int file_softmagic(struct magic_set *, const struct buffer *,
    uint16_t *, uint16_t *, int, int);
file_protected int file_apprentice(struct magic_set *, const char *, int);
file_protected size_t file_magic_strength(const struct magic *, size_t);
file_protected int buffer_apprentice(struct magic_set *, struct magic **,
    size_t *, size_t);
file_protected int file_magicfind(struct magic_set *, const char *,
    struct mlist *);
file_protected uint64_t file_signextend(struct magic_set *, struct magic *,
    uint64_t);
file_protected uintmax_t file_varint2uintmax_t(const unsigned char *, int,
    size_t *);

file_protected void file_badread(struct magic_set *);
file_protected void file_badseek(struct magic_set *);
file_protected void file_oomem(struct magic_set *, size_t);
file_protected void file_error(struct magic_set *, int, const char *, ...)
    __attribute__((__format__(__printf__, 3, 4)));
file_protected void file_magerror(struct magic_set *, const char *, ...)
    __attribute__((__format__(__printf__, 2, 3)));
file_protected void file_magwarn(struct magic_set *, const char *, ...)
    __attribute__((__format__(__printf__, 2, 3)));
file_protected void file_mdump(struct magic *);
file_protected void file_showstr(FILE *, const char *, size_t);
file_protected size_t file_mbswidth(struct magic_set *, const char *);
file_protected const char *file_getbuffer(struct magic_set *);
file_protected ssize_t sread(int, void *, size_t, int);
file_protected int file_check_mem(struct magic_set *, unsigned int);
file_protected int file_looks_utf8(const unsigned char *, size_t,
    file_unichar_t *, size_t *);
file_protected size_t file_pstring_length_size(struct magic_set *,
    const struct magic *);
file_protected size_t file_pstring_get_length(struct magic_set *,
    const struct magic *, const char *);
file_protected char * file_printable(struct magic_set *, char *, size_t,
    const char *, size_t);
#ifdef __EMX__
file_protected int file_os2_apptype(struct magic_set *, const char *,
    const void *, size_t);
#endif /* __EMX__ */
file_protected int file_pipe_closexec(int *);
file_protected int file_clear_closexec(int);
file_protected char *file_strtrim(char *);

file_protected void buffer_init(struct buffer *, int, const zend_stat_t *,
    const void *, size_t);
file_protected void buffer_fini(struct buffer *);
file_protected int buffer_fill(const struct buffer *);



typedef struct {
	char *buf;
	size_t blen;
	uint32_t offset;
} file_pushbuf_t;

file_protected file_pushbuf_t *file_push_buffer(struct magic_set *);
file_protected char  *file_pop_buffer(struct magic_set *, file_pushbuf_t *);

#ifndef COMPILE_ONLY
extern const char *file_names[];
extern const size_t file_nnames;
#endif

#ifndef strlcpy
size_t strlcpy(char *, const char *, size_t);
#endif
#ifndef strlcat
size_t strlcat(char *, const char *, size_t);
#endif
#ifndef HAVE_STRCASESTR
char *strcasestr(const char *, const char *);
#endif
#ifndef HAVE_GETLINE
ssize_t getline(char **, size_t *, FILE *);
ssize_t getdelim(char **, size_t *, int, FILE *);
#endif
#ifndef HAVE_CTIME_R
char   *ctime_r(const time_t *, char *);
#endif
#ifndef HAVE_ASCTIME_R
char   *asctime_r(const struct tm *, char *);
#endif

#if defined(HAVE_MMAP) && defined(HAVE_SYS_MMAN_H) && !defined(QUICK)
#define QUICK
#endif

#ifndef O_BINARY
#define O_BINARY	0
#endif
#ifndef O_NONBLOCK
#define O_NONBLOCK	0
#endif

#ifndef __cplusplus
#if defined(__GNUC__) && (__GNUC__ >= 3)
#define FILE_RCSID(id) \
static const char rcsid[] __attribute__((__used__)) = id;
#else
#define FILE_RCSID(id) \
static const char *rcsid(const char *p) { \
	return rcsid(p = id); \
}
#endif
#else
#define FILE_RCSID(id)
#endif
#ifndef __RCSID
#define __RCSID(a)
#endif

#endif /* __file_h__ */
