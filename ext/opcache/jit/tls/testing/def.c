
/* _tsrm_ls_cache is defined here */

#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef NO_SURPLUS
# include "def-vars.h"
DEF_VARS(def);
#endif

__thread void* _tsrm_ls_cache;

size_t tsrm_get_ls_cache_tcb_offset(void) {
	return 0;
}

void zend_accel_error(int type, const char *format, ...) {
	if (type < 4) {
		va_list ap;
		va_start(ap, format);
		vprintf(format, ap);
		va_end(ap);
	}
}

int test(void);

int decl(void) {
	return test();
}

