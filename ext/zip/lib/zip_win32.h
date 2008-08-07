#define _POSIX_
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>
#include <zconf.h>

#ifndef strcasecmp
# define strcmpi _strcmpi
#endif

#ifndef ssize_t
#	define ssize_t SSIZE_T
#endif

#ifndef mode_t
#	define mode_t int
#endif

#ifndef snprintf
#	define snprintf _snprintf
#endif

#ifndef mkstemp
#	define mkstemp(t) _creat(_mktemp(t), _S_IREAD|_S_IWRITE)
#endif
/*
#ifndef fseeko
#	define fseeko fseek
#endif
*/
