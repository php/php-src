#include "config.w32.h"
#if ZTS
# define THREADSAFE 1
#endif
#if !ZEND_DEBUG && !defined(NDEBUG)
# define NDEBUG
#endif
#define SQLITE_PTR_SZ 4