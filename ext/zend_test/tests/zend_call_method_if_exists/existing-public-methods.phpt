--TEST--
zend_call_method_if_exists() with existing public method
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

$r = zend_call_method_if_exists($a, 'foo');
var_dump($r);

?>
--EXPECT--
string(6) "A::foo"
