#ifndef PHP_MCRYPT_H
#define PHP_MCRYPT_H

#if HAVE_LIBMCRYPT

extern zend_module_entry mcrypt_module_entry;
#define mcrypt_module_ptr &mcrypt_module_entry

PHP_FUNCTION(mcrypt_ecb);

#else
#define mcrypt_module_ptr NULL
#endif

#endif
