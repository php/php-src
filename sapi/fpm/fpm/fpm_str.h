	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_STR_H
#define FPM_STR_H 1

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
