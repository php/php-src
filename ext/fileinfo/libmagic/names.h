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
 * Names.h - names and types used by ascmagic in file(1).
 * These tokens are here because they can appear anywhere in
 * the first HOWMANY bytes, while tokens in MAGIC must
 * appear at fixed offsets into the file. Don't make HOWMANY
 * too high unless you have a very fast CPU.
 *
 * $File: names.h,v 1.32 2008/02/11 00:19:29 rrt Exp $
 */

/*
	modified by Chris Lowth - 9 April 2000
	to add mime type strings to the types table.
*/

/* these types are used to index the table 'types': keep em in sync! */
#define	L_C	0		/* first and foremost on UNIX */
#define	L_CC	1		/* Bjarne's postincrement */
#define	L_MAKE	2		/* Makefiles */
#define	L_PLI	3		/* PL/1 */
#define	L_MACH	4		/* some kinda assembler */
#define	L_ENG	5		/* English */
#define	L_PAS	6		/* Pascal */
#define	L_MAIL	7		/* Electronic mail */
#define	L_NEWS	8		/* Usenet Netnews */
#define	L_JAVA	9		/* Java code */
#define	L_HTML	10		/* HTML */
#define	L_BCPL	11		/* BCPL */
#define	L_M4	12		/* M4 */
#define	L_PO	13		/* PO */

static const struct {
	char human[48];
	char mime[16];
} types[] = {
	{ "C program",					"text/x-c", },
	{ "C++ program",				"text/x-c++" },
	{ "make commands",				"text/x-makefile" },
	{ "PL/1 program",				"text/x-pl1" },
	{ "assembler program",				"text/x-asm" },
	{ "English",					"text/plain" },
	{ "Pascal program",				"text/x-pascal" },
	{ "mail",					"text/x-mail" },
	{ "news",					"text/x-news" },
	{ "Java program",				"text/x-java" },
	{ "HTML document",				"text/html", },
	{ "BCPL program",				"text/x-bcpl" },
	{ "M4 macro language pre-processor",		"text/x-m4" },
	{ "PO (gettext message catalogue)",             "text/x-po" },
	{ "cannot happen error on names.h/types",	"error/x-error" }
};

/*
 * XXX - how should we distinguish Java from C++?
 * The trick used in a Debian snapshot, of having "extends" or "implements"
 * as tags for Java, doesn't work very well, given that those keywords
 * are often preceded by "class", which flags it as C++.
 *
 * Perhaps we need to be able to say
 *
 *	If "class" then
 *
 *		if "extends" or "implements" then
 *			Java
 *		else
 *			C++
 *	endif
 *
 * Or should we use other keywords, such as "package" or "import"?
 * Unfortunately, Ada95 uses "package", and Modula-3 uses "import",
 * although I infer from the language spec at
 *
 *	http://www.research.digital.com/SRC/m3defn/html/m3.html
 *
 * that Modula-3 uses "IMPORT" rather than "import", i.e. it must be
 * in all caps.
 *
 * So, for now, we go with "import".  We must put it before the C++
 * stuff, so that we don't misidentify Java as C++.  Not using "package"
 * means we won't identify stuff that defines a package but imports
 * nothing; hopefully, very little Java code imports nothing (one of the
 * reasons for doing OO programming is to import as much as possible
 * and write only what you need to, right?).
 *
 * Unfortunately, "import" may cause us to misidentify English text
 * as Java, as it comes after "the" and "The".  Perhaps we need a fancier
 * heuristic to identify Java?
 */
static const struct names {
	char name[14];
	short type;
} names[] = {
	/* These must be sorted by eye for optimal hit rate */
	/* Add to this list only after substantial meditation */
	{"msgid",	L_PO},
	{"dnl",		L_M4},
	{"import",	L_JAVA},
	{"\"libhdr\"",	L_BCPL},
	{"\"LIBHDR\"",	L_BCPL},
	{"//",		L_CC},
	{"template",	L_CC},
	{"virtual",	L_CC},
	{"class",	L_CC},
	{"public:",	L_CC},
	{"private:",	L_CC},
	{"/*",		L_C},	/* must precede "The", "the", etc. */
	{"#include",	L_C},
	{"char",	L_C},
	{"The",		L_ENG},
	{"the",		L_ENG},
	{"double",	L_C},
	{"extern",	L_C},
	{"float",	L_C},
	{"struct",	L_C},
	{"union",	L_C},
	{"CFLAGS",	L_MAKE},
	{"LDFLAGS",	L_MAKE},
	{"all:",	L_MAKE},
	{".PRECIOUS",	L_MAKE},
	{".ascii",	L_MACH},
	{".asciiz",	L_MACH},
	{".byte",	L_MACH},
	{".even",	L_MACH},
	{".globl",	L_MACH},
	{".text",	L_MACH},
	{"clr",		L_MACH},
	{"(input,",	L_PAS},
	{"program",	L_PAS},
	{"record",	L_PAS},
	{"dcl",		L_PLI},
	{"Received:",	L_MAIL},
	{">From",	L_MAIL},
	{"Return-Path:",L_MAIL},
	{"Cc:",		L_MAIL},
	{"Newsgroups:",	L_NEWS},
	{"Path:",	L_NEWS},
	{"Organization:",L_NEWS},
	{"href=",	L_HTML},
	{"HREF=",	L_HTML},
	{"<body",	L_HTML},
	{"<BODY",	L_HTML},
	{"<html",	L_HTML},
	{"<HTML",	L_HTML},
	{"<!--",	L_HTML},
};
#define NNAMES (sizeof(names)/sizeof(struct names))
