/*
 * COPYRIGHT NOTICE
 *
 * This file is a portion of "streamable kanji code filter and converter"
 * library, which is distributed under GNU Lesser General Public License
 * version 2.1.  
 * 
 */

#ifndef MBFL_MBFILTER_ISO8859_16_H
#define MBFL_MBFILTER_ISO8859_16_H

#include "mbfilter.h"

extern const mbfl_encoding mbfl_encoding_8859_16;
extern const struct mbfl_identify_vtbl vtbl_identify_8859_16;
extern const struct mbfl_convert_vtbl vtbl_8859_16_wchar;
extern const struct mbfl_convert_vtbl vtbl_wchar_8859_16;

int mbfl_filt_conv_8859_16_wchar(int c, mbfl_convert_filter *filter);
int mbfl_filt_conv_wchar_8859_16(int c, mbfl_convert_filter *filter);

#endif /* MBFL_MBFILTER_ISO8859_16_H */
