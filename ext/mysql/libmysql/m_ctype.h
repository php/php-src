/* Copyright (C) 1996  TCX DataKonsult AB & Monty Program KB & Detron HB
   For a more info consult the file COPYRIGHT distributed with this file */
/*
  A better inplementation of the UNIX ctype(3) library.
  Notes:   global.h should be included before ctype.h
*/

#ifndef _m_ctype_h
#define _m_ctype_h

#define MY_CHARSET_UNDEFINED    0
#define MY_CHARSET_BIG5		1
#define MY_CHARSET_CZECH	2
#define MY_CHARSET_DEC8		3
#define MY_CHARSET_DOS		4
#define MY_CHARSET_GERMAN1	5
#define MY_CHARSET_HP8		6
#define MY_CHARSET_KOI8_RU	7
#define MY_CHARSET_LATIN1	8
#define MY_CHARSET_LATIN2	9
#define MY_CHARSET_SWE7		10
#define MY_CHARSET_USA7		11
#define MY_CHARSET_UJIS		12
#define MY_CHARSET_SJIS		13
#define MY_CHARSET_CP1251	14
#define MY_CHARSET_DANISH	15
#define MY_CHARSET_HEBREW	16
#define MY_CHARSET_WIN1251	17
#define MY_CHARSET_TIS620	18
#define MY_CHARSET_EUC_KR	19
#define MY_CHARSET_ESTONIA	20
#define MY_CHARSET_HUNGARIAN	21
#define MY_CHARSET_KOI8_UKR	22
#define MY_CHARSET_WIN1251UKR	23
#define MY_CHARSET_GB2312	24
#define MY_CHARSET_GREEK	25
#define MY_CHARSET_WIN1250	26
#define MY_CHARSET_CROAT	27
#define MY_CHARSET_GBK		28

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef __WIN32__
#include <ctype.h>
#endif
/* Don't include std ctype.h when this is included */
#define _CTYPE_H
#define _CTYPE_INCLUDED
#define __CTYPE_INCLUDED
#define _CTYPE_USING   /* Don't put names in global namespace. */

#ifndef CTYPE_LIBRARY
#define EXT extern
#define D(x)
#else
#define EXT
#define D(x)	= x
#endif

#define	_U	01	/* Upper case */
#define	_L	02	/* Lower case */
#define	_N	04	/* Numeral (digit) */
#define	_S	010	/* Spacing character */
#define	_P	020	/* Punctuation */
#define	_C	040	/* Control character */
#define	_B	0100	/* Blank */
#define	_X	0200	/* heXadecimal digit */

extern uchar NEAR ctype_latin1[];
extern uchar NEAR to_upper_latin1[];
extern uchar NEAR to_lower_latin1[];
extern uchar NEAR sort_order_latin1[];

#define my_ctype	ctype_latin1
#define my_to_upper	to_upper_latin1
#define my_to_lower	to_lower_latin1
#define my_sort_order	sort_order_latin1

#ifndef __WIN32__
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

#endif	/* __WIN32__ */

/* Some macros that should be cleaned up a little */
#define isvar(c)	(isalnum(c) || (c) == '_')
#define isvar_start(c)	(isalpha(c) || (c) == '_')
#define tocntrl(c)	((c) & 31)
#define toprint(c)	((c) | 64)

/* Support for Japanese(UJIS) characters, by tommy@valley.ne.jp */
#if MY_CHARSET_CURRENT == MY_CHARSET_UJIS
#define USE_MB
#define	USE_MB_IDENT
#define isujis(c)     ((0xa1<=((c)&0xff) && ((c)&0xff)<=0xfe))
#define iskata(c)     ((0xa1<=((c)&0xff) && ((c)&0xff)<=0xdf))
#define isujis_ss2(c) (((c)&0xff) == 0x8e)
#define isujis_ss3(c) (((c)&0xff) == 0x8f)
#define ismbchar(p, end)	((*(uchar*)(p)<0x80)? 0:\
	isujis(*(p)) && (end)-(p)>1 && isujis(*((p)+1))? 2:\
	isujis_ss2(*(p)) && (end)-(p)>1 && iskata(*((p)+1))? 2:\
	isujis_ss3(*(p)) && (end)-(p)>2 && isujis(*((p)+1)) && isujis(*((p)+2))? 3:\
	0)
#define ismbhead(c)	(isujis(c) || isujis_ss2(c) || isujis_ss3(c))
#define mbcharlen(c)	(isujis(c)? 2: isujis_ss2(c)? 2: isujis_ss3(c)? 3: 0)
#define MBMAXLEN	3
#endif

/* Support for Japanese(SJIS) characters, by tommy@valley.ne.jp */
#if MY_CHARSET_CURRENT == MY_CHARSET_SJIS
#define USE_MB
#define USE_MB_IDENT
#define issjishead(c) ((0x81<=((c)&0xff) && ((c)&0xff)<=0x9f) || (0xe0<=((c)&0xff) && ((c)&0xff)<=0xfc))
#define issjistail(c) ((0x40<=((c)&0xff) && ((c)&0xff)<=0x7e) || (0x80<=((c)&0xff) && ((c)&0xff)<=0xfc))
#define ismbchar(p, end)	(issjishead(*(p)) && (end)-(p)>1 && issjistail(*((p)+1))? 2: 0)
#define ismbhead(c)	issjishead(c)
#define mbcharlen(c)	(issjishead(c)? 2: 0)
#define MBMAXLEN	2
#endif

/* Support for Chinese(BIG5) characters, by jou@nematic.ieo.nctu.edu.tw
   modified by Wei He (hewei@mail.ied.ac.cn) */

#if MY_CHARSET_CURRENT == MY_CHARSET_BIG5
#define USE_MB
#define USE_MB_IDENT
#define isbig5head(c) (0xa1<=(uchar)(c) && (uchar)(c)<=0xf9)
#define isbig5tail(c) ((0x40<=(uchar)(c) && (uchar)(c)<=0x7e) || \
                      (0xa1<=(uchar)(c) && (uchar)(c)<=0xfe))
#define ismbchar(p, end)  (isbig5head(*(p)) && (end)-(p)>1 && isbig5tail(*((p)+1))? 2: 0)
#define ismbhead(c)     isbig5head(c)
#define mbcharlen(c)    (isbig5head(c)? 2: 0)
#define MBMAXLEN        2
#
#undef USE_STRCOLL
#define USE_STRCOLL
#endif

/* Support for Chinese(GB2312) characters, by Miles Tsai (net-bull@126.com)
  modified by Wei He (hewei@mail.ied.ac.cn) */

#if MY_CHARSET_CURRENT == MY_CHARSET_GB2312
#define USE_MB
#define USE_MB_IDENT
#define isgb2312head(c) (0xa1<=(uchar)(c) && (uchar)(c)<=0xf7)
#define isgb2312tail(c) (0xa1<=(uchar)(c) && (uchar)(c)<=0xfe)
#define ismbchar(p, end)  (isgb2312head(*(p)) && (end)-(p)>1 && isgb2312tail(*((p)+1))? 2: 0)
#define ismbhead(c)     isgb2312head(c)
#define mbcharlen(c)    (isgb2312head(c)? 2:0)
#define MBMAXLEN        2
#endif

/* Support for Chinese(GBK) characters, by hewei@mail.ied.ac.cn */

#if MY_CHARSET_CURRENT == MY_CHARSET_GBK
#define USE_MB
#define USE_MB_IDENT
#define isgbkhead(c) (0x81<=(uchar)(c) && (uchar)(c)<=0xfe)
#define isgbktail(c) ((0x40<=(uchar)(c) && (uchar)(c)<=0x7e) || \
                          (0x80<=(uchar)(c) && (uchar)(c)<=0xfe))
#define ismbchar(p, end)  (isgbkhead(*(p)) && (end)-(p)>1 && isgbktail(*((p)+1))? 2: 0)
#define ismbhead(c)     isgbkhead(c)
#define mbcharlen(c)    (isgbkhead(c)? 2:0)
#define MBMAXLEN        2
#undef USE_STRCOLL
#define USE_STRCOLL
#endif

/* Define, how much will the string grow under strxfrm */
#if MY_CHARSET_CURRENT == MY_CHARSET_CZECH
#undef USE_STRCOLL
#define USE_STRCOLL
#endif
#if MY_CHARSET_CURRENT == MY_CHARSET_TIS620
#undef USE_STRCOLL
#define USE_STRCOLL
#define USE_TIS620
#include "t_ctype.h"
#endif

/* Support for Korean(EUC_KR) characters, by powerm90@tinc.co.kr and mrpark@tinc.co.kr */
#if MY_CHARSET_CURRENT == MY_CHARSET_EUC_KR
#define USE_MB
#define USE_MB_IDENT
#define iseuc_kr(c)     ((0xa1<=(uchar)(c) && (uchar)(c)<=0xfe))
#define ismbchar(p, end)        ((*(uchar*)(p)<0x80)? 0:\
        iseuc_kr(*(p)) && (end)-(p)>1 && iseuc_kr(*((p)+1))? 2:\
        0)
#define ismbhead(c)     (iseuc_kr(c))
#define mbcharlen(c)    (iseuc_kr(c) ? 2 : 0)
#define MBMAXLEN        2
#endif

#ifdef USE_STRCOLL
extern uint MY_STRXFRM_MULTIPLY;
extern int my_strnxfrm(unsigned char *, unsigned char *, int, int);
extern int my_strnncoll(const unsigned char *, int, const unsigned char *, int);
extern int my_strxfrm(unsigned char *, unsigned char *, int);
extern int my_strcoll(const unsigned char *, const unsigned char *);
extern my_bool my_like_range(const char *ptr,uint ptr_length,pchar escape,
			     uint res_length, char *min_str,char *max_str,
			     uint *min_length,uint *max_length);
#endif

#ifdef	__cplusplus
}
#endif

#endif /* _m_ctype_h */
