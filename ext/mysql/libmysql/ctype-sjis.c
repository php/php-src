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

/* This file is for Shift JIS charset, and created by tommy@valley.ne.jp.
 */

#include <global.h>
#include "m_string.h"
#include "m_ctype.h"

/*
 * This comment is parsed by configure to create ctype.c,
 * so don't change it unless you know what you are doing.
 *
 * .configure. strxfrm_multiply_sjis=1
 * .configure. mbmaxlen_sjis=2
 */

uchar NEAR ctype_sjis[257] =
{
    0,				/* For standard library */
    0040, 0040, 0040, 0040, 0040, 0040, 0040, 0040,	/* NUL ^A - ^G */
    0040, 0050, 0050, 0050, 0050, 0050, 0040, 0040,	/* ^H - ^O */
    0040, 0040, 0040, 0040, 0040, 0040, 0040, 0040,	/* ^P - ^W */
    0040, 0040, 0040, 0040, 0040, 0040, 0040, 0040,	/* ^X - ^Z ^[ ^\ ^] ^^ ^_ */
    0110, 0020, 0020, 0020, 0020, 0020, 0020, 0020,	/* SPC ! " # $ % ^ ' */
    0020, 0020, 0020, 0020, 0020, 0020, 0020, 0020,	/* ( ) * + , - . / */
    0204, 0204, 0204, 0204, 0204, 0204, 0204, 0204,	/* 0 1 2 3 4 5 6 7 */
    0204, 0204, 0020, 0020, 0020, 0020, 0020, 0020,	/* 8 9 : ; < = > ? */
    0020, 0201, 0201, 0201, 0201, 0201, 0201, 0001,	/* @ A B C D E F G */
    0001, 0001, 0001, 0001, 0001, 0001, 0001, 0001,	/* H I J K L M N O */
    0001, 0001, 0001, 0001, 0001, 0001, 0001, 0001,	/* P Q R S T U V W */
    0001, 0001, 0001, 0020, 0020, 0020, 0020, 0020,	/* X Y Z [ \ ] ^ _ */
    0020, 0202, 0202, 0202, 0202, 0202, 0202, 0002,	/* ` a b c d e f g */
    0002, 0002, 0002, 0002, 0002, 0002, 0002, 0002,	/* h i j k l m n o */
    0002, 0002, 0002, 0002, 0002, 0002, 0002, 0002,	/* p q r s t u v w */
    0002, 0002, 0002, 0020, 0020, 0020, 0020, 0040,	/* x y z { | } + DEL */
    0020, 0020, 0020, 0020, 0020, 0020, 0020, 0020,
    0020, 0020, 0020, 0020, 0020, 0020, 0020, 0020,
    0020, 0020, 0020, 0020, 0020, 0020, 0020, 0020,
    0020, 0020, 0020, 0020, 0020, 0020, 0020, 0020,
    0020, 0020, 0020, 0020, 0020, 0020, 0020, 0020,
    0020, 0020, 0020, 0020, 0020, 0020, 0020, 0020,
    0020, 0020, 0020, 0020, 0020, 0020, 0020, 0020,
    0020, 0020, 0020, 0020, 0020, 0020, 0020, 0020,
    0020, 0020, 0020, 0020, 0020, 0020, 0020, 0020,
    0020, 0020, 0020, 0020, 0020, 0020, 0020, 0020,
    0020, 0020, 0020, 0020, 0020, 0020, 0020, 0020,
    0020, 0020, 0020, 0020, 0020, 0020, 0020, 0020,
    0020, 0020, 0020, 0020, 0020, 0020, 0020, 0020,
    0020, 0020, 0020, 0020, 0020, 0020, 0020, 0020,
    0020, 0020, 0020, 0020, 0020, 0020, 0020, 0020,
    0020, 0020, 0020, 0020, 0020, 0000, 0000, 0000
};

uchar NEAR to_lower_sjis[]=
{
  '\000','\001','\002','\003','\004','\005','\006','\007',
  '\010','\011','\012','\013','\014','\015','\016','\017',
  '\020','\021','\022','\023','\024','\025','\026','\027',
  '\030','\031','\032','\033','\034','\035','\036','\037',
  ' ',	 '!',	'"',   '#',   '$',   '%',   '&',   '\'',
  '(',	 ')',	'*',   '+',   ',',   '-',   '.',   '/',
  '0',	 '1',	'2',   '3',   '4',   '5',   '6',   '7',
  '8',	 '9',	':',   ';',   '<',   '=',   '>',   '?',
  '@',	 'a',	'b',   'c',   'd',   'e',   'f',   'g',
  'h',	 'i',	'j',   'k',   'l',   'm',   'n',   'o',
  'p',	 'q',	'r',   's',   't',   'u',   'v',   'w',
  'x',	 'y',	'z',   '[',   '\\',  ']',   '^',   '_',
  '`',	 'a',	'b',   'c',   'd',   'e',   'f',   'g',
  'h',	 'i',	'j',   'k',   'l',   'm',   'n',   'o',
  'p',	 'q',	'r',   's',   't',   'u',   'v',   'w',
  'x',	 'y',	'z',   '{',   '|',   '}',   '~',   '\177',
  '\200','\201','\202','\203','\204','\205','\206','\207',
  '\210','\211','\212','\213','\214','\215','\216','\217',
  '\220','\221','\222','\223','\224','\225','\226','\227',
  '\230','\231','\232','\233','\234','\235','\236','\237',
  '\240','\241','\242','\243','\244','\245','\246','\247',
  '\250','\251','\252','\253','\254','\255','\256','\257',
  '\260','\261','\262','\263','\264','\265','\266','\267',
  '\270','\271','\272','\273','\274','\275','\276','\277',
  '\300','\301','\302','\303','\304','\305','\306','\307',
  '\310','\311','\312','\313','\314','\315','\316','\317',
  '\320','\321','\322','\323','\324','\325','\326','\327',
  '\330','\331','\332','\333','\334','\335','\336','\337',
  '\340','\341','\342','\343','\344','\345','\346','\347',
  '\350','\351','\352','\353','\354','\355','\356','\357',
  '\360','\361','\362','\363','\364','\365','\366','\367',
  '\370','\371','\372','\373','\374','\375','\376','\377'
};

uchar NEAR to_upper_sjis[]=
{
  '\000','\001','\002','\003','\004','\005','\006','\007',
  '\010','\011','\012','\013','\014','\015','\016','\017',
  '\020','\021','\022','\023','\024','\025','\026','\027',
  '\030','\031','\032','\033','\034','\035','\036','\037',
  ' ',	 '!',	'"',   '#',   '$',   '%',   '&',   '\'',
  '(',	 ')',	'*',   '+',   ',',   '-',   '.',   '/',
  '0',	 '1',	'2',   '3',   '4',   '5',   '6',   '7',
  '8',	 '9',	':',   ';',   '<',   '=',   '>',   '?',
  '@',	 'A',	'B',   'C',   'D',   'E',   'F',   'G',
  'H',	 'I',	'J',   'K',   'L',   'M',   'N',   'O',
  'P',	 'Q',	'R',   'S',   'T',   'U',   'V',   'W',
  'X',	 'Y',	'Z',   '[',   '\\',  ']',   '^',   '_',
  '`',	 'A',	'B',   'C',   'D',   'E',   'F',   'G',
  'H',	 'I',	'J',   'K',   'L',   'M',   'N',   'O',
  'P',	 'Q',	'R',   'S',   'T',   'U',   'V',   'W',
  'X',	 'Y',	'Z',   '{',   '|',   '}',   '~',   '\177',
  '\200','\201','\202','\203','\204','\205','\206','\207',
  '\210','\211','\212','\213','\214','\215','\216','\217',
  '\220','\221','\222','\223','\224','\225','\226','\227',
  '\230','\231','\232','\233','\234','\235','\236','\237',
  '\240','\241','\242','\243','\244','\245','\246','\247',
  '\250','\251','\252','\253','\254','\255','\256','\257',
  '\260','\261','\262','\263','\264','\265','\266','\267',
  '\270','\271','\272','\273','\274','\275','\276','\277',
  '\300','\301','\302','\303','\304','\305','\306','\307',
  '\310','\311','\312','\313','\314','\315','\316','\317',
  '\320','\321','\322','\323','\324','\325','\326','\327',
  '\330','\331','\332','\333','\334','\335','\336','\337',
  '\340','\341','\342','\343','\344','\345','\346','\347',
  '\350','\351','\352','\353','\354','\355','\356','\357',
  '\360','\361','\362','\363','\364','\365','\366','\367',
  '\370','\371','\372','\373','\374','\375','\376','\377'
};

uchar NEAR sort_order_sjis[]=
{
  '\000','\001','\002','\003','\004','\005','\006','\007',
  '\010','\011','\012','\013','\014','\015','\016','\017',
  '\020','\021','\022','\023','\024','\025','\026','\027',
  '\030','\031','\032','\033','\034','\035','\036','\037',
  ' ',	 '!',	'"',   '#',   '$',   '%',   '&',   '\'',
  '(',	 ')',	'*',   '+',   ',',   '-',   '.',   '/',
  '0',	 '1',	'2',   '3',   '4',   '5',   '6',   '7',
  '8',	 '9',	':',   ';',   '<',   '=',   '>',   '?',
  '@',	 'A',	'B',   'C',   'D',   'E',   'F',   'G',
  'H',	 'I',	'J',   'K',   'L',   'M',   'N',   'O',
  'P',	 'Q',	'R',   'S',   'T',   'U',   'V',   'W',
  'X',	 'Y',	'Z',   '[',   '\\',  ']',   '^',   '_',
  '`',	 'A',	'B',   'C',   'D',   'E',   'F',   'G',
  'H',	 'I',	'J',   'K',   'L',   'M',   'N',   'O',
  'P',	 'Q',	'R',   'S',   'T',   'U',   'V',   'W',
  'X',	 'Y',	'Z',   '{',   '|',   '}',   '~',   '\177',
  '\200','\201','\202','\203','\204','\205','\206','\207',
  '\210','\211','\212','\213','\214','\215','\216','\217',
  '\220','\221','\222','\223','\224','\225','\226','\227',
  '\230','\231','\232','\233','\234','\235','\236','\237',
  '\240','\241','\242','\243','\244','\245','\246','\247',
  '\250','\251','\252','\253','\254','\255','\256','\257',
  '\260','\261','\262','\263','\264','\265','\266','\267',
  '\270','\271','\272','\273','\274','\275','\276','\277',
  '\300','\301','\302','\303','\304','\305','\306','\307',
  '\310','\311','\312','\313','\314','\315','\316','\317',
  '\320','\321','\322','\323','\324','\325','\326','\327',
  '\330','\331','\332','\333','\334','\335','\336','\337',
  '\340','\341','\342','\343','\344','\345','\346','\347',
  '\350','\351','\352','\353','\354','\355','\356','\357',
  '\360','\361','\362','\363','\364','\365','\366','\367',
  '\370','\371','\372','\373','\374','\375','\376','\377'
};

#define issjishead(c) ((0x81<=(c) && (c)<=0x9f) || \
                       ((0xe0<=(c)) && (c)<=0xfc))
#define issjistail(c) ((0x40<=(c) && (c)<=0x7e) || \
                       (0x80<=(c) && (c)<=0xfc))


int ismbchar_sjis(const char* p, const char *e)
{
  return (issjishead((uchar) *p) && (e-p)>1 && issjistail((uchar)p[1]) ? 2: 0);
}

my_bool ismbhead_sjis(uint c)
{
  return issjishead((uchar) c);
}

int mbcharlen_sjis(uint c)
{
  return (issjishead((uchar) c) ? 2: 0);
}


#define sjiscode(c,d)	((((uint) (uchar)(c)) << 8) | (uint) (uchar) (d))

int my_strnncoll_sjis(const uchar *s1, int len1, const uchar *s2, int len2)
{
  const uchar *e1 = s1 + len1;
  const uchar *e2 = s2 + len2;
  while (s1 < e1 && s2 < e2) {
    if (ismbchar_sjis((char*) s1, (char*) e1) &&
	ismbchar_sjis((char*) s2, (char*) e2)) {
      uint c1 = sjiscode(*s1, *(s1+1));
      uint c2 = sjiscode(*s2, *(s2+1));
      if (c1 != c2)
	return c1 - c2;
      s1 += 2;
      s2 += 2;
    } else {
      if (sort_order_sjis[(uchar)*s1] != sort_order_sjis[(uchar)*s2])
	return sort_order_sjis[(uchar)*s1] - sort_order_sjis[(uchar)*s2];
      s1++;
      s2++;
    }
  }
  return len1 - len2;
}

int my_strcoll_sjis(const uchar *s1, const uchar *s2)
{
  return (uint) my_strnncoll_sjis(s1,(uint) strlen((char*) s1),
				  s2,(uint) strlen((char*) s2));
}

int my_strnxfrm_sjis(uchar *dest, uchar *src, int len, int srclen)
{
  uchar *d_end = dest + len;
  uchar *s_end = src + srclen;
  while (dest < d_end && src < s_end) {
    if (ismbchar_sjis((char*) src, (char*) s_end)) {
      *dest++ = *src++;
      if (dest < d_end && src < s_end)
	*dest++ = *src++;
    } else {
      *dest++ = sort_order_sjis[(uchar)*src++];
    }
  }
  return srclen;
}

int my_strxfrm_sjis(uchar *dest, uchar *src, int len)
{
  return my_strnxfrm_sjis(dest, src, len, (uint) strlen((char*) src));
}


/*
** Calculate min_str and max_str that ranges a LIKE string.
** Arguments:
** ptr		Pointer to LIKE string.
** ptr_length	Length of LIKE string.
** escape	Escape character in LIKE.  (Normally '\').
**		All escape characters should be removed from min_str and max_str
** res_length	Length of min_str and max_str.
** min_str	Smallest case sensitive string that ranges LIKE.
**		Should be space padded to res_length.
** max_str	Largest case sensitive string that ranges LIKE.
**		Normally padded with the biggest character sort value.
**
** The function should return 0 if ok and 1 if the LIKE string can't be
** optimized !
*/

#define max_sort_char ((char) 255)
#define wild_one '_'
#define wild_many '%'

my_bool my_like_range_sjis(const char *ptr,uint ptr_length,pchar escape,
                      uint res_length, char *min_str,char *max_str,
                      uint *min_length,uint *max_length)
{
  const char *end=ptr+ptr_length;
  char *min_org=min_str;
  char *min_end=min_str+res_length;

  while (ptr < end && min_str < min_end) {
    if (ismbchar_sjis(ptr, end)) {
      *min_str++ = *max_str++ = *ptr++;
      if (min_str < min_end)
	*min_str++ = *max_str++ = *ptr++;
      continue;
    }
    if (*ptr == escape && ptr+1 < end) {
      ptr++;				/* Skip escape */
      if (ismbchar_sjis(ptr, end))
	*min_str++ = *max_str++ = *ptr++;
      if (min_str < min_end)
	*min_str++ = *max_str++ = *ptr++;
      continue;
    }
    if (*ptr == wild_one) {		/* '_' in SQL */
      *min_str++ = '\0';		/* This should be min char */
      *max_str++ = max_sort_char;
      ptr++;
      continue;
    }
    if (*ptr == wild_many) {		/* '%' in SQL */
      *min_length = (uint)(min_str - min_org);
      *max_length = res_length;
      do {
	*min_str++ = ' ';		/* Because if key compression */
	*max_str++ = max_sort_char;
      } while (min_str < min_end);
      return 0;
    }
    *min_str++ = *max_str++ = *ptr++;
  }
  *min_length = *max_length = (uint)(min_str - min_org);
  while (min_str < min_end)
    *min_str++ = *max_str++ = ' ';	/* Because if key compression */
  return 0;
}
