/**********************************************************************
  gb18030.c -  Oniguruma (regular expression library)
**********************************************************************/
/*-
 * Copyright (c) 2005-2007  KUBO Takehiro <kubo AT jiubao DOT org>
 *                          K.Kosako <sndgk393 AT ybb DOT ne DOT jp>
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

#include "regenc.h"

#if 1
#define DEBUG_GB18030(arg)
#else
#define DEBUG_GB18030(arg) printf arg
#endif

enum {
  C1, /* one-byte char */
  C2, /* one-byte or second of two-byte char */
  C4, /* one-byte or second or fourth of four-byte char */
  CM  /* first of two- or four-byte char or second of two-byte char */
};

static const char GB18030_MAP[] = {
  C1, C1, C1, C1, C1, C1, C1, C1, C1, C1, C1, C1, C1, C1, C1, C1,
  C1, C1, C1, C1, C1, C1, C1, C1, C1, C1, C1, C1, C1, C1, C1, C1,
  C1, C1, C1, C1, C1, C1, C1, C1, C1, C1, C1, C1, C1, C1, C1, C1,
  C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C1, C1, C1, C1, C1, C1,
  C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2,
  C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2,
  C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2,
  C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C2, C1,
  C2, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM,
  CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM,
  CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM,
  CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM,
  CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM,
  CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM,
  CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM,
  CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, CM, C1
};

typedef enum { FAILURE = -2, ACCEPT = -1, S0 = 0, S1, S2, S3 } state_t;
#define A ACCEPT
#define F FAILURE
static const signed char trans[][0x100] = {
  { /* S0   0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f */
    /* 0 */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* 1 */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* 2 */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* 3 */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* 4 */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* 5 */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* 6 */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* 7 */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* 8 */ F, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* 9 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* a */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* b */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* c */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* d */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* e */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* f */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, F
  },
  { /* S1   0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f */
    /* 0 */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* 1 */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* 2 */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* 3 */ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, F, F, F, F, F, F,
    /* 4 */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* 5 */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* 6 */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* 7 */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, F,
    /* 8 */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* 9 */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* a */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* b */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* c */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* d */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* e */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,
    /* f */ A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, F
  },
  { /* S2   0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f */
    /* 0 */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* 1 */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* 2 */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* 3 */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* 4 */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* 5 */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* 6 */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* 7 */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* 8 */ F, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    /* 9 */ 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    /* a */ 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    /* b */ 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    /* c */ 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    /* d */ 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    /* e */ 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    /* f */ 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, F
  },
  { /* S3   0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f */
    /* 0 */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* 1 */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* 2 */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* 3 */ A, A, A, A, A, A, A, A, A, A, F, F, F, F, F, F,
    /* 4 */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* 5 */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* 6 */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* 7 */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* 8 */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* 9 */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* a */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* b */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* c */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* d */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* e */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    /* f */ F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F
  }
};
#undef A
#undef F

static int
gb18030_mbc_enc_len(const UChar* p, const UChar* e, OnigEncoding enc ARG_UNUSED)
{
  int firstbyte = *p++;
  state_t s = trans[0][firstbyte];
#define RETURN(n) \
    return s == ACCEPT ? ONIGENC_CONSTRUCT_MBCLEN_CHARFOUND(n) : \
                         ONIGENC_CONSTRUCT_MBCLEN_INVALID()
  if (s < 0) RETURN(1);
  if (p == e) return ONIGENC_CONSTRUCT_MBCLEN_NEEDMORE(2-1);
  s = trans[s][*p++];
  if (s < 0) RETURN(2);
  if (p == e) return ONIGENC_CONSTRUCT_MBCLEN_NEEDMORE(4-2);
  s = trans[s][*p++];
  if (s < 0) RETURN(3);
  if (p == e) return ONIGENC_CONSTRUCT_MBCLEN_NEEDMORE(4-3);
  s = trans[s][*p++];
  RETURN(4);
#undef RETURN
}

static OnigCodePoint
gb18030_mbc_to_code(const UChar* p, const UChar* end, OnigEncoding enc)
{
  int c, i, len;
  OnigCodePoint n;

  len = enclen(enc, p, end);
  n = (OnigCodePoint )(*p++);
  if (len == 1) return n;

  for (i = 1; i < len; i++) {
    if (p >= end) break;
    c = *p++;
    n <<= 8;  n += c;
  }
  return n;
}

static int
gb18030_code_to_mbc(OnigCodePoint code, UChar *buf, OnigEncoding enc)
{
  return onigenc_mb4_code_to_mbc(enc, code, buf);
}

static int
gb18030_mbc_case_fold(OnigCaseFoldType flag, const UChar** pp, const UChar* end,
                      UChar* lower, OnigEncoding enc)
{
  return onigenc_mbn_mbc_case_fold(enc, flag,
                                   pp, end, lower);
}

#if 0
static int
gb18030_is_mbc_ambiguous(OnigCaseFoldType flag,
			 const UChar** pp, const UChar* end, OnigEncoding enc)
{
  return onigenc_mbn_is_mbc_ambiguous(enc, flag, pp, end);
}
#endif

static int
gb18030_is_code_ctype(OnigCodePoint code, unsigned int ctype, OnigEncoding enc)
{
  return onigenc_mb4_is_code_ctype(enc, code, ctype);
}

enum state {
  S_START,
  S_one_C2,
  S_one_C4,
  S_one_CM,

  S_odd_CM_one_CX,
  S_even_CM_one_CX,

  /* CMC4 : pair of "CM C4" */
  S_one_CMC4,
  S_odd_CMC4,
  S_one_C4_odd_CMC4,
  S_even_CMC4,
  S_one_C4_even_CMC4,

  S_odd_CM_odd_CMC4,
  S_even_CM_odd_CMC4,

  S_odd_CM_even_CMC4,
  S_even_CM_even_CMC4,

  /* C4CM : pair of "C4 CM" */
  S_odd_C4CM,
  S_one_CM_odd_C4CM,
  S_even_C4CM,
  S_one_CM_even_C4CM,

  S_even_CM_odd_C4CM,
  S_odd_CM_odd_C4CM,
  S_even_CM_even_C4CM,
  S_odd_CM_even_C4CM
};

static UChar*
gb18030_left_adjust_char_head(const UChar* start, const UChar* s, const UChar* end, OnigEncoding enc)
{
  const UChar *p;
  enum state state = S_START;

  DEBUG_GB18030(("----------------\n"));
  for (p = s; p >= start; p--) {
    DEBUG_GB18030(("state %d --(%02x)-->\n", state, *p));
    switch (state) {
    case S_START:
      switch (GB18030_MAP[*p]) {
      case C1:
	return (UChar *)s;
      case C2:
	state = S_one_C2; /* C2 */
	break;
      case C4:
	state = S_one_C4; /* C4 */
	break;
      case CM:
	state = S_one_CM; /* CM */
	break;
      }
      break;
    case S_one_C2: /* C2 */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)s;
      case CM:
	state = S_odd_CM_one_CX; /* CM C2 */
	break;
      }
      break;
    case S_one_C4: /* C4 */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)s;
      case CM:
	state = S_one_CMC4;
	break;
      }
      break;
    case S_one_CM: /* CM */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
	return (UChar *)s;
      case C4:
	state = S_odd_C4CM;
	break;
      case CM:
	state = S_odd_CM_one_CX; /* CM CM */
	break;
      }
      break;

    case S_odd_CM_one_CX: /* CM C2 */ /* CM CM */ /* CM CM CM C4 */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)(s - 1);
      case CM:
	state = S_even_CM_one_CX;
	break;
      }
      break;
    case S_even_CM_one_CX: /* CM CM C2 */ /* CM CM CM */ /* CM CM C4 */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)s;
      case CM:
	state = S_odd_CM_one_CX;
	break;
      }
      break;

    case S_one_CMC4: /* CM C4 */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
	return (UChar *)(s - 1);
      case C4:
	state = S_one_C4_odd_CMC4; /* C4 CM C4 */
	break;
      case CM:
	state = S_even_CM_one_CX; /* CM CM C4 */
	break;
      }
      break;
    case S_odd_CMC4: /* CM C4 CM C4 CM C4 */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
	return (UChar *)(s - 1);
      case C4:
	state = S_one_C4_odd_CMC4;
	break;
      case CM:
	state = S_odd_CM_odd_CMC4;
	break;
      }
      break;
    case S_one_C4_odd_CMC4: /* C4 CM C4 */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)(s - 1);
      case CM:
	state = S_even_CMC4; /* CM C4 CM C4 */
	break;
      }
      break;
    case S_even_CMC4: /* CM C4 CM C4 */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
	return (UChar *)(s - 3);
      case C4:
	state = S_one_C4_even_CMC4;
	break;
      case CM:
	state = S_odd_CM_even_CMC4;
	break;
      }
      break;
    case S_one_C4_even_CMC4: /* C4 CM C4 CM C4 */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)(s - 3);
      case CM:
	state = S_odd_CMC4;
	break;
      }
      break;

    case S_odd_CM_odd_CMC4: /* CM CM C4 CM C4 CM C4 */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)(s - 3);
      case CM:
	state = S_even_CM_odd_CMC4;
	break;
      }
      break;
    case S_even_CM_odd_CMC4: /* CM CM CM C4 CM C4 CM C4 */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)(s - 1);
      case CM:
	state = S_odd_CM_odd_CMC4;
	break;
      }
      break;

    case S_odd_CM_even_CMC4: /* CM CM C4 CM C4 */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)(s - 1);
      case CM:
	state = S_even_CM_even_CMC4;
	break;
      }
      break;
    case S_even_CM_even_CMC4: /* CM CM CM C4 CM C4 */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)(s - 3);
      case CM:
	state = S_odd_CM_even_CMC4;
	break;
      }
      break;

    case S_odd_C4CM: /* C4 CM */  /* C4 CM C4 CM C4 CM*/
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)s;
      case CM:
	state = S_one_CM_odd_C4CM; /* CM C4 CM */
	break;
      }
      break;
    case S_one_CM_odd_C4CM: /* CM C4 CM */ /* CM C4 CM C4 CM C4 CM */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
	return (UChar *)(s - 2); /* |CM C4 CM */
      case C4:
	state = S_even_C4CM;
	break;
      case CM:
	state = S_even_CM_odd_C4CM;
	break;
      }
      break;
    case S_even_C4CM: /* C4 CM C4 CM */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)(s - 2);  /* C4|CM C4 CM */
      case CM:
	state = S_one_CM_even_C4CM;
	break;
      }
      break;
    case S_one_CM_even_C4CM: /* CM C4 CM C4 CM */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
	return (UChar *)(s - 0);  /*|CM C4 CM C4|CM */
      case C4:
	state = S_odd_C4CM;
	break;
      case CM:
	state = S_even_CM_even_C4CM;
	break;
      }
      break;

    case S_even_CM_odd_C4CM: /* CM CM C4 CM */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)(s - 0); /* |CM CM|C4|CM */
      case CM:
	state = S_odd_CM_odd_C4CM;
	break;
      }
      break;
    case S_odd_CM_odd_C4CM: /* CM CM CM C4 CM */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)(s - 2); /* |CM CM|CM C4 CM */
      case CM:
	state = S_even_CM_odd_C4CM;
	break;
      }
      break;

    case S_even_CM_even_C4CM: /* CM CM C4 CM C4 CM */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)(s - 2); /* |CM CM|C4|CM C4 CM */
      case CM:
	state = S_odd_CM_even_C4CM;
	break;
      }
      break;
    case S_odd_CM_even_C4CM: /* CM CM CM C4 CM C4 CM */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)(s - 0);  /* |CM CM|CM C4 CM C4|CM */
      case CM:
	state = S_even_CM_even_C4CM;
	break;
      }
      break;
    }
  }

  DEBUG_GB18030(("state %d\n", state));
  switch (state) {
  case S_START:             return (UChar *)(s - 0);
  case S_one_C2:            return (UChar *)(s - 0);
  case S_one_C4:            return (UChar *)(s - 0);
  case S_one_CM:            return (UChar *)(s - 0);

  case S_odd_CM_one_CX:     return (UChar *)(s - 1);
  case S_even_CM_one_CX:    return (UChar *)(s - 0);

  case S_one_CMC4:          return (UChar *)(s - 1);
  case S_odd_CMC4:          return (UChar *)(s - 1);
  case S_one_C4_odd_CMC4:   return (UChar *)(s - 1);
  case S_even_CMC4:         return (UChar *)(s - 3);
  case S_one_C4_even_CMC4:  return (UChar *)(s - 3);

  case S_odd_CM_odd_CMC4:   return (UChar *)(s - 3);
  case S_even_CM_odd_CMC4:  return (UChar *)(s - 1);

  case S_odd_CM_even_CMC4:  return (UChar *)(s - 1);
  case S_even_CM_even_CMC4: return (UChar *)(s - 3);

  case S_odd_C4CM:          return (UChar *)(s - 0);
  case S_one_CM_odd_C4CM:   return (UChar *)(s - 2);
  case S_even_C4CM:         return (UChar *)(s - 2);
  case S_one_CM_even_C4CM:  return (UChar *)(s - 0);

  case S_even_CM_odd_C4CM:  return (UChar *)(s - 0);
  case S_odd_CM_odd_C4CM:   return (UChar *)(s - 2);
  case S_even_CM_even_C4CM: return (UChar *)(s - 2);
  case S_odd_CM_even_C4CM:  return (UChar *)(s - 0);
  }

  return (UChar* )s;  /* never come here. (escape warning) */
}

static int
gb18030_is_allowed_reverse_match(const UChar* s, const UChar* end ARG_UNUSED, OnigEncoding enc ARG_UNUSED)
{
  return GB18030_MAP[*s] == C1 ? TRUE : FALSE;
}

/*
 * Name: GB18030
 * MIBenum: 114
 * Link: http://www.iana.org/assignments/charset-reg/GB18030
 */
OnigEncodingDefine(gb18030, GB18030) = {
  gb18030_mbc_enc_len,
  "GB18030",   /* name */
  4,          /* max enc length */
  1,          /* min enc length */
  onigenc_is_mbc_newline_0x0a,
  gb18030_mbc_to_code,
  onigenc_mb4_code_to_mbclen,
  gb18030_code_to_mbc,
  gb18030_mbc_case_fold,
  onigenc_ascii_apply_all_case_fold,
  onigenc_ascii_get_case_fold_codes_by_str,
  onigenc_minimum_property_name_to_ctype,
  gb18030_is_code_ctype,
  onigenc_not_support_get_ctype_code_range,
  gb18030_left_adjust_char_head,
  gb18030_is_allowed_reverse_match,
  0,
  ONIGENC_FLAG_NONE,
};

