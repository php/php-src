#ifndef MBFL_MBFILTER_CJK_H
#define MBFL_MBFILTER_CJK_H

#include "mbfilter.h"

extern const mbfl_encoding mbfl_encoding_jis;
extern const mbfl_encoding mbfl_encoding_2022jp;
extern const mbfl_encoding mbfl_encoding_2022jp_kddi;
extern const mbfl_encoding mbfl_encoding_2022jpms;
extern const mbfl_encoding mbfl_encoding_2022jp_2004;
extern const mbfl_encoding mbfl_encoding_cp50220;
extern const mbfl_encoding mbfl_encoding_cp50221;
extern const mbfl_encoding mbfl_encoding_cp50222;
extern const mbfl_encoding mbfl_encoding_2022kr;

extern const mbfl_encoding mbfl_encoding_sjis;
extern const mbfl_encoding mbfl_encoding_sjis_mac;
extern const mbfl_encoding mbfl_encoding_sjis_docomo;
extern const mbfl_encoding mbfl_encoding_sjis_kddi;
extern const mbfl_encoding mbfl_encoding_sjis_sb;
extern const mbfl_encoding mbfl_encoding_sjis2004;
extern const mbfl_encoding mbfl_encoding_cp932;
extern const mbfl_encoding mbfl_encoding_sjiswin;

extern const mbfl_encoding mbfl_encoding_euc_jp;
extern const mbfl_encoding mbfl_encoding_eucjp_win;
extern const mbfl_encoding mbfl_encoding_eucjp2004;
extern const mbfl_encoding mbfl_encoding_cp51932;
extern const mbfl_encoding mbfl_encoding_euc_cn;
extern const mbfl_encoding mbfl_encoding_euc_tw;
extern const mbfl_encoding mbfl_encoding_euc_kr;
extern const mbfl_encoding mbfl_encoding_uhc;

extern const mbfl_encoding mbfl_encoding_gb18030;
extern const mbfl_encoding mbfl_encoding_gb18030_2022;
extern const mbfl_encoding mbfl_encoding_cp936;
extern const mbfl_encoding mbfl_encoding_big5;
extern const mbfl_encoding mbfl_encoding_cp950;
extern const mbfl_encoding mbfl_encoding_hz;

int mbfilter_sjis_emoji_docomo2unicode(int s, int *snd);
int mbfilter_sjis_emoji_kddi2unicode(int s, int *snd);
int mbfilter_sjis_emoji_sb2unicode(int s, int *snd);

int mbfilter_unicode2sjis_emoji_docomo(int c, int *s1, mbfl_convert_filter *filter);
int mbfilter_unicode2sjis_emoji_kddi_sjis(int c, int *s1, mbfl_convert_filter *filter);
int mbfilter_unicode2sjis_emoji_sb(int c, int *s1, mbfl_convert_filter *filter);

#endif /* MBFL_MBFILTER_CJK_H */
