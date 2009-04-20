/**********************************************************************
  regenc.c -  Oniguruma (regular expression library)
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

#include "regint.h"

OnigEncoding OnigEncDefaultCharEncoding = ONIG_ENCODING_INIT_DEFAULT;

extern int
onigenc_init(void)
{
  return 0;
}

extern OnigEncoding
onigenc_get_default_encoding(void)
{
  return OnigEncDefaultCharEncoding;
}

extern int
onigenc_set_default_encoding(OnigEncoding enc)
{
  OnigEncDefaultCharEncoding = enc;
  return 0;
}

extern UChar*
onigenc_get_right_adjust_char_head(OnigEncoding enc, const UChar* start, const UChar* s)
{
  UChar* p = ONIGENC_LEFT_ADJUST_CHAR_HEAD(enc, start, s);
  if (p < s) {
    p += enc_len(enc, p);
  }
  return p;
}

extern UChar*
onigenc_get_right_adjust_char_head_with_prev(OnigEncoding enc,
				   const UChar* start, const UChar* s, const UChar** prev)
{
  UChar* p = ONIGENC_LEFT_ADJUST_CHAR_HEAD(enc, start, s);

  if (p < s) {
    if (prev) *prev = (const UChar* )p;
    p += enc_len(enc, p);
  }
  else {
    if (prev) *prev = (const UChar* )NULL; /* Sorry */
  }
  return p;
}

extern UChar*
onigenc_get_prev_char_head(OnigEncoding enc, const UChar* start, const UChar* s)
{
  if (s <= start)
    return (UChar* )NULL;

  return ONIGENC_LEFT_ADJUST_CHAR_HEAD(enc, start, s - 1);
}

extern UChar*
onigenc_step_back(OnigEncoding enc, const UChar* start, const UChar* s, int n)
{
  while (ONIG_IS_NOT_NULL(s) && n-- > 0) {
    if (s <= start)
      return (UChar* )NULL;

    s = ONIGENC_LEFT_ADJUST_CHAR_HEAD(enc, start, s - 1);
  }
  return (UChar* )s;
}

extern UChar*
onigenc_step(OnigEncoding enc, const UChar* p, const UChar* end, int n)
{
  UChar* q = (UChar* )p;
  while (n-- > 0) {
    q += ONIGENC_MBC_ENC_LEN(enc, q);
  }
  return (q <= end ? q : NULL);
}

extern int
onigenc_strlen(OnigEncoding enc, const UChar* p, const UChar* end)
{
  int n = 0;
  UChar* q = (UChar* )p;
  
  while (q < end) {
    q += ONIGENC_MBC_ENC_LEN(enc, q);
    n++;
  }
  return n;
}

extern int
onigenc_strlen_null(OnigEncoding enc, const UChar* s)
{
  int n = 0;
  UChar* p = (UChar* )s;
  
  while (1) {
    if (*p == '\0') {
      UChar* q;
      int len = ONIGENC_MBC_MINLEN(enc);

      if (len == 1) return n;
      q = p + 1;
      while (len > 1) {
        if (*q != '\0') break;
        q++;
        len--;
      }
      if (len == 1) return n;
    }
    p += ONIGENC_MBC_ENC_LEN(enc, p);
    n++;
  }
}

extern int
onigenc_str_bytelen_null(OnigEncoding enc, const UChar* s)
{
  UChar* start = (UChar* )s;
  UChar* p = (UChar* )s;

  while (1) {
    if (*p == '\0') {
      UChar* q;
      int len = ONIGENC_MBC_MINLEN(enc);

      if (len == 1) return (int )(p - start);
      q = p + 1;
      while (len > 1) {
        if (*q != '\0') break;
        q++;
        len--;
      }
      if (len == 1) return (int )(p - start);
    }
    p += ONIGENC_MBC_ENC_LEN(enc, p);
  }
}

#ifndef ONIG_RUBY_M17N

#ifndef NOT_RUBY

#define USE_APPLICATION_TO_LOWER_CASE_TABLE

const unsigned short OnigEnc_Unicode_ISO_8859_1_CtypeTable[256] = {
  0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008,
  0x2008, 0x228c, 0x2289, 0x2288, 0x2288, 0x2288, 0x2008, 0x2008,
  0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008,
  0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008,
  0x2284, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0,
  0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0,
  0x38b0, 0x38b0, 0x38b0, 0x38b0, 0x38b0, 0x38b0, 0x38b0, 0x38b0,
  0x38b0, 0x38b0, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0,
  0x21a0, 0x3ca2, 0x3ca2, 0x3ca2, 0x3ca2, 0x3ca2, 0x3ca2, 0x34a2,
  0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2,
  0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2,
  0x34a2, 0x34a2, 0x34a2, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x31a0,
  0x21a0, 0x38e2, 0x38e2, 0x38e2, 0x38e2, 0x38e2, 0x38e2, 0x30e2,
  0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2,
  0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2,
  0x30e2, 0x30e2, 0x30e2, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x2008,
  0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0288, 0x0008, 0x0008,
  0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008,
  0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008,
  0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008,
  0x0284, 0x01a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0,
  0x00a0, 0x00a0, 0x10e2, 0x01a0, 0x00a0, 0x00a8, 0x00a0, 0x00a0,
  0x00a0, 0x00a0, 0x10a0, 0x10a0, 0x00a0, 0x10e2, 0x00a0, 0x01a0,
  0x00a0, 0x10a0, 0x10e2, 0x01a0, 0x10a0, 0x10a0, 0x10a0, 0x01a0,
  0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2,
  0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2,
  0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x00a0,
  0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x10e2,
  0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2,
  0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2,
  0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x00a0,
  0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2
};
#endif

const UChar* OnigEncAsciiToLowerCaseTable = (const UChar* )0;

#ifndef USE_APPLICATION_TO_LOWER_CASE_TABLE
static const UChar BuiltInAsciiToLowerCaseTable[] = {
  '\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
  '\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
  '\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
  '\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
  '\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
  '\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
  '\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
  '\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
  '\100', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
  '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
  '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
  '\170', '\171', '\172', '\133', '\134', '\135', '\136', '\137',
  '\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
  '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
  '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
  '\170', '\171', '\172', '\173', '\174', '\175', '\176', '\177',
  '\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
  '\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
  '\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
  '\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
  '\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
  '\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
  '\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
  '\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
  '\300', '\301', '\302', '\303', '\304', '\305', '\306', '\307',
  '\310', '\311', '\312', '\313', '\314', '\315', '\316', '\317',
  '\320', '\321', '\322', '\323', '\324', '\325', '\326', '\327',
  '\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337',
  '\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
  '\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
  '\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
  '\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377',
};
#endif /* not USE_APPLICATION_TO_LOWER_CASE_TABLE */

#ifdef USE_UPPER_CASE_TABLE
const UChar OnigEncAsciiToUpperCaseTable[256] = {
  '\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
  '\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
  '\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
  '\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
  '\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
  '\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
  '\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
  '\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
  '\100', '\101', '\102', '\103', '\104', '\105', '\106', '\107',
  '\110', '\111', '\112', '\113', '\114', '\115', '\116', '\117',
  '\120', '\121', '\122', '\123', '\124', '\125', '\126', '\127',
  '\130', '\131', '\132', '\133', '\134', '\135', '\136', '\137',
  '\140', '\101', '\102', '\103', '\104', '\105', '\106', '\107',
  '\110', '\111', '\112', '\113', '\114', '\115', '\116', '\117',
  '\120', '\121', '\122', '\123', '\124', '\125', '\126', '\127',
  '\130', '\131', '\132', '\173', '\174', '\175', '\176', '\177',
  '\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
  '\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
  '\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
  '\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
  '\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
  '\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
  '\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
  '\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
  '\300', '\301', '\302', '\303', '\304', '\305', '\306', '\307',
  '\310', '\311', '\312', '\313', '\314', '\315', '\316', '\317',
  '\320', '\321', '\322', '\323', '\324', '\325', '\326', '\327',
  '\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337',
  '\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
  '\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
  '\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
  '\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377',
};
#endif

const unsigned short OnigEncAsciiCtypeTable[256] = {
  0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008,
  0x2008, 0x220c, 0x2209, 0x2208, 0x2208, 0x2208, 0x2008, 0x2008,
  0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008,
  0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008,
  0x2284, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0,
  0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0,
  0x38b0, 0x38b0, 0x38b0, 0x38b0, 0x38b0, 0x38b0, 0x38b0, 0x38b0,
  0x38b0, 0x38b0, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0,
  0x21a0, 0x3ca2, 0x3ca2, 0x3ca2, 0x3ca2, 0x3ca2, 0x3ca2, 0x34a2,
  0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2,
  0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2,
  0x34a2, 0x34a2, 0x34a2, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x31a0,
  0x21a0, 0x38e2, 0x38e2, 0x38e2, 0x38e2, 0x38e2, 0x38e2, 0x30e2,
  0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2,
  0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2,
  0x30e2, 0x30e2, 0x30e2, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x2008,

  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};

const UChar OnigEncISO_8859_1_ToLowerCaseTable[256] = {
  '\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
  '\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
  '\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
  '\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
  '\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
  '\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
  '\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
  '\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
  '\100', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
  '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
  '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
  '\170', '\171', '\172', '\133', '\134', '\135', '\136', '\137',
  '\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
  '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
  '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
  '\170', '\171', '\172', '\173', '\174', '\175', '\176', '\177',
  '\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
  '\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
  '\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
  '\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
  '\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
  '\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
  '\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
  '\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
  '\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
  '\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
  '\360', '\361', '\362', '\363', '\364', '\365', '\366', '\327',
  '\370', '\371', '\372', '\373', '\374', '\375', '\376', '\337',
  '\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
  '\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
  '\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
  '\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377'
};

#ifdef USE_UPPER_CASE_TABLE
const UChar OnigEncISO_8859_1_ToUpperCaseTable[256] = {
  '\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
  '\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
  '\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
  '\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
  '\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
  '\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
  '\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
  '\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
  '\100', '\101', '\102', '\103', '\104', '\105', '\106', '\107',
  '\110', '\111', '\112', '\113', '\114', '\115', '\116', '\117',
  '\120', '\121', '\122', '\123', '\124', '\125', '\126', '\127',
  '\130', '\131', '\132', '\133', '\134', '\135', '\136', '\137',
  '\140', '\101', '\102', '\103', '\104', '\105', '\106', '\107',
  '\110', '\111', '\112', '\113', '\114', '\115', '\116', '\117',
  '\120', '\121', '\122', '\123', '\124', '\125', '\126', '\127',
  '\130', '\131', '\132', '\173', '\174', '\175', '\176', '\177',
  '\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
  '\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
  '\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
  '\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
  '\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
  '\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
  '\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
  '\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
  '\300', '\301', '\302', '\303', '\304', '\305', '\306', '\307',
  '\310', '\311', '\312', '\313', '\314', '\315', '\316', '\317',
  '\320', '\321', '\322', '\323', '\324', '\325', '\326', '\327',
  '\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337',
  '\300', '\301', '\302', '\303', '\304', '\305', '\306', '\307',
  '\310', '\311', '\312', '\313', '\314', '\315', '\316', '\317',
  '\320', '\321', '\322', '\323', '\324', '\325', '\326', '\367',
  '\330', '\331', '\332', '\333', '\334', '\335', '\336', '\377',
};
#endif

extern void
onigenc_set_default_caseconv_table(const UChar* table)
{
  if (table == (const UChar* )0) {
#ifndef USE_APPLICATION_TO_LOWER_CASE_TABLE
    table = BuiltInAsciiToLowerCaseTable;
#else
    return ;
#endif
  }

  if (table != OnigEncAsciiToLowerCaseTable) {
    OnigEncAsciiToLowerCaseTable = table;
  }
}

extern UChar*
onigenc_get_left_adjust_char_head(OnigEncoding enc, const UChar* start, const UChar* s)
{
  return ONIGENC_LEFT_ADJUST_CHAR_HEAD(enc, start, s);
}

const OnigPairAmbigCodes OnigAsciiPairAmbigCodes[] = {
  { 0x41, 0x61 },
  { 0x42, 0x62 },
  { 0x43, 0x63 },
  { 0x44, 0x64 },
  { 0x45, 0x65 },
  { 0x46, 0x66 },
  { 0x47, 0x67 },
  { 0x48, 0x68 },
  { 0x49, 0x69 },
  { 0x4a, 0x6a },
  { 0x4b, 0x6b },
  { 0x4c, 0x6c },
  { 0x4d, 0x6d },
  { 0x4e, 0x6e },
  { 0x4f, 0x6f },
  { 0x50, 0x70 },
  { 0x51, 0x71 },
  { 0x52, 0x72 },
  { 0x53, 0x73 },
  { 0x54, 0x74 },
  { 0x55, 0x75 },
  { 0x56, 0x76 },
  { 0x57, 0x77 },
  { 0x58, 0x78 },
  { 0x59, 0x79 },
  { 0x5a, 0x7a },

  { 0x61, 0x41 },
  { 0x62, 0x42 },
  { 0x63, 0x43 },
  { 0x64, 0x44 },
  { 0x65, 0x45 },
  { 0x66, 0x46 },
  { 0x67, 0x47 },
  { 0x68, 0x48 },
  { 0x69, 0x49 },
  { 0x6a, 0x4a },
  { 0x6b, 0x4b },
  { 0x6c, 0x4c },
  { 0x6d, 0x4d },
  { 0x6e, 0x4e },
  { 0x6f, 0x4f },
  { 0x70, 0x50 },
  { 0x71, 0x51 },
  { 0x72, 0x52 },
  { 0x73, 0x53 },
  { 0x74, 0x54 },
  { 0x75, 0x55 },
  { 0x76, 0x56 },
  { 0x77, 0x57 },
  { 0x78, 0x58 },
  { 0x79, 0x59 },
  { 0x7a, 0x5a }
};

extern int
onigenc_ascii_get_all_pair_ambig_codes(OnigAmbigType flag,
                                       const OnigPairAmbigCodes** ccs)
{
  if (flag == ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE) {
    *ccs = OnigAsciiPairAmbigCodes;
    return (sizeof(OnigAsciiPairAmbigCodes) / sizeof(OnigPairAmbigCodes));
  }
  else {
    return 0;
  }
}

extern int
onigenc_nothing_get_all_comp_ambig_codes(OnigAmbigType flag,
                                         const OnigCompAmbigCodes** ccs)
{
  return 0;
}

extern int
onigenc_iso_8859_1_get_all_pair_ambig_codes(OnigAmbigType flag,
                                            const OnigPairAmbigCodes** ccs)
{
  static const OnigPairAmbigCodes cc[] = {
    { 0xc0, 0xe0 },
    { 0xc1, 0xe1 },
    { 0xc2, 0xe2 },
    { 0xc3, 0xe3 },
    { 0xc4, 0xe4 },
    { 0xc5, 0xe5 },
    { 0xc6, 0xe6 },
    { 0xc7, 0xe7 },
    { 0xc8, 0xe8 },
    { 0xc9, 0xe9 },
    { 0xca, 0xea },
    { 0xcb, 0xeb },
    { 0xcc, 0xec },
    { 0xcd, 0xed },
    { 0xce, 0xee },
    { 0xcf, 0xef },

    { 0xd0, 0xf0 },
    { 0xd1, 0xf1 },
    { 0xd2, 0xf2 },
    { 0xd3, 0xf3 },
    { 0xd4, 0xf4 },
    { 0xd5, 0xf5 },
    { 0xd6, 0xf6 },
    { 0xd8, 0xf8 },
    { 0xd9, 0xf9 },
    { 0xda, 0xfa },
    { 0xdb, 0xfb },
    { 0xdc, 0xfc },
    { 0xdd, 0xfd },
    { 0xde, 0xfe },

    { 0xe0, 0xc0 },
    { 0xe1, 0xc1 },
    { 0xe2, 0xc2 },
    { 0xe3, 0xc3 },
    { 0xe4, 0xc4 },
    { 0xe5, 0xc5 },
    { 0xe6, 0xc6 },
    { 0xe7, 0xc7 },
    { 0xe8, 0xc8 },
    { 0xe9, 0xc9 },
    { 0xea, 0xca },
    { 0xeb, 0xcb },
    { 0xec, 0xcc },
    { 0xed, 0xcd },
    { 0xee, 0xce },
    { 0xef, 0xcf },

    { 0xf0, 0xd0 },
    { 0xf1, 0xd1 },
    { 0xf2, 0xd2 },
    { 0xf3, 0xd3 },
    { 0xf4, 0xd4 },
    { 0xf5, 0xd5 },
    { 0xf6, 0xd6 },
    { 0xf8, 0xd8 },
    { 0xf9, 0xd9 },
    { 0xfa, 0xda },
    { 0xfb, 0xdb },
    { 0xfc, 0xdc },
    { 0xfd, 0xdd },
    { 0xfe, 0xde }
  };

  if (flag == ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE) {
    *ccs = OnigAsciiPairAmbigCodes;
    return (sizeof(OnigAsciiPairAmbigCodes) / sizeof(OnigPairAmbigCodes));
  }
  else if (flag == ONIGENC_AMBIGUOUS_MATCH_NONASCII_CASE) {
    *ccs = cc;
    return sizeof(cc) / sizeof(OnigPairAmbigCodes);
  }
  else
    return 0;
}

extern int
onigenc_ess_tsett_get_all_comp_ambig_codes(OnigAmbigType flag,
                                           const OnigCompAmbigCodes** ccs)
{
  static const OnigCompAmbigCodes folds[] = {
    { 2, 0xdf, {{ 2, { 0x53, 0x53 } }, { 2, { 0x73, 0x73} } } }
  };

  if (flag == ONIGENC_AMBIGUOUS_MATCH_NONASCII_CASE) {
    *ccs = folds;
    return sizeof(folds) / sizeof(OnigCompAmbigCodes);
  }
  else
    return 0;
}

extern int
onigenc_not_support_get_ctype_code_range(int ctype,
                             const OnigCodePoint* sbr[], const OnigCodePoint* mbr[])
{
  return ONIG_NO_SUPPORT_CONFIG;
}

extern int
onigenc_is_mbc_newline_0x0a(const UChar* p, const UChar* end)
{
  if (p < end) {
    if (*p == 0x0a) return 1;
  }
  return 0;
}

/* for single byte encodings */
extern int
onigenc_ascii_mbc_to_normalize(OnigAmbigType flag, const UChar** p, const UChar*end,
                               UChar* lower)
{
  if ((flag & ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE) != 0) {
    *lower = ONIGENC_ASCII_CODE_TO_LOWER_CASE(**p);
  }
  else {
    *lower = **p;
  }

  (*p)++;
  return 1; /* return byte length of converted char to lower */
}

extern int
onigenc_ascii_is_mbc_ambiguous(OnigAmbigType flag,
			       const UChar** pp, const UChar* end)
{
  const UChar* p = *pp;

  (*pp)++;
  if ((flag & ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE) != 0) {
    return ONIGENC_IS_ASCII_CODE_CASE_AMBIG(*p);
  }
  else {
    return FALSE;
  }
}

extern int
onigenc_single_byte_mbc_enc_len(const UChar* p)
{
  return 1;
}

extern OnigCodePoint
onigenc_single_byte_mbc_to_code(const UChar* p, const UChar* end)
{
  return (OnigCodePoint )(*p);
}

extern int
onigenc_single_byte_code_to_mbclen(OnigCodePoint code)
{
  return 1;
}

extern int
onigenc_single_byte_code_to_mbc_first(OnigCodePoint code)
{
  return (code & 0xff);
}

extern int
onigenc_single_byte_code_to_mbc(OnigCodePoint code, UChar *buf)
{
  *buf = (UChar )(code & 0xff);
  return 1;
}

extern UChar*
onigenc_single_byte_left_adjust_char_head(const UChar* start, const UChar* s)
{
  return (UChar* )s;
}

extern int
onigenc_always_true_is_allowed_reverse_match(const UChar* s, const UChar* end)
{
  return TRUE;
}

extern int
onigenc_always_false_is_allowed_reverse_match(const UChar* s, const UChar* end)
{
  return FALSE;
}

extern OnigCodePoint
onigenc_mbn_mbc_to_code(OnigEncoding enc, const UChar* p, const UChar* end)
{
  int c, i, len;
  OnigCodePoint n;

  len = enc_len(enc, p);
  n = (OnigCodePoint )(*p++);
  if (len == 1) return n;

  for (i = 1; i < len; i++) {
    if (p >= end) break;
    c = *p++;
    n <<= 8;  n += c;
  }
  return n;
}

extern int
onigenc_mbn_mbc_to_normalize(OnigEncoding enc, OnigAmbigType flag,
                             const UChar** pp, const UChar* end, UChar* lower)
{
  int len;
  const UChar *p = *pp;

  if (ONIGENC_IS_MBC_ASCII(p)) {
    if ((flag & ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE) != 0) {
      *lower = ONIGENC_ASCII_CODE_TO_LOWER_CASE(*p);
    }
    else {
      *lower = *p;
    }
    (*pp)++;
    return 1;
  }
  else {
    len = enc_len(enc, p);
    if (lower != p) {
      int i;
      for (i = 0; i < len; i++) {
	*lower++ = *p++;
      }
    }
    (*pp) += len;
    return len; /* return byte length of converted to lower char */
  }
}

extern int
onigenc_mbn_is_mbc_ambiguous(OnigEncoding enc, OnigAmbigType flag,
                             const UChar** pp, const UChar* end)
{
  const UChar* p = *pp;

  if (ONIGENC_IS_MBC_ASCII(p)) {
    (*pp)++;
    if ((flag & ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE) != 0) {
      return ONIGENC_IS_ASCII_CODE_CASE_AMBIG(*p);
    }
    else {
      return FALSE;
    }
  }

  (*pp) += enc_len(enc, p);
  return FALSE;
}

extern int
onigenc_mb2_code_to_mbclen(OnigCodePoint code)
{
  if ((code & 0xff00) != 0) return 2;
  else return 1;
}

extern int
onigenc_mb4_code_to_mbclen(OnigCodePoint code)
{
       if ((code & 0xff000000) != 0) return 4;
  else if ((code & 0xff0000) != 0) return 3;
  else if ((code & 0xff00) != 0) return 2;
  else return 1;
}

extern int
onigenc_mb2_code_to_mbc_first(OnigCodePoint code)
{
  int first;

  if ((code & 0xff00) != 0) {
    first = (code >> 8) & 0xff;
  }
  else {
    return (int )code;
  }
  return first;
}

extern int
onigenc_mb4_code_to_mbc_first(OnigCodePoint code)
{
  int first;

  if ((code & 0xff000000) != 0) {
    first = (code >> 24) & 0xff;
  }
  else if ((code & 0xff0000) != 0) {
    first = (code >> 16) & 0xff;
  }
  else if ((code & 0xff00) != 0) {
    first = (code >>  8) & 0xff;
  }
  else {
    return (int )code;
  }
  return first;
}

extern int
onigenc_mb2_code_to_mbc(OnigEncoding enc, OnigCodePoint code, UChar *buf)
{
  UChar *p = buf;

  if ((code & 0xff00) != 0) {
    *p++ = (UChar )((code >>  8) & 0xff);
  }
  *p++ = (UChar )(code & 0xff);

#if 1
  if (enc_len(enc, buf) != (p - buf))
    return ONIGENCERR_INVALID_WIDE_CHAR_VALUE;
#endif
  return p - buf;
}

extern int
onigenc_mb4_code_to_mbc(OnigEncoding enc, OnigCodePoint code, UChar *buf)
{
  UChar *p = buf;

  if ((code & 0xff000000) != 0) {
    *p++ = (UChar )((code >> 24) & 0xff);
  }
  if ((code & 0xff0000) != 0 || p != buf) {
    *p++ = (UChar )((code >> 16) & 0xff);
  }
  if ((code & 0xff00) != 0 || p != buf) {
    *p++ = (UChar )((code >> 8) & 0xff);
  }
  *p++ = (UChar )(code & 0xff);

#if 1
  if (enc_len(enc, buf) != (p - buf))
    return ONIGENCERR_INVALID_WIDE_CHAR_VALUE;
#endif
  return p - buf;
}

extern int
onigenc_mb2_is_code_ctype(OnigEncoding enc, OnigCodePoint code,
			  unsigned int ctype)
{
  if (code < 128)
    return ONIGENC_IS_ASCII_CODE_CTYPE(code, ctype);
  else {
    if ((ctype & (ONIGENC_CTYPE_WORD |
                  ONIGENC_CTYPE_GRAPH | ONIGENC_CTYPE_PRINT)) != 0) {
      return (ONIGENC_CODE_TO_MBCLEN(enc, code) > 1 ? TRUE : FALSE);
    }
  }

  return FALSE;
}

extern int
onigenc_mb4_is_code_ctype(OnigEncoding enc, OnigCodePoint code,
			  unsigned int ctype)
{
  if (code < 128)
    return ONIGENC_IS_ASCII_CODE_CTYPE(code, ctype);
  else {
    if ((ctype & (ONIGENC_CTYPE_WORD |
                  ONIGENC_CTYPE_GRAPH | ONIGENC_CTYPE_PRINT)) != 0) {
      return (ONIGENC_CODE_TO_MBCLEN(enc, code) > 1 ? TRUE : FALSE);
    }
  }

  return FALSE;
}

extern int
onigenc_with_ascii_strncmp(OnigEncoding enc, const UChar* p, const UChar* end,
                           const UChar* sascii /* ascii */, int n)
{
  int x, c;

  while (n-- > 0) {
    if (p >= end) return (int )(*sascii);

    c = (int )ONIGENC_MBC_TO_CODE(enc, p, end);
    x = *sascii - c;
    if (x) return x;

    sascii++;
    p += enc_len(enc, p);
  }
  return 0;
}

#else /* ONIG_RUBY_M17N */

extern int
onigenc_is_code_ctype(OnigEncoding enc, OnigCodePoint code, int ctype)
{
  switch (ctype) {
  case ONIGENC_CTYPE_NEWLINE:
    if (code == 0x0a) return 1;
    break;

  case ONIGENC_CTYPE_ALPHA:
    return m17n_isalpha(enc, code);
    break;
  case ONIGENC_CTYPE_BLANK:
    return ONIGENC_IS_CODE_BLANK(enc, (int )(code));
    break;
  case ONIGENC_CTYPE_CNTRL:
    return m17n_iscntrl(enc, code);
    break;
  case ONIGENC_CTYPE_DIGIT:
    return m17n_isdigit(enc, code);
    break;
  case ONIGENC_CTYPE_GRAPH:
    return ONIGENC_IS_CODE_GRAPH(enc, (int )(code));
    break;
  case ONIGENC_CTYPE_LOWER:
    return m17n_islower(enc, code);
    break;
  case ONIGENC_CTYPE_PRINT:
    return m17n_isprint(enc, code);
    break;
  case ONIGENC_CTYPE_PUNCT:
    return m17n_ispunct(enc, code);
    break;
  case ONIGENC_CTYPE_SPACE:
    return m17n_isspace(enc, code);
    break;
  case ONIGENC_CTYPE_UPPER:
    return m17n_isupper(enc, code);
    break;
  case ONIGENC_CTYPE_XDIGIT:
    return m17n_isxdigit(enc, code);
    break;
  case ONIGENC_CTYPE_WORD:
    return m17n_iswchar(enc, code);
    break;
  case ONIGENC_CTYPE_ASCII:
    return (code < 128 ? TRUE : FALSE);
    break;
  case ONIGENC_CTYPE_ALNUM:
    return m17n_isalnum(enc, code);
    break;
  default:
    break;
  }

  return 0;
}

extern int
onigenc_code_to_mbc(OnigEncoding enc, OnigCodePoint code, UChar *buf)
{
  int c, len;

  m17n_mbcput(enc, code, buf);
  c = m17n_firstbyte(enc, code);
  len = enc_len(enc, c);
  return len;
}

extern int
onigenc_mbc_to_lower(OnigEncoding enc, UChar* p, UChar* buf)
{
  unsigned int c, low;

  c   = m17n_codepoint(enc, p, p + enc_len(enc, *p));
  low = m17n_tolower(enc, c);
  m17n_mbcput(enc, low, buf);

  return m17n_codelen(enc, low);
}

extern int
onigenc_is_mbc_ambiguous(OnigEncoding enc, OnigAmbigType flag,
                         UChar** pp, UChar* end)
{
  int len;
  unsigned int c;
  UChar* p = *pp;

  len = enc_len(enc, *p);
  (*pp) += len;
  c = m17n_codepoint(enc, p, p + len);

  if ((flag & ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE) != 0) {
    if (m17n_isupper(enc, c) || m17n_islower(enc, c))
      return TRUE;
  }

  return FALSE;
}

extern UChar*
onigenc_get_left_adjust_char_head(OnigEncoding enc, UChar* start, UChar* s)
{
  UChar *p;
  int len;

  if (s <= start) return s;
  p = s;

  while (!m17n_islead(enc, *p) && p > start) p--;
  while (p + (len = enc_len(enc, *p)) < s) {
    p += len;
  }
  if (p + len == s) return s;
  return p;
}

extern int
onigenc_is_allowed_reverse_match(OnigEncoding enc,
				 const UChar* s, const UChar* end)
{
  return ONIGENC_IS_SINGLEBYTE(enc);
}

extern void
onigenc_set_default_caseconv_table(UChar* table) { }

#endif /* ONIG_RUBY_M17N */
