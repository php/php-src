	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#include <sys/types.h>

#include "fpm_trace.h"

zend_result fpm_trace_get_strz(char *buf, size_t sz, long addr) /* {{{ */
{
	int i;
	long l = addr;
	char *lc = (char *) &l;

	i = l % SIZEOF_LONG;
	l -= i;
	for (addr = l; ; addr += SIZEOF_LONG) {
		if (fpm_trace_get_long(addr, &l) != SUCCESS) {
			return FAILURE;
		}
		for ( ; i < SIZEOF_LONG; i++) {
			--sz;
			if (sz && lc[i]) {
				*buf++ = lc[i];
				continue;
			}
			*buf = '\0';
			return SUCCESS;
		}
		i = 0;
	}
}
/* }}} */
