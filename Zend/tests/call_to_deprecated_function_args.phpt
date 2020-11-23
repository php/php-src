--TEST--
Check that arguments are freed when calling a deprecated function
--SKIPIF--
<?php
if (!extension_loaded('zend-test')) die('skip zend-test extension not loaded');
--FILE--
<?php

set_error_handler(function($code, $msg) {
    throw new Error($msg);
});

try {
    zend_test_deprecated(new stdClass);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$ret = new stdClass;
try {
    $ret = zend_test_deprecated(new stdClass());
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    $fn = 'zend_test_deprecated';
    $fn(new stdClass);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$ret = new stdClass;
try {
    $fn = 'zend_test_deprecated';
    $ret = $fn(new stdClass);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Function zend_test_deprecated() is deprecated
Function zend_test_deprecated() is deprecated
Function zend_test_deprecated() is deprecated
Function zend_test_deprecated() is deprecated
