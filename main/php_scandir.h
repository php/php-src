#include <sys/types.h>
#include "php_config.h"

#ifdef HAVE_DIRENT_H
# include <dirent.h>
#endif

#ifdef PHP_WIN32
#include "win32/readdir.h"
#endif

#ifndef HAVE_ALPHASORT 
int alphasort(const struct dirent **a, const struct dirent **b);
#endif

#ifndef HAVE_SCANDIR
int scandir(const char *dirname, struct dirent **namelist[], int (*selector) (const struct dirent *entry), int (*compare) (const struct dirent **a, const struct dirent **b));
#endif
