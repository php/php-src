#if _MSC_VERS <= 1300
#include "php.h"
#include "php_stdint.h"

PHPAPI int64_t _strtoi64(const char *nptr, char **endptr, int base);
#define _strtoui64 _strtoi64
#endif
