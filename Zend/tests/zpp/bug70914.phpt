--TEST--
Bug #70914 zend_throw_or_error() format string vulnerability
--EXTENSIONS--
zend_test
--FILE--
<?php

try {
    var_dump(zend_class_name('%Z'));
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, "%Z" given
