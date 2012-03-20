/**********************************************************************
  mktable.c
**********************************************************************/
/*-
 * Copyright (c) 2002-2007  K.Kosako  <sndgk393 AT ybb DOT ne DOT jp>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <locale.h>

#define __USE_ISOC99
#include <ctype.h>

#include "regenc.h"

#define ASCII                0
#define UNICODE_ISO_8859_1   1
#define ISO_8859_1           2
#define ISO_8859_2           3
#define ISO_8859_3           4
#define ISO_8859_4           5
#define ISO_8859_5           6
#define ISO_8859_6           7
#define ISO_8859_7           8
#define ISO_8859_8           9
#define ISO_8859_9          10
#define ISO_8859_10         11
#define ISO_8859_11         12
#define ISO_8859_13         13
#define ISO_8859_14         14
#define ISO_8859_15         15
#define ISO_8859_16         16
#define KOI8                17
#define KOI8_R              18

typedef struct {
  int   num;
  const char* name;
} ENC_INFO;

static ENC_INFO Info[] = {
  { ASCII,               "ASCII" },
  { UNICODE_ISO_8859_1,  "UNICODE_ISO_8859_1"  },
  { ISO_8859_1,  "ISO_8859_1"  },
  { ISO_8859_2,  "ISO_8859_2"  },
  { ISO_8859_3,  "ISO_8859_3"  },
  { ISO_8859_4,  "ISO_8859_4"  },
  { ISO_8859_5,  "ISO_8859_5"  },
  { ISO_8859_6,  "ISO_8859_6"  },
  { ISO_8859_7,  "ISO_8859_7"  },
  { ISO_8859_8,  "ISO_8859_8"  },
  { ISO_8859_9,  "ISO_8859_9"  },
  { ISO_8859_10, "ISO_8859_10" },
  { ISO_8859_11, "ISO_8859_11" },
  { ISO_8859_13, "ISO_8859_13" },
  { ISO_8859_14, "ISO_8859_14" },
  { ISO_8859_15, "ISO_8859_15" },
  { ISO_8859_16, "ISO_8859_16" },
  { KOI8,        "KOI8" },
  { KOI8_R,      "KOI8_R" }
};


static int IsAlpha(int enc, int c)
{
  if (enc == ASCII)
    return isalpha(c);

  if (c >= 0x41 && c <= 0x5a) return 1;
  if (c >= 0x61 && c <= 0x7a) return 1;

  switch (enc) {
  case UNICODE_ISO_8859_1:
  case ISO_8859_1:
  case ISO_8859_9:
    if (c == 0xaa) return 1;
    if (c == 0xb5) return 1;
    if (c == 0xba) return 1;
    if (c >= 0xc0 && c <= 0xd6) return 1;
    if (c >= 0xd8 && c <= 0xf6) return 1;
    if (c >= 0xf8 && c <= 0xff) return 1;
    break;

  case ISO_8859_2:
    if (c == 0xa1 || c == 0xa3) return 1;
    if (c == 0xa5 || c == 0xa6) return 1;
    if (c >= 0xa9 && c <= 0xac) return 1;
    if (c >= 0xae && c <= 0xaf) return 1;
    if (c == 0xb1 || c == 0xb3) return 1;
    if (c == 0xb5 || c == 0xb6) return 1;
    if (c >= 0xb9 && c <= 0xbc) return 1;
    if (c >= 0xbe && c <= 0xbf) return 1;
    if (c >= 0xc0 && c <= 0xd6) return 1;
    if (c >= 0xd8 && c <= 0xf6) return 1;
    if (c >= 0xf8 && c <= 0xfe) return 1;
    break;

  case ISO_8859_3:
    if (c == 0xa1) return 1;
    if (c == 0xa6) return 1;
    if (c >= 0xa9 && c <= 0xac) return 1;
    if (c == 0xaf) return 1;
    if (c == 0xb1) return 1;
    if (c == 0xb5 || c == 0xb6) return 1;
    if (c >= 0xb9 && c <= 0xbc) return 1;
    if (c == 0xbf) return 1;
    if (c >= 0xc0 && c <= 0xc2) return 1;
    if (c >= 0xc4 && c <= 0xcf) return 1;
    if (c >= 0xd1 && c <= 0xd6) return 1;
    if (c >= 0xd8 && c <= 0xe2) return 1;
    if (c >= 0xe4 && c <= 0xef) return 1;
    if (c >= 0xf1 && c <= 0xf6) return 1;
    if (c >= 0xf8 && c <= 0xfe) return 1;
    break;

  case ISO_8859_4:
    if (c >= 0xa1 && c <= 0xa3) return 1;
    if (c == 0xa5 || c == 0xa6) return 1;
    if (c >= 0xa9 && c <= 0xac) return 1;
    if (c == 0xae) return 1;
    if (c == 0xb1 || c == 0xb3) return 1;
    if (c == 0xb5 || c == 0xb6) return 1;
    if (c >= 0xb9 && c <= 0xbf) return 1;
    if (c >= 0xc0 && c <= 0xd6) return 1;
    if (c >= 0xd8 && c <= 0xf6) return 1;
    if (c >= 0xf8 && c <= 0xfe) return 1;
    break;

  case ISO_8859_5:
    if (c >= 0xa1 && c <= 0xcf && c != 0xad) return 1;
    if (c >= 0xd0 && c <= 0xff && c != 0xf0 && c != 0xfd) return 1;
    break;

  case ISO_8859_6:
    if (c >= 0xc1 && c <= 0xda) return 1;
    if (c >= 0xe0 && c <= 0xf2) return 1;
    break;

  case ISO_8859_7:
    if (c == 0xb6) return 1;
    if (c >= 0xb8 && c <= 0xba) return 1;
    if (c == 0xbc) return 1;
    if (c >= 0xbe && c <= 0xbf) return 1;
    if (c == 0xc0) return 1;
    if (c >= 0xc1 && c <= 0xdb && c != 0xd2) return 1;
    if (c >= 0xdc && c <= 0xfe) return 1;
    break;

  case ISO_8859_8:
    if (c == 0xb5) return 1;
    if (c >= 0xe0 && c <= 0xfa) return 1;
    break;

  case ISO_8859_10:
    if (c >= 0xa1 && c <= 0xa6) return 1;
    if (c >= 0xa8 && c <= 0xac) return 1;
    if (c == 0xae || c == 0xaf) return 1;
    if (c >= 0xb1 && c <= 0xb6) return 1;
    if (c >= 0xb8 && c <= 0xbc) return 1;
    if (c >= 0xbe && c <= 0xff) return 1;
    break;

  case ISO_8859_11:
    if (c >= 0xa1 && c <= 0xda) return 1;
    if (c >= 0xdf && c <= 0xfb) return 1;
    break;

  case ISO_8859_13:
    if (c == 0xa8) return 1;
    if (c == 0xaa) return 1;
    if (c == 0xaf) return 1;
    if (c == 0xb5) return 1;
    if (c == 0xb8) return 1;
    if (c == 0xba) return 1;
    if (c >= 0xbf && c <= 0xd6) return 1;
    if (c >= 0xd8 && c <= 0xf6) return 1;
    if (c >= 0xf8 && c <= 0xfe) return 1;
    break;

  case ISO_8859_14:
    if (c == 0xa1 || c == 0xa2) return 1;
    if (c == 0xa4 || c == 0xa5) return 1;
    if (c == 0xa6 || c == 0xa8) return 1;
    if (c >= 0xaa && c <= 0xac) return 1;
    if (c >= 0xaf && c <= 0xb5) return 1;
    if (c >= 0xb7 && c <= 0xff) return 1;
    break;

  case ISO_8859_15:
    if (c == 0xaa) return 1;
    if (c == 0xb5) return 1;
    if (c == 0xba) return 1;
    if (c >= 0xc0 && c <= 0xd6) return 1;
    if (c >= 0xd8 && c <= 0xf6) return 1;
    if (c >= 0xf8 && c <= 0xff) return 1;
    if (c == 0xa6) return 1;
    if (c == 0xa8) return 1;
    if (c == 0xb4) return 1;
    if (c == 0xb8) return 1;
    if (c == 0xbc) return 1;
    if (c == 0xbd) return 1;
    if (c == 0xbe) return 1;
    break;

  case ISO_8859_16:
    if (c == 0xa1) return 1;
    if (c == 0xa2) return 1;
    if (c == 0xa3) return 1;
    if (c == 0xa6) return 1;
    if (c == 0xa8) return 1;
    if (c == 0xaa) return 1;
    if (c == 0xac) return 1;
    if (c == 0xae) return 1;
    if (c == 0xaf) return 1;
    if (c == 0xb2) return 1;
    if (c == 0xb3) return 1;
    if (c == 0xb4) return 1;
    if (c >= 0xb8 && c <= 0xba) return 1;
    if (c == 0xbc) return 1;
    if (c == 0xbd) return 1;
    if (c == 0xbe) return 1;
    if (c == 0xbf) return 1;
    if (c >= 0xc0 && c <= 0xde) return 1;
    if (c >= 0xdf && c <= 0xff) return 1;
    break;

  case KOI8_R:
    if (c == 0xa3 || c == 0xb3) return 1;
    /* fall */
  case KOI8:
    if (c >= 0xc0 && c <= 0xff) return 1;
    break;

  default:
    exit(-1);
  }

  return 0;
}

static int IsBlank(int enc, int c)
{
  if (enc == ASCII)
    return isblank(c);

  if (c == 0x09	|| c == 0x20) return 1;

  switch (enc) {
  case UNICODE_ISO_8859_1:
  case ISO_8859_1:
  case ISO_8859_2:
  case ISO_8859_3:
  case ISO_8859_4:
  case ISO_8859_5:
  case ISO_8859_6:
  case ISO_8859_7:
  case ISO_8859_8:
  case ISO_8859_9:
  case ISO_8859_10:
  case ISO_8859_11:
  case ISO_8859_13:
  case ISO_8859_14:
  case ISO_8859_15:
  case ISO_8859_16:
  case KOI8:
    if (c == 0xa0) return 1;
    break;

  case KOI8_R:
    if (c == 0x9a) return 1;
    break;

  default:
    exit(-1);
  }

  return 0;
}

static int IsCntrl(int enc, int c)
{
  if (enc == ASCII)
    return iscntrl(c);

  if (c >= 0x00	&& c <= 0x1F) return 1;

  switch (enc) {
  case UNICODE_ISO_8859_1:
    if (c == 0xad) return 1;
    /* fall */
  case ISO_8859_1:
  case ISO_8859_2:
  case ISO_8859_3:
  case ISO_8859_4:
  case ISO_8859_5:
  case ISO_8859_6:
  case ISO_8859_7:
  case ISO_8859_8:
  case ISO_8859_9:
  case ISO_8859_10:
  case ISO_8859_11:
  case ISO_8859_13:
  case ISO_8859_14:
  case ISO_8859_15:
  case ISO_8859_16:
  case KOI8:
    if (c >= 0x7f && c <= 0x9F) return 1;
    break;


  case KOI8_R:
    if (c == 0x7f) return 1;
    break;

  default:
    exit(-1);
  }

  return 0;
}

static int IsDigit(int enc ARG_UNUSED, int c)
{
  if (c >= 0x30 && c <= 0x39) return 1;
  return 0;
}

static int IsGraph(int enc, int c)
{
  if (enc == ASCII)
    return isgraph(c);

  if (c >= 0x21 && c <= 0x7e) return 1;

  switch (enc) {
  case UNICODE_ISO_8859_1:
  case ISO_8859_1:
  case ISO_8859_2:
  case ISO_8859_4:
  case ISO_8859_5:
  case ISO_8859_9:
  case ISO_8859_10:
  case ISO_8859_13:
  case ISO_8859_14:
  case ISO_8859_15:
  case ISO_8859_16:
    if (c >= 0xa1 && c <= 0xff) return 1;
    break;

  case ISO_8859_3:
    if (c >= 0xa1) {
      if (c == 0xa5 || c == 0xae || c == 0xbe || c == 0xc3 || c == 0xd0 ||
	  c == 0xe3 || c == 0xf0)
	return 0;
      else
	return 1;
    }
    break;

  case ISO_8859_6:
    if (c == 0xa4 || c == 0xac || c == 0xad || c == 0xbb || c == 0xbf)
      return 1;
    if (c >= 0xc1 && c <= 0xda) return 1;
    if (c >= 0xe0 && c <= 0xf2) return 1;
    break;

  case ISO_8859_7:
    if (c >= 0xa1 && c <= 0xfe &&
	c != 0xa4 && c != 0xa5 && c != 0xaa &&
	c != 0xae && c != 0xd2) return 1;
    break;

  case ISO_8859_8:
    if (c >= 0xa2 && c <= 0xfa) {
      if (c >= 0xbf && c <= 0xde) return 0;
      return 1;
    }
    break;

  case ISO_8859_11:
    if (c >= 0xa1 && c <= 0xda) return 1;
    if (c >= 0xdf && c <= 0xfb) return 1;
    break;

  case KOI8:
    if (c >= 0xc0 && c <= 0xff) return 1;
    break;

  case KOI8_R:
    if (c >= 0x80 && c <= 0xff && c != 0x9a) return 1;
    break;

  default:
    exit(-1);
  }

  return 0;
}

static int IsLower(int enc, int c)
{
  if (enc == ASCII)
    return islower(c);

  if (c >= 0x61 && c <= 0x7a) return 1;

  switch (enc) {
  case UNICODE_ISO_8859_1:
  case ISO_8859_1:
  case ISO_8859_9:
    if (c == 0xaa) return 1;
    if (c == 0xb5) return 1;
    if (c == 0xba) return 1;
    if (c >= 0xdf && c <= 0xf6) return 1;
    if (c >= 0xf8 && c <= 0xff) return 1;
    break;

  case ISO_8859_2:
    if (c == 0xb1 || c == 0xb3) return 1;
    if (c == 0xb5 || c == 0xb6) return 1;
    if (c >= 0xb9 && c <= 0xbc) return 1;
    if (c >= 0xbe && c <= 0xbf) return 1;
    if (c >= 0xdf && c <= 0xf6) return 1;
    if (c >= 0xf8 && c <= 0xfe) return 1;
    break;

  case ISO_8859_3:
    if (c == 0xb1) return 1;
    if (c == 0xb5 || c == 0xb6) return 1;
    if (c >= 0xb9 && c <= 0xbc) return 1;
    if (c == 0xbf) return 1;
    if (c == 0xdf) return 1;
    if (c >= 0xe0 && c <= 0xe2) return 1;
    if (c >= 0xe4 && c <= 0xef) return 1;
    if (c >= 0xf1 && c <= 0xf6) return 1;
    if (c >= 0xf8 && c <= 0xfe) return 1;
    break;

  case ISO_8859_4:
    if (c == 0xa2) return 1;
    if (c == 0xb1 || c == 0xb3) return 1;
    if (c == 0xb5 || c == 0xb6) return 1;
    if (c >= 0xb9 && c <= 0xbc) return 1;
    if (c >= 0xbe && c <= 0xbf) return 1;
    if (c == 0xdf) return 1;
    if (c >= 0xe0 && c <= 0xf6) return 1;
    if (c >= 0xf8 && c <= 0xfe) return 1;
    break;

  case ISO_8859_5:
    if (c >= 0xd0 && c <= 0xff && c != 0xf0 && c != 0xfd) return 1;
    break;

  case ISO_8859_6:
    break;

  case ISO_8859_7:
    if (c == 0xc0) return 1;
    if (c >= 0xdc && c <= 0xfe) return 1;
    break;

  case ISO_8859_8:
    if (c == 0xb5) return 1;
    break;

  case ISO_8859_10:
    if (c >= 0xb1 && c <= 0xb6) return 1;
    if (c >= 0xb8 && c <= 0xbc) return 1;
    if (c == 0xbe || c == 0xbf) return 1;
    if (c >= 0xdf && c <= 0xff) return 1;
    break;

  case ISO_8859_11:
    break;

  case ISO_8859_13:
    if (c == 0xb5) return 1;
    if (c == 0xb8) return 1;
    if (c == 0xba) return 1;
    if (c == 0xbf) return 1;
    if (c >= 0xdf && c <= 0xf6) return 1;
    if (c >= 0xf8 && c <= 0xfe) return 1;
    break;

  case ISO_8859_14:
    if (c == 0xa2) return 1;
    if (c == 0xa5) return 1;
    if (c == 0xab) return 1;
    if (c == 0xb1 || c == 0xb3 || c == 0xb5) return 1;
    if (c >= 0xb8 && c <= 0xba) return 1;
    if (c == 0xbc) return 1;
    if (c == 0xbe || c == 0xbf) return 1;
    if (c >= 0xdf && c <= 0xff) return 1;
    break;

  case ISO_8859_15:
    if (c == 0xaa) return 1;
    if (c == 0xb5) return 1;
    if (c == 0xba) return 1;
    if (c >= 0xdf && c <= 0xf6) return 1;
    if (c >= 0xf8 && c <= 0xff) return 1;
    if (c == 0xa8) return 1;
    if (c == 0xb8) return 1;
    if (c == 0xbd) return 1;
    break;

  case ISO_8859_16:
    if (c == 0xa2) return 1;
    if (c == 0xa8) return 1;
    if (c == 0xae) return 1;
    if (c == 0xb3) return 1;
    if (c >= 0xb8 && c <= 0xba) return 1;
    if (c == 0xbd) return 1;
    if (c == 0xbf) return 1;
    if (c >= 0xdf && c <= 0xff) return 1;
    break;

  case KOI8_R:
    if (c == 0xa3) return 1;
    /* fall */
  case KOI8:
    if (c >= 0xc0 && c <= 0xdf) return 1;
    break;

  default:
    exit(-1);
  }

  return 0;
}

static int IsPrint(int enc, int c)
{
  if (enc == ASCII)
    return isprint(c);

  if (c >= 0x20 && c <= 0x7e) return 1;

  switch (enc) {
  case UNICODE_ISO_8859_1:
    /* if (c >= 0x09 && c <= 0x0d) return 1; */
    if (c == 0x85) return 1;
    /* fall */
  case ISO_8859_1:
  case ISO_8859_2:
  case ISO_8859_4:
  case ISO_8859_5:
  case ISO_8859_9:
  case ISO_8859_10:
  case ISO_8859_13:
  case ISO_8859_14:
  case ISO_8859_15:
  case ISO_8859_16:
    if (c >= 0xa0 && c <= 0xff) return 1;
    break;

  case ISO_8859_3:
    if (c >= 0xa0) {
      if (c == 0xa5 || c == 0xae || c == 0xbe || c == 0xc3 || c == 0xd0 ||
	  c == 0xe3 || c == 0xf0)
	return 0;
      else
	return 1;
    }
    break;

  case ISO_8859_6:
    if (c == 0xa0) return 1;
    if (c == 0xa4 || c == 0xac || c == 0xad || c == 0xbb || c == 0xbf)
      return 1;
    if (c >= 0xc1 && c <= 0xda) return 1;
    if (c >= 0xe0 && c <= 0xf2) return 1;
    break;

  case ISO_8859_7:
    if (c >= 0xa0 && c <= 0xfe &&
	c != 0xa4 && c != 0xa5 && c != 0xaa &&
	c != 0xae && c != 0xd2) return 1;
    break;

  case ISO_8859_8:
    if (c >= 0xa0 && c <= 0xfa) {
      if (c >= 0xbf && c <= 0xde) return 0;
      if (c == 0xa1) return 0;
      return 1;
    }
    break;

  case ISO_8859_11:
    if (c >= 0xa0 && c <= 0xda) return 1;
    if (c >= 0xdf && c <= 0xfb) return 1;
    break;

  case KOI8:
    if (c == 0xa0) return 1;
    if (c >= 0xc0 && c <= 0xff) return 1;
    break;

  case KOI8_R:
    if (c >= 0x80 && c <= 0xff) return 1;
    break;

  default:
    exit(-1);
  }

  return 0;
}

static int IsPunct(int enc, int c)
{
  if (enc == ASCII)
    return ispunct(c);

  if (enc == UNICODE_ISO_8859_1) {
    if (c == 0x24 || c == 0x2b || c == 0x5e || c == 0x60 ||
        c == 0x7c || c == 0x7e) return 1;
    if (c >= 0x3c && c <= 0x3e) return 1;
  }

  if (c >= 0x21 && c <= 0x2f) return 1;
  if (c >= 0x3a && c <= 0x40) return 1;
  if (c >= 0x5b && c <= 0x60) return 1;
  if (c >= 0x7b && c <= 0x7e) return 1;

  switch (enc) {
  case ISO_8859_1:
  case ISO_8859_9:
  case ISO_8859_15:
    if (c == 0xad) return 1;
    /* fall */
  case UNICODE_ISO_8859_1:
    if (c == 0xa1) return 1;
    if (c == 0xab) return 1;
    if (c == 0xb7) return 1;
    if (c == 0xbb) return 1;
    if (c == 0xbf) return 1;
    break;

  case ISO_8859_2:
  case ISO_8859_4:
  case ISO_8859_5:
  case ISO_8859_14:
    if (c == 0xad) return 1;
    break;

  case ISO_8859_3:
  case ISO_8859_10:
    if (c == 0xad) return 1;
    if (c == 0xb7) return 1;
    if (c == 0xbd) return 1;
    break;

  case ISO_8859_6:
    if (c == 0xac) return 1;
    if (c == 0xad) return 1;
    if (c == 0xbb) return 1;
    if (c == 0xbf) return 1;
    break;

  case ISO_8859_7:
    if (c == 0xa1 || c == 0xa2) return 1;
    if (c == 0xab) return 1;
    if (c == 0xaf) return 1;
    if (c == 0xad) return 1;
    if (c == 0xb7 || c == 0xbb) return 1;
    break;

  case ISO_8859_8:
    if (c == 0xab) return 1;
    if (c == 0xad) return 1;
    if (c == 0xb7) return 1;
    if (c == 0xbb) return 1;
    if (c == 0xdf) return 1;
    break;

  case ISO_8859_13:
    if (c == 0xa1 || c == 0xa5) return 1;
    if (c == 0xab || c == 0xad) return 1;
    if (c == 0xb4 || c == 0xb7) return 1;
    if (c == 0xbb) return 1;
    if (c == 0xff) return 1;
    break;

  case ISO_8859_16:
    if (c == 0xa5) return 1;
    if (c == 0xab) return 1;
    if (c == 0xad) return 1;
    if (c == 0xb5) return 1;
    if (c == 0xb7) return 1;
    if (c == 0xbb) return 1;
    break;

  case KOI8_R:
    if (c == 0x9e) return 1;
    break;

  case ISO_8859_11:
  case KOI8:
    break;

  default:
    exit(-1);
  }

  return 0;
}

static int IsSpace(int enc, int c)
{
  if (enc == ASCII)
    return isspace(c);

  if (c >= 0x09 && c <= 0x0d) return 1;
  if (c == 0x20) return 1;

  switch (enc) {
  case UNICODE_ISO_8859_1:
    if (c == 0x85) return 1;
    /* fall */
  case ISO_8859_1:
  case ISO_8859_2:
  case ISO_8859_3:
  case ISO_8859_4:
  case ISO_8859_5:
  case ISO_8859_6:
  case ISO_8859_7:
  case ISO_8859_8:
  case ISO_8859_9:
  case ISO_8859_10:
  case ISO_8859_11:
  case ISO_8859_13:
  case ISO_8859_14:
  case ISO_8859_15:
  case ISO_8859_16:
  case KOI8:
    if (c == 0xa0) return 1;
    break;

  case KOI8_R:
    if (c == 0x9a) return 1;
    break;

  default:
    exit(-1);
  }

  return 0;
}

static int IsUpper(int enc, int c)
{
  if (enc == ASCII)
    return isupper(c);

  if (c >= 0x41 && c <= 0x5a) return 1;

  switch (enc) {
  case UNICODE_ISO_8859_1:
  case ISO_8859_1:
  case ISO_8859_9:
    if (c >= 0xc0 && c <= 0xd6) return 1;
    if (c >= 0xd8 && c <= 0xde) return 1;
    break;

  case ISO_8859_2:
    if (c == 0xa1 || c == 0xa3) return 1;
    if (c == 0xa5 || c == 0xa6) return 1;
    if (c >= 0xa9 && c <= 0xac) return 1;
    if (c >= 0xae && c <= 0xaf) return 1;
    if (c >= 0xc0 && c <= 0xd6) return 1;
    if (c >= 0xd8 && c <= 0xde) return 1;
    break;

  case ISO_8859_3:
    if (c == 0xa1) return 1;
    if (c == 0xa6) return 1;
    if (c >= 0xa9 && c <= 0xac) return 1;
    if (c == 0xaf) return 1;
    if (c >= 0xc0 && c <= 0xc2) return 1;
    if (c >= 0xc4 && c <= 0xcf) return 1;
    if (c >= 0xd1 && c <= 0xd6) return 1;
    if (c >= 0xd8 && c <= 0xde) return 1;
    break;

  case ISO_8859_4:
    if (c == 0xa1 || c == 0xa3) return 1;
    if (c == 0xa5 || c == 0xa6) return 1;
    if (c >= 0xa9 && c <= 0xac) return 1;
    if (c == 0xae) return 1;
    if (c == 0xbd) return 1;
    if (c >= 0xc0 && c <= 0xd6) return 1;
    if (c >= 0xd8 && c <= 0xde) return 1;
    break;

  case ISO_8859_5:
    if (c >= 0xa1 && c <= 0xcf && c != 0xad) return 1;
    break;

  case ISO_8859_6:
    break;

  case ISO_8859_7:
    if (c == 0xb6) return 1;
    if (c >= 0xb8 && c <= 0xba) return 1;
    if (c == 0xbc) return 1;
    if (c >= 0xbe && c <= 0xbf) return 1;
    if (c >= 0xc1 && c <= 0xdb && c != 0xd2) return 1;
    break;

  case ISO_8859_8:
  case ISO_8859_11:
    break;

  case ISO_8859_10:
    if (c >= 0xa1 && c <= 0xa6) return 1;
    if (c >= 0xa8 && c <= 0xac) return 1;
    if (c == 0xae || c == 0xaf) return 1;
    if (c >= 0xc0 && c <= 0xde) return 1;
    break;

  case ISO_8859_13:
    if (c == 0xa8) return 1;
    if (c == 0xaa) return 1;
    if (c == 0xaf) return 1;
    if (c >= 0xc0 && c <= 0xd6) return 1;
    if (c >= 0xd8 && c <= 0xde) return 1;
    break;

  case ISO_8859_14:
    if (c == 0xa1) return 1;
    if (c == 0xa4 || c == 0xa6) return 1;
    if (c == 0xa8) return 1;
    if (c == 0xaa || c == 0xac) return 1;
    if (c == 0xaf || c == 0xb0) return 1;
    if (c == 0xb2 || c == 0xb4 || c == 0xb7) return 1;
    if (c == 0xbb || c == 0xbd) return 1;
    if (c >= 0xc0 && c <= 0xde) return 1;
    break;

  case ISO_8859_15:
    if (c >= 0xc0 && c <= 0xd6) return 1;
    if (c >= 0xd8 && c <= 0xde) return 1;
    if (c == 0xa6) return 1;
    if (c == 0xb4) return 1;
    if (c == 0xbc) return 1;
    if (c == 0xbe) return 1;
    break;

  case ISO_8859_16:
    if (c == 0xa1) return 1;
    if (c == 0xa3) return 1;
    if (c == 0xa6) return 1;
    if (c == 0xaa) return 1;
    if (c == 0xac) return 1;
    if (c == 0xaf) return 1;
    if (c == 0xb2) return 1;
    if (c == 0xb4) return 1;
    if (c == 0xbc) return 1;
    if (c == 0xbe) return 1;
    if (c >= 0xc0 && c <= 0xde) return 1;
    break;

  case KOI8_R:
    if (c == 0xb3) return 1;
    /* fall */
  case KOI8:
    if (c >= 0xe0 && c <= 0xff) return 1;
    break;

  default:
    exit(-1);
  }

  return 0;
}

static int IsXDigit(int enc, int c)
{
  if (enc == ASCII)
    return isxdigit(c);

  if (c >= 0x30 && c <= 0x39) return 1;
  if (c >= 0x41 && c <= 0x46) return 1;
  if (c >= 0x61 && c <= 0x66) return 1;
  return 0;
}

static int IsWord(int enc, int c)
{
  if (enc == ASCII) {
    return (isalpha(c) || isdigit(c) || c == 0x5f);
  }

  if (c >= 0x30 && c <= 0x39) return 1;
  if (c >= 0x41 && c <= 0x5a) return 1;
  if (c == 0x5f) return 1;
  if (c >= 0x61 && c <= 0x7a) return 1;

  switch (enc) {
  case UNICODE_ISO_8859_1:
  case ISO_8859_1:
  case ISO_8859_9:
    if (c == 0xaa) return 1;
    if (c >= 0xb2 && c <= 0xb3) return 1;
    if (c == 0xb5) return 1;
    if (c >= 0xb9 && c <= 0xba) return 1;
    if (c >= 0xbc && c <= 0xbe) return 1;
    if (c >= 0xc0 && c <= 0xd6) return 1;
    if (c >= 0xd8 && c <= 0xf6) return 1;
    if (c >= 0xf8 && c <= 0xff) return 1;
    break;

  case ISO_8859_2:
    if (c == 0xa1 || c == 0xa3) return 1;
    if (c == 0xa5 || c == 0xa6) return 1;
    if (c >= 0xa9 && c <= 0xac) return 1;
    if (c >= 0xae && c <= 0xaf) return 1;
    if (c == 0xb1 || c == 0xb3) return 1;
    if (c == 0xb5 || c == 0xb6) return 1;
    if (c >= 0xb9 && c <= 0xbc) return 1;
    if (c >= 0xbe && c <= 0xbf) return 1;
    if (c >= 0xc0 && c <= 0xd6) return 1;
    if (c >= 0xd8 && c <= 0xf6) return 1;
    if (c >= 0xf8 && c <= 0xfe) return 1;
    break;

  case ISO_8859_3:
    if (c == 0xa1) return 1;
    if (c == 0xa6) return 1;
    if (c >= 0xa9 && c <= 0xac) return 1;
    if (c == 0xaf) return 1;
    if (c >= 0xb1 && c <= 0xb3) return 1;
    if (c == 0xb5 || c == 0xb6) return 1;
    if (c >= 0xb9 && c <= 0xbd) return 1;
    if (c == 0xbf) return 1;
    if (c >= 0xc0 && c <= 0xc2) return 1;
    if (c >= 0xc4 && c <= 0xcf) return 1;
    if (c >= 0xd1 && c <= 0xd6) return 1;
    if (c >= 0xd8 && c <= 0xe2) return 1;
    if (c >= 0xe4 && c <= 0xef) return 1;
    if (c >= 0xf1 && c <= 0xf6) return 1;
    if (c >= 0xf8 && c <= 0xfe) return 1;
    break;

  case ISO_8859_4:
    if (c >= 0xa1 && c <= 0xa3) return 1;
    if (c == 0xa5 || c == 0xa6) return 1;
    if (c >= 0xa9 && c <= 0xac) return 1;
    if (c == 0xae) return 1;
    if (c == 0xb1 || c == 0xb3) return 1;
    if (c == 0xb5 || c == 0xb6) return 1;
    if (c >= 0xb9 && c <= 0xbf) return 1;
    if (c >= 0xc0 && c <= 0xd6) return 1;
    if (c >= 0xd8 && c <= 0xf6) return 1;
    if (c >= 0xf8 && c <= 0xfe) return 1;
    break;

  case ISO_8859_5:
    if (c >= 0xa1 && c <= 0xcf && c != 0xad) return 1;
    if (c >= 0xd0 && c <= 0xff && c != 0xf0 && c != 0xfd) return 1;
    break;

  case ISO_8859_6:
    if (c >= 0xc1 && c <= 0xda) return 1;
    if (c >= 0xe0 && c <= 0xea) return 1;
    if (c >= 0xeb && c <= 0xf2) return 1;
    break;

  case ISO_8859_7:
    if (c == 0xb2 || c == 0xb3) return 1;
    if (c == 0xb6) return 1;
    if (c >= 0xb8 && c <= 0xba) return 1;
    if (c >= 0xbc && c <= 0xbf) return 1;
    if (c == 0xc0) return 1;
    if (c >= 0xc1 && c <= 0xdb && c != 0xd2) return 1;
    if (c >= 0xdc && c <= 0xfe) return 1;
    break;

  case ISO_8859_8:
    if (c == 0xb2 || c == 0xb3 || c == 0xb5 || c == 0xb9) return 1;
    if (c >= 0xbc && c <= 0xbe) return 1;
    if (c >= 0xe0 && c <= 0xfa) return 1;
    break;

  case ISO_8859_10:
    if (c >= 0xa1 && c <= 0xff) {
      if (c != 0xa7 && c != 0xad && c != 0xb0 && c != 0xb7 && c != 0xbd)
	return 1;
    }
    break;

  case ISO_8859_11:
    if (c >= 0xa1 && c <= 0xda) return 1;
    if (c >= 0xdf && c <= 0xfb) return 1;
    break;

  case ISO_8859_13:
    if (c == 0xa8) return 1;
    if (c == 0xaa) return 1;
    if (c == 0xaf) return 1;
    if (c == 0xb2 || c == 0xb3 || c == 0xb5 || c == 0xb9) return 1;
    if (c >= 0xbc && c <= 0xbe) return 1;
    if (c == 0xb8) return 1;
    if (c == 0xba) return 1;
    if (c >= 0xbf && c <= 0xd6) return 1;
    if (c >= 0xd8 && c <= 0xf6) return 1;
    if (c >= 0xf8 && c <= 0xfe) return 1;
    break;

  case ISO_8859_14:
    if (c >= 0xa1 && c <= 0xff) {
      if (c == 0xa3 || c == 0xa7 || c == 0xa9 || c == 0xad || c == 0xae ||
	  c == 0xb6) return 0;
      return 1;
    }
    break;

  case ISO_8859_15:
    if (c == 0xaa) return 1;
    if (c >= 0xb2 && c <= 0xb3) return 1;
    if (c == 0xb5) return 1;
    if (c >= 0xb9 && c <= 0xba) return 1;
    if (c >= 0xbc && c <= 0xbe) return 1;
    if (c >= 0xc0 && c <= 0xd6) return 1;
    if (c >= 0xd8 && c <= 0xf6) return 1;
    if (c >= 0xf8 && c <= 0xff) return 1;
    if (c == 0xa6) return 1;
    if (c == 0xa8) return 1;
    if (c == 0xb4) return 1;
    if (c == 0xb8) return 1;
    break;

  case ISO_8859_16:
    if (c == 0xa1) return 1;
    if (c == 0xa2) return 1;
    if (c == 0xa3) return 1;
    if (c == 0xa6) return 1;
    if (c == 0xa8) return 1;
    if (c == 0xaa) return 1;
    if (c == 0xac) return 1;
    if (c == 0xae) return 1;
    if (c == 0xaf) return 1;
    if (c == 0xb2) return 1;
    if (c == 0xb3) return 1;
    if (c == 0xb4) return 1;
    if (c >= 0xb8 && c <= 0xba) return 1;
    if (c == 0xbc) return 1;
    if (c == 0xbd) return 1;
    if (c == 0xbe) return 1;
    if (c == 0xbf) return 1;
    if (c >= 0xc0 && c <= 0xde) return 1;
    if (c >= 0xdf && c <= 0xff) return 1;
    break;

  case KOI8_R:
    if (c == 0x9d) return 1;
    if (c == 0xa3 || c == 0xb3) return 1;
    /* fall */
  case KOI8:
    if (c >= 0xc0 && c <= 0xff) return 1;
    break;

  default:
    exit(-1);
  }

  return 0;
}

static int IsAscii(int enc ARG_UNUSED, int c)
{
  if (c >= 0x00 && c <= 0x7f) return 1;
  return 0;
}

static int IsNewline(int enc ARG_UNUSED, int c)
{
  if (c == 0x0a) return 1;
  return 0;
}

static int exec(FILE* fp, ENC_INFO* einfo)
{
#define NCOL  8

  int c, val, enc;

  enc = einfo->num;

  fprintf(fp, "static const unsigned short Enc%s_CtypeTable[256] = {\n",
	  einfo->name);

  for (c = 0; c < 256; c++) {
    val = 0;
    if (IsNewline(enc, c))  val |= BIT_CTYPE_NEWLINE;
    if (IsAlpha (enc, c))   val |= (BIT_CTYPE_ALPHA | BIT_CTYPE_ALNUM);
    if (IsBlank (enc, c))   val |= BIT_CTYPE_BLANK;
    if (IsCntrl (enc, c))   val |= BIT_CTYPE_CNTRL;
    if (IsDigit (enc, c))   val |= (BIT_CTYPE_DIGIT | BIT_CTYPE_ALNUM);
    if (IsGraph (enc, c))   val |= BIT_CTYPE_GRAPH;
    if (IsLower (enc, c))   val |= BIT_CTYPE_LOWER;
    if (IsPrint (enc, c))   val |= BIT_CTYPE_PRINT;
    if (IsPunct (enc, c))   val |= BIT_CTYPE_PUNCT;
    if (IsSpace (enc, c))   val |= BIT_CTYPE_SPACE;
    if (IsUpper (enc, c))   val |= BIT_CTYPE_UPPER;
    if (IsXDigit(enc, c))   val |= BIT_CTYPE_XDIGIT;
    if (IsWord  (enc, c))   val |= BIT_CTYPE_WORD;
    if (IsAscii (enc, c))   val |= BIT_CTYPE_ASCII;

    if (c % NCOL == 0) fputs("  ", fp);
    fprintf(fp, "0x%04x", val);
    if (c != 255) fputs(",", fp);
    if (c != 0 && c % NCOL == (NCOL-1))
      fputs("\n", fp);
    else
      fputs(" ", fp);
  }
  fprintf(fp, "};\n");
  return 0;
}

extern int main(int argc ARG_UNUSED, char* argv[] ARG_UNUSED)
{
  int i;
  FILE* fp = stdout;

  setlocale(LC_ALL, "C");
  /* setlocale(LC_ALL, "POSIX"); */
  /* setlocale(LC_ALL, "en_GB.iso88591"); */
  /* setlocale(LC_ALL, "de_BE.iso88591"); */
  /* setlocale(LC_ALL, "fr_FR.iso88591"); */

  for (i = 0; i < (int )(sizeof(Info)/sizeof(ENC_INFO)); i++) {
    exec(fp, &Info[i]);
  }

  return 0;
}
