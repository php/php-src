--TEST--
Return type for internal functions

--SKIPIF--
<?php
if (!function_exists('zend_test_func')) {
    print 'skip';
}

--FILE--
<?php
zend_test_func();
?>
--EXPECTF--
Fatal error: Return value of zend_test_func() must be of the type array, null returned in %s on line %d
