/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Anatol Belski <ab@php.net>                                  |
   +----------------------------------------------------------------------+
 */

#include <config.w32.h>

#include <win32/time.h>
#include <php.h>

#ifdef HAVE_LIBXML
#include <libxml/threads.h>
#endif

/* TODO this file, or part of it, could be machine generated, to
	allow extensions and SAPIs adding their own init stuff.
	However expected is that MINIT is enough in most cases.
	This file is only useful for some really internal stuff,
	eq. initializing something before the DLL even is
	available to be called. */

BOOL WINAPI DllMain(HINSTANCE inst, DWORD reason, LPVOID dummy)
{
	BOOL ret = TRUE;

	switch (reason)
	{
		case DLL_PROCESS_ATTACH:
			ret = ret && php_win32_init_gettimeofday();
			if (!ret) {
				fprintf(stderr, "gettimeofday() initialization failed");
				return ret;
			}
			break;
#if 0 /* prepared */
		case DLL_PROCESS_DETACH:
			/* pass */
			break;

		case DLL_THREAD_ATTACH:
			/* pass */
			break;

		case DLL_THREAD_DETACH:
			/* pass */
			break;
#endif
	}

#ifdef HAVE_LIBXML
	/* This imply that only LIBXML_STATIC_FOR_DLL is supported ATM. 
		If that changes, this place will need some rework. 
	   TODO Also this should be revisited as no initialization
		might be needed for TS build (libxml build with TLS
		support. */
	ret = ret && xmlDllMain(inst, reason, dummy);
#endif

	return ret;
}

