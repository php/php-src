#ifndef PHP_CALENDAR_H
#define PHP_CALENDAR_H

extern zend_module_entry calendar_module_entry;
#define	calendar_module_ptr &calendar_module_entry

/* Functions */

PHP_MINIT_FUNCTION(calendar);
PHP_RINIT_FUNCTION(calendar);
PHP_RSHUTDOWN_FUNCTION(calendar);
PHP_MINFO_FUNCTION(calendar);

PHP_FUNCTION(jdtogregorian);
PHP_FUNCTION(gregoriantojd);
PHP_FUNCTION(jdtojulian);
PHP_FUNCTION(juliantojd);
PHP_FUNCTION(jdtojewish);
PHP_FUNCTION(jewishtojd);
PHP_FUNCTION(jdtofrench);
PHP_FUNCTION(frenchtojd);
PHP_FUNCTION(jddayofweek);
PHP_FUNCTION(jdmonthname);
PHP_FUNCTION(easter_days);
PHP_FUNCTION(easter_date);
PHP_FUNCTION(unixtojd);
PHP_FUNCTION(jdtounix);


#define phpext_calendar_ptr calendar_module_ptr

#endif
