/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Anatol Belski <ab@php.net>                                  |
   +----------------------------------------------------------------------+
 */

#include <config.w32.h>

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
