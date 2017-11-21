#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "php.h"
#include "php_sigsegforce.h"

static zend_function_entry sigseg_functions[] = {
    PHP_FE(force_sigseg, NULL)
    {NULL, NULL, NULL}
};

zend_module_entry sigsegforce_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_SIGSEGFORCE_EXTNAME,
    sigseg_functions,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
#if ZEND_MODULE_API_NO >= 20010901
    PHP_SIGSEGFORCE_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_SIGSEGFORCE
ZEND_GET_MODULE(sigsegforce)
#endif

PHP_FUNCTION(force_sigseg)
{
 	int *ptr = NULL;
 	int *ptrX = NULL;
  *ptr = 1;
  RETURN_STRING("Hello Segmentation Fault", 1);
}
