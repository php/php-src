/*
  +----------------------------------------------------------------------+
  | PHP Version 4                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2003 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.02 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/2_02.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+

  $Id$ 
*/

#ifndef PHP_MIME_MAGIC_H
#define PHP_MIME_MAGIC_H

#define MIME_MAGIC_DEBUG        0

#define MIME_BINARY_UNKNOWN    "application/octet-stream"
#define MIME_TEXT_UNKNOWN    "text/plain"

#define MAXMIMESTRING        256

#define MIME_MAGIC_OK 0
#define MIME_MAGIC_DECLINED -1
#define MIME_MAGIC_DONE -2
#define MIME_MAGIC_ERROR -3

#define DIR_MAGIC_TYPE "httpd/unix-directory"

/* HOWMANY must be at least 4096 to make gzip -dcq work */
#define HOWMANY	4096
/* SMALL_HOWMANY limits how much work we do to figure out text files */
#define SMALL_HOWMANY 1024
#define MAXDESC    50		/* max leng of text description */
#define MAXstring 64		/* max leng of "string" types */

struct magic {
    struct magic *next;		/* link to next entry */
    int lineno;			/* line number from magic file */

    short flag;
#define INDIR    1		/* if '>(...)' appears,  */
#define    UNSIGNED 2		/* comparison is unsigned */
    short cont_level;		/* level of ">" */
    struct {
		char type;		/* byte short long */
		long offset;		/* offset from indirection */
    } in;
    long offset;		/* offset to magic number */
    unsigned char reln;		/* relation (0=eq, '>'=gt, etc) */
    char type;			/* int, short, long or string. */
    char vallen;		/* length of string value, if any */
#define BYTE    1
#define SHORT    2
#define LONG    4
#define STRING    5
#define DATE    6
#define BESHORT    7
#define BELONG    8
#define BEDATE    9
#define LESHORT    10
#define LELONG    11
#define LEDATE    12
    union VALUETYPE {
		unsigned char b;
		unsigned short h;
		unsigned long l;
		char s[MAXstring];
		unsigned char hs[2];	/* 2 bytes of a fixed-endian "short" */
		unsigned char hl[4];	/* 2 bytes of a fixed-endian "long" */
    } value;			/* either number or string */
    unsigned long mask;		/* mask before comparison with value */
    char nospflag;		/* supress space character */

    /* NOTE: this string is suspected of overrunning - find it! */
    char desc[MAXDESC];		/* description */
};

/*
 * data structures for tar file recognition
 * --------------------------------------------------------------------------
 * Header file for public domain tar (tape archive) program.
 *
 * @(#)tar.h 1.20 86/10/29    Public Domain. Created 25 August 1985 by John
 * Gilmore, ihnp4!hoptoad!gnu.
 *
 * Header block on tape.
 *
 * I'm going to use traditional DP naming conventions here. A "block" is a big
 * chunk of stuff that we do I/O on. A "record" is a piece of info that we
 * care about. Typically many "record"s fit into a "block".
 */
#define RECORDSIZE    512
#define NAMSIZ    100
#define TUNMLEN    32
#define TGNMLEN    32

union record {
    char charptr[RECORDSIZE];
    struct header {
	char name[NAMSIZ];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char chksum[8];
	char linkflag;
	char linkname[NAMSIZ];
	char magic[8];
	char uname[TUNMLEN];
	char gname[TGNMLEN];
	char devmajor[8];
	char devminor[8];
    } header;
};

/* The magic field is filled with this if uname and gname are valid. */
#define    TMAGIC        "ustar  "	/* 7 chars and a null */

/*
 * file-function prototypes
 */
/*TODO*/ 
#define request_rec void
#define server_rec void
#define pool void
#define cmd_parms void

/*
 * includes for ASCII substring recognition formerly "names.h" in file
 * command
 *
 * Original notes: names and types used by ascmagic in file(1). These tokens are
 * here because they can appear anywhere in the first HOWMANY bytes, while
 * tokens in /etc/magic must appear at fixed offsets into the file. Don't
 * make HOWMANY too high unless you have a very fast CPU.
 */

/* these types are used to index the table 'types': keep em in sync! */
/* HTML inserted in first because this is a web server module now */
#define L_HTML    0		/* HTML */
#define L_C       1		/* first and foremost on UNIX */
#define L_FORT    2		/* the oldest one */
#define L_MAKE    3		/* Makefiles */
#define L_PLI     4		/* PL/1 */
#define L_MACH    5		/* some kinda assembler */
#define L_ENG     6		/* English */
#define L_PAS     7		/* Pascal */
#define L_MAIL    8		/* Electronic mail */
#define L_NEWS    9		/* Usenet Netnews */

static char *types[] =
	{
		"text/html",		/* HTML */
		"text/plain",		/* "c program text", */
		"text/plain",		/* "fortran program text", */
		"text/plain",		/* "make commands text", */
		"text/plain",		/* "pl/1 program text", */
		"text/plain",		/* "assembler program text", */
		"text/plain",		/* "English text", */
		"text/plain",		/* "pascal program text", */
		"message/rfc822",		/* "mail text", */
		"message/news",		/* "news text", */
		"application/binary",	/* "can't happen error on names.h/types", */
		0
	};

static struct names {
    char *name;
    short type;
} names[] = {

    /* These must be sorted by eye for optimal hit rate */
    /* Add to this list only after substantial meditation */
    {
		"<html>", L_HTML
    },
    {
		"<HTML>", L_HTML
    },
    {
		"<head>", L_HTML
    },
    {
		"<HEAD>", L_HTML
    },
    {
		"<title>", L_HTML
    },
    {
		"<TITLE>", L_HTML
    },
    {
		"<h1>", L_HTML
    },
    {
		"<H1>", L_HTML
    },
    {
		"<!--", L_HTML
    },
    {
		"<!DOCTYPE HTML", L_HTML
    },
    {
		"/*", L_C
    },				/* must precede "The", "the", etc. */
    {
		"#include", L_C
    },
    {
		"char", L_C
    },
    {
		"The", L_ENG
    },
    {
		"the", L_ENG
    },
    {
		"double", L_C
    },
    {
		"extern", L_C
    },
    {
		"float", L_C
    },
    {
		"real", L_C
    },
    {
		"struct", L_C
    },
    {
		"union", L_C
    },
    {
		"CFLAGS", L_MAKE
    },
    {
		"LDFLAGS", L_MAKE
    },
    {
		"all:", L_MAKE
    },
    {
		".PRECIOUS", L_MAKE
    },
    /*
     * Too many files of text have these words in them.  Find another way to
     * recognize Fortrash.
     */
#ifdef    NOTDEF
    {
		"subroutine", L_FORT
    },
    {
		"function", L_FORT
    },
    {
		"block", L_FORT
    },
    {
		"common", L_FORT
    },
    {
		"dimension", L_FORT
    },
    {
		"integer", L_FORT
    },
    {
		"data", L_FORT
    },
#endif /* NOTDEF */
    {
		".ascii", L_MACH
    },
    {
		".asciiz", L_MACH
    },
    {
		".byte", L_MACH
    },
    {
		".even", L_MACH
    },
    {
		".globl", L_MACH
    },
    {
		"clr", L_MACH
    },
    {
		"(input,", L_PAS
    },
    {
		"dcl", L_PLI
    },
    {
		"Received:", L_MAIL
    },
    {
		">From", L_MAIL
    },
    {
		"Return-Path:", L_MAIL
    },
    {
		"Cc:", L_MAIL
    },
    {
		"Newsgroups:", L_NEWS
    },
    {
		"Path:", L_NEWS
    },
    {
		"Organization:", L_NEWS
    },
    {
		NULL, 0
    }
};

#define NNAMES ((sizeof(names)/sizeof(struct names)) - 1)

/*
 * Result String List (RSL)
 *
 * The file(1) command prints its output.  Instead, we store the various
 * "printed" strings in a list (allocating memory as we go) and concatenate
 * them at the end when we finally know how much space they'll need.
 */

typedef struct magic_rsl_s {
    char *str;			/* string, possibly a fragment */
    struct magic_rsl_s *next;	/* pointer to next fragment */
} magic_rsl;

/*
 * Apache module configuration structures
 */

/* per-server info */
typedef struct {
    char *magicfile;		/* where magic be found */
    struct magic *magic;	/* head of magic config list */
    struct magic *last;
} magic_server_config_rec;

/* per-request info */
typedef struct {
    magic_rsl *head;		/* result string list */
    magic_rsl *tail;
    unsigned suf_recursion;	/* recursion depth in suffix check */
} magic_req_rec;




extern zend_module_entry mime_magic_module_entry;
#define phpext_mime_magic_ptr &mime_magic_module_entry

#ifdef PHP_WIN32
#define PHP_MIME_MAGIC_API __declspec(dllexport)
#else
#define PHP_MIME_MAGIC_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(mime_magic);
PHP_MSHUTDOWN_FUNCTION(mime_magic);
PHP_RINIT_FUNCTION(mime_magic);
PHP_RSHUTDOWN_FUNCTION(mime_magic);
PHP_MINFO_FUNCTION(mime_magic);

PHP_FUNCTION(mime_content_type);	

ZEND_BEGIN_MODULE_GLOBALS(mime_magic)
	char *magicfile;
	magic_req_rec *req_dat;
	int    debug;           /* shall magic file parser errors be shown? */
	char *status;           /* status message for phpinfo() */
ZEND_END_MODULE_GLOBALS(mime_magic)

/* In every utility function you add that needs to use variables 
   in php_mime_magic_globals, call TSRM_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMG_CC
   after the last function argument and declare your utility function
   with TSRMG_DC after the last declared argument.  Always refer to
   the globals in your function as MIME_MAGIC_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define MIME_MAGIC_G(v) TSRMG(mime_magic_globals_id, zend_mime_magic_globals *, v)
#else
#define MIME_MAGIC_G(v) (mime_magic_globals.v)
#endif




#endif	/* PHP_MIME_MAGIC_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
