--TEST--
Internal class variance
--EXTENSIONS--
zend_test
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
