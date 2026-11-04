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

#if defined(ZTS) && defined(TSRM_WIN32)

#include <stdio.h>

static bool tsrm_thread_exit_armed = false;
static DWORD tsrm_exit_key = FLS_OUT_OF_INDEXES;
static BOOLEAN (NTAPI *tsrm_dll_shutdown_in_progress)(void);

static void WINAPI tsrm_thread_exit_handler(void *arg)
{
	if (!tsrm_thread_exit_armed || tsrm_dll_shutdown_in_progress()) {
		return;
	}
	if (IsThreadAFiber()) {
		fprintf(stderr, "TSRM: Native Windows fibers are not supported\n");
		return;
	}
	FlsSetValue(tsrm_exit_key, NULL);
	tsrm_free_thread_at_exit(arg);
}

void tsrm_atexit_init(void)
{
	tsrm_dll_shutdown_in_progress = (BOOLEAN (NTAPI *)(void)) (void *) GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlDllShutdownInProgress");
	/* Use FlsAlloc() because TlsAlloc() does not provide a thread-exit callback
	 * parameter. The callback calls ts_free_thread() if the SAPI has not done so
	 * explicitly, cleaning up EG(persistent_list) and other TSRM resources on the
	 * owning thread before its native TLS is released. */
	tsrm_exit_key = tsrm_dll_shutdown_in_progress ? FlsAlloc(tsrm_thread_exit_handler) : FLS_OUT_OF_INDEXES;
	tsrm_thread_exit_armed = tsrm_exit_key != FLS_OUT_OF_INDEXES;
	if (!tsrm_thread_exit_armed) {
		fprintf(stderr, "TSRM: %s failed (error %lu); automatic thread cleanup is disabled\n",
			tsrm_dll_shutdown_in_progress ? "FlsAlloc()" : "RtlDllShutdownInProgress lookup",
			(unsigned long) GetLastError());
	}
}

void tsrm_atexit_set_data(tsrm_tls_entry *thread_resources)
{
	if (IsThreadAFiber()) {
		fprintf(stderr, "TSRM: Native Windows fibers are not supported\n");
		return;
	}
	if (!tsrm_thread_exit_armed) {
		return;
	}

	FlsSetValue(tsrm_exit_key, thread_resources);
}

void tsrm_atexit_disarm(void)
{
	if (!tsrm_thread_exit_armed) {
		return;
	}
	tsrm_thread_exit_armed = false;
	DWORD key = tsrm_exit_key;
	tsrm_exit_key = FLS_OUT_OF_INDEXES;
	FlsFree(key);
}

#endif /* ZTS && TSRM_WIN32 */
