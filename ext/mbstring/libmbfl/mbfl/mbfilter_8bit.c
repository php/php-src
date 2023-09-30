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
 * by Moriyoshi Koizumi <moriyoshi@php.net> on 20 Dec 2002. The file
 * mbfilter.c is included in this package .
 *
 */

#include <stddef.h>

#include "mbfilter.h"

const struct mbfl_convert_vtbl vtbl_8bit_wchar;
const struct mbfl_convert_vtbl vtbl_wchar_8bit;
static int mbfl_filt_conv_8bit_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_8bit(int c, mbfl_convert_filter *filter);
static size_t mb_8bit_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_8bit(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

static const char *mbfl_encoding_8bit_aliases[] = {"binary", NULL};

const mbfl_encoding mbfl_encoding_8bit = {
	mbfl_no_encoding_8bit,
	"8bit",
	"8bit",
	mbfl_encoding_8bit_aliases,
	NULL,
	MBFL_ENCTYPE_SBCS,
	&vtbl_8bit_wchar,
	&vtbl_wchar_8bit,
	mb_8bit_to_wchar,
	mb_wchar_to_8bit,
	NULL,
	NULL
};

const struct mbfl_convert_vtbl vtbl_8bit_wchar = {
	mbfl_no_encoding_8bit,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_8bit_wchar,
	mbfl_filt_conv_common_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_8bit = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_8bit,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_8bit,
	mbfl_filt_conv_common_flush,
	NULL,
};

#define CK(statement) do { if ((statement) < 0) return (-1); } while (0)

static int mbfl_filt_conv_8bit_wchar(int c, mbfl_convert_filter *filter)
{
	return (*filter->output_function)(c, filter->data);
}

static int mbfl_filt_conv_wchar_8bit(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < 0x100) {
		CK((*filter->output_function)(c, filter->data));
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static size_t mb_8bit_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
  unsigned char *p = *in, *e = p + *in_len;
  uint32_t *out = buf, *limit = buf + bufsize;

  while (p < e && out < limit) {
    unsigned char c = *p++;
    *out++ = c;
  }

  *in_len = e - p;
  *in = p;
  return out - buf;
}

static void mb_wchar_to_8bit(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
  unsigned char *out, *limit;
  MB_CONVERT_BUF_LOAD(buf, out, limit);
  MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

  while (len--) {
    uint32_t w = *in++;
    if (w <= 0xFF) {
      out = mb_convert_buf_add(out, w);
    } else {
      MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_8bit);
      MB_CONVERT_BUF_ENSURE(buf, out, limit, len);
    }
  }

  MB_CONVERT_BUF_STORE(buf, out, limit);
}
