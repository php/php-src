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

#ifndef MBFL_MBFILTER_ISO8859_14_H
#define MBFL_MBFILTER_ISO8859_14_H

#include "mbfilter.h"

extern const mbfl_encoding mbfl_encoding_8859_14;
extern const struct mbfl_identify_vtbl vtbl_identify_8859_14;
extern const struct mbfl_convert_vtbl vtbl_8859_14_wchar;
extern const struct mbfl_convert_vtbl vtbl_wchar_8859_14;

int mbfl_filt_conv_8859_14_wchar(int c, mbfl_convert_filter *filter);
int mbfl_filt_conv_wchar_8859_14(int c, mbfl_convert_filter *filter);

#endif /* MBFL_MBFILTER_ISO8859_14_H */
