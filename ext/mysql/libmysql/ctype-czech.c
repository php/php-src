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

/* File strings/ctype-czech.c for MySQL.

	This file implements the Czech sorting for the MySQL database
	server (www.mysql.com). Due to some complicated rules the
	Czech language has for sorting strings, a more complex
	solution was needed than the one-to-one conversion table. To
	note a few, here is an example of a Czech sorting sequence:

		co < hlaska < hláska < hlava < chlapec < krtek

	It because some of the rules are: double char 'ch' is sorted
	between 'h' and 'i'. Accented character 'á' (a with acute) is
	sorted after 'a' and before 'b', but only if the word is
	otherwise the same. However, because 's' is sorted before 'v'
	in hlava, the accentness of 'á' is overridden. There are many
	more rules.

	This file defines functions my_strxfrm and my_strcoll for
	C-like zero terminated strings and my_strnxfrm and my_strnncoll
	for strings where the length comes as an parameter. Also
	defined here you will find function my_like_range that returns
	index range strings for LIKE expression and the
	MY_STRXFRM_MULTIPLY set to value 4 -- this is the ratio the
	strings grows during my_strxfrm. The algorithm has four
	passes, that's why we need four times more space for expanded
	string.

	This file also contains the ISO-Latin-2 definitions of
	characters.

	Author: (c) 1997--1998 Jan Pazdziora, adelton@fi.muni.cz
	Jan Pazdziora has a shared copyright for this code

	The original of this file can also be found at
	http://www.fi.muni.cz/~adelton/l10n/

	Bug reports and suggestions are always welcome.
*/

/*
 * This comment is parsed by configure to create ctype.c,
 * so don't change it unless you know what you are doing.
 *
 * .configure. strxfrm_multiply_czech=4
 */

#define SKIP_TRAILING_SPACES 1

#define REAL_MYSQL

#ifdef REAL_MYSQL

#include <global.h>
#include "m_string.h"

#else

#include <stdio.h>
#define uchar unsigned char

#endif

/*
	These are four tables for four passes of the algorithm. Please see
	below for what are the "special values"
*/

static uchar * CZ_SORT_TABLE[] = {
  (uchar*) "\000\000\000\000\000\000\000\000\000\002\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\002\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\043\044\045\046\047\050\051\052\053\054\000\000\000\000\000\000\000\003\004\377\007\010\011\012\013\015\016\017\020\022\023\024\025\026\027\031\033\034\035\036\037\040\041\000\000\000\000\000\000\003\004\377\007\010\011\012\013\015\016\017\020\022\023\024\025\026\027\031\033\034\035\036\037\040\041\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\003\000\021\000\020\032\000\000\032\032\033\042\000\042\042\000\003\000\021\000\020\032\000\000\032\032\033\042\000\042\042\027\003\003\003\003\020\006\006\006\010\010\010\010\015\015\007\007\023\023\024\024\024\024\000\030\034\034\034\034\040\033\000\027\003\003\003\003\020\006\006\006\010\010\010\010\015\015\007\007\023\023\024\024\024\024\000\030\034\034\034\034\040\033\000",
  (uchar*) "\000\000\000\000\000\000\000\000\000\002\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\002\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\106\107\110\111\112\113\114\115\116\117\000\000\000\000\000\000\000\003\011\377\016\021\026\027\030\032\035\036\037\043\044\047\054\055\056\061\065\070\075\076\077\100\102\000\000\000\000\000\000\003\011\377\016\021\026\027\030\032\035\036\037\043\044\047\054\055\056\061\065\070\075\076\077\100\102\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\010\000\042\000\041\063\000\000\062\064\066\104\000\103\105\000\010\000\042\000\041\063\000\000\062\064\066\104\000\103\105\057\004\005\007\006\040\014\015\013\022\025\024\023\033\034\017\020\046\045\050\051\053\052\000\060\072\071\074\073\101\067\000\057\004\005\007\006\040\014\015\013\022\025\024\023\033\034\017\020\046\045\050\051\053\052\000\060\072\071\074\073\101\067\000",
(uchar*) "\000\000\000\000\000\000\000\000\000\002\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\002\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\212\213\214\215\216\217\220\221\222\223\000\000\000\000\000\000\000\004\020\377\032\040\052\054\056\063\071\073\075\105\107\115\127\131\133\141\151\157\171\173\175\177\203\000\000\000\000\000\000\003\017\377\031\037\051\053\055\062\070\072\074\104\106\114\126\130\132\140\150\156\170\172\174\176\202\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\016\000\103\000\101\145\000\000\143\147\153\207\000\205\211\000\015\000\102\000\100\144\000\000\142\146\152\206\000\204\210\135\006\010\014\012\077\026\030\024\042\050\046\044\065\067\034\036\113\111\117\121\125\123\000\137\163\161\167\165\201\155\000\134\005\007\013\011\076\025\027\023\041\047\045\043\064\066\033\035\112\110\116\120\124\122\000\136\162\160\166\164\200\154\000",
(uchar*) "\264\265\266\267\270\271\272\273\274\002\276\277\300\301\302\303\304\305\306\307\310\311\312\313\314\315\316\317\320\321\322\323\002\230\232\253\324\252\251\234\240\241\261\260\225\262\224\235\212\213\214\215\216\217\220\221\222\223\231\226\244\257\245\227\250\004\020\377\032\040\052\054\056\063\071\073\075\105\107\115\127\131\133\141\151\157\171\173\175\177\203\242\237\243\254\255\233\003\017\377\031\037\051\053\055\062\070\072\074\104\106\114\126\130\132\140\150\156\170\172\174\176\202\246\236\247\256\325\264\265\266\267\270\271\272\273\274\275\276\277\300\301\302\303\304\305\306\307\310\311\312\313\314\315\316\317\320\321\322\323\326\016\327\103\330\101\145\331\332\143\147\153\207\333\205\211\334\015\335\102\336\100\144\337\340\142\146\152\206\341\204\210\135\006\010\014\012\077\026\030\024\042\050\046\044\065\067\034\036\113\111\117\121\125\123\263\137\163\161\167\165\201\155\342\134\005\007\013\011\076\025\027\023\041\047\045\043\064\066\033\035\112\110\116\120\124\122\343\136\162\160\166\164\200\154\344",
};

/*
	These define the valuse for the double chars that need to be
	sorted as they were single characters -- in Czech these are
	'ch', 'Ch' and 'CH'.
*/

struct wordvalue
	{
	  const char * word;
	  uchar * outvalue;
	};
static struct wordvalue doubles[] = {
	{ "ch", (uchar*) "\014\031\057\057" },
	{ "Ch", (uchar*) "\014\031\060\060" },
	{ "CH", (uchar*) "\014\031\061\061" },
	{ "c",  (uchar*) "\005\012\021\021" },
	{ "C",  (uchar*) "\005\012\022\022" },
	};

/*
	Unformal description of the algorithm:

	We walk the string left to right.

	The end of the string is either passed as parameter, or is
	*p == 0. This is hidden in the IS_END macro.

	In the first two passes, we compare word by word. So we make
	first and second pass on the first word, first and second pass
	on the second word, etc. If we come to the end of the string
	during the first pass, we need to jump to the last word of the
	second pass.

	End of pass is marked with value 1 on the output.

	For each character, we read it's value from the table.

	If the value is ignore (0), we go straight to the next character.

	If the value is space/end of word (2) and we are in the first
	or second pass, we skip all characters having value 0 -- 2 and
	switch the passwd.

	If it's the compose character (255), we check if the double
	exists behind it, find its value.

	We append 0 to the end.
---
	Neformální popis algoritmu:

	Procházíme øetìzec zleva doprava.

	Konec øetìzce je pøedán buï jako parametr, nebo je to *p == 0.
	Toto je o¹etøeno makrem IS_END.

	Pokud jsme do¹li na konec øetìzce pøi prùchodu 0, nejdeme na
	zaèátek, ale na ulo¾enou pozici, proto¾e první a druhý prùchod
	bì¾í souèasnì.

	Konec vstupu (prùchodu) oznaèíme na výstupu hodnotou 1.

	Pro ka¾dý znak øetìzce naèteme hodnotu z tøídící tabulky.

	Jde-li o hodnotu ignorovat (0), skoèíme ihned na dal¹í znak..

	Jde-li o hodnotu konec slova (2) a je to prùchod 0 nebo 1,
	pøeskoèíme v¹echny dal¹í 0 -- 2 a prohodíme prùchody.

	Jde-li o kompozitní znak (255), otestujeme, zda následuje
	správný do dvojice, dohledáme správnou hodnotu.

	Na konci pøipojíme znak 0
 */

#define ADD_TO_RESULT(dest, len, totlen, value)				\
	if ((totlen) < (len)) { dest[totlen] = value; } (totlen++);

#define NEXT_CMP_VALUE(src, p, store, pass, value, len)			\
	while (1)		/* we will make a loop */		\
		{							\
		if (IS_END(p, src, len))				\
				/* when we are at the end of string */	\
			{	/* return either 0 for end of string */	\
					/* or 1 for end of pass */	\
			if (pass == 3)	{ value = 0; break; }		\
			if (pass == 0)	p = store;			\
			else		p = src;			\
			value = 1; pass++; break;			\
			}						\
					/* not at end of string */	\
		value = CZ_SORT_TABLE[pass][*p];			\
									\
		if (value == 0)	{ p++; continue; } /* ignore value */	\
		if (value == 2)		/* space */			\
			{						\
			const uchar * tmp;				\
			const uchar * runner = ++p;			\
			while (!(IS_END(runner, src, len)) && (CZ_SORT_TABLE[pass][*runner] == 2))							\
				runner++;	/* skip all spaces */	\
			if (IS_END(runner, src, len) && SKIP_TRAILING_SPACES)										\
				p = runner;				\
			if ((pass <= 2) && !(IS_END(runner, src, len)))	\
				p = runner;				\
			if (IS_END(p, src, len))			\
				continue;				\
				/* we switch passes */			\
			if (pass > 1)					\
				break;					\
			tmp = p;					\
			if (pass == 0)	pass = 1;			\
			else		pass = 0;			\
			p = store; store = tmp;				\
			break;						\
			}						\
		if (value == 255)					\
			{						\
			int i;						\
			for (i = 0; i < (int) sizeof(doubles); i++)	\
				{					\
				const char * pattern = doubles[i].word;	\
				const char * q = (const char *) p;	\
				int j = 0;				\
				while (pattern[j])			\
					{				\
					if (IS_END(q, src, len) || (*q != pattern[j]))									\
						{ break ; }		\
					j++; q++;			\
					}				\
				if (!(pattern[j]))			\
					{				\
					value = (int)(doubles[i].outvalue[pass]);									\
					p = (const uchar *) q - 1;			\
					break;				\
					}				\
				}					\
			}						\
		p++;							\
		break;							\
		}

#define IS_END(p, src, len)	(!(*p))

/* Function strcoll, with Czech sorting, for zero terminated strings */
int my_strcoll_czech(const uchar * s1, const uchar * s2)
	{
	int v1, v2;
	const uchar * p1, * p2, * store1, * store2;
	int pass1 = 0, pass2 = 0;
	int diff;

	p1 = s1;	p2 = s2;
	store1 = s1;	store2 = s2;

	do
		{
		NEXT_CMP_VALUE(s1, p1, store1, pass1, v1, 0);
		NEXT_CMP_VALUE(s2, p2, store2, pass2, v2, 0);
		diff = v1 - v2;
		if (diff != 0)		return diff;
		}
	while (v1);
	return 0;
	}

/* Function strxfrm, with Czech sorting, for zero terminated strings */
int my_strxfrm_czech(uchar * dest, const uchar * src, int len)
{
	int value;
	const uchar * p, * store;
	int pass = 0;
	int totlen = 0;
	p = store = src;

	do
		{
		NEXT_CMP_VALUE(src, p, store, pass, value, 0);
		ADD_TO_RESULT(dest, len, totlen, value);
		}
	while (value);
	return totlen;
	}

#undef IS_END




#define IS_END(p, src, len)	(((char *)p - (char *)src) >= (len))

/* Function strnncoll, actually strcoll, with Czech sorting, which expect
   the length of the strings being specified */
int my_strnncoll_czech(const uchar * s1, int len1, const uchar * s2, int len2)
	{
	int v1, v2;
	const uchar * p1, * p2, * store1, * store2;
	int pass1 = 0, pass2 = 0;
	int diff;

	p1 = s1;	p2 = s2;
	store1 = s1;	store2 = s2;

	do
		{
		NEXT_CMP_VALUE(s1, p1, store1, pass1, v1, len1);
		NEXT_CMP_VALUE(s2, p2, store2, pass2, v2, len2);
		diff = v1 - v2;

		if (diff != 0)		return diff;
		}
	while (v1);
	return 0;
	}

/* Function strnxfrm, actually strxfrm, with Czech sorting, which expect
   the length of the strings being specified */
int my_strnxfrm_czech(uchar * dest, const uchar * src, int len, int srclen)
	{
	int value;
	const uchar * p, * store;
	int pass = 0;
	int totlen = 0;
	p = src;	store = src;

	do
		{
		NEXT_CMP_VALUE(src, p, store, pass, value, srclen);
		ADD_TO_RESULT(dest, len, totlen, value);
		}
	while (value);
	return totlen;
	}

#undef IS_END


/*
	Neformální popis algoritmu:

	procházíme øetìzec zleva doprava
	konec øetìzce poznáme podle *p == 0
	pokud jsme do¹li na konec øetìzce pøi prùchodu 0, nejdeme na
		zaèátek, ale na ulo¾enou pozici, proto¾e první a druhý
		prùchod bì¾í souèasnì
	konec vstupu (prùchodu) oznaèíme na výstupu hodnotou 1

	naèteme hodnotu z tøídící tabulky
	jde-li o hodnotu ignorovat (0), skoèíme na dal¹í prùchod
	jde-li o hodnotu konec slova (2) a je to prùchod 0 nebo 1,
		pøeskoèíme v¹echny dal¹í 0 -- 2 a prohodíme
		prùchody
	jde-li o kompozitní znak (255), otestujeme, zda následuje
		správný do dvojice, dohledáme správnou hodnotu

	na konci pøipojíme znak 0
 */


/*
** Calculate min_str and max_str that ranges a LIKE string.
** Arguments:
** ptr		Pointer to LIKE string.
** ptr_length	Length of LIKE string.
** escape	Escape character in LIKE.  (Normally '\').
**		All escape characters should be removed from min_str and max_str
** res_length   Length of min_str and max_str.
** min_str      Smallest case sensitive string that ranges LIKE.
**		Should be space padded to res_length.
** max_str	Largest case sensitive string that ranges LIKE.
**		Normally padded with the biggest character sort value.
**
** The function should return 0 if ok and 1 if the LIKE string can't be
** optimized !
*/

#ifdef REAL_MYSQL

#define min_sort_char ' '
#define max_sort_char '9'
#define wild_one '_'
#define wild_many '%'

#define EXAMPLE

my_bool my_like_range_czech(const char *ptr,uint ptr_length,pchar escape,
		            uint res_length, char *min_str,char *max_str,
		            uint *min_length,uint *max_length)
{
#ifdef EXAMPLE
  uchar value;
  const char *end=ptr+ptr_length;
  char *min_org=min_str;
  char *min_end=min_str+res_length;

  for (; ptr != end && min_str != min_end ; ptr++)
  {
    if (*ptr == wild_one)		/* '_' in SQL */
    { break; }
    if (*ptr == wild_many)		/* '%' in SQL */
    { break; }

    if (*ptr == escape && ptr+1 != end)
    { ptr++; }			/* Skip escape */

    value = CZ_SORT_TABLE[0][(int) (uchar) *ptr];

    if (value == 0)			/* Ignore in the first pass */
    { continue; }
    if (value <= 2)			/* End of pass or end of string */
    { break; }
    if (value == 255)		/* Double char too compicated */
    { break; }

    *min_str++= *max_str++ = *ptr;
  }
  *min_length= (uint) (min_str - min_org);
  *max_length= res_length;
  while (min_str != min_end)
  {
    *min_str++ = min_sort_char;	/* Because of key compression */
    *max_str++ = max_sort_char;
  }
  return 0;
#else
  return 1;
#endif
}

#endif

#ifdef REAL_MYSQL
/* This is a latin2 file */

/*
 * File generated by cset
 * (C) Abandoned 1997 Zarko Mocnik <zarko.mocnik@dem.si>
 *
 * definition table reworked by Jaromir Dolecek <dolecek@ics.muni.cz>
 */
#include <global.h>
#include "m_string.h"

uchar NEAR ctype_czech[257] = {
0,
 32, 32, 32, 32, 32, 32, 32, 32, 32, 40, 40, 40, 40, 40, 32, 32,
 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
 72, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
132,132,132,132,132,132,132,132,132,132, 16, 16, 16, 16, 16, 16,
 16,129,129,129,129,129,129,  1,  1,  1,  1,  1,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 16, 16, 16, 16, 16,
 16,130,130,130,130,130,130,  2,  2,  2,  2,  2,  2,  2,  2,  2,
  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 16, 16, 16, 16, 32,
 32, 32, 32, 32, 32, 32, 32, 32, 40, 40, 40, 40, 40, 32, 32, 32,
 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 72,
  1, 16,  1, 16,  1,  1, 16,  0,  0,  1,  1,  1,  1, 16,  1,  1,
 16,  2, 16,  2, 16,  2,  2, 16, 16,  2,  2,  2,  2, 16,  2,  2,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
 16,  1,  1,  1,  1,  1,  1, 16,  1,  1,  1,  1,  1,  1,  1, 16,
  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
  2,  2,  2,  2,  2,  2,  2, 16,  2,  2,  2,  2,  2,  2,  2, 16,
};

uchar NEAR to_lower_czech[] = {
  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
 64, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,
112,113,114,115,116,117,118,119,120,121,122, 91, 92, 93, 94, 95,
 96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,
112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,
128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
177,161,179,163,181,182,166,167,168,185,186,187,188,173,190,191,
176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
208,241,242,243,244,245,246,215,248,249,250,251,252,253,254,223,
224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,
};

uchar NEAR to_upper_czech[] = {
  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
 96, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,123,124,125,126,127,
128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
176,160,178,162,180,164,165,183,184,169,170,171,172,189,174,175,
192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
240,209,210,211,212,213,214,247,216,217,218,219,220,221,222,255,
};

uchar NEAR sort_order_czech[] = {
  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
 64, 65, 71, 72, 76, 78, 83, 84, 85, 86, 90, 91, 92, 96, 97,100,
105,106,107,110,114,117,122,123,124,125,127,131,132,133,134,135,
136, 65, 71, 72, 76, 78, 83, 84, 85, 86, 90, 91, 92, 96, 97,100,
105,106,107,110,114,117,122,123,124,125,127,137,138,139,140,  0,
  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16,
 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,255,
 66,255, 93,255, 94,111,255,255,255,112,113,115,128,255,129,130,
255, 66,255, 93,255, 94,111,255,255,112,113,115,128,255,129,130,
108, 67, 68, 69, 70, 95, 73, 75, 74, 79, 81, 82, 80, 89, 87, 77,
255, 98, 99,101,102,103,104,255,109,119,118,120,121,126,116,255,
108, 67, 68, 69, 70, 95, 73, 75, 74, 79, 81, 82, 80, 89, 88, 77,
255, 98, 99,101,102,103,104,255,109,119,118,120,121,126,116,255,
};

#endif
