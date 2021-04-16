--TEST--
Return type for internal functions
--EXTENSIONS--
zend_test
--PLATFORM--
# Internal function return types are only checked in debug builds
debug: true
--INI--
opcache.jit=0
--FILE--
<?php
zend_test_array_return();
?>
--EXPECT--
Fatal error: zend_test_array_return(): Return value must be of type array, null returned in Unknown on line 0
