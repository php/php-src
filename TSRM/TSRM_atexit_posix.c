/*
   +----------------------------------------------------------------------+
   | Thread Safe Resource Manager                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1999-2011, Andi Gutmans, Sascha Schumann, Zeev Suraski |
   | This source file is subject to the TSRM license, that is bundled     |
   | with this package in the file LICENSE                                |
   +----------------------------------------------------------------------+
   | Authors:  Marc Henderkes <m@pyc.ac>                                  |
   +----------------------------------------------------------------------+
*/

#include "TSRM_atexit.h"

#if defined(ZTS) && !defined(TSRM_WIN32) && !defined(__APPLE__)

#include <stdio.h>

static bool tsrm_thread_exit_armed = false;
static pthread_key_t tsrm_exit_key;

static void tsrm_thread_exit_handler(void *arg)
{
	if (tsrm_thread_exit_armed) {
		tsrm_free_thread_at_exit(arg);
	}
}

void tsrm_atexit_init(void)
{
	int error = pthread_key_create(&tsrm_exit_key, tsrm_thread_exit_handler);
	tsrm_thread_exit_armed = error == 0;
	if (!tsrm_thread_exit_armed) {
		fprintf(stderr, "TSRM: pthread_key_create() failed (error %d); automatic thread cleanup is disabled\n", error);
	}
}

void tsrm_atexit_set_data(tsrm_tls_entry *thread_resources)
{
	if (!tsrm_thread_exit_armed) {
		return;
	}

	pthread_setspecific(tsrm_exit_key, thread_resources);
}

void tsrm_atexit_disarm(void)
{
	if (!tsrm_thread_exit_armed) {
		return;
	}
	tsrm_thread_exit_armed = false;
	pthread_key_delete(tsrm_exit_key);
}

#ifdef __GNUC__
static void __attribute__((destructor)) tsrm_thread_exit_unload(void)
{
	tsrm_atexit_disarm();
}
#endif

#endif /* ZTS && !TSRM_WIN32 && !__APPLE__ */
