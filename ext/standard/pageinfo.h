#ifndef PAGEINFO_H
#define PAGEINFO_H

PHP_FUNCTION(getmyuid);
PHP_FUNCTION(getmypid);
PHP_FUNCTION(getmyinode);
PHP_FUNCTION(getlastmod);

extern long php_getuid(void);

#endif
