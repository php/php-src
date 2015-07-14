typedef zend_bool (*php_iterator_each_t)(zval *value, zval *key, void *context);

#define PHP_TRAVERSE_MODE_VAL     2
#define PHP_TRAVERSE_MODE_KEY_VAL 3

#define PHP_TRAVERSE_CONTEXT_STANDARD_MEMBERS() \
	zval *traversable; \
	zend_fcall_info fci; \
	zend_fcall_info_cache fci_cache;

PHPAPI void php_traverse(zval *t, php_iterator_each_t each_func, int traverse_mode, void *context);
