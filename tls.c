/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors:                                                             |
   |                                                                      |
   +----------------------------------------------------------------------+
 */
#include "tls.h"

#if THREAD_SAFE
/*This is indeed a global!*/
DWORD TlsIndex;
#else
php3_globals_struct *php3_globals; 
#endif

/* just copying from some samples, can replace this with
standard php error messaging */
VOID ErrorExit (LPTSTR lpszMessage) 
{ 
   fprintf(stderr, "%s\n", lpszMessage); 
   ExitProcess(0); 
} 

/*all these functions are called from DllMain()
  in a NON thread safe version, tls_startup and
  tls_shutdown should be initiated before anything
  else.  This will allocate memory for globals.

  called at dll load*/
int tls_startup(void){
#if THREAD_SAFE
	if ((TlsIndex=TlsAlloc())==0xFFFFFFFF){
		return 0;
	}
#else
	php3_globals=malloc(sizeof(php3_globals));
#endif
	return 1;
}
/*called at dll unload*/
int tls_shutdown(void){
#if THREAD_SAFE
	if (!TlsFree(TlsIndex)){
		return 0;
	}
#else
	free(php3_globals);
#endif
	return 1;
}
 
#if THREAD_SAFE
/*called at start of thread*/
int tls_create(void){
	php3_globals_struct *php3_globals;
	php3_globals = (php3_globals_struct *) LocalAlloc(LPTR, sizeof(php3_globals_struct)); 
	if (! TlsSetValue(TlsIndex, (void *) php3_globals)) 
		ErrorExit("TlsSetValue error"); 

	return 1;
}

/*called at end of thread*/
int tls_destroy(void){
	php3_globals_struct *php3_globals;
	php3_globals = TlsGetValue(TlsIndex); 
	if (php3_globals != 0) 
		LocalFree((HLOCAL) php3_globals); 
	return 1;
}

#endif


/*
accessing data inside a thread
This short function shows how the global struct
is accessed in a function.  THREAD_SAFE should
only need to be defined on windows server modules

void thread_safe_access_of_globals(VOID) 
{ 
	TLS_VARS;

	if ((php3_globals == 0) && (GetLastError() != 0)) {
		ErrorExit("TlsGetValue error");
	}
} 

*/
