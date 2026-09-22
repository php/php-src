--TEST--
zend_call_method_if_exists() with non existing method
--EXTENSIONS--
zend_test
--FILE--
<?php

class A {
    public function foo() {
        return __METHOD__;
    }
}

$a = new A();

$r = zend_call_method_if_exists($a, 'bar');
var_dump($r);

?>
--EXPECT--
NULL
