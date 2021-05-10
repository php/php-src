--TEST--
Internal class variance
--SKIPIF--
<?php if (!extension_loaded('zend_test')) die('skip requires zend_test'); ?>
--FILE--
<?php
$test = new _ZendTestChildClass;

try {
    $test->returnsThrowable();
} catch (\Error) {
    echo "OK";
}
?>
--EXPECT--
OK
