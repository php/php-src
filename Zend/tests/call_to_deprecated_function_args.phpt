--TEST--
Check that arguments are freed when calling a deprecated function
--EXTENSIONS--
zend_test
--FILE--
<?php

set_error_handler(function($code, $msg) {
    throw new Error($msg);
});

try {
    zend_test_deprecated(new stdClass);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$ret = new stdClass;
try {
    $ret = zend_test_deprecated(new stdClass());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $fn = 'zend_test_deprecated';
    $fn(new stdClass);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$ret = new stdClass;
try {
    $fn = 'zend_test_deprecated';
    $ret = $fn(new stdClass);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Function zend_test_deprecated() is deprecated
Error: Function zend_test_deprecated() is deprecated
Error: Function zend_test_deprecated() is deprecated
Error: Function zend_test_deprecated() is deprecated
