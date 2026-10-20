--TEST--
GH-22175: zend_call_method_if_exists() must unref return value
--CREDITS--
YuanchengJiang
--EXTENSIONS--
zend_test
--FILE--
<?php

class Foo {
    public function &test() {}
}

zend_call_method_if_exists(new Foo, 'test');

?>
--EXPECTF--
Notice: Only variable references should be returned by reference in %s on line %d
