/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: a715bf6a8d5fe69732623cc17f03bd463f369648 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Directory_close, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Directory_rewind arginfo_class_Directory_close

#define arginfo_class_Directory_read arginfo_class_Directory_close


ZEND_FUNCTION(closedir);
ZEND_FUNCTION(rewinddir);
ZEND_FUNCTION(readdir);


static const zend_function_entry class_Directory_methods[] = {
	ZEND_ME_MAPPING(close, closedir, arginfo_class_Directory_close, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(rewind, rewinddir, arginfo_class_Directory_rewind, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(read, readdir, arginfo_class_Directory_read, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
