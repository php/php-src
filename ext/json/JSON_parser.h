/* JSON_checker.h */

#include "php.h"
#include "ext/standard/php_smart_str.h"

static char digits[] = "0123456789abcdef";

extern int JSON_parser(zval *z, unsigned short p[], int length, int assoc TSRMLS_DC);
