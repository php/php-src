#ifndef PHP3_CRYPT_H
#define PHP3_CRYPT_H

#if HAVE_CRYPT
extern php3_module_entry crypt_module_entry;
#define crypt_module_ptr &crypt_module_entry
extern void php3_crypt(INTERNAL_FUNCTION_PARAMETERS);
extern int php_minit_crypt(INIT_FUNC_ARGS);
#else
#define crypt_module_ptr NULL
#endif

#endif
