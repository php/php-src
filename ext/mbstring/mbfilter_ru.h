#ifndef MBFL_MBFILTER_RU_H
#define MBFL_MBFILTER_RU_H

int mbfl_filt_conv_cp1251_wchar(int c, mbfl_convert_filter *filter TSRMLS_DC);
int mbfl_filt_conv_wchar_cp1251(int c, mbfl_convert_filter *filter TSRMLS_DC);
int mbfl_filt_conv_cp866_wchar(int c, mbfl_convert_filter *filter TSRMLS_DC);
int mbfl_filt_conv_wchar_cp866(int c, mbfl_convert_filter *filter TSRMLS_DC);
int mbfl_filt_conv_koi8r_wchar(int c, mbfl_convert_filter *filter TSRMLS_DC);
int mbfl_filt_conv_wchar_koi8r(int c, mbfl_convert_filter *filter TSRMLS_DC);

#endif /* MBFL_MBFILTER_RU_H */
