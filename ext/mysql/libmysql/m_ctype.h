/* Copyright (C) 2000 MySQL AB & MySQL Finland AB & TCX DataKonsult AB
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
   MA 02111-1307, USA */

/*
  A better inplementation of the UNIX ctype(3) library.
  Notes:   global.h should be included before ctype.h
*/

#ifndef _m_ctype_h
#define _m_ctype_h

#ifdef	__cplusplus
extern "C" {
#endif

#define CHARSET_DIR	"charsets/"

typedef struct charset_info_st
{
    uint      number;
    const char *name;
    uchar    *ctype;
    uchar    *to_lower;
    uchar    *to_upper;
    uchar    *sort_order;

    uint      strxfrm_multiply;
    int     (*strcoll)(const uchar *, const uchar *);
    int     (*strxfrm)(uchar *, const uchar *, int);
    int     (*strnncoll)(const uchar *, int, const uchar *, int);
    int     (*strnxfrm)(uchar *, const uchar *, int, int);
    my_bool (*like_range)(const char *, uint, pchar, uint,
                          char *, char *, uint *, uint *);

    uint      mbmaxlen;
    int     (*ismbchar)(const char *, const char *);
    my_bool (*ismbhead)(uint);
    int     (*mbcharlen)(uint);
} CHARSET_INFO;

/* strings/ctype.c */
extern CHARSET_INFO *default_charset_info;
extern CHARSET_INFO *find_compiled_charset(uint cs_number);
extern CHARSET_INFO *find_compiled_charset_by_name(const char *name);
extern CHARSET_INFO  compiled_charsets[];

#define MY_CHARSET_UNDEFINED 0
#define MY_CHARSET_CURRENT (default_charset_info->number)

#ifdef __WIN__
#include <ctype.h>
#endif
/* Don't include std ctype.h when this is included */
#define _CTYPE_H
#define _CTYPE_INCLUDED
#define __CTYPE_INCLUDED
#define _CTYPE_USING   /* Don't put names in global namespace. */

#define	_U	01	/* Upper case */
#define	_L	02	/* Lower case */
#define	_N	04	/* Numeral (digit) */
#define	_S	010	/* Spacing character */
#define	_P	020	/* Punctuation */
#define	_C	040	/* Control character */
#define	_B	0100	/* Blank */
#define	_X	0200	/* heXadecimal digit */

#define my_ctype	(default_charset_info->ctype)
#define my_to_upper	(default_charset_info->to_upper)
#define my_to_lower	(default_charset_info->to_lower)
#define my_sort_order	(default_charset_info->sort_order)

#ifndef __WIN__
#define	_toupper(c)	(char) my_to_upper[(uchar) (c)]
#define	_tolower(c)	(char) my_to_lower[(uchar) (c)]
#define toupper(c)	(char) my_to_upper[(uchar) (c)]
#define tolower(c)	(char) my_to_lower[(uchar) (c)]

#define	isalpha(c)	((my_ctype+1)[(uchar) (c)] & (_U | _L))
#define	isupper(c)	((my_ctype+1)[(uchar) (c)] & _U)
#define	islower(c)	((my_ctype+1)[(uchar) (c)] & _L)
#define	isdigit(c)	((my_ctype+1)[(uchar) (c)] & _N)
#define	isxdigit(c)	((my_ctype+1)[(uchar) (c)] & _X)
#define	isalnum(c)	((my_ctype+1)[(uchar) (c)] & (_U | _L | _N))
#define	isspace(c)	((my_ctype+1)[(uchar) (c)] & _S)
#define	ispunct(c)	((my_ctype+1)[(uchar) (c)] & _P)
#define	isprint(c)	((my_ctype+1)[(uchar) (c)] & (_P | _U | _L | _N | _B))
#define	isgraph(c)	((my_ctype+1)[(uchar) (c)] & (_P | _U | _L | _N))
#define	iscntrl(c)	((my_ctype+1)[(uchar) (c)] & _C)
#define	isascii(c)	(!((c) & ~0177))
#define	toascii(c)	((c) & 0177)

#ifdef ctype
#undef ctype
#endif /* ctype */

#endif	/* __WIN__ */

#define	my_isalpha(s, c)  (((s)->ctype+1)[(uchar) (c)] & (_U | _L))
#define	my_isupper(s, c)  (((s)->ctype+1)[(uchar) (c)] & _U)
#define	my_islower(s, c)  (((s)->ctype+1)[(uchar) (c)] & _L)
#define	my_isdigit(s, c)  (((s)->ctype+1)[(uchar) (c)] & _N)
#define	my_isxdigit(s, c) (((s)->ctype+1)[(uchar) (c)] & _X)
#define	my_isalnum(s, c)  (((s)->ctype+1)[(uchar) (c)] & (_U | _L | _N))
#define	my_isspace(s, c)  (((s)->ctype+1)[(uchar) (c)] & _S)
#define	my_ispunct(s, c)  (((s)->ctype+1)[(uchar) (c)] & _P)
#define	my_isprint(s, c)  (((s)->ctype+1)[(uchar) (c)] & (_P | _U | _L | _N | _B))
#define	my_isgraph(s, c)  (((s)->ctype+1)[(uchar) (c)] & (_P | _U | _L | _N))
#define	my_iscntrl(s, c)  (((s)->ctype+1)[(uchar) (c)] & _C)

#define use_strcoll(s)                ((s)->strcoll != NULL)
#define MY_STRXFRM_MULTIPLY           (default_charset_info->strxfrm_multiply)
#define my_strnxfrm(s, a, b, c, d)    ((s)->strnxfrm((a), (b), (c), (d)))
#define my_strnncoll(s, a, b, c, d)   ((s)->strnncoll((a), (b), (c), (d)))
#define my_strxfrm(s, a, b, c, d)     ((s)->strnxfrm((a), (b), (c)))
#define my_strcoll(s, a, b)           ((s)->strcoll((a), (b)))
#define my_like_range(s, a, b, c, d, e, f, g, h) \
                ((s)->like_range((a), (b), (c), (d), (e), (f), (g), (h)))

#define use_mb(s)                     ((s)->ismbchar != NULL)
#define MBMAXLEN                      (default_charset_info->mbmaxlen)
#define my_ismbchar(s, a, b)          ((s)->ismbchar((a), (b)))
#define my_ismbhead(s, a)             ((s)->ismbhead((a)))
#define my_mbcharlen(s, a)            ((s)->mbcharlen((a)))

/* Some macros that should be cleaned up a little */
#define isvar(c)	(isalnum(c) || (c) == '_')
#define isvar_start(c)	(isalpha(c) || (c) == '_')
#define tocntrl(c)	((c) & 31)
#define toprint(c)	((c) | 64)

/* XXX: still need to take care of this one */
#ifdef MY_CHARSET_TIS620
#error The TIS620 charset is broken at the moment.  Tell tim to fix it.
#define USE_TIS620
#include "t_ctype.h"
#endif

#ifdef	__cplusplus
}
#endif

#endif /* _m_ctype_h */
