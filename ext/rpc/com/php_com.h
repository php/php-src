#ifndef PHP_COM_H
#define PHP_COM_H

#if WIN32|WINNT

extern PHP_MINIT_FUNCTION(COM);
extern PHP_MSHUTDOWN_FUNCTION(COM);
PHP_FUNCTION(COM_load);
PHP_FUNCTION(COM_invoke);

PHP_FUNCTION(com_propget);
PHP_FUNCTION(com_propput);
extern zend_module_entry COM_module_entry;
#define COM_module_ptr &COM_module_entry

#else

#define COM_module_ptr NULL

#endif  /* Win32|WINNT */

#define phpext_COM_ptr COM_module_ptr

#endif  /* PHP_COM_H */
