--TEST--
Return type for internal functions 2

--SKIPIF--
<?php
if (!function_exists('zend_test_func2')) {
    print 'skip';
}

--FILE--
<?php
zend_test_func2();
echo "==DONE==\n"
?>
--EXPECTF--
==DONE==
