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
 * This file is basicly usa7 character sets with some extra functions
 * for big5 handling
*/

/*
 * This comment is parsed by configure to create ctype.c,
 * so don't change it unless you know what you are doing.
 *
 * .configure. strxfrm_multiply_big5=1
 * .configure. mbmaxlen_big5=2
 */

#include <global.h>
#include "m_string.h"
#include "m_ctype.h"

/* Support for Chinese(BIG5) characters, by jou@nematic.ieo.nctu.edu.tw
   modified by Wei He (hewei@mail.ied.ac.cn) */

#define isbig5head(c) (0xa1<=(uchar)(c) && (uchar)(c)<=0xf9)
#define isbig5tail(c) ((0x40<=(uchar)(c) && (uchar)(c)<=0x7e) || \
                      (0xa1<=(uchar)(c) && (uchar)(c)<=0xfe))

#define isbig5code(c,d) (isbig5head(c) && isbig5tail(d))
#define big5code(c,d)   (((uchar)(c) <<8) | (uchar)(d))
#define big5head(e)	((uchar)(e>>8))
#define big5tail(e)	((uchar)(e&0xff))

uchar NEAR ctype_big5[257] =
{
  0,				/* For standard library */
  32,32,32,32,32,32,32,32,32,40,40,40,40,40,32,32,
  32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
  72,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
  132,132,132,132,132,132,132,132,132,132,16,16,16,16,16,16,
  16,129,129,129,129,129,129,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,16,16,16,16,16,
  16,130,130,130,130,130,130,2,2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,2,2,2,16,16,16,16,32,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

uchar NEAR to_lower_big5[]=
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
  (uchar) '\200',(uchar) '\201',(uchar) '\202',(uchar) '\203',(uchar) '\204',(uchar) '\205',(uchar) '\206',(uchar) '\207',
  (uchar) '\210',(uchar) '\211',(uchar) '\212',(uchar) '\213',(uchar) '\214',(uchar) '\215',(uchar) '\216',(uchar) '\217',
  (uchar) '\220',(uchar) '\221',(uchar) '\222',(uchar) '\223',(uchar) '\224',(uchar) '\225',(uchar) '\226',(uchar) '\227',
  (uchar) '\230',(uchar) '\231',(uchar) '\232',(uchar) '\233',(uchar) '\234',(uchar) '\235',(uchar) '\236',(uchar) '\237',
  (uchar) '\240',(uchar) '\241',(uchar) '\242',(uchar) '\243',(uchar) '\244',(uchar) '\245',(uchar) '\246',(uchar) '\247',
  (uchar) '\250',(uchar) '\251',(uchar) '\252',(uchar) '\253',(uchar) '\254',(uchar) '\255',(uchar) '\256',(uchar) '\257',
  (uchar) '\260',(uchar) '\261',(uchar) '\262',(uchar) '\263',(uchar) '\264',(uchar) '\265',(uchar) '\266',(uchar) '\267',
  (uchar) '\270',(uchar) '\271',(uchar) '\272',(uchar) '\273',(uchar) '\274',(uchar) '\275',(uchar) '\276',(uchar) '\277',
  (uchar) '\300',(uchar) '\301',(uchar) '\302',(uchar) '\303',(uchar) '\304',(uchar) '\305',(uchar) '\306',(uchar) '\307',
  (uchar) '\310',(uchar) '\311',(uchar) '\312',(uchar) '\313',(uchar) '\314',(uchar) '\315',(uchar) '\316',(uchar) '\317',
  (uchar) '\320',(uchar) '\321',(uchar) '\322',(uchar) '\323',(uchar) '\324',(uchar) '\325',(uchar) '\326',(uchar) '\327',
  (uchar) '\330',(uchar) '\331',(uchar) '\332',(uchar) '\333',(uchar) '\334',(uchar) '\335',(uchar) '\336',(uchar) '\337',
  (uchar) '\340',(uchar) '\341',(uchar) '\342',(uchar) '\343',(uchar) '\344',(uchar) '\345',(uchar) '\346',(uchar) '\347',
  (uchar) '\350',(uchar) '\351',(uchar) '\352',(uchar) '\353',(uchar) '\354',(uchar) '\355',(uchar) '\356',(uchar) '\357',
  (uchar) '\360',(uchar) '\361',(uchar) '\362',(uchar) '\363',(uchar) '\364',(uchar) '\365',(uchar) '\366',(uchar) '\367',
  (uchar) '\370',(uchar) '\371',(uchar) '\372',(uchar) '\373',(uchar) '\374',(uchar) '\375',(uchar) '\376',(uchar) '\377',
};

uchar NEAR to_upper_big5[]=
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
  (uchar) '\200',(uchar) '\201',(uchar) '\202',(uchar) '\203',(uchar) '\204',(uchar) '\205',(uchar) '\206',(uchar) '\207',
  (uchar) '\210',(uchar) '\211',(uchar) '\212',(uchar) '\213',(uchar) '\214',(uchar) '\215',(uchar) '\216',(uchar) '\217',
  (uchar) '\220',(uchar) '\221',(uchar) '\222',(uchar) '\223',(uchar) '\224',(uchar) '\225',(uchar) '\226',(uchar) '\227',
  (uchar) '\230',(uchar) '\231',(uchar) '\232',(uchar) '\233',(uchar) '\234',(uchar) '\235',(uchar) '\236',(uchar) '\237',
  (uchar) '\240',(uchar) '\241',(uchar) '\242',(uchar) '\243',(uchar) '\244',(uchar) '\245',(uchar) '\246',(uchar) '\247',
  (uchar) '\250',(uchar) '\251',(uchar) '\252',(uchar) '\253',(uchar) '\254',(uchar) '\255',(uchar) '\256',(uchar) '\257',
  (uchar) '\260',(uchar) '\261',(uchar) '\262',(uchar) '\263',(uchar) '\264',(uchar) '\265',(uchar) '\266',(uchar) '\267',
  (uchar) '\270',(uchar) '\271',(uchar) '\272',(uchar) '\273',(uchar) '\274',(uchar) '\275',(uchar) '\276',(uchar) '\277',
  (uchar) '\300',(uchar) '\301',(uchar) '\302',(uchar) '\303',(uchar) '\304',(uchar) '\305',(uchar) '\306',(uchar) '\307',
  (uchar) '\310',(uchar) '\311',(uchar) '\312',(uchar) '\313',(uchar) '\314',(uchar) '\315',(uchar) '\316',(uchar) '\317',
  (uchar) '\320',(uchar) '\321',(uchar) '\322',(uchar) '\323',(uchar) '\324',(uchar) '\325',(uchar) '\326',(uchar) '\327',
  (uchar) '\330',(uchar) '\331',(uchar) '\332',(uchar) '\333',(uchar) '\334',(uchar) '\335',(uchar) '\336',(uchar) '\337',
  (uchar) '\340',(uchar) '\341',(uchar) '\342',(uchar) '\343',(uchar) '\344',(uchar) '\345',(uchar) '\346',(uchar) '\347',
  (uchar) '\350',(uchar) '\351',(uchar) '\352',(uchar) '\353',(uchar) '\354',(uchar) '\355',(uchar) '\356',(uchar) '\357',
  (uchar) '\360',(uchar) '\361',(uchar) '\362',(uchar) '\363',(uchar) '\364',(uchar) '\365',(uchar) '\366',(uchar) '\367',
  (uchar) '\370',(uchar) '\371',(uchar) '\372',(uchar) '\373',(uchar) '\374',(uchar) '\375',(uchar) '\376',(uchar) '\377',
};

uchar NEAR sort_order_big5[]=
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
  'X',	 'Y',	'Z',   '\\',  ']',   '[',   '^',   '_',
  'E',	 'A',	'B',   'C',   'D',   'E',   'F',   'G',
  'H',	 'I',	'J',   'K',   'L',   'M',   'N',   'O',
  'P',	 'Q',	'R',   'S',   'T',   'U',   'V',   'W',
  'X',	 'Y',	'Z',   '{',   '|',   '}',   'Y',   '\177',
  (uchar) '\200',(uchar) '\201',(uchar) '\202',(uchar) '\203',(uchar) '\204',(uchar) '\205',(uchar) '\206',(uchar) '\207',
  (uchar) '\210',(uchar) '\211',(uchar) '\212',(uchar) '\213',(uchar) '\214',(uchar) '\215',(uchar) '\216',(uchar) '\217',
  (uchar) '\220',(uchar) '\221',(uchar) '\222',(uchar) '\223',(uchar) '\224',(uchar) '\225',(uchar) '\226',(uchar) '\227',
  (uchar) '\230',(uchar) '\231',(uchar) '\232',(uchar) '\233',(uchar) '\234',(uchar) '\235',(uchar) '\236',(uchar) '\237',
  (uchar) '\240',(uchar) '\241',(uchar) '\242',(uchar) '\243',(uchar) '\244',(uchar) '\245',(uchar) '\246',(uchar) '\247',
  (uchar) '\250',(uchar) '\251',(uchar) '\252',(uchar) '\253',(uchar) '\254',(uchar) '\255',(uchar) '\256',(uchar) '\257',
  (uchar) '\260',(uchar) '\261',(uchar) '\262',(uchar) '\263',(uchar) '\264',(uchar) '\265',(uchar) '\266',(uchar) '\267',
  (uchar) '\270',(uchar) '\271',(uchar) '\272',(uchar) '\273',(uchar) '\274',(uchar) '\275',(uchar) '\276',(uchar) '\277',
  (uchar) '\300',(uchar) '\301',(uchar) '\302',(uchar) '\303',(uchar) '\304',(uchar) '\305',(uchar) '\306',(uchar) '\307',
  (uchar) '\310',(uchar) '\311',(uchar) '\312',(uchar) '\313',(uchar) '\314',(uchar) '\315',(uchar) '\316',(uchar) '\317',
  (uchar) '\320',(uchar) '\321',(uchar) '\322',(uchar) '\323',(uchar) '\324',(uchar) '\325',(uchar) '\326',(uchar) '\327',
  (uchar) '\330',(uchar) '\331',(uchar) '\332',(uchar) '\333',(uchar) '\334',(uchar) '\335',(uchar) '\336',(uchar) '\337',
  (uchar) '\340',(uchar) '\341',(uchar) '\342',(uchar) '\343',(uchar) '\344',(uchar) '\345',(uchar) '\346',(uchar) '\347',
  (uchar) '\350',(uchar) '\351',(uchar) '\352',(uchar) '\353',(uchar) '\354',(uchar) '\355',(uchar) '\356',(uchar) '\357',
  (uchar) '\360',(uchar) '\361',(uchar) '\362',(uchar) '\363',(uchar) '\364',(uchar) '\365',(uchar) '\366',(uchar) '\367',
  (uchar) '\370',(uchar) '\371',(uchar) '\372',(uchar) '\373',(uchar) '\374',(uchar) '\375',(uchar) '\376',(uchar) '\377',
};

static uint16 big5strokexfrm(uint16 i)
{
  if ((i == 0xA440) || (i == 0xA441))  return 0xA440;
  else if (((i >= 0xA442) && (i <= 0xA453)) || ((i >= 0xC940) && (i <= 0xC944)))  return 0xA442;
  else if (((i >= 0xA454) && (i <= 0xA47E)) || ((i >= 0xC945) && (i <= 0xC94C)))  return 0xA454;
  else if (((i >= 0xA4A1) && (i <= 0xA4FD)) || ((i >= 0xC94D) && (i <= 0xC962)))  return 0xA4A1;
  else if (((i >= 0xA4FE) && (i <= 0xA5DF)) || ((i >= 0xC963) && (i <= 0xC9AA)))  return 0xA4FE;
  else if (((i >= 0xA5E0) && (i <= 0xA6E9)) || ((i >= 0xC9AB) && (i <= 0xCA59)))  return 0xA5E0;
  else if (((i >= 0xA6EA) && (i <= 0xA8C2)) || ((i >= 0xCA5A) && (i <= 0xCBB0)))  return 0xA6EA;
  else if ((i == 0xA260) || ((i >= 0xA8C3) && (i <= 0xAB44)) || ((i >= 0xCBB1) && (i <= 0xCDDC)))  return 0xA8C3;
  else if ((i == 0xA259) || (i == 0xF9DA) || ((i >= 0xAB45) && (i <= 0xADBB)) || ((i >= 0xCDDD) && (i <= 0xD0C7)))  return 0xAB45;
  else if ((i == 0xA25A) || ((i >= 0xADBC) && (i <= 0xB0AD)) || ((i >= 0xD0C8) && (i <= 0xD44A)))  return 0xADBC;
  else if ((i == 0xA25B) || (i == 0xA25C) || ((i >= 0xB0AE) && (i <= 0xB3C2)) || ((i >= 0xD44B) && (i <= 0xD850)))  return 0xB0AE;
  else if ((i == 0xF9DB) || ((i >= 0xB3C3) && (i <= 0xB6C2)) || ((i >= 0xD851) && (i <= 0xDCB0)))  return 0xB3C3;
  else if ((i == 0xA25D) || (i == 0xA25F) || (i == 0xC6A1) || (i == 0xF9D6) || (i == 0xF9D8) || ((i >= 0xB6C3) && (i <= 0xB9AB)) || ((i >= 0xDCB1) && (i <= 0xE0EF)))  return 0xB6C3;
  else if ((i == 0xF9DC) || ((i >= 0xB9AC) && (i <= 0xBBF4)) || ((i >= 0xE0F0) && (i <= 0xE4E5)))  return 0xB9AC;
  else if ((i == 0xA261) || ((i >= 0xBBF5) && (i <= 0xBEA6)) || ((i >= 0xE4E6) && (i <= 0xE8F3)))  return 0xBBF5;
  else if ((i == 0xA25E) || (i == 0xF9D7) || (i == 0xF9D9) || ((i >= 0xBEA7) && (i <= 0xC074)) || ((i >= 0xE8F4) && (i <= 0xECB8)))  return 0xBEA7;
  else if (((i >= 0xC075) && (i <= 0xC24E)) || ((i >= 0xECB9) && (i <= 0xEFB6)))  return 0xC075;
  else if (((i >= 0xC24F) && (i <= 0xC35E)) || ((i >= 0xEFB7) && (i <= 0xF1EA)))  return 0xC24F;
  else if (((i >= 0xC35F) && (i <= 0xC454)) || ((i >= 0xF1EB) && (i <= 0xF3FC)))  return 0xC35F;
  else if (((i >= 0xC455) && (i <= 0xC4D6)) || ((i >= 0xF3FD) && (i <= 0xF5BF)))  return 0xC455;
  else if (((i >= 0xC4D7) && (i <= 0xC56A)) || ((i >= 0xF5C0) && (i <= 0xF6D5)))  return 0xC4D7;
  else if (((i >= 0xC56B) && (i <= 0xC5C7)) || ((i >= 0xF6D6) && (i <= 0xF7CF)))  return 0xC56B;
  else if (((i >= 0xC5C8) && (i <= 0xC5F0)) || ((i >= 0xF7D0) && (i <= 0xF8A4)))  return 0xC5C8;
  else if (((i >= 0xC5F1) && (i <= 0xC654)) || ((i >= 0xF8A5) && (i <= 0xF8ED)))  return 0xC5F1;
  else if (((i >= 0xC655) && (i <= 0xC664)) || ((i >= 0xF8EE) && (i <= 0xF96A)))  return 0xC655;
  else if (((i >= 0xC665) && (i <= 0xC66B)) || ((i >= 0xF96B) && (i <= 0xF9A1)))  return 0xC665;
  else if (((i >= 0xC66C) && (i <= 0xC675)) || ((i >= 0xF9A2) && (i <= 0xF9B9)))  return 0xC66C;
  else if (((i >= 0xC676) && (i <= 0xC678)) || ((i >= 0xF9BA) && (i <= 0xF9C5)))  return 0xC676;
  else if (((i >= 0xC679) && (i <= 0xC67C)) || ((i >= 0xF9C7) && (i <= 0xF9CB)))  return 0xC679;
  else if ((i == 0xC67D) || ((i >= 0xF9CC) && (i <= 0xF9CF)))  return 0xC67D;
  else if (i == 0xF9D0)  return 0xF9D0;
  else if ((i == 0xC67E) || (i == 0xF9D1))  return 0xC67E;
  else if ((i == 0xF9C6) || (i == 0xF9D2))  return 0xF9C6;
  else if (i == 0xF9D3)  return 0xF9D3;
  else if (i == 0xF9D4)  return 0xF9D4;
  else if (i == 0xF9D5)  return 0xF9D5;
  return 0xA140;
}

int my_strnncoll_big5(const uchar * s1, int len1, const uchar * s2, int len2)
{
  uint len;

  len = min(len1,len2);
  while (len--)
  {
    if ((len > 0) && isbig5code(*s1,*(s1+1)) && isbig5code(*s2, *(s2+1)))
    {
      if (*s1 != *s2 || *(s1+1) != *(s2+1))
	return ((int) big5code(*s1,*(s1+1)) -
		(int) big5code(*s2,*(s2+1)));
      s1 +=2;
      s2 +=2;
      len--;
    } else if (sort_order_big5[(uchar) *s1++] != sort_order_big5[(uchar) *s2++])
      return ((int) sort_order_big5[(uchar) s1[-1]] -
	      (int) sort_order_big5[(uchar) s2[-1]]);
  }
  return (int) (len1-len2);
}

int my_strnxfrm_big5(uchar * dest, uchar * src, int len, int srclen)
{
  uint16 e;
  /*uchar *d = dest; XXX: unused*/

  len = srclen;
  while (len--)
  {
    if ((len > 0) && isbig5code(*src, *(src+1)))
    {
      e = big5strokexfrm((uint16) big5code(*src, *(src+1)));
      *dest++ = big5head(e);
      *dest++ = big5tail(e);
      src +=2;
      len--;
    } else
      *dest++ = sort_order_big5[(uchar) *src++];
  }
  return srclen;
}

int my_strcoll_big5(const uchar * s1, const uchar * s2)
{

  while (*s1 && *s2)
  {
    if (*(s1+1) && *(s2+1) && isbig5code(*s1,*(s1+1)) && isbig5code(*s2, *(s2+1)))
    {
      if (*s1 != *s2 || *(s1+1) != *(s2+1))
	return ((int) big5code(*s1,*(s1+1)) -
		(int) big5code(*s2,*(s2+1)));
      s1 +=2;
      s2 +=2;
    } else if (sort_order_big5[(uchar) *s1++] != sort_order_big5[(uchar) *s2++])
      return ((int) sort_order_big5[(uchar) s1[-1]] -
	      (int) sort_order_big5[(uchar) s2[-1]]);
  }
  return 0;
}

int my_strxfrm_big5(uchar * dest, uchar * src, int len)
{
  uint16 e;
  uchar *d = dest;

  if (len < 1) return 0;
  if (!*src)
  {
    *d = '\0';
    return 0;
  }
  while (*src && (len > 1))
  {
    if (*(src+1) && isbig5code(*src, *(src+1)))
    {
      e = big5strokexfrm((uint16) big5code(*src, *(src+1)));
      *d++ = big5head(e);
      *d++ = big5tail(e);
      src +=2;
      len--;
    } else
      *d++ = sort_order_big5[(uchar) *src++];
  }
  *d = '\0';
  return (int) (d-dest);
}

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

#define max_sort_char ((char) 255)
#define wild_one '_'
#define wild_many '%'

my_bool my_like_range_big5(const char *ptr,uint ptr_length,pchar escape,
		           uint res_length, char *min_str,char *max_str,
		           uint *min_length,uint *max_length)
{
  const char *end=ptr+ptr_length;
  char *min_org=min_str;
  char *min_end=min_str+res_length;

  for (; ptr != end && min_str != min_end ; ptr++)
  {
    if (ptr+1 != end && isbig5code(ptr[0],ptr[1]))
    {
      *min_str++= *max_str++ = *ptr++;
      *min_str++= *max_str++ = *ptr;
      continue;
    }
    if (*ptr == escape && ptr+1 != end)
    {
      ptr++;				/* Skipp escape */
      *min_str++= *max_str++ = *ptr;
      continue;
    }
    if (*ptr == wild_one)		/* '_' in SQL */
    {
      *min_str++='\0';			/* This should be min char */
      *max_str++=max_sort_char;
      continue;
    }
    if (*ptr == wild_many)		/* '%' in SQL */
    {
      *min_length= (uint) (min_str-min_org);
      *max_length= res_length;
      do {
	*min_str++ = '\0';		/* Because if key compression */
	*max_str++ = max_sort_char;
      } while (min_str != min_end);
      return 0;
    }
    *min_str++= *max_str++ = *ptr;
  }
  *min_length= *max_length= (uint) (min_str-min_org);
  while (min_str != min_end)
  {
    *min_str++ = ' ';			/* Because if key compression */
    *max_str++ = ' ';
  }
  return 0;
}

int ismbchar_big5(const char* p, const char *e)
{
  return (isbig5head(*(p)) && (e)-(p)>1 && isbig5tail(*((p)+1))? 2: 0);
}

my_bool ismbhead_big5(uint c)
{
  return isbig5head(c);
}

int mbcharlen_big5(uint c)
{
  return (isbig5head(c)? 2: 0);
}
