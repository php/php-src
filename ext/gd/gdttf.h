/* $Id$ */

#ifdef _OSD_POSIX
#ifndef APACHE
#error On this EBCDIC platform, PHP is only supported as an Apache module.
#else /*APACHE*/
#ifndef CHARSET_EBCDIC
#define CHARSET_EBCDIC /* this machine uses EBCDIC, not ASCII! */
#endif
#include "ebcdic.h"
#endif /*APACHE*/
#endif /*_OSD_POSIX*/

char * gdttf(gdImage *im, int *brect, int fg, char *fontname,
    double ptsize, double angle, int x, int y, char *str);

