/*
 * COPYRIGHT NOTICE
 *
 * This file is a portion of "streamable kanji code filter and converter"
 * library, which is distributed under GNU Lesser General Public License
 * version 2.1.
 *
 * The source code included in this files was separated from mbfilter.c
 * by Moriyoshi Koizumi <moriyoshi@php.net> on 4 Dec 2002.
 *
 */

#ifndef MBFL_MBFILTER_ISO8859_15_H
#define MBFL_MBFILTER_ISO8859_15_H

#include "mbfilter.h"

extern const mbfl_encoding mbfl_encoding_8859_15;
extern const struct mbfl_identify_vtbl vtbl_identify_8859_15;
extern const struct mbfl_convert_vtbl vtbl_8859_15_wchar;
extern const struct mbfl_convert_vtbl vtbl_wchar_8859_15;

int mbfl_filt_conv_8859_15_wchar(int c, mbfl_convert_filter *filter);
int mbfl_filt_conv_wchar_8859_15(int c, mbfl_convert_filter *filter);

#endif /* MBFL_MBFILTER_ISO8859_15_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */

