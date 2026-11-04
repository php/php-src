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

#if defined(ZTS) && defined(__APPLE__)

#include <mach-o/dyld.h>
#include <stdio.h>

static bool tsrm_thread_exit_armed = false;
static pthread_key_t tsrm_exit_key;
static TSRM_TLS bool tsrm_thread_exit_registered = false;

static void tsrm_thread_exit_handler(void *arg)
{
	tsrm_thread_exit_registered = false;
	if (!tsrm_thread_exit_armed) {
		return;
	}
	tsrm_free_thread_at_exit(pthread_getspecific(tsrm_exit_key));
}

void tsrm_atexit_init(void)
{
	int error = pthread_key_create(&tsrm_exit_key, NULL);
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

	if (pthread_setspecific(tsrm_exit_key, thread_resources) != 0) {
		return;
	}
	if (thread_resources && !tsrm_thread_exit_registered) {
		tsrm_thread_exit_registered = true;
		_tlv_atexit(tsrm_thread_exit_handler, NULL);
	}
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

#endif /* ZTS && __APPLE__ */
