#include <php.h>

#ifndef THREAD_SAFE
static char Win_Error_msg[256];
#endif

PHPAPI char *php_win_err(void)
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
