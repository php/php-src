/* JSON_checker.h */

#include "php.h"
#include "ext/standard/php_smart_str.h"

extern int JSON_parser(zval *z, unsigned short p[], int length, int assoc TSRMLS_DC);
