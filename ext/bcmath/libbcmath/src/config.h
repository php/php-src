#if PHP_WIN32
#include "../../../../main/config.w32.h"
#elif defined(NETWARE)
#include "config.nw.h"
#else
#include "php_config.h"
#endif

#include <php.h>
#include <string.h>
#include "zend.h"
#include "zend_alloc.h"
