#ifndef PHP_TYPEDEF_VARIANT_H
#define PHP_TYPEDEF_VARIANT_H

#if PHP_WIN32

extern PHP_MINIT_FUNCTION(VARIANT);
extern PHP_MSHUTDOWN_FUNCTION(VARIANT);

extern zend_module_entry VARIANT_module_entry;

#define VARIANT_module_ptr &VARIANT_module_entry

#else

#define VARIANT_module_ptr NULL

#endif  /* PHP_WIN32 */

#define phpext_VARIANT_ptr VARIANT_module_ptr

#endif  /* PHP_TYPEDEF_VARIANT_H */
