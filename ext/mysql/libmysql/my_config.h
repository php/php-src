#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php_config.h>

/* PHP might define ulong, but we want to use our own typedef */
#ifdef ulong
#undef ulong
#endif

/* The client doesn't use multiple charsets, so only the compiled-in
   default is really needed */
#define SHAREDIR "NONEXISTENT"
#define DEFAULT_CHARSET_HOME SHAREDIR
