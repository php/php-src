#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "winutil.h"

#ifndef THREAD_SAFE
static char Win_Error_msg[256];
#endif

char *php_win_err(void)
{
	FormatMessage(
					 FORMAT_MESSAGE_FROM_SYSTEM,
					 NULL,
					 GetLastError(),
					 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),	/* Default language */
					  (LPTSTR) Win_Error_msg,
					 256,
					 NULL);

	return Win_Error_msg;
}
