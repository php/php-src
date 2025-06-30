#include <stdio.h>
#include <dlfcn.h>

#ifdef NO_SURPLUS
# include "def-vars.h"
DEF_VARS(main);
#endif

__thread int some_tls_var;

#ifndef DL_DECL
int decl(void);
#endif

int main(void) {
	/* Ensure TLS vars are allocated */
	some_tls_var = 1;

	int (*decl_p)(void);
#ifdef DL_DECL
	int flags = RTLD_LAZY | RTLD_GLOBAL;
# ifdef RTLD_DEEPBIND
	flags |= RTLD_DEEPBIND;
# endif
	void *handle = dlopen("./libdef.so", flags);
	if (!handle) {
		fprintf(stderr, "dlopen: %s\n", dlerror());
		return 1;
	}

	decl_p = (int (*)(void)) dlsym(handle, "decl");
	if (!decl_p) {
		fprintf(stderr, "dlsym: %s\n", dlerror());
		return 1;
	}
#else
	decl_p = decl;
#endif

	int ret = decl_p();
	if (!ret) {
		fprintf(stderr, "FAIL\n");
	} else {
		fprintf(stderr, "OK\n");
	}

	return !ret;
}
