/* hack workaround for libfcgi configure */
#ifdef _WIN32
#include "fcgi_config_win32.h"
#else
#include "main/php_config.h"
#endif

