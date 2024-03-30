#ifdef _WIN32
# define HAVE_STRICMP 1
#elif defined(HAVE_CONFIG_H)
# include <config.h>
#else
# include <main/php_config.h>
#endif
