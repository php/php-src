--TEST--
zend_call_method_if_exists() shadowing a private method
--EXTENSIONS--
zend_test
--FILE--
<?php

class A {
    private function foo() {
        return 'Not available';
    }
}

class B extends A {
    public function foo() {
        return __METHOD__;
    }
}

$b = new B();

$r = zend_call_method_if_exists($b, 'foo');
var_dump($r);

?>
--EXPECT--
string(6) "B::foo"
