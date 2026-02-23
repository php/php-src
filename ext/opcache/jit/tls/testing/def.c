
/* _tsrm_ls_cache is defined here */

#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#if __STDC_NO_THREADS__
# define thread_local _Thread_local
#else
# include <threads.h>
#endif

#ifdef NO_SURPLUS
# include "def-vars.h"
DEF_VARS(def);
#endif

thread_local void* _tsrm_ls_cache;

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

