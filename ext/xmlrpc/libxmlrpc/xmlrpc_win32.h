#ifndef _XMLRPC_WIN32_H
#define _XMLRPC_WIN32_H
/* just some things needed to compile win32 */
#include <windows.h>
#include <stdlib.h>
#define inline __inline
#define snprintf _snprintf
#define strcasecmp(s1, s2) stricmp(s1, s2)


#endif