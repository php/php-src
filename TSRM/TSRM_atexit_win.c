/*
   +----------------------------------------------------------------------+
   | Thread Safe Resource Manager                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1999-2011, Andi Gutmans, Sascha Schumann, Zeev Suraski |
   | This source file is subject to the TSRM license, that is bundled     |
   | with this package in the file LICENSE                                |
   +----------------------------------------------------------------------+
   | Authors:  Zeev Suraski <zeev@php.net>                                |
   +----------------------------------------------------------------------+
*/

#include "TSRM_atexit.h"

#if defined(ZTS) && defined(TSRM_WIN32)

#include <stdlib.h>

TSRMLS_CACHE_EXTERN();

static bool tsrm_thread_exit_armed = false;
static DWORD tsrm_exit_key = FLS_OUT_OF_INDEXES;
static BOOLEAN (NTAPI *tsrm_dll_shutdown_in_progress)(void);

typedef struct {
	tsrm_tls_entry *thread_resources;
} tsrm_thread_exit_data;

static void WINAPI tsrm_thread_exit_handler(void *arg)
{
	tsrm_thread_exit_data *data = arg;
	tsrm_tls_entry *thread_resources;

	if (tsrm_dll_shutdown_in_progress()) {
		return;
	}
	/* FLS callbacks also run when another fiber is deleted. TSRM storage is
	 * thread-wide, so only tear it down for the exiting execution context. */
	if (!tsrm_thread_exit_armed) {
		free(data);
		return;
	}
	if (FlsGetValue(tsrm_exit_key) != data) {
		free(data);
		return;
	}
	FlsSetValue(tsrm_exit_key, NULL);
	thread_resources = data->thread_resources;
	if (thread_resources == TSRMLS_CACHE && thread_resources->thread_exit_data == data) {
		tsrm_free_thread_at_exit(thread_resources);
	}
	free(data);
}

void tsrm_atexit(void)
{
	tsrm_dll_shutdown_in_progress = (BOOLEAN (NTAPI *)(void)) (void *) GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlDllShutdownInProgress");
	/* Use FlsAlloc() because TlsAlloc() does not provide a thread-exit callback
	 * parameter. The callback calls ts_free_thread() if the SAPI has not done so
	 * explicitly, cleaning up EG(persistent_list) and other TSRM resources on the
	 * owning thread before its native TLS is released. */
	tsrm_exit_key = tsrm_dll_shutdown_in_progress ? FlsAlloc(tsrm_thread_exit_handler) : FLS_OUT_OF_INDEXES;
	tsrm_thread_exit_armed = tsrm_exit_key != FLS_OUT_OF_INDEXES;
}

void tsrm_atexit_set_data(tsrm_tls_entry *thread_resources)
{
	tsrm_thread_exit_data *data;

	if (!tsrm_thread_exit_armed) {
		return;
	}

	data = FlsGetValue(tsrm_exit_key);
	if (!thread_resources) {
		if (data && FlsSetValue(tsrm_exit_key, NULL)) {
			free(data);
		}
		return;
	}

	if (!data) {
		data = malloc(sizeof(tsrm_thread_exit_data));
		if (!data) {
			return;
		}
		data->thread_resources = thread_resources;
		if (!FlsSetValue(tsrm_exit_key, data)) {
			free(data);
			return;
		}
		thread_resources->thread_exit_data = data;
		return;
	}
	data->thread_resources = thread_resources;
	thread_resources->thread_exit_data = data;
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
