%HEADER%

#ifndef PHP_%EXTNAMECAPS%_H
# define PHP_%EXTNAMECAPS%_H

extern zend_module_entry %EXTNAME%_module_entry;
# define phpext_%EXTNAME%_ptr &%EXTNAME%_module_entry

# define PHP_%EXTNAMECAPS%_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_%EXTNAMECAPS%)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_%EXTNAMECAPS%_H */
%FOOTER%
