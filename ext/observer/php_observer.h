/* observer extension for PHP */

#ifndef PHP_OBSERVER_H
# define PHP_OBSERVER_H

extern zend_module_entry observer_module_entry;
# define phpext_observer_ptr &observer_module_entry

# define PHP_OBSERVER_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_OBSERVER)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_OBSERVER_H */
