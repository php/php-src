#ifndef PHP_SIGSEGFORCE_H
#define PHP_SIGSEGFORCE_H 1
#define PHP_SIGSEGFORCE_VERSION "1.0"
#define PHP_SIGSEGFORCE_EXTNAME "sigsegforce"

PHP_FUNCTION(force_sigseg);

extern zend_module_entry sigsegforce_module_entry;
#define phpext_sigsegforce_ptr &sigsegforce_module_entry

#endif
