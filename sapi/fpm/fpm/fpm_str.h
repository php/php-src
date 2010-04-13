
	/* $Id: fpm_str.h,v 1.3 2008/05/24 17:38:47 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_STR_H
#define FPM_STR_H 1

static inline char *cpystrn(char *dst, const char *src, size_t dst_size) /* {{{ */
{
	char *d, *end;
	
	if (!dst_size) {
		return dst;
	}
	
	d = dst;
	end = dst + dst_size - 1;
	
	for (; d < end; ++d, ++src) {
		if (!(*d = *src)) {
			return d;
		}
	}

	*d = '\0';

	return d;
}
/* }}} */

static inline char *str_purify_filename(char *dst, char *src, size_t size) /* {{{ */
{
	char *d, *end;

	d = dst;
	end = dst + size - 1;

	for (; d < end && *src; ++d, ++src) {
		if (* (unsigned char *) src < ' ' || * (unsigned char *) src > '\x7f') {
			*d = '.';
		} else {
			*d = *src;
		}
	}

	*d = '\0';

	return d;
}
/* }}} */

#endif
