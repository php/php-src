#ifndef PHP3_CRYPT_H
#define PHP3_CRYPT_H

#if HAVE_CRYPT
extern php3_module_entry crypt_module_entry;
#define crypt_module_ptr &crypt_module_entry
PHP_FUNCTION(crypt);
extern PHP_MINIT_FUNCTION(crypt);
#else
#define crypt_module_ptr NULL
#endif

#define phpext_crypt_ptr crypt_module_ptr

#endif
