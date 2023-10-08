/*
 * "streamable kanji code filter and converter"
 * Copyright (c) 1998-2002 HappySize, Inc. All rights reserved.
 *
 * LICENSE NOTICES
 *
 * This file is part of "streamable kanji code filter and converter",
 * which is distributed under the terms of GNU Lesser General Public
 * License (version 2) as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with "streamable kanji code filter and converter";
 * if not, write to the Free Software Foundation, Inc., 59 Temple Place,
 * Suite 330, Boston, MA  02111-1307  USA
 *
 * The author of this file:
 *
 */
/*
 * The source code included in this files was separated from mbfilter.c
 * by Moriyoshi Koizumi <moriyoshi@php.net> on 4 Dec 2002. The file
 * mbfilter.c is included in this package .
 *
 */

#include "mbfilter.h"
#include "mbfilter_7bit.h"

static size_t mb_7bit_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_7bit(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

const struct mbfl_convert_vtbl vtbl_7bit_wchar = {
	mbfl_no_encoding_7bit,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_7bit_any,
	mbfl_filt_conv_common_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_7bit = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_7bit,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_any_7bit,
	mbfl_filt_conv_common_flush,
	NULL,
};

const mbfl_encoding mbfl_encoding_7bit = {
	mbfl_no_encoding_7bit,
	"7bit",
	"7bit",
	NULL,
	NULL,
	MBFL_ENCTYPE_SBCS,
	&vtbl_7bit_wchar,
	&vtbl_wchar_7bit,
	mb_7bit_to_wchar,
	mb_wchar_to_7bit,
	NULL,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

int mbfl_filt_conv_7bit_any(int c, mbfl_convert_filter *filter)
{
	return (*filter->output_function)(c < 0x80 ? c : MBFL_BAD_INPUT, filter->data);
}

int mbfl_filt_conv_any_7bit(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < 0x80) {
		CK((*filter->output_function)(c, filter->data));
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}
	return 0;
}

static size_t mb_7bit_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
  unsigned char *p = *in, *e = p + *in_len;
  uint32_t *out = buf, *limit = buf + bufsize;

  while (p < e && out < limit) {
    unsigned char c = *p++;
    *out++ = (c < 0x80) ? c : MBFL_BAD_INPUT;
  }

  *in_len = e - p;
  *in = p;
  return out - buf;
}

static void mb_wchar_to_7bit(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
  unsigned char *out, *limit;
  MB_CONVERT_BUF_LOAD(buf, out, limit);
  MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

  while (len--) {
    uint32_t w = *in++;
    if (w <= 0x7F) {
      out = mb_convert_buf_add(out, w);
    } else {
      MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_7bit);
      MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
    }
  }

  MB_CONVERT_BUF_STORE(buf, out, limit);
}
