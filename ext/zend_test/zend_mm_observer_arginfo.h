/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 167191499ef7b4e3517478e7e9298a62364aafb2 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_memprof_enable, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_memprof_disable arginfo_memprof_enable


static ZEND_FUNCTION(memprof_enable);
static ZEND_FUNCTION(memprof_disable);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(memprof_enable, arginfo_memprof_enable)
	ZEND_FE(memprof_disable, arginfo_memprof_disable)
	ZEND_FE_END
};
